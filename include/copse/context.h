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

#include <libcork/config.h>
#include <libcork/core.h>

#include <copse/detect.h>


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

/* 64-bit x86 SysV ELF (eg Linux) */
#if CPS_HAVE_X86_64_SYSV_ELF_GAS
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
#endif


/* 32-bit x86 SysV ELF (eg Linux) */
#if CPS_HAVE_I386_SYSV_ELF_GAS
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
#endif


/* 32-bit PowerPC SysV ELF (eg Linux) */
#if CPS_HAVE_PPC32_SYSV_ELF_GAS
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
#endif


/* TileGX SysV ELF (eg Linux) */
#if CPS_HAVE_TILEGX_SYSV_ELF_GAS
struct cps_context {
    size_t  stack_size;
    uint64_t  reg[28];
};
#endif


/* 64-bit SysV Mach-O (eg Mac OS X) */
#if CPS_HAVE_X86_64_SYSV_MACHO_GAS
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
#endif


/* 32-bit SysV Mach-O (eg Mac OS X) */
#if CPS_HAVE_I386_SYSV_MACHO_GAS
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
#endif


#endif /* COPSE_CONTEXT_H */
