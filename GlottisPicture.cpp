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

#include "GlottisPicture.h"
#include <typeinfo>


// ****************************************************************************
/// Construcor. Passes the parent parameter.
// ****************************************************************************

GlottisPicture::GlottisPicture(wxWindow *parent, Glottis *glottis) : BasicPicture(parent)
{
  this->glottis = glottis;

  viewFactor = 1.0;
  originU = this->GetSize().GetWidth() / 2;
}


// ****************************************************************************
// ****************************************************************************

void GlottisPicture::draw(wxDC &dc)
{
  if (typeid(*glottis) == typeid(GeometricGlottis))
  {
    drawGeometricGlottis(dc);
  }
  else
  if (typeid(*glottis) == typeid(TwoMassModel))
  {
    drawTwoMassModel(dc);
  }
  else
  if (typeid(*glottis) == typeid(TriangularGlottis))
  {
    drawTriangularGlottis(dc);
  }
  else
  {
    dc.SetBackground(*wxWHITE_BRUSH);
    dc.Clear();
    dc.DrawText("Unknown glottis type!", 0, 0);
  }
}


// ****************************************************************************
/// Draws the geometric glottis model in isometric projection.
// ****************************************************************************

void GlottisPicture::drawGeometricGlottis(wxDC &dc)
{
  // ****************************************************************
  // Fill the background.
  // ****************************************************************

  dc.SetBackground(*wxWHITE_BRUSH);
  dc.Clear();

  // ****************************************************************
  // Initialize the viewport.
  // MIN and MAX values are in cm.
  // ****************************************************************

  const double X_MIN = -1.0;
  const double X_MAX = 1.0;
  const double Y_MIN = 0.0;
  const double Y_MAX = 1.5;
  const double Z_MIN = -2.0;
  const double Z_MAX = 0.0;

  initView(X_MIN, X_MAX, Y_MIN, Y_MAX, Z_MIN, Z_MAX);

  // Mark the 3D-bounding box with gray lines

  dc.SetPen(wxPen(*wxLIGHT_GREY, lineWidth));
  dc.SetBrush(wxBrush(*wxBLACK, wxBRUSHSTYLE_TRANSPARENT));

  Point3d p1[4] =
  {
    Point3d(X_MIN, Y_MIN, Z_MIN),
    Point3d(X_MAX, Y_MIN, Z_MIN),
    Point3d(X_MAX, Y_MAX, Z_MIN),
    Point3d(X_MIN, Y_MAX, Z_MIN)
  };
  //  drawPolygon(dc, 4, p1);

  Point3d p2[4] =
  {
    Point3d(X_MIN, Y_MIN, Z_MAX),
    Point3d(X_MAX, Y_MIN, Z_MAX),
    Point3d(X_MAX, Y_MAX, Z_MAX),
    Point3d(X_MIN, Y_MAX, Z_MAX)
  };
  //  drawPolygon(dc, 4, p2);


  // ****************************************************************
  // Determine the coordinates of 3d points.
  // ****************************************************************

  GeometricGlottis *g = (GeometricGlottis*)glottis;

  const int NUM_EDGE_POINTS = 20;
  int i;
  double t;
  double x, y, z;

  double endX[2];
  double cordX[2];
  endX[0] = g->controlParam[GeometricGlottis::LOWER_END_X].x;
  endX[1] = g->controlParam[GeometricGlottis::UPPER_END_X].x;
  cordX[0] = g->derivedParam[GeometricGlottis::LOWER_CORD_X].x;
  cordX[1] = g->derivedParam[GeometricGlottis::UPPER_CORD_X].x;
  double length = g->derivedParam[GeometricGlottis::LENGTH].x;
  double thickness = g->derivedParam[GeometricGlottis::THICKNESS].x;
  double amplitude = g->derivedParam[GeometricGlottis::AMPLITUDE].x;

  Point3d upperLeftEdge[NUM_EDGE_POINTS];
  Point3d upperRightEdge[NUM_EDGE_POINTS];
  Point3d lowerLeftEdge[NUM_EDGE_POINTS];
  Point3d lowerRightEdge[NUM_EDGE_POINTS];
  Point3d slit[NUM_EDGE_POINTS * 2];

  for (i = 0; i < NUM_EDGE_POINTS; i++)
  {
    // Right lower edge
    t = (double)i / (double)(NUM_EDGE_POINTS - 1);    // 0 <= t <= 1
    x = endX[0] * t + cordX[0] * sin(t*M_PI);
    if (x < 0.0)
    {
      x = 0.0;
    }
    y = thickness;
    z = -t*length;
    lowerRightEdge[i] = Point3d(x, y, z);

    // Left lower edge
    x = -x;
    lowerLeftEdge[i] = Point3d(x, y, z);

    // Right upper edge
    t = (double)i / (double)(NUM_EDGE_POINTS - 1);    // 0 <= t <= 1
    x = endX[1] * t + cordX[1] * sin(t*M_PI);
    if (x < 0.0)
    {
      x = 0.0;
    }
    y = 0.0;
    z = -t*length;
    upperRightEdge[i] = Point3d(x, y, z);

    // Left upper edge
    x = -x;
    upperLeftEdge[i] = Point3d(x, y, z);
  }

  // Fill the area of the glottis

  dc.SetBrush(*wxLIGHT_GREY_BRUSH);
  dc.SetPen(*wxTRANSPARENT_PEN);

  for (i = 0; i < NUM_EDGE_POINTS; i++)
  {
    slit[i] = upperLeftEdge[i];
    slit[NUM_EDGE_POINTS + i] = upperRightEdge[NUM_EDGE_POINTS - 1 - i];
  }
  drawPolygon(dc, NUM_EDGE_POINTS * 2, slit);

  for (i = 0; i < NUM_EDGE_POINTS; i++)
  {
    slit[i] = lowerLeftEdge[i];
    slit[NUM_EDGE_POINTS + i] = lowerRightEdge[NUM_EDGE_POINTS - 1 - i];
  }
  drawPolygon(dc, NUM_EDGE_POINTS * 2, slit);

  // Draw the outline of the glottis

  dc.SetPen(wxPen(*wxBLACK, lineWidth));
  drawLines(dc, NUM_EDGE_POINTS, lowerLeftEdge);
  drawLines(dc, NUM_EDGE_POINTS, lowerRightEdge);
  drawLines(dc, NUM_EDGE_POINTS, upperLeftEdge);
  drawLines(dc, NUM_EDGE_POINTS, upperRightEdge);

  if (endX[0] < 0.0)
  {
    endX[0] = 0.0;
  }
  if (endX[1] < 0.0)
  {
    endX[1] = 0.0;
  }

  drawLine(dc, endX[0], thickness, -length, endX[1], 0.0, -length);
  drawLine(dc, -endX[0], thickness, -length, -endX[1], 0.0, -length);
  drawLine(dc, 0.0, thickness, 0.0, 0.0, 0.0, 0.0);

  // ****************************************************************
  // Draw the thyroid outline
  // ****************************************************************

  const int NUM_THYROID_POINTS = 7;
  Point3d thyroidOutline[NUM_THYROID_POINTS] =
  {
    Point3d(-1.5, 0.0, -1.5),
    Point3d(-1.3, 0.0, -0.8),
    Point3d(-1.0, 0.0, -0.5),
    Point3d(0.0, 0.0, 0.0),
    Point3d(1.0, 0.0, -0.5),
    Point3d(1.3, 0.0, -0.8),
    Point3d(1.5, 0.0, -1.5),
  };

  drawLines(dc, NUM_THYROID_POINTS, thyroidOutline);

  // ****************************************************************
  // Draw the chink between the arytenoids.
  // ****************************************************************

  double chinkLength = g->staticParam[GeometricGlottis::CHINK_LENGTH].x;
  double chinkWidth = g->derivedParam[GeometricGlottis::CHINK_WIDTH].x;

  const int NUM_CHINK_POINTS = 4;
  Point3d chinkTop[NUM_CHINK_POINTS] =
  {
    Point3d(-0.5*chinkWidth, 0.0, -length),
    Point3d(-0.5*chinkWidth, 0.0, -length - chinkLength),
    Point3d(0.5*chinkWidth, 0.0, -length - chinkLength),
    Point3d(0.5*chinkWidth, 0.0, -length),
  };

  Point3d chinkBottom[NUM_CHINK_POINTS];
  for (i = 0; i < NUM_CHINK_POINTS; i++)
  {
    chinkBottom[i].x = chinkTop[i].x;
    chinkBottom[i].y = thickness;
    chinkBottom[i].z = chinkTop[i].z;
  }

  // Fill the chink area

  dc.SetBrush(*wxLIGHT_GREY_BRUSH);
  dc.SetPen(*wxTRANSPARENT_PEN);
  drawPolygon(dc, NUM_CHINK_POINTS, chinkTop);
  drawPolygon(dc, NUM_CHINK_POINTS, chinkBottom);

  // Draw the outline of the chink

  dc.SetPen(wxPen(*wxBLACK, lineWidth));

  drawLines(dc, NUM_CHINK_POINTS, chinkTop);
  drawLine(dc, -endX[1], 0.0, -length, chinkTop[0].x, chinkTop[0].y, chinkTop[0].z);
  drawLine(dc, endX[1], 0.0, -length, chinkTop[3].x, chinkTop[3].y, chinkTop[3].z);

  drawLines(dc, NUM_CHINK_POINTS, chinkBottom);
  drawLine(dc, -endX[0], thickness, -length, chinkBottom[0].x, chinkBottom[0].y, chinkBottom[0].z);
  drawLine(dc, endX[0], thickness, -length, chinkBottom[3].x, chinkBottom[3].y, chinkBottom[3].z);
}


