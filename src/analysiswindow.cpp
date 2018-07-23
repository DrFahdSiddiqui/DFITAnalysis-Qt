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
 *   Source file for AnalysisWindow Class                                    *
 *   Class for creating the secondary window of the application.             *
 ****************************************************************************/

/*****************************************************************************
 * TODO                                                                      *                                         *
 *   Code reusability for plots, axes ranges, saving etc                     *
 ****************************************************************************/


/****************************************************************************/

#include "analysiswindow.h"
#include "ui_analysiswindow.h"


// ------------------------------------------------------------------------ //
// Constructor
AnalysisWindow::AnalysisWindow(DFITAnalysis* dfitanalysis, int selector,
                               QVector<AnalysisWindow*> &analysiswindowV,
                               QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::AnalysisWindow)
{
    ui->setupUi(this);
    this->selector = selector;
    this->analysiswindowV = &analysiswindowV;
    windowNum = analysiswindowV.size()-1;

    bcanalysis = new BCAnalysis(ui->fig, dfitanalysis);

    // Perform G-Function related calculations
    if (selector == 0){
        setWindowTitle("G-Function Analysis");
        bcanalysis->GFunction();
    }

    // Perform square root time related calculations
    if (selector == 1){
        setWindowTitle("SRT Analysis");
        bcanalysis->SRTFunction();
    }

    // Performs analysis and plots the data
    bcanalysis->analysisPlot();
}


// ------------------------------------------------------------------------ //
// Destructor
AnalysisWindow::~AnalysisWindow()
{
    delete bcanalysis;
    delete ui;
}


// ------------------------------------------------------------------------ //
// Close window
void AnalysisWindow::closeEvent(QCloseEvent *event)
{
    // delete analysiswindowV[windowNum];
    for (int i=0; i<analysiswindowV->size();++i)
        if (i>windowNum)
            analysiswindowV->at(i)->windowNum -=1;

    // There will be a memory leak (I guess?) if Windows
#ifndef _WIN32
    delete analysiswindowV->at(windowNum);
#else
    delete bcanalysis;
    delete ui;
#endif

    analysiswindowV->removeAt(windowNum);
    event->accept();
}

// ------------------------------------------------------------------------ //
// Gets new smoothing window and replots
void AnalysisWindow::on_dSmoothButton_clicked()
{
    // Make the input dialog
    bool ok;
    long num = QInputDialog::getInt(this,
                                    QObject::tr("Smoothing window"),
                                    QObject::tr("Enter number of points in the derivative window:"),
                                    bcanalysis->dwindow,
                                    1, bcanalysis->dfitanalysis->p_shut.count()-1, 2,
                                    &ok);

    // Take appropriate actions
    if (ok){
        bcanalysis->dwindow = num;
        // Redo the analysis and plot
        bcanalysis->calculateDerivatives();
        // Plot the variables
        bcanalysis->plotData(); // Plot actual data
        bcanalysis->fig->replot();
    }
}


// ------------------------------------------------------------------------ //
// Connects the mouse to straight line drawing
void AnalysisWindow::on_stLineButton_clicked()
{
    QObject::connect(bcanalysis->fig, SIGNAL(mouseMove(QMouseEvent*)), this , SLOT(stLinePlot(QMouseEvent*)));
    QObject::connect(bcanalysis->fig, SIGNAL(mouseRelease(QMouseEvent*)), this , SLOT(stLinePlot(QMouseEvent*)));
}


