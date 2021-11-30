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

#ifndef __SPECTROGRAM_PLOT_H__
#define __SPECTROGRAM_PLOT_H__

#include <wx/wx.h>
#include "VocalTractLabBackend/Dsp.h"
#include <vector>

using namespace std;

// ****************************************************************************
/// The window shape is always a gaussian.
// ****************************************************************************

class SpectrogramPlot
{
  // **************************************************************************
  // Public data.
  // **************************************************************************

public:
  int windowLength_pt;
  int frameLengthExponent;
  double viewRange_Hz;
  double dynamicRange_dB;

  // **************************************************************************
  // Public functions.
  // **************************************************************************

public:
  SpectrogramPlot();
  
  void drawSpectrogram(wxDC &dc, int areaX, int areaY, int areaWidth, int areaHeight, 
    Signal16 *s, int firstSample, int numSamples);

  int getFrameLengthExponent(int windowLength_pt);
  void getGaussWindow(Signal &window, int windowLength_pt);

  void drawCurve(
    wxDC &dc, int areaX, int areaY, int areaWidth, int areaHeight, 
    vector<double> &samples, double timeStep_s, double startTime_s, double duration_s,
    double minValue, double maxValue, wxColor color, bool dashed = false);

};


#endif