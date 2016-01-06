/************** subroutines to handle display functions *****/
/*********************************************************/	

.equ    DISPLAY_UNIT_GM=0
.equ    DISPLAY_UNIT_KG=1
.equ    DISPLAY_UNIT_N=2
.equ    NUM_DISPLAY_UNITS=3


        .cseg
        
        
/*********************************************************/	
        
Initialize_Display:

        ;init vars
        ldi     r26, low(DisplayVarStart)
        ldi     r27, high(DisplayVarStart)
        
        clr     r17
        ldi     r18, (DisplayVarEnd - DisplayVarStart)
        
Initialize_Display_Loop:
        st      x+, r17
        dec     r18
        brne    Initialize_Display_Loop
        
		;turn off all leds
		ldi		r17, $FF
		sts     DisplayTemp5, r17
        ret    

        
     
/*********************************************************/	
Initialize_Averaging:

        ;init vars
        ldi     r26, low(DisplayVarStart)
        ldi     r27, high(DisplayVarStart)
        
        clr     r17
        ldi     r18, (DisplayVarAVGReset - DisplayVarStart)
        
Initialize_Averaging_Lopp:
        st      x+, r17
        dec     r18
        brne    Initialize_Averaging_Lopp
        
        ret    



/*********************************************************/	
//         a        
//       ------
//     f | g  | b
//       |----|
//     e |    | c
//       ------    dp    
//          d
           
           
HexTo7SegTbl:   .dw    ~(($06 << 8) + $3F)     //1, 0
                .dw    ~(($4F << 8) + $5B)     //3, 2
                .dw    ~(($6D << 8) + $66)     //5, 4
                .dw    ~(($07 << 8) + $7D)     //7, 6
                .dw    ~(($67 << 8) + $7F)     //9, 8
                

/*********************************************************/	

		.cseg
        
HexTo7Seg:


        ;converts a hex digit in r17 and returns corresponding 7 segment code 
        ;in r17
        
        ;must preserve r20:r25
        
        push r30
        push r31
        push r0

        ldi  r30, low(HexTo7SegTbl << 1)
        add  r30, r17
	    ldi  r31, high(HexTo7SegTbl << 1)
        clr  r17
        adc  r31, r17
    
        lpm
	   
        mov r17, r0
        pop r0
        pop r31
        pop r30 

        
        ret       
        
        
               
 /*********************************************************/	       
DisplayDigit:        
		;Input is 16 bit number in the range 0 to 9999
        
        ;input is in r21:r20

        ;get 1000 digit

        
        ldi r22, low(1000)
        ldi r23, high(1000)
        
        
        call UnsignedShortDiv
        
        ;result is in r24:r25
        
        ;assume r25 is zero. Also, store digit for uart transmission
        mov r17, r24
        ldi r25, '0'
        add r24,r25
		sts UartDigit1, r24

        call HexTo7Seg
        
        push r17
        
        
        ;get 100 digit

        
        ldi r22, low(100)
        clr r23
        
        
        call UnsignedShortDiv
        
        ;result is in r24:r25
        
        ;assume r25 is zero
        mov r17, r24
        ldi r25, '0'
        add r24,r25
		sts UartDigit2, r24

        call HexTo7Seg
        
        push r17
        
        
        ;get 10 digit

        
        ldi r22, low(10)
        clr r23
        
        
        call UnsignedShortDiv
        
        ;result is in r24:r25
        
        ;assume r25 is zero
        mov r17, r24
        ldi r25, '0'
        add r24,r25
		sts UartDigit3, r24

        call HexTo7Seg
        
        push r17
        
        ;unit digit is in r20
        
        mov r17, r20
        ldi r25, '0'
        add r20,r25
		sts UartDigit4, r20
        
        call HexTo7Seg
        
        ;now update all 4 display digits
        
        cli
        
        ldi r26, low(DisplayTemp)
        ldi r27, high(DisplayTemp)
        
        st x+, r17
        pop r17
        st x+, r17
        pop r17
        st x+, r17
        pop r17
        st x+, r17
        
        
        ;enable interrupts and return
        sei
        
        ret

