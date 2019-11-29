/**
    NVT utilities for command customization

    @file       nt96680_utils.c
    @ingroup
    @note
    Copyright   Novatek Microelectronics Corp. 2016.  All rights reserved.

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 2 as
    published by the Free Software Foundation.
*/
#include <common.h>
#include <fs.h>
#include <asm/io.h>
#include <nand.h>
#include <mmc.h>
#include <spi_flash.h>
#include <lzma/LzmaTools.h>
#include <asm/arch/modelext/emb_partition_info.h>
#include <asm/arch/modelext/bin_info.h>
#include <asm/arch/nvt_common.h>
#include "na51000_utils.h"
#include "nvtpack.h"
#include <asm/arch-nvt-na51000/crypto.h>
#include <asm/arch/crypto.h>
#define FDT_SIZE    SZ_128K

extern HEADINFO gHeadInfo;
#ifdef CONFIG_NVT_LINUX_SPINOR_BOOT
extern struct spi_flash *board_init_spi_flash;
#endif

#if defined(CONFIG_NVT_LINUX_SPINAND_BOOT) || defined(CONFIG_NVT_LINUX_AUTOLOAD) || defined(CONFIG_NVT_LINUX_SPINOR_BOOT) || defined(CONFIG_NVT_LINUX_RAMDISK_BOOT) || defined(CONFIG_NVT_LINUX_EMMC_BOOT)
int nvt_part_config(char *p_cmdline, EMB_PARTITION *partition_ptr)
{
	EMB_PARTITION *p_partition = NULL;
	uint32_t i, part_num, pst_part_num, ro_attr;
	int ret = 0;
	char cmd[512];
	char buf[40];

#if defined(CONFIG_NVT_LINUX_RAMDISK_BOOT) || defined(CONFIG_NVT_LINUX_AUTOLOAD)
#if defined(CONFIG_NVT_SPI_NAND)
	struct mtd_info *nand = nand_info[nand_curr_device];
#elif defined(CONFIG_NVT_SPI_NOR)
	struct spi_flash *spi_flash = board_init_spi_flash;
#elif defined(CONFIG_NVT_IVOT_EMMC)
	struct mmc *mmc = find_mmc_device(CONFIG_NVT_IVOT_EMMC);
#else
	return 0;
#endif
#elif defined(CONFIG_NVT_LINUX_SPINAND_BOOT)
	struct mtd_info *nand = nand_info[nand_curr_device];
#elif defined(CONFIG_NVT_LINUX_EMMC_BOOT)
	struct mmc *mmc = find_mmc_device(CONFIG_NVT_IVOT_EMMC);
#else
	struct spi_flash *spi_flash = board_init_spi_flash;
#endif

#if defined(CONFIG_NVT_LINUX_EMMC_BOOT)
	/* MMC device doesn't need to init mtdparts parameter */
	if (p_cmdline == NULL) {
		return 0;
	}
#endif /* CONFIG_NVT_LINUX_EMMC_BOOT */

	if (partition_ptr == NULL) {
		p_partition = emb_partition_info_data_curr;
		ret = nvt_getfdt_emb((ulong)nvt_fdt_buffer, p_partition);
		if (ret < 0) {
			printf("failed to get partition resource.\r\n");
			return -1;
		}
	} else {
		/* Receive partition table from argu */
		p_partition = partition_ptr;
	}

#if defined(CONFIG_NVT_SPI_NAND)
	sprintf(cmd, "nand0=spi_nand.0");
	ret = setenv("mtdids", cmd);
	if (ret) {
		printf("%s: error set\n", __func__);
		return ret;
	}

	sprintf(cmd, "mtdparts=spi_nand.0:");
#elif defined(CONFIG_NVT_SPI_NOR)
	sprintf(cmd, "nor0=spi_nor.0");
	ret = setenv("mtdids", cmd);
	if (ret) {
		printf("%s: error set\n", __func__);
		return ret;
	}

	sprintf(cmd, "mtdparts=spi_nor.0:");
	if (p_cmdline != NULL) {
		/* To find if mtdparts string existed. If yes, it needs to expand mtdparts environment */
		char *mtdparts_off = NULL;
		mtdparts_off = strstr((char *)p_cmdline, "mtdparts=");
		if (mtdparts_off) {
			p_cmdline = mtdparts_off;
			*p_cmdline = '\0';
		}
		strcat(p_cmdline, "mtdparts=spi_nor.0:");
	}
#elif defined(CONFIG_NVT_IVOT_EMMC)
	if (p_cmdline != NULL) {
		/* To find if mtdparts string existed. If yes, it needs to expand mtdparts environment */
		char *nvtemmcpart_off = NULL;
		nvtemmcpart_off = strstr((char *)p_cmdline, "nvtemmcpart=");
		if (nvtemmcpart_off) {
			p_cmdline = nvtemmcpart_off;
			*p_cmdline = '\0';
		}
		strcat(p_cmdline, "nvtemmcpart=");
	}
#endif
	part_num = 0;
	pst_part_num = 0;
	ro_attr = 0;
	/* To parse mtdparts for rootfs partition table */
	for (i = 0 ; i < EMB_PARTITION_INFO_COUNT ; i++) {
		const EMB_PARTITION *p = p_partition + i;
		unsigned int PartitionSize = 0, PartitionOffset = 0;
#if defined(CONFIG_NVT_SPI_NAND)
		PartitionSize = p->PartitionSize;
		PartitionOffset = p->PartitionOffset;
#elif defined(CONFIG_NVT_IVOT_EMMC)
		PartitionSize = p->PartitionSize;
		PartitionOffset = p->PartitionOffset;
#else
		PartitionSize = p->PartitionSize;
		PartitionOffset = p->PartitionOffset;
#endif

		if (p->EmbType != EMBTYPE_UNKNOWN && PartitionSize == 0) {
			printf("%s skip mtdpart config of partition[%d], because its size is 0\n%s", ANSI_COLOR_YELLOW, i, ANSI_COLOR_RESET);
			continue;
		}
		switch (p->EmbType) {
		case EMBTYPE_UITRON:
			sprintf(buf, "0x%x@0x%x(uitron),", PartitionSize, PartitionOffset);
			strcat(cmd, buf);
			if (p_cmdline != NULL) {
#if defined(CONFIG_NVT_IVOT_EMMC)
				sprintf(buf, "0x%x@0x%x(uitron)ro,", PartitionSize, PartitionOffset);
				strcat(p_cmdline, buf);
#endif
			}
			break;
		case EMBTYPE_FDT:
			sprintf(buf, "0x%x@0x%x(fdt),", PartitionSize, PartitionOffset);
			strcat(cmd, buf);
			if (p_cmdline != NULL) {
#if defined(CONFIG_NVT_IVOT_EMMC)
				sprintf(buf, "0x%x@0x%x(fdt)ro,", PartitionSize, PartitionOffset);
				strcat(p_cmdline, buf);
#endif
			}
			break;
		case EMBTYPE_UBOOT:
			sprintf(buf, "0x%x@0x%x(uboot),", PartitionSize, PartitionOffset);
			strcat(cmd, buf);
			if (p_cmdline != NULL) {
#if defined(CONFIG_NVT_IVOT_EMMC)
				sprintf(buf, "0x%x@0x%x(uboot)ro,", PartitionSize, PartitionOffset);
				strcat(p_cmdline, buf);
#endif
			}
			break;
		case EMBTYPE_UENV:
			sprintf(buf, "0x%x@0x%x(uenv),", PartitionSize, PartitionOffset);
			strcat(cmd, buf);
			if (p_cmdline != NULL) {
#if defined(CONFIG_NVT_IVOT_EMMC)
				strcat(p_cmdline, buf);
#endif
			}
			break;
		case EMBTYPE_LINUX:
			sprintf(buf, "0x%x@0x%x(linux),", PartitionSize, PartitionOffset);
			strcat(cmd, buf);
			if (p_cmdline != NULL) {
#if defined(CONFIG_NVT_IVOT_EMMC)
				sprintf(buf, "0x%x@0x%x(linux)ro,", PartitionSize, PartitionOffset);
				strcat(p_cmdline, buf);
#endif
			}
			break;
		case EMBTYPE_RAMFS:
			sprintf(buf, "0x%x@0x%x(ramfs),", PartitionSize, PartitionOffset);
			strcat(cmd, buf);
			if (p_cmdline != NULL) {
#if defined(CONFIG_NVT_IVOT_EMMC)
				strcat(p_cmdline, buf);
#endif
			}
			break;
		case EMBTYPE_ROOTFS:
			sprintf(buf, "0x%x@0x%x(rootfs%d),", PartitionSize, PartitionOffset, p->OrderIdx);
			strcat(cmd, buf);
			if (p_cmdline != NULL) {
#if defined(CONFIG_NVT_IVOT_EMMC)
				strcat(p_cmdline, buf);
				/* Multiple rootfs partitions if OrderIdx > 0 */
				ro_attr = p->OrderIdx;
#endif
			}
			break;
		case EMBTYPE_ROOTFSL:
			sprintf(buf, "0x%x@0x%x(rootfsl%d),", PartitionSize, PartitionOffset, p->OrderIdx);
			strcat(cmd, buf);
			if (p_cmdline != NULL) {
#if defined(CONFIG_NVT_IVOT_EMMC)
				strcat(p_cmdline, buf);
				/* Multiple rootfs partitions if OrderIdx > 0 */
				ro_attr = p->OrderIdx;
#endif
			}
			break;
		default:
			break;
		}
	}

	if (p_cmdline != NULL) {
		/* To add entire nand mtd device */
#if defined(CONFIG_NVT_IVOT_EMMC)
		sprintf(buf, "0x%llx@0(total),", mmc->capacity / MMC_MAX_BLOCK_LEN);
		strcat(p_cmdline, buf);
#endif
		/*
		 * Add bootarg rootfs extension parameter
		 */
		p_cmdline[strlen(p_cmdline) - 1] = ' ';
		ret = nvt_getfdt_rootfs_mtd_num((ulong)nvt_fdt_buffer, &part_num, &ro_attr);
		if (ret < 0) {
			printf("%s We can't find your root device, \
					please check if your dts is without partition_rootfs name %s\n",
				   ANSI_COLOR_RED, ANSI_COLOR_RESET);
			return -1;
		}
#if defined(CONFIG_NVT_UBIFS_SUPPORT)
		if (ro_attr > 0) {
			sprintf(buf, " ubi.mtd=%d ro ", part_num);
		} else {
			sprintf(buf, " ubi.mtd=%d rw ", part_num);
		}
		strcat(p_cmdline, buf);
#elif defined(CONFIG_NVT_EXT4_SUPPORT)
		sprintf(buf, " root=/dev/mmcblk2p%d nvt_pst=/dev/mmcblk2p%d ", part_num, pst_part_num);
		strcat(p_cmdline, buf);
#elif defined(CONFIG_NVT_LINUX_RAMDISK_BOOT)
		printf("RAMDISK boot\n");
#else
		/*
		 * To handle non-ubifs rootfs type (squashfs/jffs2)
		 */
		if (ro_attr > 0) {
			sprintf(buf, " root=/dev/mtdblock%d ro ", part_num);
		} else {
			sprintf(buf, " root=/dev/mtdblock%d ", part_num);
		}
		strcat(p_cmdline, buf);
#endif /* _NVT_ROOTFS_TYPE_ */
	}

	/* To handle uboot mtd env config */
	cmd[strlen(cmd) - 1] = '\0';
	ret = setenv("mtdparts", cmd);

	sprintf(cmd, "printenv mtdparts");
	run_command(cmd, 0);

	return ret;
}
#endif /* CONFIG_NVT_LINUX_SPINAND_BOOT || CONFIG_NVT_LINUX_AUTODETECT || CONFIG_NVT_LINUX_SPINOR_BOOT || defined(CONFIG_NVT_LINUX_EMMC_BOOT) */