// ****************************************************************************
/// Draws the two-mass-model in isometric projection.
// ****************************************************************************

void GlottisPicture::drawTwoMassModel(wxDC &dc)
{
  // ****************************************************************
  // Fill the background.
  // ****************************************************************

  dc.SetBackground(*wxWHITE_BRUSH);
  dc.Clear();

  // ****************************************************************
  // Initialize the viewport.
  // MIN and MAX values are in cm.
  // ****************************************************************

  const double X_MIN = -0.6;
  const double X_MAX =  0.6;
  const double Y_MIN =  0.0;
  const double Y_MAX =  1.0;
  const double Z_MIN =  0.0;
  const double Z_MAX =  1.7;

  initView(X_MIN, X_MAX, Y_MIN, Y_MAX, Z_MIN, Z_MAX);

  // Mark the 3D-bounding box with gray lines

  dc.SetPen(wxPen(*wxLIGHT_GREY, lineWidth));
  dc.SetBrush( wxBrush(*wxBLACK, wxBRUSHSTYLE_TRANSPARENT) );
  
  Point3d p1[4] =
  {
    Point3d(X_MIN, Y_MIN, Z_MIN),
    Point3d(X_MAX, Y_MIN, Z_MIN),
    Point3d(X_MAX, Y_MAX, Z_MIN),
    Point3d(X_MIN, Y_MAX, Z_MIN)
  };

  Point3d p2[4] =
  {
    Point3d(X_MIN, Y_MIN, Z_MAX),
    Point3d(X_MAX, Y_MIN, Z_MAX),
    Point3d(X_MAX, Y_MAX, Z_MAX),
    Point3d(X_MIN, Y_MAX, Z_MAX)
  };


  // ****************************************************************
  // Get some glottis dimensions.
  // ****************************************************************

  TwoMassModel *g = (TwoMassModel*)glottis;

  // Constant width of the masses = 2 mm
  const double W = 0.3;
  double length = g->derivedParam[TwoMassModel::CURRENT_LENGTH].x;
  
  double x[2];
  x[0] = g->derivedParam[TwoMassModel::ABSOLUTE_DISP_1].x;
  x[1] = g->derivedParam[TwoMassModel::ABSOLUTE_DISP_2].x;
  
  double d[2];
  d[0] = g->derivedParam[TwoMassModel::CURRENT_THICKNESS_1].x;
  d[1] = g->derivedParam[TwoMassModel::CURRENT_THICKNESS_2].x;

  double chinkLength = g->staticParam[TwoMassModel::CHINK_LENGTH].x;
  double chinkWidth  = g->derivedParam[TwoMassModel::CHINK_WIDTH].x;

  // ****************************************************************
  // Lower masses.
  // ****************************************************************

  Point3d lowerFrontalPlane[4] =
  {
    Point3d(0.0, d[1], 0.0),
    Point3d(W,   d[1], 0.0),
    Point3d(W,   d[0]+d[1], 0.0),
    Point3d(0.0, d[0]+d[1], 0.0)
  };

  Point3d lowerSagittalPlane[4] =
  {
    Point3d(W, d[1], 0.0),
    Point3d(W, d[1], length),
    Point3d(W, d[0]+d[1], length),
    Point3d(W, d[0]+d[1], 0.0)
  };

  Point3d lowerTransversePlane[4] =
  {
    Point3d(0.0, d[1], 0.0),
    Point3d(0.0, d[1], length),
    Point3d(W, d[1], length),
    Point3d(W, d[1], 0.0)
  };

  Point3d lowerChinkContour[6] =
  {
    Point3d(-x[0], d[1]+d[0], 0.0),
    Point3d(-0.5*chinkWidth, d[1]+d[0], 0.0),
    Point3d(-0.5*chinkWidth, d[1]+d[0], -chinkLength),
    Point3d(0.5*chinkWidth, d[1]+d[0], -chinkLength),
    Point3d(0.5*chinkWidth, d[1]+d[0], 0.0),
    Point3d(x[0], d[1]+d[0], 0.0),
  };

  Point3d lowerArea[4] =
  {
    Point3d(-x[0], d[0]+d[1], 0.0),
    Point3d(-x[0], d[0]+d[1], length),
    Point3d(x[0], d[0]+d[1], length),
    Point3d(x[0], d[0]+d[1], 0.0),
  };

  Point3d lowerChinkArea[4] =
  {
    Point3d(-0.5*chinkWidth, d[0]+d[1], 0.0),
    Point3d(-0.5*chinkWidth, d[0]+d[1], -chinkLength),
    Point3d(0.5*chinkWidth, d[0]+d[1], -chinkLength),
    Point3d(0.5*chinkWidth, d[0]+d[1], 0.0),
  };

  // Draw the lower area
  
  dc.SetPen(*wxTRANSPARENT_PEN);
  dc.SetBrush(*wxLIGHT_GREY_BRUSH);
  drawPolygon(dc, 4, lowerArea);
  drawPolygon(dc, 4, lowerChinkArea);

  // Draw left lower mass
  
  dc.SetPen(wxPen(*wxBLACK, lineWidth));
  dc.SetBrush(*wxWHITE_BRUSH);

  drawPolygon(dc, 4, lowerFrontalPlane, -W-x[0], 0.0, 0.0);
  drawPolygon(dc, 4, lowerSagittalPlane, -W-x[0], 0.0, 0.0);
  drawPolygon(dc, 4, lowerTransversePlane, -W-x[0], 0.0, 0.0);

  // Draw right lower mass
  
  dc.SetPen(wxPen(*wxBLACK, lineWidth));
  dc.SetBrush(*wxWHITE_BRUSH);

  drawPolygon(dc, 4, lowerFrontalPlane, x[0], 0.0, 0.0);
  drawPolygon(dc, 4, lowerSagittalPlane, x[0], 0.0, 0.0);
  drawPolygon(dc, 4, lowerTransversePlane, x[0], 0.0, 0.0);

  drawLines(dc, 6, lowerChinkContour);

  // ****************************************************************
  // Upper masses.
  // ****************************************************************

  Point3d upperFrontalPlane[4] =
  {
    Point3d(0.0, 0.0, 0.0),
    Point3d(W,   0.0, 0.0),
    Point3d(W,   d[1], 0.0),
    Point3d(0.0, d[1], 0.0)
  };

  Point3d upperSagittalPlane[4] =
  {
    Point3d(W, 0.0, 0.0),
    Point3d(W, 0.0, length),
    Point3d(W, d[1], length),
    Point3d(W, d[1], 0.0)
  };

  Point3d upperTransversePlane[4] =
  {
    Point3d(0.0, 0.0, 0.0),
    Point3d(0.0, 0.0, length),
    Point3d(W, 0.0, length),
    Point3d(W, 0.0, 0.0)
  };

  Point3d upperChinkContour[6] =
  {
    Point3d(-x[1], 0.0, 0.0),
    Point3d(-0.5*chinkWidth, 0.0, 0.0),
    Point3d(-0.5*chinkWidth, 0.0, -chinkLength),
    Point3d(0.5*chinkWidth, 0.0, -chinkLength),
    Point3d(0.5*chinkWidth, 0.0, 0.0),
    Point3d(x[1], 0.0, 0.0),
  };

  // Draw left upper mass
  
  dc.SetPen(wxPen(*wxBLACK, lineWidth));
  dc.SetBrush(*wxWHITE_BRUSH);

  drawPolygon(dc, 4, upperFrontalPlane, -W-x[1], 0.0, 0.0);
  drawPolygon(dc, 4, upperSagittalPlane, -W-x[1], 0.0, 0.0);
  drawPolygon(dc, 4, upperTransversePlane, -W-x[1], 0.0, 0.0);

  // Draw right upper mass
  
  dc.SetPen(wxPen(*wxBLACK, lineWidth));
  dc.SetBrush(*wxWHITE_BRUSH);

  drawPolygon(dc, 4, upperFrontalPlane, x[1], 0.0, 0.0);
  drawPolygon(dc, 4, upperSagittalPlane, x[1], 0.0, 0.0);
  drawPolygon(dc, 4, upperTransversePlane, x[1], 0.0, 0.0);

  drawLines(dc, 6, upperChinkContour);

}

