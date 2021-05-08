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

#include "SpectrumPicture.h"
#include "Data.h"
#include "SpectrumOptionsDialog.h"
#include "SilentMessageBox.h"

// ****************************************************************************
// The event table.
// ****************************************************************************

BEGIN_EVENT_TABLE(SpectrumPicture, BasicPicture)
  EVT_LEFT_DOWN(SpectrumPicture::OnLeftButtonDown)
  EVT_RIGHT_DOWN(SpectrumPicture::OnRightButtonDown)
END_EVENT_TABLE()



// ****************************************************************************
// Initialization of static members.
// ****************************************************************************

const double SpectrumPicture::REFERENCE_SPECTRUM_PRESSURE = 1.0;   // 1000.0 = 0.1 kPa 
const double SpectrumPicture::LOWEST_SPECTRUM_PRESSURE = 100.0*SpectrumPicture::REFERENCE_SPECTRUM_PRESSURE;
const double SpectrumPicture::HIGHEST_SPECTRUM_PRESSURE = 100000.0*SpectrumPicture::REFERENCE_SPECTRUM_PRESSURE;

const double SpectrumPicture::REFERENCE_SPECTRUM_IMPEDANCE = 40.7;     // characteristic impedance rho*c
const double SpectrumPicture::LOWEST_SPECTRUM_IMPEDANCE = 1.0*SpectrumPicture::REFERENCE_SPECTRUM_IMPEDANCE;
const double SpectrumPicture::HIGHEST_SPECTRUM_IMPEDANCE = 1000.0*SpectrumPicture::REFERENCE_SPECTRUM_IMPEDANCE;

const double SpectrumPicture::REFERENCE_SPECTRUM_VOLUMEVELOCITY = 1.0; // cm^3/s
const double SpectrumPicture::LOWEST_SPECTRUM_VOLUMEVELOCITY = 100.0;    // cm^3/s
const double SpectrumPicture::HIGHEST_SPECTRUM_VOLUMEVELOCITY = 800.0;

const double SpectrumPicture::REFERENCE_SPECTRUM_RATIO = 1.0;
const double SpectrumPicture::LOWEST_SPECTRUM_RATIO = 2.0;
const double SpectrumPicture::HIGHEST_SPECTRUM_RATIO = 30.0;


// ****************************************************************************
/// Constructor.
// ****************************************************************************

