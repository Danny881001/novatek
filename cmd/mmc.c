/*
 * (C) Copyright 2003
 * Kyle Harris, kharris@nexus-tech.net
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <command.h>
#include <console.h>
#include <mmc.h>

typedef enum {
	EMMC_PART_TYPE_USER = 0,
	EMMC_PART_TYPE_BOOT1 = 1,
	EMMC_PART_TYPE_BOOT2,
	EMMC_PART_TYPE_RPMB,
	EMMC_PART_TYPE_USER_,
	EMMC_PART_TYPE_MAX
}EMMC_PART_TYPE;
#define BYTE_TO_SECTOR_SFT 	9

static int curr_device = -1;

extern u8 saved_ext_csd[MMC_MAX_BLOCK_LEN];

static void print_mmcinfo(struct mmc *mmc)
{
	int i;

	printf("Device: %s\n", mmc->cfg->name);
	printf("Manufacturer ID: %x\n", mmc->cid[0] >> 24);
	printf("OEM: %x\n", (mmc->cid[0] >> 8) & 0xffff);
	printf("Name: %c%c%c%c%c \n", mmc->cid[0] & 0xff,
			(mmc->cid[1] >> 24), (mmc->cid[1] >> 16) & 0xff,
			(mmc->cid[1] >> 8) & 0xff, mmc->cid[1] & 0xff);

	printf("Tran Speed: %d\n", mmc->tran_speed);
	printf("Rd Block Len: %d\n", mmc->read_bl_len);

	printf("%s version %d.%d", IS_SD(mmc) ? "SD" : "MMC",
			EXTRACT_SDMMC_MAJOR_VERSION(mmc->version),
			EXTRACT_SDMMC_MINOR_VERSION(mmc->version));
	if (EXTRACT_SDMMC_CHANGE_VERSION(mmc->version) != 0)
		printf(".%d", EXTRACT_SDMMC_CHANGE_VERSION(mmc->version));
	printf("\n");

	printf("High Capacity: %s\n", mmc->high_capacity ? "Yes" : "No");
	puts("Capacity: ");
	print_size(mmc->capacity, "\n");

	printf("Bus Width: %d-bit%s\n", mmc->bus_width,
			mmc->ddr_mode ? " DDR" : "");

#ifndef CONFIG_NVT_IVOT_SOC
	printf("ext_csd[177] = 0x%x, ext_csd[179] = 0x%x\n", saved_ext_csd[EXT_CSD_BOOT_BUS_WIDTH], saved_ext_csd[EXT_CSD_PART_CONF]);
	printf("ext_csd[267] = 0x%x ext_csd[268] = 0x%x, ext_csd[269] = 0x%x\n", saved_ext_csd[267], saved_ext_csd[268], saved_ext_csd[269]);

	switch(saved_ext_csd[267]) {
		case 0:
			printf("Life time not define\n");
			break;

		case 1:
			printf("Life time normal\n");
			break;

		case 2:
			printf("Life time warning\n");
			break;

		case 3:
			printf("Life time urgent\n");
			break;

		default:
			break;
	}
#endif
	puts("Erase Group Size: ");
	print_size(((u64)mmc->erase_grp_size) << 9, "\n");

	if (!IS_SD(mmc) && mmc->version >= MMC_VERSION_4_41) {
		bool has_enh = (mmc->part_support & ENHNCD_SUPPORT) != 0;
		bool usr_enh = has_enh && (mmc->part_attr & EXT_CSD_ENH_USR);

		puts("HC WP Group Size: ");
		print_size(((u64)mmc->hc_wp_grp_size) << 9, "\n");

		puts("User Capacity: ");
		print_size(mmc->capacity_user, usr_enh ? " ENH" : "");
		if (mmc->wr_rel_set & EXT_CSD_WR_DATA_REL_USR)
			puts(" WRREL\n");
		else
			putc('\n');
		if (usr_enh) {
			puts("User Enhanced Start: ");
			print_size(mmc->enh_user_start, "\n");
			puts("User Enhanced Size: ");
			print_size(mmc->enh_user_size, "\n");
		}
		puts("Boot Capacity: ");
		print_size(mmc->capacity_boot, has_enh ? " ENH\n" : "\n");
		puts("RPMB Capacity: ");
		print_size(mmc->capacity_rpmb, has_enh ? " ENH\n" : "\n");

		for (i = 0; i < ARRAY_SIZE(mmc->capacity_gp); i++) {
			bool is_enh = has_enh &&
				(mmc->part_attr & EXT_CSD_ENH_GP(i));
			if (mmc->capacity_gp[i]) {
				printf("GP%i Capacity: ", i+1);
				print_size(mmc->capacity_gp[i],
					   is_enh ? " ENH" : "");
				if (mmc->wr_rel_set & EXT_CSD_WR_DATA_REL_GP(i))
					puts(" WRREL\n");
				else
					putc('\n');
			}
		}
		mmc_dump_boot_info(mmc);
	}
}
/*static*/ struct mmc *init_mmc_device(int dev, bool force_init)
{
	struct mmc *mmc;
	mmc = find_mmc_device(dev);
	if (!mmc) {
		printf("no mmc device at slot %x\n", dev);
		return NULL;
	}

	if (force_init)
		mmc->has_init = 0;
	if (mmc_init(mmc))
		return NULL;
	return mmc;
}
u32 get_emmc_capacity(void)
{
        struct mmc *mmc = init_mmc_device(curr_device, false);
        if (!mmc){
		printf("fail to init emmc\r\n");
                return 0;
	}
        return (mmc->capacity / 512);
}
static int do_mmcinfo(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	struct mmc *mmc;

	if (curr_device < 0) {
		if (get_mmc_num() > 0)
			curr_device = 0;
		else {
			puts("No MMC device available\n");
			return 1;
		}
	}

	mmc = init_mmc_device(curr_device, false);
	if (!mmc)
		return CMD_RET_FAILURE;

	print_mmcinfo(mmc);
	return CMD_RET_SUCCESS;
}
#if defined(CONFIG_NVT_MMC_INFO)
static int do_mmc_ext_info(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	struct mmc *mmc;

	if (curr_device < 0) {
		if (get_mmc_num() > 0)
			curr_device = 0;
		else {
			puts("No MMC device available\n");
			return 1;
		}
	}

	mmc = init_mmc_device(curr_device, false);
	if (!mmc)
		return CMD_RET_FAILURE;


		if (!strcmp(argv[1], "all")) {
			print_mmc_ext_csd();
		} else if (!strcmp(argv[1], "life")) {
			print_mmc_life();
		} else if (!strcmp(argv[1], "boot")) {
			print_mmc_partition_cfg();
		} else if (!strcmp(argv[1], "wr")) {
			print_mmc_write_reliability();
		} else if (!strcmp(argv[1], "help")) {
			print_mmc_ext_info_help();
		} else {
			print_mmc_ext_info_help();
		}

	return CMD_RET_SUCCESS;
}
#endif

