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
 *   Source file for MainWindow Class                                        *
 *   Class for creating the main window of the application.                  *
 ****************************************************************************/

/*****************************************************************************
 * TODO                                                                      *
 *   Implement saving to file                                                *
 *   Implement loading from file                                             *
 *   Code reusability for plots, axes ranges, saving etc                     *
 *   Code cleanup                                                            *
 ****************************************************************************/


/****************************************************************************/

#include <QtWidgets>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "resources/programstrings.h"


// ------------------------------------------------------------------------ //
// Constructor
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowTitle(versionString);
    // setWindowTitle(setText(tr(versionString)));
    setWindowIcon(QIcon(":images/logo.png"));

    // Sets column header labels
    ui->tableWidget->setHorizontalHeaderItem(0, new QTableWidgetItem("Time\n[secs]"));
    ui->tableWidget->setHorizontalHeaderItem(1, new QTableWidgetItem("Pressure\n[psi]"));
    ui->tableWidget->setHorizontalHeaderItem(2, new QTableWidgetItem("Rate\n[bpm]"));

    ui->tinjLabel->setText("   t<sub>inj</sub> = ");

    // Make sure to delete or find a better way
    dfitanalysis = new DFITAnalysis(ui->tprPlot);

    this->unsavedChanges = true;

    createActions();
    createStatusBar();

    readSettings();

#ifndef QT_NO_SESSIONMANAGER
    QGuiApplication::setFallbackSessionManagementEnabled(false);
    connect(qApp, &QGuiApplication::commitDataRequest,
            this, &MainWindow::commitData);
#endif

    setCurrentFile(QString());
    setUnifiedTitleAndToolBarOnMac(true);
}


// ------------------------------------------------------------------------ //
// Close window
void MainWindow::closeEvent(QCloseEvent *event)
{
    if (maybeSave()) {
        writeSettings();
        resetEverything();
        if (dfitanalysis)
            delete dfitanalysis;
        dfitanalysis = NULL;
        QApplication::closeAllWindows();
        event->accept();
    } else {
        event->ignore();
    }
}


// ------------------------------------------------------------------------ //
// New file: Define all things that will be reset here
void MainWindow::newFile()
{
    if (maybeSave()) {
        // Resetting
        resetEverything();

        setCurrentFile(QString());
    }
}


// ------------------------------------------------------------------------ //
// Open file
void MainWindow::open()
{
    if (maybeSave()) {
        QString fileName = QFileDialog::getOpenFileName(this, NULL, NULL,
                                                        tr("DFITFiles (*.dft)"));
        if (!fileName.isEmpty())
            resetEverything();
        loadFile(fileName);
    }
}


// ------------------------------------------------------------------------ //
// Load CSV
void MainWindow::load()
{
    QString fileName = QFileDialog::getOpenFileName(this);
    if (!fileName.isEmpty()){
        resetEverything();
        statusBar()->showMessage(tr("Loading file..."), 2000); // show number of lines read
        // Load data from file to memory
        FSHelper::readCsv(fileName, 1, dfitanalysis->t, dfitanalysis->p, dfitanalysis->r);

        loadCSV_actions();

        statusBar()->showMessage(tr("File loaded and plotted"), 2000); // show number of lines read

        unsavedChanges = true;
    }
}


// ------------------------------------------------------------------------ //
// Save file
bool MainWindow::save()
{
    if (curFile.isEmpty()) {
        return saveAs();
    } else {
        return saveFile(curFile);
    }
}


// ------------------------------------------------------------------------ //
// Save As
bool MainWindow::saveAs()
{
    QFileDialog dialog(this);
    dialog.setWindowModality(Qt::WindowModal);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.setNameFilter(tr("DFITFiles (*.dft)"));
    if (dialog.exec() != QDialog::Accepted)
        return false;
    return saveFile(dialog.selectedFiles().first());
}


