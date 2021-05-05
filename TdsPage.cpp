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

#include "TdsPage.h"

#include <stdio.h>
#include <wx/config.h>
#include <wx/statline.h>
#include "SilentMessageBox.h"
#include "VocalTractDialog.h"
#include "Backend/SoundLib.h"

using namespace std;


// ****************************************************************************
// Ids.
// ****************************************************************************

static const int IDB_RESET_SYNTHESIS            = 5000;
static const int IDB_START_SYNTHESIS            = 5001;
static const int IDC_SYNTHESIS_SPEED            = 5002;
static const int IDC_SHOW_ANIMATION             = 5003;
static const int IDR_SYNTHESIS_TYPE_BOX         = 5004;
static const int IDC_CONSTANT_F0                = 5005;
static const int IDB_SET_AREA_FUNCTION          = 5006;
static const int IDB_SET_UNIFORM_TUBE           = 5007;
static const int IDB_GLOTTIS_DIALOG             = 5008;
static const int IDB_LF_WAVEFORM                = 5009;
static const int IDB_TDS_ACOUSTICS              = 5010;
static const int IDB_TRIANGULAR_MODEL_F0_PARAMS = 5011;
static const int IDC_SHOW_AREA_FUNCTION_ARTICULATORS = 5012;

static const int IDB_ACOUSTIC_UPPER_LIMIT_PLUS  = 5200;
static const int IDB_ACOUSTIC_UPPER_LIMIT_MINUS = 5201;
static const int IDB_ACOUSTIC_LOWER_LIMIT_PLUS  = 5202;
static const int IDB_ACOUSTIC_LOWER_LIMIT_MINUS = 5203;
static const int IDB_AREA_UPPER_LIMIT_PLUS      = 5204;
static const int IDB_AREA_UPPER_LIMIT_MINUS     = 5205;
static const int IDB_AREA_LOWER_LIMIT_PLUS      = 5206;
static const int IDB_AREA_LOWER_LIMIT_MINUS     = 5207;

static const int IDR_SIGNAL_BOX                 = 5220;
static const int IDC_SHOW_MAIN_PATH             = 5221;
static const int IDC_SHOW_SIDE_PATHS            = 5222;

static const int IDS_TIME                       = 5230;

// ****************************************************************************
// The event table.
// ****************************************************************************

BEGIN_EVENT_TABLE(TdsPage, wxPanel)

  // Custom event handler for update requests by child widgets.
  EVT_COMMAND(wxID_ANY, updateRequestEvent, TdsPage::OnUpdateRequest)

  EVT_MENU(SYNTHESIS_THREAD_EVENT, TdsPage::OnSynthesisThreadEvent)

  EVT_BUTTON(IDB_RESET_SYNTHESIS, TdsPage::OnResetSynthesis)
  EVT_BUTTON(IDB_START_SYNTHESIS, TdsPage::OnStartSynthesis)
  EVT_SPINCTRL(IDC_SYNTHESIS_SPEED, TdsPage::OnSynthesisSpeedChanged)
  EVT_RADIOBOX(IDR_SYNTHESIS_TYPE_BOX, TdsPage::OnSelectSynthesisType)
  EVT_CHECKBOX(IDC_SHOW_ANIMATION, TdsPage::OnShowAnimation)
  EVT_CHECKBOX(IDC_CONSTANT_F0, TdsPage::OnConstantF0)
  EVT_BUTTON(IDB_SET_AREA_FUNCTION, TdsPage::OnSetAreaFunction)
  EVT_BUTTON(IDB_SET_UNIFORM_TUBE, TdsPage::OnSetUniformTube)
  EVT_BUTTON(IDB_GLOTTIS_DIALOG, TdsPage::OnGlottisDialog)
  EVT_BUTTON(IDB_LF_WAVEFORM, TdsPage::OnLfWaveform)
  EVT_BUTTON(IDB_TDS_ACOUSTICS, TdsPage::OnTdsAcoustics)
  EVT_BUTTON(IDB_TRIANGULAR_MODEL_F0_PARAMS, TdsPage::OnTriangularGlottisF0Params)
  EVT_CHECKBOX(IDC_SHOW_AREA_FUNCTION_ARTICULATORS, TdsPage::OnShowAreaFunctionArticulators)

  EVT_RADIOBOX(IDR_SIGNAL_BOX, TdsPage::OnSelectSignal)
  EVT_CHECKBOX(IDC_SHOW_MAIN_PATH, TdsPage::OnShowMainPath)
  EVT_CHECKBOX(IDC_SHOW_SIDE_PATHS, TdsPage::OnShowSidePaths)
  
  EVT_BUTTON(IDB_ACOUSTIC_UPPER_LIMIT_PLUS, TdsPage::OnSignalUpperLimitPlus)
  EVT_BUTTON(IDB_ACOUSTIC_UPPER_LIMIT_MINUS, TdsPage::OnSignalUpperLimitMinus)
  EVT_BUTTON(IDB_ACOUSTIC_LOWER_LIMIT_PLUS, TdsPage::OnSignalLowerLimitPlus)
  EVT_BUTTON(IDB_ACOUSTIC_LOWER_LIMIT_MINUS, TdsPage::OnSignalLowerLimitMinus)
  EVT_BUTTON(IDB_AREA_UPPER_LIMIT_PLUS, TdsPage::OnAreaUpperLimitPlus)
  EVT_BUTTON(IDB_AREA_UPPER_LIMIT_MINUS, TdsPage::OnAreaUpperLimitMinus)
  EVT_BUTTON(IDB_AREA_LOWER_LIMIT_PLUS, TdsPage::OnAreaLowerLimitPlus)
  EVT_BUTTON(IDB_AREA_LOWER_LIMIT_MINUS, TdsPage::OnAreaLowerLimitMinus)

  EVT_COMMAND_SCROLL(IDS_TIME, TdsPage::OnTimeScrollbarChanged)

