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

#ifndef __TABLE_TEXT_PICTURE_H__
#define __TABLE_TEXT_PICTURE_H__

#include <wx/wx.h>
#include <string>
#include <sstream>
#include <iomanip>

using namespace std;

struct cell
{
  string label;
  string value;
};

class TableTextPicture
{
public:

  TableTextPicture();
  TableTextPicture(wxCoord cellWidth, wxCoord cellHeight, wxCoord cellSpacing, int nbCols);

  template <typename T> void addCell(string label, T value)
  {
    ostringstream ost;
    cell cellToAdd;

    cellToAdd.label = label;

    ost << setprecision(2) << fixed;
    ost << value;

    cellToAdd.value = ost.str();

    m_cells.push_back(cellToAdd);
  };

  template <typename T> void setCell(int cellIdx, string label, T value)
  {
    ostringstream ost;
    cell cellToAdd;

    cellToAdd.label = label;

    ost << setprecision(2) << fixed;
    ost << value;

    cellToAdd.value = ost.str();

    m_cells[cellIdx] = cellToAdd;
  }

  void printCells(wxDC& dc);

private:

  wxCoord m_cellWidth;
  wxCoord m_cellHeight;
  wxCoord m_cellSpacing;
  int m_nbCols;

  vector<cell> m_cells;

};



#endif
