#include <nvt_npt.h>
#include <linux/crc32.h>
#include <command.h>

#define npt_dbg(args...) //printf(args)
#define npt_msg(args...) printf(args)
#define npt_err(args...) printf(args)

struct nt72_npt_info g_npt __aligned(4096);
uint32_t npt_store_blk_offset = 0;
uint32_t npt_store_blk_nr = 0;

uint32_t nt72_npt_get_store_blk_offset()
{
	if (npt_store_blk_offset == 0) {
		return NVT_NPT_STORE_DEFAULT_BLK_OFF;
	}
	return npt_store_blk_offset;
}

uint32_t nt72_npt_get_store_blk_nr()
{
	if (npt_store_blk_nr == 0) {
		return NVT_NPT_STORE_DEFAULT_BLK_NR;
	}
	return npt_store_blk_nr;
}

int atoi(const char *str)
{
	return (int)simple_strtoul(str, '\0', 10);
}

int nt72_npt_if_load(struct nt72_npt_info *p, const char *csv_file)
{
	extern char *source_cmd;
	int ret;
	char cmd_buf[256];

	memset(p, 0, sizeof(struct nt72_npt_info));
	memset((unsigned char *)CONFIG_FWUP_RAM_START, 0, 4096);

	/* load partition CSV file to DRAM */
	sprintf(cmd_buf, "%s 0x%x %s", source_cmd, CONFIG_FWUP_RAM_START, csv_file);
	if (run_command(cmd_buf, 0)) {
		npt_err("npt_err: error loading %s\n", csv_file);
		goto has_error;
	} else if (simple_strtoul(getenv("filesize"), NULL, 16) >= 4096) {
		npt_err("%s is too large!!\n", csv_file);
		goto has_error;		
	}

	memcpy(p->pt_buf, (unsigned char *)CONFIG_FWUP_RAM_START, simple_strtoul(getenv("filesize"), NULL, 16));

	ret = nt72_npt_parse(p);
	if (ret) {
		npt_err("npt_err: failed to load partition\n");
		goto has_error;
	}

	return 0;
has_error:
	return -1;
}

unsigned int nt72_npt_if_block_sz(void)
{
	return 128 * 1024;
}

unsigned int nt72_npt_if_chip_id(void)
{
	return 72171;
}

unsigned int nt72_npt_if_nand_id(void)
{
	return 0x01020304;
}

unsigned int nt72_npt_if_nand_sz(void)
{
	return 2048;
}

int nt72_npt_match_table(struct nt72_npt_info *p)
{
	int chip_id = !p->chip_id || (p->chip_id == nt72_npt_if_chip_id());
	int nand_id = !p->nand_id || (p->nand_id == nt72_npt_if_nand_id());
	int nand_sz = !p->nand_sz || (p->nand_sz == nt72_npt_if_nand_sz());

	return (chip_id && nand_id && nand_sz) ? 1 : 0;
}

void nt72_npt_parse_csv_field(char *next_field, char *f0, char *f1, char *f2,
			      char *f3, char *f4, char *f5)
{
	char *cur_field;

	/* our field end with ','
	 * after strsep, ',' will be replaced by '\0'
	 */

	cur_field = strsep(&next_field, ",");
	if (*cur_field == 0) *f0 = '\0';
	else strcpy(f0, cur_field);

	cur_field = strsep(&next_field, ",");
	if (*cur_field == 0) *f1 = '\0';
	else strcpy(f1, cur_field);

	cur_field = strsep(&next_field, ",");
	if (*cur_field == 0) *f2 = '\0';
	else strcpy(f2, cur_field);

	cur_field = strsep(&next_field, ",");
	if (*cur_field == 0) *f3 = '\0';
	else strcpy(f3, cur_field);

	cur_field = strsep(&next_field, ",");
	if (*cur_field == 0) *f4 = '\0';
	else strcpy(f4, cur_field);

	cur_field = strsep(&next_field, ",");
	if (*cur_field == 0) *f5 = '\0';
	else strcpy(f5, cur_field);
}

void nt72_npt_parse_csv_line(char **next_line, char *f0, char *f1, char *f2,
			     char *f3, char *f4, char *f5)
{
	char *cur_line;

	/* our line end with \r\n
	 * after strsep, \r will be replaced by \0
	 */
	cur_line = strsep(next_line, "\r");
	if (cur_line == NULL || next_line == NULL)
		return;

	nt72_npt_parse_csv_field(cur_line, f0, f1, f2, f3, f4, f5);

	/* now remove the remaining '\n' */
	strsep(next_line, "\n");
}

