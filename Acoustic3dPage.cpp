#include "Acoustic3dPage.h"
#include "ParamSimu3DDialog.h"
#include "Backend/SoundLib.h"

#include <stdio.h>
#include <math.h>
#include <wx/config.h>
#include <wx/statline.h>
#include <wx/splitter.h>
#include <wx/gbsizer.h>
#include <wx/clipbrd.h>
#include <wx/filename.h>

#include <chrono>    // to get the computation time
#include <ctime>  

// for eigen
#include <Eigen/Dense>
#include <unsupported/Eigen/MatrixFunctions>


using namespace std;

// ****************************************************************************
// Ids.
// ****************************************************************************

// Left side controls

//static const int IDB_RUN_TEST_JUNCTION = 5995;
//static const int IDB_RUN_RAD_IMP = 5996;
static const int IDB_RUN_TEST_MATRIX_E = 5997;
static const int IDB_RUN_TEST_DISCONTINUITY = 5998;
static const int IDB_RUN_TEST_ELEPHANT = 5999;
static const int IDB_RUN_STATIC_SIMULATION = 6000;
static const int IDB_COMPUTE_MODES = 6001;
static const int IDB_SHAPES_DIALOG = 6002;
static const int IDB_IMPORT_GEOMETRY = 6003;
static const int IDB_PARAM_SIMU_DIALOG = 6004;
static const int IDB_PLAY_LONG_VOWEL = 6005;
static const int IDB_PLAY_NOISE_SOURCE = 6006;
static const int IDB_COMPUTE_ACOUSTIC_FIELD = 6007;
static const int IDB_EXPORT_TF = 6008;
static const int IDB_EXPORT_FIELD = 6009;

// Main panel controls
static const int IDB_SHOW_LOWER_ORDER_MODE = 6010;
static const int IDB_SHOW_MESH = 6011;
static const int IDB_SHOW_MODE = 6012;
static const int IDB_SHOW_TRANSVERS_FIELD = 6013;
//static const int IDB_SHOW_F = 6013;
static const int IDB_SHOW_HIGHER_ORDER_MODE = 6014;

// segments picture controls
static const int IDB_SHOW_PREVIOUS_SEGMENT = 6015;
static const int IDB_SHOW_NEXT_SEGMENT = 6016;
static const int IDB_SHOW_SEGMENTS = 6017;
static const int IDB_SHOW_FIELD = 6018;

// Spectrum panel controls
static const int IDB_UPPER_SPECTRUM_LIMIT_PLUS		= 7000;
static const int IDB_UPPER_SPECTRUM_LIMIT_MINUS		= 7001;
static const int IDB_LOWER_SPECTRUM_LIMIT_PLUS		= 7002;
static const int IDB_LOWER_SPECTRUM_LIMIT_MINUS		= 7003;
static const int IDB_FREQUENCY_RANGE_MINUS			  = 7006;
static const int IDB_FREQUENCY_RANGE_PLUS 		  	= 7007;

// ****************************************************************************
// The event table.
// ****************************************************************************

