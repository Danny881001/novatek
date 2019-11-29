#ifdef _NT72_NAND_SPECS_H_
#error Please include nt72_nand_specs.h only once.
#endif

#ifndef _NT72_NAND_SPECS_H_
#define _NT72_NAND_SPECS_H_

#include "nt72_nand.h"

#define NT72_NAND_DEFAULT_2K_PAGE \
		.page_per_block = 64, \
		.page_size = 2048, \
		.oob_size = 64, \
		.subpage_size = 512, \
		.flash_width = 8, \
		.ftune = 0x0000C0C0, \
		.bbt_options = NAND_BBT_SCAN2NDPAGE,

static struct nt72_nand_spec g_nt72_nand_specs[] __initdata = {
	/* Samsung */
	{ /* Samsung K9F2G08U0A */
		NT72_NAND_DEFAULT_2K_PAGE
		.num_blocks = 2048,
		.chip_id = 0x9510DAEC,
		.cfg0_read = 0x82103023,
		.cfg0_prog = 0x10103023,
	},
	/* End of Samsung */

	/* Winbond */
	{ /* Winbond W29N01GVSIAA */
		NT72_NAND_DEFAULT_2K_PAGE
		.num_blocks = 1024,
		.chip_id = 0x9580F1EF,
		.cfg0_read = 0x82103021,
		.cfg0_prog = 0x10103021,
	},
	{ /* Winbond W29N01HVSINF */
		NT72_NAND_DEFAULT_2K_PAGE
		.num_blocks = 1024,
		.chip_id = 0x9500F1EF,
		.cfg0_read = 0x82103021,
		.cfg0_prog = 0x10103021,
	},
	{ /* Winbond W29N02GVSIAA */
		NT72_NAND_DEFAULT_2K_PAGE
		.num_blocks = 2048,
		.chip_id = 0x9590DAEF,
		.cfg0_read = 0x82103023,
		.cfg0_prog = 0x10103023,
	},
	{ /* Winbond W29N04GVSIAA */
		NT72_NAND_DEFAULT_2K_PAGE
		.num_blocks = 4096,
		.chip_id = 0x9590DCEF,
		.cfg0_read = 0x82103023,
		.cfg0_prog = 0x10103023,
	},
	{ /* Winbond W29N08GVSIAA */
		NT72_NAND_DEFAULT_2K_PAGE
		.num_blocks = 8192,
		.chip_id = 0x9591D3EF,
		.cfg0_read = 0x82323123,
		.cfg0_prog = 0x10103023,
	},
	/* End of Winbond */

	/* MXIC */
	{ /* MXIC MX30LF1G18AC-TI */
		NT72_NAND_DEFAULT_2K_PAGE
		.num_blocks = 1024,
		.chip_id = 0x9580F1C2,
		.cfg0_read = 0x72100129,
		.cfg0_prog = 0x1010011B,
	},
	{ /* MXIC MX30LF2G18AC-TI */
		NT72_NAND_DEFAULT_2K_PAGE
		.num_blocks = 2048,
		.chip_id = 0x9590DAC2,
		.cfg0_read = 0x82103023,
		.cfg0_prog = 0x10103023,
	},
	{ /* MXIC MX30LF4G18AC-TI */
		NT72_NAND_DEFAULT_2K_PAGE
		.num_blocks = 4096,
		.chip_id = 0x9590DCC2,
		.cfg0_read = 0x82103023,
		.cfg0_prog = 0x10103023,
	},
	{ /* MXIC MX60LF8G18AC-TI */
		NT72_NAND_DEFAULT_2K_PAGE
		.num_blocks = 8192,
		.chip_id = 0x95D1D3C2,
		.cfg0_read = 0x82213123,
		.cfg0_prog = 0x10103023,
	},
	{ /* MXIC MX30UF1G18AC-TI (1.8V) */
		NT72_NAND_DEFAULT_2K_PAGE
		.num_blocks = 1024,
		.chip_id = 0x1580A1C2,
		.cfg0_read = 0x82103021,
		.cfg0_prog = 0x10103021,
	},
	{ /* MXIC MX30UF2G18AC-TI (1.8V) */
		NT72_NAND_DEFAULT_2K_PAGE
		.num_blocks = 2048,
		.chip_id = 0x1590AAC2,
		.cfg0_read = 0x82103023,
		.cfg0_prog = 0x10103023,
	},
	{ /* MXIC MX30UF4G18AC (1.8V) */
		NT72_NAND_DEFAULT_2K_PAGE
		.num_blocks = 4096,
		.chip_id = 0x1590ACC2,
		.cfg0_read = 0x82103023,
		.cfg0_prog = 0x10103023,
	},
	/* End of MXIC */

