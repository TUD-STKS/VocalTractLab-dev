#include "ParamSimu3DDialog.h"
#include <fstream>
#include "Backend/Constants.h"

// ****************************************************************************
// IDs.
// ****************************************************************************

static const int IDE_MESH_DENSITY       = 4000;
//static const int IDE_MODE_NUMBER        = 4001;
static const int IDE_MAX_CUT_ON         = 4002;
static const int IDE_MAX_SIM_FREQ       = 4003;
static const int IDE_NUM_INTEGRATION    = 4004;
static const int IDE_SEC_NOISE_SOURCE   = 4005;
static const int IDE_SEC_CONSTRICTION	= 4006;
static const int IDE_EXP_SPECTRUM_LGTH  = 4007;
static const int IDE_PERCENT_LOSSES		= 4008;
static const int IDE_FREQ_COMPUTE_FIELD = 4009;
static const int IDE_RESOLUTION_FIELD = 4010;
static const int IDE_BBOX_MIN_X       = 4011;
static const int IDE_BBOX_MIN_Y       = 4012;
static const int IDE_BBOX_MAX_X       = 4013;
static const int IDE_BBOX_MAX_Y       = 4014;

static const int IDB_CHK_FDEP_LOSSES	= 5001;
static const int IDB_CHK_WALL_LOSSES    = 5002;

static const int IDB_CHK_STRAIGHT       = 5003;
static const int IDB_CHK_MAGNUS			= 5004;
static const int IDB_CHK_CURV			= 5005;
static const int IDB_CHK_VAR_AREA		= 5006;

// ****************************************************************************
// The event table.
// ****************************************************************************

BEGIN_EVENT_TABLE(ParamSimu3DDialog, wxDialog)
EVT_TEXT_ENTER(IDE_MESH_DENSITY, ParamSimu3DDialog::OnMeshDensityEnter)
//EVT_TEXT_ENTER(IDE_MODE_NUMBER, ParamSimu3DDialog::OnModeNumberEnter)
EVT_TEXT_ENTER(IDE_MAX_CUT_ON, ParamSimu3DDialog::OnMaxCutOnEnter)
EVT_TEXT_ENTER(IDE_MAX_SIM_FREQ, ParamSimu3DDialog::OnMaxSimFreq)
EVT_TEXT_ENTER(IDE_NUM_INTEGRATION, ParamSimu3DDialog::OnNumIntegrationEnter)
EVT_TEXT_ENTER(IDE_SEC_NOISE_SOURCE, ParamSimu3DDialog::OnSecNoiseSourceEnter)
EVT_TEXT_ENTER(IDE_SEC_CONSTRICTION, ParamSimu3DDialog::OnSecConstrictionEnter)
EVT_TEXT_ENTER(IDE_EXP_SPECTRUM_LGTH, ParamSimu3DDialog::OnExpSpectrumLgthEnter)
EVT_TEXT_ENTER(IDE_PERCENT_LOSSES, ParamSimu3DDialog::OnPercentLosses)
EVT_TEXT_ENTER(IDE_FREQ_COMPUTE_FIELD, ParamSimu3DDialog::OnFreqComputeField)
EVT_TEXT_ENTER(IDE_RESOLUTION_FIELD, ParamSimu3DDialog::OnResolutionField)
EVT_TEXT_ENTER(IDE_BBOX_MIN_X, ParamSimu3DDialog::OnBboxMinX)
EVT_TEXT_ENTER(IDE_BBOX_MIN_Y, ParamSimu3DDialog::OnBboxMinY)
EVT_TEXT_ENTER(IDE_BBOX_MAX_X, ParamSimu3DDialog::OnBboxMaxX)
EVT_TEXT_ENTER(IDE_BBOX_MAX_Y, ParamSimu3DDialog::OnBboxMaxY)

