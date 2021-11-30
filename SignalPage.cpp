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

#include "SignalPage.h"

#include <stdio.h>
#include <wx/config.h>
#include <wx/statline.h>
#include <wx/splitter.h>
#include "Backend/Dsp.h"
#include "SignalComparisonPicture.h"

using namespace std;

// ****************************************************************************
// Ids.
// ****************************************************************************

static const int IDB_MIX_TRACKS           = 7009;
static const int IDB_ANALYSIS_SETTINGS	  = 7010;
static const int IDB_ANALYSIS_RESULTS 	  = 7011;
static const int IDB_CALC_F0          	  = 7012;
static const int IDB_CALC_VOICE_QUALITY   = 7013;

static const int IDS_SCROLL_BAR           = 7020;
static const int IDC_SHOW_MAIN_TRACK      = 7021;
static const int IDC_SHOW_EGG_TRACK       = 7022;
static const int IDC_SHOW_EXTRA_TRACK     = 7023;
static const int IDB_HORZ_ZOOM_MINUS      = 7024;
static const int IDB_HORZ_ZOOM_PLUS       = 7025;
static const int IDB_VERT_ZOOM_MINUS      = 7026;
static const int IDB_VERT_ZOOM_PLUS       = 7027;
static const int IDB_EXCHANGE_MAIN_EGG    = 7030;
static const int IDB_EXCHANGE_MAIN_EXTRA  = 7031;

static const int IDR_SPECTROGRAM_CHOICE   = 7040;
static const int IDC_SHOW_SPECTROGRAM_TEXT = 7044;


// ****************************************************************************
// The event table.
// ****************************************************************************

BEGIN_EVENT_TABLE(SignalPage, wxPanel)
  // Custom event handler for update requests by child widgets.
  EVT_COMMAND(wxID_ANY, updateRequestEvent, SignalPage::OnUpdateRequest)

  EVT_COMMAND_SCROLL(IDS_SCROLL_BAR, SignalPage::OnScrollEvent)
  
  EVT_CHECKBOX(IDC_SHOW_MAIN_TRACK, SignalPage::OnShowMainTrackClicked)
  EVT_CHECKBOX(IDC_SHOW_EGG_TRACK, SignalPage::OnShowEggTrackClicked)
  EVT_CHECKBOX(IDC_SHOW_EXTRA_TRACK, SignalPage::OnShowExtraTrackClicked)
  
  EVT_BUTTON(IDB_HORZ_ZOOM_PLUS, SignalPage::OnOsziHorzZoomPlusClicked)
  EVT_BUTTON(IDB_HORZ_ZOOM_MINUS, SignalPage::OnOsziHorzZoomMinusClicked)
  EVT_BUTTON(IDB_VERT_ZOOM_PLUS, SignalPage::OnOsziVertZoomPlusClicked)
  EVT_BUTTON(IDB_VERT_ZOOM_MINUS, SignalPage::OnOsziVertZoomMinusClicked)
  EVT_BUTTON(IDB_EXCHANGE_MAIN_EGG, SignalPage::OnExchangeMainEggClicked)
  EVT_BUTTON(IDB_EXCHANGE_MAIN_EXTRA, SignalPage::OnExchangeMainExtraClicked)

  EVT_RADIOBOX(IDR_SPECTROGRAM_CHOICE, SignalPage::OnSpectrogramChoice)
  EVT_CHECKBOX(IDC_SHOW_SPECTROGRAM_TEXT, SignalPage::OnShowSpectrogramText)

  EVT_BUTTON(IDB_ANALYSIS_SETTINGS, SignalPage::OnAnalysisSettings)
  EVT_BUTTON(IDB_ANALYSIS_RESULTS, SignalPage::OnAnalysisResults)
  EVT_BUTTON(IDB_CALC_F0, SignalPage::OnCalcF0)
  EVT_BUTTON(IDB_CALC_VOICE_QUALITY, SignalPage::OnCalcVoiceQuality)
END_EVENT_TABLE()


// ****************************************************************************
// ****************************************************************************

SignalPage::SignalPage(wxWindow *parent) : wxPanel(parent)
{
  initVars();
  initWidgets();
  updateWidgets();
}

// ****************************************************************************
// ****************************************************************************

