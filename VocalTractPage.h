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

#ifndef _VOCALTRACT_PAGE_H_
#define _VOCALTRACT_PAGE_H_

#include <wx/wx.h>
#include <wx/frame.h>
#include <wx/splitter.h>
#include <wx/laywin.h>

#include "VocalTractPicture.h"
#include "AreaFunctionPicture.h"
#include "CrossSectionPicture.h"
#include "SpectrumPicture.h"
#include "Data.h"

#include "LfPulseDialog.h"
#include "FdsOptionsDialog.h"
#include "VocalTractShapesDialog.h"
#include "PhoneticParamsDialog.h"
#include "PoleZeroDialog.h"


// ****************************************************************************
// ****************************************************************************

class VocalTractPage : public wxPanel
{
  // ****************************************************************
  // Public functions.
  // ****************************************************************

public:
  VocalTractPage(wxWindow *parent, VocalTractPicture *picVocalTract);
  void updateWidgets();

  // ****************************************************************
  // Private data.
  // ****************************************************************

private:
  Data *data;
  static const int NUM_TONGUE_SIDE_POINTS = 4;
  static const int NUM_MIN_AREAS = 3;

  wxSplitterWindow *splitter;

  AreaFunctionPicture *picAreaFunction;
  CrossSectionPicture *picCrossSection;
  SpectrumPicture *picSpectrum;

  // Left side control

  wxCheckBox *chkShowControlPoints;
  wxCheckBox *chkShowCenterLine;
  wxRadioButton *radRoughCenterLine;
  wxRadioButton *radSmoothCenterLine;
  wxCheckBox *chkShowCutVectors;

  wxStaticText *labBackgroundImageFileName;
  wxCheckBox *chkShowBackgroundImage;
  wxCheckBox *chkBackgroundImageEditMode;

  // Vocal tract variables controls

  wxScrollBar *scrTongueSide[NUM_TONGUE_SIDE_POINTS];
  wxStaticText *labTongueSide[NUM_TONGUE_SIDE_POINTS];
  
  wxScrollBar *scrMinArea[NUM_MIN_AREAS];
  wxStaticText *labMinArea[NUM_MIN_AREAS];

  wxRadioButton *radNoTract;
  wxRadioButton *rad2dTract;
  wxRadioButton *rad3dTract;
  wxRadioButton *radWireFrameTract;
  wxCheckBox *chkBothSides;

  wxCheckBox *chkDiscreteAreaFunction;
  wxCheckBox *chkAreaCircumference;
  wxCheckBox *chkShowBranches;
  wxCheckBox *chkShowAreaFunctionText;
  wxCheckBox *chkShowAreaFunctionArticulators;

  // Spectrum-related controls

  wxScrollBar *scrSpectrumOffset;

  wxCheckBox *chkShowModelSpectrum;
  wxCheckBox *chkShowUserSpectrum;
  wxCheckBox *chkShowTdsSpectrum;
  wxCheckBox *chkShowPoleZeroSpectrum;
  wxCheckBox *chkShowFormants;
  wxCheckBox *chkShowMagnitude;
  wxCheckBox *chkShowPhase;

  wxStaticText *labFrequencyRange;
  wxStaticText *labNoiseFilterCutoff;


  // ****************************************************************
  // Private functions.
  // ****************************************************************

private:
  void initVars();
  void initWidgets(VocalTractPicture *picVocalTract);

  void createSpecialVocalTractShapes();

  void OnUpdateRequest(wxCommandEvent &event);

  // Event handers for controls at the left side

  void OnVocalTractDialog(wxCommandEvent &event);
  void OnShapesDialog(wxCommandEvent &event);
  void OnPhoneticParamsDialog(wxCommandEvent &event);
  void OnLfPulse(wxCommandEvent &event);
  void OnFdsAcoustics(wxCommandEvent &event);
  void OnImproveFormants(wxCommandEvent &event);
  void OnCheckVowelShape(wxCommandEvent &event);
  void OnGetFormantError(wxCommandEvent &event);
  
  void OnEditPoleZeroPlan(wxCommandEvent &event);
  void OnTest(wxCommandEvent &event);
  void OnMapShape(wxCommandEvent &event);
  void OnTransitionDialog(wxCommandEvent &event);

  void OnPlayShortVowelTds(wxCommandEvent& event);
  void OnPlayLongVowelTds(wxCommandEvent& event);
  void OnPlayFricativeTds(wxCommandEvent& event);

  void OnPlayShortVowelFds(wxCommandEvent &event);
  void OnPlayLongVowelFds(wxCommandEvent &event);

  // Event handers for controls at the spectrum side

  void OnFrequencyRangeMinus(wxCommandEvent &event);
  void OnFrequencyRangePlus(wxCommandEvent &event);
  void OnNoiseFilterCutoffMinus(wxCommandEvent &event);
  void OnNoiseFilterCutoffPlus(wxCommandEvent &event);
  void OnSynthesizeNoise(wxCommandEvent &event);
  void OnUpperSpectrumLimitPlus(wxCommandEvent &event);
  void OnUpperSpectrumLimitMinus(wxCommandEvent &event);
  void OnLowerSpectrumLimitPlus(wxCommandEvent &event);
  void OnLowerSpectrumLimitMinus(wxCommandEvent &event);

  void OnShowModelSpectrum(wxCommandEvent &event);
  void OnShowUserSpectrum(wxCommandEvent &event);
  void OnShowTdsSpectrum(wxCommandEvent &event);
  void OnShowPoleZeroSpectrum(wxCommandEvent &event);
  void OnShowFormants(wxCommandEvent &event);
  void OnShowMagnitude(wxCommandEvent &event);
  void OnShowPhase(wxCommandEvent &event);
  void OnSpectrumOffsetChanged(wxScrollEvent &event);

  // Event handers for controls in the main panel

  void OnShowDiscreteAreas(wxCommandEvent &event);
  void OnShowAreas(wxCommandEvent &event);
  void OnShowBranches(wxCommandEvent &event);
  void OnShowText(wxCommandEvent &event);
  void OnShowAreaFunctionArticulators(wxCommandEvent &event);

  // ****************************************************************************
  // Declare the event table right at the end
  // ****************************************************************************

  DECLARE_EVENT_TABLE()
};

#endif
