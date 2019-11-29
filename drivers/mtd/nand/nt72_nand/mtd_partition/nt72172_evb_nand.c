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
		.size        =  SZ_128K * 1,
	},

	[7] = {
		.name        = "uboot",
		.offset      = MTDPART_OFS_APPEND,
		.size        = SZ_128K * 7,
	},
	[8] = {
		.name        = "stbcdat",
		.offset      = MTDPART_OFS_APPEND,
		.size        = SZ_128K * 3,
	},
	[9] = {
		.name        = "fdt0",
		.offset      = MTDPART_OFS_APPEND,
		.size        = SZ_128K * 3,
	},
	[10] = {
		.name        = "fdt1",
		.offset      = MTDPART_OFS_APPEND,
		.size        = SZ_128K * 3,
	},
	[11] = {
		.name        = "ker0",
		.offset      = MTDPART_OFS_APPEND,
		.size        = SZ_128K * 43,
	},
	[12] = {
		.name        = "ker1",
		.offset      = MTDPART_OFS_APPEND,
		.size        = SZ_128K * 43,
	},
	[13] = {
		.name        = "fs0",
		.offset      = MTDPART_OFS_APPEND,
		.size        = SZ_128K * 72,
	},
	[14] = {
		.name        = "fs1",
		.offset      = MTDPART_OFS_APPEND,
		.size        = SZ_128K * 72,
	},
	[15] = {
		.name        = "ap0",
		.offset      = MTDPART_OFS_APPEND,
		.size        = SZ_128K * 400,
	},
	[16] = {
		.name        = "ap1",
		.offset      = MTDPART_OFS_APPEND,
		.size        = SZ_128K * 400,
	},
	[17] = {
		.name        = "apdat0",
		.offset      = MTDPART_OFS_APPEND,
		.size        = SZ_128K * 52,
	},
	[18] = {
		.name        = "pridat0",
		.offset      = MTDPART_OFS_APPEND,
		.size        = SZ_128K * 25,
	},
};


