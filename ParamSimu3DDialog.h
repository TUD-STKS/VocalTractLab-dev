#ifndef __PARAM_SIMU3D_DIALOG__
#define __PARAM_SIMU3D_DIALOG__

#include <wx/wx.h>
#include <wx/dialog.h>

#include "Backend/Acoustic3dSimulation.h"

class ParamSimu3DDialog : public wxDialog
{
   
// **************************************************************************
// Public functions.
// **************************************************************************

public:
    static ParamSimu3DDialog* getInstance(wxWindow *parent, 
        Acoustic3dSimulation* inSimu3d);
    void updateWidgets();
    void setUpdateRequestReceiver(wxWindow* receiver);

// **************************************************************************
// Private data.
// **************************************************************************

private:
  static ParamSimu3DDialog *instance;
  wxWindow* updateRequestReceiver;

  wxTextCtrl* txtTemperature;
  wxTextCtrl* txtSndSpeed;
  wxTextCtrl *txtMeshDensity;
  wxTextCtrl* txtMaxCutOnFreq;
  wxTextCtrl* txtMaxSimFreq;
  wxTextCtrl* txtNumIntegrationStep;
  wxTextCtrl* txtSecNoiseSource;
	wxTextCtrl* txtConstriction;
	wxTextCtrl* txtPercLoss;

  // for acoustic field computation
  wxTextCtrl* txtFreqComputeField;
  wxTextCtrl* txtResolutionField;
  wxTextCtrl* txtBboxMinX;
  wxTextCtrl* txtBboxMinY;
  wxTextCtrl* txtBboxMaxX;
  wxTextCtrl* txtBboxMaxY;

  // transfer function point
  wxTextCtrl* txtTfPointX;
  wxTextCtrl* txtTfPointY;
  wxTextCtrl* txtTfPointZ;

  wxTextCtrl* txtWallAdmit;

	wxCheckBox* chkFdepLosses;
  wxCheckBox* chkWallLosses;
  wxCheckBox* chkWallAdmittance;
  wxCheckBox* chkStraight;
  wxCheckBox* chkMagnus;
	wxCheckBox* chkCurv;
	wxCheckBox* chkVarArea;
  wxCheckBox* chkMultiTFPts;
  wxCheckBox* chkComputeRad;

  wxComboBox* lstMouthBcond;
  wxComboBox* lstFreqRes;
    
  Acoustic3dSimulation* simu3d;
  double m_meshDensity;
  double m_maxCutOnFreq;
  int m_secNoiseSource;
	int m_secConstriction;
  int m_expSpectrumLgth;
  vector<string> m_listMouthBcond;
  int m_expSpectrumLgthStart;
  int m_expSpectrumLgthEnd;
  vector<string> m_listFreqRes;
  openEndBoundaryCond m_mouthBoundaryCond;
	struct simulationParameters m_simuParams;

// **************************************************************************
// Private functions.
// **************************************************************************

private:
  ParamSimu3DDialog(wxWindow *parent, Acoustic3dSimulation* inSimu3d);
  void initWidgets();

  void OnTemperatureEnter(wxCommandEvent& event);
  void OnSndSpeedEnter(wxCommandEvent& event);
  void OnMeshDensityEnter(wxCommandEvent& event);
  void OnMaxCutOnEnter(wxCommandEvent& event);
  void OnMaxSimFreq(wxCommandEvent& event);
  void OnNumIntegrationEnter(wxCommandEvent& event);
  void OnSecNoiseSourceEnter(wxCommandEvent& event);
	void OnSecConstrictionEnter(wxCommandEvent& event);
  //void OnExpSpectrumLgthEnter(wxCommandEvent& event);
	void OnPercentLosses(wxCommandEvent& event);
  void OnFreqComputeField(wxCommandEvent& event);
  void OnResolutionField(wxCommandEvent& event);
  void OnBboxMinX(wxCommandEvent& event);
  void OnBboxMinY(wxCommandEvent& event);
  void OnBboxMaxX(wxCommandEvent& event);
  void OnBboxMaxY(wxCommandEvent& event);

  void OnTfPointX(wxCommandEvent& event);
  void OnTfPointY(wxCommandEvent& event);
  void OnTfPointZ(wxCommandEvent& event);
  void OnLoadTfPts(wxCommandEvent& event);

  void OnWallAdmitEnter(wxCommandEvent& event);

	void OnChkFdepLosses(wxCommandEvent& event);
  void OnChkWallLosses(wxCommandEvent& event);
  void OnChkWallAdmittance(wxCommandEvent& event);
  void OnChkStraight(wxCommandEvent& event);
  void OnChkMagnus(wxCommandEvent& event);
	void OnChkCurv(wxCommandEvent& event);
	void OnChkVarArea(wxCommandEvent& event);
  void OnChkMultiTFPts(wxCommandEvent& event);
  void OnChkComputeRad(wxCommandEvent& event);

  void OnMouthBcond(wxCommandEvent& event);
  void OnFreqRes(wxCommandEvent& event);

  void OnSetDefaultParamsFast(wxCommandEvent& event);
  void OnSetDefaultParamsAccurate(wxCommandEvent& event);
  void SetDefaultParams(bool fast);

// **************************************************************************
// Declare the event table.
// **************************************************************************

    DECLARE_EVENT_TABLE()
};

#endif
