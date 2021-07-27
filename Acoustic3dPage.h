#ifndef _ACOUSTIC3D_PAGE_
#define _ACOUSTIC3D_PAGE_

#include <wx/wx.h>
#include <wx/frame.h>
#include <wx/splitter.h>
#include <wx/laywin.h>
#include <wx/progdlg.h>
#include "Data.h"
#include "Backend/Acoustic3dSimulation.h"
#include "VocalTractPicture.h"
#include "VocalTractShapesDialog.h"
#include "AreaFunctionPicture.h"
#include "CrossSectionPicture.h"
#include "PropModesPicture.h"

class Acoustic3dPage : public wxPanel
{
// ****************************************************************
// Public functions.
// ****************************************************************

public:
  Acoustic3dPage(wxWindow* parent, VocalTractPicture *picVocalTract);
  void updateWidgets();
  
// ****************************************************************
// Private data.
// ****************************************************************
private:

  Data *data;
  Acoustic3dSimulation *simu3d;
  wxSplitterWindow *splitter;
  AreaFunctionPicture *picAreaFunction;
  PropModesPicture *picPropModes;
  //wxGenericProgressDialog *progressDialog;

  // propagation modes controls

  wxCheckBox* chkShowMesh;
  wxCheckBox* chkShowMode;
  //wxCheckBox* chkShowF;

// ****************************************************************
// Private functions.
// ****************************************************************
private:

  void initVars();
  void initWidgets(VocalTractPicture* picVocalTract);
  void OnUpdateRequest(wxCommandEvent& event);

  // Event handers for controls at the left side

  //void OnRunTest(wxCommandEvent& event);
  void OnRunTestJunction(wxCommandEvent& event);
  void OnRunTestRadImp(wxCommandEvent& event);
  void OnRunTestMatrixE(wxCommandEvent& event);
  void OnRunTestDiscontinuity(wxCommandEvent& event);
  void OnRunTestElephant(wxCommandEvent& event);
  void OnRunStaticSimulation(wxCommandEvent& event);
  void OnParamSimuDialog(wxCommandEvent& event);
  void OnShapesDialog(wxCommandEvent& event);
  void OnImportGeometry(wxCommandEvent& event);
  void OnPlayLongVowel(wxCommandEvent& event);
  void OnPlayLongVowel();
  void OnPlayNoiseSource(wxCommandEvent& event);
  void OnComputeModes(wxCommandEvent& event);

  // event handlers for main panel control

  void OnShowLowerOrderMode(wxCommandEvent& event);
  void OnShowMesh(wxCommandEvent& event);
  void OnShowMode(wxCommandEvent& event);
  //void OnShowF(wxCommandEvent& event);
  void OnShowHigherOrderMode(wxCommandEvent& event);

// ****************************************************************************
 // Declare the event table right at the end
 // ****************************************************************************

  DECLARE_EVENT_TABLE()

};
#endif
