/**
 *
 *  Written in 2003 by Leo Fink (mail@leofink.de).
 *
 **/

#pragma once

#include <cmath>
#include <ostream>


template<typename T>
class QuaternionT
{
    public:
        QuaternionT() {}
		template <typename T2> explicit QuaternionT(const QuaternionT<T2>& inQ) : r(inQ.r),i(inQ.i),j(inQ.j),k(inQ.k) {}
        QuaternionT(const T& inR,
                   const T& inI,
                   const T& inJ,
                   const T& inK) : r(inR),i(inI),j(inJ),k(inK) {}
        explicit QuaternionT(const T& inR) : r(inR),i(0),j(0),k(0) {}
        
        QuaternionT quadrat() const { const T& zweiR = r+r; return QuaternionT(r*r-i*i-j*j-k*k,zweiR*i,zweiR*j,zweiR*k); }
        void quadriereUndAddiere(const QuaternionT<float>& inZ);
        
        template <typename T2> QuaternionT operator+(const QuaternionT<T2>& inZ) const { return QuaternionT(r+inZ.r,i+inZ.i,j+inZ.j,k+inZ.k); }
        template <typename T2> QuaternionT operator-(const QuaternionT<T2>& inZ) const { return QuaternionT(r-inZ.r,i-inZ.i,j-inZ.j,k-inZ.k); }
        
        template <typename T2> void operator+=(const QuaternionT<T2>& inZ) { r+=inZ.r;i+=inZ.i;j+=inZ.j;k+=inZ.k; }
        template <typename T2> void operator-=(const QuaternionT<T2>& inZ) { r-=inZ.r;i-=inZ.i;j-=inZ.j;k-=inZ.k; }
        template <typename T2> void operator*=(const QuaternionT<T2>& inZ);
        
        T r,i,j,k;
} __attribute__ ((aligned (16))); // 16er-Ausrichtung praktisch für AltiVec

typedef QuaternionT<float> Quaternion;

template<typename T>
inline std::ostream& operator<<(std::ostream& inOutStream, const QuaternionT<T>& inZ)
{
    inOutStream << "(" << inZ.r << "," << inZ.i << "," << inZ.j << "," << inZ.k << ")";
    return inOutStream;
}

template<typename T>
inline void QuaternionT<T>::quadriereUndAddiere(const QuaternionT<float>& inZ)
{
	const T& zweiR = r+r;
	
	r = r*r-i*i-j*j-k*k+inZ.r;
	i = zweiR*i+inZ.i;
	j = zweiR*j+inZ.j;
	k = zweiR*k+inZ.k;
}

template<typename T>
template<typename T2>
inline void QuaternionT<T>::operator*=(const QuaternionT<T2>& inZ)
{ // Definiert als (*this)=inZ*(*this), Multiplikation nicht kommutativ
	
	const T& r_ = r*inZ.r-i*inZ.i-j*inZ.j-k*inZ.k;
    const T& i_ = r*inZ.i+i*inZ.r+j*inZ.k-k*inZ.j;
    const T& j_ = r*inZ.j-i*inZ.k+j*inZ.r+k*inZ.i;
    k = r*inZ.k+i*inZ.j-j*inZ.i+k*inZ.r;
    r=r_; i=i_; j=j_;
}

template<typename T>
inline QuaternionT<T> operator*(const float& inL, const QuaternionT<T>& inR)
{
    return QuaternionT<T>(inL*inR.r,inL*inR.i,inL*inR.j,inL*inR.k);
}

template<typename T>
inline QuaternionT<T> operator*(const T& inL, const QuaternionT<T>& inR)
{
    return QuaternionT<T>(inL*inR.r,inL*inR.i,inL*inR.j,inL*inR.k);
}

template<typename T>
inline QuaternionT<T> operator*(const QuaternionT<T>& inL, const float& inR)
{
    return QuaternionT<T>(inL.r*inR,inL.i*inR,inL.j*inR,inL.k*inR);
}

