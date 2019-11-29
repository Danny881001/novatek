#include "include/nt72_nand.h"

static void nt72_nand_nfc_dump_ctrl_regs(const char *who)
{
#ifdef DUMP_CONTROL_REGS
	nfc_err("%s: dump control registers\n", who);
	nfc_err("REG_NFC_XD_CFG=0x%08X\n", nfc_read32(REG_NFC_XD_CFG));
	nfc_err("REG_NFC_SYSCTRL=0x%08X\n", nfc_read32(REG_NFC_SYSCTRL));
	nfc_err("REG_NFC_SYSCTRL1=0x%08X\n", nfc_read32(REG_NFC_SYSCTRL1));
	nfc_err("REG_NFC_SYSCTRL2=0x%08X\n", nfc_read32(REG_NFC_SYSCTRL2));
	nfc_err("REG_NFC_CFG0=0x%08X\n", nfc_read32(REG_NFC_CFG0));
	nfc_err("REG_NFC_CFG1=0x%08X\n", nfc_read32(REG_NFC_CFG1));
	nfc_err("REG_NFC_FTUNE=0x%08X\n", nfc_read32(REG_NFC_FTUNE));
	nfc_err("REG_NFC_XFER=0x%08X\n", nfc_read32(REG_NFC_XFER));
#endif
}

static void nt72_nand_nfc_dump_regs(struct nt72_nand_info *info, int page_addr)
{
#ifdef DUMP_REG_ON_ERROR
#ifndef IS_XBOOT
	int i;

	nfc_err("nfc_err: dump_reg: p: %d\n", page_addr);
	nfc_err("INTSTS:0x%08X CN0:0x%08X CN1:0x%08X STS:0x%08X\n",
		nfc_read32(REG_NFC_INTSTS), nfc_read32(REG_NFC_ERR_CNT0),
		nfc_read32(REG_NFC_ERR_CNT1), nfc_read32(REG_NFC_STS));
	for (i = 0; i < 0x1F0; i += 16) {
		nfc_err("0x%08X    %08X %08X %08X %08X\n", i,
			nfc_read32(REG_NFC_BASE + i),
			nfc_read32(REG_NFC_BASE + i + 4),
			nfc_read32(REG_NFC_BASE + i + 8),
			nfc_read32(REG_NFC_BASE + i + 12));
	}
	nfc_err("0xFC0F0088:0x%08X 0xFC0F00CC:0x%08X\n",
		ahb_read32(REG_AHB_TIMEOUT0), ahb_read32(REG_AHB_TIMEOUT1));

#else
	int i;

	nfc_err("nfc_err: dump_reg: p: %d\n", page_addr);
	for (i = 0; i < 0x1F0; i += 16) {
		nfc_err("0x%08X    %08X %08X %08X %08X\n", i,
			nfc_read32(REG_NFC_BASE + i),
			nfc_read32(REG_NFC_BASE + i + 4),
			nfc_read32(REG_NFC_BASE + i + 8),
			nfc_read32(REG_NFC_BASE + i + 12));
	}

#endif /* IS_XBOOT */
#endif /* DUMP_REG_ON_ERROR */
}

void nt72_nand_nfc_dump_data(struct nt72_nand_info *info, int page_addr)
{
	struct nt72_nand_params *p = &(info->nand_params);

#ifdef IS_KERNEL
	nfc_err("nfc: page: %d dump: 0x%08X\n", page_addr, info->dma_data);
	print_hex_dump(KERN_ERR, "", DUMP_PREFIX_OFFSET, 16, 1, info->data,
		       p->page_size, 1);
	print_hex_dump(KERN_ERR, "", DUMP_PREFIX_OFFSET, 16, 1, info->oob,
		       p->oob_size, 1);
#else
	int i;
	int j;

	nfc_err("nfc_err: dump_data: p: %d\n", page_addr);
	for (i = 0; i < p->page_size; i += 16) {
		nfc_err("0x%08X   ", i);
		for (j = 0; j < 16; j++)
			nfc_cont("%02X ", info->data[i + j]);
		nfc_cont("\n");
	}
	for (i = 0; i < p->oob_size; i += 16) {
		nfc_err("0x%08X   ", p->page_size + i);
		for (j = 0; j < 16; j++)
			nfc_cont("%02X ", info->oob[i + j]);
		nfc_cont("\n");
	}
#endif
}

static void nt72_nand_nfc_dump_ecc(struct nt72_nand_info *info, int page_addr,
				   int use_inv)
{
#ifndef IS_XBOOT
	struct nt72_nand_ecc_result *e = &(info->ecc_result);

#ifndef USE_ECC_INV
	use_inv = 0;
#endif

	if (info->nand_params.nr_subpage == 4) {
		nfc_err("nfc: pg: %d cnt: %d %d %d %d unc: %d %d %d %d blnk: %d %d %d %d inv: %d\n",
			page_addr,
			e->_err_count[0], e->_err_count[1],
			e->_err_count[2], e->_err_count[3],
			e->_uncorrectable[0], e->_uncorrectable[1],
			e->_uncorrectable[2], e->_uncorrectable[3],
			e->_blank[0], e->_blank[1], e->_blank[2], e->_blank[3],
			use_inv);
	} else {
		nfc_err("nfc: pg: %d cnt: %d %d %d %d %d %d %d %d unc: %d %d %d %d %d %d %d %d blnk: %d %d %d %d %d %d %d %d inv: %d\n",
			page_addr,
			e->_err_count[0], e->_err_count[1],
			e->_err_count[2], e->_err_count[3],
			e->_err_count[4], e->_err_count[5],
			e->_err_count[6], e->_err_count[7],
			e->_uncorrectable[0], e->_uncorrectable[1],
			e->_uncorrectable[2], e->_uncorrectable[3],
			e->_uncorrectable[4], e->_uncorrectable[5],
			e->_uncorrectable[6], e->_uncorrectable[7],
			e->_blank[0], e->_blank[1], e->_blank[2], e->_blank[3],
			e->_blank[4], e->_blank[5], e->_blank[6], e->_blank[7],
			use_inv);
	}

#ifdef IS_CVT
	{
		int i;

		nfc_err("nfc: bcnt: ");
		for (i = 0; i < info->nand_params.nr_subpage; i++)
			nfc_cont("%d ", info->ecc_result.blank_count[i]);
	}
#endif
#endif
}

