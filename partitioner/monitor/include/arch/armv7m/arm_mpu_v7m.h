/*
 * Copyright (c) 2018 Linaro Limited.
 * Copyright (c) 2018 Nordic Semiconductor ASA.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdint.h>
#ifndef _ASMLANGUAGE
#if defined(CONFIG_CPU_CORTEX_M4)
#include "core_cm4.h"
#endif
#if defined(CONFIG_CPU_CORTEX_M7)
#ifndef __FPU_PRESENT
#define __FPU_PRESENT 1
#endif
#include "core_cm7.h"
#endif 

#define ARM_MPU_REGION_SIZE_32B      ((uint8_t)0x04U) ///!< MPU Region Size 32 Bytes
#define ARM_MPU_REGION_SIZE_64B      ((uint8_t)0x05U) ///!< MPU Region Size 64 Bytes
#define ARM_MPU_REGION_SIZE_128B     ((uint8_t)0x06U) ///!< MPU Region Size 128 Bytes
#define ARM_MPU_REGION_SIZE_256B     ((uint8_t)0x07U) ///!< MPU Region Size 256 Bytes
#define ARM_MPU_REGION_SIZE_512B     ((uint8_t)0x08U) ///!< MPU Region Size 512 Bytes
#define ARM_MPU_REGION_SIZE_1KB      ((uint8_t)0x09U) ///!< MPU Region Size 1 KByte
#define ARM_MPU_REGION_SIZE_2KB      ((uint8_t)0x0AU) ///!< MPU Region Size 2 KBytes
#define ARM_MPU_REGION_SIZE_4KB      ((uint8_t)0x0BU) ///!< MPU Region Size 4 KBytes
#define ARM_MPU_REGION_SIZE_8KB      ((uint8_t)0x0CU) ///!< MPU Region Size 8 KBytes
#define ARM_MPU_REGION_SIZE_16KB     ((uint8_t)0x0DU) ///!< MPU Region Size 16 KBytes
#define ARM_MPU_REGION_SIZE_32KB     ((uint8_t)0x0EU) ///!< MPU Region Size 32 KBytes
#define ARM_MPU_REGION_SIZE_64KB     ((uint8_t)0x0FU) ///!< MPU Region Size 64 KBytes
#define ARM_MPU_REGION_SIZE_128KB    ((uint8_t)0x10U) ///!< MPU Region Size 128 KBytes
#define ARM_MPU_REGION_SIZE_256KB    ((uint8_t)0x11U) ///!< MPU Region Size 256 KBytes
#define ARM_MPU_REGION_SIZE_512KB    ((uint8_t)0x12U) ///!< MPU Region Size 512 KBytes
#define ARM_MPU_REGION_SIZE_1MB      ((uint8_t)0x13U) ///!< MPU Region Size 1 MByte
#define ARM_MPU_REGION_SIZE_2MB      ((uint8_t)0x14U) ///!< MPU Region Size 2 MBytes
#define ARM_MPU_REGION_SIZE_4MB      ((uint8_t)0x15U) ///!< MPU Region Size 4 MBytes
#define ARM_MPU_REGION_SIZE_8MB      ((uint8_t)0x16U) ///!< MPU Region Size 8 MBytes
#define ARM_MPU_REGION_SIZE_16MB     ((uint8_t)0x17U) ///!< MPU Region Size 16 MBytes
#define ARM_MPU_REGION_SIZE_32MB     ((uint8_t)0x18U) ///!< MPU Region Size 32 MBytes
#define ARM_MPU_REGION_SIZE_64MB     ((uint8_t)0x19U) ///!< MPU Region Size 64 MBytes
#define ARM_MPU_REGION_SIZE_128MB    ((uint8_t)0x1AU) ///!< MPU Region Size 128 MBytes
#define ARM_MPU_REGION_SIZE_256MB    ((uint8_t)0x1BU) ///!< MPU Region Size 256 MBytes
#define ARM_MPU_REGION_SIZE_512MB    ((uint8_t)0x1CU) ///!< MPU Region Size 512 MBytes
#define ARM_MPU_REGION_SIZE_1GB      ((uint8_t)0x1DU) ///!< MPU Region Size 1 GByte
#define ARM_MPU_REGION_SIZE_2GB      ((uint8_t)0x1EU) ///!< MPU Region Size 2 GBytes
#define ARM_MPU_REGION_SIZE_4GB      ((uint8_t)0x1FU) ///!< MPU Region Size 4 GBytes

#define MPU_CTRL_HFNMIENA_Pos               1U                                            /*!< MPU CTRL: HFNMIENA Position */
#define MPU_CTRL_HFNMIENA_Msk              (1UL << MPU_CTRL_HFNMIENA_Pos)                 /*!< MPU CTRL: HFNMIENA Mask */

