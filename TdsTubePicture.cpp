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

#include "TdsTubePicture.h"
#include "Data.h"
#include <wx/tipwin.h>


const wxBrush TdsTubePicture::ARTICULATOR_BRUSH[Tube::NUM_ARTICULATORS] =
{
  wxBrush( wxColor(255, 197, 27) ),  // Vocal folds
  wxBrush( wxColor(255, 197, 27) ),  // Tongue
  wxBrush( wxColor(250, 250, 240) ), // Lower incisors
  wxBrush( wxColor(255, 197, 27) ),  // Lower lip
  wxBrush( wxColor(200, 200, 200) )  // other
};


// ****************************************************************************
// The event table.
// ****************************************************************************

BEGIN_EVENT_TABLE(TdsTubePicture, BasicPicture)
    EVT_MOUSE_EVENTS(TdsTubePicture::OnMouseEvent)
END_EVENT_TABLE()


// ****************************************************************************
/// Construcor. Passes the parent parameter.
// ****************************************************************************

TdsTubePicture::TdsTubePicture(wxWindow *parent, wxWindow *updateEventReceiver) : BasicPicture(parent)
{
  this->updateEventReceiver = updateEventReceiver;

  const int LEFT_MARGIN = 55;
  const int RIGHT_MARGIN = 0;
  const int BOTTOM_MARGIN = 25;
  const int TOP_MARGIN = 0;

  graph.init(this, LEFT_MARGIN, RIGHT_MARGIN, TOP_MARGIN, BOTTOM_MARGIN);

  graph.initAbscissa(PQ_LENGTH, 0.0, 1.0,
                     -24.0, -24.0, -24.0, 20.0, 20.0, 20.0,
                     1, 0, true, true, true);
  graph.initLinearOrdinate(PQ_AREA, 0.0, 1.0,
                           -20.0, -0.2, -8.0, 6.0, 30.0, 16.0,
                           10, 0, true, true, true);

  // ****************************************************************

  showArticulators = false;
}


// ****************************************************************************
// ****************************************************************************

