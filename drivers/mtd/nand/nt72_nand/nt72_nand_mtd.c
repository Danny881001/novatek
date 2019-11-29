#include "include/nt72_nand.h"

#ifdef USE_BBT
static uint8_t bbt_pattern[] = { 'B', 't', '0' };
static uint8_t mirror_pattern[] = { '1', 't', 'B' };
static struct nand_bbt_descr bbt_main_descr = {
	.options	= NAND_BBT_LASTBLOCK | NAND_BBT_CREATE | NAND_BBT_WRITE
	| NAND_BBT_2BIT | NAND_BBT_VERSION | NAND_BBT_PERCHIP,
	.offs		= 0,
	.len		= 3,
	.veroffs	= 3,
	.maxblocks	= 4,
	.pattern	= bbt_pattern,
#ifdef USE_BBT_CHECKSUM
	.check_sum_offs = 32,
	.check_sum_len  = 3,
#endif
};

static struct nand_bbt_descr bbt_mirror_descr = {
	.options	= NAND_BBT_LASTBLOCK | NAND_BBT_CREATE | NAND_BBT_WRITE
	| NAND_BBT_2BIT | NAND_BBT_VERSION | NAND_BBT_PERCHIP,
	.offs		= 0,
	.len		= 3,
	.veroffs	= 3,
	.maxblocks	= 4,
	.pattern	= mirror_pattern,
#ifdef USE_BBT_CHECKSUM
	.check_sum_offs = 32,
	.check_sum_len  = 3,
#endif
};

#ifdef USE_BBT_CHECKSUM
static uint8_t scan_ff_pattern[] = { 0xff };
static struct nand_bbt_descr nvt_bbt_descr = {
	.options        = 0,
	.offs           = 0,
	.len            = 1,
	.pattern        = scan_ff_pattern,
};
#endif /* USE_BBT_CHECKSUM */
#endif /* USE_BBT */

static int nt72_nand_update_ecc_stats(struct mtd_info *mtd)
{
	struct nt72_nand_info *info = mtd->priv;

	if (unlikely(info->retcode == ERR_ECC_FAIL)) {
		/*
		 * We have no data on uncorrectable error count, let it be 1.
		 * Also we had corrected nothing, so return early.
		 * The MTD layer will notice that the ecc_stats.failed is
		 * incremented and return -EBADMSG to mtd_read.
		 */
		mtd->ecc_stats.failed++;
		return 0;
	} else if (likely(info->retcode == ERR_NONE)) {
		/*
		* Let MTD layer know how many errors are corrected by ECC,
		* although UBI does not use this value, ISW's NTS need it.
		*/
		mtd->ecc_stats.corrected += info->ecc_result.err_count;
		/*
		* Return the max corrected error count among every ECC steps
		* during this read.
		* If this max value is greater than bitflip_threshold, mtd_read
		* will return -EUCLEAN. UBI will do scrubbing in this case.
		*/
		return info->ecc_result.max_err;
	}

	/*
	* Prevent nand_base.c copy the buffer content to user space,
	* also the -EIO is returned to mtd_read.
	*/
	return -EIO;
}

static int nt72_nand_dev_ready(struct mtd_info *mtd)
{
	return 1;
}

static int nt72_nand_inv_range(struct nt72_nand_info *info, int page_addr)
{
#if defined(IS_UBOOT)
	return g_use_inv;
#elif defined(USE_ECC_INV_TABLE)
	int page_per_blk = info->nand_params.nr_page_per_block;

	return g_nt72_nand_inv_map.block[page_addr / page_per_blk].val;
#else
	return 0;
#endif /* USE_ECC_INV_TABLE */
}

