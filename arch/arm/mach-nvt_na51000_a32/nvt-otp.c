/**
    NVT SRAM Contrl
    This file will Enable and disable SRAM shutdown
    @file       nvt-sramctl.c
    @ingroup
    @note
    Copyright   Novatek Microelectronics Corp. 2018.  All rights reserved.

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 2 as
    published by the Free Software Foundation.
*/

#include <common.h>
#include <asm/arch/efuse_protected.h>




#ifndef CHKPNT
#define CHKPNT    printf("\033[37mCHK: %d, %s\033[0m\r\n", __LINE__, __func__)
#endif

#ifndef DBGD
#define DBGD(x)   printf("\033[0;35m%s=%d\033[0m\r\n", #x, x)
#endif

#ifndef DBGH
#define DBGH(x)   printf("\033[0;35m%s=0x%08X\033[0m\r\n", #x, x)
#endif

#ifndef DBG_DUMP
#define DBG_DUMP(fmtstr, args...) printf(fmtstr, ##args)
#endif

#ifndef DBG_ERR
#define DBG_ERR(fmtstr, args...)  printf("\033[0;31mERR:%s() \033[0m" fmtstr, __func__, ##args)
#endif

#ifndef DBG_WRN
#define DBG_WRN(fmtstr, args...)  printf("\033[0;33mWRN:%s() \033[0m" fmtstr, __func__, ##args)
#endif

#if 0
#define DBG_IND(fmtstr, args...) printf("%s(): " fmtstr, __func__, ##args)
#else
#ifndef DBG_IND
#define DBG_IND(fmtstr, args...)
#endif
#endif

extern INT32    efuse_program_addr(UINT32 efuse_addr, UINT32 address);

static UINT32 OTP_OP[] = {
	0xE92D41F0,
	0xE1A04000,
	0xE1A06001,
	0xE30F2066,
	0xE1A00802,
	0xE30F2062,
	0xE1A03802,
	0xE2867030,
	0xE3A01000,
	0xE5801000,
	0xE3A01001,
	0xE5801020,
	0xE5901030,
	0xE7E01051,
	0xE3510001,
	0x11A0F007,
	0xE3A01010,
	0xE0811104,
	0xE3A05001,
	0xE5835020,
	0xE2867064,
	0xE5801004,
	0xE5902020,
	0xE3822002,
	0xE5802020,
	0xE5902020,
	0xE7E020D2,
	0xE3520001,
	0x01A0F007,
	0xE5902030,
	0xE5802030,
	0xE2811001,
	0xE3A05002,
	0xE5835020,
	0xE286709C,
	0xE5801004,
	0xE5902020,
	0xE3822002,
	0xE5802020,
	0xE5902020,
	0xE7E020D2,
	0xE3520001,
	0x01A0F007,
	0xE5902030,
	0xE5802030,
	0xE2811001,
	0xE3A05004,
	0xE5835020,
	0xE28670D4,
	0xE5801004,
	0xE5902020,
	0xE3822002,
	0xE5802020,
	0xE5902020,
	0xE7E020D2,
	0xE3520001,
	0x01A0F007,
	0xE5902030,
	0xE5802030,
	0xE2811001,
	0xE3A05008,
	0xE5835020,
	0xE2867F43,
	0xE5801004,
	0xE5902020,
	0xE3822002,
	0xE5802020,
	0xE5902020,
	0xE7E020D2,
	0xE3520001,
	0x01A0F007,
	0xE5902030,
	0xE5802030,
	0xE3A02000,
	0xE5802020,
	0xE8BD81F0,
	0xE320F000,
	0xEAFFFFFE,
	0xE320F000,
};

static UINT32 OTP_PG[] = {
	0xE92D41F0,
	0xE1A04000,
	0xE1A06001,
	0xE30F2066,
	0xE3E00020,
	0xE286709C,
	0xE3A08010,
	0xE3A0501F,
	0xE0045005,
	0xE1580005,
	0xC1A0F007,
	0xE1A00802,
	0xE286703C,
	0xE3A01001,
	0xE5801020,
	0xE5901030,
	0xE7E01051,
	0xE3510001,
	0x11A0F007,
	0xE3A01000,
	0xE0811004,
	0xE3A05004,
	0xE5805000,
	0xE2867070,
	0xE5801004,
	0xE5902020,
	0xE3822002,
	0xE5802020,
	0xE5902020,
	0xE7E020D2,
	0xE3520001,
	0x01A0F007,
	0xE5902030,
	0xE5802030,
	0xE3A02000,
	0xE5802020,
	0xE5802004,
	0xE5802000,
	0xE3A00000,
	0xE8BD81F0,
	0xE320F000,
	0xEAFFFFFE,
	0xE320F000,
};

