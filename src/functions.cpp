#include "functions.h"

functions::functions()
{
    //ctor
}

functions::~functions()
{
    //dtor
}

void functions::setnelx()
{
    std::cout<<"enter ratio of elements in the horizontal direction"<<endl;
    cin>>_nelx;
}

int functions::getnelx()
{
    return _nelx;
}

void functions::setnely()
{
    cout<<"enter ratio of elements in the vertical direction"<<endl;
    cin>>_nely;
}

int functions::getnely()
{
    return _nely;
}

void functions::setvolfrac()
{
    cout<<"enter volume fraction"<<endl;
    cin>>_volfrac;
}

double functions::getvolfrac()
{
    return _volfrac;
}

void functions::setpenal()
{
    cout<<"enter penalizing power"<<endl;
    cin>>_penal;
}

double functions::getpenal()
{
    return _penal;
}

void functions::setrmin()
{
    cout<<"enter filter size"<<endl;
    cin>>_rmin;
}

double functions::getrmin()
{
    return _rmin;
}

void functions::intialize()
{
    setnelx();
    setnely();
    setvolfrac();
    setpenal();
    setrmin();
}

ArrayXXd functions::OC(ArrayXXd &x,const double &dc)
{
    ArrayXXd xnew(_nely,_nelx);
    ArrayXXd xmin;
    double moov=0.2;
    double l1=0.0,l2=100000,lmid;

    while (l2-l1>1.e-4)
    {
        lmid=0.5*(l2+l1);
        xnew=(x+moov).min(x*sqrt(-dc/lmid));
        xnew=xnew.max(x-moov);
        xnew=xnew.max(0.001);
        if (xnew.sum()-_volfrac*_nelx*_nely>0)
        {
            l1=lmid;
        }
        else
        {
            l2=lmid;
        }

    }

    return xnew;
}
SpVec functions::FE()
{
    MatrixXd KE=lk();
    /* note : block writing operations for sparse matrices
    are not available with Eigen (unless the columns are contiguous) */
    MatrixXd K=MatrixXd::Zero(2*(_nelx+1)*(_nely+1),2*(_nelx+1)*(_nely+1));
    SpVec F(2*(_nelx+1)*(_nely+1)), U(2*(_nelx+1)*(_nely+1));
    int n1,n2, ind1,ind2;
    std::array<int,8> edof;
    std::vector<int> edof2;

    for (int ely=0; ely<_nely; ely++)
    {
        for (int elx=0; elx<_nelx; elx++)
        {
            n1=(_nely+1)*(elx-1)+ely;
            n2=(_nely+1)*elx+ely;
            edof[0]=2*n1-1;
            edof[1]=2*n1;
            edof[2]=2*n2-1;
            edof[3]=2*n2;
            edof[4]=2*n2+1;
            edof[5]=2*n2+2;
            edof[6]=2*n1+1;
            edof[7]=2*n1+2;

            for (int i=0; i<8; ++i)
            {
                edof2.push_back(edof[i]);
            };

            //cout<<edof.size()<<endl;
            //cout<<edof[0]<<endl;

            K(edof2,edof2)=K(edof2,edof2)+pow(x(ely,elx,_penal))*KE;

            /*for (int i=0; i<8; ++i)
            {
                for(int j=0; j<8; ++j)
                {
                    ind1=edof[i];
                    ind2=edof[j];
                    K.insert(ind1,ind2) = K(ind1,ind2)+pow(x(ely,elx),_penal)*KE(ind1,ind2);
                };
            };*/


        };
    };

    F.insert(2,1)=-1.0;

    return U;
}

MatrixXd lk()
{
    double E=1.0, nu=0.3;
    double k[8];
    k[0]=0.5-nu/6.0;
    k[1]=1.0/8.0+nu/8.0;
    k[2]=-0.25-nu/12.0;
    k[3]=-1.0/8.0+3.0*nu/8.0;
    k[4]=-0.25+nu/12.0;
    k[5]=-k[1];
    k[6]=nu/6.0;
    k[7]=-k[3];

    MatrixXd KE=k[0]*MatrixXd::Identity(8,8),KEt=KE;

    KE(0,1)=2.0*k[1];
    KE(0,2)=2.0*k[2];
    KE(0,3)=2.0*k[3];
    KE(0,4)=2.0*k[4];
    KE(0,5)=2.0*k[5];
    KE(0,6)=2.0*k[6];
    KE(0,7)=2.0*k[7];

    KE(1,2)=2.0*k[8];
    KE(1,3)=2.0*k[7];
    KE(1,4)=2.0*k[6];
    KE(1,5)=2.0*k[5];
    KE(1,6)=2.0*k[4];
    KE(1,7)=2.0*k[3];

    KE(2,3)=2.0*k[6];
    KE(2,4)=2.0*k[7];
    KE(2,5)=2.0*k[4];
    KE(2,6)=2.0*k[5];
    KE(2,7)=2.0*k[2];

    KE(3,4)=2.0*k[8];
    KE(3,5)=2.0*k[3];
    KE(3,6)=2.0*k[2];
    KE(3,7)=2.0*k[5];

    KE(4,5)=2.0*k[2];
    KE(4,6)=2.0*k[3];
    KE(4,7)=2.0*k[4];

    KE(5,6)=2.0*k[8];
    KE(5,7)=2.0*k[7];

    KE(6,7)=2.0*k[6];

    //matrix is symmetric
    KE+=KEt.transpose();
    KE*=0.5;
    KE*=E/(1.0-nu*nu);
    return KE;
}
