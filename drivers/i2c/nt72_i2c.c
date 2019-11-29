/*
 * nvt72668 i2c functions
 * for 324 frc control 
 * 2014.01.22, zeke 
 */

#include <common.h>
#include <i2c.h>

#include <asm/arch/hardware.h>


//control by i2c function mode
//Reg control dedicate to 324 FRC now...
#define I2C_REG_BASE	0xFD000000
#define I2C_M3_REG_BASE	(I2C_REG_BASE+0x300)

#define I2C_MASTER3 		__REG(I2C_M3_REG_BASE)
#define I2C_M3_CTRL			__REG(I2C_M3_REG_BASE+0xC0)
#define I2C_M3_SCK			__REG(I2C_M3_REG_BASE+0xC4)
#define I2C_M3_SDATA		__REG(I2C_M3_REG_BASE+0xC8)
#define I2C_M3_DTSIZE		__REG(I2C_M3_REG_BASE+0xCC)
#define I2C_M3_SDATA0		__REG(I2C_M3_REG_BASE+0xD0)
#define I2C_M3_SDATA1		__REG(I2C_M3_REG_BASE+0xD4)
#define I2C_M3_SADDR0		__REG(I2C_M3_REG_BASE+0xD8)
#define I2C_M3_SADDR1		__REG(I2C_M3_REG_BASE+0xDC)
#define I2C_M3_SUBADDR		__REG(I2C_M3_REG_BASE+0xE0)
#define I2C_M3_PINGPONG		__REG(I2C_M3_REG_BASE+0xE4)
#define I2C_M3_INT			__REG(I2C_M3_REG_BASE+0xE8)
#define I2C_M3_SDATA2		__REG(I2C_M3_REG_BASE+0xEC)
#define I2C_M3_SDATA3		__REG(I2C_M3_REG_BASE+0xF0)
#define I2C_M3_SADDR2		__REG(I2C_M3_REG_BASE+0xF4)
#define I2C_M3_SADDR3		__REG(I2C_M3_REG_BASE+0xF8)
#define I2C_M3_CLKSET		__REG(I2C_M3_REG_BASE+0xFC)


//#define I2C_M3_FUNCENABLE	__REG(0xFD100028) |=0x11000 // GPC 19,20 for 673A2 Bus MF
//#define I2C_M3_FUNCENABLE	__REG(0xFD0F0020) |=0x110000 // GPA 4,5 for 673B Bus ME
#define I2C_M3_FUNCENABLE  __REG(0xFD100060) |=0x0033 // GPH 0,1 for 673B Bus MD



#define BIT0		(0x1<<0)
#define BIT1		(0x1<<1)
#define BIT2		(0x1<<2)
#define BIT3		(0x1<<3)
#define BIT4		(0x1<<4)
#define BIT5		(0x1<<5)
#define BIT6		(0x1<<6)
#define BIT7		(0x1<<7)
#define BIT8		(0x1<<8)
#define BIT9		(0x1<<9)
#define BIT10	(0x1<<10)
#define BIT11	(0x1<<11)
#define BIT12	(0x1<<12)
#define BIT13	(0x1<<13)
#define BIT14	(0x1<<14)
#define BIT15	(0x1<<15)
#define BIT16	(0x1<<16)
#define BIT17	(0x1<<17)
#define BIT18	(0x1<<18)
#define BIT19	(0x1<<19)
#define BIT20	(0x1<<20)
#define BIT21	(0x1<<21)
#define BIT22	(0x1<<22)
#define BIT23	(0x1<<23)
#define BIT24	(0x1<<24)
#define BIT25	(0x1<<25)
#define BIT26	(0x1<<26)
#define BIT27	(0x1<<27)
#define BIT28	(0x1<<28)
#define BIT29	(0x1<<29)
#define BIT30	(0x1<<30)
#define BIT31	(0x1<<31)

#define _I2C_CTRL_OPERATING                 			0x00000001
#define _I2C_CTRL_TRIGGER                   			0x00000001
#define _I2C_CTRL_BUSY                      				0x00000002
#define _I2C_CTRL_I2C_ENABLE                			0x00000004
#define _I2C_CTRL_I2C_DISABLE               			0x00000000
#define _I2C_CTRL_MODE_SEL_MASK             		0x00000030
#define _I2C_CTRL_SUBADDR_ENABLE            		0x00000040
#define _I2C_CTRL_SUBADDR_DISABLE           		0x00000000
#define _I2C_CTRL_REPEAT_ENABLE             		0x00000080
#define _I2C_CTRL_REPEAT_DISABLE            		0x00000000
#define _I2C_CTRL_READ_OPERATION				BIT8
#define _I2C_CTRL_SLAVE_ADDRESS_ONLY_MASK   	0x0000FE00
#define _I2C_CTRL_SLAVE_ADDRESS_MASK        		0x0000FF00
#define _I2C_CTRL_16BITSUBADDR_ENABLE		BIT16
#define _I2C_CTRL_16BITSUBADDR_DISABLE		0
#define _I2C_CTRL_24BITSUBADDR_ENABLE		BIT17
#define _I2C_CTRL_24BITSUBADDR_DISABLE 	  	0
#define _I2C_CTRL_32BITSUBADDR_ENABLE		BIT18
#define _I2C_CTRL_32BTISUBADDR_DISABLE		0
#define _I2C_CTRL_SUBADDR_LITTLE_ENDIAN_EN  	BIT19
#define _I2C_CTRL_SUBADDR_BIG_ENDIAN_ENABLE 0

#define _I2C_CTRL_CLOCK_DUTY_ENABLE			BIT21
#define _I2C_CTRL_HIGH_DUTY_MASK				0x0000FFF0


#define _I2C_CTRL_ACK_STATUS_MASK           		0x01000000
#define _I2C_CTRL_CLOCK_STRETCH_ENABLE		BIT27
#define _I2C_CTRL_CLOCK_STRETCH_DISABLE	  	0
#define _I2C_CTRL_MASTER_CLK_STRETCH_ENABLE 	BIT28
#define _I2C_CTRL_MASTER_CLK_STRETCH_DISABLE 0

