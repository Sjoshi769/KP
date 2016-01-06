/************** subroutines to handle adc functions *****/


.EQU    NUM_SAMPLES_FOR_AVERAGING=1
.EQU    ADC_BUFF_SIZE=16*NUM_SAMPLES_FOR_AVERAGING
.EQU    ADC_ADJUSTMENT_FACTOR=10001
.EQU    MAX_CLOCK_COUNT_VALUE=(20001  + ADC_ADJUSTMENT_FACTOR)
.EQU    MAX_LOAD_VAL=9999



		.cseg

/*********************************************************/	
Initialize_ADC:
	

Initialize_ADC_BuffsAndVars:

		ldi r16, low(ADCBuff)
		ldi r17, high(ADCBuff)

		ldi r26, low(ADCBuffWrPtr)
		ldi r27, high(ADCBuffWrPtr)

		st x+, r16
		st x+, r17
		st x+, r16
		st x+, r17

		clr r18
		st x+, r18


		ret

		

/*********************************************************/	
TIM1_CAPT:

        ;This is ADC ready (exsternal ADC) ISR.

		;in ISR, we are allowed to use only r0-r15. This saves backup/restore cycles.
		;If other registers need to be used, backup/restore them on stack.


		;need to preserve flags 




		push r26
		push r27
		push r16
		push r17
        push r18

		in  r16, SREG
		push r16

		lds r26,	ADCBuffWrPtr
		lds r27,	ADCBuffWrPtr + 1
        
        

        ;here input is positive
		;read adc and store it
		in	r16, ICR1L
		in	r17, ICR1H
        
        ;If value is larger than permissible limit, this is a case of 
        ;some mis behavior. Need further investigation.
        ;For now, just discard the sample
        
        
        cpi  r17, high(MAX_CLOCK_COUNT_VALUE)
    
        ;if higher, need to discard
        brpl  DiscardADCVal
    
        ;if equal, need to compare lower val
        brne ADCApplyAdjustmentFactor
    
        cpi  r16, low(MAX_CLOCK_COUNT_VALUE)
        brsh DiscardADCVal
    
        
        
        
ADCApplyAdjustmentFactor:        
        
        
        
        subi  r16, low(ADC_ADJUSTMENT_FACTOR)
        sbci  r17, high(ADC_ADJUSTMENT_FACTOR)
        
        ;check polarity
        
        in    r18, PINC
        andi  r18, $40
        brne  ADCStoreVal
        
        ;negate input
        neg   r16
        clr   r18
        adc   r17, r18
        neg   r17
        
        
        
ADCStoreVal:
		st x+,  r16
		st x+,  r17
        
        
        sts (ADCCurrentValue+1), r17
        sts (ADCCurrentValue), r16



		;wrap around the pointer and store it back


		rcall RewindADCRdWrPtr

		;store the updated pointer

		sts ADCBuffWrPtr,	r26
		sts ADCBuffWrPtr + 1,	r27


		;increment the dipstick


		lds 	r26,	ADCBuffDipstick

		inc		r26

		sts 	ADCBuffDipstick,		r26

DiscardADCVal:

		;now clear TIMER1 count for next run
        clr r17
        out TCNT1H, r17
        out TCNT1L, r17
        
		
		pop	r16
		out SREG, r16
        
        
        pop r18
			


		pop r17
		pop r16
		; 

		pop r27
		pop r26


		reti

        
    


/*********************************************************/	
RewindADCRdWrPtr:

	;This function is also called from foreground thread (ADC_RDY) ISR.
	;Input is X pointer (r27:r26)
	;Output is also in X
	;Make sure to use minimal set of registers.
	;currently using r17, r17, SREG

	ldi r16, low(ADCBuffEnd)
	ldi r17, high(ADCBuffEnd)

	cp  r27, r17
	brlo	store_ADC
	breq    compare_lower_ADC
	jmp		rewind_ADC


compare_lower_ADC:
	;here upper byte is matching, we need to compare lower bytes


	cp 	r26, r16
	brlo	store_ADC

	rewind_ADC:

	ldi r26, low(ADCBuff)
	ldi r27, high(ADCBuff)



	store_ADC:

	ret