static UINT32 OTP_SEC_EN[] = {
	0xE92D41F0,
	0xE1A04000,
	0xE1A06000,
	0xE30F2066,
	0xE1A00802,
	0xE2867020,
	0xE3A01001,
	0xE5801020,
	0xE5901030,
	0xE7E01051,
	0xE3510001,
	0x11A0F007,
	0xE3A01000,
	0xE3A01008,
	0xE3A05004,
	0xE5805000,
	0xE2867054,
	0xE5801004,
	0xE5902020,
	0xE3822002,
	0xE5802020,
	0xE5902020,
	0xE7E020D2,
	0xE3520001,
	0x01A0F007,
	0xE5902030,
	0xE5802030,
	0xE3A02000,
	0xE5802020,
	0xE5802004,
	0xE5802000,
	0xE8BD81F0,
	0xE320F000,
	0xEAFFFFFE,
	0xE320F000,
};

static UINT32 OTP_JTAG_DIS[] = {
	0xE92D41F0,
	0xE1A04000,
	0xE1A06000,
	0xE30F2066,
	0xE1A00802,
	0xE2867020,
	0xE3A01001,
	0xE5801020,
	0xE5901030,
	0xE7E01051,
	0xE3510001,
	0x11A0F007,
	0xE3A01000,
	0xE3A01048,
	0xE3A05004,
	0xE5805000,
	0xE2867054,
	0xE5801004,
	0xE5902020,
	0xE3822002,
	0xE5802020,
	0xE5902020,
	0xE7E020D2,
	0xE3520001,
	0x01A0F007,
	0xE5902030,
	0xE5802030,
	0xE3A02000,
	0xE5802020,
	0xE5802004,
	0xE5802000,
	0xE8BD81F0,
	0xE320F000,
	0xEAFFFFFE,
	0xE320F000,
};

static UINT32 OTP_KEY0_SET[] = {
	0xE92D41F0,
	0xE1A04000,
	0xE1A06000,
	0xE30F2066,
	0xE1A00802,
	0xE2867020,
	0xE3A01001,
	0xE5801020,
	0xE5901030,
	0xE7E01051,
	0xE3510001,
	0x11A0F007,
	0xE3A01000,
	0xE3A01FE2,
	0xE3A05004,
	0xE5805000,
	0xE2867054,
	0xE5801004,
	0xE5902020,
	0xE3822002,
	0xE5802020,
	0xE5902020,
	0xE7E020D2,
	0xE3520001,
	0x01A0F007,
	0xE5902030,
	0xE5802030,
	0xE3A02000,
	0xE5802020,
	0xE5802004,
	0xE5802000,
	0xE8BD81F0,
	0xE320F000,
	0xEAFFFFFE,
	0xE320F000,
};

static UINT32 OTP_KEY1_SET[] = {
	0xE92D41F0,
	0xE1A04000,
	0xE1A06000,
	0xE30F2066,
	0xE1A00802,
	0xE2867020,
	0xE3A01001,
	0xE5801020,
	0xE5901030,
	0xE7E01051,
	0xE3510001,
	0x11A0F007,
	0xE3A01000,
	0xE3A01FEA,
	0xE3A05004,
	0xE5805000,
	0xE2867054,
	0xE5801004,
	0xE5902020,
	0xE3822002,
	0xE5802020,
	0xE5902020,
	0xE7E020D2,
	0xE3520001,
	0x01A0F007,
	0xE5902030,
	0xE5802030,
	0xE3A02000,
	0xE5802020,
	0xE5802004,
	0xE5802000,
	0xE8BD81F0,
	0xE320F000,
	0xEAFFFFFE,
	0xE320F000,
};

