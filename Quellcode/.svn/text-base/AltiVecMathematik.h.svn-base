/**
 *
 *  Written in 2003 by Leo Fink (mail@leofink.de).
 *
 **/

#pragma once

//#include <math.h>
//#include <vecLib/vecLib.h>
#include <iostream>
//#include "AltiVecHilfsmittel.h"

// Was hiervon kann ersetzt werden durch Funktionen aus dem Accelerate.framework?


inline vector float rsqrt(const vector float& v)
{  
    const vector float _0 = (vector float)(-0.); 
	vector float y = vec_rsqrte(v); 

	y = vec_madd(vec_nmsub(v,vec_madd(y,y,_0),(vector float)(1.0)),vec_madd(y,(vector float)(0.5),_0),y);
	
	return y;
}

inline vector float sqrt(const vector float& v)
{
    const vector float _0 = (vector float)(-0.);
    return vec_sel(vec_madd(v,rsqrt(v),_0),_0,vec_cmpeq(v,_0));
    //return vec_madd(v,rsqrt(v),(-0.));
}

inline vector float log2(const vector float& x)
{
    const vector unsigned int SHIFT = (vector unsigned int)(23);
    const vector unsigned int BIAS = (vector unsigned int)(127);
    
    const vector float e = vec_ctf((vector signed int)vec_sub(vec_sr(vec_and((vector unsigned int)(x),(vector unsigned int)(0x7f800000)),SHIFT),BIAS),0);
    vector float m = (vector float)(vec_or(vec_and((vector unsigned int)(x),(vector unsigned int)(0x007FFFFF)),vec_sl(BIAS,SHIFT))); 
    
    m = vec_sub(m,(vector float)(1.));
    
    vector float y = vec_madd(m,(vector float)(-.0258411662),(vector float)(.1217970128));
    y = vec_madd(m,y,(vector float)(-.2779042655));
    y = vec_madd(m,y,(vector float)(.4575485901));
    y = vec_madd(m,y,(vector float)(-.7181451002));
    y = vec_madd(m,y,(vector float)(1.4425449290));
    
    return vec_madd(m,y,e);
}

inline vector float log(const vector float& x)
{
    return vec_madd(log2(x),(vector float)(.69314718056),(vector float)(-0.));
}

inline vector float exp2(const vector float& x)
{	
	const vector float n = vec_floor(x);
    const vector float r = vec_sub(x,n);
    
    vector float y = vec_madd(r,(vector float)(.0135557571),(vector float)(.0520323499));
    y = vec_madd(r,y,(vector float)(.2413797743));
    y = vec_madd(r,y,(vector float)(.6930321187));
    y = vec_madd(r,y,(vector float)(1.0));
    
    const vector float zhn = (vector float)(vec_sl(vec_add((vector signed int)(127),vec_cts(n,0)),(vector unsigned int)(23)));
    
    return vec_madd(y,zhn,(vector float)(-0.));
}

inline vector float exp(const vector float& x)
{
    return exp2(vec_madd(x,(vector float)(1.442695041),(vector float)(-0.)));
}

inline vector float pow(const vector float& x,const vector float& p)
{
    return exp2(vec_madd(p,log2(x),(vector float)(-0.)));
}

inline vector float cos(const vector float& x)
{
    const vector float _0 = (vector float)(-0.);
    const vector float H = (vector float)(.5);
    
    vector float r = vec_madd(x,(vector float)(.1591549431),_0);
    r = vec_abs(vec_sub(vec_sub(r,vec_floor(r)),H));
    r = vec_nmsub(r,(vector float)(2),H);
    
    vector unsigned int s = vec_andc((vector unsigned int)(0x80000000),(vector unsigned int)(r));
    
    r = vec_nmsub((vector float)(3.141592654),vec_abs(r),(vector float)(1.570796327));
    r = vec_madd(r,r,_0);
    
    vector float y = vec_madd(r,(vector float)(-2.755670761e-7),(vector float)(2.480158729e-5));
    y = vec_madd(y,r,(vector float)(-1.388888889e-3));
    y = vec_madd(y,r,(vector float)(4.166666667e-2));
    y = vec_madd(y,r,(vector float)(-.5));
    y = vec_madd(y,r,(vector float)(1.));
    
    return (vector float)(vec_xor((vector unsigned int)(y),s));
}


inline vector float sin(const vector float& x)
{
    const vector float _0 = (vector float)(-0.);
    const vector float H = (vector float)(.5);

    vector float r = vec_madd(x,(vector float)(.1591549431),_0);	
    r = vec_sub(vec_sub(r,vec_floor(r)),H);
    
    vector unsigned int s = vec_andc((vector unsigned int)(0x80000000),(vector unsigned int)(r));
    
    r = vec_abs(vec_nmsub(vec_abs(r),(vector float)(2),H));
    r = vec_nmsub((vector float)(3.141592654),r,(vector float)(1.570796327));
    
    const vector float r_ = r;
    r = vec_madd(r,r,_0);
    
    vector float y = vec_madd(r,(vector float)(-2.505163791e-8),(vector float)(2.755731922e-6));
    y = vec_madd(y,r,(vector float)(-1.984126984e-4));
    y = vec_madd(y,r,(vector float)(8.333333333e-3));
    y = vec_madd(y,r,(vector float)(-1.666666667e-1));
    y = vec_madd(y,r,(vector float)(1.));
    y = vec_madd(y,r_,_0);
    
    return (vector float)(vec_xor((vector unsigned int)(y),s));
}

//#include <Accelerate/Accelerate.h>

inline vector float zweiHochNteWurzel(const vector float& x, const vector signed int& N) // N = 0..20
{		
	const vector float zhmn = (vector float)(vec_sl(vec_sub((vector signed int)(127),N),(vector unsigned int)(23)));
	return exp2(vec_madd(log2(x),zhmn,(vector float)(-0.)));
	//return exp2(vec_madd(vlogf(x),zhmn,(vector float)(-0.))); // DEBUG
	//return vexpf(vec_madd(vlogf(x),zhmn,(vector float)(-0.))); // Probleme mit großen Zahlen und lahm
    //return vec_expte(vec_madd(vec_loge(x),zhmn,(vector float)(-0.))); // sehr ungenau
	
	//return vpowf(x,vscalbf((vector float)(1),vec_sub((vector int)(0),N)));
}



