#include <common.h>
#include <command.h>
#include <nand.h>
#include <nvt_nand_partition.h>
#include <crypto_core.h>
#include <nvt_stbc.h>
#include <cli_hush.h>
#if defined(CONFIG_NVT_SCRIPT_UPDATE_BLINK_LED)
#include "n330xx_led_stbc.h"
#endif

static int do_nvt_nwriteloader_cmd(cmd_tbl_t * cmdtp, int flag, int argc, char * const argv[])
{
	char cmd_buf[256] = {0};
	u32 page_size;
	u32 write_size;
	u32 fst_write_size;
	u32 bad_size;
	int cmd_ret;
	struct mtd_info *info = nand_info[nand_curr_device];
	u_char loader_type;
	size_t ooblen = 1;
	u32 fstblk_wlen;//first block write length
	loff_t offset;
	size_t        Actual;
	int ptn_idx = -1;
	unsigned long write_off, buffer;

	if (argc != 4) {
		return cmd_usage(cmdtp);
	}

	page_size = nand_get_page_size();

	buffer = (ulong)simple_strtoul(argv[1], NULL, 16);
	write_off = (ulong)simple_strtoul(argv[2], NULL, 16);
	write_size = (ulong)simple_strtoul(argv[3], NULL, 16);
	if((write_size % page_size))
		write_size = (write_size & ~(page_size - 1)) + page_size;
	fstblk_wlen = info->erasesize;

	//erase nand partition
	sprintf(cmd_buf,"nand erase 0x%x 0x%x",write_off
			,write_size);
	cmd_ret = run_command(cmd_buf, 0);
	if(cmd_ret == -1) {
		printf("erase nand partition fail !\n");
		goto err_out;
	}

	/* Rom code read xboot magic in oob first 3 bytes, But first 2 bytes
	 * be used for bad block mark. So we just need set one byte.
	 * And nand_write_skip_bad_oob change to using MTD_OPS_AUTO_OOB mode.
	 * We dont care oob layout.
	 */
	loader_type = 0x01;
	
	//write image to nand partition
	//write fist block
	//	printf(" 1. write par name=%d, addr = %x \n", ptn_idx,write_off);
	//	printf(" 1. fstblk_wlen=%x, write_size=%x\n", fstblk_wlen,write_size);
	fst_write_size = (write_size > fstblk_wlen) ? fstblk_wlen:write_size;
	cmd_ret = nand_write_skip_bad_oob(info, write_off, &fst_write_size, (u_char*)(buffer), &ooblen, &loader_type, 0);


	if(cmd_ret) {
		printf("write loader image to nand first block fail !\n");
		goto err_out;
	}

	//check bad block size for first block
	offset = write_off;
	bad_size = 0;
	while(nand_block_isbad(info, offset)) {
		bad_size += info->erasesize;
		offset += info->erasesize;
	}

	//write remains
	if(write_size > fstblk_wlen){
		write_size -= fstblk_wlen;
		//	printf(" 2. write par name=%d, addr = %x\n", ptn_idx,write_off);
		//	printf(" 2. fstblk_wlen=%x, write_size=%x\n", fstblk_wlen,write_size);

		cmd_ret = nand_write_skip_bad(info, write_off + bad_size + info->erasesize, &write_size, &Actual, write_size, (u_char*)(buffer + fstblk_wlen), 0);

		if(cmd_ret) {
			printf("write loader image remain parts nand first block fail !\n");
			goto err_out;
		}
	}
	printf("Write loader to nand success!\n");

	return 0;

err_out:
	return -1;
}

U_BOOT_CMD(
        nwriteloader, 4,      0,      do_nvt_nwriteloader_cmd,
        "write loader image to nand flash",
        "<buffer> <offset> <length>"
);

static int do_aesecb(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	unsigned char  __attribute__((__aligned__(CONFIG_SYS_CACHELINE_SIZE))) key[16] = {0xa9, 0xa0, 0x5f, 0xc6, 0xf9, 0x86, 0x25, 0x43, 0x82, 0xdf, 0x2c, 0x93, 0xde, 0xdf, 0x7b, 0x98};
	ulong src, dst, size, enc;
	ulong cache_align = 0;
	int ret;

	if (argc < 4) {
		return cmd_usage(cmdtp);
	}

	src = simple_strtoul(argv[1], NULL, 16);
	dst = simple_strtoul(argv[2], NULL, 16);
	size = simple_strtoul(argv[3], NULL, 16);
	enc = (argc == 5);

	CryptoCoreInit();
	cache_align = ((size + CONFIG_SYS_CACHELINE_SIZE - 1) / CONFIG_SYS_CACHELINE_SIZE ) * CONFIG_SYS_CACHELINE_SIZE;
	memset((void *)dst, 0 , cache_align);

	flush_dcache_range((unsigned long)src, (unsigned long)src + cache_align);
	if (argc == 5) {
		ret = AES_ecb_enc((unsigned char *)src, (unsigned char *)dst, (unsigned int) size, (unsigned char *)key);
	}else {
		ret = AES_ecb_dec((unsigned char *)src, (unsigned char *)dst, (unsigned int) size, (unsigned char *)key);
	}
        invalidate_dcache_range((unsigned long)dst, (unsigned long)dst + cache_align);

	if (ret != CRYPTO_OK) {
		printf("ret = %d\n", ret);
	}

	return 0;
}

U_BOOT_CMD(
        aesecb, 5,      0,      do_aesecb,
        "AES ECB decrypt",
        "[src] [dst] [size] [enc]\n"
);

