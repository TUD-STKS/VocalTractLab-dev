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

#ifndef __ANNOTATION_DIALOG_H__
#define __ANNOTATION_DIALOG_H__

#include <wx/wx.h>
#include <wx/dialog.h>
#include <wx/grid.h>
#include <vector>

#include "Data.h"
#include "VocalTractLabBackend/SegmentSequence.h"

using namespace std;


// ****************************************************************************
/// This allows to annotate phonetic segments.
// ****************************************************************************

class AnnotationDialog : public wxDialog
{
  // **************************************************************************
  // Public data.
  // **************************************************************************

public:
  wxGrid* grid;

  // **************************************************************************
  // Public functions.
  // **************************************************************************

public:
  static AnnotationDialog *getInstance(wxWindow *updateEventReceiver);
  void updateWidgets();

  // **************************************************************************
  // Private data.
  // **************************************************************************

private:
  // The single instance of this Singleton
  static AnnotationDialog *instance;
  Data *data;
  wxWindow *updateEventReceiver;

  // **************************************************************************
  // Private functions.
  // **************************************************************************

private:
  AnnotationDialog(wxWindow *parent);
  void initWidgets();

  void OnCellChanged(wxGridEvent &event);
  void OnShow(wxShowEvent &event);
  void OnKillFocus(wxFocusEvent &event);

  // **************************************************************************
  // Declare the event table.
  // **************************************************************************

  DECLARE_EVENT_TABLE()
};


#endif