SpectrumPicture::SpectrumPicture(wxWindow *parent, 
  VocalTractPicture *picVocalTract) : BasicPicture(parent)
{
  // ****************************************************************
  // Init the variables
  // ****************************************************************

  this->picVocalTract = picVocalTract;

  showModelSpectrum = true;
  showUserSpectrum = false;
  showTdsSpectrum = false;
  showPoleZeroSpectrum = false;
  showFormants = true;
  showVttfHarmonics = false;
  showMagnitude = true;
  showPhase = false;

  modelSpectrumType = SPECTRUM_UU;
 
  levelShift = 0.0;

  spectrumFileName = "";

  // Cutoff-freq. of the first-order low-pass filter to visualize the
  // radiated sound spectrum due to a dipole noise source.
  noiseFilterCutoffFrequency = 150;

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

void SpectrumPicture::draw(wxDC &dc)
{
  const int SPECTRUM_LENGTH_EXPONENT = 12;
  const int SPECTRUM_LENGTH = 1 << SPECTRUM_LENGTH_EXPONENT;

  ComplexSignal mouthTF(0);
  ComplexSignal noseTF(0);
  ComplexSignal radiationSpectrum(0);
  Signal        pulseForm(0);
  ComplexSignal pulseSpectrum(0);

  Data *data = Data::getInstance();

  int i;
  int graphX, graphY, graphW, graphH;
  int width, height;
  double F0;
  int pulseLength;

  graph.getDimensions(graphX, graphY, graphW, graphH);
  this->GetSize(&width, &height);


  // ****************************************************************
  // Clear the background and paint the axes.
  // ****************************************************************

  dc.SetBackground(*wxWHITE_BRUSH);
  dc.Clear();
  graph.paintAbscissa(dc);
  graph.paintOrdinate(dc);

  // The scale for the phase angle **********************************

  if (showPhase)
  {
    double y[5];
    wxPen linePen(*wxLIGHT_GREY, lineWidth, wxPENSTYLE_SHORT_DASH);
    wxPen blackPen(*wxBLACK, lineWidth);
    dc.SetPen(blackPen);
    dc.DrawLine(graphX + graphW, graphY, graphX + graphW, graphY + graphH);

    for (i=0; i < 5; i++) 
    { 
      y[i] = graphY + graphH - 1 - 0.25*graphH*i; 

      dc.SetPen(blackPen);
      dc.DrawLine(graphX + graphW, (int)y[i], graphX + graphW + 6, (int)y[i]);
      
      dc.SetPen(linePen);
      dc.DrawLine(graphX, (int)y[i], graphX + graphW - 1, (int)y[i]);
    }

    dc.SetPen(blackPen);
    dc.SetFont(wxFont(9, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

    dc.DrawText("Pi",    graphX + graphW + 8, y[4] + 1);
    dc.DrawText("Pi/2",  graphX + graphW + 8, y[3]-8);
    dc.DrawText("0",     graphX + graphW + 8, y[2]-8);
    dc.DrawText("-Pi/2", graphX + graphW + 8, y[1]-8);
    dc.DrawText("-Pi",   graphX + graphW + 8, y[0]-8);

    dc.DrawText("rad",   graphX + graphW + 8, height-16);
  }


  // ****************************************************************
  // Display the selected function.
  // ****************************************************************

  // Set the latest vocal tract geometry for the transmission line model.   
  data->updateTlModelGeometry(data->vocalTract);


  double upperFrequencyLimit = graph.abscissa.positiveLimit + 100.0;

  if (showModelSpectrum)
  {
    TlModel *tlModel = data->tlModel;

    // ****************************************************************
    // Vocal tract transfer function.
    // ****************************************************************

    if ((modelSpectrumType == SPECTRUM_UU) || 
      (modelSpectrumType == SPECTRUM_PU) ||
      (modelSpectrumType == SPECTRUM_NOSE_UU))
    {
      if (modelSpectrumType == SPECTRUM_NOSE_UU)
      {
        tlModel->getSpectrum(TlModel::FLOW_SOURCE_TF, data->primarySpectrum, SPECTRUM_LENGTH, Tube::FIRST_NOSE_SECTION);
      }
      else
      {
        tlModel->getSpectrum(TlModel::FLOW_SOURCE_TF, data->primarySpectrum, SPECTRUM_LENGTH, Tube::FIRST_PHARYNX_SECTION);
      }

      if (modelSpectrumType == SPECTRUM_PU) 
      { 
        tlModel->getSpectrum(TlModel::RADIATION, &radiationSpectrum, SPECTRUM_LENGTH, 0);
        (*data->primarySpectrum)*= radiationSpectrum;
        (*data->primarySpectrum)*= 10.0;
      }

      paintContinualSpectrum(dc, data->primarySpectrum, 1.0, *wxBLACK);

      // Paint also the harmonics corresponding to the current F0 ?

      if ((modelSpectrumType == SPECTRUM_UU)  && (showVttfHarmonics))
      { 
        F0 = data->lfPulse.F0;
        pulseLength = (int)((double)SAMPLING_RATE / F0);

        pulseSpectrum.reset(pulseLength);
        double f;
        int index;
        for (i=0; i < pulseSpectrum.N; i++)
        {
          f = F0*(double)i;
          index = (int)(f / ((double)SAMPLING_RATE/(double)SPECTRUM_LENGTH));
          if (index >= SPECTRUM_LENGTH) 
          { 
            index = SPECTRUM_LENGTH-1; 
          }
          pulseSpectrum.setValue(i, data->primarySpectrum->getValue(index));
        }
        paintDiscreteSpectrum(dc, &pulseSpectrum, 1.0, *wxBLACK);
      }

      // Calculate and display the formant frequencies.

      if ((modelSpectrumType == SPECTRUM_UU) && (showFormants))
      {
        const int MAX_FORMANTS = 4;
        double formantFreq[MAX_FORMANTS];
        double formantBw[MAX_FORMANTS];
        int numFormants;
        bool frictionNoise;
        bool isClosure;
        bool isNasal;
        wxString st;
        int i;
        int x, y;

        wxPen dashedPen(*wxBLACK, lineWidth, wxPENSTYLE_LONG_DASH);
        dc.SetPen(dashedPen);
        dc.SetFont(wxFont(9, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

        tlModel->getFormants(formantFreq, formantBw, numFormants, MAX_FORMANTS, frictionNoise, isClosure, isNasal);

        for (i=0; i < numFormants; i++)
        {
          st = wxString::Format("%d Hz", (int)(formantFreq[i] + 0.5));
          x = graph.getXPos(formantFreq[i]);
          y = 5;
          dc.DrawLine(x, graphY, x, graphY + graphH - 1);
          dc.DrawText(st, x + 5, y);
        }
      }

    } 

    else

    // ****************************************************************
    // The vocal tract input impedance.
    // ****************************************************************

    if (modelSpectrumType == SPECTRUM_INPUT_IMPEDANCE) 
    { 
      tlModel->getSpectrum(TlModel::INPUT_IMPEDANCE, data->primarySpectrum, SPECTRUM_LENGTH, 
        Tube::FIRST_PHARYNX_SECTION);
       
      (*data->primarySpectrum)*= 0.1;
      paintContinualSpectrum(dc, data->primarySpectrum, 1.0, *wxBLACK);
    } 
    else

    // ****************************************************************
    // The subglottal input impedance.
    // ****************************************************************
    
    if (modelSpectrumType == SPECTRUM_SUBGLOTTAL_INPUT_IMPEDANCE) 
    { 
      tlModel->getSpectrum(TlModel::OUTPUT_IMPEDANCE, data->primarySpectrum, SPECTRUM_LENGTH, 
        Tube::LOWER_GLOTTIS_SECTION-1);

      (*data->primarySpectrum)*= 0.1;
      paintContinualSpectrum(dc, data->primarySpectrum, 1.0, *wxBLACK);
    } 
    else

    // ****************************************************************
    // Glottal volume velocity multiplied with the radiation characteristic. 
    // ****************************************************************

    if (modelSpectrumType == SPECTRUM_VOLUME_VELOCITY_IMPEDANCE) 
    { 
      F0 = data->lfPulse.F0;
      pulseLength = (int)((double)SAMPLING_RATE / F0);
  
      data->lfPulse.getPulse(pulseForm, pulseLength, false);
      data->primarySpectrum->reset(pulseLength);
      realDFT(pulseForm, *data->primarySpectrum, pulseLength, true);

      tlModel->getSpectrum(TlModel::RADIATION, &radiationSpectrum, pulseLength, 0);
      (*data->primarySpectrum)*= radiationSpectrum; 
      (*data->primarySpectrum)*= 500.0;

      paintDiscreteSpectrum(dc, data->primarySpectrum, 1.0, *wxBLACK);
    } 
    else

    // ****************************************************************
    // The radiated sound pressure due to a glottal voice source.
    // ****************************************************************

    if (modelSpectrumType == SPECTRUM_SOUND_PRESSURE_VOICE_SOURCE)
    { 
      F0 = data->lfPulse.F0;
      pulseLength = (int)((double)SAMPLING_RATE / F0);

      // The discrete spectrum.

      data->lfPulse.getPulse(pulseForm, pulseLength, false);
      data->primarySpectrum->reset(pulseLength);
      realDFT(pulseForm, pulseSpectrum, pulseLength, true);

      // Multiply with the transfer function
      tlModel->getSpectrum(TlModel::FLOW_SOURCE_TF, data->primarySpectrum, pulseLength, Tube::FIRST_PHARYNX_SECTION);
      tlModel->getSpectrum(TlModel::RADIATION, &radiationSpectrum, pulseLength, 0);

      (*data->primarySpectrum)*= radiationSpectrum; 
      (*data->primarySpectrum)*= pulseSpectrum; 
      (*data->primarySpectrum)*= 50.0;

      paintDiscreteSpectrum(dc, data->primarySpectrum, 1.0, *wxBLACK);
    } 
    else

    // ****************************************************************
    // The radiated sound pressure due to a supraglottal dipole noise 
    // source at the position of the current cut plane.
    // ****************************************************************

    if (modelSpectrumType == SPECTRUM_SOUND_PRESSURE_NOISE_SOURCE)
    {
      double noiseSourcePos_cm = picVocalTract->cutPlanePos_cm;
      data->calcRadiatedNoiseSpectrum(noiseSourcePos_cm, data->noiseFilterCutoffFreq,
        SPECTRUM_LENGTH, data->primarySpectrum);

      paintContinualSpectrum(dc, data->primarySpectrum, 1.0, *wxBLACK);
    }
    else

    // ****************************************************************
    // The radiated pressure based on the Transfer function in the 
    // pole-zero plot.
    // ****************************************************************

    if (modelSpectrumType == SPECTRUM_SOUND_PRESSURE_PZ)
    {
      F0 = data->lfPulse.F0;
      pulseLength = (int)((double)SAMPLING_RATE / F0);

      // The discrete spectrum **************************************

      data->lfPulse.getPulse(pulseForm, pulseLength, false);
      data->primarySpectrum->reset(pulseLength);
      realDFT(pulseForm, pulseSpectrum, pulseLength, true);

      // Multiply with the transfer function

      PoleZeroPlan *plan = data->poleZeroPlan;

      plan->getPoleZeroSpectrum(data->primarySpectrum, pulseLength, upperFrequencyLimit);

      if (plan->higherPoleCorrection)
      {
        ComplexSignal hpc(pulseLength);
        double effectiveLength = 17.0;	  // cm
        double mouthArea = 3.0;			      // cm^2

        effectiveLength += 0.8*sqrt(mouthArea / M_PI);

        plan->getHigherPoleCorrection(&hpc, pulseLength, effectiveLength);
        *data->primarySpectrum *= hpc;
      }

      tlModel->getSpectrum(TlModel::RADIATION, &radiationSpectrum, pulseLength, 0);

      (*data->primarySpectrum) *= radiationSpectrum;
      (*data->primarySpectrum) *= pulseSpectrum;
      (*data->primarySpectrum) *= 50.0;

      paintDiscreteSpectrum(dc, data->primarySpectrum, 1.0, *wxBLACK);
    }
    else

    // ****************************************************************
    // The glottal volume velocity.
    // ****************************************************************

    if (modelSpectrumType == SPECTRUM_VOLUME_VELOCITY) 
    { 
      F0 = data->lfPulse.F0;
      pulseLength = (int)((double)SAMPLING_RATE / F0);
  
      data->lfPulse.getPulse(pulseForm, pulseLength, false);
      data->primarySpectrum->reset(pulseLength);
      realDFT(pulseForm, *data->primarySpectrum, pulseLength, true);
      (*data->primarySpectrum)*= 0.5;

      paintDiscreteSpectrum(dc, data->primarySpectrum, 1.0, *wxBLACK);
    }

  }


  // ****************************************************************
  // Paint the transfer function from the pole-zero-plan ?
  // ****************************************************************

  if (showPoleZeroSpectrum) 
  { 
    ComplexSignal poleZeroSpectrum(SPECTRUM_LENGTH);
    PoleZeroPlan *plan = data->poleZeroPlan;

    plan->getPoleZeroSpectrum(&poleZeroSpectrum, SPECTRUM_LENGTH, upperFrequencyLimit);
    
    if (plan->higherPoleCorrection)
    {
      ComplexSignal hpc(SPECTRUM_LENGTH);
      double effectiveLength = 17.0;	  // cm
      double mouthArea = 3.0;			      // cm^2

      effectiveLength+= 0.8*sqrt(mouthArea/M_PI);

      plan->getHigherPoleCorrection(&hpc, SPECTRUM_LENGTH, effectiveLength);
      poleZeroSpectrum*= hpc;
    }

    if (modelSpectrumType == SPECTRUM_PU) 
    { 
      TlModel *tlModel = data->tlModel;
      tlModel->getSpectrum(TlModel::RADIATION, &radiationSpectrum, SPECTRUM_LENGTH, 0);
      poleZeroSpectrum*= radiationSpectrum; 
      poleZeroSpectrum*= 10.0;
    }
    paintContinualSpectrum(dc, &poleZeroSpectrum, 1.0, *wxRED);
  }

  // ****************************************************************
  // Paint the user spectrum ?
  // ****************************************************************

  if (showUserSpectrum)
  {
    double scaleFactor = exp(log(10.0)*(-levelShift/20.0));

    if ((data->userSpectrumType == Data::NORMAL_SPECTRUM) ||
        (data->userSpectrumType == Data::AVERAGE_SPECTRUM))
    {
      paintContinualSpectrum(dc, data->userSpectrum, scaleFactor, *wxRED);
    }
  }

  // ****************************************************************
  // Paint the TDS spectrum (of the impulse responses) ?
  // ****************************************************************

  if (showTdsSpectrum)
  {
    double scaleFactor = exp(log(10.0)*(-levelShift/20.0));
    paintContinualSpectrum(dc, data->tdsSpectrum, scaleFactor, *wxGREEN);
  }
}

// ****************************************************************************
// Paint a continual signal into the spectrum picture.
// ****************************************************************************

void SpectrumPicture::paintContinualSpectrum(wxDC &dc, ComplexSignal *spectrum, double ampFactor, wxColor color)
{
  const int LINE_WIDTH = lineWidth;   // = 1
  int i;
  int graphX, graphY, graphW, graphH;
  double freq;
  int h0, h1;
  double d0, d1;
  double d;
  int numHarmonics = spectrum->N/2 - 1;
  double F0 = (double)SAMPLING_RATE / (double)spectrum->N;
  int y;
  int lastY;
  wxPen magnitudePen(color, LINE_WIDTH);
  wxPen phasePen(wxColor(0, 0, 255), LINE_WIDTH);

  graph.getDimensions(graphX, graphY, graphW, graphH);

  // ****************************************************************
  // Paint the magnitude spectrum.
  // ****************************************************************

  if (showMagnitude)
  {
    dc.SetPen(magnitudePen);

    for (i=0; i < graphW; i++)
    {
      freq = graph.getAbsXValue(graphX + i);
      h0 = (int)(freq / F0);
      h1 = h0 + 1;

      // Linear interpolation between the two harmonics
      if ((h0 >= 0) && (h1 < numHarmonics))
      {
        d0 = spectrum->getMagnitude(h0);
        d1 = spectrum->getMagnitude(h1);
        d = d0 + ((d1 - d0)*(freq - h0*F0)) / F0;

        y = graph.getYPos(d*ampFactor);

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
  }

  // ****************************************************************
  // Paint the phase spectrum.
  // ****************************************************************

  if (showPhase)
  {
    double lastPhase = 0.0;
    double phase;
    int y[2];

    dc.SetPen(phasePen);

    for (i=0; i < graphW; i++)
    {
      freq = graph.getAbsXValue(graphX + i);
      h0 = (int)(freq / F0);
      h1 = h0 + 1;

      // Linear interpolation between the two harmonics
      if ((h0 >= 0) && (h1 < numHarmonics))
      {
        d0 = spectrum->getPhase(h0);
        d1 = spectrum->getPhase(h1);

        if (fabs(d0-d1) > M_PI)
        {
          if (d0 < d1) { d0+= 2.0*M_PI; } else { d0-= 2.0*M_PI; }
        }
        phase = d0 + ((d1 - d0)*(freq - h0*F0)) / F0;

        // When there is a phase jump, paint two separate strokes

        if (fabs(lastPhase - phase) > M_PI)
        {
          // Line rest in the last pixel column
  
          if (i > 0)
          {
            double merkePhase = phase;
            if (phase > lastPhase) { phase-= 2.0*M_PI; } else { phase+= 2.0*M_PI; }

            y[0] = graphY + graphH - 1 - (int)(((double)graphH*(lastPhase + M_PI))/(2.0*M_PI));
            y[1] = graphY + graphH - 1 - (int)(((double)graphH*(phase + M_PI))/(2.0*M_PI));

            if (y[0] < graphY) { y[0] = graphY; }
            if (y[1] < graphY) { y[1] = graphY; }
            if (y[0] >= graphY + graphH) { y[0] = graphY + graphH - 1; }
            if (y[1] >= graphY + graphH) { y[1] = graphY + graphH - 1; }

            dc.DrawLine(graphX+i-1, y[0], graphX+i, y[1]);

            phase = merkePhase;
          }

          if (lastPhase < phase) { lastPhase+= 2.0*M_PI; } else { lastPhase-= 2.0*M_PI; }
        }

        // New line part in the current pixel column

        y[0] = graphY + graphH - 1 - (int)(((double)graphH*(lastPhase + M_PI))/(2.0*M_PI));
        y[1] = graphY + graphH - 1 - (int)(((double)graphH*(phase + M_PI))/(2.0*M_PI));

        if (y[0] < graphY) { y[0] = graphY; }
        if (y[1] < graphY) { y[1] = graphY; }
        if (y[0] >= graphY + graphH) { y[0] = graphY + graphH - 1; }
        if (y[1] >= graphY + graphH) { y[1] = graphY + graphH - 1; }

        if (i > 0)
        { 
          dc.DrawLine(graphX+i-1, y[0], graphX+i, y[1]); 
        }

        lastPhase = phase;
      }
    }
  }

}


// ****************************************************************************
// Paint a discrete signal into the spectrum picture.
// ****************************************************************************

void SpectrumPicture::paintDiscreteSpectrum(wxDC &dc, ComplexSignal *spectrum, double ampFactor, wxColor color)
{
  const int LINE_WIDTH = lineWidth; // 3;
  int graphX, graphY, graphW, graphH;
  int x, y;
  int i;
  bool paintTop;
  wxPen magnitudePen(color, LINE_WIDTH);
  wxPen phasePen(wxColor(0, 0, 255), LINE_WIDTH);

  int numHarmonics = spectrum->N/2 - 1;
  double F0 = (double)SAMPLING_RATE / (double)spectrum->N;

  graph.getDimensions(graphX, graphY, graphW, graphH);


  // ****************************************************************
  // Paint the magnitudes of the harmonics.
  // ****************************************************************

  if (showMagnitude)
  {
    dc.SetPen(magnitudePen);

    for (i=0; i <= numHarmonics; i++)
    {
      // Determine position on the abscissa *************************
      x = graph.getXPos((double)i*F0);

      if ((x >= graphX) && (x < graphX + graphW))
      {
        // Position on the ordinate *********************************
        y = graph.getYPos(spectrum->getMagnitude(i)*ampFactor);

        if (y < graphY+graphH)
        {
          paintTop = true;
          if (y < graphY)
          {
            y = graphY;
            paintTop = false;
          }

          dc.DrawLine(x, y, x, graphY+graphH-1);
//          dc.DrawLine(x-1, y, x-1, graphY+graphH-1);

          /*
          if (paintTop) 
          { 
            dc.DrawLine(x-3, y, x+2, y); 
          }
          */
        }
      }
    }   // Run through the harmonics
  }

  // ****************************************************************
  // Paint the pases of the harmonics.
  // ****************************************************************
    
  if (showPhase)
  {
    dc.SetPen(phasePen);

    double value;

    for (i=0; i <= numHarmonics; i++)
    {
      // Position on the abscissa ***********************************
      x = graph.getXPos((double)i*F0);

      if ((x >= graphX) && (x < graphX + graphW))
      {
        // Position on the ordinate *********************************
        value = spectrum->getPhase(i);
        y = graphY + graphH - 1 - (int)(((double)graphH*(value + M_PI))/(2.0*M_PI));
        if (y < graphY) { y = graphY; }
        if (y >= graphY + graphH) { y = graphY + graphH - 1; }

        dc.DrawLine(x+1, graphY + graphH/2, x+1, y);
        dc.DrawLine(x+2, graphY + graphH/2, x+2, y);
      }
    }   // Run through the harmonics
  }

}


// ****************************************************************************
// ****************************************************************************

void SpectrumPicture::OnLeftButtonDown(wxMouseEvent &event)
{
  // Show the dialog with spectrum options

  SpectrumOptionsDialog *dialog = SpectrumOptionsDialog::getInstance(this);
  dialog->SetParent(this);
  dialog->Show();

  event.Skip();
}


// ****************************************************************************
// ****************************************************************************

void SpectrumPicture::OnRightButtonDown(wxMouseEvent &event)
{
  TlModel *model = Data::getInstance()->tlModel;

  const int MAX_FORMANTS = 10;
  double formantFreq[MAX_FORMANTS];
  double formantBw[MAX_FORMANTS];
  int numFormants;
  bool frictionNoise;
  bool isClosure;
  bool isNasal;
  int i;

  model->getFormants(formantFreq, formantBw, numFormants, MAX_FORMANTS, frictionNoise, isClosure, isNasal);

  // ****************************************************************
  // Show a message box with the information
  // ****************************************************************

  wxString st = "Formant frequency (bandwidth) in Hz\n\n";
  wxString line;

  for (i=0; i < numFormants; i++)
  {
    line = wxString::Format("#%d  %5.1f   (%2.1f) Hz\n", i+1, formantFreq[i], formantBw[i]);
    st+= line;
  }

  st+= "\n";
  st+= wxString::Format(
    "Friction noise with open glottis = %d\n"
    "Complete closure = %d\n"
    "Open nasal port = %d\n",
    frictionNoise,
    isClosure,
    isNasal
    );

//  wxMessageBox(st, "Primary spectrum information");
  SilentMessageBox dialog(st, "Primary spectrum information");
  dialog.ShowModal();

  event.Skip();
}

// ****************************************************************************

