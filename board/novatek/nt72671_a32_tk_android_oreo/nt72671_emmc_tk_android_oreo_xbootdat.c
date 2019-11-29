/*
 *  board/novatek/nt72671/nt72671_emmc_nvt_xbootdat.c
 *
 *  Author:	Alvin lin
 *  Created:	Jun 5, 2015
 *  Copyright:	Novatek Inc.
 *
 */

static int panel_xdat_help(struct _nvt_emmc_xbootdat_info *this, NVT_EMMC_XBOOTDAT_HELP_TYPE help_type);
static int panel_xdat_dump(struct _nvt_emmc_xbootdat_info *this, int argc, char* const argv[]);
static int panel_xdat_save(struct _nvt_emmc_xbootdat_info *this, int argc, char* const argv[]);
static int panel_xdat_erase(struct _nvt_emmc_xbootdat_info *this, int argc, char* const argv[]);
static int kercmd_xdat_help(struct _nvt_emmc_xbootdat_info *this, NVT_EMMC_XBOOTDAT_HELP_TYPE help_type);
static int kercmd_xdat_dump(struct _nvt_emmc_xbootdat_info *this, int argc, char* const argv[]);
static int kercmd_xdat_save(struct _nvt_emmc_xbootdat_info *this, int argc, char* const argv[]);
static int kercmd_xdat_erase(struct _nvt_emmc_xbootdat_info *this, int argc, char* const argv[]);
static int rcvcmd_xdat_help(struct _nvt_emmc_xbootdat_info *this, NVT_EMMC_XBOOTDAT_HELP_TYPE help_type);
static int rcvcmd_xdat_dump(struct _nvt_emmc_xbootdat_info *this, int argc, char* const argv[]);
static int rcvcmd_xdat_save(struct _nvt_emmc_xbootdat_info *this, int argc, char* const argv[]);
static int rcvcmd_xdat_erase(struct _nvt_emmc_xbootdat_info *this, int argc, char* const argv[]);


static  nvt_emmc_xbootdat_info xdat_info[] = {
	{
		//panel param
		.xdat_name = "panel",
		.xdathelp = panel_xdat_help,
		.xdatdump = panel_xdat_dump,
		.xdatsave = panel_xdat_save,
		.xdaterase = panel_xdat_erase,
		.start_sector = 0,
		.size = (PANEL_PARM_NUM + 1),
	},
	{
		//kernel command
		.xdat_name = "kercmd",
		.xdathelp = kercmd_xdat_help,
		.xdatdump = kercmd_xdat_dump,
		.xdatsave = kercmd_xdat_save,
		.xdaterase = kercmd_xdat_erase,
		.start_sector = (PANEL_PARM_NUM + 1),
		.size = 1,
	},
	{
		//recovery command
		.xdat_name = "rcvcmd",
		.xdathelp = rcvcmd_xdat_help,
		.xdatdump = rcvcmd_xdat_dump,
		.xdatsave = rcvcmd_xdat_save,
		.xdaterase = rcvcmd_xdat_erase,
		.start_sector = (PANEL_PARM_NUM + 2),
		.size = 1,
	},

};

static int panel_xdat_help(struct _nvt_emmc_xbootdat_info *this, NVT_EMMC_XBOOTDAT_HELP_TYPE help_type)
{
	int ret = 0;

	switch(help_type) {
		case XDAT_HELP_DUMP:
			printf("xdump panel - dump current panel select index and all panel settings\n");
			break;

		case XDAT_HELP_SAVE:
			printf("xsave panel index idx(0-9) - select panel config index for xboot use\n");
			printf("xsave panel config config_file_name - save panel config data into xbootdat\n");
			break;

		case XDAT_HELP_ERASE:
			printf("xerase panel - erase all panel settings and panel index, don't do this unless you know what you are doing\n");
			break;

		default:
			printf("unknown xbootdat panel help ?\n");
	}

	return ret;
}

