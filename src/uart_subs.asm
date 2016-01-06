/************** subroutines to handle uart functions *****/

.equ	USB_PACKET_SIZE=6
.equ	UART_TX_BUFF_SIZE=4*USB_PACKET_SIZE
.equ	UART_RX_BUFF_SIZE=2*USB_PACKET_SIZE
.equ    CR=13
.equ    LF=10



        .dseg

UARTBuff:
UARTRxBuff:				.byte	(UART_RX_BUFF_SIZE)
UARTRxBuffEnd:
UARTTxBuff:				.byte	(UART_TX_BUFF_SIZE)
UARTTxBuffEnd:
UARTBuffEnd:
        
        .dseg
       
UARTRxBuffRdPtr:		.byte   (2)
UARTRxBuffWrPtr:		.byte   (2)
UARTTxBuffRdPtr:		.byte   (2)
UARTTxBuffWrPtr:		.byte   (2)

		.dseg
//CRITICAL ORDER. DO NOT RE-ARRANGE
UARTVarsStart:
UARTRxDipstick:			.byte   (2)
UARTTxDipstick:			.byte   (2)
UARTInsertQueryResponse:.byte   (1) 
UARTTxTriggerNeeded:    .byte   (1)
//CRITICAL ORDER ENDS
        

		.cseg



/*********************************************************/	
Initialize_UART:



	;use baud rate of 9600, 8 data bits, 1 stop bit and no parity.

	;clear UDRE bit. We don't need an immediate interrupt.
	ldi r17, (1 << TXC)
	out UCSRA, r17


	ldi r17, (1 << UCPOL) | (1 << UCSZ0) | (1 << UCSZ1) + (1 << URSEL)
	out UCSRC, r17

	;for a CPU clock of 16MHz and a baud rate of 9600, UBRR value of 103 is needed
	ldi r17, 103
	out UBRRL, r17

    
	;finally, enable RX,TX, and associated interrupts.
	ldi r17, (1 << TXEN) | (1 << RXEN) | (1 << TXCIE) | (1 << RXCIE)
	out UCSRB, r17

InitializeUARTBufsAndVars:

	ldi r26,	low(UARTRxBuffRdPtr)
	ldi r27,	high(UARTRxBuffRdPtr)

	ldi r16,	low(UARTRxBuff)
	ldi r17,	high(UARTRxBuff)

	st x+, r16	;UARTRxBuffRdPtr
	st x+, r17
	st x+, r16	;UARTRxBuffWrPtr
	st x+, r17

	ldi r16,	low(UARTTxBuff)
	ldi r17,	high(UARTTxBuff)

	st x+, r16	;UARTTxBuffRdPtr
	st x+, r17
	st x+, r16	;UARTTxBuffWrPtr
	st x+, r17

	

	clr r17
	;clear dipsticks, timestampCnt and  overflow count
	ldi r26,	low(UARTBuff)
	ldi r27,	high(UARTBuff)
	ldi r18,    (UARTBuffEnd - UARTBuff)

UARTInitBuffClearLoop1:

		st x+, r17
		dec r18
		brne	UARTInitBuffClearLoop1	

	; now init other flags
	ldi r26,	low(UARTVarsStart)
	ldi r27,	high(UARTVarsStart)


	st x+, r17	;UARTRxDipstick
	st x+, r17	;UARTRxDipstick
	st x+, r17	;UARTTxDipstick	
	st x+, r17	;UARTTxDipstick	
	st x+, r17	;UARTInsertQueryResponse	

	ldi r17, 1
	st x+, r17	;UARTTxTriggerNeeded

	ret


	 
/*********************************************************/	
RewindUARTRxRdWrPtr:

	;Make sure to use minimal set of registers.
	;currently using r17, r17, SREG

	;input pointer is in r27:r26 pair

	ldi r16, low(UARTRxBuffEnd)
	ldi r17, high(UARTRxBuffEnd)

	cp  r27, r17
	brlo	store_UartRx
	breq    compare_lower_UartRx
	jmp		rewind_UartRx


compare_lower_UartRx:
	;here upper byte is matching, we need to compare lower bytes


	cp 	r26, r16
	brlo	store_UartRx

	rewind_UartRx:

	ldi r26, low(UARTRxBuff)
	ldi r27, high(UARTRxBuff)



	store_UartRx:

	ret


/*********************************************************/	
RewindUARTTxRdWrPtr:

	;Make sure to use minimal set of registers.
	;currently using r16, r17, SREG
    ;must preserve all other registers

	;input pointer is in r31:r30 pair

	ldi r16, low(UARTTxBuffEnd)
	ldi r17, high(UARTTxBuffEnd)

	cp  r31, r17
	brlo	store_UartTx
	breq    compare_lower_UartTx
	jmp		rewind_UartTx


