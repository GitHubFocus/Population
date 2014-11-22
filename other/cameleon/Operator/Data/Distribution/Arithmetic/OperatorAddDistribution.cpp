#include "OperatorAddDistribution.h"
#include<DataDistribution.h>
OperatorAddDistribution::OperatorAddDistribution()
    :COperator()
{

    this->path().push_back("Data");
    this->path().push_back("Distribution");
    this->path().push_back("Arithmetic");
    this->setKey("PopulationOperatorAddDistribution");
    this->setName("addition");
    this->setInformation("h=f+g\n");
    this->structurePlug().addPlugIn(DataDistribution::KEY,"f.dist");
    this->structurePlug().addPlugIn(DataDistribution::KEY,"g.dist");
    this->structurePlug().addPlugOut(DataDistribution::KEY,"h.dist");
}

void OperatorAddDistribution::exec(){
    Distribution f= dynamic_cast<DataDistribution *>(this->plugIn()[0]->getData())->getValue();
    Distribution g= dynamic_cast<DataDistribution *>(this->plugIn()[1]->getData())->getValue();
    f =f+g;
    dynamic_cast<DataDistribution *>(this->plugOut()[0]->getData())->setValue(f);
}

COperator * OperatorAddDistribution::clone(){
    return new OperatorAddDistribution();
}
