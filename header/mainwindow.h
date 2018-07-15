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
 *   Header file for MainWindow Class                                        *
 *   Class for creating the main window of the application.                  *
 ****************************************************************************/


/****************************************************************************/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "dfitanalysis.h"
#include "analysiswindow.h"
#include "FSHelper/fshelper.h"

class QAction;
class QMenu;
// class QPlainTextEdit;
class QSessionManager;


namespace Ui {
class MainWindow;
}

// ------------------------------------------------------------------------ //
class MainWindow : public QMainWindow
{
    Q_OBJECT

    // -------------------------------------------------------------------- //
    /// Properties
public:
    bool unsavedChanges;
    DFITAnalysis *dfitanalysis;
    AnalysisWindow *analysiswindow;
    Ui::MainWindow *ui;
    // QPlainTextEdit *textEdit;
    QString curFile;


    // -------------------------------------------------------------------- //
    /// Methods
public:
    // Constructor
    explicit MainWindow(QWidget *parent = 0);

    // Loads and sets CSV to table
    void loadCSV(const QString &fileName);

    // Loads file
    void loadFile(const QString &fileName);

protected:
    // Close window
    void closeEvent(QCloseEvent *event) override;

private slots:
    // New file: Define all things that will be reset here
    void newFile();

    // Load CSV
    void load();

    // Open file
    void open();

    // Save file
    bool save();

    // Save As
    bool saveAs();

    // Shows about box
    void about();

    // Holds if changes need to be saved (Needs Modification)
    void documentWasModified();

#ifndef QT_NO_SESSIONMANAGER
    // Unknown
    void commitData(QSessionManager &);
#endif

    // Actions to take on injection time button clicked
    void on_tinjButton_clicked();

    // Actions to take on G-function button clicked
    void on_gButton_clicked();

    // Actions to take on SRT function button clicked
    void on_stButton_clicked();

    // Set xmin range
    void on_xminButton_clicked();

    // Set y1min range
    void on_y1minButton_clicked();

    // Set y2min range
    void on_y2minButton_clicked();

    // Set xmax range
    void on_xmaxButton_clicked();

    // Set y1max range
    void on_y1maxButton_clicked();

    // Set y2max range
    void on_y2maxButton_clicked();

    // Save the figure as a jpg button
    void on_saveJpgButton_clicked();

private:
    // Creates actions
    void createActions();

    // Creats status bar
    void createStatusBar();

    // Read settings for the window
    void readSettings();

    // Write settings for the window
    void writeSettings();

    // Asks if changes need to be saved, and redirects approporately (Needs Modification)
    bool maybeSave();

    void resetEverything();

    // Saves file (Modification needed)
    bool saveFile(const QString &fileName);

    // Sets title of window to open file name (Needs Modification)
    void setCurrentFile(const QString &fileName);

    // Unknown (Needs Modification)
    QString strippedName(const QString &fullFileName);

    // Save the figure as jpg
    bool saveFig(const QString &fileName);

};


#endif // MAINWINDOW_H


/****************************************************************************/
