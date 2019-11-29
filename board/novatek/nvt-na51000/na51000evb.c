/**
    NVT evb board file
    To handle nt96680 basic init.
    @file       nt96680evb.c
    @ingroup
    @note
    Copyright   Novatek Microelectronics Corp. 2016.  All rights reserved.

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 2 as
    published by the Free Software Foundation.
*/


#include <common.h>
#include <asm/mach-types.h>
#include <asm/arch/nvt_common.h>
#include <asm/arch/rcw_macro.h>
#include <asm/arch/IOAddress.h>
#include <asm/arch/na51000evb.h>
#include <libfdt.h>

DECLARE_GLOBAL_DATA_PTR;
extern int nvt_mmc_init(int id);
extern int na51000_eth_initialize(bd_t *bis);
const char *boardinfo[] = {
	"Board:"_CHIP_NAME_"EVB\n"
};

#define WDT_REG_ADDR(ofs)       (IOADDR_WDT_REG_BASE+(ofs))
#define WDT_GETREG(ofs)         INW(WDT_REG_ADDR(ofs))
#define WDT_SETREG(ofs,value)   OUTW(WDT_REG_ADDR(ofs), (value))

#define CG_REG_ADDR(ofs)       (IOADDR_CG_REG_BASE+(ofs))
#define CG_GETREG(ofs)         INW(CG_REG_ADDR(ofs))
#define CG_SETREG(ofs,value)   OUTW(CG_REG_ADDR(ofs), (value))

#define CG_ENABLE_OFS 0x84
#define CG_RESET_OFS 0xA4
#define WDT_POS (1 << 17)

#define WDT_CTRL_OFS 0x0

void reset_cpu(unsigned long ignored)
{
	u32 reg_value;

	reg_value = CG_GETREG(CG_ENABLE_OFS);
	CG_SETREG(CG_ENABLE_OFS, reg_value | WDT_POS);

	reg_value = CG_GETREG(CG_RESET_OFS);
	CG_SETREG(CG_RESET_OFS, reg_value | WDT_POS);


	WDT_SETREG(WDT_CTRL_OFS, 0x5A960112);

	udelay(80);

	WDT_SETREG(WDT_CTRL_OFS, 0x5A960113);
}

static void gpio_set_output(u32 pin)
{
	u32 reg_data;
	u32 ofs = (pin >> 5) << 2;

	pin &= (32 - 1);

	reg_data = INW(IOADDR_GPIO_REG_BASE + 0x30 + ofs);
	reg_data |= (1 << pin);    //output
	OUTW(IOADDR_GPIO_REG_BASE + 0x30 + ofs, reg_data);
}

static void gpio_set_pin(u32 pin)
{
	u32 tmp;
	u32 ofs = (pin >> 5) << 2;

	pin &= (32 - 1);
	tmp = (1 << pin);

	OUTW(IOADDR_GPIO_REG_BASE + 0x50 + ofs, tmp);
}

static void gpio_clear_pin(u32 pin)
{
	u32 tmp;
	u32 ofs = (pin >> 5) << 2;

	pin &= (32 - 1);
	tmp = (1 << pin);

	OUTW(IOADDR_GPIO_REG_BASE + 0x70 + ofs, tmp);
}