EVT_CHECKBOX(IDB_CHK_FDEP_LOSSES, ParamSimu3DDialog::OnChkFdepLosses)
EVT_CHECKBOX(IDB_CHK_WALL_LOSSES, ParamSimu3DDialog::OnChkWallLosses)
EVT_CHECKBOX(IDB_CHK_STRAIGHT, ParamSimu3DDialog::OnChkStraight)
EVT_CHECKBOX(IDB_CHK_MAGNUS, ParamSimu3DDialog::OnChkMagnus)
EVT_CHECKBOX(IDB_CHK_CURV, ParamSimu3DDialog::OnChkCurv)
EVT_CHECKBOX(IDB_CHK_VAR_AREA, ParamSimu3DDialog::OnChkVarArea)
END_EVENT_TABLE()

// The single instance of this class.
ParamSimu3DDialog *ParamSimu3DDialog::instance = NULL;

// ****************************************************************************
/// Returns the single instance of this dialog.
// ****************************************************************************

ParamSimu3DDialog* ParamSimu3DDialog::getInstance(wxWindow *parent,
    Acoustic3dSimulation* inSimu3d)
{
    if (instance == NULL)
    {
        instance = new ParamSimu3DDialog(parent, inSimu3d);
    }

    return instance;
}

// ****************************************************************************
// ****************************************************************************

