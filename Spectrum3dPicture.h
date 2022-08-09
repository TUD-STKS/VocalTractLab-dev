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

#ifndef __SPECTRUM_3D_PICTURE_H__
#define __SPECTRUM_3D_PICTURE_H__

#include "BasicPicture.h"
#include "Graph.h"
#include "Backend/Acoustic3dSimulation.h"

class Spectrum3dPicture : public BasicPicture
{
  // **************************************************************************
  // Public data.
  // **************************************************************************

public:
  Graph graph;

  // **************************************************************************
  // Public functions.
  // **************************************************************************

public:
  Spectrum3dPicture(wxWindow *parent, Acoustic3dSimulation *simu3d);
  virtual void draw(wxDC &dc);
  void paintSpectrum(wxDC &dc);
  void setShowGlottalTf(bool show) { m_showGlottalTf = show; }
  void setShowNoiseTf(bool show) { m_showNoiseTf = show; }
  void setShowInputImped(bool show) { m_showInputImped = show; }
  void setIdxTfPoint(int idx) { m_idxPtTf = idx; }

  // accessors
  bool showGlottalTf() const { return m_showGlottalTf; }
  bool showNoise() const { return m_showNoiseTf; }
  bool showInputImped() const { return m_showInputImped; }
  
  // **************************************************************************
  // Private data.
  // **************************************************************************

private:

  wxMenu* m_contextMenu;

  int lineWidth{ this->FromDIP(1) };
  Acoustic3dSimulation *simu3d;
  bool m_showGlottalTf;
  bool m_showNoiseTf;
  bool m_showInputImped;

  // for acoustic field computation
  bool m_changeFreqField;
  int m_lastMousePosX;
  int m_lastMousePosY;

  int m_idxPtTf;

  // **************************************************************************
  // Private functions.
  // **************************************************************************

private:
  void drawTf(wxDC& dc, enum tfType type);
  void exportResult(enum tfType type);

  void OnMouseEvent(wxMouseEvent& event);
  void OnExportGlottalTf(wxCommandEvent& event);
  void OnEXportNoiseSrcTf(wxCommandEvent& event);
  void OnExportInputImpedance(wxCommandEvent& event);

// ****************************************************************************
// Declare the event table right at the end
// ****************************************************************************

  DECLARE_EVENT_TABLE()
};

#endif
