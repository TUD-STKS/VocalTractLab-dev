#include "PropModesPicture.h"
#include <iostream>
#include <fstream>
#include <string>
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
/// Constructor. Passes the parent parameter.
// ****************************************************************************

PropModesPicture::PropModesPicture(wxWindow* parent,
	Acoustic3dSimulation* simu3d, SegmentsPicture *segPic)
	: BasicPicture(parent),
	m_objectToDisplay(1),
	m_modeIdx(0)
{
	//this->m_picVocalTract = picVocalTract;
	this->m_simu3d = simu3d;
  this->m_segPic = segPic;
}

// ****************************************************************************
/// Draws the picture.
// ****************************************************************************

void PropModesPicture::draw(wxDC& dc)
{

	int width, height;
	//int meshWidth, meshHeight;
	double zoom;
	int xBig, yBig, xEnd, yEnd;
	Data *data = Data::getInstance();
	VocalTract *tract = data->vocalTract;
	//double pos = m_picVocalTract->cutPlanePos_cm;
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
		dc.DrawText("No modes computed.", 0, 0);
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
	double centerX = width / 2;
	double centerY = height / 2;
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
		zoom = (double)width * 1 / maxLength;
	}
	else
	{
		zoom = (double)height * 1 / maxLength;
	}

// ****************************************************************
// plot the segments of the mesh
// ****************************************************************

	if (((m_simu3d->crossSection(sectionIdx))->numberOfFaces() > 0)) {

    info << "Section " << sectionIdx;
    info << "      area  " << m_simu3d->crossSection(sectionIdx)->area() << " cm^2"
      << "      length  " << m_simu3d->crossSection(sectionIdx)->length() << " cm " << endl;
    info << "Curvature angle     "
      << 180. * m_simu3d->crossSection(sectionIdx)->circleArcAngle() / M_PI << " deg   "
      << "        radius  " << m_simu3d->crossSection(sectionIdx)->curvRadius() << " cm" << endl;
    info << "Scaling in  " << m_simu3d->crossSection(sectionIdx)->scaleIn()
      << "      scaling out  " << m_simu3d->crossSection(sectionIdx)->scaleOut() << endl;

		switch (m_objectToDisplay) {
		case 1: {

			array<int, 3> tri;

			int numFaces = (m_simu3d->crossSection(sectionIdx))->numberOfFaces();
			vector<array<double, 2>> pts = (m_simu3d->crossSection(sectionIdx))->getPoints();
			vector<array<int, 3>> triangles = (m_simu3d->crossSection(sectionIdx))->getTriangles();
			
			//log << "\nDraw mesh" << endl;

			vector<int> surf = (m_simu3d->crossSection(sectionIdx))->surfaceIdx();

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
					xBig = (int)(zoom * (pts[tri[v]][0]) + centerX);
					yBig = (int)(centerY - zoom * (pts[tri[v]][1]));
					xEnd = (int)(zoom * (pts[tri[(v + 1) % 3]][0]) + centerX);
					yEnd = (int)(centerY - zoom * (pts[tri[(v + 1) % 3]][1]));
					end = std::chrono::system_clock::now();
					tCoord += end - start;
					dc.DrawLine(xBig, yBig, xEnd, yEnd);
				}
			}

			// draw contour
			Polygon_2 contour = (m_simu3d->crossSection(sectionIdx))->contour();
			CGAL::Polygon_2<K>::Edge_const_iterator vit;
			int s;
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

				xBig = (int)(zoom * (vit->point(0).x()) + centerX);
				yBig = (int)(centerY - zoom * (vit->point(0).y()));
				dc.DrawCircle(xBig, yBig, 1);
			}
			//log << endl;
			//log.close();

			//// if it exists, draw the next contour
			//if ((sectionIdx+1) < (m_simu3d->sectionNumber() - 1))
			//{
			//	Polygon_2 contour = (m_simu3d->crossSection(sectionIdx+1)).contour(0);
			//	dc.SetPen(*wxBLUE_PEN);
			//	CGAL::Polygon_2<K>::Edge_const_iterator vit;
			//	for (vit = contour.edges_begin(); vit != contour.edges_end(); ++vit)
			//	{
			//		xBig = (int)(zoom * (vit->point(0).x()) + centerX);
			//		yBig = (int)(centerY - zoom * (vit->point(0).y()));
			//		xEnd = (int)(zoom * (vit->point(1).x()) + centerX);
			//		yEnd = (int)(centerY - zoom * (vit->point(1).y()));
			//		dc.DrawLine(xBig, yBig, xEnd, yEnd);
			//	}
			//}

			//if (sectionIdx == 57)
			//{
			//	double xB, xE, yB, yE;
			//	ofstream mesh;
			//	mesh.open("mesh.txt");
			//	for (Finite_faces_iterator it = cdt.finite_faces_begin();
			//		it != cdt.finite_faces_end(); ++it)
			//	{
			//		for (int v(0); v < 3; v++)
			//		{
			//			xB = (zoom * (it->vertex(v)->point().x()) + centerX);
			//			yB = (centerY - zoom * (it->vertex(v)->point().y()));
			//			xE = (zoom * (it->vertex((v + 1) % 3)->point().x()) + centerX);
			//			yE = (centerY - zoom * (it->vertex((v + 1) % 3)->point().y()));
			//			mesh << xB << "  " << yB << endl;
			//			mesh << xE << "  " << yE << endl;
			//			mesh << 10000 << "  " << 10000 << endl;
			//		}
			//	}
			//	mesh.close();
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
		case 2: {

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
				numPtSide = (int)ceil(maxDist * zoom) + 1;

				for (int i(0); i < numPtSide; i++)
				{
					for (int j(0); j < (numPtSide - i); j++)
					{
						alpha = ((double)(i) / (double)(numPtSide - 1));
						beta = ((double)(j) / (double)(numPtSide - 1));
						pointToDraw = alpha * vecTri[1] + beta * vecTri[2] +
							(1. - alpha - beta) * vecTri[0];
						normAmp = max(1, (int)(256 * (pointToDraw.z / max(maxAmp, -minAmp) + 1.) / 2.) - 1);
						p.MoveTo(data, (int)(zoom * pointToDraw.x + centerX), (int)(centerY - zoom * pointToDraw.y));
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

		case 3: {
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

    case 4:

      ColorMap colorMap = ColorScale::getColorMap();
      double maxAmp;
      double minAmp;
      Point3D vecTri[3];
      Point3D pointToDraw;
      int numPtSide;
      double alpha;
      double beta;
      double maxDist;
      int normAmp;

      Matrix field(width, height);
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
      auto bid = modes * modesAmpl;

      Vec amplitudes((modes* modesAmpl).cwiseAbs());

      maxAmp = amplitudes.maxCoeff();
      minAmp = amplitudes.minCoeff();

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
        numPtSide = (int)ceil(maxDist * zoom) + 1;

        for (int i(0); i < numPtSide; i++)
        {
          for (int j(0); j < (numPtSide - i); j++)
          {
            alpha = ((double)(i) / (double)(numPtSide - 1));
            beta = ((double)(j) / (double)(numPtSide - 1));
            pointToDraw = alpha * vecTri[1] + beta * vecTri[2] +
              (1. - alpha - beta) * vecTri[0];
            idxI = (int)(zoom * pointToDraw.x + centerX);
            idxJ = (int)(centerY - zoom * pointToDraw.y);
            field(idxI, idxJ) = pointToDraw.z;
            normAmp = max(1, (int)(256. * pointToDraw.z / max(maxAmp, abs(minAmp))) - 1);
            //p.MoveTo(data, (int)(zoom * pointToDraw.x + centerX), (int)(centerY - zoom * pointToDraw.y));
            p.MoveTo(data, idxI, idxJ);
            p.Red() = (*colorMap)[normAmp][0];
            p.Green() = (*colorMap)[normAmp][1];
            p.Blue() = (*colorMap)[normAmp][2];
          }
        }
      }
      // write informations
      info << "f = " << m_simu3d->simuParams().freqField << " Hz" << endl;

      dc.DrawBitmap(bmp, 0, 0, 0);

      // export field
      ofstream ofs("tField.txt");
      ofs << field << endl;
      ofs.close();
		}
	}
	else
	{
  info << "No modes computed." << endl;
	}

  dc.SetPen(*wxBLACK_PEN);
  dc.SetBackgroundMode(wxTRANSPARENT);
  dc.SetFont(wxFont(9, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
  dc.DrawText(info.str(), 0., 0.);

	log.close();
}

// ****************************************************************************
// ****************************************************************************

void PropModesPicture::showMesh()
{
	m_objectToDisplay = 1;
	Refresh();
}

// ****************************************************************************

void PropModesPicture::showMode()
{
	m_objectToDisplay = 2;
	Refresh();
}

// ****************************************************************************

void PropModesPicture::showF()
{
	m_objectToDisplay = 3;
	Refresh();
}

// ****************************************************************************

void PropModesPicture::showField()
{
  m_objectToDisplay = 4;
  Refresh();
}

// ****************************************************************************

void PropModesPicture::setModeIdx(int idx)
{
	//double pos = m_picVocalTract->cutPlanePos_cm;
	//double cumLength(0.);
	//int sectionIdx(0);

	//// ****************************************************************
	//// Identify the index of the corresponding tube
	//// ****************************************************************

	//cumLength = 0.;
	//sectionIdx = -1;

	//for (int i(0); i < m_simu3d->sectionNumber(); i++)
	//{
	//	if (((pos - cumLength) >= -Mesh2d::MINIMAL_DISTANCE) &&
	//		((pos - (cumLength +
	//			(m_simu3d->crossSection(i)).length()))
	//			<= Mesh2d::MINIMAL_DISTANCE))
	//	{
	//		sectionIdx = i;
	//		break;
	//	}
	//	cumLength += (m_simu3d->crossSection(i)).length();
	//}

	//// ****************************************************************
	//// set the index of the mode and update picture
	//// ****************************************************************

	//m_modeIdx = max(0, min((m_simu3d->crossSection(sectionIdx)).numberOfModes()-1, 
	//	idx));
	m_modeIdx = idx;
	Refresh();
}