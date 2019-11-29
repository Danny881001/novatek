/*
 * SAMSUNG EXYNOS USB HOST EHCI Controller
 *
 * Copyright (C) 2012 Samsung Electronics Co.Ltd
 *	Vivek Gautam <gautam.vivek@samsung.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <dm.h>
#include <fdtdec.h>
#include <libfdt.h>
#include <malloc.h>
#include <usb.h>
#include <asm-generic/errno.h>
#include <asm/io.h>
#include "ehci.h"

#include <ehci-nvt.h>

/* Declare global data pointer */
DECLARE_GLOBAL_DATA_PTR;

static int phy_init = 0;

uint32_t NVT_USB_BASE = USB0_EHCI_BASE;
#if defined(CONFIG_ARCH_NT72673_A32) || defined(CONFIG_ARCH_NT72673_A64) || defined(CONFIG_ARCH_NT72671_A32) || defined(CONFIG_ARCH_NT72671_A64)

/*======================72673==========================*/
const struct usb_clk_table_freestyle stSYSClkRst_fs[] =
{
		//uclk
		{0x260, 0xFFFFFFFF, (1UL << 8),
		 0x200, 0xFFFFFFFF, (1UL << 8),
		 EN_SYS_CLK_RST_CORE_USB20_U0},

		{0x260, 0xFFFFFFFF, (1UL << 9),
		 0x200, 0xFFFFFFFF, (1UL << 9),
		 EN_SYS_CLK_RST_CORE_USB20_U1},

		{0x68 , 0xFFFFFFFF, (1UL << 27),
		 0x8  , 0xFFFFFFFF, (1UL << 27),
		 EN_SYS_CLK_RST_CORE_USB20_U2},

		{0x68 , 0xFFFFFFFF, (1UL << 28),
		 0x8  , 0xFFFFFFFF, (1UL << 28),
		 EN_SYS_CLK_RST_CORE_USB20_U3},

		//aclk
		{0x160, 0xFFFFFFFF, (1UL << 6),
		 0x100, 0xFFFFFFFF, (1UL << 6),
		 EN_SYS_CLK_RST_AXI_USB20_U0},

		{0x160, 0xFFFFFFFF, (1UL << 7),
		 0x100, 0xFFFFFFFF, (1UL << 7),
		 EN_SYS_CLK_RST_AXI_USB20_U1},

		{0x68 , 0xFFFFFFFF, (1UL << 24),
		 0x8 , 0xFFFFFFFF, (1UL << 24),
		 EN_SYS_CLK_RST_AXI_USB20_U2},

		{0x68 , 0xFFFFFFFF, (1UL << 25),
		 0x8 , 0xFFFFFFFF, (1UL << 25),
		 EN_SYS_CLK_RST_AXI_USB20_U3},

		//ahb
		{0x60  , 0xFFFFFFFF, (1UL << 8),
		 0x0 , 0xFFFFFFFF, (1UL << 8),
		 EN_SYS_CLK_RST_AHB_USB20_U0},

		{0x60  , 0xFFFFFFFF, (1UL << 10),
		 0x0 , 0xFFFFFFFF, (1UL << 10),
		 EN_SYS_CLK_RST_AHB_USB20_U1},

		{0x68  , 0xFFFFFFFF, (1UL << 19),
		 0x8 , 0xFFFFFFFF, (1UL << 19),
		 EN_SYS_CLK_RST_AHB_USB20_U2},

		{0x68  , 0xFFFFFFFF, (1UL << 21),
		 0x8 , 0xFFFFFFFF, (1UL << 21),
		 EN_SYS_CLK_RST_AHB_USB20_U3},

		//apb
		{0x60  , 0xFFFFFFFF, (1UL << 9),
		 0x0 , 0xFFFFFFFF, (1UL << 9),
		 EN_SYS_CLK_RST_AHB_USB20_U0_PCLK},

		{0x60  , 0xFFFFFFFF, (1UL << 11),
		 0x0 , 0xFFFFFFFF, (1UL << 11),
		 EN_SYS_CLK_RST_AHB_USB20_U1_PCLK},

		{0x68 , 0xFFFFFFFF, (1UL << 20),
		 0x8  , 0xFFFFFFFF, (1UL << 20),
		 EN_SYS_CLK_RST_AHB_USB20_U2_PCLK},

		{0x68 , 0xFFFFFFFF, (1UL << 22),
		 0x8  , 0xFFFFFFFF, (1UL << 22),
		 EN_SYS_CLK_RST_AHB_USB20_U3_PCLK},

		//usb30 hi-rst
		{0x268 , 0xFFFFFFFF, (1UL << 31),
		 0x208  , 0xFFFFFFFF, (1UL << 31),
		 EN_SYS_CLK_RST_AHB_USB30},

		{0x0  , 0x0, 0x0,
		 0x0  , 0x0, 0x0,
		 EN_END},
};

