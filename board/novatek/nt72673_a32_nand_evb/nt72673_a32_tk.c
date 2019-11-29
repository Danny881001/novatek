#include <common.h>
#include <dm.h>
#include <errno.h>
#include <malloc.h>
#include <netdev.h>
#include <asm/io.h>
#include <usb.h>
#include <asm/arch/hardware.h>
#include <nvt_stbc.h>
#include <nvt_nand_utils.h>
#include <nvt_common_utils.h>
#include "nt72673_update_cmds.h"
#include <nvt_nand_xbootdat.h>
#include <nvt_nand_partition.h>

DECLARE_GLOBAL_DATA_PTR;

int misc_init_r(void)
{
	printf("%s called !\n", __func__);
	return 0;
}

int board_init(void)
{
	printf("%s called !\n", __func__);
	gd->bd->bi_arch_number = 0x104d;
	gd->bd->bi_boot_params = 0x06408000;
	return 0;
}

int board_late_init(void)
{
	nvt_stbc_disable_wdog();

	nvt_fwupdate_help_init();
	nvt_nand_ptn_init();
	nvt_nand_xbootdat_init();

#if defined(CONFIG_NVTTK_NO_EBURNER)
	return 0;
#endif
	if(nvt_in_empty_burner_mode()) {
		run_command("nwriteall", 0);
	}

	return 0;
}
