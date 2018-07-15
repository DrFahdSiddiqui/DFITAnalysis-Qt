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
 *   Source file for BCAnalysis Class                                        *
 *   Class for performing analysis and plotting the                          *
 *   Before Closure Analysis data.                                           *
 ****************************************************************************/


/****************************************************************************/

#include "bcanalysis.h"


// ------------------------------------------------------------------------ //
// Constructor
BCAnalysis::BCAnalysis(QCustomPlot *fig, DFITAnalysis* dfitanalysis)
{
    // Hardcodes a default dwindow to 15
    dwindow = 15;
    this->dfitanalysis = dfitanalysis;
    this->fig = fig;
}


// ------------------------------------------------------------------------ //
// Calculates smooth derivatives over the window
void BCAnalysis::calculateDerivatives()
{
    dx.clear(); dx.squeeze();
    xdx.clear(); xdx.squeeze();

    // Without Eigen library
    /*
    vector<double> xvarV   =  xvar.toStdVector();
    vector<double> p_shutV =  dfitanalysis->p_shut.toStdVector();
    vector<double> dxV     =  dx.toStdVector();
    FSHelper::smoothDerivative(xvarV, p_shutV, dwindow, dxV);
    dx = QVector<double>::fromStdVector(dxV);
    */

    // With Eigen library
    FSHelper::smoothDerivative(x, dfitanalysis->p_shut, dwindow, dx);

    for (unsigned long i = 0; i<dfitanalysis->tD.size();++i){
        xdx.push_back (x[i]*dx[i]);
    }
}


// ------------------------------------------------------------------------ //
// Sets up a blank 3 yaxis figure
void BCAnalysis::threeAxesFigure()
{
    this->fig = fig;
    fig->plotLayout()->clear();
    fig->setCurrentLayer("main");

    // Colors
    yColor = Qt::blue;
    xdxColor = Qt::red;
    dxColor = Qt::darkGreen;

    QCPAxisRect *rectAxes = new QCPAxisRect(fig);
    rectAxes->setupFullAxesBox(false);
    rectAxes->addAxis(QCPAxis::atRight); // add third yaxis

    xaxis  = rectAxes->axis(QCPAxis::atBottom, 0);
    yaxis1 = rectAxes->axis(QCPAxis::atLeft, 0);
    yaxis2 = rectAxes->axis(QCPAxis::atRight, 0);
    yaxis3 = rectAxes->axis(QCPAxis::atRight, 1);

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
    pressPlot = fig->addGraph(xaxis, yaxis1);
    pressPlot->setPen(QPen(yColor, 2));

    logDerPlot = fig->addGraph(xaxis, yaxis2);
    logDerPlot->setPen(QPen(xdxColor, 2));

    derPlot = fig->addGraph(xaxis, yaxis3);
    derPlot->setPen(QPen(dxColor, 2));

    // Set axes colors, ticks etc
    yaxis1->setTickLabels(true);
    yaxis1->setTickLabelColor(yColor);
    yaxis1->setLabelColor(yColor);
    yaxis1->setBasePen(QPen(yColor));

    yaxis2->setTickLabels(true);
    yaxis2->setTickLabelColor(xdxColor);
    yaxis2->setLabelColor(xdxColor);
    yaxis2->setBasePen(QPen(xdxColor));

    yaxis3->setTickLabels(true);
    yaxis3->setTickLabelColor(dxColor);
    yaxis3->setLabelColor(dxColor);
    yaxis3->setBasePen(QPen(dxColor));

    // Insert plot rect in second row (because of title)
    fig->plotLayout()->addElement(1, 0, rectAxes);

    // Add Analysis Lines layer (because it will be updated)
    fig-> QCustomPlot::addLayer("analysisLines");
    fig->layer("analysisLines")->setMode(QCPLayer::lmBuffered);

    // Add annotation layer (because it will be updated)
    fig-> QCustomPlot::addLayer("annotations");
    fig->layer("annotations")->setMode(QCPLayer::lmBuffered);
    // fig->replot();

    QObject::connect(fig, SIGNAL(mouseMove(QMouseEvent*)), this , SLOT(dataCursorPlot(QMouseEvent*)));
    // QObject::connect(fig, SIGNAL(mouseMove(QMouseEvent*)), this, SLOT(dataCursorPlot(QMouseEvent*)));
    // QObject::connect(fig, SIGNAL(mouseDoubleClick(QMouseEvent*)), this, SLOT(showPointToolTip(QMouseEvent*)));
}


