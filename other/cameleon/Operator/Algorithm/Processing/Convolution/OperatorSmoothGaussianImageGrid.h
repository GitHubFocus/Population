#ifndef OPERATORSMOOTHGAUSSIANMatN_H
#define OPERATORSMOOTHGAUSSIANMatN_H

#include"COperator.h"
#include"data/mat/MatN.h"
#include"algorithm/Processing.h"
using namespace pop;
class OperatorSmoothGaussianMatN : public COperator
{
public:
    OperatorSmoothGaussianMatN();
    void exec();
    COperator * clone();
    void initState();
    struct foo
    {
        template<int DIM,typename Type>
        void operator()(MatN<DIM,Type> * in1cast,double alpha, BaseMatN * &out)
        {
            MatN<DIM,Type>* outcast  =  new  MatN<DIM,Type>(in1cast->getDomain());
            * outcast = Processing::smoothGaussian(*in1cast,alpha);
            out=outcast;
        }
    };

};
#endif // OPERATORSMOOTHGAUSSIANMatN_H