/*********************************************************/	
DisplayStartupRoutine:
	;display a test pattern
	ldi r20, low(0000)
	ldi r21, high(0000)
	//call DisplayDigit
    call DisplayAdjustUnit
	call DisplayWaitForASecond

	ldi r20, low(1111)
	ldi r21, high(1111)
	//call DisplayDigit
    call DisplayAdjustUnit
	call DisplayWaitForASecond

	ldi r20, low(2222)
	ldi r21, high(2222)
	//call DisplayDigit
    call DisplayAdjustUnit
	call DisplayWaitForASecond

	ldi r20, low(3333)
	ldi r21, high(3333)
	//call DisplayDigit
    call DisplayAdjustUnit
	call DisplayWaitForASecond

	ldi r20, low(4444)
	ldi r21, high(4444)
	//call DisplayDigit
    call DisplayAdjustUnit
	call DisplayWaitForASecond

	ldi r20, low(5555)
	ldi r21, high(5555)
	//call DisplayDigit
    call DisplayAdjustUnit
	call DisplayWaitForASecond

	ldi r20, low(6666)
	ldi r21, high(6666)
	//call DisplayDigit
    call DisplayAdjustUnit
	call DisplayWaitForASecond

	ldi r20, low(7777)
	ldi r21, high(7777)
	//call DisplayDigit
    call DisplayAdjustUnit
	call DisplayWaitForASecond

	ldi r20, low(8888)
	ldi r21, high(8888)
	//call DisplayDigit
    call DisplayAdjustUnit
	call DisplayWaitForASecond

	ldi r20, low(9999)
	ldi r21, high(9999)
	//call DisplayDigit
    call DisplayAdjustUnit
	call DisplayWaitForASecond

	ret



/*********************************************************/	
DisplayWaitForASecond:
	;disable interrupts
	cli
	ldi r17, low(TIMER_TICKS_PER_SECOND)
	sts TimerOneSecCount, r17
	ldi r17, high(TIMER_TICKS_PER_SECOND)
	sts TimerOneSecCount+1, r17
	clr r17
	sts TimerOneSecondFlag, r17
	sei

DisplayWaitForASecondLoop:
	;reset watchdog
    WDR
	lds		r17, TimerOneSecondFlag
	and		r17, r17
	breq	DisplayWaitForASecondLoop

	ret



/*********************************************************/	       
DisplayAdjustUnit: 


    ;turn off all unit leds
    lds     r17, DisplayTemp5
    ori     r17, $70
    sts     DisplayTemp5, r17
    
    
    

    ;value is in r21:r20

    ;check unit format
    lds     r17,  DisplayUnit
    cpi     r17,  DISPLAY_UNIT_GM
    breq    DisplayAdjustUnit_GM
    
    
    cpi     r17,  DISPLAY_UNIT_KG
    brne    DisplayAdjustUnitNewton
    
    
    
    ;here unit is KG.
    ;just set decimal point
    
    call    DisplayDigit
    
    lds     r17, DisplayTemp + 3
    andi    r17, ~$80
    sts     DisplayTemp + 3, r17
    
    ldi     r18, ~$20
    jmp     DisplayAdjustUnit_Copy
    
DisplayAdjustUnitNewton:
   
    ;unit is newton
    ;need to multiply value by 9.8
    ;max input val is 9999
    
    ;we are going to offer two digits past decimal point
    ;so we need to compute x*0.98 = x*(1 - 0.02)
    ;x*0.02 can be computed as x/50
    
    push r20
    push r21
    
    ldi r22, 50
    clr r23
    
    call UnsignedShortDiv
    
    ;quotient is in r25:r24
    ;ignore remainder
    
    pop  r21
    pop  r20
    
    sub  r20, r24
    sbc  r21, r25
    
    call DisplayDigit

    
    ;adjust decimal point
    
    lds     r17, DisplayTemp + 2
    andi    r17, ~$80
    sts     DisplayTemp + 2, r17
    
    
    ldi     r18, ~$40    
    
    jmp     DisplayAdjustUnit_Copy
        

