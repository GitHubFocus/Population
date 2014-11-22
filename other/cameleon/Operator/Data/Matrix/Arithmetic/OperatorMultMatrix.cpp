#include "OperatorMultMatrix.h"
#include<CData.h>
#include<DataMatrix.h>
OperatorMultMatrix::OperatorMultMatrix(){

    this->structurePlug().addPlugIn(DataMatrix::KEY,"A.m");
    this->structurePlug().addPlugIn(DataMatrix::KEY,"B.m");
    this->structurePlug().addPlugOut(DataMatrix::KEY,"C.m");
    this->path().push_back("Data");
    this->path().push_back("Matrix");
    this->path().push_back("Arithmetic");
    this->setKey("OperatorMultMatrix");
    this->setName("multiplication");
    this->setInformation("C=A*B");
}

void OperatorMultMatrix::exec(){
    shared_ptr<Mat2F64> m1 = dynamic_cast<DataMatrix*>(this->plugIn()[0]->getData())->getData();
    shared_ptr<Mat2F64> m2 = dynamic_cast<DataMatrix*>(this->plugIn()[1]->getData())->getData();
    try{
       Mat2F64* m = new  Mat2F64;
       *m = *(m1.get())* *(m2.get());
       dynamic_cast<DataMatrix*>(this->plugOut()[0]->getData())->setData(shared_ptr<Mat2F64>(m));
    }

    catch(pexception msg){
        this->error(msg.what());
    }
}



COperator * OperatorMultMatrix::clone(){
    return new OperatorMultMatrix();
}