void ethernet_init(void)
{
	ulong fdt_addr = nvt_readl((ulong)nvt_shminfo_boot_fdt_addr);
	int nodeoffset, len;
	u32 *cell = NULL;
	char path[20] = {0};
	u32 sensor_cfg =0, eth_cfg = 0;

	sprintf(path,"/top@%x/sensor",IOADDR_TOP_REG_BASE);

	nodeoffset = fdt_path_offset((const void*)fdt_addr, path);
	if (nodeoffset < 0) {
		printf("%s(%d) nodeoffset < 0\n",__func__, __LINE__);
		return ;
	}

	cell = (u32*)fdt_getprop((const void*)fdt_addr, nodeoffset, "pinmux", &len);
	if (len == 0) {
		printf("%s(%d) len = 0\n",__func__, __LINE__);
		return ;
	}

	sensor_cfg = __be32_to_cpu(cell[0]);
	//printf("%s(%d) sensor_cfg = 0x%x\n",__func__, __LINE__,sensor_cfg);


	sprintf(path,"/eth@%x",IOADDR_ETH_REG_BASE);

	nodeoffset = fdt_path_offset((const void*)fdt_addr, path);
	if (nodeoffset < 0) {
		printf("%s(%d) nodeoffset < 0\n",__func__, __LINE__);
		return ;
	}

	cell = (u32*)fdt_getprop((const void*)fdt_addr, nodeoffset, "sp-clk", &len);
	if (len == 0) {
		printf("%s(%d) len = 0\n",__func__, __LINE__);
		return ;
	}

	eth_cfg = __be32_to_cpu(cell[0]);
	//printf("%s(%d) eth_cfg = 0x%x\n",__func__, __LINE__,eth_cfg);


	if ((sensor_cfg&0x20000) || (eth_cfg == 1)) {
		*(uint32_t*) 0xF0020000 &= ~(0x01<<4);    // pll4_en disable

		*(uint32_t*) 0xF0021318 = 0x00;
		*(uint32_t*) 0xF002131C = 0x00;
		*(uint32_t*) 0xF0021320 = 0x32;

		*(uint32_t*) 0xF0020024 &=  ~0x4300;
		*(uint32_t*) 0xF0020024 |= (0x01<<8);    // spclk_sel: pll4

		*(uint32_t*) 0xF002003C &=  ~0xFF;
		*(uint32_t*) 0xF002003C |= 0x0B;

		*(uint32_t*) 0xF0020000 |= (0x01<<4);    // pll4_en
		*(uint32_t*) 0xF0020080 |= (0x01<<12);   // spclk_en

		*(uint32_t*) 0xF0010018 &= ~(0x3<<8);    // pinmux: SP_CLK_1
		*(uint32_t*) 0xF0010018 |= (0x1<<8);
		*(uint32_t*) 0xF00100A8 &= ~(0x01<<22);  // pinumx: pgpio22
    }

#ifdef ETH_PHY_HW_RESET
	gpio_set_output(NVT_PHY_RST_PIN);
	gpio_clear_pin(NVT_PHY_RST_PIN);
	mdelay(20);
	gpio_set_pin(NVT_PHY_RST_PIN);
	mdelay(50);
#endif
}

void sdio_power_cycle(void)
{
#ifdef CONFIG_SD_CARD1_POWER_PIN
	gpio_set_output(CONFIG_SD_CARD1_POWER_PIN);
	if (CONFIG_SD_CARD1_ON_STATE)
		gpio_clear_pin(CONFIG_SD_CARD1_POWER_PIN);
	else
		gpio_set_pin(CONFIG_SD_CARD1_POWER_PIN);

	*(u32*) 0xF00100A0 |= 0x3F0000;
	*(u32*) 0xF0030004 &= ~0xFFF;
	*(u32*) 0xF0030004 |= 0x555;
	gpio_set_output(C_GPIO(16));
	gpio_set_output(C_GPIO(17));
	gpio_set_output(C_GPIO(18));
	gpio_set_output(C_GPIO(19));
	gpio_set_output(C_GPIO(20));
	gpio_set_output(C_GPIO(21));
	gpio_clear_pin(C_GPIO(16));
	gpio_clear_pin(C_GPIO(17));
	gpio_clear_pin(C_GPIO(18));
	gpio_clear_pin(C_GPIO(19));
	gpio_clear_pin(C_GPIO(20));
	gpio_clear_pin(C_GPIO(21));
#endif

#ifdef CONFIG_SD_CARD2_POWER_PIN
	gpio_set_output(CONFIG_SD_CARD2_POWER_PIN);
	if (CONFIG_SD_CARD2_ON_STATE)
		gpio_clear_pin(CONFIG_SD_CARD2_POWER_PIN);
	else
		gpio_set_pin(CONFIG_SD_CARD2_POWER_PIN);

	*(u32*) 0xF00100A0 |= 0xFC00000;
	*(u32*) 0xF0030004 &= ~0xFFF000;
	*(u32*) 0xF0030004 |= 0x555000;
	gpio_set_output(C_GPIO(22));
	gpio_set_output(C_GPIO(23));
	gpio_set_output(C_GPIO(24));
	gpio_set_output(C_GPIO(25));
	gpio_set_output(C_GPIO(26));
	gpio_set_output(C_GPIO(27));
	gpio_clear_pin(C_GPIO(22));
	gpio_clear_pin(C_GPIO(23));
	gpio_clear_pin(C_GPIO(24));
	gpio_clear_pin(C_GPIO(25));
	gpio_clear_pin(C_GPIO(26));
	gpio_clear_pin(C_GPIO(27));
#endif
}

