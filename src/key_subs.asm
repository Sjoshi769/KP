/************** subroutines to handle keyboard functions *****/
/*********************************************************/	


.equ    KEY_BUFF_SIZE=8

.equ    KEY_SENSE=7
.equ    KEY_SENSE_PIN=PINC
.equ    KEY_PIN=PIND
.equ    KEY_PORT=PORTD
.equ    KEY_DDRD=DDRD
.equ    KEY_MASK=$3C
.equ    KEY_K1=3
.equ    KEY_K2=4
.equ    KEY_K3=5
.equ    KEY_K4=6
.equ    KEY_DEBOUNCE_COUNT=100
.equ    NUM_KEYS=4


    
    
    

        .dseg
        
KeyPressBuff:           .byte KEY_BUFF_SIZE
KeyPressBuff_End:
KeyPressRdPtr:          .dw (KeyPressBuff)
KeyPressWrPtr:          .dw (KeyPressBuff)
KeyPressDipstick:       .db (0)
KeyDebounceCount:       .db (0)
KeyTimerTickFlag:       .db (0)
KeyReleasedFlag:        .db (0) 
                        
        


		.cseg

/*********************************************************/	
Initialize_Keys:

        ;init buffers and pointers
        ldi r16, low(KeyPressBuff)
        ldi r17, high(KeyPressBuff)
        
        ldi r26, low(KeyPressRdPtr)
        ldi r27, high(KeyPressRdPtr)
        
        st x+, r16      //RdPtr  
        st x+, r17
        st x+, r16      //WrPtr
        st x+, r17
        
        clr r17
        st x+, r17      //Dipstick
        st x+, r17      //Debounce Count
        st x+, r17      //TimerTick
        st x+, r17      //key released
        
        
        
        
        ;just output logic 0 on all key drive pins so that 
        ;a key press can be detected
        
        in   r17, KEY_PIN
        andi r17, ~($3C) 
        out  KEY_PORT, r17
        
        ;PORT -D bits 2-5 are now zero. Any single (or multiple)
        ;key press will result in a logic 0 on PORT-D bit 2.
        ;Otherwise, it will remain at logic 1
        
        ret


        

/*********************************************************/	
ProcessKeys:

        cli
        ;check if new timer tick has happened
        lds r17, (KeyTimerTickFlag)
        and r17, r17
        brne ProcessKeyContinue1
        jmp ProcessKeys_ret
ProcessKeyContinue1:    
        sei
        
        ;check debounce counter
        

        lds r17, (KeyDebounceCount)
        and r17, r17
        breq  Debounce_count_zero
        
        
        
        ;decrement count
        dec r17
        sts KeyDebounceCount, r17
        
        breq  ProcessKeyContinue2
        jmp   ProcessKeys_ret
        
ProcessKeyContinue2:
        
        ;here count is zero. Key press is stable
        
        
        ;if key was released, no action
        
        
        in r17, KEY_SENSE_PIN
        sbrc r17, KEY_SENSE
        jmp  ProcessKeys_ret
        
        ;key is pressed. Identify the key
        
        ;first set all but first key drive to logic 1
        
        
        ;key identifier  
        clr r18
        
        ;mask to test first key        
        ldi r16, ~(1 << 2)
        
        
        ;bit mask for keys
        ldi  r20, 1
        
        
        ;get loop counter in r19
        ldi r19, NUM_KEYS
        
KeySenseLoop:

            ;only configure one key signal as output and rest as input.
            ;this is needed to detect simultaneous key processing
            
            in   r17, KEY_DDRD
            andi r17, ~(KEY_MASK)
            com  r16
            or   r17, r16
            out  KEY_DDRD, r17
            com  r16
           
            
            

            in  r17, KEY_PIN
            or  r17, r16
            and r17, r16
            out KEY_PORT, r17
            
                ;wait for  2 microseconds
                ldi  r17, 16
KeySense_Delay_Loop:
                    dec  r17
                    brne  KeySense_Delay_Loop
                    
            ;sense the key again
            in r17, KEY_SENSE_PIN
            sbrc r17, KEY_SENSE
            jmp CheckForNextKey
            
            or  r18, r20
            
            
            