END_EVENT_TABLE()


// ****************************************************************************
// ****************************************************************************

TdsPage::TdsPage(wxWindow *parent) : wxPanel(parent)
{
  initVars();
  initWidgets();
  updateWidgets();
}


// ****************************************************************************
/// Update all controls (widgets) INCLUDING the pictures.
// ****************************************************************************

void TdsPage::updateWidgets()
{
  int i;

  // ****************************************************************
  // Main frame controls.
  // ****************************************************************

  chkShowMainPath->SetValue(data->showMainPath);
  chkShowSidePaths->SetValue(data->showSidePath);

  if (data->quantity == Data::QUANTITY_PRESSURE)
  {
    i = 0;
  }
  else
  if (data->quantity == Data::QUANTITY_FLOW)
  {
    i = 1;
  }
  else
  if (data->quantity == Data::QUANTITY_AREA)
  {
    i = 2;
  }
  else
  if (data->quantity == Data::QUANTITY_VELOCITY)
  {
    i = 3;
  }
  radSignal->SetSelection(i);

  // ****************************************************************
  // Set the time scroll bar and label.
  // ****************************************************************

  if (data->synthesisType == Data::SYNTHESIS_GESMOD)
  {
    scrTime->Enable();
    labTime->Enable();

    // Pos. and range of the scroll bar are in ms.
    int range = 1000.0*(double)data->gesturalScore->getScoreDuration_s();
    if (range < 1)
    {
      range = 1;
    }
    int pos = (int)((1000.0*data->gesturalScoreMark_s) + 0.5);
    if (pos > range)
    {
      pos = range;
    }
    
    scrTime->SetScrollbar(pos, 1, range, 1);
    scrTime->Update();
    labTime->SetLabel( wxString::Format("%d ms", pos) );
    labTime->Update();
  }
  else
  {
    labTime->SetLabel("");
    scrTime->Disable();
    labTime->Disable();
  }

  // ****************************************************************
  // Side frame controls.
  // ****************************************************************

  TubeSequence *tubeSequence = data->getSelectedTubeSequence();

  chkShowAnimation->SetValue(data->showAnimation);
  chkConstantF0->SetValue(data->staticPhone->useConstantF0);
  ctrlSynthesisSpeed->SetValue(data->synthesisSpeed_percent);

  // Position of the current synthesis
  wxString st = wxString::Format("%1.3f s", (double)tubeSequence->getPos_pt() / (double)SAMPLING_RATE);
  labSynthesisPos->SetLabel(st);

  // The synthesis type
  radSynthesisType->SetSelection( (int)data->synthesisType );

  // The button label
  if ((tubeSequence->getPos_pt() == 0) || (tubeSequence->getPos_pt() >= tubeSequence->getDuration_pt()))
  {
    btnStartSynthesis->SetLabel("Start synthesis");
  }
  else
  {
    btnStartSynthesis->SetLabel("Continue synthesis");
  }

  chkShowAreaFunctionArticulators->SetValue(tdsTubePicture->showArticulators);

  // ****************************************************************
  // Refresh all pictures.
  // ****************************************************************

  tdsTimeSignalPicture->Refresh();
  tdsSpatialSignalPicture->Refresh();
  tdsTubePicture->Refresh();

}

// ****************************************************************************
// ****************************************************************************

void TdsPage::initVars()
{
  data = Data::getInstance();
}


// ****************************************************************************
// ****************************************************************************

