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

#include "VocalTractPage.h"

#include <stdio.h>
#include <math.h>
#include <wx/config.h>
#include <wx/statline.h>
#include <wx/splitter.h>
#include <wx/gbsizer.h>
#include <wx/clipbrd.h>
#include <wx/filename.h>
//#include <wx/tipwin.h>

#include "Data.h"
#include "SilentMessageBox.h"
#include "VocalTractDialog.h"
#include "TransitionDialog.h"
#include "Backend/SoundLib.h"
#include "Backend/Synthesizer.h"
#include "Backend/Dsp.h"
#include "Backend/Synthesizer.h"

using namespace std;

// ****************************************************************************
// Ids.
// ****************************************************************************

// Spectrum panel controls

static const int IDB_UPPER_SPECTRUM_LIMIT_PLUS		= 4000;
static const int IDB_UPPER_SPECTRUM_LIMIT_MINUS		= 4001;
static const int IDB_LOWER_SPECTRUM_LIMIT_PLUS		= 4002;
static const int IDB_LOWER_SPECTRUM_LIMIT_MINUS		= 4003;
static const int IDB_FREQUENCY_RANGE_MINUS			  = 4006;
static const int IDB_FREQUENCY_RANGE_PLUS 		  	= 4007;
static const int IDB_NOISE_FILTER_CUTOFF_MINUS    = 4008;
static const int IDB_NOISE_FILTER_CUTOFF_PLUS     = 4009;
static const int IDB_SYNTHESIZE_NOISE             = 4010;

static const int IDS_SPECTRUM_OFFSET		      		= 4020;
static const int IDC_SHOW_MODEL_SPECTRUM          = 4021;
static const int IDC_SHOW_USER_SPECTRUM           = 4022;
static const int IDC_SHOW_TDS_SPECTRUM            = 4023;
static const int IDC_SHOW_POLE_ZERO_SPECTRUM      = 4024;
static const int IDC_SHOW_FORMANTS                = 4025;
static const int IDC_SHOW_MAGNITUDE               = 4026;
static const int IDC_SHOW_PHASE                   = 4027;

// Left side controls

static const int IDB_VOCAL_TRACT_DIALOG           = 4109;
static const int IDB_SHAPES_DIALOG                = 4110;
static const int IDB_PHONETIC_PARAMS_DIALOG       = 4111;
static const int IDB_LF_PULSE                     = 4112;
static const int IDB_FDS_ACOUSTICS                = 4113;
static const int IDB_IMPROVE_FORMANTS             = 4114;
static const int IDB_CHECK_VOWEL_SHAPE            = 4115;
static const int IDB_GET_FORMANT_ERROR            = 4116;
static const int IDB_EDIT_POLE_ZERO_PLAN          = 4117;
static const int IDB_CALC_POLE_ZERO_PLAN          = 4118;
static const int IDB_TESTING                      = 4119;
static const int IDB_MAP_SHAPE                    = 4120;
static const int IDB_TRANSITION_DIALOG            = 4121;

static const int IDB_PLAY_SHORT_VOWEL_TDS         = 4130;
static const int IDB_PLAY_LONG_VOWEL_TDS          = 4131;
static const int IDB_PLAY_FRICATIVE_TDS           = 4132;
static const int IDB_PLAY_SHORT_VOWEL_FDS         = 4133;
static const int IDB_PLAY_LONG_VOWEL_FDS          = 4134;

// Main panel controls

static const int IDC_SHOW_DISCRETE_AREAS          = 4155;
static const int IDC_SHOW_AREAS                   = 4156;
static const int IDC_SHOW_BRANCHES                = 4157;
static const int IDC_SHOW_TEXT                    = 4158;
static const int IDC_SHOW_AREA_FUNCTION_ARTICULATORS = 4159;


// ****************************************************************************
// The event table.
// ****************************************************************************

BEGIN_EVENT_TABLE(VocalTractPage, wxPanel)

  // Custom event handler for update requests by child widgets.
  EVT_COMMAND(wxID_ANY, updateRequestEvent, VocalTractPage::OnUpdateRequest)

  // Spectrum panel controls

  EVT_BUTTON(IDB_UPPER_SPECTRUM_LIMIT_PLUS, VocalTractPage::OnUpperSpectrumLimitPlus)
  EVT_BUTTON(IDB_UPPER_SPECTRUM_LIMIT_MINUS, VocalTractPage::OnUpperSpectrumLimitMinus)
  EVT_BUTTON(IDB_LOWER_SPECTRUM_LIMIT_PLUS, VocalTractPage::OnLowerSpectrumLimitPlus)
  EVT_BUTTON(IDB_LOWER_SPECTRUM_LIMIT_MINUS, VocalTractPage::OnLowerSpectrumLimitMinus)
  
  EVT_BUTTON(IDB_FREQUENCY_RANGE_MINUS, VocalTractPage::OnFrequencyRangeMinus)
  EVT_BUTTON(IDB_FREQUENCY_RANGE_PLUS, VocalTractPage::OnFrequencyRangePlus)

  EVT_BUTTON(IDB_NOISE_FILTER_CUTOFF_MINUS, VocalTractPage::OnNoiseFilterCutoffMinus)
  EVT_BUTTON(IDB_NOISE_FILTER_CUTOFF_PLUS, VocalTractPage::OnNoiseFilterCutoffPlus)
  EVT_BUTTON(IDB_SYNTHESIZE_NOISE, VocalTractPage::OnSynthesizeNoise)

  EVT_COMMAND_SCROLL(IDS_SPECTRUM_OFFSET, VocalTractPage::OnSpectrumOffsetChanged)
  EVT_CHECKBOX(IDC_SHOW_MODEL_SPECTRUM, VocalTractPage::OnShowModelSpectrum)
  EVT_CHECKBOX(IDC_SHOW_USER_SPECTRUM, VocalTractPage::OnShowUserSpectrum)
  EVT_CHECKBOX(IDC_SHOW_TDS_SPECTRUM, VocalTractPage::OnShowTdsSpectrum)
  EVT_CHECKBOX(IDC_SHOW_POLE_ZERO_SPECTRUM, VocalTractPage::OnShowPoleZeroSpectrum)
  EVT_CHECKBOX(IDC_SHOW_FORMANTS, VocalTractPage::OnShowFormants)
  EVT_CHECKBOX(IDC_SHOW_MAGNITUDE, VocalTractPage::OnShowMagnitude)
  EVT_CHECKBOX(IDC_SHOW_PHASE, VocalTractPage::OnShowPhase)

  // Left side controls

  EVT_BUTTON(IDB_VOCAL_TRACT_DIALOG, VocalTractPage::OnVocalTractDialog)
  EVT_BUTTON(IDB_SHAPES_DIALOG, VocalTractPage::OnShapesDialog)
  EVT_BUTTON(IDB_PHONETIC_PARAMS_DIALOG, VocalTractPage::OnPhoneticParamsDialog)
  EVT_BUTTON(IDB_LF_PULSE, VocalTractPage::OnLfPulse)
  EVT_BUTTON(IDB_FDS_ACOUSTICS, VocalTractPage::OnFdsAcoustics)
  EVT_BUTTON(IDB_IMPROVE_FORMANTS, VocalTractPage::OnImproveFormants)
  EVT_BUTTON(IDB_CHECK_VOWEL_SHAPE, VocalTractPage::OnCheckVowelShape)
  EVT_BUTTON(IDB_GET_FORMANT_ERROR, VocalTractPage::OnGetFormantError)

  EVT_BUTTON(IDB_EDIT_POLE_ZERO_PLAN, VocalTractPage::OnEditPoleZeroPlan)
  EVT_BUTTON(IDB_MAP_SHAPE, VocalTractPage::OnMapShape)
  EVT_BUTTON(IDB_TRANSITION_DIALOG, VocalTractPage::OnTransitionDialog)

  EVT_BUTTON(IDB_PLAY_SHORT_VOWEL_TDS, VocalTractPage::OnPlayShortVowelTds)
  EVT_BUTTON(IDB_PLAY_LONG_VOWEL_TDS, VocalTractPage::OnPlayLongVowelTds)
  EVT_BUTTON(IDB_PLAY_FRICATIVE_TDS, VocalTractPage::OnPlayFricativeTds)
  EVT_BUTTON(IDB_PLAY_SHORT_VOWEL_FDS, VocalTractPage::OnPlayShortVowelFds)
  EVT_BUTTON(IDB_PLAY_LONG_VOWEL_FDS, VocalTractPage::OnPlayLongVowelFds)

  EVT_BUTTON(IDB_TESTING, VocalTractPage::OnTest)

  // Main panel controls

  EVT_CHECKBOX(IDC_SHOW_DISCRETE_AREAS, VocalTractPage::OnShowDiscreteAreas)
  EVT_CHECKBOX(IDC_SHOW_AREAS, VocalTractPage::OnShowAreas)
  EVT_CHECKBOX(IDC_SHOW_BRANCHES, VocalTractPage::OnShowBranches)
  EVT_CHECKBOX(IDC_SHOW_TEXT, VocalTractPage::OnShowText)
  EVT_CHECKBOX(IDC_SHOW_AREA_FUNCTION_ARTICULATORS, VocalTractPage::OnShowAreaFunctionArticulators)

