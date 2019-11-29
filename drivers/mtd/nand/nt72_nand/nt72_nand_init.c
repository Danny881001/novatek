#include "include/nt72_nand.h"
#include "include/nt72_nand_specs.h"

#if defined(IS_KERNEL) && !defined(STBC_ECC_SWITCH)
static void __init nt72_nand_init_regs(struct nt72_nand_info *info)
#else
void nt72_nand_init_regs(struct nt72_nand_info *info)
#endif
{
	u32 _reg;
	struct nt72_nand_params *p = &(info->nand_params);

	/* init REG_NFC_XD_CFG */
	_reg = nfc_read32(REG_NFC_XD_CFG);
	nvt_bclear(_reg, NFC_AHB_MASTER_MSK);
	nvt_bset(_reg, NFC_AHB_MASTER_16_8_4BEAT);
	p->regs.xd_cfg = _reg;

	/* init REG_NFC_SYSCTRL */
	_reg = NFC_SYSCTRL_EXTRA_SIZE(p->ecc.extra_size);
	switch (p->page_size) {
	case 2048:
		_reg |= NFC_SYSCTRL_PAGE_2048;
		break;
	case 4096:
		_reg |= NFC_SYSCTRL_PAGE_4096;
		break;
	case 8192:
		_reg |= NFC_SYSCTRL_PAGE_8192;
		break;
	default:
		nfc_err("nfc_err: REG_NFC_SYSCTRL illegal page size\n");
		break;
	}
	switch (p->subpage_size) {
	case 512:
		_reg |= NFC_SYSCTRL_SUBPAGE_512;
		break;
	case 1024:
		_reg |= NFC_SYSCTRL_SUBPAGE_1024;
		break;
	default:
		nfc_err("nfc_err: REG_NFC_SYSCTRL illegal subpage size\n");
		break;
	}
	/* block size */
	switch (p->page_size * p->nr_page_per_block / 1024) {
	case 128:
		_reg |= NFC_SYSCTRL_BLK_128K;
		break;
	case 256:
		_reg |= NFC_SYSCTRL_BLK_256K;
		break;
	case 512:
		_reg |= NFC_SYSCTRL_BLK_512K;
		break;
	case 1024:
		_reg |= NFC_SYSCTRL_BLK_1M;
		break;
	default:
		nfc_err("nfc_err: REG_NFC_SYSCTRL illegal block size\n");
		break;
	}
	switch (p->flash_width) {
	case 8:
		break;
	case 16:
		nfc_err("nfc_err: REG_NFC_SYSCTRL width=16 not supported\n");
		break;
	default:
		nfc_err("nfc_err: REG_NFC_SYSCTRL illegal width\n");
		break;
	}
	switch (p->ecc.type) {
	case BCH:
		_reg |= NFC_SYSCTRL_ECC_BCH;
		break;
	case RS:
		break;
	default:
		nfc_err("nfc_err: REG_NFC_SYSCTRL illegal ecc type\n");
		break;
	}
	p->regs.sysctrl = _reg;

	/* init REG_NFC_SYSCTRL1 */
	p->regs.sysctrl1_read = NFC_SYSCTRL1_NEW_VERSION |
				NFC_SYSCTRL1_DECODE_FROM_BUFFER |
				NFC_SYSCTRL1_FRUN_FULL_PAGE_DMA |
				NFC_SYSCTRL1_LATCH_READ_DELAY_1T;
	p->regs.sysctrl1_prog = NFC_SYSCTRL1_NEW_VERSION |
				NFC_SYSCTRL1_DECODE_FROM_BUFFER |
				NFC_SYSCTRL1_FRUN_FULL_PAGE_DMA_WRITE |
				NFC_SYSCTRL1_FRUN_FULL_PAGE_DMA |
				NFC_SYSCTRL1_LATCH_READ_DELAY_1T;
	if (p->ecc.extra_size == 16) {
		nvt_bset(p->regs.sysctrl1_read,
			 NFC_SYSCTRL1_NO_REMOVE_EXTRA_2DUMMY_BYTE);
		nvt_bset(p->regs.sysctrl1_prog,
			 NFC_SYSCTRL1_NO_REMOVE_EXTRA_2DUMMY_BYTE);
	}

	/* init REG_NFC_SYSCTRL2 */
	p->regs.sysctrl2_prog = 0;
	/* Disable NFC blank check, use software to detect blank sub-pages */
#ifndef ENABLE_BLANK_CHECK
	if (p->ecc.support_inv) {
		p->regs.sysctrl2_read =
			NFC_SYSCTRL2_NEW_BLANK_CHECK_THRESHOLD(-1) |
			NFC_SYSCTRL2_NEW_DISABLE_FULLPAGE_DMA_WRITE_ECC;
	} else {
		p->regs.sysctrl2_read =
			NFC_SYSCTRL2_BLANK_CHECK_THRESHOLD(-1) |
			NFC_SYSCTRL2_XTRA_PTR_PARK |
			NFC_SYSCTRL2_DISABLE_FULLPAGE_DMA_WRITE_ECC;
	}
#else
	if (p->ecc.support_inv) {
		u32 thr = p->subpage_size + p->suboob_size - p->ecc.nr_bits;

		p->regs.sysctrl2_read =
			NFC_SYSCTRL2_NEW_BLANK_CHECK_THRESHOLD(thr) |
			NFC_SYSCTRL2_NEW_BLANK_CHECK_EN |
			NFC_SYSCTRL2_NEW_DISABLE_FULLPAGE_DMA_WRITE_ECC;
	} else {
		u32 thr = p->suboob_size - p->ecc.nr_bits;

		p->regs.sysctrl2_read =
			NFC_SYSCTRL2_BLANK_CHECK_THRESHOLD(thr) |
			NFC_SYSCTRL2_BLANK_CHECK_EN |
			NFC_SYSCTRL2_XTRA_PTR_PARK |
			NFC_SYSCTRL2_DISABLE_FULLPAGE_DMA_WRITE_ECC;
	}
#endif

	/* init REG_NFC_CFG1 */
	_reg = NFC_CFG1_BUSY_TO_READY_TIMEOUT(-1) |
	       NFC_CFG1_LITTLE_ENDIAN |
	       NFC_CFG1_LITTLE_ENDIAN_XTRA | NFC_CFG1_READY_TO_BUSY_TIMEOUT(-1);
	if ((p->ecc.type == RS) && (p->ecc.nr_bits == 4)) {
		switch (p->ic_id) {
#ifndef LEGACY_SUPPORT
		case 0x72458:
		case 0x72658:
			_reg |= NFC_CFG1_ECC_RS_NEW;
			break;
#endif
		case 0x72563:
		case 0x72171:
		case 0x72172:
		case 0x72673:
			_reg |= NFC_CFG1_ECC_RS;
			break;
		default:
			nfc_err("nfc_err: REG_NFC_CFG1 unknown ic_id (RS)\n");
			break;
		}
	} else if ((p->ecc.type == BCH) && (p->ecc.nr_bits == 8)) {
		switch (p->ic_id) {
#ifndef LEGACY_SUPPORT
		case 0x72453:
#endif
		case 0x72563:
		case 0x72171:
		case 0x72172:
		case 0x72673:
			_reg |= NFC_CFG1_BCH_EN;
			switch (p->ecc.extra_size) {
			case 16:
				_reg |= NFC_CFG1_ECC_BCH8_16;
				break;
			case 32:
				_reg |= NFC_CFG1_ECC_BCH8_32;
				break;
			case 56:
				_reg |= NFC_CFG1_ECC_BCH8_56;
				break;
			default:
				nfc_err("nfc_err: REG_NFC_CFG1 illegal BCH\n");
			}
			break;
		default:
			nfc_err("nfc_err: REG_NFC_CFG1 unknown ic_id (BCH)\n");
			break;
		}
#ifndef LEGACY_SUPPORT
	} else if ((p->ecc.type == BCH) && (p->ecc.nr_bits == 24)) {
		nfc_err("nfc_err: REG_NFC_CFG1 not yet implemented (BCH24)\n");
		switch (p->ecc.extra_size) {
		case 54:
			break;
		default:
			break;
		}
#endif
	} else
		nfc_err("nfc_err: REG_NFC_CFG1 illegal ECC\n");
	p->regs.cfg1 = _reg;
#ifdef USE_ECC_INV
	p->regs.cfg1_inv = _reg | NFC_CFG1_DATA_INV;
#endif
}

