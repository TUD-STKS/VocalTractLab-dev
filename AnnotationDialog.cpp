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

#include "AnnotationDialog.h"

// The single instance of this class.
AnnotationDialog *AnnotationDialog::instance = NULL;


// ****************************************************************************
// IDs.
// ****************************************************************************

static const int IDG_GRID              = 9000;


// ****************************************************************************
// The event table.
// ****************************************************************************

BEGIN_EVENT_TABLE(AnnotationDialog, wxDialog)
  EVT_GRID_CMD_CELL_CHANGED(IDG_GRID, AnnotationDialog::OnCellChanged)
  EVT_SHOW(AnnotationDialog::OnShow)
  EVT_KILL_FOCUS(AnnotationDialog::OnKillFocus)
END_EVENT_TABLE()


// ****************************************************************************
/// Returns the single instance of this dialog.
// ****************************************************************************

AnnotationDialog *AnnotationDialog::getInstance(wxWindow *updateEventReceiver)
{
  if (instance == NULL)
  {
    instance = new AnnotationDialog(NULL);
  }
  instance->updateEventReceiver = updateEventReceiver;

  return instance;
}


// ****************************************************************************
/// Update the widget states.
/// Output the key-value pairs of the selected segment.
// ****************************************************************************

void AnnotationDialog::updateWidgets()
{
  const int NUM_ROWS = Segment::MAX_LABELS;
  const int NUM_COLS = 2;

  SegmentSequence *sequence = data->segmentSequence;
  Segment *s = sequence->getSegment(data->selectedSegmentIndex);
  int i;

  if (s != NULL)
  {
    char st[256];
    sprintf(st, "%2.4f", s->duration_s);
    s->value[Segment::DURATION_INDEX] = string(st);

    for (i=0; i < NUM_ROWS; i++)
    {
      if (Segment::fixedKey[i].empty())
      {
        grid->SetCellValue(i, 0, wxString( s->key[i] ));
      }
      grid->SetCellValue(i, 1, wxString( s->value[i] ));
    }
  }
  else
  {
    for (i=0; i < NUM_ROWS; i++)
    {
      if (Segment::fixedKey[i].empty())
      {
        grid->SetCellValue(i, 0, wxString(""));
      }
      grid->SetCellValue(i, 1, wxString(""));
    }
  }

  this->Update();
}


// ****************************************************************************
/// Private constructor.
// ****************************************************************************

AnnotationDialog::AnnotationDialog(wxWindow *parent) : 
  wxDialog(parent, wxID_ANY, wxString("Segment annotations"), 
           wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE)
{
  this->Move(200, 200);

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

void AnnotationDialog::initWidgets()
{
  const int NUM_ROWS = Segment::MAX_LABELS;
  const int NUM_COLS = 2;
  int i;

  // Create an invisible dummy button with the ID wxID_CANCEL, so that
  // the user can close the dialog with ESC.
  wxButton *button = new wxButton(this, wxID_CANCEL, "", wxPoint(0,0), wxSize(0,0));

  // ****************************************************************
  // Create the table grid.
  // ****************************************************************

  grid = new wxGrid(this, IDG_GRID, wxDefaultPosition, this->FromDIP(wxSize(250, 350)));
  grid->CreateGrid(NUM_ROWS, NUM_COLS);

  grid->DisableDragColSize();
  grid->DisableDragRowSize();

  grid->SetRowLabelSize(25);    // Width of the row labels
  grid->SetColSize(0, 130);     // Width of columns with the keys
  grid->SetSelectionMode(wxGrid::wxGridSelectCells);


  // ****************************************************************
  // Cells with fixed keys are read-only.
  // ****************************************************************

  for (i=0; i < NUM_ROWS; i++)
  {
    if (Segment::fixedKey[i].empty() == false)
    {
      grid->SetReadOnly(i, 0);
      // Make the read-only cells slightly gray.
      grid->SetCellBackgroundColour(i, 0, wxColor(240, 240, 240));
      grid->SetCellValue(i, 0, wxString( Segment::fixedKey[i] ));
    }
  }

  // ****************************************************************
  // Set the labels for the rows and columns.
  // ****************************************************************

  grid->SetColLabelValue(0, "Feature");
  grid->SetColLabelValue(1, "    Value    ");

  // ****************************************************************
  // Fit the grid size to the content and set the client size of the 
  // dialog to fit the grid size.
  // ****************************************************************

//  grid->Fit();
  // Enable vertical scrolling by setting the vertical scroll rate > 0.
  grid->FitInside();
  grid->SetScrollRate(0, 5);

  wxSize size = grid->GetSize();
  this->SetClientSize(size.x, 300);
}


// ****************************************************************************
/// The value of a cell was changed.
// ****************************************************************************

void AnnotationDialog::OnCellChanged(wxGridEvent &event)
{
  int row = event.GetRow();
  int col = event.GetCol();
  string st = grid->GetCellValue(row, col).ToStdString();

  if (row >= Segment::MAX_LABELS)
  {
    return;
  }

  Segment *s = data->segmentSequence->getSegment(data->selectedSegmentIndex);

  if (s == NULL)
  {
    return;
  }

  // A key name was changed.
  if (col == 0)
  {
    s->key[row] = st;
  }
  else
  // A value was changed.
  {
    s->value[row] = st;
    if (row == Segment::DURATION_INDEX)
    {
      s->duration_s = atof(st.c_str());
      // Ensure a minimum duration of 1 ms.
      if (s->duration_s < 0.001)
      {
        s->duration_s = 0.001;
      }
    }
  }

  updateWidgets();

  // ****************************************************************
  // Update the parent window (pictures) of the dialog.
  // ****************************************************************

  if (updateRequestEvent != 0)
  {
    wxCommandEvent event(updateRequestEvent);
    event.SetInt(REFRESH_PICTURES);
    wxPostEvent(updateEventReceiver, event);
  }
}


// ****************************************************************************
// ****************************************************************************

void AnnotationDialog::OnShow(wxShowEvent &event)
{
  updateWidgets();
}

// ****************************************************************************
// ****************************************************************************

void AnnotationDialog::OnKillFocus(wxFocusEvent &event)
{
}

// ****************************************************************************
