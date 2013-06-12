/**
*
 *  Written in 2003 by Leo Fink (mail@leofink.de).
 *
 **/

#pragma once


#include "Parallel.h"
#include "VecMath.h"

#define MAX_FLOAT (__FLT_MAX__)

class Juliamenge
{
public:
    Juliamenge(const int& inIterationslimit,
               const Quaternion& inKonstante,
               const float& inWinkel);
    
	virtual ~Juliamenge() {}
	
	int iterationslimit() const { return mIterationslimit; }
    void setzeIterationslimit(const int& inN);
    void setzeKonstante(const Quaternion& inC);
    void setzeWinkel(const float& inWinkel);
    
    BoolVec liegtInnerhalb(const QuaternionVec& inZ) const;
    QuaternionVec gradient(const QuaternionVec& inZ) const;
    template <int D> Vec<float,D> inline abstand(const QuaternionT<Vec<float,D> >& inZ) const;
    
protected:    
    Quaternion	mKonstante;
    Quaternion	mRotor;
    int			mIterationslimit;
	//float		mX;
};

/*template<int D>
inline Vec<float,D> Juliamenge::abstand(const QuaternionT<Vec<float,D> >& inZ) const
{
	const Quaternion	c(mKonstante),r(mRotor);
	const int			n = mIterationslimit;
	QuaternionVec		z(inZ);
	    
	for (int i=0;i<n;i++)
    {
        z.quadriereUndAddiere(c);
        z *= r;
    }
	
	const FloatVec& f = norm(z);
	const FloatVec& a = zweiHochNteWurzel(f,n)-zweiHochNteWurzel(4.f,n);
	
    return minimum(a,.5f);
}*/

template<int D>
inline Vec<float,D> Juliamenge::abstand(const QuaternionT<Vec<float,D> >& inZ) const
{
	QuaternionVec		z(inZ);
	const Quaternion	c(mKonstante),r(mRotor);
	FloatVec			f;
	IntVec				n(1);
	    
	for (int i=0;i<mIterationslimit;i++)
    {
        z.quadriereUndAddiere(c);
        z *= r;
        
        const FloatVec& f_ = norm(z);
        const BoolVec& m = f_< MAX_FLOAT; //(0x1.FFFFFEp+127f);
		//const BoolVec& m = f_<65536.f; // TODO: Werte ausprobieren
		        
        //if (!teilweiseWahr(m))
   		//	break;
				
        f = auswahl(m,f_,f);
		n = auswahl(m,n+1,n);
    }
		    			
    return zweiHochNteWurzel(f,n)-zweiHochNteWurzel(4.f,n);
}


#if __ALTIVEC__

#include "AltiVecHilfsmittel.h"
#include "AltiVecMathematik.h"


template<>
inline Vec<float,4> Juliamenge::abstand(const QuaternionT<Vec<float,4> >& inZ) const
{ // Spezialisiert auf 4, auskodiert
	const vector float MAX_NORM = (vector float)(MAX_FLOAT);
	const vector float _0 = (vector float)(-0.);
    const vector signed int _1 = (vector signed int)(1);
    
	vector float zr=vec_ld(0,&inZ.r[0]),
				 zi=vec_ld(0,&inZ.i[0]),
				 zj=vec_ld(0,&inZ.j[0]),
				 zk=vec_ld(0,&inZ.k[0]);
    const vector float dieKonst = vec_ld(0,&mKonstante.r);
    const vector float derRot = vec_ld(0,&mRotor.r);
    const vector float kr=vec_splat(dieKonst,0), // splats im Code?
					   ki=vec_splat(dieKonst,1),
					   kj=vec_splat(dieKonst,2),
					   kk=vec_splat(dieKonst,3);
    const vector float rr=vec_splat(derRot,0),
					   ri=vec_splat(derRot,1),
					   rj=vec_splat(derRot,2),
					   rk=vec_splat(derRot,3);
    
    vector float f;
	vector signed int n=_1;
    
	for (int i=0;i<mIterationslimit;i++)
    {
        const vector float& zweiR=vec_add(zr,zr);
    
        zr = vec_nmsub(zk,zk,vec_nmsub(zj,zj,vec_nmsub(zi,zi,vec_madd(zr,zr,kr))));
        zi = vec_madd(zweiR,zi,ki);
        zj = vec_madd(zweiR,zj,kj);
        zk = vec_madd(zweiR,zk,kk);
     
        const vector float& zr_ = vec_nmsub(rk,zk,vec_nmsub(rj,zj,vec_nmsub(ri,zi,vec_madd(rr,zr,_0))));
        const vector float& zi_ = vec_nmsub(rj,zk,vec_madd(rk,zj,vec_madd(rr,zi,vec_madd(ri,zr,_0))));
        const vector float& zj_ = vec_madd(ri,zk,vec_madd(rr,zj,vec_nmsub(rk,zi,vec_madd(rj,zr,_0))));
        zk = vec_madd(rr,zk,vec_nmsub(ri,zj,vec_madd(rj,zi,vec_madd(rk,zr,_0))));
        zr=zr_; zi=zi_; zj=zj_;
    
        const vector float& f_ = vec_madd(zk,zk,vec_madd(zj,zj,vec_madd(zi,zi,vec_madd(zr,zr,_0))));
        
        const vector bool int& m = vec_cmplt(f_,MAX_NORM);
    		
        //if (vec_all_eq(m,(vector bool int)(0)))
        //    break;
        //i = (vec_all_eq(m,(vector bool int)(0))?mIterationslimit:i);
             
        f = vec_sel(f,f_,m);
   		
        n = vec_sel(n,vec_add(n,_1),m);
    }
    	
    const vector float a = vec_sub(zweiHochNteWurzel(f,n),zweiHochNteWurzel((vector float)(4),n));
    
	Vec<float,4> derAbstand;
    vec_st(a,0,&derAbstand[0]);
    return derAbstand;
}

