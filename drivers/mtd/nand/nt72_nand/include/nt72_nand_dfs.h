#ifndef _NT72_NAND_DFS_H_
#define _NT72_NAND_DFS_H_

#include "nt72_nand.h"

#ifndef HAS_DEBUGFS
#define nt72_nand_dfs_init() do { } while (0)
#else
int nt72_nand_dfs_init(void);
#endif

#endif
