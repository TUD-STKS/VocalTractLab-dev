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

#include "FdsOptionsDialog.h"
#include "VocalTractLabBackend/Constants.h"

// IDs of the controls

static const int IDR_RADIATION_OPTIONS      = 6000;
static const int IDC_BOUNDARY_LAYER         = 6001;
static const int IDC_HEAT_CONDUCTION        = 6002;
static const int IDC_SOFT_WALLS             = 6003;
static const int IDC_HAGEN_RESISTANCE       = 6004;
static const int IDC_PARANASAL_SINUSES      = 6005;
static const int IDC_PIRIFORM_FOSSA         = 6006;
static const int IDC_STATIC_PRESSURE_DROPS  = 6007;
static const int IDC_LUMPED_ELEMENTS        = 6008;
static const int IDC_INNER_LENGTH_CORRECTIONS = 6009;
static const int IDB_CONSTANTS              = 6010;

// The single instance of this class.

FdsOptionsDialog *FdsOptionsDialog::instance = NULL;

// ****************************************************************************
// The event table.
// ****************************************************************************

BEGIN_EVENT_TABLE(FdsOptionsDialog, wxDialog)
  EVT_RADIOBOX(IDR_RADIATION_OPTIONS, FdsOptionsDialog::OnRadiationOptions)
  EVT_CHECKBOX(IDC_BOUNDARY_LAYER, FdsOptionsDialog::OnBoundaryLayer)
  EVT_CHECKBOX(IDC_HEAT_CONDUCTION, FdsOptionsDialog::OnHeatConduction)
  EVT_CHECKBOX(IDC_SOFT_WALLS, FdsOptionsDialog::OnSoftWalls)
  EVT_CHECKBOX(IDC_HAGEN_RESISTANCE, FdsOptionsDialog::OnHagenResistance)
  EVT_CHECKBOX(IDC_PARANASAL_SINUSES, FdsOptionsDialog::OnParanasalSinuses)
  EVT_CHECKBOX(IDC_PIRIFORM_FOSSA, FdsOptionsDialog::OnPiriformFossa)
  EVT_CHECKBOX(IDC_STATIC_PRESSURE_DROPS, FdsOptionsDialog::OnStaticPressureDrops)
  EVT_CHECKBOX(IDC_LUMPED_ELEMENTS, FdsOptionsDialog::OnLumpedElements)
  EVT_CHECKBOX(IDC_INNER_LENGTH_CORRECTIONS, FdsOptionsDialog::OnInnerLengthCorrections)
  EVT_BUTTON(IDB_CONSTANTS, FdsOptionsDialog::OnConstants)
END_EVENT_TABLE()


// ****************************************************************************
/// Returns the single instance of this dialog.
// ****************************************************************************

FdsOptionsDialog *FdsOptionsDialog::getInstance()
{
  if (instance == NULL)
  {
    instance = new FdsOptionsDialog(NULL);
  }

  return instance;
}


// ****************************************************************************
/// Private constructor.
// ****************************************************************************

FdsOptionsDialog::FdsOptionsDialog(wxWindow *parent) : 
  wxDialog(parent, wxID_ANY, wxString("Options for frequency-domain synthesis"), 
           wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE)
{
//  this->Move(100, 50);

  // ****************************************************************
  // Init the variables first.
  // ****************************************************************

  model = Data::getInstance()->tlModel;

  // ****************************************************************
  // Init and update the widgets.
  // ****************************************************************

  initWidgets();
  updateWidgets(false);
}


// ****************************************************************************
/// Init the widgets.
// ****************************************************************************

