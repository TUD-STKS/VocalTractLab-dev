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

#include "FormantOptimizationDialog.h"
#include <stdio.h>
#include <sstream>
#include <wx/statline.h>
#include <wx/tokenzr.h>

// ****************************************************************************
/// IDs.
// ****************************************************************************

static const int IDB_SET_DEFAULT_VALUES     = 6000;
static const int IDB_OPTIMIZE_VOWEL         = 6001;
static const int IDB_OPTIMIZE_CONSONANT     = 6002;


// ****************************************************************************
// The event table.
// ****************************************************************************

BEGIN_EVENT_TABLE(FormantOptimizationDialog, wxDialog)
  EVT_BUTTON(IDB_SET_DEFAULT_VALUES, FormantOptimizationDialog::OnSetDefaultValues)
  EVT_BUTTON(IDB_OPTIMIZE_VOWEL, FormantOptimizationDialog::OnOptimizeVowel)
  EVT_BUTTON(IDB_OPTIMIZE_CONSONANT, FormantOptimizationDialog::OnOptimizeConsonant)
  EVT_SHOW(FormantOptimizationDialog::OnShow)
END_EVENT_TABLE()


// ****************************************************************************
/// Constructor.
// ****************************************************************************

FormantOptimizationDialog::FormantOptimizationDialog(wxWindow *parent, VocalTract *vocalTract) :
  wxDialog(parent, wxID_ANY, wxString("Formant optimization"), 
  wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE)
{
  this->vocalTract = vocalTract;

  resetValues();
  initWidgets();
  updateWidgets();
}


// ****************************************************************************
/// Init all widgets.
// ****************************************************************************

