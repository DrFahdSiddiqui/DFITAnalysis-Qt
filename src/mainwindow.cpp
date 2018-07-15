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


// ------------------------------------------------------------------------ //
// Constructor
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowTitle("DFIT Analysis");

    // Sets column header labels
    ui->tableWidget->setHorizontalHeaderItem(0, new QTableWidgetItem("Time\n[secs]"));
    ui->tableWidget->setHorizontalHeaderItem(1, new QTableWidgetItem("Pressure\n[psi]"));
    ui->tableWidget->setHorizontalHeaderItem(2, new QTableWidgetItem("Rate\n[bpm]"));

    ui->tinjLabel->setText("   t<sub>inj</sub> = ");

    // Make sure to delete or find a better way
    dfitanalysis = new DFITAnalysis::DFITAnalysis(ui->tprPlot);

    this->unsavedChanges = true;

    createActions();
    createStatusBar();

    readSettings();
    // Connect the editable parts to this
    //    connect(textEdit->document(), &QTextDocument::contentsChanged,
    //            this, &MainWindow::documentWasModified);

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
        // Clear Out and reset all here


        // Resetting
        resetEverything();

        // TODO: Reset the plot as well
        setCurrentFile(QString());
    }
}


// ------------------------------------------------------------------------ //
// Open file
void MainWindow::open()
{
    if (maybeSave()) {
        QString fileName = QFileDialog::getOpenFileName(this);
        if (!fileName.isEmpty())
            loadFile(fileName);
    }
}


// ------------------------------------------------------------------------ //
// Load CSV
void MainWindow::load()
{
    QString fileName = QFileDialog::getOpenFileName(this);
    if (!fileName.isEmpty())
        loadCSV(fileName);
    int dummy; dummy =0;
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
    if (dialog.exec() != QDialog::Accepted)
        return false;
    return saveFile(dialog.selectedFiles().first());
}


// ------------------------------------------------------------------------ //
// Shows about box (Needs Modification, low priority)
void MainWindow::about()
{
    QMessageBox::about(this, tr("About Application"),
                       tr("The <b>Application</b> example demonstrates how to "
                          "write modern GUI applications using Qt, with a menu bar, "
                          "toolbars, and a status bar."));
}


// ------------------------------------------------------------------------ //
// Holds if changes need to be saved (Needs Modification)
void MainWindow::documentWasModified()
{
    setWindowModified(this->unsavedChanges);
    //setWindowModified(textEdit->document()->isModified());
    return;
}


