/*
 * SAMSUNG EXYNOS5 USB HOST XHCI Controller
 *
 * Copyright (C) 2012 Samsung Electronics Co.Ltd
 *	Vivek Gautam <gautam.vivek@samsung.com>
 *	Vikas Sajjan <vikas.sajjan@samsung.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */


#include <common.h>
#include <dm.h>
#include <fdtdec.h>
#include <libfdt.h>
#include <malloc.h>
#include <usb.h>
#include <watchdog.h>
#include <asm/gpio.h>
#include <linux/compat.h>
#include <linux/usb/dwc3.h>

#include <asm/io.h>
#include <errno.h>

#include "xhci.h"

#define __REG(x)        (*((volatile u32 *) (x)))
#define clear(add,wValue)    writel(~(wValue)&readl(add),add)
#define set(add,wValue)      writel(wValue|readl(add),add)


/* Declare global data pointer */
DECLARE_GLOBAL_DATA_PTR;

/**
 * @hc_base: xHCI AHB reg base
 * @clk_base: clock reg base for reset
 * @apb_base: APB reg base
 * @u2phy_base: UTMI reg base
 * @u3phy_base: PIPE3 reg base
 */
struct nvt_u3_regs {
	uint32_t hc_base;
	uint32_t clk_base;
	uint32_t apb_base;
	uint32_t u2phy_base;
	uint32_t u3phy_base;
};

/**
 * @fun_sel: [0]:reg address  of function select, [1]:bit mask for set GPIO mode
 * @dir: [0]:reg address of direction select, [1]:set value to select output direction
 * @high: [0]:reg address of set level high, [1]:set value to set level high
 * @low: [0]:reg address of set level low, [1]:set value to set level low
 */
struct nvt_u3_gpio {
	uint32_t fun_sel[2];
	uint32_t dir_sel[2];
	uint32_t high_set[2];
	uint32_t low_set[2];
};

struct nvt_xhci_platdata {
	struct nvt_u3_regs init_regs;
	struct nvt_u3_gpio vbus_gpio;
};

/**
 * Contains pointers to register base addresses
 * for the usb controller.
 */
struct nvt_xhci {
	//struct usb_platdata usb_plat;
	struct xhci_ctrl ctrl;
	struct nvt_u3_regs *hc_reg;
	struct nvt_u3_gpio *vbus_reg;
	struct xhci_hccr *hcd;
	struct dwc3 *dwc3_reg;
};

static int xhci_usb_ofdata_to_platdata(struct udevice *dev)
{
	struct nvt_xhci_platdata *plat = dev_get_platdata(dev);
	const void *blob = gd->fdt_blob;
	unsigned int node;
	unsigned int regs[2], phys[4], gpios[8];	

	/*
	 * Get the base address for XHCI controller from the device node
	 */
	node = dev->of_offset;
	fdtdec_get_int_array(blob, node, "reg", regs, 2);
	plat->init_regs.hc_base = regs[0];

	/*
	 * Get the base address for usbphy from the device node
	 */
	node = fdtdec_lookup_phandle(blob, dev->of_offset, "usb3phy");
	fdtdec_get_int_array(blob, node, "reg", phys, 4);
	plat->init_regs.clk_base = phys[0];	
	plat->init_regs.apb_base = phys[1];	
	plat->init_regs.u2phy_base = phys[2];	
	plat->init_regs.u3phy_base = phys[3];	

	/* Vbus gpio */
	node = dev->of_offset;
	fdtdec_get_int_array(blob, node, "gpio", gpios, 8);
	plat->vbus_gpio.fun_sel[0] = gpios[0];
	plat->vbus_gpio.fun_sel[1] = gpios[1];

	plat->vbus_gpio.dir_sel[0] = gpios[2];
	plat->vbus_gpio.dir_sel[1] = gpios[3];

	plat->vbus_gpio.high_set[0] = gpios[4];
	plat->vbus_gpio.high_set[1] = gpios[5];

	plat->vbus_gpio.low_set[0] = gpios[6];
	plat->vbus_gpio.low_set[1] = gpios[7];

	/* 
	printf("\n============== NVT-xhci info =============\n");
	printf("Host REG:\n");
	printf("[xHC]  =0x%08x\n", plat->init_regs.hc_base);
	printf("[clk]  =0x%08x\n", plat->init_regs.clk_base);
	printf("[apb]  =0x%08x\n", plat->init_regs.apb_base);
	printf("[u2phy]=0x%08x\n", plat->init_regs.u2phy_base);
	printf("[u3phy]=0x%08x\n", plat->init_regs.u3phy_base);
	printf("\nGPIO:\n");
	printf("[fun sel] =0x%08x, mask=0x%08x\n", plat->vbus_gpio.fun_sel[0], plat->vbus_gpio.fun_sel[1]);
	printf("[dir sel] =0x%08x, val=0x%08x\n", plat->vbus_gpio.dir_sel[0], plat->vbus_gpio.dir_sel[1]);
	printf("[lvl high]=0x%08x, val=0x%08x\n", plat->vbus_gpio.high_set[0], plat->vbus_gpio.high_set[1]);
	printf("[lvl low] =0x%08x, val=0x%08x\n", plat->vbus_gpio.low_set[0], plat->vbus_gpio.low_set[1]);
	printf("==========================================\n");
	*/

	return 0;
}