#ifdef CONFIG_SUPPORT_EMMC_RPMB
static int confirm_key_prog(void)
{
	puts("Warning: Programming authentication key can be done only once !\n"
	     "         Use this command only if you are sure of what you are doing,\n"
	     "Really perform the key programming? <y/N> ");
	if (confirm_yesno())
		return 1;

	puts("Authentication key programming aborted\n");
	return 0;
}
static int do_mmcrpmb_key(cmd_tbl_t *cmdtp, int flag,
			  int argc, char * const argv[])
{
	void *key_addr;
	struct mmc *mmc = find_mmc_device(curr_device);

	if (argc != 2)
		return CMD_RET_USAGE;

	key_addr = (void *)simple_strtoul(argv[1], NULL, 16);
	if (!confirm_key_prog())
		return CMD_RET_FAILURE;
	if (mmc_rpmb_set_key(mmc, key_addr)) {
		printf("ERROR - Key already programmed ?\n");
		return CMD_RET_FAILURE;
	}
	return CMD_RET_SUCCESS;
}
static int do_mmcrpmb_read(cmd_tbl_t *cmdtp, int flag,
			   int argc, char * const argv[])
{
	u16 blk, cnt;
	void *addr;
	int n;
	void *key_addr = NULL;
	struct mmc *mmc = find_mmc_device(curr_device);

	if (argc < 4)
		return CMD_RET_USAGE;

	addr = (void *)simple_strtoul(argv[1], NULL, 16);
	blk = simple_strtoul(argv[2], NULL, 16);
	cnt = simple_strtoul(argv[3], NULL, 16);

	if (argc == 5)
		key_addr = (void *)simple_strtoul(argv[4], NULL, 16);

	printf("\nMMC RPMB read: dev # %d, block # %d, count %d ... ",
	       curr_device, blk, cnt);
	n =  mmc_rpmb_read(mmc, addr, blk, cnt, key_addr);

	printf("%d RPMB blocks read: %s\n", n, (n == cnt) ? "OK" : "ERROR");
	if (n != cnt)
		return CMD_RET_FAILURE;
	return CMD_RET_SUCCESS;
}
static int do_mmcrpmb_write(cmd_tbl_t *cmdtp, int flag,
			    int argc, char * const argv[])
{
	u16 blk, cnt;
	void *addr;
	int n;
	void *key_addr;
	struct mmc *mmc = find_mmc_device(curr_device);

	if (argc != 5)
		return CMD_RET_USAGE;

	addr = (void *)simple_strtoul(argv[1], NULL, 16);
	blk = simple_strtoul(argv[2], NULL, 16);
	cnt = simple_strtoul(argv[3], NULL, 16);
	key_addr = (void *)simple_strtoul(argv[4], NULL, 16);

	printf("\nMMC RPMB write: dev # %d, block # %d, count %d ... ",
	       curr_device, blk, cnt);
	n =  mmc_rpmb_write(mmc, addr, blk, cnt, key_addr);

	printf("%d RPMB blocks written: %s\n", n, (n == cnt) ? "OK" : "ERROR");
	if (n != cnt)
		return CMD_RET_FAILURE;
	return CMD_RET_SUCCESS;
}
static int do_mmcrpmb_counter(cmd_tbl_t *cmdtp, int flag,
			      int argc, char * const argv[])
{
	unsigned long counter;
	struct mmc *mmc = find_mmc_device(curr_device);

	if (mmc_rpmb_get_counter(mmc, &counter))
		return CMD_RET_FAILURE;
	printf("RPMB Write counter= %lx\n", counter);
	return CMD_RET_SUCCESS;
}

static cmd_tbl_t cmd_rpmb[] = {
	U_BOOT_CMD_MKENT(key, 2, 0, do_mmcrpmb_key, "", ""),
	U_BOOT_CMD_MKENT(read, 5, 1, do_mmcrpmb_read, "", ""),
	U_BOOT_CMD_MKENT(write, 5, 0, do_mmcrpmb_write, "", ""),
	U_BOOT_CMD_MKENT(counter, 1, 1, do_mmcrpmb_counter, "", ""),
};

static int do_mmcrpmb(cmd_tbl_t *cmdtp, int flag,
		      int argc, char * const argv[])
{
	cmd_tbl_t *cp;
	struct mmc *mmc;
	char original_part;
	int ret;

	cp = find_cmd_tbl(argv[1], cmd_rpmb, ARRAY_SIZE(cmd_rpmb));

	/* Drop the rpmb subcommand */
	argc--;
	argv++;

	if (cp == NULL || argc > cp->maxargs)
		return CMD_RET_USAGE;
	if (flag == CMD_FLAG_REPEAT && !cp->repeatable)
		return CMD_RET_SUCCESS;

	mmc = init_mmc_device(curr_device, false);
	if (!mmc)
		return CMD_RET_FAILURE;

	if (!(mmc->version & MMC_VERSION_MMC)) {
		printf("It is not a EMMC device\n");
		return CMD_RET_FAILURE;
	}
	if (mmc->version < MMC_VERSION_4_41) {
		printf("RPMB not supported before version 4.41\n");
		return CMD_RET_FAILURE;
	}
	/* Switch to the RPMB partition */
	original_part = mmc->block_dev.hwpart;
	if (blk_select_hwpart_devnum(IF_TYPE_MMC, curr_device, MMC_PART_RPMB) !=
	    0)
		return CMD_RET_FAILURE;
	ret = cp->cmd(cmdtp, flag, argc, argv);

	/* Return to original partition */
	if (blk_select_hwpart_devnum(IF_TYPE_MMC, curr_device, original_part) !=
	    0)
		return CMD_RET_FAILURE;
	return ret;
}
#endif

