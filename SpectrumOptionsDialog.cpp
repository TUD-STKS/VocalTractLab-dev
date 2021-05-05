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

#include "SpectrumOptionsDialog.h"

// IDs of the controls

static const int IDR_SPECTRUM_TYPE  = 5000;
static const int IDC_SHOW_HARMONICS = 5001;

// The single instance of this class.

SpectrumOptionsDialog *SpectrumOptionsDialog::instance = NULL;
SpectrumPicture *SpectrumOptionsDialog::spectrumPicture = NULL;

// ****************************************************************************
// The event table.
// ****************************************************************************

BEGIN_EVENT_TABLE(SpectrumOptionsDialog, wxDialog)
  EVT_RADIOBOX(IDR_SPECTRUM_TYPE, SpectrumOptionsDialog::OnSpectrumType)
  EVT_CHECKBOX(IDC_SHOW_HARMONICS, SpectrumOptionsDialog::OnShowHarmonics)
END_EVENT_TABLE()


// ****************************************************************************
/// Returns the single instance of this dialog.
// ****************************************************************************

SpectrumOptionsDialog *SpectrumOptionsDialog::getInstance(SpectrumPicture *spectrumPicture)
{
  if (instance == NULL)
  {
    instance = new SpectrumOptionsDialog(NULL);
  }

  SpectrumOptionsDialog::spectrumPicture = spectrumPicture;
  return instance;
}


// ****************************************************************************
/// Private constructor.
// ****************************************************************************

SpectrumOptionsDialog::SpectrumOptionsDialog(wxWindow *parent) : 
  wxDialog(parent, wxID_ANY, wxString("Options for the primary spectrum"), 
           wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE)
{
  // ****************************************************************
  // Init the variables first.
  // ****************************************************************

  data = Data::getInstance();

  // ****************************************************************
  // Init and update the widgets.
  // ****************************************************************

  initWidgets();
  updateWidgets();
}


// ****************************************************************************
/// Init the widgets.
// ****************************************************************************

void SpectrumOptionsDialog::initWidgets()
{
  wxBoxSizer *topLevelSizer = new wxBoxSizer(wxVERTICAL);
  wxBoxSizer *topSizer = new wxBoxSizer(wxVERTICAL);
  wxStaticBoxSizer *bottomSizer = new wxStaticBoxSizer(wxVERTICAL, this, "Options");


  // Create an invisible dummy button with the ID wxID_CANCEL, so that
  // the user can close the dialog with ESC.
  wxButton *button = new wxButton(this, wxID_CANCEL, "", wxPoint(0,0), wxSize(0,0));


  // ****************************************************************
  // Radio buttons to select the spectrum type.
  // ****************************************************************

  wxString TYPE_STRINGS[SpectrumPicture::NUM_SPECTRUM_TYPES] =
  {
    "Transfer function U/U",
    "Transfer function P/U",
    "Nose transfer function U/U",
    "Input impedance of the vocal tract",
    "Input impedance of the subglottal system",
    "Glottal volume velocity * radiation impedance",
    "Radiated pressure (glottal voice source)",
    "Radiated pressure (supraglottal noise source)",
    "Radiated pressure (pole-zero)",
    "Glottal volume velocity"
  };

  radSpectrumType = new wxRadioBox(this, IDR_SPECTRUM_TYPE, "Spectrum type", wxDefaultPosition,
    wxDefaultSize, SpectrumPicture::NUM_SPECTRUM_TYPES, TYPE_STRINGS, 1);

  topSizer->Add(radSpectrumType, 0, wxALL, 0);

  // ****************************************************************
  // Bottom sizer with additional options.
  // ****************************************************************

  chkShowHarmonics = new wxCheckBox(this, IDC_SHOW_HARMONICS, "Show transfer function harmonics");

  bottomSizer->Add(chkShowHarmonics, 0, wxGROW | wxALL, 5);

  // ****************************************************************
  // Init the top-level sizer and set it for this dialog
  // ****************************************************************

  topLevelSizer->Add(topSizer, 0, wxGROW | wxALL, 5);
  topLevelSizer->Add(bottomSizer, 0, wxGROW | wxALL, 5);

  this->SetSizer(topLevelSizer);
  topLevelSizer->Fit(this);
  topLevelSizer->SetSizeHints(this);
}

// ****************************************************************************
/// Update the widgets.
// ****************************************************************************

void SpectrumOptionsDialog::updateWidgets()
{
  if (spectrumPicture != NULL)
  {
    radSpectrumType->SetSelection((int)spectrumPicture->modelSpectrumType);
    chkShowHarmonics->SetValue(spectrumPicture->showVttfHarmonics);
  }
}

// ****************************************************************************
// ****************************************************************************

void SpectrumOptionsDialog::OnSpectrumType(wxCommandEvent &event)
{
  if (spectrumPicture != NULL)
  {
    int i = event.GetInt();
    spectrumPicture->modelSpectrumType = (SpectrumPicture::ModelSpectrumType)i;

    updateWidgets();
    spectrumPicture->Refresh();
  }
}

// ****************************************************************************
// ****************************************************************************

void SpectrumOptionsDialog::OnShowHarmonics(wxCommandEvent &event)
{
  if (spectrumPicture != NULL)
  {
    spectrumPicture->showVttfHarmonics = !spectrumPicture->showVttfHarmonics;

    updateWidgets();
    spectrumPicture->Refresh();
  }
}

// ****************************************************************************
