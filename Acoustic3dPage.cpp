#include "Acoustic3dPage.h"
#include "ParamSimu3DDialog.h"
#include "VocalTractDialog.h"
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
//static const int IDB_RUN_TEST_MATRIX_E = 5997;
//static const int IDB_RUN_TEST_DISCONTINUITY = 5998;
//static const int IDB_RUN_TEST_ELEPHANT = 5999;
static const int IDB_PARAM_SIMU_DIALOG            = 6000;
static const int IDB_VOCAL_TRACT_DIALOG           = 6001;
static const int IDB_SHAPES_DIALOG                = 6002;
static const int IDB_IMPORT_GEOMETRY              = 6003;
static const int IDB_COMPUTE_MODES                = 6004;
static const int IDB_COMPUTE_TF                   = 6005;
static const int IDB_COMPUTE_ACOUSTIC_FIELD       = 6006;
static const int IDB_EXPORT_GLOTTAL_SOURCE_TF     = 6007;
static const int IDB_EXPORT_NOISE_SOURCE_TF       = 6008;
static const int IDB_LF_PULSE                     = 6009;
static const int IDB_PLAY_LONG_VOWEL              = 6010;
static const int IDB_PLAY_NOISE_SOURCE            = 6011;


//static const int IDB_EXPORT_FIELD = 6010;

// Modes picture controls
static const int IDB_SHOW_LOWER_ORDER_MODE        = 6020;
static const int IDB_SHOW_CONTOUR                 = 6021;
static const int IDB_SHOW_MESH                    = 6022;
static const int IDB_SHOW_MODE                    = 6023;
static const int IDB_SHOW_TRANSVERS_FIELD         = 6024;
//static const int IDB_SHOW_F = 6025;
static const int IDB_SHOW_HIGHER_ORDER_MODE       = 6026;

// segments picture controls
static const int IDB_SHOW_PREVIOUS_SEGMENT        = 6027;
static const int IDB_SHOW_NEXT_SEGMENT            = 6028;
static const int IDB_SHOW_SEGMENTS                = 6029;
static const int IDB_SHOW_FIELD                   = 6030;

// Spectrum panel controls
static const int IDB_UPPER_SPECTRUM_LIMIT_PLUS		= 7000;
static const int IDB_UPPER_SPECTRUM_LIMIT_MINUS		= 7001;
static const int IDB_LOWER_SPECTRUM_LIMIT_PLUS		= 7002;
static const int IDB_LOWER_SPECTRUM_LIMIT_MINUS		= 7003;
static const int IDB_FREQUENCY_RANGE_MINUS			  = 7006;
static const int IDB_FREQUENCY_RANGE_PLUS 		  	= 7007;

static const int IDB_SHOW_NOISE_SOURCE_SPEC       = 7008;
static const int IDB_PREVIOUS_TF                  = 7009;
static const int IDB_NEXT_TF                      = 7010;

// ****************************************************************************
// The event table.
// ****************************************************************************

