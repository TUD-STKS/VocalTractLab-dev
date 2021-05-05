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

#include "TransitionDialog.h"
#include "SilentMessageBox.h"
#include "Backend/SoundLib.h"
#include "Backend/GesturalScore.h"

static const double MIN_ALPHA = 0.0;
static const double MAX_ALPHA = 1.0;
static const double MIN_BETA = 0.0;
static const double MAX_BETA = 1.0;


// ****************************************************************************
// IDs.
// ****************************************************************************

static const int IDL_CONSONANT          = 4000;
static const int IDR_FIXED_VOWEL        = 4002;
static const int IDL_VOWEL              = 4003;
static const int IDL_VOWEL_PARAMS       = 4004;
static const int IDR_INTERPOLATED_VOWEL = 4005;
static const int IDS_ALPHA              = 4006;
static const int IDS_BETA               = 4007;
static const int IDL_ALPHA              = 4008;
static const int IDL_BETA               = 4009;
static const int IDS_TRANSITION_POS     = 4010;
static const int IDL_TRANSITION_POS     = 4011;
static const int IDB_UPDATE_LISTS       = 4012;

static const int IDB_FIND_RELEASE_POS   = 4020;
static const int IDE_MIN_AREA           = 4021;

// ****************************************************************************
// The event table.
// ****************************************************************************

BEGIN_EVENT_TABLE(TransitionDialog, wxDialog)
  EVT_COMBOBOX(IDL_CONSONANT, TransitionDialog::OnConsonantSelected)
  EVT_COMBOBOX(IDL_VOWEL, TransitionDialog::OnVowelSelected)
  EVT_RADIOBUTTON(IDR_FIXED_VOWEL, TransitionDialog::OnUseFixedVowel)
  EVT_RADIOBUTTON(IDR_INTERPOLATED_VOWEL, TransitionDialog::OnUseInterpolatedVowel)
  EVT_SCROLL(TransitionDialog::OnScrollbarChanged)
  EVT_BUTTON(IDB_UPDATE_LISTS, TransitionDialog::OnUpdateLists)
  EVT_BUTTON(IDB_FIND_RELEASE_POS, TransitionDialog::OnFindReleasePosition)
  EVT_TEXT_ENTER(IDE_MIN_AREA, TransitionDialog::OnMinAreaEnter)
END_EVENT_TABLE()

// The single instance of this class.
TransitionDialog *TransitionDialog::instance = NULL;


// ****************************************************************************
/// Returns the single instance of this dialog.
// ****************************************************************************

TransitionDialog *TransitionDialog::getInstance(wxWindow *parent)
{
  if (instance == NULL)
  {
    instance = new TransitionDialog(parent);
  }

  return instance;
}


// ****************************************************************************
// ****************************************************************************

void TransitionDialog::updateWidgets()
{
  int pos;
  wxString st;

  // ****************************************************************
  // Information about the consonant.
  // ****************************************************************

  lstConsonant->SetValue(consonant);

  // ****************************************************************
  // Information about the vowel.
  // ****************************************************************

  if (useFixedVowel)
  {
    radFixedVowel->SetValue(true);
  }
  else
  {
    radInterpolatedVowel->SetValue(true);
  }

  lstVowel->SetValue(vowel);

  double fixedAlphaTongue;
  double fixedBetaTongue;
  double fixedAlphaLips;
  double fixedBetaLips;

  get2dVowelParams(vowel, fixedAlphaTongue, fixedBetaTongue, fixedAlphaLips, fixedBetaLips);
  st = wxString::Format("T(%2.2f %2.2f) L(%2.2f %2.2f)",
    fixedAlphaTongue, fixedBetaTongue, fixedAlphaLips, fixedBetaLips);
  labVowelParams->SetLabel(st);

  // Scrollbars for alpha and beta.

  if (alphaTongue < MIN_ALPHA)
  {
    alphaTongue = MIN_ALPHA;
  }
  if (alphaTongue > MAX_ALPHA)
  {
    alphaTongue = MAX_ALPHA;
  }
  pos = (int)(1000.0*(alphaTongue - MIN_ALPHA) / (MAX_ALPHA - MIN_ALPHA) + 0.5);
  scrAlpha->SetThumbPosition(pos);
  st = wxString::Format("%2.3f", alphaTongue);
  labAlpha->SetLabel(st);

  if (betaTongue < MIN_BETA)
  {
    betaTongue = MIN_BETA;
  }
  if (betaTongue > MAX_BETA)
  {
    betaTongue = MAX_BETA;
  }
  pos = (int)(1000.0*(betaTongue - MIN_BETA) / (MAX_BETA - MIN_BETA) + 0.5);
  scrBeta->SetThumbPosition(pos);
  st = wxString::Format("%2.3f", betaTongue);
  labBeta->SetLabel(st);

  // ****************************************************************
  // Transition pos.
  // ****************************************************************

  pos = (int)(1000.0*transitionPos + 0.5);
  scrTransitionPos->SetThumbPosition(pos);
  st = wxString::Format("%2.1f%%", transitionPos*100.0);
  labTransitionPos->SetLabel(st);

  // ****************************************************************
  // Minimum area.
  // ****************************************************************

  st = wxString::Format("%2.1f", minArea_mm2);
  txtMinArea->SetValue(st);
}


