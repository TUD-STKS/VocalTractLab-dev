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

#include "VocalTractPicture.h"
#include "Data.h"

#include <cmath>
#include <fstream>
#include <string>
#include <wx/image.h>
#include <wx/tipwin.h>


#ifdef WIN32
#include <windows.h>
#else
typedef unsigned int COLORREF;
#define RGB(r, g, b)  ((COLORREF)((unsigned char)(b) | ((unsigned char)(g) << 8)) | ((unsigned char)(r) << 16))
#endif


using namespace std;

const double VocalTractPicture::DEFAULT_DISTANCE = 17.0;
const double VocalTractPicture::DEFAULT_X_TRANSLATION = -1.5;
const double VocalTractPicture::DEFAULT_Y_TRANSLATION = 2.5;

// ****************************************************************************
// The event table.
// ****************************************************************************

BEGIN_EVENT_TABLE(VocalTractPicture, wxGLCanvas)
    EVT_PAINT (VocalTractPicture::OnPaint)
    EVT_ERASE_BACKGROUND (VocalTractPicture::OnEraseBackground)
    EVT_SIZE(VocalTractPicture::OnResize)
    EVT_MOUSE_EVENTS(VocalTractPicture::OnMouseChanged)
END_EVENT_TABLE()


// ****************************************************************************
/// Constructor.
/// \param args Zero-terminated array of ints which should be 
/// { WX_GL_RGBA, WX_GL_DOUBLEBUFFER, WX_GL_DEPTH_SIZE, 32, 0 }
// ****************************************************************************

VocalTractPicture::VocalTractPicture(wxWindow *parent, VocalTract *vocalTract,
  wxGLAttributes openGlArgs, wxWindow *updateEventReceiver) :
  wxGLCanvas(parent, openGlArgs, wxID_ANY, wxDefaultPosition, 
  wxDefaultSize, wxFULL_REPAINT_ON_RESIZE, "GLCanvas", wxNullPalette)
{
  vtContext = new wxGLContext(this, NULL, NULL);
  this->SetCurrent(*vtContext);

  this->tract = vocalTract;
  this->updateEventReceiver = updateEventReceiver;

  if (tract == NULL)
  {
    wxMessageBox("The class VocalTractPicture was initialized with NULL-arguments", "Fatal error!");
  }

  // ****************************************************************
  // Init some image handlers so that corresponding files can be
  // loaded by wxWidgets.
  // ****************************************************************

  wxImage::AddHandler(new wxPNGHandler);
  wxImage::AddHandler(new wxGIFHandler);

  // ****************************************************************
  // Init the variables.
  // ****************************************************************

  // Position and orientation of the vocal tract

  distance_cm = DEFAULT_DISTANCE;
  yRotation_deg = 0.0;
  zRotation_deg = 0.0;
  cutPlanePos_cm = 10.4; //5.43;

  // Poster data

  poster = NULL;           // RGB-color values
  posterWidth_pix = 0;
  posterHeight_pix = 0;
  posterCenterX_cm = 0.0;
  posterCenterY_cm = 0.0;
  posterScalingFactor = 1.0;
  showPalatogramDivision = false;
  showPoster = true;
  posterEditing = false;
  showControlPoints = true;
  showCenterLine = false;
  isRoughCenterLine = false;
  showCutVectors = false;
  showTongueCrossSections = false;
  crossSectionWithTongue = true;
  renderBothSides = true;
  renderMode = RM_3DSOLID;
  selectedControlPoint = -1;
  showEmaPoints = false;

}


// ****************************************************************************
// ****************************************************************************

VocalTractPicture::~VocalTractPicture()
{
}


// ****************************************************************************
/// Set the OpenGl view in 2D-Projection Mode.
// ****************************************************************************

void VocalTractPicture::projection2D()
{
  int width = this->GetSize().x;
  int height = this->GetSize().y;
#ifdef __linux__
  // On Linux, all pixel coordinates need to be scaled by the current
  // display scaling factor before passing them to OpenGL
  width *= GetContentScaleFactor();
  height *= GetContentScaleFactor();
#endif

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0.0, width, 0.0, height);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  glViewport(0, 0, width, height);
}


// ****************************************************************************
/// Set the OpenGl view in 3D-Projection Mode.
// ****************************************************************************

void VocalTractPicture::projection3D()
{
  int width = this->GetSize().x;
  int height = this->GetSize().y;

#ifdef __linux__
  // On Linux, all pixel coordinates need to be scaled by the current
  // display scaling factor before passing them to OpenGL
  width *= GetContentScaleFactor();
  height *= GetContentScaleFactor();
#endif
	
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(40.0, (double)width/(double)height, 5.0, 100.0);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  glViewport(0, 0, width, height);
}


// ****************************************************************************
// ****************************************************************************

void VocalTractPicture::drawTestImage()
{
  // Important: Make sure that the window is shown !!
  if(this->IsShown() == false) 
  {
    return;
  }

  wxGLCanvas::SetCurrent(*vtContext);

  // ****************************************************************

  projection2D();

  int w = this->GetSize().x;
  int h = this->GetSize().y;
#ifdef __linux__
  // On Linux, all pixel coordinates need to be scaled by the current
  // display scaling factor before passing them to OpenGL
  w *= GetContentScaleFactor();
  h *= GetContentScaleFactor();
#endif

  // White background
  glColor4f(1, 1, 1, 1);
  glBegin(GL_QUADS);
  glVertex3f(0,0,0);
  glVertex3f(w,0,0);
  glVertex3f(w,h,0);
  glVertex3f(0,h,0);
  glEnd();

  // Red square
  glColor4f(1, 0, 0, 1);
  glBegin(GL_QUADS);
  glVertex3f(w/8, h/3, 0);
  glVertex3f(w*3/8, h/3, 0);
  glVertex3f(w*3/8, h*2/3, 0);
  glVertex3f(w/8, h*2/3, 0);
  glEnd();

  glFlush();
  SwapBuffers();
}


// ****************************************************************************
/// Render the vocal tract with the selected display options.
// ****************************************************************************

void VocalTractPicture::display()
{
  const double INVALID = VocalTract::INVALID_PROFILE_SAMPLE;
  int i;

  // Important: Make sure that the window is shown !!
  if(this->IsShown() == false) 
  {
    return;
  }

  wxGLCanvas::SetCurrent(*vtContext);

  // Clear the background *******************************************

  if (renderMode == RM_3DSOLID) 
  { 
    glClearColor(0.4f, 0.4f, 1.0f, 0);    // Das Original
//    glClearColor(0.9f, 0.9f, 0.9f, 0);      // Light-gray background
  }
  else
  if (renderMode == RM_3DWIRE) 
  {
    glClearColor(0.0f, 0.0f, 0.0f, 0);    // Das Original
  }
  else
  {
    glClearColor(1.0f, 1.0f, 1.0f, 0);
  }
	
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // ****************************************************************
  // Get 2D display area.
  // ****************************************************************

  double left_cm, right_cm, bottom_cm, top_cm;
  get2DRegion(left_cm, right_cm, bottom_cm, top_cm);

  // ****************************************************************
  // Possibly draw the background poster.
  // ****************************************************************

  if ((showPoster) && (poster != NULL))
  {
    int pictureWidth, pictureHeight;
    this->GetSize(&pictureWidth, &pictureHeight);
#ifdef __linux__
    // On Linux, all pixel coordinates need to be scaled by the current
    // display scaling factor before passing them to OpenGL
    pictureWidth *= GetContentScaleFactor();
    pictureHeight *= GetContentScaleFactor();
#endif

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0, (GLfloat)pictureWidth, 0.0, (GLfloat)pictureHeight);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    double zoom = (double)pictureWidth*posterScalingFactor / (double)posterWidth_pix;
    glPixelZoom((GLfloat)zoom, (GLfloat)(-zoom));

    // The poster section to draw equals the whole poster first.
    int subimageX = 0;
    int subimageY = 0;
    int subimageWidth = posterWidth_pix;
    int subimageHeight = posterHeight_pix;

    int screenPosterWidth_pix = (int)(zoom*posterWidth_pix);
    int screenPosterHeight_pix = (int)(zoom*posterHeight_pix);
    
    int centerX_pix = (posterCenterX_cm - left_cm)*pictureWidth / (right_cm - left_cm);
    int centerY_pix = (posterCenterY_cm - bottom_cm)*pictureHeight / (top_cm - bottom_cm);
    int screenPosterX_pix = centerX_pix - screenPosterWidth_pix/2;
    int screenPosterY_pix = centerY_pix + screenPosterHeight_pix/2;

    if (screenPosterX_pix < 0)
    {
      subimageX = (int)(-(double)screenPosterX_pix/zoom);
      subimageWidth = posterWidth_pix - subimageX;
      screenPosterX_pix = 0;
    }

    if (screenPosterY_pix >= pictureHeight)
    {
      subimageY = (int)((double)(screenPosterY_pix - pictureHeight + 1) / zoom);
      subimageHeight = posterHeight_pix - subimageY;
      screenPosterY_pix = pictureHeight;
    }

    glDisable(GL_LIGHTING);
    glDepthMask(GL_FALSE);   // no writing into the z-buffer

    glPixelStorei(GL_UNPACK_ROW_LENGTH,  posterWidth_pix);
    glPixelStorei(GL_UNPACK_SKIP_PIXELS, subimageX);
    glPixelStorei(GL_UNPACK_SKIP_ROWS,   subimageY);

    // Set the raster position and write the data into the graphics memory.

    glRasterPos2i(screenPosterX_pix, screenPosterY_pix);
    glDrawPixels(subimageWidth, subimageHeight, GL_RGBA, GL_UNSIGNED_BYTE, (GLvoid*)poster);

    glDepthMask(GL_TRUE);    // z-buffer is read and write
  }


  glDepthFunc(GL_LEQUAL);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);

  // ****************************************************************
  // Set transformation matrix, perspective and light.
  // ****************************************************************

  setLights();    // Muss vor der Berechnung der ModelView-Matrix aufgerufen werden

  if ((renderMode == RM_2D) || (renderMode == RM_NONE))
  {
    setProjectionMatrix2D(left_cm, right_cm, bottom_cm, top_cm);
    setModelViewMatrix2D();
  }
  else
  {
    setProjectionMatrix3D(40.0, 5.0, 100.0);
    setModelViewMatrix3D();
  }

  // ****************************************************************
  // Paint the coordinate axes.
  // ****************************************************************

  if (orthogonalProjection)
  {
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glEnable(GL_LINE_SMOOTH);
    glLineWidth(1.8f);
    glColor3f(0.0f, 0.0f, 0.0f);

    glBegin(GL_LINES);
   
    glVertex2d((int)left_cm, (int)bottom_cm);
    glVertex2d((int)right_cm+1, (int)bottom_cm);

    glVertex2d((int)left_cm, (int)top_cm+1);
    glVertex2d((int)left_cm, (int)bottom_cm);

    for (i=(int)left_cm; i <= (int)right_cm+1; i++)
    {
      glVertex2d(i, (int)bottom_cm+0.25);
      glVertex2d(i, (int)bottom_cm);
    }

    for (i=(int)bottom_cm; i <= (int)top_cm+1; i++)
    {
      glVertex2d((int)left_cm, i);
      glVertex2d((int)left_cm+0.25, i);
    }

    glEnd();
  }

  // ****************************************************************
  // Do the actual rendering.
  // ****************************************************************

  if (renderMode == RM_3DSOLID) 
  { 
    renderSolid(); 
  } 
  else
  if (renderMode == RM_3DWIRE) 
  {
    renderWireFrame();
  }
  else
  if (renderMode == RM_2D)
  {
    render2D();
  }

  // ****************************************************************
  // Draw the center line.
  // ****************************************************************

  if ((showCenterLine) && (renderMode != RM_NONE))
  {
    Tube::Articulator articulator;

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);

    glEnable(GL_LINE_SMOOTH);
    glLineWidth(1.8f);
    glColor3f(0.9f, 0.9f, 0.9f);

    if (isRoughCenterLine)
    {
      glBegin(GL_LINES);
      for (i = 0; i < VocalTract::NUM_CENTERLINE_POINTS - 1; i++)
      {
        glVertex3d(tract->roughCenterLine[i].point.x, tract->roughCenterLine[i].point.y, 0.0);
        glVertex3d(tract->roughCenterLine[i + 1].point.x, tract->roughCenterLine[i + 1].point.y, 0.0);
      }
      glEnd();
    }
    else
    {
      glBegin(GL_LINES);
      for (i = 0; i < VocalTract::NUM_CENTERLINE_POINTS - 1; i++)
      {
        glVertex3d(tract->centerLine[i].point.x, tract->centerLine[i].point.y, 0.0);
        glVertex3d(tract->centerLine[i+1].point.x, tract->centerLine[i+1].point.y, 0.0);
      }
      glEnd();
    }

    // Die aktuelle Schnittfläche einzeichnen ***********************

    glDisable(GL_DEPTH_TEST);

    Point2D P, v;
    double upperProfile[VocalTract::NUM_PROFILE_SAMPLES];
    double lowerProfile[VocalTract::NUM_PROFILE_SAMPLES];

    if (showTongueCrossSections)
    {
      i = (int)(cutPlanePos_cm*2.0);
      if (i < 0) { i = 0; }
      if (i >= VocalTract::NUM_TONGUE_RIBS) { i = VocalTract::NUM_TONGUE_RIBS-1; }
      P = tract->tongueRib[i].point;
      v = tract->tongueRib[i].normal;
    }
    else
    {
      tract->getCutVector(cutPlanePos_cm, P, v);
    }
    tract->getCrossProfiles(P, v, upperProfile, lowerProfile, crossSectionWithTongue, articulator);
    
    glBegin(GL_LINES);

	  // Oberes und unteres Profil ************************************

    if (renderMode == RM_2D)
    {
      for (i=0; i < VocalTract::NUM_PROFILE_SAMPLES-1; i++)
      {
        if ((upperProfile[i] != INVALID) && (upperProfile[i+1] != INVALID))
        {
          glVertex3d(P.x+v.x*upperProfile[i],   P.y+v.y*upperProfile[i],   0.0);
          glVertex3d(P.x+v.x*upperProfile[i+1], P.y+v.y*upperProfile[i+1], 0.0);
        }

        if ((lowerProfile[i] != INVALID) && (lowerProfile[i+1] != INVALID))
        {
          glVertex3d(P.x+v.x*lowerProfile[i],   P.y+v.y*lowerProfile[i],   0.0);
          glVertex3d(P.x+v.x*lowerProfile[i+1], P.y+v.y*lowerProfile[i+1], 0.0);
        }
      }
    }
    else
    {
      double x0, x1, xMin, yMin;
      yMin = -0.5*VocalTract::PROFILE_LENGTH;
      xMin = -0.5*VocalTract::PROFILE_LENGTH;

      for (i=0; i < VocalTract::NUM_PROFILE_SAMPLES-1; i++)
      {
        if ((upperProfile[i] != INVALID) && (upperProfile[i+1] != INVALID))
        {
          glColor3f(1.0f, 0.0f, 0.0f);

          x0 = xMin + i*VocalTract::PROFILE_SAMPLE_LENGTH;
          x1 = xMin + (i+1)*VocalTract::PROFILE_SAMPLE_LENGTH;
          glVertex3d(P.x+v.x*upperProfile[i],   P.y+v.y*upperProfile[i],   x0);
          glVertex3d(P.x+v.x*upperProfile[i+1], P.y+v.y*upperProfile[i+1], x1);
        }

        if ((lowerProfile[i] != INVALID) && (lowerProfile[i+1] != INVALID))
        {
          glColor3f(0.0f, 1.0f, 0.0f); 

          x0 = xMin + i*VocalTract::PROFILE_SAMPLE_LENGTH;
          x1 = xMin + (i+1)*VocalTract::PROFILE_SAMPLE_LENGTH;
          glVertex3d(P.x+v.x*lowerProfile[i],   P.y+v.y*lowerProfile[i],   x0);
          glVertex3d(P.x+v.x*lowerProfile[i+1], P.y+v.y*lowerProfile[i+1], x1);
        }
      }
    }

    glEnd();
  }

  // ****************************************************************
  // Draw the normal vectors on the center line.
  // ****************************************************************

  if ((showCutVectors) && (renderMode != RM_NONE))
  {
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);

    glEnable(GL_LINE_SMOOTH);
    glLineWidth(1.8f);
    glColor3f(0.8f, 0.8f, 0.8f);

    // Die Normalenvektoren auf der Mittellinie einzeichnen *********
    Point2D P0, P1;
    VocalTract::CenterLinePoint *cl = NULL;

    if (isRoughCenterLine) { cl = &tract->roughCenterLine[0]; } else { cl = &tract->centerLine[0]; }

    
    glBegin(GL_LINES);
      for (i=0; i < VocalTract::NUM_CENTERLINE_POINTS; i++)
      {
        P0 = cl[i].point + cl[i].min*cl[i].normal;
        P1 = cl[i].point + cl[i].max*cl[i].normal;

        glVertex3d(P0.x, P0.y, 0.0);
        glVertex3d(P1.x, P1.y, 0.0);
      }
    glEnd();
  }

  // ****************************************************************
  // Draw the control points.
  // ****************************************************************

  if ((showControlPoints) && (renderMode != RM_NONE))
  {
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glEnable(GL_POINT_SMOOTH);
    
    glPointSize(8.0 * GetContentScaleFactor());

    parameterToControlPoint(CP_TONGUE_CENTER);
    parameterToControlPoint(CP_TONGUE_TIP);

    Point2D setTongueBody(controlPoint[CP_TONGUE_CENTER].x, controlPoint[CP_TONGUE_CENTER].y);
    Point2D setTongueTip(controlPoint[CP_TONGUE_TIP].x, controlPoint[CP_TONGUE_TIP].y);
    Point2D limitedTongueBody(tract->param[VocalTract::TCX].limitedX, tract->param[VocalTract::TCY].limitedX);
    Point2D limitedTongueTip(tract->param[VocalTract::TTX].limitedX, tract->param[VocalTract::TTY].limitedX);

    // A dotted line connects the limited and set positions.

    glColor3f(1.0, 0.8f, 0.0);
    glLineStipple(1, 0x00FF);
    glEnable(GL_LINE_STIPPLE);
    glBegin(GL_LINES);
      glVertex3d(limitedTongueBody.x, limitedTongueBody.y, 0.0);
      glVertex3d(setTongueBody.x, setTongueBody.y, 0.0);
      glVertex3d(limitedTongueTip.x, limitedTongueTip.y, 0.0);
      glVertex3d(setTongueTip.x, setTongueTip.y, 0.0);
    glEnd();
    glDisable(GL_LINE_STIPPLE);

    // For the tongue tip and the tongue body, draw the control
    // point for the LIMITED positions.

    glColor3f(1.0, 0.2f, 0.0);
    glBegin(GL_POINTS);
      glVertex3d(limitedTongueBody.x, limitedTongueBody.y, 0.0);
      glVertex3d(limitedTongueTip.x, limitedTongueTip.y, 0.0);
    glEnd();

    // Draw all control points at their SET positions.

    glColor3f(1.0, 0.8f, 0.0);

    bool ok;

    for (i=0; i < NUM_CONTROLPOINTS; i++)
    {
      parameterToControlPoint(i);

      ok = true;
      if ((i == CP_CUT_PLANE) && (showCenterLine == false)) 
      { 
        ok = false; 
      }
      if ((i == CP_TONGUE_BACK) && (tract->anatomy.automaticTongueRootCalc))
      {
        ok = false;
      }

      if (ok)
      {
        glBegin(GL_POINTS);
          glVertex3d(controlPoint[i].x, controlPoint[i].y, controlPoint[i].z);
        glEnd();
      }
    }
  }

  // ****************************************************************
  // Draw the EMA points.
  // ****************************************************************

  if ((showEmaPoints) && (renderMode != RM_NONE))
  {
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glEnable(GL_POINT_SMOOTH);
    
    glPointSize(8.0);
    glColor3f(1.0f, 0.2f, 0.0f);

    Point3D P;

    for (i=0; i < (int)tract->emaPoints.size(); i++)
    {
      P = tract->getEmaPointCoord(i);

      glBegin(GL_POINTS);
        glVertex3d(P.x, P.y, P.z);
      glEnd();
    }
  }

  // ****************************************************************
  // Paint the normal vectors of the surfaces.
  // ****************************************************************

  if (false)
  {
    const int N = 7;
    Point3D P0, P1, n;
    Surface *s[7] = 
    { &tract->surface[VocalTract::UPPER_COVER], &tract->surface[VocalTract::LOWER_COVER],
      &tract->surface[VocalTract::UPPER_TEETH], &tract->surface[VocalTract::LOWER_TEETH],
      &tract->surface[VocalTract::UPPER_LIP],   &tract->surface[VocalTract::LOWER_LIP],
      &tract->surface[VocalTract::TONGUE] };

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);

    glEnable(GL_LINE_SMOOTH);
    glLineWidth(1.8f);
    glColor3f(0.0f, 0.0f, 1.0f);

    for (i=0; i < N; i++)
    {
      P0 = s[i]->getVertex(s[i]->numRibs/2, s[i]->numRibPoints/2);
      n = s[i]->getNormal(s[i]->numRibs/2, s[i]->numRibPoints/2);
      P1 = P0 + 0.5*n;

      glBegin(GL_LINES);
      glVertex3d(P0.x, P0.y, P0.z);
      glVertex3d(P1.x, P1.y, P1.z);
      glEnd();
    }
  }

  // ****************************************************************
  // Finish the picture.
  // ****************************************************************

  glFlush();
  SwapBuffers();

}