// ------------------------------------------------------------------------ //
// Shows about box
void MainWindow::about()
{
    //    QMessageBox::about(this, tr("About Application"),
    //                       tr(aboutString));

    QMessageBox aboutBox;

    aboutBox.setText(tr(aboutString));
    aboutBox.setWindowTitle(versionString);
    aboutBox.setStandardButtons(QMessageBox::Ok);

    QPixmap myPixmap(QPixmap(":images/icon.png").scaled(80, 80, Qt::KeepAspectRatio));
    aboutBox.setIconPixmap(myPixmap);

    // about.setIconPixmap(QPixmap(":images/icon.png"));   // here is the error
    aboutBox.setDefaultButton(QMessageBox::Ok);
    aboutBox.show();
    aboutBox.exec();
}


// ------------------------------------------------------------------------ //
// Shows help box
void MainWindow::help()
{
    QDialog helpDialog;
    QScrollArea helpScroll;
    QVBoxLayout helpLayout;
    QLabel helpText;
    QLabel iconImage;

    helpDialog.setWindowTitle(versionString);
    helpDialog.setWindowIcon(QIcon(":image/icon.png") );

    helpScroll.setMinimumHeight(300);
    helpScroll.setMinimumWidth(200);
    helpScroll.setWidgetResizable(true);
    helpScroll.setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    helpScroll.setWidget(&helpText);

    helpText.setText(tr(helpString));
    helpText.setWordWrap(true);

    iconImage.setPixmap(QPixmap(QPixmap(":images/icon.png").scaled(80, 80, Qt::KeepAspectRatio)));
    iconImage.setAlignment(Qt::AlignHCenter);

    helpLayout.addWidget(&iconImage);
    helpLayout.addWidget(&helpScroll);

    helpDialog.setLayout(&helpLayout);
    helpDialog.exec();
}


// ------------------------------------------------------------------------ //
// Holds if changes need to be saved
void MainWindow::documentWasModified()
{
    setWindowModified(this->unsavedChanges);
    return;
}


