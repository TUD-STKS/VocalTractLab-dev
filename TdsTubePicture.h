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

#ifndef __TDS_AREA_FUNCTION_PICTURE_H__
#define __TDS_AREA_FUNCTION_PICTURE_H__

#include "BasicPicture.h"
#include "Graph.h"
#include "Backend/Tube.h"

// ****************************************************************************
// ****************************************************************************

class TdsTubePicture : public BasicPicture
{
  // **************************************************************************
  // Public data.
  // **************************************************************************

public:
  static const wxBrush ARTICULATOR_BRUSH[Tube::NUM_ARTICULATORS];

  Graph graph;
  bool showArticulators;

  // **************************************************************************
  // Public functions.
  // **************************************************************************

public:
  TdsTubePicture(wxWindow *parent, wxWindow *updateEventReceiver);
  virtual void draw(wxDC &dc);

  // **************************************************************************
  // Private data.
  // **************************************************************************

private:
  wxWindow *updateEventReceiver;
  int lineWidth{ this->FromDIP(1) };

  // **************************************************************************
  // Event handlers.
  // **************************************************************************

private:
  void OnMouseEvent(wxMouseEvent &event);

  // **************************************************************************
  // Declare the event table right at the end
  // **************************************************************************

  DECLARE_EVENT_TABLE()
};

#endif

// ****************************************************************************
