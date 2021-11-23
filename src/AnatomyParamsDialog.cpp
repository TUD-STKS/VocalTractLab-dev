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

#include "AnatomyParamsDialog.h"
#include "VocalTractShapesDialog.h"
#include <wx/filename.h>

// The single instance of this class.
AnatomyParamsDialog *AnatomyParamsDialog::instance = NULL;

// ****************************************************************************
// IDs for the controls.
// ****************************************************************************

static const int IDB_LOAD_REF_SPEAKER   = 4000;
static const int IDL_AGE                = 4001;
static const int IDS_AGE                = 4002;
static const int IDR_MALE               = 4003;
static const int IDR_FEMALE             = 4004;
static const int IDB_GET_PARAMS_FROM_VT = 4005;
static const int IDB_SET_PARAMS_FOR_VT  = 4006;
static const int IDS_PARAM0             = 4100;


// ****************************************************************************
// The event table.
// ****************************************************************************

BEGIN_EVENT_TABLE(AnatomyParamsDialog, wxDialog)
  EVT_BUTTON(IDB_LOAD_REF_SPEAKER, AnatomyParamsDialog::OnLoadReferenceSpeaker)
  EVT_BUTTON(IDB_GET_PARAMS_FROM_VT, AnatomyParamsDialog::OnGetParamsFromVocalTract)
  EVT_BUTTON(IDB_SET_PARAMS_FOR_VT, AnatomyParamsDialog::OnSetParamsForVocalTract)
  EVT_RADIOBUTTON(IDR_MALE, AnatomyParamsDialog::OnSelectMale)
  EVT_RADIOBUTTON(IDR_FEMALE, AnatomyParamsDialog::OnSelectFemale)
  EVT_SCROLL(AnatomyParamsDialog::OnScrollEvent)
END_EVENT_TABLE()


// ****************************************************************************
/// Returns the single instance of this dialog.
// ****************************************************************************

AnatomyParamsDialog *AnatomyParamsDialog::getInstance(wxWindow *parent)
{
  if (instance == NULL)
  {
    instance = new AnatomyParamsDialog(parent);
  }

  return instance;
}


// ****************************************************************************
/// Set the windows that receive an update request message when a vocal tract
/// parameter was changed.
// ****************************************************************************

void AnatomyParamsDialog::setUpdateRequestReceiver(wxWindow *receiver)
{
  updateRequestReceiver = receiver;
}


// ****************************************************************************
/// Private constructor.
// ****************************************************************************

AnatomyParamsDialog::AnatomyParamsDialog(wxWindow *parent) : 
  wxDialog(parent, wxID_ANY, wxString("Anatomy parameters"), 
           wxDefaultPosition, wxDefaultSize, 
           wxDEFAULT_DIALOG_STYLE)
{
  this->Move(100, 0);

  // ****************************************************************
  // Init the variables first.
  // ****************************************************************

  isMale = true;
  referenceSpeakerFileName = "";
  age_month = 0;

  data = Data::getInstance();
  params = data->anatomyParams;

  // ****************************************************************
  // Init and update the widgets.
  // ****************************************************************

  initWidgets();
  updateWidgets();
}


// ****************************************************************************
/// Init the widgets.
// ****************************************************************************