// ****************************************************************************
/// Set the windows that receive an update request message when a vocal tract
/// parameter was changed.
// ****************************************************************************

void TransitionDialog::setUpdateRequestReceiver(wxWindow *receiver)
{
  updateRequestReceiver = receiver;
}


// ****************************************************************************
/// Constructor.
// ****************************************************************************

TransitionDialog::TransitionDialog(wxWindow *parent) : 
  wxDialog(parent, wxID_ANY, wxString("CV transition dialog"), 
    wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE)
{
  this->Move(100, 100);

  // ****************************************************************
  // Init the variables first.
  // ****************************************************************

  data = Data::getInstance();
  updateRequestReceiver = NULL;

  consonant = "";
  vowel = "a";
  alphaTongue = 0.0;
  betaTongue = 0.0;
  transitionPos = 0.0;   // 0 <= x <= 1
  minArea_mm2 = 25.0;
  useFixedVowel = true;

  // ****************************************************************
  // Init and update the widgets.
  // ****************************************************************

  initWidgets();
  fillShapeLists();
  updateWidgets();
}


// ****************************************************************************
/// Init the widgets of this dialog.
// ****************************************************************************

void TransitionDialog::initWidgets()
{
  wxBoxSizer *topLevelSizer = new wxBoxSizer(wxVERTICAL);
  wxBoxSizer *lineSizer = NULL;
  wxStaticText *label = NULL;
  wxButton *button = NULL;

  // ****************************************************************
  // Choice of the consonant.
  // ****************************************************************

  topLevelSizer->AddSpacer(10);

  lineSizer = new wxBoxSizer(wxHORIZONTAL);

  label = new wxStaticText(this, wxID_ANY, "Consonant: ");
  lineSizer->Add(label, 0, wxALL | wxALIGN_CENTER, 3);

  lstConsonant = new wxComboBox(this, IDL_CONSONANT, "", wxDefaultPosition, 
    wxSize(150, -1), wxArrayString(), wxCB_DROPDOWN | wxCB_READONLY);
  lineSizer->Add(lstConsonant, 0, wxALL, 3);

  button = new wxButton(this, IDB_UPDATE_LISTS, "Update lists");
  lineSizer->Add(button, 0, wxALL, 3);

  topLevelSizer->Add(lineSizer);

  // ****************************************************************
  // Choice of the vowel.
  // ****************************************************************

  topLevelSizer->AddSpacer(10);

  lineSizer = new wxBoxSizer(wxHORIZONTAL);

  radFixedVowel = new wxRadioButton(this, IDR_FIXED_VOWEL, "Fixed vowel: ",
    wxDefaultPosition, wxDefaultSize, wxRB_GROUP);
  lineSizer->Add(radFixedVowel, 0, wxALL | wxALIGN_CENTER, 3);

  lstVowel = new wxComboBox(this, IDL_VOWEL, "", wxDefaultPosition, 
    wxSize(100, -1), wxArrayString(), wxCB_DROPDOWN);
//  | wxCB_READONLY);  // When read-only is set, the box is case-INsensitive !!
  lineSizer->Add(lstVowel, 0, wxALL, 3);

  labVowelParams = new wxStaticText(this, IDL_VOWEL_PARAMS, "for a=...  b=...", 
    wxDefaultPosition, wxSize(160, -1));
  lineSizer->Add(labVowelParams, 0, wxALL | wxALIGN_CENTER, 3);

  topLevelSizer->Add(lineSizer);

  // ****************************************************************

  lineSizer = new wxBoxSizer(wxHORIZONTAL);

  radInterpolatedVowel = new wxRadioButton(this, IDR_INTERPOLATED_VOWEL, "Interpolated vowel:");
  lineSizer->Add(radInterpolatedVowel, 0, wxALL, 3);

  topLevelSizer->Add(lineSizer);

  // ****************************************************************

  lineSizer = new wxBoxSizer(wxHORIZONTAL);
  lineSizer->Add(30, 0);    // An empty space

  label = new wxStaticText(this, wxID_ANY, "a: ");
  lineSizer->Add(label, 0, wxALL | wxALIGN_CENTER, 3);

  scrAlpha = new wxScrollBar(this, IDS_ALPHA, wxDefaultPosition, wxSize(180, -1), wxSB_HORIZONTAL);
  scrAlpha->SetScrollbar(0, 1, 1000, 1, true);
  lineSizer->Add(scrAlpha, 0, wxALL, 3);

  labAlpha = new wxStaticText(this, IDL_ALPHA, "xx", wxDefaultPosition, wxSize(30, -1));
  lineSizer->Add(labAlpha, 0, wxALL | wxALIGN_CENTER, 3);

  topLevelSizer->Add(lineSizer);

  // ****************************************************************

  lineSizer = new wxBoxSizer(wxHORIZONTAL);
  lineSizer->Add(30, 0);    // An empty space

  label = new wxStaticText(this, wxID_ANY, "b: ");
  lineSizer->Add(label, 0, wxALL | wxALIGN_CENTER, 3);

  scrBeta = new wxScrollBar(this, IDS_BETA, wxDefaultPosition, wxSize(180, -1), wxSB_HORIZONTAL);
  scrBeta->SetScrollbar(0, 1, 1000, 1, true);
  lineSizer->Add(scrBeta, 0, wxALL, 3);

  labBeta = new wxStaticText(this, IDL_BETA, "xx", wxDefaultPosition, wxSize(30, -1));
  lineSizer->Add(labBeta, 0, wxALL | wxALIGN_CENTER, 3);
  
  topLevelSizer->Add(lineSizer);

  // ****************************************************************
  // Scrollbar for the transition position.
  // ****************************************************************

  topLevelSizer->AddSpacer(15);

  lineSizer = new wxBoxSizer(wxHORIZONTAL);

  label = new wxStaticText(this, wxID_ANY, "Transition pos.: ");
  lineSizer->Add(label, 0, wxALL | wxALIGN_CENTER, 3);

  scrTransitionPos = new wxScrollBar(this, IDS_TRANSITION_POS, 
    wxDefaultPosition, wxDefaultSize, wxSB_HORIZONTAL);
  scrTransitionPos->SetScrollbar(0, 1, 1000, 1, true);
  lineSizer->Add(scrTransitionPos, 2, wxALL, 3);

  labTransitionPos = new wxStaticText(this, IDL_TRANSITION_POS, "xx", wxDefaultPosition, wxSize(40, -1));
  lineSizer->Add(labTransitionPos, 0, wxALL | wxALIGN_CENTER, 3);

  topLevelSizer->Add(lineSizer, 0, wxEXPAND);

  // ****************************************************************
  // Button and area to find release position.
  // ****************************************************************

  topLevelSizer->AddSpacer(10);

  lineSizer = new wxBoxSizer(wxHORIZONTAL);

  button = new wxButton(this, IDB_FIND_RELEASE_POS, "Find release position");
  lineSizer->Add(button, 0, wxALL, 3);

  label = new wxStaticText(this, wxID_ANY, " at area of ");
  lineSizer->Add(label, 0, wxALL | wxALIGN_CENTER, 3);

  txtMinArea = new wxTextCtrl(this, IDE_MIN_AREA, "", wxDefaultPosition, wxSize(40, -1), wxTE_PROCESS_ENTER);
//  txtMinArea->Connect(-1, wxEVT_KILL_FOCUS, wxFocusEventHandler(TransitionDialog::OnMinAreaLostFocus), NULL, this);
  lineSizer->Add(txtMinArea, 0, wxALL, 3);

  label = new wxStaticText(this, wxID_ANY, "mm^2");
  lineSizer->Add(label, 0, wxALL | wxALIGN_CENTER, 3);

  topLevelSizer->Add(lineSizer);

  // ****************************************************************
  // Other buttons.
  // ****************************************************************

  topLevelSizer->AddSpacer(10);

  lineSizer = new wxBoxSizer(wxHORIZONTAL);

  button = new wxButton(this, wxID_CANCEL, "Cancel");
  lineSizer->Add(button, 0, wxALL, 3);

  topLevelSizer->Add(lineSizer, 0, wxEXPAND);

  // ****************************************************************
  // Set the top-level-sizer for this window.
  // ****************************************************************

  topLevelSizer->AddSpacer(10);

  this->SetSizer(topLevelSizer);
  topLevelSizer->Fit(this);
}


