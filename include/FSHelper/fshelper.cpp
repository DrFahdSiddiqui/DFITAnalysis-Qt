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
 *   Source file for FSHelper Class                                          *
 *   Class contains the helper functions written by Fahd Siddiqui            *
 ****************************************************************************/

/*****************************************************************************
 * TODO                                                                      *
 *   Rigorous code testing                                                   *
 *   Remove warnings about Eigen during compile                              *
 *   Code cleanup                                                            *
 ****************************************************************************/


/****************************************************************************/

#include "fshelper.h"

// ------------------------------------------------------------------------ //
// Matlab like find function for returning the nearest index of a value in
// a sorted vector, using binary search.
//   Inputs     :   Qvector v, double value
//   Outputs    :   unsigned long idx
unsigned long FSHelper::find(QVector<double> &v, double value){
    // Binary search for position
    QVector<double>::iterator idx_iter;
    idx_iter = std::lower_bound(v.begin(), v.end(), value);
    unsigned long idx = idx_iter - v.begin();

    // Because lower_bound is used, if not found, then it must be the last value
    if(idx >= (unsigned long)v.size())
        idx = v.size() -1;
    return idx;
}


// ------------------------------------------------------------------------ //
// Matlab like find function for returning the nearest index of a value in
// a sorted vector, using binary search. (Overlaod for std vector)
//   Inputs     :   vector v, double value
//   Outputs    :   unsigned long idx
unsigned long FSHelper::find(vector<double> &v, double value){
    // Binary search for position
    vector<double>::iterator idx_iter;
    idx_iter = std::lower_bound(v.begin(), v.end(), value);
    unsigned long idx = idx_iter - v.begin();

    // Because lower_bound is used, if not found, then it must be the last value
    if(idx >= (unsigned long)v.size())
        idx = v.size() -1;
    return idx;
}


// ------------------------------------------------------------------------ //
// Computes smooth derivatives around the window using QVectors using Eigen
//   Inputs     :   Qvector Xv, Qvector Yv, unsigned long dwindow
//   Outputs    :   Qvector dYdXv
void FSHelper::smoothDerivative(QVector<double>& Xv, QVector<double>& Yv,
                                int dwindow, QVector<double>& dYdXv)
{
    // double* X = new double[Xv.size()]();
    // double* Y = new double[Yv.size()]();
    unsigned long n = Xv.size();

    VectorXd X   =   Map<VectorXd>(Xv.data(), n);
    VectorXd Y   =  Map<VectorXd>(Yv.data(), n);
    VectorXd dYdX =  VectorXd::Zero(n);

    VectorXd x(2*dwindow);
    VectorXd y(2*dwindow);
    MatrixXd A(2*dwindow, 2); // assert that dwindow = x.length()
    VectorXd result(2);
    A.col(0) = MatrixXd::Ones(2*dwindow,1);
    for (unsigned long i = 0 + dwindow; i < n - dwindow; ++i){
        x = X.segment(i - dwindow, 2 * dwindow);
        y = Y.segment(i - dwindow, 2 * dwindow);
        A.col(1) = x;
        result = (A.transpose() * A).ldlt().solve(A.transpose() * y);
        dYdX(i) = abs(result(1));
    }
    // Copy results
    for (unsigned long i = 0; i < dYdX.size(); ++i){
        dYdXv.push_back(dYdX(i));
    }

}


// ------------------------------------------------------------------------ //
// Computes smooth derivatives around the window using std vector w/o Eigen
//   Inputs     :   vector Xv, vector Yv, unsigned long dwindow
//   Outputs    :   vector dYdXv
void FSHelper::smoothDerivative(vector<double>& Xv, vector<double>& Yv,
                                unsigned long dwindow, vector<double>& dYdXv)
{
    unsigned long n = Xv.size();

    vector<double> x(2*dwindow);
    vector<double> y(2*dwindow);
    double result;

    for (unsigned long i = 0; i < dwindow; ++i){
        dYdXv.push_back(0.0);
    }

    for (unsigned long i = 0 + dwindow; i < n - dwindow; ++i){
        segment(Xv, i-dwindow, 2*dwindow, x);
        segment(Yv, i-dwindow, 2*dwindow, y);

        result = abs(slope(x, y));
        dYdXv.push_back(result);
    }

    for (unsigned long i = 0; i < dwindow; ++i){
        dYdXv.push_back(0.0);
    }
}