BEGIN_EVENT_TABLE(Acoustic3dPage, wxPanel)

  // Custom event handler for update requests by child widgets.
  EVT_COMMAND(wxID_ANY, updateRequestEvent, Acoustic3dPage::OnUpdateRequest)

  // Left side controls

  //EVT_BUTTON(IDB_RUN_TEST_JUNCTION, Acoustic3dPage::OnRunTestJunction)
  //EVT_BUTTON(IDB_RUN_RAD_IMP, Acoustic3dPage::OnRunTestRadImp)
  EVT_BUTTON(IDB_RUN_TEST_MATRIX_E, Acoustic3dPage::OnRunTestMatrixE)
  //EVT_BUTTON(IDB_RUN_TEST_DISCONTINUITY, Acoustic3dPage::OnRunTestDiscontinuity)
  EVT_BUTTON(IDB_RUN_TEST_ELEPHANT, Acoustic3dPage::OnRunTestElephant)
  EVT_BUTTON(IDB_SHAPES_DIALOG, Acoustic3dPage::OnShapesDialog)
  EVT_BUTTON(IDB_IMPORT_GEOMETRY, Acoustic3dPage::OnImportGeometry)
  EVT_BUTTON(IDB_PARAM_SIMU_DIALOG, Acoustic3dPage::OnParamSimuDialog)
  EVT_BUTTON(IDB_RUN_STATIC_SIMULATION, Acoustic3dPage::OnRunStaticSimulation)
  EVT_BUTTON(IDB_PLAY_LONG_VOWEL, Acoustic3dPage::OnPlayLongVowel)
  EVT_BUTTON(IDB_PLAY_NOISE_SOURCE, Acoustic3dPage::OnPlayNoiseSource)
  EVT_BUTTON(IDB_COMPUTE_MODES, Acoustic3dPage::OnComputeModes)
  EVT_BUTTON(IDB_COMPUTE_ACOUSTIC_FIELD, Acoustic3dPage::OnComputeAcousticField)
  EVT_BUTTON(IDB_EXPORT_TF, Acoustic3dPage::OnExportTf)
  EVT_BUTTON(IDB_EXPORT_FIELD, Acoustic3dPage::OnExportField)

  // Main panel controls
  EVT_BUTTON(IDB_SHOW_LOWER_ORDER_MODE, Acoustic3dPage::OnShowLowerOrderMode)
  EVT_CHECKBOX(IDB_SHOW_MESH, Acoustic3dPage::OnShowMesh)
  EVT_CHECKBOX(IDB_SHOW_MODE, Acoustic3dPage::OnShowMode)
  EVT_CHECKBOX(IDB_SHOW_TRANSVERS_FIELD, Acoustic3dPage::OnShowTransField)
  //EVT_CHECKBOX(IDB_SHOW_F, Acoustic3dPage::OnShowF)
  EVT_BUTTON(IDB_SHOW_HIGHER_ORDER_MODE, Acoustic3dPage::OnShowHigherOrderMode)

  // segments picture controls
  EVT_BUTTON(IDB_SHOW_PREVIOUS_SEGMENT, Acoustic3dPage::OnShowPreviousSegment)
  EVT_BUTTON(IDB_SHOW_NEXT_SEGMENT, Acoustic3dPage::OnShowNextSegment)
  EVT_CHECKBOX(IDB_SHOW_SEGMENTS, Acoustic3dPage::OnShowSegments)
  EVT_CHECKBOX(IDB_SHOW_FIELD, Acoustic3dPage::OnShowField)

  // Spectrum panel controls
  EVT_BUTTON(IDB_UPPER_SPECTRUM_LIMIT_PLUS, Acoustic3dPage::OnUpperSpectrumLimitPlus)
  EVT_BUTTON(IDB_UPPER_SPECTRUM_LIMIT_MINUS, Acoustic3dPage::OnUpperSpectrumLimitMinus)
  EVT_BUTTON(IDB_LOWER_SPECTRUM_LIMIT_PLUS, Acoustic3dPage::OnLowerSpectrumLimitPlus)
  EVT_BUTTON(IDB_LOWER_SPECTRUM_LIMIT_MINUS, Acoustic3dPage::OnLowerSpectrumLimitMinus)
  EVT_BUTTON(IDB_FREQUENCY_RANGE_MINUS, Acoustic3dPage::OnFrequencyRangeMinus)
  EVT_BUTTON(IDB_FREQUENCY_RANGE_PLUS, Acoustic3dPage::OnFrequencyRangePlus)
END_EVENT_TABLE()

// ****************************************************************************
/// Constructor. 
// ****************************************************************************

Acoustic3dPage::Acoustic3dPage(wxWindow* parent, VocalTractPicture *picVocalTract) :
  wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxCLIP_CHILDREN)
{
  initVars();
  initWidgets(picVocalTract);
  updateWidgets();
}

// ****************************************************************************
// ****************************************************************************

// ****************************************************************************
/// Update the widgets
// ****************************************************************************

void Acoustic3dPage::updateWidgets()
{
  VocalTract* tract = data->vocalTract;

  // display selected options for mode picture
  chkShowMesh->SetValue(picPropModes->meshSelected());
  chkShowMode->SetValue(picPropModes->modeSelected());
  chkShowTransField->SetValue(picPropModes->fieldSelected());
  //chkShowF->SetValue(picPropModes->fSelected());

  // options for segment picture
  segPic->setShowSegments(chkShowSegments->GetValue());
  segPic->setShowField(chkShowField->GetValue());

  picPropModes->Refresh();
  picSpectrum->Refresh();
  segPic->Refresh();
}

// ****************************************************************************
/// Init some variables.
// ****************************************************************************

void Acoustic3dPage::initVars()
{
  data = Data::getInstance();
  simu3d = Acoustic3dSimulation::getInstance();
}

// ****************************************************************************
/// Init the widgets.
// ****************************************************************************

