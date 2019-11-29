
#define UBOOT_NVT_UNGZIP
//#define CONFIG_ATF

#ifndef UBOOT_NVT_UNGZIP
#include <cache.h>
#include <string.h>
#include <hw_unzip.h>
#include <serial.h>
#else
#include "hw_unzip.h"
#endif
//#include <common.h>
//#include <info.h>
//#include <ntirq.h>
//#include <nt72668_inc.h>

#define BIT(nr)			(1UL << (nr))

#define u32 unsigned int
#define u16 unsigned short
//#define CONFIG_OPT

#ifdef CONFIG_ATF
#ifdef CONFIG_OPT
ATF_OPT_DEF

ATF_OPT_HEADER
	ATF_OPTION(reg_w)
	ATF_OPTION(reg_r)
ATF_OPT_END

ATF_OPT_IMP;
ATF_OPT(reg_w);
ATF_OPT(reg_r);
#endif
int printf( const char *format, ... );
static unsigned int _readl(volatile unsigned int * addr)
{
	return *addr;
}
//#define readl(addr) _readl((volatile unsigned int*) (addr))
static u32 readl(u32 addr)
{
	u32 read =_readl((volatile unsigned int*) (addr));
#ifdef CONFIG_OPT
	if(ATF_OPT_GET(reg_r)){
		printf("atf: reg read %x  =>%x\n",  addr, read);
	}
#endif
	return read;
}

static void _writel(volatile unsigned int * addr, unsigned int val)
{
	*addr = val;
	//printf("atf:set  %p => %x\n",  addr, val);
	//printf("atf:read  %p => %x\n",  addr, *addr);
}
//#define writel(val, addr) _writel((volatile unsigned int*) (addr), (unsigned int)(val))
static void writel(u32 val, u32 addr){

	 _writel((volatile unsigned int*) (addr), (unsigned int)(val));
#ifdef CONFIG_OPT

	read = _readl((volatile unsigned int*) addr);
	if(ATF_OPT_GET(reg_w)){
		printf("atf: set %x  =>%x\n",  addr, val);
		if(val != read)
			printf("atf: reg write check fail :  %x => write  %x but %x\n", addr,  val , read);
	}
#endif
}

static unsigned short _readw(volatile unsigned short * addr)
{
#ifdef CONFIG_OPT
	if(ATF_OPT_GET(reg_r)){
		printf("atf: reg read %x  =>%x\n",  addr, *addr);
	}
#endif
	return *addr;
}
#define readw(addr) _readw((volatile unsigned short*) (addr))
static void _writew(volatile unsigned short * addr, unsigned short val)
{
	*addr = val;
#ifdef CONFIG_OPT
	if(ATF_OPT_GET(reg_w)){
		printf("atf:set  %p => %x\n",  addr, val);
		if(val != *addr)
			printf("atf: reg write check fail :  %x => write  %x but %x\n", addr,  val , *addr);
	}
#endif
}
#define writew(val, addr) _writew((volatile unsigned short*) (addr), (unsigned short)(val))



/***************************************
//	iimeplemeation 

****************************************/
//this code is for 172 and other than 670

#define HAL_ATF_REG_BASE	0xfd060100
#define HAL_ATF_OFFSET		(0x9c)
#define HAL_ATF_LENGTH		(0xa0)
#define HAL_ATF_TIMEOUT		(0xa4) //why 31 bit?

