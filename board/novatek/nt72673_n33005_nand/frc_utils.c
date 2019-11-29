#include <common.h>
#include <command.h>

#include <i2c.h>
#include <malloc.h>
#include <asm/arch/hardware.h>

#define FRC_SLAVEID		(0x34)

#define TRUE        (1)                  /*!< Define TRUE 1 */
#define FALSE       (0)                  /*!< Define FALSE 0 */

#define DEBUG_FRC
#ifdef DEBUG_FRC
#define DBG(fmt,args...)	do {	\
		printf (fmt ,##args);	\
	} while (0)
#else
#define DBG(fmt,args...)
#endif

#define REG_GPD_BASE                    (0xFD110000)
#define REG_GPD_VALUE                   (REG_GPD_BASE+0x00)
#define REG_GPD_CLEAR                   (REG_GPD_BASE+0x00)
#define REG_GPD_SET                     (REG_GPD_BASE+0x04)
#define REG_GPD_DIR                     (REG_GPD_BASE+0x08)
#define GPD_DIR_OUTPUT(index)           ((*(volatile unsigned long*)REG_GPD_DIR) |= (1<<(index)))
#define GPD_OUTPUT_SET(index)           ((*(volatile unsigned long*)REG_GPD_SET) |= (1<<(index)) )
#define GPD_DIR_INPUT(index)            ((*(volatile unsigned long*)REG_GPD_DIR) &= ~(1<<(index)))
#define GPD_INPUT_GET(offset)           (((*(volatile unsigned long*)REG_GPD_VALUE) >> (offset)) & 1)
#define GPD_OUTPUT_CLEAR(index)           ((*(volatile unsigned long*)REG_GPD_CLEAR) |= (1<<(index)) )//333 reset and clock 12MHz

extern char *source_cmd;

int nt72668_FRC_WriteReg(unsigned int u32RegAddr, unsigned int u32RegVal)
{
	unsigned char u8Buffer[8] = {0};
	unsigned char chipID = FRC_SLAVEID;
	int count=8;
	unsigned int addr = 0x00;
	int alen = 1;
	
	int i;
	for(i=0; i<4; ++i)
	{
		u8Buffer[i] = (unsigned char)( (u32RegAddr>>(i*8) ) & 0xFF);
		u8Buffer[i+4] = (unsigned char)( (u32RegVal>>(i*8) ) & 0xFF);
	}
	
	if(0!=i2c_write(chipID, addr, alen, u8Buffer, count))
	{
		DBG("Write FRC register 0x%x failed\n", u32RegAddr);
		return -1;
	}

	return 1;
}

int nt72668_FRC_ReadReg(unsigned int u32RegAddr, unsigned int* pu32RegVal)
{
	unsigned char u8Buffer[4] = {0};
	unsigned char chipID = FRC_SLAVEID;
	int count=4;
	unsigned int addr = 0x0;
	int alen = 1;
	unsigned int value=0;
	
	int i;
	for(i=0; i<4; ++i)
	{
		u8Buffer[i] = (unsigned char)( (u32RegAddr>>(i*8) ) & 0xFF);
	}
	addr = 0x08;
	if(0!=i2c_write(chipID, addr, alen, u8Buffer, count))
	{
		DBG("i2c write failed\n");
		return -1;
	}

	addr = 0x0c;
	if(0!=i2c_read(chipID, addr, alen, u8Buffer, count))
	{
		DBG("i2c read failed\n");
		return -1;
	}

	for(i=0; i<4; ++i)
	{
		value|=( u8Buffer[i]<<(i*8) );
	}

	*pu32RegVal = value;
//	printf("read addr 0x%x value : 0x%x\n",u32RegAddr,value);
	return 1;
}

int nt72668_FRC_Unlock()
{
	u8 buffer;
	int ret=0;

	buffer = 0x31;
	ret = i2c_write(FRC_SLAVEID, 0xFE, 1, &buffer, 1);
	if(ret!=0)
		return ret;
	
	buffer = 0x72;
	ret = i2c_write(FRC_SLAVEID, 0xFF, 1, &buffer, 1);

	return ret;
}

int nt72668_FRC_CheckExist()
{
	return i2c_probe(FRC_SLAVEID);
}

