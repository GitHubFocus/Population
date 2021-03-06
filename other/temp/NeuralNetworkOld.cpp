
#define SIGMOID(x) (1.7159f*tanh(0.6666667f*x))
#define DSIGMOID(S) (0.6666667f/1.7159f*(1.7159f+(S))*(1.7159f-(S)))  // derivative of the sigmoid as a function of the sigmoid's output
///////////////////////////////////////////////////////////////////////
//
//  NeuralNetwork class definition

NeuralNetworkFeedForward::NeuralNetworkFeedForward()
    :_is_input_layer(false)
{
}
NeuralNetworkFeedForward::NeuralNetworkFeedForward(const NeuralNetworkFeedForward& n){
    *this = n;
}
NeuralNetworkFeedForward & NeuralNetworkFeedForward::operator=(const NeuralNetworkFeedForward& n){
    this->_label2string = n._label2string;
    for(unsigned int i=0;i<n._layers.size();i++){
        NNLayer * layer = n._layers[i];
        if(i==0)
        {
            if(layer->_type==NNLayer::INPUTMATRIX){
                NNLayerMatrix* inputlayer = dynamic_cast<NNLayerMatrix*>(layer);
                this->addInputLayerMatrix(inputlayer->_neurons_matrix[0].getDomain()(0),inputlayer->_neurons_matrix[0].getDomain()(1),inputlayer->_method,inputlayer->_normalization_value);
            }else{
                this->addInputLayer(static_cast<unsigned int>(layer->_neurons.size()));
            }
        }else{
            if(layer->_type==NNLayer::MATRIXCONVOLUTIONNAL){
                NNLayerMatrixConvolutionalPlusSubScaling* inputlayer = dynamic_cast<NNLayerMatrixConvolutionalPlusSubScaling*>(layer);

                this->addLayerConvolutionalPlusSubScaling(inputlayer->_neurons_matrix.getDomain(),inputlayer->_size_convolutution,inputlayer->_sub_scaling,-1);
                NNLayer* my_layer = *(this->_layers.rbegin());
                for(unsigned int index_w=0;index_w<inputlayer->_weights.size();index_w++){
                    NNWeight * weight = layer->_weights[index_w];
                    my_layer->_weights[index_w]->_Wn = weight->_Wn;
                }
            }else if(layer->_type==NNLayer::MATRIXCONVOLUTIONNAL2){
                NNLayerMatrixConvolutional* inputlayer = dynamic_cast<NNLayerMatrixConvolutional*>(layer);

                this->addLayerConvolutional(inputlayer->_neurons_matrix.getDomain(),inputlayer->_size_convolutution,-1);
                NNLayer* my_layer = *(this->_layers.rbegin());
                for(unsigned int index_w=0;index_w<inputlayer->_weights.size();index_w++){
                    NNWeight * weight = layer->_weights[index_w];
                    my_layer->_weights[index_w]->_Wn = weight->_Wn;
                }
            }else if(layer->_type==NNLayer::MATRIXSUBSALE){
                NNLayerMatrixSubScale* inputlayer = dynamic_cast<NNLayerMatrixSubScale*>(layer);

                this->addLayerSubScaling(inputlayer->_sub_scaling,-1);
                NNLayer* my_layer = *(this->_layers.rbegin());
                for(unsigned int index_w=0;index_w<inputlayer->_weights.size();index_w++){
                    NNWeight * weight = layer->_weights[index_w];
                    my_layer->_weights[index_w]->_Wn = weight->_Wn;
                }

            }else if(layer->_type==NNLayer::MATRIXMAXPOOLING){
                NNLayerMatrixMaxPooling* inputlayer = dynamic_cast<NNLayerMatrixMaxPooling*>(layer);
                this->addLayerMaxPooling(inputlayer->_sub_scaling);
            }
            else if(layer->_type==NNLayer::FULLYCONNECTED)
            {
                this->addLayerFullyConnected(static_cast<unsigned int>(layer->_neurons.size()),-1);
                NNLayer* my_layer = *(this->_layers.rbegin());
                for(unsigned int index_w=0;index_w<layer->_weights.size();index_w++){
                    NNWeight * weight = layer->_weights[index_w];
                    my_layer->_weights[index_w]->_Wn = weight->_Wn;
                }
            }
        }
    }
    return *this;
}


void NeuralNetworkFeedForward::propagateFront(const pop::VecF32& in , pop::VecF32 &out)
{
    // first layer is imput layer: directly set outputs of all of its neurons
    // to the input vector
    NNLayer & layerfirst = *(_layers[0]);
    POP_DbgAssert( in.size() == layerfirst._neurons.size() );
#if defined(HAVE_OPENMP)
#pragma omp parallel for
#endif
    for(int i_neuron=0;i_neuron<static_cast<int>(layerfirst._neurons.size());i_neuron++){
        NNNeuron *  neuron = (layerfirst._neurons[i_neuron]);
        neuron->_Xn =  in(i_neuron);
    }

    // propagate layer by layer
    for(unsigned int i_layer=1;i_layer<_layers.size();i_layer++){
        NNLayer & layer = *(_layers[i_layer]);
        layer.propagateFront();
    }

    // load up output vector with results
    NNLayer & layerout = *(_layers[_layers.size()-1]);
    out.resize(layerout._neurons.size());
#if defined(HAVE_OPENMP)
#pragma omp parallel for
#endif
    for( int i_neuron=0;i_neuron<static_cast<int>(layerout._neurons.size());i_neuron++){
        NNNeuron *  neuron = (layerout._neurons[i_neuron]);
        out(i_neuron) = neuron->_Xn;
    }
}
void NeuralNetworkFeedForward::propagateBackFirstDerivate(const pop::VecF32& desired_output)
{
    NNLayer & layerlast = *(_layers[_layers.size()-1]);
    POP_DbgAssert( desired_output.size() == layerlast._neurons.size() );
#if defined(HAVE_OPENMP)
#pragma omp parallel for
#endif
    for( int i_neuron=0;i_neuron<static_cast<int>(layerlast._neurons.size());i_neuron++){
        NNNeuron *  neuron = (layerlast._neurons[i_neuron]);
        neuron->_dErr_dXn = neuron->_Xn -  desired_output(i_neuron);
    }
    for(int i_layer=static_cast<int>(_layers.size())-1;i_layer>0;i_layer--){
        NNLayer * layer = (_layers[i_layer]);
        layer->propagateBackFirstDerivate();
    }
}
void NeuralNetworkFeedForward::propagateBackSecondDerivate()
{
    NNLayer & layerlast = *(_layers[_layers.size()-1]);
#if defined(HAVE_OPENMP)
#pragma omp parallel for
#endif
    for( int i_neuron=0;i_neuron<static_cast<int>(layerlast._neurons.size());i_neuron++){
        NNNeuron *  neuron = (layerlast._neurons[i_neuron]);
        neuron->_d2Err_dXn2 = 1;
    }

    for(unsigned int i_layer=static_cast<unsigned int>(_layers.size()-1);i_layer>0;i_layer--){
        NNLayer & layer = *(_layers[i_layer]);
        layer.propagateBackSecondDerivate();
    }
}

void NeuralNetworkFeedForward::learningFirstDerivate()
{
    for(unsigned int i_layer=static_cast<unsigned int>(_layers.size()-1);i_layer>0;i_layer--){
        NNLayer & layer = *(_layers[i_layer]);
        layer.learningFirstDerivate();
    }
}

