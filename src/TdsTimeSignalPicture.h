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

#ifndef __TDS_TIME_SIGNAL_PICTURE_H__
#define __TDS_TIME_SIGNAL_PICTURE_H__

#include "BasicPicture.h"
#include "Graph.h"
#include "Data.h"

// ****************************************************************************
// ****************************************************************************

class TdsTimeSignalPicture : public BasicPicture
{
  // **************************************************************************
  // Public data.
  // **************************************************************************

public:
  Graph flowGraph;
  Graph pressureGraph;
  Graph areaGraph;
  Graph velocityGraph;

  // **************************************************************************
  // Public functions.
  // **************************************************************************

public:
  TdsTimeSignalPicture(wxWindow *parent);
  virtual void draw(wxDC &dc);

  // **************************************************************************
  // Private data.
  // **************************************************************************

private:
  int lineWidth{ this->FromDIP(1) };
};

#endif

// ****************************************************************************
