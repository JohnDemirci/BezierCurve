#ifndef MAINVIEW_H
#define MAINVIEW_H

#include <wx/wx.h>
#include <wx/notebook.h>
#include "../BezierCurvePanel/BezierCurvePanel.h"
// Define a custom event ID
enum {
    ID_NEW_TAB = wxID_HIGHEST + 1,
    ID_CLOSE_TAB
};

class MainView : public wxFrame {
public:
    MainView(const wxString& title);

private:
    wxNotebook* notebook;
    void OnAddTab(wxCommandEvent& event);
    void OnCloseTab(wxCommandEvent& event);
    void OnKeyDown(wxKeyEvent& event);
    void CreateMenuBar();
};

#endif // MAINVIEW_H