// ------------------------------------------------------------------------ //
// Creates actions
void MainWindow::createActions()
{
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
    QToolBar *fileToolBar = addToolBar(tr("File"));

    // New
    const QIcon newIcon = QIcon(":images/new.png");
    QAction *newAct = new QAction(newIcon, tr("&New"), this);
    newAct->setShortcuts(QKeySequence::New);
    newAct->setStatusTip(tr("Create a new file"));
    connect(newAct, &QAction::triggered, this, &MainWindow::newFile);
    fileMenu->addAction(newAct);
    fileToolBar->addAction(newAct);

    // Open action and icon
    const QIcon openIcon = QIcon(":images/open.png");
    QAction *openAct = new QAction(openIcon, tr("&Open..."), this);
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setStatusTip(tr("Open an existing file"));
    connect(openAct, &QAction::triggered, this, &MainWindow::open);
    fileMenu->addAction(openAct);
    fileToolBar->addAction(openAct);

    // Load CSV
    const QIcon loadIcon = QIcon(":images/csv.png");
    QAction *loacAct = new QAction(loadIcon, tr("&Load CSV..."), this);
    //    loacAct->setShortcuts(QKeySequence::Open);
    loacAct->setStatusTip(tr("Load time, pressure and rate data from CSV file"));
    connect(loacAct, &QAction::triggered, this, &MainWindow::load);
    fileMenu->addAction(loacAct);
    fileToolBar->addAction(loacAct);

    // Save
    const QIcon saveIcon = QIcon(":images/save.png");
    QAction *saveAct = new QAction(saveIcon, tr("&Save"), this);
    saveAct->setShortcuts(QKeySequence::Save);
    saveAct->setStatusTip(tr("Save the document to disk"));
    connect(saveAct, &QAction::triggered, this, &MainWindow::save);
    fileMenu->addAction(saveAct);
    fileToolBar->addAction(saveAct);

    const QIcon saveAsIcon = QIcon(":images/saveas.png");
    QAction *saveAsAct = fileMenu->addAction(saveAsIcon, tr("Save &As..."), this, &MainWindow::saveAs);
    saveAsAct->setShortcuts(QKeySequence::SaveAs);
    saveAsAct->setStatusTip(tr("Save the document under a new name"));


    fileMenu->addSeparator();

    const QIcon exitIcon = QIcon(":images/exit.png");
    QAction *exitAct = fileMenu->addAction(exitIcon, tr("E&xit"), this, &QWidget::close);
    exitAct->setShortcuts(QKeySequence::Quit);
    exitAct->setStatusTip(tr("Exit the application"));

    /*
    // QMenu *editMenu = menuBar()->addMenu(tr("&Edit"));
    // QToolBar *editToolBar = addToolBar(tr("Edit"));
    //#ifndef QT_NO_CLIPBOARD
    //    const QIcon cutIcon = QIcon::fromTheme("edit-cut", QIcon(":/images/cut.png"));
    //    QAction *cutAct = new QAction(cutIcon, tr("Cu&t"), this);
    //    cutAct->setShortcuts(QKeySequence::Cut);
    //    cutAct->setStatusTip(tr("Cut the current selection's contents to the "
    //                            "clipboard"));
    //    connect(cutAct, &QAction::triggered, textEdit, &QPlainTextEdit::cut);
    //    editMenu->addAction(cutAct);
    //    editToolBar->addAction(cutAct);

    //    const QIcon copyIcon = QIcon::fromTheme("edit-copy", QIcon(":/images/copy.png"));
    //    QAction *copyAct = new QAction(copyIcon, tr("&Copy"), this);
    //    copyAct->setShortcuts(QKeySequence::Copy);
    //    copyAct->setStatusTip(tr("Copy the current selection's contents to the "
    //                             "clipboard"));
    //    connect(copyAct, &QAction::triggered, textEdit, &QPlainTextEdit::copy);
    //    editMenu->addAction(copyAct);
    //    editToolBar->addAction(copyAct);

    //    const QIcon pasteIcon = QIcon::fromTheme("edit-paste", QIcon(":/images/paste.png"));
    //    QAction *pasteAct = new QAction(pasteIcon, tr("&Paste"), this);
    //    pasteAct->setShortcuts(QKeySequence::Paste);
    //    pasteAct->setStatusTip(tr("Paste the clipboard's contents into the current "
    //                              "selection"));
    //    connect(pasteAct, &QAction::triggered, textEdit, &QPlainTextEdit::paste);
    //    editMenu->addAction(pasteAct);
    //    editToolBar->addAction(pasteAct);

    //    menuBar()->addSeparator();

    //#endif // !QT_NO_CLIPBOARD

    */

    QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));
    QAction *helpAct = helpMenu->addAction(tr("&Help"), this, &MainWindow::help);
    helpAct->setStatusTip(tr("Show Help"));

    QAction *aboutAct = helpMenu->addAction(tr("&About"), this, &MainWindow::about);
    aboutAct->setStatusTip(tr("Show the About box"));

    QAction *aboutQtAct = helpMenu->addAction(tr("About &Qt"), qApp, &QApplication::aboutQt);
    aboutQtAct->setStatusTip(tr("Show the Qt library's About box"));

    //#ifndef QT_NO_CLIPBOARD
    //    cutAct->setEnabled(false);
    //    copyAct->setEnabled(false);
    //    connect(textEdit, &QPlainTextEdit::copyAvailable, cutAct, &QAction::setEnabled);
    //    connect(textEdit, &QPlainTextEdit::copyAvailable, copyAct, &QAction::setEnabled);
    //#endif // !QT_NO_CLIPBOARD
}


// ------------------------------------------------------------------------ //
// Creats status bar
void MainWindow::createStatusBar()
{
    statusBar()->showMessage(tr("Ready"));
}


// ------------------------------------------------------------------------ //
// Read settings for the window
void MainWindow::readSettings()
{
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
    const QByteArray geometry = settings.value("geometry", QByteArray()).toByteArray();
    if (geometry.isEmpty()) {
        const QRect availableGeometry = QApplication::desktop()->availableGeometry(this);
        resize(availableGeometry.width() / 3, availableGeometry.height() / 2);
        move((availableGeometry.width() - width()) / 2,
             (availableGeometry.height() - height()) / 2);
    } else {
        restoreGeometry(geometry);
    }
}


