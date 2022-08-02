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

#include "VocalTractDialog.h"
#include <wx/statline.h>
#include <wx/filename.h>
#include "VocalTractShapesDialog.h"

// IDs of the controls

static const int IDI_VOCAL_TRACT                  = 6000;
static const int IDB_SHOW_HIDE_CONTROLS           = 6001;

static const int IDS_TONGUE_SIDE_ELEVATION_0      = 6170;
static const int IDC_AUTOMATIC_TONGUE_ROOT_CALC   = 6190;

static const int IDR_SHOW_NO_TRACT                = 6250;
static const int IDR_SHOW_2D_TRACT                = 6251;
static const int IDR_SHOW_3D_TRACT                = 6252;
static const int IDR_SHOW_WIRE_FRAME_TRACT        = 6253;
static const int IDC_SHOW_BOTH_SIDES              = 6254;

static const int IDC_SHOW_CONTROL_POINTS	    		= 4100;
static const int IDC_SHOW_CENTER_LINE			      	= 4101;
static const int IDC_SHOW_CUT_VECTORS		      		= 4102;
static const int IDC_SMOOTH_CENTER_LINE			    	= 4106;
static const int IDC_SHOW_EMA_POINTS            	= 4107;
static const int IDB_EDIT_EMA_POINTS      	    	= 4108;

static const int IDB_LOAD_BACKGROUND_IMAGE        = 4110;
static const int IDB_CURRENT_IMAGE_TO_BACKGROUND  = 4111;
static const int IDC_SHOW_BACKGROUND_IMAGE        = 4112;
static const int IDC_BACKGROUND_IMAGE_EDITING_MODE = 4113;

static const int NUM_SCROLLBAR_STEPS = 500;

// The single instance of this class.

VocalTractDialog *VocalTractDialog::instance = NULL;

// ****************************************************************************
// The event table.
// ****************************************************************************

BEGIN_EVENT_TABLE(VocalTractDialog, wxDialog)
  // Custom event handler for update requests by child widgets.
  EVT_COMMAND(wxID_ANY, updateRequestEvent, VocalTractDialog::OnUpdateRequest)

  EVT_BUTTON(IDB_SHOW_HIDE_CONTROLS, VocalTractDialog::OnShowHideControls)

  EVT_COMMAND_SCROLL(IDS_TONGUE_SIDE_ELEVATION_0, VocalTractDialog::OnTongueSideElevationChanged)
  EVT_COMMAND_SCROLL(IDS_TONGUE_SIDE_ELEVATION_0 + 1, VocalTractDialog::OnTongueSideElevationChanged)
  EVT_COMMAND_SCROLL(IDS_TONGUE_SIDE_ELEVATION_0 + 2, VocalTractDialog::OnTongueSideElevationChanged)

  EVT_CHECKBOX(IDC_AUTOMATIC_TONGUE_ROOT_CALC, VocalTractDialog::OnAutomaticTongueRootCalc)

  EVT_RADIOBUTTON(IDR_SHOW_NO_TRACT, VocalTractDialog::OnShowNoTract)
  EVT_RADIOBUTTON(IDR_SHOW_2D_TRACT, VocalTractDialog::OnShow2dTract)
  EVT_RADIOBUTTON(IDR_SHOW_3D_TRACT, VocalTractDialog::OnShow3dTract)
  EVT_RADIOBUTTON(IDR_SHOW_WIRE_FRAME_TRACT, VocalTractDialog::OnShowWireFrameTract)
  EVT_CHECKBOX(IDC_SHOW_BOTH_SIDES, VocalTractDialog::OnShowBothSides)

  EVT_CHECKBOX(IDC_SHOW_CONTROL_POINTS, VocalTractDialog::OnShowControlPoints)
  EVT_CHECKBOX(IDC_SHOW_CENTER_LINE, VocalTractDialog::OnShowCenterLine)
  EVT_CHECKBOX(IDC_SHOW_CUT_VECTORS, VocalTractDialog::OnShowCutVectors)
  EVT_CHECKBOX(IDC_SMOOTH_CENTER_LINE, VocalTractDialog::OnShowSmoothCenterLine)

  EVT_CHECKBOX(IDC_SHOW_EMA_POINTS, VocalTractDialog::OnShowEmaPoints)
  EVT_BUTTON(IDB_EDIT_EMA_POINTS, VocalTractDialog::OnEditEmaPoints)

  EVT_BUTTON(IDB_LOAD_BACKGROUND_IMAGE, VocalTractDialog::OnLoadBackgroundImage)
  EVT_BUTTON(IDB_CURRENT_IMAGE_TO_BACKGROUND, VocalTractDialog::OnCurrentImageToBackground)

  EVT_CHECKBOX(IDC_SHOW_BACKGROUND_IMAGE, VocalTractDialog::OnShowBackgroundImage)
  EVT_CHECKBOX(IDC_BACKGROUND_IMAGE_EDITING_MODE, VocalTractDialog::OnBackgroundImageEditingMode)