static int nt72_nand_nfc_check_ready(const char *who,
				     struct nt72_nand_info *info)
{
#ifdef IS_XBOOT
	if (nfc_read32(REG_NFC_STS) & (NFC_STS_NAND_BUSY | NFC_STS_ACTIVE)) {
		info->retcode = ERR_NAND_TIMEOUT;
		nfc_err("nfc_err: %s nand not ready\n", who);
		return ERR_NAND_TIMEOUT;
	}
#endif

	return ERR_NONE;
}

static void nt72_nand_nfc_ecc_workaround(struct nt72_nand_info *info)
{
	/* This is workaround to prevent ECC misbehavior during PIO mode */
	nt72_nand_nfc_sw_reset(info);
	/* And reset ECC just in case */
	nt72_nand_nfc_ecc_reset(info);
}

void nt72_nand_nfc_disable_ecc(struct nt72_nand_info *info)
{
	u32 _reg;

	/* Turn-off BCH */
	if (info->nand_params.ecc.type == BCH) {
		_reg = nfc_read32(REG_NFC_SYSCTRL);
		nvt_bclear(_reg, NFC_SYSCTRL_ECC_BCH);
		nfc_write32(REG_NFC_SYSCTRL, _reg);
	}

	/* Disable free-run ECC */
	_reg = nfc_read32(REG_NFC_SYSCTRL2);
	if (info->nand_params.ecc.support_inv) {
		nvt_bset(_reg, NFC_SYSCTRL2_NEW_AUTO_CORRECT_DISABLE |
			 NFC_SYSCTRL2_NEW_DISABLE_FULLPAGE_DMA_WRITE_ECC);
	} else {
		nvt_bset(_reg, NFC_SYSCTRL2_AUTO_CORRECT_DISABLE |
			 NFC_SYSCTRL2_DISABLE_FULLPAGE_DMA_WRITE_ECC);
	}
	nfc_write32(REG_NFC_SYSCTRL2, _reg);

	/* Disable ECC inversion */
	_reg = nfc_read32(REG_NFC_CFG1);
	nvt_bclear(_reg, NFC_CFG1_DATA_INV);
	nfc_write32(REG_NFC_CFG1, _reg);
}

static u32 nt72_nand_nfc_count0(struct nt72_nand_info *info, u64 *data)
{
	if (info->nand_params.ecc.type == RS) {
		int count = 0;
		u8 *data8 = (u8 *)data;
		u8 *data8_end = data8 + 8;

		for (; data8 < data8_end; data8++) {
			if (unlikely(*data8 != 0xFF))
				count++;
		}

		return count;
	} else if (info->nand_params.ecc.type == BCH) {
		return 64 - nt72_nand_hweight((u32)(*data)) -
		       nt72_nand_hweight((u32)((*data) >> 32));
	}

	nfc_err("nfc_err: invalid ECC in count0\n");
	return 0;
}

static u32 nt72_nand_nfc_count1(struct nt72_nand_info *info, u64 *data)
{
	if (info->nand_params.ecc.type == RS) {
		int count = 0;
		u8 *data8 = (u8 *)data;
		u8 *data8_end = data8 + 8;

		for (; data8 < data8_end; data8++) {
			if (unlikely(*data8 != 0x0))
				count++;
		}

		return count;
	} else if (info->nand_params.ecc.type == BCH) {
		return nt72_nand_hweight((u32)(*data)) +
		       nt72_nand_hweight((u32)((*data) >> 32));
	}

	nfc_err("nfc_err: invalid ECC in count1\n");
	return 0;
}

static void nt72_nand_nfc_fill_subpage(struct nt72_nand_info *info,
				       int page_addr, int subpage_id,
				       int oob_only, int use_inv)
{
	struct nt72_nand_params *p = &(info->nand_params);
	struct nt72_nand_ecc_result *e = &(info->ecc_result);

	int count = 0;
	int threshold = p->ecc.nr_bits;

	/* pointers to subpage and suboob */
#ifndef IS_XBOOT
	u8 *main8 = info->data + (p->subpage_size * subpage_id);
	u64 *main64 = (u64 *)main8;
	u64 *main64_end = (u64 *)(main8 + p->subpage_size);
#endif
	u8 *extra8 = info->oob + (p->suboob_size * subpage_id);
	u64 *extra64 = (u64 *)extra8;
	u64 *extra64_end = (u64 *)(extra8 + p->suboob_size);

	if (use_inv) {
		/* Count how many 1s in the full-0 subpage */
#ifndef IS_XBOOT
		/* We cannot read main data if xboot ATF is enabled */
		for (; main64 < main64_end; main64++) {
			if (*main64 != 0x0) {
				count += nt72_nand_nfc_count1(info, main64);
				if (count > threshold)
					return;
			}
		}
#endif
		for (; extra64 < extra64_end; extra64++) {
			if (*extra64 != 0x0) {
				count += nt72_nand_nfc_count1(info, extra64);
				if (count > threshold)
					return;
			}
		}
	} else {
		/* Count how many 0s in the full-1 subpage */
#ifndef IS_XBOOT
		/* We cannot read main data if xboot ATF is enabled */
		for (; main64 < main64_end; main64++) {
			if (*main64 != 0xFFFFFFFFFFFFFFFFul) {
				count += nt72_nand_nfc_count0(info, main64);
				if (count > threshold)
					return;
			}
		}
#endif
		for (; extra64 < extra64_end; extra64++) {
			if (*extra64 != 0xFFFFFFFFFFFFFFFFul) {
				count += nt72_nand_nfc_count0(info, extra64);
				if (count > threshold)
					return;
			}
		}
	}

	/* Now we know this sub-page is full-0 or full-1, remove the
	 * uncorrectable mark
	 */
	e->uncorrectable--;
	e->_uncorrectable[subpage_id] = 0;
	e->_err_count[subpage_id] = count;
	if (!use_inv) {
		/* Mark the full-1 sub-page blank */
		e->blank++;
		e->_blank[subpage_id] = 1;
	}

#if !defined(IS_CVT) && !defined(IS_XBOOT)
	/* If this full-0 or full-1 sub-page has bit-flips */
	if (unlikely(count)) {
		/* Correct bit-flips */
		if (use_inv) {
			if (likely(!oob_only))
				memset(main8, 0x0, p->subpage_size);
			memset(extra8, 0x0, p->suboob_size);
		} else {
			if (likely(!oob_only))
				memset(main8, 0xFF, p->subpage_size);
			memset(extra8, 0xFF, p->suboob_size);
		}
		/* Report ECC error count to MTD */
		e->err_count += count;
		if (unlikely(e->max_err < count))
			e->max_err = count;
	}
#endif
}

