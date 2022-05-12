#include "PropModesPicture.h"
#include <iostream>
#include <fstream>
#include <string>
#include <regex>
#include "Backend/Geometry.h"
#include "Data.h"
#include <wx/rawbmp.h>
#include <chrono>
#include "ColorScale.h"

// for eigen
#include <Eigen/Dense>

// for CGAL
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>

// typedef for eigen
typedef Eigen::MatrixXd Matrix;
typedef Eigen::VectorXd Vec;

// typedef for CGAL
typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef CGAL::Polygon_2<K>                            Polygon_2;

// for the colormap
typedef int(*ColorMap)[256][3];

// ****************************************************************************
// IDs.
// ****************************************************************************

static const int IDM_EXPORT_ACOUSTIC_FIELD = 1000;

// ****************************************************************************
// The event table.
// ****************************************************************************

BEGIN_EVENT_TABLE(PropModesPicture, BasicPicture)
  EVT_MOUSE_EVENTS(PropModesPicture::OnMouseEvent)
  EVT_MENU(IDM_EXPORT_ACOUSTIC_FIELD, PropModesPicture::OnExportAcousticField)
END_EVENT_TABLE()

// ****************************************************************************
/// Constructor. Passes the parent parameter.
// ****************************************************************************

PropModesPicture::PropModesPicture(wxWindow* parent,
	Acoustic3dSimulation* simu3d, SegmentsPicture *segPic)
	: BasicPicture(parent),
  m_fieldInLogScale(true),
	m_objectToDisplay(CONTOUR),
	m_modeIdx(0)
{
	//this->m_picVocalTract = picVocalTract;
	this->m_simu3d = simu3d;
  this->m_segPic = segPic;

  m_contextMenu = new wxMenu();
  m_contextMenu->Append(IDM_EXPORT_ACOUSTIC_FIELD, "Export acoustic field in text file");
}

// ****************************************************************************
/// Draws the picture.
// ****************************************************************************

