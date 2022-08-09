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

#ifndef __FDS_OPTIONS_DIALOG_H__
#define __FDS_OPTIONS_DIALOG_H__

#include <wx/wx.h>
#include <wx/dialog.h>

#include "Data.h"

// ****************************************************************************
/// This dialog lets the user adjust the options for the frequence-domain 
/// synthesis.
// ****************************************************************************

class FdsOptionsDialog : public wxDialog
{
  // **************************************************************************
  // Public functions.
  // **************************************************************************

public:
  static FdsOptionsDialog *getInstance();

  // **************************************************************************
  // Private data.
  // **************************************************************************

private:
  // The single instance of this Singleton
  static FdsOptionsDialog *instance;
  TlModel *model;

  wxRadioBox *radRadiationOptions;

  wxCheckBox *chkBoundaryLayer;
  wxCheckBox *chkHeatConduction;
  wxCheckBox *chkSoftWalls;
  wxCheckBox *chkHagenResistance;

  wxCheckBox *chkParanasalSinuses;
  wxCheckBox *chkPiriformFossa;
  wxCheckBox *chkStaticPressureDrops;
  wxCheckBox *chkLumpedElements;
  wxCheckBox *chkInnerLengthCorrections;

  wxButton *btnConstants;

  // **************************************************************************
  // Private functions.
  // **************************************************************************

private:
  FdsOptionsDialog(wxWindow *parent);
  void initWidgets();
  void updateWidgets(bool updateParent = true);

  void OnRadiationOptions(wxCommandEvent &event);
  void OnBoundaryLayer(wxCommandEvent &event);
  void OnHeatConduction(wxCommandEvent &event);
  void OnSoftWalls(wxCommandEvent &event);
  void OnHagenResistance(wxCommandEvent &event);
  void OnParanasalSinuses(wxCommandEvent &event);
  void OnPiriformFossa(wxCommandEvent &event);
  void OnStaticPressureDrops(wxCommandEvent &event);
  void OnLumpedElements(wxCommandEvent &event);
  void OnInnerLengthCorrections(wxCommandEvent &event);
  void OnConstants(wxCommandEvent &event);

  // **************************************************************************
  // Declare the event table.
  // **************************************************************************

  DECLARE_EVENT_TABLE()
};


#endif
