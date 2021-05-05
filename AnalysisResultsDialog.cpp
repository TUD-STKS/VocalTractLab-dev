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

#include <wx/clipbrd.h>
#include "AnalysisResultsDialog.h"
#include "Backend/Dsp.h"

// The single instance of this class.
AnalysisResultsDialog *AnalysisResultsDialog::instance = NULL;


// ****************************************************************************
// IDs.
// ****************************************************************************

static const int IDG_GRID              = 8000;
static const int IDM_COPY_TO_CLIPBOARD = 8001;


// ****************************************************************************
// The event table.
// ****************************************************************************

BEGIN_EVENT_TABLE(AnalysisResultsDialog, wxDialog)
  EVT_GRID_CMD_CELL_RIGHT_CLICK(IDG_GRID, AnalysisResultsDialog::OnCellRightClicked)
  EVT_MENU(IDM_COPY_TO_CLIPBOARD, AnalysisResultsDialog::OnCopyToClipboard)
  EVT_SHOW(AnalysisResultsDialog::OnShow)
END_EVENT_TABLE()


// ****************************************************************************
/// Returns the single instance of this dialog.
// ****************************************************************************

AnalysisResultsDialog *AnalysisResultsDialog::getInstance()
{
  if (instance == NULL)
  {
    instance = new AnalysisResultsDialog(NULL);
  }

  return instance;
}


// ****************************************************************************
/// Update the widget states.
/// Output the measured values at the selected point in time.
// ****************************************************************************

void AnalysisResultsDialog::updateWidgets()
{
  double pos_s = 0.0;
  double value;
  double semitones;
  int i;

  // ****************************************************************
  // For which time position shall we show the results?
  // ****************************************************************

  if (data->currentPage == Data::GESTURAL_SCORE_PAGE)
  {
    pos_s = data->gesturalScoreMark_s;
  }
  else
  {
    pos_s = (double)data->mark_pt / (double)SAMPLING_RATE;
  }

  // ****************************************************************
  // F0 values.
  // ****************************************************************

  for (i=0; i < Data::NUM_TRACKS; i++)
  {
    value = getInterpolatedValue(data->f0Signal[i], data->f0TimeStep_s, pos_s);
    semitones = hertzToSemitones(value);
    if (semitones < 0.0)
    {
      semitones = 0.0;
    }
    grid->SetCellValue(i, 0, wxString::Format("%2.8f", semitones));
    grid->SetCellValue(i, 1, wxString::Format("%2.8f", value));
  }

  // ****************************************************************
  // Voice quality values.
  // ****************************************************************

  for (i=0; i < Data::NUM_TRACKS; i++)
  {
    value = getInterpolatedValue(data->voiceQualitySignal[i], data->voiceQualityTimeStep_s, pos_s);
    grid->SetCellValue(i, 6, wxString::Format("%2.3f", value));
  }

  this->Update();
}


// ****************************************************************************
/// Private constructor.
// ****************************************************************************

AnalysisResultsDialog::AnalysisResultsDialog(wxWindow *parent) : 
  wxDialog(parent, wxID_ANY, wxString("Analysis results"), 
           wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxSTAY_ON_TOP)
{
  this->Move(100, 100);

  // ****************************************************************
  // Init the variables first.
  // ****************************************************************

  data = Data::getInstance();

  // ****************************************************************
  // Init and update the widgets.
  // ****************************************************************

  initWidgets();
  updateWidgets();
}


// ****************************************************************************
/// Init the widgets.
// ****************************************************************************

void AnalysisResultsDialog::initWidgets()
{
  const int NUM_ROWS = 3;
  const int NUM_COLS = 7;
  int i, j;

  // Create an invisible dummy button with the ID wxID_CANCEL, so that
  // the user can close the dialog with ESC.
  wxButton *button = new wxButton(this, wxID_CANCEL, "", wxPoint(0,0), wxSize(0,0));

  // ****************************************************************
  // Install a user key-event handler to capture "Ctrl+C" and
  // "Ctrl+Ins" events.
  // ****************************************************************

  this->Connect(wxID_ANY, wxEVT_KEY_DOWN, wxKeyEventHandler(AnalysisResultsDialog::OnKeyDown), NULL, this);

  // ****************************************************************
  // Create the context menu.
  // ****************************************************************

  contextMenu = new wxMenu();
  contextMenu->Append(IDM_COPY_TO_CLIPBOARD, "Copy to clipboard");

  // ****************************************************************
  // Create the table grid.
  // ****************************************************************

  grid = new wxGrid(this, IDG_GRID, wxDefaultPosition, wxSize(200, 100));
  grid->CreateGrid(NUM_ROWS, NUM_COLS);

  grid->DisableDragColSize();
  grid->DisableDragRowSize();

  // ****************************************************************
  // All of the cells are read-only.
  // ****************************************************************

  for (i=0; i < NUM_ROWS; i++)
  {
    for (j=0; j < NUM_COLS; j++)
    {
      grid->SetReadOnly(i, j);
    }
  }

  // ****************************************************************
  // Set the labels for the rows and columns.
  // ****************************************************************

  grid->SetRowLabelValue(0, "Main track");
  grid->SetRowLabelValue(1, "EGG track");
  grid->SetRowLabelValue(2, "Extra track");

  grid->SetColLabelValue(0, "F0 [st]");
  grid->SetColLabelValue(1, "F0 [Hz]");
  grid->SetColLabelValue(2, "F1 [Hz]");
  grid->SetColLabelValue(3, "F2 [Hz]");
  grid->SetColLabelValue(4, "F3 [Hz]");
  grid->SetColLabelValue(5, "SPL [dB]");
  grid->SetColLabelValue(6, "   VQ   ");

  // ****************************************************************
  // Make the background color of the formant colums slightly gray.
  // ****************************************************************

  for (i=0; i < NUM_ROWS; i++)
  {
    for (j=2; j <= 4; j++)
    {
      grid->SetCellBackgroundColour(i, j, wxColor(240, 240, 240));
    }
  }

  // ****************************************************************
  // Formant values are integers and all others are floats with 
  // precision 1 or 2.
  // ****************************************************************

  grid->SetColFormatFloat(0, 6, 1);
  grid->SetColFormatFloat(1, 6, 1);
  grid->SetColFormatFloat(2, 6, 0);
  grid->SetColFormatFloat(3, 6, 0);
  grid->SetColFormatFloat(4, 6, 0);
  grid->SetColFormatFloat(5, 6, 1);
  grid->SetColFormatFloat(6, 6, 2);

  // ****************************************************************
  // Fit the grid size to the content and set the client size of the 
  // dialog to fit the grid size.
  // ****************************************************************

  grid->Fit();
  this->SetClientSize( grid->GetSize() );
}


