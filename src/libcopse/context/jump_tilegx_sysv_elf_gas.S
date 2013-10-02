/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2013, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the COPYING file in this distribution for license details.
 * ----------------------------------------------------------------------
 */

/* u64[ 0]   0x00  stack size
 * u64[ 1]   0x08  ip
 * u64[ 2]   0x10  spr INTERRUPT_CRITICAL_SECTION
 * u64[ 3]   0x18  tp
 * u64[ 4]   0x20  sp
 * u64[ 5]   0x28  lr
 * u64[ 6]   0x30  r30
 * u64[ 7]   0x38  r31
 * u64[ 8]   0x40  r32
 * u64[ 9]   0x48  r33
 * u64[10]   0x50  r34
 * u64[11]   0x58  r35
 * u64[12]   0x60  r36
 * u64[13]   0x68  r37
 * u64[14]   0x70  r38
 * u64[15]   0x78  r39
 * u64[16]   0x80  r40
 * u64[17]   0x88  r41
 * u64[18]   0x90  r42
 * u64[19]   0x98  r43
 * u64[20]   0xa0  r44
 * u64[21]   0xa8  r45
 * u64[22]   0xb0  r46
 * u64[23]   0xb8  r47
 * u64[24]   0xc0  r48
 * u64[25]   0xc8  r49
 * u64[26]   0xd0  r50
 * u64[27]   0xd8  r51
 * u64[28]   0xe0  r52
 */

.text
.globl cps_context_jump
.type cps_context_jump,@function
.align 16
cps_context_jump:
    /* input:
     *   r0 = from context
     *   r1 = to context
     *   r2 = return param
     *   r3 = preserve_fpu (ignored on this platform)
     *
     * internal:
     *   r4 = address into ctx
     *   r5 = temp
     *   r6 = return address
     */

    /* store current state into "from" context */

  { addi   r4, r0, 0x08
    mfspr  r5, INTERRUPT_CRITICAL_SECTION }
  { st     r4, lr
    addi   r4, r4, 8 }
  { st     r4, r5
    addi   r4, r4, 8 }
  { st     r4, tp
    addi   r4, r4, 8 }
  { st     r4, sp
    addi   r4, r4, 8 }
  { st     r4, lr
    addi   r4, r4, 8 }
  { st     r4, r30
    addi   r4, r4, 8 }
  { st     r4, r31
    addi   r4, r4, 8 }
  { st     r4, r32
    addi   r4, r4, 8 }
  { st     r4, r33
    addi   r4, r4, 8 }
  { st     r4, r34
    addi   r4, r4, 8 }
  { st     r4, r35
    addi   r4, r4, 8 }
  { st     r4, r36
    addi   r4, r4, 8 }
  { st     r4, r37
    addi   r4, r4, 8 }
  { st     r4, r38
    addi   r4, r4, 8 }
  { st     r4, r39
    addi   r4, r4, 8 }
  { st     r4, r40
    addi   r4, r4, 8 }
  { st     r4, r41
    addi   r4, r4, 8 }
  { st     r4, r42
    addi   r4, r4, 8 }
  { st     r4, r43
    addi   r4, r4, 8 }
  { st     r4, r44
    addi   r4, r4, 8 }
  { st     r4, r45
    addi   r4, r4, 8 }
  { st     r4, r46
    addi   r4, r4, 8 }
  { st     r4, r47
    addi   r4, r4, 8 }
  { st     r4, r48
    addi   r4, r4, 8 }
  { st     r4, r49
    addi   r4, r4, 8 }
  { st     r4, r50
    addi   r4, r4, 8 }
  { st     r4, r51
    addi   r4, r4, 8 }
  { st     r4, r52
    addi   r4, r4, 8 }

    /* restore state from "to" context */

  { addi   r4, r1, 0x08 }
  { ld     r6, r4
    addi   r4, r4, 8 }
  { ld     r5, r4
    addi   r4, r4, 8 }
  { ld     tp, r4
    addi   r4, r4, 8 }
  { mtspr  INTERRUPT_CRITICAL_SECTION, r5 }
  { ld     sp, r4
    addi   r4, r4, 8 }
  { ld     lr, r4
    addi   r4, r4, 8 }
  { ld     r30, r4
    addi   r4, r4, 8 }
  { ld     r31, r4
    addi   r4, r4, 8 }
  { ld     r32, r4
    addi   r4, r4, 8 }
  { ld     r33, r4
    addi   r4, r4, 8 }
  { ld     r34, r4
    addi   r4, r4, 8 }
  { ld     r35, r4
    addi   r4, r4, 8 }
  { ld     r36, r4
    addi   r4, r4, 8 }
  { ld     r37, r4
    addi   r4, r4, 8 }
  { ld     r38, r4
    addi   r4, r4, 8 }
  { ld     r39, r4
    addi   r4, r4, 8 }
  { ld     r40, r4
    addi   r4, r4, 8 }
  { ld     r41, r4
    addi   r4, r4, 8 }
  { ld     r42, r4
    addi   r4, r4, 8 }
  { ld     r43, r4
    addi   r4, r4, 8 }
  { ld     r44, r4
    addi   r4, r4, 8 }
  { ld     r45, r4
    addi   r4, r4, 8 }
  { ld     r46, r4
    addi   r4, r4, 8 }
  { ld     r47, r4
    addi   r4, r4, 8 }
  { ld     r48, r4
    addi   r4, r4, 8 }
  { ld     r49, r4
    addi   r4, r4, 8 }
  { ld     r50, r4
    addi   r4, r4, 8 }
  { ld     r51, r4
    addi   r4, r4, 8 }
  { ld     r52, r4
    addi   r4, r4, 8 }

    /* jump into the "to" context */
  { move   r0, r2
    jrp    r6 }
.size cps_context_jump,.-cps_context_jump
