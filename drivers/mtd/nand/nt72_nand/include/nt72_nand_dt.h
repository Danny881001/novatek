#ifndef _NT72_NAND_DT_H_
#define _NT72_NAND_DT_H_

#include "nt72_nand.h"
#include <linux/of_device.h>

#ifdef USE_DT
extern const struct of_device_id g_match_table[];
#endif  /* USE_DT */

#endif  /* _NT72_NAND_DT_H_ */
