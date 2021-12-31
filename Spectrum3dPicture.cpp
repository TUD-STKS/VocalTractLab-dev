#include "Spectrum3dPicture.h"
#include <fstream>

// ****************************************************************************
/// Constructor.
// ****************************************************************************

Spectrum3dPicture::Spectrum3dPicture(wxWindow *parent, 
    Acoustic3dSimulation *simu3d): BasicPicture(parent)
{
  // ****************************************************************
  // Init the variables
  // ****************************************************************

  this->simu3d = simu3d;

  // ****************************************************************
  // The spectrum graph
  // ****************************************************************

  graph.init(this, 40, 45, 0, 25);

  graph.initAbscissa(PQ_FREQUENCY, 0.0, 100.0,
    0.0, 0.0, 0.0, 
    2000.0, 14000.0, 5000.0,
    8, 0, false, false, true);

  graph.initLogOrdinate(1.0, 5.0,
    -100.0, -10.0, -30, 10.0, 100.0, 50.0,
    true, 10);

  graph.isLinearOrdinate = false;
}

// ****************************************************************************
// ****************************************************************************

void Spectrum3dPicture::draw(wxDC &dc)
{
  // ****************************************************************
  // Clear the background and paint the axes.
  // ****************************************************************

  dc.SetBackground(*wxWHITE_BRUSH);
  dc.Clear();
  graph.paintAbscissa(dc);
  graph.paintOrdinate(dc);

  paintSpectrum(dc);
}

// ****************************************************************************
// ****************************************************************************

void Spectrum3dPicture::paintSpectrum(wxDC &dc)
{
  int graphX, graphY, graphW, graphH, y, lastY;
  double freq, val;

  graph.getDimensions(graphX, graphY, graphW, graphH);

  dc.SetPen(wxPen(*wxBLACK, lineWidth));
  
// 
  for (int i(0); i < graphW; i++)
  {
    freq = graph.getAbsXValue(graphX + i);
    val = abs(simu3d->interpolateTransferFunction(freq, 0));

    y = graph.getYPos(val);

    if (y < graphY) 
    { 
      y = graphY; 
    }
    if (y >= graphY + graphH) 
    { 
      y = graphY + graphH - 1; 
    }

    if (i > 0) 
    { 
      dc.DrawLine(graphX+i-1, lastY, graphX+i, y); 
    }

    lastY = y;
  }
}