	/* MICRON */
	{ /* MICRON MT29F2G08ABAEA WP */
		.page_per_block = 64,
		.page_size = 2048,
		.oob_size = 64,
		.subpage_size = 512,
		.flash_width = 8,
		.num_blocks = 2048,
		.chip_id = 0x9590DA2C,
		.cfg0_read = 0x82223023,
		.cfg0_prog = 0x22223023,
		.ftune = 0x0,
		.bbt_options = 0,
	},
	{ /* MICRON MT29F4G08ABAEA WP */
		.page_per_block = 64,
		.page_size = 4096,
		.oob_size = 16 * 8, /* Spec is 224 */
		.subpage_size = 512,
		.flash_width = 8,
		.num_blocks = 2048,
		.chip_id = 0xA690DC2C,
		.cfg0_read = 0x82223123,
		.cfg0_prog = 0x10103023,
		.ftune = 0x0000C0C0,
		.bbt_options = 0,
	},
	{ /* MICRON MT29F4G08ABADA WP */
		.page_per_block = 64,
		.page_size = 2048,
		.oob_size = 64,
		.subpage_size = 512,
		.flash_width = 8,
		.num_blocks = 4096,
		.chip_id = 0x9590DC2C,
		.cfg0_read = 0x82213123,
		.cfg0_prog = 0x10103023,
		.ftune = 0x0000C0C0,
		.bbt_options = 0,
	},
#ifdef IS_CVT
	{ /* MICRON MT29F8G08ABACA WP */
		.page_per_block = 64,
		.page_size = 4096,
		.oob_size = 16 * 8, /* Spec is 224 */
		.subpage_size = 512,
		.flash_width = 8,
		.num_blocks = 4096,
		.chip_id = 0xA690D32C,
		.cfg0_read = 0x72100123,
		.cfg0_prog = 0x10100113,
		.ftune = 0x0000C0C0,
		.bbt_options = 0,
	},
#endif
#ifdef IS_CVT
	{ /* MICRON MT29F32G08ABAAA WP */
		.page_per_block = 128,
		.page_size = 8192,
		.oob_size = 16 * 16, /* Spec is 448 */
		.subpage_size = 512,
		.flash_width = 8,
		.num_blocks = 4096,
		.chip_id = 0x2700682C,
		.cfg0_read = 0x72100123,
		.cfg0_prog = 0x10100113,
		.ftune = 0x0000C0C0,
		.bbt_options = 0,
	},
#endif
	/* End of MICRON */

	/* Toshiba */
	{ /* Toshiba TC58NVG1S3HTA00 */
		.page_per_block = 64,
		.page_size = 2048,
		.oob_size = 64,
		.subpage_size = 512,
		.flash_width = 8,
		.num_blocks = 2048,
		.chip_id = 0x1590DA98,
		.cfg0_read = 0x82223023,
		.cfg0_prog = 0x22223023,
		.ftune = 0x0,
		.bbt_options = NAND_BBT_SCAN2NDPAGE,
	},
	{ /* Toshiba TC58NVG0S3HTA00 */
		.page_per_block = 64,
		.page_size = 2048,
		.oob_size = 64,
		.subpage_size = 512,
		.flash_width = 8,
		.num_blocks = 1024,
		.chip_id = 0x1580F198,
		.cfg0_read = 0x82223021,
		.cfg0_prog = 0x22223021,
		.ftune = 0x0,
		.bbt_options = NAND_BBT_SCAN2NDPAGE,
	},
	{ /* Toshiba TC58NVG2S0HTA00 */
		.page_per_block = 64,
		.page_size = 4096,
		.oob_size = 128 /* Spec is 256 */,
		.subpage_size = 512,
		.flash_width = 8,
		.num_blocks = 2048,
		.chip_id = 0x2690DC98,
		.cfg0_read = 0x82223023,
		.cfg0_prog = 0x22223023,
		.ftune = 0x0,
		.bbt_options = NAND_BBT_SCAN2NDPAGE,
	},
	/* Toshiba TC58NVG2S3HTA00 */
	{
		.page_per_block = 64,
		.page_size = 2048,
		.oob_size = 64,
		.subpage_size = 512,
		.flash_width = 8,
		.num_blocks = 4096,
		.chip_id = 0x1591DC98,
		.cfg0_read = 0x82223023,
		.cfg0_prog = 0x22223023,
		.ftune = 0x0,
		.bbt_options = NAND_BBT_SCAN2NDPAGE,
	},
	/* End of Toshiba */
};

static const unsigned int g_nr_nt72_nand_specs = ARRAY_SIZE(g_nt72_nand_specs);

#endif /* _NT72_NAND_SPECS_H_ */
