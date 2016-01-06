/************** subroutines to handle math  functions *****/
/*********************************************************/	


		.cseg
        
UnsignedShortDiv:   
        ;performs division of two unsigned shorts
		;Input:
        ;r21:r20  numerator
        ;r23:r22  denominator
		;Output
        ;r25:r24  quotient
        ;r21:r20  remainder
        
        ;Takes about 660 cycles max



        ;clear quotient
        clr r24
        clr r25
        
        

        ;use successive approximation method
        
        ;set approximation mask in r19:r18
        ldi r19, 0x80
        clr r18       


DivisionIterationLoop:
        ;check if mask is zero
		and r19, r19
		brne LoopContinue

		and  r18, r18
		breq UnsignedShortDiv_ret
			

LoopContinue:

        ;update quotient with current approximation
		or  r25, r19
		or  r24, r18

		;need to perform r25:r24 * r23:r22


		;step 1
		mul r23, r25
		or r0, r1
		brne LoopSkipBit



		mul r23, r24
		;if this result is not zero, skip this bit
		and r1, r1
		brne LoopSkipBit

		;backup result 
		mov r2, r0
		mov r3, r1



		;now perform r25*r22

		mul r25, r22
		and r1, r1
		brne LoopSkipBit

		;add result to backup
		add r2, r0
		adc r3, r1

		and r3, r3
		brne LoopSkipBit

		;get last step
		mul r22, r24
		add r1, r2

		brcs LoopSkipBit

		;now compare result with numerator

		cp r21, r1
		brlo LoopSkipBit

		breq CheckNext

		jmp LoopSkipBitRotate

		;here result is equal, compare lower bytes
CheckNext:
		cp r20, r0
		brsh LoopSkipBitRotate


		



LoopSkipBit:
        ;here the mask bits needs to be cleared
		eor r25, r19
		eor r24, r18

		;now rotate mask bits
LoopSkipBitRotate:
		
		clc
		ror r19
		ror r18

		jmp DivisionIterationLoop
       
     
     
UnsignedShortDiv_ret:

		;now compute remainder in r21:r20

		mul  r22, r24
		sub  r20, r0
		sbc  r21, r1

		mul  r22, r25
		mov  r2, r0
		

		mul  r23, r24
		add  r0, r2
		sub  r21, r0


		ret

        

/*********************************************************/	
UnsignedShortNorm:
        ;Input: r21:r20
        ;unsigned 16 bit int
        
        ;must preserve r23:r22
        
        ;output: r21:r20: Normalized output
        
        ;r0: notmalization count
        
        ;trashing r31, r30, r17, r18, r0
        
        clr  r0
        
        mov  r17, r20
        or   r17, r21
        breq UnsignedShortNorm_Ret
        
        ;skip upper byte if it is zero
        and  r21, r21
        breq UnsignedNormCheckLowerByte
        
        ldi  r30, low(MathSubNormTable  << 1)
        ldi  r31, high(MathSubNormTable  << 1)
        
        add  r30, r21
        clr  r0
        adc  r31, r0
        lpm
        
        jmp  UnsignedNormCountFound
        
        ;here upper byte is zero
        ;get count from lower nibble
        
UnsignedNormCheckLowerByte:        
        
        ldi  r30, low(MathSubNormTable  << 1)
        ldi  r31, high(MathSubNormTable  << 1)
        
        add  r30, r20
        clr  r0
        adc  r31, r0
        lpm
        
        
        
        ;add 8
        ldi r17, 8
        add r0, r17
        
UnsignedNormCountFound:

        ;r0 has count
        ;save it
        mov r17, r0
        
        cpi r17, 8
        brlt NormCountSet
        
        ;left shift by 8 
        
        mov  r21, r20
        clr  r20
        subi r17, 8 
         
        

NormCountSet:

            ;now normalize as per count
            dec r17
            brmi ExitNormLoop
        
            clc
            rol r20
            rol r21
            jmp  NormCountSet
        


