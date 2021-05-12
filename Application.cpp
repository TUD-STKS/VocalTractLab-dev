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

#include "Application.h"
#include "Data.h"
#include "IconsXpm.h"

#ifdef WIN32
#include <windows.h>
#include <fcntl.h>
#include <io.h>
#endif

#include <clocale>
#include <locale>
#include <cstdio>
#include <iostream>
#include <fstream>


using namespace std;

IMPLEMENT_APP(Application)

// ****************************************************************************
// ****************************************************************************

bool Application::OnInit()
{
  #ifdef __linux__
	// On Linux (or at least GTK), a ton of warnings are generated and 
	// printed to the terminal. This is because of a well-known 
	// hyper-sensitivity of GTK and thus simplest way to avoid flooding 
	// the terminal is to redirect stderr to a log file.
	auto f = freopen( "stderr.log", "w", stderr );
  #endif

  // Set the current locale to UTF-8 so "special" characters (e.g. ä, é) are 
  // handled correctly across platforms
  std::setlocale(LC_ALL, "C.UTF-8");
  
  createConsole();

  wxPrintf("=== Console output for VocalTractLab 2.3 (built %s) ===\n\n", __DATE__);

  // Init the data class at the very beginning.
  
  Data *data = Data::getInstance();
  if (argc < 1)
  {
    wxPrintf("Error: At least the default command line argument is expected.\n");
  }
  data->init(argv[0]);

  // Create and show the main window.

  MainWindow *mainWindow = new MainWindow();

  // Set the icon for the main window title bar.
  // The icons shown by Windows on the desktop, in the explorer etc. are
  // defined in the resource file.

  wxIcon icon(xpmLogo16);
  mainWindow->SetIcon(icon);

  SetTopWindow(mainWindow);
  mainWindow->Show();


  // After the vocal tract page and the vocal tract dialog were 
  // initialzed, tell some of the non-modal dialogs about it.
  
  VocalTractDialog *vocalTractDialog = VocalTractDialog::getInstance();
  
  VocalTractDialog::getInstance()->setUpdateRequestReceiver(mainWindow->vocalTractPage);
  PhoneticParamsDialog::getInstance()->setUpdateRequestReceiver(vocalTractDialog);
  AnatomyParamsDialog::getInstance()->setUpdateRequestReceiver(vocalTractDialog);
  TransitionDialog::getInstance()->setUpdateRequestReceiver(vocalTractDialog);
  VocalTractShapesDialog::getInstance()->setUpdateRequestReceiver(vocalTractDialog);

  // The vocal tract page always handles update requests from the LF pulse dialog.
  LfPulseDialog::getInstance()->SetParent(mainWindow->vocalTractPage);

  // Show the vocal tract dialog by default.
  vocalTractDialog->SetParent(mainWindow);
  vocalTractDialog->Show(true);

  return true;
}

// ****************************************************************************
// ****************************************************************************

void Application::createConsole()
{

#ifdef WIN32

  // maximum mumber of lines the output console should have

  static const WORD MAX_CONSOLE_LINES = 500;
  int hConHandle;
  HANDLE lStdHandle;

  CONSOLE_SCREEN_BUFFER_INFO coninfo;

  // ****************************************************************

  // allocate a console for this app
  AllocConsole();

  // set the screen buffer to be big enough to let us scroll text
  GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &coninfo);

  coninfo.dwSize.Y = MAX_CONSOLE_LINES;
  SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), coninfo.dwSize);

  // ****************************************************************
  // redirect unbuffered STDOUT to the console
  // e.g. printf("...", ...);
  // ****************************************************************

  lStdHandle = GetStdHandle(STD_OUTPUT_HANDLE);
  hConHandle = _open_osfhandle((intptr_t)lStdHandle, _O_TEXT);

  freopen("CONOUT$", "w", stdout);
  setvbuf(stdout, NULL, _IONBF, 0);

  // ****************************************************************
  // redirect unbuffered STDIN to the console
  // ****************************************************************

  lStdHandle = GetStdHandle(STD_INPUT_HANDLE);
  hConHandle = _open_osfhandle((intptr_t)lStdHandle, _O_TEXT);

  freopen("CONOUT$", "w", stdin);
  setvbuf(stdin, NULL, _IONBF, 0);

  // ****************************************************************
  // redirect unbuffered STDERR to the console
  // ****************************************************************

  lStdHandle = GetStdHandle(STD_ERROR_HANDLE);
  hConHandle = _open_osfhandle((intptr_t)lStdHandle, _O_TEXT);
  freopen("CONOUT$", "w", stderr);
  setvbuf(stderr, NULL, _IONBF, 0);

  // ****************************************************************
  // make cout, wcout, cin, wcin, wcerr, cerr, wclog and clog
  // point to console as well
  // ****************************************************************

  //ios::sync_with_stdio();

#endif
}

// ****************************************************************************

#ifdef __linux__
  int Application::OnExit()
  {
    return fclose(stderr);
  }
#endif