/**
 * @brief board_early_init_f
 *
 * @return 0
 */
int board_early_init_f(void)
{
	//nvt_ipc_init();
	return 0;
}

/**
 * @brief board_init
 *
 * @return 0
 */
int board_init(void)
{
	int ret = 0;
	unsigned int  cval_high;
	unsigned int  cval_low;

	nvt_tm0_cnt_beg = get_nvt_timer0_cnt();

	/*gd->bd->bi_arch_number = MACH_TYPE_EVB96680;
	gd->bd->bi_boot_params = NVT_LINUX_BOOT_PARAM_ADDR;*/
	printf("Relocation to 0x%08lx, Offset is 0x%08lx sp at %08lx\n", gd->relocaddr, gd->reloc_off, gd->start_addr_sp);

	icache_disable();
	dcache_disable();

	invalidate_icache_all();
	invalidate_dcache_all();

	asm volatile ("mrrc p15, 1, %0, %1, c15" : "=r" (cval_low), "=r" (cval_high));
	cval_low &= ~0x40;
	asm volatile ("mcrr p15, 1, %0, %1, c15" : : "r" (cval_low), "r" (cval_high));
	cval_low |= 0x40;
	asm volatile ("mcrr p15, 1, %0, %1, c15" : : "r" (cval_low), "r" (cval_high));
	asm volatile ("mrrc p15, 1, %0, %1, c15" : "=r" (cval_low), "=r" (cval_high));

	printf("CPUECTLR->SMPEN = %d\n", (cval_low & 0x40)?1:0);

	icache_enable();
	dcache_enable();

	nvt_shminfo_init();
	ret = nvt_fdt_init(false);
	if (ret < 0) {
		printf("fdt init fail\n");
	}
	ethernet_init();
	nvt_print_system_info();

	return 0;
}

int board_mmc_init(bd_t *bis)
{
	int ret = 0, i;

	for (i = 0; i < CONFIG_NVT_MMC_MAX_NUM; i++) {
		if((CONFIG_NVT_MMC_CHANNEL >> i) & 0x1) {
			ret = nvt_mmc_init(i);
			if(ret)
				break;
		}
	}

	return ret;
}

#if 0
int board_nand_init(bd_t *bis)
{
	return 0;
}
#endif

int board_eth_init(bd_t *bis)
{
	int rc = 0;

#ifdef CONFIG_ETHNET
	rc = na51000_eth_initialize(bis);
#endif

	return rc;
}

