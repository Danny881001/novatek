/*
 * Configuration for Versatile Express. Parts were derived from other ARM
 *   configurations.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef __NT72172_A64_TK_ANDROID_H
#define __NT72172_A64_TK_ANDROID_H

/*#define DEBUG*/


#define CONFIG_TK_172_A64_ANDROID

#define CONFIG_REMAKE_ELF

#define CONFIG_SUPPORT_RAW_INITRD

/* MMU Definitions */
#define CONFIG_SYS_CACHELINE_SIZE	64

#define CONFIG_IDENT_STRING		"nt72172_a64_tk_android"

/* Physical Memory Map */
#ifndef CONFIG_NR_DRAM_BANKS
#define CONFIG_NR_DRAM_BANKS		4
#endif
#define CONFIG_SYS_SDRAM_BASE		0x00000000
#define CONFIG_SYS_SDRAM_SIZE		0x20000000

#define CONFIG_BOARD_LATE_INIT
#define PHYS_SDRAM_1_SIZE 		CONFIG_SYS_SDRAM_SIZE

/* Link Definitions */
#define CONFIG_SYS_TEXT_BASE		0x02800000
#define CONFIG_SYS_INIT_SP_ADDR         (CONFIG_SYS_SDRAM_BASE + 0x7fff0)



#define CONFIG_SYS_BOOTM_LEN (64 << 20)      /* Increase max gunzip size */

/* Generic Timer Definitions */
#define COUNTER_FREQUENCY		(0x1800000)	/* 24MHz */

#define CONFIG_CPU_FREQ                         1000 
/* Generic Interrupt Controller Definitions */
#define GICD_BASE			(0xFFD01000)
#define GICC_BASE			(0xFFD02000)

/*
 * The following are general good-enough settings for U-Boot.  We set a
 * large malloc pool as we generally have a lot of DDR, and we opt for
 * function over binary size in the main portion of U-Boot as this is
 * generally easily constrained later if needed.  We enable the config
 * options that give us information in the environment about what board
 * we are on so we do not need to rely on the command prompt.  We set a
 * console baudrate of 115200 and use the default baud rate table.
 */
#define CONFIG_SYS_MALLOC_LEN   (CONFIG_ENV_SIZE + 256*1024)
#define CONFIG_SYS_HUSH_PARSER
#define CONFIG_SYS_CONSOLE_INFO_QUIET
#define CONFIG_BAUDRATE				115200
#define CONFIG_ENV_VARS_UBOOT_CONFIG	/* Strongly encouraged */
#define CONFIG_ENV_OVERWRITE			/* Overwrite ethaddr / serial# */
#define CONFIG_CMDLINE_TAG

/* As stated above, the following choices are optional. */
#define CONFIG_SYS_LONGHELP
#define CONFIG_AUTO_COMPLETE
#define CONFIG_CMDLINE_EDITING

#define CONFIG_SYS_NO_FLASH
/* We set the max number of command args high to avoid HUSH bugs. */
#define CONFIG_SYS_MAXARGS			64

/* Console I/O Buffer Size */
#define CONFIG_SYS_CBSIZE			512
/* Print Buffer Size */
#define CONFIG_SYS_PBSIZE			(CONFIG_SYS_CBSIZE + sizeof(CONFIG_SYS_PROMPT) + 16)
/* Boot Argument Buffer Size */
#define CONFIG_SYS_BARGSIZE			CONFIG_SYS_CBSIZE

#define CONFIG_ZERO_BOOTDELAY_CHECK			/* allow stopping of boot process */
#define CONFIG_SILENT_CONSOLE   1
#define CONFIG_SYS_DEVICE_NULLDEV               1
#define CONFIG_SILENT_CONSOLE_UPDATE_ON_RELOC   1
#define CONFIG_AUTOBOOT_KEYED		1
#define CONFIG_AUTOBOOT_STOP_STR	"~"

/* UART */
#define CONFIG_SYS_NS16550_SERIAL
#define CONFIG_SYS_NS16550_REG_SIZE	(-4)			/* (-) means CPU little endian */
#define CONFIG_SYS_NS16550_MEM32
#define CONFIG_SYS_NS16550_CLK		100000000
#define CONFIG_CONS_INDEX			2				/* Serial Port B */
#define CONFIG_SYS_NS16550_COM2		0xfd091000

#if defined(CONFIG_USB)
#define CONFIG_USB_EHCI_NVT
#endif

/* Miscellaneous configurable options */
#define CONFIG_SYS_LOAD_ADDR		0x20000000

/* Do not preserve environment */
#define CONFIG_ENV_IS_NOWHERE		1
#define CONFIG_ENV_SIZE			0x1000

#ifdef CONFIG_MMC
#define CONFIG_SYS_MMC_SYS_DEV 		0
#define CONFIG_GENERIC_MMC
#define CONFIG_MMC_SDMA
#define CONFIG_NVT_ERASE_TOSHIBA_TEST_PATCH
#define CONFIG_NVT_EMMC_ONLY_HOST       /*host controller ignore sd card detection flow*/
#define CONFIG_NVT_EMMC_SET_PLL
#define CONFIG_NVT_EMMC_DDR52
#define CONFIG_NVT_EMMC_MIN_OPCLK       400000
#define CONFIG_NVT_EMMC_MAX_OPCLK   	52000000
#define CONFIG_SYS_FWUPDATE_BUF         0x3200000//set 50MB as firmware update buffer
#define CONFIG_SYS_FDT_BUF              (0x8000000)
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

#define CONFIG_CMD_NAND
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

/*#define CONFIG_DOS_PARTITION*/

/*fwupdate*/
#define CONFIG_NVT_INGNORE_FSREAD_SIZE_CHECK
#define CONFIG_FWUP_RAM_START	CONFIG_SYS_FWUPDATE_BUF	
#define CONFIG_FWUP_RAM_SIZE			0x4000000
#define DDRA_BASE_ADDR 0x40000000

#define CONFIG_SETUP_MEMORY_TAGS

#define CONFIG_RANDOM_UUID
#define CONFIG_SHA256                   1

#define CONFIG_CMD_MISC 		1
#define CONFIG_CMD_MEMORY 		1
#define ANDROID_RAMDISK_OFFSET_SZ 		(0x2800000)

#define CONFIG_EXTRA_ENV_SETTINGS       \
	"kernel_loadaddr=0x1BFFFC0\0" \
	"kermem=667M@28M,413M@2048M\0" \
	"cpus=4\0" \
	"usb_port=0\0" \
	"usb=/dev/mmcblk0p7\0" \
	"verify=y\0" \
	"quiet=n\0" \
        "selinux=n\0" \
        "serialno=nt7270B\0" \
	"kparam_addon=no_console_suspend androidboot.hardware=nt72566_a64_tk"
	
#define CONFIG_BOOTCOMMAND                      "reboot"
#define CONFIG_BOOTARGS 			""


#endif /* __NT72172_A64_EVB_H */