#ifndef STBC_ECC_SWITCH
#define nt72_nand_stbc_ecc_switch(...) do { } while (0)
#else
static void nt72_nand_stbc_ecc_switch(struct nt72_nand_info *info, int command,
				      int page_addr)
{
	struct nt72_nand_params *p = &g_info->nand_params;

	if (command == NAND_CMD_PAGEPROG)
		page_addr = info->prog_addr;

	/* abort if the access is outside STBC partition */
	if (likely((page_addr >= 0x600) || (page_addr < 0x4C0)))
		return;

	/* below code is for STBC upgrade only */

	if ((command != NAND_CMD_READ0) && (command != NAND_CMD_READOOB) &&
	    (command != NAND_CMD_PAGEPROG))
		return;

	if (p->ecc.type == BCH) {
		p->ecc.type = RS;
		p->ecc.nr_bits = 4;
		p->ecc.data_size = 512;
		p->ecc.extra_size = 16;
	} else {
		p->ecc.type = BCH;
		p->ecc.nr_bits = 8;
		p->ecc.data_size = 512;
		p->ecc.extra_size = 16;
	}

	nt72_nand_nfc_regs_init(g_info);

	nt72_nand_nfc_sw_reset(g_info);
}

int nt72_nand_get_ecc(void)
{
	return (g_info->nand_params.ecc.type == RS) ? 0 : 1;
}
EXPORT_SYMBOL(nt72_nand_get_ecc);

void nt72_nand_set_ecc(int ecc_type)
{
	struct nt72_nand_params *p = &g_info->nand_params;

	if (ecc_type == 0) {
		p->ecc.type = RS;
		p->ecc.nr_bits = 4;
		p->ecc.data_size = 512;
		p->ecc.extra_size = 16;
	} else {
		p->ecc.type = BCH;
		p->ecc.nr_bits = 8;
		p->ecc.data_size = 512;
		p->ecc.extra_size = 16;
	}

	nt72_nand_nfc_regs_init(g_info);

	nt72_nand_nfc_sw_reset(g_info);
}
EXPORT_SYMBOL(nt72_nand_set_ecc);
#endif

static void nt72_nand_inv_cache(void)
{
#ifdef IS_UBOOT
	struct nt72_nand_params *p = &g_info->nand_params;

	invalidate_dcache_range(g_info->dma_data, g_info->dma_data + p->page_size);
	invalidate_dcache_range(g_info->dma_oob, g_info->dma_oob + p->oob_size);
#endif
}

static void nt72_nand_cmdfunc(struct mtd_info *mtd, unsigned int command,
			      int column, int page_addr)
{
	struct nt72_nand_info *info = mtd->priv;

	info->retcode = ERR_NONE;

	nt72_nand_stbc_ecc_switch(info, command, page_addr);
	nt72_nand_inv_cache();

	switch (command) {
	case NAND_CMD_READOOB:
		info->column = mtd->writesize + column;
		/* We actaully read whole page for ECC */
		nt72_nand_nfc_read_page(info, 0, page_addr, 1, 1,
					nt72_nand_inv_range(info, page_addr));
		/*
		 * We use the default read OOB function from nand_base.c, which
		 * does not update ECC stats.
		 */
		nt72_nand_update_ecc_stats(mtd);
		break;
	case NAND_CMD_READ0:
		info->column = column;
#ifndef MTD_COPY_FREE
		nt72_nand_nfc_read_page(info, column, page_addr, 1, 0,
					nt72_nand_inv_range(info, page_addr));
#endif
		break;
	case NAND_CMD_SEQIN:
		nt72_nand_cmd_hist_add("seqin");
		nt72_nand_cmd_hist_save_start_timestamp();

		info->column = column;
		info->prog_addr = page_addr;

		nt72_nand_cmd_hist_save_end_timestamp();
		break;
	case NAND_CMD_PAGEPROG:
		if (unlikely(info->raw_write)) {
			info->raw_write = 0;
			nt72_nand_nfc_write_page(info, info->column,
						 info->prog_addr, 0, 0);
		} else {
			int inv = nt72_nand_inv_range(info, info->prog_addr);

			nt72_nand_nfc_write_page(info, info->column,
						 info->prog_addr, 1, inv);
		}
		break;
	case NAND_CMD_ERASE1:
		nt72_nand_nfc_erase_block(info, page_addr);
		break;
	case NAND_CMD_ERASE2:
		break;
	case NAND_CMD_READID:
#ifdef MTD_COPY_FREE
		info->dma_data = info->dma_data_kernel;
		info->data = info->data_kernel;
#endif
		nt72_nand_nfc_read_id(info, column, NULL, NULL, 0);
		break;
	case NAND_CMD_STATUS:
#ifdef MTD_COPY_FREE
		info->dma_data = info->dma_data_kernel;
		info->data = info->data_kernel;
#endif
		nt72_nand_nfc_read_status(info, NULL);
		break;
	case NAND_CMD_RESET:
		/* ignore reset command because we had done it during probe */
		break;
	default:
		nfc_err("nfc_err: unsupported command: 0x%X\n", command);
		break;
	}

	nt72_nand_stbc_ecc_switch(info, command, page_addr);
}