#ifdef  CONFIG_NVT_LINUX_AUTOLOAD

static int nvt_runfw_bin_chk_valid(ulong addr)
{
	unsigned int size = gHeadInfo.Resv1[0];
	NVTPACK_VERIFY_OUTPUT np_verify = {0};
	NVTPACK_GET_PARTITION_INPUT np_get_input;
	NVTPACK_MEM mem_in = {(void *)addr, size};

	memset(&np_get_input, 0, sizeof(np_get_input));

	if (nvtpack_verify(&mem_in, &np_verify) != NVTPACK_ER_SUCCESS) {
		printf("verify failed.\r\n");
		return -1;
	}
	if (np_verify.ver != NVTPACK_VER_16072017) {
		printf("wrong all-in-one bin version\r\n");
		return -1;
	}

	return 0;
}

static int nvt_on_partition_enum_copy_to_dest(unsigned int id, NVTPACK_MEM *p_mem, void *p_user_data)
{
	EMB_PARTITION *pEmb = (EMB_PARTITION *)p_user_data;
	u32 dst_addr = 0, dst_size = 0;
	//NVTPACK_BFC_HDR *pBFC = (NVTPACK_BFC_HDR *)p_mem->p_data;
	/* Copy to dest address */
	switch (pEmb[id].EmbType) {
	case EMBTYPE_LOADER:
		printf("RUNFW: Ignore T.bin loader\n");
		break;
	case EMBTYPE_FDT:
		break;
	case EMBTYPE_UITRON:
	case EMBTYPE_ECOS:
	case EMBTYPE_UBOOT:
	case EMBTYPE_DSP:
		break;
	case EMBTYPE_LINUX:
		dst_addr = CONFIG_LINUX_SDRAM_START;
		dst_size = p_mem->len;
		printf("RUNFW: Copy Linux from 0x%08x to 0x%08x with length %d bytes\r\n",
			   (unsigned int)p_mem->p_data, (unsigned int)dst_addr, dst_size);
		break;
	case EMBTYPE_ROOTFS:
#ifdef CONFIG_NVT_LINUX_RAMDISK_BOOT
		nvt_ramdisk_size = ALIGN_CEIL(p_mem->len, 4096);
		nvt_ramdisk_addr = _BOARD_LINUX_ADDR_ + _BOARD_LINUX_SIZE_ - nvt_ramdisk_size - 0x10000;
		dst_addr = nvt_ramdisk_addr;
		dst_size = nvt_ramdisk_size;
		if ((dst_size) > CONFIG_RAMDISK_SDRAM_SIZE) {
			printf("%sRUNFW: The ramdisk image size %d bytes is larger than %d bytes, can't boot. %s\r\n",
				   ANSI_COLOR_RED, dst_size, CONFIG_RAMDISK_SDRAM_SIZE, ANSI_COLOR_RESET);
			printf("%sRUNFW: Please check your modelext dram partition.%s\r\n", ANSI_COLOR_RED, ANSI_COLOR_RESET);
			while (1);
		} else {
			printf("RUNFW: Copy RAMDISK from 0x%08x to 0x%08x with length %d bytes\r\n",
				   (unsigned int)p_mem->p_data, (unsigned int)dst_addr, dst_size);
		}
#endif /* CONFIG_NVT_LINUX_RAMDISK_BOOT */
		break;
	default:
		break;
	}

	if (dst_size != 0) {
		memcpy((void *)dst_addr, (void *)p_mem->p_data, dst_size);
		flush_dcache_range((u32)dst_addr, (u32)(dst_addr + dst_size));
	}
	return 0;
}


