#include"Population.h"
using namespace pop;//Population namespace
int main(){
    {
        NeuralNet net;
        net.addLayerLinearInput(2);//2 scalar input
        net.addLayerLinearFullyConnected(5);// 1 fully connected layer with 3 neurons
        net.addLayerLinearFullyConnected(1);// 1 scalar output
        //create the training set
        // (-1,-1)->-1
        // ( 1,-1)-> 1
        // (-1, 1)-> 1
        // ( 1, 1)->-1
        Vec<VecF32> v_input(4,VecF32(2));//4 vector of two scalar values
        v_input(0)(0)=-1;v_input(0)(1)=-1; // (-1,-1)
        v_input(1)(0)= 1;v_input(1)(1)=-1; // ( 1,-1)
        v_input(2)(0)=-1;v_input(2)(1)= 1; // (-1, 1)
        v_input(3)(0)= 1;v_input(3)(1)= 1; // ( 1, 1)

        Vec<VecF32> v_output_expected(4,VecF32(1));//4 vector of one scalar value
        v_output_expected(0)(0)=-1;// -1
        v_output_expected(1)(0)= 1;//  1
        v_output_expected(2)(0)= 1;//  1
        v_output_expected(3)(0)=-1;// -1


        //use the backprogation algorithm with first order method

        net.setLearnableParameter(0.1);
        net.setTrainable(true);


        //random vector to shuffle the trraining set
        Vec<int> v_global_rand(v_input.size());
        for(unsigned int i=0;i<v_global_rand.size();i++)
            v_global_rand[i]=i;

        std::cout<<"iter_epoch\t error_train\t  learning rate"<<std::endl;
        unsigned int nbr_epoch=100;
        for(unsigned int i=0;i<nbr_epoch;i++){
            std::random_shuffle ( v_global_rand.begin(), v_global_rand.end() ,Distribution::irand());
            F32 error_training=0;
            for(unsigned int j=0;j<v_global_rand.size();j++){
                VecF32 vout;
                net.forwardCPU(v_input(v_global_rand[j]),vout);
                net.backwardCPU(v_output_expected(v_global_rand[j]));
                net.learn();
                error_training+=std::abs(v_output_expected(v_global_rand[j])(0)-vout(0));
            }
            std::cout<<i<<"\t"<<error_training<<"\t"<<std::endl;
        }
        //test the training
        for(int j=0;j<4;j++){
            VecF32 vout;
            net.forwardCPU(v_input(j),vout);
            std::cout<<vout(0)<<std::endl;// we obtain the expected value -1 , 1 , 1 , -1
        }

    }
    return 1;
    Mat2UI8 img;//2d grey-level image object
    img.load(POP_PROJECT_SOURCE_DIR+std::string("/image/iex.png"));//replace this path by those on your computer
    img = PDE::nonLinearAnisotropicDiffusion(img);//filtering
    int value;
    Mat2UI8 threshold = Processing::thresholdOtsuMethod(img,value);//threshold segmentation
    threshold.save("iexthreshold.pgm");
    Mat2RGBUI8 color = Visualization::labelForeground(threshold,img);//Visual validation
    color.display();
    return 0;
}


