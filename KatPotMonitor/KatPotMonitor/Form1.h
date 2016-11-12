#pragma once
#define MAX_COMM_PORTS_IN_SYSTEM   10


namespace KatPotMonitor {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Windows::Forms::DataVisualization;
	using namespace System::Data;
	using namespace System::Drawing;
	using namespace System::IO;
	using namespace System::IO::Ports;
	using namespace std;


	/// <summary>
	/// Summary for Form1
	/// </summary>
	public ref class Form1 : public System::Windows::Forms::Form
	{
	public:
		Form1(void)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//
		}
	public:
	void frmNewFormThread(Object^ data)
	{
		//Forms2::Form1^ main_form;
		//main_form = gcnew Form1();
		Application::Run(this);
		//return main_form;
	}

	delegate void ClearChartDelegate(int index);

	public: 
	void ClearChart(int index)
	{
		if (this->chart1->InvokeRequired)
		{
            ClearChartDelegate^ d = 
                gcnew ClearChartDelegate(this, &Form1::ClearChart);
            this->Invoke(d,index);

		}
		else
		{
			this->series1[index]->Points->Clear();
		}
	}


	delegate void UpdateCommStatusDelegate(int status);

	public:
	void UpdateCommStatus(int status)
	{
		String^ DisplayMessageString;

				
		if (this->label1->InvokeRequired) {

            UpdateCommStatusDelegate^ d = 
                gcnew UpdateCommStatusDelegate(this, &Form1::UpdateCommStatus);
            this->Invoke(d, status);


		}
		else {

			switch (status)
			{

			case 	SERIAL_PORT_SUCCESS:
				{
					//this->label43->Text =  ;
					DisplayMessageString = gcnew  String {L"Connected To Comm Port " + SelectedCommPort + L" ." };
					break;
				}

		
			case 	SERIAL_PORT_UNINITIALIZED:
				{
					//this->label43->Text =  ;
					DisplayMessageString = gcnew  String {L"Serial Port is Not Initialized" };
					break;
				}

			case 	SERIAL_PORT_WRITE_TIMEOUT:
				{
					DisplayMessageString = gcnew  String {L"Write To Comm Port " + SelectedCommPort + L" Timed Out."} ;
					break;
				}

			case 	SERIAL_PORT_READTIMEOUT:
				{
					DisplayMessageString = gcnew  String {L"Read From Comm Port " + SelectedCommPort + L" Timed Out." };
					break;
				}

			case 	SERIAL_PORT_INVALID_RESPONSE:
				{
					DisplayMessageString = gcnew  String {L"Device Connected To Comm Port " + SelectedCommPort + L" Is Not WirelessMonitor Receiver."} ;
					break;
				}

			case 	SERIAL_PORT_NOT_DETECTED:
				{
					DisplayMessageString = gcnew  String {L"No valid Comm Port Detected. Please Check The Connection."};
					break;
				}

			case SERIAL_PORT_AUTHORIZATION_ERROR:
				{
					DisplayMessageString = gcnew  String {L"Access To Comm Port " + SelectedCommPort + L" Denied."} ;
					break;
				}

			case SERIAL_PORT_INVALID_NAME_ERROR:
				{
					DisplayMessageString = gcnew  String {L"Port Name " + SelectedCommPort + L" Is Invalid." };
					break;
				}

			case SERIAL_PORT_ARGUMENT_ERROR:
				{
					DisplayMessageString = gcnew  String {L"Invalid Configuration Parameters Specified."} ;
					break;
				}
			case SERIAL_PORT_IO_ERROR:

				{
					DisplayMessageString = gcnew  String {L"Comm Port " + SelectedCommPort + L" Is In Invalid State." };
					break;
				}

			case SERIAL_PORT_ALREADY_OPEN_ERROR:
				{
					DisplayMessageString = gcnew  String {"Comm Port " + SelectedCommPort + L" Is Already In Use."} ;
					break;
				}

			case SERIAL_PORT_SYNC_LOST:
				{
					DisplayMessageString = gcnew  String {"Serial Port Synchronization Lost. Re-attempting"} ;
					break;
				}

			}
			this->label1->Text = DisplayMessageString;
			delete DisplayMessageString;
		}

		//make a thread safe call to controls.

	}

	delegate void GetSystemCommPortsAndInitCommMenuDelegate(void);

