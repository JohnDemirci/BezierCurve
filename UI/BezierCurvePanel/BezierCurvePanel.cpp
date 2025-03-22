#include "BezierCurvePanel.h"
#include <wx/dcbuffer.h>

BezierCurvePanel::BezierCurvePanel(wxWindow* parent, const wxString& name)
    : wxPanel(parent, wxID_ANY)
{
    // Enable double buffering to prevent flickering during redraw
    SetBackgroundStyle(wxBG_STYLE_PAINT);
    
    // Set white background
    SetBackgroundColour(*wxWHITE);
    
    CreateControls();
    
    // Initialize with default control points
    InitializeDefaultPoints();
    
    // Set panel name
    nameLabel->SetLabelText(name);
    
    // Bind mouse events
    Bind(wxEVT_LEFT_DOWN, &BezierCurvePanel::OnMouseDown, this);
    Bind(wxEVT_LEFT_UP, &BezierCurvePanel::OnMouseUp, this);
    Bind(wxEVT_MOTION, &BezierCurvePanel::OnMouseMove, this);
    Bind(wxEVT_PAINT, &BezierCurvePanel::OnPaint, this);
}

void BezierCurvePanel::CreateControls()
{
    // Create a sizer for the panel
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
    
    // Create a sizer for buttons
    wxBoxSizer* buttonSizer = new wxBoxSizer(wxHORIZONTAL);
    
    // Add the name label
    nameLabel = new wxStaticText(this, wxID_ANY, "Bezier Curve Panel");
    mainSizer->Add(nameLabel, 0, wxALL | wxALIGN_CENTER, 5);
    
    // Add the buttons
    resetPointsButton = new wxButton(this, wxID_ANY, "Reset Points");
    clearPointsButton = new wxButton(this, wxID_ANY, "Clear Points");
    
    buttonSizer->Add(resetPointsButton, 0, wxALL, 5);
    buttonSizer->Add(clearPointsButton, 0, wxALL, 5);
    
    mainSizer->Add(buttonSizer, 0, wxALIGN_CENTER);
    
    // Add a spacer to push everything to the top
    mainSizer->AddStretchSpacer(1);
    
    SetSizer(mainSizer);
    
    // Bind button events
    resetPointsButton->Bind(wxEVT_BUTTON, &BezierCurvePanel::OnResetPointsClick, this);
    clearPointsButton->Bind(wxEVT_BUTTON, &BezierCurvePanel::OnClearPointsClick, this);
}

void BezierCurvePanel::InitializeDefaultPoints()
{
    controlPoints.clear();
    
    // Get the size of the panel
    wxSize size = GetClientSize();
    int width = size.GetWidth();
    int height = size.GetHeight();
    
    // If the panel size is not yet initialized, use default values
    if (width < 100 || height < 100) {
        width = 600;
        height = 400;
    }
    
    // Create default control points for a cubic Bezier curve
    controlPoints.push_back(wxPoint(width * 0.1, height * 0.5));  // First point (left)
    controlPoints.push_back(wxPoint(width * 0.3, height * 0.2));  // Second point (control)
    controlPoints.push_back(wxPoint(width * 0.7, height * 0.8));  // Third point (control)
    controlPoints.push_back(wxPoint(width * 0.9, height * 0.5));  // Fourth point (right)
    
    Refresh();
}

int BezierCurvePanel::HitTest(const wxPoint& pos)
{
    // Check if the position is over any control point
    for (size_t i = 0; i < controlPoints.size(); ++i) {
        wxPoint diff = pos - controlPoints[i];
        int distanceSquared = diff.x * diff.x + diff.y * diff.y;
        
        // If the distance is less than the point radius, return the index
        if (distanceSquared <= POINT_RADIUS * POINT_RADIUS) {
            return i;
        }
    }
    
    return -1; // No point was hit
}

void BezierCurvePanel::OnMouseDown(wxMouseEvent& event)
{
    // Try to select a control point
    selectedPointIndex = HitTest(event.GetPosition());
    
    // If no point was selected and we have less than MAX_CONTROL_POINTS, add a new point
    if (selectedPointIndex == -1 && controlPoints.size() < MAX_CONTROL_POINTS) {
        controlPoints.push_back(event.GetPosition());
        selectedPointIndex = controlPoints.size() - 1;
    }
    
    Refresh();
    event.Skip();
}