END_EVENT_TABLE()


// ****************************************************************************
/// Constructor. 
// ****************************************************************************

VocalTractPage::VocalTractPage(wxWindow *parent, VocalTractPicture *picVocalTract) : 
  wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxCLIP_CHILDREN)
{
  initVars();
  initWidgets(picVocalTract);
  updateWidgets();
}


// ****************************************************************************
// ****************************************************************************

void VocalTractPage::updateWidgets()
{
  VocalTract *tract = data->vocalTract;
  
  // ****************************************************************
  // Widgets on the main panel
  // ****************************************************************

  // Display options for the area function.

  chkDiscreteAreaFunction->SetValue( picAreaFunction->discreteAreaFunction );
  chkAreaCircumference->SetValue( picAreaFunction->showAreas );
  chkShowBranches->SetValue( picAreaFunction->showSideBranches );
  chkShowAreaFunctionText->SetValue( picAreaFunction->showText );
  chkShowAreaFunctionArticulators->SetValue( picAreaFunction->showArticulators );

  // ****************************************************************
  // Widgets on the spectrum panel
  // ****************************************************************

  // Checkboxes.

  chkShowMagnitude->SetValue( picSpectrum->showMagnitude );
  chkShowPhase->SetValue( picSpectrum->showPhase ); 

  chkShowModelSpectrum->SetValue( picSpectrum->showModelSpectrum );
  chkShowUserSpectrum->SetValue( picSpectrum->showUserSpectrum );
  chkShowPoleZeroSpectrum->SetValue( picSpectrum->showPoleZeroSpectrum );
  
  // Formants are only shown for the model spectrum
  chkShowFormants->SetValue( picSpectrum->showFormants );
  if (picSpectrum->showModelSpectrum)
  {
    chkShowFormants->Enable();
  }
  else
  {
    chkShowFormants->Disable();
  }

  // Display the current secondary spectrum and the frequency range.

  labFrequencyRange->SetLabel( wxString::Format("%3.1f kHz", picSpectrum->graph.abscissa.positiveLimit / 1000.0) );

  // Display the cutoff freq. of the noise source

  labNoiseFilterCutoff->SetLabel(wxString::Format("%d Hz", (int)data->noiseFilterCutoffFreq));
  
  // The scroll bar for the level shift.

  scrSpectrumOffset->Enable( picSpectrum->showUserSpectrum | picSpectrum->showTdsSpectrum );
  
  if (picSpectrum->showUserSpectrum)
  {
    if (picSpectrum->levelShift < SpectrumPicture::MIN_SPECTRUM_LEVEL_SHIFT) 
    { 
      picSpectrum->levelShift = SpectrumPicture::MIN_SPECTRUM_LEVEL_SHIFT; 
    }
    if (picSpectrum->levelShift > SpectrumPicture::MAX_SPECTRUM_LEVEL_SHIFT) 
    { 
      picSpectrum->levelShift = SpectrumPicture::MAX_SPECTRUM_LEVEL_SHIFT; 
    }

    scrSpectrumOffset->SetThumbPosition( (int)picSpectrum->levelShift - SpectrumPicture::MIN_SPECTRUM_LEVEL_SHIFT );
  }

  // ****************************************************************
  // Repaint the pictures
  // ****************************************************************

  picAreaFunction->Refresh();
  picCrossSection->Refresh();
  picSpectrum->Refresh();
}


// ****************************************************************************
/// Init some variables.
// ****************************************************************************

void VocalTractPage::initVars()
{
  data = Data::getInstance();
}


// ****************************************************************************
/// Init the widgets.
// ****************************************************************************

