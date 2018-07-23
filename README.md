# DFIT Analysis Version 1
This program helps with the analysis of Diagnostic Fracture Injection Tests (DFIT). It employes the standard G-Function and Square root analysis techniques. It has been written in C++ to be fast and optimized. It has been tested with 1+ Million points. Loading large number of points is as fast as MS Excel, while plotting is and analyses are much faster.

## How to Use

### Download
For Windows 7 to 10, download all files located in release/win32. 
Run PSDFITAnlaysis.exe.

### Workflow
Load the data from csv file or open existing project. 
From the plot/table identify the time at which injection stops.
Input the injection time using the "Enter Injection Time" button.
Push "G-Function" and/or "Sqrt Time" buttons to open the plots. Multiple analyses may be performed. 
Figures may be rescaled by buttons at the bottom.
Derivatives smoothing may also be performed by selecting the number of points used for computing derivatives by hitting the button "Derivative Smoothing".
On the plots, use the buttons to draw a straing line through origin.
Based on the departure of data from the straight line, select the closure using the button.
Closure time and pressure will be displayed on the figure.
The figures are interactive and display the plot data cursor on mouse over. Figures may also be exported as csv or as jpg files. 

### Saving/Loading
The session is saved by saving all the open windows and data, including the closure analyses to .dft file. Session may be restored alongwith all open windows and analyses results upon loading from the .dft file.

## Contributing
Git clone or download all files. Open the DFITAnalysis.pro file with Qt-Creator. Works and tested with 
1. Clang on Xubuntu (Qt-Creator)
2. G++ on Xubuntu (Qt-Creator)
3. MSVC on Windows 7 (Qt-Creator)

### Open Source
This program is released as open source under Mozilla Public License (MPL) v2. The complete source code may be obtained from

It uses following:
1. Qt 5.11 https://www.qt.io/ (GNU GPL v3)
2. qcustomplot library https://www.qcustomplot.com/ (GNU GPL v3)
3. Eigen library http://eigen.tuxfamily.org (MPL v2)

## Contact
Please visit www.FahdSiddiqui.com.

