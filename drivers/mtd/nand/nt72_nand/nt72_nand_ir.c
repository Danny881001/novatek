#include "include/nt72_nand.h"

void nt72_nand_ir_init(struct nt72_nand_info *info, const int busy_wait,
		       const u32 what)
{
	nfc_write32(REG_NFC_INTCTR, 0);
	/* REG_NFC_INITSTS: write 1 to clear */
	nfc_write32(REG_NFC_INTSTS, 0xFFFFFFFF);
#ifdef USE_INTERRUPT
	if (!busy_wait) {
		nt72_nand_init_comp(&info->nfc_completion);
		nfc_write32(REG_NFC_INTCTR, what);
	}
#endif
}

int nt72_nand_ir_wait(struct nt72_nand_info *info, const char *who,
		      const u32 what)
{
#ifdef USE_INTERRUPT
	int ret;

	ret = nt72_nand_wait_for_comp(&info->nfc_completion, INTERRUPT_TIMEOUT);
	if (!ret) {
		nfc_err("nfc_err: %s wait interrupt timeout\n", who);
		return ERR_CTRL_TIMEOUT;
	}

	return ERR_NONE;
#else
	return nt72_nand_ir_busy_wait(info, who, what);
#endif
}

int nt72_nand_ir_busy_wait(struct nt72_nand_info *info, const char *who,
			   const u32 what)
{
	unsigned long stop_time = nt72_nand_timeout_init();

#ifdef SELFTEST_REG_POLLING_TIMEOUT
	int counter = 1;
#endif

	while (!(nfc_read32(REG_NFC_INTSTS) & what)) {
#ifdef SELFTEST_REG_POLLING_TIMEOUT
		if ((counter % 1000) == 0)
			ssleep(REG_POLLING_TIMEOUT / HZ * 2);
#endif
		if (nt72_nand_timeout_check(stop_time)) {
			nfc_err("nfc_err: %s polling int status timeout\n",
				who);
			goto has_error;
		}
		nt72_nand_cpu_relax();
	}

	return ERR_NONE;
has_error:
	return ERR_CTRL_TIMEOUT;
}