// ------------------------------------------------------------------------ //
// Draws the straight line thru origin on click
void AnalysisWindow::stLinePlot(QMouseEvent *event)
{
    // BCAnalysis::stLinePlot(event);
    QVector<double> xdata;
    QVector<double> ydata;

    xdata.push_back(0);
    ydata.push_back(0);
    xdata.push_back(bcanalysis->xaxis->pixelToCoord(event->pos().x()));
    ydata.push_back(bcanalysis->yaxis2->pixelToCoord(event->pos().y()));

    bcanalysis->fig->setCurrentLayer("annotations");

    bcanalysis->stLnPlot->setData( xdata, ydata );
    bcanalysis->stLnPlot->setPen(QPen(Qt::black, 1.5));
    bcanalysis->stLnPlot->setPen(QPen(Qt::DashLine));

    bcanalysis->fig->layer("annotations")-> QCPLayer::replot();
    if(event->type()==QMouseEvent::MouseButtonRelease){
        disconnect(bcanalysis->fig, 0, this, SLOT(stLinePlot(QMouseEvent*)));
        bcanalysis->fig->layer("annotations")-> QCPLayer::replot();
    }
}


// ------------------------------------------------------------------------ //
// Connects the mouse to vertical line drawing
void AnalysisWindow::on_vLineButton_clicked()
{
    QObject::connect(bcanalysis->fig, SIGNAL(mouseMove(QMouseEvent*)), this , SLOT(vLinePlot(QMouseEvent*)));
    QObject::connect(bcanalysis->fig, SIGNAL(mouseRelease(QMouseEvent*)), this , SLOT(vLinePlot(QMouseEvent*)));
}


// ------------------------------------------------------------------------ //
// Draws the vertical line for closure and displays values on plot
void AnalysisWindow::vLinePlot(QMouseEvent *event)
{
    // BCAnalysis::stLinePlot(event);
    QVector<double> xdata;
    QVector<double> ydata;

    xdata.push_back(bcanalysis->x[bcanalysis->dtcrsr.pos]);
    xdata.push_back(bcanalysis->x[bcanalysis->dtcrsr.pos]);

    ydata.push_back(bcanalysis->yaxis2->range().lower);
    ydata.push_back(bcanalysis->yaxis2->range().upper);

    bcanalysis->fig->setCurrentLayer("annotations");

    // Show gray vertical line
    vLinePlot_vline(xdata, ydata);

    // Lebel the plot on mouse release
    if(event->type()==QMouseEvent::MouseButtonRelease){
        bcanalysis->pClosure = bcanalysis->dfitanalysis->p_shut[bcanalysis->dtcrsr.pos];
        bcanalysis->tClosure = bcanalysis->dfitanalysis->tD[bcanalysis->dtcrsr.pos] * bcanalysis->dfitanalysis->tp;
        bcanalysis->xClosure = bcanalysis->x[bcanalysis->dtcrsr.pos];

        // Adding label to display pclosure on plot
        vLinePlot_label();
        bcanalysis->fig->layer("annotations")-> QCPLayer::replot();
        disconnect(bcanalysis->fig, 0, this, SLOT(vLinePlot(QMouseEvent*)));
    }
}


// ------------------------------------------------------------------------ //
// Show gray vertical line
void AnalysisWindow::vLinePlot_vline(    QVector<double> xdata, QVector<double> ydata)
{
    bcanalysis->fig->setCurrentLayer("annotations");

    bcanalysis->clsrPtPlot->setData( xdata, ydata);
    bcanalysis->clsrPtPlot->setPen(QPen(Qt::black, 1.5));
    bcanalysis->clsrPtPlot->setPen(QPen(Qt::DashLine));
}


// ------------------------------------------------------------------------ //
// Adding label to display pclosure on plot
void AnalysisWindow::vLinePlot_label()
{
    if (selector == 0)
        bcanalysis->closureLabel->setText(QString("Closure pressure = %1 psi "
                                                  "\nClosure time = %2 sec "
                                                  "\nClosure G = %3 ")
                                          .arg(bcanalysis->pClosure)
                                          .arg(bcanalysis->tClosure)
                                          .arg(bcanalysis->xClosure));
    if (selector == 1)
        bcanalysis->closureLabel->setText(QString("Closure pressure = %1 psi "
                                                  "\nClosure time = %2 sec "
                                                  "\nClosure tD^1/2 = %3 ")
                                          .arg(bcanalysis->pClosure)
                                          .arg(bcanalysis->tClosure)
                                          .arg(bcanalysis->xClosure));

    bcanalysis->closureLabel->setVisible(true);
}


