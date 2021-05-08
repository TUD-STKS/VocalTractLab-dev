// ****************************************************************************
// This file is part of VocalTractLab.
// Copyright (C) 2020, Peter Birkholz, Dresden, Germany
// www.vocaltractlab.de
// author: Peter Birkholz
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.
//
// ****************************************************************************

#include "AnalysisSettingsDialog.h"

// ****************************************************************************
// Ranges for the scroll bars.
// ****************************************************************************

static const int MIN_WINDOW_LENGTH_PT = 60;
static const int MAX_WINDOW_LENGTH_PT = 2205;
static const int MIN_FFT_EXPONENT = 6;
static const int MAX_FFT_EXPONENT = 12;
static const int MIN_VIEW_RANGE_HZ = 1000;
static const int MAX_VIEW_RANGE_HZ = 14000;
static const int MIN_DYNAMIC_RANGE_DB = 20;
static const int MAX_DYNAMIC_RANGE_DB = 160;

static const double MIN_DIFFERENCE_FUNCTION_THRESHOLD = 0.05;
static const double MAX_DIFFERENCE_FUNCTION_THRESHOLD = 0.8;
static const double MIN_TIME_STEP_S = 0.001;
static const double MAX_TIME_STEP_S = 0.1;

// ****************************************************************************
/// IDs.
// ****************************************************************************

static const int IDN_NOTEBOOK                   = 3000;

// IDs for spectrum settings

static const int IDS_SPECTRUM_WINDOW_LENGTH  = 3500;
static const int IDL_SPECTRUM_WINDOW_LENGTH  = 3501;
static const int IDE_AVERAGE_SPECTRUM_TIME_STEP = 3502;
static const int IDR_SPECTRUM_TYPE           = 3503;
static const int IDB_CALC_AVERAGE_SPECTRUM   = 3504;

// IDs for spectrogram settings

static const int IDS_SPECTROGRAM_WINDOW_LENGTH  = 4000;
static const int IDS_SPECTROGRAM_DYNAMIC_RANGE  = 4001;
static const int IDS_SPECTROGRAM_VIEW_RANGE     = 4002;
static const int IDS_SPECTROGRAM_FFT_LENGTH     = 4003;

static const int IDL_SPECTROGRAM_WINDOW_LENGTH  = 4004;
static const int IDL_SPECTROGRAM_DYNAMIC_RANGE  = 4005;
static const int IDL_SPECTROGRAM_VIEW_RANGE     = 4006;
static const int IDL_SPECTROGRAM_FFT_LENGTH     = 4007;

static const int IDB_SET_SPECTROGRAM_DEFAULTS = 4010;

// IDs for F0 estimator settings

static const int IDC_SHOW_F0                    = 4020;
static const int IDT_F0_THRESHOLD               = 4021;
static const int IDT_F0_TIME_STEP               = 4022;
static const int IDB_CALC_F0                    = 4023;

// IDs for voice quality estimator settings

static const int IDC_SHOW_VOICE_QUALITY         = 4080;
static const int IDB_CALC_VOICE_QUALITY         = 4081;

// ...


// ****************************************************************************
// The event table.
// ****************************************************************************

BEGIN_EVENT_TABLE(AnalysisSettingsDialog, wxDialog)
  EVT_NOTEBOOK_PAGE_CHANGED(IDN_NOTEBOOK, AnalysisSettingsDialog::OnePageChanged)

  // Page for spectrum settings
  EVT_COMMAND_SCROLL(IDS_SPECTRUM_WINDOW_LENGTH, AnalysisSettingsDialog::OnSpectrumWindowLengthChanged)
  EVT_RADIOBOX(IDR_SPECTRUM_TYPE, AnalysisSettingsDialog::OnSpectrumTypeChanged)
  EVT_BUTTON(IDB_CALC_AVERAGE_SPECTRUM, AnalysisSettingsDialog::OnCalcAverageSpectrum)

  // Page for spectrogram settings
  EVT_COMMAND_SCROLL(IDS_SPECTROGRAM_WINDOW_LENGTH, AnalysisSettingsDialog::OnSpectrogramWindowLengthChanged)
  EVT_COMMAND_SCROLL(IDS_SPECTROGRAM_DYNAMIC_RANGE, AnalysisSettingsDialog::OnDynamicRangeChanged)
  EVT_COMMAND_SCROLL(IDS_SPECTROGRAM_VIEW_RANGE, AnalysisSettingsDialog::OnViewRangeChanged)
  EVT_COMMAND_SCROLL(IDS_SPECTROGRAM_FFT_LENGTH, AnalysisSettingsDialog::OnFftLengthChanged)
  EVT_BUTTON(IDB_SET_SPECTROGRAM_DEFAULTS, AnalysisSettingsDialog::OnSetSpectrogramDefaults)

  // Page for F0 settings
  EVT_CHECKBOX(IDC_SHOW_F0, AnalysisSettingsDialog::OnShowF0)
  EVT_BUTTON(IDB_CALC_F0, AnalysisSettingsDialog::OnCalcF0)

  // Page for voice quality settings
  EVT_CHECKBOX(IDC_SHOW_VOICE_QUALITY, AnalysisSettingsDialog::OnShowVoiceQuality)
  EVT_BUTTON(IDB_CALC_VOICE_QUALITY, AnalysisSettingsDialog::OnCalcVoiceQuality)

