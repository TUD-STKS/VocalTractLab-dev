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
// Returns a pointer to an array containing the values of a color map
// from https://peterkovesi.com/projects/colourmaps/
// 
// Peter Kovesi. Good Colour Maps: How to Design Them.
//
// ****************************************************************************
typedef int(*ColorMap)[256][3];
ColorMap ColorScale::getColorMap() {

	// CET-CBD1:   diverging-protanopic-deuteranopic_bwy_60-95_c32_n256
	static int colorMap[256][3] = {
					{58,144,254},
					{62,145,254},
					{65,146,254},
					{68,146,254},
					{70,147,254},
					{73,148,254},
					{76,148,254},
					{78,149,254},
					{80,150,254},
					{83,151,253},
					{85,151,253},
					{87,152,253},
					{89,153,253},
					{91,153,253},
					{94,154,253},
					{96,155,253},
					{97,155,253},
					{99,156,253},
					{101,157,253},
					{103,158,253},
					{105,158,253},
					{107,159,253},
					{109,160,252},
					{110,160,252},
					{112,161,252},
					{114,162,252},
					{115,163,252},
					{117,163,252},
					{119,164,252},
					{120,165,252},
					{122,165,252},
					{124,166,252},
					{125,167,252},
					{127,168,251},
					{128,168,251},
					{130,169,251},
					{131,170,251},
					{133,171,251},
					{134,171,251},
					{136,172,251},
					{137,173,251},
					{139,173,251},
					{140,174,251},
					{141,175,251},
					{143,176,250},
					{144,176,250},
					{146,177,250},
					{147,178,250},
					{148,179,250},
					{150,179,250},
					{151,180,250},
					{152,181,250},
					{154,182,250},
					{155,182,250},
					{156,183,250},
					{158,184,249},
					{159,185,249},
					{160,185,249},
					{161,186,249},
					{163,187,249},
					{164,187,249},
					{165,188,249},
					{166,189,249},
					{168,190,249},
					{169,190,249},
					{170,191,248},
					{171,192,248},
					{173,193,248},
					{174,194,248},
					{175,194,248},
					{176,195,248},
					{177,196,248},
					{179,197,248},
					{180,197,248},
					{181,198,247},
					{182,199,247},
					{183,200,247},
					{185,200,247},
					{186,201,247},
					{187,202,247},
					{188,203,247},
					{189,203,247},
					{190,204,247},
					{192,205,246},
					{193,206,246},
					{194,206,246},
					{195,207,246},
					{196,208,246},
					{197,209,246},
					{198,210,246},
					{199,210,246},
					{201,211,246},
					{202,212,245},
					{203,213,245},
					{204,213,245},
					{205,214,245},
					{206,215,245},
					{207,216,245},
					{208,217,245},
					{209,217,245},
					{211,218,244},
					{212,219,244},
					{213,220,244},
					{214,220,244},
					{215,221,244},
					{216,222,244},
					{217,223,244},
					{218,224,244},
					{219,224,243},
					{220,225,243},
					{221,226,243},
					{222,227,243},
					{223,228,243},
					{224,228,243},
					{226,229,243},
					{227,230,242},
					{228,231,242},
					{229,231,242},
					{230,232,242},
					{231,233,242},
					{232,234,241},
					{233,234,241},
					{234,235,241},
					{234,236,240},
					{235,236,240},
					{236,236,239},
					{236,237,238},
					{237,237,237},
					{237,237,236},
					{238,237,235},
					{238,236,234},
					{238,236,232},
					{238,236,231},
					{238,235,229},
					{237,234,228},
					{237,234,226},
					{237,233,224},
					{236,232,223},
					{236,231,221},
					{236,231,219},
					{235,230,218},
					{235,229,216},
					{234,228,214},
					{234,228,213},
					{233,227,211},
					{233,226,209},
					{233,225,208},
					{232,224,206},
					{232,224,204},
					{231,223,202},
					{231,222,201},
					{230,221,199},
					{230,220,197},
					{229,220,196},
					{229,219,194},
					{228,218,192},
					{228,217,191},
					{227,216,189},
					{227,216,187},
					{226,215,186},
					{226,214,184},
					{226,213,182},
					{225,213,181},
					{225,212,179},
					{224,211,177},
					{224,210,176},
					{223,209,174},
					{223,209,172},
					{222,208,171},
					{222,207,169},
					{221,206,167},
					{220,206,166},
					{220,205,164},
					{219,204,162},
					{219,203,161},
					{218,203,159},
					{218,202,157},
					{217,201,156},
					{217,200,154},
					{216,199,152},
					{216,199,151},
					{215,198,149},
					{215,197,148},
					{214,196,146},
					{214,196,144},
					{213,195,143},
					{212,194,141},
					{212,193,139},
					{211,193,138},
					{211,192,136},
					{210,191,134},
					{210,190,133},
					{209,190,131},
					{208,189,129},
					{208,188,128},
					{207,187,126},
					{207,187,125},
					{206,186,123},
					{206,185,121},
					{205,184,120},
					{204,184,118},
					{204,183,116},
					{203,182,115},
					{203,181,113},
					{202,181,111},
					{201,180,110},
					{201,179,108},
					{200,178,106},
					{199,178,105},
					{199,177,103},
					{198,176,102},
					{198,175,100},
					{197,175,98},
					{196,174,97},
					{196,173,95},
					{195,172,93},
					{194,172,92},
					{194,171,90},
					{193,170,88},
					{193,169,87},
					{192,169,85},
					{191,168,83},
					{191,167,81},
					{190,166,80},
					{189,166,78},
					{189,165,76},
					{188,164,75},
					{187,164,73},
					{187,163,71},
					{186,162,69},
					{185,161,68},
					{185,161,66},
					{184,160,64},
					{183,159,62},
					{183,159,60},
					{182,158,59},
					{181,157,57},
					{180,156,55},
					{180,156,53},
					{179,155,51},
					{178,154,49},
					{178,153,47},
					{177,153,45},
					{176,152,43},
					{176,151,41},
					{175,151,39},
					{174,150,36},
					{173,149,34},
					{173,149,32},
					{172,148,29},
					{171,147,26},
					{171,146,23},
					{170,146,20},
					{169,145,17},
					{168,144,13},
					{168,144,8},
	};
	return &colorMap;
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
