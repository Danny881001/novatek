#ifndef _NT72_NAND_IO_H_
#define _NT72_NAND_IO_H_

#include "nt72_nand.h"

#ifdef USE_IO_REMAP
/* struct resource: for newer kernel */
#include <linux/io.h>
/* struct resource: for older kernel */
#include <linux/ioport.h>
#endif

#ifndef USE_IO_REMAP
#define nt72_nand_ioremap(...) do { } while (0)
#define nt72_nand_ioremap_peripherals(...) do { } while (0)
#define nt72_nand_iounmap_all(...) do { } while (0)
#else
int nt72_nand_ioremap(struct resource *resource);
int nt72_nand_ioremap_peripherals(void);
void nt72_nand_iounmap_all(void);
#endif

u32 clkgen_read32(const u32 addr);

u32 nfc_read32(const u32 addr);
void nfc_write32(const u32 addr, u32 val);

u32 stbc_read32(const u32 addr);
void stbc_write32(const u32 addr, u32 val);

u32 gp_read32(const u32 addr);
void gp_write32(const u32 addr, u32 val);

u32 ahb_read32(const u32 addr);

#endif /* _NT72_NAND_IO_H_ */
