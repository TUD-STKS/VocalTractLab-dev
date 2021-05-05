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

#include "SpectrogramPlot.h"
#include <iostream>
#include <limits>
#include "Backend/Constants.h"


// ****************************************************************************
/// Constructor. Init the variables.
// ****************************************************************************

SpectrogramPlot::SpectrogramPlot()
{
  windowLength_pt = (int)(0.006*SAMPLING_RATE);
  frameLengthExponent = 9;     // = 1024 points
  viewRange_Hz = 5000.0;
  dynamicRange_dB = 120.0;
}

// ****************************************************************************
/// Paint the plot on the given device context.
// ****************************************************************************

void SpectrogramPlot::drawSpectrogram(wxDC &dc, int areaX, int areaY, int areaWidth, int areaHeight, 
  Signal16 *s, int firstSample, int numSamples)
{
  if ((areaWidth < 1) || (areaHeight < 1))
  {
    return;
  }

  int i, k;
  
  // 2d-array of amplitude values for all pixels of the image.
  double *ampValue = new double[areaWidth*areaHeight];

  int minFrameLengthExponent = getFrameLengthExponent(windowLength_pt);
  if (frameLengthExponent < minFrameLengthExponent)
  {
    frameLengthExponent = minFrameLengthExponent;
  }
  int frameLength = ((int)1) << frameLengthExponent;

  int windowCenterSample = 0;
  int windowStartSample = 0;

  // Get the gauss window

  Signal gaussWindow;
  getGaussWindow(gaussWindow, windowLength_pt);

  // How many points (samples) of a spectrum are visible in one column
  // of the target image?

  int numVisSpectrumPoints = (viewRange_Hz*frameLength) / (double)SAMPLING_RATE;
  if (numVisSpectrumPoints < 1)
  {
    numVisSpectrumPoints = 1;
  }
  if (numVisSpectrumPoints >= frameLength/2)
  {
    numVisSpectrumPoints = frameLength/2 - 1;
  }

  ComplexSignal frame(frameLength);

  // ****************************************************************
  // Target image
  // ****************************************************************

  wxImage image(areaWidth, areaHeight);

  // Get the pointer to the RGB-RGB-RGB-sequence of the image.
  unsigned char *imageData = image.GetData();
  unsigned char *targetPtr = NULL;
  unsigned char grayValue;
  double *ampValuePtr = NULL;

  // ****************************************************************
  // Loop through all columns of the target image.
  // ****************************************************************

  const double EPSILON = 0.000000001;
  int indexE12 = 0;     // Index of the spectrum point (*2^12)
  int deltaIndexE12 = (numVisSpectrumPoints << 12) / areaHeight;
  int index = 0;
  double value;
  double real, imag;
  double sqaredMagnitude;

  for (i=0; i < areaWidth; i++)
  {
    windowCenterSample = firstSample + i*numSamples / areaWidth;
    windowStartSample = windowCenterSample - windowLength_pt/2;
    
    // **************************************************************
    // Fill the real part of the frame. The imaginary part is ignored 
    // by the FFT-function.
    // **************************************************************

    for (k=0; k < windowLength_pt; k++)
    {
      frame.re[k] = s->getValue(windowStartSample + k) * gaussWindow.x[k];
    }
    // Pad with zeros up to the frame length.
    for (k=windowLength_pt; k < frameLength; k++)
    {
      frame.re[k] = 0.0;
    }

    // The last parameter ("normalize") must be false to get the same
    // intensity for a given signal independent of the frame length!!
    realFFT(frame, frameLengthExponent, false);

    // **************************************************************
    // Fill the corresponding column in the target image.
    // **************************************************************

    // Point to the first amplitude value at column i in the last row
    ampValuePtr = ampValue + (areaHeight-1)*areaWidth + i;
    indexE12 = 0;

    for (k=0; k < areaHeight; k++)
    {
      // Index of the current point in the spectrum
      index = indexE12 >> 12;
      real = frame.re[index];
      imag = frame.im[index];
      sqaredMagnitude = real*real + imag*imag;
      // Avoid taking the log of 0.0
      if (sqaredMagnitude < EPSILON)
      {
        sqaredMagnitude = EPSILON;
      }
      value = 10.0*log( sqaredMagnitude );

      *ampValuePtr = value;
      ampValuePtr-= areaWidth;
      indexE12+= deltaIndexE12;
    }
  }

  // For a very quiet signal, the maximum FFT value is around 210 dB.
  // For a very loud signal, it is around 260 dB.
  // For now, we fix the maximum value to 240 dB as reference (=black).

  double maxValue = 240.0;

  // ****************************************************************
  // Convert the amplitude values in pixel colors and paint them.
  // ****************************************************************

  double factor = -255.0 / dynamicRange_dB;
  int numPixels = areaWidth*areaHeight;
  ampValuePtr = ampValue;
  targetPtr = imageData;

  for (i=0; i < numPixels; i++)
  {
    value = *ampValuePtr++;
    value-= maxValue;

    if (value > 0.0)
    {
      grayValue = 0;
    }
    else
    if (value > -dynamicRange_dB)
    {
      grayValue = (unsigned char)(factor*value);
    }
    else
    {
      grayValue = 255;
    }

    *targetPtr++= grayValue;
    *targetPtr++= grayValue;
    *targetPtr++= grayValue;
  }


  // ****************************************************************
  // Convert the image to a bitmap and draw it.
  // ****************************************************************

  wxBitmap bitmap(image);
  dc.DrawBitmap(bitmap, areaX, areaY);

  // Free the memory.
  delete ampValue;
}