ExitNormLoop:

        

        ;r17 has norm count
        ;r21:r20 is normalized

UnsignedShortNorm_Ret:

        ret
        
/*****************************************************/
UnsignedShiftLeft:
        ;Input: r21:r20
        ;unsigned 16 bit int
        
        ;output: r21:r20: Left Shifted
        
        ;r0: Left Shift  count
        
        mov r17, r0

        
        and r0, r0
        breq UnsignedShiftLeft_Ret
        
        cpi r17, 8
        brlt UnsignedShiftLeft_CountSet 
        
        mov r21, r20
        clr r20
        subi r17, 8 
        
        
UnsignedShiftLeft_CountSet:
        dec r17
        brmi UnsignedShiftLeft_Ret
        clc
        rol r20
        rol r21
        jmp UnsignedShiftLeft_CountSet
        
        
        
        
        
UnsignedShiftLeft_Ret:
        ret
        
/*****************************************************/
UnsignedShiftLeft32Bit:

        ;Input: r21:r20:r23:r22
        ;unsigned 16 bit int
        
        ;output: r21:r20:r23:r22 Left Shifted
        
        ;r0: Left Shift  count
        
        mov r17, r0

        
        and r0, r0
        breq UnsignedShiftLeft32Bit_Ret
        
        cpi r17, 8
        brlt UnsignedShiftLeft32Bit_CountSet 
        
        mov r21, r20
        mov r20, r23
        mov r23, r22
        clr r22
        subi r17, 8 
        
        
UnsignedShiftLeft32Bit_CountSet:
        dec r17
        brmi UnsignedShiftLeft32Bit_Ret
        clc
        rol r22
        rol r23
        rol r20
        rol r21
        jmp UnsignedShiftLeft32Bit_CountSet
        
UnsignedShiftLeft32Bit_Ret:
        ret        
        
/*****************************************************/
UnsignedDivGetFract:
        ;performs N/D and returns fraction in r21:r20
        ;assumes that N < D
        ;r21:r20 Num
        ;r23:r22 Denom
        
        ;on return r21:r20 holds fraction
                
        ;N is in r21:r20
        ;normalize it
        call UnsignedShortNorm
        
        ;r0 has norm count
        ;r21: has normalized remainder
        
        ;backup r0
        push r0
        
        
        
        ;perform division
        
        call UnsignedShortDiv
        
        
        ;restore r0
        
        ;left Shift by 16 - r0
        pop r0
        
        ldi r17, 16
        sub r0, r17
        neg r0
        
        
        ;Also, remainder from above division is in r21:r20
        ;back it up
        
        ;push r20
        ;push r21
        
        mov r20, r24
        mov r21, r25
        
        
        call UnsignedShiftLeft
        
        
        ret
        
        
        
        
/*****************************************************/
UnsignedFractionalDivision:
        ;Performs a 16 bit division and returns a 32bit 
        ;fractional number in 16.16 format
        
        ;r21:r20   Numerator
        ;r23:r22   Denominator
        
        
        ;on return
        ;result is passed in r23:r22:r21:r20
        
        ;backup Denominator
        push r22
        push r23
        
        
        ;now compute the ratio N/D
        
        
        
        call UnsignedShortDiv
        
        sts (MathTemp1), r24
        sts (MathTemp2), r25
        
        
        
        
        
        ;remainder is in r21:r20
        ;normalize it
        call UnsignedShortNorm
        
        ;r0 has norm count
        ;r21:r20 has normalized remainder
        
        
        ;get fractional part
        ;restore denominator
        pop r23
        pop r22


        
        ;push it again for next use
        push r22
        push r23
        

        ;backup r0
        push r0
        
        ;perform division
        
        call UnsignedShortDiv
        
        
        ;restore r0
        
        ;left Shift by 16 - r0
        pop r0
        
        ldi r17, 16
        sub r0, r17
        neg r0
        
        
        ;backup integer part        
        sts (MathTemp3),  r24
        sts (MathTemp4), r25
        
        
        
        
        ;now get fractional part corresponding to remainder
        
        ;restore denominator
        pop r23
        pop r22

        ;backup norm count
        push r0

        
        call UnsignedDivGetFract
        
        
        mov r23, r21
        mov r22, r20
        
        
        pop r0
        
        lds r20, (MathTemp3)
        lds r21, (MathTemp4)
        
        
        
        
        call UnsignedShiftLeft32Bit
        
        
        ;Finally, restore integer part
        lds r22, (MathTemp1)
        lds r23, (MathTemp2)
        
        
        
        ret
        
        
        

        
        
        

