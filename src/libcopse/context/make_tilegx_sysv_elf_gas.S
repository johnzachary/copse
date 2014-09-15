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
.globl cps_context_new_from_sp
.type cps_context_new_from_sp,@function
.align 16
cps_context_new_from_sp:
    /* input:
     *   r0 = [top of] allocated stack memory
     *   r1 = size of stack
     *   r2 = pointer to context's run() function
     *
     * internal:
     *   r3 = cps_context instance, initial sp
     *   r4 = temp
     *   r5 = &ctx.stack_size, &ctx.sp, &ctx.tp
     *   r6 = &ctx.ip,         &ctx.lp
     *   r7 = &ctx.spr
     */

    /* reserve space for cps_context at top of context stack */
  { addli        r0, r0, -0x110 }

    /* shift address to lower 16 byte boundary */
    /* == pointer to cps_context and address of context stack */
  { andi         r0, r0, -16 }

    /* Fill in some initial context fields */

  { addi         r5, r0, 0x00                        // r5 := &ctx.stack_size
    mfspr        r4, INTERRUPT_CRITICAL_SECTION }    // r4 := spr

  { st           r5, r1                              // store stack_size
    addi         r6, r0, 0x08 }                      // r6 := &ctx.ip

  { st           r6, r2                              // store start() address
    addi         r7, r0, 0x10 }                      // r7 := &ctx.spr

  { addi         r5, r0, 0x18                        // r5 := &ctx.tp
    st           r7, r4 }                            // store spr

  { st           r5, tp }                            // store tp

    /* Create the initial stack frame for the context's start() function */
  { addi         r5, r0, 0x20                        // r5 := &ctx.sp
    addi         r4, r0, -16 }                       // r4 := initial sp
  { st           r5, r4 }

    /* The value of the lp register is where the start() function should return
     * to when it finishes. */
  { addi         r6, r0, 0x28
    moveli       r4, hw2_last(finish) }
  { shl16insli   r4, r4, hw1(finish) }
  { shl16insli   r4, r4, hw0(finish) }
  { st           r6, r4 }

    /* Return the address of the cps_context instance */
  { jrp          lr }

finish:
  { move         r0, zero
    jal          plt(exit) }                 /* exit application */
.size cps_context_new_from_sp,.-cps_context_new_from_sp