template<typename T>
inline QuaternionT<T> operator*(const QuaternionT<T>& inL, const T& inR)
{
    return QuaternionT<T>(inL.r*inR,inL.i*inR,inL.j*inR,inL.k*inR);
}

template<typename T>
inline QuaternionT<T> operator*(const QuaternionT<T>& inL, const QuaternionT<T>& inR)
{
	return QuaternionT<T>(inL.r*inR.r-inL.i*inR.i-inL.j*inR.j-inL.k*inR.k,
						  inL.r*inR.i+inL.i*inR.r+inL.j*inR.k-inL.k*inR.j,
						  inL.r*inR.j-inL.i*inR.k+inL.j*inR.r+inL.k*inR.i,
						  inL.r*inR.k+inL.i*inR.j-inL.j*inR.i+inL.k*inR.r);
}

template<typename T>
inline QuaternionT<T> operator*(const Quaternion& inL, const QuaternionT<T>& inR)
{
	return QuaternionT<T>(inL.r*inR.r-inL.i*inR.i-inL.j*inR.j-inL.k*inR.k,
						  inL.r*inR.i+inL.i*inR.r+inL.j*inR.k-inL.k*inR.j,
						  inL.r*inR.j-inL.i*inR.k+inL.j*inR.r+inL.k*inR.i,
						  inL.r*inR.k+inL.i*inR.j-inL.j*inR.i+inL.k*inR.r);
}

template<typename T>
inline QuaternionT<T> operator*(const QuaternionT<T>& inL, const Quaternion& inR)
{
	return QuaternionT<T>(inL.r*inR.r-inL.i*inR.i-inL.j*inR.j-inL.k*inR.k,
						  inL.r*inR.i+inL.i*inR.r+inL.j*inR.k-inL.k*inR.j,
						  inL.r*inR.j-inL.i*inR.k+inL.j*inR.r+inL.k*inR.i,
						  inL.r*inR.k+inL.i*inR.j-inL.j*inR.i+inL.k*inR.r);
}

inline Quaternion operator*(const Quaternion& inL, const Quaternion& inR)
{
	return Quaternion(inL.r*inR.r-inL.i*inR.i-inL.j*inR.j-inL.k*inR.k,
						  inL.r*inR.i+inL.i*inR.r+inL.j*inR.k-inL.k*inR.j,
						  inL.r*inR.j-inL.i*inR.k+inL.j*inR.r+inL.k*inR.i,
						  inL.r*inR.k+inL.i*inR.j-inL.j*inR.i+inL.k*inR.r);
}


template<typename T>
inline T sp(const QuaternionT<T>& inA,const QuaternionT<T>& inB)
{	// Skalarprodukt
    return inA.r*inB.r+inA.i*inB.i+inA.j*inB.j+inA.k*inB.k;
}

template<typename T>
inline QuaternionT<T> akh(const QuaternionT<T>& inA,const QuaternionT<T>& inB)
{	// halber Antikommutator: (a*b+b*a)/2
    return QuaternionT<T>(inA.r*inB.r-inA.i*inB.i-inA.j*inB.j-inA.k*inB.k,
                      inA.r*inB.i+inA.i*inB.r,
                      inA.r*inB.j+inA.j*inB.r,
                      inA.r*inB.k+inA.k*inB.r);
}

template<typename T>
inline QuaternionT<T> conj(const QuaternionT<T>& inZ)
{
    return QuaternionT<T>(inZ.r,-inZ.i,-inZ.j,-inZ.k);
}

template<typename T>
inline T norm(const QuaternionT<T>& inZ)
{
    return inZ.r*inZ.r+inZ.i*inZ.i+inZ.j*inZ.j+inZ.k*inZ.k;
}

template<typename T>
inline T abs(const QuaternionT<T>& inZ)
{
    return sqrt(norm(inZ));
}

