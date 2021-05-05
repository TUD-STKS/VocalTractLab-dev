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

#ifndef __VOCAL_TRACT_DIALOG_H__
#define __VOCAL_TRACT_DIALOG_H__

#include <wx/wx.h>
#include <wx/dialog.h>

#include "Data.h"
#include "VocalTractPicture.h"
#include "EmaConfigDialog.h"

// ****************************************************************************
/// This dialog displays the vocal tract picture.
// ****************************************************************************

class VocalTractDialog : public wxDialog
{
  // **************************************************************************
  // Public functions.
  // **************************************************************************

public:
  static VocalTractDialog *getInstance(wxWindow *parent = NULL);
  VocalTractPicture *getVocalTractPicture();
  void setUpdateRequestReceiver(wxWindow *updateRequestReceiver);
  void updateWidgets();

  // **************************************************************************
  // Private data.
  // **************************************************************************

private:
  static const int NUM_TONGUE_SIDE_POINTS = 3;

  // The single instance of this Singleton
  static VocalTractDialog *instance;

  wxWindow *updateRequestReceiver;  
  Data *data;
  wxSize minSizeWithControls;
  wxSize minSizeWithoutControls;
  wxBoxSizer *topLevelSizer;
  wxBoxSizer *controlSizer;

  VocalTractPicture *picVocalTract;
  wxButton *showHideControlsButton;
  wxPanel *controlPanel;

  // Vocal tract variables controls

  wxScrollBar *scrTongueSide[NUM_TONGUE_SIDE_POINTS];
  wxStaticText *labTongueSide[NUM_TONGUE_SIDE_POINTS];
  
  wxCheckBox *chkAutomaticTongueRootCalc;

  // Vocal tract display options.

  wxRadioButton *radNoTract;
  wxRadioButton *rad2dTract;
  wxRadioButton *rad3dTract;
  wxRadioButton *radWireFrameTract;
  wxCheckBox *chkBothSides;

  wxCheckBox *chkShowControlPoints;
  wxCheckBox *chkShowCenterLine;
  wxCheckBox *chkSmoothCenterLine;
  wxCheckBox *chkShowCutVectors;
  wxCheckBox *chkShowEmaPoints;

  wxStaticText *labBackgroundImageFileName;
  wxCheckBox *chkShowBackgroundImage;
  wxCheckBox *chkBackgroundImageEditMode;

  // **************************************************************************
  // Private functions.
  // **************************************************************************

private:
  VocalTractDialog(wxWindow *parent);
  void initWidgets();
  void updateVocalTractPage();

  void OnUpdateRequest(wxCommandEvent &event);

  void OnShowHideControls(wxCommandEvent &event);

  void OnTongueSideElevationChanged(wxScrollEvent &event);
  void OnAutomaticTongueRootCalc(wxCommandEvent &event);

  void OnShowNoTract(wxCommandEvent &event);
  void OnShow2dTract(wxCommandEvent &event);
  void OnShow3dTract(wxCommandEvent &event);
  void OnShowWireFrameTract(wxCommandEvent &event);
  void OnShowBothSides(wxCommandEvent &event);

  void OnShowControlPoints(wxCommandEvent &event);
  void OnShowCenterLine(wxCommandEvent &event);
  void OnShowCutVectors(wxCommandEvent &event);
  void OnShowSmoothCenterLine(wxCommandEvent &event);

  void OnShowEmaPoints(wxCommandEvent &event);
  void OnEditEmaPoints(wxCommandEvent &event);

  void OnLoadBackgroundImage(wxCommandEvent &event);
  void OnCurrentImageToBackground(wxCommandEvent& event);
  void OnShowBackgroundImage(wxCommandEvent &event);
  void OnBackgroundImageEditingMode(wxCommandEvent &event);

  // **************************************************************************
  // Declare the event table.
  // **************************************************************************

  DECLARE_EVENT_TABLE()
};


#endif