void AnatomyParamsDialog::initWidgets()
{
  int i;
  wxStaticText *label = NULL;

  wxBoxSizer *topLevelSizer = new wxBoxSizer(wxVERTICAL);
  wxStaticBoxSizer *upperSizer = new wxStaticBoxSizer(wxVERTICAL, this, "High-level parameters");
  wxStaticBoxSizer *lowerSizer = new wxStaticBoxSizer(wxVERTICAL, this, "Low-level parameters");
  wxSizer *sizer = NULL;

  // Create an invisible dummy button with the ID wxID_CANCEL, so that
  // the user can close the dialog with ESC.
  wxButton *button = new wxButton(this, wxID_CANCEL, "", wxPoint(0,0), wxSize(0,0));

  // ****************************************************************
  // Controls to load the reference speaker.
  // ****************************************************************

  sizer = new wxBoxSizer(wxHORIZONTAL);

  button = new wxButton(this, IDB_LOAD_REF_SPEAKER, "Load reference speaker");
  sizer->Add(button);

  labFileName = new wxStaticText(this, wxID_ANY, "No reference speaker loaded");
  sizer->Add(labFileName, 0, wxALIGN_CENTER | wxLEFT, 10);

  topLevelSizer->Add(sizer, 0, wxALL, 5);

  // ****************************************************************
  // High-level parameter controls (for age + gender settings).
  // ****************************************************************

  sizer = new wxBoxSizer(wxHORIZONTAL);

  label = new wxStaticText(this, wxID_ANY, "Age: ");
  sizer->Add(label);

  labAge = new wxStaticText(this, wxID_ANY, "x");
  sizer->Add(labAge);

  upperSizer->Add(sizer, 0, wxALL, 5);

  // ****************************************************************

  scrAge = new wxScrollBar(this, IDS_AGE, wxDefaultPosition, wxSize(300, -1));
  scrAge->SetScrollbar(0, 1, 12*20, 1, false);    // Step size is in month.
  upperSizer->Add(scrAge, 0, wxALL, 5);

  // ****************************************************************

  sizer = new wxBoxSizer(wxHORIZONTAL);
  
  radMale = new wxRadioButton(this, IDR_MALE, "Male");
  sizer->Add(radMale);

  radFemale = new wxRadioButton(this, IDR_FEMALE, "Female");
  sizer->Add(radFemale, 0, wxLEFT, 10);

  upperSizer->Add(sizer, 0, wxALL, 5);

  // ****************************************************************
  // Low-level parameter controls.
  // ****************************************************************

  sizer = new wxBoxSizer(wxHORIZONTAL);

  button = new wxButton(this, IDB_GET_PARAMS_FROM_VT, "Get parameters from VT");
  sizer->Add(button, 1);

  button = new wxButton(this, IDB_SET_PARAMS_FOR_VT, "Set parameters for VT");
  sizer->Add(button, 1, wxLEFT, 10);

  lowerSizer->Add(sizer, 0, wxALL | wxGROW, 5);

  // ****************************************************************

  wxString st;

  for (i=0; i < AnatomyParams::NUM_ANATOMY_PARAMS; i++)
  {
    st = wxString(params->param[i].name) + " (" + wxString(params->param[i].abbr) + ")";
    label = new wxStaticText(this, wxID_ANY, st);
    lowerSizer->Add(label, 0, wxALL, 3);

    sizer = new wxBoxSizer(wxHORIZONTAL);

    scrParam[i] = new wxScrollBar(this, IDS_PARAM0 + i, wxDefaultPosition, wxSize(250, -1));
    scrParam[i]->SetScrollbar(0, 1, 101, 1, false);
    sizer->Add(scrParam[i], 0);

    labParam[i] = new wxStaticText(this, wxID_ANY, "x", wxDefaultPosition, wxSize(50, -1));
    sizer->Add(labParam[i], 0, wxALIGN_CENTER | wxLEFT, 10);

    lowerSizer->Add(sizer, 0, wxALL, 3);
  }

  // ****************************************************************
  // Assemble the top-level sizer.
  // ****************************************************************

  topLevelSizer->Add(upperSizer, 0, wxGROW | wxALL, 5);
  topLevelSizer->Add(lowerSizer, 0, wxGROW | wxALL, 5);

  // Set the top-level-sizer for this window
  this->SetSizer(topLevelSizer);
  topLevelSizer->Fit(this);
  topLevelSizer->SetSizeHints(this);
}


// ****************************************************************************
/// Update the widget states.
// ****************************************************************************

