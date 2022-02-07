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

  SegmentsPicture(wxWindow* parent, Acoustic3dSimulation* simu3d);
  virtual void draw(wxDC& dc);
  void showPreivousSegment();
  void showNextSegment();

  // acessors
  int activeSegment() const { return m_activeSegment; }

// **************************************************************************
// Private data.
// **************************************************************************

private:

  Acoustic3dSimulation* m_simu3d;
  int m_activeSegment;
};
#endif