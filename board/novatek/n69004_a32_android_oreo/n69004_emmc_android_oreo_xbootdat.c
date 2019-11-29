/*
 *  board/novatek/nt72670b/nt72670b_emmc_nvt_xbootdat.c
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
static int mac_xdat_help(struct _nvt_emmc_xbootdat_info *this, NVT_EMMC_XBOOTDAT_HELP_TYPE help_type);
static int mac_xdat_dump(struct _nvt_emmc_xbootdat_info *this, int argc, char* const argv[]);
static int mac_xdat_save(struct _nvt_emmc_xbootdat_info *this, int argc, char* const argv[]);
static int mac_xdat_erase(struct _nvt_emmc_xbootdat_info *this, int argc, char* const argv[]);
static int type_xdat_help(struct _nvt_emmc_xbootdat_info *this, NVT_EMMC_XBOOTDAT_HELP_TYPE help_type);
static int type_xdat_dump(struct _nvt_emmc_xbootdat_info *this, int argc, char* const argv[]);
static int type_xdat_save(struct _nvt_emmc_xbootdat_info *this, int argc, char* const argv[]);
static int type_xdat_erase(struct _nvt_emmc_xbootdat_info *this, int argc, char* const argv[]);
static int audeff_xdat_help(struct _nvt_emmc_xbootdat_info *this, NVT_EMMC_XBOOTDAT_HELP_TYPE help_type);
static int audeff_xdat_dump(struct _nvt_emmc_xbootdat_info *this, int argc, char* const argv[]);
static int audeff_xdat_save(struct _nvt_emmc_xbootdat_info *this, int argc, char* const argv[]);
static int audeff_xdat_erase(struct _nvt_emmc_xbootdat_info *this, int argc, char* const argv[]);

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
	{
		//mac addree
		.xdat_name = "mac",
		.xdathelp = mac_xdat_help,
		.xdatdump = mac_xdat_dump,
		.xdatsave = mac_xdat_save,
		.xdaterase = mac_xdat_erase,
		.start_sector = (PANEL_PARM_NUM + 3),
		.size = 1,
	},
	{
		//type name
		.xdat_name = "type",
		.xdathelp = type_xdat_help,
		.xdatdump = type_xdat_dump,
		.xdatsave = type_xdat_save,
		.xdaterase = type_xdat_erase,
		.start_sector = (PANEL_PARM_NUM + 4),
		.size = 1,
	},
	{
		//audio effect
		.xdat_name = "audeff",
		.xdathelp = audeff_xdat_help,
		.xdatdump = audeff_xdat_dump,
		.xdatsave = audeff_xdat_save,
		.xdaterase = audeff_xdat_erase,
		.start_sector = (PANEL_PARM_NUM + 5),
		.size = 1,
	},
};

static int panel_xdat_help(struct _nvt_emmc_xbootdat_info *this, NVT_EMMC_XBOOTDAT_HELP_TYPE help_type)
{
	int ret = 0;

	switch(help_type) {
		case XDAT_HELP_DUMP:
			printf("xdump panel [brief] - dump current panel select index and all panel settings\n");
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
	unsigned int index;
	unsigned int rdlen;
	unsigned int cnt;
	nvt_emmc_image_info *pimg;
	unsigned char blk_buffer[EMMC_SECTOR_SIZE];
	unsigned char *ptr;
	panel_param_blk __attribute__((__aligned__(64))) pp_blk;
	panel_index_blk *pi_blk_ptr;
	int i;
	int verbose_mode = 1;

	if(argc != 2 && argc != 3) {
		printf("Usage: %s panel [brief]\n",argv[0]);
		ret = CMD_RET_USAGE;
		goto out;
	}

	if(argc == 3 && strncmp(argv[2], "brief", strlen("brief")) == 0)
		verbose_mode = 0;

	//read index
	pimg = nvt_emmc_get_img_by_name("param");
	if(pimg == NULL) {
		printf("%s param partition not exist !\n");
		goto out;
	}

	ret = pimg->read_img(pimg, (unsigned char*)CONFIG_SYS_FWUPDATE_BUF, &rdlen, NULL);

	if(ret < 0) {
		printf("%s read panel index fail !\n", __func__);
		goto out;
	}

	ptr = (unsigned char*)CONFIG_SYS_FWUPDATE_BUF;
	ptr += EMMC_SECTOR_SIZE;
	ptr += sizeof(int);
	memcpy((void*)(&index), ptr, sizeof(int));
	index = ntohl(index);

	if(index > PANEL_PARM_NUM) {
		printf("panel param index out of range, reset default !\n");
		index = 0;
	}

	printf("panel param index : %d\n", index);

	//read cnt
	memset(cmd, 0, sizeof(cmd));
	sprintf(cmd, "mmc read 0x%x 0x%x 0x%x", &pp_blk, this->start_sector, 1);
	ret = run_command(cmd, 0);
	if(ret < 0) {
		printf("%s read panel count fail !\n", __func__);
		goto out;
	}

	pi_blk_ptr = (panel_index_blk*)(&pp_blk);
	cnt = pi_blk_ptr->idx.param_cnt;

	for(i=0; i < cnt ;i++) {
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
		if(verbose_mode) {
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
		} else 
			printf("au8Name: 			%s\n", pp_blk.p.au8Name);
		printf("================================\n\n");
#else//CONFIG_OLD_PANEL_PARAM
		printf("=====panel param index : %d=====\n", i);
		if (verbose_mode) {
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
			printf("b8BacklightPowerInver: 		%d\n", pp_blk.p.b8BacklightPowerInvert);
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
		} else 
			printf("au8Name: 			%s\n", pp_blk.p.au8Name);
		printf("================================\n\n");
#endif//CONFIG_OLD_PANEL_PARAM
	}

out:
	return ret;
}

#define N69004_PANEL_INDEX_MAX 	30
#define tole(x) 	(x)

const unsigned int crc_table[256] = {
tole(0x00000000L), tole(0x77073096L), tole(0xee0e612cL), tole(0x990951baL),
tole(0x076dc419L), tole(0x706af48fL), tole(0xe963a535L), tole(0x9e6495a3L),
tole(0x0edb8832L), tole(0x79dcb8a4L), tole(0xe0d5e91eL), tole(0x97d2d988L),
tole(0x09b64c2bL), tole(0x7eb17cbdL), tole(0xe7b82d07L), tole(0x90bf1d91L),
tole(0x1db71064L), tole(0x6ab020f2L), tole(0xf3b97148L), tole(0x84be41deL),
tole(0x1adad47dL), tole(0x6ddde4ebL), tole(0xf4d4b551L), tole(0x83d385c7L),
tole(0x136c9856L), tole(0x646ba8c0L), tole(0xfd62f97aL), tole(0x8a65c9ecL),
tole(0x14015c4fL), tole(0x63066cd9L), tole(0xfa0f3d63L), tole(0x8d080df5L),
tole(0x3b6e20c8L), tole(0x4c69105eL), tole(0xd56041e4L), tole(0xa2677172L),
tole(0x3c03e4d1L), tole(0x4b04d447L), tole(0xd20d85fdL), tole(0xa50ab56bL),
tole(0x35b5a8faL), tole(0x42b2986cL), tole(0xdbbbc9d6L), tole(0xacbcf940L),
tole(0x32d86ce3L), tole(0x45df5c75L), tole(0xdcd60dcfL), tole(0xabd13d59L),
tole(0x26d930acL), tole(0x51de003aL), tole(0xc8d75180L), tole(0xbfd06116L),
tole(0x21b4f4b5L), tole(0x56b3c423L), tole(0xcfba9599L), tole(0xb8bda50fL),
tole(0x2802b89eL), tole(0x5f058808L), tole(0xc60cd9b2L), tole(0xb10be924L),
tole(0x2f6f7c87L), tole(0x58684c11L), tole(0xc1611dabL), tole(0xb6662d3dL),
tole(0x76dc4190L), tole(0x01db7106L), tole(0x98d220bcL), tole(0xefd5102aL),
tole(0x71b18589L), tole(0x06b6b51fL), tole(0x9fbfe4a5L), tole(0xe8b8d433L),
tole(0x7807c9a2L), tole(0x0f00f934L), tole(0x9609a88eL), tole(0xe10e9818L),
tole(0x7f6a0dbbL), tole(0x086d3d2dL), tole(0x91646c97L), tole(0xe6635c01L),
tole(0x6b6b51f4L), tole(0x1c6c6162L), tole(0x856530d8L), tole(0xf262004eL),
tole(0x6c0695edL), tole(0x1b01a57bL), tole(0x8208f4c1L), tole(0xf50fc457L),
tole(0x65b0d9c6L), tole(0x12b7e950L), tole(0x8bbeb8eaL), tole(0xfcb9887cL),
tole(0x62dd1ddfL), tole(0x15da2d49L), tole(0x8cd37cf3L), tole(0xfbd44c65L),
tole(0x4db26158L), tole(0x3ab551ceL), tole(0xa3bc0074L), tole(0xd4bb30e2L),
tole(0x4adfa541L), tole(0x3dd895d7L), tole(0xa4d1c46dL), tole(0xd3d6f4fbL),
tole(0x4369e96aL), tole(0x346ed9fcL), tole(0xad678846L), tole(0xda60b8d0L),
tole(0x44042d73L), tole(0x33031de5L), tole(0xaa0a4c5fL), tole(0xdd0d7cc9L),
tole(0x5005713cL), tole(0x270241aaL), tole(0xbe0b1010L), tole(0xc90c2086L),
tole(0x5768b525L), tole(0x206f85b3L), tole(0xb966d409L), tole(0xce61e49fL),
tole(0x5edef90eL), tole(0x29d9c998L), tole(0xb0d09822L), tole(0xc7d7a8b4L),
tole(0x59b33d17L), tole(0x2eb40d81L), tole(0xb7bd5c3bL), tole(0xc0ba6cadL),
tole(0xedb88320L), tole(0x9abfb3b6L), tole(0x03b6e20cL), tole(0x74b1d29aL),
tole(0xead54739L), tole(0x9dd277afL), tole(0x04db2615L), tole(0x73dc1683L),
tole(0xe3630b12L), tole(0x94643b84L), tole(0x0d6d6a3eL), tole(0x7a6a5aa8L),
tole(0xe40ecf0bL), tole(0x9309ff9dL), tole(0x0a00ae27L), tole(0x7d079eb1L),
tole(0xf00f9344L), tole(0x8708a3d2L), tole(0x1e01f268L), tole(0x6906c2feL),
tole(0xf762575dL), tole(0x806567cbL), tole(0x196c3671L), tole(0x6e6b06e7L),
tole(0xfed41b76L), tole(0x89d32be0L), tole(0x10da7a5aL), tole(0x67dd4accL),
tole(0xf9b9df6fL), tole(0x8ebeeff9L), tole(0x17b7be43L), tole(0x60b08ed5L),
tole(0xd6d6a3e8L), tole(0xa1d1937eL), tole(0x38d8c2c4L), tole(0x4fdff252L),
tole(0xd1bb67f1L), tole(0xa6bc5767L), tole(0x3fb506ddL), tole(0x48b2364bL),
tole(0xd80d2bdaL), tole(0xaf0a1b4cL), tole(0x36034af6L), tole(0x41047a60L),
tole(0xdf60efc3L), tole(0xa867df55L), tole(0x316e8eefL), tole(0x4669be79L),
tole(0xcb61b38cL), tole(0xbc66831aL), tole(0x256fd2a0L), tole(0x5268e236L),
tole(0xcc0c7795L), tole(0xbb0b4703L), tole(0x220216b9L), tole(0x5505262fL),
tole(0xc5ba3bbeL), tole(0xb2bd0b28L), tole(0x2bb45a92L), tole(0x5cb36a04L),
tole(0xc2d7ffa7L), tole(0xb5d0cf31L), tole(0x2cd99e8bL), tole(0x5bdeae1dL),
tole(0x9b64c2b0L), tole(0xec63f226L), tole(0x756aa39cL), tole(0x026d930aL),
tole(0x9c0906a9L), tole(0xeb0e363fL), tole(0x72076785L), tole(0x05005713L),
tole(0x95bf4a82L), tole(0xe2b87a14L), tole(0x7bb12baeL), tole(0x0cb61b38L),
tole(0x92d28e9bL), tole(0xe5d5be0dL), tole(0x7cdcefb7L), tole(0x0bdbdf21L),
tole(0x86d3d2d4L), tole(0xf1d4e242L), tole(0x68ddb3f8L), tole(0x1fda836eL),
tole(0x81be16cdL), tole(0xf6b9265bL), tole(0x6fb077e1L), tole(0x18b74777L),
tole(0x88085ae6L), tole(0xff0f6a70L), tole(0x66063bcaL), tole(0x11010b5cL),
tole(0x8f659effL), tole(0xf862ae69L), tole(0x616bffd3L), tole(0x166ccf45L),
tole(0xa00ae278L), tole(0xd70dd2eeL), tole(0x4e048354L), tole(0x3903b3c2L),
tole(0xa7672661L), tole(0xd06016f7L), tole(0x4969474dL), tole(0x3e6e77dbL),
tole(0xaed16a4aL), tole(0xd9d65adcL), tole(0x40df0b66L), tole(0x37d83bf0L),
tole(0xa9bcae53L), tole(0xdebb9ec5L), tole(0x47b2cf7fL), tole(0x30b5ffe9L),
tole(0xbdbdf21cL), tole(0xcabac28aL), tole(0x53b39330L), tole(0x24b4a3a6L),
tole(0xbad03605L), tole(0xcdd70693L), tole(0x54de5729L), tole(0x23d967bfL),
tole(0xb3667a2eL), tole(0xc4614ab8L), tole(0x5d681b02L), tole(0x2a6f2b94L),
tole(0xb40bbe37L), tole(0xc30c8ea1L), tole(0x5a05df1bL), tole(0x2d02ef8dL)
};

#define DO_CRC(x) crc = tab[(crc ^ (x)) & 255] ^ (crc >> 8)

static unsigned int crc32_sky(const char *p, unsigned int len)
{
	unsigned int i, crc;
	const unsigned int *tab = crc_table;

	crc = 0;

	for(i=0; i < len; i++)
		DO_CRC(p[i]);

	return crc;
}
#undef DO_CRC


static int panel_xdat_save(struct _nvt_emmc_xbootdat_info *this, int argc, char* const argv[])
{
	int ret = 0;
	char cmd[128] = {0};
	panel_index_blk pi_blk;
	unsigned char *ptr;
	unsigned int rdlen;
	unsigned int flen;
	nvt_emmc_image_info *pimg;
	u32 idx = 0;
	unsigned int calc_crc;

	if(argc != 4 || (strcmp(argv[2], "index") != 0 && strcmp(argv[2],"config") != 0)) {
		ret = CMD_RET_USAGE;
		goto out;
	}

	if (strcmp(argv[2], "index") == 0) {
		idx = simple_strtoul(argv[3], NULL, 10);
		if(idx >= N69004_PANEL_INDEX_MAX) {
			printf("panel index must between 0~%d\n", (N69004_PANEL_INDEX_MAX - 1));
			ret = -EINVAL;
			goto out;
		}

		pimg = nvt_emmc_get_img_by_name("param");
		if(pimg == NULL) {
			printf("%s param partition not exist !\n", __func__);
			ret = -EINVAL;
			goto out;
		}

		ret = pimg->read_img(pimg, (unsigned char*)CONFIG_SYS_FWUPDATE_BUF, &rdlen, NULL);
		if(ret < 0) {
			printf("%s read panel index fail !\n", __func__);
			ret = -EINVAL;
			goto out;
		}

		ptr = (unsigned char*)CONFIG_SYS_FWUPDATE_BUF;
		ptr += EMMC_SECTOR_SIZE;
		ptr += sizeof(int);
		idx = htonl(idx);
		memcpy(ptr, &idx, sizeof(idx));

		//update crc
		ptr = (unsigned char*)CONFIG_SYS_FWUPDATE_BUF;
		ptr += EMMC_SECTOR_SIZE;
		calc_crc = crc32_sky(ptr, EMMC_SECTOR_SIZE - sizeof(int));
		calc_crc = htonl(calc_crc);
		ptr += (EMMC_SECTOR_SIZE-sizeof(int));
		memcpy(ptr, &calc_crc, sizeof(int));


		ret = pimg->write_img(pimg, (unsigned char*)CONFIG_SYS_FWUPDATE_BUF, rdlen, NULL);

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

		flen = simple_strtoul(getenv("filesize"), NULL, 16);

		pimg = nvt_emmc_get_img_by_name("xbootdat");
		if(pimg == NULL) {
			printf("%s : panelparam partition not exist !\n", __func__);
			ret = -EINVAL;
			goto out;
		}

		ret = pimg->write_img(pimg, (unsigned char*)CONFIG_SYS_FWUPDATE_BUF, flen, NULL);

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

#define N69004_3G_ADC_VAL 	0x72
#define N69004_3G_ADC_RANGE 	10

static int n69004_ddr_sz_is_3g(void)
{
	uint32_t reg_val;
	uint32_t val_min = N69004_3G_ADC_VAL - N69004_3G_ADC_RANGE;
	uint32_t val_max = N69004_3G_ADC_VAL + N69004_3G_ADC_RANGE;

	reg_val = __REG(0xFC040324);

	if(reg_val > val_min && reg_val < val_max)
		return 1;
	else
		return 0;

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
	struct boot_img_hdr *phdr;
	image_header_t *pimg_hdr;
	unsigned long ker_hdr_offset;

	memset(buf, 0, sizeof(buf));

	if(NULL != getenv("console"))
		sprintf(cmd, "console=%s,115200 ", getenv("console"));
	else
		sprintf(cmd, "%s ", "console=ttyS0,115200");

	if(n69004_ddr_sz_is_3g()) {
		if (getenv("memstr_3g") != NULL)
			sprintf(buf,"%s ",getenv("memstr_3g"));
		else
			sprintf(buf,"%s ","mem=512M");

	} else {
		if (getenv("memstr") != NULL)
			sprintf(buf,"%s ",getenv("memstr"));
		else
			sprintf(buf,"%s ","mem=512M");
	}

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
	struct boot_img_hdr *phdr;
	image_header_t *pimg_hdr;
	unsigned long ker_hdr_offset;

	memset(buf, 0, sizeof(buf));

	if(NULL != getenv("console"))
		sprintf(cmd, "console=%s,115200 ", getenv("console"));
	else
		sprintf(cmd, "%s ", "console=ttyS0,115200");

	if(n69004_ddr_sz_is_3g()) {
		if (getenv("memstr_3g") != NULL)
			sprintf(buf,"%s ",getenv("memstr_3g"));
		else
			sprintf(buf,"%s ","mem=512M");

	} else {
		if (getenv("memstr") != NULL)
			sprintf(buf,"%s ",getenv("memstr"));
		else
			sprintf(buf,"%s ","mem=512M");
	}

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

static int mac_xdat_help(struct _nvt_emmc_xbootdat_info *this, NVT_EMMC_XBOOTDAT_HELP_TYPE help_type)
{
	int ret = 0;

	switch(help_type) {
		case XDAT_HELP_DUMP:
			printf("xdump mac - dump mac address store in param partition\n");
			break;

		case XDAT_HELP_SAVE:
			printf("xsave mac - xsave mac address in param partition\n");
			break;

		case XDAT_HELP_ERASE:
			printf("xerase mac - erase mac address in param partition\n");
			break;

		default:
			printf("unknown xbootdat mac help ?\n");
	}

	return ret;

}

#define SKYWORTH_PARAM_MAC_OFS 	0x40

static int mac_xdat_dump(struct _nvt_emmc_xbootdat_info *this, int argc, char* const argv[])
{
	int ret = 0;
	nvt_emmc_image_info *pimg;
	unsigned int rdlen;
	unsigned char *ptr;

	if(argc != 2) {
		printf("Usage: %s mac \n",argv[0]);
		ret = CMD_RET_USAGE;
		goto out;
	}

	pimg = nvt_emmc_get_img_by_name("param");

	if(pimg == NULL) {
		printf("%s param partition not exist !\n", __func__);
		ret = -EINVAL;
		goto out;
	}

	ret = pimg->read_img(pimg, (unsigned char*)CONFIG_SYS_FWUPDATE_BUF, &rdlen, NULL);

	if(ret < 0) {
		printf("%s read param partition fail !\n", __func__);
		goto out;
	}

	ptr = (unsigned char*)CONFIG_SYS_FWUPDATE_BUF;
	ptr += EMMC_SECTOR_SIZE;
	ptr += SKYWORTH_PARAM_MAC_OFS;

	printf("mac addr : %02X:%02X:%02X:%02X:%02X:%02X\n",
			ptr[0], ptr[1], ptr[2],
			ptr[3], ptr[4], ptr[5]);
out:
	return ret;
}

static int mac_xdat_save(struct _nvt_emmc_xbootdat_info *this, int argc, char* const argv[])
{
	int ret = 0;
	nvt_emmc_image_info *pimg;
	unsigned int rdlen;
	unsigned int calc_crc;
	unsigned char *ptr;
	char *str1;
	char buff[24] = {};
	int i;
	char ch;

	if (argc != 3) {
		printf("Usage: %s mac xx:xx:xx:xx:xx:xx\n", argv[0]);
		ret = CMD_RET_USAGE;
		goto out;
	}

	pimg = nvt_emmc_get_img_by_name("param");

	if(pimg == NULL) {
		printf("%s param partition not exist !\n", __func__);
		ret = -EINVAL;
		goto out;
	}

	ret = pimg->read_img(pimg, (unsigned char*)CONFIG_SYS_FWUPDATE_BUF, &rdlen, NULL);

	if(ret < 0) {
		printf("%s read param partition fail !\n", __func__);
		goto out;
	}

	ptr = (unsigned char*)CONFIG_SYS_FWUPDATE_BUF;
	ptr += EMMC_SECTOR_SIZE;
	ptr += SKYWORTH_PARAM_MAC_OFS;

	memcpy(buff, argv[2], strlen(argv[2]));
	str1 = buff;
	for(i=0;i<sizeof(buff);i++)
		if(str1[i] == ':')
			str1[i] = '\0';

	str1 = buff;
	for(i=0;i<6;i++) {
		ch = (char)simple_strtoul(str1, NULL, 16);
		*ptr = ch;
		ptr++;
		str1 += 3;
	}

	//update crc
	ptr = (unsigned char*)CONFIG_SYS_FWUPDATE_BUF;
	ptr += EMMC_SECTOR_SIZE;
	calc_crc = crc32_sky(ptr, EMMC_SECTOR_SIZE - sizeof(int));
	calc_crc = htonl(calc_crc);
	ptr += (EMMC_SECTOR_SIZE-sizeof(int));
	memcpy(ptr, &calc_crc, sizeof(int));

	ret = pimg->write_img(pimg, (unsigned char*)CONFIG_SYS_FWUPDATE_BUF, rdlen, NULL);
	if(ret < 0) {
		printf("%s write mac to param partition fail !\n", __func__);
		ret = -EIO;
		goto out;
	}

out:
	return ret;
}


static int mac_xdat_erase(struct _nvt_emmc_xbootdat_info *this, int argc, char* const argv[])
{
	int ret = 0;

	if(argc != 2) {
		printf("Usage: %s mac\n", argv[0]);
		ret = CMD_RET_USAGE;
		goto out;
	}

	run_command("xsave mac 00:00:00:00:00:00", 0);

out:
	return ret;
}

#define SKYWORTH_PARAM_TYPE_OFS 	0x120
#define SKYWORTH_PARAM_TYPE_HEAD 	0xAA
#define SKYWORTH_PARAM_TYPE_LEN 	15

static int type_xdat_help(struct _nvt_emmc_xbootdat_info *this, NVT_EMMC_XBOOTDAT_HELP_TYPE help_type)
{
	int ret = 0;

	switch(help_type) {
		case XDAT_HELP_DUMP:
			printf("xdump type - dump type store in param partition\n");
			break;

		case XDAT_HELP_SAVE:
			printf("xsave type - xsave type in param partition\n");
			break;

		case XDAT_HELP_ERASE:
			printf("xerase type - erase type in param partition\n");
			break;

		default:
			printf("unknown xbootdat type help ?\n");
	}

	return ret;

}


static int type_xdat_dump(struct _nvt_emmc_xbootdat_info *this, int argc, char* const argv[])
{
	int ret = 0;
	nvt_emmc_image_info *pimg;
	unsigned int rdlen;
	unsigned char *ptr;
	unsigned char type_str[SKYWORTH_PARAM_TYPE_LEN] = {};
	int i;

	if(argc != 2) {
		printf("Usage: %s type_name \n",argv[0]);
		ret = CMD_RET_USAGE;
		goto out;
	}

	pimg = nvt_emmc_get_img_by_name("param");

	if(pimg == NULL) {
		printf("%s param partition not exist !\n", __func__);
		ret = -EINVAL;
		goto out;
	}

	ret = pimg->read_img(pimg, (unsigned char*)CONFIG_SYS_FWUPDATE_BUF, &rdlen, NULL);

	if(ret < 0) {
		printf("%s read param partition fail !\n", __func__);
		goto out;
	}

	ptr = (unsigned char*)CONFIG_SYS_FWUPDATE_BUF;
	ptr += EMMC_SECTOR_SIZE;
	ptr += SKYWORTH_PARAM_TYPE_OFS;

	for(i=0;i<SKYWORTH_PARAM_TYPE_LEN;i++) {
		if(ptr[i] == 0xFF)
			break;
		else
			type_str[i] = ptr[i];
	}

	if(type_str[0] == SKYWORTH_PARAM_TYPE_HEAD)
		printf("type name = %s\n", &(type_str[1]));
	else
		printf("no type name define !\n");

out:
	return ret;
}

static int type_xdat_save(struct _nvt_emmc_xbootdat_info *this, int argc, char* const argv[])
{
	int ret = 0;
	nvt_emmc_image_info *pimg;
	unsigned int rdlen;
	unsigned int calc_crc;
	unsigned char *ptr;

	if (argc != 3) {
		printf("Usage: %s type type_name\n", argv[0]);
		ret = CMD_RET_USAGE;
		goto out;
	}

	pimg = nvt_emmc_get_img_by_name("param");

	if(pimg == NULL) {
		printf("%s param partition not exist !\n", __func__);
		ret = -EINVAL;
		goto out;
	}

	ret = pimg->read_img(pimg, (unsigned char*)CONFIG_SYS_FWUPDATE_BUF, &rdlen, NULL);

	if(ret < 0) {
		printf("%s read param partition fail !\n", __func__);
		goto out;
	}

	ptr = (unsigned char*)CONFIG_SYS_FWUPDATE_BUF;
	ptr += EMMC_SECTOR_SIZE;
	ptr += SKYWORTH_PARAM_TYPE_OFS;

	memset(ptr, 0xFF, SKYWORTH_PARAM_TYPE_LEN);

	ptr[0] = SKYWORTH_PARAM_TYPE_HEAD;

	strncpy(&(ptr[1]), argv[2], strlen(argv[2]));

	//update crc
	ptr = (unsigned char*)CONFIG_SYS_FWUPDATE_BUF;
	ptr += EMMC_SECTOR_SIZE;
	calc_crc = crc32_sky(ptr, EMMC_SECTOR_SIZE - sizeof(int));
	calc_crc = htonl(calc_crc);
	ptr += (EMMC_SECTOR_SIZE-sizeof(int));
	memcpy(ptr, &calc_crc, sizeof(int));

	ret = pimg->write_img(pimg, (unsigned char*)CONFIG_SYS_FWUPDATE_BUF, rdlen, NULL);
	if(ret < 0) {
		printf("%s write type to param partition fail !\n", __func__);
		ret = -EIO;
		goto out;
	}

out:
	return ret;
}


static int type_xdat_erase(struct _nvt_emmc_xbootdat_info *this, int argc, char* const argv[])
{
	int ret = 0;
	nvt_emmc_image_info *pimg;
	unsigned int rdlen;
	unsigned int calc_crc;
	unsigned char *ptr;

	if(argc != 2) {
		printf("Usage: %s type\n", argv[0]);
		ret = CMD_RET_USAGE;
		goto out;
	}

	pimg = nvt_emmc_get_img_by_name("param");

	if(pimg == NULL) {
		printf("%s param partition not exist !\n", __func__);
		ret = -EINVAL;
		goto out;
	}

	ret = pimg->read_img(pimg, (unsigned char*)CONFIG_SYS_FWUPDATE_BUF, &rdlen, NULL);

	if(ret < 0) {
		printf("%s read param partition fail !\n", __func__);
		goto out;
	}

	ptr = (unsigned char*)CONFIG_SYS_FWUPDATE_BUF;
	ptr += EMMC_SECTOR_SIZE;
	ptr += SKYWORTH_PARAM_TYPE_OFS;

	memset(ptr, 0xFF, SKYWORTH_PARAM_TYPE_LEN);


	//update crc
	ptr = (unsigned char*)CONFIG_SYS_FWUPDATE_BUF;
	ptr += EMMC_SECTOR_SIZE;
	calc_crc = crc32_sky(ptr, EMMC_SECTOR_SIZE - sizeof(int));
	calc_crc = htonl(calc_crc);
	ptr += (EMMC_SECTOR_SIZE-sizeof(int));
	memcpy(ptr, &calc_crc, sizeof(int));

	ret = pimg->write_img(pimg, (unsigned char*)CONFIG_SYS_FWUPDATE_BUF, rdlen, NULL);
	if(ret < 0) {
		printf("%s write type to param partition fail !\n", __func__);
		ret = -EIO;
		goto out;
	}

out:
	return ret;

}

static int audeff_xdat_help(struct _nvt_emmc_xbootdat_info *this, NVT_EMMC_XBOOTDAT_HELP_TYPE help_type)
{
	int ret = 0;

	switch(help_type) {
		case XDAT_HELP_DUMP:
			printf("xdump audeff - dump audio effect store in param partition\n");
			break;

		case XDAT_HELP_SAVE:
			printf("xsave audeff - xsave audio effect in param partition\n");
			break;

		case XDAT_HELP_ERASE:
			printf("xerase audeff - erase audio effect in param partition\n");
			break;

		default:
			printf("unknown xbootdat type help ?\n");
	}

	return ret;

}

#define SKYWORTH_PARAM_AUDEFF_OFS 	0x9E

static int audeff_xdat_dump(struct _nvt_emmc_xbootdat_info *this, int argc, char* const argv[])
{
	int ret = 0;
	nvt_emmc_image_info *pimg;
	unsigned int rdlen;
	unsigned char *ptr;

	if(argc != 2) {
		printf("Usage: %s audeff \n",argv[0]);
		ret = CMD_RET_USAGE;
		goto out;
	}

	pimg = nvt_emmc_get_img_by_name("param");

	if(pimg == NULL) {
		printf("%s param partition not exist !\n", __func__);
		ret = -EINVAL;
		goto out;
	}

	ret = pimg->read_img(pimg, (unsigned char*)CONFIG_SYS_FWUPDATE_BUF, &rdlen, NULL);

	if(ret < 0) {
		printf("%s read param partition fail !\n", __func__);
		goto out;
	}

	ptr = (unsigned char*)CONFIG_SYS_FWUPDATE_BUF;
	ptr += EMMC_SECTOR_SIZE;
	ptr += SKYWORTH_PARAM_AUDEFF_OFS;

	switch(*ptr) {
		case 0x2:
			printf("value = 2, DOLBY_DAP\n");
			break;
		case 0x3:
			printf("value = 3, DOLBY_ATMOS\n");
			break;
		case 0x4:
			printf("value = 4, DTS\n");
			break;
		default:
			printf("invalid audio effect value !\n");
			break;
	}

out:
	return ret;
}

static int audeff_xdat_save(struct _nvt_emmc_xbootdat_info *this, int argc, char* const argv[])
{
	int ret = 0;
	nvt_emmc_image_info *pimg;
	unsigned int rdlen;
	unsigned int calc_crc;
	unsigned char *ptr;
	unsigned int val;

	if (argc != 3) {
		printf("Usage: %s audeff eff_val\n", argv[0]);
		ret = CMD_RET_USAGE;
		goto out;
	}

	pimg = nvt_emmc_get_img_by_name("param");

	if(pimg == NULL) {
		printf("%s param partition not exist !\n", __func__);
		ret = -EINVAL;
		goto out;
	}

	ret = pimg->read_img(pimg, (unsigned char*)CONFIG_SYS_FWUPDATE_BUF, &rdlen, NULL);

	if(ret < 0) {
		printf("%s read param partition fail !\n", __func__);
		goto out;
	}

	ptr = (unsigned char*)CONFIG_SYS_FWUPDATE_BUF;
	ptr += EMMC_SECTOR_SIZE;
	ptr += SKYWORTH_PARAM_AUDEFF_OFS;

	val = simple_strtoul(argv[2], NULL, 10);

	*ptr = (unsigned char)val;

	//update crc
	ptr = (unsigned char*)CONFIG_SYS_FWUPDATE_BUF;
	ptr += EMMC_SECTOR_SIZE;
	calc_crc = crc32_sky(ptr, EMMC_SECTOR_SIZE - sizeof(int));
	calc_crc = htonl(calc_crc);
	ptr += (EMMC_SECTOR_SIZE-sizeof(int));
	memcpy(ptr, &calc_crc, sizeof(int));

	ret = pimg->write_img(pimg, (unsigned char*)CONFIG_SYS_FWUPDATE_BUF, rdlen, NULL);
	if(ret < 0) {
		printf("%s write audio effect type to param partition fail !\n", __func__);
		ret = -EIO;
		goto out;
	}

out:
	return ret;
}


static int audeff_xdat_erase(struct _nvt_emmc_xbootdat_info *this, int argc, char* const argv[])
{
	int ret = 0;
	nvt_emmc_image_info *pimg;
	unsigned int rdlen;
	unsigned int calc_crc;
	unsigned char *ptr;

	if(argc != 2) {
		printf("Usage: %s audeff\n", argv[0]);
		ret = CMD_RET_USAGE;
		goto out;
	}

	pimg = nvt_emmc_get_img_by_name("param");

	if(pimg == NULL) {
		printf("%s param partition not exist !\n", __func__);
		ret = -EINVAL;
		goto out;
	}

	ret = pimg->read_img(pimg, (unsigned char*)CONFIG_SYS_FWUPDATE_BUF, &rdlen, NULL);

	if(ret < 0) {
		printf("%s read param partition fail !\n", __func__);
		goto out;
	}

	ptr = (unsigned char*)CONFIG_SYS_FWUPDATE_BUF;
	ptr += EMMC_SECTOR_SIZE;
	ptr += SKYWORTH_PARAM_AUDEFF_OFS;

	*ptr = 0;

	//update crc
	ptr = (unsigned char*)CONFIG_SYS_FWUPDATE_BUF;
	ptr += EMMC_SECTOR_SIZE;
	calc_crc = crc32_sky(ptr, EMMC_SECTOR_SIZE - sizeof(int));
	calc_crc = htonl(calc_crc);
	ptr += (EMMC_SECTOR_SIZE-sizeof(int));
	memcpy(ptr, &calc_crc, sizeof(int));

	ret = pimg->write_img(pimg, (unsigned char*)CONFIG_SYS_FWUPDATE_BUF, rdlen, NULL);
	if(ret < 0) {
		printf("%s write audio effect type to param partition fail !\n", __func__);
		ret = -EIO;
		goto out;
	}

out:
	return ret;

}
