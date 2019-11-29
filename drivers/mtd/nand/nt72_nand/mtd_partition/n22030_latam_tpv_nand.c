struct mtd_partition g_nt72_nand_mtd_partitions[] = {
	[0] = {
		.name        = "xboot",
		.offset      = 0,
		.size        = SZ_128K * 4,
	},
	[1] = {
		.name        = "ddrcfg",
		.offset      = MTDPART_OFS_APPEND,
		.size        = SZ_128K * 4,
	},
	[2] = {
		.name        = "ddrbud",
		.offset      = MTDPART_OFS_APPEND,
		.size        = SZ_128K * 3,
	},
	[3] = {
		.name        = "xbootdat",
		.offset      = MTDPART_OFS_APPEND,
		.size        = SZ_128K * 5,
	},
	[4] = {
		.name        = "misc",
		.offset      = MTDPART_OFS_APPEND,
		.size        = SZ_128K * 3,
	},
	[5] = {
		.name        = "stbc",
		.offset      = MTDPART_OFS_APPEND,
		.size        = SZ_128K * 5,
	},
	[6] = {
		.name        = "secos",
		.offset      = MTDPART_OFS_APPEND,
		.size        = SZ_128K * 1,
	},
	[7] = {
		.name        = "uboot",
		.offset      = MTDPART_OFS_APPEND,
		.size        = SZ_128K * 7,
	},
	[8] = {
		.name        = "fdt0",
		.offset      = MTDPART_OFS_APPEND,
		.size        = SZ_128K * 3,
	},
	[9] = {
		.name        = "ker0",
		.offset      = MTDPART_OFS_APPEND,
		.size        = SZ_128K * 30,
	},
	[10] = {
		.name        = "fs0",
		.offset      = MTDPART_OFS_APPEND,
		.size        = SZ_128K * 52,
	},
	[11] = {
		.name        = "ap0",
		.offset      = MTDPART_OFS_APPEND,
		.size        = SZ_128K * 223,
	},
	[12] = {
		.name        = "apdat0",
		.offset      = MTDPART_OFS_APPEND,
		.size        = SZ_128K * 52,
	},
	[13] = {
		.name        = "ap1",
		.offset      = MTDPART_OFS_APPEND,
		.size        = SZ_128K * 223,
	},
	[14] = {
		.name        = "apdat1",
		.offset      = MTDPART_OFS_APPEND,
		.size        = SZ_128K * 52,
	},
	[15] = {
		.name        = "stbcdat",
		.offset      = MTDPART_OFS_APPEND,
		.size        = SZ_128K * 3,
	},
	[16] = {
		.name        = "roarea",
		.offset      = MTDPART_OFS_APPEND,
		.size        = SZ_128K * 350,
	},
};