void SignalPage::updateWidgets()
{
  // Update the scroll bar
  
  scrTime->SetThumbPosition(data->centerPos_pt);

  // Update the labels
  
//  labHorzZoom->SetLabel( wxString::Format("%4.2f", (double)data->oscillogramVisTimeRange_pt / 700.0) );
  labHorzZoom->SetLabel(wxString::Format("%4.2f", 700.0 / (double)data->oscillogramVisTimeRange_pt));
  labVertZoom->SetLabel(wxString::Format("%4.2f", (double)data->oscillogramAmpZoom));

  // Check boxes
  
  chkShowMainTrack->SetValue(data->showTrack[Data::MAIN_TRACK]);
  chkShowEggTrack->SetValue(data->showTrack[Data::EGG_TRACK]);
  chkShowExtraTrack->SetValue(data->showTrack[Data::EXTRA_TRACK]);

  // Spectrogram controls

  chkShowSpectrogramText->SetValue( data->showSpectrogramText );
  if ((data->selectedSpectrogram >= 0) && (data->selectedSpectrogram < data->NUM_TRACKS))
  {
    radSpectrogramChoice->SetSelection( data->selectedSpectrogram );
  }

  // Refresh the images

  picSignal->Refresh();
  picSpectrogram->Refresh();
  picSimpleSpectrum->Refresh();
}

// ****************************************************************************
// ****************************************************************************

void SignalPage::scrollLeft()
{
  data->centerPos_pt-= 200;
  if (data->centerPos_pt < 0) 
  { 
    data->centerPos_pt = 0; 
  }
  updateWidgets();
}


// ****************************************************************************
// ****************************************************************************

void SignalPage::scrollRight()
{
  data->centerPos_pt+= 200;
  const int N = Data::TRACK_DURATION_S*SAMPLING_RATE;

  if (data->centerPos_pt >= N) 
  { 
    data->centerPos_pt = N-1; 
  }

  updateWidgets();

}

// ****************************************************************************
// ****************************************************************************

void SignalPage::initVars()
{
  // Init the data class instance !
  data = Data::getInstance();
}

// ****************************************************************************
// ****************************************************************************

