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

#include "LfPulsePicture.h"
#include "VocalTractLabBackend/Signal.h"

// ****************************************************************************
// The event table.
// ****************************************************************************

BEGIN_EVENT_TABLE(LfPulsePicture, BasicPicture)
  EVT_LEFT_DOWN(LfPulsePicture::OnMouseClick)
END_EVENT_TABLE()


// ****************************************************************************
/// Constructor. Init the variables.
// ****************************************************************************

LfPulsePicture::LfPulsePicture(wxWindow *parent) : BasicPicture(parent)
{
  showDerivative = false;
  lfPulse = &Data::getInstance()->lfPulse;
}

// ****************************************************************************
/// Draws the picture.
// ****************************************************************************

void LfPulsePicture::draw(wxDC &dc)
{
  Signal s;
  double min, max;
  int i;

  // ****************************************************************
  // Fill the background.
  // ****************************************************************

  dc.SetBackground(*wxWHITE_BRUSH);
  dc.Clear();

  int width, height;
  this->GetSize(&width, &height);
  lfPulse->getPulse(s, width, showDerivative);
  s.getMinMax(min, max);

  // Paint the pulse ************************************************

  double factor = max - min;
  if (factor < 0.0001) { factor = 0.0001; }
  factor = (height-20) / factor;
  int y;
  int lastY;

  dc.SetPen(*wxBLACK_PEN);

  for (i=0; i < width; i++)
  {
    y = height - 10 - (int)((s.getValue(i) - min)*factor);
    if (i > 0) 
    { 
      dc.DrawLine(i-1, lastY, i, y); 
    }
    lastY = y;
  }
}

// ****************************************************************************
/// A mouse click toggels between the normal pulse and the time-derivative of
/// the pulse.
// ****************************************************************************

void LfPulsePicture::OnMouseClick(wxMouseEvent &event)
{
  showDerivative = !showDerivative;
  this->Refresh();
}

// ****************************************************************************
