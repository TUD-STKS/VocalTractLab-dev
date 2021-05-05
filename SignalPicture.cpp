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

#include "AnalysisResultsDialog.h"
#include "SignalPicture.h"
#include "Data.h"
#include "VocalTractLabApi/src/Dsp.h"


static const wxPen trackPen[Data::NUM_TRACKS] = 
{
  *wxBLACK_PEN,
  wxPen( wxColor(0, 190, 0) ),
  *wxRED_PEN
};

// ****************************************************************************
// IDs.
// ****************************************************************************

static const int IDM_CLEAR_SELECTION      = 1000;
static const int IDM_SET_SELECTION_START  = 1001;
static const int IDM_SET_SELECTION_END    = 1002;
static const int IDM_SELECT_ALL           = 1003;


// ****************************************************************************
// The event table.
// ****************************************************************************

BEGIN_EVENT_TABLE(SignalPicture, BasicPicture)
  EVT_MOUSE_EVENTS(SignalPicture::OnMouseEvent)
  EVT_MENU(IDM_CLEAR_SELECTION, SignalPicture::OnClearSelection)
  EVT_MENU(IDM_SET_SELECTION_START, SignalPicture::OnSetSelectionStart)
  EVT_MENU(IDM_SET_SELECTION_END, SignalPicture::OnSetSelectionEnd)
  EVT_MENU(IDM_SELECT_ALL, SignalPicture::OnSelectAll)
END_EVENT_TABLE()


// ****************************************************************************
/// Construcor. Passes the parent parameter.
// ****************************************************************************

SignalPicture::SignalPicture(wxWindow *parent, wxWindow *updateParent) : BasicPicture(parent)
{
  // Init some variables.
  
  data = Data::getInstance();
  menuX = 0;
  menuY = 0;
  lastMx = 0;
  lastMy = 0;
  moveLeftBorder = false;
  moveRightBorder = false;
  this->updateParent = updateParent;

  // Init the context menu.
  
  contextMenu = new wxMenu();
  contextMenu->Append(IDM_CLEAR_SELECTION, "Clear selection");
  contextMenu->Append(IDM_SET_SELECTION_START, "Set selection start");
  contextMenu->Append(IDM_SET_SELECTION_END, "Set selection end");
  contextMenu->Append(IDM_SELECT_ALL, "Select all");
}


// ****************************************************************************
// ****************************************************************************

void SignalPicture::draw(wxDC &dc)
{
  int w, h;
  this->GetSize(&w, &h);

  int upperHeight = getUpperPictureHeight();
  paintUpperOscillogram(dc, w, upperHeight);
  paintLowerOscillogram(dc, upperHeight, w, h - upperHeight);
}


// ****************************************************************************
/// Returns the sample index of the audio signal under the position (mx, my)
/// in the picture. The sample index differs depending on whether the position
/// is on the upper or lower oscillogram.
/// When constrain = true, the result is constrained to 0 .. N-1.
// ****************************************************************************

int SignalPicture::getSampleIndex(int x, int y, bool constrain)
{
  int width, height;
  this->GetSize(&width, &height);
  int sampleIndex = 0;

  if (y < getUpperPictureHeight())
  {
    sampleIndex = data->centerPos_pt + (x - width/2)*data->oscillogramVisTimeRange_pt/width;
  }
  else
  {
    sampleIndex = data->centerPos_pt + (x - width/2)*data->spectrogramVisTimeRange_pt/width;
  }

  if (constrain)
  {
    if (sampleIndex < 0) 
    { 
      sampleIndex = 0; 
    }
    if (sampleIndex >= data->track[Data::MAIN_TRACK]->N) 
    { 
      sampleIndex = data->track[Data::MAIN_TRACK]->N-1; 
    }
  }

  return sampleIndex;
}


// ****************************************************************************
/// Returns the x-position of the pixel at the given sample index for either
/// the upper oscillogram or the lower oscillogram.
// ****************************************************************************

