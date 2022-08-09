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

#include "TdsSpatialSignalPicture.h"
#include "Data.h"


// ****************************************************************************
/// Construcor. Passes the parent parameter.
// ****************************************************************************

TdsSpatialSignalPicture::TdsSpatialSignalPicture(wxWindow *parent) : BasicPicture(parent)
{
  const int LEFT_MARGIN = this->FromDIP(55);
  const int RIGHT_MARGIN = 0;
  const int BOTTOM_MARGIN = this->FromDIP(25);
  const int TOP_MARGIN = 0;

  Graph *graph = NULL;

  // ****************************************************************
  // Pressure-place graph
  // ****************************************************************
  
  graph = &pressureGraph;
  graph->init(this, LEFT_MARGIN, RIGHT_MARGIN, TOP_MARGIN, BOTTOM_MARGIN);
  
  graph->initAbscissa(PQ_LENGTH, 0.0, 1.0,
    -24.0, -24.0, -24.0, 20.0, 20.0, 20.0,
    1, 0, true, true, true);

  graph->initLinearOrdinate(PQ_PRESSURE, 0.0, 1000.0,
                            -30000, -5000, -10000, 5000, 30000, 10000,
                            10, 0, true, true, true);

  // ****************************************************************
  // Flow-place graph
  // ****************************************************************
  
  graph = &flowGraph;
  graph->init(this, LEFT_MARGIN, RIGHT_MARGIN, TOP_MARGIN, BOTTOM_MARGIN);

  graph->initAbscissa(PQ_LENGTH, 0.0, 1.0,
    -24.0, -24.0, -24.0, 20.0, 20.0, 20.0,
    1, 0, true, true, true);

  graph->initLinearOrdinate(PQ_VOLUME_VELOCITY, 0.0, 10.0,
                            -5000.0, -100.0, -500.0, 100.0, 5000.0, 500.0,
                            10, 0, true, true, true);

  // ****************************************************************
  // Area-place graph
  // ****************************************************************
  
  graph = &areaGraph;
  graph->init(this, LEFT_MARGIN, RIGHT_MARGIN, TOP_MARGIN, BOTTOM_MARGIN);

  graph->initAbscissa(PQ_LENGTH, 0.0, 1.0,
    -24.0, -24.0, -24.0, 20.0, 20.0, 20.0,
    1, 0, true, true, true);

  graph->initLinearOrdinate(PQ_AREA, 0.0, 0.01,
                            0.0, 0.0, 0.0, 1.0, 20.0, 15.0,
                            10, 1, true, true, true);

  // ****************************************************************
  // Velocity-place graph
  // ****************************************************************

  graph = &velocityGraph;
  graph->init(this, LEFT_MARGIN, RIGHT_MARGIN, TOP_MARGIN, BOTTOM_MARGIN);

  graph->initAbscissa(PQ_LENGTH, 0.0, 1.0,
    -24.0, -24.0, -24.0, 20.0, 20.0, 20.0,
    1, 0, true, true, true);

  graph->initLinearOrdinate(PQ_VELOCITY, 0.0, 1.0,
    -5000.0, -100.0, -4000.0, 100.0, 5000.0, 4000.0,
    10, 0, true, true, true);
}


// ****************************************************************************
// ****************************************************************************

void TdsSpatialSignalPicture::draw(wxDC &dc)
{
  const int MAX_TUBE_SECTIONS = 1024;

  Data *data = Data::getInstance();
  int i;
  int graphX, graphY, graphW, graphH;

  TdsModel *model = data->tdsModel;
  TdsModel::TubeSection *ts = NULL;
  double lowerLimit = 0.0;
  double upperLimit = 0.0;
  int leftY[MAX_TUBE_SECTIONS];
  int rightY[MAX_TUBE_SECTIONS];
  int leftX;
  int rightX;
  double leftValue;
  double rightValue;
  bool isSideBranch;
  bool paintSection;
  wxColor color;

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
  // Paint the curve.
  // ****************************************************************

  // Get the upper and lower limits.

  lowerLimit = graph->linearOrdinate.reference + graph->linearOrdinate.negativeLimit;
  upperLimit = graph->linearOrdinate.reference + graph->linearOrdinate.positiveLimit;

  // Run through all tube sections **********************************

  for (i=0; i < Tube::NUM_SECTIONS; i++)
  {
    // Is it a side branch of the vocal tract ?
    
    if ((i >= Tube::FIRST_TRACHEA_SECTION) && 
        (i < Tube::FIRST_PHARYNX_SECTION + Tube::NUM_PHARYNX_MOUTH_SECTIONS))
    { 
      isSideBranch = false; 
    }
    else
    { 
      isSideBranch = true; 
    }
    
    // Will the current tube section be painted ?

    paintSection = true;

    if ((i < Tube::FIRST_PHARYNX_SECTION) && (!data->showSubglottalSystem)) 
    { 
      paintSection = false; 
    }
    if ((!data->showMainPath) && (!isSideBranch)) 
    { 
      paintSection = false; 
    }
    if ((!data->showSidePath) && (isSideBranch)) 
    { 
      paintSection = false; 
    }
    if ((i >= Tube::FIRST_SINUS_SECTION) && 
        (i <  Tube::FIRST_SINUS_SECTION + Tube::NUM_SINUS_SECTIONS)) 
    { 
      paintSection = false; 
    }

    if (paintSection)
    {
      ts = &model->tubeSection[i];

      leftX = graph->getXPos(ts->pos);
      rightX = graph->getXPos(ts->pos + ts->length);

      data->getTubeSectionQuantity(model, i, leftValue, rightValue);
      leftY[i] = graph->getYPos(leftValue);
      rightY[i] = graph->getYPos(rightValue);

      if (isSideBranch) 
      {
        dc.SetPen(wxPen(*wxRED, lineWidth));
      } 
      else 
      { 
        dc.SetPen(wxPen(*wxBLACK, lineWidth));
      }

      dc.DrawLine(leftX, leftY[i], rightX, rightY[i]);

      if ((isSideBranch == false) && (i > 0))
      {
        dc.DrawLine(leftX, rightY[i-1], leftX, leftY[i]);
      }

      if ((isSideBranch) && (i != Tube::FIRST_NOSE_SECTION) && (i != Tube::FIRST_FOSSA_SECTION))
      {
        dc.DrawLine(leftX, rightY[i - 1], leftX, leftY[i]);
      }
    }
  }

  // *********************************************************************
  // Paint a dashed line through the selected tube section.
  // *********************************************************************

  i = data->userProbeSection;
  if ((i >= Tube::FIRST_TRACHEA_SECTION) && (i < Tube::NUM_SECTIONS))
  {
    ts = &model->tubeSection[i];

    leftX = graph->getXPos(ts->pos + 0.5*ts->length);
    if ((leftX >= graphX) && (leftX < graphX + graphW))
    {
      dc.SetPen(wxPen(*wxBLACK, lineWidth, wxPENSTYLE_LONG_DASH));
      dc.DrawLine(leftX, graphY, leftX, graphY + graphH - 1);
    }
  }

}

// ****************************************************************************
