/*
 *	SWI replacements for the slow libgcc division functions.
 *
 *	$Revision: 1.2 $
 *
 */
 
	.thumb
	.align
	.thumb_func
	.global	   __divsi3	       /* Signed integer division. */
	.thumb_set __udivsi3, __divsi3 /* Unsigned integer division. */
__divsi3:
	swi 9
	bx lr
 
	.thumb
	.align
	.thumb_func
	.global	   __modsi3	       /* Signed integer modulus. */
	.thumb_set __umodsi3, __modsi3 /* Unsigned integer modulus. */
__modsi3:
	swi 9
	mov r0,	r1
	bx lr