static void nt72_nand_read_buf(struct mtd_info *mtd, uint8_t *buf, int len)
{
	struct nt72_nand_info *info = mtd->priv;
	int page_size = info->nand_params.page_size;

	if (likely(info->column < page_size)) {
		/* only data */
		memcpy(buf, info->data + info->column, len);
	} else if (unlikely(info->column >= page_size)) {
		/* only oob */
		memcpy(buf, info->oob + info->column - page_size, len);
	} else {
		/* should not be here */
		goto has_error;
	}

	info->column += len;

	return;
has_error:
	nfc_err("nfc_err: read_buf error, col: %d, len: %d\n", info->column,
		len);
}

static uint8_t nt72_nand_read8(struct mtd_info *mtd)
{
	u8 data8;

	nt72_nand_read_buf(mtd, &data8, 1);

	return data8;
}

static u16 nt72_nand_read16(struct mtd_info *mtd)
{
	u16 data16;

	nt72_nand_read_buf(mtd, (u8 *)&data16, 2);

	return data16;
}

static void nt72_nand_write_buf(struct mtd_info *mtd, const uint8_t *buf,
				int len)
{
	/* This function is used for write OOB only:
	 * NAND_CMD_SEQIN 2048 --> write_buf --> NAND_CMD_PAGEPROG
	 */
	struct nt72_nand_info *info = mtd->priv;
	struct nt72_nand_params *p = &info->nand_params;

#ifdef MTD_COPY_FREE
	/* Use the default internal driver DMA buffer */
	info->dma_data = info->dma_data_kernel;
	info->data = info->data_kernel;
#endif

	if (info->column == p->page_size) {
		/* Because we do write OOB only, fill the data with 0xFF */
		memset(info->data, 0xFF, p->page_size);
		/* Copy data to OOB buffer */
		memcpy(info->oob, buf, len);
	} else
		goto has_error;

	return;
has_error:
	nfc_err("nfc_err: write_buf error, col: %d, len: %d\n", info->column,
		len);
}

static void nt72_nand_select_chip(struct mtd_info *mtd, int chip)
{
}

static int nt72_nand_waitfunc(struct mtd_info *mtd, struct nand_chip *chip)
{
	uint32_t nand_status = 0;
	struct nt72_nand_info *info = mtd->priv;
	int ctrl_status_fail = info->retcode != ERR_NONE;

	info->retcode = ERR_NONE;

	/* NFC has errors during last command */
	if (unlikely(ctrl_status_fail))
		return NAND_STATUS_FAIL;

	/* Status reported by NAND */
	nt72_nand_nfc_read_status(info, &nand_status);
	if (unlikely(nand_status & NAND_STATUS_FAIL)) {
		nfc_err("nfc_warn: nand status fail\n");
		nt72_nand_cmd_hist_dump();
	}

	return nand_status;
}

#ifdef MTD_COPY_FREE
static int nt72_nand_read_page_hwecc_fallback(struct mtd_info *mtd,
					      struct nand_chip *chip,
					      uint8_t *buf, int oob_required,
					      int page)
{
	struct nt72_nand_info *info = mtd->priv;

	info->dma_data = info->dma_data_kernel;
	info->data = info->data_kernel;

	nt72_nand_nfc_read_page(info, 0, page, 1, 0,
				nt72_nand_inv_range(info, page));

	memcpy(buf, info->data, info->nand_params.page_size);
	if (oob_required && (chip->oob_poi != info->oob))
		memcpy(chip->oob_poi, info->oob, info->nand_params.oob_size);

	return nt72_nand_update_ecc_stats(mtd);
}
#endif

