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

#ifndef __POLE_ZERO_PLOT_H__
#define __POLE_ZERO_PLOT_H__

#include "Data.h"
#include "BasicPicture.h"
#include "Graph.h"

// ****************************************************************************
/// This class represents a picture showing a pole-zero plot.
// ****************************************************************************

class PoleZeroPlot : public BasicPicture
{
  // **************************************************************************
  // Public functions.
  // **************************************************************************

public:
  PoleZeroPlot(wxWindow *parent);
  virtual void draw(wxDC &dc);

  // **************************************************************************
  // Private data.
  // **************************************************************************

private:
  Graph graph;
  wxMenu *contextMenu;
  PoleZeroPlan *plan;

  int menuX;
  int menuY;
  int lastMx;
  int lastMy;

  // **************************************************************************
  // Private functions.
  // **************************************************************************

private:
  void updateParent();
  int getNearPole(int x, int y);
  int getNearZero(int x, int y);
  void OnMouseEvent(wxMouseEvent &event);

  void OnInsertPole(wxCommandEvent &event);
  void OnInsertZero(wxCommandEvent &event);
  void OnDeletePole(wxCommandEvent &event);
  void OnDeleteZero(wxCommandEvent &event);
  void OnDeleteAllPoles(wxCommandEvent &event);
  void OnDeleteAllZeros(wxCommandEvent &event);

  // **************************************************************************
  // Declare the event table right at the end
  // **************************************************************************

  DECLARE_EVENT_TABLE()
};


#endif
