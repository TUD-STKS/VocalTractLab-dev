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

#include "SilentMessageBox.h"

// ****************************************************************************
/// Constructor.
// ****************************************************************************

SilentMessageBox::SilentMessageBox(const wxString& message, 
  const wxString& caption, wxWindow *parent) : wxDialog(parent, wxID_ANY, 
  caption, wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE & ~wxCLOSE_BOX)
{
  // Init the widgets.
  wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);

  wxStaticText *labText = new wxStaticText(this, wxID_ANY, message);
  sizer->Add(labText, 0, wxALL, 10);

  wxButton *btnOk = new wxButton(this, wxID_OK, "OK");
  sizer->Add(btnOk, 0, wxALL | wxALIGN_CENTER, 10);

  this->SetSizer(sizer);
  sizer->Fit(this);
  sizer->SetSizeHints(this);
}

// ****************************************************************************