void FormantOptimizationDialog::initWidgets()
{
  int i;
  wxButton *button = NULL;
  wxStaticText *label = NULL;
  wxBoxSizer *topLevelSizer = new wxBoxSizer(wxVERTICAL);
  wxBoxSizer *horizSizer = NULL;

  // Create an invisible dummy button with the ID wxID_CANCEL, so that
  // the user can close the dialog with ESC.
  button = new wxButton(this, wxID_CANCEL, "", wxPoint(0,0), wxSize(0,0));
  
  // ****************************************************************
  // Create the widgets.
  // ****************************************************************

  // ****************************************************************
  // Text field for entering the formant frequencies.
  // ****************************************************************

  topLevelSizer->AddSpacer(10);
  label = new wxStaticText(this, wxID_ANY, "First two or three formant frequencies (separated by spaces).\n"
  "Type 'x' or '0' for formant frequencies that do not matter.");
  topLevelSizer->Add(label, 0, wxALL, 3);
  topLevelSizer->AddSpacer(10);

  txtFormants = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, this->FromDIP(wxSize(300, -1)));
  topLevelSizer->Add(txtFormants, 0, wxALL, 3);

  // ****************************************************************
  // Check boxes for fixing certain parameters.
  // ****************************************************************

  topLevelSizer->AddSpacer(10);
  label = new wxStaticText(this, wxID_ANY, "Please check the parameters that are fixed:");
  topLevelSizer->Add(label, 0, wxALL, 3);
  topLevelSizer->AddSpacer(10);

  wxFlexGridSizer *gridSizer = new wxFlexGridSizer(6, 5, 5);  // num cols
  for (i=0; i < VocalTract::NUM_PARAMS; i++)
  {
    chkParamFixed[i] = new wxCheckBox(this, wxID_ANY, wxString(vocalTract->param[i].abbr));
    gridSizer->Add(chkParamFixed[i]); //, wxALL, 3);
  }
  topLevelSizer->Add(gridSizer, 0, wxALL, 3);
  topLevelSizer->AddSpacer(10);

  // ****************************************************************
  // Maximal allowed displacement of the contours.
  // ****************************************************************

  horizSizer = new wxBoxSizer(wxHORIZONTAL);
  
  label = new wxStaticText(this, wxID_ANY, "Max. contour displacement:");
  horizSizer->Add(label, 0, wxALL | wxALIGN_CENTER, 3);

  txtMaxDisplacement = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, this->FromDIP(wxSize(50, -1)));
  horizSizer->Add(txtMaxDisplacement, 0, wxALL, 3);

  label = new wxStaticText(this, wxID_ANY, "mm");
  horizSizer->Add(label, 0, wxALL | wxALIGN_CENTER, 3);

  topLevelSizer->Add(horizSizer, 0, wxALL, 3);

  // ****************************************************************
  // Minimal cross-sectional area.
  // ****************************************************************

  horizSizer = new wxBoxSizer(wxHORIZONTAL);
  
  label = new wxStaticText(this, wxID_ANY, "Min. cross-sectional area:");
  horizSizer->Add(label, 0, wxALL | wxALIGN_CENTER, 3);

  txtMinArea = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, this->FromDIP(wxSize(50, -1)));
  horizSizer->Add(txtMinArea, 0, wxALL, 3);

  label = new wxStaticText(this, wxID_ANY, "mm^2");
  horizSizer->Add(label, 0, wxALL | wxALIGN_CENTER, 3);

  topLevelSizer->Add(horizSizer, 0, wxALL, 3);

  // ****************************************************************
  // Button to optimize a vowel.
  // ****************************************************************

  button = new wxButton(this, IDB_OPTIMIZE_VOWEL, "Optimize vowel");
  topLevelSizer->Add(button, 0, wxALL | wxEXPAND, 3);

  // A horizontal line

  wxStaticLine *horzLine = new wxStaticLine(this, wxID_ANY, wxDefaultPosition, 
    wxDefaultSize, wxLI_HORIZONTAL);

  topLevelSizer->Add(horzLine, 0, wxGROW | wxALL, 4);


  // ****************************************************************
  // Release area (only for consonant optimization).
  // ****************************************************************

  horizSizer = new wxBoxSizer(wxHORIZONTAL);
  
  label = new wxStaticText(this, wxID_ANY, "Release area:");
  horizSizer->Add(label, 0, wxALL | wxALIGN_CENTER, 3);

  txtReleaseArea = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, this->FromDIP(wxSize(50, -1)));
  horizSizer->Add(txtReleaseArea, 0, wxALL, 3);

  label = new wxStaticText(this, wxID_ANY, "mm^2 (for consonants)");
  horizSizer->Add(label, 0, wxALL | wxALIGN_CENTER, 3);

  topLevelSizer->Add(horizSizer, 0, wxALL, 3);

  // ****************************************************************
  // Context vowel (only for consonant optimization).
  // ****************************************************************

  horizSizer = new wxBoxSizer(wxHORIZONTAL);
  
  label = new wxStaticText(this, wxID_ANY, "Context vowel:");
  horizSizer->Add(label, 0, wxALL | wxALIGN_CENTER, 3);

  txtContextVowel = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, this->FromDIP(wxSize(50, -1)));
  horizSizer->Add(txtContextVowel, 0, wxALL, 3);

  label = new wxStaticText(this, wxID_ANY, "(for consonants)");
  horizSizer->Add(label, 0, wxALL | wxALIGN_CENTER, 3);

  topLevelSizer->Add(horizSizer, 0, wxALL, 3);

  // ****************************************************************
  // Button to optimize a consonant.
  // ****************************************************************

  button = new wxButton(this, IDB_OPTIMIZE_CONSONANT, "Optimize consonant");
  topLevelSizer->Add(button, 0, wxALL | wxEXPAND, 3);

  topLevelSizer->AddSpacer(5);

  // ****************************************************************
  // Row of buttons at the bottom.
  // ****************************************************************

  horizSizer = new wxBoxSizer(wxHORIZONTAL);

  button = new wxButton(this, wxID_CANCEL, "Cancel");
  horizSizer->Add(button, 0, wxALL, 3);

  // Add a horizontal space.
  horizSizer->Add(new wxStaticText(this, wxID_ANY, ""), 2);

  button = new wxButton(this, IDB_SET_DEFAULT_VALUES, "Set default values");
  horizSizer->Add(button, 0, wxALL, 3);

  topLevelSizer->Add(horizSizer, 0, wxALL, 3);

  // ****************************************************************

  this->SetSizer(topLevelSizer);
  topLevelSizer->Fit(this);
  topLevelSizer->SetSizeHints(this);
}


// ****************************************************************************
// ****************************************************************************

void FormantOptimizationDialog::updateWidgets()
{
  wxString st;

  // ****************************************************************
  // The two or three formant frequencies.
  // ****************************************************************

  if (F3 != 0.0)
  {
    st = wxString::Format("%d %d %d", (int)F1, (int)F2, (int)F3);
  }
  else
  {
    st = wxString::Format("%d %d", (int)F1, (int)F2);
  }
  txtFormants->SetValue(st);

  // ****************************************************************
  // The fixed parameters.
  // ****************************************************************

  int i;
  for (i=0; i < VocalTract::NUM_PARAMS; i++)
  {
    chkParamFixed[i]->SetValue(paramFixed[i]);
  }

  // ****************************************************************
  // Minimum area, maximum displacement, context vowel.
  // ****************************************************************

  st = wxString::Format("%2.1f", minArea_mm2);
  txtMinArea->SetValue(st);

  st = wxString::Format("%2.1f", releaseArea_mm2);
  txtReleaseArea->SetValue(st);

  st = wxString::Format("%2.1f", maxDisplacement_mm);
  txtMaxDisplacement->SetValue(st);

  txtContextVowel->SetValue(contextVowel);
}


