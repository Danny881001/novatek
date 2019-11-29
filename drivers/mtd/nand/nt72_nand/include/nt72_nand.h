#ifndef _NT72_NAND_H_
#define _NT72_NAND_H_

#include "nt72_nand_config.h"

#ifdef IS_UBOOT
#include <common.h>
#include <nand.h>
#include <linux/mtd/nand.h>
#include <linux/types.h>
#endif

#ifdef IS_KERNEL
#include <linux/completion.h>
#include <linux/crc32.h>
#include <linux/debugfs.h>
#include <linux/delay.h>
#include <linux/dma-mapping.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/jiffies.h>
#include <linux/module.h>
#include <linux/of_device.h>
#include <linux/platform_device.h>
#include <linux/random.h>
#include <linux/semaphore.h>
#include <linux/slab.h>
#include <linux/time.h>
#include <linux/mtd/nand.h>
#include <linux/mtd/partitions.h>
#include <linux/mtd/nt72_nand_api.h>
#endif

#include "nt72_nand_globals.h"
#include "nt72_nand_nfc.h"
#include "nt72_nand_specs_internal.h"

#ifdef USE_DT
#include "nt72_nand_dt.h"
#endif

#ifdef IS_XBOOT
#include <nt72_nand_xboot.h>
#include <string.h>
#include <cache.h>
#include <math.h>
#endif

#ifdef IS_UBOOT
#include "nt72_nand_ecc_layouts.h"
#include "nt72_nand_mtd.h"
#include "nt72_nand_parts.h"
#endif

#ifdef IS_KERNEL
#include "nt72_nand_dfs.h"
#include "nt72_nand_ecc_layouts.h"
#include "nt72_nand_mtd.h"
#include "nt72_nand_parts.h"
#include "nt72_nand_kernel.h"
#endif

#include "nt72_nand_gpio.h"
#include "nt72_nand_io.h"
#include "nt72_nand_ir.h"
#include "nt72_nand_kapi.h"
#ifndef IS_XBOOT
#include "nt72_nand_init.h"
#endif
#include "nt72_nand_regs.h"
#include "nt72_nand_utils.h"

#ifdef HAS_NTS
#include "../../nts_plugin/nvt_nts_util.h"
#endif

#endif /* _NT72_NAND_H_ */
