#include "OperatorMinValueImageGrid.h"

#include<DataImageGrid.h>
#include<DataNumber.h>
OperatorMinValueMatN::OperatorMinValueMatN()
    :COperator()
{
    this->path().push_back("Algorithm");
    this->path().push_back("Analysis");
    this->path().push_back("Geometry");
    this->path().push_back("Scalar");
    this->setKey("PopulationOperatorMinValueImageGrid");
    this->setName("minValue");
    this->setInformation("min = min$_\\{x \\in E\\}$ h(x)\n");
    this->structurePlug().addPlugIn(DataMatN::KEY,"f.pgm");
    this->structurePlug().addPlugOut(DataNumber::KEY,"min.num");
}

void OperatorMinValueMatN::exec(){
    shared_ptr<BaseMatN> f1 = dynamic_cast<DataMatN *>(this->plugIn()[0]->getData())->getData();
    double value;
    foo func;
    BaseMatN * fc1= f1.get();
    try{Dynamic2Static<TListImgGridScalar>::Switch(func,fc1,value,Loki::Type2Type<MatN<2,int> >());}
    catch(pexception msg){
        this->error("Pixel/voxel type of input image must be registered type");
        return;
    }
    dynamic_cast<DataNumber *>(this->plugOut()[0]->getData())->setValue(value);
}

COperator * OperatorMinValueMatN::clone(){
    return new OperatorMinValueMatN();
}
