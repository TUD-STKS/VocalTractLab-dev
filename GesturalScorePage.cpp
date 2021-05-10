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

#include "GesturalScorePage.h"
#include "AnalysisResultsDialog.h"
#include "AnnotationDialog.h"
#include "SilentMessageBox.h"
#include "Backend/SoundLib.h"

#include <stdio.h>
#include <wx/config.h>
#include <wx/statline.h>
#include <wx/splitter.h>
#include <wx/gbsizer.h>
#include <wx/progdlg.h>
#include <wx/filename.h>
#include <wx/wupdlock.h>

using namespace std;

// ****************************************************************************
// Ids.
// ****************************************************************************

static const int IDS_HORZ_SCROLL_BAR  = 4000;
static const int IDS_LOWER_OFFSET     = 4001;
static const int IDS_UPPER_OFFSET     = 4002;
static const int IDS_SPLITTER         = 4003;

static const int IDC_NEUTRAL_GESTURE  = 4010;
static const int IDL_NOMINAL_VALUE    = 4011;
static const int IDL_CONTINUAL_VALUE  = 4012;
static const int IDT_CONTINUAL_VALUE  = 4013;
static const int IDL_START_TIME       = 4014;
static const int IDE_DURATION         = 4015;
static const int IDL_TIME_CONSTANT    = 4016;
static const int IDL_SLOPE            = 4017;
static const int IDS_TIME_CONSTANT    = 4019;
static const int IDS_SLOPE            = 4020;
static const int IDB_TIME_ZOOM_MINUS  = 4030;
static const int IDB_TIME_ZOOM_PLUS   = 4031;

static const int IDL_MARK_POS         = 4032;
static const int IDR_DISPLAY_CHOICES  = 4040;

static const int IDC_SHOW_EXTRA_TRACK = 4051;
static const int IDC_SHOW_SONAGRAM    = 4052;
static const int IDC_SHOW_SEGMENTATION = 4053;
static const int IDC_SHOW_MODEL_F0_CURVE = 4054;
static const int IDC_SHOW_ANIMATION    = 4055;
static const int IDC_NORMALIZE_AMPLITUDE = 4056;

static const int IDB_SYNTHESIZE       = 4060;
static const int IDB_ACOUSTICS        = 4061;
static const int IDB_SHOW_VOCAL_TRACT = 4062;
static const int IDB_SHOW_GLOTTIS     = 4063;
static const int IDB_ANALYSIS_SETTINGS = 4064;
static const int IDB_CALC_F0          = 4065;
static const int IDB_CALC_VOICE_QUALITY = 4066;
static const int IDB_ANALYSIS_RESULTS = 4067;
static const int IDB_ANNOTATION_DIALOG = 4068;


// ****************************************************************************
// The event table.
// ****************************************************************************

BEGIN_EVENT_TABLE(GesturalScorePage, wxPanel)
  // Custom event handler for update requests by child widgets.
  EVT_COMMAND(wxID_ANY, updateRequestEvent, GesturalScorePage::OnUpdateRequest)

  EVT_MENU(SYNTHESIS_THREAD_EVENT, GesturalScorePage::OnSynthesisThreadEvent)

  EVT_COMMAND_SCROLL(IDS_HORZ_SCROLL_BAR, GesturalScorePage::OnHorzScrollBar)
  EVT_COMMAND_SCROLL(IDS_LOWER_OFFSET, GesturalScorePage::OnLowerOffsetScrollBar)
  EVT_COMMAND_SCROLL(IDS_UPPER_OFFSET, GesturalScorePage::OnUpperOffsetScrollBar)
  EVT_COMBOBOX(IDL_NOMINAL_VALUE, GesturalScorePage::OnNominalValueSelected)
  EVT_TEXT_ENTER(IDL_NOMINAL_VALUE, GesturalScorePage::OnNominalValueEntered)
  EVT_CHECKBOX(IDC_NEUTRAL_GESTURE, GesturalScorePage::OnNeutralGesture)
  EVT_SPINCTRL(IDE_DURATION, GesturalScorePage::OnDurationChanged)
  EVT_COMMAND_SCROLL(IDS_TIME_CONSTANT, GesturalScorePage::OnTimeConstantChanged)
  EVT_COMMAND_SCROLL(IDS_SLOPE, GesturalScorePage::OnSlopeChanged)
  EVT_BUTTON(IDB_TIME_ZOOM_MINUS, GesturalScorePage::OnTimeZoomMinus)
  EVT_BUTTON(IDB_TIME_ZOOM_PLUS, GesturalScorePage::OnTimeZoomPlus)
  EVT_RADIOBOX(IDR_DISPLAY_CHOICES, GesturalScorePage::OnDisplayChoice)
  EVT_CHECKBOX(IDC_SHOW_EXTRA_TRACK, GesturalScorePage::OnShowExtraTrack)
  EVT_CHECKBOX(IDC_SHOW_SONAGRAM, GesturalScorePage::OnShowSonagram)
  EVT_CHECKBOX(IDC_SHOW_SEGMENTATION, GesturalScorePage::OnShowSegmentation)
  EVT_CHECKBOX(IDC_SHOW_MODEL_F0_CURVE, GesturalScorePage::OnShowModelF0Curve)
  EVT_CHECKBOX(IDC_SHOW_ANIMATION, GesturalScorePage::OnShowAnimation)
  EVT_CHECKBOX(IDC_NORMALIZE_AMPLITUDE, GesturalScorePage::OnNormalizeAmplitude)

  EVT_BUTTON(IDB_SYNTHESIZE, GesturalScorePage::OnSynthesize)
  EVT_BUTTON(IDB_ACOUSTICS, GesturalScorePage::OnAcoustics)
  EVT_BUTTON(IDB_SHOW_VOCAL_TRACT, GesturalScorePage::OnShowVocalTract)
  EVT_BUTTON(IDB_SHOW_GLOTTIS, GesturalScorePage::OnShowGlottis)
  EVT_BUTTON(IDB_ANALYSIS_SETTINGS, GesturalScorePage::OnAnalysisSettings)
  EVT_BUTTON(IDB_CALC_F0, GesturalScorePage::OnCalcF0)
  EVT_BUTTON(IDB_CALC_VOICE_QUALITY, GesturalScorePage::OnCalcVoiceQuality)
  EVT_BUTTON(IDB_ANALYSIS_RESULTS, GesturalScorePage::OnAnalysisResults)
  EVT_BUTTON(IDB_ANNOTATION_DIALOG, GesturalScorePage::OnAnnotationDialog)
  
  EVT_SPLITTER_SASH_POS_CHANGING(IDS_SPLITTER, GesturalScorePage::OnSplitterPosChanged)
  EVT_SIZE(GesturalScorePage::OnResize)
END_EVENT_TABLE()

// ****************************************************************************
// ****************************************************************************

GesturalScorePage::GesturalScorePage(wxWindow *parent) : wxPanel(parent)
{
  data = Data::getInstance();

  initWidgets();
  updateWidgets();
}

// ****************************************************************************
/// Init. all widgets.
// ****************************************************************************