void NeuralNetworkFeedForward::addInputLayer(unsigned int nbr_neuron){
    POP_DbgAssertMessage(_is_input_layer==false,"Add an input neuron before to add weighed layer");
    _is_input_layer = true;
    _layers.push_back( new NNLayer(nbr_neuron) );
    NNLayer & layer = *(_layers[_layers.size()-1]);
    layer._type = NNLayer::INPUT;
}

void NeuralNetworkFeedForward::addInputLayerMatrix(unsigned int height,unsigned int width,NNLayerMatrix::CenteringMethod method,NNLayerMatrix::NormalizationValue normalization){
    POP_DbgAssertMessage(_is_input_layer==false,"Add an input neuron before to add weighed layer");
    _is_input_layer = true;
    _layers.push_back( new NNLayerMatrix(1,height,width,method,normalization) );
    NNLayer & layer = *(_layers[_layers.size()-1]);
    layer._type = NNLayer::INPUTMATRIX;
}

void NeuralNetworkFeedForward::addLayerFullyConnected(unsigned int nbr_neuron,F32 standart_deviation_weight){

    NNLayer & layerprevious = *(_layers[_layers.size()-1]);
    _layers.push_back( new NNLayer(nbr_neuron) );
    NNLayer & layer = *(_layers[_layers.size()-1]);
    layer._type = NNLayer::FULLYCONNECTED;
    // This layer is a fully-connected layer
    // with nbr_neuron units.  Since it is fully-connected,
    // each of the nbr_neuron neurons in the layer
    // is connected to all nbr_neuron_previous_layer in
    // the previous layer.
    // So, there are nbr_neuron neurons and nbr_neuron*(nbr_neuron_previous_layer+1) weights
    //    pop::DistributionUniformInt d(0,1);


    //normalize tbe number inverse square root of the connection feeding into the nodes)
    pop::DistributionNormal d(0,standart_deviation_weight/std::sqrt(layerprevious._neurons.size()*1.f));
    unsigned int nbr_neuron_previous_layer = static_cast<unsigned int>(layerprevious._neurons.size());
    unsigned int nbr_weight = nbr_neuron*(nbr_neuron_previous_layer+1);
    for (unsigned int  i_weight=0; i_weight<nbr_weight; ++i_weight )
    {
        F32 initweight =0;
        if(standart_deviation_weight>0)
            initweight =  d.randomVariable();
        layer._weights.push_back(new NNWeight(initweight ) );
    }

    // Interconnections with previous layer: fully-connected

    unsigned int i_weight = 0;  // weights are not shared in this layer
    for (unsigned int  i_neuron=0; i_neuron<nbr_neuron; ++i_neuron )
    {
        NNNeuron * n = (layer._neurons[ i_neuron ]) ;
        n->addConnectionBiais(layer._weights[i_weight] );  // bias weight
        i_weight++;
        for (unsigned int  i_neuron_previous_layer=0; i_neuron_previous_layer<nbr_neuron_previous_layer; ++i_neuron_previous_layer )
        {
            NNNeuron * nprevious = layerprevious._neurons[ i_neuron_previous_layer ] ;
            n->addConnection(layer._weights[i_weight], nprevious );
            i_weight++;
        }
    }


}
void NeuralNetworkFeedForward::addLayerConvolutional( unsigned int nbr_map, unsigned int kernelsize,F32 standart_deviation_weight){
    if(NNLayerMatrix * layerprevious =dynamic_cast<NNLayerMatrix *>(_layers[_layers.size()-1])){
        unsigned int height_previous = layerprevious->_neurons_matrix(0).getDomain()(0);
        unsigned int width_previous  = layerprevious->_neurons_matrix(0).getDomain()(1);


        unsigned int height = (height_previous-(kernelsize-1));
        unsigned int width  = (width_previous-(kernelsize-1));
        NNLayerMatrixConvolutional * layer = new NNLayerMatrixConvolutional(nbr_map,height,width);
        layer->_size_convolutution = kernelsize;

        layer->_type = NNLayer::MATRIXCONVOLUTIONNAL2;
        _layers.push_back(layer);

        // This layer is a convolutional connected layer
        // with nbr_neuron units.
        // (kernelsize*kernelsize+1)* nbr_map_previous_layer*nbr_map_layer
        unsigned int nbr_map_previous = static_cast<unsigned int>(layerprevious->_neurons_matrix.size());


        //normalize tbe number inverse square root of the connection feeding into the nodes)
        pop::DistributionNormal d(0,standart_deviation_weight/kernelsize);

        unsigned int nbr_weight = nbr_map*nbr_map_previous*(kernelsize*kernelsize+1);
        for (unsigned int  i_weight=0; i_weight<nbr_weight; ++i_weight )
        {
            F32 initweight =0;
            if(standart_deviation_weight>0)
                initweight =  d.randomVariable();
            layer->_weights.push_back(new NNWeight(initweight ) );
        }



        for(unsigned int i_width=0;i_width<width;i_width++)
        {
            for(unsigned int i_height=0;i_height<height;i_height++)
            {
                for(unsigned int i_pattern_previous = 0;i_pattern_previous<nbr_map_previous;i_pattern_previous++)
                {
                    for(unsigned int i_pattern = 0;i_pattern<nbr_map;i_pattern++)
                    {

                        NNNeuron * n = layer->_neurons_matrix(i_pattern)(i_height,i_width) ;
                        unsigned int i_weight_begin = i_pattern*(kernelsize*kernelsize+1)+i_pattern_previous*nbr_map*(kernelsize*kernelsize+1);
                        n->addConnectionBiais(layer->_weights[i_weight_begin]);
                        for(unsigned int i_heigh_kernel=0;i_heigh_kernel<kernelsize;i_heigh_kernel++)
                        {
                            for(unsigned int i_width_kernel=0;i_width_kernel<kernelsize;i_width_kernel++)
                            {

                                int i_width_previous =  i_width + i_width_kernel ;
                                int i_height_previous =  i_height+ i_heigh_kernel;

                                NNNeuron * nprevious = layerprevious->_neurons_matrix(i_pattern_previous)(i_height_previous,i_width_previous) ;
                                unsigned int i_weight = 1+i_width_kernel + i_heigh_kernel*kernelsize + i_weight_begin;
                                n->addConnection(layer->_weights[i_weight], nprevious );
                            }

                        }
                    }

                }

            }
        }
    }
}
void NeuralNetworkFeedForward::addLayerIntegral(unsigned int nbr_integral,F32 standart_deviation_weight){
    if(NNLayerMatrix * layerprevious =dynamic_cast<NNLayerMatrix *>(_layers[_layers.size()-1])){
        unsigned int height_previous = layerprevious->_neurons_matrix(0).getDomain()(0);
        unsigned int width_previous  = layerprevious->_neurons_matrix(0).getDomain()(1);
        unsigned int nbr_map_previous = static_cast<unsigned int>(layerprevious->_neurons_matrix.size());



        NNLayer * layer = new NNLayer(nbr_integral*nbr_map_previous);



        _layers.push_back(layer);

        //normalize tbe number inverse square root of the connection feeding into the nodes)
        pop::DistributionNormal d(0,standart_deviation_weight/std::sqrt(height_previous*width_previous*1.f));

        unsigned int nbr_weight = nbr_integral*(height_previous*width_previous+1);
        for (unsigned int  i_weight=0; i_weight<nbr_weight; ++i_weight )
        {
            F32 initweight =0;
            if(standart_deviation_weight>0)
                initweight =  d.randomVariable();
            layer->_weights.push_back(new NNWeight(initweight ) );
        }

        for(unsigned int i_integral = 0;i_integral<nbr_integral;i_integral++)
        {
            for(unsigned int i_pattern_previous = 0;i_pattern_previous<nbr_map_previous;i_pattern_previous++)
            {
                unsigned int i_weight_begin = i_integral*(height_previous*width_previous+1);
                NNNeuron * n = (layer->_neurons[i_pattern_previous+i_integral*nbr_map_previous ]) ;
                n->addConnectionBiais(layer->_weights[i_weight_begin]);
                for(unsigned int i_height=0;i_height<height_previous;i_height++)
                {
                    for(unsigned int i_width=0;i_width<width_previous;i_width++)
                    {
                        NNNeuron * nprevious = (layerprevious->_neurons_matrix(i_pattern_previous)(i_height,i_width)) ;

                        unsigned int i_weight = 1+i_width + i_height*width_previous + i_weight_begin;
                        n->addConnection(layer->_weights[i_weight], nprevious );
                    }

                }
            }

        }

    }
}

