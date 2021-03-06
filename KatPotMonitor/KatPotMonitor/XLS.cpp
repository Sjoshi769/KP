#include <string.h>
#include <vcclr.h>
#include "KatPotMonitor.h"
//#include "stdafx.h"
using namespace System;


void MyXLStest(String^ OutFileName, int TestSelected, int FormStartIndex, int FormEndIndex, array<double>^ SeriesMin, array<double>^ SeriesMax, array<double>^ SeriesAverage);
#include "Form1.h"

//using namespace KatPotMonitor;

using namespace System::Collections::Generic;
using namespace System::Text;
using namespace System::Drawing;
using namespace Microsoft::Office::Interop::Excel;
//using namespace Microsoft::Office::Tools::Excel;

enum BackGroundColor {
	COLOR_YELLOW=0,
	COLOR_GRAY,
	COLOR_GAINSBORO,
	COLOR_TURQUOISE,
	COLOR_PEACHPUFF
};

extern gcroot<array<int>^> NumSamples;
extern gcroot<array<array<int>^>^> SampleArray;
extern gcroot<KatPotMonitor::Form1^> main_form;





    class CreateExcelDoc
    {

		private: gcroot<Microsoft::Office::Interop::Excel::Application^> app;
		private: gcroot<Microsoft::Office::Interop::Excel::Workbook^> workbook;
		private: gcroot<Microsoft::Office::Interop::Excel::Worksheet^> worksheet;
		private: gcroot<Microsoft::Office::Interop::Excel::Range^> workSheet_range;
		private: gcroot<Microsoft::Office::Interop::Excel::ChartObjects^> myxlChart;
		private: gcroot<Microsoft::Office::Interop::Excel::ChartObject^> myChart;
	    private: gcroot<Microsoft::Office::Interop::Excel::Axis^>  myAxis;
	    private: gcroot<Microsoft::Office::Interop::Excel::Series^> mySeries;
	    private: gcroot<Microsoft::Office::Interop::Excel::Range^> X_range;

	
		public: CreateExcelDoc()
        {
			createDoc(); 
        }

		public: ~CreateExcelDoc()
        {
			app->Quit();
			releaseObject(workSheet_range);
			releaseObject(worksheet);
			releaseObject(workbook);
			releaseObject(app);
        }


	
		public: void createDoc()
        {
            try
            {     
                app = gcnew Microsoft::Office::Interop::Excel::ApplicationClass();
                app->Visible = true;
                workbook = app->Workbooks->Add(1);
                worksheet = (Microsoft::Office::Interop::Excel::Worksheet^)workbook->Sheets[1];
				myxlChart = (Microsoft::Office::Interop::Excel::ChartObjects^) worksheet->ChartObjects(Type::Missing);
            }
            catch (Exception^ ex)
            {
               Console::Write("Exception Occured while creating object " + ex->ToString());
            }
            finally
            {
            }
        }

		public: void CloseWorkBook(bool flag1, String^ Filename, bool flag2)
		{
			workbook->Close(flag1, Filename, flag2);
			//workbook->SaveAs(Filename);
			//workbook->SaveAs(Filename, Microsoft::Office::Interop::Excel::XlFileFormat::xlWorkbookDefault, Type::Missing, Type::Missing, true, false, XlSaveAsAccessMode::xlNoChange, XlSaveConflictResolution::xlLocalSessionChanges, Type::Missing, Type::Missing);
		}

		public: void SetXRange(String^ R1, String^ R2)
		{
			X_range = worksheet->Range[R1, R2];
		}

		public: void XLDrawChart(int left, int top, int  width, int height, String^ R1, String^ R2 , int index, int TestType, double Min, double Max, double Average)
		{

			//myxlChart = (Microsoft::Office::Interop::Excel::ChartObjects^) worksheet->ChartObjects(Type::Missing);
			myChart =  myxlChart->Add(left, top, width, height);
            workSheet_range = worksheet->Range[R1, R2];
					

            myChart->Chart->SetSourceData(workSheet_range, Microsoft::Office::Interop::Excel::XlRowCol::xlColumns);


			//myChart->Chart->ChartType = Microsoft::Office::Interop::Excel::XlChartType::xlLine;
			myChart->Chart->ChartType = Microsoft::Office::Interop::Excel::XlChartType::xlXYScatterSmooth;	
			myChart->Chart->HasTitle = true;
			mySeries = (Microsoft::Office::Interop::Excel::Series^) myChart->Chart->SeriesCollection(1);
			mySeries->XValues = (X_range);
			mySeries->Name = mySeries->Name->Format("Min = {0}\n", Min);
			mySeries->Name += mySeries->Name->Format("Max = {0}\n", Max);
			mySeries->Name += mySeries->Name->Format("Average = {0}\n", Average);



			if (TestType==1)
			{
				myChart->Chart->ChartTitle->Text = "Load Vs Time For Test-" + (index+1).ToString();
			}
			else
			{
				myChart->Chart->ChartTitle->Text = "Load Vs Time For Test-" + (index+1).ToString();
			}


			//set y axis
			myAxis  = (Microsoft::Office::Interop::Excel::Axis^) myChart->Chart->Axes(Microsoft::Office::Interop::Excel::XlAxisType::xlValue, Microsoft::Office::Interop::Excel::XlAxisGroup::xlPrimary);
			myAxis->HasTitle = true;
			if (TestType==1)
			{
				myAxis->AxisTitle->Text = "Load";
			}
			else
			{
				myAxis->AxisTitle->Text = "Time (mili seconds)";
			}

			//set x axis
			myAxis  = (Microsoft::Office::Interop::Excel::Axis^) myChart->Chart->Axes(Microsoft::Office::Interop::Excel::XlAxisType::xlCategory, Microsoft::Office::Interop::Excel::XlAxisGroup::xlPrimary);
			myAxis->HasTitle = true;
			myAxis->AxisTitle->Text = "Time (mili seconds)";


			//myChart->Chart->Visible = Microsoft::Office::Interop::Excel::XlSheetVisibility::xlSheetVisible;
			// myChart->Chart->ChartWizard(workSheet_range,
			//	XlChartType::xlLine,
			//	6L,XlRowCol::xlColumns, 1L, 1L, true,
			//	"My Graph", "x", "f(x)");
			//	//Give the chart sheet a name
			// myChart->Chart->Name = "Load Vs Time";
		}



		private: void releaseObject(Object^ obj)
        {
            try
            {
                System::Runtime::InteropServices::Marshal::ReleaseComObject(obj);
                obj = nullptr;
            }
            catch (Exception^ ex)
            {
                obj = nullptr;
                Console::Write("Exception Occured while releasing object " + ex->ToString());
            }
            finally
            {
                GC::Collect();
            }
        }



		public: void createHeaders(int row, int col, String^ htext, String^ cell1,
String^ cell2, int mergeColumns,int b, bool font,int size,String^
fcolor)
        {
            worksheet->Cells[row, col] = htext;
            workSheet_range = worksheet->Range[cell1, cell2];
            workSheet_range->Merge(mergeColumns);
            switch(b)
            {
                case COLOR_YELLOW:
                workSheet_range->Interior->Color = System::Drawing::Color::Yellow.ToArgb();
                break;
                case COLOR_GRAY:
                    workSheet_range->Interior->Color = System::Drawing::Color::Gray.ToArgb();
                break;
                case COLOR_GAINSBORO:
                    workSheet_range->Interior->Color = 
			System::Drawing::Color::Gainsboro.ToArgb();
                    break;
                case COLOR_TURQUOISE:
                    workSheet_range->Interior->Color = 
			System::Drawing::Color::Turquoise.ToArgb();
                    break;
                case COLOR_PEACHPUFF:
                    workSheet_range->Interior->Color = 
			System::Drawing::Color::PeachPuff.ToArgb();
                    break;
                default:
                  //  workSheet_range.Interior.Color = System.Drawing.Color..ToArgb();
                    break;
            }
         
            workSheet_range->Borders->Color = System::Drawing::Color::Black.ToArgb();
            workSheet_range->Font->Bold = font;
            workSheet_range->ColumnWidth = size;
            if (fcolor->Equals(""))
            {
                workSheet_range->Font->Color = System::Drawing::Color::White.ToArgb();
            }
            else {
                workSheet_range->Font->Color = System::Drawing::Color::Black.ToArgb();
            }
        }

	
	public: void addData(int row, int col, String^ data, 
			String^ cell1, String^ cell2, int mergeColumns, String^ format)
        {
            worksheet->Cells[row, col] = data;
            workSheet_range = worksheet->Range[cell1, cell2];
			workSheet_range->Merge(mergeColumns);
            workSheet_range->Borders->Color = System::Drawing::Color::Black.ToArgb();
            workSheet_range->NumberFormat = format;
        }    
	
    };
	

