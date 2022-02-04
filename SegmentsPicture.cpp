#include "SegmentsPicture.h"

// ****************************************************************************
/// Constructor. Passes the parent parameter.
// ****************************************************************************

SegmentsPicture::SegmentsPicture(wxWindow* parent, Acoustic3dSimulation* simu3d)
  : BasicPicture(parent)
{
  this->m_simu3d = simu3d;
}

// ****************************************************************************
/// Draws the picture.
// ****************************************************************************

void SegmentsPicture::draw(wxDC& dc)
{
  // Clear the background.
  dc.SetBackground(*wxWHITE_BRUSH);
  dc.Clear();

  dc.SetPen(*wxBLACK_PEN);
  dc.SetBackgroundMode(wxTRANSPARENT);
  dc.SetFont(wxFont(9, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
  dc.DrawText("No geometry loaded.", 0, 0);
}