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

#include "PoleZeroDialog.h"
#include "SilentMessageBox.h"
#include "SoundLib.h"

// ****************************************************************************
// IDs.
// ****************************************************************************

static const int IDC_HIGHER_POLE_CORRECTION = 4000;
static const int IDB_ENTER_POLES            = 4001;
static const int IDB_ENTER_ZEROS            = 4002;
static const int IDB_PLAY_SHORT_VOWEL       = 4003;
static const int IDB_PLAY_LONG_VOWEL        = 4004;

// ****************************************************************************
// The event table.
// ****************************************************************************

BEGIN_EVENT_TABLE(PoleZeroDialog, wxDialog)
  EVT_CHECKBOX(IDC_HIGHER_POLE_CORRECTION, PoleZeroDialog::OnHigherPoleCorrection)
  EVT_BUTTON(IDB_ENTER_POLES, PoleZeroDialog::OnEnterPoles)
  EVT_BUTTON(IDB_ENTER_ZEROS, PoleZeroDialog::OnEnterZeros)
  EVT_BUTTON(IDB_PLAY_SHORT_VOWEL, PoleZeroDialog::OnPlayShortVowel)
  EVT_BUTTON(IDB_PLAY_LONG_VOWEL, PoleZeroDialog::OnPlayLongVowel)
  EVT_COMMAND(wxID_ANY, updateRequestEvent, PoleZeroDialog::OnUpdateRequest)
END_EVENT_TABLE()

// The single instance of this class.
PoleZeroDialog *PoleZeroDialog::instance = NULL;


// ****************************************************************************
/// Returns the single instance of this dialog.
// ****************************************************************************

PoleZeroDialog *PoleZeroDialog::getInstance()
{
  if (instance == NULL)
  {
    instance = new PoleZeroDialog(NULL);
  }

  return instance;
}


// ****************************************************************************
/// Update all widgets and refresh the picture.
// ****************************************************************************

void PoleZeroDialog::updateWidgets()
{
  int i;
  wxString st;

  // Output the data about all pole and zero locations.

  vector<PoleZeroPlan::Location> list;
  plan->sortLocations(plan->poles, list);

  for (i=0; (i < (int)list.size()) && (i < 10); i++)
  {
    st+= wxString::Format("%d  (%d)\n", (int)list[i].freq_Hz, (int)list[i].bw_Hz);
  }
  labPoleLocations->SetLabel(st);

  st.Clear();
  plan->sortLocations(plan->zeros, list);

  for (i=0; (i < (int)list.size()) && (i < 10); i++)
  {
    st+= wxString::Format("%d  (%d)\n", (int)list[i].freq_Hz, (int)list[i].bw_Hz);
  }
  labZeroLocations->SetLabel(st);

  // Set the checkbox state.

  chkHigherPoleCorrection->SetValue(plan->higherPoleCorrection);

  // Send an update request to the parent window to refresh the spectrum panel

  if (spectrumPicture != NULL)
  {
    spectrumPicture->Refresh();
  }

  // Refresh the plot picture.

  poleZeroPlot->Refresh();
}


// ****************************************************************************
/// Set a pointer to the spectrum picture so that it can be refreshed when
/// the pole-zero plan changes.
// ****************************************************************************

void PoleZeroDialog::setSpectrumPicture(wxPanel *spectrumPicture)
{
  this->spectrumPicture = spectrumPicture;
}

// ****************************************************************************
/// Constructor.
// ****************************************************************************

PoleZeroDialog::PoleZeroDialog(wxWindow *parent) : 
  wxDialog(parent, wxID_ANY, wxString("Pole-zero plot of a vocal tract transfer function"), 
    wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE)
{
  this->Move(100, 50);

  // ****************************************************************
  // Init the variables first.
  // ****************************************************************

  data = Data::getInstance();
  plan = data->poleZeroPlan;
  spectrumPicture = NULL;   // Must be set later.

  // ****************************************************************
  // Init and update the widgets.
  // ****************************************************************

  initWidgets();
  updateWidgets();
}


// ****************************************************************************
/// Init the widgets of this dialog.
// ****************************************************************************

