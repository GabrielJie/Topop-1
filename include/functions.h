#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "Eigen/Dense"
#include "Eigen/Sparse"

#include <time.h>
#include <iostream>
#include <ctime>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <cstdlib>
#include <memory>
#include <thread>
#include <string>
#include <math.h>
#include <cmath>
#include <complex>

using namespace std;

using namespace Eigen;

typedef Eigen::SparseMatrix<double> SpMat;
typedef Eigen::SparseVector<double> SpVec;
typedef Triplet<double> T;

class functions
{
    public:
        functions();
        virtual ~functions();

        void setnelx();
        int getnelx();

        void setnely();
        int getnely();

        void setvolfrac();
        double getvolfrac();

        void setpenal();
        double getpenal();

        void setrmin();
        double getrmin();

        void intialize();

        void defbeam();

        ArrayXXd OC(const ArrayXXd &x,const ArrayXXd &dc);

        SpVec FE(const ArrayXXd &x);

        VectorXd FE_dense(const ArrayXXd &x);

        ArrayXXd check(const ArrayXXd &x,const ArrayXXd &dc);

    protected:

    private:

        int _nelx,_nely;
        double _volfrac,_penal,_rmin;
};

MatrixXd lk();

#endif // FUNCTIONS_H