static int nvt_runfw_bin_unpack_to_dest(ulong addr)
{
	u32 i;
	EMB_PARTITION *pEmb = emb_partition_info_data_curr;
	int ret = 0;
	unsigned int size = gHeadInfo.Resv1[0];
	NVTPACK_GET_PARTITION_INPUT np_get_input;
	NVTPACK_ENUM_PARTITION_INPUT np_enum_input;
	NVTPACK_MEM mem_in = {(void *)addr, size};
	NVTPACK_MEM mem_out = {0};

	memset(&np_get_input, 0, sizeof(np_get_input));
	memset(&np_enum_input, 0, sizeof(np_enum_input));

	ret = nvt_getfdt_emb((ulong)nvt_fdt_buffer, pEmb);
	if (ret < 0) {
		printf("failed to get current partition resource.\r\n");
		return -1;
	}

	/* Check for all partition existed */
	for (i = 1; i < EMB_PARTITION_INFO_COUNT; i++) {
		if (pEmb[i].PartitionSize != 0) {
			switch (pEmb[i].EmbType) {
			case EMBTYPE_FDT:
#ifndef _NVT_LINUX_SMP_ON_
			case EMBTYPE_UITRON:
#endif
			case EMBTYPE_UBOOT:
			case EMBTYPE_LINUX:
				//check this type exist in all-in-one
				np_get_input.id = i;
				np_get_input.mem = mem_in;
				if (nvtpack_get_partition(&np_get_input, &mem_out) != NVTPACK_ER_SUCCESS) {
					printf("RUNFW boot: need partition[%d]\n", i);
					return -1;
				}
				break;
			}
		}
	}

	/* Enum all partition to do necessary handling */
	np_enum_input.mem = mem_in;
	np_enum_input.p_user_data = pEmb;
	/* Loading images */
	np_enum_input.fp_enum = nvt_on_partition_enum_copy_to_dest;

	if (nvtpack_enum_partition(&np_enum_input) != NVTPACK_ER_SUCCESS) {
		printf("failed sanity.\r\n");
		return -1;
	}

	/* Make sure images had been already copied */
	flush_dcache_all();

	return 0;
}