// ****************************************************************************
// Gibt zu den übergebenen Bildschirmkoordinaten die Objektkoordinaten zurück, 
// die auf der übergebenen Ebene liegen.
// ****************************************************************************

Point3D VocalTractPicture::getObjectCoordinates(int screenX, int screenY, 
                      const Point3D planePoint, const Point3D planeNormal)
{
  // Den Vektor bestimmen, der vom Auge auf den Pixel bei 
  // (screenX, screenY) zeigt

  double t;
  Point3D R;                  // Der Ergebnispunkt 
  Point3D P(0.0, 0.0, 0.0);   // Projektionszentrum (Auge)
  Point3D Q;
  Point3D v;

  // Q ist der Punkt auf der dichten Clippingebene des View-Frustums

  t = (double)(screenX - viewportX) / (double)viewportWidth;
  Q.x = frustumLeft + t*(frustumRight-frustumLeft);
 
  screenY = (int)viewportHeight - 1 - screenY;   // y-Wert "auf den Kopf stellen"
  t = (double)(screenY - viewportY) / (double)viewportHeight;
  Q.y = frustumBottom + t*(frustumTop-frustumBottom);

  Q.z = -frustumNear;

  // Bei der Parallelprojektion (2D) ist P.x=Q.x und P.y=Q.y
  if (orthogonalProjection) { P.set(Q.x, Q.y, 0.0); }

  // Die Punkte P und Q rückwärts transformieren

  double x, y, z, w;
  double *m = inverseModelViewMatrix;

  x = m[0]*P.x + m[4]*P.y + m[8]*P.z  + m[12];
  y = m[1]*P.x + m[5]*P.y + m[9]*P.z  + m[13];
  z = m[2]*P.x + m[6]*P.y + m[10]*P.z + m[14];
  w = m[3]*P.x + m[7]*P.y + m[11]*P.z + m[15];

  P.x = x/w;
  P.y = y/w;
  P.z = z/w;

  x = m[0]*Q.x + m[4]*Q.y + m[8]*Q.z  + m[12];
  y = m[1]*Q.x + m[5]*Q.y + m[9]*Q.z  + m[13];
  z = m[2]*Q.x + m[6]*Q.y + m[10]*Q.z + m[14];
  w = m[3]*Q.x + m[7]*Q.y + m[11]*Q.z + m[15];

  Q.x = x/w;
  Q.y = y/w;
  Q.z = z/w;

  // v ist der Vektor von P nach Q
  v = Q - P;

  // Schnittpunkt zwischen dem Strahl und der Ebene bilden

  double denominator = scalarProduct(v, planeNormal);
  if (denominator == 0.0) { denominator = 0.000001; }
  Point3D temp = planePoint;
  t = scalarProduct(temp - P, planeNormal) / denominator;

  // Rückgabepunkt

  R = P + v*t;
  return R;
}


// ****************************************************************************
// Gibt den Punkt H auf dem Linienzug points[0 .. numPoints-1] zurück, der am  
// dichtesten an dem Sichtstrahl durch (screenX, screenY) liegt.
// ****************************************************************************

Point3D VocalTractPicture::getMinDistancePoint(int screenX, int screenY, Point3D *point, int numPoints, int& bestIndex, double& bestT)
{
  // Den Vektor bestimmen, der vom Auge auf den Pixel bei (screenX, screenY) zeigt

  double s, t;
  Point3D P(0.0, 0.0, 0.0);  // Projektionszentrum (Auge)
  Point3D Q;
  Point3D v;

  // Q ist der Punkt auf der dichten Clippingebene des View-Frustums

  t = (double)(screenX - viewportX) / (double)viewportWidth;
  Q.x = frustumLeft + t*(frustumRight-frustumLeft);
 
  screenY = (int)viewportHeight - 1 - screenY;   // y-Wert "auf den Kopf stellen"
  t = (double)(screenY - viewportY) / (double)viewportHeight;
  Q.y = frustumBottom + t*(frustumTop-frustumBottom);

  Q.z = -frustumNear;

  // Bei der Parallelprojektion (2D) ist P.x=Q.x und P.y=Q.y
  if (orthogonalProjection) { P.set(Q.x, Q.y, 0.0); }

  // Die Punkte P und Q rückwärts transformieren

  double x, y, z, a;
  double *m = inverseModelViewMatrix;

  x = m[0]*P.x + m[4]*P.y + m[8]*P.z  + m[12];
  y = m[1]*P.x + m[5]*P.y + m[9]*P.z  + m[13];
  z = m[2]*P.x + m[6]*P.y + m[10]*P.z + m[14];
  a = m[3]*P.x + m[7]*P.y + m[11]*P.z + m[15];

  P.x = x/a;
  P.y = y/a;
  P.z = z/a;

  x = m[0]*Q.x + m[4]*Q.y + m[8]*Q.z  + m[12];
  y = m[1]*Q.x + m[5]*Q.y + m[9]*Q.z  + m[13];
  z = m[2]*Q.x + m[6]*Q.y + m[10]*Q.z + m[14];
  a = m[3]*Q.x + m[7]*Q.y + m[11]*Q.z + m[15];

  Q.x = x/a;
  Q.y = y/a;
  Q.z = z/a;

  // v ist der Vektor von P nach Q

  v = Q - P;

  // Der Sichtstrahl ist nun P + s*v ********************************

  double minDist = 1000000.0;
  double dist;
  
  Point3D H(0.0, 0.0, 0.0);
  bestIndex = 0;
  bestT = 0.0;

  // Alle Punkte und Liniensegmente des Linienzuges durchlaufen

  int i;
  Point3D w;
  double A, B, C, D, E, F;
  Point3D R;
  double denominator;

  for (i=0; i < numPoints; i++)
  {
    // Abstand vom Stützpunkt zum Blickpunktstrahl

    R = point[i] - P;
    s = scalarProduct(R, v) / scalarProduct(v, v);
    R-= v*s;

    dist = R.magnitude();

    if (dist < minDist)
    {
      minDist = dist;
      bestIndex = i;
      bestT = 0.0;
      H = point[i];
    }

    // Gibt es ein Abstandsminimum zw. den Punkten point[i] und point[i+1] ?
    if (i < numPoints-1)
    {
      w = point[i+1] - point[i];
      R = point[i] - P;

      A = -scalarProduct(v, v);
      B = scalarProduct(v, w);
      C = -B;
      D = scalarProduct(w, w);
      
      E = -scalarProduct(R, v);
      F = -scalarProduct(R, w);

      denominator = A*D - B*C;
      if (denominator == 0.0) { denominator = 0.000001; }
      t = (A*F - E*C) / denominator;

      if ((t >= 0.0) && (t <= 1.0))
      {
        R = point[i] + w*t - P;
        s = scalarProduct(R, v) / scalarProduct(v, v);
        R-= v*s;

        dist = R.magnitude();

        if (dist < minDist)
        {
          minDist = dist;
          bestIndex = i;
          bestT = t;
          H = point[i] + w*t;
        }
      }
    }

  }

  return H;
}


// ****************************************************************************
// Setzt den Kontrollpunkt pointIndex auf die Bildschirmkoordinaten (screenX,  
// screenY) und verändert entsprechend die betroffenen Vokaltraktparameter.
// Der Wertebereich der Parameter (und die Lage des Kontrollpunktes) wird 
// automatisch eingeschränkt.
// ****************************************************************************