static void nt72_nand_nfc_fill_page(struct nt72_nand_info *info, int page_addr,
				    int oob_only, int use_inv)
{
	int i;

	for (i = 0; i < info->nand_params.nr_subpage; i++) {
		if (info->ecc_result._uncorrectable[i]) {
			nt72_nand_nfc_fill_subpage(info, page_addr, i, oob_only,
						   use_inv);
		}
	}
}

#ifndef IS_XBOOT
static void nt72_nand_nfc_error_msg_for_human(struct nt72_nand_info *info)
{
	u32 sts = nfc_read32(REG_NFC_INTSTS);

	nfc_err("nfc_err: NAND command failed, precise error message:\n");

	switch (info->retcode) {
	case ERR_ECC_FAIL:
		nfc_err("nfc_err: too many errors to be corrected by ECC\n");
		break;
	case ERR_NAND_TIMEOUT:
		nfc_err("nfc_err: the NAND flash has no respond\n");
		break;
	case ERR_STATUS:
		nfc_err("nfc_err: the NAND controller has error\n");
		break;
	case ERR_CTRL_TIMEOUT:
		nfc_err("nfc_err: the NAND controller has no respond\n");
		break;
	}

	if (sts & NFC_INTSTS_PIO_READ_ERR)
		nfc_err("nfc_err: PIO read error\n");
	if (sts & NFC_INTSTS_PIO_WRITE_ERR)
		nfc_err("nfc_err: PIO write error\n");
	if (sts & NFC_INTSTS_DCR_WRITE_ERR)
		nfc_err("nfc_err: DCR write error\n");
	if (sts & NFC_INTSTS_CMD_REG_WRITE_ERR)
		nfc_err("nfc_err: write CMD reg error\n");
	if (sts & NFC_INTSTS_XFER_REG_WRITE_ERR)
		nfc_err("nfc_err: write XFER reg error\n");
	if (sts & NFC_INTSTS_DMA_XFER_REG_WRITE_ERR)
		nfc_err("nfc_err: write DMA XFER reg error\n");
	if (sts & NFC_INTSTS_CMD_WAIT_READY_TIMEOUT)
		nfc_err("nfc_err: command wait ready signal timeout\n");
	if (sts & NFC_INTSTS_CMD_WAIT_BUSY_TIMEOUT)
		nfc_err("nfc_err: command wait busy signal timeout\n");
	if (sts & NFC_INTSTS_XFER_WAIT_READY_TIMEOUT)
		nfc_err("nfc_err: data transfer wait ready signal timeout\n");
	if (sts & NFC_INTSTS_XFER_WAIT_BUSY_TIMEOUT)
		nfc_err("nfc_err: data transfer wait busy signal timeout\n");
	if (sts & NFC_INTSTS_XFER_WAIT_READY2_TIMEOUT)
		nfc_err("nfc_err: data transfer wait ready2 signal timeout\n");
	if (sts & NFC_INTSTS_EMULATOR_TIMEOUT)
		nfc_err("nfc_err: emulator timeout\n");
}
#endif

static void nt72_nand_nfc_error_handling(struct nt72_nand_info *info,
					 int page_addr, int read, int use_inv)
{
#if defined(IS_CVT)
	/* do nothing */
#elif !defined(IS_XBOOT)
	nt72_nand_nfc_error_msg_for_human(info);
	nt72_nand_nfc_dump_regs(info, page_addr);
	if (read) {
		nt72_nand_nfc_dump_ecc(info, page_addr, use_inv);
		nt72_nand_nfc_dump_data(info, page_addr);
	}
	nt72_nand_cmd_hist_dump();

	nt72_nand_nfc_hw_reset();
	nt72_nand_nfc_sw_reset(info);
	nt72_nand_nfc_ecc_reset(info);
#else
	nt72_nand_nfc_dump_regs(info, page_addr);
#endif
}

void nt72_nand_nfc_hw_reset(void)
{
	u32 _reg = stbc_read32(REG_STBC_1C);

	nvt_bset(_reg, STBC_1C_NFC_RST);
	stbc_write32(REG_STBC_1C, _reg);
	nvt_bclear(_reg, STBC_1C_NFC_RST);
	stbc_write32(REG_STBC_1C, _reg);
}

void nt72_nand_nfc_sw_reset(struct nt72_nand_info *info)
{
	unsigned long stop_time = nt72_nand_timeout_init();

#ifdef SELFTEST_REG_POLLING_TIMEOUT
	int counter = 1;
#endif

	if (info->nand_params.ecc.support_inv) {
		nfc_write32(REG_NFC_SFTRST, NFC_SFTRST_CONTROLLER |
			    NFC_SFTRST_DMA | NFC_SFTRST_BUFFER |
			    NFC_SFTRST_BCH_DECODE);
	} else {
		nfc_write32(REG_NFC_SFTRST, NFC_SFTRST_CONTROLLER |
			    NFC_SFTRST_DMA | NFC_SFTRST_BUFFER);
	}

	while (nfc_read32(REG_NFC_SFTRST) != 0) {
#ifdef SELFTEST_REG_POLLING_TIMEOUT
		if ((counter % 1000) == 0)
			ssleep(REG_POLLING_TIMEOUT / HZ * 2);
#endif
		if (nt72_nand_timeout_check(stop_time))
			break;
		nt72_nand_cpu_relax();
	}
}

void nt72_nand_nfc_ecc_reset(struct nt72_nand_info *info)
{
	u32 cfg1 = info->nand_params.regs.cfg1;

	/*
	 * Workaround:
	 * In order to reset in BCH, you need to be in non-Freerun mode
	 */
	if (info->nand_params.ecc.type == BCH) {
		u32 sysctrl1 = info->nand_params.regs.sysctrl1_read;

		nvt_bclear(sysctrl1, NFC_SYSCTRL1_FRUN_FULL_PAGE_DMA |
			   NFC_SYSCTRL1_FRUN_FULL_PAGE_DMA_WRITE |
			   NFC_SYSCTRL1_DECODE_FROM_BUFFER);
		nfc_write32(REG_NFC_SYSCTRL1, sysctrl1);
	}

	/*
	 * Do the reset, because it is blocking write through APB, no need
	 * waiting, just flip the bit
	 */
	nvt_bset(cfg1, NFC_CFG1_ECC_ENCODE_RESET | NFC_CFG1_ECC_DECODE_RESET);
	nfc_write32(REG_NFC_CFG1, cfg1);

	/* Pull down reset and restore NFC_CFG1_BCH_EN */
	nfc_write32(REG_NFC_CFG1, info->nand_params.regs.cfg1);
}