static void clk_rst(int index, int on)
{
	int x = 0;
	unsigned int dbg_val,dbg_base;
	while(1) {
		if (stSYSClkRst_fs[x].index == EN_END) {
			printf("Error, clk_rst function can't seek match clk index %d on %d\n", index, on);
			return;
		} else if (stSYSClkRst_fs[x].index == index) {
			if (on) {
				//dbg
				dbg_val = ((*(volatile uint32_t *)(CLK_BASE + stSYSClkRst_fs[x].RegOff_rst)) & (~stSYSClkRst_fs[x].mask_rst)) | stSYSClkRst_fs[x].val_rst;
				dbg_base =  CLK_BASE + stSYSClkRst_fs[x].RegOff_rst;
				//dbg end
				*(volatile uint32_t *)(CLK_BASE + stSYSClkRst_fs[x].RegOff_rst) =
					((*(volatile uint32_t *)(CLK_BASE + stSYSClkRst_fs[x].RegOff_rst)) & (~stSYSClkRst_fs[x].mask_rst)) | stSYSClkRst_fs[x].val_rst;
			} else {
				//dbg
				dbg_val = ((*(volatile uint32_t *)(CLK_BASE + stSYSClkRst_fs[x].RegOff_clr)) & (~stSYSClkRst_fs[x].mask_clr)) | stSYSClkRst_fs[x].val_clr;
				dbg_base =  CLK_BASE + stSYSClkRst_fs[x].RegOff_clr;
				//dbg end
				*(volatile uint32_t *)(CLK_BASE + stSYSClkRst_fs[x].RegOff_clr) =
					((*(volatile uint32_t *)(CLK_BASE + stSYSClkRst_fs[x].RegOff_clr)) & (~stSYSClkRst_fs[x].mask_clr)) | stSYSClkRst_fs[x].val_clr;
			}
			printf("clk_rst set val %u , base %u , on %d\n", dbg_val, dbg_base, on);
			return;
		}
		x++;
	}
}

#else
const struct usb_clk_table stSYSClkRst[] =
{
		{0x50, (1UL << 8), EN_SYS_CLK_RST_CORE_USB20_U0},
		{0x50, (1UL << 9), EN_SYS_CLK_RST_CORE_USB20_U1},
		{0x60, (1UL << 4), EN_SYS_CLK_RST_CORE_USB20_U2},
		{0x60, (1UL << 5), EN_SYS_CLK_RST_CORE_USB20_U3},

		{0x70, (1UL << 6), EN_SYS_CLK_RST_AXI_USB20_U0},
		{0x70, (1UL << 7), EN_SYS_CLK_RST_AXI_USB20_U1},
		{0x80, (1UL << 2), EN_SYS_CLK_RST_AXI_USB20_U2},
		{0x80, (1UL << 3), EN_SYS_CLK_RST_AXI_USB20_U3},

		{0x90, (1UL << 8), EN_SYS_CLK_RST_AHB_USB20_U0},
		{0x90, (1UL << 10), EN_SYS_CLK_RST_AHB_USB20_U1},
		{0xA0, (1UL << 10), EN_SYS_CLK_RST_AHB_USB20_U2},
		{0xA0, (1UL << 11), EN_SYS_CLK_RST_AHB_USB20_U3},

		{0x90, (1UL << 9), EN_SYS_CLK_RST_AHB_USB20_U0_PCLK},
		{0x90, (1UL << 11), EN_SYS_CLK_RST_AHB_USB20_U1_PCLK},
		{0xA0, (1UL << 12), EN_SYS_CLK_RST_AHB_USB20_U2_PCLK},
		{0xA0, (1UL << 13), EN_SYS_CLK_RST_AHB_USB20_U3_PCLK},

		{0xA0, (1UL << 13), EN_END},
};

