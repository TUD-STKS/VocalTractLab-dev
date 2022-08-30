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

#ifndef __MESH_PICTURE_H__
#define __MESH_PICTURE_H__

#include "BasicPicture.h"
#include "VocalTractPicture.h"
#include "SegmentsPicture.h"
#include "Backend/Acoustic3dSimulation.h"


// ****************************************************************************
// ****************************************************************************

enum objectToDisplay {
  MESH,
  TRANSVERSE_MODE,
  JUNCTION_MATRIX,
  ACOUSTIC_FIELD,
  CONTOUR
};

class PropModesPicture : public BasicPicture
{
// **************************************************************************
// Public functions.
// **************************************************************************

public:
	PropModesPicture(wxWindow* parent,
    Acoustic3dSimulation* simu3d, SegmentsPicture* segPic);

	virtual void draw(wxDC& dc);

  enum objectToDisplay getObjectDisplayed();
  void setObjectToDisplay(enum objectToDisplay object);
	void setModeIdx(int idx);

  // accessors
  bool meshSelected() const { return(m_objectToDisplay == MESH); }
  bool modeSelected() const { return(m_objectToDisplay == TRANSVERSE_MODE); }
  bool fSelected() const { return(m_objectToDisplay == JUNCTION_MATRIX); }
  bool fieldSelected() const { return(m_objectToDisplay == ACOUSTIC_FIELD); }
  int modeIdx() const { return(m_modeIdx); }
  void prevContourPosition();
  void nextContourPosition();

// **************************************************************************
// Private data.
// **************************************************************************

private:

  wxMenu* m_contextMenu;

  enum objectToDisplay m_objectToDisplay;
	int m_modeIdx;
  double m_zoom;
  int lineWidth1{ this->FromDIP(1) };
  int lineWidth2{ this->FromDIP(2) };
  double m_centerX;
  double m_centerY;
  int m_positionContour;
  vector<string> m_infoStr;
  vector<string> m_labelStr;
  vector<string> m_valueStr;

  Matrix m_field;

	Acoustic3dSimulation* m_simu3d;
  SegmentsPicture* m_segPic;

// **************************************************************************
// Private functions.
// **************************************************************************

private:

  double getScaling();
  void drawContour(int sectionIdx, vector<int>& surf, wxDC& dc);

  void OnMouseEvent(wxMouseEvent& event);
  void OnExportAcousticField(wxCommandEvent& event);
  void OnExportContour(wxCommandEvent& event);

// ****************************************************************************
// Declare the event table right at the end
// ****************************************************************************

  DECLARE_EVENT_TABLE()
};

#endif

// ****************************************************************************
