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

#include "GesturalScorePicture.h"
#include "AnalysisResultsDialog.h"
#include "Data.h"
#include <wx/busyinfo.h>


// ****************************************************************************
// IDs.
// ****************************************************************************

static const int IDM_INSERT_GESTURE = 8000;
static const int IDM_DELETE_GESTURE = 8001;
static const int IDM_SET_INITIAL_SCORE = 8002;
static const int IDM_SCORE_FROM_SEGMENT_SEQUENCE = 8003;


// ****************************************************************************
// The event table.
// ****************************************************************************

BEGIN_EVENT_TABLE(GesturalScorePicture, BasicPicture)
  EVT_MOUSE_EVENTS(GesturalScorePicture::OnMouseEvent)
  EVT_MENU(IDM_INSERT_GESTURE, GesturalScorePicture::OnInsertGesture)
  EVT_MENU(IDM_DELETE_GESTURE, GesturalScorePicture::OnDeleteGesture)
  EVT_MENU(IDM_SET_INITIAL_SCORE, GesturalScorePicture::OnSetInitialScore)
  EVT_MENU(IDM_SCORE_FROM_SEGMENT_SEQUENCE, GesturalScorePicture::OnScoreFromSegmentSequence)
  END_EVENT_TABLE()


// ****************************************************************************
/// Construcor. Passes the parent parameter.
// ****************************************************************************

GesturalScorePicture::GesturalScorePicture(wxWindow *parent, wxWindow *updateParent)
  : BasicPicture(parent)
{
  this->updateParent = updateParent;
  data = Data::getInstance();
  gs = data->gesturalScore;

  int i;
  for (i=0; i < GesturalScore::NUM_GESTURE_TYPES; i++)
  {
    gestureRowY[i] = 0;
    gestureRowH[i] = 0;
  }
  
  for (i=0; i < MAX_PARAMS; i++)
  {
    paramRowY[i] = 0;
    paramRowH[i] = 0;
  }

  // Create the context menu

  contextMenu = new wxMenu();
  contextMenu->Append(IDM_INSERT_GESTURE, "Insert gesture");
  contextMenu->Append(IDM_DELETE_GESTURE, "Delete gesture");
  contextMenu->AppendSeparator();
  contextMenu->Append(IDM_SET_INITIAL_SCORE, "Set initial gestural score");
  contextMenu->Append(IDM_SCORE_FROM_SEGMENT_SEQUENCE, "Init. score from segment sequence");
  
  verticalOffset_percent = 0.0;
  showGesturalScore = true;
  menuX = 0;
  menuY = 0;
  lastMx = 0;
  lastMy = 0;
  moveBorder = false;
  moveTarget = false;
}


// ****************************************************************************
/// Call the appropriate functions to draw the picture.
// ****************************************************************************

void GesturalScorePicture::draw(wxDC &dc)
{
  if (showGesturalScore)
  {
    paintGesturalScore(dc);
  }
  else
  {
    paintMotorProgram(dc);
  }
}


// ****************************************************************************
/// Returns the number of vocal tract parameters and glottis parameters.
/// This number varies
// ****************************************************************************

int GesturalScorePicture::getNumControlParams()
{
  int x = VocalTract::NUM_PARAMS + (int)gs->glottis->controlParam.size();
  return x;
}

// ****************************************************************************
/// Returns the virtual height of the picture.
// ****************************************************************************

int GesturalScorePicture::getVirtualHeight()
{
  int h = 0;

  if (showGesturalScore)
  {
    calcGestureRowCoord();
    h = gestureRowY[GesturalScore::NUM_GESTURE_TYPES-1] + 
        gestureRowH[GesturalScore::NUM_GESTURE_TYPES-1];
  }
  else
  {
    calcParamRowCoord();
    int numRows = getNumControlParams();
    h = paramRowY[numRows-1] + paramRowH[numRows-1];
  }

  return h;
}


// ****************************************************************************
/// Returns the heigth of this picture on the screen.
// ****************************************************************************

int GesturalScorePicture::getWindowHeight()
{
  return this->GetSize().GetHeight();
}


// ****************************************************************************
/// Sets the vertical scroll offset in percent.
// ****************************************************************************

void GesturalScorePicture::setVertOffset(double offset_percent)
{
  verticalOffset_percent = offset_percent;
}


// ****************************************************************************
/// Returns the vertical scroll offset in percent.
// ****************************************************************************

double GesturalScorePicture::getVertOffset_percent()
{
  return verticalOffset_percent;
}


// ****************************************************************************
/// Paints the gestural score.
// ****************************************************************************

