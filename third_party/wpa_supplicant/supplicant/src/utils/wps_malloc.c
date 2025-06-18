#include "includes.h"
#include "os.h"
#ifdef DATA_PATH_UMAC_ENABLE
#include "bget.h"
#endif /* DATA_PATH_UMAC_ENABLE */

#ifdef SUPPLICANT_NON_ROM
#ifdef ENABLE_MALLOC_PRINTS
long glbl_free_val;
#endif
uint8 *sl_static_memory_pool;

void wps_mem_init(uint32 size)
{
   uint8  *base_addr;
   base_addr = sl_static_memory_pool;
   base_addr = (uint8 *)(((uint32)base_addr + (4-1)) & ~(4- 1));
#ifdef DATA_PATH_UMAC_ENABLE
   bpool(base_addr, size);
#else
   base_addr = (uint8 *)os_malloc(size);  // TODO: Recheck this in Everest
#endif /* DATA_PATH_UMAC_ENABLE */
}

void os_assert(int boolean) _OBE_LOW_POWER_RAM_
void os_assert(int boolean)
{
	if (!boolean) 
	{
	SLI_WLAN_MGMT_ASSERT(UMAC_ASSERT_OS_ASSERT);
#ifdef WISE_DEBUG_ENABLE
		SLI_WLAN_MGMT_ASSERT(WSC_ASSERT_BGET_ASSERT);
#endif
	}
}

void * os_realloc_array(void *ptr, size_t nmemb, size_t size)
{
  UINT8 *buf;

	buf =  os_realloc(ptr, nmemb * size);
  return buf;
}
#endif
