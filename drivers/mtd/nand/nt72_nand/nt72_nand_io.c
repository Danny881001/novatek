#include "include/nt72_nand.h"

#ifdef USE_IO_REMAP
static void __iomem *nfc_base;
static void __iomem *stbc_base;
static void __iomem *clkgen_base;
static void __iomem *gp_base;
static void __iomem *ahb_base;
#endif

#ifdef USE_IO_REMAP
int nt72_nand_ioremap(struct resource *resource)
{
	/*
	 * We would either get STBC-side NFC or ARM-side NFC from device tree,
	 * but we use offset to access the MMIO, so it does not matter.
	 */
	nfc_base = ioremap_nocache(resource->start, resource_size(resource));

	if (nfc_base == NULL) {
		nfc_err("nfc_err: ioremap NFC failed\n");
		goto has_error;
	}

	return 0;
has_error:
	return -1;
}

int nt72_nand_ioremap_peripherals(void)
{
	int result = 0;

	clkgen_base = ioremap_nocache(REG_CLK_GEN_BASE, 0x1000);
	stbc_base = ioremap_nocache(REG_STBC_BASE, 0x1000);
	gp_base = ioremap_nocache(REG_GP_BASE, 0x1000);
	ahb_base = ioremap_nocache(REG_AHB_BASE, 0x1000);

	if (stbc_base == NULL) {
		nfc_err("nfc_err: ioremap STBC failed\n");
		result = -1;
	}
	if (gp_base == NULL) {
		nfc_err("nfc_err: ioremap GBP failed\n");
		result = -1;
	}
	if (clkgen_base == NULL) {
		nfc_err("nfc_err: ioremap CLKGEN failed\n");
		result = -1;
	}
	if (ahb_base == NULL) {
		nfc_err("nfc_err: ioremap AHB failed\n");
		result = -1;
	}
	return result;
}

void nt72_nand_iounmap_all(void)
{
	if (nfc_base != NULL)
		iounmap(nfc_base);
	if (stbc_base != NULL)
		iounmap(stbc_base);
	if (gp_base != NULL)
		iounmap(gp_base);
	if (clkgen_base != NULL)
		iounmap(clkgen_base);
}
#endif

#ifndef USE_IO_REMAP
static inline u32 nt72_nand_io_read32(const u32 addr)
{
	return (u32)(*(volatile unsigned long *)addr);
}

static inline void nt72_nand_io_write32(const u32 addr, u32 val)
{
	*((volatile unsigned long *)addr) = val;
}
#endif

inline u32 clkgen_read32(const u32 addr)
{
	u32 val;

#ifdef USE_IO_REMAP
	val = ioread32(clkgen_base + ((addr) - (REG_CLK_GEN_BASE)));
#else
	val = nt72_nand_io_read32(addr);
#endif

	nt72_nand_bus_sniffer_add("CLKR", addr, val);

	return val;
}

inline u32 nfc_read32(const u32 addr)
{
	u32 val;

#ifdef USE_IO_REMAP
	val = ioread32(nfc_base + ((addr) - (REG_NFC_BASE)));
#else
	if (g_info->io_param.type == ARM_CI_IO)
		val = nt72_nand_io_read32(addr + REG_NFC_BASE_ARM_OFFSET);
	else
		val = nt72_nand_io_read32(addr);
#endif

	nt72_nand_bus_sniffer_add("NFCR", addr, val);

	return val;
}

inline void nfc_write32(const u32 addr, u32 val)
{
#ifdef USE_IO_REMAP
	iowrite32(val, nfc_base + ((addr) - (REG_NFC_BASE)));
#else
	if (g_info->io_param.type == ARM_CI_IO)
		nt72_nand_io_write32(addr + REG_NFC_BASE_ARM_OFFSET, val);
	else
		nt72_nand_io_write32(addr, val);
#endif

	nt72_nand_bus_sniffer_add("NFCW", addr, val);
}

inline u32 stbc_read32(const u32 addr)
{
	u32 val;

#ifdef USE_IO_REMAP
	val = ioread32(stbc_base + ((addr) - (REG_STBC_BASE)));
#else
	val = nt72_nand_io_read32(addr);
#endif

	nt72_nand_bus_sniffer_add("STBCR", addr, val);

	return val;
}

inline void stbc_write32(const u32 addr, u32 val)
{
#ifdef USE_IO_REMAP
	iowrite32(val, stbc_base + ((addr) - (REG_STBC_BASE)));
#else
	nt72_nand_io_write32(addr, val);
#endif

	nt72_nand_bus_sniffer_add("STBCW", addr, val);
}

inline u32 gp_read32(const u32 addr)
{
	u32 val;

#ifdef USE_IO_REMAP
	val = ioread32(gp_base + ((addr) - (REG_GP_BASE)));
#else
	val = nt72_nand_io_read32(addr);
#endif

	nt72_nand_bus_sniffer_add("GPBR", addr, val);

	return val;
}

inline void gp_write32(const u32 addr, u32 val)
{
#ifdef USE_IO_REMAP
	iowrite32(val, gp_base + ((addr) - (REG_GP_BASE)));
#else
	nt72_nand_io_write32(addr, val);
#endif

	nt72_nand_bus_sniffer_add("GPBW", addr, val);
}

inline u32 ahb_read32(const u32 addr)
{
	u32 val;

#ifdef USE_IO_REMAP
	val = ioread32(ahb_base + ((addr) - (REG_AHB_BASE)));
#else
	val = nt72_nand_io_read32(addr);
#endif

	nt72_nand_bus_sniffer_add("AHBR", addr, val);

	return val;
}