void VocalTractPage::initWidgets(VocalTractPicture *picVocalTract)
{
  wxSizer *sizer;
  wxSizer *extraSizer;
  wxButton *button;

  // ****************************************************************
  // Top level sizer.
  // ****************************************************************

  wxBoxSizer *topLevelSizer = new wxBoxSizer(wxHORIZONTAL);

  // ****************************************************************
  // Left side sizer.
  // ****************************************************************

  wxBoxSizer *leftSizer = new wxBoxSizer(wxVERTICAL);

  // ****************************************************************

  button = new wxButton(this, IDB_VOCAL_TRACT_DIALOG, "Show vocal tract");
  leftSizer->Add(button, 0, wxGROW | wxALL, 3);

  button = new wxButton(this, IDB_SHAPES_DIALOG, "Vocal tract shapes");
  leftSizer->Add(button, 0, wxGROW | wxALL, 3);

  button = new wxButton(this, IDB_PHONETIC_PARAMS_DIALOG, "Phonetic parameters");
  leftSizer->Add(button, 0, wxGROW | wxALL, 3);

  button = new wxButton(this, IDB_LF_PULSE, "LF glottal flow pulse");
  leftSizer->Add(button, 0, wxGROW | wxALL, 3);

  button = new wxButton(this, IDB_FDS_ACOUSTICS, "Acoustics (frequency domain)");
  leftSizer->Add(button, 0, wxGROW | wxALL, 3);

  leftSizer->AddSpacer(10);

  button = new wxButton(this, IDB_IMPROVE_FORMANTS, "Improve formants");
  leftSizer->Add(button, 0, wxGROW | wxALL, 3);

  button = new wxButton(this, IDB_CHECK_VOWEL_SHAPE, "Check vowel shape");
  leftSizer->Add(button, 0, wxGROW | wxALL, 3);

  button = new wxButton(this, IDB_GET_FORMANT_ERROR, "Get formant error");
  leftSizer->Add(button, 0, wxGROW | wxALL, 3);

  button = new wxButton(this, IDB_EDIT_POLE_ZERO_PLAN, "Edit pole-zero plan");
  leftSizer->Add(button, 0, wxGROW | wxALL, 3);

  button = new wxButton(this, IDB_MAP_SHAPE, "Map shape to vowel subspace");
  leftSizer->Add(button, 0, wxGROW | wxALL, 3);
  
  button = new wxButton(this, IDB_TRANSITION_DIALOG, "CV transition dialog");
  leftSizer->Add(button, 0, wxGROW | wxALL, 3);

  leftSizer->AddSpacer(20);

  button = new wxButton(this, IDB_PLAY_SHORT_VOWEL_TDS, "Play short vowel (TDS)");
  leftSizer->Add(button, 0, wxGROW | wxALL, 3);

  button = new wxButton(this, IDB_PLAY_LONG_VOWEL_TDS, "Play long vowel (TDS)");
  leftSizer->Add(button, 0, wxGROW | wxALL, 3);

  button = new wxButton(this, IDB_PLAY_FRICATIVE_TDS, "Play fricative (TDS)");
  leftSizer->Add(button, 0, wxGROW | wxALL, 3);

  leftSizer->AddSpacer(20);

  button = new wxButton(this, IDB_PLAY_SHORT_VOWEL_FDS, "Play short vowel (FDS)");
  leftSizer->Add(button, 0, wxGROW | wxALL, 3);

  button = new wxButton(this, IDB_PLAY_LONG_VOWEL_FDS, "Play long vowel (FDS)");
  leftSizer->Add(button, 0, wxGROW | wxALL, 3);

  leftSizer->AddSpacer(20);

  button = new wxButton(this, IDB_TESTING, "Test");
  leftSizer->Add(button, 0, wxGROW | wxALL, 3);


  // ****************************************************************

  topLevelSizer->Add(leftSizer, 0, wxALL, 5);

  // ****************************************************************
  // Static line to separate the left and right part.
  // ****************************************************************

  wxStaticLine *verticalLine = new wxStaticLine(this, wxID_ANY, wxDefaultPosition, 
    wxDefaultSize, wxLI_VERTICAL);

  topLevelSizer->Add(verticalLine, 0, wxGROW | wxALL, 2);

  // ****************************************************************
  // Main splitter on the right size.
  // ****************************************************************

  splitter = new wxSplitterWindow(this, wxID_ANY,
    wxDefaultPosition, wxDefaultSize, wxSP_3DSASH | wxSP_LIVE_UPDATE);

  splitter->SetBackgroundColour(*wxWHITE); // If this is not set to white, every label will show up with a white background against the gray panels.
  splitter->SetMinimumPaneSize(200);   // Pane size may not be reduced to zero!

  wxPanel *topPanel = new wxPanel(splitter, wxID_ANY, wxDefaultPosition, wxDefaultSize);
  wxPanel *bottomPanel = new wxPanel(splitter, wxID_ANY, wxDefaultPosition, wxDefaultSize);
  splitter->SplitHorizontally(topPanel, bottomPanel);
  splitter->SetSashPosition(400);

  topLevelSizer->Add(splitter, 1, wxEXPAND);


  // ****************************************************************
  // Create the top panel (area function picture and cross-section
  // picture).
  // ****************************************************************

  wxBoxSizer *topSizer = new wxBoxSizer(wxHORIZONTAL);
  topPanel->SetSizer(topSizer);

  wxBoxSizer *topLeftSizer = new wxBoxSizer(wxVERTICAL);

  picAreaFunction = new AreaFunctionPicture(topPanel, picVocalTract, this);
  topLeftSizer->Add(picAreaFunction, 1, wxEXPAND | wxALL, 2);

  // Area function options sizer

  sizer = new wxBoxSizer(wxHORIZONTAL);

  chkDiscreteAreaFunction = new wxCheckBox(topPanel, IDC_SHOW_DISCRETE_AREAS, "Discrete");
  sizer->Add(chkDiscreteAreaFunction, 0, wxALL, 2);
  
  chkAreaCircumference = new wxCheckBox(topPanel, IDC_SHOW_AREAS, "Area/Circ.");
  sizer->Add(chkAreaCircumference, 0, wxALL, 2);
  
  chkShowBranches = new wxCheckBox(topPanel, IDC_SHOW_BRANCHES, "Show branches");
  sizer->Add(chkShowBranches, 0, wxALL, 2);
  
  chkShowAreaFunctionText = new wxCheckBox(topPanel, IDC_SHOW_TEXT, "Text");
  sizer->Add(chkShowAreaFunctionText, 0, wxALL, 2);

  chkShowAreaFunctionArticulators = new wxCheckBox(topPanel, IDC_SHOW_AREA_FUNCTION_ARTICULATORS, 
    "Articulators");
  sizer->Add(chkShowAreaFunctionArticulators, 0, wxALL, 2);

  topLeftSizer->Add(sizer);
  topSizer->Add(topLeftSizer, 2, wxEXPAND | wxALL, 2);

  // ****************************************************************

  picCrossSection = new CrossSectionPicture(topPanel, picVocalTract);
  topSizer->Add(picCrossSection, 1, wxEXPAND | wxALL, 2);

  // ****************************************************************
  // Create the bottom panel (spectrum panel).
  // ****************************************************************

  wxBoxSizer *bottomSizer = new wxBoxSizer(wxHORIZONTAL);
  bottomPanel->SetSizer(bottomSizer);

  // Left part

  sizer = new wxBoxSizer(wxVERTICAL);

  button = new wxButton(bottomPanel, IDB_UPPER_SPECTRUM_LIMIT_PLUS, "+", wxDefaultPosition, wxSize(25, 25));
  sizer->Add(button, 0, wxALL, 5);
  button = new wxButton(bottomPanel, IDB_UPPER_SPECTRUM_LIMIT_MINUS, "-", wxDefaultPosition, wxSize(25, 25));
  sizer->Add(button, 0, wxALL, 5);

  sizer->AddSpacer(5);
  sizer->AddStretchSpacer(1);

  button = new wxButton(bottomPanel, IDB_LOWER_SPECTRUM_LIMIT_PLUS, "+", wxDefaultPosition, wxSize(25, 25));
  sizer->Add(button, 0, wxALL, 5);
  button = new wxButton(bottomPanel, IDB_LOWER_SPECTRUM_LIMIT_MINUS, "-", wxDefaultPosition, wxSize(25, 25));
  sizer->Add(button, 0, wxALL, 5);

  bottomSizer->Add(sizer, 0, wxGROW | wxALL, 2);

  // The actual spectrum picture

  picSpectrum = new SpectrumPicture(bottomPanel, picVocalTract);
  bottomSizer->Add(picSpectrum, 1, wxGROW | wxTOP | wxBOTTOM, 5);

  // The scroll bar next to the spectrum

  scrSpectrumOffset = new wxScrollBar(bottomPanel, IDS_SPECTRUM_OFFSET, 
	  wxDefaultPosition, wxDefaultSize, wxVERTICAL);
  scrSpectrumOffset->SetScrollbar(0, 1, 
    SpectrumPicture::MAX_SPECTRUM_LEVEL_SHIFT - SpectrumPicture::MIN_SPECTRUM_LEVEL_SHIFT, 1);
  bottomSizer->Add(scrSpectrumOffset, 0, wxGROW);

  // The right side

  sizer = new wxBoxSizer(wxVERTICAL);

  sizer->AddSpacer(10);

  // Add some check boxes

  chkShowModelSpectrum = new wxCheckBox(bottomPanel, IDC_SHOW_MODEL_SPECTRUM, "Model spectrum");
  sizer->Add(chkShowModelSpectrum, 0, wxALL, 3);

  chkShowUserSpectrum = new wxCheckBox(bottomPanel, IDC_SHOW_USER_SPECTRUM, "User spectrum");
  sizer->Add(chkShowUserSpectrum, 0, wxALL, 3);

  chkShowTdsSpectrum = new wxCheckBox(bottomPanel, IDC_SHOW_TDS_SPECTRUM, "TDS spectrum");
  sizer->Add(chkShowTdsSpectrum, 0, wxALL, 3);

  chkShowPoleZeroSpectrum = new wxCheckBox(bottomPanel, IDC_SHOW_POLE_ZERO_SPECTRUM, "P.-Z.-Spectrum");
  sizer->Add(chkShowPoleZeroSpectrum, 0, wxALL, 3);

  chkShowFormants = new wxCheckBox(bottomPanel, IDC_SHOW_FORMANTS, "Formants (model s.)");
  sizer->Add(chkShowFormants, 0, wxALL, 3);

  sizer->AddSpacer(15);

  chkShowMagnitude = new wxCheckBox(bottomPanel, IDC_SHOW_MAGNITUDE, "Magnitude");
  sizer->Add(chkShowMagnitude, 0, wxALL, 3);

  chkShowPhase = new wxCheckBox(bottomPanel, IDC_SHOW_PHASE, "Phase");
  sizer->Add(chkShowPhase, 0, wxALL, 3);

  sizer->AddSpacer(15);

  // Extra horizontal sizer

  extraSizer = new wxBoxSizer(wxHORIZONTAL);
  
  button = new wxButton(bottomPanel, IDB_FREQUENCY_RANGE_MINUS, "-", wxDefaultPosition, wxSize(25, 25)); 
  extraSizer->Add(button);

  labFrequencyRange = new wxStaticText(bottomPanel, wxID_ANY, "0");
  extraSizer->Add(labFrequencyRange, 1, wxALIGN_CENTER | wxLEFT | wxRIGHT, 5);

  button = new wxButton(bottomPanel, IDB_FREQUENCY_RANGE_PLUS, "+", wxDefaultPosition, wxSize(25, 25)); 
  extraSizer->Add(button);

  sizer->Add(extraSizer, 0, wxGROW);

  sizer->AddSpacer(15);

  // Buttons and label to change the cutoff-freq. of the noise source.

  wxStaticText *label = new wxStaticText(bottomPanel, wxID_ANY, "Noise filter cutoff freq.");
  sizer->Add(label, 1, wxALL, 5);

  extraSizer = new wxBoxSizer(wxHORIZONTAL);

  button = new wxButton(bottomPanel, IDB_NOISE_FILTER_CUTOFF_MINUS, "-", wxDefaultPosition, wxSize(25, 25));
  extraSizer->Add(button);

  labNoiseFilterCutoff = new wxStaticText(bottomPanel, wxID_ANY, "0");
  extraSizer->Add(labNoiseFilterCutoff, 1, wxALIGN_CENTER | wxLEFT | wxRIGHT, 5);

  button = new wxButton(bottomPanel, IDB_NOISE_FILTER_CUTOFF_PLUS, "+", wxDefaultPosition, wxSize(25, 25));
  extraSizer->Add(button);

  sizer->Add(extraSizer, 0, wxGROW);

  // ********************************************

  button = new wxButton(bottomPanel, IDB_SYNTHESIZE_NOISE, "Synthesize noise", wxDefaultPosition, wxSize(120, 25));
  button->SetToolTip("Synthesize the radiated noise based on a noise source at the position marked in the area function.");
  sizer->Add(button, 0, wxBOTTOM | wxTOP | wxGROW, 5);

  // ********************************************

  bottomSizer->Add(sizer, 0, wxALL, 5);

  // ****************************************************************
  // Set the top-level sizer for this window.
  // ****************************************************************

  this->SetSizer(topLevelSizer);
}



