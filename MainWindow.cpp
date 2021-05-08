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

#include "MainWindow.h"
#include "IconsXpm.h"
#include "SilentMessageBox.h"
#include "Backend/SoundLib.h"
#include "Backend/AudioFile.h"
#include "Backend/Dsp.h"
#include "Backend/TimeFunction.h"
#include "Backend/XmlNode.h"
#include "Backend/Synthesizer.h"

#include <iostream>
#include <wx/choicdlg.h>
#include <wx/clipbrd.h>
#include <wx/busyinfo.h>

using namespace std;

// ****************************************************************************
// IDs.
// ****************************************************************************

// Toolbar
static const int IDB_TOOLBAR_PLAY_ALL       = 1100;
static const int IDB_TOOLBAR_PLAY_PART      = 1101;
static const int IDB_TOOLBAR_RECORD         = 1102;
static const int IDB_TOOLBAR_CLEAR          = 1103;
static const int ID_NOTEBOOK                = 1104;

// Menu
static const int IDM_LOAD_WAV_EGG           = 1200;
static const int IDM_SAVE_WAV_EGG           = 1201;
static const int IDM_LOAD_WAV               = 1203;
static const int IDM_SAVE_WAV               = 1204;
static const int IDM_SAVE_WAV_AS_TXT        = 1205;
static const int IDM_LOAD_SEGMENT_SEQUENCE  = 1208;
static const int IDM_SAVE_SEGMENT_SEQUENCE  = 1209;
static const int IDM_IMPORT_SEGMENT_SEQUENCE_ESPS = 1210;
static const int IDM_LOAD_GESTURAL_SCORE    = 1220;
static const int IDM_SAVE_GESTURAL_SCORE    = 1221;
static const int IDM_LOAD_SPEAKER           = 1222;
static const int IDM_SAVE_SPEAKER           = 1223;

static const int IDM_SET_AUDIO_ZERO         = 2500;
static const int IDM_NORMALIZE_AMPLITUDE    = 2501;
static const int IDM_SCALE_AMPLITUDE_UP     = 2502;
static const int IDM_SCALE_AMPLITUDE_DOWN   = 2503;
static const int IDM_REDUCE_CONSONANT_VARIABILITY = 2504;

static const int IDM_CHANGE_GESTURAL_SCORE_F0_OFFSET = 2510;
static const int IDM_CHANGE_GESTURAL_SCORE_F0_RANGE = 2511;
static const int IDM_CHANGE_GESTURAL_SCORE_F0_TARGET_SLOPES = 2513;
static const int IDM_SUBSTITUTE_GESTURAL_SCORE_GLOTTAL_SHAPES = 2514;
static const int IDM_CHANGE_GESTURAL_SCORE_SUBGLOTTAL_PRESSURE = 2515;
static const int IDM_GET_GESTURAL_SCORE_F0_STATISTICS = 2516;
static const int IDM_CHANGE_GESTURAL_SCORE_DURATION = 2517;
static const int IDM_CHANGE_GESTURAL_SCORE_TIME_CONSTANTS = 2518;

static const int IDM_EXPORT_AREA_FUNCTION   = 1230;
static const int IDM_EXPORT_CROSS_SECTIONS = 1231;
static const int IDM_EXPORT_VT_MODEL_SVG = 1232;
static const int IDM_EXPORT_WIREFRAME_MODEL_SVG = 1233;
static const int IDM_EXPORT_VT_MODEL_OBJ    = 1234;
static const int IDM_EXPORT_VT_CONTOUR_SVG  = 1235;
static const int IDM_EXPORT_PRIMARY_SPECTRUM = 1236;
static const int IDM_EXPORT_SECONDARY_SPECTRUM = 1237;
static const int IDM_EXPORT_EMA_TRAJECTORIES = 1238;
static const int IDM_EXPORT_VIDEO_FRAMES     = 1239;
static const int IDM_EXPORT_TRANSFER_FUNCTIONS_FROM_SCORE = 1241;

static const int IDM_SHOW_VOCAL_TRACT_DIALOG  = 1250;
static const int IDM_SHOW_VOCAL_TRACT_SHAPES  = 1251;
static const int IDM_SHOW_PHONETIC_PARAMS     = 1252;
static const int IDM_SHOW_ANATOMY_PARAMS      = 1253;
static const int IDM_SHOW_FDS_OPTIONS_DIALOG  = 1254;
static const int IDM_SHOW_TDS_OPTIONS_DIALOG  = 1255;
static const int IDM_SHOW_VOCAL_FOLD_DIALOG   = 1256;
static const int IDM_SHOW_LF_MODEL_DIALOG     = 1257;

static const int IDM_GESTURAL_SCORE_FILE_TO_AUDIO = 1260;
static const int IDM_TUBE_SEQUENCE_FILE_TO_AUDIO = 1261;
static const int IDM_TRACT_SEQUENCE_FILE_TO_AUDIO = 1262;
static const int IDM_GESTURAL_SCORE_TO_TUBE_SEQUENCE_FILE = 1263;
static const int IDM_GESTURAL_SCORE_TO_TRACT_SEQUENCE_FILE = 1264;

static const int IDM_HERTZ_TO_SEMITONES     = 1270;  

// Context menu to clear different things
static const int IDM_CLEAR_ALL              = 1300;
static const int IDM_CLEAR_MAIN_TRACK       = 1301;
static const int IDM_CLEAR_EGG_TRACK        = 1302;
static const int IDM_CLEAR_EXTRA_TRACK      = 1303;
static const int IDM_CLEAR_ALL_TRACKS       = 1304;
static const int IDM_CLEAR_GESTURAL_SCORE   = 1305;
static const int IDM_CLEAR_SEGMENT_SEQUENCE = 1306;
static const int IDM_CLEAR_ANALYSIS_TRACKS  = 1307;

// Accelerator keys
static const int IDK_CTRL_LEFT              = 1900;
static const int IDK_CTRL_RIGHT             = 1901;
static const int IDK_SHIFT_LEFT             = 1902;
static const int IDK_SHIFT_RIGHT            = 1903;
static const int IDK_CTRL_LESS              = 1904;
static const int IDK_CTRL_DELETE            = 1905;
static const int IDK_CTRL_INSERT            = 1906;
static const int IDK_F6                     = 1908;
static const int IDK_F7                     = 1909;
static const int IDK_F8                     = 1910;
static const int IDK_F9                     = 1911;
static const int IDK_F11                    = 1912;
static const int IDK_F12                    = 1913;


// ****************************************************************************
// The event table.
// ****************************************************************************

BEGIN_EVENT_TABLE(MainWindow, wxFrame)
  EVT_CLOSE(MainWindow::OnCloseWindow)
  EVT_NOTEBOOK_PAGE_CHANGED(ID_NOTEBOOK, MainWindow::OnPageChanged)

  EVT_MENU(IDM_LOAD_WAV_EGG, MainWindow::OnLoadWavEgg)
  EVT_MENU(IDM_SAVE_WAV_EGG, MainWindow::OnSaveWavEgg)
  EVT_MENU(IDM_LOAD_WAV, MainWindow::OnLoadWav)
  EVT_MENU(IDM_SAVE_WAV, MainWindow::OnSaveWav)
  EVT_MENU(IDM_SAVE_WAV_AS_TXT, MainWindow::OnSaveWavAsTxt)
  EVT_MENU(IDM_LOAD_SEGMENT_SEQUENCE, MainWindow::OnLoadSegmentSequence)
  EVT_MENU(IDM_SAVE_SEGMENT_SEQUENCE, MainWindow::OnSaveSegmentSequence)
  EVT_MENU(IDM_IMPORT_SEGMENT_SEQUENCE_ESPS, MainWindow::OnImportSegmentSequenceEsps)
  EVT_MENU(IDM_LOAD_GESTURAL_SCORE, MainWindow::OnLoadGesturalScore)
  EVT_MENU(IDM_SAVE_GESTURAL_SCORE, MainWindow::OnSaveGesturalScore)
  EVT_MENU(IDM_LOAD_SPEAKER, MainWindow::OnLoadSpeaker)
  EVT_MENU(IDM_SAVE_SPEAKER, MainWindow::OnSaveSpeaker)
  EVT_MENU(wxID_EXIT, MainWindow::OnExit)

  EVT_MENU(IDM_SET_AUDIO_ZERO, MainWindow::OnSetAudioZero)
  EVT_MENU(IDM_NORMALIZE_AMPLITUDE, MainWindow::OnNormalizeAmplitude)
  EVT_MENU(IDM_SCALE_AMPLITUDE_UP, MainWindow::OnScaleAmplitudeUp)
  EVT_MENU(IDM_SCALE_AMPLITUDE_DOWN, MainWindow::OnScaleAmplitudeDown)
  EVT_MENU(IDM_REDUCE_CONSONANT_VARIABILITY, MainWindow::OnReduceConsonantVariability)

  EVT_MENU(IDM_CHANGE_GESTURAL_SCORE_F0_OFFSET, MainWindow::OnChangeGesturalScoreF0Offset)
  EVT_MENU(IDM_CHANGE_GESTURAL_SCORE_F0_RANGE, MainWindow::OnChangeGesturalScoreF0Range)
  EVT_MENU(IDM_CHANGE_GESTURAL_SCORE_F0_TARGET_SLOPES, MainWindow::OnChangeGesturalScoreF0TargetSlopes)
  EVT_MENU(IDM_SUBSTITUTE_GESTURAL_SCORE_GLOTTAL_SHAPES, MainWindow::OnSubstituteGesturalScoreGlottalShapes)
  EVT_MENU(IDM_CHANGE_GESTURAL_SCORE_SUBGLOTTAL_PRESSURE, MainWindow::OnChangeGesturalScoreSubglottalPressure)
  EVT_MENU(IDM_GET_GESTURAL_SCORE_F0_STATISTICS, MainWindow::OnGetGesturalScoreF0Statistics)
  EVT_MENU(IDM_CHANGE_GESTURAL_SCORE_DURATION, MainWindow::OnChangeGesturalScoreDuration)
  EVT_MENU(IDM_CHANGE_GESTURAL_SCORE_TIME_CONSTANTS, MainWindow::OnChangeGesturalScoreTimeConstants)

  EVT_MENU(IDM_EXPORT_AREA_FUNCTION, MainWindow::OnExportAreaFunction)
  EVT_MENU(IDM_EXPORT_CROSS_SECTIONS, MainWindow::OnExportCrossSections)

  EVT_MENU(IDM_EXPORT_VT_MODEL_SVG, MainWindow::OnExportModelSvg)
  EVT_MENU(IDM_EXPORT_WIREFRAME_MODEL_SVG, MainWindow::OnExportWireframeModelSvg)
  EVT_MENU(IDM_EXPORT_VT_MODEL_OBJ, MainWindow::OnExportModelObj)
  EVT_MENU(IDM_EXPORT_VT_CONTOUR_SVG, MainWindow::OnExportContourSvg)
  EVT_MENU(IDM_EXPORT_PRIMARY_SPECTRUM, MainWindow::OnExportPrimarySpectrum)
  EVT_MENU(IDM_EXPORT_SECONDARY_SPECTRUM, MainWindow::OnExportSecondarySpectrum)
  EVT_MENU(IDM_EXPORT_EMA_TRAJECTORIES, MainWindow::OnExportEmaTrajectories)
  EVT_MENU(IDM_EXPORT_VIDEO_FRAMES, MainWindow::OnExportVocalTractVideoFrames)
  EVT_MENU(IDM_EXPORT_TRANSFER_FUNCTIONS_FROM_SCORE, MainWindow::OnExportTransferFunctionsFromScore)

  EVT_MENU(IDM_SHOW_VOCAL_TRACT_DIALOG, MainWindow::OnShowVocalTractDialog)
  EVT_MENU(IDM_SHOW_VOCAL_TRACT_SHAPES, MainWindow::OnShowVocalTractShapes)
  EVT_MENU(IDM_SHOW_PHONETIC_PARAMS, MainWindow::OnShowPhoneticParameters)
  EVT_MENU(IDM_SHOW_ANATOMY_PARAMS, MainWindow::OnShowAnatomyParameters)
  EVT_MENU(IDM_SHOW_FDS_OPTIONS_DIALOG, MainWindow::OnShowFdsOptionsDialog)
  EVT_MENU(IDM_SHOW_TDS_OPTIONS_DIALOG, MainWindow::OnShowTdsOptionsDialog)
  EVT_MENU(IDM_SHOW_VOCAL_FOLD_DIALOG, MainWindow::OnShowVocalFoldDialog)
  EVT_MENU(IDM_SHOW_LF_MODEL_DIALOG, MainWindow::OnShowLfModelDialog)

  EVT_MENU(IDM_GESTURAL_SCORE_FILE_TO_AUDIO, MainWindow::OnGesturalScoreFileToAudio)
  EVT_MENU(IDM_TUBE_SEQUENCE_FILE_TO_AUDIO, MainWindow::OnTubeSequenceFileToAudio)
  EVT_MENU(IDM_TRACT_SEQUENCE_FILE_TO_AUDIO, MainWindow::OnTractSequenceFileToAudio)
  EVT_MENU(IDM_GESTURAL_SCORE_TO_TUBE_SEQUENCE_FILE, MainWindow::OnGesturalScoreToTubeSequenceFile)
  EVT_MENU(IDM_GESTURAL_SCORE_TO_TRACT_SEQUENCE_FILE, MainWindow::OnGesturalScoreToTractSequenceFile)

  EVT_MENU(IDM_HERTZ_TO_SEMITONES, MainWindow::OnHertzToSemitones)

  EVT_MENU(wxID_ABOUT, MainWindow::OnAbout)

  // Toolbar
  EVT_MENU(IDB_TOOLBAR_PLAY_ALL, MainWindow::OnPlayAll)
  EVT_MENU(IDB_TOOLBAR_PLAY_PART, MainWindow::OnPlayPart)
  EVT_MENU(IDB_TOOLBAR_RECORD, MainWindow::OnRecord)
  EVT_MENU(IDB_TOOLBAR_CLEAR, MainWindow::OnClearItems)

  // Several clear events
  EVT_MENU(IDM_CLEAR_ALL, MainWindow::OnClearAll)
  EVT_MENU(IDM_CLEAR_MAIN_TRACK, MainWindow::OnClearMainTrack)
  EVT_MENU(IDM_CLEAR_EGG_TRACK, MainWindow::OnClearEggTrack)
  EVT_MENU(IDM_CLEAR_EXTRA_TRACK, MainWindow::OnClearExtraTrack)
  EVT_MENU(IDM_CLEAR_ALL_TRACKS, MainWindow::OnClearAudioTracks)
  EVT_MENU(IDM_CLEAR_GESTURAL_SCORE, MainWindow::OnClearGesturalScore)
  EVT_MENU(IDM_CLEAR_SEGMENT_SEQUENCE, MainWindow::OnClearSegmentSequence)
  EVT_MENU(IDM_CLEAR_ANALYSIS_TRACKS, MainWindow::OnClearAnalysisTracks)

  // Accelerator keys must be handled with a menu event
  EVT_MENU(IDK_CTRL_LEFT, MainWindow::OnKeyCtrlLeft)
  EVT_MENU(IDK_CTRL_RIGHT, MainWindow::OnKeyCtrlRight)
  EVT_MENU(IDK_SHIFT_LEFT, MainWindow::OnKeyShiftLeft)
  EVT_MENU(IDK_SHIFT_RIGHT, MainWindow::OnKeyShiftRight)
  EVT_MENU(IDK_CTRL_LESS, MainWindow::OnKeyCtrlLess)
  EVT_MENU(IDK_CTRL_DELETE, MainWindow::OnKeyCtrlDelete)
  EVT_MENU(IDK_CTRL_INSERT, MainWindow::OnKeyCtrlInsert)
  EVT_MENU(IDK_F6, MainWindow::OnKeyF6)
  EVT_MENU(IDK_F7, MainWindow::OnKeyF7)
  EVT_MENU(IDK_F8, MainWindow::OnKeyF8)
  EVT_MENU(IDK_F9, MainWindow::OnKeyF9)
  EVT_MENU(IDK_F11, MainWindow::OnKeyF11)
  EVT_MENU(IDK_F12, MainWindow::OnKeyF12)