void ParamSimu3DDialog::updateWidgets()
{
  ofstream log("log.txt", ofstream::app);
  log << "Update param simu 3D dialog" << endl;

  wxString st;

  st = wxString::Format("%2.1f", m_meshDensity);
  txtMeshDensity->SetValue(st);

  //st = wxString::Format("%d", m_modeNumber);
  //txtModeNumber->SetValue(st);

  st = wxString::Format("%6.f", m_maxCutOnFreq);
  txtMaxCutOnFreq->SetValue(st);

  st = wxString::Format("%5.f", m_simuParams.maxComputedFreq);
  txtMaxSimFreq->SetValue(st);

  st = wxString::Format("%d", m_simuParams.numIntegrationStep);
  txtNumIntegrationStep->SetValue(st);

  st = wxString::Format("%d", m_secNoiseSource);
  txtSecNoiseSource->SetValue(st);

	st = wxString::Format("%d", m_secConstriction);
	txtConstriction->SetValue(st);

  st = wxString::Format("%d", m_expSpectrumLgth);
  txtExpLgth->SetValue(st);

  st = wxString::Format(" %2.1f Hz", (double)SAMPLING_RATE / 2./
      (double)(1 << (m_expSpectrumLgth - 1)));
  txtNbFreqs->SetLabel(st);

	st = wxString::Format("%1.2f", m_simuParams.percentageLosses);
	txtPercLoss->SetValue(st);

	chkFdepLosses->SetValue(m_simuParams.freqDepLosses);

  chkWallLosses->SetValue(m_simuParams.wallLosses);

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

	chkCurv->SetValue(m_simuParams.curved);

	chkVarArea->SetValue(m_simuParams.varyingArea);

  // set compute acoustic field options
  log << "freq field " << m_simuParams.freqField << endl; 

  st = wxString::Format("%1.1f", m_simuParams.freqField);
  txtFreqComputeField->SetLabel(st);

  st = wxString::Format("%d", m_simuParams.fieldResolution);
  txtResolutionField->SetLabel(st);

  st = wxString::Format("%1.1f", m_simuParams.bboxField[0].x());
  txtBboxMinX->SetLabel(st);

  st = wxString::Format("%1.1f", m_simuParams.bboxField[0].y());
  txtBboxMinY->SetLabel(st);

  st = wxString::Format("%1.1f", m_simuParams.bboxField[1].x());
  txtBboxMaxX->SetLabel(st);

  st = wxString::Format("%1.1f", m_simuParams.bboxField[1].y());
  txtBboxMaxY->SetLabel(st);

  simu3d->setSimulationParameters(m_meshDensity, m_maxCutOnFreq, m_secNoiseSource, 
		m_secConstriction, m_expSpectrumLgth, m_simuParams);
  
  log.close();
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
/// Constructor.
// ****************************************************************************

ParamSimu3DDialog::ParamSimu3DDialog(wxWindow* parent, 
    Acoustic3dSimulation *inSimu3d) :
wxDialog(parent, wxID_ANY, wxString("Parameters 3D simulations"),
    wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE)
{
  simu3d = inSimu3d;
  m_meshDensity = simu3d->meshDensity();
  //m_modeNumber = simu3d->modeNumber();
  m_maxCutOnFreq = simu3d->maxCutOnFreq();
  m_secNoiseSource = simu3d->idxSecNoiseSource();
	m_secConstriction = simu3d->idxConstriction();
  m_expSpectrumLgth = simu3d->spectrumLgthExponent();
	m_simuParams = simu3d->simuParams();

  this->Move(100, 100);

  // ****************************************************************
  // Init and update the widgets.
  // ****************************************************************

    initWidgets();
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

    // ****************************************************************
    // Set mesh density.
    // ****************************************************************

    topLevelSizer->AddSpacer(10);

    lineSizer = new wxBoxSizer(wxHORIZONTAL);

    label = new wxStaticText(this, wxID_ANY, "Mesh density: ");
    lineSizer->Add(label, 0, wxALL | wxALIGN_CENTER, 3);

    txtMeshDensity = new wxTextCtrl(this, IDE_MESH_DENSITY, "", wxDefaultPosition,
        wxSize(60, -1), wxTE_PROCESS_ENTER);
    lineSizer->Add(txtMeshDensity, 0, wxALL, 3);

    topLevelSizer->Add(lineSizer);

    // ****************************************************************
    // Set mode number.
    // ****************************************************************

   /* topLevelSizer->AddSpacer(10);

    lineSizer = new wxBoxSizer(wxHORIZONTAL);

    label = new wxStaticText(this, wxID_ANY, "Mode number: ");
    lineSizer->Add(label, 0, wxALL | wxALIGN_CENTER, 3);

    txtModeNumber = new wxTextCtrl(this, IDE_MODE_NUMBER, "", wxDefaultPosition,
        wxSize(40, -1), wxTE_PROCESS_ENTER);
    lineSizer->Add(txtModeNumber, 0, wxALL, 3);

    topLevelSizer->Add(lineSizer);*/

    // ****************************************************************
    // Set the maximal cut-on frequency
    // ****************************************************************

    topLevelSizer->AddSpacer(10);

    lineSizer = new wxBoxSizer(wxHORIZONTAL);

    label = new wxStaticText(this, wxID_ANY, "Maximal cut-on frequency: ");
    lineSizer->Add(label, 0, wxALL | wxALIGN_CENTER, 3);

    txtMaxCutOnFreq = new wxTextCtrl(this, IDE_MAX_CUT_ON, "", wxDefaultPosition,
        wxSize(80, -1), wxTE_PROCESS_ENTER);
    lineSizer->Add(txtMaxCutOnFreq, 0, wxALL, 3);

    topLevelSizer->Add(lineSizer);

    // ****************************************************************
    // Set the maximal computed frequency
    // ****************************************************************

    topLevelSizer->AddSpacer(10);

    lineSizer = new wxBoxSizer(wxHORIZONTAL);

    label = new wxStaticText(this, wxID_ANY, "Maximal simulated frequency: ");
    lineSizer->Add(label, 0, wxALL | wxALIGN_CENTER, 3);

    txtMaxSimFreq = new wxTextCtrl(this, IDE_MAX_SIM_FREQ, "", wxDefaultPosition,
        wxSize(80, -1), wxTE_PROCESS_ENTER);
    lineSizer->Add(txtMaxSimFreq, 0, wxALL, 3);

    topLevelSizer->Add(lineSizer);

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

    topLevelSizer->Add(lineSizer);

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

    topLevelSizer->Add(lineSizer);

    // ****************************************************************
    // Set the section of the constriction
    // ****************************************************************

    topLevelSizer->AddSpacer(10);

    lineSizer = new wxBoxSizer(wxHORIZONTAL);

    label = new wxStaticText(this, wxID_ANY, "Index of constriction section: ");
    lineSizer->Add(label, 0, wxALL | wxALIGN_CENTER, 3);

    txtConstriction = new  wxTextCtrl(this, IDE_SEC_CONSTRICTION, "", wxDefaultPosition,
      wxSize(40, -1), wxTE_PROCESS_ENTER);
    lineSizer->Add(txtConstriction, 0, wxALL, 3);

    topLevelSizer->Add(lineSizer);

    // ****************************************************************
    // Set the length of the spectrum
    // ****************************************************************

    topLevelSizer->AddSpacer(10);

    lineSizer = new wxBoxSizer(wxHORIZONTAL);

    label = new wxStaticText(this, wxID_ANY, "Exponent of spectrum length: ");
    lineSizer->Add(label, 0, wxALL | wxALIGN_CENTER, 3);

    txtExpLgth = new wxTextCtrl(this, IDE_EXP_SPECTRUM_LGTH, "", wxDefaultPosition,
        wxSize(40, -1), wxTE_PROCESS_ENTER);
    lineSizer->Add(txtExpLgth, 0, wxALL, 3);

    wxString st(wxString::Format(" %d freqs", 1 << (m_expSpectrumLgth - 1)));
    txtNbFreqs = new wxStaticText(this, wxID_ANY, st);
    lineSizer->Add(txtNbFreqs, 0, wxALL | wxALIGN_CENTER, 3);

    topLevelSizer->Add(lineSizer);

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

	topLevelSizer->Add(lineSizer);

	// ****************************************************************
	// Set options losses
	// ****************************************************************

	topLevelSizer->AddSpacer(10);

	lineSizer = new wxBoxSizer(wxHORIZONTAL);

	chkFdepLosses = new wxCheckBox(this, IDB_CHK_FDEP_LOSSES,
		"Frequency dependant losses");
	lineSizer->Add(chkFdepLosses, 0, wxALL, 2);

    chkWallLosses = new wxCheckBox(this, IDB_CHK_WALL_LOSSES,
        "Wall losses");
    lineSizer->Add(chkWallLosses, 0, wxALL, 2);

	topLevelSizer->Add(lineSizer);

    // ****************************************************************
    // Select the simulation type
    // ****************************************************************

    topLevelSizer->AddSpacer(10);

    lineSizer = new wxBoxSizer(wxHORIZONTAL);

    chkStraight = new wxCheckBox(this, IDB_CHK_STRAIGHT, "Straight");
    lineSizer->Add(chkStraight, 0, wxALL, 2);

	chkMagnus = new wxCheckBox(this, IDB_CHK_MAGNUS, "Magnus");
    lineSizer->Add(chkMagnus, 0, wxALL, 2);

	chkCurv = new wxCheckBox(this, IDB_CHK_CURV, "Curved");
	lineSizer->Add(chkCurv, 0, wxALL, 2);

	chkVarArea = new wxCheckBox(this, IDB_CHK_VAR_AREA, "Varying area");
	lineSizer->Add(chkVarArea, 0, wxALL, 2);

    topLevelSizer->Add(lineSizer);

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
      wxSize(50, -1), wxTE_PROCESS_ENTER);
    lineSizer->Add(txtFreqComputeField, 0, wxALL, 3);

    label = new wxStaticText(this, wxID_ANY, " Hz, resolution ");
    lineSizer->Add(label, 0, wxALL | wxALIGN_CENTER, 3);

    txtResolutionField = new wxTextCtrl(this, IDE_RESOLUTION_FIELD, "", wxDefaultPosition,
      wxSize(40, -1), wxTE_PROCESS_ENTER);
    lineSizer->Add(txtResolutionField, 0, wxALL, 3);

    label = new wxStaticText(this, wxID_ANY, " point per cm");
    lineSizer->Add(label, 0, wxALL | wxALIGN_CENTER, 3);

    topLevelSizer->Add(lineSizer);

    // ****************************************************************
    // Set the bunding box to compute the acoustic field
    // ****************************************************************

    topLevelSizer->AddSpacer(10);

    lineSizer = new wxBoxSizer(wxHORIZONTAL);

    label = new wxStaticText(this, wxID_ANY, "Min X ");
    lineSizer->Add(label, 0, wxALL | wxALIGN_CENTER, 3);

    txtBboxMinX = new wxTextCtrl(this, IDE_BBOX_MIN_X, "", wxDefaultPosition,
      wxSize(40, -1), wxTE_PROCESS_ENTER);
    lineSizer->Add(txtBboxMinX, 0, wxALL, 3);

    label = new wxStaticText(this, wxID_ANY, " min Y ");
    lineSizer->Add(label, 0, wxALL | wxALIGN_CENTER, 3);

    txtBboxMinY = new wxTextCtrl(this, IDE_BBOX_MIN_Y, "", wxDefaultPosition,
      wxSize(40, -1), wxTE_PROCESS_ENTER);
    lineSizer->Add(txtBboxMinY, 0, wxALL, 3);

    label = new wxStaticText(this, wxID_ANY, " max X ");
    lineSizer->Add(label, 0, wxALL | wxALIGN_CENTER, 3);

    txtBboxMaxX = new wxTextCtrl(this, IDE_BBOX_MAX_X, "", wxDefaultPosition,
      wxSize(40, -1), wxTE_PROCESS_ENTER);
    lineSizer->Add(txtBboxMaxX, 0, wxALL, 3);

    label = new wxStaticText(this, wxID_ANY, " max Y ");
    lineSizer->Add(label, 0, wxALL | wxALIGN_CENTER, 3);

    txtBboxMaxY = new wxTextCtrl(this, IDE_BBOX_MAX_Y, "", wxDefaultPosition,
      wxSize(40, -1), wxTE_PROCESS_ENTER);
    lineSizer->Add(txtBboxMaxY, 0, wxALL, 3);

    topLevelSizer->Add(lineSizer);

    // ****************************************************************
    // Set the top-level-sizer for this window.
    // ****************************************************************

    topLevelSizer->AddSpacer(10);

    this->SetSizer(topLevelSizer);
    topLevelSizer->Fit(this);
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

    updateWidgets();
}