	public: 
	void GetSystemCommPortsAndInitCommMenu()
	{

		if (this->InvokeRequired)
		{
			GetSystemCommPortsAndInitCommMenuDelegate^ d = 
				gcnew GetSystemCommPortsAndInitCommMenuDelegate(this, &Form1::GetSystemCommPortsAndInitCommMenu);
			this->Invoke(d);
			return;
		}
		else {

		int temp;

		    
		try
		{
			if (SystemserialPorts)
				delete SystemserialPorts;
			// Get a list of serial port names.
			SystemserialPorts = SerialPort::GetPortNames();
		}
		catch (Win32Exception^ ex)
		{
			Console::WriteLine(ex->Message);
		}

		//hack
		//SystemserialPorts =  gcnew array<String^>(11) {"0",  L"COM1",L"2",L"3","4","5","6","7","8","9","10"};

		//Console::WriteLine("The following serial ports were found:");
		NumCommPorts = 0;

		// Display each port name to the console. 
		for each(String^ port in SystemserialPorts)
		{
			//Console::WriteLine(port);
			NumCommPorts++;
		}

		
		if (NumCommPorts > MAX_COMM_PORTS_IN_SYSTEM)
			NumCommPorts = MAX_COMM_PORTS_IN_SYSTEM;

		if (COMToolStripMenuItemRuntimeArray)
			delete COMToolStripMenuItemRuntimeArray;

		//if (this->toolStripMenuItem2->DropDownItems)
		//	delete (this->toolStripMenuItem2->DropDownItems);

		COMToolStripMenuItemRuntimeArray =  gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(NumCommPorts);
			
		for (temp = 0; temp < NumCommPorts; temp++)
		{
			
			if (temp == 0) {
				// 
				// cOM1ToolStripMenuItem
				// 
				this->cOM1ToolStripMenuItem->Name = L"cOM1ToolStripMenuItem";
				this->cOM1ToolStripMenuItem->Size = System::Drawing::Size(114, 22);
				this->cOM1ToolStripMenuItem->Text = SystemserialPorts[0];
				this->cOM1ToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::cOM1ToolStripMenuItem_Click);
			}			
			if (temp == 1) {
				// 
				// cOM2ToolStripMenuItem
				// 
				this->cOM2ToolStripMenuItem->Name = L"cOM2ToolStripMenuItem";
				this->cOM2ToolStripMenuItem->Size = System::Drawing::Size(114, 22);
				this->cOM2ToolStripMenuItem->Text = SystemserialPorts[1];
				this->cOM2ToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::cOM2ToolStripMenuItem_Click);
			}
			if (temp == 2) {
				// 
				// cOM3ToolStripMenuItem
				// 
				this->cOM3ToolStripMenuItem->Name = L"cOM3ToolStripMenuItem";
				this->cOM3ToolStripMenuItem->Size = System::Drawing::Size(114, 22);
				this->cOM3ToolStripMenuItem->Text = SystemserialPorts[2];
				this->cOM3ToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::cOM3ToolStripMenuItem_Click);
			}
			if (temp == 3) {
				// 
				// cOM4ToolStripMenuItem
				// 
				this->cOM4ToolStripMenuItem->Name = L"cOM4ToolStripMenuItem";
				this->cOM4ToolStripMenuItem->Size = System::Drawing::Size(114, 22);
				this->cOM4ToolStripMenuItem->Text = SystemserialPorts[3];
				this->cOM4ToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::cOM4ToolStripMenuItem_Click);
			}
			if (temp == 4) {
				// 
				// cOM5ToolStripMenuItem
				// 
				this->cOM5ToolStripMenuItem->Name = L"cOM5ToolStripMenuItem";
				this->cOM5ToolStripMenuItem->Size = System::Drawing::Size(114, 22);
				this->cOM5ToolStripMenuItem->Text = SystemserialPorts[4];
				this->cOM5ToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::cOM5ToolStripMenuItem_Click);
			}
			if (temp == 5) {
				// 
				// cOM6ToolStripMenuItem
				// 
				this->cOM6ToolStripMenuItem->Name = L"cOM6ToolStripMenuItem";
				this->cOM6ToolStripMenuItem->Size = System::Drawing::Size(114, 22);
				this->cOM6ToolStripMenuItem->Text = SystemserialPorts[5];
				this->cOM6ToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::cOM6ToolStripMenuItem_Click);
			}
			if (temp == 6) {
				// 
				// cOM7ToolStripMenuItem
				// 
				this->cOM7ToolStripMenuItem->Name = L"cOM7ToolStripMenuItem";
				this->cOM7ToolStripMenuItem->Size = System::Drawing::Size(114, 22);
				this->cOM7ToolStripMenuItem->Text = SystemserialPorts[6];
				this->cOM7ToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::cOM7ToolStripMenuItem_Click);
			}
			if (temp == 7) {
				// 
				// cOM8ToolStripMenuItem
				//
				this->cOM8ToolStripMenuItem->Name = L"cOM8ToolStripMenuItem";
				this->cOM8ToolStripMenuItem->Size = System::Drawing::Size(114, 22);
				this->cOM8ToolStripMenuItem->Text = SystemserialPorts[7];
				this->cOM8ToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::cOM8ToolStripMenuItem_Click);
			}
			if (temp == 8) {
				// 
				// cOM9ToolStripMenuItem
				// 
				this->cOM9ToolStripMenuItem->Name = L"cOM9ToolStripMenuItem";
				this->cOM9ToolStripMenuItem->Size = System::Drawing::Size(114, 22);
				this->cOM9ToolStripMenuItem->Text = SystemserialPorts[8];
				this->cOM9ToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::cOM9ToolStripMenuItem_Click);
			}
			if (temp == 9) {
				// 
				// cOM10ToolStripMenuItem
				// 
				this->cOM10ToolStripMenuItem->Name = L"cOM10ToolStripMenuItem";
				this->cOM10ToolStripMenuItem->Size = System::Drawing::Size(114, 22);
				this->cOM10ToolStripMenuItem->Text = SystemserialPorts[9];
				this->cOM10ToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::cOM10ToolStripMenuItem_Click);
			}

			//this->toolStripMenuItem2->DropDownItems->Add(COMToolStripMenuItemArray[temp]);
			COMToolStripMenuItemRuntimeArray[temp] = COMToolStripMenuItemArray[temp];
		
			}
		}

	}


	delegate void UpdateChartPointDelegate(int SeriesIndex, int Xval, int Yval);

	public:
	void UpdateChartPoint(int SeriesIndex, int Xval, int Yval)
	{
		if (this->chart1->InvokeRequired)
		{
			UpdateChartPointDelegate^ d = 
				gcnew UpdateChartPointDelegate(this, &Form1::UpdateChartPoint);
			this->Invoke(d,SeriesIndex, Xval, Yval);
			return;
		}
		else {
			this->series1[SeriesIndex]->Points->AddXY(Xval,Yval);
		}
	}







	public: bool SerialPortValidated;
	private: static array<String^>^ SystemserialPorts = nullptr;
	public:  static int NumCommPorts = 0;
	private: static bool ComPortConnected = false;
	public:  static String^ SelectedCommPort = L"        ";
	public:  bool   UserExitRequested;
	public: static int TestSelected=1;
	public: static int DegreeSelected=0;



	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~Form1()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::MenuStrip^  menuStrip1;
	protected: 
	private: System::Windows::Forms::ToolStripMenuItem^  fileOptionsToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  loadFileToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  saveFileToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  cOMPortToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  cOM1ToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  cOM2ToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  cOM3ToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  cOM4ToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  cOM5ToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  cOM6ToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  cOM7ToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  cOM8ToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  cOM9ToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  cOM10ToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  GridOptionsToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  GridOnToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  GridOffToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  captureOptionsToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  autoCaptureToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  manualCaptureToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  testTypeToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  shearTestToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  peelTestToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  testDegreeToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  degreeToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  degreeToolStripMenuItem1;
	private: System::Windows::Forms::ToolStripMenuItem^  testSetupToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  customerNameToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  itemNameToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  itemNoToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  drawingNoToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  manufacturingDateToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  inspectionDateToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  batchNoToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  materialTypeToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  widthToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  colorToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  weightAppliedToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  minimumLengthToolStripMenuItem;
	public:  System::Windows::Forms::ToolStripTextBox^  toolStripTextBox1;
	public:  System::Windows::Forms::ToolStripTextBox^  toolStripTextBox2;
	public:  System::Windows::Forms::ToolStripTextBox^  toolStripTextBox3;
	public:  System::Windows::Forms::ToolStripTextBox^  toolStripTextBox4;
	public:  System::Windows::Forms::ToolStripTextBox^  toolStripTextBox5;
	public:  System::Windows::Forms::ToolStripTextBox^  toolStripTextBox6;
	public:  System::Windows::Forms::ToolStripTextBox^  toolStripTextBox7;
	public:  System::Windows::Forms::ToolStripTextBox^  toolStripTextBox8;
	public:  System::Windows::Forms::ToolStripTextBox^  toolStripTextBox9;
	public:  System::Windows::Forms::ToolStripTextBox^  toolStripTextBox10;
	public:  System::Windows::Forms::ToolStripTextBox^  toolStripTextBox11;
	public:  System::Windows::Forms::ToolStripTextBox^  toolStripTextBox12;
	private: System::Windows::Forms::ToolStripMenuItem^  exitToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  exitProgramToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  returnToProgramToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  helpToolStripMenuItem;
	private: System::Windows::Forms::DataVisualization::Charting::Chart^  chart1;
	private: System::Windows::Forms::Label^  label1;
	private: array<System::Windows::Forms::ToolStripItem^>^ COMToolStripMenuItemArray;
	private: array<System::Windows::Forms::ToolStripItem^>^ COMToolStripMenuItemRuntimeArray;
	private: System::ComponentModel::IContainer^  components;
	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
			System::Windows::Forms::DataVisualization::Charting::ChartArea^  chartArea1;
			System::Windows::Forms::DataVisualization::Charting::Legend^  legend1;
			array<System::Windows::Forms::DataVisualization::Charting::Series^>^  series1;



