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

#include "GlottisPanel.h"
#include <wx/choicdlg.h>

// IDs of the controls

static const int IDL_SETTINGS           = 6000;
static const int IDB_SAVE_AS_SELECTED   = 6001;
static const int IDB_SAVE_AS_EXISTING   = 6002;
static const int IDB_SAVE_AS_NEW        = 6003;
static const int IDB_REMOVE             = 6004;

static const int IDS_CONTROL_PARAM_0    = 6100;
static const int IDL_CONTROL_PARAM_0    = 6200;
static const int IDT_STATIC_PARAM_0     = 6300;
static const int IDL_DERIVED_PARAM_0    = 6400;

// ****************************************************************************

static const int NUM_SCROLL_STEPS = 1000;

// ****************************************************************************
// The event table.
// ****************************************************************************

BEGIN_EVENT_TABLE(GlottisPanel, wxPanel)
  EVT_COMBOBOX(IDL_SETTINGS, GlottisPanel::OnListItemSelected)
  EVT_BUTTON(IDB_SAVE_AS_SELECTED, GlottisPanel::OnSaveAsSelected)
  EVT_BUTTON(IDB_SAVE_AS_EXISTING, GlottisPanel::OnSaveAsExisting)
  EVT_BUTTON(IDB_SAVE_AS_NEW, GlottisPanel::OnSaveAsNew)
  EVT_BUTTON(IDB_REMOVE, GlottisPanel::OnRemove)
  EVT_SCROLL(GlottisPanel::OnScrollEvent)
END_EVENT_TABLE()


// ****************************************************************************
/// Constructor.
// ****************************************************************************

GlottisPanel::GlottisPanel(wxWindow *parent, int index) : wxScrolledWindow(parent) //wxPanel(parent)
{
  modelIndex = index;
  Data *data = Data::getInstance();
  glottis = data->glottis[modelIndex];

  initWidgets();
  updateWidgets();
  updateShapeList();

  // Enable vertical scrolling by setting the vertical scroll rate > 0.
  this->SetScrollRate(0, 5);
}


// ****************************************************************************
/// Update all widgets.
// ****************************************************************************

void GlottisPanel::updateWidgets()
{
  int i;
  int pos;
  wxString st;
  Glottis::Parameter *p = NULL;

  // ****************************************************************
  // Update the derived values
  // ****************************************************************
  
  for (i=0; i < (int)glottis->derivedParam.size(); i++)
  {
    st = wxString::Format("%2.2f %s", 
      glottis->derivedParam[i].x*glottis->derivedParam[i].factor,
      glottis->derivedParam[i].userUnit.c_str());
    if (st != labDerivedParam[i]->GetLabel())
    {
      labDerivedParam[i]->SetLabel(st);    
    }
  }

  // ****************************************************************
  // Update the static parameters
  // ****************************************************************

  for (i=0; i < (int)glottis->staticParam.size(); i++)
  {
    st = wxString::Format("%2.3f", glottis->staticParam[i].x*glottis->staticParam[i].factor);
    txtStaticParam[i]->SetValue(st);    
  }

  // ****************************************************************
  // Update the control parameters
  // ****************************************************************

  for (i=0; i < (int)glottis->controlParam.size(); i++)
  {
    p = &glottis->controlParam[i];

    st = wxString::Format("%2.2f %s", p->x*p->factor, p->userUnit.c_str());
    if (st != labControlParam[i]->GetLabel())
    {
      labControlParam[i]->SetLabel(st);
    }

    // Update the scroll bar

    // Add 0.5 to value before casting to int to round to nearest int!
    pos = (int)((double)NUM_SCROLL_STEPS*(p->x - p->min) / (p->max - p->min) + 0.5);
    if (pos < 0)
    {
      pos = 0;
    }
    if (pos > NUM_SCROLL_STEPS)
    {
      pos = NUM_SCROLL_STEPS;
    }

    if (pos != scrControlParam[i]->GetThumbPosition())
    {
      scrControlParam[i]->SetThumbPosition(pos);
    }
  }

  // ****************************************************************
  // Update the picture.
  // ****************************************************************
  
  picGlottis->Refresh();
}


// ****************************************************************************
/// Updates the list with the glottal shapes.
// ****************************************************************************

void GlottisPanel::updateShapeList()
{
  lstShape->Clear();

  int i;
  for (i=0; i < (int)glottis->shape.size(); i++)
  {
    lstShape->Append(glottis->shape[i].name);    
  }
}


// ****************************************************************************
// ****************************************************************************

