/*
            Copyright Oliver Kowalke 2009.
   Distributed under the Boost Software License, Version 1.0.
      (See accompanying file LICENSE_1_0.txt or copy at
          http://www.boost.org/LICENSE_1_0.txt)
*/

/********************************************************************
 *                                                                  *
 *  --------------------------------------------------------------  *
 *  |    0    |    1    |    2    |    3    |    4     |    5    |  *
 *  --------------------------------------------------------------  *
 *  |   0x0   |   0x4   |   0x8   |   0xc   |   0x10   |   0x14  |  *
 *  --------------------------------------------------------------  *
 *  |   EDI   |   ESI   |   EBX   |   EBP   |   ESP    |   EIP   |  *
 *  --------------------------------------------------------------  *
 *  --------------------------------------------------------------  *
 *  |    6    |    7    |                                        |  *
 *  --------------------------------------------------------------  *
 *  |   0x18  |   0x1c  |                                        |  *
 *  --------------------------------------------------------------  *
 *  |    sp   |   size  |                                        |  *
 *  --------------------------------------------------------------  *
 *  --------------------------------------------------------------  *
 *  |    8    |    9    |                                        |  *
 *  --------------------------------------------------------------  *
 *  |   0x20  |   0x24  |                                        |  *
 *  --------------------------------------------------------------  *
 *  | fc_mxcsr|fc_x87_cw|                                        |  *
 *  --------------------------------------------------------------  *
 *                                                                  *
 * *****************************************************************/

.text
.globl _cps_context_new_from_sp
.align 2
_cps_context_new_from_sp:
    movl   0x4(%esp),      %eax         /* load 1. arg of cps_context_new_from_sp, pointer to context stack (base) */
    leal   -0x28(%eax),    %eax         /* reserve space for cps_context at top of context stack */

    /* shift address in EAX to lower 16 byte boundary */
    /* == pointer to cps_context and address of context stack */
    andl   $-16,           %eax

    movl   0x4(%esp),      %edx         /* load 1. arg of cps_context_new_from_sp, pointer to context stack (base) */
    movl   %edx,           0x18(%eax)   /* save address of stack pointer (base) in cps_context */
    movl   0x8(%esp),      %edx         /* load 2. arg of cps_context_new_from_sp, context stack size */
    movl   %edx,           0x1c(%eax)   /* save stack size in cps_context */
    movl   0xc(%esp),      %edx         /* load 3. arg of cps_context_new_from_sp, pointer to context function */
    movl   %edx,           0x14(%eax)   /* save address of context fcuntion in cps_context */

    stmxcsr  0x20(%eax)                 /* save MMX control and status word */
    fnstcw   0x24(%eax)                 /* save x87 control word */

    leal   -0x14(%eax),    %edx         /* reserve space for the last frame on context stack */
    movl   %edx,           0x10(%eax)   /* save address in EDX as stack pointer for context function */

    call   1f
1:  popl   %ecx                         /* address of label 1 */
    addl   $finish-1b,     %ecx         /* compute abs address of label finish */
    movl   %ecx, (%edx)                 /* save address of finish as return address for context function */
                                        /* entered after context function returns */

    ret

finish:
    /* ESP points to same address as ESP on entry of context function + 0x4 */
    xorl    %eax,  %eax
    movl    %eax,  (%esp)               /* exit code is zero */
    call   __exit                       /* exit application */
    hlt
