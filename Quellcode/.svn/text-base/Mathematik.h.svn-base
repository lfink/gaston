/**
 *
 *  Written in 2003 by Leo Fink (mail@leofink.de).
 *
 **/

#pragma once



union Float2Int
{
	unsigned long i;
    float f;
};

inline float log2_approx(const float& x)
{
	Float2Int u;
	
	u.f = x;
	const int e = ((u.i & 0x7f800000) >> 23) - 127; 
	u.i = (u.i & 0x007FFFFF) | (127 << 23);
	
	float m = u.f-1.f;
    
    return (((((-.0258411662f*m+.1217970128f)*m-.2779042655f)*m+.4575485901f)*m-.7181451002f)*m+1.4425449290f)*m+e;
	
	// noch ungenauer, noch schneller
	/*float m = x;
	int* mp = (int*)&m;
    int y = *mp;
    int e = ((y >> 23) & 255) - 128;
    y &= ~(255 << 23);
    y += 127 << 23;
    *mp = y;
 	
    return m+e;*/
}

inline float log_approx(const float& x)
{	
	return log2_approx(x)*.6931471806f;
}

inline float exp2_approx(const float& x)
{
	const long	i = long(x); // Apple sagt, das dauert aufm G5.
	const float  r = x-i;
	
	Float2Int zhn;
	
	zhn.i = (127+i) << 23;
	
	return (1.f+r*(0.6930321187f+r*(0.2413797743f+r*(0.0520323499f+r*0.0135557571f))))*zhn.f;
	
	//return (1.f+r*(0.6930321187f+r*(0.2413797743f+r*(0.0520323499f+r*0.0135557571f))))*(1 << i); // nur für positive x
}

inline float exp_approx(const float& x)
{
	return exp2_approx(x*1.442695041f);
}

inline float pow_approx(const float& x,const float& p)
{
	return exp2_approx(p*log2_approx(x));
}

inline float zweiHochNteWurzel(const float& x, const int& N) // N = 0..20
{	
	return pow_approx(x,1.f/float(1 << N));
    
	//return pow(x,1.f/float(1 << N));
}