#define HAL_ATF_CONFIG		(0xa8)
#define ATF_SRC_SPI		(BIT(0))
#define ATF_SRC_NFC		(BIT(1))
#define ATF_SRC_NFC_ARM		(BIT(2))
#define ATF_START		(BIT(4))
#define ATF_TO_ZIP		(BIT(5))
#define ATF_TO_AES		(BIT(6))
#define ATF_TO_SHA		(BIT(7))
#define ATF_HW_CLEAR	(BIT(15))
#define ATF_SRC_SPI_BIT			(0)
#define ATF_SRC_SPI_BIT_LEN		(1)
#define ATF_SRC_NFC_BIT			(1)
#define ATF_SRC_NFC_BIT_LEN		(1)
#define ATF_SRC_NFC_ARM_BIT		(2)
#define ATF_SRC_NFC_ARM_BIT_LEN		(1)
#define ATF_START_BIT			(4)
#define ATF_START_BIT_LEN		(1)
#define ATF_TO_ZIP_BIT			(5)
#define ATF_TO_ZIP_BIT_LEN		(1)
#define ATF_TO_AES_BIT			(6)
#define ATF_TO_AES_BIT_LEN		(1)
#define ATF_TO_SHA_BIT			(7)
#define ATF_TO_SHA_BIT_LEN		(1)
#define ATF_MANUAL_CLEAR_BIT		(15)
#define ATF_MANUAL_CLEAR_BIT_LEN	(1)


#define HAL_ATF_INT_EN_W	(0xac)
#define ATF_TIMEOUT			(BIT(0))
#define ATF_INTR_DONE		(BIT(1))
#define ATF_TIMEOUT_BIT			(0)
#define ATF_TIMEOUT_BIT_LEN		(1)
#define ATF_INTR_DONE_BIT		(1)
#define ATF_INTR_DONE_BIT_LEN		(1)
//#define ATF_INT

/*notice : to clear this register , write 1 and write 0 is needed*/
#define HAL_ATF_INT_W		(0xac+2)
//#define ATF_TIMEOUT		(BIT(0))
//#define ATF_INTR_DONE		(BIT(1))

#define HAL_ATF_START_OFFSET	(0xb0)
#define ATF_HW_OFFSET		(0)
#define ATF_HW_OFFSET_LEN	(29-0+1)
#define ATF_VER_LOW		(30)
#define ATF_VER_LOW_LEN		(31-30+1)

#define HAL_ATF_START_LEN	(0xb4)
#define ATF_HW_LEN		(0)
#define ATF_HW_LEN_LEN		(29-0+1)
#define ATF_VER_HIGH		(30)
#define ATF_VER_HIGH_LEN	(31-30+1)	


#define	HAL_ATF_DEBUG		(0xb8)
#define ATF_FIFO_WADDR		(0)
#define ATF_FIFO_WADDR_LEN	(2-0+1)
#define ATF_FIFO_FULL		(3)
#define ATF_FIFO_FULL_LEN	(3-3+1)
#define ATF_FIFO_RADD		(4)
#define ATF_FIFO_RADD_LEN	(6-4+1)
#define ATF_FIFO_EMPTY		(7)
#define ATF_FIFO_EMPTY_LEN	(7-7+1)
#define ATF_FIFO_STATE		(8)
#define ATF_FIFO_STATE_LEN	(9-8+1)

#define HAL_ATF_AUTO_RESET_SIZE	(0xd4)	//version 2

static int atf_version = 0;
void fifo_atf_isr(void);

void fifo_atf_config(int timeout, int done_intr, int cpu_intr)
{
	u32 cfg = 0;
//	int ver_low = 
	
	atf_version = (readl(HAL_ATF_REG_BASE + HAL_ATF_START_OFFSET) >> ATF_VER_LOW) |
	((readl(HAL_ATF_REG_BASE + HAL_ATF_START_LEN)>> ATF_VER_HIGH) <<(1<<ATF_VER_LOW_LEN)); 
//	printf("atf version : %d \n", atf_version);

	if (timeout){
		writel(timeout, HAL_ATF_REG_BASE+ HAL_ATF_TIMEOUT );
		cfg |= ATF_TIMEOUT;
	}

	if (done_intr){
		cfg |= ATF_INTR_DONE;
	}
	
	writel(cfg, HAL_ATF_REG_BASE + HAL_ATF_INT_EN_W);
	//writew(cfg, HAL_ATF_REG_BASE + HAL_ATF_INT_EN_W);
#ifdef CONFIG_INTR
	NVTRequest_irq(EN_KER_IRQ_ID_FIFO_UNZIP_ATF, fifo_atf_isr);
	if(cpu_intr){
		//printf("enable atf irq %d \n", EN_KER_IRQ_ID_FIFO_UNZIP_ATF);
		NVTEnable_irq(EN_KER_IRQ_ID_FIFO_UNZIP_ATF);
	}
	else
		NVTDisable_irq(EN_KER_IRQ_ID_FIFO_UNZIP_ATF);
#endif
}