#ifdef MTD_COPY_FREE_READ_A
static int nt72_nand_read_page_hwecc_aligned(struct mtd_info *mtd,
					     struct nand_chip *chip,
					     uint8_t *buf, int oob_required,
					     int page)
{
	struct nt72_nand_info *info = mtd->priv;

	info->data_mtd_a = buf;
	info->dma_data_mtd_a =
		dma_map_single(info->dev, info->data_mtd_a,
			       info->nand_params.page_size,
			       DMA_FROM_DEVICE);
	if (dma_mapping_error(info->dev, info->dma_data_mtd_a)) {
		return nt72_nand_read_page_hwecc_fallback(mtd, chip, buf,
							  oob_required, page);
	}

	info->dma_data = info->dma_data_mtd_a;
	info->data = info->data_mtd_a;

	nt72_nand_nfc_read_page(info, 0, page, 1, 0,
				nt72_nand_inv_range(info, page));

	/* Clear DMA data buffer mapping */
	dma_unmap_single(info->dev, info->dma_data_mtd_a,
			 info->nand_params.page_size, DMA_FROM_DEVICE);

	if (oob_required && (chip->oob_poi != info->oob))
		memcpy(chip->oob_poi, info->oob, info->nand_params.oob_size);

	return nt72_nand_update_ecc_stats(mtd);
}
#endif

#ifdef MTD_COPY_FREE
static int nt72_nand_read_page_hwecc_unaligned(struct mtd_info *mtd,
					       struct nand_chip *chip,
					       uint8_t *buf, int oob_required,
					       int page)
{
	struct nt72_nand_info *info = mtd->priv;

	info->dma_data = info->dma_data_mtd_ua;
	info->data = info->data_mtd_ua;

	nt72_nand_nfc_read_page(info, 0, page, 1, 0,
				nt72_nand_inv_range(info, page));

	if (oob_required && (chip->oob_poi != info->oob))
		memcpy(chip->oob_poi, info->oob, info->nand_params.oob_size);

	return nt72_nand_update_ecc_stats(mtd);
}
#endif

static int nt72_nand_read_page_hwecc(struct mtd_info *mtd,
				     struct nand_chip *chip, uint8_t *buf,
				     int oob_required, int page)
{
#ifdef MTD_COPY_FREE
	int mtd_unaligned = (chip->buffers->databuf == buf);
#ifdef MTD_COPY_MTD_COPY_FREE_READ_A
	int direct_data = ((u32)buf % 64) == 0;
#endif

	if (mtd_unaligned) {
		return nt72_nand_read_page_hwecc_unaligned(mtd, chip, buf,
							   oob_required, page);
	}
#ifdef MTD_COPY_MTD_COPY_FREE_READ_A
	if (direct_data) {
		return nt72_nand_read_page_hwecc_aligned(mtd, chip, buf,
							 oob_required, page);
	}
#endif

	return nt72_nand_read_page_hwecc_fallback(mtd, chip, buf,
						  oob_required, page);
#else
	struct nt72_nand_info *info = mtd->priv;

	memcpy(buf, info->data, info->nand_params.page_size);
	if (oob_required && (chip->oob_poi != info->oob))
		memcpy(chip->oob_poi, info->oob, info->nand_params.oob_size);
#endif

	return nt72_nand_update_ecc_stats(mtd);
}

#ifdef MTD_COPY_FREE_WRITE_UA
#ifdef IS_UBOOT
static int nt72_nand_write_page_hwecc_unaligned(struct mtd_info *mtd,
						struct nand_chip *chip,
						const uint8_t *buf,
						int oob_required, int page)
#else
static int nt72_nand_write_page_hwecc_unaligned(struct mtd_info *mtd,
						struct nand_chip *chip,
						const uint8_t *buf,
						int oob_required)
#endif
{
	struct nt72_nand_info *info = mtd->priv;
	struct nt72_nand_params *p = &info->nand_params;

	info->dma_data = info->dma_data_mtd_ua;
	info->data = info->data_mtd_ua;
	if (oob_required && (info->oob != chip->oob_poi))
		memcpy(info->oob, chip->oob_poi, p->oob_size);
	else
		memset(info->oob, 0xFF, p->oob_size);

	return 0;
}
#endif

#ifdef IS_UBOOT
static int nt72_nand_write_page_hwecc(struct mtd_info *mtd,
				      struct nand_chip *chip,
				      const uint8_t *buf, int oob_required,
				      int page)
#else
static int nt72_nand_write_page_hwecc(struct mtd_info *mtd,
				      struct nand_chip *chip,
				      const uint8_t *buf, int oob_required)
