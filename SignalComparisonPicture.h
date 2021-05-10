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

#ifndef __SIGNAL_COMPARISON_PICTURE_H__
#define __SIGNAL_COMPARISON_PICTURE_H__

#include "BasicPicture.h"
#include "SpectrogramPlot.h"
#include "Graph.h"
#include "Data.h"


// ****************************************************************************
// ****************************************************************************

class SignalComparisonPicture : public BasicPicture
{
  // **************************************************************************
  // Public data.
  // **************************************************************************

public:
  bool showExtraTrack;
  bool showSonagrams;
  bool showSegmentation;
  bool showModelF0Curve;
  SpectrogramPlot *spectrogramPlot;

  // **************************************************************************
  // Public functions.
  // **************************************************************************

public:
  SignalComparisonPicture(wxWindow *parent, wxWindow *updateParent);
  virtual void draw(wxDC &dc);
  
  int getVirtualHeight();
  int getWindowHeight();
  void setVertOffset(double offset_percent);
  double getVertOffset_percent();

  // **************************************************************************
  // Private data.
  // **************************************************************************

private:
  static const int PLAY_BUTTON_WIDTH = 20;
  static const int PLAY_BUTTON_HEIGHT = 20;
  static const int NUM_PLAY_BUTTONS = 2;

  enum Rows
  {
    SEGMENTATION_ROW_1,
    SEGMENTATION_ROW_2,
    OSCILLOGRAM_ROW_1,
    SPECTROGRAM_ROW_1,
    OSCILLOGRAM_ROW_2,
    SPECTROGRAM_ROW_2,
    NUM_ROWS
  };

  Data *data;
  wxWindow *updateParent;
  int rowY[NUM_ROWS];
  int rowH[NUM_ROWS];
  double verticalOffset_percent;
  int playButtonX[NUM_PLAY_BUTTONS];
  int playButtonY[NUM_PLAY_BUTTONS];
  wxMenu *contextMenu;
  wxMenu *contextMenuSpectrogram;

  int menuX, menuY;
  int lastMx, lastMy;
  bool moveBorder;
  int lineWidth{ this->FromDIP(1) };

  // **************************************************************************
  // Private functions.
  // **************************************************************************

private:
  void paintSignals(wxDC &dc);
  void paintOscillogram(wxDC &dc, int areaX, int areaY, int areaWidth, int areaHeight, 
    Signal16 *s, int firstSample, int numSamples);
  void paintPhoneSegmentation(wxDC &dc, int areaX, int areaY, int areaWidth, int areaHeight, 
    double areaStartTime_s, double areaDuration_s);
  void paintWordSegmentation(wxDC &dc, int areaX, int areaY, int areaWidth, int areaHeight, 
    double areaStartTime_s, double areaDuration_s);
  void paintPlayButton(wxDC &dc, int x, int y);
  void drawSelectionMark(wxDC &dc, int x, int y1, int y2, bool isLeftMark);

  void calcRowCoord();
  void updatePage();
  void updateAnnotationDialog();
  void getUnderlyingSegment(int mx, int my, int &segmentIndex, bool &isOnBorder);

  void OnMouseEvent(wxMouseEvent &event);

  void OnPlaySegmentMainTrack(wxCommandEvent& event);
  void OnPlaySegmentExtraTrack(wxCommandEvent& event);
  void OnInsertSegment(wxCommandEvent &event);
  void OnDeleteSegment(wxCommandEvent &event);
  void OnClearSegmentSelection(wxCommandEvent &event);
  void OnSetMinSegmentDuration(wxCommandEvent& event);

  void OnSetReferenceMark(wxCommandEvent &event);
  void OnClearReferenceMark(wxCommandEvent &event);
  void OnSwapTracks(wxCommandEvent &event);

  void OnClearSelection(wxCommandEvent &event);
  void OnSetSelectionStart(wxCommandEvent &event);
  void OnSetSelectionEnd(wxCommandEvent &event);
  void OnSelectAll(wxCommandEvent &event);

  void OnSpectrogramRange4kHz(wxCommandEvent &event);
  void OnSpectrogramRange8kHz(wxCommandEvent &event);
  void OnSpectrogramRange12kHz(wxCommandEvent &event);

  // ****************************************************************************
  // Declare the event table right at the end
  // ****************************************************************************

  DECLARE_EVENT_TABLE()
};

#endif

// ****************************************************************************