// ------------------------------------------------------------------------ //
// Get a segment (y) from the given vector (x) based on start and interval
//   Inputs     :   vector x, unsigned long start, unsgined long inverval
//   Outputs    :   vector y
void FSHelper::segment(const vector<double>& x, unsigned long start,
                       unsigned long interval, vector<double>& y)
{
    vector<double>::const_iterator first;
    vector<double>::const_iterator last;

    first = x.begin() + start;
    last  = x.begin() + start + interval;
    vector<double>dummy(first, last);
    y = dummy;
}


// ------------------------------------------------------------------------ //
// Find the linear regression slope from given vectors x and y.
//   Inputs     :   vector x, vector y
//   Outputs    :   double slope
double FSHelper::slope(const vector<double>& x, const vector<double>& y){
    //    if(x.size() != y.size()){
    //        throw exception("...");
    //    }
    double n = x.size();

    double avgX = accumulate(x.begin(), x.end(), 0.0) / n;
    double avgY = accumulate(y.begin(), y.end(), 0.0) / n;

    double numerator = 0.0;
    double denominator = 0.0;

    for(int i=0; i<n; ++i){
        numerator += (x[i] - avgX) * (y[i] - avgY);
        denominator += (x[i] - avgX) * (x[i] - avgX);
    }

    //    if(denominator == 0){
    //        throw exception("...");
    //    }
    return numerator / denominator;
}


// ------------------------------------------------------------------------ //
// Reads data from csv file. Assumes a three column structure, in the order:
// time, pressure, and rate.
//   Inputs     :   QString filename, unsigned long skipRows
//   Outputs    :   QVector t, QVector p, QVector r
void FSHelper::readCsv(QString filename, unsigned long skipRows,
                       QVector<double>& t, QVector<double>& p,
                       QVector<double>& r)
{
    QString splitChar = ",";

    // Read data
    QFile file( filename );
    if(!file.open(QFile::ReadOnly | QFile::Text))
        return; // TODO: Generate error

    QTextStream in(&file);
    QStringList lines = in.readAll().split('\n');
    for (int i = skipRows; i < lines.size(); ++i){ // [2]
        auto row_string = lines.at(i).split(splitChar, QString::SkipEmptyParts);
        if(row_string.size() < 1) // TODO: skip if file is less than 3 columns
            continue;
        t.push_back(row_string[0].toDouble());
        p.push_back(row_string[1].toDouble());
        r.push_back(row_string[2].toDouble());
    }
}


/*
//MatrixXd DFITAnalysis :: readCsv(QString filename,  int skip_rows)
//{
//    // My own csv read function modified from:
//    // [1] https://gist.github.com/ialhashim/14db82926cdab2575508
//    // [2] http://doc.qt.io/qt-5/qstringlist.html

//    MatrixXd M;
//    QString splitChar = ",";

//    // Read data
//    QFile file( filename );
//    if(!file.open(QFile::ReadOnly | QFile::Text)) return M;
//    vector <vector<double>> mat;
//    QTextStream in(&file);
//    QStringList lines = in.readAll().split('\n');
//    //for( auto line : lines ){
//    //    auto row_string = line.split(splitChar, QString::SkipEmptyParts);
//    for (int i = skip_rows; i < lines.size(); ++i){ // [2]
//        auto row_string = lines.at(i).split(splitChar, QString::SkipEmptyParts);
//        if(row_string.size() < 1) continue;

//        std::vector<double> row;
//        for(auto token : row_string)
//            row.push_back(token.toDouble());
//        mat.push_back( row );
//    }

//    // Copy values
//    M = MatrixXd(mat.size(), mat.front().size());
//    for(size_t i = 0; i < mat.size(); i++){
//        for(size_t j = 0; j < mat.front().size(); j++){
//            M(i,j) = mat[i][j];
//        }
//    }
//    return M;
//}
*/


/****************************************************************************/
