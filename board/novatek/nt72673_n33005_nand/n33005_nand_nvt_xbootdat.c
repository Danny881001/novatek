
static int panel_xdat_help(struct _nvt_nand_xbootdat_info *this, NVT_NAND_XBOOTDAT_HELP_TYPE help_type);
static int panel_xdat_dump(struct _nvt_nand_xbootdat_info *this, int argc, char* const argv[]);
static int panel_xdat_save(struct _nvt_nand_xbootdat_info *this, int argc, char* const argv[]);
static int panel_xdat_erase(struct _nvt_nand_xbootdat_info *this, int argc, char* const argv[]);

static int kercmd_xdat_help(struct _nvt_nand_xbootdat_info *this, NVT_NAND_XBOOTDAT_HELP_TYPE help_type);
static int kercmd_xdat_dump(struct _nvt_nand_xbootdat_info *this, int argc, char* const argv[]);
static int kercmd_xdat_save(struct _nvt_nand_xbootdat_info *this, int argc, char* const argv[]);
static int kercmd_xdat_erase(struct _nvt_nand_xbootdat_info *this, int argc, char* const argv[]);

static int rcvcmd_xdat_help(struct _nvt_nand_xbootdat_info *this, NVT_NAND_XBOOTDAT_HELP_TYPE help_type);
static int rcvcmd_xdat_dump(struct _nvt_nand_xbootdat_info *this, int argc, char* const argv[]);
static int rcvcmd_xdat_save(struct _nvt_nand_xbootdat_info *this, int argc, char* const argv[]);
static int rcvcmd_xdat_erase(struct _nvt_nand_xbootdat_info *this, int argc, char* const argv[]);

static int npt_xdat_help(struct _nvt_nand_xbootdat_info *this, NVT_NAND_XBOOTDAT_HELP_TYPE help_type);
static int npt_xdat_dump(struct _nvt_nand_xbootdat_info *this, int argc, char* const argv[]);
static int npt_xdat_save(struct _nvt_nand_xbootdat_info *this, int argc, char* const argv[]);
static int npt_xdat_erase(struct _nvt_nand_xbootdat_info *this, int argc, char* const argv[]);


int nvt_prepare_kercmd(void);
int nvt_prepare_rcvcmd(void);

static  nvt_nand_xbootdat_info xdat_info[] = {
	{
		//panel param
		.xdat_name = "panel",
		.xdathelp = panel_xdat_help,
		.xdatdump = panel_xdat_dump,
		.xdatsave = panel_xdat_save,
		.xdaterase = panel_xdat_erase,
		.start_block = 0,
		.size = 1,
	},
	{
		//kernel command
		.xdat_name = "kercmd",
		.xdathelp = kercmd_xdat_help,
		.xdatdump = kercmd_xdat_dump,
		.xdatsave = kercmd_xdat_save,
		.xdaterase = kercmd_xdat_erase,
		.start_block= 0,
		.size = 1,
	},
	{
		//recovery command
		.xdat_name = "rcvcmd",
		.xdathelp = rcvcmd_xdat_help,
		.xdatdump = rcvcmd_xdat_dump,
		.xdatsave = rcvcmd_xdat_save,
		.xdaterase = rcvcmd_xdat_erase,
		.start_block= 0,
		.size = 1,
	},
	{
		/* Only Support dump NPT */
		.xdat_name = "npt",
		.xdathelp = npt_xdat_help,
		.xdatdump = npt_xdat_dump,
		.xdatsave = npt_xdat_save,
		.xdaterase = npt_xdat_erase,
		.start_block= 0,
		.size = 1,
	},

};

static int panel_xdat_help(struct _nvt_nand_xbootdat_info *this, NVT_NAND_XBOOTDAT_HELP_TYPE help_type)
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

