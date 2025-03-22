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
    
    // Store points for the Bezier curve
    std::vector<wxPoint> controlPoints;
    int selectedPointIndex = -1;  // -1 means no point is selected
    const int POINT_RADIUS = 6;
    const int MAX_CONTROL_POINTS = 4;
    
    // Zoom related variables
    double zoomFactor = 1.0;
    wxPoint panOffset = wxPoint(0, 0);
    wxPoint dragStart;
    bool isPanning = false;
    
    // Convert between screen and world coordinates
    wxPoint WorldToScreen(const wxPoint& point);
    wxPoint ScreenToWorld(const wxPoint& point);
    
    // Mouse event handlers
    void OnMouseDown(wxMouseEvent& event);
    void OnMouseUp(wxMouseEvent& event);
    void OnMouseMove(wxMouseEvent& event);
    void OnMouseWheel(wxMouseEvent& event);
    void OnPaint(wxPaintEvent& event);
    void OnZoomSlider(wxCommandEvent& event);
    void OnKeyDown(wxKeyEvent& event);
    
    // Helper methods
    int HitTest(const wxPoint& pos);
    void InitializeDefaultPoints();
    void SetZoom(double zoom);
    
    // Draw the Bezier curve using control points
    void DrawBezierCurve(wxDC& dc);
};

#endif // BEZIERCURVEPANEL_H 