int nt72_npt_parse(struct nt72_npt_info *p)
{
	int i = 0;
	char *next_line = NULL;
	char f[6][128];
	int ubi_vol_table = 0;

	next_line = (char *)(p->pt_buf);
	//line 1: NT72_PART_TABLE_START
	nt72_npt_parse_csv_line(&next_line, f[0], f[1], f[2], f[3], f[4], f[5]);
	if (strcmp(f[0], "NT72_PART_TABLE_START")) {
		npt_err("npt_err: error parsing start token\n");
		goto parse_error;
	}
parse_header:
	//line 2: VERSION
	nt72_npt_parse_csv_line(&next_line, f[0], f[1], f[2], f[3], f[4], f[5]);
	if (strcmp(f[0], "VERSION")) {
		npt_err("npt_err: error parsing version\n");
		goto parse_error;
	}
	p->version = atoi(f[1]);
	if (p->version != 1) {
		npt_err("npt_err: un-supported version\n");
		goto parse_error;
	}
	npt_dbg("version: %u\n", p->version);

	//line 3: NAME
	nt72_npt_parse_csv_line(&next_line, f[0], f[1], f[2], f[3], f[4], f[5]);
	if (strcmp(f[0], "NAME")) {
		npt_err("npt_err: error parsing name\n");
		goto parse_error;
	}
	strncpy(p->name, f[1], sizeof(p->name) - 1);
	npt_dbg("name: %s\n", p->name);

	//line 4: CHIP ID
	nt72_npt_parse_csv_line(&next_line, f[0], f[1], f[2], f[3], f[4], f[5]);
	if (strcmp(f[0], "CHIP")) {
		npt_err("npt_err: error parsing chip id\n");
		goto parse_error;
	}
	p->chip_id = atoi(f[1]);
	npt_dbg("chip id: %u\n", p->chip_id);

	//line 5: NAND ID
	nt72_npt_parse_csv_line(&next_line, f[0], f[1], f[2], f[3], f[4], f[5]);
	if (strcmp(f[0], "NAND")) {
		npt_err("npt_err: error parsing nand id\n");
		goto parse_error;
	}
	p->nand_id = atoi(f[1]);
	npt_dbg("nand id: %u\n", p->nand_id);

	//line 6: NAND SIZE
	nt72_npt_parse_csv_line(&next_line, f[0], f[1], f[2], f[3], f[4], f[5]);
	if (strcmp(f[0], "SIZE")) {
		npt_err("npt_err: error parsing nand size\n");
		goto parse_error;
	}
	p->nand_sz = atoi(f[1]);
	npt_dbg("nand size: %u\n", p->nand_sz);

	//line 7: UNIT SIZE
	nt72_npt_parse_csv_line(&next_line, f[0], f[1], f[2], f[3], f[4], f[5]);
	if (strcmp(f[0], "UNIT_KB")) {
		npt_err("npt_err: error parsing unit_kb\n");
		goto parse_error;
	}
	p->unit_kb = atoi(f[1]);
	if (!p->unit_kb || (p->unit_kb & (p->unit_kb - 1))) {
		npt_err("unit size (KB): %u is not power of 2!!\n", p->unit_kb);
		goto parse_error;
	}
	npt_dbg("unit size (KB): %u\n", p->unit_kb);

	//line 7-1: UNIT VOL SIZE
	nt72_npt_parse_csv_line(&next_line, f[0], f[1], f[2], f[3], f[4], f[5]);
	if (strcmp(f[0], "UNIT_VOL_KB")) {
		npt_err("npt_err: error parsing unit_vol_kb\n");
		goto parse_error;
	}
	p->unit_vol_kb = atoi(f[1]);
	if (!p->unit_vol_kb) {
		npt_err("invalid unit vol size (KB): %u\n", p->unit_vol_kb);
		goto parse_error;
	}
	npt_dbg("unit vol size (KB): %u\n", p->unit_vol_kb);

	//line 8: table title
	nt72_npt_parse_csv_line(&next_line, f[0], f[1], f[2], f[3], f[4], f[5]);
	if (strcmp(f[0], "ID") && strcmp(f[1], "NAME") && strcmp(f[2], "SIZE")) {
		npt_err("npt_err: error parsing table title\n");
		goto parse_error;
	}

	//line 9~: table rows
	for (i = 0; i < MAX_NR_PART; i++) {
		nt72_npt_parse_csv_line(&next_line, f[0], f[1], f[2], f[3], f[4], f[5]);
		if (!strcmp(f[0], "NT72_PART_TABLE_NEXT")) {
			if (nt72_npt_match_table(p))
				break;
			goto parse_header;
		} else if (!strcmp(f[0], "NT72_PART_UBI_VOL_TABLE")) {
			ubi_vol_table = 1;
			break;
		} else if (!strcmp(f[0], "NT72_PART_TABLE_END")) {
			if (nt72_npt_match_table(p))
				break;
			goto no_match;
		}

		if (i != atoi(f[0])) {
			npt_err("npt_err: error reading part id\n");
			goto parse_error;
		}
		p->part_cnt = i + 1;

		strncpy(p->part[i].name, f[1], sizeof(p->part[i].name) - 1);
		if (strlen(p->part[i].name) == 0) {
			npt_err("npt_err: error reading part name\n");
			goto parse_error;
		} else if (strlen(f[1]) >= sizeof(p->part[i].name)) {
			npt_err("npt_err: part name:%s is too long, max %d byte!\n", f[1], sizeof(p->part[i].name)-1);
			goto parse_error;
		}

		p->part[i].size = atoi(f[2]) * p->unit_kb * 1024;
		if (p->part[i].size <= 0) {
			npt_err("npt_err: error reading part size\n");
			goto parse_error;
		}

		if (p->part[i].size & ((p->unit_kb * 1024) - 1)) {
			npt_err("npt_err: part size not aligned\n");
			goto parse_error;
		}

		strncpy(p->img_name[i], f[5], sizeof(p->img_name[i]) - 1);

		npt_dbg("%2d %016s\t0x%016llx\t%s\n", i, p->part[i].name, p->part[i].size,
					p->img_name[i]);
	}

	/* Start to parser ubi vol */
	if (ubi_vol_table == 1) {
		nt72_npt_parse_csv_line(&next_line, f[0], f[1], f[2], f[3], f[4], f[5]);
		if (strcmp(f[0], "MTD_IDX") && strcmp(f[1], "NAME") && strcmp(f[2], "SIZE")) {
			npt_err("npt_err: error parsing vol table title\n");
			goto parse_error;
		}

		for (i = 0; i < MAX_NR_PART; i++) {
			nt72_npt_parse_csv_line(&next_line, f[0], f[1], f[2], f[3], f[4], f[5]);
			if (!strcmp(f[0], "NT72_PART_TABLE_NEXT")) {
				if (nt72_npt_match_table(p))
					break;
				goto parse_header;
			} else if (!strcmp(f[0], "NT72_PART_TABLE_END")) {
				if (nt72_npt_match_table(p))
					break;
				goto no_match;
			}

			p->vol_cnt = i + 1;
			p->vol[i].mtd_idx = atoi(f[0]);
			if (p->vol[i].mtd_idx < 0 || p->vol[i].mtd_idx > p->part_cnt - 1) {
				npt_err("npt_err: invalid mtd_idx: %d, out of %d~%d\n", p->vol[i].mtd_idx, 0, p->part_cnt - 1);
				goto parse_error;
			}

			strncpy(p->vol[i].name, f[1], sizeof(p->vol[i].name) - 1);
			if (strlen(p->vol[i].name) == 0) {
				npt_err("npt_err: error reading vol name\n");
				goto parse_error;
			} else if (strlen(f[1]) >= sizeof(p->vol[i].name)) {
				npt_err("npt_err: vol name:%s is too long, max %d byte!\n", f[1], sizeof(p->vol[i].name)-1);
				goto parse_error;
			}

			p->vol[i].size = atoi(f[2]) * p->unit_vol_kb * 1024;
			if (p->vol[i].size <= 0) {
				npt_err("npt_err: error reading vol size\n");
				goto parse_error;
			}

			p->vol[i].vol_idx = atoi(f[3]);
			if (p->vol[i].vol_idx < 0 || p->vol[i].vol_idx > 254) {
				npt_err("npt_err: invalid vol_idx: %d, out of 0~254\n", p->vol[i].vol_idx);
				goto parse_error;
			}

			npt_dbg("vol: mtd_idx=%d, name=%s, size=%016llx, vol_idx=%d\n", p->vol[i].mtd_idx, p->vol[i].name, p->vol[i].size, p->vol[i].vol_idx);
		}
	}

	npt_dbg("partition count: %d, vol count: %d\n", p->part_cnt, p->vol_cnt);

	return 0;

parse_error:
	npt_err("npt_err: error partition: %s %s %s %s\n", f[0], f[1], f[2], f[5]);
	return -1;
no_match:
	npt_err("npt_err: no matching partition table\n");
	return -1;
}