int nt72668_FRC_WriteMailBox(unsigned char u8ApiCmd, unsigned char u8SubCmd, unsigned int u32Value)
{
	unsigned int u32RegValue, u32Result;
	int ret=0;
	int count=0;
	
	nt72668_FRC_Unlock();
	
	//write command to API command register
	u32RegValue = (u8ApiCmd<<24) + (u8SubCmd<<16) + (u32Value & 0xFFFF);
	ret = nt72668_FRC_WriteReg(0x9f0a0100, u32RegValue);
	if(ret<0)
		goto error;

	while(count<10)
	{
		ret = nt72668_FRC_ReadReg(0x9f0a0100, &u32Result);
		if(ret<0)
			goto error;

		if(u32Result==0xffffffff)
			break;
		
		count++;
		mdelay(50);
	}

	//read command status
	ret = nt72668_FRC_ReadReg(0x9f0a0104, &u32Result);
	if(ret<0)
		goto error;
	
	if(u32Result != 0)
	{
		ret = -1;
		goto error;
	}

	goto out;
	
error:
	printf("FRC write mailbox failed!\n");
out:
	return ret;
}

int nt72668_FRC_ReadMailBox(unsigned char u8ApiCmd, unsigned char u8SubCmd, unsigned int* pu32Value)
{
	unsigned int u32RegValue, u32Result;
	int ret=0;
	int count=0;

	//write command to API command register
	u32RegValue = 0x80000000 | ((u8ApiCmd<<24) + (u8SubCmd<<16));
	ret = nt72668_FRC_WriteReg(0x9f0a0100, u32RegValue);
	if(ret<0)
		goto error;

	while(count<10)
	{
		ret = nt72668_FRC_ReadReg(0x9f0a0100, &u32Result);
		if(ret<0)
			goto error;

		if(u32Result==0xffffffff)
			break;
		
		count++;
		udelay(25);
	}

	//read command status
	ret = nt72668_FRC_ReadReg(0x9f0a0108, &u32Result);
	if(ret<0)
		goto error;
	
	if(u32Result != 0xffffffff)
	{
		*pu32Value = (u32Result & 0x00ffffff);
	}
	else
	{
		ret = -1;
		goto error;
	}

	goto out;
	
error:
	printf("FRC write mailbox failed!\n");
out:
	return ret;
}

int FRC_WriteByte(u8 u32Off, u8 u8Value)
{
	int bResult = TRUE;
	u8 str[2];
	int alen = 1;
	
	memset(str, 0, 2);

	str[0] = u8Value;

	if(0!=i2c_write(FRC_SLAVEID, u32Off, alen, str, 1))
	{
		DBG("i2c write failed\n");
		bResult = FALSE;
	}

	return bResult;
}

int FRC_ReadByte(u8 u32Off, u8 *pu8RegValue)
{
    int bResult = TRUE;
    u8 str[2];
	int alen = 1;
	
	memset(str, 0, 2);

	if(0!=i2c_read(FRC_SLAVEID, u32Off, alen, str, 1))
	{
		DBG("i2c read failed\n");
		bResult = FALSE;
	}

    *pu8RegValue = str[0];
	printf("read addr 0x%x value 0x%x\n",u32Off,*pu8RegValue);
	return bResult;
}

void DEV_FRC_ResetByWatchdog()
{
	u32 u32RegValue =0;
	nt72668_FRC_Unlock(); // step 1
	nt72668_FRC_WriteReg(0xC00C00F0,0x723105A5); // step 2
	nt72668_FRC_WriteReg(0xC00C00F4,0x5A501327); // step 3
	//step 4 , Addr 0x9F020008 set bit[12] as '1'
	nt72668_FRC_ReadReg(0x9F020008, &u32RegValue);
	u32RegValue |=( 1 << 12); 
	nt72668_FRC_WriteReg(0x9F020008,u32RegValue); 
	//step 5 , Set Addr(0xC00C0014) bit[25:16] as 0x003
	nt72668_FRC_ReadReg(0xC00C0014, &u32RegValue);
	u32RegValue  &=0xFE03FFFF ; 
	nt72668_FRC_WriteReg(0xC00C0014,u32RegValue); 
       
	nt72668_FRC_WriteReg(0xD00C0000,0x0000000E); // step6
	printf("%s: start to delay 1000 ms\n", __FUNCTION__);
	mdelay(1000); //step 7 Wait for 10ms for system to restart
	printf("%s: finish to delay 1000 ms\n", __FUNCTION__);
}

int DEV_FRC_UPD_CheckBufferNull(u8 *pu8ptr, s32 size)
{
	s32 i;

    for( i=0; i<size; i++ )
    {
        if( (*(pu8ptr+i)) != 0xFF )
        {
            return FALSE;
        }
    }
    return TRUE;
}