END_EVENT_TABLE()


// ****************************************************************************
/// Constructor.
// ****************************************************************************

AnalysisSettingsDialog::AnalysisSettingsDialog(wxWindow *parent, 
  wxWindow *updateEventReceiver, SpectrogramPlot *plot) : 
  wxDialog(parent, wxID_ANY, wxString("Analysis settings"), 
           wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE)
{
  this->spectrogramPlot = plot;
  this->updateEventReceiver = updateEventReceiver;
  data = Data::getInstance();

  initWidgets();

  // Update all pages individually instead of calling updateWidgets().
  updateSpectrogramPage();
  updateF0Page();

  // to also update the parent page:
  updateWidgets();
}


// ****************************************************************************
/// Init the widgets.
// ****************************************************************************

void AnalysisSettingsDialog::initWidgets()
{
  wxButton *button = NULL;
  wxBoxSizer *topLevelSizer = new wxBoxSizer(wxVERTICAL);

  // Create an invisible dummy button with the ID wxID_CANCEL, so that
  // the user can close the dialog with ESC.
  button = new wxButton(this, wxID_CANCEL, "", wxPoint(0,0), wxSize(0,0));
  
  // ****************************************************************
  // Create the notebook and add the pages.
  // ****************************************************************

  notebook = new wxNotebook(this, IDN_NOTEBOOK);

  spectrumPage = initSpectrumPage(notebook);
  spectrogramPage = initSpectrogramPage(notebook);
  f0Page = initF0Page(notebook);
  voiceQualityPage = initVoiceQualityPage(notebook);

  notebook->AddPage(spectrumPage, "Spectrum", false);
  notebook->AddPage(spectrogramPage, "Spectrogram", false);
  notebook->AddPage(f0Page, "F0", false);
  notebook->AddPage(voiceQualityPage, "Voice quality", false);

  // Set a fixed size for the notebook area.
//  notebook->SetMinSize(wxSize(300, 300));

  topLevelSizer->Add(notebook, 0, wxALL, 5);

  // ****************************************************************

  this->SetSizer(topLevelSizer);
  topLevelSizer->Fit(this);
  topLevelSizer->SetSizeHints(this);
}

// ****************************************************************************
/// Creates the tab page for the spectrum settings.
// ****************************************************************************

