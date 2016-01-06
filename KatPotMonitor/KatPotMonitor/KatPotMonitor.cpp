// WirelessMonitor.cpp : main project file.

#include "stdafx.h"
#include <string.h>
#include <string>
#include <vcclr.h>
#include <sstream>
#include <iostream>

#include "KatPotMonitor.h"
using namespace System;
void MyXLStest(String^ OutputFileName);

#include "Form1.h"

using namespace KatPotMonitor;
using namespace std;

using namespace System::Threading;

#define MAX_NUM_SAMPLES  5*1000

int InitSerialPort( SerialPort^ _serialPort, String^ PortName, int NumPorts);
bool GetSerialPortPacket( SerialPort^ _serialPort, array<unsigned char>^ readArray);

#define NUM_CH_TO_SHOW  6
#define PACKET_COUNT_THRESHOLD  15

//void MyXLStest();
gcroot<KatPotMonitor::Form1^> main_form;

gcroot<array<int>^> NumSamples;
//gcroot<array<array<int>^>^> SampleArray;
gcroot<array<array<int>^>^> SampleArray;

void newFormThread()
{
	main_form = gcnew Form1();
	Application::Run(main_form);
}



//[STAThreadAttribute]
[STAThread]
int main(array<System::String ^> ^args)
{
	int Val;
	Thread^ formThread;
	SerialPort^ _serialPort;
	int SuccessCount = 0;
	int DirectionFlag[NUM_CH_TO_SHOW] = { 0, 0, 0, 0, 0, 0};
	int PacketCount[NUM_CH_TO_SHOW] = { 0, 0, 0, 0, 0, 0};
	bool AveragingStarted=false;
	int TestCount=0;

	//Thread^ serialPortReadThread;
	//Thread^ serialPortWriteThread;
	
	array<unsigned char>^ SerialPortReadArray;
	int status = SERIAL_PORT_UNINITIALIZED;
	array<int>^ NewLoadValues;

	// Enabling Windows XP +visual effects before any controls are created
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false); 

	//main_form = gcnew Form1();
	//newThread = gcnew System::Threading::Thread(gcnew ThreadStart(&mytemp));
	//formThread = gcnew System::Threading::Thread(gcnew ParameterizedThreadStart(main_form,&Form1::frmNewFormThread));
	formThread = gcnew System::Threading::Thread(gcnew ThreadStart(newFormThread));
	//newThread = gcnew System::Threading::Thread(gcnew ThreadStart(frmNewFormThread(main_form)));
	//newThread->IsBackground = false;
	
	//allocate memory for array 
	SerialPortReadArray = gcnew array<unsigned char>(SERIAL_PORT_PACKET_SIZE);
	SampleArray = gcnew array<array<int>^>(MAX_NUM_TESTS);
	NumSamples = gcnew array<int> (MAX_NUM_TESTS);
	for (int i = 0; i < MAX_NUM_TESTS; i ++)
		SampleArray[i] = gcnew array<int>(MAX_NUM_SAMPLES);


	NewLoadValues = gcnew array<int>(4);


	formThread->SetApartmentState(ApartmentState::STA);
	formThread->Start();
	//formThread->Join();

	//let the form thread start.
	Thread::Sleep(3000);

	//wait while thread is started
	while (!formThread->IsAlive) {};

	

	//initialize Serial Port
	_serialPort = gcnew SerialPort;

	
	while (1)
	{

		//check if user wants to close it
		if(main_form->UserExitRequested==true)
		{
				//MyXLStest("Trial1");
				formThread->Abort();
				formThread->Join();
		}

		//if form thread is closed just exit
		if (!formThread->IsAlive)
		{
			if (main_form->SerialPortValidated)
			{
				//close serial port read thread
				//serialPortReadThread->Abort();
				//serialPortReadThread->Join();
				_serialPort->Close();
			}
			break;
		}
			
		if (main_form->SerialPortValidated)
		{
			if (GetSerialPortPacket(_serialPort, SerialPortReadArray))
			{
				if ((SerialPortReadArray[5] == 0x0A) && (SerialPortReadArray[4] == 0x0D))
				{
					if ((SerialPortReadArray[0] == 'S') && (SerialPortReadArray[1] == 'T') && \
						(SerialPortReadArray[2] == 'R') && (SerialPortReadArray[3] == 'T'))
					{
							AveragingStarted = true;
							NumSamples[TestCount]=0;
							main_form->ClearChart(TestCount);

					}

					else if ((SerialPortReadArray[0] == 'S') && (SerialPortReadArray[1] == 'T') && \
						(SerialPortReadArray[2] == 'O') && (SerialPortReadArray[3] == 'P'))
					{
							AveragingStarted = false;
							TestCount++;
							if (TestCount == MAX_NUM_TESTS)
								TestCount = 0;
					}
					else if (AveragingStarted)				
					{
						Val =  (SerialPortReadArray[0]  - '0')*1000 + (SerialPortReadArray[1]  - '0')*100 + \
							(SerialPortReadArray[2]  - '0')*10 + (SerialPortReadArray[3]  - '0');
						main_form->UpdateChartPoint(TestCount,Val);
						if (NumSamples[TestCount]< MAX_NUM_SAMPLES)
							SampleArray[TestCount][NumSamples[TestCount]++]=Val;
					}
				}
				else
				{
					status = SERIAL_PORT_SYNC_LOST;
					main_form->SerialPortValidated = false;
					_serialPort->Close();

				}
			}
			else 
			{
				status = SERIAL_PORT_READTIMEOUT;
				main_form->SerialPortValidated = false;
				_serialPort->Close();

			}
		}
		else
		{
			if(_serialPort)
				_serialPort->Close();

		}

		if (formThread->IsAlive)	
		    main_form->UpdateCommStatus(status);

		if (status)
			Thread::Sleep(500);



		if (!main_form->SerialPortValidated)
		{
			if (_serialPort)
				_serialPort->Close();
			//Attempt to connect to serial port
			
			main_form->GetSystemCommPortsAndInitCommMenu();
			status = InitSerialPort(_serialPort, main_form->SelectedCommPort, main_form->NumCommPorts);
			if (status == SERIAL_PORT_SUCCESS)
				main_form->SerialPortValidated = true;
			if (formThread->IsAlive)
				main_form->UpdateCommStatus(status);
			if (status)
				Thread::Sleep(500);
		}

		if (0) //formThread->IsAlive)
		{
			NewLoadValues[0] = 4*Val+0;
			NewLoadValues[1] = 4*Val+1;
			NewLoadValues[2] = 4*Val+2;
			NewLoadValues[3] = 4*Val+3;

			main_form->UpdateChartPoint(0,NewLoadValues[0]);
			main_form->UpdateChartPoint(0,NewLoadValues[1]);
			main_form->UpdateChartPoint(0,NewLoadValues[2]);
			main_form->UpdateChartPoint(0,NewLoadValues[3]);
			Thread::Sleep(1000);
		}


	}

	delete (_serialPort);
	delete SerialPortReadArray;
	return 0;
}
