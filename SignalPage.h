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

#ifndef _SIGNAL_PAGE_H_
#define _SIGNAL_PAGE_H_

#include <wx/wx.h>
#include <wx/frame.h>
#include "Data.h"
#include "SignalPicture.h"
#include "SpectrogramPicture.h"
#include "SimpleSpectrumPicture.h"
#include "AnalysisSettingsDialog.h"
#include "AnalysisResultsDialog.h"


// ****************************************************************************
// ****************************************************************************

class SignalPage : wxPanel
{
  // ****************************************************************
  // Public data.
  // ****************************************************************

public:


  // ****************************************************************
  // Public functions.
  // ****************************************************************

public:
  SignalPage(wxWindow *parent);
  void updateWidgets();
  void scrollLeft();
  void scrollRight();

  // ****************************************************************
  // Private data.
  // ****************************************************************

private:
  Data *data;

  SignalPicture *picSignal;
  SpectrogramPicture *picSpectrogram;
  SimpleSpectrumPicture *picSimpleSpectrum;
  wxScrollBar *scrTime;

  wxCheckBox *chkShowMainTrack;
  wxCheckBox *chkShowEggTrack;
  wxCheckBox *chkShowExtraTrack;
  wxStaticText *labHorzZoom;
  wxStaticText *labVertZoom;

  wxRadioBox *radSpectrogramChoice;
  wxCheckBox *chkShowSpectrogramText;


  // ****************************************************************
  // Private functions.
  // ****************************************************************

private:
  void initVars();
  void initWidgets();

  void OnUpdateRequest(wxCommandEvent &event);

  void OnScrollEvent(wxScrollEvent &event);
  void OnShowMainTrackClicked(wxCommandEvent &event);
  void OnShowEggTrackClicked(wxCommandEvent &event);
  void OnShowExtraTrackClicked(wxCommandEvent &event);
  void OnOsziHorzZoomPlusClicked(wxCommandEvent &event);
  void OnOsziHorzZoomMinusClicked(wxCommandEvent &event);
  void OnOsziVertZoomPlusClicked(wxCommandEvent &event);
  void OnOsziVertZoomMinusClicked(wxCommandEvent &event);
  void OnExchangeMainEggClicked(wxCommandEvent &event);
  void OnExchangeMainExtraClicked(wxCommandEvent &event);
  
  void OnSpectrogramChoice(wxCommandEvent &event);
  void OnShowSpectrogramText(wxCommandEvent &event);
  
  void OnAnalysisSettings(wxCommandEvent &event);
  void OnAnalysisResults(wxCommandEvent &event);
  void OnCalcF0(wxCommandEvent &event);
  void OnCalcVoiceQuality(wxCommandEvent &event);

  // ****************************************************************************
  // Declare the event table right at the end
  // ****************************************************************************

  DECLARE_EVENT_TABLE()
};

#endif
