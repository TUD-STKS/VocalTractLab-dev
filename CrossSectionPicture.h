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

#ifndef __CROSS_SECTION_PICTURE_H__
#define __CROSS_SECTION_PICTURE_H__

#include "BasicPicture.h"
#include "VocalTractPicture.h"

// ****************************************************************************
// ****************************************************************************

class CrossSectionPicture : public BasicPicture
{
  // **************************************************************************
  // Public functions.
  // **************************************************************************

public:
  CrossSectionPicture(wxWindow *parent, VocalTractPicture *picVocalTract);
  virtual void draw(wxDC &dc);

  // **************************************************************************
  // Private data.
  // **************************************************************************

private:
  VocalTractPicture *picVocalTract;
  int lineWidth{ this->FromDIP(1) };
};

#endif

// ****************************************************************************