int nt72_npt_gen_mtd_parts(struct nt72_npt_info *p, struct mtd_partition *mtd_parts)
{
	int i;

	npt_msg("npt: found %d mtd partitions\n", p->part_cnt);
	memset(mtd_parts, 0, sizeof(struct mtd_partition) * p->part_cnt);

	for (i = 0; i < p->part_cnt; i++) {
		mtd_parts[i].name = p->part[i].name;
		mtd_parts[i].offset = MTDPART_OFS_APPEND;
		mtd_parts[i].size = p->part[i].size;
	}
	mtd_parts[0].offset = 0;

	return 0;
}

int nt72_npt_gen_npt(struct nt72_npt_info *npt_info, struct nand_partition_table *npt)
{
	int i;
	uint64_t offset = 0;

	memset(npt, 0, sizeof(struct nand_partition_table));

	for (i = 0; i < npt_info->part_cnt; i++) {
		strncpy(npt->npt_part[i].name, npt_info->part[i].name,
			sizeof(npt->npt_part[i].name) - 1);
		npt->npt_part[i].size = npt_info->part[i].size;
		npt->npt_part[i].offset = offset;
		offset += npt->npt_part[i].size;
	}

	for (i = 0; i < npt_info->vol_cnt; i++) {
		strncpy(npt->npt_vol[i].name, npt_info->vol[i].name,
			sizeof(npt->npt_vol[i].name) - 1);
		npt->npt_vol[i].size = npt_info->vol[i].size;
		npt->npt_vol[i].mtd_idx = npt_info->vol[i].mtd_idx;
		npt->npt_vol[i].vol_idx = npt_info->vol[i].vol_idx;
	}

	npt->crc = crc32(0, (const unsigned char *)(npt->npt_part), sizeof(struct nvt_partition)* MAX_NR_PART);
	npt->crc = crc32(npt->crc, (const unsigned char *)(npt->npt_vol), sizeof(struct nvt_ubi_vol)* MAX_NR_PART);
	npt->crc_xboot = crc32(0, (const unsigned char *)(npt->npt_part), sizeof(struct nvt_partition)* MAX_NR_PART_XBOOT);

	return 0;
}