int DEV_FRC_BurstWrite(u32 u32Addr, u8 *pu8Buffer, u32 u32Size)
{
	int count = 0;
	u32 i;
	int alen = 1;
    u32 u32SubAddr ;
	u8 ucData[256]={0};

    // Note: Burst Read/Write length must be multiple of 32
    // u32Size could be 0x100, 0xE0, 0xC0, 0xA0, 0x80, 0x60, 0x40, 0x20

	
    // do endian swap if it is required
	for (i = 0; i < u32Size; i+=4)
    {
		ucData[i] = *(pu8Buffer+i+3);
	    ucData[i+1] = *(pu8Buffer+i+2);
	    ucData[i+2] = *(pu8Buffer+i+1);
	    ucData[i+3] = *(pu8Buffer+i);
    }

    // set DMA buffer start point
    FRC_WriteByte(0x20, 0x00);
	
    // write data to DMA buffer
    count = u32Size;
    u32SubAddr = 0x21;

		if(0!=i2c_write(FRC_SLAVEID, u32SubAddr, alen, ucData, count))
		{
			DBG( "IIC fail to write data!!" );
			return -1;
		}		

	
    // write target address, length and trigger burst write
    count = 6;
    u32SubAddr = 0x10;
    for (i = 0; i < 4; i++)
    {
        ucData[i] = (u8)((u32Addr >> (i * 8)) & 0xFF);
    }
    ucData[i++] = (u8)((u32Size - 1) & 0xFF);
    ucData[i++] = 0x01;

	
	if(0!=i2c_write(FRC_SLAVEID, u32SubAddr, alen, ucData, count))
	{
		DBG( "IIC fail to write data!!" );
		return -1;
	}
    return TRUE;
}

int DEV_FRC_UPD_LoadData(u8 *fbuf, u32 file_pos, u32 tgt_addr, s32 size, int Alt)
{
    s32     i, j, k, count, realsize;
    u8      u8val;
    u8 *pu8DataBuf;
    int 	bsts;

    count = size % 256;
    if(count==0)
    {
        realsize = size;
    }
    else
    {
        realsize = (size/256 +1) * 256;
    }
    pu8DataBuf = (u8 *) malloc(realsize);

	memcpy(pu8DataBuf, fbuf+file_pos, size);
	
	//--- fill 0xFF to end of buffer    
    if( count != 0 )
    {
        for( i=0; i<(256-count); i++)
        {
            *(pu8DataBuf+size+i) = 0xFF;
        }
    }



	//--- load data to destination 
    if(Alt==TRUE)
    {
        u32 u32addr =0,u32value =0;
        //--- load data to destination 
        for( i=0; i<realsize; i+=256 )
        {
            if( DEV_FRC_UPD_CheckBufferNull( pu8DataBuf+i, 256) )
            {
                continue;
            }
			for(j=0; j<256; j+=32)
			{
				DEV_FRC_BurstWrite( 0x9F090F00+j, pu8DataBuf+j+i, 32); 
			}
           
           u32addr = 0x30000000 + ((tgt_addr+i) >> 8);
    	    bsts = nt72668_FRC_WriteReg(0x9F0A0140, u32addr);
            //---polling ready
    	    for( k=0; k<128; k++ )
    	    {
    	        u32value = 0xFFFFFFFF;
    	        bsts = nt72668_FRC_ReadReg(0x9F0A0140, &u32value);
    	        if( bsts==TRUE )
    		  {
    		        if( u32value==0 ) // finished
    			  break;
    	        }
    	    }
            if(bsts==FALSE) // Burst Write fail!
            {
                free(pu8DataBuf);  
                return FALSE;
            }
        } 
    }
    else
    {
        for( i=0; i<realsize; i+=32 )
        {
            if( DEV_FRC_UPD_CheckBufferNull( pu8DataBuf+i, 32) )
            {
                continue;
            }
            if(FALSE==DEV_FRC_BurstWrite( tgt_addr+(i), pu8DataBuf+i, 32))
            	{
			printf(" DEV_FRC_BurstWrite fail in %s \n",__FUNCTION__);
            	}
        }
	
    }
   
    free( pu8DataBuf );
    return TRUE;
}