// ------------------------------------------------------------------------ //
// Write settings for the window
void MainWindow::writeSettings()
{
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
    settings.setValue("geometry", saveGeometry());
}


// ------------------------------------------------------------------------ //
// Asks if changes need to be saved, and redirects approporately
bool MainWindow::maybeSave()
{
    if (!this->unsavedChanges)
        return true;
    const QMessageBox::StandardButton ret
            = QMessageBox::warning(this, tr("Application"),
                                   tr("The document has been modified.\n"
                                      "Do you want to save your changes?"),
                                   QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
    switch (ret) {
    case QMessageBox::Save:
        return save();
    case QMessageBox::Cancel:
        return false;
    default:
        break;
    }
    return true;
}


// ------------------------------------------------------------------------ //
// Actions to perform after reading CSV
void MainWindow::loadCSV_actions()
{
    // Set data to the table
    for (int i=0; i < dfitanalysis->t.size();++i){
        ui->tableWidget->insertRow(ui->tableWidget->rowCount());
        ui->tableWidget->setItem(i, 0, new QTableWidgetItem(QString("%1").arg(dfitanalysis->t[i])));
        ui->tableWidget->setItem(i, 1, new QTableWidgetItem(QString("%1").arg(dfitanalysis->p[i])));
        ui->tableWidget->setItem(i, 2, new QTableWidgetItem(QString("%1").arg(dfitanalysis->r[i])));
    }

    // Set data to plot
    dfitanalysis->jobPlot();

    // Enable the buttons
    ui->tinjButton->setEnabled(true);
    ui->saveJpgButton->setEnabled(true);
    ui->xminButton->setEnabled(true);
    ui->xmaxButton->setEnabled(true);
    ui->y1minButton->setEnabled(true);
    ui->y1maxButton->setEnabled(true);
    ui->y2minButton->setEnabled(true);
    ui->y2maxButton->setEnabled(true);
}


// ------------------------------------------------------------------------ //
// Loads file
void MainWindow::loadFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("Application"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(QDir::toNativeSeparators(fileName), file.errorString()));
        return;
    }

    // Reading the file
    QTextStream in(&file);
    QStringList lines = in.readAll().split('\n');

    // Check if actually the DFIT Analysis file was opened
    if (!(lines[24].contains(versionString))){
        QMessageBox::warning(this, tr("Application"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(QDir::toNativeSeparators(fileName), file.errorString()));
        return;
    }

    QString splitChar = ",";


    bool istprData = 0;

    struct BCATypes
    {
        int bcaType;
        double tClosure;
        double pClosure;
        double xClosure;
    };

    QVector<BCATypes> bcatypeV;

    for (int i = 25; i < lines.size(); ++i){

        if (lines[i].contains("tinj")){
            splitChar = " ";
            auto row_string = lines.at(i).split(splitChar, QString::SkipEmptyParts);
            dfitanalysis->tp = row_string[1].toDouble();
        }

        if (lines[i].contains("GFunction")){
            istprData = 0;
            bcatypeV.push_back({});
            bcatypeV.last().bcaType = 0;
            continue;
        }

        if (lines[i].contains("SRT")){
            istprData = 0;
            bcatypeV.push_back({});
            bcatypeV.last().bcaType = 1;
            continue;
        }

        if (lines[i].contains("tClosure")){
            splitChar = " ";
            auto row_string = lines.at(i).split(splitChar, QString::SkipEmptyParts);
            bcatypeV.last().tClosure = row_string[1].toDouble();
        }

        if (lines[i].contains("pClosure")){
            splitChar = " ";
            auto row_string = lines.at(i).split(splitChar, QString::SkipEmptyParts);
            bcatypeV.last().pClosure = row_string[1].toDouble();
        }

        if (lines[i].contains("xClosure")){
            splitChar = " ";
            auto row_string = lines.at(i).split(splitChar, QString::SkipEmptyParts);
            bcatypeV.last().xClosure = row_string[1].toDouble();
        }

        // Reads tpr data
        if (lines[i].contains("t, p, r")){
            istprData = 1;
            continue;
        }

        if (istprData){
            splitChar = ",";
            auto row_string = lines.at(i).split(splitChar, QString::SkipEmptyParts);
            if(row_string.size() < 3)
                continue;
            dfitanalysis->t.push_back(row_string[0].toDouble());
            dfitanalysis->p.push_back(row_string[1].toDouble());
            dfitanalysis->r.push_back(row_string[2].toDouble());
        }
    }

#ifndef QT_NO_CURSOR
    QApplication::setOverrideCursor(Qt::WaitCursor);
#endif
    // Perform actions for updating the ui
    this->loadCSV_actions();
    if (dfitanalysis->tp > 0){
        this->tinjButton_actions();
    }

    // Finally show windows etc
    for (int i=0;i<bcatypeV.size();++i){
        analysiswindowV.push_back(NULL);

        analysiswindowV.last() = new AnalysisWindow(dfitanalysis, bcatypeV[i].bcaType, analysiswindowV);
        analysiswindowV.last()->bcanalysis->tClosure = bcatypeV[i].tClosure;
        analysiswindowV.last()->bcanalysis->pClosure = bcatypeV[i].pClosure;
        analysiswindowV.last()->bcanalysis->xClosure = bcatypeV[i].xClosure;

        QVector<double> xdata;
        QVector<double> ydata;

        xdata.push_back(analysiswindowV.last()->bcanalysis->xClosure);
        xdata.push_back(analysiswindowV.last()->bcanalysis->xClosure);

        ydata.push_back(analysiswindowV.last()->bcanalysis->yaxis2->range().lower);
        ydata.push_back(analysiswindowV.last()->bcanalysis->yaxis2->range().upper);

        analysiswindowV.last()->bcanalysis->fig->setCurrentLayer("annotations");

        // Show gray vertical line
        analysiswindowV.last()->vLinePlot_vline(xdata, ydata);

        // Show the closure points
        analysiswindowV.last()->vLinePlot_label();
        analysiswindowV.last()->bcanalysis->fig->layer("annotations")-> QCPLayer::replot();

        analysiswindowV.last()->show();
    }

#ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
#endif

    setCurrentFile(fileName);
    statusBar()->showMessage(tr("File loaded"), 2000);
}


