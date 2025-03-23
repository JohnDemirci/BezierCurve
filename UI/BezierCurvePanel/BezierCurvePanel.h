#ifndef BEZIERCURVEPANEL_H
#define BEZIERCURVEPANEL_H

#include <wx/wx.h>
#include <wx/slider.h>
#include <vector>

class BezierCurvePanel : public wxPanel {
public:
    BezierCurvePanel(wxWindow* parent, const wxString& name);

private:
    void CreateControls();
    wxStaticText* nameLabel;
    wxSlider* zoomSlider;
    wxStaticText* zoomLabel;
    
    std::vector<wxPoint> controlPoints;
    int selectedPointIndex = -1;
    const int POINT_RADIUS = 6;
    const int MAX_CONTROL_POINTS = 4;
    
    double zoomFactor = 1.0;
    wxPoint panOffset = wxPoint(0, 0);
    wxPoint dragStart;
    bool isPanning = false;
    
    wxPoint worldToScreen(const wxPoint& point);
    wxPoint screenToWorld(const wxPoint& point);
    
    void onMouseDown(wxMouseEvent& event);
    void onMouseUp(wxMouseEvent& event);
    void onMouseMove(wxMouseEvent& event);
    void onMouseWheel(wxMouseEvent& event);
    void onPaint(wxPaintEvent& event);
    void onZoomSlider(wxCommandEvent& event);
    void onKeyDown(wxKeyEvent& event);
    
    int checkForControlPointClick(const wxPoint& pos);
    void initializeDefaultPoints();
    void setZoom(double zoom);
    
    void drawBezierCurve(wxDC& dc);
};

#endif // BEZIERCURVEPANEL_H 