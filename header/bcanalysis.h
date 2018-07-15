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
 *   Header file for BCAnalysis Class                                        *
 *   Class for performing analysis and plotting the before closure analysis  *
 *   data.                                                                   *
 ****************************************************************************/


/****************************************************************************/

#ifndef BCANALYSIS_H
#define BCANALYSIS_H

#include <QVector>
#include <QObject>
#include "dfitanalysis.h"
#include "qcustomplot/qcustomplot.h"


// ------------------------------------------------------------------------ //
class BCAnalysis: public QObject
{
    Q_OBJECT


    // -------------------------------------------------------------------- //
    /// Properties
public:
    unsigned long dwindow;      // Derivative window
    DFITAnalysis* dfitanalysis; // Main window data object

    // Titles
    struct FigTitles
    {
        QString plot;
        QString x;
        QString y1;
        QString y2;
        QString y3;
    };

    FigTitles figtitles;

    // Colors
    Qt::GlobalColor yColor;
    Qt::GlobalColor xdxColor;
    Qt::GlobalColor dxColor;

    // Plotting vectors
    QVector<double> x;
    QVector<double> dx;
    QVector<double> xdx;

    // Figures and axes handles
    QCustomPlot* fig;
    QCPAxis* xaxis;
    QCPAxis* yaxis1;
    QCPAxis* yaxis2;
    QCPAxis* yaxis3;

    // Data cursor related plots and data readings
    struct DataCursor
    {
        unsigned long pos;

        QCPGraph *pressPlot = NULL;
        QCPGraph *logDerPlot = NULL;
        QCPGraph *derPlot = NULL;
        QCPGraph *vericalLine = NULL;

        QCPItemText *presLabel = NULL;
        QCPItemText *logDerLabel = NULL;
        QCPItemText *derLabel = NULL;
    };
    DataCursor dtcrsr;

    // Straight line through origin and vertical line for closure handles
    QCPGraph *stLnPlot = NULL;
    QCPGraph *clsrPtPlot = NULL;

    // Pressure and derivative plot handles
    QCPGraph *pressPlot;
    QCPGraph *logDerPlot;
    QCPGraph *derPlot;

    // Closure pressure and time
    double pClosure = 1.0;
    double tClosure = 1.0;


    // -------------------------------------------------------------------- //
    /// Methods
public:
    // Constructor
    BCAnalysis(QCustomPlot *fig, DFITAnalysis *dfitanalysis);

    // Calculates smooth derivatives over the window
    void calculateDerivatives();

    // Sets up a blank 3 yaxis figure
    void threeAxesFigure();

    // Adds the data to the figure
    void plotData();

    // Adds titles to the plot and axes
    void annotatePlot();

    // Analyzes and plot, puts everything together
    void analysisPlot();

    // SRT Function
    void SRTFunction();

    // GFunction
    void GFunction();

public slots:
    // Slot: Creates data cursor and readings on the plot.
    void dataCursorPlot(QMouseEvent *event);

};


#endif // BCANALYSIS_H


/****************************************************************************/