template <> 
inline Vec<float,8> Juliamenge::abstand(const QuaternionT<Vec<float,8> >& inZ) const
{ // Spezialisiert auf 8, auskodiert
	const vector float MAX_NORM = (vector float)(MAX_FLOAT);
	const vector float _0 = (vector float)(-0.);
    const vector signed int _1 = (vector signed int)(1);
    
    vector float zr1=vec_ld(0,&inZ.r[0]),zr2=vec_ld(0,&inZ.r[4]),
                 zi1=vec_ld(0,&inZ.i[0]),zi2=vec_ld(0,&inZ.i[4]),
                 zj1=vec_ld(0,&inZ.j[0]),zj2=vec_ld(0,&inZ.j[4]),
                 zk1=vec_ld(0,&inZ.k[0]),zk2=vec_ld(0,&inZ.k[4]);
    const vector float dieKonst = vec_ld(0,&mKonstante.r);
    const vector float derRot = vec_ld(0,&mRotor.r);
    const vector float kr=vec_splat(dieKonst,0), // splats im Code?
                 ki=vec_splat(dieKonst,1),
                 kj=vec_splat(dieKonst,2),
                 kk=vec_splat(dieKonst,3);
    const vector float rr=vec_splat(derRot,0),
                 ri=vec_splat(derRot,1),
                 rj=vec_splat(derRot,2),
                 rk=vec_splat(derRot,3);
    
    vector float f1,f2;
	vector signed int n1=_1,n2=_1;
    
	for (int i=0;i<mIterationslimit;i++)
    {
        //z.quadriereUndAddiere(mKonstante4);
        const vector float& zweiR1=vec_add(zr1,zr1), &zweiR2=vec_add(zr2,zr2);
    
        zr1 = vec_nmsub(zk1,zk1,vec_nmsub(zj1,zj1,vec_nmsub(zi1,zi1,vec_madd(zr1,zr1,kr))));
        zr2 = vec_nmsub(zk2,zk2,vec_nmsub(zj2,zj2,vec_nmsub(zi2,zi2,vec_madd(zr2,zr2,kr))));
        zi1 = vec_madd(zweiR1,zi1,ki); zi2 = vec_madd(zweiR2,zi2,ki);
        zj1 = vec_madd(zweiR1,zj1,kj); zj2 = vec_madd(zweiR2,zj2,kj);
        zk1 = vec_madd(zweiR1,zk1,kk); zk2 = vec_madd(zweiR2,zk2,kk);
     
        //z *= mRotor4;
        const vector float& zr1_ = vec_nmsub(rk,zk1,vec_nmsub(rj,zj1,vec_nmsub(ri,zi1,vec_madd(rr,zr1,_0))));
        const vector float& zr2_ = vec_nmsub(rk,zk2,vec_nmsub(rj,zj2,vec_nmsub(ri,zi2,vec_madd(rr,zr2,_0))));
        const vector float& zi1_ = vec_nmsub(rj,zk1,vec_madd(rk,zj1,vec_madd(rr,zi1,vec_madd(ri,zr1,_0))));
        const vector float& zi2_ = vec_nmsub(rj,zk2,vec_madd(rk,zj2,vec_madd(rr,zi2,vec_madd(ri,zr2,_0))));
        const vector float& zj1_ = vec_madd(ri,zk1,vec_madd(rr,zj1,vec_nmsub(rk,zi1,vec_madd(rj,zr1,_0))));
        const vector float& zj2_ = vec_madd(ri,zk2,vec_madd(rr,zj2,vec_nmsub(rk,zi2,vec_madd(rj,zr2,_0))));
        zk1 = vec_madd(rr,zk1,vec_nmsub(ri,zj1,vec_madd(rj,zi1,vec_madd(rk,zr1,_0))));
        zk2 = vec_madd(rr,zk2,vec_nmsub(ri,zj2,vec_madd(rj,zi2,vec_madd(rk,zr2,_0))));
        zr1=zr1_; zr2=zr2_;
        zi1=zi1_; zi2=zi2_;
        zj1=zj1_; zj2=zj2_;
    
        //const Float8& f_ = norm(z);
        const vector float& f1_ = vec_madd(zk1,zk1,vec_madd(zj1,zj1,vec_madd(zi1,zi1,vec_madd(zr1,zr1,_0))));
        const vector float& f2_ = vec_madd(zk2,zk2,vec_madd(zj2,zj2,vec_madd(zi2,zi2,vec_madd(zr2,zr2,_0))));
        
        //const Bool8& m = f_<MAX_NORM;         
        const vector bool int& m1 = vec_cmplt(f1_,MAX_NORM);
        const vector bool int& m2 = vec_cmplt(f2_,MAX_NORM);
    		
   		//if (!m.teilweiseWahr())
   		//	break;
        //i = (m.teilweiseWahr()?i:mIterationslimit);
        //if (vec_all_eq(vec_or(m1,m2),(vector bool int)(0)))
        //    break;
        //i = (vec_all_eq(vec_or(m1,m2),(vector bool int)(0))?mIterationslimit:i);
             
        //f = auswahl(m,f_,f);
        f1 = vec_sel(f1,f1_,m1);
        f2 = vec_sel(f2,f2_,m2);
   		
        //n = auswahl(m,n+_1,n);
        n1 = vec_sel(n1,vec_add(n1,_1),m1);
        n2 = vec_sel(n2,vec_add(n2,_1),m2);
    }
    
    const vector float a1 = vec_sub(zweiHochNteWurzel(f1,n1),zweiHochNteWurzel((vector float)(4),n1));
    const vector float a2 = vec_sub(zweiHochNteWurzel(f2,n2),zweiHochNteWurzel((vector float)(4),n2));
    
    Vec<float,8> derAbstand;
    vec_st(a1,0,&derAbstand[0]);
	vec_st(a2,0,&derAbstand[4]);
    return derAbstand;
}


