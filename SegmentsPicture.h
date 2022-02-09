#ifndef __SEGMENTS_PICTURE_H__
#define __SEGMENTS_PICTURE_H__

#include "BasicPicture.h"
#include "Backend/Acoustic3dSimulation.h"

class SegmentsPicture : public BasicPicture
{
// **************************************************************************
// Public functions.
// **************************************************************************

public:

  SegmentsPicture(wxWindow* parent, Acoustic3dSimulation* simu3d, wxWindow* updateEventReceiver);
  virtual void draw(wxDC& dc);
  void showPreivousSegment();
  void showNextSegment();
  void OnMouseEvent(wxMouseEvent& event);
  void resetActiveSegment();

  // acessors
  int activeSegment() const { return m_activeSegment; }

// **************************************************************************
// Private functions.
// **************************************************************************

private:

  void getZoomAndCenter(int& width, int& height, double& centerX, double& centerY,
    double& zoom);

// **************************************************************************
// Private data.
// **************************************************************************

private:

  Acoustic3dSimulation* m_simu3d;
  wxWindow* updateEventReceiver;
  int m_activeSegment;

 // ****************************************************************************
 // Declare the event table right at the end
 // ****************************************************************************

  DECLARE_EVENT_TABLE()
};
#endif