#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			chartArea1 = (gcnew System::Windows::Forms::DataVisualization::Charting::ChartArea());
			legend1 = (gcnew System::Windows::Forms::DataVisualization::Charting::Legend());
			series1 = gcnew array<System::Windows::Forms::DataVisualization::Charting::Series^>(MAX_NUM_TESTS);

			series1[0] = (gcnew System::Windows::Forms::DataVisualization::Charting::Series());
			series1[1] = (gcnew System::Windows::Forms::DataVisualization::Charting::Series());
			series1[2] = (gcnew System::Windows::Forms::DataVisualization::Charting::Series());


			this->menuStrip1 = (gcnew System::Windows::Forms::MenuStrip());
			this->fileOptionsToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->loadFileToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->saveFileToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->cOMPortToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->cOM1ToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->cOM2ToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->cOM3ToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->cOM4ToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->cOM5ToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->cOM6ToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->cOM7ToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->cOM8ToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->cOM9ToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->cOM10ToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->GridOptionsToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->GridOnToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->GridOffToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->captureOptionsToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->autoCaptureToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->manualCaptureToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->testTypeToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->shearTestToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->peelTestToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->testDegreeToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->degreeToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->degreeToolStripMenuItem1 = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->testSetupToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->customerNameToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripTextBox1 = (gcnew System::Windows::Forms::ToolStripTextBox());
			this->itemNameToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripTextBox2 = (gcnew System::Windows::Forms::ToolStripTextBox());
			this->itemNoToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripTextBox3 = (gcnew System::Windows::Forms::ToolStripTextBox());
			this->drawingNoToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripTextBox4 = (gcnew System::Windows::Forms::ToolStripTextBox());
			this->manufacturingDateToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripTextBox5 = (gcnew System::Windows::Forms::ToolStripTextBox());
			this->inspectionDateToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripTextBox6 = (gcnew System::Windows::Forms::ToolStripTextBox());
			this->batchNoToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripTextBox7 = (gcnew System::Windows::Forms::ToolStripTextBox());
			this->materialTypeToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripTextBox8 = (gcnew System::Windows::Forms::ToolStripTextBox());
			this->widthToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripTextBox9 = (gcnew System::Windows::Forms::ToolStripTextBox());
			this->colorToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripTextBox10 = (gcnew System::Windows::Forms::ToolStripTextBox());
			this->weightAppliedToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripTextBox11 = (gcnew System::Windows::Forms::ToolStripTextBox());
			this->minimumLengthToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripTextBox12 = (gcnew System::Windows::Forms::ToolStripTextBox());
			this->exitToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->exitProgramToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->returnToProgramToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->helpToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->chart1 = (gcnew System::Windows::Forms::DataVisualization::Charting::Chart());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->menuStrip1->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->chart1))->BeginInit();
			this->SuspendLayout();
			// 
			// menuStrip1
			// 
			this->menuStrip1->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(9) {this->fileOptionsToolStripMenuItem, 
				this->cOMPortToolStripMenuItem, this->GridOptionsToolStripMenuItem, this->captureOptionsToolStripMenuItem, this->testTypeToolStripMenuItem, 
				this->testDegreeToolStripMenuItem, this->testSetupToolStripMenuItem, this->exitToolStripMenuItem, this->helpToolStripMenuItem});
			this->menuStrip1->Location = System::Drawing::Point(0, 0);
			this->menuStrip1->Name = L"menuStrip1";
			this->menuStrip1->Size = System::Drawing::Size(1045, 24);
			this->menuStrip1->TabIndex = 0;
			this->menuStrip1->Text = L"menuStrip1";
			// 
			// fileOptionsToolStripMenuItem
			// 
			this->fileOptionsToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(2) {this->loadFileToolStripMenuItem, 
				this->saveFileToolStripMenuItem});
			this->fileOptionsToolStripMenuItem->Name = L"fileOptionsToolStripMenuItem";
			this->fileOptionsToolStripMenuItem->Size = System::Drawing::Size(82, 20);
			this->fileOptionsToolStripMenuItem->Text = L"File Options";
			// 
			// loadFileToolStripMenuItem
			// 
			this->loadFileToolStripMenuItem->Name = L"loadFileToolStripMenuItem";
			this->loadFileToolStripMenuItem->Size = System::Drawing::Size(121, 22);
			this->loadFileToolStripMenuItem->Text = L"Load File";
			this->loadFileToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::loadFileToolStripMenuItem_Click);
			// 
			// saveFileToolStripMenuItem
			// 
			this->saveFileToolStripMenuItem->Name = L"saveFileToolStripMenuItem";
			this->saveFileToolStripMenuItem->Size = System::Drawing::Size(121, 22);
			this->saveFileToolStripMenuItem->Text = L"Save File";
			this->saveFileToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::saveFileToolStripMenuItem_Click);
			// 
			// cOMMPortToolStripMenuItem
			// 
			this->cOMPortToolStripMenuItem->Name = L"cOMPortToolStripMenuItem";
			this->cOMPortToolStripMenuItem->Size = System::Drawing::Size(72, 20);
			this->cOMPortToolStripMenuItem->Text = L"COM Port";
			COMToolStripMenuItemArray = gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(10) {this->cOM1ToolStripMenuItem,
				this->cOM2ToolStripMenuItem, this->cOM3ToolStripMenuItem, this->cOM4ToolStripMenuItem, this->cOM5ToolStripMenuItem, this->cOM6ToolStripMenuItem, 
				this->cOM7ToolStripMenuItem, this->cOM8ToolStripMenuItem, this->cOM9ToolStripMenuItem, this->cOM10ToolStripMenuItem};
			
			this->cOMPortToolStripMenuItem->DropDownItems->AddRange(COMToolStripMenuItemArray);
			// 
			// cOM1ToolStripMenuItem
			// 

			this->cOM1ToolStripMenuItem->Name = L"cOM1ToolStripMenuItem";
			this->cOM1ToolStripMenuItem->Size = System::Drawing::Size(152, 22);
			// 
			// cOM2ToolStripMenuItem
			// 
			this->cOM2ToolStripMenuItem->Name = L"cOM2ToolStripMenuItem";
			this->cOM2ToolStripMenuItem->Size = System::Drawing::Size(152, 22);
			// 
			// cOM3ToolStripMenuItem
			// 
			this->cOM3ToolStripMenuItem->Name = L"cOM3ToolStripMenuItem";
			this->cOM3ToolStripMenuItem->Size = System::Drawing::Size(152, 22);
			// 
			// cOM4ToolStripMenuItem
			// 
			this->cOM4ToolStripMenuItem->Name = L"cOM4ToolStripMenuItem";
			this->cOM4ToolStripMenuItem->Size = System::Drawing::Size(152, 22);
			// 
			// cOM5ToolStripMenuItem
			// 
			this->cOM5ToolStripMenuItem->Name = L"cOM5ToolStripMenuItem";
			this->cOM5ToolStripMenuItem->Size = System::Drawing::Size(152, 22);
			// 
			// cOM6ToolStripMenuItem
			// 
			this->cOM6ToolStripMenuItem->Name = L"cOM6ToolStripMenuItem";
			this->cOM6ToolStripMenuItem->Size = System::Drawing::Size(152, 22);
			// 
			// cOM7ToolStripMenuItem
			// 
			this->cOM7ToolStripMenuItem->Name = L"cOM7ToolStripMenuItem";
			this->cOM7ToolStripMenuItem->Size = System::Drawing::Size(152, 22);
			// 
			// cOM8ToolStripMenuItem
			// 
			this->cOM8ToolStripMenuItem->Name = L"cOM8ToolStripMenuItem";
			this->cOM8ToolStripMenuItem->Size = System::Drawing::Size(152, 22);
			// 
			// cOM9ToolStripMenuItem
			// 
			this->cOM9ToolStripMenuItem->Name = L"cOM9ToolStripMenuItem";
			this->cOM9ToolStripMenuItem->Size = System::Drawing::Size(152, 22);
			// 
			// cOM10ToolStripMenuItem
			// 
			this->cOM10ToolStripMenuItem->Name = L"cOM10ToolStripMenuItem";
			this->cOM10ToolStripMenuItem->Size = System::Drawing::Size(152, 22);
			// 
			// GridOptionsToolStripMenuItem
			// 
			this->GridOptionsToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(2) {this->GridOnToolStripMenuItem, 
				this->GridOffToolStripMenuItem});
			this->GridOptionsToolStripMenuItem->Name = L"gridOptionsToolStripMenuItem";
			this->GridOptionsToolStripMenuItem->Size = System::Drawing::Size(86, 20);
			this->GridOptionsToolStripMenuItem->Text = L"Grid Options";
			// 
			// gridOnToolStripMenuItem
			// 
			this->GridOnToolStripMenuItem->Name = L"gridOnToolStripMenuItem";
			this->GridOnToolStripMenuItem->Size = System::Drawing::Size(116, 22);
			this->GridOnToolStripMenuItem->Text = L"Grid On";
			this->GridOnToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::GridOnToolStripMenuItem_Click);
			// 
			// gridOffToolStripMenuItem
			// 
			this->GridOffToolStripMenuItem->Name = L"gridOffToolStripMenuItem";
			this->GridOffToolStripMenuItem->Size = System::Drawing::Size(116, 22);
			this->GridOffToolStripMenuItem->Text = L"Grid Off";
			this->GridOffToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::GridOffToolStripMenuItem_Click);
			this->GridOffToolStripMenuItem->Checked = true;
			// 
			// captureOptionsToolStripMenuItem
			// 
			this->captureOptionsToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(2) {this->autoCaptureToolStripMenuItem, 
				this->manualCaptureToolStripMenuItem});
			this->captureOptionsToolStripMenuItem->Name = L"captureOptionsToolStripMenuItem";
			this->captureOptionsToolStripMenuItem->Size = System::Drawing::Size(106, 20);
			this->captureOptionsToolStripMenuItem->Text = L"Capture Options";
			// 
			// autoCaptureToolStripMenuItem
			// 
			this->autoCaptureToolStripMenuItem->Name = L"autoCaptureToolStripMenuItem";
			this->autoCaptureToolStripMenuItem->Size = System::Drawing::Size(159, 22);
			this->autoCaptureToolStripMenuItem->Text = L"Auto Capture";
			// 
			// manualCaptureToolStripMenuItem
			// 
			this->manualCaptureToolStripMenuItem->Name = L"manualCaptureToolStripMenuItem";
			this->manualCaptureToolStripMenuItem->Size = System::Drawing::Size(159, 22);
			this->manualCaptureToolStripMenuItem->Text = L"Manual Capture";
			// 
			// testTypeToolStripMenuItem
			// 
			this->testTypeToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(2) {this->shearTestToolStripMenuItem, 
				this->peelTestToolStripMenuItem});
			this->testTypeToolStripMenuItem->Name = L"testTypeToolStripMenuItem";
			this->testTypeToolStripMenuItem->Size = System::Drawing::Size(70, 20);
			this->testTypeToolStripMenuItem->Text = L"Test Type";
			// 
			// shearTestToolStripMenuItem
			// 
			this->shearTestToolStripMenuItem->Name = L"shearTestToolStripMenuItem";
			this->shearTestToolStripMenuItem->Size = System::Drawing::Size(128, 22);
			this->shearTestToolStripMenuItem->Text = L"Shear Test";
			this->shearTestToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::shearTestToolStripMenuItem_Click);
			this->shearTestToolStripMenuItem->Checked = false;

			// 
			// peelTestToolStripMenuItem
			// 
			this->peelTestToolStripMenuItem->Name = L"peelTestToolStripMenuItem";
			this->peelTestToolStripMenuItem->Size = System::Drawing::Size(128, 22);
			this->peelTestToolStripMenuItem->Text = L"Peel Test";
			this->peelTestToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::peelTestToolStripMenuItem_Click);
			this->peelTestToolStripMenuItem->Checked = true;

			// 
			// testDegreeToolStripMenuItem
			// 
			this->testDegreeToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(2) {this->degreeToolStripMenuItem, 
				this->degreeToolStripMenuItem1});
			this->testDegreeToolStripMenuItem->Name = L"testDegreeToolStripMenuItem";
			this->testDegreeToolStripMenuItem->Size = System::Drawing::Size(81, 20);
			this->testDegreeToolStripMenuItem->Text = L"Test Degree";
			// 
			// degreeToolStripMenuItem
			// 
			this->degreeToolStripMenuItem->Name = L"degreeToolStripMenuItem";
			this->degreeToolStripMenuItem->Size = System::Drawing::Size(132, 22);
			this->degreeToolStripMenuItem->Text = L"90 Degree";
			this->degreeToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::degree90ToolStripMenuItem_Click);
			this->degreeToolStripMenuItem->Checked = false;

			// 
			// degreeToolStripMenuItem1
			// 
			this->degreeToolStripMenuItem1->Name = L"degreeToolStripMenuItem1";
			this->degreeToolStripMenuItem1->Size = System::Drawing::Size(132, 22);
			this->degreeToolStripMenuItem1->Text = L"180 Degree";
			this->degreeToolStripMenuItem1->Click += gcnew System::EventHandler(this, &Form1::degree180ToolStripMenuItem_Click);
			this->degreeToolStripMenuItem1->Checked = true;

			// 
			// testSetupToolStripMenuItem
			// 
			this->testSetupToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(12) {this->customerNameToolStripMenuItem, 
				this->itemNameToolStripMenuItem, this->itemNoToolStripMenuItem, this->drawingNoToolStripMenuItem, this->manufacturingDateToolStripMenuItem, 
				this->inspectionDateToolStripMenuItem, this->batchNoToolStripMenuItem, this->materialTypeToolStripMenuItem, this->widthToolStripMenuItem, 
				this->colorToolStripMenuItem, this->weightAppliedToolStripMenuItem, this->minimumLengthToolStripMenuItem});
			this->testSetupToolStripMenuItem->Name = L"testSetupToolStripMenuItem";
			this->testSetupToolStripMenuItem->Size = System::Drawing::Size(74, 20);
			this->testSetupToolStripMenuItem->Text = L"Test Setup";
			// 
			// customerNameToolStripMenuItem
			// 
			this->customerNameToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(1) {this->toolStripTextBox1});
			this->customerNameToolStripMenuItem->Name = L"customerNameToolStripMenuItem";
			this->customerNameToolStripMenuItem->Size = System::Drawing::Size(180, 22);
			this->customerNameToolStripMenuItem->Text = L"Customer Name";
			// 
			// toolStripTextBox1
			// 
			this->toolStripTextBox1->Name = L"toolStripTextBox1";
			this->toolStripTextBox1->Size = System::Drawing::Size(100, 23);
			// 
			// itemNameToolStripMenuItem
			// 
			this->itemNameToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(1) {this->toolStripTextBox2});
			this->itemNameToolStripMenuItem->Name = L"itemNameToolStripMenuItem";
			this->itemNameToolStripMenuItem->Size = System::Drawing::Size(180, 22);
			this->itemNameToolStripMenuItem->Text = L"Item Name";
			// 
			// toolStripTextBox2
			// 
			this->toolStripTextBox2->Name = L"toolStripTextBox2";
			this->toolStripTextBox2->Size = System::Drawing::Size(100, 23);
			// 
			// itemNoToolStripMenuItem
			// 
			this->itemNoToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(1) {this->toolStripTextBox3});
			this->itemNoToolStripMenuItem->Name = L"itemNoToolStripMenuItem";
			this->itemNoToolStripMenuItem->Size = System::Drawing::Size(180, 22);
			this->itemNoToolStripMenuItem->Text = L"Item No.";
			// 
			// toolStripTextBox3
			// 
			this->toolStripTextBox3->Name = L"toolStripTextBox3";
			this->toolStripTextBox3->Size = System::Drawing::Size(100, 23);
			// 
			// drawingNoToolStripMenuItem
			// 
			this->drawingNoToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(1) {this->toolStripTextBox4});
			this->drawingNoToolStripMenuItem->Name = L"drawingNoToolStripMenuItem";
			this->drawingNoToolStripMenuItem->Size = System::Drawing::Size(180, 22);
			this->drawingNoToolStripMenuItem->Text = L"Drawing No.";
			// 
			// toolStripTextBox4
			// 
			this->toolStripTextBox4->Name = L"toolStripTextBox4";
			this->toolStripTextBox4->Size = System::Drawing::Size(100, 23);
			// 
			// manufacturingDateToolStripMenuItem
			// 
			this->manufacturingDateToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(1) {this->toolStripTextBox5});
			this->manufacturingDateToolStripMenuItem->Name = L"manufacturingDateToolStripMenuItem";
			this->manufacturingDateToolStripMenuItem->Size = System::Drawing::Size(180, 22);
			this->manufacturingDateToolStripMenuItem->Text = L"Manufacturing Date";
			// 
			// toolStripTextBox5
			// 
			this->toolStripTextBox5->Name = L"toolStripTextBox5";
			this->toolStripTextBox5->Size = System::Drawing::Size(100, 23);
			// 
			// inspectionDateToolStripMenuItem
			// 
			this->inspectionDateToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(1) {this->toolStripTextBox6});
			this->inspectionDateToolStripMenuItem->Name = L"inspectionDateToolStripMenuItem";
			this->inspectionDateToolStripMenuItem->Size = System::Drawing::Size(180, 22);
			this->inspectionDateToolStripMenuItem->Text = L"Inspection Date";
			// 
			// toolStripTextBox6
			// 
			this->toolStripTextBox6->Name = L"toolStripTextBox6";
			this->toolStripTextBox6->Size = System::Drawing::Size(100, 23);
			// 
			// batchNoToolStripMenuItem
			// 
			this->batchNoToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(1) {this->toolStripTextBox7});
			this->batchNoToolStripMenuItem->Name = L"batchNoToolStripMenuItem";
			this->batchNoToolStripMenuItem->Size = System::Drawing::Size(180, 22);
			this->batchNoToolStripMenuItem->Text = L"Batch No.";
			// 
			// toolStripTextBox7
			// 
			this->toolStripTextBox7->Name = L"toolStripTextBox7";
			this->toolStripTextBox7->Size = System::Drawing::Size(100, 23);
			// 
			// materialTypeToolStripMenuItem
			// 
			this->materialTypeToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(1) {this->toolStripTextBox8});
			this->materialTypeToolStripMenuItem->Name = L"materialTypeToolStripMenuItem";
			this->materialTypeToolStripMenuItem->Size = System::Drawing::Size(180, 22);
			this->materialTypeToolStripMenuItem->Text = L"Material Type";
			// 
			// toolStripTextBox8
			// 
			this->toolStripTextBox8->Name = L"toolStripTextBox8";
			this->toolStripTextBox8->Size = System::Drawing::Size(100, 23);
			// 
			// widthToolStripMenuItem
			// 
			this->widthToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(1) {this->toolStripTextBox9});
			this->widthToolStripMenuItem->Name = L"widthToolStripMenuItem";
			this->widthToolStripMenuItem->Size = System::Drawing::Size(180, 22);
			this->widthToolStripMenuItem->Text = L"Width";
			// 
			// toolStripTextBox9
			// 
			this->toolStripTextBox9->Name = L"toolStripTextBox9";
			this->toolStripTextBox9->Size = System::Drawing::Size(100, 23);
			// 
			// colorToolStripMenuItem
			// 
			this->colorToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(1) {this->toolStripTextBox10});
			this->colorToolStripMenuItem->Name = L"colorToolStripMenuItem";
			this->colorToolStripMenuItem->Size = System::Drawing::Size(180, 22);
			this->colorToolStripMenuItem->Text = L"Color";
			// 
			// toolStripTextBox10
			// 
			this->toolStripTextBox10->Name = L"toolStripTextBox10";
			this->toolStripTextBox10->Size = System::Drawing::Size(100, 23);
			// 
			// weightAppliedToolStripMenuItem
			// 
			this->weightAppliedToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(1) {this->toolStripTextBox11});
			this->weightAppliedToolStripMenuItem->Name = L"weightAppliedToolStripMenuItem";
			this->weightAppliedToolStripMenuItem->Size = System::Drawing::Size(180, 22);
			this->weightAppliedToolStripMenuItem->Text = L"Weight Applied";
			this->weightAppliedToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::weightAppliedToolStripMenuItem_Click);
			// 
			// toolStripTextBox11
			// 
			this->toolStripTextBox11->Name = L"toolStripTextBox11";
			this->toolStripTextBox11->Size = System::Drawing::Size(100, 23);
			// 
			// minimumLengthToolStripMenuItem
			// 
			this->minimumLengthToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(1) {this->toolStripTextBox12});
			this->minimumLengthToolStripMenuItem->Name = L"minimumLengthToolStripMenuItem";
			this->minimumLengthToolStripMenuItem->Size = System::Drawing::Size(180, 22);
			this->minimumLengthToolStripMenuItem->Text = L"Minimum Length";
			// 
			// toolStripTextBox12
			// 
			this->toolStripTextBox12->Name = L"toolStripTextBox12";
			this->toolStripTextBox12->Size = System::Drawing::Size(100, 23);
			// 
			// exitToolStripMenuItem
			// 
			this->exitToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(2) {this->exitProgramToolStripMenuItem, 
				this->returnToProgramToolStripMenuItem});
			this->exitToolStripMenuItem->Name = L"exitToolStripMenuItem";
			this->exitToolStripMenuItem->Size = System::Drawing::Size(37, 20);
			this->exitToolStripMenuItem->Text = L"Exit";
			// 
			// exitProgramToolStripMenuItem
			// 
			this->exitProgramToolStripMenuItem->Name = L"exitProgramToolStripMenuItem";
			this->exitProgramToolStripMenuItem->Size = System::Drawing::Size(175, 22);
			this->exitProgramToolStripMenuItem->Text = L"Exit Program";
			this->exitProgramToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::exitProgramToolStripMenuItem_Click);
			// 
			// returnToProgramToolStripMenuItem
			// 
			this->returnToProgramToolStripMenuItem->Name = L"returnToProgramToolStripMenuItem";
			this->returnToProgramToolStripMenuItem->Size = System::Drawing::Size(175, 22);
			this->returnToProgramToolStripMenuItem->Text = L"Return To Program";
			this->returnToProgramToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::returnToProgramToolStripMenuItem_Click);
			// 
			// helpToolStripMenuItem
			// 
			this->helpToolStripMenuItem->Name = L"helpToolStripMenuItem";
			this->helpToolStripMenuItem->Size = System::Drawing::Size(44, 20);
			this->helpToolStripMenuItem->Text = L"Help";
			// 
			// chart1
			// 
			this->chartArea1->Name = L"ChartArea1";
			this->chart1->ChartAreas->Add(chartArea1);
			this->legend1->Name = L"Legend1";
			this->chart1->Legends->Add(legend1);
			this->chart1->Location = System::Drawing::Point(12, 42);
			this->chart1->Name = L"chart1";

			this->chart1->ChartAreas[0]->CursorX->IsUserSelectionEnabled = true;
			this->chart1->ChartAreas[0]->CursorY->IsUserSelectionEnabled = false;
			this->chart1->ChartAreas[0]->AxisX->ScaleView->Zoomable = true;
			this->chart1->ChartAreas[0]->AxisY->ScaleView->Zoomable = false;
			this->chart1->ChartAreas[0]->AxisX->Minimum = 0;
			this->chart1->ChartAreas[0]->AxisX->Title = "Time (mili seconds)";
			this->chart1->ChartAreas[0]->AxisY->Title = "Load (Newton)";
			//this->chart1->ChartAreas[0]->AxisX->ScaleView->AutoScroll = true;
			//this->chart1->ChartAreas[0]->AxisY->ScaleView->Scroll = false;
			//this->chart1->ChartAreas[0]->AxisX->ScaleView->

			this->chart1->ChartAreas[0]->AxisX->IntervalType = System::Windows::Forms::DataVisualization::Charting::DateTimeIntervalType::Number;
			this->chart1->ChartAreas[0]->AxisX->Interval = 1000;

			for (int i = 0; i < MAX_NUM_TESTS; i++)
			{
				this->series1[i]->ChartArea = L"ChartArea1";
				this->series1[i]->ChartType = System::Windows::Forms::DataVisualization::Charting::SeriesChartType::FastLine;
				this->series1[i]->Legend = L"Legend1";
				this->series1[i]->Name = L"Time  Vs Load Test-" + (i+1).ToString();
				this->chart1->Series->Add(series1[i]);
				this->chart1->Size = System::Drawing::Size(984, 357);
				this->chart1->TabIndex = 1;
				this->chart1->Text = L"chart1";
			}
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(12, 426);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(35, 13);
			this->label1->TabIndex = 2;
			this->label1->Text = L"Comm Port Status               ";
			// 
			//Other
			UserExitRequested=false;
			// 
			// Form1
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(1045, 465);
			this->Controls->Add(this->label1);
			this->Controls->Add(this->chart1);
			this->Controls->Add(this->menuStrip1);
			this->MainMenuStrip = this->menuStrip1;
			this->Name = L"Form1";
			this->Text = L"KatPot Monitor";
			this->menuStrip1->ResumeLayout(false);
			this->menuStrip1->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->chart1))->EndInit();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
	private: System::Void exitProgramToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
		if (MessageBox::Show("Do u really want to  quit ?", " ", MessageBoxButtons::YesNo) == System::Windows::Forms::DialogResult::Yes) {
				this->UserExitRequested=true;
				//Application::Exit();
				}
			 }
