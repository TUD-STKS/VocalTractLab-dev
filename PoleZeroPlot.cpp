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

#include <limits>
#include "PoleZeroPlot.h"

// ****************************************************************************
// IDs.
// ****************************************************************************

static const int IDM_INSERT_POLE        = 8000;
static const int IDM_INSERT_ZERO        = 8001;
static const int IDM_DELETE_POLE        = 8002;
static const int IDM_DELETE_ZERO        = 8003;
static const int IDM_DELETE_ALL_POLES   = 8004;
static const int IDM_DELETE_ALL_ZEROS   = 8005;

// ****************************************************************************
// The event table.
// ****************************************************************************

BEGIN_EVENT_TABLE(PoleZeroPlot, BasicPicture)
  EVT_MOUSE_EVENTS(PoleZeroPlot::OnMouseEvent)
  EVT_MENU(IDM_INSERT_POLE, PoleZeroPlot::OnInsertPole)
  EVT_MENU(IDM_INSERT_ZERO, PoleZeroPlot::OnInsertZero)
  EVT_MENU(IDM_DELETE_POLE, PoleZeroPlot::OnDeletePole)
  EVT_MENU(IDM_DELETE_ZERO, PoleZeroPlot::OnDeleteZero)
  EVT_MENU(IDM_DELETE_ALL_POLES, PoleZeroPlot::OnDeleteAllPoles)
  EVT_MENU(IDM_DELETE_ALL_ZEROS, PoleZeroPlot::OnDeleteAllZeros)
END_EVENT_TABLE()


// ****************************************************************************
/// Constructor. Init the variables.
// ****************************************************************************

PoleZeroPlot::PoleZeroPlot(wxWindow *parent) : BasicPicture(parent)
{
  // ****************************************************************
  // Init the variables.
  // ****************************************************************

  // Graphs.

  graph.init(this, 45, 0, 0, 25);
  
  graph.initAbscissa(PQ_FREQUENCY, 0.0, 1.0,
    0.0, 0.0, 0.0, 
    300.0, 300.0, 300.0,
    1, 0, false, false, true);

  graph.initLinearOrdinate(PQ_FREQUENCY, 0.0, 1.0,
    0.0, 0.0, 0.0, 
    6000.0, 6000.0, 6000.0,
    1, 0, false, false, true);

  // Create the context menu.

  contextMenu = new wxMenu();
  contextMenu->Append(IDM_INSERT_POLE, "Insert pole");
  contextMenu->Append(IDM_INSERT_ZERO, "Insert zero");
  contextMenu->AppendSeparator();
  contextMenu->Append(IDM_DELETE_POLE, "Delete pole");
  contextMenu->Append(IDM_DELETE_ZERO, "Delete zero");
  contextMenu->AppendSeparator();
  contextMenu->Append(IDM_DELETE_ALL_POLES, "Delete all poles");
  contextMenu->Append(IDM_DELETE_ALL_ZEROS, "Delete all zeros");

  // Other variables.

  plan = Data::getInstance()->poleZeroPlan;

  menuX = 0;
  menuY = 0;
  lastMx = 0;
  lastMy = 0;
}

// ****************************************************************************
/// Draws the picture.
// ****************************************************************************

void PoleZeroPlot::draw(wxDC &dc)
{
  const int RADIUS = 4;
  int i;
  int x, y;
  int graphX, graphY, graphW, graphH;

  graph.getDimensions(graphX, graphY, graphW, graphH);

  // ****************************************************************
  // Fill the background and draw the graph.
  // ****************************************************************

  dc.SetBackground(*wxWHITE_BRUSH);
  dc.Clear();

  graph.paintAbscissa(dc);
  graph.paintOrdinate(dc);


  // ****************************************************************
  // Draw the poles.
  // ****************************************************************

  for (i=0; i < (int)plan->poles.size(); i++)
  {
    x = graph.getXPos(plan->poles[i].bw_Hz);
    y = graph.getYPos(plan->poles[i].freq_Hz);

    if (plan->selectedPole == i)
    {
      dc.SetPen(wxPen(*wxBLUE, 3));
    }
    else
    {
      dc.SetPen(wxPen(*wxBLACK, 3));
    }

    dc.DrawLine(x-RADIUS, y-RADIUS, x+RADIUS, y+RADIUS);
    dc.DrawLine(x+RADIUS, y-RADIUS, x-RADIUS, y+RADIUS);
  }

  // ****************************************************************
  // Draw the zeros.
  // ****************************************************************

  for (i=0; i < (int)plan->zeros.size(); i++)
  {
    x = graph.getXPos(plan->zeros[i].bw_Hz);
    y = graph.getYPos(plan->zeros[i].freq_Hz);

    if (plan->selectedZero == i)
    {
      dc.SetPen(wxPen(*wxBLUE, 2));
    }
    else
    {
      dc.SetPen(wxPen(*wxBLACK, 2));
    }

    dc.DrawCircle(x, y, RADIUS+1);
  }
}


