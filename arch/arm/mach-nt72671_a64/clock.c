/*
 * Copyright (C) 2014 Novatek Microelectronics Corp. All rights reserved.
 * Author: SP-KSW <SP_KSW_MailGrp@novatek.com.tw>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <asm/io.h>
#include <asm/arch/clock.h>
#include <asm/arch/nvt_mpll.h>

unsigned long gaclk = 0;	// ARM frequecy
unsigned long gemmc_clk = 0;	// emmc frequecy (Hz)
#define FREQ_CPU		50000

unsigned long get_nvt_chip_type(void);

inline void apll_set_data(u8 offset, u8 value)
{
	writel(value, ((u64)(APLL_BASE_ADDR + (offset * 4))));
}

inline u32 apll_get_data(u8 offset)
{
	return readl((u64)((APLL_BASE_ADDR + (offset * 4))));
}

inline void apll_enable(pll_page_t page)
{
	if (PLL_PAGE_0 == page) {
		writel(APLL_PAGE_0_EN, ((u64)(APLL_PAGE_EN_ADDR)));
	}
	else if (PLL_PAGE_B == page) {
		writel(APLL_PAGE_B_EN, ((u64)(APLL_PAGE_EN_ADDR)));
	}
	else {
		/* ignore */
	}
}

inline void mpll_set_data(u8 offset, u8 value)
{
	writel(value, ((u64)(MPLL_BASE_ADDR + (offset * 4))));
}

inline u32 mpll_get_data(u8 offset)
{
	return readl((u64)((MPLL_BASE_ADDR + (offset * 4))));
}

inline void mpll_enable(pll_page_t page)
{
	if (PLL_PAGE_0 == page) {
		writel(MPLL_PAGE_0_EN, (u64)(MPLL_PAGE_EN_ADDR));
	}
	else if (PLL_PAGE_B == page) {
		writel(MPLL_PAGE_B_EN, (u64)(MPLL_PAGE_EN_ADDR));
	}
	else {
		/* ignore */
	}
}

void set_sys_mpll(unsigned long off, unsigned long val)
{
	val <<= 17; val  /= 12;

	mpll_enable(PLL_PAGE_B);

	mpll_set_data((off + 0), ((val >> 0) & 0xff));
	mpll_set_data((off + 1), ((val >> 8) & 0xff));
	mpll_set_data((off + 2), ((val >> 16) & 0xff));
}

unsigned long get_sys_mpll(unsigned long off)
{
	unsigned long val;

	mpll_enable(PLL_PAGE_B);

	val  = mpll_get_data(off);
	val |= (mpll_get_data((off + 1)) << 8);
	val |= (mpll_get_data((off + 2)) << 16);

	val *= 12;val += ((1UL << 17) - 1); val >>= 17;

	return val;
}

void set_cpu_clk(unsigned long freq)
{
	/* no implement */
}

unsigned long get_cpu_clk(void)
{

    unsigned long aclk = 0;	/* ARM frequency (KHz) */

    if(get_nvt_chip_type() == 1 ){
        aclk = get_sys_mpll(CPU_PLL_OFFSET);

        apll_enable(PLL_PAGE_0);
        if (apll_get_data(0x00) & 0x01) {
            aclk <<= 3;
        }

        /* the unit of value is KHz */
        return ((aclk) ? (aclk * 1000) : (CONFIG_CPU_FREQ * 1000));
    }
    else
        return FREQ_CPU;

}

unsigned long get_arm_clk(void)
{
	gaclk = get_sys_mpll(_CPU_PLL_OFFSET);

	_APLL_EnablePage0();
	if(_APLL_GetData(0x00) & 0x1)
		gaclk *= 8;

	return ((gaclk) ? (gaclk *1000):(FREQ_CPU * 1000));
}
unsigned long get_nvt_chip_id(void)
{
	return readl((CLK_BASE_ADDR + CHIP_ID_OFFSET))& CHIP_ID_MASK;
}

unsigned long get_nvt_chip_type(void)
{
	if  ( (readl((CLK_BASE_ADDR + CHIP_ID_OFFSET))& CHIP_TYPE_MASK) != CHIP_TYPE_FPGA){
		
#define _MMC_PLL_OFFSET	0xc4
		unsigned long emmc_clk = get_sys_mpll(_MMC_PLL_OFFSET);

		if(emmc_clk == 0){
			return 0;//fpga workaournd case
		}else{
			return 1;
		}
	}else {//fpga
	return 0;
	}

}

/**
 * @brief Get frequency of AHB clock
 *
 * @return Frequency (Hz) of AHB clock
 */