// ****************************************************************************
/// Returns the smallest exponent e for which windowLength <= 2^e.
/// For example, for windowLength = 500, e = 9, because 2^9 = 512.
// ****************************************************************************

int SpectrogramPlot::getFrameLengthExponent(int windowLength_pt)
{
  int e = 1;
  while (((int)1 << e) < windowLength_pt)
  {
    e++;
  }

  return e;
}

// ****************************************************************************
/// Returns a gaussian window in the given signal with the given length.
// ****************************************************************************

void SpectrogramPlot::getGaussWindow(Signal &window, int windowLength_pt)
{
  const double sigma = 0.4;
  int N2 = (windowLength_pt-1) / 2;
  double den = 2.0*sigma*sigma*N2*N2;
  int i;

  window.reset(windowLength_pt);
  for (i=0; i < windowLength_pt; i++)
  {
    window.x[i] = exp(-(i-N2)*(i-N2) / den);
  }
}

// ****************************************************************************
/// This function draws a samples curve as a continual signal on the given 
/// device context into the given area (areaX, areaY, areaWidth, areaHeight).
/// The curve is specified by its sample points (samples) and the time step.
/// The temporal interval of the curve to be drawn is given by startTime_s
/// and duration_s. minValue and maxValue are the values at the bottem and
/// the top of the paint area.
// ****************************************************************************

void SpectrogramPlot::drawCurve(
  wxDC &dc, int areaX, int areaY, int areaWidth, int areaHeight, 
  vector<double> &samples, double timeStep_s, double startTime_s, double duration_s,
  double minValue, double maxValue, wxColor color, bool dashed)
{
  const double EPSILON = 0.000000001;
  double valueDiff = maxValue - minValue;
  // Initial safety check.
  if ((areaWidth < 2) || (areaHeight < 2) || (timeStep_s < EPSILON) || (valueDiff < EPSILON))
  {
    return;
  }

  const int MAX_POINTS = 4096;
  wxPoint points[MAX_POINTS];
  int numPoints = 0;
  
  int i;
  double t_s;
  double value;
  double s0, s1;
  int y;
  int sampleIndex;
  double ratio;
  double samplingRate_Hz = 1.0 / timeStep_s;
  int numSamples = (int)samples.size();

  // Another safety check.
  if (numSamples < 2)
  {
    return;
  }


  // ****************************************************************
  // Create the list of corner points: one point for every pixel column.
  // ****************************************************************

  for (i=0; i < areaWidth; i++)
  {
    t_s = startTime_s + duration_s*i / (areaWidth-1);
    sampleIndex = (int)(t_s*samplingRate_Hz);
    ratio = (t_s - sampleIndex*timeStep_s) * samplingRate_Hz;
    if ((sampleIndex >= 0) && (sampleIndex < numSamples-1) && (numPoints < MAX_POINTS))
    {
      s0 = samples[sampleIndex];
      s1 = samples[sampleIndex+1];

      if ((s0 == 0.0) || (s1 == 0.0))
      {
        value = 0.0;
      }
      else
      {
        value = (1.0-ratio)*s0 + ratio*s1;
      }

      y = areaY + areaHeight - 1 - (int)((areaHeight - 1)*(value - minValue) / valueDiff);

      if (y < areaY)
      {
        y = areaY;
      }
      if (y >= areaY + areaHeight)
      {
        y = areaY + areaHeight - 1;
      }

      points[numPoints].x = areaX + i;
      points[numPoints].y = y;

      numPoints++;
    }
  }

  // ****************************************************************
  // Set the pen and draw the lines.
  // ****************************************************************

  if (dashed)
  {
    dc.SetPen(wxPen(color, 1, wxPENSTYLE_DOT_DASH)); // orig: wxSHORT_DASH
  }
  else
  {
    dc.SetPen( wxPen(color) );
  }

  if (numPoints > 1)
  {
    dc.DrawLines(numPoints, points);
  }
}


// ****************************************************************************
