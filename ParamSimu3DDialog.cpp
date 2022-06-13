#include "ParamSimu3DDialog.h"
#include "Data.h"
#include <fstream>
#include "Backend/Constants.h"
#include <wx/filename.h>
#include <wx/statline.h>

// ****************************************************************************
// IDs.
// ****************************************************************************

static const int IDE_TEMPERATURE        = 3999;
static const int IDE_SND_SPEED          = 4000;
static const int IDE_MESH_DENSITY       = 4001;
static const int IDE_MAX_CUT_ON         = 4002;
static const int IDE_MAX_SIM_FREQ       = 4003;
static const int IDE_NUM_INTEGRATION    = 4004;
static const int IDE_SEC_NOISE_SOURCE   = 4005;
static const int IDE_PERCENT_LOSSES		  = 4006;

// acoustic field computation options
static const int IDE_FREQ_COMPUTE_FIELD = 4009;
static const int IDE_RESOLUTION_FIELD = 4010;
static const int IDE_BBOX_MIN_X       = 4011;
static const int IDE_BBOX_MIN_Y       = 4012;
static const int IDE_BBOX_MAX_X       = 4013;
static const int IDE_BBOX_MAX_Y       = 4014;

// transfer function point
static const int IDE_TF_POINT_X       = 4015;
static const int IDE_TF_POINT_Y       = 4016;
static const int IDE_TF_POINT_Z       = 4017;
static const int IDB_LOAD_TF_POINTS   = 4018;

static const int IDE_WALL_ADMIT       = 4019;

static const int IDB_CHK_FDEP_LOSSES	= 5001;
static const int IDB_CHK_WALL_LOSSES    = 5002;
static const int IDB_CHK_WALL_ADMITTANCE = 5003;

static const int IDB_CHK_STRAIGHT       = 5004;
static const int IDB_CHK_MAGNUS			= 5005;
static const int IDB_CHK_CURV			= 5006;
static const int IDB_CHK_VAR_AREA		= 5007;

static const int IDB_CHK_MULTI_TF_PTS = 5008;

static const int IDB_COMPUTE_RAD_FIELD = 5009;

static const int IDL_SCALING_FAC_METHOD = 6000;
static const int IDL_MOUTH_BCOND = 6001;
static const int IDL_FREQ_RES = 6002;

static const int IDB_SET_DEFAULT_PARAMS_FAST = 6002;
static const int IDB_SET_DEFAULT_PARAMS_ACCURATE = 6003;
static const int IDB_CLOSE = 6004;

// ****************************************************************************
// The event table.
// ****************************************************************************

BEGIN_EVENT_TABLE(ParamSimu3DDialog, wxDialog)
EVT_TEXT_ENTER(IDE_TEMPERATURE, ParamSimu3DDialog::OnTemperatureEnter)
EVT_TEXT_ENTER(IDE_SND_SPEED, ParamSimu3DDialog::OnSndSpeedEnter)
EVT_TEXT_ENTER(IDE_MESH_DENSITY, ParamSimu3DDialog::OnMeshDensityEnter)
EVT_TEXT_ENTER(IDE_MAX_CUT_ON, ParamSimu3DDialog::OnMaxCutOnEnter)
EVT_TEXT_ENTER(IDE_MAX_SIM_FREQ, ParamSimu3DDialog::OnMaxSimFreq)
EVT_TEXT_ENTER(IDE_NUM_INTEGRATION, ParamSimu3DDialog::OnNumIntegrationEnter)
EVT_TEXT_ENTER(IDE_SEC_NOISE_SOURCE, ParamSimu3DDialog::OnSecNoiseSourceEnter)
EVT_TEXT_ENTER(IDE_PERCENT_LOSSES, ParamSimu3DDialog::OnPercentLosses)
EVT_TEXT_ENTER(IDE_FREQ_COMPUTE_FIELD, ParamSimu3DDialog::OnFreqComputeField)
EVT_TEXT_ENTER(IDE_RESOLUTION_FIELD, ParamSimu3DDialog::OnResolutionField)
EVT_TEXT_ENTER(IDE_BBOX_MIN_X, ParamSimu3DDialog::OnBboxMinX)
EVT_TEXT_ENTER(IDE_BBOX_MIN_Y, ParamSimu3DDialog::OnBboxMinY)
EVT_TEXT_ENTER(IDE_BBOX_MAX_X, ParamSimu3DDialog::OnBboxMaxX)
EVT_TEXT_ENTER(IDE_BBOX_MAX_Y, ParamSimu3DDialog::OnBboxMaxY)

EVT_TEXT_ENTER(IDE_TF_POINT_X, ParamSimu3DDialog::OnTfPointX)
EVT_TEXT_ENTER(IDE_TF_POINT_Y, ParamSimu3DDialog::OnTfPointY)
EVT_TEXT_ENTER(IDE_TF_POINT_Z, ParamSimu3DDialog::OnTfPointZ)
EVT_BUTTON(IDB_LOAD_TF_POINTS, ParamSimu3DDialog::OnLoadTfPts)

EVT_TEXT_ENTER(IDE_WALL_ADMIT, ParamSimu3DDialog::OnWallAdmitEnter)

EVT_CHECKBOX(IDB_CHK_FDEP_LOSSES, ParamSimu3DDialog::OnChkFdepLosses)
EVT_CHECKBOX(IDB_CHK_WALL_LOSSES, ParamSimu3DDialog::OnChkWallLosses)
EVT_CHECKBOX(IDB_CHK_WALL_ADMITTANCE, ParamSimu3DDialog::OnChkWallAdmittance)
EVT_CHECKBOX(IDB_CHK_STRAIGHT, ParamSimu3DDialog::OnChkStraight)
EVT_CHECKBOX(IDB_CHK_MAGNUS, ParamSimu3DDialog::OnChkMagnus)
EVT_CHECKBOX(IDB_CHK_CURV, ParamSimu3DDialog::OnChkCurv)
EVT_CHECKBOX(IDB_CHK_VAR_AREA, ParamSimu3DDialog::OnChkVarArea)
EVT_CHECKBOX(IDB_CHK_MULTI_TF_PTS, ParamSimu3DDialog::OnChkMultiTFPts)
EVT_CHECKBOX(IDB_COMPUTE_RAD_FIELD, ParamSimu3DDialog::OnChkComputeRad)

EVT_COMBOBOX(IDL_SCALING_FAC_METHOD, ParamSimu3DDialog::OnScalingFactMethod)
EVT_COMBOBOX(IDL_MOUTH_BCOND, ParamSimu3DDialog::OnMouthBcond)
EVT_COMBOBOX(IDL_FREQ_RES, ParamSimu3DDialog::OnFreqRes)

EVT_BUTTON(IDB_SET_DEFAULT_PARAMS_FAST, ParamSimu3DDialog::OnSetDefaultParamsFast)
EVT_BUTTON(IDB_SET_DEFAULT_PARAMS_ACCURATE, ParamSimu3DDialog::OnSetDefaultParamsAccurate)
EVT_BUTTON(IDB_CLOSE, ParamSimu3DDialog::OnClose)

