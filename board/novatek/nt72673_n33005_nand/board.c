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
#include "update_cmds.h"
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

static void n33005_rckey_update(void)
{
	if(0xaa == ((*((volatile u32 *) (0xFC04011C))) & 0xFF)) /* RC key update */
	{
		printf("RC key update!---------\n");
#if defined(CONFIG_CMD_NVT_SCRIPT_UPDATE)
		int i;
		char usb_port[2];

		for (i = 0; i < 4; i++)
		{
			usb_port[0] = '0' + i;
			usb_port[1] = 0;
			printf("switch to usb port%s\n", usb_port);
			setenv("usb_port", usb_port);

			if (run_command("nvt_script_update", 0) == 0)
			{
				printf("\n\nUSB script update completed.\n");
				break;
			}
		}
		printf("\n\nRebooting...\n");
		run_command("reboot", 0);
#else
		printf("\n\nFunction disabled! CONFIG_CMD_NVTUPDATE is not set!\n");
#endif
	}
	else if(0xcc == ((*((volatile u32 *) (0xFC04011C))) & 0xFF)) /* OAD update */
	{
		//TODO: update oad image
	}
}


int board_late_init(void)
{
	nvt_stbc_disable_wdog();

	nvt_fwupdate_help_init();
	nvt_nand_ptn_init();
	nvt_nand_xbootdat_init();

	if(nvt_in_empty_burner_mode()) {
		run_command("nwriteall", 0);
	}
	
	n33005_rckey_update();

	return 0;
}