void GesturalScorePage::initWidgets()
{
//  int i;
  wxSizer *sizer;
  wxButton *button;
  wxStaticText *label;
  wxStaticBox *staticBox;

  // ****************************************************************
  // Left side sizer.
  // ****************************************************************

  wxBoxSizer *leftSizer = new wxBoxSizer(wxVERTICAL);

  // ****************************************************************
  // Static box with parameters and options for a gesture.
  // ****************************************************************

  wxStaticBoxSizer *gestureSizer = new wxStaticBoxSizer(wxVERTICAL, this, "Gesture");
  staticBox = gestureSizer->GetStaticBox();
  leftSizer->Add(gestureSizer, 0, wxALL |wxGROW, 3);

  chkNeutralGesture = new wxCheckBox(staticBox, IDC_NEUTRAL_GESTURE, "neutral/absent");
  gestureSizer->Add(chkNeutralGesture, 0, wxALL, 2);

  // Label for the value

  labValue = new wxStaticText(staticBox, wxID_ANY, "Value:");
  gestureSizer->Add(labValue, 1, wxGROW | wxALL, 2);

  // Text box for continual value and drop down box for the nominal value
  
  sizer = new wxBoxSizer(wxHORIZONTAL);

  txtContinualValue = new wxTextCtrl(staticBox, IDT_CONTINUAL_VALUE, "", wxDefaultPosition,
    wxDefaultSize, wxTE_PROCESS_ENTER);
  txtContinualValue->SetMinSize(this->FromDIP(wxSize(50, -1) ));
  // Connect the lost-focus-event to the event handler
  txtContinualValue->Connect(wxEVT_KILL_FOCUS, 
    wxFocusEventHandler(GesturalScorePage::OnContinualValueChanged), NULL, this);
  this->Connect(IDT_CONTINUAL_VALUE, wxEVT_COMMAND_TEXT_ENTER,
    wxCommandEventHandler(GesturalScorePage::OnContinualValueEntered));
  sizer->Add(txtContinualValue, 0, wxGROW | wxRIGHT, 5);

  lstNominalValue = new wxComboBox(staticBox, IDL_NOMINAL_VALUE, wxEmptyString, wxDefaultPosition,
    wxDefaultSize, 0, 0, wxTE_PROCESS_ENTER);
  lstNominalValue->SetMinSize(this->FromDIP(wxSize(100, -1) ));
  sizer->Add(lstNominalValue, 1, wxGROW);

  gestureSizer->Add(sizer, 0, wxALL | wxGROW, 2);

  // Slope label

  sizer = new wxBoxSizer(wxHORIZONTAL);
  label = new wxStaticText(staticBox, wxID_ANY, "Slope:");
  sizer->Add(label, 0, wxGROW | wxRIGHT, 5);
  labSlope = new wxStaticText(staticBox, IDL_SLOPE, "0.0");
  sizer->Add(labSlope, 0);
  gestureSizer->Add(sizer, 0, wxALL | wxGROW, 2);

  // Slope scroll bar

  scrSlope = new wxScrollBar(staticBox, IDS_SLOPE);
  gestureSizer->Add(scrSlope, 1, wxALL | wxGROW, 2);

  // Start time label

  sizer = new wxBoxSizer(wxHORIZONTAL);
  label = new wxStaticText(staticBox, wxID_ANY, "Start time:");
  sizer->Add(label, 0, wxGROW | wxRIGHT, 5);
  labStartTime = new wxStaticText(staticBox, IDL_START_TIME, "0.0 s");
  sizer->Add(labStartTime, 0);
  gestureSizer->Add(sizer, 0, wxALL | wxGROW, 2);

  // Duration spin control

  sizer = new wxBoxSizer(wxHORIZONTAL);
  label = new wxStaticText(staticBox, wxID_ANY, "Duration:");
  sizer->Add(label, 0, wxGROW | wxRIGHT, 5);

  txtDuration = new wxSpinCtrl(staticBox, IDE_DURATION, "0", wxDefaultPosition, this->FromDIP(wxSize(80, -1)), wxTE_PROCESS_ENTER);
  txtDuration->SetRange(MIN_GESTURE_DURATION_MS, MAX_GESTURE_DURATION_MS);
  this->Connect(IDE_DURATION, wxEVT_COMMAND_TEXT_ENTER,
    wxCommandEventHandler(GesturalScorePage::OnDurationEntered));
  sizer->Add(txtDuration, 0, wxGROW | wxRIGHT, 5);

  label = new wxStaticText(staticBox, wxID_ANY, "ms");
  sizer->Add(label, 0, wxGROW | wxRIGHT, 5);

  gestureSizer->Add(sizer, 0, wxALL | wxGROW, 2);

  // Time constant label

  sizer = new wxBoxSizer(wxHORIZONTAL);
  label = new wxStaticText(staticBox, wxID_ANY, "Time constant:");
  sizer->Add(label, 0, wxGROW | wxRIGHT, 5);
  labTimeConstant = new wxStaticText(staticBox, IDL_TIME_CONSTANT, "0.0 s");
  sizer->Add(labTimeConstant, 0);
  gestureSizer->Add(sizer, 0, wxALL | wxGROW, 2);

  // Time constant scroll bar

  scrTimeConstant = new wxScrollBar(staticBox, IDS_TIME_CONSTANT);
  gestureSizer->Add(scrTimeConstant, 1, wxALL | wxGROW, 2);


  // ****************************************************************
  // Some ungrouped controls.
  // ****************************************************************

  // Zoom buttons for time axis
  
  sizer = new wxBoxSizer(wxHORIZONTAL);
  label = new wxStaticText(this, wxID_ANY, "Time axis:");
  sizer->Add(label, 0, wxRIGHT | wxGROW, 5);

  button = new wxButton(this, IDB_TIME_ZOOM_MINUS, "-");
  button->SetMinSize(this->FromDIP(wxSize(20, 20) ));
  sizer->Add(button, 0, wxRIGHT | wxGROW, 5);

  button = new wxButton(this, IDB_TIME_ZOOM_PLUS, "+");
  button->SetMinSize(this->FromDIP(wxSize(20, 20) ));
  sizer->Add(button, 0, wxRIGHT | wxGROW, 5);

  leftSizer->Add(sizer, 0, wxALL | wxGROW, 3);

  // Label for the mark position

  sizer = new wxBoxSizer(wxHORIZONTAL);
  label = new wxStaticText(this, wxID_ANY, "Mark pos.:");
  sizer->Add(label, 0, wxRIGHT | wxGROW, 5);

  labMark = new wxStaticText(this, IDL_MARK_POS, "0.0 s");
  sizer->Add(labMark, 0, wxRIGHT | wxGROW, 5);

  leftSizer->Add(sizer, 0, wxALL | wxGROW, 3);

  // ****************************************************************
  // Display choices.
  // ****************************************************************

  const int NUM_DISPLAY_CHOICES = 2;
  const wxString DISPLAY_CHOICE[NUM_DISPLAY_CHOICES] =
  {
    "Show gestural score",
    "Show motor program"
  };

  radDisplayChoices = new wxRadioBox(this, IDR_DISPLAY_CHOICES, "Display choice",
    wxDefaultPosition, wxDefaultSize, NUM_DISPLAY_CHOICES, DISPLAY_CHOICE, 1, wxRA_SPECIFY_COLS);

  leftSizer->Add(radDisplayChoices, 0, wxALL | wxGROW, 3);

  // ****************************************************************
  // Audio signal display options.
  // ****************************************************************

  wxStaticBoxSizer *audioSizer = new wxStaticBoxSizer(wxVERTICAL, this, "Audio signals");
  staticBox = audioSizer->GetStaticBox();
  leftSizer->Add(audioSizer, 0, wxALL | wxGROW, 3);

  chkShowExtraTrack = new wxCheckBox(staticBox, IDC_SHOW_EXTRA_TRACK, "Show extra track");
  audioSizer->Add(chkShowExtraTrack, 0, wxALL, 2);

  chkShowSonagram = new wxCheckBox(staticBox, IDC_SHOW_SONAGRAM, "Show sonagram");
  audioSizer->Add(chkShowSonagram, 0, wxALL, 2);

  chkShowSegmentation = new wxCheckBox(staticBox, IDC_SHOW_SEGMENTATION, "Show segmentation");
  audioSizer->Add(chkShowSegmentation, 0, wxALL, 2);

  chkShowModelF0Curve = new wxCheckBox(staticBox, IDC_SHOW_MODEL_F0_CURVE, "Show model F0 curve");
  audioSizer->Add(chkShowModelF0Curve, 0, wxALL, 2);

  // ****************************************************************
  // Some buttons.
  // ****************************************************************

  chkShowAnimation = new wxCheckBox(this, IDC_SHOW_ANIMATION, "Show animation");
  leftSizer->Add(chkShowAnimation, 0, wxALL | wxGROW, 3);

  chkNormalizeAmplitude = new wxCheckBox(this, IDC_NORMALIZE_AMPLITUDE, "Normalize audio amplitude");
  leftSizer->Add(chkNormalizeAmplitude, 0, wxALL | wxGROW, 3);

  button = new wxButton(this, IDB_SYNTHESIZE, "Synthesize");
  leftSizer->Add(button, 0, wxALL | wxGROW, 3);
  
  button = new wxButton(this, IDB_ACOUSTICS, "Acoustics (time-domain)");
  leftSizer->Add(button, 0, wxALL | wxGROW, 3);

  button = new wxButton(this, IDB_SHOW_VOCAL_TRACT, "Show vocal tract");
  leftSizer->Add(button, 0, wxALL | wxGROW, 3);

  button = new wxButton(this, IDB_SHOW_GLOTTIS, "Show glottis");
  leftSizer->Add(button, 0, wxALL | wxGROW, 3);

  leftSizer->AddSpacer(10);

  button = new wxButton(this, IDB_CALC_F0, "Calculate F0");
  leftSizer->Add(button, 0, wxALL | wxGROW, 3);

  button = new wxButton(this, IDB_CALC_VOICE_QUALITY, "Calc. voice quality");
  leftSizer->Add(button, 0, wxALL | wxGROW, 3);

  button = new wxButton(this, IDB_ANALYSIS_RESULTS, "Analysis results");
  leftSizer->Add(button, 0, wxALL | wxGROW, 3);

  button = new wxButton(this, IDB_ANALYSIS_SETTINGS, "Analysis settings");
  leftSizer->Add(button, 0, wxALL | wxGROW, 3);

  button = new wxButton(this, IDB_ANNOTATION_DIALOG, "Annotation dialog");
  leftSizer->Add(button, 0, wxALL | wxGROW, 3);

  // ****************************************************************
  // Static line to separate the left and right part.
  // ****************************************************************

  wxStaticLine *verticalLine = new wxStaticLine(this, wxID_ANY, wxDefaultPosition, 
    wxDefaultSize, wxLI_VERTICAL);

  // ****************************************************************
  // Main splitter on the right side.
  // ****************************************************************

  splitter = new wxSplitterWindow(this, IDS_SPLITTER,
    wxDefaultPosition, wxDefaultSize, wxSP_3DSASH | wxSP_LIVE_UPDATE);
  
  splitter->SetMinimumPaneSize(20);   // Pane size may not be reduced to zero!

  wxPanel *upperPanel = new wxPanel(splitter, wxID_ANY, wxDefaultPosition, wxDefaultSize);
  wxPanel *lowerPanel = new wxPanel(splitter, wxID_ANY, wxDefaultPosition, wxDefaultSize);
  splitter->SplitHorizontally(upperPanel, lowerPanel);
  splitter->SetSashPosition(this->FromDIP(300));

  // ****************************************************************
  // Panel above the splitter.
  // ****************************************************************

  wxBoxSizer *upperSizer = new wxBoxSizer(wxVERTICAL);
  upperPanel->SetSizer(upperSizer);

  // Put the time axis picture next to an empty space on top.
  
  sizer = new wxBoxSizer(wxHORIZONTAL);

  timeAxisPicture = new TimeAxisPicture(upperPanel);
  timeAxisPicture->SetMinSize(this->FromDIP(wxSize(-1, 25) ));
  sizer->Add(timeAxisPicture, 1, wxGROW);

  sizer->AddSpacer(this->FromDIP(20));

  upperSizer->Add(sizer, 0, wxBOTTOM | wxGROW, 5);

  // Put the signal comparison picture next to a vertical scroll bar.

  sizer = new wxBoxSizer(wxHORIZONTAL);

  signalComparisonPicture = new SignalComparisonPicture(upperPanel, this);
  sizer->Add(signalComparisonPicture, 1, wxGROW);

  scrUpperOffset = new wxScrollBar(upperPanel, IDS_UPPER_OFFSET, wxDefaultPosition,
    wxDefaultSize, wxSB_VERTICAL);
  scrUpperOffset->SetMinSize(this->FromDIP(wxSize(20, -1) ));
  scrUpperOffset->SetScrollbar(0, 1, 100, 1);
  sizer->Add(scrUpperOffset, 0, wxGROW);

  upperSizer->Add(sizer, 1, wxGROW);

  // ****************************************************************
  // Panel below the splitter.
  // ****************************************************************

  wxBoxSizer *lowerSizer = new wxBoxSizer(wxVERTICAL);
  lowerPanel->SetSizer(lowerSizer);

  // Put the horizontal scrollbar embraced by two spacers below.
  
  sizer = new wxBoxSizer(wxHORIZONTAL);

  // Add a dummy label as spacer at the left.
  label =new wxStaticText(lowerPanel, wxID_ANY, "");
  label->SetMinSize(this->FromDIP(wxSize(Data::LEFT_SCORE_MARGIN, -1) ));
  sizer->Add(label, 0, wxGROW);

  horzScrollBar = new wxScrollBar(lowerPanel, IDS_HORZ_SCROLL_BAR, wxDefaultPosition,
    wxDefaultSize, wxSB_HORIZONTAL);
  horzScrollBar->SetMinSize(this->FromDIP(wxSize(-1, 20) ));
  horzScrollBar->SetScrollbar(0, 100, 60001, 100);    // Scroll units are ms.
  sizer->Add(horzScrollBar, 1, wxGROW);

  sizer->AddSpacer(20);

  lowerSizer->Add(sizer, 0, wxGROW);

  // Put the main picture and the vertical scrollbar on a sizer
  // next to each other.

  sizer = new wxBoxSizer(wxHORIZONTAL);

  gesturalScorePicture = new GesturalScorePicture(lowerPanel, this);
  sizer->Add(gesturalScorePicture, 1, wxGROW);

  scrLowerOffset = new wxScrollBar(lowerPanel, IDS_LOWER_OFFSET, wxDefaultPosition,
    wxDefaultSize, wxSB_VERTICAL);
  scrLowerOffset->SetMinSize(this->FromDIP(wxSize(20, -1) ));
  scrLowerOffset->SetScrollbar(0, 1, 100, 1);
  sizer->Add(scrLowerOffset, 0, wxGROW);

  lowerSizer->Add(sizer, 1, wxGROW);


  // ****************************************************************
  // Top level sizer.
  // ****************************************************************

  wxBoxSizer *topLevelSizer = new wxBoxSizer(wxHORIZONTAL);

  topLevelSizer->Add(leftSizer, 0, wxALL, 5);
  topLevelSizer->Add(verticalLine, 0, wxGROW | wxALL, 2);
  topLevelSizer->Add(splitter, 1, wxEXPAND);

  this->SetSizer(topLevelSizer);
}