END_EVENT_TABLE()


// ****************************************************************************
/// Returns the single instance of this dialog.
// ****************************************************************************

VocalTractDialog *VocalTractDialog::getInstance(wxWindow *parent)
{
  if (instance == NULL)
  {
    instance = new VocalTractDialog(parent);
  }

  return instance;
}


// ****************************************************************************
// ****************************************************************************

VocalTractPicture *VocalTractDialog::getVocalTractPicture()
{
  return picVocalTract;
}


// ****************************************************************************
/// Sets the pointer to the vocal tract page to send update messages to for
/// the vocal tract related pictures.
// ****************************************************************************

void VocalTractDialog::setUpdateRequestReceiver(wxWindow* updateRequestReceiver1,
  wxWindow* updateRequestReceiver2)
{
  this->updateRequestReceiver1 = updateRequestReceiver1;
  this->updateRequestReceiver2 = updateRequestReceiver2;
}


// ****************************************************************************
/// Update the widgets.
// ****************************************************************************

void VocalTractDialog::updateWidgets()
{
  int i;
  VocalTract *tract = data->vocalTract;

  // Repaint the picture.
  picVocalTract->Refresh();
  picVocalTract->Update();

  // ****************************************************************
  // ****************************************************************

  if (controlPanel->IsShown())
  {
    showHideControlsButton->SetLabel("-");

    // **************************************************************
    // Scrollbar and label for tongue side heights.
    // **************************************************************

    double x, min, max, pos;

    for (i=0; i < NUM_TONGUE_SIDE_POINTS; i++)
    {
      x = tract->param[VocalTract::TS1+i].x;
      min = tract->param[VocalTract::TS1+i].min;
      max = tract->param[VocalTract::TS1+i].max;
      pos = (x - min)*(double)NUM_SCROLLBAR_STEPS / (max - min);

      scrTongueSide[i]->SetThumbPosition( (int)(pos + 0.5) );
      labTongueSide[i]->SetLabel( wxString::Format("%4.2f", x) );
    }

    // **************************************************************
    // Check box for automatic TRX, TRY calculation.
    // **************************************************************
    
    chkAutomaticTongueRootCalc->SetValue(tract->anatomy.automaticTongueRootCalc);

    // **************************************************************
    // Radio buttons for the render mode of the vocal tract.
    // **************************************************************

    switch (picVocalTract->renderMode)
    {
    case VocalTractPicture::RM_NONE: 
      radNoTract->SetValue(true);
      break;
  
    case VocalTractPicture::RM_2D: 
      rad2dTract->SetValue(true);
      break;
  
    case VocalTractPicture::RM_3DSOLID: 
      rad3dTract->SetValue(true);
      break;
  
    case VocalTractPicture::RM_3DWIRE: 
      radWireFrameTract->SetValue(true);
      break;
    }

    chkBothSides->SetValue(picVocalTract->renderBothSides);

    chkShowControlPoints->SetValue( picVocalTract->showControlPoints );
    chkShowCutVectors->SetValue( picVocalTract->showCutVectors );
    chkShowCenterLine->SetValue( picVocalTract->showCenterLine );
    chkSmoothCenterLine->SetValue( !picVocalTract->isRoughCenterLine );
    chkShowEmaPoints->SetValue( picVocalTract->showEmaPoints );

    if (data->backgroundImageFileName.empty())
    {
      labBackgroundImageFileName->SetLabel("No image loaded.");    
    }
    else
    {
      wxFileName name(data->backgroundImageFileName);
      labBackgroundImageFileName->SetLabel( name.GetFullName() );
    }

    chkShowBackgroundImage->SetValue( picVocalTract->showPoster );
    chkBackgroundImageEditMode->SetValue( picVocalTract->posterEditing );
  }
  else
  {
    showHideControlsButton->SetLabel("+");
  }

}