compare_lower_UartTx:
	;here upper byte is matching, we need to compare lower bytes


	cp 	r30, r16
	brlo	store_UartTx

	rewind_UartTx:

	ldi r30, low(UARTTxBuff)
	ldi r31, high(UARTTxBuff)



	store_UartTx:

	ret

/*********************************************************/	
USART_RXC:

	;This is ISR for UART RX completed event.
	;Dump the data from UART to fifo and increment dipstick.
	;Also need to add check for buffer overrun.


	push r16
	push r17
	push r26
	push r27


	in r17, SREG
	push r17


	;get current read pointer in r27:r26
	lds r26, (UARTRxBuffWrPtr)
	lds r27, (UARTRxBuffWrPtr+1)

	in r17, UDR

	st x+, r17

	call RewindUARTRxRdWrPtr


	sts (UARTRxBuffWrPtr), r26
	sts (UARTRxBuffWrPtr+1), r27


	;increment UartRxDipstick 
	lds r26, (UARTRxDipstick)
	lds r27, (UARTRxDipstick + 1)

    ;dummy read to increment dipstick
    ld  r17, x+

	sts (UARTRxDipstick), r26
	sts (UARTRxDipstick+1), r27

	;;;Add diagnostic code later to detect and handle buffer overrun.



	pop r17
	out SREG, r17

	pop r27
	pop r26
	pop r17
	pop r16

	reti


/*********************************************************/	
 USART_TXC:

 	;This is ISR for UART UDR empty event.

    ;Decrement UARTTx dipstick and load next word if dipstick is 
	;not zero

	push r16
	push r17
	push r30
	push r31


	in r17, SREG
	push r17

	;first increment UARTTxRdPtr
	;get current read pointer in r27:r26
	lds r30, (UARTTxBuffRdPtr)
	lds r31, (UARTTxBuffRdPtr+1)

	ld  r17, z+

	call RewindUARTTxRdWrPtr


	sts (UARTTxBuffRdPtr), r30
	sts (UARTTxBuffRdPtr+1), r31

	;also read next byte in buffer in case we 
	;need to transmit it

	ld  r16, z



	;now decrement dipstick
	lds r30, (UARTTxDipstick)
	lds r31, (UARTTxDipstick + 1)

    ;dummy read to decrement dipstick
    ld  r17, -z

	sts (UARTTxDipstick), r30
	sts (UARTTxDipstick+1), r31

	;check if dipstick is zero
	or r30, r31
	breq NoBytesToTransmit

	out UDR, r16

	;set UARTTxTriggerNeeded flag to 1
	ldi r17, 1
	sts UARTTxTriggerNeeded, r17

NoBytesToTransmit:


	pop r17
	out SREG, r17

	pop r31
	pop r30
	pop r17
	pop r16

	reti

	

/*********************************************************/	
UARTTxWaitForSpace:
	;input is in r17. This is the space needed in the buffer

	;Buffer emptiness = buffer size - dipstick
	ldi r16, (UART_TX_BUFF_SIZE)

	//for now assume that higher 8 bits of dipstick are zero.
	//need to fix this later.
	//NEEDS ATTENTION
	lds r18, (UARTTxDipstick)
	sub r16, r18
	cp  r16, r17
	brmi UARTTxWaitForSpace

	ret



/*********************************************************/	
UARTHandleHostEvent:

	;debug hack
    ;jmp UARTWaitForTxDipstickToFallToZero
	;This function checks to see if there is any message from Host.
	;In case a valid message is found, respond to it.

	;message packge must arrive completely before we can act
	lds r17, (UARTRxDipstick)
	cpi r17, (USB_PACKET_SIZE)
	brge 	FoundSomePacket
	
	jmp		NoPacketYet
FoundSomePacket:

	;in case we need to discard packet,
	;consume only one byte. This allows us to sync packets thjat are 
	;not aligned to packet boundaries.

	;
	ldi r16, (1)

	;now fall through


	;Now we have a valid packet. First 5 Bytes must be 'ESCWM' string.

	lds r26, (UARTRxBuffRdPtr)
	lds r27, (UARTRxBuffRdPtr + 1)

	ld r17, x+
	cpi r17, 'E'
	brne UARTRxDiscardPacket
	
	ld r17, x+
	cpi r17, 'S'
	brne UARTRxDiscardPacket

	ld r17, x+
	cpi r17, 'C'
	brne UARTRxDiscardPacket

	ld r17, x+
	cpi r17, 'W'
	brne UARTRxDiscardPacket

	ld r17, x+
	cpi r17, 'M'
	brne UARTRxDiscardPacket

	;last byte is message code

	ld r17, x+

	;act as per code.
	;For now; Just respond back with the same  string.

	;but before that just let the fifo be completely emptied.
	;so wait for Tx dipstick to fall to zero.

