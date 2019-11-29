/*
 *  nt72673_n33005_nand.h
 *
 *  Copyright:	Novatek Inc.
 *
 */

#ifndef _NT72673_N33005_NAND_H
#define _NT72673_N33005_NAND_H

#include <linux/stringify.h>

/*#define CONFIG_SYS_DCACHE_OFF*/
#define CONFIG_SYS_CACHELINE_SIZE 	64
/* Serial & console */
#define CONFIG_SYS_NS16550_SERIAL
/* ns16550 reg in the low bits of cpu reg */
#define CONFIG_SYS_NS16550_CLK		100000000

/* CPU */
#define CONFIG_DISPLAY_CPUINFO
#define CONFIG_CPU_FREQ                         1000 

#define CONFIG_SYS_MMC_SYS_DEV 		0

#define CONFIG_BOARD_LATE_INIT
/*
 * The DRAM Base differs between some models. We cannot use macros for the
 * CONFIG_FOO defines which contain the DRAM base address since they end
 * up unexpanded in include/autoconf.mk .
 *
 * So we have to have this #ifdef #else #endif block for these.
 */
#define CONFIG_SYS_SDRAM_BASE		0x00000000
#define CONFIG_SYS_SDRAM_SIZE           0x20000000
#define CONFIG_SYS_LOAD_ADDR		0x20000000 /* default load address */
#define CONFIG_SYS_TEXT_BASE		0x02800000

#define TZMA_SECURE_SRAM_START          0xeffd0000
#define TZMA_SECURE_SRAM_END            0xefff0000
#define CONFIG_SYS_INIT_SP_ADDR         (TZMA_SECURE_SRAM_END - GENERATED_GBL_DATA_SIZE)

#define CONFIG_NR_DRAM_BANKS		1
#define PHYS_SDRAM_0			CONFIG_SYS_SDRAM_BASE
#define PHYS_SDRAM_0_SIZE		CONFIG_SYS_SDRAM_SIZE 

#define CONFIG_SYS_HUSH_PARSER
#define CONFIG_SETUP_MEMORY_TAGS
#define CONFIG_SYS_CONSOLE_INFO_QUIET
#define CONFIG_ENV_VARS_UBOOT_CONFIG	/* Strongly encouraged */
#define CONFIG_ENV_OVERWRITE			/* Overwrite ethaddr / serial# */
#define CONFIG_CMDLINE_TAG
#define CONFIG_SYS_LONGHELP
#define CONFIG_AUTO_COMPLETE
#define CONFIG_CMDLINE_EDITING


#define CONFIG_SYS_FDT_BUF              (0x8000000)

/* mmc config */
#ifdef CONFIG_MMC
#define CONFIG_GENERIC_MMC
#define CONFIG_MMC_SDMA
#define CONFIG_NVT_ERASE_TOSHIBA_TEST_PATCH
#define CONFIG_NVT_EMMC_ONLY_HOST       /*host controller ignore sd card detection flow*/
#define CONFIG_NVT_EMMC_SET_PLL
#define CONFIG_NVT_EMMC_DDR52
#define CONFIG_NVT_EMMC_MIN_OPCLK       400000
#define CONFIG_NVT_EMMC_MAX_OPCLK   	52000000
#define CONFIG_SUPPORT_EMMC_BOOT 	1
#endif

/* mtd config */
#ifdef CONFIG_MTD
#define CONFIG_MTD_DEVICE
#define CONFIG_MTD_PARTITIONS

#define CONFIG_SYS_NAND_BASE            0xfc048000
#define CONFIG_SYS_MAX_NAND_DEVICE      1
#define CONFIG_SYS_NAND_MAX_CHIPS       1
#define CONFIG_SYS_NAND_SELF_INIT

/*#define CONFIG_CMD_NAND */
#define CONFIG_CMD_NAND_TORTURE
#define CONFIG_CMD_NAND_TRIMFFS
#define CONFIG_CMD_MTDPARTS
#define CONFIG_CMD_UBI      /* UBI-formated MTD partition support */
#define CONFIG_CMD_UBIFS    /* Read-only UBI volume operations */

