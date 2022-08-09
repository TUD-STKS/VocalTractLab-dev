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

#include "EmaConfigDialog.h"

// The single instance of this class.
EmaConfigDialog *EmaConfigDialog::instance = NULL;


// ****************************************************************************
/// IDs.
// ****************************************************************************

static const int IDL_SURFACES                   = 3000;
static const int IDS_VERTEX_INDEX               = 3001;
static const int IDL_EMA_POINTS                 = 3002;

static const int IDB_ITEM_UP                    = 3010;
static const int IDB_ITEM_DOWN                  = 3011;
static const int IDB_ADD_ITEM                   = 3012;
static const int IDB_REMOVE_ITEM                = 3013;
static const int IDB_RENAME_ITEM                = 3014;
static const int IDB_SET_DEFAULT_VALUES         = 3015;


// ****************************************************************************
// The event table.
// ****************************************************************************

BEGIN_EVENT_TABLE(EmaConfigDialog, wxDialog)
  EVT_LISTBOX(IDL_EMA_POINTS, EmaConfigDialog::OnEmaPointSelected)
  EVT_COMBOBOX(IDL_SURFACES, EmaConfigDialog::OnSurfaceSelected)
  EVT_SPINCTRL(IDS_VERTEX_INDEX, EmaConfigDialog::OnSpinCtrlChanged)
  
  EVT_BUTTON(IDB_ITEM_UP, EmaConfigDialog::OnItemUp)
  EVT_BUTTON(IDB_ITEM_DOWN, EmaConfigDialog::OnItemDown)
  EVT_BUTTON(IDB_ADD_ITEM, EmaConfigDialog::OnAddItem)
  EVT_BUTTON(IDB_REMOVE_ITEM, EmaConfigDialog::OnRemoveItem)
  EVT_BUTTON(IDB_RENAME_ITEM, EmaConfigDialog::OnRenameItem)
  EVT_BUTTON(IDB_SET_DEFAULT_VALUES, EmaConfigDialog::OnSetDefaultValues)
END_EVENT_TABLE()



// ****************************************************************************
/// Returns the single instance of this dialog.
// ****************************************************************************

EmaConfigDialog *EmaConfigDialog::getInstance(wxWindow *parent, wxWindow *updateEventReceiver)
{
  if (instance == NULL)
  {
    instance = new EmaConfigDialog(parent, updateEventReceiver);
  }

  return instance;
}


// ****************************************************************************
/// Constructor.
// ****************************************************************************

EmaConfigDialog::EmaConfigDialog(wxWindow *parent, wxWindow *updateEventReceiver) : 
  wxDialog(parent, wxID_ANY, wxString("EMA point definition"), 
           wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE)
{
  this->updateEventReceiver = updateEventReceiver;
  data = Data::getInstance();
  tract = data->vocalTract;

  initWidgets();
  fillEmaPointList();
  updateWidgets();
}


// ****************************************************************************
/// Init the widgets.
// ****************************************************************************

