/***************************************************************************/ /**
 * @file sl_si91x_freertos_heap_4_wrap.c
 * @brief Allocator shim: linker --wrap redirects malloc/free/calloc/realloc
 *        (and newlib _*_r, aligned_alloc/memalign/posix_memalign) to this
 *        file so all dynamic allocation uses the FreeRTOS heap (pvPortMalloc/
 *        vPortFree) and the firmware shares a single heap.
 *
 *******************************************************************************
 * # License
 * <b>Copyright 2026 Silicon Laboratories Inc. www.silabs.com</b>
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

/**
 * @details
 * The component sl_si91x_freertos_heap_4_redirect adds GCC linker options
 * -Wl,--wrap=malloc, --wrap=free, etc. References to malloc() are then
 * resolved to __wrap_malloc(); the real symbol is available as __real_malloc()
 * (unused here). All wrappers allocate via pvPortMalloc/vPortFree so the
 * application and libraries share one heap (heap_4).
 *
 * Block layouts:
 * - Normal (malloc/calloc/realloc result): [NORMAL_BLOCK_MAGIC][size] then
 *   user payload. SIZE_HEADER = 16 bytes so the user pointer is 8-byte
 *   aligned and (ptr - ALIGNMENT_OVERHEAD) stays inside our block for
 *   type detection in free/realloc.
 * - Aligned (aligned_alloc/memalign/posix_memalign): [size][ALIGNED_ALLOC_MAGIC]
 *   [block_ptr] immediately before the aligned user address, so free/realloc
 *   can distinguish aligned vs normal and realloc can read the original size.
 */

#include <stddef.h>   /* size_t, NULL */
#include <stdint.h>   /* uintptr_t, SIZE_MAX */
#include <string.h>   /* memcpy, memset */
#include <errno.h>    /* EINVAL, ENOMEM for posix_memalign */
#include "FreeRTOS.h" /* pvPortMalloc, vPortFree */
#if defined(__GNUC__) && defined(__arm__)
#include <reent.h> /* struct _reent and _errno for setting errno on OOM in _*_r */
#endif

/*------------------------------------------------------------------------------
 * Aligned allocation: store size + magic + block pointer before aligned addr so
 * __wrap_free can detect and free the correct block and realloc can copy.
 *------------------------------------------------------------------------------*/
#define ALIGNMENT_OVERHEAD     (2 * sizeof(void *))   /* magic + block ptr (for detection) */
#define ALIGNED_BLOCK_OVERHEAD (3 * sizeof(void *))   /* size + magic + block ptr */
#define ALIGNED_ALLOC_MAGIC    ((void *)0xE1A11E00UL) /* identifies aligned block at (ptr - 16) */

/*------------------------------------------------------------------------------
 * Normal (non-aligned) blocks: header must be >= ALIGNMENT_OVERHEAD so that
 * when we read (ptr - ALIGNMENT_OVERHEAD) to detect aligned vs normal we never
 * read before our allocation. User pointer is then 8-byte aligned for ARM.
 * Layout: [NORMAL_BLOCK_MAGIC][size] = SIZE_HEADER bytes, then user payload.
 *------------------------------------------------------------------------------*/
#define SIZE_HEADER        ALIGNMENT_OVERHEAD
#define NORMAL_BLOCK_MAGIC ((void *)0x4E524D42UL) /* "NRMB" in hex; distinguishes from aligned at (ptr-16) */

/***************************************************************************/ /**
 * Allocate a block with a user address aligned to @p alignment (power of two).
 * Stores size + magic + block pointer before the aligned address for free/realloc.
 *
 * @param alignment  Requested alignment (must be power of two, > 0).
 * @param size       Number of bytes to allocate.
 * @return  Aligned pointer to the allocated memory, or NULL on failure/invalid args.
 ******************************************************************************/