wxPanel *AnalysisSettingsDialog::initSpectrumPage(wxWindow *parent)
{
  wxPanel *page = new wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize);

  const int SCROLL_BAR_WIDTH = 120;
  const int VALUE_LABEL_WIDTH = 100;
  const int NAME_LABEL_WIDTH = 90;
  wxBoxSizer *topLevelSizer = new wxBoxSizer(wxVERTICAL);
  wxBoxSizer *sizer = NULL;
  wxStaticText *label;
  wxButton *button;

  topLevelSizer->AddSpacer(5);

  // ****************************************************************
  // Scroll bar for the window length.
  // ****************************************************************

  sizer = new wxBoxSizer(wxHORIZONTAL);

  label = new wxStaticText(page, wxID_ANY, "Window length");
  label->SetMinSize( this->FromDIP(wxSize(NAME_LABEL_WIDTH, wxDefaultCoord)) );    
  sizer->Add(label, 0, wxRIGHT | wxGROW, 5);

  scrSpectrumWindowLength = new wxScrollBar(page, IDS_SPECTRUM_WINDOW_LENGTH);
  scrSpectrumWindowLength->SetMinSize(this->FromDIP(wxSize(SCROLL_BAR_WIDTH, wxDefaultCoord) ));
  scrSpectrumWindowLength->SetScrollbar(0, 1, MAX_WINDOW_LENGTH_PT - MIN_WINDOW_LENGTH_PT + 1, 1, false);
  sizer->Add(scrSpectrumWindowLength, 1, wxRIGHT | wxGROW, 5);

  labSpectrumWindowLength = new wxStaticText(page, IDL_SPECTRUM_WINDOW_LENGTH, "0.0");
  labSpectrumWindowLength->SetMinSize(this->FromDIP(wxSize(VALUE_LABEL_WIDTH, wxDefaultCoord) ));
  sizer->Add(labSpectrumWindowLength, 0, wxGROW);

  topLevelSizer->Add(sizer, 0, wxALL | wxGROW, 5);

  // ****************************************************************
  // Length of time step for average spectrum.
  // ****************************************************************

  sizer = new wxBoxSizer(wxHORIZONTAL);

  label = new wxStaticText(page, wxID_ANY, "Time step for average spectrum (ms):",
    wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER);
  sizer->Add(label, 0, wxRIGHT | wxALIGN_CENTER, 5);

  txtAverageSpectrumTimeStep = new wxTextCtrl(page, IDE_AVERAGE_SPECTRUM_TIME_STEP, "", wxDefaultPosition, this->FromDIP(wxSize(50, -1)));
  txtAverageSpectrumTimeStep->Connect(-1, wxEVT_KILL_FOCUS, 
    wxFocusEventHandler(AnalysisSettingsDialog::OnSpectrumTimeStepLostFocus), NULL, this);
  sizer->Add(txtAverageSpectrumTimeStep, 0, wxRIGHT | wxGROW, 5);

  topLevelSizer->Add(sizer, 0, wxALL | wxGROW, 5);

  // ****************************************************************
  // Choice of transform type.
  // ****************************************************************

  wxString choices[Data::NUM_SPECTRUM_TYPES] =
  {
    "Short-time spectrum",
    "Cepstrum",
    "Average spectrum",
    "Test transform"
  };

  radSpectrumType = new wxRadioBox(page, IDR_SPECTRUM_TYPE, "Type", 
    wxDefaultPosition, wxDefaultSize, Data::NUM_SPECTRUM_TYPES, choices, 2, wxRA_SPECIFY_COLS);

  topLevelSizer->Add(radSpectrumType, 0, wxALL, 3);

  // ****************************************************************
  // Button to re-calculate the average spectrum.
  // ****************************************************************

  button = new wxButton(page, IDB_CALC_AVERAGE_SPECTRUM, "Calculate average spectrum");
  topLevelSizer->Add(button, 0, wxALL, 3);

  // ****************************************************************
  // Info text.
  // ****************************************************************

  label = new wxStaticText(page, wxID_ANY, 
    "A Hamming window is used.");
  topLevelSizer->Add(label, 0, wxALL | wxGROW, 5);


  // ****************************************************************
  // Set the top-level-sizer for this page
  // ****************************************************************

  page->SetSizer(topLevelSizer);

  return page;
}


// ****************************************************************************
/// Creates the tab page for the spectrogram settings.
// ****************************************************************************