// ****************************************************************************
/// Draws the triangular two-mass-model in isometric projection.
// ****************************************************************************

void GlottisPicture::drawTriangularGlottis(wxDC &dc)
{
  // ****************************************************************
  // Fill the background.
  // ****************************************************************

  dc.SetBackground(*wxWHITE_BRUSH);
  dc.Clear();

  // ****************************************************************
  // Initialize the viewport.
  // MIN and MAX values are in cm.
  // ****************************************************************

  const double X_MIN = -0.6;
  const double X_MAX =  0.6;
  const double Y_MIN =  0.0;
  const double Y_MAX =  0.6;
  const double Z_MIN =  0.0;
  const double Z_MAX =  1.4;

  initView(X_MIN, X_MAX, Y_MIN, Y_MAX, Z_MIN, Z_MAX);

  // Mark the 3D-bounding box with gray lines

  dc.SetPen(wxPen(*wxLIGHT_GREY, lineWidth));
  dc.SetBrush( wxBrush(*wxBLACK, wxBRUSHSTYLE_TRANSPARENT) );
  
  Point3d p1[4] =
  {
    Point3d(X_MIN, Y_MIN, Z_MIN),
    Point3d(X_MAX, Y_MIN, Z_MIN),
    Point3d(X_MAX, Y_MAX, Z_MIN),
    Point3d(X_MIN, Y_MAX, Z_MIN)
  };
//  drawPolygon(dc, 4, p1);

  Point3d p2[4] =
  {
    Point3d(X_MIN, Y_MIN, Z_MAX),
    Point3d(X_MAX, Y_MIN, Z_MAX),
    Point3d(X_MAX, Y_MAX, Z_MAX),
    Point3d(X_MIN, Y_MAX, Z_MAX)
  };
//  drawPolygon(dc, 4, p2);


  // ****************************************************************
  // Get some glottis dimensions.
  // ****************************************************************

  TriangularGlottis *g = (TriangularGlottis*)glottis;

  // Constant width of the masses = 2 mm
  const double W = 0.3;
  double length = g->derivedParam[TriangularGlottis::CURRENT_LENGTH].x;
  
  double backX[2];
  backX[0] = g->controlParam[TriangularGlottis::REST_DISP_1].x + g->derivedParam[TriangularGlottis::RELATIVE_DISP_1].x;
  backX[1] = g->controlParam[TriangularGlottis::REST_DISP_2].x + g->derivedParam[TriangularGlottis::RELATIVE_DISP_2].x;
  
  double frontX[2];

  // When the vocal folds are pressed together, they are assumed to 
  // be parallel, and otherwise triangular.
  
  if (g->controlParam[TriangularGlottis::REST_DISP_1].x < 0.0)
  {
    frontX[0] = backX[0];
  }
  else
  {
    frontX[0] = g->derivedParam[TriangularGlottis::RELATIVE_DISP_1].x;
  }
  
  if (g->controlParam[TriangularGlottis::REST_DISP_2].x < 0.0)
  {
    frontX[1] = backX[1];
  }
  else
  {
    frontX[1] = g->derivedParam[TriangularGlottis::RELATIVE_DISP_2].x;
  }


  double d[2];
  d[0] = g->derivedParam[TriangularGlottis::CURRENT_THICKNESS_1].x;
  d[1] = g->derivedParam[TriangularGlottis::CURRENT_THICKNESS_2].x;

  double chinkArea = g->controlParam[TriangularGlottis::ARY_AREA].x;
  if (chinkArea < 0.0)
  {
    chinkArea = 0.0;
  }
  double chinkLength = sqrt(chinkArea);
  double chinkWidth  = chinkLength;

  double inletRadius = sqrt(2.5 / M_PI);
  double inletLength = g->staticParam[TriangularGlottis::INLET_LENGTH].x;
  double outletRadius = sqrt(2.5 / M_PI);
  double outletLength = g->staticParam[TriangularGlottis::OUTLET_LENGTH].x;


  // ****************************************************************
  // Lower masses.
  // ****************************************************************

  Point3d lowerFrontalPlane[4] =
  {
    Point3d(0.0, d[1], 0.0),
    Point3d(W,   d[1], 0.0),
    Point3d(W,   d[0]+d[1], 0.0),
    Point3d(0.0, d[0]+d[1], 0.0)
  };

  Point3d lowerRightSagittalPlane[4] =
  {
    Point3d(backX[0]+W, d[1], 0.0),
    Point3d(frontX[0]+W, d[1], length),
    Point3d(frontX[0]+W, d[0]+d[1], length),
    Point3d(backX[0]+W, d[0]+d[1], 0.0)
  };

  Point3d lowerRightTransversePlane[4] =
  {
    Point3d(backX[0], d[1], 0.0),
    Point3d(frontX[0], d[1], length),
    Point3d(frontX[0]+W, d[1], length),
    Point3d(backX[0]+W, d[1], 0.0)
  };

  Point3d lowerLeftSagittalPlane[4] =
  {
    Point3d(-backX[0], d[1], 0.0),
    Point3d(-frontX[0], d[1], length),
    Point3d(-frontX[0], d[0]+d[1], length),
    Point3d(-backX[0], d[0]+d[1], 0.0)
  };

  Point3d lowerLeftTransversePlane[4] =
  {
    Point3d(-backX[0], d[1], 0.0),
    Point3d(-frontX[0], d[1], length),
    Point3d(-frontX[0]-W, d[1], length),
    Point3d(-backX[0]-W, d[1], 0.0)
  };

  Point3d lowerChinkContour[6] =
  {
    Point3d(-backX[0], d[1]+d[0], 0.0),
    Point3d(-0.5*chinkWidth, d[1]+d[0], 0.0),
    Point3d(-0.5*chinkWidth, d[1]+d[0], -chinkLength),
    Point3d(0.5*chinkWidth, d[1]+d[0], -chinkLength),
    Point3d(0.5*chinkWidth, d[1]+d[0], 0.0),
    Point3d(backX[0], d[1]+d[0], 0.0),
  };

  Point3d lowerArea[4] =
  {
    Point3d(-backX[0], d[0]+d[1], 0.0),
    Point3d(-frontX[0], d[0]+d[1], length),
    Point3d(frontX[0], d[0]+d[1], length),
    Point3d(backX[0], d[0]+d[1], 0.0),
  };

  Point3d lowerChinkArea[4] =
  {
    Point3d(-0.5*chinkWidth, d[0]+d[1], 0.0),
    Point3d(-0.5*chinkWidth, d[0]+d[1], -chinkLength),
    Point3d(0.5*chinkWidth, d[0]+d[1], -chinkLength),
    Point3d(0.5*chinkWidth, d[0]+d[1], 0.0),
  };


  // Draw the lower area
  
  dc.SetPen(*wxTRANSPARENT_PEN);
  dc.SetBrush(*wxLIGHT_GREY_BRUSH);
  drawPolygon(dc, 4, lowerArea);
  drawPolygon(dc, 4, lowerChinkArea);

  // Draw left lower mass
  
  dc.SetPen(wxPen(*wxBLACK, lineWidth));
  dc.SetBrush(*wxWHITE_BRUSH);

  drawPolygon(dc, 4, lowerFrontalPlane, -W-backX[0], 0.0, 0.0);
  drawPolygon(dc, 4, lowerLeftSagittalPlane);
  drawPolygon(dc, 4, lowerLeftTransversePlane);

  // Draw right lower mass
  
  dc.SetPen(wxPen(*wxBLACK, lineWidth));
  dc.SetBrush(*wxWHITE_BRUSH);

  drawPolygon(dc, 4, lowerFrontalPlane, backX[0], 0.0, 0.0);
  drawPolygon(dc, 4, lowerRightSagittalPlane);
  drawPolygon(dc, 4, lowerRightTransversePlane);

  drawLines(dc, 6, lowerChinkContour);


  // ****************************************************************
  // Upper masses.
  // ****************************************************************

  Point3d upperFrontalPlane[4] =
  {
    Point3d(0.0, 0.0, 0.0),
    Point3d(W,   0.0, 0.0),
    Point3d(W,   d[1], 0.0),
    Point3d(0.0, d[1], 0.0)
  };

  Point3d upperLeftSagittalPlane[4] =
  {
    Point3d(-backX[1], 0.0, 0.0),
    Point3d(-frontX[1], 0.0, length),
    Point3d(-frontX[1], d[1], length),
    Point3d(-backX[1], d[1], 0.0)
  };

  Point3d upperRightSagittalPlane[4] =
  {
    Point3d(backX[1]+W, 0.0, 0.0),
    Point3d(frontX[1]+W, 0.0, length),
    Point3d(frontX[1]+W, d[1], length),
    Point3d(backX[1]+W, d[1], 0.0)
  };

  Point3d upperLeftTransversePlane[4] =
  {
    Point3d(-backX[1], 0.0, 0.0),
    Point3d(-frontX[1], 0.0, length),
    Point3d(-frontX[1]-W, 0.0, length),
    Point3d(-backX[1]-W, 0.0, 0.0)
  };

  Point3d upperRightTransversePlane[4] =
  {
    Point3d(backX[1], 0.0, 0.0),
    Point3d(frontX[1], 0.0, length),
    Point3d(frontX[1]+W, 0.0, length),
    Point3d(backX[1]+W, 0.0, 0.0)
  };

  Point3d upperChinkContour[6] =
  {
    Point3d(-backX[1], 0.0, 0.0),
    Point3d(-0.5*chinkWidth, 0.0, 0.0),
    Point3d(-0.5*chinkWidth, 0.0, -chinkLength),
    Point3d(0.5*chinkWidth, 0.0, -chinkLength),
    Point3d(0.5*chinkWidth, 0.0, 0.0),
    Point3d(backX[1], 0.0, 0.0),
  };

  // Draw left upper mass
  
  dc.SetPen(wxPen(*wxBLACK, lineWidth));
  dc.SetBrush(*wxWHITE_BRUSH);

  drawPolygon(dc, 4, upperFrontalPlane, -W-backX[1], 0.0, 0.0);
  drawPolygon(dc, 4, upperLeftSagittalPlane);
  drawPolygon(dc, 4, upperLeftTransversePlane);

  // Draw right upper mass
  
  dc.SetPen(wxPen(*wxBLACK, lineWidth));
  dc.SetBrush(*wxWHITE_BRUSH);

  drawPolygon(dc, 4, upperFrontalPlane, backX[1], 0.0, 0.0);
  drawPolygon(dc, 4, upperRightSagittalPlane);
  drawPolygon(dc, 4, upperRightTransversePlane);

  drawLines(dc, 6, upperChinkContour);

  // ****************************************************************
  // Inlet and outlet regions.
  // ****************************************************************

  drawLine(dc, backX[1], 0.0, 0.0, outletRadius, -outletLength, 0.0);
  drawLine(dc, -backX[1], 0.0, 0.0, -outletRadius, -outletLength, 0.0);
  drawLine(dc, backX[0], d[0]+d[1], 0.0, inletRadius, d[0]+d[1]+inletLength, 0.0);
  drawLine(dc, -backX[0], d[0]+d[1], 0.0, -inletRadius, d[0]+d[1]+inletLength, 0.0);
}