template <> 
inline Vec<float,12> Juliamenge::abstand(const QuaternionT<Vec<float,12> >& inZ) const
{ // Spezialisiert auf 12, auskodiert
	const vector float MAX_NORM = (vector float)(MAX_FLOAT);
	const vector float _0 = (vector float)(-0.);
    const vector signed int _1 = (vector signed int)(1);
    
    vector float zr1=vec_ld(0,&inZ.r[0]),zr2=vec_ld(0,&inZ.r[4]),zr3=vec_ld(0,&inZ.r[8]),
                 zi1=vec_ld(0,&inZ.i[0]),zi2=vec_ld(0,&inZ.i[4]),zi3=vec_ld(0,&inZ.i[8]),
                 zj1=vec_ld(0,&inZ.j[0]),zj2=vec_ld(0,&inZ.j[4]),zj3=vec_ld(0,&inZ.j[8]),
                 zk1=vec_ld(0,&inZ.k[0]),zk2=vec_ld(0,&inZ.k[4]),zk3=vec_ld(0,&inZ.k[8]);
    const vector float dieKonst = vec_ld(0,&mKonstante.r);
    const vector float derRot = vec_ld(0,&mRotor.r);
    const vector float kr=vec_splat(dieKonst,0), // splats im Code?
                 ki=vec_splat(dieKonst,1),
                 kj=vec_splat(dieKonst,2),
                 kk=vec_splat(dieKonst,3);
    const vector float rr=vec_splat(derRot,0),
                 ri=vec_splat(derRot,1),
                 rj=vec_splat(derRot,2),
                 rk=vec_splat(derRot,3);
    
    vector float f1,f2,f3;
	vector signed int n1=_1,n2=_1,n3=_1;
    
	for (int i=0;i<mIterationslimit;i++)
    {
        //z.quadriereUndAddiere(mKonstante4);
        const vector float& zweiR1=vec_add(zr1,zr1), &zweiR2=vec_add(zr2,zr2), &zweiR3=vec_add(zr3,zr3);
    
        zr1 = vec_nmsub(zk1,zk1,vec_nmsub(zj1,zj1,vec_nmsub(zi1,zi1,vec_madd(zr1,zr1,kr))));
        zr2 = vec_nmsub(zk2,zk2,vec_nmsub(zj2,zj2,vec_nmsub(zi2,zi2,vec_madd(zr2,zr2,kr))));
        zr3 = vec_nmsub(zk3,zk3,vec_nmsub(zj3,zj3,vec_nmsub(zi3,zi3,vec_madd(zr3,zr3,kr))));
        zi1 = vec_madd(zweiR1,zi1,ki); zi2 = vec_madd(zweiR2,zi2,ki); zi3 = vec_madd(zweiR3,zi3,ki);
        zj1 = vec_madd(zweiR1,zj1,kj); zj2 = vec_madd(zweiR2,zj2,kj); zj3 = vec_madd(zweiR3,zj3,kj);
        zk1 = vec_madd(zweiR1,zk1,kk); zk2 = vec_madd(zweiR2,zk2,kk); zk3 = vec_madd(zweiR3,zk3,kk);
     
        //z *= mRotor4;
        const vector float& zr1_ = vec_nmsub(rk,zk1,vec_nmsub(rj,zj1,vec_nmsub(ri,zi1,vec_madd(rr,zr1,_0))));
        const vector float& zr2_ = vec_nmsub(rk,zk2,vec_nmsub(rj,zj2,vec_nmsub(ri,zi2,vec_madd(rr,zr2,_0))));
        const vector float& zr3_ = vec_nmsub(rk,zk3,vec_nmsub(rj,zj3,vec_nmsub(ri,zi3,vec_madd(rr,zr3,_0))));
        const vector float& zi1_ = vec_nmsub(rj,zk1,vec_madd(rk,zj1,vec_madd(rr,zi1,vec_madd(ri,zr1,_0))));
        const vector float& zi2_ = vec_nmsub(rj,zk2,vec_madd(rk,zj2,vec_madd(rr,zi2,vec_madd(ri,zr2,_0))));
        const vector float& zi3_ = vec_nmsub(rj,zk3,vec_madd(rk,zj3,vec_madd(rr,zi3,vec_madd(ri,zr3,_0))));
        const vector float& zj1_ = vec_madd(ri,zk1,vec_madd(rr,zj1,vec_nmsub(rk,zi1,vec_madd(rj,zr1,_0))));
        const vector float& zj2_ = vec_madd(ri,zk2,vec_madd(rr,zj2,vec_nmsub(rk,zi2,vec_madd(rj,zr2,_0))));
        const vector float& zj3_ = vec_madd(ri,zk3,vec_madd(rr,zj3,vec_nmsub(rk,zi3,vec_madd(rj,zr3,_0))));
        zk1 = vec_madd(rr,zk1,vec_nmsub(ri,zj1,vec_madd(rj,zi1,vec_madd(rk,zr1,_0))));
        zk2 = vec_madd(rr,zk2,vec_nmsub(ri,zj2,vec_madd(rj,zi2,vec_madd(rk,zr2,_0))));
        zk3 = vec_madd(rr,zk3,vec_nmsub(ri,zj3,vec_madd(rj,zi3,vec_madd(rk,zr3,_0))));
        zr1=zr1_; zr2=zr2_; zr3=zr3_;
        zi1=zi1_; zi2=zi2_; zi3=zi3_;
        zj1=zj1_; zj2=zj2_; zj3=zj3_;
    
        //const Float8& f_ = norm(z);
        const vector float& f1_ = vec_madd(zk1,zk1,vec_madd(zj1,zj1,vec_madd(zi1,zi1,vec_madd(zr1,zr1,_0))));
        const vector float& f2_ = vec_madd(zk2,zk2,vec_madd(zj2,zj2,vec_madd(zi2,zi2,vec_madd(zr2,zr2,_0))));
        const vector float& f3_ = vec_madd(zk3,zk3,vec_madd(zj3,zj3,vec_madd(zi3,zi3,vec_madd(zr3,zr3,_0))));
        
        //const Bool8& m = f_<MAX_NORM;         
        const vector bool int& m1 = vec_cmplt(f1_,MAX_NORM);
        const vector bool int& m2 = vec_cmplt(f2_,MAX_NORM);
        const vector bool int& m3 = vec_cmplt(f3_,MAX_NORM);
    		
   		//if (!m.teilweiseWahr())
   		//	break;
        //i = (m.teilweiseWahr()?i:mIterationslimit);
        //if (vec_all_eq(vec_or(m1,m2),(vector bool int)(0)))
        //    break;
        //i = (vec_all_eq(vec_or(m1,m2),(vector bool int)(0))?mIterationslimit:i);
             
        //f = auswahl(m,f_,f);
        f1 = vec_sel(f1,f1_,m1);
        f2 = vec_sel(f2,f2_,m2);
        f3 = vec_sel(f3,f3_,m3);
   		
        //n = auswahl(m,n+_1,n);
        n1 = vec_sel(n1,vec_add(n1,_1),m1);
        n2 = vec_sel(n2,vec_add(n2,_1),m2);
        n3 = vec_sel(n3,vec_add(n3,_1),m3);
    }
    
    const vector float a1 = vec_sub(zweiHochNteWurzel(f1,n1),zweiHochNteWurzel((vector float)(4),n1));
    const vector float a2 = vec_sub(zweiHochNteWurzel(f2,n2),zweiHochNteWurzel((vector float)(4),n2));
    const vector float a3 = vec_sub(zweiHochNteWurzel(f3,n3),zweiHochNteWurzel((vector float)(4),n3));
    
    Vec<float,12> derAbstand;
    vec_st(a1,0,&derAbstand[0]);
	vec_st(a2,0,&derAbstand[4]);
	vec_st(a3,0,&derAbstand[8]);
    return derAbstand;
}