void TdsTubePicture::draw(wxDC &dc)
{
  static Tube tube;

  Data *data = Data::getInstance();
  TdsModel *model = data->tdsModel;
  int graphX, graphY, graphW, graphH;
  
  int i, k;
  wxColor color;
  int rectX, rectY, rectW, rectH;
  Tube::Section *ts = NULL;
  int leftX, rightX;
  int y;
  int zeroY = graph.getYPos(0.0);
  double lowerLimit = 0.0;
  double upperLimit = 0.0;
  double leftValue;
  double rightValue;
  double meanValue;
  double A;
  int colorIndex = 0;
  bool isSideBranch;
  bool paintSection = true;

  // ****************************************************************
  // Get the current tube from the TDS model.
  // ****************************************************************

  model->getTube(&tube);

  // ****************************************************************
  // Fill the background.
  // ****************************************************************

  dc.SetBackground(*wxWHITE_BRUSH);
  dc.Clear();

  // ****************************************************************
  // Paint the scales.
  // ****************************************************************

  graph.paintAbscissa(dc);
  graph.paintOrdinate(dc);
  graph.getDimensions(graphX, graphY, graphW, graphH);

  // ****************************************************************
  // Determine the range limits.
  // ****************************************************************

  if (data->quantity == Data::QUANTITY_PRESSURE)
  {
    if (data->tdsPressureTimeGraph != NULL)
    {
      lowerLimit = data->tdsPressureTimeGraph->linearOrdinate.negativeLimit;
      upperLimit = data->tdsPressureTimeGraph->linearOrdinate.positiveLimit;
    }
    else
    {
      // dPa
      lowerLimit = -20000.0;
      upperLimit =  20000.0;
    }
  }
  else
  if (data->quantity == Data::QUANTITY_FLOW)
  {
    if (data->tdsFlowTimeGraph != NULL)
    {
      lowerLimit = data->tdsFlowTimeGraph->linearOrdinate.negativeLimit;
      upperLimit = data->tdsFlowTimeGraph->linearOrdinate.positiveLimit;
    }
    else
    {
      // cm^3/s
      lowerLimit = -500.0;
      upperLimit = 500.0;
    }
  }
  else
  if (data->quantity == Data::QUANTITY_AREA)
  {
    if (data->tdsAreaTimeGraph != NULL)
    {
      lowerLimit = data->tdsAreaTimeGraph->linearOrdinate.negativeLimit;
      upperLimit = data->tdsAreaTimeGraph->linearOrdinate.positiveLimit;
    }
    else
    {
      // cm^2
      lowerLimit = 0.0;
      upperLimit = 20.0;
    }
  }
  else
  {
    if (data->tdsVelocityTimeGraph != NULL)
    {
      lowerLimit = data->tdsVelocityTimeGraph->linearOrdinate.negativeLimit;
      upperLimit = data->tdsVelocityTimeGraph->linearOrdinate.positiveLimit;
    }
    else
    {
      // cm
      lowerLimit = -4000.0;
      upperLimit = 4000.0;
    }
  }


  // ****************************************************************
  // Fill the background of the tube sections with the color of the
  // corresponding articulator.
  // ****************************************************************

  if (showArticulators)
  {
    for (i=Tube::LOWER_GLOTTIS_SECTION; i <= Tube::LAST_MOUTH_SECTION; i++)
    {
      ts = tube.section[i];
      leftX = graph.getXPos(ts->pos_cm);
      rightX = graph.getXPos(ts->pos_cm + ts->length_cm);

      dc.SetPen(*wxTRANSPARENT_PEN);
      dc.SetBrush( ARTICULATOR_BRUSH[ts->articulator] );
      dc.DrawRectangle(leftX, graphY, rightX - leftX + 1, zeroY - graphY + 1);
    }
  }

  // ****************************************************************
  // Run through all tube sections.
  // ****************************************************************

  for (i=0; i < Tube::NUM_SECTIONS; i++)
  {
    // Is it a side branch of the vocal tract? **********************
    
    if ((i >= Tube::FIRST_TRACHEA_SECTION) && 
        (i <  Tube::FIRST_PHARYNX_SECTION + Tube::NUM_PHARYNX_MOUTH_SECTIONS))
    { 
      isSideBranch = false; 
    }
    else
    { 
      isSideBranch = true; 
    }
    
    // Will the current tube section be drawn ? *********************

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

    // **************************************************************

    if (paintSection)
    {
      ts = tube.section[i];

      // The nasal sinuses are drawn as circles *********************

      if ((i >= Tube::FIRST_SINUS_SECTION) && 
          (i <  Tube::FIRST_SINUS_SECTION + Tube::NUM_SINUS_SECTIONS))
      {
        // Determine the color for the current section
        data->getTubeSectionQuantity(model, i, leftValue, rightValue);
        meanValue = 0.5*(leftValue + rightValue);
        colorIndex = (int)(((double)Data::NUM_TDS_SCALE_COLORS*(meanValue - lowerLimit)) / (upperLimit - lowerLimit));
        if (colorIndex < 0) 
        { 
          colorIndex = 0; 
        }
        if (colorIndex >= Data::NUM_TDS_SCALE_COLORS) 
        { 
          colorIndex = Data::NUM_TDS_SCALE_COLORS-1; 
        }
        color = data->tdsScaleColor[colorIndex];

        // Determine circle center and draw the circle

        int xm, ym, r;
        int sinusIndex = i - Tube::FIRST_SINUS_SECTION;
        int sourceSection = Tube::SINUS_COUPLING_SECTION[sinusIndex];
        A = -tube.noseSection[sourceSection].area_cm2;
        y = graph.getYPos(A);

        r = graph.getXPos(0.6) - graph.getXPos(0.0);    // A radius corresponding to 0.6 cm

        ym = y+r+1;
        xm = graph.getXPos(ts->pos_cm);

        rectX = xm-r;
        rectY = ym-r;
        rectW = 2*r;
        rectH = 2*r;

        dc.SetBrush(color);
        dc.SetPen(*wxBLACK_PEN);
        dc.DrawEllipse(rectX, rectY, rectW, rectH);
      }
      else

      // Normal tube section ******************************************
      {
        leftX = graph.getXPos(ts->pos_cm);
        rightX = graph.getXPos(ts->pos_cm + ts->length_cm);

        // Flip the area function of the side branches upside down
        A = ts->area_cm2;
        if (isSideBranch) { A = -A; }
        y = graph.getYPos(A);

        // Determine the fill color for the section
        data->getTubeSectionQuantity(model, i, leftValue, rightValue);
        meanValue = 0.5*(leftValue + rightValue);
        colorIndex = (int)(((double)Data::NUM_TDS_SCALE_COLORS*(meanValue - lowerLimit)) / (upperLimit - lowerLimit));
        if (colorIndex < 0) 
        { 
          colorIndex = 0; 
        }
        if (colorIndex >= Data::NUM_TDS_SCALE_COLORS) 
        { 
          colorIndex = Data::NUM_TDS_SCALE_COLORS-1; 
        }
        color = data->tdsScaleColor[colorIndex];

        // Paint the tube section ***************************************

        if (A < 0.0)
        {
          rectX = leftX;
          rectY = zeroY;
          rectW = rightX - leftX + 1;
          rectH = y - zeroY + 1;
        }
        else
        {
          rectX = leftX;
          rectY = y;
          rectW = rightX - leftX + 1;
          rectH = zeroY - y + 1;
        }

        dc.SetBrush(color);
        dc.SetPen(*wxBLACK_PEN);
        dc.DrawRectangle(rectX, rectY, rectW, rectH); 

        // **********************************************************
        // Indicate where dipole sources are active.
        // **********************************************************

        if (model->tubeSection[i].dipoleSource.targetAmp1kHz > 0.0)
        {
          int centerX = leftX;
          int centerY = 8;
          int radius = 7;

          dc.SetBrush( wxColor(255, 100, 100) );
          dc.SetPen(*wxBLACK_PEN);
          dc.DrawCircle(centerX, centerY, radius);
          dc.DrawLine(centerX - radius - 5, centerY, centerX - radius, centerY);
          dc.DrawLine(centerX + radius, centerY, centerX + radius + 5, centerY);

          dc.SetPen(wxPen(*wxBLACK, 1, wxPENSTYLE_DOT));
          dc.DrawLine(centerX, centerY + radius, centerX, zeroY);
        }

      }
    }

  }


  // ****************************************************************
  // Indicate if there is the most anterior lip dipole source active.
  // ****************************************************************

  if (model->lipsDipoleSource.targetAmp1kHz > 0.0)
  {
    ts = tube.section[Tube::LAST_MOUTH_SECTION];

    int centerX = graph.getXPos(ts->pos_cm + ts->length_cm);
    int centerY = 8;
    int radius = 7;

    dc.SetBrush( wxColor(255, 100, 100) );
    dc.SetPen(*wxBLACK_PEN);
    dc.DrawCircle(centerX, centerY, radius);
    dc.DrawLine(centerX - radius - 5, centerY, centerX - radius, centerY);
    dc.DrawLine(centerX + radius, centerY, centerX + radius + 5, centerY);

    dc.SetPen(wxPen(*wxBLACK, 1, wxPENSTYLE_DOT));
    dc.DrawLine(centerX, centerY + radius, centerX, zeroY);
  }



  // *********************************************************************
  // Paint a line under constriction sections.
  // *********************************************************************

  TdsModel::Constriction *cons = NULL;
  
  for (k=0; k < model->numConstrictions; k++)
  {
    cons = &model->constriction[k];
    leftX = graph.getXPos(tube.section[cons->firstSection]->pos_cm);
    rightX = graph.getXPos(tube.section[cons->lastSection]->pos_cm + tube.section[cons->lastSection]->length_cm);
    y = graph.getYPos(0.0);

    dc.SetPen( wxColor(255, 100, 100) );    // red pen

    dc.DrawLine(leftX, y + 1, rightX, y + 1);
    dc.DrawLine(leftX, y + 2, rightX, y + 2);
    dc.DrawLine(leftX, y + 3, rightX, y + 3);
    dc.DrawLine(leftX, y + 4, rightX, y + 4);
    dc.DrawLine(leftX, y + 5, rightX, y + 5);
    dc.SetPen( *wxBLACK );    // black pen
    dc.DrawLine(leftX, y + 6, rightX, y + 6);
    
    // Left and right black borders
    dc.DrawLine(leftX, y + 1, leftX, y + 6);
    dc.DrawLine(rightX, y + 1, rightX, y + 6);
  }

  // *********************************************************************
  // Paint the incisors as a small white triangle.
  // *********************************************************************

  int teethX = graph.getXPos(tube.teethPosition_cm);
  int teethY = graph.getYPos(0.0);

  wxPoint triangle[3];
  triangle[0].x = teethX-8;
  triangle[0].y = teethY+8;

  triangle[1].x = teethX;
  triangle[1].y = teethY;

  triangle[2].x = teethX+8;
  triangle[2].y = teethY+8;

  dc.SetBrush( wxColor(255, 255, 128) );
  dc.SetPen(*wxBLACK_PEN);
  dc.DrawPolygon(3, triangle);

  // *********************************************************************
  // Paint a dashed line through the selected tube section.
  // *********************************************************************

  i = data->userProbeSection;
  if ((i >= Tube::FIRST_TRACHEA_SECTION) && (i < Tube::NUM_SECTIONS))
  {
    ts = tube.section[i];

    leftX = graph.getXPos(ts->pos_cm + 0.5*ts->length_cm);

    if ((leftX >= graphX) && (leftX < graphX + graphW))
    {
      if ((i >= Tube::FIRST_TRACHEA_SECTION) && 
          (i <  Tube::FIRST_PHARYNX_SECTION + Tube::NUM_PHARYNX_MOUTH_SECTIONS))
      { 
        y = graphY;
      }
      else
      { 
        y = graphY + graphH - 1;
      }

      dc.SetPen(wxPen(*wxBLACK, 1, wxPENSTYLE_DOT));
      dc.DrawLine(leftX, y, leftX, zeroY);
    }
  }

  // ****************************************************************
  // Output the selected tube section index.
  // ****************************************************************

  wxString st = wxString::Format("Selected section: %i", data->userProbeSection);
  dc.SetFont(wxFont(9, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

  dc.DrawText(st, graphX+5, 0);

}


// ****************************************************************************
/// Process all kinds of mouse events.
// ****************************************************************************

void TdsTubePicture::OnMouseEvent(wxMouseEvent &event)
{
  static Tube tube;

  Data *data = Data::getInstance();
  Tube::Section *ts = NULL;
  TdsModel *model = data->tdsModel;

  int graphX, graphY, graphW, graphH;
  graph.getDimensions(graphX, graphY, graphW, graphH);

  int mx = event.m_x;
  int my = event.m_y;

  if ((mx >= graphX) && (mx < graphX + graphW) && (my >= graphY) && (my < graphY + graphH))
  {

    // **************************************************************
    // Left mouse button was just pressed or dragging with the LMB.
    // **************************************************************

    if ((event.LeftDown()) || ((event.Dragging()) && (event.LeftIsDown())))
    {
      // Get the current tube from the TDS model.
      model->getTube(&tube);

      int i;
      int leftX, rightX;
      int newSelection = -1;
      bool isSideBranch = false;
      if (my > graph.getYPos(0.0)) { isSideBranch = true; }

      // Run through all tube sections

      for (i=0; i < Tube::NUM_SECTIONS; i++)
      {
        if ((i < Tube::FIRST_SINUS_SECTION) || (i >= Tube::FIRST_SINUS_SECTION + Tube::NUM_SINUS_SECTIONS))
        {
          ts = tube.section[i];
          leftX = graph.getXPos(ts->pos_cm);
          rightX = graph.getXPos(ts->pos_cm + ts->length_cm);

          if ((mx >= leftX) && (mx < rightX))
          {
            if (isSideBranch == (i >= Tube::FIRST_PHARYNX_SECTION + Tube::NUM_PHARYNX_MOUTH_SECTIONS))
            {
              newSelection = i;
            }
          }
        }
      }

      // Monitor the constriction data around this section.
      model->constrictionMonitorTubeSection = newSelection;

      // Has a new section been selected ? 

      if (newSelection != data->userProbeSection)
      {
        data->userProbeSection = newSelection;

        for (int i = 0; i < Data::TDS_BUFFER_LENGTH; i++)
        {
          data->userProbeFlow[i] = 0.0;
          data->userProbePressure[i] = 0.0;
          data->userProbeArea[i] = 0.0;
          data->userProbeVelocity[i] = 0.0;
        }
        
        // Redraw all pictures on the same notebook page including this
        wxCommandEvent myEvent(updateRequestEvent);
        myEvent.SetInt(REFRESH_PICTURES);
        wxPostEvent(updateEventReceiver, myEvent);
      }

    }

    // **************************************************************
    // Right-click.
    // **************************************************************

    if (event.RightDown())
    {
      int i;
      int k = -1;

      // Is the mouse over a vocal tract constriction ?

      TdsModel::Constriction *cons = NULL;
  
      for (i=0; i < model->numConstrictions; i++)
      {
        const int MARGIN = 4;

        cons = &model->constriction[i];
        int leftX = graph.getXPos(tube.section[cons->firstSection]->pos_cm);
        int rightX = graph.getXPos(tube.section[cons->lastSection]->pos_cm + tube.section[cons->lastSection]->length_cm);

        if ((mx >= leftX - MARGIN) && (mx <= rightX + MARGIN))
        {
          k = i;
        }
      }

      if (k != -1)
      {
        cons = &model->constriction[k];

        wxString st = wxString::Format(
          "Area: %2.2f cm^2\nVelocity: %2.0f cm/s\nf_c: %2.0f Hz",
          cons->area,
          cons->velocity,
          cons->cutoffFreq);

        wxTipWindow *tipWindow = NULL;
        tipWindow = new wxTipWindow(this, st);
      }
    }

  }
}

// ****************************************************************************