static int do_mmc_read(cmd_tbl_t *cmdtp, int flag,
		       int argc, char * const argv[])
{
	struct mmc *mmc;
	u32 blk, cnt, n;
	void *addr;

	if (argc != 4)
		return CMD_RET_USAGE;

	addr = (void *)simple_strtoul(argv[1], NULL, 16);
	blk = simple_strtoul(argv[2], NULL, 16);
	cnt = simple_strtoul(argv[3], NULL, 16);

	mmc = init_mmc_device(curr_device, false);
	if (!mmc)
		return CMD_RET_FAILURE;

	printf("\nMMC read: dev # %d, block # %d, count %d ... ",
	       curr_device, blk, cnt);

	n = blk_dread(mmc_get_blk_desc(mmc), blk, cnt, addr);
	/* flush cache after read */
	flush_cache((ulong)addr, cnt * 512); /* FIXME */
	printf("%d blocks read: %s\n", n, (n == cnt) ? "OK" : "ERROR");

	return (n == cnt) ? CMD_RET_SUCCESS : CMD_RET_FAILURE;
}
static int do_mmc_write(cmd_tbl_t *cmdtp, int flag,
			int argc, char * const argv[])
{
	struct mmc *mmc;
	u32 blk, cnt, n;
	void *addr;

	if (argc != 4)
		return CMD_RET_USAGE;

	addr = (void *)simple_strtoul(argv[1], NULL, 16);
	blk = simple_strtoul(argv[2], NULL, 16);
	cnt = simple_strtoul(argv[3], NULL, 16);

	mmc = init_mmc_device(curr_device, false);
	if (!mmc)
		return CMD_RET_FAILURE;

	printf("\nMMC write: dev # %d, block # %d, count %d ... ",
	       curr_device, blk, cnt);

	if (mmc_getwp(mmc) == 1) {
		printf("Error: card is write protected!\n");
		return CMD_RET_FAILURE;
	}
	n = blk_dwrite(mmc_get_blk_desc(mmc), blk, cnt, addr);
	printf("%d blocks written: %s\n", n, (n == cnt) ? "OK" : "ERROR");

	return (n == cnt) ? CMD_RET_SUCCESS : CMD_RET_FAILURE;
}

#ifndef CONFIG_NVT_IVOT_SOC
#define ERASE_PAT_SIZE (512*CONFIG_SYS_MMC_MAX_BLK_COUNT)
#define EMMC_BLK_SIZE (512)
//unsigned char erase_pattern[ERASE_PAT_SIZE];
static int do_mmc_write_erase(struct mmc *mmc, u32 blk, u32 cnt, void *addr)
{

	u32  n=0;
	int i = 0;
	int check_print =  0;
	int mix_write_buf_size =  0;



	printf("\nMMC erase : dev # %d, block # %d, count %d ... \n",
	       curr_device, blk, cnt);

	if (mmc_getwp(mmc) == 1) {
		printf("Error: card is write protected!\n");
		return CMD_RET_FAILURE;
	}

	for(i = 0; i < cnt; ){
		int _check = i*100/cnt;
		if(_check != check_print){
//			printf("[%d %d-%d]", _check, i, cnt);
			printf("\r Processing(%d%%).....", _check);
			check_print = _check;
		}
		if( (i+(ERASE_PAT_SIZE/EMMC_BLK_SIZE)) < cnt){
			mmc->block_dev.block_write(&mmc->block_dev, blk+i, (ERASE_PAT_SIZE/EMMC_BLK_SIZE), addr);
			i+= (ERASE_PAT_SIZE/EMMC_BLK_SIZE);
			n+=(ERASE_PAT_SIZE/EMMC_BLK_SIZE);
		}else{
			for(; i < cnt; ){
				if(ERASE_PAT_SIZE/EMMC_BLK_SIZE > cnt )
					mix_write_buf_size = cnt;
				else
					mix_write_buf_size = cnt - i;


				mmc->block_dev.block_write(&mmc->block_dev, blk+i, mix_write_buf_size, addr);
				i+=mix_write_buf_size;
				n+=mix_write_buf_size;
			}
		}
	}
	printf("%d blocks written 1 to erase: %s\n", n, (n == cnt) ? "OK" : "ERROR");

	return (n == cnt) ? CMD_RET_SUCCESS : CMD_RET_FAILURE;

}

static int do_mmc_write1erase(cmd_tbl_t *cmdtp, int flag,
			int argc, char * const argv[])
{
	struct mmc *mmc;
	u32 blk, cnt;
	void *addr;
	unsigned char *erase_pattern = (unsigned char *)CONFIG_SYS_FWUPDATE_BUF;

	if (argc != 3)
		return CMD_RET_USAGE;

	mmc = init_mmc_device(curr_device, false);
	if (!mmc)
		return CMD_RET_FAILURE;

	addr = erase_pattern;
	blk = simple_strtoul(argv[1], NULL, 16);
	cnt = simple_strtoul(argv[2], NULL, 16);
	memset(erase_pattern, 0xff,ERASE_PAT_SIZE);


	printf("\nMMC erase by 1:  block # %d, count %d ... ",
	        blk, cnt);
	return do_mmc_write_erase(mmc, blk, cnt, addr);
}