static int nt72_nand_nfc_check_int_status(const char *who,
					  struct nt72_nand_info *info)
{
	int result = ERR_NONE;
	u32 int_status = nfc_read32(REG_NFC_INTSTS);

#if defined(SHARE_GPIO_WITH_CI) && !defined(IS_XBOOT)
	if (unlikely(nt72_nand_check_gpio(info))) {
		nfc_err("nfc_err: %s gpio check fail, reprogram GPIO!\n", who);
		nt72_nand_switch_io(info);
		result = ERR_STATUS;
		goto has_error;
	}
#endif

	if (unlikely(int_status & NFC_INTSTS_ERRORS)) {
		nfc_err("nfc_err: %s error!\n", who);
		result = ERR_STATUS;
		goto has_error;
	}

	if (unlikely(int_status & NFC_INTSTS_TIMEOUTS)) {
		nfc_err("nfc_err: %s timeout!\n", who);
		result = ERR_NAND_TIMEOUT;
		goto has_error;
	}

	return ERR_NONE;

has_error:
	return result;
}

static void nt72_nand_nfc_init_pio_regs(struct nt72_nand_info *info)
{
	u32 sysctrl1;
	struct nt72_nand_params *p = &(info->nand_params);

	nfc_write32(REG_NFC_SYSCTRL, p->regs.sysctrl);

	sysctrl1 = p->regs.sysctrl1_read;
	nvt_bclear(sysctrl1, NFC_SYSCTRL1_NEW_VERSION |
		   NFC_SYSCTRL1_FRUN_FULL_PAGE_DMA |
		   NFC_SYSCTRL1_FRUN_FULL_PAGE_DMA_WRITE |
		   NFC_SYSCTRL1_DECODE_FROM_BUFFER);
	nfc_write32(REG_NFC_SYSCTRL1, sysctrl1);

	nfc_write32(REG_NFC_CFG0, p->regs.cfg0_read);

	nfc_write32(REG_NFC_CFG1, p->regs.cfg1);

	nfc_write32(REG_NFC_FTUNE, p->regs.ftune);
}

static void nt72_nand_nfc_set_rw_regs(struct nt72_nand_info *info, int write,
				      int use_ecc, int use_inv)
{
	struct nt72_nand_params *p = &(info->nand_params);

#ifndef USE_ECC_INV
	use_inv = 0;
#endif
#ifdef IS_XBOOT
	use_ecc = 1;
	write = 0;
#endif

	if (!write)
		nfc_write32(REG_NFC_XD_CFG, p->regs.xd_cfg);

	nfc_write32(REG_NFC_SYSCTRL, p->regs.sysctrl);

	if (write)
		nfc_write32(REG_NFC_SYSCTRL1, p->regs.sysctrl1_prog);
	else
		nfc_write32(REG_NFC_SYSCTRL1, p->regs.sysctrl1_read);

	if (write)
		nfc_write32(REG_NFC_SYSCTRL2, p->regs.sysctrl2_prog);
	else
		nfc_write32(REG_NFC_SYSCTRL2, p->regs.sysctrl2_read);

	if (write)
		nfc_write32(REG_NFC_CFG0, p->regs.cfg0_prog);
	else
		nfc_write32(REG_NFC_CFG0, p->regs.cfg0_read);

	if (use_ecc && use_inv)
		nfc_write32(REG_NFC_CFG1, p->regs.cfg1_inv);
	else
		nfc_write32(REG_NFC_CFG1, p->regs.cfg1);

	nfc_write32(REG_NFC_FTUNE, p->regs.ftune);
}

static void nt72_nand_set_dma_regs(struct nt72_nand_info *info, int column,
				   int page_addr, int write, int use_ecc)
{
	u32 _reg;
	struct nt72_nand_params *p = &(info->nand_params);

#ifdef IS_XBOOT
	use_ecc = 1;
	write = 0;
#endif

	nfc_write32(REG_NFC_ROWADR, page_addr);

	nfc_write32(REG_NFC_COLADR, 0);

	nfc_write32(REG_NFC_SYSADR, info->dma_data);
	nfc_write32(REG_NFC_XTRA_ADDR, info->dma_oob);

	if (write) {
		nfc_write32(REG_NFC_RNDACC,
			    NFC_RNDACC_COL_ADDR(p->page_size) |
			    NFC_RNDACC_CODE0(0x85));
	} else {
		nfc_write32(REG_NFC_RNDACC,
			    NFC_RNDACC_COL_ADDR(p->page_size) |
			    NFC_RNDACC_CODE0(0x05) | NFC_RNDACC_CODE1(0xe0));
	}

	_reg = NFC_XFER_DATA_SEL_DMA |
	       NFC_XFER_TRANSFER_BYTE_COUNT(p->page_size) |
	       (use_ecc ? NFC_XFER_ECC_RESET : 0) |
	       (use_ecc ? NFC_XFER_ECC_ENABLE : 0) |
	       /* only after REG_NFC_RNDACC set */
	       NFC_XFER_RAND_DATA_ACC_CMD |
	       NFC_XFER_CHIP_SEL(TARGET_CHIP);
	if (p->ecc.extra_size == 16)
		_reg |= NFC_XFER_XTRA_DATA_COUNT_16;
	else
		nfc_err("nfc_err: NFC_XFER extra size error\n");
	if (write)
		_reg |= NFC_XFER_WRITE | NFC_XFER_RAND_DATA_ACC_CMD_CYCLE_ONE |
			NFC_XFER_KEEP_CE;
	else
		_reg |= NFC_XFER_START_WAIT_RDY |
			NFC_XFER_RAND_DATA_ACC_CMD_CYCLE_TWO;
	nfc_write32(REG_NFC_XFER, _reg);
}

static void nt72_nand_nfc_send_piocmd(struct nt72_nand_info *info,
				      const int busy_wait, const u32 cmd)
{
	nt72_nand_ir_init(info, busy_wait,
			  NFC_INTCTR_ENABLE |
			  NFC_INTCTR_CMD_COMPLETE |
			  NFC_INTCTR_ERR_TIMEOUT);

	nfc_write32(REG_NFC_CMD, cmd);
}