void GlottisPanel::initWidgets()
{
  int i;
  wxBoxSizer *topLevelSizer = new wxBoxSizer(wxVERTICAL);
  wxBoxSizer *topSizer = new wxBoxSizer(wxHORIZONTAL);
  wxStaticBoxSizer *topRightSizer = new wxStaticBoxSizer(wxVERTICAL, this, "Derived values");
  wxStaticBoxSizer *middleSizer = new wxStaticBoxSizer(wxVERTICAL, this, "Control parameters");
  wxStaticBoxSizer *bottomSizer = new wxStaticBoxSizer(wxVERTICAL, this, "Static parameters");
  wxFlexGridSizer *derivedParamSizer = new wxFlexGridSizer((int)glottis->derivedParam.size(), 2, 0, 0);
  wxFlexGridSizer *controlParamSizer = new wxFlexGridSizer((int)glottis->controlParam.size(), 3, 0, 0);
  wxFlexGridSizer *staticParamSizer  = new wxFlexGridSizer((int)glottis->staticParam.size(), 3, 0, 0);
  wxBoxSizer *settingSizer1 = new wxBoxSizer(wxHORIZONTAL);
  wxBoxSizer *settingSizer2 = new wxBoxSizer(wxHORIZONTAL);

  wxButton *button;
  wxStaticText *label;
  wxString st;


  // ****************************************************************
  // Init the top sizer.
  // ****************************************************************

  picGlottis = new GlottisPicture(this, glottis);

  topSizer->Add(picGlottis, 1, wxALL | wxGROW, 5);
  topSizer->Add(topRightSizer, 0, wxALL | wxGROW, 5);
  topRightSizer->Add(derivedParamSizer, 1, wxALL | wxGROW, 5);

  for (i=0; i < (int)glottis->derivedParam.size(); i++)
  {
    label = new wxStaticText(this, wxID_ANY, glottis->derivedParam[i].name);
    derivedParamSizer->Add(label, 0, wxALL, 2);

    labDerivedParam[i] = new wxStaticText(this, IDL_DERIVED_PARAM_0 + i, "xx");
    labDerivedParam[i]->SetMinSize( wxSize(55, -1) );
    derivedParamSizer->Add(labDerivedParam[i], 0, wxALL, 2);
  }

  // ****************************************************************
  // Init the middle sizer.
  // ****************************************************************

  middleSizer->Add(controlParamSizer, 0, wxALL | wxGROW, 5);
  middleSizer->Add(settingSizer1, 0, wxALL | wxGROW, 0);
  middleSizer->Add(settingSizer2, 0, wxALL | wxGROW, 0);

  for (i=0; i < (int)glottis->controlParam.size(); i++)
  {
    label = new wxStaticText(this, wxID_ANY, glottis->controlParam[i].name);
    controlParamSizer->Add(label, 0, wxALL, 5);

    scrControlParam[i] = new wxScrollBar(this, IDS_CONTROL_PARAM_0 + i);
    scrControlParam[i]->SetScrollbar(0, 1, NUM_SCROLL_STEPS + 1, 1);
    scrControlParam[i]->SetMinSize( wxSize(120, -1) );
    controlParamSizer->Add(scrControlParam[i], 0, wxALL, 5);

    labControlParam[i] = new wxStaticText(this, IDL_CONTROL_PARAM_0 + i, "xx");
    labControlParam[i]->SetMinSize( wxSize(55, -1) );
    controlParamSizer->Add(labControlParam[i], 0, wxALL, 5);
  }

  // Buttons to manage the glottal shapes.

  label = new wxStaticText(this, wxID_ANY, "Shape:");
  settingSizer1->Add(label, 0, wxALL, 5);

  lstShape = new wxComboBox(this, IDL_SETTINGS, wxEmptyString, wxDefaultPosition, 
    wxDefaultSize, 0, 0, wxCB_READONLY | wxCB_SORT);
  lstShape->SetMinSize( wxSize(100, -1) );
  settingSizer1->Add(lstShape, 0, wxALL, 5);

  button = new wxButton(this, IDB_SAVE_AS_SELECTED, "Save as selected");
  settingSizer1->Add(button, 1, wxALL, 5);

  button = new wxButton(this, IDB_SAVE_AS_EXISTING, "Save as existing");
  settingSizer2->Add(button, 1, wxALL, 5);

  button = new wxButton(this, IDB_SAVE_AS_NEW, "Save as new");
  settingSizer2->Add(button, 1, wxALL, 5);

  button = new wxButton(this, IDB_REMOVE, "Remove");
  settingSizer2->Add(button, 1, wxALL, 5);

  // ****************************************************************
  // Init the bottom sizer.
  // ****************************************************************

  bottomSizer->Add(staticParamSizer, 0, wxALL, 5);

  for (i=0; i < (int)glottis->staticParam.size(); i++)
  {
    st = wxString::Format("%s (%2.1f ... %2.1f %s)", 
      glottis->staticParam[i].name.c_str(),
      glottis->staticParam[i].min*glottis->staticParam[i].factor,
      glottis->staticParam[i].max*glottis->staticParam[i].factor,
      glottis->staticParam[i].userUnit.c_str());

    label = new wxStaticText(this, wxID_ANY, st);
    staticParamSizer->Add(label, 0, wxALL, 3);

    txtStaticParam[i] = new wxTextCtrl(this, IDT_STATIC_PARAM_0 + i, "", wxDefaultPosition,
      wxDefaultSize, wxTE_PROCESS_ENTER);
    txtStaticParam[i]->SetMinSize( wxSize(60, -1) );
    staticParamSizer->Add(txtStaticParam[i], 0, wxALL, 3);
    
    // Connect the lost-focus-event to the event handler
    this->Connect(IDT_STATIC_PARAM_0 + i, wxEVT_COMMAND_TEXT_ENTER,
      wxCommandEventHandler(GlottisPanel::OnStaticParamEntered));
    
    label = new wxStaticText(this, wxID_ANY, glottis->staticParam[i].userUnit);
    staticParamSizer->Add(label, 0, wxALL, 3);
  }
  
  // ****************************************************************
  // Init and set the top-level sizer.
  // ****************************************************************

  topLevelSizer->Add(topSizer, 1, wxALL | wxGROW, 0);
  topLevelSizer->Add(middleSizer, 0, wxALL | wxGROW, 3);
  topLevelSizer->Add(bottomSizer, 0, wxALL | wxGROW, 3);

  this->SetSizer(topLevelSizer);
}


