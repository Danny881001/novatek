/**
    NVT common api

    @file       nvt_common.c
    @ingroup
    @note
    Copyright   Novatek Microelectronics Corp. 2016.  All rights reserved.

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 2 as
    published by the Free Software Foundation.
*/
#include <common.h>
#include <malloc.h>
#include <fs.h>
#include <jffs2/jffs2.h>
#include <mmc.h>
#include <nand.h>
#include <fdt_support.h>
#include <libfdt.h>
#include <linux/ctype.h>
#include <asm/arch/nvt_types.h>
#include <asm/arch/modelext/bin_info.h>
#include <asm/arch/modelext/emb_partition_info.h>
#include <asm/arch/shm_info.h>
#include <asm/arch/nvt_common.h>
#include <asm/arch/na51000evb.h>
#include "nvtpack.h"

unsigned long nvt_tm0_cnt_beg = 0;
unsigned long nvt_tm0_cnt_end = 0;
unsigned long nvt_shminfo_comm_uboot_boot_func = 0;
unsigned long nvt_shminfo_comm_core1_start = 0;
unsigned long nvt_shminfo_comm_core2_start = 0;
unsigned long nvt_shminfo_comm_itron_comp_addr = 0;
unsigned long nvt_shminfo_comm_itron_comp_len = 0;
unsigned long nvt_shminfo_comm_fw_update_addr = 0;
unsigned long nvt_shminfo_comm_fw_update_len = 0;
unsigned int nvt_shminfo_boot_fdt_addr = 0;
unsigned long nvt_ramdisk_addr = 0;
unsigned long nvt_ramdisk_size = 0;
uint8_t *nvt_fdt_buffer = NULL;
char *nvt_bin_name = NULL;
char *nvt_bin_name_t = NULL;
char *nvt_bin_name_r = NULL;
char *nvt_bin_name_fdt = NULL;

EMB_PARTITION_FDT_TRANSLATE_TABLE emb_part_fdt_map[EMBTYPE_TOTAL_SIZE] = {
	{"partition_mbr", EMBTYPE_MBR},
	{"partition_loader", EMBTYPE_LOADER},
	{"partition_fdt", EMBTYPE_FDT},
	{"partition_uboot", EMBTYPE_UBOOT},
	{"partition_uenv", EMBTYPE_UENV},
	{"partition_kernel", EMBTYPE_LINUX},
	{"partition_rootfs", EMBTYPE_ROOTFS},
	{"partition_rootfsl", EMBTYPE_ROOTFSL}
};

EMB_PARTITION emb_partition_info_data_curr[EMB_PARTITION_INFO_COUNT];
EMB_PARTITION emb_partition_info_data_new[EMB_PARTITION_INFO_COUNT];

void nvt_shminfo_init(void)
{
	SHMINFO *p_shminfo;
	//MODELEXT_HEADER *header;

	//p_bininfo = (BININFO *)modelext_get_cfg((unsigned char*)CONFIG_SMEM_SDRAM_BASE, MODELEXT_TYPE_BIN_INFO, &header);
	p_shminfo = (SHMINFO *)CONFIG_SMEM_SDRAM_BASE;

	if (strncmp(p_shminfo->boot.LdInfo_1, "LD_NVT", 6) != 0) {
		printf("%sAttention!!!! Please update to latest version loader%s", ANSI_COLOR_RED, ANSI_COLOR_RESET);
		while(1);
	}

	nvt_shminfo_comm_uboot_boot_func = (unsigned long)&p_shminfo->comm.Resv[0];
	nvt_shminfo_comm_core1_start = (unsigned long)&p_shminfo->comm.Resv[1];
	nvt_shminfo_comm_core2_start = (unsigned long)&p_shminfo->comm.Resv[2];
	nvt_shminfo_comm_itron_comp_addr = (unsigned long)&p_shminfo->comm.Resv[3];
	nvt_shminfo_comm_itron_comp_len = (unsigned long)&p_shminfo->comm.Resv[4];
	nvt_shminfo_comm_fw_update_addr = (unsigned long)&p_shminfo->comm.Resv[5];
	nvt_shminfo_comm_fw_update_len = (unsigned long)&p_shminfo->comm.Resv[6];
	nvt_shminfo_boot_fdt_addr = (unsigned int)&p_shminfo->boot.fdt_addr;
	nvt_fdt_buffer = malloc(FDT_SIZE);
}