CheckForNextKey:
            
            ;update mask for next key
            
            ;set carry
            sec
            rol r16
            
            clc
            
            rol r20
                        
            dec r19
            brne KeySenseLoop
            
KeyIdentified: 

        ;write the key in buffer and update dipstick
        
        
        lds r26, (KeyPressWrPtr)
        lds r27, (KeyPressWrPtr+1)
        
        st  x+, r18
        
        ;rewind pointer here
        call RewindKeyRdWrPtr
        
        sts KeyPressWrPtr, r26
        sts KeyPressWrPtr+1, r27
        
        
        
        
        lds r17, (KeyPressDipstick)
        inc r17
        sts KeyPressDipstick, r17
  
  
        ;set releasedFlag
        ldi r17, 1
        sts KeyReleasedFlag, r17
 
        
        jmp ProcessKeys_ret 

            
                    
            
            

Debounce_count_zero:


        ;check key sense signal
        
        in r17, KEY_SENSE_PIN
        sbrc r17, KEY_SENSE
        jmp ClearReleasedFlag
        
        ;here some key is pressed, 
        
        ;if last key is not released ignore it
        lds r17, (KeyReleasedFlag)
        and r17, r17
        brne ProcessKeys_ret
        
        
        ;set debounce count to non zero value and exit
        
        ldi r17, KEY_DEBOUNCE_COUNT 
        sts  KeyDebounceCount, r17
        
        ;Fall through
        
ProcessKeys_ret:

        cli

 	    in  r17, KEY_DDRD
        ori r17, KEY_MASK
	    out KEY_DDRD, r17

        ;set all key drives to zero
        in   r17, KEY_PIN
        andi r17, ~(KEY_MASK) 
        out  KEY_PORT, r17
        
        
        
        
        ;unconditionally clear the flag
        clr r17
        sts KeyTimerTickFlag, r17
        
        sei

        ret
        
ClearReleasedFlag:

        clr r17
        sts KeyReleasedFlag, r17
        jmp ProcessKeys_ret

        


/*********************************************************/	
RewindKeyRdWrPtr:

	;Make sure to use minimal set of registers.
	;currently using r17, r17, SREG

	;input pointer is in r27:r26 pair

	ldi r16, low(KeyPressBuff_End)
	ldi r17, high(KeyPressBuff_End)

	cp  r27, r17
	brlo	store_Key
	breq    compare_lower_Key
	jmp		rewind_Key


compare_lower_Key:
	;here upper byte is matching, we need to compare lower bytes


	cp 	r26, r16
	brlo	store_Key

	rewind_key:

	ldi r26, low(KeyPressBuff)
	ldi r27, high(KeyPressBuff)



	store_key:

	ret


/*********************************************************/	
CheckKeyPress:
    ;checks if any key press events are pending in buffer.
    ;return the key code or -1 if none
    
    lds r17, KeyPressDipstick
    and r17, r17
    
    breq NoKeyEvent
    
    ;decrement dipstick
    dec r17
    sts KeyPressDipstick, r17


    ;read key from buffer
    
    lds r26, (KeyPressRdPtr)
    lds r27, (KeyPressRdPtr+1)
        
    ld  r17, x+
    push r17
        
    ;rewind pointer here
    call RewindKeyRdWrPtr
        
    sts KeyPressRdPtr, r26
    sts KeyPressRdPtr+1, r27
        
    pop r17
    
    ret
        

NoKeyEvent:
    ldi r17, $FF
    ret
    

/**************************************************/
HandleKeyEvent:
    
    ;r17 has key pressed mask.
    ;bit 0:  ZeroSet key
    ;        When pressed, current load is set as  zero load
    ;bit 1:  DisplayUnit key: 
    ;        Unit is toggled as gm, Kg, N in the sequence
    ;bit 2:  Start Average Key
    ;bit 3:  Reserved
    
    ;following combinations of simultanious key press are legal
    ;bit 0,1:  Enter Calibration mode: 1000 gm load
    ;bit 0,2:  Enter Calibration mode: 5000 gm load
    
    
    cpi r17, 01
    brne  CheckNextKeyEvent1 
    
    
    ;use current value as zero reference