// ------------------------------------------------------------------------ //
// Saves file
bool MainWindow::saveFile(const QString &fileName)
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

    // Write save documentation header
    out << saveDocString << endl;

    // Write version number on line 24 this will be checked when loading
    out << versionString << endl;

    // Write injection time
    out << endl << "tinj " << dfitanalysis->tp << endl;

    // Writing t p r table csv
    out << endl << "t, p, r" << endl;
    for (unsigned long i=0; i<dfitanalysis->t.size(); ++i){
        out << dfitanalysis->t[i] << ", "
            << dfitanalysis->p[i] << ", "
            << dfitanalysis->r[i] <<endl;
    }

    // Writing the BCAnalysis closure picks
    for (int i=0; i<analysiswindowV.size();++i){
        if (analysiswindowV[i]->selector == 0)
            out << endl << "GFunction " << endl;
        if (analysiswindowV[i]->selector == 1)
            out << endl << "SRT " << endl;
        out << "  tClosure " << analysiswindowV[i]->bcanalysis->tClosure << endl;
        out << "  pClosure " << analysiswindowV[i]->bcanalysis->pClosure << endl;
        out << "  xClosure " << analysiswindowV[i]->bcanalysis->xClosure << endl;
    }


#ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
#endif

    setCurrentFile(fileName);
    statusBar()->showMessage(tr("File saved"), 2000);
    return true;
}


