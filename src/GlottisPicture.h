// ****************************************************************************
// This file is part of VocalTractLab.
// Copyright (C) 2020, Peter Birkholz, Dresden, Germany
// www.vocaltractlab.de
// author: Peter Birkholz
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

#ifndef __GLOTTIS_PICTURE_H__
#define __GLOTTIS_PICTURE_H__

#include "BasicPicture.h"
#include "Graph.h"
#include "Backend/Glottis.h"
#include "Backend/GeometricGlottis.h"
#include "Backend/TwoMassModel.h"
#include "Backend/TriangularGlottis.h"

// ****************************************************************************
// ****************************************************************************

class GlottisPicture : public BasicPicture
{
  // **************************************************************************
  // Public functions.
  // **************************************************************************

public:
  GlottisPicture(wxWindow *parent, Glottis *glottis);
  virtual void draw(wxDC &dc);

  // **************************************************************************
  // Private data.
  // **************************************************************************

private:
  struct Point3d
  {
    double x, y, z;
    Point3d() {}
    Point3d(double x, double y, double z)
    {
      this->x = x;
      this->y = y;
      this->z = z;
    }
  };

  Glottis *glottis;

  int originU;
  int originV;
  double viewFactor;

  // **************************************************************************
  // Private functions.
  // **************************************************************************

private:
  void drawGeometricGlottis(wxDC &dc);
  void drawTwoMassModel(wxDC &dc);
  void drawTriangularGlottis(wxDC &dc);

  void initView(const double minX, const double maxX, double const minY, 
    double const maxY, const double minZ, const double maxZ);
  void transformCoord(const double x, const double y, const double z, int &u, int &v);
  void drawLine(wxDC &dc, const double x0, const double y0, const double z0,
    const double x1, const double y1, const double z1);
  void drawLines(wxDC &dc, int n, Point3d p3d[], double offsetX = 0.0, double offsetY = 0.0, double offsetZ = 0.0);
  void drawPolygon(wxDC &dc, int n, Point3d p3d[], double offsetX = 0.0, double offsetY = 0.0, double offsetZ = 0.0);
};

#endif

// ****************************************************************************
