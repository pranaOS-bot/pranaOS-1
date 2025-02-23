/*
 * Copyright (c) 2021, Krisna Pranav
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef _KERNEL_PLATFORM_AARCH32_PMM_SETTINGS_H
#define _KERNEL_PLATFORM_AARCH32_PMM_SETTINGS_H

/* TODO: By default settings for Cortex-A15 are here */
#include <platform/aarch32/target/cortex-a15/memmap.h>

#define MEMORY_MAP_REGION (arm_memmap)
#define PMM_BLOCK_SIZE (1024)
#define PMM_BLOCK_SIZE_KB (1)
#define PMM_BLOCKS_PER_BYTE (8)

#endif /* _KERNEL_PLATFORM_AARCH32_PMM_SETTINGS_H */