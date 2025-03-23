#include "MainView.h"

/**
 * initializer for the MainView
 * @param title The title of the main view
 */
MainView::MainView(
    const wxString& title
) : wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxSize(600, 400)) {
    wxPanel* panel = new wxPanel(this, wxID_ANY);
    notebook = new wxNotebook(panel, wxID_ANY, wxPoint(0, 0), wxSize(600, 400));

    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(notebook, 1, wxEXPAND | wxALL, 5);
    panel->SetSizer(sizer);

    createMenuBar();

    BezierCurvePanel* initialTab = new BezierCurvePanel(notebook, "Bezier Curve 1");
    notebook->AddPage(initialTab, "Bezier Curve 1");

    Bind(wxEVT_MENU, &MainView::onAddTab, this, ID_NEW_TAB);
    Bind(wxEVT_MENU, &MainView::onCloseTab, this, ID_CLOSE_TAB);

    panel->Bind(wxEVT_KEY_DOWN, &MainView::onKeyDown, this);

    panel->SetFocus();
}

/**
 * creates the menu bar for the main view
 */
void MainView::createMenuBar() {
    wxMenuBar* menuBar = new wxMenuBar;
    wxMenu* fileMenu = new wxMenu;
    
    fileMenu->Append(ID_NEW_TAB, "New Tab\tCtrl+T");
    
    fileMenu->Append(ID_CLOSE_TAB, "Close Tab\tCtrl+W");
    
    menuBar->Append(fileMenu, "File");
    SetMenuBar(menuBar);
}

/**
 * typically used for keyboard shortcuts to add a new tab or close a tab.
 * @param event The key event
 */
void MainView::onAddTab(wxCommandEvent& event) {
    int tabCount = notebook->GetPageCount() + 1;
    BezierCurvePanel* newTab = new BezierCurvePanel(notebook, wxString::Format("Bezier Curve %d", tabCount));
    notebook->AddPage(newTab, wxString::Format("Bezier Curve %d", tabCount));
}

/**
 * This is automatically called when a key is pressed
 * observes the modifier key and the key code to determine the action.
 * typically used for keyboard shortcuts to add a new tab or close a tab.
 * @param event The key event
 */
void MainView::onCloseTab(wxCommandEvent& event) {
    int selection = notebook->GetSelection();
    if (selection != wxNOT_FOUND && notebook->GetPageCount() > 1) {
        notebook->DeletePage(selection);
    }
}

/**
 * This is automatically called when a key is pressed
 * observes the modifier key and the key code to determine the action.
 * typically used for keyboard shortcuts to add a new tab or close a tab.
 * @param event The key event
 */
void MainView::onKeyDown(wxKeyEvent& event) {
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