int nvt_fdt_init(bool reload)
{
	int ret;
	ulong fdt_addr = nvt_readl((ulong)nvt_shminfo_boot_fdt_addr);

	if (reload) {
		memset((unsigned char *)fdt_addr, 0, NVT_FDT_MEM_SIZE);
	#ifdef CONFIG_NVT_LINUX_AUTOLOAD
		char command[128];
		#if defined(CONFIG_NVT_LINUX_SPINAND_BOOT)
		sprintf(command, "nand read %lx %x %x", fdt_addr, CONFIG_MODELEXT_FLASH_BASE, CONFIG_SMEM_SDRAM_SIZE);
		ret = run_command(command, 0);
		#elif defined(CONFIG_NVT_LINUX_SPINOR_BOOT)
		sprintf(command, "sf read 0x%lx 0x%x 0x%x", fdt_addr, CONFIG_MODELEXT_FLASH_BASE, CONFIG_SMEM_SDRAM_SIZE);
		ret = run_command(command, 0);
		#elif defined(CONFIG_NVT_LINUX_EMMC_BOOT)
		/* MMC read should use block number unit */
		sprintf(command, "mmc read 0x%lx 0x%x 0x%x", fdt_addr, CONFIG_MODELEXT_FLASH_BASE, CONFIG_SMEM_SDRAM_SIZE/MMC_MAX_BLOCK_LEN);
		ret = run_command(command, 0);
		#elif defined(CONFIG_NVT_LINUX_SD_BOOT)
		sprintf(command, "fatload mmc 0:1 0x%lx %s", fdt_addr, get_nvt_bin_name(NVT_BIN_NAME_TYPE_MODELEXT));
		ret = run_command(command, 0);
		#else
		/* All-in-one SD boot */
		#endif /* CONFIG_NVT_LINUX_SPINAND_BOOT */
	#else /* !CONFIG_NVT_LINUX_AUTOLOAD */
		/* FIXME: To do customized boot */

	#endif /* CONFIG_NVT_LINUX_AUTOLOAD */
		if (ret) {
			printf("fdt init fail return %d\n", ret);
			return ret;
		}
	}

	/* Copy fdt data from loader fdt area to uboot fdt area */
	memcpy(nvt_fdt_buffer, (void*)fdt_addr, FDT_SIZE);

	return 0;
}

/*
 * Return 0
 *		fdt
 * else
 *		none
 */
int nvt_check_isfdt(ulong addr)
{
	int  nodeoffset, ret;

	ret = fdt_check_header((void *)addr);
	if (ret < 0) {
		debug("fdt address %lx is not valid with error code: %d\n", addr, ret);
		return -1;
	}

	#if defined(CONFIG_NVT_LINUX_EMMC_BOOT)
	nodeoffset = fdt_path_offset((void*)addr, "/mmc@f0510000");
	#elif defined(CONFIG_NVT_LINUX_SPINOR_BOOT)
	nodeoffset = fdt_path_offset((void*)addr, "/nor");
	#else
	nodeoffset = fdt_path_offset((void*)addr, "/nand");
	#endif
	if (nodeoffset < 0)
		return -1;
	else
		return 0;
}

int nvt_chk_mtd_fdt_is_null(void)
{
	int ret;
	char command[128] = {0};

	void *tmp_model_ptr = memalign(ARCH_DMA_MINALIGN, CONFIG_SMEM_SDRAM_SIZE);
	#if defined(CONFIG_NVT_LINUX_SPINAND_BOOT)
	sprintf(command, "nand read %x %x %x", (unsigned int)tmp_model_ptr, CONFIG_MODELEXT_FLASH_BASE, CONFIG_SMEM_SDRAM_SIZE);
	#elif defined(CONFIG_NVT_LINUX_EMMC_BOOT)
	sprintf(command, "mmc read 0x%x 0x%x 0x%x", (unsigned int)tmp_model_ptr, CONFIG_MODELEXT_FLASH_BASE, CONFIG_SMEM_SDRAM_SIZE/MMC_MAX_BLOCK_LEN);
	#else
	sprintf(command, "sf read 0x%x 0x%x 0x%x", (unsigned int)tmp_model_ptr, CONFIG_MODELEXT_FLASH_BASE, CONFIG_SMEM_SDRAM_SIZE);
	#endif
	ret = run_command(command, 0);
	if (ret < 0)
		return ret;

	ret = nvt_check_isfdt((unsigned int)tmp_model_ptr);
	free(tmp_model_ptr);
	if(ret != 0) {
		return 1;
	} else {
		return 0;
	}
}

