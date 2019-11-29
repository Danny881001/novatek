/* hwgzip driver for no kernel service support like booting and uboot*/

//#define LINUX_NVT_UNZIP
#define UBOOT_NVT_UNGZIP
//#define CONFIG_ATF
#if defined( LINUX_NVT_UNZIP)
	#include <linux/decompress/mm.h> 
	#define printf(...) 
	#define TRACE(...) 
#elif defined( UBOOT_NVT_UNGZIP)
	#include <common.h>
	#define STATIC 
	#define INIT
	#define TRACE(...) printf(__VA_ARGS__);
	#include "hw_unzip.h"
	#define WB_DCACHE_RANGE(addr, len)	flush_dcache_range((unsigned int)(addr), (len))
	#define INV_DCACHE_RANGE(addr, len)	invalidate_dcache_range((unsigned int)(addr), (len))
#else //xboot and cvt
	#include <cache.h>
	//#include <cacheops.h>
	//#include <stdint.h>
	#include <string.h>

	#include <stdio.h>
	#define STATIC 
	#define INIT
	#define TRACE(...) //printf(__VA_ARGS__);
	#define u32 unsigned int

	#define NO_CACHABLE_ADDR( ADDR) (DC_UNCACHED(ADDR))
	#define WB_DCACHE_RANGE(addr, len)	wb_dcache_range((unsigned int)(addr), (len))
	#define INV_DCACHE_RANGE(addr, len)	inv_dcache_range((unsigned int)(addr), (len))
	#include <hw_unzip.h>
#endif
	
#define BIT(nr)			(1UL << (nr))
#define ATTRI_DESC_EFFECTIVE         	0x0001	/* indicates this line of descriptor is effective */
#define ATTRI_DESC_END               	0x0002	/* indicates to end of descriptor */
#define ATTRI_DESC_INTERRUPT         	0x0004	/* generates DMA Interrupt when the operation of
						   the descriptor line is completed */

#define ATTRI_DESC_NOP               	0x0000	/* do not execute current line and go to next line */
#define ATTRI_DESC_RSV               	0x0010	/* same as Nop */
#define ATTRI_DESC_TRAN              	0x0020	/* transfer data of one descriptor line */
#define ATTRI_DESC_LINK              	0x0030	/* link to another descriptor */

//#define HAL_GZIP_REG_BASE			 0xfd6f0000UL
//static uintptr_t HAL_GZIP_REG_BASE = 0xfd6f0000UL;

#define HAL_GZIP_HW_SETTING		( 0x00)
#define HAL_GZIP_SRC_ADMA_ADDR		( 0x04)
#define HAL_GZIP_SRC_ADMA_ADDR_CURR	( 0x08)
#define HAL_GZIP_SRC_ADMA_STATUS	( 0x0c)
#define HAL_GZIP_DES_ADMA_ADDR		( 0x10)
#define HAL_GZIP_DES_ADMA_ADDR_CURR	( 0x14)
#define HAL_GZIP_DES_ADMA_STATUS	( 0x18)
#define HAL_GZIP_HW_CONFIG		( 0x1c)
#define HAL_GZIP_HW_DEBUG		( 0x20)
#define HAL_GZIP_SRC_DCOUNT		( 0x24)
#define HAL_GZIP_DES_DCOUNT		( 0x28)
#define HAL_GZIP_TIMEOUT		( 0x2c)
#define HAL_GZIP_INTERRUPT_EN		( 0x30)
#define HAL_GZIP_INTERRUPT		( 0x34)

#define HAL_GZIP_SRC_ADMA_ADDR_FINAL	( 0x38)
#define HAL_GZIP_DES_ADMA_ADDR_FINAL 	( 0x3c)
#define HAL_GZIP_IFF_DATA_BYTE_CNT	( 0x68)
#define HAL_GZIP_IFF_DEBUG_DATA		( 0x6c)
#define HAL_GZIP_IFF_FIFO_DEBUG_CFG	( 0x108)
#define HAL_GZIP_IFF_FIFO_DEBUG_DATA	( 0x10C)

/* bit mapping of gzip setting register */
#define HAL_GZIP_CLR_DST_CNT		 (BIT(6))
#define HAL_GZIP_CLR_SRC_CNT		 (BIT(5))
#define HAL_GZIP_HW_RESET		 0x00000010UL
#define HAL_GZIP_HW_RESET		 0x00000010UL
#define HAL_GZIP_SRC_ADMA_START		 0x00000001UL
#define HAL_GZIP_SRC_ADMA_CONTINUE	 0x00000002UL
#define HAL_GZIP_DES_ADMA_START		 0x00000004UL
#define HAL_GZIP_DES_ADMA_CONTINUE	 0x00000008UL

//#define HAL_GZIP_HW_DEBUG                      (HAL_GZIP_REG_BASE + 0x20)
#define HAL_GZIP_IFF_INPUT_RDY		(BIT(4))
#define HAL_GZIP_CRC_OK			(BIT(3))
#define HAL_GZIP_FINIAL_BLOCK		(BIT(2))
#define HAL_GZIP_DST_HAVE_DATA		(BIT(1))
#define HAL_GZIP_SRC_HAVE_DATA          (BIT(0))

//#define HAL_GZIP_HW_CONFIG                     (HAL_GZIP_REG_BASE + 0x1c)
#define HAL_GZIP_IFF_CAP		(BIT(15))	//read onely)
#define HAL_GZIP_IFF_INPUT_SELECT	(BIT(14))	//0 card, 1 aes
#define HAL_GZIP_IFF_INPUT_AES		(BIT(14))	//0 card, 1 aes
#define HAL_GZIP_DEBUG_MODE_EN		(BIT(13))
#define HAL_GZIP_IFF_MODE_EN		(BIT(12))
#define DUMMY_READ_AXI_BUS_ID		(3)	//for read, write, dummy read, there is a id for each of they
#define HAL_GZIP_ADMA_STOP_DUMMY_READ   (BIT(11))
#define HAL_GZIP_DUMMY_READ_AXI_BUS_ID  (BIT(6)) //4 bits
#define HAL_GZIP_DUMMY_READ_AXI_BUS_ID_BITSHIFT	(6)
#define HAL_GZIP_ADMA_CRC         	(BIT(5))
#define HAL_GZIP_DST_BIG_ENDIAN         (BIT(4))
#define HAL_GZIP_SRC_BIG_ENDIAN         (BIT(3))
#define HAL_GZIP_BUS_SYNC               (BIT(2))
#define HAL_GZIP_RFC_1951               (BIT(1))
#define HAL_GZIP_BYPASS                 (BIT(0))

