#include <wx/wx.h>
#include <wx/notebook.h>
#include <wx/propgrid/propgrid.h>
#include "AudioInterface.hpp"
#include "ListenLoop.hpp"

// Should be defined in virtually every app
class MyApp : public wxApp {
public:
	virtual bool OnInit();
};

//Create a main window below by deriving from wxFrame
//	Any class that wants to respond to events (clicks...) must declare an event table
//	The OnHello, OnExit, ... are handlers for events
class MyFrame : public wxFrame {
public:
	MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size);
private:
	LoopThread *lThread;
	void OnHello(wxCommandEvent& event);
	void OnExit(wxCommandEvent& event);
	void OnAbout(wxCommandEvent& event);
	void OnAdd(wxCommandEvent& event);
	void OnRun(wxCommandEvent& event);
	void OnLoop(wxCommandEvent& event);
	void StopLoop(wxCommandEvent& event);
	wxFlexGridSizer *table;
	wxPanel *page;
	wxDECLARE_EVENT_TABLE();
};

//To react to menu command, the command must be given a unique identifier (const variable or enum)
//	Some events have ID's already defined in wxWidgets
enum {
	ID_Hello = 1,
	ID_CheckBox1 = 2,
	ID_AddRow = 3,
	ID_DeleteRow = 4,
	ID_LoopStart = ID_THREAD_UPDATE,
	ID_LoopEnd
};

// The event table is created so that events are routed to the appropriate handlers.
wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
EVT_MENU(ID_Hello, MyFrame::OnHello)
EVT_MENU(wxID_EXIT, MyFrame::OnExit)
EVT_MENU(wxID_ABOUT, MyFrame::OnAbout)
EVT_BUTTON(ID_AddRow, MyFrame::OnAdd)
EVT_BUTTON(ID_LoopStart, MyFrame::OnLoop)
EVT_BUTTON(ID_LoopEnd, MyFrame::StopLoop)
wxEND_EVENT_TABLE()

//Creates and starts the main function
wxIMPLEMENT_APP(MyApp);

//Define OnInit() to create windows or show a splash screen
bool MyApp::OnInit() {
	MyFrame *frame = new MyFrame("Hello World", wxPoint(50, 50), wxSize(450, 340));
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
	SetStatusText("Welcome to wxWidgets!");

	//create tabs
	//wxNotebook *notebook = new wxNotebook(this, wxID_ANY);

	//create main tab
	page = new wxPanel(this);

	//Create loop thread
	lThread = new LoopThread(this);

	//Generic Placeholder Items
	//wxSizer *row1 = new wxStaticBoxSizer(new wxStaticBox(page, wxID_ANY, wxT("")), wxHORIZONTAL);
	int xPad = 20;
	int yPad = 30;
	table = new wxFlexGridSizer(4, xPad, yPad);
	wxSizer *mainSizer = new wxBoxSizer(wxVERTICAL); //buttons and table
	wxStaticText *titleName = new wxStaticText(page, wxID_ANY, wxT("Program Name"));
	table->Add(titleName, -1, wxALIGN_LEFT);
	wxStaticText *titleListen = new wxStaticText(page, wxID_ANY, wxT("Listen To"));
	table->Add(titleListen, -1, wxALIGN_CENTER);
	wxStaticText *titleDim = new wxStaticText(page, wxID_ANY, wxT("Dim"));
	table->Add(titleDim, -1, wxALIGN_CENTER);
	wxStaticText *titleMute = new wxStaticText(page, wxID_ANY, wxT("Mute Key"));
	table->Add(titleMute, -1, wxALIGN_CENTER);
	wxStaticText *name1 = new wxStaticText(page, wxID_ANY, wxT("&Placholder EXE Name 1"));
	table->Add(name1, -1, wxALIGN_CENTER);
	wxCheckBox *box1r1 = new wxCheckBox(page, wxID_ANY, wxT(""));
	table->Add(box1r1, -1, wxALIGN_CENTER);
	wxCheckBox *box2r1 = new wxCheckBox(page, wxID_ANY, wxT(""));
	table->Add(box2r1, -1, wxALIGN_CENTER);
	wxCheckBox *box3r1 = new wxCheckBox(page, wxID_ANY, wxT(""));
	table->Add(box3r1, -1, wxALIGN_CENTER);
	mainSizer->Add(table);
	//Create Add/Delete buttons -> now run/stop
	wxStaticBoxSizer *buttons = new wxStaticBoxSizer(wxHORIZONTAL, page, wxT(""));
	wxButton *addB = new wxButton(page, ID_LoopStart, wxT("Run"));
	buttons->Add(addB);
	wxButton *deleteB = new wxButton(page, ID_LoopEnd, wxT("Stop"));
	buttons->Add(deleteB);
	mainSizer->Add(buttons);

	//Adjust size of window
	wxSize minTableSize = table->CalcMin();
	minTableSize.x += xPad * 3;
	minTableSize.y += yPad * 2;
	table->SetMinSize(minTableSize);
	wxSize minPageSize = minTableSize;
	minPageSize.y += 100;
	this->SetSizeHints(minPageSize);
	this->SetInitialSize(minPageSize);
	page->SetSizerAndFit(mainSizer);

	//	notebook->AddPage(page, wxT("Main"));
	//	notebook->CalcSizeFromPage(table->CalcMin());


	//create settings tab
	//	page = new wxPanel(notebook);

	//	notebook->AddPage(page, wxT("Settings"));
}


void MyFrame::OnAdd(wxCommandEvent& event) {
	wxStaticText *newLabel = new wxStaticText(page, wxID_ANY, wxT("New Label"));
	table->Add(newLabel);
	for (int i = 0; i < 3; i++) {
		wxCheckBox *newBox = new wxCheckBox(page, wxID_ANY, wxT(""));
		table->Add(newBox, -1, wxALIGN_CENTER);
	}
	table->CalcMin();
	page->Layout();

}

void MyFrame::OnExit(wxCommandEvent& event) {
	Close(true);
}

void MyFrame::OnAbout(wxCommandEvent& event) {
	wxMessageBox("This is a wxWidgets' Hello World sample", "About Hello World", wxOK | wxICON_INFORMATION);
}

void MyFrame::OnHello(wxCommandEvent& event) {
	wxLogMessage("Hello world from wxWidgets!");
}

void MyFrame::OnRun(wxCommandEvent& event) {

}

void MyFrame::StopLoop(wxCommandEvent& event) {
	lThread->setLoopVar(0);
}

void MyFrame::OnLoop(wxCommandEvent& event) {
	if (lThread->Run() != wxTHREAD_NO_ERROR) {
		wxLogError(wxT("Can't start thread!"));
	}
}
