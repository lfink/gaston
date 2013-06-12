/**
 *
 *  Written in 2003 by Leo Fink (mail@leofink.de).
 *
 **/
 
#include "GeoObjekt.h"
#include "Farbe.h"

// Beispielobjekt

class Kugel : public GeoObjekt
{
    public:
        Kugel(const E3Punkt& inMitte,
        	  const float& inRadius,
        	  const Farbe& inFarbe)
            : mMitte(inMitte), mRadius(inRadius), mFarbe(inFarbe) {}
        
        virtual ~Kugel() {}
        
        void setzeFarbe(const Farbe& inFarbe) { mFarbe = inFarbe; }
		
        virtual void setzeOrientierung(const Quaternion& inOrientierung) {};
        virtual void drehe(const Quaternion& inDrehung) {};
        virtual Quaternion orientierung() const { return Quaternion(1); };

        virtual void schnitt(const E3PunktVec& inStrahlursprung,
                              const R3VektorVec& inStrahlrichtung,
                              const float& inSichtweite, // sollte GeoObjekt selbst ermitteln
                              BoolVec& outSchneidet,
                              E3PunktVec& outSchnittpunkt,
                              R3VektorVec& outNormale,
                              FarbVec& outFarbe) const;
        
	protected:        
        E3Punkt   mMitte;
        float		mRadius;
        Farbe	   mFarbe;
};