// ****************************************************************************
/// Updates all widgets.
// ****************************************************************************

void GesturalScorePage::updateWidgets()
{
  // ****************************************************************
  // Calculate the targets and time functions of the parameters.
  // ****************************************************************

  data->gesturalScore->calcCurves();

  // Label for the mark position

  labMark->SetLabel(wxString::Format("%2.3f s", data->gesturalScoreMark_s));

  // ****************************************************************
  // Display choice.
  // ****************************************************************

  if (gesturalScorePicture->showGesturalScore)
  {
    radDisplayChoices->SetSelection(0);
  }
  else
  {
    radDisplayChoices->SetSelection(1);
  }

  // ****************************************************************
  // Audio signal display options.
  // ****************************************************************

  chkShowExtraTrack->SetValue( signalComparisonPicture->showExtraTrack );
  chkShowSonagram->SetValue( signalComparisonPicture->showSonagrams );
  chkShowSegmentation->SetValue( signalComparisonPicture->showSegmentation );
  chkShowModelF0Curve->SetValue( signalComparisonPicture->showModelF0Curve );

  chkShowAnimation->SetValue( data->showAnimation );
  chkNormalizeAmplitude->SetValue(data->normalizeAmplitude);

  // ****************************************************************
  // Vertical scrollbar at the gestural score picture.
  // ****************************************************************

  double virtualHeight = gesturalScorePicture->getVirtualHeight();
  double windowHeight = gesturalScorePicture->getWindowHeight();
  if (virtualHeight > windowHeight)
  {
    scrLowerOffset->Enable(true);
    
    if (virtualHeight < 1.0)
    {
      virtualHeight = 1.0;
    }
    double d = windowHeight / virtualHeight;
    if (d > 0.9)
    {
      d = 0.9;
    }
    int thumbSize = (int)(100.0 / (1 - d));
    int range = 100 + thumbSize;
    int pos = (int)(gesturalScorePicture->getVertOffset_percent() + 0.5);

    scrLowerOffset->SetScrollbar(pos, thumbSize, range, thumbSize);
  }
  else
  {
    scrLowerOffset->Enable(false);
  }

  // ****************************************************************
  // Time scroll bar.
  // ****************************************************************

  double pos_ms = data->gsTimeAxisGraph->abscissa.reference * 1000.0;
  int pos = (int)(pos_ms + 0.5);
  if (pos != horzScrollBar->GetThumbPosition())
  {
    horzScrollBar->SetThumbPosition(pos);
  }

  // ****************************************************************
  // Scrollbar at the signal comparison picture.
  // ****************************************************************

  virtualHeight = signalComparisonPicture->getVirtualHeight();
  windowHeight = signalComparisonPicture->getWindowHeight();
  if (virtualHeight > windowHeight)
  {
    scrUpperOffset->Enable(true);
    
    if (virtualHeight < 1.0)
    {
      virtualHeight = 1.0;
    }
    double d = windowHeight / virtualHeight;
    if (d > 0.9)
    {
      d = 0.9;
    }
    int thumbSize = (int)(100.0 / (1 - d));
    int range = 100 + thumbSize;
    int pos = (int)(signalComparisonPicture->getVertOffset_percent() + 0.5);

    scrUpperOffset->SetScrollbar(pos, thumbSize, range, thumbSize);
  }
  else
  {
    scrUpperOffset->Enable(false);
  }

  // ****************************************************************
  // Options for a gesture.
  // ****************************************************************

  Gesture *gesture = data->getSelectedGesture();
  if (gesture != NULL)
  {
    chkNeutralGesture->Enable(true);
    txtDuration->Enable(true);
    scrTimeConstant->Enable(true);

    GestureSequence *sequence = &data->gesturalScore->gestures[ data->selectedGestureType ];
    int pos;
    wxString st;

    // Controls to adjust the gesture value

    if (gesture->neutral)
    {
      lstNominalValue->Enable(false);
      txtContinualValue->Enable(false);
      scrTimeConstant->Enable(false);
      labValue->SetLabel("Value:");
    }
    else
    if (sequence->nominalValues)
    {
      lstNominalValue->Enable(true);
      txtContinualValue->Enable(false);
      labValue->SetLabel("Value:");

      // Fill the list.
      fillGestureValueList();
      // Set the current value of the gesture.
      lstNominalValue->SetValue(gesture->sVal);

    }
    else
    {
      lstNominalValue->Enable(false);
      txtContinualValue->Enable(true);

      st = wxString::Format("Value: %1.2f ... %1.2f %s", 
        sequence->minValue, sequence->maxValue, sequence->unit.c_str());
      labValue->SetLabel(st);

      st = wxString::Format("%1.2f", gesture->dVal);
      txtContinualValue->SetValue(st);
    }

    // Can the gesture have a slope ?
    
    if (sequence->maxSlope > sequence->minSlope)
    {
      scrSlope->Enable(true);

      // Label and scrollbar for the slope
      st = wxString::Format("%2.2f %s/s", gesture->slope, sequence->unit.c_str());
      labSlope->SetLabel(st);

      double d = 1000.0*(gesture->slope - sequence->minSlope) / (sequence->maxSlope - sequence->minSlope);
      pos = (int)(d + 0.5);
      scrSlope->SetScrollbar(pos, 0, 1000, 1);
    }
    else
    {
      labSlope->SetLabel("");
      scrSlope->Enable(false);
    }

    // Check box for "neutral gesture"

    chkNeutralGesture->SetValue(gesture->neutral);

    // Label for the start time

    st = wxString::Format("%1.3f s", sequence->getGestureBegin_s(data->selectedGestureIndex));
    labStartTime->SetLabel(st);

    // Spin control for the gesture duration.

    pos = (int)((gesture->duration_s*1000.0) + 0.5);
    txtDuration->SetValue(pos);

    // Scroll bar for the time constant (100 steps).

    double tauRange_s = sequence->maxTau_s - sequence->minTau_s;
    if (tauRange_s < 0.000001)
    {
      tauRange_s = 0.000001;
    }
    pos = (int)(100.0*(gesture->tau_s - sequence->minTau_s) / tauRange_s + 0.5);
    scrTimeConstant->SetScrollbar(pos, 1, 100, 1);

    // Label for the time constant
    
    st = wxString::Format("%1.1f ms", gesture->tau_s*1000.0);
    labTimeConstant->SetLabel(st);
  }
  else

  // ****************************************************************
  // No gesture is selected. Disable the corresponding controls.
  // ****************************************************************
  {
    chkNeutralGesture->Enable(false);
    lstNominalValue->Enable(false);
    txtContinualValue->Enable(false);
    txtDuration->Enable(false);
    scrTimeConstant->Enable(false);
    scrSlope->Enable(false);

    labSlope->SetLabel("");
    labValue->SetLabel("Value:");
  }

}


