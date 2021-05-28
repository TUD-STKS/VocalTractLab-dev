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

#ifndef __COLOR_SCALE__
#define __COLOR_SCALE__

#include <wx/wx.h>

// ****************************************************************************
/// This class contains functions to create perceptually-based color scales.
// ****************************************************************************

class ColorScale
{
    typedef int(*ColorMap)[256][3];
  // ****************************************************************
  // Public functions.
  // ****************************************************************

public:
  static void rgbToHsv(const double r, const double g, const double b, double &h, double &s, double &v);
  static void hsvToRgb(const double h, const double s, const double v, double &r, double &g, double &b);
  static void getYellowBlueScale(int numColors, wxColor scale[]);
  static ColorMap getColorMap();							

  static void test();

  // ****************************************************************
  // Private functions.
  // ****************************************************************

  static double maximum(double a, double b, double c);
  static double minimum(double a, double b, double c);
};


#endif