#ifdef CONFIG_NVT_LINUX_AUTODETECT
static int nvt_boot_linux_bin_auto(void)
{
	char cmd[256] = {0};
	int ret = 0;
	image_header_t *hdr;
	uint size;
    BOOL    is_secure = 0;
    is_secure = efuse_is_secure_en();
    //is_secure = 1;
#if defined(CONFIG_NVT_LINUX_SPINOR_BOOT) || defined(CONFIG_NVT_LINUX_EMMC_BOOT)
	loff_t part_off = 0, part_size = 0;
	u32 align_size = 0;
	u64 align_off = 0;
	get_part("linux", &part_off, &part_size);
#endif

	if (nvt_detect_fw_tbin() >= 0) {
		/* Check fw is valid */
		nvt_runfw_bin_chk_valid((ulong)CONFIG_NVT_RUNFW_SDRAM_BASE);
		/* Copy linux binary to destination address */
		ret = nvt_runfw_bin_unpack_to_dest((ulong)CONFIG_NVT_RUNFW_SDRAM_BASE);
		if (ret < 0) {
			return ret;
		}
	} else {
		// get uImage size firstly
#if defined(CONFIG_NVT_LINUX_SPINAND_BOOT)
        sprintf(cmd, "nand read 0x%x linux 0x%x",CONFIG_LINUX_SDRAM_START,is_secure==0?sizeof(image_header_t):(sizeof(image_header_t)+sizeof(HEADINFO)));
#elif defined(CONFIG_NVT_LINUX_SPINOR_BOOT)
		align_size = ALIGN_CEIL(sizeof(image_header_t), ARCH_DMA_MINALIGN);
        sprintf(cmd, "sf read 0x%x 0x%llx 0x%x", CONFIG_LINUX_SDRAM_START, part_off,is_secure==0? align_size:(align_size+sizeof(HEADINFO)));
#elif defined(CONFIG_NVT_LINUX_EMMC_BOOT)
		align_off = ALIGN_CEIL(part_off, MMC_MAX_BLOCK_LEN) / MMC_MAX_BLOCK_LEN;
		sprintf(cmd, "mmc read 0x%x 0x%llx 0x%x", CONFIG_LINUX_SDRAM_START, align_off, 2);
#endif
		run_command(cmd, 0);

        if(is_secure == 0){
            hdr = (image_header_t *)CONFIG_LINUX_SDRAM_START;
                   size = image_get_data_size(hdr) + sizeof(image_header_t);
        }
        else{
            HEADINFO *encrypt_header = (HEADINFO *)(CONFIG_LINUX_SDRAM_START);
                   size = encrypt_header->BinLength;

        }

		// Reading Linux kernel image
#if defined(CONFIG_NVT_LINUX_SPINAND_BOOT)
		sprintf(cmd, "nand read 0x%x linux 0x%x", CONFIG_LINUX_SDRAM_START, size);
#elif defined(CONFIG_NVT_LINUX_SPINOR_BOOT)
		align_size = ALIGN_CEIL(size, ARCH_DMA_MINALIGN);
		sprintf(cmd, "sf read 0x%x 0x%llx 0x%x", CONFIG_LINUX_SDRAM_START, part_off, align_size);
#elif defined(CONFIG_NVT_LINUX_EMMC_BOOT)
		align_size = ALIGN_CEIL(size, MMC_MAX_BLOCK_LEN) / MMC_MAX_BLOCK_LEN;
		sprintf(cmd, "mmc read 0x%x 0x%llx 0x%x", CONFIG_LINUX_SDRAM_START, align_off, align_size);
#endif
		run_command(cmd, 0);
	}

	return 0;
}

static int nvt_boot_fw_init_detect(void)
{
	printf("NVT firmware boot.....\n");

	nvt_boot_linux_bin_auto();

	return 0;
}
#else /* !CONFIG_NVT_LINUX_AUTODETECT */