void NeuralNetworkFeedForward::addLayerSubScaling(unsigned int sub_scale_factor,F32 standart_deviation_weight){
    if(NNLayerMatrix * layerprevious =dynamic_cast<NNLayerMatrix *>(_layers[_layers.size()-1])){
        unsigned int height_previous = layerprevious->_neurons_matrix(0).getDomain()(0);
        unsigned int width_previous  = layerprevious->_neurons_matrix(0).getDomain()(1);
        unsigned int nbr_map_previous = static_cast<unsigned int>(layerprevious->_neurons_matrix.size());

        unsigned int height = (height_previous/sub_scale_factor);
        unsigned int width  = (width_previous/sub_scale_factor);

        NNLayerMatrixSubScale * layer = new NNLayerMatrixSubScale(nbr_map_previous,height,width);
        layer->_sub_scaling = sub_scale_factor;

        layer->_type = NNLayer::MATRIXSUBSALE;
        _layers.push_back(layer);




        //normalize tbe number inverse square root of the connection feeding into the nodes)
        pop::DistributionNormal d(0,standart_deviation_weight/sub_scale_factor);

        unsigned int nbr_weight = nbr_map_previous*height*width*(sub_scale_factor*sub_scale_factor+1);
        for (unsigned int  i_weight=0; i_weight<nbr_weight; ++i_weight )
        {
            F32 initweight =0;
            if(standart_deviation_weight>0)
                initweight =  d.randomVariable();
            layer->_weights.push_back(new NNWeight(initweight ) );
        }




        for(unsigned int i_pattern_previous = 0;i_pattern_previous<nbr_map_previous;i_pattern_previous++)
        {
            for(unsigned int i_height=0;i_height<height;i_height++)
            {
                for(unsigned int i_width=0;i_width<width;i_width++)
                {

                    NNNeuron * n = (layer->_neurons_matrix(i_pattern_previous)(i_height,i_width)) ;
                    unsigned int i_weight_begin =i_pattern_previous*width*height*(sub_scale_factor*sub_scale_factor+1) +  (i_width + i_height*width)*(sub_scale_factor*sub_scale_factor+1) ;
                    n->addConnectionBiais(layer->_weights[i_weight_begin]);
                    for(unsigned int i_heigh_kernel=0;i_heigh_kernel<sub_scale_factor;i_heigh_kernel++)
                    {
                        for(unsigned int i_width_kernel=0;i_width_kernel<sub_scale_factor;i_width_kernel++)
                        {
                            int i_width_previous =  i_width*sub_scale_factor + i_width_kernel ;
                            int i_height_previous = i_height*sub_scale_factor+ i_heigh_kernel;

                            NNNeuron * nprevious = (layerprevious->_neurons_matrix(i_pattern_previous)(i_height_previous,i_width_previous)) ;
                            unsigned int i_weight =i_weight_begin+     1+i_width_kernel + i_heigh_kernel*sub_scale_factor ;
                            n->addConnection(layer->_weights[i_weight], nprevious );
                        }

                    }
                }

            }

        }
    }
}
void NeuralNetworkFeedForward::addLayerMaxPooling(unsigned int sub_scale_factor){
    if(NNLayerMatrix * layerprevious =dynamic_cast<NNLayerMatrix *>(_layers[_layers.size()-1])){
        unsigned int height_previous = layerprevious->_neurons_matrix(0).getDomain()(0);
        unsigned int width_previous  = layerprevious->_neurons_matrix(0).getDomain()(1);
        unsigned int nbr_map_previous = static_cast<unsigned int>(layerprevious->_neurons_matrix.size());

        unsigned int height = (height_previous/sub_scale_factor);
        unsigned int width  = (width_previous/sub_scale_factor);

        NNLayerMatrixMaxPooling * layer = new NNLayerMatrixMaxPooling(nbr_map_previous,height,width);
        layer->_sub_scaling = sub_scale_factor;

        layer->_type = NNLayer::MATRIXMAXPOOLING;
        _layers.push_back(layer);




        for(unsigned int i_pattern_previous = 0;i_pattern_previous<nbr_map_previous;i_pattern_previous++)
        {
            for(unsigned int i_height=0;i_height<height;i_height++)
            {
                for(unsigned int i_width=0;i_width<width;i_width++)
                {

                    NNNeuronMaxPool * n = dynamic_cast<NNNeuronMaxPool*>((layer->_neurons_matrix(i_pattern_previous)(i_height,i_width))) ;

                    for(unsigned int i_heigh_kernel=0;i_heigh_kernel<sub_scale_factor;i_heigh_kernel++)
                    {
                        for(unsigned int i_width_kernel=0;i_width_kernel<sub_scale_factor;i_width_kernel++)
                        {
                            int i_width_previous =  i_width*sub_scale_factor + i_width_kernel ;
                            int i_height_previous = i_height*sub_scale_factor+ i_heigh_kernel;
                            NNNeuron * nprevious = (layerprevious->_neurons_matrix(i_pattern_previous)(i_height_previous,i_width_previous)) ;
                            n->addConnection( nprevious );
                        }

                    }
                }

            }

        }
    }
}