int nvt_board_init(void)
{
	ethernet_init();

	sdio_power_cycle();
	return 0;
}

int nvt_fw_load_tbin(void)
{
	loff_t size = 0;
	int ret = 0;

	if (nvt_fs_set_blk_dev()) 
		return ERR_NVT_UPDATE_OPENFAILED;
	else {
		if (!fs_exists(get_nvt_bin_name(NVT_BIN_NAME_TYPE_RUNFW))) {
			return ERR_NVT_UPDATE_NO_NEED;
		}
	}

	if (nvt_fs_set_blk_dev())
		return ERR_NVT_UPDATE_OPENFAILED;
	else {
		ret = fs_read(get_nvt_bin_name(NVT_BIN_NAME_TYPE_RUNFW), (ulong)CONFIG_NVT_RUNFW_SDRAM_BASE, 0, 0, &size);
		if (size <= 0 || ret < 0) {
			printf("Read %s at 0x%x failed ret=%lld\n", get_nvt_bin_name(NVT_BIN_NAME_TYPE_RUNFW), CONFIG_NVT_RUNFW_SDRAM_BASE, size);
			return ERR_NVT_UPDATE_READ_FAILED;
		} else {
			printf("Read %s at 0x%x successfully, size=%lld\n", get_nvt_bin_name(NVT_BIN_NAME_TYPE_RUNFW), CONFIG_NVT_RUNFW_SDRAM_BASE, size);
		}
	}

	return 0;
}

int nvt_detect_fw_tbin(void)
{
	loff_t size = 0;
	int ret = 0;

	if (nvt_fs_set_blk_dev()) 
		return ERR_NVT_UPDATE_OPENFAILED;
	else {
		if (!fs_exists(get_nvt_bin_name(NVT_BIN_NAME_TYPE_RUNFW))) {
			return ERR_NVT_UPDATE_NO_NEED;
		}
	}

	return 0;
}


/*************************************************************************
* _LZ_ReadVarSize() - Read unsigned integer with variable number of
* bytes depending on value.
*************************************************************************/

static unsigned int _lz_read_var_size(unsigned int *x, unsigned char *buf)
{
	unsigned int y, b, num_bytes;

	/* Read complete value (stop when byte contains zero in 8:th bit) */
	y = 0;
	num_bytes = 0;
	do
	{
		b = (unsigned int) (*buf ++);
		y = (y << 7) | (b & 0x0000007f);
		++ num_bytes;
	}
	while( b & 0x00000080 );

	/* Store value in x */
	*x = y;

	/* Return number of bytes read */
	return num_bytes;
}

/**
	LZ Uncompress

	Uncompress/decompress by LZ77 decoder

	@param[in] in       Input buffer of compressed data
	@param[out] out     Output buffer to store decompressed data (Caller should maintain large enough buffer size to hold decompressed data)
	@param[in] insize   Length of input buffer in

	@return void
*/
void lz_uncompress(unsigned char *in, unsigned char *out, unsigned int insize)
{
	unsigned char marker, symbol;
	unsigned int  i, inpos, outpos, length, offset;

	/* Do we have anything to uncompress? */
	if( insize < 1 )
	{
		return;
	}

	/* Get marker symbol from input stream */
	marker = in[ 0 ];
	inpos = 1;

	/* Main decompression loop */
	outpos = 0;
	do
	{
		symbol = in[ inpos ++ ];
		if( symbol == marker )
		{
			/* We had a marker byte */
			if( in[ inpos ] == 0 )
			{
				/* It was a single occurrence of the marker byte */
				out[ outpos ++ ] = marker;
				++ inpos;
			}
			else
			{
				/* Extract true length and offset */
				inpos += _lz_read_var_size( &length, &in[ inpos ] );
				inpos += _lz_read_var_size( &offset, &in[ inpos ] );

				/* Copy corresponding data from history window */
				for( i = 0; i < length; ++ i )
				{
					out[ outpos ] = out[ outpos - offset ];
					++ outpos;
				}
			}
		}
		else
		{
			/* No marker, plain copy */
			out[ outpos ++ ] = symbol;
		}
	}
	while( inpos < insize );
}