static int nvt_handle_fw_abin(void)
{
	int ret = 0;
	char cmdline[1024] = {0};
	char buf[1024] = {0};

	/* To handle firmware update */
	ret = nvt_fw_update(false);
	if (ret < 0) {
		switch (ret) {
		case ERR_NVT_UPDATE_FAILED:
			printf("%sUpdate fail %s\r\n", ANSI_COLOR_RED, ANSI_COLOR_RESET);
			return -1;
			break;
		case ERR_NVT_UPDATE_OPENFAILED:
			printf("Open SD fail:%s No SD device? %s\r\n", ANSI_COLOR_YELLOW, ANSI_COLOR_RESET);
			break;
		case ERR_NVT_UPDATE_READ_FAILED:
			printf("%sRead SD fail %s\r\n", ANSI_COLOR_RED, ANSI_COLOR_RESET);
			return -1;
			break;
		case ERR_NVT_UPDATE_NO_NEED:
			printf("%sNo need to update (%s) %s\r\n", ANSI_COLOR_YELLOW, get_nvt_bin_name(NVT_BIN_NAME_TYPE_FW), ANSI_COLOR_RESET);
			break;
		default:
			break;
		}
		ret = nvt_fdt_init(true);
		if (ret < 0) {
			printf("modelext init fail\n");
		}
	} else
		printf("%sUpdate successfully %s\r\n", ANSI_COLOR_YELLOW, ANSI_COLOR_RESET);
	/*
	 * To handle bootargs expanding for the kernel /proc/cmdline and uboot mtdids env setting
	 */
	sprintf(buf,"%s ",getenv("bootargs"));
	strcat(cmdline, buf);
	ret = nvt_part_config((char *)cmdline, NULL);
	if (ret < 0)
		return ret;

	setenv("bootargs",cmdline);

	return 0;
}

static int nvt_handle_fw_tbin(void)
{
	int ret = 0;
	char cmdline[1024] = {0};
	char buf[1024] = {0};

	ret = nvt_fw_load_tbin();
	if (ret < 0) {
		switch (ret) {
		case ERR_NVT_UPDATE_OPENFAILED:
			printf("Open SD fail:%s No SD device? (%s) %s\r\n", ANSI_COLOR_YELLOW, get_nvt_bin_name(NVT_BIN_NAME_TYPE_RUNFW), ANSI_COLOR_RESET);
			break;
		case ERR_NVT_UPDATE_NO_NEED:
		case ERR_NVT_UPDATE_READ_FAILED:
			printf("%sRead SD fail (%s) %s\r\n", ANSI_COLOR_RED, get_nvt_bin_name(NVT_BIN_NAME_TYPE_RUNFW), ANSI_COLOR_RESET);
			return -1;
			break;
		default:
			break;
		}
	} else
		printf("%sLoad successfully %s\r\n", ANSI_COLOR_YELLOW, ANSI_COLOR_RESET);

	ret = nvt_fdt_init(false);
	if (ret < 0) {
		printf("modelext init fail\n");
		return ret;
	}

	/*
	 * To handle bootargs expanding for the kernel /proc/cmdline and uboot mtdids env setting
	 */
	sprintf(buf,"%s ",getenv("bootargs"));
	strcat(cmdline, buf);
	ret = nvt_part_config((char *)cmdline, NULL);
	if (ret < 0)
		return ret;

	setenv("bootargs",cmdline);

	return 0;
}

static int nvt_handle_fw_update_fw(void)
{
	int ret = 0;
	unsigned long val = 0;

	printf("%sStarting to update firmware runtime %s\r\n", ANSI_COLOR_YELLOW, ANSI_COLOR_RESET);
	/* To handle firmware update */
	ret = nvt_fw_update(true);
	if (ret < 0) {
		printf("%sUpdate fail %s\r\n", ANSI_COLOR_RED, ANSI_COLOR_RESET);
		val = (nvt_readl((ulong)nvt_shminfo_comm_uboot_boot_func) & ~COMM_UBOOT_BOOT_FUNC_BOOT_DONE_MASK) | COMM_UBOOT_BOOT_FUNC_BOOT_NG;
		nvt_writel(val, (ulong)nvt_shminfo_comm_uboot_boot_func);
		flush_dcache_all();
		return -1;
	} else {
		printf("%sUpdate firmware successfully %s\r\n", ANSI_COLOR_YELLOW, ANSI_COLOR_RESET);

		// Update finished.
		val = (nvt_readl((ulong)nvt_shminfo_comm_uboot_boot_func) & ~COMM_UBOOT_BOOT_FUNC_BOOT_DONE_MASK) | COMM_UBOOT_BOOT_FUNC_BOOT_DONE;
		nvt_writel(val, (ulong)nvt_shminfo_comm_uboot_boot_func);
		flush_dcache_all();
		while(1) {
			// Waiting for itron trigger reboot.
			printf(".");
			mdelay(1000);
		}
	}

	return 0;
}

