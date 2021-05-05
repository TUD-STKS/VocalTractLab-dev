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

#ifndef __GESTURAL_SCORE_PICTURE_H__
#define __GESTURAL_SCORE_PICTURE_H__

#include "BasicPicture.h"
#include "Data.h"


// ****************************************************************************
// ****************************************************************************

class GesturalScorePicture : public BasicPicture
{
  // **************************************************************************
  // Public data.
  // **************************************************************************

public:
  bool showGesturalScore;

  // **************************************************************************
  // Public functions.
  // **************************************************************************

public:
  GesturalScorePicture(wxWindow *parent, wxWindow *updateParent);
  virtual void draw(wxDC &dc);

  int getNumControlParams();
  int getVirtualHeight();
  int getWindowHeight();
  void setVertOffset(double offset_percent);
  double getVertOffset_percent();

  // **************************************************************************
  // Private data.
  // **************************************************************************

private:
  static const int MAX_PARAMS = 100;

  Data *data;
  GesturalScore *gs;
  wxWindow *updateParent;
  double verticalOffset_percent;
  int gestureRowY[GesturalScore::NUM_GESTURE_TYPES];
  int gestureRowH[GesturalScore::NUM_GESTURE_TYPES];
  int paramRowY[MAX_PARAMS];
  int paramRowH[MAX_PARAMS];
  wxMenu *contextMenu;

  int menuX, menuY;
  int lastMx, lastMy;
  bool moveBorder;
  bool moveTarget;

  // **************************************************************************
  // Private functions.
  // **************************************************************************

private:
  void paintGesturalScore(wxDC &dc);
  void paintMotorProgram(wxDC &dc);
  void calcGestureRowCoord();
  void calcParamRowCoord();
  void cutString(wxDC &dc, wxString &st, int maxWidth_px);
  void getUnderlyingGesture(int localX, int localY, int &gestureType, 
    int &gestureIndex, bool &isOnBorder, bool &isOnTarget);
  void getTargetLineCoord(
    int gestureType, int leftBorderX, int rightBorderX,
    double t0, double v0, double t1, double v1, 
    double &x0, double &y0, double &x1, double &y1);
  void getParamCurveMinMax(int paramIndex, double &min, double &max);
  int getRepresentativeParam(int gestureType);
  
  void updatePage(int updateParam = UPDATE_PICTURES_AND_CONTROLS);

  void OnMouseEvent(wxMouseEvent &event);
  void OnInsertGesture(wxCommandEvent &event);
  void OnDeleteGesture(wxCommandEvent &event);
  void OnSetInitialScore(wxCommandEvent &event);
  void OnScoreFromSegmentSequence(wxCommandEvent &event);

  // ****************************************************************************
  // Declare the event table right at the end
  // ****************************************************************************

  DECLARE_EVENT_TABLE()
};

#endif

// ****************************************************************************
