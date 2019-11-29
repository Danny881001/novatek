#include "include/nt72_nand.h"

#ifdef IS_CVT
static struct pcg32_random_t rng;
#endif

#ifdef IS_CVT
void nt72_nand_rand_reset(void)
{
	rng.state = 0x1234567812345678ULL;
	rng.inc =   0x1988080219880802ULL;
}
#endif

u8 nt72_nand_rand8(void)
{
#ifdef IS_KERNEL
	u8 result;

	get_random_bytes(&result, sizeof(result));

	return result;
#else
	return (u8)nt72_nand_rand32();
#endif
}

u32 nt72_nand_rand32(void)
{
#if defined(IS_KERNEL)
	u32 result;

	get_random_bytes(&result, sizeof(result));

	return result;
#elif defined(IS_CVT)
	u64 oldstate = rng.state;
	/* Calculate output function (XSH RR), uses old state for max ILP */
	u32 xorshifted = ((oldstate >> 18u) ^ oldstate) >> 27u;
	u32 rot = oldstate >> 59u;
	/* Advance internal state */
	rng.state = oldstate * 6364136223846793005ULL + (rng.inc | 1);

	return (xorshifted >> rot) | (xorshifted << ((-rot) & 31));
#else
	return 0;
#endif
}

void nt72_nand_get_timestamp(u32 *s, u32 *us)
{
#ifdef IS_KERNEL
	struct timeval t;

	do_gettimeofday(&t);
	*s = (u32)t.tv_sec;
	*us = (u32)t.tv_usec;
#else
	*s = 0;
	*us = 0;
#endif
}

void nt72_nand_cpu_relax(void)
{
#ifdef IS_KERNEL
	cpu_relax();
#endif
}

#if defined(IS_CVT) || defined(IS_UBOOT)
static int timeout_counter_us;
#endif

unsigned long nt72_nand_timeout_init(void)
{
#if defined(IS_KERNEL)
	return jiffies + REG_POLLING_TIMEOUT;
#elif defined(IS_CVT) || defined(IS_UBOOT)
	timeout_counter_us = 0;
	return REG_POLLING_TIMEOUT;
#else
	return 0;
#endif
}

int nt72_nand_timeout_check(unsigned long stop_time)
{
#if defined(IS_KERNEL)
	if (unlikely(time_after(jiffies, stop_time)))
		return 1;

	return 0;
#elif defined(IS_CVT) || defined(IS_UBOOT)
	/* stop_time = timeout in second */
	if (timeout_counter_us++ >= stop_time)
		return 1;
	return 0;
#else
	return 0;
#endif
}

#ifdef IS_KERNEL
void nt72_nand_lock_nfc(struct nt72_nand_info *info)
{
	/* Share GPIO lock with NFC lock */
	int ret;
	unsigned long stop_time = nt72_nand_timeout_init();

	while (1) {
		ret = down_interruptible(&g_sem_nfc);
		switch (ret) {
		case 0:
			/*
			 * Acquired the lock, check if GPIOs need to be
			 * re-configured for IO port.
			 */
			nt72_nand_switch_io(info);
			return;
		case -EINTR:
			/* try again */
			if (unlikely(nt72_nand_timeout_check(stop_time)))
				nfc_warn("nfc_warn: waiting nfc lock\n");
			break;
		default:
			nfc_err("nfc_err: error locking nfc\n");
			break;
		}
	}
}
#endif

#ifdef IS_KERNEL
void nt72_nand_unlock_nfc(struct nt72_nand_info *info)
{
	/* Share GPIO lock with NFC lock */
	up(&g_sem_nfc);
}
#endif

u32 nt72_nand_hweight(u32 x)
{
#ifdef IS_KERNEL
	return hweight32(x);
#else
	/* Refer to LLVM */
	x = x - ((x >> 1) & 0x55555555);
	/* Every 2 bits holds the sum of every pair of bits */
	x = ((x >> 2) & 0x33333333) + (x & 0x33333333);
	/* Every 4 bits holds the sum of every 4-set of bits */
	x = (x + (x >> 4)) & 0x0F0F0F0F;
	/* Every 8 bits holds the sum of every 8-set of bits */
	x = (x + (x >> 16));
	/* The lower 16 bits hold two 8 bit sums */
	return (x + (x >> 8)) & 0x0000003F;
#endif
}

void nt72_nand_init_comp(struct completion *comp)
{
#if defined(IS_KERNEL)
	init_completion(comp);
#else
	comp->irq = 0;
#endif
}

int nt72_nand_wait_for_comp(struct completion *comp, int timeout)
{
#if defined(IS_KERNEL)
	return wait_for_completion_timeout(comp, timeout);
#else
	unsigned long stop_time = nt72_nand_timeout_init();

	while (comp->irq == 0) {
		if (nt72_nand_timeout_check(stop_time)) {
			nfc_err("nfc_err: completion timeout\n");
			break;
		}
	}

	comp->irq = 0;

	return 1;
#endif
}

void nt72_nand_comp(struct completion *comp)
{
#if defined(IS_KERNEL)
	complete(comp);
#else
	comp->irq = 1;
#endif
}
