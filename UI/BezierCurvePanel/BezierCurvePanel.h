#ifndef BEZIERCURVEPANEL_H
#define BEZIERCURVEPANEL_H

#include <wx/wx.h>
#include <vector>

class BezierCurvePanel : public wxPanel {
public:
    BezierCurvePanel(wxWindow* parent, const wxString& name);

private:
    void CreateControls();
    wxStaticText* nameLabel;
    wxButton* clearPointsButton;
    wxButton* resetPointsButton;
    
    // Store points for the Bezier curve
    std::vector<wxPoint> controlPoints;
    int selectedPointIndex = -1;  // -1 means no point is selected
    const int POINT_RADIUS = 6;
    const int MAX_CONTROL_POINTS = 4;
    
    // Mouse event handlers
    void OnMouseDown(wxMouseEvent& event);
    void OnMouseUp(wxMouseEvent& event);
    void OnMouseMove(wxMouseEvent& event);
    void OnPaint(wxPaintEvent& event);
    void OnClearPointsClick(wxCommandEvent& event);
    void OnResetPointsClick(wxCommandEvent& event);
    
    // Helper methods
    int HitTest(const wxPoint& pos);
    void InitializeDefaultPoints();
    
    // Draw the Bezier curve using control points
    void DrawBezierCurve(wxDC& dc);
};

#endif // BEZIERCURVEPANEL_H 