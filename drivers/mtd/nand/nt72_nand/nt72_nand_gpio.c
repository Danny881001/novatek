#include "include/nt72_nand.h"

#ifdef SHARE_GPIO_WITH_CI
void nt72_nand_switch_io(struct nt72_nand_info *info)
{
	if (info->io_param.type == ARM_CI_IO) {
		u32 _reg;

		/* If nand_params is not initialized, read the ic_id */
		if (info->nand_params.ic_id == 0) {
			u32 low_id = clkgen_read32(REG_CLK_GEN_IC_ID) & 0xFFFF;
			u32 ext_id = (clkgen_read32(REG_CLK_GEN_IC_ID) >> 24) & 0xF;
			info->nand_params.ic_id = low_id | (ext_id << 16);
		}

		if (info->nand_params.ic_id == 0x72673) {
			/* GPIO direction: GPC_29 ~ GPC_31, GPD_0 ~ GPD_11
			* Set to input to avoid mystical issues.
			*/
			_reg = gp_read32(REG_GPC_IODCR);
			nvt_bclear(_reg, 0xe0000000);
			gp_write32(REG_GPC_IODCR, _reg);

			_reg = gp_read32(REG_GPD_IODCR);
			nvt_bclear(_reg, 0x00000fff);
			gp_write32(REG_GPD_IODCR, _reg);

			/* GPIO function select: GPC_29 ~ GPC_31 */
			_reg = gp_read32(REG_GPC_MUX_SEL0);
			nvt_bclear(_reg, 0xfff00000);
			nvt_bset(_reg, 0x55500000);
			gp_write32(REG_GPC_MUX_SEL0, _reg);

			/* GPIO function select: GPD_0 ~ GPD_11 */
			_reg = gp_read32(REG_GPD_MUX_SEL0);
			nvt_bclear(_reg, 0xffffffff);
			nvt_bset(_reg, 0x55555555);
			gp_write32(REG_GPD_MUX_SEL0, _reg);

			_reg = gp_read32(REG_GPD_MUX_SEL1);
			nvt_bclear(_reg, 0x0000ffff);
			nvt_bset(_reg, 0x00005555);
			gp_write32(REG_GPD_MUX_SEL1, _reg);

		} else {
			/* GPIO direction: GPB_2 ~ GPB_16
			* Set to input to avoid mystical issues
			*/
			_reg = gp_read32(REG_GPB_IODCR);
			nvt_bclear(_reg, GPB_NAND_MASK);
			gp_write32(REG_GPB_IODCR, _reg);

			/* GPIO function select: GPB_2 ~ GPB_7 */
			_reg = gp_read32(REG_GPB_MUX_SEL0);
			_reg &= 0x000000FF;
			_reg |= 0x77777700;
			gp_write32(REG_GPB_MUX_SEL0, _reg);

			/* GPIO function select: GPB_8 ~ GPB_15 */
			gp_write32(REG_GPB_MUX_SEL1, 0x77777777);

			/* GPIO function select: GPB_16 */
			_reg = gp_read32(REG_GPB_MUX_SEL2);
			_reg &= 0xFFFFFFF0;
			_reg |= 0x00000007;
			gp_write32(REG_GPB_MUX_SEL2, _reg);
		}
	}
}
#endif