END_EVENT_TABLE()

// The single instance of this class.
ParamSimu3DDialog *ParamSimu3DDialog::instance = NULL;

// ****************************************************************************
/// Returns the single instance of this dialog.
// ****************************************************************************

ParamSimu3DDialog* ParamSimu3DDialog::getInstance(wxWindow *parent)
{
    if (instance == NULL)
    {
        instance = new ParamSimu3DDialog(parent);
    }

    instance->updateParams();
    instance->updateWidgets();

    return instance;
}

// ****************************************************************************
// ****************************************************************************

void ParamSimu3DDialog::updateWidgets()
{
  //ofstream log("log.txt", ofstream::app);

  if (m_simuParams.propMethod == MAGNUS)
  {
    m_simuParams.curved = m_simuParamsManus.curved;
    m_simuParams.varyingArea = m_simuParamsManus.varyingArea;
    m_simuParams.numIntegrationStep = m_simuParamsManus.numIntegrationStep;
  }

  wxString st;

  st = wxString::Format("%2.1f", m_simuParams.temperature);
  txtTemperature->SetValue(st);

  st = wxString::Format("%2.1f", m_simuParams.sndSpeed/100.);
  txtSndSpeed->SetValue(st);

  st = wxString::Format("%2.1f", m_meshDensity);
  txtMeshDensity->SetValue(st);

  st = wxString::Format("%6.f", m_simuParams.maxCutOnFreq);
  txtMaxCutOnFreq->SetValue(st);

  st = wxString::Format("%5.f", m_simuParams.maxComputedFreq);
  txtMaxSimFreq->SetValue(st);

  st = wxString::Format("%d", m_simuParamsManus.numIntegrationStep);
  txtNumIntegrationStep->SetValue(st);

  st = wxString::Format("%d", m_secNoiseSource);
  txtSecNoiseSource->SetValue(st);

  //st = wxString::Format("%d", m_expSpectrumLgth);
  //txtExpLgth->SetValue(st);

  //st = wxString::Format(" %2.1f Hz", (double)SAMPLING_RATE / 2./
      //(double)(1 << (m_expSpectrumLgth - 1)));
  //txtNbFreqs->SetLabel(st);

	st = wxString::Format("%1.2f", 100. * m_simuParams.percentageLosses);
	txtPercLoss->SetValue(st);

	chkFdepLosses->SetValue(m_simuParams.freqDepLosses);

  chkWallLosses->SetValue(m_simuParams.wallLosses);

  if (m_simuParams.freqDepLosses || m_simuParams.wallLosses)
  {
    chkWallAdmittance->SetValue(false);
  }
  else
  {
    chkWallAdmittance->SetValue(true);
  }

  switch (m_simuParams.propMethod) {
      case MAGNUS:
          chkStraight->SetValue(false);
    chkMagnus->SetValue(true);
          break;
      case STRAIGHT_TUBES:
          chkStraight->SetValue(true);
    chkMagnus->SetValue(false);
          break;
  }

	chkCurv->SetValue(m_simuParamsManus.curved);

	chkVarArea->SetValue(m_simuParamsManus.varyingArea);

  // scaling factor computation method
  switch (m_contInterpMeth)
  {
  case AREA:
    lstScalingFacMethods->SetValue(m_listScalingMethods[0]);
    break;
  case BOUNDING_BOX:
    lstScalingFacMethods->SetValue(m_listScalingMethods[1]);
    break;
  case FROM_FILE:
    lstScalingFacMethods->SetValue(m_listScalingMethods[2]);
    break;
  }

  // set compute acoustic field options
  st = wxString::Format("%1.1f", m_simuParams.freqField);
  txtFreqComputeField->SetValue(st);

  st = wxString::Format("%d", m_simuParams.fieldResolution);
  txtResolutionField->SetValue(st);

  st = wxString::Format("%1.1f", m_simuParams.bbox[0].x());
  txtBboxMinX->SetValue(st);

  st = wxString::Format("%1.1f", m_simuParams.bbox[0].y());
  txtBboxMinY->SetValue(st);

  st = wxString::Format("%1.1f", m_simuParams.bbox[1].x());
  txtBboxMaxX->SetValue(st);

  st = wxString::Format("%1.1f", m_simuParams.bbox[1].y());
  txtBboxMaxY->SetValue(st);

  // transfer function point
  st = wxString::Format("%1.1f", m_simuParams.tfPoint[0].x());
  txtTfPointX->SetValue(st);

  st = wxString::Format("%1.1f", m_simuParams.tfPoint[0].y());
  txtTfPointY->SetValue(st);

  st = wxString::Format("%1.1f", m_simuParams.tfPoint[0].z());
  txtTfPointZ->SetValue(st);

  // mouth boundary condition
  switch (m_mouthBoundaryCond)
  {
  case RADIATION:
    lstMouthBcond->SetValue(m_listMouthBcond[0]);
    break;
  case IFINITE_WAVGUIDE:
    lstMouthBcond->SetValue(m_listMouthBcond[0]);
    break;
  case HARD_WALL:
    lstMouthBcond->SetValue(m_listMouthBcond[1]);
    break;
  case ADMITTANCE_1:
    lstMouthBcond->SetValue(m_listMouthBcond[1]);
    break;
  case ZERO_PRESSURE:
    lstMouthBcond->SetValue(m_listMouthBcond[2]);
    break;
  default:
    lstMouthBcond->SetValue(m_listMouthBcond[1]);
    break;
  }

  // frequency resolution
  lstFreqRes->SetValue(m_listFreqRes[m_simuParams.spectrumLgthExponent - m_expSpectrumLgthStart]);

  st = wxString::Format("%1.4f", real(m_simuParams.thermalBndSpecAdm));
  txtWallAdmit->SetValue(st);

  chkComputeRad->SetValue(m_simuParams.computeRadiatedField);

  m_simu3d->setSimulationParameters(m_meshDensity, m_secNoiseSource, 
		m_simuParams, m_mouthBoundaryCond, m_contInterpMeth);

  //log.close();
}

// ****************************************************************************

void ParamSimu3DDialog::updateParams()
{
  m_meshDensity = m_simu3d->meshDensity();
  m_secNoiseSource = m_simu3d->idxSecNoiseSource();
  m_simuParams.spectrumLgthExponent = m_simu3d->spectrumLgthExponent();
  m_mouthBoundaryCond = m_simu3d->mouthBoundaryCond();
  m_contInterpMeth = m_simu3d->contInterpMeth();
  m_simuParams = m_simu3d->simuParams();
  if (m_simuParams.propMethod == MAGNUS)
  {
    m_simuParamsManus = m_simu3d->simuParams();
  }
}

// ****************************************************************************
/// Set the windows that receive an update request message when a vocal tract
/// parameter was changed.
// ****************************************************************************

