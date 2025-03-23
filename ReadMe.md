
# Prerequisites #

1. You have a c++ compiler installed.
2. You have CMake 3.15+ installed.
3. Make sure wxWidgets is installed and it is in your PATH.

# Features #

1. You can adjust the zoom factor of the screen through the slider
2. You may add new tabs of different bezier curves through file -> add tab
3. You may adjust the screen real estate through dragging the muse
4. You may adjust screen real estate through mouse wheel (for horizontal use shift + mouse wheel)

# How to build #

1. Create a build folder 

$ mkdir cmake-build-debug

2. navigate to the build folder and build

$ cd cmake-build-debug
$ cmake ..
$ make

3. Run the application
$ ./BezierCurve