static void __init nt72_nand_parse_ecc_1pin_bs(struct nt72_nand_info *info)
{
	struct nt72_nand_params *p = &info->nand_params;

	/* BCHxxx or RS */
	if (stbc_read32(REG_STBC_BOOTSTRAP) & (1 << 18)) {
		switch (p->ic_id) {
#ifndef LEGACY_SUPPORT
		case 0x72458:
		case 0x72658:
			p->ecc.type = BCH;
			p->ecc.nr_bits = 24;
			p->ecc.data_size = 1024;
			p->ecc.extra_size = 54;
			break;
#endif
		case 0x72563:
		case 0x72171:
		case 0x72172:
		case 0x72673:
			p->ecc.type = BCH;
			p->ecc.nr_bits = 8;
			p->ecc.data_size = 512;
			p->ecc.extra_size = 16;
			break;
		default:
			nfc_err(
				"nfc_err: Unknown ic_id to determine ECC type (bs1).\n");
			break;
		}
	} else {
		p->ecc.type = RS;
		p->ecc.nr_bits = 4;
		p->ecc.data_size = 512;
		p->ecc.extra_size = 16;
	}
}

static void __init nt72_nand_parse_ecc_2pin_bs(struct nt72_nand_info *info)
{
	struct nt72_nand_params *p = &info->nand_params;

	switch ((stbc_read32(REG_STBC_BOOTSTRAP) >> 17) & 0x3) {
	case 0:
		p->ecc.type = RS;
		p->ecc.nr_bits = 4;
		p->ecc.data_size = 512;
		p->ecc.extra_size = 16;
		break;
	/* be care that this is 2 */
	case 2:
		p->ecc.type = BCH;
		p->ecc.nr_bits = 8;
		p->ecc.data_size = 512;
		p->ecc.extra_size = 16;
		break;
	/* be care that this is 1 */
	case 1:
		p->ecc.type = BCH;
		p->ecc.nr_bits = 8;
		p->ecc.data_size = 512;
		p->ecc.extra_size = 32;
		break;
	case 3:
		p->ecc.type = BCH;
		p->ecc.nr_bits = 8;
		p->ecc.data_size = 512;
		p->ecc.extra_size = 56;
		break;
	default:
		nfc_err("nfc_err: Unknown bs to determine ECC type (bs2).\n");
		break;
	}
}