static int nvt_boot_linux_bin(void)
{
	char cmd[256] = {0};

#if defined(CONFIG_NVT_LINUX_SPINAND_BOOT)
	sprintf(cmd, "nand read 0x%x linux ", CONFIG_LINUX_SDRAM_START);
	run_command(cmd, 0);
#elif defined(CONFIG_NVT_LINUX_SPINOR_BOOT)
	loff_t part_off = 0, part_size = 0;
	/* Get linux partition offset and size */
	get_part("linux", &part_off, &part_size);
	sprintf(cmd, "sf read 0x%x 0x%llx 0x%llx", CONFIG_LINUX_SDRAM_START, part_off, part_size);
	ret = run_command(cmd, 0);
#elif defined(CONFIG_NVT_LINUX_SD_BOOT) /* CONFIG_NVT_LINUX_SD_BOOT */
	sprintf(cmd, "fatload mmc 0:1 0x%x uImage.bin ", CONFIG_LINUX_SDRAM_START);
	run_command(cmd, 0);
#elif defined(CONFIG_NVT_LINUX_EMMC_BOOT) /* CONFIG_NVT_LINUX_EMMC_BOOT */
	loff_t part_off = 0, part_size = 0;
	/* Get linux partition offset and size */
	get_part("linux", &part_off, &part_size);
	sprintf(cmd, "mmc read 0x%x 0x%llx 0x%llx ", CONFIG_LINUX_SDRAM_START, part_off, part_size);
	run_command(cmd, 0);
#else /* !defined(CONFIG_NVT_LINUX_SPINAND_BOOT) && !defined(CONFIG_NVT_LINUX_SD_BOOT) */
	int ret = 0;
	/* Check fw is valid */
	nvt_runfw_bin_chk_valid((ulong)CONFIG_NVT_RUNFW_SDRAM_BASE);
	/* Copy linux binary to destination address */
	ret = nvt_runfw_bin_unpack_to_dest((ulong)CONFIG_NVT_RUNFW_SDRAM_BASE);
	if (ret < 0) {
		return ret;
	}
#endif /* CONFIG_NVT_LINUX_SPINAND_BOOT */
}

static int nvt_boot_fw_init(void)
{
	char cmd[256] = {0};

	printf("NVT firmware boot.....\n");

#ifndef _NVT_LINUX_SMP_ON_

#endif /* !_NVT_LINUX_SMP_ON_ */
#ifdef _CPU2_NONE_
	/* Core1 will turn off core2 if core2 is not linux */
	while (1);
#endif /* _CPU2_NONE_ */
	nvt_boot_linux_bin();

	return 0;
}
#endif /* CONFIG_NVT_LINUX_AUTODETECT */
#endif /* CONFIG_NVT_LINUX_AUTOLOAD */


static int nvt_decrypt_kernel(void){



    unsigned int uimage_data_size =0;
    unsigned int uimage_total_size =0;
    unsigned char *decrypt_start_addr =NULL;
    HEADINFO *encryt_header = (HEADINFO *)(CONFIG_LINUX_SDRAM_START);
    uimage_data_size = encryt_header->BinLength - sizeof(HEADINFO);
    uimage_total_size = encryt_header->BinLength;


    unsigned int aes_encrypto_size = uimage_data_size;
    decrypt_start_addr = (unsigned char *)(CONFIG_LINUX_SDRAM_START+sizeof(HEADINFO));
    printf("====security boot start====\r\n");
    printf("aes decrypto size:%u\r\n",aes_encrypto_size);
    printf("uimage total size:%u\r\n",uimage_total_size);
    printf("decrypt_start_addr:%lx\r\n",(UINT32)decrypt_start_addr);

    CRYPT_OP crypt_op_param ={0};

    crypt_op_param.op_mode = CRYPTO_EBC;
    crypt_op_param.en_de_crypt = CRYPTO_DECRYPT;
    crypt_op_param.src_addr = (UINT32)decrypt_start_addr;
    crypt_op_param.dst_addr = (UINT32)decrypt_start_addr;
    crypt_op_param.length = aes_encrypto_size;
        #if 0
        printf("before crypt data: %x %x %x %x %x %x %x %x %x\r\n",
        *decrypt_start_addr,*(decrypt_start_addr+1),*(decrypt_start_addr+2),
        *(decrypt_start_addr+3),*(decrypt_start_addr+4),*(decrypt_start_addr+5),
        *(decrypt_start_addr+6),*(decrypt_start_addr+7),*(decrypt_start_addr+8)
        );
        printf("before crypt data: %x %x %x %x %x %x %x %x %x\r\n",
        *(decrypt_start_addr+9),*(decrypt_start_addr+10),*(decrypt_start_addr+11),
        *(decrypt_start_addr+12),*(decrypt_start_addr+13),*(decrypt_start_addr+14),
        *(decrypt_start_addr+15),*(decrypt_start_addr+16),*(decrypt_start_addr+17)
        );
        #endif

    if(crypto_data_operation(EFUSE_OTP_1ST_KEY_SET_FIELD,crypt_op_param) != E_OK){
        printf("crypto_data_operation fail\r\n");
        return -1;
    }
        #if 0
        printf("after crypt data: %x %x %x %x %x %x %x %x %x\r\n",
        *decrypt_start_addr,*(decrypt_start_addr+1),*(decrypt_start_addr+2),
        *(decrypt_start_addr+3),*(decrypt_start_addr+4),*(decrypt_start_addr+5),
        *(decrypt_start_addr+6),*(decrypt_start_addr+7),*(decrypt_start_addr+8)
        );
        printf("after crypt data: %x %x %x %x %x %x %x %x %x\r\n",
        *(decrypt_start_addr+9),*(decrypt_start_addr+10),*(decrypt_start_addr+11),
        *(decrypt_start_addr+12),*(decrypt_start_addr+13),*(decrypt_start_addr+14),
        *(decrypt_start_addr+15),*(decrypt_start_addr+16),*(decrypt_start_addr+17)
        );
        #endif

    return 0;
}


