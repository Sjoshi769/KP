/*
 * dataStructures
 *
 *  Created: 04/12/2014 13:27:12
 *   Author: Sanjay
 */ 

.equ	USB_PACKET_SIZE=6
.equ	UART_TX_BUFF_SIZE=2*USB_PACKET_SIZE*MAX_NUM_PIPES
.equ	UART_RX_BUFF_SIZE=2*USB_PACKET_SIZE
 
	.dseg

	

/* Critical order. Do not re-arrange  */
	.if (board_config_mode==TX_MODE)
TxBuff:					.byte	NRF_PAYLOAD_SIZE*NUM_PAYLOADS_PER_BUFF
TxBuffEnd:
	.endif

	.if (board_config_mode==RX_MODE)
RxBuff:					.byte	NRF_PAYLOAD_SIZE*NUM_PAYLOADS_PER_BUFF*MAX_NUM_PIPES
RxBuffEnd:

UARTBuff:
UARTRxBuff:				.byte	(UART_RX_BUFF_SIZE)
UARTRxBuffEnd:
UARTTxBuff:				.byte	(UART_TX_BUFF_SIZE)
UARTTxBuffEnd:
UARTBuffEnd:

	.endif

	.if (board_config_mode==TX_MODE)
TxBuffRdPtr:			.dw		(TxBuff)
TxBuffWrPtr:			.dw		(TxBuff)
	.endif

	.if (board_config_mode==RX_MODE)

/*************** WARNING *******************************/
/**** CRITICAL ORDER. DO NOT RE-ARRANGE ****************/
RxBuffStartPtr:			.byte   (MAX_NUM_PIPES*2)
RxBuffRdPtr:			.byte   (MAX_NUM_PIPES*2)
RxBuffWrPtr:			.byte   (MAX_NUM_PIPES*2)
RxBuffEndPtr:			.byte   (MAX_NUM_PIPES*2)
UARTRxBuffRdPtr:		.byte   (2)
UARTRxBuffWrPtr:		.byte   (2)
UARTTxBuffRdPtr:		.byte   (2)
UARTTxBuffWrPtr:		.byte   (2)
/************* CRITICAL ORDER ENDS  ******************/

	.endif

	.if (board_config_mode==TX_MODE)
TxBuffClearStart:
TxBuffDipstick:			.db		(0)
TransmitterBusy:		.db		(0)
TimeStampCnt:			.dw     (0)
TxBuffRdPtrTemp:		.dw		0
BuffOverflowCnt:        .dw		(0)
TransmitterBackOffOn:	.dw		(0)
TxRetrialCount:			.dw		(0)
TxBuffClearEnd:


	.endif

RandomSeed:				.db		0


	.if (board_config_mode==RX_MODE)
;;;; NOTE. CRITICAL ORDER. DO NOT RE-ARRANGE ;;;;
RxVarsStart:
RxBuffDipstick:			.byte	(MAX_NUM_PIPES)
LastPacketnum:			.byte	(2*MAX_NUM_PIPES)
PacketErrorCount:		.byte	(2*MAX_NUM_PIPES)
PacketReceiveCount:		.byte	(2*MAX_NUM_PIPES)
RxLinkActiveCount:		.byte	(MAX_NUM_PIPES)
RxPipeNumber:			.db		0
RxPipeCounter:			.db		0
RxLinkActiveIndicator:	.db		0
RxVarsEnd:

UARTVarsStart:
UARTRxDipstick:			.dw		0
UARTTxDipstick:			.dw		0
UARTInsertQueryResponse:.db		0
UARTTxTriggerNeeded:	.db		1
UARTVarsEnd:
;;;; CRITICAL ORDER ENDS. RE-ARRANGE ALLOWED ;;;;
	.endif


/************************************************************/
/*** These are constants and needs to be placed in EEPROM ***/

	.eseg
	.org 0
;Trasnsmitter address. This needs to be unique for each transmitter.
;Use unique value of TX_PIPE_NUM for each transmitter.

 	.if (board_config_mode==TX_MODE)
TxRxPipeAddress:		.db  	(ADDR_BYTE0 + TX_PIPE_NUM*30)
	.endif

 	.if (board_config_mode==RX_MODE)
TxRxPipeAddress:		.db  	0
	.endif

						.db		ADDR_BYTE1
						.db		ADDR_BYTE2
						.db		ADDR_BYTE3
						.db		ADDR_BYTE4
						

RandomSeedInit:			.db		(ADDR_BYTE4 ^ (TX_PIPE_NUM*30 + 1))

