#include <iostream>

#include "SegmentsPicture.h"
#include "Acoustic3dPage.h"
#include "ColorScale.h"

#include <wx/rawbmp.h>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Aff_transformation_2.h>

// Types for CGAL
typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef CGAL::Point_2<K>                                Point;
typedef CGAL::Aff_transformation_2<K>           Transformation;

typedef Eigen::VectorXd                 Vec;

typedef int(*ColorMap)[256][3];   // for the colormap

// ****************************************************************************
// IDs.
// ****************************************************************************

static const int IDM_DEFINE_BBOX_LOWER_CORNER = 1000;
static const int IDM_DEFINE_BBOX_UPPER_CORNER = 1001;
static const int IDM_UPDATE_BBOX              = 1002;

// ****************************************************************************
// The event table.
// ****************************************************************************

BEGIN_EVENT_TABLE(SegmentsPicture, BasicPicture)
  EVT_MOUSE_EVENTS(SegmentsPicture::OnMouseEvent)
  EVT_MENU(IDM_DEFINE_BBOX_LOWER_CORNER, SegmentsPicture::OnDefineBboxLowerCorner)
  EVT_MENU(IDM_DEFINE_BBOX_UPPER_CORNER, SegmentsPicture::OnDefineBboxUpperCorner)
  EVT_MENU(IDM_UPDATE_BBOX, SegmentsPicture::OnUpdateBbox)
END_EVENT_TABLE()

// ****************************************************************************
/// Constructor. Passes the parent parameter.
// ****************************************************************************

SegmentsPicture::SegmentsPicture(wxWindow* parent, Acoustic3dSimulation* simu3d,
  wxWindow* updateEventReceiver)
  : BasicPicture(parent),
  m_activeSegment(0),
  m_showSegments(true),
  m_showField(false),
  m_showSndSourceSeg(true),
  m_fieldInLogScale(true)
{
  this->m_simu3d = simu3d;
  this->updateEventReceiver = updateEventReceiver;

  m_contextMenu = new wxMenu();
  m_contextMenu->Append(IDM_DEFINE_BBOX_LOWER_CORNER, "Define bounding box lower corner");
  m_contextMenu->Append(IDM_DEFINE_BBOX_UPPER_CORNER, "Define bounding box upper corner");
  m_contextMenu->Append(IDM_UPDATE_BBOX, "Reset bounding box");

  getZoomAndBbox();
}

// ****************************************************************************
/// Draws the picture.
// ****************************************************************************