// ****************************************************************************
// ****************************************************************************

void GesturalScorePage::scrollLeft()
{
  double pos_s = data->gsTimeAxisGraph->abscissa.reference;
  pos_s-= 0.05;
  if (pos_s < 0.0)
  {
    pos_s = 0.0;
  }
  data->gsTimeAxisGraph->abscissa.reference = pos_s;

  // The pictures.
  gesturalScorePicture->Refresh();
  signalComparisonPicture->Refresh();
  timeAxisPicture->Refresh();

  // Update time scroll bar only.
  double pos_ms = data->gsTimeAxisGraph->abscissa.reference * 1000.0;
  int pos = (int)(pos_ms + 0.5);
  if (pos != horzScrollBar->GetThumbPosition())
  {
    horzScrollBar->SetThumbPosition(pos);
  }
}


// ****************************************************************************
// ****************************************************************************

void GesturalScorePage::scrollRight()
{
  double pos_s = data->gsTimeAxisGraph->abscissa.reference;
  pos_s+= 0.05;
  if (pos_s > 60.0)
  {
    pos_s = 60.0;
  }
  data->gsTimeAxisGraph->abscissa.reference = pos_s;

  // The pictures.
  gesturalScorePicture->Refresh();
  signalComparisonPicture->Refresh();
  timeAxisPicture->Refresh();

  // Update time scroll bar only.
  double pos_ms = data->gsTimeAxisGraph->abscissa.reference * 1000.0;
  int pos = (int)(pos_ms + 0.5);
  if (pos != horzScrollBar->GetThumbPosition())
  {
    horzScrollBar->SetThumbPosition(pos);
  }
}

