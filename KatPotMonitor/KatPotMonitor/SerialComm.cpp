
//file to handle serial port communication.

#include "stdafx.h"
#include <string.h>
//#include <random>
#include <iostream>
#include "KatPotMonitor.h"

using namespace System;
using namespace std;
using namespace System::IO::Ports;
using namespace System::Threading;

#define SERIAL_PORT_BAUDRATE_VAL		9600
#define SERIAL_PORT_DATA_BITS_VAL		8
#define SERIAL_PORT_READ_TIMEOUT_VAL    500
#define SERIAL_PORT_WRITE_TIMEOUT_VAL   500
#define SERIAL_PORT_READ_TIMEOUT_SYNC_VAL   1000
#define MAX_SYNC_BYTES    50

//#define EMULATE_SERIAL_PORT     1

int InitSerialPort( SerialPort^ _serialPort, String^ PortName, int NumPorts, int TestSelected)
{

#if defined(EMULATE_SERIAL_PORT)
	return SERIAL_PORT_SUCCESS ;
#endif

	array<unsigned char>^ cTemp;
	int iTemp;

	bool Synced;

	if (!NumPorts)
	{
		return SERIAL_PORT_NOT_DETECTED;
	}

	// Allow the user to set the appropriate properties.
	_serialPort->PortName		= PortName;
    _serialPort->BaudRate		= SERIAL_PORT_BAUDRATE_VAL;
    _serialPort->Parity			= System::IO::Ports::Parity::None;
    _serialPort->DataBits		= SERIAL_PORT_DATA_BITS_VAL;
    _serialPort->StopBits		= System::IO::Ports::StopBits::One;
    _serialPort->Handshake		= System::IO::Ports::Handshake::None;
    _serialPort->ReadTimeout	= SERIAL_PORT_READ_TIMEOUT_VAL;
    _serialPort->WriteTimeout	= SERIAL_PORT_WRITE_TIMEOUT_VAL;

	cTemp = gcnew array<unsigned char>(6);
	cTemp[0] = 'E';
	cTemp[1] = 'S';
	cTemp[2] = 'C';
	cTemp[3] = 'W';
	cTemp[4] = 'M';
	cTemp[5] = 0;


	try {
		_serialPort->Open();
	}
	catch (ArgumentException ^) {

		delete cTemp;
		return SERIAL_PORT_INVALID_NAME_ERROR;

	}
	catch (UnauthorizedAccessException ^) {
		delete cTemp;
		return SERIAL_PORT_AUTHORIZATION_ERROR;

	}
	//No need for ArgumentOutOfRangeException.
	//This is same as ArgumentException
	//catch (ArgumentOutOfRangeException ^) {
	//	delete cTemp;
	//	return SERIAL_PORT_ARGUMENT_ERROR;
    //}
	catch (System::IO::IOException ^) {
		delete cTemp;
		return SERIAL_PORT_IO_ERROR;	
	}
	catch (InvalidOperationException ^) {

		delete cTemp;
		return SERIAL_PORT_ALREADY_OPEN_ERROR;
	}

	
	//_serialPort->DiscardInBuffer();
	//_serialPort->DiscardOutBuffer();
	

	//send a secret message to the device and wait for the proper response.
   // _serialPort->ReadTimeout	= SERIAL_PORT_READ_TIMEOUT_SYNC_VAL;


	//try {
	//	_serialPort->Write(cTemp,0,6);
	//}
	//catch (TimeoutException^ ex) {
	//	delete cTemp;
	//	_serialPort->Close();
	//	return SERIAL_PORT_WRITE_TIMEOUT;
	//}

	//now read back the response

	
	Synced = false;
	iTemp = 0; 


	while (!Synced)
	{

		try {
			cTemp[0] = _serialPort->ReadByte();
			
		}
		catch (TimeoutException^ ex) {
			delete cTemp;
			_serialPort->Close();
			return SERIAL_PORT_READTIMEOUT;
		}

		
		if (TestSelected == 0) //peel test
		{
			if ((cTemp[1] == 0x0D) && (cTemp[0] == 0x0A)&&(iTemp > 6))
			{
				Synced = true;
				break;
			} else {
				//shift delay line
				cTemp[1] = cTemp[0];
				cTemp[2] = cTemp[1];
				cTemp[3] = cTemp[2];
				cTemp[4] = cTemp[3];
				cTemp[5] = cTemp[4];

			}

			iTemp++;
			if (iTemp >= MAX_SYNC_BYTES)
				break;
		}
	
		else {
			//shear test
			if ((cTemp[2] == 'T') && (cTemp[1] == '1')&& (cTemp[0] == '='))
			{
				int i;
				Synced = true;
				//skipnext 8 + 11 + 11 bytes
				for (i=0;i<8+11+11;i++)
				{
					try {
						cTemp[0] = _serialPort->ReadByte();
			
					}
					catch (TimeoutException^ ex) {
						Synced = false;
						break;
					}

				}
				break;


			} else {
				//shift delay line
				cTemp[1] = cTemp[0];
				cTemp[2] = cTemp[1];
				cTemp[3] = cTemp[2];
				cTemp[4] = cTemp[3];
				cTemp[5] = cTemp[4];

			}

			iTemp++;
			if (iTemp >= MAX_SYNC_BYTES)
				break;
		}

	}


			
	delete cTemp;
	if (Synced)
			return SERIAL_PORT_SUCCESS;
	else {			
		_serialPort->Close();
		return SERIAL_PORT_INVALID_RESPONSE;
	}

}