void NeuralNetworkFeedForward::addLayerConvolutionalPlusSubScaling( unsigned int nbr_map, unsigned int kernelsize,unsigned int sub_scale_sampling,F32 standart_deviation_weight){
    if(NNLayerMatrix * layerprevious =dynamic_cast<NNLayerMatrix *>(_layers[_layers.size()-1])){
        unsigned int height_previous = layerprevious->_neurons_matrix(0).getDomain()(0);
        unsigned int width_previous  = layerprevious->_neurons_matrix(0).getDomain()(1);

        if ((height_previous-(kernelsize-sub_scale_sampling)) % sub_scale_sampling!=0) {
            std::cerr<<"The height of the input matrix must be even "<<std::endl;
        }

        unsigned int height = (height_previous-(kernelsize-sub_scale_sampling))/sub_scale_sampling;
        unsigned int width  = (width_previous-(kernelsize-sub_scale_sampling))/sub_scale_sampling;

        NNLayerMatrixConvolutionalPlusSubScaling * layer = new NNLayerMatrixConvolutionalPlusSubScaling(nbr_map,height,width);
        layer->_size_convolutution = kernelsize;
        layer->_sub_scaling = sub_scale_sampling;
        layer->_type = NNLayer::MATRIXCONVOLUTIONNAL;
        _layers.push_back(layer);

        // This layer is a convolutional connected layer
        // with nbr_neuron units.
        // (kernelsize*kernelsize+1)* nbr_map_previous_layer*nbr_map_layer
        unsigned int nbr_map_previous = static_cast<unsigned int>(layerprevious->_neurons_matrix.size());

        //normalize tbe number inverse square root of the connection feeding into the nodes)
        pop::DistributionNormal d(0,standart_deviation_weight/kernelsize);

        unsigned int nbr_weight = nbr_map*nbr_map_previous*(kernelsize*kernelsize+1);
        for (unsigned int  i_weight=0; i_weight<nbr_weight; ++i_weight )
        {
            F32 initweight =0;
            if(standart_deviation_weight>0)
                initweight =  d.randomVariable();
            layer->_weights.push_back(new NNWeight(initweight ) );
        }

        for(unsigned int i_width=0;i_width<width;i_width++)
        {
            for(unsigned int i_height=0;i_height<height;i_height++)
            {
                for(unsigned int i_pattern = 0;i_pattern<nbr_map;i_pattern++)
                {
                    NNNeuron * n = (layer->_neurons_matrix(i_pattern)(i_height,i_width));
                    for(unsigned int i_pattern_previous = 0;i_pattern_previous<nbr_map_previous;i_pattern_previous++)
                    {
                        unsigned int i_weight = i_pattern*(kernelsize*kernelsize+1)+i_pattern_previous*nbr_map*(kernelsize*kernelsize+1);
                        n->addConnectionBiais(layer->_weights[i_weight]);
                        for(unsigned int i_width_kernel=0;i_width_kernel<kernelsize;i_width_kernel++)
                        {
                            for(unsigned int i_heigh_kernel=0;i_heigh_kernel<kernelsize;i_heigh_kernel++)
                            {
                                int i_width_previous =  i_width*2 + i_width_kernel ;
                                int i_height_previous =  i_height*2 + i_heigh_kernel ;

                                NNNeuron * nprevious = (layerprevious->_neurons_matrix(i_pattern_previous)(i_height_previous,i_width_previous)) ;
                                unsigned int i_weight_index = 1+i_width_kernel + i_heigh_kernel*kernelsize + i_pattern*(kernelsize*kernelsize+1)+i_pattern_previous*nbr_map*(kernelsize*kernelsize+1);
                                n->addConnection(layer->_weights[i_weight_index], nprevious );
                            }
                        }
                    }
                }
            }
        }
    }
}
NeuralNetworkFeedForward::~NeuralNetworkFeedForward()
{
    init();
}

void NeuralNetworkFeedForward::init(){
    for(unsigned int i=0;i<_layers.size();i++){
        delete _layers[i];
    }
    _layers.clear();
}
void NeuralNetworkFeedForward::setLearningRate(F32 eta)
{
    for(unsigned int i_layer=0;i_layer<_layers.size();i_layer++)
    {
        NNLayer & layer = *(_layers[i_layer]);
        layer.setLearningRate(eta);
    }
}

void NeuralNetworkFeedForward::save(const char * file)const
{
    XMLDocument doc;

    XMLNode node1 = doc.addChild("label2String");
    node1.addAttribute("id",BasicUtility::Any2String(_label2string));
    XMLNode node = doc.addChild("layers");
    for(unsigned int i=0;i<this->_layers.size();i++){
        NNLayer * layer = this->_layers[i];
        if(i==0)
        {
            if(layer->_type==NNLayer::INPUTMATRIX){
                NNLayerMatrix* inputlayer = dynamic_cast<NNLayerMatrix*>(layer);
                XMLNode nodechild = node.addChild("layer");
                nodechild.addAttribute("type","NNLayer::INPUTMATRIX");
                nodechild.addAttribute("size",BasicUtility::Any2String(inputlayer->_neurons_matrix[0].getDomain()));
                nodechild.addAttribute("method",BasicUtility::Any2String((unsigned int)inputlayer->_method));
                nodechild.addAttribute("normalization",BasicUtility::Any2String((unsigned int)inputlayer->_normalization_value));
            }else{

                XMLNode nodechild = node.addChild("layer");
                nodechild.addAttribute("type","NNLayer::INPUT");
                nodechild.addAttribute("size",BasicUtility::Any2String(layer->_neurons.size()));
            }
        }else{
            if(layer->_type==NNLayer::MATRIXCONVOLUTIONNAL){
                NNLayerMatrixConvolutionalPlusSubScaling* inputlayer = dynamic_cast<NNLayerMatrixConvolutionalPlusSubScaling*>(layer);
                XMLNode nodechild = node.addChild("layer");
                nodechild.addAttribute("type","NNLayer::MATRIXCONVOLUTIONNAL");
                nodechild.addAttribute("nbrpattern",BasicUtility::Any2String(inputlayer->_neurons_matrix.getDomain()));
                nodechild.addAttribute("sizekernel",BasicUtility::Any2String(inputlayer->_size_convolutution));
                nodechild.addAttribute("subsampling",BasicUtility::Any2String(inputlayer->_sub_scaling));

                std::string weight_str;
                for(unsigned int index_w=0;index_w<inputlayer->_weights.size();index_w++){
                    NNWeight * weight = inputlayer->_weights[index_w];
                    weight_str+=BasicUtility::Any2String(weight->_Wn)+";";
                }
                nodechild.addAttribute("weight",weight_str);
            }else if(layer->_type==NNLayer::MATRIXCONVOLUTIONNAL2){
                NNLayerMatrixConvolutional* inputlayer = dynamic_cast<NNLayerMatrixConvolutional*>(layer);
                XMLNode nodechild = node.addChild("layer");
                nodechild.addAttribute("type","NNLayer::MATRIXCONVOLUTIONNAL2");
                nodechild.addAttribute("nbrpattern",BasicUtility::Any2String(inputlayer->_neurons_matrix.getDomain()));
                nodechild.addAttribute("sizekernel",BasicUtility::Any2String(inputlayer->_size_convolutution));

                std::string weight_str;
                for(unsigned int index_w=0;index_w<inputlayer->_weights.size();index_w++){
                    NNWeight * weight = inputlayer->_weights[index_w];
                    weight_str+=BasicUtility::Any2String(weight->_Wn)+";";
                }
                nodechild.addAttribute("weight",weight_str);
            }else if(layer->_type==NNLayer::MATRIXSUBSALE){
                NNLayerMatrixSubScale* inputlayer = dynamic_cast<NNLayerMatrixSubScale*>(layer);
                XMLNode nodechild = node.addChild("layer");
                nodechild.addAttribute("type","NNLayer::MATRIXSUBSALE");
                nodechild.addAttribute("nbrpattern",BasicUtility::Any2String(inputlayer->_neurons_matrix.getDomain()));
                nodechild.addAttribute("subsampling",BasicUtility::Any2String(inputlayer->_sub_scaling));
                std::string weight_str;
                for(unsigned int index_w=0;index_w<inputlayer->_weights.size();index_w++){
                    NNWeight * weight = inputlayer->_weights[index_w];
                    weight_str+=BasicUtility::Any2String(weight->_Wn)+";";
                }
                nodechild.addAttribute("weight",weight_str);
            }else if(layer->_type==NNLayer::MATRIXMAXPOOLING){
                NNLayerMatrixMaxPooling* inputlayer = dynamic_cast<NNLayerMatrixMaxPooling*>(layer);
                XMLNode nodechild = node.addChild("layer");
                nodechild.addAttribute("type","NNLayer::MATRIXMAXPOOLING");
                nodechild.addAttribute("nbrpattern",BasicUtility::Any2String(inputlayer->_neurons_matrix.getDomain()));
                nodechild.addAttribute("subsampling",BasicUtility::Any2String(inputlayer->_sub_scaling));
            }
            else if(layer->_type==NNLayer::FULLYCONNECTED)
            {
                XMLNode nodechild = node.addChild("layer");
                nodechild.addAttribute("type","NNLayer::FULLYCONNECTED");
                nodechild.addAttribute("size",BasicUtility::Any2String(layer->_neurons.size()));

                std::string weight_str;
                for(unsigned int index_w=0;index_w<layer->_weights.size();index_w++){
                    NNWeight * weight = layer->_weights[index_w];
                    weight_str+=BasicUtility::Any2String(weight->_Wn)+";";
                }
                nodechild.addAttribute("weight",weight_str);
            }
        }
    }
    doc.save(file);

}