static void nt72_nand_nfc_send_rwcmd(struct nt72_nand_info *info, int column,
				     int page_addr, int write)
{
#ifdef IS_XBOOT
	write = 0;
#endif
	nt72_nand_ir_init(info, 0,
			  NFC_INTCTR_ENABLE |
			  NFC_INTCTR_FRUN_COMPLETE |
			  NFC_INTCTR_ERR_TIMEOUT);

	if (write) {
		/*
		 * During write, we write DMACTR after CMD to prevent DMA
		 * misbehaving (lose extra data).
		 * It can be easily observed by running mtd_oobtest.ko.
		 * */
		nfc_write32(REG_NFC_CMD,
			    NFC_CMD_CODE0(0x80) |
			    NFC_CMD_CYCLE_ONE |
			    NFC_CMD_ADDR_CYCLE_COL_OTHER |
			    NFC_CMD_DATA_XFER |
			    NFC_CMD_WP_KEEP |
			    NFC_CMD_WP_NEG |
			    NFC_CMD_CHIP_SEL(TARGET_CHIP));
		nfc_write32(REG_NFC_DMACTR,
			    NFC_DMACTR_TRAN_BYTE(info->nand_params.page_size));
	} else {
		nfc_write32(REG_NFC_DMACTR,
			    NFC_DMACTR_WRITE_TO_DRAM |
			    NFC_DMACTR_TRAN_BYTE(info->nand_params.page_size));
		nfc_write32(REG_NFC_CMD,
			    NFC_CMD_CODE0(0x00) |
			    NFC_CMD_CODE1(0x30) |
			    NFC_CMD_CYCLE_TWO |
			    NFC_CMD_END_WAIT_READY |
			    NFC_CMD_ADDR_CYCLE_COL_ROW |
			    NFC_CMD_DATA_XFER |
			    NFC_CMD_WP_KEEP |
			    NFC_CMD_CHIP_SEL(TARGET_CHIP));
	}
}

#ifndef IS_XBOOT
static void nt72_nand_nfc_get_ecc_result_full(struct nt72_nand_info *info)
{
	int i;
	int odd, even;
	u32 cnt0, cnt1;
#ifdef IS_CVT
	u32 bcnt0, bcnt1;
#endif
	struct nt72_nand_ecc_result *e = &(info->ecc_result);

	memset(e, 0, sizeof(*e));

	cnt0 = nfc_read32(REG_NFC_ERR_CNT0);
	cnt1 = nfc_read32(REG_NFC_ERR_CNT1);
#ifdef IS_CVT
	bcnt0 = nfc_read32(REG_NFC_BLANK_CNT0);
	bcnt1 = nfc_read32(REG_NFC_BLANK_CNT1);
#endif

	for (i = 0; i < (info->nand_params.nr_subpage / 2); i++) {
		even = i * 2;
		odd = even + 1;
		/* REG_NFC_ERR_CNT0: subpage: 0/2/4/6 */
#ifdef IS_CVT
		e->blank_count[even] = (bcnt0 >> (i * 8)) & 0xFF;
		if (cnt0 & NFC_ERR_CNTX_IS_BLANK(i)) {
			e->_blank[even] = 1;
			e->blank++;
			goto odd_subpage;
		}
#endif
		/* Update ECC counter and uncorrectable bit */
		if (info->nand_params.ecc.type == BCH) {
			e->_err_count[even] = (cnt0 >> (5 * i)) & 0x1F;
			if (unlikely(((cnt0 >> (24 + i * 2)) & 0x3) == 0x2)) {
				e->_uncorrectable[even] = 1;
				e->uncorrectable++;
			} else {
				e->err_count += e->_err_count[even];
				if (unlikely(e->max_err < e->_err_count[even]))
					e->max_err = e->_err_count[even];
			}
		} else {
			/* RS4 */
			e->_err_count[even] = (cnt0 >> (5 * i)) & 0x07;
			if (unlikely(((cnt0 >> (5 * i + 3)) & 0x3) == 0x2)) {
				e->_uncorrectable[even] = 1;
				e->uncorrectable++;
			} else {
				e->err_count += e->_err_count[even];
				if (unlikely(e->max_err < e->_err_count[even]))
					e->max_err = e->_err_count[even];
			}
		}
		/* REG_NFC_ERR_CNT1: subpage: 1/3/5/7 */
#ifdef IS_CVT
odd_subpage:
		e->blank_count[odd] = (bcnt1 >> (i * 8)) & 0xFF;
		if (cnt1 & NFC_ERR_CNTX_IS_BLANK(i)) {
			e->_blank[odd] = 1;
			e->blank++;
			continue;
		}
#endif
		/* Update ECC counter and uncorrectable bit */
		if (info->nand_params.ecc.type == BCH) {
			e->_err_count[odd] = (cnt1 >> (5 * i)) & 0x1F;
			if (unlikely(((cnt1 >> (24 + i * 2)) & 0x3) == 0x2)) {
				e->_uncorrectable[odd] = 1;
				e->uncorrectable++;
			} else {
				e->err_count += e->_err_count[odd];
				if (unlikely(e->max_err < e->_err_count[odd]))
					e->max_err = e->_err_count[odd];
			}
		} else {
			/* RS4 */
			e->_err_count[odd] = (cnt1 >> (5 * i)) & 0x07;
			if (unlikely(((cnt1 >> (5 * i + 3)) & 0x3) == 0x2)) {
				e->_uncorrectable[odd] = 1;
				e->uncorrectable++;
			} else {
				e->err_count += e->_err_count[odd];
				if (unlikely(e->max_err < e->_err_count[odd]))
					e->max_err = e->_err_count[odd];
			}
		}
	}
}
#endif

