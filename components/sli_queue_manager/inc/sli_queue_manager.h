/***************************************************************************/ /**
 * @file sli_queue_manager.h
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
#ifndef SLI_QUEUE_MANAGER_H
#define SLI_QUEUE_MANAGER_H
#include "sli_queue_manager_types.h"

/**
 * @brief Initialize the queue manager instance.
 * @brief This creates an array of queue handles of length indicated by queue_count in the instance pointer provided by the user.
 *
 * @param queue Queue Handle.
 * @return sl_status_t
 */
sl_status_t sli_queue_manager_init(sli_queue_t *handle);

/**
 * @brief Enqueue a packet to the queue.
 *
 * @param handle Queue Handle.
 * @param node Packet which is to be added in to the queue. 
 * @return sl_status_t
 */
sl_status_t sli_queue_manager_enqueue(sli_queue_t *handle, sl_slist_node_t *node);

/**
 * @brief Add a packet to the head of the queue identified by queue_id in the given instance.
 * 
 * @param handle Queue Handle.
 * @param node Packet which is to be added in to the queue.
 * @return sl_status_t
 */
sl_status_t sli_queue_manager_add_to_queue_head(sli_queue_t *handle, sl_slist_node_t *node);

/**
 * @brief Dequeue a packet from the queue identified by queue_id in the given instance.
 * 
 * @param handle Queue Handle.
 * @param node Pointer to hold packet which is to be removed from the queue.
 * @return sl_status_t
 */
sl_status_t sli_queue_manager_dequeue(sli_queue_t *handle, sl_slist_node_t **node);

/**
 * @brief Remove a packet from the queue identified by id_handler.
 * 
 * @param handle Queue Handle.
 * @param id_handler Pointer to function which can identify the target node in the queue.
 * @param node_match_data User specific pointer which can be passed to id_handler.
 * @param node Pointer to hold packet which is to be removed from the queue. 
 * @return sl_status_t
 */
sl_status_t sli_queue_manager_remove_node_from_queue(sli_queue_t *handle,
                                                     sli_queue_manager_node_match_handler_t id_handler,
                                                     void *node_match_data,
                                                     sl_slist_node_t **node);

/**
 * @brief Remove all packets from the queue identified by id_handler.
 * 
 * @param handle Queue Handle.
 * @param id_handler Pointer to function which can identify the target node in the queue.
 * @param node_match_data User specific pointer which can be passed to id_handler and flush_handler. 
 * @param flush_handler Pointer to function which can free the target node in the queue.   
 * @return sl_status_t
 */
sl_status_t sli_queue_manager_flush_nodes_from_queue(sli_queue_t *handle,
                                                     sli_queue_manager_node_match_handler_t id_handler,
                                                     void *node_match_data,
                                                     sli_queue_manager_flush_handler_t flush_handler);

/**
 * @brief Flush all the nodes present in the queue.
 * 
 * @param handle Queue Handle.
 * @param flush_handler Pointer to function which can free the all nodes in the queue.   
 * @return sl_status_t
 */
sl_status_t sli_queue_manager_flush_queue(sli_queue_t *handle, sli_queue_manager_flush_handler_t flush_handler);

/**
 * @brief De-Initialize the queue manager.
 * 
 * @param handle Queue Handle.
 * @param flush_handler Pointer to function which can free the all nodes in the queue. 
 * @return sl_status_t
 */
sl_status_t sli_queue_manager_deinit(sli_queue_t *handle, sli_queue_manager_flush_handler_t flush_handler);

#endif // SLI_QUEUE_MANAGER_H