// ****************************************************************************
/// Creates variants of the normal vocal tract shapes that are 2 cm longer and
/// 2 cm shorter than the normal shapes by modifying larynx height and lip
/// protrusion.
/// Furthermore, for vowels, more centralized vocal tract shapes are created
/// to synthesize reduced vowel precision.
/// This is all for the PhD-experiment of Lucia Martin.
// ****************************************************************************

void VocalTractPage::createSpecialVocalTractShapes()
{
  VocalTract::Shape *origShape;
  VocalTract::Shape newShape;
  vector<VocalTract::Shape> newShapes;

  int i, k;
  int index;
  string name;
  VocalTract *vocalTract = data->vocalTract;
  double schwaParam[VocalTract::NUM_PARAMS];
  
  // ****************************************************************
  // Determine the change in the parameter LP that corresponds to 
  // about 1 cm difference in protrusion.
  // ****************************************************************

  double maxProtrusion_cm =
    vocalTract->narrowLipCornerPath.getPoint(vocalTract->narrowLipCornerPath.getNumPoints() - 1).x -
    vocalTract->narrowLipCornerPath.getPoint(0).x;

  double lpRange = vocalTract->param[VocalTract::LP].max - vocalTract->param[VocalTract::LP].min;

  double lpDelta = lpRange / maxProtrusion_cm;

  // ****************************************************************
  // Determine the parameter values of the schwa.
  // ****************************************************************

  int schwaIndex = vocalTract->getShapeIndex("@");
  if (schwaIndex == -1)
  {
    printf("Error: Schwa shape was not found in the list!\n");
    return;
  }

  for (i = 0; i < VocalTract::NUM_PARAMS; i++)
  {
    schwaParam[i] = vocalTract->shapes[schwaIndex].param[i];
  }
  

  // ****************************************************************
  // Run through all original shapes.
  // ****************************************************************

  int N = (int)(data->vocalTract->shapes.size());

  for (i = 0; i < N; i++)
  {
    origShape = &data->vocalTract->shapes[i];
    name = origShape->name;

    if ((name.find("raw") == string::npos) &&
      (name.find("%") == string::npos) &&
      (name.find("-longer") == string::npos) &&
      (name.find("-shorter") == string::npos))
    {

      printf("%s\n", name.c_str());

      // ************************************************************
      // Insert the original shape into the new list.
      // ************************************************************

      newShape = *origShape;
      newShapes.push_back(newShape);

      // ************************************************************
      // Insert a shape with lowered larynx and protruded lips.
      // ************************************************************

      newShape = *origShape;

      index = VocalTract::HY;
      newShape.param[index] -= 1.0;
      if (newShape.param[index] < vocalTract->param[index].min)
      {
        newShape.param[index] = vocalTract->param[index].min;
      }

      index = VocalTract::LP;
      newShape.param[index] += lpDelta;
      if (newShape.param[index] > vocalTract->param[index].max)
      {
        newShape.param[index] = vocalTract->param[index].max;
      }

      // Insert new shape only for vowels but not consonants.
      if ((newShape.name.find("(a)") == string::npos) &&
        (newShape.name.find("(i)") == string::npos) &&
        (newShape.name.find("(u)") == string::npos))
      {
        newShape.name.append("-longer");
        newShapes.push_back(newShape);
      }


      // ************************************************************
      // Insert a shape with raised larynx and retracted lips.
      // ************************************************************

      newShape = *origShape;

      index = VocalTract::HY;
      newShape.param[index] += 1.0;
      if (newShape.param[index] > vocalTract->param[index].max)
      {
        newShape.param[index] = vocalTract->param[index].max;
      }

      index = VocalTract::LP;
      newShape.param[index] -= lpDelta;
      if (newShape.param[index] < vocalTract->param[index].min)
      {
        newShape.param[index] = vocalTract->param[index].min;
      }

      // Insert new shape only for vowels but not consonants.
      if ((newShape.name.find("(a)") == string::npos) &&
        (newShape.name.find("(i)") == string::npos) &&
        (newShape.name.find("(u)") == string::npos))
      {
        newShape.name.append("-shorter");
        newShapes.push_back(newShape);
      }


      // ************************************************************
      // Insert shapes with a more centralized VT configuration
      // (for vowels only).
      // ************************************************************

      if ((newShape.name.find("ll-") == string::npos) &&
        (newShape.name.find("tt-") == string::npos) &&
        (newShape.name.find("tb-") == string::npos))
      {
        // One shape with 75% of the original variation.
        newShape = *origShape;

        double factor = 0.75;
        for (k = 0; k < VocalTract::NUM_PARAMS; k++)
        {
          newShape.param[k] = factor*newShape.param[k] + (1.0 - factor) * schwaParam[k];
        }

        newShape.name.append("-75%");
        newShapes.push_back(newShape);

        // One shape with 50% of the original variation.
        newShape = *origShape;

        factor = 0.5;
        for (k = 0; k < VocalTract::NUM_PARAMS; k++)
        {
          newShape.param[k] = factor*newShape.param[k] + (1.0 - factor) * schwaParam[k];
        }

        newShape.name.append("-50%");
        newShapes.push_back(newShape);
      }
    }


  }


  // ****************************************************************
  // Replace the original shapes with the new shapes.
  // ****************************************************************

  data->vocalTract->shapes = newShapes;

  if (VocalTractShapesDialog::getInstance(this)->IsShown())
  {
    VocalTractShapesDialog::getInstance(this)->fillShapeList();
  }
}


