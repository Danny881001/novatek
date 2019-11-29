#include "include/nt72_nand.h"

#ifdef EXTRA_CHECK_CACHE_ALIGNMENT
void check_cache_alignment(struct nt72_nand_info *info, const char *who,
			   unsigned char *addr)
{
	if ((u32)addr % info->nand_params.cache_line_size)
		nfc_err("nfc_err: %s is not aligned to cache\n", who);
}
#endif

#ifdef ENABLE_CMD_HIST
struct nt72_nand_cmd_hist cmd_hist;
#endif

#ifdef ENABLE_CMD_HIST
static u64 nt72_nand_cmd_hist_get_timestamp_diff(int i)
{
	u32 start_s = cmd_hist.records[i].start_s;
	u32 start_us = cmd_hist.records[i].start_us;
	u32 end_s = cmd_hist.records[i].end_s;
	u32 end_us = cmd_hist.records[i].end_us;

	if (likely(start_s == end_s))
		return (u64)end_us - start_us;

	return (u64)((u64)end_s * 1000000 + end_us) - ((u64)start_s * 1000000 +
						       start_us);
}
#endif

#ifdef ENABLE_CMD_HIST
void nt72_nand_cmd_hist_init(void)
{
	int i;

	memset(&cmd_hist, 0, sizeof(cmd_history));
	for (i = 0; i < cmd_hist_SIZE; i++)
		strcpy(cmd_hist.records[i].command, "xxx");
	cmd_hist.head = 0;
	cmd_hist.tail = CMD_HISTORY_SIZE - 1;
}
#endif

#ifdef ENABLE_CMD_HIST
void nt72_nand_cmd_hist_add(const char *cmd)
{
	cmd_hist.head++;
	if (cmd_hist.head == CMD_HISTORY_SIZE)
		cmd_hist.head = 0;
	cmd_hist.tail++;
	if (cmd_hist.tail == CMD_HISTORY_SIZE)
		cmd_hist.tail = 0;
	strncpy(cmd_hist.records[cmd_history.tail].command, cmd,
		sizeof(cmd_hist.records[cmd_history.tail].command));
	cmd_hist.records[cmd_history.tail].data_valid = 0;
}
#endif

#ifdef ENABLE_CMD_HIST
void nt72_nand_cmd_hist_add_attr(int ecc, int oob, int inv, int retry)
{
	if (ecc) {
		strncat(cmd_hist.records[cmd_history.tail].command, "e",
			sizeof(cmd_hist.records[cmd_history.tail].command));
	}
	if (oob) {
		strncat(cmd_hist.records[cmd_history.tail].command, "o",
			sizeof(cmd_hist.records[cmd_history.tail].command));
	}
	if (inv) {
		strncat(cmd_hist.records[cmd_history.tail].command, "i",
			sizeof(cmd_hist.records[cmd_history.tail].command));
	}
	if (retry) {
		strncat(cmd_hist.records[cmd_history.tail].command, "r",
			sizeof(cmd_hist.records[cmd_history.tail].command));
	}
}
#endif

#ifdef ENABLE_CMD_HIST
void nt72_nand_cmd_hist_save_start_timestamp(void)
{
	nt72_nand_get_timestamp(
		&(cmd_hist.records[cmd_history.tail].start_s),
		&(cmd_hist.records[cmd_history.tail].start_us));
}
#endif

#ifdef ENABLE_CMD_HIST
void nt72_nand_cmd_hist_save_end_timestamp(void)
{
	nt72_nand_get_timestamp(
		&(cmd_hist.records[cmd_history.tail].end_s),
		&(cmd_hist.records[cmd_history.tail].end_us));
}
#endif

#ifdef ENABLE_CMD_HIST
void nt72_nand_cmd_hist_add_data(u32 data)
{
	cmd_hist.records[cmd_history.tail].data = data;
	cmd_hist.records[cmd_history.tail].data_valid = 1;
}
#endif

#ifdef ENABLE_CMD_HIST
void nt72_nand_cmd_hist_dump(void)
{
	int i;

	nfc_err("nfc_err: start dump cmd history\n");
	for (i = cmd_hist.head;; i++) {
		if (i == cmd_hist_SIZE)
			i = 0;
		if (cmd_hist.records[i].data_valid) {
			nfc_err("command: %s\t%llu us\t0x%02X\n",
				cmd_hist.records[i].command,
				nt72_nand_cmd_hist_get_timestamp_diff(i),
				cmd_hist.records[i].data);
		} else {
			nfc_err("command: %s\t%llu us\n",
				cmd_hist.records[i].command,
				nt72_nand_cmd_hist_get_timestamp_diff(i));
		}
		if (i == cmd_hist.tail)
			break;
	}
	nfc_err("nfc_err: dump cmd history end\n");
}
#endif

#ifdef ENABLE_BUS_SNIFFER
struct nt72_nand_bus_sniffer sniffer;
#endif

#ifdef ENABLE_BUS_SNIFFER
void nt72_nand_bus_sniffer_init(void)
{
	int i;

	memset(&sniffer, 0, sizeof(sniffer));
	for (i = 0; i < BUS_SNIFFER_SIZE; i++) {
		strcpy(sniffer.records[i].op, "xxx");
		sniffer.records[i].addr = 0;
		sniffer.records[i].val = 0;
	}
	sniffer.head = 0;
	sniffer.tail = BUS_SNIFFER_SIZE - 1;
}
#endif

#ifdef ENABLE_BUS_SNIFFER
void nt72_nand_bus_sniffer_add(const char *op, u32 addr, u32 val)
{
	sniffer.head++;
	if (sniffer.head == BUS_SNIFFER_SIZE)
		sniffer.head = 0;
	sniffer.tail++;
	if (sniffer.tail == BUS_SNIFFER_SIZE)
		sniffer.tail = 0;
	strncpy(sniffer.records[sniffer.tail].op, op,
		sizeof(sniffer.records[sniffer.tail].op));
	sniffer.records[sniffer.tail].addr = (u16)addr;
	sniffer.records[sniffer.tail].val = val;
}
#endif

#ifdef ENABLE_BUS_SNIFFER
void nt72_nand_bus_sniffer_dump(void)
{
	int i;

	nfc_err("nfc: start dump bus sniffer\n");
	for (i = sniffer.head;; i++) {
		if (i == BUS_SNIFFER_SIZE)
			i = 0;
		pr_cont("%s:%04X=%08X ",
			sniffer.records[i].op,
			sniffer.records[i].addr,
			sniffer.records[i].val);
		if (i == sniffer.tail)
			break;
	}
	nfc_err("nfc: dump bus sniffer end\n");
}
#endif