#ifdef LEGACY_SUPPORT
static void __init nt72_nand_parse_io_fallback(
	struct nt72_nand_info *info)
{
	info->io_param.type = STBC_EMMC_IO;
}
#endif

static void __init nt72_nand_parse_io(struct nt72_nand_info *info)
{
	/* BS[20] */
	if ((stbc_read32(REG_STBC_BOOTSTRAP) >> 20) & 0x1) {
		/* BS[4:3] */
		switch ((stbc_read32(REG_STBC_BOOTSTRAP) >> 3) & 0x3) {
		case 0: /* 00: STBC boot from SPI, CPU boot from NAND 2 Row */
			info->io_param.type = ARM_CI_IO;
			break;
		case 1: /* 01: STBC and CPU boot from NAND 2 Row */
			info->io_param.type = STBC_SPI_IO;
			break;
		case 2: /* 10: STBC and CPU boot from NAND 3 Row */
			info->io_param.type = STBC_SPI_IO;
			break;
		case 3: /* 11: STBC boot from SPI, CPU boot from NAND 3 Row */
			info->io_param.type = ARM_CI_IO;
			break;
		}
	} else {
		/* BS[4:3] */
		switch ((stbc_read32(REG_STBC_BOOTSTRAP) >> 3) & 0x3) {
		case 0: /* 00: STBC boot from SPI, CPU boot from NAND 2 Row */
		case 1: /* 01: STBC and CPU boot from NAND 2 Row */
		case 2: /* 10: STBC and CPU boot from NAND 3 Row */
		case 3: /* 11: STBC boot from SPI, CPU boot from NAND 3 Row */
			info->io_param.type = STBC_EMMC_IO;
			break;
		}
	}
}

