
#define OUTPUT_MAX_LENGTH (20<<20)	//we set 10 MB ... if kernel is 10MB , we need set bigger
#define INPUT_MAX_LENGTH (10<<20)	//we set 10 MB ... if kernel is 10MB , we need set bigger

 int  hwunzip(unsigned char *buf, int len,
		       int(*fill)(void*, unsigned int),
		       int(*flush)(void*, unsigned int),
		       unsigned char *out_buf,
		       int *pos,
		       void(*error)(char *x),
			int flush_input, int inval_output); 
#define decompress hwgunzip
#define unzip(INBUF, LEN, OUTBUF, FLUSH_INPUT, INVALID_OUTPUT)	\
	hwunzip(INBUF, LEN, NULL, NULL, OUTBUF, NULL,NULL, FLUSH_INPUT, INVALID_OUTPUT )

//#define CONFIG_ATF
//#ifdef CONFIG_ATF
#if 1
enum iff_src
{
	iff_NONE,
	iff_SPI,
	iff_NFC,
	iff_AES,
	iff_NAND_ARM,
};

enum iff_dest
{
	to_ZIP = 32,
	to_AES = 64,
	to_SHA = 128,
};

void nvt_gunzip_setting_config(int fifo_enable, enum iff_src src, int debug_mode, int bypass, int cpu_intr);
void nvt_gunzip_cvt_setting_config(	
				int fifo_enable,
				enum iff_src src,
				int debug_mode,
				int bypass,
				unsigned int timeout,
				int cpu_intr,
				int dummy_read,
				int dma_stop,
				int defealt_mode
				);
struct atf_setting
{
	unsigned int 	offset;
	unsigned int 	timeout;
	unsigned int 	src;
	unsigned int	length;
	unsigned int 	dest;
	int 		disable_done_intr;	//this is used for intr config
	int 		auto_reset;	//support for  version 1
//	int 		eanble_auto_reset;	//support for  version 1
	unsigned int 	auto_reset_size;
	
};

void fifo_atf_start(struct atf_setting config);
void fifo_atf_done(void);
void fifo_regdump(void);
void nvt_gunzip_prepare(unsigned char *buf_in, int len_in , unsigned char * buf_out, unsigned int len_out, int output_invalidate);
void nvt_gunzip_start(void);
int nvt_gunzip_wait(unsigned char* out_buf);
int nvt_gunzip_fifo_mode_support(void);
#endif
