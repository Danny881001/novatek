#ifndef _NT72_NAND_INIT_H_
#define _NT72_NAND_INIT_H_

#include "nt72_nand.h"

#if !defined(IS_KERNEL) || defined(STBC_ECC_SWITCH)
void nt72_nand_init_regs(struct nt72_nand_info *info);
#endif
void __init nt72_nand_init_io(struct nt72_nand_info *info) __init;
void __init nt72_nand_init_params(struct nt72_nand_info *info,
				  struct nt72_nand_spec *f) __init;
u32 __init nt72_nand_formal_name(struct nt72_nand_info *info) __init;
void __init nt72_nand_print_params(struct nt72_nand_info *info) __init;

#ifndef SELFTEST_NAND_PARAMS
#define nt72_nand_params_selftest(...) do {} while (0)
#else
void __init nt72_nand_params_selftest(struct nt72_nand_info *info)
__init;
#endif

int __init nt72_nand_detect_flash(struct nt72_nand_info *info,
				  struct nt72_nand_spec **nand_spec) __init;

#ifndef IS_KERNEL
void __init nt72_nand_stbc_keypass(void) __init;
#endif

#endif
