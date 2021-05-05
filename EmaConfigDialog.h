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

#ifndef __EMA_CONFIG_DIALOG_H__
#define __EMA_CONFIG_DIALOG_H__

#include <wx/wx.h>
#include <wx/dialog.h>
#include <wx/spinctrl.h>

#include "Data.h"
#include "VocalTractLabApi/src/VocalTract.h"
#include "VocalTractPicture.h"

// ****************************************************************************
/// This dialog lets the user configure the virtual EMA points.
// ****************************************************************************

class EmaConfigDialog : public wxDialog
{
  // **************************************************************************
  // Public functions.
  // **************************************************************************

public:
  static EmaConfigDialog *getInstance(wxWindow *parent, wxWindow *updateEventReceiver);

  // **************************************************************************
  // Private data.
  // **************************************************************************

private:
  static EmaConfigDialog *instance;
  Data *data;
  VocalTract *tract;
  wxWindow *updateEventReceiver;

  wxComboBox *lstSurfaces;
  wxSpinCtrl *spinVertex;
  wxListBox *lstEmaPoints;

  // **************************************************************************
  // Private functions.
  // **************************************************************************

private:
  EmaConfigDialog(wxWindow *parent, wxWindow *updateEventReceiver);

  void initWidgets();
  void updateWidgets();

  void fillEmaPointList();
  int getSelectedEmaPoint();

  void OnEmaPointSelected(wxCommandEvent &event);
  void OnSurfaceSelected(wxCommandEvent &event);
  void OnSpinCtrlChanged(wxSpinEvent &event);
  void OnItemUp(wxCommandEvent &event);
  void OnItemDown(wxCommandEvent &event);
  void OnAddItem(wxCommandEvent &event);
  void OnRemoveItem(wxCommandEvent &event);
  void OnRenameItem(wxCommandEvent &event);
  void OnSetDefaultValues(wxCommandEvent &event);

  // **************************************************************************
  // Declare the event table.
  // **************************************************************************

  DECLARE_EVENT_TABLE()
};


#endif