// ****************************************************************************
// ****************************************************************************

void GlottisPicture::initView(const double minX, const double maxX, double const minY, 
  double const maxY, const double minZ, const double maxZ)
{
  const double EPSILON = 0.000001;
  const double Z_FACTOR = 0.7071;

  double rangeX = maxX - minX;
  double rangeY = maxY - minY;
  double rangeZ = maxZ - minZ;

  double rangeXZ = rangeX + Z_FACTOR*rangeZ;
  double rangeYZ = rangeY + Z_FACTOR*rangeZ;

  if (rangeXZ < EPSILON)
  {
    rangeXZ = EPSILON;
  }
  if (rangeYZ < EPSILON)
  {
    rangeYZ = EPSILON;
  }

  wxSize size = this->GetSize();
  int w = size.GetWidth();
  int h = size.GetHeight();

  if (w < 1) { w = 1; }
  if (h < 1) { h = 1; }

  if ((double)w/(double)h > rangeXZ/rangeYZ)
  {
    viewFactor = 0.95*h / rangeYZ;
  }
  else
  {
    viewFactor = 0.95*w / rangeXZ;
  }

  // The range of the view port in pixels
  double viewportRangeU = viewFactor*rangeXZ;
  double viewportRangeV = viewFactor*rangeYZ;

  double viewportLeftU = (w - viewportRangeU) / 2;
  double viewportTopV  = (h - viewportRangeV) / 2;

  double zeroU_rel = (0.0 - minX)*viewFactor + (0.0 - minZ)*viewFactor*Z_FACTOR;
  double zeroV_rel = (maxZ - minZ)*viewFactor*Z_FACTOR + (0.0 - minY)*viewFactor - (0.0 - minZ)*viewFactor*Z_FACTOR;

  originU = viewportLeftU + zeroU_rel;
  originV = viewportTopV + zeroV_rel;
}


