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

#include "TdsTimeSignalPicture.h"
#include "Data.h"


// ****************************************************************************
/// Construcor. Passes the parent parameter.
// ****************************************************************************

TdsTimeSignalPicture::TdsTimeSignalPicture(wxWindow *parent) : BasicPicture(parent)
{
  // ****************************************************************
  // Init. the scales.
  // ****************************************************************

  const int LEFT_MARGIN = this->FromDIP(80);
  const int RIGHT_MARGIN = 0;
  const int BOTTOM_MARGIN = this->FromDIP(25);
  const int TOP_MARGIN = 0;

  Graph *graph = NULL;

  // ****************************************************************
  // Pressure-time graph
  // ****************************************************************

  graph = &pressureGraph;
  graph->init(this, LEFT_MARGIN, RIGHT_MARGIN, TOP_MARGIN, BOTTOM_MARGIN);
  graph->initAbscissa(PQ_TIME, 0.0, 0.01,
                      -0.05, -0.05, -0.05, 0.0, 0.0, 0.0,
                      1, 2, false, true, true);
  
  graph->initLinearOrdinate(PQ_PRESSURE, 0.0, 1000.0,
                            -30000, -10000, -20000, 10000, 30000, 20000,
                            10, 0, true, true, true);

  // ****************************************************************
  // Flow-time graph
  // ****************************************************************

  graph = &flowGraph;
  graph->init(this, LEFT_MARGIN, RIGHT_MARGIN, TOP_MARGIN, BOTTOM_MARGIN);
  graph->initAbscissa(PQ_TIME, 0.0, 0.01,
                      -0.05, -0.05, -0.05, 0.0, 0.0, 0.0,
                      1, 2, false, true, true);
  
  graph->initLinearOrdinate(PQ_VOLUME_VELOCITY, 0.0, 10.0,
                            -1000.0, -100.0, -500.0, 100.0, 2000.0, 500.0,
                            10, 0, true, true, true);

  // ****************************************************************
  // Area-time graph
  // ****************************************************************

  graph = &areaGraph;
  graph->init(this, LEFT_MARGIN, RIGHT_MARGIN, TOP_MARGIN, BOTTOM_MARGIN);
  graph->initAbscissa(PQ_TIME, 0.0, 0.01,
                      -0.05, -0.05, -0.05, 0.0, 0.0, 0.0,
                      1, 2, false, true, true);
  
  graph->initLinearOrdinate(PQ_AREA, 0.0, 0.01,
                            0.0, 0.0, 0.0, 1.0, 20.0, 15.0,
                            10, 1, true, true, true);

  // ****************************************************************
  // Velocity-time graph
  // ****************************************************************

  graph = &velocityGraph;
  graph->init(this, LEFT_MARGIN, RIGHT_MARGIN, TOP_MARGIN, BOTTOM_MARGIN);
  graph->initAbscissa(PQ_TIME, 0.0, 0.01,
    -0.05, -0.05, -0.05, 0.0, 0.0, 0.0,
    1, 2, false, true, true);

  graph->initLinearOrdinate(PQ_VELOCITY, 0.0, 1.0,
    -5000.0, -100.0, -4000.0, 100.0, 5000.0, 4000.0,
    10, 0, true, true, true);


  // Set two "global" pointers to the graph objects, so that the graph
  // data can be used by the TDS area function picture!

  Data *data = Data::getInstance();

  data->tdsPressureTimeGraph = &pressureGraph;
  data->tdsFlowTimeGraph = &flowGraph;
  data->tdsAreaTimeGraph = &areaGraph;
  data->tdsVelocityTimeGraph = &velocityGraph;
}


// ****************************************************************************
// Draw the picture.
// ****************************************************************************

void TdsTimeSignalPicture::draw(wxDC &dc)
{
  Data *data = Data::getInstance();
  int graphX, graphY, graphW, graphH;
  int i;

  // ****************************************************************
  // Fill the background.
  // ****************************************************************

  dc.SetBackground(*wxWHITE_BRUSH);
  dc.Clear();

  // ****************************************************************
  // Paint the scales.
  // ****************************************************************

  Graph *graph = NULL;

  if (data->quantity == Data::QUANTITY_FLOW)
  {
    graph = &flowGraph;
  }
  else
  if (data->quantity == Data::QUANTITY_PRESSURE)
  {
    graph = &pressureGraph;
  }
  else
  if (data->quantity == Data::QUANTITY_AREA)
  {
    graph = &areaGraph;
  }
  else
  {
    graph = &velocityGraph;
  }

  graph->paintAbscissa(dc);
  graph->paintOrdinate(dc);
  graph->getDimensions(graphX, graphY, graphW, graphH);

  // ****************************************************************
  // Paint the color scale.
  // ****************************************************************

  int index;
  wxColor c;

  for (i=0; i < graphH; i++)
  {
    index = Data::NUM_TDS_SCALE_COLORS - 1 - ((Data::NUM_TDS_SCALE_COLORS*i) / graphH);
    c = data->tdsScaleColor[index];
    dc.SetPen( wxPen(c, lineWidth) );
    dc.DrawLine(0, graphY+i, 15, graphY+i);
  }

  // ****************************************************************
  // Paint the function.
  // ****************************************************************

  double t_s;
  int pos;
  double *buffer;
  TubeSequence *sequence = data->getSelectedTubeSequence();
  int posOffset = sequence->getPos_pt();
  int y, lastY;
  double d;

  if (data->quantity == Data::QUANTITY_PRESSURE)
  {
    buffer = data->userProbePressure;
  }
  else
  if (data->quantity == Data::QUANTITY_FLOW)
  {
    buffer = data->userProbeFlow;
  }
  else
  if (data->quantity == Data::QUANTITY_AREA)
  {
    buffer = data->userProbeArea;
  }
  else
  {
    buffer = data->userProbeVelocity;
  }
  
  dc.SetPen(wxPen(*wxBLACK, lineWidth));

  for (i=0; i < graphW; i++)
  {
    t_s = graph->getAbsXValue(graphX + i);        // Time instant on the time axis
    pos = (int)(t_s*(double)SAMPLING_RATE);   // relative sampling position
    pos+= posOffset;
    pos&= Data::TDS_BUFFER_MASK;                  // modulo buffer length

    d = buffer[pos];
    y = graph->getYPos(d);

    if (i > 0)
    {
      dc.DrawLine(graphX+i-1, lastY, graphX+i, y);
    }
    
    lastY = y;
  }

}

// ****************************************************************************