static int panel_xdat_dump(struct _nvt_emmc_xbootdat_info *this, int argc, char* const argv[])
{
	int ret = 0;
	char cmd[128] = {0};
	panel_index_blk __attribute__((__aligned__(64))) pi_blk;
	panel_param_blk __attribute__((__aligned__(64))) pp_blk;
	int i;

	if(argc != 2) {
		printf("Usage: %s panel \n",argv[0]);
		ret = CMD_RET_USAGE;
		goto out;
	}

	//read index
	sprintf(cmd, "mmc read 0x%x 0x%x 0x%x", &pi_blk, this->start_sector, 1);
	ret = run_command(cmd, 0);
	if(ret < 0) {
		printf("%s read panel index fail !\n", __func__);
		goto out;
	}

	if(memcmp(pi_blk.idx.sign,PANEL_INDEX_SIGN, strlen(PANEL_INDEX_SIGN)) != 0) {
		printf("panel index not valid !\n");
		goto out;
	}

	printf("panel param version : %d\n", pi_blk.idx.version);
	printf("panel param index : %d\n", pi_blk.idx.index);

	for(i=0; i < PANEL_PARM_NUM ;i++) {
		memset(cmd, 0, sizeof(cmd));
		sprintf(cmd, "mmc read 0x%x 0x%x 0x%x", &pp_blk, this->start_sector + 1 + i, 1);
		ret = run_command(cmd, 0);
		if(ret < 0) {
			printf("%s read panel param error !\n", __func__);
			goto out;
		}

		if(memcmp(pp_blk.p.sign, PANEL_PARAM_SIGN, strlen(PANEL_PARAM_SIGN)) != 0) 
			break;
#if defined(CONFIG_OLD_PANEL_PARAM)
		printf("=====panel param index : %d=====\n", i);
		printf("version: 			%d\n", pp_blk.p.version);
		printf("u8PanelID: 			%d\n", pp_blk.p.u8PanelID);
		printf("b8Mode50Hz: 			%d\n", pp_blk.p.b8Mode50Hz);
		printf("u8Panel: 			%d\n", pp_blk.p.u8Panel);
		printf("au8Name: 			%s\n", pp_blk.p.au8Name);
		printf("b8DualPixelOutput: 		%d\n", pp_blk.p.b8DualPixelOutput);
		printf("b8ABSwap: 			%d\n", pp_blk.p.b8ABSwap);
		printf("b8ChannelSwap: 			%d\n", pp_blk.p.b8ChannelSwap);
		printf("b8FrameSyncEnable: 		%d\n", pp_blk.p.b8FrameSyncEnable);
		printf("u8FrameSyncMode: 		%d\n", pp_blk.p.u8FrameSyncMode);
		printf("u8MISC: 			%d\n", pp_blk.p.u8MISC);
		printf("enLvdsFormat: 			%d\n", pp_blk.p.enLvdsFormat);
		printf("b8RBSwap: 			%d\n", pp_blk.p.b8RBSwap);
		printf("u8DitheringBit: 		%d\n", pp_blk.p.u8DitheringBit);
		printf("u16LVDSPhase: 			%d\n", pp_blk.p.u16LVDSPhase);
		printf("u16LVDSPhase_PortA: 		%d\n", pp_blk.p.u16LVDSPhase_PortA);
		printf("u16LVDSPhase_PortB: 		%d\n", pp_blk.p.u16LVDSPhase_PortB);
		printf("u16VFreqDiff_Min: 		%d\n", pp_blk.p.u16VFreqDiff_Min);
		printf("u16VFreqDiff_Max: 		%d\n", pp_blk.p.u16VFreqDiff_Max);
		printf("u16VTotalDiff_Min: 		%d\n", pp_blk.p.u16VTotalDiff_Min);
		printf("u16VTotalDiff_Max: 		%d\n", pp_blk.p.u16VTotalDiff_Max);
		printf("stMode50Hz.u16Width: 		%d\n", pp_blk.p.stMode50Hz.u16Width);
		printf("stMode50Hz.u16Height: 		%d\n", pp_blk.p.stMode50Hz.u16Height);
		printf("stMode50Hz.u16TypHTotal: 	%d\n", pp_blk.p.stMode50Hz.u16TypHTotal);
		printf("stMode50Hz.u8HSyncWidth: 	%d\n", pp_blk.p.stMode50Hz.u8HSyncWidth);
		printf("stMode50Hz.u16HSyncBackPorch: 	%d\n", pp_blk.p.stMode50Hz.u16HSyncBackPorch);
		printf("stMode50Hz.u16HSyncStart: 	%d\n", pp_blk.p.stMode50Hz.u16HSyncStart);
		printf("stMode50Hz.u16TypVTotal: 	%d\n", pp_blk.p.stMode50Hz.u16TypVTotal);
		printf("stMode50Hz.u8VSyncWidth: 	%d\n", pp_blk.p.stMode50Hz.u8VSyncWidth);
		printf("stMode50Hz.u16VSyncBackPorch: 	%d\n", pp_blk.p.stMode50Hz.u16VSyncBackPorch);
		printf("stMode50Hz.u16VSyncStart: 	%d\n", pp_blk.p.stMode50Hz.u16VSyncStart);
		printf("stMode50Hz.u16PLL: 		%d\n", pp_blk.p.stMode50Hz.u16PLL);
		printf("stMode60Hz.u16Width: 		%d\n", pp_blk.p.stMode60Hz.u16Width);
		printf("stMode60Hz.u16Height: 		%d\n", pp_blk.p.stMode60Hz.u16Height);
		printf("stMode60Hz.u16TypHTotal: 	%d\n", pp_blk.p.stMode60Hz.u16TypHTotal);
		printf("stMode60Hz.u8HSyncWidth: 	%d\n", pp_blk.p.stMode60Hz.u8HSyncWidth);
		printf("stMode60Hz.u16HSyncBackPorch: 	%d\n", pp_blk.p.stMode60Hz.u16HSyncBackPorch);
		printf("stMode60Hz.u16HSyncStart: 	%d\n", pp_blk.p.stMode60Hz.u16HSyncStart);
		printf("stMode60Hz.u16TypVTotal: 	%d\n", pp_blk.p.stMode60Hz.u16TypVTotal);
		printf("stMode60Hz.u8VSyncWidth: 	%d\n", pp_blk.p.stMode60Hz.u8VSyncWidth);
		printf("stMode60Hz.u16VSyncBackPorch: 	%d\n", pp_blk.p.stMode60Hz.u16VSyncBackPorch);
		printf("stMode60Hz.u16VSyncStart: 	%d\n", pp_blk.p.stMode60Hz.u16VSyncStart);
		printf("stMode60Hz.u16PLL: 		%d\n", pp_blk.p.stMode60Hz.u16PLL);
		printf("u16PanelPowerOnDelay1: 		%d\n", pp_blk.p.u16PanelPowerOnDelay1);
		printf("u16PanelPowerOnDelay2: 		%d\n", pp_blk.p.u16PanelPowerOnDelay2);
		printf("u16PanelPowerOFFDelay1: 	%d\n", pp_blk.p.u16PanelPowerOFFDelay1);
		printf("u16PanelPowerOFFDelay2: 	%d\n", pp_blk.p.u16PanelPowerOFFDelay2);
		printf("u16BackLightLevel: 		%d\n", pp_blk.p.u16BackLightLevel);
		printf("b8PWMDutyInv: 			%d\n", pp_blk.p.b8PWMDutyInv);
		printf("b8HFlip: 			%d\n", pp_blk.p.b8HFlip);
		printf("b8VFlip: 			%d\n", pp_blk.p.b8VFlip);
		printf("b8OutputPanelFHD: 		%d\n", pp_blk.p.b8OutputPanelFHD);
		printf("VX1: 				%d\n", pp_blk.p.VX1);
		printf("FRC: 				%d\n", pp_blk.p.FRC);
		printf("SEP: 				%d\n", pp_blk.p.SEP);
		printf("b8Blinking: 			%d\n", pp_blk.p.b8Blinking);
		printf("u8PanelType: 			%d\n", pp_blk.p.u8PanelType);
		printf("b8ForcePanelTiming2D: 		%d\n", pp_blk.p.b8ForcePanelTiming2D);
		printf("b8ForcePanelTiming2DSport: 	%d\n", pp_blk.p.b8ForcePanelTiming2DSport);
		printf("b8ForcePanelTiming3D: 		%d\n", pp_blk.p.b8ForcePanelTiming3D);
		printf("u16PanelPowerGPIO: 		%d\n", pp_blk.p.u16PanelPowerGPIO);
		printf("u16PanelBackLightGPIO: 		%d\n", pp_blk.p.u16PanelBackLightGPIO);
		printf("u16PanelTiming3D: 		%d\n", pp_blk.p.u16PanelTiming3D);
		printf("u16PWM2DSync: 			%d\n", pp_blk.p.u16PWM2DSync);
		printf("u16PWM3DSync: 			%d\n", pp_blk.p.u16PWM3DSync);
		printf("u16PDIMFreq2D50Hz: 		%d\n", pp_blk.p.u16PDIMFreq2D50Hz);
		printf("u16PDIMFreq2D60Hz: 		%d\n", pp_blk.p.u16PDIMFreq2D60Hz);
		printf("u16PDIMFreq2DSport50Hz: 	%d\n", pp_blk.p.u16PDIMFreq2DSport50Hz);
		printf("u16PDIMFreq2DSport60Hz: 	%d\n", pp_blk.p.u16PDIMFreq2DSport60Hz);
		printf("u16PDIMFreq3D50Hz: 		%d\n", pp_blk.p.u16PDIMFreq3D50Hz);
		printf("u16PDIMFreq3D60Hz: 		%d\n", pp_blk.p.u16PDIMFreq3D60Hz);
		printf("u16PDIMDelay2D50Hz: 		%d\n", pp_blk.p.u16PDIMDelay2D50Hz);
		printf("u16PDIMDelay2D60Hz: 		%d\n", pp_blk.p.u16PDIMDelay2D60Hz);
		printf("u16PDIMDelay2DSport50Hz: 	%d\n", pp_blk.p.u16PDIMDelay2DSport50Hz);
		printf("u16PDIMDelay2DSport60Hz: 	%d\n", pp_blk.p.u16PDIMDelay2DSport60Hz);
		printf("u16PDIMDelay3D50Hz: 		%d\n", pp_blk.p.u16PDIMDelay3D50Hz);
		printf("u16PDIMDelay3D60Hz: 		%d\n", pp_blk.p.u16PDIMDelay3D60Hz);
		printf("u16ADIMFreq2D50Hz: 		%d\n", pp_blk.p.u16ADIMFreq2D50Hz);
		printf("u16ADIMFreq2D60Hz: 		%d\n", pp_blk.p.u16ADIMFreq2D60Hz);
		printf("u16ADIMFreq2DSport50Hz: 	%d\n", pp_blk.p.u16ADIMFreq2DSport50Hz);
		printf("u16ADIMFreq2DSport60Hz: 	%d\n", pp_blk.p.u16ADIMFreq2DSport60Hz);
		printf("u16ADIMFreq3D50Hz: 		%d\n", pp_blk.p.u16ADIMFreq3D50Hz);
		printf("u16ADIMFreq3D60Hz: 		%d\n", pp_blk.p.u16ADIMFreq3D60Hz);
		printf("b8LocalDIMControl: 		%d\n", pp_blk.p.b8LocalDIMControl);
		printf("u16LRDelay3D: 			%d\n", pp_blk.p.u16LRDelay3D);
		printf("u16BTDelay3D: 			%d\n", pp_blk.p.u16BTDelay3D);
		printf("b8PDIMConByPanel3D: 		%d\n", pp_blk.p.b8PDIMConByPanel3D);
		printf("u8Tcon: 			%d\n", pp_blk.p.u8Tcon);
		printf("u8TconType: 			%d\n", pp_blk.p.u8TconType);
		printf("u16VX1PreEmphasisG1: 		%d\n", pp_blk.p.u16VX1PreEmphasisG1);
		printf("u16VX1PreEmphasisG2: 		%d\n", pp_blk.p.u16VX1PreEmphasisG2);
		printf("u16VX1PreEmphasisG3: 		%d\n", pp_blk.p.u16VX1PreEmphasisG3);
		printf("u16VX1SwingG1: 			%d\n", pp_blk.p.u16VX1SwingG1);
		printf("u16VX1SwingG2: 			%d\n", pp_blk.p.u16VX1SwingG2);
		printf("u16VX1SwingG3: 			%d\n", pp_blk.p.u16VX1SwingG3);
		printf("b8PanelPowerInvert: 		%d\n", pp_blk.p.b8PanelPowerInvert);
		printf("b8BacklightPowerInver: 		%d\n", pp_blk.p.b8BacklightPowerInver);
		printf("u16BacklightFreq: 		%d\n", pp_blk.p.u16BacklightFreq);
		printf("================================\n\n");
#else//CONFIG_OLD_PANEL_PARAM
		printf("=====panel param index : %d=====\n", i);
		printf("version: 			%d\n", pp_blk.p.version);
		printf("u8PanelID: 			%d\n", pp_blk.p.u8PanelID);
		printf("b8Mode50Hz: 			%d\n", pp_blk.p.b8Mode50Hz);
		printf("u8Panel: 			%d\n", pp_blk.p.u8Panel);
		printf("au8Name: 			%s\n", pp_blk.p.au8Name);
		printf("b8DualPixelOutput: 		%d\n", pp_blk.p.b8DualPixelOutput);
		printf("b8ABSwap: 			%d\n", pp_blk.p.b8ABSwap);
		printf("b8ChannelSwap: 			%d\n", pp_blk.p.b8ChannelSwap);
		printf("b8FrameSyncEnable: 		%d\n", pp_blk.p.b8FrameSyncEnable);
		printf("u8FrameSyncMode: 		%d\n", pp_blk.p.u8FrameSyncMode);
		printf("u8MISC: 			%d\n", pp_blk.p.u8MISC);
		printf("enLvdsFormat: 			%d\n", pp_blk.p.enLvdsFormat);
		printf("b8RBSwap: 			%d\n", pp_blk.p.b8RBSwap);
		printf("u8DitheringBit: 		%d\n", pp_blk.p.u8DitheringBit);
		printf("u16LVDSPhase: 			%d\n", pp_blk.p.u16LVDSPhase);
		printf("u16LVDSPhase_PortA: 		%d\n", pp_blk.p.u16LVDSPhase_PortA);
		printf("u16LVDSPhase_PortB: 		%d\n", pp_blk.p.u16LVDSPhase_PortB);
		printf("u16VFreqDiff_Min: 		%d\n", pp_blk.p.u16VFreqDiff_Min);
		printf("u16VFreqDiff_Max: 		%d\n", pp_blk.p.u16VFreqDiff_Max);
		printf("u16VTotalDiff_Min: 		%d\n", pp_blk.p.u16VTotalDiff_Min);
		printf("u16VTotalDiff_Max: 		%d\n", pp_blk.p.u16VTotalDiff_Max);
		printf("stMode50Hz.u16Width: 		%d\n", pp_blk.p.stMode50Hz.u16Width);
		printf("stMode50Hz.u16Height: 		%d\n", pp_blk.p.stMode50Hz.u16Height);
		printf("stMode50Hz.u16TypHTotal: 	%d\n", pp_blk.p.stMode50Hz.u16TypHTotal);
		printf("stMode50Hz.u8HSyncWidth: 	%d\n", pp_blk.p.stMode50Hz.u8HSyncWidth);
		printf("stMode50Hz.u16HSyncBackPorch: 	%d\n", pp_blk.p.stMode50Hz.u16HSyncBackPorch);
		printf("stMode50Hz.u16HSyncStart: 	%d\n", pp_blk.p.stMode50Hz.u16HSyncStart);
		printf("stMode50Hz.u16TypVTotal: 	%d\n", pp_blk.p.stMode50Hz.u16TypVTotal);
		printf("stMode50Hz.u8VSyncWidth: 	%d\n", pp_blk.p.stMode50Hz.u8VSyncWidth);
		printf("stMode50Hz.u16VSyncBackPorch: 	%d\n", pp_blk.p.stMode50Hz.u16VSyncBackPorch);
		printf("stMode50Hz.u16VSyncStart: 	%d\n", pp_blk.p.stMode50Hz.u16VSyncStart);
		printf("stMode50Hz.u16PLL: 		%d\n", pp_blk.p.stMode50Hz.u16PLL);
		printf("stMode60Hz.u16Width: 		%d\n", pp_blk.p.stMode60Hz.u16Width);
		printf("stMode60Hz.u16Height: 		%d\n", pp_blk.p.stMode60Hz.u16Height);
		printf("stMode60Hz.u16TypHTotal: 	%d\n", pp_blk.p.stMode60Hz.u16TypHTotal);
		printf("stMode60Hz.u8HSyncWidth: 	%d\n", pp_blk.p.stMode60Hz.u8HSyncWidth);
		printf("stMode60Hz.u16HSyncBackPorch: 	%d\n", pp_blk.p.stMode60Hz.u16HSyncBackPorch);
		printf("stMode60Hz.u16HSyncStart: 	%d\n", pp_blk.p.stMode60Hz.u16HSyncStart);
		printf("stMode60Hz.u16TypVTotal: 	%d\n", pp_blk.p.stMode60Hz.u16TypVTotal);
		printf("stMode60Hz.u8VSyncWidth: 	%d\n", pp_blk.p.stMode60Hz.u8VSyncWidth);
		printf("stMode60Hz.u16VSyncBackPorch: 	%d\n", pp_blk.p.stMode60Hz.u16VSyncBackPorch);
		printf("stMode60Hz.u16VSyncStart: 	%d\n", pp_blk.p.stMode60Hz.u16VSyncStart);
		printf("stMode60Hz.u16PLL: 		%d\n", pp_blk.p.stMode60Hz.u16PLL);
		printf("u16PanelPowerOnDelay1: 		%d\n", pp_blk.p.u16PanelPowerOnDelay1);
		printf("u16PanelPowerOnDelay2: 		%d\n", pp_blk.p.u16PanelPowerOnDelay2);
		printf("u16PanelPowerOnDelay3: 		%d\n", pp_blk.p.u16PanelPowerOnDelay3);
		printf("u16PanelPowerOnDelay4: 		%d\n", pp_blk.p.u16PanelPowerOnDelay4);
		printf("u16PanelPowerOFFDelay1: 	%d\n", pp_blk.p.u16PanelPowerOFFDelay1);
		printf("u16PanelPowerOFFDelay2: 	%d\n", pp_blk.p.u16PanelPowerOFFDelay2);
		printf("u16PanelPowerOFFDelay3: 	%d\n", pp_blk.p.u16PanelPowerOFFDelay3);
		printf("u16PanelPowerGPIO: 		%d\n", pp_blk.p.u16PanelPowerGPIO);
		printf("u16PanelBackLightGPIO: 		%d\n", pp_blk.p.u16PanelBackLightGPIO);
		printf("u16BackLightLevel: 		%d\n", pp_blk.p.u16BackLightLevel);
		printf("u16BacklightFreq: 		%d\n", pp_blk.p.u16BacklightFreq);
		printf("b8PWMDutyInv: 			%d\n", pp_blk.p.b8PWMDutyInv);
		printf("b8PanelPowerInvert: 		%d\n", pp_blk.p.b8PanelPowerInvert);
		printf("b8BacklightPowerInver: 		%d\n", pp_blk.p.b8BacklightPowerInver);
		printf("b8HFlip: 			%d\n", pp_blk.p.b8HFlip);
		printf("b8VFlip: 			%d\n", pp_blk.p.b8VFlip);
		printf("VX1: 				%d\n", pp_blk.p.VX1);
		printf("FRC: 				%d\n", pp_blk.p.FRC);
		printf("SEP: 				%d\n", pp_blk.p.SEP);
		printf("u8PanelType: 			%d\n", pp_blk.p.u8PanelType);
		printf("u8Tcon: 			%d\n", pp_blk.p.u8Tcon);
		printf("u8TconType: 			%d\n", pp_blk.p.u8TconType);
		printf("u8LocalDIMControl: 		%d\n", pp_blk.p.u8LocalDIMControl);
		printf("u8LocalDIM_Zone_X: 		%d\n", pp_blk.p.u8LocalDIM_Zone_X);
		printf("u8LocalDIM_Zone_Y: 		%d\n", pp_blk.p.u8LocalDIM_Zone_Y);
		printf("u16VX1PreEmphasisG1: 		%d\n", pp_blk.p.u16VX1PreEmphasisG1);
		printf("u16VX1PreEmphasisG2: 		%d\n", pp_blk.p.u16VX1PreEmphasisG2);
		printf("u16VX1PreEmphasisG3: 		%d\n", pp_blk.p.u16VX1PreEmphasisG3);
		printf("u16VX1SwingG1: 			%d\n", pp_blk.p.u16VX1SwingG1);
		printf("u16VX1SwingG2: 			%d\n", pp_blk.p.u16VX1SwingG2);
		printf("u16VX1SwingG3: 			%d\n", pp_blk.p.u16VX1SwingG3);
		printf("u8PanelIOMode: 			%d\n", pp_blk.p.u8PanelIOMode);
		printf("u8PanelPwmMaxDuty: 		%d\n", pp_blk.p.u8PanelPwmMaxDuty);
		printf("u8PanelPwmMinDuty: 		%d\n", pp_blk.p.u8PanelPwmMinDuty);
		printf("u8PanelCustomerID: 		%d\n", pp_blk.p.u8PanelCustomerID);
		printf("u8Reserved0: 			%d\n", pp_blk.p.u8Reserved0);
		printf("u8Reserved1: 			%d\n", pp_blk.p.u8Reserved1);
		printf("u8Reserved2: 			%d\n", pp_blk.p.u8Reserved2);
		printf("u8Reserved3: 			%d\n", pp_blk.p.u8Reserved3);
		printf("u8Reserved4: 			%d\n", pp_blk.p.u8Reserved4);
		printf("u8Reserved5: 			%d\n", pp_blk.p.u8Reserved5);
		printf("u8Reserved6: 			%d\n", pp_blk.p.u8Reserved6);
		printf("u8Reserved7: 			%d\n", pp_blk.p.u8Reserved7);
		printf("u8Reserved8: 			%d\n", pp_blk.p.u8Reserved8);
		printf("u8Reserved9: 			%d\n", pp_blk.p.u8Reserved9);
		printf("u8PanelI2cPortNum: 		%d\n", pp_blk.p.u8PanelI2cPortNum);
		printf("u8PanelI2cSlaveAddr: 		%d\n", pp_blk.p.u8PanelI2cSlaveAddr);
		printf("u8PanelI2cWorkLog0: 		%d\n", pp_blk.p.u8PanelI2cWorkLog0);
		printf("u8PanelI2cWorkLog1: 		%d\n", pp_blk.p.u8PanelI2cWorkLog1);
		printf("u8PanelI2cWorkLog2: 		%d\n", pp_blk.p.u8PanelI2cWorkLog2);
		printf("u8PanelI2cWorkLog3: 		%d\n", pp_blk.p.u8PanelI2cWorkLog3);
		printf("u8PanelI2cWorkLog4: 		%d\n", pp_blk.p.u8PanelI2cWorkLog4);
		printf("u8PanelI2cWorkLog5: 		%d\n", pp_blk.p.u8PanelI2cWorkLog5);
		printf("u8PanelI2cWorkLog6: 		%d\n", pp_blk.p.u8PanelI2cWorkLog6);
		printf("u8PanelI2cWorkLog7: 		%d\n", pp_blk.p.u8PanelI2cWorkLog7);
		printf("u8PanelI2cWorkLog8: 		%d\n", pp_blk.p.u8PanelI2cWorkLog8);
		printf("u8PanelI2cWorkLog9: 		%d\n", pp_blk.p.u8PanelI2cWorkLog9);
		printf("u8PanelI2cWorkLog10: 		%d\n", pp_blk.p.u8PanelI2cWorkLog10);
		printf("u8PanelI2cWorkLog11: 		%d\n", pp_blk.p.u8PanelI2cWorkLog11);
		printf("u8PanelI2cWorkLog12: 		%d\n", pp_blk.p.u8PanelI2cWorkLog12);
		printf("u8PanelI2cWorkLog13: 		%d\n", pp_blk.p.u8PanelI2cWorkLog13);
		printf("u8PanelI2cWorkLog14: 		%d\n", pp_blk.p.u8PanelI2cWorkLog14);
		printf("u8PanelI2cWorkLog15: 		%d\n", pp_blk.p.u8PanelI2cWorkLog15);
		printf("u8PanelI2cWorkLog16: 		%d\n", pp_blk.p.u8PanelI2cWorkLog16);
		printf("u8PanelI2cWorkLog17: 		%d\n", pp_blk.p.u8PanelI2cWorkLog17);
		printf("u8PanelI2cWorkLog18: 		%d\n", pp_blk.p.u8PanelI2cWorkLog18);
		printf("u8PanelI2cWorkLog19: 		%d\n", pp_blk.p.u8PanelI2cWorkLog19);
		printf("u8PanelI2cWorkLog20: 		%d\n", pp_blk.p.u8PanelI2cWorkLog20);
		printf("u8PanelI2cWorkLog21: 		%d\n", pp_blk.p.u8PanelI2cWorkLog21);
		printf("u8PanelI2cWorkLog22: 		%d\n", pp_blk.p.u8PanelI2cWorkLog22);
		printf("u8PanelI2cWorkLog23: 		%d\n", pp_blk.p.u8PanelI2cWorkLog23);
		printf("u8PanelI2cWorkLog24: 		%d\n", pp_blk.p.u8PanelI2cWorkLog24);
		printf("u8PanelI2cWorkLog25: 		%d\n", pp_blk.p.u8PanelI2cWorkLog25);
		printf("u8PanelI2cWorkLog26: 		%d\n", pp_blk.p.u8PanelI2cWorkLog26);
		printf("u8PanelI2cWorkLog27: 		%d\n", pp_blk.p.u8PanelI2cWorkLog27);
		printf("u8PanelI2cWorkLog28: 		%d\n", pp_blk.p.u8PanelI2cWorkLog28);
		printf("u8PanelI2cWorkLog29: 		%d\n", pp_blk.p.u8PanelI2cWorkLog29);
		printf("u8PanelI2cWorkLog30: 		%d\n", pp_blk.p.u8PanelI2cWorkLog30);
		printf("u8PanelI2cWorkLog31: 		%d\n", pp_blk.p.u8PanelI2cWorkLog31);
		printf("u8PanelI2cWorkLog32: 		%d\n", pp_blk.p.u8PanelI2cWorkLog32);
		printf("u8PanelI2cWorkLog33: 		%d\n", pp_blk.p.u8PanelI2cWorkLog33);
		printf("u8PanelI2cWorkLog34: 		%d\n", pp_blk.p.u8PanelI2cWorkLog34);
		printf("u8PanelI2cWorkLog35: 		%d\n", pp_blk.p.u8PanelI2cWorkLog35);
		printf("u8PanelI2cWorkLog36: 		%d\n", pp_blk.p.u8PanelI2cWorkLog36);
		printf("u8PanelI2cWorkLog37: 		%d\n", pp_blk.p.u8PanelI2cWorkLog37);
		printf("u8PanelI2cWorkLog38: 		%d\n", pp_blk.p.u8PanelI2cWorkLog38);
		printf("u8PanelI2cWorkLog39: 		%d\n", pp_blk.p.u8PanelI2cWorkLog39);
		printf("u8PanelI2cWorkLog40: 		%d\n", pp_blk.p.u8PanelI2cWorkLog40);
		printf("u8PanelI2cWorkLog41: 		%d\n", pp_blk.p.u8PanelI2cWorkLog41);
		printf("u8PanelI2cWorkLog42: 		%d\n", pp_blk.p.u8PanelI2cWorkLog42);
		printf("u8PanelI2cWorkLog43: 		%d\n", pp_blk.p.u8PanelI2cWorkLog43);
		printf("u8PanelI2cWorkLog44: 		%d\n", pp_blk.p.u8PanelI2cWorkLog44);
		printf("u8PanelI2cWorkLog45: 		%d\n", pp_blk.p.u8PanelI2cWorkLog45);
		printf("u8PanelI2cWorkLog46: 		%d\n", pp_blk.p.u8PanelI2cWorkLog46);
		printf("u8PanelI2cWorkLog47: 		%d\n", pp_blk.p.u8PanelI2cWorkLog47);
		printf("u8PanelI2cWorkLog48: 		%d\n", pp_blk.p.u8PanelI2cWorkLog48);
		printf("u8PanelI2cWorkLog49: 		%d\n", pp_blk.p.u8PanelI2cWorkLog49);
		printf("u8PanelI2cWorkLog50: 		%d\n", pp_blk.p.u8PanelI2cWorkLog50);
		printf("u8PanelI2cWorkLog51: 		%d\n", pp_blk.p.u8PanelI2cWorkLog51);
		printf("u8PanelI2cWorkLog52: 		%d\n", pp_blk.p.u8PanelI2cWorkLog52);
		printf("u8PanelI2cWorkLog53: 		%d\n", pp_blk.p.u8PanelI2cWorkLog53);
		printf("u8PanelI2cWorkLog54: 		%d\n", pp_blk.p.u8PanelI2cWorkLog54);
		printf("u8PanelI2cWorkLog55: 		%d\n", pp_blk.p.u8PanelI2cWorkLog55);
		printf("u8PanelI2cWorkLog56: 		%d\n", pp_blk.p.u8PanelI2cWorkLog56);
		printf("u8PanelI2cWorkLog57: 		%d\n", pp_blk.p.u8PanelI2cWorkLog57);
		printf("u8PanelI2cWorkLog58: 		%d\n", pp_blk.p.u8PanelI2cWorkLog58);
		printf("u8PanelI2cWorkLog59: 		%d\n", pp_blk.p.u8PanelI2cWorkLog59);
		printf("u8PanelI2cWorkLog60: 		%d\n", pp_blk.p.u8PanelI2cWorkLog60);
		printf("u8PanelI2cWorkLog61: 		%d\n", pp_blk.p.u8PanelI2cWorkLog61);
		printf("u8PanelI2cWorkLog62: 		%d\n", pp_blk.p.u8PanelI2cWorkLog62);
		printf("u8PanelI2cWorkLog63: 		%d\n", pp_blk.p.u8PanelI2cWorkLog63);
		printf("u8PanelI2cWorkLog64: 		%d\n", pp_blk.p.u8PanelI2cWorkLog64);
		printf("u8PanelI2cWorkLog65: 		%d\n", pp_blk.p.u8PanelI2cWorkLog65);
		printf("u8PanelI2cWorkLog66: 		%d\n", pp_blk.p.u8PanelI2cWorkLog66);
		printf("u8PanelI2cWorkLog67: 		%d\n", pp_blk.p.u8PanelI2cWorkLog67);
		printf("u8PanelI2cWorkLog68: 		%d\n", pp_blk.p.u8PanelI2cWorkLog68);
		printf("u8PanelI2cWorkLog69: 		%d\n", pp_blk.p.u8PanelI2cWorkLog69);
		printf("u8PanelI2cWorkLog70: 		%d\n", pp_blk.p.u8PanelI2cWorkLog70);
		printf("u8PanelI2cWorkLog71: 		%d\n", pp_blk.p.u8PanelI2cWorkLog71);
		printf("u8PanelI2cWorkLog72: 		%d\n", pp_blk.p.u8PanelI2cWorkLog72);
		printf("u8PanelI2cWorkLog73: 		%d\n", pp_blk.p.u8PanelI2cWorkLog73);
		printf("u8PanelI2cWorkLog74: 		%d\n", pp_blk.p.u8PanelI2cWorkLog74);
		printf("u8PanelI2cWorkLog75: 		%d\n", pp_blk.p.u8PanelI2cWorkLog75);
		printf("u8PanelI2cWorkLog76: 		%d\n", pp_blk.p.u8PanelI2cWorkLog76);
		printf("u8PanelI2cWorkLog77: 		%d\n", pp_blk.p.u8PanelI2cWorkLog77);
		printf("u8PanelI2cWorkLog78: 		%d\n", pp_blk.p.u8PanelI2cWorkLog78);
		printf("u8PanelI2cWorkLog79: 		%d\n", pp_blk.p.u8PanelI2cWorkLog79);
		printf("u8PanelI2cWorkLog80: 		%d\n", pp_blk.p.u8PanelI2cWorkLog80);
		printf("u8PanelI2cWorkLog81: 		%d\n", pp_blk.p.u8PanelI2cWorkLog81);
		printf("u8PanelI2cWorkLog82: 		%d\n", pp_blk.p.u8PanelI2cWorkLog82);
		printf("u8PanelI2cWorkLog83: 		%d\n", pp_blk.p.u8PanelI2cWorkLog83);
		printf("u8PanelI2cWorkLog84: 		%d\n", pp_blk.p.u8PanelI2cWorkLog84);
		printf("u8PanelI2cWorkLog85: 		%d\n", pp_blk.p.u8PanelI2cWorkLog85);
		printf("u8PanelI2cWorkLog86: 		%d\n", pp_blk.p.u8PanelI2cWorkLog86);
		printf("u8PanelI2cWorkLog87: 		%d\n", pp_blk.p.u8PanelI2cWorkLog87);
		printf("u8PanelI2cWorkLog88: 		%d\n", pp_blk.p.u8PanelI2cWorkLog88);
		printf("u8PanelI2cWorkLog89: 		%d\n", pp_blk.p.u8PanelI2cWorkLog89);
		printf("u8PanelI2cWorkLog90: 		%d\n", pp_blk.p.u8PanelI2cWorkLog90);
		printf("u8PanelI2cWorkLog91: 		%d\n", pp_blk.p.u8PanelI2cWorkLog91);
		printf("u8PanelI2cWorkLog92: 		%d\n", pp_blk.p.u8PanelI2cWorkLog92);
		printf("u8PanelI2cWorkLog93: 		%d\n", pp_blk.p.u8PanelI2cWorkLog93);
		printf("u8PanelI2cWorkLog94: 		%d\n", pp_blk.p.u8PanelI2cWorkLog94);
		printf("u8PanelI2cWorkLog95: 		%d\n", pp_blk.p.u8PanelI2cWorkLog95);
		printf("u8PanelI2cWorkLog96: 		%d\n", pp_blk.p.u8PanelI2cWorkLog96);
		printf("u8PanelI2cWorkLog97: 		%d\n", pp_blk.p.u8PanelI2cWorkLog97);
		printf("u8PanelI2cWorkLog98: 		%d\n", pp_blk.p.u8PanelI2cWorkLog98);
		printf("u8PanelI2cWorkLog99: 		%d\n", pp_blk.p.u8PanelI2cWorkLog99);
		printf("================================\n\n");
#endif//CONFIG_OLD_PANEL_PARAM
	}

out:
	return ret;
}