// ****************************************************************************
// ****************************************************************************

bool TransitionDialog::getConsonantAndVowelShape(double *consonantParams, 
  double *vowelParams, double &aTongue, double &bTongue, double &aLips, double &bLips)
{
  int i;
  VocalTract *vt = data->vocalTract;

  // Init the reference variables.

  aTongue = 0.0;
  bTongue = 0.0;
  aLips = 0.0;
  bLips = 0.0;

  // ****************************************************************
  // Get the vowel vocal tract parameters.
  // ****************************************************************

  if (useFixedVowel)
  {
    int vowelIndex = vt->getShapeIndex( vowel.ToStdString() );
    if (vowelIndex == -1)
    {
      return false;
    }

    for (i=0; i < VocalTract::NUM_PARAMS; i++)
    {
      vowelParams[i] = vt->shapes[vowelIndex].param[i];
    }

    GesturalScore::mapToVowelSubspace(vt, vowelParams, aTongue, bTongue, aLips, bLips);
    GesturalScore::limitVowelSubspaceCoord(aTongue, bTongue, aLips, bLips);
  }
  else
  {
    // Interpolate the vowel between /a/, /i/ and /u/
    aTongue = alphaTongue;
    bTongue = betaTongue;
    aLips = alphaTongue;    // Lips are NOT interpolated independently.
    bLips = betaTongue;

    int vowelIndexA = vt->getShapeIndex("a");
    int vowelIndexI = vt->getShapeIndex("i");
    int vowelIndexU = vt->getShapeIndex("u");

    if ((vowelIndexA == -1) || (vowelIndexI == -1) || (vowelIndexU == -1))
    {
      return false;
    }

    for (i=0; i < VocalTract::NUM_PARAMS; i++)
    {
      vowelParams[i] = vt->shapes[vowelIndexA].param[i] +
        aTongue*(vt->shapes[vowelIndexI].param[i] - vt->shapes[vowelIndexA].param[i]) +
        bTongue*(vt->shapes[vowelIndexU].param[i] - vt->shapes[vowelIndexA].param[i]);
    }
  }

  // ****************************************************************
  // Get the consonant vocal tract parameters. Therefore, use the 
  // alpha and beta values obtained for the context vowel.
  // ****************************************************************

  bool consonantOk = GesturalScore::getContextDependentConsonant(vt, 
    (const char*)consonant.c_str(), aTongue, bTongue, aLips, bLips, consonantParams);

  if (consonantOk == false)
  {
    return false;      
  }

  // ****************************************************************

  return true;
}


