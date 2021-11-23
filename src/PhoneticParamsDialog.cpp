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

#include "PhoneticParamsDialog.h"
#include <wx/wupdlock.h>

// ****************************************************************************
// IDs.
// ****************************************************************************

static const int IDS_VALUE_0            = 4000;
static const int IDL_VALUE_0            = 4100;

// ****************************************************************************
// The event table.
// ****************************************************************************

BEGIN_EVENT_TABLE(PhoneticParamsDialog, wxDialog)
  EVT_SCROLL(PhoneticParamsDialog::OnScrollbarChanged)
END_EVENT_TABLE()

// The single instance of this class.
PhoneticParamsDialog *PhoneticParamsDialog::instance = NULL;


// ****************************************************************************
/// Returns the single instance of this dialog.
// ****************************************************************************

PhoneticParamsDialog *PhoneticParamsDialog::getInstance(wxWindow *parent)
{
  if (instance == NULL)
  {
    instance = new PhoneticParamsDialog(parent);
  }

  return instance;
}


// ****************************************************************************
// ****************************************************************************

void PhoneticParamsDialog::updateWidgets()
{
  // ****************************************************************
  // Update the scrollbar positions and labels for all values.
  // ****************************************************************

  int i;
  int x;
  wxString st;

  for (i=0; i < Data::NUM_PHONETIC_PARAMS; i++)
  {
    st = wxString::Format("%d %%", (int)(data->phoneticParamValue[i]*100.0));
    labValue[i]->SetLabel(st);

    x = (int)(data->phoneticParamValue[i]*100 + 0.5);
    scrValue[i]->SetThumbPosition(x);
  }

}


// ****************************************************************************
/// Set the windows that receive an update request message when a vocal tract
/// parameter was changed.
// ****************************************************************************

void PhoneticParamsDialog::setUpdateRequestReceiver(wxWindow *receiver)
{
  updateRequestReceiver = receiver;
}


// ****************************************************************************
/// Constructor.
// ****************************************************************************

PhoneticParamsDialog::PhoneticParamsDialog(wxWindow *parent) : 
  wxDialog(parent, wxID_ANY, wxString("Phonetic parameters"), 
    wxDefaultPosition, wxDefaultSize, 
    wxDEFAULT_DIALOG_STYLE)
{
  this->SetDoubleBuffered(true);
  this->Move(100, 100);

  // ****************************************************************
  // Init the variables first.
  // ****************************************************************

  data = Data::getInstance();
  updateRequestReceiver = NULL;

  // ****************************************************************
  // Init and update the widgets.
  // ****************************************************************

  initWidgets();
  updateWidgets();
}


// ****************************************************************************
/// Init the widgets of this dialog.
// ****************************************************************************

void PhoneticParamsDialog::initWidgets()
{
  int i;
  wxBoxSizer *topLevelSizer = new wxBoxSizer(wxVERTICAL);
  wxBoxSizer *lineSizer = NULL;
  wxStaticText *label = NULL;

  // Create an invisible dummy button with the ID wxID_CANCEL, so that
  // the user can close the dialog with ESC.
  wxButton *button = new wxButton(this, wxID_CANCEL, "", wxPoint(0,0), wxSize(0,0));

  // ****************************************************************
  // ****************************************************************

  topLevelSizer->AddSpacer(10);

  wxString text = "The generation of vocal tract shapes from these parameters "
    "refers to the vocal tract shapes [a], [i], [u], [ll-labial-closure], [tt-alveolar-closure], "
    "and [tb-velar-closure]. Make sure that these shapes are in the vocal tract shape list!";

  label = new wxStaticText(this, wxID_ANY, text);
  label->Wrap(350);
  topLevelSizer->Add(label, 0, wxALL, 4);

  for (i=0; i < Data::NUM_PHONETIC_PARAMS; i++)
  {
    lineSizer = new wxBoxSizer(wxHORIZONTAL);

    label = new wxStaticText(this, wxID_ANY, Data::phoneticParamName[i], 
      wxDefaultPosition, wxSize(150, 20));
    lineSizer->Add(label, 0, wxALL, 2);

    scrValue[i] = new wxScrollBar(this, IDS_VALUE_0 + i, wxDefaultPosition, wxSize(150, 20));
    scrValue[i]->SetScrollbar(0, 1, 101, 1, true);
    lineSizer->Add(scrValue[i], 0, wxALL, 2);

    labValue[i] = new wxStaticText(this, IDL_VALUE_0 + i, "0", wxDefaultPosition, wxSize(50, 20));
    lineSizer->Add(labValue[i], 0, wxALL, 2);

    topLevelSizer->Add(lineSizer, 0, wxALL, 2);
  }

  topLevelSizer->AddSpacer(10);

  // ****************************************************************
  // Set the top-level-sizer for this window.
  // ****************************************************************

  this->SetSizer(topLevelSizer);
  topLevelSizer->Fit(this);
}


// ****************************************************************************
/// Calculate the vocal tract shape according to the phonetic parameters and 
/// send a message to the parent window to update the vocal tract picture.
// ****************************************************************************

void PhoneticParamsDialog::updateVocalTract()
{
  data->phoneticParamsToVocalTract();

  if (updateRequestReceiver != NULL)
  {
    // Refresh the pictures of other windows.
    wxCommandEvent event(updateRequestEvent);
    event.SetInt(UPDATE_VOCAL_TRACT);
    wxPostEvent(updateRequestReceiver, event);
  }
}


// ****************************************************************************
/// A parameter value was changed using the scrollbar.
// ****************************************************************************

void PhoneticParamsDialog::OnScrollbarChanged(wxScrollEvent &event)
{
  int pos = event.GetPosition();
  int id = event.GetId();
  int index = id - IDS_VALUE_0;

  if ((index >= 0) && (index < Data::NUM_PHONETIC_PARAMS))
  {
    data->phoneticParamValue[index] = (double)pos / 100.0;
  }

  updateWidgets();
  updateVocalTract();
  wxYield();
}

// ****************************************************************************