void usb_power_on(struct nvt_u3_gpio *p)
{
	/* Turn on VBUS */
	//Set GPIO mode
	clear(p->fun_sel[0], p->fun_sel[1]);

	//Set Output Direction
	set(p->dir_sel[0], p->dir_sel[1]);

	//Set level high
	set(p->high_set[0], p->high_set[1]);
}

void usb_power_off(struct nvt_u3_gpio *p)
{
	/* Turn off VBUS */
	//Set GPIO mode
	clear(p->fun_sel[0], p->fun_sel[1]);

	//Set Output Direction
	set(p->dir_sel[0], p->dir_sel[1]);

	//Set level low
	set(p->low_set[0], p->low_set[1]);
}

static int xhci_nvt_init(struct nvt_u3_regs *p)
{
	printf("[%s] 0x%08x\n", __func__, p->hc_base);

	//ssc setting
	writel(0x2, p->clk_base+0xbc);
	udelay(0x20);
	writel(0xbb, p->clk_base+0xd8);
	writel(0x3b, p->clk_base+0xd4);
	udelay(0x20);
	writel(0x33, p->clk_base+0xd4);
	udelay(0x20);

	//usb2.0 phy 30M Clk switch
	set(p->clk_base+0x48, 0x1<<22);
	udelay(10);

	//# set Aclk sw reset
	writel(0x1<<27, p->clk_base+0x2168);
	udelay(0x20);
	//# set Aclk sw clear
	writel(0x1<<27, p->clk_base+0x2108);
	udelay(0x20);

	//# set Pclk reset
	writel(0x1<<14, p->clk_base+0x2068);
	udelay(0x20);
	//# set Pbus clk clear
	writel(0x1<<14, p->clk_base+0x2008);
	udelay(0x20);

	//# set Hclk sw reset
	writel(0x1<<31, p->clk_base+0x2268);
	udelay(0x20);
	//# set Hclk sw clear
	writel(0x1<<31, p->clk_base+0x2208);
	udelay(0x20);

	//usb2.0 PONRST, SUSPEND, PORTRST
	//special case: use faraday usb2.0's reg, 0xfd184000
	writel(0xc5a0,(volatile void *)0xfd184000);
	writel(0x4a0,(volatile void *)0xfd184000);
	udelay(10);

	//usb3.0 PONRST
	writel(0x4a0, p->apb_base);
	writel(0x5a0, p->apb_base);
	udelay(10);

	// #30M setting
	clear(p->apb_base+0xc, 0x3f<<20);
	set(p->apb_base+0xc, 0x20<<20);

	/* usb 3.0 phy setting*/
	// EOC RX_ICTRL's offset=0
	writel(0x40,p->u3phy_base+0x6fc);

	// TX_AMP_CTL=3, TX_DEC_EM_CTL=f
	writel(0xfb,p->u3phy_base+0x50);

	// TX_LFPS_AMP_CTL=0x1
	writel(0xfc,p->u3phy_base+0xd0);

	// power mode change ready response time = 3 ms
	writel(0x0b,p->u3phy_base+0x450);

	// the low byte of vbs tune frequency count (560MHz, PLL=2.8GHz)
	writel(0x2e,p->u3phy_base+0x548);

	// the high byte of vbs tune frequency count (560MHz, PLL=2.8GHz)
	writel(0x01,p->u3phy_base+0x54c);

	// ECO use 2-data sampler output only
	writel(0xc0,p->u3phy_base+0x6c0);

	// The low byte of ECO threshold
	writel(0x91,p->u3phy_base+0x6c4);

	// The high byte of ECO threshold
	writel(0x00,p->u3phy_base+0x6c8);

	// Enable VGA force mode
	writel(0x88,p->u3phy_base+0x4d4);

	// VGA = 5
	writel(0x50,p->u3phy_base+0x4a8);

	// Enable BIAS_ICDR_SEL force mode
	writel(0x80,p->u3phy_base+0x7c0);

	// Set BIAS_ICDR_SEL =0xB
	writel(0xb1,p->u3phy_base+0x7d4);

	// 125M Clk switch
	set(p->clk_base+0x48, 0x1<<30);

	set(p->hc_base+0xc200, 0x1<<3);
	/*HS-Program UTMI interface for 8 or 16 bit or ULPI - GUSB2PHYCFGn*/
	set(p->hc_base+0xc12c, 0x1<<14);

	//fix sandisk disconnect issue with new fifo setting
	set(p->hc_base+0xc2c0, 0x1<<0);

	//fix super speed device disconnection when enter u3.
	//bit 28 raise from P3 to P2 when XHC want to request
	//rx termination detection. This problem may be caused
	//by low sleep clk of XHC
	set(p->hc_base+0xc2c0, 0x1<<28);

	// #30M setting
	writel(0x02ac4110, p->u3phy_base+0xc);
	writel(0x027c4110, p->u3phy_base+0xc);

	return 0;
}

