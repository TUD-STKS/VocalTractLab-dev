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

#include <fstream>
#include "SynthesisThread.h"
#include <wx/stopwatch.h>


// ****************************************************************************
// ****************************************************************************

SynthesisThread::SynthesisThread(wxWindow *window, TubeSequence *tubeSequence) : wxThread()
{
  this->window = window;
  this->tubeSequence = tubeSequence;
  canceled = false;
}


// ****************************************************************************
// ****************************************************************************

void *SynthesisThread::Entry()
{
  Data *data = Data::getInstance();
  TdsModel *tdsModel = data->tdsModel;
  int duration = tubeSequence->getDuration_pt();
  int startPos = tubeSequence->getPos_pt();
  double timeStep_s = tdsModel->timeStep;

  int pos_percent;
  Tube tube;
  int flowSourceSection;
  int pressureSourceSection;
  double pressureSource_dPa;
  double flowSource_cm3_s;

  double totalFlow_cm3_s;
  double mouthFlow_cm3_s;
  double nostrilFlow_cm3_s;
  double skinFlow_cm3_s;

  double pressure_dPa[4];
  double inflow_cm3_s;
  double outflow_cm3_s;

  int i, k;
  
  // ****************************************************************
  // Restrict the synthesis speed.
  // ****************************************************************

  int speed_percent = data->synthesisSpeed_percent;
  if (speed_percent < 1)
  {
    speed_percent = 1;
  }
  if (speed_percent > 100)
  {
    speed_percent = 100;
  }

  // ****************************************************************
  // Shall we write out the glottis data ?
  // ****************************************************************
  
  ofstream glottisStream;
  if (data->saveGlottisSignals)
  {
    glottisStream.open(data->glottisSignalsFileName.ToStdString());
    if (!glottisStream)
    {
      wxPrintf("Error: Failed to open the file %s to write out the glottis signals!\n",
        data->glottisSignalsFileName.c_str());
    }
    else
    {
      data->getSelectedGlottis()->printParamNames( glottisStream );
    }
  }

  // ****************************************************************
  // This starts the stop watch.
  // ****************************************************************

  wxStopWatch stopWatch;    

  // ****************************************************************
  // Main loop.
  // ****************************************************************

  bool filtering;

  for (i = startPos; (i < duration) && (wasCanceled() == false); i++)
  {
    // Check if we were asked to exit
    if (TestDestroy())
    {
      break;
    }

    // **************************************************************
    // Create any type of command event here to send the position
    // in percent in a thread-safe way.
    // If no animation is shown, send an event every 200 samples to
    // stay reactive (react on Cancel-button).
    // **************************************************************

    if (((data->showAnimation) && ((i % speed_percent) == 0)) ||
        ((data->showAnimation == false) && ((i % 200) == 0)))
    {
      // Set guiUpdateFinished to false BEFORE the command event is
      // sent to the GUI
      guiUpdateFinished = false;

      pos_percent = 100*i / duration;
      wxCommandEvent event(wxEVT_COMMAND_MENU_SELECTED, SYNTHESIS_THREAD_EVENT);
      event.SetInt(pos_percent);
      wxPostEvent(window, event);

      // Wait until the GUI update in response to our event finished.
      while (guiUpdateFinished == false)
      {
        // Give the rest of the thread time slice to the system allowing the other threads to run.
        this->Yield();   
      }
    }

    // **************************************************************
    // Make a time step with the current tube geometry.
    // **************************************************************

    tubeSequence->getTube(tube);
    tubeSequence->getFlowSource(flowSource_cm3_s, flowSourceSection);
    tubeSequence->getPressureSource(pressureSource_dPa, pressureSourceSection);
    
    if (i == 0)
    {
      filtering = false;
    }
    else
    {
      filtering = true;
    }

    tdsModel->setTube(&tube, filtering);
    tdsModel->setFlowSource(flowSource_cm3_s, flowSourceSection);
    tdsModel->setPressureSource(pressureSource_dPa, pressureSourceSection);

    // Get the four relevant pressure values for the glottis model:
    // subglottal, lower glottis, upper glottis, supraglottal.

    pressure_dPa[0] = tdsModel->getSectionPressure(Tube::LAST_TRACHEA_SECTION);
    pressure_dPa[1] = tdsModel->getSectionPressure(Tube::LOWER_GLOTTIS_SECTION);
    pressure_dPa[2] = tdsModel->getSectionPressure(Tube::UPPER_GLOTTIS_SECTION);
    pressure_dPa[3] = tdsModel->getSectionPressure(Tube::FIRST_PHARYNX_SECTION);

    tubeSequence->incPos(pressure_dPa);

    totalFlow_cm3_s = tdsModel->proceedTimeStep(mouthFlow_cm3_s, nostrilFlow_cm3_s, skinFlow_cm3_s);

    // **************************************************************
    // Sample the pressure and flow values at the mouth and the probe.
    // **************************************************************

    k = i & Data::TDS_BUFFER_MASK;
    data->outputFlow[k] = totalFlow_cm3_s;
    data->outputPressure[k] = (data->outputFlow[k] - data->outputFlow[(k-1) & Data::TDS_BUFFER_MASK]) / timeStep_s;
    data->filteredOutputPressure[k] = data->outputPressureFilter.getOutputSample(data->outputPressure[k]);
    // Original scaling factor: 0.004 !
    data->track[Data::MAIN_TRACK]->setValue(i, (short)(data->filteredOutputPressure[k]*0.003));

    if ((data->userProbeSection >= 0) && (data->userProbeSection < Tube::NUM_SECTIONS))
    {
      tdsModel->getSectionFlow(data->userProbeSection, inflow_cm3_s, outflow_cm3_s);
      data->userProbeFlow[k] = inflow_cm3_s;
      data->userProbePressure[k] = tdsModel->getSectionPressure(data->userProbeSection);
      
      double area = tdsModel->tubeSection[data->userProbeSection].area;
      if (area < TdsModel::MIN_AREA_CM2)
      {
        area = TdsModel::MIN_AREA_CM2;
      }
      data->userProbeArea[k] = area;
      data->userProbeVelocity[k] = inflow_cm3_s / area;
    }

    if ((data->internalProbeSection >= 0) && (data->internalProbeSection < Tube::NUM_SECTIONS))
    {
      tdsModel->getSectionFlow(data->internalProbeSection, inflow_cm3_s, outflow_cm3_s);
      data->internalProbeFlow[k] = inflow_cm3_s;
      data->internalProbePressure[k] = tdsModel->getSectionPressure(data->internalProbeSection);
    }

    // **************************************************************
    // Output the glottis parameter in a file.
    // **************************************************************

    if ((data->saveGlottisSignals) && (glottisStream))
    {
      tdsModel->getSectionFlow(Tube::UPPER_GLOTTIS_SECTION, inflow_cm3_s, outflow_cm3_s);
      double glottisFlow_cm3_s = inflow_cm3_s;
      data->getSelectedGlottis()->printParamValues(glottisStream, glottisFlow_cm3_s,
        pressure_dPa, mouthFlow_cm3_s, nostrilFlow_cm3_s, skinFlow_cm3_s, data->filteredOutputPressure[k]);
    }
  }

  stopWatch.Pause();
  wxPrintf("The synthesis took %ld ms.\n", stopWatch.Time());


  // ****************************************************************
  // Calculate a spectrum for some kinds of synthesis.
  // ****************************************************************

  if ((data->synthesisType == Data::SYNTHESIS_TRANSFER_FUNCTION) ||
      (data->synthesisType == Data::SYNTHESIS_SUPRAGLOTTAL_INPUT_IMPEDANCE) ||
      (data->synthesisType == Data::SYNTHESIS_SUBGLOTTAL_INPUT_IMPEDANCE))
  {
    const int IMPULSE_RESPONSE_LENGTH = Data::TDS_BUFFER_LENGTH;
    const int IMPULSE_RESPONSE_EXPONENT = Data::TDS_BUFFER_EXPONENT;
    ComplexSignal impulseResponse(IMPULSE_RESPONSE_LENGTH);
    double factor = 1.0;

    if (data->synthesisType == Data::SYNTHESIS_TRANSFER_FUNCTION)
    {
      for (i=0; i < IMPULSE_RESPONSE_LENGTH; i++)
      {
        impulseResponse.setValue(i, data->outputFlow[i]);
      }
      factor = 30.0;
    }
    else
    if (data->synthesisType == Data::SYNTHESIS_SUPRAGLOTTAL_INPUT_IMPEDANCE)
    {
      for (i=0; i < IMPULSE_RESPONSE_LENGTH; i++)
      {
        impulseResponse.setValue(i, data->internalProbePressure[i]);
      }
      factor = 3.0;
    }
    else
    if (data->synthesisType == Data::SYNTHESIS_SUBGLOTTAL_INPUT_IMPEDANCE)
    {
      for (i=0; i < IMPULSE_RESPONSE_LENGTH; i++)
      {
        impulseResponse.setValue(i, data->internalProbePressure[i]);
      }
      factor = 3.0;
    }

    // Low-pass filter the impulse response with a cut-off
    // frequency of SYNTHETIC_SPEECH_BANDWIDTH_HZ.

    IirFilter filter;
    filter.createChebyshev((double)SYNTHETIC_SPEECH_BANDWIDTH_HZ / (double)SAMPLING_RATE, false, 8);
    for (i = 0; i < IMPULSE_RESPONSE_LENGTH; i++)
    {
      impulseResponse.re[i] = filter.getOutputSample(impulseResponse.re[i]);
    }

    // Force the impulse response to be zero at its end

    Signal window;
    getWindow(window, IMPULSE_RESPONSE_LENGTH, RIGHT_HALF_OF_HANN_WINDOW);
    for (i=0; i < IMPULSE_RESPONSE_LENGTH; i++) 
    { 
      impulseResponse.re[i]*= window.x[i]; 
    }

    ComplexSignal *spectrum = data->tdsSpectrum;
    spectrum->reset(IMPULSE_RESPONSE_LENGTH);
    
    *spectrum = impulseResponse;
    complexFFT(*spectrum, IMPULSE_RESPONSE_EXPONENT, true);
    (*spectrum)*= factor;
  }

  // ****************************************************************
  // Close the file with the glottis signals.
  // ****************************************************************

  if ((data->saveGlottisSignals) && (glottisStream))
  {
    glottisStream.close();
  }

  // ****************************************************************
  // Send an event to the GUI thread that tells it that this thread
  // has finished.
  // ****************************************************************

  wxCommandEvent event( wxEVT_COMMAND_MENU_SELECTED, SYNTHESIS_THREAD_EVENT );
  event.SetInt(-1); // that's all
  wxPostEvent(window, event);

  return NULL;
}


// ****************************************************************************
// ****************************************************************************

void SynthesisThread::OnExit()
{
}

// ****************************************************************************
