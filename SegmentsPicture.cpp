#include "SegmentsPicture.h"
#include <iostream>

#include "Acoustic3dPage.h"

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Aff_transformation_2.h>

// Types for CGAL
typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef CGAL::Point_2<K>                                Point;
typedef CGAL::Aff_transformation_2<K>           Transformation;

// ****************************************************************************
// The event table.
// ****************************************************************************

BEGIN_EVENT_TABLE(SegmentsPicture, BasicPicture)
  EVT_MOUSE_EVENTS(SegmentsPicture::OnMouseEvent)
END_EVENT_TABLE()

// ****************************************************************************
/// Constructor. Passes the parent parameter.
// ****************************************************************************

SegmentsPicture::SegmentsPicture(wxWindow* parent, Acoustic3dSimulation* simu3d,
  wxWindow* updateEventReceiver)
  : BasicPicture(parent),
  m_activeSegment(0)
{
  this->m_simu3d = simu3d;
  this->updateEventReceiver = updateEventReceiver;
}

// ****************************************************************************
/// Draws the picture.
// ****************************************************************************

void SegmentsPicture::draw(wxDC& dc)
{
  int width, height;
  double zoom, centerX, centerY;

  //ofstream log("log.txt", ofstream::app);
  //log << "Start draw segments picture" << endl;

  // Clear the background.
  dc.SetBackground(*wxWHITE_BRUSH);
  dc.Clear();

  // check if the geometry have been imported
  if (m_simu3d->sectionNumber() == 0)
  {
    dc.SetPen(*wxBLACK_PEN);
    dc.SetBackgroundMode(wxTRANSPARENT);
    dc.SetFont(wxFont(9, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
    dc.DrawText("No geometry loaded.", 0, 0);
  }
  else
  {

    // ****************************************************************
    // Determine the zoom (pix/cm).
    // ****************************************************************

    getZoomAndCenter(width, height, centerX, centerY, zoom);

    // ****************************************************************
    // plot the bounding box
    // ****************************************************************

    int xBig, yBig, xEnd, yEnd;

    dc.SetPen(*wxBLACK_PEN);

    //// bottom line
    //xBig = (int)(zoom * bboxSagittalPlane.first.x + centerX);
    //yBig = height - (int)(zoom * bboxSagittalPlane.first.y + centerY);
    //xEnd = (int)(zoom * bboxSagittalPlane.second.x + centerX);
    //yEnd = height - (int)(zoom * bboxSagittalPlane.first.y + centerY);

    //log << "xBig " << xBig << " yBig " << yBig << " xEnd "
    //  << xEnd << " yEnd " << yEnd << endl;

    //dc.DrawLine(xBig, yBig, xEnd, yEnd);

    //// right line
    //xBig = xEnd;
    //yBig = yEnd;
    //yEnd = height - (int)(zoom * bboxSagittalPlane.second.y + centerY);

    //dc.DrawLine(xBig, yBig, xEnd, yEnd);

    //// top line
    //xBig = xEnd;
    //yBig = yEnd;
    //xEnd = (int)(zoom * bboxSagittalPlane.first.x + centerX);

    //dc.DrawLine(xBig, yBig, xEnd, yEnd);

    //// left line
    //xBig = xEnd;
    //yBig = yEnd;
    //yEnd = height - (int)(zoom * bboxSagittalPlane.first.y + centerY);

    //dc.DrawLine(xBig, yBig, xEnd, yEnd);

    // ****************************************************************
    // Plot first section
    // ****************************************************************

    auto sec = m_simu3d->crossSection(0);
    auto bbox = sec->contour().bbox();
    Point ptInMin, ptInMax, ptOutMin, ptOutMax;

    for (int i(0); i < m_simu3d->numCrossSections(); i++)
    {
      //log << "draw section " << i << endl;

      sec = m_simu3d->crossSection(i);
      bbox = sec->contour().bbox();

      Transformation translateInMin(CGAL::TRANSLATION,
        sec->scaleIn() * bbox.ymin() * sec->normalIn());
      ptInMin = translateInMin(sec->ctrLinePtIn());

      Transformation translateInMax(CGAL::TRANSLATION,
        sec->scaleIn() * bbox.ymax() * sec->normalIn());
      ptInMax = translateInMax(sec->ctrLinePtIn());

      Transformation translateOutMin(CGAL::TRANSLATION,
        sec->scaleOut() * bbox.ymin() * sec->normalOut());
      ptOutMin = translateOutMin(sec->ctrLinePtOut());

      Transformation translateOutMax(CGAL::TRANSLATION,
        sec->scaleOut() * bbox.ymax() * sec->normalOut());
      ptOutMax = translateOutMax(sec->ctrLinePtOut());

      //log << "ptInMin " << ptInMin
      //  << " ptInMax " << ptInMax
      //  << " ptOutMin " << ptOutMin
      //  << " ptOutMax " << ptOutMax << endl;

      // draw first line
      xBig = (int)(zoom * ptInMin.x() + centerX);
      yBig = height - (int)(zoom * ptInMin.y() + centerY);
      xEnd = (int)(zoom * ptInMax.x() + centerX);
      yEnd = height - (int)(zoom * ptInMax.y() + centerY);
      dc.DrawLine(xBig, yBig, xEnd, yEnd);

      // draw second line
      xBig = xEnd;
      yBig = yEnd;
      xEnd = (int)(zoom * ptOutMax.x() + centerX);
      yEnd = height - (int)(zoom * ptOutMax.y() + centerY);
      dc.DrawLine(xBig, yBig, xEnd, yEnd);

      // draw third line
      xBig = xEnd;
      yBig = yEnd;
      xEnd = (int)(zoom * ptOutMin.x() + centerX);
      yEnd = height - (int)(zoom * ptOutMin.y() + centerY);
      dc.DrawLine(xBig, yBig, xEnd, yEnd);

      // draw fourth line
      xBig = xEnd;
      yBig = yEnd;
      xEnd = (int)(zoom * ptInMin.x() + centerX);
      yEnd = height - (int)(zoom * ptInMin.y() + centerY);
      dc.DrawLine(xBig, yBig, xEnd, yEnd);
    }

    //log << "Draw active segment" << endl;

    // plot the active segment
    dc.SetPen(*wxRED_PEN);
    sec = m_simu3d->crossSection(m_activeSegment);
    bbox = sec->contour().bbox();

    Transformation translateInMin(CGAL::TRANSLATION,
      sec->scaleIn()* bbox.ymin()* sec->normalIn());
    ptInMin = translateInMin(sec->ctrLinePtIn());

    Transformation translateInMax(CGAL::TRANSLATION,
      sec->scaleIn()* bbox.ymax()* sec->normalIn());
    ptInMax = translateInMax(sec->ctrLinePtIn());

    Transformation translateOutMin(CGAL::TRANSLATION,
      sec->scaleOut()* bbox.ymin()* sec->normalOut());
    ptOutMin = translateOutMin(sec->ctrLinePtOut());

    Transformation translateOutMax(CGAL::TRANSLATION,
      sec->scaleOut()* bbox.ymax()* sec->normalOut());
    ptOutMax = translateOutMax(sec->ctrLinePtOut());

    // draw first line
    xBig = (int)(zoom * ptInMin.x() + centerX);
    yBig = height - (int)(zoom * ptInMin.y() + centerY);
    xEnd = (int)(zoom * ptInMax.x() + centerX);
    yEnd = height - (int)(zoom * ptInMax.y() + centerY);
    dc.DrawLine(xBig, yBig, xEnd, yEnd);

    // draw second line
    xBig = xEnd;
    yBig = yEnd;
    xEnd = (int)(zoom * ptOutMax.x() + centerX);
    yEnd = height - (int)(zoom * ptOutMax.y() + centerY);
    dc.DrawLine(xBig, yBig, xEnd, yEnd);

    // draw third line
    xBig = xEnd;
    yBig = yEnd;
    xEnd = (int)(zoom * ptOutMin.x() + centerX);
    yEnd = height - (int)(zoom * ptOutMin.y() + centerY);
    dc.DrawLine(xBig, yBig, xEnd, yEnd);

    // draw fourth line
    xBig = xEnd;
    yBig = yEnd;
    xEnd = (int)(zoom * ptInMin.x() + centerX);
    yEnd = height - (int)(zoom * ptInMin.y() + centerY);
    dc.DrawLine(xBig, yBig, xEnd, yEnd);
  }

  //log.close();
}

// ****************************************************************************
// Change the active segment
// ****************************************************************************

void SegmentsPicture::showPreivousSegment()
{
  m_activeSegment = max(0, m_activeSegment - 1);
  Refresh();
}

// ****************************************************************************

void SegmentsPicture::showNextSegment()
{
  m_activeSegment = min(m_simu3d->numCrossSections() - 1, m_activeSegment + 1);
  Refresh();
}

// ****************************************************************************

void SegmentsPicture::OnMouseEvent(wxMouseEvent& event)
{
  int mx, my;
  int width, height;
  double zoom, centerX, centerY;
  Point ptSelected;
  int idxSeg(-1);

  ofstream log("log.txt", ofstream::app);

  if (event.ButtonDown(wxMOUSE_BTN_LEFT) && (m_simu3d->numCrossSections() > 0))
  {
    mx = event.GetX();
    my = event.GetY();

    //log << "mx " << mx << " my " << my << endl;

    getZoomAndCenter(width, height, centerX, centerY, zoom);

    ptSelected = Point(((double)mx - centerX) / zoom,
      ((double)height - (double)my - centerY) / zoom);

    //log << "Point selected " << ptSelected << endl;

    m_simu3d->findSegmentContainingPoint(ptSelected, idxSeg);

    //log << "Segment identified " << idxSeg << endl;

    if (idxSeg >= 0)
    {
      m_activeSegment = idxSeg;
      Refresh();

      wxCommandEvent event(updateRequestEvent);
      event.SetInt(UPDATE_PICTURES);
      wxPostEvent(updateEventReceiver, event);
    }
  }

  log.close();
}

// ****************************************************************************

void SegmentsPicture::resetActiveSegment()
{
  m_activeSegment = 0;
}

// ****************************************************************************

void SegmentsPicture::getZoomAndCenter(int& width, int& height, double& centerX,
  double& centerY, double& zoom)
{
  ofstream log("log.txt", ofstream::app);
  double maxLength;
  pair<Point2D, Point2D> bboxSagittalPlane = m_simu3d->bboxSagittalPlane();

  // get the dimensions of the picture
  this->GetSize(&width, &height);

  //log << "Width " << width << " height " << height << endl;

  maxLength = 1.1 * max(bboxSagittalPlane.second.x - bboxSagittalPlane.first.x,
    bboxSagittalPlane.second.y - bboxSagittalPlane.first.y);
  zoom = (double)min(width, height) / maxLength;

  centerX = (double)width * abs(bboxSagittalPlane.first.x) /
    (bboxSagittalPlane.second.x - bboxSagittalPlane.first.x);
  centerY = (double)height * abs(bboxSagittalPlane.first.y) /
    (bboxSagittalPlane.second.y - bboxSagittalPlane.first.y);

  //log << "Zoom " << zoom << " centerX " << centerX << " centerY " << centerY << endl;

  log.close();
}