BEGIN_EVENT_TABLE(Acoustic3dPage, wxPanel)

  // Custom event handler for update requests by child widgets.
  EVT_COMMAND(wxID_ANY, updateRequestEvent, Acoustic3dPage::OnUpdateRequest)

  // Left side controls

  //EVT_BUTTON(IDB_RUN_TEST_JUNCTION, Acoustic3dPage::OnRunTestJunction)
  //EVT_BUTTON(IDB_RUN_RAD_IMP, Acoustic3dPage::OnRunTestRadImp)
  //EVT_BUTTON(IDB_RUN_TEST_MATRIX_E, Acoustic3dPage::OnRunTestMatrixE)
  //EVT_BUTTON(IDB_RUN_TEST_DISCONTINUITY, Acoustic3dPage::OnRunTestDiscontinuity)
  //EVT_BUTTON(IDB_RUN_TEST_ELEPHANT, Acoustic3dPage::OnRunTestElephant)
  EVT_BUTTON(IDB_PARAM_SIMU_DIALOG, Acoustic3dPage::OnParamSimuDialog)
  EVT_BUTTON(IDB_VOCAL_TRACT_DIALOG, Acoustic3dPage::OnVocalTractDialog)
  EVT_BUTTON(IDB_SHAPES_DIALOG, Acoustic3dPage::OnShapesDialog)
  EVT_BUTTON(IDB_IMPORT_GEOMETRY, Acoustic3dPage::OnImportGeometry)
  EVT_BUTTON(IDB_COMPUTE_MODES, Acoustic3dPage::OnComputeModes)
  EVT_BUTTON(IDB_COMPUTE_TF, Acoustic3dPage::OnComputeTf)
  EVT_BUTTON(IDB_COMPUTE_ACOUSTIC_FIELD, Acoustic3dPage::OnComputeAcousticField)
  EVT_BUTTON(IDB_EXPORT_GLOTTAL_SOURCE_TF, Acoustic3dPage::OnExportGlottalSourceTf)
  EVT_BUTTON(IDB_EXPORT_NOISE_SOURCE_TF, Acoustic3dPage::OnExportNoiseSourceTf)
  EVT_BUTTON(IDB_LF_PULSE, Acoustic3dPage::OnLfPulse)
  EVT_BUTTON(IDB_PLAY_LONG_VOWEL, Acoustic3dPage::OnPlayLongVowel)
  EVT_BUTTON(IDB_PLAY_NOISE_SOURCE, Acoustic3dPage::OnPlayNoiseSource)
  //EVT_BUTTON(IDB_EXPORT_FIELD, Acoustic3dPage::OnExportField)

  // Modes picture controls
  EVT_BUTTON(IDB_SHOW_LOWER_ORDER_MODE, Acoustic3dPage::OnShowPrevious)
  EVT_CHECKBOX(IDB_SHOW_CONTOUR, Acoustic3dPage::OnShowContour)
  EVT_CHECKBOX(IDB_SHOW_MESH, Acoustic3dPage::OnShowMesh)
  EVT_CHECKBOX(IDB_SHOW_MODE, Acoustic3dPage::OnShowMode)
  EVT_CHECKBOX(IDB_SHOW_TRANSVERS_FIELD, Acoustic3dPage::OnShowTransField)
  //EVT_CHECKBOX(IDB_SHOW_F, Acoustic3dPage::OnShowF)
  EVT_BUTTON(IDB_SHOW_HIGHER_ORDER_MODE, Acoustic3dPage::OnShowNext)

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
  EVT_CHECKBOX(IDB_SHOW_NOISE_SOURCE_SPEC, Acoustic3dPage::OnShowNoiseSourceSpec)
  EVT_BUTTON(IDB_PREVIOUS_TF, Acoustic3dPage::OnPreviousTf)
  EVT_BUTTON(IDB_NEXT_TF, Acoustic3dPage::OnNextTf)
END_EVENT_TABLE()

// ****************************************************************************
/// Constructor. 
// ****************************************************************************

Acoustic3dPage::Acoustic3dPage(wxWindow* parent, VocalTractPicture *picVocalTract) :
  wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxCLIP_CHILDREN),
  m_idxTfPoint(0)
{
  initVars();
  initWidgets(picVocalTract);
  importGeometry();
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
  chkShowContour->SetValue(false);
  chkShowMesh->SetValue(false);
  chkShowMode->SetValue(false);
  chkShowTransField->SetValue(false);
  enum objectToDisplay object(picPropModes->getObjectDisplayed());
  switch (object)
  {
  case MESH:
    chkShowMesh->SetValue(true);
    break;
  case TRANSVERSE_MODE:
    chkShowMode->SetValue(true);
    break;
  case ACOUSTIC_FIELD:
    chkShowTransField->SetValue(true);
    break;
  case CONTOUR:
    chkShowContour->SetValue(true);
    break;
  default:
    chkShowContour->SetValue(true);
    break;
  }

  // options for segment picture
  segPic->setShowSegments(chkShowSegments->GetValue());
  segPic->setShowField(chkShowField->GetValue());

  // option for spectrum 3D picture
  chkShowNoiseSourceSpec->SetValue(picSpectrum->showNoise());

  picPropModes->Refresh();
  picSpectrum->Refresh();
  segPic->Refresh();

  m_tfPoint = simu3d->oldSimuParams().tfPoint[m_idxTfPoint];
  txtTfPoint->SetLabelText(generateTfPointCoordString());
}

