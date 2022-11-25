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

#include <iostream>

#include "SegmentsPicture.h"
#include "Acoustic3dPage.h"
#include "ColorScale.h"
#include "ParamSimu3DDialog.h"

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
static const int IDM_EXPORT_ACOUSTIC_FIELD    = 1003;
static const int IDM_DEFINE_NOISE_SRC_SEG     = 1004;
static const int IDM_EXPORT_GEO_AS_CSV        = 1005;
static const int IDM_EXPORT_SEGMENT_PIC       = 1006;

// ****************************************************************************
// The event table.
// ****************************************************************************

BEGIN_EVENT_TABLE(SegmentsPicture, BasicPicture)
EVT_MOUSE_EVENTS(SegmentsPicture::OnMouseEvent)
EVT_MENU(IDM_DEFINE_BBOX_LOWER_CORNER, SegmentsPicture::OnDefineBboxLowerCorner)
EVT_MENU(IDM_DEFINE_BBOX_UPPER_CORNER, SegmentsPicture::OnDefineBboxUpperCorner)
EVT_MENU(IDM_UPDATE_BBOX, SegmentsPicture::OnUpdateBbox)
EVT_MENU(IDM_EXPORT_ACOUSTIC_FIELD, SegmentsPicture::OnExportAcousticField)
EVT_MENU(IDM_DEFINE_NOISE_SRC_SEG, SegmentsPicture::OnDefineNoiseSourceSeg)
EVT_MENU(IDM_EXPORT_GEO_AS_CSV, SegmentsPicture::OnEXportGeoAsCsv)
EVT_MENU(IDM_EXPORT_SEGMENT_PIC, SegmentsPicture::OnExportSegPic)
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
  m_showTfPts(true),
  m_showSndSourceSeg(true),
  //m_interpolateField(true),
  m_oldWidth(0),
  m_oldHeight(0),
  m_maxAmp(0.),
  m_widthColorbar(60),
  m_oldBbox(Point2D(0., 0.), Point2D(0., 0.))
{
  this->m_simu3d = simu3d;
  this->updateEventReceiver = updateEventReceiver;

  m_contextMenu = new wxMenu();
  m_contextMenu->Append(IDM_DEFINE_BBOX_LOWER_CORNER, "Define bounding box lower corner");
  m_contextMenu->Append(IDM_DEFINE_BBOX_UPPER_CORNER, "Define bounding box upper corner");
  m_contextMenu->Append(IDM_UPDATE_BBOX, "Reset bounding box");
  m_contextMenu->Append(IDM_EXPORT_ACOUSTIC_FIELD, "Export acoustic field as text file");
  m_contextMenu->Append(IDM_DEFINE_NOISE_SRC_SEG, "Define current segment as noise source location");
  m_contextMenu->Append(IDM_EXPORT_GEO_AS_CSV, "Export geometry in a csv file");
  m_contextMenu->Append(IDM_EXPORT_SEGMENT_PIC, "Export segment picture");

  getZoomAndBbox();
}

// ****************************************************************************
/// Draws the picture.
// ****************************************************************************

