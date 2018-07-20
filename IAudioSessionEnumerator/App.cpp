#include "App.hpp"
//Define OnInit() to create windows or show a splash screen

bool MyApp::OnInit() {
	MyFrame *frame = new MyFrame("AudioShake v0.0.1", wxPoint(50, 50), wxSize(450, 340));
	//Settings automatically load settings.json
	settings = Settings();
	frame->Show(true);
	return true;
}