// ****************************************************************************
/// Is called, when one of the child windows has requested the update of
/// other child windows.
// ****************************************************************************

void VocalTractPage::OnUpdateRequest(wxCommandEvent &event)
{
  if (event.GetInt() == REFRESH_PICTURES)
  {
    picAreaFunction->Refresh();
    picCrossSection->Refresh();
    picSpectrum->Refresh();
  }
  else
  if (event.GetInt() == UPDATE_PICTURES)
  {
    picAreaFunction->Refresh();
    picAreaFunction->Update();
    picCrossSection->Refresh();
    picCrossSection->Update();
    picSpectrum->Refresh();
    picSpectrum->Update();
  }
  else
  if (event.GetInt() == REFRESH_PICTURES_AND_CONTROLS)
  {
    updateWidgets();
  }
  else
  if (event.GetInt() == UPDATE_PICTURES_AND_CONTROLS)
  {
    updateWidgets();
    picAreaFunction->Update();
    picCrossSection->Update();
    picSpectrum->Update();
  }
  else
  if (event.GetInt() == UPDATE_VOCAL_TRACT)
  {
    updateWidgets();
    picAreaFunction->Update();
    picCrossSection->Update();
    picSpectrum->Update();
  }
}


// ****************************************************************************
// ****************************************************************************

void VocalTractPage::OnVocalTractDialog(wxCommandEvent &event)
{
  VocalTractDialog *dialog = VocalTractDialog::getInstance(this);
  dialog->Show(true);
}


// ****************************************************************************
// ****************************************************************************

void VocalTractPage::OnShapesDialog(wxCommandEvent &event)
{
  VocalTractShapesDialog *dialog = VocalTractShapesDialog::getInstance();
  dialog->Show(true);
}


// ****************************************************************************
// ****************************************************************************

void VocalTractPage::OnPhoneticParamsDialog(wxCommandEvent &event)
{
  PhoneticParamsDialog *dialog = PhoneticParamsDialog::getInstance();
  dialog->SetParent(this);
  dialog->Show(true);
}


// ****************************************************************************
// ****************************************************************************

void VocalTractPage::OnLfPulse(wxCommandEvent &event)
{
  LfPulseDialog *dialog = LfPulseDialog::getInstance();
  dialog->Show(true);
}

// ****************************************************************************
// ****************************************************************************

void VocalTractPage::OnFdsAcoustics(wxCommandEvent &event)
{
  FdsOptionsDialog *dialog = FdsOptionsDialog::getInstance();
  dialog->SetParent(this);
  dialog->Show(true);
}


// ****************************************************************************
/// Optimize the vocal tract parameters with respect to the first three
/// formant frequencies.
// ****************************************************************************

void VocalTractPage::OnImproveFormants(wxCommandEvent &event)
{
  FormantOptimizationDialog *dialog = data->formantOptimizationDialog;

  dialog->SetParent(this);
  if (dialog->ShowModal() == wxID_OK)
  {
    if (dialog->optimizeVowel)
    {
      printf("Optimize vowel shape...\n");
      data->optimizeFormantsVowel(this, data->vocalTract, dialog->F1, dialog->F2, dialog->F3, 
        dialog->maxDisplacement_mm/10.0, dialog->minArea_mm2/100.0, dialog->paramFixed);
    }
    else
    {
      printf("Optimize consonant shape...\n");
      data->optimizeFormantsConsonant(this, data->vocalTract, dialog->contextVowel, 
        dialog->F1, dialog->F2, dialog->F3,  
        dialog->maxDisplacement_mm/10.0, dialog->minArea_mm2/100.0, dialog->releaseArea_mm2/100.0, dialog->paramFixed);
    }
  }

}


// ****************************************************************************
/// Checks wheather this is a vowel shape, i.e. that all cross-sectional areas
/// are > 0.25 cm^2. If this is not the case, parameters are changed until it
/// is a vowel shape.
// ****************************************************************************

void VocalTractPage::OnCheckVowelShape(wxCommandEvent &event)
{
  double currF1, currF2, currF3;
  double minArea_cm2;

  data->getVowelFormants(data->vocalTract, currF1, currF2, currF3, minArea_cm2);

  if (minArea_cm2 < Data::MIN_ADVISED_VOWEL_AREA_CM2)
  {
    wxString st = wxString::Format("The current minimum cross-sectional area is %2.2f cm^2.\n"
      "The recommended minimum area is %2.2f cm^2.\n"
      "Would you like an automatic adjustment of the vocal tract shape?", 
      minArea_cm2, Data::MIN_ADVISED_VOWEL_AREA_CM2);
    if (wxMessageBox(st, "Do automatic adjustment?", wxYES_NO ) == wxYES)
    {
      data->createMinVocalTractArea(this, data->vocalTract, Data::MIN_ADVISED_VOWEL_AREA_CM2);
    }
  }
  else
  {
    wxString st = wxString::Format("The cross-sectional area is everywhere greater than %2.2f cm^2.", 
      Data::MIN_ADVISED_VOWEL_AREA_CM2);
    wxMessageBox(st, "Good vowel shape");
  }

}


// ****************************************************************************
/// Returns the average deviation of the current VT formants from the user-
/// given formants.
// ****************************************************************************

void VocalTractPage::OnGetFormantError(wxCommandEvent &event)
{
  static wxString lastString = "500 1500 2500";

  wxString st = wxGetTextFromUser(
    "Please enter the reference frequencies of the first three formants,\nseparated by spaces.",
    "Input target formant frequencies",
    lastString, this, -1, -1, true);

  if (st.IsEmpty())
  {
    return;
  }

  lastString = st;

  // Parse the string for the three reference formant frequencies.
  double referenceF1 = 0.0, referenceF2 = 0.0, referenceF3 = 0.0;

  istringstream is(st.ToStdString());
  is >> referenceF1;
  is >> referenceF2;
  is >> referenceF3;

  // Get the three current vocal tract formants.
  double currF1, currF2, currF3;
  double minArea_cm2;

  data->getVowelFormants(data->vocalTract, currF1, currF2, currF3, minArea_cm2);

  double e = data->getFormantError(currF1, currF2, currF3, referenceF1, referenceF2, referenceF3);

  st = wxString::Format("The current formants deviate by %2.2f percent.", e);
  wxMessageBox(st, "Formant error");
}