private: System::Void returnToProgramToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
		 }
private: System::Void cOM1ToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
			 SelectedCommPort = SystemserialPorts[0];
			 SerialPortValidated = false;
		 }
private: System::Void cOM2ToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
			 SelectedCommPort = SystemserialPorts[1];
			 SerialPortValidated = false;
		 }
private: System::Void cOM3ToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
			 SelectedCommPort = SystemserialPorts[2];
			 SerialPortValidated = false;
		 }
private: System::Void cOM4ToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
			 SelectedCommPort = SystemserialPorts[3];
			 SerialPortValidated = false;
		 }
private: System::Void cOM5ToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
			 SelectedCommPort = SystemserialPorts[4];
			 SerialPortValidated = false;
		 }
private: System::Void cOM6ToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
			 SelectedCommPort = SystemserialPorts[5];
			 SerialPortValidated = false;
		 }
private: System::Void cOM7ToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
			 SelectedCommPort = SystemserialPorts[6];
			 SerialPortValidated = false;
		 }
private: System::Void cOM8ToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
			 SelectedCommPort = SystemserialPorts[7];
			 SerialPortValidated = false;
		 }
private: System::Void cOM9ToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
			 SelectedCommPort = SystemserialPorts[8];
			 SerialPortValidated = false;
		 }
private: System::Void cOM10ToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
			 SelectedCommPort = SystemserialPorts[9];
			 SerialPortValidated = false;
		 }
