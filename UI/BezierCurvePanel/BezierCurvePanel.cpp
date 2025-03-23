#include "BezierCurvePanel.h"
#include <wx/dcbuffer.h>

/**
 * initializer for the BezierCurvePanel
 * @param parent The parent window
 * @param name The name of the panel
 */
BezierCurvePanel::BezierCurvePanel(
    wxWindow* parent, 
    const wxString& name
) : wxPanel(parent, wxID_ANY) {
    SetBackgroundStyle(wxBG_STYLE_PAINT);
    
    SetBackgroundColour(*wxWHITE);
    
    CreateControls();
    
    initializeDefaultPoints();
    
    nameLabel->SetLabelText(name);
    
    Bind(wxEVT_LEFT_DOWN, &BezierCurvePanel::onMouseDown, this);
    Bind(wxEVT_LEFT_UP, &BezierCurvePanel::onMouseUp, this);
    Bind(wxEVT_MOTION, &BezierCurvePanel::onMouseMove, this);
    Bind(wxEVT_MOUSEWHEEL, &BezierCurvePanel::onMouseWheel, this);
    Bind(wxEVT_PAINT, &BezierCurvePanel::onPaint, this);
    Bind(wxEVT_KEY_DOWN, &BezierCurvePanel::onKeyDown, this);
    
    SetFocus();
}

/**
 * creates the controls for the panel
 */
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
    
    zoomSlider->Bind(wxEVT_SLIDER, &BezierCurvePanel::onZoomSlider, this);
}

/**
 * configures the default control points for a cubic Bezier curve
 */
void BezierCurvePanel::initializeDefaultPoints() {
    controlPoints.clear();
    
    zoomFactor = 1.0;
    panOffset = wxPoint(0, 0);
    if (zoomSlider) {
        zoomSlider->SetValue(100);
        zoomLabel->SetLabelText("Zoom: 100%");
    }
    
    wxSize size = GetClientSize();
    int width = size.GetWidth();
    int height = size.GetHeight();

    // The control points appear clustered when first initialized because they're positioned using relative proportions of the panel's client size.
    // This is a hack to ensure the control points are visible.
    if (width < 100 || height < 100) {
        width = 600;
        height = 400;
    }
    
    controlPoints.push_back(wxPoint(width * 0.1, height * 0.5));
    controlPoints.push_back(wxPoint(width * 0.3, height * 0.2));
    controlPoints.push_back(wxPoint(width * 0.7, height * 0.8));
    controlPoints.push_back(wxPoint(width * 0.9, height * 0.5));
    
    Refresh();
}

/**
 * converts a point from regular coordinates to onscreen coordinates
 * @param point The point to convert
 * @return The point in onscreen coordinates
 */
wxPoint BezierCurvePanel::worldToScreen(const wxPoint& point) {
    int x = (point.x * zoomFactor) + panOffset.x;
    int y = (point.y * zoomFactor) + panOffset.y;
    return wxPoint(x, y);
}

/**
 * converts a point from onscreen coordinates to regular coordinates
 * @param point The point to convert
 * @return The point in regular coordinates
 */
wxPoint BezierCurvePanel::screenToWorld(const wxPoint& point) {
    int x = (point.x - panOffset.x) / zoomFactor;
    int y = (point.y - panOffset.y) / zoomFactor;
    return wxPoint(x, y);
}

void BezierCurvePanel::setZoom(double zoom) {
    // Center of the view in world coordinates before zoom
    wxSize size = GetClientSize();
    wxPoint center = screenToWorld(wxPoint(size.GetWidth() / 2, size.GetHeight() / 2));
    
    // Set the new zoom factor
    zoomFactor = zoom;
    
    // Update the zoom label
    int zoomPercent = static_cast<int>(zoomFactor * 100);
    zoomLabel->SetLabelText(wxString::Format("Zoom: %d%%", zoomPercent));
    zoomSlider->SetValue(zoomPercent);
    
    // Calculate new panOffset to keep the center of the view fixed
    wxPoint newCenter = worldToScreen(center);
    panOffset.x += (size.GetWidth() / 2) - newCenter.x;
    panOffset.y += (size.GetHeight() / 2) - newCenter.y;
    
    Refresh();
}

