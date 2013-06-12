/*
 *  SSEMathematik.h
 *  Gaston
 *
 *  Created by Leo Fink on 05.07.07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#include <emmintrin.h>
#include <Accelerate/Accelerate.h>

inline vFloat log2(const vFloat& x)
{
    const int SHIFT = 23;
    const vUInt32 BIAS = _mm_set1_epi32(127);
    	
    const vFloat e = _mm_cvtepi32_ps(_mm_sub_epi32(_mm_srai_epi32(_mm_and_si128((vUInt32)(x),_mm_set1_epi32(0x7f800000)),SHIFT),BIAS));
    vFloat m = (vFloat)(_mm_or_si128(_mm_and_si128((vUInt32)(x),_mm_set1_epi32(0x007FFFFF)),_mm_slli_epi32(BIAS,SHIFT)));
    
    m = _mm_sub_ps(m,_mm_set1_ps(1.));
    
    vFloat y = _mm_add_ps(_mm_mul_ps(m,_mm_set1_ps(-.0258411662)),_mm_set1_ps(.1217970128));
    y = _mm_add_ps(_mm_mul_ps(m,y),_mm_set1_ps(-.2779042655));
    y = _mm_add_ps(_mm_mul_ps(m,y),_mm_set1_ps(.4575485901));
    y = _mm_add_ps(_mm_mul_ps(m,y),_mm_set1_ps(-.7181451002));
    y = _mm_add_ps(_mm_mul_ps(m,y),_mm_set1_ps(1.4425449290));
    
    return _mm_add_ps(_mm_mul_ps(m,y),e);
}

inline vFloat log(const vFloat& x)
{
    return _mm_mul_ps(log2(x),_mm_set1_ps(.69314718056));
}

inline vFloat exp2(const vFloat& x)
{	
    const vSInt32 n = _mm_cvttps_epi32(x);
    const vFloat r = _mm_sub_ps(x,_mm_cvtepi32_ps(n));
    
    vFloat y = _mm_add_ps(_mm_mul_ps(r,_mm_set1_ps(.0135557571)),_mm_set1_ps(.0520323499));
    y = _mm_add_ps(_mm_mul_ps(r,y),_mm_set1_ps(.2413797743));
    y = _mm_add_ps(_mm_mul_ps(r,y),_mm_set1_ps(.6930321187));
    y = _mm_add_ps(_mm_mul_ps(r,y),_mm_set1_ps(1.));
    
    const vFloat zhn = (vFloat)_mm_slli_epi32(_mm_add_epi32(_mm_set1_epi32(127),n),23);
    
    return _mm_mul_ps(y,zhn);
}

inline vFloat exp(const vFloat& x)
{
    return exp2(_mm_mul_ps(x,_mm_set1_ps(1.442695041)));
}

inline vFloat pow(const vFloat& x,const vFloat& p)
{
    return exp2(_mm_mul_ps(p,log2(x)));
}


inline vFloat zweiHochNteWurzel(const vFloat& x, const vSInt32& N) // N = 0..20
{		
	const vFloat zhmn = (vFloat)_mm_slli_epi32(_mm_sub_epi32(_mm_set1_epi32(127),N),23);
	
	return exp2(_mm_mul_ps(log2(x),zhmn));
}
