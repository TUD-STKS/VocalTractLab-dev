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

#include "Spectrum3dPicture.h"
#include "ParamSimu3DDialog.h"
#include <fstream>
#include <wx/filename.h>

// ****************************************************************************
// IDs.
// ****************************************************************************

static const int IDM_EXPORT_GLOTTAL_TF = 1000;
static const int IDM_EXPORT_NOISE_SRC_TF = 1001;
static const int IDM_EXPORT_INPUT_IMPEDANCE = 1002;
static const int IDM_EXPORT_RAD_IMPED_REAL_PART = 1003;
static const int IDM_EXPORT_RAD_IMPED_IMAG_PART = 1004;

// ****************************************************************************
// The event table.
// ****************************************************************************

BEGIN_EVENT_TABLE(Spectrum3dPicture, BasicPicture)
  EVT_MOUSE_EVENTS(Spectrum3dPicture::OnMouseEvent)
  EVT_MENU(IDM_EXPORT_GLOTTAL_TF, Spectrum3dPicture::OnExportGlottalTf)
  EVT_MENU(IDM_EXPORT_NOISE_SRC_TF, Spectrum3dPicture::OnEXportNoiseSrcTf)
  EVT_MENU(IDM_EXPORT_INPUT_IMPEDANCE, Spectrum3dPicture::OnExportInputImpedance)
  EVT_MENU(IDM_EXPORT_RAD_IMPED_REAL_PART, Spectrum3dPicture::OnExportRadImpedRealPart)
  EVT_MENU(IDM_EXPORT_RAD_IMPED_IMAG_PART, Spectrum3dPicture::OnExportRadImpedImagPart)
END_EVENT_TABLE()

// ****************************************************************************
/// Constructor.
// ****************************************************************************

Spectrum3dPicture::Spectrum3dPicture(wxWindow *parent, 
    Acoustic3dSimulation *simu3d): BasicPicture(parent),
  m_idxPtTf(0)
{
  // ****************************************************************
  // Init the variables
  // ****************************************************************

  this->simu3d = simu3d;

  m_showGlottalTf = true;
  m_showNoiseTf = true;
  m_showInputImped = true;

  // ****************************************************************
  // The spectrum graph
  // ****************************************************************

  graph.init(this, 40, 45, 0, 25);

  graph.initAbscissa(PQ_FREQUENCY, 0.0, 100.0,
    0.0, 0.0, 0.0, 
    2000.0, 20000.0, 10000.0,
    8, 0, false, false, true);

  graph.initLogOrdinate(1.0, 5.0,
    -100.0, -10.0, -100, 
    10.0, 100.0, 50.0,
    true, 10);

  graph.isLinearOrdinate = false;
  m_changeFreqField = false;

  // ****************************************************************
  // The context menu
  // ****************************************************************

  m_contextMenu = new wxMenu();
  m_contextMenu->Append(IDM_EXPORT_GLOTTAL_TF, "Save the glottal transfer function as txt file");
  m_contextMenu->Append(IDM_EXPORT_NOISE_SRC_TF, "Save the noise transfer function as txt file");
  m_contextMenu->Append(IDM_EXPORT_INPUT_IMPEDANCE, "Save input impedance as txt file");
  m_contextMenu->Append(IDM_EXPORT_RAD_IMPED_REAL_PART, "Save the radiation impedance real part as txt file");
  m_contextMenu->Append(IDM_EXPORT_RAD_IMPED_IMAG_PART, "Save the radiation impedance imaginary part as txt file");
}

// ****************************************************************************
// ****************************************************************************

void Spectrum3dPicture::draw(wxDC &dc)
{
  dc.SetBackground(*wxWHITE_BRUSH);
  dc.Clear();
  graph.paintAbscissa(dc);
  graph.paintOrdinate(dc);

  paintSpectrum(dc);

  //***************************************************************************
  // Draw the acoustic field frequency as a vertical dashed line
  //***************************************************************************

  int graphX, graphY, graphW, graphH;
  graph.getDimensions(graphX, graphY, graphW, graphH);
  double x = graph.getXPos(simu3d->freqAcousticField());
  if ((x >= graphX) && (x < graphX + graphW))
  {
    wxPen pen(*wxBLACK, lineWidth, wxPENSTYLE_LONG_DASH);
    dc.SetPen(pen);
    dc.DrawLine(x, graphY, x, graphY+graphH-1);
  }
}

// ****************************************************************************
// ****************************************************************************

void Spectrum3dPicture::paintSpectrum(wxDC &dc)
{
  if (m_showGlottalTf)
  {
    drawTf(dc, GLOTTAL);
  }

  if (m_showNoiseTf)
  {
    drawTf(dc, NOISE);
  }
  
  if (m_showInputImped)
  {
    drawTf(dc, INPUT_IMPED);
  }
}

// ****************************************************************************
// ****************************************************************************

