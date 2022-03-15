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
#include "Spectrum3dPicture.h"
#include "SegmentsPicture.h"

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
  PropModesPicture *picPropModes;
  Spectrum3dPicture *picSpectrum;
  SegmentsPicture* segPic;

  // segment picture controls
  wxCheckBox* chkShowSegments;
  wxCheckBox* chkShowField;

  // propagation modes controls
  wxCheckBox* chkShowMesh;
  wxCheckBox* chkShowMode;
  wxCheckBox* chkShowTransField;
  //wxCheckBox* chkShowF;

// ****************************************************************
// Private functions.
// ****************************************************************
private:

  void initVars();
  void initWidgets(VocalTractPicture* picVocalTract);
  void OnUpdateRequest(wxCommandEvent& event);

  // Event handers for controls at the left side

  //void OnRunTestJunction(wxCommandEvent& event);
  //void OnRunTestRadImp(wxCommandEvent& event);
  void OnRunTestMatrixE(wxCommandEvent& event);
  //void OnRunTestDiscontinuity(wxCommandEvent& event);
  void OnRunTestElephant(wxCommandEvent& event);
  void OnRunStaticSimulation(wxCommandEvent& event);
  void OnParamSimuDialog(wxCommandEvent& event);
  void OnShapesDialog(wxCommandEvent& event);
  void OnImportGeometry(wxCommandEvent& event);
  void OnPlayLongVowel(wxCommandEvent& event);
  void OnPlayLongVowel();
  void OnPlayNoiseSource(wxCommandEvent& event);
  void OnComputeModes(wxCommandEvent& event);
  void OnComputeAcousticField(wxCommandEvent& event);
  void OnExportTf(wxCommandEvent& event);
  void OnExportField(wxCommandEvent& event);

  // event handlers for main panel control
  void OnShowLowerOrderMode(wxCommandEvent& event);
  void OnShowMesh(wxCommandEvent& event);
  void OnShowMode(wxCommandEvent& event);
  void OnShowTransField(wxCommandEvent& event);
  //void OnShowF(wxCommandEvent& event);
  void OnShowHigherOrderMode(wxCommandEvent& event);

  // event handlers for segments picture
  void OnShowPreviousSegment(wxCommandEvent& event);
  void OnShowNextSegment(wxCommandEvent& event);
  void OnShowSegments(wxCommandEvent& event);
  void OnShowField(wxCommandEvent& event);

  // event handlers for bottom panel
  void OnUpperSpectrumLimitPlus(wxCommandEvent& event);
  void OnUpperSpectrumLimitMinus(wxCommandEvent& event);
  void OnLowerSpectrumLimitPlus(wxCommandEvent& event);
  void OnLowerSpectrumLimitMinus(wxCommandEvent& event);
  void OnFrequencyRangeMinus(wxCommandEvent& event);
  void OnFrequencyRangePlus(wxCommandEvent& event);

  void importGeometry();

// ****************************************************************************
 // Declare the event table right at the end
 // ****************************************************************************

  DECLARE_EVENT_TABLE()
};
#endif