void PoleZeroDialog::initWidgets()
{
  wxBoxSizer *topLevelSizer = new wxBoxSizer(wxHORIZONTAL);
  wxBoxSizer *sizer;
  wxStaticText *label;
  wxButton *button;


  // Create an invisible dummy button with the ID wxID_CANCEL, so that
  // the user can close the dialog with ESC.
  button = new wxButton(this, wxID_CANCEL, "", wxPoint(0,0), wxSize(0,0));


  // ****************************************************************
  // Add the pole-zero plot.
  // ****************************************************************

  poleZeroPlot = new PoleZeroPlot(this);
  poleZeroPlot->SetMinSize( wxSize(350, 500) );

  topLevelSizer->Add(poleZeroPlot, 1, wxALL, 3);

  // ****************************************************************
  // Add the frequencies of the poles and zeros.
  // ****************************************************************

  sizer = new wxBoxSizer(wxVERTICAL);

  label = new wxStaticText(this, wxID_ANY, "Frequency (Bandwidth) in Hz");
  sizer->Add(label, 0, wxALL, 3);

  label = new wxStaticText(this, wxID_ANY, "Poles");
  sizer->Add(label, 0, wxALL, 3);

  labPoleLocations = new wxStaticText(this, wxID_ANY, "", wxDefaultPosition, wxSize(80, 150));
  sizer->Add(labPoleLocations, 0, wxALL, 3);

  label = new wxStaticText(this, wxID_ANY, "Zeros");
  sizer->Add(label, 0, wxALL, 3);

  labZeroLocations = new wxStaticText(this, wxID_ANY, "", wxDefaultPosition, wxSize(80, 150));
  sizer->Add(labZeroLocations, 0, wxALL, 3);

  // ... and a check box and buttons

  chkHigherPoleCorrection = new wxCheckBox(this, IDC_HIGHER_POLE_CORRECTION, "Higher pole correction");
  sizer->Add(chkHigherPoleCorrection, 0, wxALL | wxGROW, 3);

  button = new wxButton(this, IDB_ENTER_POLES, "Enter poles");
  sizer->Add(button, 0, wxALL | wxGROW, 3);

  button = new wxButton(this, IDB_ENTER_ZEROS, "Enter zeros");
  button->Disable();
  sizer->Add(button, 0, wxALL | wxGROW, 3);

  button = new wxButton(this, IDB_PLAY_SHORT_VOWEL, "Play short vowel");
  sizer->Add(button, 0, wxALL | wxGROW, 3);

  button = new wxButton(this, IDB_PLAY_LONG_VOWEL, "Play long vowel");
  sizer->Add(button, 0, wxALL | wxGROW, 3);

  topLevelSizer->Add(sizer, 0, wxALL, 3);

  // ****************************************************************
  // Set the top-level-sizer for this window
  // ****************************************************************

  this->SetSizer(topLevelSizer);
  topLevelSizer->Fit(this);
}


// ****************************************************************************
/// This function is called when the pole-zero plot picture wants an update
/// of this dialog, for example, because the user added a pole or a zero.
// ****************************************************************************

void PoleZeroDialog::OnUpdateRequest(wxCommandEvent &event)
{
  updateWidgets();
}


// ****************************************************************************
// Toggle the check box "higher pole correction".
// ****************************************************************************

void PoleZeroDialog::OnHigherPoleCorrection(wxCommandEvent &event)
{
  plan->higherPoleCorrection = !plan->higherPoleCorrection;
  updateWidgets();
}


// ****************************************************************************
/// The user can enter numerical values for the poles.
// ****************************************************************************

void PoleZeroDialog::OnEnterPoles(wxCommandEvent &event)
{
  static wxString lastString = "500 1500 2500";

  wxString st = wxGetTextFromUser(
    "Please enter 1-4 pole/formant frequencies separated by spaces.\n"
    "Example: \"500 1500 2500\"",
    "Input formant frequencies",
    lastString, this, -1, -1, true);

  if (st.IsEmpty())
  {
    return;
  }

  // ****************************************************************
  // Parse the string for the three numbers.
  // ****************************************************************
  
  const int MAX_POLES = 4;
  double poleFreq[MAX_POLES] = { 0.0 };

  istringstream is(st.ToStdString());
  is >> poleFreq[0];
  is >> poleFreq[1];
  is >> poleFreq[2];
  is >> poleFreq[3];

  // ****************************************************************
  // Sort the list of poles.
  // ****************************************************************

  vector<PoleZeroPlan::Location> tempList;
  plan->sortLocations(plan->poles, tempList);
  plan->poles = tempList;

  // ****************************************************************
  // Set the entered pole frequencies.
  // ****************************************************************

  const double MIN_FREQ = 1.0;
  const double MAX_FREQ = 6000.0;
  int i;

  for (i=0; ((i < MAX_POLES) && (poleFreq[i] > 0.0) && (poleFreq[i] >= MIN_FREQ) && (poleFreq[i] <= MAX_FREQ)); i++)
  {
    if ((int)plan->poles.size() > i)
    {
      plan->poles[i].freq_Hz = poleFreq[i];
    }
    else
    {
      PoleZeroPlan::Location location;
      location.freq_Hz = poleFreq[i];
      location.bw_Hz = 200.0;
      plan->poles.push_back(location);
    }
  }

  // Keep in mind the entered values.
  lastString = st;

  updateWidgets();
}


// ****************************************************************************
/// The user can enter numerical values for the zeros.
// ****************************************************************************

void PoleZeroDialog::OnEnterZeros(wxCommandEvent &event)
{

}


// ****************************************************************************
// ****************************************************************************

void PoleZeroDialog::OnPlayShortVowel(wxCommandEvent &event)
{
  data->track[Data::MAIN_TRACK]->setZero();

  int duration_ms = data->synthesizeVowelFormantLf(data->lfPulse, 0, false);

  if (waveStartPlaying(data->track[Data::MAIN_TRACK]->x, data->track[Data::MAIN_TRACK]->N, false))
  {
    wxMilliSleep(duration_ms);
    waveStopPlaying();
  }
  else
  {
    wxMessageBox("Playing failed.", "Attention!");
  }
}

// ****************************************************************************
// ****************************************************************************

void PoleZeroDialog::OnPlayLongVowel(wxCommandEvent &event)
{
  data->track[Data::MAIN_TRACK]->setZero();

  int duration_ms = data->synthesizeVowelFormantLf(data->lfPulse, 0, true);

  if (waveStartPlaying(data->track[Data::MAIN_TRACK]->x, data->track[Data::MAIN_TRACK]->N, false))
  {
    wxMilliSleep(duration_ms);
    waveStopPlaying();
  }
  else
  {
    wxMessageBox("Playing failed.", "Attention!");
  }
}

// ****************************************************************************