wxPanel *AnalysisSettingsDialog::initSpectrogramPage(wxWindow *parent)
{
  wxPanel *page = new wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize);

  const int SCROLL_BAR_WIDTH = 120;
  const int VALUE_LABEL_WIDTH = 100;
  const int NAME_LABEL_WIDTH = 90;
  wxBoxSizer *topLevelSizer = new wxBoxSizer(wxVERTICAL);
  wxBoxSizer *sizer = NULL;
  wxStaticText *label;

  topLevelSizer->AddSpacer(5);

  // ****************************************************************
  // Scroll bar for the window length.
  // ****************************************************************

  sizer = new wxBoxSizer(wxHORIZONTAL);

  label = new wxStaticText(page, wxID_ANY, "Window length");
  label->SetMinSize(this->FromDIP(wxSize(NAME_LABEL_WIDTH, wxDefaultCoord) ));
  sizer->Add(label, 0, wxRIGHT | wxGROW, 5);

  scrSpectrogramWindowLength = new wxScrollBar(page, IDS_SPECTROGRAM_WINDOW_LENGTH);
  scrSpectrogramWindowLength->SetMinSize(this->FromDIP(wxSize(SCROLL_BAR_WIDTH, wxDefaultCoord) ));
  scrSpectrogramWindowLength->SetScrollbar(0, 1, MAX_WINDOW_LENGTH_PT - MIN_WINDOW_LENGTH_PT + 1, 1, false);
  sizer->Add(scrSpectrogramWindowLength, 1, wxRIGHT | wxGROW, 5);

  labSpectrogramWindowLength = new wxStaticText(page, IDL_SPECTROGRAM_WINDOW_LENGTH, "0.0");
  labSpectrogramWindowLength->SetMinSize(this->FromDIP(wxSize(VALUE_LABEL_WIDTH, wxDefaultCoord) ));
  sizer->Add(labSpectrogramWindowLength, 0, wxGROW);

  topLevelSizer->Add(sizer, 0, wxALL | wxGROW, 5);

  // ****************************************************************
  // Scroll bar for the FFT length.
  // ****************************************************************

  sizer = new wxBoxSizer(wxHORIZONTAL);

  label = new wxStaticText(page, wxID_ANY, "FFT length");
  label->SetMinSize(this->FromDIP(wxSize(NAME_LABEL_WIDTH, wxDefaultCoord) ));
  sizer->Add(label, 0, wxRIGHT | wxGROW, 5);

  scrFftLength = new wxScrollBar(page, IDS_SPECTROGRAM_FFT_LENGTH);
  scrFftLength->SetMinSize(this->FromDIP(wxSize(SCROLL_BAR_WIDTH, wxDefaultCoord) ));
  scrFftLength->SetScrollbar(0, 1, MAX_FFT_EXPONENT - MIN_FFT_EXPONENT + 1, 1, false);
  sizer->Add(scrFftLength, 1, wxRIGHT | wxGROW, 5);

  labFftLength = new wxStaticText(page, IDL_SPECTROGRAM_FFT_LENGTH, "0.0");
  labFftLength->SetMinSize(this->FromDIP(wxSize(VALUE_LABEL_WIDTH, wxDefaultCoord) ));
  sizer->Add(labFftLength, 0, wxGROW);

  topLevelSizer->Add(sizer, 0, wxALL | wxGROW, 5);

  // ****************************************************************
  // Scroll bar for the dynamic range.
  // ****************************************************************

  sizer = new wxBoxSizer(wxHORIZONTAL);

  label = new wxStaticText(page, wxID_ANY, "Dynamic range");
  label->SetMinSize(this->FromDIP(wxSize(NAME_LABEL_WIDTH, wxDefaultCoord) ));
  sizer->Add(label, 0, wxRIGHT | wxGROW, 5);

  scrDynamicRange = new wxScrollBar(page, IDS_SPECTROGRAM_DYNAMIC_RANGE);
  scrDynamicRange->SetMinSize(this->FromDIP(wxSize(SCROLL_BAR_WIDTH, wxDefaultCoord) ));
  scrDynamicRange->SetScrollbar(0, 1, MAX_DYNAMIC_RANGE_DB - MIN_DYNAMIC_RANGE_DB + 1, 1, false);
  sizer->Add(scrDynamicRange, 1, wxRIGHT | wxGROW, 5);

  labDynamicRange = new wxStaticText(page, IDL_SPECTROGRAM_DYNAMIC_RANGE, "0.0");
  labDynamicRange->SetMinSize(this->FromDIP(wxSize(VALUE_LABEL_WIDTH, wxDefaultCoord) ));
  sizer->Add(labDynamicRange, 0, wxGROW);

  topLevelSizer->Add(sizer, 0, wxALL | wxGROW, 5);

  // ****************************************************************
  // Scroll bar for the view range.
  // ****************************************************************

  sizer = new wxBoxSizer(wxHORIZONTAL);

  label = new wxStaticText(page, wxID_ANY, "View range");
  label->SetMinSize(this->FromDIP(wxSize(NAME_LABEL_WIDTH, wxDefaultCoord) ));
  sizer->Add(label, 0, wxRIGHT | wxGROW, 5);

  scrViewRange = new wxScrollBar(page, IDS_SPECTROGRAM_VIEW_RANGE);
  scrViewRange->SetMinSize(this->FromDIP(wxSize(SCROLL_BAR_WIDTH, wxDefaultCoord) ));
  scrViewRange->SetScrollbar(0, 1, MAX_VIEW_RANGE_HZ - MIN_VIEW_RANGE_HZ + 1, 1, false);
  sizer->Add(scrViewRange, 1, wxRIGHT | wxGROW, 5);

  labViewRange = new wxStaticText(page, IDL_SPECTROGRAM_VIEW_RANGE, "0.0");
  labViewRange->SetMinSize(this->FromDIP(wxSize(VALUE_LABEL_WIDTH, wxDefaultCoord) ));
  sizer->Add(labViewRange, 0, wxGROW);

  topLevelSizer->Add(sizer, 0, wxALL | wxGROW, 5);

  // ****************************************************************
  // Info text.
  // ****************************************************************

  label = new wxStaticText(page, wxID_ANY, 
    "A gaussian window is used.");
  topLevelSizer->Add(label, 0, wxALL | wxGROW, 5);

  // ****************************************************************
  // Button to set the default parameter values.
  // ****************************************************************

  wxButton *button = new wxButton(page, IDB_SET_SPECTROGRAM_DEFAULTS, "Set default values");
  topLevelSizer->Add(button, 0, wxALL, 3);

  // ****************************************************************
  // Set the top-level-sizer for this page
  // ****************************************************************

  page->SetSizer(topLevelSizer);

  return page;
}


