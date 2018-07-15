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
 *   Header file for FSHelper Class                                          *
 *   Class contains the helper functions written by Fahd Siddiqui            *
 ****************************************************************************/


/****************************************************************************/

#ifndef FSHELPER_H
#define FSHELPER_H

#include <QVector>
#include <QFileInfo>
#include <QTextStream>
#include "Eigen/Dense"
#include "Eigen/Cholesky"

using namespace Eigen;
using namespace std;


// ------------------------------------------------------------------------ //
class FSHelper
{
    // -------------------------------------------------------------------- //
    /// Methods
public:
    // ------------------------------------------------------------------------ //
    // Reads data from csv file. Assumes a three column structure, in the order:
    // time, pressure, and rate.
    //   Inputs     :   QString filename, unsigned long skipRows
    //   Outputs    :   QVector t, QVector p, QVector r
    static void readCsv(QString filename, unsigned long skipRows,
                        QVector<double>& t, QVector<double>& p,
                        QVector<double>& r);

    // ------------------------------------------------------------------------ //
    // Matlab like find function for returning the nearest index of a value in
    // a sorted vector, using binary search.
    //   Inputs     :   Qvector v, double value
    //   Outputs    :   unsigned long idx
    static unsigned long find(QVector<double> &v, double value);


    // ------------------------------------------------------------------------ //
    // Matlab like find function for returning the nearest index of a value in
    // a sorted vector, using binary search. (Overlaod for std vector)
    //   Inputs     :   vector v, double value
    //   Outputs    :   unsigned long idx
    static unsigned long find(vector<double> &v, double value);


    // ------------------------------------------------------------------------ //
    // Computes smooth derivatives around the window using QVectors using Eigen
    //   Inputs     :   Qvector Xv, Qvector Yv, unsigned long dwindow
    //   Outputs    :   Qvector dYdXv
    static void smoothDerivative(QVector<double>& Xv, QVector<double>& Yv, int dwindow, QVector<double>& dYdXv);


    // ------------------------------------------------------------------------ //
    // Computes smooth derivatives around the window using std vector w/o Eigen
    //   Inputs     :   vector Xv, vector Yv, unsigned long dwindow
    //   Outputs    :   vector dYdXv
    static void smoothDerivative(vector<double>& Xv, vector<double>& Yv, unsigned long dwindow, vector<double>& dYdXv);

private:
    // ------------------------------------------------------------------------ //
    // Find the linear regression slope from given vectors x and y.
    //   Inputs     :   vector x, vector y
    //   Outputs    :   double slope
    static double slope(const vector<double>& x, const vector<double>& y);

    // ------------------------------------------------------------------------ //
    // Get a segment (y) from the given vector (x) based on start and interval
    //   Inputs     :   vector x, unsigned long start, unsgined long inverval
    //   Outputs    :   vector y
    static void segment(const vector<double>& x, unsigned long start, unsigned long interval, vector<double>& y);

private:
    // Constructor (Static class)
    FSHelper() {}
};


#endif // FSHELPER_H


/****************************************************************************/
