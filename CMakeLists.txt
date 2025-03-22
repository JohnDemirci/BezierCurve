cmake_minimum_required(VERSION 3.15)
project(BezierCurve)

find_package(wxWidgets REQUIRED COMPONENTS core base)

include(${wxWidgets_USE_FILE})

add_executable(BezierCurve 
    main.cpp
    UI/MainView/MainView.cpp
    UI/BezierCurvePanel/BezierCurvePanel.cpp
)

target_link_libraries(BezierCurve ${wxWidgets_LIBRARIES})