int DEV_FRC_ROM_UpdateCode(unsigned long buf_addr)
{
	char cmd[64];
	char img_name[64];
	int ret;
    u8    s8NameBuf[33];
    u32  u32sub_addr;
    u32  u32sub_size;
    u32  u32sub_up_addr;
    u32  u32sub_checksum;
    u32  u32floader_addr;
    u32  u32floader_size;
    u32  u32floader_up_addr;
    u32  u32floader_checksum;
    u32  i, u32value;
    int fg_sts,Alt = FALSE; 
	u32  fbuf = CONFIG_FWUP_RAM_START;

if(buf_addr==0)
{
	// load image to ram
	sprintf(cmd, "%s 0x%x %s",source_cmd,fbuf, "frc.bin");
	ret = run_command(cmd, 0);
	if(ret != 0) {
		if(!strcmp(source_cmd,"tftp"))
			printf("Load %s from ethernet fail !\n", "frc.bin");
		else
			printf("Load %s from usb fail !\n", "frc.bin");
		ret = FALSE;
		return ret;
	}
}
else
{
	fbuf = buf_addr;
	printf("%s buffer address = 0x%x \n",__FUNCTION__,fbuf,fbuf);
}

	// load from assign address
//	fbuf = (ulong)simple_strtoul(buffer[1], NULL, 16);
	

    //--- unlock I2C Bridge
    FRC_WriteByte(0xFE, 0x31);
    FRC_WriteByte(0xFF, 0x72);
    nt72668_FRC_ReadReg(0xE00E0000, &u32value);		//dummy read
    
//--- Parsing result_I2C.bin Header......
	memset(s8NameBuf,'\0',33);
	memcpy(s8NameBuf, (void*)fbuf, 32);	
	memcpy(&u32sub_addr, (void*)(fbuf+32), 4);	
	memcpy(&u32sub_size, (void*)(fbuf+36), 4);	
	memcpy(&u32sub_up_addr, (void*)(fbuf+40), 4);	
	memcpy(&u32sub_checksum, (void*)(fbuf+44), 4);	
	memcpy(&u32floader_addr, (void*)(fbuf+48), 4);
	memcpy(&u32floader_size, (void*)(fbuf+52), 4);
	memcpy(&u32floader_up_addr, (void*)(fbuf+56), 4);
	memcpy(&u32floader_checksum, (void*)(fbuf+60), 4);
	
//--- Update Subprogram code......
    printf("\n--- Update Subprogram code...sub_addr = %x .sub_up_addr = %x.u32sub_size = %d.!",u32sub_addr,u32sub_up_addr,u32sub_size);
	if(u32sub_size > 0)
	    fg_sts = DEV_FRC_UPD_LoadData((u8 *)fbuf, u32sub_addr, u32sub_up_addr, u32sub_size,Alt);
    if( fg_sts == FALSE )
    {
        printf("\nUpdate Subprogram code.......Error!");
        return FALSE;
    }

//--- Send Subprogram Password.......
    DBG("\n--- Send Subprogram Password.......!");
    nt72668_FRC_WriteReg(0x9F0A0110, 0x72310000);
    mdelay(10);    // delay 10ms for subprogram to clear DDR //10:

    //--- Wait Subprogram Password.......
    //--- 0x9F0A011C read back value should be 0x7231FFFF
	for( i=0; i<100; i++) //100
    {
        mdelay(1);    // delay 1ms
        nt72668_FRC_ReadReg(0x9F0A011C, &u32value);
        if(u32value == 0x7231FFFF)
        {
        
            break;
        }	
    }
    if( i>=100 )
    {
        printf("\nWait Subprogram Password.......Error!u32value = %x",u32value);
        return FALSE;
    }

//--- Wait Subprogram Password OK, Update flash code......  
    printf("\nUpdate flash code.......!");
    nt72668_FRC_ReadReg(0xC00C0024, &u32value);
    printf("\nFor Rev.B Update by DEV_FRC_UPD_LoadData_Alt!u32value = %x",u32value);
    DBG("\n--- Update flash code...floader_addr = %x .floader_up_addr = %x.floader_size = %d.!",u32floader_addr,u32floader_up_addr,u32floader_size);

//    Alt = TRUE;
	if(u32floader_size > 0)
	    fg_sts = DEV_FRC_UPD_LoadData((u8 *)fbuf, u32floader_addr, u32floader_up_addr, u32floader_size,Alt);
 
    if( fg_sts == FALSE )
    {
        printf("\nUpdate flash code.......Error!");
        return FALSE;
    }

//--- Send flash code Password......

    nt72668_FRC_WriteReg(0x9F0A0110, 0x72310010);
    mdelay(10);    

//--- Wait flash code Password...... 
//--- 0x9F0A0110 read back value should be 0x7231FFFF
    for( i=0; i<8000; i++ ) //8000
    {
        mdelay(1);	// delay 1ms 
        nt72668_FRC_ReadReg(0x9F0A0110, &u32value);
        if(u32value == 0x7231FFFF)
        {
            break;
        }
    }
    if( i >= 8000 )
    {
        printf("\nWait flash code Password.......Error!u32value = %x",u32value);
        return FALSE;
    }
        printf("\n--- get flash code Password...... !");

//--- Wait flash code Password OK, Update finished!

    return TRUE;
}