void VocalTractPicture::setControlPoint(int pointIndex, int screenX, int screenY)
{
  const Point3D XY_PLANE_POINT(0.0, 0.0, 0.0);
  const Point3D XY_PLANE_NORMAL(0.0, 0.0, 1.0);
  Point3D planePoint;
  Point3D P, Q;
  VocalTract::Param *p;
  double t;
  Surface *surface;
  int rib;
  int ribPoint;
  int i;
  Point3D points[32];
//  int numPoints;
  int bestIndex;
  double bestT;

  // ****************************************************************
  // Some control points may (possibly temporarily) not be controlled
  // by means of the control points.
  // ****************************************************************

  if ((pointIndex == CP_CUT_PLANE) && (showCenterLine == false)) { return; }


  // ****************************************************************

  switch(pointIndex)
  {

    // Velum shape and velic opening.

    case CP_VELUM:
    {
      double xLeft = -2.5;
      double xRight = -1.5;
      double yTop = 1.0;
      double yBottom = 0.0;

      Q = getObjectCoordinates(screenX, screenY, XY_PLANE_POINT, XY_PLANE_NORMAL);

      // Velic opening is set along the horizontal axis.
      t = (Q.x - xLeft) / (xRight - xLeft);
      p = &tract->param[VocalTract::VO];
      p->x = p->min + t*(p->max - p->min);
      tract->restrictParam(VocalTract::VO);

      // Velum shape is set along the vertical axis.
      t = (Q.y - yTop) / (yBottom - yTop);
      p = &tract->param[VocalTract::VS];
      p->x = p->min + t*(p->max - p->min);
      tract->restrictParam(VocalTract::VS);
    }
    break;

    // **************************************************************
    // Position and rotation of the jaw.
    // **************************************************************

    case CP_JAW:
    {
      // The control point has the coordinates of one of the jaw vertices.
      surface = &tract->surface[VocalTract::LOWER_COVER];
      rib = VocalTract::NUM_LARYNX_RIBS + VocalTract::NUM_THROAT_RIBS;
      planePoint = surface->getVertex(rib, 0);

      Q = getObjectCoordinates(screenX, screenY, planePoint, XY_PLANE_NORMAL);
      Point2D v = tract->anatomy.jawRestPos - tract->anatomy.jawFulcrum;
      Point2D w = Q.toPoint2D() - tract->anatomy.jawFulcrum;

      // Determine the sinus of the rotation angle first.
      double denominator = w.y*w.y + w.x*w.x;
      if (fabs(denominator) < 0.000001)
      {
        denominator = 0.000001;
      }
      double p = 2.0*v.y*w.x / denominator;
      double q = (v.y*v.y - w.y*w.y) / denominator;
      double radicand = 0.25*p*p - q;

      // Change the parameters only if the radicant is positiv !
      if (radicand >= 0.0)
      {
        double S1 = -0.5*p + sqrt(radicand);
        double S2 = -0.5*p - sqrt(radicand);

        // We are only interested in negative angles, so that 
        // we require: sin(angle) <= 0 -> we consider only the smaller S2.

        if ((S2 > -1.0) && (S2 < 1.0))
        {
          double angle_rad = asin(S2);

          tract->param[VocalTract::JA].x = angle_rad*180.0 / M_PI;
          if ((angle_rad > -M_PI/4.0) && (angle_rad < M_PI/4.0))
          {
            tract->param[VocalTract::JX].x = (w.x + sin(angle_rad)*v.y) / cos(angle_rad) - v.x;
          }          
          
          tract->restrictParam(VocalTract::JX);
          tract->restrictParam(VocalTract::JA);
        }
      }
    }
    break;

    // **************************************************************
    // Control point at the hyoid.
    // **************************************************************

    case CP_HYOID:
    {
      Q = getObjectCoordinates(screenX, screenY, XY_PLANE_POINT, XY_PLANE_NORMAL);

      Point3D narrowPoint, widePoint;
      rib = VocalTract::NUM_LARYNX_RIBS - 1;
      ribPoint = VocalTract::NUM_LOWER_COVER_POINTS - 1;

      surface = &tract->surface[VocalTract::NARROW_LARYNX_FRONT];
      narrowPoint = surface->getVertex(rib, ribPoint);

      surface = &tract->surface[VocalTract::WIDE_LARYNX_FRONT];
      widePoint = surface->getVertex(rib, ribPoint);

      double hyoidY = tract->surface[VocalTract::LOWER_COVER].getVertex(
        VocalTract::NUM_LARYNX_RIBS-1, VocalTract::NUM_LOWER_COVER_POINTS-1).y;
      Q.x-= tract->getPharynxBackX(hyoidY);
      t = (Q.x - narrowPoint.x) / (widePoint.x - narrowPoint.x);

      tract->param[VocalTract::HX].x = t;
      tract->param[VocalTract::HY].x = Q.y;

      tract->restrictParam(VocalTract::HX);
      tract->restrictParam(VocalTract::HY);
    }
    break;

    // **************************************************************
    // Control point at the mouth corner.
    // **************************************************************

    case CP_LIP_CORNER:
    {
      Point3D path[MAX_SPLINE_POINTS];
      int N = tract->lipCornerPath.getNumPoints();
      int i;
      double param[MAX_SPLINE_POINTS];

      for (i=0; i < N; i++) 
      { 
        path[i] = tract->lipCornerPath.getControlPoint(i); 
        if (i == 0)
        {
          param[i] = 0.0;
        }
        else
        {
          param[i] = param[i-1] + (path[i] - path[i-1]).magnitude();
        }
      }
      
      // Normalize the parameter for the curve length.
      double length = param[N-1];
      if (length < 0.000001) { length = 0.000001; }
      for (i=1; i < N; i++) { param[i]/= length; }

      p = &tract->param[VocalTract::LP];
      Q = getMinDistancePoint(screenX, screenY, path, N, bestIndex, bestT);

      double s = param[bestIndex];
      if (bestIndex < N-1) { s+= bestT*(param[bestIndex+1] - param[bestIndex]); }

      p->x = p->min + s*(p->max - p->min);
      tract->restrictParam(VocalTract::LP);
    }
    break;

    // **************************************************************
    // Control point for the vertical lip distance.
    // **************************************************************

    case CP_LIP_DISTANCE:
    {
      Point3D onset, corner, F0, F1;
      double yClose;

      tract->getImportantLipPoints(onset, corner, F0, F1, yClose);
      Q = getObjectCoordinates(screenX, screenY, XY_PLANE_POINT, XY_PLANE_NORMAL);

      p = &tract->param[VocalTract::LD];
      t = Q.y - yClose;
      P = tract->surface[VocalTract::UPPER_TEETH].getVertex(VocalTract::NUM_TEETH_RIBS-1, 3);
      if (t > P.y - yClose) { t = P.y - yClose; }
      p->x = 2.0*t;

      tract->restrictParam(VocalTract::LD);
    }
    break;

    // **************************************************************
    // Control points for tongue parameters.
    // **************************************************************

    case CP_TONGUE_CENTER:
    {
      Q = getObjectCoordinates(screenX, screenY, XY_PLANE_POINT, XY_PLANE_NORMAL);
      tract->param[VocalTract::TCX].x = Q.x;
      tract->param[VocalTract::TCY].x = Q.y;

      tract->restrictTongueParams();
    }
    break;

    case CP_TONGUE_TIP:
    {
      Q = getObjectCoordinates(screenX, screenY, XY_PLANE_POINT, XY_PLANE_NORMAL);
      tract->param[VocalTract::TTX].x = Q.x;
      tract->param[VocalTract::TTY].x = Q.y;

      tract->restrictTongueParams();
    }
    break;

    case CP_TONGUE_BLADE:
    {
      Q = getObjectCoordinates(screenX, screenY, XY_PLANE_POINT, XY_PLANE_NORMAL);
      tract->param[VocalTract::TBX].x = Q.x;
      tract->param[VocalTract::TBY].x = Q.y;

      tract->restrictTongueParams();
    }
    break;

    case CP_TONGUE_BACK:
    {
      Q = getObjectCoordinates(screenX, screenY, XY_PLANE_POINT, XY_PLANE_NORMAL);
      tract->param[VocalTract::TRX].x = Q.x;
      tract->param[VocalTract::TRY].x = Q.y;
      tract->restrictTongueParams();
    }
    break;

    // **************************************************************
    // Positioning of the intersection plane.
    // **************************************************************

    case CP_CUT_PLANE:
    {
      Point3D P[257];
      if (VocalTract::NUM_CENTERLINE_POINTS > 257) { break; }

      for (i=0; i < VocalTract::NUM_CENTERLINE_POINTS; i++)
      {
        P[i] = tract->centerLine[i].point.toPoint3D();
      }

      Q = getMinDistancePoint(screenX, screenY, P, VocalTract::NUM_CENTERLINE_POINTS, bestIndex, bestT);
      cutPlanePos_cm = tract->centerLine[bestIndex].pos;
      if (bestIndex < VocalTract::NUM_CENTERLINE_POINTS-1)
      { 
        cutPlanePos_cm+= bestT*(tract->centerLine[bestIndex+1].pos - tract->centerLine[bestIndex].pos);
      }

      if (cutPlanePos_cm < 0.0) { cutPlanePos_cm = 0.0; }
      if (cutPlanePos_cm > tract->centerLineLength) { cutPlanePos_cm = tract->centerLineLength; }
    }
    break;

  }

  parameterToControlPoint(pointIndex);
}


// ****************************************************************************
// Berechnet die Lage eines Kontrollpunktes in Objektkoordinaten in Abhängigkeit
// von den entsprechenden Vokaltrakt-Parameterwerten.
// ****************************************************************************

void VocalTractPicture::parameterToControlPoint(int pointIndex)
{
  double x, y, z;
  double t;
  VocalTract::Param *p;
  Surface *surface;
  int rib;
  int ribPoint;


  switch(pointIndex)
  {
    // **************************************************************
    // Opening and shape of the velum.
    // **************************************************************

    case CP_VELUM:
    {
      double xLeft = -2.5;
      double xRight = -1.5;
      double yTop = 1.0;
      double yBottom = 0.0;

      // Velic opening is set along the horizontal axis.
      p = &tract->param[VocalTract::VO];
      t = (p->x - p->min) / (p->max - p->min);
      x = xLeft + t*(xRight - xLeft);

      // Velum shape is set along the vertical axis.
      p = &tract->param[VocalTract::VS];
      t = (p->x - p->min) / (p->max - p->min);
      y = yTop + t*(yBottom - yTop);

      z = 0.0;
    }
    break;

    // **************************************************************
    // Position and opening angle of the jaw.
    // **************************************************************

    case CP_JAW:
    {
      // Der Kontrollpunkt bekommt die Koordinaten von einem der Kieferpunkte
      surface = &tract->surface[VocalTract::LOWER_COVER];
      rib = VocalTract::NUM_LARYNX_RIBS + VocalTract::NUM_THROAT_RIBS;
      surface->getVertex(rib, 0, x, y, z);
    }
    break;

    // **************************************************************
    // Hyoid.
    // **************************************************************

    case CP_HYOID:
    {
      // Der Kontrollpunkt bekommt die Koordinaten vom Zungenbein
      surface = &tract->surface[VocalTract::LOWER_COVER];
      rib = VocalTract::NUM_LARYNX_RIBS-1;
      ribPoint = VocalTract::NUM_LOWER_COVER_POINTS-1;
      surface->getVertex(rib, ribPoint, x, y, z);
    }
    break;

    // **************************************************************
    // Retraction/protrusion of the lips.
    // **************************************************************

    case CP_LIP_CORNER:
    {
      Point3D onset, corner, F0, F1;
      double yClose;

      tract->getImportantLipPoints(onset, corner, F0, F1, yClose);
      x = corner.x;
      y = corner.y;
      z = corner.z;
    }
    break;

    // **************************************************************
    // Distance between upper and lower lip.
    // **************************************************************

    case CP_LIP_DISTANCE:
    {
      Point3D onset, corner, F0, F1;
      double yClose;

      tract->getImportantLipPoints(onset, corner, F0, F1, yClose);
      p = &tract->param[VocalTract::LD];

      x = F0.x;
      y = yClose + 0.5*p->x;
      z = 0.0;
    }
    break;

    // **************************************************************
    // Tongue points.
    // **************************************************************

    case CP_TONGUE_CENTER:
    {
      x = tract->param[VocalTract::TCX].x;
      y = tract->param[VocalTract::TCY].x;
      z = 0.0;
    }
    break;

    case CP_TONGUE_TIP:
    {
      x = tract->param[VocalTract::TTX].x;
      y = tract->param[VocalTract::TTY].x;
      z = 0.0;
    }
    break;

    case CP_TONGUE_BLADE:
    {
      x = tract->param[VocalTract::TBX].x;
      y = tract->param[VocalTract::TBY].x;
      z = 0.0;
    }
    break;

    case CP_TONGUE_BACK:
    {
      x = tract->param[VocalTract::TRX].x;
      y = tract->param[VocalTract::TRY].x;
      z = 0.0;
    }
    break;

    // **************************************************************
    // Position of the intersection plane.
    // **************************************************************

    case CP_CUT_PLANE:
    {
      Point2D P, v;
      tract->getCutVector(cutPlanePos_cm, P, v);
      x = P.x;
      y = P.y;
      z = 0.0;
    }
    break;

    default:
      x = 0.0; 
      y = 0.0; 
      z = 0.0;
  }

  // Set the resulting coordinates for the given control point.

  controlPoint[pointIndex].set(x, y, z);
}


// ****************************************************************************
// Projeziert den Punkt P auf die Bildschirmkoordinaten (screenX, screenY).
// ****************************************************************************

void VocalTractPicture::projection(Point3D P, double &screenX, double &screenY, double &screenZ)
{
  double sx, sy, sz;

  gluProject(P.x, P.y, P.z,
             modelViewMatrix, projectionMatrix, viewport, 
             &sx, &sy, &sz);

  screenX = sx;
  screenY = viewportHeight - 1 - sy;     // Die y-Koordinate auf den Kopf stellen
  screenZ = sz;
#ifdef __linux__
  // On Linux, all pixel coordinates need to be scaled by the current
  // display scaling factor before passing them to OpenGL
  screenX /= GetContentScaleFactor();
  screenY /= GetContentScaleFactor();
#endif
}


// ****************************************************************************
/// Returns the borders of the region for the 2D display of the vocal tract.
// ****************************************************************************

void VocalTractPicture::get2DRegion(double &left_cm, double &right_cm, double &bottom_cm, double &top_cm)
{
  left_cm = -4.0;
  bottom_cm = -9.0;
  right_cm = 7.5;

  int w = this->GetSize().x;
  int h = this->GetSize().y;
  if (w < 1) { w = 1; }

  top_cm = bottom_cm + (double)h*(right_cm - left_cm)/(double)w;
}



// ****************************************************************************
// Schreibt die Daten zu einer Gitter-Oberfläche in eine SVG-Datei.
// ****************************************************************************

void VocalTractPicture::addSurfaceToSVG(std::ostream &os, ::Surface *surface, int firstRib, int lastRib,
    int firstRibPoint, int lastRibPoint, double minZ, double maxZ, bool localColors)
{
  double x1, y1, x2, y2;
  double strokeWidth;
  int r, g, b;      // Frabkomponenten
  int i, k;
  double sx[2], sy[2], sz[2];
  char st[1024];
  double scale = 1.0;
  
  // Daten zu den Einzellinien
  int numLines;
  Point2D *P0, *P1;     // Anfangs- und Endpunkt der projezierten Linie
  double *depth;

  // Evtl. die minimale und maximale Tiefe der Modellpunkte neu bestimmen

  if (localColors)
  {
    minZ = 1000000000.0;
    maxZ =-1000000000.0;

    for (i=firstRib; i <= lastRib; i++)
    {
      for (k=firstRibPoint; k < lastRibPoint; k++)
      {
        projection(surface->getVertex(i, k), sx[0], sy[0], sz[0]);
        if (sz[0] < minZ) { minZ = sz[0]; }
        if (sz[0] > maxZ) { maxZ = sz[0]; }
      }
    }
  }

  // Speicher reservieren *******************************************
  
  int maxLines = (surface->numRibs-1)*surface->numRibPoints + surface->numRibs*(surface->numRibPoints-1);
  P0 = new Point2D[maxLines];
  P1 = new Point2D[maxLines];
  depth = new double[maxLines];
  
  // Alle Einzellinien durchlaufen und deren Daten bestimmen ********

  numLines = 0;

  // Original
  for (i=firstRib; i <= lastRib; i++)
  {
    for (k=firstRibPoint; k < lastRibPoint; k++)
    {
      projection(surface->getVertex(i, k), sx[0], sy[0], sz[0]);
      projection(surface->getVertex(i, k+1), sx[1], sy[1], sz[1]);
      
      P0[numLines].set(sx[0], sy[0]);
      P1[numLines].set(sx[1], sy[1]);
      depth[numLines] = 0.5*(sz[0] + sz[1]);
      numLines++;
    }
  }

  // Connection lines between the ribs (Original)
  for (i=firstRib; i < lastRib; i++)
  {
    for (k=firstRibPoint; k <= lastRibPoint; k++)
    {
      projection(surface->getVertex(i, k), sx[0], sy[0], sz[0]);
      projection(surface->getVertex(i+1, k), sx[1], sy[1], sz[1]);
      
      P0[numLines].set(sx[0], sy[0]);
      P1[numLines].set(sx[1], sy[1]);
      depth[numLines] = 0.5*(sz[0] + sz[1]);
      numLines++;
    }
  }

  // Linien sortieren und in die Datei schreiben ********************

  const double EPSILON = 0.000001;
  if (maxZ < minZ + EPSILON) { maxZ = minZ + EPSILON; }
  double maxDepth;
  int index;
  double d;

  // Start a new group !
  sprintf(st, "<g>%c%c", 13, 10);
  os << st;  
  
  for (i=0; i < numLines; i++)
  {
    // Diejenige der verbleibenden Linien mit der größten Tiefe raussuchen
    index = -1;
    maxDepth = 0.0;
    for (k=0; k < numLines; k++)
    {
      if (depth[k] > maxDepth)
      {
        maxDepth = depth[k];
        index = k;
      }
    }

    // Eine neue Linie in die Datei schreiben ***********************
    if (index != -1)
    {
      // Die Linienfarbe und -dicke aus der Tiefe bestimmen
      d = (depth[index] - minZ) / (maxZ - minZ);
      if (d < 0.0) { d = 0.0; }
      if (d > 1.0) { d = 1.0; }

      r = g = b = (int)(d*160);
      strokeWidth=0.5; //2.0-d;

//      r = g = b = 0;
//      strokeWidth=2.0-d;

      x1 = P0[index].x*scale;
      y1 = P0[index].y*scale;
      x2 = P1[index].x*scale;
      y2 = P1[index].y*scale;

      sprintf(st, "<line x1=\"%2.2f\" y1=\"%2.2f\" x2=\"%2.2f\" y2=\"%2.2f\""
        " stroke=\"rgb(%d,%d,%d)\" stroke-width=\"%2.2f\"/>%c%c", 
        x1, y1, x2, y2, r, g, b, strokeWidth, 13, 10);
      os << st;

      depth[index] = -1;
    }
  }

  // End the new group !
  sprintf(st, "</g>%c%c", 13, 10);
  os << st;

  // Speicher freigeben *********************************************

  delete P0;
  delete P1;
  delete depth;
}


