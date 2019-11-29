/*
 *  board/novatek/nt72668tk/nt72668_panel_init.c
 *
 *  Created:	Mar 22, 2016
 *  Copyright:	Novatek Inc.
 *
 */

#include <common.h>
#include <asm/hardware.h>
#include <nvt_panel_init.h>
#include <nvt_emmc_xbootdat.h>
#include <nvt_emmc_fwupdate.h>
#include <nvt_eburner.h>
#include <nvt_common_utils.h>

#include <asm/arch/nvt_stbc_regs.h>
#include <nvt_stbc.h>

#include <asm/arch-nt72673_a32/nvt_mpll.h>
/*-----------------------------------------------------------------------------*/
/* Local Types Declarations                                                    */
/*-----------------------------------------------------------------------------*/

#define GET_PANEL_TABLE_FROM_INTERNAL   0
#define GET_PANEL_TABLE_FROM_EXTERNAL   1

#define _SHOW_TEST_PATTERN_             _DISABLE_ //_ENABLE_//_DISABLE_

/*-----------------------------------------------------------------------------*/
/* Local Global Variables                                                      */
/*-----------------------------------------------------------------------------*/

static struct ST_KER_VID_PANEL_SETTING gstPanelSetting = {0};

static bool gbPanelParamInited = 0;
static bool gbPanelParamSettingDone = 0;

/*-----------------------------------------------------------------------------*/
/* Local Function Protype                                                      */
/*-----------------------------------------------------------------------------*/
static unsigned char *panel_param_tmp_buff = (unsigned char*)(CONFIG_SYS_FWUPDATE_BUF + (24 << 20));
static unsigned char emmc_buff[EMMC_SECTOR_SIZE];

static u32 UTL_SHL_SetValue( u32 u32Val, u32 u32Msb, u32 u32Lsb, u32 u32SetVal);
static void UTL_SHL_WriteRegMask( u32 u32Addr, u32 u32Msb, u32 u32Lsb, u32 u32Val );
static void _Out_P_SetLvdsFormat( EN_DRV_SCLR_LVDS_FMT enFormat );
static void _Img_P_SetVideoOutputDither( EN_DRV_SCLR_VIDEO_OUTPUT_DITHER enDither );
static void _VID_Get_PanelTablePara( bool b8GetFromTxt);
static void _VID_Set_PanelPowerOn( void );
static void _VID_Set_MPLL( u32 u32PLL );
static void _VID_Set_Panel( struct ST_KER_VID_PANEL_SETTING *pstPanelSetting );
static void _VID_Set_LvdsPathSel( struct ST_KER_VID_PANEL_SETTING *pstPanelSetting );
static void _VID_Set_ComboTx( void );
static void _VID_Set_ScalerInit( void );
static void _VID_Set_Backlight( void );
static void _VID_Set_ComboTx_Vx1( void );
static void _VID_Set_ComboTx_LVDS( void );
static void _VID_Set_OSDDisplayWindow( void );


#if (_SHOW_TEST_PATTERN_ == _ENABLE_)
static void _Set_VEP_TestPattern(void);
static void _VID_DP_SetFrameWindowColor(u16 u16R, u16 u16G, u16 u16B);
static void _VID_DP_EnableDisplayWindow(u32 u32Path, bool bEnable);
#endif
/////////////////////////////////////////////////////////////////////////////////

/* ********************************************************************************************* */
/* these code are to specify 673 TKB v1.0 / v1.1 */
typedef enum {
	NVT673_TKB_V1_0,
	NVT673_TKB_V1_1,
	NVT673_TKB_V3_1,
	N62002_V00,
	NVT673_TKB_V_MAX,
} NVT673_tk_board_version;

//#define __REG(x)        (*((volatile unsigned int *) (x)))
#define TKB_TYPE_RANGE 	10
#define TKB_V1_1_VAL 	0x94
#define TKB_V1_0_VAL 	0xFF
#define TKB_V3_1_VAL 	0x4B
#define N62002_V00_BOARD_VAL 	0x74

static NVT673_tk_board_version lvds_get_673_board_type(void)
{
	unsigned int reg_val;
	reg_val = __REG(0xFC040324);

	if(reg_val <= (TKB_V1_1_VAL + TKB_TYPE_RANGE) && reg_val >= (TKB_V1_1_VAL - TKB_TYPE_RANGE))
		return NVT673_TKB_V1_1;

	if (reg_val <= (TKB_V1_0_VAL + TKB_TYPE_RANGE) && reg_val >= (TKB_V1_0_VAL - TKB_TYPE_RANGE))
		return NVT673_TKB_V1_0;

	if (reg_val <= (TKB_V3_1_VAL + TKB_TYPE_RANGE) && reg_val >= (TKB_V3_1_VAL - TKB_TYPE_RANGE))
		return NVT673_TKB_V3_1;

	if (reg_val <= (N62002_V00_BOARD_VAL + TKB_TYPE_RANGE) && reg_val >= (N62002_V00_BOARD_VAL - TKB_TYPE_RANGE))
		return N62002_V00;

	return NVT673_TKB_V_MAX;
}
/* ********************************************************************************************* */

static int nvt_eburner_read_panel_param_from_img(int *pindex, panel_param_blk *pparam)
{
	int ret = 0;
	char *ptr;
    panel_index *panel_ind = NULL;

	//read index from update image
	ret = nvt_eburner_load_img(panel_param_tmp_buff, "xbootdat", NVT_EBURNER_IMG_FNAME);
	if(ret < 0) {
		printf("%s read param from image fail !\n", __func__);
		ret = -EINVAL;
		*pindex = -1;
		goto out;
	}

	panel_ind = (panel_index *)panel_param_tmp_buff;
	*pindex = panel_ind->index; 

    printf("panel index: %d\n", *pindex);

	ptr = panel_param_tmp_buff;
	ptr += (*pindex + 1) * EMMC_SECTOR_SIZE;
	memcpy(pparam, ptr, sizeof(panel_param_blk));

	//do some sanity check
	if(memcmp(pparam->p.sign, PANEL_PARAM_SIGN, strlen(PANEL_PARAM_SIGN))) {
		printf("%s invalid panel param !\n", __func__);
		*pindex = -1;
		goto out;
	}

out:
	return ret;

}