static int do_mmc_write1all(cmd_tbl_t *cmdtp, int flag,
			int argc, char * const argv[])
{
	struct mmc *mmc;
	u32 blk, cnt;
	int ret = 0;
	int i = 0;
	char cmd[128] ;
	unsigned char *erase_pattern = (unsigned char *)CONFIG_SYS_FWUPDATE_BUF;

	for(i = 0 ; i < 4; i++){
		if(i == 3 ){
			printf("rpmb is not erase!\n");
			break;
		}

		sprintf(cmd,"mmc dev %d %d", curr_device, i);
		ret = run_command(cmd, 0);
		if(ret ){
			printf("partition switch fail\n");
			break;
		}

		mmc = init_mmc_device(curr_device, false);
		if (!mmc)
			return CMD_RET_FAILURE;

		memset(erase_pattern, 0xff,ERASE_PAT_SIZE);

		blk = 0;
		cnt =(mmc->read_bl_len == 512 )? mmc->capacity/512 :
			(mmc->read_bl_len == 2048 )? mmc->capacity/2048 :
			mmc->capacity/512; //check...
		printf("\nMMC erase all dev %d  by 1:  block # %d, count %d ... ",
				i ,blk, cnt);

		ret = do_mmc_write_erase(mmc, blk, cnt, erase_pattern);
		if(ret )
			break;
	}

	sprintf(cmd, "mmc dev %d 0", curr_device);
	run_command(cmd, 0);

	return ret;
}

static int do_mmc_write0all(cmd_tbl_t *cmdtp, int flag,
			int argc, char * const argv[])
{
	struct mmc *mmc;
	u32 blk, cnt;
	int ret = 0;
	int i = 0;
	char cmd[128] ;
	unsigned char *erase_pattern = (unsigned char *)CONFIG_SYS_FWUPDATE_BUF;

	for(i = 0 ; i < 4;i++){
		if(i == 3 ){
			printf("rpmb is not erase!\n");
			break;
		}
		sprintf(cmd,"mmc dev %d  %d", curr_device, i);
		ret = run_command(cmd, 0);
		if(ret ){
			printf("partition switch fail\n");
			break;
		}

		//printf("cmd =>%s %d \n",cmd,curr_device);
		memset(erase_pattern, 0x0,ERASE_PAT_SIZE);

		mmc = init_mmc_device(curr_device, false);
		if (!mmc)
			return CMD_RET_FAILURE;

		blk = 0;
		cnt =(mmc->read_bl_len == 512 )? mmc->capacity/512 :
			(mmc->read_bl_len == 2048 )? mmc->capacity/2048 :
			mmc->capacity/512; //check...

		printf("\nMMC erase all by 0:  block # %d, count %d ... ",
				blk, cnt);

		ret =  do_mmc_write_erase(mmc, blk, cnt, erase_pattern);
		if(ret )break;
	}

	sprintf(cmd, "mmc dev %d 0", curr_device);
	run_command(cmd, 0);

	return ret;
}
#endif

#if defined(CONFIG_EMMC_WRITE_RELIABILITY_TEST)
static int do_mmc_write0boot(cmd_tbl_t *cmdtp, int flag,
			int argc, char * const argv[])
{
	struct mmc *mmc;
	u32 blk, cnt;
	int ret = 0;
	unsigned char *erase_pattern = (unsigned char *)CONFIG_SYS_FWUPDATE_BUF;

		if(ret ){
			printf("partition switch fail\n");
			return CMD_RET_FAILURE;
		}

		//printf("cmd =>%s %d \n",cmd,curr_device);
		memset(erase_pattern, 0x0,ERASE_PAT_SIZE);

		mmc = init_mmc_device(curr_device, false);
		if (!mmc)
			return CMD_RET_FAILURE;

		blk = 0;
		cnt =(mmc->read_bl_len == 512 )? mmc->capacity/512 :
			(mmc->read_bl_len == 2048 )? mmc->capacity/2048 :
			mmc->capacity/512; //check...

		printf("\nMMC erase all by 0:  block # %d, count %d ... ",
				blk, cnt);

		ret =  do_mmc_write_erase(mmc, blk, cnt, erase_pattern);


	return ret;
}
#endif

#ifndef CONFIG_NVT_IVOT_SOC
static int do_mmc_write0erase(cmd_tbl_t *cmdtp, int flag,
			int argc, char * const argv[])
{
	struct mmc *mmc;
	u32 blk, cnt;
	void *addr;
	unsigned char *erase_pattern = (unsigned char *)CONFIG_SYS_FWUPDATE_BUF;

	if (argc != 3)
		return CMD_RET_USAGE;

	mmc = init_mmc_device(curr_device, false);
	if (!mmc)
		return CMD_RET_FAILURE;

	addr = erase_pattern;
	blk = simple_strtoul(argv[1], NULL, 16);
	cnt = simple_strtoul(argv[2], NULL, 16);
	memset(erase_pattern, 0x0,ERASE_PAT_SIZE);


	printf("\nMMC erase by 0:  block # %d, count %d ... ",
	        blk, cnt);

	return do_mmc_write_erase(mmc, blk, cnt, addr);
}
#endif