// ****************************************************************************
/// Save the 2D-projection of the 3D vocal tract as wireframe object.
// ****************************************************************************

bool VocalTractPicture::exportTractWireframeSVG(const wxString &fileName, int item)
{
  int i, k;
  char st[1024];

  ofstream os(fileName.ToStdString());
  if (!os)
  {
    return false;
  }
 
  tract->calculateAll();

  // ****************************************************************
  // Render the model on the screen as wireframe, such that the 
  // transformation matrix is set properly
  // ****************************************************************
  
  renderMode = RM_3DWIRE;
  display();

  // Vorspann der Datei *********************************************
  
  sprintf(st, "<svg width=\"400\" height=\"400\">%c%c", 13,10);
  os << st;

  sprintf(st, "<g style=\"stroke-linecap:round;stroke-linejoin:round\">%c%c", 13,10);
  os << st;

  // Die minimale und maximale Tiefe ALLER Modellpunkte/-gitter ermitteln *****

  ::Surface *surface = NULL;
  double screenX, screenY, screenZ;
  double minZ = 1000000000.0;
  double maxZ =-1000000000.0;

  for (i=0; i < VocalTract::NUM_SURFACES; i++)
  {

    // Beidseitige Darstellung
    if 
    (
     ((renderBothSides) &&
      ((i == VocalTract::UPPER_TEETH_TWOSIDE) ||
      (i == VocalTract::LOWER_TEETH_TWOSIDE) ||
      (i == VocalTract::UPPER_COVER_TWOSIDE) ||
      (i == VocalTract::LOWER_COVER_TWOSIDE) ||
      (i == VocalTract::UPPER_LIP_TWOSIDE) ||
      (i == VocalTract::LOWER_LIP_TWOSIDE) ||
      (i == VocalTract::LEFT_COVER) ||
      (i == VocalTract::RIGHT_COVER) ||
      (i == VocalTract::TONGUE))
     )
     ||
     ((renderBothSides == false) &&
     ((i == VocalTract::UPPER_TEETH) ||
      (i == VocalTract::LOWER_TEETH) ||
      (i == VocalTract::UPPER_COVER) ||
      (i == VocalTract::LOWER_COVER) ||
      (i == VocalTract::UPPER_LIP) ||
      (i == VocalTract::LOWER_LIP) ||
      (i == VocalTract::LEFT_COVER) ||
      (i == VocalTract::TONGUE))
      )
     )
    {
      surface = &tract->surface[i];
      for (k=0; k < surface->numVertices; k++)
      {
        projection(surface->vertex[k].coord, screenX, screenY, screenZ);
        if (screenZ < minZ) { minZ = screenZ; }
        if (screenZ > maxZ) { maxZ = screenZ; }
      }
    }
  }

  // Die eigentlichen Strichdaten.
  
  if (item == 0)
  {
    // Larynx Hinterseite
    surface = &tract->surface[VocalTract::UPPER_COVER_TWOSIDE];
    addSurfaceToSVG(os, surface, 
      0, VocalTract::NUM_LARYNX_RIBS-1, 0, surface->numRibPoints-1, minZ, maxZ, true);
  }
  else
  if (item == 1)
  {
    // Rachen Hinterseite
    surface = &tract->surface[VocalTract::UPPER_COVER_TWOSIDE];
    addSurfaceToSVG(os, surface, 
      VocalTract::NUM_LARYNX_RIBS-1, VocalTract::NUM_LARYNX_RIBS+VocalTract::NUM_PHARYNX_RIBS-1, 0, surface->numRibPoints-1, minZ, maxZ, true);
  }
  else
  if (item == 2)
  {
    // Velum
    surface = &tract->surface[VocalTract::UPPER_COVER_TWOSIDE];
    addSurfaceToSVG(os, surface, 
      VocalTract::NUM_LARYNX_RIBS+VocalTract::NUM_PHARYNX_RIBS, 
      VocalTract::NUM_LARYNX_RIBS+VocalTract::NUM_PHARYNX_RIBS + VocalTract::NUM_VELUM_RIBS, 
      0, surface->numRibPoints-1, minZ, maxZ, true);
  }
  else
  if (item == 3)
  {
    // Palatum
    surface = &tract->surface[VocalTract::UPPER_COVER_TWOSIDE];
    addSurfaceToSVG(os, surface, 
      VocalTract::NUM_LARYNX_RIBS+VocalTract::NUM_PHARYNX_RIBS + VocalTract::NUM_VELUM_RIBS,
      surface->numRibs-1,
      0, surface->numRibPoints-1, minZ, maxZ, true);
  }

  // ****************************************************************
  
  if (item == 4)
  {
    // Vorderseite Larynx
    surface = &tract->surface[VocalTract::LOWER_COVER_TWOSIDE];
    addSurfaceToSVG(os, surface, 
      0, VocalTract::NUM_LARYNX_RIBS-1, 0, surface->numRibPoints-1, minZ, maxZ, true);
  }
  else
  if (item == 5)
  {
    // Vorderseite Rachen
    surface = &tract->surface[VocalTract::LOWER_COVER_TWOSIDE];
    addSurfaceToSVG(os, surface, 
      VocalTract::NUM_LARYNX_RIBS-1, VocalTract::NUM_LARYNX_RIBS+VocalTract::NUM_THROAT_RIBS,
      0, surface->numRibPoints-1, minZ, maxZ, true);
  }
  else
  if (item == 6)
  {
    // Unterkiefer
    surface = &tract->surface[VocalTract::LOWER_COVER_TWOSIDE];
    addSurfaceToSVG(os, surface, 
      VocalTract::NUM_LARYNX_RIBS+VocalTract::NUM_THROAT_RIBS, surface->numRibs-1,
      0, surface->numRibPoints-1, minZ, maxZ, true);
  }

  // Das gesamte Modell in eine Datei speichern.

  if (item == -1)
  {
    if (renderBothSides)
    {
      // Gesamte obere Hülle
      surface = &tract->surface[VocalTract::UPPER_COVER_TWOSIDE];
      addSurfaceToSVG(os, surface, 
        0, surface->numRibs-1, 0, surface->numRibPoints-1, minZ, maxZ, false);

      // Gesamte untere Hülle
      surface = &tract->surface[VocalTract::LOWER_COVER_TWOSIDE];
      addSurfaceToSVG(os, surface, 
        0, surface->numRibs-1, 0, surface->numRibPoints-1, minZ, maxZ, false);

      // Obere Zähne
      surface = &tract->surface[VocalTract::UPPER_TEETH_TWOSIDE];
      addSurfaceToSVG(os, surface, 
        0, surface->numRibs-1, 0, surface->numRibPoints-1, minZ, maxZ, false);

      // Untere Zähne
      surface = &tract->surface[VocalTract::LOWER_TEETH_TWOSIDE];
      addSurfaceToSVG(os, surface, 
        0, surface->numRibs-1, 0, surface->numRibPoints-1, minZ, maxZ, false);

      // Lippen
      surface = &tract->surface[VocalTract::UPPER_LIP_TWOSIDE];
      addSurfaceToSVG(os, surface, 
        0, surface->numRibs-1, 0, surface->numRibPoints-1, minZ, maxZ, false);

      surface = &tract->surface[VocalTract::LOWER_LIP_TWOSIDE];
      addSurfaceToSVG(os, surface, 
        0, surface->numRibs-1, 0, surface->numRibPoints-1, minZ, maxZ, false);

      // Zunge
      surface = &tract->surface[VocalTract::TONGUE];
      addSurfaceToSVG(os, surface, 
        0, surface->numRibs-1, 0, surface->numRibPoints-1, minZ, maxZ, false);

      // Epiglottis
      surface = &tract->surface[VocalTract::EPIGLOTTIS_TWOSIDE];
      addSurfaceToSVG(os, surface, 
        0, surface->numRibs-1, 0, surface->numRibPoints-1, minZ, maxZ, false);

      // Uvula     
      surface = &tract->surface[VocalTract::UVULA_TWOSIDE];
      addSurfaceToSVG(os, surface, 
        0, surface->numRibs-1, 0, surface->numRibPoints-1, minZ, maxZ, false);
    }
    else
    {
      // Gesamte obere Hülle
      surface = &tract->surface[VocalTract::UPPER_COVER];
      addSurfaceToSVG(os, surface, 
        0, surface->numRibs-1, 0, surface->numRibPoints-1, minZ, maxZ, false);

      // Gesamte untere Hülle
      surface = &tract->surface[VocalTract::LOWER_COVER];
      addSurfaceToSVG(os, surface, 
        0, surface->numRibs-1, 0, surface->numRibPoints-1, minZ, maxZ, false);

      // Obere Zähne
      surface = &tract->surface[VocalTract::UPPER_TEETH];
      addSurfaceToSVG(os, surface, 
        0, surface->numRibs-1, 0, surface->numRibPoints-1, minZ, maxZ, false);

      // Untere Zähne
      surface = &tract->surface[VocalTract::LOWER_TEETH];
      addSurfaceToSVG(os, surface, 
        0, surface->numRibs-1, 0, surface->numRibPoints-1, minZ, maxZ, false);

      // Lippen
      surface = &tract->surface[VocalTract::UPPER_LIP];
      addSurfaceToSVG(os, surface, 
        0, surface->numRibs-1, 0, surface->numRibPoints-1, minZ, maxZ, false);

      surface = &tract->surface[VocalTract::LOWER_LIP];
      addSurfaceToSVG(os, surface, 
        0, surface->numRibs-1, 0, surface->numRibPoints-1, minZ, maxZ, false);

      // Zunge
      surface = &tract->surface[VocalTract::TONGUE];
      addSurfaceToSVG(os, surface, 
        0, surface->numRibs-1, 0, surface->numRibPoints-1, minZ, maxZ, false);

      // Epiglottis
      surface = &tract->surface[VocalTract::EPIGLOTTIS];
      addSurfaceToSVG(os, surface, 
        0, surface->numRibs-1, 0, surface->numRibPoints-1, minZ, maxZ, false);

      // Uvula     
      surface = &tract->surface[VocalTract::UVULA];
      addSurfaceToSVG(os, surface, 
        0, surface->numRibs-1, 0, surface->numRibPoints-1, minZ, maxZ, false);
    }
  }
  else

  // ****************************************************************
  // Nur die Lippenpartie speichern.
  // ****************************************************************

  if (item == -2)
  {
    // Obere Zähne
    surface = &tract->surface[VocalTract::UPPER_TEETH_TWOSIDE];
    addSurfaceToSVG(os, surface, 0, surface->numRibs-1, 3, 3, minZ, maxZ, false);

    // Untere Zähne
    surface = &tract->surface[VocalTract::LOWER_TEETH_TWOSIDE];
    addSurfaceToSVG(os, surface, 0, surface->numRibs-1, 3, 3, minZ, maxZ, false);

    // Obere Lippe
    surface = &tract->surface[VocalTract::UPPER_LIP_TWOSIDE];
    for (i=0; i < surface->numRibs; i++)
    {
      addSurfaceToSVG(os, surface, i, i, 0, VocalTract::NUM_INNER_LIP_POINTS-1, minZ, maxZ, false);
    }
    addSurfaceToSVG(os, surface, 
      0, surface->numRibs-1, VocalTract::NUM_INNER_LIP_POINTS-1, VocalTract::NUM_INNER_LIP_POINTS-1, minZ, maxZ, false);

    // Untere Lippe
    surface = &tract->surface[VocalTract::LOWER_LIP_TWOSIDE];
    for (i=0; i < surface->numRibs; i++)
    {
      addSurfaceToSVG(os, surface, i, i, 0, VocalTract::NUM_INNER_LIP_POINTS-1, minZ, maxZ, false);
    }
    addSurfaceToSVG(os, surface, 
      0, surface->numRibs-1, VocalTract::NUM_INNER_LIP_POINTS-1, VocalTract::NUM_INNER_LIP_POINTS-1, minZ, maxZ, false);

    // Die Mundwinkelleitlinien

    LineStrip3D *ls = NULL;
    int N;
    Point3D P;

    ls = &tract->lipCornerPath;
    N  = ls->getNumPoints();

    // Vorspann
    sprintf(st, "<g stroke=\"black\" stroke-linecap=\"round\" stroke-linejoin=\"round\">%c%c", 13,10);
    os << st;

    // Den mittleren Pfad links.

    // Den 1. Punkt
    projection(ls->getControlPoint(0), screenX, screenY, screenZ);
    sprintf(st, "<path d=\"M %2.2f %2.2f ", screenX, screenY);
    os << st;

    for (i=1; i < N; i++)
    {
      projection(ls->getControlPoint(i), screenX, screenY, screenZ);
      sprintf(st, "L %2.2f %2.2f ", screenX, screenY);
      os << st;
    }
    sprintf(st, "\"/>%c%c", 13,10);
    os << st;

    // Den mittleren Pfad rechts.

    // Den 1. Punkt
    P = ls->getControlPoint(0);
    P.z = -P.z;
    projection(P, screenX, screenY, screenZ);
    sprintf(st, "<path d=\"M %2.2f %2.2f ", screenX, screenY);
    os << st;

    for (i=1; i < N; i++)
    {
      P = ls->getControlPoint(i);
      P.z = -P.z;
      projection(P, screenX, screenY, screenZ);
      sprintf(st, "L %2.2f %2.2f ", screenX, screenY);
      os << st;
    }
    sprintf(st, "\"/>%c%c", 13,10);
    os << st;

    // Nachspann
    sprintf(st, "</g>%c%c", 13,10);
    os << st;
  }

  else

  // ****************************************************************
  // Nur die Mundwinkelleitlinien speichern.
  // ****************************************************************

  if (item == -3)
  {
    // Obere Zähne
    surface = &tract->surface[VocalTract::UPPER_TEETH_TWOSIDE];
    addSurfaceToSVG(os, surface, 0, surface->numRibs-1, 3, 4, minZ, maxZ, false);

    // Die Mundwinkelleitlinien

    LineStrip3D *ls = NULL;
    int N;
    Point3D P;

    // Vorspann
    sprintf(st, "<g stroke=\"black\" stroke-linecap=\"round\" stroke-linejoin=\"round\">%c%c", 13,10);
    os << st;

    // Den weiten Pfad links ****************************************

    ls = &tract->wideLipCornerPath;
    N  = ls->getNumPoints();

    // Den 1. Punkt
    projection(ls->getControlPoint(0), screenX, screenY, screenZ);
    sprintf(st, "<path d=\"M %2.2f %2.2f ", screenX, screenY);
    os << st;

    for (i=1; i < N; i++)
    {
      projection(ls->getControlPoint(i), screenX, screenY, screenZ);
      sprintf(st, "L %2.2f %2.2f ", screenX, screenY);
      os << st;
    }
    sprintf(st, "\"/>%c%c", 13,10);
    os << st;

    // Den weiten Pfad rechts ***************************************

    // Den 1. Punkt
    P = ls->getControlPoint(0);
    P.z = -P.z;
    projection(P, screenX, screenY, screenZ);
    sprintf(st, "<path d=\"M %2.2f %2.2f ", screenX, screenY);
    os << st;

    for (i=1; i < N; i++)
    {
      P = ls->getControlPoint(i);
      P.z = -P.z;
      projection(P, screenX, screenY, screenZ);
      sprintf(st, "L %2.2f %2.2f ", screenX, screenY);
      os << st;
    }
    sprintf(st, "\"/>%c%c", 13,10);
    os << st;

    // Den engen Pfad links *****************************************

    ls = &tract->narrowLipCornerPath;
    N  = ls->getNumPoints();

    // Den 1. Punkt
    projection(ls->getControlPoint(0), screenX, screenY, screenZ);
    sprintf(st, "<path d=\"M %2.2f %2.2f ", screenX, screenY);
    os << st;

    for (i=1; i < N; i++)
    {
      projection(ls->getControlPoint(i), screenX, screenY, screenZ);
      sprintf(st, "L %2.2f %2.2f ", screenX, screenY);
      os << st;
    }
    sprintf(st, "\"/>%c%c", 13,10);
    os << st;

    // Den engen Pfad rechts ****************************************

    // Den 1. Punkt
    P = ls->getControlPoint(0);
    P.z = -P.z;
    projection(P, screenX, screenY, screenZ);
    sprintf(st, "<path d=\"M %2.2f %2.2f ", screenX, screenY);
    os << st;

    for (i=1; i < N; i++)
    {
      P = ls->getControlPoint(i);
      P.z = -P.z;
      projection(P, screenX, screenY, screenZ);
      sprintf(st, "L %2.2f %2.2f ", screenX, screenY);
      os << st;
    }
    sprintf(st, "\"/>%c%c", 13,10);
    os << st;

    // Nachspann
    sprintf(st, "</g>%c%c", 13,10);
    os << st;
  }

  // Nachspann der Datei ********************************************

  sprintf(st, "</g>%c%c", 13,10);
  os << st;

  sprintf(st, "</svg>%c%c", 13,10);
  os << st;

  // Datei schliessen ************************************************
  os.close();

  return true;
}


