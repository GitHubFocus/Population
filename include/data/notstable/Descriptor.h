#ifndef DESCRIPTOR_H
#define DESCRIPTOR_H
#include<vector>
#include"data/utility/BasicUtility.h"
#include"data/vec/VecN.h"
#include"data/mat/MatN.h"
namespace pop{

template<int DIM,typename Type>
struct Pyramid
{
    pop::MatN<DIM+1,Type> & octave(unsigned int index_octave){
        POP_DbgAssert(index_octave<_pyramid.size());
        return _pyramid(index_octave);
    }
    const pop::MatN<DIM+1,Type> & octave(unsigned int index_octave)const{
        POP_DbgAssert(index_octave<_pyramid.size());
        return _pyramid(index_octave);
    }
    pop::MatN<DIM,Type>  getLayer(unsigned int index_octave,unsigned int index_layer)const{

        const pop::MatN<DIM+1,Type> &  moctave = octave(index_octave);
        MatN<DIM,Type> layer(moctave.getDomain().removeCoordinate(DIM));
        typename MatN<DIM,Type> ::IteratorEDomain it_plane(layer.getIteratorEDomain());
        while(it_plane.next()){
            layer(it_plane.x()) = moctave(it_plane.x().addCoordinate(DIM,index_layer));
        }

        return layer;
    }
    void setLayer(unsigned int index_octave,unsigned int index_layer,const pop::MatN<DIM,Type> & layer){

        pop::MatN<DIM+1,Type> &  moctave = octave(index_octave);
        typename pop::MatN<DIM,Type>::IteratorEDomain it(layer.getIteratorEDomain());
        while(it.next())
        {
            moctave(it.x().addCoordinate(DIM,index_layer))=layer(it.x());
        }
    }
    void pushOctave(const VecN<DIM,int>& size,unsigned int nbr_layers)
    {
        VecN<DIM+1,int> domain(size);
        domain(DIM) = nbr_layers;
        _pyramid.push_back(pop::MatN<DIM+1,Type>(domain));
    }
    void pushOctave(const pop::MatN<DIM+1,Type>& octave_single)
    {
        _pyramid.push_back(octave_single);
    }
    unsigned int nbrOctave()const{return _pyramid.size();}
    unsigned int nbrLayers(unsigned int index_octave=0)const{return _pyramid(index_octave).getDomain()(DIM);}

    unsigned int getNbrExtraLayerPerOctave()const{return _nbrextralayerperoctave;}
    void setNbrExtraLayerPerOctave(unsigned int nbrextralayerperoctave ){_nbrextralayerperoctave = nbrextralayerperoctave;}

    Vec<Type> getProfile(const VecN<DIM,int>& point)const{
        Vec<Type> v (_pyramid.size()*(_pyramid[0].getDomain()(DIM)-_nbrextralayerperoctave));
        int k=0;
        VecN<DIM+1,F32> point_pyramid;
        for(int i=0;i<DIM;i++)
            point_pyramid(i)=point(i);
        for(unsigned int i_octave =0;i_octave<_pyramid.size();i_octave++){
            for(unsigned int i_layer =0;i_layer<_pyramid[i_octave].getDomain()(DIM)-_nbrextralayerperoctave;i_layer++){
                point_pyramid(DIM)=i_layer;
                v[k]=_pyramid(i_octave).interpolationBilinear(point_pyramid);
                k++;
            }
            for(int i=0;i<DIM;i++)
                point_pyramid(i)/=2.;
        }
        return v;
    }

    unsigned int _nbrextralayerperoctave;
    pop::Vec<pop::MatN<DIM+1,Type> >_pyramid;
};

template<int Dim>
struct KeyPoint
{
    enum{DIM=Dim};
    KeyPoint(){}
    KeyPoint(const VecN<DIM,F32>& x_value)
        :_x(x_value){}
    VecN<DIM,F32> _x;
    const VecN<DIM,F32> &x()const
    {
        return _x;
    }
    VecN<DIM,F32> &x()
    {
        return _x;
    }

