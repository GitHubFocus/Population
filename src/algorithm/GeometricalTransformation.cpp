#include"algorithm/GeometricalTransformation.h"
#include"algorithm/LinearAlgebra.h"


namespace pop{

Mat2x22F32 GeometricalTransformation::rotation2D(F32 theta_radian){
    Mat2x22F32 r;
    r(0,0) = std::cos(theta_radian);r(0,1) = -std::sin(theta_radian);
    r(1,0) = std::sin(theta_radian);r(1,1) =  std::cos(theta_radian);
    return r;
}
Mat2x33F32 GeometricalTransformation::rotation2DHomogeneousCoordinate(F32 angle_radian )
{
    Mat2x33F32 rot;
    rot(0,0)=std::cos(angle_radian);rot(0,1)=-std::sin(angle_radian);rot(0,2)=0;
    rot(1,0)=std::sin(angle_radian);rot(1,1)=std::cos(angle_radian);rot(1,2)=0;
    rot(2,0)=0;rot(2,1)=0;rot(2,2)=1;
    return rot;
}
Mat2x33F32 GeometricalTransformation::rotationFromAxis(Vec3F32 u,F32 angle_radian)
{
    F32 costheta = std::cos(angle_radian);
    F32 sintheta = std::sin(angle_radian);
    Mat2x33F32 R;
    R(0,0)= costheta + u(0)*u(0)*(1-costheta)     ;R(0,1) =  u(0)*u(1)*(1-costheta) - u(2)*sintheta; R(0,2) = u(0)*u(2)*(1-costheta) + u(1)*sintheta;
    R(1,0)= u(1)*u(0)*(1-costheta) + u(2)*sintheta;R(1,1) =  costheta + u(1)*u(1)*(1-costheta)     ; R(1,2) = u(1)*u(2)*(1-costheta) - u(0)*sintheta;
    R(2,0)= u(2)*u(0)*(1-costheta) - u(1)*sintheta;R(2,1) =  u(2)*u(1)*(1-costheta) + u(0)*sintheta; R(2,2) = costheta + u(2)*u(2)*(1-costheta);
    return R;
}
Mat2x33F32 GeometricalTransformation::rotationFromVectorToVector(const Vec3F32 & s, const Vec3F32 &  t)
{
    if(s==t)
       return Mat2x33F32::identity();
    Vec3F32 source=s/s.norm();
    Vec3F32 target=t/t.norm();
    F32 dot = productInner(source, target);
    F32 angle = std::acos(dot);
    Vec3F32 cross = productVectoriel(source, target);
    cross/=cross.norm();
    return   rotationFromAxis(cross,angle);

}
Mat2x33F32 GeometricalTransformation::rotation3D(F32 theta_radian,int coordinate)
{
    if(coordinate<0||coordinate>2)
        coordinate=0;
    Mat2x33F32 r;
    if(coordinate==0){
        r(0,0)=1;r(0,1)=0;r(0,2)=0;
        r(1,0)=0;r(1,1)=std::cos(theta_radian);r(1,2)=-std::sin(theta_radian);
        r(2,0)=0;r(2,1)=std::sin(theta_radian);r(2,2)=std::cos(theta_radian);
    }else if(coordinate==1){
        r(0,0)=std::cos(theta_radian);r(0,1)=0;r(0,2)=-std::sin(theta_radian);
        r(1,0)=0;r(1,1)=1;r(1,2)=0;
        r(2,0)=std::sin(theta_radian);r(2,1)=0;r(2,2)=std::cos(theta_radian);
    }else{
        r(0,0)=std::cos(theta_radian);r(0,1)=-std::sin(theta_radian);r(0,2)=0;
        r(1,0)=std::sin(theta_radian);r(1,1)=std::cos(theta_radian);r(1,2)=0;
        r(2,0)=0;r(2,1)=0;r(2,2)=1;
    }
    return r;
}
Mat2F32 GeometricalTransformation::rotation3DHomogeneousCoordinate(F32 theta_radian,int coordinate)
{
    if(coordinate<0||coordinate>2)
        coordinate=0;
    Mat2F32 r(4,4);
    r(3,3)=1;
    if(coordinate==0){
        r(0,0)=1;r(0,1)=0;r(0,2)=0;
        r(1,0)=0;r(1,1)=std::cos(theta_radian);r(1,2)=-std::sin(theta_radian);
        r(2,0)=0;r(2,1)=std::sin(theta_radian);r(2,2)=std::cos(theta_radian);
    }else if(coordinate==1){
        r(0,0)=std::cos(theta_radian);r(0,1)=0;r(0,2)=-std::sin(theta_radian);
        r(1,0)=0;r(1,1)=1;r(1,2)=0;
        r(2,0)=std::sin(theta_radian);r(2,1)=0;r(2,2)=std::cos(theta_radian);
    }else{
        r(0,0)=std::cos(theta_radian);r(0,1)=-std::sin(theta_radian);r(0,2)=0;
        r(1,0)=std::sin(theta_radian);r(1,1)=std::cos(theta_radian);r(1,2)=0;
        r(2,0)=0;r(2,1)=0;r(2,2)=1;
    }
    return r;
}
Mat2x33F32 GeometricalTransformation::translation2DHomogeneousCoordinate(const Vec2F32 &t )
{
    Mat2x33F32 trans;
    trans(0,0)=1;trans(0,1)=0;trans(0,2)=t(0);
    trans(1,0)=0;trans(1,1)=1;trans(1,2)=t(1);
    trans(2,0)=0;trans(2,1)=0;trans(2,2)=1;
    return trans;
}
Mat2F32 GeometricalTransformation::translation3DHomogeneousCoordinate(const Vec3F32 &t ){
    Mat2F32 r(4,4);
    r(0,0)=1;r(1,1)=1;r(2,2)=1;r(3,3)=1;
    r(0,3)=t(0);
    r(1,3)=t(1);
    r(2,3)=t(2);
    return r;
}
Mat2x22F32 GeometricalTransformation::scale2D(const Vec2F32 &s  )
{
    Mat2x22F32 r;
    r(0,0)=s(0);r(1,1)=s(1);
    return r;
}
Mat2x33F32 GeometricalTransformation::scale2DHomogeneousCoordinate(const Vec2F32 &s  )
{
    Mat2x33F32 r;
    r(0,0)=s(0);r(1,1)=s(1);r(2,2)=1;
    return r;
}
Mat2x33F32 GeometricalTransformation::scale3D(const Vec3F32 &s  )
{
    Mat2x33F32 r;
    r(0,0)=s(0);r(1,1)=s(1);r(2,2)=s(2);
    return r;
}
Mat2F32 GeometricalTransformation::scale3DHomogeneousCoordinate(const Vec3F32 &s  )
{
    Mat2F32 r(4,4);
    r(0,0)=s(0);r(1,1)=s(1);r(2,2)=s(2);r(3,3)=1;
    return r;
}


Mat2x33F32 GeometricalTransformation::affine2D(const Vec2F32 src[3], const Vec2F32 dst[3],bool isfastinversion)
{
    Mat2F32 m(6,6);

    m(0,0)=src[0](0);m(0,1)=src[0](1);m(0,2)=1;
    m(1,0)=src[1](0);m(1,1)=src[1](1);m(1,2)=1;
    m(2,0)=src[2](0);m(2,1)=src[2](1);m(2,2)=1;

    m(3,3)=src[0](0);m(3,4)=src[0](1);m(3,5)=1;
    m(4,3)=src[1](0);m(4,4)=src[1](1);m(4,5)=1;
    m(5,3)=src[2](0);m(5,4)=src[2](1);m(5,5)=1;

    VecF32 V(6);
    V(0)=dst[0](0);V(1)=dst[1](0);V(2)=dst[2](0);V(3)=dst[0](1);V(4)=dst[1](1);V(5)=dst[2](1);
    if(isfastinversion==true){
        V=pop::LinearAlgebra::inverseGaussianElimination(m)*V;
    }else{
        V = m.inverse()*V;
    }
    Mat2x33F32 maffine;
    maffine(0,0)=V(0);maffine(0,1)=V(1);maffine(0,2)=V(2);
    maffine(1,0)=V(3);maffine(1,1)=V(4);maffine(1,2)=V(5);
    maffine(2,0)=0;maffine(2,1)=0;maffine(2,2)=1;
    return maffine;
}

/* Calculates coefficients of perspective transformation
 * which maps (xi,yi) to (ui,vi), (i=1,2,3,4):
 *
 *      c00*xi + c01*yi + c02
 * ui = ---------------------
 *      c20*xi + c21*yi + c22
 *
 *      c10*xi + c11*yi + c12
 * vi = ---------------------
 *      c20*xi + c21*yi + c22
 *
 * Coefficients are calculated by solving linear system:
 * / x0 y0  1  0  0  0 -x0*u0 -y0*u0 \ /c00\ /u0\
 * | x1 y1  1  0  0  0 -x1*u1 -y1*u1 | |c01| |u1|
 * | x2 y2  1  0  0  0 -x2*u2 -y2*u2 | |c02| |u2|
 * | x3 y3  1  0  0  0 -x3*u3 -y3*u3 |.|c10|=|u3|,
 * |  0  0  0 x0 y0  1 -x0*v0 -y0*v0 | |c11| |v0|
 * |  0  0  0 x1 y1  1 -x1*v1 -y1*v1 | |c12| |v1|
 * |  0  0  0 x2 y2  1 -x2*v2 -y2*v2 | |c20| |v2|
 * \  0  0  0 x3 y3  1 -x3*v3 -y3*v3 / \c21/ \v3/
 *
 * where:
 *   cij - matrix coefficients, c22 = 1
 */

Mat2x33F32 GeometricalTransformation::projective2D( const VecN<4,Vec2F32>& src,  const VecN<4,Vec2F32>&dst, bool isfastinversion)
{
    Mat2F32 m(8,8);
    m.fill(0);
    m(0,0)=src[0](0);m(0,1)=src[0](1);m(0,2)=1;  m(0,6)=-src[0](0)*dst[0](0);m(0,7)=-src[0](1)*dst[0](0);
    m(1,0)=src[1](0);m(1,1)=src[1](1);m(1,2)=1;  m(1,6)=-src[1](0)*dst[1](0);m(1,7)=-src[1](1)*dst[1](0);
    m(2,0)=src[2](0);m(2,1)=src[2](1);m(2,2)=1;  m(2,6)=-src[2](0)*dst[2](0);m(2,7)=-src[2](1)*dst[2](0);
    m(3,0)=src[3](0);m(3,1)=src[3](1);m(3,2)=1;  m(3,6)=-src[3](0)*dst[3](0);m(3,7)=-src[3](1)*dst[3](0);

    m(4,3)=src[0](0);m(4,4)=src[0](1);m(4,5)=1;  m(4,6)=-src[0](0)*dst[0](1);m(4,7)=-src[0](1)*dst[0](1);
    m(5,3)=src[1](0);m(5,4)=src[1](1);m(5,5)=1;  m(5,6)=-src[1](0)*dst[1](1);m(5,7)=-src[1](1)*dst[1](1);
    m(6,3)=src[2](0);m(6,4)=src[2](1);m(6,5)=1;  m(6,6)=-src[2](0)*dst[2](1);m(6,7)=-src[2](1)*dst[2](1);
    m(7,3)=src[3](0);m(7,4)=src[3](1);m(7,5)=1;  m(7,6)=-src[3](0)*dst[3](1);m(7,7)=-src[3](1)*dst[3](1);


    VecF32 V(8);
    V(0)=dst[0](0);V(1)=dst[1](0);V(2)=dst[2](0);V(3)=dst[3](0);V(4)=dst[0](1);V(5)=dst[1](1);V(6)=dst[2](1);V(7)=dst[3](1);

    if(isfastinversion==true){
        V=pop::LinearAlgebra::inverseGaussianElimination(m)*V;
    }else{
        V = m.inverse()*V;
    }
    Mat2x33F32 mproj;
    mproj(0,0)=V(0);mproj(0,1)=V(1);mproj(0,2)=V(2);
    mproj(1,0)=V(3);mproj(1,1)=V(4);mproj(1,2)=V(5);
    mproj(2,0)=V(6);mproj(2,1)=V(7);mproj(2,2)=1;
    return mproj;
}

Mat2x22F32 GeometricalTransformation::shear2D(F32 theta_radian, int coordinate)
{
   if(coordinate<0||coordinate>1)
       coordinate=0;
   Mat2x22F32 shear;
   if(coordinate==0){
       shear(0,0)=1;shear(0,1)=std::sin(theta_radian);
       shear(1,0)=0;shear(1,1)=1;
   }else{
       shear(0,0)=1;shear(0,1)=0;
       shear(1,0)=std::sin(theta_radian);shear(1,1)=1;
   }

 return shear;
}

//-------------------------------------------------------------------
/*!
 * \brief 2D shear matrix in homogeneous coordinates
 * \param theta_radian angle in radian
 * \return  Shear Mat2x33F32
 *
 *  Generate the 2D shear matrix from the rotation of angle \a theta_radian  in homogeneous coordinates:\n
 *  For coordinate=0 we have,\f$S_x(\theta) = \left(\begin{array}{ccc}
 *   1 & \sin \theta &0 \\
 *   0  & 1 &0\\
 *   0 & 0 & 1
 *  \end{array}\right)\f$, and so one
*/
Mat2x33F32 GeometricalTransformation::shear2DHomogeneousCoordinate(F32 theta_radian, int coordinate)
{
   if(coordinate<0||coordinate>1)
       coordinate=0;
   Mat2x33F32 shear;
   if(coordinate==0){
       shear(0,0)=1;shear(0,1)=std::tan(theta_radian);shear(0,2)=0;
       shear(1,0)=0;shear(1,1)=1;shear(1,2)=0;
       shear(2,0)=0;shear(2,1)=0;shear(2,2)=1;
   }else{
       shear(0,0)=1;shear(0,1)=0;shear(0,2)=0;
       shear(1,0)=std::tan(theta_radian);shear(1,1)=1;shear(1,2)=0;
       shear(2,0)=0;shear(2,1)=0;shear(2,2)=1;
   }

 return shear;
}

}