#define CONFIG_RBTREE       /* required by CONFIG_CMD_UBI */
#define CONFIG_LZO          /* required by CONFIG_CMD_UBIFS */
#endif

#define CONFIG_NVT_SPI
#ifdef CONFIG_NVT_SPI
#define CONFIG_CMD_SF
#define CONFIG_SYS_MAX_SF_DEVICE    1
#define CONFIG_SPI_FLASH_WINBOND    1
#define CONFIG_SPI_FLASH_MACRONIX   1

#define CONFIG_SF_DEFAULT_BUS       0
#define CONFIG_SF_DEFAULT_CS        0
#define CONFIG_SF_DEFAULT_MODE      3
#define CONFIG_SF_DEFAULT_SPEED     12000000
#define CONFIG_SPI_LOCK_CTRL
#define CONFIG_SPI_AHB_DMA_MODE
#endif

/* 64MB of malloc() pool */
#define CONFIG_SYS_MALLOC_LEN		(CONFIG_ENV_SIZE + (64 << 20))

/*
 * Miscellaneous configurable options
 */
#define CONFIG_SYS_CBSIZE	1024	/* Console I/O Buffer Size */
#define CONFIG_SYS_PBSIZE	1024	/* Print Buffer Size */
#define CONFIG_SYS_MAXARGS	16	/* max number of command args */

/* Boot Argument Buffer Size */
#define CONFIG_SYS_BARGSIZE		CONFIG_SYS_CBSIZE

/* standalone support */
#define CONFIG_STANDALONE_LOAD_ADDR	CONFIG_SYS_LOAD_ADDR

/* baudrate */
#define CONFIG_BAUDRATE			115200

/* The stack sizes are set up in start.S using the settings below */
#define CONFIG_STACKSIZE		(256 << 10)	/* 256 KiB */

/* FLASH and environment organization */

#define CONFIG_SYS_NO_FLASH

#define CONFIG_SYS_MONITOR_LEN		(768 << 10)	/* 768 KiB */
#define CONFIG_IDENT_STRING		" Novatek"
#define CONFIG_DISPLAY_BOARDINFO

#define CONFIG_ENV_OFFSET		(544 << 10) /* (8 + 24 + 512) KiB */
#define CONFIG_ENV_SIZE			(128 << 10)	/* 128 KiB */

#define CONFIG_FAT_WRITE	/* enable write access */


#define CONFIG_CONS_INDEX              2       /* UARTB */


#ifdef CONFIG_USB_EHCI_HCD
#define CONFIG_USB_EHCI_NVT
#endif

#define CONFIG_ENV_IS_NOWHERE
#define CONFIG_USB_EHCI_NVT
#define CONFIG_SYS_USB_EHCI_MAX_ROOT_PORTS 1
#define CONFIG_EHCI_IS_TDI
#define CONFIG_RANDOM_UUID
#define CONFIG_NVT_TURNKEY_FWUPDATE    1

#define CONFIG_SYS_FWUPDATE_BUF         0x3200000//set 50MB as firmware update buffer
#define CONFIG_FWUP_RAM_START   CONFIG_SYS_FWUPDATE_BUF

#define CONFIG_VTBL_HASH_START          0xA00000
#define CONFIG_NVT_VHASH_HDR_SIZE       4096
#define CONFIG_SIGN_TOOL_DIR            nt72673_n33005_nand

/*#define CONFIG_DOS_PARTITION*/

#define CONFIG_SYS_BOOTM_LEN 		(16 << 20)

#define CONFIG_EXTRA_ENV_SETTINGS       \
	"kernel_loadaddr=0x3807FC0\0" \
	"memstr=mem=570M@38M mem=292M@2048M\0" \
	"cpus=4\0" \
	"rootfs_mtd=11\0" \
	"verify=y\0" \
	"quiet=y\0" \
        "selinux=n\0" \
        "serialno=nt72673\0" \
	"kparam_addon=no_console_suspend earlyprintk"
	
#define CONFIG_BOOTCOMMAND                      "reboot"
#define CONFIG_BOOTARGS 			""


#endif /* _NT72673_N33005_NAND_H */
