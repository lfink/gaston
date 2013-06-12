#pragma once

#include "Ops.h"

/////////////// Traits //////////////////

template <typename T>
struct Traits
{
    typedef T typ;
    enum { laenge=1 };
    inline static T& element(T& restrict t,const int& i) __attribute__((always_inline)) { return t; }
    inline static const T& element(const T& restrict t,const int& i) __attribute__((always_inline)) { return t; }
};

/////////////// Wiederholungen ////////////////

template<int D,typename A,typename B,typename Op>
struct Schleife
{
    inline static void fuehreAus(A& restrict a, const B& b) __attribute__((always_inline))
    {
        Schleife<D-1,A,B,Op>::fuehreAus(a,b); 
//		Op::wendeAn(Traits<A>::element(a,D-1),typename Traits<A>::typ(Traits<B>::element(b,D-1))); // cast nötig? Praktisch bei float a = int(1) etc.
		Op::wendeAn(Traits<A>::element(a,D-1),Traits<B>::element(b,D-1)); 

		//for (int i=0;i<D;++i) Op::wendeAn(Traits<A>::element(a,i),typename Traits<A>::typ(Traits<B>::element(b,i)));
    }
};

template<typename A,typename B,typename Op>
struct Schleife<0,A,B,Op>
{
    inline static void fuehreAus(A& restrict, const B&) __attribute__((always_inline)) {}
};

template<int D,typename A,typename Op>
struct Verkettung
{
    inline static typename Traits<A>::typ fuehreAus(const A& a) __attribute__((always_inline))
    {
        return Op::wendeAn(a[D-1],Verkettung<D-1,A,Op>::fuehreAus(a));
        /*typename Traits<A>::typ r = a[0];
        for (int i=1;i<D;++i) r = Op::wendeAn(r,a[i]);
        return r;*/
    }
};

template<typename A,typename Op>
struct Verkettung<1,A,Op>
{
    inline static typename Traits<A>::typ fuehreAus(const A& a) __attribute__((always_inline)) { return a[0]; }
};


/////////////// Kompositoren ////////////////

template<typename Op,typename A,typename B=void,typename C=void>
class Ausdr
{
    private:
    public:
        inline explicit Ausdr(const A& inA,const B& inB,const C& inC) __attribute__((always_inline)) : a(inA),b(inB),c(inC) {}
        
        inline typename Traits<Ausdr<Op,A,B,C> >::typ operator[](const int& inI) const restrict __attribute__((always_inline))
        {
            return Op::wendeAn(Traits<A>::element(a,inI),Traits<B>::element(b,inI),Traits<C>::element(c,inI));
        }

        const A& a;
        const B& b;
        const C& c;
};

template<typename Op,typename A,typename B>
class Ausdr<Op,A,B,void>
{
    public:
        inline explicit Ausdr(const A& inA,const B& inB) __attribute__((always_inline)) : a(inA),b(inB) {}
        
        inline typename Traits<Ausdr<Op,A,B> >::typ operator[](const int& inI) const restrict __attribute__((always_inline))
        {
            //return Op::wendeAn(Traits<A>::element(a,inI),Traits<A>::typ(Traits<B>::element(b,inI))); // cast nötig? Praktisch bei float a = int(1) etc.
            return Op::wendeAn(Traits<A>::element(a,inI),Traits<B>::element(b,inI));
        }
    
        const A& a;
        const B& b;
};

template<typename Op,typename A>
class Ausdr<Op,A,void,void>
{
    public:
        explicit Ausdr(const A&  inA) : a(inA) {}
        
        inline typename Traits<Ausdr<Op,A> >::typ operator[](const int& inI) const restrict __attribute__((always_inline))
        {
            return Op::wendeAn(Traits<A>::element(a,inI));
        }
    
        const A& a;
};

template<typename Op,typename A,typename B,typename C>
std::ostream& operator<<(std::ostream& inOutStream, const Ausdr<Op,A,B,C>& inA)
{
    inOutStream << "[";
    
    for (int i=0;i<int(Traits<A>::laenge)-1;++i)
        inOutStream << inA[i] << ",";
    
    inOutStream << inA[int(Traits<A>::laenge)-1] << "]";
    
    return inOutStream;
}

#define T typename Traits<Ausdr<Op,A,B,C> >::typ