void fifo_atf_config_intr(int cpu_intr)
{
#ifdef CONFIG_INTR
	if(cpu_intr)
		NVTEnable_irq(EN_KER_IRQ_ID_FIFO_UNZIP_ATF);
	else
		NVTDisable_irq(EN_KER_IRQ_ID_FIFO_UNZIP_ATF);
#endif
}

void fifo_atf_issue(struct atf_setting config)
{
	u32 cfg = 0;
//	u32 reg = 0;

	cfg |= config.src;
	cfg |= config.dest;
	cfg |= (ATF_START );
//	printf("auto_clean_disable: %d version %d \n", config.disable_auto_clean, atf_version);

	if( !config.auto_reset && (atf_version > 0 )){

		//printf("atf disable 173 auto clean\n");
	}else if(atf_version == 0){
		//printf(" 172 version no clean\n");
	}
	else{
#ifdef CONFIG_ATF_VERSION_COMMON
		cfg |= 	ATF_HW_CLEAR;
		printf("atf  173 auto clean version: %d\n", atf_version);
		writel(config.auto_reset_size, HAL_ATF_REG_BASE+ HAL_ATF_AUTO_RESET_SIZE );
#endif
	}
	//writel(0x8000ffff, HAL_ATF_REG_BASE+ HAL_ATF_TIMEOUT );
	//writel(config.timeout, HAL_ATF_REG_BASE+ HAL_ATF_TIMEOUT );
	writel(config.length, HAL_ATF_REG_BASE+ HAL_ATF_LENGTH );
	writel(config.offset, HAL_ATF_REG_BASE+ HAL_ATF_OFFSET );
	writel(cfg, HAL_ATF_REG_BASE+ HAL_ATF_CONFIG );
	
//	fifo_regdump();

}

void fifo_atf_start(struct atf_setting config)
{
	fifo_atf_config( config.timeout,config.disable_done_intr? 0:1, 1);
	fifo_atf_issue( config);
}

void fifo_atf_isr(void)
{
	u16 status = readw( HAL_ATF_REG_BASE+ HAL_ATF_INT_W);
	u16 status2 = readw( HAL_ATF_REG_BASE+ HAL_ATF_INT_EN_W);
	u16 done_event = status  & (ATF_INTR_DONE);
	u16 timeout_event = status & (ATF_TIMEOUT);

	printf("<<atf intr :  status %x done_event %x timeout %x>>\n", status, done_event, timeout_event);
	
	if(timeout_event)
		fifo_regdump();

	writew( status, HAL_ATF_REG_BASE+HAL_ATF_INT_W);
	//writew( status2, HAL_ATF_REG_BASE+HAL_ATF_INT_EN_W);
	
	if(timeout_event){
		status -= ATF_TIMEOUT;
		status2 -= ATF_TIMEOUT;
	}
	if(done_event){
		status -= ATF_INTR_DONE;
		status2 -= ATF_INTR_DONE;
	}
	writew( status, HAL_ATF_REG_BASE+HAL_ATF_INT_W);
	//disable timeout interrupt because it will not auto clean on 172
	//printf("write 0xac = 0;");
	writew( 0, HAL_ATF_REG_BASE+HAL_ATF_INT_EN_W);

	//writew(0, HAL_ATF_REG_BASE+HAL_ATF_INT);
	//printf("change check \n");
	//fifo_regdump();
}

