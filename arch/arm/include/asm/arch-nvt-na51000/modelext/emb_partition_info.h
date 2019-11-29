#ifndef _EMB_PARTITION_INFO_H
#define _EMB_PARTITION_INFO_H

#define EMB_PARTITION_INFO_VER 0x16072117 ///< YYYY/MM/DD HH

/**
    Partition Infomation
    This is common header used between firmware of uITRON, eCos, Linux, DSP
    so !!!!!! DO NOT modify it !!!!!!
*/

#define EMB_PARTITION_INFO_COUNT  16

enum {
	EMBTYPE_UNKNOWN  = 0x00,
	EMBTYPE_LOADER   = 0x01,  /* loader must always put in partition[0] */
	EMBTYPE_FDT      = 0x02,  /* modelext must always put in partition[1] */
	EMBTYPE_UITRON   = 0x03,
	EMBTYPE_ECOS     = 0x04,
	EMBTYPE_UBOOT    = 0x05,
	EMBTYPE_LINUX    = 0x06,
	EMBTYPE_DSP      = 0x07,
	EMBTYPE_PSTORE   = 0x08,
	EMBTYPE_FAT      = 0x09,
	EMBTYPE_EXFAT    = 0x0A,
	EMBTYPE_ROOTFS   = 0x0B,
	EMBTYPE_RAMFS    = 0x0C,
	EMBTYPE_UENV     = 0x0D, /* u-boot environment data */
	EMBTYPE_MBR      = 0x0E, /* for emmc partition, mbr always put in partition[0] instead of loader */
	EMBTYPE_ROOTFSL  = 0x0F, /* for emmc logical partition */
	EMBTYPE_SIZE,
};
/**
    customer defined data partition format
*/
enum {
	EMBTYPE_USER0	= 0x80,
	EMBTYPE_USER1	= 0x81,
	EMBTYPE_USER2	= 0x82,
	EMBTYPE_USER3	= 0x83,
	EMBTYPE_USER4	= 0x84,
	EMBTYPE_USER5	= 0x85,
	EMBTYPE_USER6	= 0x86,
	EMBTYPE_USER7	= 0x87,
	EMBTYPE_USR_SIZE,
};

#define EMBTYPE_TOTAL_SIZE	(EMBTYPE_SIZE + EMBTYPE_USR_SIZE - 0x80)
/* for reason of compatiable linux, we use original type to decalre */

typedef struct _EMB_PARTITION {
	unsigned short		EmbType;         /* EMBTYPE_ */
	unsigned short		OrderIdx;        /* Order index of the same EmbType based on '0' */
	unsigned long long	PartitionOffset; /* Phyical offset of partition */
	unsigned long long	PartitionSize;   /* Size of this partition */
	unsigned long long	ReversedSize;    /* Reserved size for bad block */
} EMB_PARTITION, *PEMB_PARTITION;

typedef struct _EMB_PARTITION_FDT_TRANSLATE_TABLE {
	char fdt_node_name[30];
	unsigned short emb_type;
} EMB_PARTITION_FDT_TRANSLATE_TABLE;
#endif