/**
 * checks if the user has clicked a control point
 * @param pos The position of the mouse
 * @return The index of the control point that was clicked, or -1 if no control point was clicked
 */
int BezierCurvePanel::checkForControlPointClick(const wxPoint& pos) {
    for (size_t i = 0; i < controlPoints.size(); ++i) {
        wxPoint screenPos = worldToScreen(controlPoints[i]);
        wxPoint diff = pos - screenPos;
        int distanceSquared = diff.x * diff.x + diff.y * diff.y;
        
        int scaledRadius = POINT_RADIUS * zoomFactor;
        
        if (distanceSquared <= scaledRadius * scaledRadius) {
            return i;
        }
    }
    
    // -1 implies no control point was clicked
    return -1; 
}

/**
 * if the user clicks the mouse, this function is called.
 * if the user clicks a control point, the control point is selected.
 * if the user clicks the middle button or the control key and the left button, the view is panned.
 * if the user clicks the left button without the control key, a new control point is added.
 * @param event The mouse event
 */
void BezierCurvePanel::onMouseDown(wxMouseEvent& event) {
    SetFocus();
    
    selectedPointIndex = checkForControlPointClick(event.GetPosition());
    
    if (selectedPointIndex == -1) {
        if (event.MiddleDown() || (event.LeftDown() && event.ControlDown())) {
            isPanning = true;
            dragStart = event.GetPosition();
            SetCursor(wxCursor(wxCURSOR_HAND));
        }
        else if (event.LeftDown() && !event.ControlDown()) {
            if (controlPoints.size() < MAX_CONTROL_POINTS) {
                controlPoints.push_back(screenToWorld(event.GetPosition()));
                selectedPointIndex = controlPoints.size() - 1;
            } else {
                isPanning = true;
                dragStart = event.GetPosition();
                SetCursor(wxCursor(wxCURSOR_HAND));
            }
        }
    }
    
    Refresh();
    event.Skip();
}

/**
 * if the user releases the mouse button, this function is called.
 * if the user is panning the view, the view is stopped.
 * @param event The mouse event
 */
void BezierCurvePanel::onMouseUp(wxMouseEvent& event) {
    if (isPanning) {
        isPanning = false;
        SetCursor(wxCursor(wxCURSOR_ARROW));
    }
    
    selectedPointIndex = -1;
    event.Skip();
}

/**
 * if the user is dragging the mouse, this function is called invoked by the system
 * if the user is dragging a control point, the control point is moved.
 * if the user is panning the view, the view is panned.
 * @param event The mouse event
 */
void BezierCurvePanel::onMouseMove(wxMouseEvent& event) {
    if (selectedPointIndex != -1 && event.Dragging() && event.LeftIsDown()) {
        controlPoints[selectedPointIndex] = screenToWorld(event.GetPosition());
        Refresh();
    }
    else if (isPanning && event.Dragging()) {
        wxPoint currentPos = event.GetPosition();
        wxPoint delta = currentPos - dragStart;
        
        panOffset.x += delta.x;
        panOffset.y += delta.y;
        
        dragStart = currentPos;
        Refresh();
    }
    event.Skip();
}

/**
 * uses the mouse wheel to pan the view.
 * use shift to pan horizontally.
 * use regular mouse wheel to pan vertically.
 * @param event The mouse wheel event
 */