#if 1
static int nvt_check_uimage_dcrc(void){

    image_header_t *uImage_header = NULL;
    uImage_header = (image_header_t *)(CONFIG_LINUX_SDRAM_START+sizeof(HEADINFO));

    if(!image_check_dcrc((const image_header_t*)(uImage_header)))
    {
        printf(" %s linux-kernel , res check sum fail.\r\n",ANSI_COLOR_YELLOW);
        return -1;
    }
    else{
        printf(" %s check uImage crc OK!\r\n",ANSI_COLOR_YELLOW);

    }

    return 0;
}
#endif


int do_nvt_boot_cmd(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
	int ret = 0;
	char cmdline[512] = {0};
	char cmd[256] = {0};
	char buf[255] = {0};

	/*
	 * To handle bootargs expanding for the kernel /proc/cmdline
	 */
	sprintf(buf, "%s ", getenv("bootargs"));
	strcat(cmdline, buf);

	/*
	 * Loading linux kernel
	 */
#ifdef  CONFIG_NVT_LINUX_AUTOLOAD
#ifdef CONFIG_NVT_LINUX_AUTODETECT
	ret = nvt_boot_fw_init_detect();
	if (ret < 0) {
		printf("boot firmware init failed\n");
		return ret;
	}
#else /* !CONFIG_NVT_LINUX_AUTODETECT */
	ret = nvt_boot_fw_init();
	if (ret < 0) {
		printf("boot firmware init failed\n");
		return ret;
	}
#endif /* CONFIG_NVT_LINUX_AUTODETECT */
#else /* !CONFIG_NVT_LINUX_AUTOLOAD */
	/* FIXME: To do customized boot */

#endif /* CONFIG_NVT_LINUX_AUTOLOAD */

    /* security boot if need*/
    BOOL is_secure;
    is_secure = efuse_is_secure_en();
    //is_secure =1;
    if(is_secure != 0){
        ret = nvt_decrypt_kernel();
        if(ret < 0){
            printf("nvt_decrypt_kernel fail ret %d\r\n",ret);
            return -1;
        }

        ret = nvt_check_uimage_dcrc();
        if(ret != 0){
            printf("check uImage crc fail\r\n");
            return -1;
        }
    }




	nvt_tm0_cnt_end = get_nvt_timer0_cnt();
	/* boot time recording */
	sprintf(buf, "bootts=%lu,%lu resume_addr=0x%08lx user_debug=0xff", nvt_tm0_cnt_beg, nvt_tm0_cnt_end, nvt_shminfo_comm_core2_start);
	strcat(cmdline, buf);

	//set ethaddr in bootargs, if ethaddr is exist
#ifdef CONFIG_ETHNET
	if (getenv("ethaddr")) {
		sprintf(buf," ethaddr=%s ",getenv("ethaddr"));
		strcat(cmdline, buf);
	}
#endif

	setenv("bootargs", cmdline);
	/* To assign relocated fdt address */
	sprintf(buf, "0x%08x ", _BOARD_LINUX_ADDR_ + _BOARD_LINUX_SIZE_);
	setenv("fdt_high", buf);
	/*
	 * Issue boot command
	 */
#ifdef  CONFIG_NVT_LINUX_RAMDISK_BOOT
	sprintf(buf, "0x%08lx ", nvt_ramdisk_addr + nvt_ramdisk_size);
	setenv("initrd_high", buf);
	if (nvt_ramdisk_addr == 0) {
		printf("Stop boot because your ramdisk address is wrong!!! addr: 0x%08lx \n", nvt_ramdisk_addr);
		while(1);
	}
	sprintf(cmd, "bootm %x %lx %lx", CONFIG_LINUX_SDRAM_START, nvt_ramdisk_addr, (ulong)nvt_fdt_buffer);
	printf("%s\n", cmd);
#else
	printf("%s uImage is at %x, uboot fdt image is at %lx, loader tmp fdt address is at %x %s\n", ANSI_COLOR_YELLOW, CONFIG_LINUX_SDRAM_START, (ulong)nvt_fdt_buffer, nvt_readl((ulong)nvt_shminfo_boot_fdt_addr), ANSI_COLOR_RESET);
	sprintf(cmd, "bootm %x - %lx", CONFIG_LINUX_SDRAM_START, (ulong)nvt_fdt_buffer);

    printf("%s uImage is at %x, uboot fdt image is at %lx, loader tmp fdt address is at %x %s\n", ANSI_COLOR_YELLOW,is_secure == 0?CONFIG_LINUX_SDRAM_START:(CONFIG_LINUX_SDRAM_START+sizeof(HEADINFO)),(ulong)nvt_fdt_buffer, nvt_readl((ulong)nvt_shminfo_boot_fdt_addr), ANSI_COLOR_RESET);
    sprintf(cmd, "bootm %x - %lx", is_secure == 0?CONFIG_LINUX_SDRAM_START:(CONFIG_LINUX_SDRAM_START+sizeof(HEADINFO)),(ulong)nvt_fdt_buffer);


#endif
    
	printf("%s Uboot boot time: \n\tstart:\t%lu us\n\tending:\t%lu us %s\r\n", ANSI_COLOR_GREEN, nvt_tm0_cnt_beg, nvt_tm0_cnt_end, ANSI_COLOR_RESET);
    printf("bootm cmd:%s\r\n",cmd);
	run_command(cmd, 0);
	return 0;
}

U_BOOT_CMD(
	nvt_boot,   1,  0,  do_nvt_boot_cmd,
	"To do nvt platform boot init.", "\n"
);

