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
    Bind(wxEVT_MOUSEWHEEL, &BezierCurvePanel::OnMouseWheel, this);
    Bind(wxEVT_PAINT, &BezierCurvePanel::OnPaint, this);
    Bind(wxEVT_KEY_DOWN, &BezierCurvePanel::OnKeyDown, this);
    
    // Enable keyboard input for this panel
    SetFocus();
}

void BezierCurvePanel::CreateControls() {
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
    
    wxBoxSizer* topSizer = new wxBoxSizer(wxHORIZONTAL);
    
    nameLabel = new wxStaticText(this, wxID_ANY, "Bezier Curve Panel");
    topSizer->Add(nameLabel, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
    
    topSizer->AddStretchSpacer(1);
    
    zoomLabel = new wxStaticText(this, wxID_ANY, "Zoom: 100%");
    topSizer->Add(zoomLabel, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
    
    zoomSlider = new wxSlider(
        this, 
        wxID_ANY, 
        100, 
        10, 
        500, 
        wxDefaultPosition, 
        wxSize(150, -1),
        wxSL_HORIZONTAL
    );

    topSizer->Add(zoomSlider, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
    
    mainSizer->Add(topSizer, 0, wxEXPAND | wxALL, 5);
    
    mainSizer->AddStretchSpacer(1);
    
    SetSizer(mainSizer);
    
    zoomSlider->Bind(wxEVT_SLIDER, &BezierCurvePanel::OnZoomSlider, this);
}

void BezierCurvePanel::InitializeDefaultPoints() {
    controlPoints.clear();
    
    // Reset zoom and pan
    zoomFactor = 1.0;
    panOffset = wxPoint(0, 0);
    if (zoomSlider) {
        zoomSlider->SetValue(100);
        zoomLabel->SetLabelText("Zoom: 100%");
    }
    
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

wxPoint BezierCurvePanel::WorldToScreen(const wxPoint& point)
{
    // Convert world coordinates to screen coordinates
    int x = (point.x * zoomFactor) + panOffset.x;
    int y = (point.y * zoomFactor) + panOffset.y;
    return wxPoint(x, y);
}

wxPoint BezierCurvePanel::ScreenToWorld(const wxPoint& point)
{
    // Convert screen coordinates to world coordinates
    int x = (point.x - panOffset.x) / zoomFactor;
    int y = (point.y - panOffset.y) / zoomFactor;
    return wxPoint(x, y);
}

void BezierCurvePanel::SetZoom(double zoom)
{
    // Center of the view in world coordinates before zoom
    wxSize size = GetClientSize();
    wxPoint center = ScreenToWorld(wxPoint(size.GetWidth() / 2, size.GetHeight() / 2));
    
    // Set the new zoom factor
    zoomFactor = zoom;
    
    // Update the zoom label
    int zoomPercent = static_cast<int>(zoomFactor * 100);
    zoomLabel->SetLabelText(wxString::Format("Zoom: %d%%", zoomPercent));
    zoomSlider->SetValue(zoomPercent);
    
    // Calculate new panOffset to keep the center of the view fixed
    wxPoint newCenter = WorldToScreen(center);
    panOffset.x += (size.GetWidth() / 2) - newCenter.x;
    panOffset.y += (size.GetHeight() / 2) - newCenter.y;
    
    Refresh();
}

int BezierCurvePanel::HitTest(const wxPoint& pos)
{
    // Check if the position is over any control point
    for (size_t i = 0; i < controlPoints.size(); ++i) {
        wxPoint screenPos = WorldToScreen(controlPoints[i]);
        wxPoint diff = pos - screenPos;
        int distanceSquared = diff.x * diff.x + diff.y * diff.y;
        
        // Scale the hit radius by zoom factor
        int scaledRadius = POINT_RADIUS * zoomFactor;
        
        // If the distance is less than the point radius, return the index
        if (distanceSquared <= scaledRadius * scaledRadius) {
            return i;
        }
    }
    
    return -1; // No point was hit
}

void BezierCurvePanel::OnMouseDown(wxMouseEvent& event)
{
    SetFocus(); // Ensure panel gets keyboard events
    
    // Try to select a control point first
    selectedPointIndex = HitTest(event.GetPosition());
    
    if (selectedPointIndex == -1) {
        // If middle button or control+left button, start panning
        if (event.MiddleDown() || (event.LeftDown() && event.ControlDown())) {
            isPanning = true;
            dragStart = event.GetPosition();
            SetCursor(wxCursor(wxCURSOR_HAND));
        }
        // If no point was selected and left button without control, 
        // either add a new point (if we have less than MAX)
        // or start panning if we already have all points
        else if (event.LeftDown() && !event.ControlDown()) {
            if (controlPoints.size() < MAX_CONTROL_POINTS) {
                controlPoints.push_back(ScreenToWorld(event.GetPosition()));
                selectedPointIndex = controlPoints.size() - 1;
            } else {
                // We already have all control points, so start panning instead
                isPanning = true;
                dragStart = event.GetPosition();
                SetCursor(wxCursor(wxCURSOR_HAND));
            }
        }
    }
    
    Refresh();
    event.Skip();
}

void BezierCurvePanel::OnMouseUp(wxMouseEvent& event) {
    if (isPanning) {
        isPanning = false;
        SetCursor(wxCursor(wxCURSOR_ARROW));
    }
    
    selectedPointIndex = -1; // Deselect any selected point
    event.Skip();
}

void BezierCurvePanel::OnMouseMove(wxMouseEvent& event) {
    if (selectedPointIndex != -1 && event.Dragging() && event.LeftIsDown()) {
        // Move the selected control point in world coordinates
        controlPoints[selectedPointIndex] = ScreenToWorld(event.GetPosition());
        Refresh();
    }
    else if (isPanning && event.Dragging()) {
        // Pan the view
        wxPoint currentPos = event.GetPosition();
        wxPoint delta = currentPos - dragStart;
        
        panOffset.x += delta.x;
        panOffset.y += delta.y;
        
        dragStart = currentPos;
        Refresh();
    }
    event.Skip();
}

void BezierCurvePanel::OnMouseWheel(wxMouseEvent& event) {
    // Use the mouse wheel for vertical panning instead of zooming
    int wheelRotation = event.GetWheelRotation();
    int panAmount = wheelRotation / 3; // Make panning speed reasonable
    
    if (event.ShiftDown()) {
        panOffset.x += panAmount;
    } else {
        panOffset.y += panAmount;
    }
    
    Refresh();
    event.Skip();
}

void BezierCurvePanel::OnKeyDown(wxKeyEvent& event) {
    if (event.GetModifiers() == wxMOD_CMD) {
        if (event.GetKeyCode() == '+' || event.GetKeyCode() == WXK_NUMPAD_ADD) {
            double newZoom = zoomFactor + 0.1;
            if (newZoom > 5.0) newZoom = 5.0;
            SetZoom(newZoom);
        }
        else if (event.GetKeyCode() == '-' || event.GetKeyCode() == WXK_NUMPAD_SUBTRACT) {
            double newZoom = zoomFactor - 0.1;
            if (newZoom < 0.1) newZoom = 0.1;
            SetZoom(newZoom);
        }
        else if (event.GetKeyCode() == '0' || event.GetKeyCode() == WXK_NUMPAD0) {
            zoomFactor = 1.0;
            panOffset = wxPoint(0, 0);
            SetZoom(1.0);
        }
    }
    
    event.Skip();
}

void BezierCurvePanel::OnZoomSlider(wxCommandEvent& event) {
    double newZoom = zoomSlider->GetValue() / 100.0;
    SetZoom(newZoom);
}

void BezierCurvePanel::OnPaint(wxPaintEvent& event) {
    wxAutoBufferedPaintDC dc(this);
    dc.Clear();
    
    if (zoomFactor >= 0.5) {
        dc.SetPen(wxPen(wxColour(240, 240, 240), 1));
        wxSize size = GetClientSize();
        
        int gridSpacing = static_cast<int>(20 * zoomFactor);
        if (gridSpacing < 5) gridSpacing = 5;
        
        int startX = panOffset.x % gridSpacing;
        int startY = panOffset.y % gridSpacing;
        
        for (int x = startX; x < size.GetWidth(); x += gridSpacing) {
            dc.DrawLine(x, 0, x, size.GetHeight());
        }
        
        // Draw horizontal grid lines
        for (int y = startY; y < size.GetHeight(); y += gridSpacing) {
            dc.DrawLine(0, y, size.GetWidth(), y);
        }
    }
    
    // Draw the control points and their connections
    dc.SetPen(*wxBLACK_PEN);
    
    // Draw lines connecting control points
    if (controlPoints.size() > 1) {
        dc.SetPen(wxPen(*wxBLUE, 1, wxPENSTYLE_DOT));
        for (size_t i = 0; i < controlPoints.size() - 1; ++i) {
            wxPoint p1 = WorldToScreen(controlPoints[i]);
            wxPoint p2 = WorldToScreen(controlPoints[i + 1]);
            dc.DrawLine(p1, p2);
        }
    }
    
    // Draw the Bezier curve if we have all 4 control points
    if (controlPoints.size() == MAX_CONTROL_POINTS) {
        DrawBezierCurve(dc);
    }
    
    // Draw the control points on top
    for (size_t i = 0; i < controlPoints.size(); ++i) {
        wxPoint screenPos = WorldToScreen(controlPoints[i]);
        
        // Use different color for selected point
        if (static_cast<int>(i) == selectedPointIndex) {
            dc.SetBrush(*wxYELLOW_BRUSH);
        } else {
            dc.SetBrush(*wxRED_BRUSH);
        }
        
        dc.SetPen(*wxBLACK_PEN);
        int scaledRadius = POINT_RADIUS * zoomFactor;
        if (scaledRadius < 3) scaledRadius = 3; // Minimum radius
        dc.DrawCircle(screenPos, scaledRadius);
        
        // Draw point number
        wxString pointLabel = wxString::Format("%zu", i+1);
        wxSize textSize = dc.GetTextExtent(pointLabel);
        dc.DrawText(pointLabel, wxPoint(
            screenPos.x - textSize.GetWidth()/2, 
            screenPos.y - textSize.GetHeight()/2)
        );
    }
}

// Recursive De Casteljau's algorithm to calculate points on a Bezier curve
wxPoint DeCasteljau(const std::vector<wxPoint>& points, double t) {
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

void BezierCurvePanel::DrawBezierCurve(wxDC& dc) {
    if (controlPoints.size() < 2) {
        return;
    }
    
    // Draw the Bezier curve using De Casteljau's algorithm
    dc.SetPen(wxPen(*wxGREEN, std::max(2, int(2 * zoomFactor))));
    
    std::vector<wxPoint> curvePoints;
    
    // Calculate 100 points along the curve
    const int STEPS = 100;
    for (int i = 0; i <= STEPS; ++i) {
        double t = static_cast<double>(i) / STEPS;
        curvePoints.push_back(WorldToScreen(DeCasteljau(controlPoints, t)));
    }
    
    // Draw lines connecting the calculated points
    for (size_t i = 0; i < curvePoints.size() - 1; ++i) {
        dc.DrawLine(curvePoints[i], curvePoints[i + 1]);
    }
} 