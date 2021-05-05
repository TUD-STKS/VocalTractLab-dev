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

#ifndef __GLOTTIS_PANEL_H__
#define __GLOTTIS_PANEL_H__

#include <wx/wx.h>

#include "Data.h"
#include "GlottisPicture.h"
#include "VocalTractLabApi/src/Glottis.h"

// ****************************************************************************
// ****************************************************************************

class GlottisPanel : public wxScrolledWindow      //wxPanel
{
  // **************************************************************************
  // Public data.
  // **************************************************************************

public:
  static const int MAX_PARAMS = 32;

  // **************************************************************************
  // Public functions.
  // **************************************************************************

public:
  GlottisPanel(wxWindow *parent, int index);
  void updateWidgets();
  void updateShapeList();

  // **************************************************************************
  // Private data.
  // **************************************************************************

private:
  int modelIndex;
  Glottis *glottis;

  GlottisPicture *picGlottis;
  wxScrollBar *scrControlParam[MAX_PARAMS];
  wxStaticText *labControlParam[MAX_PARAMS];
  wxComboBox *lstShape;
  wxTextCtrl *txtStaticParam[MAX_PARAMS];
  wxStaticText *labDerivedParam[MAX_PARAMS];

  // **************************************************************************
  // Private functions.
  // **************************************************************************

private:
  void initWidgets();
  int getSelectedShapeIndex();

  void OnListItemSelected(wxCommandEvent &event);
  void OnSaveAsSelected(wxCommandEvent &event);
  void OnSaveAsExisting(wxCommandEvent &event);
  void OnSaveAsNew(wxCommandEvent &event);
  void OnRemove(wxCommandEvent &event);
  void OnScrollEvent(wxScrollEvent &event);
  
  void OnStaticParamChanged(wxFocusEvent &event);
  void OnStaticParamEntered(wxCommandEvent &event);

  // **************************************************************************
  // Declare the event table.
  // **************************************************************************

  DECLARE_EVENT_TABLE()
};

#endif
