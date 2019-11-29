/*
 *  nt72172_emmc_tk_android_img.c
 *
 *  Author:	Alvin lin
 *  Created:	Feb 2, 2014
 *  Copyright:	Novatek Inc.
 *
 */

#if defined(CONFIG_ANDROID_AB_UPDATE)
static struct part_parm nvt_part_parm_tbl[MAX_ALLOW_PART_NUM] = {
	{
		//binary partition
		.part_name = "bootloader_a",
		.sectors = (24 * 1024 * 2),//12MB, count in sectors
	},
	{
		//binary partition
		.part_name = "bootloader_b",
		.sectors = (24 * 1024 * 2),//12MB, count in sectors
	},
	{
		//ker0
		.part_name = "boot_a",
		.sectors = (24 * 1024 * 2),//24MB, count in sectors
	},
	{
		//ker1
		.part_name = "boot_b",
		.sectors = (24 * 1024 * 2),//24MB, count in sectors
	},
	{
		//misc
		.part_name = "misc",
		.sectors = (8 * 1024 * 2),//2MB, count in sectors
	},
	{
		//eeprom
		.part_name = "eeprom",
		.sectors = (16 * 1024 * 2),
	},
	{
		//tvos
		.part_name = "tvos",
		.sectors = (5 * 1024 * 2),
	},
	{
		//tvconfig
		.part_name = "tvconfig",
		.sectors = (100 * 1024 * 2),
	},
	{
		//tvdatabase
		.part_name = "tvdatabase",
		.sectors = (8 * 1024 * 2),
	},
	{
		//logofs
		.part_name = "logofs",
		.sectors = (20 * 1024 * 2),
	},
	{
		//logo
		.part_name = "logo",
		.sectors = (16 * 1024 * 2),
	},
	{
		//hsdtv
		.part_name = "hsdtv",
		.sectors = (20 * 1024 * 2),
	},
	{
		//vendor_a
		.part_name = "vendor_a",
		.sectors = (400 * 1024 * 2),
	},
	{
		//vendor_b
		.part_name = "vendor_b",
		.sectors = (400 * 1024 * 2),
	},
	{
		//opera_a
		.part_name = "opera_a",
		.sectors = (512 * 1024 * 2),
	},
	{
		//opera_b
		.part_name = "opera_b",
		.sectors = (512 * 1024 * 2),
	},
	{
		//apdat
		.part_name = "apdat",
		.sectors = (16 * 1024 * 2),
	},
	{
		//system_a
		.part_name = "system_a",
		.sectors = (2048 * 1024 * 2),
	},
	{
		//system_b
		.part_name = "system_b",
		.sectors = (2048 * 1024 * 2),
	},
	{
		//user
		.part_name = "data",
		.sectors = 0,
	},

};