static int do_sha256_chk(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	ulong src;
	ulong size = 0;
	char *s;
	char *dst[SHA256_DIGEST_SIZE] __attribute__((__aligned__(CONFIG_SYS_CACHELINE_SIZE)));
	char *sha_tmp[SHA256_DIGEST_SIZE] __attribute__((__aligned__(CONFIG_SYS_CACHELINE_SIZE)));


	if (argc < 2)
	{
		return cmd_usage(cmdtp);
	}
	src = simple_strtoul(argv[1], NULL, 16);
	s = getenv("filesize");
	if (s != NULL)
	{
		size = simple_strtoul(s, NULL, 16);
	}
	else
	{
		// Can't find size
		return 1;
	}

	// Backup SHA2 value at end of file, to prevent from destory while padding
	if (size > SHA256_DIGEST_SIZE)
		memcpy((unsigned long*)(sha_tmp), (unsigned long*)(src + size - SHA256_DIGEST_SIZE), SHA256_DIGEST_SIZE);

	// Do SHA2 calculate
	if (size > SHA256_DIGEST_SIZE)
		MDrv_SHA256_Calculate( (unsigned long ) src, (unsigned long )(size - SHA256_DIGEST_SIZE), (unsigned long *)dst );

	// compare SHA2 value
	if (memcmp((unsigned long*)dst,(unsigned long*)sha_tmp, SHA256_DIGEST_SIZE) != 0)
	{
		//printf("Check Fail\n");
		setenv("sha256","1");
		return 1;
	}
	else
	{
		//printf("Check OK\n");
		setenv("sha256","2");
		return 0;
	}
}

U_BOOT_CMD(
        sha256_chk,     2,      0,      do_sha256_chk,
        "SHA256 hash check",
        "[File src]\n"
);

#ifdef CONFIG_CMD_NVT_SCRIPT_UPDATE

#ifdef CONFIG_SYS_HUSH_PARSER
#define FLAG_PARSE_SEMICOLON (1 << 1)               /* symbol ';' is special for parser */
#endif
extern char *source_cmd;

static int do_nvt_source(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	ulong addr;
	int rcode = CMD_RET_SUCCESS, i;
	char *cmd, *buf, delim[] = {0x0A, 0x0D};

	if (argc < 2) {
		addr = CONFIG_NVT_SCRIPT_UPDATE_BUF;
	} else {
		if (strcmp(argv[1], "help") == 0)
		{
			return CMD_RET_USAGE;
		}

		addr = simple_strtoul(argv[1], NULL, 16);
	}

	if ((buf = (char*)malloc(CONFIG_NVT_SCRIPT_UPDATE_SIZE)) == NULL) {
		return CMD_RET_FAILURE;
	}

	printf ("## Executing auto script at %08lx\n", addr);
	memcpy(buf, (char*)addr, CONFIG_NVT_SCRIPT_UPDATE_SIZE);

	for (i = 0; i < CONFIG_NVT_SCRIPT_UPDATE_SIZE; i++) {
		if (buf[i] == '%')
			break;
	}

	if (i == CONFIG_NVT_SCRIPT_UPDATE_SIZE) {
		printf("incorrect script format! place %% at the end of auto script\n");
		rcode = CMD_RET_FAILURE;
		goto err;
	}

	nvt_stbc_stay_in_sram();
#if defined(CONFIG_NVT_SCRIPT_UPDATE_BLINK_LED)
	nvt_stbc_led_init();
#endif

	cmd = buf;
	strtok(cmd, (const char *)&delim);
	while (cmd) {
		if (*cmd == '%')
			break;
		if (*cmd != '#') {
			printf("[%s]\n", cmd);

#if defined(CONFIG_NVT_SCRIPT_UPDATE_BLINK_LED)
			nvt_stbc_led_ctl(EN_LED_BLINK, 5);
#endif

#ifdef CONFIG_SYS_HUSH_PARSER
			rcode += parse_string_outer(cmd, FLAG_PARSE_SEMICOLON);
#else
			rcode += run_command(cmd, 0);
#endif
		}
		cmd = strtok(NULL, (const char *)&delim);
	}

err:
	free(buf);

#if defined(CONFIG_NVT_SCRIPT_UPDATE_BLINK_LED)
	nvt_stbc_led_ctl(EN_LED_OFF, 0);
#endif

	return rcode;
}

static int do_nvt_script_update(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	int rcode;
	char cmd[80];

	if (argc < 2) {
		sprintf(cmd, "%s 0x%x %s",source_cmd, CONFIG_NVT_SCRIPT_UPDATE_BUF, CONFIG_NVT_SCRIPT_UPDATE_SCRIPT_NAME);
	} else {
		if (strcmp(argv[1], "help") == 0)
		{
			return CMD_RET_USAGE;
		}
		sprintf(cmd, "%s 0x%x %s",source_cmd, CONFIG_NVT_SCRIPT_UPDATE_BUF, argv[1]);
	}

	if ((rcode = run_command(cmd, 0))) {
		return rcode;
	}

	printf("\033[1;31m USB AUTO UPDATE!! \033[0m\n");
	rcode = run_command("nvtsource", 0);
	if (rcode != 0)
	{
		printf("Warning!! At least one command in the script file failed! Please check the log...\n");
#ifdef CONFIG_NVT_SCRIPT_UPDATE_SKIP_CHECK
		return 0;
#endif
	}

	return rcode;
}

U_BOOT_CMD(
	nvtsource,	2,	0,	do_nvt_source,
	"run script from memory",
	"[addr in hex]\n"
	"\t- run script starting at addr\n"
);

U_BOOT_CMD(
	nvt_script_update,	3,	0,	do_nvt_script_update,
	"upgrade firmware according to auto script",
	"[file]\n"
);

#endif /* CONFIG_CMD_NVT_SCRIPT_UPDATE */