// ****************************************************************************
/// Send an event to the parent dialog of this picture to update all dialog
/// controls including this picture.
// ****************************************************************************

void PoleZeroPlot::updateParent()
{
  wxCommandEvent event(updateRequestEvent);
  event.SetInt(REFRESH_PICTURES_AND_CONTROLS);
  wxPostEvent(this->GetParent(), event);
}


// ****************************************************************************
/// Returns the index of the nearest pole in the vicinity of the window 
/// position (x,y), or -1, if there is no near pole.
// ****************************************************************************

int PoleZeroPlot::getNearPole(int x, int y)
{
  int itemX;
  int itemY;
  double distance;
  double minDistance = numeric_limits<double>::max();
  int dx, dy;
  int i;
  vector<PoleZeroPlan::Location> &list = plan->poles;
  int N = (int)list.size();
  int winner = -1;

  for (i=0; i < N; i++)
  {
    itemX = graph.getXPos(list[i].bw_Hz);
    itemY = graph.getYPos(list[i].freq_Hz);
    dx = x - itemX;
    dy = y - itemY;
    distance = sqrt((double)(dx*dx + dy*dy));

    if (distance < minDistance)
    {
      winner = i;
      minDistance = distance;
    }
  }

  const int MAX_DISTANCE_PIX = 8;
  if (minDistance > MAX_DISTANCE_PIX)
  {
    winner = -1;
  }

  return winner;
}


// ****************************************************************************
/// Returns the index of the nearest zero in the vicinity of the window 
/// position (x,y), or -1, if there is no near zero.
// ****************************************************************************

int PoleZeroPlot::getNearZero(int x, int y)
{
  int itemX;
  int itemY;
  double distance;
  double minDistance = numeric_limits<double>::max();
  int dx, dy;
  int i;
  vector<PoleZeroPlan::Location> &list = plan->zeros;
  int N = (int)list.size();
  int winner = -1;

  for (i=0; i < N; i++)
  {
    itemX = graph.getXPos(list[i].bw_Hz);
    itemY = graph.getYPos(list[i].freq_Hz);
    dx = x - itemX;
    dy = y - itemY;
    distance = sqrt((double)(dx*dx + dy*dy));

    if (distance < minDistance)
    {
      winner = i;
      minDistance = distance;
    }
  }

  const int MAX_DISTANCE_PIX = 8;
  if (minDistance > MAX_DISTANCE_PIX)
  {
    winner = -1;
  }

  return winner;
}


// ****************************************************************************
/// Mouse event handling.
// ****************************************************************************