static int panel_xdat_save(struct _nvt_emmc_xbootdat_info *this, int argc, char* const argv[])
{
	int ret = 0;
	char cmd[128] = {0};
	panel_index_blk pi_blk;
	u32 idx = 0;

	if(argc != 4 || (strcmp(argv[2], "index") != 0 && strcmp(argv[2],"config") != 0)) {
		ret = CMD_RET_USAGE;
		goto out;
	}

	if (strcmp(argv[2], "index") == 0) {
		idx = simple_strtoul(argv[3], NULL, 10);
		if(idx >= PANEL_PARM_NUM) {
			printf("panel index must between 0~9\n");
			ret = -EINVAL;
			goto out;
		}

		memset(&pi_blk, 0, sizeof(pi_blk));

		memcpy(pi_blk.idx.sign, PANEL_INDEX_SIGN, strlen(PANEL_INDEX_SIGN));
		pi_blk.idx.version = PANEL_PARAM_VER;
		pi_blk.idx.index = idx;

		sprintf(cmd, "mmc write 0x%x 0x%x 0x%x", &pi_blk, this->start_sector, 1);
		ret = run_command(cmd, 0);

		if(ret < 0) {
			printf("save panel index fail !\n");
			goto out;
		}
	} else if(strcmp(argv[2], "config") == 0) {
		run_command("usb start", 0);
		sprintf(cmd, "fatload usb 0:auto 0x%x %s", CONFIG_SYS_FWUPDATE_BUF, argv[3]);
		ret = run_command(cmd, 0);

		if(ret != 0) {
			printf("load panel config %s fail !\n", argv[3]);
			goto out;
		}

		memset(cmd, 0, sizeof(cmd));

		sprintf(cmd, "mmc write 0x%x 0x%x 0x%x", CONFIG_SYS_FWUPDATE_BUF, this->start_sector, (PANEL_PARM_NUM + 1));
		ret = run_command(cmd, 0);

		if(ret < 0) {
			printf("write panel config to xbootdat fail !\n");
			goto out;
		}
	}

out:
	return ret;
}

