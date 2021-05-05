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

#ifndef __TRANSITION_DIALOG__
#define __TRANSITION_DIALOG__

#include <wx/wx.h>
#include <wx/dialog.h>

#include "Data.h"

// ****************************************************************************
/// This dialog lets the analyze a consonant-vowel transition with the new
/// coarticulation model.
// ****************************************************************************

class TransitionDialog : public wxDialog
{
  // **************************************************************************
  // Public data.
  // **************************************************************************

public:

  // **************************************************************************
  // Public functions.
  // **************************************************************************

public:
  static TransitionDialog *getInstance(wxWindow *parent = NULL);
  void updateWidgets();
  void setUpdateRequestReceiver(wxWindow *receiver);

  // **************************************************************************
  // Private data.
  // **************************************************************************

private:
  static TransitionDialog *instance;

  // Variables of the controls
  wxString consonant;
  wxString vowel;
  double alphaTongue, betaTongue;
  double transitionPos;   // 0 <= x <= 1
  double minArea_mm2;
  bool useFixedVowel;

  Data *data;
  wxWindow *updateRequestReceiver;

  wxComboBox *lstConsonant;
  wxRadioButton *radFixedVowel;
  wxComboBox *lstVowel;
  wxStaticText *labVowelParams;
  wxRadioButton *radInterpolatedVowel;
  wxScrollBar *scrAlpha;
  wxScrollBar *scrBeta;
  wxStaticText *labAlpha;
  wxStaticText *labBeta;

  wxScrollBar *scrTransitionPos;
  wxStaticText *labTransitionPos;
  wxTextCtrl *txtMinArea;

  // **************************************************************************
  // Private functions.
  // **************************************************************************

private:
  TransitionDialog(wxWindow *parent);
  void initWidgets();
  
  bool getConsonantAndVowelShape(double *consonantParams, double *vowelParams,
    double &aTongue, double &bTongue, double &aLips, double &bLips);

  void calcVocalTract();
  void updateVocalTract();
  void fillShapeLists();
  void get2dVowelParams(const wxString &vowel, double &alphaTongue, double &betaTongue, 
    double &alphaLips, double &betaLips);

  void OnConsonantSelected(wxCommandEvent &event);
  void OnUpdateLists(wxCommandEvent &event);
  void OnVowelSelected(wxCommandEvent &event);
  void OnUseFixedVowel(wxCommandEvent &event);
  void OnUseInterpolatedVowel(wxCommandEvent &event);
  void OnScrollbarChanged(wxScrollEvent &event);
  void OnFindReleasePosition(wxCommandEvent &event);
  void OnMinAreaEnter(wxCommandEvent &event);

  // **************************************************************************
  // Declare the event table.
  // **************************************************************************

  DECLARE_EVENT_TABLE()
};

#endif