// ****************************************************************************
/// Creates the tab page for the F0 estimation settings.
// ****************************************************************************

wxPanel *AnalysisSettingsDialog::initF0Page(wxWindow *parent)
{
  wxPanel *page = new wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize);
  wxBoxSizer *topLevelSizer = new wxBoxSizer(wxVERTICAL);
  wxSizer *sizer = NULL;
  wxStaticText *label;
  wxButton *button;

  // ****************************************************************
  // Create the controls.
  // ****************************************************************

  chkShowF0 = new wxCheckBox(page, IDC_SHOW_F0, "Show F0 signal");
  topLevelSizer->Add(chkShowF0, 0, wxALL, 5);

  // ****************************************************************

  sizer = new wxBoxSizer(wxHORIZONTAL);
  label = new wxStaticText(page, wxID_ANY, "Difference function threshold (default is 0.2) :",
    wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER);
  sizer->Add(label, 0, wxALL | wxALIGN_CENTER, 5);

  txtF0Threshold = new wxTextCtrl(page, IDT_F0_THRESHOLD, "", wxDefaultPosition, this->FromDIP(wxSize(40, -1)));
  txtF0Threshold->Connect(-1, wxEVT_KILL_FOCUS, 
    wxFocusEventHandler(AnalysisSettingsDialog::OnThresholdLostFocus), NULL, this);
  sizer->Add(txtF0Threshold, 0, wxALL, 5);

  topLevelSizer->Add(sizer, 0, wxALL, 0);

  // ****************************************************************

  sizer = new wxBoxSizer(wxHORIZONTAL);
  label = new wxStaticText(page, wxID_ANY, "Time step in s (default is 0.01) :",
    wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER);
  sizer->Add(label, 0, wxALL | wxALIGN_CENTER, 5);

  txtF0TimeStep = new wxTextCtrl(page, IDT_F0_TIME_STEP, "", wxDefaultPosition, this->FromDIP(wxSize(50, -1)));
  txtF0TimeStep->Connect(-1, wxEVT_KILL_FOCUS, 
    wxFocusEventHandler(AnalysisSettingsDialog::OnF0TimeStepLostFocus), NULL, this);
  sizer->Add(txtF0TimeStep, 0, wxALL, 5);

  topLevelSizer->Add(sizer, 0, wxALL, 0);

  // ****************************************************************

  label = new wxStaticText(page, wxID_ANY, "The F0 range is 40 Hz to 800 Hz.");
  topLevelSizer->Add(label, 0, wxALL, 5);

  // ****************************************************************

  button = new wxButton(page, IDB_CALC_F0, "Calculate F0");
  topLevelSizer->Add(button, 0, wxALL, 5);

  // ****************************************************************
  // Set the top-level-sizer for this page
  // ****************************************************************

  page->SetSizer(topLevelSizer);

  return page;
}


// ****************************************************************************
/// Creates the tab page for the voice quality estimation settings.
// ****************************************************************************

