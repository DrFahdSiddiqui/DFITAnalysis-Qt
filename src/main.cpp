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
 *   Source file for main C++ starting function                              *
 *   Code initializes the mainwindow and starts the program.                 *
 ****************************************************************************/


/****************************************************************************/

#include "mainwindow.h"
#include <QApplication>


// ------------------------------------------------------------------------ //
// Main C++ function
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}


/****************************************************************************/