void Acoustic3dPage::initWidgets(VocalTractPicture* picVocalTract)
{
  wxSizer* sizer;
  wxButton* button;

// ****************************************************************
// Top level sizer.
// ****************************************************************

  wxBoxSizer* topLevelSizer = new wxBoxSizer(wxHORIZONTAL);

// ****************************************************************
// Left side sizer.
// ****************************************************************

  wxBoxSizer* leftSizer = new wxBoxSizer(wxVERTICAL);

  button = new wxButton(this, IDB_PARAM_SIMU_DIALOG, "Simulation parameters");
  leftSizer->Add(button, 0, wxGROW | wxALL, 3);

  button = new wxButton(this, IDB_SHAPES_DIALOG, "Vocal tract shapes");
  leftSizer->Add(button, 0, wxGROW | wxALL, 3);

  button = new wxButton(this, IDB_IMPORT_GEOMETRY, "Import geometry");
  leftSizer->Add(button, 0, wxGROW | wxALL, 3);

  leftSizer->AddSpacer(20);

  //button = new wxButton(this, IDB_RUN_TEST_JUNCTION, "Run test junction");
  //leftSizer->Add(button, 0, wxGROW | wxALL, 3);

  //button = new wxButton(this, IDB_RUN_RAD_IMP, "Run test scale rad imped");
  //leftSizer->Add(button, 0, wxGROW | wxALL, 3);

  button = new wxButton(this, IDB_RUN_TEST_MATRIX_E, "Run test matrix E");
  leftSizer->Add(button, 0, wxGROW | wxALL, 3);

  //button = new wxButton(this, IDB_RUN_TEST_DISCONTINUITY, "Run test discontinuity");
  //leftSizer->Add(button, 0, wxGROW | wxALL, 3);

  button = new wxButton(this, IDB_RUN_TEST_ELEPHANT, "Run test elphant");
  leftSizer->Add(button, 0, wxGROW | wxALL, 3);

  button = new wxButton(this, IDB_COMPUTE_MODES, "Compute modes");
  leftSizer->Add(button, 0, wxGROW | wxALL, 3);

  button = new wxButton(this, IDB_RUN_STATIC_SIMULATION, "Compute transfer function");
  leftSizer->Add(button, 0, wxGROW | wxALL, 3);

  button = new wxButton(this, IDB_COMPUTE_ACOUSTIC_FIELD, "Compute acoustic field");
  leftSizer->Add(button, 0, wxGROW | wxALL, 3);

  leftSizer->AddSpacer(20);
  
  button = new wxButton(this, IDB_EXPORT_TF, "Export transfer function");
  leftSizer->Add(button, 0, wxGROW | wxALL, 3);

  button = new wxButton(this, IDB_EXPORT_FIELD, "Export acoustic field");
  leftSizer->Add(button, 0, wxGROW | wxALL, 3);

  leftSizer->AddSpacer(20);
  
  button = new wxButton(this, IDB_PLAY_LONG_VOWEL, "Play long vowel");
  leftSizer->Add(button, 0, wxGROW | wxALL, 3);

  button = new wxButton(this, IDB_PLAY_NOISE_SOURCE, "Play fricative");
  leftSizer->Add(button, 0, wxGROW | wxALL, 3);

// ****************************************************************

  topLevelSizer->Add(leftSizer, 0, wxALL, 5);

// ****************************************************************
// Static line to separate the left and right part.
// ****************************************************************

  wxStaticLine* verticalLine = new wxStaticLine(this, wxID_ANY, wxDefaultPosition,
    wxDefaultSize, wxLI_VERTICAL);

  topLevelSizer->Add(verticalLine, 0, wxGROW | wxALL, 2);

// ****************************************************************
// Main splitter on the right sizer.
// ****************************************************************

  splitter = new wxSplitterWindow(this, wxID_ANY,
    wxDefaultPosition, wxDefaultSize, wxSP_3DSASH | wxSP_LIVE_UPDATE);

  splitter->SetBackgroundColour(*wxWHITE); // If this is not set to white, every label will show up with a white background against the gray panels.
  splitter->SetMinimumPaneSize(200);   // Pane size may not be reduced to zero!

  wxPanel* topPanel = new wxPanel(splitter, wxID_ANY, wxDefaultPosition, wxDefaultSize);
  wxPanel* bottomPanel = new wxPanel(splitter, wxID_ANY, wxDefaultPosition, wxDefaultSize);
  splitter->SplitHorizontally(topPanel, bottomPanel);
  splitter->SetSashPosition(550);

  topLevelSizer->Add(splitter, 1, wxEXPAND);

// ****************************************************************
// Create the top panel (area function picture and cross-section
// picture).
// ****************************************************************

  wxBoxSizer* topSizer = new wxBoxSizer(wxHORIZONTAL);
  topPanel->SetSizer(topSizer);

  //wxBoxSizer* topLeftSizer = new wxBoxSizer(wxVERTICAL);

  //picAreaFunction = new AreaFunctionPicture(topPanel, picVocalTract, this);
  //picAreaFunction->showSideBranches = false;
  //topLeftSizer->Add(picAreaFunction, 1, wxEXPAND | wxALL, 2);

  //topSizer->Add(topLeftSizer, 2, wxEXPAND | wxALL, 2);

  // ****************************************************************

  wxBoxSizer* middleSizer = new wxBoxSizer(wxVERTICAL);

  segPic = new SegmentsPicture(topPanel, simu3d, this);
  middleSizer->Add(segPic, 1, wxEXPAND | wxALL, 2);

  // segment options sizer

  sizer = new wxBoxSizer(wxHORIZONTAL);

  sizer->AddStretchSpacer(1);

  button = new wxButton(topPanel, IDB_SHOW_PREVIOUS_SEGMENT, "<", wxDefaultPosition, wxSize(35, 35));
  sizer->Add(button, 0, wxALL, 2);

  sizer->AddStretchSpacer(1);

  chkShowSegments = new wxCheckBox(topPanel, IDB_SHOW_SEGMENTS, "Show segments");
  sizer->Add(chkShowSegments, 0, wxALIGN_BOTTOM | wxALL, 2);
  chkShowSegments->SetValue(true);

  sizer->AddStretchSpacer(1);

  chkShowField = new wxCheckBox(topPanel, IDB_SHOW_FIELD, "Show field");
  sizer->Add(chkShowField, 0, wxALIGN_BOTTOM | wxALL, 2);

  sizer->AddStretchSpacer(1);

  button = new wxButton(topPanel, IDB_SHOW_NEXT_SEGMENT, ">", wxDefaultPosition, wxSize(35, 35));
  sizer->Add(button, 0, wxALL, 2);

  sizer->AddStretchSpacer(1);

  middleSizer->Add(sizer, 0, wxEXPAND | wxALIGN_CENTER_HORIZONTAL);
  topSizer->Add(middleSizer, 1, wxEXPAND | wxALL, 2);

  // ****************************************************************

  wxBoxSizer* topRightSizer = new wxBoxSizer(wxVERTICAL);

  picPropModes = new PropModesPicture(topPanel, simu3d, segPic);
  topRightSizer->Add(picPropModes, 1, wxEXPAND | wxALL, 2);

  // propagation modes option sizer

  sizer = new wxBoxSizer(wxHORIZONTAL);

  sizer->AddStretchSpacer(1);

  button = new wxButton(topPanel, IDB_SHOW_LOWER_ORDER_MODE, "<", wxDefaultPosition, wxSize(35, 35));
  sizer->Add(button, 0, wxALL, 2);

  sizer->AddStretchSpacer(1);

  chkShowMesh = new wxCheckBox(topPanel, IDB_SHOW_MESH, "Mesh");
  sizer->Add(chkShowMesh, 0, wxALIGN_BOTTOM | wxALL, 2);

  sizer->AddStretchSpacer(1);

  chkShowMode = new wxCheckBox(topPanel, IDB_SHOW_MODE, "Modes");
  sizer->Add(chkShowMode, 0, wxALIGN_BOTTOM | wxALL, 2);

  sizer->AddStretchSpacer(1);

  chkShowTransField = new wxCheckBox(topPanel, IDB_SHOW_TRANSVERS_FIELD, "Field");
  sizer->Add(chkShowTransField, 0, wxALIGN_BOTTOM | wxALL, 2);

  //chkShowF = new wxCheckBox(topPanel, IDB_SHOW_F, "F");
  //sizer->Add(chkShowF, 0, wxALL, 2);

  sizer->AddStretchSpacer(1);
  
  button = new wxButton(topPanel, IDB_SHOW_HIGHER_ORDER_MODE, ">", wxDefaultPosition, wxSize(35, 35));
  sizer->Add(button, 0, wxALL, 2);

  sizer->AddStretchSpacer(1);

  topRightSizer->Add(sizer, 0, wxEXPAND | wxALIGN_CENTER_HORIZONTAL);
  topSizer->Add(topRightSizer, 1, wxEXPAND | wxALL, 2);

  // ****************************************************************
  // Create the bottom panel (spectrum panel).
  // ****************************************************************

  wxBoxSizer *bottomSizer = new wxBoxSizer(wxHORIZONTAL);
  bottomPanel->SetSizer(bottomSizer);

  // the right side

  sizer = new wxBoxSizer(wxVERTICAL);
  button = new wxButton(bottomPanel, IDB_UPPER_SPECTRUM_LIMIT_PLUS, "+", wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
  button->SetMinSize(wxSize(button->GetSize().GetHeight(), button->GetSize().GetHeight()));
  sizer->Add(button, 0, wxALL, 5);
  button = new wxButton(bottomPanel, IDB_UPPER_SPECTRUM_LIMIT_MINUS, "-", wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
  button->SetMinSize(wxSize(button->GetSize().GetHeight(), button->GetSize().GetHeight()));
  sizer->Add(button, 0, wxALL, 5);

  sizer->AddSpacer(5);
  sizer->AddStretchSpacer(1);

  button = new wxButton(bottomPanel, IDB_LOWER_SPECTRUM_LIMIT_PLUS, "+", wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
  button->SetMinSize(wxSize(button->GetSize().GetHeight(), button->GetSize().GetHeight()));
  sizer->Add(button, 0, wxALL, 5);
  button = new wxButton(bottomPanel, IDB_LOWER_SPECTRUM_LIMIT_MINUS, "-", wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
  button->SetMinSize(wxSize(button->GetSize().GetHeight(), button->GetSize().GetHeight()));
  sizer->Add(button, 0, wxALL, 5);

  bottomSizer->Add(sizer, 0, wxGROW | wxALL, 2);

  // The actual spectrum picture

  picSpectrum = new Spectrum3dPicture(bottomPanel, simu3d);
  bottomSizer->Add(picSpectrum, 1, wxGROW | wxTOP | wxBOTTOM, 5);
  
  // the bottom side

  sizer = new wxBoxSizer(wxVERTICAL);
  sizer->AddStretchSpacer(1);

  wxBoxSizer* subSizer = new wxBoxSizer(wxHORIZONTAL);

  button = new wxButton(bottomPanel, IDB_FREQUENCY_RANGE_MINUS, "-", wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
  button->SetMinSize(wxSize(button->GetSize().GetHeight(), button->GetSize().GetHeight()));
  subSizer->Add(button, 0, wxALL, 5);
  button = new wxButton(bottomPanel, IDB_FREQUENCY_RANGE_PLUS, "+", wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
  button->SetMinSize(wxSize(button->GetSize().GetHeight(), button->GetSize().GetHeight()));
  subSizer->Add(button, 0, wxALL, 5);

  sizer->Add(subSizer, 0, wxGROW | wxALL, 2); 

  bottomSizer->Add(sizer, 0, wxGROW | wxALL, 2);

  // ****************************************************************
  // Set the top-level sizer for this window.
  // ****************************************************************

  this->SetSizer(topLevelSizer);
}

// ****************************************************************************
/// Is called, when one of the child windows has requested the update of
/// other child windows.
// ****************************************************************************

void Acoustic3dPage::OnUpdateRequest(wxCommandEvent& event)
{
  ofstream log("log.txt", ofstream::app);
  log << "OnUpdateRequest" << endl;
  log.close();

  if (event.GetInt() == REFRESH_PICTURES)
  {
    segPic->Refresh();
    picPropModes->Refresh();
  }
  else
  if (event.GetInt() == UPDATE_PICTURES)
  {
    segPic->Refresh();
    segPic->Update();
    picPropModes->Refresh();
    picPropModes->Update();
  }
  else
  if (event.GetInt() == REFRESH_PICTURES_AND_CONTROLS)
  {
    updateWidgets();
  }
  else
  if (event.GetInt() == UPDATE_PICTURES_AND_CONTROLS)
  {
    updateWidgets();
    segPic->Update();
    picPropModes->Update();
  }
  else
  if (event.GetInt() == UPDATE_VOCAL_TRACT)
  {
    updateWidgets();
    segPic->Update();
    picPropModes->Update();
  }
}

// ****************************************************************************
// ****************************************************************************

//void Acoustic3dPage::OnRunTestJunction(wxCommandEvent& event)
//{
//  Acoustic3dSimulation* simu3d = Acoustic3dSimulation::getInstance();
//  // load geometry configuration file
//  wxFileName fileName;
//  wxString name = wxFileSelector("Load geometry description file", fileName.GetPath(),
//    fileName.GetFullName(), ".csv", "Geometry file (*.csv)|*.csv",
//    wxFD_OPEN | wxFD_FILE_MUST_EXIST, this);
//
//  if (!name.empty()){
//  simu3d->coneConcatenationSimulation(name.ToStdString());
//  updateWidgets();
//  picAreaFunction->Update();
//  picPropModes->Update();
//  }
//}

// ****************************************************************************
// ****************************************************************************

//void Acoustic3dPage::OnRunTestRadImp(wxCommandEvent& event)
//{
//  Acoustic3dSimulation* simu3d = Acoustic3dSimulation::getInstance();
//  string fileName("file");
//  simu3d->runTest(SCALE_RAD_IMP, fileName);
//  updateWidgets();
//  picAreaFunction->Update();
//  picPropModes->Update();
//}

// ****************************************************************************
// ****************************************************************************

void Acoustic3dPage::OnRunTestMatrixE(wxCommandEvent& event)
{
  Acoustic3dSimulation* simu3d = Acoustic3dSimulation::getInstance();
  string fileName("file");
  simu3d->runTest(MATRIX_E, fileName);
  updateWidgets();
  //picAreaFunction->Update();
  picPropModes->Update();
}

// ****************************************************************************
// ****************************************************************************

//void Acoustic3dPage::OnRunTestDiscontinuity(wxCommandEvent& event)
//{
//  Acoustic3dSimulation* simu3d = Acoustic3dSimulation::getInstance();
//  string fileName("file");
//  simu3d->runTest(DISCONTINUITY, fileName);
//  updateWidgets();
//  picAreaFunction->Update();
//  picPropModes->Update();
//}

// ****************************************************************************
// ****************************************************************************

void Acoustic3dPage::OnRunTestElephant(wxCommandEvent& event)
{
  Acoustic3dSimulation* simu3d = Acoustic3dSimulation::getInstance();
  string fileName("file");
  simu3d->runTest(ELEPHANT_TRUNK, fileName);
  updateWidgets();
  segPic->Update();
  picPropModes->Update();
}

// ****************************************************************************
// ****************************************************************************

void Acoustic3dPage::OnRunStaticSimulation(wxCommandEvent& event)
{
  Data* data = Data::getInstance();
  Acoustic3dSimulation* simu3d = Acoustic3dSimulation::getInstance();
  VocalTract* tract = data->vocalTract;

  simu3d->computeTransferFunction(tract);

  // update pictures
  updateWidgets();
  //picAreaFunction->Update();
  //picPropModes->Update();

  OnPlayLongVowel();
}

// ****************************************************************************
// ****************************************************************************

void Acoustic3dPage::OnComputeModes(wxCommandEvent& event)
{
  Data* data = Data::getInstance();
  Acoustic3dSimulation* simu3d = Acoustic3dSimulation::getInstance();
  VocalTract* tract = data->vocalTract;

  simu3d->generateLogFileHeader(true);
  ofstream log("log.txt", ofstream::app);
  log << "\nStart compute modes" << endl;

  simu3d->createCrossSections(tract, false);
  log << "Geometry succesfully imported" << endl;

  simu3d->exportGeoInCsv("test.csv");
  simu3d->computeMeshAndModes();

  ofstream ofs("modes.txt");
  for (int i(0); i < simu3d->numCrossSections(); i++)
  {
    ofs << simu3d->crossSection(i)->numberOfModes() << endl;
  }
  ofs.close();

  // update pictures
  updateWidgets();
  //picAreaFunction->Update();
  picPropModes->Update();

  log.close();
}

// ****************************************************************************
// ****************************************************************************

void Acoustic3dPage::OnComputeAcousticField(wxCommandEvent& event)
{
  // hide the previous field if it exists
  chkShowField->SetValue(false);
  updateWidgets();

  Data* data = Data::getInstance();
  Acoustic3dSimulation* simu3d = Acoustic3dSimulation::getInstance();
  VocalTract* tract = data->vocalTract;

  simu3d->computeAcousticField(tract);

  // update pictures
  chkShowField->SetValue(true);
  updateWidgets();
  picPropModes->Update();
}

// ****************************************************************************
// ****************************************************************************

void Acoustic3dPage::OnExportTf(wxCommandEvent& event)
{
  wxFileName fileName;
  wxString name = wxFileSelector("Save transfer functions", fileName.GetPath(),
    fileName.GetFullName(), ".txt", "(*.txt)|*.txt",
    wxFD_SAVE | wxFD_OVERWRITE_PROMPT, this);

  simu3d->exportTransferFucntions(name.ToStdString());
}

// ****************************************************************************
// ****************************************************************************

void Acoustic3dPage::OnExportField(wxCommandEvent& event)
{
  wxFileName fileName;
  wxString name = wxFileSelector("Save acoustic field", fileName.GetPath(),
    fileName.GetFullName(), ".txt", "(*.txt)|*.txt",
    wxFD_SAVE | wxFD_OVERWRITE_PROMPT, this);

  simu3d->exportAcousticField(name.ToStdString());
}

// ****************************************************************************
// ****************************************************************************

void Acoustic3dPage::OnParamSimuDialog(wxCommandEvent& event)
{
  ParamSimu3DDialog *dialog = ParamSimu3DDialog::getInstance(NULL, simu3d);
  dialog->SetParent(this);
  dialog->Show(true);
  dialog->Raise();
}

// ****************************************************************************
// ****************************************************************************

void Acoustic3dPage::OnShapesDialog(wxCommandEvent& event)
{
  VocalTractShapesDialog* dialog = VocalTractShapesDialog::getInstance();
  dialog->Show(true);

  simu3d->setGeometryImported(false);
  simu3d->requestModesAndJunctionComputation();

  importGeometry();

  updateWidgets();

  //segPic->resetActiveSegment();
}

// ****************************************************************************
// ****************************************************************************

void Acoustic3dPage::OnImportGeometry(wxCommandEvent& event)
{
  wxFileName fileName;
  wxString name = wxFileSelector("Import geometry as csv file", fileName.GetPath(),
    fileName.GetFullName(), ".csv", "Geometry file (*.csv)|*.csv",
    wxFD_OPEN | wxFD_FILE_MUST_EXIST, this);

  simu3d->setGeometryImported(true);
  simu3d->setContourInterpolationMethod(FROM_FILE);
  simu3d->setGeometryFile(name.ToStdString());
  simu3d->requestModesAndJunctionComputation();

  importGeometry();

  updateWidgets();
}

// ****************************************************************************
// ****************************************************************************

void Acoustic3dPage::OnPlayLongVowel(wxCommandEvent& event)
{

  OnPlayLongVowel();
  //Data* data = Data::getInstance();
  //Acoustic3dSimulation* simu3d = Acoustic3dSimulation::getInstance();
  //ofstream log;
  ////log.open("log.txt", ofstream::app);
  ////log << "Pointer data: " << data << endl;
  ////log << "Pointer simu3d: " << simu3d << endl;

  ////for (int i(0); i < simu3d->spectrum.N; i++)
  ////{
  ////  log << simu3d->spectrum.getMagnitude(i) << endl;
  ////}
  ////log.close();
  ////int duration_ms = data->synthesizeVowelLf(simu3d->spectrum, data->lfPulse, 0, true);
  //int duration_ms = data->synthesizeVowelLf(simu3d, data->lfPulse, 0, true);

  ////log.open("sig.txt");
  ////for (int i(0); i < data->track[Data::MAIN_TRACK]->N; i++)
  ////{
  ////  log << data->track[Data::MAIN_TRACK]->getValue(i) << endl;
  ////}
  ////log.close();

  //data->normalizeAudioAmplitude(Data::MAIN_TRACK);

  ////log.open("sigNorm.txt");
  ////for (int i(0); i < data->track[Data::MAIN_TRACK]->N; i++)
  ////{
  ////  log << data->track[Data::MAIN_TRACK]->getValue(i) << endl;
  ////}
  ////log.close();

  //if (waveStartPlaying(data->track[Data::MAIN_TRACK]->x, data->track[Data::MAIN_TRACK]->N, false))
  //{
  //  wxMilliSleep(duration_ms);
  //  waveStopPlaying();
  //}
  //else
  //{
  //  wxMessageBox("Playing failed.", "Attention!");
  //}
}

// ****************************************************************************
// ****************************************************************************
// overload of OnPlayLongVowel without event 

void Acoustic3dPage::OnPlayLongVowel()
{
  Data* data = Data::getInstance();
  Acoustic3dSimulation* simu3d = Acoustic3dSimulation::getInstance();

  int duration_ms = data->synthesizeVowelLf(simu3d, data->lfPulse, 0, true);

  data->normalizeAudioAmplitude(Data::MAIN_TRACK);

  if (waveStartPlaying(data->track[Data::MAIN_TRACK]->x, data->track[Data::MAIN_TRACK]->N, false))
  {
    wxMilliSleep(duration_ms);
    waveStopPlaying();
  }
  else
  {
    wxMessageBox("Playing failed.", "Attention!");
  }
}

// ****************************************************************************
// ****************************************************************************

void Acoustic3dPage::OnPlayNoiseSource(wxCommandEvent& event)
{
  Data* data = Data::getInstance();
  Acoustic3dSimulation* simu3d = Acoustic3dSimulation::getInstance();

  int duration_ms = data->synthesizeNoiseSource(simu3d, 0);
  data->normalizeAudioAmplitude(Data::MAIN_TRACK);

  if (waveStartPlaying(data->track[Data::MAIN_TRACK]->x, data->track[Data::MAIN_TRACK]->N, false))
  {
    wxMilliSleep(duration_ms);
    waveStopPlaying();
  }
  else
  {
    wxMessageBox("Playing failed.", "Attention!");
  }
}

// ****************************************************************************
// ****************************************************************************

// ****************************************************************
// Check boxes callback functions
// ****************************************************************

void Acoustic3dPage::OnShowMesh(wxCommandEvent& event)
{
  picPropModes->showMesh();
  chkShowMode->SetValue(picPropModes->modeSelected());
  chkShowTransField->SetValue(picPropModes->fieldSelected());
  //chkShowF->SetValue(picPropModes->fSelected());
}

// ****************************************************************************

void Acoustic3dPage::OnShowMode(wxCommandEvent& event)
{
  picPropModes->showMode();
  chkShowMesh->SetValue(picPropModes->meshSelected());
  chkShowTransField->SetValue(picPropModes->fieldSelected());
  //chkShowF->SetValue(picPropModes->fSelected());
}

// ****************************************************************************

void Acoustic3dPage::OnShowTransField(wxCommandEvent& event)
{
  picPropModes->showField();
  chkShowMesh->SetValue(picPropModes->meshSelected());
  chkShowMode->SetValue(picPropModes->modeSelected());
}

// ****************************************************************************

//void Acoustic3dPage::OnShowF(wxCommandEvent& event)
//{
//  picPropModes->showF();
//  chkShowMesh->SetValue(picPropModes->meshSelected());
//  chkShowMode->SetValue(picPropModes->modeSelected());
//}

// ****************************************************************************

void Acoustic3dPage::OnShowLowerOrderMode(wxCommandEvent& event)
{
  picPropModes->setModeIdx(picPropModes->modeIdx() - 1);
}

// ****************************************************************************

void Acoustic3dPage::OnShowHigherOrderMode(wxCommandEvent& event)
{
  picPropModes->setModeIdx(picPropModes->modeIdx() + 1);
}

// ****************************************************************************

void Acoustic3dPage::OnShowPreviousSegment(wxCommandEvent& event)
{
  segPic->showPreivousSegment();
  picPropModes->Refresh();
}

// ****************************************************************************

void Acoustic3dPage::OnShowNextSegment(wxCommandEvent& event)
{
  segPic->showNextSegment();
  picPropModes->Refresh();
}

// ****************************************************************************

void Acoustic3dPage::OnShowSegments(wxCommandEvent& event)
{
  segPic->setShowSegments(chkShowSegments->GetValue());
  segPic->Refresh();
}

// ****************************************************************************

void Acoustic3dPage::OnShowField(wxCommandEvent& event)
{
  segPic->setShowField(chkShowField->GetValue());
  segPic->Refresh();
}

// ****************************************************************************

void Acoustic3dPage::OnUpperSpectrumLimitPlus(wxCommandEvent& event)
{
  picSpectrum->graph.zoomOutOrdinate(false, true);
  picSpectrum->Refresh();
}

// ****************************************************************************

void Acoustic3dPage::OnUpperSpectrumLimitMinus(wxCommandEvent &event)
{
  picSpectrum->graph.zoomInOrdinate(false, true);
  picSpectrum->Refresh();
}

// ****************************************************************************

void Acoustic3dPage::OnLowerSpectrumLimitPlus(wxCommandEvent &event)
{
  picSpectrum->graph.zoomInOrdinate(true, false);
  picSpectrum->Refresh();
}

// ****************************************************************************

void Acoustic3dPage::OnLowerSpectrumLimitMinus(wxCommandEvent &event)
{
  picSpectrum->graph.zoomOutOrdinate(true, false);
  picSpectrum->Refresh();
}

// ****************************************************************************

void Acoustic3dPage::OnFrequencyRangeMinus(wxCommandEvent &event)
{
  picSpectrum->graph.zoomInAbscissa(false, true);
  picSpectrum->Refresh();
}

// ****************************************************************************

void Acoustic3dPage::OnFrequencyRangePlus(wxCommandEvent &event)
{
  picSpectrum->graph.zoomOutAbscissa(false, true);
  picSpectrum->Refresh();
}

// ****************************************************************************

void Acoustic3dPage::importGeometry()
{

  VocalTract* tract = data->vocalTract;

  ofstream log("log.txt", ofstream::app);

  if (simu3d->createCrossSections(tract, false))
  {
    log << "Geometry successfully imported" << endl;
  }
  else
  {
    log << "Importation failed" << endl;
  }

  log.close();

  segPic->resetActiveSegment();
}