void AnatomyParamsDialog::updateWidgets()
{
  wxString st;
  int pos;
  int i;

  // ****************************************************************
  // File name of the reference speaker.
  // ****************************************************************
  
  if (referenceSpeakerFileName == "")
  {
    labFileName->SetLabel("No reference speaker loaded");
  }
  else
  {
    wxFileName fileName(referenceSpeakerFileName);
    labFileName->SetLabel(fileName.GetFullName());
  }

  // ****************************************************************
  // Age of the speaker.
  // ****************************************************************

  if (age_month < 0)
  {
    age_month = 0;
  }

  if (age_month > 12*20)
  {
    age_month = 12*20;
  }

  int years = (int)(age_month / 12);
  int months = (int)(age_month % 12);

  st = wxString::Format("%d year(s) and %d months", years, months);
  labAge->SetLabel(st);

  if (age_month != scrAge->GetThumbPosition())
  {
    scrAge->SetThumbPosition(age_month);
  }

  // ****************************************************************
  // Gender of the speaker.
  // ****************************************************************

  if (isMale)
  {
    radMale->SetValue(true);
  }
  else
  {
    radFemale->SetValue(true);
  }

  // ****************************************************************
  // Values of the low-level parameters.
  // ****************************************************************

  for (i=0; i < AnatomyParams::NUM_ANATOMY_PARAMS; i++)
  {
    // Update the scroll bar position

    // Add 0.5 to value before casting to int to round to nearest int!
    pos = (int)(100.0*(params->param[i].x - params->param[i].min) / 
                      (params->param[i].max - params->param[i].min) + 0.5);
    if (pos < 0)
    {
      pos = 0;
    }
    if (pos > 100)
    {
      pos = 100;
    }

    if (pos != scrParam[i]->GetThumbPosition())
    {
      scrParam[i]->SetThumbPosition(pos);
    }

    // Update the label
    st = wxString::Format("%2.2f %s", params->param[i].x, wxString(params->param[i].unit));
    labParam[i]->SetLabel(st);
  }

}


// ****************************************************************************
/// Send a message to the vocal tract dialog to update the vocal tract picture.
// ****************************************************************************

void AnatomyParamsDialog::updateVocalTract()
{
  if (updateRequestReceiver != NULL)
  {
    // Refresh the pictures of other windows.
    wxCommandEvent event(updateRequestEvent);
    event.SetInt(UPDATE_VOCAL_TRACT);
    wxPostEvent(updateRequestReceiver, event);

    wxYield();
  }
}


// ****************************************************************************
// ****************************************************************************

void AnatomyParamsDialog::OnLoadReferenceSpeaker(wxCommandEvent &event)
{
  wxFileName fileName(referenceSpeakerFileName);

  wxString name = wxFileSelector("Load reference speaker", fileName.GetPath(), 
    fileName.GetFullName(), "speaker", "Speaker files (*.speaker)|*.speaker", 
    wxFD_OPEN | wxFD_FILE_MUST_EXIST, this);

  if (name.empty() == false)
  {
    if (params->loadReferenceVocalTract(name.ToStdString()))
    {
      referenceSpeakerFileName = name;
    }
    else
    {
      wxMessageBox(wxString::Format("Failed to load reference speaker from file %s.", name),
        "Error");
    }
  }

  updateWidgets();
}


// ****************************************************************************
// ****************************************************************************

void AnatomyParamsDialog::OnGetParamsFromVocalTract(wxCommandEvent &event)
{
  params->getFrom(data->vocalTract);
  updateWidgets();
}


// ****************************************************************************
// ****************************************************************************

void AnatomyParamsDialog::OnSetParamsForVocalTract(wxCommandEvent &event)
{
  params->setFor(data->vocalTract);

  updateWidgets();
  updateVocalTract();
}


// ****************************************************************************
// ****************************************************************************

void AnatomyParamsDialog::OnSelectMale(wxCommandEvent &event)
{
  isMale = true;
  updateWidgets();
}


// ****************************************************************************
// ****************************************************************************

void AnatomyParamsDialog::OnSelectFemale(wxCommandEvent &event)
{
  isMale = false;
  updateWidgets();
}


// ****************************************************************************
/// One of the scroll bars for the parameters has changed.
// ****************************************************************************

void AnatomyParamsDialog::OnScrollEvent(wxScrollEvent &event)
{
  if (event.GetId() == IDS_AGE)
  {
    age_month = scrAge->GetThumbPosition();
    params->calcFromAge(age_month, isMale);
  }
  else
  {
    int index = event.GetId() - IDS_PARAM0;

    if ((index < 0) || (index >= AnatomyParams::NUM_ANATOMY_PARAMS))
    {
      return;
    }

    int pos = scrParam[index]->GetThumbPosition();
    double value = params->param[index].min + 
      (pos/100.0)*(params->param[index].max - params->param[index].min);
    
    params->param[index].x = value;
  }

  params->setFor(data->vocalTract);

  // Bacause the shape list was also transformed, refill the list in 
  // the corresponding dialog.

  VocalTractShapesDialog *dialog = VocalTractShapesDialog::getInstance();
  dialog->updateWidgets();
  dialog->fillShapeList();


  updateWidgets();
  updateVocalTract();

}

// ****************************************************************************