void NeuralNetworkFeedForward::load(const char * file)
{
    XMLDocument doc;
    doc.load(file);
    load(doc);
}
void NeuralNetworkFeedForward::loadByteArray(const char *  file)
{
    XMLDocument doc;
    doc.loadFromByteArray(file);
    load(doc);

}

void NeuralNetworkFeedForward::load(XMLDocument &doc)
{
    //to circumvent our locales problem in String2Float()
    bool use_optimized_string2float = true;
    std::string sf = "0.1234";
    F32 f1, f2;
    pop::BasicUtility::String2Any(sf, f1);
    pop::BasicUtility::String2Float(sf, f2);
    if (f1 != f2) {
        use_optimized_string2float = false;
    }

    this->init();
    XMLNode node1 = doc.getChild("label2String");
    std::string type1 = node1.getAttribute("id");
    BasicUtility::String2Any(type1,_label2string);
    XMLNode node = doc.getChild("layers");
    int i=0;
    for (XMLNode tool = node.firstChild(); tool; tool = tool.nextSibling(),++i)
    {
        std::string type = tool.getAttribute("type");
        if(i==0)
        {
            if(type=="NNLayer::INPUTMATRIX"){
                Vec2I32 domain;
                BasicUtility::String2Any(tool.getAttribute("size"),domain);

                NNLayerMatrix::CenteringMethod method_enum=NNLayerMatrix::BoundingBox;
                if(tool.hasAttribute("method")){
                    int method;
                    //                    std::cout<<tool.getAttribute("method")<<std::endl;
                    BasicUtility::String2Any(tool.getAttribute("method"),method);
                    method_enum = static_cast<NNLayerMatrix::CenteringMethod>(method) ;
                }

                NNLayerMatrix::NormalizationValue method_norm_enum=NNLayerMatrix::MinusOneToOne;
                if(tool.hasAttribute("normalization")){
                    int method_norm;
                    //                    std::cout<<tool.getAttribute("normalization")<<std::endl;
                    BasicUtility::String2Any(tool.getAttribute("normalization"),method_norm);
                    method_norm_enum= static_cast<NNLayerMatrix::NormalizationValue>(method_norm) ;
                }

                this->addInputLayerMatrix(domain(0),domain(1),method_enum,method_norm_enum);

            }else{
                int domain;
                BasicUtility::String2Any(tool.getAttribute("size"),domain);
                this->addInputLayer(domain);
            }
        }
        else
        {
            if(type=="NNLayer::MATRIXCONVOLUTIONNAL"){

                std::string str = tool.getAttribute("nbrpattern");
                int nbr_map;
                BasicUtility::String2Any(str,nbr_map);

                str = tool.getAttribute("sizekernel");
                int sizekernel;
                BasicUtility::String2Any(str,sizekernel);

                str = tool.getAttribute("subsampling");
                int subsampling;
                BasicUtility::String2Any(str,subsampling);

                this->addLayerConvolutionalPlusSubScaling(nbr_map,sizekernel,subsampling,-1);

                str = tool.getAttribute("weight");
                NNLayer* layer = *(this->_layers.rbegin());
                std::istringstream stream(str);
                for(unsigned int index_weight=0;index_weight<layer->_weights.size();index_weight++){
                    F32 weight ;
                    str = pop::BasicUtility::getline( stream, ";" );
                    (use_optimized_string2float ? pop::BasicUtility::String2Float(str, weight) : pop::BasicUtility::String2Any(str, weight));
                    layer->_weights[index_weight]->_Wn = weight;
                }
            }else if(type=="NNLayer::MATRIXCONVOLUTIONNAL2"){

                std::string str = tool.getAttribute("nbrpattern");
                int nbr_map;
                BasicUtility::String2Any(str,nbr_map);

                str = tool.getAttribute("sizekernel");
                int sizekernel;
                BasicUtility::String2Any(str,sizekernel);

                this->addLayerConvolutional(nbr_map,sizekernel,-1);

                str = tool.getAttribute("weight");
                NNLayer* layer = *(this->_layers.rbegin());
                std::istringstream stream(str);
                for(unsigned int index_weight=0;index_weight<layer->_weights.size();index_weight++){
                    F32 weight ;
                    str = pop::BasicUtility::getline( stream, ";" );
                    (use_optimized_string2float ? pop::BasicUtility::String2Float(str, weight) : pop::BasicUtility::String2Any(str, weight));
                    layer->_weights[index_weight]->_Wn = weight;
                }
            }else if(type=="NNLayer::MATRIXSUBSALE"){

                std::string str = tool.getAttribute("nbrpattern");
                int nbr_map;
                BasicUtility::String2Any(str,nbr_map);


                str = tool.getAttribute("subsampling");
                int subsampling;
                BasicUtility::String2Any(str,subsampling);

                this->addLayerSubScaling(subsampling,-1);

                str = tool.getAttribute("weight");
                NNLayer* layer = *(this->_layers.rbegin());
                std::istringstream stream(str);
                for(unsigned int index_weight=0;index_weight<layer->_weights.size();index_weight++){
                    F32 weight ;
                    str = pop::BasicUtility::getline( stream, ";" );
                    (use_optimized_string2float ? pop::BasicUtility::String2Float(str, weight) : pop::BasicUtility::String2Any(str, weight));
                    layer->_weights[index_weight]->_Wn = weight;
                }
            }else if(type=="NNLayer::MATRIXMAXPOOLING"){

                std::string str = tool.getAttribute("nbrpattern");
                int nbr_map;
                BasicUtility::String2Any(str,nbr_map);
                str = tool.getAttribute("subsampling");
                int subsampling;
                BasicUtility::String2Any(str,subsampling);
                this->addLayerMaxPooling(subsampling);
            }
            else if(type=="NNLayer::FULLYCONNECTED")
            {
                std::string str = tool.getAttribute("size");
                int size;
                BasicUtility::String2Any(str,size);
                this->addLayerFullyConnected(size,-1);

                str = tool.getAttribute("weight");
                NNLayer* layer = *(this->_layers.rbegin());
                std::istringstream stream(str);
                for(unsigned int index_weight=0;index_weight<layer->_weights.size();index_weight++){
                    F32 weight ;
                    str = pop::BasicUtility::getline( stream, ";" );
                    (use_optimized_string2float ? pop::BasicUtility::String2Float(str, weight) : pop::BasicUtility::String2Any(str, weight));
                    layer->_weights[index_weight]->_Wn = weight;
                }
            }
        }
    }
}


///////////////////////////////////////////////////////////////////////
//
//  NNLayerMatrix class definition