static int nvt_eburner_read_panel_param_from_dev(int *pindex, panel_param_blk *pparam)
{
	int ret = 0;
	nvt_emmc_image_info *pimg;
	unsigned int rdlen;
	char *ptr;
	char cmd[64] = {};
    panel_index *panel_ind = NULL;

	//read panel index from dev partition
	pimg = nvt_emmc_get_img_by_name("xbootdat");
	if(pimg == NULL) {
		printf("%s read param partition fail !\n", __func__);
		ret = -EINVAL;
		*pindex = -1;
		goto out;
	}

	ret = pimg->read_img(pimg, panel_param_tmp_buff, &rdlen, NULL);
	if(ret < 0) {
		printf("%s read panel index fail !\n", __func__);
		ret = -EINVAL;
		*pindex = -1;
		goto out;
	}

	panel_ind = (panel_index *)panel_param_tmp_buff;
	*pindex = panel_ind->index; 

    printf("panel index: %d\n", *pindex);
    
	if(*pindex >= PANEL_PARM_NUM) {
		printf("%s panel index out of range !\n", __func__);
		ret = -EINVAL;
		*pindex = -1;
		goto out;
	}

	ptr = panel_param_tmp_buff;
	ptr += EMMC_SECTOR_SIZE * (*pindex + 1);
	memcpy(pparam, ptr, sizeof(panel_param_blk));

	//do some sanity check
	if(memcmp(pparam->p.sign, PANEL_PARAM_SIGN, strlen(PANEL_PARAM_SIGN))) {
		printf("%s invalid panel param !\n", __func__);
		*pindex = -1;
		goto out;
	}

out:
	return ret;
}


int nvt_eburner_panel_param_init(int *pindex, panel_param_blk *pparam)
{
	int ret = 0;

	if(pindex == NULL || pparam == NULL) {
		printf("%s pindex or pparam buffer is NULL !\n", __func__);
		ret = -EINVAL;
		goto out;
	}

    if(gbPanelParamInited == 0)
    {
    	ret = nvt_eburner_read_panel_param_from_img(pindex, pparam);

    	if(ret < 0)
    		ret = nvt_eburner_read_panel_param_from_dev(pindex, pparam);

        gbPanelParamInited = 1;
    }
out:
	return ret;
}

u32 UTL_SHL_SetValue( u32 u32Val, u32 u32Msb, u32 u32Lsb, u32 u32SetVal )
{
	u32 u32Msk;
	u32 u32Tmp;

	u32Msk = ( ( 1 << u32Msb ) - ( 1 << u32Lsb ) ) | ( 1 << u32Msb );
	u32Tmp = u32Val & ( ~u32Msk );
	u32SetVal = ( u32SetVal << u32Lsb ) & u32Msk;
	u32Tmp = u32Tmp | u32SetVal;

	return u32Tmp;
}

void UTL_SHL_WriteRegMask( u32 u32Addr, u32 u32Msb, u32 u32Lsb, u32 u32Val )
{
	u32 u32Tmp;

	HAL_READ_UINT32( u32Addr,u32Tmp );

	u32Tmp = UTL_SHL_SetValue( u32Tmp, u32Msb, u32Lsb, u32Val );

	HAL_WRITE_UINT32( u32Addr, u32Tmp );
}

static panel_param_blk panel_param_block = {0};

bool _VID_PowerON_Sequence()
{
	int index;
	int ret = 0;
    bool b8GetFromTxt = GET_PANEL_TABLE_FROM_EXTERNAL;
    
	ret = nvt_eburner_panel_param_init(&index, &panel_param_block);

	if(index < 0)
	{
	    b8GetFromTxt = GET_PANEL_TABLE_FROM_INTERNAL;
		printf("use default panel param\n");
	}

       /////////////////////////////////////////////

      // do.1 : get panel table
	_VID_Get_PanelTablePara( b8GetFromTxt );
      
	// do.2 : set panel power on
	_VID_Set_PanelPowerOn();  

	// do.3 : set MPLL
	_VID_Set_MPLL( gstPanelSetting.u32PLL );    

	// do.4 : set panel
	_VID_Set_Panel( &gstPanelSetting );

	// do.5 : set LVDS_PATH_SEL module for LVDS/VB1 digital circuit
	_VID_Set_LvdsPathSel( &gstPanelSetting );

	// do.6 : set combo tx module for LVDS/VB1 analog circuit
	_VID_Set_ComboTx();

    /* ZY: do it in  panel_init_late */
	// do.7 : set scaler init here if needed (not related to LVDS or DP setting ...)
	//_VID_Set_ScalerInit();

    // do.8 turn on backlight
    // _VID_Set_Backlight();
    
    
	return 1;
}

/*!\fn void _VID_Get_PanelTablePara( bool b8GetFromTxt )
 * \brief
 * Get panel table parameter
 *
 * \param b8GetFromTxt	(Input) 1 : Get panel table from txt file
 *								0 : Get panel table by switch case of _VID_Get_PanelTablePara()
 * \param cfg			(Input) configurable parameters for flash device
 *
 * \return None
 *
 */
