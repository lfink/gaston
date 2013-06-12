/*
 *  Ops.h
 *  exprtemplatetest
 *
 *  Created by Leo Fink on 09.09.04.
 *  Copyright 2004 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef OPS_H
#define OPS_H

#define restrict  // Hat wahrscheinlich kaum Wirkung

template <typename T> struct SIMDTraits { typedef T typ; enum { multi=1 }; };
template <typename T> struct BoolTraits { typedef bool typ; };

struct Zuw { template<typename T> inline static void wendeAn(T& restrict a, const T& b) { a=b; } };
struct Neg { template<typename T> inline static T wendeAn(const T& a) { return -a; } };
struct Add { template<typename T> inline static T wendeAn(const T& a, const T& b) { return a+b; } };
struct Sub { template<typename T> inline static T wendeAn(const T& a, const T& b) { return a-b; } };
struct Mul { template<typename T> inline static T wendeAn(const T& a, const T& b) { return a*b; } };
struct Div { template<typename T> inline static T wendeAn(const T& a, const T& b) { return a/b; } };
struct ZuwAdd { template<typename T> inline static void wendeAn(T& restrict a, const T& b) { a+=b; } };
struct ZuwSub { template<typename T> inline static void wendeAn(T& restrict a, const T& b) { a-=b; } };
struct ZuwMul { template<typename T> inline static void wendeAn(T& restrict a, const T& b) { a*=b; } };
struct MulAdd { template<typename T> inline static T wendeAn(const T& a, const T& b, const T& c) { return a*b+c; } };
struct MulSub { template<typename T> inline static T wendeAn(const T& a, const T& b, const T& c) { return a-b*c; } };
struct Gleich { template<typename T> inline static typename BoolTraits<T>::typ wendeAn(const T& a, const T& b) { return a==b; } };
struct Klein { template<typename T> inline static typename BoolTraits<T>::typ wendeAn(const T& a, const T& b) { return a<b; } };
struct Groess { template<typename T> inline static typename BoolTraits<T>::typ wendeAn(const T& a, const T& b) { return a>b; } };
struct Und { template<typename T> inline static T wendeAn(const T& a, const T& b) { return a&&b; } };
struct Oder { template<typename T> inline static T wendeAn(const T& a, const T& b) { return a||b; } };
struct Nicht { template<typename T> inline static T wendeAn(const T& a) { return !a; } };
struct Max { template<typename T> inline static T wendeAn(const T& a, const T& b) { return max(a,b); } };
struct Min { template<typename T> inline static T wendeAn(const T& a, const T& b) { return min(a,b); } };
struct Ausw { template<typename T> inline static T wendeAn(const typename BoolTraits<T>::typ& a, const T& b, const T& c) { return a?b:c; } };

#endif

#if __ALTIVEC__

//#include "OpsAltiVec.h"

#endif