/* bit mapping of gzip interrupt register */
#define HAL_GZIP_COMMAND_CONFLICT	(BIT(21))
#define HAL_GZIP_DUMMY_READ_STOP	(BIT(20))
#define HAL_GZIP_ERROR_INVALID_DST_DSC	(BIT(19))
#define HAL_GZIP_ERROR_INVALID_SRC_DSC	(BIT(18))
#define HAL_GZIP_HEADER_RFC1951         (BIT(17))
#define HAL_GZIP_ERR_FILE_ABORTED       (BIT(16))
#define HAL_GZIP_ERR_TABLE_GEN          (BIT(15))
#define HAL_GZIP_ERR_HEADER             (BIT(14))
#define HAL_GZIP_ERR_EOF                (BIT(13))
#define HAL_GZIP_ERR_MEMORY             (BIT(12))
#define HAL_GZIP_ERR_CODE               (BIT(11))
#define HAL_GZIP_ERR_DISTANCE           (BIT(10))
#define HAL_GZIP_ERR_SIZE               (BIT(9))
#define HAL_GZIP_ERR_CRC                (BIT(8))
#define HAL_GZIP_RESERVED               (BIT(7))
#define HAL_GZIP_DUMMY_READ_FINISH	(BIT(7))
#define HAL_GZIP_ERR_TIMEOUT            (BIT(6))
#define HAL_GZIP_DST_ADMA_DATA_INSIDE   (BIT(5))
#define HAL_GZIP_DST_ADMA_STOP          (BIT(4))
#define HAL_GZIP_DST_ADMA_EOF           (BIT(3))
#define HAL_GZIP_SRC_ADMA_DATA_INSIDE   (BIT(2))
#define HAL_GZIP_SRC_ADMA_STOP          (BIT(1))
#define HAL_GZIP_SRC_ADMA_EOF           (BIT(0))

#if 0
#define ERROR_BTIS                    	(HAL_GZIP_ERR_FILE_ABORTED|HAL_GZIP_ERR_TABLE_GEN|HAL_GZIP_ERR_HEADER|HAL_GZIP_ERR_EOF|\
		HAL_GZIP_ERR_MEMORY|HAL_GZIP_ERR_CODE|HAL_GZIP_ERR_DISTANCE|HAL_GZIP_ERR_SIZE|HAL_GZIP_ERR_CRC|HAL_GZIP_ERR_TIMEOUT | HAL_GZIP_ERROR_INVALID_DST_DSC | HAL_GZIP_ERROR_INVALID_SRC_DSC)
#endif
#define ERROR_BTIS                    	(HAL_GZIP_ERR_FILE_ABORTED|HAL_GZIP_ERR_TABLE_GEN|HAL_GZIP_ERR_HEADER|\
		HAL_GZIP_ERR_MEMORY|HAL_GZIP_ERR_CODE|HAL_GZIP_ERR_DISTANCE|HAL_GZIP_ERR_CRC|HAL_GZIP_ERR_TIMEOUT| HAL_GZIP_ERROR_INVALID_DST_DSC | HAL_GZIP_ERROR_INVALID_SRC_DSC)
#define CONTROL_BITS                    (HAL_GZIP_DST_ADMA_STOP|HAL_GZIP_DST_ADMA_EOF|HAL_GZIP_SRC_ADMA_STOP|HAL_GZIP_SRC_ADMA_EOF)
#define CONTROL_BITS_DST                    (HAL_GZIP_DST_ADMA_STOP|HAL_GZIP_DST_ADMA_EOF|HAL_GZIP_SRC_ADMA_STOP)
#define CONTROL_BITS_670                    (HAL_GZIP_DST_ADMA_STOP|HAL_GZIP_DST_ADMA_EOF|HAL_GZIP_SRC_ADMA_STOP|HAL_GZIP_SRC_ADMA_EOF| HAL_GZIP_DUMMY_READ_FINISH)
#define CONTROL_BITS_DST_670                    (HAL_GZIP_DST_ADMA_STOP|HAL_GZIP_DST_ADMA_EOF|HAL_GZIP_SRC_ADMA_STOP| HAL_GZIP_SRC_ADMA_STOP|HAL_GZIP_DUMMY_READ_FINISH)
#define CONTROL_BITS_DST_DMMUY_READ                    (HAL_GZIP_DST_ADMA_STOP|HAL_GZIP_DST_ADMA_EOF|HAL_GZIP_SRC_ADMA_STOP| HAL_GZIP_SRC_ADMA_STOP|HAL_GZIP_DUMMY_READ_FINISH)


//#define HAL_GZIP_IFF_FIFO_DEBUG_CFG	( 0x108)
#define HAL_GZIP_IFF_DEBUG_EN		(BIT(8))
#define HAL_GZIP_IFF_DEBUG_EN_OFFSET	(8)
#define HAL_GZIP_IFF_DEBUG_EN_LEN	(1)
#define HAL_GZIP_IFF_DBG_RADDR_OFFSET	(16)
#define HAL_GZIP_IFF_DBG_RADDR_LEN	(21-16+1)
#define HAL_GZIP_IFF_DBG_SEL_OFFSET	(0)
#define HAL_GZIP_IFF_DBG_SEL_LEN	(4-0+1)

