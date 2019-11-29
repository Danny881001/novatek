#ifndef	__HS_TYPE_H__
#define	__HS_TYPE_H__

/**************************************************************************
 * 常量定义		*
 **************************************************************************/
typedef enum
{
	HS_FALSE	= 0,
	HS_TRUE 	= 1,
} HS_BOOL;


/**************************************************************************
 * 基本数据类型定义，应用层和BSP代码均使用 												  *
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
 *                            数据类型                                     *
 **************************************************************************/

//1024 Byte
struct HisDatabase
{
	int   RawEmmc[4];				//裸板
	int   Volume[4];				//音量
	int   FirstPowerOn[4];			//首次上电
	int   FactoryMode[4];			//工厂模式
	int   ToFacMode[4]; 			//ToFac模式
	int   Navigation[4];			//开机导航
	int   UpgradeFinish[4]; 		//升级完成标志
	char   MacAddress[16];			//MAC 地址
	char   OriDevNum[32];			//原始机号
	char   CurDevNum[32];			//当前机号
	char   SerialNum[32];			//主板序列号
	char   FunctionCode[64];		//功能码
	int    UartMode[4]; 			//串口
	int    FacUpMode[4];			//一拖多升级
	int    FacUpClear[4];			//一拖多清母块
	int    fliterTime[4];			//滤网更换时间
	int    reserved[172];			//预留
};

//512 Byte
struct HisDataInfo
{
    HS_U8     upgrade_count;				//升级次数
    HS_CHAR   upgrade_info[10][48];         //记录每次升级的版本信息
    HS_U8     reserved[31];                 //预留
};

//512 Byte
struct HisDeviceInfo
{
    HS_CHAR   local_his_string[16];        //����������ʶ
    HS_U8     Froceupgrade;                //�û��´�������
    HS_U8     tcon_fastboot;                  //Tcon�����ϵ��־
    HS_U8     factory_poweron_mode;        //���ػ�ģʽ
    HS_U8     led_poweroff_status;         //������Ч״̬
    HS_U8     led_poweroff_type;           //������Ч����
    HS_U8     led_poweron_status;          //������Ч״̬
    HS_U8     led_poweron_type;            //������Ч����
    HS_CHAR   logo_cmd[16];                //���ƿ���logo��أ�Ԥ����
    HS_CHAR   logo_state[16];              //���ƿ���logo��أ�Ԥ����
    HS_U8     enable_tcon_panel;                //����TCON��־
    HS_U8      VBORange;                                    //VbyOne �������
    HS_U8      hisense_panel_swing_level;   //VbyOne �������
    HS_U8     dc_poweroff;                 //��ǵ��Ӵ���״̬
    HS_U8     panel_index;                 //���α��
    HS_U8     reserved[452];               //Ԥ��
};

#endif /* __HS_TYPE_H__ */
