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

#ifndef __BASIC_PICTURE_H__
#define __BASIC_PICTURE_H__

#include <wx/wx.h>
#include <wx/dcbuffer.h>

// ****************************************************************************
// ****************************************************************************

class BasicPicture : public wxPanel
{
  // **************************************************************************
  // Public functions.
  // **************************************************************************

public:
  BasicPicture(wxWindow *parent);
  virtual void draw(wxDC &dc);
  wxBitmap *getBitmap();

  // **************************************************************************
  // Private data.
  // **************************************************************************

private:
  /// Background bitmap of the picture for double-buffered painting
  wxBitmap *bitmap;    

  // **************************************************************************
  // Private functions.
  // **************************************************************************

private:
  void OnPaint(wxPaintEvent &event);
  void OnEraseBackground(wxEraseEvent &event);

  // ****************************************************************************
  // Declare the event table right at the end
  // ****************************************************************************

  DECLARE_EVENT_TABLE()
};

#endif