// ------------------------------------------------------------------------ //
// Adds the data to the figure
void BCAnalysis::plotData()
{
    fig->setCurrentLayer("main");
    pressPlot->setData(x, dfitanalysis->p_shut);
    logDerPlot->setData(x, xdx);
    derPlot->setData(x, dx);

    // Rescale axes
    xaxis->rescale();
    xaxis->setRangeLower(0);
    yaxis1->rescale();
    yaxis2->rescale();
    yaxis3->rescale();
}


// ------------------------------------------------------------------------ //
// Adds titles to the plot and axes
void BCAnalysis::annotatePlot()
{
    // Plot title:
    fig->plotLayout()->addElement(0, 0,
                                  new QCPTextElement(fig, figtitles.plot,
                                                     QFont("sans", 12, QFont::Bold)));
    // Axes labels
    xaxis->setLabel(figtitles.x);
    yaxis1->setLabel(figtitles.y1);
    yaxis2->setLabel(figtitles.y2);
    yaxis3->setLabel(figtitles.y3);
}


// ------------------------------------------------------------------------ //
// Analyzes and plot, puts everything together
void BCAnalysis::analysisPlot()
{
    // Perform analysis calculations
    calculateDerivatives();

    // Plot the variables
    threeAxesFigure(); // Set up a blank figure
    plotData(); // Plot actual data

    // Label chart and axes
    annotatePlot();

    fig->replot();
}


// ------------------------------------------------------------------------ //
// Sets squre root time labels and x-axis ploting variable to sqrt time
void BCAnalysis::SRTFunction()
{
    figtitles.plot  = "Square Root Time";
    figtitles.x     = "tD^1/2";
    figtitles.y1    = "Pressure [psi]";
    figtitles.y2    = "tD^1/2.dp/dtD^1/2 [psi]";
    figtitles.y3    = "dp/dtD^1/2 [psi]";

    // Calculate tD^1/2
    x.clear(); x.squeeze();
    for (unsigned long i = 0; i<dfitanalysis->tD.size();++i){
        x.push_back( sqrt(dfitanalysis->tD[i]));
    }
}


// ------------------------------------------------------------------------ //
// Sets G-function labels and x-axis ploting variable to G
void BCAnalysis::GFunction()
{
    figtitles.plot  = "G-Function";
    figtitles.x     = "G-Function";
    figtitles.y1    = "Pressure [psi]";
    figtitles.y2    = "G.dp/dG [psi]";
    figtitles.y3    = "dp/dG [psi]";

    // Calculate G
    x.clear(); x.squeeze();
    for (unsigned long i = 0; i<dfitanalysis->tD.size();++i){
        x.push_back (4.0 / M_PI * 4 / 3 *(pow((1 + dfitanalysis->tD[i]), 3.0 / 2.0) - pow(dfitanalysis->tD[i], 3.0 / 2.0) - 1.0));
    }
}


