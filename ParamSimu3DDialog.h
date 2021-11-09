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

    wxTextCtrl *txtMeshDensity;
    //wxTextCtrl* txtModeNumber;
    wxTextCtrl* txtMaxCutOnFreq;
    wxTextCtrl* txtMaxSimFreq;
    wxTextCtrl* txtNumIntegrationStep;
    wxTextCtrl* txtSecNoiseSource;
	wxTextCtrl* txtConstriction;
    wxTextCtrl* txtExpLgth;
    wxStaticText* txtNbFreqs;
	wxTextCtrl* txtPercLoss;

  // for acoustic field computation
  wxTextCtrl* txtFreqComputeField;
  wxTextCtrl* txtResolutionField;
  wxTextCtrl* txtBboxMinX;
  wxTextCtrl* txtBboxMinY;
  wxTextCtrl* txtBboxMaxX;
  wxTextCtrl* txtBboxMaxY;

	wxCheckBox* chkFdepLosses;
    wxCheckBox* chkWallLosses;
    wxCheckBox* chkStraight;
    wxCheckBox* chkMagnus;
	wxCheckBox* chkCurv;
	wxCheckBox* chkVarArea;
    
    Acoustic3dSimulation* simu3d;
    double m_meshDensity;
    //int m_modeNumber;
    double m_maxCutOnFreq;
    //int m_numIntegrationStep;
    int m_secNoiseSource;
	int m_secConstriction;
    int m_expSpectrumLgth;
    //int m_method;
	struct simulationParameters m_simuParams;

// **************************************************************************
// Private functions.
// **************************************************************************

private:
    ParamSimu3DDialog(wxWindow *parent, Acoustic3dSimulation* inSimu3d);
    void initWidgets();

    void OnMeshDensityEnter(wxCommandEvent& event);
    //void OnModeNumberEnter(wxCommandEvent& event);
    void OnMaxCutOnEnter(wxCommandEvent& event);
    void OnMaxSimFreq(wxCommandEvent& event);
    void OnNumIntegrationEnter(wxCommandEvent& event);
    void OnSecNoiseSourceEnter(wxCommandEvent& event);
	void OnSecConstrictionEnter(wxCommandEvent& event);
    void OnExpSpectrumLgthEnter(wxCommandEvent& event);
	void OnPercentLosses(wxCommandEvent& event);
  void OnFreqComputeField(wxCommandEvent& event);
  void OnResolutionField(wxCommandEvent& event);
  void OnBboxMinX(wxCommandEvent& event);
  void OnBboxMinY(wxCommandEvent& event);
  void OnBboxMaxX(wxCommandEvent& event);
  void OnBboxMaxY(wxCommandEvent& event);


	void OnChkFdepLosses(wxCommandEvent& event);
    void OnChkWallLosses(wxCommandEvent& event);
    void OnChkStraight(wxCommandEvent& event);
    void OnChkMagnus(wxCommandEvent& event);
	void OnChkCurv(wxCommandEvent& event);
	void OnChkVarArea(wxCommandEvent& event);

// **************************************************************************
// Declare the event table.
// **************************************************************************

    DECLARE_EVENT_TABLE()
};

#endif
