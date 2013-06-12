/**
 *
 *  Written in 2003 by Leo Fink (mail@leofink.de).
 *
 **/

#include "Juliamenge.h"
#include "VecMath.h"

#include <iostream>


using namespace std;

Juliamenge::Juliamenge(const int& inIterationslimit,
        			   const Quaternion& inKonstante,
        		   	   const float& inWinkel)
	: mKonstante(inKonstante),
      mRotor(cos(inWinkel),sin(inWinkel),0,0),
      mIterationslimit(inIterationslimit)/*,
	  mX(zweiHochNteWurzel(4.f,mIterationslimit))*/
{	
}

void Juliamenge::setzeIterationslimit(const int& inN)
{
	mIterationslimit = inN;
	//mX = zweiHochNteWurzel(4.f,mIterationslimit);
}

void Juliamenge::setzeKonstante(const Quaternion& inC)
{
	mKonstante = inC;
}

void Juliamenge::setzeWinkel(const float& inWinkel)
{
	mRotor = Quaternion(cos(inWinkel),sin(inWinkel),0,0);
}

BoolVec Juliamenge::liegtInnerhalb(const QuaternionVec& inZ) const
{		
	QuaternionVec z(inZ);
	Quaternion	  c(mKonstante),r(mRotor);
    BoolVec       m(false);
    
    for (long i=0;i<mIterationslimit;i++)
    {
        z.quadriereUndAddiere(c);
        z *= r;
        
		m = norm(z)<4.f;
		
		//if (!teilweiseWahr(m))
		//	break;
    }
     
    return m;
}

QuaternionVec Juliamenge::gradient(const QuaternionVec& inZ) const
{    
    QuaternionVec	z(inZ);
    QuaternionVec	dr(Quaternion(1,0,0,0)),di(Quaternion(0,1,0,0)),dj(Quaternion(0,0,1,0)),dk(Quaternion(0,0,0,1));
    Quaternion		r(mRotor),c(mKonstante);
	
    for (int i=0;i<mIterationslimit;i++)
    {
        dr = akh(dr,z);		// dr = (dr*z+z*dr)/2;
        di = akh(di,z);		// ...
        dj = akh(dj,z);	
        dk = akh(dk,z);	
        
        z.quadriereUndAddiere(c);
        
        dr *= r;
        di *= r;
        dj *= r;
        dk *= r;
        
        z *= r;
    }
    
    return QuaternionVec(sp(z,dr),sp(z,di),sp(z,dj),sp(z,dk));
}




