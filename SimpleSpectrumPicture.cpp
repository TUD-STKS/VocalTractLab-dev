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

#include "SimpleSpectrumPicture.h"

// ****************************************************************************
// The event table.
// ****************************************************************************

BEGIN_EVENT_TABLE(SimpleSpectrumPicture, BasicPicture)
  EVT_MOUSE_EVENTS(SimpleSpectrumPicture::OnMouseEvent)
END_EVENT_TABLE()



// ****************************************************************************
/// Construcor. Passes the parent parameter.
// ****************************************************************************

SimpleSpectrumPicture::SimpleSpectrumPicture(wxWindow *parent) : BasicPicture(parent)
{
  const int LEFT_MARGIN = this->FromDIP(55);
  const int RIGHT_MARGIN = 0;
  const int BOTTOM_MARGIN = this->FromDIP(25);
  const int TOP_MARGIN = 0;

  data = Data::getInstance();

  // ****************************************************************
  // A graph for a spectrum with a log.-scale ordinate.
  // ****************************************************************
  
  graph = &spectrumGraph;
  graph->init(this, LEFT_MARGIN, RIGHT_MARGIN, TOP_MARGIN, BOTTOM_MARGIN);
  graph->initAbscissa(PQ_FREQUENCY, 0.0, 10.0,
                      0.0, 0.0, 0.0, 
                      8000.0, 8000.0, 8000.0,
                      1, 0, false, true, true);
  graph->initLogOrdinate(1.0, 5.0,
                        -20.0, -20.0, -20.0, 
                        60.0, 60.0, 60.0,
                        true, 10);

  graph->isLinearOrdinate = false;


  // ****************************************************************
  // A graph for a time signal.
  // ****************************************************************
  
  graph = &timeSignalGraph;
  graph->init(this, LEFT_MARGIN, RIGHT_MARGIN, TOP_MARGIN, BOTTOM_MARGIN);
  graph->initAbscissa(PQ_TIME, 0.0, 0.001,
                      0.0, 0.0, 0.0, 0.025, 0.025, 0.025,
                      1, 3, false, true, true);
  graph->initLinearOrdinate(PQ_RATIO, 0.0, 1.0,
                            0, 0, 0, 1000, 1000, 1000,
                            1, 0, false, true, true);
}


// ****************************************************************************
// ****************************************************************************

void SimpleSpectrumPicture::draw(wxDC &dc)
{
  // ****************************************************************
  // Calculate the current user spectrum, but omit the calculation
  // of the average spectrum. The average spectrum is not calculated
  // on the fly but only on request by the user.
  // ****************************************************************

  if (data->userSpectrumType != Data::AVERAGE_SPECTRUM)
  {
    data->calcUserSpectrum();
  }

  // ****************************************************************
  // Fill the background.
  // ****************************************************************

  dc.SetBackground(*wxWHITE_BRUSH);
  dc.Clear();

  // Draw a frequency or time function
  drawFunction(dc);
}

// ****************************************************************************
/// This function simply draws the signal Data::userSpectrum as either a 
/// frequency spectrum or a time signal (depending on Data::spectrumType).
// ****************************************************************************

