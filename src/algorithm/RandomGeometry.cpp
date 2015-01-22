#include"algorithm/RandomGeometry.h"
#include"data/mat/MatNDisplay.h"
namespace pop{


void  RandomGeometry::rhombohedron( ModelGermGrain3  & grain,const Distribution &distradius,const Distribution &distangle, const DistributionMultiVariate &distorientation )
{
    if(distorientation.getNbrVariable() !=3)
    {
        std::cerr<<"In RandomGeometry::rhombohedron, for d = 3, the angle distribution std::vector must have 3 variables with d the space dimension";
    }
    for( std::vector<Germ<3> * >::iterator it= grain.grains().begin();it!=grain.grains().end();it++ )
    {
        Germ<3> * g = (*it);
        GrainEquilateralRhombohedron * box = new GrainEquilateralRhombohedron();
        box->setGerm(*g);
        box->radius= distradius.randomVariable();
        box->setAnglePlane( distangle.randomVariable());
        VecF32 v = distorientation.randomVariable();
        for(int i=0;i<3;i++)
            box->orientation.setAngle_ei(v(i),i);
        (*it) = box;
        delete g;
    }
}
void RandomGeometry::cylinder( ModelGermGrain3  & grain,Distribution  distradius,Distribution distheight, const DistributionMultiVariate & distorientation )
{

    if(distorientation.getNbrVariable()!=3)
    {
        std::cerr<<"In RandomGeometry::cylinder, for d = 3, the angle distribution std::vector must have 3 variables with d the space dimension";
    }
    for( std::vector<Germ<3> * >::iterator it= grain.grains().begin();it!=grain.grains().end();it++ )
    {
        Germ<3> * g = (*it);
        GrainCylinder * box = new GrainCylinder();
        box->setGerm(*g);
        box->radius = distradius.randomVariable();
        box->height = distheight.randomVariable();
        VecF32 v = distorientation.randomVariable();
        for(int i=0;i<3;i++)
            box->orientation.setAngle_ei(v(i),i);
        (*it) = box;
        delete g;
    }
}

MatN<2,UI8 > RandomGeometry::diffusionLimitedAggregation2D(int size,int nbrwalkers){
    MatN<2,UI8 > in(size,size);
    in(VecN<2,F32>(in.getDomain())*0.5)=255;

    Distribution d(0,1,"UNIFORMINT");
    Distribution dface(0,MatN<2,UI8 >::DIM-1,"UNIFORMINT");

    Distribution dpos(0,in.sizeI()-1,"UNIFORMINT");

    MatN<2,UI8 >::IteratorENeighborhood N(in.getIteratorENeighborhood(1,0));

    MatNDisplay windows;

    for(int index_walker =0;index_walker<nbrwalkers;index_walker++){
        std::cout<<"walker "<<index_walker<<std::endl;
        MatN<2,UI8 >::E randomwalker;
        //GENERATE INIT POSITION
        int face= dface.randomVariable();
        int noindex= face/2;
        for(int j =0;j<MatN<2,UI8 >::DIM;j++){
            if(j==noindex){
                randomwalker(j)=(in.sizeI()-1)*(face%2);
            }
            else{
                randomwalker(j)=dpos.randomVariable();
            }
        }
        bool touch=false;
        while(touch==false){
            //MOUVEMENT
            MatN<2,UI8 >::E temp;
            do{
                temp=randomwalker;
                for(int i =0;i<MatN<2,UI8 >::DIM;i++){
                    temp(i)+=(d.randomVariable()*2-1);
                }
            }while(in.isValid(temp)==false);
            randomwalker = temp;
            N.init(randomwalker);
            while(N.next()==true){
                if(in(N.x())!=0)
                {
                    touch=true;
                    in(randomwalker)=255;
                    if(index_walker%50==0)
                        windows.display(in);
                }
            }

        }
    }
    return in;
}
MatN<3,UI8 > RandomGeometry::diffusionLimitedAggregation3D(int size,int nbrwalkers){
    MatN<3,UI8 > in(size,size,size);
    in(VecN<3,F32>(in.getDomain())*0.5)=255;

    Distribution d(0,1,"UNIFORMINT");
    Distribution dface(0,MatN<3,UI8 >::DIM-1,"UNIFORMINT");

    Distribution dpos(0,in.sizeI()-1,"UNIFORMINT");

    MatN<3,UI8 >::IteratorENeighborhood N(in.getIteratorENeighborhood(1,1));
    for(int index_walker =0;index_walker<nbrwalkers;index_walker++){
        std::cout<<"walker "<<index_walker<<std::endl;

        MatN<3,UI8 >::E randomwalker;
        //GENERATE INIT POSITION
        int face= dface.randomVariable();
        int noindex= face/2;
        for(int i =0;i<MatN<3,UI8 >::DIM;i++){
            if(i==noindex){
                randomwalker(i)=(in.sizeI()-1)*(face%2);
            }
            else{
                randomwalker(i)=dpos.randomVariable();
            }
        }
        bool touch=false;
        while(touch==false){
            //MOUVEMENT
            MatN<3,UI8 >::E temp;
            do{
                temp=randomwalker;
                for(int i =0;i<MatN<3,UI8 >::DIM;i++){
                    temp(i)+=(d.randomVariable()*2-1);
                }
            }while(in.isValid(temp)==false);
            randomwalker = temp;
            N.init(randomwalker);
            while(N.next()==true){
                if(in(N.x())!=0)
                {
                    touch=true;
                    in(randomwalker)=255;
                }
            }

        }
    }
    return in;
}
Distribution RandomGeometry::generateProbabilitySpectralDensity(const Mat2F32& correlation,F32 beta)
{
    Mat2F32 autocorrelation(correlation.sizeI() ,1);

    for(unsigned int i= 0; i<correlation.sizeI();i++)
    {
        autocorrelation(i,0)=correlation(i,1)-correlation(0,1)*correlation(0,1);

    }
    std::string s = BasicUtility::Any2String(beta);
    std::string  equation= "1/(2*pi)*1/((1-x^2)^(1./2))*exp(-("+s+")^(2)/(1+x))";
    Distribution f(equation.c_str());
    std::string  equation2= "1/(2*pi)*1/((1-x^2)^(1./2))*exp(-("+s+")^(2)/(1-x))";
    Distribution f2(equation2.c_str());
    Distribution fintegral = Statistics::integral(f,0,1,0.001);
    Distribution fintegral2 = Statistics::integral(f2,0,1,0.001);
    Mat2F32 g(correlation.sizeI() ,2);
    for(unsigned int i= 0; i<correlation.sizeI();i++) {
        g(i,0) = i;
        if(autocorrelation(i,0)>=0)
            g(i,1)=Statistics::FminusOneOfYMonotonicallyIncreasingFunction(fintegral,autocorrelation(i,0),0,1,0.001);
        else
            g(i,1)=-Statistics::FminusOneOfYMonotonicallyIncreasingFunction(fintegral2,-autocorrelation(i,0),0,1,0.001);
    }
    Distribution(g).display();
    F32 pi =3.14159265;
    F32 step2 =0.001;
    F32 maxrange=2;
    Mat2F32 rho_k(maxrange/step2,2);
    for(unsigned int i= 1; i<rho_k.sizeI();i++)
    {
        F32 sum=0;
        F32 k =step2*i;
        rho_k(i,0)=k;
        for(unsigned int r= 1; r<g.sizeI();r++)//(int)g.proba.size();r++)P(k,1)=
        {

            sum +=(1/(2*pi*pi))*r*r*g(r,1)*std::sin(k*r)/(k*r);
        }
        rho_k(i,1)=sum;
    }
    Distribution(rho_k).display();
    Mat2F32 P(rho_k.sizeI(),2);
    F32 sumnegative=0;

//  F32 min_value =0;
//    for(int k= 0; k<(int)rho_k.sizeI();k++)//(int)g.proba.size();r++)
//    {
//        P(k,0)=rho_k(k,0);
//        F32 value = rho_k(k,1)*4*pi*rho_k(k,0)*rho_k(k,0);//4*pi*k^2*rho(k)
//        P(k,1)= value;
//        min_value = std::min(min_value,P(k,1));
//    }
//    Distribution(P).display();
//    int index_neg_min=0, index_neg_max=0;
//    for(int k= 0; k<(int)rho_k.sizeI();k++)//(int)g.proba.size();r++)
//    {
//        P(k,1)+=-min_value;
//        if(P(k,1)<0){
//            sumnegative+=-P(k,1);
//            if(index_neg_min==0){
//                index_neg_min =k;
//            }
//        }else{
//            if(sumnegative!=0){
//                index_neg_max = k;
//                index_neg_min=index_neg_min-1;
//                while(sumnegative>0){
//                    F32 mini = std::min(P(index_neg_min,1),P(index_neg_max,1));
//                    if(mini<0.5*sumnegative){
//                        P(index_neg_min,1)-=0.5*sumnegative;
//                        P(index_neg_max,1)-=0.5*sumnegative;
//                        sumnegative=0;
//                    }else{
//                        P(index_neg_min,1)-=mini;
//                        P(index_neg_max,1)-=mini;
//                        sumnegative=sumnegative-2*mini;
//                        index_neg_max++;
//                        index_neg_min--;
//                    }
//                }
//                sumnegative=0;
//                index_neg_min=0;
//            }
//        }
//    }
//    Distribution(P).display();

    for(int k= 0; k<(int)rho_k.sizeI();k++)//(int)g.proba.size();r++)
    {
        P(k,0)=rho_k(k,0);
        F32 value = rho_k(k,1)*4*pi*rho_k(k,0)*rho_k(k,0);//4*pi*k^2*rho(k)

        if(value>0){
            if(sumnegative==0)
                P(k,1)= value;
            else{
                if(value<sumnegative){
                    P(k,1)=0;
                    sumnegative-=value;
                }else{
                    P(k,1)=value-sumnegative;
                    sumnegative=0;
                }

                //                int k1=k;
                //                F32 sum1=0;
                //                while(k1<(int)rho_k.sizeI()&&rho_k(k,1)>=0){
                //                    sum1+= rho_k(k1,1)*4*pi*rho_k(k1,0)*rho_k(k1,0);
                //                    k1++;
                //                }
                //                if(sum1>sumnegative)
                //                    factor=(sum1-sumnegative)/sum1;
                //                else
                //                {
                //                    factor = 0;
                //                }
                //                sumnegative=0;

            }

        }else
        {
            P(k,1)=0;
            sumnegative+=(-value);
        }
    }
    DistributionRegularStep Prob(P);
//    Prob.smoothGaussian(10);
    Prob = Statistics::toProbabilityDistribution(Prob,Prob.getXmin(),Prob.getXmax(),Prob.getStep());
//    Prob.display();
    return Prob;
}
}
