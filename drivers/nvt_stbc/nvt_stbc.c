/*
 *  board/novatek/evb658/nt72658_stbc.c
 *
 *  Author:	Alvin lin
 *  Created:	April 29, 2013
 *  Copyright:	Novatek Inc.
 *
 */
#include <common.h>
#include <asm/arch/nvt_stbc_regs.h>
#include <nvt_stbc.h>
#include <errno.h>


void nvt_stbc_unlock_reg(void)
{
	if(!(REG_STBC_KEYPASS & STBC_KEY_PASS_EN)) {
		REG_STBC_PATT = STBC_PATT_0;
		REG_STBC_PATT = STBC_PATT_1;
	}
}

void nvt_stbc_set_keypass(int enable)
{
	nvt_stbc_unlock_reg();

	if(enable)
		REG_STBC_KEYPASS |= STBC_KEY_PASS_EN;
	else
		REG_STBC_KEYPASS &= ~STBC_KEY_PASS_EN;
}


static int nvt_stbc_wait_response(void)
{
	unsigned long	tmo, out;

	tmo = get_timer(0);
	out = tmo + (1000/5) ; 	/*timeout 0.2 s*/

	while(get_timer(0) < out) {
		if(REG_STBC_INT_STAT & STBC_IPC_UPDATE_INT_STATUS) {
			REG_STBC_INT_STAT |= STBC_IPC_UPDATE_INT_STATUS;
			return 1;
		}
	}

	printf("%s timeout !\n",__func__);
	return 0;
}


void nvt_stbc_stay_in_sram(void)
{
	unsigned int reg_val __attribute__((unused));

	nvt_stbc_unlock_reg();

	REG_STBC_DOWNLOAD = (KER_STBC_IPC_CMD_SYS_STBC_IDLE_IN_SRAM << STBC_CMD_SHIFT);

	if(nvt_stbc_wait_response() == 0) {
		printf("stbc stay in sram command timeout !\n");
		return;
	}

	reg_val = REG_STBC_UPLOAD;
}

void nvt_stbc_exit_idle(void)
{
	nvt_stbc_unlock_reg();

	REG_STBC_DOWNLOAD = (KER_STBC_IPC_CMD_SYS_STBC_EXIT_IDLE << STBC_CMD_SHIFT);
}
//If singlely use this function, the system will be reseted.
//If use this function after nt72658_stbc_stay_in_sram, it not reset the system, only jump out of stay in sram
void nvt_stbc_reset(void)
{
	nvt_stbc_unlock_reg();

	REG_STBC_DOWNLOAD = (KER_STBC_IPC_CMD_RESET_STBC << STBC_CMD_SHIFT);

}

void nvt_stbc_lock_keypad(void)
{
	nvt_stbc_unlock_reg();
	REG_STBC_DOWNLOAD = (IPC_CMD_SYS_LOCK_KEYPAD << STBC_CMD_SHIFT);
	
	if(nvt_stbc_wait_response() == 0) {
		printf("stbc lock keypad command timeout !\n");
		return;
	}
}

void nvt_stbc_unlock_keypad(void)
{
	nvt_stbc_unlock_reg();
	REG_STBC_DOWNLOAD = (IPC_CMD_SYS_UNLOCK_KEYPAD << STBC_CMD_SHIFT);

	if(nvt_stbc_wait_response() == 0) {
		printf("stbc unlock keypad command timeout !\n");
		return;
	}
}

void nvt_stbc_shutdown_arm(void)
{
	nvt_stbc_unlock_reg();
	REG_STBC_DOWNLOAD = (KER_STBC_IPC_CMD_POWER_OFF << STBC_CMD_SHIFT);

	if(nvt_stbc_wait_response() == 0) {
		printf("stbc shutdown arm command timeout !\n");
		return;
	}
}


void nvt_stbc_enable_wdog(unsigned short sec)
{
	unsigned short msec = sec * 1000;
	unsigned int reg_val __attribute__((unused));

	nvt_stbc_unlock_reg();
	REG_STBC_INT_STAT |= STBC_IPC_UPDATE_INT_STATUS;
	REG_STBC_DOWNLOAD  = (KER_STBC_IPC_CMD_ENABLE_WATCHDOG_TIMER << STBC_CMD_SHIFT) | (msec << STBC_WDOG_TIME_SHIFT);

	if(nvt_stbc_wait_response() == 0) {
		printf("enable wdog no response !\n");
		return;
	}

	reg_val = REG_STBC_UPLOAD;


}

void nvt_stbc_disable_wdog(void)
{
	unsigned int reg_val __attribute__((unused));

	nvt_stbc_unlock_reg();
	REG_STBC_INT_STAT |= STBC_IPC_UPDATE_INT_STATUS;
	REG_STBC_DOWNLOAD  = (KER_STBC_IPC_CMD_ENABLE_WATCHDOG_TIMER << STBC_CMD_SHIFT) | (0 << STBC_WDOG_TIME_SHIFT);

	if(nvt_stbc_wait_response() == 0) {
		printf("disable wdog no response !\n");
		return;
	}

	reg_val = REG_STBC_UPLOAD;

}

