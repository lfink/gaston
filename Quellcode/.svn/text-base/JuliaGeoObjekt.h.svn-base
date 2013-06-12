/**
 *
 *  Written in 2003 by Leo Fink (mail@leofink.de).
 *
 **/
 
#include "Farbe.h"
#include "Juliamenge.h"
#include "GeoObjekt.h"

#include "Parallel.h"

class JuliaGeoObjekt : public GeoObjekt, public Juliamenge
{
	public:
		JuliaGeoObjekt(const int& inIterationsMaximum,
					   const Quaternion& inC,
					   const float& inWinkel,
        		   	   const float& inGenauigkeit, // Zahl >= 1,
					   const Farbe& inFarbe);
		
        virtual void setzeOrientierung(const Quaternion& inOrientierung);
        virtual void drehe(const Quaternion& inDrehung);
        virtual Quaternion orientierung() const { return mOrientierung; }

        void setzeFarbe(const Farbe& inFarbe) { mFarbe = inFarbe; }
        void setzeGenauigkeit(const float& inGenauigkeit);

		virtual ~JuliaGeoObjekt() {}
        
        virtual void schnitt(const E3PunktVec& inStrahlursprung,
                              const R3VektorVec& inStrahlrichtung,
                              const float& inSichtweite, // sollte GeoObjekt selbst ermitteln
                              BoolVec& outSchneidet,
                              E3PunktVec& outSchnittpunkt,
                              R3VektorVec& outNormale,
                              FarbVec& outFarbe) const;
					
   		static volatile int64_t __attribute__ ((aligned (8))) anzSchritte; // debug
	
	protected:        
        float       mMinimalschritt;
        float       mSchrittfaktor;
        Farbe	  	mFarbe;
        Quaternion	mOrientierung; // Rotation durch Quaternion repräsentiert
        
    protected:
    	template<class T> QuaternionVec tripelZuQuaternion(const T& inTripel) const;
    	template<class T> T quaternionZuTripel(const QuaternionVec& inQ) const;
};

/* Rotation R durch Quaternionen q=(r,i,j,k):
 * (r',i',j',k') = R*(0,r,i,j)*conj(R)
 * => q' = (i',j',k',0);
 */

template<class T> QuaternionVec JuliaGeoObjekt::tripelZuQuaternion(const T& inTripel) const
{
    const QuaternionVec& q = mOrientierung*QuaternionVec(FloatVec(0.f),inTripel.x,inTripel.y,inTripel.z)*conj(mOrientierung);
	return QuaternionVec(q.i,q.j,q.k,FloatVec(0.f));
}

template<class T> T JuliaGeoObjekt::quaternionZuTripel(const QuaternionVec& inQ) const
{	
    const QuaternionVec& q = conj(mOrientierung)*QuaternionVec(FloatVec(0.f),inQ.r,inQ.i,inQ.j)*mOrientierung;	
	return T(q.i,q.j,q.k);
}

