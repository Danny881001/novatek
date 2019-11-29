#ifndef _NT72_NAND_NFC_H_
#define _NT72_NAND_NFC_H_

#include "nt72_nand.h"

#define TARGET_CHIP (0)

enum nt72_nand_ecc_type {
	RS,
	BCH,
};

enum nt72_nand_io_port {
	STBC_EMMC_IO,
	STBC_SPI_IO,
	ARM_CI_IO,
};

struct nt72_nand_ecc_param {
	u8				support_inv;
	enum nt72_nand_ecc_type		type;
	u16				nr_bits;
	u16				data_size;
	u16				extra_size;
};

struct nt72_nand_nfc_io_param {
	enum nt72_nand_io_port type;
};

enum {
	ERR_NONE = 0,
	ERR_ECC_FAIL = -1,
	ERR_NAND_TIMEOUT = -2,
	ERR_STATUS = -3,
	ERR_CTRL_TIMEOUT = -4,
};

struct nt72_nand_ecc_result {
	u16	err_count;
	u16	max_err;
	u8	uncorrectable;
	u8	blank;
	u8	_err_count[MAX_NR_SUBPAGE];
	u8	_uncorrectable[MAX_NR_SUBPAGE];
	u8	_blank[MAX_NR_SUBPAGE];
#ifdef IS_CVT
	u8	blank_count[MAX_NR_SUBPAGE];
#endif
};

struct nt72_nand_regs {
	u32	xd_cfg;

	u32	cfg0_read;
	u32	cfg0_prog;

	u32	cfg1;
	u32	cfg1_inv;

	u32	sysctrl;

	u32	sysctrl1_read;
	u32	sysctrl1_prog;

	u32	sysctrl2_read;
	u32	sysctrl2_prog;

	u32	ftune;
};

struct nt72_nand_params {
	u32				        chip_id;
	u16				        flash_width;

	u32				        page_size;
	u32				        subpage_size;
	u32				        oob_size;
	u32				        suboob_size;

	u32				        nr_blocks;
	u32				        nr_page_per_block;
	u32				        nr_subpage;

	u64				        size;

	u32				        ic_id;
	u32				        ic_ver;
	u32				        cache_line_size;
	u32				        bitflip_threshold;
	u32				        bbt_options;

	struct nt72_nand_ecc_param	        ecc;
	struct nt72_nand_regs		        regs;
};

struct nt72_nand_info {
	/* linux-specific data structures */
#if defined(IS_KERNEL) || defined(IS_UBOOT)
	struct nand_chip		chip;
	/*
	 * U-boot stores struct nand_chip inside struct mtd_info, but kernel
	 * does not. So we use pointer for portability.
	 */
	struct mtd_info			*mtd;
#endif

#if defined(IS_KERNEL)
	/* For kernel, you should handle struct mtd_info by yourself */
	struct mtd_info			_mtd;
	struct device			*dev;
	struct platform_device		*pdev;
#endif

	struct completion		nfc_completion;

	dma_addr_t			dma_data;
	dma_addr_t			dma_oob;

#ifdef MTD_COPY_FREE
	dma_addr_t			dma_data_kernel;
	dma_addr_t			dma_data_mtd_a;
	dma_addr_t			dma_data_mtd_ua;
	dma_addr_t			_dma_data_mtd_ua;
#endif

	/* general data structures */
	int				raw_write;

	unsigned int			column;
	int				prog_addr;

	unsigned char			*data;
	unsigned char			*oob;

#ifdef MTD_COPY_FREE
	unsigned char			*data_kernel;
	unsigned char			*data_mtd_a;
	unsigned char			*data_mtd_ua;
	unsigned char			*_data_mtd_ua;
#endif

	struct nt72_nand_params		nand_params;
	struct nt72_nand_ecc_result	ecc_result;
	struct nt72_nand_nfc_io_param	io_param;

	int				retcode;
#ifdef HAS_DEBUGFS
	struct dentry			*dfs;
	u32				dfs_addr;
	u32				dfs_cmd;
#endif
};

void nt72_nand_nfc_dump_data(struct nt72_nand_info *info, int page_addr);
void nt72_nand_nfc_disable_ecc(struct nt72_nand_info *info);
void nt72_nand_nfc_hw_reset(void);
void nt72_nand_nfc_sw_reset(struct nt72_nand_info *info);
void nt72_nand_nfc_ecc_reset(struct nt72_nand_info *info);
void nt72_nand_nfc_read_id(struct nt72_nand_info *info, int column, u32 *id,
			   u32 *id_ext, int probe);
void nt72_nand_nfc_read_status(struct nt72_nand_info *info, uint32_t *status);
void nt72_nand_nfc_read_page(struct nt72_nand_info *info, int column,
			     int page_addr, int use_ecc, int oob_only,
			     int use_inv);
void nt72_nand_nfc_erase_block(struct nt72_nand_info *info, int page_addr);
void nt72_nand_nfc_write_page(struct nt72_nand_info *info, int column,
			      int page_addr, int use_ecc, int use_inv);
void nt72_nand_nfc_reset_nand(struct nt72_nand_info *info);

#endif /* #define _NT72_NAND_NFC_H_ */