static int panel_xdat_erase(struct _nvt_emmc_xbootdat_info *this, int argc, char* const argv[])
{
	char cmd[128] = {0};
	int ret = 0;

	memset((void*)CONFIG_SYS_FWUPDATE_BUF, 0 ,(PANEL_PARM_NUM + 1) * PANEL_PARAM_SZ);
	sprintf(cmd, "mmc write 0x%x 0x%x 0x%x", CONFIG_SYS_FWUPDATE_BUF, this->start_sector, (PANEL_PARM_NUM));
	ret = run_command(cmd, 0);

	if(ret < 0) 
		printf("erase panel config fail !\n");

	return ret;
}


static int kercmd_xdat_help(struct _nvt_emmc_xbootdat_info *this, NVT_EMMC_XBOOTDAT_HELP_TYPE help_type)
{
	int ret = 0;

	switch(help_type) {
		case XDAT_HELP_DUMP:
			printf("xdump kercmd - dump current kernel command line\n");
			break;

		case XDAT_HELP_SAVE:
			printf("xsave kercmd - generate kernel command line by uboot variables and save to xbootdat\n");
			break;

		case XDAT_HELP_ERASE:
			printf("xerase kercmd - erase kernel command line save in xbootdat, don't do this unless you know what you are doing\n");
			break;

		default:
			printf("unknown xbootdat kercmd help ?\n");
	}

	return ret;

}