template <> 
inline Vec<float,16> Juliamenge::abstand(const QuaternionT<Vec<float,16> >& inZ) const
{ // Spezialisiert auf 12, auskodiert
	const vector float MAX_NORM = (vector float)(MAX_FLOAT);
	const vector float _0 = (vector float)(-0.);
    const vector signed int _1 = (vector signed int)(1);
    
    vector float zr1=vec_ld(0,&inZ.r[0]),zr2=vec_ld(0,&inZ.r[4]),zr3=vec_ld(0,&inZ.r[8]),zr4=vec_ld(0,&inZ.r[12]),
                 zi1=vec_ld(0,&inZ.i[0]),zi2=vec_ld(0,&inZ.i[4]),zi3=vec_ld(0,&inZ.i[8]),zi4=vec_ld(0,&inZ.i[12]),
                 zj1=vec_ld(0,&inZ.j[0]),zj2=vec_ld(0,&inZ.j[4]),zj3=vec_ld(0,&inZ.j[8]),zj4=vec_ld(0,&inZ.j[12]),
                 zk1=vec_ld(0,&inZ.k[0]),zk2=vec_ld(0,&inZ.k[4]),zk3=vec_ld(0,&inZ.k[8]),zk4=vec_ld(0,&inZ.k[12]);
    const vector float dieKonst = vec_ld(0,&mKonstante.r);
    const vector float derRot = vec_ld(0,&mRotor.r);
    const vector float kr=vec_splat(dieKonst,0), // splats im Code?
                 ki=vec_splat(dieKonst,1),
                 kj=vec_splat(dieKonst,2),
                 kk=vec_splat(dieKonst,3);
    const vector float rr=vec_splat(derRot,0),
                 ri=vec_splat(derRot,1),
                 rj=vec_splat(derRot,2),
                 rk=vec_splat(derRot,3);
    
    vector float f1,f2,f3,f4;
	vector signed int n1=_1,n2=_1,n3=_1,n4=_1;
    
	for (int i=0;i<mIterationslimit;i++)
    {
        //z.quadriereUndAddiere(mKonstante4);
        const vector float& zweiR1=vec_add(zr1,zr1), &zweiR2=vec_add(zr2,zr2), &zweiR3=vec_add(zr3,zr3), &zweiR4=vec_add(zr4,zr4);
    
        zr1 = vec_nmsub(zk1,zk1,vec_nmsub(zj1,zj1,vec_nmsub(zi1,zi1,vec_madd(zr1,zr1,kr))));
        zr2 = vec_nmsub(zk2,zk2,vec_nmsub(zj2,zj2,vec_nmsub(zi2,zi2,vec_madd(zr2,zr2,kr))));
        zr3 = vec_nmsub(zk3,zk3,vec_nmsub(zj3,zj3,vec_nmsub(zi3,zi3,vec_madd(zr3,zr3,kr))));
		zr4 = vec_nmsub(zk4,zk4,vec_nmsub(zj4,zj4,vec_nmsub(zi4,zi4,vec_madd(zr4,zr4,kr))));
        zi1 = vec_madd(zweiR1,zi1,ki); zi2 = vec_madd(zweiR2,zi2,ki); zi3 = vec_madd(zweiR3,zi3,ki); zi4 = vec_madd(zweiR4,zi4,ki);
        zj1 = vec_madd(zweiR1,zj1,kj); zj2 = vec_madd(zweiR2,zj2,kj); zj3 = vec_madd(zweiR3,zj3,kj); zj4 = vec_madd(zweiR4,zj4,kj); 
        zk1 = vec_madd(zweiR1,zk1,kk); zk2 = vec_madd(zweiR2,zk2,kk); zk3 = vec_madd(zweiR3,zk3,kk); zk4 = vec_madd(zweiR4,zk4,kk);
		
     
        //z *= mRotor4;
        const vector float& zr1_ = vec_nmsub(rk,zk1,vec_nmsub(rj,zj1,vec_nmsub(ri,zi1,vec_madd(rr,zr1,_0))));
        const vector float& zr2_ = vec_nmsub(rk,zk2,vec_nmsub(rj,zj2,vec_nmsub(ri,zi2,vec_madd(rr,zr2,_0))));
        const vector float& zr3_ = vec_nmsub(rk,zk3,vec_nmsub(rj,zj3,vec_nmsub(ri,zi3,vec_madd(rr,zr3,_0))));
		const vector float& zr4_ = vec_nmsub(rk,zk4,vec_nmsub(rj,zj4,vec_nmsub(ri,zi4,vec_madd(rr,zr4,_0))));
        const vector float& zi1_ = vec_nmsub(rj,zk1,vec_madd(rk,zj1,vec_madd(rr,zi1,vec_madd(ri,zr1,_0))));
        const vector float& zi2_ = vec_nmsub(rj,zk2,vec_madd(rk,zj2,vec_madd(rr,zi2,vec_madd(ri,zr2,_0))));
        const vector float& zi3_ = vec_nmsub(rj,zk3,vec_madd(rk,zj3,vec_madd(rr,zi3,vec_madd(ri,zr3,_0))));
		const vector float& zi4_ = vec_nmsub(rj,zk4,vec_madd(rk,zj4,vec_madd(rr,zi4,vec_madd(ri,zr4,_0))));
        const vector float& zj1_ = vec_madd(ri,zk1,vec_madd(rr,zj1,vec_nmsub(rk,zi1,vec_madd(rj,zr1,_0))));
        const vector float& zj2_ = vec_madd(ri,zk2,vec_madd(rr,zj2,vec_nmsub(rk,zi2,vec_madd(rj,zr2,_0))));
        const vector float& zj3_ = vec_madd(ri,zk3,vec_madd(rr,zj3,vec_nmsub(rk,zi3,vec_madd(rj,zr3,_0))));
		const vector float& zj4_ = vec_madd(ri,zk4,vec_madd(rr,zj4,vec_nmsub(rk,zi4,vec_madd(rj,zr4,_0))));
        zk1 = vec_madd(rr,zk1,vec_nmsub(ri,zj1,vec_madd(rj,zi1,vec_madd(rk,zr1,_0))));
        zk2 = vec_madd(rr,zk2,vec_nmsub(ri,zj2,vec_madd(rj,zi2,vec_madd(rk,zr2,_0))));
        zk3 = vec_madd(rr,zk3,vec_nmsub(ri,zj3,vec_madd(rj,zi3,vec_madd(rk,zr3,_0))));
		zk4 = vec_madd(rr,zk4,vec_nmsub(ri,zj4,vec_madd(rj,zi4,vec_madd(rk,zr4,_0))));
        zr1=zr1_; zr2=zr2_; zr3=zr3_; zr4=zr4_;
        zi1=zi1_; zi2=zi2_; zi3=zi3_; zi4=zi4_;
        zj1=zj1_; zj2=zj2_; zj3=zj3_; zj4=zj4_;
    
        //const Float8& f_ = norm(z);
        const vector float& f1_ = vec_madd(zk1,zk1,vec_madd(zj1,zj1,vec_madd(zi1,zi1,vec_madd(zr1,zr1,_0))));
        const vector float& f2_ = vec_madd(zk2,zk2,vec_madd(zj2,zj2,vec_madd(zi2,zi2,vec_madd(zr2,zr2,_0))));
        const vector float& f3_ = vec_madd(zk3,zk3,vec_madd(zj3,zj3,vec_madd(zi3,zi3,vec_madd(zr3,zr3,_0))));
        const vector float& f4_ = vec_madd(zk4,zk4,vec_madd(zj4,zj4,vec_madd(zi4,zi4,vec_madd(zr4,zr4,_0))));
		
        //const Bool8& m = f_<MAX_NORM;         
        const vector bool int& m1 = vec_cmplt(f1_,MAX_NORM);
        const vector bool int& m2 = vec_cmplt(f2_,MAX_NORM);
        const vector bool int& m3 = vec_cmplt(f3_,MAX_NORM);
		const vector bool int& m4 = vec_cmplt(f4_,MAX_NORM);
    		
   		//if (!m.teilweiseWahr())
   		//	break;
        //i = (m.teilweiseWahr()?i:mIterationslimit);
        //if (vec_all_eq(vec_or(m1,m2),(vector bool int)(0)))
        //    break;
        //i = (vec_all_eq(vec_or(m1,m2),(vector bool int)(0))?mIterationslimit:i);
             
        //f = auswahl(m,f_,f);
        f1 = vec_sel(f1,f1_,m1);
        f2 = vec_sel(f2,f2_,m2);
        f3 = vec_sel(f3,f3_,m3);
		f4 = vec_sel(f4,f4_,m4);
   		
        //n = auswahl(m,n+_1,n);
        n1 = vec_sel(n1,vec_add(n1,_1),m1);
        n2 = vec_sel(n2,vec_add(n2,_1),m2);
        n3 = vec_sel(n3,vec_add(n3,_1),m3);
		n4 = vec_sel(n4,vec_add(n4,_1),m4);
    }
    
    const vector float a1 = vec_sub(zweiHochNteWurzel(f1,n1),zweiHochNteWurzel((vector float)(4),n1));
    const vector float a2 = vec_sub(zweiHochNteWurzel(f2,n2),zweiHochNteWurzel((vector float)(4),n2));
    const vector float a3 = vec_sub(zweiHochNteWurzel(f3,n3),zweiHochNteWurzel((vector float)(4),n3));
	const vector float a4 = vec_sub(zweiHochNteWurzel(f4,n4),zweiHochNteWurzel((vector float)(4),n4));
    
    Vec<float,16> derAbstand;
	vec_st(a1,0,&derAbstand[0]);
	vec_st(a2,0,&derAbstand[4]);
	vec_st(a3,0,&derAbstand[8]);
	vec_st(a4,0,&derAbstand[12]);
    return derAbstand;
}