void FdsOptionsDialog::initWidgets()
{
  wxFlexGridSizer *topLevelSizer = new wxFlexGridSizer(2, 2, 0, 0);
  wxBoxSizer *topLeftSizer = new wxBoxSizer(wxHORIZONTAL);
  wxStaticBoxSizer *topRightSizer = new wxStaticBoxSizer(wxVERTICAL, this, "Energy losses");
  wxStaticBoxSizer *bottomLeftSizer = new wxStaticBoxSizer(wxVERTICAL, this, "Additional options");
  wxBoxSizer *bottomRightSizer = new wxBoxSizer(wxVERTICAL);
  wxButton *button = NULL;


  // Create an invisible dummy button with the ID wxID_CANCEL, so that
  // the user can close the dialog with ESC.
  button = new wxButton(this, wxID_CANCEL, "", wxPoint(0,0), wxSize(0,0));


  // ****************************************************************
  // The radio box with the radiation impedance options
  // ****************************************************************

  wxString RADIATION_OPTIONS[TlModel::NUM_RADIATION_OPTIONS] = 
  {
    "= 0 (lossless)",
    "Piston in spere (Wakita, Fant)",
    "Piston in wall",
    "Parallel circuit or R and L"
  };

  radRadiationOptions = new wxRadioBox(this, IDR_RADIATION_OPTIONS, "Radiation impedance",
    wxDefaultPosition, wxDefaultSize, TlModel::NUM_RADIATION_OPTIONS, RADIATION_OPTIONS, 1);

  topLeftSizer->Add(radRadiationOptions, 1, wxGROW, 0);

  // ****************************************************************
  // Check boxes for the energy losses
  // ****************************************************************

  chkBoundaryLayer = new wxCheckBox(this, IDC_BOUNDARY_LAYER, "Boundary layer resistance");
  topRightSizer->Add(chkBoundaryLayer, 0, wxGROW | wxALL, 5);

  chkHeatConduction = new wxCheckBox(this, IDC_HEAT_CONDUCTION, "Heat conduction losses");
  topRightSizer->Add(chkHeatConduction, 0, wxGROW | wxALL, 5);

  chkSoftWalls = new wxCheckBox(this, IDC_SOFT_WALLS, "Soft walls");
  topRightSizer->Add(chkSoftWalls, 0, wxGROW | wxALL, 5);

  chkHagenResistance = new wxCheckBox(this, IDC_HAGEN_RESISTANCE, "Hagen-Poiseuille resistance");
  topRightSizer->Add(chkHagenResistance, 0, wxGROW | wxALL, 5);


  // ****************************************************************
  // Additional options
  // ****************************************************************

  chkParanasalSinuses = new wxCheckBox(this, IDC_PARANASAL_SINUSES, "Paranasal sinuses");
  bottomLeftSizer->Add(chkParanasalSinuses, 0, wxGROW | wxALL, 5);

  chkPiriformFossa = new wxCheckBox(this, IDC_PIRIFORM_FOSSA, "Piriform fossa");
  bottomLeftSizer->Add(chkPiriformFossa, 0, wxGROW | wxALL, 5);

  chkStaticPressureDrops = new wxCheckBox(this, IDC_STATIC_PRESSURE_DROPS, "Static pressure drops");
  bottomLeftSizer->Add(chkStaticPressureDrops, 0, wxGROW | wxALL, 5);

  chkLumpedElements = new wxCheckBox(this, IDC_LUMPED_ELEMENTS, "Lumped elements in T-sections");
  bottomLeftSizer->Add(chkLumpedElements, 0, wxGROW | wxALL, 5);

  chkInnerLengthCorrections = new wxCheckBox(this, IDC_INNER_LENGTH_CORRECTIONS, "Inner (tube) length corrections");
  bottomLeftSizer->Add(chkInnerLengthCorrections, 0, wxGROW | wxALL, 5);

  // ****************************************************************
  // Buttons
  // ****************************************************************

  button = new wxButton(this, IDB_CONSTANTS, "Constants");
  bottomRightSizer->Add(button, 0, wxALL, 5);

  // ****************************************************************
  // Init the top-level sizer and set it for this dialog
  // ****************************************************************

  topLevelSizer->Add(topLeftSizer, 1, wxALL | wxGROW, 5);
  topLevelSizer->Add(topRightSizer, 0, wxALL, 5);
  topLevelSizer->Add(bottomLeftSizer, 0, wxALL, 5);
  topLevelSizer->Add(bottomRightSizer, 1, wxALL | wxGROW, 5);

  this->SetSizer(topLevelSizer);
  topLevelSizer->Fit(this);
  topLevelSizer->SetSizeHints(this);
}


// ****************************************************************************
/// Update the widgets.
// ****************************************************************************

void FdsOptionsDialog::updateWidgets(bool updateParent)
{
  TlModel::Options *options = &model->options;

  radRadiationOptions->SetSelection((int)options->radiation);

  chkBoundaryLayer->SetValue(options->boundaryLayer);
  chkHeatConduction->SetValue(options->heatConduction);
  chkSoftWalls->SetValue(options->softWalls);
  chkHagenResistance->SetValue(options->hagenResistance);

  chkParanasalSinuses->SetValue(options->paranasalSinuses);
  chkPiriformFossa->SetValue(options->piriformFossa);
  chkStaticPressureDrops->SetValue(options->staticPressureDrops);
  chkLumpedElements->SetValue(options->lumpedElements);
  chkInnerLengthCorrections->SetValue(options->innerLengthCorrections);

  if (updateParent)
  {
     wxCommandEvent event(updateRequestEvent);
     event.SetInt(REFRESH_PICTURES);
     wxPostEvent(this->GetParent(), event);
  }
}


