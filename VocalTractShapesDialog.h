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

#ifndef __VOCAL_TRACT_SHAPES_DIALOG__
#define __VOCAL_TRACT_SHAPES_DIALOG__

#include <wx/wx.h>
#include <wx/dialog.h>

#include "Data.h"

// ****************************************************************************
/// This dialog lets the user select, edit and add vocal tract configurations
/// to the list of shapes.
// ****************************************************************************

class VocalTractShapesDialog : public wxDialog
{
  // **************************************************************************
  // Public data.
  // **************************************************************************

public:

  // **************************************************************************
  // Public functions.
  // **************************************************************************

public:
  static VocalTractShapesDialog *getInstance(wxWindow *parent = NULL);
  void updateWidgets();
  void fillShapeList();
  void setUpdateRequestReceiver(wxWindow* receiver1, wxWindow* receiver2);

  // **************************************************************************
  // Private data.
  // **************************************************************************

private:
  static VocalTractShapesDialog *instance;

  Data *data;
  VocalTract *tract;
  wxWindow *updateRequestReceiver1;
  wxWindow* updateRequestReceiver2;

  wxListBox *lstShapes;
  wxTextCtrl *txtValue[VocalTract::NUM_PARAMS];

  // **************************************************************************
  // Private functions.
  // **************************************************************************

private:
  VocalTractShapesDialog(wxWindow *parent);
  void initWidgets();

  void outputShapeVars(int selection);
  void selectShape(int index);
  int getSelectedShape();

  void updateVocalTract();

  void OnAdd(wxCommandEvent &event);
  void OnReplace(wxCommandEvent &event);
  void OnDelete(wxCommandEvent &event);
  void OnRename(wxCommandEvent &event);
  void OnSelect(wxCommandEvent &event);
  void OnMoveItemUp(wxCommandEvent &event);
  void OnMoveItemDown(wxCommandEvent &event);
  void OnValueLostFocus(wxFocusEvent &event);

  void OnItemSelected(wxCommandEvent &event);
  void OnItemActivated(wxCommandEvent &event);
  void OnListKeyDown(wxKeyEvent &event);

  void OnClose(wxCloseEvent &event);

  // **************************************************************************
  // Declare the event table.
  // **************************************************************************

  DECLARE_EVENT_TABLE()
};

#endif