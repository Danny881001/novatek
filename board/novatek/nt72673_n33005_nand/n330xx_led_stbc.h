#ifndef __N330XX_LED_STBC_H__
#define __N330XX_LED_STBC_H__

#define REG_GPF_MUX             __REG(0xFC040438)
#define REG_GPF_DIR             __REG(0xFC04042C)
#define REG_GPF_CTL             __REG(0xFC040420)

#define REG_GPG_MUX             __REG(0xFC04043C)
#define REG_GPG_DIR             __REG(0xFC040430)
#define REG_GPG_CTL             __REG(0xFC040424)

#define LED1_SFT 			(1 << 11)   //GPF11
#define LED2_SFT 			(1 << 4)    //GPG4
#define LED3_SFT 			(1 << 7)    //GPG7

#define CONFIG_LED1()         {REG_GPF_MUX |= LED1_SFT; \
	REG_GPF_DIR |= LED1_SFT;}

#define CONFIG_LED2()         {REG_GPG_MUX |= LED2_SFT; \
	REG_GPG_DIR |= LED2_SFT;}

#define CONFIG_LED3()         {REG_GPG_MUX |= LED3_SFT; \
	REG_GPG_DIR |= LED3_SFT;}

#define LED1_ON()           (REG_GPF_CTL |= (LED1_SFT))
#define LED1_OFF()          (REG_GPF_CTL &= ~(LED1_SFT))

#define LED2_ON()           (REG_GPG_CTL |= (LED2_SFT))
#define LED2_OFF()          (REG_GPG_CTL &= ~(LED2_SFT))

#define LED3_ON()           (REG_GPG_CTL |= (LED3_SFT))
#define LED3_OFF()          (REG_GPG_CTL &= ~(LED3_SFT))


typedef enum {
	EN_LED_ON,
	EN_LED_OFF,
	EN_LED_BLINK,
	EN_LED_MAX
}EN_LED_STATE;

void nvt_stbc_led_init(void);
void nvt_stbc_led_ctl(EN_LED_STATE enstate, u8 u8BlinkNum);

#endif /* __N330XX_LED_STBC_H__ */