#ifdef IS_XBOOT
static void nt72_nand_nfc_get_ecc_result_light(struct nt72_nand_info *info)
{
	int i;
	u32 cnt0, cnt1;
	struct nt72_nand_ecc_result *e = &(info->ecc_result);

	memset(e, 0, sizeof(*e));

	cnt0 = nfc_read32(REG_NFC_ERR_CNT0);
	cnt1 = nfc_read32(REG_NFC_ERR_CNT1);

	if (info->nand_params.ecc.type == BCH) {
		e->_uncorrectable[0] = ((cnt0 > 24) & 0x3) == 0x2;
		e->_uncorrectable[1] = ((cnt1 > 24) & 0x3) == 0x2;
		e->_uncorrectable[2] = ((cnt0 > 26) & 0x3) == 0x2;
		e->_uncorrectable[3] = ((cnt1 > 26) & 0x3) == 0x2;
		e->_uncorrectable[4] = ((cnt0 > 28) & 0x3) == 0x2;
		e->_uncorrectable[5] = ((cnt1 > 28) & 0x3) == 0x2;
		e->_uncorrectable[6] = ((cnt0 > 30) & 0x3) == 0x2;
		e->_uncorrectable[7] = ((cnt1 > 30) & 0x3) == 0x2;
	} else {
		e->_uncorrectable[0] = ((cnt0 > 3) & 0x3) == 0x2;
		e->_uncorrectable[1] = ((cnt1 > 3) & 0x3) == 0x2;
		e->_uncorrectable[2] = ((cnt0 > 8) & 0x3) == 0x2;
		e->_uncorrectable[3] = ((cnt1 > 8) & 0x3) == 0x2;
		e->_uncorrectable[4] = ((cnt0 > 13) & 0x3) == 0x2;
		e->_uncorrectable[5] = ((cnt1 > 13) & 0x3) == 0x2;
		e->_uncorrectable[6] = ((cnt0 > 18) & 0x3) == 0x2;
		e->_uncorrectable[7] = ((cnt1 > 18) & 0x3) == 0x2;
	}

	for (i = 0; i < info->nand_params.nr_subpage; i++)
		e->uncorrectable += e->_uncorrectable[i];
}
#endif

static void nt72_nand_nfc_get_ecc_result(struct nt72_nand_info *info)
{
#ifdef IS_XBOOT
	nt72_nand_nfc_get_ecc_result_light(info);
#else
	nt72_nand_nfc_get_ecc_result_full(info);
#endif
}

void nt72_nand_nfc_read_id(struct nt72_nand_info *info, int column, u32 *id,
			   u32 *id_ext, int probe)
{
	u32 _id = 0;
	u32 _id_ext = 0;
	int read_onfi = column == 0x20;

	nt72_nand_lock_nfc(info);
	nt72_nand_cmd_hist_add("ID");
	info->retcode = nt72_nand_nfc_check_ready("read_id", info);
	if (unlikely(info->retcode))
		goto has_error;

	if (read_onfi) {
		/*
		 * Linux may read ONFI identifier but we don't support it,
		 * so skip it to boot faster.
		 * */
		goto skip_read_onfi;
	}

	if (!probe) {
		/* During probe, we have no init values for registers */
		nt72_nand_nfc_init_pio_regs(info);
	}

	if (read_onfi) {
		nfc_write32(REG_NFC_XFER,
			    NFC_XFER_TRANSFER_BYTE_COUNT(4) |
			    NFC_XFER_CHIP_SEL(TARGET_CHIP));
	} else {
		nfc_write32(REG_NFC_XFER,
			    NFC_XFER_TRANSFER_BYTE_COUNT(5) |
			    NFC_XFER_CHIP_SEL(TARGET_CHIP));
	}

	nt72_nand_nfc_dump_ctrl_regs("read_id");

	nt72_nand_cmd_hist_save_start_timestamp();
	nt72_nand_nfc_send_piocmd(info, 1, NFC_CMD_CODE0(0x90) |
				  NFC_CMD_CYCLE_ONE |
				  NFC_CMD_ADDR_CYCLE_DUMMY |
				  NFC_CMD_DATA_XFER |
				  NFC_CMD_CHIP_SEL(TARGET_CHIP));
	info->retcode =
		nt72_nand_ir_busy_wait(info, "read_id",
				       NFC_INTSTS_DATA_XFER_TO_SRAM_COMPLETE |
				       NFC_INTSTS_ERRORS | NFC_INTSTS_TIMEOUTS);
	nt72_nand_cmd_hist_save_end_timestamp();
	if (unlikely(info->retcode))
		goto has_error;

	info->retcode = nt72_nand_nfc_check_ready("read_id_after", info);
	if (unlikely(info->retcode))
		goto has_error;

	info->retcode = nt72_nand_nfc_check_int_status("read_id", info);
	if (unlikely(info->retcode))
		goto has_error;

	_id = nfc_read32(REG_NFC_DATA);
	if (!read_onfi)
		_id_ext = nfc_read32(REG_NFC_DATA);

skip_read_onfi:

	if (id) {
		*id = _id;
		if (id_ext)
			*id_ext = _id_ext;
	} else {
		u8 id_buff[5];

		id_buff[0] = _id & 0xff;
		id_buff[1] = (_id >> 8) & 0xff;
		id_buff[2] = (_id >> 16) & 0xff;
		id_buff[3] = (_id >> 24) & 0xff;
		id_buff[4] = read_onfi ? 0 : _id_ext & 0xff;
		info->column = 0;
		memcpy(info->data, id_buff, read_onfi ? 4 : 5);
	}

	if (!probe)
		nt72_nand_nfc_ecc_workaround(info);

	nt72_nand_unlock_nfc(info);
	return;
has_error:
	nt72_nand_nfc_error_handling(info, 0, 0, 0);
	nt72_nand_unlock_nfc(info);
	nfc_err("nfc_err: read_id failed: %d\n", info->retcode);
}

void nt72_nand_nfc_read_status(struct nt72_nand_info *info, uint32_t *status)
{
	uint8_t st_buff[2];

	nt72_nand_lock_nfc(info);
	nt72_nand_cmd_hist_add("STS");
	info->retcode = nt72_nand_nfc_check_ready("read_status", info);
	if (unlikely(info->retcode))
		goto has_error;

	nt72_nand_nfc_init_pio_regs(info);

	nfc_write32(REG_NFC_XFER, NFC_XFER_CHIP_SEL(TARGET_CHIP) |
		    NFC_XFER_TRANSFER_BYTE_COUNT(1));

	nt72_nand_nfc_dump_ctrl_regs("read_status");

	nt72_nand_cmd_hist_save_start_timestamp();
	nt72_nand_nfc_send_piocmd(info, 1, NFC_CMD_CHIP_SEL(TARGET_CHIP) |
				  NFC_CMD_DATA_XFER | NFC_CMD_CYCLE_ONE |
				  NFC_CMD_WP_NEG | NFC_CMD_CODE0(0x70));
	info->retcode =
		nt72_nand_ir_busy_wait(info, "read_status",
				       NFC_INTSTS_CMD_COMPLETE |
				       NFC_INTSTS_ERRORS | NFC_INTSTS_TIMEOUTS);
	nt72_nand_cmd_hist_save_end_timestamp();
	if (unlikely(info->retcode))
		goto has_error;

	info->retcode = nt72_nand_nfc_check_ready("read_status_after", info);
	if (unlikely(info->retcode))
		goto has_error;

	info->retcode = nt72_nand_nfc_check_int_status("read_status", info);
	if (unlikely(info->retcode))
		goto has_error;

	st_buff[0] = nfc_read32(REG_NFC_DATA) & 0xFF;

	if (status)
		*status = st_buff[0];
	else {
		info->column = 0;
		info->data[0] = st_buff[0];
	}

	nt72_nand_nfc_ecc_workaround(info);

	nt72_nand_unlock_nfc(info);
	return;
has_error:
	nt72_nand_nfc_error_handling(info, 0, 0, 0);
	nfc_err("nfc_err: read_status failed: %d\n", info->retcode);
	nt72_nand_unlock_nfc(info);
}