void _VID_Get_PanelTablePara( bool b8GetFromTxt )
{
	int ret = 0;

    if(gbPanelParamSettingDone)
        return;

	if( b8GetFromTxt == GET_PANEL_TABLE_FROM_INTERNAL )
	{
		gstPanelSetting.VX1 = 1;
		gstPanelSetting.SEP = 0;
		gstPanelSetting.u16Width 				= 		3840;
		gstPanelSetting.u16Height				= 		2160;
		gstPanelSetting.u16TypHTotal			= 		4400;
		gstPanelSetting.u8MinHSyncWidth		= 		22;
		gstPanelSetting.u16HSyncStart			= 		80;
		gstPanelSetting.u16TypVTotal			= 		2250;
		gstPanelSetting.u8MinVSyncWidth		=		5;
		gstPanelSetting.u16VSyncStart			=		21;
		gstPanelSetting.b8DualPixelOutput		=		1;
		gstPanelSetting.b8ABSwap				=		0;
		gstPanelSetting.b8ChannelSwap			=		0;
		gstPanelSetting.b8RBSwap				=		0;
		gstPanelSetting.u32PLL				=		594000000;
		gstPanelSetting.enLvdsFormat 			= 		EN_DRV_SCLR_LVDS_FMT_8BITS_VESA;
		gstPanelSetting.u8MISC				=		0;
		gstPanelSetting.PanelPowerOnDelay1 	= 		30;
		gstPanelSetting.u8Tcon                        =           0;
		gstPanelSetting.u16PanelPowerGPIO        =           1033; 
		gstPanelSetting.u16PanelBackLightGPIO    =           34318;        
	}
	else if( b8GetFromTxt == GET_PANEL_TABLE_FROM_EXTERNAL )
	{
		
		gstPanelSetting.VX1 = panel_param_block.p.VX1;
		gstPanelSetting.SEP = panel_param_block.p.SEP;

        if(panel_param_block.p.b8Mode50Hz == 1)
        {
            printf("b8Mode50HZ\n");
            gstPanelSetting.u16Width = panel_param_block.p.stMode50Hz.u16Width;
            gstPanelSetting.u16Height = panel_param_block.p.stMode50Hz.u16Height;
            gstPanelSetting.u16TypHTotal = panel_param_block.p.stMode50Hz.u16TypHTotal;
            gstPanelSetting.u8MinHSyncWidth = panel_param_block.p.stMode50Hz.u8HSyncWidth;
            gstPanelSetting.u16HSyncStart = panel_param_block.p.stMode50Hz.u16HSyncStart;
            gstPanelSetting.u16TypVTotal = panel_param_block.p.stMode50Hz.u16TypVTotal;
            gstPanelSetting.u8MinVSyncWidth = panel_param_block.p.stMode50Hz.u8VSyncWidth;
            gstPanelSetting.u16VSyncStart = panel_param_block.p.stMode50Hz.u16VSyncStart;
            gstPanelSetting.u32PLL = panel_param_block.p.stMode50Hz.u16PLL*10000;
        }
        else
        {
            printf("b8Mode60HZ\n");
    		gstPanelSetting.u16Width = panel_param_block.p.stMode60Hz.u16Width;
    		gstPanelSetting.u16Height = panel_param_block.p.stMode60Hz.u16Height;
    		gstPanelSetting.u16TypHTotal = panel_param_block.p.stMode60Hz.u16TypHTotal;
    		gstPanelSetting.u8MinHSyncWidth = panel_param_block.p.stMode60Hz.u8HSyncWidth;
    		gstPanelSetting.u16HSyncStart = panel_param_block.p.stMode60Hz.u16HSyncStart;
    		gstPanelSetting.u16TypVTotal = panel_param_block.p.stMode60Hz.u16TypVTotal;
    		gstPanelSetting.u8MinVSyncWidth = panel_param_block.p.stMode60Hz.u8VSyncWidth;
    		gstPanelSetting.u16VSyncStart = panel_param_block.p.stMode60Hz.u16VSyncStart;
    		gstPanelSetting.u32PLL = panel_param_block.p.stMode60Hz.u16PLL*10000;
        }
        
		gstPanelSetting.b8DualPixelOutput = panel_param_block.p.b8DualPixelOutput;
		gstPanelSetting.b8ABSwap = panel_param_block.p.b8ABSwap;
		gstPanelSetting.b8ChannelSwap = panel_param_block.p.b8ChannelSwap;
		gstPanelSetting.b8RBSwap = panel_param_block.p.b8RBSwap;
		gstPanelSetting.enLvdsFormat = panel_param_block.p.enLvdsFormat;
		gstPanelSetting.u8MISC = panel_param_block.p.u8MISC;
		gstPanelSetting.PanelPowerOnDelay1 = panel_param_block.p.u16PanelPowerOnDelay1;
		gstPanelSetting.u8Tcon = panel_param_block.p.u8Tcon;
		gstPanelSetting.u16PanelPowerGPIO = panel_param_block.p.u16PanelPowerGPIO; 
		gstPanelSetting.u16PanelBackLightGPIO = panel_param_block.p.u16PanelBackLightGPIO;

					
		 
		
	}
	else
	{
		printf( "Unknow get panel table from where\n" );
	}

    printf("VX1 = %d \n",gstPanelSetting.VX1);
    printf("SEP = %d \n",gstPanelSetting.SEP);
    printf("u16Width = %d \n",gstPanelSetting.u16Width);
    printf("u16Height = %d \n",gstPanelSetting.u16Height);
    printf("u16TypHTotal = %d \n",gstPanelSetting.u16TypHTotal);
    printf("u8MinHSyncWidth = %d \n",gstPanelSetting.u8MinHSyncWidth);
    printf("u16HSyncStart = %d \n",gstPanelSetting.u16HSyncStart);
    printf("u16TypVTotal = %d \n",gstPanelSetting.u16TypVTotal);
    printf("u8MinVSyncWidth = %d \n",gstPanelSetting.u8MinVSyncWidth);
    printf("u16VSyncStart = %d \n",gstPanelSetting.u16VSyncStart);
    printf("b8DualPixelOutput = %d \n",gstPanelSetting.b8DualPixelOutput);
    printf("b8ABSwap = %d \n",gstPanelSetting.b8ABSwap);
    printf("b8ChannelSwap = %d \n",gstPanelSetting.b8ChannelSwap);
    printf("b8RBSwap = %d \n",gstPanelSetting.b8RBSwap);
    printf("u32PLL = %d \n",gstPanelSetting.u32PLL);
    printf("enLvdsFormat = %d \n",gstPanelSetting.enLvdsFormat);
    printf("u8MISC = %d \n",gstPanelSetting.u8MISC);
    printf("PanelPowerOnDelay1 = %d \n",gstPanelSetting.PanelPowerOnDelay1);
    printf("u8Tcon = %d \n",gstPanelSetting.u8Tcon);
    printf("u16PanelPowerGPIO = 0x%x \n",gstPanelSetting.u16PanelPowerGPIO);
    printf("u16PanelBackLightGPIO = 0x%x \n",gstPanelSetting.u16PanelBackLightGPIO);

    gbPanelParamSettingDone = 1;
}

