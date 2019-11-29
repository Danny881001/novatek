
#if defined(CONFIG_ARCH_NT72673_A32) || defined(CONFIG_ARCH_NT72673_A64) || defined(CONFIG_ARCH_NT72671_A32) || defined(CONFIG_ARCH_NT72671_A64)
#define CLK_BASE						0xfd022000

#define USB0_EHCI_BASE                  0XFC1F0000
#define USB1_EHCI_BASE                  0XFC000000
#define USB2_EHCI_BASE                  0xFC1A0000
#define USB3_EHCI_BASE                  0xFC350000


#define NVT_USB0_APB_BASE				(0xFD170000)
#define NVT_USB1_APB_BASE				(0xFD174000)
#define NVT_USB2_APB_BASE				(0xFD184000)
#define NVT_USB3_APB_BASE				(0xFD180000)
#define NVT_USB30_APB_BASE				(0xFD188000)

#define NVT_USB30_HS_PHY_BASE			(0xFD189000)
#define NVT_USB30_SS_PHY_BASE			(0xFD18C000)

#else
#define CLK_BASE						0xfd020000

#define USB0_EHCI_BASE                  0XFC1F0000
#define USB1_EHCI_BASE                  0XFC000000
#define USB2_EHCI_BASE                  0xFC140000
#define USB3_EHCI_BASE                  0xFC1A0000


#define NVT_USB0_APB_BASE				(0xFD170000)
#define NVT_USB1_APB_BASE				(0xFD178000)
#define NVT_USB2_APB_BASE				(0xFD180000)
#define NVT_USB3_APB_BASE				(0xFD184000)
#endif

// clk base address
#define AHB_CLK_RESET_ENABLE_USB_0_1	0xfd020094
#define AHB_CLK_RESET_ENABLE_USB1_P		(1<<11)
#define AHB_CLK_RESET_ENABLE_USB1_H		(1<<10)
#define AHB_CLK_RESET_ENABLE_USB0_P		(1<<9)
#define AHB_CLK_RESET_ENABLE_USB0_H		(1<<8)
#define AHB_CLK_RESET_DISABLE_USB_0_1	0xfd020090
#define AHB_CLK_RESET_DISABLE_USB1_P	(1<<11)
#define AHB_CLK_RESET_DISABLE_USB1_H	(1<<10)
#define AHB_CLK_RESET_DISABLE_USB0_P	(1<<9)
#define AHB_CLK_RESET_DISABLE_USB0_H	(1<<8)

#define AHB_CLK_RESET_ENABLE_USB_2_3	0xfd0200a4
#define AHB_CLK_RESET_ENABLE_USB3_P		(1<<13)
#define AHB_CLK_RESET_ENABLE_USB2_P		(1<<12)
#define AHB_CLK_RESET_ENABLE_USB3_H		(1<<11)
#define AHB_CLK_RESET_ENABLE_USB2_H		(1<<10)
#define AHB_CLK_RESET_DISABLE_USB_2_3	0xfd0200a0
#define AHB_CLK_RESET_DISABLE_USB3_P	(1<<13)
#define AHB_CLK_RESET_DISABLE_USB2_P	(1<<12)
#define AHB_CLK_RESET_DISABLE_USB3_H	(1<<11)
#define AHB_CLK_RESET_DISABLE_USB2_H	(1<<10)

#define AXI_CLK_RESET_ENABLE_USB_0_1	0xfd020074
#define AXI_CLK_RESET_ENABLE_USB1		(1<<7)
#define AXI_CLK_RESET_ENABLE_USB0		(1<<6)
#define AXI_CLK_RESET_DISABLE_USB_0_1	0xfd020070
#define AXI_CLK_RESET_DISABLE_USB1		(1<<7)
#define AXI_CLK_RESET_DISABLE_USB0		(1<<6)

#define AXI_CLK_RESET_ENABLE_USB_2_3	0xfd020084
#define AXI_CLK_RESET_ENABLE_USB3		(1<<3)
#define AXI_CLK_RESET_ENABLE_USB2		(1<<2)
#define AXI_CLK_RESET_DISABLE_USB_2_3	0xfd020080
#define AXI_CLK_RESET_DISABLE_USB3		(1<<3)
#define AXI_CLK_RESET_DISABLE_USB2		(1<<2)

#define CORE_CLK_RESET_ENABLE_0_1		0xfd020054
#define CORE_CLK_RESET_ENABLE_USB1		(1<<9)
#define CORE_CLK_RESET_ENABLE_USB0		(1<<8)
#define CORE_CLK_RESET_DISABLE_0_1		0xfd020050
#define CORE_CLK_RESET_DISABLE_USB1		(1<<9)
#define CORE_CLK_RESET_DISABLE_USB0		(1<<8)

#define CORE_CLK_RESET_ENABLE_2_3		0xfd020064
#define CORE_CLK_RESET_ENABLE_USB3		(1<<5)
#define CORE_CLK_RESET_ENABLE_USB2		(1<<4)
#define CORE_CLK_RESET_DISABLE_2_3		0xfd020060
#define CORE_CLK_RESET_DISABLE_USB3		(1<<5)
#define CORE_CLK_RESET_DISABLE_USB2		(1<<4)

#define ENGINE_CLK_SOURCE				0xfd020048
#define ENGINE_CLK_SOURCE_USB_0_1_PHY_12M (1<<27)
#define ENGINE_CLK_SOURCE_USB_2_3_PHY_12M (1<<22)



#define clear(add,wValue)    writel(~(wValue)&readl(add),add)
#define set(add,wValue)      writel(wValue|readl(add),add)

#define setMode 		(1<<4|1<<5|1<<10|1<<8|1<<9|1<<11|1<<12)
#define clearMode 		(1|1<<5|1<<10|1<<8|1<<9)



typedef enum _EN_SYS_CLK_RST
{
	EN_SYS_CLK_RST_CORE_USB20_U0,
	EN_SYS_CLK_RST_CORE_USB20_U1,
	EN_SYS_CLK_RST_CORE_USB20_U2,
	EN_SYS_CLK_RST_CORE_USB20_U3,

	EN_SYS_CLK_RST_AXI_USB20_U0,
	EN_SYS_CLK_RST_AXI_USB20_U1,
	EN_SYS_CLK_RST_AXI_USB20_U2,
	EN_SYS_CLK_RST_AXI_USB20_U3,

	EN_SYS_CLK_RST_AHB_USB20_U0,
	EN_SYS_CLK_RST_AHB_USB20_U1,
	EN_SYS_CLK_RST_AHB_USB20_U2,
	EN_SYS_CLK_RST_AHB_USB20_U3,

	EN_SYS_CLK_RST_AHB_USB20_U0_PCLK,
	EN_SYS_CLK_RST_AHB_USB20_U1_PCLK,
	EN_SYS_CLK_RST_AHB_USB20_U2_PCLK,
	EN_SYS_CLK_RST_AHB_USB20_U3_PCLK,

	EN_SYS_CLK_RST_AHB_USB30,
	EN_END,
}EN_SYS_CLK_RST;

struct usb_clk_table{
	unsigned int RegOff;
	unsigned int bit;
	unsigned int index;
};

struct usb_clk_table_freestyle{
	unsigned int RegOff_rst;
	unsigned int mask_rst;
	unsigned int val_rst;
	unsigned int RegOff_clr;
	unsigned int mask_clr;
	unsigned int val_clr;
	unsigned int index;
};

struct uboot_ehci_nvt_priv_data{
	struct ehci_ctrl ctrl; /* Needed by EHCI */
	struct ehci_hccr *hccr;
	struct ehci_hcor *hcor;
};