// ****************************************************************************
// ****************************************************************************

void GesturalScorePage::selectNextLeftSegment()
{
  Data *data = Data::getInstance();
  if (data->selectedSegmentIndex > 0)
  {
    data->selectedSegmentIndex--;

    // Update the key-value pairs in the annotation dialog.
    AnnotationDialog *dialog = AnnotationDialog::getInstance(this);
    if (dialog->IsShown())
    {
      dialog->updateWidgets();
    }
    signalComparisonPicture->Refresh();
  }
}

// ****************************************************************************
// ****************************************************************************

void GesturalScorePage::selectNextRightSegment()
{
  Data *data = Data::getInstance();
  SegmentSequence *sequence = data->segmentSequence;
  if (data->selectedSegmentIndex < sequence->numSegments() - 1)
  {
    data->selectedSegmentIndex++;

    // Update the key-value pairs in the annotation dialog.
    AnnotationDialog *dialog = AnnotationDialog::getInstance(this);
    if (dialog->IsShown())
    {
      dialog->updateWidgets();
    }
    signalComparisonPicture->Refresh();
  }
}

// ****************************************************************************
/// Change between the gestural score picture and the motor program picture.
// ****************************************************************************

void GesturalScorePage::togglePicture()
{
  gesturalScorePicture->showGesturalScore = !gesturalScorePicture->showGesturalScore;
  gesturalScorePicture->Refresh();
  updateWidgets();  
}


// ****************************************************************************
// Lengthen the gestural score at the position of the mark by a short piece.
// ****************************************************************************

void GesturalScorePage::lengthenGesturalScore()
{
  const double DELTA_LENGTH_S = 0.005;    // orig: 0.005
  int i;
  int index;
  double pos_s = data->gesturalScoreMark_s;
  GesturalScore *gs = data->gesturalScore;

  for (i=0; i < GesturalScore::NUM_GESTURE_TYPES; i++)
  {
    index = gs->gestures[i].getIndexAt(pos_s);
    if (index != -1)
    {
      gs->gestures[i].getGesture(index)->duration_s+= DELTA_LENGTH_S;
    }
  }

  updateWidgets();
  gesturalScorePicture->Refresh();
  signalComparisonPicture->Refresh();
}


// ****************************************************************************
// Shorten the gestural score at the position of the mark by a short piece.
// ****************************************************************************

void GesturalScorePage::shortenGesturalScore()
{
  const double DELTA_LENGTH_S = 0.5/100.0;
  int i, k, m;
  int index;
  GestureSequence *s;
  Gesture *g;
  double pos_s = data->gesturalScoreMark_s;
  GesturalScore *gs = data->gesturalScore;

  for (i=0; i < GesturalScore::NUM_GESTURE_TYPES; i++)
  {
    s = &gs->gestures[i];
    index = s->getIndexAt(pos_s);
    if (index != -1)
    {
      g = s->getGesture(index);
      if (s->getGestureEnd_s(index) > pos_s + DELTA_LENGTH_S)
      {
        g->duration_s-= DELTA_LENGTH_S;
      }
      else
      {
        k = -1;
        for (m=index+1; (m < s->numGestures()) && (k == -1); m++)
        {
          if (s->getGestureEnd_s(m) > pos_s + DELTA_LENGTH_S) { k = m; }
        }
        
        if (k == -1)
        {
          int num = s->numGestures();
          for (m=index+1; m < num; m++) { s->deleteGesture(index+1); }
          g->duration_s = pos_s - s->getGestureBegin_s(index);
        }
        else
        {
          double newLengthK = s->getGestureEnd_s(k) - (pos_s + DELTA_LENGTH_S);
          s->getGesture(k)->duration_s = newLengthK;
          g->duration_s = pos_s - s->getGestureBegin_s(index);
          int num = k - index - 1;
          for (m=0; m < num; m++) { s->deleteGesture(index+1); }
        }

        if (g->duration_s <= 0.0) { s->deleteGesture(index); }
      }
    }
  }

  updateWidgets();
  gesturalScorePicture->Refresh();
  signalComparisonPicture->Refresh();
}


// ****************************************************************************
/// Fills the combo box with the possible nominal values for the selected 
/// gesture.
// ****************************************************************************

