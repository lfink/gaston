/**
 *
 *  Written in 2003 by Leo Fink (mail@leofink.de).
 *
 **/
 
#pragma once

// PowerPC 970: Skalares Optimum bei 3x1 oder 2x2, AltiVec bei 4x2 oder 4x3
// PowerPC 750: Skalares Optimum bei 2x1
// Intel Core2: Skalares Optimum bei 1x1 oder 2x1, SSE bei 2x2

#if !defined PARX || !defined PARY 
	#if __ALTIVEC__
		#define PARX 4
		#define PARY 2
	#elif __SSE2__
		#define PARX 2
		#define PARY 2
	#else
		#define PARX 2
		#define PARY 1
	#endif
#endif

#define PARALLEL (PARX*PARY)

// Folgendes sollte nicht zentral sein

#include "Vec.h"
#include "R3Vektor.h"
#include "E3Punkt.h"
#include "Quaternion.h"
#include "Farbe.h"

typedef Vec<bool,PARALLEL> BoolVec;
typedef Vec<int,PARALLEL> IntVec;
typedef Vec<float,PARALLEL> FloatVec;
typedef R3VektorT<FloatVec> R3VektorVec;
typedef E3PunktT<FloatVec> E3PunktVec;
typedef QuaternionT<FloatVec> QuaternionVec;
typedef Vec<Farbe,PARALLEL> FarbVec; // Farbe auch "umdrehen" in Template wie die anderen?