// ------------------------------------------------------------------------ //
// Sets title of window to open file name (Needs Modification)
void MainWindow::setCurrentFile(const QString &fileName)
{
    curFile = fileName;
    this->unsavedChanges = false;
    setWindowModified(false);

    QString shownName = curFile;
    if (curFile.isEmpty())
        shownName = "untitled.txt";
    setWindowFilePath(shownName);
}


// ------------------------------------------------------------------------ //
// Unknown (Needs Modification)
QString MainWindow::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}
#ifndef QT_NO_SESSIONMANAGER


// ------------------------------------------------------------------------ //
// Unknown
void MainWindow::commitData(QSessionManager &manager)
{
    if (manager.allowsInteraction()) {
        if (!maybeSave())
            manager.cancel();
    } else {
        // Non-interactive: save without asking
        if (this->unsavedChanges)
            save();
    }
}
#endif


// ------------------------------------------------------------------------ //
// Reset variables for new or load
void MainWindow::resetEverything()
{
    // Reset the variables
    dfitanalysis->reset(ui->tprPlot);

    // Reset the table
    if(!(ui->tableWidget->rowCount()==0))
        ui->tableWidget->setRowCount(0);

    // Reset button states
    ui->tinjButton->setEnabled(false);
    ui->gButton->setEnabled(false);
    ui->stButton->setEnabled(false);
    ui->saveJpgButton->setEnabled(false);
    ui->tinjLabel->setText("   t<sub>inj</sub> = ");
    ui->xminButton->setEnabled(false);
    ui->xmaxButton->setEnabled(false);
    ui->y1minButton->setEnabled(false);
    ui->y1maxButton->setEnabled(false);
    ui->y2minButton->setEnabled(false);
    ui->y2maxButton->setEnabled(false);

    // Close all windows
    while (analysiswindowV.size()>0)
        analysiswindowV.last()->close();
}


// ------------------------------------------------------------------------ //
// Get input from dialog for injection time
void MainWindow::on_tinjButton_clicked()
{
    // Make the input dialog
    bool ok;
    double num = QInputDialog::getDouble(this,
                                         tr("Injection time"),
                                         tr("Enter injection time in seconds:"),
                                         100,
                                         0, dfitanalysis->t.last(), 2,
                                         &ok);


    // Take appropriate actions
    if (ok){
        // Set tp to dialog from dialog
        dfitanalysis->tp = num;
        tinjButton_actions();

    }
}


// ------------------------------------------------------------------------ //
// Actions to take on injection time button clicked
void MainWindow::tinjButton_actions()
{
    // Display the tinj on window
    ui->tinjLabel->setText(QString("   t<sub>inj</sub> = %1 sec").arg(dfitanalysis->tp));

    // Enable G-function etc buttons
    ui->gButton->setEnabled(true);
    ui->stButton->setEnabled(true);

    // Binary search for tp_row
    unsigned long tp_row = FSHelper::find(dfitanalysis->t, dfitanalysis->tp);

    // Get shut-in time and pressure
    dfitanalysis->tD.clear();
    dfitanalysis->p_shut.clear();
    for(int i=tp_row; i<dfitanalysis->t.size(); ++i){
        dfitanalysis->tD.push_back((dfitanalysis->t[i]-dfitanalysis->tp)/dfitanalysis->tp);
        dfitanalysis->p_shut.push_back(dfitanalysis->p[i]);
    }
    unsavedChanges = true;
}


// ------------------------------------------------------------------------ //
// Actions to take on G-function button clicked
void MainWindow::on_gButton_clicked()
{
    statusBar()->showMessage(tr("Computing G-Function..."), 2000);
    analysiswindowV.push_back(NULL);
    analysiswindowV.last() = new AnalysisWindow(dfitanalysis, 0, analysiswindowV);
    analysiswindowV.last()->show();
    unsavedChanges = true;
}


// ------------------------------------------------------------------------ //
// Actions to take on SRT function button clicked
void MainWindow::on_stButton_clicked()
{
    // ++numAnalysisWindows;
    statusBar()->showMessage(tr("Computing SRT-Function..."), 2000);
    analysiswindowV.push_back(NULL);
    analysiswindowV.last() = new AnalysisWindow(dfitanalysis, 1, analysiswindowV);
    analysiswindowV.last()->show();
    unsavedChanges = true;
}


