HEADERS += $${PWD}/include/Population.h \
           $${PWD}/include/PopulationConfig.h \
           $${PWD}/include/algorithm/Analysis.h \
           $${PWD}/include/algorithm/Arithmetic.h \
           $${PWD}/include/algorithm/AnalysisAdvanced.h \
           $${PWD}/include/algorithm/Application.h \
           $${PWD}/include/algorithm/Convertor.h \
           $${PWD}/include/algorithm/Draw.h \
           $${PWD}/include/algorithm/Feature.h \
           $${PWD}/include/algorithm/ForEachFunctor.h \
           $${PWD}/include/algorithm/GeometricalTransformation.h \
           $${PWD}/include/algorithm/LinearAlgebra.h \
           $${PWD}/include/algorithm/PDE.h \
           $${PWD}/include/algorithm/PDEAdvanced.h \
           $${PWD}/include/algorithm/Processing.h \
           $${PWD}/include/algorithm/ProcessingAdvanced.h \
           $${PWD}/include/algorithm/ProcessingVideo.h \
           $${PWD}/include/algorithm/RandomGeometry.h \
           $${PWD}/include/algorithm/Representation.h \
           $${PWD}/include/algorithm/Statistics.h \
           $${PWD}/include/algorithm/Visualization.h \
           $${PWD}/include/3rdparty/bipmap.h \
           $${PWD}/include/3rdparty/CImg.h \
           $${PWD}/include/3rdparty/ConvertorCImg.h \
           $${PWD}/include/3rdparty/ConvertorOpenCV.h \
           $${PWD}/include/3rdparty/ConvertorQImage.h \
           $${PWD}/include/3rdparty/direntvc.h \
           $${PWD}/include/3rdparty/fparser.hh \
           $${PWD}/include/3rdparty/fpconfig.hh \
           $${PWD}/include/3rdparty/fptypes.hh \
           $${PWD}/include/3rdparty/jpgd.h \
           $${PWD}/include/3rdparty/jpge.h \
           $${PWD}/include/3rdparty/lodepng.h \
           $${PWD}/include/3rdparty/MatNDisplayCImg.h \
           $${PWD}/include/3rdparty/MTRand.h \
           $${PWD}/include/3rdparty/pugiconfig.hpp \
           $${PWD}/include/3rdparty/pugixml.hpp \
           $${PWD}/include/3rdparty/tinythread.h \
           $${PWD}/include/3rdparty/VideoFFMPEG.h \
           $${PWD}/include/3rdparty/VideoVLC.h \
           $${PWD}/include/3rdparty/VideoVLCDeprecated.h \
           $${PWD}/include/data/3d/GLFigure.h \
           $${PWD}/include/data/distribution/Distribution.h \
           $${PWD}/include/data/distribution/DistributionAnalytic.h \
           $${PWD}/include/data/distribution/DistributionArithmetic.h \
           $${PWD}/include/data/distribution/DistributionFromDataStructure.h \
           $${PWD}/include/data/distribution/DistributionMultiVariate.h \
           $${PWD}/include/data/distribution/DistributionMultiVariateArithmetic.h \
           $${PWD}/include/data/distribution/DistributionMultiVariateFromDataStructure.h \
           $${PWD}/include/data/functor/FunctorF.h \
           $${PWD}/include/data/functor/FunctorPDE.h \
           $${PWD}/include/data/germgrain/Germ.h \
           $${PWD}/include/data/germgrain/GermGrain.h \
           $${PWD}/include/data/mat/Mat2x.h \
           $${PWD}/include/data/mat/MatN.h \
           $${PWD}/include/data/mat/MatNBoundaryCondition.h \
           $${PWD}/include/data/mat/MatNDisplay.h \
           $${PWD}/include/data/mat/MatNInOut.h \
           $${PWD}/include/data/mat/MatNIteratorE.h \
           $${PWD}/include/data/neuralnetwork/NeuralNetwork.h \
           $${PWD}/include/data/notstable/CharacteristicCluster.h \
           $${PWD}/include/data/notstable/Classifer.h \
           $${PWD}/include/data/notstable/Descriptor.h \
           $${PWD}/include/data/notstable/Ransac.h \
           $${PWD}/include/data/notstable/Wavelet.h \
           $${PWD}/include/data/notstable/MatNReference.h \
           $${PWD}/include/data/ocr/OCR.h \
           $${PWD}/include/data/population/PopulationData.h \
           $${PWD}/include/data/population/PopulationFunctor.h \
           $${PWD}/include/data/population/PopulationGrows.h \
           $${PWD}/include/data/population/PopulationPDE.h \
           $${PWD}/include/data/population/PopulationQueues.h \
           $${PWD}/include/data/population/PopulationRestrictedSet.h \
           $${PWD}/include/data/typeF/Complex.h \
           $${PWD}/include/data/typeF/RGB.h \
           $${PWD}/include/data/typeF/TypeF.h \
           $${PWD}/include/data/typeF/TypeTraitsF.h \
           $${PWD}/include/data/utility/BasicUtility.h \
           $${PWD}/include/data/utility/Cryptography.h \
           $${PWD}/include/data/utility/BSPTree.h \
           $${PWD}/include/data/utility/XML.h \
           $${PWD}/include/data/vec/Vec.h \
           $${PWD}/include/data/vec/VecN.h \
           $${PWD}/include/data/video/Video.h \
           $${PWD}/include/data/notstable/graph/Graph.h \
           $${PWD}/include/data/functor/FunctorMatN.h \
           $${PWD}/include/tensor/tensor.h \
           $${PWD}/include/algorithm/processingtensor.h