void GesturalScorePage::fillGestureValueList(bool forced)
{
  static int prevSelectedGestureType = -1;
  int i;
  wxString st;
  Gesture *gesture = data->getSelectedGesture();
  VocalTract *tract = data->vocalTract;

  if ((gesture != NULL) && 
      ((data->selectedGestureType != prevSelectedGestureType) || (forced)))
  {
    GestureSequence *sequence = &data->gesturalScore->gestures[ data->selectedGestureType ];
    wxArrayString stringList;

    // **************************************************************
    // Fill the list with vowel shapes.
    // **************************************************************

    if (data->selectedGestureType == GesturalScore::VOWEL_GESTURE)
    {
      for (i=0; i < (int)tract->shapes.size(); i++)
      {
        st = wxString( tract->shapes[i].name );
        if ((st.StartsWith("ll-") == false) && (st.StartsWith("tt-") == false) && 
            (st.StartsWith("tb-") == false))
        {
          stringList.Add(st);
        }
      }
    }
    else

    // **************************************************************
    // Fill the list with lip gestures.
    // **************************************************************

    if (data->selectedGestureType == GesturalScore::LIP_GESTURE)
    {
      for (i=0; i < (int)tract->shapes.size(); i++)
      {
        st = wxString( tract->shapes[i].name );
        if (st.StartsWith("ll-"))
        {
          if ((st.EndsWith("(a)")) || (st.EndsWith("(i)")) || (st.EndsWith("(u)")))
          {
            // Remove the last three chars.
            st.RemoveLast();
            st.RemoveLast();
            st.RemoveLast();
          }

          if (stringList.Index(st, true) == wxNOT_FOUND)
          {
            stringList.Add(st);
          }
        }
      }
    }
    else

    // **************************************************************
    // Fill the list with tongue-tip gestures.
    // **************************************************************

    if (data->selectedGestureType == GesturalScore::TONGUE_TIP_GESTURE)
    {
      for (i=0; i < (int)tract->shapes.size(); i++)
      {
        st = wxString( tract->shapes[i].name );
        if (st.StartsWith("tt-"))
        {
          if ((st.EndsWith("(a)")) || (st.EndsWith("(i)")) || (st.EndsWith("(u)")))
          {
            // Remove the last three chars.
            st.RemoveLast();
            st.RemoveLast();
            st.RemoveLast();
          }

          if (stringList.Index(st, true) == wxNOT_FOUND)
          {
            stringList.Add(st);
          }
        }
      }
    }
    else

    // **************************************************************
    // Fill the list with tongue-body gestures.
    // **************************************************************

    if (data->selectedGestureType == GesturalScore::TONGUE_BODY_GESTURE)
    {
      for (i=0; i < (int)tract->shapes.size(); i++)
      {
        st = wxString( tract->shapes[i].name );
        if (st.StartsWith("tb-"))
        {
          if ((st.EndsWith("(a)")) || (st.EndsWith("(i)")) || (st.EndsWith("(u)")))
          {
            // Remove the last three chars.
            st.RemoveLast();
            st.RemoveLast();
            st.RemoveLast();
          }

          if (stringList.Index(st, true) == wxNOT_FOUND)
          {
            stringList.Add(st);
          }
        }
      }
    }
    else

    // **************************************************************
    // Fill the list with glottal shapes.
    // **************************************************************

    if (data->selectedGestureType == GesturalScore::GLOTTAL_SHAPE_GESTURE)
    {
      Glottis *glottis = data->getSelectedGlottis();
      for (i=0; i < (int)glottis->shape.size(); i++)
      {
        stringList.Add( wxString(glottis->shape[i].name) );
      }
    }

    // **************************************************************
    // Fill the list with the collected strings.
    // **************************************************************

    lstNominalValue->Freeze();
    lstNominalValue->Clear();
    lstNominalValue->Append(stringList);
    lstNominalValue->Thaw();
  }

  // Keep in mind the current gesture type.
  prevSelectedGestureType = data->selectedGestureType;
}


// ****************************************************************************
/// Is called, when one of the child windows has requested the update of
/// other child windows.
// ****************************************************************************

void GesturalScorePage::OnUpdateRequest(wxCommandEvent &event)
{
  if (event.GetInt() == REFRESH_PICTURES)
  {
    gesturalScorePicture->Refresh();
    signalComparisonPicture->Refresh();
    timeAxisPicture->Refresh();
    // Of the controls, update only the time mark.
    labMark->SetLabel(wxString::Format("%2.3f s", data->gesturalScoreMark_s));
  }
  else
  if (event.GetInt() == UPDATE_PICTURES)
  {
    gesturalScorePicture->Refresh();
    signalComparisonPicture->Refresh();
    timeAxisPicture->Refresh();
    // Of the controls, update only the time mark.
    labMark->SetLabel(wxString::Format("%2.3f s", data->gesturalScoreMark_s));
  }
  else
  if (event.GetInt() == REFRESH_PICTURES_AND_CONTROLS)
  {
    updateWidgets();

    // Refreshing of pictures is not included in updateWidgets() currently !!!
    gesturalScorePicture->Refresh();
    signalComparisonPicture->Refresh();
    timeAxisPicture->Refresh();
  }
  else
  if (event.GetInt() == UPDATE_PICTURES_AND_CONTROLS)
  {
    updateWidgets();

    // Refreshing of pictures is not included in updateWidgets() currently !!!
    gesturalScorePicture->Refresh();
    signalComparisonPicture->Refresh();
    timeAxisPicture->Refresh();
  }

  // Update this window and all of its children in a single pass	
  this->Update();
}


// ****************************************************************************
/// Change the position on the time axis.
// ****************************************************************************

void GesturalScorePage::OnHorzScrollBar(wxScrollEvent &event)
{
  int pos = event.GetPosition();
  double pos_s = (double)pos / 1000.0;

  data->gsTimeAxisGraph->abscissa.reference = pos_s;

  updateWidgets();
  this->Refresh();
}


// ****************************************************************************
// ****************************************************************************

void GesturalScorePage::OnLowerOffsetScrollBar(wxScrollEvent &event)
{
  gesturalScorePicture->setVertOffset( event.GetPosition() );
  gesturalScorePicture->Refresh();
}

// ****************************************************************************
// ****************************************************************************

void GesturalScorePage::OnUpperOffsetScrollBar(wxScrollEvent &event)
{
  signalComparisonPicture->setVertOffset( event.GetPosition() );
  signalComparisonPicture->Refresh();
}

// ****************************************************************************
// ****************************************************************************

void GesturalScorePage::OnSplitterPosChanged(wxSplitterEvent &event)
{
  // To correctly enable/disable a vertical scroll bar.
  updateWidgets();
}


// ****************************************************************************
/// A new string was selected from the combo box for the gesture value.
// ****************************************************************************

void GesturalScorePage::OnNominalValueSelected(wxCommandEvent &event)
{
  Gesture *g = data->getSelectedGesture();
  if (g != NULL)
  {
    g->sVal = lstNominalValue->GetValue();
    updateWidgets();
    gesturalScorePicture->Refresh();
    gesturalScorePicture->Update();
    signalComparisonPicture->Refresh();
    signalComparisonPicture->Update();
    data->updateModelsFromGesturalScore();
    wxYield();
  }
}


// ****************************************************************************
/// A new string was entered into the combo box for the gesture value.
// ****************************************************************************

void GesturalScorePage::OnNominalValueEntered(wxCommandEvent &event)
{
  Gesture *g = data->getSelectedGesture();
  if (g != NULL)
  {
    g->sVal = lstNominalValue->GetValue();
    // Refill the list (forced)!
    fillGestureValueList(true);
    updateWidgets();
    gesturalScorePicture->Refresh();
    gesturalScorePicture->Update();
    signalComparisonPicture->Refresh();
    signalComparisonPicture->Update();
    data->updateModelsFromGesturalScore();
    wxYield();
  }
}

// ****************************************************************************
// ****************************************************************************

void GesturalScorePage::OnNeutralGesture(wxCommandEvent &event)
{
  Gesture *g = data->getSelectedGesture();
  if (g != NULL)
  {
    g->neutral = !g->neutral;
    updateWidgets();
    gesturalScorePicture->Refresh();
    gesturalScorePicture->Update();
    signalComparisonPicture->Refresh();
    signalComparisonPicture->Update();
    data->updateModelsFromGesturalScore();
    wxYield();
  }
}

// ****************************************************************************
/// The value text box lost focus.
// ****************************************************************************

