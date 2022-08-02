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

#include <wx/filename.h>

#include "VocalTractShapesDialog.h"
#include "Data.h"
#include "SilentMessageBox.h"
#include "Backend/SoundLib.h"
#include "Backend/Synthesizer.h"
#include "Backend/Acoustic3dSimulation.h"

// ****************************************************************************
// IDs.
// ****************************************************************************

static const int IDL_SHAPES             = 4004;
static const int IDT_VALUE_0            = 4010;
static const int IDT_VALUE_N            = 4050;
static const int IDT_DOMI_0             = 4051;
static const int IDT_DOMI_N             = 4099;

static const int IDB_ADD                = 4100;
static const int IDB_REPLACE            = 4101;
static const int IDB_DELETE             = 4102;
static const int IDB_RENAME             = 4103;
static const int IDB_SELECT             = 4104;
static const int IDB_MOVE_UP            = 4105;
static const int IDB_MOVE_DOWN          = 4106;

// ****************************************************************************
// The event table.
// ****************************************************************************

BEGIN_EVENT_TABLE(VocalTractShapesDialog, wxDialog)
  EVT_BUTTON(IDB_ADD, VocalTractShapesDialog::OnAdd)
  EVT_BUTTON(IDB_REPLACE, VocalTractShapesDialog::OnReplace)
  EVT_BUTTON(IDB_DELETE, VocalTractShapesDialog::OnDelete)
  EVT_BUTTON(IDB_RENAME, VocalTractShapesDialog::OnRename)
  EVT_BUTTON(IDB_SELECT, VocalTractShapesDialog::OnSelect)
  EVT_BUTTON(IDB_MOVE_UP, VocalTractShapesDialog::OnMoveItemUp)
  EVT_BUTTON(IDB_MOVE_DOWN, VocalTractShapesDialog::OnMoveItemDown)
  EVT_LISTBOX(IDL_SHAPES, VocalTractShapesDialog::OnItemSelected)
  EVT_LISTBOX_DCLICK(IDL_SHAPES, VocalTractShapesDialog::OnItemActivated)

  EVT_CLOSE(VocalTractShapesDialog::OnClose)
END_EVENT_TABLE()

// The single instance of this class.
VocalTractShapesDialog *VocalTractShapesDialog::instance = NULL;


// ****************************************************************************
/// Returns the single instance of this dialog.
// ****************************************************************************

VocalTractShapesDialog *VocalTractShapesDialog::getInstance(wxWindow *parent)
{
  if (instance == NULL)
  {
    instance = new VocalTractShapesDialog(parent);
  }

  return instance;
}


// ****************************************************************************
// ****************************************************************************

void VocalTractShapesDialog::updateWidgets()
{
  // ****************************************************************
  // Set the heading of the dialog
  // ****************************************************************

  if (data->speakerFileName == "")
  {
    this->SetLabel("Speaker: No file loaded");
  }
  else
  {
    this->SetLabel("Speaker: " + data->speakerFileName);
  }

  outputShapeVars( getSelectedShape() );
}


// ****************************************************************************
// ****************************************************************************

void VocalTractShapesDialog::fillShapeList()
{
  int i;

  lstShapes->Freeze();

  lstShapes->Clear();

  for (i=0; i < (int)tract->shapes.size(); i++)
  {
    lstShapes->Append(tract->shapes[i].name);
  }

  lstShapes->Thaw();
}


// ****************************************************************************
/// Set the windows that receive an update request message when a vocal tract
/// parameter was changed.
// ****************************************************************************

void VocalTractShapesDialog::setUpdateRequestReceiver(wxWindow *receiver1, wxWindow* receiver2)
{
  updateRequestReceiver1 = receiver1;
  updateRequestReceiver2 = receiver2;
}

// ****************************************************************************
/// Constructor.
// ****************************************************************************

VocalTractShapesDialog::VocalTractShapesDialog(wxWindow *parent) : 
  wxDialog(parent, wxID_ANY, wxString("Vocal tract shapes"), 
    wxDefaultPosition, wxDefaultSize, 
    wxDEFAULT_DIALOG_STYLE | wxFRAME_FLOAT_ON_PARENT | wxRESIZE_BORDER)
{
  this->Move(50, 0);
  // Enable automatic scroll bars when screen is to small for dialog size.
//  this->SetLayoutAdaptationMode(wxDIALOG_ADAPTATION_MODE_ENABLED);

  // ****************************************************************
  // Init the variables first.
  // ****************************************************************

  data = Data::getInstance();
  tract = data->vocalTract;
  updateRequestReceiver1 = NULL;
  updateRequestReceiver2 = NULL;

  // ****************************************************************
  // Init and update the widgets.
  // ****************************************************************

  initWidgets();
  updateWidgets();

  // ****************************************************************
  // Fill the shape list and set the variables.
  // ****************************************************************

  fillShapeList();
  outputShapeVars( getSelectedShape() );
}