static void clk_rst(int index, int on)
{
	int x = 0;
	while(1) {
		if (stSYSClkRst[x].index == EN_END) {
			printf("Error, clk_rst function can't seek match clk index %d on %d\n", index, on);
			return;
		} else if (stSYSClkRst[x].index == index) {
			if (on) {
				*(volatile uint32_t *)(CLK_BASE + stSYSClkRst[x].RegOff + 4) = stSYSClkRst[x].bit;
			} else {
				*(volatile uint32_t *)(CLK_BASE + stSYSClkRst[x].RegOff) = stSYSClkRst[x].bit;
			}
			return;
		}
		x++;
	}
}
#endif

static int ehci_usb_ofdata_to_platdata(struct udevice *dev)
{
	return 0;
}

void ehci_nvt_init(void * addr, struct ehci_hccr **hccr, struct ehci_hcor **hcor, struct udevice *dev)
{
	int len;
	const fdt32_t *trim_val;

	NVT_USB_BASE = (uint32_t)addr;

	printf("[%s] 0x%p\n", __func__, addr);
	if (phy_init == 1)
		goto skip_phy;
	printf("phy init\n");
	phy_init = 1;
	//usb port 0, 1 ACP disable
	*(volatile uint32_t *)(0xfd120084) &= ~(0x3<<28);
	//usb port 2, 3 ACP disable
	*(volatile uint32_t *)(0xfd140084) &= ~(0x3<<29);

#if defined(CONFIG_ARCH_NT72673_A32) || defined(CONFIG_ARCH_NT72673_A64) || defined(CONFIG_ARCH_NT72671_A32) || defined(CONFIG_ARCH_NT72671_A64)
	//usb port 2,3, 3.0 phy hi rst
	clk_rst(EN_SYS_CLK_RST_AHB_USB30, 1);
	udelay(0x20);
	clk_rst(EN_SYS_CLK_RST_AHB_USB30, 0);
	udelay(0x20);
#endif

	//usb port 0, 1 pclk reset
	clk_rst(EN_SYS_CLK_RST_AHB_USB20_U0_PCLK, 1);
	clk_rst(EN_SYS_CLK_RST_AHB_USB20_U1_PCLK, 1);
	udelay(0x20);
	clk_rst(EN_SYS_CLK_RST_AHB_USB20_U0_PCLK, 0);
	clk_rst(EN_SYS_CLK_RST_AHB_USB20_U1_PCLK, 0);

	//usb port 2,3 pclk reset
	clk_rst(EN_SYS_CLK_RST_AHB_USB20_U2_PCLK, 1);
	clk_rst(EN_SYS_CLK_RST_AHB_USB20_U3_PCLK, 1);
	udelay(0x20);
	clk_rst(EN_SYS_CLK_RST_AHB_USB20_U2_PCLK, 0);
	clk_rst(EN_SYS_CLK_RST_AHB_USB20_U3_PCLK, 0);


	//# set PONRST to 1
	*(volatile uint32_t *)(NVT_USB0_APB_BASE) &= ~(0x1<<8);
	*(volatile uint32_t *)(NVT_USB2_APB_BASE) &= ~(0x1<<8);
	udelay(0x20);

	// waiting of APB read period
	*(volatile uint32_t *)(NVT_USB0_APB_BASE) &= ~(0x7 << 5);
	*(volatile uint32_t *)(NVT_USB1_APB_BASE) &= ~(0x7 << 5);
	*(volatile uint32_t *)(NVT_USB2_APB_BASE) &= ~(0x7 << 5);
	*(volatile uint32_t *)(NVT_USB3_APB_BASE) &= ~(0x7 << 5);
	udelay(0x20);
	*(volatile uint32_t *)(NVT_USB0_APB_BASE) |= (0x1 << 5);
	*(volatile uint32_t *)(NVT_USB1_APB_BASE) |= (0x1 << 5);
	*(volatile uint32_t *)(NVT_USB2_APB_BASE) |= (0x1 << 5);
	*(volatile uint32_t *)(NVT_USB3_APB_BASE) |= (0x1 << 5);
	udelay(0x20);

	//# select SUSPN and RST mask as 0
	*(volatile uint32_t *)(NVT_USB0_APB_BASE) &= ~(0x3<<14);
	*(volatile uint32_t *)(NVT_USB1_APB_BASE) &= ~(0x3<<14);
	*(volatile uint32_t *)(NVT_USB2_APB_BASE) &= ~(0x3<<14);
	*(volatile uint32_t *)(NVT_USB3_APB_BASE) &= ~(0x3<<14);
	udelay(0x20);

	//# set usb port 0,1 uclk sw reset
	clk_rst(EN_SYS_CLK_RST_CORE_USB20_U0, 1);
	clk_rst(EN_SYS_CLK_RST_CORE_USB20_U1, 1);

	//# set usb port 2,3 uclk sw reset
	clk_rst(EN_SYS_CLK_RST_CORE_USB20_U2, 1);
	clk_rst(EN_SYS_CLK_RST_CORE_USB20_U3, 1);
	udelay(0x20);

	//# set usb port 0,1 hclk sw reset
	clk_rst(EN_SYS_CLK_RST_AHB_USB20_U0, 1);
	clk_rst(EN_SYS_CLK_RST_AHB_USB20_U1, 1);
	//# set usb port 2,3 hclk sw reset
	clk_rst(EN_SYS_CLK_RST_AHB_USB20_U2, 1);
	clk_rst(EN_SYS_CLK_RST_AHB_USB20_U3, 1);
	udelay(0x20);

	//# set usb port 0,1 aclk sw reset
	clk_rst(EN_SYS_CLK_RST_AXI_USB20_U0, 1);
	clk_rst(EN_SYS_CLK_RST_AXI_USB20_U1, 1);
	//# set usb port 2,3 aclk sw reset
	clk_rst(EN_SYS_CLK_RST_AXI_USB20_U2, 1);
	clk_rst(EN_SYS_CLK_RST_AXI_USB20_U3, 1);
	udelay(0x20);

	//# set usb port 0,1 aclk sw clear
	clk_rst(EN_SYS_CLK_RST_AXI_USB20_U0, 0);
	clk_rst(EN_SYS_CLK_RST_AXI_USB20_U1, 0);
	//# set usb port 2,3 aclk sw clear
	clk_rst(EN_SYS_CLK_RST_AXI_USB20_U2, 0);
	clk_rst(EN_SYS_CLK_RST_AXI_USB20_U3, 0);
	udelay(0x20);

	//# set usb port 0,1 hclk sw clear
	clk_rst(EN_SYS_CLK_RST_AHB_USB20_U0, 0);
	clk_rst(EN_SYS_CLK_RST_AHB_USB20_U1, 0);
	//# set usb port 2,3 hclk sw clear
	clk_rst(EN_SYS_CLK_RST_AHB_USB20_U2, 0);
	clk_rst(EN_SYS_CLK_RST_AHB_USB20_U3, 0);
	udelay(0x20);

	//# set usb port 0,1 uclk sw clear
	clk_rst(EN_SYS_CLK_RST_CORE_USB20_U0, 0);
	clk_rst(EN_SYS_CLK_RST_CORE_USB20_U1, 0);
	//# set usb port 2,3 uclk sw clear
	clk_rst(EN_SYS_CLK_RST_CORE_USB20_U2, 0);
	clk_rst(EN_SYS_CLK_RST_CORE_USB20_U3, 0);
	udelay(0x20);

	//#set usb port 0,1 UCLK to PHY 30M
	*(volatile uint32_t *)(0xfd020048) |= (0x1<<27);
	//#set usb port 2,3 UCLK to PHY 30M
	*(volatile uint32_t *)(0xfd020048) |= (0x1<<22);
	udelay(0x20);

	//pw setting
	//*(volatile uint32_t *)(0xfd0d0004) |= (0xF<<20);
	//udelay(0x20);
	//*(volatile uint32_t *)(0xfd0d0008) |= (0xF<<20);

	trim_val = fdt_getprop(gd->fdt_blob, dev->of_offset, "nvt,triming", &len);
	if (!trim_val)
		goto skip_phy;

	//USB phy group 0 trimming data
	printf("\nupdate USB0/1 triming data : 0xb\n");

	writel(readl(NVT_USB0_APB_BASE + 0x540) | 0x20, NVT_USB0_APB_BASE + 0x540);
	writel(0x60 | 0xb, NVT_USB0_APB_BASE + 0x548);

	writel(0x20, NVT_USB0_APB_BASE + 0x544);
	udelay(0x20);
	writel(0x0, NVT_USB0_APB_BASE + 0x544);

	printf("set USB0 triming done 0x%08x",readl(NVT_USB0_APB_BASE + 0x4ec));

	//USB phy group 1 trimming data
#if defined(CONFIG_ARCH_NT72673_A32) || defined(CONFIG_ARCH_NT72673_A64) || defined(CONFIG_ARCH_NT72671_A32) || defined(CONFIG_ARCH_NT72671_A64)
	printf("\nupdate USB3/4 + usb 3.0 triming data : 0xb\n");
	//clear usb 3.0 ss phy ponrst
	writel(readl(NVT_USB30_APB_BASE) | (1<<8), NVT_USB30_APB_BASE);
	writel(0x40 | 0xb, NVT_USB30_SS_PHY_BASE + 0x680);
	writel(0x60, NVT_USB30_SS_PHY_BASE + 0x68C);
	udelay(0x10);
	writel(0x20, NVT_USB30_HS_PHY_BASE + 0x144);
	writel(0x00, NVT_USB30_HS_PHY_BASE + 0x144);
	udelay(0x100);

	printf("set USB3 triming done 0x%08x\n",readl(NVT_USB30_HS_PHY_BASE + 0xec));

#else
	printf("\nupdate USB2/3 triming data : 0xb\n");

	writel(readl(NVT_USB2_APB_BASE + 0x540) | 0x20, NVT_USB2_APB_BASE + 0x540);
	writel(0x60 | 0xb, NVT_USB2_APB_BASE + 0x548);

	writel(0x20, NVT_USB2_APB_BASE + 0x544);
	udelay(0x20);
	writel(0x0, NVT_USB2_APB_BASE + 0x544);

	printf("set USB3 triming done 0x%08x\n",readl(NVT_USB2_APB_BASE + 0x4ec));
#endif

skip_phy:

	writel(0x20,(volatile void *)(NVT_USB_BASE+0x100));
	writel(0x0200000e,(volatile void *)(NVT_USB_BASE+0xe0));
	writel(readl((const volatile void *)NVT_USB_BASE+0x84),(volatile void *)NVT_USB_BASE+0x84);

	set((NVT_USB_BASE+0xc4),0x3);

	clear(NVT_USB_BASE+0x88,clearMode);
	set(NVT_USB_BASE+0x88,setMode);

	clear((NVT_USB_BASE+0x80),0x20);
	set((NVT_USB_BASE+0x80),0x10);
	//for full speed device
	//set((NVT_USB_BASE + 0x80),1<<28);

	*hccr = (struct ehci_hccr *)addr;
	*hcor = (struct ehci_hcor *)((uint32_t)*hccr +
			HC_LENGTH(ehci_readl(&(*hccr)->cr_capbase)));

	printf("%s\n", __func__);

}

