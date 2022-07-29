#ifndef __PARAM_SIMU3D_DIALOG__
#define __PARAM_SIMU3D_DIALOG__

#include <wx/wx.h>
#include <wx/dialog.h>
//#include "Backend/VocalTract.h"

#include "Backend/Acoustic3dSimulation.h"

class ParamSimu3DDialog : public wxDialog
{
   
// **************************************************************************
// Public functions.
// **************************************************************************

public:
    static ParamSimu3DDialog* getInstance(wxWindow *parent = NULL);
    void updateWidgets();
    void updateParams();
    void setUpdateRequestReceiver(wxWindow* receiver);

    void updateGeometry();
    void updatePictures();

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
  wxTextCtrl* txtWallAdmitReal;
  wxTextCtrl* txtWallAdmitImag;

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
  wxButton* buttonMultiTfPts;

  // check boxes
	wxCheckBox* chkViscoThermLoss;
  wxCheckBox* chkWallLosses;
  wxCheckBox* chkWallAdmittance;
  wxCheckBox* chkStraight;
  wxCheckBox* chkMagnus;
	wxCheckBox* chkCurv;
	wxCheckBox* chkVarArea;
  wxCheckBox* chkMultiTFPts;
  wxCheckBox* chkComputeRad;

  // lists
  wxComboBox* lstScalingFacMethods;
  wxComboBox* lstMouthBcond;
  wxComboBox* lstFreqRes;
    
  Acoustic3dSimulation* m_simu3d;
  VocalTract* m_tract;

  double m_meshDensity;
  int m_secNoiseSource;
  vector<string> m_listMouthBcond;
  vector<string> m_listScalingMethods;
  int m_expSpectrumLgthStart;
  int m_expSpectrumLgthEnd;
  vector<string> m_listFreqRes;
  bool m_tfPtsFromFile;
  Point_3 m_singlePtTf;
  openEndBoundaryCond m_mouthBoundaryCond;
  contourInterpolationMethod m_contInterpMeth;
	struct simulationParameters m_simuParams;
  struct simulationParameters m_simuParamsMagnus;
  struct simulationParameters m_simuParamsFreqDepLosses;
  double m_maxBbox;

// **************************************************************************
// Private functions.
// **************************************************************************

private:
  ParamSimu3DDialog(wxWindow *parent);
  void initWidgets();

  void OnTemperatureEnter(wxCommandEvent& event);
  void OnSndSpeedEnter(wxCommandEvent& event);
  void OnMeshDensityEnter(wxCommandEvent& event);
  void OnMaxCutOnEnter(wxCommandEvent& event);
  void OnMaxSimFreq(wxCommandEvent& event);
  void OnNumIntegrationEnter(wxCommandEvent& event);
  void OnSecNoiseSourceEnter(wxCommandEvent& event);
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

  void OnScalingFactMethod(wxCommandEvent& event);
  void OnMouthBcond(wxCommandEvent& event);
  void OnFreqRes(wxCommandEvent& event);

  void OnSetDefaultParamsFast(wxCommandEvent& event);
  void OnSetDefaultParamsAccurate(wxCommandEvent& event);
  void SetDefaultParams(bool fast);

  void OnClose(wxCommandEvent& event);

  // for updating some variables which imply modifying others
  void setMeshDensity(double density);
  void setMaxCutOnFreq(double freq);

// **************************************************************************
// Declare the event table.
// **************************************************************************

    DECLARE_EVENT_TABLE()
};

#endif
