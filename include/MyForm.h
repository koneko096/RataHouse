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
using namespace System;
using namespace System::Windows::Forms;
using namespace System::Drawing;
using namespace System::Collections::Generic;

namespace RataHouse
{
	/// <summary>
	/// Smart Home Energy Management - Main Form
	/// Uses dropdown controls for device time range selection
	/// </summary>
	public ref class MyForm : public System::Windows::Forms::Form
	{
	public:
		MyForm(void)
		{
			InitializeComponent();
		}

	protected:
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
		bool eme = false;

	private:
		System::ComponentModel::Container ^components;

		// Top controls
		Button^ btnOpenFile;
		Button^ btnSolve;
		Button^ btnAddDevice;
		Button^ btnSaveYaml;
		Button^ btnLoadDefaults;
		Label^ lblGridLimit;
		TextBox^ txtGridLimit;
		Label^ lblFilePath;
		OpenFileDialog^ openFileDialog1;
		SaveFileDialog^ saveFileDialog1;

		// Pricing & Tiers panel
		Label^ lblPricingTitle;
		Button^ btnAddInterval;
		Label^ lblProLevel;
		NumericUpDown^ numProLevel;
		Panel^ pnlPricing;
		
		ref struct PricingColumn {
			int intervalIndex;
			ComboBox^ cmbStart;
			ComboBox^ cmbEnd;
			List<TextBox^>^ tierCosts;
			List<TextBox^>^ tierLimits;
			Button^ btnRemove;
		};
		List<PricingColumn^>^ pricingCols;

		// Device panel (scrollable) with dropdown controls
		Panel^ pnlDevices;
		Label^ lblDevicesTitle;

		// Results section
		Label^ lblCostTitle;
		Label^ lblCostValue;
		Panel^ pnlSuggestions;
		Label^ lblSuggestionsTitle;
		TextBox^ txtSuggestions;

		// Track dropdown pairs for each device
		ref struct DeviceRow {
			int deviceIndex;
			TextBox^ txtName;
			TextBox^ txtPower;
			TextBox^ txtSlot;
			TextBox^ txtSessions;
			CheckBox^ chkMandatory;
			Panel^ pnlRanges;
			Button^ btnAddRange;
			List<ComboBox^>^ startCombos;
			List<ComboBox^>^ endCombos;
		};

		List<DeviceRow^>^ deviceRows;

		// Helper: generate time strings for 24 hours in 30-min increments
		cli::array<String^>^ GenerateTimeSlots() {
			cli::array<String^>^ slots = gcnew cli::array<String^>(49); // 0:00 to 24:00
			for (int i = 0; i <= 48; i++) {
				int hour = i / 2;
				int minute = (i % 2) * 30;
				slots[i] = String::Format("{0:D2}:{1:D2}", hour, minute);
			}
			return slots;
		}

		void InitializeComponent(void)
		{
			this->SuspendLayout();

			deviceRows = gcnew List<DeviceRow^>();

			// ---- Open File Button ----
			btnOpenFile = gcnew Button();
			btnOpenFile->Text = L"Open File";
			btnOpenFile->BackColor = Color::FromArgb(30, 60, 114);
			btnOpenFile->ForeColor = Color::White;
			btnOpenFile->FlatStyle = FlatStyle::Flat;
			btnOpenFile->FlatAppearance->BorderSize = 0;
			btnOpenFile->Font = gcnew Drawing::Font("Segoe UI", 9, FontStyle::Bold);
			btnOpenFile->Location = Point(20, 15);
			btnOpenFile->Size = Drawing::Size(110, 32);
			btnOpenFile->Click += gcnew EventHandler(this, &MyForm::btnOpenFile_Click);

			// ---- Solve Button ----
			btnSolve = gcnew Button();
			btnSolve->Text = L"Solve";
			btnSolve->BackColor = Color::FromArgb(46, 125, 50);
			btnSolve->ForeColor = Color::White;
			btnSolve->FlatStyle = FlatStyle::Flat;
			btnSolve->FlatAppearance->BorderSize = 0;
			btnSolve->Font = gcnew Drawing::Font("Segoe UI", 9, FontStyle::Bold);
			btnSolve->Location = Point(140, 15);
			btnSolve->Size = Drawing::Size(100, 32);
			btnSolve->Visible = true; // Show by default
			btnSolve->Click += gcnew EventHandler(this, &MyForm::btnSolve_Click);

			// ---- Add Device Button ----
			btnAddDevice = gcnew Button();
			btnAddDevice->Text = L"Add Device";
			btnAddDevice->BackColor = Color::FromArgb(63, 81, 181);
			btnAddDevice->ForeColor = Color::White;
			btnAddDevice->FlatStyle = FlatStyle::Flat;
			btnAddDevice->FlatAppearance->BorderSize = 0;
			btnAddDevice->Font = gcnew Drawing::Font("Segoe UI", 9, FontStyle::Bold);
			btnAddDevice->Location = Point(250, 15);
			btnAddDevice->Size = Drawing::Size(100, 32);
			btnAddDevice->Visible = true; // Show by default
			btnAddDevice->Click += gcnew EventHandler(this, &MyForm::btnAddDevice_Click);

			// ---- Save YAML Button ----
			btnSaveYaml = gcnew Button();
			btnSaveYaml->Text = L"Save YAML";
			btnSaveYaml->BackColor = Color::FromArgb(211, 47, 47);
			btnSaveYaml->ForeColor = Color::White;
			btnSaveYaml->FlatStyle = FlatStyle::Flat;
			btnSaveYaml->FlatAppearance->BorderSize = 0;
			btnSaveYaml->Font = gcnew Drawing::Font("Segoe UI", 9, FontStyle::Bold);
			btnSaveYaml->Location = Point(650, 15);
			btnSaveYaml->Size = Drawing::Size(100, 32);
			btnSaveYaml->Click += gcnew EventHandler(this, &MyForm::btnSaveYaml_Click);

			// ---- Grid Limit Label ----
			lblGridLimit = gcnew Label();
			lblGridLimit->Text = L"Grid Limit (W):";
			lblGridLimit->ForeColor = Color::White;
			lblGridLimit->Font = gcnew Drawing::Font("Segoe UI", 9, FontStyle::Bold);
			lblGridLimit->Location = Point(360, 20);
			lblGridLimit->AutoSize = true;

			// ---- Grid Limit TextBox ----
			txtGridLimit = gcnew TextBox();
			txtGridLimit->Text = L"600";
			txtGridLimit->BackColor = Color::FromArgb(40, 40, 80);
			txtGridLimit->ForeColor = Color::White;
			txtGridLimit->BorderStyle = BorderStyle::FixedSingle;
			txtGridLimit->Location = Point(460, 18);
			txtGridLimit->Size = Drawing::Size(60, 25);
			txtGridLimit->TextAlign = HorizontalAlignment::Center;

			// ---- File Path Label ----
			lblFilePath = gcnew Label();
			lblFilePath->AutoSize = true;
			lblFilePath->ForeColor = Color::LightGray;
			lblFilePath->Font = gcnew Drawing::Font("Segoe UI", 8);
			lblFilePath->Location = Point(530, 22);
			lblFilePath->Text = L"No file loaded";

			// ---- Open File Dialog ----
			openFileDialog1 = gcnew OpenFileDialog();
			openFileDialog1->Filter = L"YAML files (*.yaml;*.yml)|*.yaml;*.yml|Text files (*.txt)|*.txt|All files (*.*)|*.*";

			// ---- Pricing Title ----
			lblPricingTitle = gcnew Label();
			lblPricingTitle->Text = L"Pricing Intervals & Tiers";
			lblPricingTitle->Font = gcnew Drawing::Font("Segoe UI", 11, FontStyle::Bold);
			lblPricingTitle->ForeColor = Color::White;
			lblPricingTitle->Location = Point(20, 60);
			lblPricingTitle->AutoSize = true;

			// ---- Add Interval Button ----
			btnAddInterval = gcnew Button();
			btnAddInterval->Text = L"+ Interval";
			btnAddInterval->BackColor = Color::FromArgb(63, 81, 181);
			btnAddInterval->ForeColor = Color::White;
			btnAddInterval->FlatStyle = FlatStyle::Flat;
			btnAddInterval->FlatAppearance->BorderSize = 0;
			btnAddInterval->Font = gcnew Drawing::Font("Segoe UI", 8, FontStyle::Bold);
			btnAddInterval->Location = Point(210, 58);
			btnAddInterval->Size = Drawing::Size(80, 24);
			btnAddInterval->Click += gcnew EventHandler(this, &MyForm::btnAddInterval_Click);

			// ---- Pro Level (Tiers) ----
			lblProLevel = gcnew Label();
			lblProLevel->Text = L"Tiers:";
			lblProLevel->ForeColor = Color::White;
			lblProLevel->Font = gcnew Drawing::Font("Segoe UI", 9, FontStyle::Bold);
			lblProLevel->Location = Point(310, 62);
			lblProLevel->AutoSize = true;

			numProLevel = gcnew NumericUpDown();
			numProLevel->Minimum = 1;
			numProLevel->Maximum = 5;
			numProLevel->Value = 1;
			numProLevel->BackColor = Color::FromArgb(40, 40, 80);
			numProLevel->ForeColor = Color::White;
			numProLevel->Location = Point(360, 60);
			numProLevel->Size = Drawing::Size(40, 22);
			numProLevel->ValueChanged += gcnew EventHandler(this, &MyForm::numProLevel_ValueChanged);

			// ---- Pricing Panel ----
			pnlPricing = gcnew Panel();
			pnlPricing->Location = Point(20, 85);
			pnlPricing->Size = Drawing::Size(880, 80);
			pnlPricing->AutoScroll = true;
			pnlPricing->BackColor = Color::FromArgb(30, 30, 50);
			pnlPricing->BorderStyle = BorderStyle::FixedSingle;

			// ---- Devices Title ----
			lblDevicesTitle = gcnew Label();
			lblDevicesTitle->Text = L"Devices & Scheduling";
			lblDevicesTitle->Font = gcnew Drawing::Font("Segoe UI", 11, FontStyle::Bold);
			lblDevicesTitle->ForeColor = Color::White;
			lblDevicesTitle->Location = Point(20, 180);
			lblDevicesTitle->AutoSize = true;

			// ---- Devices Panel ----
			pnlDevices = gcnew Panel();
			pnlDevices->Location = Point(20, 205);
			pnlDevices->Size = Drawing::Size(880, 160);
			pnlDevices->AutoScroll = true;
			pnlDevices->BackColor = Color::FromArgb(25, 25, 50);
			pnlDevices->BorderStyle = BorderStyle::FixedSingle;

			// ---- Cost Display ----
			lblCostTitle = gcnew Label();
			lblCostTitle->Text = L"Total Cost:";
			lblCostTitle->Font = gcnew Drawing::Font("Segoe UI", 10, FontStyle::Bold);
			lblCostTitle->ForeColor = Color::White;
			lblCostTitle->Location = Point(20, 378);
			lblCostTitle->AutoSize = true;

			lblCostValue = gcnew Label();
			lblCostValue->Text = L"Rp 0";
			lblCostValue->Font = gcnew Drawing::Font("Segoe UI", 14, FontStyle::Bold);
			lblCostValue->ForeColor = Color::FromArgb(76, 175, 80);
			lblCostValue->Location = Point(110, 373);
			lblCostValue->AutoSize = true;

			// ---- Suggestions Section ----
			lblSuggestionsTitle = gcnew Label();
			lblSuggestionsTitle->Text = L"Suggestions for Optional Devices";
			lblSuggestionsTitle->Font = gcnew Drawing::Font("Segoe UI", 10, FontStyle::Bold);
			lblSuggestionsTitle->ForeColor = Color::FromArgb(255, 183, 77);
			lblSuggestionsTitle->Location = Point(20, 410);
			lblSuggestionsTitle->AutoSize = true;
			lblSuggestionsTitle->Visible = false;

			txtSuggestions = gcnew TextBox();
			txtSuggestions->Multiline = true;
			txtSuggestions->ReadOnly = true;
			txtSuggestions->ScrollBars = ScrollBars::Vertical;
			txtSuggestions->BackColor = Color::FromArgb(30, 30, 60);
			txtSuggestions->ForeColor = Color::FromArgb(255, 235, 179);
			txtSuggestions->Font = gcnew Drawing::Font("Consolas", 9);
			txtSuggestions->BorderStyle = BorderStyle::FixedSingle;
			txtSuggestions->Location = Point(20, 435);
			txtSuggestions->Size = Drawing::Size(880, 100);
			txtSuggestions->Visible = false;

			// ---- Form ----
			this->BackColor = Color::FromArgb(15, 15, 40);
			this->ClientSize = Drawing::Size(920, 580);
			this->Text = L"SmartGrid - Energy Manager";
			this->StartPosition = FormStartPosition::CenterScreen;
			this->Font = gcnew Drawing::Font("Segoe UI", 9);

			this->Controls->Add(btnOpenFile);
			this->Controls->Add(btnSolve);
			this->Controls->Add(btnAddDevice);
			this->Controls->Add(btnSaveYaml);
			this->Controls->Add(lblGridLimit);
			this->Controls->Add(txtGridLimit);
			this->Controls->Add(lblFilePath);
			this->Controls->Add(lblPricingTitle);
			this->Controls->Add(btnAddInterval);
			this->Controls->Add(lblProLevel);
			this->Controls->Add(numProLevel);
			this->Controls->Add(pnlPricing);
			this->Controls->Add(lblDevicesTitle);
			this->Controls->Add(pnlDevices);
			this->Controls->Add(lblCostTitle);
			this->Controls->Add(lblCostValue);
			this->Controls->Add(lblSuggestionsTitle);
			this->Controls->Add(txtSuggestions);

			pricingCols = gcnew List<PricingColumn^>();
			saveFileDialog1 = gcnew SaveFileDialog();
			saveFileDialog1->Filter = L"YAML files (*.yaml)|*.yaml|All files (*.*)|*.*";

			this->ResumeLayout(false);
			this->PerformLayout();
		}

		void PopulatePricingPanel() {
			pnlPricing->Controls->Clear();
			pricingCols->Clear();
			numProLevel->Value = (Decimal)ProLevel;
			if (ninterval == 0) return;

			cli::array<String^>^ timeSlots = GenerateTimeSlots();

			int xOffset = 10;
			for (int j = 0; j < ninterval; j++) {
				PricingColumn^ col = gcnew PricingColumn();
				col->intervalIndex = j;
				col->tierCosts = gcnew List<TextBox^>();
				col->tierLimits = gcnew List<TextBox^>();

				Panel^ intvBox = gcnew Panel();
				intvBox->Size = Drawing::Size(220, 110 + (ProLevel * 22));
				intvBox->Location = Point(xOffset, 5);
				intvBox->BackColor = Color::FromArgb(40, 40, 70);
				intvBox->BorderStyle = BorderStyle::FixedSingle;

				// Start Hour
				col->cmbStart = gcnew ComboBox();
				col->cmbStart->DropDownStyle = ComboBoxStyle::DropDownList;
				col->cmbStart->Items->AddRange(timeSlots);
				col->cmbStart->Size = Drawing::Size(60, 21);
				col->cmbStart->Location = Point(5, 5);
				col->cmbStart->Font = gcnew Drawing::Font("Segoe UI", 8);
				if (intervals[j].begin >= 0 && intervals[j].begin <= 48) col->cmbStart->SelectedIndex = intervals[j].begin;
				intvBox->Controls->Add(col->cmbStart);

				Label^ lblTo = gcnew Label();
				lblTo->Text = "to";
				lblTo->ForeColor = Color::White;
				lblTo->Location = Point(68, 7);
				lblTo->AutoSize = true;
				intvBox->Controls->Add(lblTo);

				// End Hour
				col->cmbEnd = gcnew ComboBox();
				col->cmbEnd->DropDownStyle = ComboBoxStyle::DropDownList;
				col->cmbEnd->Items->AddRange(timeSlots);
				col->cmbEnd->Size = Drawing::Size(60, 21);
				col->cmbEnd->Location = Point(90, 5);
				col->cmbEnd->Font = gcnew Drawing::Font("Segoe UI", 8);
				if (intervals[j].end >= 0 && intervals[j].end <= 48) col->cmbEnd->SelectedIndex = intervals[j].end;
				intvBox->Controls->Add(col->cmbEnd);

				// Remove Button
				col->btnRemove = gcnew Button();
				col->btnRemove->Text = "X";
				col->btnRemove->Size = Drawing::Size(24, 22);
				col->btnRemove->Location = Point(185, 4);
				col->btnRemove->BackColor = Color::Maroon;
				col->btnRemove->ForeColor = Color::White;
				col->btnRemove->FlatStyle = FlatStyle::Flat;
				col->btnRemove->Tag = j;
				col->btnRemove->Click += gcnew EventHandler(this, &MyForm::btnRemoveInterval_Click);
				intvBox->Controls->Add(col->btnRemove);

				int tierY = 32;
				for (int i = 0; i < ProLevel; i++) {
					Label^ lblT = gcnew Label();
					lblT->Text = "T" + (i + 1);
					lblT->ForeColor = Color::Yellow;
					lblT->Location = Point(5, tierY + 3);
					lblT->AutoSize = true;
					intvBox->Controls->Add(lblT);

					// Limit
					TextBox^ txtLim = gcnew TextBox();
					txtLim->Text = ProLimit[i] > 100000 ? "999999" : ProLimit[i].ToString();
					txtLim->Size = Drawing::Size(60, 20);
					txtLim->Location = Point(30, tierY);
					txtLim->BackColor = Color::FromArgb(30, 30, 60);
					txtLim->ForeColor = Color::White;
					txtLim->BorderStyle = BorderStyle::FixedSingle;
					intvBox->Controls->Add(txtLim);
					col->tierLimits->Add(txtLim);

					Label^ lblW = gcnew Label();
					lblW->Text = "W";
					lblW->ForeColor = Color::Gray;
					lblW->Location = Point(92, tierY + 3);
					lblW->AutoSize = true;
					intvBox->Controls->Add(lblW);

					// Cost
					TextBox^ txtCost = gcnew TextBox();
					txtCost->Text = proCost[i][j].ToString();
					txtCost->BackColor = Color::FromArgb(30, 30, 60);
					txtCost->ForeColor = Color::White;
					txtCost->BorderStyle = BorderStyle::FixedSingle;
					txtCost->Location = Point(115, tierY);
					txtCost->Size = Drawing::Size(70, 20);
					intvBox->Controls->Add(txtCost);
					col->tierCosts->Add(txtCost);

					tierY += 24;
				}

				pnlPricing->Controls->Add(intvBox);
				pricingCols->Add(col);
				xOffset += 230;
			}
		}

		// ---- Build device rows with dropdown controls ----
		void PopulateDevicePanel() {
			pnlDevices->Controls->Clear();
			deviceRows->Clear();

			// Header row
			Label^ hdrName = gcnew Label();
			hdrName->Text = L"Device";
			hdrName->Font = gcnew Drawing::Font("Segoe UI", 9, FontStyle::Bold);
			hdrName->ForeColor = Color::LightGray;
			hdrName->Location = Point(10, 5);
			hdrName->Size = Drawing::Size(80, 20);
			pnlDevices->Controls->Add(hdrName);

			Label^ hdrPower = gcnew Label();
			hdrPower->Text = L"Power";
			hdrPower->Font = gcnew Drawing::Font("Segoe UI", 9, FontStyle::Bold);
			hdrPower->ForeColor = Color::LightGray;
			hdrPower->Location = Point(100, 5);
			hdrPower->Size = Drawing::Size(40, 20);
			pnlDevices->Controls->Add(hdrPower);

			Label^ hdrSlot = gcnew Label();
			hdrSlot->Text = L"Dur";
			hdrSlot->Font = gcnew Drawing::Font("Segoe UI", 9, FontStyle::Bold);
			hdrSlot->ForeColor = Color::LightGray;
			hdrSlot->Location = Point(145, 5);
			hdrSlot->Size = Drawing::Size(30, 20);
			pnlDevices->Controls->Add(hdrSlot);

			Label^ hdrSes = gcnew Label();
			hdrSes->Text = L"Ses";
			hdrSes->Font = gcnew Drawing::Font("Segoe UI", 9, FontStyle::Bold);
			hdrSes->ForeColor = Color::LightGray;
			hdrSes->Location = Point(180, 5);
			hdrSes->Size = Drawing::Size(30, 20);
			pnlDevices->Controls->Add(hdrSes);

			Label^ hdrType = gcnew Label();
			hdrType->Text = L"Wajib";
			hdrType->Font = gcnew Drawing::Font("Segoe UI", 9, FontStyle::Bold);
			hdrType->ForeColor = Color::LightGray;
			hdrType->Location = Point(220, 5);
			hdrType->Size = Drawing::Size(80, 20);
			pnlDevices->Controls->Add(hdrType);

			Label^ hdrRanges = gcnew Label();
			hdrRanges->Text = L"Scheduled Ranges (Start \u2192 End)";
			hdrRanges->Font = gcnew Drawing::Font("Segoe UI", 9, FontStyle::Bold);
			hdrRanges->ForeColor = Color::LightGray;
			hdrRanges->Location = Point(310, 5);
			hdrRanges->Size = Drawing::Size(300, 20);
			pnlDevices->Controls->Add(hdrRanges);

			int yOffset = 30;

			for (int i = 0; i < ndevice; i++) {
				DeviceRow^ row = gcnew DeviceRow();
				row->deviceIndex = i;
				row->startCombos = gcnew List<ComboBox^>();
				row->endCombos = gcnew List<ComboBox^>();

				// Device Name
				row->txtName = gcnew TextBox();
				row->txtName->Text = gcnew String(devices[i].name.c_str());
				row->txtName->BackColor = Color::FromArgb(40, 40, 80);
				row->txtName->ForeColor = Color::White;
				row->txtName->BorderStyle = BorderStyle::FixedSingle;
				row->txtName->Location = Point(10, yOffset + 5);
				row->txtName->Size = Drawing::Size(80, 20);
				pnlDevices->Controls->Add(row->txtName);

				// Power
				row->txtPower = gcnew TextBox();
				row->txtPower->Text = devices[i].power.ToString();
				row->txtPower->BackColor = Color::FromArgb(40, 40, 80);
				row->txtPower->ForeColor = Color::White;
				row->txtPower->BorderStyle = BorderStyle::FixedSingle;
				row->txtPower->Location = Point(100, yOffset + 5);
				row->txtPower->Size = Drawing::Size(40, 20);
				pnlDevices->Controls->Add(row->txtPower);

				// Slot
				row->txtSlot = gcnew TextBox();
				row->txtSlot->Text = devices[i].slot.ToString();
				row->txtSlot->BackColor = Color::FromArgb(40, 40, 80);
				row->txtSlot->ForeColor = Color::White;
				row->txtSlot->BorderStyle = BorderStyle::FixedSingle;
				row->txtSlot->Location = Point(145, yOffset + 5);
				row->txtSlot->Size = Drawing::Size(30, 20);
				pnlDevices->Controls->Add(row->txtSlot);

				// Sessions
				row->txtSessions = gcnew TextBox();
				row->txtSessions->Text = devices[i].nyala.ToString();
				row->txtSessions->BackColor = Color::FromArgb(40, 40, 80);
				row->txtSessions->ForeColor = Color::White;
				row->txtSessions->BorderStyle = BorderStyle::FixedSingle;
				row->txtSessions->Location = Point(180, yOffset + 5);
				row->txtSessions->Size = Drawing::Size(30, 20);
				pnlDevices->Controls->Add(row->txtSessions);

				// Mandatory
				row->chkMandatory = gcnew CheckBox();
				row->chkMandatory->Checked = devices[i].wajib;
				row->chkMandatory->Location = Point(220, yOffset + 5);
				row->chkMandatory->Size = Drawing::Size(15, 20);
				pnlDevices->Controls->Add(row->chkMandatory);

				// Range Panel
				row->pnlRanges = gcnew Panel();
				row->pnlRanges->Location = Point(310, yOffset);
				row->pnlRanges->Size = Drawing::Size(500, 30);
				row->pnlRanges->BackColor = Color::Transparent;
				pnlDevices->Controls->Add(row->pnlRanges);

				// Add existing assigned ranges as dropdown pairs
				if (!devices[i].assignedRange.empty()) {
					for (int r = 0; r < (int)devices[i].assignedRange.size(); r++) {
						AddRangeDropdowns(row, devices[i].assignedRange[r].begin, devices[i].assignedRange[r].end);
					}
				} else {
					// Default to permitted range so user sees checkboxes
					AddRangeDropdowns(row, devices[i].permittedRange.begin, devices[i].permittedRange.end);
				}

				// Add Range button
				row->btnAddRange = gcnew Button();
				row->btnAddRange->Text = L"+";
				row->btnAddRange->Font = gcnew Drawing::Font("Segoe UI", 8, FontStyle::Bold);
				row->btnAddRange->BackColor = Color::FromArgb(63, 81, 181);
				row->btnAddRange->ForeColor = Color::White;
				row->btnAddRange->FlatStyle = FlatStyle::Flat;
				row->btnAddRange->FlatAppearance->BorderSize = 0;
				row->btnAddRange->Size = Drawing::Size(28, 26);
				row->btnAddRange->Tag = i; // Store device index
				row->btnAddRange->Click += gcnew EventHandler(this, &MyForm::btnAddRange_Click);
				// Position is set dynamically
				UpdateAddButtonPosition(row);
				pnlDevices->Controls->Add(row->btnAddRange);

				deviceRows->Add(row);
				yOffset += 35;
			}
		}

		void AddRangeDropdowns(DeviceRow^ row, int startSlot, int endSlot) {
			cli::array<String^>^ timeSlots = GenerateTimeSlots();

			int rangeIndex = row->startCombos->Count;
			int xOffset = rangeIndex * 175;

			// Start ComboBox
			ComboBox^ cmbStart = gcnew ComboBox();
			cmbStart->DropDownStyle = ComboBoxStyle::DropDownList;
			cmbStart->Items->AddRange(timeSlots);
			cmbStart->BackColor = Color::FromArgb(40, 40, 80);
			cmbStart->ForeColor = Color::White;
			cmbStart->Font = gcnew Drawing::Font("Consolas", 9);
			cmbStart->Size = Drawing::Size(65, 25);
			cmbStart->Location = Point(xOffset, 2);
			cmbStart->Tag = row->deviceIndex;
			if (startSlot >= 0 && startSlot <= 48) cmbStart->SelectedIndex = startSlot;
			cmbStart->SelectedIndexChanged += gcnew EventHandler(this, &MyForm::RangeChanged);
			row->pnlRanges->Controls->Add(cmbStart);
			row->startCombos->Add(cmbStart);

			// Arrow label
			Label^ lblArrow = gcnew Label();
			lblArrow->Text = L"\u2192";
			lblArrow->ForeColor = Color::LightGray;
			lblArrow->Font = gcnew Drawing::Font("Segoe UI", 10);
			lblArrow->Location = Point(xOffset + 67, 4);
			lblArrow->Size = Drawing::Size(20, 20);
			row->pnlRanges->Controls->Add(lblArrow);

			// End ComboBox
			ComboBox^ cmbEnd = gcnew ComboBox();
			cmbEnd->DropDownStyle = ComboBoxStyle::DropDownList;
			cmbEnd->Items->AddRange(timeSlots);
			cmbEnd->BackColor = Color::FromArgb(40, 40, 80);
			cmbEnd->ForeColor = Color::White;
			cmbEnd->Font = gcnew Drawing::Font("Consolas", 9);
			cmbEnd->Size = Drawing::Size(65, 25);
			cmbEnd->Location = Point(xOffset + 88, 2);
			cmbEnd->Tag = row->deviceIndex;
			if (endSlot >= 0 && endSlot <= 48) cmbEnd->SelectedIndex = endSlot;
			cmbEnd->SelectedIndexChanged += gcnew EventHandler(this, &MyForm::RangeChanged);
			row->pnlRanges->Controls->Add(cmbEnd);
			row->endCombos->Add(cmbEnd);

			// Expand range panel if needed
			int newWidth = (rangeIndex + 1) * 175 + 10;
			if (newWidth > row->pnlRanges->Width) {
				row->pnlRanges->Size = Drawing::Size(newWidth, 30);
			}
		}

		void UpdateAddButtonPosition(DeviceRow^ row) {
			int x = 310 + row->startCombos->Count * 175 + 5;
			row->btnAddRange->Location = Point(x, row->pnlRanges->Location.Y + 2);
		}

		int SafeIntParse(String^ s, int defaultVal) {
			int val;
			if (Int32::TryParse(s, val)) return val;
			return defaultVal;
		}

		void SyncDevice(int deviceIndex) {
			if (deviceIndex < 0 || deviceIndex >= ndevice) return;

			DeviceRow^ row = deviceRows[deviceIndex];
			Device& dev = devices[deviceIndex];
			
			// Corrected memory management for std::string
			IntPtr ptr = Marshal::StringToHGlobalAnsi(row->txtName->Text);
			dev.name = (char*)ptr.ToPointer();
			Marshal::FreeHGlobal(ptr); // std::string has copied the content
			
			dev.power = SafeIntParse(row->txtPower->Text, dev.power);
			dev.slot = SafeIntParse(row->txtSlot->Text, dev.slot);
			dev.nyala = SafeIntParse(row->txtSessions->Text, dev.nyala);
			dev.wajib = row->chkMandatory->Checked;

			dev.assignedRange.clear();
			for (int r = 0; r < row->startCombos->Count; r++) {
				int startIdx = row->startCombos[r]->SelectedIndex;
				int endIdx = row->endCombos[r]->SelectedIndex;
				if (startIdx >= 0 && endIdx > startIdx) {
					dev.assignedRange.push_back(Interval(startIdx, endIdx));
				}
			}
		}

		// ---- Sync dropdown values back to device assignedRange ----
		void SyncDeviceRanges(int deviceIndex) {
			if (deviceIndex < 0 || deviceIndex >= ndevice) return;
			SyncDevice(deviceIndex);
			UpdateCostDisplay();
		}

		void UpdateCostDisplay() {
			int totalCost = GetCost();
			if (totalCost > 0) {
				lblCostValue->Text = "Rp " + totalCost.ToString("N0");
				lblCostValue->ForeColor = Color::FromArgb(76, 175, 80);
			} else {
				lblCostValue->Text = "No valid solution";
				lblCostValue->ForeColor = Color::FromArgb(244, 67, 54);
			}
		}

		void DisplaySuggestions(const std::vector<DeviceSuggestion>& suggestions) {
			if (suggestions.empty()) {
				lblSuggestionsTitle->Visible = false;
				txtSuggestions->Visible = false;
				return;
			}

			lblSuggestionsTitle->Visible = true;
			txtSuggestions->Visible = true;

			System::Text::StringBuilder^ sb = gcnew System::Text::StringBuilder();
			for (int i = 0; i < (int)suggestions.size(); i++) {
				const DeviceSuggestion& s = suggestions[i];
				sb->AppendLine(gcnew String(("--- " + s.deviceName + " (" + std::to_string(s.power) + "W) ---").c_str()));
				sb->AppendLine(gcnew String(("  Reason: " + s.reason).c_str()));
				sb->AppendLine(gcnew String(("  Suggestion: " + s.recommendation).c_str()));
				sb->AppendLine();
			}
			txtSuggestions->Text = sb->ToString();
		}

		// ---- Event Handlers ----

		void SyncPricing() {
			ninterval = pricingCols->Count;
			intervals.clear();
			intervals.resize(ninterval + 1); // +1 just in case
			
			ProLevel = (int)numProLevel->Value;
			ProLimit.clear();
			ProLimit.resize(ProLevel);
			
			proCost.clear();
			proCost.resize(ProLevel);
			for (int i = 0; i < ProLevel; i++) {
				proCost[i].resize(ninterval);
			}

			for (int j = 0; j < ninterval; j++) {
				PricingColumn^ col = pricingCols[j];
				intervals[j].begin = col->cmbStart->SelectedIndex;
				intervals[j].end = col->cmbEnd->SelectedIndex;

				for (int i = 0; i < ProLevel; i++) {
					// Use limits from the first column for all
					if (j == 0) {
						ProLimit[i] = SafeIntParse(pricingCols[0]->tierLimits[i]->Text, 999999);
					}
					proCost[i][j] = SafeIntParse(col->tierCosts[i]->Text, 0);
				}
			}
		}

		void btnAddInterval_Click(Object^ sender, EventArgs^ e) {
			SyncPricing();
			Interval newIntv(0, 48);
			if (ninterval > 0) {
				newIntv.begin = intervals[ninterval - 1].end;
				newIntv.end = (std::min)(48, newIntv.begin + 4);
			}
			intervals[ninterval] = newIntv;
			ninterval++;
			
			// Adjust proCost for new interval
			for (int i = 0; i < ProLevel; i++) {
				proCost[i].resize(ninterval);
				proCost[i][ninterval - 1] = 0;
			}

			PopulatePricingPanel();
		}

		void btnRemoveInterval_Click(Object^ sender, EventArgs^ e) {
			Button^ btn = safe_cast<Button^>(sender);
			int idx = safe_cast<int>(btn->Tag);
			
			SyncPricing();
			if (idx >= 0 && idx < ninterval) {
				intervals.erase(intervals.begin() + idx);
				for (int i = 0; i < ProLevel; i++) {
					proCost[i].erase(proCost[i].begin() + idx);
				}
				ninterval--;
			}
			PopulatePricingPanel();
		}

		void numProLevel_ValueChanged(Object^ sender, EventArgs^ e) {
			int newLevel = (int)numProLevel->Value;
			if (newLevel == ProLevel) return;

			SyncPricing();
			ProLevel = newLevel;
			ProLimit.resize(ProLevel, 999999);
			proCost.resize(ProLevel);
			for (int i = 0; i < ProLevel; i++) {
				proCost[i].resize(ninterval, 0);
			}
			PopulatePricingPanel();
		}

		void btnOpenFile_Click(Object^ sender, EventArgs^ e) {
			if (openFileDialog1->ShowDialog() == System::Windows::Forms::DialogResult::OK) {
				String^ pathStr = openFileDialog1->FileName;
				this->pathfile = (char*)(Marshal::StringToHGlobalAnsi(pathStr)).ToPointer();
				input = this->pathfile;

				lblFilePath->Text = pathStr;
				btnSolve->Visible = true;
				btnAddDevice->Visible = true;

				// Determine file type and load
				if (pathStr->EndsWith(".yaml") || pathStr->EndsWith(".yml")) {
					YamlFileInput();
				} else {
					FileInput();
				}

				// Populate panels with loaded data
				txtGridLimit->Text = powerLimit.ToString();
				PopulatePricingPanel();
				PopulateDevicePanel();

				// Clear previous results
				lblCostValue->Text = "Rp 0";
				lblSuggestionsTitle->Visible = false;
				txtSuggestions->Visible = false;
			}
		}

		void btnAddDevice_Click(Object^ sender, EventArgs^ e) {
			Device newDev;
			newDev.name = "New_Device";
			newDev.power = 100;
			newDev.slot = 1;
			newDev.nyala = 1;
			newDev.wajib = false;
			newDev.permittedRange = Interval(0, 48);

			devices.push_back(newDev);
			ndevice++;
			
			btnSolve->Visible = true;
			PopulateDevicePanel();
		}

		void btnSolve_Click(Object^ sender, EventArgs^ e) {
			// Sync Grid Limit
			powerLimit = SafeIntParse(txtGridLimit->Text, powerLimit);

			// Sync Pricing from UI
			SyncPricing();

			// Sync properties from UI
			for (int i = 0; i < deviceRows->Count; i++) {
				SyncDevice(i);
			}

			// Solve with suggestions
			calculateMean();
			sortDevices();

			int maxSkip = (std::max)(1, ndevice / 20);
			int skip = 0;
			bool solvable = true;
			for (Device& d : devices) {
				if (skip == maxSkip) {
					solvable = false;
					break;
				} else if (set(d) == false) {
					skip++;
					continue;
				} else {
					skip = 0;
				}
			}

			// Collect suggestions for unscheduled optional devices
			std::vector<DeviceSuggestion> suggestions;
			for (const Device& d : devices) {
				if (!d.wajib && d.assignedRange.empty()) {
					suggestions.push_back(DiagnoseDevice(d));
				}
			}

			// Update UI
			UpdateCostDisplay();
			PopulateDevicePanel(); // Refresh dropdowns with solved ranges
			DisplaySuggestions(suggestions);
		}

		void ApplyDropdownOverrides() {
			for (int i = 0; i < deviceRows->Count && i < ndevice; i++) {
				SyncDevice(i);
			}
		}

		void btnAddRange_Click(Object^ sender, EventArgs^ e) {
			Button^ btn = safe_cast<Button^>(sender);
			int devIdx = safe_cast<int>(btn->Tag);

			if (devIdx < deviceRows->Count) {
				DeviceRow^ row = deviceRows[devIdx];
				AddRangeDropdowns(row, -1, -1);
				UpdateAddButtonPosition(row);
			}
		}

		void btnSaveYaml_Click(Object^ sender, EventArgs^ e) {
			if (saveFileDialog1->ShowDialog() == System::Windows::Forms::DialogResult::OK) {
				System::IO::StreamWriter^ sw = gcnew System::IO::StreamWriter(saveFileDialog1->FileName);
				
				// Sync all current values from UI to global variables first
				powerLimit = SafeIntParse(txtGridLimit->Text, powerLimit);
				SyncPricing();
				for (int i = 0; i < deviceRows->Count; i++) {
					SyncDevice(i);
				}

				sw->WriteLine("grid:");
				sw->WriteLine("  total_slots: " + totalSlot);
				sw->WriteLine("  power_limit: " + powerLimit);
				sw->WriteLine();
				
				sw->WriteLine("pricing:");
				sw->WriteLine("  intervals:");
				for (int j = 0; j < ninterval; j++) {
					sw->WriteLine("    - start_hour: " + (intervals[j].begin / 2));
					sw->WriteLine("      end_hour: " + (intervals[j].end / 2));
					sw->WriteLine("      tiers:");
					for (int i = 0; i < ProLevel; i++) {
						String^ limit = ProLimit[i] > 10000 ? "unlimited" : ProLimit[i].ToString();
						sw->WriteLine("        - max_watts: " + limit);
						sw->WriteLine("          cost_per_slot: " + proCost[i][j]);
					}
				}
				sw->WriteLine();

				sw->WriteLine("devices:");
				for (int i = 0; i < ndevice; i++) {
					sw->WriteLine("  - name: " + gcnew String(devices[i].name.c_str()));
					sw->WriteLine("    power: " + devices[i].power);
					sw->WriteLine("    slots_needed: " + devices[i].slot);
					sw->WriteLine("    permitted_start: " + (devices[i].permittedRange.begin / 2));
					sw->WriteLine("    permitted_end: " + (devices[i].permittedRange.end / 2));
					sw->WriteLine("    type: " + (devices[i].wajib ? "mandatory" : "optional"));
					sw->WriteLine("    sessions: " + devices[i].nyala);
					sw->WriteLine();
				}

				sw->Close();
				MessageBox::Show("Configuration saved to " + saveFileDialog1->FileName, "Success", MessageBoxButtons::OK, MessageBoxIcon::Information);
			}
		}

		void RangeChanged(Object^ sender, EventArgs^ e) {
			ComboBox^ cmb = safe_cast<ComboBox^>(sender);
			int devIdx = safe_cast<int>(cmb->Tag);
			SyncDeviceRanges(devIdx);
		}
	};
}