void SegmentsPicture::draw(wxDC& dc)
{
  ofstream log("log.txt", ofstream::app);
  log << "Start draw segments picture" << endl;

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
    int xBig, yBig, xEnd, yEnd;

    // Determine the zoom (pix/cm).
    getZoomAndBbox();

    //***************************************************************
    // Plot the acoustic field
    //***************************************************************

    if (m_showField)
    {
      if (m_simu3d->acousticFieldSize() > 0)
      {
        ColorMap colorMap = ColorScale::getColorMap();

        Matrix field;
        int normAmp;
        double maxAmp(m_simu3d->maxAmpField());
        double minAmp(m_simu3d->minAmpField());
        // to avoid singular values when the field is displayed in dB
        double dbShift(0.5);
        if (m_fieldInLogScale) { 
          maxAmp = 20. * log10(maxAmp); 
          minAmp = 20. * log10(minAmp);
          maxAmp = maxAmp - minAmp + dbShift;
        }

        // generate the coordinates of the points of the pixel map
        Vec coordX(m_width), coordY(m_height);
        for (int i(0); i < m_width; i++)
        {
          coordX(i) = getCoordXFromPixel(i);
        }
        for (int i(0); i < m_height; i++)
        {
          coordY(i) = getCoordYFromPixel(i);
        }
        m_simu3d->interpolateAcousticField(coordX, coordY, field);
        if (m_fieldInLogScale) { field = 20. * field.array().log10() - minAmp + dbShift; }

        // initialise white bitmap
        wxBitmap bmp(m_width, m_height, 24);
        wxNativePixelData data(bmp);
        wxNativePixelData::Iterator p(data);
        for (int i(0); i < m_height; ++i)
        {
          wxNativePixelData::Iterator rowStart = p;
          for (int j(0); j < m_width; ++j, ++p)
          {
            if (field(i, j) > 0.)
            {
              normAmp = max(1, (int)(256. * field(i, j) / maxAmp));

              p.Red() = (*colorMap)[normAmp][0];
              p.Green() = (*colorMap)[normAmp][1];
              p.Blue() = (*colorMap)[normAmp][2];
            }
            else
            {
              p.Red() = 254;
              p.Green() = 254;
              p.Blue() = 254;
            }

          }
          p = rowStart;
          p.OffsetY(data, 1);
        }

        dc.DrawBitmap(bmp, 0, 0, 0);
      }
    }

    // ****************************************************************
    // plot the bounding box
    // ****************************************************************

    // bottom line
    xBig = getPixelCoordX(m_bbox.first.x);
    yBig = getPixelCoordY(m_bbox.first.y);
    xEnd = getPixelCoordX(m_bbox.second.x);
    yEnd = yBig;

    dc.SetPen(*wxLIGHT_GREY_PEN);
    dc.DrawLine(xBig, yBig, xEnd, yEnd);

    // right line
    xBig = xEnd;
    yEnd = getPixelCoordY(m_bbox.second.y);

    dc.DrawLine(xBig, yBig, xEnd, yEnd);

    // top line
    xEnd = getPixelCoordX(m_bbox.first.x);
    yBig = yEnd;

    dc.DrawLine(xBig, yBig, xEnd, yEnd);

    // left line
    xBig = xEnd;
    yEnd = getPixelCoordY(m_bbox.first.y);

    dc.DrawLine(xBig, yBig, xEnd, yEnd);

    // ****************************************************************
    // Draw the segments
    // ****************************************************************

    if (m_showSegments)
    {

      auto sec = m_simu3d->crossSection(0);
      auto bbox = sec->contour().bbox();
      Point ptInMin, ptInMax, ptOutMin, ptOutMax;

      for (int i(0); i < m_simu3d->numCrossSections(); i++)
      {
        sec = m_simu3d->crossSection(i);
        bbox = sec->contour().bbox();

        // draw centerline point
        ptInMin = sec->ctrLinePtIn();
        xBig = getPixelCoordX(ptInMin.x());
        yBig = getPixelCoordY(ptInMin.y());
        ptInMax = sec->ctrLinePtOut();
        xEnd = getPixelCoordX(ptInMax.x());
        yEnd = getPixelCoordY(ptInMax.y());
        dc.SetPen(*wxRED_PEN);
        dc.DrawLine(xBig, yBig, xEnd, yEnd);

        //************************************************
        // Draw the segments 
        //************************************************

        dc.SetPen(*wxGREY_PEN);
        drawSegment(sec, bbox, dc);
      }

      // plot the active segment
      sec = m_simu3d->crossSection(m_activeSegment);
      bbox = sec->contour().bbox();
      dc.SetPen(wxPen(*wxRED, 2, wxPENSTYLE_SOLID));
      drawSegment(sec, bbox, dc);

      // Plot the sound source segment
      sec = m_simu3d->crossSection(m_simu3d->idxSecNoiseSource());
      bbox = sec->contour().bbox();
      if (m_activeSegment == m_simu3d->idxSecNoiseSource())
      {
        dc.SetPen(wxPen(wxColour(255, 0, 255, 255), 2, wxPENSTYLE_SOLID));
      }
      else
      {
        dc.SetPen(wxPen(*wxBLUE, 2, wxPENSTYLE_SOLID));
      }
      drawSegment(sec, bbox, dc);

      // plot the reception points of the transfer functions
      struct simulationParameters simuParams = m_simu3d->simuParams();
      dc.SetPen(wxPen(*wxRED, 2, wxPENSTYLE_SOLID));
      Point_3 pt;
      for (auto it : simuParams.tfPoint)
      {
        pt = m_simu3d->movePointFromExitLandmarkToGeoLandmark(it);
        xBig = getPixelCoordX(pt.x());
        yBig = getPixelCoordY(pt.z());
        dc.DrawCircle(xBig, yBig, 1);
      }
    }
  }
  log.close();
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
  Point ptSelected;
  int idxSeg(-1);

  // left click
  if (event.ButtonDown(wxMOUSE_BTN_LEFT) && (m_simu3d->numCrossSections() > 0))
  {
    m_mousePosX = event.GetX();
    m_mousePosY = event.GetY();

    getZoomAndBbox();

    ptSelected = Point(getCoordXFromPixel(m_mousePosX), getCoordYFromPixel(m_mousePosY));

    m_simu3d->findSegmentContainingPoint(ptSelected, idxSeg);

    if (idxSeg >= 0)
    {
      m_activeSegment = idxSeg;
      Refresh();

      wxCommandEvent event(updateRequestEvent);
      event.SetInt(UPDATE_PICTURES);
      wxPostEvent(updateEventReceiver, event);
    }
  }

  // Right click
  if (event.ButtonDown(wxMOUSE_BTN_RIGHT) && (m_simu3d->numCrossSections() > 0))
  {
    m_mousePosX = event.GetX();
    m_mousePosY = event.GetY();
    PopupMenu(m_contextMenu);
  }
}

// ****************************************************************************

void SegmentsPicture::resetActiveSegment()
{
  m_activeSegment = 0;
}

// ****************************************************************************