END_EVENT_TABLE()


// ****************************************************************************
// ****************************************************************************

MainWindow::MainWindow()
{
  // Init the audio device
  
  printf("Trying to initialize sound ... ");
  initSound(SAMPLING_RATE);
  printf("done.\n");
  
  // ****************************************************************
  // Init the variables BEFORE the child widgets.
  // ****************************************************************

  audioFileName = wxFileName("");
  exportFileName = wxFileName("");
  data = Data::getInstance();

  // ****************************************************************
  // Do some unit testing.
  // ****************************************************************

//  Matrix::test();


  // ****************************************************************
  // Set window properties and init the widgets.
  // ****************************************************************

	wxFrame::Create(NULL, wxID_ANY, "VocalTractLab2", wxDefaultPosition, 
    wxDefaultSize, wxCLOSE_BOX | wxMINIMIZE_BOX | wxMAXIMIZE_BOX | 
    wxSYSTEM_MENU | wxCAPTION | wxRAISED_BORDER | wxRESIZE_BORDER);

  // Output the title of the main window.

  this->SetLabel("VocalTractLab2");

  // ****************************************************************
  // Init all widgets.
  // ****************************************************************

  // Init the vocal tract dialog before all other widgets/pages,
  // because the pointer to the vocal tract picture is needed
  // for the initialization of the pictures on the vocal tract page.

  VocalTractDialog *vocalTractDialog = VocalTractDialog::getInstance();

  initWidgets();

  // Make the main window double buffered to avoid any flickering
  // of the child-windows and during resizing.
  this->SetDoubleBuffered(true);

  this->Maximize();
}


// ****************************************************************************
// ****************************************************************************