// ------------------------------------------------------------------------ //
// Set xmin range
void MainWindow::on_xminButton_clicked()
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
        dfitanalysis->xaxis->setRangeLower(num);
        dfitanalysis->fig->replot();
    }
}


// ------------------------------------------------------------------------ //
// Set y1min range
void MainWindow::on_y1minButton_clicked()
{
    // Make the input dialog
    bool ok;
    double num = QInputDialog::getDouble(this,
                                         tr("Minimum y1axis range"),
                                         tr("Enter minimum y1axis value:"),
                                         dfitanalysis->p.last(),
                                         -2147483647,  2147483647, 2,
                                         &ok);
    // Take appropriate actions
    if (ok){
        dfitanalysis->yaxis1->setRange(num, dfitanalysis->yaxis1->range().upper);
        dfitanalysis->fig->replot();
    }
}


// ------------------------------------------------------------------------ //
// Set y2min range
void MainWindow::on_y2minButton_clicked()
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
        dfitanalysis->yaxis2->setRangeLower(num);
        dfitanalysis->fig->replot();
    }
}


// ------------------------------------------------------------------------ //
// Set xmax range
void MainWindow::on_xmaxButton_clicked()
{
    // Make the input dialog
    bool ok;
    double num = QInputDialog::getDouble(this,
                                         tr("Maximum xaxis range"),
                                         tr("Enter maximum xaxis value:"),
                                         dfitanalysis->t.last(),
                                         -2147483647,  2147483647, 2,
                                         &ok);
    // Take appropriate actions
    if (ok){
        dfitanalysis->xaxis->setRangeUpper(num);
        dfitanalysis->fig->replot();
    }
}


// ------------------------------------------------------------------------ //
// Set y1max range
void MainWindow::on_y1maxButton_clicked()
{
    // Make the input dialog
    bool ok;
    double num = QInputDialog::getDouble(this,
                                         tr("Maximum y1axis range"),
                                         tr("Enter maximum y1axis value:"),
                                         dfitanalysis->p.first(),
                                         -2147483647,  2147483647, 2,
                                         &ok);
    // Take appropriate actions
    if (ok){
        dfitanalysis->yaxis1->setRangeUpper(num);
        dfitanalysis->fig->replot();
    }
}


// ------------------------------------------------------------------------ //
// Set y2max range
void MainWindow::on_y2maxButton_clicked()
{
    // Make the input dialog
    bool ok;
    double num = QInputDialog::getDouble(this,
                                         tr("Maximum y2axis range"),
                                         tr("Enter maximum y2axis value:"),
                                         50,
                                         -2147483647,  2147483647, 2,
                                         &ok);
    // Take appropriate actions
    if (ok){
        dfitanalysis->yaxis2->setRangeUpper(num);
        dfitanalysis->fig->replot();
    }
}


// ------------------------------------------------------------------------ //
// Save the figure as a jpg button
void MainWindow::on_saveJpgButton_clicked()
{
    QFileDialog dialog(this);
    dialog.setWindowModality(Qt::WindowModal);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    if (dialog.exec() != QDialog::Accepted)
        return;
    saveFig(dialog.selectedFiles().first());
}


// ------------------------------------------------------------------------ //
// Save the figure as jpg
bool MainWindow::saveFig(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("Application"),
                             tr("Cannot write file %1:\n%2.")
                             .arg(QDir::toNativeSeparators(fileName),
                                  file.errorString()));
        return false;
    }

    //QTextStream out(&file);
#ifndef QT_NO_CURSOR
    QApplication::setOverrideCursor(Qt::WaitCursor);
#endif

    ui->tprPlot->saveJpg(fileName,  0, 0, 5.0, 100 );

#ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
#endif

    statusBar()->showMessage(tr("File saved as jpg"), 2000);
    return true;
}


/****************************************************************************/