// ****************************************************************************
/// Init the widgets of this dialog.
// ****************************************************************************

void VocalTractShapesDialog::initWidgets()
{
  int i;
  wxButton *button = NULL;
  wxStaticText *label = NULL;
  wxString st;

  wxBoxSizer *scrolledWindowSizer = new wxBoxSizer(wxVERTICAL);
  wxBoxSizer *topLevelSizer = new wxBoxSizer(wxVERTICAL);
  wxBoxSizer *middleSizer = new wxBoxSizer(wxHORIZONTAL);

  // Create an invisible dummy button with the ID wxID_CANCEL, so that
  // the user can close the dialog with ESC.
  button = new wxButton(this, wxID_CANCEL, "", wxPoint(0,0), wxSize(0,0));

  // ****************************************************************
  // ****************************************************************

  wxScrolledWindow *scrolledWindow = new wxScrolledWindow(this); // , wxID_ANY, wxDefaultPosition, wxSize(200, 200));
  scrolledWindowSizer->Add(scrolledWindow, 1, wxEXPAND, 0);

  // ****************************************************************
  // ****************************************************************

  topLevelSizer->AddSpacer(10);

  // ****************************************************************
  // ****************************************************************

  topLevelSizer->Add(middleSizer, 0, wxGROW);

  // Do not use wxLB_SORT; we want an unsorted list !
  lstShapes = new wxListBox(scrolledWindow, IDL_SHAPES, wxDefaultPosition, this->FromDIP(wxSize(190, -1)), 0, NULL, 0);
  lstShapes->Connect(wxID_ANY, wxEVT_KEY_DOWN, wxKeyEventHandler(VocalTractShapesDialog::OnListKeyDown), NULL, this);
  lstShapes->SetToolTip("Press <SPACE> to play the shape sound.");
  middleSizer->Add(lstShapes, 1, wxGROW | wxALL, 8);

  // ****************************************************************
  // Create the variable list
  // ****************************************************************

  wxFlexGridSizer *varSizer = new wxFlexGridSizer(VocalTract::NUM_PARAMS + 3, 2, 5, 5);
  middleSizer->Add(varSizer, 0, wxGROW | wxALL, 5);

  label = new wxStaticText(scrolledWindow, wxID_ANY, "Variable");
  varSizer->Add(label);

  label = new wxStaticText(scrolledWindow, wxID_ANY, "Value");
  varSizer->Add(label);

  for (i=0; i < VocalTract::NUM_PARAMS; i++)
  {
    if (i == VocalTract::TS1)
    {
      // Add a row of small empty dummy labels
      label = new wxStaticText(scrolledWindow, wxID_ANY, "", wxDefaultPosition, this->FromDIP(wxSize(5, 5)));
      varSizer->Add(label);
      label = new wxStaticText(scrolledWindow, wxID_ANY, "", wxDefaultPosition, this->FromDIP(wxSize(5, 5)));
      varSizer->Add(label);
    }

    st = wxString(tract->param[i].name) + " (" + wxString(tract->param[i].abbr) + ")";
    label = new wxStaticText(scrolledWindow, wxID_ANY, st);
    varSizer->Add(label);

    txtValue[i] = new wxTextCtrl(scrolledWindow, IDT_VALUE_0 + i, "", wxDefaultPosition, this->FromDIP(wxSize(60, -1)));
    txtValue[i]->Connect(-1, wxEVT_KILL_FOCUS, 
      wxFocusEventHandler(VocalTractShapesDialog::OnValueLostFocus), NULL, this);
    varSizer->Add(txtValue[i]);
  }

  topLevelSizer->AddSpacer(5);

  // ****************************************************************
  // Bottom part.
  // ****************************************************************

  wxBoxSizer *bottomSizer = new wxBoxSizer(wxHORIZONTAL);

  button = new wxButton(scrolledWindow, IDB_MOVE_UP, "Move up");
  bottomSizer->Add(button, 0, wxALL, 3);

  button = new wxButton(scrolledWindow, IDB_MOVE_DOWN, "Move down");
  bottomSizer->Add(button, 0, wxALL, 3);

  button = new wxButton(scrolledWindow, IDB_SELECT, "Select");
  button->SetDefault();
  bottomSizer->Add(button, 0, wxALL, 3);

  topLevelSizer->Add(bottomSizer, 0, wxGROW);

  // ****************************************************************

  bottomSizer = new wxBoxSizer(wxHORIZONTAL);

  button = new wxButton(scrolledWindow, IDB_ADD, "Add");
  bottomSizer->Add(button, 0, wxALL, 3);

  button = new wxButton(scrolledWindow, IDB_REPLACE, "Replace");
  bottomSizer->Add(button, 0, wxALL, 3);

  button = new wxButton(scrolledWindow, IDB_DELETE, "Delete");
  bottomSizer->Add(button, 0, wxALL, 3);

  button = new wxButton(scrolledWindow, IDB_RENAME, "Rename");
  bottomSizer->Add(button, 0, wxALL, 3);

  topLevelSizer->Add(bottomSizer, 0, wxGROW);
  topLevelSizer->AddSpacer(10);

  // ****************************************************************
  // Set the top-level-sizer for this window.
  // ****************************************************************

  scrolledWindow->SetSizerAndFit(topLevelSizer);
  scrolledWindow->SetScrollRate(0, 10);   // Make a vertical scrollbar only

  this->SetSizer(scrolledWindowSizer);
  scrolledWindowSizer->Fit(this);
}