/***************************************************************/
ADCtoDisplay:
    ;converts latest sample from ADC and displays it
    
    ldi r18, NUM_SAMPLES_FOR_AVERAGING
    
    lds r17, ADCBuffDipstick
    cp  r17, r18
    
    brsh  ADCtoDisplay_Continue  
    jmp   ADCtoDisplay_ret
    
    
ADCtoDisplay_Continue:
    
    
    ;now compute average
    ;directly compute average in r21:r20
    
    ;with a max adc signal value of 511, max nume of samples that can
    ;be added  without 16 bit overflow is 65535/511 = 12
    ;so make sure  NUM_SAMPLES_FOR_AVERAGING is less than 12
    
    clr r20
    clr r21
    
    
    lds r26,  ADCBuffRdPtr
    lds r27,  ADCBuffRdPtr + 1
    
ADCSampleAveragingLoop:

        ld  r17, x+
        add r20, r17
        ld  r17, x+
        adc r21, r17
    
    
    call RewindADCRdWrPtr
    
    
        dec r18
        brne  ADCSampleAveragingLoop
        
        
        
    ;store pointer    
    sts ADCBuffRdPtr, r26
    sts ADCBuffRdPtr+1, r27
    
        
    ;now update dipstick        
    
    
    cli
    lds r17, ADCBuffDipstick
    subi r17, NUM_SAMPLES_FOR_AVERAGING
    sts  ADCBuffDipstick, r17
    
    sei
    
    ;now divide  by number of samples
    
    
    ldi r22, NUM_SAMPLES_FOR_AVERAGING
    clr r23
    
    call UnsignedShortDiv
    
    ;result is in r25:r24
    ;pass it on in r21:r20
    
    mov r20, r24
    mov r21, r25
    
    sts ADCCurrentAverageValue, r20
    sts ADCCurrentAverageValue + 1, r21
    
        
    ;calibrate this value
    
    call CalibrateADCval
    
    ;max value we can display is 9999
    ;so clip result to 9999
    cpi  r21, high(MAX_LOAD_VAL)
    
    ;if higher, need clipping
    brpl  ValibValClip
    
    ;if equal, need to compare lower val
    brne ValibValClipDone
    
    cpi  r20, low(MAX_LOAD_VAL)
    brsh ValibValClip
    
    jmp  ValibValClipDone
    
    
    
ValibValClip:

    ldi  r20, low(MAX_LOAD_VAL)
    ldi  r21, high(MAX_LOAD_VAL)
    
    
        
    
ValibValClipDone:
    

	push r20
	push r21
    
    sts CurrentCalibratedVal, r20
    sts CurrentCalibratedVal+1, r21
    
    
    //call UpdateStatistics
    
    call CheckStartStop 
    
    call DisplayDigit


	call SendPackedtoUART

	pop r21
	pop r20
    
    
    call SelectDisplay
    
    
    call DisplayAdjustUnit
    
    
      
    
ADCtoDisplay_ret:

    ret
    
    
   

/***************************************************************/
CheckStartStop:

    ;r21:r20 has current calibrated val

    push    r20
    push    r21

    ;see if start/stop condition has changed
    lds r17,    DisplayAvgMode
    and r17,    r17
    brne DisplayIsOn
    
    ;here display is off
    ;check if current value is greater than thresh
    
    cpi     r21, high(START_STOP_THRESH)
    breq    ModeOffCheckLower
    
    brsh    ToggleDisplayMode
    
    jmp     CheckStartStop_Ret
    
ModeOffCheckLower:    
    
    ;check lower digit
    cpi     r20, low(START_STOP_THRESH)
    brlo    CheckStartStop_Ret
    
    jmp     ToggleDisplayMode
    
DisplayIsOn:

    ;check if value is falling below thresh
    
    cpi     r21, high(START_STOP_THRESH)
    brlo    ToggleDisplayMode
    breq    ModeOnCheckLower
    jmp     CheckStartStop_Ret

ModeOnCheckLower:   
    cpi     r20, low(START_STOP_THRESH)
    brsh   CheckStartStop_Ret
    
    




ToggleDisplayMode:

    ;use existing code
    ;fake a key event
    //ldi  r17,2 
    
    call KeyToggleStartStop

