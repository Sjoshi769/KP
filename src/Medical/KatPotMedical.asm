/*
 * KatPot.asm
 *
 *  Created: 03/Jun/2015 13:27:12
 *   Author: Sanjay
 */ 

 	;.include "atmega16.inc"
  
.EQU    REVERSE_ADC_LOGIC=1
.EQU    ENABLE_DIAGNOSTICS=0
.EQU    START_STOP_THRESH=4

/**************************************************************/
	.cseg


	.org 0

	jmp RESET				;reset handler	jmp RESET				;reset handler
	jmp EXT_INT0			;IRQ0 handler
	jmp EXT_INT1			;IRQ1 handler
	jmp TIM2_COMP			;Timer2 compare handler
	jmp TIM2_OVF			;Timer2 overflow handler
	jmp TIM1_CAPT			;Timer1 capture handler
	jmp TIM1_COMPA			;Timer1 compareA handler
	jmp TIM1_COMPB			;Timer1 compareB handler
	jmp TIM1_OVF			;Timer1 overflow handler
	jmp TIM0_OVF			;Timer0 overflow handler
	jmp SPI_STC				;SPI transfer complete handler
	jmp USART_RXC			;USART Rx complete handler
	jmp USART_UDRE			;USART empty handler7
	jmp USART_TXC			;USART Tx complete handler
	jmp ADC_RDY				;ADC conversion complete handler
	jmp EE_RDY				;EEPROM ready handler
	jmp ANA_COMP			;Analog comparison handler
	jmp TWSI				;Two wire serial comm handler
	jmp EXT_INT2			;IRQ2 handler
	jmp TIM0_COMP			;Timer0 compare handler
	jmp SPM_RDY				;Store program memory ready handler

/**************************************************************/

 EXT_INT1:
 TIM2_OVF:
 //TIM1_CAPT:
 TIM1_COMPA:
 TIM1_COMPB:
 TIM1_OVF:
 //TIM0_OVF:
 SPI_STC:
 // USART_RXC:
 //USART_TXC:
 USART_UDRE:
 ADC_RDY:
 TIM2_COMP:
 EE_RDY:
 ANA_COMP:
 TWSI:
 EXT_INT0:
 TIM0_COMP:
 SPM_RDY:
 EXT_INT2:
	reti

    
/*****************//*********************************************/    
RESET:

    ldi r16, high(RAMEND)
	out SPH,r16
	ldi r16, low(RAMEND)
	out SPL,r16


	;device booted.
	;in case watchdog expired, devie will be reset.
	;disable interrupts
	cli

	;initialize ports.
	;PORTA is used for display segments and is always an output port.
	ldi r17, $FF
	out DDRA, r17
	clr r18
	out PORTA, r18

	;port B bits 3-7 are used as display digit drivers and 
	;should be configured as outptut
        ;PB1 is input (T1)
	ldi r17, $F8
	out DDRB, r17
	out PORTB, r18


	;also configure PORTC bits as input. Bit 7 is used for key sense
	out DDRC, r18
	nop
	out PORTC, r18

	;PORTD bits 2,3,4,5 are used as key outputs,
	;bit 1 is UART Tx output
	;Bit 7 is OC2 output
	ldi r17, $BE
	out DDRD, r17
	out PORTD, r18



	;all ports configured. Now initialize peripherals.


	call Initialize_ADC

	call Initialize_Timers

	call Initialize_UART

	call Initialize_CalibParams
    
    
    //No keys for medical version

	call Initialize_Keys
    
    call Initialize_Display
    
	;configure watchdog for 1 sec interval and enable it.
	WDR

	ldi r16, (1 << WDE) | (1 << WDP2) | (1 << WDP1)
	out WDTCR, r16
    


	;enable interrupts
	sei


	call DisplayStartupRoutine



	;reset ADC pointers as there was a possible overflow in buffer as 
	;we are not reading values from buffer
	cli
	call Initialize_ADC_BuffsAndVars
	sei

	;wait for a second, this will place some adc values in buffer
	call DisplayWaitForASecond

	call ADCtoDisplay

	;fake a zero key event, so zero offset is set
	call UpdateZero_Offset_Key_Event

	;ready for main loop


SystemMainLoop:


        ;reset watchdog timer
        WDR

		call UARTHandleHostEvent


        call ProcessKeys
        call CheckKeyPress
        
        //;key code is returned in r17
        and r17, r17
        brmi NoKeyEventsToHandle
        breq NoKeyEventsToHandle  

            call HandleKeyEvent
        
        
NoKeyEventsToHandle:



        //call HandleAveraging


        call ADCtoDisplay
        
        



    jmp SystemMainLoop
    
    

	
	.include "adc_subs.asm"

	.include "timer_subs.asm"

	.include "uart_subs.asm"

	.include "display_subs.asm"

	.include "key_subs.asm"

	.include "math_subs.asm"

	.include "calib_subs.asm"

	.include "test_subs.asm"