int get_part(const char *partname, loff_t *off, loff_t *maxsize)
{
#if defined(CONFIG_NVT_LINUX_EMMC_BOOT)
	char cmdline[1024] = {0}, nvtemmcpart[32] = {0};
	char *nvtemmcpart_off = NULL, *nvtemmcpart_off_next = NULL, *nvtemmcpart_off_end = NULL, *tmp = NULL;
	u32 nvtemmcpart_sz = 0;

	sprintf(cmdline,"%s ",getenv("bootargs"));
	nvtemmcpart_off = strstr((char *)cmdline, "nvtemmcpart=") + strlen("nvtemmcpart=") - 1;
	nvtemmcpart_off_end = strstr((char *)nvtemmcpart_off, " ");
	nvtemmcpart_off_next = strstr((char *)nvtemmcpart_off, ",");
	*maxsize = 0;
	*off = 0;

	if (nvtemmcpart_off == NULL || ((u32)nvtemmcpart_off_end - (u32)nvtemmcpart_off) < 20)
		return -1;
	do
	{
		memset(nvtemmcpart, 0, sizeof(nvtemmcpart));
		nvtemmcpart_sz = (u32)(nvtemmcpart_off_next - nvtemmcpart_off - 1);
		strncpy(nvtemmcpart, nvtemmcpart_off+1, nvtemmcpart_sz);
		nvtemmcpart_off = nvtemmcpart_off_next;
		nvtemmcpart_off_next = strstr((char *)nvtemmcpart_off_next+1, ",");

		if (strstr(nvtemmcpart, partname) != NULL) {
			*maxsize = simple_strtoul(nvtemmcpart, &tmp, 0);
			*off = simple_strtoul((tmp + 1), NULL, 0);
			break;
		}
	} while((u32)nvtemmcpart_off_next < (u32)nvtemmcpart_off_end && (u32)nvtemmcpart_off_next != 0);

	return 0;
#elif defined(CONFIG_CMD_MTDPARTS)
	struct mtd_device *dev;
	struct part_info *part;
	u8 pnum;
	int ret;

	ret = mtdparts_init();
	if (ret)
		return ret;

	ret = find_dev_and_part(partname, &dev, &pnum, &part);
	if (ret)
		return ret;

	*off = part->offset;
	*maxsize = part->size;

	return 0;
#else
	puts("offset is not a number\n");
	return -1;
#endif
}

unsigned long get_nvt_timer0_cnt(void)
{
	return nvt_readl((ulong)NVT_TIMER0_CNT);
}

/*
 * get from bin info.
 * select:
 *		0: FW_NAME
 *		1: RUNFW_NAME
 *		2: MODELEXT_NAME
 */
