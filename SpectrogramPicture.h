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

#ifndef __SPECTROGRAM_PICTURE_H__
#define __SPECTROGRAM_PICTURE_H__

#include "Data.h"
#include "BasicPicture.h"
#include "Graph.h"
#include "SpectrogramPlot.h"

// ****************************************************************************
/// This is the class for the main spectrogram picture on the signal page.
// ****************************************************************************

class SpectrogramPicture : public BasicPicture
{
  // **************************************************************************
  // Public data.
  // **************************************************************************

public:
  SpectrogramPlot *spectrogramPlot;

  // **************************************************************************
  // Public functions.
  // **************************************************************************

public:
  SpectrogramPicture(wxWindow *parent, wxWindow *updateEventReceiver);
  virtual void draw(wxDC &dc);
  void paintSpectrogram(wxDC &dc);

  // **************************************************************************
  // Private data.
  // **************************************************************************

private:
  Graph graph;
  Data *data;
  wxWindow *updateEventReceiver;

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
