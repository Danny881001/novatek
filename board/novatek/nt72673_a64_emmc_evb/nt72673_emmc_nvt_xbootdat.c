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
		printf("================================\n\n");
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

	if (getenv("kermem") != NULL)
		sprintf(buf,"kermem=%s ",getenv("kermem"));

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
	sprintf(buf, "maxcpus=%d rootwait ",cpus);
	strcat(cmd,buf);


	if(getenv("kparam_addon") != NULL) {
		sprintf(buf," %s ", getenv("kparam_addon"));
		strcat(cmd, buf);
	}

	if(pp_blk.p.VX1) {
		sprintf(buf, "androidboot.VX1=%d ", pp_blk.p.VX1);
		strcat(cmd, buf);
	}

	if(pp_blk.p.FRC) {
		sprintf(buf, "androidboot.FRC=%d ", pp_blk.p.FRC);
		strcat(cmd, buf);
	}

	if(pp_blk.p.SEP) {
		sprintf(buf, "androidboot.SEP=%d ", pp_blk.p.SEP);
		strcat(cmd, buf);
	}

	if(pp_blk.p.u8PanelType) {
		sprintf(buf, "androidboot.PANEL=%d ", pp_blk.p.u8PanelType);
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

	if (getenv("kermem") != NULL)
		sprintf(buf,"kermem=%s ",getenv("kermem"));

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
	sprintf(buf, "maxcpus=%d rootwait ",cpus);
	strcat(cmd,buf);


	if(getenv("kparam_addon") != NULL) {
		sprintf(buf," %s ", getenv("kparam_addon"));
		strcat(cmd, buf);
	}

	if(pp_blk.p.VX1) {
		sprintf(buf, "androidboot.VX1=%d ", pp_blk.p.VX1);
		strcat(cmd, buf);
	}

	if(pp_blk.p.FRC) {
		sprintf(buf, "androidboot.FRC=%d ", pp_blk.p.FRC);
		strcat(cmd, buf);
	}

	if(pp_blk.p.SEP) {
		sprintf(buf, "androidboot.SEP=%d ", pp_blk.p.SEP);
		strcat(cmd, buf);
	}

	if(pp_blk.p.u8PanelType) {
		sprintf(buf, "androidboot.PANEL=%d ", pp_blk.p.u8PanelType);
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