// ****************************************************************************
/// Calculates the new vocal tract shape based on the settings in this dialog.
// ****************************************************************************

void TransitionDialog::calcVocalTract()
{
  int i;
  VocalTract *vt = data->vocalTract;

  double aTongue = 0.0;
  double bTongue = 0.0;
  double aLips = 0.0;
  double bLips = 0.0;
  double vowelParams[VocalTract::NUM_PARAMS];
  double consonantParams[VocalTract::NUM_PARAMS];

  // ****************************************************************

  if (getConsonantAndVowelShape(consonantParams, vowelParams, aTongue, bTongue, aLips, bLips) == false)
  {
    return;
  }

  // ****************************************************************
  // Interpolate between the (context-dependent) consonant shape and
  // the vowel shape.
  // ****************************************************************

  for (i=0; i < VocalTract::NUM_PARAMS; i++)
  {
    vt->param[i].x = (1.0-transitionPos)*consonantParams[i] + transitionPos*vowelParams[i];
  }

  vt->calculateAll();
}


// ****************************************************************************
/// Send a message to the parent window to update the vocal tract picture.
// ****************************************************************************

void TransitionDialog::updateVocalTract()
{
  if (updateRequestReceiver != NULL)
  {
    // Refresh the pictures of other windows.
    wxCommandEvent event(updateRequestEvent);
    event.SetInt(REFRESH_PICTURES); // UPDATE_VOCAL_TRACT);
    wxPostEvent(updateRequestReceiver, event);
    wxYield();
  }
}


