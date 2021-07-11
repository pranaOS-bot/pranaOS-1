/*
 * Copyright (c) 2021, krishpranav
 *
 * SPDX-License-Identifier: BSD-2-Clause
*/


#pragma once

// includes
#include <libutils/Prelude.h>
#include "system/Streams.h"

namespace Arch::x86_32
{

#define GDT_ENTRY_COUNT 6

#define GDT_PRESENT 0b10010000     
#define GDT_TSS_PRESENT 0b10000000 
#define GDT_USER 0b01100000        
#define GDT_EXECUTABLE 0b00001000  
#define GDT_READWRITE 0b00000010   
#define GDT_ACCESSED 0b00000001

#define GDT_FLAGS 0b1100
#define TSS_FLAGS 0b0000

struct PACKED TSS
{
    uint32_t prev_tss;
    uint32_t esp0;
    uint32_t ss0;
    uint32_t esp1;
    uint32_t ss1;
    uint32_t esp2;
    uint32_t ss2;
    uint32_t cr3;
    uint32_t eip;
    uint32_t eflags;
    uint32_t eax;
    uint32_t ecx;
    uint32_t edx;
    uint32_t ebx;
    uint32_t esp;
    uint32_t ebp;
    uint32_t esi;
    uint32_t edi;
    uint32_t es;
    uint32_t cs;
    uint32_t ss;
    uint32_t ds;
    uint32_t fs;
    uint32_t gs;
    uint32_t ldt;
    uint16_t trap;
    uint16_t iomap_base;
};