void BezierCurvePanel::onMouseWheel(wxMouseEvent& event) {
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

/**
 * if the user presses a key, this function is called.
 * observes the modifier key and the key code to determine the action.
 * typically used for zooming in and out.
 * 
 * @param event The key event
 */
void BezierCurvePanel::onKeyDown(wxKeyEvent& event) {
    if (event.GetModifiers() == wxMOD_CMD) {
        if (event.GetKeyCode() == '=' || event.GetKeyCode() == WXK_NUMPAD_ADD) {
            double newZoom = zoomFactor + 0.1;
            if (newZoom > 5.0) newZoom = 5.0; 
            setZoom(newZoom);
        }
        else if (event.GetKeyCode() == '-' || event.GetKeyCode() == WXK_NUMPAD_SUBTRACT) {
            double newZoom = zoomFactor - 0.1;
            if (newZoom < 0.1) newZoom = 0.1;
            setZoom(newZoom);
        }
        else if (event.GetKeyCode() == '0' || event.GetKeyCode() == WXK_NUMPAD0) {
            zoomFactor = 1.0;
            panOffset = wxPoint(0, 0);
            setZoom(1.0);
        }
    }
    
    event.Skip();
}

/**
 * This is automatically called when the zoom slider is changed
 * @param event The command event
 */
void BezierCurvePanel::onZoomSlider(wxCommandEvent& event) {
    double newZoom = zoomSlider->GetValue() / 100.0;
    setZoom(newZoom);
}

/**
 * This is automatically called when the panel needs to be redrawn
 * @param event The paint event
 */
void BezierCurvePanel::onPaint(wxPaintEvent& event) {
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
        
        for (int y = startY; y < size.GetHeight(); y += gridSpacing) {
            dc.DrawLine(0, y, size.GetWidth(), y);
        }
    }
    
    dc.SetPen(*wxBLACK_PEN);
    
    if (controlPoints.size() > 1) {
        dc.SetPen(wxPen(*wxBLUE, 1, wxPENSTYLE_DOT));
        for (size_t i = 0; i < controlPoints.size() - 1; ++i) {
            wxPoint p1 = worldToScreen(controlPoints[i]);
            wxPoint p2 = worldToScreen(controlPoints[i + 1]);
            dc.DrawLine(p1, p2);
        }
    }
    
    if (controlPoints.size() == MAX_CONTROL_POINTS) {
        drawBezierCurve(dc);
    }
    
    for (size_t i = 0; i < controlPoints.size(); ++i) {
        wxPoint screenPos = worldToScreen(controlPoints[i]);
        
        if (static_cast<int>(i) == selectedPointIndex) {
            dc.SetBrush(*wxYELLOW_BRUSH);
        } else {
            dc.SetBrush(*wxRED_BRUSH);
        }
        
        dc.SetPen(*wxBLACK_PEN);
        int scaledRadius = POINT_RADIUS * zoomFactor;
        if (scaledRadius < 3) scaledRadius = 3; // Minimum radius
        dc.DrawCircle(screenPos, scaledRadius);
        
        wxString pointLabel = wxString::Format("%zu", i+1);
        wxSize textSize = dc.GetTextExtent(pointLabel);
        dc.DrawText(pointLabel, wxPoint(
            screenPos.x - textSize.GetWidth()/2, 
            screenPos.y - textSize.GetHeight()/2)
        );
    }
}

/**
 * Calculates a point on the Bezier curve using De Casteljau's algorithm
 * @param points The control points of the Bezier curve
 * @param t The parameter value (0 <= t <= 1)
 * @return The point on the Bezier curve
 */
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

/**
 * Draws the Bezier curve using De Casteljau's algorithm
 * @param dc The device context to draw on
 */
void BezierCurvePanel::drawBezierCurve(wxDC& dc) {
    if (controlPoints.size() < 2) {
        return;
    }
    
    dc.SetPen(wxPen(*wxGREEN, std::max(2, int(2 * zoomFactor))));
    
    std::vector<wxPoint> curvePoints;
    
    const int STEPS = 100;
    for (int i = 0; i <= STEPS; ++i) {
        double t = static_cast<double>(i) / STEPS;
        curvePoints.push_back(worldToScreen(DeCasteljau(controlPoints, t)));
    }
    
    for (size_t i = 0; i < curvePoints.size() - 1; ++i) {
        dc.DrawLine(curvePoints[i], curvePoints[i + 1]);
    }
} 