// ****************************************************************************
/// Fills the lists with vocal tract shapes for vowels and consonants.
// ****************************************************************************

void TransitionDialog::fillShapeLists()
{
  wxString prevVowel = lstVowel->GetValue();
  wxString prevConsonant = lstConsonant->GetValue();

  // Refill the lists.

  int i;
  VocalTract *vt = data->vocalTract;
  wxString st;

  lstVowel->Clear();
  lstConsonant->Clear();

  for (i=0; i < (int)(vt->shapes.size()); i++)
  {
    if (vt->isVowelShapeName(vt->shapes[i].name))
    {
      lstVowel->Append(wxString( vt->shapes[i].name ));
    }
    else
    {
      // For each consonantal shape, three items with the endings
      // "(a)", "(i)", and "(u)" exist. Only the base name is
      // supposed to appear in the list (and this only once).

      st = wxString( vt->shapes[i].name );
      if ((st.EndsWith("(a)")) || (st.EndsWith("(i)")) || (st.EndsWith("(u)")))
      {
        // Remove the last three chars.
        st.RemoveLast();
        st.RemoveLast();
        st.RemoveLast();
      }

      if (lstConsonant->FindString(st, true) == wxNOT_FOUND)
      {
        lstConsonant->Append(st);
      }
    }
  }

  // Set the previous selected items.

  lstVowel->SetValue(prevVowel);
  lstConsonant->SetValue(prevConsonant);
}


// ****************************************************************************
/// Returns the parameters of the given vowel in the shape list in the 2D-
/// vowel subspace.
// ****************************************************************************

void TransitionDialog::get2dVowelParams(const wxString &vowel, 
  double &alphaTongue, double &betaTongue, double &alphaLips, double &betaLips)
{
  int i;
  VocalTract *vt = data->vocalTract;
  double vtParams[VocalTract::NUM_PARAMS];

  int shapeIndex = vt->getShapeIndex( vowel.ToStdString() );
  if (shapeIndex == -1)
  {
    alphaTongue = 0.0;
    betaTongue = 0.0;
    alphaLips = 0.0;
    betaLips = 0.0;
  }
  else
  {
    for (i=0; i < VocalTract::NUM_PARAMS; i++)
    {
      vtParams[i] = vt->shapes[shapeIndex].param[i];
    }
    data->gesturalScore->mapToVowelSubspace(vt, vtParams, alphaTongue, betaTongue, alphaLips, betaLips);
  }
}


// ****************************************************************************
// ****************************************************************************

void TransitionDialog::OnConsonantSelected(wxCommandEvent &event)
{
  consonant = lstConsonant->GetValue();

  updateWidgets();
  calcVocalTract();
  updateVocalTract();
}


// ****************************************************************************
// ****************************************************************************

void TransitionDialog::OnUpdateLists(wxCommandEvent &event)
{
  fillShapeLists();
  updateWidgets();
}


// ****************************************************************************
// ****************************************************************************

void TransitionDialog::OnVowelSelected(wxCommandEvent &event)
{
  vowel = lstVowel->GetValue();
  useFixedVowel = true;

  double fixedAlphaTongue;
  double fixedBetaTongue;
  double fixedAlphaLips;
  double fixedBetaLips;

  get2dVowelParams(vowel, fixedAlphaTongue, fixedBetaTongue, fixedAlphaLips, fixedBetaLips);
  
  alphaTongue = fixedAlphaTongue;
  betaTongue = fixedBetaTongue;
  
  // Restrict alphaTongue and betaTongue to [0..1]
  if (alphaTongue < MIN_ALPHA)
  {
    alphaTongue = MIN_ALPHA;
  }
  if (alphaTongue > MAX_ALPHA)
  {
    alphaTongue = MAX_ALPHA;
  }

  if (betaTongue < MIN_BETA)
  {
    betaTongue = MIN_BETA;
  }
  if (betaTongue > MAX_BETA)
  {
    betaTongue = MAX_BETA;
  }

  updateWidgets();
  calcVocalTract();
  updateVocalTract();
}