// ****************************************************************************
/// Returns the vector index of the selected shape or -1, if no shape is
/// selected.
// ****************************************************************************

int GlottisPanel::getSelectedShapeIndex()
{
  wxString value = lstShape->GetValue();
  int i;
  int k = -1;

  for (i=0; i < (int)glottis->shape.size(); i++)
  {
    if (wxString(glottis->shape[i].name) == value)
    {
      k = i;
    }
  }

  return k;
}


// ****************************************************************************
/// A new shape was selected.
// ****************************************************************************

void GlottisPanel::OnListItemSelected(wxCommandEvent &event)
{
  int i, k = -1;
  wxString value = lstShape->GetValue();
  for (i=0; i < (int)glottis->shape.size(); i++)
  {
    if (wxString(glottis->shape[i].name) == value)
    {
      k = i;
    }
  }

  if (k == -1)
  {
    return;
  }

  Glottis::Shape *s = &glottis->shape[k];

  // Start with the 3rd control parameter - pressure and F0 keep untouched.
  for (i=2; i < (int)s->controlParam.size(); i++)
  {
    glottis->controlParam[i].x = s->controlParam[i];
  }

  glottis->calcGeometry();
  updateWidgets();
}


// ****************************************************************************
/// Save the current parameter values as the selected shape.
// ****************************************************************************

void GlottisPanel::OnSaveAsSelected(wxCommandEvent &event)
{
  int i;
  int k = getSelectedShapeIndex();

  if (k != -1)
  {
    wxString name = wxString( glottis->shape[k].name );
    if (wxMessageBox("Do you really want to save the parameters as the shape <"+name+"> ?",
      "Save shape", wxYES_NO) == wxYES)
    {
      int numParams = (int)glottis->controlParam.size();
      glottis->shape[k].controlParam.resize(numParams);
      for (i=0; i < numParams; i++)
      {
        glottis->shape[k].controlParam[i] = glottis->controlParam[i].x;
      }
    }
  }
  else
  {
    wxMessageBox("No shape selected!", "Invalid operation");
  }
}


// ****************************************************************************
/// Saves the current parameter values as one of the existing shape.
// ****************************************************************************

void GlottisPanel::OnSaveAsExisting(wxCommandEvent &event)
{
  const int MAX_CHOICES = 256;
  wxString choiceString[MAX_CHOICES];
  int i;

  // Create the list with shape names

  int numChoices = (int)glottis->shape.size();
  if (numChoices < 1)
  {
    wxMessageBox("There are no existing shapes!", "Invalid operation");
    return;
  }

  for (i=0; i < numChoices; i++)
  {
    choiceString[i] = wxString( glottis->shape[i].name );
  }

  int k = wxGetSingleChoiceIndex("Please select a shape.", "Save parameters",
    numChoices, choiceString, this, -1, -1, true, 100, 150);

  if (k != -1)
  {
    int numParams = (int)glottis->controlParam.size();
    glottis->shape[k].controlParam.resize(numParams);
    for (i=0; i < numParams; i++)
    {
      glottis->shape[k].controlParam[i] = glottis->controlParam[i].x;
    }

    lstShape->SetValue( wxString(glottis->shape[k].name) );
  }
}


// ****************************************************************************
/// Save the control parameter values as a new shape.
// ****************************************************************************

