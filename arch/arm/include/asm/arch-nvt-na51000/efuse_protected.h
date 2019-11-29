/*
    Novatek protected header file of NT96660's driver.

    The header file for Novatek protected APIs of NT96660's driver.

    @file       efuse_protected.h
    @ingroup    mIDriver
    @note       For Novatek internal reference, don't export to agent or customer.

    Copyright   Novatek Microelectronics Corp. 2012.  All rights reserved.
*/

#ifndef _NVT_EFUSE_PROTECTED_H
#define _NVT_EFUSE_PROTECTED_H

#include "nvt_types.h"

typedef enum {
	EFUSE_OTP_1ST_KEY_SET_FIELD = 0x0,        // This if for secure boot
	EFUSE_OTP_2ND_KEY_SET_FIELD,
	EFUSE_OTP_3RD_KEY_SET_FIELD,
	EFUSE_OTP_4TH_KEY_SET_FIELD,
	EFUSE_OTP_TOTAL_KEY_SET_FIELD,
} EFUSE_OTP_KEY_SET_FIELD;

STATIC_ASSERT(EFUSE_OTP_TOTAL_KEY_SET_FIELD <= 4);

#define E_OK                            0
#define EFUSE_SUCCESS                E_OK
#define EFUSE_FREEZE_ERR             -1001          // Programmed already, only can read
#define EFUSE_INACTIVE_ERR           -1002          // This field is empty(not programmed yet)
#define EFUSE_INVALIDATE_ERR         -1003          // This field force invalidate already
#define EFUSE_UNKNOW_PARAM_ERR       -1004          // efuse param field not defined
#define EFUSE_OPS_ERR                -1005          // efuse operation error
#define EFUSE_SECURITY_ERR           -1006          // efuse under security mode => can not read back
#define EFUSE_PARAM_ERR              -1007          // efuse param error

#define EFUSE_OPT_PARAM_FIELD       8


#define EFUSE_OTP_SECURE_EN_FLAG            0       // This bit identify secure enable
#define EFUSE_OTP_1ST_KEY_SET_FIELD_FLAG    28  	// This bit identify key set 0 was set
#define EFUSE_OTP_2ND_KEY_SET_FIELD_FLAG    29  	// This bit identify key set 1 was set
#define EFUSE_OTP_3RD_KEY_SET_FIELD_FLAG    30 		// This bit identify key set 2 was set
#define EFUSE_OTP_4TH_KEY_SET_FIELD_FLAG    31  	// This bit identify key set 3 was set

extern void     efuse_jtag_dis(void);
extern void     efuse_secure_en(void);
extern UINT32   efuse_is_secure_en(void);
extern INT32    efuse_otp_set_key(EFUSE_OTP_KEY_SET_FIELD key_set_index);
extern INT32    efuse_key_set_flag(EFUSE_OTP_KEY_SET_FIELD key_set_index);
extern INT32    efuse_write_key(EFUSE_OTP_KEY_SET_FIELD key_set_index, UINT8 *uc_key);
extern UINT32   efuse_key_get_flag(EFUSE_OTP_KEY_SET_FIELD key_set_index);




#endif

