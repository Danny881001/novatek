#ifndef _NT72_NAND_GPIO_H_
#define _NT72_NAND_GPIO_H_

#include "nt72_nand.h"

#ifndef SHARE_GPIO_WITH_CI
#define nt72_nand_switch_io(...) do { } while (0)
#else
void nt72_nand_switch_io(struct nt72_nand_info *info);
#endif
int nt72_nand_check_gpio(struct nt72_nand_info *info);

#endif /* _NT72_NAND_GPIO_H_ */
