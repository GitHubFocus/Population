#include "OperatorGradientMagnitudeGaussianImageGrid.h"

#include<DataImageGrid.h>
#include<DataNumber.h>
OperatorGradientGaussianMatN::OperatorGradientGaussianMatN()
    :COperator()
{
    this->path().push_back("Algorithm");
    this->path().push_back("Processing");
    this->path().push_back("Recursive");
    this->setKey("PopulationOperatorGradientGaussianImageGrid");
    this->setName("gradientMagnitudeGaussian");
    this->setInformation("h(x)=|grad(g,sigma)| with grad is the Gaussian's gradient with parameter sigma\n");
    this->structurePlug().addPlugIn(DataMatN::KEY,"f.pgm");
    this->structurePlug().addPlugIn(DataNumber::KEY,"sigma.pgm(by default 1)");
    this->structurePlug().addPlugOut(DataMatN::KEY,"h.pgm");
}








void OperatorGradientGaussianMatN::exec(){
    shared_ptr<BaseMatN> f1 = dynamic_cast<DataMatN *>(this->plugIn()[0]->getData())->getData();

    double sigma;
    if(this->plugIn()[1]->isDataAvailable()==true){
        sigma = dynamic_cast<DataNumber *>(this->plugIn()[1]->getData())->getValue();
    }
    else{
        sigma = 1;
    }

    BaseMatN * h;
    foo func;

    BaseMatN * fc1= f1.get();
    try{Dynamic2Static<TListImgGridScalarAndRGB>::Switch(func,fc1,sigma,h,Loki::Type2Type<MatN<2,int> >());}
    catch(pexception msg){
        this->error("Pixel/voxel type of input image must be registered type");
        return;
    }
    dynamic_cast<DataMatN *>(this->plugOut()[0]->getData())->setData(shared_ptr<BaseMatN>(h));
}

void OperatorGradientGaussianMatN::initState(){
    this->plugIn()[0]->setState(CPlug::EMPTY);
    if(this->plugIn()[1]->isConnected()==false)
        this->plugIn()[1]->setState(CPlug::OLD);
    else
        this->plugIn()[1]->setState(CPlug::EMPTY);
    this->plugOut()[0]->setState(CPlug::EMPTY);
}
COperator * OperatorGradientGaussianMatN::clone(){
    return new OperatorGradientGaussianMatN();
}