// ****************************************************************************
// Export the current contour image as SVG file.
// ****************************************************************************

bool VocalTractPicture::exportCrossSectionSVG(const wxString &fileName)
{
  const double INVALID = VocalTract::INVALID_PROFILE_SAMPLE;
  int i;
  Tube::Articulator articulator;
  char st[1024];

  ofstream os(fileName.ToStdString());
  if (!os)
  {
    return false;
  }


  // Vorspann der Datei *********************************************
  
  sprintf(st, "<svg width=\"400\" height=\"400\">%c%c", 13,10);
  os << st;

  sprintf(st, "<g style=\"stroke-linecap:round;stroke-linejoin:round\">%c%c", 13,10);
  os << st;

  // Die eigentlichen Profillinien **********************************

  Point2D P, v;
  double upperProfile[VocalTract::NUM_PROFILE_SAMPLES];
  double lowerProfile[VocalTract::NUM_PROFILE_SAMPLES];

  if (showTongueCrossSections)
  {
    i = (int)(cutPlanePos_cm*2.0);
    if (i < 0) { i = 0; }
    if (i >= VocalTract::NUM_TONGUE_RIBS) { i = VocalTract::NUM_TONGUE_RIBS-1; }
    P = tract->tongueRib[i].point;
    v = tract->tongueRib[i].normal;
  }
  else
  {
    tract->getCutVector(cutPlanePos_cm, P, v);
  }
  tract->getCrossProfiles(P, v, upperProfile, lowerProfile, crossSectionWithTongue, articulator);

  // In die Datei schreiben...

  const double SCALE = 50.0;
  double x1, y1, x2, y2;
  int r=0, g=0, b=0;
  double strokeWidth=1.5;

  for (i=0; i < VocalTract::NUM_PROFILE_SAMPLES-1; i++)
  {
    if ((upperProfile[i] != INVALID) && (upperProfile[i+1] != INVALID))
    {
      x1 = SCALE*(double)i*VocalTract::PROFILE_SAMPLE_LENGTH;
      y1 = -SCALE*upperProfile[i];
      x2 = SCALE*(double)(i+1)*VocalTract::PROFILE_SAMPLE_LENGTH;
      y2 = -SCALE*upperProfile[i+1];

      sprintf(st, "<line x1=\"%2.2f\" y1=\"%2.2f\" x2=\"%2.2f\" y2=\"%2.2f\""
        " stroke=\"rgb(%d,%d,%d)\" stroke-width=\"%2.2f\" stroke-linecap=\"round\" stroke-linejoin=\"round\"/>%c%c", 
        x1, y1, x2, y2, r, g, b, strokeWidth, 13, 10);
      os << st;
    }
  }

  for (i=0; i < VocalTract::NUM_PROFILE_SAMPLES-1; i++)
  {
    if ((lowerProfile[i] != INVALID) && (lowerProfile[i+1] != INVALID))
    {
      x1 = SCALE*(double)i*VocalTract::PROFILE_SAMPLE_LENGTH;
      y1 = -SCALE*lowerProfile[i];
      x2 = SCALE*(double)(i+1)*VocalTract::PROFILE_SAMPLE_LENGTH;
      y2 = -SCALE*lowerProfile[i+1];

      sprintf(st, "<line x1=\"%2.2f\" y1=\"%2.2f\" x2=\"%2.2f\" y2=\"%2.2f\""
        " stroke=\"rgb(%d,%d,%d)\" stroke-width=\"%2.2f\"  stroke-linecap=\"round\" stroke-linejoin=\"round\"/>%c%c", 
        x1, y1, x2, y2, r, g, b, strokeWidth, 13, 10);
      os << st;
    }
  }

  // Einen kleinen Kreis an den lokalen Koordinatenursprung malen ***

  sprintf(st, "<circle cx=\"%2.2f\" cy=\"%2.2f\" r=\"2.0\""
    " style=\"fill:rgb(0,0,0)\"/>%c%c",
     SCALE*VocalTract::PROFILE_SAMPLE_LENGTH * VocalTract::NUM_PROFILE_SAMPLES/2, 0.0, 13, 10);
  os << st;

  // Horizontalen Strich dorthin malen, wo das obere das untere Profil berührt

  double minY = 1000.0;
  for (i=0; i < VocalTract::NUM_PROFILE_SAMPLES; i++)
  {
    if (upperProfile[i] != INVALID)
    {
      if (upperProfile[i] < minY) { minY = upperProfile[i]; }
    }
  }

  y1 = -SCALE*minY;
  y2 = y1;
  x1 = 0.0;
  x2 = SCALE*(double)(VocalTract::NUM_PROFILE_SAMPLES-1)*VocalTract::PROFILE_SAMPLE_LENGTH;

  sprintf(st, "<line x1=\"%2.2f\" y1=\"%2.2f\" x2=\"%2.2f\" y2=\"%2.2f\""
    " stroke=\"rgb(%d,%d,%d)\" stroke-width=\"%2.2f\"  stroke-linecap=\"round\" stroke-linejoin=\"round\"/>%c%c", 
    x1, y1, x2, y2, r, g, b, strokeWidth, 13, 10);
  os << st;

  // Nachspann der Datei ********************************************

  sprintf(st, "</g>%c%c", 13,10);
  os << st;

  sprintf(st, "</svg>%c%c", 13,10);
  os << st;

  // Datei schließen ************************************************
  os.close();

  return true;
}


// ****************************************************************************
/// Saves the current vocal tract image as BMP-file.
// ****************************************************************************

bool VocalTractPicture::saveImageBmp(const wxString &fileName)
{
  // Read the OpenGL image into a pixel array
  GLint view[4];
  glGetIntegerv(GL_VIEWPORT, view);
  void *pixels = malloc(3 * view[2] * view[3]); // must use malloc
  
  glPixelStorei(GL_PACK_ALIGNMENT, 1);
  glReadBuffer( GL_BACK_LEFT );
  glReadPixels(0, 0, view[2], view[3], GL_RGB, GL_UNSIGNED_BYTE, pixels);
 
  // Put the image into a wxImage
  wxImage image((int)view[2], (int)view[3]);
  // After SetData(...), the image owns the pixel data and will also delete them.
  image.SetData((unsigned char*) pixels);
  image = image.Mirror(false); 

  return image.SaveFile(fileName, wxBITMAP_TYPE_BMP);
}


// ****************************************************************************
// Set the perspective and the viewport for the randering.
// ****************************************************************************

void VocalTractPicture::setProjectionMatrix3D(double fovy, double nearPlane, double farPlane)
{
  orthogonalProjection = false;

  int w = this->GetSize().x;
  int h = this->GetSize().y;
#ifdef __linux__
  // On Linux, all pixel coordinates need to be scaled by the current
  // display scaling factor before passing them to OpenGL
  w *= GetContentScaleFactor();
  h *= GetContentScaleFactor();
#endif
  if (w < 1) { w = 1; }
  if (h < 1) { h = 1; }

  frustumNear   = nearPlane;
  frustumFar    = farPlane;
  frustumTop    = tan(0.5*fovy*M_PI/180.0)*nearPlane;
  frustumRight  = frustumTop*(double)w/(double)h;
  frustumBottom = -frustumTop;
  frustumLeft   = -frustumRight;

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glFrustum(frustumLeft, frustumRight, frustumBottom, frustumTop, frustumNear, frustumFar);
  glGetDoublev(GL_PROJECTION_MATRIX, projectionMatrix);   // Projektionsmatrix merken

  // Den Viewport setzen
  viewportX = 0;
  viewportY = 0;
  viewportWidth = w;
  viewportHeight = h;

  glViewport((GLint)viewportX, (GLint)viewportY, (GLint)viewportWidth, (GLint)viewportHeight);
  glGetIntegerv(GL_VIEWPORT, viewport);   // Viewport-Geometrie in ein Array laden
}


// ****************************************************************************
// ****************************************************************************

void VocalTractPicture::setProjectionMatrix2D(double left, double right, double bottom, double top)
{
  int w = this->GetSize().x;
  int h = this->GetSize().y;
#ifdef __linux__
  // On Linux, all pixel coordinates need to be scaled by the current
  // display scaling factor before passing them to OpenGL
  w *= GetContentScaleFactor();
  h *= GetContentScaleFactor();
#endif

  orthogonalProjection = true;

  frustumNear   = -1000.0;
  frustumFar    = 1000.0;
  frustumTop    = top;
  frustumRight  = right;
  frustumBottom = bottom;
  frustumLeft   = left;

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(left, right, bottom, top, frustumNear, frustumFar);
  glGetDoublev(GL_PROJECTION_MATRIX, projectionMatrix);   // Projektionsmatrix merken

  // Den Viewport setzen
  viewportX = 0;
  viewportY = 0;
  viewportWidth = w;
  viewportHeight = h;

  glViewport((GLint)viewportX, (GLint)viewportY, (GLint)viewportWidth, (GLint)viewportHeight);
  glGetIntegerv(GL_VIEWPORT, viewport);   // Viewport-Geometrie in ein Array laden
}


// ****************************************************************************
// Modeling and viewpoint transformation for the 3D rendering.
// ****************************************************************************

void VocalTractPicture::setModelViewMatrix3D()
{
  glMatrixMode(GL_MODELVIEW);
  
  // Zuerst die inverse Matrix berechnen (umgekehrte Reihenfolge) ***

  glLoadIdentity();

  glTranslatef((GLfloat)(-DEFAULT_X_TRANSLATION), (GLfloat)(-DEFAULT_Y_TRANSLATION), 0.0);
  glRotated(-zRotation_deg, 0.0, 0.0, 1.0);   // Rotation um die z-Achse
  glRotated(-yRotation_deg, 0.0, 1.0, 0.0);   // Rotation um die y-Achse
  glTranslated(0.0, 0.0, distance_cm);     // Blickpunkttransformations
  
  glGetDoublev(GL_MODELVIEW_MATRIX, inverseModelViewMatrix);

  // Dann die Matrix für die richtige Transformationsreihenfolge ****

  glLoadIdentity();

  glTranslated(0.0, 0.0, -distance_cm);     // Blickpunkttransformations
  glRotated(yRotation_deg, 0.0, 1.0, 0.0);   // Rotation um die y-Achse
  glRotated(zRotation_deg, 0.0, 0.0, 1.0);   // Rotation um die z-Achse
  glTranslatef((GLfloat)DEFAULT_X_TRANSLATION, (GLfloat)DEFAULT_Y_TRANSLATION, 0.0);
  
  glGetDoublev(GL_MODELVIEW_MATRIX, modelViewMatrix);
}


// ****************************************************************************
// Modeling and viewpoint transformation for the 2D rendering.
// ****************************************************************************

void VocalTractPicture::setModelViewMatrix2D()
{
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glGetDoublev(GL_MODELVIEW_MATRIX, inverseModelViewMatrix);
  glGetDoublev(GL_MODELVIEW_MATRIX, modelViewMatrix);
}


// ****************************************************************************
// Load a poster image for the background.
// ****************************************************************************

bool VocalTractPicture::loadPoster(const wxString &fileName)
{
  if (fileName.IsEmpty()) 
  { 
    return false; 
  }

  wxImage *image = new wxImage(fileName);
  if (image->IsOk() == false)
  {
    delete image;
    return false;
  }

  // Free the memory of the current poster
  
  if (poster != NULL)
  {
    delete[] (COLORREF*)poster;
    poster = NULL;
    posterWidth_pix = 0;
    posterHeight_pix = 0;
  }

  posterWidth_pix  = image->GetWidth();
  posterHeight_pix = image->GetHeight();
  poster = new COLORREF[posterWidth_pix*posterHeight_pix];
  
  int x, y;
  int address;
  unsigned char red, green, blue;

  for (x=0; x < posterWidth_pix; x++)
  {
    for (y=0; y < posterHeight_pix; y++)
    {
      red   = image->GetRed(x, y);
      green = image->GetGreen(x, y);
      blue  = image->GetBlue(x, y);
      
      address = y*posterWidth_pix + x;
      COLORREF &pixel(((COLORREF*)poster)[address]);
      pixel = RGB(red, green, blue);
      pixel |= 0xFF000000;     // Set the alpha value to 1.
    }
  }

  // Free the wxImage object.
  delete image;

  return true;
}


// ****************************************************************************
/// Copy the current rendering of the vocal tract to the poster image with
/// slightly faded colors. This allows the easier comparison of changes of
/// vocal tract parameters.
// ****************************************************************************

