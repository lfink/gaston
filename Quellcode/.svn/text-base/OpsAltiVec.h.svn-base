/*
 *  OpsAltiVec.h
 *  exprtemplatetest
 *
 *  Created by Leo Fink on 09.09.04.
 *  Copyright 2004 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef OPS_ALTIVEC_H
#define OPS_ALTIVEC_H

#include "Ops.h"

typedef vector bool int AltivecBool; // workaround, Compiler meckert sinnloserweise

template <> struct SIMDTraits<float> { typedef vector float typ; enum { multi=4 }; };
template <> struct SIMDTraits<int> { typedef vector signed int typ; enum { multi=4 }; };
template <> struct SIMDTraits<bool> { typedef AltivecBool typ; enum { multi=4 }; };

template <> struct BoolTraits<vector float> { typedef AltivecBool typ; };

//To DO:
/*struct Neg { template<> inline static vector float wendeAn(const vector float& a) { return vec_sub((vector float)(0),a); } };
struct Add { template<> inline static vector float wendeAn(const vector float& a, const vector float& b) { return vec_add(a,b); } };
struct Add { template<> inline static vector signed int wendeAn(const vector signed int& a, const vector signed int& b) { return vec_add(a,b); } };
struct Sub { template<> inline static vector float wendeAn(const vector float& a, const vector float& b) { return vec_sub(a,b); } };
struct Mul { template<> inline static vector float wendeAn(const vector float& a, const vector float& b) { return vec_madd(a,b,(vector float)(0)); } };
//struct Div { template<> inline static vector float wendeAn(const vector float& a, const vector float& b) { return a/b; } };
struct ZuwAdd { template<> inline static void wendeAn(vector float& a, const vector float& b) { a = vec_add(a,b); } };
struct ZuwSub { template<> inline static void wendeAn(vector float& a, const vector float& b) { a = vec_sub(a,b); } };
struct ZuwMul { template<> inline static void wendeAn(vector float& a, const vector float& b) { a = vec_madd(a,b,(vector float)(0)); } };
struct MulAdd { template<> inline static vector float wendeAn(const vector float& a, const vector float& b, const vector float& c) { return vec_madd(a,b,c); } };
struct MulSub { template<> inline static vector float wendeAn(const vector float& a, const vector float& b, const vector float& c) { return vec_nmsub(b,c,a); } };
struct Gleich { template<> inline static typename BoolTraits<vector float>::typ wendeAn(const vector float& a, const vector float& b) { return veq_eq(a,b); } };
struct Klein { template<> inline static typename BoolTraits<vector float>::typ wendeAn(const vector float& a, const vector float& b) { return vec_cmplt(a,b); } };
struct Groess { template<> inline static typename BoolTraits<vector float>::typ wendeAn(const vector float& a, const vector float& b) { return vec_cmpgt(a,b); } };
struct Und { template<> inline static vector bool wendeAn(const vector bool& a, const vector bool& b) { return vec_and(a,b); } };
struct Oder { template<> inline static vector bool wendeAn(const vector bool& a, const vector bool& b) { return vec_or(a,b); } };
//struct Nicht { template<> inline static vector bool wendeAn(const vector float& a) { return !a; } };
struct Max { template<> inline static vector float wendeAn(const vector float& a, const vector float& b) { return vec_max(a,b); } };
struct Min { template<> inline static vector float wendeAn(const vector float& a, const vector float& b) { return vec_min(a,b); } };
struct Ausw { template<> inline static vector float wendeAn(const typename BoolTraits<vector float>::typ& a, const vector float& b, const vector float& c) { return vec_sel(c,b,a); } };
struct Ausw { template<> inline static vector signed int wendeAn(const typename BoolTraits<vector signed int>::typ& a, const vector signed int& b, const vector signed int& c) { return vec_sel(c,b,a); } };*/

#endif