wxPanel *AnalysisSettingsDialog::initVoiceQualityPage(wxWindow *parent)
{
  wxPanel *page = new wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize);
  wxBoxSizer *topLevelSizer = new wxBoxSizer(wxVERTICAL);
  wxSizer *sizer = NULL;
//  wxStaticText *label;
  wxButton *button;

  // ****************************************************************
  // Create the controls.
  // ****************************************************************

  chkShowVoiceQuality = new wxCheckBox(page, IDC_SHOW_VOICE_QUALITY, "Show voice quality signal");
  topLevelSizer->Add(chkShowVoiceQuality, 0, wxALL, 5);

  button = new wxButton(page, IDB_CALC_VOICE_QUALITY, "Calc. voice quality");
  topLevelSizer->Add(button, 0, wxALL, 5);

  // ****************************************************************
  // Set the top-level-sizer for this page
  // ****************************************************************

  page->SetSizer(topLevelSizer);

  return page;
}


// ****************************************************************************
/// Update the widgets.
// ****************************************************************************

void AnalysisSettingsDialog::updateWidgets()
{
  wxWindow *page = notebook->GetCurrentPage();
  if (page == (wxWindow*)spectrumPage)
  {
    updateSpectrumPage();
  }
  else
  if (page == (wxWindow*)spectrogramPage)
  {
    updateSpectrogramPage();
  }
  else
  if (page == (wxWindow*)f0Page)
  {
    updateF0Page();
  }
  else
  if (page == (wxWindow*)voiceQualityPage)
  {
    updateVoiceQualityPage();
  }

  // ****************************************************************
  // Update the parent window of the dialog (the signal page).
  // ****************************************************************

   wxCommandEvent event(updateRequestEvent);
   event.SetInt(REFRESH_PICTURES);
   wxPostEvent(updateEventReceiver, event);

   wxYield();
}

// ****************************************************************************
// ****************************************************************************

void AnalysisSettingsDialog::updateSpectrumPage()
{
  wxString st;
  int newPos = 0;

  // ****************************************************************
  // Window length.
  // ****************************************************************

  int windowLength_pt = data->spectrumWindowLength_pt;
  newPos = windowLength_pt - MIN_WINDOW_LENGTH_PT;
  scrSpectrumWindowLength->SetThumbPosition(newPos);
  st = wxString::Format("%2.1f ms (%d pt)",
    1000.0*windowLength_pt / SAMPLING_RATE,
    windowLength_pt);
  labSpectrumWindowLength->SetLabel(st);

  // Time step for average spectrum.

  txtAverageSpectrumTimeStep->SetLabel( wxString::Format("%2.1f", data->averageSpectrumTimeStep_ms) );

  // Selection of the spectrum type.

  radSpectrumType->SetSelection(data->userSpectrumType);
}


// ****************************************************************************
// ****************************************************************************

void AnalysisSettingsDialog::updateSpectrogramPage()
{
  wxString st;
  int newPos = 0;

  // ****************************************************************
  // Window length.
  // ****************************************************************

  int windowLength_pt = spectrogramPlot->windowLength_pt;
  newPos = windowLength_pt - MIN_WINDOW_LENGTH_PT;
  scrSpectrogramWindowLength->SetThumbPosition(newPos);
  st = wxString::Format("%2.1f ms (%d pt)",
    1000.0*windowLength_pt / SAMPLING_RATE,
    windowLength_pt);
  labSpectrogramWindowLength->SetLabel(st);

  // ****************************************************************
  // FFT length.
  // ****************************************************************

  if (spectrogramPlot->frameLengthExponent < spectrogramPlot->getFrameLengthExponent(spectrogramPlot->windowLength_pt))
  {
    spectrogramPlot->frameLengthExponent = spectrogramPlot->getFrameLengthExponent(spectrogramPlot->windowLength_pt);
  }

  int frameLengthExponent = spectrogramPlot->frameLengthExponent;
  newPos = frameLengthExponent - MIN_FFT_EXPONENT;
  scrFftLength->SetThumbPosition(newPos);
  st = wxString::Format("%2.1f ms (%d pt)",
    1000.0*((int)1 << frameLengthExponent) / SAMPLING_RATE,
    ((int)1 << frameLengthExponent));
  labFftLength->SetLabel(st);

  // ****************************************************************
  // Dynamic range.
  // ****************************************************************

  int dynamicRange_dB = (int)(spectrogramPlot->dynamicRange_dB + 0.5);
  newPos = dynamicRange_dB - MIN_DYNAMIC_RANGE_DB;
  scrDynamicRange->SetThumbPosition(newPos);
  st = wxString::Format("%d dB", dynamicRange_dB);
  labDynamicRange->SetLabel(st);

  // ****************************************************************
  // View range.
  // ****************************************************************

  int viewRange_Hz = (int)(spectrogramPlot->viewRange_Hz + 0.5);
  newPos = viewRange_Hz - MIN_VIEW_RANGE_HZ;
  scrViewRange->SetThumbPosition(newPos);
  st = wxString::Format("%d Hz", viewRange_Hz);
  labViewRange->SetLabel(st);
}