// ------------------------------------------------------------------------ //
// Slot: Creates data cursor and readings on the plot.
void BCAnalysis::dataCursorPlot(QMouseEvent *event)
{
    double xcoord = xaxis->pixelToCoord(event->pos().x());
    dtcrsr.pos = FSHelper::find(x, xcoord);

    QVector<double> x_crsr;
    QVector<double> y_crsr;
    QVector<double> xdx_crsr;
    QVector<double> dx_crsr;
    QVector<double> v_crsr;

    x_crsr.push_back(x[dtcrsr.pos]);
    x_crsr.push_back(x[dtcrsr.pos]);
    y_crsr.push_back(dfitanalysis->p_shut[dtcrsr.pos]);
    xdx_crsr.push_back(xdx[dtcrsr.pos]);
    dx_crsr.push_back(dx[dtcrsr.pos]);
    v_crsr.push_back(yaxis1->range().lower);
    v_crsr.push_back(yaxis1->range().upper);

    // To update just the annotations layer
    fig->setCurrentLayer("annotations");

    // Show gray vertical line
    if(dtcrsr.vericalLine == NULL){
        dtcrsr.vericalLine = fig->addGraph(xaxis, yaxis1);
        // fig->replot();
    }
    dtcrsr.vericalLine->setData( x_crsr, v_crsr );
    dtcrsr.vericalLine->setPen(QPen(Qt::gray, 1));
    dtcrsr.vericalLine->setPen(QPen(Qt::DashLine));

    // Show pressure data cursor
    if(dtcrsr.pressPlot == NULL){
        dtcrsr.pressPlot = fig->addGraph(xaxis, yaxis1);
        // fig->replot();
    }
    dtcrsr.pressPlot->setData( x_crsr, y_crsr );
    dtcrsr.pressPlot->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle,
                                                      QPen(Qt::black),
                                                      QBrush(yColor),
                                                      10));

    // Show x.dp/dx data cursor
    if(dtcrsr.logDerPlot == NULL){
        dtcrsr.logDerPlot = fig->addGraph(xaxis, yaxis2);
        // fig->replot();
    }
    dtcrsr.logDerPlot->setData( x_crsr, xdx_crsr );
    dtcrsr.logDerPlot->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle,
                                                       QPen(Qt::black),
                                                       QBrush(xdxColor),
                                                       10));

    // Show dp/dx data cursor
    if(dtcrsr.derPlot == NULL){
        dtcrsr.derPlot = fig->addGraph(xaxis, yaxis3);
        // fig->replot();
    }
    dtcrsr.derPlot->setData( x_crsr, dx_crsr );
    dtcrsr.derPlot->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle,
                                                    QPen(Qt::black),
                                                    QBrush(dxColor),
                                                    10));

    // Show pressure data reading
    if(dtcrsr.presLabel == NULL){
        dtcrsr.presLabel = new QCPItemText(fig);
        dtcrsr.presLabel->setPositionAlignment(Qt::AlignBottom|Qt::AlignLeft);
        dtcrsr.presLabel->position->setType(QCPItemPosition::ptPlotCoords);
        dtcrsr.presLabel->position->setAxes(xaxis, yaxis1);
        dtcrsr.presLabel->setFont(QFont("sans", 8));
        dtcrsr.presLabel->setColor(yColor);
        dtcrsr.presLabel->setPadding(QMargins(10, 0, 0, 10));
        dtcrsr.presLabel->setClipToAxisRect(false);
        // fig->replot();
    }
    dtcrsr.presLabel->position->setCoords(x_crsr[0], y_crsr[0]);
    dtcrsr.presLabel->setText(QString("%1 , %2").arg(x_crsr[0]).arg(y_crsr[0]));

    // Show x.dp/dx data reading
    if(dtcrsr.logDerLabel == NULL){
        dtcrsr.logDerLabel = new QCPItemText(fig);
        dtcrsr.logDerLabel->setPositionAlignment(Qt::AlignBottom|Qt::AlignLeft);
        dtcrsr.logDerLabel->position->setType(QCPItemPosition::ptPlotCoords);
        dtcrsr.logDerLabel->position->setAxes(xaxis, yaxis2);
        dtcrsr.logDerLabel->setFont(QFont("sans", 8));
        dtcrsr.logDerLabel->setColor(xdxColor);
        dtcrsr.logDerLabel->setPadding(QMargins(10, 0, 0, 10));
        dtcrsr.logDerLabel->setClipToAxisRect(false);
        // fig->replot();
    }
    dtcrsr.logDerLabel->position->setCoords(x_crsr[0], xdx_crsr[0]);
    dtcrsr.logDerLabel->setText(QString("%1 , %2").arg(x_crsr[0]).arg(xdx_crsr[0]));

    // Show dp/dx data reading
    if(dtcrsr.derLabel == NULL){
        dtcrsr.derLabel = new QCPItemText(fig);
        dtcrsr.derLabel->setPositionAlignment(Qt::AlignBottom|Qt::AlignLeft);
        dtcrsr.derLabel->position->setType(QCPItemPosition::ptPlotCoords);
        dtcrsr.derLabel->position->setAxes(xaxis, yaxis3);
        dtcrsr.derLabel->setFont(QFont("sans", 8));
        dtcrsr.derLabel->setColor(dxColor);
        dtcrsr.derLabel->setPadding(QMargins(10, 0, 0, 10));
        dtcrsr.derLabel->setClipToAxisRect(false);
        fig->replot();
    }
    dtcrsr.derLabel->position->setCoords(x_crsr[0], dx_crsr[0]);
    dtcrsr.derLabel->setText(QString("%1 , %2").arg(x_crsr[0]).arg(dx_crsr[0]));

    // Update just the annotation layer to improve performance
    fig->layer("annotations")-> QCPLayer::replot();
}


/****************************************************************************/
