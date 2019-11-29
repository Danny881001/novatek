#ifndef _NT72_NAND_KERNEL_H_
#define _NT72_NAND_KERNEL_H_

#include "nt72_nand.h"

#ifdef USE_ECC_INV_TABLE
struct _nt72_nand_inv_map {
	unsigned int val: 1;
};

struct nt72_nand_inv_map {
	struct _nt72_nand_inv_map block[MAX_NR_BLOCKS];
};
#endif

#ifdef HAS_NPT
struct npt_part {
	char name[16]; /* identifier string */
	uint64_t size; /* partition size */
	uint64_t offset; /* offset within the master MTD space */
};

struct npt_table {
	u32 crc;
	u32 crc_xboot;
	struct npt_part parts[NPT_MAX_NR_PART];
};
#endif

#ifndef USE_ECC_INV_TABLE
#define nt72_nand_init_inv_table(info, npt) do { } while (0)
#else
void __init nt72_nand_init_inv_table(struct nt72_nand_info *info,
				     struct npt_table *npt) __init;
#endif

int __init nt72_nand_probe(struct platform_device *pdev) __init;
int nt72_nand_remove(struct platform_device *pdev);

#endif /*  _NT72_NAND_KERNEL_H_ */