DisplayAdjustUnit_GM:


    call DisplayDigit

    ;led mask
    ldi     r18, ~$10

DisplayAdjustUnit_Copy:
    
    ;source pointer
    ldi     r26, low(DisplayTemp)
    ldi     r27, high(DisplayTemp)
    
    ;destination pointer
    ldi     r28, low(DisplaySeg1)
    ldi     r29, high(DisplaySeg1)
    
    
    ld      r17, x+
    st      y+,  r17
    ld      r17, x+
    st      y+,  r17
    ld      r17, x+
    st      y+,  r17
    ld      r17, x+
    st      y+,  r17
    
    ;led
    ld      r17, x+
    ;apply mask
    //and     r17, r18
    st      y+,  r17
    
    
    
    ret    
    
    
    
/*********************************************************/	
HandleAveraging:
    ;look for change in Average start/stop status
    lds r17,  DisplayAvgMode
    lds r18,  DisplayAvgModeOld
    cp  r17,  r18
    breq AveragingNoChange
    
    
    ;here status has changed
    ;update old copy
    sts DisplayAvgModeOld, r17
    
    ;check new status
    and   r17, r17
    breq  AveragingTurnedOff
    
    ;Here averaging is turned on
    call Initialize_Averaging
    
    ;use current value as minimum
    lds r17, CurrentCalibratedVal
    sts  DisplayMin, r17
    lds r17, CurrentCalibratedVal+1
    sts  DisplayMin+1, r17
    
    
    
    jmp  AveragingNoChange
    
    
    
    
    
AveragingTurnedOff:

    
    ;compute the average value
    nop
    
    ;perform fractional division
    ;first divide upper 16bits of accumulated sum by number of samples
    
    lds r20,  DisplayAvgAccum1
    lds r21,  DisplayAvgAccum1  +  1
    
    lds r22,  DisplayAvgCount
    lds r23,  DisplayAvgCount   +  1
    
    ;perform fractional division
    
    call  UnsignedFractionalDivision
    
    ;result is in r23:r22:r21:r20
    
    ;r23:r22 should be zero, otherwise we have overflow problem
    ;push r23
    ;push r22
    push r21
    push r20
    
    ;now divide lower 16 bits. This is another fractional division
    
    lds r20,  DisplayAvgAccum0
    lds r21,  DisplayAvgAccum0  +  1
    
    lds r22,  DisplayAvgCount
    lds r23,  DisplayAvgCount   +  1
    
    ;perform fractional division
    
    call  UnsignedFractionalDivision
    
    ;now ignore fractional part of this division
    ;add integer part with fractional part of first division
    
    pop r20
    pop r21
    
    add r20, r22
    adc r21, r23
    
    sts DisplayAvg, r20
    sts DisplayAvg+1, r21
    
    
    
    
    
    
    
        

AveragingNoChange:





    ret
    

    

/*********************************************************/	

    .dseg
//CRITICAL ORDER. DO NOT RE-ARRANGE
DisplayVarStart:    
DisplayMin:         .dw     0
DisplayMax:         .dw     0
DisplayAvg:         .dw     0
DisplayAvgAccum0:   .dw     0
DisplayAvgAccum1:   .dw     0
DisplayAvgCount:    .dw     0
DisplayVarAVGReset: 

DisplayType:        .db     0
DisplayAvgMode:     .db     0
DisplayAvgModeOld:  .db     0
DisplayUnit:        .db     (DISPLAY_UNIT_GM) 
DisplayVarEnd:
//CRITICAL ORDER ENDS


DisplayTemp:        
DisplayTemp1:   .byte   1        
DisplayTemp2:   .byte   1        
DisplayTemp3:   .byte   1        
DisplayTemp4:   .byte   1        
DisplayTemp5:   .byte   1        
        
        
        

/*********************************************************/	
