/*
 * Copyright (C) 2014 Novatek Microelectronics Corp. All rights reserved.
 * Author: SP-KSW <SP_KSW_MailGrp@novatek.com.tw>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <asm/io.h>
#include <asm/arch/soc.h>
#include <asm/arch/clock.h>

#include <div64.h>

DECLARE_GLOBAL_DATA_PTR;

#define PTIMER_CTRL_TIMER_DISABLE		0
#define PTIMER_CTRL_TIMER_ENABLE		1
#define PTIMER_CTRL_OVERFLOW_VAL		0xffffffffffffffffUL
#define PTIMER_CTRL_LOAD_VAL			0xffffffff

static unsigned int read_cntfrq(void)
{
	unsigned int freq;

	asm volatile ("mrc p15, 0, %0, c14, c0, 0" : "=r" (freq));

	return freq;
}

static unsigned int read_cntp_ctl(void)
{
	unsigned int ctl;

	asm volatile ("mrc p15, 0, %0, c14, c2, 1" : "=r" (ctl));

	return ctl;
}

static void write_cntp_ctl(unsigned int ctl)
{
	asm volatile ("mcr p15, 0, %0, c14, c2, 1" : : "r" (ctl));
}

static unsigned long long read_cntpct(void)
{
	unsigned long long cval;

	asm volatile("mrrc p15, 0, %Q0, %R0, c14" : "=r" (cval));
	return cval;
}

static void write_cntp_cval(unsigned long long cval)
{
	unsigned int cval_high = (unsigned int)((cval) >> 32);
	unsigned int cval_low  = (unsigned int)((cval) & -1);

	asm volatile ("mcrr p15, 2, %0, %1, c14" : : "r" (cval_low), "r" (cval_high));
}



int timer_init(void)
{
	unsigned long long cval;
	unsigned int ctl = 0, period = 0;

	/* disable timer clock */
	writel(PTIMER_CTRL_TIMER_DISABLE, SYSCNT_CTRL_BASE_ADDR);

	/* disable timer */
	write_cntp_ctl(0);

	period = read_cntfrq();
	cval = read_cntpct() + period;
	write_cntp_cval(cval);

	/* enable timer */
	ctl  = read_cntp_ctl();
	ctl |= (1 << CNT_CTL_ENABLE_SHIFT);
	write_cntp_ctl(ctl);

	/* enable timer clock */
	writel((CNTCR_FCREQ(0) | CNTCR_HDBG | CNTCR_EN), SYSCNT_CTRL_BASE_ADDR);

	return 0;

}

unsigned long long timer_read_counter(void)
{
	unsigned long long cval;

	asm volatile("mrrc p15, 0, %Q0, %R0, c14" : "=r" (cval));
	return cval;
}


ulong get_timer_masked(void)
{
	ulong now;

	now = timer_read_counter() /
			(gd->arch.timer_rate_hz / CONFIG_SYS_HZ);

	if (gd->arch.lastinc >= now) {
		/* Normal mode */
		gd->arch.tbl += (gd->arch.lastinc - now);
	} else {
		/* We have an overflow ... */
		gd->arch.tbl += ((PTIMER_CTRL_LOAD_VAL / (gd->arch.timer_rate_hz / CONFIG_SYS_HZ))
				- now) + gd->arch.lastinc;
	}
	gd->arch.lastinc = now;

	return gd->arch.tbl;
}

ulong get_timer(ulong base)
{
#if 0
	return get_timer_masked() - base;
#else
	unsigned long long now = timer_read_counter();
	unsigned long long tick_per_usec = (read_cntfrq() >> 20);
	ulong time_msec;

	time_msec = (ulong)((now / tick_per_usec) / 1000);

	return time_msec - base;
#endif
}

unsigned long long get_ticks(void)
{
	return get_timer(0);
}

ulong get_tbclk(void)
{
	return read_cntfrq();
}

void __udelay(unsigned long usec)
{
#if 0
	long long tmo = usec * (gd->arch.timer_rate_hz / 1000) / 1000;
	unsigned long long now, last = timer_read_counter();

	while (tmo > 0) {
		now = timer_read_counter();
		if (last > now)
			tmo -= last - now;
		else	/* count up timer overflow */
			tmo -= PTIMER_CTRL_OVERFLOW_VAL - now + last + 1;

		last = now;
	}
#else
	long long tmo;
	unsigned long long now, last = timer_read_counter();

	tmo = (read_cntfrq() >> 20) * usec;
	while (tmo > 0)
	{
		now = timer_read_counter();
		if (last > now) /* count up timer overflow */
			tmo -= PTIMER_CTRL_OVERFLOW_VAL - last + now + 1;
		else
			tmo -= now - last;
		last = now;
	}

#endif
}
