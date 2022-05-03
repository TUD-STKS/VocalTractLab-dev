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

#ifndef _MAIN_WINDOW_H_
#define _MAIN_WINDOW_H_

#include <wx/wx.h>
#include <wx/notebook.h>
#include <wx/filename.h>
#include "SignalPage.h"
#include "VocalTractPage.h"
#include "TdsPage.h"
#include "GesturalScorePage.h"
#include "Acoustic3dPage.h"						   

#include "VocalTractShapesDialog.h"
#include "PhoneticParamsDialog.h"
#include "AnatomyParamsDialog.h"
#include "TdsOptionsDialog.h"
#include "FdsOptionsDialog.h"
#include "LfPulseDialog.h"
#include "SpectrumOptionsDialog.h"
#include "GlottisDialog.h"
#include "VocalTractDialog.h"
#include "AnalysisResultsDialog.h"
#include "AnnotationDialog.h"
#include "PoleZeroDialog.h"
#include "TransitionDialog.h"
#include "ParamSimu3DDialog.h"

#include "Data.h"

// ****************************************************************************
// Main window of the application.
// ****************************************************************************

class MainWindow : public wxFrame
{
  // **************************************************************************
  // Public data.
  // **************************************************************************


  // **************************************************************************
  // Public functions.
  // **************************************************************************

public:
  MainWindow();
  void initWidgets();
  void updateWidgets();
    
  // ****************************************************************************
  // Public data.
  // **************************************************************************

public:
  static const int NUM_TOOLBAR_TOOLS = 2;

  // Notebook and pages in that notebook
  wxNotebook *notebook;
  SignalPage *signalPage;
  VocalTractPage *vocalTractPage;
  TdsPage *tdsPage;
  GesturalScorePage *gesturalScorePage;
  Acoustic3dPage *acoustic3dPage;								 

  wxToolBar *toolBar;
  wxMenuBar *menuBar;
  wxMenu *clearContextMenu;
  wxStatusBar *statusBar;

  wxButton *btnTest;

  wxFileName audioFileName;
  wxFileName exportFileName;
  Data *data;
  Acoustic3dSimulation* simu3d;

  // **************************************************************************
  // Private functions.
  // **************************************************************************

private:
  bool saveWaveformAsTxtFile(const wxString &fileName, Signal16 *signal, int pos, int length);
  bool saveAreaFunction(const wxString &fileName);
  bool saveSpectrum(const wxString &fileName, ComplexSignal *spectrum);

  // Window events
  void OnCloseWindow(wxCloseEvent &event);
  void OnPageChanged(wxNotebookEvent &event);

  // Menu functions
  void OnLoadWavEgg(wxCommandEvent &event);
  void OnSaveWavEgg(wxCommandEvent &event);
  void OnLoadWav(wxCommandEvent &event);
  void OnSaveWav(wxCommandEvent &event);
  void OnSaveWavAsTxt(wxCommandEvent &event);
  void OnLoadSegmentSequence(wxCommandEvent &event);
  void OnSaveSegmentSequence(wxCommandEvent &event);
  void OnImportSegmentSequenceEsps(wxCommandEvent &event);
  void OnLoadGesturalScore(wxCommandEvent &event);
  void OnSaveGesturalScore(wxCommandEvent &event);
  void OnLoadSpeaker(wxCommandEvent &event);
  void OnSaveSpeaker(wxCommandEvent &event);
  void OnExit(wxCommandEvent &event);

  void OnSetAudioZero(wxCommandEvent &event);
  void OnNormalizeAmplitude(wxCommandEvent &event);
  void OnScaleAmplitudeUp(wxCommandEvent &event);
  void OnScaleAmplitudeDown(wxCommandEvent &event);


  void OnReduceConsonantVariability(wxCommandEvent &event);

