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

#ifndef __SPECTRUM_PICTURE_H__
#define __SPECTRUM_PICTURE_H__

#include "BasicPicture.h"
#include "VocalTractPicture.h"
#include "Graph.h"
#include "VocalTractLabBackend/Signal.h"

// ****************************************************************************
// ****************************************************************************

class SpectrumPicture : public BasicPicture
{
  // **************************************************************************
  // Public data.
  // **************************************************************************

public:
  static const double REFERENCE_SPECTRUM_PRESSURE;
  static const double LOWEST_SPECTRUM_PRESSURE;
  static const double HIGHEST_SPECTRUM_PRESSURE;

  static const double REFERENCE_SPECTRUM_IMPEDANCE;
  static const double LOWEST_SPECTRUM_IMPEDANCE;
  static const double HIGHEST_SPECTRUM_IMPEDANCE;

  static const double REFERENCE_SPECTRUM_VOLUMEVELOCITY;
  static const double LOWEST_SPECTRUM_VOLUMEVELOCITY;
  static const double HIGHEST_SPECTRUM_VOLUMEVELOCITY;

  static const double REFERENCE_SPECTRUM_RATIO;
  static const double LOWEST_SPECTRUM_RATIO;
  static const double HIGHEST_SPECTRUM_RATIO;

  static const int    MIN_SPECTRUM_LEVEL_SHIFT = -50;
  static const int    MAX_SPECTRUM_LEVEL_SHIFT = -MIN_SPECTRUM_LEVEL_SHIFT;

  // Enumerations

  enum ModelSpectrumType
  {
    SPECTRUM_UU,
    SPECTRUM_PU,
    SPECTRUM_NOSE_UU,
    SPECTRUM_INPUT_IMPEDANCE,
    SPECTRUM_SUBGLOTTAL_INPUT_IMPEDANCE,
    SPECTRUM_VOLUME_VELOCITY_IMPEDANCE,
    SPECTRUM_SOUND_PRESSURE_VOICE_SOURCE,
    SPECTRUM_SOUND_PRESSURE_NOISE_SOURCE,
    SPECTRUM_SOUND_PRESSURE_PZ,
    SPECTRUM_VOLUME_VELOCITY,
    NUM_SPECTRUM_TYPES
  };

  enum UnitType 
  { 
    UNIT_NONE, 
    UNIT_PRESSURE, 
    UNIT_IMPEDANCE, 
    UNIT_VOLUME_VELOCITY 
  };

  // Variables
  Graph graph;

  bool showModelSpectrum;
  bool showUserSpectrum;
  bool showTdsSpectrum;
  bool showVttfHarmonics;
  bool showPoleZeroSpectrum;
  bool showFormants;
  bool showMagnitude;
  bool showPhase;

  ModelSpectrumType modelSpectrumType;

  double levelShift;              // Level shift of the secondary spectrum
  double noiseFilterCutoffFrequency;  // Simulated first-order low-pass for noise source

  // **************************************************************************
  // Public functions.
  // **************************************************************************

public:
  SpectrumPicture(wxWindow *parent, VocalTractPicture *picVocalTract);
  virtual void draw(wxDC &dc);
  void paintContinualSpectrum(wxDC &dc, ComplexSignal *spectrum, double ampFactor, wxColor color);
  void paintDiscreteSpectrum(wxDC &dc, ComplexSignal *spectrum, double ampFactor, wxColor color);

  // **************************************************************************
  // Private data.
  // **************************************************************************

private:
  wxString spectrumFileName;
  VocalTractPicture *picVocalTract;

  // **************************************************************************
  // Private functions.
  // **************************************************************************

private:
  void OnLeftButtonDown(wxMouseEvent &event);
  void OnRightButtonDown(wxMouseEvent &event);

  // **************************************************************************
  // Declare the event table.
  // **************************************************************************

  DECLARE_EVENT_TABLE()
};

#endif

// ****************************************************************************
