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

#ifndef __ANATOMY_PARAMS_DIALOG__
#define __ANATOMY_PARAMS_DIALOG__

#include <wx/wx.h>
#include <wx/dialog.h>

#include "Data.h"
#include "Backend/AnatomyParams.h"

// ****************************************************************************
/// This dialog lets the user adjust some high-level anatomy parameters
/// to simulate vocal tract growth, for example.
// ****************************************************************************

class AnatomyParamsDialog : public wxDialog
{
  // **************************************************************************
  // Public data.
  // **************************************************************************

public:

  // **************************************************************************
  // Public functions.
  // **************************************************************************

public:
  static AnatomyParamsDialog *getInstance(wxWindow *parent = NULL);
  void setUpdateRequestReceiver(wxWindow *receiver);

  // **************************************************************************
  // Private data.
  // **************************************************************************

private:
  // The single instance of this Singleton
  static AnatomyParamsDialog *instance;
  bool isMale;
  int age_month;
  wxString referenceSpeakerFileName;

  Data *data;
  AnatomyParams *params;
  wxWindow *updateRequestReceiver;
  
  wxStaticText *labFileName;
  wxStaticText *labAge;
  wxScrollBar *scrAge;
  wxRadioButton *radMale;
  wxRadioButton *radFemale;

  wxScrollBar *scrParam[AnatomyParams::NUM_ANATOMY_PARAMS];
  wxStaticText *labParam[AnatomyParams::NUM_ANATOMY_PARAMS];

  // **************************************************************************
  // Private functions.
  // **************************************************************************

private:
  AnatomyParamsDialog(wxWindow *parent);
  void initWidgets();
  void updateWidgets();
  void updateVocalTract();

  void OnLoadReferenceSpeaker(wxCommandEvent &event);
  void OnGetParamsFromVocalTract(wxCommandEvent &event);
  void OnSetParamsForVocalTract(wxCommandEvent &event);
  void OnSelectMale(wxCommandEvent &event);
  void OnSelectFemale(wxCommandEvent &event);
  void OnScrollEvent(wxScrollEvent &event);

  // **************************************************************************
  // Declare the event table.
  // **************************************************************************

  DECLARE_EVENT_TABLE()
};


#endif
