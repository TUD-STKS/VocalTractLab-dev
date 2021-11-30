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

#include "ColorScale.h"
#include <stdio.h>
#include <math.h>

// ****************************************************************************
/// RGB, each 0.0 to 1.0, to HSV.
/// H = 0.0 to 360.0 (corresponding to 0..360.0 degrees around hexcone)
/// S = 0.0 (shade of gray) to 1.0 (pure color)
/// V = 0.0 (black) to 1.0 {white)
// ****************************************************************************

void ColorScale::rgbToHsv(const double r, const double g, const double b, double &h, double &s, double &v)
{
  double max = maximum(r, g, b);
  double min = minimum(r, g, b);

  v = max;      // This is the value (brightness)

  // Next, calculate saturation
  s = (max != 0.0) ? ((max-min)/max) : 0.0;
  
  // Finally, determine hue

  if (s == 0.0)
  {
    h = 0.0;    // actually undefined
  }
  else
  {
    double delta = max - min;
    if (r == max)
    {
      h = (g - b) / delta;    // between yellow and magenta [degrees]
    }
    else
    if (g == max)
    {
      h = 2.0 + (b - r) / delta;    // between cyan and yellow
    }
    else
    if (b == max)
    {
      h = 4.0 + (r - g) / delta;    // between magenta and cyan
    }

    h*= 60.0;   // Convert to degrees
    if (h < 0.0)
    {
      h+= 360.0;
    }
  }
}


// ****************************************************************************
/// H = 0.0 to 360.0 (corresponding to 0..360 degrees around hexcone)
/// NaN (undefined) for S = 0
/// S = 0.0 (shade of gray) to 1.0 (pure color)
/// V = 0.0 (black) to 1.0 (white)
/// The RGB values are in [0,1].
// ****************************************************************************

void ColorScale::hsvToRgb(const double h, const double s, const double v, double &r, double &g, double &b)
{
  // The color is on the black-white centerline
  if (s == 0.0)
  {
    r = v;
    g = v;
    b = v;
  }
  else
  {
    double f, p, q, t;
    int i;

    double newH = h;
    if (newH == 360.0)
    {
      newH = 0.0;
    }
    newH/= 60.0;   // h is now in [0,6)
    i = (int)floor(newH);
    f = newH - i;
    p = v*(1.0 - s);
    q = v*(1.0 - s*f);
    t = v*(1.0 - (s*(1.0 - f)));

    switch (i)
    {
    case 0: r = v; g = t; b = p; break;
    case 1: r = q; g = v; b = p; break;
    case 2: r = p; g = v; b = t; break;
    case 3: r = p; g = q; b = v; break;
    case 4: r = t; g = p; b = v; break;
    case 5: r = v; g = p; b = q; break;
    }
  }
}

// ****************************************************************************
/// Fills the given array with the colors of a quantitative, perceptually-
/// based color scale between blue and yellow.
// ****************************************************************************

void ColorScale::getYellowBlueScale(int numColors, wxColor scale[])
{
  int half = numColors / 2;
  int i;
  double h, s, v;   // Hue, saturation, value (brightness)
  double r, g, b;

  v = 0.9;    // Keep brightness constant

  h = 240.0;  // 240 degrees = blue
  for (i=0; i < half; i++)
  {
    s = 1.0 - (double)i / (double)(half-1);
    hsvToRgb(h, s, v, r, g, b);
    scale[i] = wxColor((int)(r*255.0), (int)(g*255.0), (int)(b*255.0));
  }

  h = 60.0;  // 60 degrees = yellow
  for (i=half; i < numColors; i++)
  {
    s = (double)(i-half) / (double)(numColors-half-1);
    hsvToRgb(h, s, v, r, g, b);
    scale[i] = wxColor((int)(r*255.0), (int)(g*255.0), (int)(b*255.0));
  }
}

// ****************************************************************************
/// Unit tests.
// ****************************************************************************

void ColorScale::test()
{
  double r, g, b;

  r = 0.2;
  g = 0.5;
  b = 0.7;

  double h, s, v;
  rgbToHsv(r, g, b, h, s, v);

  double r2, g2, b2;
  hsvToRgb(h, s, v, r2, g2, b2);

  wxPrintf("=== Color model conversion test ===\n");
  wxPrintf("before: r=%f g=%f b=%f\n", r, g, b);
  wxPrintf("after : r=%f g=%f b=%f\n", r2, g2, b2);
}

// ****************************************************************************
/// Returns the maximum of a, b, and c.
// ****************************************************************************

double ColorScale::maximum(double a, double b, double c)
{
  double v = a;
  if (b > v) { v = b; }
  if (c > v) { v = c; }
  return v;
}

// ****************************************************************************
/// Returns the minimum of a, b, and c.
// ****************************************************************************

double ColorScale::minimum(double a, double b, double c)
{
  double v = a;
  if (b < v) { v = b; }
  if (c < v) { v = c; }
  return v;
}

// ****************************************************************************
