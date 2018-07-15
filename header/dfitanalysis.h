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
 *   Header file for DFITAnalysis Class                                      *
 *   Class for creating the main window job plot and adding cursor.          *
 ****************************************************************************/


/****************************************************************************/

#ifndef DFITANALYSIS_H
#define DFITANALYSIS_H

#include <iostream>
#include <QVector>
#include <QObject>
#include "qcustomplot/qcustomplot.h"
#include "FSHelper/fshelper.h"

using namespace std;


// ------------------------------------------------------------------------ //
class DFITAnalysis: public QObject
{
Q_OBJECT


    // -------------------------------------------------------------------- //
    /// Properties
public:
    QString filename;
    double tp;

    // Colors
    Qt::GlobalColor presTimeColor;
    Qt::GlobalColor rateTimeColor;

    QVector<double> t;
    QVector<double> p;
    QVector<double> r;

    QVector<double> tD;
    QVector<double> p_shut;

    QCustomPlot* fig;
    QCPAxis* xaxis = NULL;
    QCPAxis* yaxis1 = NULL;
    QCPAxis* yaxis2 = NULL;


    struct DataCursor
    {
        QCPGraph *presTime = NULL;
        QCPGraph *rateTime = NULL;
        QCPGraph *vericalLine = NULL;

        QCPItemText *presTimeLabel = NULL;
        QCPItemText *rateTimeLabel = NULL;
    };
    DataCursor dtcrsr;


    // -------------------------------------------------------------------- //
    /// Methods
public:
    // Constructor
    DFITAnalysis(QCustomPlot *fig);

    // Plots the pressure, and rate vs time
    void jobPlot();

    // Reset all variables
    void reset(QCustomPlot* fig);

public slots:
    // Slot: Creates data cursor and readings on the plot.
    void dataCursorPlot(QMouseEvent *event);

};


#endif // DFITANALYSIS_H


/****************************************************************************/
