#ifndef _NT72_NAND_CONFIG_H_
#define _NT72_NAND_CONFIG_H_

#if defined(__UBOOT__)
#define IS_UBOOT
#elif defined(__XBOOT__)
#define IS_XBOOT
#elif defined(__NAND_CVT__)
#define IS_CVT
#else
#define IS_KERNEL
#endif

#ifdef IS_CVT
#include <string.h>
#include "arch_time.h"
#include "common.h"
#include "mmu.h"
#include "ntreg.h"
#include "ntirq.h"
#include "nt72668_acp.h"
#include "nt72668_inc.h"
#include "irq_test.h"
#endif

#if defined(IS_CVT) || defined(IS_XBOOT)
#include "nt72_nand_osless.h"
#endif

#ifdef IS_UBOOT
#include "nt72_nand_uboot.h"
#endif

#ifdef IS_XBOOT
#include <cache.h>
#include <common.h>
#include <boot1/disk.h>
#endif

#if defined(IS_UBOOT)
#define nfc_msg printf
#define nfc_warn printf
#define nfc_err printf
#define nfc_cont printf
#else
#define nfc_msg pr_info
#define nfc_warn pr_warning
#define nfc_err pr_err
#define nfc_cont pr_cont
#endif
#define nfc_ui nfc_msg

#ifdef CONFIG_NVT_NAND_FLASH_CONTROLLER_BBT
#define USE_BBT
/* #define USE_BBT_CHECKSUM */
#endif

#if defined(CONFIG_NVT_NAND_FLASH_CONTROLLER_SHARE_CI) || !defined(IS_KERNEL)
#define SHARE_GPIO_WITH_CI
#endif

#if defined(CONFIG_NVT_NAND_FLASH_CONTROLLER_QUIET)
#define QUIET_NFC
#endif

#ifdef CONFIG_NVT_NAND_FLASH_CONTROLLER_STBC_ECC_SWITCH
#define STBC_ECC_SWITCH
#endif

#ifdef CONFIG_NVT_NAND_FLASH_CONTROLLER_DEBUGFS
#define HAS_DEBUGFS
#endif

#ifdef CONFIG_NVT_NAND_FLASH_CONTROLLER_BY_PART_ECC_INV
#define USE_ECC_INV
#define USE_ECC_INV_TABLE
#define MAX_NR_BLOCKS 8192
#endif

#if defined(CONFIG_MTD_CMDLINE_PARTS)
#define USE_CMDLINE_PARTS
#elif defined(CONFIG_NVT_NAND_FLASH_CONTROLLER_NPT)
#define HAS_NPT
#define NPT_BASE_BLOCK		(16)
#define NPT_MAX_NR_PART		(32)
#define NPT_RESERVED_BLOCKS	(3)
#endif

#ifdef CONFIG_NVT_NAND_FLASH_CONTROLLER_MTD_DIRECT_DMA
#define MTD_COPY_FREE
/* #define MTD_COPY_FREE_READ_A */
/* #define MTD_COPY_FREE_WRITE_UA */
#endif

#ifdef CONFIG_MTD_NAND_TEST_SUITE
#define HAS_NTS
#endif

#if defined(IS_KERNEL)
#define USE_DT
#define USE_IO_REMAP
#endif

#ifndef CONFIG_NVT_NAND_FLASH_CONTROLLER_BITFLIP_THRESHOLD
#define MTD_BITFLIP_THRESHOLD 0
#else
#define MTD_BITFLIP_THRESHOLD \
	(CONFIG_NVT_NAND_FLASH_CONTROLLER_BITFLIP_THRESHOLD)
#endif


/* global configurations */
#if defined(IS_KERNEL)
#define USE_INTERRUPT
#define MAX_NR_SUBPAGE 8
#define READ_RETRY_COUNT 10
#define INTERRUPT_TIMEOUT (20 * HZ)     /* 20 seconds */
#define REG_POLLING_TIMEOUT (20 * HZ)   /* 20 seconds */
#elif defined(IS_UBOOT) || defined(IS_XBOOT)
#define MAX_NR_SUBPAGE 8
#define READ_RETRY_COUNT 10
#define INTERRUPT_TIMEOUT (5 * HZ)
#define REG_POLLING_TIMEOUT (5 * HZ)
#else
#define USE_INTERRUPT
#define MAX_NR_SUBPAGE 16
#define READ_RETRY_COUNT 0
#define INTERRUPT_TIMEOUT (50 * HZ)
#define REG_POLLING_TIMEOUT (50 * HZ)
#endif

/* extra checks */
#ifndef IS_XBOOT
#define EXTRA_WAIT_NFC_READY
#define EXTRA_CHECK_CACHE_ALIGNMENT
#endif

/* dump */
#ifndef IS_CVT
#define DUMP_REG_ON_ERROR
#ifndef IS_XBOOT
#define DUMP_ECC_ON_BITFLIP
/* #define DUMP_ECC_ON_BLANK */
#define DUMP_DATA_ON_ERROR
/* #define DUMP_CONTROL_REGS */
#endif
#endif

/* debug features */
#if !defined(IS_XBOOT) && !defined(IS_CVT)
#define ENABLE_CMD_HISTORY
#define CMD_HISTORY_SIZE 10
/*
#define ENABLE_BUS_SNIFFER
#define BUS_SNIFFER_SIZE 512
*/
#endif

#ifdef IS_CVT
/* #define ENABLE_BLANK_CHECK */
#endif

/* selftests */
#ifndef IS_XBOOT
#define SELFTEST_NAND_PARAMS
/*
#define SELFTEST_REG_POLLING_TIMEOUT
#define SELFTEST_FAKE_BITFLIP
*/
#endif

#if defined(IS_CVT) || defined(IS_UBOOT)
#define MAX_PAGE_SIZE 8192
#define MAX_OOB_SIZE 512
#define MAX_PAGE_PER_BLOCK 128
#define MAX_BLOCK_SIZE ((MAX_PAGE_SIZE + MAX_OOB_SIZE) * MAX_PAGE_PER_BLOCK)
#endif

/* #define LEGACY_SUPPORT */

#endif /* _NT72_NAND_CONFIG_H_ */