static void debug_print_range(char* regname, u32 reg, char * name, int offset, int len)
{
	printf("   %s->%s(0x%x): 0x%x\n", regname, name, (((1<<len) -1)<<offset), (reg & (((1<<len) -1)<<offset)) >> offset);
}
#define ATF_DR(REG, NAME) 	debug_print_range(#REG,readl(HAL_ATF_REG_BASE+REG), #NAME, NAME, NAME##_LEN);
#define ATF_DR_W(REG, NAME) 	debug_print_range(#REG,readw(HAL_ATF_REG_BASE+REG), #NAME, NAME, NAME##_LEN);

void fifo_atf_done(void)
{
	//printf(" atf done\n");
	//udelay(1000);
	//fifo_atf_config_check();
	//fifo_regdump();
	writel(0, HAL_ATF_REG_BASE+ HAL_ATF_CONFIG );
}

void fifo_atf_config_check(){
	u32 reg = readl(HAL_ATF_REG_BASE+ HAL_ATF_CONFIG );
	printf("atf config %x\n", reg);
}

void fifo_regdump(void)
{
//	u32 reg = 0;
//	int debug_mode = 0;

	printf("========atf reg dump=====\n");
#define ATF_REG_INFO(REG)			 \
	printf("	%s(%x) : 0x%x\n", #REG, REG,readl(HAL_ATF_REG_BASE  + REG));

#define ATF_REG_INFO_W(REG) 			\
	printf("	%s(%x) : 0x%x\n", #REG, REG,readw(HAL_ATF_REG_BASE  + REG));

	ATF_REG_INFO(HAL_ATF_OFFSET);
	ATF_REG_INFO(HAL_ATF_LENGTH);

	ATF_REG_INFO(HAL_ATF_CONFIG);
	ATF_DR(HAL_ATF_CONFIG, ATF_SRC_SPI_BIT);
	ATF_DR(HAL_ATF_CONFIG, ATF_SRC_NFC_BIT);
	ATF_DR(HAL_ATF_CONFIG, ATF_SRC_NFC_ARM_BIT);
	ATF_DR(HAL_ATF_CONFIG, ATF_START_BIT);
	ATF_DR(HAL_ATF_CONFIG, ATF_TO_ZIP_BIT);
	ATF_DR(HAL_ATF_CONFIG, ATF_TO_AES_BIT);
	ATF_DR(HAL_ATF_CONFIG, ATF_TO_SHA_BIT);
	ATF_DR(HAL_ATF_CONFIG, ATF_MANUAL_CLEAR_BIT); //support after version 1

	ATF_REG_INFO(HAL_ATF_TIMEOUT);
	ATF_REG_INFO_W(HAL_ATF_INT_EN_W);
	ATF_DR_W(HAL_ATF_INT_EN_W, ATF_TIMEOUT_BIT);
	ATF_DR_W(HAL_ATF_INT_EN_W, ATF_INTR_DONE_BIT);
	ATF_REG_INFO_W(HAL_ATF_INT_W);
	ATF_DR_W(HAL_ATF_INT_W, ATF_TIMEOUT_BIT);
	ATF_DR_W(HAL_ATF_INT_W, ATF_INTR_DONE_BIT);
	
	printf("========atf debug reg dump=====\n");
	ATF_REG_INFO(HAL_ATF_START_OFFSET);
	ATF_DR(HAL_ATF_START_OFFSET, ATF_VER_LOW);
	ATF_DR(HAL_ATF_START_OFFSET, ATF_HW_OFFSET);
	ATF_REG_INFO(HAL_ATF_START_LEN);
	ATF_DR(HAL_ATF_START_LEN, ATF_VER_HIGH);
	ATF_DR(HAL_ATF_START_LEN, ATF_HW_LEN);
	ATF_REG_INFO(HAL_ATF_DEBUG);
	ATF_DR(HAL_ATF_DEBUG, ATF_FIFO_WADDR);
	ATF_DR(HAL_ATF_DEBUG, ATF_FIFO_RADD);
	ATF_DR(HAL_ATF_DEBUG, ATF_FIFO_EMPTY);
	ATF_REG_INFO(HAL_ATF_AUTO_RESET_SIZE);
	
	printf("========atf reg dump=====\n");
}

#endif // for CONFIG_ATF