void GlottisPanel::OnSaveAsNew(wxCommandEvent &event)
{
  wxString input = wxGetTextFromUser("Please enter a name for the new shape!",
    "Save new shape");

  int i;

  // ****************************************************************
  // Check if the input name is valid.
  // ****************************************************************

  if (input.IsEmpty())
  {
    wxMessageBox("The name for the shape is not valid!", "Invalid name", wxOK);
    return;
  }

  // ****************************************************************
  // Check if a shape with the input name already exists
  // ****************************************************************

  int k = -1;
  for (i=0; i < (int)glottis->shape.size(); i++)
  {
    if (wxString(glottis->shape[i].name) == input)
    {
      k = i;
    }
  }

  // ****************************************************************
  // The name for the shape is new.
  // ****************************************************************

  if (k == -1)
  {
    Glottis::Shape s;
    s.name = input.ToStdString();
    int numParams = (int)glottis->controlParam.size();
    s.controlParam.resize(numParams);
    for (i=0; i < numParams; i++)
    {
      s.controlParam[i] = glottis->controlParam[i].x;
    }

    glottis->shape.push_back(s);
    
    updateShapeList();
    lstShape->SetValue(input);
  }
  else

  // ****************************************************************
  // The name for the shape already exists.
  // ****************************************************************
  {
    if (wxMessageBox("Do you want to replace the shape <"+input+"> ?",
      "A shape with that name already exists.", wxYES_NO) == wxYES)
    {
      int numParams = (int)glottis->controlParam.size();
      glottis->shape[k].controlParam.resize(numParams);
      for (i=0; i < numParams; i++)
      {
        glottis->shape[k].controlParam[i] = glottis->controlParam[i].x;
      }

      updateShapeList();
      lstShape->SetValue(input);
    }
  }
  
}


// ****************************************************************************
/// Removes the selected shape from the list.
// ****************************************************************************

void GlottisPanel::OnRemove(wxCommandEvent &event)
{
  int k = getSelectedShapeIndex();

  if (k != -1)
  {
    wxString value = wxString( glottis->shape[k].name );
    if (wxMessageBox("Do you really want to remove the shape <"+value+"> ?",
      "Are you sure?", wxYES_NO) == wxYES)
    {
      glottis->shape.erase( glottis->shape.begin() + k );
      updateShapeList();
    }
  }
  else
  {
    wxMessageBox("No shape selected!", "Invalid operation");
  }
}


// ****************************************************************************
/// One of the control parameters was changed.
// ****************************************************************************

void GlottisPanel::OnScrollEvent(wxScrollEvent &event)
{
  int index = event.GetId() - IDS_CONTROL_PARAM_0;

  if ((index < 0) || (index >= (int)glottis->controlParam.size()))
  {
    return;
  }

  int pos = scrControlParam[index]->GetThumbPosition();
  glottis->controlParam[index].x = glottis->controlParam[index].min + 
    (pos/(double)NUM_SCROLL_STEPS)*(glottis->controlParam[index].max - glottis->controlParam[index].min);
  
  glottis->calcGeometry();
  updateWidgets();
}


// ****************************************************************************
/// One of the static parameters was changed (text control lost focus).
// ****************************************************************************

void GlottisPanel::OnStaticParamChanged(wxFocusEvent &event)
{
  int i = event.GetId() - IDT_STATIC_PARAM_0;
  
  if ((i < 0) || (i > (int)glottis->staticParam.size()))
  {
    return;
  }

  Glottis::Parameter *p = &glottis->staticParam[i];
  wxString st = txtStaticParam[i]->GetValue();
  double value = 0.0;

  if (st.ToDouble(&value))
  {
    // Transform from user scaled value to CGS scaled value
    value = value / p->factor;
    if (value < p->min)
    {
      value = p->min;
    }
    if (value > p->max)
    {
      value = p->max;
    }
  }
  else
  {
    value = p->neutral;
  }

  p->x = value;

  glottis->calcGeometry();
  updateWidgets();
}

// ****************************************************************************
/// The user pressed <Enter> in a text field for a static parameter.
// ****************************************************************************

void GlottisPanel::OnStaticParamEntered(wxCommandEvent &event)
{
  int i = event.GetId() - IDT_STATIC_PARAM_0;
  
  if ((i < 0) || (i > (int)glottis->staticParam.size()))
  {
    return;
  }

  Glottis::Parameter *p = &glottis->staticParam[i];
  wxString st = txtStaticParam[i]->GetValue();
  double value = 0.0;

  if (st.ToDouble(&value))
  {
    // Transform from user scaled value to CGS scaled value
    value = value / p->factor;
    if (value < p->min)
    {
      value = p->min;
    }
    if (value > p->max)
    {
      value = p->max;
    }
  }
  else
  {
    value = p->x;     // Keep the current value
  }

  p->x = value;

  // Re-calculate the glottis geometry and update the widgets incl. the picture.
  glottis->calcGeometry();
  updateWidgets();
}

// ****************************************************************************

