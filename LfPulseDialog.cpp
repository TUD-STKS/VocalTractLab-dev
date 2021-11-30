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

#include "LfPulseDialog.h"

static const int IDS_FIRST_PARAM = 4000;
static const int IDL_FIRST_PARAM = 4010;

const double LfPulseDialog::PARAM_MIN[LfPulseDialog::NUM_PARAMS] =
{
  0.2,    // OQ
  2.0,    // SQ
  0.0,    // TL
  60.0,   // F0
  0.	  // SNR
};

const double LfPulseDialog::PARAM_MAX[LfPulseDialog::NUM_PARAMS] =
{
  0.8,    // OP
  5.0,    // SQ
  0.2,    // TL
  520.0,  // F0
  50.     // SNR
};

// The single instance of this class.
LfPulseDialog *LfPulseDialog::instance = NULL;


// ****************************************************************************
// The event table.
// ****************************************************************************

BEGIN_EVENT_TABLE(LfPulseDialog, wxDialog)
  EVT_SCROLL(LfPulseDialog::OnScrollEvent)
END_EVENT_TABLE()


// ****************************************************************************
/// Returns the single instance of this dialog.
// ****************************************************************************

LfPulseDialog *LfPulseDialog::getInstance()
{
  if (instance == NULL)
  {
    instance = new LfPulseDialog(NULL);
  }

  return instance;
}


// ****************************************************************************
/// Private constructor.
// ****************************************************************************

LfPulseDialog::LfPulseDialog(wxWindow *parent) : 
  wxDialog(parent, wxID_ANY, wxString("LF glottal flow pulse"), 
           wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE)
{
  this->Move(100, 50);

  // ****************************************************************
  // Init the variables first.
  // ****************************************************************

  data = Data::getInstance();
  lfPulse = &data->lfPulse;

  // ****************************************************************
  // Init and update the widgets.
  // ****************************************************************

  initWidgets();
  updateWidgets();
}


// ****************************************************************************
/// Init the widgets.
// ****************************************************************************

void LfPulseDialog::initWidgets()
{
  int i;
  wxStaticText *label = NULL;
  wxString paramNames[NUM_PARAMS] = 
  {
    "Open quotient",
    "Shape quotient",
    "Spectral tilt",
    "F0",
	"SNR"
  };

  wxBoxSizer *topLevelSizer = new wxBoxSizer(wxVERTICAL);
  wxFlexGridSizer *topSizer = new wxFlexGridSizer(NUM_PARAMS, 3, 0, 0);
  wxBoxSizer *bottomSizer = new wxBoxSizer(wxHORIZONTAL);


  // Create an invisible dummy button with the ID wxID_CANCEL, so that
  // the user can close the dialog with ESC.
  wxButton *button = new wxButton(this, wxID_CANCEL, "", wxPoint(0,0), wxSize(0,0));


  // ****************************************************************
  // Create the top sizer with the scroll bars.
  // ****************************************************************

  topSizer->AddGrowableCol(1);  // Make the 2nd column stretchable

  for (i=0; i < NUM_PARAMS; i++)
  {
    label = new wxStaticText(this, wxID_ANY, paramNames[i]);
    topSizer->Add(label, 0, wxALL, 5);

    scrParam[i] = new wxScrollBar(this, IDS_FIRST_PARAM + i);
    scrParam[i]->SetMinSize(this->FromDIP(wxSize(100, wxDefaultCoord) ));    // Set a fixed size
    scrParam[i]->SetScrollbar(0, 1, 101, 1, false);
    topSizer->Add(scrParam[i], 1, wxALL, 5);

    labParam[i] = new wxStaticText(this, IDL_FIRST_PARAM + i, "0.00");
    labParam[i]->SetMinSize(this->FromDIP(wxSize(50, wxDefaultCoord) ));    // Set a fixed size
    topSizer->Add(labParam[i], 0, wxALL, 5);
  }

  // ****************************************************************
  // Create the bottom sizer with the picture.
  // ****************************************************************

  lfPulsePicture = new LfPulsePicture(this);
  lfPulsePicture->SetMinSize(this->FromDIP(wxSize(250, 200) ));

  bottomSizer->Add(lfPulsePicture, 0, wxALL | wxGROW, 5);

  // ****************************************************************
  // Assemble the top-level sizer.
  // ****************************************************************

  topLevelSizer->AddSpacer(10);
  topLevelSizer->Add(topSizer, 0, wxALL, 5);
  topLevelSizer->Add(bottomSizer, 1, wxGROW | wxALL, 5);

  // Set the top-level-sizer for this window
  this->SetSizer(topLevelSizer);
  topLevelSizer->Fit(this);
  topLevelSizer->SetSizeHints(this);
}


// ****************************************************************************
/// Update the widget states.
// ****************************************************************************

void LfPulseDialog::updateWidgets()
{
  int i;
  wxString st;
  double value[NUM_PARAMS] =
  {
    lfPulse->OQ,
    lfPulse->SQ,
    lfPulse->TL,
    lfPulse->F0,
	lfPulse->SNR
  };
  int pos;

  // ****************************************************************

  for (i=0; i < NUM_PARAMS; i++)
  {
    // Update the scroll bar position

    // Add 0.5 to value before casting to int to round to nearest int!
    pos = (int)(100.0*(value[i] - PARAM_MIN[i]) / (PARAM_MAX[i] - PARAM_MIN[i]) + 0.5);
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
    st = wxString::Format("%2.2f", value[i]);
    if (i == FUNDAMENTAL_FREQUENCY)
    {
      st+= " Hz";
    }
    labParam[i]->SetLabel(st);
  }

  // Refresh the pulse picture
  lfPulsePicture->Refresh();
}


// ****************************************************************************
/// One of the scroll bars for the parameters has changed.
// ****************************************************************************

void LfPulseDialog::OnScrollEvent(wxScrollEvent &event)
{
  int index = event.GetId() - IDS_FIRST_PARAM;

  if ((index < 0) || (index >= NUM_PARAMS))
  {
    return;
  }

  int pos = scrParam[index]->GetThumbPosition();
  double value = PARAM_MIN[index] + (pos/100.0)*(PARAM_MAX[index] - PARAM_MIN[index]);

  switch (index)
  {
  case OPEN_QUOTIENT: lfPulse->OQ = value; break;
  case SHAPE_QUOTIENT: lfPulse->SQ = value; break;
  case SPECTRAL_TILT: lfPulse->TL = value; break;
  case FUNDAMENTAL_FREQUENCY: lfPulse->F0 = value; break;
  case SIGNAL_TO_NOISE_RATIO: lfPulse->SNR = value; break;
  default: break;
  }

  updateWidgets();

  // Update the pictures on the parent window of the dialog.

   wxCommandEvent myEvent(updateRequestEvent);
   myEvent.SetInt(REFRESH_PICTURES);
   wxPostEvent(this->GetParent(), myEvent);

   wxYield();
}

// ****************************************************************************