/*****************************************************/
MathSubNormTable:
   .db  0  , 7
   .db  6  , 6
   .db  5  , 5
   .db  5  , 5
   .db  4  , 4
   .db  4  , 4
   .db  4  , 4
   .db  4  , 4
   .db  3  , 3
   .db  3  , 3
   .db  3  , 3
   .db  3  , 3
   .db  3  , 3
   .db  3  , 3
   .db  3  , 3
   .db  3  , 3
   .db  2  , 2
   .db  2  , 2
   .db  2  , 2
   .db  2  , 2
   .db  2  , 2
   .db  2  , 2
   .db  2  , 2
   .db  2  , 2
   .db  2  , 2
   .db  2  , 2
   .db  2  , 2
   .db  2  , 2
   .db  2  , 2
   .db  2  , 2
   .db  2  , 2
   .db  2  , 2
   .db  1  , 1
   .db  1  , 1
   .db  1  , 1
   .db  1  , 1
   .db  1  , 1
   .db  1  , 1
   .db  1  , 1
   .db  1  , 1
   .db  1  , 1
   .db  1  , 1
   .db  1  , 1
   .db  1  , 1
   .db  1  , 1
   .db  1  , 1
   .db  1  , 1
   .db  1  , 1
   .db  1  , 1
   .db  1  , 1
   .db  1  , 1
   .db  1  , 1
   .db  1  , 1
   .db  1  , 1
   .db  1  , 1
   .db  1  , 1
   .db  1  , 1
   .db  1  , 1
   .db  1  , 1
   .db  1  , 1
   .db  1  , 1
   .db  1  , 1
   .db  1  , 1
   .db  1  , 1
   .db  0  , 0
   .db  0  , 0
   .db  0  , 0
   .db  0  , 0
   .db  0  , 0
   .db  0  , 0
   .db  0  , 0
   .db  0  , 0
   .db  0  , 0
   .db  0  , 0
   .db  0  , 0
   .db  0  , 0
   .db  0  , 0
   .db  0  , 0
   .db  0  , 0
   .db  0  , 0
   .db  0  , 0
   .db  0  , 0
   .db  0  , 0
   .db  0  , 0
   .db  0  , 0
   .db  0  , 0
   .db  0  , 0
   .db  0  , 0
   .db  0  , 0
   .db  0  , 0
   .db  0  , 0
   .db  0  , 0
   .db  0  , 0
   .db  0  , 0
   .db  0  , 0
   .db  0  , 0
   .db  0  , 0
   .db  0  , 0
   .db  0  , 0
   .db  0  , 0
   .db  0  , 0
   .db  0  , 0
   .db  0  , 0
   .db  0  , 0
   .db  0  , 0
   .db  0  , 0
   .db  0  , 0
   .db  0  , 0
   .db  0  , 0
   .db  0  , 0
   .db  0  , 0
   .db  0  , 0
   .db  0  , 0
   .db  0  , 0
   .db  0  , 0
   .db  0  , 0
   .db  0  , 0
   .db  0  , 0
   .db  0  , 0
   .db  0  , 0
   .db  0  , 0
   .db  0  , 0
   .db  0  , 0
   .db  0  , 0
   .db  0  , 0
   .db  0  , 0
   .db  0  , 0
   .db  0  , 0


/************************************************************/
    .dseg
MathTemp1:  .byte   1
MathTemp2:  .byte   1
MathTemp3:  .byte   1
MathTemp4:  .byte   1
    
    