int SignalPicture::getXPos(int sampleIndex, bool atUpperOscillogram)
{
  int width, height;
  this->GetSize(&width, &height);
  int x = 0;

  if (atUpperOscillogram)
  {
    int startPos = data->centerPos_pt - data->oscillogramVisTimeRange_pt / 2;
    x = ((sampleIndex - startPos)*width) / (int)data->oscillogramVisTimeRange_pt;
  }
  else
  {
    int startPos = data->centerPos_pt - data->spectrogramVisTimeRange_pt / 2;
    x = ((sampleIndex - startPos)*width) / (int)data->spectrogramVisTimeRange_pt;
  }

  return x;
}

// ****************************************************************************
/// Draws a mark at the left or right margin of the selected region.
// ****************************************************************************

void SignalPicture::drawSelectionMark(wxDC &dc, int x, int y1, int y2, bool isLeftMark)
{
  wxPen oldPen = dc.GetPen();
  
  dc.SetPen(wxPen(*wxBLACK, 1, wxPENSTYLE_DOT));
  dc.DrawLine(x, y1, x, y2);

  dc.SetPen(*wxBLACK_PEN);
  dc.SetBrush(*wxBLACK_BRUSH);
  const int W = 8;
  
  if (isLeftMark)
  {
    wxPoint topLeftTriangle[3] = { wxPoint(x, y1), wxPoint(x+W, y1), wxPoint(x, y1+W) };
    wxPoint bottomLeftTriangle[3] = { wxPoint(x, y2), wxPoint(x+W, y2), wxPoint(x, y2-W) };
    dc.DrawPolygon(3, topLeftTriangle);
    dc.DrawPolygon(3, bottomLeftTriangle);
  }
  else
  {
    wxPoint topRightTriangle[3] = { wxPoint(x, y1), wxPoint(x-W, y1), wxPoint(x, y1+W) };
    wxPoint bottomRightTriangle[3] = { wxPoint(x, y2), wxPoint(x-W, y2), wxPoint(x, y2-W) };
    dc.DrawPolygon(3, topRightTriangle);
    dc.DrawPolygon(3, bottomRightTriangle);
  }

  dc.SetPen(oldPen);
}

// ****************************************************************************
// ****************************************************************************

