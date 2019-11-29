#ifndef _NT72_NAND_KAPI_H_
#define _NT72_NAND_KAPI_H_

#include "nt72_nand.h"

#ifdef IS_CVT
struct pcg32_random_t {
	u64 state;
	u64 inc;
};
#endif

#ifdef IS_CVT
void nt72_nand_rand_reset(void);
#endif

u8 nt72_nand_rand8(void);
u32 nt72_nand_rand32(void);

void nt72_nand_get_timestamp(u32 *s, u32 *us);

void nt72_nand_cpu_relax(void);

unsigned long nt72_nand_timeout_init(void);
int nt72_nand_timeout_check(unsigned long stop_time);

#ifndef IS_KERNEL
#define nt72_nand_lock_nfc(...) do { } while (0)
#else
void nt72_nand_lock_nfc(struct nt72_nand_info *info);
#endif

#ifndef IS_KERNEL
#define nt72_nand_unlock_nfc(...) do { } while (0)
#else
void nt72_nand_unlock_nfc(struct nt72_nand_info *info);
#endif

u32 nt72_nand_hweight(u32 x);

void nt72_nand_init_comp(struct completion *comp);
int nt72_nand_wait_for_comp(struct completion *comp, int timeout);
void nt72_nand_comp(struct completion *comp);

#endif