void DEV_FRC_Reset()
{
       u32 u32RegValue =0;
       nt72668_FRC_Unlock(); // step 1
       nt72668_FRC_WriteReg(0xC00C00F0,0x00000000); // step 2
       nt72668_FRC_WriteReg(0xC00C00F4,0x00000000); // step 3
       //step 4 , Addr 0x9F020008 set bit[12] as '1'
       nt72668_FRC_ReadReg(0x9F020008, &u32RegValue);
       u32RegValue |=( 1 << 12); 
       nt72668_FRC_WriteReg(0x9F020008,u32RegValue); 
       //step 5 , Set Addr(0xC00C0014) bit[25:16] as 0x003
       nt72668_FRC_ReadReg(0xC00C0014, &u32RegValue);
       u32RegValue  &=0xFE0FFFFF ; 
       nt72668_FRC_WriteReg(0xC00C0014,u32RegValue); 
       
       nt72668_FRC_WriteReg(0xD00C0000,0x0000000E); // step6
       mdelay(5); //step 7 Wait for 10ms for system to restart

}

int DEV_FRC_UpdateFRCFirmware(unsigned long buf_addr)
{
    int bResult = FALSE;

    nt72668_FRC_WriteMailBox(0x0E,0x02,1); // Set SPI_WP pin is high, set SPI hardware unprotected
    DEV_FRC_ResetByWatchdog(); //reset FRC
    nt72668_FRC_Unlock(); //UNLOCK AHB
    DEV_FRC_ROM_UpdateCode(buf_addr) ;                   //  Parsing FW header 
    nt72668_FRC_WriteMailBox(0x0E,0x02,0); // Set SPI_WP pin is low, set SPI hardware protected
    DEV_FRC_Reset();
    mdelay(10);
    nt72668_FRC_Unlock(); //UNLOCK AHB
    mdelay(500);//Delay 500ms for waiting for NT72324 to boot up and run application code
    DBG("%s Done!!!\n", __FUNCTION__);

    return TRUE;
}

int DEV_FRC_GetFirmwareVersion()
{
    u32 u32Result=0;

	if(nt72668_FRC_ReadMailBox(0x0e, 0x30, &u32Result)!=TRUE){
		DBG("in %s Read Back Version fail\n",__FUNCTION__);
		return FALSE;
	}
	else
	{
		u32Result = (u32Result & 0xfff); //0~12
		DBG("FRC version is ver.%x\n", u32Result);
	}
	
    return TRUE;
}

int cmd_frc_write_reg(unsigned int u32RegAddr, unsigned int u32RegVal)
{
	if(0>nt72668_FRC_WriteReg(u32RegAddr, u32RegVal))
		printf("Write 324 register 0x%x failed!\n", u32RegAddr);
	else
		printf("Write 324 register 0x%x success\n", u32RegAddr);
}

int cmd_frc_read_reg(unsigned int u32RegAddr)
{
	unsigned int value=0;
	if(0>nt72668_FRC_ReadReg(u32RegAddr, &value))
		printf("Read 324 register 0x%x failed!\n", u32RegAddr);
	else
		printf("[0x%x] = 0x%x\n",u32RegAddr,  value);
}

int cmd_frc_write_mailbox(unsigned char u8ApiCmd, unsigned char u8SubCmd, unsigned int u32Value)
{
	if(0>nt72668_FRC_WriteMailBox(u8ApiCmd, u8SubCmd, u32Value))
		printf("Write 324 via mailbox mode failed!\n");
	else
		printf("Write 324 via mailbox success\n");
}

int cmd_frc_read_mailbox(unsigned char u8ApiCmd, unsigned char u8SubCmd)
{
	unsigned int value=0;
	if(0>nt72668_FRC_ReadMailBox(u8ApiCmd, u8SubCmd, &value))
		printf("Read 324 via mailbox failed!\n");
	else
		printf("Read value = 0x%x via 324 mailbox success\n",  value);
}