void PoleZeroPlot::OnMouseEvent(wxMouseEvent &event)
{
  int mx = event.GetX();
  int my = event.GetY();


  // ****************************************************************
  // The mouse is entering the window or a mouse button was released.
  // ****************************************************************

  if ((event.Entering()) || (event.ButtonUp()))
  {
    plan->selectedPole = -1;
    plan->selectedZero = -1;
    lastMx = mx;
    lastMy = my;
    this->Refresh();
  }

  // ****************************************************************
  // Mouse is outside the graph region -> return.
  // ****************************************************************

  int graphX, graphY, graphW, graphH;
  graph.getDimensions(graphX, graphY, graphW, graphH);

  if ((mx < graphX) || (mx >= graphX + graphW) || (my < graphY) || (my >= graphY + graphH))
  {
    plan->selectedPole = -1;
    plan->selectedZero = -1;
    lastMx = mx;
    lastMy = my;
  }

  // Change the type of the cursor when a pole or zero is under the cursor.
  
  if ((getNearPole(mx, my) != -1) || (getNearZero(mx, my) != -1) || 
      (plan->selectedPole != -1) || (plan->selectedZero != -1))
  {
    this->SetCursor( wxCursor(wxCURSOR_SIZING) );
  }
  else
  {
    this->SetCursor( wxCursor(wxCURSOR_ARROW) );
  }

  // ****************************************************************
  // The right mouse button changed to down.
  // ****************************************************************

  if (event.RightDown())
  {
    menuX = mx;
    menuY = my;
    this->SetCursor( wxCursor(wxCURSOR_ARROW) );
    PopupMenu(contextMenu);
    return;
  }

  // ****************************************************************
  // The left mouse button changed to down.
  // ****************************************************************

  if (event.LeftDown())
  {
    if (getNearPole(mx, my) != -1)
    {
      plan->selectedPole = getNearPole(mx, my);
    }
    else
    {
      plan->selectedZero = getNearZero(mx, my);
    }

    this->Refresh();
    this->updateParent();
    lastMx = mx;
    lastMy = my;
    return;
  }

  // ****************************************************************
  // The mouse is dragged (with one or more mouse buttons pressed).
  // ****************************************************************

  if (event.Dragging())
  {
    double freq_Hz = graph.getAbsYValue(my);
    double bw_Hz = graph.getAbsXValue(mx);

    if (plan->selectedPole != -1)
    {
      plan->poles[ plan->selectedPole ].freq_Hz = freq_Hz;
      plan->poles[ plan->selectedPole ].bw_Hz = bw_Hz;
    }
    else
    if (plan->selectedZero != -1)
    {
      plan->zeros[ plan->selectedZero ].freq_Hz = freq_Hz;
      plan->zeros[ plan->selectedZero ].bw_Hz = bw_Hz;
    }

    this->updateParent();
    lastMx = mx;
    lastMy = my;
  }

  lastMx = mx;
  lastMy = my;
}

// ****************************************************************************
/// Insert a single pole.
// ****************************************************************************

void PoleZeroPlot::OnInsertPole(wxCommandEvent &event)
{
  if ((int)plan->poles.size() < 10)
  {
    PoleZeroPlan::Location p;
    p.bw_Hz = graph.getAbsXValue(menuX);
    p.freq_Hz = graph.getAbsYValue(menuY);
    plan->poles.push_back(p);

    updateParent();
  }
  else
  {
    wxMessageBox("You can not have more than 10 poles!", "Pole not added");
  }
}

// ****************************************************************************
/// Insert a single zero.
// ****************************************************************************

void PoleZeroPlot::OnInsertZero(wxCommandEvent &event)
{
  if ((int)plan->zeros.size() < 10)
  {
    PoleZeroPlan::Location z;
    z.bw_Hz = graph.getAbsXValue(menuX);
    z.freq_Hz = graph.getAbsYValue(menuY);
    plan->zeros.push_back(z);

    updateParent();
  }
  else
  {
    wxMessageBox("You can not have more than 10 zeros!", "Zero not added");
  }
}

// ****************************************************************************
/// Delete the pole nearest to the mouse cursor.
// ****************************************************************************

void PoleZeroPlot::OnDeletePole(wxCommandEvent &event)
{
  int index = getNearPole(menuX, menuY);
  if (index != -1)
  {
    plan->poles.erase( plan->poles.begin() + index );
    updateParent();
  }
}

// ****************************************************************************
/// Delete the zero nearest to the mouse cursor.
// ****************************************************************************

void PoleZeroPlot::OnDeleteZero(wxCommandEvent &event)
{
  int index = getNearZero(menuX, menuY);
  if (index != -1)
  {
    plan->zeros.erase( plan->zeros.begin() + index );
    updateParent();
  }
}

// ****************************************************************************
/// Delete all poles.
// ****************************************************************************

void PoleZeroPlot::OnDeleteAllPoles(wxCommandEvent &event)
{
  plan->poles.clear();
  updateParent();
}

// ****************************************************************************
/// Delete all zeros.
// ****************************************************************************

void PoleZeroPlot::OnDeleteAllZeros(wxCommandEvent &event)
{
  plan->zeros.clear();
  updateParent();
}


// ****************************************************************************