//debug mode.
#define HAL_GZIP_DEBUG_LOW_WORD		(2)
#define HAL_GZIP_DEBUG_HIGH_WORD	(3)

//#define HAL_GZIP_IFF_FIFO_DEBUG_DATA	( 0x10C)
/*
the usage to debug iffmode
When 0x108 bit[4:0]=1, debug register(0x10C) can read
[31:26] : iff fifo write pointer
[25:20]: iff fifo read pointer
[6:0] : iff fifo remainder

When iff debug mode2 is enabled (reg 0x108[8])
When 0x108 bit[4:0]=2, debug register(0x10C) can read iff fifo output [63:32] (address = reg0x108[21:16]

When 0x108 bit[4:0]=3, debug register(0x10C) can read iff fifo output [31:0] (address = reg0x108[21:16]


*/

typedef enum gzip_attribute_e {
        EN_DESC_VALID     = 0x01,
        EN_DESC_END       = 0x02,
        EN_DESC_INT       = 0x04,
        EN_DESC_TRANSFER  = 0x20,
        EN_DESC_LINK      = 0x30
} gzip_attribute_t;

typedef struct gzip_desc_s {
        unsigned short attribute;
        unsigned short len;                     /* decode length */
        unsigned int   addr;            /* buffer address(physical) */
} gzip_desc_t;

#define NVT_UNGZIP_BASE (0xfd6f0000)
#define _DMA_MAX_LENGTH ((1<<16) -1) //the hw capability
#define DMA_MAX_LENGTH ((1<<15))        //the sw setting
//#define MAX_ADMA_NUM ((OUTPUT_MAX_LENGTH/DMA_MAX_LENGTH+1)	//output usually is large than input.
//#define MAX_WADMA_NUM MAX_ADMA_NUM//for out_des
//#define MAX_RADMA_NUM MAX_ADMA_NUM//for in_des
#define MAX_WADMA_NUM (OUTPUT_MAX_LENGTH/DMA_MAX_LENGTH+1)//for out_des
#define MAX_RADMA_NUM (INPUT_MAX_LENGTH/DMA_MAX_LENGTH+1)//for in_des
gzip_desc_t in_des[MAX_RADMA_NUM] __attribute__((aligned(32))) ;
gzip_desc_t out_des[MAX_WADMA_NUM] __attribute__((aligned(32)));
//#define NVT_670_HWGZIP
//#define TEST_ADMA_STOP


//#define NON_CACHED_DES

typedef struct gzip_config_s {
	unsigned long chip_id;
	int dummy_read_mode;//this is not correct before 670.. IC after 670 should be ok.
	int iff_mode;
	enum iff_src src;
	void * out_buf;
	int bypass_mode;
	int timeout;
	int cpu_intr;
	int dma_stop;
	int cpu_intr_inited;	//internal
	int decompressed;
	int unzip_done;
	int on_request;
}gzip_config_t;
/*volatile*/ static gzip_config_t gzip_config = {0};
static unsigned int _readl(volatile unsigned int * addr)
{
	return *addr;
}

//#define readl(addr) _readl((volatile unsigned int*) (addr))
static u32 readl(u32 addr)
{
	u32 read =_readl((volatile unsigned int*) (addr));
#if 0 //cvt only
	if(UNZIP_OPT_GET(reg_r)){
		printf("unzip: reg read %x  =>%x\n",  addr, read);
	}
#endif
	return read;
}

