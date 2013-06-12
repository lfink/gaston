#pragma once

#include "Mathematik.h"

struct RSqrt { template <typename T> inline static T wendeAn(const T& a) { return rsqrt(a); } };
template<typename A>
inline Ausdr<RSqrt,A> rsqrt(const A& inA)
{   
    return Ausdr<RSqrt,A>(inA);
}

struct Sqrt { template <typename T> inline static T wendeAn(const T& a) { return sqrt(a); } };
template<typename A>
inline Ausdr<Sqrt,A> sqrt(const A& inA)
{
    return Ausdr<Sqrt,A>(inA);
}

struct Potenz { template<typename T,typename I> inline static T wendeAn(const T& a, const I& n) { return pow(a,n); } };
template<typename A,typename B>
inline Ausdr<Potenz,A,B> pow(const A& inA,const B& inB)
{   
    return Ausdr<Potenz,A,B>(inA,inB);
}

struct Abs { template <typename T> inline static T wendeAn(const T& a) { return abs(a); } };
template<typename A>
inline Ausdr<Abs,A> abs(const A& inA)
{
    return Ausdr<Abs,A>(inA);
}

struct ZweiHochNteWurzel { template<typename T,typename I> inline static T wendeAn(const T& a, const I& n) { return zweiHochNteWurzel(a,n); } };
template<typename A,typename B>
inline Ausdr<ZweiHochNteWurzel,A,B> zweiHochNteWurzel(const A& inA,const B& inB)
{   
    return Ausdr<ZweiHochNteWurzel,A,B>(inA,inB);
}