#if 0
void nt72_nand_nfc_read_subpage(struct nt72_nand_info *info, int column,
				int page_addr, int use_ecc, int oob_only)
{
	u32 _reg;
	int subpage;
	struct nt72_nand_params *p = &(info->nand_params);
	u32 subextra_size = p->ecc.extra_size / p->nr_subpage;
	u32 subpage_offset = p->subpage_size * subpage;
	u32 subextra_offset = p->subextra_size * subpage;

	/* REG_NFC_SYSCTRL1: disable free-run mode */
	_reg = nfc_read32(REG_NFC_SYSCTRL1);
	nvt_bclear(_reg, NFC_SYSCTRL1_FRUN_FULL_PAGE_DMA);
	nfc_write32(REG_NFC_SYSCTRL1, _reg);

	/* REG_NFC_XFER: transfer size = subpage size */
	_reg = nfc_read32(REG_NFC_XFER);
	nvt_bclear(_reg, NFC_XFER_TRANSFER_BYTE_COUNT(-1));
	nvt_set(_reg, NFC_XFER_TRANSFER_BYTE_COUNT(p->subpage_size));
	nfc_write32(REG_NFC_SYSCTRL1, _reg);

	/* REG_NFC_SYSCTRL: extra size = subextra size */
	_reg = nfc_read32(REG_NFC_SYSCTRL);
	nvt_bclear(_reg, NFC_SYSCTRL_EXTRA_SIZE(-1));
	nvt_set(_reg, NFC_SYSCTRL_EXTRA_SIZE(subextra_size));
	nfc_write32(REG_NFC_SYSCTRL, _reg);

	/* dma regs */
	/* nfc_write32(REG_NFC_ROWADR, page_addr); */


	/* REG_NFC_SYSADR */
	nfc_write32(REG_NFC_SYSADR, info->dma_data + subpage_offset);

	/* REG_NFC_XTRA_ADDR */
	nfc_write32(REG_NFC_XTRA_ADDR, info->dma_data + p->page_size +
		    subextra_offset);

	/* REG_NFC_COLADR */
	nfc_write32(REG_NFC_COLADR, subpage_offset);

	/* REG_NFC_RNDACC */
	_reg = nfc_read32(REG_NFC_RNDACC);
	nvt_bclear(_reg, NFC_RNDACC_COL_ADDR(-1));
	nvt_set(_reg, NFC_RNDACC_COL_ADDR(subpage_offset));
	nfc_write32(REG_NFC_RNDACC, _reg);
}
#endif

void nt72_nand_nfc_read_page(struct nt72_nand_info *info, int column,
			     int page_addr, int use_ecc, int oob_only,
			     int use_inv)
{
	int retry_count = READ_RETRY_COUNT;

	nt72_nand_lock_nfc(info);

#ifndef USE_ECC_INV
	use_inv = 0;
#endif
#ifdef IS_XBOOT
	column = 0;
	use_ecc = 1;
	oob_only = 0;
#endif

do_retry:
	nt72_nand_cmd_hist_add("RD");
	nt72_nand_cmd_hist_add_attr(use_ecc, oob_only, use_inv,
				    retry_count < READ_RETRY_COUNT);

	nt72_nand_nfc_ecc_workaround(info);
	info->retcode = nt72_nand_nfc_check_ready("read_page", info);
	if (unlikely(info->retcode))
		goto has_error;

	nt72_nand_nfc_set_rw_regs(info, 0, 1, use_inv);
	if (unlikely(!use_ecc))
		nt72_nand_nfc_disable_ecc(info);
	nt72_nand_set_dma_regs(info, column, page_addr, 0, use_ecc);

	nt72_nand_nfc_dump_ctrl_regs("read_page");

	nt72_nand_cmd_hist_save_start_timestamp();
	nt72_nand_nfc_send_rwcmd(info, column, page_addr, 0);
	info->retcode =
		nt72_nand_ir_wait(info, "read_page",
				  NFC_INTSTS_FULL_PAGE_DMA_TERMINATED |
				  NFC_INTSTS_ERRORS | NFC_INTSTS_TIMEOUTS);
	nt72_nand_cmd_hist_save_end_timestamp();
	if (unlikely(info->retcode))
		goto has_error;

	info->retcode = nt72_nand_nfc_check_ready("read_page_after", info);
	if (unlikely(info->retcode))
		goto has_error;

	info->retcode = nt72_nand_nfc_check_int_status("read_page", info);
	if (unlikely(info->retcode))
		goto has_error;

	if (likely(use_ecc)) {
		/* Read raw ECC info */
		nt72_nand_nfc_get_ecc_result(info);
		/* For blank subpage: fill and remove uncorrectable mark */
#ifndef ENABLE_BLANK_CHECK
		nt72_nand_nfc_fill_page(info, page_addr, oob_only, use_inv);
#endif
		if (unlikely(info->ecc_result.uncorrectable)) {
			nfc_err("nfc_err: ECC uncorrectable: %d\n", page_addr);
			info->retcode = ERR_ECC_FAIL;
			goto has_error;
		}
	}

	if (unlikely(retry_count < READ_RETRY_COUNT)) {
		nfc_msg("nfc: retry success, dump good page: %d\n", page_addr);
		nt72_nand_nfc_dump_regs(info, page_addr);
		nt72_nand_nfc_dump_ecc(info, page_addr, use_inv);
		nt72_nand_nfc_dump_data(info, page_addr);
		nt72_nand_cmd_hist_dump();
	} else {
#if defined(DUMP_ECC_ON_BLANK)
		if ((info->ecc_result.err_count || info->ecc_result.blank))
			nt72_nand_nfc_dump_ecc(info, page_addr, use_inv);
#elif defined(DUMP_ECC_ON_BITFLIP)
		if (info->ecc_result.err_count)
			nt72_nand_nfc_dump_ecc(info, page_addr, use_inv);
#endif
	}

#ifdef HAS_NTS
	nts_do_random_bitflips(info, page_addr);
#endif

	nt72_nand_unlock_nfc(info);
	return;

has_error:
	nt72_nand_nfc_error_handling(info, page_addr, 1, use_inv);
	if (retry_count > 0) {
		nfc_msg("nfc: read_page retry, count = %d\n",
			retry_count);
		retry_count--;
		goto do_retry;
	}
	nfc_err("nfc_err: read_page failed: %d\n", info->retcode);
	nt72_nand_unlock_nfc(info);
}

