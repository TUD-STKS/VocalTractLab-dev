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

#ifndef _GESTURAL_SCORE_PAGE_H_
#define _GESTURAL_SCORE_PAGE_H_

#include <wx/wx.h>
#include <wx/frame.h>
#include <wx/splitter.h>
#include <wx/laywin.h>
#include <wx/progdlg.h>

#include "SignalComparisonPicture.h"
#include "GesturalScorePicture.h"
#include "TimeAxisPicture.h"
#include "AnalysisSettingsDialog.h"
#include "GlottisDialog.h"
#include "VocalTractDialog.h"
#include "TdsOptionsDialog.h"
#include "Data.h"
#include "SynthesisThread.h"


// ****************************************************************************
// ****************************************************************************

class GesturalScorePage : public wxPanel
{
  // ****************************************************************
  // Public data.
  // ****************************************************************

public:


  // ****************************************************************
  // Public functions.
  // ****************************************************************

public:
  GesturalScorePage(wxWindow *parent);
  void initWidgets();
  void updateWidgets();
  void scrollLeft();
  void scrollRight();
  void selectNextLeftSegment();
  void selectNextRightSegment();
  void togglePicture();
  void lengthenGesturalScore();
  void shortenGesturalScore();

  // ****************************************************************
  // Private data.
  // ****************************************************************

private:
  Data *data;

  wxGenericProgressDialog *progressDialog;
  SynthesisThread *synthesisThread;

  wxSplitterWindow *splitter;
  wxScrollBar *horzScrollBar;
  wxScrollBar *scrLowerOffset;
  wxScrollBar *scrUpperOffset;
  
  // Gesture box controls.
  wxStaticBox *gestureBox;
  wxCheckBox *chkNeutralGesture;
  wxStaticText *labValue;
  wxComboBox *lstNominalValue;
  wxTextCtrl *txtContinualValue;
  wxStaticText *labSlope;
  wxScrollBar *scrSlope;
  wxStaticText *labStartTime;
  wxSpinCtrl *txtDuration;
  wxStaticText *labTimeConstant;
  wxScrollBar *scrTimeConstant;
  
  // Ungrouped controls
  wxStaticText *labMark;
  wxRadioBox *radDisplayChoices;

  // Audio signal display options
  wxCheckBox *chkShowExtraTrack;
  wxCheckBox *chkShowSonagram;
  wxCheckBox *chkShowSegmentation;
  wxCheckBox *chkShowModelF0Curve;

  wxCheckBox *chkShowAnimation;
  wxCheckBox* chkNormalizeAmplitude;

  GesturalScorePicture *gesturalScorePicture;
  SignalComparisonPicture *signalComparisonPicture;
  TimeAxisPicture *timeAxisPicture;

  // ****************************************************************
  // Private functions.
  // ****************************************************************

private:
  void fillGestureValueList(bool forced = false);

  void OnUpdateRequest(wxCommandEvent &event);

  void OnHorzScrollBar(wxScrollEvent &event);
  void OnLowerOffsetScrollBar(wxScrollEvent &event);
  void OnUpperOffsetScrollBar(wxScrollEvent &event);
  void OnSplitterPosChanged(wxSplitterEvent &event);

  void OnNominalValueSelected(wxCommandEvent &event);
  void OnNominalValueEntered(wxCommandEvent &event);
  void OnNeutralGesture(wxCommandEvent &event);
  void OnContinualValueChanged(wxFocusEvent &event);
  void OnContinualValueEntered(wxCommandEvent &event);
  void OnDurationChanged(wxSpinEvent &event);
  void OnDurationEntered(wxCommandEvent &event);
  void OnTimeConstantChanged(wxScrollEvent &event);
  void OnSlopeChanged(wxScrollEvent &event);
  
  void OnTimeZoomMinus(wxCommandEvent &event);
  void OnTimeZoomPlus(wxCommandEvent &event);
  void OnDisplayChoice(wxCommandEvent &event);

  void OnShowMainTrack(wxCommandEvent &event);
  void OnShowExtraTrack(wxCommandEvent &event);
  void OnShowSonagram(wxCommandEvent &event);
  void OnShowSegmentation(wxCommandEvent &event);
  void OnShowModelF0Curve(wxCommandEvent &event);

  void OnShowAnimation(wxCommandEvent &event);
  void OnNormalizeAmplitude(wxCommandEvent& event);
  void OnSynthesize(wxCommandEvent &event);
  
  // Events from the synthesis thread
  void OnSynthesisThreadEvent(wxCommandEvent& event);

  void OnAcoustics(wxCommandEvent &event);
  void OnShowVocalTract(wxCommandEvent &event);
  void OnShowGlottis(wxCommandEvent &event);
  void OnAnalysisSettings(wxCommandEvent &event);
  void OnCalcF0(wxCommandEvent &event);
  void OnCalcVoiceQuality(wxCommandEvent &event);
  void OnAnalysisResults(wxCommandEvent &event);
  void OnAnnotationDialog(wxCommandEvent &event);

  void OnResize(wxSizeEvent& event);

  // ****************************************************************************
  // Declare the event table right at the end
  // ****************************************************************************

  DECLARE_EVENT_TABLE()
};

#endif