void __init nt72_nand_init_io(struct nt72_nand_info *info)
{
	u32 low_id, ext_id, ic_id;

	low_id = clkgen_read32(REG_CLK_GEN_IC_ID) & 0xFFFF;
	ext_id = (clkgen_read32(REG_CLK_GEN_IC_ID) >> 24) & 0xF;
	ic_id = low_id | (ext_id << 16);

	switch (ic_id) {
	case 0x72563:
	case 0x72171:
	case 0x72172:
	case 0x72673:
		nt72_nand_parse_io(info);
		break;
	default:
		nfc_err("nfc_err: unknown ic_id for init_io: 0x%X\n", ic_id);
		break;
	}

#ifdef IS_CVT
	{
		u32 stbc_switch = stbc_read32(REG_STBC_SWITCH);

		switch (info->io_param.type) {
		case STBC_EMMC_IO:
		case ARM_CI_IO:
			nvt_bclear(stbc_switch, 1 << 10);
			stbc_write32(REG_STBC_SWITCH, stbc_switch);
			break;
		case STBC_SPI_IO:
			nvt_bset(stbc_switch, 1 << 10);
			stbc_write32(REG_STBC_SWITCH, stbc_switch);
			break;
		default:
			nfc_err("nfc_err: unkown io for init_io\n");
			break;
		}
	}
#endif
}

void __init nt72_nand_init_params(struct nt72_nand_info *info,
				  struct nt72_nand_spec *f)
{
	u32 low_id, ext_id;
	struct nt72_nand_params *p = &info->nand_params;

	/* nand parameters */
	p->chip_id = f->chip_id;
	p->flash_width = f->flash_width;

	p->page_size = f->page_size;
	p->subpage_size = f->subpage_size;
	p->oob_size = f->oob_size;
	p->suboob_size = (f->oob_size) / (f->page_size / f->subpage_size);

	p->nr_blocks = f->num_blocks;
	p->nr_page_per_block = f->page_per_block;
	p->nr_subpage = p->page_size / p->subpage_size;

	p->size = p->page_size * p->nr_page_per_block * p->nr_blocks;

	/* set ic_id */
	low_id = clkgen_read32(REG_CLK_GEN_IC_ID) & 0xFFFF;
	ext_id = (clkgen_read32(REG_CLK_GEN_IC_ID) >> 24) & 0xF;
	p->ic_id = low_id | (ext_id << 16);

	/* set ic_ver */
	p->ic_ver = clkgen_read32(REG_CLK_GEN_IC_VER) & 0xFF;

	/* set cache_line_size and BCH type */
	switch (p->ic_id) {
#ifndef LEGACY_SUPPORT
	case 0x72453:
		p->cache_line_size = 64;
		nt72_nand_parse_ecc_2pin_bs(info);
		p->ecc.support_inv = 0;
		break;
	case 0x72458:
	case 0x72658:
		p->cache_line_size = 32;
		nt72_nand_parse_ecc_1pin_bs(info);
		p->ecc.support_inv = 0;
		break;
#endif
	case 0x72563:
		p->cache_line_size = 32;
		nt72_nand_parse_ecc_1pin_bs(info);
		p->ecc.support_inv = (p->ic_ver > 0) ? 1 : 0;
		break;
	case 0x72171:
	case 0x72172:
	case 0x72673:
		p->cache_line_size = 32;
		nt72_nand_parse_ecc_1pin_bs(info);
		p->ecc.support_inv = 1;
		break;
	default:
		nfc_err("nfc_err: error ic_id\n");
		break;
	}

	/* load timing */
	p->regs.cfg0_read = f->cfg0_read;
	p->regs.cfg0_prog = f->cfg0_prog;
	p->regs.ftune = f->ftune;

	/* misc */
	if (MTD_BITFLIP_THRESHOLD == 0) {
		if (p->ecc.nr_bits == 8)
			p->bitflip_threshold = 6;
		else
			p->bitflip_threshold = 3;
	} else
		p->bitflip_threshold = MTD_BITFLIP_THRESHOLD;

	p->bbt_options = f->bbt_options;

	/* init other regs */
	nt72_nand_init_regs(info);
}

u32 __init nt72_nand_formal_name(struct nt72_nand_info *info)
{
	struct nt72_nand_params *p = &info->nand_params;

	switch (p->ic_id) {
	case 0x72563:
		return 0x70085;
	case 0x72171:
		return 0x70091;
	default:
		break;
	}

	return p->ic_id;
}