// ****************************************************************************
// ****************************************************************************

void FdsOptionsDialog::OnRadiationOptions(wxCommandEvent &event)
{
  int i = radRadiationOptions->GetSelection();

  switch (i)
  {
  case TlModel::NO_RADIATION:             model->options.radiation = TlModel::NO_RADIATION; break;
  case TlModel::PISTONINSPHERE_RADIATION: model->options.radiation = TlModel::PISTONINSPHERE_RADIATION; break;
  case TlModel::PISTONINWALL_RADIATION:   model->options.radiation = TlModel::PISTONINWALL_RADIATION; break;
  case TlModel::PARALLEL_RADIATION:       model->options.radiation = TlModel::PARALLEL_RADIATION; break;
  default: break;
  }

  updateWidgets();
}


// ****************************************************************************
// ****************************************************************************

void FdsOptionsDialog::OnBoundaryLayer(wxCommandEvent &event)
{
  model->options.boundaryLayer = chkBoundaryLayer->GetValue();
  updateWidgets();
}


// ****************************************************************************
// ****************************************************************************

void FdsOptionsDialog::OnHeatConduction(wxCommandEvent &event)
{
  model->options.heatConduction = chkHeatConduction->GetValue();
  updateWidgets();
}


// ****************************************************************************
// ****************************************************************************

void FdsOptionsDialog::OnSoftWalls(wxCommandEvent &event)
{
  model->options.softWalls = chkSoftWalls->GetValue();
  updateWidgets();
}


// ****************************************************************************
// ****************************************************************************

void FdsOptionsDialog::OnHagenResistance(wxCommandEvent &event)
{
  model->options.hagenResistance = chkHagenResistance->GetValue();
  updateWidgets();
}


// ****************************************************************************
// ****************************************************************************

void FdsOptionsDialog::OnParanasalSinuses(wxCommandEvent &event)
{
  model->options.paranasalSinuses = chkParanasalSinuses->GetValue();
  updateWidgets();
}


// ****************************************************************************
// ****************************************************************************

void FdsOptionsDialog::OnPiriformFossa(wxCommandEvent &event)
{
  model->options.piriformFossa = chkPiriformFossa->GetValue();
  updateWidgets();
}


// ****************************************************************************
// ****************************************************************************

void FdsOptionsDialog::OnStaticPressureDrops(wxCommandEvent &event)
{
  model->options.staticPressureDrops = chkStaticPressureDrops->GetValue();
  updateWidgets();
}


// ****************************************************************************
// ****************************************************************************

void FdsOptionsDialog::OnLumpedElements(wxCommandEvent &event)
{
  model->options.lumpedElements = chkLumpedElements->GetValue();
  updateWidgets();
}


// ****************************************************************************
// ****************************************************************************

void FdsOptionsDialog::OnInnerLengthCorrections(wxCommandEvent &event)
{
  model->options.innerLengthCorrections = chkInnerLengthCorrections->GetValue();
  updateWidgets();
}


// ****************************************************************************
// ****************************************************************************

void FdsOptionsDialog::OnConstants(wxCommandEvent &event)
{
  wxString st = wxString::Format(
    "Static pressure: %2.2f dPa (=ubar)\n"
    "Ambient density: %2.6f g/cm^3\n"
    "Adiabatic constant: %2.1f\n"
    "Sound velocity: %2.1f cm/s\n"
    "Air viscosity: %2.7f dyne-s/cm^2\n"
    "Specific heat: %2.2f cal/g-K\n"
    "Heat conduction coeff.: %2.6f cal/cm-s-K",
    STATIC_PRESSURE_CGS,
    AMBIENT_DENSITY_CGS,
    ADIABATIC_CONSTANT,
    SOUND_VELOCITY_CGS,
    AIR_VISCOSITY_CGS,
    SPECIFIC_HEAT_CGS,
    HEAT_CONDUCTION_CGS);

  wxMessageBox(st, "Physical constants:", wxOK | wxICON_INFORMATION, this);
}

// ****************************************************************************