/*!\fn void _VID_Set_PanelPowerOn( void )
 * \brief
 * Set panel power on
 *
 * \param None
 *
 * \return None
 *
 */
void _VID_Set_PanelPowerOn( void )
{
	u8 u8LSB;
	u8 u8MSB;

	u8MSB = gstPanelSetting.u16PanelPowerGPIO>>8;
	u8LSB =  gstPanelSetting.u16PanelPowerGPIO&0x00FF;
    
	printf("Panel Power1 GPIO,MSB=0x%x , LSB=%d\r\n",u8MSB,u8LSB);
	printf("Panel BackLight GPIO=%d\r\n",gstPanelSetting.u16PanelBackLightGPIO);

	if(u8MSB==4)    //GPIO E
	{
		GPE_DIR_OUTPUT(u8LSB);
        GPE_OUTPUT_SET(u8LSB);
	}
	else if(u8MSB==1) //GPIO B
	{
		GPB_DIR_OUTPUT(u8LSB);
		GPB_OUTPUT_SET(u8LSB);        
	} 
	else//673 TK Board GPIO panel power on GPE4 
	{
		printf("Panel Power GPIO group not set, use default GPE4");  
		GPE_DIR_OUTPUT(4);
		GPE_OUTPUT_SET(4);
	}


	// For 673TKB 1.0 must delay over 100ms
	mdelay(100);//gstPanelSetting.PanelPowerOnDelay1);
}

/*!\fn void _VID_Set_MPLL( u32 u32PLL )
 * \brief
 * Set MPLL
 *
 * \param u32PLL	(Input) MPLL value
 *
 * \return None
 *
 */
void _VID_Set_MPLL( u32 u32PLL )
{
	u32 u32Ratio;

	// the input 'u32PLL' is a one-port view
	// 673 DP is a two-port architecture
	u32Ratio = (u32PLL / 2) /100000;
	u32Ratio= (u32Ratio*0x20000)/120;

	_MPLL_EnablePage0();

	//LVDS_CLK
	_MPLL_SetData((0xFD6703C8 & 0xFFFF) / 4, (u32Ratio >>  0) & 0xFF); //Ratio[7:0]
	_MPLL_SetData((0xFD6703CC & 0xFFFF) / 4, (u32Ratio >>  8) & 0xFF); //Ratio[15:8]
	_MPLL_SetData((0xFD6703D0 & 0xFFFF) / 4, (u32Ratio >> 16) & 0xFF); //Ratio[23:16]

	//FRC_CLK
	_MPLL_SetData((0xFD6701AC & 0xFFFF) / 4, (u32Ratio >>  0) & 0xFF); //Ratio[7:0]
	_MPLL_SetData((0xFD6701B0 & 0xFFFF) / 4, (u32Ratio >>  8) & 0xFF); //Ratio[15:8]
	_MPLL_SetData((0xFD6701B4 & 0xFFFF) / 4, (u32Ratio >> 16) & 0xFF); //Ratio[23:16]

	_MPLL_EnablePageB();

	//2017-0824, jaydan says vx1/lvds/epi/usit all need it
	//clk mux register
	HAL_WRITE_UINT32(0xfd020134, 0x00000000);
	HAL_WRITE_UINT32(0xfd020138, 0x00000033);

	//2017-0914, osd lbm clk use atv frc clk for 4K output
	UTL_SHL_WriteRegMask( 0xFD0200FC, 6, 5, 0x2 );

}

/*!\fn void _VID_Set_Panel( struct ST_KER_VID_PANEL_SETTING *pstPanelSetting )
 * \brief
 * Set panel
 *
 * \param pstPanelSetting	(Input) Panel setting
 *
 * \return None
 *
 */
void _VID_Set_Panel( struct ST_KER_VID_PANEL_SETTING *pstPanelSetting )
{
	if(pstPanelSetting == NULL)
	{
		return;
	}

	//puts(" _VID_DP_SetupPanel() \n");

	// h/v total
	HAL_WRITE_UINT32(0xFE000F00, (pstPanelSetting->u16TypVTotal << 16) | (pstPanelSetting->u16TypHTotal / 2));

	// h/v sync pulse width
	HAL_WRITE_UINT32(0xFE000F04, (pstPanelSetting->u8MinVSyncWidth << 16) | (pstPanelSetting->u8MinHSyncWidth/2));

	// h/v back porch
	HAL_WRITE_UINT32(0xFE000F08, (pstPanelSetting->u16VSyncStart << 16) | (pstPanelSetting->u16HSyncStart/2));

	// frame window size
	HAL_WRITE_UINT32(0xFE000F0C, (pstPanelSetting->u16Height << 16) | (pstPanelSetting->u16Width/2));

	// dpw0 position
	HAL_WRITE_UINT32(0xFE000F10, 0);

	// dpw0 size
	HAL_WRITE_UINT32(0xFE000F14, (pstPanelSetting->u16Height << 16) | (pstPanelSetting->u16Width/2));

	// panel window size, the same as frame window size
	HAL_WRITE_UINT32(0xFE000FEC, (pstPanelSetting->u16Height << 16) | (pstPanelSetting->u16Width/2));

	// ENABLE panel window
	// panel window position, the same as h/v back porch
	HAL_WRITE_UINT32(0xFE000FFC, (1 << 31) | (pstPanelSetting->u16VSyncStart << 16) | (pstPanelSetting->u16HSyncStart/2));

	// DISABLE frame sync
	// 0F20[20] = 0, DP htotal reference DP side
	// 0F20[20] = 1, DP htotal reference FRC side
	// if we want to let DP in free run mode, 0F20[20] should be 0
	HAL_WRITE_UINT32(0xFE000F20, ((pstPanelSetting->u8FrameSyncMode & 0x3) << 16) | (0 << 20));

	// DISABLE dpw0/dpw1
	UTL_SHL_WriteRegMask(0xFE000F30, 24, 24, 0);
	UTL_SHL_WriteRegMask(0xFE000F30, 25, 25, 0);

	// frame window color - BLACK
	HAL_WRITE_UINT32(0xFE000F34, 0);

	// ENABLE blue screen
	UTL_SHL_WriteRegMask(0xFE000F98, 0, 0, 1);
	UTL_SHL_WriteRegMask(0xFE000F98, 1, 1, 1);

	// blue screen color - BLACK
	UTL_SHL_WriteRegMask(0xFE000F98,  9,  8, 0);
	UTL_SHL_WriteRegMask(0xFE000F98, 13, 12, 0);
	UTL_SHL_WriteRegMask(0xFE000F98, 17, 16, 0);
	UTL_SHL_WriteRegMask(0xFE000F98, 21, 20, 0);
	UTL_SHL_WriteRegMask(0xFE000F98, 25, 24, 0);
	UTL_SHL_WriteRegMask(0xFE000F98, 29, 28, 0);
	HAL_WRITE_UINT32(0xFE000F9C, 0);
	HAL_WRITE_UINT32(0xFE000FA0, 0);

	// ABswap, even/odd port swap
	UTL_SHL_WriteRegMask(0xFE000F60, 5, 5, !!pstPanelSetting->b8ABSwap);

	// RBswap
	UTL_SHL_WriteRegMask(0xFE000F60, 1, 1, !!pstPanelSetting->b8RBSwap);

	// NOTE: 673 DP has no this h/w function
	// ignore 'pstPanelSetting->b8ChannelSwap'

	// NOTE: 673 dual-port output only
	// ignore 'pstPanelSetting->b8DualPixelOutput'

	// path mux settings
	UTL_SHL_WriteRegMask(0xFE000F44,  4,  4, 0); //DON'T bypass OSD, due to fast-logo
	UTL_SHL_WriteRegMask(0xFE000F44,  9,  9, 1); //bypass OD
	UTL_SHL_WriteRegMask(0xFE000F44, 12, 12, 1); //bypass tcon
	UTL_SHL_WriteRegMask(0xFE000F44, 13, 13, 1); //bypass bandwidth meter

	// LVDS format
	_Out_P_SetLvdsFormat( pstPanelSetting->enLvdsFormat );

	// dithering
	_Img_P_SetVideoOutputDither( pstPanelSetting->eDitheringBit );
}