#define _I2C_CTRL_DATA_SIZE_MASK 			  	0xFFFFFF00

#define _I2C_NORMAL_MODE                    			0x00000000
#define _I2C_BURST_MODE                     			0x00000010
#define _I2C_MANUAL_MODE                    			0x00000020


#define TIMEOUT_COUNT		(512)


void i2c_init (int speed, int slaveaddr)
{
	return;
}

int i2c_probe(uchar addr)
{
	return i2c_write(addr, 0, 0, NULL, 0);	
}


int  i2c_read(unsigned char chip, unsigned int addr, int alen, unsigned char *buffer, int len)
{
	int ret=0;
	unsigned int u32CtrlMask=0;
	unsigned int count=0;
	unsigned int tempBuffer=0;
	int i, j;

	I2C_M3_FUNCENABLE;	//switch from gpio to i2c function
	
	I2C_M3_CTRL = BIT2;		//enable i2c
	I2C_M3_PINGPONG |= (BIT7|BIT23);

	//clear clock and duty
	I2C_M3_SCK = 0x1F4;
	I2C_M3_CLKSET = 0x710000;

	switch(alen)
	{
		case 0:
			I2C_M3_SUBADDR = 0;
			I2C_M3_CTRL &=(~_I2C_CTRL_SUBADDR_ENABLE);
			break;
		case 1:
			I2C_M3_SUBADDR = addr;
			I2C_M3_CTRL |=(_I2C_CTRL_SUBADDR_ENABLE);
			break;
		case 2:
			I2C_M3_SUBADDR = addr;
			I2C_M3_CTRL |=(_I2C_CTRL_SUBADDR_ENABLE | _I2C_CTRL_16BITSUBADDR_ENABLE);
			break;
		case 3:
			I2C_M3_SUBADDR = addr;
			I2C_M3_CTRL |=(_I2C_CTRL_SUBADDR_ENABLE | _I2C_CTRL_24BITSUBADDR_ENABLE);
			break;
		case 4:
			I2C_M3_SUBADDR = addr;
			I2C_M3_CTRL |=(_I2C_CTRL_SUBADDR_ENABLE | _I2C_CTRL_32BITSUBADDR_ENABLE);
			break;
		default:
			I2C_M3_SUBADDR = 0;
			I2C_M3_CTRL &=(~_I2C_CTRL_SUBADDR_ENABLE);
			break;
	}


	I2C_M3_DTSIZE = (len*8)<<8;	//bit count
	I2C_M3_PINGPONG |= (BIT7 | BIT23 | BIT13);      // set "byte alignment"
	u32CtrlMask = ( (chip<<8) | _I2C_CTRL_I2C_ENABLE | _I2C_CTRL_REPEAT_ENABLE | _I2C_CTRL_READ_OPERATION | _I2C_CTRL_CLOCK_STRETCH_ENABLE | _I2C_CTRL_MASTER_CLK_STRETCH_ENABLE | _I2C_CTRL_CLOCK_DUTY_ENABLE);
	I2C_M3_CTRL |= u32CtrlMask;
	I2C_M3_INT = (BIT0 | BIT3 | BIT4 | BIT16 | BIT17 | BIT18 | BIT19 | BIT20);

	if(len>32)
	{
		I2C_M3_INT |= BIT2;
		//for lenght > 32 large size transfer, need further operation to ping pong register
		//TODO...
	}

	//trigger
	u32CtrlMask |= (_I2C_CTRL_TRIGGER);
	I2C_M3_CTRL |= u32CtrlMask;
	
	if(len<=16)
	{
		//wait for transmit complete
		while(count<TIMEOUT_COUNT)
		{
			if(I2C_M3_INT & BIT8)		//INTR_FINISH
				break;
			
			count++;
			udelay(2);
		}

		if(count<TIMEOUT_COUNT)
		{
			if(I2C_M3_CTRL & BIT24)
			{
				printf("Slave device NACK!\n");
				ret = -1;
			}
			else
			{
				//printf("Read Success!\n");
				for(i=0; i<len/4; i++)
				{
					switch(i)
					{
						case 0:
							tempBuffer = I2C_M3_SDATA0;
							break;
						case 1:
							tempBuffer = I2C_M3_SDATA1;
							break;
						case 2:
							tempBuffer = I2C_M3_SADDR0;
							break;
						case 3:
							tempBuffer = I2C_M3_SADDR1;
							break;
						default:
							break;
					}
                    for(j=0; j<4; j++)
					{
						*buffer++ = tempBuffer & 0xFF;
						tempBuffer>>=8;
					}
				}
				
				if(len%4!=0)
				{
					switch(i)
					{
						case 0:
							tempBuffer = I2C_M3_SDATA0;
							break;
						case 1:
							tempBuffer = I2C_M3_SDATA1;
							break;
						case 2:
							tempBuffer = I2C_M3_SADDR0;
							break;
						case 3:
							tempBuffer = I2C_M3_SADDR1;
							break;
						default:
							break;
					}
					for(j=0; j<len%4; j++)
					{
						*buffer++ = tempBuffer & 0xFF;
						tempBuffer>>=8;
					}
				}

				I2C_M3_PINGPONG |= BIT7;       // Clear FIFO1
			}
		}
		else
		{
			ret =-1;
			printf("timeout failed!\n");
		}
		
	}
	else if(len<=32)
	{
		//wait for transmit complete
		while(count<TIMEOUT_COUNT)
		{
			if(I2C_M3_INT & BIT8)		//INTR_FINISH
				break;
			
			count++;
			udelay(2);
		}

		if(count<TIMEOUT_COUNT)
		{
			if(I2C_M3_CTRL & BIT24)
			{
				printf("Slave device NACK!\n");
				ret = -1;
			}
			else
			{
				//printf("Read Success!\n");
				for(i=0; i<4; i++)
				{
					switch(i)
					{
						case 0:
							tempBuffer = I2C_M3_SDATA0;
							break;
						case 1:
							tempBuffer = I2C_M3_SDATA1;
							break;
						case 2:
							tempBuffer = I2C_M3_SADDR0;
							break;
						case 3:
							tempBuffer = I2C_M3_SADDR1;
							break;
						default:
							break;
					}
					for(j=0; j<4; j++)
					{
						*buffer++ = tempBuffer & 0xFF;
						tempBuffer>>=8;
					}
				}

				I2C_M3_PINGPONG |= BIT7;       // Clear FIFO1

				len-=16;

				for(i=0; i<len/4; i++)
				{
					switch(i)
					{
						case 0:
							tempBuffer = I2C_M3_SDATA2;
							break;
						case 1:
							tempBuffer = I2C_M3_SDATA3;
							break;
						case 2:
							tempBuffer = I2C_M3_SADDR2;
							break;
						case 3:
							tempBuffer = I2C_M3_SADDR3;
							break;
						default:
							break;
					}
					for(j=0; j<4; j++)
					{
						*buffer++ = tempBuffer & 0xFF;
						tempBuffer>>=8;
					}
				}
				
				if(len%4!=0)
				{
					switch(i)
					{
						case 0:
							tempBuffer = I2C_M3_SDATA2;
							break;
						case 1:
							tempBuffer = I2C_M3_SDATA3;
							break;
						case 2:
							tempBuffer = I2C_M3_SADDR2;
							break;
						case 3:
							tempBuffer = I2C_M3_SADDR3;
							break;
						default:
							break;
					}
					for(j=0; j<len%4; j++)
					{
						*buffer++ = tempBuffer & 0xFF;
						tempBuffer>>=8;
					}
				}

				I2C_M3_PINGPONG |= BIT23;       // Clear FIFO2
			}
		}
		else
		{
			ret =-1;
			printf("timeout failed!\n");
		}
	}
	else
	{
		//for lenght > 32 large size transfer, need further operation to ping pong register
		//TODO...
	}

	I2C_M3_CTRL &= ~BIT2;	//disable i2c
	return ret;
}