void __init nt72_nand_print_params(struct nt72_nand_info *info)
{
	struct nt72_nand_params *p = &info->nand_params;

	nfc_msg("nfc: ic_id=%X(v%d) flash_id=0x%08X flash_width=%d\n",
		nt72_nand_formal_name(info), p->ic_ver, p->chip_id,
		p->flash_width);
	nfc_msg("nfc: nr_subpage=%d page=%d-%d oob=%d-%d\n", p->nr_subpage,
		p->subpage_size, p->page_size, p->suboob_size, p->oob_size);
	nfc_msg("nfc: nr_blocks=%d-%d size=%uMB\n", p->nr_page_per_block,
		p->nr_blocks, (u32)(p->size / 1024 / 1024));
	nfc_msg("nfc: cache_ls=%d bbt_opt=0x%X bs=0x%08X\n", p->cache_line_size,
		p->bbt_options, stbc_read32(REG_STBC_BOOTSTRAP));

	switch (p->ecc.type) {
	case RS:
		nfc_msg("nfc: ECC=RS%d (%d+%d support_inv=%d) ",
			p->ecc.nr_bits, p->ecc.data_size, p->ecc.extra_size,
			p->ecc.support_inv);
		break;
	case BCH:
		nfc_msg("nfc: ECC=BCH%d (%d+%d support_inv=%d) ",
			p->ecc.nr_bits, p->ecc.data_size, p->ecc.extra_size,
			p->ecc.support_inv);
		break;
	default:
		nfc_err("nfc_err: ECC type unknown!\n");
		break;
	}

	switch (info->io_param.type) {
	case STBC_EMMC_IO:
		nfc_msg("nfc: STBC EMMC IO\n");
		break;
	case ARM_CI_IO:
		nfc_msg("nfc: ARM CI IO\n");
		break;
	case STBC_SPI_IO:
		nfc_msg("nfc: STBC SPI IO\n");
		break;
	default:
		nfc_err("nfc: error IO type\n");
	}

	nfc_msg("nfc: cfg0_read=0x%08X cfg0_prog=0x%08X ftune=0x%08X\n",
		p->regs.cfg0_read, p->regs.cfg0_prog, p->regs.ftune);

	/* Print enabled NFC features */
	nfc_msg("nfc: ");
#ifdef USE_BBT
	nfc_cont("bbt ");
#endif
#ifdef USE_BBT_CHECKSUM
	nfc_cont("bbt_sum ");
#endif
#ifdef SHARE_GPIO_WITH_CI
	nfc_cont("gpio_ci ");
#endif
#ifdef STBC_ECC_SWITCH
	nfc_cont("ecc_switch ");
#endif
#ifdef ENABLE_cmd_hist
	nfc_cont("cmd_hist ");
#endif
#ifdef USE_ECC_INV
	nfc_cont("inv ");
#endif
#ifdef USE_ECC_INV_TABLE
	nfc_cont("inv_part ");
#endif
#ifdef USE_CMDLINE_PARTS
	nfc_cont("cmd_part ");
#endif
#ifdef HAS_NPT
	nfc_cont("npt ");
#endif
#ifdef MTD_COPY_FREE
	nfc_cont("direct_dma ");
#endif
#ifdef ENABLE_BLANK_CHECK
	nfc_cont("bcheck ");
#endif
#ifdef HAS_DEBUGFS
	nfc_cont("dfs ");
#endif
#ifdef HAS_NTS
	nfc_cont("nts ");
#endif
	nfc_cont("\n");
}

