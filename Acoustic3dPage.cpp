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

static const int IDB_RUN_TEST_JUNCTION = 5995;
static const int IDB_RUN_RAD_IMP = 5996;
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

// Main panel controls

static const int IDB_SHOW_LOWER_ORDER_MODE = 6010;
static const int IDB_SHOW_MESH = 6011;
static const int IDB_SHOW_MODE = 6012;
//static const int IDB_SHOW_F = 6013;
static const int IDB_SHOW_HIGHER_ORDER_MODE = 6014;

// ****************************************************************************
// The event table.
// ****************************************************************************

BEGIN_EVENT_TABLE(Acoustic3dPage, wxPanel)

  // Custom event handler for update requests by child widgets.
  EVT_COMMAND(wxID_ANY, updateRequestEvent, Acoustic3dPage::OnUpdateRequest)

  // Left side controls

  EVT_BUTTON(IDB_RUN_TEST_JUNCTION, Acoustic3dPage::OnRunTestJunction)
  EVT_BUTTON(IDB_RUN_RAD_IMP, Acoustic3dPage::OnRunTestRadImp)
  EVT_BUTTON(IDB_RUN_TEST_MATRIX_E, Acoustic3dPage::OnRunTestMatrixE)
  EVT_BUTTON(IDB_RUN_TEST_DISCONTINUITY, Acoustic3dPage::OnRunTestDiscontinuity)
  EVT_BUTTON(IDB_RUN_TEST_ELEPHANT, Acoustic3dPage::OnRunTestElephant)
  EVT_BUTTON(IDB_SHAPES_DIALOG, Acoustic3dPage::OnShapesDialog)
  EVT_BUTTON(IDB_IMPORT_GEOMETRY, Acoustic3dPage::OnImportGeometry)
  EVT_BUTTON(IDB_PARAM_SIMU_DIALOG, Acoustic3dPage::OnParamSimuDialog)
  EVT_BUTTON(IDB_RUN_STATIC_SIMULATION, Acoustic3dPage::OnRunStaticSimulation)
  EVT_BUTTON(IDB_PLAY_LONG_VOWEL, Acoustic3dPage::OnPlayLongVowel)
  EVT_BUTTON(IDB_PLAY_NOISE_SOURCE, Acoustic3dPage::OnPlayNoiseSource)
  EVT_BUTTON(IDB_COMPUTE_MODES, Acoustic3dPage::OnComputeModes)
  EVT_BUTTON(IDB_COMPUTE_ACOUSTIC_FIELD, Acoustic3dPage::OnComputeAcousticField)

  // Main panel controls

  EVT_BUTTON(IDB_SHOW_LOWER_ORDER_MODE, Acoustic3dPage::OnShowLowerOrderMode)
  EVT_CHECKBOX(IDB_SHOW_MESH, Acoustic3dPage::OnShowMesh)
  EVT_CHECKBOX(IDB_SHOW_MODE, Acoustic3dPage::OnShowMode)
  //EVT_CHECKBOX(IDB_SHOW_F, Acoustic3dPage::OnShowF)
  EVT_BUTTON(IDB_SHOW_HIGHER_ORDER_MODE, Acoustic3dPage::OnShowHigherOrderMode)

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
  //chkShowF->SetValue(picPropModes->fSelected());

  picAreaFunction->Refresh();
  picPropModes->Refresh();
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

  button = new wxButton(this, IDB_RUN_TEST_JUNCTION, "Run test junction");
  leftSizer->Add(button, 0, wxGROW | wxALL, 3);

  button = new wxButton(this, IDB_RUN_RAD_IMP, "Run test scale rad imped");
  leftSizer->Add(button, 0, wxGROW | wxALL, 3);

  button = new wxButton(this, IDB_RUN_TEST_MATRIX_E, "Run test matrix E");
  leftSizer->Add(button, 0, wxGROW | wxALL, 3);

  button = new wxButton(this, IDB_RUN_TEST_DISCONTINUITY, "Run test discontinuity");
  leftSizer->Add(button, 0, wxGROW | wxALL, 3);

  button = new wxButton(this, IDB_RUN_TEST_ELEPHANT, "Run test elphant");
  leftSizer->Add(button, 0, wxGROW | wxALL, 3);

  button = new wxButton(this, IDB_RUN_STATIC_SIMULATION, "Run static simulation");
  leftSizer->Add(button, 0, wxGROW | wxALL, 3);

  button = new wxButton(this, IDB_COMPUTE_MODES, "Compute modes");
  leftSizer->Add(button, 0, wxGROW | wxALL, 3);

  button = new wxButton(this, IDB_COMPUTE_ACOUSTIC_FIELD, "Compute acoustic field");
  leftSizer->Add(button, 0, wxGROW | wxALL, 3);

  leftSizer->AddSpacer(10);
  
  button = new wxButton(this, IDB_PARAM_SIMU_DIALOG, "Simulation parameters");
  leftSizer->Add(button, 0, wxGROW | wxALL, 3);

  button = new wxButton(this, IDB_SHAPES_DIALOG, "Vocal tract shapes");
  leftSizer->Add(button, 0, wxGROW | wxALL, 3);

  button = new wxButton(this, IDB_IMPORT_GEOMETRY, "Import geometry");
  leftSizer->Add(button, 0, wxGROW | wxALL, 3);

  leftSizer->AddSpacer(10);

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
  splitter->SetSashPosition(400);

  topLevelSizer->Add(splitter, 1, wxEXPAND);