int  i2c_write(unsigned char chip, unsigned int addr, int alen, unsigned char *buffer, int len)
{
	int ret=0, i, j;
	unsigned int u32CtrlMask=0;
	unsigned int count=0;
	unsigned int tempBuffer;

	I2C_M3_FUNCENABLE;	//switch from gpio to i2c function

	I2C_M3_CTRL = BIT2;

	//clear FIFO data
	I2C_M3_SDATA0 = 0;
	I2C_M3_SDATA1 = 0;
	I2C_M3_SADDR0 = 0;
	I2C_M3_SADDR1 = 0;
	I2C_M3_SDATA2 = 0;
	I2C_M3_SDATA3 = 0;
	I2C_M3_SADDR2 = 0;
	I2C_M3_SADDR3 = 0;

	//set clock and duty
	I2C_M3_SCK = 0x1F4;
	I2C_M3_CLKSET = 0x710000;

	switch(alen)
	{
		case 0:
			I2C_M3_SUBADDR = 0;
			I2C_M3_CTRL &=(~_I2C_CTRL_SUBADDR_ENABLE);
			break;
		case 1:
			I2C_M3_SUBADDR = addr;
			I2C_M3_CTRL |=(_I2C_CTRL_SUBADDR_ENABLE);
			break;
		case 2:
			I2C_M3_SUBADDR = addr;
			I2C_M3_CTRL |=(_I2C_CTRL_SUBADDR_ENABLE | _I2C_CTRL_16BITSUBADDR_ENABLE);
			break;
		case 3:
			I2C_M3_SUBADDR = addr;
			I2C_M3_CTRL |=(_I2C_CTRL_SUBADDR_ENABLE | _I2C_CTRL_24BITSUBADDR_ENABLE);
			break;
		case 4:
			I2C_M3_SUBADDR = addr;
			I2C_M3_CTRL |=(_I2C_CTRL_SUBADDR_ENABLE | _I2C_CTRL_32BITSUBADDR_ENABLE);
			break;
		default:
			I2C_M3_SUBADDR = 0;
			I2C_M3_CTRL &=(~_I2C_CTRL_SUBADDR_ENABLE);
			break;
	}

	I2C_M3_DTSIZE = (len*8)<<8;	//bit count
	I2C_M3_PINGPONG |= (BIT7 | BIT23 | BIT13);      // set "byte alignment"
	u32CtrlMask = ( (chip<<8) | _I2C_CTRL_I2C_ENABLE | _I2C_CTRL_CLOCK_STRETCH_ENABLE | _I2C_CTRL_MASTER_CLK_STRETCH_ENABLE | _I2C_CTRL_CLOCK_DUTY_ENABLE);
	I2C_M3_CTRL |= u32CtrlMask;
	I2C_M3_INT = (BIT0 | BIT3 | BIT4 | BIT16 | BIT17 | BIT18 | BIT19 | BIT20);


	if(len>32)
		I2C_M3_INT |= BIT1;		// enable tx empty INT.

	if(len<=16)
	{
		for(i=0; i<len/4; i++)
		{
			tempBuffer = 0;
			for(j=0; j<4; j++)
			{
				tempBuffer |= (*buffer++ << (j*8));
			}

			switch(i)
			{
				case 0:
					I2C_M3_SDATA0 = tempBuffer;
					break;
				case 1:
					I2C_M3_SDATA1 = tempBuffer;
					break;
				case 2:
					I2C_M3_SADDR0 = tempBuffer;
					break;
				case 3:
					I2C_M3_SADDR1 = tempBuffer;
					break;
				default:
					break;
			}
		}

		if(len%4!=0)
		{
			tempBuffer = 0;
			for(j=0; j<len%4; j++)
			{
				tempBuffer |= (*buffer++<<(j*8));
			}

			switch(i)
			{
				case 0:
					I2C_M3_SDATA0 = tempBuffer;
					break;
				case 1:
					I2C_M3_SDATA1 = tempBuffer;
					break;
				case 2:
					I2C_M3_SADDR0 = tempBuffer;
					break;
				case 3:
					I2C_M3_SADDR1 = tempBuffer;
					break;
				default:
					break;
			}
		}

		I2C_M3_CTRL |= (_I2C_CTRL_TRIGGER);	//trigger

		//wait for transmit complete
		while(count<TIMEOUT_COUNT)
		{
			if(I2C_M3_INT & BIT8)		//INTR_FINISH
				break;
			
			count++;
			udelay(2);
		}

		if(count<TIMEOUT_COUNT)
		{
			if(I2C_M3_CTRL & BIT24)
			{
				printf("Slave device NACK!\n");
				ret = -1;
			}
			else
			{
				//printf("Write Success!\n");
			}
		}
		else
		{
			ret =-1;
			printf("timeout failed!\n");
		}
	}
	else if(len<=32)
	{
		len-=16;
		
		for(i=0; i<4; i++)
		{
			tempBuffer = 0;
			for(j=0; j<4; j++)
			{
				tempBuffer |= (*buffer++ << (j*8));
			}

			switch(i)
			{
				case 0:
					I2C_M3_SDATA0 = tempBuffer;
					break;
				case 1:
					I2C_M3_SDATA1 = tempBuffer;
					break;
				case 2:
					I2C_M3_SADDR0 = tempBuffer;
					break;
				case 3:
					I2C_M3_SADDR1 = tempBuffer;
					break;
				default:
					break;
			}
		}
		
		for(i=0; i<len/4; i++)
		{
			tempBuffer = 0;
			for(j=0; j<4; j++)
			{
				tempBuffer |= (*buffer++ << (j*8));
			}

			switch(i)
			{
				case 0:
					I2C_M3_SDATA2 = tempBuffer;
					break;
				case 1:
					I2C_M3_SDATA3 = tempBuffer;
					break;
				case 2:
					I2C_M3_SADDR2 = tempBuffer;
					break;
				case 3:
					I2C_M3_SADDR3 = tempBuffer;
					break;
				default:
					break;
			}
		}

		if(len%4!=0)
		{
			tempBuffer = 0;
			for(j=0; j<len%4; j++)
			{
				tempBuffer |= (*buffer++<<(j*8));
			}

			switch(i)
			{
				case 0:
					I2C_M3_SDATA2 = tempBuffer;
					break;
				case 1:
					I2C_M3_SDATA3 = tempBuffer;
					break;
				case 2:
					I2C_M3_SADDR2 = tempBuffer;
					break;
				case 3:
					I2C_M3_SADDR3 = tempBuffer;
					break;
				default:
					break;
			}
		}
		I2C_M3_CTRL |= (_I2C_CTRL_TRIGGER);	//trigger

		//wait for transmit complete
		while(count<TIMEOUT_COUNT)
		{
			if(I2C_M3_INT & BIT8)		//INTR_FINISH
				break;
			
			count++;
			udelay(2);
		}

		if(count<TIMEOUT_COUNT)
		{
			if(I2C_M3_CTRL & BIT24)
			{
				printf("Slave device NACK!\n");
				ret = -1;
			}
			else
			{
				//printf("Write Success!\n");
			}
		}
		else
		{
			ret =-1;
			printf("timeout failed!\n");
		}
	}
	else
	{
		//large size data transfer, to do later, need ping pong operation
		for(i=0; i<4; i++)
		{
			tempBuffer = 0;
			for(j=0; j<4; j++)
			{
				tempBuffer |= (*buffer++ << (j*8));
			}

			switch(i)
			{
				case 0:
					I2C_M3_SDATA0 = tempBuffer;
					break;
				case 1:
					I2C_M3_SDATA1 = tempBuffer;
					break;
				case 2:
					I2C_M3_SADDR0 = tempBuffer;
					break;
				case 3:
					I2C_M3_SADDR1 = tempBuffer;
					break;
				default:
					break;
			}
		}
	
		for(i=0; i<len; i++)
		{
			tempBuffer = 0;
			for(j=0; j<4; j++)
			{
				tempBuffer |= (*buffer++ << (j*8));
			}

			switch(i)
			{
				case 0:
					I2C_M3_SDATA2 = tempBuffer;
					break;
				case 1:
					I2C_M3_SDATA3 = tempBuffer;
					break;
				case 2:
					I2C_M3_SADDR2 = tempBuffer;
					break;
				case 3:
					I2C_M3_SADDR3 = tempBuffer;
					break;
				default:
					break;
			}
		}
		len-=32;
		//to do
		
	}

	I2C_M3_CTRL &= ~BIT2;	//disable i2c
	return ret;
}


