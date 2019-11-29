/*
 * Copyright (C) 2014 Novatek Microelectronics Corp. All rights reserved.
 * Author: SP-KSW <SP_KSW_MailGrp@novatek.com.tw>
 *
 * A lowlevel_init function that sets up the stack to call a C function to
 * perform further init.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <asm-offsets.h>
#include <config.h>
#include <linux/linkage.h>
#include <asm/ptrace.h>

/*
 * save_boot_params
 *
 * Stack pointer is not yet initialized at this moment.
 * Don't save anything to stack even if compiled with -O0
 */

ENTRY(save_boot_params)
	b	save_boot_params_ret		@ back to my caller
ENDPROC(save_boot_params)

ENTRY(lowlevel_init)
	cps     #MON_MODE
	MRC     p15, 0, r1, c1,c1,0
	isb
	orr     r0, r1, #1
	MCR     p15,0,r0,c1,c1,0
	isb
	mov     r0, #0
	mcrr    p15, 4, r0, r0, c14		/* CNTVOFF = 0 */
	isb
	mcr	p15, 0, r1, c1, c1, 0		/* Set Secure bit */
	isb
	cps     #SVC_MODE

	/*
	 * Setup a temporary stack
	 */
	ldr	sp, =CONFIG_SYS_INIT_SP_ADDR
	bic	sp, sp, #7 /* 8-byte alignment for ABI compliance */
#ifdef CONFIG_SPL_BUILD
	ldr	r9, =gdata
#else
	sub	sp, sp, #GD_SIZE
	bic	sp, sp, #7
	mov	r9, sp
#endif
	/*
	 * Save the old lr(passed in ip) and the current lr to stack
	 */
	push	{ip, lr}

	/*
	 * go setup pll, mux, memory
	 */
	bl	s_init
	pop	{ip, pc}
ENDPROC(lowlevel_init)
