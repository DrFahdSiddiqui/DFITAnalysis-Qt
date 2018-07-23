/*****************************************************************************
 *                                                                           *
 *           DIAGNOSTIC FRACTURE INJECTION TESTS ANALYSIS PROGRAM            *
 *                               Version 1.0                                 *
 *            Written for C++ by : Fahd Siddiqui and Aqsa Qureshi            *
 *             https://github.com/DrFahdSiddiqui/DFITAnalysis-QT             *
 *                                                                           *
 * ------------------------------------------------------------------------- *
 * LICENSE: MOZILLA 2.0                                                      *
 *   This Source Code Form is subject to the terms of the Mozilla Public     *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this     *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.                *
 ****************************************************************************/

/*****************************************************************************
 * DOCUMENTATION                                                             *
 *   File containing somewhat larger strings.                                *
 ****************************************************************************/


/****************************************************************************/

#ifndef PROGRAMSTRINGS_H
#define PROGRAMSTRINGS_H

static QString headerString = R"header(
*****************************************************************************
*                                                                           *
*           DIAGNOSTIC FRACTURE INJECTION TESTS ANALYSIS PROGRAM            *
*                               Version 1.0                                 *
*            Written for C++ by : Fahd Siddiqui and Aqsa Qureshi            *
*             https://github.com/DrFahdSiddiqui/DFITAnalysis-QT             *
*                                                                           *
* ------------------------------------------------------------------------- *
* LICENSE: MOZILLA 2.0                                                      *
*   This Source Code Form is subject to the terms of the Mozilla Public     *
*   License, v. 2.0. If a copy of the MPL was not distributed with this     *
*   file, You can obtain one at http://mozilla.org/MPL/2.0/.                *
*****************************************************************************
)header";


static QString saveDocString = R"saveDoc(
*****************************************************************************
* DOCUMENTATION                                                             *
*   Save file for the program                                               *
*   Do not manually edit this file                                          *
*****************************************************************************


*****************************************************************************
)saveDoc";


static QString exportCsvDocString = R"csvDoc(
*****************************************************************************
* DOCUMENTATION                                                             *
*   Exported csv file containing before closure anlayses                    *
*   Results, and tabulated time, pressure and derivatives                   *
*****************************************************************************


*****************************************************************************

)csvDoc";

static QString versionString = R"ver(PS DFIT Analysis v1.0)ver";

static const char* aboutString = R"about(
 <h1>DFIT Analysis Version 1</h1>
 <p>This program was written by Fahd Siddiqui for performing quick analyses of hydraulic fracturing injection pressure data.&nbsp;</p>
 <p>Fahd has a PhD in Petroleum Enigneering and is interested in solving engineering problems. Please visit <a href="http://www.FahdSiddiqui.com">www.FahdSiddiqui.com</a>&nbsp;and give your feedback.</p>
                             )about";

static const char* helpString = R"help(
<h1>DFIT Analysis Version 1</h1>
<p>This program helps with the analysis of Diagnostic Fracture Injection Tests (DFIT). It employes the standard G-Function and Square root analysis techniques. It has been written in C++ to be fast and optimized. It has been tested with 1+ Million points. Loading large number of points is as fast as MS Excel, while plotting is and analyses are much faster.</p>
<h2>Workflow</h2>
<ol>
<li>Load the data from csv file or open existing project.&nbsp;</li>
<li>From the plot/table identify the time at which injection stops.</li>
<li>Input the injection time using the "Enter Injection Time" button.</li>
<li>Push "G-Function" and/or "Sqrt Time" buttons to open the plots.&nbsp;Multiple analyses may be performed.&nbsp;</li>
<li>Figures may be rescaled by buttons at the bottom.</li>
<li>Derivatives smoothing may also be performed by selecting the number of points used for computing derivatives by hitting the button "Derivative Smoothing".</li>
<li>On the plots, use the buttons to draw a straing line through origin.</li>
<li>Based on the departure of data from the straight line, select the closure using the button.</li>
<li>Closure time and pressure will be displayed on the figure.</li>
</ol>
<p>The figures are interactive and display the plot data cursor on mouse over. Figures may also be exported as csv or as jpg files.&nbsp;</p>
<h2>Saving/Loading</h2>
<p>The session is saved by saving all the open windows and data, including the closure analyses to .dft file.&nbsp;Session may be restored alongwith all open windows and analyses results upon loading from the .dft file.</p>
<h2>Open Source</h2>
<p>This program is released as open source under Mozilla Public License (MPL) v2. The complete source code may be obtained from&nbsp;<a href="https://github.com/DrFahdSiddiqui/DFITAnalysis-Qt">https://github.com/DrFahdSiddiqui/DFITAnalysis-Qt</a>.&nbsp;</p>
<p>It uses following:</p>
<ol>
<li>Qt 5.11&nbsp;https://www.qt.io/ (​GNU GPL v3)</li>
<li>qcustomplot library&nbsp;<a href="https://www.qcustomplot.com/">https://www.qcustomplot.com/</a> (​GNU GPL v3)</li>
<li>Eigen library&nbsp;<a href="http://eigen.tuxfamily.org">http://eigen.tuxfamily.org</a>&nbsp;(MPL v2)</li>
</ol>
<h2>Contact</h2>
<p>Please visit <a href="http://www.FahdSiddiqui.com">www.FahdSiddiqui.com</a>.</p>
)help";


#endif //PROGRAMSTRINGS_H


/****************************************************************************/
