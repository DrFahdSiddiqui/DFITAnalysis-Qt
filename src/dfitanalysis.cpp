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
 *   Source file for DFITAnalysis Class                                      *
 *   Class for creating the main window job plot and adding cursor.          *
 ****************************************************************************/

/*****************************************************************************
 * TODO                                                                      *                                         *
 *   Code reusability for plots, and data cursor                             *
 ****************************************************************************/


/****************************************************************************/

#include "dfitanalysis.h"

// ------------------------------------------------------------------------ //
// Constructor
DFITAnalysis::DFITAnalysis(QCustomPlot* fig)
{
    this->fig = fig;
}


// ------------------------------------------------------------------------ //
// Plots the pressure, and rate vs time
void DFITAnalysis::jobPlot()
{
    fig->plotLayout()->clear();
    fig->setCurrentLayer("main");

    // Colors
    presTimeColor = Qt::blue;
    rateTimeColor  = Qt::red;

    QCPAxisRect *rectAxes = new QCPAxisRect(fig);
    rectAxes->setupFullAxesBox(false);
    // rectAxes->addAxis(QCPAxis::atRight); add third yaxis

    xaxis  = rectAxes->axis(QCPAxis::atBottom, 0);
    yaxis1 = rectAxes->axis(QCPAxis::atLeft, 0);
    yaxis2 = rectAxes->axis(QCPAxis::atRight, 0);

    // Plot title:
    fig->plotLayout()->addElement(0, 0,
                                  new QCPTextElement(fig,
                                                     "Job Plot",
                                                     QFont("sans",
                                                           12,
                                                           QFont::Bold)));

    // Axes labels
    xaxis->setLabel("Time [secs]");
    yaxis1->setLabel("Pressure [psi]");
    yaxis2->setLabel("Rate [bpm]");

    /*
    // synchronize the left and right margins of the top and bottom axis rects:
    //    QCPMarginGroup *marginGroup = new QCPMarginGroup(fig);
    //    rectAxes->setMarginGroup(QCP::msLeft | QCP::msRight, marginGroup);
    // move newly created axes on "axes" layer and grids on "grid" layer:
    //    foreach (QCPAxisRect *rect, fig->axisRects())
    //    {
    //      foreach (QCPAxis *axis, rect->axes())
    //      {
    //        axis->setLayer("axes");
    //        axis->grid()->setLayer("grid");
    //      }
    //    }
    */

    // Create and configure plots:
    QCPGraph *pressTime = fig->addGraph(xaxis, yaxis1);
    pressTime->setData(t, p);
    pressTime->setPen(QPen(presTimeColor, 2));

    QCPGraph *rateTime = fig->addGraph(xaxis, yaxis2);
    rateTime->setData(t, r);
    rateTime->setPen(QPen(rateTimeColor, 2));

    // Set axes ranges:
    xaxis->rescale();
    xaxis->setRangeLower(0);
    yaxis1->rescale();
    yaxis2->rescale();

    // Set axes colors, ticks etc
    yaxis1->setTickLabels(true);
    yaxis1->setTickLabelColor(presTimeColor);
    yaxis1->setLabelColor(presTimeColor);
    yaxis1->setBasePen(QPen(presTimeColor));

    yaxis2->setTickLabels(true);
    yaxis2->setTickLabelColor(rateTimeColor);
    yaxis2->setLabelColor(rateTimeColor);
    yaxis2->setBasePen(QPen(rateTimeColor));

    // Insert plot rect in second row (because of title)
    fig->plotLayout()->addElement(1, 0, rectAxes);

    // Add annotation layer (because it will be updated)
    fig-> QCustomPlot::addLayer("annotations");
    fig->layer("annotations")->setMode(QCPLayer::lmBuffered);
    fig->replot();

    QObject::connect(fig, SIGNAL(mouseMove(QMouseEvent*)), this, SLOT(dataCursorPlot(QMouseEvent*)));
    // QObject::connect(fig, SIGNAL(mouseDoubleClick(QMouseEvent*)), this, SLOT(showPointToolTip(QMouseEvent*)));
}


// ------------------------------------------------------------------------ //
// Reset all variables
void DFITAnalysis::reset(QCustomPlot* fig)
{
    this->fig=fig;
    fig->plotLayout()->clear();
    fig->replot();

    filename    = "";
    tp          = 0.0;

    t.clear(); t.squeeze();
    p.clear(); p.squeeze();
    r.clear(); r.squeeze();

    tD.clear(); tD.squeeze();
    p_shut.clear(); p_shut.squeeze();

    if(dtcrsr.presTime != NULL)
        disconnect(fig, 0, 0, 0);

    fig     = NULL;
    xaxis   = NULL;
    yaxis1  = NULL;
    yaxis2  = NULL;

    dtcrsr.presTime     = NULL;
    dtcrsr.rateTime     = NULL;
    dtcrsr.vericalLine  = NULL;

    dtcrsr.presTimeLabel = NULL;
    dtcrsr.rateTimeLabel = NULL;
}