// ------------------------------------------------------------------------ //
// Set xmin range
void AnalysisWindow::on_xminButton_clicked()
{
    // Make the input dialog
    bool ok;
    double num = QInputDialog::getDouble(this,
                                         tr("Minimum xaxis range"),
                                         tr("Enter minimum xaxis value:"),
                                         0,
                                         -2147483647,  2147483647, 2,
                                         &ok);
    // Take appropriate actions
    if (ok){
        bcanalysis->xaxis->setRangeLower(num);
        bcanalysis->fig->replot();
    }
}


// ------------------------------------------------------------------------ //
// Set y1min range
void AnalysisWindow::on_y1minButton_clicked()
{
    // Make the input dialog
    bool ok;
    double num = QInputDialog::getDouble(this,
                                         tr("Minimum y1axis range"),
                                         tr("Enter minimum y1axis value:"),
                                         bcanalysis->dfitanalysis->p_shut.last(),
                                         -2147483647,  2147483647, 2,
                                         &ok);
    // Take appropriate actions
    if (ok){
        bcanalysis->yaxis1->setRange(num, bcanalysis->yaxis1->range().upper);
        bcanalysis->fig->replot();
    }
}


// ------------------------------------------------------------------------ //
// Set y2min range
void AnalysisWindow::on_y2minButton_clicked()
{
    // Make the input dialog
    bool ok;
    double num = QInputDialog::getDouble(this,
                                         tr("Minimum y2axis range"),
                                         tr("Enter minimum y2axis value:"),
                                         0,
                                         -2147483647,  2147483647, 2,
                                         &ok);
    // Take appropriate actions
    if (ok){
        bcanalysis->yaxis2->setRangeLower(num);
        bcanalysis->fig->replot();
    }
}


// ------------------------------------------------------------------------ //
// Set y3min range
void AnalysisWindow::on_y3minButton_clicked()
{
    // Make the input dialog
    bool ok;
    double num = QInputDialog::getDouble(this,
                                         tr("Minimum y3axis range"),
                                         tr("Enter minimum y3axis value:"),
                                         0,
                                         -2147483647,  2147483647, 2,
                                         &ok);
    // Take appropriate actions
    if (ok){
        bcanalysis->yaxis3->setRangeLower(num);
        bcanalysis->fig->replot();
    }
}


// ------------------------------------------------------------------------ //
// Set xmax range
void AnalysisWindow::on_xmaxButton_clicked()
{
    // Make the input dialog
    bool ok;
    double num = QInputDialog::getDouble(this,
                                         tr("Maximum xaxis range"),
                                         tr("Enter maximum xaxis value:"),
                                         bcanalysis->x.last(),
                                         -2147483647,  2147483647, 2,
                                         &ok);
    // Take appropriate actions
    if (ok){
        bcanalysis->xaxis->setRangeUpper(num);
        bcanalysis->fig->replot();
    }
}


// ------------------------------------------------------------------------ //
// Set y1max range
void AnalysisWindow::on_y1maxButton_clicked()
{
    // Make the input dialog
    bool ok;
    double num = QInputDialog::getDouble(this,
                                         tr("Maximum y1axis range"),
                                         tr("Enter maximum y1axis value:"),
                                         bcanalysis->dfitanalysis->p_shut.first(),
                                         -2147483647,  2147483647, 2,
                                         &ok);
    // Take appropriate actions
    if (ok){
        bcanalysis->yaxis1->setRangeUpper(num);
        bcanalysis->fig->replot();
    }
}


// ------------------------------------------------------------------------ //
// Set y2max range
void AnalysisWindow::on_y2maxButton_clicked()
{
    // Make the input dialog
    bool ok;
    double num = QInputDialog::getDouble(this,
                                         tr("Maximum y2axis range"),
                                         tr("Enter maximum y2axis value:"),
                                         4000,
                                         -2147483647,  2147483647, 2,
                                         &ok);
    // Take appropriate actions
    if (ok){
        bcanalysis->yaxis2->setRangeUpper(num);
        bcanalysis->fig->replot();
    }
}