static UINT32 OTP_KEY2_SET[] = {
	0xE92D41F0,
	0xE1A04000,
	0xE1A06000,
	0xE30F2066,
	0xE1A00802,
	0xE2867020,
	0xE3A01001,
	0xE5801020,
	0xE5901030,
	0xE7E01051,
	0xE3510001,
	0x11A0F007,
	0xE3A01000,
	0xE3A01FF2,
	0xE3A05004,
	0xE5805000,
	0xE2867054,
	0xE5801004,
	0xE5902020,
	0xE3822002,
	0xE5802020,
	0xE5902020,
	0xE7E020D2,
	0xE3520001,
	0x01A0F007,
	0xE5902030,
	0xE5802030,
	0xE3A02000,
	0xE5802020,
	0xE5802004,
	0xE5802000,
	0xE8BD81F0,
	0xE320F000,
	0xEAFFFFFE,
	0xE320F000,
};

static UINT32 OTP_KEY3_SET[] = {
	0xE92D41F0,
	0xE1A04000,
	0xE1A06000,
	0xE30F2066,
	0xE1A00802,
	0xE2867020,
	0xE3A01001,
	0xE5801020,
	0xE5901030,
	0xE7E01051,
	0xE3510001,
	0x11A0F007,
	0xE3A01000,
	0xE3A01FFA,
	0xE3A05004,
	0xE5805000,
	0xE2867054,
	0xE5801004,
	0xE5902020,
	0xE3822002,
	0xE5802020,
	0xE5902020,
	0xE7E020D2,
	0xE3520001,
	0x01A0F007,
	0xE5902030,
	0xE5802030,
	0xE3A02000,
	0xE5802020,
	0xE5802004,
	0xE5802000,
	0xE8BD81F0,
	0xE320F000,
	0xEAFFFFFE,
	0xE320F000,
};

static UINT32 OTP_READ[] = {
	0xE92D41F0,
	0xE1A04000,
	0xE1A06001,
	0xE30F2066,
	0xE3E00020,
	0xE2867094,
	0xE3A08008,
	0xE3A0501F,
	0xE0045005,
	0xE1550008,
	0x11A0F007,
	0xE1A00802,
	0xE286703C,
	0xE3A01001,
	0xE5801020,
	0xE5901030,
	0xE7E01051,
	0xE3510001,
	0x11A0F007,
	0xE3A01000,
	0xE0811004,
	0xE3A05000,
	0xE5805000,
	0xE2867070,
	0xE5801004,
	0xE5902020,
	0xE3822002,
	0xE5802020,
	0xE5902020,
	0xE7E020D2,
	0xE3520001,
	0x01A0F007,
	0xE5902030,
	0xE5802030,
	0xE3A02000,
	0xE5802020,
	0xE5900008,
	0xE8BD81F0,
	0xE320F000,
	0xEAFFFFFE,
	0xE320F000,
};


static INT32 efuse_set_key(EFUSE_OTP_KEY_SET_FIELD key_set_index, UINT32 address)
{
	__asm__("MOV r0, %0\n\t"
			"MOV r1, %1\n\t"
			"MOV lr, pc\n\t"
			"MOV pc, %1\n\t"
			:
			: "r"(key_set_index), "r"(address)
			: "lr", "r0", "r1", "r2", "r3");
	return E_OK;
}

static INT32 _efuse_prog_(UINT32 addr, UINT32 address)
{
	INT32  result;

	__asm__("MOV r0, %1\n\t"
			"MOV r1, %2\n\t"
			"MOV lr, pc\n\t"
			"MOV pc, %2\n\t"
			"MOV %0, r0\n\t"
			: "=r"(result)
			: "r"(addr), "r"(address)
			: "lr", "r0", "r1", "r2", "r3");
	return result;
}

static void _efuse_secure_en_(UINT32 address)
{
	__asm__("MOV r0, %0\n\t"
			"MOV lr, pc\n\t"
			"MOV pc, %0\n\t"
			:
			: "r"(address)
			: "lr", "r0", "r1", "r2", "r3");
	return;
}

static void _efuse_jtag_dis_(UINT32 address)
{
	__asm__("MOV r0, %0\n\t"
			"MOV lr, pc\n\t"
			"MOV pc, %0\n\t"
			:
			: "r"(address)
			: "lr", "r0", "r1", "r2", "r3");
	return;
}