int nt72_npt_gen_mtd_parts_by_npt(struct nand_partition_table *npt, struct mtd_partition *mtd_parts)
{
	int i;

	for (i = 0; i < MAX_NR_PART; i++) {
		mtd_parts[i].name = npt->npt_part[i].name;
		mtd_parts[i].offset = npt->npt_part[i].offset;
		mtd_parts[i].size = npt->npt_part[i].size;
	}
	return 0;
}

void nt72_npt_print_mtd_parts(struct nvt_partition *mtd_parts, int count)
{
	int i;

	npt_msg("npt: print final mtd partitions:\n");
	npt_msg("id            name\t\tsize\n");
	for (i = 0; i < count; i++)
		npt_msg("%02d\t%016s\t0x%016llx\n", i, mtd_parts[i].name, mtd_parts[i].size);

}

#if 0
char g_npt_csv[][32] = {
	"partition.csv",
	"partition_ksw.csv",
};

const unsigned int g_npt_nr_csv = sizeof(g_npt_csv) / sizeof(g_npt_csv[0]);



static int do_nvt_npt(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
	int ret;
	char cmd_buf[256] = {0};

	if (argc == 2)
		npt_msg("npt start: %d %s %s\n", argc, argv[0], argv[1]);
	else {
		npt_err("npt: invalid parameters\n");
		goto has_error;
	}

	if (!strcmp(argv[1], "load")) {
		if (argc == 3) {
			npt_msg("npt: start loading partition table: %s\n", g_npt_csv[2]);
			ret = nt72_npt_if_load(&g_npt, g_npt_csv[2]);
		} else {
			ret = nt72_npt_if_load(&g_npt, "partition_evb461.csv");
			npt_msg("npt: start loading partition table: %s\n", "partition_evb461.csv");
		}

		if (ret) {
			npt_err("npt_err: npt if load failed\n");
			goto has_error;
		}
		npt_msg("npt: partition table loaded: %s\n", g_npt.name);



        	nt72_npt_gen_mtd_parts(&g_npt);
	        nt72_npt_print_mtd_parts(&g_npt);
		nvt_nand_ptn_init();
		nvt_nand_part_setenv();
	} else
		goto has_error;


	return 0;
has_error:
	npt_err("npt_err: npt command failed\n");
	return -1;
}
U_BOOT_CMD(
	npt, 2, 0, do_nvt_npt,
	"npt - NT72 NAND Partition Table",
	""
);
#endif