char *get_nvt_bin_name(NVT_BIN_NAME_TYPE type)
{
	int  nodeoffset, len = 0;
	const void *nodep;      /* property node pointer */

	nodeoffset = fdt_path_offset((const void*)nvt_fdt_buffer, "/nvt_info");
	nodep = fdt_getprop((const void *)nvt_fdt_buffer, nodeoffset, "BIN_NAME", &len);

	if (nvt_bin_name == NULL) {
		nvt_bin_name = calloc(sizeof(char),15);
		if (!nvt_bin_name) {
			printf( "%s: allocation failure \n", __FUNCTION__);
			return NULL;
		}
		strcpy(nvt_bin_name, nodep);
		strcat(nvt_bin_name, ".bin");
	}

	if ( nvt_bin_name_t == NULL) {
		nvt_bin_name_t = calloc(sizeof(char),15);
		if (!nvt_bin_name_t) {
			printf( "%s: allocation failure \n", __FUNCTION__);
			free(nvt_bin_name);
			return NULL;
		}
		strcpy(nvt_bin_name_t, nodep);
		nvt_bin_name_t[strlen(nvt_bin_name_t) - 1] = '\0';
		strcat(nvt_bin_name_t, "T.bin");
	}

	if ( nvt_bin_name_r == NULL) {
		nvt_bin_name_r = calloc(sizeof(char),15);
		if (!nvt_bin_name_r) {
			printf( "%s: allocation failure \n", __FUNCTION__);
			free(nvt_bin_name);
			free(nvt_bin_name_t);
			return NULL;
		}
		strcpy(nvt_bin_name_r, nodep);
		nvt_bin_name_r[strlen(nvt_bin_name_r) - 1] = '\0';
		strcat(nvt_bin_name_r, "R.bin");
	}

	if ( nvt_bin_name_fdt == NULL) {
		nvt_bin_name_fdt = calloc(sizeof(char),15);
		if (!nvt_bin_name_fdt) {
			printf( "%s: allocation failure \n", __FUNCTION__);
			free(nvt_bin_name);
			free(nvt_bin_name_t);
			free(nvt_bin_name_r);
			return NULL;
		}
		strcpy(nvt_bin_name_fdt, nodep);
		strcat(nvt_bin_name_fdt, ".fdt.bin");
	}

	if (type == NVT_BIN_NAME_TYPE_FW) {
		return nvt_bin_name;
	} else if (type == NVT_BIN_NAME_TYPE_RUNFW) {
		return nvt_bin_name_t;
	} else if (type == NVT_BIN_NAME_TYPE_RECOVERY_FW) {
		return nvt_bin_name_r;
	} else {
		return nvt_bin_name_fdt;
	}
}

int nvt_check_is_fw_update_fw(void)
{
	unsigned long      boot_reason;

	boot_reason = nvt_readl((ulong)nvt_shminfo_comm_uboot_boot_func) & COMM_UBOOT_BOOT_FUNC_BOOT_REASON_MASK;

	return (boot_reason == COMM_UBOOT_BOOT_FUNC_BOOT_UPDFIRM);
}

int nvt_check_is_fomat_rootfs(void)
{
	unsigned long      boot_reason;
	static int         is_format = -1;

	if(is_format != -1)
		return is_format;

	boot_reason = nvt_readl((ulong)nvt_shminfo_comm_uboot_boot_func) & COMM_UBOOT_BOOT_FUNC_BOOT_REASON_MASK;
	is_format = ((boot_reason == COMM_UBOOT_BOOT_FUNC_BOOT_FORMAT_ROOTFS) ? 1 : 0);

	return is_format;
}

int nvt_fs_set_blk_dev(void)
{
	#if 0
	if (fs_set_blk_dev("mmc", "0:1", FS_TYPE_FAT))
		if (fs_set_blk_dev("mmc", "0:1", FS_TYPE_EXFAT))
			if (fs_set_blk_dev("mmc", "0:0", FS_TYPE_FAT))
				if (fs_set_blk_dev("mmc", "0:0", FS_TYPE_EXFAT)){
			                printf("MMC interface configure failed\n");
			                return -1;
				}
	#else
	if (fs_set_blk_dev("mmc", "0:1", FS_TYPE_FAT))
			if (fs_set_blk_dev("mmc", "0:0", FS_TYPE_FAT)) {
			                printf("MMC interface configure failed\n");
			                return -1;
				}
	#endif

	return 0;
}

int nvt_flash_mark_bad(void)
{
	#if defined(CONFIG_NVT_LINUX_SPINAND_BOOT)
	int i = 0, ret = 0;
	struct mtd_info *nand = nand_info[nand_curr_device];

	for (i = 0; i < 100; i++) {
		if (nvt_flash_ecc_err_pages[i] != 0) {
			ret = mtd_block_markbad(nand, (loff_t)nvt_flash_ecc_err_pages[i]);
			if (ret) {
				printf("cannot mark nand flash block %d as bad, error %d", nvt_flash_ecc_err_pages[i], ret);
				return ret;
			}
		}
	}
	#endif
	return 0;
}