/*!\fn void _VID_Set_LvdsPathSel( struct ST_KER_VID_PANEL_SETTING *
pstPanelSetting )
 * \brief
 * Set LVDS path select module for LVDS/VB1 digital circuit
 *
 * \param pstPanelSetting	(Input) Panel setting
 *
 * \return None
 *
 */
void _VID_Set_LvdsPathSel( struct ST_KER_VID_PANEL_SETTING *pstPanelSetting )
{
	if(gstPanelSetting.VX1) //Vx1 interface output
	{
		// LVDS_PATH_SEL resolution setting _ 3840x2160
		if(gstPanelSetting.u8Tcon == 2)
		{
			printf("Set 2 Tcon mode \n");  
			HAL_WRITE_UINT32(0xfd6a0030, 0x01870780);
			HAL_WRITE_UINT32(0xfd6a0024, 0x01870780);
		}
		else
		{
			HAL_WRITE_UINT32(0xfd6a0030, 0x00870780);
			HAL_WRITE_UINT32(0xfd6a0024, 0x00870780);
		}

		// VX1 output_combination : ATV 8lane
		HAL_WRITE_UINT32(0xfd6a0010, 0x00016000);
	}
	else //LVDS interface output
	{
		// LVDS_PATH_SEL resolution setting _ FHD
		HAL_WRITE_UINT32(0xfd6a0030, 0x4383C0);
		HAL_WRITE_UINT32(0xfd6a0024, 0x4383C0);

		// LVDS enable repomatter
		HAL_WRITE_UINT32(0xfd6a0000, 0x00080001);
	}

#if 0 // FIXME: add if-else-condition for USIT interface output
	// LVDS Output For Tcon Display
	HAL_WRITE_UINT32(0xFD020210, 0x00000001);
	HAL_WRITE_UINT32(0xFD020314, 0x00000002);
	HAL_WRITE_UINT32(0xfd020314, 0x00000000);
	HAL_WRITE_UINT32(0xFD020210, 0x00000000);

	// #SFC
	// #FD10_00A0[6:4] = 0x6         //sfc
	// #FD10_00A0[10:8] = 0x6             //lock
	// #nt71789_sfc_open_en & nt71789_sfc_sd_sel
	HAL_WRITE_UINT32(0xfd1000a0, 0x00000060);
#endif
}

/*!\fn void _VID_Set_ComboTx( void )
 * \brief
 * Set combo tx module for LVDS/VB1 analog circuit
 *
 * \param None
 *
 * \return None
 *
 */
void _VID_Set_ComboTx( void )
{
	if(gstPanelSetting.VX1) //Vx1 interface output
	{
		printf("Uboot Panel Set VX1\r\n"); 
		_VID_Set_ComboTx_Vx1();

	}
	else //LVDS interface output
	{
		printf("Uboot Panel Set LVDS\r\n"); 
		_VID_Set_ComboTx_LVDS();

	}

	//kwinyee debug
	/* these code are to specify 673 TKB v1.0 / v1.1 */
	if(gstPanelSetting.u8MISC == 8)
	{
		printf("-> MB220 PCB, no need apply patch %s\n", __FILE__);
	}
	else
	{
		switch(lvds_get_673_board_type())
		{
			case NVT673_TKB_V1_0:
				{
					printf("-> TKB_V1_0, apply patch @ %s\n", __FILE__);

					/* this settings are for 673 TK old-PCB, from SE1's requirement */
					//#==============================
					//# HTPDN & LOCKN pull low
					//#==============================
					HAL_WRITE_UINT32(0xfd100110, 0x00017f07); //DPHY 3port control
					HAL_WRITE_UINT32(0xFD100110, 0x0001010C); //HTPDN & LOCKN internal enable
					HAL_WRITE_UINT32(0xfd100110, 0x00010004); //HTPDN internal pull low
					mdelay(1);
					HAL_WRITE_UINT32(0xfd100110, 0x00010000); //LOCKN internal pull low
					break;
				}

			case NVT673_TKB_V1_1:
				{
					printf("-> TKB_V1_1, %s\n", __FILE__);
					break;
				}

			case NVT673_TKB_V3_1:
				printf("-> TKB_V3_1, %s\n", __FILE__);
				break;

			case N62002_V00:
				printf("-> N62002_V00_BOARD, %s\n", __FILE__);
				break;

			case NVT673_TKB_V_MAX:
			default:
				{
					printf("-> TKB_V_MAX, %s\n", __FILE__);
					break;
				}		}
	}
}