#define MPU_CTRL_ENABLE_Pos                 0U                                            /*!< MPU CTRL: ENABLE Position */
#define MPU_CTRL_ENABLE_Msk                (1UL /*<< MPU_CTRL_ENABLE_Pos*/)               /*!< MPU CTRL: ENABLE Mask */

/* MPU Region Number Register Definitions */
#define MPU_RNR_REGION_Pos                  0U                                            /*!< MPU RNR: REGION Position */
#define MPU_RNR_REGION_Msk                 (0xFFUL /*<< MPU_RNR_REGION_Pos*/)             /*!< MPU RNR: REGION Mask */

/* MPU Region Base Address Register Definitions */
#define MPU_RBAR_ADDR_Pos                   5U                                            /*!< MPU RBAR: ADDR Position */
#define MPU_RBAR_ADDR_Msk                  (0x7FFFFFFUL << MPU_RBAR_ADDR_Pos)             /*!< MPU RBAR: ADDR Mask */

#define MPU_RBAR_VALID_Pos                  4U                                            /*!< MPU RBAR: VALID Position */
#define MPU_RBAR_VALID_Msk                 (1UL << MPU_RBAR_VALID_Pos)                    /*!< MPU RBAR: VALID Mask */

#define MPU_RBAR_REGION_Pos                 0U                                            /*!< MPU RBAR: REGION Position */
#define MPU_RBAR_REGION_Msk                (0xFUL /*<< MPU_RBAR_REGION_Pos*/)             /*!< MPU RBAR: REGION Mask */

/* MPU Region Attribute and Size Register Definitions */
#define MPU_RASR_ATTRS_Pos                 16U                                            /*!< MPU RASR: MPU Region Attribute field Position */
#define MPU_RASR_ATTRS_Msk                 (0xFFFFUL << MPU_RASR_ATTRS_Pos)               /*!< MPU RASR: MPU Region Attribute field Mask */

#define MPU_RASR_XN_Pos                    28U                                            /*!< MPU RASR: ATTRS.XN Position */
#define MPU_RASR_XN_Msk                    (1UL << MPU_RASR_XN_Pos)                       /*!< MPU RASR: ATTRS.XN Mask */

#define MPU_RASR_AP_Pos                    24U                                            /*!< MPU RASR: ATTRS.AP Position */
#define MPU_RASR_AP_Msk                    (0x7UL << MPU_RASR_AP_Pos)                     /*!< MPU RASR: ATTRS.AP Mask */

#define MPU_RASR_TEX_Pos                   19U                                            /*!< MPU RASR: ATTRS.TEX Position */
#define MPU_RASR_TEX_Msk                   (0x7UL << MPU_RASR_TEX_Pos)                    /*!< MPU RASR: ATTRS.TEX Mask */

#define MPU_RASR_S_Pos                     18U                                            /*!< MPU RASR: ATTRS.S Position */
#define MPU_RASR_S_Msk                     (1UL << MPU_RASR_S_Pos)                        /*!< MPU RASR: ATTRS.S Mask */

#define MPU_RASR_C_Pos                     17U                                            /*!< MPU RASR: ATTRS.C Position */
#define MPU_RASR_C_Msk                     (1UL << MPU_RASR_C_Pos)                        /*!< MPU RASR: ATTRS.C Mask */

#define MPU_RASR_B_Pos                     16U                                            /*!< MPU RASR: ATTRS.B Position */
#define MPU_RASR_B_Msk                     (1UL << MPU_RASR_B_Pos)                        /*!< MPU RASR: ATTRS.B Mask */

#define MPU_RASR_SRD_Pos                    8U                                            /*!< MPU RASR: Sub-Region Disable Position */
#define MPU_RASR_SRD_Msk                   (0xFFUL << MPU_RASR_SRD_Pos)                   /*!< MPU RASR: Sub-Region Disable Mask */

#define MPU_RASR_SIZE_Pos                   1U                                            /*!< MPU RASR: Region Size Field Position */
#define MPU_RASR_SIZE_Msk                  (0x1FUL << MPU_RASR_SIZE_Pos)                  /*!< MPU RASR: Region Size Field Mask */

