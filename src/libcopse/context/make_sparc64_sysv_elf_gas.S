/*
            Copyright Martin Husemann 2013.
   Distributed under the Boost Software License, Version 1.0.
      (See accompanying file LICENSE_1_0.txt or copy at
          http://www.boost.org/LICENSE_1_0.txt)
*/

/*******************************************************************
 *                                                                 *
 *  -------------------------------------------------------------  *
 *  |  Offset (in 4 or 8 byte units) | Content                  |  *
 *  -------------------------------------------------------------  *
 *  | 0                              | %sp                      |  *
 *  -------------------------------------------------------------  *
 *  | 1                              | %pc                      |  *
 *  -------------------------------------------------------------  *
 *  | 2                              | %i7 (return address)     |  *
 *  -------------------------------------------------------------  *
 *  | 3                              | %g1                      |  *
 *  -------------------------------------------------------------  *
 *  | 4                              | %g2                      |  *
 *  -------------------------------------------------------------  *
 *  | 5                              | %g3                      |  *
 *  -------------------------------------------------------------  *
 *  | 6                              | %g6                      |  *
 *  -------------------------------------------------------------  *
 *  | 7                              | %g7                      |  *
 *  -------------------------------------------------------------  *
 *    The local and in registers are stored on the stack.          *
 *******************************************************************/

#define	OFF(N)	(8*(N))
#define	CCFSZ	176		// C Compiler Frame Size
#define	BIAS	(2048-1)	// Stack offset for 64 bit programs
#define	FC_SZ	448		// sizeof(cps_context)
#define	FC_STK	384		// offsetof(cps_context, fc_stack)
#define	FC_FPU	0		// offsetof(cps_context, fc_fp)
#define	FC_FSR	264		// offsetof(cps_context, fc_fp.fp_fsr)
#define	FC_FPRS	256		// offsetof(cps_context, fc_fp.fp_fprs)
#define	FC_GREG	320		// offsetof(cps_context, fc_greg)
#define	BLOCK_SIZE	64

	.register %g2,#ignore
	.register %g3,#ignore
	.register %g6,#ignore

.text
.globl cps_context_new_from_sp
.align 4
.type cps_context_new_from_sp,@function
// cps_context *
// cps_context_new_from_sp( void * sp, std::size_t size, void (* fn)( intptr_t) )
cps_context_new_from_sp:
    save %sp, -CCFSZ, %sp
    // %i0 initial stack pointer
    // %i1 stack size limit
    // %i2 function pointer for context start function

    sub %i0, FC_SZ, %i4		// allocate cps_context at on the new stack and keep pointer as return value
    andn %i4, BLOCK_SIZE-1, %i5	// force block ops usable alignement and keep pointer to cps_context in %i5

    stx %i0, [%i5+FC_STK+OFF(0)]	// save fs_stack.sp
    stx %i1, [%i5+FC_STK+OFF(1)]	// save fs_stack.size
    sub %i5, CCFSZ+BIAS, %o1	   	// leave space for one register window (and offset stack for 64bit)
    stx %o1, [%i5+FC_GREG+OFF(0)]	// save new stack pointer
    stx %i2, [%i5+FC_GREG+OFF(1)]	// save new %pc (function pointer)
    stx %g1, [%i5+FC_GREG+OFF(3)]
    stx %g2, [%i5+FC_GREG+OFF(4)]
    stx %g3, [%i5+FC_GREG+OFF(5)]
    stx %g6, [%i5+FC_GREG+OFF(6)]
    stx %g7, [%i5+FC_GREG+OFF(7)]

    // synthesize "return address": jump to finish
1:  rd %pc, %i4
    add %i4, finish-1b-8, %i4
    stx %i4, [%i5+FC_GREG+OFF(2)]

    ret
    restore %g0, %i5, %o0	// return cps_context

finish:
    mov %g0, %o0
    call _exit
     nop

.size cps_context_new_from_sp,.-cps_context_new_from_sp