static int kercmd_xdat_dump(struct _nvt_emmc_xbootdat_info *this, int argc, char* const argv[])
{
	int ret = 0, len;
	char cmd[512] = {0};
	nvt_emmc_image_info *pimg;
	unsigned char *fdt_buf = (unsigned char*)CONFIG_SYS_FDT_BUF;
	int with_common_hdr = 0;

	if(argc != 2) {
		printf("Usage: %s kercmd \n",argv[0]);
		ret = CMD_RET_USAGE;
		goto out;
	}

	pimg = nvt_emmc_get_img_by_name("fdt0");
	if(pimg == NULL) {
		printf("%s get kernel fdt image fail !\n",__func__);
		ret = -1;
		goto out;
	}

	if(nvt_emmc_check_common_header_exist(pimg) == 1) 
		with_common_hdr = 1;

	if(with_common_hdr)
		ret = pimg->read_img(pimg, fdt_buf - sizeof(image_header_t), &len, NULL);
	else
		ret = pimg->read_img(pimg, fdt_buf, &len, NULL);
	if(ret < 0) {
		printf("%s read kernel fdt fail !\n",__func__);
		ret = -1;
		goto out;
	}

	if(fdt_check_header(fdt_buf) < 0) {
		printf("%s: Invaild fdt image, run ewrite fdt0 first !\n", __func__);
		goto out;
	}


	sprintf(cmd, "fdt addr 0x%x", fdt_buf);
	ret = run_command(cmd, 0);

	if(ret < 0) {
		printf("read kernel fdt line fail !\n");
		goto out;
	}

	ret = run_command("fdt get value barg /chosen bootargs", 0);
	if(ret < 0) {
		printf("read kenel command line fail !\n");
		goto out;
	}

	printf("kercmd : %s\n", getenv("barg"));
	setenv("barg",NULL);
out:
	return ret;
}