void SignalPage::initWidgets()
{
  wxButton *button = NULL;
  wxStaticText *label = NULL;

  // ****************************************************************
  // Top level sizer.
  // ****************************************************************

  wxBoxSizer *topLevelSizer = new wxBoxSizer(wxHORIZONTAL);

  // ****************************************************************
  // Left side sizer.
  // ****************************************************************

  wxBoxSizer *leftSizer = new wxBoxSizer(wxVERTICAL);

  leftSizer->AddSpacer(5);

  button = new wxButton(this, IDB_ANALYSIS_SETTINGS, "Analysis settings");
  leftSizer->Add(button, 0, wxALL | wxGROW, 3);

  button = new wxButton(this, IDB_ANALYSIS_RESULTS, "Analysis results");
  leftSizer->Add(button, 0, wxALL | wxGROW, 3);

  button = new wxButton(this, IDB_CALC_F0, "Calculate F0");
  leftSizer->Add(button, 0, wxALL | wxGROW, 3);

  button = new wxButton(this, IDB_CALC_VOICE_QUALITY, "Calc. voice quality");
  leftSizer->Add(button, 0, wxALL | wxGROW, 3);


  leftSizer->AddSpacer(20);

  // Add the sizer to the top-level sizer!
  topLevelSizer->Add(leftSizer, 0, wxALL, 5);

  // ****************************************************************
  // Static line to separate the left and right part.
  // ****************************************************************

  wxStaticLine *verticalLine = new wxStaticLine(this, wxID_ANY, wxDefaultPosition, 
    wxDefaultSize, wxLI_VERTICAL);

  topLevelSizer->Add(verticalLine, 0, wxGROW | wxALL, 2);

  // ****************************************************************
  // Devide the right side by 2 splitters.
  // ****************************************************************

  wxSplitterWindow *splitter = new wxSplitterWindow(this, wxID_ANY, 
    wxDefaultPosition, wxDefaultSize, wxSP_3DSASH | wxSP_LIVE_UPDATE);
  splitter->SetBackgroundColour(*wxWHITE); // If this is not set to white, every label will show up with a white background against the gray panels.

  splitter->SetMinimumPaneSize(20);   // Pane size may not be reduced to zero!

  wxSplitterWindow *subSplitter = new wxSplitterWindow(splitter, wxID_ANY, 
    wxDefaultPosition, wxDefaultSize, wxSP_NO_XP_THEME | wxSP_LIVE_UPDATE);
  subSplitter->SetBackgroundColour(*wxWHITE); // If this is not set to white, every label will show up with a white background against the gray panels.

  subSplitter->SetMinimumPaneSize(20);   // Pane size may not be reduced to zero!

  // ********************************************

  wxPanel *topPanel = new wxPanel(splitter, wxID_ANY, wxDefaultPosition, wxDefaultSize);
  wxPanel *middlePanel = new wxPanel(subSplitter, wxID_ANY, wxDefaultPosition, wxDefaultSize);
  wxPanel *bottomPanel = new wxPanel(subSplitter, wxID_ANY, wxDefaultPosition, wxDefaultSize);
  
  splitter->SplitHorizontally(topPanel, subSplitter);
  subSplitter->SplitHorizontally(middlePanel, bottomPanel);

  topLevelSizer->Add(splitter, 1, wxEXPAND);

  // ****************************************************************
  // Top panel
  // ****************************************************************

  wxBoxSizer *sizer = NULL;       // Sizer for one of the panels
  wxBoxSizer *controlSizer = NULL;
  wxBoxSizer *subSizer = NULL;

  sizer = new wxBoxSizer(wxHORIZONTAL);
  controlSizer = new wxBoxSizer(wxVERTICAL);

  // Add the controls at the left side

  chkShowMainTrack = new wxCheckBox(topPanel, IDC_SHOW_MAIN_TRACK, "Main track");
  controlSizer->Add(chkShowMainTrack, 0, wxALL, 3);
  
  chkShowEggTrack = new wxCheckBox(topPanel, IDC_SHOW_EGG_TRACK, "EGG track");
  controlSizer->Add(chkShowEggTrack, 0, wxALL, 3);
  
  chkShowExtraTrack = new wxCheckBox(topPanel, IDC_SHOW_EXTRA_TRACK, "Extra track");
  controlSizer->Add(chkShowExtraTrack, 0, wxALL, 3);

  controlSizer->AddSpacer(10);

  // ********************************************

  subSizer = new wxBoxSizer(wxHORIZONTAL);
  
  button = new wxButton(topPanel, IDB_HORZ_ZOOM_MINUS, "-", wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
  button->SetMinSize(wxSize(button->GetSize().GetHeight(), button->GetSize().GetHeight()));
  subSizer->Add(button, 0, wxALIGN_CENTER | wxALL, 3);

  
  labHorzZoom = new wxStaticText(topPanel, wxID_ANY, "M.MM", 
      wxDefaultPosition, wxDefaultSize, wxST_NO_AUTORESIZE | wxALIGN_CENTER_HORIZONTAL);
  subSizer->Add(labHorzZoom, 0, wxALIGN_CENTER | wxALL, 3);

  button = new wxButton(topPanel, IDB_HORZ_ZOOM_PLUS, "+", wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
  button->SetMinSize(wxSize(button->GetSize().GetHeight(), button->GetSize().GetHeight()));
  subSizer->Add(button, 0, wxALIGN_CENTER | wxALL, 3);

  controlSizer->Add(subSizer, 0, wxGROW | wxALL, 3);

  controlSizer->AddSpacer(10);

  // ********************************************

  button = new wxButton(topPanel, IDB_VERT_ZOOM_PLUS, "+", wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
  button->SetMinSize(wxSize(button->GetSize().GetHeight(), button->GetSize().GetHeight()));
  controlSizer->Add(button, 0, wxALIGN_CENTER | wxALL, 3);

  labVertZoom = new wxStaticText(topPanel, wxID_ANY, "MM.MM", 
      wxDefaultPosition, wxDefaultSize, wxST_NO_AUTORESIZE | wxALIGN_CENTER_HORIZONTAL);
  controlSizer->Add(labVertZoom, 0, wxALIGN_CENTER | wxALL, 3);

  button = new wxButton(topPanel, IDB_VERT_ZOOM_MINUS, "-", wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
  button->SetMinSize(wxSize(button->GetSize().GetHeight(), button->GetSize().GetHeight()));
  controlSizer->Add(button, 0, wxALIGN_CENTER | wxALL, 3);

  controlSizer->AddSpacer(10);

  // ********************************************

  button = new wxButton(topPanel, IDB_EXCHANGE_MAIN_EGG, "Main <-> EGG");
  controlSizer->Add(button, 0, wxGROW | wxALL, 3);

  button = new wxButton(topPanel, IDB_EXCHANGE_MAIN_EXTRA, "Main <-> Extra");
  controlSizer->Add(button, 0, wxGROW | wxALL, 3);

  // ********************************************

  sizer->Add(controlSizer, 0, wxGROW);

  // Add the signal picture and the scroll bar

  picSignal = new SignalPicture(topPanel, this);
  scrTime = new wxScrollBar(topPanel, IDS_SCROLL_BAR);
  scrTime->SetScrollbar(0, 500, Data::TRACK_DURATION_S * SAMPLING_RATE, 499);

  subSizer = new wxBoxSizer(wxVERTICAL);
  subSizer->Add(picSignal, 1, wxGROW);
  subSizer->Add(scrTime, 0, wxGROW);
  
  sizer->Add(subSizer, 1, wxGROW);
  
  // ********************************************

  topPanel->SetSizer(sizer);

  // ****************************************************************
  // Middle panel
  // ****************************************************************

  sizer = new wxBoxSizer(wxHORIZONTAL);
  controlSizer = new wxBoxSizer(wxVERTICAL);

  // Add the controls at the left side

  wxString spectrogramChoices[Data::NUM_TRACKS] =
  {
    "Main track",
    "EGG track",
    "Extra track"
  };
  radSpectrogramChoice = new wxRadioBox(middlePanel, IDR_SPECTROGRAM_CHOICE,
    "Spectrogram of", wxDefaultPosition, wxDefaultSize, Data::NUM_TRACKS, spectrogramChoices,
    1, wxRA_SPECIFY_COLS);

  controlSizer->Add(radSpectrogramChoice, 0, wxALL, 3);

  controlSizer->AddSpacer(10);

  // ********************************************

  chkShowSpectrogramText = new wxCheckBox(middlePanel, IDC_SHOW_SPECTROGRAM_TEXT, "Show text");
  controlSizer->Add(chkShowSpectrogramText, 0, wxALL, 3);
  
  // ********************************************

  sizer->Add(controlSizer);

  // Add the spectrogram pictrue

  picSpectrogram = new SpectrogramPicture(middlePanel, this);
  sizer->Add(picSpectrogram, 1, wxGROW);

  // ********************************************

  middlePanel->SetSizer(sizer);

  // ****************************************************************
  // Bottom panel
  // ****************************************************************

  sizer = new wxBoxSizer(wxHORIZONTAL);
  controlSizer = new wxBoxSizer(wxVERTICAL);

  // Add the controls at the left side

  label = new wxStaticText(bottomPanel, wxID_ANY, "");
  label->Disable();
  controlSizer->Add(label, 0, wxALL, 3);

  controlSizer->AddSpacer(10);

  // ********************************************

  sizer->Add(controlSizer);

  // Add the simple spectrum pictrue

  picSimpleSpectrum = new SimpleSpectrumPicture(bottomPanel);
  sizer->Add(picSimpleSpectrum, 1, wxGROW);

  // ********************************************

  bottomPanel->SetSizer(sizer);

  // ****************************************************************
  // Set the top-level sizer for this window.
  // ****************************************************************

  this->SetSizer(topLevelSizer);
}


// ****************************************************************************
/// Is called, when one of the child windows has requested the update of
/// other child windows.
// ****************************************************************************

void SignalPage::OnUpdateRequest(wxCommandEvent &event)
{
  if (event.GetInt() == REFRESH_PICTURES)
  {
    picSignal->Refresh();
    picSpectrogram->Refresh();
    picSimpleSpectrum->Refresh();
  }
  else
  if (event.GetInt() == UPDATE_PICTURES)
  {
    picSignal->Refresh();
    picSignal->Update();
    picSpectrogram->Refresh();
    picSpectrogram->Update();
    picSimpleSpectrum->Refresh();
    picSimpleSpectrum->Update();
  }
  else
  if (event.GetInt() == REFRESH_PICTURES_AND_CONTROLS)
  {
    updateWidgets();
  }
}


// ****************************************************************************
// ****************************************************************************

void SignalPage::OnScrollEvent(wxScrollEvent &event)
{
  data->centerPos_pt = event.GetPosition();
  updateWidgets();
}

// ****************************************************************************
// ****************************************************************************

void SignalPage::OnShowMainTrackClicked(wxCommandEvent &event)
{
  data->showTrack[ Data::MAIN_TRACK ] = event.IsChecked();
  updateWidgets();
}

// ****************************************************************************
// ****************************************************************************

void SignalPage::OnShowEggTrackClicked(wxCommandEvent &event)
{
  data->showTrack[ Data::EGG_TRACK ] = event.IsChecked();
  updateWidgets();
}

// ****************************************************************************
// ****************************************************************************

void SignalPage::OnShowExtraTrackClicked(wxCommandEvent &event)
{
  data->showTrack[ Data::EXTRA_TRACK ] = event.IsChecked();
  updateWidgets();
}

// ****************************************************************************
// ****************************************************************************

void SignalPage::OnOsziHorzZoomPlusClicked(wxCommandEvent &event)
{
  if (data->oscillogramVisTimeRange_pt > 700)
  {
    data->oscillogramVisTimeRange_pt /= 2;
    data->spectrogramVisTimeRange_pt /= 2;
  }
  updateWidgets();
}

// ****************************************************************************
// ****************************************************************************

void SignalPage::OnOsziHorzZoomMinusClicked(wxCommandEvent &event)
{
  if (data->oscillogramVisTimeRange_pt < 5000)
  {
    data->oscillogramVisTimeRange_pt *= 2;
    data->spectrogramVisTimeRange_pt *= 2;
  }
  updateWidgets();
}

// ****************************************************************************
// ****************************************************************************

void SignalPage::OnOsziVertZoomPlusClicked(wxCommandEvent &event)
{
  if (data->oscillogramAmpZoom < 8.0) 
  { 
    data->oscillogramAmpZoom*= 1.5; 
  }  
  updateWidgets();
}

// ****************************************************************************
// ****************************************************************************

void SignalPage::OnOsziVertZoomMinusClicked(wxCommandEvent &event)
{
  if (data->oscillogramAmpZoom > 0.25) 
  { 
    data->oscillogramAmpZoom/= 1.5; 
  }
  updateWidgets();
}


// ****************************************************************************
// ****************************************************************************

void SignalPage::OnExchangeMainEggClicked(wxCommandEvent &event)
{
  int i;
  signed short temp;
  Signal16 *s1 = data->track[Data::MAIN_TRACK];
  Signal16 *s2 = data->track[Data::EGG_TRACK];

  int N = s1->N;
  if (s2->N < N) { N = s2->N; }

  for (i=0; i < N; i++)
  {
    temp = s1->x[i];
    s1->x[i] = s2->x[i];
    s2->x[i] = temp;
  }

  // ****************************************************************
  // Exchange signals for F0...F3.
  // ****************************************************************

  vector<double> dummy;

  dummy = data->f0Signal[Data::MAIN_TRACK];
  data->f0Signal[Data::MAIN_TRACK] = data->f0Signal[Data::EGG_TRACK];
  data->f0Signal[Data::EGG_TRACK] = dummy;

  updateWidgets();
}

// ****************************************************************************
// ****************************************************************************

void SignalPage::OnExchangeMainExtraClicked(wxCommandEvent &event)
{
  int i;
  signed short temp;
  Signal16 *s1 = data->track[Data::MAIN_TRACK];
  Signal16 *s2 = data->track[Data::EXTRA_TRACK];

  int N = s1->N;
  if (s2->N < N) { N = s2->N; }

  for (i=0; i < N; i++)
  {
    temp = s1->x[i];
    s1->x[i] = s2->x[i];
    s2->x[i] = temp;
  }

  // ****************************************************************
  // Exchange signals for F0...F3.
  // ****************************************************************

  vector<double> dummy;

  dummy = data->f0Signal[Data::MAIN_TRACK];
  data->f0Signal[Data::MAIN_TRACK] = data->f0Signal[Data::EXTRA_TRACK];
  data->f0Signal[Data::EXTRA_TRACK] = dummy;

  updateWidgets();
}


// ****************************************************************************
// ****************************************************************************

void SignalPage::OnSpectrogramChoice(wxCommandEvent &event)
{
  data->selectedSpectrogram = event.GetInt();
  updateWidgets();
}


// ****************************************************************************
// ****************************************************************************

void SignalPage::OnShowSpectrogramText(wxCommandEvent &event)
{
  data->showSpectrogramText = !data->showSpectrogramText;
  updateWidgets();
}


// ****************************************************************************
/// Call the dialog for the analysis settings.
// ****************************************************************************

void SignalPage::OnAnalysisSettings(wxCommandEvent &event)
{
  AnalysisSettingsDialog dialog(this, this, picSpectrogram->spectrogramPlot);
  dialog.ShowModal();
}

// ****************************************************************************
/// Display the non-modal dialog for the analysis results of F0, F1, F2, ...
// ****************************************************************************

void SignalPage::OnAnalysisResults(wxCommandEvent &event)
{
  AnalysisResultsDialog *dialog = AnalysisResultsDialog::getInstance();
  dialog->SetParent(this);
  dialog->Show(true);
}


// ****************************************************************************
/// Calculate the F0 track.
// ****************************************************************************

void SignalPage::OnCalcF0(wxCommandEvent &event)
{
  data->estimateF0(this);
  updateWidgets();
}


// ****************************************************************************
/// Calculate the voice quality signal.
// ****************************************************************************

void SignalPage::OnCalcVoiceQuality(wxCommandEvent &event)
{
  data->estimateVoiceQuality(this);
  updateWidgets();
}


// ****************************************************************************