// ****************************************************************************
/// Prints the data for the shape with the given index in the internal shape
/// list into the text controls.
/// \param selection Index of the shape in the internal shape list.
// ****************************************************************************

void VocalTractShapesDialog::outputShapeVars(int selection)
{
  int i;
  bool enable;

  if ((selection < 0) || (selection >= (int)tract->shapes.size())) 
  { 
    // Deactivate all controls 
    for (i=0; i < VocalTract::NUM_PARAMS; i++)
    {
      txtValue[i]->SetLabel("");
      txtValue[i]->Disable();
    }
    return; 
  }

  VocalTract::Shape *s = &(tract->shapes[selection]);
  bool isVowel = tract->isVowelShapeName(s->name);

  for (i=0; i < VocalTract::NUM_PARAMS; i++)
  {
    enable = true;

    if ((tract->anatomy.automaticTongueRootCalc) &&
        ((i == VocalTract::TRX) || (i == VocalTract::TRY)))
    {
      enable = false;
    }

    if (enable)
    {
      txtValue[i]->Enable();
    }
    else
    {
      txtValue[i]->Disable();
    }

    txtValue[i]->SetValue( wxString::Format("%2.2f", s->param[i]) );
  }
}

// ****************************************************************************
/// Selects the shape in the GUI-list, that has the given index in the internal
/// shape list.
// ****************************************************************************

void VocalTractShapesDialog::selectShape(int index)
{
  if ((index < 0) || (index >= (int)tract->shapes.size()))
  {
    return;
  }
  lstShapes->Select(index);
}

// ****************************************************************************
/// Returns the index of the selected shape or -1, when no shape is selected.
// ****************************************************************************

int VocalTractShapesDialog::getSelectedShape()
{
  int x = lstShapes->GetSelection();
  if (x == wxNOT_FOUND)
  {
    x = -1;
  }
  return x;
}

// ****************************************************************************
// ****************************************************************************

void VocalTractShapesDialog::updateVocalTract()
{
  int i;
  int sel = getSelectedShape();
  if (sel == -1) 
  { 
    return; 
  }

  for (i=0; i < VocalTract::NUM_PARAMS; i++)
  {
    tract->param[i].x = tract->shapes[sel].param[i];
  }
  tract->calculateAll();
  data->updateTlModelGeometry(tract);

  // request reloading the 3d geometry
  Acoustic3dSimulation * simu3d = Acoustic3dSimulation::getInstance();
  simu3d->requestReloadGeometry();
  simu3d->setGeometryImported(false);
  if (simu3d->contInterpMeth() == FROM_FILE)
  {
    simu3d->setContourInterpolationMethod(AREA);
  }

  // Refresh the pictures of the parent window.
  if (updateRequestReceiver1 != NULL)
  {
    wxCommandEvent event(updateRequestEvent);
    event.SetInt(UPDATE_VOCAL_TRACT);
    wxPostEvent(updateRequestReceiver1, event);
  }
  if (updateRequestReceiver2 != NULL)
  {
    wxCommandEvent event(updateRequestEvent);
    event.SetInt(UPDATE_VOCAL_TRACT);
    wxPostEvent(updateRequestReceiver2, event);
  }
}


