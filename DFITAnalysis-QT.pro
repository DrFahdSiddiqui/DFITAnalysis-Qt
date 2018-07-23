#############################################################################
#                                                                           #
#           DIAGNOSTIC FRACTURE INJECTION TESTS ANALYSIS PROGRAM            #
#                               Version 1.0                                 #
#            Written for C++ by : Fahd Siddiqui and Aqsa Qureshi            #
#             https://github.com/DrFahdSiddiqui/DFITAnalysis-QT             #
#                                                                           #
# ------------------------------------------------------------------------- #
# LICENSE: MOZILLA 2.0                                                      #
#   This Source Code Form is subject to the terms of the Mozilla Public     #
#   License, v. 2.0. If a copy of the MPL was not distributed with this     #
#   file, You can obtain one at http://mozilla.org/MPL/2.0/.                #
#############################################################################

#############################################################################
# DOCUMENTATION                                                             #
#   QT Creator project file containing paths and names to images            #
#############################################################################


#############################################################################

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

requires(qtConfig(filedialog))

TARGET = DFITAnalysis-QT
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# Defines whether to use Eigen library. Eigen does not work in Windows
DEFINES += "USE_EIGEN=\"0\""

RESOURCES     = resources/resources.qrc


SOURCES += \
        src/main.cpp \
        src/mainwindow.cpp \
        src/bcanalysis.cpp \
        src/dfitanalysis.cpp \
        src/analysiswindow.cpp \
        include/qcustomplot/qcustomplot.cpp \
        include/FSHelper/fshelper.cpp



HEADERS += \
        header/mainwindow.h \
        header/bcanalysis.h \
        header/dfitanalysis.h \
        header/analysiswindow.h \
        include/qcustomplot/qcustomplot.h \
        include/FSHelper/fshelper.h \
        resources/programstrings.h


INCLUDEPATH += \
        include \
        header

FORMS += \
        forms/mainwindow.ui \
        forms/analysiswindow.ui

QMAKE_CXXFLAGS +=   -O3 \
                    -Ofast

RC_ICONS = resources/images/DFIT.ico


#############################################################################