void EmaConfigDialog::initWidgets()
{
  wxButton *button = NULL;
  wxStaticText *label = NULL;
  wxBoxSizer *topLevelSizer = new wxBoxSizer(wxVERTICAL);
  wxBoxSizer *leftSizer = new wxBoxSizer(wxVERTICAL);
  wxBoxSizer *rightSizer = new wxBoxSizer(wxVERTICAL);
  wxBoxSizer *sizer = NULL;

  // Create an invisible dummy button with the ID wxID_CANCEL, so that
  // the user can close the dialog with ESC.
  
  button = new wxButton(this, wxID_CANCEL, "", wxPoint(0,0), wxSize(0,0));

  // ****************************************************************
  // Create the left sizer with the list of EMA points.
  // ****************************************************************

  label = new wxStaticText(this, wxID_ANY, "EMA points:");
  leftSizer->Add(label, 0, wxALL, 3);

  lstEmaPoints = new wxListBox(this, IDL_EMA_POINTS, wxDefaultPosition, this->FromDIP(wxSize(-1, 180)),
    wxArrayString(), wxLB_SINGLE | wxLB_ALWAYS_SB);
  leftSizer->Add(lstEmaPoints, 0, wxGROW | wxALL, 3);

  // ****************************************************************
  // Create the right sizer with the widgets to select the surface
  // and vertex of an EMA point.
  // ****************************************************************

  label = new wxStaticText(this, wxID_ANY, "Surface:");
  rightSizer->Add(label, 0, wxALL, 3);

  wxString surfaceChoices[VocalTract::NUM_EMA_SURFACES] =
  {
    "Tongue",
    "Upper cover",
    "Lower cover",
    "Upper lip",
    "Lower lip"
  };

  lstSurfaces = new wxComboBox(this, IDL_SURFACES, wxEmptyString, wxDefaultPosition, 
    wxDefaultSize, VocalTract::NUM_EMA_SURFACES, surfaceChoices, wxTE_PROCESS_ENTER | wxCB_READONLY);
  lstSurfaces->SetMinSize(this->FromDIP(wxSize(120, -1) ));
  rightSizer->Add(lstSurfaces, 1, wxGROW);

  rightSizer->AddSpacer(10);
  label = new wxStaticText(this, wxID_ANY, "Vertex:");
  rightSizer->Add(label, 0, wxALL, 3);

  spinVertex = new wxSpinCtrl(this, IDS_VERTEX_INDEX, "0", wxDefaultPosition, 
    wxDefaultSize, wxSP_ARROW_KEYS, 0, 100);
  rightSizer->Add(spinVertex, 0, wxALL, 3);

  // ****************************************************************
  // Add the left and right sizers to the top level sizer.
  // ****************************************************************

  sizer = new wxBoxSizer(wxHORIZONTAL);
  sizer->Add(leftSizer, 1, wxALL, 3);
  sizer->Add(rightSizer, 0, wxALL, 3);
  topLevelSizer->Add(sizer, 0, wxGROW);

  // ****************************************************************
  // Add the buttons at the bottom of the dialog.
  // ****************************************************************

  sizer = new wxBoxSizer(wxHORIZONTAL);

  button = new wxButton(this, IDB_ITEM_UP, "Up");
  sizer->Add(button, 0, wxALL, 3);

  button = new wxButton(this, IDB_ITEM_DOWN, "Down");
  sizer->Add(button, 0, wxALL, 3);

  button = new wxButton(this, IDB_ADD_ITEM, "Add");
  sizer->Add(button, 0, wxALL, 3);

  button = new wxButton(this, IDB_REMOVE_ITEM, "Remove");
  sizer->Add(button, 0, wxALL, 3);

  topLevelSizer->Add(sizer);

  // ****************************************************************

  sizer = new wxBoxSizer(wxHORIZONTAL);

  button = new wxButton(this, IDB_RENAME_ITEM, "Rename");
  sizer->Add(button, 0, wxALL, 3);

  button = new wxButton(this, IDB_SET_DEFAULT_VALUES, "Set default points");
  sizer->Add(button, 0, wxALL, 3);

  sizer->AddStretchSpacer();

  button = new wxButton(this, wxID_CANCEL, "Close");
  sizer->Add(button, 0, wxALL, 3);

  topLevelSizer->Add(sizer, 0, wxGROW);

  // ****************************************************************

  this->SetSizer(topLevelSizer);
  topLevelSizer->Fit(this);
  topLevelSizer->SetSizeHints(this);
}


// ****************************************************************************
/// Update the widgets.
// ****************************************************************************

void EmaConfigDialog::updateWidgets()
{
  int emaPointIndex = getSelectedEmaPoint();
  if (emaPointIndex == -1)
  {
    lstSurfaces->Disable();
    spinVertex->Disable();
  }
  else
  {
    VocalTract::EmaPoint *p = &tract->emaPoints[emaPointIndex];

    lstSurfaces->Enable();
    if ((p->emaSurface >= 0) && (p->emaSurface < VocalTract::NUM_EMA_SURFACES))
    {
      lstSurfaces->Select(p->emaSurface);
    
      spinVertex->Enable();
      int min, max;
      tract->getEmaSurfaceVertexRange(p->emaSurface, &min, &max);
      if (p->vertexIndex < min)
      {
        p->vertexIndex = 0;
      }
      if (p->vertexIndex > max)
      {
        p->vertexIndex = max;
      }

      spinVertex->SetRange(min, max);
      spinVertex->SetValue(p->vertexIndex);
    }
  }

  // ****************************************************************
  // Update the parent window of the dialog (the vocal tract dialog).
  // ****************************************************************

   wxCommandEvent event(updateRequestEvent);
   event.SetInt(REFRESH_PICTURES);
   wxPostEvent(updateEventReceiver, event);
}


// ****************************************************************************
// ****************************************************************************

void EmaConfigDialog::fillEmaPointList()
{
  int i;

  lstEmaPoints->Freeze();

  lstEmaPoints->Clear();

  for (i=0; i < (int)tract->emaPoints.size(); i++)
  {
    lstEmaPoints->Append(tract->emaPoints[i].name);
  }

  lstEmaPoints->Thaw();
}


// ****************************************************************************
/// Returns the index of the selected item, or -1, if no item is selected.
// ****************************************************************************

int EmaConfigDialog::getSelectedEmaPoint()
{
  int index = lstEmaPoints->GetSelection();
  if (index == wxNOT_FOUND)
  {
    index = -1;
  }
  return index;  
}


// ****************************************************************************
// ****************************************************************************

void EmaConfigDialog::OnEmaPointSelected(wxCommandEvent &event)
{
  updateWidgets();
}


// ****************************************************************************
/// Change the surface of the EMA point.
// ****************************************************************************

