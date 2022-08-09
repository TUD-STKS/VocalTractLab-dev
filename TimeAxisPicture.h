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

#ifndef __TIME_AXIS_PICTURE_H__
#define __TIME_AXIS_PICTURE_H__

#include "BasicPicture.h"
#include "Graph.h"

// ****************************************************************************
/// Draws the time axis on the gestural score page.
// ****************************************************************************

class TimeAxisPicture : public BasicPicture
{
  // **************************************************************************
  // Public functions.
  // **************************************************************************

public:
  TimeAxisPicture(wxWindow *parent);
  virtual void draw(wxDC &dc);

  // Returns the minimum height so that the axis with labels fits 
  int getMinHeight();
private:
  Graph *graph;
};

#endif

// ****************************************************************************
