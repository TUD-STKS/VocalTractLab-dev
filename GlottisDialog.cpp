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

#include <wx/filename.h>
#include "GlottisDialog.h"

// IDs of the controls.
static const int IDB_USE_THIS_MODEL  = 6000;
static const int IDN_NOTEBOOK        = 6001;  
static const int IDB_FILE_NAME       = 6002;
static const int IDC_SAVE_DATA       = 6003;

// The single instance of this class.
GlottisDialog *GlottisDialog::instance = NULL;

// ****************************************************************************
// The event table.
// ****************************************************************************

BEGIN_EVENT_TABLE(GlottisDialog, wxDialog)
  EVT_BUTTON(IDB_USE_THIS_MODEL, GlottisDialog::OnUseThisModel)
  EVT_CHECKBOX(IDC_SAVE_DATA, GlottisDialog::OnSaveDataChanged)
  EVT_BUTTON(IDB_FILE_NAME, GlottisDialog::OnFileName)
  EVT_NOTEBOOK_PAGE_CHANGED(IDN_NOTEBOOK, GlottisDialog::OnePageChanged)
  EVT_SHOW(GlottisDialog::OnShow)
END_EVENT_TABLE()


// ****************************************************************************
/// Returns the single instance of this dialog.
// ****************************************************************************

GlottisDialog *GlottisDialog::getInstance()
{
  if (instance == NULL)
  {
    instance = new GlottisDialog(NULL);
  }

  return instance;
}


// ****************************************************************************
/// Update the widgets.
// ****************************************************************************

void GlottisDialog::updateWidgets()
{
  int i;
  Data *data = Data::getInstance();
  wxString st;

  // ****************************************************************
  // Checkbox for saving the data during synthesis.
  // ****************************************************************

  chkSaveData->SetValue( data->saveGlottisSignals );

  // ****************************************************************
  // Mark the tab with the currently selected glottis model with
  // a star (*).
  // ****************************************************************
  
  for (i=0; i < Data::NUM_GLOTTIS_MODELS; i++)
  {
    st = notebook->GetPageText(i);
    if (i == data->getSelectedGlottisIndex())
    {
      if (st.EndsWith("*") == false)
      {
        st+= "*";
      }
    }
    else
    {
      if (st.EndsWith("*"))
      {
        st = st.RemoveLast();
      }
    }

    notebook->SetPageText(i, st);
  }

  // ****************************************************************
  // Update the widgets on the selected page.
  // ****************************************************************

  int k = notebook->GetSelection();
  if (k != -1)
  {
    page[k]->updateWidgets();
  }
}

// ****************************************************************************
/// Private constructor.
// ****************************************************************************

GlottisDialog::GlottisDialog(wxWindow *parent) : 
  wxDialog(parent, wxID_ANY, wxString("Glottis models"), 
           wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE)
{
  this->SetDoubleBuffered(true);
  this->Move(150, 100);

  data = Data::getInstance();

  initWidgets();
  updateWidgets();
}


// ****************************************************************************
/// Init the widgets.
// ****************************************************************************

void GlottisDialog::initWidgets()
{
  int i;
  wxBoxSizer *topLevelSizer = new wxBoxSizer(wxVERTICAL);
  wxBoxSizer *upperSizer = new wxBoxSizer(wxHORIZONTAL);

  // Create an invisible dummy button with the ID wxID_CANCEL, so that
  // the user can close the dialog with ESC.
  wxButton *button = new wxButton(this, wxID_CANCEL, "", wxPoint(0,0), wxSize(0,0));

  // ****************************************************************
  // Info text and select button.
  // ****************************************************************

  wxButton *selectButton = new wxButton(this, IDB_USE_THIS_MODEL, "Use selected model for synthesis");
  wxButton *fileButton = new wxButton(this, IDB_FILE_NAME, "File name");
  chkSaveData = new wxCheckBox(this, IDC_SAVE_DATA, "Save glottis signals during synthesis");

  upperSizer->Add(chkSaveData, 0, wxALL | wxGROW, 5);
  upperSizer->Add(fileButton, 0, wxALL | wxGROW, 5);

  // ****************************************************************
  // Create the notebook and add the pages.
  // ****************************************************************

  notebook = new wxNotebook(this, IDN_NOTEBOOK);
  bool doSelect = false;

  for (i=0; i < Data::NUM_GLOTTIS_MODELS; i++)
  {
    page[i] = new GlottisPanel(notebook, i);
    
    if (data->getSelectedGlottisIndex() == i)
    {
      doSelect = true;
    }
    else
    {
      doSelect = false;
    }

    notebook->AddPage(page[i], wxString(data->glottis[i]->getName()), doSelect);
  }

  // Set a fixed size for the notebook area.
  // The notebook pages are wxScrolledWindows, so that
  // a (vertical) scroll bar is provided automatically if needed.

  notebook->SetMinSize(this->FromDIP(wxSize(400, 600)));

  // ****************************************************************
  // Init the top-level sizer and set it for this dialog
  // ****************************************************************

  topLevelSizer->Add(upperSizer, 0, wxALL, 0);
  topLevelSizer->Add(selectButton, 0, wxLEFT, 5);
  topLevelSizer->Add(notebook, 0, wxALL, 5);


  this->SetSizer(topLevelSizer);
  topLevelSizer->Fit(this);
  topLevelSizer->SetSizeHints(this);
}

// ****************************************************************************
// ****************************************************************************

void GlottisDialog::OnUseThisModel(wxCommandEvent &event)
{
  int i = notebook->GetSelection();
  data->selectGlottis(i);
  updateWidgets();
}


// ****************************************************************************
// ****************************************************************************

void GlottisDialog::OnSaveDataChanged(wxCommandEvent &event)
{
  data->saveGlottisSignals = chkSaveData->IsChecked();
  updateWidgets();
}


// ****************************************************************************
/// Set the file name for the glottis signals during synthesis.
// ****************************************************************************

void GlottisDialog::OnFileName(wxCommandEvent &event)
{
  wxFileName fileName(data->glottisSignalsFileName);

  data->glottisSignalsFileName = 
    wxFileSelector("Select file for glottis signals", fileName.GetPath(), 
      fileName.GetFullName(), ".txt", "Text files (*.txt)|*.txt", 
      wxFD_SAVE, this);
}

// ****************************************************************************
/// The user changed the page to display.
// ****************************************************************************

void GlottisDialog::OnePageChanged(wxNotebookEvent &event)
{
  int k = event.GetSelection();

  if (k != -1)
  {
    page[k]->updateWidgets();
  }
}

// ****************************************************************************
// ****************************************************************************

void GlottisDialog::OnShow(wxShowEvent &event)
{
  updateWidgets();
}

// ****************************************************************************