// ****************************************************************************
// ****************************************************************************

void VocalTractPage::OnEditPoleZeroPlan(wxCommandEvent &event)
{
  PoleZeroDialog *dialog = PoleZeroDialog::getInstance();
  dialog->setSpectrumPicture(picSpectrum);
  dialog->SetParent(this);
  dialog->Show(true);
}


// ****************************************************************************
/// Implement different kinds of function tests here.
// ****************************************************************************

void VocalTractPage::OnTest(wxCommandEvent &event)
{
  Data *data = Data::getInstance();

  return;     // Do nothin here in the official release!

/*
  vtlInitialize("c:\\temp\\JD2.speaker");

  int code = 0;
  code = vtlGesturalScoreToTractSequence("c:\\temp\\asa.ges", "c:\\temp\\asa.txt");
  printf("Return code 1: %d\n", code);

  code = vtlGesturalScoreToTractSequence("c:\\temp\\isi.ges", "c:\\temp\\isi.txt");
  printf("Return code 2: %d\n", code);
*/


}


// ****************************************************************************
/// Maps the current vocal tract shape into the 2D-subspace defined by the 
/// shapes for the corner vowels /a/, /i/, and /u/. The coordinates in the 
/// subspace are displayed and the projected shape is set as the current shape.
// ****************************************************************************

void VocalTractPage::OnMapShape(wxCommandEvent &event)
{
  int i;
  double vocalTractParams[VocalTract::NUM_PARAMS];
  
  double alphaTongue;
  double betaTongue;
  double alphaLips;
  double betaLips;

  for (i=0; i < VocalTract::NUM_PARAMS; i++)
  {
    vocalTractParams[i] = data->vocalTract->param[i].x;
  }
  
  GesturalScore::mapToVowelSubspace(data->vocalTract, vocalTractParams, 
    alphaTongue, betaTongue, alphaLips, betaLips);

  wxString st = wxString::Format("%2.3f  %2.3f  %2.3f  %2.3f", alphaTongue, betaTongue, alphaLips, betaLips);
  wxString result = wxGetTextFromUser("Results for a_tongue, b_tongue, a_lips, b_lips. Press OK to set the interpolated shape.", 
    "Coefficients of the subspace mapping", st);

  // If the user pressed CANCEL when the dialog with the coefficients
  // was shown, return immediately. Otherwise re-calculate the current
  // vocal tract shape.

  if (result.IsEmpty())
  {
    return;
  }

  // ****************************************************************
  // Set the current shape to the determined position in the vowel 
  // subspace.
  // ****************************************************************

  // Get the indices of the vocal tract shapes for the vowels /i/, /a/, and /u/.

  VocalTract *vt = data->vocalTract;

  int shapeIndexA = vt->getShapeIndex("a");
  int shapeIndexI = vt->getShapeIndex("i");
  int shapeIndexU = vt->getShapeIndex("u");

  // Return if one of the shapes was not found.

  if ((shapeIndexA == -1) || (shapeIndexI == -1) || (shapeIndexU == -1))
  {
    return;
  }

  // Interpolate the new shape between /a/, /i/, and /u/.
  
  for (i=0; i < VocalTract::NUM_PARAMS; i++)
  {
    vt->param[i].x = vt->shapes[shapeIndexA].param[i] + 
      alphaTongue*(vt->shapes[shapeIndexI].param[i] - vt->shapes[shapeIndexA].param[i]) +
      betaTongue*(vt->shapes[shapeIndexU].param[i] - vt->shapes[shapeIndexA].param[i]);
  }

  // The two lip parameters are interpolated individually

  i = VocalTract::LP;
  vt->param[i].x = vt->shapes[shapeIndexA].param[i] + 
    alphaLips*(vt->shapes[shapeIndexI].param[i] - vt->shapes[shapeIndexA].param[i]) +
    betaLips*(vt->shapes[shapeIndexU].param[i] - vt->shapes[shapeIndexA].param[i]);

  i = VocalTract::LD;
  vt->param[i].x = vt->shapes[shapeIndexA].param[i] + 
    alphaLips*(vt->shapes[shapeIndexI].param[i] - vt->shapes[shapeIndexA].param[i]) +
    betaLips*(vt->shapes[shapeIndexU].param[i] - vt->shapes[shapeIndexA].param[i]);

  // Recalculate the VT and update all widgets.

  vt->calculateAll();

  updateWidgets();

  VocalTractDialog *vocalTractDialog = VocalTractDialog::getInstance(NULL);
  if (vocalTractDialog->IsShown())
  {
    vocalTractDialog->Refresh();
    vocalTractDialog->Update();
  }

}


// ****************************************************************************
/// Calls a dialog to analyze a CV-transition.
// ****************************************************************************

void VocalTractPage::OnTransitionDialog(wxCommandEvent &event)
{
  TransitionDialog *dialog = TransitionDialog::getInstance();
  dialog->SetParent(this);
  dialog->Show(true);
  dialog->Raise();
}


// ****************************************************************************
// ****************************************************************************

void VocalTractPage::OnPlayShortVowelTds(wxCommandEvent& event)
{
  Data* data = Data::getInstance();
  Glottis* g = data->glottis[Data::GEOMETRIC_GLOTTIS];
  int numGlottisParams = (int)g->controlParam.size();
  vector<double> audio;
  int i;

  // Store the current glottis model parameters.
  
  double storedGlottisParams[Glottis::MAX_CONTROL_PARAMS];
  for (i = 0; i < numGlottisParams; i++)
  {
    storedGlottisParams[i] = g->controlParam[i].x;
  }
  
  // Set the glottis model parameters for modal phonation
  
  Glottis::Shape* s = g->getShape("modal");
  for (i = 0; i < numGlottisParams; i++)
  {
    if (s != NULL)
    {
      g->controlParam[i].x = s->controlParam[i];
    }
    else
    {
      g->controlParam[i].x = g->controlParam[i].neutral;
    }
  }
  g->controlParam[Glottis::FREQUENCY].x = 110.0;

  // Do the actual synthesis.
  
  Synthesizer::synthesizeStaticPhoneme(g, data->vocalTract,
    data->tdsModel, true, true, audio);

  data->track[Data::MAIN_TRACK]->setZero();
  Synthesizer::copySignal(audio, *data->track[Data::MAIN_TRACK], 0);
  data->normalizeAudioAmplitude(Data::MAIN_TRACK);

  int duration_ms = (double)audio.size() * 1000.0 / (double)SAMPLING_RATE;

  if (waveStartPlaying(data->track[Data::MAIN_TRACK]->x, data->track[Data::MAIN_TRACK]->N, false))
  {
    wxMilliSleep(duration_ms);
    waveStopPlaying();
  }
  else
  {
    wxMessageBox("Playing failed.", "Attention!");
  }

  // Restore the glottis model parameters.

  for (i = 0; i < numGlottisParams; i++)
  {
    g->controlParam[i].x = storedGlottisParams[i];
  }
}


// ****************************************************************************
// ****************************************************************************