#if 0
//gpio sw control mode seems not working..., gpio level will drop sometimes

//The register setting below is according to 668+324 evb revA.
#define GPIO_SDA	(18)		//0xFD0F0004 bit 18, GPA18
#define GPIO_SCL		(19)		//0xFD0F0004 bit 19, GPA19
#define NT72668_SET_GPIO_INPUT(x)		__REG(0xFD0F0008)&=~(0x1<<x)
#define NT72668_SET_GPIO_OUTPUT(x)	__REG(0xFD0F0008)|=(0x1<<x)
#define NT72668_GET_GPIO_LEVEL(x)		(__REG(0xFD0F0000)>>x)&0x1

#define NT72668_SET_GPIO_HIGH(x)	\
	do{		\
		if(!(NT72668_GET_GPIO_LEVEL(x)))  \
			__REG(0xFD0F0004)|=(0x1<<x);	\
	}while(0)

#define NT72668_SET_GPIO_LOW(x)	\
	do{		\
		if(NT72668_GET_GPIO_LEVEL(x))	\
			__REG(0xFD0F0000)|=(0x1<<x);	\
	}while(0)

#if 1
void i2c_wait()
{
	int i;
	volatile unsigned int tmp=0;
	for(i=0; i<1; ++i)
		tmp = __REG(0xFD0F0000);
}

