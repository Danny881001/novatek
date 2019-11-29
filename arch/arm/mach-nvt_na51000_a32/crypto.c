/*
 * Copyright (C) 2014 Novatek Microelectronics Corp. All rights reserved.
 * Author: SP-KSW <SP_KSW_MailGrp@novatek.com.tw>
 *
 * SPDX-License-Identifier: GPL-2.0+
 */

#include <common.h>
#include <asm/armv7.h>
#include <asm/io.h>
#include <asm/arch/clock.h>
#include <asm/arch/IOAddress.h>
#include <asm/arch/rcw_macro.h>
#include <asm/arch/crypto.h>
#include <asm/arch/efuse_protected.h>


#define CRYPTO_DMA_CONFIG_REG_OFS         0x90
#define CRYPTO_DMA_STS_REG_OFS            0x94

#define CRYPTO_TRANS_SIZE_REG_OFS         0x98
#define CRYPTO_TRANS_COUNT_REG_OFS        0x9C
#define CRYPTO_SRC_ADDR_REG_OFS           0xA0
#define CRYPTO_DST_ADDR_REG_OFS           0xA4

#define CRYPTO_DMA_CONTROL_REG_OFS        0x88

#define CRYPTO_KEY000_REG_OFS             0x30
#define CRYPTO_KEY032_REG_OFS             0x34
#define CRYPTO_KEY064_REG_OFS             0x38
#define CRYPTO_KEY096_REG_OFS             0x3C


#define dma_getPhyAddr(addr)    ((((uint32_t)(addr))>=0x60000000UL)?((uint32_t)(addr)-0x60000000UL):(uint32_t)(addr))


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
#define SWAP(a,b)           { unsigned long t = a; a = b; b = t; t = 0; }
#define MKWORD(B) (UINT32) (((UINT32)((B)[3]))<<24 | ((UINT32)((B)[2]))<<16 | \
							((UINT32)((B)[1]))<< 8 | ((UINT32)((B)[0])))


#define CRYPTO_SETREG(ofs,value)    OUTW(IOADDR_CRYPTO_REG_BASE+(ofs),(value))
#define CRYPTO_GETREG(ofs)          INW(IOADDR_CRYPTO_REG_BASE+(ofs))

//CRYPTO_AES, crypt_op_param.op_mode, crypt_op_param.en_de_crypt, CRYPTO_DMA);

static void crypto_setKey(UINT8 *ucKey)
{
	UINT32 i;
    UINT8 *ptr;
	UINT8  Key[32];

	memset(Key, 0x00, 32);
	memcpy(Key, ucKey, 16);
	for (i = 0 ; i < 16 / 4 ; i++) {
		ptr = Key + i * 4;
		SWAP(ptr[0], ptr[3]);
		SWAP(ptr[1], ptr[2]);
	}
	CRYPTO_SETREG(CRYPTO_KEY000_REG_OFS, MKWORD(Key + 12));
	CRYPTO_SETREG(CRYPTO_KEY032_REG_OFS, MKWORD(Key + 8));
	CRYPTO_SETREG(CRYPTO_KEY064_REG_OFS, MKWORD(Key + 4));
	CRYPTO_SETREG(CRYPTO_KEY096_REG_OFS, MKWORD(Key + 0));
}

static s32 crypto_set_mode(UINT32 uiMode, UINT32 uiOPMode, UINT32 uiType, UINT32 uiACCMOD)
{
	UINT32 uiReg;
	if (uiMode != CRYPTO_AES) {
		DBG_ERR("Crypto engine support AES128 bits\r\n");
		return -1;
	}

	if (uiOPMode != CRYPTO_EBC) {
		DBG_ERR("Crypto engine support EBC mode\r\n");
		return -1;
	}

	OUTW(IOADDR_CRYPTO_REG_BASE, (1 << 9) | (uiType << 8) | (2 << 1) | (1 << 0) | (uiACCMOD << 11)); // big endian, descrypt, AES-128, sw-reset
	while (1) {     // wait sw reset clear
		uiReg = INW(IOADDR_CRYPTO_REG_BASE);
		if ((uiReg & (1 << 0)) == 0) {
			break;
		}
	}
	return 0;
}