int nvt_prepare_kercmd(void)
{
	char cmd[512] = {0};
	char buf[255] = {0};
	int ret = 0,len;
	unsigned char *ker_load_addr = 0;
	unsigned char *ramdisk_load_addr = 0;
	nvt_emmc_image_info *pimg;
	int block_cnt = 0;
	int cpus;
	panel_index_blk __attribute__((__aligned__(64))) pi_blk;
	panel_param_blk __attribute__((__aligned__(64))) pp_blk;
	nvt_emmc_xbootdat_info *pxdat_panel = nvt_emmc_get_xdat_by_name("panel");
	struct boot_img_hdr *phdr;
	image_header_t *pimg_hdr;
	unsigned long ker_hdr_offset;

	if(pxdat_panel == NULL) {
		printf("%s get xbootdat panel config fail !\n",__func__);
		goto out;
	}
	//read index
	sprintf(buf, "mmc read 0x%x 0x%x 0x%x", &pi_blk, pxdat_panel->start_sector, 1);
	ret = run_command(buf, 0);
	if(ret < 0) {
		printf("%s read panel index fail !\n", __func__);
		goto out;
	}

	if(memcmp(pi_blk.idx.sign,PANEL_INDEX_SIGN, strlen(PANEL_INDEX_SIGN)) != 0) {
		printf("panel index not valid !\n");
		goto out;
	}

	sprintf(buf, "mmc read 0x%x 0x%x 0x%x", &pp_blk, pxdat_panel->start_sector + 1 + pi_blk.idx.index, 1);
	ret = run_command(buf, 0);
	if(ret < 0) {
		printf("%s read panel config %d fail !\n", __func__, pi_blk.idx.index);
		goto out;
	}

	memset(buf, 0, sizeof(buf));

	if(NULL != getenv("console"))
		sprintf(cmd, "console=%s,115200 ", getenv("console"));
	else
		sprintf(cmd, "%s ", "console=ttyS0,115200");

	if (getenv("memstr") != NULL)
		sprintf(buf,"%s ",getenv("memstr"));
	else
		sprintf(buf,"%s ","mem=512M");

	strcat(cmd,buf);

	if(getenv("lpj") != NULL) {
		sprintf(buf, "lpj=%s ",getenv("lpj"));
		strcat(cmd, buf);
	}

	if(getenv("dtv_log_addr") != NULL) {
		sprintf(buf, "dtv_log_addr=%s ",getenv("dtv_log_addr"));
		strcat(cmd,buf);
	}


	if (getenv("ethaddr") != NULL ) {
		sprintf(buf,"ethaddr=%s ",getenv("ethaddr"));
		strcat(cmd,buf);
	}

	if(getenv("usb") != NULL){
		sprintf(buf,"root=%s ",getenv("usb"));
		setenv("ramdisk_loadaddr", NULL);
	}else {
		sprintf(buf,"%s ","root=/dev/ram0 rw ");
	}

	if(getenv("fstype") != NULL) {
		strcat(cmd, buf);
		sprintf(buf, "rootfstype=%s ", getenv("fstype"));
		strcat(cmd, buf);
	} else {
		strcat(buf, "rootfstype=ext4 ");
		strcat(cmd,buf);
	}

	if(getenv_yesno("quiet"))
		strcat(cmd,"quiet ");


	if(getenv("cpus") != NULL) {
		cpus = simple_strtoul(getenv("cpus"),NULL,10);
		if(cpus > 4 || cpus < 1)
			cpus = 4;
	} else {
		cpus = 4;
	}
#if defined(CONFIG_ANDROID_AB_UPDATE)
	sprintf(buf, "maxcpus=%d skip_initramfs init=/init rootwait ",cpus);
#else
	sprintf(buf, "maxcpus=%d rootwait ",cpus);
#endif
	strcat(cmd,buf);


	if(getenv("kparam_addon") != NULL) {
		sprintf(buf," %s ", getenv("kparam_addon"));
		strcat(cmd, buf);
	}


	ker_load_addr = (unsigned char*)simple_strtoul(getenv("kernel_loadaddr"),NULL,16);

	if(!getenv_yesno("selinux"))
		strcat(cmd," androidboot.selinux=permissive ");


	pimg = nvt_emmc_get_img_by_name("ker0");

	if(pimg == NULL) {
		printf("%s get normal kernel image fail !\n");
		goto out;
	}


	ret = pimg->read_img(pimg, ker_load_addr, &len, NULL);
	if(ret < 0) {
		printf("load kernel 0 from emmc fail !\n");
		goto out;
	}

	phdr = (struct boot_img_hdr*)ker_load_addr;

#if 0
	if(memcmp((void*)(phdr->magic), BOOT_MAGIC, BOOT_MAGIC_SIZE) == 0) {
		ker_hdr_offset = ((sizeof(struct boot_img_hdr) + phdr->page_size - 1) / phdr->page_size) * phdr->page_size;
		pimg_hdr = (image_header_t*)(ker_load_addr + ker_hdr_offset);
		sprintf(buf, " initrd=0x%x,0x%x ", ntohl(pimg_hdr->ih_load) + ANDROID_RAMDISK_OFFSET_SZ, phdr->ramdisk_size);
		strcat(cmd, buf);
	}
#endif

	setenv("bootargs",cmd);

out:
	return ret;
}