static int nvt_handle_update_fw_by_usb_eth(void)
{
	int ret = 0;
	char cmdline[1024] = {0};
	char buf[1024] = {0};

	printf("%sStarting to update firmware from USB/ETH%s\r\n", ANSI_COLOR_YELLOW, ANSI_COLOR_RESET);
	/* To handle firmware update */
	ret = nvt_fw_update(true);
	if (ret < 0) {
		printf("%sUpdate fail %s\r\n", ANSI_COLOR_RED, ANSI_COLOR_RESET);
		return -1;
	} else {
		printf("%sUpdate firmware successfully %s\r\n", ANSI_COLOR_YELLOW, ANSI_COLOR_RESET);
	}

	/*
	 * To handle bootargs expanding for the kernel /proc/cmdline and uboot mtdids env setting
	 * Continue to boot
	 */
	sprintf(buf,"%s ",getenv("bootargs"));
	strcat(cmdline, buf);
	ret = nvt_part_config((char *)cmdline, NULL);
	if (ret < 0)
		return ret;

	setenv("bootargs",cmdline);
	return 0;
}

static int nvt_handle_format_rw_rootfs(void)
{
	int ret = 0;
	ret = nvt_process_rootfs_format();

	return ret;
}

static int nvt_handle_recovery_sys(void)
{
	int ret = 0;
	char cmdline[1024] = {0};
	char buf[1024] = {0};
	ret = nvt_process_sys_recovery();

	/*
	 * To handle bootargs expanding for the kernel /proc/cmdline and uboot mtdids env setting
	 */
	sprintf(buf,"%s ",getenv("bootargs"));
	strcat(cmdline, buf);
	ret = nvt_part_config((char *)cmdline, NULL);
	if (ret < 0)
		return ret;

	setenv("bootargs",cmdline);

	return ret;
}

#ifdef CONFIG_NVT_LINUX_AUTODETECT
static int nvt_handle_fw_auto(void)
{
	int ret = 0;
	unsigned long boot_reason = nvt_readl((ulong)nvt_shminfo_comm_uboot_boot_func) & COMM_UBOOT_BOOT_FUNC_BOOT_REASON_MASK;

	if (boot_reason == COMM_UBOOT_BOOT_FUNC_BOOT_UPDFIRM) {
		// Update by itron: firmware will be loaded by itron.
		ret = nvt_handle_fw_update_fw();
		if (ret < 0)
			return ret;
	} else if ((boot_reason == COMM_UBOOT_BOOT_FUNC_BOOT_UPD_FRM_USB) || \
				(boot_reason == COMM_UBOOT_BOOT_FUNC_BOOT_UPD_FRM_ETH)){
		// Update by USB/ETH: firmware will be loaded by loader.
		ret = nvt_handle_update_fw_by_usb_eth();
		if (ret < 0)
			return ret;
	} else if (nvt_detect_fw_tbin() >= 0) {
		ret = nvt_handle_fw_tbin();
		if (ret < 0)
			return ret;

		#ifndef CONFIG_NVT_LINUX_RAMDISK_BOOT
		char cmdline[512] = {0};
		char buf[255] = {0};
		/*
		 * To handle bootargs expanding for the kernel /proc/cmdline and uboot mtdids env setting
		 */
		sprintf(buf,"%s ",getenv("bootargs"));
		strcat(cmdline, buf);
		ret = nvt_part_config((char *)cmdline, NULL);
		if (ret < 0)
			return ret;

		setenv("bootargs",cmdline);
		#endif /* CONFIG_NVT_LINUX_RAMDISK_BOOT */
	} else if (boot_reason == COMM_UBOOT_BOOT_FUNC_BOOT_FORMAT_ROOTFS){
		// Format R/W rootfs, we should check if this partition is existed firstly.
		ret = nvt_handle_format_rw_rootfs();
		if (ret < 0)
			return ret;
	} else if (boot_reason == COMM_UBOOT_BOOT_FUNC_BOOT_RECOVERY_SYS){
		// Recovery system if loader send event to do system recovery. (EMMC boot only)
		ret = nvt_handle_recovery_sys();
		if (ret < 0)
			return ret;
	} else {
		ret = nvt_handle_fw_abin();
		if (ret < 0)
			return ret;
	}

	return 0;
}
#endif /* CONFIG_NVT_LINUX_AUTODETECT */