// ****************************************************************************
// ****************************************************************************

void TransitionDialog::OnUseFixedVowel(wxCommandEvent &event)
{
  useFixedVowel = true;

  updateWidgets();
  calcVocalTract();
  updateVocalTract();
}


// ****************************************************************************
// ****************************************************************************

void TransitionDialog::OnUseInterpolatedVowel(wxCommandEvent &event)
{
  useFixedVowel = false;

  updateWidgets();
  calcVocalTract();
  updateVocalTract();
}


// ****************************************************************************
// ****************************************************************************

void TransitionDialog::OnScrollbarChanged(wxScrollEvent &event)
{
  int pos = event.GetPosition();

  // Which scroll bar sent this message ?
  if (event.GetId() == IDS_ALPHA)
  {
    useFixedVowel = false;
    alphaTongue = MIN_ALPHA + (MAX_ALPHA - MIN_ALPHA)*(double)pos/1000.0;
    // alpha and beta together must always be <= 1.
    if (alphaTongue + betaTongue >= 1-0)
    {
      betaTongue = 1.0 - alphaTongue;
    }
  }
  else
  if (event.GetId() == IDS_BETA)
  {
    useFixedVowel = false;
    betaTongue = MIN_BETA + (MAX_BETA - MIN_BETA)*(double)pos/1000.0;
    // alpha and beta together must always be <= 1.
    if (alphaTongue + betaTongue >= 1-0)
    {
      alphaTongue = 1.0 - betaTongue;
    }
  }
  else
  if (event.GetId() == IDS_TRANSITION_POS)
  {
    transitionPos = (double)pos / 1000.0;
  }

  updateWidgets();
  calcVocalTract();
  updateVocalTract();
}


// ****************************************************************************
// ****************************************************************************

void TransitionDialog::OnFindReleasePosition(wxCommandEvent &event)
{
  double consonantParams[VocalTract::NUM_PARAMS];
  double vowelParams[VocalTract::NUM_PARAMS];
  double releaseParams[VocalTract::NUM_PARAMS];
  double releasePos;  // = [0 .. 1]

  // ****************************************************************
  // Get min. area value from the text box.
  // ****************************************************************

  double x = 0.0;
  wxString st = txtMinArea->GetValue();
  if ((st.ToDouble(&x)) && (x >= 0.0) && (x <= 1000.0))
  {
    minArea_mm2 = x;
  }
  updateWidgets();

  double releaseArea_cm2 = 0.01*minArea_mm2;

  // ****************************************************************
  // Obtain the parameters for the current consonant and vowel.
  // ****************************************************************

  double aTongue = 0.0;
  double bTongue = 0.0;
  double aLips = 0.0;
  double bLips = 0.0;

  if (getConsonantAndVowelShape(consonantParams, vowelParams, aTongue, bTongue, aLips, bLips) == false)
  {
    wxMessageBox("Invalid consonant or vowel shape!", "Error");
    return;
  }

  // ****************************************************************
  // Get the corresponding release shape.
  // ****************************************************************

  bool ok = data->getReleaseShape(data->vocalTract, consonantParams, 
    vowelParams, releaseParams, releasePos, releaseArea_cm2);

  if (ok)
  {
    transitionPos = releasePos;

    updateWidgets();
    calcVocalTract();
    updateVocalTract();
  }
  else
  {
    wxMessageBox("Error finding a release position", "Error");
  }
}


// ****************************************************************************
// ****************************************************************************

void TransitionDialog::OnMinAreaEnter(wxCommandEvent &event)
{
  double x = 0.0;
  wxString st = txtMinArea->GetValue();
  if ((st.ToDouble(&x)) && (x >= 0.0) && (x <= 1000.0))
  {
    minArea_mm2 = x;
  }

  updateWidgets();
}


// ****************************************************************************