static void *aligned_alloc_impl(size_t alignment, size_t size)
{
  /* Reject alignment 0 or non-power-of-two (alignment & (alignment-1)) is 0 iff power of two */
  if (alignment == 0 || (alignment & (alignment - 1)) != 0) {
    return NULL;
  }
  /* Reject zero-size request */
  if (size == 0) {
    return NULL;
  }
  /* Reject if size + (alignment-1) + OVERHEAD would overflow SIZE_MAX */
  if (size > (SIZE_MAX - (alignment - 1) - ALIGNED_BLOCK_OVERHEAD)) {
    return NULL;
  }
  /* Total bytes: user size + padding for alignment + our header (size+magic+block ptr) */
  size_t alloc_size = size + alignment - 1 + ALIGNED_BLOCK_OVERHEAD;
  void *block       = pvPortMalloc(alloc_size); /* allocate from FreeRTOS heap */
  if (block == NULL) {
    errno = ENOMEM;
    return NULL;
  }
  /* First possible aligned address after block + OVERHEAD; round up to alignment */
  uintptr_t base                                  = (uintptr_t)block + ALIGNED_BLOCK_OVERHEAD;
  uintptr_t aligned                               = (base + alignment - 1) & ~(uintptr_t)(alignment - 1);
  *((size_t *)(aligned - ALIGNED_BLOCK_OVERHEAD)) = size;                /* store size for realloc */
  *((void **)(aligned - ALIGNMENT_OVERHEAD))      = ALIGNED_ALLOC_MAGIC; /* mark as aligned block */
  *((void **)(aligned - sizeof(void *)))          = block;               /* store block for vPortFree */
  return (void *)aligned;                                                /* return aligned user pointer */
}

/***************************************************************************/ /**
 * Free a block that was allocated by aligned_alloc_impl.
 * The block pointer is stored at (ptr - sizeof(void *)).
 *
 * @param ptr  User pointer previously returned by aligned_alloc_impl (may be NULL).
 ******************************************************************************/
static void aligned_free_impl(void *ptr)
{
  if (ptr == NULL) {
    return; /* free(NULL) is no-op */
  }
  /* Recover block pointer stored at (ptr - 8) by aligned_alloc_impl */
  void *block = *((void **)((uintptr_t)ptr - sizeof(void *)));
  vPortFree(block); /* return block to FreeRTOS heap */
}

/*------------------------------------------------------------------------------
 * Standard C allocators → FreeRTOS heap (with size header for realloc safety)
 *------------------------------------------------------------------------------*/

/***************************************************************************/ /**
 * Wrapper for malloc(): allocates via pvPortMalloc with a 16-byte header
 * (magic + size) so realloc can copy the minimum of old and new size.
 *
 * @param size  Number of bytes to allocate.
 * @return  Pointer to allocated memory, or NULL on failure/overflow.
 ******************************************************************************/
void *__wrap_malloc(size_t size)
{
  /* Reject if size + SIZE_HEADER would overflow (e.g. on 32-bit) */
  if (size > (SIZE_MAX - SIZE_HEADER)) {
    errno = ENOMEM;
    return NULL;
  }
  /* Allocate header + user payload from FreeRTOS heap */
  void *block = pvPortMalloc(size + SIZE_HEADER);
  if (block == NULL) {
    errno = ENOMEM;
    return NULL;
  }
  *((void **)block)                             = NORMAL_BLOCK_MAGIC; /* mark as normal block */
  *((size_t *)((char *)block + sizeof(void *))) = size;               /* store size for realloc */
  return (char *)block + SIZE_HEADER;                                 /* return pointer to user payload */
}

/***************************************************************************/ /**
 * Wrapper for free(): detects normal vs aligned block by reading magic at
 * (ptr - ALIGNMENT_OVERHEAD), then frees the underlying pvPortMalloc block.
 *
 * @param ptr  Pointer previously returned by malloc/calloc/realloc or
 *             aligned_alloc/memalign/posix_memalign (NULL is a no-op).
 ******************************************************************************/
void __wrap_free(void *ptr)
{
  if (ptr == NULL) {
    return; /* free(NULL) is no-op */
  }
  /* Read magic at (ptr - 16): inside our block for both normal and aligned layouts */
  void *magic = *((void **)((uintptr_t)ptr - ALIGNMENT_OVERHEAD));
  if (magic == ALIGNED_ALLOC_MAGIC) {
    aligned_free_impl(ptr); /* aligned block: free via stored block pointer */
  } else {
    void *block = (char *)ptr - SIZE_HEADER; /* normal block: header is SIZE_HEADER bytes */
    vPortFree(block);                        /* return block to FreeRTOS heap */
  }
}

/***************************************************************************/ /**
 * Wrapper for calloc(): allocates (nmemb * size) bytes via malloc and zeroes
 * the block. Overflow check ensures nmemb * size does not wrap.
 *
 * @param nmemb  Number of elements.
 * @param size   Size of each element.
 * @return  Pointer to zeroed memory, or NULL on failure/overflow.
 ******************************************************************************/