void VocalTractPicture::currentPictureToPoster()
{
  // ****************************************************************
  // Redraw the current model picture (WITHOUT the current background
  // poster) before copying it to the background poster.
  // ****************************************************************

  bool oldShowPoster = showPoster;
//  bool oldShowControlPoints = showControlPoints;

  showPoster = false;
//  showControlPoints = false;

  display();
  this->Refresh();
  this->Update();
  wxYield();

  // ****************************************************************
  // Free the memory of the current poster and reset the position.
  // ****************************************************************

  if (poster != NULL)
  {
    delete[](COLORREF*)poster;
    poster = NULL;
    posterWidth_pix = 0;
    posterHeight_pix = 0;
  }

  // Get the 2D display area.
  double left_cm, right_cm, bottom_cm, top_cm;
  get2DRegion(left_cm, right_cm, bottom_cm, top_cm);

  posterCenterX_cm = (left_cm + right_cm) / 2.0;
  posterCenterY_cm = (bottom_cm + top_cm) / 2.0;
  posterScalingFactor = 1.0;

  // ****************************************************************
  // Read the current OpenGL image into a pixel array.
  // ****************************************************************

  GLint view[4];
  glGetIntegerv(GL_VIEWPORT, view);

  posterWidth_pix = view[2];
  posterHeight_pix = view[3];

  unsigned char* pixelBuffer = (unsigned char*)malloc(3 * posterWidth_pix * posterHeight_pix);

  glPixelStorei(GL_PACK_ALIGNMENT, 1);
  glReadBuffer(GL_BACK_LEFT);
  glReadPixels(0, 0, posterWidth_pix, posterHeight_pix, GL_RGB, GL_UNSIGNED_BYTE, pixelBuffer);

  // ****************************************************************
  // Copy the pixels from the OpenGL pixel buffer into the target
  // buffer (poster).
  // ****************************************************************

  poster = new COLORREF[posterWidth_pix * posterHeight_pix];

  int x, y;
  int address;
  int address2;
  unsigned char red, green, blue;

  for (x = 0; x < posterWidth_pix; x++)
  {
    for (y = 0; y < posterHeight_pix; y++)
    {
      address = y * posterWidth_pix + x;
      // One image is flipped upside-down.
      address2 = (posterHeight_pix - 1 - y) * posterWidth_pix + x;

      red = pixelBuffer[address2 * 3];
      green = pixelBuffer[address2 * 3 + 1];
      blue = pixelBuffer[address2 * 3 + 2];

      // Make the colors more faint (i.e. more white).
      red = red + (255 - red) * 2 / 3;
      green = green + (255 - green) * 2 / 3;
      blue = blue + (255 - blue) * 2 / 3;

      COLORREF& pixel(((COLORREF*)poster)[address]);
      pixel = RGB(red, green, blue);
      pixel |= 0xFF000000;     // Set the alpha value to 1.
    }
  }

  // Free the memory.
  free(pixelBuffer);

  showPoster = oldShowPoster;
//  showControlPoints = oldShowControlPoints;
}


// ****************************************************************************
// Place the light sources for 3D rendering.
// ****************************************************************************

void VocalTractPicture::setLights()
{
  GLfloat lightPos1[] = { 0.5, 0.0, 1.0, 0.0 };
  GLfloat lightAmbient[]  = { 0.4f, 0.4f, 0.4f, 1.0 };
  GLfloat lightDiffuse[]  = { 0.8f, 0.8f, 0.8f, 1.0 };
  GLfloat lightSpecular[] = { 0.3f, 0.3f, 0.3f, 1.0 };

  // Die Lichtquellen sollen nicht transformiert werden
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  glLightfv(GL_LIGHT0, GL_POSITION, lightPos1);
  glLightfv(GL_LIGHT0, GL_AMBIENT,  lightAmbient);
  glLightfv(GL_LIGHT0, GL_DIFFUSE,  lightDiffuse);
  glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);

  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);

  // Für korrekte Beleuchtung der Rückseiten
  glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
}


// ****************************************************************************
// Render the vocal tract with filled surfaces.
// ****************************************************************************

void VocalTractPicture::renderSolid()
{
  Point3D *V, *N;
  Point3D P, Q;
  int i, k;

  // ****************************************************************
  // Material properties 
  // ****************************************************************

  // Transparency factors
  GLfloat transTeeth = 0.5f; //0.4f;
  GLfloat transLip   = 0.6f;
  GLfloat transCover = 0.4f;

  // Upper and lower teeth ******************************************
  GLfloat teethMaterialSpecular[]  = { 1.0f, 1.0f, 1.0f, transTeeth };
  GLfloat teethMaterialShininess[] = { 50.0f };
  GLfloat teethMaterialDiffuse[]   = { 0.9f, 0.9f, 0.9f, transTeeth };
  GLfloat teethMaterialAmbient[]   = { 1.0f, 1.0f, 1.0f, 0.2f };

  // Upper and lower lip ********************************************
  GLfloat lipMaterialSpecular[]     = { 1.0f, 1.0f, 1.0f, transLip };
  GLfloat lipMaterialShininess[]    = { 50.0f };
  GLfloat frontLipMaterialDiffuse[] = { 1.0f, 0.5f, 0.5f, transLip };
  GLfloat backLipMaterialDiffuse[]  = { 1.0f, 0.7f, 0.7f, transLip };

  // Tongue *********************************************************
  GLfloat tongueMaterialSpecular[]     = { 1.0f, 1.0f, 1.0f, 1.0f };
  GLfloat tongueMaterialShininess[]    = { 50.0f };
  GLfloat frontTongueMaterialDiffuse[] = { 1.0f, 0.5f, 0.5f, 1.0f };
  GLfloat backTongueMaterialDiffuse[]  = { 1.0f, 0.5f, 0.5f, 1.0f };

  // Upper and lower cover ******************************************
  GLfloat coverMaterialSpecular[]  = { 1.0f, 1.0f, 1.0f, transCover };
  GLfloat coverMaterialShininess[] = { 50.0f };
  GLfloat coverMaterialDiffuse[]   = { 0.8f, 0.8f, 0.8f, transCover };
  GLfloat coverMaterialAmbient[]   = { 0.5f, 0.5f, 0.5f, 1.0f };

  // ****************************************************************
  // Render the tongue two-sided without transparency
  // ****************************************************************

  Surface *tongue = &tract->surface[VocalTract::TONGUE];

  glMaterialfv(GL_FRONT, GL_AMBIENT, frontTongueMaterialDiffuse);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, frontTongueMaterialDiffuse);

  glMaterialfv(GL_BACK, GL_AMBIENT, backTongueMaterialDiffuse);
  glMaterialfv(GL_BACK, GL_DIFFUSE, backTongueMaterialDiffuse);
  
  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, tongueMaterialSpecular);
  glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, tongueMaterialShininess);

  glDisable(GL_BLEND);
  glDepthMask(GL_TRUE);    // z-buffer is read and write

  tongue->calculateNormals();

  glBegin(GL_TRIANGLES);
  for (i=0; i < tongue->numTriangles; i++)
  {
    for (k=0; k < 3; k++)
    {
      N = &tongue->triangle[i].cornerNormal[k];
      V = &tongue->vertex[ tongue->triangle[i].vertex[k] ].coord;
      glNormal3d(N->x, N->y, N->z);
      glVertex3d(V->x, V->y, V->z);
    }
  }
  glEnd();

  // ****************************************************************
  // Create an array with all transparent surfaces.
  // ****************************************************************

  const int NUM_TRANSPARENT_SURFACES = 10;
  enum { 
    UPPER_TEETH = 0, LOWER_TEETH = 1, UPPER_LIP = 2, LOWER_LIP = 3, 
    UPPER_COVER = 4, LOWER_COVER = 5, LEFT_COVER = 6, RIGHT_COVER = 7,
    EPIGLOTTIS = 8, UVULA = 9
  };
  Surface *transSurface[NUM_TRANSPARENT_SURFACES] = 
    { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };

  if (renderBothSides)
  {
	  transSurface[UPPER_TEETH] = &tract->surface[VocalTract::UPPER_TEETH_TWOSIDE];
	  transSurface[LOWER_TEETH] = &tract->surface[VocalTract::LOWER_TEETH_TWOSIDE];
    transSurface[UPPER_LIP]   = &tract->surface[VocalTract::UPPER_LIP_TWOSIDE];
    transSurface[LOWER_LIP]   = &tract->surface[VocalTract::LOWER_LIP_TWOSIDE];
    transSurface[UPPER_COVER] = &tract->surface[VocalTract::UPPER_COVER_TWOSIDE];
    transSurface[LOWER_COVER] = &tract->surface[VocalTract::LOWER_COVER_TWOSIDE];
    transSurface[LEFT_COVER]  = &tract->surface[VocalTract::LEFT_COVER];
    transSurface[RIGHT_COVER] = &tract->surface[VocalTract::RIGHT_COVER];
    transSurface[EPIGLOTTIS]  = &tract->surface[VocalTract::EPIGLOTTIS_TWOSIDE];
    transSurface[UVULA]       = &tract->surface[VocalTract::UVULA_TWOSIDE];
  }
  else
  {
    transSurface[UPPER_TEETH] = &tract->surface[VocalTract::UPPER_TEETH];
	  transSurface[LOWER_TEETH] = &tract->surface[VocalTract::LOWER_TEETH];
	  transSurface[UPPER_LIP]   = &tract->surface[VocalTract::UPPER_LIP];
    transSurface[LOWER_LIP]   = &tract->surface[VocalTract::LOWER_LIP];
    transSurface[UPPER_COVER] = &tract->surface[VocalTract::UPPER_COVER];
    transSurface[LOWER_COVER] = &tract->surface[VocalTract::LOWER_COVER];
    transSurface[LEFT_COVER]  = &tract->surface[VocalTract::LEFT_COVER];
    transSurface[RIGHT_COVER] = NULL;
    transSurface[EPIGLOTTIS]  = &tract->surface[VocalTract::EPIGLOTTIS];
    transSurface[UVULA]       = &tract->surface[VocalTract::UVULA];
  }

  // ****************************************************************
  // Calculate the order of the triangles within each surface and the 
  // plane normals.
  // ****************************************************************

  for (i=0; i < NUM_TRANSPARENT_SURFACES; i++)
  {
    if (transSurface[i] != NULL)
    {
      transSurface[i]->calculateNormals();
      transSurface[i]->calculatePaintSequence(modelViewMatrix);
    }
  }

  // ****************************************************************
  // Some plane normals must be adjusted in order to avoid sharp 
  // edges.
  // ****************************************************************

  // Normals at the interface between the upper and lower cover *****

  int numRibs = VocalTract::NUM_LARYNX_RIBS + VocalTract::NUM_PHARYNX_RIBS;
  Surface *upperCover = transSurface[UPPER_COVER];
  Surface *lowerCover = transSurface[LOWER_COVER];

  for (i=0; i < numRibs; i++)
  {
    P = upperCover->getNormal(i, 0) + lowerCover->getNormal(i, 0);
    P.normalize();
    upperCover->setNormal(i, 0, P);
    lowerCover->setNormal(i, 0, P);

    if (renderBothSides) 
    { 
      P = upperCover->getNormal(i, upperCover->numRibPoints-1) + 
          lowerCover->getNormal(i, lowerCover->numRibPoints-1);
      P.normalize();
      upperCover->setNormal(i, upperCover->numRibPoints-1, P); 
      lowerCover->setNormal(i, lowerCover->numRibPoints-1, P);
    }
  }

  // Normals at the edge of the filling surfaces ********************

  Surface *leftCover = transSurface[LEFT_COVER];
  Surface *rightCover = transSurface[RIGHT_COVER];

  int firstRib = VocalTract::NUM_LARYNX_RIBS + VocalTract::NUM_PHARYNX_RIBS - 1;
  int lastRib  = VocalTract::NUM_LARYNX_RIBS + VocalTract::NUM_PHARYNX_RIBS + VocalTract::NUM_VELUM_RIBS-1;

  if (leftCover != NULL)
  {
    P = upperCover->getNormal(firstRib, 0);
    for (i=firstRib; i <= lastRib; i++) { upperCover->setNormal(i, 0, P); }
    leftCover->setNormal(0, 0, P);
    leftCover->setNormal(0, 1, P);
    leftCover->setNormal(0, 2, P);
    leftCover->setNormal(0, 3, P);
    
    leftCover->setNormal(1, 0, P);
    leftCover->setNormal(1, 1, P);

    P = lowerCover->getNormal(VocalTract::NUM_LARYNX_RIBS + VocalTract::NUM_THROAT_RIBS, 0);
    leftCover->setNormal(1, 2, P);
    leftCover->setNormal(1, 3, P);
  }

  if (rightCover != NULL)
  {
    P = upperCover->getNormal(firstRib, upperCover->numRibPoints-1);
    for (i=firstRib; i <= lastRib; i++) { upperCover->setNormal(i, upperCover->numRibPoints-1, P); }
    rightCover->setNormal(0, 0, P);
    rightCover->setNormal(0, 1, P);
    rightCover->setNormal(0, 2, P);
    rightCover->setNormal(0, 3, P);
    
    rightCover->setNormal(1, 0, P);
    rightCover->setNormal(1, 1, P);

    P = lowerCover->getNormal(VocalTract::NUM_LARYNX_RIBS + VocalTract::NUM_THROAT_RIBS, lowerCover->numRibPoints-1);
    rightCover->setNormal(1, 2, P);
    rightCover->setNormal(1, 3, P);
  }
  
  // Beginning and end of the uvula ribs ****************************

  if ((renderBothSides) && (transSurface[UVULA] != NULL))
  {
    Surface *s = transSurface[UVULA];
    for (i=0; i < s->numRibs; i++)
    {
      P = s->getNormal(i, 0) + s->getNormal(i, s->numRibPoints-1);
      P.normalize();
      s->setNormal(i, 0, P);
      s->setNormal(i, s->numRibPoints-1, P);
    }
  }

  // Beginning and end of the epiglottis ribs ***********************

  if ((renderBothSides) && (transSurface[EPIGLOTTIS] != NULL))
  {
    Surface *s = transSurface[EPIGLOTTIS];
    for (i=0; i < s->numRibs; i++)
    {
      P = s->getNormal(i, 0) + s->getNormal(i, s->numRibPoints-1);
      P.normalize();
      s->setNormal(i, 0, P);
      s->setNormal(i, s->numRibPoints-1, P);
    }
  }

  // ****************************************************************
  // Draw the transparent surfaces using merge sort for the 
  // individually sorted surfaces.
  // ****************************************************************

  int nextIndex[NUM_TRANSPARENT_SURFACES] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
  int numTriangles[NUM_TRANSPARENT_SURFACES] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
  int winningSurface;
  double z, minZ;
  Surface *s;
  bool drawTriangle;

  for (i=0; i < NUM_TRANSPARENT_SURFACES; i++)
  {
    if (transSurface[i] != NULL) { numTriangles[i] = transSurface[i]->numTriangles; }
  }

  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  // Blending function
  glEnable(GL_BLEND);                                 // Enable blending
  glDepthMask(GL_FALSE);                              // z-buffer is read-only

  // Main loop ******************************************************

  while ((nextIndex[0] < numTriangles[0]) || (nextIndex[1] < numTriangles[1]) ||
         (nextIndex[2] < numTriangles[2]) || (nextIndex[3] < numTriangles[3]) ||
		     (nextIndex[4] < numTriangles[4]) || (nextIndex[5] < numTriangles[5]) ||
         (nextIndex[6] < numTriangles[6]) || (nextIndex[7] < numTriangles[7]) ||
         (nextIndex[8] < numTriangles[8]) || (nextIndex[9] < numTriangles[9]))
  {
    // Choose, from which surface to draw the next triangle **********

    winningSurface = -1;
    minZ = 100000000.0;

    for (i=0; i < NUM_TRANSPARENT_SURFACES; i++)
    {
      if ((transSurface[i] != NULL) && (nextIndex[i] < numTriangles[i]))
      {
        z = transSurface[i]->triangle[ transSurface[i]->sequence[nextIndex[i]] ].distance;
        if (z < minZ)
        {
		      winningSurface = i;
          minZ = z;
        }
      }
    }

    // Draw the triangle from the choosen surface *******************

    if ((winningSurface != -1) && (transSurface[winningSurface] != NULL))
    {
      s = transSurface[winningSurface];
      i = s->sequence[ nextIndex[winningSurface] ];
		
	    // Cover ******************************************************
	    if ((winningSurface == UPPER_COVER) || (winningSurface == LOWER_COVER) ||
		      (winningSurface == LEFT_COVER)  || (winningSurface == RIGHT_COVER) ||
		      (winningSurface == EPIGLOTTIS)  || (winningSurface == UVULA))
	    {
		    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, coverMaterialAmbient);
		    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, coverMaterialDiffuse);
		    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, coverMaterialSpecular);
		    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, coverMaterialShininess);
	    }

	    // Teeth ******************************************************
	    if ((winningSurface == UPPER_TEETH) || (winningSurface == LOWER_TEETH))
	    {
		    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, teethMaterialAmbient);
		    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, teethMaterialDiffuse);
		    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, teethMaterialSpecular);
		    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, teethMaterialShininess);
	    }

	    // Lips *******************************************************
	    if ((winningSurface == UPPER_LIP) || (winningSurface == LOWER_LIP))
	    {
		    glMaterialfv(GL_FRONT, GL_AMBIENT, frontLipMaterialDiffuse);
		    glMaterialfv(GL_FRONT, GL_DIFFUSE, frontLipMaterialDiffuse);

		    glMaterialfv(GL_BACK, GL_AMBIENT, backLipMaterialDiffuse);
		    glMaterialfv(GL_BACK, GL_DIFFUSE, backLipMaterialDiffuse);
    
		    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, lipMaterialSpecular);
		    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, lipMaterialShininess);
	    }

      // Some triangles of the filling surfaces shall not be drawn

      drawTriangle = true;
      if (((winningSurface == RIGHT_COVER) && (i > 5)) ||
          ((winningSurface == LEFT_COVER) && (i > 5))) { drawTriangle = false; }

      k = (transSurface[UPPER_LIP]->numRibPoints-1)*2;
      if (((winningSurface == UPPER_LIP) && ((i % k) < 2)) ||
          ((winningSurface == LOWER_LIP) && ((i % k) < 2))) { drawTriangle = false; }

      if (drawTriangle)
      {
        glBegin(GL_TRIANGLES);
        for (k=0; k < 3; k++)
        {
          N = &s->triangle[i].cornerNormal[k];
          V = &s->vertex[ s->triangle[i].vertex[k] ].coord;
          glNormal3d(N->x, N->y, N->z);
          glVertex3d(V->x, V->y, V->z);
        }
        glEnd();
      }

      nextIndex[winningSurface]++;
    }

  }

  glDepthMask(GL_TRUE);   // z-buffer is read and write enabled
}