void MainWindow::initWidgets()
{
  // ****************************************************************
  // Accellerator keys.
  // ****************************************************************

  int numAccels = 0;
  wxAcceleratorEntry entries[256];

  entries[numAccels++].Set(wxACCEL_CTRL, WXK_LEFT,  IDK_CTRL_LEFT);
  entries[numAccels++].Set(wxACCEL_CTRL, WXK_RIGHT, IDK_CTRL_RIGHT);
  entries[numAccels++].Set(wxACCEL_SHIFT, WXK_LEFT,  IDK_SHIFT_LEFT);
  entries[numAccels++].Set(wxACCEL_SHIFT, WXK_RIGHT, IDK_SHIFT_RIGHT);
  entries[numAccels++].Set(wxACCEL_CTRL, (int)'R',  IDB_TOOLBAR_RECORD);
  entries[numAccels++].Set(wxACCEL_CTRL, (int)'P',  IDB_TOOLBAR_PLAY_ALL);
  entries[numAccels++].Set(wxACCEL_CTRL, WXK_SPACE, IDB_TOOLBAR_PLAY_PART);
  entries[numAccels++].Set(wxACCEL_CTRL, (int)'C',  IDB_TOOLBAR_CLEAR);
  entries[numAccels++].Set(wxACCEL_NORMAL, WXK_F2,  IDM_SAVE_SPEAKER);
  entries[numAccels++].Set(wxACCEL_NORMAL, WXK_F3,  IDM_LOAD_SPEAKER);
  entries[numAccels++].Set(wxACCEL_NORMAL, WXK_F4,  IDM_SAVE_SEGMENT_SEQUENCE);
  entries[numAccels++].Set(wxACCEL_NORMAL, WXK_F5,  IDM_LOAD_SEGMENT_SEQUENCE);
  entries[numAccels++].Set(wxACCEL_NORMAL, WXK_F6,  IDK_F6);
  entries[numAccels++].Set(wxACCEL_NORMAL, WXK_F7,  IDK_F7);
  entries[numAccels++].Set(wxACCEL_NORMAL, WXK_F8,  IDK_F8);
  entries[numAccels++].Set(wxACCEL_NORMAL, WXK_F9, IDK_F9);
  entries[numAccels++].Set(wxACCEL_CTRL, (int)'<',  IDK_CTRL_LESS);
  entries[numAccels++].Set(wxACCEL_CTRL, WXK_DELETE,  IDK_CTRL_DELETE);
  entries[numAccels++].Set(wxACCEL_CTRL, WXK_INSERT,  IDK_CTRL_INSERT);
  entries[numAccels++].Set(wxACCEL_CTRL, (int)'N',  IDM_NORMALIZE_AMPLITUDE);
  entries[numAccels++].Set(wxACCEL_NORMAL, WXK_F11,  IDK_F11);
  entries[numAccels++].Set(wxACCEL_NORMAL, WXK_F12,  IDK_F12);

  wxAcceleratorTable accel(numAccels, entries);
  this->SetAcceleratorTable(accel);


  // ****************************************************************
  // Set properties of this window.
  // ****************************************************************

  this->SetSize(10, 10, 1000, 750);

  // ****************************************************************
  // Create the menu.
  // ****************************************************************

  wxMenu *menu = NULL;
  wxMenu *subMenu = NULL;
  
  menuBar = new wxMenuBar();
  menu = new wxMenu();
  menu->Append(IDM_LOAD_WAV_EGG, "Load WAV+EGG (stereo)");
  menu->Append(IDM_SAVE_WAV_EGG, "Save WAV+EGG (stereo)");
  menu->Append(IDM_LOAD_WAV, "Load WAV");
  menu->Append(IDM_SAVE_WAV, "Save WAV");
  menu->Append(IDM_SAVE_WAV_AS_TXT, "Save WAV as TXT");
  menu->AppendSeparator();
  menu->Append(IDM_LOAD_GESTURAL_SCORE, "Load gestural score");
  menu->Append(IDM_SAVE_GESTURAL_SCORE, "Save gestural score");
  menu->AppendSeparator();
  menu->Append(IDM_LOAD_SEGMENT_SEQUENCE, "Load segment sequence (F5)");
  menu->Append(IDM_SAVE_SEGMENT_SEQUENCE, "Save segment sequence (F4)");

  subMenu = new wxMenu();
  subMenu->Append(IDM_IMPORT_SEGMENT_SEQUENCE_ESPS, "Import from ESPS/waves+ files");
  menu->AppendSubMenu(subMenu, "Import segment sequence");

  menu->AppendSeparator();
  menu->Append(IDM_LOAD_SPEAKER, "Load speaker (F3)");
  menu->Append(IDM_SAVE_SPEAKER, "Save speaker (F2)");
  menu->AppendSeparator();
  menu->Append(wxID_EXIT, "Exit");

  menuBar->Append(menu, "File");

  // ****************************************************************

  menu = new wxMenu();
  menu->Append(IDM_SET_AUDIO_ZERO, "Set audio selection to zero");
  menu->Append(IDM_NORMALIZE_AMPLITUDE, "Normalize amplitude (Ctrl+N)");
  menu->Append(IDM_SCALE_AMPLITUDE_UP, "Main track amplitude +20%");
  menu->Append(IDM_SCALE_AMPLITUDE_DOWN, "Main track amplitude -20%");
  menu->Append(IDM_REDUCE_CONSONANT_VARIABILITY, "Reduce consonant variability");

  menu->AppendSeparator();
  menu->Append(IDM_CHANGE_GESTURAL_SCORE_F0_OFFSET, "Change gestural score F0 offset");
  menu->Append(IDM_CHANGE_GESTURAL_SCORE_F0_RANGE, "Change gestural score F0 range");
  menu->Append(IDM_CHANGE_GESTURAL_SCORE_F0_TARGET_SLOPES, "Change gestural score F0 target slopes");
  menu->Append(IDM_SUBSTITUTE_GESTURAL_SCORE_GLOTTAL_SHAPES, "Substitute gestural score glottal shapes");
  menu->Append(IDM_CHANGE_GESTURAL_SCORE_SUBGLOTTAL_PRESSURE, "Change gestural score subglottal pressure");
  menu->Append(IDM_GET_GESTURAL_SCORE_F0_STATISTICS, "Get gestural score F0 statistics");
  menu->AppendSeparator();
  menu->Append(IDM_CHANGE_GESTURAL_SCORE_DURATION, "Change gestural score duration");
  menu->Append(IDM_CHANGE_GESTURAL_SCORE_TIME_CONSTANTS, "Change gestural score time constants");

  menuBar->Append(menu, "Edit");

  // ****************************************************************

  menu = new wxMenu();
  menu->Append(IDM_EXPORT_AREA_FUNCTION, "Area function");
  menu->Append(IDM_EXPORT_CROSS_SECTIONS, "Cross sections");
  menu->Append(IDM_EXPORT_VT_CONTOUR_SVG, "2D vocal tract contour (SVG)");
  menu->Append(IDM_EXPORT_WIREFRAME_MODEL_SVG, "3D wireframe vocal tract (SVG)");
  menu->Append(IDM_EXPORT_VT_MODEL_OBJ, "3D vocal tract shape (OBJ)");
  menu->Append(IDM_EXPORT_PRIMARY_SPECTRUM, "Primary spectrum");
  menu->Append(IDM_EXPORT_SECONDARY_SPECTRUM, "User spectrum");
  menu->Append(IDM_EXPORT_EMA_TRAJECTORIES, "EMA trajectories from gestural score");
  menu->Append(IDM_EXPORT_VIDEO_FRAMES, "Vocal tract video frames from ges. score");
  menu->Append(IDM_EXPORT_TRANSFER_FUNCTIONS_FROM_SCORE, "Transfer functions from gestural score");

  menuBar->Append(menu, "Export");

  // ****************************************************************

  menu = new wxMenu();
  menu->Append(IDM_SHOW_VOCAL_TRACT_DIALOG, "Vocal tract model");
  menu->Append(IDM_SHOW_VOCAL_TRACT_SHAPES, "Vocal tract shapes");
  menu->Append(IDM_SHOW_PHONETIC_PARAMS, "Phonetic parameters");
  menu->Append(IDM_SHOW_ANATOMY_PARAMS, "Anatomy parameters");

  menu->AppendSeparator();
  menu->Append(IDM_SHOW_FDS_OPTIONS_DIALOG, "Acoustic model options (freq. domain)");
  menu->Append(IDM_SHOW_TDS_OPTIONS_DIALOG, "Acoustic model options (time domain)");
  menu->AppendSeparator();
  menu->Append(IDM_SHOW_VOCAL_FOLD_DIALOG, "Vocal fold models");
  menu->Append(IDM_SHOW_LF_MODEL_DIALOG, "LF glottal flow model");

  menuBar->Append(menu, "Synthesis models");

  // ****************************************************************

  menu = new wxMenu();
  menu->Append(IDM_GESTURAL_SCORE_FILE_TO_AUDIO, "Ges. score file to audio");
  menu->Append(IDM_TUBE_SEQUENCE_FILE_TO_AUDIO, "Tube sequence file to audio");
  menu->Append(IDM_TRACT_SEQUENCE_FILE_TO_AUDIO, "Tract sequence file to audio");
  menu->AppendSeparator();
  menu->Append(IDM_GESTURAL_SCORE_TO_TUBE_SEQUENCE_FILE, "Ges. score to tube sequence file");
  menu->Append(IDM_GESTURAL_SCORE_TO_TRACT_SEQUENCE_FILE, "Ges. score to tract sequence file");

  menuBar->Append(menu, "Synthesis from file");

  // ****************************************************************

  menu = new wxMenu();
  menu->Append(IDM_HERTZ_TO_SEMITONES, "Hertz <-> semitones");
  menu->Append(wxID_ABOUT, "About");

  menuBar->Append(menu, "Help");

  // ****************************************************************

  this->SetMenuBar(menuBar);

  // ****************************************************************
  // Create the toolbar.
  // ****************************************************************

  int numTools = 0;
  wxBitmap bitmap;
  toolBar = CreateToolBar();
  toolBar->SetToolBitmapSize(wxSize(24, 24));
  
  toolBar->AddTool(IDB_TOOLBAR_RECORD, "Record", xpmRecord);
  toolBar->AddTool(IDB_TOOLBAR_PLAY_ALL, "Play all", xpmPlayAll);
  toolBar->AddTool(IDB_TOOLBAR_PLAY_PART, "Play part", xpmPlayPart);
  toolBar->AddTool(IDB_TOOLBAR_CLEAR, "Clear items", xpmClear);

  // Make the added tool appear in the tool bar!
  toolBar->Realize();

  // ****************************************************************
  // Create the context menu with the choices to clear things up.
  // ****************************************************************

  clearContextMenu = new wxMenu();
  clearContextMenu->Append(IDM_CLEAR_ALL, "Clear all");
  clearContextMenu->AppendSeparator();
  clearContextMenu->Append(IDM_CLEAR_MAIN_TRACK, "Clear main track");
  clearContextMenu->Append(IDM_CLEAR_EGG_TRACK, "Clear EGG track");
  clearContextMenu->Append(IDM_CLEAR_EXTRA_TRACK, "Clear extra track");
  clearContextMenu->AppendSeparator();
  clearContextMenu->Append(IDM_CLEAR_ALL_TRACKS, "Clear audio tracks");
  clearContextMenu->Append(IDM_CLEAR_GESTURAL_SCORE, "Clear gestural score");
  clearContextMenu->Append(IDM_CLEAR_SEGMENT_SEQUENCE, "Clear segment sequence");
  clearContextMenu->Append(IDM_CLEAR_ANALYSIS_TRACKS, "Clear F0, GCIs, formant tracks");

  // ****************************************************************
  // Create the pages.
  // ****************************************************************

  notebook = new wxNotebook(this, ID_NOTEBOOK);

  signalPage = new SignalPage(notebook);
  vocalTractPage = new VocalTractPage(notebook, VocalTractDialog::getInstance(this)->getVocalTractPicture());
  tdsPage = new TdsPage(notebook);
  gesturalScorePage = new GesturalScorePage(notebook);

  notebook->AddPage((wxPanel*)signalPage, "Signals", false);
  notebook->AddPage((wxPanel*)vocalTractPage, "Vocal tract", true);
  notebook->AddPage((wxPanel*)tdsPage, "Time domain simulation", false);
  notebook->AddPage((wxPanel*)gesturalScorePage, "Gestural score", false);

  // ****************************************************************
  // Update all widgets.
  // ****************************************************************

  updateWidgets();
}


// ****************************************************************************
// ****************************************************************************

void MainWindow::updateWidgets()
{
  int prevPage = data->currentPage;

  if (notebook->GetCurrentPage() == (wxWindow*)signalPage)
  {
    signalPage->updateWidgets();
    data->currentPage = Data::SIGNAL_PAGE;
  }
  else
  if (notebook->GetCurrentPage() == (wxWindow*)vocalTractPage)
  {
    vocalTractPage->updateWidgets();
    data->currentPage = Data::VOCAL_TRACT_PAGE;
  }
  else
  if (notebook->GetCurrentPage() == (wxWindow*)tdsPage)
  {
    tdsPage->updateWidgets();
    data->currentPage = Data::TDS_PAGE;
  }
  else
  if (notebook->GetCurrentPage() == (wxWindow*)gesturalScorePage)
  {
    gesturalScorePage->updateWidgets();
    gesturalScorePage->Refresh();
    data->currentPage = Data::GESTURAL_SCORE_PAGE;
  }

  // ****************************************************************
  // If the page changed since the last update, refresh the dialog
  // with the analysis results, because they depend on the page
  // shown.
  // ****************************************************************

  if (data->currentPage != prevPage)
  {
    AnalysisResultsDialog *dialog = AnalysisResultsDialog::getInstance();
    if (dialog->IsShown())
    {
      dialog->updateWidgets();
    }
  }
}


// ****************************************************************************
// Save a signal as a text file with temporal positions and amplitudes.
// ****************************************************************************

bool MainWindow::saveWaveformAsTxtFile(const wxString &fileName, Signal16 *signal, int pos, int length)
{
  ofstream file(fileName.ToStdString());
  if (!file) 
  { 
    return false; 
  }

  const int N = 1000;
  int i, index;
  double time;
  char st[256];

  // ****************************************************************
  // Time and amplitude of the samples.
  // ****************************************************************

  file << "time_s amplitude" << endl;

  for (i=0; i < N; i++)
  {
    index = i*length/N;
    time = (double)index/(double)SAMPLING_RATE;

    index = pos + i*length/N;

    sprintf(st, "%2.4f %2.2f", time, (double)signal->getValue(index));
    file << string(st) << endl;
  }

  // File is closed automatically ...
  return true;
}


// ****************************************************************************
// Save the area functions of the new vocal tract model.
// ****************************************************************************

bool MainWindow::saveAreaFunction(const wxString &fileName)
{
  ofstream file(fileName.ToStdString());
  if (!file) 
  { 
    return false; 
  }

  int i;
  VocalTract *vt = data->vocalTract;

  // ****************************************************************

  vt->calculateAll();

  // ****************************************************************
  
  file << "=== Piecewise linear area function ===" << endl;
  file << "position_cm  area_cm2  perimeter_cm  articulator" << endl;

  for (i=0; i < VocalTract::NUM_CENTERLINE_POINTS; i++)
  {
    file << (double)vt->crossSection[i].pos << "  " 
         << (double)vt->crossSection[i].area << "  "
         << (double)vt->crossSection[i].circ << "  " 
         << Tube::ARTICULATOR_LETTER[ vt->crossSection[i].articulator ] << endl;
  }
  file << endl;

  // ****************************************************************

  file << "=== Piecewise constant (stepwise changing) area function ===" << endl;
  file << "position_cm  area_cm2  perimeter_cm  articulator" << endl;

  for (i=0; i < VocalTract::NUM_TUBE_SECTIONS; i++)
  {
    file << (double)vt->tubeSection[i].pos << "  " 
         << (double)vt->tubeSection[i].area << "  "
         << (double)vt->tubeSection[i].circ << "  "
         << Tube::ARTICULATOR_LETTER[ vt->tubeSection[i].articulator ] << endl;

    file << (double)(vt->tubeSection[i].pos + vt->tubeSection[i].length) << "  "
         << (double)vt->tubeSection[i].area << "  "
         << (double)vt->tubeSection[i].circ << "  "
         << Tube::ARTICULATOR_LETTER[ vt->tubeSection[i].articulator ] << endl;
  }
  file << endl;

  // The file is closed automatically.
  return true;
}


// ****************************************************************************
/// Save a spectrum into a text file.
// ****************************************************************************