#define MPU_RASR_ENABLE_Pos                 0U                                            /*!< MPU RASR: Region enable bit Position */
#define MPU_RASR_ENABLE_Msk                (1UL /*<< MPU_RASR_ENABLE_Pos*/)               /*!< MPU RASR: Region enable bit Disable Mask */

/* Convenience macros to represent the ARMv7-M-specific
 * configuration for memory access permission and
 * cache-ability attribution.
 */

/* Privileged No Access, Unprivileged No Access */
#define NO_ACCESS       0x0
#define NO_ACCESS_Msk   ((NO_ACCESS << MPU_RASR_AP_Pos) & MPU_RASR_AP_Msk)
/* Privileged No Access, Unprivileged No Access */
#define P_NA_U_NA       0x0
#define P_NA_U_NA_Msk   ((P_NA_U_NA << MPU_RASR_AP_Pos) & MPU_RASR_AP_Msk)
/* Privileged Read Write, Unprivileged No Access */
#define P_RW_U_NA       0x1
#define P_RW_U_NA_Msk   ((P_RW_U_NA << MPU_RASR_AP_Pos) & MPU_RASR_AP_Msk)
/* Privileged Read Write, Unprivileged Read Only */
#define P_RW_U_RO       0x2
#define P_RW_U_RO_Msk   ((P_RW_U_RO << MPU_RASR_AP_Pos) & MPU_RASR_AP_Msk)
/* Privileged Read Write, Unprivileged Read Write */
#define P_RW_U_RW       0x3U
#define P_RW_U_RW_Msk   ((P_RW_U_RW << MPU_RASR_AP_Pos) & MPU_RASR_AP_Msk)
/* Privileged Read Write, Unprivileged Read Write */
#define FULL_ACCESS     0x3
#define FULL_ACCESS_Msk ((FULL_ACCESS << MPU_RASR_AP_Pos) & MPU_RASR_AP_Msk)
/* Privileged Read Only, Unprivileged No Access */
#define P_RO_U_NA       0x5
#define P_RO_U_NA_Msk   ((P_RO_U_NA << MPU_RASR_AP_Pos) & MPU_RASR_AP_Msk)
/* Privileged Read Only, Unprivileged Read Only */
#define P_RO_U_RO       0x6
#define P_RO_U_RO_Msk   ((P_RO_U_RO << MPU_RASR_AP_Pos) & MPU_RASR_AP_Msk)
/* Privileged Read Only, Unprivileged Read Only */
#define RO              0x7
#define RO_Msk          ((RO << MPU_RASR_AP_Pos) & MPU_RASR_AP_Msk)

/* Attribute flag for not-allowing execution (eXecute Never) */
#define NOT_EXEC MPU_RASR_XN_Msk

/* The following definitions are for internal use in arm_mpu.h. */
#define STRONGLY_ORDERED_SHAREABLE      MPU_RASR_S_Msk
#define DEVICE_SHAREABLE                (MPU_RASR_B_Msk | MPU_RASR_S_Msk)
#define NORMAL_OUTER_INNER_WRITE_THROUGH_SHAREABLE \
		(MPU_RASR_C_Msk | MPU_RASR_S_Msk)
#define NORMAL_OUTER_INNER_WRITE_THROUGH_NON_SHAREABLE	MPU_RASR_C_Msk
#define NORMAL_OUTER_INNER_WRITE_BACK_SHAREABLE	\
		(MPU_RASR_C_Msk | MPU_RASR_B_Msk | MPU_RASR_S_Msk)
#define NORMAL_OUTER_INNER_WRITE_BACK_NON_SHAREABLE \
		(MPU_RASR_C_Msk | MPU_RASR_B_Msk)
#define NORMAL_OUTER_INNER_NON_CACHEABLE_SHAREABLE \
		((1 << MPU_RASR_TEX_Pos) | MPU_RASR_S_Msk)
#define NORMAL_OUTER_INNER_NON_CACHEABLE_NON_SHAREABLE \
		(1 << MPU_RASR_TEX_Pos)
#define NORMAL_OUTER_INNER_WRITE_BACK_WRITE_READ_ALLOCATE_SHAREABLE \
	((1 << MPU_RASR_TEX_Pos) |\
	 MPU_RASR_C_Msk | MPU_RASR_B_Msk | MPU_RASR_S_Msk)
#define NORMAL_OUTER_INNER_WRITE_BACK_WRITE_READ_ALLOCATE_NON_SHAREABLE \
	((1 << MPU_RASR_TEX_Pos) | MPU_RASR_C_Msk | MPU_RASR_B_Msk)
