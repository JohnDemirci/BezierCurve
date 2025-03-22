#include <wx/wx.h>
#include "UI/MainView/MainView.h"

class MyApp : public wxApp {
public:
    virtual bool OnInit();
};

wxIMPLEMENT_APP(MyApp);

bool MyApp::OnInit() {
    MainView* frame = new MainView("Hello World from wxWidgets");
    frame->Show(true);
    return true;
}