/*!\fn void _VID_Set_ScalerInit( void )
 * \brief
 * Set init value for scaler needed
 *
 * \param None
 *
 * \return None
 *
 */
void _VID_Set_ScalerInit( void )
{

    HAL_WRITE_UINT32(0xFE000F60, 0x80003F04);
    _VID_Set_OSDDisplayWindow();
}

void _VID_Set_STBC_Gpio(u8 u8Msb, u8 u8Lsb)
{
    u8 u8GpioInverse;
    int emptyBurnerMode = nvt_in_empty_burner_mode();
    
    if(u8Msb & 0x80)
    {
        u8GpioInverse = 1;
        printf("Inverse the GPIO\n");
    }
    else
    {
        u8GpioInverse = 0;
    }

    if(emptyBurnerMode)
    {    
        printf("In empty burn mode\n");
        
    	//switch authority
    	nvt_stbc_set_keypass(1);
    	REG_STBC_GPIO_AUTHORITY = STBC_DEDICATED_GPIO_SWITCH_TO_ARM;
        GPIO_SWITCH_CTRL |= STBC_ALTERNATIVE_GPIO_SWITCH_TO_ARM;

        if((u8Msb&0x0F) == EN_DRV_SCLR_GPIO_GROUP_F)
        {
            GPIO_F_CTRL_DIR |= (1 << u8Lsb);
            GPIO_F_CTRL_SWITCH |= (1 << u8Lsb);

            if(u8GpioInverse)
            {
                GPIO_F_CTRL_LEVEL &= ~(1 << u8Lsb);
            }
            else
            {
                GPIO_F_CTRL_LEVEL |= (1 << u8Lsb);
            }
        }
        else if((u8Msb&0x0F) == EN_DRV_SCLR_GPIO_GROUP_G)
        {
            GPIO_G_CTRL_DIR |= (1 << u8Lsb);
            GPIO_G_CTRL_SWITCH |= (1 << u8Lsb);

            if(u8GpioInverse)
            {
                GPIO_G_CTRL_LEVEL &= ~(1 << u8Lsb);
            }
            else
            {
                GPIO_G_CTRL_LEVEL |= (1 << u8Lsb);
            }
        }
        else if((u8Msb&0x0F) == EN_DRV_SCLR_GPIO_GROUP_I)
        {
            GPIO_I_CTRL_DIR |= (1 << u8Lsb);
            GPIO_I_CTRL_SWITCH |= (1 << u8Lsb);

            if(u8GpioInverse)
            {
                GPIO_I_CTRL_LEVEL &= ~(1 << u8Lsb);
            }
            else
            {
                GPIO_I_CTRL_LEVEL |= (1 << u8Lsb);
            }
        }
    }
    else
    {
        printf("Using command to upgrade\n");

        if((u8Msb&0x0F) == EN_DRV_SCLR_GPIO_GROUP_F)
        {
            nvt_stbc_set_gpio(PIN_GPF_0 + u8Lsb, (u8GpioInverse ? EN_GPIO_CTRL_GPIO_OUTPUT_LOW : EN_GPIO_CTRL_GPIO_OUTPUT_HIGH));
        }
        else if((u8Msb&0x0F) == EN_DRV_SCLR_GPIO_GROUP_G)
        {
            nvt_stbc_set_gpio(PIN_GPG_0 + u8Lsb, (u8GpioInverse ? EN_GPIO_CTRL_GPIO_OUTPUT_LOW : EN_GPIO_CTRL_GPIO_OUTPUT_HIGH));
        }
        else if((u8Msb&0x0F) == EN_DRV_SCLR_GPIO_GROUP_I)
        {
            nvt_stbc_set_gpio(PIN_GPI_0 + u8Lsb, (u8GpioInverse ? EN_GPIO_CTRL_GPIO_OUTPUT_LOW : EN_GPIO_CTRL_GPIO_OUTPUT_HIGH));
        }        
    }
    
    return;
}

/*!\fn void _VID_Set_Backlight( void )
 * \brief
 * Set init value for scaler needed
 *
 * \param None
 *
 * \return None
 *
 */
void _VID_Set_Backlight( void )
{
    u8 u8LSB;
    u8 u8MSB;
    
	u8MSB = gstPanelSetting.u16PanelBackLightGPIO>>8;
	u8LSB =  gstPanelSetting.u16PanelBackLightGPIO&0x00FF;
    printf("Panel Backlight GPIO,MSB=0x%x , LSB=%d\r\n",u8MSB,u8LSB);

	if((u8MSB&0x0F)==4)    //GPIO E
	{
		GPE_DIR_OUTPUT(u8LSB);
		GPE_OUTPUT_SET(u8LSB);
	}
	else if((u8MSB&0x0F)==1) //GPIO B
	{
        if(u8LSB==8 )
        {
            GPB_FUN_SET(0);
        }
        GPB_DIR_OUTPUT(u8LSB);

        if( u8MSB & 0x80)
        {
            GPB_OUTPUT_CLEAR(u8LSB);
        }
        else
        {
            GPB_OUTPUT_SET(u8LSB);
        }
	}
    else if(((u8MSB&0x0F) > EN_DRV_SCLR_GPIO_GROUP_E && (u8MSB&0x0F) < EN_DRV_SCLR_GPIO_GROUP_H)
            || (u8MSB&0x0F) == EN_DRV_SCLR_GPIO_GROUP_I)
    {
        /* STBC side gpio */
        _VID_Set_STBC_Gpio(u8MSB, u8LSB);
    }
}


