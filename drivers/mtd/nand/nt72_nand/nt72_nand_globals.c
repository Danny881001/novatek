#include "include/nt72_nand.h"

#ifdef IS_CVT
u8 _g_mem_area_nfc[MAX_PAGE_SIZE + MAX_OOB_SIZE] __aligned(128);
u8 _g_mem_area_golden[MAX_BLOCK_SIZE] __aligned(128);

u8 *g_mem_area_nfc = _g_mem_area_nfc;
u8 *g_mem_area_golden = _g_mem_area_golden;

int g_use_nt72_nand;
int g_pollute_dma;
int g_dump_ecc;
int g_dump_page;
int g_use_inv;
int g_use_cache;
#endif

#ifdef IS_UBOOT
int g_use_inv;
u8 _g_mem_area_nfc[MAX_PAGE_SIZE + MAX_OOB_SIZE] __aligned(128);
u8 *g_mem_area_nfc = _g_mem_area_nfc;
#endif

#ifdef IS_XBOOT
u32 g_dma_buf;
#endif

#ifdef IS_KERNEL
DEFINE_SEMAPHORE(g_sem_nfc);
#endif

#ifdef USE_ECC_INV_TABLE
struct nt72_nand_inv_map g_nt72_nand_inv_map;
#endif

#ifdef SHARE_GPIO_WITH_CI
int g_nfc_ready;
#endif

struct nt72_nand_info _info;
struct nt72_nand_info *g_info = &_info;