static int kercmd_xdat_save(struct _nvt_emmc_xbootdat_info *this, int argc, char* const argv[])
{
	nvt_emmc_image_info *pimg;
	int ret = 0,len;
	unsigned char *fdt_buf = (unsigned char*)CONFIG_SYS_FDT_BUF;
	int with_common_hdr = 0;

	pimg = nvt_emmc_get_img_by_name("fdt0");
	if(pimg == NULL) {
		printf("%s get kernel fdt image fail !\n",__func__);
		ret = -1;
		goto out;
	}

	if(nvt_emmc_check_common_header_exist(pimg) == 1) 
		with_common_hdr = 1;

	if(with_common_hdr == 1)
		ret = pimg->read_img(pimg, fdt_buf - sizeof(image_header_t), &len, NULL);
	else
		ret = pimg->read_img(pimg, fdt_buf, &len, NULL);
	if(ret < 0) {
		printf("%s read kernel fdt fail !\n",__func__);
		ret = -1;
		goto out;
	}

	if(with_common_hdr == 1)
		ret = pimg->write_img(pimg, fdt_buf - sizeof(image_header_t), (pimg->size) << BYTE_TO_SECTOR_SFT, NULL);
	else
		ret = pimg->write_img(pimg, fdt_buf, (pimg->size) << BYTE_TO_SECTOR_SFT, NULL);

	if(ret < 0) {
		printf("save kernel command to xbootdat fail !\n");
		goto out;
	}


out:

	return ret;

}


static int kercmd_xdat_erase(struct _nvt_emmc_xbootdat_info *this, int argc, char* const argv[])
{
	int ret = 0,len;
	int i;
	nvt_emmc_image_info *pimg;
	unsigned char *fdt_buf = (unsigned char*)CONFIG_SYS_FDT_BUF;

	if(argc != 2) {
		printf("Usage: %s kercmd \n",argv[0]);
		ret = CMD_RET_USAGE;
		goto out;
	}

	setenv("bootargs"," ");

	pimg = nvt_emmc_get_img_by_name("fdt0");
	if(pimg == NULL) {
		printf("%s get kernel fdt image fail !\n",__func__);
		ret = -1;
		goto out;
	}

	ret = pimg->read_img(pimg, fdt_buf, &len, NULL);
	if(ret < 0) {
		printf("%s read kernel fdt fail !\n",__func__);
		ret = -1;
		goto out;
	}

	fdt_chosen(fdt_buf);

	ret = pimg->write_img(pimg, fdt_buf, (pimg->size) << BYTE_TO_SECTOR_SFT, NULL);

	if(ret < 0) {
		printf("erase kernel command line fail !\n");
		goto out;
	}

out:
	return ret;

}

static int rcvcmd_xdat_help(struct _nvt_emmc_xbootdat_info *this, NVT_EMMC_XBOOTDAT_HELP_TYPE help_type)
{
	int ret = 0;

	switch(help_type) {
		case XDAT_HELP_DUMP:
			printf("xdump rcvcmd - dump current recovery command line\n");
			break;

		case XDAT_HELP_SAVE:
			printf("xsave rcvcmd - generate recovery command line by uboot variables and save to xbootdat\n");
			break;

		case XDAT_HELP_ERASE:
			printf("xerase rcvcmd - erase recovery command line save in xbootdat, don't do this unless you know what you are doing\n");
			break;

		default:
			printf("unknown xbootdat rcvcmd help ?\n");
	}

	return ret;

}


static int rcvcmd_xdat_dump(struct _nvt_emmc_xbootdat_info *this, int argc, char* const argv[])
{
	int ret = 0, len;
	char cmd[512] = {0};
	nvt_emmc_image_info *pimg;
	unsigned char *fdt_buf = (unsigned char*)CONFIG_SYS_FDT_BUF;
	int with_common_hdr = 0;

	if(argc != 2) {
		printf("Usage: %s rcvcmd \n",argv[0]);
		ret = CMD_RET_USAGE;
		goto out;
	}

	pimg = nvt_emmc_get_img_by_name("fdt1");
	if(pimg == NULL) {
		printf("%s get recovery fdt image fail !\n",__func__);
		ret = -1;
		goto out;
	}

	if(nvt_emmc_check_common_header_exist(pimg) == 1) 
		with_common_hdr = 1;

	if(with_common_hdr)
		ret = pimg->read_img(pimg, fdt_buf - sizeof(image_header_t), &len, NULL);
	else
		ret = pimg->read_img(pimg, fdt_buf, &len, NULL);

	if(ret < 0) {
		printf("%s read recovery fdt fail !\n",__func__);
		ret = -1;
		goto out;
	}

	if(fdt_check_header(fdt_buf) < 0) {
		printf("%s: Invaild fdt image, run ewrite fdt1 first !\n", __func__);
		goto out;
	}


	sprintf(cmd, "fdt addr 0x%x", fdt_buf);
	ret = run_command(cmd, 0);

	if(ret < 0) {
		printf("read recovery fdt line fail !\n");
		goto out;
	}

	ret = run_command("fdt get value barg /chosen bootargs", 0);
	if(ret < 0) {
		printf("read recovery command line fail !\n");
		goto out;
	}

	printf("rcvcmd : %s\n", getenv("barg"));
	setenv("barg", NULL);
out:
	return ret;
}