static nvt_emmc_image_info image_info[] = {
	{
		//mloader = xboot0(16KB) + sbin(2KB) + stbc(2KB) + xboot1(32KB)
		.user_part_name = EMMC_BOOT0_PART_NAME,
		.image_name = "xboot",
		.default_name = "xboot.bin",
		.part_type = PART_TYPE_BOOT1,
		.image_help = nvt_emmc_image_help,
		.read_img = nvt_emmc_read_img,
		.write_img = nvt_emmc_write_img,
		.erase_img = nvt_emmc_erase_img,
		.part_offset = 0,
		.size = 512
	},
	{
		//ddr cfg 24K * 30
		.user_part_name = EMMC_BOOT0_PART_NAME,
		.image_name = "ddrcfg",
		.default_name = "673_ddr_cfg.ddr",
		.part_type = PART_TYPE_BOOT1,
		.image_help = nvt_emmc_image_help,
		.read_img = nvt_emmc_read_img,
		.write_img = nvt_emmc_write_img,
		.erase_img = nvt_emmc_erase_img,
		.part_offset = 512,
		.size = 1440 
	},
	{
		//ddr buddy 16KB
		.user_part_name = EMMC_BOOT0_PART_NAME,
		.image_name = "ddrbud",
		.default_name = "673_ddr_cfg.bud",
		.part_type = PART_TYPE_BOOT1,
		.image_help = nvt_emmc_image_help,
		.read_img = nvt_emmc_read_img,
		.write_img = nvt_emmc_write_img,
		.erase_img = nvt_emmc_erase_img,
		.part_offset = 1952,
		.size = 32 
	},
	{
		//stbc
		.user_part_name = "bootloader_a",
		.image_name = "stbc",
		.default_name = "stbc_emmc.bin",
		.part_type = PART_TYPE_BIN,
		.image_help = nvt_emmc_image_help,
		.read_img = nvt_emmc_read_img,
		.write_img = nvt_emmc_write_img,
		.erase_img = nvt_emmc_erase_img,
		.part_offset = 0,
		.size = 1024
	},
	{
		//edid
		.user_part_name = "bootloader_a",
		.image_name = "edid",
		.default_name = "edid.bin",
		.part_type = PART_TYPE_BIN,
		.image_help = nvt_emmc_image_help,
		.read_img = nvt_emmc_read_img,
		.write_img = nvt_emmc_write_img,
		.erase_img = nvt_emmc_erase_img,
		.part_offset = 1024,
		.size = 1024
	},
	{
		//uboot
		.user_part_name = "bootloader_a",
		.image_name = "uboot",
		.default_name = "u-boot.img",
		.part_type = PART_TYPE_BIN,
		.image_help = nvt_emmc_image_help,
		.read_img = nvt_emmc_read_img,
		.write_img = nvt_emmc_write_img,
		.erase_img = nvt_emmc_erase_img,
		.part_offset = 2048,
		.size = 1280
	},
	{
		//uboot data
		.user_part_name = "bootloader_a",
		.image_name = "ubootdat",
		.default_name = "ubootdat.bin",
		.part_type = PART_TYPE_BIN,
		.image_help = nvt_emmc_image_help,
		.read_img = nvt_emmc_read_img,
		.write_img = nvt_emmc_write_img,
		.erase_img = nvt_emmc_erase_img,
		.part_offset = 3328 ,
		.size = 16
	},
	{
		//xboot data
		.user_part_name = "bootloader_a",
		.image_name = "xbootdat",
		.default_name = "xbootdat.bin",
		.part_type = PART_TYPE_BIN,
		.image_help = nvt_emmc_image_help,
		.read_img = nvt_emmc_read_img,
		.write_img = nvt_emmc_write_img,
		.erase_img = nvt_emmc_erase_img,
		.part_offset = 3344 ,
		.size = 128
	},
	{
		//fdt0
		.user_part_name = "bootloader_a",
		.image_name = "fdt0",
		.default_name = "nvtca53_673_android.dtb.img",
		.part_type = PART_TYPE_BIN,
		.image_help = nvt_emmc_image_help,
		.read_img = nvt_emmc_read_img,
		.write_img = nvt_emmc_write_fdt_img,
		.erase_img = nvt_emmc_erase_img,
		.part_offset = 12279,
		.size = 1024
	},
	{
		//stbc data
		.user_part_name = "bootloader_b",
		.image_name = "stbcdat_bk",
		.default_name = "stbcdat.bin",
		.part_type = PART_TYPE_BIN,
		.image_help = nvt_emmc_image_help,
		.read_img = nvt_emmc_read_img,
		.write_img = nvt_emmc_write_img,
		.erase_img = nvt_emmc_erase_img,
		.part_offset = 12152,
		.size = 1
	},
	{
		//fdt1
		.user_part_name = "bootloader_b",
		.image_name = "fdt1",
		.default_name = "nvtca53_673_android.dtb.img",
		.part_type = PART_TYPE_BIN,
		.image_help = nvt_emmc_image_help,
		.read_img = nvt_emmc_read_img,
		.write_img = nvt_emmc_write_fdt_img,
		.erase_img = nvt_emmc_erase_img,
		.part_offset = 12279,
		.size = 1024
	},
#if defined(CONFIG_OF_LIBFDT_OVERLAY)
	{
		//fdt_base
		.user_part_name = "boot1",
		.image_name = "fdt_base",
		.default_name = "base.dtb",
		.part_type = PART_TYPE_BIN,
		.image_help = nvt_emmc_image_help,
		.read_img = nvt_emmc_read_img,
		.write_img = nvt_emmc_write_img,
		.erase_img = nvt_emmc_erase_img,
		.part_offset = 13303,
		.size = 2048
	},
	{
		//fdt_overlay
		.user_part_name = "boot1",
		.image_name = "fdt_overlay",
		.default_name = "overlay.dtb",
		.part_type = PART_TYPE_BIN,
		.image_help = nvt_emmc_image_help,
		.read_img = nvt_emmc_read_img,
		.write_img = nvt_emmc_write_img,
		.erase_img = nvt_emmc_erase_img,
		.part_offset = 15351,
		.size = 2048
	},
#endif//CONFIG_OF_LIBFDT_OVERLAY

	{
		//kernel 0
		.user_part_name = "boot_a",
		.image_name = "ker0",
		.default_name = "boot.img",
		.part_type = PART_TYPE_BIN,
		.image_help = nvt_emmc_image_help,
		.read_img = nvt_emmc_read_img,
		.write_img = nvt_emmc_write_img_and_update_bootarg,
		.erase_img = nvt_emmc_erase_img,
		.part_offset = 0,
		.size = 49152
	},
	{
		//kernel 1
		.user_part_name = "boot_b",
		.image_name = "ker1",
		.default_name = "boot.img",
		.part_type = PART_TYPE_BIN,
		.image_help = nvt_emmc_image_help,
		.read_img = nvt_emmc_read_img,
		.write_img = nvt_emmc_write_img_and_update_bootarg,
		.erase_img = nvt_emmc_erase_img,
		.part_offset = 0,
		.size = 49152
	},
	{
		//misc
		.user_part_name = "misc",
		.image_name = "misc",
		.default_name = "misc.img",
		.part_type = PART_TYPE_BIN,
		.image_help = nvt_emmc_image_help,
		.read_img = nvt_emmc_read_img,
		.write_img = nvt_emmc_write_img,
		.erase_img = nvt_emmc_erase_img,
		.part_offset = 0,
		.size = 16384
	},
	{
		//eeprom
		.user_part_name = "eeprom",
		.image_name = "eeprom",
		.default_name = "eeprom.img",
		.part_type = PART_TYPE_BIN,
		.image_help = nvt_emmc_image_help,
		.read_img = nvt_emmc_read_img,
		.write_img = nvt_emmc_write_img,
		.erase_img = nvt_emmc_erase_img,
		.part_offset = 0,
		.size = 32768
	},
	{
		//tvos
		.user_part_name = "tvos",
		.image_name = "tvos",
		.default_name = "tvos.img",
		.part_type = PART_TYPE_FS,
		.image_help = nvt_emmc_image_help,
		.read_img = nvt_emmc_read_img,
		.write_img = nvt_emmc_write_img,
		.erase_img = nvt_emmc_erase_img,
		.part_offset = 0,
		.size = 0
	},
	{
		//tvconfig
		.user_part_name = "tvconfig",
		.image_name = "tvconfig",
		.default_name = "tvconfig.img",
		.part_type = PART_TYPE_FS,
		.image_help = nvt_emmc_image_help,
		.read_img = nvt_emmc_read_img,
		.write_img = nvt_emmc_write_img,
		.erase_img = nvt_emmc_erase_img,
		.part_offset = 0,
		.size = 0
	},
	{
		//tvdatabase
		.user_part_name = "tvdatabase",
		.image_name = "tvdatabase",
		.default_name = "tvdatabase.img",
		.part_type = PART_TYPE_FS,
		.image_help = nvt_emmc_image_help,
		.read_img = nvt_emmc_read_img,
		.write_img = nvt_emmc_write_img,
		.erase_img = nvt_emmc_erase_img,
		.part_offset = 0,
		.size = 0
	},
	{
		//logofs
		.user_part_name = "logofs",
		.image_name = "logofs",
		.default_name = "logofs.img",
		.part_type = PART_TYPE_FS,
		.image_help = nvt_emmc_image_help,
		.read_img = nvt_emmc_read_img,
		.write_img = nvt_emmc_write_img,
		.erase_img = nvt_emmc_erase_img,
		.part_offset = 0,
		.size = 0
	},
	{
		//logo
		.user_part_name = "logo",
		.image_name = "logo",
		.default_name = "logo.img",
		.part_type = PART_TYPE_BIN,
		.image_help = nvt_emmc_image_help,
		.read_img = nvt_emmc_read_img,
		.write_img = nvt_emmc_write_img,
		.erase_img = nvt_emmc_erase_img,
		.part_offset = 0,
		.size = 32768
	},
	{
		//hsdtv
		.user_part_name = "hsdtv",
		.image_name = "hsdtv",
		.default_name = "hsdtv.img",
		.part_type = PART_TYPE_FS,
		.image_help = nvt_emmc_image_help,
		.read_img = nvt_emmc_read_img,
		.write_img = nvt_emmc_write_img,
		.erase_img = nvt_emmc_erase_img,
		.part_offset = 0,
		.size = 0
	},
	{
		//cache
		.user_part_name = "cache",
		.image_name = "cache",
		.default_name = "cache.img",
		.part_type = PART_TYPE_FS,
		.image_help = nvt_emmc_image_help,
		.read_img = nvt_emmc_read_img,
		.write_img = nvt_emmc_write_img,
		.erase_img = nvt_emmc_erase_img,
		.part_offset = 0,
		.size = 0
	},
	{
		//vendor
		.user_part_name = "vendor_a",
		.image_name = "vendor0",
		.default_name = "vendor.img",
		.part_type = PART_TYPE_FS,
		.image_help = nvt_emmc_image_help,
		.read_img = nvt_emmc_read_img,
		.write_img = nvt_emmc_write_img,
		.erase_img = nvt_emmc_erase_img,
		.part_offset = 0,
		.size = 0
	},
	{
		//vendor
		.user_part_name = "vendor_b",
		.image_name = "vendor1",
		.default_name = "vendor.img",
		.part_type = PART_TYPE_FS,
		.image_help = nvt_emmc_image_help,
		.read_img = nvt_emmc_read_img,
		.write_img = nvt_emmc_write_img,
		.erase_img = nvt_emmc_erase_img,
		.part_offset = 0,
		.size = 0
	},
	{
		//opera
		.user_part_name = "opera_a",
		.image_name = "opera0",
		.default_name = "opera.img",
		.part_type = PART_TYPE_FS,
		.image_help = nvt_emmc_image_help,
		.read_img = nvt_emmc_read_img,
		.write_img = nvt_emmc_write_img,
		.erase_img = nvt_emmc_erase_img,
		.part_offset = 0,
		.size = 0
	},
	{
		//opera
		.user_part_name = "opera_b",
		.image_name = "opera1",
		.default_name = "opera.img",
		.part_type = PART_TYPE_FS,
		.image_help = nvt_emmc_image_help,
		.read_img = nvt_emmc_read_img,
		.write_img = nvt_emmc_write_img,
		.erase_img = nvt_emmc_erase_img,
		.part_offset = 0,
		.size = 0
	},
	{
		//apdat
		.user_part_name = "apdat",
		.image_name = "apdat",
		.default_name = "apdat.img",
		.part_type = PART_TYPE_FS,
		.image_help = nvt_emmc_image_help,
		.read_img = nvt_emmc_read_img,
		.write_img = nvt_emmc_write_img,
		.erase_img = nvt_emmc_erase_img,
		.part_offset = 0,
		.size = 0

	},
	{
		//system
		.user_part_name = "system_a",
		.image_name = "system0",
		.default_name = "system.img",
		.part_type = PART_TYPE_FS,
		.image_help = nvt_emmc_image_help,
		.read_img = nvt_emmc_read_img,
		.write_img = nvt_emmc_write_img,
		.erase_img = nvt_emmc_erase_img,
		.part_offset = 0,
		.size = 0
	},
	{
		//system
		.user_part_name = "system_b",
		.image_name = "system1",
		.default_name = "system.img",
		.part_type = PART_TYPE_FS,
		.image_help = nvt_emmc_image_help,
		.read_img = nvt_emmc_read_img,
		.write_img = nvt_emmc_write_img,
		.erase_img = nvt_emmc_erase_img,
		.part_offset = 0,
		.size = 0
	},
	{
		//user
		.user_part_name = "data",
		.image_name = "data",
		.default_name = "userdata.img",
		.part_type = PART_TYPE_FS,
		.image_help = nvt_emmc_image_help,
		.read_img = nvt_emmc_read_img,
		.write_img = nvt_emmc_write_img,
		.erase_img = nvt_emmc_erase_img,
		.part_offset = 0,
		.size = 0
	},
};	