#endif
{
	/* This function is used for full page program only:
	 * NAND_CMD_SEQIN 0 --> write_page_hwecc --> NAND_CMD_PAGEPROG
	 */

#ifdef MTD_COPY_FREE_WRITE_UA
	struct nt72_nand_info *info = mtd->priv;
	struct nt72_nand_params *p = &info->nand_params;

	if (likely(info->column == 0)) {
		if (buf == chip->buffers->databuf) {
			/* This buffer is used for MTD partial write conversion,
			 * so we actually write whole page here
			 */
			int oob = oob_required;

			return nt72_nand_write_page_hwecc_unaligned(mtd, chip,
								    buf, oob);
		} else {
			/* program whole page using driver's DMA buffer */
			info->dma_data = info->dma_data_kernel;
			info->data = info->data_kernel;

			memcpy(info->data, buf, p->page_size);
			if (oob_required && (info->oob != chip->oob_poi))
				memcpy(info->oob, chip->oob_poi, p->oob_size);
			else
				memset(info->oob, 0xFF, p->oob_size);
		}
	} else
		goto has_error;

	return 0;
has_error:
	nfc_err("nfc_err: write_page_hwecc error, col: %d\n", info->column);
	return -1;
#else
	struct nt72_nand_info *info = mtd->priv;
	struct nt72_nand_params *p = &info->nand_params;

	if (likely(info->column == 0)) {
		/* program whole page */
		memcpy(info->data, buf, p->page_size);
		if (oob_required && (info->oob != chip->oob_poi))
			memcpy(info->oob, chip->oob_poi, p->oob_size);
		else
			memset(info->oob, 0xFF, p->oob_size);
	} else
		goto has_error;

	return 0;
has_error:
	nfc_err("nfc_err: write_page_hwecc error, col: %d\n", info->column);
	return -1;
#endif
}

static int nt72_nand_read_page_raw(struct mtd_info *mtd, struct nand_chip *chip,
				   uint8_t *buf, int oob_required, int page)
{
#ifdef MTD_COPY_FREE
	/* Identical to nt72_nand_read_page_hwecc_fallback, but with ECC off */
	struct nt72_nand_info *info = mtd->priv;

	info->dma_data = info->dma_data_kernel;
	info->data = info->data_kernel;

	nt72_nand_nfc_read_page(info, 0, page, 0, 0, 0);

	memcpy(buf, info->data, info->nand_params.page_size);
	if (oob_required && (chip->oob_poi != info->oob))
		memcpy(chip->oob_poi, info->oob, info->nand_params.oob_size);

	return 0;
#else
	struct nt72_nand_info *info = mtd->priv;

	nt72_nand_nfc_read_page(info, info->column, page, 0, 0, 0);
	nt72_nand_read_page_hwecc(mtd, chip, buf, oob_required, page);

	return 0;
#endif
}

#ifdef IS_UBOOT
static int nt72_nand_write_page_raw(struct mtd_info *mtd,
				    struct nand_chip *chip, const uint8_t *buf,
				    int oob_required, int page)
#else
static int nt72_nand_write_page_raw(struct mtd_info *mtd,
				    struct nand_chip *chip, const uint8_t *buf,
				    int oob_required)
#endif
{
	struct nt72_nand_info *info = mtd->priv;

	info->raw_write = 1;
#ifdef IS_UBOOT
	nt72_nand_write_page_hwecc(mtd, chip, buf, oob_required, page);
#else
	nt72_nand_write_page_hwecc(mtd, chip, buf, oob_required);
#endif

	return 0;
}

static void nt72_nand_ecc_hwctl(struct mtd_info *mtd, int mode)
{
}

static int nt72_nand_ecc_calculate(struct mtd_info *mtd, const uint8_t *dat,
				   uint8_t *ecc_code)
{
	return 0;
}

static int nt72_nand_ecc_correct(struct mtd_info *mtd, uint8_t *dat,
				 uint8_t *read_ecc, uint8_t *calc_ecc)
{
	return 0;
}