// ****************************************************************************
// ****************************************************************************

//void ParamSimu3DDialog::OnModeNumberEnter(wxCommandEvent& event)
//{
//    double x = 0.0;
//    wxString st = txtModeNumber->GetValue();
//    if ((st.ToDouble(&x)) && (x > 0.) && (x <= 100.))
//    {
//        m_modeNumber = (int)x;
//    }
//
//    updateWidgets();
//}

// ****************************************************************************
// ****************************************************************************

void ParamSimu3DDialog::OnMaxCutOnEnter(wxCommandEvent& event)
{
    double x = 0.0;
    wxString st = txtMaxCutOnFreq->GetValue();
    if ((st.ToDouble(&x)) && (x >= 0.0) && (x <= 500000.0))
    {
        m_maxCutOnFreq = x;
    }

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
        m_simuParams.numIntegrationStep = (int)x;
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
}

// ****************************************************************************
// ****************************************************************************

void ParamSimu3DDialog::OnSecConstrictionEnter(wxCommandEvent& event)
{
	double x(0.);
	wxString st = txtConstriction->GetValue();
	if ((st.ToDouble(&x)) && (x >= 0.) && (x <= 500.))
	{
		m_secConstriction = (int)x;
	}

	updateWidgets();
}

// ****************************************************************************
// ****************************************************************************