void ParamSimu3DDialog::setUpdateRequestReceiver(wxWindow* receiver)
    
{
    updateRequestReceiver = receiver;
}

// ****************************************************************************
// To update the segment picture and reload the geometry
// ****************************************************************************

void ParamSimu3DDialog::updateGeometry()
{
  if (updateRequestReceiver != NULL)
  {
    wxCommandEvent event(updateRequestEvent);
    event.SetInt(UPDATE_VOCAL_TRACT);
    wxPostEvent(updateRequestReceiver, event);
  }
}

// ****************************************************************************
// To update the pictures
// ****************************************************************************

void ParamSimu3DDialog::updatePictures()
{
  if (updateRequestReceiver != NULL)
  {
    wxCommandEvent event(updateRequestEvent);
    event.SetInt(UPDATE_PICTURES_AND_CONTROLS);
    wxPostEvent(updateRequestReceiver, event);
  }
}

// ****************************************************************************
/// Constructor.
// ****************************************************************************

ParamSimu3DDialog::ParamSimu3DDialog(wxWindow* parent) :
  wxDialog(parent, wxID_ANY, wxString("Parameters 3D simulations"),
    wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE)
{

  m_simu3d = Acoustic3dSimulation::getInstance();
  m_tract = Data::getInstance()->vocalTract;
  m_meshDensity = m_simu3d->meshDensity();
  m_secNoiseSource = m_simu3d->idxSecNoiseSource();
  m_simuParams.spectrumLgthExponent = m_simu3d->spectrumLgthExponent();
  m_mouthBoundaryCond = m_simu3d->mouthBoundaryCond();
  m_contInterpMeth = m_simu3d->contInterpMeth(); 
	m_simuParams = m_simu3d->simuParams();
  m_simuParamsManus = m_simu3d->simuParams();
  m_maxBbox = 100.;

  this->Move(100, 100);

  // ****************************************************************
  // Init and update the widgets.
  // ****************************************************************

  initWidgets();

  // create the list of scaling factor computation methods
  m_listScalingMethods.clear();
  m_listScalingMethods.push_back("Area");
  m_listScalingMethods.push_back("Bounding box");
  m_listScalingMethods.push_back("From file");

  lstScalingFacMethods->Clear();
  for (int i(0); i < m_listScalingMethods.size(); i++)
  {
    lstScalingFacMethods->Append(m_listScalingMethods[i]);
  }

  // create the list of boundary conditions
  m_listMouthBcond.clear();
  m_listMouthBcond.push_back("Radiation");
  m_listMouthBcond.push_back("Constant admittance");
  m_listMouthBcond.push_back("Zero pressure");

  lstMouthBcond->Clear();
  for (int i(0); i < m_listMouthBcond.size(); i++)
  {
    lstMouthBcond->Append(m_listMouthBcond[i]);
  }

  // create the list of frequency resolutions
  m_expSpectrumLgthStart = 8;
  m_expSpectrumLgthEnd = 15;
  m_listFreqRes.clear();
  wxString st;
  for (int i(m_expSpectrumLgthStart); i < m_expSpectrumLgthEnd; i++)
  {
  st = wxString::Format(" %2.1f Hz", (double)SAMPLING_RATE / 2./
      (double)(1 << (i - 1)));
  m_listFreqRes.push_back(st.ToStdString());
  }

  lstFreqRes->Clear();
  for (int i(0); i < m_listFreqRes.size(); i++)
  {
    lstFreqRes->Append(m_listFreqRes[i]);
  }

  updateWidgets();
}

// ****************************************************************************
/// Init the widgets of this dialog.
// ****************************************************************************

