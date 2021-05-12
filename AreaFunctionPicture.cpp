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

#include "AreaFunctionPicture.h"
#include "VocalTractDialog.h"
#include "Data.h"

#include <sstream>


const wxBrush AreaFunctionPicture::ARTICULATOR_BRUSH[Tube::NUM_ARTICULATORS] =
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

BEGIN_EVENT_TABLE(AreaFunctionPicture, BasicPicture)
  EVT_MOUSE_EVENTS(AreaFunctionPicture::OnMouseEvent)
END_EVENT_TABLE()


// ****************************************************************************
/// Construcor. Passes the parent parameter.
// ****************************************************************************

AreaFunctionPicture::AreaFunctionPicture(wxWindow *parent, VocalTractPicture *picVocalTract, 
                                         wxWindow *updateEventReceiver) : BasicPicture(parent)
{
  this->picVocalTract = picVocalTract;
  this->updateEventReceiver = updateEventReceiver;

  // ****************************************************************
  // Create the graphs for the area and circumference function.
  // ****************************************************************

  // Graph for the circumference function 

  circGraph.init(this, this->FromDIP(35), this->FromDIP(5), 0, this->FromDIP(25));
  circGraph.initAbscissa(PQ_LENGTH, 0.0, 1.0, 
    0.0, 0.0, 0.0, 
    20.0, 20.0, 20.0,
    1, 0, true, false, true);

  circGraph.initLinearOrdinate(PQ_LENGTH, 0.0, 0.01,
    0.0, 0.0, 0.0,
    10.0, 20.0, 20.0,
    4, 0, true, false, true);
  
  circGraph.isLinearOrdinate = true;

  // Graph for the area function

  areaGraph.init(this, this->FromDIP(40), this->FromDIP(5), 0, this->FromDIP(25));
  areaGraph.initAbscissa(PQ_LENGTH, 0.0, 1.0, 
    0.0, 0.0, 0.0, 
    20.0, 20.0, 20.0,
    1, 0, true, false, true);

  areaGraph.initLinearOrdinate(PQ_AREA, 0.0, 1.0,
    0.0, 0.0, 0.0,
    10.0, 20.0, 14.0,
    4, 0, true, false, true);
  
  areaGraph.isLinearOrdinate = true;

  // ****************************************************************
  // Options for the rendering of the area function
  // ****************************************************************

  discreteAreaFunction = false;
  showAreas = true;
  showSideBranches = true;
  showText = true;
  showArticulators = false;

  moveCutPos = false;
  lastMx = 0;
  lastMy = 0;
}


// ****************************************************************************
// ****************************************************************************