NNLayerMatrix::NNLayerMatrix(unsigned int nbr_map,unsigned int height,unsigned int width ,NNLayerMatrix::CenteringMethod method,NormalizationValue normalization)
    :NNLayer(nbr_map*height*width)
{
    _method = method;
    _normalization_value = normalization;
    _neurons_matrix.resize(nbr_map,MatN<2,NNNeuron*>(Vec2I32(height,width)));
    for (unsigned int i_pattern=0; i_pattern<nbr_map; ++i_pattern )
    {

        for (unsigned int i_width=0; i_width<width; ++i_width )
        {
            for (unsigned int i_height=0; i_height<height; ++i_height )
            {
                _neurons_matrix(i_pattern)(i_height,i_width)=this->_neurons[i_width + i_height*width+i_pattern*width*height ];
            }
        }
    }
}
NNLayerMatrixConvolutionalPlusSubScaling::NNLayerMatrixConvolutionalPlusSubScaling(unsigned int nbr_map,unsigned int height,unsigned int width )
    :NNLayerMatrix(nbr_map,height, width )
{
}

NNLayerMatrixConvolutional::NNLayerMatrixConvolutional(unsigned int nbr_map,unsigned int height,unsigned int width )
    :NNLayerMatrix(nbr_map,height, width )
{
}
NNLayerMatrixSubScale::NNLayerMatrixSubScale(unsigned int nbr_map,unsigned int height,unsigned int width )
    :NNLayerMatrix(nbr_map,height, width )
{
}



NNLayerMatrixMaxPooling::NNLayerMatrixMaxPooling(unsigned int nbr_map,unsigned int height,unsigned int width )
    :NNLayerMatrix(0,0,0)
{
    for (unsigned int i_index=0; i_index<nbr_map*height*width; ++i_index ){
        this->_neurons.push_back( new NNNeuronMaxPool() );
    }
    this->_neurons_matrix.resize(nbr_map,MatN<2,NNNeuron*>(Vec2I32(height,width)));
    for (unsigned int i_pattern=0; i_pattern<nbr_map; ++i_pattern )
    {

        for (unsigned int i_width=0; i_width<width; ++i_width )
        {
            for (unsigned int i_height=0; i_height<height; ++i_height )
            {
                this->_neurons_matrix(i_pattern)(i_height,i_width)=this->_neurons[i_width + i_height*width+i_pattern*width*height ];
            }
        }
    }
}




void NNLayerMatrix::setLearningRate(F32 eta){
    if(this->_type==MATRIXCONVOLUTIONNAL){
        eta = eta/std::sqrt(_neurons_matrix(0).getDomain()(0)*_neurons_matrix(0).getDomain()(1)*1.f);
    }
    NNLayer::setLearningRate(eta);
}

///////////////////////////////////////////////////////////////////////
//
//  NNLayer class definition


NNLayer::NNLayer(unsigned int nbr_neuron )
{
    for (unsigned int i_index=0; i_index<nbr_neuron; ++i_index ){
        _neurons.push_back( new NNNeuron() );
    }
}
NNLayer::~NNLayer(){
    for (unsigned int i_index=0; i_index<_neurons.size(); ++i_index ){
        delete _neurons[i_index];
    }
    for (unsigned int i_index=0; i_index<_weights.size(); ++i_index ){
        delete _weights[i_index];
    }
}



void NNLayer::setLearningRate(F32 eta)
{
#if defined(HAVE_OPENMP)
#pragma omp parallel for
#endif
    for( int i=0;i<static_cast<int>(_weights.size());i++){
        _weights[i]->_eta = eta;
    }
}
void NNLayer::propagateFront()
{
#if defined(HAVE_OPENMP)
#pragma omp parallel for
#endif
    for( int i_neuron =0;i_neuron<static_cast<int>( _neurons.size()); i_neuron++ )
    {
        NNNeuron * n =(_neurons[i_neuron]);
        n->propagateFront();
    }
}
void NNLayer::initPropagateBackFirstDerivate(){
#if defined(HAVE_OPENMP)
#pragma omp parallel for
#endif
    for( int i_neuron =0;i_neuron< static_cast<int>(_neurons.size()); i_neuron++ )
    {
        NNNeuron * n =(_neurons[i_neuron]);
        n->initPropagateBackFirstDerivate();
    }
}
void NNLayer::propagateBackFirstDerivate()
{
#if defined(HAVE_OPENMP)
#pragma omp parallel for
#endif
    for( int i_neuron =0;i_neuron< static_cast<int>(_neurons.size()); i_neuron++ )
    {
        NNNeuron * n =(_neurons[i_neuron]);
        n->initPropagateBackFirstDerivate();
    }
#if defined(HAVE_OPENMP)
#pragma omp parallel for
#endif
    for( int i_neuron =0;i_neuron<static_cast<int>( _neurons.size()); i_neuron++ )
    {
        NNNeuron * n =(_neurons[i_neuron]);
        n->propagateBackFirstDerivate();
    }
}
void NNLayer::initPropagateBackSecondDerivate(){
#if defined(HAVE_OPENMP)
#pragma omp parallel for
#endif
    for( int i_neuron =0;i_neuron< static_cast<int>(_neurons.size()); i_neuron++ )
    {
        NNNeuron * n =(_neurons[i_neuron]);
        n->initPropagateBackSecondDerivate();
    }
}

void NNLayer::propagateBackSecondDerivate()
{
#if defined(HAVE_OPENMP)
#pragma omp parallel for
#endif
    for( int i_neuron =0;i_neuron< static_cast<int>(_neurons.size()); i_neuron++ )
    {
        NNNeuron * n =(_neurons[i_neuron]);
        n->initPropagateBackSecondDerivate();
    }
#if defined(HAVE_OPENMP)
#pragma omp parallel for
#endif
    for( int i_neuron =0;i_neuron<static_cast<int>( _neurons.size()); i_neuron++ )
    {
        NNNeuron * n =(_neurons[i_neuron]);
        n->propagateBackSecondDerivate();
    }
}


void NNLayer::learningFirstDerivate()
{
#if defined(HAVE_OPENMP)
#pragma omp parallel for
#endif
    for( int i_weight =0;i_weight< static_cast<int>(_weights.size()); i_weight++ )
    {
        NNWeight * w =(_weights[i_weight]);
        w->learningFirstDerivate();
    }
}

///////////////////////////////////////////////////////////////////////
//
//  NNWeight


NNWeight::NNWeight(  F32 Wn, F32 eta) :
    _Wn( Wn ),_eta(eta)
{
}
void NNWeight::learningFirstDerivate()
{
    _Wn = _Wn-_eta*_dE_dWn;
}

///////////////////////////////////////////////////////////////////////
//
//  NNNeuron


NNNeuron::NNNeuron(ActivationFunction f_act) :
    _f_act(f_act), _Xn( 0.0 )
{
}
NNNeuron::~NNNeuron(){
}

void NNNeuron::addConnection(NNWeight* weight , NNNeuron* neuron)
{
    _connections.push_back( NNConnection( weight, neuron ) );
}

void NNNeuron::addConnectionBiais(NNWeight* weight)
{
    _connections.push_back( NNConnection( weight ) );
}
void NNNeuron::propagateFront(){

    _Yn=0;
    for ( unsigned int i_connection =0;i_connection< _connections.size(); i_connection++  )
    {
        NNConnection& c = (_connections[i_connection]);
        F32 weight = c._weight->_Wn;
        F32 neuron_out_previous = (c.isBiais() ? 1 : c._neuron->_Xn);
        _Yn += weight*neuron_out_previous;
    }
    switch ( _f_act) {
    case SIGMOID_FUNCTION:
        _Xn = SIGMOID( _Yn );
        break;
    case IDENTITY_FUNCTION:
        _Xn = _Yn;
        break;
    default:
        _Xn = SIGMOID( _Yn );
        POP_DbgAssertMessage(false,"error not known function");
        break;
    }
}
void NNNeuron::initPropagateBackFirstDerivate(){
    _dErr_dYn=0;
    for ( unsigned int i_connection =0;i_connection< _connections.size(); i_connection++  )
    {
        NNConnection& c = (_connections[i_connection]);
        c._weight->_dE_dWn =0;
        if(!c.isBiais())
        {
            c._neuron->_dErr_dXn=0;
        }
    }
}
void NNNeuron::initPropagateBackSecondDerivate(){
    _d2Err_dYn2=0;
    for ( unsigned int i_connection =0;i_connection< _connections.size(); i_connection++  )
    {
        NNConnection& c = (_connections[i_connection]);
        c._weight->_d2E_dWn2 =0;
        if(!c.isBiais())
        {
            c._neuron->_d2Err_dXn2=0;
        }
    }
}