void VocalTractPage::OnPlayLongVowelTds(wxCommandEvent& event)
{
  Data* data = Data::getInstance();
  Glottis* g = data->glottis[Data::GEOMETRIC_GLOTTIS];
  int numGlottisParams = (int)g->controlParam.size();
  vector<double> audio;
  int i;

  // Store the current glottis model parameters.

  double storedGlottisParams[Glottis::MAX_CONTROL_PARAMS];
  for (i = 0; i < numGlottisParams; i++)
  {
    storedGlottisParams[i] = g->controlParam[i].x;
  }

  // Set the glottis model parameters for modal phonation

  Glottis::Shape* s = g->getShape("modal");
  for (i = 0; i < numGlottisParams; i++)
  {
    if (s != NULL)
    {
      g->controlParam[i].x = s->controlParam[i];
    }
    else
    {
      g->controlParam[i].x = g->controlParam[i].neutral;
    }
  }
  g->controlParam[Glottis::FREQUENCY].x = 110.0;

  // Do the actual synthesis.

  Synthesizer::synthesizeStaticPhoneme(g, data->vocalTract,
    data->tdsModel, false, true, audio);

  data->track[Data::MAIN_TRACK]->setZero();
  Synthesizer::copySignal(audio, *data->track[Data::MAIN_TRACK], 0);
  data->normalizeAudioAmplitude(Data::MAIN_TRACK);

  int duration_ms = (double)audio.size() * 1000.0 / (double)SAMPLING_RATE;

  if (waveStartPlaying(data->track[Data::MAIN_TRACK]->x, data->track[Data::MAIN_TRACK]->N, false))
  {
    wxMilliSleep(duration_ms);
    waveStopPlaying();
  }
  else
  {
    wxMessageBox("Playing failed.", "Attention!");
  }

  // Restore the glottis model parameters.

  for (i = 0; i < numGlottisParams; i++)
  {
    g->controlParam[i].x = storedGlottisParams[i];
  }
}


// ****************************************************************************
// ****************************************************************************

void VocalTractPage::OnPlayFricativeTds(wxCommandEvent& event)
{
  Data* data = Data::getInstance();
  Glottis* g = data->glottis[Data::GEOMETRIC_GLOTTIS];
  int numGlottisParams = (int)g->controlParam.size();
  vector<double> audio;
  int i;

  // Store the current glottis model parameters.

  double storedGlottisParams[Glottis::MAX_CONTROL_PARAMS];
  for (i = 0; i < numGlottisParams; i++)
  {
    storedGlottisParams[i] = g->controlParam[i].x;
  }

  // Set the glottis model parameters for modal phonation

  Glottis::Shape* s = g->getShape("voiceless-fricative");
  for (i = 0; i < numGlottisParams; i++)
  {
    if (s != NULL)
    {
      g->controlParam[i].x = s->controlParam[i];
    }
    else
    {
      g->controlParam[i].x = g->controlParam[i].neutral;
    }
  }

  // Do the actual synthesis.

  Synthesizer::synthesizeStaticPhoneme(g, data->vocalTract,
    data->tdsModel, false, true, audio);

  data->track[Data::MAIN_TRACK]->setZero();
  Synthesizer::copySignal(audio, *data->track[Data::MAIN_TRACK], 0);
  data->normalizeAudioAmplitude(Data::MAIN_TRACK);

  int duration_ms = (double)audio.size() * 1000.0 / (double)SAMPLING_RATE;

  if (waveStartPlaying(data->track[Data::MAIN_TRACK]->x, data->track[Data::MAIN_TRACK]->N, false))
  {
    wxMilliSleep(duration_ms);
    waveStopPlaying();
  }
  else
  {
    wxMessageBox("Playing failed.", "Attention!");
  }

  // Restore the glottis model parameters.

  for (i = 0; i < numGlottisParams; i++)
  {
    g->controlParam[i].x = storedGlottisParams[i];
  }
}


// ****************************************************************************
// ****************************************************************************

void VocalTractPage::OnPlayShortVowelFds(wxCommandEvent &event)
{
  Data *data = Data::getInstance();

  data->track[Data::MAIN_TRACK]->setZero();

  data->vocalTract->calculateAll();
  data->updateTlModelGeometry( data->vocalTract );
  int duration_ms = data->synthesizeVowelLf(data->tlModel, data->lfPulse, 0, false);
  data->normalizeAudioAmplitude(Data::MAIN_TRACK);

  if (waveStartPlaying(data->track[Data::MAIN_TRACK]->x, data->track[Data::MAIN_TRACK]->N, false))
  {
    wxMilliSleep(duration_ms);
    waveStopPlaying();
  }
  else
  {
    wxMessageBox("Playing failed.", "Attention!");
  }
}

// ****************************************************************************
// ****************************************************************************

void VocalTractPage::OnPlayLongVowelFds(wxCommandEvent &event)
{
  Data *data = Data::getInstance();

  data->track[Data::MAIN_TRACK]->setZero();

  data->vocalTract->calculateAll();
  data->updateTlModelGeometry( data->vocalTract );
  int duration_ms = data->synthesizeVowelLf(data->tlModel, data->lfPulse, 0, true);
  data->normalizeAudioAmplitude(Data::MAIN_TRACK);

  if (waveStartPlaying(data->track[Data::MAIN_TRACK]->x, data->track[Data::MAIN_TRACK]->N, false))
  {
    wxMilliSleep(duration_ms);
    waveStopPlaying();
  }
  else
  {
    wxMessageBox("Playing failed.", "Attention!");
  }
}


// ****************************************************************************
// ****************************************************************************

void VocalTractPage::OnFrequencyRangeMinus(wxCommandEvent &event)
{
  picSpectrum->graph.zoomInAbscissa(false, true);
  updateWidgets();
}

// ****************************************************************************
// ****************************************************************************

void VocalTractPage::OnFrequencyRangePlus(wxCommandEvent &event)
{
  picSpectrum->graph.zoomOutAbscissa(false, true);
  updateWidgets();
}


// ****************************************************************************
// ****************************************************************************

void VocalTractPage::OnNoiseFilterCutoffMinus(wxCommandEvent &event)
{
  if (data->noiseFilterCutoffFreq > 1.0)
  {
    data->noiseFilterCutoffFreq /= 1.25;
    updateWidgets();
  }
}


// ****************************************************************************
// ****************************************************************************

void VocalTractPage::OnNoiseFilterCutoffPlus(wxCommandEvent &event)
{
  if (data->noiseFilterCutoffFreq < 8000.0)
  {
    data->noiseFilterCutoffFreq *= 1.25;
    updateWidgets();
  }
}


// ****************************************************************************
// ****************************************************************************

