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

#include "SignalComparisonPicture.h"
#include "Data.h"
#include "AnalysisResultsDialog.h"
#include "AnnotationDialog.h"
#include "SilentMessageBox.h"
#include "Backend/SoundLib.h"
#include <wx/numdlg.h> 

// ****************************************************************************
// IDs.
// ****************************************************************************

static const int IDM_PLAY_SEGMENT_MAIN_TRACK = 9000;
static const int IDM_PLAY_SEGMENT_EXTRA_TRACK = 9001;
static const int IDM_INSERT_SEGMENT = 9002;
static const int IDM_DELETE_SEGMENT = 9003;
static const int IDM_CLEAR_SEGMENT_SELECTION = 9004;
static const int IDM_SET_MIN_SEGMENT_DURATION = 9005;

static const int IDM_SET_REFERENCE_MARK   = 9010;
static const int IDM_CLEAR_REFERENCE_MARK = 9011;
static const int IDM_SWAP_TRACKS          = 9012;

static const int IDM_CLEAR_SELECTION = 9020;
static const int IDM_SET_SELECTION_START = 9021;
static const int IDM_SET_SELECTION_END = 9022;
static const int IDM_SELECT_ALL = 9023;

static const int IDM_SPECTROGRAM_4KHZ = 9030;
static const int IDM_SPECTROGRAM_8KHZ = 9031;
static const int IDM_SPECTROGRAM_12KHZ = 9032;


// ****************************************************************************
// The event table.
// ****************************************************************************

BEGIN_EVENT_TABLE(SignalComparisonPicture, BasicPicture)
  EVT_MOUSE_EVENTS(SignalComparisonPicture::OnMouseEvent)

  EVT_MENU(IDM_PLAY_SEGMENT_MAIN_TRACK, SignalComparisonPicture::OnPlaySegmentMainTrack)
  EVT_MENU(IDM_PLAY_SEGMENT_EXTRA_TRACK, SignalComparisonPicture::OnPlaySegmentExtraTrack)

  EVT_MENU(IDM_INSERT_SEGMENT, SignalComparisonPicture::OnInsertSegment)
  EVT_MENU(IDM_DELETE_SEGMENT, SignalComparisonPicture::OnDeleteSegment)
  EVT_MENU(IDM_CLEAR_SEGMENT_SELECTION, SignalComparisonPicture::OnClearSegmentSelection)
  EVT_MENU(IDM_SET_MIN_SEGMENT_DURATION, SignalComparisonPicture::OnSetMinSegmentDuration)

  EVT_MENU(IDM_SET_REFERENCE_MARK, SignalComparisonPicture::OnSetReferenceMark)
  EVT_MENU(IDM_CLEAR_REFERENCE_MARK, SignalComparisonPicture::OnClearReferenceMark)
  EVT_MENU(IDM_SWAP_TRACKS, SignalComparisonPicture::OnSwapTracks)

  EVT_MENU(IDM_CLEAR_SELECTION, SignalComparisonPicture::OnClearSelection)
  EVT_MENU(IDM_SET_SELECTION_START, SignalComparisonPicture::OnSetSelectionStart)
  EVT_MENU(IDM_SET_SELECTION_END, SignalComparisonPicture::OnSetSelectionEnd)
  EVT_MENU(IDM_SELECT_ALL, SignalComparisonPicture::OnSelectAll)
  
  EVT_MENU(IDM_SPECTROGRAM_4KHZ, SignalComparisonPicture::OnSpectrogramRange4kHz)
  EVT_MENU(IDM_SPECTROGRAM_8KHZ, SignalComparisonPicture::OnSpectrogramRange8kHz)
  EVT_MENU(IDM_SPECTROGRAM_12KHZ, SignalComparisonPicture::OnSpectrogramRange12kHz)
  END_EVENT_TABLE()


// ****************************************************************************
/// Constructor. Init the variables.
// ****************************************************************************

SignalComparisonPicture::SignalComparisonPicture(wxWindow *parent, wxWindow *updateParent)
  : BasicPicture(parent)
{
  int i;
  this->updateParent = updateParent;
  data = Data::getInstance();

  for (i=0; i < NUM_ROWS; i++)
  {
    rowY[i] = 0;
    rowH[i] = 0;
  }
  verticalOffset_percent = 0.0;

  for (i=0; i < NUM_PLAY_BUTTONS; i++)
  {
    playButtonX[i] = 0;
    playButtonY[i] = 0;
  }

  spectrogramPlot = new SpectrogramPlot();

  showExtraTrack = false;
  showSonagrams = true;
  showSegmentation = true;
  showModelF0Curve = true;

  // Create the context menu for the segment sequence.

  contextMenu = new wxMenu();

  contextMenu->Append(IDM_PLAY_SEGMENT_MAIN_TRACK, "Play segment (main track)");
  contextMenu->Append(IDM_PLAY_SEGMENT_EXTRA_TRACK, "Play segment (extra track)");
  contextMenu->AppendSeparator();
  contextMenu->Append(IDM_INSERT_SEGMENT, "Insert segment");
  contextMenu->Append(IDM_DELETE_SEGMENT, "Delete segment");
  contextMenu->Append(IDM_CLEAR_SEGMENT_SELECTION, "Clear segment selection");
  contextMenu->Append(IDM_SET_MIN_SEGMENT_DURATION, "Set min. segment duration");

  // Create the context menu for the spectrogram.

  contextMenuSpectrogram = new wxMenu();
  contextMenuSpectrogram->Append(IDM_SET_REFERENCE_MARK, "Set reference mark");
  contextMenuSpectrogram->Append(IDM_CLEAR_REFERENCE_MARK, "Clear reference mark");
  contextMenuSpectrogram->Append(IDM_SWAP_TRACKS, "Swap audio tracks");
  contextMenuSpectrogram->AppendSeparator();
  contextMenuSpectrogram->Append(IDM_CLEAR_SELECTION, "Clear selection");
  contextMenuSpectrogram->Append(IDM_SET_SELECTION_START, "Set selection start");
  contextMenuSpectrogram->Append(IDM_SET_SELECTION_END, "Set selection end");
  contextMenuSpectrogram->Append(IDM_SELECT_ALL, "Select all");
  contextMenuSpectrogram->AppendSeparator();
  contextMenuSpectrogram->Append(IDM_SPECTROGRAM_4KHZ, "Spectrogram up to 4 kHz", wxEmptyString, wxITEM_RADIO);
  contextMenuSpectrogram->Append(IDM_SPECTROGRAM_8KHZ, "Spectrogram up to 8 kHz", wxEmptyString, wxITEM_RADIO);
  contextMenuSpectrogram->Append(IDM_SPECTROGRAM_12KHZ, "Spectrogram up to 12 kHz", wxEmptyString, wxITEM_RADIO);

  menuX = 0;
  menuY = 0;
  lastMx = 0;
  lastMy = 0;
  moveBorder = false;
}


// ****************************************************************************
/// Overwritten function of the base class.
// ****************************************************************************

void SignalComparisonPicture::draw(wxDC &dc)
{
  paintSignals(dc);
}


// ****************************************************************************
/// Returns the virtual height of the picture.
// ****************************************************************************