// ****************************************************************************
// ****************************************************************************

void VocalTractShapesDialog::OnAdd(wxCommandEvent &event)
{
  int i;

  wxString targetName = wxGetTextFromUser("Please enter a name for the new shape!", "Add a shape", "", this);
  if (targetName == "")
  {
    return;
  }

  // Create the data for the new shape.
    
  VocalTract::Shape s;
  s.name = targetName.ToStdString();
  for (i=0; i < VocalTract::NUM_PARAMS; i++)
  {
    s.param[i] = tract->param[i].x;
  }

  int index = tract->getShapeIndex( targetName.ToStdString() );

  if (index != -1)
  {
    if (wxMessageBox("A shape with the same name is already in the list. Shall I overwrite it?",
      "Question", wxYES_NO, this) == wxYES)
    {
      tract->shapes[index] = s;
    }
  }
  else
  {
    // Put the new shape just before the currently selected item.
    int sel = getSelectedShape();   // Current selection in the list

    if (sel == -1)
    {
      // Append the item at the end.
      tract->shapes.push_back(s);
      index = (int)tract->shapes.size() - 1;
    }
    else
    {
      tract->shapes.insert( tract->shapes.begin() + sel, s );
      index = sel;
    }
  }

  // Refill the list and select the new item
  fillShapeList();
  selectShape(index);
  outputShapeVars(index);
}

// ****************************************************************************
// ****************************************************************************

void VocalTractShapesDialog::OnReplace(wxCommandEvent &event)
{
  int i;
  int sel = getSelectedShape();

  if (sel == -1)
  {
    wxMessageBox("You must choose the shape to be replaced with the current vocal tract shape!", 
      "Attention!", wxICON_INFORMATION, this);
  }
  else
  {
    if (wxMessageBox(wxString("Are you sure you want to replace the shape of '") +
      wxString( tract->shapes[sel].name ) + "'?",
      "Question", wxYES_NO, this) == wxYES)
    {
      for (i=0; i < VocalTract::NUM_PARAMS; i++)
      {
        tract->shapes[sel].param[i] = tract->param[i].x;              
      }
      outputShapeVars(sel);
    }
  }
}

// ****************************************************************************
// ****************************************************************************

void VocalTractShapesDialog::OnDelete(wxCommandEvent &event)
{
  int sel = getSelectedShape();

  if (sel == -1)
  {
    wxMessageBox("You must select a shape in order to delete it!", 
      "Attention!", wxICON_INFORMATION, this);
  }
  else
  if (wxMessageBox("Are you sure?", "Delete shape", wxYES_NO, this) == wxYES)
  {
    tract->shapes.erase( tract->shapes.begin() + sel );
    fillShapeList();
    // Select the next in the list.
    selectShape(sel);
  }
}

// ****************************************************************************
// ****************************************************************************

void VocalTractShapesDialog::OnRename(wxCommandEvent &event)
{
  int sel = getSelectedShape();

  if (sel != -1)
  {
    wxString newName = wxGetTextFromUser("Please enter a new name for the shape!", "Enter a name", 
      wxString(tract->shapes[sel].name), this );

    if (newName != "")
    {
      tract->shapes[sel].name = newName.ToStdString();
      fillShapeList();
      selectShape(sel);
    }
  }
  else
  {
    wxMessageBox("A shape must be selected!", "Attention!", wxICON_INFORMATION, this);
  }
}

// ****************************************************************************
// ****************************************************************************

void VocalTractShapesDialog::OnSelect(wxCommandEvent &event)
{
  updateVocalTract();
}


// ****************************************************************************
/// Move the selected shape one position up in the list.
// ****************************************************************************

void VocalTractShapesDialog::OnMoveItemUp(wxCommandEvent &event)
{
  int index = getSelectedShape();
  if ((index != -1) && (index > 0))
  {
    VocalTract::Shape s = tract->shapes[index - 1];
    tract->shapes[index - 1] = tract->shapes[index];
    tract->shapes[index] = s;
    fillShapeList();
    selectShape(index - 1);
  }
}


// ****************************************************************************
/// Move the selected shape one position down in the list.
// ****************************************************************************