// ------------------------------------------------------------------------ //
// Set y3max range
void AnalysisWindow::on_y3maxButton_clicked()
{
    // Make the input dialog
    bool ok;
    double num = QInputDialog::getDouble(this,
                                         tr("Maximum y3axis range"),
                                         tr("Enter maximum y3axis value:"),
                                         4000,
                                         -2147483647,  2147483647, 2,
                                         &ok);
    // Take appropriate actions
    if (ok){
        bcanalysis->yaxis3->setRangeUpper(num);
        bcanalysis->fig->replot();
    }
}


// ------------------------------------------------------------------------ //
// Save the figure as a jpg button
void AnalysisWindow::on_saveJpgButton_clicked()
{
    QFileDialog dialog(this);
    dialog.setWindowModality(Qt::WindowModal);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.setNameFilter(tr("JPG (*.jpg)"));
    if (dialog.exec() != QDialog::Accepted)
        return;
    saveFig(dialog.selectedFiles().first());
}


// ------------------------------------------------------------------------ //
// Saves figure as jpg
bool AnalysisWindow::saveFig(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("Application"),
                             tr("Cannot write file %1:\n%2.")
                             .arg(QDir::toNativeSeparators(fileName),
                                  file.errorString()));
        return false;
    }

#ifndef QT_NO_CURSOR
    QApplication::setOverrideCursor(Qt::WaitCursor);
#endif

    ui->fig->saveJpg(fileName,  0, 0, 5.0, 100 );

#ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
#endif

    statusBar()->showMessage(tr("Figure saved as jpg"), 2000);
    return true;
}


// ------------------------------------------------------------------------ //
// Exports figure as csv
void AnalysisWindow::on_exportCsvButton_clicked()
{
    QFileDialog dialog(this);
    dialog.setWindowModality(Qt::WindowModal);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    if (dialog.exec() != QDialog::Accepted)
        return;
    saveCsv(dialog.selectedFiles().first());
}


// ------------------------------------------------------------------------ //
// Exports figure as csv
bool AnalysisWindow::saveCsv(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("Application"),
                             tr("Cannot write file %1:\n%2.")
                             .arg(QDir::toNativeSeparators(fileName),
                                  file.errorString()));
        return false;
    }
    QTextStream out(&file);
#ifndef QT_NO_CURSOR
    QApplication::setOverrideCursor(Qt::WaitCursor);
#endif
    // Write file header 23 rows
    out << headerString;
    // Write export documentation header
    out << exportCsvDocString << endl;
    // Write version number on line 24 this will be checked when loading
    out << versionString << endl;

    if (bcanalysis->tClosure != 0.0)
    {
        out << endl << "Results" << endl;
        out << "pClosure = " << bcanalysis->pClosure << endl;
        out << "tClosure = " << bcanalysis->tClosure << endl;
    }
    if (selector == 0){
        if (bcanalysis->tClosure != 0.0)
            out << "GClosure = " << bcanalysis->xClosure;
        out << endl << "G, p_shut, G.dp/dG, dp/dG" << endl;
    }
    if (selector == 1){
        if (bcanalysis->tClosure != 0.0)
            out << "tD^1/2Closure = " << bcanalysis->xClosure;
        out << endl << "tD^1/2, p_shut, tD^1/2.dp/dtD^1/2, dp/dtD^1/2" << endl;
    }
    // Writing t p r table csv
    for (unsigned long i=0; i<bcanalysis->x.size(); ++i){
        out << bcanalysis->x[i] << ", "
            << bcanalysis->dfitanalysis->p_shut[i] << ", "
            << bcanalysis->xdx[i] << ", "
            << bcanalysis->dx[i] << endl;
    }
#ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
#endif

    statusBar()->showMessage(tr("Exported data as csv"), 2000);
    return true;
}


/****************************************************************************/
