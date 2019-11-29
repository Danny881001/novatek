/*
 *  board/novatek/nt72668tk/nt72668_panel_init.c
 *
 *  Created:	Mar 22, 2016
 *  Copyright:	Novatek Inc.
 *
 */

#include <common.h>
#include <asm/hardware.h>
#include <nvt_panel_init.h>
#include <nvt_emmc_xbootdat.h>


bool _VID_PowerON_Sequence()
{
	printf("%s implemented !\n", __func__);
	return 1;
}

void panel_init_late(void)
{
}