// ****************************************************************************
/// Private constructor.
/// This dialog must be constructed with the flag wxSTAY_ON_TOP, because due to
/// its early initialization (before mainWindow is fully created), the dialog
/// does not automatically stay on top of its parent window, as the other
/// modeless dialogs.
// ****************************************************************************

VocalTractDialog::VocalTractDialog(wxWindow *parent) :
wxDialog(parent, wxID_ANY, wxString("Vocal tract"),
  wxDefaultPosition, wxDefaultSize,
  wxDEFAULT_DIALOG_STYLE | wxSTAY_ON_TOP | wxRESIZE_BORDER)
{
  // Do NOT enable double buffering here, because the sometimes the OpenGl 
  // frame gets black (has its own double buffer?).
//  this->SetDoubleBuffered(true);

  this->Move(700, 100);

  // ****************************************************************
  // Init the variables first.
  // ****************************************************************

  data = Data::getInstance();
  updateRequestReceiver1 = NULL;
  updateRequestReceiver2 = NULL;

  // ****************************************************************
  // Init and update the widgets.
  // ****************************************************************

  initWidgets();
  updateWidgets();
}


// ****************************************************************************
/// Init the widgets.
// ****************************************************************************

void VocalTractDialog::initWidgets()
{
  topLevelSizer = new wxBoxSizer(wxVERTICAL);

  wxBoxSizer *sizer = NULL;
  wxButton *button = NULL;
  wxStaticLine *line = NULL;
  wxStaticText *label = NULL;
  int i;

  // Create an invisible dummy button with the ID wxID_CANCEL, so that
  // the user can close the dialog with ESC.
  button = new wxButton(this, wxID_CANCEL, "", wxPoint(0,0), wxSize(0,0));

  // ****************************************************************
  // Create the vocal tract picture and add it to the dialog.
  // ****************************************************************

  wxGLAttributes openGlArgs;
  openGlArgs.PlatformDefaults().Defaults().RGBA().DoubleBuffer().Depth(24).EndList(); // Depth(32) causes display error. Possible bug?
  picVocalTract = new VocalTractPicture(this, data->vocalTract, openGlArgs, this);
  
  picVocalTract->SetMinSize(this->FromDIP(wxSize(300, 300) ));

  picVocalTract->showControlPoints = true;
  picVocalTract->showCenterLine = false;

  topLevelSizer->Add(picVocalTract, 1, wxALL |wxGROW, 5);

  // ****************************************************************
  // Show the button to show/hide the controls.
  // ****************************************************************

  sizer = new wxBoxSizer(wxHORIZONTAL);
  showHideControlsButton = new wxButton(this, IDB_SHOW_HIDE_CONTROLS, "+", wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
  showHideControlsButton->SetMinSize(wxSize(showHideControlsButton->GetSize().GetHeight(), showHideControlsButton->GetSize().GetHeight()));
  sizer->Add(showHideControlsButton, 0, wxALL, 2);

  line = new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL);
  sizer->Add(line, 1, wxTOP | wxBOTTOM, 11);

  topLevelSizer->Add(sizer, 0, wxGROW | wxLEFT | wxRIGHT, 2);

  // ****************************************************************
  // Add a panel that contains all the controls.
  // ****************************************************************

  controlPanel = new wxPanel(this, wxID_ANY);
  controlSizer = new wxBoxSizer(wxVERTICAL);
  controlPanel->SetSizer(controlSizer);

  topLevelSizer->Add(controlPanel, 0, wxALL | wxGROW, 5);

  // ****************************************************************
  // Add the controls to the controlPanel with the controlSizer.
  // ****************************************************************

  // ****************************************************************
  // Add a grid sizer for the scroll bars to adjust some extra
  // vocal tract parameters.
  // ****************************************************************

  wxFlexGridSizer *gridSizer = new wxFlexGridSizer(3, 3, 3);  // cols, vgap, hgap

  label = new wxStaticText(controlPanel, wxID_ANY, "Tongue side elev. (-1 ... +1)");
  gridSizer->Add(label);

  label = new wxStaticText(controlPanel, wxID_ANY, "");
  gridSizer->Add(label);

  chkAutomaticTongueRootCalc = new wxCheckBox(controlPanel, IDC_AUTOMATIC_TONGUE_ROOT_CALC,
    "Automatic TRX,TRY calc.");
  gridSizer->Add(chkAutomaticTongueRootCalc, 1, wxGROW);

  for (i=0; i < NUM_TONGUE_SIDE_POINTS; i++)
  {
    scrTongueSide[i] = new wxScrollBar(controlPanel, IDS_TONGUE_SIDE_ELEVATION_0 + i);
    scrTongueSide[i]->SetScrollbar(0, 1, NUM_SCROLLBAR_STEPS, 1);
    labTongueSide[i] = new wxStaticText(controlPanel, wxID_ANY, "0", wxDefaultPosition, this->FromDIP(wxSize(50, -1)));
  }

  // The columns with the scroll bars are stretchable.
  gridSizer->AddGrowableCol(0, 1);
//  gridSizer->AddGrowableCol(2, 1);

  gridSizer->Add(scrTongueSide[0], 1, wxGROW); 
  gridSizer->Add(labTongueSide[0]);
  gridSizer->Add(new wxStaticText(controlPanel, wxID_ANY, "")); // A dummy label

  gridSizer->Add(scrTongueSide[1], 1, wxGROW); 
  gridSizer->Add(labTongueSide[1]);
  gridSizer->Add(new wxStaticText(controlPanel, wxID_ANY, "")); // A dummy label

  gridSizer->Add(scrTongueSide[2], 1, wxGROW); 
  gridSizer->Add(labTongueSide[2]);
  gridSizer->Add(new wxStaticText(controlPanel, wxID_ANY, "")); // A dummy label

  controlSizer->Add(gridSizer, 0, wxGROW);

  // ****************************************************************
  // Add a horizontal line and the display options.
  // ****************************************************************

  line = new wxStaticLine(controlPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL);
  controlSizer->Add(line, 1, wxGROW | wxTOP | wxBOTTOM, 5);

  // Vocal tract options panel

  sizer = new wxBoxSizer(wxHORIZONTAL);

  radNoTract = new wxRadioButton(controlPanel, IDR_SHOW_NO_TRACT, "X",
    wxDefaultPosition, wxDefaultSize, wxRB_GROUP);
  sizer->Add(radNoTract, 0, wxALL, 5);
  
  rad2dTract = new wxRadioButton(controlPanel, IDR_SHOW_2D_TRACT, "2D");
  sizer->Add(rad2dTract, 0, wxALL, 5);
  
  rad3dTract = new wxRadioButton(controlPanel, IDR_SHOW_3D_TRACT, "3D");
  sizer->Add(rad3dTract, 0, wxALL, 5);
  
  radWireFrameTract = new wxRadioButton(controlPanel, IDR_SHOW_WIRE_FRAME_TRACT, "Wire frame");
  sizer->Add(radWireFrameTract, 0, wxALL, 5);
  
  chkBothSides = new wxCheckBox(controlPanel, IDC_SHOW_BOTH_SIDES, "Both sides");
  sizer->Add(chkBothSides, 0, wxALL, 5);

  controlSizer->Add(sizer);

  // ****************************************************************
  // Add additional options.
  // ****************************************************************

  sizer = new wxBoxSizer(wxHORIZONTAL);
  chkShowControlPoints = new wxCheckBox(controlPanel, IDC_SHOW_CONTROL_POINTS, "Show control points");
  sizer->Add(chkShowControlPoints, 0, wxALL, 5);
  chkShowCutVectors = new wxCheckBox(controlPanel, IDC_SHOW_CUT_VECTORS, "Show cut vectors");
  sizer->Add(chkShowCutVectors, 0, wxALL, 5);
  controlSizer->Add(sizer);

  sizer = new wxBoxSizer(wxHORIZONTAL);
  chkShowCenterLine = new wxCheckBox(controlPanel, IDC_SHOW_CENTER_LINE, "Show center line");
  sizer->Add(chkShowCenterLine, 0, wxALL, 5);
  chkSmoothCenterLine = new wxCheckBox(controlPanel, IDC_SMOOTH_CENTER_LINE, "smooth");
  sizer->Add(chkSmoothCenterLine, 0, wxALL, 5);
  controlSizer->Add(sizer);

  sizer = new wxBoxSizer(wxHORIZONTAL);
  chkShowEmaPoints = new wxCheckBox(controlPanel, IDC_SHOW_EMA_POINTS, "Show EMA points");
  sizer->Add(chkShowEmaPoints, 0, wxALL, 5);
  button = new wxButton(controlPanel, IDB_EDIT_EMA_POINTS, "Edit EMA points");
  sizer->Add(button, 0, wxALL, 2);
  controlSizer->Add(sizer);

  // ****************************************************************

  sizer = new wxBoxSizer(wxHORIZONTAL);
  button = new wxButton(controlPanel, IDB_LOAD_BACKGROUND_IMAGE, "Load background image");
  sizer->Add(button, 0, wxALL, 3);
  button = new wxButton(controlPanel, IDB_CURRENT_IMAGE_TO_BACKGROUND, "Current image to background");
  sizer->Add(button, 0, wxALL, 3);
  controlSizer->Add(sizer);

  labBackgroundImageFileName = new wxStaticText(controlPanel, wxID_ANY, "No file loaded");
  controlSizer->Add(labBackgroundImageFileName, 0, wxALL, 5);

  sizer = new wxBoxSizer(wxHORIZONTAL);
  chkShowBackgroundImage = new wxCheckBox(controlPanel, IDC_SHOW_BACKGROUND_IMAGE, "Show background image");
  sizer->Add(chkShowBackgroundImage, 0, wxALL, 5);
  chkBackgroundImageEditMode = new wxCheckBox(controlPanel, IDC_BACKGROUND_IMAGE_EDITING_MODE, "Background image editing");
  sizer->Add(chkBackgroundImageEditMode, 0, wxALL, 5);
  controlSizer->Add(sizer);

  
  // ****************************************************************
  // Set the sizer for this dialog.
  // The dialog should never be resized to a size smaller than the
  // picture min size but with the control panel hidden.
  // ****************************************************************

  this->SetSizer(topLevelSizer);

  controlPanel->Hide();
  topLevelSizer->Fit(this);
  minSizeWithoutControls = topLevelSizer->GetMinSize();

  controlPanel->Show();
  topLevelSizer->Fit(this);
  minSizeWithControls = topLevelSizer->GetMinSize();

  // Set the minimum size of this dialog.
  this->SetMinSize(minSizeWithControls);

  // Set the size to start up with.
  picVocalTract->SetMinSize(this->FromDIP(wxSize(400, 400)));
  topLevelSizer->Fit(this);
}