void TdsPage::initWidgets()
{
  wxButton *button = NULL;
  wxStaticText *label = NULL;
  wxBoxSizer *sizer = NULL;     // Help sizer

  // ****************************************************************
  // Top level sizer.
  // ****************************************************************

  wxBoxSizer *topLevelSizer = new wxBoxSizer(wxHORIZONTAL);

  // ****************************************************************
  // Left side sizer.
  // ****************************************************************

  wxBoxSizer *leftSizer = new wxBoxSizer(wxVERTICAL);

  leftSizer->AddSpacer(5);

  chkShowAnimation = new wxCheckBox(this, IDC_SHOW_ANIMATION, "Show animation");
  leftSizer->Add(chkShowAnimation, 0, wxGROW | wxALL, 3);

  leftSizer->AddSpacer(5);

  // ********************************************

  sizer = new wxBoxSizer(wxHORIZONTAL);
  leftSizer->Add(sizer, 0, wxALL, 3);
  
  label = new wxStaticText(this, wxID_ANY, "Speed in %");
  sizer->Add(label, 0, wxALL | wxALIGN_CENTER, 3);

  ctrlSynthesisSpeed = new wxSpinCtrl(this, IDC_SYNTHESIS_SPEED, "", wxDefaultPosition,
    wxSize(50, 20), wxSP_ARROW_KEYS, 1, 100, 1);
  sizer->Add(ctrlSynthesisSpeed, 0, wxALL, 3);

  // ********************************************

  sizer = new wxBoxSizer(wxHORIZONTAL);
  leftSizer->Add(sizer, 0, wxALL, 3);
  
  label = new wxStaticText(this, wxID_ANY, "Position: ");
  sizer->Add(label, 0, wxALL | wxALIGN_CENTER, 3);

  labSynthesisPos = new wxStaticText(this, wxID_ANY, "0");
  sizer->Add(labSynthesisPos, 0, wxALL | wxALIGN_CENTER, 3);

  // ********************************************

  leftSizer->AddSpacer(5);

  button = new wxButton(this, IDB_RESET_SYNTHESIS, "Reset");
  leftSizer->Add(button, 0, wxGROW | wxALL, 3);

  btnStartSynthesis = new wxButton(this, IDB_START_SYNTHESIS, "Start synthesis");
  leftSizer->Add(btnStartSynthesis, 0, wxGROW | wxALL, 3);

  // Radio buttons with choices for synthesis type 

  const int NUM_SYNTHESIS_TYPES = Data::NUM_SYNTHESIS_TYPES;
  const wxString SYNTHESIS_TYPE_CHOICES[NUM_SYNTHESIS_TYPES] = 
  { 
    "Subglottal impedance", 
    "Supraglottal impedance",
    "Transfer function",
    "Vowel (LF flow pulse)",
    "Phone (full simulation)",
    "Gestural model"
  };
  
  radSynthesisType = new wxRadioBox(this, IDR_SYNTHESIS_TYPE_BOX, "Synthesis type", 
    wxDefaultPosition, wxDefaultSize,
    NUM_SYNTHESIS_TYPES, SYNTHESIS_TYPE_CHOICES, NUM_SYNTHESIS_TYPES, wxRA_SPECIFY_ROWS);
  leftSizer->Add(radSynthesisType, 0, wxALL | wxGROW, 3);

  // ********************************************

  chkConstantF0 = new wxCheckBox(this, IDC_CONSTANT_F0, "Constant F0 for phone sim.");
  leftSizer->Add(chkConstantF0, 0, wxGROW | wxALL, 3);

  leftSizer->AddSpacer(5);


  // ********************************************

  button = new wxButton(this, IDB_SET_AREA_FUNCTION, "Set area function");
  leftSizer->Add(button, 0, wxGROW | wxALL, 3);

  button = new wxButton(this, IDB_SET_UNIFORM_TUBE, "Set uniform tube");
  leftSizer->Add(button, 0, wxGROW | wxALL, 3);

  button = new wxButton(this, IDB_GLOTTIS_DIALOG, "Glottis dialog");
  leftSizer->Add(button, 0, wxGROW | wxALL, 3);

  button = new wxButton(this, IDB_LF_WAVEFORM, "LF glottal flow pulse");
  leftSizer->Add(button, 0, wxGROW | wxALL, 3);

  button = new wxButton(this, IDB_TDS_ACOUSTICS, "Acoustics (time domain)");
  leftSizer->Add(button, 0, wxGROW | wxALL, 3);

  button = new wxButton(this, IDB_TRIANGULAR_MODEL_F0_PARAMS, "Calc. F0 params of glottis");
  leftSizer->Add(button, 0, wxGROW | wxALL, 3);

  chkShowAreaFunctionArticulators = new wxCheckBox(this, IDC_SHOW_AREA_FUNCTION_ARTICULATORS, "Articulator colors");
  leftSizer->Add(chkShowAreaFunctionArticulators, 0, wxALL, 2);

  // Add the sizer to the top-level sizer!
  topLevelSizer->Add(leftSizer, 0, wxALL, 5);

  // ****************************************************************
  // Static line to separate the left and right part.
  // ****************************************************************

  wxStaticLine *verticalLine = new wxStaticLine(this, wxID_ANY, wxDefaultPosition, 
    wxDefaultSize, wxLI_VERTICAL);

  topLevelSizer->Add(verticalLine, 0, wxGROW | wxALL, 2);

  // ****************************************************************
  // ****************************************************************

  wxBoxSizer *rightSizer = new wxBoxSizer(wxVERTICAL);
  
  wxBoxSizer *topSizer = new wxBoxSizer(wxHORIZONTAL);
  wxBoxSizer *middleSizer = new wxBoxSizer(wxHORIZONTAL);
  wxBoxSizer *bottomSizer = new wxBoxSizer(wxHORIZONTAL);

  rightSizer->Add(topSizer, 1, wxGROW | wxALL, 2);
  rightSizer->Add(middleSizer, 1, wxGROW | wxALL, 2);
  rightSizer->Add(bottomSizer, 1, wxGROW | wxALL, 2);

  topLevelSizer->Add(rightSizer, 1, wxGROW);

  // ****************************************************************
  // Top sizer.
  // ****************************************************************

  // Time signal picture
  tdsTimeSignalPicture = new TdsTimeSignalPicture(this);

  topSizer->Add(tdsTimeSignalPicture, 1, wxGROW);

  // ********************************************

  sizer = new wxBoxSizer(wxVERTICAL);

  const int NUM_QUANTITY_CHOICES = Data::NUM_QUANTITIES;
  const wxString QUANTITY_CHOICES[NUM_QUANTITY_CHOICES] = 
  { 
    "Pressure", 
    "Flow",
    "Area",
    "Velocity"
  };
  
  radSignal = new wxRadioBox(this, IDR_SIGNAL_BOX, "Signal", wxDefaultPosition, wxDefaultSize,
    NUM_QUANTITY_CHOICES, QUANTITY_CHOICES, NUM_QUANTITY_CHOICES, wxRA_SPECIFY_ROWS);
  sizer->Add(radSignal, 0, wxALL | wxGROW, 3);

  chkShowMainPath = new wxCheckBox(this, IDC_SHOW_MAIN_PATH, "Main path");
  sizer->Add(chkShowMainPath, 0, wxALL, 3);

  chkShowSidePaths = new wxCheckBox(this, IDC_SHOW_SIDE_PATHS, "Side paths");
  sizer->Add(chkShowSidePaths, 0, wxALL, 3);

  // ********************************************

  topSizer->Add(sizer, 0, wxGROW);

  // ****************************************************************
  // Middle sizer.
  // ****************************************************************

  sizer = new wxBoxSizer(wxVERTICAL);

  button = new wxButton(this, IDB_ACOUSTIC_UPPER_LIMIT_PLUS, "+", wxDefaultPosition, wxSize(25, 25));
  sizer->Add(button, 0, wxALL, 3);

  button = new wxButton(this, IDB_ACOUSTIC_UPPER_LIMIT_MINUS, "-", wxDefaultPosition, wxSize(25, 25));
  sizer->Add(button, 0, wxALL, 3);

  sizer->AddStretchSpacer();

  button = new wxButton(this, IDB_ACOUSTIC_LOWER_LIMIT_PLUS, "+", wxDefaultPosition, wxSize(25, 25));
  sizer->Add(button, 0, wxALL, 3);

  button = new wxButton(this, IDB_ACOUSTIC_LOWER_LIMIT_MINUS, "-", wxDefaultPosition, wxSize(25, 25));
  sizer->Add(button, 0, wxALL, 3);

  middleSizer->Add(sizer, 0, wxGROW);

  // Spatial signal picture

  tdsSpatialSignalPicture = new TdsSpatialSignalPicture(this);
  middleSizer->Add(tdsSpatialSignalPicture, 1, wxGROW);


  // ****************************************************************
  // Bottom sizer.
  // ****************************************************************

  sizer = new wxBoxSizer(wxVERTICAL);

  button = new wxButton(this, IDB_AREA_UPPER_LIMIT_PLUS, "+", wxDefaultPosition, wxSize(25, 25));
  sizer->Add(button, 0, wxALL, 3);

  button = new wxButton(this, IDB_AREA_UPPER_LIMIT_MINUS, "-", wxDefaultPosition, wxSize(25, 25));
  sizer->Add(button, 0, wxALL, 3);

  sizer->AddStretchSpacer();

  button = new wxButton(this, IDB_AREA_LOWER_LIMIT_PLUS, "+", wxDefaultPosition, wxSize(25, 25));
  sizer->Add(button, 0, wxALL, 3);

  button = new wxButton(this, IDB_AREA_LOWER_LIMIT_MINUS, "-", wxDefaultPosition, wxSize(25, 25));
  sizer->Add(button, 0, wxALL, 3);

  bottomSizer->Add(sizer, 0, wxGROW);

  // Area function picture
  
  tdsTubePicture = new TdsTubePicture(this, this);
  bottomSizer->Add(tdsTubePicture, 1, wxGROW);

  // Scroll bar for the time.

  sizer = new wxBoxSizer(wxHORIZONTAL);

  label = new wxStaticText(this, wxID_ANY, "Time:");
  sizer->Add(label, 0, wxALL | wxGROW, 2);
  
  scrTime = new wxScrollBar(this, IDS_TIME, wxDefaultPosition, wxDefaultSize, wxSB_HORIZONTAL);
  scrTime->SetMinSize( wxSize(60, 20) );
  scrTime->SetScrollbar(0, 1, 100, 1);
  sizer->Add(scrTime, 1, wxGROW);

  labTime = new wxStaticText(this, wxID_ANY, "0 ms", wxDefaultPosition, wxSize(80, 16));
  sizer->Add(labTime, 0, wxALL | wxGROW, 2);

  rightSizer->Add(sizer, 0, wxGROW | wxALL, 3);

  // ****************************************************************
  // Set the top-level sizer for this window.
  // ****************************************************************

  this->SetSizer(topLevelSizer);

}