static int do_mmc_erase(cmd_tbl_t *cmdtp, int flag,
			int argc, char * const argv[])
{
	struct mmc *mmc;
	u32 blk, cnt, n;

	if (argc != 3)
		return CMD_RET_USAGE;

	blk = simple_strtoul(argv[1], NULL, 16);
	cnt = simple_strtoul(argv[2], NULL, 16);

	mmc = init_mmc_device(curr_device, false);
	if (!mmc)
		return CMD_RET_FAILURE;

	printf("\nMMC erase: dev # %d, block # %d, count %d ... ",
	       curr_device, blk, cnt);

	if (mmc_getwp(mmc) == 1) {
		printf("Error: card is write protected!\n");
		return CMD_RET_FAILURE;
	}
	n = blk_derase(mmc_get_blk_desc(mmc), blk, cnt);
	printf("%d blocks erased: %s\n", n, (n == cnt) ? "OK" : "ERROR");

	return (n == cnt) ? CMD_RET_SUCCESS : CMD_RET_FAILURE;
}
static int do_mmc_rescan(cmd_tbl_t *cmdtp, int flag,
			 int argc, char * const argv[])
{
	struct mmc *mmc;

	mmc = init_mmc_device(curr_device, true);
	if (!mmc)
		return CMD_RET_FAILURE;

	return CMD_RET_SUCCESS;
}
static int do_mmc_part(cmd_tbl_t *cmdtp, int flag,
		       int argc, char * const argv[])
{
	struct blk_desc *mmc_dev;
	struct mmc *mmc;

	mmc = init_mmc_device(curr_device, false);
	if (!mmc)
		return CMD_RET_FAILURE;

	mmc_dev = blk_get_devnum_by_type(IF_TYPE_MMC, curr_device);
	if (mmc_dev != NULL && mmc_dev->type != DEV_TYPE_UNKNOWN) {
		part_print(mmc_dev);
		return CMD_RET_SUCCESS;
	}

	puts("get mmc type error!\n");
	return CMD_RET_FAILURE;
}
static int do_mmc_dev(cmd_tbl_t *cmdtp, int flag,
		      int argc, char * const argv[])
{
	int dev, part = 0, ret;
	struct mmc *mmc;

	if (argc == 1) {
		dev = curr_device;
	} else if (argc == 2) {
		dev = simple_strtoul(argv[1], NULL, 10);
	} else if (argc == 3) {
		dev = (int)simple_strtoul(argv[1], NULL, 10);
		part = (int)simple_strtoul(argv[2], NULL, 10);
		if (part > PART_ACCESS_MASK) {
			printf("#part_num shouldn't be larger than %d\n",
			       PART_ACCESS_MASK);
			return CMD_RET_FAILURE;
		}
	} else {
		return CMD_RET_USAGE;
	}

	mmc = init_mmc_device(dev, true);
	if (!mmc)
		return CMD_RET_FAILURE;

	ret = blk_select_hwpart_devnum(IF_TYPE_MMC, dev, part);
	printf("switch to partitions #%d, %s\n",
	       part, (!ret) ? "OK" : "ERROR");
	if (ret)
		return 1;

	curr_device = dev;
	if (mmc->part_config == MMCPART_NOAVAILABLE)
		printf("mmc%d is current device\n", curr_device);
	else
		printf("mmc%d(part %d) is current device\n",
		       curr_device, mmc_get_blk_desc(mmc)->hwpart);

	return CMD_RET_SUCCESS;
}
static int do_mmc_list(cmd_tbl_t *cmdtp, int flag,
		       int argc, char * const argv[])
{
	print_mmc_devices('\n');
	return CMD_RET_SUCCESS;
}

static int parse_hwpart_user(struct mmc_hwpart_conf *pconf,
			     int argc, char * const argv[])
{
	int i = 0;

	memset(&pconf->user, 0, sizeof(pconf->user));

	while (i < argc) {
		if (!strcmp(argv[i], "enh")) {
			if (i + 2 >= argc)
				return -1;
			pconf->user.enh_start =
				simple_strtoul(argv[i+1], NULL, 10);
			pconf->user.enh_size =
				simple_strtoul(argv[i+2], NULL, 10);
			i += 3;
		} else if (!strcmp(argv[i], "wrrel")) {
			if (i + 1 >= argc)
				return -1;
			pconf->user.wr_rel_change = 1;
			if (!strcmp(argv[i+1], "on"))
				pconf->user.wr_rel_set = 1;
			else if (!strcmp(argv[i+1], "off"))
				pconf->user.wr_rel_set = 0;
			else
				return -1;
			i += 2;
		} else {
			break;
		}
	}
	return i;
}

static int parse_hwpart_gp(struct mmc_hwpart_conf *pconf, int pidx,
			   int argc, char * const argv[])
{
	int i;

	memset(&pconf->gp_part[pidx], 0, sizeof(pconf->gp_part[pidx]));

	if (1 >= argc)
		return -1;
	pconf->gp_part[pidx].size = simple_strtoul(argv[0], NULL, 10);

	i = 1;
	while (i < argc) {
		if (!strcmp(argv[i], "enh")) {
			pconf->gp_part[pidx].enhanced = 1;
			i += 1;
		} else if (!strcmp(argv[i], "wrrel")) {
			if (i + 1 >= argc)
				return -1;
			pconf->gp_part[pidx].wr_rel_change = 1;
			if (!strcmp(argv[i+1], "on"))
				pconf->gp_part[pidx].wr_rel_set = 1;
			else if (!strcmp(argv[i+1], "off"))
				pconf->gp_part[pidx].wr_rel_set = 0;
			else
				return -1;
			i += 2;
		} else {
			break;
		}
	}
	return i;
}

