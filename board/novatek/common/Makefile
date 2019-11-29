# Copyright (C) 2015-2016 Novatek corp.
#
# SPDX-License-Identifier:	GPL-2.0+
#
KBUILD_CFLAGS := -fno-builtin -Werror -Iboard/$(BOARDDIR)/../common/include

ifeq ($(CONFIG_NT72673), y)
obj-y += crypto_core_673.o
else
obj-y += crypto_core.o
endif

obj-$(CONFIG_NVT_TK_EMMC_UTILS) += nvt_tk_gpt.o nvt_emmc_fwupdate.o nvt_emmc_partition.o nvt_emmc_xbootdat.o nvt_emmc_utils.o nvt_common_utils.o
obj-$(CONFIG_NVT_TK_NAND_UTILS) += nt72_npt.o   nvt_nand_fwupdate.o nvt_nand_partition.o nvt_nand_xbootdat.o nvt_nand_utils.o nvt_common_utils.o
obj-$(CONFIG_NVT_MMC_INFO) += nvt_emmc_info.o
obj-$(CONFIG_NVT_TK_SPI_UTILS) += nvt_spi_fwupdate.o nvt_spi_partition.o
obj-$(CONFIG_NVTTK_EBURNER_V2) += nvt_eburner.o
obj-$(CONFIG_NVT_FM_TOOL) += nvt_tkfm.o
obj-$(CONFIG_NVT_BOOTM_LOAD_SECOS) += nvt_tzone.o nvt_sys.o
obj-$(CONFIG_NVT_GWRITE) += nvt_gwrite.o
obj-$(CONFIG_NVT_SECURE_BOOT) += nvt_kx_hash.o nvt_vtbl_check.o