template<typename Op,typename A,typename B,typename C> inline Ausdr<Add,T,Ausdr<Op,A,B,C> > operator+(const T& inA,const Ausdr<Op,A,B,C>& inB) { return Ausdr<Add,T,Ausdr<Op,A,B,C> >(inA,inB); }
template<typename Op,typename A,typename B,typename C,typename X> inline Ausdr<Add,Ausdr<Op,A,B,C>,X> operator+(const Ausdr<Op,A,B,C>& inA,const X& inB) { return Ausdr<Add,Ausdr<Op,A,B,C>,X>(inA,inB); }
template<typename Op,typename A,typename B,typename C> inline Ausdr<Sub,T,Ausdr<Op,A,B,C> > operator-(const T& inA,const Ausdr<Op,A,B,C>& inB) { return Ausdr<Sub,T,Ausdr<Op,A,B,C> >(inA,inB); }
template<typename Op,typename A,typename B,typename C,typename X> inline Ausdr<Sub,Ausdr<Op,A,B,C>,X> operator-(const Ausdr<Op,A,B,C>& inA,const X& inB) { return Ausdr<Sub,Ausdr<Op,A,B,C>,X>(inA,inB); }
template<typename Op,typename A,typename B,typename C> inline Ausdr<Mul,T,Ausdr<Op,A,B,C> > operator*(const T& inA,const Ausdr<Op,A,B,C>& inB) { return Ausdr<Mul,T,Ausdr<Op,A,B,C> >(inA,inB); }
template<typename Op,typename A,typename B,typename C,typename X> inline Ausdr<Mul,Ausdr<Op,A,B,C>,X> operator*(const Ausdr<Op,A,B,C>& inA,const X& inB) { return Ausdr<Mul,Ausdr<Op,A,B,C>,X>(inA,inB); }
template<typename Op,typename A,typename B,typename C> inline Ausdr<Div,T,Ausdr<Op,A,B,C> > operator/(const T& inA, const Ausdr<Op,A,B,C>& inB) { return Ausdr<Div,T,Ausdr<Op,A,B,C> >(inA,inB); }
template<typename Op,typename A,typename B,typename C,typename X> inline Ausdr<Div,Ausdr<Op,A,B,C>,X> operator/(const Ausdr<Op,A,B,C>& inA,const X& inB) { return Ausdr<Div,Ausdr<Op,A,B,C>,X>(inA,inB); }
template<typename Op,typename A,typename B,typename C> inline Ausdr<Und,T,Ausdr<Op,A,B,C> > operator&&(const T& inA, const Ausdr<Op,A,B,C>& inB) { return Ausdr<Und,T,Ausdr<Op,A,B,C> >(inA,inB); }
template<typename Op,typename A,typename B,typename C,typename X> inline Ausdr<Und,Ausdr<Op,A,B,C>,X> operator&&(const Ausdr<Op,A,B,C>& inA,const X& inB) { return Ausdr<Und,Ausdr<Op,A,B,C>,X>(inA,inB); }
template<typename Op,typename A,typename B,typename C> inline Ausdr<Oder,T,Ausdr<Op,A,B,C> > operator||(const T& inA, const Ausdr<Op,A,B,C>& inB) { return Ausdr<Oder,T,Ausdr<Op,A,B,C> >(inA,inB); }
template<typename Op,typename A,typename B,typename C,typename X> inline Ausdr<Oder,Ausdr<Op,A,B,C>,X> operator||(const Ausdr<Op,A,B,C>& inA,const X& inB) { return Ausdr<Oder,Ausdr<Op,A,B,C>,X>(inA,inB); }
template<typename Op,typename A,typename B,typename C> inline Ausdr<Klein,T,Ausdr<Op,A,B,C> > operator<(const T& inA, const Ausdr<Op,A,B,C>& inB) { return Ausdr<Klein,T,Ausdr<Op,A,B,C> >(inA,inB); }
template<typename Op,typename A,typename B,typename C,typename X> inline Ausdr<Klein,Ausdr<Op,A,B,C>,X> operator<(const Ausdr<Op,A,B,C>& inA,const X& inB) { return Ausdr<Klein,Ausdr<Op,A,B,C>,X>(inA,inB); }
template<typename Op,typename A,typename B,typename C> inline Ausdr<Groess,T,Ausdr<Op,A,B,C> > operator>(const T& inA, const Ausdr<Op,A,B,C>& inB) { return Ausdr<Groess,T,Ausdr<Op,A,B,C> >(inA,inB); }
template<typename Op,typename A,typename B,typename C,typename X> inline Ausdr<Groess,Ausdr<Op,A,B,C>,X> operator>(const Ausdr<Op,A,B,C>& inA,const X& inB) { return Ausdr<Groess,Ausdr<Op,A,B,C>,X>(inA,inB); }

#undef T

// Spezialisierungen von Ausdrücken (Multiply-Add a+b*c, Multiply-Sub c-a*b)
template<typename Op,typename A,typename B,typename C,typename A2,typename B2> inline Ausdr<MulAdd,A2,B2,Ausdr<Op,A,B,C> > operator+(const Ausdr<Op,A,B,C>& inA,const Ausdr<Mul,A2,B2>& inB) { return Ausdr<MulAdd,A2,B2,Ausdr<Op,A,B,C> >(inB.a,inB.b,inA); }
template<typename Op,typename A,typename B,typename C,typename A2,typename B2> inline Ausdr<MulSub,Ausdr<Op,A,B,C>,A2,B2> operator-(const Ausdr<Op,A,B,C>& inA,const Ausdr<Mul,A2,B2>& inB) { return Ausdr<MulSub,Ausdr<Op,A,B,C>,A2,B2>(inA,inB.a,inB.b); }

//////// Spezialisierungen: Rückgabe

template <typename Op,typename A,typename B,typename C>
struct Traits<Ausdr<Op,A,B,C> >
{
    typedef typename Traits<A>::typ typ;
    enum { laenge=Traits<A>::laenge };
    inline static typ element(const Ausdr<Op,A,B,C>& inA,const int& inI) __attribute__((always_inline)) { return inA[inI]; }
};

template <typename A,typename B>
struct Traits<Ausdr<Klein,A,B> >
{
    typedef bool typ;
    enum { laenge=Traits<A>::laenge };
    inline static typ element(const Ausdr<Klein,A,B>& inA,const int& inI) __attribute__((always_inline)) { return inA[inI]; }
};

template <typename A,typename B>
struct Traits<Ausdr<Groess,A,B> >
{
    typedef bool typ;
    enum { laenge=Traits<A>::laenge };
    inline static typ element(const Ausdr<Groess,A,B>& inA,const int& inI) __attribute__((always_inline)) { return inA[inI]; }
};

template <typename A,typename B,typename C>
struct Traits<Ausdr<Ausw,A,B,C> >
{
    typedef typename Traits<B>::typ typ;
    enum { laenge=Traits<A>::laenge };
    inline static typ element(const Ausdr<Ausw,A,B,C>& inA,const int& inI) __attribute__((always_inline)) { return inA[inI]; }
};


