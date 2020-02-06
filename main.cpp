#include "functions.h"


#define Pi 3.14159265358979323846

using namespace std;

using namespace Eigen;

int main()
{
    functions* MyFunc = new functions();

    ///Initializing parameters
    MyFunc->intialize();

    int nelx=MyFunc->getnelx(),nely=MyFunc->getnely();
    int n1,n2;
    double volfrac=MyFunc->getvolfrac(),penal=MyFunc->getpenal();

    ArrayXXd xold,x=MatrixXd::Constant(nely,nelx,volfrac),dc(nely,nelx);
    VectorXd Ue(8),U_dense;
    MatrixXd KE;
    int loop=0;
    double change=1.0,c=0.0;
    SpVec U;

    std::clock_t start;
    double duration;

    /// Start iteration
    while (change>0.01)
    {
        loop++;
        xold=x;

        start = std::clock();
        U_dense=MyFunc->FE_dense(x);
        duration = ( std::clock() - start ) / (double) CLOCKS_PER_SEC;
        cout<<"FE time "<<duration<<endl;
        KE=lk();

        /// Objective function and sensitivity analysis

        for (int ely=0; ely<nely; ++ely)
        {
            for (int elx=0; elx<nelx; ++elx)
            {
                n1=(nely+1)*elx+ely+1;
                n2=(nely+1)*(elx+1)+ely+1;

                /*Ue(0)=U.coeff(2*n1-2);
                Ue(1)=U.coeff(2*n1-1);
                Ue(2)=U.coeff(2*n2-2);
                Ue(3)=U.coeff(2*n2-1);
                Ue(4)=U.coeff(2*n2);
                Ue(5)=U.coeff(2*n2+1);
                Ue(6)=U.coeff(2*n1);
                Ue(7)=U.coeff(2*n1+1);*/

                Ue(0)=U_dense(2*n1-2);
                Ue(1)=U_dense(2*n1-1);
                Ue(2)=U_dense(2*n2-2);
                Ue(3)=U_dense(2*n2-1);
                Ue(4)=U_dense(2*n2);
                Ue(5)=U_dense(2*n2+1);
                Ue(6)=U_dense(2*n1);
                Ue(7)=U_dense(2*n1+1);

                //cout<<"Ue assigned"<<endl;
                //cout<<Ue<<endl;

                c+=pow(x(ely,elx),penal)*Ue.dot(KE*Ue);
                //cout<<"c computed"<<endl;
                dc(ely,elx)=-penal*pow(x(ely,elx),penal-1.0)*Ue.dot(KE*Ue);
            };
        };

        cout<<Ue<<endl;

        //cout<<"dc "<<dc.mean()<<endl;

        //cout<<"dc and c built"<<endl;
        /// filtering
        dc=MyFunc->check(x,dc);
        //cout<<"filtered"<<endl;
        ///design update by the OC method
        x=MyFunc->OC(x,dc);
        //cout<<"updated"<<endl;
        /// print results
        change=(x-xold).maxCoeff();
        cout<<"It.: "<<loop<<" Sol.: "<<U_dense.norm()<<" Obj.: "<<c<<" Filt.: "<<dc.mean()<<" Vol.: "<<x.mean()<<" ch.: "<<change<<endl;
    }

    delete MyFunc;

    return 0;
}
