/*
 *  arch/arm/include/asm/arch-nt72670b/nvt_stbc_regs.h
 *
 *  Author:	Alvin lin
 *  Created:	April 26, 2013
 *  Copyright:	Novatek Inc.
 *
 */

#ifndef __NVT_STBC_REGS_H__
#define __NVT_STBC_REGS_H__
#include <asm/types.h>
#include <asm/arch/hardware.h>

#define REG_STBC_BASE			0xFC040000
#define REG_STBC_DOWNLOAD		__REG(REG_STBC_BASE)
#define REG_STBC_UPLOAD		    __REG((REG_STBC_BASE + 4))
#define REG_STBC_INT_STAT		__REG((REG_STBC_BASE + 8))
#define REG_STBC_RTC_BASE_REG		__REG((REG_STBC_BASE + 0x100))
#define REG_STBC_RTC_SEC_REG 		__REG((REG_STBC_BASE + 0x104))
#define REG_STBC_RTC_DAY_REG 		__REG((REG_STBC_BASE + 0x108))
#define REG_STBC_RTC_CTRL_REG 		__REG((REG_STBC_BASE + 0x114))

#define STBC_CMD_SHIFT			24
#define STBC_DATA_SHIFT		    16
#define STBC_WDOG_TIME_SHIFT	8
#define STBC_CMD_DATA_MASK	    0xFF

#define KER_STBC_IPC_CMD_GET_POWER_ON_EVENT 	0x21
#define KER_STBC_IPC_CMD_POWER_OFF				0x31
#define KER_STBC_IPC_CMD_POWER_OFF_BY_KEYPAD	0xB0
#define KER_STBC_IPC_CMD_REPLY_POWER_ON_EVENT   0x31
#define KER_STBC_IPC_CMD_GET_STBC_VERSION		0xB5
#define KER_STBC_IPC_CMD_REPLY_STBC_VERSION		0xB6
#define KER_STBC_IPC_CMD_RESET_STBC 			0xC4

#define IPC_CMD_SYS_LOCK_KEYPAD   (0xA2)       //2014.07.22 for TCL to lock keypad
#define IPC_CMD_SYS_UNLOCK_KEYPAD (0xA3)

#define IPC_CMD_SYS_CEC_AUTO_ON   (0xA6)       //2014.08.12 for TCL CEC auto on/off
#define IPC_CMD_SYS_CEC_AUTO_OFF (0xA7)


#define KER_STBC_IPC_CMD_SAVE_WATCHDOG_SETTING          (0xAD)          //2012.0305
#define KER_STBC_IPC_CMD_ENABLE_WATCHDOG_TIMER          (0xAE)          //2012.0305
#define KER_STBC_IPC_CMD_RESET_WATCHDOG                         (0xAF)          //2012.0305

#define KER_STBC_IPC_CMD_GET_POWERKEY_IDX               0xE5
#define KER_STBC_IPC_CMD_REPLY_POWERKEY_IDX             0xE6
#define KER_STBC_IPC_CMD_SET_POWERKEY                   0xE7
#define KER_STBC_IPC_CMD_REPLY_SET_POWERKEY             0xE8
#define KER_STBC_IPC_CMD_SYS_STBC_IDLE_IN_SRAM          0xE9
#define KER_STBC_IPC_CMD_SYS_STBC_EXIT_IDLE          	0xEA

#define KER_STBC_IPC_CMD_GET_STBC_REGISTER		0x41

#define KER_STBC_IPC_CMD_SET_GPIO					 (0x40)

#define STBC_IPC_UPDATE_INT_STATUS			(0x00000100)

#define REG_STBC_PATT                   __REG(0xFC040204)
#define STBC_PATT_0                             0x72682
#define STBC_PATT_1                             0x28627

#define REG_STBC_KEYPASS 		__REG(0xFC040208)
#define STBC_KEY_PASS_EN		(1 << 0)

#define REG_STBC_SWITCH         __REG(0xFC040200)
#define STBC_NFC_OLD_TYPE                       (1 << 4)
#define STBC_STORAGE_CTRL_MODE  (1 << 5)

#define REG_STBC_SPI_MODE      __REG(0xFC040288)
#define SPI_NEW_DESIGN				(1 << 0)

#define GPIO_SWITCH_CTRL			__REG(0xFC040220)
#define GPIO_LED_CTRL_DIR			__REG(0xFC040430)
#define GPIO_LED_CTRL_LEVEL		    __REG(0xFC040424)
#define GPIO_LED_CTRL_SWITCH		__REG(0xFC04043C)
//#define GPIO_GROUP0_SWITCH_TO_MIPS		(1 << 9)
#define GPIO_GROUP1_SWITCH_TO_MIPS		    (1 << 10)
#define LEDR_DIR_OUT						(1 << 12)
#define LEDR_TURN_ON						(1 << 12)
#define LEDR_SWITCH_GPIO					(1 << 12)
#define LEDG_DIR_OUT						(1 << 22)
#define LEDG_TURN_ON						(1 << 22)
#define LEDG_SWITCH_GPIO					(1 << 22)
#define LED_DIR_OUT						(1 << 26)
#define LED_TURN_ON						(1 << 26)
#define LED_SWITCH_GPIO					(1 << 26)

#define SARADC0_VAL_REG			__REG(0xFC04030C)
#define SARADC1_VAL_REG			__REG(0xFC040310)
#define SARADC2_VAL_REG			__REG(0xFC040314)
#define SARADC3_VAL_REG			__REG(0xFC040318)
#define SARADC4_VAL_REG			__REG(0xFC04031C)
#define SARADC5_VAL_REG			__REG(0xFC040320)
#define SARADC6_VAL_REG			__REG(0xFC040324)
#define SARADC7_VAL_REG			__REG(0xFC040328)