// ****************************************************************************
// ****************************************************************************

void AnalysisSettingsDialog::updateF0Page()
{
  chkShowF0->SetValue( data->showF0 );
  txtF0Threshold->SetLabel( wxString::Format("%2.2f", data->f0EstimatorYin->differenceFunctionThreshold) );
  txtF0TimeStep->SetLabel( wxString::Format("%2.3f", data->f0EstimatorYin->timeStep_s) );
}

// ****************************************************************************
// ****************************************************************************

void AnalysisSettingsDialog::updateVoiceQualityPage()
{
  chkShowVoiceQuality->SetValue( data->showVoiceQuality );
}


// ****************************************************************************
/// The user changed the page to display.
// ****************************************************************************

void AnalysisSettingsDialog::OnePageChanged(wxNotebookEvent &event)
{
  updateWidgets();
}


// ****************************************************************************
// ****************************************************************************

void AnalysisSettingsDialog::OnSpectrumWindowLengthChanged(wxScrollEvent &event)
{
  int pos = event.GetPosition();
  if (data->spectrumWindowLength_pt != MIN_WINDOW_LENGTH_PT + pos)
  {
    data->spectrumWindowLength_pt = MIN_WINDOW_LENGTH_PT + pos;
    updateWidgets();
  }
}


// ****************************************************************************
// ****************************************************************************

void AnalysisSettingsDialog::OnSpectrumTypeChanged(wxCommandEvent &event)
{
  data->userSpectrumType = event.GetInt();
  updateWidgets();
}


// ****************************************************************************
// ****************************************************************************

void AnalysisSettingsDialog::OnCalcAverageSpectrum(wxCommandEvent &event)
{
  data->userSpectrumType = Data::AVERAGE_SPECTRUM;
  data->calcUserSpectrum();
  updateWidgets();
}


// ****************************************************************************
// ****************************************************************************

void AnalysisSettingsDialog::OnSpectrumTimeStepLostFocus(wxFocusEvent &event)
{
  static const double MIN_TIME_STEP_MS = 1.0;
  static const double MAX_TIME_STEP_MS = 100.0;
  double x;
  wxString text = txtAverageSpectrumTimeStep->GetValue();
  if (text.ToDouble(&x))
  {
    if (x < MIN_TIME_STEP_MS) { x = MIN_TIME_STEP_MS; }
    if (x > MAX_TIME_STEP_MS) { x = MAX_TIME_STEP_MS; }
    data->averageSpectrumTimeStep_ms = x;
  }
  updateWidgets();

  // Important: Also call the base class handler.
  event.Skip();
}

// ****************************************************************************
// ****************************************************************************

void AnalysisSettingsDialog::OnSpectrogramWindowLengthChanged(wxScrollEvent &event)
{
  int pos = event.GetPosition();
  if (spectrogramPlot->windowLength_pt != MIN_WINDOW_LENGTH_PT + pos)
  {
    spectrogramPlot->windowLength_pt = MIN_WINDOW_LENGTH_PT + pos;
    updateWidgets();
  }
}

// ****************************************************************************
// ****************************************************************************

void AnalysisSettingsDialog::OnDynamicRangeChanged(wxScrollEvent &event)
{
  int pos = event.GetPosition();
  if ((int)spectrogramPlot->dynamicRange_dB != MIN_DYNAMIC_RANGE_DB + pos)
  {
    spectrogramPlot->dynamicRange_dB = MIN_DYNAMIC_RANGE_DB + pos;
    updateWidgets();
  }
}


// ****************************************************************************
// ****************************************************************************