private: System::Void oTHERToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
			 SelectedCommPort = SystemserialPorts[10];
			 SerialPortValidated = false;
		 }
private: System::Void loadFileToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
			Stream^ myStream;
			OpenFileDialog^ openFileDialog1 = gcnew OpenFileDialog;

			//openFileDialog1->InitialDirectory = "c:\\";
			openFileDialog1->Filter = "ewm files (*.ewm)|*.ewm|All files (*.*)|*.*";
			openFileDialog1->FilterIndex = 2;
			openFileDialog1->RestoreDirectory = true;

			if ( openFileDialog1->ShowDialog() == System::Windows::Forms::DialogResult::OK )
			{
				if ( (myStream = openFileDialog1->OpenFile()) != nullptr )
				{
					// Insert code to read the stream here.
					myStream->Close();
				}
			}

		 }
private: System::Void saveFileToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {

			double xStart = this->chartArea1->CursorX->SelectionStart;
			double xEnd = this->chartArea1->CursorX->SelectionEnd;

			Stream^ myStream;
			SaveFileDialog^ saveFileDialog1 = gcnew SaveFileDialog;
			saveFileDialog1->Filter = "png files (*.png)|*.png|All files (*.*)|*.*";
			saveFileDialog1->FilterIndex = 2;
			saveFileDialog1->RestoreDirectory = true;
			if ( saveFileDialog1->ShowDialog() == System::Windows::Forms::DialogResult::OK )
			{
				if ( (myStream = saveFileDialog1->OpenFile()) != nullptr )
				{

					myStream->Close();
					MyXLStest(saveFileDialog1->FileName, this->TestSelected);
					// Code to write the stream goes here.
					//this->chart1->SaveImage(myStream, System::Windows::Forms::DataVisualization::Charting::ChartImageFormat::Png);
					//myStream->Close();
				}
			}

		 }
