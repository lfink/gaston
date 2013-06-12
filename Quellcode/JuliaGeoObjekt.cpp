/**
 *
 *  Written in 2003 by Leo Fink (mail@leofink.de).
 *
 **/

#include "JuliaGeoObjekt.h"
#include "VecMath.h"

#include <limits>
#include <iostream>

#if __ppc__ || 1
	#define atomic_add(X,C) (*X+=C) // Da nicht verfŸgbar auf PowerPC
#else
	#define atomic_add(X,C) __sync_fetch_and_add(X,C) // Endlosschleife in LLVM
#endif

volatile int64_t JuliaGeoObjekt::anzSchritte __attribute__ ((aligned (8))) = 0;

JuliaGeoObjekt::JuliaGeoObjekt(const int& inIterationsMaximum,
					   const Quaternion& inC,
					   const float& inWinkel,
					   const float& inGenauigkeit,
					   const Farbe& inFarbe)
: Juliamenge(inIterationsMaximum,inC,inWinkel),
  mFarbe(inFarbe),
  mOrientierung(1,0,0,0),
  mMinimalschritt(0.01),
  mSchrittfaktor(1)
{
	setzeGenauigkeit(inGenauigkeit);
}

void JuliaGeoObjekt::setzeOrientierung(const Quaternion& inOrientierung)
{	
	mOrientierung = inOrientierung;
}

void JuliaGeoObjekt::drehe(const Quaternion& inDrehung)
{	
	mOrientierung = mOrientierung*inDrehung;
}

void JuliaGeoObjekt::setzeGenauigkeit(const float& inGenauigkeit)
{
	mMinimalschritt = pow(10.f,-inGenauigkeit); // Konstanten auslagern
	mSchrittfaktor = 1.-.2*(inGenauigkeit-1);
	//mSchrittfaktor = pow(2.,1-mIterationslimit);
}

// Mit Bisektion
void JuliaGeoObjekt::schnitt(const E3PunktVec& inStrahlursprung,
                              const R3VektorVec& inStrahlrichtung,
                              const float& inSichtweite, // sollte GeoObjekt selbst ermitteln
                              BoolVec& outSchneidet,
                              E3PunktVec& outSchnittpunkt,
                              R3VektorVec& outNormale,
                              FarbVec& outFarbe) const
{   
	QuaternionVec  z=tripelZuQuaternion(inStrahlursprung);
    QuaternionVec  r=tripelZuQuaternion(inStrahlrichtung);
	FloatVec s(0.f),sa,t(0.f);
	BoolVec h,a(true);
	unsigned long ns = 0;
    	
	while (true) // rein...
	{			
		sa = auswahl(a,s,sa);
		s = abstand(z)*mSchrittfaktor;
        
        a = s>0.f;
		h = t<inSichtweite;
		
		if (!teilweiseWahr(a && h))
			break;
		
		s = auswahl(a,maximum(s,mMinimalschritt),0.f);

		t += s;
        z += r*s;
        
        ns += PARALLEL;
	}
	
	atomic_add(&anzSchritte,ns); // anzSchritte += ns;
    outSchneidet = h;
	
	if (!teilweiseWahr(h))
		return;
	
	// ...wieder raus, Bisektion (TODO: Interpolation?)
	s = sa*.5f;
	z -= r*s;
	
	for (int i=0;i<5;i++)
	{
		s *= .5f;
        z += r*FloatVec(auswahl(liegtInnerhalb(z),-s,s)); // geht besser
	}
	atomic_add(&anzSchritte,6*PARALLEL); // anzSchritte += 6*PARALLEL;
	
	outSchnittpunkt = quaternionZuTripel<E3PunktVec>(z);
	outNormale = quaternionZuTripel<R3VektorVec>(gradient(z)).normiert(); // k=0??
	outFarbe = mFarbe;
}/**/

// Alternativ: ohne Bisektion
/*void JuliaGeoObjekt::schnitt(const E3PunktVec& inStrahlursprung,
							 const R3VektorVec& inStrahlrichtung,
							 const float& inSichtweite, // sollte GeoObjekt selbst ermitteln
							 BoolVec& outSchneidet,
							 E3PunktVec& outSchnittpunkt,
							 R3VektorVec& outNormale,
							 FarbVec& outFarbe) const
{   
	QuaternionVec  z=tripelZuQuaternion(inStrahlursprung);
	QuaternionVec  r=tripelZuQuaternion(inStrahlrichtung);
	FloatVec s,t(0.f);
	BoolVec h,a(true);
	unsigned long ns = 0;
	
	while (true) // rein...
	{	
		s = abstand(z)*mSchrittfaktor;
		
		a = s>0.f;
		h = t<inSichtweite;
		
		if (!teilweiseWahr(a && h))
			break;
		
		s = auswahl(a,maximum(s,mMinimalschritt),0.f);
		
		t += s;
		z += r*s;
		
		ns+=PARALLEL;
	};
	
	outSchneidet = h;
	
	if (!teilweiseWahr(h))
		return;
	
	// ... wieder raus
	z -= r*mMinimalschritt;
	ns += PARALLEL;
	
	outSchnittpunkt = quaternionZuTripel<E3PunktVec>(z);
	outNormale = quaternionZuTripel<R3VektorVec>(gradient(z)).normiert(); // k=0??
	outFarbe = mFarbe;
	__sync_fetch_and_add(ns,&anzSchritte); // anzSchritte += ns;
}*/

/*void JuliaGeoObjekt::schnitt(const E3PunktVec& inStrahlursprung,
                              const R3VektorVec& inStrahlrichtung,
                              const float& inSichtweite, // sollte GeoObjekt selbst ermitteln
                              BoolVec& outSchneidet,
                              E3PunktVec& outSchnittpunkt,
                              R3VektorVec& outNormale,
                              FarbVec& outFarbe) const
{   
	QuaternionVec  z=tripelZuQuaternion(inStrahlursprung);
    QuaternionVec  r=tripelZuQuaternion(inStrahlrichtung);
	FloatVec s(0.f),sa,t(0.f);
	BoolVec h,a(true);
	unsigned long ns = 0;
    	
	while (true) // rein...
	{			
		sa = auswahl(a,s,sa);
		s = abstand(z)*mSchrittfaktor;
        
        a = s>0.f;
		h = t<inSichtweite;
		
		if (!teilweiseWahr(a && h))
			break;
		
		s = auswahl(a,maximum(s,mMinimalschritt),0.f);

		t += s;
        z += r*s;
        
        ns += PARALLEL;
	}
	
	__sync_fetch_and_add(ns,&anzSchritte); // anzSchritte += ns;
    outSchneidet = h;
	
	if (!teilweiseWahr(h))
		return;
	
	// ...wieder raus
	z -= r*sa;
	ns += PARALLEL;
	
	outSchnittpunkt = quaternionZuTripel<E3PunktVec>(z);
	outNormale = quaternionZuTripel<R3VektorVec>(gradient(z)).normiert(); // k=0??
	outFarbe = mFarbe;
	__sync_fetch_and_add(ns,&anzSchritte); // anzSchritte += ns;
}*/