unsigned long get_ahb_clk(void)
{
	unsigned long ratio = AHB_CLK_SEL;
	unsigned long hclk;

  
	if((ratio == 0))
	{	
		//!< Source is OSC16X/2
		hclk = 96000000;	//!< 3'bx00: OSC16X/2
	}
	else if(ratio == 1)
	{
		//!< Source is ARM_D8CK
		hclk = get_arm_clk();
		hclk *= 1000;
		hclk /= 8;
	}
	else //!< if((ratio == 2) || (ratio == 3))
	{
		//!< Source is AHB_CK
		hclk = get_sys_mpll(_AHB_PLL_OFFSET);
		hclk *= 1000000;
	}

	return hclk;
}
/**
 * @brief Get frequency of AHB clock
 *
 * @return Frequency (Hz) of AHB clock
 */

/**
 * @brief Get frequency of emmc clock
 *
 * @return Frequency (Hz) of emmc clock
 */

unsigned long get_emmc_clk(void)
{
	extern unsigned long get_nvt_chip_type(void);

	if(get_nvt_chip_type() == 1 ){
		gemmc_clk = get_sys_mpll(_MMC_PLL_OFFSET);
		gemmc_clk *= 1000000;
		gemmc_clk *= 4;	///< emmc MPLL = emmc MPLL ratio X 4
	}else{
		gemmc_clk = 12000000;
	}

	return gemmc_clk;
}
unsigned long set_emmc_clk(unsigned long freq)
{
	freq /=1000000;
	freq /=4;
	set_sys_mpll(_MMC_PLL_OFFSET, freq);
	return get_emmc_clk();
}

static unsigned long axi_clk, arm_clk ,ddr_clk;

#if 0
static void compute_ahb_clk(void)
{
	if(get_sys_mpll(0x6c))	///< Real Chip
	{
		switch(_AHB_CLK_SEL)
		{
			case EN_SYS_AHB_CLK_SRC_REF_96M:	///< OSC16X/2
				ahb_clk = 96000000;

				break;

			case EN_SYS_AHB_CLK_SRC_ARM_D8:		///< ARM_D8CK
				ahb_clk = (arm_clk / 8);

				break;

			case EN_SYS_AHB_CLK_SRC_AHB:		///< AHB_CK
			default:
				ahb_clk = SYS_CLK_GetMpll(EN_MPLL_OFF_AHB);
				ahb_clk *= 1000000;

				break;
		}
	}
	else
	{
		ahb_clk = 12000000;
	}
}
#endif

static void compute_axi_clk(void)
{
	if(get_sys_mpll(0x6c))	///< Real Chip
	{
		switch(_AXI_CLK_SEL)
		{
			case EN_SYS_AXI_CLK_SRC_ARM_D8:	///< ARM_D8CK
				axi_clk = (arm_clk / 8);

				break;

			case EN_SYS_AXI_CLK_SRC_DDR_D2:	///< DDR_D2CK
				axi_clk = (get_sys_mpll(EN_MPLL_OFF_DDR) / 2);;
				axi_clk *= 1000000;

				break;

			case EN_SYS_AXI_CLK_SRC_AXI_D2:	///< AXI_CLK/2
			case EN_SYS_AXI_CLK_SRC_AXI: 	///< AXI_CLK
				axi_clk = (get_sys_mpll(EN_MPLL_OFF_AXI) / (4 - _AXI_CLK_SEL));
				axi_clk *= 1000000;

				break;

			default:
				printf("Invalid AXI clock selection\n");
				break;
		}
	}
	else
	{
		axi_clk = 27000000;
	}
}

#if 0 //unusage
static void compute_cpu_clk(void)
{

	if(get_sys_mpll(0x6c))	///< Real Chip
	{
		/**
		 * Get ratio of ARM PLL
		 */
		arm_clk = get_sys_mpll(EN_MPLL_OFF_ARM);

		/**
		 * Check MUX
		 */
		_APLL_EnablePage0();
		if((_APLL_GetData(0x00) & 0x1))	///< Select local PLL
		{
			printf("\tSelect local PLL\n");
			arm_clk *= 8;
		}
		else
		{
			printf("\tSelect MPLL\n");
		}

		arm_clk *= 1000000;
	}
	else
	{
		arm_clk = 55000000;
	}
}
#endif

unsigned long get_axi_clk(void)
{
	if(axi_clk == 0)
		compute_axi_clk();

	return axi_clk;
}

unsigned long get_ddr_clk(void)
{
	if (ddr_clk == 0)
		ddr_clk = get_sys_mpll(_DDR_PLL_OFFSET);

	return (ddr_clk << 2);
}
