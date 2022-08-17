// ****************************************************************************
// This file is part of VocalTractLab3D.
// Copyright (C) 2022, Peter Birkholz, Dresden, Germany
// www.vocaltractlab.de
// author: Peter Birkholz and Rémi Blandin
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.
//
// ****************************************************************************

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
  void setShowTfPts(bool showTfPts) {m_showTfPts = showTfPts;}
  void setIdxTfPoint(int idx) { m_idxPtTf = idx; }
  void requestFieldInterpolation() { m_interpolateField = true; }

  // acessors
  int activeSegment() const { return m_activeSegment; }
  bool showSegements() const { return m_showSegments; }
  bool showField() const { return m_showField; }
  bool showTfPts() const { return m_showTfPts; }

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
  bool m_showTfPts;
  bool m_showSndSourceSeg;
  bool m_fieldInLogScale;
  bool m_interpolateField;
  int m_idxPtTf;

  int m_width;
  int m_height;
  int m_oldWidth;
  int m_oldHeight;
  double m_zoom;
  double m_halfWidth;
  double m_halfHeight;
  double m_bboxHalfWidth;
  double m_bboxHalfHeight;
  pair<Point2D, Point2D> m_bbox;

  wxBitmap m_fieldImage;

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
  void getSegmentPts(CrossSection2d* sec, CGAL::Bbox_2& bbox,
    Point& ptInMin, Point& ptInMax, Point& ptOutMin, Point& ptOutMax);

  void OnUpdateBbox(wxCommandEvent& event);
  void OnDefineBboxLowerCorner(wxCommandEvent& event);
  void OnDefineBboxUpperCorner(wxCommandEvent& event);
  void OnExportAcousticField(wxCommandEvent& event);
  void OnDefineNoiseSourceSeg(wxCommandEvent& event);
  void OnEXportGeoAsCsv(wxCommandEvent& event);
  void OnExportSegPic(wxCommandEvent& event);

 // ****************************************************************************
 // Declare the event table right at the end
 // ****************************************************************************

  DECLARE_EVENT_TABLE()
};
#endif
