#ifndef	__HS_TYPE_H__
#define	__HS_TYPE_H__

/**************************************************************************
 * å¸¸é‡å®šä¹‰		*
 **************************************************************************/
typedef enum
{
	HS_FALSE	= 0,
	HS_TRUE 	= 1,
} HS_BOOL;


/**************************************************************************
 * åŸºæœ¬æ•°æ®ç±»å‹å®šä¹‰ï¼Œåº”ç”¨å±‚å’ŒBSPä»£ç å‡ä½¿ç”¨ 												  *
 **************************************************************************/

typedef unsigned char			HS_U8;
typedef unsigned char			HS_UCHAR;
typedef unsigned short			HS_U16;

typedef unsigned int			HS_U32;

typedef signed char 			HS_S8;
typedef short					HS_S16;
typedef int 					HS_S32;
typedef unsigned char			HS_BYTE;  // 1 byte

#ifndef _M_IX86
	typedef unsigned long long		HS_U64;
	typedef long long				HS_S64;
#else
	typedef __int64 				HS_U64;
	typedef __int64 				HS_S64;
#endif

typedef char					HS_CHAR;
typedef char*					HS_PCHAR;

typedef float					HS_FLOAT;
typedef double					HS_DOUBLE;

#define HS_VOID                  void

typedef HS_U32					HS_HANDLE;

/**************************************************************************
 *                            æ•°æ®ç±»å‹                                     *
 **************************************************************************/

//1024 Byte
struct HisDatabase
{
	int   RawEmmc[4];				//è£¸æ¿
	int   Volume[4];				//éŸ³é‡
	int   FirstPowerOn[4];			//é¦–æ¬¡ä¸Šç”µ
	int   FactoryMode[4];			//å·¥å‚æ¨¡å¼
	int   ToFacMode[4]; 			//ToFacæ¨¡å¼
	int   Navigation[4];			//å¼€æœºå¯¼èˆª
	int   UpgradeFinish[4]; 		//å‡çº§å®Œæˆæ ‡å¿—
	char   MacAddress[16];			//MAC åœ°å€
	char   OriDevNum[32];			//åŸå§‹æœºå·
	char   CurDevNum[32];			//å½“å‰æœºå·
	char   SerialNum[32];			//ä¸»æ¿åºåˆ—å·
	char   FunctionCode[64];		//åŠŸèƒ½ç 
	int    UartMode[4]; 			//ä¸²å£
	int    FacUpMode[4];			//ä¸€æ‹–å¤šå‡çº§
	int    FacUpClear[4];			//ä¸€æ‹–å¤šæ¸…æ¯å—
	int    fliterTime[4];			//æ»¤ç½‘æ›´æ¢æ—¶é—´
	int    reserved[172];			//é¢„ç•™
};

//512 Byte
struct HisDataInfo
{
    HS_U8     upgrade_count;				//å‡çº§æ¬¡æ•°
    HS_CHAR   upgrade_info[10][48];         //è®°å½•æ¯æ¬¡å‡çº§çš„ç‰ˆæœ¬ä¿¡æ¯
    HS_U8     reserved[31];                 //é¢„ç•™
};

//512 Byte
struct HisDeviceInfo
{
    HS_CHAR   local_his_string[16];        //Éı¼¶·À´ô±êÊ¶
    HS_U8     Froceupgrade;                //ÓÃ»§ÏÂ´¥·¢Éı¼¶
    HS_U8     tcon_fastboot;                  //Tcon¿ìËÙÉÏµç±êÖ¾
    HS_U8     factory_poweron_mode;        //¿ª¹Ø»úÄ£Ê½
    HS_U8     led_poweroff_status;         //´ı»úµÆĞ§×´Ì¬
    HS_U8     led_poweroff_type;           //´ı»úµÆĞ§ÖÖÀà
    HS_U8     led_poweron_status;          //¿ª»úµÆĞ§×´Ì¬
    HS_U8     led_poweron_type;            //¿ª»úµÆĞ§ÖÖÀà
    HS_CHAR   logo_cmd[16];                //¶¨ÖÆ¿ª»úlogoÏà¹Ø£¨Ô¤Áô£©
    HS_CHAR   logo_state[16];              //¶¨ÖÆ¿ª»úlogoÏà¹Ø£¨Ô¤Áô£©
    HS_U8     enable_tcon_panel;                //ÄÚÖÃTCON±êÖ¾
    HS_U8      VBORange;                                    //VbyOne ·ù¶ÈÏà¹Ø
    HS_U8      hisense_panel_swing_level;   //VbyOne ·ù¶ÈÏà¹Ø
    HS_U8     dc_poweroff;                 //±ê¼ÇµçÊÓ´ı»ú×´Ì¬
    HS_U8     panel_index;                 //ÆÁ²Î±àºÅ
    HS_U8     reserved[452];               //Ô¤Áô
};

#endif /* __HS_TYPE_H__ */
