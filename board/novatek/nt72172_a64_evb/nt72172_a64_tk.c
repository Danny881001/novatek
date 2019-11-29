/*
 *  nt72172_a32_tk.c
 *
 *  Author:	Alvin lin
 *  Created:	Oct 13, 2016
 *  Copyright:	Novatek Inc.
 *
 */
#include <common.h>
#include <dm.h>
#include <errno.h>
#include <malloc.h>
#include <netdev.h>
#include <asm/io.h>
#include <usb.h>
#include <nvt_emmc_partition.h>
#include <asm/armv8/mmu.h>
DECLARE_GLOBAL_DATA_PTR;

static struct mm_region nt72172_a64_emmc_evb_mem_map[] = {
	{
		.base = 0x0UL,
		.size = 0x40000000UL,
		.attrs = PTE_BLOCK_MEMTYPE(MT_NORMAL) |
			 PTE_BLOCK_INNER_SHARE
	}, {
		.base = 0xeffd0000UL,
		.size = 0x20000UL,
		.attrs = PTE_BLOCK_MEMTYPE(MT_NORMAL) |
			 PTE_BLOCK_INNER_SHARE
	}, {
		.base = 0xfa000000UL,
		.size = 0x3000000UL,
		.attrs = PTE_BLOCK_MEMTYPE(MT_DEVICE_NGNRNE) |
			 PTE_BLOCK_NON_SHARE
			 /* TODO: Do we need these? */
			 /* | PTE_BLOCK_PXN | PTE_BLOCK_UXN */

	}, {
		.base = 0xfd000000UL,
		.size = 0x2000000UL,
		.attrs = PTE_BLOCK_MEMTYPE(MT_DEVICE_NGNRNE) |
			 PTE_BLOCK_NON_SHARE
			 /* TODO: Do we need these? */
			 /* | PTE_BLOCK_PXN | PTE_BLOCK_UXN */

	}, {
		.base = 0xffd00000UL,
		.size = 0x300000UL,
		.attrs = PTE_BLOCK_MEMTYPE(MT_DEVICE_NGNRNE) |
			 PTE_BLOCK_NON_SHARE
			 /* TODO: Do we need these? */
			 /* | PTE_BLOCK_PXN | PTE_BLOCK_UXN */

	}, {

		/* List terminator */
		0,
	}
};

struct mm_region *mem_map = nt72172_a64_emmc_evb_mem_map;


int misc_init_r(void)
{
	printf("%s called !\n", __func__);
	return 0;
}

int board_init(void)
{
	gd->bd->bi_arch_number = 0x104d;
	gd->bd->bi_boot_params = 0x06408000;
	//printf("reloc:0x%x\n", gd->relocaddr);
	return 0;
}

int board_mmc_init(bd_t *bis)
{
	printf("%s called !\n", __func__);
	//nvt_mmc_init();
	return 0;
}

int board_late_init(void)
{
	nvt_stbc_disable_wdog();

#if defined(CONFIG_NVT_TK_EMMC_UTILS)
	nvt_emmc_part_init();
#endif

#if defined(CONFIG_NVTTK_NO_EBURNER)
	return 0;
#endif
	return 0;
}