void *__wrap_calloc(size_t nmemb, size_t size)
{
  size_t total = nmemb * size; /* total bytes requested */
  /* Overflow check: if nmemb*size wrapped, then total/nmemb != size */
  if (nmemb != 0 && total / nmemb != size) {
    errno = ENOMEM;
    return NULL;
  }
  /* Reject if total + SIZE_HEADER would overflow */
  if (total > (SIZE_MAX - SIZE_HEADER)) {
    errno = ENOMEM;
    return NULL;
  }
  void *block = pvPortMalloc(total + SIZE_HEADER); /* allocate from FreeRTOS heap */
  if (block == NULL) {
    errno = ENOMEM;
    return NULL;
  }
  *((void **)block)                             = NORMAL_BLOCK_MAGIC;          /* mark as normal block */
  *((size_t *)((char *)block + sizeof(void *))) = total;                       /* store total for realloc */
  void *p                                       = (char *)block + SIZE_HEADER; /* user payload start */
  memset(p, 0, total);                                                         /* zero memory as required by calloc */
  return p;
}

/***************************************************************************/ /**
 * Wrapper for realloc(): supports both normal (malloc/calloc) and aligned
 * pointers. Allocates a new block of @p size bytes, copies min(old_size, size)
 * bytes to avoid over-read, frees the old block, and returns the new pointer.
 * For aligned pointers the new block is from malloc (alignment not preserved).
 *
 * @param ptr   Pointer to reallocate (from malloc/calloc/realloc or
 *              aligned_alloc/memalign/posix_memalign), or NULL to malloc(size).
 * @param size  New size in bytes.
 * @return  New pointer, or NULL on failure (old block is unchanged on failure).
 ******************************************************************************/
void *__wrap_realloc(void *ptr, size_t size)
{
  if (ptr == NULL) {
    return __wrap_malloc(size); /* realloc(NULL, size) equivalent to malloc(size) */
  }
  if (size == 0) {
    __wrap_free(ptr); /* realloc(ptr, 0) frees and returns NULL */
    return NULL;
  }
  /* Read magic at (ptr - 16) to distinguish aligned vs normal block */
  void *magic = *((void **)((uintptr_t)ptr - ALIGNMENT_OVERHEAD));
  size_t old_size;
  if (magic == ALIGNED_ALLOC_MAGIC) {
    old_size = *((size_t *)((uintptr_t)ptr - ALIGNED_BLOCK_OVERHEAD)); /* size stored at ptr-24 */
  } else {
    old_size = *((size_t *)((char *)ptr - sizeof(void *))); /* size at second word of normal header */
  }
  /* Copy min(old, new) bytes to avoid reading past end of old allocation */
  size_t copy_size = (old_size < size) ? old_size : size;
  void *new_ptr    = __wrap_malloc(size); /* allocate new block (normal; alignment not preserved for aligned ptr) */
  if (new_ptr == NULL) {
    errno = ENOMEM; /* leave old block unchanged on failure */
    return NULL;
  }
  memcpy(new_ptr, ptr, copy_size); /* copy existing data */
  __wrap_free(ptr);                /* free old block (normal or aligned) */
  return new_ptr;
}

/*------------------------------------------------------------------------------
 * Aligned allocation API wrappers
 *------------------------------------------------------------------------------*/

/***************************************************************************/ /**
 * Wrapper for C11 aligned_alloc(): alignment and size validated before calling
 * aligned_alloc_impl. Alignment must be power of two; size must be a multiple
 * of alignment when size != 0.
 *
 * @param alignment  Requested alignment (power of two).
 * @param size       Number of bytes (must be multiple of alignment if non-zero).
 * @return  Aligned pointer, or NULL on failure/invalid args.
 ******************************************************************************/
void *__wrap_aligned_alloc(size_t alignment, size_t size)
{
  /* Check alignment first so size % alignment is safe (no divide-by-zero) */
  if (alignment == 0 || (alignment & (alignment - 1)) != 0) {
    return NULL; /* alignment must be power of two */
  }
  /* C11: size must be a multiple of alignment when size != 0 */
  if (size != 0 && (size % alignment) != 0) {
    return NULL;
  }
  return aligned_alloc_impl(alignment, size); /* do the actual aligned allocation */
}