void EmaConfigDialog::OnSurfaceSelected(wxCommandEvent &event)
{
  int index = getSelectedEmaPoint();
  if (index != -1)
  {
    VocalTract::EmaPoint *p = &tract->emaPoints[index];
    p->emaSurface = (VocalTract::EmaSurface)event.GetInt();

    // Make sure the vertex index does not exceed the limits 
    // of the new surface.
    int min, max;
    tract->getEmaSurfaceVertexRange(p->emaSurface, &min, &max);
    if (p->vertexIndex < min)
    {
      p->vertexIndex = 0;
    }
    if (p->vertexIndex > max)
    {
      p->vertexIndex = max;
    }

    updateWidgets();
  }
}


// ****************************************************************************
/// Change the vertex index of the EMA point.
// ****************************************************************************

void EmaConfigDialog::OnSpinCtrlChanged(wxSpinEvent &event)
{
  int index = getSelectedEmaPoint();
  if (index != -1)
  {
    VocalTract::EmaPoint *p = &tract->emaPoints[index];

    p->vertexIndex = event.GetPosition();

    int min, max;
    tract->getEmaSurfaceVertexRange(p->emaSurface, &min, &max);
    if (p->vertexIndex < min)
    {
      p->vertexIndex = 0;
    }
    if (p->vertexIndex > max)
    {
      p->vertexIndex = max;
    }

    updateWidgets();
  }
}


// ****************************************************************************
/// Move the selected EMA point by one position up in the list.
// ****************************************************************************

void EmaConfigDialog::OnItemUp(wxCommandEvent &event)
{
  int index = getSelectedEmaPoint();
  if ((index > 0) && (index < (int)tract->emaPoints.size()))
  {
    VocalTract::EmaPoint p = tract->emaPoints[index];
    tract->emaPoints[index] = tract->emaPoints[index - 1];
    tract->emaPoints[index - 1] = p;

    fillEmaPointList();
    lstEmaPoints->Select(index - 1);
    updateWidgets();
  }
}


// ****************************************************************************
/// Move the selected EMA point by one position down in the list.
// ****************************************************************************

void EmaConfigDialog::OnItemDown(wxCommandEvent &event)
{
  int index = getSelectedEmaPoint();
  if ((index >= 0) && (index < (int)tract->emaPoints.size() - 1))
  {
    VocalTract::EmaPoint p = tract->emaPoints[index];
    tract->emaPoints[index] = tract->emaPoints[index + 1];
    tract->emaPoints[index + 1] = p;

    fillEmaPointList();
    lstEmaPoints->Select(index + 1);
    updateWidgets();
  }
}


// ****************************************************************************
/// Create a new EMA point.
// ****************************************************************************

void EmaConfigDialog::OnAddItem(wxCommandEvent &event)
{
  wxString st = wxGetTextFromUser("Enter a name for the new EMA point", 
    "Create new EMA point", "", this);

  if (st != "")
  {
    VocalTract::EmaPoint p;
    p.emaSurface = VocalTract::EMA_SURFACE_TONGUE;
    p.vertexIndex = 0;
    p.name = st.ToStdString();

    tract->emaPoints.push_back(p);
    fillEmaPointList();
    int lastIndex = (int)tract->emaPoints.size() - 1;
    lstEmaPoints->SetSelection(lastIndex);
    updateWidgets();
  }
}


// ****************************************************************************
/// Remove the selected EMA point.
// ****************************************************************************

void EmaConfigDialog::OnRemoveItem(wxCommandEvent &event)
{
  int index = getSelectedEmaPoint();
  if (index == -1)
  {
    wxMessageBox("You must select a point first.", "Error");
    return;
  }

  wxString message = "Do you really want to delete the point " + 
    wxString(tract->emaPoints[index].name) + " ?";

  if (wxMessageBox(message, "Are you sure?", wxYES_NO, this) == wxYES)
  {
    tract->emaPoints.erase( tract->emaPoints.begin() + index );
    fillEmaPointList();
    lstEmaPoints->SetSelection(index);
    updateWidgets();
  }
}


// ****************************************************************************
/// Rename the selected EMA point.
// ****************************************************************************

void EmaConfigDialog::OnRenameItem(wxCommandEvent &event)
{
  int index = getSelectedEmaPoint();
  
  if (index == -1)
  {
    wxMessageBox("You must select a point first.", "Error");
    return;
  }

  wxString st = wxGetTextFromUser("Enter a new name for the EMA point", "Enter name", 
    wxString(tract->emaPoints[index].name), this);

  if (st != "")
  {
    tract->emaPoints[index].name = st.ToStdString();
  }

  fillEmaPointList();
  lstEmaPoints->SetSelection(index);
  updateWidgets();
}


// ****************************************************************************
/// Set default values for all EMA points.
// ****************************************************************************

void EmaConfigDialog::OnSetDefaultValues(wxCommandEvent &event)
{
  tract->setDefaultEmaPoints();
  fillEmaPointList();
  updateWidgets();
}


// ****************************************************************************