int nvt_enum_fdt_props(ulong fdt_addr, char *part_node_name, void *pdata,
					int (*enum_funcs)(ulong fdt_addr, const char *node_name, const char *prop_name, char *prop_val, void *pdata))
{
	int nodeoffset = 0, nextsubnodeoffset = 0, nextnodeoffset = 0, ret = 0, len = 0;
	const char *list_node_name = NULL;
	char path[100];
	nodeoffset = fdt_path_offset((const void*)fdt_addr, part_node_name);
	if (nodeoffset < 0)
		return -1;

	/* To check if the next subnode is existed */
	nextsubnodeoffset = fdt_first_subnode((const void*)fdt_addr, nodeoffset);
	if (nextsubnodeoffset < 0)
		goto loop_prop_parse;

	for (nextnodeoffset = nextsubnodeoffset;
		(nextnodeoffset > 0);
		(nextnodeoffset = fdt_next_node((const void*)fdt_addr, nextnodeoffset, &len))) {

		nodeoffset = nextnodeoffset;
		list_node_name = fdt_get_name((const void*)fdt_addr, nextnodeoffset, NULL);
		if (list_node_name == NULL)
			return 0;

		/* get node path to decide if this loop is end */
		fdt_get_path((const void*)fdt_addr, nodeoffset, path, sizeof(path));
		if (strncmp(part_node_name, path, strlen(part_node_name)) != 0)
			return 0;
loop_prop_parse:
		for (nodeoffset = fdt_first_property_offset((const void*)fdt_addr, nodeoffset);
			(nodeoffset >= 0);
			(nodeoffset = fdt_next_property_offset((const void*)fdt_addr, nodeoffset))) {
			const struct fdt_property *prop;

			if (!(prop = fdt_get_property_by_offset((const void*)fdt_addr, nodeoffset, &len))) {
				break;
			}

			const char *p_id_name = fdt_string((const void*)fdt_addr, fdt32_to_cpu(prop->nameoff));
			debug("%s: node_name=%s prop_name=%s prop_data=%s\n", __func__, list_node_name, p_id_name, (char*)&prop->data);
			ret = (*enum_funcs)((ulong)fdt_addr, list_node_name, p_id_name, (char*)&prop->data, pdata);
			if (ret < 0)
				return -1;
		}
	}

	return 0;
}

int nvt_fdt_embpart_lookup(char *part_node_name, unsigned short *embtype)
{
	int i = 0, ret = -1;
	size_t len = 0;

	len = strlen(part_node_name);
	while (!isdigit(part_node_name[i]) && i < len) {
		i++;
	}

	len = (size_t)i;
	*embtype = 0;
	for (i = 0; i < EMBTYPE_TOTAL_SIZE; i++) {
		if (strncmp(emb_part_fdt_map[i].fdt_node_name, part_node_name, len) == 0) {
			*embtype = emb_part_fdt_map[i].emb_type;
			ret = len;
			break;
		}
	}

	return ret;
}

static int nvt_getfdt_emb_part_info(ulong fdt_addr, char *part_node_name, EMB_PARTITION *pemb, unsigned int part_id)
{
	int  nodeoffset, nextoffset, ret;
	const char *ptr = NULL;
	char *endptr = NULL;
	u64 addr, size;
	const unsigned long long *val = NULL;
	unsigned int idx = 0;

	#if defined(CONFIG_NVT_LINUX_EMMC_BOOT)
	nodeoffset = fdt_path_offset((void*)fdt_addr, "/mmc@f0510000");
	#elif defined(CONFIG_NVT_LINUX_SPINOR_BOOT)
	nodeoffset = fdt_path_offset((void*)fdt_addr, "/nor");
	#else
	nodeoffset = fdt_path_offset((void*)fdt_addr, "/nand");
	#endif
	/* Got partition_loader node */
	nextoffset = fdt_first_subnode((void*)fdt_addr, nodeoffset);
	if (nextoffset < 0) {
		return -1;
	}

	nodeoffset = nextoffset;
	ptr = fdt_get_name((const void*)fdt_addr, nextoffset, NULL);
	while(strcmp(ptr, part_node_name) != 0) {
		nextoffset = fdt_next_node((void*)fdt_addr, nodeoffset, NULL);
		if (nextoffset < 0)
			return -1;
		ptr = fdt_get_name((const void*)fdt_addr, nextoffset, NULL);
		nodeoffset = nextoffset;
	}

	val = (const unsigned long long *)fdt_getprop((const void*)fdt_addr, nodeoffset, "reg", NULL);
	addr = be64_to_cpu(val[0]);
	size = be64_to_cpu(val[1]);

	ret = nvt_fdt_embpart_lookup(part_node_name, &pemb[part_id].EmbType);
	if (ret < 0) {
		printf("%s Skip this partition node: %s %s\n", ANSI_COLOR_RED, part_node_name, ANSI_COLOR_RESET);
		return 0;
	} else {
		if (!isdigit(part_node_name[(u32)ret])) {
			idx = 0;
		} else {
			idx = simple_strtoul(&part_node_name[(u32)ret], &endptr, 10);
			if (*endptr != '\0') {
				printf("%s: get partition table index error with %s\n", __func__, part_node_name);
				return -1;
			}
		}
		pemb[part_id].OrderIdx = idx;
		pemb[part_id].PartitionOffset = addr;
		pemb[part_id].PartitionSize = size;
	}

	debug("EmbType=%u PartitionOffset=%llx PartitionSize=%llx Node_name=%s idx=%u\n",
											pemb[part_id].EmbType,
											pemb[part_id].PartitionOffset,
											pemb[part_id].PartitionSize,
											part_node_name,
											idx);
	return 0;
}