static int nvt_get_port_speed(struct ehci_ctrl *ctrl, uint32_t reg)
{
{
	unsigned tmp = ehci_readl(((void *)ctrl->hcor) + 0x70);
//	unsigned int tmp = *(volatile unsigned int *)(0xfc1f0080);

//	printf("[%s] AAA hcor->or_portsc 0x%x  value 0x%x\n", __func__, ((void *)ctrl->hcor) + 0x70, tmp);
	switch ((tmp >> 22) & 3) {
		case 0:
			return PORTSC_PSPD_FS;
		case 1:
			return PORTSC_PSPD_LS;
		case 2:
		default:
			return PORTSC_PSPD_HS;
	}
}
}

const struct ehci_ops nvt_ehci_ops = {
	.set_usb_mode		= NULL,
	.get_port_speed		= nvt_get_port_speed,
	.powerup_fixup		= NULL,
	.get_portsc_register	= NULL,
};

static void ehci_nvt_vbus(struct udevice *dev)
{
	int len, index;
	unsigned long reg_base;
	unsigned int mask, value;
	const fdt32_t *reg;

	reg = fdt_getprop(gd->fdt_blob, dev->of_offset, "gpio", &len);
	if (!reg) {
		printf("[%s] no gpio dts\n", __func__);
		return;
	}

	if (len < 12 || len % 12) {
		printf("[%s] incorrect gpio dts length %d\n", __func__, len);
		return;
	}

/* dts example :
*	gpio = <0x30000000 0xff00 0x3f0>,
*		<0x30000004 0xff0000 0x2f000>;
*/
	index = 0;
	while (1) {
		if (len == 0)
			break;
		reg_base = fdt32_to_cpu(reg[index]);
		mask = fdt32_to_cpu(reg[index + 1]);
		value = fdt32_to_cpu(reg[index + 2]);
//		printf("[%s] 3 reg_base 0x%lx mask 0x%x value 0x%x index %d\n", __func__, reg_base, mask, value, index);
		writel((readl((const volatile void *)reg_base) & (~mask)) | value,(volatile void *)reg_base);
		len -= 12; index += 3;
	}


	printf("[%s]\n", __func__);


}

