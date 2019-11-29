#ifndef _NT72_NAND_IR_H_
#define _NT72_NAND_IR_H_

#include "nt72_nand.h"

void nt72_nand_ir_init(struct nt72_nand_info *info, const int busy_wait,
		       const u32 what);

int nt72_nand_ir_wait(struct nt72_nand_info *info, const char *who,
		      const u32 what);
int nt72_nand_ir_busy_wait(struct nt72_nand_info *info, const char *who,
			   const u32 what);

#endif /* _NT72_NAND_IR_H_ */