#define DEVICE_NON_SHAREABLE            (2 << MPU_RASR_TEX_Pos)

/* Bit-masks to disable sub-regions. */
#define SUB_REGION_0_DISABLED	((0x01 << MPU_RASR_SRD_Pos) & MPU_RASR_SRD_Msk)
#define SUB_REGION_1_DISABLED	((0x02 << MPU_RASR_SRD_Pos) & MPU_RASR_SRD_Msk)
#define SUB_REGION_2_DISABLED	((0x04 << MPU_RASR_SRD_Pos) & MPU_RASR_SRD_Msk)
#define SUB_REGION_3_DISABLED	((0x08 << MPU_RASR_SRD_Pos) & MPU_RASR_SRD_Msk)
#define SUB_REGION_4_DISABLED	((0x10 << MPU_RASR_SRD_Pos) & MPU_RASR_SRD_Msk)
#define SUB_REGION_5_DISABLED	((0x20 << MPU_RASR_SRD_Pos) & MPU_RASR_SRD_Msk)
#define SUB_REGION_6_DISABLED	((0x40 << MPU_RASR_SRD_Pos) & MPU_RASR_SRD_Msk)
#define SUB_REGION_7_DISABLED	((0x80 << MPU_RASR_SRD_Pos) & MPU_RASR_SRD_Msk)


#define REGION_SIZE(size) ((ARM_MPU_REGION_SIZE_ ## size \
	<< MPU_RASR_SIZE_Pos) & MPU_RASR_SIZE_Msk)

#define REGION_32B      REGION_SIZE(32B)
#define REGION_64B      REGION_SIZE(64B)
#define REGION_128B     REGION_SIZE(128B)
#define REGION_256B     REGION_SIZE(256B)
#define REGION_512B     REGION_SIZE(512B)
#define REGION_1K       REGION_SIZE(1KB)
#define REGION_2K       REGION_SIZE(2KB)
#define REGION_4K       REGION_SIZE(4KB)
#define REGION_8K       REGION_SIZE(8KB)
#define REGION_16K      REGION_SIZE(16KB)
#define REGION_32K      REGION_SIZE(32KB)
#define REGION_64K      REGION_SIZE(64KB)
#define REGION_128K     REGION_SIZE(128KB)
#define REGION_256K     REGION_SIZE(256KB)
#define REGION_512K     REGION_SIZE(512KB)
#define REGION_1M       REGION_SIZE(1MB)
#define REGION_2M       REGION_SIZE(2MB)
#define REGION_4M       REGION_SIZE(4MB)
#define REGION_8M       REGION_SIZE(8MB)
#define REGION_16M      REGION_SIZE(16MB)
#define REGION_32M      REGION_SIZE(32MB)
#define REGION_64M      REGION_SIZE(64MB)
#define REGION_128M     REGION_SIZE(128MB)
#define REGION_256M     REGION_SIZE(256MB)
#define REGION_512M     REGION_SIZE(512MB)
#define REGION_1G       REGION_SIZE(1GB)
#define REGION_2G       REGION_SIZE(2GB)
#define REGION_4G       REGION_SIZE(4GB)

/* Some helper defines for common regions */
#define REGION_RAM_ATTR(size) \
{ \
	(NORMAL_OUTER_INNER_WRITE_BACK_WRITE_READ_ALLOCATE_NON_SHAREABLE | \
	 MPU_RASR_XN_Msk | size | P_RW_U_NA_Msk) \
}
#define REGION_RAM_NOCACHE_ATTR(size) \
{ \
	(NORMAL_OUTER_INNER_NON_CACHEABLE_NON_SHAREABLE | \
	 MPU_RASR_XN_Msk | size | P_RW_U_NA_Msk) \
}
#if defined(CONFIG_MPU_ALLOW_FLASH_WRITE)
#define REGION_FLASH_ATTR(size) \
{ \
	(NORMAL_OUTER_INNER_WRITE_THROUGH_NON_SHAREABLE | size | \
		P_RW_U_RO_Msk) \
}
#else
#define REGION_FLASH_ATTR(size) \
{ \
	(NORMAL_OUTER_INNER_WRITE_THROUGH_NON_SHAREABLE | size | RO_Msk) \
}
#endif
#define REGION_PPB_ATTR(size) { (STRONGLY_ORDERED_SHAREABLE | size | \
		P_RW_U_NA_Msk) }
#define REGION_IO_ATTR(size) { (DEVICE_NON_SHAREABLE | size | P_RW_U_NA_Msk) }

