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

#ifndef __GLOTTIS_DIALOG_H__
#define __GLOTTIS_DIALOG_H__

#include <wx/wx.h>
#include <wx/dialog.h>
#include <wx/notebook.h>

#include "GlottisPanel.h"


// ****************************************************************************
/// This dialog lets the user adjust the parameters for the different 
/// implemented glottis models.
// ****************************************************************************

class GlottisDialog : public wxDialog
{
  // **************************************************************************
  // Public functions.
  // **************************************************************************

public:
  static GlottisDialog *getInstance();
  void updateWidgets();

  // **************************************************************************
  // Private data.
  // **************************************************************************

private:
  // The single instance of this Singleton
  static GlottisDialog *instance;
  Data *data;
  
  wxNotebook *notebook;
  GlottisPanel *page[Data::NUM_GLOTTIS_MODELS];
  wxCheckBox *chkSaveData;

  // **************************************************************************
  // Private functions.
  // **************************************************************************

private:
  GlottisDialog(wxWindow *parent);
  void initWidgets();

  void OnUseThisModel(wxCommandEvent &event);
  void OnSaveDataChanged(wxCommandEvent &event);
  void OnFileName(wxCommandEvent &event);
  void OnePageChanged(wxNotebookEvent &event);
  void OnShow(wxShowEvent &event);

  // **************************************************************************
  // Declare the event table.
  // **************************************************************************

  DECLARE_EVENT_TABLE()
};


#endif
