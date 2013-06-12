#pragma once

#include <ostream>

#include "MetaProg.h"

#include <iostream>


using namespace std;

// Testweise ohne Expression Templates? Zumindest ohne Kompositoren?

template<typename T,int D>
class Vec
{
	public:
        inline Vec() __attribute__((always_inline)) {}
        
        template<typename A> inline /*explicit*/ Vec(const A& inA) __attribute__((always_inline)); // ohne explicit gefährlich? Aber wie sonst Ausdrücke zuweisen?
        
        template<typename A> inline const Vec<T,D>& operator=(const A& inA) restrict { Schleife<D,Vec<T,D>,A,Zuw>::fuehreAus(*this,inA); return *this; }
        template<typename A> inline void operator+=(const A& inA) restrict { Schleife<D,Vec<T,D>,A,ZuwAdd>::fuehreAus(*this,inA); }
        template<typename A> inline void operator-=(const A& inA) restrict { Schleife<D,Vec<T,D>,A,ZuwSub>::fuehreAus(*this,inA); }
        template<typename A> inline void operator*=(const A& inA) restrict { Schleife<D,Vec<T,D>,A,ZuwMul>::fuehreAus(*this,inA); }
                                
        inline Ausdr<Neg,Vec<T,D> > operator-() const __attribute__((always_inline)) { return Ausdr<Neg,Vec<T,D> >(*this); }
        
        template<typename A> inline Ausdr<Add,Vec<T,D>,A> operator+(const A& inA) const { return Ausdr<Add,Vec<T,D>,A>(*this,inA); } 
        template<typename A> inline Ausdr<Sub,Vec<T,D>,A> operator-(const A& inA) const { return Ausdr<Sub,Vec<T,D>,A>(*this,inA); } // global wegen Kompositoren
        template<typename A> inline Ausdr<Mul,Vec<T,D>,A> operator*(const A& inA) const { return Ausdr<Mul,Vec<T,D>,A>(*this,inA); }
        template<typename A> inline Ausdr<Div,Vec<T,D>,A> operator/(const A& inA) const { return Ausdr<Div,Vec<T,D>,A>(*this,inA); }
        
        template<typename A> inline Ausdr<Klein,Vec<T,D>,A> operator<(const A& inA) const { return Ausdr<Klein,Vec<T,D>,A>(*this,inA); }
        template<typename A> inline Ausdr<Groess,Vec<T,D>,A> operator>(const A& inA) const { return Ausdr<Groess,Vec<T,D>,A>(*this,inA); }

        inline Ausdr<Nicht,Vec<T,D> > operator!() const __attribute__((always_inline)) { return Ausdr<Nicht,Vec<T,D> >(*this); }
        template<typename A> inline Ausdr<Oder,Vec<T,D>,A> operator||(const A& inA) const { return Ausdr<Oder,Vec<T,D>,A>(*this,inA); }
        template<typename A> inline Ausdr<Und,Vec<T,D>,A> operator&&(const A& inA) const { return Ausdr<Und,Vec<T,D>,A>(*this,inA); }
        
        inline T& operator[](const int& inI) __attribute__((always_inline)) { return a[inI]; }
        inline const T& operator[](const int& inI) const __attribute__((always_inline)) { return a[inI]; }

    private:
        T a[D];
} __attribute__ ((aligned (16))); // 16er-Ausrichtung praktisch für AltiVec

template<typename T,int D>
template<typename A> inline Vec<T,D>::Vec(const A& inA) { Schleife<D,Vec<T,D>,A,Zuw>::fuehreAus(*this,inA); }

template <typename T,int D> struct Traits<Vec<T,D> >
{
    typedef T typ; enum { laenge=D };
    inline static typ& element(Vec<T,D>& restrict inA,const int& inI) { return inA[inI]; }
    inline static const typ& element(const Vec<T,D>& inA,const int& inI) { return inA[inI]; }
};

template<typename T,int D>
std::ostream& operator<<(std::ostream& inOutStream, const Vec<T,D>& inF)
{
    if (D>1)
		inOutStream << "[";
    
    for (int i=0;i<D-1;++i)
        inOutStream << inF[i] << ",";
    
    inOutStream << inF[D-1];
	
	if (D>1)
		inOutStream  << "]";
    
    return inOutStream;
}

template<typename T,int D> inline Ausdr<Add,T,Vec<T,D> > operator+(const T& inA,const Vec<T,D>& inB) { return Ausdr<Add,T,Vec<T,D> >(inA,inB); }
template<typename T,int D> inline Ausdr<Sub,T,Vec<T,D> > operator-(const T& inA,const Vec<T,D>& inB) { return Ausdr<Sub,T,Vec<T,D> >(inA,inB); }
template<typename T,int D> inline Ausdr<Mul,T,Vec<T,D> > operator*(const T& inA,const Vec<T,D>& inB) { return Ausdr<Mul,T,Vec<T,D> >(inA,inB); }
template<typename T,int D> inline Ausdr<Div,T,Vec<T,D> > operator/(const T& inA,const Vec<T,D>& inB) { return Ausdr<Div,T,Vec<T,D> >(inA,inB); }
template<typename T,int D> inline Ausdr<Und,T,Vec<T,D> > operator&&(const T& inA,const Vec<T,D>& inB) { return Ausdr<Und,T,Vec<T,D> >(inA,inB); }
template<typename T,int D> inline Ausdr<Oder,T,Vec<T,D> > operator||(const T& inA,const Vec<T,D>& inB) { return Ausdr<Oder,T,Vec<T,D> >(inA,inB); }
template<typename T,int D> inline Ausdr<Klein,T,Vec<T,D> > operator<(const T& inA,const Vec<T,D>& inB) { return Ausdr<Klein,T,Vec<T,D> >(inA,inB); }
template<typename T,int D> inline Ausdr<Groess,T,Vec<T,D> > operator>(const T& inA,const Vec<T,D>& inB) { return Ausdr<Groess,T,Vec<T,D> >(inA,inB); }

template<typename A,typename B,typename C> inline Ausdr<Ausw,A,B,C> auswahl(const A& inBedingung,const B& inWahr,const C& inFalsch) { return Ausdr<Ausw,A,B,C>(inBedingung,inWahr,inFalsch); }
template<typename A,typename B> inline Ausdr<Max,A,B> maximum(const A& inA,const B& inB) { return Ausdr<Max,A,B>(inA,inB); }
template<typename A,typename B> inline Ausdr<Min,A,B> minimum(const A& inA,const B& inB) { return Ausdr<Min,A,B>(inA,inB); }
template<typename A> inline bool teilweiseWahr(const A& inBedingung) { return Verkettung<Traits<A>::laenge,A,Oder>::fuehreAus(inBedingung); }
template<typename A> inline bool vollstaendigWahr(const A& inBedingung) { return Verkettung<Traits<A>::laenge,A,Und>::fuehreAus(inBedingung); }