/***************************************************************************/ /**
 * Wrapper for memalign(): standard memalign only requires alignment to be
 * a power of two (no minimum). Delegates to aligned_alloc_impl.
 *
 * @param alignment  Requested alignment (power of two).
 * @param size       Number of bytes to allocate.
 * @return  Aligned pointer, or NULL on failure/invalid alignment.
 ******************************************************************************/
void *__wrap_memalign(size_t alignment, size_t size)
{
  /* Reject alignment 0 or non-power-of-two; no sizeof(void *) minimum (that is posix_memalign). */
  if (alignment == 0 || (alignment & (alignment - 1)) != 0) {
    errno = EINVAL;
    return NULL;
  }
  return aligned_alloc_impl(alignment, size); /* memalign has no size multiple rule */
}

/***************************************************************************/ /**
 * Wrapper for posix_memalign(): returns 0 on success, EINVAL for invalid
 * alignment (e.g. 0 or not a multiple of sizeof(void *) or not power of two),
 * ENOMEM on allocation failure. Size 0 is valid and sets *memptr = NULL.
 *
 * @param memptr     Output: receives the allocated pointer (or NULL if size 0).
 * @param alignment  Requested alignment (power of two, >= sizeof(void *)).
 * @param size       Number of bytes to allocate.
 * @return  0 on success, EINVAL or ENOMEM on error.
 ******************************************************************************/
int __wrap_posix_memalign(void **memptr, size_t alignment, size_t size)
{
  if (memptr == NULL) {
    return EINVAL; /* NULL output pointer is invalid */
  }
  /* Reject alignment 0 explicitly (0 & (0-1) is 0, so power-of-two check would pass) */
  if (alignment == 0) {
    return EINVAL;
  }
  /* Alignment must be multiple of sizeof(void *) and a power of two */
  if ((alignment % sizeof(void *)) != 0 || (alignment & (alignment - 1)) != 0) {
    return EINVAL;
  }
  /* POSIX allows size 0: success with *memptr = NULL */
  if (size == 0) {
    *memptr = NULL;
    return 0;
  }
  void *p = aligned_alloc_impl(alignment, size); /* attempt allocation */
  if (p == NULL) {
    return ENOMEM; /* allocation failed; POSIX reports via return value, not errno */
  }
  *memptr = p; /* store result and return success */
  return 0;
}

/*------------------------------------------------------------------------------
 * newlib re-entrant variants (ignore reent ptr; use same FreeRTOS heap).
 * Linker uses --wrap=_malloc_r so it expects __wrap__malloc_r (one underscore).
 *------------------------------------------------------------------------------*/
#if defined(__GNUC__) && defined(__arm__) /* newlib _*_r only used on ARM+GCC */

/***************************************************************************/ /**
 * Wrapper for newlib _malloc_r. Uses same heap; sets reent errno on failure.
 ******************************************************************************/
void *__wrap__malloc_r(struct _reent *reent_ptr, size_t size)
{
  void *p = __wrap_malloc(size);
  if (p == NULL) {
    reent_ptr->_errno = ENOMEM; /* match newlib behavior for OOM */
  }
  return p;
}

/***************************************************************************/ /**
 * Wrapper for newlib _free_r. Reent pointer is ignored.
 ******************************************************************************/
void __wrap__free_r(struct _reent *reent_ptr, void *addr)
{
  (void)reent_ptr;
  __wrap_free(addr); /* delegate to free wrapper */
}

/***************************************************************************/ /**
 * Wrapper for newlib _calloc_r. Uses same heap; sets reent errno on failure.
 ******************************************************************************/
void *__wrap__calloc_r(struct _reent *reent_ptr, size_t nmemb, size_t size)
{
  void *p = __wrap_calloc(nmemb, size);
  if (p == NULL) {
    reent_ptr->_errno = ENOMEM; /* match newlib behavior for OOM */
  }
  return p;
}

/***************************************************************************/ /**
 * Wrapper for newlib _realloc_r. Uses same heap; sets reent errno only on
 * allocation failure (not when realloc(ptr, 0) frees and returns NULL).
 ******************************************************************************/
void *__wrap__realloc_r(struct _reent *reent_ptr, void *addr, size_t size)
{
  void *p = __wrap_realloc(addr, size);
  /* Set ENOMEM only when allocation failed; realloc(ptr, 0) returns NULL after free (success). */
  if (p == NULL && size != 0) {
    reent_ptr->_errno = ENOMEM;
  }
  return p;
}
#endif /* __GNUC__ && __arm__ */