#else

static struct part_parm nvt_part_parm_tbl[MAX_ALLOW_PART_NUM] = {
	{
		//binary partition
		.part_name = "boot0",
		.sectors = (24 * 1024 * 2),//24MB, count in sectors
	},
	{
		//binary partition
		.part_name = "boot1",
		.sectors = (24 * 1024 * 2),//24MB, count in sectors
	},
	{
		//ker0
		.part_name = "ker0",
		.sectors = (24 * 1024 * 2),//24MB, count in sectors
	},
	{
		//ker1
		.part_name = "ker1",
		.sectors = (48 * 1024 * 2),//48MB, count in sectors
	},
	{
		//misc
		.part_name = "misc",
		.sectors = (8 * 1024 * 2),//8MB, count in sectors
	},
	{
		//eeprom
		.part_name = "eeprom",
		.sectors = (1 * 1024 * 2),
	},
    {
        //panel param
        .part_name = "panelparam",
        .sectors = (10 * 1024 * 2),
    },
	{
		//device info
		.part_name = "deviceinfo",
		.sectors = (5 * 1024 * 2),
	},
	{
		//tvos
		.part_name = "tvos",
		.sectors = (5 * 1024 * 2),
	},
	{
		//tvconfig
		.part_name = "tvconfig",
		.sectors = (100 * 1024 * 2),
	},
	{
		//tvdatabase
		.part_name = "tvdatabase",
		.sectors = (8 * 1024 * 2),
	},
	{
		//logofs
		.part_name = "logofs",
		.sectors = (20 * 1024 * 2),
	},
	{
		//logo
		.part_name = "logo",
		.sectors = (16 * 1024 * 2),
	},
	{
		//hsdtv
		.part_name = "hsdtv",
		.sectors = (20 * 1024 * 2),
	},
	{
		//cache
		.part_name = "cache",
		.sectors = (500 * 1024 * 2),
	},
	{
		//vendor
		.part_name = "vendor",
		.sectors = (400 * 1024 * 2),
	},
	{
		//ota
		.part_name = "hsota",
		.sectors = (512 * 1024 * 2),
	},
	{
		//apdat
		.part_name = "apdat",
		.sectors = (16 * 1024 * 2),
	},
	{
		//system
		.part_name = "system",
		.sectors = (2048 * 1024 * 2),
	},
	{
		//user
		.part_name = "data",
		.sectors = 0,
	},

};