int SignalComparisonPicture::getVirtualHeight()
{
  calcRowCoord();
  int h = rowY[NUM_ROWS-1] + rowH[NUM_ROWS-1];
  return h;
}


// ****************************************************************************
/// Returns the heigth of this picture on the screen.
// ****************************************************************************

int SignalComparisonPicture::getWindowHeight()
{
  return this->GetSize().GetHeight();
}


// ****************************************************************************
/// Sets the vertical scroll offset in percent.
// ****************************************************************************

void SignalComparisonPicture::setVertOffset(double offset_percent)
{
  verticalOffset_percent = offset_percent;
}


// ****************************************************************************
/// Returns the vertical scroll offset in percent.
// ****************************************************************************

double SignalComparisonPicture::getVertOffset_percent()
{
  return verticalOffset_percent;
}


// ****************************************************************************
/// Do the actual painting.
// ****************************************************************************

void SignalComparisonPicture::paintSignals(wxDC &dc)
{
  wxColor mainF0Color(255, 128, 0);
  wxColor extraF0Color(255, 215, 0);
  wxColor voiceQualityColor(128, 255, 128);
  int x, y;

  // Clear the background
  dc.SetBackground(*wxWHITE_BRUSH);
  dc.Clear();

  int windowWidth, windowHeight;
  this->GetSize(&windowWidth, &windowHeight);

  // ****************************************************************
  // Calc. the y-pos. and height of the rows.
  // ****************************************************************

  int i;
  //calcRowCoord();    // <- is done in getVirtualHeight() !
  int virtualHeight = getVirtualHeight();

  if (virtualHeight > windowHeight)
  {
    int delta = (int)(verticalOffset_percent*0.01*(virtualHeight - windowHeight));
    for (i=0; i < NUM_ROWS; i++)
    {
      rowY[i]-= delta;
    }
  }

  // ****************************************************************
  // ****************************************************************

  const int LEFT_MARGIN = this->FromDIP(Data::LEFT_SCORE_MARGIN);
  double startTime_s = data->gsTimeAxisGraph->abscissa.reference;
  double duration_s  = data->gsTimeAxisGraph->abscissa.positiveLimit;
  
  int firstSample = startTime_s * (double)SAMPLING_RATE;
  int numSamples = duration_s * (double)SAMPLING_RATE;
  int index = 0;

  // ****************************************************************
  // Paint the four subplots.
  // ****************************************************************

  dc.SetFont(wxFont(9, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

  index = SEGMENTATION_ROW_1;
  if (rowH[index] > 0)
  {
    paintWordSegmentation(dc, LEFT_MARGIN, rowY[index], 
      windowWidth-LEFT_MARGIN, rowH[index], startTime_s, duration_s);
    dc.DrawText("Words", 5, rowY[index]);
  }

  index = SEGMENTATION_ROW_2;
  if (rowH[index] > 0)
  {
    paintPhoneSegmentation(dc, LEFT_MARGIN, rowY[index], 
      windowWidth-LEFT_MARGIN, rowH[index], startTime_s, duration_s);
    dc.DrawText("Phones", 5, rowY[index]);
  }

  // ****************************************************************
  // ****************************************************************

  index = OSCILLOGRAM_ROW_1;
  if ((rowH[index] > 0) && (rowY[index] < windowHeight) && (rowY[index] + rowH[index] >= 0))
  {
    // Paint the oscillogram.
    paintOscillogram(dc, LEFT_MARGIN, rowY[index], windowWidth-LEFT_MARGIN, rowH[index],
      data->track[Data::MAIN_TRACK], firstSample, numSamples);
    dc.DrawText("Main oscillo.", 5, rowY[index]);

    playButtonX[0] = (LEFT_MARGIN - this->FromDIP(PLAY_BUTTON_WIDTH)) / 2;
    playButtonY[0] = rowY[index] + this->FromDIP(20);
    paintPlayButton(dc, playButtonX[0], playButtonY[0]);
  }
  else
  {
    playButtonX[0] = 1000000;
    playButtonY[0] = 1000000;
  }

  // ****************************************************************
  // ****************************************************************

  index = SPECTROGRAM_ROW_1;
  if ((rowH[index] > 0) && (rowY[index] < windowHeight) && (rowY[index] + rowH[index] >= 0))
  {
    // Paint the main spectrogram.
    spectrogramPlot->drawSpectrogram(dc, LEFT_MARGIN, rowY[index], windowWidth-LEFT_MARGIN, rowH[index], 
      data->track[Data::MAIN_TRACK], firstSample, numSamples);
    dc.DrawText("Main spectro.", 5, rowY[index]);

    if (showModelF0Curve)
    {
      // Paint the model F0 curve for the main track (from the gestural score).
      vector<double> *modelF0Curve = &data->gesturalScore->glottisParamCurve[Glottis::FREQUENCY];
      spectrogramPlot->drawCurve(dc, LEFT_MARGIN, rowY[index], windowWidth-LEFT_MARGIN, rowH[index], 
        *modelF0Curve, 1.0/GesturalScore::CURVE_SAMPLING_RATE, startTime_s, duration_s,
        0.0, 600.0, mainF0Color, true);
    }

    if (data->showF0)
    {
      // Paint the F0 curve for the main track.
      spectrogramPlot->drawCurve(dc, LEFT_MARGIN, rowY[index], windowWidth-LEFT_MARGIN, rowH[index], 
        data->f0Signal[Data::MAIN_TRACK], data->f0TimeStep_s, startTime_s, duration_s,
        0.0, 600.0, mainF0Color, false);

      // Paint the F0 curve for the extra track.
      spectrogramPlot->drawCurve(dc, LEFT_MARGIN, rowY[index], windowWidth-LEFT_MARGIN, rowH[index], 
        data->f0Signal[Data::EXTRA_TRACK], data->f0TimeStep_s, startTime_s, duration_s,
        0.0, 600.0, extraF0Color, false);
    }

    if (data->showVoiceQuality)
    {
      // Paint the voice quality curve for the main track.
      spectrogramPlot->drawCurve(dc, LEFT_MARGIN, rowY[index], windowWidth-LEFT_MARGIN, rowH[index], 
        data->voiceQualitySignal[Data::MAIN_TRACK], data->voiceQualityTimeStep_s, startTime_s, duration_s,
        VoiceQualityEstimator::MIN_PEAK_SLOPE, VoiceQualityEstimator::MAX_PEAK_SLOPE, voiceQualityColor, false);

      // Paint the voice quality curve for the extra track.
      spectrogramPlot->drawCurve(dc, LEFT_MARGIN, rowY[index], windowWidth-LEFT_MARGIN, rowH[index], 
        data->voiceQualitySignal[Data::EXTRA_TRACK], data->voiceQualityTimeStep_s, startTime_s, duration_s,
        VoiceQualityEstimator::MIN_PEAK_SLOPE, VoiceQualityEstimator::MAX_PEAK_SLOPE, voiceQualityColor, true);
    }
  }

  // ****************************************************************
  // ****************************************************************

  index = OSCILLOGRAM_ROW_2;
  if ((rowH[index] > 0) && (rowY[index] < windowHeight) && (rowY[index] + rowH[index] >= 0))
  {
    paintOscillogram(dc, LEFT_MARGIN, rowY[index], windowWidth-LEFT_MARGIN, rowH[index],
      data->track[Data::EXTRA_TRACK], firstSample, numSamples);
    dc.DrawText("Extra oscillo.", 5, rowY[index]);

    playButtonX[1] = (LEFT_MARGIN - this->FromDIP(PLAY_BUTTON_WIDTH)) / 2;
    playButtonY[1] = rowY[index] + this->FromDIP(20);
    paintPlayButton(dc, playButtonX[1], playButtonY[1]);
  }
  else
  {
    playButtonX[1] = 1000000;
    playButtonY[1] = 1000000;
  }

  // ****************************************************************
  // ****************************************************************

  index = SPECTROGRAM_ROW_2;
  if ((rowH[index] > 0) && (rowY[index] < windowHeight) && (rowY[index] + rowH[index] >= 0))
  {
    // Paint the extra spectrogram.
    spectrogramPlot->drawSpectrogram(dc, LEFT_MARGIN, rowY[index], windowWidth-LEFT_MARGIN, rowH[index], 
      data->track[Data::EXTRA_TRACK], firstSample, numSamples);
    dc.DrawText("Extra spectro.", 5, rowY[index]);

    if (data->showF0)
    {
      // Paint the F0 curve for the extra track.
      spectrogramPlot->drawCurve(dc, LEFT_MARGIN, rowY[index], windowWidth-LEFT_MARGIN, rowH[index], 
        data->f0Signal[Data::EXTRA_TRACK], data->f0TimeStep_s, startTime_s, duration_s,
        0.0, 600.0, extraF0Color, false);
    }
  }

  // ****************************************************************
  // Draw black lines separating the rows.
  // ****************************************************************

  dc.SetPen(wxPen(*wxBLACK, lineWidth));

  for (i=0; i < NUM_ROWS; i++)
  {
    y = rowY[i] - 1;
    dc.DrawLine(LEFT_MARGIN, y, windowWidth-1, y);
  }

  // Draw one vertical line at the left end and right end.
  dc.DrawLine(LEFT_MARGIN, 0, LEFT_MARGIN, windowHeight-1);
  dc.DrawLine(windowWidth-1, 0, windowWidth-1, windowHeight-1);

    
  // ****************************************************************
  // Draw the selected region.
  // ****************************************************************

  int graphX, graphY, graphW, graphH;
  data->gsTimeAxisGraph->getDimensions(graphX, graphY, graphW, graphH);

  int x1 = data->gsTimeAxisGraph->getXPos(data->selectionMark_pt[0] / (double)SAMPLING_RATE);
  int x2 = data->gsTimeAxisGraph->getXPos(data->selectionMark_pt[1] / (double)SAMPLING_RATE);
  int y1 = rowY[OSCILLOGRAM_ROW_1];
  int y2 = rowY[SPECTROGRAM_ROW_2] + rowH[SPECTROGRAM_ROW_2];
  int h = y2 - y1;

  if ((data->isValidSelection()) && (x1 < graphX + graphW) && (x2 >= graphX))
  {
    if (x1 <= graphX)
    {
      x1 = graphX + 1;
    }
    if (x2 >= graphX + graphW)
    {
      x2 = graphX + graphW - 1;
    }

    dc.SetPen(wxPen(*wxBLACK, 0, wxPENSTYLE_TRANSPARENT));    // no pen
    dc.SetBrush(wxBrush(wxColor(220, 220, 220), wxBRUSHSTYLE_CROSSDIAG_HATCH));
    dc.DrawRectangle(x1, y1, x2 - x1, h / 10);
    dc.DrawRectangle(x1, y2 - h / 10, x2 - x1, h / 10);
  }

  // Draw the two selection marks (no matter whether the selection is valid)

  x1 = data->gsTimeAxisGraph->getXPos(data->selectionMark_pt[0] / (double)SAMPLING_RATE);
  x2 = data->gsTimeAxisGraph->getXPos(data->selectionMark_pt[1] / (double)SAMPLING_RATE);

  if ((data->selectionMark_pt[0] >= 0) && (x1 >= graphX) && (x1 < graphX + graphW))
  {
    drawSelectionMark(dc, x1, y1, y2 - 1, true);
  }

  if ((data->selectionMark_pt[1] >= 0) && (x2 >= graphX) && (x2 < graphX + graphW))
  {
    drawSelectionMark(dc, x2, y1, y2 - 1, false);
  }

    
  // ****************************************************************
  // Draw the other marks.
  // ****************************************************************
  
  // Draw the reference mark.

  x = data->gsTimeAxisGraph->getXPos( data->gesturalScoreRefMark_s );
  if ((x >= graphX) && (x < graphX + graphW))
  {
    dc.SetPen(wxPen(*wxRED, lineWidth, wxPENSTYLE_LONG_DASH));
    dc.DrawLine(x, 0, x, windowHeight-1);
  }

  // Draw the normal mark.

  x = data->gsTimeAxisGraph->getXPos( data->gesturalScoreMark_s );
  if ((x >= graphX) && (x < graphX + graphW))
  {
    dc.SetPen(wxPen(*wxRED, lineWidth));
    dc.DrawLine(x, 0, x, windowHeight-1);

    if (data->gesturalScoreRefMark_s >= 0.0)
    {
      int y = rowY[SEGMENTATION_ROW_2] + rowH[SEGMENTATION_ROW_2];
      double diff_ms = fabs(data->gesturalScoreRefMark_s - data->gesturalScoreMark_s) * 1000.0;
      wxString st = wxString::Format("Diff=%d ms", (int)diff_ms);

      dc.SetBackgroundMode(wxSOLID);
      dc.SetTextForeground(*wxBLACK);
      dc.SetTextBackground(*wxWHITE);
      dc.DrawText(st, x, y);
      dc.SetBackgroundMode(wxTRANSPARENT);
    }
  }

  // ****************************************************************
  // Draw the border lines of the selected phoneme.
  // ****************************************************************

  if ((data->selectedSegmentIndex >= 0) && (data->selectedSegmentIndex < data->segmentSequence->numSegments()))
  {
    double t0_s = data->segmentSequence->getSegmentBegin_s( data->selectedSegmentIndex );
    double t1_s = data->segmentSequence->getSegmentEnd_s( data->selectedSegmentIndex );

    dc.SetPen(wxPen(*wxBLACK, lineWidth, wxPENSTYLE_LONG_DASH));
    x = data->gsTimeAxisGraph->getXPos(t0_s);
    if ((x >= graphX) && (x < graphX + graphW))
    {
      dc.DrawLine(x, rowY[OSCILLOGRAM_ROW_1], x, windowHeight-1);
    }

    x = data->gsTimeAxisGraph->getXPos(t1_s);
    if ((x >= graphX) && (x < graphX + graphW))
    {
      dc.DrawLine(x, rowY[OSCILLOGRAM_ROW_1], x, windowHeight-1);
    }
  }

}


// ****************************************************************************
/// Paint the oscillogram of the signal s in the given part of the device
/// context.
// ****************************************************************************

void SignalComparisonPicture::paintOscillogram(wxDC &dc, int areaX, int areaY, 
       int areaWidth, int areaHeight, Signal16 *s, int firstSample, int numSamples)
{
  if ((areaWidth < 1) || (areaHeight <1))
  {
    return;
  }

  // ****************************************************************
  // Paint the signals
  // ****************************************************************

  int i, k, x;
  int min, max;
  int yMin, yMax;
  int leftIndex;
  int rightIndex;
  int y;
  double yFactor = 2.3*areaHeight / 65536.0;

  dc.SetPen(wxPen(*wxBLACK, lineWidth));

  rightIndex = firstSample;

  for (i=0; i < areaWidth; i++)
  {
    leftIndex = rightIndex;
    rightIndex = firstSample + (i+1)*numSamples / areaWidth;
    min = max = s->getValue(leftIndex);
    for (k=leftIndex+1; k <= rightIndex; k++)
    {
      y = s->getValue(k);
      if (y < min)
      {
        min = y;
      }
      if (y > max)
      {
        max = y;
      }
    }

    yMin = areaY + areaHeight / 2 - (int)(max * yFactor);
    yMax = areaY + areaHeight / 2 - (int)(min * yFactor);

    x = areaX + i;

    if (yMin > yMax)
    {
      int dummy = yMin;
      yMin = yMax;
      yMax = dummy;
    }

    if ((yMax >= areaY) && (yMin < areaY + areaHeight))
    {
      if (yMin < areaY)
      {
        yMin = areaY;
      }
      if (yMax >= areaY + areaHeight)
      {
        yMax = areaY + areaHeight - 1;
      }

      dc.DrawLine(x, yMin, x, yMax + 1);
    }
  }

}

// ****************************************************************************
/// Draw the segmentation of phones and syllables.
// ****************************************************************************

void SignalComparisonPicture::paintPhoneSegmentation(wxDC &dc, int areaX, int areaY, 
  int areaWidth, int areaHeight, double areaStartTime_s, double areaDuration_s)
{
  if ((areaWidth < 1) || (areaHeight < 1))
  {
    return;
  }

  const double EPSILON = 0.000001;
  SegmentSequence *sequence = data->segmentSequence;
  int numSegments = sequence->numSegments();
  Segment *s = NULL;
  double startTime_s = 0.0;
  double endTime_s = 0.0;
  int x1, x2;

  if (areaDuration_s < EPSILON)
  {
    areaDuration_s = EPSILON;
  }

  // ****************************************************************
  // Run through all syllables and toggle the background color from
  // one syllable to the next.
  // ****************************************************************

  dc.SetPen(wxPen(*wxBLACK, 0, wxPENSTYLE_TRANSPARENT));

  int counter = 0;
  sequence->resetIteration();
  while ((s = sequence->getNextSyllable(startTime_s, endTime_s)) != NULL)
  {
    x1 = areaX + areaWidth*(startTime_s - areaStartTime_s) / areaDuration_s;
    x2 = areaX + areaWidth*(endTime_s - areaStartTime_s) / areaDuration_s;

    if ((x1 < areaX + areaWidth) && (x2 >= areaX))
    {
      if (x1 < areaX)
      {
        x1 = areaX;
      }
      if (x2 >= areaX + areaWidth)
      {
        x2 = areaX + areaWidth - 1;
      }

      // Toggle the background color.
      if ((counter % 2) == 0)
      {
        dc.SetBrush(wxColor(220, 220, 220));
      }
      else
      {
        dc.SetBrush(*wxWHITE_BRUSH);
      }
      dc.DrawRectangle(x1, areaY, x2 - x1 + 1, areaHeight);
    }

    counter++;
  }

  // ****************************************************************
  // Run through all phones.
  // ****************************************************************

  dc.SetPen(wxPen(*wxBLACK, lineWidth));
  dc.SetFont(wxFont(9, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

  wxString st;
  int textX, textY, textWidth, textHeight;
  bool drawText;
  counter = 0;

  sequence->resetIteration();
  while ((s = sequence->getNextPhone(startTime_s, endTime_s)) != NULL)
  {
    x1 = areaX + areaWidth*(startTime_s - areaStartTime_s) / areaDuration_s;
    x2 = areaX + areaWidth*(endTime_s - areaStartTime_s) / areaDuration_s;

    // **************************************************************
    // Paint the selected phone yellow.
    // **************************************************************

    if (counter == data->selectedSegmentIndex)
    {
      if ((x1 < areaX + areaWidth) && (x2 >= areaX))
      {
        if (x1 < areaX)
        {
          x1 = areaX;
        }
        if (x2 >= areaX + areaWidth)
        {
          x2 = areaX + areaWidth - 1;
        }

        dc.SetPen(wxPen(*wxBLACK, 0, wxPENSTYLE_TRANSPARENT));
        dc.SetBrush( wxColor(255, 215, 0) );
        dc.DrawRectangle(x1, areaY, x2 - x1 + 1, areaHeight);
        dc.SetPen(wxPen(*wxBLACK, lineWidth));
      }
    }
    
    // **************************************************************
    // Draw the left and right border line.
    // **************************************************************

    if ((x1 < areaX + areaWidth) && (x1 >= areaX))
    {
      dc.DrawLine(x1, areaY, x1, areaY + areaHeight);
    }
    if ((x2 < areaX + areaWidth) && (x2 >= areaX))
    {
      dc.DrawLine(x2, areaY, x2, areaY + areaHeight);
    }

    // **************************************************************
    // Draw the phone name.
    // **************************************************************

    if ((x1 < areaX + areaWidth) && (x2 >= areaX))
    {
      st = wxString( s->getValue("name") );
      dc.GetTextExtent(st, &textWidth, &textHeight);
      textY = areaY + (areaHeight - textHeight) / 2;
      textX = x1 + ((x2-x1)-textWidth) / 2;
      drawText = true;

      if (textX < areaX)
      {
        if (x2 - areaX >= textWidth)
        {
          textX = areaX;
        }
        else
        {
          drawText = false;
        }
      }
      else
      if (textX + textWidth >= areaX + areaWidth)
      {
        textX = areaX + areaWidth - 1 - textWidth;
        if (textX < x1)
        {
          textX = x1;
        }
      }

      if (drawText)
      {
        dc.DrawText(st, textX, textY);
      }
    }

    // Increase the phone counter.

    counter++;
  }

}


// ****************************************************************************
/// Draw the segmentation of words and phrases.
// ****************************************************************************

void SignalComparisonPicture::paintWordSegmentation(wxDC &dc, int areaX, int areaY, 
  int areaWidth, int areaHeight, double areaStartTime_s, double areaDuration_s)
{
  if ((areaWidth < 1) || (areaHeight < 1))
  {
    return;
  }

  const double EPSILON = 0.000001;
  SegmentSequence *sequence = data->segmentSequence;
  int numSegments = sequence->numSegments();
  Segment *s = NULL;
  double startTime_s = 0.0;
  double endTime_s = 0.0;
  int x1, x2;

  if (areaDuration_s < EPSILON)
  {
    areaDuration_s = EPSILON;
  }

  // ****************************************************************
  // Run through all phrases and toggle the background color from
  // one phrase to the next.
  // ****************************************************************

  dc.SetPen(wxPen(*wxBLACK, 0, wxPENSTYLE_TRANSPARENT));

  int counter = 0;
  sequence->resetIteration();
  while ((s = sequence->getNextPhrase(startTime_s, endTime_s)) != NULL)
  {
    x1 = areaX + areaWidth*(startTime_s - areaStartTime_s) / areaDuration_s;
    x2 = areaX + areaWidth*(endTime_s - areaStartTime_s) / areaDuration_s;

    if ((x1 < areaX + areaWidth) && (x2 >= areaX))
    {
      if (x1 < areaX)
      {
        x1 = areaX;
      }
      if (x2 >= areaX + areaWidth)
      {
        x2 = areaX + areaWidth - 1;
      }

      // Toggle the background color.
      if ((counter % 2) == 0)
      {
        dc.SetBrush(wxColor(220, 220, 220));
      }
      else
      {
        dc.SetBrush(*wxWHITE_BRUSH);
      }

      dc.DrawRectangle(x1, areaY, x2 - x1 + 1, areaHeight);
    }

    counter++;
  }

  // ****************************************************************
  // Run through all words.
  // ****************************************************************

  dc.SetPen(wxPen(*wxBLACK, lineWidth));
  dc.SetFont(wxFont(9, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

  wxString st;
  int textX, textY, textWidth, textHeight;
  bool drawText;

  sequence->resetIteration();
  while ((s = sequence->getNextWord(startTime_s, endTime_s)) != NULL)
  {
    x1 = areaX + areaWidth*(startTime_s - areaStartTime_s) / areaDuration_s;
    x2 = areaX + areaWidth*(endTime_s - areaStartTime_s) / areaDuration_s;

    if ((x1 < areaX + areaWidth) && (x1 >= areaX))
    {
      dc.DrawLine(x1, areaY, x1, areaY + areaHeight);
    }
    if ((x2 < areaX + areaWidth) && (x2 >= areaX))
    {
      dc.DrawLine(x2, areaY, x2, areaY + areaHeight);
    }

    // **************************************************************
    // Draw the word in orthographic form.
    // **************************************************************

    if ((x1 < areaX + areaWidth) && (x2 >= areaX))
    {
      st = wxString( s->getValue("word_orthographic") );
      dc.GetTextExtent(st, &textWidth, &textHeight);
      textY = areaY + (areaHeight - textHeight) / 2;
      textX = x1 + ((x2-x1)-textWidth) / 2;
      drawText = true;

      if (textX < areaX)
      {
        if (x2 - areaX >= textWidth)
        {
          textX = areaX;
        }
        else
        {
          drawText = false;
        }
      }
      else
      if (textX + textWidth >= areaX + areaWidth)
      {
        textX = areaX + areaWidth - 1 - textWidth;
        if (textX < x1)
        {
          textX = x1;
        }
      }

      if (drawText)
      {
        dc.DrawText(st, textX, textY);
      }
    }

  }

}


// ****************************************************************************
/// Paint a play icon.
// ****************************************************************************

void SignalComparisonPicture::paintPlayButton(wxDC &dc, int x, int y)
{
  dc.SetBrush( wxColor(250, 250, 250) );
  dc.SetPen(wxPen(*wxBLACK, lineWidth));
  const auto buttonWidth = this->FromDIP(PLAY_BUTTON_WIDTH);
  const auto buttonHeight = this->FromDIP(PLAY_BUTTON_HEIGHT);
  dc.DrawRectangle(x, y, buttonWidth, buttonHeight);

  wxPoint points[3] =
  {
    wxPoint(0.15*buttonWidth, 0.15*buttonHeight),
    wxPoint(0.15*buttonWidth, 0.85*buttonHeight),
    wxPoint(0.85*buttonWidth, 0.5*buttonHeight)
  };

  dc.SetBrush( wxColor(210, 210, 210) );
  dc.SetPen(wxPen(*wxBLACK, lineWidth));

  dc.DrawPolygon(3, points, x, y);

  dc.SetPen(wxPen(*wxBLACK, lineWidth));
}


// ****************************************************************************
/// Draws a mark at the left or right margin of the selected region.
// ****************************************************************************

void SignalComparisonPicture::drawSelectionMark(wxDC &dc, int x, int y1, int y2, bool isLeftMark)
{
  wxPen oldPen = dc.GetPen();

  dc.SetPen(wxPen(*wxBLACK, lineWidth, wxPENSTYLE_LONG_DASH));
  dc.DrawLine(x, y1, x, y2);

  dc.SetPen(wxPen(*wxBLACK, lineWidth));
  dc.SetBrush(*wxBLACK_BRUSH);
  const int W = 8;

  if (isLeftMark)
  {
    wxPoint topLeftTriangle[3] = { wxPoint(x, y1), wxPoint(x + W, y1), wxPoint(x, y1 + W) };
    wxPoint bottomLeftTriangle[3] = { wxPoint(x, y2), wxPoint(x + W, y2), wxPoint(x, y2 - W) };
    dc.DrawPolygon(3, topLeftTriangle);
    dc.DrawPolygon(3, bottomLeftTriangle);
  }
  else
  {
    wxPoint topRightTriangle[3] = { wxPoint(x, y1), wxPoint(x - W, y1), wxPoint(x, y1 + W) };
    wxPoint bottomRightTriangle[3] = { wxPoint(x, y2), wxPoint(x - W, y2), wxPoint(x, y2 - W) };
    dc.DrawPolygon(3, topRightTriangle);
    dc.DrawPolygon(3, bottomRightTriangle);
  }

  dc.SetPen(oldPen);
}


// ****************************************************************************
/// Calculate the height and y-position of every row.
// ****************************************************************************

void SignalComparisonPicture::calcRowCoord()
{
  const int GAP_SIZE = 1;
  bool showRow[NUM_ROWS] = { true, true, true, true, true, true };

  if (showSegmentation == false)
  {
    showRow[SEGMENTATION_ROW_1] = false;
    showRow[SEGMENTATION_ROW_2] = false;
  }
  if (showExtraTrack == false)
  {
    showRow[OSCILLOGRAM_ROW_2] = false;
    showRow[SPECTROGRAM_ROW_2] = false;
  }
  if (showSonagrams == false)
  {
    showRow[SPECTROGRAM_ROW_1] = false;
    showRow[SPECTROGRAM_ROW_2] = false;
  }

  // ****************************************************************
  // Set the minimum height for all rows.
  // ****************************************************************

  int i;
  for (i=0; i < NUM_ROWS; i++)
  {
    rowH[i] = 0;
  }

  if (showRow[SEGMENTATION_ROW_1])
  {
    rowH[SEGMENTATION_ROW_1] = 20;
  }
  if (showRow[SEGMENTATION_ROW_2])
  {
    rowH[SEGMENTATION_ROW_2] = 20;
  }
  
  if (showRow[OSCILLOGRAM_ROW_1])
  {
    rowH[OSCILLOGRAM_ROW_1] = 40;
  }
  if (showRow[SPECTROGRAM_ROW_1])
  {
    rowH[SPECTROGRAM_ROW_1] = 60;
  }
  
  if (showRow[OSCILLOGRAM_ROW_2])
  {
    rowH[OSCILLOGRAM_ROW_2] = 40;
  }
  if (showRow[SPECTROGRAM_ROW_2])
  {
    rowH[SPECTROGRAM_ROW_2] = 60;
  }

  // ****************************************************************
  // Distribute the remaining height of the window, if there is some.
  // ****************************************************************

  int minNeededHeight = 0;

  for (i=0; i < NUM_ROWS; i++)
  {
    if (rowH[i] > 0)
    {
      minNeededHeight+= rowH[i] + GAP_SIZE;
    }
  }

  int windowHeight = getWindowHeight();
  if (windowHeight > minNeededHeight)
  {
    int constHeight = rowH[SEGMENTATION_ROW_1] + rowH[SEGMENTATION_ROW_2];
    double factor = (double)(windowHeight - constHeight) / (double)(minNeededHeight - constHeight);
    if (rowH[OSCILLOGRAM_ROW_1] > 0) { rowH[OSCILLOGRAM_ROW_1] = (int)(factor*rowH[OSCILLOGRAM_ROW_1]); }
    if (rowH[SPECTROGRAM_ROW_1] > 0) { rowH[SPECTROGRAM_ROW_1] = (int)(factor*rowH[SPECTROGRAM_ROW_1]); }
    if (rowH[OSCILLOGRAM_ROW_2] > 0) { rowH[OSCILLOGRAM_ROW_2] = (int)(factor*rowH[OSCILLOGRAM_ROW_2]); }
    if (rowH[SPECTROGRAM_ROW_2] > 0) { rowH[SPECTROGRAM_ROW_2] = (int)(factor*rowH[SPECTROGRAM_ROW_2]); }
  }

  // ****************************************************************
  // Determine the row y-positions.
  // ****************************************************************

  int y = GAP_SIZE;
  for (i=0; i < NUM_ROWS; i++)
  {
    rowY[i] = y;
    if (rowH[i] > 0)
    {
      y+= rowH[i] + GAP_SIZE;
    }
  }

}


// ****************************************************************************
/// Post an event to the parent page to update its widgets.
// ****************************************************************************

void SignalComparisonPicture::updatePage()
{
  wxCommandEvent event(updateRequestEvent);
  event.SetInt(UPDATE_PICTURES);
  wxPostEvent(updateParent, event);

  // Important for good usability and to prevent delays of reactions 
  // on the screen to user input  with the mouse.
  //wxYield();
}


// ****************************************************************************
// Update the annotation dialog.
// ****************************************************************************

void SignalComparisonPicture::updateAnnotationDialog()
{
  // Update the key-value pairs in the annotation dialog.
  AnnotationDialog *dialog = AnnotationDialog::getInstance(updateParent);
  if (dialog->IsShown())
  {
    dialog->updateWidgets();
  }
}


// ****************************************************************************
/// Checks if a segment interval or border between two segments is at the 
/// mouse position (mx, my).
// ****************************************************************************

void SignalComparisonPicture::getUnderlyingSegment(int mx, int my, int &segmentIndex, bool &isOnBorder)
{
  segmentIndex = -1;
  isOnBorder = false;

  if ((my >= rowY[SEGMENTATION_ROW_2]) && (my < rowY[SEGMENTATION_ROW_2] + rowH[SEGMENTATION_ROW_2]))
  {
    int graphX, graphY, graphW, graphH;
    data->gsTimeAxisGraph->getDimensions(graphX, graphY, graphW, graphH);

    int i = 0;
    SegmentSequence *sequence = data->segmentSequence;
    double startTime_s, endTime_s;
    int leftX, rightX;

    sequence->resetIteration();

    while (sequence->getNextPhone(startTime_s, endTime_s) != NULL)
    {
      leftX = data->gsTimeAxisGraph->getXPos(startTime_s);
      rightX = data->gsTimeAxisGraph->getXPos(endTime_s);

      // Is it a border ?
      if ((mx > rightX-4) && (mx < rightX+4))
      {
        segmentIndex = i;
        isOnBorder = true;
        break;
      }

      if ((mx >= leftX) && (mx <= rightX))
      {
        segmentIndex = i;
        break;
      }

      i++;
    }
  }

}

// ****************************************************************************
/// React on all mouse events.
// ****************************************************************************

void SignalComparisonPicture::OnMouseEvent(wxMouseEvent &event)
{
  int mx = event.GetX();
  int my = event.GetY();

  int graphX, graphY, graphW, graphH;
  data->gsTimeAxisGraph->getDimensions(graphX, graphY, graphW, graphH);
  if (graphW < 1)
  {
    graphW = 1;
  }

  // ****************************************************************
  // Show a tooltip with the time and frequency in the spectrogram
  // parts of the picture.
  // ****************************************************************

  if ((event.Dragging()) || (event.Moving()))
  {
    if (((my >= rowY[SPECTROGRAM_ROW_1]) && (my < rowY[SPECTROGRAM_ROW_1] + rowH[SPECTROGRAM_ROW_1])) ||
      ((my >= rowY[SPECTROGRAM_ROW_2]) && (my < rowY[SPECTROGRAM_ROW_2] + rowH[SPECTROGRAM_ROW_2])))
    {
      double t_s = data->gsTimeAxisGraph->getAbsXValue(mx);
      double f_Hz = 0.0;
      double y, h;

      if ((my >= rowY[SPECTROGRAM_ROW_1]) && (my < rowY[SPECTROGRAM_ROW_1] + rowH[SPECTROGRAM_ROW_1]))
      {
        y = rowY[SPECTROGRAM_ROW_1];
        h = rowH[SPECTROGRAM_ROW_1];
      }
      else
      {
        y = rowY[SPECTROGRAM_ROW_2];
        h = rowH[SPECTROGRAM_ROW_2];
      }

      if (h < 1.0)
      {
        h = 1.0;
      }
      f_Hz = (y + h - 1.0 - my)*spectrogramPlot->viewRange_Hz / h;

      wxString st = wxString::Format("%2.3f s, %d Hz", t_s, (int)f_Hz);
      this->SetToolTip(st);
    }
    else
    {
      this->SetToolTip("");
    }
  }

  // ****************************************************************
  // The mouse is entering the window.
  // ****************************************************************

  if (event.Entering())
  {
    // Automatically set the focus to this window, so that
    // mouse wheel events are properly received !!
    this->SetFocus();
    lastMx = mx;
    lastMy = my;
    return;
  }
  
  // ****************************************************************
  // The wheel was rotated.
  // ****************************************************************

  if (event.GetWheelRotation() != 0)
  {
    if (event.GetWheelRotation() < 0)
    {
      verticalOffset_percent+= 20.0;
      if (verticalOffset_percent > 100.0)
      {
        verticalOffset_percent = 100.0;
      }
    }
    else
    if (event.GetWheelRotation() > 0)
    {
      verticalOffset_percent-= 20.0;
      if (verticalOffset_percent < 0.0)
      {
        verticalOffset_percent = 0.0;
      }
    }

    updatePage();
    lastMx = mx;
    lastMy = my;
    return;
  }

  // ****************************************************************
  // Check if there is a segment interval or a segment border under 
  // the mouse coursor.
  // ****************************************************************

  int underlyingSegmentIndex;
  bool isOnBorder;

  getUnderlyingSegment(mx, my, underlyingSegmentIndex, isOnBorder);

  if (((isOnBorder) && (event.Dragging() == false)) || (moveBorder))
  {
    this->SetCursor( wxCursor(wxCURSOR_SIZEWE) );
  }
  else
  {
    this->SetCursor( wxCursor(wxCURSOR_ARROW) );
  }

  // ****************************************************************
  // Stop dragging a border when a button is released or the area
  // is left.
  // ****************************************************************

  if ((event.ButtonUp()) || (event.Leaving()))
  {
    moveBorder = false;
    lastMx = mx;
    lastMy = my;
    return;
  }

  // ****************************************************************
  // The left mouse button just changed to down.
  // ****************************************************************

  if (event.ButtonDown(wxMOUSE_BTN_LEFT))
  {
    // **************************************************************
    // Was one of the play buttons pressed?
    // **************************************************************

    int i = 0;
    if ((mx >= playButtonX[i]) && (mx < playButtonX[i] + PLAY_BUTTON_WIDTH) &&
        (my >= playButtonY[i]) && (my < playButtonY[i] + PLAY_BUTTON_HEIGHT))
    {
      if (waveStartPlaying(data->track[Data::MAIN_TRACK]->x, data->track[Data::MAIN_TRACK]->N, true))
      {
        SilentMessageBox dialog("Press OK to stop playing!", "Stop playing");
        dialog.ShowModal();
        waveStopPlaying();
      }
      else
      {
        wxMessageBox("Playing failed.", "Attention!", wxOK, this);
      }
    }

    i = 1;
    if ((mx >= playButtonX[i]) && (mx < playButtonX[i] + PLAY_BUTTON_WIDTH) &&
        (my >= playButtonY[i]) && (my < playButtonY[i] + PLAY_BUTTON_HEIGHT))
    {
      if (waveStartPlaying(data->track[Data::EXTRA_TRACK]->x, data->track[Data::EXTRA_TRACK]->N, true))
      {
        SilentMessageBox dialog("Press OK to stop playing!", "Stop playing");
        dialog.ShowModal();
        waveStopPlaying();
      }
      else
      {
        wxMessageBox("Playing failed.", "Attention!", wxOK, this);
      }
    }

    // **************************************************************
    // Selection of a segment (phone) or setting of the mark.
    // **************************************************************

    if ((mx >= graphX) && (mx < graphX + graphW))
    {
      // A new segment was selected.
      if (underlyingSegmentIndex != -1)
      {
        data->selectedSegmentIndex = underlyingSegmentIndex;
        updateAnnotationDialog();
        // Start moving a border.
        if (isOnBorder)
        {
          moveBorder = true;
        }
      }
      else
      // Set the mark to the position of the mouse.
      {
        data->gesturalScoreMark_s = data->gsTimeAxisGraph->getAbsXValue(mx);
        data->updateModelsFromGesturalScore();
        
        // Update the numeric values in the analysis results dialog.
        AnalysisResultsDialog *dialog = AnalysisResultsDialog::getInstance();
        if (dialog->IsShown())
        {
          dialog->updateWidgets();
        }
      }

      updatePage();
    }

    lastMx = mx;
    lastMy = my;
    return;
  }

  // ****************************************************************
  // Was the context menu called with the right mouse button?
  // ****************************************************************

  if (event.ButtonDown(wxMOUSE_BTN_RIGHT))
  {
    if ((mx >= graphX) && (mx < graphX + graphW))
    {
      // Call the context menu for the upper part with the segment sequence.

      if ((my >= rowY[SEGMENTATION_ROW_2]) && 
          (my < rowY[SEGMENTATION_ROW_2] + rowH[SEGMENTATION_ROW_2]))
      {
        if (underlyingSegmentIndex != data->selectedSegmentIndex)
        {
          data->selectedSegmentIndex = underlyingSegmentIndex;
          updateAnnotationDialog();
          updatePage();
        }

        menuX = mx;
        menuY = my;
	      PopupMenu(contextMenu);
      }
      else

      // Call the context menu for the lower part with the spectrogram.

      if (my > rowY[SEGMENTATION_ROW_2] + rowH[SEGMENTATION_ROW_2])
      {
        menuX = mx;
        menuY = my;
	      PopupMenu(contextMenuSpectrogram);
      }
    }

    return;
  }

  // ****************************************************************
  // A double click.
  // ****************************************************************

  if (event.ButtonDClick())
  {
    // Open the annotation dialog.
    if ((mx >= graphX) && (mx < graphX + graphW) && (underlyingSegmentIndex != -1))
    {
      data->selectedSegmentIndex = underlyingSegmentIndex;
      AnnotationDialog *dialog = AnnotationDialog::getInstance(updateParent);
      dialog->SetParent(this);
      dialog->Show(true);

      // Set the "name" field for writing, so that the user can 
      // directly type the new name (SAMPA symbol).

      dialog->grid->GoToCell(0, 1);
      dialog->grid->ShowCellEditControl();
      dialog->grid->EnableCellEditControl();
    }
    return;
  }

  // ****************************************************************
  // The user is dragging the mouse.
  // ****************************************************************

  if (event.Dragging())
  {
    if ((event.LeftIsDown()) && (mx >= graphX) && (mx < graphX + graphW))
    {
      if ((moveBorder) && (data->selectedSegmentIndex != -1))
      {
        double delta_s = 
          data->gsTimeAxisGraph->getAbsXValue(mx) - 
          data->gsTimeAxisGraph->getAbsXValue(lastMx);

        Segment* segment = data->segmentSequence->getSegment(data->selectedSegmentIndex);
        Segment* nextSegment = data->segmentSequence->getSegment(data->selectedSegmentIndex + 1);

        // When SHIFT is pressed, the selected border is moved
        // independently from all other borders.

        if ((event.ShiftDown()) && (nextSegment != NULL))
        {
          if ((segment->duration_s + delta_s >= 0.005) &&
            (nextSegment->duration_s - delta_s >= 0.005))
          {
            segment->duration_s += delta_s;
            nextSegment->duration_s -= delta_s;
          }
          else
          {
            moveBorder = false;
          }
        }
        else

        // Move this and all following segment boundaries by the same 
        // amount.
        {
          segment->duration_s += delta_s;
          if (segment->duration_s < 0.005)
          {
            segment->duration_s = 0.005;
            moveBorder = false;
          }
        }

        updateAnnotationDialog();
      }
      else
      {
        // A new segment was selected.
        if (underlyingSegmentIndex != -1)
        {
          data->selectedSegmentIndex = underlyingSegmentIndex;
          updateAnnotationDialog();
        }
        else
        // Set the mark to the position of the mouse.
        {
          data->gesturalScoreMark_s = data->gsTimeAxisGraph->getAbsXValue(mx);
          data->updateModelsFromGesturalScore();
          
          // Update the numeric values in the analysis results dialog.
          AnalysisResultsDialog *dialog = AnalysisResultsDialog::getInstance();
          if (dialog->IsShown())
          {
            dialog->updateWidgets();
          }
        }

      }

      updatePage();
    }

    lastMx = mx;
    lastMy = my;
    return;
  }

}


// ****************************************************************************
/// Play the selected segment (speech sound) in the main track.
// ****************************************************************************

void SignalComparisonPicture::OnPlaySegmentMainTrack(wxCommandEvent& event)
{
  int index = data->selectedSegmentIndex;
  if (index != -1)
  {
    int left = (int)(data->segmentSequence->getSegmentBegin_s(index) * SAMPLING_RATE);
    int right = (int)(data->segmentSequence->getSegmentEnd_s(index) * SAMPLING_RATE);

    waveStartPlaying(&data->track[Data::MAIN_TRACK]->x[left], right - left + 1, false);

    double duration_ms = 50 + (double)(right - left) * 1000.0 / SAMPLING_RATE;
    wxMilliSleep(duration_ms);

    waveStopPlaying();
  }
}


// ****************************************************************************
/// Play the selected segment (speech sound) in the extra track.
// ****************************************************************************

void SignalComparisonPicture::OnPlaySegmentExtraTrack(wxCommandEvent& event)
{
  int index = data->selectedSegmentIndex;
  if (index != -1)
  {
    int left = (int)(data->segmentSequence->getSegmentBegin_s(index) * SAMPLING_RATE);
    int right = (int)(data->segmentSequence->getSegmentEnd_s(index) * SAMPLING_RATE);

    waveStartPlaying(&data->track[Data::EXTRA_TRACK]->x[left], right - left + 1, false);

    double duration_ms = 50 + (double)(right - left) * 1000.0 / SAMPLING_RATE;
    wxMilliSleep(duration_ms);

    waveStopPlaying();
  }
}


// ****************************************************************************
/// Insert a segment.
// ****************************************************************************

void SignalComparisonPicture::OnInsertSegment(wxCommandEvent &event)
{
  SegmentSequence *sequence = data->segmentSequence;
  Graph *graph = data->gsTimeAxisGraph;
  double mousePos_s = graph->getAbsXValue(menuX);

  // The time must be positive.

  if (mousePos_s < 0.0)
  {
    return;
  }

  int segmentIndex;
  bool isOnBorder;
  getUnderlyingSegment(menuX, menuY, segmentIndex, isOnBorder);

  // ****************************************************************
  // There is an existing segment under the mouse cursor.
  // -> Insert the segment.
  // ****************************************************************

  if (segmentIndex != -1)
  {
    Segment s;
    s.duration_s = 0.05;
    sequence->insertSegment(s, segmentIndex);

    data->selectedSegmentIndex = segmentIndex;
  }
  else

  // ****************************************************************
  // The mouse cursor is behind the end of the segment sequence.
  // -> add a new segment.
  // ****************************************************************

  {
    Segment s;
    s.duration_s = mousePos_s - sequence->getDuration_s();
    sequence->appendSegment(s);
  }

  updateAnnotationDialog();
  updatePage();
}


// ****************************************************************************
/// Delete a segment.
// ****************************************************************************

void SignalComparisonPicture::OnDeleteSegment(wxCommandEvent &event)
{
  SegmentSequence *sequence = data->segmentSequence;
  int segmentIndex;
  bool isOnBorder;

  getUnderlyingSegment(menuX, menuY, segmentIndex, isOnBorder);
  if (segmentIndex != -1)
  {
    sequence->deleteSegment(segmentIndex);

    updateAnnotationDialog();
    updatePage();
  }
}


// ****************************************************************************
// ****************************************************************************

void SignalComparisonPicture::OnClearSegmentSelection(wxCommandEvent &event)
{
  data->selectedSegmentIndex = -1;
  updateAnnotationDialog();
  updatePage();
}


// ****************************************************************************
// ****************************************************************************

void SignalComparisonPicture::OnSetMinSegmentDuration(wxCommandEvent& event)
{
  int value = wxGetNumberFromUser(
    "Please enter the minimum duration for the segments in milliseconds.\n"
    "If there are segments shorter than the minimum duration, they will\n"
    "get the minimum duration, while longer segments are shortend, so\n"
    "that the total duration remains roughly constant.",
    "Min. duration in ms (10 ... 100)",
    "Set minimum duration for segments",
    40, 10, 100, this);

  if (value == -1)    // Dialog was canceled.
  {
    return;
  }

  double minSegmentDuration_s = (double)value / 1000.0;

  data->segmentSequence->setMinSegmentDuration(minSegmentDuration_s);  
  
  updateAnnotationDialog();
  updatePage();
}


// ****************************************************************************
// ****************************************************************************

void SignalComparisonPicture::OnSetReferenceMark(wxCommandEvent &event)
{
  data->gesturalScoreRefMark_s = data->gsTimeAxisGraph->getAbsXValue(menuX);
  this->Refresh();
}


// ****************************************************************************
// ****************************************************************************

void SignalComparisonPicture::OnClearReferenceMark(wxCommandEvent &event)
{
  data->gesturalScoreRefMark_s = -1.0;
  this->Refresh();
}


// ****************************************************************************
// ****************************************************************************

void SignalComparisonPicture::OnSwapTracks(wxCommandEvent &event)
{
  int i;
  signed short temp;
  Signal16 *s1 = data->track[Data::MAIN_TRACK];
  Signal16 *s2 = data->track[Data::EXTRA_TRACK];

  int N = s1->N;
  if (s2->N < N) { N = s2->N; }

  for (i=0; i < N; i++)
  {
    temp = s1->x[i];
    s1->x[i] = s2->x[i];
    s2->x[i] = temp;
  }

  // ****************************************************************
  // Exchange signals for F0...F3.
  // ****************************************************************

  vector<double> dummy;

  dummy = data->f0Signal[Data::MAIN_TRACK];
  data->f0Signal[Data::MAIN_TRACK] = data->f0Signal[Data::EXTRA_TRACK];
  data->f0Signal[Data::EXTRA_TRACK] = dummy;

  this->Refresh();
}


// ****************************************************************************
// ****************************************************************************

void SignalComparisonPicture::OnClearSelection(wxCommandEvent &event)
{
  data->selectionMark_pt[0] = -1;
  data->selectionMark_pt[1] = -1;
  this->Refresh();
}


// ****************************************************************************
// ****************************************************************************

void SignalComparisonPicture::OnSetSelectionStart(wxCommandEvent &event)
{
  data->selectionMark_pt[0] = (int)((double)SAMPLING_RATE * data->gsTimeAxisGraph->getAbsXValue(menuX));
  this->Refresh();
}


// ****************************************************************************
// ****************************************************************************

void SignalComparisonPicture::OnSetSelectionEnd(wxCommandEvent &event)
{
  data->selectionMark_pt[1] = (int)((double)SAMPLING_RATE * data->gsTimeAxisGraph->getAbsXValue(menuX));
  this->Refresh();
}

// ****************************************************************************
// ****************************************************************************

void SignalComparisonPicture::OnSelectAll(wxCommandEvent &event)
{
  data->selectionMark_pt[0] = 0;
  data->selectionMark_pt[1] = Data::TRACK_DURATION_S*SAMPLING_RATE - 1;
  this->Refresh();
}


// ****************************************************************************
// ****************************************************************************

void SignalComparisonPicture::OnSpectrogramRange4kHz(wxCommandEvent &event)
{
  spectrogramPlot->viewRange_Hz = 4000.0;
  this->Refresh();
}


// ****************************************************************************
// ****************************************************************************

void SignalComparisonPicture::OnSpectrogramRange8kHz(wxCommandEvent &event)
{
  spectrogramPlot->viewRange_Hz = 8000.0;
  this->Refresh();
}


// ****************************************************************************
// ****************************************************************************

void SignalComparisonPicture::OnSpectrogramRange12kHz(wxCommandEvent &event)
{
  spectrogramPlot->viewRange_Hz = 12000.0;
  this->Refresh();
}


// ****************************************************************************
