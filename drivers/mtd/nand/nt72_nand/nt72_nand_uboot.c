#include "include/nt72_nand.h"

void nt72_nand_set_inv(int inv)
{
	nfc_msg("nfc: ECC inversion mode %s\n", inv ? "on" : "off");
	g_use_inv = inv;
}

void nt72_nand_scan_bbt(struct nt72_nand_info *info)
{
	if (!(info->chip.options & NAND_SKIP_BBTSCAN) &&
	    !(info->chip.options & NAND_BBT_SCANNED)) {
		info->chip.options |= NAND_BBT_SCANNED;
		info->chip.scan_bbt(info->mtd);
	}
}

void board_nand_init(void)
{
	struct nt72_nand_info *info = g_info;
	struct mtd_info *mtd = &(info->chip.mtd);
	int ret = 0;
	struct nt72_nand_spec *nand_spec = NULL;

	/* clear nt72_nand_info */
	memset(info, 0, sizeof(*info));

	/* init nand_info */
	info->mtd = mtd;

	/*
	 * init mtd_info:
	 * mtd->priv should point to nand_chip, which is also the first item of
	 * nand_info, so we can use priv to access both of them.
	 */
	mtd->priv = info;
	mtd->owner = THIS_MODULE;

	/* Start getting up our NFC */
	nt72_nand_init_io(info);
	nt72_nand_switch_io(info);
	nt72_nand_cmd_hist_init();
	nt72_nand_bus_sniffer_init();

	ret = nt72_nand_detect_flash(info, &nand_spec);
	if (ret) {
		nfc_err("fail to detect flash\n");
		ret = -ENODEV;
		goto fail_exit;
	}

	//nt72_nand_init_params(info, nand_spec);
	nt72_nand_print_params(info);
	nt72_nand_init_nand_chip(info);

	g_info->dma_data = (u32)g_mem_area_nfc;
	g_info->data = (u8 *)g_mem_area_nfc;

	info->oob = info->data + info->nand_params.page_size;
	info->dma_oob = info->dma_data + info->nand_params.page_size;

	ret = nand_scan(mtd, 1);
	if (ret) {
		nfc_err("nt72_nand_scan failed: %d\n", ret);
		goto fail_exit;
	}
	nt72_nand_scan_bbt(info);

	/* fallback to prebuilt partition table */
	/*
	if (g_nr_nt72_nand_mtd_partitions > 1) {
		nfc_msg("nfc: Found the prebuilt partition table\n");
		ret = mtd_device_register(
			      mtd, g_nt72_nand_mtd_partitions,
			      g_nr_nt72_nand_mtd_partitions);
	} else
		nfc_err("nfc_err: partition table not found\n");
	*/

	nt72_nand_params_selftest(info);

#ifdef SHARE_GPIO_WITH_CI
	g_nfc_ready = 1;
#endif
	g_use_inv = 0;

	nand_register(0, mtd);

	return;
fail_exit:
	nfc_err("nfc_err: board_nand_init failed\n");
}