void i2c_start(void)
{
    // for second start signal on i2c_read
    //NT72668_SET_GPIO_OUTPUT(GPIO_SDA);
    //NT72668_SET_GPIO_OUTPUT(GPIO_SCL);

    //I2C_SDA = HIGH;
    NT72668_SET_GPIO_HIGH(GPIO_SDA);
    //I2C_SCL = HIGH;
    NT72668_SET_GPIO_HIGH(GPIO_SCL);
    i2c_wait();

    // send start signal
    //I2C_SDA = LOW;
    NT72668_SET_GPIO_LOW(GPIO_SDA);
    i2c_wait();
    //I2C_SCL = LOW;
    
    NT72668_SET_GPIO_LOW(GPIO_SCL);
    i2c_wait();
}

void i2c_stop(void)
{
    //NT72668_SET_GPIO_OUTPUT(GPIO_SDA);
    //NT72668_SET_GPIO_OUTPUT(GPIO_SCL);
	
    //I2C_SDA = LOW;
    NT72668_SET_GPIO_LOW(GPIO_SDA);
    i2c_wait();
    //I2C_SCL = HIGH;
    NT72668_SET_GPIO_HIGH(GPIO_SCL);
    i2c_wait();
    //I2C_SDA = HIGH;
    NT72668_SET_GPIO_HIGH(GPIO_SDA);
}

unsigned char i2c_write_byte(unsigned char value)
{
    char i=9;
    int j=0;
    unsigned  char ack;
    // upload data
    while(--i)
    {
        //i2c_wait2();
        //I2C_SDA = (value & 0x80) ? HIGH : LOW;
	 //NT72668_SET_GPIO_OUTPUT(GPIO_SDA);
	 if(value&0x80)
	 	NT72668_SET_GPIO_HIGH(GPIO_SDA);
	 else
	 	NT72668_SET_GPIO_LOW(GPIO_SDA);

//	 printf("GPIO_SDA = %d, 0xFD0F0000 = 0x%08x - 0x%08x - 0x%08x\n", NT72668_GET_GPIO_LEVEL(GPIO_SDA), __REG(0xfd0f0000), __REG(0xfd0f0004), __REG(0xfd0f0008));
		
        i2c_wait();
        // send data
        //I2C_SCL = HIGH;
        //NT72668_SET_GPIO_OUTPUT(GPIO_SCL);
    	 NT72668_SET_GPIO_HIGH(GPIO_SCL);

	 i2c_wait();
        value = value<< 1;
        //I2C_SCL = LOW;
        //NT72668_SET_GPIO_OUTPUT(GPIO_SCL);
    	 NT72668_SET_GPIO_LOW(GPIO_SCL);
    }
    // get acknowledgement
    i2c_wait();
    //I2C_SDA = HIGH;
    //NT72668_SET_GPIO_OUTPUT(GPIO_SDA);
    NT72668_SET_GPIO_HIGH(GPIO_SDA);
	
    i2c_wait();
    //I2C_SCL = HIGH;
    //NT72668_SET_GPIO_OUTPUT(GPIO_SCL);
    NT72668_SET_GPIO_HIGH(GPIO_SCL);
    
    //i2c_wait();
    //ack = I2C_SDA;
    NT72668_SET_GPIO_INPUT(GPIO_SDA);
    //i2c_wait();
    //printf("GPIO_SDA = %d, 0xFD0F0000 = 0x%08x - 0x%08x - 0x%08x\n", NT72668_GET_GPIO_LEVEL(GPIO_SDA), __REG(0xfd0f0000), __REG(0xfd0f0004), __REG(0xfd0f0008));

    for(j=0; j<128; j++)
    {
    	ack = NT72668_GET_GPIO_LEVEL(GPIO_SDA);
	if(ack==0)
		break;
	i2c_wait();
	NT72668_SET_GPIO_OUTPUT(GPIO_SDA);
    }

    //I2C_SCL = LOW;
    //NT72668_SET_GPIO_OUTPUT(GPIO_SCL);
    NT72668_SET_GPIO_LOW(GPIO_SCL);
	
    // return acknowledge
    return ack;
}

unsigned char i2c_read_byte(unsigned char acknowledge)
{
    char i=9;
    unsigned char value=0;
    // read data
     while(--i)
    {
        value <<= 1;
        i2c_wait();
        //I2C_SCL = HIGH;
	 //NT72668_SET_GPIO_OUTPUT(GPIO_SCL);
    	 NT72668_SET_GPIO_HIGH(GPIO_SCL);
		
        i2c_wait();
        //value |= I2C_SDA;
	 NT72668_SET_GPIO_INPUT(GPIO_SDA);
    	 value|=NT72668_GET_GPIO_LEVEL(GPIO_SDA);
//	 printf("GPIO_SDA = %d, 0xFD0F0000 = 0x%08x - 0x%08x - 0x%08x\n", NT72668_GET_GPIO_LEVEL(GPIO_SDA), __REG(0xfd0f0000), __REG(0xfd0f0004), __REG(0xfd0f0008));
	 NT72668_SET_GPIO_OUTPUT(GPIO_SDA);	
        //i2c_wait();
        //I2C_SCL = LOW;
        //NT72668_SET_GPIO_OUTPUT(GPIO_SCL);
    	 NT72668_SET_GPIO_LOW(GPIO_SCL);
    }
    // send acknowledge
    i2c_wait();
    //I2C_SDA = acknowledge;
    //NT72668_SET_GPIO_OUTPUT(GPIO_SDA);
    if(acknowledge)
		NT72668_SET_GPIO_HIGH(GPIO_SDA);
    else
		NT72668_SET_GPIO_LOW(GPIO_SDA);
	
    i2c_wait();
    //I2C_SCL = HIGH;
    //NT72668_SET_GPIO_OUTPUT(GPIO_SCL);
    NT72668_SET_GPIO_HIGH(GPIO_SCL);

    i2c_wait();
    //I2C_SCL = LOW;
    //NT72668_SET_GPIO_OUTPUT(GPIO_SCL);
    NT72668_SET_GPIO_LOW(GPIO_SCL);

    // return data
    return value;
}


