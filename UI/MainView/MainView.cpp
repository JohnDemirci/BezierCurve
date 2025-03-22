#include "MainView.h"

MainView::MainView(const wxString& title)
    : wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxSize(600, 400))
{
    wxPanel* panel = new wxPanel(this, wxID_ANY);
    notebook = new wxNotebook(panel, wxID_ANY, wxPoint(0, 0), wxSize(600, 400));

    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(notebook, 1, wxEXPAND | wxALL, 5);
    panel->SetSizer(sizer);

    CreateMenuBar();

    // Add an initial tab with a Bezier curve panel
    BezierCurvePanel* initialTab = new BezierCurvePanel(notebook, "Bezier Curve 1");
    notebook->AddPage(initialTab, "Bezier Curve 1");

    Bind(wxEVT_MENU, &MainView::OnAddTab, this, ID_NEW_TAB);
    Bind(wxEVT_MENU, &MainView::OnCloseTab, this, ID_CLOSE_TAB);

    panel->Bind(wxEVT_KEY_DOWN, &MainView::OnKeyDown, this);

    panel->SetFocus();
}

void MainView::CreateMenuBar() {
    wxMenuBar* menuBar = new wxMenuBar;
    wxMenu* fileMenu = new wxMenu;
    
    fileMenu->Append(ID_NEW_TAB, "New Tab\tCtrl+T");
    
    fileMenu->Append(ID_CLOSE_TAB, "Close Tab\tCtrl+W");
    
    menuBar->Append(fileMenu, "File");
    SetMenuBar(menuBar);
}

void MainView::OnAddTab(wxCommandEvent& event) {
    int tabCount = notebook->GetPageCount() + 1;
    // Create a Bezier curve panel for the new tab
    BezierCurvePanel* newTab = new BezierCurvePanel(notebook, wxString::Format("Bezier Curve %d", tabCount));
    notebook->AddPage(newTab, wxString::Format("Bezier Curve %d", tabCount));
}

void MainView::OnCloseTab(wxCommandEvent& event) {
    int selection = notebook->GetSelection();
    if (selection != wxNOT_FOUND && notebook->GetPageCount() > 1) {
        notebook->DeletePage(selection);
    }
}

void MainView::OnKeyDown(wxKeyEvent& event) {
    if (event.GetModifiers() == wxMOD_CMD) {
        if (event.GetKeyCode() == 'T') {
            wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, ID_NEW_TAB);
            ProcessEvent(evt);
        }
        else if (event.GetKeyCode() == 'W') {
            wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, ID_CLOSE_TAB);
            ProcessEvent(evt);
        }
    } else {
        event.Skip(); 
    }
}