// ****************************************************************************
/// Transforms the given coordinates x, y, z from the 3D-model coordinate system
/// into the 2D-screen coordinate system using Kavalier-projection.
/// x is pointing to the right.
/// y is pointing downwards.
/// z is pointing into the screen.
/// u is pointing to the right.
/// v is pointing downwards.
// ****************************************************************************

void GlottisPicture::transformCoord(const double x, const double y, const double z, int &u, int &v)
{
  const double Z_FACTOR = 0.7071;
  u = originU + x*viewFactor + z*viewFactor*Z_FACTOR;
  v = originV + y*viewFactor - z*viewFactor*Z_FACTOR;
}


// ****************************************************************************
/// Draws a line on the given DC between the given 3D-points using the current
/// pen.
// ****************************************************************************

void GlottisPicture::drawLine(wxDC &dc, const double x0, const double y0, const double z0,
  const double x1, const double y1, const double z1)
{
  int u0, v0, u1, v1;

  transformCoord(x0, y0, z0, u0, v0);
  transformCoord(x1, y1, z1, u1, v1);
  
  dc.DrawLine(u0, v0, u1, v1);
}


// ****************************************************************************
/// Draws connected lines defined by n points.
// ****************************************************************************

void GlottisPicture::drawLines(wxDC &dc, int n, Point3d p3d[], double offsetX, double offsetY, double offsetZ)
{
  const int MAX_POINTS = 64;
  int i;
  wxPoint p2d[MAX_POINTS];

  if (n > MAX_POINTS)
  {
    return;
  }

  for (i=0; i < n; i++)
  {
    transformCoord(p3d[i].x + offsetX, p3d[i].y + offsetY, p3d[i].z + offsetZ, p2d[i].x, p2d[i].y);
  }

  dc.DrawLines(n, p2d);
}


// ****************************************************************************
/// Draws a polygon with the selected pen and brush.
// ****************************************************************************

void GlottisPicture::drawPolygon(wxDC &dc, int n, Point3d p3d[], double offsetX, double offsetY, double offsetZ)
{
  const int MAX_POINTS = 64;
  int i;
  wxPoint p2d[MAX_POINTS];

  if (n > MAX_POINTS)
  {
    return;
  }

  for (i=0; i < n; i++)
  {
    transformCoord(p3d[i].x + offsetX, p3d[i].y + offsetY, p3d[i].z + offsetZ, p2d[i].x, p2d[i].y);
  }

  dc.DrawPolygon(n, p2d);
}

// ****************************************************************************