// ****************************************************************************
/// This function returns the interpolated value in a samples function signal
/// at pos_s.
/// When a function value is zero, it is considered as invalid.
// ****************************************************************************

double AnalysisResultsDialog::getInterpolatedValue(vector<double> signal, double timeStep_s, double pos_s)
{
  const double EPSILON = 0.000001;
  double value = 0.0;
  int N = (int)signal.size();

  if (pos_s < 0.0)
  {
    pos_s = 0.0;
  }
  int i = (int)(pos_s / timeStep_s);
  if (i < N-1)
  {
    if ((fabs(signal[i]) > EPSILON) && (fabs(signal[i+1]) > EPSILON))
    {
      double frac = (pos_s - i*timeStep_s) / timeStep_s;
      value = (1.0 - frac)*signal[i] + frac*signal[i+1];
    }
  }

  return value;
}


// ****************************************************************************
/// Copies the content of the selected cells to the clipboard.
// ****************************************************************************

void AnalysisResultsDialog::copyToClipboard()
{
  int numRows = grid->GetNumberRows();
  int numCols = grid->GetNumberCols();
  int x, y;
  int minX = numCols;
  int maxX = -1;
  int minY = numRows;
  int maxY = -1;

  for (y=0; y < numRows; y++)
  {
    for (x=0; x < numCols; x++)
    {
      if (grid->IsInSelection(y, x))
      {
        if (x < minX) { minX = x; }
        if (x > maxX) { maxX = x; }
        if (y < minY) { minY = y; }
        if (y > maxY) { maxY = y; }
      }
    }
  }

  if ((minX >= 0) && (minX < numCols) && (maxX >= 0) && (maxX < numCols) &&
      (minY >= 0) && (minY < numRows) && (maxY >= 0) && (maxY < numRows))
  {
    wxString st;

    for (y=minY; y <= maxY; y++)
    {
      for (x=minX; x <= maxX; x++)
      {
        if (grid->IsInSelection(y, x))
        {
          st+= grid->GetCellValue(y, x);
        }
        // Separate columns by tabs
        if (x < maxX)
        {
          st+="\t";
        }
      }
      // Separate rows by a new line char
      if (y < maxY)
      {
        st+="\n";
      }
    }

    // Write the string into the clipboard
    if (wxTheClipboard->Open())
    {
      wxTextDataObject *data = new wxTextDataObject(st);
      wxTheClipboard->SetData(data);
      wxTheClipboard->Close();
    }
  }
}

// ****************************************************************************
// ****************************************************************************

void AnalysisResultsDialog::OnCellRightClicked(wxGridEvent &event)
{
  PopupMenu(contextMenu);
}

// ****************************************************************************
/// Handle "Ctrl+C" and "Ctrl+Ins" to insert selected cell text into the 
/// clipboard.
// ****************************************************************************

void AnalysisResultsDialog::OnKeyDown(wxKeyEvent &event)
{
  if ((event.ControlDown()) && ((event.GetKeyCode() == WXK_INSERT) || (event.GetKeyCode() == 'C')))
  {
    copyToClipboard();
  }

  // Handle other key events as usual.
  event.Skip();
}

// ****************************************************************************
// ****************************************************************************

void AnalysisResultsDialog::OnCopyToClipboard(wxCommandEvent &event)
{
  copyToClipboard();
}


// ****************************************************************************
// ****************************************************************************

void AnalysisResultsDialog::OnShow(wxShowEvent &event)
{
  updateWidgets();
  event.Skip();
}

// ****************************************************************************
