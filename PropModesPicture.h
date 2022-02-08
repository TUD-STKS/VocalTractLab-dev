#ifndef __MESH_PICTURE_H__
#define __MESH_PICTURE_H__

#include "BasicPicture.h"
#include "VocalTractPicture.h"
#include "SegmentsPicture.h"
#include "Backend/Acoustic3dSimulation.h"


// ****************************************************************************
// ****************************************************************************

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
	void showF();
	void setModeIdx(int idx);
	bool meshSelected();
	bool modeSelected();
	bool fSelected();
	int modeIdx();

// **************************************************************************
// Public data.
// **************************************************************************

public:

// **************************************************************************
// Private data.
// **************************************************************************

private:

	//bool m_displayMesh;
	int m_objectToDisplay;
	int m_modeIdx;
	//VocalTractPicture* m_picVocalTract;
	Acoustic3dSimulation* m_simu3d;
  SegmentsPicture* m_segPic;
};

#endif

// ****************************************************************************