void _Out_P_SetLvdsFormat( EN_DRV_SCLR_LVDS_FMT enFormat )
{
	u32 u32TenBits;
	u32 u32Format;

	if ( enFormat >= EN_DRV_SCLR_LVDS_FMT_TOTAL )
	{
		printf( "_Out_P_SetLvdsFormat() ERROR\n" );

		return;
	}

	switch ( enFormat )
	{
		case EN_DRV_SCLR_LVDS_FMT_10BITS_VESA:
			//puts( "LVDS_FMT_10BITS_VESA\n" );
			u32TenBits = 1;
			u32Format = 1;
			break;

		case EN_DRV_SCLR_LVDS_FMT_10BITS_JEIDA:
			//puts( "LVDS_FMT_10BITS_JEIDA\n" );
			u32TenBits = 1;
			u32Format = 2;
			break;

		case EN_DRV_SCLR_LVDS_FMT_10BITS_NS:
			//puts( "LVDS_FMT_10BITS_NS\n" );
			u32TenBits = 1;
			u32Format = 0;
			break;

		case EN_DRV_SCLR_LVDS_FMT_8BITS_VESA:
			//puts( "LVDS_FMT_8BITS_VESA\n" );
			u32TenBits = 0;
			u32Format = 0;
			break;

		case EN_DRV_SCLR_LVDS_FMT_8BITS_JEIDA:
			//puts( "LVDS_FMT_8BITS_JEIDA:\n" );
			u32TenBits = 0;
			u32Format = 2;
			break;

		default :
			//puts( "Default: LVDS_FMT_10BITS_NS\n" );
			u32TenBits = 1;
			u32Format = 0;

			break;
	}

	UTL_SHL_WriteRegMask( 0xFE000F60, 2, 2, u32TenBits );
	UTL_SHL_WriteRegMask( 0xFE000F60, 4, 3, u32Format );
}

void _Img_P_SetVideoOutputDither( EN_DRV_SCLR_VIDEO_OUTPUT_DITHER enDither )
{
	if ( enDither <= EN_DRV_SCLR_VIDEO_OUTPUT_DITHER_SCALER_DITHER_6BITS )
	{
		// scaler dithering
		u32 u32Lsb4BitsToDither = 0;
		u32 u32InputBusWidth12To10Bits = 0;

		if ( enDither == EN_DRV_SCLR_VIDEO_OUTPUT_DITHER_SCALER_DITHER_10BITS )
		{
			u32Lsb4BitsToDither = 0;
			u32InputBusWidth12To10Bits = 0;
		}
		else if ( enDither == EN_DRV_SCLR_VIDEO_OUTPUT_DITHER_SCALER_DITHER_8BITS )
		{
			u32Lsb4BitsToDither = 1;
			u32InputBusWidth12To10Bits = 0;
		}
		else // 6bits
		{
			u32Lsb4BitsToDither = 1;
			u32InputBusWidth12To10Bits = 1;
		}

		UTL_SHL_WriteRegMask( 0xFE000F40, 16, 16, u32Lsb4BitsToDither );
		UTL_SHL_WriteRegMask( 0xFE000F40, 18, 18, u32InputBusWidth12To10Bits ); // toggle this bit will have spot noise
		UTL_SHL_WriteRegMask( 0xFE000F40, 19, 19, 1 );
		HAL_WRITE_UINT32( 0xFE000F48, 0x00970003 );
	}
}

static void _VID_Set_ComboTx_Vx1( void )
{
	// VX1 HTPDN,LOCKN setting
	// ####setting GPB 2,3 / GPB 4,5 => HTPDN,LOCKN
	// ####HTPDN_Video GPB2 func sel = 0xfd0f0060[11:8]
	// ####HTPDN_OSD   GPB3 function sel = 0xfd0f0060[15:12]
	// ####LOCKN_Video GPB4 function sel = 0xfd0f0060[19:16]
	// ####LOCKN_OSD   GPB5 function sel = 0xfd0f0060[23:20]
	UTL_SHL_WriteRegMask(0xfd0f0060, 11,  8, 2);
	UTL_SHL_WriteRegMask(0xfd0f0060, 19, 16, 2);

	// Reset V_BY_ONE_RSTB 
	HAL_WRITE_UINT32( 0xfd022270, 0x00000300 ); //write 1 to the relevent bit directly, DON'T use bitmask

	// #  VX1 TX PHY setting
	// #  VXI_HI:0xFD100110
	// # APHY_HI:0xFD100108
	// #  P2P_HI:0xFD10011C
	HAL_WRITE_UINT32(0xfd100108, 0x00010201); //bias power on
	HAL_WRITE_UINT32(0xfd100108, 0x00019FF1); //VB_VX1 select
	HAL_WRITE_UINT32(0xfd100108, 0x000147f1); //power on TX/VCO LDO
	HAL_WRITE_UINT32(0xfd100108, 0x00015c07); //power on PLL
	HAL_WRITE_UINT32(0xfd100108, 0x00012503); //enable ODP

	// # APLL setting
	HAL_WRITE_UINT32(0xfd100108, 0x00012640);
	HAL_WRITE_UINT32(0xfd100108, 0x00012740);
	mdelay(1);

	HAL_WRITE_UINT32(0xfd100108, 0x00015c37); //reset PLL
	HAL_WRITE_UINT32(0xfd100108, 0x00012300); //Disable PI(CK)
	HAL_WRITE_UINT32(0xfd100108, 0x00012400); //Disable PI(Data)
	HAL_WRITE_UINT32(0xfd100108, 0x00015E00); //Disable output pull-low(portB/C)
	HAL_WRITE_UINT32(0xfd100108, 0x00015f00); //Disable output pull-low(portB/C)
	HAL_WRITE_UINT32(0xfd100108, 0x00016410); //select R50(port B/C)
	HAL_WRITE_UINT32(0xfd100108, 0x00016510); //select R50(port B/C)
	HAL_WRITE_UINT32(0xfd100108, 0x00012563); //Enable ZTX(R50)
	HAL_WRITE_UINT32(0xfd100108, 0x00010B00); //CLK5X selection
	HAL_WRITE_UINT32(0xfd100108, 0x00010C3C); //CLK5X selection
	HAL_WRITE_UINT32(0xfd100108, 0x0001067e); //TX output buffer power on
	HAL_WRITE_UINT32(0xfd100108, 0x00010703); //TX output buffer power on
	HAL_WRITE_UINT32(0xfd100108, 0x00014D7E); //P2S power on
	HAL_WRITE_UINT32(0xfd100108, 0x00014E03); //P2S power on
	HAL_WRITE_UINT32(0xfd100108, 0x0001337E); //enable TX output
	HAL_WRITE_UINT32(0xfd100108, 0x00013403); //enable TX output

	// Release V_BY_ONE_RSTB 
	HAL_WRITE_UINT32( 0xfd022210, 0x00000300 ); //write 1 to the relevent bit directly, DON'T use bitmask

	HAL_WRITE_UINT32( 0xfd100104, 0x0000003a ); // #4byte mode
}