private: System::Void GridOnToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
			//turn on Grid
			this->chartArea1->AxisX->MajorGrid->LineWidth = 1;
			this->chartArea1->AxisY->MajorGrid->LineWidth = 1;
			this->GridOffToolStripMenuItem->Checked = false;
			this->GridOnToolStripMenuItem->Checked = true;


		 }
private: System::Void GridOffToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
			//turn off Grid
			this->chartArea1->AxisX->MajorGrid->LineWidth = 0;
			this->chartArea1->AxisY->MajorGrid->LineWidth = 0;
			this->GridOffToolStripMenuItem->Checked = true;
			this->GridOnToolStripMenuItem->Checked = false;

		 }

private: System::Void shearTestToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
			this->TestSelected=0;
			this->shearTestToolStripMenuItem->Checked = true;
			this->peelTestToolStripMenuItem->Checked = false;

		 }
private: System::Void peelTestToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
			this->TestSelected=1;
			this->shearTestToolStripMenuItem->Checked = false;
			this->peelTestToolStripMenuItem->Checked = true;
		 }

private: System::Void degree90ToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
			this->degreeToolStripMenuItem1->Checked = false;
			this->degreeToolStripMenuItem->Checked = true;
			this->DegreeSelected=0;
		 }
private: System::Void degree180ToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {

			this->degreeToolStripMenuItem->Checked = false;
			this->degreeToolStripMenuItem1->Checked = true;
			this->DegreeSelected=1;
		 }


		 
private: System::Void weightAppliedToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {	 
		 }
};
}

