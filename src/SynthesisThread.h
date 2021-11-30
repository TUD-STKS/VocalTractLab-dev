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

#ifndef __SYNTHESIS_THREAD_H__
#define __SYNTHESIS_THREAD_H__

#include <wx/thread.h>
#include <wx/wx.h>
#include "VocalTractLabBackend/TubeSequence.h"
#include "VocalTractLabBackend/TdsModel.h"
#include "Data.h"

// Define the IDs for events that are sent to the GUI treat.
static const int SYNTHESIS_THREAD_EVENT = 54321;

// ****************************************************************************
/// This class encapsulates the thread that performs the background calculation
/// of time-domain simulations of the vocal system.
/// The actual computation is done in void Entry(), which is executed when
/// wxThread::Run() is called from an other thread.
/// To terminate the thread before it finishes by itself, call 
/// cancelNow(). 
/// During execution, the thread continually posts events
/// (SYNTHESIS_THREAD_EVENT) to the GUI thread, which contains the progress of
/// the thread in percent (0..100). When the thread finished its task it sends
/// the value -1. When the GUI thread updated the GUI in response to such an
/// event, it must call signalGuiUpdateFinished() to tell the thread to 
/// continue its operation.
// ****************************************************************************

class SynthesisThread : public wxThread
{
  // **************************************************************************
  // **************************************************************************

public:
  SynthesisThread(wxWindow *window, TubeSequence *tubeSequence);
  inline void cancelNow() { canceled = true; }
  inline bool wasCanceled() { return canceled; }
  inline void signalGuiUpdateFinished() { guiUpdateFinished = true; }

  // Thread execution starts here
  virtual void *Entry();

  // Called when the thread exits (normally or with Delete()), but not when it is killed
  virtual void OnExit();

  // **************************************************************************
  // **************************************************************************

private:
  wxWindow *window;
  TubeSequence *tubeSequence;
  bool canceled;
  bool guiUpdateFinished;
};

// ****************************************************************************

#endif
