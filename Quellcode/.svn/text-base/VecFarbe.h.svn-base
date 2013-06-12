/*
 *  VecFarbe.h
 *  Gaston
 *
 *  Created by Leo Fink on 05.09.04.
 *  Copyright 2004 __MyCompanyName__. All rights reserved.
 *
 */
 
#include "Farbe.h"

/*template <int D>
class Vec<Farbe,D>
{
  public:
    Vec() {}
    Vec(const Vec<E3Punkt,D>& inV) : x(inV.x),y(inV.y),z(inV.z) {}
    Vec(const E3Punkt& inT) : x(inT.x),y(inT.y),z(inT.z) {}
    Vec(const Vec<float,D> inX,const Vec<float,D> inY,const Vec<float,D> inZ) : x(inX),y(inY),z(inZ) {}

    void operator-=(const Vec<R3Vektor,D>& inV) { x-=inV.x; y-=inV.y; z-=inV.z; }
    
    FloatVec x,y,z;
};*/

template<int D>
inline Vec<Farbe,D> operator*(const Vec<Farbe,D>& inA, const Vec<float,D>& inB)
{   
    Vec<Farbe,D> v;
    
    for (int i=0;i<D;++i)
        v[i] = inA[i]*inB[i];
        
    return v;
}

template<int D>
inline Vec<Farbe,D> operator*(const Vec<Farbe,D>& inA, const float& inB)
{   
    Vec<Farbe,D> v;
    
    for (int i=0;i<D;++i)
        v[i] = inA[i]*inB;
        
    return v;
}

template<int D>
inline Vec<Farbe,D> operator*(const Farbe& inA, const Vec<float,D>& inB)
{   
    Vec<Farbe,D> v;
    
    for (int i=0;i<D;++i)
        v[i] = inA*inB[i];
        
    return v;
}



