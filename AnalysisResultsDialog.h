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

#ifndef __ANALYSIS_RESULTS_DIALOG_H__
#define __ANALYSIS_RESULTS_DIALOG_H__

#include <wx/wx.h>
#include <wx/dialog.h>
#include <wx/grid.h>
#include <vector>

#include "Data.h"

using namespace std;


// ****************************************************************************
/// This dialog presents the estimation results for F0, F1, F2, F3, and voice
/// quality in a table for all audio tracks.
// ****************************************************************************

class AnalysisResultsDialog : public wxDialog
{
  // **************************************************************************
  // Public data.
  // **************************************************************************

public:

  // **************************************************************************
  // Public functions.
  // **************************************************************************

public:
  static AnalysisResultsDialog *getInstance();
  void updateWidgets();

  // **************************************************************************
  // Private data.
  // **************************************************************************

private:
  // The single instance of this Singleton
  static AnalysisResultsDialog *instance;
  wxGrid *grid;
  Data *data;
  wxMenu *contextMenu;

  // **************************************************************************
  // Private functions.
  // **************************************************************************

private:
  AnalysisResultsDialog(wxWindow *parent);
  void initWidgets();
  double getInterpolatedValue(vector<double> signal, double timeStep_s, double pos_s);
  void copyToClipboard();

  void OnCellRightClicked(wxGridEvent &event);
  void OnKeyDown(wxKeyEvent &event);
  void OnCopyToClipboard(wxCommandEvent &event);
  void OnShow(wxShowEvent &event);

  // **************************************************************************
  // Declare the event table.
  // **************************************************************************

  DECLARE_EVENT_TABLE()
};


#endif