bool MainWindow::saveSpectrum(const wxString &fileName, ComplexSignal *spectrum)
{
  double F0 = (double)SAMPLING_RATE / (double)spectrum->N;

  ofstream file(fileName.ToStdString());
  if (!file) 
  { 
    return false; 
  }

  int i;
  int numPoints = spectrum->N/2 - 1;
  double freqSpacing_Hz = (double)SAMPLING_RATE / (double)spectrum->N;

  // ****************************************************************

  file << "num_points: " << numPoints << endl;
  file << "frequency_Hz  magnitude  phase_rad" << endl;

  for (i=0; i < numPoints; i++)
  {
    file << (double)(i*freqSpacing_Hz) << "  "
         << (double)spectrum->getMagnitude(i) << "  "
         << (double)spectrum->getPhase(i) << endl;
  }
  file << endl;

  // The file is closed automatically.
  return true;
}


// ****************************************************************************
// ****************************************************************************

void MainWindow::OnCloseWindow(wxCloseEvent &event)
{
  if (wxMessageBox("Do you really want to quit?", "Quit", wxYES_NO, this) == wxYES)
  {
    // **************************************************************
    // Check if there are unsaved changes to the speaker data.
    // **************************************************************

    int i;
    bool unsavedVocalTract = data->vocalTract->hasUnsavedChanges();
    bool unsavedGlottis = false;
    for (i=0; i < Data::NUM_GLOTTIS_MODELS; i++)
    {
      if (data->glottis[i]->hasUnsavedChanges())
      {
        unsavedGlottis = true;
      }
    }

    if ((unsavedVocalTract) || (unsavedGlottis))
    {
      wxString st = "There are unsaved changes to the following items:\n";
      if (unsavedVocalTract)
      {
        st+= "  o Vocal tract anatomy or shapes\n";
      }
      if (unsavedGlottis)
      {
        st+= "  o Glottis models\n";
      }
      st+= "\n";
      st+= "Do you want to save the changes in a speaker file?\n";

      if (wxMessageBox(st, "Save changes?", wxYES_NO, this) == wxYES)
      {
        wxFileName fileName(data->speakerFileName);
        wxString name = wxFileSelector("Save speaker file", fileName.GetPath(), 
          fileName.GetFullName(), "speaker", "Speaker files (*.speaker)|*.speaker", 
          wxFD_SAVE | wxFD_OVERWRITE_PROMPT, this);

        if (name.empty() == false)
        {
          data->saveSpeaker(name);
        }
      }
    }

    // **************************************************************
    // Save the configuration.
    // **************************************************************

    data->writeConfig();
    // The destructor of config will actually save the data.
    delete data->config;

    // **************************************************************
    // Close all hidden dialogs.
    // **************************************************************

    VocalTractShapesDialog::getInstance()->Close(true);
    PhoneticParamsDialog::getInstance()->Close(true);
    AnatomyParamsDialog::getInstance()->Close(true);
    LfPulseDialog::getInstance()->Close(true);
    TdsOptionsDialog::getInstance()->Close(true);
    FdsOptionsDialog::getInstance()->Close(true);
    SpectrumOptionsDialog::getInstance(NULL)->Close(true);
    GlottisDialog::getInstance()->Close(true);
    VocalTractDialog::getInstance(this)->Close(true);
    AnalysisResultsDialog::getInstance()->Close(true);
    AnnotationDialog::getInstance(NULL)->Close(true);
    PoleZeroDialog::getInstance()->Close(true);
    TransitionDialog::getInstance()->Close(true);

    this->Destroy();
    exit(0);
  }
}


// ****************************************************************************
/// The notebook page has just changed.
// ****************************************************************************

void MainWindow::OnPageChanged(wxNotebookEvent &event)
{
  updateWidgets();
}


// ****************************************************************************
// ****************************************************************************

void MainWindow::OnLoadWavEgg(wxCommandEvent &event)
{
  wxString name = wxFileSelector("Open a stereo wave file", audioFileName.GetPath(), 
    audioFileName.GetFullName(), "wav", "Wave-files (*.wav)|*.wav", 
    wxFD_OPEN | wxFD_FILE_MUST_EXIST, this);

  if (name.empty() == false)
  {
    audioFileName = wxFileName(name);

    AudioFile<double> audioFile;
    if (!audioFile.load(audioFileName.GetFullPath().ToStdString()))
    {
      wxMessageBox("Error in loading the file.", "Error!");
      return;
    }

    if (audioFile.isStereo() == false)
    {
      wxMessageBox("Error: The audio file must be stereo.");
      return;
    }

    int targetPos = 0;

    int answer = wxMessageBox(
      "Press YES if you want the loaded signal to start at the beginning "
      "of the track (and clear the rest of the track). Press NO if you "
      "want to insert the loaded signal at the marked position.",
      "Put signal at the beginning of the track?", wxYES_NO | wxCANCEL);

    if (answer == wxYES)
    {
      targetPos = 0;
      data->track[Data::MAIN_TRACK]->setZero();
      data->track[Data::EGG_TRACK]->setZero();
    }
    else
      if (answer == wxNO)
      {
        targetPos = data->mark_pt;
      }

    if (answer != wxCANCEL)
    {
      int i;
      int sourceSamplingRate = audioFile.getSampleRate();
      int numSourceSamples = audioFile.getNumSamplesPerChannel();
      int numTargetSamples = numSourceSamples * (double)SAMPLING_RATE / (double)sourceSamplingRate;
      int sourceIndex = 0;

      for (i = 0; i < numTargetSamples; i++)
      {
        // Make a (linear) sampling rate conversion.
        sourceIndex = i*(double)numSourceSamples / (double)numTargetSamples;
        if (sourceIndex >= numSourceSamples)
        {
          sourceIndex = numSourceSamples - 1;
        }
        data->track[Data::MAIN_TRACK]->setValue(targetPos + i, audioFile.samples[0][sourceIndex] * 32767.0);
        data->track[Data::EGG_TRACK]->setValue(targetPos + i, audioFile.samples[1][sourceIndex] * 32767.0);
      }
      updateWidgets();
    }

    updateWidgets();
  }
}

// ****************************************************************************
// ****************************************************************************

void MainWindow::OnSaveWavEgg(wxCommandEvent &event)
{
  if (data->isValidSelection() == false)
  {
    wxMessageBox("You must select the region to be saved.", "Invalid selection");
    return;
  }

  int leftPos = data->selectionMark_pt[0];
  int rightPos = data->selectionMark_pt[1];


  wxString name = wxFileSelector("Save a stereo wave file", audioFileName.GetPath(), 
    audioFileName.GetFullName(), "wav", "Wave-files (*.wav)|*.wav", 
    wxFD_SAVE | wxFD_OVERWRITE_PROMPT, this);

  if (name.empty() == false)
  {
    audioFileName = wxFileName(name);

    // Create and save the audio file.

    AudioFile<double> audioFile;
    int numSamples = rightPos - leftPos + 1;
    audioFile.setAudioBufferSize(2, numSamples);
    audioFile.setBitDepth(16);
    audioFile.setSampleRate(SAMPLING_RATE);
    int i;
    for (i = 0; i < numSamples; i++)
    {
      audioFile.samples[0][i] = (double)data->track[Data::MAIN_TRACK]->x[leftPos + i] / 32768.0;
      audioFile.samples[1][i] = (double)data->track[Data::EGG_TRACK]->x[leftPos + i] / 32768.0;
    }

    if (audioFile.save(audioFileName.GetFullPath().ToStdString()) == false)
    {
      wxMessageBox("Error saving the file.", "Error!");
    }

  }
}


// ****************************************************************************
/// Load a wave-file into one of the tracks.
// ****************************************************************************

void MainWindow::OnLoadWav(wxCommandEvent &event)
{
  wxFileDialog dialog(this, "Open a wave file", 
    audioFileName.GetPath(), audioFileName.GetFullName(), 
    "Wave-files (*.wav)|*.wav", wxFD_OPEN | wxFD_FILE_MUST_EXIST);

  if (dialog.ShowModal() == wxID_OK)
  {
    audioFileName = wxFileName(dialog.GetPath());

    int trackIndex = Data::selectTrack(this, 
      "Where do you want to put the signal\n" + audioFileName.GetFullName() + "?");

    if (trackIndex != -1)
    {
      AudioFile<double> audioFile;
      if (!audioFile.load(audioFileName.GetFullPath().ToStdString()))
      {
        wxMessageBox("Error in loading the file.", "Error!");
        return;
      }

      if (audioFile.isMono() == false)
      {
        wxMessageBox("Error: The audio file must be mono.");
        return;
      }

      int targetPos = 0;

      int answer = wxMessageBox(
        "Press YES if you want the loaded signal to start at the beginning "
        "of the track (and clear the rest of the track). Press NO if you "
        "want to insert the loaded signal at the marked position.",
        "Put signal at the beginning of the track?", wxYES_NO | wxCANCEL);

      if (answer == wxYES)
      {
        targetPos = 0;
        data->track[trackIndex]->setZero();
      }
      else
        if (answer == wxNO)
        {
          targetPos = data->mark_pt;
        }

      if (answer != wxCANCEL)
      {
        int sourceSamplingRate = audioFile.getSampleRate();
        int numSourceSamples = audioFile.getNumSamplesPerChannel();
        int numTargetSamples = numSourceSamples * (double)SAMPLING_RATE / (double)sourceSamplingRate;
        int sourceIndex = 0;
        int i;

        for (i = 0; i < numTargetSamples; i++)
        {
          // Make a (linear) sampling rate conversion.
          sourceIndex = i*(double)numSourceSamples / (double)numTargetSamples;
          if (sourceIndex >= numSourceSamples)
          {
            sourceIndex = numSourceSamples - 1;
          }
          data->track[trackIndex]->setValue(targetPos + i, audioFile.samples[0][sourceIndex] * 32767.0);
        }

        updateWidgets();
      }

    }
  }

}

// ****************************************************************************
// ****************************************************************************

void MainWindow::OnSaveWav(wxCommandEvent &event)
{
  if (data->isValidSelection() == false)
  {
    wxMessageBox("You must select the region to be saved.", "Invalid selection");
    return;
  }

  int leftPos = data->selectionMark_pt[0];
  int rightPos = data->selectionMark_pt[1];

  wxFileDialog dialog(this, "Save a wave file", 
    audioFileName.GetPath(), audioFileName.GetFullName(), 
    "Wave-files (*.wav)|*.wav", wxFD_SAVE | wxFD_OVERWRITE_PROMPT);

  if (dialog.ShowModal() == wxID_OK)
  {
    audioFileName = wxFileName(dialog.GetPath());

    int trackIndex = Data::selectTrack(this, 
      "From where do you want to save the signal to\n" + audioFileName.GetFullName() + "?");

    if (trackIndex != -1)
    {
      // Create and save the audio file.

      AudioFile<double> audioFile;
      int numSamples = rightPos - leftPos + 1;
      audioFile.setAudioBufferSize(1, numSamples);
      audioFile.setBitDepth(16);
      audioFile.setSampleRate(SAMPLING_RATE);
      int i;
      for (i = 0; i < numSamples; i++)
      {
        audioFile.samples[0][i] = (double)data->track[trackIndex]->x[leftPos + i] / 32768.0;
      }

      if (audioFile.save(audioFileName.GetFullPath().ToStdString()) == false)
      {
        wxMessageBox("Error saving the file.", "Error!");
      }
    }
  }
}


// ****************************************************************************
// ****************************************************************************

