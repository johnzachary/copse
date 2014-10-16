/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2013-2014, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the COPYING file in this distribution for license details.
 * ----------------------------------------------------------------------
 */

#ifndef COPSE_DETECT_H
#define COPSE_DETECT_H

#include <libcork/config.h>


/*-----------------------------------------------------------------------
 * Platform detection
 */

#if defined(__linux__)
#define CPS_CONFIG_BINARY_ELF    1
#define CPS_CONFIG_ABI_SYSV      1

#elif defined(__APPLE__)
#define CPS_CONFIG_BINARY_MACHO  1
#define CPS_CONFIG_ABI_SYSV      1

#endif


#if CPS_CONFIG_ABI_SYSV
#if CPS_CONFIG_BINARY_ELF

#if CORK_CONFIG_ARCH_X64
/* 64-bit x86 SysV ELF (eg Linux) */
#define CPS_HAVE_X86_64_SYSV_ELF_GAS  1
#define CPS_HAVE_CONTEXT  "x86_64_sysv_elf_gas.S"
#define CPS_STACK_GROWS_DOWN  1

#elif CORK_CONFIG_ARCH_X86
/* 32-bit x86 SysV ELF (eg Linux) */
#define CPS_HAVE_I386_SYSV_ELF_GAS  1
#define CPS_HAVE_CONTEXT  "i386_sysv_elf_gas.S"
#define CPS_STACK_GROWS_DOWN  1

#elif CORK_CONFIG_ARCH_PPC
/* 32-bit PowerPC SysV ELF (eg Linux) */
#define CPS_HAVE_PPC32_SYSV_ELF_GAS  1
#define CPS_HAVE_CONTEXT  "ppc32_sysv_elf_gas.S"
#define CPS_STACK_GROWS_DOWN  1

#elif defined(__tilegx__)
/* TileGX SysV ELF (eg Linux) */
#define CPS_HAVE_TILEGX_SYSV_ELF_GAS  1
#define CPS_HAVE_CONTEXT  "tilegx_sysv_elf_gas.S"
#define CPS_STACK_GROWS_DOWN  1

#endif /* CORK_CONFIG_ARCH */


#elif CPS_CONFIG_BINARY_MACHO

#if CORK_CONFIG_ARCH_X64
/* 64-bit SysV Mach-O (eg Mac OS X) */
#define CPS_HAVE_X86_64_SYSV_MACHO_GAS  1
#define CPS_HAVE_CONTEXT  "x86_64_sysv_macho_gas.S"
#define CPS_STACK_GROWS_DOWN  1

#elif CORK_CONFIG_ARCH_X86
/* 32-bit SysV Mach-O (eg Mac OS X) */
#define CPS_HAVE_I386_SYSV_MACHO_GAS  1
#define CPS_HAVE_CONTEXT  "i386_sysv_macho_gas.S"
#define CPS_STACK_GROWS_DOWN  1

#endif /* CORK_CONFIG_ARCH */


#endif /* CPS_CONFIG_BINARY */
#endif /* CPS_CONFIG_ABI_SYSV */


#if !defined(CPS_HAVE_CONTEXT)
#error "Don't know how to implement contexts on this platform"
#endif


#endif /* COPSE_DETECT_H */
