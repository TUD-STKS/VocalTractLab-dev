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

#ifndef __FORMANT_OPTIMIZATION_DIALOG_H__
#define __FORMANT_OPTIMIZATION_DIALOG_H__

#include <wx/wx.h>
#include <wx/dialog.h>

#include "VocalTractLabBackend/VocalTract.h"


// ****************************************************************************
/// This dialog lets the user adjust the parameters for the formant 
/// optimization.
// ****************************************************************************

class FormantOptimizationDialog : public wxDialog
{
  // **************************************************************************
  // Public data.
  // **************************************************************************

public:
  double F1, F2, F3;
  bool paramFixed[VocalTract::NUM_PARAMS];
  double minArea_mm2;
  double releaseArea_mm2;
  double maxDisplacement_mm;
  wxString contextVowel;    // Only for optimization of consonants
  bool optimizeVowel;

  // **************************************************************************
  // Public functions.
  // **************************************************************************

public:
  FormantOptimizationDialog(wxWindow *parent, VocalTract *vocalTract);

  // **************************************************************************
  // Private data.
  // **************************************************************************

private:
  VocalTract *vocalTract;
  wxTextCtrl *txtFormants;
  wxCheckBox *chkParamFixed[VocalTract::NUM_PARAMS];
  wxTextCtrl *txtMinArea;
  wxTextCtrl *txtReleaseArea;
  wxTextCtrl *txtMaxDisplacement;
  wxTextCtrl *txtContextVowel;

  // **************************************************************************
  // Private functions.
  // **************************************************************************

private:
  void initWidgets();
  void updateWidgets();
  void resetValues();
  bool setParamsFromControls();

  // Event handlers.

  void OnSetDefaultValues(wxCommandEvent &event);
  void OnOptimizeVowel(wxCommandEvent &event);
  void OnOptimizeConsonant(wxCommandEvent &event);
  void OnShow(wxShowEvent &event);

  // **************************************************************************
  // Declare the event table.
  // **************************************************************************

  DECLARE_EVENT_TABLE()
};


#endif