void MainWindow::OnSaveWavAsTxt(wxCommandEvent &event)
{
  if (data->isValidSelection() == false)
  {
    wxMessageBox("You must select the region to be saved.", "Invalid selection");
    return;
  }

  int leftPos = data->selectionMark_pt[0];
  int rightPos = data->selectionMark_pt[1];

  wxFileDialog dialog(this, "Save waveform as text file", 
    audioFileName.GetPath(), audioFileName.GetName(), 
    "Text files (*.txt)|*.txt", wxFD_SAVE | wxFD_OVERWRITE_PROMPT);

  if (dialog.ShowModal() == wxID_OK)
  {
    audioFileName = wxFileName(dialog.GetPath());

    int trackIndex = Data::selectTrack(this, 
      "From where do you want to save the signal to\n" + audioFileName.GetFullName() + "?");

    if (trackIndex != -1)
    {
      if (!saveWaveformAsTxtFile(audioFileName.GetFullPath(), 
            data->track[trackIndex], leftPos, rightPos-leftPos+1))
      {
        wxMessageBox("Error in saving the file.", "Error!");
      }
    }
  }

}


// ****************************************************************************
/// Load a segment sequence.
// ****************************************************************************

void MainWindow::OnLoadSegmentSequence(wxCommandEvent &event)
{
  wxFileName fileName(data->segmentSequenceFileName);

  wxString name = wxFileSelector("Load segment sequence", fileName.GetPath(), 
    fileName.GetFullName(), ".seg", "Segment sequence (*.seg)|*.seg", 
    wxFD_OPEN | wxFD_FILE_MUST_EXIST, this);

  if (name.empty() == false)
  {
    data->segmentSequenceFileName = name;
    if (data->segmentSequence->readFromFile(name.ToStdString()) == false)
    {
      wxMessageBox("Loading the segment sequence failed!", "Error!");
    }
    updateWidgets();
  }
}


// ****************************************************************************
/// Save a segment sequence.
// ****************************************************************************

void MainWindow::OnSaveSegmentSequence(wxCommandEvent &event)
{
  wxFileName fileName(data->segmentSequenceFileName);

  wxString name = wxFileSelector("Save segment sequence", fileName.GetPath(), 
    fileName.GetFullName(), ".seg", "Segment sequence (*.seg)|*.seg", 
    wxFD_SAVE | wxFD_OVERWRITE_PROMPT, this);

  if (name.empty() == false)
  {
    data->segmentSequenceFileName = name;
    if (data->segmentSequence->writeToFile(name.ToStdString()) == false)
    {
      wxMessageBox("Saving the segment sequence failed!", "Error!");
    }
  }
}


// ****************************************************************************
/// Import a segment sequence from the ESPS/waves+ software.
// ****************************************************************************

void MainWindow::OnImportSegmentSequenceEsps(wxCommandEvent &event)
{
  wxFileName fileName(data->segmentSequenceFileName);

  wxString name = wxFileSelector("Load phoneme sequence (ESPS/waves+ format)", fileName.GetPath(), 
    fileName.GetFullName(), "*.*", "All files|*.*", 
    wxFD_OPEN | wxFD_FILE_MUST_EXIST, this);

  if (name.empty() == false)
  {
    data->segmentSequenceFileName = name;
    if (data->segmentSequence->importFromEsps(name.ToStdString()) == false)
    {
      wxMessageBox("Importing the segment sequence failed!", "Error!");
    }
    updateWidgets();
  }
}


// ****************************************************************************
/// Load a gestural score.
// ****************************************************************************

void MainWindow::OnLoadGesturalScore(wxCommandEvent &event)
{
  wxFileName fileName(data->gesturalScoreFileName);

  wxString name = wxFileSelector("Load gestural score", fileName.GetPath(), 
    fileName.GetFullName(), ".ges", "Gestural scores (*.ges)|*.ges", 
    wxFD_OPEN | wxFD_FILE_MUST_EXIST, this);

  if (name.empty() == false)
  {
    bool allValuesInRange = true;
    data->gesturalScoreFileName = name;
    if (data->gesturalScore->loadGesturesXml(name.ToStdString(), allValuesInRange) == false)
    {
      wxMessageBox("Loading the gestural score failed!", "Error!");
    }
    else
    if (allValuesInRange == false)
    {
      wxMessageBox("Some gesture values in the score were out of range and have been constricted.", "Warning!");
    }
    updateWidgets();
  }
}


// ****************************************************************************
/// Save a gestural score.
// ****************************************************************************

void MainWindow::OnSaveGesturalScore(wxCommandEvent &event)
{
  wxFileName fileName(data->gesturalScoreFileName);

  wxString name = wxFileSelector("Save gestural score", fileName.GetPath(), 
    fileName.GetFullName(), ".ges", "Gestural scores (*.ges)|*.ges", 
    wxFD_SAVE | wxFD_OVERWRITE_PROMPT, this);

  if (name.empty() == false)
  {
    data->gesturalScoreFileName = name;
    if (data->gesturalScore->saveGesturesXml(name.ToStdString()) == false)
    {
      wxMessageBox("Saving the gestural score failed!", "Error!");
    }
  }
}

// ****************************************************************************
/// Load speaker data.
// ****************************************************************************

void MainWindow::OnLoadSpeaker(wxCommandEvent &event)
{
  wxFileName fileName(data->speakerFileName);

  wxString name = wxFileSelector("Load speaker file", fileName.GetPath(), 
    fileName.GetFullName(), "speaker", "Speaker files (*.speaker)|*.speaker", 
    wxFD_OPEN | wxFD_FILE_MUST_EXIST, this);

  if (name.empty() == false)
  {
    data->loadSpeaker(name);

    // Refill the vocal tract shapes list.
    VocalTractShapesDialog *shapesDialog = VocalTractShapesDialog::getInstance();
    shapesDialog->updateWidgets();
    shapesDialog->fillShapeList();

    // Update the vocal tract picture.
    VocalTractDialog *vtDialog = VocalTractDialog::getInstance(this);
    vtDialog->getVocalTractPicture()->Refresh();

    // Update the glottis dialog.
    GlottisDialog *glottisDialog = GlottisDialog::getInstance();
    glottisDialog->updateWidgets();

    data->updateTlModelGeometry(data->vocalTract);
    updateWidgets();
  }
}


// ****************************************************************************
/// Save speaker data.
// ****************************************************************************

void MainWindow::OnSaveSpeaker(wxCommandEvent &event)
{
  wxFileName fileName(data->speakerFileName);

  wxString name = wxFileSelector("Save speaker file", fileName.GetPath(), 
    fileName.GetFullName(), "speaker", "Speaker files (*.speaker)|*.speaker", 
    wxFD_SAVE | wxFD_OVERWRITE_PROMPT, this);

  if (name.empty() == false)
  {
    data->saveSpeaker(name);

    // Change file name in shape list dialog
    VocalTractShapesDialog *dialog = VocalTractShapesDialog::getInstance();
    dialog->updateWidgets();
  }
}


// ****************************************************************************
// ****************************************************************************

void MainWindow::OnExit(wxCommandEvent &event)
{
  Close(true);
}


// ****************************************************************************
// ****************************************************************************

void MainWindow::OnSetAudioZero(wxCommandEvent &event)
{
  int firstSample = data->selectionMark_pt[0];
  int lastSample = data->selectionMark_pt[1];
  int i;

  if (lastSample <= firstSample)
  {
    wxMessageBox("You must select the audio region to set to zero!", "Please select a region");
    return;
  }

  int trackIndex = data->selectTrack(this, wxString("Please select a track!"));
  if (trackIndex != -1)
  {
    for (i=firstSample; i < lastSample; i++)
    {
      data->track[trackIndex]->setValue(i, 0);
    }
  }
  updateWidgets();
}


// ****************************************************************************
// ****************************************************************************

void MainWindow::OnNormalizeAmplitude(wxCommandEvent &event)
{
  int trackIndex = data->selectTrack(this, wxString("Please select a track for normalization (to -1 dB)!"));
  if (trackIndex != -1)
  {
    data->normalizeAudioAmplitude(trackIndex);
  }
  updateWidgets();
}


// ****************************************************************************
// ****************************************************************************

void MainWindow::OnScaleAmplitudeUp(wxCommandEvent &event)
{
  const int MAX = 32767;
  const int MIN = -32768;

  Signal16 *s = data->track[Data::MAIN_TRACK];
  double d;

  for (int i = 0; i < s->N; i++)
  {
    d = s->x[i] * 1.2;
    if (d < MIN) { d = MIN; }
    if (d > MAX) { d = MAX; }
    s->x[i] = (signed short)d;
  }

  updateWidgets();
}


// ****************************************************************************
// ****************************************************************************

void MainWindow::OnScaleAmplitudeDown(wxCommandEvent &event)
{
  Signal16 *s = data->track[Data::MAIN_TRACK];
  for (int i = 0; i < s->N; i++)
  {
    s->x[i] = (signed short)(s->x[i] / 1.2);
  }

  updateWidgets();
}


// ****************************************************************************
/// Reduces the degree of coarticulatory variation of a consonant that is 
/// selected by the user to a certain degree.
// ****************************************************************************