void Spectrum3dPicture::drawTf(wxDC& dc, enum tfType type)
{
  int graphX, graphY, graphW, graphH, y, lastY;
  double val;

  graph.getDimensions(graphX, graphY, graphW, graphH);
  lastY = graphY;

  switch (type)
  {
  case GLOTTAL:
    dc.SetPen(wxPen(*wxBLACK, lineWidth));
    break;
  case NOISE:
    dc.SetPen(wxPen(wxColour(0, 90, 181, 255), lineWidth));
    break;
  case INPUT_IMPED:
    dc.SetPen(wxPen(wxColour(68, 170, 153, 255), lineWidth));
    break;
  }

  // create the frequency vector
  vector<double> freqs;
  freqs.reserve(graphW);
  for (int i(0); i < graphW; i++)
  {
    freqs.push_back(graph.getAbsXValue(graphX + i));
  }

  // interpolate the transfer function
  vector<complex<double>> interpolatedTF;
  simu3d->interpolateTransferFunction(freqs, m_idxPtTf, type, interpolatedTF);

  // draw the transfer function curve
  for (int i(0); i < graphW; i++)
  {
    val = abs(interpolatedTF[i]);
    y = graph.getYPos(val);

    if (y < graphY)
    {
      y = graphY;
    }
    if (y >= graphY + graphH)
    {
      y = graphY + graphH - 1;
    }

    if (isnan(val)) { y = NAN; }

    dc.DrawLine(graphX + i - 1, lastY, graphX + i, y);

    lastY = y;
  }
}

// ****************************************************************************
// ****************************************************************************

void Spectrum3dPicture::exportResult(enum tfType type)
{
  wxFileName fileName;
  wxString name = wxFileSelector("Save transfer functions", fileName.GetPath(),
    fileName.GetFullName(), ".txt", "(*.txt)|*.txt",
    wxFD_SAVE | wxFD_OVERWRITE_PROMPT, this);

  simu3d->exportTransferFucntions(name.ToStdString(), type);
}

// ****************************************************************************
// ****************************************************************************

void Spectrum3dPicture::OnMouseEvent(wxMouseEvent& event)
{
  int mx = event.GetX();
  int my = event.GetY();
  
  int graphX, graphY, graphW, graphH;
  graph.getDimensions(graphX, graphY, graphW, graphH);

  // ****************************************************************
  // Right click
  // ****************************************************************

  if (event.ButtonDown(wxMOUSE_BTN_RIGHT))
  {
    PopupMenu(m_contextMenu);
  }

  // ****************************************************************
  // The mouse is entering the window.
  // ****************************************************************

  if (event.Entering())
  {
    m_lastMousePosX = mx;
    m_lastMousePosY = my;
    return;
  }

  // ****************************************************************
  // Check if the cut position is under the mouse cursor.
  // ****************************************************************

  bool isOnCutPos = false;
  int cutPosX = graph.getXPos(simu3d->freqAcousticField());
  if ((mx >= cutPosX-4) && (mx <= cutPosX+4))
  {
    isOnCutPos = true;
  }

  if (((isOnCutPos) && (event.Dragging() == false)) || (m_changeFreqField))
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
    m_changeFreqField = false;
    m_lastMousePosX = mx;
    m_lastMousePosY = my;
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
      m_changeFreqField = true;
    }
    m_lastMousePosX = mx;
    m_lastMousePosY = my;
    return;
  }
  
  // ****************************************************************
  // The user is dragging the mouse.
  // ****************************************************************

  if (event.Dragging())
  {
    if ((event.LeftIsDown()) && (mx >= graphX) && (mx < graphX + graphW))
    {
      if (m_changeFreqField)
      {
        double deltaFreq = 
          graph.getAbsXValue(mx) - 
          graph.getAbsXValue(m_lastMousePosX);

        simu3d->setAcousticFieldFreq(
            simu3d->freqAcousticField() + deltaFreq);

        if (simu3d->freqAcousticField() < 1.0)
        {
          simu3d->setAcousticFieldFreq(1.0);
        }

        // Update the simulation parameter dialog.
        ParamSimu3DDialog* dialog = ParamSimu3DDialog::getInstance(NULL);
        dialog->updateParams();

        this->Refresh();
      }
    }

    m_lastMousePosX = mx;
    m_lastMousePosY = my;
    return;
  }
}

// ****************************************************************************
// ****************************************************************************

void Spectrum3dPicture::OnExportGlottalTf(wxCommandEvent& event)
{
  exportResult(GLOTTAL);
}

// ****************************************************************************
// ****************************************************************************

void Spectrum3dPicture::OnEXportNoiseSrcTf(wxCommandEvent& event)
{
  exportResult(NOISE);
}

// ****************************************************************************
// ****************************************************************************

void Spectrum3dPicture::OnExportInputImpedance(wxCommandEvent& event)
{
  exportResult(INPUT_IMPED);
}

// ****************************************************************************
// ****************************************************************************

void Spectrum3dPicture::OnExportRadImpedRealPart(wxCommandEvent& event)
{
  exportResult(RAD_IMPED_REAL_PART);
}

// ****************************************************************************
// ****************************************************************************

void Spectrum3dPicture::OnExportRadImpedImagPart(wxCommandEvent& event)
{
  exportResult(RAD_IMPED_IMAG_PART);
}