int nvt_prepare_rcvcmd(void)
{
	char cmd[512] = {0};
	char buf[255] = {0};
	int ret = 0,len;
	unsigned char *ker_load_addr = 0;
	unsigned char *ramdisk_load_addr = 0;
	nvt_emmc_image_info *pimg;
	int block_cnt = 0;
	int cpus;
	panel_index_blk __attribute__((__aligned__(64))) pi_blk;
	panel_param_blk __attribute__((__aligned__(64))) pp_blk;
	nvt_emmc_xbootdat_info *pxdat_panel = nvt_emmc_get_xdat_by_name("panel");
	struct boot_img_hdr *phdr;
	image_header_t *pimg_hdr;
	unsigned long ker_hdr_offset;

	if(pxdat_panel == NULL) {
		printf("%s get xbootdat panel config fail !\n",__func__);
		goto out;
	}
	//read index
	sprintf(buf, "mmc read 0x%x 0x%x 0x%x", &pi_blk, pxdat_panel->start_sector, 1);
	ret = run_command(buf, 0);
	if(ret < 0) {
		printf("%s read panel index fail !\n", __func__);
		goto out;
	}

	if(memcmp(pi_blk.idx.sign,PANEL_INDEX_SIGN, strlen(PANEL_INDEX_SIGN)) != 0) {
		printf("panel index not valid !\n");
		goto out;
	}

	sprintf(buf, "mmc read 0x%x 0x%x 0x%x", &pp_blk, pxdat_panel->start_sector + 1 + pi_blk.idx.index, 1);
	ret = run_command(buf, 0);
	if(ret < 0) {
		printf("%s read panel config %d fail !\n", __func__, pi_blk.idx.index);
		goto out;
	}

	memset(buf, 0, sizeof(buf));

	if(NULL != getenv("console"))
		sprintf(cmd, "console=%s,115200 ", getenv("console"));
	else
		sprintf(cmd, "%s ", "console=ttyS0,115200");

	if (getenv("memstr") != NULL)
		sprintf(buf,"%s ",getenv("memstr"));
	else
		sprintf(buf,"%s ","mem=512M");

	strcat(cmd,buf);

	if(getenv("lpj") != NULL) {
		sprintf(buf, "lpj=%s ",getenv("lpj"));
		strcat(cmd, buf);
	}

	if(getenv("dtv_log_addr") != NULL) {
		sprintf(buf, "dtv_log_addr=%s ",getenv("dtv_log_addr"));
		strcat(cmd,buf);
	}


	if (getenv("ethaddr") != NULL ) {
		sprintf(buf,"ethaddr=%s ",getenv("ethaddr"));
		strcat(cmd,buf);
	}

	if(getenv("usb") != NULL){
		sprintf(buf,"root=%s rw ",getenv("usb"));
		setenv("ramdisk_loadaddr", NULL);
	}else {
		sprintf(buf,"%s ","root=/dev/ram0 rw ");
	}

	if(getenv("fstype") != NULL) {
		strcat(cmd, buf);
		sprintf(buf, "rootfstype=%s ", getenv("fstype"));
		strcat(cmd, buf);
	} else {
		strcat(buf, "rootfstype=ext4 ");
		strcat(cmd,buf);
	}

	if(getenv_yesno("quiet"))
		strcat(cmd,"quiet ");


	if(getenv("cpus") != NULL) {
		cpus = simple_strtoul(getenv("cpus"),NULL,10);
		if(cpus > 4 || cpus < 1)
			cpus = 4;
	} else {
		cpus = 4;
	}
#if defined(CONFIG_ANDROID_AB_UPDATE)
	sprintf(buf, "maxcpus=%d skip_initramfs init=/init rootwait ",cpus);
#else
	sprintf(buf, "maxcpus=%d rootwait ",cpus);
#endif
	strcat(cmd,buf);


	if(getenv("kparam_addon") != NULL) {
		sprintf(buf," %s ", getenv("kparam_addon"));
		strcat(cmd, buf);
	}


	ker_load_addr = (unsigned char*)simple_strtoul(getenv("kernel_loadaddr"),NULL,16);

	if(getenv("ramdisk_loadaddr") != NULL)
		ramdisk_load_addr = (unsigned char*)simple_strtoul(getenv("ramdisk_loadaddr"),NULL,16);
	else
		ramdisk_load_addr = 0;


	if(!getenv_yesno("selinux"))
		strcat(cmd," androidboot.selinux=permissive ");

	pimg = nvt_emmc_get_img_by_name("ker1");

	if(pimg == NULL) {
		printf("%s get recovery kernel image fail !\n");
		goto out;
	}
	ret = pimg->read_img(pimg, ker_load_addr, &len, NULL);
	if(ret < 0) {
		printf("load kernel 1 from emmc fail !\n");
		goto out;
	}

	phdr = (struct boot_img_hdr*)ker_load_addr;

#if 0
	if(memcmp((void*)(phdr->magic), BOOT_MAGIC, BOOT_MAGIC_SIZE) == 0) {
		ker_hdr_offset = ((sizeof(struct boot_img_hdr) + phdr->page_size - 1) / phdr->page_size) * phdr->page_size;
		pimg_hdr = (image_header_t*)(ker_load_addr + ker_hdr_offset);
		sprintf(buf, " initrd=0x%x,0x%x ", ntohl(pimg_hdr->ih_load) + ANDROID_RAMDISK_OFFSET_SZ, phdr->ramdisk_size);
		strcat(cmd, buf);
	}
#endif



	setenv("bootargs",cmd);
out:
	return ret;

}

static int rcvcmd_xdat_save(struct _nvt_emmc_xbootdat_info *this, int argc, char* const argv[])
{
	unsigned char *fdt_buf = (unsigned char*)CONFIG_SYS_FDT_BUF;
	int ret = 0,len;
	nvt_emmc_image_info *pimg;
	int with_common_hdr = 0;

	pimg = nvt_emmc_get_img_by_name("fdt1");
	if(pimg == NULL) {
		printf("%s get recovery fdt image fail !\n",__func__);
		ret = -1;
		goto out;
	}

	if(nvt_emmc_check_common_header_exist(pimg) == 1) 
		with_common_hdr = 1;

	if(with_common_hdr == 1)
		ret = pimg->read_img(pimg, fdt_buf - sizeof(image_header_t), &len, NULL);
	else
		ret = pimg->read_img(pimg, fdt_buf, &len, NULL);
	if(ret < 0) {
		printf("%s read recovery fdt fail !\n",__func__);
		ret = -1;
		goto out;
	}

	if(with_common_hdr == 1)
		ret = pimg->write_img(pimg, fdt_buf - sizeof(image_header_t), (pimg->size) << BYTE_TO_SECTOR_SFT, NULL);
	else
		ret = pimg->write_img(pimg, fdt_buf, (pimg->size) << BYTE_TO_SECTOR_SFT, NULL);

	if(ret < 0) {
		printf("save recovery command to xbootdat fail !\n");
		goto out;
	}


out:

	return ret;


}


static int rcvcmd_xdat_erase(struct _nvt_emmc_xbootdat_info *this, int argc, char* const argv[])
{
	int ret = 0,len;
	int i;
	nvt_emmc_image_info *pimg;
	unsigned char *fdt_buf = (unsigned char*)CONFIG_SYS_FDT_BUF;

	if(argc != 2) {
		printf("Usage: %s rcvcmd \n",argv[0]);
		ret = CMD_RET_USAGE;
		goto out;
	}

	setenv("bootargs"," ");

	pimg = nvt_emmc_get_img_by_name("fdt1");
	if(pimg == NULL) {
		printf("%s get recovery fdt image fail !\n",__func__);
		ret = -1;
		goto out;
	}

	ret = pimg->read_img(pimg, fdt_buf, &len, NULL);
	if(ret < 0) {
		printf("%s read recovery fdt fail !\n",__func__);
		ret = -1;
		goto out;
	}

	fdt_chosen(fdt_buf);

	ret = pimg->write_img(pimg, fdt_buf, (pimg->size) << BYTE_TO_SECTOR_SFT, NULL);

	if(ret < 0) {
		printf("erase recovery command line fail !\n");
		goto out;
	}

out:
	return ret;


}