void SimpleSpectrumPicture::drawFunction(wxDC &dc)
{
  // Safety check.
  if (data->userSpectrum->N == 0)
  {
    return;
  }

  // ****************************************************************
  // Paint the scales.
  // ****************************************************************

  int i;
  ComplexSignal *s = data->userSpectrum;

  if ((data->userSpectrumType == Data::CEPSTRUM) || (data->userSpectrumType == Data::TEST_SPECTRUM))
  {
    graph = &timeSignalGraph;
    // Set the upper time limit for the abscissa.
    graph->abscissa.positiveLimit = (double)data->userSpectrum->N / (double)SAMPLING_RATE;

    // Scale the ordinate to fit the value range of the time signal.
    double minValue = s->re[0];
    double maxValue = s->re[0];

    for (i=1; i < s->N; i++)
    {
      if (s->re[i] < minValue)
      {
        minValue = s->re[i];
      }
      if (s->re[i] > maxValue)
      {
        maxValue = s->re[i];
      }
    }

    if (maxValue < minValue + 1.0)
    {
      maxValue = minValue + 1.0;
    }
    graph->linearOrdinate.negativeLimit = minValue;
    graph->linearOrdinate.positiveLimit = maxValue;
  }
  else
  {
    graph = &spectrumGraph;
    // Set the upper frequency limit for the abscissa.
    graph->abscissa.positiveLimit = 8000.0;
  }

  int graphX, graphY, graphW, graphH;
  graph->getDimensions(graphX, graphY, graphW, graphH);

  graph->paintAbscissa(dc);
  graph->paintOrdinate(dc);

  // ****************************************************************
  // Paint a time signal.
  // ****************************************************************

  if (graph == &timeSignalGraph)
  {
    int x, y;
    int lastY = 0;
    double t_s;
    double timeStep_s = 1.0 / (double)SAMPLING_RATE;
    int index;
    double w;
    double value;

    dc.SetPen(wxPen(*wxBLACK, lineWidth));

    for (i=0; i < graphW; i++)
    {
      x = graphX + i;
      t_s = graph->getAbsXValue(x);
      index = (int)(t_s / timeStep_s);
      w = (t_s - index*timeStep_s) / timeStep_s;    // 0 <= w <= 1
      if ((index >= 0) && (index < s->N-1))
      {
        value = (1.0-w)*s->re[index] + w*s->re[index+1];
        y = graph->getYPos(value);

        if (i > 0)
        {
          dc.DrawLine(x-1, lastY, x, y);
        }
        lastY = y;
      }
    }
  }
  else

  // ****************************************************************
  // Paint a frequency signal.
  // ****************************************************************
  {
    int x, y;
    int lastY = 0;
    double f_Hz;
    double freqStep_Hz = (double)SAMPLING_RATE / (double)s->N;
    int index;
    double w;
    double value;

    dc.SetPen(wxPen(*wxBLACK, lineWidth));

    for (i=0; i < graphW; i++)
    {
      x = graphX + i;
      f_Hz = graph->getAbsXValue(x);
      index = (int)(f_Hz / freqStep_Hz);
      w = (f_Hz - index*freqStep_Hz) / freqStep_Hz;    // 0 <= w <= 1
      if ((index >= 0) && (index < s->N-1))
      {
        value = (1.0-w)*s->getMagnitude(index) + w*s->getMagnitude(index+1);
        y = graph->getYPos(value);
        if (y < 0)
        {
          y = 0;
        }
        if (y >= graphH)
        {
          y = graphH - 1;
        }

        if (i > 0)
        {
          dc.DrawLine(x-1, lastY, x, y);
        }
        lastY = y;
      }
    }
  }

}

// ****************************************************************************
/// React on all mouse events.
// ****************************************************************************

void SimpleSpectrumPicture::OnMouseEvent(wxMouseEvent &event)
{
  int mx = event.GetX();
  int my = event.GetY();

  int graphX = 0, graphY = 0, graphW = 1, graphH = 1;
  
  if (graph == &spectrumGraph)
  {
    graph->getDimensions(graphX, graphY, graphW, graphH);
    if (graphW < 1)
    {
      graphW = 1;
    }
  }

  // ****************************************************************
  // Show a tooltip with the frequency in the picture.
  // ****************************************************************

  if ((event.Dragging()) || (event.Moving()))
  {
    if (graph == &spectrumGraph)
    {
      if ((mx >= graphX) && (mx < graphX + graphW) && (my >= graphY) && (my < graphY + graphH))
      {
        double f_Hz = graph->getAbsXValue(mx);
        wxString st = wxString::Format("%d Hz", (int)f_Hz);
        this->SetToolTip(st);
      }
      else
      {
        this->SetToolTip("");
      }
    }
    else
    {
      this->SetToolTip("");
    }
  }
}

// ****************************************************************************