static void _writel(volatile unsigned int * addr, unsigned int val)
{
	*addr = val;
}
//#define writel(val, addr) _writel((volatile unsigned int*) (addr), (unsigned int)(val))
static void writel(u32 val, u32 addr){

	 _writel((volatile unsigned int*) (addr), (unsigned int)(val));

#if 0 //cvt only
	u32 read = 0;
	read = _readl((volatile unsigned int*) addr);
	if(UNZIP_OPT_GET(reg_w)){
		printf("unzip: set %x  =>%x\n",  addr, val);
		if(val != read)
			printf("unzip: reg write check fail :  %x => write  %x but %x\n", addr,  val , read);
	}
#endif
}
void nvt_gunzip_config(gzip_config_t _config)
{
	TRACE("in_des: %p %p out_des:%p %p",in_des, &in_des,out_des, &out_des);
	TRACE("NVT UNGZIP : %s \n", __FUNCTION__);

	memcpy(&gzip_config, &_config, sizeof(gzip_config_t));
#ifdef GET_CHIP_ID
	gzip_config.chip_id = getChipID();
#endif
	if(gzip_config.on_request == 1){
		printf("ERROR : %s %d unzip is on request status...but should be idle\n", __func__, __LINE__);
		while(1);
	}
	gzip_config.on_request = 1;
#if 0 //always polling mode.
	if(gzip_config.cpu_intr_inited){
		if(gzip_config.cpu_intr){
			
			NVTEnable_irq( EN_KER_IRQ_ID_GUNZIP);
		}else{
			NVTDisable_irq( EN_KER_IRQ_ID_GUNZIP);
		}	
	}else{
		if(gzip_config.cpu_intr){
			//printf("unzip intr enble\n");
			NVTRequest_irq(EN_KER_IRQ_ID_GUNZIP, nvt_gunzip_intr);
			NVTEnable_irq( EN_KER_IRQ_ID_GUNZIP);
			gzip_config.cpu_intr_inited = 1;
		}
	}
#endif
#ifdef GET_CHIP_ID	
//	printf("%d %x\n", getChipID(), getChipID());
	switch(gzip_config.chip_id){
		case 0x658:case 0x668:case 0x458:case 0x656:
			gzip_config.dummy_read_mode = 0;
		break;
		default://after 670, default support.
			gzip_config.dummy_read_mode = 1;
			//gzip_config.dummy_read_mode = 0;
			break;;
	}
#endif	
	//reset
	writel( HAL_GZIP_HW_RESET, NVT_UNGZIP_BASE + HAL_GZIP_HW_SETTING);
	writel( HAL_GZIP_CLR_SRC_CNT, NVT_UNGZIP_BASE + HAL_GZIP_HW_SETTING);
	writel( HAL_GZIP_CLR_DST_CNT, NVT_UNGZIP_BASE + HAL_GZIP_HW_SETTING);
	
	//hw config
	//writel( HAL_GZIP_RFC_1951, NVT_UNGZIP_BASE + HAL_GZIP_HW_CONFIG);
	//writel( HAL_GZIP_BYPASS, NVT_UNGZIP_BASE + HAL_GZIP_HW_CONFIG);
	{	
		u32 hwconfig = 0;
		if(gzip_config.dummy_read_mode){
			//we may not need set HAL_GZIP_HW_CONFIG  usually.
			//but here we don't use default value, we set value force for more flexibility
			hwconfig |= HAL_GZIP_ADMA_STOP_DUMMY_READ | HAL_GZIP_BUS_SYNC | 
				(DUMMY_READ_AXI_BUS_ID<<HAL_GZIP_DUMMY_READ_AXI_BUS_ID_BITSHIFT);//dummy read axi bus id
		}
		hwconfig |= _config.iff_mode;

		if(gzip_config.bypass_mode)
			hwconfig |= HAL_GZIP_BYPASS;

		if( _config.iff_mode){ //if(UNZIP_OPT_GET(log_iff)){
		//	printf("iff mode :%x \n", _config.iff_mode);
		}
#if 0
		if(gzip_config.iff_mode){
			hwconfig |= HAL_GZIP_IFF_MODE_EN	
				| HAL_GZIP_IFF_INPUT_AES 
				;
		}
#endif
		writel(hwconfig, NVT_UNGZIP_BASE + HAL_GZIP_HW_CONFIG);
	}
 
	// intr setting
	{
		u32 intr_enable = 0 ;
		if(gzip_config.dummy_read_mode){
			intr_enable = ERROR_BTIS| CONTROL_BITS_DST_DMMUY_READ;
			if(gzip_config.dma_stop)
				intr_enable |= HAL_GZIP_DUMMY_READ_STOP;
			//printf("enable dummy read mode\n");
		}else{
			intr_enable = ERROR_BTIS| CONTROL_BITS_DST;
			if(gzip_config.dma_stop)
				intr_enable |= HAL_GZIP_DST_ADMA_STOP;
		}
	writel( intr_enable,NVT_UNGZIP_BASE + HAL_GZIP_INTERRUPT_EN );
	}
	//set timeout
	writel(gzip_config.timeout, NVT_UNGZIP_BASE + HAL_GZIP_TIMEOUT );

	readl( NVT_UNGZIP_BASE + HAL_GZIP_TIMEOUT );
	//writel( gzip_config.timeout, NVT_UNGZIP_BASE + HAL_GZIP_TIMEOUT );

	//reset data structure for adma ?
}

void nvt_gunzip_reset(void)
{
	memset(&gzip_config, 0x0, sizeof(gzip_config_t));
//	gzip_config.cpu_intr = 1;//force using interrupt mode
	nvt_gunzip_config(gzip_config);
}

/*interface for fifo*/
void nvt_gunzip_setting_config(int fifo_enable, enum iff_src src, int debug_mode, int bypass, int cpu_intr)
{
	gzip_config_t gzip_config = {0};

	if(fifo_enable){
		gzip_config.iff_mode |= HAL_GZIP_IFF_MODE_EN;
		if(src == iff_AES)
			gzip_config.iff_mode |= HAL_GZIP_IFF_INPUT_AES;

		if(debug_mode)
			gzip_config.iff_mode |= HAL_GZIP_DEBUG_MODE_EN;
	}

	if(bypass)
		gzip_config.bypass_mode = 1;
	
	gzip_config.cpu_intr = cpu_intr;// using interrupt mode
	gzip_config.src = src;
	gzip_config.timeout = 0xffffffff;
	nvt_gunzip_config(gzip_config);
}

void nvt_gunzip_src_adma_set(unsigned char *buf, int len)
{
	int i = 0;
	unsigned char * buf_ptr = buf;
	gzip_desc_t  * ptr_in_des = in_des; 
	TRACE("NVT UNGZIP : %s \n", __FUNCTION__);

#ifdef NON_CACHED_DES
	ptr_in_des = (void*)NO_CACHABLE_ADDR(ptr_in_des); 
#else 
	INV_DCACHE_RANGE(ptr_in_des, sizeof(gzip_desc_t)*MAX_RADMA_NUM );
#endif

	for(i = 0; (unsigned int)buf_ptr < (( (unsigned int)buf+(unsigned int)len -1) ) ;i++){
		if( ((unsigned int)buf_ptr + DMA_MAX_LENGTH) < ((unsigned int)buf+(unsigned int)len)){
			ptr_in_des[i].attribute = EN_DESC_VALID | EN_DESC_TRANSFER;
			ptr_in_des[i].len = DMA_MAX_LENGTH; 

		}else{
			ptr_in_des[i].attribute = EN_DESC_VALID | EN_DESC_TRANSFER | EN_DESC_END;
			ptr_in_des[i].len = (unsigned int )buf+ (unsigned int) len - (unsigned int) buf_ptr; 
		}
		ptr_in_des[i].addr = (unsigned int)buf_ptr;
		TRACE("in des : %d %p: attr: %x len:%x buf:%x \n", i, &(ptr_in_des[i]),ptr_in_des[i].attribute, ptr_in_des[i].len ,ptr_in_des[i].addr)
		buf_ptr += DMA_MAX_LENGTH;
	}

#ifndef NON_CACHED_DES 
	WB_DCACHE_RANGE(in_des, sizeof(gzip_desc_t)*MAX_RADMA_NUM );
#endif

#ifdef UBOOT_NVT_UNGZIP
	flush_dcache_range((long unsigned int)buf,(u32)buf+ len);
	TRACE("NVT UNGZIP :flush done \n");
	flush_dcache_range((u32)ptr_in_des, (u32)ptr_in_des+sizeof(gzip_desc_t)* MAX_RADMA_NUM);
	TRACE("NVT UNGZIP :flush indes done \n");
#endif

	writel( (u32)in_des, NVT_UNGZIP_BASE + HAL_GZIP_SRC_ADMA_ADDR);
}