#ifdef USE_BBT
static void __init nt72_nand_bbt_init(struct nt72_nand_info *info)
{
	struct nand_chip *this = &info->chip;

	this->bbt_options |=
		(NAND_BBT_USE_FLASH | info->nand_params.bbt_options);

	bbt_main_descr.veroffs = bbt_mirror_descr.veroffs = 1;
	bbt_main_descr.offs = bbt_mirror_descr.offs =
				      info->nand_params.oob_size /
				      info->nand_params.nr_subpage;

#ifdef USE_BBT_CHECKSUM
	bbt_main_descr.check_sum_offs = info->nand_params.suboob_size * 2;
	bbt_main_descr.check_sum_len = 3;

	bbt_mirror_descr.check_sum_offs = info->nand_params.suboob_size * 2;
	bbt_mirror_descr.check_sum_len = 3;

	this->badblock_pattern  = &nvt_bbt_descr;
#endif

	this->bbt_td = &bbt_main_descr;
	this->bbt_md = &bbt_mirror_descr;

	nfc_msg("nfc: main_bbt: offs=%d len=%d veroffs=%d maxblocks=%d\n",
		bbt_main_descr.offs, bbt_main_descr.len,
		bbt_main_descr.veroffs, bbt_main_descr.maxblocks);
	nfc_msg("nfc: mirror_bbt: offs=%d len=%d veroffs=%d maxblocks=%d\n",
		bbt_mirror_descr.offs, bbt_mirror_descr.len,
		bbt_mirror_descr.veroffs, bbt_mirror_descr.maxblocks);
}
#endif /* USE_BBT */

void __init nt72_nand_init_nand_chip(struct nt72_nand_info *info)
{
	struct nand_chip *this = &info->chip;
	struct nt72_nand_params *p = &info->nand_params;

	this->options = (p->flash_width == 16) ? NAND_BUSWIDTH_16 : 0;

	this->read_byte = nt72_nand_read8;
	this->read_word = nt72_nand_read16;
	this->write_buf = nt72_nand_write_buf;
	this->read_buf = nt72_nand_read_buf;
	this->select_chip = nt72_nand_select_chip;

	this->dev_ready = nt72_nand_dev_ready;
	this->cmdfunc = nt72_nand_cmdfunc;
	this->waitfunc = nt72_nand_waitfunc;

	this->ecc.hwctl = nt72_nand_ecc_hwctl;
	this->ecc.calculate = nt72_nand_ecc_calculate;
	this->ecc.correct = nt72_nand_ecc_correct;
	this->ecc.read_page = nt72_nand_read_page_hwecc;
	this->ecc.write_page = nt72_nand_write_page_hwecc;
	this->ecc.read_page_raw = nt72_nand_read_page_raw;
	this->ecc.write_page_raw = nt72_nand_write_page_raw;

	this->ecc.mode = NAND_ECC_HW;
	this->ecc.size = p->page_size;

	this->ecc.strength = p->ecc.nr_bits;
	info->mtd->bitflip_threshold = p->bitflip_threshold;

	if (p->page_size == 2048) {
		if ((p->ecc.type == BCH) && (p->ecc.nr_bits == 8) &&
		    (p->ecc.data_size == 512) && (p->ecc.extra_size == 16))
			this->ecc.layout = &g_hw_2KB_page_bch8_16_ecclayout;
		else if ((p->ecc.type == RS) && (p->ecc.nr_bits == 4) &&
			 (p->ecc.data_size == 512) && (p->ecc.extra_size == 16))
			this->ecc.layout = &g_hw_2KB_page_rs4_16_ecclayout;
		else
			nfc_err("nfc_err: error select 2KB page ECC layout\n");
	} else if (p->page_size == 4096) {
		if ((p->ecc.type == BCH) && (p->ecc.nr_bits == 8) &&
		    (p->ecc.data_size == 512) && (p->ecc.extra_size == 16))
			this->ecc.layout = &g_hw_4KB_page_bch8_16_ecclayout;
		else if ((p->ecc.type == RS) && (p->ecc.nr_bits == 4) &&
			 (p->ecc.data_size == 512) && (p->ecc.extra_size == 16))
			this->ecc.layout = &g_hw_4KB_page_rs4_16_ecclayout;
		else
			nfc_err("nfc_err: error select 4KB page ECC layout\n");
	} else
		nfc_err("nfc_err: error select ECC layout\n");

	this->chip_delay = 25;

#ifdef USE_BBT
	nt72_nand_bbt_init(info);
#endif
}
