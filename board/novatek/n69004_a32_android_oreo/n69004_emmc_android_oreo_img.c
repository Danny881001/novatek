/*
 *  nt72172_emmc_tk_android_img.c
 *
 *  Author:	Alvin lin
 *  Created:	Feb 2, 2014
 *  Copyright:	Novatek Inc.
 *
 */
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
		//param
		.part_name = "param",
		.sectors = (1 * 1024 * 2),
	},
	{
		//panelparam
		.part_name = "panelparam",
		.sectors = (10 * 1024 * 2),
	},
	{
		//bootlogo
		.part_name = "bootlogo",
		.sectors = (4 * 1024 * 2),
	},
	{
		//securestore
		.part_name = "securestore",
		.sectors = (20 * 1024 * 2),
	},
	{
		//factory
		.part_name = "factory",
		.sectors = (100 * 1024 * 2),
	},
	{
		//atv
		.part_name = "atv",
		.sectors = (20 * 1024 * 2),
	},
	{
		//dtv
		.part_name = "dtv",
		.sectors = (80 * 1024 * 2),
	},
	{
		//cache
		.part_name = "cache",
		.sectors = (60 * 1024 * 2),
	},
	{
		//skyworth
		.part_name = "skyworth",
		.sectors = (200 * 1024 * 2),
	},
	{
		//skyota
		.part_name = "skyota",
		.sectors = (600 * 1024 * 2),
	},
	{
		//system
		.part_name = "system",
		.sectors = (800 * 1024 * 2),
	},
	{
		//vendor
		.part_name = "vendor",
		.sectors = (600 * 1024 * 2),
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
		.size = 2048
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
		//fdt0
		.user_part_name = "boot0",
		.image_name = "fdt0",
		.default_name = "n69004_673_android.dtb.img",
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
		.default_name = "n69004_673_android.dtb.img",
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
		//param
		.user_part_name = "param",
		.image_name = "param",
		.default_name = "flasheeprom.bin",
		.part_type = PART_TYPE_BIN,
		.image_help = nvt_emmc_image_help,
		.read_img = nvt_emmc_read_img,
		.write_img = nvt_emmc_write_img,
		.erase_img = nvt_emmc_erase_img,
		.part_offset = 0,
		.size = 2048
	},
	{
		//xboot data
		.user_part_name = "panelparam",
		.image_name = "xbootdat",
		.default_name = "xbootdat.bin",
		.part_type = PART_TYPE_BIN,
		.image_help = nvt_emmc_image_help,
		.read_img = nvt_emmc_read_img,
		.write_img = nvt_emmc_write_img,
		.erase_img = nvt_emmc_erase_img,
		.part_offset = 0 ,
		.size = 128
	},
	{
		//atv
		.user_part_name = "atv",
		.image_name = "atv",
		.default_name = "atv.img",
		.part_type = PART_TYPE_FS,
		.image_help = nvt_emmc_image_help,
		.read_img = nvt_emmc_read_img,
		.write_img = nvt_emmc_write_img,
		.erase_img = nvt_emmc_erase_img,
		.part_offset = 0,
		.size = 0
	},
	{
		//factory
		.user_part_name = "factory",
		.image_name = "factory",
		.default_name = "factory.img",
		.part_type = PART_TYPE_FS,
		.image_help = nvt_emmc_image_help,
		.read_img = nvt_emmc_read_img,
		.write_img = nvt_emmc_write_img,
		.erase_img = nvt_emmc_erase_img,
		.part_offset = 0,
		.size = 0
	},
	{
		//dtv
		.user_part_name = "dtv",
		.image_name = "dtv",
		.default_name = "dtv.img",
		.part_type = PART_TYPE_FS,
		.image_help = nvt_emmc_image_help,
		.read_img = nvt_emmc_read_img,
		.write_img = nvt_emmc_write_img,
		.erase_img = nvt_emmc_erase_img,
		.part_offset = 0,
		.size = 0
	},
	{
		//skyworth
		.user_part_name = "skyworth",
		.image_name = "skyworth",
		.default_name = "skyworth.img",
		.part_type = PART_TYPE_FS,
		.image_help = nvt_emmc_image_help,
		.read_img = nvt_emmc_read_img,
		.write_img = nvt_emmc_write_img,
		.erase_img = nvt_emmc_erase_img,
		.part_offset = 0,
		.size = 0
	},
	{
		//skyota
		.user_part_name = "skyota",
		.image_name = "skyota",
		.default_name = "skyota.img",
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
		.user_part_name = "vendor",
		.image_name = "vendor",
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
		//system
		.user_part_name = "system",
		.image_name = "system",
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


