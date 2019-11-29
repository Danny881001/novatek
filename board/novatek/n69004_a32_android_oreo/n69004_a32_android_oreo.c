/*
 *  n69004_a32_android_oreo.c
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
#include <asm/arch/hardware.h>
#include <nvt_emmc_partition.h>
#include <nvt_emmc_fwupdate.h>
#include <nvt_eburner.h>
#include <nvt_stbc.h>
#include <nvt_emmc_utils.h>
#include <nvt_common_utils.h>
#if defined(CONFIG_NVT_RTK_BT_FWLOAD)
#include "../3rd_party/rtk_bt/osi/include/osi_usb.h"
#endif

DECLARE_GLOBAL_DATA_PTR;

#if defined(CONFIG_NVT_RTK_BT_FWLOAD)
extern int load_rtl_firmware_dev(struct osi_usb_device *dev);
extern struct osi_usb_device *osi_udev_rtk_bt;
#endif

#define NVT_EBURNER_V2_ADC_REG 			0xFC040310
#define NVT_EBURNER_V2_ADC_VAL                  0x3
#define NVT_EBURNER_V2_ADC_RANGE                0x3

int misc_init_r(void)
{
	printf("%s called !\n", __func__);
	return 0;
}

int board_init(void)
{
	gd->bd->bi_arch_number = 0x104d;
	gd->bd->bi_boot_params = 0x06408000;
	return 0;
}

int board_mmc_init(bd_t *bis)
{
	printf("%s called !\n", __func__);
	//nvt_mmc_init();
	return 0;
}

void n69004_a32_android_oreo_burn_image(void)
{
	int ret;
	u32 reg_val, tmp_val;
	char cmd[128] = {0};

	nvt_setup_emmc_boot_config();

	setenv("usb_port","0");

	run_command("usb start", 0);

	sprintf(cmd, "fatload usb 0:auto 0x%x xboot.bin 4", CONFIG_SYS_FWUPDATE_BUF);

	if(run_command(cmd , 0))
		setenv("usb_port", "3");

	ret = run_command("ewrite xboot", 0);
	if(ret < 0) {
		printf("write xboot fail ...!\n");
		goto out;
	}
	printf("write xboot done ...\n");

#if defined(CONFIG_NVT_TK_SPI_UTILS)
	ret = run_command("swrite stbc stbc.bin", 0);

	if(ret < 0) {
		printf("write spi stbc fail ...!\n");
		goto out;
	}
	printf("write spi stbc done ...\n");
#endif
	ret = run_command("ewrite stbc stbc_emmc.bin", 0);

	if(ret < 0) {
		printf("write emmc stbc fail ...!\n");
		goto out;
	}
	printf("write spi stbc done ...\n");


	ret = run_command("ewrite ddrcfg", 0);

	if(ret < 0) {
		printf("write ddrcfg fail !\n");
		goto out;
	}

	ret = run_command("ewrite uboot", 0);
	if(ret < 0) {
		printf("write U-Boot fail ...!\n");
		goto out;
	}
	printf("write U-Boot done...\n");

	ret = run_command("xsave panel config xbootdat.bin", 0);
	if(ret < 0) {
		printf("write panel config fail !\n");
		goto out;
	}
	#if 0
	ret = run_command("ewrite param", 0);
	if(ret < 0) {
		printf("write param fail ...!\n");
		goto out;
	}
	printf("write param done...\n");
	#endif

	ret = run_command("ewrite fdt0", 0);
	if(ret < 0) {
		printf("write fdt0 fail ...!\n");
		goto out;
	}
	printf("write fdt0 done...\n");

	ret = run_command("ewrite fdt1", 0);
	if(ret < 0) {
		printf("write fdt1 fail ...!\n");
		goto out;
	}
	printf("write fdt1 done...\n");

	ret = run_command("ewrite ker0", 0);
	if(ret < 0) {
		printf("write kernel fail ...!\n");
		goto out;
	}
	printf("write kernel done...\n");

	ret = run_command("ewrite ker1", 0);
	if(ret < 0) {
		printf("write recovery fail ...!\n");
		goto out;
	}
	printf("write recovery done...\n");

	ret = run_command("ewrite kerupm", 0);
	if(ret < 0) {
		printf("write kerupm fail ...!\n");
		goto out;
	}
	printf("write kerupm done...\n");

	ret = run_command("epart write", 0);
	if(ret < 0) {
		printf("write partition fail ...!\n");
		goto out;
	}
	printf("write partition done ...\n");

	ret = run_command("ewrite system", 0);
	if(ret < 0) {
		printf("write system image fail ...!\n");
		goto out;
	}

	ret = run_command("ewrite cache", 0);
	if(ret < 0) {
		printf("write cache image fail ...!\n");
		goto out;
	}

	ret = run_command("ewrite atv", 0);
	if(ret < 0) {
		printf("write atv image fail ...!\n");
		goto out;
	}
	
	ret = run_command("ewrite dtv", 0);
	if(ret < 0) {
		printf("write dtv image fail ...!\n");
		goto out;
	}

	ret = run_command("ewrite factory", 0);
	if(ret < 0) {
		printf("write factory image fail ...!\n");
		goto out;
	}

	ret = run_command("ewrite skyworth", 0);
	if(ret < 0) {
		printf("write skyworth image fail ...!\n");
		goto out;
	}

	ret = run_command("ewrite securestore", 0);
	if(ret < 0) {
		printf("write securestore image fail ...!\n");
		goto out;
	}

	ret = run_command("ewrite skyota", 0);
	if(ret < 0) {
		printf("write skyota image fail ...!\n");
		goto out;
	}

	ret = run_command("ewrite vendor", 0);
	if(ret < 0) {
		printf("write vendor image fail ...!\n");
		goto out;
	}

	ret = run_command("ewrite data", 0);
	if(ret < 0) {
		printf("write data image fail ...!\n");
		goto out;
	}


	run_command("emiscmd \"\"", 0);

	ret = run_command("xsave kercmd", 0);
	if(ret < 0) {
		printf("generate kernel command line fail !\n");
		goto out;
	}

	ret = run_command("xsave rcvcmd", 0);
	if(ret < 0) {
		printf("generate recovery command line fail !\n");
		goto out;
	}

	printf("Burning image process complete, please reboot this device.\n");

	//auto reboot
	nvt_stbc_set_keypass(1);

	//switch authority
	__REG(0xFC04041C) |= ((1 << 2) | (1 << 0));

	//set STBC_GPIO0,2 set as input
	__REG(0xFC040438) |= (1 << 6);
	__REG(0xFC04042C) &= ~(1 << 6);
	__REG(0xFC040438) |= (1 << 8);
	__REG(0xFC04042C) &= ~(1 << 8);

	__REG(0xFC040210) = 0xF0;

	while(1);

out:
	return;
}

static int do_nvt_ewriteall(cmd_tbl_t * cmdtp, int flag, int argc, char * const argv[])
{
	n69004_a32_android_oreo_burn_image();
}

U_BOOT_CMD(
		ewriteall,3,	0,	do_nvt_ewriteall,
		"ewriteall    - write all images to emmc\n",
		""
	  );

static int do_sky_emmc_io_test(cmd_tbl_t * cmdtp, int flag, int argc, char * const argv[])
{
	int count;
	int ret = 0;

	if(argc != 2) {
		ret = CMD_RET_USAGE;
		goto out;
	}

	count = simple_strtoul(argv[1], NULL, 10);

	ret = nvt_mmc_io_test("skyota", count);

	if(ret < 0)
		printf("emmc io test fail !\n");
	else
		printf("emmc io test sucess !\n");

out:
	return ret;
}

U_BOOT_CMD(
		emmc_io_test,2,	1,	do_sky_emmc_io_test,
		"emmc_io_test  count  - do emmc read/write/reset test\n",
		"emmc_io_test count"
	  );

#if defined(CONFIG_NVT_EUPM)

static int do_nvt_sky_raw_dump(cmd_tbl_t * cmdtp, int flag, int argc, char * const argv[])
{
	setenv("eupm_param", "sky_raw_dump");
	run_command("eupm", 0);
}
U_BOOT_CMD(
	sky_raw_dump,	1,	0,	do_nvt_sky_raw_dump,
	"sky_raw_dump",
	""
);

static int do_nvt_sky_raw_restore(cmd_tbl_t * cmdtp, int flag, int argc, char * const argv[])
{
	setenv("eupm_param", "sky_raw_restore");
	run_command("eupm", 0);
}
U_BOOT_CMD(
	sky_raw_restore,	1,	0,	do_nvt_sky_raw_restore,
	"sky_raw_restore",
	""
);
#endif //CONFIG_NVT_EUPM


#if defined(CONFIG_NVTTK_EBURNER_V2)
static int n69004_eburner_v2(void)
{
	int ret = 0;
	char cmd[128] = {};

	if(nvt_in_empty_burner_mode()) {
		nvt_setup_emmc_boot_config();
		run_command("epart write", 0);
		run_command("emiscmd \"\"", 0);
		setenv("cpus", "1");
	}

	run_command("usb reset", 0);

	sprintf(cmd, "fatload usb 0:auto 0x%x %s 4", CONFIG_SYS_FWUPDATE_BUF, NVT_EBURNER_IMG_FNAME);

	//if update image not exist, boot to recovery mode
	if(run_command(cmd , 0)) {
		printf("update file %s not exist, boot to recovery mode \n", NVT_EBURNER_IMG_FNAME);
		run_command("emiscmd boot-recovery", 0);
		run_command("reboot", 0);
		while(1);
	}


	setenv("quiet", "y");

	ret = nvt_eburner_run_updater();

	if(ret < 0) {
		printf("%s run empty burner v2 fail !\n", __func__);
		while(1);
	}

	return ret;
}

static int do_nvt_eburner(cmd_tbl_t * cmdtp, int flag, int argc, char * const argv[])
{
	nvt_setup_emmc_boot_config();
	run_command("epart write", 0);
	run_command("emiscmd \"\"", 0);
	n69004_eburner_v2();
}

U_BOOT_CMD(
		nvt_eburner,3,	0,	do_nvt_eburner,
		"nvt_eburner    - run novatek eburner to update image\n",
		""
	  );


#endif//CONFIG_NVTTK_EBURNER_V2

#if defined(CONFIG_NVT_RTK_BT_FWLOAD)
static int do_nvt_bt_fw_load(cmd_tbl_t * cmdtp, int flag, int argc, char * const argv[])
{	
	run_command("usb reset", 0);
	load_rtl_firmware_dev(osi_udev_rtk_bt);
}

U_BOOT_CMD(
		nvt_bt_fw_load,3,	0,	do_nvt_bt_fw_load,
		"nvt_bt_fw_load    - load firmware to bt dongle\n",
		""
	  );
#endif


int board_late_init(void)
{
	nvt_stbc_disable_wdog();

#if defined(CONFIG_NVT_TK_EMMC_UTILS)
	nvt_emmc_part_init();
#endif

#if defined(CONFIG_NVTTK_NO_EBURNER)
	return 0;
#endif


#if defined(CONFIG_EMMC_WRITE_RELIABILITY_TEST)
	if(!nvt_in_empty_burner_mode()) {
		run_command("mmc_wr_check", 0);
		run_command("mmc_wr_test", 0);
        } else {
		nvt_setup_emmc_boot_config();
		run_command("ewrite xboot", 0);
		run_command("ewrite uboot", 0);
        }
	return 0;
#endif

#if defined(CONFIG_NVTTK_EBURNER_V2)
	if(nvt_in_empty_burner_mode()) {
		nvt_setup_emmc_boot_config();
		run_command("epart write", 0);
		run_command("emiscmd \"\"", 0);
		n69004_eburner_v2();
	}
	if(nvt_eburner_check_trigger(NVT_EBURNER_V2_ADC_REG, NVT_EBURNER_V2_ADC_VAL, NVT_EBURNER_V2_ADC_RANGE))
		n69004_eburner_v2();
#endif//CONFIG_NVTTK_EBURNER_V2


	if(nvt_in_empty_burner_mode()) {
		run_command("ewriteall", 0);
	}

#if defined(CONFIG_NVT_RTK_BT_FWLOAD)
	int ch;
	ch = serial_getc_nonblock();
	if(ch == '~')
		return 0;
	if(nvt_stbc_read_power_on_event() == EN_POWER_ON_EVENT_AC_POWER_TO_ON_THEN_STANDBY) {
		run_command("nvt_bt_fw_load", 0);
		run_command("halt", 0);
	}
#endif
	return 0;
}
