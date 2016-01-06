
.EQU TIMER_TICKS_PER_SECOND=((16000000/64)/510)
.EQU NUM_SEGS_TO_DISPLAY=5

	.MACRO TURN_DISPLAY_OFF
	    ;turn off all 8 segments
		ldi r17, $00
		out PORTA, r17

		;also turn off all digits
		in  r17, PORTB
		ori r17, $F8
		out PORTB, r17
	.ENDMACRO


/************** subroutines to handle timer functions *****/


		.cseg



/*********************************************************/	
Initialize_Timers:

	;Timer/Counter0 is used as a trigger source for display switching .
	;with a 16MHz of CPU clock, using a prescaler value of 64
	;and phase correct PWM mode, PWM frequency = (16M/(64*510)
	;which is close to 490Hz. In this case, since we are using 
	;oveflow flag, value of OCR0 is really don't care

	;prescaler = 64
	;OC0 disconnected
	;Phase Correct PWM mode

	ldi r17, (1 << CS00) | (1 << CS01) | (1 << WGM00)
	//ldi r17, (1 << CS02) | (1 << WGM00)

	out	TCCR0, r17

	//ldi r17, (128 - 1)
	//out OCR0,	r17

	;ldi r17, (1 << TOV0) | (1 << OCF0) | (1 << TOV2) | (1 << OCF2) 
    ;clear all flags
    ldi r17, $FF
		
	out	TIFR, r17


	;enable Timer0 overflow interrupt
	ldi		r17, (1 << TOIE0)
	out  	TIMSK, r17


	clr r17
	sts DisplaySegCounter, r17


	;now initialize Timer 1. Timer 2 is used to generate a 160KHz clock.
    ;This is fed to ADC. Busy signal of ADC is anded with this clock and
    ;is	fed as external clock input.IPC1 is connected to BUSY of  ADC and 
    ;falling edge of BUSY is used to capture. This is ADC count
    
    ;use normal mode of operation
    clr r17
    out TCCR1A, r17
    
    out TCNT1H, r17
    out TCNT1L, r17
    
    
    ;enable Input capture noise cancler.
    ;Also configure for falling edge
    ;Clock on T1, falling edge
    
    ldi r17, (1 << ICNC1) | (0 << ICES1) | (1 << CS12) | (1 << CS11) | (1 << CS10)
    out TCCR1B, r17
    
    ;clear all pending interrupts
    in  r17, TIFR
    ori r17, (1 << ICF1) | (1 << OCF1A) | (1 << OCF1B) | (TOV1)
    out TIFR, r17
    
    ;enable input capture interrupt
    in  r17, TIMSK
    ori r17, (1 << TICIE1)
    out TIMSK,  r17
    
    
    ;setup timer 2 to generate a 160KHz sq wave on OC2
    ;we need a divider count of 100
    ;use a prescaler of 1. This means we need a coun of 
    ;100/1 = 125 in OCR2. Since OC2 toggle on comparison,
    ;count needs to be half
    ;use CTC mode
    
    
    ldi r17, (1 << CS20) | (1 << WGM21) | (1 << COM20)
    out TCCR2, r17
    
    ldi r17, (100/2 - 1)
    out OCR2, r17
    
    clr r17
    out TCNT2, r17
    
    out ASSR, r17
    
    ;clear pending interrupts
    in  r17, TIFR
    ori r17, (1 << OCF2) | (1 << TOV2)
    out TIFR, r17
    
    ;no need to enable any interrupts
    in  r17, TIMSK
    andi r17, ~((1 << OCIE2) | (1 << TOIE2))
    out TIMSK, r17
    
    
    
        
    
 	;clear display segments
	ldi r26, low(DisplaySeg1)
	ldi r27, high(DisplaySeg1)

	;turn off display segments
	ldi r17, $FF
	st x+, r17		//DisplaySeg1
	st x+, r17		//DisplaySeg2
	st x+, r17		//DisplaySeg3
	st x+, r17		//DisplaySeg4
	st x+, r17		//DisplaySeg5

	clr  r17
	st x+, r17		//DisplaySegCounter
	st x+, r17		//TimerOneSecondFlag
	st x+, r17      //TimerOneSecCount
	st x+, r17


	ret


/****************************************************************/
/**** ISR to implement display segment scanning *****************/
TIM0_OVF:
	push r17
	push r18
	push r30
	push r31



	in	r17, SREG
	push r17

	TURN_DISPLAY_OFF

	;get the current segment display value
	ldi r30, low(DisplaySeg1)
	ldi r31, high(DisplaySeg1)
	lds r17, DisplaySegCounter
	clr r18
	add r30, r17
	adc r31, r18
	ld  r18, z
	out PORTA, r18

	;increment counter for next round
	;make a backup copy in r18
	mov r18, r17
	inc r17
	cpi r17, NUM_SEGS_TO_DISPLAY
    brlt SisplaySegCountSet
    
    ;reset counter
    clr r17
    
    
SisplaySegCountSet:    
    
	sts  DisplaySegCounter, r17

	;now turn the appropriate segment on

	ldi r30, low(DisplaySegMaskTable << 1)
	ldi r31, high(DisplaySegMaskTable << 1)

	add r30, r18
	clr r18
	adc r31, r18

	;get segment turn on mask in r0
	push r0
	lpm 


	in r18, PORTB
	and r18, r0
	out PORTB, r18

	pop r0

	;now clear TOV0 flag so that next interrupt can happen
	in	r17, TIFR
	ori	r17, (1 << TOV0)
	out	TIFR, r17
    
    
    ;set a timer tick flag for key processing
    
    ldi r17,1
    sts (KeyTimerTickFlag), r17


	;check for one sec count
	lds  r30, TimerOneSecCount
	lds  r31, TimerOneSecCount + 1

	mov  r17, r30
	or   r17, r31
	breq TimerNoOneSecCheck

	;dummy decrement
	ld   r17, -z
	sts  TimerOneSecCount, r30
	sts  TimerOneSecCount + 1, r31

	mov   r17, r30
	or    r17, r31
	brne  TimerNoOneSecCheck

	;here a second has expired
	;set flag
	ldi  r17, 1
	sts  TimerOneSecondFlag, r17




TimerNoOneSecCheck:


	pop r17
	out SREG, r17

	pop r31
	pop r30
	pop r18
	pop r17

	reti

/*********************************************************/
    
    	.cseg 
DisplaySegMaskTable:  
                      .db   ~(1 << 3), ~(1 << 4)
                      .db   ~(1 << 5), ~(1 << 6)
                      .db   ~(1 << 7), $FF
					  
					  

	.dseg

//CRITICAL ORDER. DO NOT RE-ARRANGE
DisplaySeg1:				.db  $FF
DisplaySeg2:				.db  $FF
DisplaySeg3:				.db  $FF
DisplaySeg4:				.db  $FF
DisplaySeg5:                .db  $FF
DisplaySegCounter:			.db	 0
TimerOneSecondFlag:			.db	 0
TimerOneSecCount:			.dw  0
//CRITICAL ORDER ENDS












