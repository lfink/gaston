/**
 *
 *  Written in 2003 by Leo Fink (mail@leofink.de).
 *
 **/

#pragma once

#include <cmath>
#include <ostream>
//#include "T.h"

//#include "Mathematik.h"
#include "Quaternion.h" // FŸr Rotationen

//using namespace std;

template<typename T>
class R3VektorT
{
    public:
		R3VektorT() : x(T()),y(T()),z(T()) {}
		R3VektorT(const R3VektorT& v) : x(v.x), y(v.y), z(v.z) {}
		template <typename T2> explicit R3VektorT(const R3VektorT<T2>& v) : x(v.x), y(v.y), z(v.z) {}
        R3VektorT(const T& x,
                    const T& y,
                    const T& z) : x(x), y(y), z(z) {}
					
        void		normiere();
        R3VektorT   normiert() const;
        void drehe(const T& inWinkel, const R3VektorT& inAchse);
        R3VektorT	gedreht(const T& inWinkel, const R3VektorT& inAchse) const;
        void        drehe(const QuaternionT<T>& inDrehung);
            
        template <typename T2> R3VektorT operator + (const R3VektorT<T2>& v) const { return R3VektorT(x+v.x,y+v.y,z+v.z); }
        template <typename T2> R3VektorT operator - (const R3VektorT<T2>& v) const { return R3VektorT(x-v.x,y-v.y,z-v.z); }
        R3VektorT	operator - () const {return R3VektorT(-x,-y,-z);}
        template <typename T2> void operator += (const R3VektorT<T2>& v) { x+=v.x;y+=v.y;z+=v.z; }
        template <typename T2> void operator -= (const R3VektorT<T2>& v) { x-=v.x;y-=v.y;z-=v.z; }
        
		R3VektorT	operator * (const T& s) const { return R3VektorT(x*s,y*s,z*s); }
        
		void        operator *= (const T& s) { x*=s;y*=s;z*=s; }
        inline R3VektorT	kreuzprodukt(const R3VektorT& v) const;
        inline T	operator * (const R3VektorT& v) const; // Skalarprodukt
        inline T	cosDesWinkelsZu(const R3VektorT& v) const;
        inline T	sinDesWinkelsZu(const R3VektorT& v) const;
        T		winkelZu(const R3VektorT& v) const { return acos(cosDesWinkelsZu(v)); }
        inline R3VektorT	projiziertAuf(const R3VektorT& v) const;
        
        T x,y,z;
};

typedef R3VektorT<float> R3Vektor;

template<typename T>
inline T norm(const R3VektorT<T>& v)
{
    return v.x*v.x+v.y*v.y+v.z*v.z;
}

template<typename T>
inline T abs(const R3VektorT<T>& v)
{
    return sqrt(v.x*v.x+v.y*v.y+v.z*v.z);
}

/*template<typename T>
inline R3VektorT<T> operator*(const R3VektorT<T>& v,const T& s)
{
    return R3VektorT<T>(v.x*s,v.y*s,v.z*s);
}*/

template<typename T>
inline R3VektorT<T> operator*(const T& s,const R3VektorT<T>& v)
{
    return R3VektorT<T>(v.x*s,v.y*s,v.z*s);
}

template<typename T>
inline T R3VektorT<T>::operator*(const R3VektorT<T>& v) const
{ // Skalarprodukt
    return x*v.x+y*v.y+z*v.z;
}

template<typename T>
inline R3VektorT<T> R3VektorT<T>::kreuzprodukt(const R3VektorT<T>& v) const
{
    return R3VektorT<T>(y*v.z-z*v.y,z*v.x-x*v.z,x*v.y-y*v.x);
}

template<typename T>
inline R3VektorT<T> kp(const R3VektorT<T>& v1,const R3VektorT<T>& v2)
{ // alternativ
    return R3VektorT<T>(v1.y*v2.z-v1.z*v2.y,v1.z*v2.x-v1.x*v2.z,v1.x*v2.y-v1.y*v2.x);
}

template<typename T>
inline R3VektorT<T> R3VektorT<T>::normiert() const
{
    const T& rl = T(1.f)/abs(*this);
    return R3VektorT<T>(x*rl,y*rl,z*rl);
}

template<typename T>
inline R3VektorT<T> R3VektorT<T>::gedreht(const T& inWinkel, const R3VektorT<T>& inAchse) const
{
	if (inWinkel==0 || !abs(inAchse)>0)
		return *this;
	
	T s = sin(inWinkel);
	T c = cos(inWinkel);
	T t = T(1)-c;
	const R3VektorT<T>& a = inAchse.normiert();
	
	R3VektorT<T> r((t*a.x*a.x+c)*x + (t*a.x*a.y+s*a.z)*y + (t*a.x*a.z-s*a.y)*z,
                (t*a.x*a.y-s*a.z)*x + (t*a.y*a.y+c)*y + (t*a.y*a.z+s*a.x)*z,
                (t*a.x*a.z+s*a.y)*x + (t*a.y*a.z-s*a.x)*y + (t*a.z*a.z+c)*z);
	
	return r;
}

template<typename T>
inline void R3VektorT<T>::drehe(const QuaternionT<T>& inDrehung)
{
    QuaternionT<T> q(conj(inDrehung)*QuaternionT<T>(0,x,y,z)*inDrehung);
    //return Quaternion(q.i,q.j,q.k);
    x = q.i;
    y = q.j;
    z = q.k;
}

template<typename T>
inline void R3VektorT<T>::normiere()
{
    T rl = T(1)/abs(*this);
    x = x*rl;
    y = y*rl;
    z = z*rl;
}

template<typename T>
inline void R3VektorT<T>::drehe(const T& inWinkel, const R3VektorT<T>& inAchse)
{
    *this = gedreht(inWinkel,inAchse.normiert());
}

template<typename T>
R3VektorT<T> R3VektorT<T>::projiziertAuf(const R3VektorT<T>& v) const
{
    return v*(operator*(v)/abs(v));
}

template<typename T>
inline T R3VektorT<T>::cosDesWinkelsZu(const R3VektorT<T>& v) const
{
    return operator*(v)/(abs(*this)*abs(v));
}

template<typename T>
inline T R3VektorT<T>::sinDesWinkelsZu(const R3VektorT<T>& v) const
{
    return abs(kreuzprodukt(v))/(abs(*this)*abs(v));
}

template<typename T>
inline std::ostream& operator<<(std::ostream& inOutStream, const R3VektorT<T>& inP)
{
    inOutStream << "(" << inP.x << "," << inP.y << "," << inP.z << ")";
    return inOutStream;
}