struct arm_mpu_region_attr {
	/* Attributes belonging to RASR (including the encoded region size) */
	uint32_t rasr;
};

typedef struct arm_mpu_region_attr arm_mpu_region_attr_t;

/* Typedef for the k_mem_partition attribute */
typedef struct {
	uint32_t rasr_attr;
} k_mem_partition_attr_t;

/* Read-Write access permission attributes */
#define _K_MEM_PARTITION_P_NA_U_NA	(NO_ACCESS_Msk | NOT_EXEC)
#define _K_MEM_PARTITION_P_RW_U_RW	(P_RW_U_RW_Msk | NOT_EXEC)
#define _K_MEM_PARTITION_P_RW_U_RO	(P_RW_U_RO_Msk | NOT_EXEC)
#define _K_MEM_PARTITION_P_RW_U_NA	(P_RW_U_NA_Msk | NOT_EXEC)
#define _K_MEM_PARTITION_P_RO_U_RO	(P_RO_U_RO_Msk | NOT_EXEC)
#define _K_MEM_PARTITION_P_RO_U_NA	(P_RO_U_NA_Msk | NOT_EXEC)

/* Execution-allowed attributes */
#define _K_MEM_PARTITION_P_RWX_U_RWX (P_RW_U_RW_Msk)
#define _K_MEM_PARTITION_P_RWX_U_RX  (P_RW_U_RO_Msk)
#define _K_MEM_PARTITION_P_RX_U_RX   (P_RO_U_RO_Msk)

/* Kernel macros for memory attribution
 * (access permissions and cache-ability).
 *
 * The macros are to be stored in k_mem_partition_attr_t
 * objects. The format of k_mem_partition_attr_t is an
 * "1-1" mapping of the ARMv7-M MPU RASR attribute register
 * fields (excluding the <size> and <enable> bit-fields).
 */

/* Read-Write access permission attributes (default cache-ability) */
#define K_MEM_PARTITION_P_NA_U_NA	((k_mem_partition_attr_t) \
	{ _K_MEM_PARTITION_P_NA_U_NA | \
	  NORMAL_OUTER_INNER_WRITE_BACK_WRITE_READ_ALLOCATE_NON_SHAREABLE})
#define K_MEM_PARTITION_P_RW_U_RW	((k_mem_partition_attr_t) \
	{ _K_MEM_PARTITION_P_RW_U_RW | \
	  NORMAL_OUTER_INNER_WRITE_BACK_WRITE_READ_ALLOCATE_NON_SHAREABLE})
#define K_MEM_PARTITION_P_RW_U_RO	((k_mem_partition_attr_t) \
	{ _K_MEM_PARTITION_P_RW_U_RO | \
	  NORMAL_OUTER_INNER_WRITE_BACK_WRITE_READ_ALLOCATE_NON_SHAREABLE})
#define K_MEM_PARTITION_P_RW_U_NA	((k_mem_partition_attr_t) \
	{ _K_MEM_PARTITION_P_RW_U_NA | \
	  NORMAL_OUTER_INNER_WRITE_BACK_WRITE_READ_ALLOCATE_NON_SHAREABLE})
#define K_MEM_PARTITION_P_RO_U_RO	((k_mem_partition_attr_t) \
	{ _K_MEM_PARTITION_P_RO_U_RO | \
	  NORMAL_OUTER_INNER_WRITE_BACK_WRITE_READ_ALLOCATE_NON_SHAREABLE})
#define K_MEM_PARTITION_P_RO_U_NA	((k_mem_partition_attr_t) \
	{ _K_MEM_PARTITION_P_RO_U_NA | \
	  NORMAL_OUTER_INNER_WRITE_BACK_WRITE_READ_ALLOCATE_NON_SHAREABLE})

/* Execution-allowed attributes (default-cacheability) */
#define K_MEM_PARTITION_P_RWX_U_RWX	((k_mem_partition_attr_t) \
	{ _K_MEM_PARTITION_P_RWX_U_RWX | \
	  NORMAL_OUTER_INNER_WRITE_BACK_WRITE_READ_ALLOCATE_NON_SHAREABLE})
#define K_MEM_PARTITION_P_RWX_U_RX	((k_mem_partition_attr_t) \
	{ _K_MEM_PARTITION_P_RWX_U_RX | \
	  NORMAL_OUTER_INNER_WRITE_BACK_WRITE_READ_ALLOCATE_NON_SHAREABLE})