void NNNeuron::propagateBackFirstDerivate(){

    F32 fprime_Y;
    switch ( _f_act) {
    case SIGMOID_FUNCTION:
        fprime_Y = DSIGMOID( _Xn );
        break;
    case IDENTITY_FUNCTION:
        fprime_Y = 1;
        break;
    default:
        fprime_Y =  DSIGMOID( _Xn );
        POP_DbgAssertMessage(false,"error not known function");
        break;
    }

    _dErr_dYn = fprime_Y * _dErr_dXn;

    for ( unsigned int i_connection =0;i_connection< _connections.size(); i_connection++  )
    {
        NNConnection& c = (_connections[i_connection]);
        F32 Xnm1 = (c.isBiais() ? 1 : c._neuron->_Xn);
        c._weight->_dE_dWn += Xnm1 * _dErr_dYn;
        if(!c.isBiais())
        {
            c._neuron->_dErr_dXn += c._weight->_Wn * _dErr_dYn;
        }
    }
}
void NNNeuron::propagateBackSecondDerivate(){

    F32 fprime_Y;
    switch ( _f_act) {
    case SIGMOID_FUNCTION:
        fprime_Y = DSIGMOID( _Xn );
        break;
    case IDENTITY_FUNCTION:
        fprime_Y = 1;
        break;
    default:
        fprime_Y =  DSIGMOID( _Xn );
        POP_DbgAssertMessage(false,"error not known function");
        break;
    }

    _d2Err_dYn2 = fprime_Y*fprime_Y*_d2Err_dXn2;
    for ( unsigned int i_connection =0;i_connection< _connections.size(); i_connection++  )
    {
        NNConnection& c = (_connections[i_connection]);
        F32 Xnm1;
        Xnm1 = (c.isBiais() ? 1 : c._neuron->_Xn);
        c._weight->_d2E_dWn2 += Xnm1*Xnm1*_d2Err_dYn2;
        if(!c.isBiais())
        {
            c._neuron->_d2Err_dXn2+= c._weight->_Wn*c._weight->_Wn*_d2Err_dYn2;
        }
    }
}
NNConnection::NNConnection()
    :_weight(NULL),_neuron(NULL)
{

}


NNConnection::NNConnection(NNWeight* weight , NNNeuron* neuron)
    :_weight(weight),_neuron(neuron)
{

}

void TrainingNeuralNetwork::neuralNetworkForRecognitionForHandwrittenDigits(NeuralNetworkFeedForward &n,std::string train_datapath,  std::string train_labelpath,std::string test_datapath,  std::string test_labelpath,int lecun_or_simard,F32 elastic_distortion)
{

    Vec<Vec<Mat2UI8> > number_training =  loadMNIST(train_datapath,train_labelpath);
    Vec<Vec<Mat2UI8> > number_test =  loadMNIST(test_datapath,test_labelpath);

    //        number_training.resize(2);
    //        number_test.resize(2);

    if(lecun_or_simard==0){
        std::cout<<"LECUN"<<std::endl;
        //LeCun network lenet5
        Vec2I32 domain(28,28);
        n.addInputLayerMatrix(domain(0),domain(1));
        n.addLayerConvolutional(10,5,1);
        n.addLayerMaxPooling(2);
        n.addLayerConvolutional(50,5,1);
        n.addLayerMaxPooling(2);
        n.addLayerFullyConnected(100,1);
    }else if(lecun_or_simard==1){
        std::cout<<"SIMARD"<<std::endl;
        //Simard network
        Vec2I32 domain(29,29);
        n.addInputLayerMatrix(domain(0),domain(1));
        n.addLayerConvolutionalPlusSubScaling(6,5,2,1);
        n.addLayerConvolutionalPlusSubScaling(50,5,2,1);
        n.addLayerFullyConnected(100,1);
    }else{
        std::cout<<"TARIEL"<<std::endl;
        Vec2I32 domain(29,29);
        n.addInputLayerMatrix(domain(0),domain(1));
        n.addLayerConvolutionalPlusSubScaling(6,5,2,1);
        n.addLayerIntegral(500,1);
        n.addLayerFullyConnected(100,1);
    }
    n.addLayerFullyConnected(static_cast<unsigned int>(number_training.size()),1);

    Vec<std::string> label_digit;
    for(int i=0;i<10;i++)
        label_digit.push_back(BasicUtility::Any2String(i));
    n.label2String() = label_digit;


    F32 sigma_min =0.15f;
    F32 sigma_max =0.25f;
    F32 alpha =0.13f;

    F32 standard_deviation_angle = PI/4;
    F32 standard_deviation_shear_j = PI/6;
    DistributionUniformReal dAngle(-standard_deviation_angle,standard_deviation_angle);
    DistributionUniformReal dShear(-standard_deviation_shear_j,standard_deviation_shear_j);

    DistributionUniformReal d_deviation_length(sigma_min,sigma_max);
    DistributionUniformReal d_correlation_lenght(0,alpha);

    DistributionUniformReal d(0,0.5);
    DistributionSign sign;



    Vec<VecF32> vtraining_in;
    Vec<VecF32> vtraining_out;

    for(unsigned int i=0;i<number_training.size();i++){
        for(unsigned int j=0;j<number_training(i).size();j++){
            Mat2UI8 binary = number_training(i)(j);
            int size_i = binary.sizeI();
            Draw::addBorder(binary,10, UI8(0));

            //            disp.display(binary);
            // We apply elastic deformation and affine transformation (rotation, shear)

            Mat2UI8 binary_scale =  binary;//GeometricalTransformation::scale(binary,Vec3F32(2,2));
            for(unsigned int k=0;k<elastic_distortion;k++){
                F32 deviation_length_random = d_deviation_length.randomVariable();
                F32 correlation_lenght_random =d_correlation_lenght.randomVariable();
                //                std::cout<<"deviation_length "<<deviation_length_random<<std::endl;
                //                std::cout<<"correlation_lenght_random "<<correlation_lenght_random<<std::endl;
                Mat2UI8 m= GeometricalTransformation::elasticDeformation(binary_scale,deviation_length_random*size_i,correlation_lenght_random*size_i);
                F32 angle = dAngle.randomVariable();
                F32 shear = dShear.randomVariable();

                F32 alphax=1,alphay=1;
                if(sign.randomVariable()>0)
                    alphax= 1+d.randomVariable();
                else
                    alphay= 1/(1+d.randomVariable());
                Vec2F32 v(alphax,alphay);
                //                std::cout<<"scale "<<v<<std::endl;
                //                std::cout<<"angle "<<angle<<std::endl;
                //                std::cout<<"shear "<<shear<<std::endl;
                Mat2x33F32 maffine  = GeometricalTransformation::translation2DHomogeneousCoordinate(m.getDomain()/2);//go back to the buttom left corner (origin)
                maffine *=  GeometricalTransformation::scale2DHomogeneousCoordinate(v);
                maffine *=  GeometricalTransformation::shear2DHomogeneousCoordinate(shear,0);
                maffine *=  GeometricalTransformation::rotation2DHomogeneousCoordinate(angle);//rotate
                maffine *=  GeometricalTransformation::translation2DHomogeneousCoordinate(-m.getDomain()/2);
                m = GeometricalTransformation::transformHomogeneous2D(maffine, m);
                //                m.display();
                VecF32 vin = n.inputMatrixToInputNeuron(m);
                vtraining_in.push_back(vin);
                VecF32 v_out(static_cast<int>(number_training.size()),-1);
                v_out(i)=1;
                vtraining_out.push_back(v_out);
            }
            VecF32 vin = n.inputMatrixToInputNeuron(binary);
            vtraining_in.push_back(vin);
            VecF32 v_out(static_cast<int>(number_training.size()),-1);
            v_out(i)=1;
            vtraining_out.push_back(v_out);

        }
    }

    Vec<VecF32> vtest_in;
    Vec<VecF32> vtest_out;
    for(unsigned int i=0;i<number_test.size();i++){
        for(unsigned int j=0;j<number_test(i).size();j++){
            Mat2UI8 binary = number_test(i)(j);
            VecF32 vin = n.inputMatrixToInputNeuron(binary);
            vtest_in.push_back(vin);
            VecF32 v_out(static_cast<int>(number_test.size()),-1);
            v_out(i)=1;
            vtest_out.push_back(v_out);
        }
    }

    trainingFirstDerivative(n,vtraining_in,vtraining_out,vtest_in,vtest_out,0.01f,50);
}

