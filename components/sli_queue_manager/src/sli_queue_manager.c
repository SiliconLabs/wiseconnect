/***************************************************************************/ /**
 * @file sli_queue_manager.c
 * @brief
 *******************************************************************************
 * # License
 * <b>Copyright 2024 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/
#include <stdint.h>
#include <assert.h>
#include "sli_queue_manager_types.h"
#include "sl_core.h"

sl_status_t sli_queue_manager_init(sli_queue_t *handle)
{
  if (NULL == handle) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  handle->head = NULL;
  handle->tail = NULL;

  return SL_STATUS_OK;
}

sl_status_t sli_queue_manager_enqueue(sli_queue_t *handle, sl_slist_node_t *node)
{
  if (NULL == handle) {
    return SL_STATUS_INVALID_PARAMETER;
  }
  CORE_irqState_t state = CORE_EnterAtomic();
  node->node            = NULL;
  if (handle->tail == NULL) {
    assert(handle->head == NULL); // Both should be NULL at the same time
    handle->head = node;
    handle->tail = node;
  } else {
    handle->tail->node = node;
    handle->tail       = node;
  }

  CORE_ExitAtomic(state);

  return SL_STATUS_OK;
}

sl_status_t sli_queue_manager_add_to_queue_head(sli_queue_t *handle, sl_slist_node_t *node)
{
  if (NULL == handle) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  CORE_irqState_t state = CORE_EnterAtomic();

  if (handle->head == NULL) {
    assert(handle->tail == NULL); // Both should be NULL at the same time
    handle->head = node;
    handle->tail = node;
  } else {
    node->node   = handle->head;
    handle->head = node;
  }

  CORE_ExitAtomic(state);

  return SL_STATUS_OK;
}

sl_status_t sli_queue_manager_dequeue(sli_queue_t *handle, sl_slist_node_t **node)
{
  if (NULL == handle) {
    return SL_STATUS_INVALID_PARAMETER;
  }
  sl_status_t status    = SL_STATUS_EMPTY;
  CORE_irqState_t state = CORE_EnterAtomic();
  if (handle->head == NULL) {
    assert(handle->tail == NULL); // Both should be NULL at the same time
    *node  = NULL;
    status = SL_STATUS_EMPTY;
  } else {
    *node  = handle->head;
    status = SL_STATUS_OK;
    if (handle->head == handle->tail) {
      handle->head = NULL;
      handle->tail = NULL;
    } else {
      handle->head = handle->head->node;
    }

    (*node)->node = NULL;
  }
  CORE_ExitAtomic(state);
  return status;
}

sl_status_t sli_queue_manager_remove_node_from_queue(sli_queue_t *handle,
                                                     sli_queue_manager_node_match_handler_t id_handler,
                                                     void *node_match_data,
                                                     sl_slist_node_t **node)
{
  if (NULL == handle) {
    return SL_STATUS_INVALID_PARAMETER;
  }
  if (NULL == node) {
    return SL_STATUS_INVALID_PARAMETER;
  }
  sl_status_t status    = SL_STATUS_EMPTY;
  sl_slist_node_t *prev = NULL;
  CORE_irqState_t state = CORE_EnterAtomic();

  if (NULL == id_handler) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  *node = NULL;

  if (handle->head == NULL) {
    assert(handle->tail == NULL); // Both should be NULL at the same time
    CORE_ExitAtomic(state);
    return SL_STATUS_EMPTY;
  }

  prev  = NULL;
  *node = handle->head;
  while (*node) {
    if (true == id_handler(handle, *node, node_match_data)) {
      status = SL_STATUS_OK;
      break;
    }

    prev  = *node;
    *node = (*node)->node;
  }
  if (*node) {
    if ((handle->head == *node) && (handle->head == handle->tail)) {
      handle->head = NULL;
      handle->tail = NULL;
    } else if (handle->head == *node) {
      handle->head = (*node)->node;
    } else if (handle->tail == *node) {
      handle->tail = prev;
    } else {
      prev->node = (*node)->node;
    }

    (*node)->node = NULL;
  }
  CORE_ExitAtomic(state);
  return status;
}

sl_status_t sli_queue_manager_flush_nodes_from_queue(sli_queue_t *handle,
                                                     sli_queue_manager_node_match_handler_t id_handler,
                                                     void *node_match_data,
                                                     sli_queue_manager_flush_handler_t flush_handler)
{
  if (NULL == handle) {
    return SL_STATUS_INVALID_PARAMETER;
  }
  sl_slist_node_t *node    = NULL;
  sl_slist_node_t *prev    = NULL;
  sl_slist_node_t *element = NULL;

  if (NULL == id_handler) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  CORE_irqState_t state = CORE_EnterAtomic();
  if (handle->head == NULL) {
    assert(handle->tail == NULL); // Both should be NULL at the same time
    CORE_ExitAtomic(state);
    return SL_STATUS_OK;
  }

  prev = NULL;
  node = handle->head;

  while (NULL != node) {
    if (true == id_handler(handle, node, node_match_data)) {
      element = node;

      // Remove the matched packet from the queue
      if (NULL == prev) {
        handle->head = node->node->node;
        node         = handle->head;
      } else {
        prev->node = element->node;
        node       = element->node;
      }

      if (handle->tail == element) {
        handle->tail = prev;
      }

      element->node = NULL;
    } else {
      prev = node;
      node = node->node;
    }

    if ((element != NULL) && (flush_handler != NULL)) {
      // Free the matched packet if required by the user
      flush_handler(handle, element, node_match_data);
      element = NULL;
    }
  }

  if (NULL == handle->head) {
    handle->tail = NULL;
  }

  CORE_ExitAtomic(state);
  return SL_STATUS_OK;
}

sl_status_t sli_queue_manager_flush_queue(sli_queue_t *handle, sli_queue_manager_flush_handler_t flush_handler)
{
  if (NULL == handle) {
    return SL_STATUS_INVALID_PARAMETER;
  }
  sl_slist_node_t *node = NULL;
  CORE_irqState_t state = CORE_EnterAtomic();
  if (handle->head == NULL) {
    assert(handle->tail == NULL); // Both should be NULL at the same time
    CORE_ExitAtomic(state);
    return SL_STATUS_OK;
  }

  while (handle->head) {
    node = handle->head;

    if (handle->head == handle->tail) {
      handle->head = NULL;
      handle->tail = NULL;
    } else {
      handle->head = handle->head->node;
    }

    if (flush_handler) {
      flush_handler(handle, node, NULL);
    }
  }

  CORE_ExitAtomic(state);
  return SL_STATUS_OK;
}

sl_status_t sli_queue_manager_deinit(sli_queue_t *handle, sli_queue_manager_flush_handler_t flush_handler)
{
  return sli_queue_manager_flush_queue(handle, flush_handler);
}