void SignalPicture::paintUpperOscillogram(wxDC &dc, int w, int h)
{
  int startPos = data->centerPos_pt - data->oscillogramVisTimeRange_pt / 2;

  // ****************************************************************
  // Paint the background.
  // ****************************************************************

  int zeroX = getXPos(0, true); //w/2 - data->centerPos_pt*w / data->oscillogramVisTimeRange_pt;

  // Make the pen transparent.
  dc.SetPen(wxPen(*wxWHITE, 0, wxPENSTYLE_TRANSPARENT));

  // Whole background is white.
  if (zeroX < 0)
  {
    dc.SetBrush(*wxWHITE_BRUSH);
    dc.DrawRectangle(0, 0, w, h);
    // Set zeroX to zero!
    zeroX = 0;
  }
  else

  // A part of the background is grey, because the time is partly negative.
  {
    if (zeroX >= w)
    {
      zeroX = w - 1;
    }
    dc.SetBrush(*wxGREY_BRUSH);
    dc.DrawRectangle(0, 0, zeroX, h);

    dc.SetBrush(*wxWHITE_BRUSH);
    dc.DrawRectangle(zeroX, 0, w-zeroX, h);
  }

  // Horizontal center line
  if (data->showSpectrogramText) 
  { 
    wxPen grayPen( wxColor(240, 240, 240) );
    dc.SetPen(grayPen);
    dc.DrawLine(zeroX, h/2, w-1, h/2); 
  }

  // ****************************************************************
  // Overly the selected region.
  // ****************************************************************
  
  if (data->isValidSelection())
  {
    int x1 = getXPos(data->selectionMark_pt[0], true);
    int x2 = getXPos(data->selectionMark_pt[1], true);
    if ((x1 < w) && (x2 >= 0))
    {
      if (x1 < 0)
      {
        x1 = 0;
      }
      if (x2 >= w)
      {
        x2 = w-1;
      }

      dc.SetPen(wxPen(*wxBLACK, 1, wxPENSTYLE_TRANSPARENT));    // no pen
      dc.SetBrush( wxBrush(wxColor(220, 220, 220)) );
      dc.DrawRectangle(x1, 0, x2 - x1, h/4);
      dc.DrawRectangle(x1, 3*h/4, x2 - x1, h/4);
    }
  }

  // ****************************************************************
  // Paint the signals
  // ****************************************************************

  int x, k;
  int sampleIndex;
  int lastY[Data::NUM_TRACKS] = { 0, 0, 0 };
  int y;
  double yFactor = (double)h * data->oscillogramAmpZoom / 65536.0;

  if (zeroX < 0)
  {
    zeroX = 0;
  }

  for (x=zeroX; x < w; x++)
  {
    sampleIndex = startPos + x*data->oscillogramVisTimeRange_pt / w;
    sampleIndex = modulo(sampleIndex, data->track[data->MAIN_TRACK]->N);

    // Paint the lines

    for (k = data->NUM_TRACKS-1; k >= 0; k--)
    {
      if (data->showTrack[k])
      {
        y = h/2 - (int)(data->track[k]->x[sampleIndex] * yFactor);
        if (y < 0)
        {
          y = 0;
        }
        if (y >= h)
        {
          y = h-1;
        }

        if (x > zeroX)
        {
          dc.SetPen(trackPen[k]);
          dc.DrawLine(x-1, lastY[k], x, y);
        }
        lastY[k] = y;
      }
    }
  }

  // ****************************************************************
  // Paint the two selection marks and the main mark.
  // ****************************************************************

  x = getXPos(data->selectionMark_pt[0], true);
  if ((data->selectionMark_pt[0] >= 0) && (x >= 0) && (x < w))
  {
    drawSelectionMark(dc, x, 0, h-1, true);
  }

  x = getXPos(data->selectionMark_pt[1], true);
  if ((data->selectionMark_pt[1] >= 0) && (x >= 0) && (x < w))
  {
    drawSelectionMark(dc, x, 0, h-1, false);
  }

  x = getXPos(data->mark_pt, true);
  if ((x >= 0) && (x < w))
  {
    dc.SetPen(*wxBLACK_PEN);
    dc.DrawLine(x, 0, x, h-1);
  }

  // ****************************************************************
  // Output some text.
  // ****************************************************************

  if (data->showSpectrogramText)
  {
    dc.SetPen(*wxBLACK_PEN);
    dc.SetBackgroundMode(wxSOLID);    // Set a solid white background
    dc.SetFont(wxFont(9, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

    wxString st;

    st = wxString::Format(
      "View range: %d ms",
      (int)((double)data->oscillogramVisTimeRange_pt*1000.0 / SAMPLING_RATE));
    dc.DrawText(st, 0, 0);

    st = wxString::Format(
      "Mark pos.: %2.4f s",
      (double)data->mark_pt / (double)SAMPLING_RATE);
    dc.DrawText(st, 150, 0);

    if (data->isValidSelection())
    {
      int left = data->selectionMark_pt[0];
      int right = data->selectionMark_pt[1];
      double denominator = right - left;
      if (denominator == 0.0) 
      { 
        denominator = 1.0; 
      }
      double f = (double)SAMPLING_RATE / denominator;

      st = wxString::Format("Selection: %2.3f ... %2.3f s (%2.3f s = %2.1f Hz)",
        (double)left / (double)SAMPLING_RATE,
        (double)right / (double)SAMPLING_RATE,
        (double)(right-left) / (double)SAMPLING_RATE,
        f);
      dc.DrawText(st, 320, 0);
    }

  }

  // Horizontal black line at the bottom to sepearate the smaller osc.

  dc.SetPen(*wxBLACK_PEN);
  dc.DrawLine(0, h-1, w-1, h-1); 
}


// ****************************************************************************
// ****************************************************************************

void SignalPicture::paintLowerOscillogram(wxDC &dc, int yOffset, int w, int h)
{
  int startPos = data->centerPos_pt - data->spectrogramVisTimeRange_pt/2;
  int numSamples  = data->spectrogramVisTimeRange_pt;

  // ****************************************************************
  // Paint the background.
  // ****************************************************************

  int zeroX = w/2 - (data->centerPos_pt*w) / data->spectrogramVisTimeRange_pt;
  wxColor gray(240, 240, 240);

  // Make the pen transparent.
  dc.SetPen(wxPen(*wxWHITE, 0, wxPENSTYLE_TRANSPARENT));

  // Whole background is light gray.
  if (zeroX < 0)
  {
    dc.SetBrush(gray);
    dc.DrawRectangle(0, yOffset, w, h);
    // Set zeroX to zero!
    zeroX = 0;
  }
  else

  // A part of the background is grey, because the time is partly negative.
  {
    if (zeroX >= w)
    {
      zeroX = w - 1;
    }
    dc.SetBrush(*wxGREY_BRUSH);
    dc.DrawRectangle(0, yOffset, zeroX, h);

    dc.SetBrush(gray);
    dc.DrawRectangle(zeroX, yOffset, w-zeroX, h);
  }

  // The area that corresponds to the upper oscillogram view

  int leftX  = w/2 - (w*(int)data->oscillogramVisTimeRange_pt) / 
                      (2*(int)data->spectrogramVisTimeRange_pt);
  int rightX = w/2 + (w*(int)data->oscillogramVisTimeRange_pt) / 
                      (2*(int)data->spectrogramVisTimeRange_pt);

  if (leftX < zeroX)
  {
    leftX = zeroX;
  }

  dc.SetBrush(*wxWHITE_BRUSH);
  dc.DrawRectangle(leftX, yOffset, rightX - leftX, h);

  // Horizontal center line
  if (data->showSpectrogramText) 
  { 
    dc.SetPen(*wxWHITE_PEN);
    dc.DrawLine(zeroX, yOffset + h/2, w-1, yOffset + h/2); 
  }

  // ****************************************************************
  // Overly the selected region.
  // ****************************************************************
  
  if (data->isValidSelection())
  {
    int x1 = ((data->selectionMark_pt[0] - startPos)*w) / numSamples;
    int x2 = ((data->selectionMark_pt[1] - startPos)*w) / numSamples;
    if ((x1 < w) && (x2 >= 0))
    {
      if (x1 < 0)
      {
        x1 = 0;
      }
      if (x2 >= w)
      {
        x2 = w-1;
      }

      dc.SetPen(wxPen(*wxBLACK, 1, wxPENSTYLE_TRANSPARENT));    // no pen
      dc.SetBrush( wxBrush(wxColor(220, 220, 220)) );
      dc.DrawRectangle(x1, yOffset, x2 - x1, h/4);
      dc.DrawRectangle(x1, yOffset + 3*h/4, x2 - x1, h/4);
    }
  }

  // ****************************************************************

  int x, i, k;
  int sampleIndex;
  int lastY[Data::NUM_TRACKS] = { 0, 0, 0 };
  int lastX = 0;
  int y;

  double yFactor = 2.0*(double)h*data->oscillogramAmpZoom / 65536.0;

  // ****************************************************************
  // Run through all samples.
  // ****************************************************************

  for (i=0; i < numSamples; i++)
  {
    sampleIndex = startPos + i;
    sampleIndex = modulo(sampleIndex, data->track[Data::MAIN_TRACK]->N);

    x = i*w / numSamples;

    // Draw the line

    for (k = Data::NUM_TRACKS-1; k >= 0; k--)
    {
      if (data->showTrack[k])
      {
        y = yOffset + (h/2) - (int)(data->track[k]->x[sampleIndex]*yFactor);
        if (y < yOffset)
        {
          y = yOffset;
        }
        if (y >= yOffset + h)
        {
          y = yOffset + h - 1;
        }

        if (x > zeroX)
        {
          dc.SetPen(trackPen[k]);
          dc.DrawLine(lastX, lastY[k], x, y);
        }

        lastY[k] = y;
      }
    }

    lastX = x;
  }

  // ****************************************************************
  // Draw the selection marks and the main mark.
  // ****************************************************************
  
  x = ((data->selectionMark_pt[0] - startPos)*w) / numSamples;
  if ((data->selectionMark_pt[0] >= 0) && (x >= 0) && (x < w))
  {
    drawSelectionMark(dc, x, yOffset, yOffset+h-1, true);
  }

  x = ((data->selectionMark_pt[1] - startPos)*w) / numSamples;
  if ((data->selectionMark_pt[1] >= 0) && (x >= 0) && (x < w))
  {
    drawSelectionMark(dc, x, yOffset, yOffset+h-1, false);
  }

  x = ((data->mark_pt - startPos)*w) / numSamples;
  if ((x >= 0) && (x < w))
  {
    dc.SetPen(*wxBLACK_PEN);
    dc.DrawLine(x, yOffset, x, yOffset+h-1);
  }

}

// ****************************************************************************
/// Returns the height of the upper one of the two pictures.
// ****************************************************************************

int SignalPicture::getUpperPictureHeight()
{
  int w, h;
  this->GetSize(&w, &h);
  return (h*2/3);
}


// ****************************************************************************
/// Process all mouse events.
// ****************************************************************************

void SignalPicture::OnMouseEvent(wxMouseEvent &event)
{
  int width, height;
  this->GetSize(&width, &height);

  int mx = event.GetX();
  int my = event.GetY();

  // Which signal sample is under the mouse cursor ?
  int sampleIndex = getSampleIndex(mx, my, true);

  // ****************************************************************
  // The x-position in pixels of the selection region borders.
  // ****************************************************************

  int leftBorderX = 0;
  int rightBorderX = 0;
  int upperPictureHeight = getUpperPictureHeight();

  // Is there a border of the selection region under the mouse ?
  if (my < upperPictureHeight)
  {
    leftBorderX = getXPos(data->selectionMark_pt[0], true);
    rightBorderX = getXPos(data->selectionMark_pt[1], true);
  }
  else
  {
    leftBorderX = getXPos(data->selectionMark_pt[0], false);
    rightBorderX = getXPos(data->selectionMark_pt[1], false);
  }

  bool isOnLeftBorder = false;
  bool isOnRightBorder = false;

  int M = 4;
  if ((mx > leftBorderX - M) && (mx < leftBorderX + M))
  {
    isOnLeftBorder = true;
  }
  else
  if ((mx > rightBorderX - M) && (mx < rightBorderX + M))
  {
    isOnRightBorder = true;
  }

  // ****************************************************************
  // When the mouse moves from one picture to the other stop dragging
  // the borders.
  // ****************************************************************

  if (((my < upperPictureHeight) && (lastMy >= upperPictureHeight)) ||
      ((my >= upperPictureHeight) && (lastMy < upperPictureHeight)))
  {
    moveLeftBorder = false;
    moveRightBorder = false;
  }

  // ****************************************************************
  // The mouse is entering the window.
  // ****************************************************************

  if ((event.Entering()) || (event.Leaving()))
  {
    moveLeftBorder = false;
    moveRightBorder = false;
    lastMx = mx;
    lastMy = my;
    return;
  }

  // ****************************************************************
  // This a button-up event.
  // ****************************************************************

  if (event.ButtonUp())
  {
    moveLeftBorder = false;
    moveRightBorder = false;
    lastMx = mx;
    lastMy = my;
    return;
  }

  // ****************************************************************
  // The left mouse button changed to down.
  // ****************************************************************

  if (event.LeftDown())
  {
    if (isOnLeftBorder)
    {
      moveLeftBorder = true;
    }
    else
    if (isOnRightBorder)
    {
      moveRightBorder = true;
    }
    else
    {
      data->mark_pt = sampleIndex;

      // Update all pictures on the signal page.
      wxCommandEvent event(updateRequestEvent);
      event.SetInt(REFRESH_PICTURES);
      wxPostEvent(updateParent, event);

      // Update the numeric values in the analysis results dialog.
      AnalysisResultsDialog *dialog = AnalysisResultsDialog::getInstance();
      if (dialog->IsShown())
      {
        dialog->updateWidgets();
      }

    }

    lastMx = mx;
    lastMy = my;
    return;
  }

  // ****************************************************************
  // Was the mouse moved? The possibly change the cursor type.   
  // ****************************************************************

  if (event.Moving())
  {
    // Change the type of the cursor when a border of the selection 
    // region is under the cursor.
    if ((isOnLeftBorder) || (isOnRightBorder) || (moveLeftBorder) || (moveRightBorder))
    {
      this->SetCursor( wxCursor(wxCURSOR_SIZEWE) );
    }
    else
    {
      this->SetCursor( wxCursor(wxCURSOR_ARROW) );
    }
  }

  // ****************************************************************
  // The mouse is dragged (with one or more mouse buttons pressed).
  // ****************************************************************

  if (event.Dragging())
  {
    if (moveLeftBorder)
    {
      data->selectionMark_pt[0] = getSampleIndex(mx, my, true);
      this->Refresh();
    }
    else
    if (moveRightBorder)
    {
      data->selectionMark_pt[1] = getSampleIndex(mx, my, true);
      this->Refresh();
    }
    else
    {
      data->mark_pt = sampleIndex;

      // Update all pictures on the signal page.
      wxCommandEvent event(updateRequestEvent);
      event.SetInt(REFRESH_PICTURES);
      wxPostEvent(updateParent, event);

      // Update the numeric values in the analysis results dialog.
      AnalysisResultsDialog *dialog = AnalysisResultsDialog::getInstance();
      if (dialog->IsShown())
      {
        dialog->updateWidgets();
      }

    }
  }

  // ****************************************************************
  // The right mouse button changed to down. Call the context menu.
  // ****************************************************************

  if (event.RightDown())
  {
    menuX = mx;
    menuY = my;
	  PopupMenu(contextMenu);
    return;
  }

}


// ****************************************************************************
// ****************************************************************************

void SignalPicture::OnClearSelection(wxCommandEvent &event)
{
  data->selectionMark_pt[0] = -1;
  data->selectionMark_pt[1] = -1;
  this->Refresh();
}


// ****************************************************************************
// ****************************************************************************

void SignalPicture::OnSetSelectionStart(wxCommandEvent &event)
{
  data->selectionMark_pt[0] = getSampleIndex(menuX, menuY, true);
  this->Refresh();
}


// ****************************************************************************
// ****************************************************************************

void SignalPicture::OnSetSelectionEnd(wxCommandEvent &event)
{
  data->selectionMark_pt[1] = getSampleIndex(menuX, menuY, true);
  this->Refresh();
}

// ****************************************************************************
// ****************************************************************************

void SignalPicture::OnSelectAll(wxCommandEvent &event)
{
  data->selectionMark_pt[0] = 0;
  data->selectionMark_pt[1] = Data::TRACK_DURATION_S*SAMPLING_RATE - 1;
  this->Refresh();
}

// ****************************************************************************

