/************** subroutines to handle calibration functions *****/
/*********************************************************/	


        .eseg
        
//CRITICAL ORDER. DO NOT RE-ARRANGE 
CalibParamStart:       
Zero_Offset:            .dw     0
Calib_Gain:             .dw     ((512*2000)/2544)/2
Calib_Val:              .dw     5000
Param_CRC:              .db     0
CalibParamEnd:
//CRITICAL ORDER ENDS 


        .dseg
//CRITICAL ORDER. DO NOT RE-ARRANGE         
Zero_Offset_RAM:        .dw     0 
Calib_Gain_RAM:         .dw     ((512*2000)/2544)/2
Calib_Val_RAM:          .dw     5000
ParamCRC_RAM:           .db     0
Calib_Ratio_Int:        .dw     0
Calib_Ratio_Fraction:   .dw     0

//CRITICAL ORDER ENDS                

/*********************************************************/	

		.cseg
        
Initialize_CalibParams:
        ldi r26, low(Zero_Offset)
        ldi r27, high(Zero_Offset)
        
        ldi r28, low(Zero_Offset_RAM)
        ldi r29, high(Zero_Offset_RAM)
        
        ldi r18, (CalibParamEnd - CalibParamStart)
        
Initialize_CalibParams_Loop:
            out EEARL, r26
            out EEARH, r27
            
Initialize_CalibParams_Wait:

	        sbic EECR, EEWE
	        rjmp Initialize_CalibParams_Wait

	        ;startr read
	        sbi EECR, EERE
	        in 	r17, EEDR
            st y+, r17
            ;dummy read to increment pointers
            
            ld r17, x+
            
            dec r18
            brne  Initialize_CalibParams_Loop
            
Initialize_CalibParams_ComputeRatio:
           
        ;now compute the ratio Calib_Val/Calib_Gain
        
        lds r20,  (Calib_Val_RAM)
        lds r21,  (Calib_Val_RAM+1)
        
        lds r22,  (Calib_Gain_RAM)
        lds r23,  (Calib_Gain_RAM+1)
        
        
        call UnsignedFractionalDivision
        
                
        sts (Calib_Ratio_Int),   r22
        sts (Calib_Ratio_Int+1), r23
        
        
        
        sts (Calib_Ratio_Fraction),   r20
        sts (Calib_Ratio_Fraction+1), r21
        
        
                    
            
        ret            

/*********************************************************/	
CalibUpdateEEPROM:



        ;write value in r17 to EEPROM
        ;address is in x
        
        ;r17 is trashed
        ;x is incremented by 1
        
	        sbic EECR, EEWE
	        rjmp CalibUpdateEEPROM
        
        
        out EEARL, r26
        out EEARH, r27
        out EEDR,  r17
        
        cli
        sbi EECR, EEMWE
        sbi EECR, EEWE
        sei
        
        ;dummy read to update x
        ld r17, x+
        
        ret
        

/*********************************************************/
UpdateZero_Offset:
        ;update zero offset in EEPROM and RAM  
        
           
        
        ;input value is in r21:r20
        
        ;first update RAM copy
        ldi r26, low(Zero_Offset_RAM)
        ldi r27, high(Zero_Offset_RAM)
        
        st x+, r20
        st x+, r21
        
        ;now update EEPROM Copy
        
        ldi r26, low(Zero_Offset)
        ldi r27, high(Zero_Offset)
        mov r17, r20
        
        call CalibUpdateEEPROM
        
        
        mov r17, r21
        
        call CalibUpdateEEPROM
        
        ret
        
/*********************************************************/
UpdateCalib_Gain:
        ;update calibration gain in EEPROM and RAM  
        
           
        
        ;input value is in r21:r20
        
        ;first update RAM copy
        ldi r26, low(Calib_Gain_RAM)
        ldi r27, high(Calib_Gain_RAM)
        
        st x+, r20
        st x+, r21
        
        ;now update EEPROM Copy
        
        ldi r26, low(Calib_Gain)
        ldi r27, high(Calib_Gain)
        mov r17, r20
        
        call CalibUpdateEEPROM
        
        
        mov r17, r21
        
        call CalibUpdateEEPROM
        
        ret
        
        
/*********************************************************/
UpdateCalib_Val:
        ;update calibration value in EEPROM and RAM  
        
           
        
        ;input value is in r21:r20
        
        ;compensate for zero offset
        
        
        
        
        ;first update RAM copy
        ldi r26, low(Calib_Val_RAM)
        ldi r27, high(Calib_Val_RAM)
        
        st x+, r20
        st x+, r21
        
        ;now update EEPROM Copy
        
        ldi r26, low(Calib_Val)
        ldi r27, high(Calib_Val)
        mov r17, r20
        
        call CalibUpdateEEPROM
        
        
        mov r17, r21
        
        call CalibUpdateEEPROM
        
        ret
        
        
/*********************************************************/
CalibrateADCval:
        ;converts adc value in r21:r20 to value in gram
        ;value is returned in r21:r20
        
        ;since we have 10 bit adc and we are using only +ve values,
        ;input is  restricted to [0,511] range.
        ;calibrated value is in the range [0,1000] or [0, 9999]
        
        
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
        
        ;if -ve, clamp to zero
        brpl  NoNeedToClip
        
        clr r20
        clr r21
        
NoNeedToClip:        
        
        
        
        
        ;now multiply by integer part of gain
        
        ;We are multiplying two 16 bit numbers
        ;Also, we no that result fits in 16 bit
        ;(256*Ah + Al)*(256*Bh + Bl)
        ;= 256*256*AhBh + 256*(Ah*Bl + Bh*Al) + Al*Bl
        ;we no that Ah*Bh is zero
        ;also, hi byte of (Ah*Bl + Bh*Al) also must be zero
        
        
        
        lds r17, (Calib_Ratio_Int)
        ;AH * Bl
        mul r21, r17
        
        mov r2,r0
       
        ;Al*Bl
        mul r20, r17
        add r1, r2
        
        push r0
        push r1
        
        lds r17, (Calib_Ratio_Int + 1)
        ;Bh*Al
        mul r17, r20
               
       
        
        pop     r3
        add     r3, r0
        pop     r2
        
        
        ;r3:r2 holds result from multiplication by integer part   
        ;multiply by remainder
        
        ;We are multiplying two 16 bit numbers
        ;Also, we need only upper 16 bits of result
        ;(256*Ah + Al)*(256*Bh + Bl)
        ;= 256*256*AhBh + 256*(Ah*Bl + Bh*Al) + Al*Bl
        
        
        
        
        
        lds r17, (Calib_Ratio_Fraction)
        ;Al*Bl
        mul r20, r17
        
       ;backup r1
        mov r4, r1
        
        
        ;Ah*Bl
        mul r21, r17
        
        add r0, r4
        clr r4
        adc r1, r4
        
        ;add to final result
        adc r2, r1
        clr r1
        adc r3, r1
        
        
        
        lds r17, (Calib_Ratio_Fraction+1)
        
        ;Al*BH
        mul r20, r17
        
        
        ;add to final result
        add r2, r1
        clr r1
        adc r3, r1
        
        
        ;AH*BH
        mul r21, r17

        ;add to final result
        add r2, r0
        adc r3, r1
        
        ;restore results
        mov r20, r2
        mov r21, r3
        
        
                
                
        ret
        
                
        
        
        
        
        
        
        
        
        
          
        
        
          
        
        
        

                
        
        
        
        
        

/*********************************************************/	