static int ehci_usb_probe(struct udevice *dev)
{
	printf("%s\n", __func__);

	struct uboot_ehci_nvt_priv_data *ctx = dev_get_priv(dev);

	ehci_nvt_init((void *)dev_get_addr(dev), &ctx->hccr , &ctx->hcor, dev);


	printf("%s hccr 0x%p hcor 0x%p\n", __func__, ctx->hccr, ctx->hcor);
	ctx->ctrl.rootdev = 0;

	ehci_nvt_vbus(dev);

	return ehci_register(dev, ctx->hccr, ctx->hcor, &nvt_ehci_ops, 0, USB_INIT_HOST);
}

static int ehci_usb_remove(struct udevice *dev)
{
	int ret;
	printf("%s\n", __func__);
	phy_init = 0;

	ret = ehci_deregister(dev);
	if (ret)
		return ret;

	return 0;
}

static const struct udevice_id ehci_usb_ids[] = {
	{ .compatible = "nvt,NT72668-ehci" },
	{ }
};

U_BOOT_DRIVER(usb_ehci) = {
	.name	= "ehci_nvt",
	.id	= UCLASS_USB,
	.of_match = ehci_usb_ids,
	.ofdata_to_platdata = ehci_usb_ofdata_to_platdata,
	.probe = ehci_usb_probe,
	.remove = ehci_usb_remove,
	.ops	= &ehci_usb_ops,
	.priv_auto_alloc_size = sizeof(struct uboot_ehci_nvt_priv_data),
//	.platdata_auto_alloc_size = sizeof(struct exynos_ehci_platdata),
	.flags	= DM_FLAG_ALLOC_PRIV_DMA,
};
