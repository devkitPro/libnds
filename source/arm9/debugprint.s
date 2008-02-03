@--------------------------------------------
@ no$gba debug output function
@--------------------------------------------
 
.global nocashMessage
 
.text
.thumb
.align
 
.thumb_func
@--------------------------------------------
nocashMessage:  @ params = { string }
@--------------------------------------------
 
@ max string length == 120 bytes
 
@----------------------------
@ copy string into buffer
@----------------------------
 
        ldr     r1,=1f          @ get buffer address
        mov     r2, #30         @ max length = 120
3:      ldrb    r3, [r0,#0]     @ load character
        strb    r3, [r1,#0]     @ store character
        cmp     r3, #0          @ character == NULL?
        beq     3f              @ yes, send message
        ldrb    r3, [r0,#1]     @ load character
        strb    r3, [r1,#1]     @ store character
        cmp     r3, #0          @ character == NULL?
        beq     3f              @ yes, send message
        ldrb    r3, [r0,#2]     @ load character
        strb    r3, [r1,#2]     @ store character
        cmp     r3, #0          @ character == NULL?
        beq     3f              @ yes, send message
        ldrb    r3, [r0,#3]     @ load character
        strb    r3, [r1,#3]     @ store character
        cmp     r3, #0          @ character == NULL?
        beq     3f              @ yes, send message
        add     r0, #4          @ increment read position
        add     r1, #4          @ increment write position
        sub     r2, #1          @ decrement word counter
        bne     3b              @ loop if < 120 characters
 
@---------------------------
@ send message to no$
@---------------------------
 
3:      mov     r12,r12 @ first ID
        b       2f      @ skip the text section
        .hword  0x6464  @ second ID
        .hword  0       @ flags
1:
        .space  120     @ data
2:      bx      lr      @ exit
 
@--------------------------------------------
.pool 