void PropModesPicture::draw(wxDC& dc)
{

	int width, height;
	//double zoom;
	int xBig, yBig, xEnd, yEnd;
	Data *data = Data::getInstance();
	VocalTract *tract = data->vocalTract;
	double cumLength(0.), minDist(1e-15);
	int sectionIdx(0);
  ostringstream info;

	ofstream log("log.txt", ofstream::app);
	log << "\nStart draw mode picture" << endl;

	// Clear the background.
	dc.SetBackground(*wxWHITE_BRUSH);
	dc.Clear();

	// check if the modes have been computed
	if (m_simu3d->sectionNumber() == 0)
	{
		dc.SetPen(*wxBLACK_PEN);
		dc.SetBackgroundMode(wxTRANSPARENT);
		dc.SetFont(wxFont(9, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
		dc.DrawText("No segments.", 0, 0);
		return;
	}

// ****************************************************************
// Identify the index of the corresponding tube
// ****************************************************************

  sectionIdx = m_segPic->activeSegment();

// ****************************************************************
// Determine the zoom (pix/cm).
// ****************************************************************

	// get the dimensions of the picture
	this->GetSize(&width, &height);
	m_centerX = width / 2;
	m_centerY = height / 2;
	double maxLength;
	pair<Point2D, Point2D> bbox(m_simu3d->maxCSBoundingBox());

	// get the maximal dimension of the cross-sections
	if (m_simu3d->isGeometryImported())
	{
		maxLength = 2.*max(max(abs(bbox.first.x), abs(bbox.second.x)), 
		max(abs(bbox.first.y), abs(bbox.second.y)));
	}
	else
	{
		maxLength = VocalTract::PROFILE_LENGTH;
	}
	//log << "maxLength " << maxLength << endl;
	

	if (width < height)
	{
		m_zoom = (double)width * 1 / maxLength;
	}
	else
	{
    m_zoom = (double)height * 1 / maxLength;
	}

// ****************************************************************
// plot the segments of the mesh
// ****************************************************************

  if (((m_simu3d->crossSection(sectionIdx))->numberOfFaces() == 0)) {
    m_objectToDisplay = CONTOUR;
  }

    info << "Section " << sectionIdx;
    info << "      area  " << m_simu3d->crossSection(sectionIdx)->area() << " cm^2"
      << "      length  " << m_simu3d->crossSection(sectionIdx)->length() << " cm " << endl;
    info << "Curvature angle     "
      << 180. * m_simu3d->crossSection(sectionIdx)->circleArcAngle() / M_PI << " deg   "
      << "        radius  " << m_simu3d->crossSection(sectionIdx)->curvRadius() << " cm" << endl;
    info << "Scaling in  " << m_simu3d->crossSection(sectionIdx)->scaleIn()
      << "      scaling out  " << m_simu3d->crossSection(sectionIdx)->scaleOut() << endl;

    vector<int> surf = (m_simu3d->crossSection(sectionIdx))->surfaceIdx();

		switch (m_objectToDisplay) {
		case MESH: {

			array<int, 3> tri;

			int numFaces = (m_simu3d->crossSection(sectionIdx))->numberOfFaces();
			vector<array<double, 2>> pts = (m_simu3d->crossSection(sectionIdx))->getPoints();
			vector<array<int, 3>> triangles = (m_simu3d->crossSection(sectionIdx))->getTriangles();
			
			//log << "\nDraw mesh" << endl;

			auto start = std::chrono::system_clock::now();
			auto end = std::chrono::system_clock::now();
			std::chrono::duration<double> tTri;
			std::chrono::duration<double> tCoord;

			dc.SetPen(*wxBLACK_PEN);
			tTri.zero();
			tCoord.zero();
			for (int it(0); it < numFaces; ++it)
			{
				start = std::chrono::system_clock::now();
				tri = triangles[it];
				end = std::chrono::system_clock::now();
				tTri += end - start;
				for (int v(0); v < 3; v++)
				{
					start = std::chrono::system_clock::now();
					xBig = (int)(m_zoom * (pts[tri[v]][0]) + m_centerX);
					yBig = (int)(m_centerY - m_zoom * (pts[tri[v]][1]));
					xEnd = (int)(m_zoom * (pts[tri[(v + 1) % 3]][0]) + m_centerX);
					yEnd = (int)(m_centerY - m_zoom * (pts[tri[(v + 1) % 3]][1]));
					end = std::chrono::system_clock::now();
					tCoord += end - start;
					dc.DrawLine(xBig, yBig, xEnd, yEnd);
				}
			}

      drawContour(sectionIdx, surf, dc);

			//// if it exists, draw the next contour
			//if ((sectionIdx+1) < (m_simu3d->sectionNumber() - 1))
			//{
			//	Polygon_2 contour = (m_simu3d->crossSection(sectionIdx+1)).contour(0);
			//	dc.SetPen(*wxBLUE_PEN);
			//	CGAL::Polygon_2<K>::Edge_const_iterator vit;
			//	for (vit = contour.edges_begin(); vit != contour.edges_end(); ++vit)
			//	{
			//		xBig = (int)(m_zoom * (vit->point(0).x()) + m_centerX);
			//		yBig = (int)(m_centerY - m_zoom * (vit->point(0).y()));
			//		xEnd = (int)(m_zoom * (vit->point(1).x()) + m_centerX);
			//		yEnd = (int)(m_centerY - m_zoom * (vit->point(1).y()));
			//		dc.DrawLine(xBig, yBig, xEnd, yEnd);
			//	}
			//}

			// display number of vertex, segments and triangles
      info << (m_simu3d->crossSection(sectionIdx))->numberOfVertices() << "  vertexes  "
        << (m_simu3d->crossSection(sectionIdx))->numberOfFaces() << "  faces" << endl;

			//end = std::chrono::system_clock::now();
			//elapsed_seconds = end - start;
			//log << elapsed_seconds.count() << endl;
			//log << "tri " << tTri.count() << endl;
			//log << "Coord " << tCoord.count() << endl;
			//log.close();

			}
			break;
			// ****************************************************************
			// plot the modes
			// ****************************************************************
		case TRANSVERSE_MODE: {

			auto start = std::chrono::system_clock::now();
			auto end = std::chrono::system_clock::now();
			std::chrono::duration<double> elapsed_seconds = end - start;

			int normAmp;
			double maxAmp(0.);
			double minAmp(0.);
			Point3D vecTri[3];
			Point3D pointToDraw;
			int numPtSide;
			double alpha;
			double beta;
			double maxDist;

			// check if the mode index is in the range of the number
			// of modes
			m_modeIdx = max(0, min((
				m_simu3d->crossSection(sectionIdx))->numberOfModes()-1,
				m_modeIdx));

			ColorMap colorMap = ColorScale::getColorMap();

			// initialise white bitmap
			wxBitmap bmp(width, height, 24);
			wxNativePixelData data(bmp);
			wxNativePixelData::Iterator p(data);
			for (int i = 0; i < height; ++i)
			{
				wxNativePixelData::Iterator rowStart = p;
				for (int j = 0; j < width; ++j, ++p)
				{
					p.Red() = 254;
					p.Green() = 254;
					p.Blue() = 254;
				}
				p = rowStart;
				p.OffsetY(data, 1);
			}

			//end = std::chrono::system_clock::now();
			//elapsed_seconds = end - start;
			//log << "map initialization "  << endl;
			//start = std::chrono::system_clock::now();

			int numFaces = (m_simu3d->crossSection(sectionIdx))->numberOfFaces();
			int numVertex = (m_simu3d->crossSection(sectionIdx))->numberOfVertices();
			vector<array<double, 2>> pts = (m_simu3d->crossSection(sectionIdx))->getPoints();
			vector<array<int, 3>> triangles = (m_simu3d->crossSection(sectionIdx))->getTriangles();
			Matrix modes = (m_simu3d->crossSection(sectionIdx))->getModes();

			//end = std::chrono::system_clock::now();
			//elapsed_seconds = end - start;
			//log << "Load cdt " << elapsed_seconds.count() << endl;
			//start = std::chrono::system_clock::now();

			// extract the maximum and minimum of amplitude
			maxAmp = (m_simu3d->crossSection(sectionIdx))->getMaxAmplitude(m_modeIdx);
			minAmp = (m_simu3d->crossSection(sectionIdx))->getMinAmplitude(m_modeIdx);


			// draw the propagation mode
			//
			// The triangles are interpolated by adding the the fraction a and b 
			// of the vector formed by two sides and the corresponding vertex amplitude.
			// The corresponding values are attributed to the location of the nearest 
			// pixel in the bitmap grid. 
			// This avoid searching to which triangle a pixel belongs, which has a higher
			// computational cost.
			//
			//
			//   ^ P1
			//   |
			//   |   P?
			//   |________\ P2
			//  O         /
			//
			//  P = a*P1 + b*P2 + (1 - a - b)*O 
			//  
			//  with 0 <= a <= 1 and 0 <= b <= 1
			std::chrono::duration<double> tPt3, tCalc;
			tPt3.zero(); tCalc.zero();
			auto big = std::chrono::system_clock::now();
			auto fin = std::chrono::system_clock::now();

			for (int it(0); it < numFaces; ++it)
			{
				//big = std::chrono::system_clock::now();
				for (int i(0); i < 3; i++)
				{
					vecTri[i] = Point3D(pts[triangles[it][i]][0], pts[triangles[it][i]][1],
						modes(triangles[it][i], m_modeIdx));// [m_modeIdx * numVertex + triangles[it][i]] );
				}
				//fin = std::chrono::system_clock::now();
				//tPt3 += fin - big;
				big = std::chrono::system_clock::now();
				maxDist = max(sqrt(pow(vecTri[1].x - vecTri[0].x, 2) + pow(vecTri[1].y - vecTri[0].y, 2)),
					max(sqrt(pow(vecTri[2].x - vecTri[0].x, 2) + pow(vecTri[2].y - vecTri[0].y, 2)),
						sqrt(pow(vecTri[1].x - vecTri[2].x, 2) + pow(vecTri[1].y - vecTri[2].y, 2))));
				numPtSide = (int)ceil(maxDist * m_zoom) + 1;

				for (int i(0); i < numPtSide; i++)
				{
					for (int j(0); j < (numPtSide - i); j++)
					{
						alpha = ((double)(i) / (double)(numPtSide - 1));
						beta = ((double)(j) / (double)(numPtSide - 1));
						pointToDraw = alpha * vecTri[1] + beta * vecTri[2] +
							(1. - alpha - beta) * vecTri[0];
						normAmp = max(1, (int)(256 * (pointToDraw.z / max(maxAmp, -minAmp) + 1.) / 2.) - 1);
						p.MoveTo(data, (int)(m_zoom* pointToDraw.x + m_centerX), (int)(m_centerY - m_zoom * pointToDraw.y));
						p.Red() = (*colorMap)[normAmp][0];
						p.Green() = (*colorMap)[normAmp][1];
						p.Blue() = (*colorMap)[normAmp][2];
					}
				}
				//fin = std::chrono::system_clock::now();
				//tCalc += fin - big;
			}
			//end = std::chrono::system_clock::now();
			//elapsed_seconds = end - start;
			//log << "Compute pixels " << elapsed_seconds.count() << endl;
			//log << "Time extract points " << tPt3.count() << endl;
			//log << "Time compute pixels " << tCalc.count() << endl;
			//start = std::chrono::system_clock::now();

			// write informations about the mode
			info << "mode " << m_modeIdx + 1 << " over " <<
				(m_simu3d->crossSection(sectionIdx))->numberOfModes() <<
				"    f" << m_modeIdx + 1 <<
				"= " << (m_simu3d->crossSection(sectionIdx))->eigenFrequency(m_modeIdx) <<
				" Hz" << endl;
				//area " << (m_simu3d->crossSection(sectionIdx)).area(m) << " cm^2" << endl;
			dc.DrawBitmap(bmp, 0, 0, 0);

			//end = std::chrono::system_clock::now();
			//elapsed_seconds = end - start;
			//log << "Draw map " << elapsed_seconds.count() << endl;
		}
		break;

    // ****************************************************************
    // plot the junction matrix
    // ****************************************************************

		case JUNCTION_MATRIX: {
			vector<Matrix> F = (m_simu3d->crossSection(sectionIdx))->getMatrixF();
			int numCont(F.size());
			int maxNumF(0);

			maxNumF = 1;// max(maxNumF, numCont);


			double widthFn((double)min(width, height) / ((double)maxNumF * 1.1));
			int margin((int)(0.05 * widthFn));
			int topMargin((int)(((double)height - maxNumF*widthFn*1.05) / 2.));
			int widthSquare, numCols, numRows, normAmp;
			double maxF, minF, normF;
			ColorMap colorMap = ColorScale::getColorMap();
			wxColor color;


			// loop over junction matrices
			for (int n(0); n < F.size(); n++)
			{
				numCols = F[n].cols();
				numRows = F[n].rows();
				// compute the width of the squares displaying the values of the matrix
				widthSquare = (int)(widthFn / (double)numCols);

				// search for the maximal and minimal values of the matrix
				maxF = 0.; minF = 0.;
				// loop over rows
				for (int r(0); r < numRows; r++)
				{
					// loop over columns
					for (int c(0); c < numCols; c++)
					{
						maxF = max(maxF, F[n](r, c));
						minF = min(minF, F[n](r, c));
					}
				}
				maxF = max(maxF, 1.);
				minF = min(minF, -1.);
				normF = 1. / (maxF - minF);

				// loop over rows
				for (int r(0); r < numRows; r++)
				{
					// loop over columns
					for (int c(0); c < numCols; c++)
					{

						normAmp = max(0, (int)(255. * normF * (F[n](r, c) - minF)));
						color = wxColor((*colorMap)[normAmp][0], (*colorMap)[normAmp][1],
							(*colorMap)[normAmp][2]);
						dc.SetPen(color);
						dc.SetBrush(color);
						dc.DrawRectangle(margin + min(width, height) / maxNumF + c * widthSquare, 
							topMargin + n* min(width, height) / maxNumF + r * widthSquare, 
							widthSquare, widthSquare);
					}
				}
			}

			}
		break;

    // ****************************************************************
    // plot the acoustic field
    // ****************************************************************

    case ACOUSTIC_FIELD: {

      if (m_simu3d->crossSection(sectionIdx)->Pout().rows() > 0)
      {
        ColorMap colorMap = ColorScale::getColorMap();
        double maxAmp;
        double minAmp;
        // to avoid singular values when the field is displayed in dB
        double dbShift(0.5);
        Point3D vecTri[3];
        Point3D pointToDraw;
        int numPtSide;
        double alpha;
        double beta;
        double maxDist;
        int normAmp;

        auto start = std::chrono::system_clock::now();


        m_field.resize(height, width);
        m_field.setConstant(NAN);

        auto end = std::chrono::system_clock::now();
        std::chrono::duration<double> duration = start - end;

        log << "Time m_field " << duration.count() << endl;

        int idxI, idxJ;

        // initialise white bitmap
        wxBitmap bmp(width, height, 24);
        wxNativePixelData data(bmp);
        wxNativePixelData::Iterator p(data);
        for (int i = 0; i < height; ++i)
        {
          wxNativePixelData::Iterator rowStart = p;
          for (int j = 0; j < width; ++j, ++p)
          {
            p.Red() = 254;
            p.Green() = 254;
            p.Blue() = 254;
          }
          p = rowStart;
          p.OffsetY(data, 1);
        }

        int numFaces = (m_simu3d->crossSection(sectionIdx))->numberOfFaces();
        int numVertex = (m_simu3d->crossSection(sectionIdx))->numberOfVertices();
        vector<array<double, 2>> pts = (m_simu3d->crossSection(sectionIdx))->getPoints();
        vector<array<int, 3>> triangles = (m_simu3d->crossSection(sectionIdx))->getTriangles();
        Matrix modes = (m_simu3d->crossSection(sectionIdx))->getModes();
        int mn(modes.cols());

        log << "Mode number " << mn << endl;

        auto modesAmpl(m_simu3d->crossSection(sectionIdx)->Pout());

        log << "modesAmpl:\n" << modesAmpl << endl << endl;

        Vec amplitudes((modes * modesAmpl).cwiseAbs());

        maxAmp = m_simu3d->maxAmpField();
        minAmp = m_simu3d->minAmpField();
        // if no acoustic field has been computed in the sagittal plane 
        // the maximum and minimum amplitude are taken from the local transverse field
        if (maxAmp < 0.)
        {
          maxAmp = amplitudes.maxCoeff();
          minAmp = amplitudes.minCoeff();
        }
        if (m_fieldInLogScale) {
          maxAmp = 20. * log10(maxAmp);
          minAmp = 20. * log10(minAmp);
          maxAmp = maxAmp - minAmp + dbShift;
        }

        log << "maxAmp " << maxAmp << " minAmp " << minAmp << endl;

        // draw the acoustic field
        //
        // The triangles are interpolated by adding the the fraction a and b 
        // of the vector formed by two sides and the corresponding vertex amplitude.
        // The corresponding values are attributed to the location of the nearest 
        // pixel in the bitmap grid. 
        // This avoid searching to which triangle a pixel belongs, which has a higher
        // computational cost.
        //
        //
        //   ^ P1
        //   |
        //   |   P?
        //   |________\ P2
        //  O         /
        //
        //  P = a*P1 + b*P2 + (1 - a - b)*O 
        //  
        //  with 0 <= a <= 1 and 0 <= b <= 1

        for (int it(0); it < numFaces; ++it)
        {
          for (int i(0); i < 3; i++)
          {
            vecTri[i] = Point3D(pts[triangles[it][i]][0], pts[triangles[it][i]][1],
              amplitudes(triangles[it][i]));
          }

          maxDist = max(sqrt(pow(vecTri[1].x - vecTri[0].x, 2) + pow(vecTri[1].y - vecTri[0].y, 2)),
            max(sqrt(pow(vecTri[2].x - vecTri[0].x, 2) + pow(vecTri[2].y - vecTri[0].y, 2)),
              sqrt(pow(vecTri[1].x - vecTri[2].x, 2) + pow(vecTri[1].y - vecTri[2].y, 2))));
          numPtSide = (int)ceil(maxDist * m_zoom) + 1;

          for (int i(0); i < numPtSide; i++)
          {
            for (int j(0); j < (numPtSide - i); j++)
            {
              alpha = ((double)(i) / (double)(numPtSide - 1));
              beta = ((double)(j) / (double)(numPtSide - 1));
              pointToDraw = alpha * vecTri[1] + beta * vecTri[2] +
                (1. - alpha - beta) * vecTri[0];
              idxI = (int)(m_zoom * pointToDraw.x + m_centerX);
              idxJ = (int)(m_centerY - m_zoom * pointToDraw.y);
              m_field(height - idxJ -1, idxI) = pointToDraw.z;
              if (m_fieldInLogScale) 
              {
                pointToDraw.z = 20. * log10(pointToDraw.z) - minAmp + dbShift;
              }
              normAmp = min(255, max(1, (int)(256. * pointToDraw.z / max(maxAmp, abs(minAmp))) - 1));

              p.MoveTo(data, idxI, idxJ);
              p.Red() = (*colorMap)[normAmp][0];
              p.Green() = (*colorMap)[normAmp][1];
              p.Blue() = (*colorMap)[normAmp][2];
            }
          }
        }
        // write informations
        info << "f = " << m_simu3d->lastFreqComputed() << " Hz" << endl;

        dc.DrawBitmap(bmp, 0, 0, 0);
        break;
      }
    }

      default:
        drawContour(sectionIdx, surf, dc);
        break;
		}

  dc.SetPen(*wxBLACK_PEN);
  dc.SetBackgroundMode(wxTRANSPARENT);
  dc.SetFont(wxFont(9, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
  dc.DrawText(info.str(), 0., 0.);

	log.close();
}

// ****************************************************************************
// ****************************************************************************

enum objectToDisplay PropModesPicture::getObjectDisplayed()
{
  return(m_objectToDisplay);
}

// ****************************************************************************

void PropModesPicture::setObjectToDisplay(enum objectToDisplay object)
{
  m_objectToDisplay = object;
  Refresh();
}

// ****************************************************************************

void PropModesPicture::setModeIdx(int idx)
{
	m_modeIdx = idx;
	Refresh();
}

// ****************************************************************************

void PropModesPicture::drawContour(int sectionIdx, vector<int> &surf, wxDC& dc)
{
  Polygon_2 contour = (m_simu3d->crossSection(sectionIdx))->contour();
  CGAL::Polygon_2<K>::Edge_const_iterator vit;
  int s, xBig, yBig, xEnd, yEnd;
  for (s = 0, vit = contour.edges_begin(); vit != contour.edges_end(); ++vit, ++s)
  {
    switch (surf[s])
    {
    case 2: case 3: case 23: case 24:		// covers
      dc.SetPen(wxPen(*wxCYAN, 2, wxPENSTYLE_SOLID));
      break;
    case 16:						// tongue
      dc.SetPen(wxPen(*wxRED, 2, wxPENSTYLE_SOLID));
      break;
    case 0: case 1:					// teeth
      dc.SetPen(wxPen(*wxYELLOW, 2, wxPENSTYLE_SOLID));
      break;
    case 29:						// epiglotis
      dc.SetPen(wxPen(*wxBLUE, 2, wxPENSTYLE_SOLID));
      break;
    case 26:						// uvula
      dc.SetPen(wxPen(*wxLIGHT_GREY, 2, wxPENSTYLE_SOLID));
      break;
    case 4: case 5:					// lips
      dc.SetPen(wxPen(wxColour(255, 0, 255, 255), 2, wxPENSTYLE_SOLID));
      break;
    case 31:						// radiation
      dc.SetPen(wxPen(wxColour(128, 0, 128, 255), 2, wxPENSTYLE_SOLID));
      break;
    default:
      dc.SetPen(wxPen(*wxGREEN, 2, wxPENSTYLE_SOLID));
    }

    xBig = (int)(m_zoom * (vit->point(0).x()) + m_centerX);
    yBig = (int)(m_centerY - m_zoom * (vit->point(0).y()));
    dc.DrawCircle(xBig, yBig, 1);

    xEnd = (int)(m_zoom * (vit->point(1).x()) + m_centerX);
    yEnd = (int)(m_centerY - m_zoom * (vit->point(1).y()));

    dc.SetPen(*wxBLACK_PEN);
    dc.DrawLine(xBig, yBig, xEnd, yEnd);
  }
}

// ****************************************************************************

void PropModesPicture::OnMouseEvent(wxMouseEvent& event)
{
  // Right click
  if (event.ButtonDown(wxMOUSE_BTN_RIGHT) && (m_simu3d->numberOfSegments() > 0))
  {
    PopupMenu(m_contextMenu);
  }
}

// ****************************************************************************

void PropModesPicture::OnExportAcousticField(wxCommandEvent& event)
{
  wxFileName fileName;
  wxString name = wxFileSelector("Save acoustic field", fileName.GetPath(),
    fileName.GetFullName(), ".txt", "(*.txt)|*.txt",
    wxFD_SAVE | wxFD_OVERWRITE_PROMPT, this);

  // export field
  ofstream ofs(name.ToStdString());
  stringstream txtField;
  txtField << m_field;
  ofs << regex_replace(txtField.str(), regex("-nan\\(ind\\)"), "nan");
  ofs.close();

  ofstream log("log.txt", ofstream::app);
  log << "Transverse acoustic field of segment " 
    << m_segPic->activeSegment() << " saved in file:" << endl;
  log << name.ToStdString() << endl;
  log.close();
}