void MyXLStest(String^ OutFileName,int TestSelected, int FormStartIndex, int FormEndIndex, array<double>^ SeriesMin, array<double>^ SeriesMax, array<double>^ SeriesAverage)
{
	int i;
	int StartIndex, EndIndex;
	CreateExcelDoc* excell_app = new CreateExcelDoc();         
	//creates the logo header
	excell_app->createHeaders(3, 4, "KatPot Systems", "D3", "F3", 2,COLOR_YELLOW,true,10,"n");


	//create test info header
	if (main_form->TestSelected==0)
		excell_app->createHeaders(4, 4, "Shear Test", "D4", "D4", 0,COLOR_YELLOW,true,10,"n");
	else
		excell_app->createHeaders(4, 4, "Peel Test", "D4", "D4", 0,COLOR_YELLOW,true,10,"n");

		excell_app->createHeaders(4, 5, "Report For", "G4", "G4", 0,COLOR_YELLOW,true,10,"n");

	if (main_form->DegreeSelected==0)
		excell_app->createHeaders(4, 6, "90 Degree", "F4", "F4", 0,COLOR_YELLOW,true,10,"n");
	else
		excell_app->createHeaders(4, 6, "180 Degree", "F4", "F4", 0,COLOR_YELLOW,true,10,"n");


	//create header for item and customer name
	excell_app->createHeaders(5, 1, "Item Name", "A5", "B5", 1, COLOR_YELLOW, true,10,"");
	excell_app->createHeaders(5, 3, main_form->toolStripTextBox2->Text, "C5", "D5", 1, COLOR_YELLOW, true,10,"");
	excell_app->createHeaders(5, 5, "Customer Name", "F5", "G5", 1, COLOR_YELLOW, true,10,"");
	excell_app->createHeaders(5, 7, main_form->toolStripTextBox1->Text, "G5", "H5", 1, COLOR_YELLOW, true,10,"");

	//create header for item and drawing  number
	excell_app->createHeaders(6, 1, "Item No.", "A6", "B6", 1, COLOR_YELLOW, true,10,"");
	excell_app->createHeaders(6, 3, main_form->toolStripTextBox3->Text, "C6", "D6", 1, COLOR_YELLOW, true,10,"");
	excell_app->createHeaders(6, 5, "Drawing Number", "E6", "F6", 1, COLOR_YELLOW, true,10,"");
	excell_app->createHeaders(6, 7, main_form->toolStripTextBox4->Text, "G6", "H6", 1, COLOR_YELLOW, true,10,"");



	//create header for inspection  and manufacturing  dates
	excell_app->createHeaders(7, 1, "Inspection Date", "A7", "B7", 1, COLOR_YELLOW, true,10,"");
	excell_app->createHeaders(7, 3, main_form->toolStripTextBox6->Text, "C7", "D7", 1, COLOR_YELLOW, true,10,"");
	excell_app->createHeaders(7, 5, "Manufacturing Date", "E7", "F7", 1, COLOR_YELLOW, true,10,"");
	excell_app->createHeaders(7, 7, main_form->toolStripTextBox5->Text, "G7", "H7", 1, COLOR_YELLOW, true,10,"");


	//create header for batch No. and   and manufacturing  dates
	excell_app->createHeaders(8, 5, "Batch No.", "E8", "F8", 1, COLOR_YELLOW, true,10,"");
	excell_app->createHeaders(8, 7, main_form->toolStripTextBox7->Text, "G8", "H8", 1, COLOR_YELLOW, true,10,"");


	//create header for material  and color
	excell_app->createHeaders(9, 1, "Material Type", "A9", "B9", 1, COLOR_YELLOW, true,10,"");
	excell_app->createHeaders(9, 3, main_form->toolStripTextBox8->Text, "C9", "D9", 1, COLOR_YELLOW, true,10,"");
	excell_app->createHeaders(9, 5, "Color ", "E9", "F9", 1, COLOR_YELLOW, true,10,"");
	excell_app->createHeaders(9, 7, main_form->toolStripTextBox10->Text, "G9", "H9", 1, COLOR_YELLOW, true,10,"");


	//create header for width   and min length
	excell_app->createHeaders(10, 1, "Width ", "A10", "B10", 1, COLOR_YELLOW, true,10,"");
	excell_app->createHeaders(10, 3, main_form->toolStripTextBox9->Text, "C10", "D10", 1, COLOR_YELLOW, true,10,"");
	excell_app->createHeaders(10, 5, "Minimum Length ", "E10", "F10", 1, COLOR_YELLOW, true,10,"");
	excell_app->createHeaders(10, 7, main_form->toolStripTextBox12->Text, "G10", "H12", 1, COLOR_YELLOW, true,10,"");


	//create header for weight applied
	excell_app->createHeaders(11, 5, "Weigth Applied", "E11", "F11", 1, COLOR_YELLOW, true,10,"");
	excell_app->createHeaders(11, 7, main_form->toolStripTextBox11->Text, "G11", "H11", 1, COLOR_YELLOW, true,10,"");


	//create Table header headre

	excell_app->createHeaders(12, 1, "Sample  Time (mili seconds)", "A12", "B12", 1, COLOR_GRAY, true,10,"");
	excell_app->createHeaders(12, 3, "Test-1 ", "C12", "D12", 1, COLOR_GRAY, true,10,"");

	//excell_app->createHeaders(12, 5, "Test-2", "G12", "H12", 1, COLOR_GRAY, true,10,"");
	//excell_app->createHeaders(12, 7, "Test-3", "I12", "J12", 1, COLOR_GRAY, true,10,"");


	StartIndex = 13;

	//add first series
	int max = NumSamples[0];
	//if (NumSamples[1] > max)
	//	max = NumSamples[1];
	//if (NumSamples[2] > max)
	//	max = NumSamples[2];

	int NumSamps = FormEndIndex - FormStartIndex;

	//add index
	for (i=0;i < NumSamps;i++)
	{
		int temp = StartIndex + i;
		excell_app->addData(temp, 1, (i*TIME_SCALE).ToString(), "A" + temp.ToString(), "B" + temp.ToString(), 1, "");
	}

	EndIndex = StartIndex + i;

	StartIndex = 13;
	for (i=0;i < NumSamps;i++)
	{
		int temp = StartIndex + i;
		int temp1 = SampleArray[0][i+ FormStartIndex];
		excell_app->addData(temp, 3, (temp1).ToString(), "C" + temp.ToString(), "D" + temp.ToString(), 1, "");

	}
	//add second series
	//StartIndex = 13;
	//for (i=0;i < NumSamps;i++)
	//{
	//	int temp = StartIndex + i;
	//	int temp1 = SampleArray[1][i+ FormStartIndex];
	//	excell_app->addData(temp, 5, (temp1).ToString(), "G" + temp.ToString(), "H" + temp.ToString(), 1, "");
	//
	//}
	////add third series
	StartIndex = 13;
	//for (i=0;i < NumSamps;i++)
	//{
	//	int temp = StartIndex + i;
	//	int temp1 = SampleArray[2][i+ FormStartIndex];
	//	excell_app->addData(temp, 7, (temp1).ToString(), "I" + temp.ToString(), "J" + temp.ToString(), 1, "");
	//}

	//Add X data range 
	excell_app->SetXRange("A" + StartIndex.ToString(), "A" + (StartIndex + NumSamples[0]).ToString());

	//excell_app->XLDrawChart(600, 100, 400, 400, "E" + StartIndex.ToString(), "E" + (StartIndex + NumSamples[0]) .ToString(),0,TestSelected,
	//	SeriesMin[0], SeriesMax[0], SeriesAverage[0]);

	excell_app->XLDrawChart(10, 15*(NumSamps + 13), 400, 400, "C" + StartIndex.ToString(), "C" + (StartIndex + NumSamples[0]) .ToString(),0,TestSelected,
		SeriesMin[0], SeriesMax[0], SeriesAverage[0]);

	//excell_app->XLDrawChart(600, 600, 400, 400, "G" + StartIndex.ToString(), "G" + (StartIndex + NumSamples[1]) .ToString(),1,TestSelected,
	//	SeriesMin[1], SeriesMax[1], SeriesAverage[1]);
	//excell_app->XLDrawChart(600, 1100, 400, 400, "I" + StartIndex.ToString(), "I" + (StartIndex + NumSamples[2]) .ToString(),2,TestSelected,
	//	SeriesMin[2], SeriesMax[2], SeriesAverage[2]);

	//add Data to cells
	//excell_app->addData(7, 2, "114287", "B7", "B7","#,##0");
	//excell_app->addData(7, 3, "", "C7", "C7", "");
	//excell_app->addData(7, 4, "129121", "D7", "D7", "#,##0");
	//add percentage row
	//excell_app->addData(8, 2, "", "B8", "B8", "");
	//excell_app->addData(8, 3, "=B7/D7", "C8", "C8", "0.0%");
	//excell_app->addData(8, 4, "", "D8", "D8", "");
	//add empty divider
	//excell_app->createHeaders(9, 2, "", "B9", "D9", 2, COLOR_GAINSBORO, true, 10, "");

	/***
	excell_app->workbook->SaveAs("f:\\users\\sanjay\\temp2\\New.XLS", \
		Microsoft::Office::Interop::Excel::XlFileFormat::xlExcel8 , \
		System::Reflection::Missing::Value, \
		System::Reflection::Missing::Value, \
		false, false,\
		Microsoft::Office::Interop::Excel::XlSaveAsAccessMode::xlNoChange, \
		System::Reflection::Missing::Value, \
		System::Reflection::Missing::Value, \
		System::Reflection::Missing::Value, \
		System::Reflection::Missing::Value, \
		System::Reflection::Missing::Value);
		***/



	//excell_app->CloseWorkBook(true,OutFileName,false);
	

	delete excell_app;
	}

//}


#if (0)
            Excel.Range chartRange ; 

            Excel.ChartObjects xlCharts = (Excel.ChartObjects)xlWorkSheet.ChartObjects(Type.Missing);
            Excel.ChartObject myChart = (Excel.ChartObject)xlCharts.Add(10, 80, 300, 250);
            Excel.Chart chartPage = myChart.Chart;

            chartRange = xlWorkSheet.get_Range("A1", "d5");
            chartPage.SetSourceData(chartRange, misValue);
            chartPage.ChartType = Microsoft::Office::Interop::Excel::XlChartType.xlColumnClustered; 
#endif