int  i2c_write(unsigned char chip, unsigned int addr, int alen, unsigned char *buffer, int len)
{
	int shift, failures = 0;

	i2c_start();
	if(i2c_write_byte(chip)){ 
		i2c_stop();
		printf("i2c_write, no chip responded 0x%02X\n", chip);
		return(1);
	}
	shift = (alen-1) * 8;
	while(alen-- > 0) {
		if(i2c_write_byte(addr >> shift)) {
			printf("i2c_write, address not <ACK>ed\n");
			return(1);
		}
		shift -= 8;
	}

	while(len-- > 0) {
		if(i2c_write_byte(*buffer++)) {
			failures++;
		}
	}
	i2c_stop();
	return(failures);
}

int  i2c_read(unsigned char chip, unsigned int addr, int alen, unsigned char *buffer, int len)
{
	int shift;

	i2c_start();
	if(alen > 0) {
		if(i2c_write_byte(chip&0xfe)) {
			i2c_stop();
			printf("i2c_read, no chip responded 0x%02X\n", chip);
			return(1);
		}
		shift = (alen-1) * 8;
		while(alen-- > 0) {
			if(i2c_write_byte(addr >> shift)) {
				printf("i2c_read, address not <ACK>ed\n");
				return(1);
			}
			shift -= 8;
		}
	}

	i2c_write_byte(chip|1);
	while(len-- > 0) {
		*buffer++ = i2c_read_byte(1);
	}
	i2c_stop();
	return(0);
}

void i2c_init (int speed, int slaveaddr)
{
/*
	int j;

	//I2C_SCL(1);
	NT72668_SET_GPIO_OUTPUT(GPIO_SCL);
    	NT72668_SET_GPIO_HIGH(GPIO_SCL);
	//I2C_SDA(1);
	NT72668_SET_GPIO_OUTPUT(GPIO_SDA);
    	NT72668_SET_GPIO_HIGH(GPIO_SDA);

	for(j = 0; j < 9; j++) {
		//I2C_SCL(0);
		NT72668_SET_GPIO_OUTPUT(GPIO_SCL);
    		NT72668_SET_GPIO_LOW(GPIO_SCL);
		i2c_wait();
		//I2C_SCL(1);
		NT72668_SET_GPIO_OUTPUT(GPIO_SCL);
    		NT72668_SET_GPIO_HIGH(GPIO_SCL);
		i2c_wait();
	}
	i2c_stop();
*/
	NT72668_SET_GPIO_OUTPUT(GPIO_SDA);
    	NT72668_SET_GPIO_OUTPUT(GPIO_SCL);
	return;
}

int i2c_probe(uchar addr)
{
	int rc=0;

	/*
	 * perform 1 byte write transaction with just address byte
	 * (fake write)
	 */
	i2c_init(0, 0);
	i2c_wait();

	NT72668_SET_GPIO_HIGH(GPIO_SCL);
	i2c_wait();
	NT72668_SET_GPIO_LOW(GPIO_SCL);
	i2c_wait();
	NT72668_SET_GPIO_HIGH(GPIO_SCL);
	i2c_wait();
	NT72668_SET_GPIO_LOW(GPIO_SCL);
	i2c_wait();
	NT72668_SET_GPIO_HIGH(GPIO_SCL);
	i2c_wait();
	NT72668_SET_GPIO_LOW(GPIO_SCL);
	i2c_wait();
	NT72668_SET_GPIO_HIGH(GPIO_SCL);
	i2c_wait();
	NT72668_SET_GPIO_LOW(GPIO_SCL);
	i2c_wait();
	NT72668_SET_GPIO_HIGH(GPIO_SCL);
	i2c_wait();
	NT72668_SET_GPIO_LOW(GPIO_SCL);
	i2c_wait();
	NT72668_SET_GPIO_HIGH(GPIO_SCL);
	i2c_wait();
	NT72668_SET_GPIO_LOW(GPIO_SCL);

	/*
	i2c_start();
	rc = i2c_write_byte ((addr << 1) | 0);
	i2c_stop();
	*/
	return (rc ? 1 : 0);
}

#endif
#endif

int do_nvt_i2c_write(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
    int ret = 0;
    unsigned char device_adrress_write = 0;
    unsigned int address = 0;
    int address_len = 0;
    int data_len = 0;
    unsigned char *buffer = NULL;

    if (argc < 5)
    {
        printf("need 5 parameters!\n");
        ret = -1;
        goto end;
    }
    device_adrress_write = simple_strtoul(argv[1],NULL,16);
    address = simple_strtoul(argv[2],NULL,16);
    address_len = simple_strtoul(argv[3],NULL,16);
    buffer = (unsigned char*)simple_strtoul(argv[4],NULL,16);
    data_len = simple_strtoul(argv[5],NULL,16);

    ret = i2c_write(device_adrress_write, address, address_len, buffer, data_len);
    if (ret == -1)
    {
        printf("write i2c fail!\n");
    }
    else if (ret == 0)
    {
        printf("write i2c success!\n");
    }

end:
    return ret;
}

int do_nvt_i2c_read(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
    int ret = 0;
    unsigned char device_adrress_read = 0;
    unsigned int adrress = 0;
    int address_len = 0;
    int data_len = 0;
    unsigned char *buffer = NULL;

    if (argc < 5)
    {
        printf("need 5 parameters!\n");
        ret = -1;
        goto end;
    }
    device_adrress_read = simple_strtoul(argv[1],NULL,16);
    adrress = simple_strtoul(argv[2],NULL,16);
    address_len = simple_strtoul(argv[3],NULL,16);
    buffer = (unsigned char*)simple_strtoul(argv[4],NULL,16);
    data_len = simple_strtoul(argv[5],NULL,16);

    ret = i2c_read(device_adrress_read, adrress, address_len, buffer, data_len);
    if (ret == -1)
    {
        printf("read i2c fail!\n");
    }
    else if (ret == 0)
    {
        printf("read i2c success!\n");
    }

end:
    return ret;
}