void GesturalScorePage::OnContinualValueChanged(wxFocusEvent &event)
{
  Gesture *g = data->getSelectedGesture();
  if (g != NULL)
  {
    GestureSequence *sequence = &data->gesturalScore->gestures[data->selectedGestureType];
    wxString st = txtContinualValue->GetValue();
    g->dVal = atof((const char*)st.c_str());
    sequence->limitGestureParams(*g);

    updateWidgets();
    gesturalScorePicture->Refresh();
    gesturalScorePicture->Update();
    signalComparisonPicture->Refresh();
    signalComparisonPicture->Update();
    data->updateModelsFromGesturalScore();
  }

  // Important: Also call the base class handler.
  event.Skip();
}

// ****************************************************************************
/// The user pressed ENTER in the value text box.
// ****************************************************************************

void GesturalScorePage::OnContinualValueEntered(wxCommandEvent &event)
{
  Gesture *g = data->getSelectedGesture();
  if (g != NULL)
  {
    GestureSequence *sequence = &data->gesturalScore->gestures[data->selectedGestureType];
    wxString st = txtContinualValue->GetValue();
    g->dVal = atof((const char*)st.c_str());
    sequence->limitGestureParams(*g);

    updateWidgets();
    gesturalScorePicture->Refresh();
    gesturalScorePicture->Update();
    signalComparisonPicture->Refresh();
    signalComparisonPicture->Update();
    data->updateModelsFromGesturalScore();
    wxYield();
  }
}

// ****************************************************************************
/// Change the duration of a gesture.
// ****************************************************************************

void GesturalScorePage::OnDurationChanged(wxSpinEvent &event)
{
  Gesture *g = data->getSelectedGesture();
  if (g != NULL)
  {
    int val_ms = event.GetValue();
    g->duration_s = 0.001 * (double)val_ms;

    updateWidgets();
    gesturalScorePicture->Refresh();
    gesturalScorePicture->Update();
    signalComparisonPicture->Refresh();
    signalComparisonPicture->Update();
    data->updateModelsFromGesturalScore();
    wxYield();
  }
}


// ****************************************************************************
/// The user pressed ENTER in the spin ctrl for the duration.
// ****************************************************************************

void GesturalScorePage::OnDurationEntered(wxCommandEvent &event)
{
  Gesture *g = data->getSelectedGesture();
  if (g != NULL)
  {
    int val_ms = txtDuration->GetValue();
    g->duration_s = 0.001 * (double)val_ms;

    updateWidgets();
    gesturalScorePicture->Refresh();
    gesturalScorePicture->Update();
    signalComparisonPicture->Refresh();
    signalComparisonPicture->Update();
    data->updateModelsFromGesturalScore();
    wxYield();
  }
}

// ****************************************************************************
/// Change the time constant of a gesture.
// ****************************************************************************

void GesturalScorePage::OnTimeConstantChanged(wxScrollEvent &event)
{
  Gesture *g = data->getSelectedGesture();
  if (g != NULL)
  {
    GestureSequence *sequence = &data->gesturalScore->gestures[ data->selectedGestureType ];
    int pos = event.GetPosition();
    g->tau_s = sequence->minTau_s + pos*(sequence->maxTau_s - sequence->minTau_s) / 100.0;

    updateWidgets();
    gesturalScorePicture->Refresh();
    gesturalScorePicture->Update();
    signalComparisonPicture->Refresh();
    signalComparisonPicture->Update();
    data->updateModelsFromGesturalScore();
    wxYield();
  }
}


// ****************************************************************************
/// The slope of a gesture target was changed by the scroll bar.
// ****************************************************************************

void GesturalScorePage::OnSlopeChanged(wxScrollEvent &event)
{
  Gesture *g = data->getSelectedGesture();

  if (g != NULL)
  {
    GestureSequence *sequence = &data->gesturalScore->gestures[ data->selectedGestureType ];
    // Can a slope be adjusted at all?
    if (sequence->minSlope < sequence->maxSlope)
    {
      int pos = event.GetPosition();
      g->slope = sequence->minSlope + 0.001*pos*(sequence->maxSlope - sequence->minSlope);

      updateWidgets();
      gesturalScorePicture->Refresh();
      gesturalScorePicture->Update();
      signalComparisonPicture->Refresh();
      signalComparisonPicture->Update();
      data->updateModelsFromGesturalScore();
      wxYield();
    }
  }
}


// ****************************************************************************
// ****************************************************************************

void GesturalScorePage::OnTimeZoomMinus(wxCommandEvent &event)
{
  data->gsTimeAxisGraph->zoomOutAbscissa(false, true);
  this->Refresh();
}


// ****************************************************************************
// ****************************************************************************

void GesturalScorePage::OnTimeZoomPlus(wxCommandEvent &event)
{
  data->gsTimeAxisGraph->zoomInAbscissa(false, true);
  this->Refresh();
}


// ****************************************************************************
// ****************************************************************************

void GesturalScorePage::OnDisplayChoice(wxCommandEvent &event)
{
  if (event.GetInt() == 0)
  {
    gesturalScorePicture->showGesturalScore = true;
  }
  else
  {
    gesturalScorePicture->showGesturalScore = false;
  }

  updateWidgets();
  this->Refresh();
}


// ****************************************************************************
// ****************************************************************************

void GesturalScorePage::OnShowExtraTrack(wxCommandEvent &event)
{
  signalComparisonPicture->showExtraTrack = !signalComparisonPicture->showExtraTrack;
  updateWidgets();
  this->Refresh();
}

// ****************************************************************************
// ****************************************************************************

void GesturalScorePage::OnShowSonagram(wxCommandEvent &event)
{
  signalComparisonPicture->showSonagrams = !signalComparisonPicture->showSonagrams;
  updateWidgets();
  this->Refresh();
}

// ****************************************************************************
// ****************************************************************************

void GesturalScorePage::OnShowSegmentation(wxCommandEvent &event)
{
  signalComparisonPicture->showSegmentation = !signalComparisonPicture->showSegmentation;
  updateWidgets();
  this->Refresh();
}


// ****************************************************************************
// ****************************************************************************

void GesturalScorePage::OnShowModelF0Curve(wxCommandEvent &event)
{
  signalComparisonPicture->showModelF0Curve = !signalComparisonPicture->showModelF0Curve;
  updateWidgets();
  this->Refresh();
}


// ****************************************************************************
// ****************************************************************************

void GesturalScorePage::OnShowAnimation(wxCommandEvent &event)
{
  data->showAnimation = !data->showAnimation;
  updateWidgets();
}


// ****************************************************************************
// ****************************************************************************

void GesturalScorePage::OnNormalizeAmplitude(wxCommandEvent& event)
{
  data->normalizeAmplitude = !data->normalizeAmplitude;
  updateWidgets();
}


// ****************************************************************************
/// Starts the synthesis from the gestural score.
// ****************************************************************************