void MainWindow::OnReduceConsonantVariability(wxCommandEvent &event)
{
  int i;
  wxString st;
  wxArrayString consonantList;
  VocalTract *tract = data->vocalTract;

  // ****************************************************************
  // Make the list of consonants as an array of strings.
  // ****************************************************************

  for (i=0; i < (int)tract->shapes.size(); i++)
  {
    st = wxString( tract->shapes[i].name );
    if ((st.StartsWith("tb-")) || (st.StartsWith("tt-")) || (st.StartsWith("ll-")))
    {
      if ((st.EndsWith("(a)")) || (st.EndsWith("(i)")) || (st.EndsWith("(u)")))
      {
        // Remove the last three chars.
        st.RemoveLast();
        st.RemoveLast();
        st.RemoveLast();
      }

      if (consonantList.Index(st, true) == wxNOT_FOUND)
      {
        consonantList.Add(st);
      }
    }
  }

  // ****************************************************************
  // Let the user select one of the consonants in the list.
  // ****************************************************************

  wxSingleChoiceDialog consonantDialog(this, "Please select a reference consonant", 
    "Select a consonant", consonantList);

  if (consonantDialog.ShowModal() == wxID_CANCEL)
  {
    return;
  }

  st = consonantDialog.GetStringSelection();
  if (st == "")
  {
    return;
  }

  int referenceConsonantIndexA = tract->getShapeIndex((st + "(a)").ToStdString());
  int referenceConsonantIndexI = tract->getShapeIndex((st + "(i)").ToStdString());
  int referenceConsonantIndexU = tract->getShapeIndex((st + "(u)").ToStdString());

  if ((referenceConsonantIndexA == -1) || (referenceConsonantIndexI == -1) ||
    (referenceConsonantIndexU == -1))
  {
    wxMessageBox("The selected consonant " + st + " is not defined in all three contexts (a,i,u)!", "Error");
  }

  wxString consonantName = st;
  double referenceParamsA[VocalTract::NUM_PARAMS];
  double referenceParamsI[VocalTract::NUM_PARAMS];
  double referenceParamsU[VocalTract::NUM_PARAMS];
  double meanParams[VocalTract::NUM_PARAMS];

  for (i=0; i < VocalTract::NUM_PARAMS; i++)
  {
    referenceParamsA[i] = tract->shapes[referenceConsonantIndexA].param[i];
    referenceParamsI[i] = tract->shapes[referenceConsonantIndexI].param[i];
    referenceParamsU[i] = tract->shapes[referenceConsonantIndexU].param[i];
    meanParams[i] = (referenceParamsA[i] + referenceParamsI[i] + referenceParamsU[i]) / 3.0;
  }

  // ****************************************************************
  // Let the user select the reduced degree of coarticulation
  // between 0% and 100%.
  // ****************************************************************

  wxTextEntryDialog textDialog(this, "Please enter the new degree of variation as a number between 0 and 100.", "Reduced degree of variation in %", "0");

  if (textDialog.ShowModal() == wxID_CANCEL)
  {
    return;
  }

  if (textDialog.GetValue().IsEmpty())
  {
    return;
  }

  double degree_percent = 0.0;
  if ((textDialog.GetValue().ToDouble(&degree_percent) == false) ||
    (degree_percent < 0.0) || (degree_percent > 100.0))
  {
    wxMessageBox("Invalid number! The number must be between 0.0 and 100.0.", "Error");
    return;
  }

  double t = degree_percent/100.0;    // 0 <= t <= 1

  // ****************************************************************
  // Calculate the new context-sensitive target shapes for the 
  // consonants.
  // ****************************************************************

  VocalTract::Shape newShapeA, newShapeI, newShapeU;

  for (i=0; i < VocalTract::NUM_PARAMS; i++)
  {
    newShapeA.param[i] = (1.0-t)*meanParams[i] + t*referenceParamsA[i];
    newShapeI.param[i] = (1.0-t)*meanParams[i] + t*referenceParamsI[i];
    newShapeU.param[i] = (1.0-t)*meanParams[i] + t*referenceParamsU[i];
  }

  newShapeA.name = (consonantName + "-" + wxString::Format("%d", (int)degree_percent) + "%(a)").ToStdString();
  newShapeI.name = (consonantName + "-" + wxString::Format("%d", (int)degree_percent) + "%(i)").ToStdString();
  newShapeU.name = (consonantName + "-" + wxString::Format("%d", (int)degree_percent) + "%(u)").ToStdString();

  // ****************************************************************
  // Add the new shapes under a new name to the shape list.
  // ****************************************************************

  int newIndexA = tract->getShapeIndex( newShapeA.name );
  int newIndexI = tract->getShapeIndex( newShapeA.name );
  int newIndexU = tract->getShapeIndex( newShapeA.name );

  if (newIndexA == -1)
  {
    tract->shapes.push_back(newShapeA);
  }
  else
  if (wxMessageBox("The shape " + wxString(newShapeA.name) + " is already in the list. Overwrite it?", 
    "Overwrite existing shape?", wxYES_NO) == wxYES)
  {
    tract->shapes[newIndexA] = newShapeA;
  }

  // ****************************************************************

  if (newIndexI == -1)
  {
    tract->shapes.push_back(newShapeI);
  }
  else
  if (wxMessageBox("The shape " + wxString(newShapeI.name) + " is already in the list. Overwrite it?", 
    "Overwrite existing shape?", wxYES_NO) == wxYES)
  {
    tract->shapes[newIndexI] = newShapeI;
  }
  // ****************************************************************

  if (newIndexU == -1)
  {
    tract->shapes.push_back(newShapeU);
  }
  else
  if (wxMessageBox("The shape " + wxString(newShapeU.name) + " is already in the list. Overwrite it?", 
    "Overwrite existing shape?", wxYES_NO) == wxYES)
  {
    tract->shapes[newIndexU] = newShapeU;
  }

  // Update the list in the shapes dialog.

  VocalTractShapesDialog::getInstance()->fillShapeList();
}


// ****************************************************************************
// ****************************************************************************

void MainWindow::OnChangeGesturalScoreF0Offset(wxCommandEvent &event)
{
  // Obtain the current F0 statistics.

  double f0Mean_st;
  double f0Sd_st;
  double f0Mean_Hz;
  double f0Sd_Hz;
  data->gesturalScore->getF0Statistic(f0Mean_st, f0Sd_st, f0Mean_Hz, f0Sd_Hz);

  // ****************************************************************

  wxString text =
    wxGetTextFromUser("Please enter the number of semitones by which to shift the F0 contour (e.g. 3 or -4.5).",
    "Enter F0 shift in st", "0.0");

  if (text.IsEmpty())   // The user pressed "Cancel"
  {
    return;
  }

  double deltaF0_st = 0.0;
  if (text.ToDouble(&deltaF0_st) == false)
  {
    wxMessageBox("Error: Invalid number!", "Error");
    return;
  }

  data->gesturalScore->changeF0Offset(deltaF0_st);
  updateWidgets();

  // Obtain the new F0 statistics.

  double newF0Mean_st;
  double newF0Sd_st;
  double newF0Mean_Hz;
  double newF0Sd_Hz;
  data->gesturalScore->getF0Statistic(newF0Mean_st, newF0Sd_st, newF0Mean_Hz, newF0Sd_Hz);

  wxPrintf("New mean F0 - old mean F0 in st: %2.2f\n", newF0Mean_st - f0Mean_st);
}


// ****************************************************************************
// ****************************************************************************

void MainWindow::OnChangeGesturalScoreF0Range(wxCommandEvent &event)
{
  wxString text =
    wxGetTextFromUser("Please enter the factor, by which the F0 range shall be changed (e.g. 1.2 for a 20% increase).",
    "Enter F0 range factor", "1.0");

  if (text.IsEmpty())   // The user pressed "Cancel"
  {
    return;
  }

  double factor = 1.0;
  if (text.ToDouble(&factor) == false)
  {
    wxMessageBox("Error: Invalid number!", "Error");
    return;
  }

  data->gesturalScore->changeF0Range(factor);
  updateWidgets();
}


// ****************************************************************************
// ****************************************************************************

void MainWindow::OnChangeGesturalScoreF0TargetSlopes(wxCommandEvent &event)
{
  wxString text =
  wxGetTextFromUser("Please enter the value in semitones/s by which the F0 target slopes shall be changed (e.g. 30 for a 30 st/s increase).",
  "Enter F0 slope summand in st/s", "0.0");

  if (text.IsEmpty())   // The user pressed "Cancel"
  {
    return;
  }

  double summand = 0.0;
  if (text.ToDouble(&summand) == false)
  {
  wxMessageBox("Error: Invalid number!", "Error");
  return;
  }

  data->gesturalScore->changeF0TargetSlope(summand);
  updateWidgets();
}


// ****************************************************************************
// ****************************************************************************

void MainWindow::OnSubstituteGesturalScoreGlottalShapes(wxCommandEvent &event)
{
  wxString oldShapeName =
  wxGetTextFromUser("Please enter the name of the glottal shape to substitute (e.g.: modal).",
  "Enter name of glottal shape to substitute", "");

  if (oldShapeName.IsEmpty())   // The user pressed "Cancel"
  {
    return;
  }

  wxString newShapeName =
    wxGetTextFromUser("Please enter the name of the new glottal shape (e.g.: breathy).",
    "Enter name of new glottal shape", "");

  if (newShapeName.IsEmpty())   // The user pressed "Cancel"
  {
    return;
  }

  data->gesturalScore->substituteGlottalShapes(oldShapeName.ToStdString(), newShapeName.ToStdString());
  updateWidgets();
}


// ****************************************************************************
// ****************************************************************************

void MainWindow::OnChangeGesturalScoreSubglottalPressure(wxCommandEvent &event)
{
  wxString text =
  wxGetTextFromUser("Please enter the factor, by which the subglottal pressure shall be changed (e.g. 1.2 for a 20% increase).",
  "Enter subglottal pressure factor", "1.0");

  if (text.IsEmpty())   // The user pressed "Cancel"
  {
    return;
  }

  double factor = 1.0;
  if (text.ToDouble(&factor) == false)
  {
  wxMessageBox("Error: Invalid number!", "Error");
  return;
  }

  data->gesturalScore->changeSubglottalPressure(factor);
  updateWidgets();
}


// ****************************************************************************
// ****************************************************************************

void MainWindow::OnGetGesturalScoreF0Statistics(wxCommandEvent &event)
{
  double f0Mean_st;
  double f0Sd_st;
  double f0Mean_Hz;
  double f0Sd_Hz;

  data->gesturalScore->getF0Statistic(f0Mean_st, f0Sd_st, f0Mean_Hz, f0Sd_Hz);

  wxString st = wxString::Format(
    "F0 mean (SD) with a Hz scale: %2.2f (%2.2f) Hz\n"
    "F0 mean (SD) with a semitone scale: %2.2f (%2.2f) st",
    f0Mean_Hz,
    f0Sd_Hz,
    f0Mean_st,
    f0Sd_st);

  wxMessageBox(st, "Gestural score F0 statistics");
}


// ****************************************************************************
// ****************************************************************************

void MainWindow::OnChangeGesturalScoreDuration(wxCommandEvent &event)
{
  wxString text =
    wxGetTextFromUser("Please enter the factor, by which the duration shall be changed (e.g. 1.2 for a 20% increase).",
    "Enter length factor", "1.0");

  if (text.IsEmpty())   // The user pressed "Cancel"
  {
    return;
  }

  double factor = 1.0;
  if (text.ToDouble(&factor) == false)
  {
    wxMessageBox("Error: Invalid number!", "Error");
    return;
  }

  data->gesturalScore->changeDuration(factor);
  updateWidgets();
}


// ****************************************************************************
// ****************************************************************************

void MainWindow::OnChangeGesturalScoreTimeConstants(wxCommandEvent &event)
{
  wxString text =
    wxGetTextFromUser("Please enter the factor, by which all the time constants of "
    "the gestural score (except for F0) shall be changed (e.g. 1.2 for a 20% increase).",
    "Enter time constant factor", "1.0");

  if (text.IsEmpty())   // The user pressed "Cancel"
  {
    return;
  }

  double factor = 1.0;
  if (text.ToDouble(&factor) == false)
  {
    wxMessageBox("Error: Invalid number!", "Error");
    return;
  }

  data->gesturalScore->changeTimeConstants(factor);
  updateWidgets();
}


// ****************************************************************************
// ****************************************************************************

void MainWindow::OnExportAreaFunction(wxCommandEvent &event)
{
  wxFileDialog dialog(this, "Save area function", 
    exportFileName.GetPath(), exportFileName.GetFullName(), 
    "Area function (*.txt)|*.txt", wxFD_SAVE | wxFD_OVERWRITE_PROMPT);

  if (dialog.ShowModal() == wxID_OK)
  {
    exportFileName = wxFileName(dialog.GetPath());
    saveAreaFunction(exportFileName.GetFullPath());
  }
}

// ****************************************************************************
// ****************************************************************************

void MainWindow::OnExportCrossSections(wxCommandEvent &event)
{
  wxFileDialog dialog(this, "Save cross sections",
    exportFileName.GetPath(), exportFileName.GetFullName(),
    "Cross section (*.txt)|*.txt", wxFD_SAVE | wxFD_OVERWRITE_PROMPT);

  if (dialog.ShowModal() == wxID_OK)
  {
    exportFileName = wxFileName(dialog.GetPath());
    if (data->vocalTract->exportCrossSections(exportFileName.GetFullPath().ToStdString()) == false)
    {
      wxMessageBox("Failed to export cross section file.", "Error");
    }
  }
}

// ****************************************************************************
// ****************************************************************************

void MainWindow::OnExportModelSvg(wxCommandEvent &event)
{


}


// ****************************************************************************
// ****************************************************************************

void MainWindow::OnExportWireframeModelSvg(wxCommandEvent &event)
{
  wxFileName fileName(data->svgFileName);

  wxString name = wxFileSelector("Save wireframe vocal tract", fileName.GetPath(), 
    fileName.GetFullName(), ".svg", "SVG files (*.svg)|*.svg", 
    wxFD_SAVE | wxFD_OVERWRITE_PROMPT, this);

  if (name.empty() == false)
  {
    data->svgFileName = name;
    VocalTractDialog::getInstance(this)->getVocalTractPicture()->exportTractWireframeSVG(name, -1);
  }
}