int do_nvt_update_all_cmd(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
	ulong addr = 0, size = 0;
	int ret = 0;

	if (argc != 3) {
		return CMD_RET_USAGE;
	}

	size = cmd_get_data_size(argv[0], 4);
	if (size < 0) {
		return 1;
	}

	addr = simple_strtoul(argv[1], NULL, 16);
	size = simple_strtoul(argv[2], NULL, 10);

	printf("%s addr: 0x%08lx, size: 0x%08lx(%lu) bytes %s\r\n", ANSI_COLOR_YELLOW, addr, size, size, ANSI_COLOR_RESET);

	ret = nvt_process_all_in_one(addr, size, 0);
	if (ret < 0) {
		printf("Update nvt all-in-one image failed.\n");
		return ret;
	}

	return 0;
}

U_BOOT_CMD(
	nvt_update_all, 3,  0,  do_nvt_update_all_cmd,
	"To Update all-in-one image from memory address and size \n",
	"[address(hex)][size(dec)] \n"
);
static UINT8  car_in[16] = {0xb1, 0xbe, 0x28, 0x1a, 0x3e, 0xe0, 0xd3, 0x2e, 0xf8, 0xb1, 0xe0, 0x0b, 0x4a, 0x9e, 0x38, 0x13};
//static __packed __aligned(64) UINT8  output[16];
//static __packed __aligned(64) UINT8  output2[16];
//volatile UINT32 test = 1;
int do_nvt_encrypt_cmd(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
	int index_cnt;

	UINT32 test_buf;
	UINT32 test_buf2;

	UINT32 test_buf_align;
	UINT32 test_buf2_align;

	UINT8  *output;
	UINT8  *output2;

	UINT32      key_set = EFUSE_OTP_2ND_KEY_SET_FIELD;
	CRYPT_OP    crypt_op_param;

	test_buf = (UINT32)malloc(16 + 64);
	test_buf_align = (UINT32)((test_buf + 63) & 0xFFFFFFC0);

	test_buf2 = (UINT32)malloc(16 + 64);
	test_buf2_align = (UINT32)((test_buf + 63) & 0xFFFFFFC0);

	output = (UINT8 *)test_buf_align;
	output2 = (UINT8 *)test_buf2_align;

	crypt_op_param.op_mode = CRYPTO_EBC;
	crypt_op_param.en_de_crypt = CRYPTO_ENCRYPT;
	crypt_op_param.src_addr = (UINT32)car_in;
	crypt_op_param.dst_addr = (UINT32)output;
	crypt_op_param.length   = 16;

	if (crypto_data_operation(key_set, crypt_op_param) != 0) {
		printf("Encrypt operation fail [%d] set\r\n", (int)(key_set + 1));
	} else {
		printf("Encrypt operation success [%d] set\r\n", (int)(key_set + 1));
		printf("Source =>\r\n");
		for (index_cnt = 0; index_cnt < 16; index_cnt++) {
			printf("%2x ", car_in[index_cnt]);
		}
		printf("\r\n");
		printf("Destination =>\r\n");
		for (index_cnt = 0; index_cnt < 16; index_cnt++) {
			printf("%2x ", output[index_cnt]);
		}
		printf("\r\n");
	}

	crypt_op_param.en_de_crypt = CRYPTO_DECRYPT;
	crypt_op_param.src_addr = (UINT32)output;
	crypt_op_param.dst_addr = (UINT32)output2;
	crypt_op_param.length   = 16;

	if (crypto_data_operation(key_set, crypt_op_param) != 0) {
		printf("Decrypt operation fail [%d] set\r\n", (int)(key_set + 1));
	} else {
		printf("Decrypt operation success [%d] set\r\n", (int)(key_set + 1));
		for (index_cnt = 0; index_cnt < 16; index_cnt++) {
			printf("%2x ", output2[index_cnt]);
		}
		printf("\r\n");
	}


	printf("do_nvt_encrypt_cmd sample code done\r\n");

	if (test_buf) {
		free((char *)test_buf);
	}

	if (test_buf2) {
		free((char *)test_buf2);
	}

	return 0;
}
U_BOOT_CMD(
	nvt_encrypt,    1,  0,  do_nvt_encrypt_cmd,
	"To encrypt via key manager sample",
	"[keyset(0~3)]\n"
);

static UINT8 key_sample[16] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16};
static UINT8 encrypt_sample[16] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16};

//static UINT8 key_1st_sample[16] = {0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x01};