static void _efuse_key_set_en_(UINT32 address)
{
	__asm__("MOV r0, %0\n\t"
			"MOV lr, pc\n\t"
			"MOV pc, %0\n\t"
			:
			: "r"(address)
			: "lr", "r0", "r1", "r2", "r3");
	return;
}


static INT32 efuse_program(UINT32 rol, UINT32 col)
{
	return _efuse_prog_((rol | (col << 5)), (UINT32)&OTP_PG[0]);
}


static INT32 efuse_write_data(UINT32 addr, UINT32 data)
{
	UINT32  ui_bits;
	UINT32  ui_data;

	if (addr < 16) {
		return EFUSE_PARAM_ERR;
	}

	ui_data = data;

	ui_bits = 0;

	while (ui_data) {
		ui_bits = __builtin_ctz(ui_data);
		ui_data &= ~(1 << ui_bits);
		if (efuse_program(addr, ui_bits) != E_OK) {
			DBG_ERR("%s,eFuse program addr[%03lx] bit[%2d] fail\r\n", __func__, (UINT32)addr, (int)ui_bits);
			return EFUSE_OPS_ERR;
		}
	}



	return EFUSE_SUCCESS;
}

static INT32 efuse_read_data(UINT32 efuse_addr, UINT32 address)
{
	INT32  result;
	__asm__("MOV r0, %1\n\t"
			"MOV r1, %2\n\t"
			"MOV lr, pc\n\t"
			"MOV pc, %2\n\t"
			"MOV %0, r0\n\t"
			: "=r"(result)
			: "r"(efuse_addr), "r"(address)
			: "lr", "r0", "r1");
	return result;
}

#define EFUSE_READDATA_PARAM()    efuse_read_data(EFUSE_OPT_PARAM_FIELD, (UINT32)OTP_READ)

/**
    efuse_write_key

    Write specific key into specific key set (0~3)

    @Note: key set 0 is for secure boot use

    @param[in] key_set_index   key set (0~3)
    @param[in] ucKey           key (16bytes)
    @return Description of data returned.
        - @b E_OK:   Success
        - @b E_SYS:  Fail
*/
INT32 efuse_write_key(EFUSE_OTP_KEY_SET_FIELD key_set_index, UINT8 *uc_key)
{
	INT32   result = EFUSE_SUCCESS;
	UINT32  u32_key = (UINT32)(uc_key);
	UINT32  data[4];
	UINT32  key_field_start_index = 16;
	UINT32  index_cnt;
	UINT32  temp_value_UINT32;

	switch (key_set_index) {
	//Note: >>>1st Key set is dedicate for secure boot usage<<<
	case EFUSE_OTP_1ST_KEY_SET_FIELD:
		key_field_start_index = 16;
		break;

	case EFUSE_OTP_2ND_KEY_SET_FIELD:
		key_field_start_index = 20;
		break;

	case EFUSE_OTP_3RD_KEY_SET_FIELD:
		key_field_start_index = 24;
		break;

	case EFUSE_OTP_4TH_KEY_SET_FIELD:
		key_field_start_index = 28;
		break;

	default:
		DBG_ERR("Unknow key set[%d] => should be 0~3\r\n", (int)key_set_index);
		result = EFUSE_OPS_ERR;
		break;
	}

	if (result != EFUSE_SUCCESS) {
		return result;
	}
	temp_value_UINT32 = *(UINT32 *)(u32_key + 12);
	data[0] = (UINT32)(((temp_value_UINT32 & 0xFF000000) >> 24) | ((temp_value_UINT32 & 0xFF0000) >> 8) | ((temp_value_UINT32 & 0xFF00) << 8) | ((temp_value_UINT32 & 0xFF) << 24));

	temp_value_UINT32 = *(UINT32 *)(u32_key + 8);
	data[1] = (UINT32)(((temp_value_UINT32 & 0xFF000000) >> 24) | ((temp_value_UINT32 & 0xFF0000) >> 8) | ((temp_value_UINT32 & 0xFF00) << 8) | ((temp_value_UINT32 & 0xFF) << 24));

	temp_value_UINT32 = *(UINT32 *)(u32_key + 4);
	data[2] = (UINT32)(((temp_value_UINT32 & 0xFF000000) >> 24) | ((temp_value_UINT32 & 0xFF0000) >> 8) | ((temp_value_UINT32 & 0xFF00) << 8) | ((temp_value_UINT32 & 0xFF) << 24));

	temp_value_UINT32 = *(UINT32 *)(u32_key + 0);
	data[3] = (UINT32)(((temp_value_UINT32 & 0xFF000000) >> 24) | ((temp_value_UINT32 & 0xFF0000) >> 8) | ((temp_value_UINT32 & 0xFF00) << 8) | ((temp_value_UINT32 & 0xFF) << 24));

	for (index_cnt = 0; index_cnt < EFUSE_OTP_TOTAL_KEY_SET_FIELD; index_cnt++) {
		result = efuse_write_data(key_field_start_index + index_cnt, data[index_cnt]);
		if (result < 0) {
			DBG_ERR("[%d]set key => write addr[%2d][0x%08lx] fail\r\n", (int)(((key_field_start_index - 16) / 4) + 1), (int)(key_field_start_index + index_cnt), (UINT32)data[index_cnt]);
			break;
		}
	}
	efuse_key_set_flag(key_set_index);

	return result;
}