static void _VID_Set_ComboTx_LVDS( void )
{
	// Reset LVDS_RSTB
	HAL_WRITE_UINT32(0xfd022270, 0x00000400); //write 1 to the relevent bit directly, DON'T use bitmask

	// #  LVDS TX PHY setting
	// #  VXI_HI:0xFD100110
	// # APHY_HI:0xFD100108
	// #  P2P_HI:0xFD10011C
	HAL_WRITE_UINT32(0xfd100110, 0x00010010); // switch APLL source : DP_CLK
	HAL_WRITE_UINT32(0xfd100108, 0x00010201); // bias power on
	HAL_WRITE_UINT32(0xfd100108, 0x00019FF1); // VB_VX1 select
	HAL_WRITE_UINT32(0xfd100108, 0x00014771); // power on TX/VCO LDO
	HAL_WRITE_UINT32(0xfd100108, 0x00013731); // en_Rint
	HAL_WRITE_UINT32(0xfd100108, 0x00015c07); // power on PLL
	HAL_WRITE_UINT32(0xfd100108, 0x00012d00); // disable VCO high-speed mode
	HAL_WRITE_UINT32(0xfd100108, 0x00013c0c); // disable loop filter high speed mode
	HAL_WRITE_UINT32(0xfd100108, 0x00012503); // enable ODP
	HAL_WRITE_UINT32(0xfd100108, 0x00014a88); // set feedback DIV LVDS mode

	mdelay(1);

	HAL_WRITE_UINT32(0xfd100108, 0x00015c17); // reset PLL
	HAL_WRITE_UINT32(0xfd100108, 0x00012300); // Disable PI(CK)
	HAL_WRITE_UINT32(0xfd100108, 0x00012400); // Disable PI(Data)
	HAL_WRITE_UINT32(0xfd100108, 0x00019e60); // change TX mode to MLVDS
	HAL_WRITE_UINT32(0xfd100108, 0x00016701); // P2S auto mode
	HAL_WRITE_UINT32(0xfd100108, 0x00015E00); // Disable output pull-low(portB/C)
	HAL_WRITE_UINT32(0xfd100108, 0x00015f00); // Disable output pull-low(portB/C)
	HAL_WRITE_UINT32(0xfd100108, 0x0001067e); // TX output buffer power on 
	HAL_WRITE_UINT32(0xfd100108, 0x0001073f); // TX output buffer power on 
	HAL_WRITE_UINT32(0xfd100108, 0x00014894); // P2S power on 
	HAL_WRITE_UINT32(0xfd100108, 0x0001337E); // enable TX output
	HAL_WRITE_UINT32(0xfd100108, 0x0001343f); // enable TX output

	// Release LVDS_RSTB
	HAL_WRITE_UINT32(0xfd022210, 0x00000400); //write 1 to the relevent bit directly, DON'T use bitmask

	// config '#4byte mode' with Vx1 output, but LVDS output DON'T need it
}

void _VID_Set_OSDDisplayWindow( void )
{
	if ( gstPanelSetting.VX1 == 0 )
	{
		if (gstPanelSetting.u16Width == 1366)
		{
			HAL_WRITE_UINT32(0xfc0d0118, 0x05560300);
			printf("set OSD display window 1366*768\n");
		}
		else
		{
			HAL_WRITE_UINT32(0xfc0d0118, 0x07800438);
			printf("set OSD display window 1920*1080\n");
		}

	}
  else //gstPanelSetting.VX1 == 1
  {
      HAL_WRITE_UINT32(0xfc0d0118, 0x0F000870);
			printf("set OSD display window 3840*2160\n");
  }
  
}


#if (_SHOW_TEST_PATTERN_ == _ENABLE_)
static void _Set_VEP_TestPattern(void)
{
	UTL_SHL_WriteRegMask( 0xFE007200, 27, 24, 0xC ); //0xC: 75% color bar
	UTL_SHL_WriteRegMask( 0xFE007200, 11, 11, 0x0 ); //0x0: YCbCr
	UTL_SHL_WriteRegMask( 0xFE007200,  7,  7, 1 ); //enable TP
}

static void _VID_DP_SetFrameWindowColor(u16 u16R, u16 u16G, u16 u16B)
{
	UTL_SHL_WriteRegMask( 0xFE000F34,  9,  0, u16R );
	UTL_SHL_WriteRegMask( 0xFE000F34, 19, 10, u16G );
	UTL_SHL_WriteRegMask( 0xFE000F34, 29, 20, u16B );
}

static void _VID_DP_EnableDisplayWindow(u32 u32Path, bool bEnable)
{
	if(0 == u32Path) {
		UTL_SHL_WriteRegMask( 0xFE000F30, 24, 24, !!bEnable);
	}
	else {
		UTL_SHL_WriteRegMask( 0xFE000F30, 25, 25, !!bEnable);
	}
}
#endif

void panel_init_late(void)
{
    int index;
    int ret = 0;
    bool b8GetFromTxt = GET_PANEL_TABLE_FROM_EXTERNAL;
    
    ret = nvt_eburner_panel_param_init(&index, &panel_param_block);

    if(index < 0)
    {
        b8GetFromTxt = GET_PANEL_TABLE_FROM_INTERNAL;
        printf("use default panel param\n");
    }

    _VID_Get_PanelTablePara( b8GetFromTxt );
    _VID_Set_ScalerInit();   
    _VID_Set_Backlight();

}
