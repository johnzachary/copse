/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2013-2014, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the COPYING file in this distribution for license details.
 * ----------------------------------------------------------------------
 */

#ifndef COPSE_CONTEXT_H
#define COPSE_CONTEXT_H

#include <sys/param.h>

#include <libcork/config.h>
#include <libcork/core.h>


/*-----------------------------------------------------------------------
 * Platform detection
 */

#if defined(__linux__)
#define CPS_CONFIG_BINARY_ELF    1
#define CPS_CONFIG_ABI_SYSV      1

#elif defined(__APPLE__)
#define CPS_CONFIG_BINARY_MACHO  1
#define CPS_CONFIG_ABI_SYSV      1

#elif defined (__FreeBSD__)
#define CPS_CONFIG_BINARY_ELF    1
#define CPS_CONFIG_ABI_SYSV      1

#endif


/*-----------------------------------------------------------------------
 * Public context API
 */

struct cps_context;

typedef void
(*cps_context_f)(void *param);

void *
cps_context_jump(struct cps_context *from, struct cps_context const *to,
                 void *param, bool preserve_fpu);

struct cps_context *
cps_context_new(void *sp, size_t size, cps_context_f func);


/*-----------------------------------------------------------------------
 * Context implementations
 */

#if CPS_CONFIG_ABI_SYSV
#if CPS_CONFIG_BINARY_ELF

#if CORK_CONFIG_ARCH_X64

/* 64-bit x86 SysV ELF (eg Linux) */
#define CPS_HAVE_CONTEXT  "x86_64_sysv_elf_gas.S"
#define CPS_STACK_GROWS_DOWN  1

struct cps_stack {
    void  *sp;
    size_t  size;
};

struct cps_fp {
    uint32_t  fp_reg[2];
};

struct cps_context {
    uint64_t  gen_reg[8];
    struct cps_stack  stack;
    struct cps_fp  fp;
};

#elif CORK_CONFIG_ARCH_X86

/* 32-bit x86 SysV ELF (eg Linux) */
#define CPS_HAVE_CONTEXT  "i386_sysv_elf_gas.S"
#define CPS_STACK_GROWS_DOWN  1

struct cps_stack {
    void  *sp;
    size_t  size;
};

struct cps_fp {
    uint32_t  fp_reg[2];
};

struct cps_context {
    uint32_t  gen_reg[6];
    struct cps_stack  stack;
    struct cps_fp  fp;
};

#elif CORK_CONFIG_ARCH_PPC

/* 32-bit PowerPC SysV ELF (eg Linux) */
#define CPS_HAVE_CONTEXT  "ppc32_sysv_elf_gas.S"
#define CPS_STACK_GROWS_DOWN  1

struct cps_stack {
    void  *sp;
    size_t  size;
};

struct cps_fp {
    uint64_t  fp_reg[19];
};

struct cps_context {
    uint32_t  gen_reg[23];
    struct cps_stack  stack;
    struct cps_fp  fp;
};

#elif defined(__tilegx__)

/* TileGX SysV ELF (eg Linux) */
#define CPS_HAVE_CONTEXT  "tilegx_sysv_elf_gas.S"
#define CPS_STACK_GROWS_DOWN  1

struct cps_context {
    size_t  stack_size;
    uint64_t  reg[28];
};

#endif /* CORK_CONFIG_ARCH */

#elif CPS_CONFIG_BINARY_MACHO

#if CORK_CONFIG_ARCH_X64

/* 64-bit SysV Mach-O (eg Mac OS X) */
#define CPS_HAVE_CONTEXT  "x86_64_sysv_macho_gas.S"
#define CPS_STACK_GROWS_DOWN  1

struct cps_stack {
    void  *sp;
    size_t  size;
};

struct cps_fp {
    uint32_t  fp_reg[2];
};

struct cps_context {
    uint64_t  gen_reg[8];
    struct cps_stack  stack;
    struct cps_fp  fp;
};

#elif CORK_CONFIG_ARCH_X86

/* 32-bit SysV Mach-O (eg Mac OS X) */
#define CPS_HAVE_CONTEXT  "i386_sysv_macho_gas.S"
#define CPS_STACK_GROWS_DOWN  1

struct cps_stack {
    void  *sp;
    size_t  size;
};

struct cps_fp {
    uint32_t  fp_reg[2];
};

struct cps_context {
    uint32_t  gen_reg[6];
    struct cps_stack  stack;
    struct cps_fp  fp;
};

#endif /* CORK_CONFIG_ARCH */

#endif /* CPS_CONFIG_BINARY */
#endif /* CPS_CONFIG_ABI_SYSV */


#if !defined(CPS_HAVE_CONTEXT)
#error "Don't know how to implement contexts on this platform"
#endif


#endif /* COPSE_CONTEXT_H */
