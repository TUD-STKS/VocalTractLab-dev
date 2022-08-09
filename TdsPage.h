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

#ifndef _TDS_PAGE_H_
#define _TDS_PAGE_H_

#include <wx/wx.h>
#include <wx/frame.h>
#include <wx/spinctrl.h>
#include <wx/progdlg.h>

#include "TdsTimeSignalPicture.h"
#include "TdsSpatialSignalPicture.h"
#include "TdsTubePicture.h"
#include "Data.h"
#include "SynthesisThread.h"

#include "LfPulseDialog.h"
#include "TdsOptionsDialog.h"
#include "GlottisDialog.h"


// ****************************************************************************
// ****************************************************************************

class TdsPage : wxPanel
{
  // ****************************************************************
  // Public data.
  // ****************************************************************

public:
  Data *data;

  // ****************************************************************
  // Public functions.
  // ****************************************************************

public:
  TdsPage(wxWindow *parent);
  void updateWidgets();

  // ****************************************************************
  // Private data.
  // ****************************************************************

private:
  TdsTimeSignalPicture *tdsTimeSignalPicture;
  TdsSpatialSignalPicture *tdsSpatialSignalPicture;
  TdsTubePicture *tdsTubePicture;

  wxRadioBox *radSignal;
  wxCheckBox *chkShowMainPath;
  wxCheckBox *chkShowSidePaths;

  wxStaticText *labSynthesisPos;
  wxSpinCtrl *ctrlSynthesisSpeed;
  wxCheckBox *chkShowAnimation;
  wxButton *btnStartSynthesis;
  wxRadioBox *radSynthesisType;
  wxCheckBox *chkConstantF0;

  wxCheckBox *chkShowAreaFunctionArticulators;

  wxScrollBar *scrTime;
  wxStaticText *labTime;

  wxGenericProgressDialog *progressDialog;
  SynthesisThread *synthesisThread;

  // ****************************************************************
  // Private functions.
  // ****************************************************************

private:
  void initVars();
  void initWidgets();

  void OnUpdateRequest(wxCommandEvent &event);

  // Event from the synthesis thread
  void OnSynthesisThreadEvent(wxCommandEvent& event);

  // Left side panel controls
  void OnResetSynthesis(wxCommandEvent &event);
  void OnStartSynthesis(wxCommandEvent &event);
  void OnSynthesisSpeedChanged(wxSpinEvent &event);
  void OnShowAnimation(wxCommandEvent &event);
  void OnSelectSynthesisType(wxCommandEvent &event);
  void OnConstantF0(wxCommandEvent &event);
  void OnSetAreaFunction(wxCommandEvent &event);
  void OnSetUniformTube(wxCommandEvent &event);
  void OnGlottisDialog(wxCommandEvent &event);
  void OnLfWaveform(wxCommandEvent &event);
  void OnTdsAcoustics(wxCommandEvent &event);
  void OnTriangularGlottisF0Params(wxCommandEvent &event);
  void OnShowAreaFunctionArticulators(wxCommandEvent &event);

  // Right main panel controls
  void OnSelectSignal(wxCommandEvent &event);
  void OnShowMainPath(wxCommandEvent &event);
  void OnShowSidePaths(wxCommandEvent &event);
  void OnSignalUpperLimitPlus(wxCommandEvent &event);
  void OnSignalUpperLimitMinus(wxCommandEvent &event);
  void OnSignalLowerLimitPlus(wxCommandEvent &event);
  void OnSignalLowerLimitMinus(wxCommandEvent &event);
  void OnAreaUpperLimitPlus(wxCommandEvent &event);
  void OnAreaUpperLimitMinus(wxCommandEvent &event);
  void OnAreaLowerLimitPlus(wxCommandEvent &event);
  void OnAreaLowerLimitMinus(wxCommandEvent &event);

  void OnTimeScrollbarChanged(wxScrollEvent &event);

  // ****************************************************************************
  // Declare the event table right at the end
  // ****************************************************************************

  DECLARE_EVENT_TABLE()
};

#endif