int do_nvt_frc_cmd(cmd_tbl_t * cmdtp, int flag, int argc, char * const argv[])
{
	int ret = 0;
	char cmd_buf[256] = {0};
	int cmd_ret;
	int i;
	unsigned int reg=0, val=0;
	unsigned char api, cmd;
	static int reset_frc=FALSE;
	unsigned long buf_addr;

	if(0)//(reset_frc!=TRUE)
	{
		printf("reset frc and clock 12MHz \n");
		GPD_DIR_OUTPUT(0);		//GPD0 for 333 reset gpio, set GPD0 output mode
		GPD_OUTPUT_CLEAR(0);	//GPD0 Low
	
		//GPA12 for 12MHZ
		__REG(0xfd060050) = 2;
		__REG(0xfd101000) = 34;
		__REG(0xfd0f1004) = 0x1111;
		__REG(0xfd0f0030) = 0x1000;
	
		mdelay(50);
		GPD_OUTPUT_SET(0);		//GPD0 High
		mdelay(500);
		reset_frc = TRUE;
	}

	if(strcmp(argv[1],"readreg") == 0 && argc==3)
	{
		reg = (unsigned int)simple_strtoul(argv[2], NULL, 10);
		cmd_frc_read_reg(reg);
		printf("cmd_frc_read_reg(reg);\n");
	}
	else if (strcmp(argv[1],"writereg")== 0 && argc==4)
	{
		reg = (unsigned int)simple_strtoul(argv[2], NULL, 10);
		val = (unsigned int)simple_strtoul(argv[3], NULL, 10);
		cmd_frc_write_reg(reg, val);
		printf("cmd_frc_write_reg(reg, val);\n");
	}
	else if (strcmp(argv[1],"unlock")== 0 && argc==2)
	{
		if(nt72668_FRC_Unlock()==0)
		  printf("324 unlock success!\n");
		  else
		  printf("324 unlock failed!\n");
		 
		printf("nt72668_FRC_Unlock()\n");
	}
	else if (strcmp(argv[1],"check")== 0 && argc==2)
	{
		if(nt72668_FRC_CheckExist()==0)
		  {
		  printf("324 device exist, slave id = 0x%x\n", FRC_SLAVEID);
		  DEV_FRC_GetFirmwareVersion();
		  }
		  else
		  {
		  printf("No 324 device exist\n");
		  }
		printf("nt72668_FRC_CheckExist()\n");
	}
	else if (strcmp(argv[1],"readMB")== 0 && argc==4)
	{
		api = (unsigned char)simple_strtoul(argv[2], NULL, 10);
		  cmd = (unsigned char)simple_strtoul(argv[3], NULL, 10);
		  cmd_frc_read_mailbox(api, cmd);
		printf("cmd_frc_read_mailbox(api, cmd);\n");
	}
	else if (strcmp(argv[1],"writeMB")== 0 && argc==5)
	{
		api = (unsigned char)simple_strtoul(argv[2], NULL, 10);
		  cmd = (unsigned char)simple_strtoul(argv[3], NULL, 10);
		  val = (unsigned int)simple_strtoul(argv[4], NULL, 10);
		  cmd_frc_write_mailbox(api, cmd, val);
		printf("cmd_frc_write_mailbox(api, cmd, val);\n");
	}
	else if (strcmp(argv[1],"update")== 0 && (argc<= 3))
	{
		buf_addr = (ulong)simple_strtoul(argv[2], NULL, 10);
		DEV_FRC_UpdateFRCFirmware(buf_addr);
		printf("DEV_FRC_UpdateFRCFirmware();\n");
	}
	else if (strcmp(argv[1],"version")== 0 && argc==2)
	{
		DEV_FRC_GetFirmwareVersion();
		printf("DEV_FRC_GetFirmwareVersion();\n");
	}
	else
	{
		return CMD_RET_USAGE;
	}

	return ret;

err_out:
	return ret;

}

U_BOOT_CMD(
	frc,	5,	0,	do_nvt_frc_cmd,
	"frc control cmd",
	"check - check frc exist or not\n"
	"frc unlock - unlock frc\n"
	"frc readreg - read frc register [ frc readreg addr ]\n"
	"frc writereg - write frc register [ frc writereg addr val ]\n"
	"frc readMB - read value via frc mailbox interface [ frc readMB api sub ]\n"
	"frc writeMB - write value via frc mailbox interface [ frc writeMB api sub value ]\n"
	"frc update - update frc fw [ frc update => load from frc.bin, frc update addr => load from script]\n"
	"frc version - read frc fw version[ frc version ]\n"
);