// ****************************************************************************
/// Set default values for all parameters.
// ****************************************************************************

void FormantOptimizationDialog::resetValues()
{
  int i;

  F1 = 500.0;
  F2 = 1500.0;
  F3 = 2500.0;

  for (i=0; i < VocalTract::NUM_PARAMS; i++)
  {
    paramFixed[i] = false;
  }

  minArea_mm2 = 25.0;
  releaseArea_mm2 = 25.0;
  maxDisplacement_mm = 3.0;
  contextVowel = "a";
  optimizeVowel = true;
}


// ****************************************************************************
// ****************************************************************************

bool FormantOptimizationDialog::setParamsFromControls()
{
  int i;
  double x;
  bool ok = true;

  // ****************************************************************
  // Set the parameters from the controls.
  // ****************************************************************

  const double MIN_FREQ = 50.0;
  const double MAX_FREQ = 5000.0;

  double newF1 = 0.0;
  double newF2 = 0.0;
  double newF3 = 0.0;

  wxString st = txtFormants->GetValue();
  wxStringTokenizer tokenizer(st, " ");

  bool ok1 = tokenizer.GetNextToken().ToDouble(&newF1);
  bool ok2 = tokenizer.GetNextToken().ToDouble(&newF2);
  bool ok3 = tokenizer.GetNextToken().ToDouble(&newF3);

  if (ok1)
  {
    if ((newF1 < MIN_FREQ) || (newF1 > MAX_FREQ))
    {
      newF1 = 0.0;
      ok1 = false;
    }
  }
  else
  {
    newF1 = 0.0;
  }

  if (ok2)
  {
    if ((newF2 < MIN_FREQ) || (newF2 > MAX_FREQ))
    {
      newF2 = 0.0;
      ok2 = false;
    }
  }
  else
  {
    newF2 = 0.0;
  }

  if (ok3)
  {
    if ((newF3 < MIN_FREQ) || (newF3 > MAX_FREQ))
    {
      newF3 = 0.0;
      ok3 = false;
    }
  }
  else
  {
    newF3 = 0.0;
  }

  // At least one formant frequency must be valid.

  if ((ok1) || (ok2) || (ok3))
  {
    F1 = newF1;
    F2 = newF2;
    F3 = newF3;
  }
  else
  {
    wxMessageBox("All entered formant frequencies are invalid numbers.", "Error");
    ok = false;
  }

  // Get the fixed parameters.

  for (i=0; i < VocalTract::NUM_PARAMS; i++)
  {
    paramFixed[i] = chkParamFixed[i]->GetValue();
  }

  // Get the minimal allowed cross-sectional area.

  st = txtMinArea->GetValue();
  if ((st.ToDouble(&x)) && (x >= 0.0) && (x <= 1000.0))
  {
    minArea_mm2 = x;
  }
  else
  {
    wxMessageBox("The minimal allowed area value is invalid!", "Error");
    ok = false;
  }

  // Get the release area.

  st = txtReleaseArea->GetValue();
  if ((st.ToDouble(&x)) && (x >= 0.0) && (x <= 1000.0))
  {
    releaseArea_mm2 = x;
  }
  else
  {
    wxMessageBox("The release area value is invalid!", "Error");
    ok = false;
  }

  // Get the maximal allowed contour displacement.

  st = txtMaxDisplacement->GetValue();
  if ((st.ToDouble(&x)) && (x >= 0.000001) && (x <= 10.0))
  {
    maxDisplacement_mm = x;
  }
  else
  {
    wxMessageBox("The maximal allowed contour displacement value is invalid!", "Error");
    ok = false;
  }

  // Get the context vowel

  contextVowel = txtContextVowel->GetValue();

  // ****************************************************************

  return ok;
}


// ****************************************************************************
// ****************************************************************************

void FormantOptimizationDialog::OnSetDefaultValues(wxCommandEvent &event)
{
  resetValues();
  updateWidgets();
}


// ****************************************************************************
// ****************************************************************************

void FormantOptimizationDialog::OnOptimizeVowel(wxCommandEvent &event)
{
  bool ok = setParamsFromControls();

  if ((IsModal()) && (ok))
  {
    optimizeVowel = true;
    EndModal(wxID_OK);
  }
}


// ****************************************************************************
// ****************************************************************************

void FormantOptimizationDialog::OnOptimizeConsonant(wxCommandEvent &event)
{
  bool ok = setParamsFromControls();

  if ((IsModal()) && (ok))
  {
    optimizeVowel = false;
    EndModal(wxID_OK);
  }
}


// ****************************************************************************
// ****************************************************************************

void FormantOptimizationDialog::OnShow(wxShowEvent &event)
{
  updateWidgets();
  event.Skip();
}

// ****************************************************************************