int do_nvt_write_key_cmd(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
	int         index_cnt;
	UINT32      test_buf;
	UINT32      test_buf_align;

	UINT32      test_buf2;
	UINT32      test_buf2_align;

	UINT8       *output;
	UINT8       *output2;
	UINT32      key_set = EFUSE_OTP_2ND_KEY_SET_FIELD;
	CRYPT_OP    crypt_op_param;

	if (strncmp(argv[1], "0", 1) == 0) {
		key_set = EFUSE_OTP_1ST_KEY_SET_FIELD;
	} else if (strncmp(argv[1], "1", 1) == 0) {
		key_set = EFUSE_OTP_2ND_KEY_SET_FIELD;
	} else if (strncmp(argv[1], "2", 1) == 0) {
		key_set = EFUSE_OTP_3RD_KEY_SET_FIELD;
	} else if (strncmp(argv[1], "3", 1) == 0) {
		key_set = EFUSE_OTP_4TH_KEY_SET_FIELD;
	} else {
		return CMD_RET_USAGE;
	}

	if (efuse_write_key(key_set, key_sample) < 0) {
		printf("Write [%d] key operation fail\r\n", (int)(key_set + 1));
	} else {
		printf("Write [%d] key operation success\r\n", (int)(key_set + 1));
	}


	test_buf = (UINT32)malloc(16 + 64);
	test_buf_align = (UINT32)((test_buf + 63) & 0xFFFFFFC0);
	output = (UINT8 *)test_buf_align;

	test_buf2 = (UINT32)malloc(16 + 64);
	test_buf2_align = (UINT32)((test_buf2 + 63) & 0xFFFFFFC0);
	output2 = (UINT8 *)test_buf2_align;

	memset((char *)test_buf2_align, 0x0, 16);
	memset((char *)test_buf_align, 0x0, 16);

	crypt_op_param.op_mode = CRYPTO_EBC;
	crypt_op_param.en_de_crypt = CRYPTO_ENCRYPT;
	crypt_op_param.src_addr = (UINT32)encrypt_sample;
	crypt_op_param.dst_addr = (UINT32)output;   //<<<-------------(1)
	crypt_op_param.length   = 16;

	//Encrypt via key manager by using specific key set
	if (crypto_data_operation(key_set, crypt_op_param) != 0) {
		printf("Encrypt operation fail [%d] set\r\n", (int)(key_set + 1));
	} else {
		printf("Encrypt operation success [%d] set\r\n", (int)(key_set + 1));
		printf("Source =>\r\n");
		for (index_cnt = 0; index_cnt < 16; index_cnt++) {
			printf("%2x ", key_sample[index_cnt]);
		}
		printf("\r\n");
		printf("Destination =>\r\n");
		for (index_cnt = 0; index_cnt < 16; index_cnt++) {
			printf("%2x ", output[index_cnt]);
		}
		printf("\r\n");
	}

	//Encrypt via fill key by using CPU
	crypt_op_param.dst_addr = (UINT32)output2;  ////<<<-------------(2)
	crypto_data_operation_by_key(key_sample, crypt_op_param);

	printf("Verification via CPU fill key=>\r\n");
	for (index_cnt = 0; index_cnt < 16; index_cnt++) {
		printf("%2x ", output2[index_cnt]);
	}
	printf("\r\n");


	//(1) vs (2) result should the same
	if (memcmp((void *)output2, (void *)output, 16) != 0) {
		printf("write key fail [%d] set\r\n", (int)(key_set + 1));
		printf("OTP operation\r\n");
		for (index_cnt = 0; index_cnt < 16; index_cnt++) {
			printf("%2x ", output[index_cnt]);
		}

		printf("\r\n");
		printf("Fill key operation\r\n");
		for (index_cnt = 0; index_cnt < 16; index_cnt++) {
			printf("%2x ", output2[index_cnt]);
		}
	} else {
		printf("write key success [%d] set\r\n", (int)(key_set + 1));
	}
	return 0;
}
U_BOOT_CMD(
	nvt_write_key,  6,  0,  do_nvt_write_key_cmd,
	"To write key via OTP sample",
	"[keyset(0~3)] => 0 is dedicate for secure boot\n"
);

int do_nvt_secure_en_cmd(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
	if (strncmp(argv[1], "enable", 6) == 0) {
		efuse_secure_en();
	} else if (strncmp(argv[1], "get", 3) == 0) {
		if (efuse_is_secure_en()) {
			printf("secure mode 	-> enabled\r\n");
		} else {
			printf("secure mode 	-> disable\r\n");
		}
	} else {
		return CMD_RET_USAGE;
	}
	return 0;
}


U_BOOT_CMD(
	nvt_secure_en,  6,  0,  do_nvt_secure_en_cmd,
	"secure enable (get if enable)\n",
	"[enable] for enable secure mode\n[get] for get if secure mode enable\nAfter secure enable, plantext loader MUST change to cypher loader\n"
);

int do_nvt_key_get_flag_en_cmd(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
 	UINT32 	_key;
 	UINT32 	key_set = EFUSE_OTP_2ND_KEY_SET_FIELD;
	if (strncmp(argv[1], "0", 1) == 0) {
		key_set = EFUSE_OTP_1ST_KEY_SET_FIELD;
	} else if (strncmp(argv[1], "1", 1) == 0) {
		key_set = EFUSE_OTP_2ND_KEY_SET_FIELD;
	} else if (strncmp(argv[1], "2", 1) == 0) {
		key_set = EFUSE_OTP_3RD_KEY_SET_FIELD;
	} else if (strncmp(argv[1], "3", 1) == 0) {
		key_set = EFUSE_OTP_4TH_KEY_SET_FIELD;
	} else if (strncmp(argv[1], "all", 3) == 0) {
		for (_key = EFUSE_OTP_1ST_KEY_SET_FIELD; _key < EFUSE_OTP_TOTAL_KEY_SET_FIELD; _key++) {
			if (efuse_key_get_flag(_key) == 1) {
				printf("key [%d] set flag [O]\r\n", (int)(_key));
			} else {
				printf("key [%d] set flag [X]\r\n", (int)(_key));
			}
		}
		return 0;
	} else {
		return CMD_RET_USAGE;
	}

	if (efuse_key_get_flag(key_set) == 1) {
		printf("key [%d] set flag [O]\r\n", (int)(key_set));
	} else {
		printf("key [%d] set flag [X]\r\n", (int)(key_set));
	}
	return 0;
}


U_BOOT_CMD(
	nvt_key_get_flag_en,  6,  0,  do_nvt_key_get_flag_en_cmd,
	"key get flag enable",
	"Represent specific key set already, [all] for all set\n"
);

int do_nvt_jtag_dis_cmd(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
	efuse_jtag_dis();
	return 0;
}


U_BOOT_CMD(
	nvt_jtag_disable,  1,  0,  do_nvt_jtag_dis_cmd,
	"jtag disable",
	"After jtag disable, JTAG will disable forever\n"
);