UpdateZero_Offset_Key_Event:
    
    cli
    lds r20, (ADCCurrentAverageValue)
    lds r21, (ADCCurrentAverageValue + 1)
    sei
    
    call UpdateZero_Offset
    
    ret
     
    
     
    
CheckNextKeyEvent1:

    cpi     r17, 02
    brne    CheckNextKeyEvent2 
    
    //toggle average start/stop
    lds     r17, DisplayAvgMode
    sts     DisplayAvgModeOld, r17
    ldi     r18, 1
    eor     r17, r18
    sts     DisplayAvgMode, r17
    
    
    
    ;set LED
    ;assume turned off
    ldi  r17, $80
    
    breq LEDmaskSet

    ;here need to turn led on
    clr  r17    
    
LEDmaskSet:

    lds  r18, DisplayTemp5
    andi r18, ~$80
    or   r18, r17
    sts  DisplayTemp5, r18
    
    
    ;send STRT/STOP command
    
    ldi  r17, 'S'
    sts  UartDigit1, r17
    
    ldi  r17, 'T'
    sts  UartDigit2, r17
    
    
    lds  r16, DisplayAvgMode
    and  r16, r16
    breq SendStop
    
    ;here it is a start
    
    ldi  r16, 'R'
    sts  UartDigit3, r16
    
    ;ldi  r17, 'T'
    sts  UartDigit4, r17
    
    jmp SendStartStopCommand
    
    
SendStop:

    ldi  r16, 'O'
    sts  UartDigit3, r16
    
    ldi  r17, 'P'
    sts  UartDigit4, r17


    
SendStartStopCommand:

    
    call SendPackedtoUART

        
    ret




    
CheckNextKeyEvent2:


    cpi r17, 03
    brne  CheckNextKeyEvent3
    
      
    
    
    ldi r20, low(1000)
    ldi r21, high(1000)
    
    

CalibrationModeCommon:    

    
    ;calibration is always done in gram mode
    clr r17
    sts DisplayUnit, r17

    call UpdateCalib_Val
    
    
    lds r20, ADCCurrentAverageValue
    lds r21, ADCCurrentAverageValue + 1
    
        .if defined(REVERSE_ADC_LOGIC)
        
            ;zero offset adjustment
            lds r17, (Zero_Offset_RAM)
            sub r17, r20
            mov r20, r17
            lds r17, (Zero_Offset_RAM+1)
            sbc r17, r21
            mov r21, r17
                    
        
        
        .else        
            ;zero offset adjustment
            lds r17, (Zero_Offset_RAM)
    sub r20, r17
            lds r17, (Zero_Offset_RAM+1)
    sbc r21, r17
    
        .endif
    
    call UpdateCalib_Gain

    call Initialize_CalibParams_ComputeRatio
    
    ret
    
    
CheckNextKeyEvent3:


    cpi r17, 05
    brne  CheckNextKeyEvent4
    
    ldi r20, low(5000)
    ldi r21, high(5000)
    
    

    jmp CalibrationModeCommon    
    
    
    
    
CheckNextKeyEvent4:


    cpi   r17, 04
    brne  CheckNextKeyEvent5
    
    ;display max/min/avg/current selection
    lds   r17, DisplayType
    inc   r17
    andi  r17, 03
    sts   DisplayType, r17
    
    
    ret
    
    

CheckNextKeyEvent5:


    cpi r17, 8
    brne  CheckNextKeyEvent2 
    
    //toggle display unit
    lds r17, DisplayUnit
    inc r17
    cpi r17, NUM_DISPLAY_UNITS
    brmi DisplayUnitSet
    
    clr r17
    
    
DisplayUnitSet:

    sts DisplayUnit, r17
    
    ret


CheckNextKeyEvent6:


    ret    
    
    
              