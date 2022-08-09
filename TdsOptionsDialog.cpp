// ****************************************************************************
// This file is part of VocalTractLab3D.
// Copyright (C) 2022, Peter Birkholz, Dresden, Germany
// www.vocaltractlab.de
// author: Peter Birkholz and Rémi Blandin
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

#include "TdsOptionsDialog.h"

// IDs of the controls

static const int IDC_TURBULENCE_LOSSES    = 5000;
static const int IDC_SOFT_WALLS           = 5001;
static const int IDC_NOISE_SOURCES        = 5002;
static const int IDC_RADIATION_FROM_SKIN  = 5003;
static const int IDC_PIRIFORM_FOSSA       = 5004;
static const int IDC_INNER_LENGTH_CORRECTIONS = 5005;
static const int IDC_TRANSVELAR_COUPLING = 5006;
static const int IDR_SOLVER_OPTIONS = 5012;
static const int IDB_ADAPT_FROM_FDS = 5013;


// The single instance of this class.

TdsOptionsDialog *TdsOptionsDialog::instance = NULL;

// ****************************************************************************
// The event table.
// ****************************************************************************

BEGIN_EVENT_TABLE(TdsOptionsDialog, wxDialog)
  EVT_CHECKBOX(IDC_TURBULENCE_LOSSES, TdsOptionsDialog::OnTurbulenceLosses)
  EVT_CHECKBOX(IDC_SOFT_WALLS, TdsOptionsDialog::OnSoftWalls)
  EVT_CHECKBOX(IDC_NOISE_SOURCES, TdsOptionsDialog::OnNoiseSources)
  EVT_CHECKBOX(IDC_RADIATION_FROM_SKIN, TdsOptionsDialog::OnRadiationFromSkin)
  EVT_CHECKBOX(IDC_PIRIFORM_FOSSA, TdsOptionsDialog::OnPiriformFossa)
  EVT_CHECKBOX(IDC_INNER_LENGTH_CORRECTIONS, TdsOptionsDialog::OnInnerLengthCorrections)
  EVT_CHECKBOX(IDC_TRANSVELAR_COUPLING, TdsOptionsDialog::OnTransvelarCoupling)
  EVT_RADIOBOX(IDR_SOLVER_OPTIONS, TdsOptionsDialog::OnSolverOptions)
  EVT_BUTTON(IDB_ADAPT_FROM_FDS, TdsOptionsDialog::OnAdaptFromFds)
END_EVENT_TABLE()



// ****************************************************************************
/// Returns the single instance of this dialog.
// ****************************************************************************

TdsOptionsDialog *TdsOptionsDialog::getInstance()
{
  if (instance == NULL)
  {
    instance = new TdsOptionsDialog(NULL);
  }

  return instance;
}


// ****************************************************************************
/// Private constructor.
// ****************************************************************************

TdsOptionsDialog::TdsOptionsDialog(wxWindow *parent) : 
  wxDialog(parent, wxID_ANY, wxString("Options for time-domain synthesis"), 
           wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE)
{
//  this->Move(100, 50);

  // ****************************************************************
  // Init the variables first.
  // ****************************************************************

  model = Data::getInstance()->tdsModel;

  // ****************************************************************
  // Init and update the widgets.
  // ****************************************************************

  initWidgets();
  updateWidgets();
}


// ****************************************************************************
/// Init the widgets.
// ****************************************************************************