CheckStartStop_Ret:


    pop     r21
    pop     r20
    ret
    
/***************************************************************/
SelectDisplay: 
    ;select Display value to be displayed
    ;r21:r20 current value
    
    ;if averaging is on, only display current value   
    lds r17,  DisplayAvgMode
    and r17, r17
    
    brne SelectDisplay_End
    
    ;here averaging is off. Select min/max/avg value
    lds   r17, DisplayType
    and   r17, r17
    breq  SelectDisplay_End
    
    cpi   r17, 1
    brne  CheckNextDisplayType
    
    ;here display minimum
    lds  r20, DisplayMin
    lds  r21, DisplayMin + 1
    jmp  SelectDisplay_Common
    
CheckNextDisplayType:

    cpi   r17, 2
    brne  DisplayTypeAvg
    
    


    
    ;here display is Maximum
    lds  r20, DisplayMax
    lds  r21, DisplayMax + 1
    jmp  SelectDisplay_Common
    
    
    
DisplayTypeAvg:    
    
    lds  r20, DisplayAvg
    lds  r21, DisplayAvg + 1
    
    
SelectDisplay_Common:


    //;convert to 7 Seg
    //call DisplayDigit

    
    
    


SelectDisplay_End:

    ret

    
    
/***************************************************************/
UpdateStatistics:
    ;Do not trash r21:r20
    ;update minimum first
    
    ;check if averaging mode is on
    lds r17, DisplayAvgMode
    and r17, r17
    brne UpdateStatistics_Continue   
    jmp UpdateStatistics_End
    
UpdateStatistics_Continue:    
    
    
    lds r16,  DisplayMin
    lds r17,  DisplayMin +1
    
    cp   r21, r17
    brlo UpdateMin
    breq UdateMinCheckLower 
    jmp  UpdateStatCheckMax
      
    
UdateMinCheckLower:

    cp   r20, r16
    brlo UpdateMin
    jmp  UpdateStatCheckMax

    
    
UpdateMin:
        
    sts DisplayMin      , r20
    sts DisplayMin +1   , r21
    
UpdateStatCheckMax:

    lds r16,  DisplayMax
    lds r17,  DisplayMax +1
    
    
    cp  r17, r21
    brlo UpdateMax
    breq UdateMaxCheckLower
    
    jmp UpdateStatUpdateAvg
    
UdateMaxCheckLower:

    cp  r16, r20
    brlo UpdateMax
    jmp UpdateStatUpdateAvg
    
    
UpdateMax:
        
    sts DisplayMax      , r20
    sts DisplayMax +1   , r21
    
UpdateStatUpdateAvg:

    ;done with min/max
    ;update average
    
    lds r17,  DisplayAvgAccum0
    add r17, r20
    sts DisplayAvgAccum0, r17
    
    lds r17,  DisplayAvgAccum0 + 1
    adc r17, r21
    sts DisplayAvgAccum0 + 1, r17
    
    clr r18
    lds r17,  DisplayAvgAccum1 
    adc r17, r18
    sts DisplayAvgAccum1, r17

    lds r17,  DisplayAvgAccum1 + 1
    adc r17, r18
    sts DisplayAvgAccum1 + 1, r17
    
    ;update count
    lds  r26, DisplayAvgCount
    lds  r27, DisplayAvgCount +1
    
    ;dummy load to increment 
    ld  r17, x+
    
    sts  DisplayAvgCount    ,  r26
    sts  DisplayAvgCount +1 ,  r27
    
    
UpdateStatistics_End:
    
    ret
    
    
    


/***************************************************************/

	.dseg

ADCBuff:				.byte  (ADC_BUFF_SIZE)
ADCBuffEnd:


	.dseg
UartDigit1:				.db  0
UartDigit2:				.db  0
UartDigit3:				.db  0
UartDigit4:				.db  0


//CRITICAL ORDER. DO NOT RE-ARRANGE
ADCBuffWrPtr:			.dw (ADCBuff)
ADCBuffRdPtr:			.dw (ADCBuff)
ADCBuffDipstick:		.db	0
ADCCurrentValue:        .dw (0)
ADCCurrentAverageValue: .dw (0)
//CRITICAL ORDER ENDS

CurrentCalibratedVal:   .byte   2









