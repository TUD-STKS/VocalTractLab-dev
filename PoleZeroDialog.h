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

#ifndef __POLE_ZERO_DIALOG__
#define __POLE_ZERO_DIALOG__

#include <wx/wx.h>
#include <wx/dialog.h>

#include "Data.h"
#include "PoleZeroPlot.h"
#include "Backend/PoleZeroPlan.h"


// ****************************************************************************
/// This dialog lets the user edit a pole-zero plan.
// ****************************************************************************

class PoleZeroDialog : public wxDialog
{
  // **************************************************************************
  // Public data.
  // **************************************************************************

public:

  // **************************************************************************
  // Public functions.
  // **************************************************************************

public:
  static PoleZeroDialog *getInstance();
  void updateWidgets();
  void setSpectrumPicture(wxPanel *spectrumPicture);

  // **************************************************************************
  // Private data.
  // **************************************************************************

private:
  static PoleZeroDialog *instance;

  Data *data;
  PoleZeroPlan *plan;
  wxPanel *spectrumPicture;

  // Widgets.
  PoleZeroPlot *poleZeroPlot;
  wxStaticText *labPoleLocations;
  wxStaticText *labZeroLocations;
  wxCheckBox *chkHigherPoleCorrection;

  // **************************************************************************
  // Private functions.
  // **************************************************************************

private:
  PoleZeroDialog(wxWindow *parent);
  void initWidgets();
  void OnUpdateRequest(wxCommandEvent &event);

  void OnHigherPoleCorrection(wxCommandEvent &event);
  void OnEnterPoles(wxCommandEvent &event);
  void OnEnterZeros(wxCommandEvent &event);
  void OnPlayShortVowel(wxCommandEvent &event);
  void OnPlayLongVowel(wxCommandEvent &event);

  // **************************************************************************
  // Declare the event table.
  // **************************************************************************

  DECLARE_EVENT_TABLE()
};

#endif
