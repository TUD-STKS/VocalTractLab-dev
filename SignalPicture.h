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

#ifndef __SIGNAL_PICTURE_H__
#define __SIGNAL_PICTURE_H__

#include "BasicPicture.h"
#include "Graph.h"
#include "Data.h"

// ****************************************************************************
// ****************************************************************************

class SignalPicture : public BasicPicture
{
  // **************************************************************************
  // Public data.
  // **************************************************************************

public:

  // **************************************************************************
  // Public functions.
  // **************************************************************************

public:
  SignalPicture(wxWindow *parent, wxWindow *updateParent);
  virtual void draw(wxDC &dc);

  // **************************************************************************
  // Private data.
  // **************************************************************************

private:
  Data *data;
  wxMenu *contextMenu;
  int menuX, menuY;
  int lastMx, lastMy;
  bool moveLeftBorder;
  bool moveRightBorder;
  wxWindow *updateParent;
  int lineWidth{ this->FromDIP(1) };
  const wxPen trackPen[Data::NUM_TRACKS] = {
  wxPen(*wxBLACK, lineWidth),
  wxPen(wxColor(0, 190, 0), lineWidth),
  wxPen(*wxBLACK, lineWidth),
  };

  // **************************************************************************
  // Private functions.
  // **************************************************************************

private:
  int getSampleIndex(int x, int y, bool constrain);
  int getXPos(int sampleIndex, bool atUpperOscillogram);
  void drawSelectionMark(wxDC &dc, int x, int y1, int y2, bool isLeftMark);
  void paintUpperOscillogram(wxDC &dc, int w, int h);
  void paintLowerOscillogram(wxDC &dc, int yOffset, int w, int h);
  int getUpperPictureHeight();

  void OnMouseEvent(wxMouseEvent &event);
  void OnClearSelection(wxCommandEvent &event);
  void OnSetSelectionStart(wxCommandEvent &event);
  void OnSetSelectionEnd(wxCommandEvent &event);
  void OnSelectAll(wxCommandEvent &event);

  // ****************************************************************************
  // Declare the event table right at the end
  // ****************************************************************************

  DECLARE_EVENT_TABLE()
};

#endif

// ****************************************************************************