#define K_MEM_PARTITION_P_RX_U_RX	((k_mem_partition_attr_t) \
	{ _K_MEM_PARTITION_P_RX_U_RX | \
	  NORMAL_OUTER_INNER_WRITE_BACK_WRITE_READ_ALLOCATE_NON_SHAREABLE})

/*
 * @brief Evaluate Write-ability
 *
 * Evaluate whether the access permissions include write-ability.
 *
 * @param attr The k_mem_partition_attr_t object holding the
 *             MPU attributes to be checked against write-ability.
 */
#define K_MEM_PARTITION_IS_WRITABLE(attr) \
	({ \
		int __is_writable__; \
		switch (attr.rasr_attr & MPU_RASR_AP_Msk) { \
		case P_RW_U_RW_Msk: \
		case P_RW_U_RO_Msk: \
		case P_RW_U_NA_Msk: \
			__is_writable__ = 1; \
			break; \
		default: \
			__is_writable__ = 0; \
		} \
		__is_writable__; \
	})

/*
 * @brief Evaluate Execution allowance
 *
 * Evaluate whether the access permissions include execution.
 *
 * @param attr The k_mem_partition_attr_t object holding the
 *             MPU attributes to be checked against execution
 *             allowance.
 */
#define K_MEM_PARTITION_IS_EXECUTABLE(attr) \
	(!((attr.rasr_attr) & (NOT_EXEC)))

/* Attributes for no-cache enabling (share-ability is selected by default) */

#define K_MEM_PARTITION_P_NA_U_NA_NOCACHE ((k_mem_partition_attr_t) \
	{(_K_MEM_PARTITION_P_NA_U_NA \
	| NORMAL_OUTER_INNER_NON_CACHEABLE_SHAREABLE)})
#define K_MEM_PARTITION_P_RW_U_RW_NOCACHE ((k_mem_partition_attr_t) \
	{(_K_MEM_PARTITION_P_RW_U_RW \
	| NORMAL_OUTER_INNER_NON_CACHEABLE_SHAREABLE)})
#define K_MEM_PARTITION_P_RW_U_RO_NOCACHE ((k_mem_partition_attr_t) \
	{(_K_MEM_PARTITION_P_RW_U_RO \
	| NORMAL_OUTER_INNER_NON_CACHEABLE_SHAREABLE)})
#define K_MEM_PARTITION_P_RW_U_NA_NOCACHE ((k_mem_partition_attr_t) \
	{(_K_MEM_PARTITION_P_RW_U_NA \
	| NORMAL_OUTER_INNER_NON_CACHEABLE_SHAREABLE)})
#define K_MEM_PARTITION_P_RO_U_RO_NOCACHE ((k_mem_partition_attr_t) \
	{(_K_MEM_PARTITION_P_RO_U_RO \
	| NORMAL_OUTER_INNER_NON_CACHEABLE_SHAREABLE)})
#define K_MEM_PARTITION_P_RO_U_NA_NOCACHE ((k_mem_partition_attr_t) \
	{(_K_MEM_PARTITION_P_RO_U_NA \
	| NORMAL_OUTER_INNER_NON_CACHEABLE_SHAREABLE)})

#define K_MEM_PARTITION_P_RWX_U_RWX_NOCACHE ((k_mem_partition_attr_t) \
	{(_K_MEM_PARTITION_P_RWX_U_RWX \
	| NORMAL_OUTER_INNER_NON_CACHEABLE_SHAREABLE)})
#define K_MEM_PARTITION_P_RWX_U_RX_NOCACHE  ((k_mem_partition_attr_t) \
	{(_K_MEM_PARTITION_P_RWX_U_RX  \
	| NORMAL_OUTER_INNER_NON_CACHEABLE_SHAREABLE)})
#define K_MEM_PARTITION_P_RX_U_RX_NOCACHE   ((k_mem_partition_attr_t) \
	{(_K_MEM_PARTITION_P_RX_U_RX   \
	| NORMAL_OUTER_INNER_NON_CACHEABLE_SHAREABLE)})

#endif /* _ASMLANGUAGE */

#define _ARCH_MEM_PARTITION_ALIGN_CHECK(start, size) \
	BUILD_ASSERT(!(((size) & ((size) - 1))) && \
		(size) >= CONFIG_ARM_MPU_REGION_MIN_ALIGN_AND_SIZE && \
		!((uint32_t)(start) & ((size) - 1)), \
		"the size of the partition must be power of 2" \
		" and greater than or equal to the minimum MPU region size." \
		"start address of the partition must align with size.")
