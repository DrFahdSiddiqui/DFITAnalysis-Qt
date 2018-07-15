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
 *   Header file for AnalysisWindow Class                                    *
 *   Class for creating the secondary window of the application.             *
 ****************************************************************************/


/****************************************************************************/

#ifndef ANALYSISWINDOW_H
#define ANALYSISWINDOW_H

#include <QMainWindow>
#include "bcanalysis.h"

namespace Ui {
class AnalysisWindow;
}


// ------------------------------------------------------------------------ //
class AnalysisWindow : public QMainWindow
{
    Q_OBJECT


    // -------------------------------------------------------------------- //
    /// Properties
public:
    BCAnalysis* bcanalysis;
    QCPItemText *closureLabel = NULL;

private:
    Ui::AnalysisWindow *ui;


    // -------------------------------------------------------------------- //
    /// Methods
public:
    // Constructor
    AnalysisWindow(DFITAnalysis* dfitanalysis,
                   int selector, QWidget *parent=0);

    // Destructor
    ~AnalysisWindow();

private:
    // Saves figure as jpg
    bool saveFig(const QString &fileName);

private slots:
    // Gets new smoothing window and replots
    void on_dSmoothButton_clicked();

    // Connects the mouse to straight line drawing
    void on_stLineButton_clicked();

    // Connects the mouse to vertical line drawing
    void on_vLineButton_clicked();

    // Draws the straight line thru origin on click
    void stLinePlot(QMouseEvent *event);

    // Draws the vertical line for closure
    void vLinePlot(QMouseEvent *event);

    // Set xmin range
    void on_xminButton_clicked();

    // Set y1min range
    void on_y1minButton_clicked();

    // Set y2min range
    void on_y2minButton_clicked();

    // Set y3min range
    void on_y3minButton_clicked();

    // Set xmax range
    void on_xmaxButton_clicked();

    // Set y1max range
    void on_y1maxButton_clicked();

    // Set y2max range
    void on_y2maxButton_clicked();

    // Set y3max range
    void on_y3maxButton_clicked();

    // Save the figure as a jpg button
    void on_saveJpgButton_clicked();

};


#endif // ANALYSISWINDOW_H


/****************************************************************************/