void SegmentsPicture::draw(wxDC& dc)
{
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

    // recompute the field image if the bounding box have been changed
    if (m_bbox != m_oldBbox)
    {
      m_simu3d->setFieldImageComputation(true);
    }

    // recompute the field image if the size of the picture is changed
    if ((m_width != m_oldWidth) || (m_height != m_oldHeight))
    {
      m_simu3d->setFieldImageComputation(true);
    }

    if (m_showField)
    {
      if (m_simu3d->acousticFieldSize() > 0)
      {
        if (m_simu3d->computeFieldImage())
        {
          ColorMap colorMap;

          Matrix field;
          int normAmp;
          m_maxAmp = m_simu3d->maxAmpField();
          m_minAmp = m_simu3d->minAmpField();
          double diffAmp(m_maxAmp);

          // to avoid singular values when the field is displayed in dB
          double dbShift(0.5);

          if (m_simu3d->fieldIndB()) {
            m_maxAmp = 20. * log10(m_maxAmp);
            m_minAmp = max(m_maxAmp - 80, 20. * log10(m_minAmp));
            diffAmp = m_maxAmp - m_minAmp + dbShift;
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
          if (m_simu3d->fieldIndB()) { field = 20. * field.array().log10() - m_minAmp + dbShift; }

          // if the phase is displayed, ad pi so that only positive values are displayed

          if (m_simu3d->showFieldAmplitude())
          {
            colorMap = ColorScale::getColorMap(VIRIDIS);
          }
          else
          {
            colorMap = ColorScale::getColorMap(TWILIGHT);
            m_maxAmp += M_PI;
            m_minAmp += M_PI;
            field.array() += M_PI;
            diffAmp = m_maxAmp - m_minAmp;
          }

          // initialise white bitmap
          m_fieldImage = wxBitmap(m_width, m_height, 24);
          wxNativePixelData data(m_fieldImage);
          wxNativePixelData::Iterator p(data);
          for (int i(0); i < m_height; ++i)
          {
            wxNativePixelData::Iterator rowStart = p;
            for (int j(0); j < m_width; ++j, ++p)
            {
              if (field(i, j) > 0.)
              {
                normAmp = max(1, (int)(256. * field(i, j) / diffAmp));

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

          m_simu3d->setFieldImageComputation(false);
        }
        dc.DrawBitmap(m_fieldImage, 0, 0, 0);

        // ****************************************************************
        // draw the colorbar
        // ****************************************************************

        dc.SetPen(*wxBLACK_PEN);
        wxColor colorBarColor;
        yBig = m_height - 14;
        int normAmp;
        ColorMap colorMap;
        if (m_simu3d->showFieldAmplitude())
        {
          colorMap = ColorScale::getColorMap(VIRIDIS);
        }
        else
        {
          colorMap = ColorScale::getColorMap(TWILIGHT);
        }

        for (int i(15); i < m_height - 14; ++i)
        {
          normAmp = max(1, 255 * (i - 15) / (m_height - 30));

          colorBarColor = wxColor((*colorMap)[normAmp][0],
            (*colorMap)[normAmp][1],
            (*colorMap)[normAmp][2]);
          dc.SetPen(wxPen(colorBarColor, 1));

          yBig --;

          dc.DrawLine(0, yBig, 15, yBig);
        }

        // print the labels for the max and min amplitude
        double diffAmp(m_maxAmp - m_minAmp);
        double graduations[5] = { 0., 0.25, 0.5, 0.75, 1. };
        ostringstream ost;
        wxCoord w, h;

        if (m_simu3d->showFieldAmplitude())
        {
          ost << setprecision(0) << fixed;
        }
        else if (!m_simu3d->showFieldAmplitude())
        {
          ost << setprecision(1) << fixed;
        }

        for (int i(0); i < 5; i++)
        {
          ost << "-  " << m_minAmp + graduations[i] * diffAmp;
          if (m_simu3d->showFieldAmplitude())
          {
           ost << " dB";
          }
          else if (!m_simu3d->showFieldAmplitude())
          {
            ost << " rad";
          }
          dc.GetTextExtent(ost.str(), &w, &h);
          dc.DrawText(ost.str(), 13, 
            (1. - graduations[i]) * (m_height - 30) + 14 - h / 2);
          ost.str("");
        }
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

      for (int i(0); i < m_simu3d->numberOfSegments(); i++)
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

      // Plot the sound source segment
      if (m_showSndSourceSeg)
      {
        sec = m_simu3d->crossSection(m_simu3d->idxSecNoiseSource());
        bbox = sec->contour().bbox();
        // plot the segment in blue
        dc.SetPen(wxPen(wxColour(0, 90, 181, 255), 2, wxPENSTYLE_SOLID));
        drawSegment(sec, bbox, dc);
      }

      // plot the active segment
      sec = m_simu3d->crossSection(m_activeSegment);
      bbox = sec->contour().bbox();
      if (m_showSndSourceSeg && 
          (m_activeSegment == m_simu3d->idxSecNoiseSource()
            || (sec->isJunction() && 
              (abs(m_simu3d->idxSecNoiseSource() - m_activeSegment) == 1)))
          )
      {
        // plot the segment in green
        dc.SetPen(wxPen(wxColour(68, 170, 153, 255), 2, wxPENSTYLE_SOLID));
      }
      else
      {
        // plot the segment in red
        dc.SetPen(wxPen(wxColour(220, 50, 32, 255), 2, wxPENSTYLE_SOLID));
      }
      drawSegment(sec, bbox, dc);
      
      // if the noise source segment is a junction segment replot it 
      // if it is hidden behind the current segment
      sec = m_simu3d->crossSection(m_simu3d->idxSecNoiseSource());
      if (sec->isJunction() &&
          (abs(m_simu3d->idxSecNoiseSource() - m_activeSegment) == 1))
      {
        // plot the segment in green
        dc.SetPen(wxPen(wxColour(68, 170, 153, 255), 2, wxPENSTYLE_SOLID));
        drawSegment(sec, bbox, dc);
      }

      //************************************************
      // Draw transfer function points
      //************************************************

      if (m_showTfPts)
      {
        // plot the reception points of the transfer functions
        struct simulationParameters simuParams = m_simu3d->simuParams();
        wxCoord crossSize(7);
        int penWidth(2);
        // make a blue pen
        dc.SetPen(wxPen(wxColour(0, 90, 181, 255), penWidth, wxPENSTYLE_SOLID));
        Point_3 pt;
        int cnt(0);

        for (auto it : simuParams.tfPoint)
        {
          pt = m_simu3d->movePointFromExitLandmarkToGeoLandmark(it);
          xBig = getPixelCoordX(pt.x());
          yBig = getPixelCoordY(pt.z());
          if (cnt == m_idxPtTf)
          {
            // make the pen red
            dc.SetPen(wxPen(wxColour(220, 50, 32, 255), penWidth, wxPENSTYLE_SOLID));

            // draw a cross
            dc.DrawLine(xBig - crossSize, yBig, xBig + crossSize - 1, yBig);
            dc.DrawLine(xBig, yBig - crossSize, xBig, yBig + crossSize - 1);

            // reset the pen as blue
            dc.SetPen(wxPen(wxColour(0, 90, 181, 255), penWidth, wxPENSTYLE_SOLID));
            dc.DrawPoint(xBig, yBig);
          }
          else
          {
            // draw a cross
            dc.DrawLine(xBig - crossSize + 0, yBig, xBig + crossSize, yBig);
            dc.DrawLine(xBig, yBig - crossSize + 0, xBig, yBig + crossSize);
          }
          cnt++;
        }
      }
    }
  }

  // save the bounding box, width and height for comparison
  // to detect if the picture sizze have been 
  // changed when it drawn next time
  m_oldBbox = m_bbox;
  m_oldWidth = m_width;
  m_oldHeight = m_height;
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
  m_activeSegment = min(m_simu3d->numberOfSegments() - 1, m_activeSegment + 1);
  Refresh();
}

// ****************************************************************************

void SegmentsPicture::OnMouseEvent(wxMouseEvent& event)
{
  Point ptSelected;
  int idxSeg(-1);

  // left click
  if (event.ButtonDown(wxMOUSE_BTN_LEFT) && (m_simu3d->numberOfSegments() > 0))
  {
    m_mousePosX = event.GetX();
    m_mousePosY = event.GetY();

    getZoomAndBbox();

    ptSelected = Point(getCoordXFromPixel(m_mousePosX), getCoordYFromPixel(m_mousePosY));

    m_simu3d->findSegmentContainingPoint(ptSelected, idxSeg);

    if (idxSeg >= 0)
    {
      m_activeSegment = idxSeg;

      wxCommandEvent event(updateRequestEvent);
      event.SetInt(REFRESH_PICTURES);
      wxPostEvent(updateEventReceiver, event);
    }
  }

  // Right click
  if (event.ButtonDown(wxMOUSE_BTN_RIGHT) && (m_simu3d->numberOfSegments() > 0))
  {
    m_mousePosX = event.GetX();
    m_mousePosY = event.GetY();
    PopupMenu(m_contextMenu);
  }

  // Double click
  if (event.ButtonDClick() && (m_simu3d->numberOfSegments() > 0))
  {
    m_simu3d->updateBoundingBox();

    ParamSimu3DDialog* dialog = ParamSimu3DDialog::getInstance(NULL);
    dialog->updateParams();

    wxCommandEvent event(updateRequestEvent);
    event.SetInt(UPDATE_PICTURES);
    wxPostEvent(updateEventReceiver, event);
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
  m_bbox = m_simu3d->bboxSagittalPlane();

  // get the dimensions of the picture
  this->GetSize(&m_width, &m_height);

  m_halfWidth = (double)(m_width - m_widthColorbar) / 2.;
  m_halfHeight = (double)(m_height) / 2.;

  double bboxWidth(m_bbox.second.x - m_bbox.first.x);
  double bboxHeight(m_bbox.second.y - m_bbox.first.y);

  double ratioHeightWidthBbox(bboxHeight / bboxWidth);
  double ratioHeightWidth((double)m_height / (double)(m_width - m_widthColorbar));

  if (ratioHeightWidth > ratioHeightWidthBbox)
  {
    m_zoom = (double)(m_width - m_widthColorbar) / bboxWidth / 1.01;
  }
  else
  {
    m_zoom = (double)m_height / bboxHeight / 1.01;
  }

  m_bboxHalfWidth = m_zoom * (m_bbox.second.x - m_bbox.first.x) / 2.;
  m_bboxHalfHeight = m_zoom * (m_bbox.second.y - m_bbox.first.y) / 2.;
}

// ****************************************************************************

int SegmentsPicture::getPixelCoordX(double x)
{
  return((int)(m_halfWidth - m_bboxHalfWidth + m_widthColorbar
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
  return(((double)Xpix + m_bboxHalfWidth - m_halfWidth - m_widthColorbar) 
    / m_zoom + m_bbox.first.x);
}

// ****************************************************************************

double SegmentsPicture::getCoordYFromPixel(int Ypix)
{
  return((m_halfHeight + m_bboxHalfHeight - (double)Ypix) / m_zoom + m_bbox.first.y);
}

// ****************************************************************************

void SegmentsPicture::getSegmentPts(CrossSection2d* sec, CGAL::Bbox_2& bbox,
  Point& ptInMin, Point& ptInMax, Point& ptOutMin, Point& ptOutMax)
{
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
}

// ****************************************************************************

void SegmentsPicture::drawSegment(CrossSection2d *sec, CGAL::Bbox_2& bbox, wxDC& dc)
{
  Point ptInMin, ptInMax, ptOutMin, ptOutMax;
  int xBig, yBig, xEnd, yEnd;

  getSegmentPts(sec, bbox, ptInMin, ptInMax, ptOutMin, ptOutMax);

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

  ParamSimu3DDialog* dialog = ParamSimu3DDialog::getInstance(NULL);
  dialog->updateParams();

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

  ParamSimu3DDialog* dialog = ParamSimu3DDialog::getInstance(NULL);
  dialog->updateParams();

  wxCommandEvent picUpdateEvent(updateRequestEvent);
  event.SetInt(UPDATE_PICTURES);
  wxPostEvent(updateEventReceiver, picUpdateEvent);
}

// ****************************************************************************

void SegmentsPicture::OnExportAcousticField(wxCommandEvent& event)
{
  wxFileName fileName;
  wxString name = wxFileSelector("Save acoustic field", fileName.GetPath(),
    fileName.GetFullName(), ".txt", "(*.txt)|*.txt",
    wxFD_SAVE | wxFD_OVERWRITE_PROMPT, this);

  m_simu3d->exportAcousticField(name.ToStdString());
}

// ****************************************************************************

void SegmentsPicture::OnDefineNoiseSourceSeg(wxCommandEvent& event)
{
  ofstream log("log.txt", ofstream::app);
  m_simu3d->setIdxSecNoiseSource(m_activeSegment);
  log << "Segment " << m_activeSegment << " defined as active segment" << endl;
  log.close();

  wxCommandEvent picUpdateEvent(updateRequestEvent);
  event.SetInt(UPDATE_PICTURES);
  wxPostEvent(updateEventReceiver, picUpdateEvent);
}

// ****************************************************************************

void SegmentsPicture::OnEXportGeoAsCsv(wxCommandEvent& event)
{
  wxFileName fileName;
  wxString name = wxFileSelector("Save acoustic field", fileName.GetPath(),
    fileName.GetFullName(), ".csv", "(*.csv)|*.csv",
    wxFD_SAVE | wxFD_OVERWRITE_PROMPT, this);

  ofstream log("log.txt", ofstream::app);

  if (m_simu3d->exportGeoInCsv(name.ToStdString()))
  {
    log << "Geometry exported to file:\n" << name.ToStdString() << endl;
  }
  log.close();
}

// ****************************************************************************

void SegmentsPicture::OnExportSegPic(wxCommandEvent& event)
{
  wxFileName fileName;
  wxString name = wxFileSelector("Save segments picture", fileName.GetPath(),
    fileName.GetFullName(), ".txt", "(*.txt)|*.txt",
    wxFD_SAVE | wxFD_OVERWRITE_PROMPT, this);

  CrossSection2d* sec;
  CGAL::Bbox_2 bbox;
  Point ptInMin, ptInMax, ptOutMin, ptOutMax;

  ofstream of(name.ToStdString());
  ofstream log("log.txt", ofstream::app);

  if (of.is_open())
  {
    for (int i(0); i < m_simu3d->numberOfSegments(); i++)
    {
      sec = m_simu3d->crossSection(i);
      bbox = sec->contour().bbox();

      getSegmentPts(sec, bbox, ptInMin, ptInMax, ptOutMin, ptOutMax);

      // draw first line
      of << ptInMin.x() << "  " << ptInMin.y() << endl;
      of << ptInMax.x() << "  " << ptInMax.y() << endl;

      // draw second line
      of << ptOutMax.x() << "  " << ptOutMax.y() << endl;

      // draw third line
      of << ptOutMin.x() << "  " << ptOutMin.y() << endl;

      // draw fourth line
      of << ptInMin.x() << "  " << ptInMin.y() << endl;

      of << "nan  nan" << endl;
    }

    log << "Segment picture exported to file:\n" << name.ToStdString() << endl;
  }
  of.close();
  log.close();
}