// ****************************************************************************
// ****************************************************************************

void VocalTractDialog::updateVocalTractPage()
{
  // request reload 3d geometry and recompute modes and junction
  Acoustic3dSimulation* simu3d = Acoustic3dSimulation::getInstance();
  simu3d->requestReloadGeometry();
  simu3d->requestModesAndJunctionComputation();
  simu3d->setGeometryImported(false);
  if (simu3d->contInterpMeth() == FROM_FILE)
  {
    simu3d->setContourInterpolationMethod(AREA);
  }

  // refresh the pictures
  if (updateRequestReceiver1 != NULL)
  {
     wxCommandEvent event(updateRequestEvent);
     event.SetInt(UPDATE_PICTURES_AND_CONTROLS);
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
/// This is for update requests sent by the vocal tract picture in response
/// to changes of vocal tract parameters.
// ****************************************************************************

void VocalTractDialog::OnUpdateRequest(wxCommandEvent &event)
{
  data->updateTlModelGeometry(data->vocalTract);
  updateWidgets();
  updateVocalTractPage();
}


// ****************************************************************************
/// Toggle the visibility of the control panel below the vocal tract picture
/// and thereby leave the size of the picture as it is.
// ****************************************************************************

void VocalTractDialog::OnShowHideControls(wxCommandEvent &event)
{
  if (controlPanel->IsShown())
  {
    controlPanel->Hide();
    wxSize minSize  = picVocalTract->GetMinSize();
    wxSize currSize = picVocalTract->GetSize();
    picVocalTract->SetMinSize(currSize);
    this->SetMinSize(minSizeWithoutControls);
    topLevelSizer->Fit(this);
    picVocalTract->SetMinSize(minSize);
    this->Refresh();
  }
  else
  {
    controlPanel->Show();
    wxSize minSize  = picVocalTract->GetMinSize();
    wxSize currSize = picVocalTract->GetSize();
    picVocalTract->SetMinSize(currSize);
    this->SetMinSize(minSizeWithControls);
    topLevelSizer->Fit(this);
    picVocalTract->SetMinSize(minSize);
    this->Refresh();
  }

  updateWidgets();
}


// ****************************************************************************
// ****************************************************************************

void VocalTractDialog::OnTongueSideElevationChanged(wxScrollEvent &event)
{
  int index = event.GetId() - IDS_TONGUE_SIDE_ELEVATION_0;
  if ((index < 0) || (index >= NUM_TONGUE_SIDE_POINTS))
  {
    return;
  }

  VocalTract *tract = data->vocalTract;

  int pos = scrTongueSide[index]->GetThumbPosition();
  double min = tract->param[VocalTract::TS1+index].min;
  double max = tract->param[VocalTract::TS1+index].max;
  tract->param[VocalTract::TS1+index].x = min + (max-min)*(double)pos / (double)NUM_SCROLLBAR_STEPS;

  tract->calculateAll();
  data->updateTlModelGeometry(tract);
  updateWidgets();
  updateVocalTractPage();
  wxYield();
}


// ****************************************************************************
/// Toggle automatic calculation of the tongue root parameters (TRX, TRY).
// ****************************************************************************

void VocalTractDialog::OnAutomaticTongueRootCalc(wxCommandEvent &event)
{
  VocalTract *tract = data->vocalTract;
  tract->anatomy.automaticTongueRootCalc = !tract->anatomy.automaticTongueRootCalc;
  
  tract->calculateAll();
  data->updateTlModelGeometry(tract);
  updateWidgets();
  updateVocalTractPage();

  VocalTractShapesDialog::getInstance()->updateWidgets();
}


// ****************************************************************************
// ****************************************************************************

void VocalTractDialog::OnShowNoTract(wxCommandEvent &event)
{
  picVocalTract->renderMode = VocalTractPicture::RM_NONE;
  picVocalTract->Refresh();
}

// ****************************************************************************
// ****************************************************************************

void VocalTractDialog::OnShow2dTract(wxCommandEvent &event)
{
  picVocalTract->renderMode = VocalTractPicture::RM_2D;
  picVocalTract->Refresh();
}

// ****************************************************************************
// ****************************************************************************

void VocalTractDialog::OnShow3dTract(wxCommandEvent &event)
{
  picVocalTract->renderMode = VocalTractPicture::RM_3DSOLID;
  picVocalTract->Refresh();
}

// ****************************************************************************
// ****************************************************************************

void VocalTractDialog::OnShowWireFrameTract(wxCommandEvent &event)
{
  picVocalTract->renderMode = VocalTractPicture::RM_3DWIRE;
  picVocalTract->Refresh();
}

// ****************************************************************************
// ****************************************************************************

void VocalTractDialog::OnShowBothSides(wxCommandEvent &event)
{
  picVocalTract->renderBothSides = !picVocalTract->renderBothSides;
  picVocalTract->Refresh();
}


// ****************************************************************************
// ****************************************************************************

void VocalTractDialog::OnShowControlPoints(wxCommandEvent &event)
{
  picVocalTract->showControlPoints = !picVocalTract->showControlPoints;
  updateWidgets();
}

// ****************************************************************************
// ****************************************************************************

void VocalTractDialog::OnShowCenterLine(wxCommandEvent &event)
{
  picVocalTract->showCenterLine = !picVocalTract->showCenterLine;
  picVocalTract->showPalatogramDivision = picVocalTract->showCenterLine;
  updateWidgets();
}

// ****************************************************************************
// ****************************************************************************

void VocalTractDialog::OnShowCutVectors(wxCommandEvent &event)
{
  picVocalTract->showCutVectors = !picVocalTract->showCutVectors;
  updateWidgets();
}


// ****************************************************************************
// ****************************************************************************

void VocalTractDialog::OnShowSmoothCenterLine(wxCommandEvent &event)
{
  picVocalTract->isRoughCenterLine = !picVocalTract->isRoughCenterLine;
  updateWidgets();
}


// ****************************************************************************
// ****************************************************************************

void VocalTractDialog::OnShowEmaPoints(wxCommandEvent &event)
{
  picVocalTract->showEmaPoints = !picVocalTract->showEmaPoints;
  updateWidgets();
}


// ****************************************************************************
// ****************************************************************************

void VocalTractDialog::OnEditEmaPoints(wxCommandEvent &event)
{
  EmaConfigDialog *dialog = EmaConfigDialog::getInstance(this, this);
  dialog->Show(true);

  updateWidgets();
}


// ****************************************************************************
// ****************************************************************************

void VocalTractDialog::OnLoadBackgroundImage(wxCommandEvent &event)
{
  wxFileName fileName(data->backgroundImageFileName);

  wxString name = wxFileSelector("Load background image", fileName.GetPath(), 
    fileName.GetFullName(), "*.*", 
    "All files (*.*)|*.*|BMP files (*.bmp)|*.bmp|GIF files (*.gif)|*.gif|PNG files (*.png)|*.png", 
    wxFD_OPEN | wxFD_FILE_MUST_EXIST, this);

  if (name.IsEmpty() == false)
  {
    data->backgroundImageFileName = name;
    if (picVocalTract->loadPoster(name) == false)
    {
      wxMessageBox("Loading the image failed!", "Error!");
    }
    updateWidgets();
  }

}


// ****************************************************************************
// ****************************************************************************

void VocalTractDialog::OnCurrentImageToBackground(wxCommandEvent& event)
{
  picVocalTract->currentPictureToPoster();
  picVocalTract->showPoster = true;
  updateWidgets();
}


// ****************************************************************************
// ****************************************************************************

void VocalTractDialog::OnShowBackgroundImage(wxCommandEvent &event)
{
  picVocalTract->showPoster = !picVocalTract->showPoster;
  updateWidgets();
}


// ****************************************************************************
// ****************************************************************************

void VocalTractDialog::OnBackgroundImageEditingMode(wxCommandEvent &event)
{
  picVocalTract->posterEditing = !picVocalTract->posterEditing;
  updateWidgets();
}


// ****************************************************************************