void BezierCurvePanel::OnMouseUp(wxMouseEvent& event)
{
    selectedPointIndex = -1; // Deselect any selected point
    event.Skip();
}

void BezierCurvePanel::OnMouseMove(wxMouseEvent& event)
{
    if (selectedPointIndex != -1 && event.Dragging() && event.LeftIsDown()) {
        // Move the selected control point
        controlPoints[selectedPointIndex] = event.GetPosition();
        Refresh();
    }
    event.Skip();
}

void BezierCurvePanel::OnPaint(wxPaintEvent& event)
{
    // Create a buffered DC to prevent flickering
    wxAutoBufferedPaintDC dc(this);
    dc.Clear();
    
    // Draw the control points and their connections
    dc.SetPen(*wxBLACK_PEN);
    
    // Draw lines connecting control points
    if (controlPoints.size() > 1) {
        dc.SetPen(wxPen(*wxBLUE, 1, wxPENSTYLE_DOT));
        for (size_t i = 0; i < controlPoints.size() - 1; ++i) {
            dc.DrawLine(controlPoints[i], controlPoints[i + 1]);
        }
    }
    
    // Draw the Bezier curve if we have all 4 control points
    if (controlPoints.size() == MAX_CONTROL_POINTS) {
        DrawBezierCurve(dc);
    }
    
    // Draw the control points on top
    for (size_t i = 0; i < controlPoints.size(); ++i) {
        // Use different color for selected point
        if (static_cast<int>(i) == selectedPointIndex) {
            dc.SetBrush(*wxYELLOW_BRUSH);
        } else {
            dc.SetBrush(*wxRED_BRUSH);
        }
        
        dc.SetPen(*wxBLACK_PEN);
        dc.DrawCircle(controlPoints[i], POINT_RADIUS);
        
        // Draw point number
        wxString pointLabel = wxString::Format("%zu", i+1);
        wxSize textSize = dc.GetTextExtent(pointLabel);
        dc.DrawText(pointLabel, wxPoint(controlPoints[i].x - textSize.GetWidth()/2, 
                                        controlPoints[i].y - textSize.GetHeight()/2));
    }
}

void BezierCurvePanel::OnResetPointsClick(wxCommandEvent& event)
{
    InitializeDefaultPoints();
}

void BezierCurvePanel::OnClearPointsClick(wxCommandEvent& event)
{
    controlPoints.clear();
    Refresh();
}

// Recursive De Casteljau's algorithm to calculate points on a Bezier curve
wxPoint DeCasteljau(const std::vector<wxPoint>& points, double t)
{
    if (points.size() == 1) {
        return points[0];
    }
    
    std::vector<wxPoint> newPoints;
    for (size_t i = 0; i < points.size() - 1; ++i) {
        wxPoint p1 = points[i];
        wxPoint p2 = points[i + 1];
        
        int x = p1.x + t * (p2.x - p1.x);
        int y = p1.y + t * (p2.y - p1.y);
        
        newPoints.push_back(wxPoint(x, y));
    }
    
    return DeCasteljau(newPoints, t);
}

void BezierCurvePanel::DrawBezierCurve(wxDC& dc)
{
    if (controlPoints.size() < 2) {
        return;
    }
    
    // Draw the Bezier curve using De Casteljau's algorithm
    dc.SetPen(wxPen(*wxGREEN, 2));
    
    std::vector<wxPoint> curvePoints;
    
    // Calculate 100 points along the curve
    const int STEPS = 100;
    for (int i = 0; i <= STEPS; ++i) {
        double t = static_cast<double>(i) / STEPS;
        curvePoints.push_back(DeCasteljau(controlPoints, t));
    }
    
    // Draw lines connecting the calculated points
    for (size_t i = 0; i < curvePoints.size() - 1; ++i) {
        dc.DrawLine(curvePoints[i], curvePoints[i + 1]);
    }
} 