void ParamSimu3DDialog::initWidgets()
{
    wxBoxSizer* topLevelSizer = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* lineSizer = NULL;
    wxStaticText* label = NULL;
    wxButton* button; 

    // ****************************************************************
    // Set temperature or sound speed.
    // ****************************************************************

    topLevelSizer->AddSpacer(10);

    lineSizer = new wxBoxSizer(wxHORIZONTAL);

    label = new wxStaticText(this, wxID_ANY, "Temperature (°C): ");
    lineSizer->Add(label, 1, wxALL | wxALIGN_CENTER, 3);

    txtTemperature = new wxTextCtrl(this, IDE_TEMPERATURE, "", wxDefaultPosition,
      wxSize(60, -1), wxTE_PROCESS_ENTER);
    lineSizer->Add(txtTemperature, 1, wxALL, 3);

    //lineSizer->AddStretchSpacer();

    label = new wxStaticText(this, wxID_ANY, "Sound speed (m/s): ");
    lineSizer->Add(label, 1, wxALL | wxALIGN_CENTER, 3);

    txtSndSpeed = new wxTextCtrl(this, IDE_SND_SPEED, "", wxDefaultPosition,
      wxSize(60, -1), wxTE_PROCESS_ENTER);
    lineSizer->Add(txtSndSpeed, 1, wxALL, 3);

    topLevelSizer->Add(lineSizer, 0, wxLEFT | wxRIGHT | wxEXPAND, 10);

    // ****************************************************************
    // Set mesh density.
    // ****************************************************************

    topLevelSizer->AddSpacer(10);

    lineSizer = new wxBoxSizer(wxHORIZONTAL);

    label = new wxStaticText(this, wxID_ANY, "Mesh density (elements per length): ");
    lineSizer->Add(label, 0, wxALL | wxALIGN_CENTER, 3);

    txtMeshDensity = new wxTextCtrl(this, IDE_MESH_DENSITY, "", wxDefaultPosition,
        wxSize(60, -1), wxTE_PROCESS_ENTER);
    lineSizer->Add(txtMeshDensity, 0, wxALL, 3);

    //label = new wxStaticText(this, wxID_ANY, "Elements per length");
    //lineSizer->Add(label, 0, wxALL | wxALIGN_CENTER, 3);

    topLevelSizer->Add(lineSizer, 0, wxLEFT | wxRIGHT | wxEXPAND, 10);

    // ****************************************************************
    // Set the maximal cut-on frequency
    // ****************************************************************

    topLevelSizer->AddSpacer(10);

    lineSizer = new wxBoxSizer(wxHORIZONTAL);

    label = new wxStaticText(this, wxID_ANY, "Maximal cut-off frequency (Hz): ");
    lineSizer->Add(label, 0, wxALL | wxALIGN_CENTER, 3);

    txtMaxCutOnFreq = new wxTextCtrl(this, IDE_MAX_CUT_ON, "", wxDefaultPosition,
        wxSize(80, -1), wxTE_PROCESS_ENTER);
    lineSizer->Add(txtMaxCutOnFreq, 0, wxALL, 3);

    topLevelSizer->Add(lineSizer, 0, wxLEFT | wxRIGHT | wxEXPAND, 10);

    // ****************************************************************
    // Select the numerical scheme
    // ****************************************************************

    topLevelSizer->AddSpacer(10);

    lineSizer = new wxBoxSizer(wxHORIZONTAL);

    label = new wxStaticText(this, wxID_ANY, "Numerical scheme: ");
    lineSizer->Add(label, 1, wxALL | wxALIGN_CENTER, 3);

    chkStraight = new wxCheckBox(this, IDB_CHK_STRAIGHT, "Straight");
    lineSizer->Add(chkStraight, 1, wxALL, 2);

    chkMagnus = new wxCheckBox(this, IDB_CHK_MAGNUS, "Magnus");
    lineSizer->Add(chkMagnus, 1, wxALL, 2);

    topLevelSizer->Add(lineSizer, 0, wxLEFT | wxRIGHT | wxEXPAND, 10);

    // ****************************************************************
    // Set the number of integration steps
    // ****************************************************************

    topLevelSizer->AddSpacer(10);

    lineSizer = new wxBoxSizer(wxHORIZONTAL);

    label = new wxStaticText(this, wxID_ANY, "Number of integration steps: ");
    lineSizer->Add(label, 0, wxALL | wxALIGN_CENTER, 3);

    txtNumIntegrationStep = new wxTextCtrl(this, IDE_NUM_INTEGRATION, "", wxDefaultPosition,
      wxSize(60, -1), wxTE_PROCESS_ENTER);
    lineSizer->Add(txtNumIntegrationStep, 0, wxALL, 3);

    topLevelSizer->Add(lineSizer, 0, wxLEFT | wxRIGHT | wxEXPAND, 10);

    // ****************************************************************
    // Select the geometrical features
    // ****************************************************************

    topLevelSizer->AddSpacer(10);

    lineSizer = new wxBoxSizer(wxHORIZONTAL);

    chkCurv = new wxCheckBox(this, IDB_CHK_CURV, "Curved");
    lineSizer->Add(chkCurv, 1, wxALL, 2);

    chkVarArea = new wxCheckBox(this, IDB_CHK_VAR_AREA, "Varying area");
    lineSizer->Add(chkVarArea, 1, wxALL, 2);

    lstScalingFacMethods = new wxComboBox(this, IDL_SCALING_FAC_METHOD, "", wxDefaultPosition,
      this->FromDIP(wxSize(150, -1)), wxArrayString(), wxCB_DROPDOWN | wxCB_READONLY);
    lineSizer->Add(lstScalingFacMethods, 1, wxALL, 3);

    topLevelSizer->Add(lineSizer, 0, wxLEFT | wxRIGHT | wxEXPAND, 10);

    // ****************************************************************
    // Select mouth boundary condition
    // ****************************************************************

    topLevelSizer->AddSpacer(10);

    lineSizer = new wxBoxSizer(wxHORIZONTAL);

    label = new wxStaticText(this, wxID_ANY, "Mouth boundary condition ");
    lineSizer->Add(label, 0, wxALL | wxALIGN_CENTER, 3);

    lstMouthBcond = new wxComboBox(this, IDL_MOUTH_BCOND, "", wxDefaultPosition,
      this->FromDIP(wxSize(150, -1)), wxArrayString(), wxCB_DROPDOWN | wxCB_READONLY);
    lineSizer->Add(lstMouthBcond, 0, wxALL, 3);

    topLevelSizer->Add(lineSizer, 0, wxLEFT | wxRIGHT, 10);


    // ****************************************************************
    // Set the percentage of losses
    // ****************************************************************

    topLevelSizer->AddSpacer(10);

    lineSizer = new wxBoxSizer(wxHORIZONTAL);

    label = new wxStaticText(this, wxID_ANY, "Percentage of losses: ");
    lineSizer->Add(label, 0, wxALL | wxALIGN_CENTER, 3);

    txtPercLoss = new wxTextCtrl(this, IDE_PERCENT_LOSSES, "", wxDefaultPosition,
      wxSize(60, -1), wxTE_PROCESS_ENTER);
    lineSizer->Add(txtPercLoss, 0, wxALL | wxALIGN_CENTER, 3);

    topLevelSizer->Add(lineSizer, 0, wxLEFT | wxRIGHT, 10);

    // ****************************************************************
    // Set options losses
    // ****************************************************************

    topLevelSizer->AddSpacer(10);

    lineSizer = new wxBoxSizer(wxHORIZONTAL);

    chkFdepLosses = new wxCheckBox(this, IDB_CHK_FDEP_LOSSES,
      "Visco-thermal losses");
    lineSizer->Add(chkFdepLosses, 0, wxALL, 2);
    chkFdepLosses->Disable();

    chkWallLosses = new wxCheckBox(this, IDB_CHK_WALL_LOSSES,
      "Soft walls");
    lineSizer->Add(chkWallLosses, 0, wxALL, 2);

    topLevelSizer->Add(lineSizer, 0, wxLEFT | wxRIGHT, 10);

    // ****************************************************************

    topLevelSizer->AddSpacer(10);

    lineSizer = new wxBoxSizer(wxHORIZONTAL);

    chkWallAdmittance = new wxCheckBox(this, IDB_CHK_WALL_ADMITTANCE,
      "Constant wall admittance");
    lineSizer->Add(chkWallAdmittance, 0, wxALL, 2);

    label = new wxStaticText(this, wxID_ANY, " admittance ");
    lineSizer->Add(label, 0, wxALL, 2);

    txtWallAdmit = new wxTextCtrl(this, IDE_WALL_ADMIT, "", wxDefaultPosition,
      wxSize(60, -1), wxTE_PROCESS_ENTER);
    lineSizer->Add(txtWallAdmit, 0, wxALL, 2);

    topLevelSizer->Add(lineSizer, 0, wxLEFT | wxRIGHT, 10);

    ///////////////////////////////////////////////////////////////////
    // Transfer function options
    ///////////////////////////////////////////////////////////////////

    // ****************************************************************
    // Set the maximal computed frequency
    // ****************************************************************

    topLevelSizer->AddSpacer(20);

    label = new wxStaticText(this, wxID_ANY, "Transfer function computation options");
    topLevelSizer->Add(label, 0, wxALL | wxALIGN_LEFT, 3);

    lineSizer = new wxBoxSizer(wxHORIZONTAL);

    label = new wxStaticText(this, wxID_ANY, "Maximal simulated frequency: ");
    lineSizer->Add(label, 0, wxALL | wxALIGN_CENTER, 3);

    txtMaxSimFreq = new wxTextCtrl(this, IDE_MAX_SIM_FREQ, "", wxDefaultPosition,
        wxSize(80, -1), wxTE_PROCESS_ENTER);
    lineSizer->Add(txtMaxSimFreq, 0, wxALL, 3);

    topLevelSizer->Add(lineSizer, 0, wxLEFT | wxRIGHT, 10);

    // ****************************************************************
    // Set the section containing the noise source
    // ****************************************************************

    topLevelSizer->AddSpacer(10);

    lineSizer = new wxBoxSizer(wxHORIZONTAL);

    label = new wxStaticText(this, wxID_ANY, "Index of noise source section: ");
    lineSizer->Add(label, 0, wxALL | wxALIGN_CENTER, 3);

    txtSecNoiseSource = new wxTextCtrl(this, IDE_SEC_NOISE_SOURCE, "", wxDefaultPosition,
        wxSize(40, -1), wxTE_PROCESS_ENTER);
    lineSizer->Add(txtSecNoiseSource, 0, wxALL, 3);

    topLevelSizer->Add(lineSizer, 0, wxLEFT | wxRIGHT, 10);

    // ****************************************************************
    // Set the length of the spectrum
    // ****************************************************************

    topLevelSizer->AddSpacer(10);

    lineSizer = new wxBoxSizer(wxHORIZONTAL);

    label = new wxStaticText(this, wxID_ANY, "Frequency resolution");
    lineSizer->Add(label, 0, wxALL | wxALIGN_CENTER, 3);

    //txtExpLgth = new wxTextCtrl(this, IDE_EXP_SPECTRUM_LGTH, "", wxDefaultPosition,
        //wxSize(40, -1), wxTE_PROCESS_ENTER);
    //lineSizer->Add(txtExpLgth, 0, wxALL, 3);

    //wxString st(wxString::Format(" %d freqs", 1 << (m_expSpectrumLgth - 1)));
    //txtNbFreqs = new wxStaticText(this, wxID_ANY, st);
    //lineSizer->Add(txtNbFreqs, 0, wxALL | wxALIGN_CENTER, 3);

    lstFreqRes = new wxComboBox(this, IDL_FREQ_RES, "", wxDefaultPosition,
      this->FromDIP(wxSize(150, -1)), wxArrayString(), wxCB_DROPDOWN | wxCB_READONLY);
    lineSizer->Add(lstFreqRes, 0, wxALL, 3);

    topLevelSizer->Add(lineSizer, 0, wxLEFT | wxRIGHT, 10);

    // ****************************************************************
    // Set the point for transfer function computation
    // ****************************************************************

    topLevelSizer->AddSpacer(10);

    lineSizer = new wxBoxSizer(wxHORIZONTAL);

    label = new wxStaticText(this, wxID_ANY, "Transfer function point (cm) X ");
    lineSizer->Add(label, 0, wxALL | wxALIGN_CENTER, 3);

    txtTfPointX = new wxTextCtrl(this, IDE_TF_POINT_X, "", wxDefaultPosition,
      wxSize(40, -1), wxTE_PROCESS_ENTER);
    lineSizer->Add(txtTfPointX, 0, wxALL, 3);

    label = new wxStaticText(this, wxID_ANY, " Y ");
    lineSizer->Add(label, 0, wxALL | wxALIGN_CENTER, 3);

    txtTfPointY = new wxTextCtrl(this, IDE_TF_POINT_Y, "", wxDefaultPosition,
      wxSize(40, -1), wxTE_PROCESS_ENTER);
    lineSizer->Add(txtTfPointY, 0, wxALL, 3);

    label = new wxStaticText(this, wxID_ANY, " Z ");
    lineSizer->Add(label, 0, wxALL | wxALIGN_CENTER, 3);

    txtTfPointZ = new wxTextCtrl(this, IDE_TF_POINT_Z, "", wxDefaultPosition,
      wxSize(40, -1), wxTE_PROCESS_ENTER);
    lineSizer->Add(txtTfPointZ, 0, wxALL, 3);

    topLevelSizer->Add(lineSizer, 0, wxLEFT | wxRIGHT, 10);

    // ****************************************************************
    // Load the transfer function points from a file
    // ****************************************************************

    topLevelSizer->AddSpacer(10);

    lineSizer = new wxBoxSizer(wxHORIZONTAL);

    chkMultiTFPts = new wxCheckBox(this, IDB_CHK_MULTI_TF_PTS,
      "Multiple transfer function points");
    lineSizer->Add(chkMultiTFPts, 0, wxGROW | wxALL, 3);
    chkMultiTFPts->Disable();

    button = new wxButton(this, IDB_LOAD_TF_POINTS, 
      "Load coordinates to evaluate transfer functions");
    lineSizer->Add(button, 0, wxGROW | wxALL, 3);

    topLevelSizer->Add(lineSizer, 0, wxLEFT | wxRIGHT, 10);

    ///////////////////////////////////////////////////////////////////
    // Acoustic field computation options
    ///////////////////////////////////////////////////////////////////

  // ****************************************************************
  // Select the frequency and the resolution for field computation
  // ****************************************************************

  topLevelSizer->AddSpacer(20);

  label = new wxStaticText(this, wxID_ANY, "Acoustic field computation options");
  topLevelSizer->Add(label, 0, wxALL | wxALIGN_LEFT , 3);

  lineSizer = new wxBoxSizer(wxHORIZONTAL);

  label = new wxStaticText(this, wxID_ANY, "Frequency ");
  lineSizer->Add(label, 0, wxALL | wxALIGN_CENTER, 3);

  txtFreqComputeField = new wxTextCtrl(this, IDE_FREQ_COMPUTE_FIELD, "", wxDefaultPosition,
    wxSize(80, -1), wxTE_PROCESS_ENTER);
  lineSizer->Add(txtFreqComputeField, 0, wxALL, 3);

  label = new wxStaticText(this, wxID_ANY, " Hz, resolution ");
  lineSizer->Add(label, 0, wxALL | wxALIGN_CENTER, 3);

  txtResolutionField = new wxTextCtrl(this, IDE_RESOLUTION_FIELD, "", wxDefaultPosition,
    wxSize(50, -1), wxTE_PROCESS_ENTER);
  lineSizer->Add(txtResolutionField, 0, wxALL, 3);

  label = new wxStaticText(this, wxID_ANY, " point per cm");
  lineSizer->Add(label, 0, wxALL | wxALIGN_CENTER, 3);

  topLevelSizer->Add(lineSizer, 0, wxLEFT | wxRIGHT, 10);

  // ****************************************************************
  // Set the bounding box to compute the acoustic field
  // ****************************************************************

  topLevelSizer->AddSpacer(10);

  lineSizer = new wxBoxSizer(wxHORIZONTAL);

  label = new wxStaticText(this, wxID_ANY, "Min X ");
  lineSizer->Add(label, 0, wxALL | wxALIGN_CENTER, 3);

  txtBboxMinX = new wxTextCtrl(this, IDE_BBOX_MIN_X, "", wxDefaultPosition,
    wxSize(50, -1), wxTE_PROCESS_ENTER);
  lineSizer->Add(txtBboxMinX, 0, wxALL, 3);

  label = new wxStaticText(this, wxID_ANY, " max X ");
  lineSizer->Add(label, 0, wxALL | wxALIGN_CENTER, 3);

  txtBboxMaxX = new wxTextCtrl(this, IDE_BBOX_MAX_X, "", wxDefaultPosition,
    wxSize(50, -1), wxTE_PROCESS_ENTER);
  lineSizer->Add(txtBboxMaxX, 0, wxALL, 3);

  label = new wxStaticText(this, wxID_ANY, " min Y ");
  lineSizer->Add(label, 0, wxALL | wxALIGN_CENTER, 3);

  txtBboxMinY = new wxTextCtrl(this, IDE_BBOX_MIN_Y, "", wxDefaultPosition,
    wxSize(50, -1), wxTE_PROCESS_ENTER);
  lineSizer->Add(txtBboxMinY, 0, wxALL, 3);

  label = new wxStaticText(this, wxID_ANY, " max Y ");
  lineSizer->Add(label, 0, wxALL | wxALIGN_CENTER, 3);

  txtBboxMaxY = new wxTextCtrl(this, IDE_BBOX_MAX_Y, "", wxDefaultPosition,
    wxSize(50, -1), wxTE_PROCESS_ENTER);
  lineSizer->Add(txtBboxMaxY, 0, wxALL, 3);

  topLevelSizer->Add(lineSizer, 0, wxLEFT | wxRIGHT, 10);

  // ****************************************************************
  // Set if the radiated field must be computed or not
  // ****************************************************************

  topLevelSizer->AddSpacer(10);

  lineSizer = new wxBoxSizer(wxHORIZONTAL);

  chkComputeRad = new wxCheckBox(this, IDB_COMPUTE_RAD_FIELD,
    "Compute radiated field");
  chkComputeRad->SetValue(m_simuParams.computeRadiatedField);
  lineSizer->Add(chkComputeRad, 0, wxALL, 2);

  topLevelSizer->Add(lineSizer, 0, wxLEFT | wxRIGHT, 10);

  // ****************************************************************
  // Set the default parameters
  // ****************************************************************

  topLevelSizer->AddSpacer(10);
  topLevelSizer->Add(new wxStaticLine(this), 1, wxEXPAND | wxALL, 10);

  lineSizer = new wxBoxSizer(wxHORIZONTAL);

  button = new wxButton(this, IDB_SET_DEFAULT_PARAMS_FAST,
    "Default (fast)");
  lineSizer->Add(button, 1, wxEXPAND | wxALL, 3);

  button = new wxButton(this, IDB_SET_DEFAULT_PARAMS_ACCURATE,
    "Default (accurate)");
  lineSizer->Add(button, 1, wxEXPAND | wxALL, 3);

  lineSizer->AddStretchSpacer();

  button = new wxButton(this, IDB_CLOSE, "Close");
  lineSizer->Add(button, 1, wxEXPAND | wxALL, 3);

  topLevelSizer->Add(lineSizer, 0, wxEXPAND | wxALL, 10);

  // ****************************************************************
  // Set the top-level-sizer for this window.
  // ****************************************************************

  //topLevelSizer->AddSpacer(10);

  //topLevelSizer->Add(CreateSeparatedButtonSizer(wxCLOSE), 1, wxALL | wxEXPAND, 10);

  this->SetSizer(topLevelSizer);
  topLevelSizer->Fit(this);
}

