/**
 *
 *  Written in 2003 by Leo Fink (mail@leofink.de).
 *
 **/

#pragma once


extern "C" {
#include <sys/sysctl.h>
}

inline bool istAltiVecVerfuegbar() 
{
	int selectors[2] = { CTL_HW, HW_VECTORUNIT };
	int hasVectorUnit = 0;
	size_t length = sizeof(hasVectorUnit);
	int error = sysctl(selectors,2,&hasVectorUnit,&length,NULL,0);
	
	if (error==0 && hasVectorUnit)
    {
        //vec_mtvscr(vec_or(vec_mfvscr(),(vector unsigned short)(0, 0, 0, 0, 0, 0, 1, 0))); // Non-Java-Mode
        return true;
    }
    else
		return false;
}


template <class VektorTyp,class SkalarTyp>
union S2V
{
	VektorTyp	v;
	SkalarTyp	s[4];
};

template <class VektorTyp,class SkalarTyp>	// besser vec_cast<...>() nennen?
inline VektorTyp vec(const SkalarTyp& s)
{
	S2V<VektorTyp,SkalarTyp> u;
	u.s[0]=s;
	return vec_splat(u.v,0);
}

template <class VektorTyp,class SkalarTyp>
inline VektorTyp vec(const SkalarTyp& s1,const SkalarTyp& s2,const SkalarTyp& s3,const SkalarTyp& s4)
{
	S2V<VektorTyp,SkalarTyp> u;
	u.s[0]=s1; u.s[1]=s2; u.s[2]=s3; u.s[3]=s4;
	return u.v;
}

template <class VektorTyp,class SkalarTyp>
inline VektorTyp ladeUnausgerichtet(SkalarTyp* const& inS)
{
    return vec_perm(vec_ld(0,inS),vec_ld(16,inS),vec_lvsl(0,inS));
}

template <class VektorTyp,class SkalarTyp>
inline void speichereUnausgerichtet(const VektorTyp& inV,SkalarTyp* outS)
{
	vector unsigned char v = (vector unsigned char)inV;
	v = vec_perm( v, v, vec_lvsr( 0, (unsigned char*)outS ) );
	vec_ste( (vector unsigned char) v,0, (unsigned char*) outS );
	vec_ste( (vector unsigned short)v,1,(unsigned short*) outS );
	vec_ste( (vector unsigned int)  v,3, (unsigned int*) outS );
	vec_ste( (vector unsigned int)  v,4, (unsigned int*) outS );
	vec_ste( (vector unsigned int)  v,8, (unsigned int*) outS );
	vec_ste( (vector unsigned int)  v,12, (unsigned int*) outS );
	vec_ste( (vector unsigned short)v,14,(unsigned short*) outS );
	vec_ste( (vector unsigned char) v,15,(unsigned char*) outS );
}