static int nvt_fdt_enum_fill_emb_parts(ulong fdt_addr, const char *node_name, const char *prop, char *prop_val, void *pdata)
{
	int ret = 0;
	char part_name[100], *endptr = NULL;
	unsigned int part_id = 0;
	char prop_partname[] = "partition_name";
	EMB_PARTITION *pemb = (EMB_PARTITION *)pdata;

	/* Only check partition_name prop */
	if (strncmp(prop, prop_partname, strlen(prop_partname)) != 0)
		return 0;

	ret = sprintf(part_name, "partition_%s", prop_val);
	if (ret < 0)
		return -1;

	if (strncmp(node_name, "id", 2) != 0) {
		printf("nvtpack-index naming error: %s\n", prop);
		return -1;
	}

	part_id = simple_strtoul(&node_name[2], &endptr, 10);
	if (*endptr != '\0') {
		printf("nvtpack-index string transfer error: %s\n", prop);
		return -1;
	}

	#if defined(CONFIG_NVT_LINUX_EMMC_BOOT)
	debug("%s Got from /emmc/nvtpack/index node name: %s prop name: %s, prop value: %s %s \n", ANSI_COLOR_YELLOW, node_name, part_name, prop_val, ANSI_COLOR_RESET);
	#elif defined(CONFIG_NVT_LINUX_SPINOR_BOOT)
	debug("%s Got from /emmc/nvtpack/index node name: %s prop name: %s, prop value: %s %s \n", ANSI_COLOR_YELLOW, node_name, part_name, prop_val, ANSI_COLOR_RESET);
	#else
	debug("%s Got from /nor/nvtpack/index node name: %s prop name: %s, prop value: %s %s \n", ANSI_COLOR_YELLOW, node_name, part_name, prop_val, ANSI_COLOR_RESET);
	#endif
	ret = nvt_getfdt_emb_part_info(fdt_addr, part_name, pemb, part_id);
	if (ret < 0) {
		printf("%s get emb part info from nvtpack node is failed: %s %u %s\n", ANSI_COLOR_RED, part_name, part_id, ANSI_COLOR_RESET);
		return -1;
	}

	return 0;
}

int nvt_getfdt_emb(ulong fdt_addr, EMB_PARTITION *pemb)
{
	int ret = 0;

	#if defined(CONFIG_NVT_LINUX_EMMC_BOOT)
	ret = nvt_enum_fdt_props(fdt_addr, "/mmc@f0510000/nvtpack/index", pemb, nvt_fdt_enum_fill_emb_parts);
	printf("%d\n", ret);
	#elif defined(CONFIG_NVT_LINUX_SPINOR_BOOT)
	ret = nvt_enum_fdt_props(fdt_addr, "/nor@f0400000/nvtpack/index", pemb, nvt_fdt_enum_fill_emb_parts);
	#else
	ret = nvt_enum_fdt_props(fdt_addr, "/nand@f0400000/nvtpack/index", pemb, nvt_fdt_enum_fill_emb_parts);
	#endif
	if (ret < 0)
		return -1;

	return 0;
}

