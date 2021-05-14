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

#ifndef __LF_PULSE_DIALOG_H__
#define __LF_PULSE_DIALOG_H__

#include <wx/wx.h>
#include <wx/dialog.h>

#include "Data.h"
#include "LfPulsePicture.h"
#include "Backend/LfPulse.h"

// ****************************************************************************
/// This dialog lets the user adjust the LF-pulse parameters and shows a
/// picture of the pulse waveform.
// ****************************************************************************

class LfPulseDialog : public wxDialog
{
  // **************************************************************************
  // Public data.
  // **************************************************************************

public:

  // **************************************************************************
  // Public functions.
  // **************************************************************************

public:
  static LfPulseDialog *getInstance();

  // **************************************************************************
  // Private data.
  // **************************************************************************

private:
	enum Params
	{
	OPEN_QUOTIENT,
	SHAPE_QUOTIENT,
	SPECTRAL_TILT,
	FUNDAMENTAL_FREQUENCY,
	SIGNAL_TO_NOISE_RATIO,
    NUM_PARAMS
  };

  static const double PARAM_MIN[NUM_PARAMS];
  static const double PARAM_MAX[NUM_PARAMS];

  // The single instance of this Singleton
  static LfPulseDialog *instance;

  Data *data;
  LfPulse *lfPulse;

  LfPulsePicture *lfPulsePicture;
  wxScrollBar *scrParam[NUM_PARAMS];
  wxStaticText *labParam[NUM_PARAMS];

  // **************************************************************************
  // Private functions.
  // **************************************************************************

private:
  LfPulseDialog(wxWindow *parent);
  void initWidgets();
  void updateWidgets();

  void OnScrollEvent(wxScrollEvent &event);

  // **************************************************************************
  // Declare the event table.
  // **************************************************************************

  DECLARE_EVENT_TABLE()
};


#endif