void nvt_gunzip_dst_adma_set(unsigned char * out_buf, unsigned int len)
{

	int i = 0;
	unsigned char * buf_ptr = out_buf;
	gzip_desc_t  * ptr_out_des = out_des; 
	gzip_desc_t  * last_out_des = 0; 
	TRACE("NVT UNGZIP : %s \n", __FUNCTION__);

#ifdef NON_CACHED_DES
	//printf("non_cache_des\n");
	ptr_out_des = (void*)NO_CACHABLE_ADDR(ptr_out_des);
#else
	INV_DCACHE_RANGE(ptr_out_des, sizeof(gzip_desc_t)*MAX_WADMA_NUM );
#endif

	for(i = 0; (unsigned int)buf_ptr < ( (unsigned int)out_buf+(unsigned int)len ) ;i++){
		ptr_out_des[i].attribute = EN_DESC_VALID | EN_DESC_TRANSFER;
#if 1//#ifdef TEST_ADMA_STOP
		if(gzip_config.dma_stop)
			ptr_out_des[i].attribute |= EN_DESC_INT ;
#endif
		ptr_out_des[i].len = DMA_MAX_LENGTH; 

		ptr_out_des[i].addr = (unsigned int)buf_ptr; 
		buf_ptr += DMA_MAX_LENGTH;
		TRACE("out des : %d  %p: attr: %x len:%x buf:%x \n", i, &(ptr_out_des[i]), ptr_out_des[i].attribute, ptr_out_des[i].len ,ptr_out_des[i].addr);
		last_out_des = &(ptr_out_des[i]);
	}
	if(0)	//this is an optional.
		last_out_des->attribute |= EN_DESC_END;

#ifndef NON_CACHED_DES 
	WB_DCACHE_RANGE(ptr_out_des, sizeof(gzip_desc_t)*MAX_WADMA_NUM );
#endif

#ifdef UBOOT_NVT_UNGZIP
	invalidate_dcache_range((long unsigned int)out_buf, (u32)out_buf+len);
	TRACE("NVT UNGZIP :invalid out buf done \n");
	flush_dcache_range((u32)ptr_out_des, (u32)ptr_out_des+sizeof(gzip_desc_t)* MAX_WADMA_NUM);
	TRACE("NVT UNGZIP :flush out des done \n");
#endif

	writel( (u32)out_des, NVT_UNGZIP_BASE + HAL_GZIP_DES_ADMA_ADDR);
}