int nvt_getfdt_rootfs_mtd_num(ulong fdt_addr, unsigned int *mtd_num, uint32_t *ro_attr)
{
	int  nodeoffset, nextoffset;
	unsigned int i = 0;
	const char *ptr = NULL;
	int len = 0;
	const void * prop_val = NULL;

	#if defined(CONFIG_NVT_LINUX_EMMC_BOOT)
	nodeoffset = fdt_path_offset((void*)fdt_addr, "/mmc@f0510000");
	#elif defined(CONFIG_NVT_LINUX_SPINOR_BOOT)
	nodeoffset = fdt_path_offset((void*)fdt_addr, "/nor");
	#else
	nodeoffset = fdt_path_offset((void*)fdt_addr, "/nand");
	#endif
	/* Got partition_loader node */
	nextoffset = fdt_first_subnode((void*)fdt_addr, nodeoffset);
	if (nextoffset < 0) {
		return -1;
	}

	nodeoffset = nextoffset;
	ptr = fdt_get_name((const void*)fdt_addr, nextoffset, NULL);
	#if defined(CONFIG_NVT_LINUX_EMMC_BOOT)
	while(strcmp(prop_val, "true") != 0) {
		nextoffset = fdt_next_node((void*)fdt_addr, nodeoffset, NULL);
		if (nextoffset < 0)
			return -1;
		prop_val = fdt_getprop((const void*)fdt_addr, nextoffset, "active", &len);
		ptr = fdt_get_name((const void*)fdt_addr, nextoffset, NULL);
		nodeoffset = nextoffset;
		if (strncmp(ptr, "partition_rootfs", 16) == 0)
			i++;
		if (strncmp(ptr, "partition_", 10) != 0)
			break;
	}
	#else
	while(strcmp(ptr, "partition_rootfs") != 0) {
		nextoffset = fdt_next_node((void*)fdt_addr, nodeoffset, NULL);
		if (nextoffset < 0)
			return -1;
		ptr = fdt_get_name((const void*)fdt_addr, nextoffset, NULL);
		nodeoffset = nextoffset;
		i++;
	}

	/* Check if we have two rootfs partitions. (ro + rw) */
	nextoffset = fdt_next_node((void*)fdt_addr, nodeoffset, NULL);
	if (nextoffset < 0)
		return -1;
	ptr = fdt_get_name((const void*)fdt_addr, nextoffset, NULL);
	if (strncmp(ptr, "partition_rootfs", 16) == 0)
		*ro_attr = 1;
	else
		*ro_attr = 0;
	#endif

	*mtd_num = i;
	return 0;
}

/*** NAND Util ***/
uint32_t nand_get_page_size(void)
{
	struct mtd_info *mtd = nand_info[nand_curr_device];
	return mtd->writesize;
}

uint32_t nand_get_block_size(void)
{
	struct mtd_info *mtd = nand_info[nand_curr_device];
	return mtd->erasesize;
}

uint32_t nand_total_blocks(void)
{
	struct mtd_info *mtd = nand_info[nand_curr_device];
	printf("NAND : total blocks number: %d\n", (uint32_t)(mtd->size >> mtd->erasesize_shift));
	return (uint32_t)(mtd->size >> mtd->erasesize_shift);
}

void nvt_print_system_info(void)
{
	printf("\r%s \
			\r CONFIG_MEM_SIZE              =      0x%08x \n \
			\r CONFIG_NVT_UIMAGE_SIZE       =      0x%08x \n \
			\r CONFIG_UBOOT_SDRAM_BASE      =      0x%08x \n \
			\r CONFIG_UBOOT_SDRAM_SIZE      =      0x%08x \n \
			\r CONFIG_LINUX_SDRAM_BASE      =      0x%08x \n \
			\r CONFIG_LINUX_SDRAM_SIZE      =      0x%08x \n \
			\r CONFIG_LINUX_SDRAM_START     =      0x%08x %s \n", ANSI_COLOR_YELLOW, CONFIG_MEM_SIZE,
														CONFIG_NVT_UIMAGE_SIZE, CONFIG_UBOOT_SDRAM_BASE,
														CONFIG_UBOOT_SDRAM_SIZE, CONFIG_LINUX_SDRAM_BASE,
														CONFIG_LINUX_SDRAM_SIZE, CONFIG_LINUX_SDRAM_START,
														ANSI_COLOR_RESET);
}
