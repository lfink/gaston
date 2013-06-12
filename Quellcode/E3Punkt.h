/**
 *
 *  Written in 2003 by Leo Fink (mail@leofink.de).
 *
 **/

#pragma once
    
#include "R3Vektor.h"
#include <ostream>

template<typename T>
class E3PunktT
{
    public:
		E3PunktT() : x(T()),y(T()),z(T()) {}
		E3PunktT(const E3PunktT& p) : x(p.x), y(p.y), z(p.z) {}
		template <typename T2> explicit E3PunktT(const E3PunktT<T2>& p) : x(p.x), y(p.y), z(p.z) {}
        E3PunktT(const T& x,
                    const T& y,
                    const T& z) : x(x), y(y), z(z) {}

        inline void drehe(const T& inWinkel, const R3VektorT<T>& inAchse);
        inline E3PunktT gedreht(const T& inWinkel, const R3VektorT<T>& inAchse) const;
        
        template <typename T2> E3PunktT operator + (const R3VektorT<T2>& v) const { return E3PunktT(x+v.x,y+v.y,z+v.z); }
        template <typename T2> void operator += (const R3VektorT<T2>& v) { x+=v.x;y+=v.y;z+=v.z; }
        template <typename T2> void operator -= (const R3VektorT<T2>& v) { x-=v.x;y-=v.y;z-=v.z; }
        template <typename T2> E3PunktT operator - (const R3VektorT<T2>& v) const { return E3PunktT(x-v.x,y-v.y,z-v.z); }
        template <typename T2> R3VektorT<T>    operator - (const E3PunktT<T2>& p) const { return R3VektorT<T>(x-p.x,y-p.y,z-p.z); }
                                                    
        R3VektorT<T>    alsOrtsvektor() const { return R3VektorT<T>(x,y,z); }
        
        T     x,y,z;
};

typedef E3PunktT<float> E3Punkt;

template<typename T>
inline void E3PunktT<T>::drehe(const T& inWinkel, const R3VektorT<T>& inAchse)
{
    *this = gedreht(inWinkel,inAchse.normiert());
}

template<typename T>
inline E3PunktT<T> E3PunktT<T>::gedreht(const T& inWinkel, const R3VektorT<T>& inAchse) const
{
	if (inWinkel==0 || !abs(inAchse)>0)
		return *this;
		
	T s = sin(inWinkel);
	T c = cos(inWinkel);
	T t = T(1)-c;
	const R3Vektor a = inAchse.normiert();
	
	E3PunktT<T> r((t*a.x*a.x+c)*x + (t*a.x*a.y+s*a.z)*y + (t*a.x*a.z-s*a.y)*z,
                (t*a.x*a.y-s*a.z)*x + (t*a.y*a.y+c)*y + (t*a.y*a.z+s*a.x)*z,
                (t*a.x*a.z+s*a.y)*x + (t*a.y*a.z-s*a.x)*y + (t*a.z*a.z+c)*z);
	
	return r;
}

template<typename T>
inline std::ostream& operator<<(std::ostream& inOutStream, const E3PunktT<T>& inP)
{
    inOutStream << "(" << inP.x << "," << inP.y << "," << inP.z << ")";
    return inOutStream;
}

template<typename T>
class Quader
{
    public:
        Quader(const E3PunktT<T>&  inMitte,
                const R3VektorT<T>& inRadius) : mitte(inMitte), radius(inRadius) {}

        E3PunktT<T>    mitte;
        R3VektorT<T>    radius;
};


