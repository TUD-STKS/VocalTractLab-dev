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

#ifndef __PHONETIC_PARAMS_DIALOG__
#define __PHONETIC_PARAMS_DIALOG__

#include <wx/wx.h>
#include <wx/dialog.h>

#include "Data.h"

// ****************************************************************************
/// This dialog lets the user vary phonetic parameters and see the changes in
/// the vocal tract shape and the transfer function (on the parent page).
// ****************************************************************************

class PhoneticParamsDialog : public wxDialog
{
  // **************************************************************************
  // Public data.
  // **************************************************************************

public:

  // **************************************************************************
  // Public functions.
  // **************************************************************************

public:
  static PhoneticParamsDialog *getInstance(wxWindow *parent = NULL);
  void updateWidgets();
  void setUpdateRequestReceiver(wxWindow *receiver);

  // **************************************************************************
  // Private data.
  // **************************************************************************

private:
  static PhoneticParamsDialog *instance;

  Data *data;
  wxWindow *updateRequestReceiver;
  wxScrollBar  *scrValue[Data::NUM_PHONETIC_PARAMS];
  wxStaticText *labValue[Data::NUM_PHONETIC_PARAMS];

  // **************************************************************************
  // Private functions.
  // **************************************************************************

private:
  PhoneticParamsDialog(wxWindow *parent);
  void initWidgets();
  void updateVocalTract();

  void OnScrollbarChanged(wxScrollEvent &event);

  // **************************************************************************
  // Declare the event table.
  // **************************************************************************

  DECLARE_EVENT_TABLE()
};

#endif