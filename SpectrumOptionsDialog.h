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

#ifndef __SPECTRUM_OPTIONS_DIALOG_H__
#define __SPECTRUM_OPTIONS_DIALOG_H__

#include <wx/wx.h>
#include <wx/dialog.h>

#include "SpectrumPicture.h"
#include "Data.h"

// ****************************************************************************
/// This dialog lets the user adjust the options for the spectrum picture.
// ****************************************************************************

class SpectrumOptionsDialog : public wxDialog
{
  // **************************************************************************
  // Public functions.
  // **************************************************************************

public:
  static SpectrumOptionsDialog *getInstance(SpectrumPicture *spectrumPicture);

  // **************************************************************************
  // Private data.
  // **************************************************************************

private:
  // The single instance of this Singleton
  static SpectrumOptionsDialog *instance;
  static SpectrumPicture *spectrumPicture;
  Data *data;

  wxRadioBox *radSpectrumType;
  wxCheckBox *chkShowHarmonics;

  // **************************************************************************
  // Private functions.
  // **************************************************************************

private:
  SpectrumOptionsDialog(wxWindow *parent);
  void initWidgets();
  void updateWidgets();

  void OnSpectrumType(wxCommandEvent &event);
  void OnShowHarmonics(wxCommandEvent &event);

  // **************************************************************************
  // Declare the event table.
  // **************************************************************************

  DECLARE_EVENT_TABLE()
};


#endif