void AreaFunctionPicture::draw(wxDC &dc)
{
  int i;
  VocalTract *tract = Data::getInstance()->vocalTract;
  TlModel *model = Data::getInstance()->tlModel;
  Tube::Section *ts = NULL;
  Graph *graph = NULL;
  int lastY;
  double currentArea = 0.0;
  double cutPos;
  int x[2];
  int y;

  int graphX, graphY, graphW, graphH;

  // ****************************************************************
  // Fill the background.
  // ****************************************************************

  dc.SetBackground(*wxWHITE_BRUSH);
  dc.Clear();

  // ****************************************************************
  // Find the beginning and ending location of the area function.
  // ****************************************************************

  double negativeLimit = 0.0;
  double positiveLimit = 0.0;

  ts = model->tube.section[Tube::LAST_MOUTH_SECTION];
  positiveLimit = ts->pos_cm + ts->length_cm;

  ts = model->tube.section[Tube::LAST_NOSE_SECTION];
  if ((ts->pos_cm + ts->length_cm > positiveLimit) && (showSideBranches))
  { 
    positiveLimit = ts->pos_cm + ts->length_cm;
  }

  // ****************************************************************
  // Paint the graph axes.
  // ****************************************************************

  if (showAreas) { graph = &areaGraph; } else { graph = &circGraph; }

  graph->getDimensions(graphX, graphY, graphW, graphH);

  graph->abscissa.negativeLimit = negativeLimit;
  graph->abscissa.positiveLimit = positiveLimit;

  graph->paintAbscissa(dc);
  graph->paintOrdinate(dc);

  // ****************************************************************
  // Fill the background of the tube sections with the color of the
  // corresponding articulator.
  // ****************************************************************

  if ((showAreas) && (showArticulators))    //(discreteAreaFunction) && 
  {
    for (i=0; i < Tube::NUM_PHARYNX_MOUTH_SECTIONS; i++)
    {
      ts = &model->tube.pharynxMouthSection[i];
      x[0] = graph->getXPos(ts->pos_cm);
      x[1] = graph->getXPos(ts->pos_cm + ts->length_cm);

      dc.SetPen(*wxTRANSPARENT_PEN);
      dc.SetBrush( ARTICULATOR_BRUSH[ts->articulator] );
      dc.DrawRectangle(x[0], graphY, x[1]-x[0]+1, graphH);
    }
  }


  // ****************************************************************
  // Display the nose cavity and the piriform fossa ?
  // ****************************************************************

  if (showSideBranches)
  {
    for (i=0; i < Tube::NUM_FOSSA_SECTIONS; i++)
    {
      ts = &model->tube.fossaSection[i];
      x[0] = graph->getXPos(ts->pos_cm);
      x[1] = graph->getXPos(ts->pos_cm + ts->length_cm);
      if (showAreas) 
      { 
        y = graph->getYPos(ts->area_cm2); 
      }
      else 
      { 
        y = graph->getYPos( TlModel::getCircumference(ts->area_cm2) ); 
      }

      dc.SetPen(wxPen(*wxLIGHT_GREY, lineWidth));
      dc.SetBrush(*wxLIGHT_GREY_BRUSH);
      dc.DrawRectangle(x[0], y, x[1]-x[0]+1, graphY+graphH-y);

      dc.SetPen(wxPen(*wxBLACK, lineWidth));
      if (i > 0) 
      { 
        dc.DrawLine(x[0], lastY, x[0], y); 
      }
      dc.DrawLine(x[0], y, x[1], y);

      lastY = y;
    }

    for (i=0; i < Tube::NUM_NOSE_SECTIONS; i++)
    {
      ts = &model->tube.noseSection[i];
      x[0] = graph->getXPos(ts->pos_cm);
      x[1] = graph->getXPos(ts->pos_cm + ts->length_cm);
      if (showAreas) 
      { 
        y = graph->getYPos(ts->area_cm2); 
      }
      else 
      { 
        y = graph->getYPos(TlModel::getCircumference(ts->area_cm2)); 
      }
      
      dc.SetPen(wxPen(*wxLIGHT_GREY, lineWidth));
      dc.SetBrush(*wxLIGHT_GREY_BRUSH);
      dc.DrawRectangle(x[0], y, x[1]-x[0]+1, graphY+graphH-y);

      dc.SetPen(wxPen(*wxBLACK, lineWidth));
      if (i > 0) 
      { 
        dc.DrawLine(x[0], lastY, x[0], y); 
      }
      dc.DrawLine(x[0], y, x[1], y);
      lastY = y;
    }
  }

  // ****************************************************************
  // Discrete area function
  // ****************************************************************

  if (discreteAreaFunction)
  {
    cutPos = picVocalTract->cutPlanePos_cm;
    for (i=0; i < Tube::NUM_PHARYNX_MOUTH_SECTIONS; i++)
    {
      ts = &model->tube.pharynxMouthSection[i];
      x[0] = graph->getXPos(ts->pos_cm);
      x[1] = graph->getXPos(ts->pos_cm + ts->length_cm);
      if (showAreas) 
      { 
        y = graph->getYPos( ts->area_cm2 ); 
      } 
      else 
      { 
        y = graph->getYPos(TlModel::getCircumference(ts->area_cm2));
      }

      dc.SetPen(wxPen(*wxBLACK, lineWidth));
      if (i > 0) 
      { 
        dc.DrawLine(x[0], lastY, x[0], y); 
      }
      dc.DrawLine(x[0], y, x[1], y);
      
      lastY = y;
      if ((cutPos >= ts->pos_cm) && (cutPos <= ts->pos_cm + ts->length_cm)) 
      { 
        currentArea = ts->area_cm2; 
      }
    }
  }
  else

  // ****************************************************************
  // Continual area function
  // ****************************************************************
  {
    int y[2];
    double denominator;
    cutPos = picVocalTract->cutPlanePos_cm;

    for (i = 0; i < VocalTract::NUM_CENTERLINE_POINTS - 1; i++)
    {
      x[0] = graph->getXPos(tract->crossSection[i].pos);
      x[1] = graph->getXPos(tract->crossSection[i + 1].pos);

      if (showAreas)
      {
        y[0] = graph->getYPos(tract->crossSection[i].area);
        y[1] = graph->getYPos(tract->crossSection[i + 1].area);
      }
      else
      {
        y[0] = graph->getYPos(tract->crossSection[i].circ);
        y[1] = graph->getYPos(tract->crossSection[i + 1].circ);
      }

      dc.SetPen(wxPen(*wxBLACK, lineWidth));
      dc.DrawLine(x[0], y[0], x[1], y[1]);

      // Determine the area at the cutting position
      if ((cutPos >= tract->crossSection[i].pos) && (cutPos <= tract->crossSection[i + 1].pos))
      {
        denominator = tract->crossSection[i + 1].pos - tract->crossSection[i].pos;
        currentArea = tract->crossSection[i].area +
          (tract->crossSection[i + 1].area - tract->crossSection[i].area)*(cutPos - tract->crossSection[i].pos) / denominator;
      }
    }
  }

  // ****************************************************************
  // Draw the cutting position as vertical dashed line.
  // ****************************************************************

  x[0] = graph->getXPos(picVocalTract->cutPlanePos_cm);
  if ((x[0] >= graphX) && (x[0] < graphX + graphW))
  {
    wxPen pen(*wxBLACK, lineWidth, wxPENSTYLE_LONG_DASH);
    dc.SetPen(pen);
    dc.DrawLine(x[0], graphY+this->FromDIP(58), x[0], graphY+graphH-1);
  }

  // ****************************************************************
  // Find the minimum in the continual area function.
  // ****************************************************************

  double absMinArea_cm2 = 1000000.0;
  double absMinAreaPos_cm = 0.0;

  for (i=0; i < VocalTract::NUM_CENTERLINE_POINTS; i++)
  {
    if (tract->crossSection[i].area < absMinArea_cm2)
    {
      absMinArea_cm2 = tract->crossSection[i].area;
      absMinAreaPos_cm = tract->crossSection[i].pos;
    }
  }

  // ****************************************************************
  // Display text information.
  // ****************************************************************

  if (showText)
  {
    wxString st;
    int offsetX{ this->FromDIP(50) };
    int offsetY{ this->FromDIP(4) };
    int col1Width{ 0 };  // Width of the widest line in the first column
    int gutterWidth{ this->FromDIP(5) };  // Space between the two columns of text
    

    dc.SetPen(wxPen(*wxBLACK, lineWidth));
    dc.SetFont(wxFont(9, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

  	// Create the multi-line string for the first column and find the width of the widest line
    std::stringstream col1;

  	st = wxString::Format("Length: %2.2f cm", tract->centerLineLength);
    col1Width = std::max(col1Width, dc.GetTextExtent(st).GetWidth());
  	col1 << st << std::endl;

    st = wxString::Format("Velum area: %2.3f cm^2", tract->nasalPortArea_cm2);
    col1Width = std::max(col1Width, dc.GetTextExtent(st).GetWidth());
    col1 << st << std::endl;

    st = wxString::Format("Curr. area: %2.3f cm^2", currentArea);
    col1Width = std::max(col1Width, dc.GetTextExtent(st).GetWidth());
    col1 << st << std::endl;

    st = wxString::Format("Min. area: %2.3f cm^2 at %2.1f cm", absMinArea_cm2, absMinAreaPos_cm);
    col1Width = std::max(col1Width, dc.GetTextExtent(st).GetWidth());
    col1 << st << std::endl;

    if (showArticulators)
    {
        st = wxString::Format("Tongue tip sides: %2.2f", tract->param[VocalTract::TS3].x);
        col1Width = std::max(col1Width, dc.GetTextExtent(st).GetWidth());
        col1 << st << std::endl;
    }

    // Draw the first column
    dc.DrawText(col1.str(), offsetX, offsetY);
  	
    // Create the multi-line string for the second column
    std::stringstream col2;
    if (picVocalTract->showCenterLine)
    {
        st = wxString::Format("Slice pos.: %2.2f cm", picVocalTract->cutPlanePos_cm);
        col2 << st << std::endl;
    }
  	
    st = wxString::Format("Velum pos.: %2.2f cm", tract->nasalPortPos_cm);
    col2 << st << std::endl;
    
    st = wxString::Format("Teeth pos.: %2.2f cm", tract->incisorPos_cm);
    col2 << st << std::endl;

     // Draw the second column
    dc.DrawText(col2.str(), offsetX + col1Width + gutterWidth, offsetY);
    

    // Draw the teeth position
    x[0] = graph->getXPos(tract->incisorPos_cm);
    if ((x[0] >= graphX) && (x[0]+1 < graphX + graphW))
    {
      dc.DrawLine(x[0], graphY+graphH-this->FromDIP(10), x[0], graphY+graphH-1);
      dc.DrawLine(x[0]+1, graphY+graphH-this->FromDIP(10), x[0]+1, graphY+graphH-1);
    }

    // Draw the recommended minimum area for vowels as a horizontal dashed line.
    if (showAreas)
    {
      y = graph->getYPos(Data::MIN_ADVISED_VOWEL_AREA_CM2);
      dc.SetPen(wxPen(*wxBLACK, lineWidth, wxPENSTYLE_LONG_DASH));
      dc.DrawLine(graphX, y, graphX + graphW - 1, y);
    }

  }

}


// ****************************************************************************
/// Handle all mouse events.
// ****************************************************************************

void AreaFunctionPicture::OnMouseEvent(wxMouseEvent &event)
{
  int mx = event.GetX();
  int my = event.GetY();

  int graphX, graphY, graphW, graphH;
  Graph *graph = &areaGraph;
  graph->getDimensions(graphX, graphY, graphW, graphH);
  if (graphW < 1)
  {
    graphW = 1;
  }

  // ****************************************************************
  // The mouse is entering the window.
  // ****************************************************************

  if (event.Entering())
  {
    lastMx = mx;
    lastMy = my;
    return;
  }

  // ****************************************************************
  // Check if the cut position is under the mouse cursor.
  // ****************************************************************

  bool isOnCutPos = false;
  int cutPosX = graph->getXPos(picVocalTract->cutPlanePos_cm);
  if ((mx >= cutPosX-4) && (mx <= cutPosX+4))
  {
    isOnCutPos = true;
  }

  if (((isOnCutPos) && (event.Dragging() == false)) || (moveCutPos))
  {
    this->SetCursor( wxCursor(wxCURSOR_SIZEWE) );
  }
  else
  {
    this->SetCursor( wxCursor(wxCURSOR_ARROW) );
  }

  // ****************************************************************
  // Stop dragging a border when the cut pos. is released or the area
  // is left.
  // ****************************************************************

  if ((event.ButtonUp()) || (event.Leaving()))
  {
    moveCutPos = false;
    lastMx = mx;
    lastMy = my;
    return;
  }

  // ****************************************************************
  // The left mouse button just changed to down.
  // ****************************************************************

  if (event.ButtonDown(wxMOUSE_BTN_LEFT))
  {
    // Start moving the cut position.
    if (isOnCutPos)
    {
      moveCutPos = true;
    }
    lastMx = mx;
    lastMy = my;
    return;
  }

  // ****************************************************************
  // The user is dragging the mouse.
  // ****************************************************************

  if (event.Dragging())
  {
    if ((event.LeftIsDown()) && (mx >= graphX) && (mx < graphX + graphW))
    {
      if (moveCutPos)
      {
        double deltaPos_cm = 
          graph->getAbsXValue(mx) - 
          graph->getAbsXValue(lastMx);

        picVocalTract->cutPlanePos_cm+= deltaPos_cm;

        if (picVocalTract->cutPlanePos_cm < 0.0)
        {
          picVocalTract->cutPlanePos_cm = 0.0;
        }

        // Update the corresponding pictures.

        VocalTractDialog *dialog = VocalTractDialog::getInstance(this);
        if (dialog->IsShown())
        {
          dialog->Refresh();
          dialog->Update();
        }

        wxCommandEvent event(updateRequestEvent);
        event.SetInt(UPDATE_PICTURES);
        wxPostEvent(updateEventReceiver, event);
      }
    }

    lastMx = mx;
    lastMy = my;
    return;
  }

}

// ****************************************************************************