void nt72_nand_nfc_erase_block(struct nt72_nand_info *info, int page_addr)
{
	nt72_nand_lock_nfc(info);

	nt72_nand_cmd_hist_add("ERASE");
	info->retcode = nt72_nand_nfc_check_ready("erase_block", info);
	if (unlikely(info->retcode))
		goto has_error;

	nt72_nand_nfc_init_pio_regs(info);

	nfc_write32(REG_NFC_ROWADR, page_addr);
	nfc_write32(REG_NFC_COLADR, 0x00);

	nt72_nand_nfc_dump_ctrl_regs("eraseblock");

	nt72_nand_cmd_hist_save_start_timestamp();
	nt72_nand_nfc_send_piocmd(info, 0, NFC_CMD_CHIP_SEL(TARGET_CHIP) |
				  NFC_CMD_WP_NEG |
				  NFC_CMD_ADDR_CYCLE_ROW |
				  NFC_CMD_END_WAIT_READY |
				  NFC_CMD_CYCLE_TWO |
				  NFC_CMD_CODE1(0xd0) |
				  NFC_CMD_CODE0(0x60));
	info->retcode = nt72_nand_ir_wait(info, "erase_block",
					  NFC_INTSTS_ERRORS |
					  NFC_INTSTS_TIMEOUTS |
					  NFC_INTSTS_CMD_COMPLETE);
	nt72_nand_cmd_hist_save_end_timestamp();
	if (unlikely(info->retcode))
		goto has_error;

	info->retcode = nt72_nand_nfc_check_ready("erase_block_after", info);
	if (unlikely(info->retcode))
		goto has_error;

	info->retcode = nt72_nand_nfc_check_int_status("erase_block", info);
	if (unlikely(info->retcode))
		goto has_error;

	nt72_nand_nfc_ecc_workaround(info);

	nt72_nand_unlock_nfc(info);
	return;
has_error:
	nt72_nand_nfc_error_handling(info, 0, 0, 0);
	nfc_err("nfc_err: erase_block failed: %d\n", info->retcode);
	nt72_nand_unlock_nfc(info);
}

void nt72_nand_nfc_write_page(struct nt72_nand_info *info,
			      int column, int page_addr, int use_ecc,
			      int use_inv)
{
#ifndef USE_ECC_INV
	use_inv = 0;
#endif

	nt72_nand_lock_nfc(info);
	nt72_nand_cmd_hist_add("PROG");
	nt72_nand_cmd_hist_add_attr(use_ecc, 0, use_inv, 0);

	nt72_nand_nfc_ecc_workaround(info);
	info->retcode = nt72_nand_nfc_check_ready("write_page", info);
	if (unlikely(info->retcode))
		goto has_error;

	nt72_nand_nfc_set_rw_regs(info, 1, use_ecc, use_inv);
	if (unlikely(!use_ecc))
		nt72_nand_nfc_disable_ecc(info);
	nt72_nand_set_dma_regs(info, column, page_addr, 1, use_ecc);

	nt72_nand_nfc_dump_ctrl_regs("write_page");

	nt72_nand_cmd_hist_save_start_timestamp();
	nt72_nand_nfc_send_rwcmd(info, column, page_addr, 1);
	info->retcode = nt72_nand_ir_wait(info, "write_page",
					  NFC_INTSTS_FULL_PAGE_DMA_TERMINATED |
					  NFC_INTSTS_ERRORS |
					  NFC_INTSTS_TIMEOUTS);
	nt72_nand_cmd_hist_save_end_timestamp();
	if (unlikely(info->retcode))
		goto has_error;

	info->retcode = nt72_nand_nfc_check_ready("write_page_after", info);
	if (unlikely(info->retcode))
		goto has_error;

	info->retcode = nt72_nand_nfc_check_int_status("write_page", info);
	if (unlikely(info->retcode))
		goto has_error;

	nt72_nand_unlock_nfc(info);
	return;
has_error:
	nt72_nand_nfc_error_handling(info, 0, 0, 0);
	nfc_err("nfc_err: write_page failed: %d\n", info->retcode);
	nt72_nand_unlock_nfc(info);
}

void nt72_nand_nfc_reset_nand(struct nt72_nand_info *info)
{
	nt72_nand_cmd_hist_add("RST");
	info->retcode = nt72_nand_nfc_check_ready("reset_nand", info);
	if (unlikely(info->retcode))
		goto has_error;

	nt72_nand_nfc_init_pio_regs(info);

	nt72_nand_cmd_hist_save_start_timestamp();
	nt72_nand_nfc_send_piocmd(info, 0, NFC_CMD_CHIP_SEL(TARGET_CHIP) |
				  NFC_CMD_END_WAIT_READY |
				  NFC_CMD_CYCLE_ONE |
				  NFC_CMD_CODE0(0xFF));
	info->retcode = nt72_nand_ir_busy_wait(info, "reset_nand",
					       NFC_INTSTS_ERRORS |
					       NFC_INTSTS_TIMEOUTS |
					       NFC_INTSTS_CMD_COMPLETE);
	nt72_nand_cmd_hist_save_end_timestamp();
	if (unlikely(info->retcode))
		goto has_error;

	info->retcode = nt72_nand_nfc_check_ready("reset_nand_after", info);
	if (unlikely(info->retcode))
		goto has_error;

	info->retcode = nt72_nand_nfc_check_int_status("reset_nand", info);
	if (unlikely(info->retcode))
		goto has_error;

	nt72_nand_nfc_ecc_workaround(info);
	return;
has_error:
	nt72_nand_nfc_error_handling(info, 0, 0, 0);
	nfc_err("nfc_err: reset_nand failed: %d\n", info->retcode);
}
