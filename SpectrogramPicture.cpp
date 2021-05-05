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

#include "SpectrogramPicture.h"
#include "AnalysisResultsDialog.h"


// ****************************************************************************
// The event table.
// ****************************************************************************

BEGIN_EVENT_TABLE(SpectrogramPicture, BasicPicture)
  EVT_MOUSE_EVENTS(SpectrogramPicture::OnMouseEvent)
END_EVENT_TABLE()


// ****************************************************************************
/// Construcor. Passes the parent parameter.
// ****************************************************************************

SpectrogramPicture::SpectrogramPicture(wxWindow *parent, wxWindow *updateEventReceiver) : BasicPicture(parent)
{
  data = Data::getInstance();
  this->updateEventReceiver = updateEventReceiver;
  spectrogramPlot = new SpectrogramPlot();
}


// ****************************************************************************
// ****************************************************************************

void SpectrogramPicture::draw(wxDC &dc)
{
  paintSpectrogram(dc);
}

// ****************************************************************************
// ****************************************************************************

void SpectrogramPicture::paintSpectrogram(wxDC &dc)
{
  int windowWidth, windowHeight;
  this->GetSize(&windowWidth, &windowHeight);

  int firstSample     = data->centerPos_pt - data->spectrogramVisTimeRange_pt/2;
  int numSamples      = data->spectrogramVisTimeRange_pt;
  double startTime_s  = (double)firstSample / (double)SAMPLING_RATE;
  double endTime_s    = (double)(firstSample + numSamples - 1) / (double)SAMPLING_RATE;
  double duration_s   = endTime_s - startTime_s;
  double viewRange_Hz = spectrogramPlot->viewRange_Hz;

  // ****************************************************************
  // Plot the spectrogram.
  // ****************************************************************

  if ((data->selectedSpectrogram >= 0) && (data->selectedSpectrogram < data->NUM_TRACKS))
  {
    int zeroX = windowWidth/2 - (data->centerPos_pt*windowWidth) / data->spectrogramVisTimeRange_pt;
    if (zeroX < 0)
    {
      spectrogramPlot->drawSpectrogram(dc, 0, 0, windowWidth, windowHeight, 
        data->track[data->selectedSpectrogram], firstSample, numSamples);
    }
    else
    {
      if (zeroX < windowWidth - 1)
      {
        // Fill the negative time region dark grey.
        dc.SetPen(*wxGREY_PEN);
        dc.SetBrush(*wxGREY_BRUSH);
        dc.DrawRectangle(0, 0, zeroX, windowHeight);

        // Draw the spectrogram in the remaining region.
        spectrogramPlot->drawSpectrogram(dc, zeroX, 0, windowWidth-zeroX, windowHeight, 
          data->track[data->selectedSpectrogram], 0, firstSample + numSamples);
      }
      else
      {
        // Fill all dark grey.
        dc.SetBrush(*wxGREY_BRUSH);
        dc.Clear();
      }
    }
  }


  // ****************************************************************
  // Plot the F0 contour.
  // ****************************************************************

  if (data->showF0)
  {
    spectrogramPlot->drawCurve(dc, 0, 0, windowWidth, windowHeight,
      data->f0Signal[data->selectedSpectrogram], data->f0TimeStep_s, startTime_s, duration_s,
      0.0, 600.0, wxColor(255, 128, 0), false);
  }

  // ****************************************************************
  // Plot the F0 contour.
  // ****************************************************************

  if (data->showVoiceQuality)
  {
    spectrogramPlot->drawCurve(dc, 0, 0, windowWidth, windowHeight,
      data->voiceQualitySignal[data->selectedSpectrogram], data->voiceQualityTimeStep_s, 
      startTime_s, duration_s, VoiceQualityEstimator::MIN_PEAK_SLOPE, 
      VoiceQualityEstimator::MAX_PEAK_SLOPE, wxColor(128, 255, 128), false);
  }

  // ****************************************************************
  // Draw the spectrogram mark.
  // ****************************************************************

  int markX = (data->mark_pt - firstSample)*windowWidth / numSamples;
  if ((markX >= 0) && (markX < windowWidth))
  {
    dc.SetPen(*wxBLACK_PEN);
    dc.DrawLine(markX, 0, markX, windowHeight-1);
  }

  // ****************************************************************
  // Draw some text.
  // ****************************************************************

  if (data->showSpectrogramText)
  {
    int w, h;
    wxString st;

    dc.SetPen(*wxBLACK_PEN);
    dc.SetBackgroundMode(wxSOLID);    // Set a solid white background
    dc.SetFont(wxFont(9, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

    st = wxString::Format("%d Hz   %2.3f s", (int)viewRange_Hz, startTime_s);
    dc.DrawText(st, 0, 0);

    st = wxString::Format("%2.3f s", endTime_s);
    dc.GetTextExtent(st, &w, &h);
    dc.DrawText(st, windowWidth - 1 - w, 0);

    st = wxString::Format("View range: %2.3f s", endTime_s - startTime_s);
    dc.GetTextExtent(st, &w, &h);
    dc.DrawText(st, windowWidth/2 - w/2, 0);

    st = "0 Hz";
    dc.GetTextExtent(st, &w, &h);
    dc.DrawText(st, 0, windowHeight-h);
  }

}


// ****************************************************************************
/// React on all mouse events.
// ****************************************************************************

void SpectrogramPicture::OnMouseEvent(wxMouseEvent &event)
{
  int mx = event.GetX();
  int my = event.GetY();

  // ****************************************************************
  // Show a tooltip with the time and frequency.
  // ****************************************************************

  if ((event.Dragging()) || (event.Moving()))
  {
    int windowWidth, windowHeight;
    this->GetSize(&windowWidth, &windowHeight);
    if (windowWidth < 1)
    {
      windowWidth = 1;
    }
    if (windowHeight < 1)
    {
      windowHeight = 1;
    }

    int firstSample     = data->centerPos_pt - data->spectrogramVisTimeRange_pt/2;
    int numSamples      = data->spectrogramVisTimeRange_pt;
    double startTime_s  = (double)firstSample / (double)SAMPLING_RATE;
    double endTime_s    = (double)(firstSample + numSamples - 1) / (double)SAMPLING_RATE;
    double duration_s   = endTime_s - startTime_s;
    double viewRange_Hz = spectrogramPlot->viewRange_Hz;

    wxString st = wxString::Format("%2.3f s, %d Hz",
      startTime_s + duration_s*(double)mx / (double)windowWidth,
      (int)((double)(windowHeight - 1 - my)*viewRange_Hz / (double)windowHeight));

    this->SetToolTip(st);
  }

  // ****************************************************************
  // The user is setting or dragging the mark.
  // ****************************************************************

  if ((event.ButtonDown()) || (event.Dragging()))
  {
    int pictureWidth;
    int pictureHeight;
    this->GetSize(&pictureWidth, &pictureHeight);
    if (pictureWidth < 1)
    {
      pictureWidth = 1;
    }

    int firstSample = data->centerPos_pt - data->spectrogramVisTimeRange_pt/2;
    int numSamples  = data->spectrogramVisTimeRange_pt;

    data->mark_pt = firstSample + numSamples*mx / pictureWidth;
    if (data->mark_pt < 0)
    {
      data->mark_pt = 0;
    }

    // Post an event to the parent page so that the spectrum picture
    // ANT THIS PICTURE are refreshed!
    
    wxCommandEvent event(updateRequestEvent);
    event.SetInt(REFRESH_PICTURES);    // Some code to specify the action.
    wxPostEvent(updateEventReceiver, event);

    // Update the numeric values in the analysis results dialog.

    AnalysisResultsDialog *dialog = AnalysisResultsDialog::getInstance();
    if (dialog->IsShown())
    {
      dialog->updateWidgets();
    }
  }
}

// ****************************************************************************

