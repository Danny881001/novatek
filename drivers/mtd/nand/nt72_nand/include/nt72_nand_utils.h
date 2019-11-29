#ifndef _NT72_NAND_UTILS_H_
#define _NT72_NAND_UTILS_H_

#include "nt72_nand.h"

#ifdef ENABLE_cmd_hist
struct nt72_nand_cmd_hist_record {
	char	command[8];
	int	data_valid;
	u32	data;
	u32	start_s;
	u32	start_us;
	u32	end_s;
	u32	end_us;
};
#endif

#ifdef ENABLE_cmd_hist
struct nt72_nand_cmd_hist {
	struct nt72_nand_cmd_hist_record	records[cmd_hist_SIZE];
	int					head;
	int					tail;
};
#endif

#ifdef ENABLE_BUS_SNIFFER
struct nt72_nand_bus_sniffer_record {
	char	op[8];
	u16	addr;
	u32	val;
};
#endif

#ifdef ENABLE_BUS_SNIFFER
struct nt72_nand_bus_sniffer {
	struct nt72_nand_bus_sniffer_record	records[BUS_SNIFFER_SIZE];
	int					head;
	int					tail;
};
#endif

#ifndef EXTRA_CHECK_CACHE_ALIGNMENT
#define check_cache_alignment(...) do { } while (0)
#else
void check_cache_alignment(struct nt72_nand_info *info, const char *who,
			   unsigned char *addr);
#endif

#ifndef ENABLE_cmd_hist
#define nt72_nand_cmd_hist_init(...) do { } while (0)
#else
void nt72_nand_cmd_hist_init(void);
#endif

#ifndef ENABLE_cmd_hist
#define nt72_nand_cmd_hist_add(...) do { } while (0)
#else
void nt72_nand_cmd_hist_add(const char *cmd);
#endif

#ifndef ENABLE_cmd_hist
#define nt72_nand_cmd_hist_add_attr(...) do { } while (0)
#else
void nt72_nand_cmd_hist_add_attr(int ecc, int oob, int inv, int retry);
#endif

#ifndef ENABLE_cmd_hist
#define nt72_nand_cmd_hist_save_start_timestamp(...) do { } while (0)
#else
void nt72_nand_cmd_hist_save_start_timestamp(void);
#endif

#ifndef ENABLE_cmd_hist
#define nt72_nand_cmd_hist_save_end_timestamp(...) do { } while (0)
#else
void nt72_nand_cmd_hist_save_end_timestamp(void);
#endif

#ifndef ENABLE_cmd_hist
#define nt72_nand_cmd_hist_add_data(...) do { } while (0)
#else
void nt72_nand_cmd_hist_add_data(u32 data);
#endif

#ifndef ENABLE_cmd_hist
#define nt72_nand_cmd_hist_dump(...) do { } while (0)
#else
void nt72_nand_cmd_hist_dump(void);
#endif

#ifndef ENABLE_BUS_SNIFFER
#define nt72_nand_bus_sniffer_init(...) do { } while (0)
#else
void nt72_nand_bus_sniffer_init(void);
#endif

#ifndef ENABLE_BUS_SNIFFER
#define nt72_nand_bus_sniffer_add(...) do { } while (0)
#else
void nt72_nand_bus_sniffer_add(const char *op, u32 addr, u32 val);
#endif

#ifndef ENABLE_BUS_SNIFFER
#define nt72_nand_bus_sniffer_dump(...) do { } while (0)
#else
void nt72_nand_bus_sniffer_dump(void);
#endif

void nt72_nand_utils_gen_uni_rand(u32 *rand, u32 mod, int size);
void nt72_nand_utils_flip_bit(u8 *b);
void nt72_nand_utils_gen_bitflip(int flip_count);

#endif /* _NT72_NAND_UTILS_H_ */