// ****************************************************************************
// ****************************************************************************

void ParamSimu3DDialog::OnTemperatureEnter(wxCommandEvent& event)
{
  double x = 0.0;
  wxString st = txtTemperature->GetValue();
  if ((st.ToDouble(&x)) && (x >= -300.0) && (x <= 300.0))
  {
    m_simuParams.temperature = x;
    m_simuParams.volumicMass = STATIC_PRESSURE_CGS * MOLECULAR_MASS / (GAS_CONSTANT *
      (m_simuParams.temperature + KELVIN_SHIFT));
    m_simuParams.sndSpeed = 
      sqrt(ADIABATIC_CONSTANT * STATIC_PRESSURE_CGS / m_simuParams.volumicMass);
  }

  updateWidgets();
}

// ****************************************************************************
// ****************************************************************************

void ParamSimu3DDialog::OnSndSpeedEnter(wxCommandEvent& event)
{
  double x = 0.0;
  wxString st = txtSndSpeed->GetValue();
  if ((st.ToDouble(&x)) && (x >= 0.) && (x <= 10000.0))
  {
    m_simuParams.sndSpeed = x*100.;
    m_simuParams.volumicMass = ADIABATIC_CONSTANT * STATIC_PRESSURE_CGS /
      pow(m_simuParams.sndSpeed, 2);
    m_simuParams.temperature = pow(m_simuParams.sndSpeed, 2) * MOLECULAR_MASS /
      GAS_CONSTANT / ADIABATIC_CONSTANT - KELVIN_SHIFT;
  }

  updateWidgets();
}