    F32 scale()const
    {
        return 1;
    }
};


template<int Dim>
struct KeyPointPyramid
{
    enum{DIM=Dim};
    VecN<DIM+1,F32> _x_in_pyramid;
    unsigned int _octave;
    unsigned int _nbr_layer_per_octave;
    int _label;
    KeyPointPyramid(){}
    KeyPointPyramid(const VecN<DIM+1,F32> & x_in_pyramid,unsigned int octave_value,unsigned int nbr_layer_per_octave=3 )
        :_x_in_pyramid(x_in_pyramid),_octave(octave_value),_nbr_layer_per_octave(nbr_layer_per_octave){}
    VecN<DIM,F32> x()const
    {
        VecN<DIM,F32> x_value;
        for(int i=0;i<DIM;i++)
            x_value(i)=_x_in_pyramid(i)*pop::power2(_octave);
        return x_value;
    }
    F32 layer()const{
        return _x_in_pyramid(DIM);
    }
    unsigned int octave()const{
        return _octave;
    }
    unsigned int &octave(){
        return _octave;
    }
    const VecN<DIM+1,F32>& xInPyramid()const
    {
        return _x_in_pyramid;
    }
    VecN<DIM+1,F32>& xInPyramid()
    {
        return _x_in_pyramid;
    }
    F32 scale()const
    {
        return std::pow(2.,_octave*1.)*std::pow(2.,layer()/_nbr_layer_per_octave);
    }
};

template<typename TKeyPoint >
struct Descriptor
{
    enum{DIM=TKeyPoint::DIM};
    TKeyPoint _keypoint;
    VecN<TKeyPoint::DIM,F32> _orientation;
    Mat2F32 _data;


    VecN<DIM,F32> x()const{return _keypoint.x();}
    TKeyPoint &keyPoint(){return _keypoint;}
    const TKeyPoint &keyPoint()const{return _keypoint;}
    VecN<TKeyPoint::DIM,F32> &orientation(){return _orientation;}
    const VecN<TKeyPoint::DIM,F32> &orientation()const{return _orientation;}
    Mat2F32 &data(){return _data;}
    const Mat2F32 &data()const{return _data;}
};


template<typename Descriptor>
struct DescriptorMatch
{
    enum{DIM=Descriptor::DIM};
    Descriptor _d1;
    Descriptor _d2;
    F32 _error;
    bool operator <(const DescriptorMatch& d)const{
        if(_error<d._error)
            return true;
        else
            return false;
    }
    VecN<Descriptor::DIM,F32> x()const{return _d1.x();}
    VecN<Descriptor::DIM,F32> x1()const{return _d1.x();}
    VecN<Descriptor::DIM,F32> x2()const{return _d2.x();}
};
template<int DIM>
class PieChartInPieChart
{

public:
    Mat2F32 _data;
    std::vector<F32> _v_projection;
    VecN<DIM,F32> _orientation;
    PieChartInPieChart(const VecN<DIM,F32>& orientation,int nbr_orientations=6)
        :_data(nbr_orientations,nbr_orientations),_v_projection(nbr_orientations),_orientation(orientation/normValue(orientation))
    {
        for(int i=0;i<nbr_orientations;i++){
            if(DIM==2)
                _v_projection[i]=(-std::cos(1.f*i/nbr_orientations*PI));
            else
                _v_projection[i]=((1.f*i/nbr_orientations)*2.-1.f);
        }
    }
    void addValue(const VecN<DIM,F32>& orientationglobal ,  const VecN<DIM,F32>& orientationlocal,F32 weigh=1){
        F32 value = productInner(_orientation,orientationglobal/normValue(orientationglobal));

        std::vector<F32>::iterator it_projection = std::upper_bound(_v_projection.begin(),_v_projection.end(),value);
        int index_orientation_global = _v_projection.size()-int(it_projection- _v_projection.begin());

        value = productInner(_orientation,orientationlocal/normValue(orientationlocal));
        it_projection = std::upper_bound(_v_projection.begin(),_v_projection.end(),value);
        int index_orientation_local = _v_projection.size()-int(it_projection- _v_projection.begin());
        if(index_orientation_global<0)index_orientation_global=0;
        if(index_orientation_global>5)index_orientation_global=5;
        if(index_orientation_local<0)index_orientation_local=0;
        if(index_orientation_local>5)index_orientation_local=5;
        _data(index_orientation_global,index_orientation_local)+=weigh;
    }
    void normalize(){
        for(unsigned int i=0;i<_data.sizeI();i++){
            F32 sum =0;
            for(unsigned int j=0;j<_data.sizeJ();j++){
                sum+=_data(i,j);
            }
            if(sum==0)
            {
                sum=1;
            }
            for(unsigned int j=0;j<_data.sizeJ();j++){
                _data(i,j)/=sum;
            }
        }
    }
};
template<typename TKeyPoint>
F32 distance(const pop::Descriptor<TKeyPoint> & d1,const pop::Descriptor<TKeyPoint>  & d2, int p =2)
{
    return distance(d1.data(),d2.data(),p);
}
}
#endif // DESCRIPTOR_H
