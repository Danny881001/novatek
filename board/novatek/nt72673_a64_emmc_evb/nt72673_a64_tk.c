/*
 *  nt72673_a64_tk.c
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
#include <asm/arch/hardware.h>
#include <usb.h>
#include <nvt_emmc_partition.h>
#include <nvt_emmc_fwupdate.h>
#include <nvt_stbc.h>
#include <asm/armv8/mmu.h>
DECLARE_GLOBAL_DATA_PTR;

static struct mm_region nt72673_a64_emmc_evb_mem_map[] = {
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

struct mm_region *mem_map = nt72673_a64_emmc_evb_mem_map;


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

void nt72673_a64_emmc_burn_image(void)
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
	ret = run_command("ewrite stbc", 0);

	if(ret < 0) {
		printf("write emmc stbc fail ...!\n");
		goto out;
	}
	printf("write emmc stbc done ...\n");


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

	ret = run_command("ewrite memdtb", 0);
	if(ret < 0) {
		printf("write memdtb fail ...!\n");
		goto out;
	}

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

	ret = run_command("ewrite secos", 0);
	if(ret < 0) {
		printf("write secos fail....!\n");
		goto out;
	}
	printf("write secos done...\n");

	ret = run_command("epart write", 0);
	if(ret < 0) {
		printf("write partition fail ...!\n");
		goto out;
	}
	printf("write partition done ...\n");

	ret = run_command("ewrite fs0", 0);
	if(ret < 0) {
		printf("write fs0 image fail ...!\n");
		goto out;
	}

	ret = run_command("ewrite ap0", 0);
	if(ret < 0) {
		printf("write ap0 image fail ...!\n");
		goto out;
	}

	ret = run_command("emark_format_db rw_area", 0);
	if(ret < 0) {
		printf("emark_format_db rw_area fail ...!\n");
		goto out;
	}
	printf("emakr_format_db rw_area done!\n");

	ret = run_command("eerase vinfo", 0);
	if(ret < 0) {
		printf("eerase vinfo fail ...!\n");
		goto out;
	}
	printf("eerase vinfo done!\n");

	run_command("env default -a", 0);

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

#if defined(CONFIG_NVT_TURNKEY_FWUPDATE)
	ret = run_command("ewrite_boot_flag 0 0 0", 0);
	if(ret < 0) {
		printf("write boot flag fail !\n");
		goto out;
	}

	ret = run_command("ewrite vtbl", 0);
	if(ret < 0) {
		printf("write vtbl fail !\n");
		goto out;
	}

	ret = run_command("ewrite pptbl0", 0);
	if(ret < 0) {
		printf("write pptbl0 fail !\n");
		goto out;
	}

	ret = run_command("ewrite pptbl1", 0);
	if(ret < 0) {
		printf("write pptbl1 fail !\n");
		goto out;
	}

	ret = run_command("efix_bootargs", 0);

	if(ret < 0) {
		printf("fix bootargs by boot flag fail !\n");
		goto out;
	}


#endif	

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
	nt72673_a64_emmc_burn_image();
}

U_BOOT_CMD(
		ewriteall,3,	0,	do_nvt_ewriteall,
		"ewriteall    - write all images to emmc\n",
		""
	  );


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
