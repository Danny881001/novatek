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
#include "n330xx_led_stbc.h"

static bool n33004a_led_init(void)
{
	if((0xaa == ((*((volatile u32 *) (0xFC04011C))) & 0xFF)) || /* RC key update */
			(0xcc == ((*((volatile u32 *) (0xFC04011C))) & 0xFF)))
	{
		//If system run in ok key upgrade or OAD upgrade, uboot will not control LED status.
		//LED will be control by STBC.
		//
		printf("%s, init, don't ctl LED!!\n", __func__);
		return false;
	}

	nvt_stbc_unlock_reg();

	//REG_STBC_GPIO_AUTHORITY |= LED_AUTHORITY_SFT;
	GPIO_SWITCH_CTRL |= (1<<14);

	//config LED1(Red)/2(Green) and LED3
	LED1_OFF();
	LED2_ON();
	LED3_OFF();

	CONFIG_LED1();
	CONFIG_LED2();
	CONFIG_LED3();

	//printf("%s, init\n", __func__);
	return true;
}


static bool n33004a_led_ctrl(EN_LED_STATE enstate, u8 u8BlinkNum)
{
	int interval_t_ms = 50;
	int toggle = 0;
	unsigned long	tmo, out;

	switch(enstate) {
		case EN_LED_ON:
			LED1_ON();
			LED2_OFF();
			LED3_ON();
			break;

		case EN_LED_OFF:
			LED1_OFF();
			LED2_ON();
			LED3_OFF();
			break;

		case EN_LED_BLINK:
			//trun off all LED
			//printf("%s, LED blink!!", __func__);

			if(u8BlinkNum == 0)
			{
				break;
			}
			LED1_OFF();
			LED2_OFF();
			LED3_OFF();

			tmo = get_timer(0);
			out = tmo + interval_t_ms;

			while(u8BlinkNum)
			{
				if(get_timer(0) >= out)
				{
					out = get_timer(0) + interval_t_ms;
					if(toggle)
					{
						LED1_ON();
						LED2_OFF();
						LED3_OFF();
						toggle = 0;
						u8BlinkNum--;
					}
					else
					{
						LED1_OFF();
						LED2_ON();
						LED3_ON();
						toggle = 1;
					}
				}
			}
			break;
	}
}

static bool gb8CtlLed = false;

void nvt_stbc_led_init(void)
{
	gb8CtlLed = n33004a_led_init();
}

void nvt_stbc_led_ctl(EN_LED_STATE enstate, u8 u8BlinkNum)
{
	if(gb8CtlLed == false)
	{
		return;
	}
	n33004a_led_ctrl(enstate, u8BlinkNum);
}