static int panel_xdat_dump(struct _nvt_nand_xbootdat_info *this, int argc, char* const argv[])
{
	int ret = 0;
	char cmd[128] = {0};
	unsigned char *panel_buf=NULL;

	panel_index_blk __attribute__((__aligned__(64))) pi_blk;
	panel_param_blk __attribute__((__aligned__(64))) pp_blk;
	int i;
	unsigned int start_addr;

	panel_buf = (unsigned char*)malloc(( PANEL_PARM_NUM+1)*PANEL_PARAM_SZ);
	if(argc != 2) {
		printf("Usage: %s panel \n",argv[0]);
		ret = CMD_RET_USAGE;
		goto out;
	}
	start_addr = nand_calc_block_addr(this->start_block );
	memset(panel_buf, 0, ( PANEL_PARM_NUM+1)*PANEL_PARAM_SZ);
	memset(cmd, 0, sizeof(cmd));
	sprintf(cmd, "nand read 0x%x 0x%x 0x%x", panel_buf, start_addr ,( PANEL_PARM_NUM+1)*PANEL_PARAM_SZ );
	ret = run_command(cmd, 0);
	if(ret == -1) {
		printf("read panel index and config fail !\n");
		goto out;
	}

	memset(&pi_blk, 0, sizeof(pi_blk));
	//read index
	memcpy(&pi_blk, panel_buf, sizeof(pi_blk));
	if(memcmp(pi_blk.idx.sign,PANEL_INDEX_SIGN, strlen(PANEL_INDEX_SIGN)) != 0) {
		printf("panel index not valid !\n");
		goto out;
	}

	printf("panel param version : %d\n", pi_blk.idx.version);
	printf("panel param index : %d\n", pi_blk.idx.index);

	for(i=0; i < PANEL_PARM_NUM ;i++) {
		memset(&pp_blk, 0, sizeof(pp_blk));
		memcpy(&pp_blk, panel_buf+sizeof(pi_blk)+PANEL_PARAM_SZ*i, sizeof(pp_blk));
		printf("panel param signs: %s\n", pp_blk.p.sign);

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
	free(panel_buf);
	return ret;
out:
	free(panel_buf);
	return ret;
}

static int panel_xdat_save(struct _nvt_nand_xbootdat_info *this, int argc, char* const argv[])
{
	int ret = 0;
	char cmd[128] = {0};
	unsigned char *panel_buf=NULL;
	panel_index_blk pi_blk;
	u32 idx = 0;

	panel_buf = (unsigned char *)malloc(( PANEL_PARM_NUM+1)*PANEL_PARAM_SZ);

	if(argc != 4 || (strcmp(argv[2], "index") != 0 && strcmp(argv[2],"config") != 0)) {
		ret = CMD_RET_USAGE;
		goto out;
	}

	if (strcmp(argv[2], "index") == 0) {
		idx = simple_strtoul(argv[3], NULL, 10);
		if(idx >= PANEL_PARM_NUM) {
			printf("panel index must between 0~9\n");
			goto out;
		}

		memset(&pi_blk, 0, sizeof(pi_blk));
		memcpy(pi_blk.idx.sign, PANEL_INDEX_SIGN, strlen(PANEL_INDEX_SIGN));
		pi_blk.idx.version = PANEL_PARAM_VER;
		pi_blk.idx.index = idx;


		memset(panel_buf, 0, ( PANEL_PARM_NUM+1)*PANEL_PARAM_SZ);		
		memset(cmd, 0, sizeof(cmd));
		sprintf(cmd, "nand read 0x%x 0x%x 0x%x", panel_buf, nand_calc_block_addr(this->start_block),( PANEL_PARM_NUM+1)*PANEL_PARAM_SZ);
		ret = run_command(cmd, 0);
		if(ret < 0) {
			printf("read panel index and config fail !\n");
			goto out;
		}

		//copy pi_blk to panel_buf[0~512]
		memset(panel_buf, 0, sizeof(pi_blk));
		memcpy(panel_buf, &pi_blk, sizeof(pi_blk));

		memset(cmd, 0, sizeof(cmd));
		//erase kernel cmd block
		sprintf(cmd,"nerase xbootdat");
		ret = run_command(cmd, 0);

		if(ret < 0) {
			printf("erase panel index and config fail !\n");
			goto out;
		}

		memset(cmd, 0, sizeof(cmd));
		sprintf(cmd, "nand write 0x%x 0x%x 0x%x", panel_buf, nand_calc_block_addr(this->start_block),( PANEL_PARM_NUM+1)*PANEL_PARAM_SZ);
		ret = run_command(cmd, 0);


		if(ret < 0) {
			printf("save panel index fail !\n");
			goto out;
		}

	} else if(strcmp(argv[2], "config") == 0) {
		run_command("usb start", 0);
		sprintf(cmd, "fatload usb 0:auto 0x%x %s", CONFIG_FWUP_RAM_START, argv[3]);
		ret = run_command(cmd, 0);

		if(ret != 0) {
			printf("load panel config %s fail !\n", argv[3]);
			goto out;
		}	

		memset(cmd, 0, sizeof(cmd));
		//erase kernel cmd block
		sprintf(cmd,"nerase xbootdat");

		ret = run_command(cmd, 0);

		if(ret < 0) {
			printf("erase panel index and config fail !\n");
			goto out;
		}

		memset(cmd, 0, sizeof(cmd));
		sprintf(cmd, "nand write 0x%x 0x%x 0x%x", CONFIG_FWUP_RAM_START, nand_calc_block_addr(this->start_block), ( PANEL_PARM_NUM+1)*512);
		ret = run_command(cmd, 0);

		if(ret < 0) {
			printf("write panel config to xbootdat fail !\n");
			goto out;
		}
	}
	free(panel_buf);
	return ret;
out:
	free(panel_buf);
	return ret;
}

static int panel_xdat_erase(struct _nvt_nand_xbootdat_info *this, int argc, char* const argv[])
{
	char cmd[128] = {0};
	int ret = 0;

	sprintf(cmd,"nerase xbootdat");
	ret = run_command(cmd, 0);

	if(ret < 0) 
		printf("erase panel config fail !\n");

	return ret;
}

static int kercmd_xdat_help(struct _nvt_nand_xbootdat_info *this, NVT_NAND_XBOOTDAT_HELP_TYPE help_type)
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

static int kercmd_xdat_dump(struct _nvt_nand_xbootdat_info *this, int argc, char* const argv[])
{
	int ret = 0, len;
	char cmd[512] = {0};
	unsigned char *buf = (unsigned char *)CONFIG_SYS_FDT_BUF;
	unsigned char *fdt_buf = NULL;
	int fdt0_idx = nvt_nand_get_ptn_index("fdt0");
	uint32_t max_fdt_size = 0;
	int with_common_hdr = 0;
	/* FDT partition - 2BB */
	max_fdt_size = nand_calc_partition_size(fdt0_idx) - nand_get_block_size()*2;

	if(argc != 2) {
		printf("Usage: %s kercmd \n",argv[0]);
		ret = CMD_RET_USAGE;
		goto out;
	}

	memset(cmd, 0, sizeof(cmd));
	sprintf(cmd, "nand read 0x%x 0x%x 0x%x", buf,
			nand_calc_partition_addr(fdt0_idx), max_fdt_size);
	ret = run_command(cmd, 0);
	if(ret < 0) {
		printf("%s read kernel fdt fail !\n",__func__);
		ret = -1;
		goto out;
	}

	/* Check common hdr */
	if (image_get_magic((image_header_t *)buf) == IH_MAGIC) {
		printf("%s: With Common Header\n", __func__);
		with_common_hdr = 1;
	}

	/* Update fdt_buf */
	if (with_common_hdr) {
		fdt_buf = buf + sizeof(image_header_t);
	} else {
		fdt_buf = buf;
	}

	memset(cmd, 0, sizeof(cmd));
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

static int kercmd_xdat_save(struct _nvt_nand_xbootdat_info *this, int argc, char* const argv[])	
{
	char cmd[128] = {0};
	int ret = 0;
	int fdt0_idx = nvt_nand_get_ptn_index("fdt0");
	unsigned char *buf = (unsigned char *)CONFIG_SYS_FDT_BUF;
	unsigned char *fdt_buf = NULL;
	uint32_t max_fdt_size = 0;
	int with_common_hdr = 0;
	image_header_t *phdr= NULL;
	u32 write_size = 0;
	u32 page_size;

	ret = nvt_prepare_kercmd();
	if(ret < 0) {
		goto out;
		printf("%s:prepare kernel cmdline fail !",__func__);
	}

	/* FDT partition - 2BB */
	max_fdt_size = nand_calc_partition_size(fdt0_idx) - nand_get_block_size()*2;
	memset(cmd, 0, sizeof(cmd));
	sprintf(cmd, "nand read 0x%x 0x%x 0x%x", buf,
			nand_calc_partition_addr(fdt0_idx), max_fdt_size);
	ret = run_command(cmd, 0);
	if(ret < 0) {
		printf("%s read kernel fdt fail !\n",__func__);
		goto out;
	}

	/* Check common hdr */
	if (image_get_magic((image_header_t *)buf) == IH_MAGIC) {
		printf("%s: With Common Header\n", __func__);
		with_common_hdr = 1;
	}

	/* Update fdt_buf */
	if (with_common_hdr) {
		fdt_buf = buf + sizeof(image_header_t);
		phdr = (image_header_t *)buf;
	} else {
		fdt_buf = buf;
	}

	if(fdt_check_header(fdt_buf) < 0) {
		printf("%s: Invaild fdt image, run nwrite fdt first !\n", __func__);
		goto out;
	}

	/* Update FDT */
	fdt_chosen(fdt_buf);

	/* Update Common Hdr And setting write_size */
	if (with_common_hdr) {
		phdr->ih_size = cpu_to_be32(fdt_totalsize(fdt_buf));
		phdr->ih_dcrc = cpu_to_be32(crc32(0, fdt_buf, be32_to_cpu(phdr->ih_size)));
		phdr->ih_hcrc = 0;
		phdr->ih_hcrc = cpu_to_be32(crc32(0, (const unsigned char *)phdr, sizeof(image_header_t)));
		write_size = fdt_totalsize(fdt_buf) + sizeof(image_header_t);
	} else {
		write_size = fdt_totalsize(fdt_buf);
	}

	page_size = nand_get_page_size();

	if((write_size % page_size))
		write_size = (write_size & ~(page_size - 1)) + page_size;

	if(write_size > (nand_calc_max_image_size(fdt0_idx))) {
		printf("Error ! nand bad block too much, can't update\n");
		goto out;
	}
	//erase nand partition
	sprintf(cmd,"nand erase 0x%x 0x%x",nand_calc_partition_addr(fdt0_idx),
		nand_calc_partition_size(fdt0_idx));
	ret = run_command(cmd, 0);
	if(ret == -1) {
		printf("erase fdt partition fail !\n",__func__);
		goto out;
	}
	
	sprintf(cmd,"nand write 0x%x 0x%x 0x%x",buf,
		nand_calc_partition_addr(fdt0_idx),
		write_size);
	ret = run_command(cmd,0);
	if(ret == -1) {
		printf("save kernel command line to fdt fail !\n");
		goto out;
	}

	return 0;

out:

	return ret;
}

static int kercmd_xdat_erase(struct _nvt_nand_xbootdat_info *this, int argc, char* const argv[])
{
	char cmd[128] = {0};
	int ret = 0;
	int fdt0_idx = nvt_nand_get_ptn_index("fdt0");
	unsigned char *buf = (unsigned char *)CONFIG_SYS_FDT_BUF;
	unsigned char *fdt_buf = NULL;
	uint32_t max_fdt_size = 0;
	int with_common_hdr = 0;
	image_header_t *phdr= NULL;
	u32 write_size = 0;
	u32 page_size;

	/* FDT partition - 2BB */
	max_fdt_size = nand_calc_partition_size(fdt0_idx) - nand_get_block_size()*2;

	if(argc != 2) {
		printf("Usage: %s kercmd \n",argv[0]);
		ret = CMD_RET_USAGE;
		goto out;
	}

	memset(cmd, 0, sizeof(cmd));
	sprintf(cmd, "nand read 0x%x 0x%x 0x%x", buf,
			nand_calc_partition_addr(fdt0_idx), max_fdt_size);
	ret = run_command(cmd, 0);
	if(ret < 0) {
		printf("%s read kernel fdt fail !\n",__func__);
		goto out;
	}

	/* Check common hdr */
	if (image_get_magic((image_header_t *)buf) == IH_MAGIC) {
		printf("%s: With Common Header\n", __func__);
		with_common_hdr = 1;
	}

	/* Update fdt_buf */
	if (with_common_hdr) {
		fdt_buf = buf + sizeof(image_header_t);
		phdr = (image_header_t *)buf;
	} else {
		fdt_buf = buf;
	}

	if(fdt_check_header(fdt_buf) < 0) {
		printf("%s: Invaild fdt image, run nwrite fdt first !\n", __func__);
		goto out;
	}

	/* Update FDT */
	setenv("bootargs"," ");
	fdt_chosen(fdt_buf);

	/* Update Common Hdr And setting write_size */
	if (with_common_hdr) {
		phdr->ih_size = cpu_to_be32(fdt_totalsize(fdt_buf));
		phdr->ih_dcrc = cpu_to_be32(crc32(0, fdt_buf, be32_to_cpu(phdr->ih_size)));
		phdr->ih_hcrc = 0;
		phdr->ih_hcrc = cpu_to_be32(crc32(0, (const unsigned char *)phdr, sizeof(image_header_t)));
		write_size = fdt_totalsize(fdt_buf) + sizeof(image_header_t);
	} else {
		write_size = fdt_totalsize(fdt_buf);
	}

	page_size = nand_get_page_size();

	if((write_size % page_size))
		write_size = (write_size & ~(page_size - 1)) + page_size;

	if(write_size > (nand_calc_max_image_size(fdt0_idx))) {
		printf("Error ! nand bad block too much, can't update\n");
		goto out;
	}
	//erase nand partition
	sprintf(cmd,"nand erase 0x%x 0x%x",nand_calc_partition_addr(fdt0_idx),
		nand_calc_partition_size(fdt0_idx));
	ret = run_command(cmd, 0);
	if(ret == -1) {
		printf("erase fdt partition fail !\n",__func__);
		goto out;
	}
	
	sprintf(cmd,"nand write 0x%x 0x%x 0x%x",buf,
		nand_calc_partition_addr(fdt0_idx),
		write_size);
	ret = run_command(cmd,0);
	if(ret == -1) {
		printf("save kernel command line to fdt fail !\n");
		goto out;
	}

	return 0;

out:
	return ret;

}

static int rcvcmd_xdat_help(struct _nvt_nand_xbootdat_info *this, NVT_NAND_XBOOTDAT_HELP_TYPE help_type)
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

static int rcvcmd_xdat_dump(struct _nvt_nand_xbootdat_info *this, int argc, char* const argv[])
{
	int ret = 0, len;
	char cmd[512] = {0};
	unsigned char *buf = (unsigned char *)CONFIG_SYS_FDT_BUF;
	unsigned char *fdt_buf = NULL;
	int fdt1_idx = nvt_nand_get_ptn_index("fdt1");
	uint32_t max_fdt_size = 0;
	int with_common_hdr = 0;
	/* FDT partition - 2BB */
	max_fdt_size = nand_calc_partition_size(fdt1_idx) - nand_get_block_size()*2;

	if(argc != 2) {
		printf("Usage: %s rcvcmd \n",argv[0]);
		ret = CMD_RET_USAGE;
		goto out;
	}

	memset(cmd, 0, sizeof(cmd));
	sprintf(cmd, "nand read 0x%x 0x%x 0x%x", buf,
			nand_calc_partition_addr(fdt1_idx), max_fdt_size);
	ret = run_command(cmd, 0);
	if(ret < 0) {
		printf("%s read recovery fdt fail !\n",__func__);
		ret = -1;
		goto out;
	}

	/* Check common hdr */
	if (image_get_magic((image_header_t *)buf) == IH_MAGIC) {
		printf("%s: With Common Header\n", __func__);
		with_common_hdr = 1;
	}

	/* Update fdt_buf */
	if (with_common_hdr) {
		fdt_buf = buf + sizeof(image_header_t);
	} else {
		fdt_buf = buf;
	}

	memset(cmd, 0, sizeof(cmd));
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
	setenv("barg",NULL);
out:
	return ret;
}

static int rcvcmd_xdat_save(struct _nvt_nand_xbootdat_info *this, int argc, char* const argv[])	
{
	char cmd[128] = {0};
	int ret = 0;
	int fdt1_idx = nvt_nand_get_ptn_index("fdt1");
	unsigned char *buf = (unsigned char *)CONFIG_SYS_FDT_BUF;
	unsigned char *fdt_buf = NULL;
	uint32_t max_fdt_size = 0;
	int with_common_hdr = 0;
	image_header_t *phdr= NULL;
	u32 write_size = 0;
	u32 page_size;

	ret = nvt_prepare_rcvcmd();
	if(ret < 0) {
		goto out;
		printf("%s:prepare recovery cmdline fail !",__func__);
	}

	/* FDT partition - 2BB */
	max_fdt_size = nand_calc_partition_size(fdt1_idx) - nand_get_block_size()*2;
	memset(cmd, 0, sizeof(cmd));
	sprintf(cmd, "nand read 0x%x 0x%x 0x%x", buf,
			nand_calc_partition_addr(fdt1_idx), max_fdt_size);
	ret = run_command(cmd, 0);
	if(ret < 0) {
		printf("%s read recovery fdt fail !\n",__func__);
		goto out;
	}

	/* Check common hdr */
	if (image_get_magic((image_header_t *)buf) == IH_MAGIC) {
		printf("%s: With Common Header\n", __func__);
		with_common_hdr = 1;
	}

	/* Update fdt_buf */
	if (with_common_hdr) {
		fdt_buf = buf + sizeof(image_header_t);
		phdr = (image_header_t *)buf;
	} else {
		fdt_buf = buf;
	}

	if(fdt_check_header(fdt_buf) < 0) {
		printf("%s: Invaild fdt image, run nwrite fdt first !\n", __func__);
		goto out;
	}

	/* Update FDT */
	fdt_chosen(fdt_buf);

	/* Update Common Hdr And setting write_size */
	if (with_common_hdr) {
		phdr->ih_size = cpu_to_be32(fdt_totalsize(fdt_buf));
		phdr->ih_dcrc = cpu_to_be32(crc32(0, fdt_buf, be32_to_cpu(phdr->ih_size)));
		phdr->ih_hcrc = 0;
		phdr->ih_hcrc = cpu_to_be32(crc32(0, (const unsigned char *)phdr, sizeof(image_header_t)));
		write_size = fdt_totalsize(fdt_buf) + sizeof(image_header_t);
	} else {
		write_size = fdt_totalsize(fdt_buf);
	}

	page_size = nand_get_page_size();

	if((write_size % page_size))
		write_size = (write_size & ~(page_size - 1)) + page_size;

	if(write_size > (nand_calc_max_image_size(fdt1_idx))) {
		printf("Error ! nand bad block too much, can't update\n");
		goto out;
	}
	//erase nand partition
	sprintf(cmd,"nand erase 0x%x 0x%x",nand_calc_partition_addr(fdt1_idx),
		nand_calc_partition_size(fdt1_idx));
	ret = run_command(cmd, 0);
	if(ret == -1) {
		printf("erase fdt partition fail !\n",__func__);
		goto out;
	}
	
	sprintf(cmd,"nand write 0x%x 0x%x 0x%x",buf,
		nand_calc_partition_addr(fdt1_idx),
		write_size);
	ret = run_command(cmd,0);
	if(ret == -1) {
		printf("save recovery command line to fdt fail !\n");
		goto out;
	}

	return 0;

out:

	return ret;
}

static int rcvcmd_xdat_erase(struct _nvt_nand_xbootdat_info *this, int argc, char* const argv[])
{
	char cmd[128] = {0};
	int ret = 0;
	int fdt1_idx = nvt_nand_get_ptn_index("fdt1");
	unsigned char *buf = (unsigned char *)CONFIG_SYS_FDT_BUF;
	unsigned char *fdt_buf = NULL;
	uint32_t max_fdt_size = 0;
	int with_common_hdr = 0;
	image_header_t *phdr= NULL;
	u32 write_size = 0;
	u32 page_size;

	/* FDT partition - 2BB */
	max_fdt_size = nand_calc_partition_size(fdt1_idx) - nand_get_block_size()*2;

	if(argc != 2) {
		printf("Usage: %s rcvcmd \n",argv[0]);
		ret = CMD_RET_USAGE;
		goto out;
	}

	memset(cmd, 0, sizeof(cmd));
	sprintf(cmd, "nand read 0x%x 0x%x 0x%x", buf,
			nand_calc_partition_addr(fdt1_idx), max_fdt_size);
	ret = run_command(cmd, 0);
	if(ret < 0) {
		printf("%s read recovery fdt fail !\n",__func__);
		goto out;
	}

	/* Check common hdr */
	if (image_get_magic((image_header_t *)buf) == IH_MAGIC) {
		printf("%s: With Common Header\n", __func__);
		with_common_hdr = 1;
	}

	/* Update fdt_buf */
	if (with_common_hdr) {
		fdt_buf = buf + sizeof(image_header_t);
		phdr = (image_header_t *)buf;
	} else {
		fdt_buf = buf;
	}

	if(fdt_check_header(fdt_buf) < 0) {
		printf("%s: Invaild fdt image, run nwrite fdt first !\n", __func__);
		goto out;
	}

	/* Update FDT */
	setenv("bootargs"," ");
	fdt_chosen(fdt_buf);

	/* Update Common Hdr And setting write_size */
	if (with_common_hdr) {
		phdr->ih_size = cpu_to_be32(fdt_totalsize(fdt_buf));
		phdr->ih_dcrc = cpu_to_be32(crc32(0, fdt_buf, be32_to_cpu(phdr->ih_size)));
		phdr->ih_hcrc = 0;
		phdr->ih_hcrc = cpu_to_be32(crc32(0, (const unsigned char *)phdr, sizeof(image_header_t)));
		write_size = fdt_totalsize(fdt_buf) + sizeof(image_header_t);
	} else {
		write_size = fdt_totalsize(fdt_buf);
	}

	page_size = nand_get_page_size();

	if((write_size % page_size))
		write_size = (write_size & ~(page_size - 1)) + page_size;

	if(write_size > (nand_calc_max_image_size(fdt1_idx))) {
		printf("Error ! nand bad block too much, can't update\n");
		goto out;
	}
	//erase nand partition
	sprintf(cmd,"nand erase 0x%x 0x%x",nand_calc_partition_addr(fdt1_idx),
		nand_calc_partition_size(fdt1_idx));
	ret = run_command(cmd, 0);
	if(ret == -1) {
		printf("erase fdt partition fail !\n",__func__);
		goto out;
	}
	
	sprintf(cmd,"nand write 0x%x 0x%x 0x%x",buf,
		nand_calc_partition_addr(fdt1_idx),
		write_size);
	ret = run_command(cmd,0);
	if(ret == -1) {
		printf("erase recovery command line from fdt fail !\n");
		goto out;
	}

	return 0;

out:
	return ret;

}

static int npt_xdat_help(struct _nvt_nand_xbootdat_info *this, NVT_NAND_XBOOTDAT_HELP_TYPE help_type)
{
	int ret = 0;

	switch(help_type) {
		case XDAT_HELP_DUMP:
			printf("xdump npt - dump current NAND Partition Table\n");
			break;

		case XDAT_HELP_SAVE:
			printf("xsave npt - No Support!\n");
			break;

		case XDAT_HELP_ERASE:
			printf("xerase npt - No Support!\\n");
			break;

		default:
			printf("unknown xbootdat npt help ?\n");
	}

	return ret;

}

static int npt_xdat_dump(struct _nvt_nand_xbootdat_info *this, int argc, char* const argv[])
{
	nvt_nand_show_npt();
	return 0;
}

static int npt_xdat_save(struct _nvt_nand_xbootdat_info *this, int argc, char* const argv[])
{
	printf("NPT : No Support this function!\n");
	return -1;
}

static int npt_xdat_erase(struct _nvt_nand_xbootdat_info *this, int argc, char* const argv[])
{
	printf("NPT : No Support this function!\n");
	return -1;
}


int nvt_prepare_kercmd(void)
{
#if defined(CONFIG_NVT_KERCMD_LEN)
	char cmd[CONFIG_NVT_KERCMD_LEN] = {0};
	char buf[CONFIG_NVT_KERCMD_LEN] = {0};
#else
	char cmd[512] = {0};
	char buf[255] = {0};
#endif
	unsigned char *panel_buf=NULL;
	int ret = 0,len;
	int block_cnt = 0;
	int cpus;

	memset(buf, 0, sizeof(buf));

	if(NULL != getenv("console"))
		sprintf(cmd, "console=%s,115200 ", getenv("console"));
	else
		sprintf(cmd, "%s ", "console=ttyS0,115200");

	if(getenv("kparam_addon") != NULL) {
		sprintf(buf,"%s ", getenv("kparam_addon"));
		strcat(cmd, buf);
	}

	if(getenv("cpus") != NULL) {
		cpus = simple_strtoul(getenv("cpus"),NULL,10);
		if(cpus > 4 || cpus < 0)
			cpus = 4;
	} else {
		cpus = 2;
	}
	sprintf(buf, "maxcpus=%d ",cpus);
	strcat(cmd,buf);

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

	if(getenv("rootfs_mtd") != NULL) {
		sprintf(buf,"ubi.mtd=%s root=ubi0:fs0 rootfstype=ubifs ro ",
			getenv("rootfs_mtd"));
		strcat(cmd,buf);
	}

	if(getenv_yesno("quiet"))
		strcat(cmd,"quiet ");

	if(getenv("dump_restore") != NULL)
	{
		strcat(cmd,"rdinit=/linuxrc rootwait ");
	}
	else
	{
		strcat(cmd,"init=/init rootwait ");
	}

	if((getenv("use_mtdparts") != NULL) && getenv_yesno("use_mtdparts"))
	{
		if (getenv("mtdparts") != NULL)
		{
			sprintf(buf, "%s ", getenv("mtdparts"));
			strcat(cmd, buf);
		}
	}

	setenv("bootargs",cmd);

	free(panel_buf);
	return ret;
}

int nvt_prepare_rcvcmd(void)
{
	return nvt_prepare_kercmd();
}