// ****************************************************************************
/// Exports the 3D-surfaces of the vocal tract model as obj-files.
/// This is the format of 3D data of the software wavefront.
// ****************************************************************************

void MainWindow::OnExportModelObj(wxCommandEvent &event)
{
  wxFileDialog dialog(this, "Save 3d model surfaces", 
    exportFileName.GetPath(), exportFileName.GetFullName(), 
    "Wavefront OBJ file (*.obj)|*.obj", wxFD_SAVE | wxFD_OVERWRITE_PROMPT);

  if (dialog.ShowModal() == wxID_OK)
  {
    VocalTract *vt = data->vocalTract;
    vt->calculateAll();

    exportFileName = wxFileName(dialog.GetPath());

    bool renderBothSides = VocalTractDialog::getInstance(this)->getVocalTractPicture()->renderBothSides;
    data->vocalTract->saveAsObjFile(exportFileName.GetFullPath().ToStdString(), renderBothSides);
  }
  
}


// ****************************************************************************
/// Export the vocal tract contour as svg file.
// ****************************************************************************

void MainWindow::OnExportContourSvg(wxCommandEvent &event)
{
  wxFileName fileName(data->svgFileName);

  wxString name = wxFileSelector("Save vocal tract contour", fileName.GetPath(), 
    fileName.GetFullName(), ".svg", "SVG files (*.svg)|*.svg", 
    wxFD_SAVE | wxFD_OVERWRITE_PROMPT, this);

  if (name.empty() == false)
  {
    data->svgFileName = name;
    VocalTractPicture *pic = VocalTractDialog::getInstance(this)->getVocalTractPicture();
    data->vocalTract->exportTractContourSvg(name.ToStdString(), 
      pic->showCenterLine, pic->showCutVectors);


//    VocalTractDialog::getInstance(this)->getVocalTractPicture()->exportTractContourSVG(name);



  }
}


// ****************************************************************************
// ****************************************************************************

void MainWindow::OnExportPrimarySpectrum(wxCommandEvent &event)
{
  wxFileName fileName(data->spectrumFileName);

  wxString name = wxFileSelector("Save primary spectrum", fileName.GetPath(), 
    fileName.GetFullName(), ".txt", "Text files (*.txt)|*.txt", 
    wxFD_SAVE | wxFD_OVERWRITE_PROMPT, this);

  if (name.empty() == false)
  {
    data->spectrumFileName = name;
    saveSpectrum(name, data->primarySpectrum);
  }
}


// ****************************************************************************
// ****************************************************************************

void MainWindow::OnExportSecondarySpectrum(wxCommandEvent &event)
{
  wxFileName fileName(data->spectrumFileName);

  wxString name = wxFileSelector("Save user spectrum", fileName.GetPath(), 
    fileName.GetFullName(), ".txt", "Text files (*.txt)|*.txt", 
    wxFD_SAVE | wxFD_OVERWRITE_PROMPT, this);

  if (name.empty() == false)
  {
    data->spectrumFileName = name;
    saveSpectrum(name, data->userSpectrum);
  }
}


// ****************************************************************************
// ****************************************************************************

void MainWindow::OnExportEmaTrajectories(wxCommandEvent &event)
{
  wxFileName fileName(data->emaFileName);

  wxString name = wxFileSelector("Save EMA trajectories from gestural score", fileName.GetPath(), 
    fileName.GetFullName(), ".txt", "Text files (*.txt)|*.txt", 
    wxFD_SAVE | wxFD_OVERWRITE_PROMPT, this);

  if (name.empty() == false)
  {
    data->emaFileName = name;
    data->exportEmaTrajectories(name);
  }
}


// ****************************************************************************
// ****************************************************************************

void MainWindow::OnExportVocalTractVideoFrames(wxCommandEvent &event)
{
  wxDirDialog dialog(this, "Select a folder for the video frames");
  dialog.SetPath(data->videoFramesFolder);
  if (dialog.ShowModal() == wxID_OK)
  {
    data->videoFramesFolder = dialog.GetPath();
    data->exportVocalTractVideoFrames(dialog.GetPath());
  }
}


// ****************************************************************************
// ****************************************************************************

void MainWindow::OnExportTransferFunctionsFromScore(wxCommandEvent &event)
{
  wxFileName fileName(data->spectrumFileName);

  wxString name = wxFileSelector("Save sequence of transfer functions", fileName.GetPath(),
    fileName.GetFullName(), ".txt", "Text files (*.txt)|*.txt",
    wxFD_SAVE | wxFD_OVERWRITE_PROMPT, this);

  if (name.empty() == false)
  {
    data->spectrumFileName = name;
    data->exportTransferFunctionsFromScore(name);
  }
}


// ****************************************************************************
// ****************************************************************************

void MainWindow::OnShowVocalTractDialog(wxCommandEvent &event)
{
  VocalTractDialog *dialog = VocalTractDialog::getInstance(this);
  dialog->Show(true);
}


// ****************************************************************************
// ****************************************************************************

void MainWindow::OnShowVocalTractShapes(wxCommandEvent &event)
{
  VocalTractShapesDialog *dialog = VocalTractShapesDialog::getInstance();
  dialog->Show(true);
}


// ****************************************************************************
// ****************************************************************************

void MainWindow::OnShowPhoneticParameters(wxCommandEvent &event)
{
  PhoneticParamsDialog *dialog = PhoneticParamsDialog::getInstance();
  dialog->SetParent(this);
  dialog->Show(true);
}


// ****************************************************************************
// ****************************************************************************

void MainWindow::OnShowAnatomyParameters(wxCommandEvent &event)
{
  AnatomyParamsDialog *dialog = AnatomyParamsDialog::getInstance();
  dialog->SetParent(this);
  dialog->Show(true);
}


// ****************************************************************************
// ****************************************************************************

void MainWindow::OnShowFdsOptionsDialog(wxCommandEvent &event)
{
  FdsOptionsDialog *dialog = FdsOptionsDialog::getInstance();
  dialog->SetParent(vocalTractPage);
  dialog->Show(true);
}


// ****************************************************************************
// ****************************************************************************

void MainWindow::OnShowTdsOptionsDialog(wxCommandEvent &event)
{
  TdsOptionsDialog *dialog = TdsOptionsDialog::getInstance();
  dialog->Show(true);
}


// ****************************************************************************
// ****************************************************************************

void MainWindow::OnShowVocalFoldDialog(wxCommandEvent &event)
{
  GlottisDialog *dialog = GlottisDialog::getInstance();
  dialog->SetParent(this);
  dialog->Show(true);
}


// ****************************************************************************
// ****************************************************************************

void MainWindow::OnShowLfModelDialog(wxCommandEvent &event)
{
  LfPulseDialog *dialog = LfPulseDialog::getInstance();
  dialog->Show(true);
}


// ****************************************************************************
// ****************************************************************************

void MainWindow::OnGesturalScoreFileToAudio(wxCommandEvent &event)
{
  wxFileName fileName(data->gesturalScoreFileName);

  wxString name = wxFileSelector("Load gestural score", fileName.GetPath(),
    fileName.GetFullName(), ".ges", "Gestural scores (*.ges)|*.ges",
    wxFD_OPEN | wxFD_FILE_MUST_EXIST, this);

  if (name.empty())
  {
    return;
  }

  data->gesturalScoreFileName = name;

  // Create a temporary gestural score for the synthesis.
  GesturalScore *gs = new GesturalScore(data->vocalTract, data->getSelectedGlottis());

  bool allValuesInRange = true;

  if (gs->loadGesturesXml(name.ToStdString(), allValuesInRange) == false)
  {
    wxMessageBox("Loading the gestural score failed!", "Error!");
    delete gs;
    return;
  }

  if (allValuesInRange == false)
  {
    wxMessageBox("Some gesture values in the score were out of range and have been constricted.", "Warning!");
  }

  // ****************************************************************
  // Do the actual synthesis.
  // ****************************************************************

  vector<double> audio;
  wxBusyInfo wait("Please wait...");

  Synthesizer::synthesizeGesturalScore(gs, data->tdsModel, audio);

  data->track[Data::MAIN_TRACK]->setZero();
  Synthesizer::copySignal(audio, *data->track[Data::MAIN_TRACK], 0);

  // Set the selection to the newly synthesized gestural score.
  data->selectionMark_pt[0] = 0;
  data->selectionMark_pt[1] = audio.size() + 8820;  // Include 200 ms silence at the end.

  updateWidgets();

  // Delete the temporary gestural score.
  delete gs;
}



// ****************************************************************************
// ****************************************************************************

void MainWindow::OnTubeSequenceFileToAudio(wxCommandEvent &event)
{
  static wxString tubeSequenceFileName;

  wxFileName fileName(tubeSequenceFileName);

  wxString name = wxFileSelector("Load tube sequence file", fileName.GetPath(),
    fileName.GetFullName(), ".txt", "Tube sequence files (*.txt)|*.txt",
    wxFD_OPEN | wxFD_FILE_MUST_EXIST, this);

  if (name.empty())
  {
    return;
  }

  tubeSequenceFileName = name;

  // ****************************************************************
  // Do the actual synthesis.
  // ****************************************************************

  vector<double> audio;
  bool ok;

  {
    // Use a special scope so that the info window disappears after the synthesis.
    wxBusyInfo wait = ("Please wait...");
    ok = Synthesizer::synthesizeTubeSequence(name.ToStdString(),
      data->getSelectedGlottis(), data->tdsModel, audio);
  }

  if (ok)
  {
    data->track[Data::MAIN_TRACK]->setZero();
    Synthesizer::copySignal(audio, *data->track[Data::MAIN_TRACK], 0);

    // Set the selection to the newly synthesized utterance.
    data->selectionMark_pt[0] = 0;
    data->selectionMark_pt[1] = audio.size() + 8820;  // Include 200 ms silence at the end.
  }
  else
  {
    wxYield();
    wxMessageBox("Synthesizing the tube sequence failed.", "Error");
  }

  updateWidgets();
}


// ****************************************************************************
// ****************************************************************************

void MainWindow::OnTractSequenceFileToAudio(wxCommandEvent &event)
{
  static wxString tractSequenceFileName;
  
  wxFileName fileName(tractSequenceFileName);

  wxString name = wxFileSelector("Load tract sequence file", fileName.GetPath(),
    fileName.GetFullName(), ".txt", "Tract sequence files (*.txt)|*.txt",
    wxFD_OPEN | wxFD_FILE_MUST_EXIST, this);

  if (name.empty())
  {
    return;
  }

  tractSequenceFileName = name;

  // ****************************************************************
  // Do the actual synthesis.
  // ****************************************************************

  vector<double> audio;
  bool ok;

  {
    // Use a special scope so that the info window disappears after the synthesis.
    wxBusyInfo info("Please wait...");
    ok = Synthesizer::synthesizeTractSequence(name.ToStdString(),
      data->getSelectedGlottis(), data->vocalTract, data->tdsModel, audio);
  }

  if (ok)
  {
    data->track[Data::MAIN_TRACK]->setZero();
    Synthesizer::copySignal(audio, *data->track[Data::MAIN_TRACK], 0);

    // Set the selection to the newly synthesized utterance.
    data->selectionMark_pt[0] = 0;
    data->selectionMark_pt[1] = audio.size() + 8820;  // Include 200 ms silence at the end.
  }
  else
  {
    wxYield();
    wxMessageBox("Synthesizing the tract sequence failed.", "Error");
  }

  updateWidgets();
}


// ****************************************************************************
// ****************************************************************************