static int do_mmc_hwpartition(cmd_tbl_t *cmdtp, int flag,
			      int argc, char * const argv[])
{
	struct mmc *mmc;
	struct mmc_hwpart_conf pconf = { };
	enum mmc_hwpart_conf_mode mode = MMC_HWPART_CONF_CHECK;
	int i, r, pidx;

	mmc = init_mmc_device(curr_device, false);
	if (!mmc)
		return CMD_RET_FAILURE;

	if (argc < 1)
		return CMD_RET_USAGE;
	i = 1;
	while (i < argc) {
		if (!strcmp(argv[i], "user")) {
			i++;
			r = parse_hwpart_user(&pconf, argc-i, &argv[i]);
			if (r < 0)
				return CMD_RET_USAGE;
			i += r;
		} else if (!strncmp(argv[i], "gp", 2) &&
			   strlen(argv[i]) == 3 &&
			   argv[i][2] >= '1' && argv[i][2] <= '4') {
			pidx = argv[i][2] - '1';
			i++;
			r = parse_hwpart_gp(&pconf, pidx, argc-i, &argv[i]);
			if (r < 0)
				return CMD_RET_USAGE;
			i += r;
		} else if (!strcmp(argv[i], "check")) {
			mode = MMC_HWPART_CONF_CHECK;
			i++;
		} else if (!strcmp(argv[i], "set")) {
			mode = MMC_HWPART_CONF_SET;
			i++;
		} else if (!strcmp(argv[i], "complete")) {
			mode = MMC_HWPART_CONF_COMPLETE;
			i++;
		} else {
			return CMD_RET_USAGE;
		}
	}

	puts("Partition configuration:\n");
	if (pconf.user.enh_size) {
		puts("\tUser Enhanced Start: ");
		print_size(((u64)pconf.user.enh_start) << 9, "\n");
		puts("\tUser Enhanced Size: ");
		print_size(((u64)pconf.user.enh_size) << 9, "\n");
	} else {
		puts("\tNo enhanced user data area\n");
	}
	if (pconf.user.wr_rel_change)
		printf("\tUser partition write reliability: %s\n",
		       pconf.user.wr_rel_set ? "on" : "off");
	for (pidx = 0; pidx < 4; pidx++) {
		if (pconf.gp_part[pidx].size) {
			printf("\tGP%i Capacity: ", pidx+1);
			print_size(((u64)pconf.gp_part[pidx].size) << 9,
				   pconf.gp_part[pidx].enhanced ?
				   " ENH\n" : "\n");
		} else {
			printf("\tNo GP%i partition\n", pidx+1);
		}
		if (pconf.gp_part[pidx].wr_rel_change)
			printf("\tGP%i write reliability: %s\n", pidx+1,
			       pconf.gp_part[pidx].wr_rel_set ? "on" : "off");
	}

	if (!mmc_hwpart_config(mmc, &pconf, mode)) {
		if (mode == MMC_HWPART_CONF_COMPLETE)
			puts("Partitioning successful, "
			     "power-cycle to make effective\n");
		return CMD_RET_SUCCESS;
	} else {
		puts("Failed!\n");
		return CMD_RET_FAILURE;
	}
}

#ifdef CONFIG_SUPPORT_EMMC_BOOT
static int do_mmc_bootbus(cmd_tbl_t *cmdtp, int flag,
			  int argc, char * const argv[])
{
	int dev;
	struct mmc *mmc;
	u8 width, reset, mode;

	if (argc != 5)
		return CMD_RET_USAGE;
	dev = simple_strtoul(argv[1], NULL, 10);
	width = simple_strtoul(argv[2], NULL, 10);
	reset = simple_strtoul(argv[3], NULL, 10);
	mode = simple_strtoul(argv[4], NULL, 10);

	mmc = init_mmc_device(dev, false);
	if (!mmc)
		return CMD_RET_FAILURE;

	if (IS_SD(mmc)) {
		puts("BOOT_BUS_WIDTH only exists on eMMC\n");
		return CMD_RET_FAILURE;
	}

	/* acknowledge to be sent during boot operation */
	return mmc_set_boot_bus_width(mmc, width, reset, mode);
}
static int do_mmc_boot_resize(cmd_tbl_t *cmdtp, int flag,
			      int argc, char * const argv[])
{
	int dev;
	struct mmc *mmc;
	u32 bootsize, rpmbsize;

	if (argc != 4)
		return CMD_RET_USAGE;
	dev = simple_strtoul(argv[1], NULL, 10);
	bootsize = simple_strtoul(argv[2], NULL, 10);
	rpmbsize = simple_strtoul(argv[3], NULL, 10);

	mmc = init_mmc_device(dev, false);
	if (!mmc)
		return CMD_RET_FAILURE;

	if (IS_SD(mmc)) {
		printf("It is not a EMMC device\n");
		return CMD_RET_FAILURE;
	}

	if (mmc_boot_partition_size_change(mmc, bootsize, rpmbsize)) {
		printf("EMMC boot partition Size change Failed.\n");
		return CMD_RET_FAILURE;
	}

	printf("EMMC boot partition Size %d MB\n", bootsize);
	printf("EMMC RPMB partition Size %d MB\n", rpmbsize);
	return CMD_RET_SUCCESS;
}
static int do_mmc_partconf(cmd_tbl_t *cmdtp, int flag,
			   int argc, char * const argv[])
{
	int dev;
	struct mmc *mmc;
	u8 ack, part_num, access;

	if (argc != 5)
		return CMD_RET_USAGE;

	dev = simple_strtoul(argv[1], NULL, 10);
	ack = simple_strtoul(argv[2], NULL, 10);
	part_num = simple_strtoul(argv[3], NULL, 10);
	access = simple_strtoul(argv[4], NULL, 10);

	mmc = init_mmc_device(dev, false);
	if (!mmc)
		return CMD_RET_FAILURE;

	if (IS_SD(mmc)) {
		puts("PARTITION_CONFIG only exists on eMMC\n");
		return CMD_RET_FAILURE;
	}

	/* acknowledge to be sent during boot operation */
	return mmc_set_part_conf(mmc, ack, part_num, access);
}
static int do_mmc_rst_func(cmd_tbl_t *cmdtp, int flag,
			   int argc, char * const argv[])
{
	int dev;
	struct mmc *mmc;
	u8 enable;

	/*
	 * Set the RST_n_ENABLE bit of RST_n_FUNCTION
	 * The only valid values are 0x0, 0x1 and 0x2 and writing
	 * a value of 0x1 or 0x2 sets the value permanently.
	 */
	if (argc != 3)
		return CMD_RET_USAGE;

	dev = simple_strtoul(argv[1], NULL, 10);
	enable = simple_strtoul(argv[2], NULL, 10);

	if (enable > 2) {
		puts("Invalid RST_n_ENABLE value\n");
		return CMD_RET_USAGE;
	}

	mmc = init_mmc_device(dev, false);
	if (!mmc)
		return CMD_RET_FAILURE;

	if (IS_SD(mmc)) {
		puts("RST_n_FUNCTION only exists on eMMC\n");
		return CMD_RET_FAILURE;
	}

	return mmc_set_rst_n_function(mmc, enable);
}
#endif
static int do_mmc_setdsr(cmd_tbl_t *cmdtp, int flag,
			 int argc, char * const argv[])
{
	struct mmc *mmc;
	u32 val;
	int ret;

	if (argc != 2)
		return CMD_RET_USAGE;
	val = simple_strtoul(argv[2], NULL, 16);

	mmc = find_mmc_device(curr_device);
	if (!mmc) {
		printf("no mmc device at slot %x\n", curr_device);
		return CMD_RET_FAILURE;
	}
	ret = mmc_set_dsr(mmc, val);
	printf("set dsr %s\n", (!ret) ? "OK, force rescan" : "ERROR");
	if (!ret) {
		mmc->has_init = 0;
		if (mmc_init(mmc))
			return CMD_RET_FAILURE;
		else
			return CMD_RET_SUCCESS;
	}
	return ret;
}