// ****************************************************************************
// ****************************************************************************

void ParamSimu3DDialog::OnMeshDensityEnter(wxCommandEvent& event)
{
    double x = 0.0;
    wxString st = txtMeshDensity->GetValue();
    if ((st.ToDouble(&x)) && (x >= 1.0) && (x <= 50.0))
    {
        m_meshDensity = x;
    }
    m_simuParams.needToComputeModesAndJunctions = true;
    m_simuParams.radImpedPrecomputed = false;

    updateWidgets();
}

// ****************************************************************************
// ****************************************************************************

void ParamSimu3DDialog::OnMaxCutOnEnter(wxCommandEvent& event)
{
    double x = 0.0;
    wxString st = txtMaxCutOnFreq->GetValue();
    if ((st.ToDouble(&x)) && (x >= 0.0) && (x <= 500000.0))
    {
        m_simuParams.maxCutOnFreq = x;
    }
    m_simuParams.needToComputeModesAndJunctions = true;
    m_simuParams.radImpedPrecomputed = false;

    updateWidgets();
}

// ****************************************************************************
// ****************************************************************************

void ParamSimu3DDialog::OnMaxSimFreq(wxCommandEvent& event)
{
    double x(0.);
    wxString st(txtMaxSimFreq->GetValue());
    if ((st.ToDouble(&x)) && (x >= 0.) && (x <= (double)SAMPLING_RATE))
    {
        m_simuParams.maxComputedFreq = x;
    }

    updateWidgets();
}

// ****************************************************************************
// ****************************************************************************

void ParamSimu3DDialog::OnNumIntegrationEnter(wxCommandEvent& event)
{
    double x = 0.0;
    wxString st = txtNumIntegrationStep->GetValue();
    if ((st.ToDouble(&x)) && (x > 0.) && (x <= 1000.))
    {
      m_simuParamsManus.numIntegrationStep = (int)x;
    }

    updateWidgets();
}

// ****************************************************************************
// ****************************************************************************

void ParamSimu3DDialog::OnSecNoiseSourceEnter(wxCommandEvent& event)
{
    double x(0.);
    wxString st = txtSecNoiseSource->GetValue();
    if ((st.ToDouble(&x)) && (x >= 0.) && (x <= 500.))
    {
        m_secNoiseSource = (int)x;
    }

    updateWidgets();
    updatePictures();
}

// ****************************************************************************
// ****************************************************************************

