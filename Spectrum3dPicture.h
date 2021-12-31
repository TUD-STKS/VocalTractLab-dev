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
  
  // **************************************************************************
  // Private data.
  // **************************************************************************

private:
  int lineWidth{ this->FromDIP(1) };
  Acoustic3dSimulation *simu3d;
};

#endif