unsigned int nvt_stbc_read_power_on_event(void)
{
	unsigned int reg_val;

	nvt_stbc_unlock_reg();

	REG_STBC_INT_STAT |= STBC_IPC_UPDATE_INT_STATUS;
	REG_STBC_DOWNLOAD = (KER_STBC_IPC_CMD_GET_POWER_ON_EVENT << STBC_CMD_SHIFT);

	if(nvt_stbc_wait_response() == 0) {
		printf("disable wdog no response !\n");
		return 0xFF;
	}

	reg_val = REG_STBC_UPLOAD;

	reg_val >>= STBC_DATA_SHIFT;

	reg_val &= STBC_CMD_DATA_MASK;

	switch(reg_val)
	{
		case EN_DRV_STBC_POWER_ON_EVENT_AC_POWER_TO_ON:
			printf("Power on AC!\n");
			break;

		case EN_DRV_STBC_POWER_ON_EVENT_RC_PWR_KEY:
			printf("Power on IR!\n");
			break;

		case EN_DRV_STBC_POWER_ON_EVENT_FP_PWR_KEY:
			printf("Power on Key pad!\n");
			break;

		case EN_DRV_STBC_POWER_ON_EVENT_WAKE_UP_TIME_MATCH:
			printf("Power on Alarm time match!\n");
			break;

		case EN_DRV_STBC_POWER_ON_EVENT_CEC_POWER_ON:
			printf("Power on CEC!\n");
			break;

		case EN_DRV_STBC_POWER_ON_EVENT_SCART_PIN8_EXT1:
			printf("Power on SCART wake up!\n");
			break;

		case EN_DRV_STBC_POWER_ON_EVENT_WDT_RESET:
			printf("Power on watchdog time out!\n");
			break;

        case EN_DRV_STBC_POWER_ON_EVENT_RESET:
            printf("Power on reboot!\n");
            break;

        case EN_DRV_STBC_POWER_ON_EVENT_LONG_PRESS_KEYPAD_RESET:
            printf("Power on long press Key pad!\n");
            break;

		default:
			printf("Power on unknown event 0x%x!\n", reg_val);
			break;
	}

	return reg_val;
}

int do_nvt_reboot(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	nvt_stbc_reset();

	return 0;
}

U_BOOT_CMD(
	reboot, 3, 1, do_nvt_reboot,
	"reboot - reboot system",
	"       reboot system\n"
);


int do_nvt_halt(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	nvt_stbc_shutdown_arm();

	return 0;
}

U_BOOT_CMD(
	halt, 3, 1, do_nvt_halt,
	"halt - halt system",
	"       halt system\n"
);


void nvt_stbc_sar_adc_init(void)
{
	//init sar adc0,1 as input
	nvt_stbc_unlock_reg();
	REG_SAR_ADC_MUX |= (NVT_SAR_ADC0_SFT | NVT_SAR_ADC1_SFT);

	nvt_stbc_unlock_reg();
	REG_SAR_ADC_DIR &= ~(NVT_SAR_ADC0_SFT | NVT_SAR_ADC1_SFT);
	
}

unsigned int nvt_stbc_get_clock_divider(void)
{
	unsigned int reg_val;

	nvt_stbc_unlock_reg();

	REG_STBC_INT_STAT |= STBC_IPC_UPDATE_INT_STATUS;
	REG_STBC_DOWNLOAD = (KER_STBC_IPC_CMD_GET_STBC_REGISTER << STBC_CMD_SHIFT) | 0xF1BF00;

	if(nvt_stbc_wait_response() == 0) {
		printf("disable wdog no response !\n");
		return 0xFF;
	}

	reg_val = REG_STBC_UPLOAD;

	reg_val >>= STBC_DATA_SHIFT;

	reg_val &= STBC_CMD_DATA_MASK;

	if(reg_val == EN_CLOCK_DIVIDER_12M)
		printf("\nSTBC clock is 12MHz\n");
	else if(reg_val == EN_CLOCK_DIVIDER_6M)
		printf("\nSTBC clock is 6MHz\n");
	else if(reg_val ==EN_CLOCK_DIVIDER_3M)
		printf("\nSTBC clock is 3MHz\n");

	return reg_val;

}

void nvt_stbc_set_gpio(u8 pin, EN_GPIO_CTRL enCtrl)
{
	nvt_stbc_unlock_reg();

	REG_STBC_INT_STAT |= STBC_IPC_UPDATE_INT_STATUS;
	REG_STBC_DOWNLOAD = (KER_STBC_IPC_CMD_SET_GPIO << STBC_CMD_SHIFT) | (pin << STBC_DATA_SHIFT) | (enCtrl << STBC_WDOG_TIME_SHIFT);

	if(nvt_stbc_wait_response() == 0) {
		printf("Set gpio no response !\n");
		return;
	}
	return;
}