//void ParamSimu3DDialog::OnExpSpectrumLgthEnter(wxCommandEvent& event)
//{
    //double x(0.);
    //wxString st = txtExpLgth->GetValue();
    //if ((st.ToDouble(&x)) && (x >= 0.) && (x <= 16.))
    //{
        //m_expSpectrumLgth = (int)x;
    //}

    //updateWidgets();
//}

// ****************************************************************************
// ****************************************************************************

void ParamSimu3DDialog::OnPercentLosses(wxCommandEvent& event)
{
	double x(0.);
	wxString st = txtPercLoss->GetValue();
	if ((st.ToDouble(&x)) && (x >= 0.) && (x <= 100.))
	{
		m_simuParams.percentageLosses = x/100.;
	}

	updateWidgets();
}

// ****************************************************************************
// ****************************************************************************

void ParamSimu3DDialog::OnFreqComputeField(wxCommandEvent& event)
{
  double x(0.);
  wxString st = txtFreqComputeField->GetValue();
  if ((st.ToDouble(&x)) && (x >= 1.) && (x <= 20000.))
  {
    m_simuParams.freqField = x;
  }
  updateWidgets();
}

// ****************************************************************************
// ****************************************************************************

void ParamSimu3DDialog::OnResolutionField(wxCommandEvent& event)
{
  double x(0.);
  wxString st = txtResolutionField->GetValue();
  if ((st.ToDouble(&x)) && (x >= 1.) && (x <= 500.))
  {
    m_simuParams.fieldResolution = (int)x;
  }
  updateWidgets();
}

// ****************************************************************************
// ****************************************************************************

void ParamSimu3DDialog::OnBboxMinX(wxCommandEvent& event)
{
  double x(0.);
  wxString st = txtBboxMinX->GetValue();
  if ((st.ToDouble(&x)) && (x >= -m_maxBbox) && (x <= m_maxBbox))
  {
    m_simuParams.bbox[0] = Point(x, m_simuParams.bbox[0].y());
  }
  updateWidgets();
  updatePictures();
}

// ****************************************************************************
// ****************************************************************************

void ParamSimu3DDialog::OnBboxMinY(wxCommandEvent& event)
{
  double x(0.);
  wxString st = txtBboxMinY->GetValue();
  if ((st.ToDouble(&x)) && (x >= -m_maxBbox) && (x <= m_maxBbox))
  {
    m_simuParams.bbox[0] = Point(m_simuParams.bbox[0].x(), x);
  }
  updateWidgets();
  updatePictures();
}

// ****************************************************************************
// ****************************************************************************

void ParamSimu3DDialog::OnBboxMaxX(wxCommandEvent& event)
{
  double x(0.);
  wxString st = txtBboxMaxX->GetValue();
  if ((st.ToDouble(&x)) && (x >= -m_maxBbox) && (x <= m_maxBbox))
  {
    m_simuParams.bbox[1] = Point(x, m_simuParams.bbox[1].y());
  }
  updateWidgets();
  updatePictures();
}

// ****************************************************************************
// ****************************************************************************

void ParamSimu3DDialog::OnBboxMaxY(wxCommandEvent& event)
{
  double x(0.);
  wxString st = txtBboxMaxY->GetValue();
  if ((st.ToDouble(&x)) && (x >= -m_maxBbox) && (x <= m_maxBbox))
  {
    m_simuParams.bbox[1] = Point(m_simuParams.bbox[1].x(), x);
  }
  updateWidgets();
  updatePictures();
}

// ****************************************************************************
// ****************************************************************************

void ParamSimu3DDialog::OnTfPointX(wxCommandEvent& event)
{
  double x(0.);
  wxString st = txtTfPointX->GetValue();
  if ((st.ToDouble(&x)) && (x >= -100.) && (x <= 100.))
  {
    m_simuParams.tfPoint[0] = Point_3(x, m_simuParams.tfPoint[0].y(), 
      m_simuParams.tfPoint[0].z());
  }
  chkMultiTFPts->SetValue(false);
  updateWidgets();
  updatePictures();
}

// ****************************************************************************
// ****************************************************************************

void ParamSimu3DDialog::OnTfPointY(wxCommandEvent& event)
{
  double x(0.);
  wxString st = txtTfPointY->GetValue();
  if ((st.ToDouble(&x)) && (x >= -100.) && (x <= 100.))
  {
    m_simuParams.tfPoint[0] = Point_3(m_simuParams.tfPoint[0].x(), x,
      m_simuParams.tfPoint[0].z());
  }
  chkMultiTFPts->SetValue(false);
  updateWidgets();
  updatePictures();
}

// ****************************************************************************
// ****************************************************************************

void ParamSimu3DDialog::OnTfPointZ(wxCommandEvent& event)
{
  double x(0.);
  wxString st = txtTfPointZ->GetValue();
  if ((st.ToDouble(&x)) && (x >= -100.) && (x <= 100.))
  {
    m_simuParams.tfPoint[0] = Point_3(m_simuParams.tfPoint[0].x(),
      m_simuParams.tfPoint[0].y(), x);
  }
  chkMultiTFPts->SetValue(false);
  updateWidgets();
  updatePictures();
}

// ****************************************************************************
// ****************************************************************************

void ParamSimu3DDialog::OnLoadTfPts(wxCommandEvent& event)
{
  wxFileName fileName;
  wxString name = wxFileSelector("Import transfer function points as csv file", fileName.GetPath(),
  fileName.GetFullName(), ".csv", "Points file (*.csv)|*.csv",
  wxFD_OPEN | wxFD_FILE_MUST_EXIST, this);

  ofstream log("log.txt", ofstream::app);
  log << "Load tf points from file:" << endl;
  log << name.ToStdString() << endl;
  bool success = m_simu3d->setTFPointsFromCsvFile(name.ToStdString());
  if (success)
  {
    log << "Importation successfull" << endl;
    chkMultiTFPts->SetValue(true);
    updatePictures();
  }
  else
  {
    log << "Importation failed" << endl;
    chkMultiTFPts->SetValue(false);
  }
  log.close();
}

// ****************************************************************************
// ****************************************************************************

void ParamSimu3DDialog::OnWallAdmitEnter(wxCommandEvent& event)
{
  double x(0.);
  wxString st = txtWallAdmit->GetValue();
  if ((st.ToDouble(&x)) && (x >= -10000000.) && (x <= 10000000.))
  {
    m_simuParams.thermalBndSpecAdm = complex<double>(x, 0.);
  }
  updateWidgets();
}

// ****************************************************************************
// ****************************************************************************

void ParamSimu3DDialog::OnChkFdepLosses(wxCommandEvent& event)
{
	m_simuParams.freqDepLosses = !m_simuParams.freqDepLosses;
	chkFdepLosses->SetValue(m_simuParams.freqDepLosses);
    if (!m_simuParams.freqDepLosses)
    {
        m_simuParams.wallLosses = false;
    }
    if (m_simuParams.freqDepLosses)
    {
      chkWallAdmittance->SetValue(false);
    }
	updateWidgets();
}