void GesturalScorePage::OnSynthesize(wxCommandEvent &event)
{
  // Overwrite the speed to 100% when animation is shown !
  // This may be made adjustable in the StartSynthesisDialog later on ...
  if (data->showAnimation)
  {
    data->synthesisSpeed_percent = 100.0;
  }

  // Clear the audio track

  data->track[Data::MAIN_TRACK]->setZero();

  // ****************************************************************
  // Prepare the synthesis.
  // ****************************************************************

  data->synthesisType = Data::SYNTHESIS_GESMOD;
  TubeSequence *tubeSequence = data->gesturalScore;

  // Keep in mind the user settings for F0 and lung pressure
  data->userPressure_dPa = data->getSelectedGlottis()->controlParam[ Glottis::PRESSURE ].x;
  data->userF0_Hz = data->getSelectedGlottis()->controlParam[ Glottis::FREQUENCY ].x;

  // Set the currently selected glottis model.
  data->gesturalScore->glottis = data->getSelectedGlottis();

  tubeSequence->resetSequence();
  data->tdsModel->resetMotion();
  data->resetTdsBuffers();

  // ****************************************************************
  // Create the synthesis thread (it will be destroyed automatically
  // when it ends).
  // ****************************************************************

  synthesisThread = new SynthesisThread(this, tubeSequence);
  if (synthesisThread->Create() != wxTHREAD_NO_ERROR)
  {
    wxPrintf("ERROR: Can't create synthesis thread!");
    return;
  }

  // Create the progress dialog

  progressDialog = new wxGenericProgressDialog("Synthesis progress",
    "Wait until the synthesis finished or press [Cancel]", 100, this,
    wxPD_CAN_ABORT | wxPD_APP_MODAL | wxPD_ELAPSED_TIME);
    
  // Start the synthesis thread.
  // It will now continually produce events during processing and when it ends.

  synthesisThread->Run();

  // Keep in mind the type of synthesis
  data->prevSynthesisType = data->synthesisType;
}


// ****************************************************************************
// ****************************************************************************

void GesturalScorePage::OnSynthesisThreadEvent(wxCommandEvent& event)
{
  TubeSequence* sequence = data->getSelectedTubeSequence();
  int n = event.GetInt();

  // The thread reached its end - either normally or by the call
  // to wxThread::Destroy()

  if (n == -1)
  {
    progressDialog->Destroy();
    progressDialog = NULL;

    // The dialog is aborted because the event came from another thread, so
    // we may need to wake up the main event loop for the dialog to be
    // really closed
    wxWakeUpIdle();

    // When the current tube sequence was finished regularly,
    // (potentially) normalize the amplitude.

    if ((sequence->getPos_pt() >= sequence->getDuration_pt()) &&
      (data->normalizeAmplitude))
    {
      data->normalizeAudioAmplitude(Data::MAIN_TRACK);
    }

    // Refresh all pictures and controls.
    updateWidgets();

    gesturalScorePicture->Refresh();
    gesturalScorePicture->Update();
    signalComparisonPicture->Refresh();
    signalComparisonPicture->Update();


    // When the current tube sequence was finished regularly,
    // play the sound.

    if (sequence->getPos_pt() >= sequence->getDuration_pt())
    {
      if (waveStartPlaying(data->track[Data::MAIN_TRACK]->x, data->track[Data::MAIN_TRACK]->N, true))
      {
        wxYield();
        SilentMessageBox dialog("Press OK to stop playing!", "Stop playing");
        dialog.ShowModal();
        wxYield();

        waveStopPlaying();
      }
      else
      {
        wxMessageDialog dialog(this, "Playing failed.", "Attention!");
        dialog.ShowModal();
      }
    }

    // Set the lung pressure and F0 that the user adjusted before the sequence started
    data->getSelectedGlottis()->controlParam[ Glottis::PRESSURE ].x = data->userPressure_dPa;
    data->getSelectedGlottis()->controlParam[ Glottis::FREQUENCY ].x = data->userF0_Hz;

    // Update the glottis dialog

    GlottisDialog *glottisDialog = GlottisDialog::getInstance();
    if (glottisDialog->IsShownOnScreen())
    {
      glottisDialog->updateWidgets();
    }

    // Update the vocal tract dialog

    VocalTractDialog *vocalTractDialog = VocalTractDialog::getInstance(this);
    if (vocalTractDialog->IsShownOnScreen())
    {
      vocalTractDialog->Refresh();
      vocalTractDialog->Update();
    }
  }
  else

  // The event asks us to update the progress dialog state
  {
    if (progressDialog->Update(n) == false)
    {
      // If the update of the dialog failed, the user pressed the cancel button.
      // Therefore, tell the synthesis thread to cancel.
      // In respons, the thread will soon send the final event with the id -1.

      synthesisThread->cancelNow();
    }

    // Repaint all pictures IMMEDIATELY.
    if (data->showAnimation)
    {
      data->gesturalScoreMark_s = (double)data->tdsModel->getSampleIndex() / (double)SAMPLING_RATE;

      gesturalScorePicture->Refresh();
      gesturalScorePicture->Update();

      signalComparisonPicture->Refresh();
      signalComparisonPicture->Update();

      // Update the glottis dialog

      GlottisDialog *glottisDialog = GlottisDialog::getInstance();
      if (glottisDialog->IsShownOnScreen())
      {
        glottisDialog->updateWidgets();
      }

      // Update the vocal tract dialog

      VocalTractDialog *vocalTractDialog = VocalTractDialog::getInstance(NULL);   // this
      if (vocalTractDialog->IsShownOnScreen())
      {
        vocalTractDialog->Refresh();
        vocalTractDialog->Update();
      }
    }
    
    synthesisThread->signalGuiUpdateFinished();
  }
}


// ****************************************************************************
// ****************************************************************************

void GesturalScorePage::OnAcoustics(wxCommandEvent &event)
{
  TdsOptionsDialog *dialog = TdsOptionsDialog::getInstance();
  dialog->Show(true);
}

// ****************************************************************************
// ****************************************************************************

void GesturalScorePage::OnShowVocalTract(wxCommandEvent &event)
{
  VocalTractDialog *dialog = VocalTractDialog::getInstance(NULL);   // this
  dialog->Show(true);
}

// ****************************************************************************
// ****************************************************************************

void GesturalScorePage::OnShowGlottis(wxCommandEvent &event)
{
  GlottisDialog *dialog = GlottisDialog::getInstance();
  dialog->SetParent(this);
  dialog->Show(true);
}

// ****************************************************************************
/// Call the dialog for the analysis settings.
// ****************************************************************************

void GesturalScorePage::OnAnalysisSettings(wxCommandEvent &event)
{
  AnalysisSettingsDialog dialog(this, this, signalComparisonPicture->spectrogramPlot);
  dialog.ShowModal();
}


// ****************************************************************************
/// Calculate F0.
// ****************************************************************************

void GesturalScorePage::OnCalcF0(wxCommandEvent &event)
{
  data->estimateF0(this);
  updateWidgets();
  this->Refresh();
}


// ****************************************************************************
/// Calculate voice quality.
// ****************************************************************************

void GesturalScorePage::OnCalcVoiceQuality(wxCommandEvent &event)
{
  data->estimateVoiceQuality(this);
  updateWidgets();
  this->Refresh();
}

// ****************************************************************************
/// Display the non-modal dialog for the analysis results of F0, F1, F2, ...
// ****************************************************************************

void GesturalScorePage::OnAnalysisResults(wxCommandEvent &event)
{
  AnalysisResultsDialog *dialog = AnalysisResultsDialog::getInstance();
  dialog->SetParent(this);
  dialog->Show(true);
}

// ****************************************************************************
/// Display the non-modal dialog for the annotation of segments.
// ****************************************************************************

void GesturalScorePage::OnAnnotationDialog(wxCommandEvent &event)
{
  AnnotationDialog *dialog = AnnotationDialog::getInstance(this);
  dialog->SetParent(this);
  dialog->Show(true);
}

// ****************************************************************************
// ****************************************************************************

void GesturalScorePage::OnResize(wxSizeEvent& event)
{
  event.Skip();
  updateWidgets();
}


// ****************************************************************************