void VocalTractShapesDialog::OnMoveItemDown(wxCommandEvent &event)
{
  int index = getSelectedShape();
  if ((index != -1) && (index < (int)tract->shapes.size() - 1))
  {
    VocalTract::Shape s = tract->shapes[index + 1];
    tract->shapes[index + 1] = tract->shapes[index];
    tract->shapes[index] = s;
    fillShapeList();
    selectShape(index + 1);
  }
}


// ****************************************************************************
/// Parse the string in the text box that lost focus for the variable value.
// ****************************************************************************

void VocalTractShapesDialog::OnValueLostFocus(wxFocusEvent &event)
{
  int id = event.GetId();
  int k = id - IDT_VALUE_0;     // Index of the text control
  int sel = getSelectedShape();
  
  if ((k < 0) || (k >= VocalTract::NUM_PARAMS) ||
    (sel < 0) || (sel >= (int)tract->shapes.size())) 
  {
    return;
  }

  wxString text = txtValue[k]->GetValue();
  double x;
  if (text.ToDouble(&x))
  {
    if (x < tract->param[k].min) { x = tract->param[k].min; }
    if (x > tract->param[k].max) { x = tract->param[k].max; }
    tract->shapes[sel].param[k] = x;
  }

  x = tract->shapes[sel].param[k];
  txtValue[k]->SetValue(wxString::Format("%2.2f", x));

  // Important: Also call the base class handler.
  event.Skip();
}


// ****************************************************************************
// ****************************************************************************

void VocalTractShapesDialog::OnItemSelected(wxCommandEvent &event)
{
  outputShapeVars( getSelectedShape() );
}

// ****************************************************************************
// ****************************************************************************

void VocalTractShapesDialog::OnItemActivated(wxCommandEvent &event)
{
  updateVocalTract();
}

// ****************************************************************************
/// Play the vowel when space is pressed in the list box.
// ****************************************************************************

void VocalTractShapesDialog::OnListKeyDown(wxKeyEvent &event)
{
  if (event.GetKeyCode() == WXK_SPACE)
  {
    updateVocalTract();
    wxYield();

    Glottis* g = data->glottis[Data::GEOMETRIC_GLOTTIS];
    int numGlottisParams = (int)g->controlParam.size();
    vector<double> audio;
    int i;

    // Store the current glottis model parameters.

    double storedGlottisParams[Glottis::MAX_CONTROL_PARAMS];
    for (i = 0; i < numGlottisParams; i++)
    {
      storedGlottisParams[i] = g->controlParam[i].x;
    }

    // Set the glottis model parameters for either
    // a voiceless fricative or for modal phonation.

    Glottis::Shape* s = NULL;
    int sel = getSelectedShape();   // The vocal tract shape

    if (sel != -1)
    {
      string name = tract->shapes[sel].name;
      if (name.find("fricative") != std::string::npos) 
      {
        s = g->getShape("voiceless-fricative");
      }
      else
      {
        s = g->getShape("modal");
      }
    }

    for (i = 0; i < numGlottisParams; i++)
    {
      if (s != NULL)
      {
        g->controlParam[i].x = s->controlParam[i];
      }
      else
      {
        g->controlParam[i].x = g->controlParam[i].neutral;
      }
    }
    g->controlParam[Glottis::FREQUENCY].x = 110.0;

    // Do the actual synthesis.

    Synthesizer::synthesizeStaticPhoneme(g, tract,
      data->tdsModel, false, true, audio);

    data->track[Data::MAIN_TRACK]->setZero();
    Synthesizer::copySignal(audio, *data->track[Data::MAIN_TRACK], 0);
    data->normalizeAudioAmplitude(Data::MAIN_TRACK);

    int duration_ms = (double)audio.size() * 1000.0 / (double)SAMPLING_RATE;

    if (waveStartPlaying(data->track[Data::MAIN_TRACK]->x, data->track[Data::MAIN_TRACK]->N, false))
    {
      wxMilliSleep(duration_ms);
      waveStopPlaying();
    }
    else
    {
      wxMessageBox("Playing failed.", "Attention!");
    }

    // Restore the glottis model parameters.

    for (i = 0; i < numGlottisParams; i++)
    {
      g->controlParam[i].x = storedGlottisParams[i];
    }
  }

  event.Skip();
}

// ****************************************************************************
/// The dialog is just closed.
// ****************************************************************************

void VocalTractShapesDialog::OnClose(wxCloseEvent &event)
{
  event.Skip();
}

// ****************************************************************************