void TdsOptionsDialog::initWidgets()
{
  wxBoxSizer *baseSizer = new wxBoxSizer(wxVERTICAL);
  wxFlexGridSizer *gridSizer = new wxFlexGridSizer(4, 2, 0, 0);

  // Create an invisible dummy button with the ID wxID_CANCEL, so that
  // the user can close the dialog with ESC.
  wxButton *button = new wxButton(this, wxID_CANCEL, "", wxPoint(0,0), wxSize(0,0));

  // Add the check boxes for the options

  chkTurbulenceLosses = new wxCheckBox(this, IDC_TURBULENCE_LOSSES, "Fluid dynamic losses");
  gridSizer->Add(chkTurbulenceLosses, 0, wxALL, 5);

  chkSoftWalls = new wxCheckBox(this, IDC_SOFT_WALLS, "Soft walls");
  gridSizer->Add(chkSoftWalls, 0, wxALL, 5);

  chkNoiseSources = new wxCheckBox(this, IDC_NOISE_SOURCES, "Noise sources");
  gridSizer->Add(chkNoiseSources, 0, wxALL, 5);

  chkRadiationFromSkin = new wxCheckBox(this, IDC_RADIATION_FROM_SKIN, "Sound radiation from skin");
  gridSizer->Add(chkRadiationFromSkin, 0, wxALL, 5);

  chkPiriformFossa = new wxCheckBox(this, IDC_PIRIFORM_FOSSA, "Piriform fossa");
  gridSizer->Add(chkPiriformFossa, 0, wxALL, 5);

  chkInnerLengthCorrections = new wxCheckBox(this, IDC_INNER_LENGTH_CORRECTIONS, "Inner length corrections");
  gridSizer->Add(chkInnerLengthCorrections, 0, wxALL, 5);

  chkTransvelarCoupling = new wxCheckBox(this, IDC_TRANSVELAR_COUPLING, "Transvelar coupling");
  gridSizer->Add(chkTransvelarCoupling, 0, wxALL, 5);

  baseSizer->Add(gridSizer);

  // Add the choice boxes for the numeric solver.

  baseSizer->AddSpacer(8);

  const int NUM_SOLVER_CHOICES = TdsModel::NUM_SOLVER_TYPES;
  const wxString SOLVER_CHOICES[NUM_SOLVER_CHOICES] =
  {
    "Gauss-Seidel SOR",
    "Cholesky factorization"
  };

  radSolverOptions = new wxRadioBox(this, IDR_SOLVER_OPTIONS, "Numeric solver options",
    wxDefaultPosition, wxDefaultSize, NUM_SOLVER_CHOICES, SOLVER_CHOICES, NUM_SOLVER_CHOICES, wxRA_SPECIFY_ROWS);
  baseSizer->Add(radSolverOptions, 0, wxALL | wxGROW, 3);

  // Add the button

  btnAdaptFromFds = new wxButton(this, IDB_ADAPT_FROM_FDS, "Adapt data from FDS");
  baseSizer->Add(btnAdaptFromFds, 0, wxALL, 5);

  // Set the sizer for this dialog

  this->SetSizer(baseSizer);
  baseSizer->Fit(this);
  baseSizer->SetSizeHints(this);
}


// ****************************************************************************
/// Update the widgets.
// ****************************************************************************

void TdsOptionsDialog::updateWidgets()
{
  chkTurbulenceLosses->SetValue( model->options.turbulenceLosses );
  chkSoftWalls->SetValue( model->options.softWalls );
  chkNoiseSources->SetValue( model->options.generateNoiseSources );
  chkRadiationFromSkin->SetValue( model->options.radiationFromSkin );
  chkPiriformFossa->SetValue( model->options.piriformFossa );
  chkInnerLengthCorrections->SetValue( model->options.innerLengthCorrections );
  chkTransvelarCoupling->SetValue(model->options.transvelarCoupling);

  radSolverOptions->SetSelection(model->options.solverType);
}


// ****************************************************************************
// ****************************************************************************

void TdsOptionsDialog::OnTurbulenceLosses(wxCommandEvent &event)
{
  model->options.turbulenceLosses = !model->options.turbulenceLosses;
  updateWidgets();
}


// ****************************************************************************
// ****************************************************************************

void TdsOptionsDialog::OnSoftWalls(wxCommandEvent &event)
{
  model->options.softWalls = !model->options.softWalls;
  updateWidgets();
}


// ****************************************************************************
// ****************************************************************************

void TdsOptionsDialog::OnNoiseSources(wxCommandEvent &event)
{
  model->options.generateNoiseSources = !model->options.generateNoiseSources;
  updateWidgets();
}


// ****************************************************************************
// ****************************************************************************

void TdsOptionsDialog::OnRadiationFromSkin(wxCommandEvent &event)
{
  model->options.radiationFromSkin = !model->options.radiationFromSkin;
  updateWidgets();
}


// ****************************************************************************
// ****************************************************************************

void TdsOptionsDialog::OnPiriformFossa(wxCommandEvent &event)
{
  model->options.piriformFossa = !model->options.piriformFossa;
  updateWidgets();
}


// ****************************************************************************
// ****************************************************************************

void TdsOptionsDialog::OnInnerLengthCorrections(wxCommandEvent &event)
{
  model->options.innerLengthCorrections = !model->options.innerLengthCorrections;
  updateWidgets();
}


// ****************************************************************************
// ****************************************************************************

void TdsOptionsDialog::OnTransvelarCoupling(wxCommandEvent &event)
{
  model->options.transvelarCoupling = !model->options.transvelarCoupling;
  updateWidgets();
}


// ****************************************************************************
// ****************************************************************************

void TdsOptionsDialog::OnSolverOptions(wxCommandEvent &event)
{
  int selection = event.GetSelection();
  model->options.solverType = (TdsModel::SolverType)selection;
  updateWidgets();
}


// ****************************************************************************
// ****************************************************************************

void TdsOptionsDialog::OnAdaptFromFds(wxCommandEvent &event)
{
  TlModel *tlModel = Data::getInstance()->tlModel;

  model->options.turbulenceLosses = tlModel->options.staticPressureDrops;
  model->options.softWalls        = tlModel->options.softWalls;
  model->options.piriformFossa    = tlModel->options.piriformFossa;
  model->options.innerLengthCorrections = tlModel->options.innerLengthCorrections;

  updateWidgets();
}

// ****************************************************************************