UARTWaitForTxDipstickToFallToZero:

	;caution, if TX is in bad state or interrupts are disabled,
	;this will create a dead lock

	lds r16, (UARTTxDipstick)
	lds r17, (UARTTxDipstick + 1)

	or r16, r17
	brne UARTWaitForTxDipstickToFallToZero

	;since dipstick is zero, need to set trigger needed flag.

	ldi r17, 1
	sts NoTriggerNeeded, r17

	;dump response to Tx buff
	;disable interrupt

	cli

    //Place initialization code here

	;now place response in UART tx buffer

	lds r30, (UARTTxBuffWrPtr)
	lds r31, (UARTTxBuffWrPtr + 1)

	ldi r17, 'M'
	st z+, r17

	;trigger character in r16
	mov r16, r17


	ldi r17, 'W'
	st z+, r17
	ldi r17, 'C'
	st z+, r17
	ldi r17, 'S'
	st z+, r17
	ldi r17, 'E'
	st z+, r17
	clr r17
	;ldi r17, '\n'
	st z+, r17

	push r16
	call RewindUARTTxRdWrPtr

	sts (UARTTxBuffWrPtr), r30
	sts (UARTTxBuffWrPtr + 1), r31


	;dipstick increment count in r17
	ldi r17, USB_PACKET_SIZE
	pop r16
	;trigger character in r16
	


	call UpdateUARTTxDipstick

	;since we consumed a valid packet, update dipstick by entire
	;packet size
	ldi r16, (USB_PACKET_SIZE)

	;now fall through

			

UARTRxDiscardPacket:

	;simply increment the pointer by packet size and reduce the dipstick
	cli

	lds r26, (UARTRxBuffRdPtr)
	lds r27, (UARTRxBuffRdPtr + 1)


	add r26, r16
	clr r17
	adc r27, r17

	push r16
	;rewind pointer
	call RewindUARTRxRdWrPtr

	pop r16

	sts (UARTRxBuffRdPtr), r26
	sts (UARTRxBuffRdPtr + 1), r27


	lds r17, (UARTRxDipstick)
	sub r17, r16
	sts (UARTRxDipstick), r17

	sei

	;jmp UARTWaitForTxDipstickToFallToZero


NoPacketYet:

	ret

/****************************************************************/
UpdateUARTTxDipstick:

	;r17 has dipstick increment and 
	;r16 has trigger character
	;cli
	lds  r10, (UARTTxDipstick)
	lds  r11, (UARTTxDipstick+1)

	add r10, r17
	clr r17
	adc r11, r17

    ;store dipstick
	sts  (UARTTxDipstick + 1), r11
	sts  (UARTTxDipstick), r10

	;now check if trigger needed
	lds  r17, UARTTxTriggerNeeded
	and  r17, r17
	breq NoTriggerNeeded

	out UDR, r16
	clr r17
	sts UARTTxTriggerNeeded, r17



NoTriggerNeeded:

	;re-enable interrupts
	;sei
	ret

/***************************************************************/
SendPackedtoUART:





	;This function sends the current sample  to UART.

	;wait till we have enough space in UARTTXBuf

	
	;push r30
	;push r31

	
	ldi	 r17, USB_PACKET_SIZE

    ;enable interrupts
	sei
	call UARTTxWaitForSpace

    ;disable interrupts
    
	;get pointers to UART buffer in z
	lds r30, (UARTTxBuffWrPtr)
	lds r31, (UARTTxBuffWrPtr+1)

	ldi r26, low(UartDigit1)
	ldi r27, high(UartDigit1)


    ldi r19, 4

    ;get trigger character in r18
    ld r18, x

    
    ;ldi r16, '0'
    ;add r18, r16


SendToUARTDigitLoop:
	    ;get digit
	    ld r17, x+
        ;convert to ascii code
        ;add r17, r16
	    st z+, r17
        
        dec r19
        brne    SendToUARTDigitLoop

    ;now write CR and LF
    ldi r17, CR
    st z+, r17
    
    ldi r17, LF
	st z+, r17


    ;rewind pointer 
	call RewindUARTTxRdWrPtr

	sts (UARTTxBuffWrPtr), r30
	sts (UARTTxBuffWrPtr+1), r31


	;dipstick increment count in r17
	ldi r17, USB_PACKET_SIZE
	mov r16, r18
	;trigger character in r16

	;pop stored values
	;pop  r31
	;pop  r30

	cli
	
	call UpdateUARTTxDipstick
    
    reti











