/************** subroutines to handle unit testing of  functions *****/


    .cseg
    
    
	.if (0)     //Testing of division module

RESET:
   
    ldi r16, high(RAMEND)
	out SPH,r16
	ldi r16, low(RAMEND)
	out SPL,r16

   ldi r20, low(65535)
   ldi r21, high(65535)

   ldi r22, 1
   ldi r23, 0

   call UnsignedShortDiv           
   nop
   nop
   jmp RESET

   .endif


	.if (0)     //Testing of Norm  module

RESET:
   
    ldi r16, high(RAMEND)
	out SPH,r16
	ldi r16, low(RAMEND)
	out SPL,r16

   ldi r20, low(0)
   ldi r21, high(0)


   call UnsignedShortNorm           
   nop
   nop
   jmp RESET

   .endif



   .if (0)      //calibration module test

.EQU TEST_CALIB_VAL=1000
.EQU TEST_GAIN_VAL=567 
   
RESET:

    ldi r16, high(RAMEND)
	out SPH,r16
	ldi r16, low(RAMEND)
	out SPL,r16



CalibTest:

    clr r17
    
    sts (Zero_Offset_RAM), r17
    sts (Zero_Offset_RAM+1), r17
    
    ldi r17, low(TEST_GAIN_VAL)
    sts (Calib_Gain_RAM), r17
    
    ldi r17, high(TEST_GAIN_VAL)
    sts (Calib_Gain_RAM+1), r17
    
    ldi r17, low(TEST_CALIB_VAL)
    sts (Calib_Val_RAM), r17
    
    ldi r17, high(TEST_CALIB_VAL)
    sts (Calib_Val_RAM+1), r17
    
	call Initialize_CalibParams_ComputeRatio


CalibTest_Loop:

	ldi r20, low(1*TEST_GAIN_VAL)
	ldi r21, high(1*TEST_GAIN_VAL)

	call CalibrateADCval
	nop
	jmp  CalibTest

	.endif


	   .if (0)      //UnsignedFractionalDivision test

   
RESET:

    ldi r16, high(RAMEND)
	out SPH,r16
	ldi r16, low(RAMEND)
	out SPL,r16



FractionalDivTest:

    clr r17

	ldi r20, low(0x0001)
	ldi r21, high(0x0001)

	ldi r22, low(0x0001)
	ldi r23, high(0x0001)
    
	call UnsignedFractionalDivision


	nop
	jmp  FractionalDivTest

	.endif