  void OnChangeGesturalScoreF0Offset(wxCommandEvent &event);
  void OnChangeGesturalScoreF0Range(wxCommandEvent &event);
  void OnChangeGesturalScoreF0TargetSlopes(wxCommandEvent &event);
  void OnSubstituteGesturalScoreGlottalShapes(wxCommandEvent &event);
  void OnChangeGesturalScoreSubglottalPressure(wxCommandEvent &event);
  void OnGetGesturalScoreF0Statistics(wxCommandEvent &event);
  void OnChangeGesturalScoreDuration(wxCommandEvent &event);
  void OnChangeGesturalScoreTimeConstants(wxCommandEvent &event);
  
  void OnExportAreaFunction(wxCommandEvent &event);
  void OnExportCrossSections(wxCommandEvent &event);
  void OnExportVocalTractToSTL(wxCommandEvent& event);													  
  void OnExportModelSvg(wxCommandEvent &event);
  void OnExportWireframeModelSvg(wxCommandEvent &event);
  void OnExportModelObj(wxCommandEvent &event);
  void OnExportContourSvg(wxCommandEvent &event);
  void OnExportPrimarySpectrum(wxCommandEvent &event);
  void OnExportSecondarySpectrum(wxCommandEvent &event);
  void OnExportEmaTrajectories(wxCommandEvent &event);
  void OnExportVocalTractVideoFrames(wxCommandEvent &event);
  void OnExportTransferFunctionsFromScore(wxCommandEvent &event);
  void OnExportCrossSectionsFromScore(wxCommandEvent& event);

  void OnShowVocalTractDialog(wxCommandEvent &event);
  void OnShowVocalTractShapes(wxCommandEvent &event);
  void OnShowPhoneticParameters(wxCommandEvent &event);
  void OnShowAnatomyParameters(wxCommandEvent &event);
  void OnShowFdsOptionsDialog(wxCommandEvent &event);
  void OnShowTdsOptionsDialog(wxCommandEvent &event);
  void OnShowVocalFoldDialog(wxCommandEvent &event);
  void OnShowLfModelDialog(wxCommandEvent &event);

  void OnGesturalScoreFileToAudio(wxCommandEvent &event);
  void OnTubeSequenceFileToAudio(wxCommandEvent &event);
  void OnTractSequenceFileToAudio(wxCommandEvent &event);
  void OnGesturalScoreToTubeSequenceFile(wxCommandEvent &event);
  void OnGesturalScoreToTractSequenceFile(wxCommandEvent &event);
  
  void OnHertzToSemitones(wxCommandEvent &event);

  void OnAbout(wxCommandEvent &event);

  // Toolbar functions
  void OnRecord(wxCommandEvent &event);
  void OnPlayAll(wxCommandEvent &event);
  void OnPlayPart(wxCommandEvent &event);
  void OnClearItems(wxCommandEvent &event);

  // Individual clear functions
  void OnClearAll(wxCommandEvent &event);
  void OnClearMainTrack(wxCommandEvent &event);
  void OnClearEggTrack(wxCommandEvent &event);
  void OnClearExtraTrack(wxCommandEvent &event);
  void OnClearAudioTracks(wxCommandEvent &event);
  void OnClearGesturalScore(wxCommandEvent &event);
  void OnClearSegmentSequence(wxCommandEvent &event);
  void OnClearAnalysisTracks(wxCommandEvent &event);
  
  // Special shortcuts
  void OnKeyCtrlLeft(wxCommandEvent &event);
  void OnKeyCtrlRight(wxCommandEvent &event);
  void OnKeyShiftLeft(wxCommandEvent &event);
  void OnKeyShiftRight(wxCommandEvent &event);
  void OnKeyCtrlLess(wxCommandEvent &event);
  void OnKeyCtrlDelete(wxCommandEvent &event);
  void OnKeyCtrlInsert(wxCommandEvent &event);
  void OnKeyF6(wxCommandEvent& event);
  void OnKeyF7(wxCommandEvent &event);
  void OnKeyF8(wxCommandEvent &event);
  void OnKeyF9(wxCommandEvent &event);
  void OnKeyF11(wxCommandEvent &event);
  void OnKeyF12(wxCommandEvent &event);

  // ****************************************************************************
  // Declare the event table right at the end
  // ****************************************************************************

  DECLARE_EVENT_TABLE()
};

#endif