// ------------------------------------------------------------------------ //
// Creates actions
void MainWindow::createActions()
{
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
    QToolBar *fileToolBar = addToolBar(tr("File"));
    const QIcon newIcon = QIcon::fromTheme("document-new", QIcon(":/images/new.png"));
    QAction *newAct = new QAction(newIcon, tr("&New"), this);
    newAct->setShortcuts(QKeySequence::New);
    newAct->setStatusTip(tr("Create a new file"));
    connect(newAct, &QAction::triggered, this, &MainWindow::newFile);
    fileMenu->addAction(newAct);
    fileToolBar->addAction(newAct);

    // FS Load CSV
    const QIcon loadIcon = QIcon::fromTheme("document-open", QIcon(":/images/open.png"));
    QAction *loacAct = new QAction(loadIcon, tr("&Load CSV..."), this);
    //    loacAct->setShortcuts(QKeySequence::Open);
    loacAct->setStatusTip(tr("Load time, pressure and rate data from CSV file"));
    connect(loacAct, &QAction::triggered, this, &MainWindow::load);
    fileMenu->addAction(loacAct);
    fileToolBar->addAction(loacAct);

    const QIcon openIcon = QIcon::fromTheme("document-open", QIcon(":/images/open.png"));
    QAction *openAct = new QAction(openIcon, tr("&Open..."), this);
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setStatusTip(tr("Open an existing file"));
    connect(openAct, &QAction::triggered, this, &MainWindow::open);
    fileMenu->addAction(openAct);
    fileToolBar->addAction(openAct);

    const QIcon saveIcon = QIcon::fromTheme("document-save", QIcon(":/images/save.png"));
    QAction *saveAct = new QAction(saveIcon, tr("&Save"), this);
    saveAct->setShortcuts(QKeySequence::Save);
    saveAct->setStatusTip(tr("Save the document to disk"));
    connect(saveAct, &QAction::triggered, this, &MainWindow::save);
    fileMenu->addAction(saveAct);
    fileToolBar->addAction(saveAct);

    const QIcon saveAsIcon = QIcon::fromTheme("document-save-as");
    QAction *saveAsAct = fileMenu->addAction(saveAsIcon, tr("Save &As..."), this, &MainWindow::saveAs);
    saveAsAct->setShortcuts(QKeySequence::SaveAs);
    saveAsAct->setStatusTip(tr("Save the document under a new name"));


    fileMenu->addSeparator();

    const QIcon exitIcon = QIcon::fromTheme("application-exit");
    QAction *exitAct = fileMenu->addAction(exitIcon, tr("E&xit"), this, &QWidget::close);
    exitAct->setShortcuts(QKeySequence::Quit);
    exitAct->setStatusTip(tr("Exit the application"));

    QMenu *editMenu = menuBar()->addMenu(tr("&Edit"));
    QToolBar *editToolBar = addToolBar(tr("Edit"));
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

    QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));
    QAction *aboutAct = helpMenu->addAction(tr("&About"), this, &MainWindow::about);
    aboutAct->setStatusTip(tr("Show the application's About box"));


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
// Asks if changes need to be saved, and redirects approporately (Needs Modification)
bool MainWindow::maybeSave()
{
    if (!this->unsavedChanges) // FS: Done
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
// Loads and sets CSV to table
void MainWindow::loadCSV(const QString &fileName)
{
    resetEverything();
    statusBar()->showMessage(tr("Loading file..."), 2000); // show number of lines read
    // Load data from file to memory
    FSHelper::readCsv(fileName, 1, dfitanalysis->t, dfitanalysis->p, dfitanalysis->r);

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

    statusBar()->showMessage(tr("File loaded and plotted"), 2000); // show number of lines read

    unsavedChanges = true;
}


// ------------------------------------------------------------------------ //
// Loads file (Needs modification)
void MainWindow::loadFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("Application"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(QDir::toNativeSeparators(fileName), file.errorString()));
        return;
    }

    // TODO: Implement reading JSON file
    // QTextStream in(&file);

#ifndef QT_NO_CURSOR
    QApplication::setOverrideCursor(Qt::WaitCursor);
#endif


    // TODO: Implement setting data to appropriate places tableview etc
    //    textEdit->setPlainText(in.readAll()); // Read to something else


#ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
#endif

    setCurrentFile(fileName);
    statusBar()->showMessage(tr("File loaded"), 2000);
}


// ------------------------------------------------------------------------ //
// Saves file (Modification needed)
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
    // TODO: Implement writing data to JSON file
    // out << textEdit->toPlainText();
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
    //    textEdit->document()->setModified(false); // Change this to something else
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
    if(!ui->tableWidget->rowCount()==0)
        ui->tableWidget->setRowCount(0);

    // Reset plot
    //ui->tprPlot->plotLayout()->clear();

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
}


// ------------------------------------------------------------------------ //
// Actions to take on injection time button clicked
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
    if (ok && num > 0){
        // Set tp to dialog from dialog
        dfitanalysis->tp = num;

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
    }
}


// ------------------------------------------------------------------------ //
// Actions to take on G-function button clicked
void MainWindow::on_gButton_clicked()
{
    statusBar()->showMessage(tr("Computing G-Function..."), 2000);
    analysiswindow = new AnalysisWindow::AnalysisWindow(dfitanalysis, 0);
    analysiswindow->show();
}


// ------------------------------------------------------------------------ //
// Actions to take on SRT function button clicked
void MainWindow::on_stButton_clicked()
{
    statusBar()->showMessage(tr("Computing SRT-Function..."), 2000);
    analysiswindow = new AnalysisWindow::AnalysisWindow(dfitanalysis, 1);
    analysiswindow->show();
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
