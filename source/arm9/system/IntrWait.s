	.arch	armv5te
	.cpu	arm946e-s

	.text
	.arm

	.global swiIntrWait
	.type swiIntrWait STT_FUNC

swiIntrWait:
	@ savedIME = REG_IME, REG_IME = 1
	mov r2, #1
	mov r12, #0x4000000
	ldrb r3, [r12, #0x208]
	strb r2, [r12, #0x208]
	
	@ Wait for IRQ
	mov r2, #0
	mcr 15, 0, r2, c7, c0, 4
	
	@ REG_IME = savedIME
	strb r3, [r12, #0x208]
	
	@ return
	bx lr