#ifdef SELFTEST_NAND_PARAMS
void __init nt72_nand_params_selftest(struct nt72_nand_info *info)
{
	int b1, b2, b3;
	struct nt72_nand_params *p = &info->nand_params;
#ifdef IS_KERNEL
	int b4;
	struct mtd_info *m = info->mtd;
#endif

	/* make sure nand_params is reasonable */
	b1 = p->page_size == 2048;
	b2 = p->page_size == 4096;
	b3 = p->page_size == 8192;
	if (!(b1 || b2 || b3))
		nfc_err("nfc_err: illegal nand page size\n");

	b1 = p->subpage_size == 512;
	b2 = p->subpage_size == 1024;
	if (!(b1 || b2))
		nfc_err("nfc_err: illegal nand subpage size\n");

	if (!((p->flash_width == 16) || (p->flash_width == 8)))
		nfc_err("nfc_err: illegal nand flash width\n");

	if (p->page_size != (p->subpage_size * p->nr_subpage))
		nfc_err("nfc_err: illegal subpage size\n");

	if (p->nr_subpage > MAX_NR_SUBPAGE)
		nfc_err("nfc_err: nr_subpage too large\n");

#if defined(IS_KERNEL)
	/* make sure mtd_info is reasonable */
	if (m->size != p->size)
		nfc_err("nfc_err: nand flash size mismatch\n");

	if (m->oobsize != p->oob_size)
		nfc_err("nfc_err: nand flash OOB size mismatch\n");

	if (m->erasesize != (p->page_size * p->nr_page_per_block))
		nfc_err("nfc_err: nand flash size mismatch\n");

	b1 = (m->erasesize / 1024 / 1024) != 128;
	b2 = (m->erasesize / 1024 / 1024) != 256;
	b3 = (m->erasesize / 1024 / 1024) != 512;
	b4 = (m->erasesize / 1024 / 1024) != 1024;
	if (!(b1 || b2 || b3 || b4))
		nfc_err("nfc_err: illegal erasesize\n");
	/* refer to comment of struct mtd_info */
	b1 = m->writesize == p->page_size;
	b2 = m->writesize == (p->page_size / 2);
	b3 = m->writesize == (p->page_size / 4);
	if (!(b1 || b2 || b3))
		nfc_err("nfc_err: illegal writesize\n");
	/* check ECC params */
	if (p->ecc.data_size != p->subpage_size)
		nfc_err("nfc_err: invalid ecc data size\n");
	if (p->ecc.type == RS) {
		b1 = (p->ecc.nr_bits == 4) &&
		     (p->ecc.data_size == 512) && (p->ecc.extra_size == 16);
		if (!b1)
			nfc_err("nfc_err: illegal RS params\n");
	} else if (p->ecc.type == BCH) {
		b1 = (p->ecc.nr_bits == 8) &&
		     (p->ecc.data_size == 512) && (p->ecc.extra_size == 16);
		b2 = (p->ecc.nr_bits == 8) &&
		     (p->ecc.data_size == 512) && (p->ecc.extra_size == 32);
		b3 = (p->ecc.nr_bits == 8) &&
		     (p->ecc.data_size == 512) && (p->ecc.extra_size == 56);
		b4 = (p->ecc.nr_bits == 24) &&
		     (p->ecc.data_size == 1024) && (p->ecc.extra_size == 54);
		if (!(b1 || b2 || b3 || b4))
			nfc_err("nfc_err: illegal BCH params\n");
	}
#endif

	/* noted that it is BOOTSTRAP[17:18], not BOOTSTRAP[18:17] */
	switch ((stbc_read32(REG_STBC_BOOTSTRAP) >> 17) & 0x03) {
	case 0:
		if (p->ecc.type != RS)
			nfc_err("nfc_err: bootstrap ecc type is not RS\n");
		break;
	case 1:
	case 2:
	case 3:
		if (p->ecc.type != BCH)
			nfc_err("nfc_err: bootstrap ecc type is not BCH\n");
		break;
	default:
		nfc_err("nfc_err: bootstrap ecc type unknown\n");
		break;
	}
}
#endif /* SELFTEST_NAND_PARAMS */

int __init nt72_nand_detect_flash(struct nt72_nand_info *info,
				  struct nt72_nand_spec **nand_spec)
{
	u32 chip_id = -1;
	int i = 0;

	for (i = 0; i < g_nr_nt72_nand_specs; i++) {
		/*
		 * Here we do early setup for read_id because NFC registers are
		 * not initialized yet
		 */
		*nand_spec = (struct nt72_nand_spec *)&g_nt72_nand_specs[i];
		nt72_nand_init_params(g_info, *nand_spec);
		nt72_nand_nfc_sw_reset(info);
		nt72_nand_nfc_reset_nand(info);
		nt72_nand_nfc_read_id(info, 0, &chip_id, NULL, 1);

		if (info->retcode != ERR_NONE)
			continue;

		if (chip_id == g_nt72_nand_specs[i].chip_id) {
			*nand_spec =
				(struct nt72_nand_spec *)&g_nt72_nand_specs[i];
			return 0;
		}
	}
	*nand_spec = NULL;
	nfc_err("nfc_err: unknown flash chip: 0x%X\n", chip_id);
	return -ENODEV;
}

#ifndef IS_KERNEL
void __init nt72_nand_stbc_keypass(void)
{
	u32 _reg;

	/* STBC keypass */
	stbc_write32(REG_STBC_04, 0x72682);
	stbc_write32(REG_STBC_04, 0x28627);
	stbc_write32(REG_STBC_08, 0x01);

	/* NFC arbitration */
	_reg = stbc_read32(REG_STBC_00);
	nvt_bclear(_reg, 1 << 4);
	nvt_bset(_reg, 1 << 5);
	stbc_write32(REG_STBC_00, _reg);
}
#endif