#endif

#if __SSE2__

#include <emmintrin.h>
#include "SSEMathematik.h"

template<>
inline Vec<float,4> Juliamenge::abstand(const QuaternionT<Vec<float,4> >& inZ) const
{ // Spezialisiert auf 4, auskodiert
    
	// Denormals ausschalten?
	
	const vFloat MAX_NORM = _mm_set1_ps(MAX_FLOAT);
    const vSInt32 _1 = _mm_set1_epi32(1);
	
	vFloat zr=_mm_load_ps((float*)&inZ.r),
		   zi=_mm_load_ps((float*)&inZ.i),
		   zj=_mm_load_ps((float*)&inZ.j),
		   zk=_mm_load_ps((float*)&inZ.k);
					
    const vFloat kr=_mm_set_ps1(mKonstante.r),
				 ki=_mm_set_ps1(mKonstante.i),
				 kj=_mm_set_ps1(mKonstante.j),
				 kk=_mm_set_ps1(mKonstante.k);
    const vFloat rr=_mm_set_ps1(mRotor.r),
				 ri=_mm_set_ps1(mRotor.i),
				 rj=_mm_set_ps1(mRotor.j),
				 rk=_mm_set_ps1(mRotor.k);
	
	vFloat f;
	vSInt32 n=_1;
	
	for (int i=0;i<mIterationslimit;i++)
    {
        const vFloat& zweiR=_mm_add_ps(zr,zr);
		
		zr = _mm_sub_ps(_mm_sub_ps(_mm_sub_ps(_mm_add_ps(_mm_mul_ps(zr,zr),kr),_mm_mul_ps(zi,zi)),_mm_mul_ps(zj,zj)),_mm_mul_ps(zk,zk));
        zi = _mm_add_ps(_mm_mul_ps(zweiR,zi),ki);
        zj = _mm_add_ps(_mm_mul_ps(zweiR,zj),kj);
        zk = _mm_add_ps(_mm_mul_ps(zweiR,zk),kk);
	
		const vFloat& zr_ = _mm_sub_ps(_mm_sub_ps(_mm_sub_ps(_mm_mul_ps(rr,zr),_mm_mul_ps(ri,zi)),_mm_mul_ps(rj,zj)),_mm_mul_ps(rk,zk));
		const vFloat& zi_ = _mm_sub_ps(_mm_add_ps(_mm_add_ps(_mm_mul_ps(ri,zr),_mm_mul_ps(rr,zi)),_mm_mul_ps(rk,zj)),_mm_mul_ps(rj,zk));
		const vFloat& zj_ = _mm_add_ps(_mm_add_ps(_mm_sub_ps(_mm_mul_ps(rj,zr),_mm_mul_ps(rk,zi)),_mm_mul_ps(rr,zj)),_mm_mul_ps(ri,zk));
		zk = _mm_add_ps(_mm_sub_ps(_mm_add_ps(_mm_mul_ps(rk,zr),_mm_mul_ps(rj,zi)),_mm_mul_ps(ri,zj)),_mm_mul_ps(rr,zk));
		zr=zr_; zi=zi_; zj=zj_;
    
        const vFloat& f_ = _mm_add_ps(_mm_mul_ps(zk,zk),_mm_add_ps(_mm_mul_ps(zj,zj),_mm_add_ps(_mm_mul_ps(zi,zi),_mm_mul_ps(zr,zr))));
        
        const vFloat& m = _mm_cmplt_ps(f_,MAX_NORM);
             
        f = _mm_or_ps(_mm_andnot_ps(m,f),_mm_and_ps(m,f_));
		
        n = _mm_add_epi32(n,_mm_and_si128(_1,(vSInt32)m));
    }

    const vFloat a = _mm_sub_ps(zweiHochNteWurzel(f,n),zweiHochNteWurzel(_mm_set1_ps(4),n));
    
    Vec<float,4> derAbstand;
	_mm_store_ps((float*)&derAbstand,a);

    return derAbstand;
}

