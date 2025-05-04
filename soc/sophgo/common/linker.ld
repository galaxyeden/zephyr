/* Copyright (C) 2023 BeagleBoard.org Foundation
 * Copyright (C) 2023 S Prashanth
 * Copyright (C) 2025 Pete Johanson
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/arch/riscv/common/linker.ld>

SECTIONS
{
#ifdef CONFIG_OPENAMP_RSC_TABLE
	SECTION_PROLOGUE(.resource_table,, SUBALIGN(4))
	{
		KEEP(*(.resource_table*))
	} GROUP_LINK_IN(ROMABLE_REGION)
#endif
}