static void nvt_xhci_core_exit(struct nvt_xhci *ctx)
{	
	usb_power_off(ctx->vbus_reg);
}

static int xhci_usb_probe(struct udevice *dev)
{
	struct nvt_xhci_platdata *plat = dev_get_platdata(dev); 
	struct nvt_xhci *ctx = dev_get_priv(dev);
	struct xhci_hcor *hcor;
	int ret;

	printf("%s\n", __func__);

	ctx->hcd = (struct xhci_hccr *)plat->init_regs.hc_base;
	ctx->hc_reg = (struct nvt_u3_regs *)(&plat->init_regs);
	ctx->vbus_reg = (struct nvt_u3_gpio *)(&plat->vbus_gpio);
	ctx->dwc3_reg = (struct dwc3 *)((char *)(ctx->hcd) + DWC3_REG_OFFSET);
	hcor = (struct xhci_hcor *)((uint32_t)ctx->hcd +
			HC_LENGTH(xhci_readl(&ctx->hcd->cr_capbase)));

	printf("%s hccr 0x%x and hcor 0x%x hc_length %d\n", __func__,
		(uint32_t)ctx->hcd, (uint32_t)hcor,
		(uint32_t)HC_LENGTH(xhci_readl(&ctx->hcd->cr_capbase)));

	/* setup the Vbus gpio here */
	usb_power_on(&plat->vbus_gpio);
	mdelay(100);

	/* initial nvt xhci phy */
	ret = xhci_nvt_init(&plat->init_regs);
	if (ret) {
		puts("XHCI: failed to initialize controller\n");
		return -EINVAL;
	}

	return xhci_register(dev, ctx->hcd, hcor);
}

static int xhci_usb_remove(struct udevice *dev)
{
	struct nvt_xhci *ctx = dev_get_priv(dev);
	int ret;

	ret = xhci_deregister(dev);
	if (ret)
		return ret;
	nvt_xhci_core_exit(ctx);

	return 0;
}

static const struct udevice_id xhci_usb_ids[] = {
	{ .compatible = "nvt,NT72668-xhci" },
	{ }
};

U_BOOT_DRIVER(usb_xhci) = {
	.name	= "xhci_nvt",
	.id	= UCLASS_USB,
	.of_match = xhci_usb_ids,
	.ofdata_to_platdata = xhci_usb_ofdata_to_platdata,
	.probe = xhci_usb_probe,
	.remove = xhci_usb_remove,
	.ops	= &xhci_usb_ops,
	.platdata_auto_alloc_size = sizeof(struct nvt_xhci_platdata),
	.priv_auto_alloc_size = sizeof(struct nvt_xhci),
	.flags	= DM_FLAG_ALLOC_PRIV_DMA,

};