// ****************************************************************************
/// Init some variables.
// ****************************************************************************

void Acoustic3dPage::initVars()
{
  data = Data::getInstance();
  simu3d = Acoustic3dSimulation::getInstance();
  m_tfPoint = simu3d->simuParams().tfPoint[m_idxTfPoint];
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

  button = new wxButton(this, IDB_VOCAL_TRACT_DIALOG, "Show vocal tract");
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

  //button = new wxButton(this, IDB_RUN_TEST_MATRIX_E, "Run test matrix E");
  //leftSizer->Add(button, 0, wxGROW | wxALL, 3);

  //button = new wxButton(this, IDB_RUN_TEST_DISCONTINUITY, "Run test discontinuity");
  //leftSizer->Add(button, 0, wxGROW | wxALL, 3);

  //button = new wxButton(this, IDB_RUN_TEST_ELEPHANT, "Run test elphant");
  //leftSizer->Add(button, 0, wxGROW | wxALL, 3);

  button = new wxButton(this, IDB_COMPUTE_MODES, "Compute modes");
  leftSizer->Add(button, 0, wxGROW | wxALL, 3);

  button = new wxButton(this, IDB_COMPUTE_TF, "Compute transfer function");
  leftSizer->Add(button, 0, wxGROW | wxALL, 3);

  button = new wxButton(this, IDB_COMPUTE_ACOUSTIC_FIELD, "Compute acoustic field");
  leftSizer->Add(button, 0, wxGROW | wxALL, 3);

  leftSizer->AddSpacer(20);
  
  button = new wxButton(this, IDB_EXPORT_GLOTTAL_SOURCE_TF, "Export glottal transfer function");
  leftSizer->Add(button, 0, wxGROW | wxALL, 3);

  button = new wxButton(this, IDB_EXPORT_NOISE_SOURCE_TF, "Export noise transfer function");
  leftSizer->Add(button, 0, wxGROW | wxALL, 3);

  //button = new wxButton(this, IDB_EXPORT_FIELD, "Export acoustic field");
  //leftSizer->Add(button, 0, wxGROW | wxALL, 3);

  leftSizer->AddSpacer(20);
  
  button = new wxButton(this, IDB_LF_PULSE, "LF glottal flow pulse");
  leftSizer->Add(button, 0, wxGROW | wxALL, 3);
  
  button = new wxButton(this, IDB_PLAY_LONG_VOWEL, "Play vowel");
  leftSizer->Add(button, 0, wxGROW | wxALL, 3);

  button = new wxButton(this, IDB_PLAY_NOISE_SOURCE, "Play noise source");
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

  chkShowContour = new wxCheckBox(topPanel, IDB_SHOW_CONTOUR, "Contour");
  sizer->Add(chkShowContour, 0, wxALIGN_BOTTOM | wxALL, 2);

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

  chkShowNoiseSourceSpec = new wxCheckBox(bottomPanel, IDB_SHOW_NOISE_SOURCE_SPEC, "Noise source");
  sizer->Add(chkShowNoiseSourceSpec, 0, wxALL, 2);

  // text to display the transfer function point coordinates
  wxStaticText* label = new wxStaticText(bottomPanel, wxID_ANY, "Transfer function point:");
  sizer->Add(label, 0, wxALL | wxALIGN_LEFT, 3);

  txtTfPoint = new wxStaticText(bottomPanel, wxID_ANY, generateTfPointCoordString());
  sizer->Add(txtTfPoint, 0, wxALL | wxALIGN_LEFT, 3);

  // buttons to brows the transfer functions of the different points
  wxBoxSizer* subSizer = new wxBoxSizer(wxHORIZONTAL);

  button = new wxButton(bottomPanel, IDB_PREVIOUS_TF, "<", wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
  button->SetMinSize(wxSize(button->GetSize().GetHeight(), button->GetSize().GetHeight()));
  subSizer->Add(button, 0, wxALL, 5);

  button = new wxButton(bottomPanel, IDB_NEXT_TF, ">", wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
  button->SetMinSize(wxSize(button->GetSize().GetHeight(), button->GetSize().GetHeight()));
  subSizer->Add(button, 0, wxALL, 5);

  sizer->Add(subSizer, 0, wxGROW | wxALL, 2);

  // buttons to change the frequency axis
  sizer->AddStretchSpacer(1);

  subSizer = new wxBoxSizer(wxHORIZONTAL);

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

wxString Acoustic3dPage::generateTfPointCoordString()
{
  return(wxString::Format("X: %2.1f\nY: %2.1f\nZ: %2.1f",
    m_tfPoint.x(), m_tfPoint.y(), m_tfPoint.z()));
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
    segPic->Refresh();
    segPic->Update();
    picPropModes->Refresh();
    picPropModes->Update();
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
    //simu3d->setGeometryImported(false);
    simu3d->requestModesAndJunctionComputation();
    simu3d->cleanAcousticField();
    importGeometry();
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

//void Acoustic3dPage::OnRunTestMatrixE(wxCommandEvent& event)
//{
//  Acoustic3dSimulation* simu3d = Acoustic3dSimulation::getInstance();
//  string fileName("file");
//  simu3d->runTest(MATRIX_E, fileName);
//  updateWidgets();
//  //picAreaFunction->Update();
//  picPropModes->Update();
//}

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

//void Acoustic3dPage::OnRunTestElephant(wxCommandEvent& event)
//{
//  Acoustic3dSimulation* simu3d = Acoustic3dSimulation::getInstance();
//  string fileName("file");
//  simu3d->runTest(ELEPHANT_TRUNK, fileName);
//  updateWidgets();
//  segPic->Update();
//  picPropModes->Update();
//}

// ****************************************************************************
// ****************************************************************************

void Acoustic3dPage::computeModesJunctionAndRadMats(bool precomputeRadMat,
  wxGenericProgressDialog* progressDialog, bool &abort)
{
  const struct simulationParameters &simuParams(simu3d->simuParams());
  int numSeg(simu3d->numberOfSegments());
  int lastSeg(numSeg - 1);
  int nbRadFreqs(16);

  ofstream log("log.txt", ofstream::app);
  std::chrono::duration<double> time;

  if (simuParams.needToComputeModesAndJunctions)
  {
    // set mode number to 0 to make sure that it is defined by the maximal cutoff 
    // frequency when modes are computed
    for (int i(0); i < numSeg; i++)
    {
      simu3d->crossSection(i)->setModesNumber(0);
    }

    //*********************************************************
    // compute modes
    //*********************************************************

    auto start = std::chrono::system_clock::now();
    for (int i(0); i < numSeg; i++)
    {
      simu3d->computeMeshAndModes(i);

      // stop if [Cancel] is pressed
      if (progressDialog->Update(i) == false)
      {
        abort = true;
        break;
      }
    }
    auto end = std::chrono::system_clock::now();
    time = end - start;
    log << "Time mesh and modes: " << time.count() << endl;

    //*********************************************************
    // compute junction matrices
    //*********************************************************

    if (!abort)
    {
      progressDialog->Update(0,
        "Wait until the junction matrices computation finished or press [Cancel]");
      start = std::chrono::system_clock::now();
      for (int i(0); i < numSeg; i++)
      {
        simu3d->computeJunctionMatrices(i);
        log << "Junction segment " << i << " computed" << endl;
        // stop if [Cancel] is pressed
        if (progressDialog->Update(i) == false)
        {
          abort = true;
          break;
        }
      }
      end = std::chrono::system_clock::now();
      time = end - start;
      log << "Time junction matrices: " << time.count() << endl;
    }

    simu3d->setNeedToComputeModesAndJunctions(false);

    //*********************************************************
    // precompute radiation impedance
    //*********************************************************

    if (!abort && precomputeRadMat && simu3d->mouthBoundaryCond() == RADIATION)
    {
      progressDialog->Update(0, "Precompute radiation impedance");
      progressDialog->SetRange(nbRadFreqs);
      start = std::chrono::system_clock::now();
      simu3d->initCoefInterpRadiationMatrices(nbRadFreqs, lastSeg);
      for (int i(0); i < nbRadFreqs; i++)
      {
        simu3d->addRadMatToInterpolate(nbRadFreqs, lastSeg, i);
        if (progressDialog->Update(i) == false)
        {
          abort = true;
          break;
        }
      }
      simu3d->computeInterpCoefRadMat(nbRadFreqs, lastSeg);

      end = std::chrono::system_clock::now();
      time = end - start;
      log << "Time radiation impedance: " << time.count() << endl;
    }
  }

  if (!simu3d->radImpedPrecomputed() && (simu3d->mouthBoundaryCond() == RADIATION)
    && !abort && precomputeRadMat)
  {
    progressDialog->Update(0, "Precompute radiation impedance");
    progressDialog->SetRange(nbRadFreqs);
    auto start = std::chrono::system_clock::now();
    simu3d->initCoefInterpRadiationMatrices(nbRadFreqs, lastSeg);
    for (int i(0); i < nbRadFreqs; i++)
    {
      simu3d->addRadMatToInterpolate(nbRadFreqs, lastSeg, i);
      if (progressDialog->Update(i) == false)
      {
        abort = true;
        break;
      }
    }
    simu3d->computeInterpCoefRadMat(nbRadFreqs, lastSeg);

    auto end = std::chrono::system_clock::now();
    time = end - start;
    log << "Time radiation impedance: " << time.count() << endl;
  }
  log.close();
}

// ****************************************************************************
// ****************************************************************************

void Acoustic3dPage::OnComputeTf(wxCommandEvent& event)
{
  Data* data = Data::getInstance();
  Acoustic3dSimulation* simu3d = Acoustic3dSimulation::getInstance();
  VocalTract* tract = data->vocalTract;

  bool abort(false);
  double freq;
  //int nbRadFreqs(16);
  int numSeg(simu3d->numberOfSegments());
  int lastSeg(numSeg - 1);
  struct simulationParameters simuParams(simu3d->simuParams());

  // Create the progress dialog
  progressDialog = new wxGenericProgressDialog("Transfer functions progress",
    "Wait until the modes computation finished or press [Cancel]",
    numSeg, NULL,
    wxPD_CAN_ABORT | wxPD_AUTO_HIDE | wxPD_ELAPSED_TIME);

  // log file
  simu3d->generateLogFileHeader(true);
  ofstream log("log.txt", ofstream::app);

  // for time tracking
  auto startTot = std::chrono::system_clock::now();
  auto start = std::chrono::system_clock::now();
  auto end = std::chrono::system_clock::now();
  std::chrono::duration<double> timeModes(0.), timeJunctions(0.), timePropa(0.),
    timeRadImped(0.), timeComputeField(0.), timeExp(0.), time;

  simu3d->precomputationsForTf();

  computeModesJunctionAndRadMats(true, progressDialog, abort);

  //*********************************************************
  // Compute the transfer fucntion for each frequency
  //*********************************************************

  int numFreqComputed(simu3d->numFreqComputed());
  double freqSteps(simu3d->freqSteps());
  bool computeNoiseSrcTf(simu3d->idxSecNoiseSource() < numSeg - 1);
  bool needToExtractMatrixF(true);
  Matrix F;
  if (!abort)
  {
    progressDialog->Update(0,
      "Wait until the transfer functions computation finished or press [Cancel]");
    progressDialog->SetRange(numFreqComputed);

    for (int i(0); i < numFreqComputed; i++)
    {
      freq = max(0.1, (double)i * freqSteps);
      log << "frequency " << i + 1 << "/" << numFreqComputed << " f = " << freq
        << " Hz" << endl;

      simu3d->solveWaveProblem(tract, freq, timePropa, &timeExp);

      //*****************************************************************************
      //  Compute acoustic pressure 
      //*****************************************************************************

      start = std::chrono::system_clock::now();

      simu3d->computeGlottalTf(i, freq);

      end = std::chrono::system_clock::now();
      timeComputeField += end - start;

      //*****************************************************************************
      //  Compute transfer function of the noise source
      //*****************************************************************************

      if (computeNoiseSrcTf)
      {
        simu3d->solveWaveProblemNoiseSrc(needToExtractMatrixF, F, freq, &time);
        simu3d->computeNoiseSrcTf(i);
      }

      if (progressDialog->Update(i) == false)
      {
        abort = true;
        break;
      }
    }

    // generate spectra values for negative frequencies
    simu3d->generateSpectraForSynthesis(m_idxTfPoint);
  }

  log << "\nTime propagation: " << timePropa.count() << endl;
  end = std::chrono::system_clock::now();
  time = end - startTot;
  log << "\nTransfer function time (sec): " << time.count() << endl;
  log << "Time acoustic pressure computation: " << timeComputeField.count() << endl;
  log << "Time matrix exponential: " << timeExp.count() << endl;

  // print total time in HMS
  int hours(floor(time.count() / 3600.));
  int minutes(floor((time.count() - hours * 3600.) / 60.));
  double seconds(time.count() - (double)hours * 3600. -
    (double)minutes * 60.);
  log << "\nTransfer function time "
    << hours << " h " << minutes << " m " << seconds << " s" << endl;

  log.close();

  // destroy progress dialog
  progressDialog->Destroy();
  progressDialog = NULL;

  updateWidgets();
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

  int numSeg(simu3d->numberOfSegments());

  // Create the progress dialog
  progressDialog = new wxGenericProgressDialog("Modes computation progress",
    "Wait until the modes computation finished or press [Cancel]",
    numSeg, NULL,
    wxPD_CAN_ABORT | wxPD_AUTO_HIDE | wxPD_ELAPSED_TIME);

  for (int i(0); i < numSeg; i++)
  {
    simu3d->computeMeshAndModes(i);

    // stop if [Cancel] is pressed
    if (progressDialog->Update(i) == false)
    {
      break;
    }
  }
  log << "Mode computation finished" << endl;

  // destroy progress dialog
  progressDialog->Destroy();
  progressDialog = NULL;

  // update pictures
  setPicModeObjectTodisplay(TRANSVERSE_MODE);
  updateWidgets();

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

  int numSeg(simu3d->numberOfSegments());
  bool abort(false);
  struct simulationParameters simuParams(simu3d->simuParams());
  double freq(simuParams.freqField);
  std::chrono::duration<double> timePropa(0.), timeExp(0.);
  int nPtx, nPty;
  int cnt(0);

  // Create the progress dialog
  progressDialog = new wxGenericProgressDialog("Acoustic field progress",
    "Wait until the modes computation finished or press [Cancel]",
    numSeg, NULL,
    wxPD_CAN_ABORT | wxPD_AUTO_HIDE | wxPD_ELAPSED_TIME);

  // log file
  simu3d->generateLogFileHeader(true);
  ofstream log("log.txt", ofstream::app);

  computeModesJunctionAndRadMats(false, progressDialog, abort);

  simu3d->solveWaveProblem(tract, freq, timePropa, &timeExp);

  simu3d->prepareAcousticFieldComputation();
  nPtx = simu3d->numPtXField();
  nPty = simu3d->numPtYField();

  progressDialog->Update(0,
    "Wait until the acoustic field computation finished or press [Cancel]");
  progressDialog->SetRange(nPtx);
  log << "Num points on x: " << nPtx << " Num points on y: " << nPty << endl;

  for (int i(0); i < nPtx; i++)
  {
    simu3d->acousticFieldInLine(i);
    cnt += nPty;

    log << 100 * cnt / nPtx / nPty << " % of field points computed" << endl;

    // stop if [Cancel] is pressed
    if (progressDialog->Update(i) == false)
    {
      break;
    }
  }

  //simu3d->computeAcousticField(tract);

  // destroy progress dialog
  progressDialog->Destroy();
  progressDialog = NULL;

  // update pictures
  chkShowField->SetValue(true);
  setPicModeObjectTodisplay(ACOUSTIC_FIELD);
  updateWidgets();
}

// ****************************************************************************
// ****************************************************************************

void Acoustic3dPage::OnExportGlottalSourceTf(wxCommandEvent& event)
{
  wxFileName fileName;
  wxString name = wxFileSelector("Save transfer functions", fileName.GetPath(),
    fileName.GetFullName(), ".txt", "(*.txt)|*.txt",
    wxFD_SAVE | wxFD_OVERWRITE_PROMPT, this);

  simu3d->exportTransferFucntions(name.ToStdString(), GLOTTAL);
}

// ****************************************************************************
// ****************************************************************************

void Acoustic3dPage::OnExportNoiseSourceTf(wxCommandEvent& event)
{
  wxFileName fileName;
  wxString name = wxFileSelector("Save transfer functions", fileName.GetPath(),
    fileName.GetFullName(), ".txt", "(*.txt)|*.txt",
    wxFD_SAVE | wxFD_OVERWRITE_PROMPT, this);

  simu3d->exportTransferFucntions(name.ToStdString(), NOISE);
}

// ****************************************************************************
// ****************************************************************************

//void Acoustic3dPage::OnExportField(wxCommandEvent& event)
//{
//  wxFileName fileName;
//  wxString name = wxFileSelector("Save acoustic field", fileName.GetPath(),
//    fileName.GetFullName(), ".txt", "(*.txt)|*.txt",
//    wxFD_SAVE | wxFD_OVERWRITE_PROMPT, this);
//
//  simu3d->exportAcousticField(name.ToStdString());
//}

// ****************************************************************************
// ****************************************************************************

void Acoustic3dPage::OnParamSimuDialog(wxCommandEvent& event)
{
  ParamSimu3DDialog *dialog = ParamSimu3DDialog::getInstance(NULL);
  dialog->SetParent(this);
  dialog->Show(true);
  dialog->Raise();
}

// ****************************************************************************
// ****************************************************************************

void Acoustic3dPage::OnVocalTractDialog(wxCommandEvent& event)
{
  VocalTractDialog* dialog = VocalTractDialog::getInstance(this);
  dialog->Show(true);
  simu3d->setGeometryImported(false);
  simu3d->setContourInterpolationMethod(AREA);
}

// ****************************************************************************
// ****************************************************************************

void Acoustic3dPage::OnShapesDialog(wxCommandEvent& event)
{
  VocalTractShapesDialog* dialog = VocalTractShapesDialog::getInstance();
  dialog->Show(true);
  simu3d->setGeometryImported(false);
  simu3d->setContourInterpolationMethod(AREA);

  //updateWidgets();
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

void Acoustic3dPage::OnLfPulse(wxCommandEvent& event)
{
  LfPulseDialog *dialog = LfPulseDialog::getInstance();
  dialog->Show(true);
}

// ****************************************************************************
// ****************************************************************************

void Acoustic3dPage::OnPlayLongVowel(wxCommandEvent& event)
{
  OnPlayLongVowel();
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

void Acoustic3dPage::OnShowContour(wxCommandEvent& event)
{
  setPicModeObjectTodisplay(CONTOUR);
}

// ****************************************************************************

void Acoustic3dPage::OnShowMesh(wxCommandEvent& event)
{
  setPicModeObjectTodisplay(MESH);
}

// ****************************************************************************

void Acoustic3dPage::OnShowMode(wxCommandEvent& event)
{
  setPicModeObjectTodisplay(TRANSVERSE_MODE);
}

// ****************************************************************************

void Acoustic3dPage::OnShowTransField(wxCommandEvent& event)
{
  setPicModeObjectTodisplay(ACOUSTIC_FIELD);
}

// ****************************************************************************

//void Acoustic3dPage::OnShowF(wxCommandEvent& event)
//{
//  picPropModes->showF();
//  chkShowMesh->SetValue(picPropModes->meshSelected());
//  chkShowMode->SetValue(picPropModes->modeSelected());
//}

// ****************************************************************************

void Acoustic3dPage::OnShowPrevious(wxCommandEvent& event)
{
  enum objectToDisplay picType(picPropModes->getObjectDisplayed());

  switch (picType)
  {
  case TRANSVERSE_MODE:
    picPropModes->setModeIdx(picPropModes->modeIdx() - 1);
    break;
  case CONTOUR:
    picPropModes->prevContourPosition();
    break;
  }
}

// ****************************************************************************

void Acoustic3dPage::OnShowNext(wxCommandEvent& event)
{
  enum objectToDisplay picType(picPropModes->getObjectDisplayed());

  switch (picType)
  {
  case TRANSVERSE_MODE:
    picPropModes->setModeIdx(picPropModes->modeIdx() + 1);
    break;
  case CONTOUR:
    picPropModes->nextContourPosition();
    break;
  }
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

void Acoustic3dPage::OnShowNoiseSourceSpec(wxCommandEvent& event)
{
  picSpectrum->setShowNoiseTf(chkShowNoiseSourceSpec->GetValue());
  picSpectrum->Refresh();
}

// ****************************************************************************

void Acoustic3dPage::OnPreviousTf(wxCommandEvent& event)
{
  m_idxTfPoint = max(0, m_idxTfPoint - 1);
  simu3d->generateSpectraForSynthesis(m_idxTfPoint);

  picSpectrum->setIdxTfPoint(m_idxTfPoint);
  picSpectrum->Refresh();
  segPic->setIdxTfPoint(m_idxTfPoint);
  segPic->Refresh();
  updateWidgets();
}

// ****************************************************************************

void Acoustic3dPage::OnNextTf(wxCommandEvent& event)

{
  struct simulationParameters simuParams(simu3d->oldSimuParams());
  m_idxTfPoint = min((int)(simuParams.tfPoint.size()) - 1, m_idxTfPoint + 1);
  simu3d->generateSpectraForSynthesis(m_idxTfPoint);

  picSpectrum->setIdxTfPoint(m_idxTfPoint);
  picSpectrum->Refresh();
  segPic->setIdxTfPoint(m_idxTfPoint);
  segPic->Refresh();
  updateWidgets();
}

// ****************************************************************************

void Acoustic3dPage::importGeometry()
{

  VocalTract* tract = data->vocalTract;

  simu3d->importGeometry(tract);

  segPic->resetActiveSegment();

  //// clean the log file
  //ofstream log;
  //log.open("log.txt", ofstream::out | ofstream::trunc);
  //log.close();
}

// ****************************************************************************

void Acoustic3dPage::setPicModeObjectTodisplay(enum objectToDisplay object)
{
  chkShowContour->SetValue(false);
  chkShowMesh->SetValue(false);
  chkShowMode->SetValue(false);
  chkShowTransField->SetValue(false);

  switch (object)
  {
  case MESH:
    chkShowMesh->SetValue(true);
    picPropModes->setObjectToDisplay(MESH);
    break;
  case TRANSVERSE_MODE:
    chkShowMode->SetValue(true);
    picPropModes->setObjectToDisplay(TRANSVERSE_MODE);
    break;
  case ACOUSTIC_FIELD:
    chkShowTransField->SetValue(true);
    picPropModes->setObjectToDisplay(ACOUSTIC_FIELD);
    break;
  case CONTOUR:
    chkShowContour->SetValue(true);
    picPropModes->setObjectToDisplay(CONTOUR);
    break;
  default:
    chkShowContour->SetValue(true);
    picPropModes->setObjectToDisplay(CONTOUR);
    break;
  }
}