void dump_register(void)
{

#define UNZIP_REG_INFO(REG) \
	printf("	%s : 0x%x\n", #REG, readl(NVT_UNGZIP_BASE  + REG));
	
	printf("-------------DUMP GZIP registers------------\n");
	UNZIP_REG_INFO(HAL_GZIP_HW_SETTING);
	UNZIP_REG_INFO(HAL_GZIP_SRC_ADMA_ADDR);
	UNZIP_REG_INFO(HAL_GZIP_SRC_ADMA_ADDR_CURR);
	UNZIP_REG_INFO(HAL_GZIP_SRC_ADMA_STATUS);
	UNZIP_REG_INFO(HAL_GZIP_DES_ADMA_ADDR);
	UNZIP_REG_INFO(HAL_GZIP_DES_ADMA_ADDR_CURR);
	UNZIP_REG_INFO(HAL_GZIP_DES_ADMA_STATUS);
	UNZIP_REG_INFO(HAL_GZIP_HW_CONFIG);
	UNZIP_REG_INFO(HAL_GZIP_HW_DEBUG);
	UNZIP_REG_INFO(HAL_GZIP_SRC_DCOUNT);
	UNZIP_REG_INFO(HAL_GZIP_DES_DCOUNT);
	UNZIP_REG_INFO(HAL_GZIP_TIMEOUT);
	UNZIP_REG_INFO(HAL_GZIP_INTERRUPT_EN);
	UNZIP_REG_INFO(HAL_GZIP_INTERRUPT);
	{
		unsigned int intr = readl(NVT_UNGZIP_BASE  + HAL_GZIP_INTERRUPT);
		if( intr & ERROR_BTIS){
#define UNZIP_ERR_INTR(ERR) \
			if(intr & ERR) {printf("		ERROR 0x%lx %s\n", ERR, #ERR);}

			UNZIP_ERR_INTR(HAL_GZIP_ERR_FILE_ABORTED);
			UNZIP_ERR_INTR(HAL_GZIP_ERR_TABLE_GEN);
			UNZIP_ERR_INTR(HAL_GZIP_ERR_HEADER);
			UNZIP_ERR_INTR(HAL_GZIP_ERR_EOF);
			UNZIP_ERR_INTR(HAL_GZIP_ERR_MEMORY);
			UNZIP_ERR_INTR(HAL_GZIP_ERR_CODE);
			UNZIP_ERR_INTR(HAL_GZIP_ERR_DISTANCE);
			UNZIP_ERR_INTR(HAL_GZIP_ERR_SIZE);
			UNZIP_ERR_INTR(HAL_GZIP_ERR_CRC);
			UNZIP_ERR_INTR(HAL_GZIP_ERR_TIMEOUT);
			UNZIP_ERR_INTR(HAL_GZIP_ERROR_INVALID_SRC_DSC);
			UNZIP_ERR_INTR(HAL_GZIP_ERROR_INVALID_DST_DSC);
		}
	}

	UNZIP_REG_INFO(HAL_GZIP_SRC_ADMA_ADDR_FINAL);
	UNZIP_REG_INFO(HAL_GZIP_DES_ADMA_ADDR_FINAL);
	UNZIP_REG_INFO(HAL_GZIP_IFF_DATA_BYTE_CNT);
	UNZIP_REG_INFO(HAL_GZIP_IFF_DEBUG_DATA);
	UNZIP_REG_INFO(HAL_GZIP_IFF_FIFO_DEBUG_CFG);
	UNZIP_REG_INFO(HAL_GZIP_IFF_FIFO_DEBUG_DATA);
#ifdef CONFIG_ATF
	if(gzip_config.iff_mode != 0){
		fifo_regdump();
	}
#endif
	printf("--------------------------------------------\n");


}

int nvt_gunzip_irq(unsigned char* out_buf)
{
	unsigned int value = readl(NVT_UNGZIP_BASE + HAL_GZIP_INTERRUPT);
	int decompressed = readl(NVT_UNGZIP_BASE + HAL_GZIP_DES_DCOUNT);
	int ret = 0;

	u32 POLLING_EVENT = (gzip_config.dummy_read_mode)? HAL_GZIP_DUMMY_READ_FINISH:HAL_GZIP_DST_ADMA_EOF;
	u32 STOP_EVENT =  (gzip_config.dummy_read_mode)? HAL_GZIP_DUMMY_READ_STOP:HAL_GZIP_DST_ADMA_EOF;

	TRACE("NVT UNGZIP : %s decompressed :%d \n", __FUNCTION__, decompressed);

	if(value & ERROR_BTIS){
		gzip_config.unzip_done = ret =  -(value &(ERROR_BTIS));
		printf("NVT UNZIP ERROR : 0x%x  \n",value);
		decompressed = readl(NVT_UNGZIP_BASE + HAL_GZIP_DES_DCOUNT);
		TRACE("NVT UNGZIP : %s decompressed :%d \n", __FUNCTION__, decompressed);
		dump_register();
		while(1);
	}else if(value & POLLING_EVENT){
		decompressed = readl(NVT_UNGZIP_BASE + HAL_GZIP_DES_DCOUNT);
		TRACE("NVT UNGZIP : %s decompressed :%d \n", __FUNCTION__, decompressed);
		gzip_config.unzip_done =ret = decompressed;
	}else if(value & STOP_EVENT){
		if(gzip_config.dma_stop){
			decompressed = readl(NVT_UNGZIP_BASE + HAL_GZIP_DES_DCOUNT);
			printf("retrigger dst adma\n");
			writel( HAL_GZIP_DES_ADMA_CONTINUE, NVT_UNGZIP_BASE+HAL_GZIP_HW_SETTING);
			TRACE("NVT UNGZIP : %s decompressed :%d \n", __FUNCTION__, decompressed);
			ret = decompressed;
		}
	}else if(!value){
		TRACE("no intr event: %x \n", value);
	}else if(value == HAL_GZIP_SRC_ADMA_EOF){
		TRACE("src adma eof : %x \n", value);
	}else{
		printf("unexpect intr : %x \n", value);
		decompressed = readl(NVT_UNGZIP_BASE + HAL_GZIP_DES_DCOUNT);
		TRACE("NVT UNGZIP : %s decompressed :%d \n", __FUNCTION__, decompressed);
		dump_register();
		//run_command("sleep 1", 0);
	}
	writel( value, NVT_UNGZIP_BASE + HAL_GZIP_INTERRUPT);
	if((gzip_config.dma_stop) && (value & STOP_EVENT)){
		writel( HAL_GZIP_DES_ADMA_CONTINUE, NVT_UNGZIP_BASE+HAL_GZIP_HW_SETTING);
	}

	gzip_config.decompressed = decompressed;
#ifdef UBOOT_NVT_UNGZIP
	TRACE("NVT UNGZIP :flush out buf start \n");
	flush_dcache_range( (long unsigned int)out_buf, (u32)out_buf+decompressed);
	TRACE("NVT UNGZIP :flush out buf done \n");
#endif
	return ret;
}

int nvt_gunzip_wait(unsigned char* out_buf)
{
	if(gzip_config.on_request == 0){
		printf("gzip is idle now, just return\n");
		return 0;
	}

	while(gzip_config.unzip_done == 0 ){
		if(!gzip_config.cpu_intr){
			nvt_gunzip_irq(gzip_config.out_buf);
		}
	}
	gzip_config.on_request = 0;
	//printf("gzip is ready now, info %x \n", gzip_config.unzip_done);
	return gzip_config.unzip_done;
}
void nvt_gunzip_start(void)
{
	TRACE("NVT UNGZIP : %s \n", __FUNCTION__);
	writel(HAL_GZIP_DES_ADMA_START, NVT_UNGZIP_BASE + HAL_GZIP_HW_SETTING);

	if(gzip_config.iff_mode == 0 )
		writel(HAL_GZIP_SRC_ADMA_START, NVT_UNGZIP_BASE + HAL_GZIP_HW_SETTING);
	else{
	//	if(UNZIP_OPT_GET(log_iff))
	//		printf("zip source from iff mode\n");
	}
	gzip_config.unzip_done = 0;
}
void nvt_gunzip_prepare(unsigned char *buf_in, int len_in , unsigned char * buf_out, unsigned int len_out, int output_invalidate)
{

	if(gzip_config.iff_mode){
		writel(len_in, NVT_UNGZIP_BASE + HAL_GZIP_IFF_DATA_BYTE_CNT);

		//if(UNZIP_OPT_GET(log_iff))
		//	printf("iffmode len set 0x%x\n", len_in);
	}else
		nvt_gunzip_src_adma_set(buf_in, len_in);
	nvt_gunzip_dst_adma_set(buf_out, len_out);

	if(output_invalidate)
		INV_DCACHE_RANGE(buf_out, len_out);//do cache operation first.
}

int nvt_gunzip_fifo_mode_support(void)
{
	return readl(NVT_UNGZIP_BASE + HAL_GZIP_HW_CONFIG) & HAL_GZIP_IFF_CAP;
}

int nvt_unzip_debug_mode(void * data, unsigned long size,
			 void *output,  unsigned out_size, int bypass_mode)
{
	//1. set debug mode and format
	int i = 0;
	u32 * input = data;
	u32 hwconfig = 0;
	u32 ret = 0;
	u32 reg = 0;

	size = (size+3)/4*4;
	printf("data%p, size%lx, output%p, out_size 0x%x, bypass_mode%d \n",data, size, output, out_size, bypass_mode);	

	//reset
	writel( HAL_GZIP_HW_RESET, NVT_UNGZIP_BASE + HAL_GZIP_HW_SETTING);
	writel( HAL_GZIP_CLR_SRC_CNT, NVT_UNGZIP_BASE + HAL_GZIP_HW_SETTING);
	writel( HAL_GZIP_CLR_DST_CNT, NVT_UNGZIP_BASE + HAL_GZIP_HW_SETTING);

	hwconfig |= HAL_GZIP_IFF_MODE_EN | HAL_GZIP_DEBUG_MODE_EN	;
	if(bypass_mode)
		hwconfig |= HAL_GZIP_BYPASS;

	writel(hwconfig, NVT_UNGZIP_BASE + HAL_GZIP_HW_CONFIG);
	writel(size, NVT_UNGZIP_BASE + HAL_GZIP_IFF_DATA_BYTE_CNT);

	nvt_gunzip_dst_adma_set(output, out_size);

	writel(HAL_GZIP_DES_ADMA_START, NVT_UNGZIP_BASE + HAL_GZIP_HW_SETTING);

	for( i = 0; i < size/4 ; i++){
		writel(*input, NVT_UNGZIP_BASE + HAL_GZIP_IFF_DEBUG_DATA);
		input ++;
	}

	ret = nvt_gunzip_wait(output);

	for( i = 0; i < (1<<HAL_GZIP_IFF_DBG_RADDR_LEN) ; i++) {
		u32 high_word = 0, low_word = 0;
		
		reg = HAL_GZIP_DEBUG_LOW_WORD| HAL_GZIP_IFF_DEBUG_EN | (i<<HAL_GZIP_IFF_DBG_RADDR_OFFSET	);	
		writel(reg ,NVT_UNGZIP_BASE + HAL_GZIP_IFF_FIFO_DEBUG_CFG);
		low_word = readl(NVT_UNGZIP_BASE + HAL_GZIP_IFF_FIFO_DEBUG_DATA	);

		reg = HAL_GZIP_DEBUG_HIGH_WORD| HAL_GZIP_IFF_DEBUG_EN | (i<<HAL_GZIP_IFF_DBG_RADDR_OFFSET);	
		writel(reg ,NVT_UNGZIP_BASE + HAL_GZIP_IFF_FIFO_DEBUG_CFG);
		high_word = readl(NVT_UNGZIP_BASE + HAL_GZIP_IFF_FIFO_DEBUG_DATA	);
		printf("debug mode[0x%x]  low-> high 0x%x 0x%x\n", i,low_word, high_word );
	}

	return ret;
	
}

STATIC int INIT hwunzip(unsigned char *buf, int len,
		       int(*fill)(void*, unsigned int),
		       int(*flush)(void*, unsigned int),
		       unsigned char *out_buf,
		       int *pos,
		       void(*error)(char *x),
			int flush_input, int invalid_output) {
//#define OUTPUT_MAX_LENGTH (10<<20)	//we set 10 MB ... if kernel is 10MB , we need set bigger
	/*0. hw reset*/
	int ret = 0;
	//printf("hwunzip\n");
	if(invalid_output)
		INV_DCACHE_RANGE(out_buf, OUTPUT_MAX_LENGTH);
	
	if(flush_input)
		WB_DCACHE_RANGE(buf, len);

	nvt_gunzip_reset();

	/*1. setup input and output descriptor*/
	nvt_gunzip_src_adma_set(buf, len);
	nvt_gunzip_dst_adma_set(out_buf, OUTPUT_MAX_LENGTH);

	/*2. trigger adma--- ignore, we do this as we set src/dst adma*/
	nvt_gunzip_start();
	
	/*3. wait interrupt result*/
	ret =  nvt_gunzip_wait(out_buf);
	while(ret < 0  ){
	}

	return ret;
	
}
//#define decompress hwunzip

#ifdef UBOOT_NVT_UNGZIP
STATIC int INIT hwgunzip(unsigned char *buf, int len,
		       int(*fill)(void*, unsigned int),
		       int(*flush)(void*, unsigned int),
		       unsigned char *out_buf,
		       int *pos,
		       void(*error)(char *x))
{
	return hwunzip(buf, len, fill, flush,out_buf, pos,error, 1, 1);
}

int do_nvtungzip(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	int len = 0;
	char cmd[64] ;
	int outsize = 0;
	char filename[64] = {0};
	char outfn[64] = {0};
	const char* filesize = NULL;
	printf("%d 1 %s  2 %s 3  %s 4  %s \n", argc, argv[0], argv[1], argv[2], argv[3]);	
	if(argc < 3 )
		return CMD_RET_USAGE;

	strcpy(filename, argv[1]);
	//sprintf(outfn,"%s.out", filename);
	sprintf(outfn,"%s", argv[2]);
	memset(cmd, 0x0, 64);
#define NVT_INPUT_ADDR 0x1000000//0x2800000	
#define NVT_OUTPUT_ADDR	0x800000//0x3000000
	//usb start
	sprintf(cmd ,"usb start");
	printf("cmd %s \n", cmd);
	run_command(cmd, 0 );
	//fat load
	memset(cmd, 0x0, 64);
	sprintf(cmd, "fatload usb 0 0x%x %s", NVT_INPUT_ADDR, filename );
	printf("cmd %s \n", cmd);
	run_command(cmd, 0 );
	filesize= getenv("filesize");
	len = simple_strtoul(filesize, NULL, 16);
	printf("len : %d \n", len);
	outsize = hwgunzip((unsigned char*)NVT_INPUT_ADDR, len, NULL, NULL, (unsigned char*)NVT_OUTPUT_ADDR, 0, NULL);
	memset(cmd, 0x0, 64);
	//fat write
	sprintf(cmd, "fatwrite usb 0 0x%x %s %x", NVT_OUTPUT_ADDR, outfn, outsize );
	printf("cmd %s \n", cmd);
	if(outsize > 0)
		run_command(cmd, 0 );
	else
		printf("error : file not write\n");
	printf("cmd done \n");
	return 0;	
}


U_BOOT_CMD(
	nvtungzip, 3, 1, do_nvtungzip,
	"NVT ungzip ",
	"nvtungzip inputfile outputfile\n"
	"nvtungzip inputfile outputfile, file must on usb top folder "
);

int do_burn_unzip_pattern(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	//table for pattern;
	char * pat_name = 0;
	unsigned long long dev_addr = 0;
	char  file_name[64] ="";
	char cmd[64] ;
	int ret = 0;
#define GC_DEFAULT_TEST_ADDR    (0)
#define GC_DEFAULT_GOLD_ADDR    (2*128*1024)

#define UB_DEFAULT_TEST_ADDR    (4*128*1024)
#define UB_DEFAULT_GOLD_ADDR    (6*128*1024)
                                                                                                                                                                                                                               
#define UI_DEFAULT_TEST_ADDR    (8*128*1024)
#define UI_DEFAULT_GOLD_ADDR    (50*128*1024)
	if(argc < 2){ printf("pattern_name is needed\n"); ;return 0;}

	pat_name = argv[1];

	if(strcmp(pat_name,"gc") == 0){
		dev_addr = (10<<20) + GC_DEFAULT_TEST_ADDR;
		strcpy(file_name, "gzclose.zlib");
	}else if(strcmp(pat_name,"ub") == 0){
		dev_addr = (10<<20) + UB_DEFAULT_TEST_ADDR;
		strcpy(file_name, "u-boot.bin.gz");
	}else if(strcmp(pat_name,"ui") == 0){
		dev_addr = (10<<20) + UI_DEFAULT_TEST_ADDR;
		strcpy(file_name, "uImage.gz");
	}else if(strcmp(pat_name,"gc_golden") == 0){
		dev_addr = (10<<20) + GC_DEFAULT_GOLD_ADDR;
		strcpy(file_name, "gzclose.c");
	}else if(strcmp(pat_name,"ub_golden") == 0){
		dev_addr = (10<<20) + UB_DEFAULT_GOLD_ADDR;
		strcpy(file_name, "u-boot.raw");
	}else if(strcmp(pat_name,"ui_golden") == 0){
		dev_addr = (10<<20) + UI_DEFAULT_GOLD_ADDR;
		strcpy(file_name, "uImage.raw");
	}else if(strcmp(pat_name,"std_golden") == 0){
		dev_addr = (10<<20) + UI_DEFAULT_TEST_ADDR;
	}else if(strcmp(pat_name,"std") == 0){
		dev_addr = (10<<20) + UI_DEFAULT_GOLD_ADDR;
	}

	if(argc >=3)
		strcpy(file_name, argv[2]);
	if(argc >=4)
		dev_addr = simple_strtoul(argv[3], NULL, 16);	
	
	if(strcmp(file_name,"")==0){
		printf("file_name is needed\n");
		return 0;
	}
	unsigned long read_blks = 0;
	unsigned long  flen = 0;
	printf("mode %s filename %s dev_addr 0x%lx\n", pat_name, file_name, dev_addr);

	sprintf(cmd, "usb start");
	ret = run_command(cmd, 0);
#define BYTE_TO_SECTOR_SFT 	9
#define NVT_FWUPDATE_MAX_WRITE_BYTES 		(450 * 1024 *1024)
#define NVT_FWUPDATE_MAX_WRITE_BLKS 		(NVT_FWUPDATE_MAX_WRITE_BYTES >> BYTE_TO_SECTOR_SFT)
	do {
		sprintf(cmd, "fatload usb 0:auto 0x%x %s 0x%x 0x%lx", CONFIG_SYS_FWUPDATE_BUF,
				file_name, NVT_FWUPDATE_MAX_WRITE_BYTES, (read_blks << BYTE_TO_SECTOR_SFT));
		ret = run_command(cmd, 0);
		flen = simple_strtoul(getenv("filesize"), NULL, 16);
		unsigned long mmc_blk_size = (!(flen % 512))? flen>>BYTE_TO_SECTOR_SFT: (flen>>BYTE_TO_SECTOR_SFT)+1;
		sprintf(cmd, "mmc write %p 0x%lx 0x%lx",(unsigned char*)CONFIG_SYS_FWUPDATE_BUF, 
			(long unsigned int)((dev_addr>>BYTE_TO_SECTOR_SFT)+read_blks), mmc_blk_size);
		ret = run_command(cmd, 0);

		if(ret) break;
		if(flen < NVT_FWUPDATE_MAX_WRITE_BYTES)
			break;
		else
			read_blks += NVT_FWUPDATE_MAX_WRITE_BLKS;
	}while(1);
	return 0;
}

U_BOOT_CMD(
	unzip_pattern_mmc_burn, 3, 1, do_burn_unzip_pattern,
	"unzip_pattern_mmc_burn",
	"unzip_pattern_mmc_burn pattern_name inputfile byte_address(must be sector align)\n"
	"unzip_pattern_mmc_burn pattern_name inputfile byte_address(must be sector align)\n"
	" pattern_name : gc/ub/ui/std/ gc_golden/ub_golden/ui_golden/std_golden \n"
	" inputfile : optional if you want set file_name  yourself \n"
	" byte_address : optional if you want set address yourself\n"
	" please use hex number\n"
);
#endif