#ifndef CONFIG_NVT_IVOT_SOC
 __attribute__((weak)) int nvt_mmc_erase_part(EMMC_PART_TYPE part_type)
{
	char cmd[128] = {};
	int ret = 0;
//	struct mmc *mmc = NULL;
	//printf("%s %s\n",__FILE__, __FUNCTION__);
	struct mmc *mmc = find_mmc_device(CONFIG_SYS_MMC_SYS_DEV);

	switch(part_type) {
		case EMMC_PART_TYPE_BOOT1:
			ret = run_command("mmc dev 0 1", 0);
			if(ret < 0) {
				printf("%s switch to boot1 partition fail !\n",__func__);
				goto out;
			}
		//	mmc = find_mmc_device(CONFIG_SYS_MMC_SYS_DEV);
			//mmc = init_mmc_device(curr_device, false);
			sprintf(cmd,"mmc erase 0 0x%x",(unsigned)(mmc->capacity) >> BYTE_TO_SECTOR_SFT);
			ret = run_command(cmd, 0);
			if(ret < 0) {
				printf("%s erase boot1 partition fail !\n",__func__);
				goto out;
			}
			break;

		case EMMC_PART_TYPE_BOOT2:
			ret = run_command("mmc dev 0 2", 0);
			if(ret < 0) {
				printf("%s switch to boot2 partition fail !\n",__func__);
				goto out;
			}
		//	mmc = find_mmc_device(CONFIG_SYS_MMC_SYS_DEV);
			//mmc = init_mmc_device(curr_device, false);
			sprintf(cmd,"mmc erase 0 0x%x",(unsigned)(mmc->capacity) >> BYTE_TO_SECTOR_SFT);
			ret = run_command(cmd, 0);
			if(ret < 0) {
				printf("%s erase boot2 partition fail !\n",__func__);
				goto out;
			}

			break;

		case EMMC_PART_TYPE_USER:
		case EMMC_PART_TYPE_USER_:
			ret = run_command("mmc dev 0 0", 0);
			if(ret < 0) {
				printf("%s switch to user partition fail !\n",__func__);
				goto out;
			}

		//	mmc = find_mmc_device(CONFIG_SYS_MMC_SYS_DEV);
			//mmc = init_mmc_device(curr_device, false);
			sprintf(cmd,"mmc erase 0 0x%x",(unsigned)(mmc->capacity) >> BYTE_TO_SECTOR_SFT);
			ret = run_command(cmd, 0);
			if(ret < 0) {
				printf("%s erase user partition fail !\n",__func__);
				goto out;
			}
			break;

		case EMMC_PART_TYPE_RPMB:
		default:
			break;
	}

out:
	if(part_type == EMMC_PART_TYPE_BOOT1 || part_type == EMMC_PART_TYPE_BOOT2)
		run_command("mmc dev 0 0", 0);
	return ret;
}