void SegmentsPicture::getZoomAndBbox()
{
  //ofstream log("log.txt", ofstream::app);

  m_bbox = m_simu3d->bboxSagittalPlane();

  //log << "bbox " << m_bbox.first.x << "  "
  //  << m_bbox.second.x << "  "
  //  << m_bbox.first.y << "  "
  //  << m_bbox.second.y << endl;

  // get the dimensions of the picture
  this->GetSize(&m_width, &m_height);

  m_halfWidth = (double)(m_width) / 2.;
  m_halfHeight = (double)(m_height) / 2.;

  //log << "Width " << m_width << " height " << m_height << endl;

  double bboxWidth(m_bbox.second.x - m_bbox.first.x);
  double bboxHeight(m_bbox.second.y - m_bbox.first.y);

  //log << "bboxWidth " << bboxWidth << " bboxHeight " << bboxHeight << endl;

  if (bboxHeight > bboxWidth)
  {
    if (m_height > m_width)
    {
      m_zoom = (double)m_width / bboxHeight / 1.01;
    }
    else
    {
      m_zoom = (double)m_height / bboxHeight / 1.01;
    }
  }
  else 
  {
    if (m_height > m_width)
    {
      m_zoom = (double)m_width / bboxWidth / 1.01;
    }
    else
    {
      m_zoom = (double)m_height / bboxWidth / 1.01;
    }
  }

  //log << "Zoom " << m_zoom << endl;

  m_bboxHalfWidth = m_zoom * (m_bbox.second.x - m_bbox.first.x) / 2.;
  m_bboxHalfHeight = m_zoom * (m_bbox.second.y - m_bbox.first.y) / 2.;

  //log.close();
}

// ****************************************************************************

int SegmentsPicture::getPixelCoordX(double x)
{
  return((int)(m_halfWidth - m_bboxHalfWidth
    + m_zoom * (x - m_bbox.first.x)));
}

// ****************************************************************************

int SegmentsPicture::getPixelCoordY(double y)
{
  return((int)(m_bboxHalfHeight + m_halfHeight
    - m_zoom * (y - m_bbox.first.y)));
}

// ****************************************************************************

double SegmentsPicture::getCoordXFromPixel(int Xpix)
{
  return(((double)Xpix + m_bboxHalfWidth - m_halfWidth) / m_zoom + m_bbox.first.x);
}

// ****************************************************************************

double SegmentsPicture::getCoordYFromPixel(int Ypix)
{
  return((m_halfHeight + m_bboxHalfHeight - (double)Ypix) / m_zoom + m_bbox.first.y);
}

// ****************************************************************************

void SegmentsPicture::drawSegment(CrossSection2d *sec, CGAL::Bbox_2& bbox, wxDC& dc)
{
  Point ptInMin, ptInMax, ptOutMin, ptOutMax;
  int xBig, yBig, xEnd, yEnd;

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

  // draw first line
  xBig = getPixelCoordX(ptInMin.x());
  yBig = getPixelCoordY(ptInMin.y());
  xEnd = getPixelCoordX(ptInMax.x());
  yEnd = getPixelCoordY(ptInMax.y());
  dc.DrawLine(xBig, yBig, xEnd, yEnd);

  // draw second line
  xBig = xEnd;
  yBig = yEnd;
  xEnd = getPixelCoordX(ptOutMax.x());
  yEnd = getPixelCoordY(ptOutMax.y());
  dc.DrawLine(xBig, yBig, xEnd, yEnd);

  // draw third line
  xBig = xEnd;
  yBig = yEnd;
  xEnd = getPixelCoordX(ptOutMin.x());
  yEnd = getPixelCoordY(ptOutMin.y());
  dc.DrawLine(xBig, yBig, xEnd, yEnd);

  // draw fourth line
  xBig = xEnd;
  yBig = yEnd;
  xEnd = getPixelCoordX(ptInMin.x());
  yEnd = getPixelCoordY(ptInMin.y());
  dc.DrawLine(xBig, yBig, xEnd, yEnd);
}

// ****************************************************************************

void SegmentsPicture::OnUpdateBbox(wxCommandEvent& event)
{
  m_simu3d->updateBoundingBox();

  wxCommandEvent picUpdateEvent(updateRequestEvent);
  event.SetInt(UPDATE_PICTURES);
  wxPostEvent(updateEventReceiver, picUpdateEvent);
}

// ****************************************************************************

void SegmentsPicture::OnDefineBboxLowerCorner(wxCommandEvent& event)
{
  pair<Point2D, Point2D> newBbox(m_simu3d->bboxSagittalPlane());
  newBbox.first.x = getCoordXFromPixel(m_mousePosX);
  newBbox.first.y = getCoordYFromPixel(m_mousePosY);
  m_simu3d->setBoundingBox(newBbox);

  wxCommandEvent picUpdateEvent(updateRequestEvent);
  event.SetInt(UPDATE_PICTURES);
  wxPostEvent(updateEventReceiver, picUpdateEvent);
}

// ****************************************************************************

void SegmentsPicture::OnDefineBboxUpperCorner(wxCommandEvent& event)
{
  pair<Point2D, Point2D> newBbox(m_simu3d->bboxSagittalPlane());
  newBbox.second.x = getCoordXFromPixel(m_mousePosX);
  newBbox.second.y = getCoordYFromPixel(m_mousePosY);
  m_simu3d->setBoundingBox(newBbox);

  wxCommandEvent picUpdateEvent(updateRequestEvent);
  event.SetInt(UPDATE_PICTURES);
  wxPostEvent(updateEventReceiver, picUpdateEvent);
}