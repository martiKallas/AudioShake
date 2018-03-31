/*
*  Sources:
*		wxWidgets Samples
*/

#include <wx/wx.h>
#include <wx/notebook.h>
#include <wx/propgrid/propgrid.h>
#include <wx/preferences.h>
#include "AudioInterface.hpp"
#include "ListenLoop.hpp"
#include "GUISession.hpp"
#include "Settings.hpp"

// Should be defined in virtually every app
class MyApp : public wxApp {
public:
	virtual bool OnInit();
	void ShowSettings(wxWindow* parent);
	Settings* getSettings() { return &settings; }
private:
	wxScopedPtr<wxPreferencesEditor> m_prefEditor;
	Settings settings;
};

DECLARE_APP(MyApp)

void MyApp::ShowSettings(wxWindow* parent) {
	if (!m_prefEditor) {
		m_prefEditor.reset(new wxPreferencesEditor);
		m_prefEditor->AddPage(new SettingsPageGeneral);
	}
	m_prefEditor->Show(parent);
	/*TODO: old code:
	if (!settingsWindow) {
		settingsWindow = new SettingsWindow(parent, wxID_ANY, "Settings", wxPoint(50, 50), wxSize(450, 340), &settings);
		wxNotebook *notebook = new wxNotebook(settingsWindow, wxID_ANY);
		SettingsPagePanel *settingsPage = new SettingsPagePanel(notebook, &settings);
		notebook->AddPage(settingsPage, "General", false, NULL);
	}
	settingsWindow->Show(parent);
	*/
}



//Create a main window below by deriving from wxFrame
//	Any class that wants to respond to events (clicks...) must declare an event table
//	The OnHello, OnExit, ... are handlers for events
class MyFrame : public wxFrame {
public:
	MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size);
private:
	std::vector<GUISession*> sessionElements;
	std::vector<sessionID> sessionIDs;
	AudioInterface audioI;
	ListenLoop *lThread;
	bool loopRunning = false;
	void OnHello(wxCommandEvent& event);
	void OnExit(wxCommandEvent& event);
	void OnAbout(wxCommandEvent& event);
	void OnRefresh(wxCommandEvent& event);
	void OnRun(wxCommandEvent& event);
	void OnStop(wxCommandEvent& event);
	void OnSettings(wxCommandEvent& event);
	void ClearElements();
	void RunDisable();
	void StopEnable();
	wxButton * runB;
	wxButton * stopB;
	wxButton * refreshB;
	wxButton * setB;
	wxFlexGridSizer *table;
	wxSizer *mainSizer;
	wxPanel *page;
	wxDECLARE_EVENT_TABLE();
};

//To react to menu command, the command must be given a unique identifier (const variable or enum)
//	Some events have ID's already defined in wxWidgets
enum {
	ID_Run,
	ID_Stop,
	ID_Refresh,
	ID_Settings
};

// The event table is created so that events are routed to the appropriate handlers.
wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
EVT_MENU(wxID_EXIT, MyFrame::OnExit)
EVT_BUTTON(ID_Refresh, MyFrame::OnRefresh)
EVT_BUTTON(ID_Run, MyFrame::OnRun)
EVT_BUTTON(ID_Stop, MyFrame::OnStop)
EVT_BUTTON(ID_Settings, MyFrame::OnSettings)
wxEND_EVENT_TABLE()

//Creates and starts the main function
wxIMPLEMENT_APP(MyApp);

//Define OnInit() to create windows or show a splash screen
bool MyApp::OnInit() {
	MyFrame *frame = new MyFrame("AudioShake v0.0.1", wxPoint(50, 50), wxSize(450, 340));
	//Settings automatically load settings.json
	settings = Settings();
	frame->Show(true);
	return true;
}

//Menu items and buttons can be made in the constructor of the main window
//	or later on