int nt72_nand_check_gpio(struct nt72_nand_info *info)
{
#ifdef SHARE_GPIO_WITH_CI
	int ret = 0;

	if (info->io_param.type == ARM_CI_IO) {
		u32 _reg;

		if (info->nand_params.ic_id == 0x72673) {
			/* GPIO function select: GPC_29 ~ GPC_31 */
			_reg = gp_read32(REG_GPC_MUX_SEL0);
			if (unlikely((_reg & 0xfff00000) != 0x55500000)) {
				nfc_err("nfc_err: gpc_mux0: 0x%08X\n", _reg);
				ret = 1;
			}

			/* GPIO function select: GPD_0 ~ GPD_11 */
			_reg = gp_read32(REG_GPD_MUX_SEL0);
			if (unlikely((_reg & 0xffffffff) != 0x55555555)) {
				nfc_err("nfc_err: gpd_mux1: 0x%08X\n", _reg);
				ret = 1;
			}
			_reg = gp_read32(REG_GPD_MUX_SEL1);
			if (unlikely((_reg & 0x0000ffff) != 0x00005555)) {
				nfc_err("nfc_err: gpd_mux1: 0x%08X\n", _reg);
				ret = 1;
			}
#if 0
			/* GPIO direction: GPC_29 ~ GPC_31 */
			_reg = gp_read32(REG_GPC_IODCR);
			if (unlikely((_reg & 0xe0000000) != 0x00))
				nfc_err("nfc_err: gpc_dir: 0x%08X\n", _reg);
			/* GPIO direction: GPD_0 ~ GPD_11 */
			_reg = gp_read32(REG_GPD_IODCR);
			if (unlikely((_reg & 0x00000fff) != 0x00))
				nfc_err("nfc_err: gpd_dir: 0x%08X\n", _reg);

			/* GPIO set enable: GPC_29 ~ GPC_31 */
			_reg = gp_read32(REG_GPC_SET_EN);
			if (unlikely((_reg & 0xe0000000) != 0x00))
				nfc_err("nfc_err: gpc_set_en: 0x%08X\n", _reg);

			/* GPIO set enable: GPD_0 ~ GPD_11 */
			_reg = gp_read32(REG_GPD_SET_EN);
			if (unlikely((_reg & 0x00000fff) != 0x00))
				nfc_err("nfc_err: gpd_set_en: 0x%08X\n", _reg);

			/* GPIO set clear: GPC_29 ~ GPC_31 */
			_reg = gp_read32(REG_GPC_SET_CLR);
			if (unlikely((_reg & 0xe0000000) != 0x00))
				nfc_err("nfc_err: gpc_clear: 0x%08X\n", _reg);
			/* GPIO set clear: GPD_0 ~ GPD_11 */
			_reg = gp_read32(REG_GPD_SET_CLR);
			if (unlikely((_reg & 0x00000fff) != 0x00))
				nfc_err("nfc_err: gpd_clear: 0x%08X\n", _reg);
#endif
		} else {

			/* GPIO function select: GPB_2 ~ GPB_7 */
			_reg = gp_read32(REG_GPB_MUX_SEL0);
			if (unlikely((_reg & 0xFFFFFF00) != 0x77777700)) {
				nfc_err("nfc_err: gpb_sel0: 0x%08X\n", _reg);
				ret = 1;
			}

			/* GPIO function select: GPB_8 ~ GPB_15 */
			_reg = gp_read32(REG_GPB_MUX_SEL1);
			if (unlikely(_reg != 0x77777777)) {
				nfc_err("nfc_err: gpb_sel1: 0x%08X\n", _reg);
				ret = 1;
			}

			/* GPIO function select: GPB_16 */
			_reg = gp_read32(REG_GPB_MUX_SEL2);
			if (unlikely((_reg & 0x0F) != 0x07)) {
				nfc_err("nfc_err: gpb_sel2: 0x%08X\n", _reg);
				ret = 1;
			}
#if 0
			/* GPIO direction: GPB_2 ~ GPB_16 */
			_reg = gp_read32(REG_GPB_IODCR);
			if (unlikely((_reg & GPB_NAND_MASK) != 0x00))
				nfc_err("nfc_err: gpb_dir: 0x%08X\n", _reg);

			/* GPIO set enable: GPB_2 ~ GPB_16 */
			_reg = gp_read32(REG_GPB_SET_EN);
			if (unlikely((_reg & GPB_NAND_MASK) != 0x00))
				nfc_err("nfc_err: gpb_set_en: 0x%08X\n", _reg);

			/* GPIO set clear: GPB_2 ~ GPB_16 */
			_reg = gp_read32(REG_GPB_SET_CLR);
			if (unlikely((_reg & GPB_NAND_MASK) != 0x00))
				nfc_err("nfc_err: gpb_clear: 0x%08X\n", _reg);
#endif
		}
	}

	return ret;
#else
	return 0;
#endif
}

int nt72_nand_gpio_inited(void)
{
#ifdef SHARE_GPIO_WITH_CI
	if (likely(g_nfc_ready))
		return 0;
	return -1;
#else
	return 0;
#endif
}
#ifdef SHARE_GPIO_WITH_CI
EXPORT_SYMBOL(nt72_nand_gpio_inited);
#endif

int nt72_nand_gpio_lock(void)
{
#ifdef SHARE_GPIO_WITH_CI
	return down_interruptible(&g_sem_nfc);
#else
	return 0;
#endif
}
#ifdef SHARE_GPIO_WITH_CI
EXPORT_SYMBOL(nt72_nand_gpio_lock);
#endif

void nt72_nand_gpio_unlock(void)
{
#ifdef SHARE_GPIO_WITH_CI
	up(&g_sem_nfc);
#endif
}
#ifdef SHARE_GPIO_WITH_CI
EXPORT_SYMBOL(nt72_nand_gpio_unlock);
#endif