//static std::string getCurrentTime() {
//    time_t rawtime;
//    time(&rawtime);
//    struct tm * timeinfo = localtime(&rawtime);
//    return std::string(asctime(timeinfo));
//}

void TrainingNeuralNetwork::trainingFirstDerivative(NeuralNetworkFeedForward&n, const Vec<VecF32>& trainingins, const Vec<VecF32>& trainingouts, F32 eta, unsigned int nbr_epoch)
{
    n.setLearningRate(eta);
    std::vector<int> v_global_rand(trainingins.size());
    for(unsigned int i=0;i<v_global_rand.size();i++)
        v_global_rand[i]=i;
    std::cout<<"iter_epoch\t error_train"<<std::endl;

    for(unsigned int i=0;i<nbr_epoch;i++){
        std::random_shuffle ( v_global_rand.begin(), v_global_rand.end() ,Distribution::irand());
        int error=0;
        for(unsigned int j=0;j<v_global_rand.size();j++){
            VecF32 vout;
            n.propagateFront(trainingins(v_global_rand[j]),vout);
            n.propagateBackFirstDerivate(trainingouts(v_global_rand[j]));
            n.learningFirstDerivate();
            int label1 = std::distance(vout.begin(),std::max_element(vout.begin(),vout.end()));
            int label2 = std::distance(trainingouts(v_global_rand[j]).begin(),std::max_element(trainingouts(v_global_rand[j]).begin(),trainingouts(v_global_rand[j]).end()));
            if(label1!=label2)
                error++;
        }

        std::cout<<i<<"\t"<<error*1.0/v_global_rand.size()<<std::endl;
    }
}

void TrainingNeuralNetwork::trainingFirstDerivative(NeuralNetworkFeedForward&n,const Vec<VecF32>& trainingins,const Vec<VecF32>& trainingouts,const Vec<VecF32>& testins,const Vec<VecF32>& testouts,F32 eta,unsigned int nbr_epoch)
{
    n.setLearningRate(eta);
    std::vector<int> v_global_rand(trainingins.size());
    for(unsigned int i=0;i<v_global_rand.size();i++)
        v_global_rand[i]=i;
    std::cout<<"iter_epoch\t error_train\t error_test\t learning rate"<<std::endl;

    for(unsigned int i=0;i<nbr_epoch;i++){
        std::random_shuffle ( v_global_rand.begin(), v_global_rand.end() ,Distribution::irand());
        int error_training=0,error_test=0;
        for(unsigned int j=0;j<v_global_rand.size();j++){
            VecF32 vout;
            n.propagateFront(trainingins(v_global_rand[j]),vout);
            n.propagateBackFirstDerivate(trainingouts(v_global_rand[j]));
            n.learningFirstDerivate();
            int label1 = std::distance(vout.begin(),std::max_element(vout.begin(),vout.end()));
            int label2 = std::distance(trainingouts(v_global_rand[j]).begin(),std::max_element(trainingouts(v_global_rand[j]).begin(),trainingouts(v_global_rand[j]).end()));
            if(label1!=label2)
                error_training++;
        }
        //        std::cout<<testins.size()<<std::endl;

        for(unsigned int j=0;j<testins.size();j++){
            VecF32 vout;
            n.propagateFront(testins(j),vout);
            int label1 = std::distance(vout.begin(),std::max_element(vout.begin(),vout.end()));
            int label2 = std::distance(testouts(j).begin(),std::max_element(testouts(j).begin(),testouts(j).end()));
            if(label1!=label2)
                error_test++;
        }
        n.save(("neuralnetwork"+BasicUtility::IntFixedDigit2String(i,2)+".xml").c_str() );

        std::cout<<i<<"\t"<<error_training*1./trainingins.size()<<"\t"<<error_test*1.0/testins.size() <<"\t"<<eta<<std::endl;
        eta *=0.9f;
        n.setLearningRate(eta);
    }
}
void NNNeuronMaxPool::addConnection( NNNeuron* neuron){
    this->_connections.push_back( NNConnection( NULL, neuron ) );
}

void NNNeuronMaxPool::propagateFront(){
    this->_Yn=-(std::numeric_limits<F32>::max)();
    for ( unsigned int i_connection =0;i_connection< _connections.size(); i_connection++  )
    {

        NNConnection& c = (_connections[i_connection]);
        this->_Yn = (std::max)(_Yn,c._neuron->_Xn);
    }
    this->_Xn = this->_Yn;
}

void NNNeuronMaxPool::propagateBackFirstDerivate(){

    _dErr_dYn = _dErr_dXn;
    F32 max_value = -(std::numeric_limits<F32>::max)();
    unsigned int i_index_max=0;
    for ( unsigned int i_connection =0;i_connection< _connections.size(); i_connection++  )
    {
        NNConnection& c = (_connections[i_connection]);
        NNNeuron * neuron = c._neuron;
        c._neuron->_dErr_dXn = _dErr_dXn;
        if(neuron->_Xn>max_value){
            i_index_max = i_connection;
            max_value = neuron->_Xn;
        }
    }

    NNConnection& c = (_connections[i_index_max]);
    c._neuron->_dErr_dXn = 0;
}

void NNNeuronMaxPool::initPropagateBackFirstDerivate(){
    _dErr_dYn=0;
    for ( unsigned int i_connection =0;i_connection< _connections.size(); i_connection++  )
    {
        NNConnection& c = (_connections[i_connection]);
        c._neuron->_dErr_dXn=0;
    }
}
int TrainingNeuralNetwork::_reverseInt(int i) {
    unsigned char c1, c2, c3, c4;
    c1 = i & 255;
    c2 = (i >> 8) & 255;
    c3 = (i >> 16) & 255;
    c4 = (i >> 24) & 255;
    return ((int)c1 << 24) + ((int)c2 << 16) + ((int)c3 << 8) + c4;
}

