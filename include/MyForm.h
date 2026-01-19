#pragma once

#include <cmath>
#include "Interval.h"
#include "Device.h"
#include <Windows.h>
#include <assert.h>
#include <algorithm>
#include <cstdio>
#include <vector>

#include "Solver.h"

using namespace System::Runtime::InteropServices;

namespace RataHouse
{

	/// <summary>
	/// Summary for MyForm
	/// </summary>
	public ref class MyForm : public System::Windows::Forms::Form
	{
	public:
		MyForm(void)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~MyForm()
		{
			if (components)
			{
				delete components;
			}
		}

	public:
		char* pathfile;
		bool exe = false;

	private: System::Windows::Forms::Label^  label2;
	private: System::Windows::Forms::DataGridView^  dataGridView1;

	private: System::Windows::Forms::Label^  label1;


	private: System::Windows::Forms::Label^  label3;
	private: System::Windows::Forms::Label^  label4;
	private: System::Windows::Forms::Label^  label5;

	public: bool eme = false;

	private: System::Windows::Forms::Button^  button1;
	private: System::Windows::Forms::OpenFileDialog^  openFileDialog1;
	private: System::Windows::Forms::Button^  button2;

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private: System::Void dataGridView1_CellClick(System::Object^ sender, System::Windows::Forms::DataGridViewCellEventArgs^ e) {
			if (e->RowIndex >= 0 && e->ColumnIndex >= 2) {
				int r = e->RowIndex;
				int c = e->ColumnIndex;

				// 1. Toggle Visual State
				if (this->dataGridView1->Rows[r]->Cells[c]->Style->BackColor == System::Drawing::Color::Turquoise) {
					this->dataGridView1->Rows[r]->Cells[c]->Style->BackColor = System::Drawing::Color::White;
				} else {
					this->dataGridView1->Rows[r]->Cells[c]->Style->BackColor = System::Drawing::Color::Turquoise;
				}

				// 2. Update Data Model (devices[r].assignedRange)
				// Reconstruct intervals for this device by scanning the row
				if (r < ndevice) {
					devices[r].assignedRange.clear();
					bool insideInterval = false;
					int start = -1;

					// Scan slots 0 to 49 (Columns 2 to 51)
					// Note: Grid has 50 slots max (Columns 2-51 are indices 2..51)
					
					for (int j = 0; j < 50; j++) {
						int colIdx = j + 2;
						// Check bounds to be safe
						if (colIdx < this->dataGridView1->Columns->Count) {
							bool isTurquoise = (this->dataGridView1->Rows[r]->Cells[colIdx]->Style->BackColor == System::Drawing::Color::Turquoise);
							
							if (isTurquoise) {
								if (!insideInterval) {
									start = j;
									insideInterval = true;
								}
							} else {
								if (insideInterval) {
									// Interval ended at j (exclusive)
									devices[r].assignedRange.push_back(Interval(start, j));
									insideInterval = false;
								}
							}
						}
					}
					// Close final interval if open
					if (insideInterval) {
						devices[r].assignedRange.push_back(Interval(start, 50));
					}

					// 3. Real-time Cost Update
					int totalCost = GetCost();
					if (totalCost > 0) {
						this->label5->Text = totalCost.ToString();
					} else {
						this->label5->Text = "No valid solution";
					}
				}
			}
		}

		void InitializeComponent(void)
		{
			this->button1 = (gcnew System::Windows::Forms::Button());
			this->openFileDialog1 = (gcnew System::Windows::Forms::OpenFileDialog());
			this->button2 = (gcnew System::Windows::Forms::Button());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->dataGridView1 = (gcnew System::Windows::Forms::DataGridView());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->label4 = (gcnew System::Windows::Forms::Label());
			this->label5 = (gcnew System::Windows::Forms::Label());
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->dataGridView1))->BeginInit();
			this->SuspendLayout();
			// 
			// button1
			// 
			this->button1->BackColor = System::Drawing::Color::Navy;
			this->button1->ForeColor = System::Drawing::SystemColors::ControlLightLight;
			this->button1->Location = System::Drawing::Point(36, 12);
			this->button1->Name = L"button1";
			this->button1->Size = System::Drawing::Size(99, 34);
			this->button1->TabIndex = 4;
			this->button1->Text = L"Open File";
			this->button1->UseVisualStyleBackColor = false;
			this->button1->Click += gcnew System::EventHandler(this, &MyForm::button1_Click);
			// 
			// openFileDialog1
			// 
			this->openFileDialog1->FileName = L"openFileDialog1";
			// 
			// button2
			// 
			this->button2->BackColor = System::Drawing::Color::Indigo;
			this->button2->ForeColor = System::Drawing::SystemColors::ButtonHighlight;
			this->button2->Location = System::Drawing::Point(776, 12);
			this->button2->Name = L"button2";
			this->button2->Size = System::Drawing::Size(100, 34);
			this->button2->TabIndex = 0;
			this->button2->UseVisualStyleBackColor = false;
			this->button2->Visible = false;
			this->button2->Click += gcnew System::EventHandler(this, &MyForm::button2_Click);
			// 
			// label2
			// 
			this->label2->BackColor = System::Drawing::SystemColors::ButtonFace;
			this->label2->Location = System::Drawing::Point(0, 0);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(100, 23);
			this->label2->TabIndex = 0;
			this->label2->Visible = false;
			// 
			// dataGridView1
			// 
			this->dataGridView1->ColumnHeadersHeightSizeMode = System::Windows::Forms::DataGridViewColumnHeadersHeightSizeMode::AutoSize;
			this->dataGridView1->Columns->Clear();
			this->dataGridView1->Columns->Add("Name", "Name");
			this->dataGridView1->Columns->Add("Wajib", "Wajib/tidak");
			for (int i = 1; i <= 50; i++) {
				this->dataGridView1->Columns->Add("Column" + i, i.ToString());
				this->dataGridView1->Columns[i + 1]->Width = 30;
			}
			this->dataGridView1->CellClick += gcnew System::Windows::Forms::DataGridViewCellEventHandler(this, &MyForm::dataGridView1_CellClick);
			this->dataGridView1->Location = System::Drawing::Point(12, 75);
			this->dataGridView1->Name = L"dataGridView1";
			this->dataGridView1->RowTemplate->Resizable = System::Windows::Forms::DataGridViewTriState::True;
			this->dataGridView1->Size = System::Drawing::Size(899, 246);
			this->dataGridView1->TabIndex = 5;
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->BackColor = System::Drawing::Color::Transparent;
			this->label1->ForeColor = System::Drawing::SystemColors::ButtonHighlight;
			this->label1->Location = System::Drawing::Point(23, 349);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(55, 13);
			this->label1->TabIndex = 6;
			this->label1->Text = L"Total Cost";
			// 
			// label3
			// 
			this->label3->AutoSize = true;
			this->label3->ForeColor = System::Drawing::SystemColors::ButtonHighlight;
			this->label3->Location = System::Drawing::Point(84, 349);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(10, 13);
			this->label3->TabIndex = 7;
			this->label3->Text = L":";
			// 
			// label4
			// 
			this->label4->AutoSize = true;
			this->label4->ForeColor = System::Drawing::SystemColors::ButtonHighlight;
			this->label4->Location = System::Drawing::Point(100, 349);
			this->label4->Name = L"label4";
			this->label4->Size = System::Drawing::Size(21, 13);
			this->label4->TabIndex = 8;
			this->label4->Text = L"Rp";
			// 
			// label5
			// 
			this->label5->AutoSize = true;
			this->label5->ForeColor = System::Drawing::SystemColors::ButtonHighlight;
			this->label5->Location = System::Drawing::Point(127, 349);
			this->label5->Name = L"label5";
			this->label5->Size = System::Drawing::Size(13, 13);
			this->label5->TabIndex = 9;
			this->label5->Text = L"0";
			// 
			// MyForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(0)), static_cast<System::Int32>(static_cast<System::Byte>(0)),
				static_cast<System::Int32>(static_cast<System::Byte>(64)));
			this->ClientSize = System::Drawing::Size(923, 440);
			this->Controls->Add(this->label5);
			this->Controls->Add(this->label4);
			this->Controls->Add(this->label3);
			this->Controls->Add(this->label1);
			this->Controls->Add(this->dataGridView1);
			this->Controls->Add(this->label2);
			this->Controls->Add(this->button2);
			this->Controls->Add(this->button1);
			this->Name = L"MyForm";
			this->ShowInTaskbar = false;
			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterScreen;
			this->Text = L"SmartGrid";
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->dataGridView1))->EndInit();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion



	private: System::Void button1_Click(System::Object^  sender, System::EventArgs^  e) {

		if (openFileDialog1->ShowDialog() == System::Windows::Forms::DialogResult::OK)
		{

			System::IO::StreamReader ^ sr = gcnew
				System::IO::StreamReader(openFileDialog1->FileName);
			System::String^ pathfilez = openFileDialog1->FileName;
			this->pathfile = (char*)(Marshal::StringToHGlobalAnsi(pathfilez)).ToPointer();
			input = this->pathfile;
			Solve();
			this->label2->Visible = true;
			this->label2->AutoSize = true;
			this->label2->Location = System::Drawing::Point(159, 19);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(35, 13);
			this->label2->TabIndex = 5;
			this->label2->Text = pathfilez;
			this->button2->Visible = true;
			this->button2->BackColor = System::Drawing::Color::Indigo;
			this->button2->Location = System::Drawing::Point(776, 12);
			this->button2->Name = L"button2";
			this->button2->Size = System::Drawing::Size(84, 23);
			this->button2->TabIndex = 5;
			this->button2->Text = L"Import";
			this->button2->UseVisualStyleBackColor = false;
			// sr->Close();
		}
	}
	private: System::Void button2_Click(System::Object^  sender, System::EventArgs^  e) {
		Solve();

		int totalCost = GetCost();
		if (totalCost > 0)
		{
			this->label5->Text = totalCost.ToString();
		}
		else
		{
			this->label5->Text = "No valid solution";
		}
		
		int cdevices = 0;
		// Refresh Grid (Visualizing devices)
		for (Device& d : devices)
		{
			this->dataGridView1->Rows->Add();
			this->dataGridView1->Rows[cdevices]->Cells[0]->Value = gcnew System::String(d.name.c_str());
			this->dataGridView1->Rows[cdevices]->Cells[1]->Value = d.wajib ? "Wajib" : "Tidak";

			for (Interval& range : d.assignedRange)
			{
				for (int i = range.begin; i < range.end; ++i)
				{
					// Columns are 1-based index in the loop logic, but usually accessed by index
					// The loop in InitializeComponent adds "Column1" -> index 2
					// i=0 -> Column2
					if ((i + 2) < this->dataGridView1->Columns->Count) {
						this->dataGridView1->Rows[cdevices]->Cells[i + 2]->Style->BackColor = System::Drawing::Color::Turquoise;
					}
				}
			}
			cdevices++;
		}
	}
};
}