#if defined(CONFIG_NVT_LINUX_EMMC_BOOT)
static int nvt_emmc_init(void)
{
	int ret = 0;
	char command[128] = {0};

	/* Switch to emmc bus and user partition access config */
	sprintf(command, "mmc dev %d", CONFIG_NVT_IVOT_EMMC);
	ret = run_command(command, 0);
	if (ret < 0)
		return ret;
	sprintf(command, "mmc partconf %d 1 1 0", CONFIG_NVT_IVOT_EMMC);
	ret = run_command(command, 0);

	return ret;
}
#endif /* CONFIG_NVT_LINUX_EMMC_BOOT */
/**
 * @brief misc_init_r - To do nvt update and board init.
 *
 * @return 0
 */
int misc_init_r(void)
{
	int ret = 0;

	printf("%sFirmware name: %s %s %s %s \n", ANSI_COLOR_YELLOW,
						  get_nvt_bin_name(NVT_BIN_NAME_TYPE_FW),
						  get_nvt_bin_name(NVT_BIN_NAME_TYPE_RUNFW),
						  get_nvt_bin_name(NVT_BIN_NAME_TYPE_MODELEXT),
						  ANSI_COLOR_RESET);
#if defined(CONFIG_NVT_LINUX_EMMC_BOOT)
	ret = nvt_emmc_init();
	if (ret < 0)
		return ret;
#endif /* CONFIG_NVT_LINUX_EMMC_BOOT */
#ifdef CONFIG_NVT_LINUX_AUTOLOAD
	#ifdef CONFIG_NVT_LINUX_AUTODETECT
	ret = nvt_handle_fw_auto();
	if (ret < 0)
		return ret;
	#else /* !CONFIG_NVT_LINUX_AUTODETECT */
	#ifdef CONFIG_NVT_LINUX_SPINAND_BOOT
	ret = nvt_handle_fw_abin();
	if (ret < 0)
		return ret;
	#else /* !CONFIG_NVT_LINUX_SPINAND_BOOT */
	ret = nvt_handle_fw_tbin();
	if (ret < 0)
		return ret;
	#endif /* CONFIG_NVT_LINUX_SPINAND_BOOT */
	#endif /* CONFIG_NVT_LINUX_AUTODETECT */
#else /* !CONFIG_NVT_LINUX_AUTOLOAD */
	/* FIXME: To do customized boot */
	ret = nvt_fdt_init(true);
	if (ret < 0) {
		printf("modelext init fail\n");
		return ret;
	}
#endif /* CONFIG_NVT_LINUX_AUTOLOAD */

	/* Why to do this again?
	 * This is because modelext maybe updated, we should init again here.
	 */
	ret = nvt_board_init();
	if (ret < 0) {
		printf("board init fail\n");
		return ret;
	}

	return 0;
}

/*
 * dram init.
 */
void dram_init_banksize(void)
{
	gd->bd->bi_dram[0].start = PHYS_SDRAM_1;
	gd->bd->bi_dram[0].size = PHYS_SDRAM_1_SIZE;
}

/*
 * get_board_rev() - get board revision
 */
u32 get_board_rev(void)
{
	return 0;
}

int board_late_init(void)
{
	return 0;
}
