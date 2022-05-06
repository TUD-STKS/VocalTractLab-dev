#ifndef __MESH_PICTURE_H__
#define __MESH_PICTURE_H__

#include "BasicPicture.h"
#include "VocalTractPicture.h"
#include "SegmentsPicture.h"
#include "Backend/Acoustic3dSimulation.h"


// ****************************************************************************
// ****************************************************************************

enum objectToDisplay {
  MESH,
  TRANSVERSE_MODE,
  JUNCTION_MATRIX,
  ACOUSTIC_FIELD,
  CONTOUR
};

class PropModesPicture : public BasicPicture
{
// **************************************************************************
// Public functions.
// **************************************************************************

public:
	PropModesPicture(wxWindow* parent,
    Acoustic3dSimulation* simu3d, SegmentsPicture* segPic);
  //PropModesPicture(wxWindow* parent, VocalTractPicture* picVocalTract,
  //  Acoustic3dSimulation* simu3d, SegmentsPicture* segPic);

	virtual void draw(wxDC& dc);

	void showMesh();
	void showMode();
  void showField();
	void showF();
	void setModeIdx(int idx);

  // accessors
  bool meshSelected() const { return(m_objectToDisplay == MESH); }
  bool modeSelected() const { return(m_objectToDisplay == TRANSVERSE_MODE); }
  bool fSelected() const { return(m_objectToDisplay == JUNCTION_MATRIX); }
  bool fieldSelected() const { return(m_objectToDisplay == ACOUSTIC_FIELD); }
  int modeIdx() const { return(m_modeIdx); }

// **************************************************************************
// Private data.
// **************************************************************************

private:

  enum objectToDisplay m_objectToDisplay;
  bool m_fieldInLogScale;
	int m_modeIdx;
  double m_zoom;
  double m_centerX;
  double m_centerY;

	Acoustic3dSimulation* m_simu3d;
  SegmentsPicture* m_segPic;

// **************************************************************************
// Private functions.
// **************************************************************************

private:

  void drawContour(int sectionIdx, vector<int>& surf, wxDC& dc);
};

#endif

// ****************************************************************************
