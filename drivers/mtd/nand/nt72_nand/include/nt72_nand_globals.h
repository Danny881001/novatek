#ifndef _NT72_NAND_GLOBALS_H_
#define _NT72_NAND_GLOBALS_H_

#include "nt72_nand.h"

#ifdef IS_CVT
extern u8 *g_mem_area_nfc;
extern u8 *g_mem_area_ff;
extern u8 *g_mem_area_golden;

extern int g_use_nt72_nand;
extern int g_pollute_dma;
extern int g_dump_ecc;
extern int g_dump_page;
extern int g_use_inv;
extern int g_use_cache;
#endif

#ifdef IS_UBOOT
extern int g_use_inv;
extern u8 *g_mem_area_nfc;
#endif

#ifdef IS_XBOOT
extern u32 g_dma_buf;
#endif

#ifdef IS_KERNEL
extern struct semaphore g_sem_nfc;
#endif

#ifdef USE_ECC_INV_TABLE
extern struct nt72_nand_inv_map g_nt72_nand_inv_map;
#endif

#ifdef SHARE_GPIO_WITH_CI
extern int g_nfc_ready;
#endif

extern struct nt72_nand_info _info;
extern struct nt72_nand_info *g_info;

#endif