template<>
inline Vec<float,8> Juliamenge::abstand(const QuaternionT<Vec<float,8> >& inZ) const
{ // Spezialisiert auf 4, auskodiert
    
	// Denormals ausschalten?
	
	const vFloat MAX_NORM = _mm_set1_ps(MAX_FLOAT);
    const vSInt32 _1 = _mm_set1_epi32(1);
	
	vFloat zr1=_mm_load_ps((float*)&inZ.r[0]),zr2=_mm_load_ps((float*)&inZ.r[4]),
		   zi1=_mm_load_ps((float*)&inZ.i[0]),zi2=_mm_load_ps((float*)&inZ.i[4]),
		   zj1=_mm_load_ps((float*)&inZ.j[0]),zj2=_mm_load_ps((float*)&inZ.j[4]),
		   zk1=_mm_load_ps((float*)&inZ.k[0]),zk2=_mm_load_ps((float*)&inZ.k[4]);
					
    const vFloat kr=_mm_set_ps1(mKonstante.r),
				 ki=_mm_set_ps1(mKonstante.i),
				 kj=_mm_set_ps1(mKonstante.j),
				 kk=_mm_set_ps1(mKonstante.k);
    const vFloat rr=_mm_set_ps1(mRotor.r),
				 ri=_mm_set_ps1(mRotor.i),
				 rj=_mm_set_ps1(mRotor.j),
				 rk=_mm_set_ps1(mRotor.k);
	
	vFloat f1,f2;
	vSInt32 n1=_1,n2=_1;
	
	for (int i=0;i<mIterationslimit;i++)
    {
        const vFloat& zweiR1=_mm_add_ps(zr1,zr1);
		const vFloat& zweiR2=_mm_add_ps(zr2,zr2);
		
		zr1 = _mm_sub_ps(_mm_sub_ps(_mm_sub_ps(_mm_add_ps(_mm_mul_ps(zr1,zr1),kr),_mm_mul_ps(zi1,zi1)),_mm_mul_ps(zj1,zj1)),_mm_mul_ps(zk1,zk1));
		zr2 = _mm_sub_ps(_mm_sub_ps(_mm_sub_ps(_mm_add_ps(_mm_mul_ps(zr2,zr2),kr),_mm_mul_ps(zi2,zi2)),_mm_mul_ps(zj2,zj2)),_mm_mul_ps(zk2,zk2));
        zi1 = _mm_add_ps(_mm_mul_ps(zweiR1,zi1),ki);
		zi2 = _mm_add_ps(_mm_mul_ps(zweiR2,zi2),ki);
        zj1 = _mm_add_ps(_mm_mul_ps(zweiR1,zj1),kj);
		zj2 = _mm_add_ps(_mm_mul_ps(zweiR2,zj2),kj);
        zk1 = _mm_add_ps(_mm_mul_ps(zweiR1,zk1),kk);
		zk2 = _mm_add_ps(_mm_mul_ps(zweiR2,zk2),kk);
	
		const vFloat& zr1_ = _mm_sub_ps(_mm_sub_ps(_mm_sub_ps(_mm_mul_ps(rr,zr1),_mm_mul_ps(ri,zi1)),_mm_mul_ps(rj,zj1)),_mm_mul_ps(rk,zk1));
		const vFloat& zr2_ = _mm_sub_ps(_mm_sub_ps(_mm_sub_ps(_mm_mul_ps(rr,zr2),_mm_mul_ps(ri,zi2)),_mm_mul_ps(rj,zj2)),_mm_mul_ps(rk,zk2));
		const vFloat& zi1_ = _mm_sub_ps(_mm_add_ps(_mm_add_ps(_mm_mul_ps(ri,zr1),_mm_mul_ps(rr,zi1)),_mm_mul_ps(rk,zj1)),_mm_mul_ps(rj,zk1));
		const vFloat& zi2_ = _mm_sub_ps(_mm_add_ps(_mm_add_ps(_mm_mul_ps(ri,zr2),_mm_mul_ps(rr,zi2)),_mm_mul_ps(rk,zj2)),_mm_mul_ps(rj,zk2));
		const vFloat& zj1_ = _mm_add_ps(_mm_add_ps(_mm_sub_ps(_mm_mul_ps(rj,zr1),_mm_mul_ps(rk,zi1)),_mm_mul_ps(rr,zj1)),_mm_mul_ps(ri,zk1));
		const vFloat& zj2_ = _mm_add_ps(_mm_add_ps(_mm_sub_ps(_mm_mul_ps(rj,zr2),_mm_mul_ps(rk,zi2)),_mm_mul_ps(rr,zj2)),_mm_mul_ps(ri,zk2));
		zk1 = _mm_add_ps(_mm_sub_ps(_mm_add_ps(_mm_mul_ps(rk,zr1),_mm_mul_ps(rj,zi1)),_mm_mul_ps(ri,zj1)),_mm_mul_ps(rr,zk1));
		zk2 = _mm_add_ps(_mm_sub_ps(_mm_add_ps(_mm_mul_ps(rk,zr2),_mm_mul_ps(rj,zi2)),_mm_mul_ps(ri,zj2)),_mm_mul_ps(rr,zk2));
		zr1=zr1_; zi1=zi1_; zj1=zj1_;
		zr2=zr2_; zi2=zi2_; zj2=zj2_;
    
        const vFloat& f1_ = _mm_add_ps(_mm_mul_ps(zk1,zk1),_mm_add_ps(_mm_mul_ps(zj1,zj1),_mm_add_ps(_mm_mul_ps(zi1,zi1),_mm_mul_ps(zr1,zr1))));
		const vFloat& f2_ = _mm_add_ps(_mm_mul_ps(zk2,zk2),_mm_add_ps(_mm_mul_ps(zj2,zj2),_mm_add_ps(_mm_mul_ps(zi2,zi2),_mm_mul_ps(zr2,zr2))));
        
        const vFloat& m1 = _mm_cmplt_ps(f1_,MAX_NORM);
		const vFloat& m2 = _mm_cmplt_ps(f2_,MAX_NORM);
             
        f1 = _mm_or_ps(_mm_andnot_ps(m1,f1),_mm_and_ps(m1,f1_));
		f2 = _mm_or_ps(_mm_andnot_ps(m2,f2),_mm_and_ps(m2,f2_));
		
        n1 = _mm_add_epi32(n1,_mm_and_si128(_1,(vSInt32)m1));
		n2 = _mm_add_epi32(n2,_mm_and_si128(_1,(vSInt32)m2));
    }

    const vFloat a1 = _mm_sub_ps(zweiHochNteWurzel(f1,n1),zweiHochNteWurzel(_mm_set1_ps(4.f),n1));
	const vFloat a2 = _mm_sub_ps(zweiHochNteWurzel(f2,n2),zweiHochNteWurzel(_mm_set1_ps(4.f),n2));
    
    Vec<float,8> derAbstand;
	_mm_store_ps((float*)&derAbstand[0],a1);
	_mm_store_ps((float*)&derAbstand[4],a2);

    return derAbstand;
}

#endif