// ****************************************************************
// Create the top panel (area function picture and cross-section
// picture).
// ****************************************************************

  wxBoxSizer* topSizer = new wxBoxSizer(wxHORIZONTAL);
  topPanel->SetSizer(topSizer);

  wxBoxSizer* topLeftSizer = new wxBoxSizer(wxVERTICAL);

  picAreaFunction = new AreaFunctionPicture(topPanel, picVocalTract, this);
  picAreaFunction->showSideBranches = false;
  topLeftSizer->Add(picAreaFunction, 1, wxEXPAND | wxALL, 2);

  topSizer->Add(topLeftSizer, 2, wxEXPAND | wxALL, 2);

  // ****************************************************************

  wxBoxSizer* topRightSizer = new wxBoxSizer(wxVERTICAL);

  picPropModes = new PropModesPicture(topPanel, picVocalTract, simu3d);
  topRightSizer->Add(picPropModes, 1, wxEXPAND | wxALL, 2);

  // propagation modes option sizer

  sizer = new wxBoxSizer(wxHORIZONTAL);

  button = new wxButton(topPanel, IDB_SHOW_LOWER_ORDER_MODE, "<", wxDefaultPosition, wxSize(25, 25));
  sizer->Add(button, 0, wxALL, 5);

  chkShowMesh = new wxCheckBox(topPanel, IDB_SHOW_MESH, "Mesh");
  sizer->Add(chkShowMesh, 0, wxALL, 2);

  chkShowMode = new wxCheckBox(topPanel, IDB_SHOW_MODE, "Modes");
  sizer->Add(chkShowMode, 0, wxALL, 2);

  //chkShowF = new wxCheckBox(topPanel, IDB_SHOW_F, "F");
  //sizer->Add(chkShowF, 0, wxALL, 2);

  button = new wxButton(topPanel, IDB_SHOW_HIGHER_ORDER_MODE, ">", wxDefaultPosition, wxSize(25, 25));
  sizer->Add(button, 0, wxALL, 5);

  topRightSizer->Add(sizer);
  topSizer->Add(topRightSizer, 1, wxEXPAND | wxALL, 2);

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
  if (event.GetInt() == REFRESH_PICTURES)
  {
    picAreaFunction->Refresh();
    picPropModes->Refresh();
  }
  else
  if (event.GetInt() == UPDATE_PICTURES)
  {
    picAreaFunction->Refresh();
    picAreaFunction->Update();
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
    picAreaFunction->Update();
    picPropModes->Update();
  }
  else
  if (event.GetInt() == UPDATE_VOCAL_TRACT)
  {
    updateWidgets();
    picAreaFunction->Update();
    picPropModes->Update();
  }
}

// ****************************************************************************
// ****************************************************************************

void Acoustic3dPage::OnRunTestJunction(wxCommandEvent& event)
{
  Acoustic3dSimulation* simu3d = Acoustic3dSimulation::getInstance();
  // load geometry configuration file
  wxFileName fileName;
  wxString name = wxFileSelector("Load geometry description file", fileName.GetPath(),
    fileName.GetFullName(), ".csv", "Geometry file (*.csv)|*.csv",
    wxFD_OPEN | wxFD_FILE_MUST_EXIST, this);

  if (!name.empty()){
  simu3d->coneConcatenationSimulation(name.ToStdString());
  updateWidgets();
  picAreaFunction->Update();
  picPropModes->Update();
  }
}

