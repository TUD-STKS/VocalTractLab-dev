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

#include "CrossSectionPicture.h"
#include "Data.h"

// ****************************************************************************
/// Construcor. Passes the parent parameter.
// ****************************************************************************

CrossSectionPicture::CrossSectionPicture(wxWindow *parent, 
                       VocalTractPicture *picVocalTract) : BasicPicture(parent)
{
  this->picVocalTract = picVocalTract;
}


// ****************************************************************************
/// Draws the picture.
// ****************************************************************************

void CrossSectionPicture::draw(wxDC &dc)
{
  int width, height;
  double zoom;
  Tube::Articulator articulator;
  Data *data = Data::getInstance();
  VocalTract *tract = data->vocalTract;
  double pos = picVocalTract->cutPlanePos_cm;
  int i;

  // Clear the background.
  dc.SetBackground(*wxWHITE_BRUSH);
  dc.Clear();

  // Safety check.

  if ((pos < 0.0) || (pos > tract->centerLineLength))
  {
    dc.SetPen(wxPen(*wxBLACK, lineWidth));
    dc.SetBackgroundMode(wxTRANSPARENT);
    dc.SetFont(wxFont(9, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
    dc.DrawText("Cut vector position out of range.", 0, 0);
    return;
  }
  
  this->GetSize(&width, &height);
  double centerX = width / 2;
  double centerY = height / 2;

  // ****************************************************************
  // Determine the zoom (pix/cm).
  // ****************************************************************

  if (width < height)
  {
    zoom = (double)width*0.9 / VocalTract::PROFILE_LENGTH;
  }
  else
  {
    zoom = (double)height*0.9 / VocalTract::PROFILE_LENGTH;
  }

  // ****************************************************************
  // Get a particular profile.
  // ****************************************************************

  int INVALID = (int)VocalTract::INVALID_PROFILE_SAMPLE;

  double upperProfile[VocalTract::NUM_PROFILE_SAMPLES];
  double lowerProfile[VocalTract::NUM_PROFILE_SAMPLES];

  int leftX, rightX;
  int leftIndex  = VocalTract::NUM_PROFILE_SAMPLES/2;
  int rightIndex = VocalTract::NUM_PROFILE_SAMPLES/2;
  int topY[2], bottomY[2];
  int topValid, bottomValid;

  // ****************************************************************
  // Calculation of the profiles.
  // ****************************************************************

  Point2D P, v;

  if (picVocalTract->showTongueCrossSections)
  {
    i = (int)(pos*2.0);
    if (i < 0) { i = 0; }
    if (i >= VocalTract::NUM_TONGUE_RIBS) { i = VocalTract::NUM_TONGUE_RIBS-1; }
    P = tract->tongueRib[i].point;
    v = tract->tongueRib[i].normal;
  }
  else
  {
    tract->getCutVector(pos, P, v);
  }

  // ****************************************************************
  // Draw the upper and lower profile WITHOUT the tongue.
  // ****************************************************************

  bool debug = false;

/*
  if (pos > 16.0)
  {
    debug = true;
  }
*/

  tract->getCrossProfiles(P, v, upperProfile, lowerProfile, false, articulator, debug);
  leftX = (int)(centerX - 0.5*VocalTract::PROFILE_LENGTH*zoom);

  auto pen = *wxMEDIUM_GREY_PEN;
  pen.SetWidth(lineWidth);
  dc.SetPen(pen);
//  dc.SetPen(*wxGREEN_PEN);

  for (i=0; i < VocalTract::NUM_PROFILE_SAMPLES-1; i++)
  {
    rightX = (int)(centerX + (-0.5*VocalTract::PROFILE_LENGTH + (i+1)*VocalTract::PROFILE_SAMPLE_LENGTH)*zoom);

    if ((upperProfile[i] != INVALID) && (upperProfile[i+1] != INVALID))
    {
      topY[0] = (int)(zoom*upperProfile[i]);
      topY[1] = (int)(zoom*upperProfile[i+1]);
      dc.DrawLine(leftX, (int)centerY - topY[0], rightX, (int)centerY - topY[1]);
    }

    if ((lowerProfile[i] != INVALID) && (lowerProfile[i+1] != INVALID))
    {
      bottomY[0] = (int)(zoom*lowerProfile[i]);
      bottomY[1] = (int)(zoom*lowerProfile[i+1]);
      dc.DrawLine(leftX, (int)centerY - bottomY[0], rightX, (int)centerY - bottomY[1]);
    }

    leftX = rightX;
  }

  // ****************************************************************
  // Draw the upper and lower profile WITH the tongue.
  // ****************************************************************

  debug = false;

  tract->getCrossProfiles(P, v, upperProfile, lowerProfile, true, articulator, debug);
  
  leftX = (int)(centerX - 0.5*VocalTract::PROFILE_LENGTH*zoom);

  for (i=0; i < VocalTract::NUM_PROFILE_SAMPLES-1; i++)
  {
    rightX = (int)(centerX + (-0.5*VocalTract::PROFILE_LENGTH + (i+1)*VocalTract::PROFILE_SAMPLE_LENGTH)*zoom);

    if ((upperProfile[i] != INVALID) && (upperProfile[i+1] != INVALID))
    {
      topY[0] = (int)(zoom*upperProfile[i]);
      topY[1] = (int)(zoom*upperProfile[i+1]);
      topValid = true;
    }
    else { topValid = false; }

    if ((lowerProfile[i] != INVALID) && (lowerProfile[i+1] != INVALID))
    {
      bottomY[0] = (int)(zoom*lowerProfile[i]);
      bottomY[1] = (int)(zoom*lowerProfile[i+1]);
      bottomValid = true;
    }
    else { bottomValid = false; }

    if (topValid && bottomValid) 
    {
      wxPoint points[4];

      points[0].x = (int)leftX;
      points[0].y = (int)(centerY - topY[0]);
      points[1].x = (int)rightX;
      points[1].y = (int)(centerY - topY[1]);
      points[2].x = (int)rightX;
      points[2].y = (int)(centerY - bottomY[1]);
      points[3].x = (int)leftX;
      points[3].y = (int)(centerY - bottomY[0]);

      dc.SetBrush(*wxLIGHT_GREY_BRUSH);
      dc.SetPen(wxPen(*wxLIGHT_GREY, lineWidth));
      dc.DrawPolygon(4, points);
    }

    if (topValid) 
    { 
      dc.SetPen(wxPen(*wxBLACK, lineWidth));
      dc.DrawLine(leftX, (int)centerY - topY[0], rightX, (int)centerY - topY[1]); 
    }
    if (bottomValid) 
    { 
      dc.SetPen(wxPen(*wxBLACK, lineWidth));
      dc.DrawLine(leftX, (int)centerY - bottomY[0], rightX, (int)centerY - bottomY[1]); 
    }

    leftX = rightX;
  }

  // ****************************************************************
  // Draw a point where the center line crosses the section.
  // ****************************************************************

  dc.SetPen(wxPen(*wxBLACK, lineWidth));
  wxBrush yellowBrush(wxColor(255, 255, 0));
  dc.SetBrush( yellowBrush );
  dc.DrawEllipse((int)centerX-this->FromDIP(3), (int)centerY-this->FromDIP(3), this->FromDIP(6), this->FromDIP(6));

  // ****************************************************************
  // Print the area and circumference.
  // ****************************************************************

  VocalTract::CrossSection crossSection;
  tract->getCrossSection(upperProfile, lowerProfile, &crossSection);

  wxString st = wxString::Format("A: %2.2f cm^2  C: %2.2f cm", 
    crossSection.area, crossSection.circ);
  dc.SetPen(wxPen(*wxBLACK, lineWidth));
  dc.SetBackgroundMode(wxTRANSPARENT);
  dc.SetFont(wxFont(9, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
  dc.DrawText(st, 0, 0);

  // ****************************************************************
  // Print the name of the lower articulator.
  // ****************************************************************

  switch (articulator)
  {
  case Tube::OTHER_ARTICULATOR: st = "Other articulator"; break;
  case Tube::TONGUE: st = "Tongue"; break;
  case Tube::LOWER_INCISORS: st = "Lower incisors"; break;
  case Tube::LOWER_LIP: st = "Lower lip"; break;
  default: st = "Unknown articulator"; break;
  }

  dc.DrawText(st, 0, this->FromDIP(20));

}

// ****************************************************************************