MyFrame::MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
	: wxFrame(NULL, wxID_ANY, title, pos, size) {

	//create the menu
	/*
	wxMenu *menuFile = new wxMenu;
	menuFile->Append(ID_Hello, "&Hello...\tCtrl-H",
	"Help string shown in status bar for this menu item");
	menuFile->AppendSeparator();
	//Don't need to specify labels for wx Exit and wx About
	menuFile->Append(wxID_EXIT);

	wxMenu *menuHelp = new wxMenu;
	menuHelp->Append(wxID_ABOUT);

	wxMenuBar *menuBar = new wxMenuBar;
	menuBar->Append(menuFile, "&File");
	menuBar->Append(menuHelp, "&Help");
	SetMenuBar(menuBar);
	*/
	CreateStatusBar();
	SetStatusText("Welcome to AudioShake!");

	//create tabs
	//wxNotebook *notebook = new wxNotebook(this, wxID_ANY);

	//create main page
	page = new wxPanel(this);

	//Create audio interface
	audioI = AudioInterface();
	audioI.initializeManager();
	loopRunning = false;

	//Generic Placeholder Items
	//wxSizer *row1 = new wxStaticBoxSizer(new wxStaticBox(page, wxID_ANY, wxT("")), wxHORIZONTAL);
	int xPad = 20;
	int yPad = 30;
	table = new wxFlexGridSizer(4, xPad, yPad);
	mainSizer = new wxBoxSizer(wxVERTICAL);
	
	 //Create run, stop and refresh buttons
	wxStaticBoxSizer *buttons = new wxStaticBoxSizer(wxHORIZONTAL, page, wxT(""));
	runB = new wxButton(page, ID_Run, wxT("Run"));
	buttons->Add(runB);
	stopB = new wxButton(page, ID_Stop, wxT("Stop"));
	buttons->Add(stopB);
	refreshB = new wxButton(page, ID_Refresh, wxT("Refresh"));
	buttons->Add(refreshB);
	setB = new wxButton(page, ID_Settings, wxT("Settings"));
	buttons->Add(setB);
	mainSizer->Add(buttons);

	//Labels for aud
	wxStaticText *titleName = new wxStaticText(page, wxID_ANY, wxT("Program Name"));
	table->Add(titleName, -1, wxALIGN_LEFT);
	wxStaticText *titleListen = new wxStaticText(page, wxID_ANY, wxT("Listen To"));
	table->Add(titleListen, -1, wxALIGN_CENTER);
	wxStaticText *titleDim = new wxStaticText(page, wxID_ANY, wxT("Dim"));
	table->Add(titleDim, -1, wxALIGN_CENTER);
	wxStaticText *titleMute = new wxStaticText(page, wxID_ANY, wxT("Mute Key"));
	table->Add(titleMute, -1, wxALIGN_CENTER);

	//Adjust size of window
	mainSizer->Add(table);
	wxSize minTableSize = table->CalcMin();
	minTableSize.x += xPad * 3;
	minTableSize.y += yPad * 2;
	table->SetMinSize(minTableSize);
	wxSize minPageSize = minTableSize;
	minPageSize.y += 100;
	this->SetSizeHints(minPageSize);
	this->SetInitialSize(minPageSize);
	page->SetSizerAndFit(mainSizer);
	
	OnRefresh(wxCommandEvent(wxEVT_COMMAND_BUTTON_CLICKED));

	//create settings tab
}

void MyFrame::OnExit(wxCommandEvent& event) {
	if (loopRunning) {
		lThread->setLoopVar(0);
	}
	Close(true);
	loopRunning = false;
}

void MyFrame::OnRun(wxCommandEvent& event) {
	if (loopRunning) {
		lThread->setLoopVar(0);
		loopRunning = false;
		StopEnable();
	}
	lThread = new ListenLoop(this, &audioI);
	lThread->setMuteKey(audioI.getMuteKey());
	lThread->setQuitKey(audioI.getQuitKey());
	lThread->setRefreshTime(audioI.getRefreshTime());
	audioI.clearLists();
	//If the check boxes are checked, add the session to the appropriate list
	int success = 0;
	for (auto it = sessionElements.begin(); it != sessionElements.end(); ++it){
		if ((*it)->keyBoxChecked()){
			success = audioI.addMuteKeyed((*it)->getSessionID());
		}
		if ((*it)->masterBoxChecked()){
			success = audioI.addMuteMaster((*it)->getSessionID());
		}
		if ((*it)->dependentBoxChecked()){
			success = audioI.addMuteDependent((*it)->getSessionID());
		}
	}		
	loopRunning = true;
	SetStatusText("Listening...");
	RunDisable();
	if (lThread->Run() != wxTHREAD_NO_ERROR) {
		wxLogError(wxT("Can't start thread!"));
	}
}

void MyFrame::OnStop(wxCommandEvent& event) {
	//ListenLoop is a detached thread. Setting loopVar = 0 will terminate the thread
	if (loopRunning) {
		lThread->setLoopVar(0);
		StopEnable();
	}
	loopRunning = false;
}

void MyFrame::OnRefresh(wxCommandEvent& event) {
	audioI.refreshSessions(&sessionIDs);
	ClearElements();
	int exists = 0;
	int elemCount = 0;
	GUISession * newElems;
	for (auto it = sessionIDs.begin(); it != sessionIDs.end(); ++it) {
		newElems = new GUISession(page, *it);
		sessionElements.push_back(newElems);
		table->Add(newElems->getTitle(), -1, wxALIGN_LEFT);
		table->Add(newElems->getMasterBox(), -1, wxALIGN_CENTER);
		table->Add(newElems->getDependentBox(), -1, wxALIGN_CENTER);
		table->Add(newElems->getKeyBox(), -1, wxALIGN_CENTER);
	}
	int xPad = 20;
	int yPad = 30;

	wxSize minTableSize = table->CalcMin();
	minTableSize.x += xPad * 3;
	minTableSize.y += yPad * 2;
	table->SetMinSize(minTableSize);
	wxSize minPageSize = minTableSize;
	minPageSize.y += 100;
	this->SetSizeHints(minPageSize);
	this->SetInitialSize(minPageSize);
	table->Layout();
	page->SetSizerAndFit(mainSizer);
}

void MyFrame::OnSettings(wxCommandEvent& event) {
	wxGetApp().ShowSettings(this);
}

void MyFrame::ClearElements() {
	for (auto it = sessionElements.begin(); it != sessionElements.end(); ++it) {
		delete *it;
	}
	sessionElements.clear();
}

void MyFrame::RunDisable() {
	runB->Disable();
	refreshB->Disable();
	setB->Disable();
	for (auto it = sessionElements.begin(); it != sessionElements.end(); ++it) {
		(*it)->disableButtons();
	}
}

void MyFrame::StopEnable() {
	runB->Enable();
	refreshB->Enable();
	setB->Enable();
	for (auto it = sessionElements.begin(); it != sessionElements.end(); ++it) {
		(*it)->enableButtons();
	}
}
