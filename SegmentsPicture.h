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
  void setShowSegments(bool showSegs) { m_showSegments = showSegs; }
  void setShowField(bool showField) { m_showField = showField; }

  // acessors
  int activeSegment() const { return m_activeSegment; }
  bool showSegements() const { return m_showSegments; }
  bool showField() const { return m_showField; }

// **************************************************************************
// Private data.
// **************************************************************************

private:

  static SegmentsPicture* m_instance;

  Acoustic3dSimulation* m_simu3d;
  wxWindow* updateEventReceiver;

  wxMenu* m_contextMenu;

  int m_activeSegment;
  bool m_showSegments;
  bool m_showField;
  bool m_showSndSourceSeg;
  bool m_fieldInLogScale;

  int m_width;
  int m_height;
  double m_zoom;
  double m_halfWidth;
  double m_halfHeight;
  double m_bboxHalfWidth;
  double m_bboxHalfHeight;
  pair<Point2D, Point2D> m_bbox;

  int m_mousePosX;
  int m_mousePosY;

// **************************************************************************
// Private functions.
// **************************************************************************

private:

  void getZoomAndBbox();
  int getPixelCoordX(double x);
  int getPixelCoordY(double y);
  double getCoordXFromPixel(int Xpix);
  double getCoordYFromPixel(int Ypix);
  void drawSegment(CrossSection2d *sec, CGAL::Bbox_2 &bbox, wxDC& dc);

  void OnUpdateBbox(wxCommandEvent& event);
  void OnDefineBboxLowerCorner(wxCommandEvent& event);
  void OnDefineBboxUpperCorner(wxCommandEvent& event);

 // ****************************************************************************
 // Declare the event table right at the end
 // ****************************************************************************

  DECLARE_EVENT_TABLE()
};
#endif