SOURCES += $${PWD}/src/algorithm/GeometricalTransformation.cpp \
           $${PWD}/src/algorithm/LinearAlgebra.cpp \
           $${PWD}/src/algorithm/RandomGeometry.cpp \
           $${PWD}/src/algorithm/Statistics.cpp \
           $${PWD}/src/algorithm/Visualization.cpp \
           $${PWD}/src/3rdparty/ConvertorOpenCV.cpp \
           $${PWD}/src/3rdparty/ConvertorQImage.cpp \
           $${PWD}/src/3rdparty/fparser.cpp \
           $${PWD}/src/3rdparty/jpgd.cpp \
           $${PWD}/src/3rdparty/jpge.cpp \
           $${PWD}/src/3rdparty/lodepng.cpp \
           $${PWD}/src/3rdparty/MatNDisplayCImg.cpp \
           $${PWD}/src/3rdparty/MTRand.cpp \
           $${PWD}/src/3rdparty/pugixml.cpp \
           $${PWD}/src/3rdparty/tinythread.cpp \
           $${PWD}/src/3rdparty/VideoFFMPEG.cpp \
           $${PWD}/src/3rdparty/VideoVLC.cpp \
           $${PWD}/src/3rdparty/VideoVLCDeprecated.cpp \
           $${PWD}/src/data/3d/GLFigure.cpp \
           $${PWD}/src/data/distribution/Distribution.cpp \
           $${PWD}/src/data/distribution/DistributionAnalytic.cpp \
           $${PWD}/src/data/distribution/DistributionArithmetic.cpp \
           $${PWD}/src/data/distribution/DistributionFromDataStructure.cpp \
           $${PWD}/src/data/distribution/DistributionMultiVariate.cpp \
           $${PWD}/src/data/distribution/DistributionMultiVariateArithmetic.cpp \
           $${PWD}/src/data/distribution/DistributionMultiVariateFromDataStructure.cpp \
           $${PWD}/src/data/germgrain/GermGrain.cpp \
           $${PWD}/src/data/mat/MatNDisplay.cpp \
           $${PWD}/src/data/mat/MatNInOut.cpp \
           $${PWD}/src/data/neuralnetwork/NeuralNetwork.cpp \
           $${PWD}/src/data/notstable/Ransac.cpp \
           $${PWD}/src/data/ocr/OCR.cpp \
           $${PWD}/src/data/utility/BasicUtility.cpp \
           $${PWD}/src/data/utility/Cryptography.cpp \
           $${PWD}/src/data/utility/XML.cpp \
           $${PWD}/src/data/video/Video.cpp \
           $${PWD}/src/tensor/tensor.cpp \
           $${PWD}/src/algorithm/processingtensor.cpp
