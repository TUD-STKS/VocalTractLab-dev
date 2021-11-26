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

#ifndef __ANALYSIS_SETTINGS_DIALOG_H__
#define __ANALYSIS_SETTINGS_DIALOG_H__

#include <wx/wx.h>
#include <wx/dialog.h>
#include <wx/notebook.h>

#include "Data.h"
#include "SpectrogramPlot.h"
#include "VocalTractLabBackend/F0EstimatorYin.h"
#include "VocalTractLabBackend/VoiceQualityEstimator.h"


// ****************************************************************************
/// This dialog lets the user adjust the parameters for the different 
/// analysis algorithms, e.g., for F0, formants, spectrogram.
// ****************************************************************************

class AnalysisSettingsDialog : public wxDialog
{
  // **************************************************************************
  // Public functions.
  // **************************************************************************

public:
  AnalysisSettingsDialog(wxWindow *parent, wxWindow *updateEventReceiver, SpectrogramPlot *plot);

  // **************************************************************************
  // Private data.
  // **************************************************************************

private:

  Data *data;
  SpectrogramPlot *spectrogramPlot;
  wxWindow *updateEventReceiver;

  // The notebook widget and its pages

  wxNotebook *notebook;
  wxPanel *spectrumPage;
  wxPanel *spectrogramPage;
  wxPanel *f0Page;
  wxPanel *voiceQualityPage;

  // The spectrum page controls

  wxScrollBar *scrSpectrumWindowLength;
  wxStaticText *labSpectrumWindowLength;
  wxRadioBox *radSpectrumType;
  wxTextCtrl *txtAverageSpectrumTimeStep;

  // The spectrogram page controls

  wxScrollBar *scrSpectrogramWindowLength;
  wxStaticText *labSpectrogramWindowLength;
  wxScrollBar *scrDynamicRange;
  wxStaticText *labDynamicRange;
  wxScrollBar *scrViewRange;
  wxStaticText *labViewRange;
  wxScrollBar *scrFftLength;
  wxStaticText *labFftLength;

  // The F0 page controls

  wxCheckBox *chkShowF0;
  wxTextCtrl *txtF0Threshold;
  wxTextCtrl *txtF0TimeStep;

  // The voice quality page controls

  wxCheckBox *chkShowVoiceQuality;


  // **************************************************************************
  // Private functions.
  // **************************************************************************

private:
  void initWidgets();
  wxPanel *initSpectrumPage(wxWindow *parent);
  wxPanel *initSpectrogramPage(wxWindow *parent);
  wxPanel *initF0Page(wxWindow *parent);
  wxPanel *initVoiceQualityPage(wxWindow *parent);

  void updateWidgets();
  void updateSpectrumPage();
  void updateSpectrogramPage();
  void updateF0Page();
  void updateVoiceQualityPage();

  // Event handler for notebook page changed

  void OnePageChanged(wxNotebookEvent &event);

  // Event handler for spectrum settings

  void OnSpectrumWindowLengthChanged(wxScrollEvent &event);
  void OnSpectrumTypeChanged(wxCommandEvent &event);
  void OnCalcAverageSpectrum(wxCommandEvent &event);
  void OnSpectrumTimeStepLostFocus(wxFocusEvent &event);

  // Event handler for spectrogram settings

  void OnSpectrogramWindowLengthChanged(wxScrollEvent &event);
  void OnDynamicRangeChanged(wxScrollEvent &event);
  void OnViewRangeChanged(wxScrollEvent &event);
  void OnFftLengthChanged(wxScrollEvent &event);
  void OnSetSpectrogramDefaults(wxCommandEvent &event);

  // Event handler for F0 settings
  
  void OnShowF0(wxCommandEvent &event);
  void OnThresholdLostFocus(wxFocusEvent &event);
  void OnF0TimeStepLostFocus(wxFocusEvent &event);
  void OnCalcF0(wxCommandEvent &event);

  // Event handler for voice quality settings
  
  void OnShowVoiceQuality(wxCommandEvent &event);
  void OnCalcVoiceQuality(wxCommandEvent &event);

  // **************************************************************************
  // Declare the event table.
  // **************************************************************************

  DECLARE_EVENT_TABLE()
};


#endif
