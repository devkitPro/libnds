/*
 *	SWI replacements for the slow libgcc division functions.
 *
 *	$Revision: 1.3 $
 *
 */

	.thumb
	.align
	.thumb_func
	.global	   __divsi3	       /* Signed integer division. */
__divsi3:
	swi 9
	bx lr

	.thumb
	.align
	.thumb_func
	.global	   __udivsi3	       /* Unsigned integer division. */
__udivsi3:
	swi 9
        mov r0, r3
	bx lr

	.thumb
	.align
	.thumb_func
	.global	   __modsi3	       /* Signed integer modulus. */
__modsi3:
	swi 9
	mov r0,	r1
	bx lr