void MainWindow::OnGesturalScoreToTubeSequenceFile(wxCommandEvent &event)
{
  wxString name = wxFileSelector("Save tube sequence file", "",
    "", ".txt", "Text files (*.txt)|*.txt",
    wxFD_SAVE | wxFD_OVERWRITE_PROMPT, this);

  if (name.empty())
  {
    return;
  }

  if (Synthesizer::gesturalScoreToTubeSequenceFile(data->gesturalScore, name.ToStdString()) == false)
  {
    wxMessageBox("The tube sequence file could not be saved.", "Error");
  }

  wxPrintf("The tube sequence file has been successfully saved.\n");
}


// ****************************************************************************
// ****************************************************************************

void MainWindow::OnGesturalScoreToTractSequenceFile(wxCommandEvent &event)
{
  wxString name = wxFileSelector("Save tract sequence file", "",
    "", ".txt", "Text files (*.txt)|*.txt",
    wxFD_SAVE | wxFD_OVERWRITE_PROMPT, this);

  if (name.empty())
  {
    return;
  }

  if (Synthesizer::gesturalScoreToTractSequenceFile(data->gesturalScore, name.ToStdString()) == false)
  {
    wxMessageBox("The tract sequence file could not be saved.", "Error");
  }

  wxPrintf("The tract sequence file has been successfully saved.\n");
}


// ****************************************************************************
/// Calculates a value from Hertz to semitones and the other way round.
// ****************************************************************************

void MainWindow::OnHertzToSemitones(wxCommandEvent &event)
{
  wxString valueStr = wxGetTextFromUser(
    "Please enter a numeric frequency value in Hertz (Hz) or semitones (st) !", "Enter a value", "", this);

  if (valueStr.empty())
  {
    return;
  }

  double value = 0.0;
  if(valueStr.ToDouble(&value) == false)
  { 
    wxMessageBox(valueStr + " is not a number!", "Error");
    return;
  }

  double newValue_Hz = semitonesToHertz(value);
  double newValue_st = hertzToSemitones(value);

  wxString message = wxString::Format("%2.1f st = %2.1f Hz\n%2.1f Hz = %2.1f st", 
    value, newValue_Hz, value, newValue_st);

  wxMessageBox(message, "Result");
}


// ****************************************************************************
// ****************************************************************************

void MainWindow::OnAbout(wxCommandEvent &event)
{
  wxString st = "Date of this build: " + wxString(__DATE__);

  wxMessageDialog dialog(this, 
    "This is VocalTractLab 2.3, developed by Peter Birkholz.\n\n" + st);
  dialog.ShowModal();
}


// ****************************************************************************
// ****************************************************************************

void MainWindow::OnRecord(wxCommandEvent &event)
{
  if (waveStartRecording(data->track[Data::MAIN_TRACK]->x, data->track[Data::MAIN_TRACK]->N))
  {
    SilentMessageBox dialog("Press [OK] to stop recording!", "Stop recording", this);
    dialog.ShowModal();
    waveStopRecording();
  }
  else
  {
    SilentMessageBox dialog("Recording failed.", "Attention!", this);
    dialog.ShowModal();
  }

  updateWidgets();
}

// ****************************************************************************
// ****************************************************************************

void MainWindow::OnPlayAll(wxCommandEvent &event)
{
  if (waveStartPlaying(data->track[Data::MAIN_TRACK]->x, data->track[Data::MAIN_TRACK]->N, true))
  {
    SilentMessageBox dialog("Press OK to stop playing!", "Stop playing");
    dialog.ShowModal();
    waveStopPlaying();
  }
  else
  {
    wxMessageBox("Playing failed.", "Attention!", wxOK, this);
  }
}

// ****************************************************************************
// ****************************************************************************

void MainWindow::OnPlayPart(wxCommandEvent &event)
{
  if (data->isValidSelection() == false)
  {
    wxMessageBox("You must select the region to be played.", "Invalid selection");
    return;
  }

  int left = data->selectionMark_pt[0];
  int right = data->selectionMark_pt[1];

  if (right > left)
  {
    if (waveStartPlaying(&data->track[Data::MAIN_TRACK]->x[left], right-left+1, true))
    {
      SilentMessageBox dialog("Press OK to stop playing!", "Stop playing");
      dialog.ShowModal();
      waveStopPlaying();
    }
    else
    {
      wxMessageDialog dialog(this, "Playing failed.", "Attention!");
      dialog.ShowModal();
    }
  }

}

// ****************************************************************************
/// Pops up a context menu to select a particular item to clear.
// ****************************************************************************

void MainWindow::OnClearItems(wxCommandEvent &event)
{
  PopupMenu(clearContextMenu);
}

// ****************************************************************************
/// Clear all kind of user data.
// ****************************************************************************

void MainWindow::OnClearAll(wxCommandEvent &event)
{
  wxMessageDialog dialog(this, "Are you sure that you want to clear all signals?", "Clear all", wxYES_NO);
  if (dialog.ShowModal() == wxID_YES)
  {
    OnClearAudioTracks(event);
    OnClearGesturalScore(event);
    OnClearSegmentSequence(event);
    OnClearAnalysisTracks(event);
  }
}


// ****************************************************************************
// ****************************************************************************

void MainWindow::OnClearMainTrack(wxCommandEvent &event)
{
  int i = Data::MAIN_TRACK;
  
  data->track[i]->setZero();
  data->f0Signal[i].clear();
  data->voiceQualitySignal[i].clear();

  updateWidgets();
}


// ****************************************************************************
// ****************************************************************************

void MainWindow::OnClearEggTrack(wxCommandEvent &event)
{
  int i = Data::EGG_TRACK;
  
  data->track[i]->setZero();
  data->f0Signal[i].clear();
  data->voiceQualitySignal[i].clear();

  updateWidgets();
}


// ****************************************************************************
// ****************************************************************************

void MainWindow::OnClearExtraTrack(wxCommandEvent &event)
{
  int i = Data::EXTRA_TRACK;
  
  data->track[i]->setZero();
  data->f0Signal[i].clear();
  data->voiceQualitySignal[i].clear();

  updateWidgets();
}


// ****************************************************************************
// ****************************************************************************

void MainWindow::OnClearAudioTracks(wxCommandEvent &event)
{
  int i;
  for (i=0; i < Data::NUM_TRACKS; i++) 
  { 
    data->track[i]->setZero();
  }
  OnClearAnalysisTracks(event);
  
  updateWidgets();
}


// ****************************************************************************
// ****************************************************************************

void MainWindow::OnClearGesturalScore(wxCommandEvent &event)
{
  data->gesturalScore->clear();
  updateWidgets();
}


// ****************************************************************************
// ****************************************************************************

void MainWindow::OnClearSegmentSequence(wxCommandEvent &event)
{
  data->segmentSequence->clear();
  updateWidgets();
}


// ****************************************************************************
// ****************************************************************************

void MainWindow::OnClearAnalysisTracks(wxCommandEvent &event)
{
  int i;
  for (i=0; i < Data::NUM_TRACKS; i++) 
  { 
    data->f0Signal[i].clear();
    data->voiceQualitySignal[i].clear();
  }

  updateWidgets();
}


// ****************************************************************************
// ****************************************************************************

void MainWindow::OnKeyCtrlLeft(wxCommandEvent &event)
{
  if (notebook->GetCurrentPage() == (wxWindow*)signalPage)
  {
    signalPage->scrollLeft();
  }
  else
  if (notebook->GetCurrentPage() == (wxWindow*)gesturalScorePage)
  {
    gesturalScorePage->scrollLeft();
  }
}

// ****************************************************************************
// ****************************************************************************

void MainWindow::OnKeyCtrlRight(wxCommandEvent &event)
{
  if (notebook->GetCurrentPage() == (wxWindow*)signalPage)
  {
    signalPage->scrollRight();
  }
  else
  if (notebook->GetCurrentPage() == (wxWindow*)gesturalScorePage)
  {
    gesturalScorePage->scrollRight();
  }
}

// ****************************************************************************
// ****************************************************************************

void MainWindow::OnKeyShiftLeft(wxCommandEvent &event)
{
  if (notebook->GetCurrentPage() == (wxWindow*)gesturalScorePage)
  {
    gesturalScorePage->selectNextLeftSegment();
  }
}

// ****************************************************************************
// ****************************************************************************

void MainWindow::OnKeyShiftRight(wxCommandEvent &event)
{
  if (notebook->GetCurrentPage() == (wxWindow*)gesturalScorePage)
  {
    gesturalScorePage->selectNextRightSegment();
  }
}

// ****************************************************************************
// ****************************************************************************

void MainWindow::OnKeyCtrlLess(wxCommandEvent &event)
{
  if (notebook->GetCurrentPage() == (wxWindow*)gesturalScorePage)
  {
    gesturalScorePage->togglePicture();    
  }
}


// ****************************************************************************
// ****************************************************************************

void MainWindow::OnKeyCtrlDelete(wxCommandEvent &event)
{
  if (notebook->GetCurrentPage() == (wxWindow*)gesturalScorePage)
  {
    gesturalScorePage->shortenGesturalScore();    
  }
}


// ****************************************************************************
// ****************************************************************************

void MainWindow::OnKeyCtrlInsert(wxCommandEvent &event)
{
  if (notebook->GetCurrentPage() == (wxWindow*)gesturalScorePage)
  {
    gesturalScorePage->lengthenGesturalScore();    
  }
}


// ****************************************************************************
/// The key F6 is for special tasks.
// ****************************************************************************

void MainWindow::OnKeyF6(wxCommandEvent &event)
{

/*
  VocalTractPicture *pic = VocalTractDialog::getInstance(this)->getVocalTractPicture();
  pic->crossSectionWithTongue = !pic->crossSectionWithTongue;
  pic->Refresh();
  wxPrintf("On key F6: vocalTractPicture->crossSectionWithTongue = %d\n", (int)pic->crossSectionWithTongue);
*/
}


// ****************************************************************************
/// The key F7 is for special tasks.
// ****************************************************************************

void MainWindow::OnKeyF7(wxCommandEvent &event)
{

/*
  VocalTractPicture *pic = VocalTractDialog::getInstance(this)->getVocalTractPicture();
  pic->showTongueCrossSections = !pic->showTongueCrossSections;
  pic->Refresh();
  wxPrintf("On key F7: vocalTractPicture->showTongueCrossSections = %d\n", (int)pic->showTongueCrossSections);
*/
}


// ****************************************************************************
/// The key F8 is for saving the constriction buffer.
// ****************************************************************************

void MainWindow::OnKeyF8(wxCommandEvent &event)
{

/*
  static wxString constrictionBufferFileName = "";
  wxFileName fileName(constrictionBufferFileName);

  wxString name = wxFileSelector("Save constriction buffer", fileName.GetPath(),
    fileName.GetFullName(), ".txt", "Text files (*.txt)|*.txt",
    wxFD_SAVE | wxFD_OVERWRITE_PROMPT, this);

  if (name.empty() == false)
  {
    constrictionBufferFileName = name;
    data->tdsModel->saveConstrictionBuffer(name.ToStdString());
  }
*/
}


// ****************************************************************************
/// The key F9 is for special tasks.
// ****************************************************************************

void MainWindow::OnKeyF9(wxCommandEvent& event)
{

}


// ****************************************************************************
/// The key F11 is for special tasks.
// ****************************************************************************

void MainWindow::OnKeyF11(wxCommandEvent &event)
{

}


// ****************************************************************************
/// The key F12 is for special tasks.
// ****************************************************************************

void MainWindow::OnKeyF12(wxCommandEvent &event)
{

}

// ****************************************************************************