// ****************************************************************************
/// Is called, when one of the child windows has requested the update of
/// other child windows.
// ****************************************************************************

void TdsPage::OnUpdateRequest(wxCommandEvent &event)
{
  if (event.GetInt() == REFRESH_PICTURES)
  {
    tdsTimeSignalPicture->Refresh();
    tdsSpatialSignalPicture->Refresh();
    tdsTubePicture->Refresh();
  }
  else
  if (event.GetInt() == REFRESH_PICTURES_AND_CONTROLS)
  {
    updateWidgets();
  }
}


// ****************************************************************************
// ****************************************************************************

void TdsPage::OnSynthesisThreadEvent(wxCommandEvent& event)
{
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

    // Refresh all pictures and controls.
    updateWidgets();

    // When the current tube sequence was finished regularly,
    // play the sound.

    TubeSequence *sequence = data->getSelectedTubeSequence();
    if (sequence->getPos_pt() >= sequence->getDuration_pt())
    {
      if (waveStartPlaying(data->track[Data::MAIN_TRACK]->x, data->track[Data::MAIN_TRACK]->N, true))
      {
        wxYield();
        SilentMessageBox dialog("Press OK to stop playing!", "Stop playing", this);
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
      TubeSequence *sequence = data->getSelectedTubeSequence();
      data->gesturalScoreMark_s = (double)sequence->getPos_pt() / (double)SAMPLING_RATE;

      updateWidgets();
      tdsTimeSignalPicture->Update();
      tdsSpatialSignalPicture->Update();
      tdsTubePicture->Update();

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
    
    synthesisThread->signalGuiUpdateFinished();
  }
}


// ****************************************************************************
// ****************************************************************************

void TdsPage::OnResetSynthesis(wxCommandEvent &event)
{
  TubeSequence *tubeSequence = data->getSelectedTubeSequence();
  
  tubeSequence->resetSequence();
  data->tdsModel->resetMotion();
  data->resetTdsBuffers();

  updateWidgets();
}


// ****************************************************************************
// ****************************************************************************

void TdsPage::OnStartSynthesis(wxCommandEvent &event)
{
  Tube tube;

  // ****************************************************************
  // When the synthesis type changed or a synthesis is restarted, then
  // o Reset signal buffers.
  // o Reset the state of the TDS model.
  // o Setup and reset the selected tube sequence.
  // ****************************************************************

  TubeSequence *tubeSequence = data->getSelectedTubeSequence();

  if ((data->prevSynthesisType != data->synthesisType) ||
      (tubeSequence->getPos_pt() == 0) ||
      (tubeSequence->getPos_pt() >= tubeSequence->getDuration_pt()))
  {
    // Keep in mind the user settings for F0 and lung pressure

    data->userPressure_dPa = data->getSelectedGlottis()->controlParam[ Glottis::PRESSURE ].x;
    data->userF0_Hz = data->getSelectedGlottis()->controlParam[ Glottis::FREQUENCY ].x;

    data->tdsModel->getTube(&tube);

    // Configure the selected tube sequence

    switch (data->synthesisType)
    {
    case Data::SYNTHESIS_SUBGLOTTAL_INPUT_IMPEDANCE:
      // Negative impulse of 1000 cm^3/s in the trachea section right below the glottis
      data->subglottalInputImpedance->setup(tube, Tube::LAST_TRACHEA_SECTION, -1000.0);
      data->internalProbeSection = Tube::LAST_TRACHEA_SECTION;
      break;

    case Data::SYNTHESIS_SUPRAGLOTTAL_INPUT_IMPEDANCE:
      // Positive impulse of 1000 cm^3/s in the trachea section right above the glottis
      data->supraglottalInputImpedance->setup(tube, Tube::FIRST_PHARYNX_SECTION, 1000.0);
      data->internalProbeSection = Tube::FIRST_PHARYNX_SECTION;
      break;

    case Data::SYNTHESIS_TRANSFER_FUNCTION:
      // Positive impulse of 1000 cm^3/s in the trachea section right above the glottis
      data->transferFunction->setup(tube, Tube::FIRST_PHARYNX_SECTION, 1000.0);
      data->internalProbeSection = -1;
      break;

    case Data::SYNTHESIS_LF_VOWEL:
      data->vowelLf->setup(tube, data->lfPulse, 0.6*SAMPLING_RATE);
      data->internalProbeSection = -1;
      break;

    case Data::SYNTHESIS_PHONE:
      data->staticPhone->setup(tube, data->getSelectedGlottis(), 0.6*SAMPLING_RATE);
      data->internalProbeSection = -1;
      break;

    case Data::SYNTHESIS_GESMOD:
      // Set the currently selected glottis model.
      data->gesturalScore->glottis = data->getSelectedGlottis();
      break;

    default: break;
    }

    tubeSequence->resetSequence();
    data->tdsModel->resetMotion();
    data->resetTdsBuffers();
  }

  // ****************************************************************
  // Create the synthesis thread (it will be destroyed automatically
  // when it ends).
  // ****************************************************************

  synthesisThread = new SynthesisThread(this, tubeSequence);
  if (synthesisThread->Create() != wxTHREAD_NO_ERROR)
  {
    printf("ERROR: Can't create synthesis thread!");
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

void TdsPage::OnSynthesisSpeedChanged(wxSpinEvent &event)
{
  data->synthesisSpeed_percent = ctrlSynthesisSpeed->GetValue();
}

// ****************************************************************************
// ****************************************************************************

void TdsPage::OnShowAnimation(wxCommandEvent &event)
{
  data->showAnimation = event.IsChecked();
}

// ****************************************************************************
// ****************************************************************************

void TdsPage::OnSelectSynthesisType(wxCommandEvent &event)
{
  int selection = event.GetSelection();
  data->synthesisType = (Data::SynthesisType)selection;
  updateWidgets();
}

// ****************************************************************************
// ****************************************************************************

void TdsPage::OnConstantF0(wxCommandEvent &event)
{
  data->staticPhone->useConstantF0 = event.IsChecked();
  updateWidgets();
}

// ****************************************************************************
// ****************************************************************************

void TdsPage::OnSetAreaFunction(wxCommandEvent &event)
{
  static Tube tube;

  // Get the tube geometry from the vocal tract model.
  data->vocalTract->getTube(&tube);
  data->tdsModel->setTube(&tube);
  
  // Reset the TDS model for the synthesis.
  data->tdsModel->resetMotion();

  // Reset the tube sequence (position = 0).
  TubeSequence *tubeSequence = data->getSelectedTubeSequence();
  tubeSequence->resetSequence();

  // Reset the buffers for the displayed signals.
  data->resetTdsBuffers();

  updateWidgets();
}


// ****************************************************************************
// ****************************************************************************

void TdsPage::OnSetUniformTube(wxCommandEvent &event)
{
  Tube tube;
  data->tdsModel->setTube(&tube);

  // Reset the TDS model for the synthesis.
  data->tdsModel->resetMotion();

  // Reset the tube sequence (position = 0).
  TubeSequence *tubeSequence = data->getSelectedTubeSequence();
  tubeSequence->resetSequence();

  // Reset the buffers for the displayed signals.
  data->resetTdsBuffers();

  updateWidgets();
}


// ****************************************************************************
// ****************************************************************************

void TdsPage::OnGlottisDialog(wxCommandEvent &event)
{
  GlottisDialog *dialog = GlottisDialog::getInstance();
  dialog->SetParent(this);
  dialog->Show(true);
}

// ****************************************************************************
// ****************************************************************************

void TdsPage::OnLfWaveform(wxCommandEvent &event)
{
  LfPulseDialog *dialog = LfPulseDialog::getInstance();
  dialog->Show(true);
}

// ****************************************************************************
// ****************************************************************************

void TdsPage::OnTdsAcoustics(wxCommandEvent &event)
{
  TdsOptionsDialog *dialog = TdsOptionsDialog::getInstance();
  dialog->SetParent(this);
  dialog->Show(true);
}

// ****************************************************************************
/// Determines the natural frequency and the F0 derivatives of the new
/// four-mass model.
// ****************************************************************************

void TdsPage::OnTriangularGlottisF0Params(wxCommandEvent &event)
{
  // Inform and ask the user.
  wxString st = wxString(
    "This function determines the parameters 'Natural F0' and 'dF0/dQ' "
    "for the triangular glottis model, so that the tension factor Q "
    "can be calculated from the control parameter F0. "
    "To make this function work well, the control parameters of the glottis model "
    "should be set for a modal voice and the tube geometry should be set to the "
    "vowel /E/ (SAMPA). The following calculations may take one minute or less.\n"
    "Do you want to continue?");

  if (wxMessageBox(st, "Continue?", wxYES_NO, this) == wxYES)
  {
    // Calculate the actual parameters.
    data->calcTriangularGlottisF0Params();

    // Update the widgets in the glottis dialog.
    GlottisDialog *dialog = GlottisDialog::getInstance();
    dialog->SetParent(this);
    dialog->updateWidgets();
  }
}


// ****************************************************************************
// ****************************************************************************

void TdsPage::OnShowAreaFunctionArticulators(wxCommandEvent &event)
{
  tdsTubePicture->showArticulators = !tdsTubePicture->showArticulators;
  updateWidgets();
}


// ****************************************************************************
// ****************************************************************************

void TdsPage::OnSelectSignal(wxCommandEvent &event)
{
  int index = event.GetInt();

  if (index == 0)
  {
    data->quantity = Data::QUANTITY_PRESSURE;
  }
  else
  if (index == 1)
  {
    data->quantity = Data::QUANTITY_FLOW;
  }
  else
  if (index == 2)
  {
    data->quantity = Data::QUANTITY_AREA;
  }
  else
  {
    data->quantity = Data::QUANTITY_VELOCITY;
  }

  updateWidgets();
}

// ****************************************************************************
// ****************************************************************************

void TdsPage::OnShowMainPath(wxCommandEvent &event)
{
  data->showMainPath = !data->showMainPath;
  updateWidgets();
}

// ****************************************************************************
// ****************************************************************************

void TdsPage::OnShowSidePaths(wxCommandEvent &event)
{
  data->showSidePath = !data->showSidePath;
  updateWidgets();
}

// ****************************************************************************
// ****************************************************************************

void TdsPage::OnSignalUpperLimitPlus(wxCommandEvent &event)
{
  if (data->quantity == Data::QUANTITY_PRESSURE)    
  { 
    tdsTimeSignalPicture->pressureGraph.zoomOutOrdinate(false, true); 
    tdsSpatialSignalPicture->pressureGraph.zoomOutOrdinate(false, true); 
  } 
  else
  if (data->quantity == Data::QUANTITY_FLOW)
  { 
    tdsTimeSignalPicture->flowGraph.zoomOutOrdinate(false, true); 
    tdsSpatialSignalPicture->flowGraph.zoomOutOrdinate(false, true); 
  }
  else
  if (data->quantity == Data::QUANTITY_AREA)
  { 
    tdsTimeSignalPicture->areaGraph.zoomOutOrdinate(false, true); 
    tdsSpatialSignalPicture->areaGraph.zoomOutOrdinate(false, true); 
  }
  else
  if (data->quantity == Data::QUANTITY_VELOCITY)
  {
    tdsTimeSignalPicture->velocityGraph.zoomOutOrdinate(false, true);
    tdsSpatialSignalPicture->velocityGraph.zoomOutOrdinate(false, true);
  }

  updateWidgets();
}

// ****************************************************************************
// ****************************************************************************

void TdsPage::OnSignalUpperLimitMinus(wxCommandEvent &event)
{
  if (data->quantity == Data::QUANTITY_PRESSURE)    
  { 
    tdsTimeSignalPicture->pressureGraph.zoomInOrdinate(false, true); 
    tdsSpatialSignalPicture->pressureGraph.zoomInOrdinate(false, true); 
  } 
  else
  if (data->quantity == Data::QUANTITY_FLOW)
  { 
    tdsTimeSignalPicture->flowGraph.zoomInOrdinate(false, true); 
    tdsSpatialSignalPicture->flowGraph.zoomInOrdinate(false, true); 
  }
  else
  if (data->quantity == Data::QUANTITY_AREA)
  { 
    tdsTimeSignalPicture->areaGraph.zoomInOrdinate(false, true); 
    tdsSpatialSignalPicture->areaGraph.zoomInOrdinate(false, true); 
  }
  else
  if (data->quantity == Data::QUANTITY_VELOCITY)
  {
    tdsTimeSignalPicture->velocityGraph.zoomInOrdinate(false, true);
    tdsSpatialSignalPicture->velocityGraph.zoomInOrdinate(false, true);
  }

  updateWidgets();
}

// ****************************************************************************
// ****************************************************************************

void TdsPage::OnSignalLowerLimitPlus(wxCommandEvent &event)
{
  if (data->quantity == Data::QUANTITY_PRESSURE)    
  { 
    tdsTimeSignalPicture->pressureGraph.zoomInOrdinate(true, false); 
    tdsSpatialSignalPicture->pressureGraph.zoomInOrdinate(true, false); 
  } 
  else
  if (data->quantity == Data::QUANTITY_FLOW)
  { 
    tdsTimeSignalPicture->flowGraph.zoomInOrdinate(true, false); 
    tdsSpatialSignalPicture->flowGraph.zoomInOrdinate(true, false); 
  }
  else
  if (data->quantity == Data::QUANTITY_AREA)
  { 
    tdsTimeSignalPicture->areaGraph.zoomInOrdinate(true, false); 
    tdsSpatialSignalPicture->areaGraph.zoomInOrdinate(true, false); 
  }
  else
  if (data->quantity == Data::QUANTITY_VELOCITY)
  {
    tdsTimeSignalPicture->velocityGraph.zoomInOrdinate(true, false);
    tdsSpatialSignalPicture->velocityGraph.zoomInOrdinate(true, false);
  }

  updateWidgets();
}

// ****************************************************************************
// ****************************************************************************

void TdsPage::OnSignalLowerLimitMinus(wxCommandEvent &event)
{
  if (data->quantity == Data::QUANTITY_PRESSURE)    
  { 
    tdsTimeSignalPicture->pressureGraph.zoomOutOrdinate(true, false); 
    tdsSpatialSignalPicture->pressureGraph.zoomOutOrdinate(true, false); 
  } 
  else
  if (data->quantity == Data::QUANTITY_FLOW)
  { 
    tdsTimeSignalPicture->flowGraph.zoomOutOrdinate(true, false); 
    tdsSpatialSignalPicture->flowGraph.zoomOutOrdinate(true, false); 
  }
  else
  if (data->quantity == Data::QUANTITY_AREA)
  { 
    tdsTimeSignalPicture->areaGraph.zoomOutOrdinate(true, false); 
    tdsSpatialSignalPicture->areaGraph.zoomOutOrdinate(true, false); 
  }
  else
  if (data->quantity == Data::QUANTITY_VELOCITY)
  {
    tdsTimeSignalPicture->velocityGraph.zoomOutOrdinate(true, false);
    tdsSpatialSignalPicture->velocityGraph.zoomOutOrdinate(true, false);
  }

  updateWidgets();
}

// ****************************************************************************
// ****************************************************************************

void TdsPage::OnAreaUpperLimitPlus(wxCommandEvent &event)
{
  tdsTubePicture->graph.zoomOutOrdinate(false, true);
  updateWidgets();
}

// ****************************************************************************
// ****************************************************************************

void TdsPage::OnAreaUpperLimitMinus(wxCommandEvent &event)
{
  tdsTubePicture->graph.zoomInOrdinate(false, true);
  updateWidgets();
}

// ****************************************************************************
// ****************************************************************************

void TdsPage::OnAreaLowerLimitPlus(wxCommandEvent &event)
{
  tdsTubePicture->graph.zoomInOrdinate(true, false);
  updateWidgets();
}

// ****************************************************************************
// ****************************************************************************

void TdsPage::OnAreaLowerLimitMinus(wxCommandEvent &event)
{
  tdsTubePicture->graph.zoomOutOrdinate(true, false);
  updateWidgets();
}

// ****************************************************************************
// ****************************************************************************

void TdsPage::OnTimeScrollbarChanged(wxScrollEvent &event)
{
  int pos = scrTime->GetThumbPosition();
  data->gesturalScoreMark_s = (double)pos / 1000.0;

  data->updateModelsFromGesturalScore();

  Tube tube;
  data->vocalTract->getTube(&tube);
  data->tdsModel->setTube(&tube);

  updateWidgets();
}

// ****************************************************************************
