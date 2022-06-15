#include "Spectrum3dPicture.h"
#include <fstream>
#include <wx/filename.h>

// ****************************************************************************
// IDs.
// ****************************************************************************

static const int IDM_EXPORT_GLOTTAL_TF = 1000;
static const int IDM_EXPORT_NOISE_SRC_TF = 1001;
static const int IDM_EXPORT_INPUT_IMPEDANCE = 1002;

// ****************************************************************************
// The event table.
// ****************************************************************************

BEGIN_EVENT_TABLE(Spectrum3dPicture, BasicPicture)
  EVT_MOUSE_EVENTS(Spectrum3dPicture::OnMouseEvent)
  EVT_MENU(IDM_EXPORT_GLOTTAL_TF, Spectrum3dPicture::OnExportGlottalTf)
  EVT_MENU(IDM_EXPORT_NOISE_SRC_TF, Spectrum3dPicture::OnEXportNoiseSrcTf)
  EVT_MENU(IDM_EXPORT_INPUT_IMPEDANCE, Spectrum3dPicture::OnExportInputImpedance)
END_EVENT_TABLE()

// ****************************************************************************
/// Constructor.
// ****************************************************************************

Spectrum3dPicture::Spectrum3dPicture(wxWindow *parent, 
    Acoustic3dSimulation *simu3d): BasicPicture(parent),
  m_idxPtTf(0)
{
  // ****************************************************************
  // Init the variables
  // ****************************************************************

  this->simu3d = simu3d;

  m_showGlottalTf = true;
  m_showNoiseTf = true;

  // ****************************************************************
  // The spectrum graph
  // ****************************************************************

  graph.init(this, 40, 45, 0, 25);

  graph.initAbscissa(PQ_FREQUENCY, 0.0, 100.0,
    0.0, 0.0, 0.0, 
    2000.0, 20000.0, 10000.0,
    8, 0, false, false, true);

  graph.initLogOrdinate(1.0, 5.0,
    -100.0, -10.0, -100, 
    10.0, 100.0, 50.0,
    true, 10);

  graph.isLinearOrdinate = false;

  // ****************************************************************
  // The context menu
  // ****************************************************************

  m_contextMenu = new wxMenu();
  m_contextMenu->Append(IDM_EXPORT_GLOTTAL_TF, "Save the glottal transfer function as txt file");
  m_contextMenu->Append(IDM_EXPORT_NOISE_SRC_TF, "Save the noise transfer function as txt file");
  m_contextMenu->Append(IDM_EXPORT_INPUT_IMPEDANCE, "Save input impedance as txt file");
}

// ****************************************************************************
// ****************************************************************************

void Spectrum3dPicture::draw(wxDC &dc)
{
  dc.SetBackground(*wxWHITE_BRUSH);
  dc.Clear();
  graph.paintAbscissa(dc);
  graph.paintOrdinate(dc);

  paintSpectrum(dc);
}

// ****************************************************************************
// ****************************************************************************

void Spectrum3dPicture::paintSpectrum(wxDC &dc)
{
  if (m_showGlottalTf)
  {
    drawTf(dc, GLOTTAL);
  }

  if (m_showNoiseTf)
  {
    drawTf(dc, NOISE);
  }
}

// ****************************************************************************
// ****************************************************************************

void Spectrum3dPicture::drawTf(wxDC& dc, enum tfType type)
{
  int graphX, graphY, graphW, graphH, y, lastY;
  double freq, val;

  graph.getDimensions(graphX, graphY, graphW, graphH);
  lastY = graphY;

  switch (type)
  {
  case GLOTTAL:
    dc.SetPen(wxPen(*wxBLACK, lineWidth));
    break;
  case NOISE:
    dc.SetPen(wxPen(*wxBLUE, lineWidth));
    break;
  }

  // 
  for (int i(0); i < graphW; i++)
  {
    freq = graph.getAbsXValue(graphX + i);
    val = abs(simu3d->interpolateTransferFunction(freq, m_idxPtTf, type));

    y = graph.getYPos(val);

    if (y < graphY)
    {
      y = graphY;
    }
    if (y >= graphY + graphH)
    {
      y = graphY + graphH - 1;
    }

    if (isnan(val)) { y = NAN; }

      dc.DrawLine(graphX + i - 1, lastY, graphX + i, y);

    lastY = y;
  }
}

// ****************************************************************************
// ****************************************************************************

void Spectrum3dPicture::exportResult(enum tfType type)
{
  wxFileName fileName;
  wxString name = wxFileSelector("Save transfer functions", fileName.GetPath(),
    fileName.GetFullName(), ".txt", "(*.txt)|*.txt",
    wxFD_SAVE | wxFD_OVERWRITE_PROMPT, this);

  simu3d->exportTransferFucntions(name.ToStdString(), type);
}

// ****************************************************************************
// ****************************************************************************

void Spectrum3dPicture::OnMouseEvent(wxMouseEvent& event)
{
  // Right click
  if (event.ButtonDown(wxMOUSE_BTN_RIGHT))
  {
    PopupMenu(m_contextMenu);
  }
}

// ****************************************************************************
// ****************************************************************************

void Spectrum3dPicture::OnExportGlottalTf(wxCommandEvent& event)
{
  exportResult(GLOTTAL);
}

// ****************************************************************************
// ****************************************************************************

void Spectrum3dPicture::OnEXportNoiseSrcTf(wxCommandEvent& event)
{
  exportResult(NOISE);
}

// ****************************************************************************
// ****************************************************************************

void Spectrum3dPicture::OnExportInputImpedance(wxCommandEvent& event)
{
  exportResult(INPUT_IMPED);
}