/**
    efuse_secure_en

    Enable secure boot
*/
void efuse_secure_en(void)
{
	return _efuse_secure_en_((UINT32)&OTP_SEC_EN[0]);
}

/**
    efuse_jtag_dis

    Disable JTAG

    @Note: Can not re enable once disabled
*/
void efuse_jtag_dis(void)
{
	return _efuse_jtag_dis_((UINT32)&OTP_JTAG_DIS[0]);
}


/**
     key set enable identifier

     Program specific key set flag(represent key set were programmed or not)

     @param[in] key set     0~3
     @return Description of data returned.
         - @b  0:   Success
         - @b -1:   Fail
*/

INT32 efuse_key_set_flag(EFUSE_OTP_KEY_SET_FIELD key_set_index)
{
	UINT32 key_set_addr;
	switch (key_set_index) {
	case EFUSE_OTP_1ST_KEY_SET_FIELD:
		key_set_addr = (UINT32)&OTP_KEY0_SET[0];
		break;

	case EFUSE_OTP_2ND_KEY_SET_FIELD:
		key_set_addr = (UINT32)&OTP_KEY1_SET[0];
		break;

	case EFUSE_OTP_3RD_KEY_SET_FIELD:
		key_set_addr = (UINT32)&OTP_KEY2_SET[0];
		break;

	case EFUSE_OTP_4TH_KEY_SET_FIELD:
		key_set_addr = (UINT32)&OTP_KEY3_SET[0];
		break;

	default:
		DBG_ERR("Unknow key set [%d] (0~3)\r\n", key_set_index);
		return -1;
		break;
	}
	_efuse_key_set_en_(key_set_addr);
	return 0;
}

/**
     Get key set enable identifier

     Represent specific key set was programmed or not

     @param[in] key set     0~3
     @return Description of data returned.
         - @b  0:   Not programmed
         - @b  1:   Programmed
*/
UINT32 efuse_key_get_flag(EFUSE_OTP_KEY_SET_FIELD key_set_index)
{
	volatile UINT32  param;

	param = EFUSE_READDATA_PARAM();

	return (param & (1<<(EFUSE_OTP_1ST_KEY_SET_FIELD_FLAG+key_set_index)))==0?0x00000000:0x00000001;
}

/**
     Get if secure boot enabled or not

     Represent if secure enable or not

     @return Description of data returned.
         - @b  0:   Normal
         - @b  1:   Secure
*/
UINT32 efuse_is_secure_en(void)
{

	volatile UINT32  param;

	param = EFUSE_READDATA_PARAM();

	return (param & (1<<(EFUSE_OTP_SECURE_EN_FLAG)))==0?0x00000000:0x00000001;
}

/**
    efuse_otp_set_key

    Durung encrypt or decrypt, configure specific key set as AES key(0~3)

    @Note: key set 0 is for secure boot use

    @param[in] key_set_index   key set (0~3)
    @return Description of data returned.
        - @b E_OK:   Success
*/
INT32 efuse_otp_set_key(EFUSE_OTP_KEY_SET_FIELD key_set_index)
{
	efuse_set_key(key_set_index, (UINT32)OTP_OP);
	return E_OK;
}

