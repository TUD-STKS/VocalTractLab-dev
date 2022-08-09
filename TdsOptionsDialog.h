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

#ifndef __TDS_OPTIONS_DIALOG_H__
#define __TDS_OPTIONS_DIALOG_H__

#include <wx/wx.h>
#include <wx/dialog.h>

#include "Data.h"

// ****************************************************************************
/// This dialog lets the user adjust the options for the time-domain synthesis.
// ****************************************************************************

class TdsOptionsDialog : public wxDialog
{
  // **************************************************************************
  // Public data.
  // **************************************************************************

public:

  // **************************************************************************
  // Public functions.
  // **************************************************************************

public:
  static TdsOptionsDialog *getInstance();

  // **************************************************************************
  // Private data.
  // **************************************************************************

private:
  // The single instance of this Singleton
  static TdsOptionsDialog *instance;
  TdsModel *model;

  wxCheckBox *chkTurbulenceLosses;
  wxCheckBox *chkSoftWalls;
  wxCheckBox *chkNoiseSources;
  wxCheckBox *chkRadiationFromSkin;
  wxCheckBox *chkPiriformFossa;
  wxCheckBox *chkInnerLengthCorrections;
  wxCheckBox *chkTransvelarCoupling;
  wxRadioBox *radSolverOptions;

  wxButton *btnAdaptFromFds;

  // **************************************************************************
  // Private functions.
  // **************************************************************************

private:
  TdsOptionsDialog(wxWindow *parent);
  void initWidgets();
  void updateWidgets();

  void OnTurbulenceLosses(wxCommandEvent &event);
  void OnSoftWalls(wxCommandEvent &event);
  void OnNoiseSources(wxCommandEvent &event);
  void OnRadiationFromSkin(wxCommandEvent &event);
  void OnPiriformFossa(wxCommandEvent &event);
  void OnInnerLengthCorrections(wxCommandEvent &event);
  void OnTransvelarCoupling(wxCommandEvent &event);
  void OnSolverOptions(wxCommandEvent &event);
  void OnAdaptFromFds(wxCommandEvent &event);

  // **************************************************************************
  // Declare the event table.
  // **************************************************************************

  DECLARE_EVENT_TABLE()
};


#endif
