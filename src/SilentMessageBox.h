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

#ifndef __SILENT_MESSAGE_BOX__
#define __SILENT_MESSAGE_BOX__

#include <wx/wx.h>
#include <wx/dialog.h>

// ****************************************************************************
/// This dialog displays a message box dialog analogue to ::wxMessageBox(...),
/// but WITHOUT playing a sound when it pops up.
// ****************************************************************************

class SilentMessageBox : public wxDialog
{
  // **************************************************************************
  // Public functions.
  // **************************************************************************

public:
  SilentMessageBox(const wxString& message, const wxString& caption = "Message", 
    wxWindow *parent = NULL);
};

#endif

// ****************************************************************************