void AnalysisSettingsDialog::OnViewRangeChanged(wxScrollEvent &event)
{
  int pos = event.GetPosition();
  if ((int)spectrogramPlot->viewRange_Hz != MIN_VIEW_RANGE_HZ + pos)
  {
    spectrogramPlot->viewRange_Hz = MIN_VIEW_RANGE_HZ + pos;
    updateWidgets();
  }
}


// ****************************************************************************
// ****************************************************************************

void AnalysisSettingsDialog::OnFftLengthChanged(wxScrollEvent &event)
{
  int pos = event.GetPosition();
  if (spectrogramPlot->frameLengthExponent != MIN_FFT_EXPONENT + pos)
  {
    spectrogramPlot->frameLengthExponent = MIN_FFT_EXPONENT + pos;
    updateWidgets();
  }
}


// ****************************************************************************
/// Set default values for the spectrogram. This should be equal to the settings
/// in the constructor SpectrogramPlot::SpectrogramPlot().
// ****************************************************************************

void AnalysisSettingsDialog::OnSetSpectrogramDefaults(wxCommandEvent &event)
{
  spectrogramPlot->windowLength_pt = (int)(0.006*SAMPLING_RATE);
  spectrogramPlot->frameLengthExponent = 9;     // = 1024 points
  spectrogramPlot->viewRange_Hz = 5000.0;
  spectrogramPlot->dynamicRange_dB = 120.0;

  updateWidgets();
}


// ****************************************************************************
// ****************************************************************************

void AnalysisSettingsDialog::OnShowF0(wxCommandEvent &event)
{
  data->showF0 = !data->showF0;
  updateWidgets();
}


// ****************************************************************************
// ****************************************************************************

void AnalysisSettingsDialog::OnThresholdLostFocus(wxFocusEvent &event)
{
  wxString text = txtF0Threshold->GetValue();
  double x;
  if (text.ToDouble(&x))
  {
    if (x < MIN_DIFFERENCE_FUNCTION_THRESHOLD) { x = MIN_DIFFERENCE_FUNCTION_THRESHOLD; }
    if (x > MAX_DIFFERENCE_FUNCTION_THRESHOLD) { x = MAX_DIFFERENCE_FUNCTION_THRESHOLD; }
    data->f0EstimatorYin->differenceFunctionThreshold = x;
  }

  x = data->f0EstimatorYin->differenceFunctionThreshold;
  txtF0Threshold->SetValue(wxString::Format("%2.2f", x));

  // Important: Also call the base class handler.
  event.Skip();
}


// ****************************************************************************
// ****************************************************************************

void AnalysisSettingsDialog::OnF0TimeStepLostFocus(wxFocusEvent &event)
{
  wxString text = txtF0TimeStep->GetValue();
  double x;
  if (text.ToDouble(&x))
  {
    if (x < MIN_TIME_STEP_S) { x = MIN_TIME_STEP_S; }
    if (x > MAX_TIME_STEP_S) { x = MAX_TIME_STEP_S; }
    data->f0EstimatorYin->timeStep_s = x;
  }
  updateF0Page();

  // Important: Also call the base class handler.
  event.Skip();
}


// ****************************************************************************
/// Calculate the fundamental frequency curve.
// ****************************************************************************

void AnalysisSettingsDialog::OnCalcF0(wxCommandEvent &event)
{
  data->estimateF0(this);

  // ****************************************************************
  // Update the parent window of the dialog with the plot of the 
  // spectrogram!
  // ****************************************************************

   wxCommandEvent e(updateRequestEvent);
   event.SetInt(REFRESH_PICTURES);
   wxPostEvent(updateEventReceiver, e);
}


// ****************************************************************************
// ****************************************************************************

void AnalysisSettingsDialog::OnShowVoiceQuality(wxCommandEvent &event)
{
  data->showVoiceQuality = !data->showVoiceQuality;
  updateWidgets();
}


// ****************************************************************************
// ****************************************************************************

void AnalysisSettingsDialog::OnCalcVoiceQuality(wxCommandEvent &event)
{
  data->estimateVoiceQuality(this);

  // ****************************************************************
  // Update the parent window of the dialog with the plot of the 
  // spectrogram!
  // ****************************************************************

   wxCommandEvent e(updateRequestEvent);
   event.SetInt(REFRESH_PICTURES);
   wxPostEvent(updateEventReceiver, e);
}


// ****************************************************************************
