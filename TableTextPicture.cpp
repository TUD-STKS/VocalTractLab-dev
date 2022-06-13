#include "TableTextPicture.h"
#include <fstream>
#include <iostream>

TableTextPicture::TableTextPicture()
{
  m_cellWidth = 170;
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