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

#ifndef __AREA_FUNCTION_PICTURE_H__
#define __AREA_FUNCTION_PICTURE_H__

#include "BasicPicture.h"
#include "VocalTractPicture.h"
#include "Graph.h"
#include "VocalTractLabBackend/Tube.h"

// ****************************************************************************
// ****************************************************************************

class AreaFunctionPicture : public BasicPicture
{
  // **************************************************************************
  // Public data.
  // **************************************************************************

public:
  static const wxBrush ARTICULATOR_BRUSH[Tube::NUM_ARTICULATORS];

  // Options for the rendering of the area function
  bool discreteAreaFunction;
  bool showAreas;
  bool showSideBranches;
  bool showText;
  bool showArticulators;

  // **************************************************************************
  // Public functions.
  // **************************************************************************

public:
  AreaFunctionPicture(wxWindow *parent, VocalTractPicture *picVocalTract, wxWindow *updateEventReceiver);
  virtual void draw(wxDC &dc);

  // **************************************************************************
  // Private data.
  // **************************************************************************

private:
  VocalTractPicture *picVocalTract;
  wxWindow *updateEventReceiver;
  Graph circGraph;
  Graph areaGraph;
  bool moveCutPos;
  int lastMx;
  int lastMy;

  // **************************************************************************
  // Private functions.
  // **************************************************************************

private:
  void OnMouseEvent(wxMouseEvent &event);

  // ****************************************************************************
  // Declare the event table right at the end
  // ****************************************************************************

  DECLARE_EVENT_TABLE()
};

#endif

// ****************************************************************************