// ------------------------------------------------------------------------ //
// Slot: Creates data cursor and readings on the plot.
void DFITAnalysis::dataCursorPlot(QMouseEvent *event)
{
    double xcoord = xaxis->pixelToCoord(event->pos().x());
    unsigned long pos = FSHelper::find(t, xcoord);

    QVector<double> t_crsr;
    QVector<double> p_crsr;
    QVector<double> r_crsr;
    QVector<double> v_crsr;

    t_crsr.push_back(t[pos]);
    t_crsr.push_back(t[pos]);
    p_crsr.push_back(p[pos]);
    r_crsr.push_back(r[pos]);
    v_crsr.push_back(yaxis1->range().lower);
    v_crsr.push_back(yaxis1->range().upper);

    // To update just the annotations layer
    fig->setCurrentLayer("annotations");

    // Show gray vertical line
    if(dtcrsr.vericalLine == NULL){
        dtcrsr.vericalLine = fig->addGraph(xaxis, yaxis1);
        // fig->replot();
    }
    dtcrsr.vericalLine->setData( t_crsr, v_crsr );
    dtcrsr.vericalLine->setPen(QPen(Qt::gray, 1));
    dtcrsr.vericalLine->setPen(QPen(Qt::DashLine));

    // Show pressure data cursor
    if(dtcrsr.presTime == NULL){
        dtcrsr.presTime = fig->addGraph(xaxis, yaxis1);
        // fig->replot();
    }
    dtcrsr.presTime->setData( t_crsr, p_crsr );
    dtcrsr.presTime->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle,
                                                     QPen(Qt::black),
                                                     QBrush(presTimeColor),
                                                     10));

    // Show rate data cursor
    if(dtcrsr.rateTime == NULL){
        dtcrsr.rateTime = fig->addGraph(xaxis, yaxis2);
        // fig->replot();
    }
    dtcrsr.rateTime->setData( t_crsr, r_crsr );
    dtcrsr.rateTime->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle,
                                                     QPen(Qt::black),
                                                     QBrush(rateTimeColor),
                                                     10));


    // Show pressure data reading
    if(dtcrsr.presTimeLabel == NULL){
        dtcrsr.presTimeLabel = new QCPItemText(fig);
        dtcrsr.presTimeLabel->setPositionAlignment(Qt::AlignBottom|Qt::AlignLeft);
        dtcrsr.presTimeLabel->position->setType(QCPItemPosition::ptPlotCoords);
        dtcrsr.presTimeLabel->position->setAxes(xaxis, yaxis1);
        dtcrsr.presTimeLabel->setFont(QFont("sans", 8));
        dtcrsr.presTimeLabel->setColor(presTimeColor);
        dtcrsr.presTimeLabel->setPadding(QMargins(10, 0, 0, 10));
        dtcrsr.presTimeLabel->setClipToAxisRect(false);
        // annotations.pressTimeLabel->setPen(QPen(pressTimeColor));

        // fig->replot();
    }
    dtcrsr.presTimeLabel->position->setCoords(t_crsr[0], p_crsr[0]);
    dtcrsr.presTimeLabel->setText(QString("%1 , %2").arg(t_crsr[0]).arg(p_crsr[0]));

    // Show rate data reading
    if(dtcrsr.rateTimeLabel == NULL){
        dtcrsr.rateTimeLabel = new QCPItemText(fig);
        dtcrsr.rateTimeLabel->setPositionAlignment(Qt::AlignBottom|Qt::AlignLeft);
        dtcrsr.rateTimeLabel->position->setType(QCPItemPosition::ptPlotCoords);
        dtcrsr.rateTimeLabel->position->setAxes(xaxis, yaxis2);
        dtcrsr.rateTimeLabel->setFont(QFont("sans", 8));
        dtcrsr.rateTimeLabel->setColor(rateTimeColor);
        dtcrsr.rateTimeLabel->setPadding(QMargins(10, 0, 0, 10));
        dtcrsr.rateTimeLabel->setClipToAxisRect(false);

        fig->replot();
    }
    dtcrsr.rateTimeLabel->position->setCoords(t_crsr[0], r_crsr[0]);
    dtcrsr.rateTimeLabel->setText(QString("%1 , %2").arg(t_crsr[0]).arg(r_crsr[0]));

    // Update just the annotation layer to improve performance
    fig->layer("annotations")-> QCPLayer::replot();
}


/****************************************************************************/