void GesturalScorePicture::paintGesturalScore(wxDC &dc)
{
  const int N = GesturalScore::NUM_GESTURE_TYPES;
  int i, k;
  int y, h;

  // ****************************************************************
  // Calc. the y-pos. and height of the rows.
  // ****************************************************************

  //calcGestureRowCoord();    // <- is done in getVirtualHeight() !
  int virtualHeight = getVirtualHeight();
  int windowWidth;
  int windowHeight;
  this->GetSize(&windowWidth, &windowHeight);

  if (virtualHeight > windowHeight)
  {
    int delta = (int)(verticalOffset_percent*0.01*(virtualHeight - windowHeight));
    for (i=0; i < N; i++)
    {
      gestureRowY[i]-= delta;
    }
  }

  // ****************************************************************
  // Clear the background
  // ****************************************************************

  dc.SetBackground(*wxWHITE_BRUSH);
  dc.Clear();

  // ****************************************************************
  // Draw the labels for the gestures left next to the rows.
  // ****************************************************************

  wxString label;

  dc.SetFont(wxFont(9, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

  for (i=0; i < N; i++)
  {
    y = gestureRowY[i];
    h = gestureRowH[i];
    label = gs->gestures[i].name;
  	// Only draw the label if it is visible
    if ((y+h >= 0) && (y < windowHeight))
    {
      dc.DrawText(label, this->FromDIP(5), y);
    }
  }
  int LEFT_MARGIN = Data::getInstance()->LEFT_SCORE_MARGIN;
	
  // ****************************************************************
  // Draw the actual gesture boxes.
  // ****************************************************************

  GestureSequence *sequence = NULL;
  Gesture *gesture = NULL;
  int numGestures = 0;
  double startTime_s = 0.0;
  double duration_s = 0.0;
  int leftX;
  int rightX;
  int upperY;
  int lowerY;

  for (i=0; i < N; i++)
  {
    y = gestureRowY[i];
    h = gestureRowH[i];
    if ((y+h >= 0) && (y < windowHeight))
    {
      sequence = &gs->gestures[i];
      numGestures = sequence->numGestures();

      // ************************************************************
      // In the first run, draw the boxes without borders.
      // ************************************************************

      startTime_s = 0.0;
      duration_s = 0.0;
      for (k=0; k < numGestures; k++)
      {
        gesture = sequence->getGesture(k);
        duration_s = gesture->duration_s;

        leftX = data->gsTimeAxisGraph->getXPos(startTime_s);
        rightX = data->gsTimeAxisGraph->getXPos(startTime_s + duration_s);

        if ((leftX < windowWidth) && (rightX >= LEFT_MARGIN))
        {
          if (leftX < LEFT_MARGIN)
          {
            leftX = LEFT_MARGIN;
          }
          if (rightX >= windowWidth)
          {
            rightX = windowWidth - 1;
          }

          // Draw the box without border

          dc.SetPen(*wxTRANSPARENT_PEN);
          if ((i == data->selectedGestureType) && (k == data->selectedGestureIndex))
          {
            if (gesture->neutral)
            {
              dc.SetBrush( wxColor(255, 215, 0) );
            }
            else
            {
              dc.SetBrush( wxColor(220, 185, 0) );
            }
          }
          else
          if (gesture->neutral)
          {
            dc.SetBrush(*wxWHITE_BRUSH);
          }
          else
          {
            dc.SetBrush( wxColor(230, 230, 230) );
          }

          dc.DrawRectangle(leftX, gestureRowY[i], rightX - leftX + 1, gestureRowH[i]);

          // ********************************************************
          // Draw the target line for interval-valued gestures.
          // ********************************************************
          
          if ((sequence->nominalValues == false) && (gesture->neutral == false))
          {
            double x0, y0, x1, y1;

            getTargetLineCoord(
              i, LEFT_MARGIN, windowWidth-1,
              startTime_s, gesture->dVal, 
              startTime_s + duration_s, gesture->dVal + gesture->duration_s*gesture->slope,
              x0, y0, x1, y1);

            dc.SetPen(wxPen(*wxBLACK, lineWidth, wxPENSTYLE_LONG_DASH));
            dc.DrawLine((int)x0, (int)y0, (int)x1, (int)y1);
          }
        }

        startTime_s+= duration_s;
      }

      // ************************************************************
      // In the second run, draw the borders between the gestures and
      // the gesture values.
      // ************************************************************

      dc.SetPen(wxPen(*wxBLACK, lineWidth));
      dc.SetFont(wxFont(9, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

      startTime_s = 0.0;
      duration_s = 0.0;
      upperY = gestureRowY[i];
      lowerY = gestureRowY[i] + gestureRowH[i] - 1;
      wxString st;

      for (k=0; k < numGestures; k++)
      {
        gesture = sequence->getGesture(k);
        duration_s = gesture->duration_s;

        leftX = data->gsTimeAxisGraph->getXPos(startTime_s);
        rightX = data->gsTimeAxisGraph->getXPos(startTime_s + duration_s);

        if ((leftX >= LEFT_MARGIN) && (leftX < windowWidth))
        {
          dc.DrawLine(leftX, upperY, leftX, lowerY);
        }

        // Print the gesture value

        if (leftX < LEFT_MARGIN)
        {
          leftX = LEFT_MARGIN;
        }

        if ((gesture->neutral == false) && (leftX < rightX))
        {
          if (sequence->nominalValues)
          {
            st = wxString(gesture->sVal);
          }
          else
          {
            if (i == GesturalScore::VELIC_GESTURE)
            {
              st = wxString::Format("%2.2f", gesture->dVal);
            }
            else
            {
              st = wxString::Format("%d %s", (int)gesture->dVal, sequence->unit.c_str());
            }
          }
          cutString(dc, st, rightX - leftX);
          dc.DrawText(st, leftX + 1, upperY + 1);
        }

        startTime_s+= duration_s;
      }

      // Draw the final line.
      leftX = data->gsTimeAxisGraph->getXPos(startTime_s);

      if ((leftX >= LEFT_MARGIN) && (leftX < windowWidth))
      {
        dc.DrawLine(leftX, upperY, leftX, lowerY);
      }
    }
  }


  // ****************************************************************
  // Draw the time functions of the parameters.
  // ****************************************************************

  int paramIndex[N];
  double minVal[N];
  double maxVal[N];
  double valRange[N];

  for (i=0; i < N; i++)
  {
    paramIndex[i] = getRepresentativeParam(i);
    getParamCurveMinMax(paramIndex[i], minVal[i], maxVal[i]);

    valRange[i] = maxVal[i] - minVal[i];
    if (valRange[i] < 0.000001)
    {
      valRange[i] = 0.000001;
    }
  }

  // Draw the threshold line for velic closure at VO = 0.0.

  i = GesturalScore::VELIC_GESTURE;
  y = gestureRowY[i] + gestureRowH[i] - 1 - 
    (int)(gestureRowH[i]*(0.0 - minVal[i]) / valRange[i]);

  dc.SetPen( wxPen(wxColor(150, 150, 150), lineWidth));
  dc.DrawLine(LEFT_MARGIN, y, windowWidth-1, y);

  // ****************************************************************

  double params[MAX_PARAMS];
  int prevY[N] = { 0 };
  int prevDebugY = 0;
  int x;
  int index;
  double pos_s;

  // Run through all pixels from left to right.

  // Drawing a list of points is MUCH faster than drawing each individual line
  // We therefore gather each sequence of lines in a vector and then draw all of them at the end
  std::vector<std::vector<wxPoint>> lines(N);
	
  for (x=LEFT_MARGIN; x < windowWidth; x++)
  {
    pos_s = data->gsTimeAxisGraph->getAbsXValue(x);

    // Get the vocal tract and glottis params into the same array.
    gs->getParams(pos_s, &params[0], &params[VocalTract::NUM_PARAMS]);

    // Transform the F0 parameter from Hz to semitones for the display.
    params[VocalTract::NUM_PARAMS + Glottis::FREQUENCY] = 
      GesturalScore::getF0_st( params[VocalTract::NUM_PARAMS + Glottis::FREQUENCY] );

    // Run through all rows
    for (i=0; i < N; i++)
    {
      y = gestureRowY[i];
      h = gestureRowH[i];

      if ((y+h >= 0) && (y < windowHeight))
      {
        index = paramIndex[i];

        y = gestureRowY[i] + gestureRowH[i] - 1 - 
          (int)(gestureRowH[i]*(params[index] - minVal[i]) / valRange[i]);

        if (y < gestureRowY[i])
        {
          y = gestureRowY[i];
        }
        if (y >= gestureRowY[i] + gestureRowH[i])
        {
          y = gestureRowY[i] + gestureRowH[i] - 1;
        }

        if (x > LEFT_MARGIN)
        {
          lines[i].emplace_back(x, y);
        }
      }
    }

  }

  dc.SetPen(wxPen(*wxBLACK, lineWidth));
  for (const auto& line : lines)
  {
	  if (!line.empty())
	  {
    		dc.DrawLines(line.size(), &line[0]);
	  }
  }	

  // ****************************************************************
  // Draw black lines separating the rows.
  // ****************************************************************

  dc.SetPen(wxPen(*wxBLACK, lineWidth));

  for (i=0; i < N; i++)
  {
    y = gestureRowY[i] - 1;
    dc.DrawLine(LEFT_MARGIN, y, windowWidth-1, y);
  }
  y = gestureRowY[N-1] + gestureRowH[N-1];
  dc.DrawLine(LEFT_MARGIN, y, windowWidth-1, y);

  // Draw one vertical line at the left end and right end.
  int topY = gestureRowY[0] - 1;
  int bottomY = gestureRowY[N-1] + gestureRowH[N-1];
  
  dc.DrawLine(LEFT_MARGIN, topY, LEFT_MARGIN, bottomY);
  dc.DrawLine(windowWidth-1, topY, windowWidth-1, bottomY);

  // ****************************************************************
  // Draw the mark.
  // ****************************************************************
  
  int graphX, graphY, graphW, graphH;
  data->gsTimeAxisGraph->getDimensions(graphX, graphY, graphW, graphH);

  x = data->gsTimeAxisGraph->getXPos( data->gesturalScoreMark_s );
  if ((x >= graphX) && (x < graphX + graphW))
  {
    dc.SetPen(wxPen(*wxRED, lineWidth));
    dc.DrawLine(x, 0, x, windowHeight);
  }
}


// ****************************************************************************
/// Paints the motor program.
// ****************************************************************************

void GesturalScorePicture::paintMotorProgram(wxDC &dc)
{
  int LEFT_MARGIN = Data::getInstance()->LEFT_SCORE_MARGIN;
  int N = getNumControlParams();
  int i, k;
  int x, y;
  int h;

  // ****************************************************************
  // Calc. the y-pos. and height of the rows.
  // ****************************************************************

  //calcParamRowCoord();    // <- is done in getVirtualHeight() !
  int virtualHeight = getVirtualHeight();
  int windowWidth;
  int windowHeight;
  this->GetSize(&windowWidth, &windowHeight);

  if (virtualHeight > windowHeight)
  {
    int delta = (int)(verticalOffset_percent*0.01*(virtualHeight - windowHeight));
    for (i=0; i < N; i++)
    {
      paramRowY[i]-= delta;
    }
  }

  // ****************************************************************
  // Clear the background.
  // ****************************************************************

  dc.SetBackground(*wxWHITE_BRUSH);
  dc.Clear();

  // ****************************************************************
  // Get the min and max values of all parameters.
  // ****************************************************************

  double minVal[MAX_PARAMS];
  double maxVal[MAX_PARAMS];
  double valRange[MAX_PARAMS];

  for (i=0; i < VocalTract::NUM_PARAMS; i++)
  {
    minVal[i] = gs->vocalTract->param[i].min;
    maxVal[i] = gs->vocalTract->param[i].max;
  }

  for (i=VocalTract::NUM_PARAMS; i < N; i++)
  {
    k = i - VocalTract::NUM_PARAMS;
    // Should not happen:
    if (k >= (int)gs->glottis->controlParam.size())
    {
      k = (int)gs->glottis->controlParam.size() - 1;
    }
    minVal[i] = gs->glottis->controlParam[k].min;
    maxVal[i] = gs->glottis->controlParam[k].max;
  }

  for (i=0; i < N; i++)
  {
    valRange[i] = maxVal[i] - minVal[i];
    if (valRange[i] < 0.000001)
    {
      valRange[i] = 0.000001;
    }
  }


  // ****************************************************************
  // Draw the targets.
  // ****************************************************************

  vector<Target> *sequence;
  Target *target;
  int numTargets;
  double pos_s;
  int leftX, rightX;
  int targetY;

  // Draw with a black dotted pen
  dc.SetPen(wxPen(*wxBLACK, lineWidth, wxPENSTYLE_LONG_DASH));

  // ****************************************************************
  // Run through all target sequences.
  // ****************************************************************

  for (i=0; i < N; i++)
  {
    y = paramRowY[i];
    h = paramRowH[i];

    if ((y+h >= 0) && (y < windowHeight))
    {
      if (i < VocalTract::NUM_PARAMS)
      {
        sequence = &gs->tractParamTargets[i];
      }
      else
      {
        k = i - VocalTract::NUM_PARAMS;
        // Should not happen:
        if (k >= (int)gs->glottis->controlParam.size())
        {
          k = (int)gs->glottis->controlParam.size() - 1;
        }
        sequence = &gs->glottisParamTargets[k];
      }

      // ************************************************************
      // Run through all targets in the sequence.
      // ************************************************************

      numTargets = (int)sequence->size();
      pos_s = 0.0;

      for (k=0; k < numTargets; k++)
      {
        target = &sequence->at(k);
        leftX = data->gsTimeAxisGraph->getXPos(pos_s);
        rightX = data->gsTimeAxisGraph->getXPos(pos_s + target->duration);
        targetY = y + h - 1 - (int)(h*(target->value - minVal[i]) / valRange[i]);

        // Draw a vertical line at the end of the target interval.
        if ((rightX >= LEFT_MARGIN) && (rightX < windowWidth))
        {
          dc.DrawLine(rightX, y, rightX, y+h-1);
        }

        // Draw the horizontal target line.
        if ((rightX >= LEFT_MARGIN) && (leftX < windowWidth))
        {
          if (leftX < LEFT_MARGIN)
          {
            leftX = LEFT_MARGIN;
          }
          if (rightX >= windowWidth)
          {
            rightX = windowWidth - 1;
          }
          dc.DrawLine(leftX, targetY, rightX, targetY);
        }

        pos_s+= target->duration;
      }

    }
  }

  // ****************************************************************
  // Draw the time functions of the parameters.
  // ****************************************************************

  double params[MAX_PARAMS];
  int prevY[MAX_PARAMS] = { 0 };

  // Run through all pixels from left to right.
  // Drawing a list of points is MUCH faster than drawing each individual line
  // We therefore gather each sequence of lines in a vector and then draw all of them at the end
  std::vector<std::vector<wxPoint>> lines(N);
  for (x=LEFT_MARGIN; x < windowWidth; x++)
  {
    pos_s = data->gsTimeAxisGraph->getAbsXValue(x);

    // Get the vocal tract and glottis params into the same array.
    gs->getParams(pos_s, &params[0], &params[VocalTract::NUM_PARAMS]);

    // Run through all rows
    for (i=0; i < N; i++)
    {
      y = paramRowY[i];
      h = paramRowH[i];

      if ((y+h >= 0) && (y < windowHeight))
      {
        y = paramRowY[i] + paramRowH[i] - 1 - (int)(paramRowH[i]*(params[i] - minVal[i]) / valRange[i]);
        if (y < paramRowY[i])
        {
          y = paramRowY[i];
        }
        if (y >= paramRowY[i] + paramRowH[i])
        {
          y = paramRowY[i] + paramRowH[i] - 1;
        }

        if (x > LEFT_MARGIN)
        {
          lines[i].emplace_back(x, y);
        }
      }
    }
  }
  dc.SetPen(wxPen(*wxBLACK, lineWidth));
  for (const auto& line : lines)
  {
      dc.DrawLines(line.size(), &line[0]);
  }



  // ****************************************************************
  // Draw the labels for the parameters left next to the rows.
  // ****************************************************************

  wxString label;

  dc.SetFont(wxFont(9, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

  for (i=0; i < N; i++)
  {
    y = paramRowY[i];
    h = paramRowH[i];
    if ((y+h >= 0) && (y < windowHeight))
    {
      if (i < VocalTract::NUM_PARAMS)
      {
        label = wxString( data->vocalTract->param[i].abbr );
      }
      else
      {
        label = gs->glottis->controlParam[i - VocalTract::NUM_PARAMS].name;
      }

      cutString(dc, label, LEFT_MARGIN - FromDIP(10));
      dc.DrawText(label, FromDIP(5), y);
    }
  }

  // ****************************************************************
  // Draw black lines separating the rows.
  // ****************************************************************

  dc.SetPen(wxPen(*wxBLACK, lineWidth));

  for (i=0; i < N; i++)
  {
    y = paramRowY[i] - 1;
    dc.DrawLine(LEFT_MARGIN, y, windowWidth-1, y);
  }
  y = paramRowY[N-1] + paramRowH[N-1];
  dc.DrawLine(LEFT_MARGIN, y, windowWidth-1, y);

  // Draw one vertical line at the left end and right end.
  int topY = paramRowY[0] - 1;
  int bottomY = paramRowY[N-1] + paramRowH[N-1];
  
  dc.DrawLine(LEFT_MARGIN, topY, LEFT_MARGIN, bottomY);
  dc.DrawLine(windowWidth-1, topY, windowWidth-1, bottomY);

  // ****************************************************************
  // Draw the mark.
  // ****************************************************************
  
  int graphX, graphY, graphW, graphH;
  data->gsTimeAxisGraph->getDimensions(graphX, graphY, graphW, graphH);

  x = data->gsTimeAxisGraph->getXPos( data->gesturalScoreMark_s );
  if ((x >= graphX) && (x < graphX + graphW))
  {
    dc.SetPen(wxPen(*wxRED, lineWidth));
    dc.DrawLine(x, 0, x, windowHeight);
  }
}


// ****************************************************************************
/// Calc. the coord. and heights of the individual gesture rows.
// ****************************************************************************

void GesturalScorePicture::calcGestureRowCoord()
{
  const int N = GesturalScore::NUM_GESTURE_TYPES;
  const int MIN_ROW_HEIGHT = 50;
  const int GAP_SIZE = 1;
  int i;
  int windowHeight = getWindowHeight();
  
  // ****************************************************************
  // All rows are visible at once.
  // ****************************************************************

  if (windowHeight >= N*(MIN_ROW_HEIGHT + GAP_SIZE))
  {
    for (i=0; i < N; i++)
    {
      gestureRowY[i] = i*(windowHeight - N*GAP_SIZE) / N + (i+1)*GAP_SIZE;
    }
    for (i=0; i < N-1; i++)
    {
      gestureRowH[i] = gestureRowY[i+1] - gestureRowY[i] - GAP_SIZE;
    }
    gestureRowH[N-1] = windowHeight - gestureRowY[N-1];
  }
  else

  // ****************************************************************
  // Not all rows are visible.
  // ****************************************************************

  {
    for (i=0; i < N; i++)
    {
      gestureRowY[i] = GAP_SIZE + i*(MIN_ROW_HEIGHT + GAP_SIZE);
      gestureRowH[i] = MIN_ROW_HEIGHT;
    }
  }
}


// ****************************************************************************
/// Calc. the coord. and heights of the individual parameter rows.
// ****************************************************************************

void GesturalScorePicture::calcParamRowCoord()
{
  const int MIN_ROW_HEIGHT = 50;
  const int GAP_SIZE = 1;
  int N = getNumControlParams();
  int i;
  int windowHeight = getWindowHeight();

  // ****************************************************************
  // All rows are visible at once.
  // ****************************************************************

  if (windowHeight >= N*(MIN_ROW_HEIGHT + GAP_SIZE))
  {
    for (i=0; i < N; i++)
    {
      paramRowY[i] = i*(windowHeight - N*GAP_SIZE) / N + (i+1)*GAP_SIZE;
    }
    for (i=0; i < N-1; i++)
    {
      paramRowH[i] = paramRowY[i+1] - paramRowY[i] - GAP_SIZE;
    }
    paramRowH[N-1] = windowHeight - paramRowY[N-1];
  }
  else

  // ****************************************************************
  // Not all rows are visible.
  // ****************************************************************

  {
    for (i=0; i < N; i++)
    {
      paramRowY[i] = GAP_SIZE + i*(MIN_ROW_HEIGHT + GAP_SIZE);
      paramRowH[i] = MIN_ROW_HEIGHT;
    }
  }
}


// ****************************************************************************
/// Truncates the string st such that it has the given maximal length.
// ****************************************************************************

void GesturalScorePicture::cutString(wxDC &dc, wxString &st, int maxWidth_px)
{
  while(dc.GetTextExtent(st).GetWidth() > maxWidth_px)
  {
  	if(st.size() > 0)
  	{
        st.Truncate(st.size() - 1);
  	}
    else
    {
        break;
    }
  	for (int i = 1; (i <= st.size())  && i < 4; ++i)
  	{
        st[st.size() - i] = '.';
  	}
  }
}


// ****************************************************************************
// Returns the information about the target under the mouse cursor.
// ****************************************************************************

void GesturalScorePicture::getUnderlyingGesture(int localX, int localY,
       int &gestureType, int &gestureIndex, bool &isOnBorder, bool &isOnTarget)
{
  int i, k;
  int size;
  int mx = localX;
  int my = localY;
  int leftX, rightX;
  double startTime_s;
  double endTime_s;
  Graph *graph = data->gsTimeAxisGraph;
  Gesture *gesture = NULL;
  GestureSequence *sequence = NULL;

  gestureType = -1;
  gestureIndex = -1;
  isOnBorder = false;
  isOnTarget = false;

  if (mx < Data::getInstance()->LEFT_SCORE_MARGIN)
  {
    return;    
  }

  // ****************************************************************
  // Run through all gestures.
  // ****************************************************************

  for (i=0; (i < GesturalScore::NUM_GESTURE_TYPES) && (gestureType == -1); i++)
  {
    if ((my >= gestureRowY[i]) && (my < gestureRowY[i] + gestureRowH[i]))
    {
      sequence = &gs->gestures[i];
      gestureType = i;
      size = sequence->numGestures();
      startTime_s = 0.0;

      for (k=0; (k < size) && (gestureIndex == -1); k++)
      {
        gesture = sequence->getGesture(k);
        endTime_s = startTime_s + gesture->duration_s;
        leftX = graph->getXPos(startTime_s);
        rightX = graph->getXPos(endTime_s);

        // Is it a border ?
        
        if ((mx > rightX-4) && (mx < rightX+4))
        {
          gestureIndex = k;
          isOnBorder = true;
        }
        else
        
        // Is it a target ?
        
        if ((mx >= leftX) && (mx < rightX))
        {
          gestureIndex = k;
          isOnBorder = false;

          // ********************************************************
          // Is the mouse over the target line ?
          // ********************************************************

          if (sequence->nominalValues == false)
          {
            double x0, y0, x1, y1;    // Screen coord. of the target line
            int windowWidth, windowHeight;

            this->GetSize(&windowWidth, &windowHeight);

            getTargetLineCoord(
              i, Data::getInstance()->LEFT_SCORE_MARGIN, windowWidth-1,
              startTime_s, gesture->dVal, 
              endTime_s, gesture->dVal + gesture->duration_s*gesture->slope,
              x0, y0, x1, y1);

            double deltaX = x1 - x0;
            if (deltaX < 0.000001)
            {
              deltaX = 0.000001;
            }
            double targetYAtCursor = y0 + (y1 - y0)*(mx - x0) / deltaX;

            if ((my > targetYAtCursor-4) && (my < targetYAtCursor+4)) 
            {
              isOnTarget = true; 
            }
          }
        }

        startTime_s = endTime_s;
      }
    }
  }
}


// ****************************************************************************
/// Returns the two points (x0, y0) and (x1, y1) for the line on the screen
/// that connects the two time-value pairs (t0, v0) and (t1, v1) of the given 
/// gesture type. Care is taken for clipping of the lines at the upper and
/// lower boundary of the tier on the screen.
// ****************************************************************************

void GesturalScorePicture::getTargetLineCoord(int gestureType, int leftBorderX, int rightBorderX,
  double t0, double v0, double t1, double v1, double &x0, double &y0, double &x1, double &y1)
{
  int i = gestureType;
  GestureSequence *sequence = &gs->gestures[gestureType];

  x0 = data->gsTimeAxisGraph->getXPos(t0);
  x1 = data->gsTimeAxisGraph->getXPos(t1);

  double min, max;
  int paramIndex = getRepresentativeParam(gestureType);
  getParamCurveMinMax(paramIndex, min, max);

  y0 = gestureRowY[i] + gestureRowH[i] - 1 - (int)((v0 - min)*gestureRowH[i] / (max - min));
  y1 = gestureRowY[i] + gestureRowH[i] - 1 - (int)((v1 - min)*gestureRowH[i] / (max - min));

  // ****************************************************************
  // Clipping of the line.
  // ****************************************************************

  const double EPSILON = 0.000001;
  double deltaX;
  double deltaY;
  double temp;
  int upperBorderY = gestureRowY[i];
  int lowerBorderY = gestureRowY[i] + gestureRowH[i] - 1;

  // Clipping at the left

  if (x0 < leftBorderX)
  {
    deltaX = x1 - x0;
    if (deltaX < EPSILON)
    {
      deltaX = EPSILON;
    }
    y0 = y0 + (y1 - y0)*(leftBorderX - x0) / deltaX;
    x0 = leftBorderX;
  }

  // Clipping at the right

  if (x1 > rightBorderX)
  {
    deltaX = x1 - x0;
    if (deltaX < EPSILON)
    {
      deltaX = EPSILON;
    }
    y1 = y0 + (y1 - y0)*(rightBorderX - x0) / deltaX;
    x1 = rightBorderX;
  }

  // Possibly swap the two points so that y0 is the upper point.

  if (y0 > y1)
  {
    temp = y0;
    y0 = y1;
    y1 = temp;

    temp = x0;
    x0 = x1;
    x1 = temp;
  }

  // Line is completely above the gesture box.

  if ((y0 < upperBorderY) && (y1 < upperBorderY))
  {
    y0 = upperBorderY;
    y1 = upperBorderY;
  }

  // Line is completely below the gesture box.

  if ((y0 > lowerBorderY) && (y1 > lowerBorderY))
  {
    y0 = lowerBorderY;
    y1 = lowerBorderY;
  }

  // Clipping at the top

  if (y0 < upperBorderY)
  {
    deltaY = y1 - y0;
    if (deltaY < EPSILON)
    {
      deltaY = EPSILON;
    }
    x0 = x0 + (x1 - x0)*(upperBorderY - y0) / deltaY;
    y0 = upperBorderY;
  }

  // Clipping at the bottom

  if (y1 > lowerBorderY)
  {
    deltaY = y1 - y0;
    if (deltaY < EPSILON)
    {
      deltaY = EPSILON;
    }
    x1 = x0 + (x1 - x0)*(lowerBorderY - y0) / deltaY;
    y1 = lowerBorderY;
  }

  // Possibly swap the two points so that x0 is the left point.

  if (x0 > x1)
  {
    temp = y0;
    y0 = y1;
    y1 = temp;

    temp = x0;
    x0 = x1;
    x1 = temp;
  }

}


// ****************************************************************************
/// Returns the minimum and maximum values of the given parameter. The 
/// parameter indices between 0 and VocalTract::NUM_PARAMS-1 correspond to
/// vocal tract model parameters, and those between VocalTract::NUM_PARAMS and
/// VocalTract::NUM_PARAMS + numGlottisParams - 1 to glottis parameters.
// ****************************************************************************

void GesturalScorePicture::getParamCurveMinMax(int paramIndex, double &min, double &max)
{
  int numParams = VocalTract::NUM_PARAMS + (int)gs->glottis->controlParam.size();
  min = 0.0;
  max = 1.0;

  if ((paramIndex < 0) || (paramIndex >= numParams))
  {
    return;
  }

  // ****************************************************************
  // Vocal tract model parameters.
  // ****************************************************************

  if (paramIndex < VocalTract::NUM_PARAMS)
  {
    min = gs->vocalTract->param[paramIndex].min;
    max = gs->vocalTract->param[paramIndex].max;
  }
  else

  // ****************************************************************
  // Glottis model parameters.
  // ****************************************************************

  {
    int k = paramIndex - VocalTract::NUM_PARAMS;
    min = gs->glottis->controlParam[k].min;
    max = gs->glottis->controlParam[k].max;

    // For the F0 parameters, take the limits in semitones of the gesture type.
    if (k == Glottis::FREQUENCY)
    {
      min = gs->gestures[GesturalScore::F0_GESTURE].minValue;
      max = gs->gestures[GesturalScore::F0_GESTURE].maxValue;
    }
  }

}


// ****************************************************************************
/// Returns the index of the representative parameter for the given gesture
/// type.
/// The parameter indices between 0 and VocalTract::NUM_PARAMS-1 correspond to
/// vocal tract model parameters, and those between VocalTract::NUM_PARAMS and
/// VocalTract::NUM_PARAMS + numGlottisParams - 1 to glottis parameters.
// ****************************************************************************

int GesturalScorePicture::getRepresentativeParam(int gestureType)
{
  int representativeParamIndex[GesturalScore::NUM_GESTURE_TYPES] =
  {
    VocalTract::TBX,              // Vowel gestures
    VocalTract::LD,
    VocalTract::TTY,
    VocalTract::TBY,
    VocalTract::VO,
    VocalTract::NUM_PARAMS + gs->glottis->getApertureParamIndex(),
    VocalTract::NUM_PARAMS + 0,    // F0 gestures
    VocalTract::NUM_PARAMS + 1,    // Pressure gestures
  };

  int index = 0;
  if ((gestureType >= 0) && (gestureType < GesturalScore::NUM_GESTURE_TYPES))
  {
    index = representativeParamIndex[gestureType];
  }

  return index;
}


// ****************************************************************************
/// Post an event to the parent page to update its widgets.
/// updateParam is either UPDATE_PICTURES or UPDATE_PICTURES_AND_CONTROLS.
// ****************************************************************************

void GesturalScorePicture::updatePage(int updateParam)
{
  wxCommandEvent event(updateRequestEvent);
  event.SetInt(updateParam);
  wxPostEvent(updateParent, event);

  // Important for good usability and to prevent delays of reactions 
  // on the screen to user input  with the mouse.
  wxYield();
}


// ****************************************************************************
/// React on any mouse events.
// ****************************************************************************

void GesturalScorePicture::OnMouseEvent(wxMouseEvent &event)
{
  int mx = event.GetX();
  int my = event.GetY();


  // ****************************************************************
  // The mouse is entering the window.
  // ****************************************************************

  if (event.Entering())
  {
    // Automatically set the focus to this window, so that
    // mouse wheel events are properly received !!
    this->SetFocus();
    
    moveBorder = false;
    moveTarget = false;
    lastMx = mx;
    lastMy = my;
    return;
  }
  
  // ****************************************************************
  // The wheel was rotated.
  // ****************************************************************

  if (event.GetWheelRotation() != 0)
  {
    if (event.GetWheelRotation() < 0)
    {
      verticalOffset_percent+= 20.0;
      if (verticalOffset_percent > 100.0)
      {
        verticalOffset_percent = 100.0;
      }
    }
    else
    if (event.GetWheelRotation() > 0)
    {
      verticalOffset_percent-= 20.0;
      if (verticalOffset_percent < 0.0)
      {
        verticalOffset_percent = 0.0;
      }
    }

    updatePage();
    lastMx = mx;
    lastMy = my;
    return;
  }

  // **************************************************************
  // This is the motor program -> set the mark only and return.
  // **************************************************************
  
  if (showGesturalScore == false) 
  {
    if ((event.LeftDown()) || (event.Dragging()))
    {
      data->gesturalScoreMark_s = data->gsTimeAxisGraph->getAbsXValue(mx);
      data->updateModelsFromGesturalScore();
      updatePage(UPDATE_PICTURES);
    }  
  
    lastMx = mx;
    lastMy = my;
    return;
  }


  // ****************************************************************
  // This a button-up event.
  // ****************************************************************

  if (event.ButtonUp())
  {
    moveBorder = false;
    moveTarget = false;
    lastMx = mx;
    lastMy = my;
    return;
  }

  // ****************************************************************
  // Mouse is outside the editable region.
  // ****************************************************************

  const int N = GesturalScore::NUM_GESTURE_TYPES;

  if ((mx <= Data::getInstance()->LEFT_SCORE_MARGIN) || (my < gestureRowY[0]) ||
      (my >= gestureRowY[N-1] + gestureRowH[N-1]))
  {
    lastMx = mx;
    lastMy = my;
    return;
  }

  // ****************************************************************
  // The left mouse button changed to down.
  // ****************************************************************

  if (event.LeftDown())
  {
    // Set the mark only
    if (event.ControlDown()) 
    {
      data->gesturalScoreMark_s = data->gsTimeAxisGraph->getAbsXValue(mx);
      data->updateModelsFromGesturalScore();
      updatePage(UPDATE_PICTURES);
    }
    else
    // Select a gesture
    {
      int gestureType, gestureIndex;
      bool isOnBorder, isOnTarget;
      getUnderlyingGesture(mx, my, gestureType, gestureIndex, isOnBorder, isOnTarget);
      data->selectedGestureType = gestureType;
      data->selectedGestureIndex = gestureIndex;
      moveBorder = isOnBorder;
      moveTarget = isOnTarget;

      // Gesture value can only be moved for non-neutral, non-nominal gestures
      if ((gestureType >= 0) && (gestureType < GesturalScore::NUM_GESTURE_TYPES))
      {
        GestureSequence *s = &gs->gestures[gestureType];
        if (s->nominalValues)
        {
          moveTarget = false;
        }
        if ((s->getGesture(gestureIndex) != NULL) && (s->getGesture(gestureIndex)->neutral))
        {
          moveTarget = false;
        }
      }
      updatePage(UPDATE_PICTURES_AND_CONTROLS);
    }

    lastMx = mx;
    lastMy = my;
    return;
  }

  // ****************************************************************
  // The right mouse button changed to down.
  // ****************************************************************

  if (event.RightDown())
  {
    // Select the gesture at (mx, my)
    int gestureType, gestureIndex;
    bool isOnBorder, isOnTarget;
    getUnderlyingGesture(mx, my, gestureType, gestureIndex, isOnBorder, isOnTarget);
    data->selectedGestureType = gestureType;
    data->selectedGestureIndex = gestureIndex;
    moveBorder = false;
    moveTarget = false;

    data->gesturalScoreMark_s = data->gsTimeAxisGraph->getAbsXValue(mx);

    // Update the numeric values in the analysis results dialog.
    AnalysisResultsDialog *dialog = AnalysisResultsDialog::getInstance();
    if (dialog->IsShown())
    {
      dialog->updateWidgets();
    }

    updatePage(UPDATE_PICTURES_AND_CONTROLS);
    data->updateModelsFromGesturalScore();

    menuX = mx;
    menuY = my;
	  PopupMenu(contextMenu);
    return;
  }

  // ****************************************************************
  // Is this a mouse move event (no mouse buttons pressed at the same
  // time) ?
  // ****************************************************************

  if (event.Moving())
  {
    // Change the type of the cursor when a border between two target
    // segments is under the cursor.

    int gestureType, gestureIndex;
    bool isOnBorder, isOnTarget;
    getUnderlyingGesture(mx, my, gestureType, gestureIndex, isOnBorder, isOnTarget);

    if ((isOnBorder) || (moveBorder))
    {
      this->SetCursor( wxCursor(wxCURSOR_SIZEWE) );
    }
    else
    if ((isOnTarget) || (moveTarget))
    {
      this->SetCursor( wxCursor(wxCURSOR_SIZENS) );
    }
    else
    {
      this->SetCursor( wxCursor(wxCURSOR_ARROW) );
    }

    lastMx = mx;
    lastMy = my;
    return;
  }

  // ****************************************************************
  // The mouse is dragged (with one or more mouse buttons pressed).
  // ****************************************************************

  if (event.Dragging())
  {
    int gestureType, gestureIndex;
    bool isOnBorder, isOnTarget;
    getUnderlyingGesture(mx, my, gestureType, gestureIndex, isOnBorder, isOnTarget);

    // **************************************************************
    // Move the border between two gesture intervals.
    // **************************************************************

    if (moveBorder)
    {
      Gesture *gesture = data->getSelectedGesture();
      if (gesture != NULL)
      {
        GestureSequence *sequence = &gs->gestures[data->selectedGestureType];

        // There are two modi to move the border (with or without SHIFT)

        if ((event.ShiftDown()) && (sequence->numGestures() > data->selectedGestureIndex + 1))
        {
          Gesture *nextGesture = sequence->getGesture(data->selectedGestureIndex + 1);
          double cursorPos_s = data->gsTimeAxisGraph->getAbsXValue(mx);
          double gesturePos_s = sequence->getGestureBegin_s(data->selectedGestureIndex);
          const double EPSILON = 0.001;     // = 1 ms

          if (cursorPos_s < gesturePos_s + EPSILON) 
          { 
            cursorPos_s = gesturePos_s + EPSILON; 
          }

          if (cursorPos_s > gesturePos_s + gesture->duration_s + nextGesture->duration_s - EPSILON) 
          { 
            cursorPos_s = gesturePos_s + gesture->duration_s + nextGesture->duration_s - EPSILON; 
          }

          if (cursorPos_s < gesturePos_s) 
          { 
            cursorPos_s = gesturePos_s; 
          }

          double deltaPos_s = cursorPos_s - (gesturePos_s + gesture->duration_s);
          gesture->duration_s+= deltaPos_s;
          nextGesture->duration_s-= deltaPos_s;
        }
        else
        {
          gesture->duration_s = data->gsTimeAxisGraph->getAbsXValue(mx) - 
            sequence->getGestureBegin_s(data->selectedGestureIndex);
         
          sequence->limitGestureParams(*gesture);
        }

        updatePage(UPDATE_PICTURES_AND_CONTROLS);
      }
    }
    else

    // **************************************************************
    // Move the target value of a gesture up or down.
    // **************************************************************

    if (moveTarget)
    {
      Gesture *g = data->getSelectedGesture();
      if ((g != NULL) && (gestureType == data->selectedGestureType))
      {
        int dy = my - lastMy;

        double min = 0.0;
        double max = 0.0;
        int paramIndex = getRepresentativeParam(gestureType);
        getParamCurveMinMax(paramIndex, min, max);
        
        double rowHeight = (double)gestureRowH[data->selectedGestureType];
        if (rowHeight < 0.000001)
        {
          rowHeight = 0.000001;
        }

        g->dVal-= dy*(max - min) / rowHeight;
        gs->gestures[data->selectedGestureType].limitGestureParams(*g);
        updatePage(UPDATE_PICTURES_AND_CONTROLS);
      }
      else
      {
        moveTarget = false;
        this->SetCursor( wxCursor(wxCURSOR_ARROW) );
      }
    }
    else

    // **************************************************************
    // Set the mark only.
    // **************************************************************
    
    if (event.ControlDown()) 
    {
      data->gesturalScoreMark_s = data->gsTimeAxisGraph->getAbsXValue(mx);

      // Update the numeric values in the analysis results dialog.
      AnalysisResultsDialog *dialog = AnalysisResultsDialog::getInstance();
      if (dialog->IsShown())
      {
        dialog->updateWidgets();
      }

      updatePage(UPDATE_PICTURES);
    }

    data->updateModelsFromGesturalScore();
  }

  lastMx = mx;
  lastMy = my;
}


// ****************************************************************************
/// Insert a gesture.
// ****************************************************************************

void GesturalScorePicture::OnInsertGesture(wxCommandEvent &event)
{
  double cursorPos_s = data->gsTimeAxisGraph->getAbsXValue(menuX);
  
  int gestureType, gestureIndex;
  bool isOnBorder, isOnTarget;
  getUnderlyingGesture(menuX, menuY, gestureType, gestureIndex, isOnBorder, isOnTarget);

  if ((gestureType < 0) || (gestureType >= GesturalScore::NUM_GESTURE_TYPES)) 
  { 
    return; 
  }

  GestureSequence *sequence = &gs->gestures[gestureType];
  gestureIndex = sequence->getIndexAt(cursorPos_s);

  // ****************************************************************
  // Split the existing gesture.
  // ****************************************************************

  if (sequence->isValidIndex(gestureIndex))
  {
    Gesture *gesture = sequence->getGesture(gestureIndex);
    double startPos_s = sequence->getGestureBegin_s(gestureIndex);
    gesture->duration_s = startPos_s + gesture->duration_s - cursorPos_s;

    Gesture newGesture = *gesture;
    newGesture.duration_s = cursorPos_s - startPos_s;
    sequence->insertGesture(newGesture, gestureIndex);

    sequence->limitGestureParams( *sequence->getGesture(gestureIndex) );
    sequence->limitGestureParams( *sequence->getGesture(gestureIndex+1) );

    data->selectedGestureIndex = gestureIndex;
  }
  else

  // ****************************************************************
  // Add a new gesture to the end of the sequence.
  // ****************************************************************

  {
    Gesture newGesture;
    sequence->initGestureParams(newGesture);
    newGesture.duration_s = cursorPos_s - sequence->getDuration_s();
    // For f0 gestures, assign a greater time constant than the default.
    if (gestureType == GesturalScore::F0_GESTURE)
    {
      newGesture.tau_s = 0.020;   // 20 ms
    }
    sequence->limitGestureParams(newGesture);
    sequence->appendGesture(newGesture);

    data->selectedGestureIndex = sequence->numGestures() - 1;
  }

  updatePage(UPDATE_PICTURES_AND_CONTROLS);
  data->updateModelsFromGesturalScore();
}


// ****************************************************************************
/// Delete a gesture.
// ****************************************************************************

void GesturalScorePicture::OnDeleteGesture(wxCommandEvent &event)
{
  int gestureType, gestureIndex;
  bool isOnBorder, isOnTarget;
  getUnderlyingGesture(menuX, menuY, gestureType, gestureIndex, isOnBorder, isOnTarget);

  if ((gestureType < 0) || (gestureType >= GesturalScore::NUM_GESTURE_TYPES))
  { 
    return; 
  }

  GestureSequence *sequence = &gs->gestures[gestureType];
  if (sequence->isValidIndex(gestureIndex))
  {
    double duration_s = sequence->getGesture(gestureIndex)->duration_s;
    sequence->deleteGesture(gestureIndex);
    if (gestureIndex > 0) 
    { 
      sequence->getGesture(gestureIndex-1)->duration_s+= duration_s; 
    }
  }

  updatePage(UPDATE_PICTURES_AND_CONTROLS);
  data->updateModelsFromGesturalScore();
}


// ****************************************************************************
// ****************************************************************************

void GesturalScorePicture::OnSetInitialScore(wxCommandEvent &event)
{
  if (wxMessageBox("Do you really want to delete the current gestural score and set the initial score instead?",
    "Are you sure?", wxYES_NO) == wxYES)
  {
    gs->initTestScore();

    updatePage(UPDATE_PICTURES_AND_CONTROLS);
    data->updateModelsFromGesturalScore();
  }
}


// ****************************************************************************
// ****************************************************************************

void GesturalScorePicture::OnScoreFromSegmentSequence(wxCommandEvent &event)
{
  if (wxMessageBox("Do you really want to delete the current gestural score and calculate a new one from the segment sequence?",
    "Are you sure?", wxYES_NO) == wxYES)
  {
    wxBusyInfo dialog("Please wait a moment...");

    gs->createFromSegmentSequence(data->segmentSequence);

    updatePage(UPDATE_PICTURES_AND_CONTROLS);
    data->updateModelsFromGesturalScore();
  }
}

// ****************************************************************************