static nvt_emmc_image_info image_info[] = {
	{
		//mloader = xboot0(16KB) + sbin(2KB) + stbc(2KB) + xboot1(32KB)
		.user_part_name = EMMC_BOOT0_PART_NAME,
		.image_name = "xboot",
		.default_name = "xboot.bin",
		.part_type = PART_TYPE_BOOT1,
		.image_help = nvt_emmc_image_help,
		.read_img = nvt_emmc_read_img,
		.write_img = nvt_emmc_write_img,
		.erase_img = nvt_emmc_erase_img,
		.part_offset = 0,
		.size = 512
	},
	{
		//ddr cfg 24K * 30
		.user_part_name = EMMC_BOOT0_PART_NAME,
		.image_name = "ddrcfg",
		.default_name = "673_ddr_cfg.ddr",
		.part_type = PART_TYPE_BOOT1,
		.image_help = nvt_emmc_image_help,
		.read_img = nvt_emmc_read_img,
		.write_img = nvt_emmc_write_img,
		.erase_img = nvt_emmc_erase_img,
		.part_offset = 512,
		.size = 1440
	},
	{
		//ddr buddy 16KB
		.user_part_name = EMMC_BOOT0_PART_NAME,
		.image_name = "ddrbud",
		.default_name = "673_ddr_cfg.bud",
		.part_type = PART_TYPE_BOOT1,
		.image_help = nvt_emmc_image_help,
		.read_img = nvt_emmc_read_img,
		.write_img = nvt_emmc_write_img,
		.erase_img = nvt_emmc_erase_img,
		.part_offset = 1952,
		.size = 32
	},
	{
		//stbc
		.user_part_name = "boot0",
		.image_name = "stbc",
		.default_name = "stbc_emmc.bin",
		.part_type = PART_TYPE_BIN,
		.image_help = nvt_emmc_image_help,
		.read_img = nvt_emmc_read_img,
		.write_img = nvt_emmc_write_img,
		.erase_img = nvt_emmc_erase_img,
		.part_offset = 0,
		.size = 1024
	},
	{
		//edid
		.user_part_name = "boot0",
		.image_name = "edid",
		.default_name = "edid.bin",
		.part_type = PART_TYPE_BIN,
		.image_help = nvt_emmc_image_help,
		.read_img = nvt_emmc_read_img,
		.write_img = nvt_emmc_write_img,
		.erase_img = nvt_emmc_erase_img,
		.part_offset = 1024,
		.size = 1024
	},
	{
		//uboot
		.user_part_name = "boot0",
		.image_name = "uboot",
		.default_name = "u-boot.img",
		.part_type = PART_TYPE_BIN,
		.image_help = nvt_emmc_image_help,
		.read_img = nvt_emmc_read_img,
		.write_img = nvt_emmc_write_img,
		.erase_img = nvt_emmc_erase_img,
		.part_offset = 2048,
		.size = 1280
	},
	{
		//uboot data
		.user_part_name = "boot0",
		.image_name = "ubootdat",
		.default_name = "ubootdat.bin",
		.part_type = PART_TYPE_BIN,
		.image_help = nvt_emmc_image_help,
		.read_img = nvt_emmc_read_img,
		.write_img = nvt_emmc_write_img,
		.erase_img = nvt_emmc_erase_img,
		.part_offset = 3328 ,
		.size = 16
	},
	{
		//xboot data
		.user_part_name = "boot0",
		.image_name = "xbootdat",
		.default_name = "xbootdat.bin",
		.part_type = PART_TYPE_BIN,
		.image_help = nvt_emmc_image_help,
		.read_img = nvt_emmc_read_img,
		.write_img = nvt_emmc_write_img,
		.erase_img = nvt_emmc_erase_img,
		.part_offset = 3344 ,
		.size = 128
	},
	{
		//fdt0
		.user_part_name = "boot0",
		.image_name = "fdt0",
		.default_name = "n62002_673_android.dtb.img",
		.part_type = PART_TYPE_BIN,
		.image_help = nvt_emmc_image_help,
		.read_img = nvt_emmc_read_img,
		.write_img = nvt_emmc_write_fdt_img,
		.erase_img = nvt_emmc_erase_img,
		.part_offset = 12279,
		.size = 1024
	},
	{
		//secos
		.user_part_name = "boot0",
		.image_name = "secos",
		.default_name = "secos.img",
		.part_type = PART_TYPE_BIN,
		.image_help = nvt_emmc_image_help,
#if defined(CONFIG_NVT_BOOTM_LOAD_SECOS)
		.read_img = nvt_emmc_read_secos_img,
#else
		.read_img = nvt_emmc_read_img,
#endif
		.write_img = nvt_emmc_write_img,
		.erase_img = nvt_emmc_erase_img,
		.part_offset = 13303,
		.size = 16384
	},
	{
		//memdtb
		.user_part_name = "boot1",
		.image_name = "memdtb",
		.default_name = "memory.dtb",
		.part_type = PART_TYPE_BIN,
		.image_help = nvt_emmc_image_help,
		.read_img = nvt_emmc_read_img,
		.write_img = nvt_emmc_write_img,
		.erase_img = nvt_emmc_erase_img,
		.part_offset = 0,
		.size = 512
	},
	{
		//stbc data
		.user_part_name = "boot1",
		.image_name = "stbcdat_bk",
		.default_name = "stbcdat.bin",
		.part_type = PART_TYPE_BIN,
		.image_help = nvt_emmc_image_help,
		.read_img = nvt_emmc_read_img,
		.write_img = nvt_emmc_write_img,
		.erase_img = nvt_emmc_erase_img,
		.part_offset = 12152,
		.size = 1
	},
	{
		//fdt1
		.user_part_name = "boot1",
		.image_name = "fdt1",
		.default_name = "n62002_673_android.dtb.img",
		.part_type = PART_TYPE_BIN,
		.image_help = nvt_emmc_image_help,
		.read_img = nvt_emmc_read_img,
		.write_img = nvt_emmc_write_fdt_img,
		.erase_img = nvt_emmc_erase_img,
		.part_offset = 12279,
		.size = 1024
	},
#if defined(CONFIG_OF_LIBFDT_OVERLAY)
	{
		//fdt_base
		.user_part_name = "boot1",
		.image_name = "fdt_base",
		.default_name = "base.dtb",
		.part_type = PART_TYPE_BIN,
		.image_help = nvt_emmc_image_help,
		.read_img = nvt_emmc_read_img,
		.write_img = nvt_emmc_write_img,
		.erase_img = nvt_emmc_erase_img,
		.part_offset = 13303,
		.size = 2048
	},
	{
		//fdt_overlay
		.user_part_name = "boot1",
		.image_name = "fdt_overlay",
		.default_name = "overlay.dtb",
		.part_type = PART_TYPE_BIN,
		.image_help = nvt_emmc_image_help,
		.read_img = nvt_emmc_read_img,
		.write_img = nvt_emmc_write_img,
		.erase_img = nvt_emmc_erase_img,
		.part_offset = 15351,
		.size = 2048
	},
#endif//CONFIG_OF_LIBFDT_OVERLAY
    {
		//upm kernel
		.user_part_name = "boot1",
		.image_name = "kerupm",
		.default_name = "upm_kernel",
		.part_type = PART_TYPE_BIN,
		.image_help = nvt_emmc_image_help,
		.read_img = nvt_emmc_read_img,
		.write_img = nvt_emmc_write_img,
		.erase_img = nvt_emmc_erase_img,
		.part_offset = 17399,
		.size = 30720
    },
	{
		//kernel 0
		.user_part_name = "ker0",
		.image_name = "ker0",
		.default_name = "boot.img",
		.part_type = PART_TYPE_BIN,
		.image_help = nvt_emmc_image_help,
		.read_img = nvt_emmc_read_img,
		.write_img = nvt_emmc_write_img_and_update_bootarg,
		.erase_img = nvt_emmc_erase_img,
		.part_offset = 0,
		.size = 49152
	},
	{
		//kernel 1
		.user_part_name = "ker1",
		.image_name = "ker1",
		.default_name = "recovery.img",
		.part_type = PART_TYPE_BIN,
		.image_help = nvt_emmc_image_help,
		.read_img = nvt_emmc_read_img,
		.write_img = nvt_emmc_write_img_and_update_bootarg,
		.erase_img = nvt_emmc_erase_img,
		.part_offset = 0,
		.size = 98304
	},
	{
		//misc
		.user_part_name = "misc",
		.image_name = "misc",
		.default_name = "misc.img",
		.part_type = PART_TYPE_BIN,
		.image_help = nvt_emmc_image_help,
		.read_img = nvt_emmc_read_img,
		.write_img = nvt_emmc_write_img,
		.erase_img = nvt_emmc_erase_img,
		.part_offset = 0,
		.size = 16384
	},
	{
		//eeprom
		.user_part_name = "eeprom",
		.image_name = "eeprom",
		.default_name = "eeprom.bin",
		.part_type = PART_TYPE_BIN,
		.image_help = nvt_emmc_image_help,
		.read_img = nvt_emmc_read_img,
		.write_img = nvt_emmc_write_img,
		.erase_img = nvt_emmc_erase_img,
		.part_offset = 0,
		.size = 2048
	},
    {
        //panel param
        .user_part_name = "panelparam",
        .image_name = "panelparam",
        .default_name = "panelparam.bin",
        .part_type = PART_TYPE_BIN,
        .image_help = nvt_emmc_image_help,
        .read_img = nvt_emmc_read_img,
        .write_img = nvt_emmc_write_img,
        .erase_img = nvt_emmc_erase_img,
        .part_offset = 0,
        .size = 20480
    },
	{
        //device info
        .user_part_name = "deviceinfo",
        .image_name = "deviceinfo",
        .default_name = "deviceinfo.bin",
        .part_type = PART_TYPE_BIN,
        .image_help = nvt_emmc_image_help,
        .read_img = nvt_emmc_read_img,
        .write_img = nvt_emmc_write_img,
        .erase_img = nvt_emmc_erase_img,
        .part_offset = 0,
        .size = 10240
	},
	{
		//tvos
		.user_part_name = "tvos",
		.image_name = "tvos",
		.default_name = "tvos.img",
		.part_type = PART_TYPE_FS,
		.image_help = nvt_emmc_image_help,
		.read_img = nvt_emmc_read_img,
		.write_img = nvt_emmc_write_img,
		.erase_img = nvt_emmc_erase_img,
		.part_offset = 0,
		.size = 0
	},
	{
		//tvconfig
		.user_part_name = "tvconfig",
		.image_name = "tvconfig",
		.default_name = "tvconfig.img",
		.part_type = PART_TYPE_FS,
		.image_help = nvt_emmc_image_help,
		.read_img = nvt_emmc_read_img,
		.write_img = nvt_emmc_write_img,
		.erase_img = nvt_emmc_erase_img,
		.part_offset = 0,
		.size = 0
	},
	{
		//tvdatabase
		.user_part_name = "tvdatabase",
		.image_name = "tvdatabase",
		.default_name = "tvdatabase.img",
		.part_type = PART_TYPE_FS,
		.image_help = nvt_emmc_image_help,
		.read_img = nvt_emmc_read_img,
		.write_img = nvt_emmc_write_img,
		.erase_img = nvt_emmc_erase_img,
		.part_offset = 0,
		.size = 0
	},
	{
		//logofs
		.user_part_name = "logofs",
		.image_name = "logofs",
		.default_name = "logofs.img",
		.part_type = PART_TYPE_FS,
		.image_help = nvt_emmc_image_help,
		.read_img = nvt_emmc_read_img,
		.write_img = nvt_emmc_write_img,
		.erase_img = nvt_emmc_erase_img,
		.part_offset = 0,
		.size = 0
	},
	{
		//logo
		.user_part_name = "logo",
		.image_name = "logo",
		.default_name = "logo.img",
		.part_type = PART_TYPE_BIN,
		.image_help = nvt_emmc_image_help,
		.read_img = nvt_emmc_read_img,
		.write_img = nvt_emmc_write_img,
		.erase_img = nvt_emmc_erase_img,
		.part_offset = 0,
		.size = 32768
	},
	{
		//hsdtv
		.user_part_name = "hsdtv",
		.image_name = "hsdtv",
		.default_name = "hsdtv.img",
		.part_type = PART_TYPE_FS,
		.image_help = nvt_emmc_image_help,
		.read_img = nvt_emmc_read_img,
		.write_img = nvt_emmc_write_img,
		.erase_img = nvt_emmc_erase_img,
		.part_offset = 0,
		.size = 0
	},
	{
		//cache
		.user_part_name = "cache",
		.image_name = "cache",
		.default_name = "cache.img",
		.part_type = PART_TYPE_FS,
		.image_help = nvt_emmc_image_help,
		.read_img = nvt_emmc_read_img,
		.write_img = nvt_emmc_write_sparse_img,
		.erase_img = nvt_emmc_erase_img,
		.part_offset = 0,
		.size = 0
	},
	{
		//vendor
		.user_part_name = "vendor",
		.image_name = "vendor",
		.default_name = "vendor.img",
		.part_type = PART_TYPE_FS,
		.image_help = nvt_emmc_image_help,
		.read_img = nvt_emmc_read_img,
		.write_img = nvt_emmc_write_sparse_img,
		.erase_img = nvt_emmc_erase_img,
		.part_offset = 0,
		.size = 0
	},
	{
		//hsota
		.user_part_name = "hsota",
		.image_name = "hsota",
		.default_name = "hsota.img",
		.part_type = PART_TYPE_FS,
		.image_help = nvt_emmc_image_help,
		.read_img = nvt_emmc_read_img,
		.write_img = nvt_emmc_write_img,
		.erase_img = nvt_emmc_erase_img,
		.part_offset = 0,
		.size = 0
	},
	{
		//apdat
		.user_part_name = "apdat",
		.image_name = "apdat",
		.default_name = "apdat.img",
		.part_type = PART_TYPE_FS,
		.image_help = nvt_emmc_image_help,
		.read_img = nvt_emmc_read_img,
		.write_img = nvt_emmc_write_img,
		.erase_img = nvt_emmc_erase_img,
		.part_offset = 0,
		.size = 0

	},
	{
		//system
		.user_part_name = "system",
		.image_name = "system",
		.default_name = "system.img",
		.part_type = PART_TYPE_FS,
		.image_help = nvt_emmc_image_help,
		.read_img = nvt_emmc_read_img,
		.write_img = nvt_emmc_write_sparse_img,
		.erase_img = nvt_emmc_erase_img,
		.part_offset = 0,
		.size = 0
	},
	{
		//user
		.user_part_name = "data",
		.image_name = "data",
		.default_name = "userdata.img",
		.part_type = PART_TYPE_FS,
		.image_help = nvt_emmc_image_help,
		.read_img = nvt_emmc_read_img,
		.write_img = nvt_emmc_write_sparse_img,
		.erase_img = nvt_emmc_erase_img,
		.part_offset = 0,
		.size = 0
	},
};

#endif