bool GetSerialPortPacket( SerialPort^ _serialPort, array<unsigned char>^ readArray, int TestSelected)
{

#if defined(EMULATE_SERIAL_PORT)

	static int count = 0;
	//random_device rd();
	//mt19937 gen(rd());
	static int l=0;
	if (TestSelected==0)
	{
		if (count==100)
		{
			readArray[0] = 'S';
			readArray[1] = 'T';
			readArray[2] = 'R';
			readArray[3] = 'T';
			readArray[4] = '\r';
			readArray[5] = '\n';
		}
		else if (count==200)
		{
			readArray[0] = 'S';
			readArray[1] = 'T';
			readArray[2] = 'O';
			readArray[3] = 'P';
			readArray[4] = '\r';
			readArray[5] = '\n';
			count = 0;
		}
		else
		{
			int k = l;
			if (k < 0) k = -k;
			k = k % 10000;
			readArray[0] = '0' + (k/1000);
			k = k % 1000;
			readArray[1] = '0' + (k/100);
			k = k % 100;
			readArray[2] = '0' + (k/10);
			k = k % 10;
			readArray[3] = '0' + k;
			readArray[4] = '\r';
			readArray[5] = '\n';
		}
		count++;
		l++;
		Thread::Sleep(100);
		return true;
	}
	else if (TestSelected==1)
	{

		int m;
		int offset = 0;
		for (m=0;m<3;m++)
		{

			int k = l;
			if (k < 0) k = -k;

			readArray[offset++] = 'T';
			readArray[offset++] = '1' + m;
			readArray[offset++] = '=';

			k = k % 1000000;
			readArray[offset++] = '0' + (k/100000);
			k = k % 100000;
			readArray[offset++] = '0' + (k/10000);
			k = k % 10000;
			readArray[offset++] = '0' + (k/1000);
			k = k % 1000;
			readArray[offset++] = '0' + (k/100);
			k = k % 100;
			readArray[offset++] = '0' + (k/10);
			k = k % 10;
			readArray[offset++] = '0' + (k);

			readArray[offset++] = '\r';
			readArray[offset++] = '\n';

			l = l + 10;
		}

		return true;

	}
	else
	{
		return false;
	}



#else

	int BytesRead = 0;
	int BytesToRead;

	if (TestSelected)
		BytesToRead = 11*3;
	else
		BytesToRead = SERIAL_PORT_PACKET_SIZE;



	while (BytesRead < BytesToRead)
	{

		try {
			BytesRead += _serialPort->Read(readArray, BytesRead, BytesToRead - BytesRead );
			
		}
		catch (TimeoutException^ ex) {
			return false;
		}
	}


	return true;
#endif

}






	