/*
    Crypto controller Set DMA SRC/DST Address and Trigger

    @return
        - void
*/
static void crypto_dma_enable(UINT32 SrcAddr, UINT32 DstAddr, UINT32 Len)
{
	UINT32 uiReg;
	//clear DMA interrupt status
	uiReg = CRYPTO_GETREG(CRYPTO_DMA_STS_REG_OFS);
	CRYPTO_SETREG(CRYPTO_DMA_STS_REG_OFS, uiReg);

	//Disable interrupt enable
	CRYPTO_SETREG(CRYPTO_DMA_CONFIG_REG_OFS, 0x0);


	//Configure DMA buffer address and size
	CRYPTO_SETREG(CRYPTO_SRC_ADDR_REG_OFS, dma_getPhyAddr(SrcAddr));
	CRYPTO_SETREG(CRYPTO_DST_ADDR_REG_OFS, dma_getPhyAddr(DstAddr));
	CRYPTO_SETREG(CRYPTO_TRANS_SIZE_REG_OFS, Len);

	//Flush cache
	flush_dcache_range(SrcAddr, SrcAddr + Len);
	flush_dcache_range(DstAddr, DstAddr + Len);

	//Set DMA Enable
	CRYPTO_SETREG(CRYPTO_DMA_CONTROL_REG_OFS, 0x1);

	while ((CRYPTO_GETREG(CRYPTO_DMA_CONTROL_REG_OFS) & 0x1));

	CRYPTO_SETREG(CRYPTO_DMA_STS_REG_OFS, 0x1);
}



s32 crypto_data_operation(EFUSE_OTP_KEY_SET_FIELD key_set, CRYPT_OP crypt_op_param)
{
	s32  ret;
	UINT32  reg;

	reg = INW(IOADDR_CG_REG_BASE + 0x1C);
	OUTW(IOADDR_CG_REG_BASE + 0x1C, (reg | (1 << 31)));

	reg = INW(IOADDR_CG_REG_BASE + 0x80);
	reg |= (1 << 23);
	OUTW(IOADDR_CG_REG_BASE + 0x80, reg);

	ret = crypto_set_mode(CRYPTO_AES, crypt_op_param.op_mode, crypt_op_param.en_de_crypt, CRYPTO_DMA);

	if (ret < 0) {
		return ret;
	}
	efuse_otp_set_key(key_set);

	crypto_dma_enable(crypt_op_param.src_addr, crypt_op_param.dst_addr, crypt_op_param.length);

	reg = INW(IOADDR_CG_REG_BASE + 0x80);
	reg &= ~(1 << 23);
	OUTW(IOADDR_CG_REG_BASE + 0x80, reg);

	return 0;
}

s32 crypto_data_operation_by_key(UINT8 * key, CRYPT_OP crypt_op_param)
{
	s32  ret;
	UINT32  reg;

	reg = INW(IOADDR_CG_REG_BASE + 0x1C);
	OUTW(IOADDR_CG_REG_BASE + 0x1C, (reg | (1 << 31)));

	reg = INW(IOADDR_CG_REG_BASE + 0x80);
	reg |= (1 << 23);
	OUTW(IOADDR_CG_REG_BASE + 0x80, reg);

	ret = crypto_set_mode(CRYPTO_AES, crypt_op_param.op_mode, crypt_op_param.en_de_crypt, CRYPTO_DMA);

	if (ret < 0) {
		return ret;
	}
	crypto_setKey(key);

	crypto_dma_enable(crypt_op_param.src_addr, crypt_op_param.dst_addr, crypt_op_param.length);

	reg = INW(IOADDR_CG_REG_BASE + 0x80);
	reg &= ~(1 << 23);
	OUTW(IOADDR_CG_REG_BASE + 0x80, reg);

	return 0;
}