int do_emmc_eraseall(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{

	nvt_mmc_erase_part(EMMC_PART_TYPE_BOOT1);
	nvt_mmc_erase_part(EMMC_PART_TYPE_BOOT2);
	nvt_mmc_erase_part(EMMC_PART_TYPE_USER_);
	return 0;
}

int do_emmc_enable_egroup(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	struct mmc *mmc;
	int ret;

	mmc = init_mmc_device(0, false);
	if (!mmc)
		return CMD_RET_FAILURE;

#if defined(CONFIG_NVT_ERASE_TOSHIBA_TEST_PATCH)
	ret = CMD_RET_SUCCESS ;
	mmc->en_egroup = 1;
	return ret;
#else
	ret = mmc_set_enhance_user_attribute(mmc);
	return ret;
#endif
}

int do_emmc_disable_egroup(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	struct mmc *mmc;
	int ret;

	mmc = init_mmc_device(0, false);
	if (!mmc)
		return CMD_RET_FAILURE;
#if defined(CONFIG_NVT_ERASE_TOSHIBA_TEST_PATCH)
	ret = CMD_RET_SUCCESS ;
	mmc->en_egroup = 0;
	return ret;
#else

	ret = mmc_clear_enhance_user_attribute(mmc);

	return ret;
#endif
}
#endif

static cmd_tbl_t cmd_mmc[] = {
	U_BOOT_CMD_MKENT(info, 1, 0, do_mmcinfo, "", ""),
	U_BOOT_CMD_MKENT(read, 4, 1, do_mmc_read, "", ""),
	U_BOOT_CMD_MKENT(write, 4, 0, do_mmc_write, "", ""),
	U_BOOT_CMD_MKENT(erase, 3, 0, do_mmc_erase, "", ""),
	U_BOOT_CMD_MKENT(rescan, 1, 1, do_mmc_rescan, "", ""),
	U_BOOT_CMD_MKENT(part, 1, 1, do_mmc_part, "", ""),
	U_BOOT_CMD_MKENT(dev, 3, 0, do_mmc_dev, "", ""),
	U_BOOT_CMD_MKENT(list, 1, 1, do_mmc_list, "", ""),
	U_BOOT_CMD_MKENT(hwpartition, 28, 0, do_mmc_hwpartition, "", ""),
#ifndef CONFIG_NVT_IVOT_SOC
	U_BOOT_CMD_MKENT(erase1, 3, 0, do_mmc_write1erase, "", ""),
	U_BOOT_CMD_MKENT(erase0, 3, 0, do_mmc_write0erase, "", ""),
	U_BOOT_CMD_MKENT(erase1all, 1, 0, do_mmc_write1all, "", ""),
	U_BOOT_CMD_MKENT(erase0all, 1, 0, do_mmc_write0all, "", ""),
#endif
#if defined(CONFIG_EMMC_WRITE_RELIABILITY_TEST)
	U_BOOT_CMD_MKENT(erase0boot, 1, 0, do_mmc_write0boot, "", ""),
#endif
#ifndef CONFIG_NVT_IVOT_SOC
	U_BOOT_CMD_MKENT(eraseall, 1, 0,do_emmc_eraseall, "", ""),
	U_BOOT_CMD_MKENT(en_egroup, 1, 0,do_emmc_enable_egroup, "", ""),
	U_BOOT_CMD_MKENT(dis_egroup, 1, 0,do_emmc_disable_egroup, "", ""),
#endif
#ifdef CONFIG_SUPPORT_EMMC_BOOT
	U_BOOT_CMD_MKENT(bootbus, 5, 0, do_mmc_bootbus, "", ""),
	U_BOOT_CMD_MKENT(bootpart-resize, 4, 0, do_mmc_boot_resize, "", ""),
	U_BOOT_CMD_MKENT(partconf, 5, 0, do_mmc_partconf, "", ""),
	U_BOOT_CMD_MKENT(rst-function, 3, 0, do_mmc_rst_func, "", ""),
#endif
#ifdef CONFIG_SUPPORT_EMMC_RPMB
	U_BOOT_CMD_MKENT(rpmb, CONFIG_SYS_MAXARGS, 1, do_mmcrpmb, "", ""),
#endif
	U_BOOT_CMD_MKENT(setdsr, 2, 0, do_mmc_setdsr, "", ""),
#if defined(CONFIG_NVT_MMC_INFO)
	U_BOOT_CMD_MKENT(ext, 2, 0, do_mmc_ext_info, "", ""),
#endif
};

static int do_mmcops(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	cmd_tbl_t *cp;

	cp = find_cmd_tbl(argv[1], cmd_mmc, ARRAY_SIZE(cmd_mmc));

	/* Drop the mmc command */
	argc--;
	argv++;

	if (cp == NULL || argc > cp->maxargs)
		return CMD_RET_USAGE;
	if (flag == CMD_FLAG_REPEAT && !cp->repeatable)
		return CMD_RET_SUCCESS;

	if (curr_device < 0) {
		if (get_mmc_num() > 0) {
			curr_device = 0;
		} else {
			puts("No MMC device available\n");
			return CMD_RET_FAILURE;
		}
	}
	return cp->cmd(cmdtp, flag, argc, argv);
}

U_BOOT_CMD(
	mmc, 29, 1, do_mmcops,
	"MMC sub system",
	"info - display info of the current MMC device\n"
	"mmc read addr blk# cnt\n"
	"mmc write addr blk# cnt\n"
	"mmc erase blk# cnt\n"
	"mmc rescan\n"
	"mmc part - lists available partition on current mmc device\n"
	"mmc dev [dev] [part] - show or set current mmc device [partition]\n"
	"mmc list - lists available devices\n"
#ifndef CONFIG_NVT_IVOT_SOC
	"mmc erase1  blk# cnt\n"
	"mmc erase0  blk# cnt\n"
	"mmc erase1all\n"
	"mmc erase0all\n"
#endif
#if defined(CONFIG_EMMC_WRITE_RELIABILITY_TEST)
	"mmc erase0boot\n"
#endif
	"mmc eraseall\n"
	"mmc en_egroup\n"
	"mmc dis_egroup\n"
	"mmc hwpartition [args...] - does hardware partitioning\n"
	"  arguments (sizes in 512-byte blocks):\n"
	"    [user [enh start cnt] [wrrel {on|off}]] - sets user data area attributes\n"
	"    [gp1|gp2|gp3|gp4 cnt [enh] [wrrel {on|off}]] - general purpose partition\n"
	"    [check|set|complete] - mode, complete set partitioning completed\n"
	"  WARNING: Partitioning is a write-once setting once it is set to complete.\n"
	"  Power cycling is required to initialize partitions after set to complete.\n"
#ifdef CONFIG_SUPPORT_EMMC_BOOT
	"mmc bootbus dev boot_bus_width reset_boot_bus_width boot_mode\n"
	" - Set the BOOT_BUS_WIDTH field of the specified device\n"
	"mmc bootpart-resize <dev> <boot part size MB> <RPMB part size MB>\n"
	" - Change sizes of boot and RPMB partitions of specified device\n"
	"mmc partconf dev boot_ack boot_partition partition_access\n"
	" - Change the bits of the PARTITION_CONFIG field of the specified device\n"
	"mmc rst-function dev value\n"
	" - Change the RST_n_FUNCTION field of the specified device\n"
	"   WARNING: This is a write-once field and 0 / 1 / 2 are the only valid values.\n"
#endif
#ifdef CONFIG_SUPPORT_EMMC_RPMB
	"mmc rpmb read addr blk# cnt [address of auth-key] - block size is 256 bytes\n"
	"mmc rpmb write addr blk# cnt <address of auth-key> - block size is 256 bytes\n"
	"mmc rpmb key <address of auth-key> - program the RPMB authentication key.\n"
	"mmc rpmb counter - read the value of the write counter\n"
#endif
	"mmc setdsr <value> - set DSR register value\n"
#if defined(CONFIG_NVT_MMC_INFO)
	"mmc ext - display all extcsd info command\n"
#endif
	);

/* Old command kept for compatibility. Same as 'mmc info' */
U_BOOT_CMD(
	mmcinfo, 1, 0, do_mmcinfo,
	"display MMC info",
	"- display info of the current MMC device"
);
