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

  void printCells(wxDC& dc);

private:

  wxCoord m_cellWidth;
  wxCoord m_cellHeight;
  wxCoord m_cellSpacing;
  int m_nbCols;

  vector<cell> m_cells;

};



#endif