// ****************************************************************************
// ****************************************************************************

void ParamSimu3DDialog::OnChkWallLosses(wxCommandEvent& event)
{
    m_simuParams.wallLosses = !m_simuParams.wallLosses;
    chkWallLosses->SetValue(m_simuParams.wallLosses);
    if (m_simuParams.wallLosses)
    {
      chkWallAdmittance->SetValue(false);
    }
    if (!m_simuParams.freqDepLosses && m_simuParams.wallLosses)
    {
      m_simuParams.freqDepLosses = true;
      chkFdepLosses->SetValue(true);
    }
    
  updateWidgets();
}

// ****************************************************************************
// ****************************************************************************

void ParamSimu3DDialog::OnChkWallAdmittance(wxCommandEvent& event)
{
  m_simuParams.wallLosses = false;
  chkWallLosses->SetValue(m_simuParams.wallLosses);
  m_simuParams.freqDepLosses = false;
  chkFdepLosses->SetValue(m_simuParams.freqDepLosses);
  updateWidgets();
}

// ****************************************************************************
// ****************************************************************************

void ParamSimu3DDialog::OnChkStraight(wxCommandEvent& event)
{
  m_simuParams.propMethod = STRAIGHT_TUBES;
  m_simuParams.numIntegrationStep = 2;
	m_simuParams.curved = false;
  m_simuParams.varyingArea = false;
  chkCurv->Disable();
  chkVarArea->Disable();
  txtNumIntegrationStep->Disable();
  updateWidgets();
  updateGeometry();
}

// ****************************************************************************
// ****************************************************************************

void ParamSimu3DDialog::OnChkMagnus(wxCommandEvent& event)
{
	m_simuParams.propMethod = MAGNUS;
  chkCurv->Enable();
  chkVarArea->Enable();
  txtNumIntegrationStep->Enable();
  updateWidgets();
  updateGeometry();
}

// ****************************************************************************
// ****************************************************************************

void ParamSimu3DDialog::OnChkCurv(wxCommandEvent& event)
{
  m_simuParamsManus.curved = !m_simuParamsManus.curved;
  updateWidgets();

  updateGeometry();
}

// ****************************************************************************
// ****************************************************************************

void ParamSimu3DDialog::OnChkVarArea(wxCommandEvent& event)
{
  //m_simuParams.propMethod = MAGNUS;
  m_simuParamsManus.varyingArea = !m_simuParamsManus.varyingArea;
  updateWidgets();

  updateGeometry();
}

// ****************************************************************************
// ****************************************************************************

void ParamSimu3DDialog::OnChkMultiTFPts(wxCommandEvent& event)
{

}

// ****************************************************************************
// ****************************************************************************

void ParamSimu3DDialog::OnChkComputeRad(wxCommandEvent& event)
{
  m_simuParams.computeRadiatedField = !m_simuParams.computeRadiatedField;
  updateWidgets();
}

// ****************************************************************************
// ****************************************************************************

void ParamSimu3DDialog::OnScalingFactMethod(wxCommandEvent& event)
{
  auto res = lstScalingFacMethods->GetSelection();

  switch (res)
  {
  case 0:
    m_contInterpMeth = AREA;
    break;
  case 1:
    m_contInterpMeth = BOUNDING_BOX;
    break;
  case 2:
    if (m_simu3d->isGeometryImported())
    {
      m_contInterpMeth = FROM_FILE;
    }
    else
    {
      lstScalingFacMethods->SetValue(m_listScalingMethods[1]);
    }
    break;
  }

  updateWidgets();
  updateGeometry();
}

// ****************************************************************************
// ****************************************************************************

void ParamSimu3DDialog::OnMouthBcond(wxCommandEvent& event)
{
  auto res = lstMouthBcond->GetSelection();

  switch (res)
  {
  case 0:
    m_mouthBoundaryCond = RADIATION;
    break;
  case 1:
    m_mouthBoundaryCond = ADMITTANCE_1;
    break;
  case 2:
    m_mouthBoundaryCond = ZERO_PRESSURE;
    break;
  default:
    m_mouthBoundaryCond = RADIATION;
    break;
  }

  updateWidgets();
}

// ****************************************************************************
// ****************************************************************************

void ParamSimu3DDialog::OnFreqRes(wxCommandEvent& event)
{
  auto res = lstFreqRes->GetSelection();

  m_simuParams.spectrumLgthExponent = m_expSpectrumLgthStart + res;

  updateWidgets();
}

// ****************************************************************************
// ****************************************************************************

void ParamSimu3DDialog::OnSetDefaultParamsFast(wxCommandEvent& event)
{
  SetDefaultParams(true);
}

// ****************************************************************************
// ****************************************************************************

void ParamSimu3DDialog::OnSetDefaultParamsAccurate(wxCommandEvent& event)
{
  SetDefaultParams(false);
}

// ****************************************************************************
// ****************************************************************************

void ParamSimu3DDialog::SetDefaultParams(bool fast)
{
  // for fast computations, but very innacurate
  if (fast)
  {
    m_meshDensity = 5.;

    m_simuParams.maxCutOnFreq = 20000.;

    m_simuParams.maxComputedFreq = 10000.;

    // for about 40 Hz resolution
    m_simuParams.spectrumLgthExponent = m_expSpectrumLgthStart + 2;
  }
  else
  {
    m_meshDensity = 15.;

    m_simuParams.maxCutOnFreq = 40000.;

    m_simuParams.maxComputedFreq = 20000.;

    // for about 10 Hz resolution
    m_simuParams.spectrumLgthExponent = m_expSpectrumLgthStart + 4;
  }

  m_simuParams.sndSpeed = 35000.;
  m_simuParams.volumicMass = ADIABATIC_CONSTANT * STATIC_PRESSURE_CGS /
    pow(m_simuParams.sndSpeed, 2);
  m_simuParams.temperature = pow(m_simuParams.sndSpeed, 2) * MOLECULAR_MASS /
    GAS_CONSTANT / ADIABATIC_CONSTANT - KELVIN_SHIFT;

  m_simuParams.propMethod = MAGNUS;

  m_simuParams.curved = true;

  m_simuParams.varyingArea = true;

  m_mouthBoundaryCond = RADIATION;

  m_simuParams.percentageLosses = 1.;

  m_simuParams.wallLosses = false;

  m_simuParams.freqDepLosses = false;

  m_simuParams.thermalBndSpecAdm = complex<double>(0.005, 0.);

  m_secNoiseSource = 25;

  m_simuParams.tfPoint.clear();
  m_simuParams.tfPoint.push_back(Point_3(100., 0., 0.));
  chkMultiTFPts->SetValue(false);

  m_simuParams.freqField = 5000.;

  m_simuParams.fieldResolution = 30;

  m_simuParams.computeRadiatedField = false;

  updateWidgets();
}

// ****************************************************************************
// ****************************************************************************

void ParamSimu3DDialog::OnClose(wxCommandEvent& event)
{
  this->Close();
}
