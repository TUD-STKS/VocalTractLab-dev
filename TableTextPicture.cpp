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

#include "TableTextPicture.h"
#include <fstream>
#include <iostream>

TableTextPicture::TableTextPicture()
{
  m_cellWidth = 200;
  m_cellHeight = 15;
  m_cellSpacing = 30;
  m_nbCols = 2;
}

// ****************************************************************************

TableTextPicture::TableTextPicture(wxCoord cellWidth, wxCoord cellHeight, wxCoord cellSpacing, 
  int nbCols)
{
  m_cellWidth = cellWidth;
  m_cellHeight = cellHeight;
  m_cellSpacing = cellSpacing;
  m_nbCols = nbCols;
}

// ****************************************************************************

void TableTextPicture::printCells(wxDC& dc)
{
  int idxCell(0), idxRow(0), idxCol(0);
  wxCoord w, h;
  cell* ce;

  dc.SetPen(*wxBLACK_PEN);

  while (idxCell < m_cells.size())
  {
    ce = &m_cells[idxCell];

    idxRow = idxCell / m_nbCols;
    idxCol = idxCell % m_nbCols;

    dc.DrawText(ce->label, idxCol * m_cellWidth, idxRow * m_cellHeight);
    dc.GetTextExtent(ce->value, &w, &h);
    dc.DrawText(ce->value, (idxCol + 1) * m_cellWidth - w - m_cellSpacing, idxRow * m_cellHeight);

    idxCell++;
  }
}