#define STBC_RTC_RESET 		(1 << 0)
#define STBC_RTC_SEC_MSK 	0x3F
#define STBC_RTC_SEC_SFT 	0
#define STBC_RTC_MIN_MSK 	0x3F
#define STBC_RTC_MIN_SFT 	8
#define STBC_RTC_HOUR_MSK 	0xF
#define STBC_RTC_HOUR_SFT 	16

#define REG_SAR_ADC_MUX 		__REG(0xFC04043C)
#define REG_SAR_ADC_DIR 		__REG(0xFC040430)
#define REG_SAR_ADC_CTL 		__REG(0xFC040424)
#define NVT_SAR_ADC0_SFT 		(1 << 8)
#define NVT_SAR_ADC1_SFT 		(1 << 9)
#define NVT_SAR_ADC2_SFT 		(1 << 10)
#define NVT_SAR_ADC3_SFT 		(1 << 11)
#define NVT_SAR_ADC4_SFT 		(1 << 12)
#define NVT_SAR_ADC5_SFT 		(1 << 13)


#define REG_STBC_GPIO_AUTHORITY 	__REG(0xFC04041C)
#define LED_AUTHORITY_SFT 	        (1 << 0)

#define STBC_DEDICATED_GPIO_SWITCH_TO_ARM   (0x3FF)
#define STBC_ALTERNATIVE_GPIO_SWITCH_TO_ARM   (1 << 14)

#if defined(CONFIG_TCL668)
#define REG_LED_MUX 		__REG(0xFC040438)
#define REG_LED_DIR 		__REG(0xFC04042C)
#define REG_LED_CTL 		__REG(0xFC040420)

#define LED_SFT 			(1 << 6)
#define LED1_SFT 		    (1 << 18)
#define BL_SFT 			    (1 << 8)
#define BL_AUTHORITY_SFT 	(1 << 2)
#endif

#define PIN_GPF_0	0
#define PIN_GPF_1	1
#define PIN_GPF_2	2
#define PIN_GPF_3	3
#define PIN_GPF_4	4
#define PIN_GPF_5	5
#define PIN_GPF_6	6
#define PIN_GPF_7	7
#define PIN_GPF_8	8
#define PIN_GPF_9	9
#define PIN_GPF_10	10
#define PIN_GPF_11	11
#define PIN_GPF_12	12
#define PIN_GPF_13	13
#define PIN_GPF_14	14
#define PIN_GPF_15	15
#define PIN_GPF_16	16
#define PIN_GPF_17	17
#define PIN_GPF_18	18
#define PIN_GPF_19	19
#define PIN_GPF_20	20
#define PIN_GPF_21	21
#define PIN_GPF_22	22
#define PIN_GPF_23	23
#define PIN_GPF_24	24
#define PIN_GPF_25	25
#define PIN_GPF_26	26
#define PIN_GPF_27	27
#define PIN_GPF_28	28
#define PIN_GPF_29	29
#define PIN_GPF_30	30
#define PIN_GPF_31	31
#define PIN_GPG_0	32
#define PIN_GPG_1	33
#define PIN_GPG_2	34
#define PIN_GPG_3	35
#define PIN_GPG_4	36
#define PIN_GPG_5	37
#define PIN_GPG_6	38
#define PIN_GPG_7	39
#define PIN_GPG_8	40
#define PIN_GPG_9	41
#define PIN_GPG_10	42
#define PIN_GPG_11	43
#define PIN_GPG_12	44
#define PIN_GPG_13	45
#define PIN_GPG_14	46
#define PIN_GPG_15	47
#define PIN_GPG_16	48
#define PIN_GPG_17	49
#define PIN_GPG_18	50
#define PIN_GPG_19	51
#define PIN_GPG_20	52
#define PIN_GPG_21	53
#define PIN_GPG_22	54
#define PIN_GPG_23	55
#define PIN_GPG_24	56
#define PIN_GPG_25	57
#define PIN_GPG_26	58
#define PIN_GPG_27	59
#define PIN_GPG_28	60
#define PIN_GPG_29	61
#define PIN_GPG_30	62
#define PIN_GPG_31	63
#define PIN_GPI_0	64
#define PIN_GPI_1	65
#define PIN_GPI_2	66
#define PIN_GPI_3	67
#define PIN_GPI_4	68
#define PIN_GPI_5	69
#define PIN_GPI_6	70
#define PIN_GPI_7	71
#define PIN_GPI_8	72
#define PIN_GPI_9	73
#define PIN_GPI_10	74
#define PIN_GPI_11	75
#define PIN_GPI_12	76
#define PIN_GPI_13	77
#define PIN_GPI_14	78
#define PIN_GPI_15	79
#define PIN_GPI_16	80
#define PIN_GPI_17	81
#define PIN_GPI_18	82
#define PIN_GPI_19	83
#define PIN_GPI_20	84

#define GPIO_F_CTRL_DIR			__REG(0xFC04042C)
#define GPIO_F_CTRL_LEVEL		__REG(0xFC040420)
#define GPIO_F_CTRL_SWITCH		__REG(0xFC040438)

#define GPIO_G_CTRL_DIR			__REG(0xFC040430)
#define GPIO_G_CTRL_LEVEL		__REG(0xFC040424)
#define GPIO_G_CTRL_SWITCH		__REG(0xFC04043C)

#define GPIO_I_CTRL_DIR			__REG(0xFC040428)
#define GPIO_I_CTRL_LEVEL		__REG(0xFC040434)
#define GPIO_I_CTRL_SWITCH		__REG(0xFC040440)

#endif