void VocalTractPage::OnSynthesizeNoise(wxCommandEvent &event)
{
  const int SPECTRUM_EXPONENT = 13;
  const int SPECTRUM_LENGTH = ((int)1 << SPECTRUM_EXPONENT);
  const int IMPULSE_RESPONSE_LENGTH = SPECTRUM_LENGTH;
  ComplexSignal spectrum(SPECTRUM_LENGTH);
  VocalTractPicture *picVocalTract = VocalTractDialog::getInstance(this)->getVocalTractPicture();
  double noiseSourcePos_cm = picVocalTract->cutPlanePos_cm;
  Signal window;
  double impulseResponse[IMPULSE_RESPONSE_LENGTH];
  int i, k;

  // ****************************************************************
  // Calc. the spectrum and impulse response of the radiated noise.
  // ****************************************************************

  data->calcRadiatedNoiseSpectrum(noiseSourcePos_cm, 
    data->noiseFilterCutoffFreq, SPECTRUM_LENGTH, &spectrum);

  complexIFFT(spectrum, SPECTRUM_EXPONENT, true);
  getWindow(window, IMPULSE_RESPONSE_LENGTH, RIGHT_HALF_OF_HAMMING_WINDOW);

  for (i = 0; i < IMPULSE_RESPONSE_LENGTH; i++)
  {
    impulseResponse[i] = spectrum.re[i]*window.x[i];
  }

  // ****************************************************************
  // Generate a sequence of white noise with the stimulus duration
  // with fade-in and fade-out.
  // ****************************************************************

  const int INPUT_SIGNAL_LENGTH = SAMPLING_RATE;    // For 1 s of noise
  const int FADING_LENGTH = (int)(0.05 * INPUT_SIGNAL_LENGTH);
  double inputNoise[INPUT_SIGNAL_LENGTH];
  double sample = 0.0;

  for (i = 0; i < INPUT_SIGNAL_LENGTH; i++)
  {
    sample =
      rand() + rand() + rand() + rand() + rand() + rand() +
      rand() + rand() + rand() + rand() + rand() + rand();
    
    sample /= (double)RAND_MAX;
    sample -= 6.0;
    sample /= sqrt(12.0);    // Divide by the standard deviation of the above sum

    // Implement the fading-in and fading-out.
    if (i < FADING_LENGTH)
    {
      sample *= (double)i / (double)FADING_LENGTH;
    }
    else
    if (i >= INPUT_SIGNAL_LENGTH - FADING_LENGTH)
    {
      sample *= 1.0 - (double)(i - INPUT_SIGNAL_LENGTH + FADING_LENGTH) / (double)FADING_LENGTH;
    }

    inputNoise[i] = sample;
  }

  // ****************************************************************
  // Convolve the input noise signal with the filter impulse response
  // and apply the 12 kHz low-pass filter to the final signal.
  // ****************************************************************
  
  IirFilter outputPressureFilter;
  outputPressureFilter.createChebyshev((double)SYNTHETIC_SPEECH_BANDWIDTH_HZ / (double)SAMPLING_RATE, false, 8);

  // Clear the audio track.
  data->track[Data::MAIN_TRACK]->setZero();

  const int OUTPUT_SIGNAL_LENGTH = INPUT_SIGNAL_LENGTH + IMPULSE_RESPONSE_LENGTH - 1;
  double sum = 0.0;

  for (i = 0; i < OUTPUT_SIGNAL_LENGTH; i++)
  {
    sum = 0.0;
    for (k = 0; k < IMPULSE_RESPONSE_LENGTH; k++)
    {
      if ((i - k >= 0) && (i - k < INPUT_SIGNAL_LENGTH))
      {
        sum += inputNoise[i - k] * impulseResponse[k];
      }
    }

    // Apply the 12 kHz synthesis low-pass filter.
    sample = outputPressureFilter.getOutputSample(sum);

    // Put the sample in the audio track.
    data->track[Data::MAIN_TRACK]->setValue(i, sample * 200.0);
  }

  // ****************************************************************
  // Play the result.
  // ****************************************************************

  if (waveStartPlaying(data->track[Data::MAIN_TRACK]->x, data->track[Data::MAIN_TRACK]->N, true))
  {
    SilentMessageBox dialog("Press OK to stop playing!", "Stop playing");
    dialog.ShowModal();
    waveStopPlaying();
  }
  else
  {
    wxMessageBox("Playing failed.", "Attention!", wxOK, this);
  }
}


// ****************************************************************************
// ****************************************************************************

void VocalTractPage::OnUpperSpectrumLimitPlus(wxCommandEvent &event)
{
  picSpectrum->graph.zoomOutOrdinate(false, true);
  picSpectrum->Refresh();
}

// ****************************************************************************
// ****************************************************************************

void VocalTractPage::OnUpperSpectrumLimitMinus(wxCommandEvent &event)
{
  picSpectrum->graph.zoomInOrdinate(false, true);
  picSpectrum->Refresh();
}

// ****************************************************************************
// ****************************************************************************

void VocalTractPage::OnLowerSpectrumLimitPlus(wxCommandEvent &event)
{
  picSpectrum->graph.zoomInOrdinate(true, false);
  picSpectrum->Refresh();
}

// ****************************************************************************
// ****************************************************************************

void VocalTractPage::OnLowerSpectrumLimitMinus(wxCommandEvent &event)
{
  picSpectrum->graph.zoomOutOrdinate(true, false);
  picSpectrum->Refresh();
}

// ****************************************************************************
// ****************************************************************************


void VocalTractPage::OnShowModelSpectrum(wxCommandEvent &event)
{
  picSpectrum->showModelSpectrum = !picSpectrum->showModelSpectrum;
  updateWidgets();
}

// ****************************************************************************
// ****************************************************************************

void VocalTractPage::OnShowUserSpectrum(wxCommandEvent &event)
{
  picSpectrum->showUserSpectrum = !picSpectrum->showUserSpectrum;
  updateWidgets();
}

// ****************************************************************************
// ****************************************************************************

void VocalTractPage::OnShowTdsSpectrum(wxCommandEvent &event)
{
  picSpectrum->showTdsSpectrum = !picSpectrum->showTdsSpectrum;
  updateWidgets();
}

// ****************************************************************************
// ****************************************************************************

void VocalTractPage::OnShowPoleZeroSpectrum(wxCommandEvent &event)
{
  picSpectrum->showPoleZeroSpectrum = !picSpectrum->showPoleZeroSpectrum;
  picSpectrum->Refresh();
}

// ****************************************************************************
// ****************************************************************************

void VocalTractPage::OnShowFormants(wxCommandEvent &event)
{
  picSpectrum->showFormants = !picSpectrum->showFormants;
  picSpectrum->Refresh();
}

// ****************************************************************************
// ****************************************************************************

void VocalTractPage::OnShowMagnitude(wxCommandEvent &event)
{
  picSpectrum->showMagnitude = !picSpectrum->showMagnitude;
  picSpectrum->Refresh();
}

// ****************************************************************************
// ****************************************************************************

void VocalTractPage::OnShowPhase(wxCommandEvent &event)
{
  picSpectrum->showPhase = !picSpectrum->showPhase;
  picSpectrum->Refresh();
}

// ****************************************************************************
// ****************************************************************************

void VocalTractPage::OnSpectrumOffsetChanged(wxScrollEvent &event)
{
  int pos = scrSpectrumOffset->GetThumbPosition();
  picSpectrum->levelShift = SpectrumPicture::MIN_SPECTRUM_LEVEL_SHIFT + pos;

  picSpectrum->Refresh();
}

// ****************************************************************************
// ****************************************************************************


void VocalTractPage::OnShowDiscreteAreas(wxCommandEvent &event)
{
  picAreaFunction->discreteAreaFunction = !picAreaFunction->discreteAreaFunction;
  picAreaFunction->Refresh();
}

// ****************************************************************************
// ****************************************************************************

void VocalTractPage::OnShowAreas(wxCommandEvent &event)
{
  picAreaFunction->showAreas = !picAreaFunction->showAreas;
  picAreaFunction->Refresh();
}

// ****************************************************************************
// ****************************************************************************

void VocalTractPage::OnShowBranches(wxCommandEvent &event)
{
  picAreaFunction->showSideBranches = !picAreaFunction->showSideBranches;
  picAreaFunction->Refresh();
}

// ****************************************************************************
// ****************************************************************************

void VocalTractPage::OnShowText(wxCommandEvent &event)
{
  picAreaFunction->showText = !picAreaFunction->showText;
  updateWidgets();
}


// ****************************************************************************
// ****************************************************************************

void VocalTractPage::OnShowAreaFunctionArticulators(wxCommandEvent &event)
{
  picAreaFunction->showArticulators = !picAreaFunction->showArticulators;
  updateWidgets();
}

// ****************************************************************************