void ParamSimu3DDialog::OnExpSpectrumLgthEnter(wxCommandEvent& event)
{
    double x(0.);
    wxString st = txtExpLgth->GetValue();
    if ((st.ToDouble(&x)) && (x >= 0.) && (x <= 16.))
    {
        m_expSpectrumLgth = (int)x;
    }

    updateWidgets();
}

// ****************************************************************************
// ****************************************************************************

void ParamSimu3DDialog::OnPercentLosses(wxCommandEvent& event)
{
	double x(0.);
	wxString st = txtPercLoss->GetValue();
	if ((st.ToDouble(&x)) && (x >= 0.) && (x <= 1.))
	{
		m_simuParams.percentageLosses = x;
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
  if ((st.ToDouble(&x)) && (x >= -20.) && (x <= 20.))
  {
    m_simuParams.bboxField[0] = Point(x, m_simuParams.bboxField[0].y());
  }
  updateWidgets();
}

// ****************************************************************************
// ****************************************************************************

void ParamSimu3DDialog::OnBboxMinY(wxCommandEvent& event)
{
  double x(0.);
  wxString st = txtBboxMinY->GetValue();
  if ((st.ToDouble(&x)) && (x >= -20.) && (x <= 20.))
  {
    m_simuParams.bboxField[0] = Point(m_simuParams.bboxField[0].x(), x);
  }
  updateWidgets();
}

// ****************************************************************************
// ****************************************************************************

void ParamSimu3DDialog::OnBboxMaxX(wxCommandEvent& event)
{
  double x(0.);
  wxString st = txtBboxMaxX->GetValue();
  if ((st.ToDouble(&x)) && (x >= -20.) && (x <= 20.))
  {
    m_simuParams.bboxField[1] = Point(x, m_simuParams.bboxField[1].y());
  }
  updateWidgets();
}

// ****************************************************************************
// ****************************************************************************

void ParamSimu3DDialog::OnBboxMaxY(wxCommandEvent& event)
{
  double x(0.);
  wxString st = txtBboxMaxY->GetValue();
  if ((st.ToDouble(&x)) && (x >= -20.) && (x <= 20.))
  {
    m_simuParams.bboxField[1] = Point(m_simuParams.bboxField[1].x(), x);
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
	updateWidgets();
}

// ****************************************************************************
// ****************************************************************************

void ParamSimu3DDialog::OnChkWallLosses(wxCommandEvent& event)
{
    m_simuParams.wallLosses = !m_simuParams.wallLosses;
    chkWallLosses->SetValue(m_simuParams.wallLosses);
    updateWidgets();
}

// ****************************************************************************
// ****************************************************************************

void ParamSimu3DDialog::OnChkStraight(wxCommandEvent& event)
{
    m_simuParams.propMethod = STRAIGHT_TUBES;
	m_simuParams.curved = false;
    m_simuParams.varyingArea = false;
	chkMagnus->SetValue(false);
    updateWidgets();
}

// ****************************************************************************
// ****************************************************************************

void ParamSimu3DDialog::OnChkMagnus(wxCommandEvent& event)
{
	m_simuParams.propMethod = MAGNUS;
    chkStraight->SetValue(false);
    updateWidgets();
}

// ****************************************************************************
// ****************************************************************************

void ParamSimu3DDialog::OnChkCurv(wxCommandEvent& event)
{
	m_simuParams.curved = !m_simuParams.curved;
	updateWidgets();
}

// ****************************************************************************
// ****************************************************************************

void ParamSimu3DDialog::OnChkVarArea(wxCommandEvent& event)
{
	m_simuParams.varyingArea = !m_simuParams.varyingArea;
	//chkVarArea->SetValue(m_simuParams.varyingArea);
	updateWidgets();
}
