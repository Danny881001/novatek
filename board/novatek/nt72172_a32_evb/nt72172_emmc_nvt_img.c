/*
 *  board/novatek/evb670b/nt72670b_emmc_nvt_img.c
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
		.sectors = (24 * 1024 * 2),//12MB, count in sectors
	},
	{
		//binary partition
		.part_name = "boot1",
		.sectors = (24 * 1024 * 2),//12MB, count in sectors
	},
	{
		//ker0
		.part_name = "ker0",
		.sectors = (24 * 1024 * 2),//24MB, count in sectors
	},
	{
		//ker1
		.part_name = "ker1",
		.sectors = (48 * 1024 * 2),//24MB, count in sectors
	},
	{
		//misc
		.part_name = "misc",
		.sectors = (8 * 1024 * 2),//2MB, count in sectors
	},
	{
		//devcfg
		.part_name = "devcfg",
		.sectors = (10 * 1024 * 2),
	},
	{
		//fs0
		.part_name = "fs0",
		.sectors = (300 * 1024 * 2),
	},
	{
		//fs1
		.part_name = "fs1",
		.sectors = (300 * 1024 * 2),
	},
	{
		//ap0 , android cache partition
		.part_name = "apexe0",
		.sectors = (800 * 1024 * 2),
	},
	{
		//ap1
		.part_name = "apexe1",
		.sectors = (800 * 1024 * 2),
	},
	{
		//ro area
		.part_name = "ro_area",
		.sectors = (500 * 1024 * 2),
	},
	{
		//rw area
		.part_name = "apdat",
		.sectors = 0,//0 means use rest all
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
		.size = 256
	},
	{
		//ddr cfg 48KB + 1 block for sign
		.user_part_name = EMMC_BOOT0_PART_NAME,
		.image_name = "ddrcfg",
		.default_name = "172_ddr_cfg.ddr",
		.part_type = PART_TYPE_BOOT1,
		.image_help = nvt_emmc_image_help,
		.read_img = nvt_emmc_read_img,
		.write_img = nvt_emmc_write_img,
		.erase_img = nvt_emmc_erase_img,
		.part_offset = 256,
		.size = 481 
	},
	{
		//ddr buddy 16KB
		.user_part_name = EMMC_BOOT0_PART_NAME,
		.image_name = "ddrbud",
		.default_name = "172_ddr_cfg.bud",
		.part_type = PART_TYPE_BOOT1,
		.image_help = nvt_emmc_image_help,
		.read_img = nvt_emmc_read_img,
		.write_img = nvt_emmc_write_img,
		.erase_img = nvt_emmc_erase_img,
		.part_offset = 737,
		.size = 32 
	},
	{
		//ddr init 16KB
		.user_part_name = EMMC_BOOT0_PART_NAME,
		.image_name = "ddrinit",
		.default_name = "ddr.init",
		.part_type = PART_TYPE_BOOT1,
		.image_help = nvt_emmc_image_help,
		.read_img = nvt_emmc_read_img,
		.write_img = nvt_emmc_write_img,
		.erase_img = nvt_emmc_erase_img,
		.part_offset = 769,
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
#if defined(CONFIG_NVT_SECURE_BOOT)
		.default_name = "u-boot.bin.sign",
#else
		.default_name = "u-boot.bin",
#endif
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
		//logo
		.user_part_name = "boot0",
		.image_name = "logo",
		.default_name = "logo.bin",
		.part_type = PART_TYPE_BIN,
		.image_help = nvt_emmc_image_help,
		.read_img = nvt_emmc_read_img,
		.write_img = nvt_emmc_write_img,
		.erase_img = nvt_emmc_erase_img,
		.part_offset = 3472,
		.size = 8807
	},
	{
		//fdt0
		.user_part_name = "boot0",
		.image_name = "fdt0",
		.default_name = "nvtca53_172.dtb",
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
		.default_name = "secos.bin",
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
		//vtbl
		.user_part_name = "boot0",
		.image_name = "vtbl",
		.default_name = "vtbl.bin",
		.part_type = PART_TYPE_BIN,
		.image_help = nvt_emmc_image_help,
		.read_img = nvt_emmc_read_img,
#if defined(CONFIG_NVT_SECURE_BOOT)
		.write_img = nvt_emmc_write_vtbl_img,
#else
		.write_img = nvt_emmc_write_img,
#endif
		.erase_img = nvt_emmc_erase_img,
		.part_offset = 29687,
		.size = 64
	},
	{
		//vhash
		.user_part_name = "boot0",
		.image_name = "vhash",
		.default_name = "vhash.bin",
		.part_type = PART_TYPE_BIN,
		.image_help = nvt_emmc_image_help,
		.read_img = nvt_emmc_read_img,
		.write_img = nvt_emmc_write_img,
		.erase_img = nvt_emmc_erase_img,
		.part_offset = 29751,
		.size = 1
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
		//uboot
		.user_part_name = "boot1",
		.image_name = "uboot_bk",
		.default_name = "uboot.bin",
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
		.user_part_name = "boot1",
		.image_name = "ubootdat_bk",
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
		//logo
		.user_part_name = "boot1",
		.image_name = "logo_bk",
		.default_name = "logo.bin",
		.part_type = PART_TYPE_BIN,
		.image_help = nvt_emmc_image_help,
		.read_img = nvt_emmc_read_img,
		.write_img = nvt_emmc_write_img,
		.erase_img = nvt_emmc_erase_img,
		.part_offset = 3344,
		.size = 8807
	},
	{
		//boot music
		.user_part_name = "boot1",
		.image_name = "music_bk",
		.default_name = "music.bin",
		.part_type = PART_TYPE_BIN,
		.image_help = nvt_emmc_image_help,
		.read_img = nvt_emmc_read_img,
		.write_img = nvt_emmc_write_img,
		.erase_img = nvt_emmc_erase_img,
		.part_offset = 12151,
		.size = 1
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
		.default_name = "nvtca53_172.dtb",
		.part_type = PART_TYPE_BIN,
		.image_help = nvt_emmc_image_help,
		.read_img = nvt_emmc_read_img,
		.write_img = nvt_emmc_write_fdt_img,
		.erase_img = nvt_emmc_erase_img,
		.part_offset = 12279,
		.size = 1024
	},
	{
		//kernel 0
		.user_part_name = "ker0",
		.image_name = "ker0",
		.default_name = "uImage",
		.part_type = PART_TYPE_BIN,
		.image_help = nvt_emmc_image_help,
		.read_img = nvt_emmc_read_img,
		.write_img = nvt_emmc_write_img_and_update_bootarg,
		.erase_img = nvt_emmc_erase_img,
		.part_offset = 0,
		.size = 47104
	},
	{
		//kernel 1
		.user_part_name = "ker1",
		.image_name = "ker1",
		.default_name = "uImage",
		.part_type = PART_TYPE_BIN,
		.image_help = nvt_emmc_image_help,
		.read_img = nvt_emmc_read_img,
		.write_img = nvt_emmc_write_img_and_update_bootarg,
		.erase_img = nvt_emmc_erase_img,
		.part_offset = 0,
		.size = 47104
	},
	{
		//boot flag
		.user_part_name = "misc",
		.image_name = "bflag",
		.default_name = "bflag.bin",
		.part_type = PART_TYPE_BIN,
		.image_help = nvt_emmc_image_help,
		.read_img = nvt_emmc_read_img,
		.write_img = nvt_emmc_write_img,
		.erase_img = nvt_emmc_erase_img,
		.part_offset = 0,
		.size = 1
	},
	{
		//update flag
		.user_part_name = "misc",
		.image_name = "uflag",
		.default_name = "uflag.bin",
		.part_type = PART_TYPE_BIN,
		.image_help = nvt_emmc_image_help,
		.read_img = nvt_emmc_read_img,
		.write_img = nvt_emmc_write_img,
		.erase_img = nvt_emmc_erase_img,
		.part_offset = 1,
		.size = 1
	},
	{
		//pptbl 0
		.user_part_name = "misc",
		.image_name = "pptbl0",
		.default_name = "pptbl_all.bin",
		.part_type = PART_TYPE_BIN,
		.image_help = nvt_emmc_image_help,
		.read_img = nvt_emmc_read_img,
		.write_img = nvt_emmc_write_img,
		.erase_img = nvt_emmc_erase_img,
		.part_offset = 2,
		.size = 3
	},
	{
		//pptbl 1
		.user_part_name = "misc",
		.image_name = "pptbl1",
		.default_name = "pptbl_all.bin",
		.part_type = PART_TYPE_BIN,
		.image_help = nvt_emmc_image_help,
		.read_img = nvt_emmc_read_img,
		.write_img = nvt_emmc_write_img,
		.erase_img = nvt_emmc_erase_img,
		.part_offset = 5,
		.size = 3
	},
	{
		//version_info
		.user_part_name = "misc",
		.image_name = "vinfo",
		.default_name = "vinfo.bin",
		.part_type = PART_TYPE_BIN,
		.image_help = nvt_emmc_image_help,
		.read_img = nvt_emmc_read_img,
		.write_img = nvt_emmc_write_img,
		.erase_img = nvt_emmc_erase_img,
		.part_offset = 8,
		.size = 1
	},
	{
		//devcfg
		.user_part_name = "devcfg",
		.image_name = "devcfg",
		.default_name = "devcfg.img",
		.part_type = PART_TYPE_FS,
		.image_help = nvt_emmc_image_help,
		.read_img = nvt_emmc_read_img,
		.write_img = nvt_emmc_write_img,
		.erase_img = nvt_emmc_erase_img,
		.part_offset = 0,
		.size = 18432
	},

	{
		//fs0
		.user_part_name = "fs0",
		.image_name = "fs0",
#if defined(CONFIG_NFSK)
		.default_name = "rootfs_nfsb.img",
#else		
		.default_name = "rootfs.img",
#endif		
		.part_type = PART_TYPE_FS,
		.image_help = nvt_emmc_image_help,
		.read_img = nvt_emmc_read_img,
		.write_img = nvt_emmc_write_img,
		.erase_img = nvt_emmc_erase_img,
		.part_offset = 0,
		.size = 612352
	},
	{
		//fs1
		.user_part_name = "fs1",
		.image_name = "fs1",
		.default_name = "rootfs_lite.img",
		.part_type = PART_TYPE_FS,
		.image_help = nvt_emmc_image_help,
		.read_img = nvt_emmc_read_img,
		.write_img = nvt_emmc_write_img,
		.erase_img = nvt_emmc_erase_img,
		.part_offset = 0,
		.size = 612352

	},
	{
		//ap0
		.user_part_name = "apexe0",
		.image_name = "ap0",
#if defined(CONFIG_NFSK)
		.default_name = "apexe_nfsb.img",
#else		
		.default_name = "board.img",
#endif		
		.part_type = PART_TYPE_FS,
		.image_help = nvt_emmc_image_help,
		.read_img = nvt_emmc_read_img,
		.write_img = nvt_emmc_write_img,
		.erase_img = nvt_emmc_erase_img,
		.part_offset = 0,
		.size = 0
	},
	{
		//ap1
		.user_part_name = "apexe1",
		.image_name = "ap1",
#if defined(CONFIG_NFSK)
		.default_name = "apexe_nfsb.img",
#else		
		.default_name = "board.img",
#endif		
		.part_type = PART_TYPE_FS,
		.image_help = nvt_emmc_image_help,
		.read_img = nvt_emmc_read_img,
		.write_img = nvt_emmc_write_img,
		.erase_img = nvt_emmc_erase_img,
		.part_offset = 0,
		.size = 0
	},
	{
		//ro
		.user_part_name = "ro_area",
		.image_name = "ro_area",
		.default_name = "ro_area.img",
		.part_type = PART_TYPE_FS,
		.image_help = nvt_emmc_image_help,
		.read_img = nvt_emmc_read_img,
		.write_img = nvt_emmc_write_img,
		.erase_img = nvt_emmc_erase_img,
		.part_offset = 0,
		.size = 1003520
	},
	{
		//user data
		.user_part_name = "apdat",
		.image_name = "rw_area",
		.default_name = "rw_area.img",
		.part_type = PART_TYPE_FS,
		.image_help = nvt_emmc_image_help,
		.read_img = nvt_emmc_read_img,
		.write_img = nvt_emmc_write_img,
		.erase_img = nvt_emmc_erase_img,
		.part_offset = 0,
		.size = 0
	}
};	