U_BOOT_CMD(
		i2cwrite, 6, 0,	do_nvt_i2c_write,
		"i2cwrite    - write data to i2c device\n",
		"[device slave address] [address] [lengh of address] [buffer] [lengh of data]\n"
		"[device slave address]  - slave address of device, like 0x20\n"
		"[address]               - specify address of device to be written to, like 0x00\n"
		"[lengh of address]      - number of bytes that [address] consist of. if [address] is 0x00, then [lengh of address] is 0x1\n"
		"[buffer]                - start address of data in dram which would be written to device, like 0x78000000\n"
		"[lengh of data]         - bytes number of data which will be written to device.\n"
	  );

U_BOOT_CMD(
		i2cread, 6,	0,	do_nvt_i2c_read,
		"i2cread    - read data from i2c device\n",
        "[device slave address] [address] [lengh of address] [buffer] [lengh of data]\n"
        "[device slave address]  - slave address of device, like 0x21\n"
        "[address]               - specify address of device to be read from, like 0x04\n"
        "[lengh of address]      - number of bytes that [address] consist of. if [address] is 0x04, then [lengh of address] is 0x1\n"
        "[buffer]                - start address in dram which save data read from device, like 0x78000000\n"
        "[lengh of data]         - bytes number of the data which will be read from device.\n"
	  );

#if 0

//#if defined(CONFIG_SOFT_I2C_GPIO_SCL)
//# include <asm/gpio.h>

# ifndef I2C_GPIO_SYNC
#  define I2C_GPIO_SYNC
# endif

# ifndef I2C_ACTIVE
#  define I2C_ACTIVE do { } while (0)
# endif

# ifndef I2C_TRISTATE
#  define I2C_TRISTATE do { } while (0)
# endif

# ifndef I2C_READ
#  define I2C_READ NT72668_GET_GPIO_LEVEL(GPIO_SDA) //gpio_get_value(CONFIG_SOFT_I2C_GPIO_SDA)
# endif

# ifndef I2C_SDA
#  define I2C_SDA(bit) \
	do { \
		if(bit) \
			NT72668_SET_GPIO_INPUT(GPIO_SDA); \
		else \
		{ \
			NT72668_SET_GPIO_OUTPUT(GPIO_SDA); \
			NT72668_SET_GPIO_LOW(GPIO_SDA); \
		} \
		I2C_GPIO_SYNC; \
	} while (0)
# endif

# ifndef I2C_SCL
#  define I2C_SCL(bit) \
	do { \
		if(bit) \
		{ \
			NT72668_SET_GPIO_OUTPUT(GPIO_SCL); \
			NT72668_SET_GPIO_HIGH(GPIO_SCL); \
		} \
		else \
		{ \
			NT72668_SET_GPIO_OUTPUT(GPIO_SCL); \
			NT72668_SET_GPIO_LOW(GPIO_SCL); \
		} \
		I2C_GPIO_SYNC; \
	} while (0)
# endif

# ifndef I2C_DELAY
#  define I2C_DELAY udelay(2.5)	/* 1/4 I2C clock duration */
# endif

//#endif

#define	DEBUG_I2C

#ifdef DEBUG_I2C
DECLARE_GLOBAL_DATA_PTR;
#endif

/*-----------------------------------------------------------------------
 * Definitions
 */

#define RETRIES		0

#define I2C_ACK		0		/* PD_SDA level to ack a byte */
#define I2C_NOACK	1		/* PD_SDA level to noack a byte */


#ifdef DEBUG_I2C
#define PRINTD(fmt,args...)	do {	\
		printf (fmt ,##args);	\
	} while (0)
#else
#define PRINTD(fmt,args...)
#endif

/*-----------------------------------------------------------------------
 * Local functions
 */
#if !defined(CONFIG_SYS_I2C_INIT_BOARD)
static void  send_reset	(void);
#endif
static void  send_start	(void);
static void  send_stop	(void);
static void  send_ack	(int);
static int   write_byte	(uchar byte);
static uchar read_byte	(int);

#if !defined(CONFIG_SYS_I2C_INIT_BOARD)
/*-----------------------------------------------------------------------
 * Send a reset sequence consisting of 9 clocks with the data signal high
 * to clock any confused device back into an idle state.  Also send a
 * <stop> at the end of the sequence for belts & suspenders.
 */
static void send_reset(void)
{
//	I2C_SOFT_DECLARATIONS	/* intentional without ';' */
	int j;

	I2C_SCL(1);
	I2C_SDA(1);
#ifdef	I2C_INIT
	I2C_INIT;
#endif
	I2C_TRISTATE;
	for(j = 0; j < 9; j++) {
		I2C_SCL(0);
		I2C_DELAY;
		I2C_DELAY;
		I2C_SCL(1);
		I2C_DELAY;
		I2C_DELAY;
	}
	send_stop();
	I2C_TRISTATE;
}
#endif

/*-----------------------------------------------------------------------
 * START: High -> Low on SDA while SCL is High
 */
static void send_start(void)
{
//	I2C_SOFT_DECLARATIONS	/* intentional without ';' */

	I2C_DELAY;
	I2C_SDA(1);
	I2C_ACTIVE;
	I2C_DELAY;
	I2C_SCL(1);
	I2C_DELAY;
	I2C_SDA(0);
	I2C_DELAY;
}

/*-----------------------------------------------------------------------
 * STOP: Low -> High on SDA while SCL is High
 */
static void send_stop(void)
{
//	I2C_SOFT_DECLARATIONS	/* intentional without ';' */

	I2C_SCL(0);
	I2C_DELAY;
	I2C_SDA(0);
	I2C_ACTIVE;
	I2C_DELAY;
	I2C_SCL(1);
	I2C_DELAY;
	I2C_SDA(1);
	I2C_DELAY;
	I2C_TRISTATE;
}

/*-----------------------------------------------------------------------
 * ack should be I2C_ACK or I2C_NOACK
 */