// ****************************************************************************
// Render only the 2D-contour lines of the model. 
// ****************************************************************************

void VocalTractPicture::render2D()
{
  int i, k;
  Surface *s = NULL;
  int rib, ribPoint;
  Point3D P, Q, R;
  bool showRibs = false;

  glPushAttrib(GL_ALL_ATTRIB_BITS);   // Put all attributes on the stack

  // Set attributes *************************************************

  glEnable(GL_LINE_SMOOTH);
  glDisable(GL_LIGHTING);
  glDisable(GL_DEPTH_TEST);

  glLineWidth(1.8f);
  glColor3f(0.0f, 0.0f, 0.0f);

  // ****************************************************************
  // Upper cover
  // ****************************************************************

  glBegin(GL_LINE_STRIP);

  // First part of the upper cover
  s = &tract->surface[VocalTract::UPPER_COVER];
  ribPoint = s->numRibPoints-1;
  for (i=0; i <= VocalTract::NUM_LARYNX_RIBS + VocalTract::NUM_PHARYNX_RIBS; i++)
  {
    Q = s->getVertex(i, ribPoint);
    glVertex3d(Q.x, Q.y, Q.z);
  }

  // Uvula
  s = &tract->surface[VocalTract::UVULA];
  ribPoint = s->numRibPoints-1;
  for (i=0; i < s->numRibs; i++)
  {
    Q = s->getVertex(i, ribPoint);
    glVertex3d(Q.x, Q.y, 0.0);
  }

  ribPoint = 0;
  for (i=s->numRibs-1; i >= 0; i--)
  {
    Q = s->getVertex(i, ribPoint);
    glVertex3d(Q.x, Q.y, 0.0);
  }

  // Last part of the upper cover
  s = &tract->surface[VocalTract::UPPER_COVER];
  ribPoint = s->numRibPoints-1;
  for (i=VocalTract::NUM_LARYNX_RIBS + VocalTract::NUM_PHARYNX_RIBS + 1; i < s->numRibs; i++)
  {
    Q = s->getVertex(i, ribPoint);
    glVertex3d(Q.x, Q.y, Q.z);
  }
  glEnd();


  if (showRibs)
  {
    glBegin(GL_LINES);
    for (i=0; i < s->numRibs; i++)
    {
      Q = s->getVertex(i, 0);
      R = s->getVertex(i, s->numRibPoints-1);
      glVertex2d(Q.x, Q.y);
      glVertex2d(R.x, R.y);
    }
    glEnd();
  }

  // ****************************************************************
  // Lower cover
  // ****************************************************************

  // Laryngeal part of the lower cover
  glBegin(GL_LINE_STRIP);

  s = &tract->surface[VocalTract::LOWER_COVER];
  ribPoint = s->numRibPoints-1;
  for (i=0; i < VocalTract::NUM_LARYNX_RIBS-1; i++)
  {
    Q = s->getVertex(i, ribPoint);
    glVertex3d(Q.x, Q.y, Q.z);
  }

  // Epiglottis
  s = &tract->surface[VocalTract::EPIGLOTTIS];
  
  ribPoint = s->numRibPoints-1;
  for (i=0; i < s->numRibs; i++)
  {
    Q = s->getVertex(i, ribPoint);
    glVertex3d(Q.x, Q.y, 0.0);
  }

  ribPoint = 0;
  for (i=s->numRibs-1; i >= 0; i--)
  {
    Q = s->getVertex(i, ribPoint);
    glVertex3d(Q.x, Q.y, 0.0);
  }

  // Rest of the lower cover
  s = &tract->surface[VocalTract::LOWER_COVER];
  ribPoint = s->numRibPoints-1;
  for (i=VocalTract::NUM_LARYNX_RIBS-1; i < s->numRibs; i++)
  {
    Q = s->getVertex(i, ribPoint);
    glVertex3d(Q.x, Q.y, Q.z);
  }
  glEnd();

  if (showRibs)
  {
    glBegin(GL_LINES);
    for (i=0; i < s->numRibs; i++)
    {
      Q = s->getVertex(i, 0);
      R = s->getVertex(i, s->numRibPoints-1);
      glVertex2d(Q.x, Q.y);
      glVertex2d(R.x, R.y);
    }
    glEnd();
  }

  // Tongue sides (1 cm off the mediosagittal plane) ****************

  glColor3f(0.8f, 0.8f, 0.8f);
  s = &tract->surface[VocalTract::TONGUE];

  const double EPSILON = 0.000001;
  double z0;
  double d;
  bool ok;

  // Left side ******************************************************
  z0 = -1.0;
  ribPoint = s->numRibPoints/2;    // Rib point in the mediosagittal plane

  glBegin(GL_LINE_STRIP);
  for (i=0; i < s->numRibs; i++)
  {
    ok = false;
    
    // Find the point at z = z0
    for (k=0; k < ribPoint; k++)
    {
      Q = s->getVertex(i, k);
      R = s->getVertex(i, k+1);
      if ((Q.z <= z0) && (R.z >= z0))
      {
        d = R.z - Q.z;
        if (d < EPSILON) { d = EPSILON; }
        P = Q + (R - Q)*(z0 - Q.z) / d;
        ok = true;
      }
    }
    if (ok == false) { P = s->getVertex(i, 0); }
    glVertex3d(P.x, P.y, 0.0);
  }
  glEnd();

  // Tongue in the mediosagittal plane ******************************

  glColor3f(0.0f, 0.0f, 0.0f);
  s = &tract->surface[VocalTract::TONGUE];
  ribPoint = s->numRibPoints/2;

  glBegin(GL_LINE_STRIP);
  for (i=0; i < s->numRibs; i++)
  {
    Q = s->getVertex(i, ribPoint);
    glVertex3d(Q.x, Q.y, Q.z);
  }
  glEnd();

  // The tongue circle as dashed line *******************************

  glColor3f(0.0f, 0.0f, 0.0f);
  double mx = tract->param[VocalTract::TCX].limitedX;
  double my = tract->param[VocalTract::TCY].limitedX;
  double rx  = tract->anatomy.tongueCenterRadiusX_cm;
  double ry  = tract->anatomy.tongueCenterRadiusY_cm;
  const int N = 32;
  double angle_rad;

  glLineStipple(1, 0x00FF);
  glEnable(GL_LINE_STIPPLE);

  glBegin(GL_LINE_STRIP);
  for (i=0; i < N; i++)
  {
    angle_rad = 2.0*M_PI*(double)i / (double)(N-1);
    glVertex2d(mx + rx*cos(angle_rad), my + ry*sin(angle_rad));
  }
  glEnd();

  glDisable(GL_LINE_STIPPLE);


  // Upper teeth ****************************************************

  glColor3f(0.0f, 0.0f, 0.0f);
  s = &tract->surface[VocalTract::UPPER_TEETH];

  ribPoint = 0;             // Upper inner edge
  glBegin(GL_LINE_STRIP);
  for (i=0; i < s->numRibs; i++)
  {
    Q = s->getVertex(i, ribPoint);
    glVertex2d(Q.x, Q.y);
  }
  glEnd();

  ribPoint = 2;             // Lower outer edge
  glBegin(GL_LINE_STRIP);
  for (i=0; i < s->numRibs-4; i++)
  {
    Q = s->getVertex(i, ribPoint);
    glVertex2d(Q.x, Q.y);
  }
  glEnd();

  // Draw the most posterior rib completely 
  rib = 0;
  glBegin(GL_LINE_STRIP);
  for (i=0; i < 2; i++)
  {
    Q = s->getVertex(rib, i);
    glVertex2d(Q.x, Q.y);
  }
  glEnd();

  // Draw the most anterior rib completely 
  rib = s->numRibs-2;
  glBegin(GL_LINE_STRIP);
  for (i=0; i < s->numRibPoints; i++)
  {
    Q = s->getVertex(rib, i);
    glVertex2d(Q.x, Q.y);
  }
  glEnd();

  // Lower teeth ****************************************************

  s = &tract->surface[VocalTract::LOWER_TEETH];

  ribPoint = 0;             // Lower inner edge
  glBegin(GL_LINE_STRIP);
  for (i=0; i < s->numRibs; i++)
  {
    Q = s->getVertex(i, ribPoint);
    glVertex2d(Q.x, Q.y);
  }
  glEnd();

  ribPoint = 2;             // Upper outer edge
  glBegin(GL_LINE_STRIP);
  for (i=0; i < s->numRibs-7; i++)
  {
    Q = s->getVertex(i, ribPoint);
    glVertex2d(Q.x, Q.y);
  }
  glEnd();

  // Draw the most posterior rib completely 
  rib = 0;
  glBegin(GL_LINE_STRIP);
  for (i=0; i < 2; i++)
  {
    Q = s->getVertex(rib, i);
    glVertex2d(Q.x, Q.y);
  }
  glEnd();

  // Draw the most anterior rib completely 
  rib = s->numRibs-2;
  glBegin(GL_LINE_STRIP);
  for (i=0; i < s->numRibPoints; i++)
  {
    Q = s->getVertex(rib, i);
    glVertex2d(Q.x, Q.y);
  }
  glEnd();

  // Upper lip ******************************************************

  s = &tract->surface[VocalTract::UPPER_LIP];
  rib = s->numRibs-1;         // Mediosagittal contour

  glBegin(GL_LINE_STRIP);
  for (i=0; i < s->numRibPoints; i++)
  {
    Q = s->getVertex(rib, i);
    glVertex2d(Q.x, Q.y);
  }
  glEnd();

  // Lower lip ******************************************************

  s = &tract->surface[VocalTract::LOWER_LIP];
  rib = s->numRibs-1;

  glBegin(GL_LINE_STRIP);
  for (i=0; i < s->numRibPoints; i++)
  {
    Q = s->getVertex(rib, i);
    glVertex2d(Q.x, Q.y);
  }
  glEnd();

  // ****************************************************************
  // Lower end of the tooth roots of the lower incisors. It can be
  // used to adjust the degree of jaw opening with respect to MR images.
  // ****************************************************************

  s = &tract->surface[VocalTract::LOWER_TEETH];

  rib = s->numRibs-2;   // Most anterior rib (lower incisor)

  glLineStipple(1, 0x0F0F);
  glEnable(GL_LINE_STIPPLE);

  P = s->getVertex(rib, 0);
  Q = s->getVertex(rib, 3);
  P = P - (Q - P);

  glBegin(GL_LINE_STRIP);
  glVertex2d(P.x, P.y - tract->anatomy.toothRootLength_cm);
  glVertex2d(Q.x, Q.y - tract->anatomy.toothRootLength_cm);
  glEnd();

  glDisable(GL_LINE_STIPPLE);

  // Einige zusätzliche Punkte **************************************
/*
  double yClose;
  Point3D onset, corner, F0, F1;

  glPointSize(3.0);
  glColor3f(0.0, 0.0, 1.0);

  glBegin(GL_POINTS);
  
  Q = tract->lipCornerPath.getControlPoint(0);
  glVertex3d(Q.x, Q.y, 0.0);

  Q = tract->lipCornerPath.getControlPoint(tract->lipCornerPath.getNumPoints()-1);
  glVertex3d(Q.x, Q.y, 0.0);

  tract->getImportantLipPoints(onset, corner, F0, F1, yClose);

  glColor3f(0.0, 0.0, 0.0);
  glVertex3d(onset.x, onset.y, 0.0);
  
  glColor3f(0.0, 0.0, 1.0);
  glVertex3d(corner.x, corner.y, 0.0);
  glVertex3d(F0.x, F0.y, 0.0);
  glVertex3d(F1.x, F1.y, 0.0);
  glEnd();
*/

  // ****************************************************************

  glPopAttrib();
}


// ****************************************************************************
// Render the model as wire frame. 
// ****************************************************************************