// ****************************************************************************
// ****************************************************************************

void Acoustic3dPage::OnRunTestRadImp(wxCommandEvent& event)
{
  Acoustic3dSimulation* simu3d = Acoustic3dSimulation::getInstance();
  string fileName("file");
  simu3d->runTest(SCALE_RAD_IMP, fileName);
  updateWidgets();
  picAreaFunction->Update();
  picPropModes->Update();
}

// ****************************************************************************
// ****************************************************************************

void Acoustic3dPage::OnRunTestMatrixE(wxCommandEvent& event)
{
  Acoustic3dSimulation* simu3d = Acoustic3dSimulation::getInstance();
  string fileName("file");
  simu3d->runTest(MATRIX_E, fileName);
  updateWidgets();
  picAreaFunction->Update();
  picPropModes->Update();
}

// ****************************************************************************
// ****************************************************************************

void Acoustic3dPage::OnRunTestDiscontinuity(wxCommandEvent& event)
{
  Acoustic3dSimulation* simu3d = Acoustic3dSimulation::getInstance();
  string fileName("file");
  simu3d->runTest(DISCONTINUITY, fileName);
  updateWidgets();
  picAreaFunction->Update();
  picPropModes->Update();
}

// ****************************************************************************
// ****************************************************************************

void Acoustic3dPage::OnRunTestElephant(wxCommandEvent& event)
{
  Acoustic3dSimulation* simu3d = Acoustic3dSimulation::getInstance();
  string fileName("file");
  simu3d->runTest(ELEPHANT_TRUNK, fileName);
  updateWidgets();
  picAreaFunction->Update();
  picPropModes->Update();
}

// ****************************************************************************
// ****************************************************************************

void Acoustic3dPage::OnRunStaticSimulation(wxCommandEvent& event)
{
  Data* data = Data::getInstance();
  Acoustic3dSimulation* simu3d = Acoustic3dSimulation::getInstance();
  VocalTract* tract = data->vocalTract;

  simu3d->staticSimulation(tract);

  // update pictures
  updateWidgets();
  picAreaFunction->Update();
  picPropModes->Update();

  OnPlayLongVowel();
}

// ****************************************************************************
// ****************************************************************************

void Acoustic3dPage::OnComputeModes(wxCommandEvent& event)
{
  ofstream log("log.txt", ofstream::app);
  log << "\nStart compute modes" << endl;

  Data* data = Data::getInstance();
  Acoustic3dSimulation* simu3d = Acoustic3dSimulation::getInstance();
  VocalTract* tract = data->vocalTract;
  
  simu3d->createCrossSections(tract, true);
  log << "Geometry succesfully imported" << endl;

  simu3d->exportGeoInCsv("test.csv");

  // exctract some contours
  ofstream prop;
  ostringstream os;
  for (int i(0); i < 2; i++)
  {
    os.str("");
    os.clear();
    os << "c" << i << ".txt";
    prop.open(os.str());
    for (auto it : simu3d->crossSection(i)->contour())
    {
      prop << it.x() << "  " << it.y() << endl;
    }
    prop.close();
  }

  simu3d->computeMeshAndModes();

  // update pictures
  updateWidgets();
  picAreaFunction->Update();
  picPropModes->Update();

  log.close();
}

// ****************************************************************************
// ****************************************************************************

void Acoustic3dPage::OnComputeAcousticField(wxCommandEvent& event)
{
  Data* data = Data::getInstance();
  Acoustic3dSimulation* simu3d = Acoustic3dSimulation::getInstance();
  VocalTract* tract = data->vocalTract;

  simu3d->computeAcousticField(tract);

  // update pictures
  updateWidgets();
  picAreaFunction->Update();
  picPropModes->Update();
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
  //chkShowF->SetValue(picPropModes->fSelected());
}

// ****************************************************************************

void Acoustic3dPage::OnShowMode(wxCommandEvent& event)
{
  picPropModes->showMode();
  chkShowMesh->SetValue(picPropModes->meshSelected());
  //chkShowF->SetValue(picPropModes->fSelected());
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