static void send_ack(int ack)
{
//	I2C_SOFT_DECLARATIONS	/* intentional without ';' */

	I2C_SCL(0);
	I2C_DELAY;
	I2C_ACTIVE;
	I2C_SDA(ack);
	I2C_DELAY;
	I2C_SCL(1);
	I2C_DELAY;
	I2C_DELAY;
	I2C_SCL(0);
	I2C_DELAY;
}

/*-----------------------------------------------------------------------
 * Send 8 bits and look for an acknowledgement.
 */
static int write_byte(uchar data)
{
//	I2C_SOFT_DECLARATIONS	/* intentional without ';' */
	int j;
	int nack;

	I2C_ACTIVE;
	for(j = 0; j < 8; j++) {
		I2C_SCL(0);
		I2C_DELAY;
		I2C_SDA(data & 0x80);
		I2C_DELAY;
		I2C_SCL(1);
		I2C_DELAY;
		I2C_DELAY;

		data <<= 1;
	}

	/*
	 * Look for an <ACK>(negative logic) and return it.
	 */
	I2C_SCL(0);
	I2C_DELAY;
	I2C_SDA(1);
	I2C_TRISTATE;
	I2C_DELAY;
	I2C_SCL(1);
	I2C_DELAY;
	I2C_DELAY;
	nack = I2C_READ;
	I2C_SCL(0);
	I2C_DELAY;
	I2C_ACTIVE;

	return(nack);	/* not a nack is an ack */
}

/*-----------------------------------------------------------------------
 * if ack == I2C_ACK, ACK the byte so can continue reading, else
 * send I2C_NOACK to end the read.
 */
static uchar read_byte(int ack)
{
//	I2C_SOFT_DECLARATIONS	/* intentional without ';' */
	int  data;
	int  j;

	/*
	 * Read 8 bits, MSB first.
	 */
	I2C_TRISTATE;
	I2C_SDA(1);
	data = 0;
	for(j = 0; j < 8; j++) {
		I2C_SCL(0);
		I2C_DELAY;
		I2C_SCL(1);
		I2C_DELAY;
		data <<= 1;
		data |= I2C_READ;
		I2C_DELAY;
	}
	send_ack(ack);

	return(data);
}

/*=====================================================================*/
/*                         Public Functions                            */
/*=====================================================================*/

/*-----------------------------------------------------------------------
 * Initialization
 */
void i2c_init (int speed, int slaveaddr)
{
#if defined(CONFIG_SYS_I2C_INIT_BOARD)
	/* call board specific i2c bus reset routine before accessing the   */
	/* environment, which might be in a chip on that bus. For details   */
	/* about this problem see doc/I2C_Edge_Conditions.                  */
	i2c_init_board();
#else
	/*
	 * WARNING: Do NOT save speed in a static variable: if the
	 * I2C routines are called before RAM is initialized (to read
	 * the DIMM SPD, for instance), RAM won't be usable and your
	 * system will crash.
	 */
	send_reset ();
#endif
}

/*-----------------------------------------------------------------------
 * Probe to see if a chip is present.  Also good for checking for the
 * completion of EEPROM writes since the chip stops responding until
 * the write completes (typically 10mSec).
 */
int i2c_probe(uchar addr)
{
	int rc;

	/*
	 * perform 1 byte write transaction with just address byte
	 * (fake write)
	 */
	send_start();
	rc = write_byte ((addr << 1) | 0);
	send_stop();

	return (rc ? 1 : 0);
}

/*-----------------------------------------------------------------------
 * Read bytes
 */
int  i2c_read(uchar chip, uint addr, int alen, uchar *buffer, int len)
{
	int shift;
	PRINTD("i2c_read: chip %02X addr %02X alen %d buffer %p len %d\n",
		chip, addr, alen, buffer, len);

	/*
	 * Do the addressing portion of a write cycle to set the
	 * chip's address pointer.  If the address length is zero,
	 * don't do the normal write cycle to set the address pointer,
	 * there is no address pointer in this chip.
	 */
	send_start();
	if(alen > 0) {
		//if(write_byte(chip << 1)) {	/* write cycle */
		if(write_byte(chip&0xfe)) {
			send_stop();
			PRINTD("i2c_read, no chip responded %02X\n", chip);
			return(1);
		}
		shift = (alen-1) * 8;
		while(alen-- > 0) {
			if(write_byte(addr >> shift)) {
				PRINTD("i2c_read, address not <ACK>ed\n");
				return(1);
			}
			shift -= 8;
		}

		/* Some I2C chips need a stop/start sequence here,
		 * other chips don't work with a full stop and need
		 * only a start.  Default behaviour is to send the
		 * stop/start sequence.
		 */
#ifdef CONFIG_SOFT_I2C_READ_REPEATED_START
		send_start();
#else
		send_stop();
		send_start();
#endif
	}
	/*
	 * Send the chip address again, this time for a read cycle.
	 * Then read the data.  On the last byte, we do a NACK instead
	 * of an ACK(len == 0) to terminate the read.
	 */
	//write_byte((chip << 1) | 1);	/* read cycle */
	write_byte(chip|1);
	while(len-- > 0) {
		*buffer++ = read_byte(len == 0);
	}
	send_stop();
	return(0);
}

/*-----------------------------------------------------------------------
 * Write bytes
 */
int  i2c_write(uchar chip, uint addr, int alen, uchar *buffer, int len)
{
	int shift, failures = 0;

	PRINTD("i2c_write: chip %02X addr %02X alen %d buffer %p len %d\n",
		chip, addr, alen, buffer, len);

	send_start();
	//if(write_byte(chip << 1)) {	/* write cycle */
	if(write_byte(chip)){ 
		send_stop();
		PRINTD("i2c_write, no chip responded %02X\n", chip);
		return(1);
	}
	shift = (alen-1) * 8;
	while(alen-- > 0) {
		if(write_byte(addr >> shift)) {
			PRINTD("i2c_write, address not <ACK>ed\n");
			return(1);
		}
		shift -= 8;
	}

	while(len-- > 0) {
		if(write_byte(*buffer++)) {
			failures++;
		}
	}
	send_stop();
	return(failures);
}

#endif
