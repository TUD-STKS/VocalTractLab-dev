// ****************************************************************************
// This file is part of VocalTractLab3D.
// Copyright (C) 2022, Peter Birkholz, Dresden, Germany
// www.vocaltractlab.de
// author: Peter Birkholz and R�mi Blandin
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

#include "TimeAxisPicture.h"
#include "Data.h"


// ****************************************************************************
/// Construcor. Passes the parent parameter.
// ****************************************************************************

TimeAxisPicture::TimeAxisPicture(wxWindow *parent) : BasicPicture(parent)
{
  // ****************************************************************
  // Init the time axis graph.
  // ****************************************************************

  graph = Data::getInstance()->gsTimeAxisGraph;
  const int labelHeight = graph->getFont().GetPixelSize().GetHeight();
  graph->init(this, Data::getInstance()->LEFT_SCORE_MARGIN, 0, 2 * labelHeight , 0.5*labelHeight);
  graph->abscissaAtBottom = false;
  graph->initAbscissa(PQ_TIME, 0.0, 0.001,
    0.0, 0.0, 0.0, 0.1, 10.0, 1.2,
    16, 2, false, false, false);
}


// ****************************************************************************
/// Draws the picture.
// ****************************************************************************

void TimeAxisPicture::draw(wxDC &dc)
{
  // Re-init graph because the margin may have changed
  const int labelHeight = graph->getFont().GetPixelSize().GetHeight();
  graph->init(this, Data::getInstance()->LEFT_SCORE_MARGIN, 0, 2 * labelHeight, 0.5 * labelHeight);
	
  // ****************************************************************
  // Fill the background and draw the graph.
  // ****************************************************************

  dc.SetBackground(*wxWHITE_BRUSH);
  dc.Clear();
  graph->paintAbscissa(dc);
}

int TimeAxisPicture::getMinHeight()
{
	int _, top, bottom;
	graph->getMargins(_, _, top, bottom);
	return top + bottom;
}

// ****************************************************************************