void VocalTractPicture::renderWireFrame()
{
  const int NUM_SURFACES = 9;
  int i, k, n;
  Surface *surface[NUM_SURFACES];
  Surface *s;
  Point3D P;
  const float CLEAR_COLOR[] = { 0.0f, 0.0f, 0.0f, 0 };

  glPushAttrib(GL_ALL_ATTRIB_BITS);   // All attributes on the stack 

  glEnable(GL_LINE_SMOOTH);
  glDisable(GL_LIGHTING);
  glEnable(GL_BLEND);      // Must be disabled to look good
  glDepthMask(GL_TRUE);    // z-buffer is read and write

  glLineWidth(1.8f);

  // Which surfaces shall be rendered ? *****************************

  if (renderBothSides)
  {
    surface[0] = &tract->surface[VocalTract::UPPER_TEETH_TWOSIDE];
    surface[1] = &tract->surface[VocalTract::LOWER_TEETH_TWOSIDE];
    surface[2] = &tract->surface[VocalTract::UPPER_COVER_TWOSIDE];
    surface[3] = &tract->surface[VocalTract::LOWER_COVER_TWOSIDE];
    surface[4] = &tract->surface[VocalTract::EPIGLOTTIS_TWOSIDE];
    surface[5] = &tract->surface[VocalTract::UVULA_TWOSIDE];
    surface[6] = &tract->surface[VocalTract::UPPER_LIP_TWOSIDE];
    surface[7] = &tract->surface[VocalTract::LOWER_LIP_TWOSIDE];
    surface[8] = &tract->surface[VocalTract::TONGUE];
  }
  else
  {
    surface[0] = &tract->surface[VocalTract::UPPER_TEETH];
    surface[1] = &tract->surface[VocalTract::LOWER_TEETH];
    surface[2] = &tract->surface[VocalTract::UPPER_COVER];
    surface[3] = &tract->surface[VocalTract::LOWER_COVER];
    surface[4] = &tract->surface[VocalTract::EPIGLOTTIS];
    surface[5] = &tract->surface[VocalTract::UVULA];
    surface[6] = &tract->surface[VocalTract::UPPER_LIP];
    surface[7] = &tract->surface[VocalTract::LOWER_LIP];
    surface[8] = &tract->surface[VocalTract::TONGUE];
  }

  // Determine minimal and maximal z-coord. of all model points. ****

  double zMin = 1000000.0;
  double zMax = -1000000.0;

  double *m;
  double z, w;

  m = modelViewMatrix;

  for (k=0; k < NUM_SURFACES; k++)
  {
    s = surface[k];

    for (i = 0; i < s->numVertices; i++)
    {
      z = m[2]*s->vertex[i].coord.x + m[6]*s->vertex[i].coord.y + m[10]*s->vertex[i].coord.z + m[14];
      w = m[3]*s->vertex[i].coord.x + m[7]*s->vertex[i].coord.y + m[11]*s->vertex[i].coord.z + m[15];
      z/= w;
      if (z < zMin) { zMin = z; }
      if (z > zMax) { zMax = z; }
    }
  }

  // Enable fogging. ************************************************

  glEnable(GL_FOG);
  glFogfv(GL_FOG_COLOR, CLEAR_COLOR);
  glFogf(GL_FOG_MODE, GL_LINEAR);
  glFogf(GL_FOG_START, (GLfloat)(-zMax - 0.0*(zMax - zMin)));
  glFogf(GL_FOG_END,   (GLfloat)(-zMin + 0.5*(zMax - zMin)));

  // ****************************************************************
  // The 3D-vocal tract model.
  // ****************************************************************

  for (n=0; n < NUM_SURFACES; n++)
  {
    s = surface[n];

    // Determine the grid color. ************************************

    if (n == 0)
    {
      glColor3f(1.0, 1.0, 1.0);
    }
    else
    if (n == 2)
    {
      glColor3f(1.0f, 0.8f, 0.1f);
    }
    else
    // Number 6 and 7 are the lips
    if (n == 6)
    {
//      glColor3f(0.5f, 0.5f, 1.0f);
      glColor3f(1.0f, 0.3f, 0.15f);
    }
    else
    // Number 8 is the tongue
    if (n == 8)
    {
      glColor3f(1.0f, 0.3f, 0.15f);
    }

    // Paint the main and cross lines. ******************************

    for (i=0; i < s->numRibs; i++)
    {
      glBegin(GL_LINE_STRIP);
      for (k=0; k < s->numRibPoints; k++)
      {
        P = s->getVertex(i, k);
        glVertex3d(P.x, P.y, P.z);
      }
      glEnd();
    }

    for (i=0; i < s->numRibPoints; i++)
    {
      glBegin(GL_LINE_STRIP);
      for (k=0; k < s->numRibs; k++)
      {
        P = s->getVertex(k, i);
        glVertex3d(P.x, P.y, P.z);
      }
      glEnd();
    }
  }

  // ****************************************************************
  // Paint the radiation semi-sphere.
  // ****************************************************************

  if (false)
  {
    s = &tract->surface[VocalTract::RADIATION];
    glColor3f(1.0f, 1.0f, 1.0f);

    for (i=0; i < s->numRibs; i++)
    {
      glBegin(GL_LINE_STRIP);
      for (k=0; k < s->numRibPoints; k++)
      {
        P = s->getVertex(i, k);
        glVertex3d(P.x, P.y, P.z);
      }
      glEnd();
    }

    for (i=0; i < s->numRibPoints; i++)
    {
      glBegin(GL_LINE_STRIP);
      for (k=0; k < s->numRibs; k++)
      {
        P = s->getVertex(k, i);
        glVertex3d(P.x, P.y, P.z);
      }
      glEnd();
    }
  }

  glPopAttrib();
}


// ****************************************************************************
/// Returns a tooltip with the parameter values corresponding to the given
/// control point.
// ****************************************************************************

wxString VocalTractPicture::getToolTipText(int controlPointIndex)
{
  wxString st = "";
  
  if (controlPointIndex == CP_VELUM)
  {
    st = wxString::Format("%s=%2.2f\n%s=%2.2f", 
      tract->param[VocalTract::VS].abbr.c_str(), tract->param[VocalTract::VS].x, 
      tract->param[VocalTract::VO].abbr.c_str(), tract->param[VocalTract::VO].x);
  }
  else
  if (controlPointIndex == CP_HYOID)
  {
    st = wxString::Format("%s=%2.2f\n%s=%2.2f", 
      tract->param[VocalTract::HX].abbr.c_str(), tract->param[VocalTract::HX].x,
      tract->param[VocalTract::HY].abbr.c_str(), tract->param[VocalTract::HY].x);
  }
  else
  if (controlPointIndex == CP_JAW)
  {
    st = wxString::Format("%s=%2.2f\n%s=%2.2f", 
      tract->param[VocalTract::JA].abbr.c_str(), tract->param[VocalTract::JA].x,
      tract->param[VocalTract::JX].abbr.c_str(), tract->param[VocalTract::JX].x);
  }
  else
  if (controlPointIndex == CP_LIP_CORNER)
  {
    st = wxString::Format("%s=%2.2f", 
      tract->param[VocalTract::LP].abbr.c_str(), tract->param[VocalTract::LP].x);
  }
  else
  if (controlPointIndex == CP_LIP_DISTANCE)
  {
    st = wxString::Format("%s=%2.2f", 
      tract->param[VocalTract::LD].abbr.c_str(), tract->param[VocalTract::LD].x);
  }
  else
  if (controlPointIndex == CP_TONGUE_CENTER)
  {
    st = wxString::Format("%s=%2.2f\n%s=%2.2f", 
      tract->param[VocalTract::TCX].abbr.c_str(), tract->param[VocalTract::TCX].x,
      tract->param[VocalTract::TCY].abbr.c_str(), tract->param[VocalTract::TCY].x);
  }
  else
  if (controlPointIndex == CP_TONGUE_TIP)
  {
    st = wxString::Format("%s=%2.2f\n%s=%2.2f", 
      tract->param[VocalTract::TTX].abbr.c_str(), tract->param[VocalTract::TTX].x,
      tract->param[VocalTract::TTY].abbr.c_str(), tract->param[VocalTract::TTY].x);
  }
  else
  if (controlPointIndex == CP_TONGUE_BLADE)
  {
    st = wxString::Format("%s=%2.2f\n%s=%2.2f", 
      tract->param[VocalTract::TBX].abbr.c_str(), tract->param[VocalTract::TBX].x,
      tract->param[VocalTract::TBY].abbr.c_str(), tract->param[VocalTract::TBY].x);
  }
  else
  if (controlPointIndex == CP_TONGUE_BACK)
  {
    st = wxString::Format("%s=%2.2f\n%s=%2.2f", 
      tract->param[VocalTract::TRX].abbr.c_str(), tract->param[VocalTract::TRX].x,
      tract->param[VocalTract::TRY].abbr.c_str(), tract->param[VocalTract::TRY].x);
  }

  return st;
}


// ****************************************************************************
// ****************************************************************************

int VocalTractPicture::getControlPointUnderMouse(int mx, int my)
{
  int i;
  double screenX, screenY, screenZ;
  
  int controlPointUnderMouse = -1;

  for (i=0; i < NUM_CONTROLPOINTS; i++)
  {
    projection(controlPoint[i], screenX, screenY, screenZ);

    if ((mx >= (int)screenX-5) && (mx <= (int)screenX+5) &&
        (my >= (int)screenY-5) && (my <= (int)screenY+5))
    {
      controlPointUnderMouse = i;
    }
  }

  // The control point for the tongue root is not considered when
  // automaticTongueRootCalc == true.
  if ((controlPointUnderMouse == CP_TONGUE_BACK) && (tract->anatomy.automaticTongueRootCalc))
  {
    controlPointUnderMouse = -1;
  }
  
  return controlPointUnderMouse;
}


// ****************************************************************************
// ****************************************************************************

int VocalTractPicture::getEmaPointUnderMouse(int mx, int my)
{
  int i;
  double screenX, screenY, screenZ;
  Point3D P;
  
  if (showEmaPoints == false)
  {
    return -1;
  }

  int emaPointUnderMouse = -1;

  for (i=0; i < (int)tract->emaPoints.size(); i++)
  {
    P = tract->getEmaPointCoord(i);
    projection(P, screenX, screenY, screenZ);

    if ((mx >= (int)screenX-5) && (mx <= (int)screenX+5) &&
        (my >= (int)screenY-5) && (my <= (int)screenY+5))
    {
      emaPointUnderMouse = i;
    }
  }

  return emaPointUnderMouse;
}


// ****************************************************************************
/// Handler for the paint event of this picture.
// ****************************************************************************

void VocalTractPicture::OnPaint(wxPaintEvent &event)
{
  // Always create the wxPaintDC object in the paint event
  wxPaintDC dc(this);

  // Do the rendering
  display();
}

// ****************************************************************************
/// Intercept this event to avoid flickering.
// ****************************************************************************

void VocalTractPicture::OnEraseBackground(wxEraseEvent &event)
{
  // Do nothing here - especially, DO NOT class event.Skip() !!
}

// ****************************************************************************
/// Let the picture berepainted when the image is resized.
// ****************************************************************************

void VocalTractPicture::OnResize(wxSizeEvent &event)
{
  // The window flag wxFULL_REPAINT_ON_RESIZE causes the invalidation of the
  // whole window always when it is resized. So we can call Update() for
  // immediate repainting.
  this->Refresh();   
}


// ****************************************************************************
/// Processes all events of the mouse. It is used to turn and move the vocal 
/// tract and to move the control points.
// ****************************************************************************

void VocalTractPicture::OnMouseChanged(wxMouseEvent &event)
{
  static int lastMx = 0;
  static int lastMy = 0;
  
  int mx = event.GetX();
  int my = event.GetY();

  // ****************************************************************
  // Is one of the control points under the mouse?
  // ****************************************************************

  if (event.Moving())
  {
    int controlPointUnderMouse = getControlPointUnderMouse(mx, my);
    if ((controlPointUnderMouse != -1) || (selectedControlPoint != -1))
    {
      this->SetCursor( wxCursor(wxCURSOR_HAND) );
//      this->SetCursor( wxCursor(wxCURSOR_SIZING) );
//      this->SetCursor( wxCursor(wxCURSOR_LEFT_BUTTON) );
    }
    else
    {
      this->SetCursor( wxCursor(wxCURSOR_ARROW) );
    }
  }


  // ****************************************************************
  // ****************************************************************

  if (event.LeftDClick())
  {
    // Output the image size on the console
    int width, height;
    this->GetSize(&width, &height);
    wxString st = wxString::Format("Image size: %d x %d", width, height);
    wxTipWindow *tipWindow = new wxTipWindow(this, st);
  }

  // ****************************************************************
  // The mouse just left the picture region.
  // ****************************************************************

  if (event.Leaving())
  {
    selectedControlPoint = -1;
    this->UnsetToolTip();
  }
  else

  // ****************************************************************
  // Left mouse button was just pressed
  // ****************************************************************

  if (event.LeftDown())
  {
    // Has a control point been selected ?
    if ((selectedControlPoint == -1) && (showControlPoints))
    {
      selectedControlPoint = getControlPointUnderMouse(mx, my);
    }

    lastMx = mx;
    lastMy = my;
  }
  else

  // ****************************************************************
  // Left mouse button was just released
  // ****************************************************************

  if (event.LeftUp())
  {
    selectedControlPoint = -1;
  }
  else

  // ****************************************************************
  // Right mouse button was just pressed
  // ****************************************************************

  if (event.RightDown())
  {
    lastMx = mx;
    lastMy = my;

    // If a control point is under the mouse, show a tip window
    // with the associated parameters.

    int controlPointUnderMouse = getControlPointUnderMouse(mx, my);
    int emaPointUnderMouse = getEmaPointUnderMouse(mx, my);

    if (controlPointUnderMouse != -1)
    {
      wxString st = getToolTipText(controlPointUnderMouse);
      if (st != "")
      {
        wxTipWindow *tipWindow = NULL;
        tipWindow = new wxTipWindow(this, st);
      }
    }
    else
    if (emaPointUnderMouse != -1)
    {
      string name = tract->emaPoints[emaPointUnderMouse].name;
      wxTipWindow *tipWindow = NULL;
      tipWindow = new wxTipWindow(this, wxString(name));
    }

  }

  // ****************************************************************
  // The mouse was moved with the left or right button pressed.
  // ****************************************************************

  if (event.Dragging())
  {
    // ****************************************************************
    // The left mouse button is pressed -> Turn the model or change
    // a control point position or move the background image.
    // ****************************************************************

    if (event.LeftIsDown())
    {
      if (selectedControlPoint != -1)
      {
#ifdef __linux__
        // On Linux, all pixel coordinates need to be scaled by the current
        // display scaling factor before passing them to OpenGL
        setControlPoint(selectedControlPoint, mx * GetContentScaleFactor(), my * GetContentScaleFactor());;
#else
        setControlPoint(selectedControlPoint, mx, my);
#endif
              
        // Re-calculate the vocal tract model
        tract->calculateAll();

        // Update all pictures on the vocal tract page including this!
        wxCommandEvent event(updateRequestEvent);
        event.SetInt(UPDATE_VOCAL_TRACT);
        wxPostEvent(updateEventReceiver, event);
      }
      else
      if ((poster != NULL) && (showPoster) && (posterEditing))
      {
        // Move the poster center around.

        double left_cm, right_cm, bottom_cm, top_cm;
        get2DRegion(left_cm, right_cm, bottom_cm, top_cm);

        int pictureWidth_pix, pictureHeight_pix;
        this->GetSize(&pictureWidth_pix, &pictureHeight_pix);

        double factor = (right_cm - left_cm) / (double)pictureWidth_pix;

        posterCenterX_cm+= (mx - lastMx)*factor;
        posterCenterY_cm-= (my - lastMy)*factor;

        if (posterCenterX_cm < left_cm)
        {
          posterCenterX_cm = left_cm;
        }
        
        if (posterCenterX_cm > right_cm)
        {
          posterCenterX_cm = right_cm;
        }
        
        if (posterCenterY_cm < bottom_cm)
        {
          posterCenterY_cm = bottom_cm;
        }

        if (posterCenterY_cm > top_cm)
        {
          posterCenterY_cm = top_cm;
        }

        this->Refresh();
      }
      else
      {
        // Turning around the y- und z-axis
        if (renderMode != RM_2D)
        {
          yRotation_deg+= (mx - lastMx)*0.5;
          zRotation_deg-= (my - lastMy)*0.5;
          this->Refresh();
        }
      }
    }
    else

    // ****************************************************************
    // The right mouse button is pressed -> Move the model to the fornt
    // or back.
    // ****************************************************************

    if (event.RightIsDown())
    {
      if ((poster != NULL) && (showPoster) && (posterEditing))
      {
        posterScalingFactor+= (my - lastMy)*0.004;          
        
        if (posterScalingFactor < 0.5)
        {
          posterScalingFactor = 0.5;
        }
        
        if (posterScalingFactor > 2.0)
        {
          posterScalingFactor = 2.0;
        }

        this->Refresh();
      }
      else

      if (renderMode != RM_2D)
      {
        distance_cm-= (my - lastMy)*0.08;
        if (distance_cm < 4.0)  { distance_cm = 4.0; }
        if (distance_cm > 50.0) { distance_cm = 50.0; }
        this->Refresh();
      }
    }

  }

  lastMx = mx;
  lastMy = my;
}

// ****************************************************************************
