/**
 *
 *  Written in 2003 by Leo Fink (mail@leofink.de).
 *
 **/

#pragma once


#include "Parallel.h"

class Farbe;


class GeoObjekt
{
    public:
        virtual ~GeoObjekt() {}
        
        /*virtual void drehe(const float& inWinkel, const R3Vektor& inAchse) = 0;
        virtual void setzeUngedreht() = 0;*/
        
        virtual void setzeOrientierung(const Quaternion& inOrientierung) = 0;
        virtual void drehe(const Quaternion& inDrehung) = 0;
        virtual Quaternion orientierung() const = 0;

        virtual void schnitt(const E3PunktVec& inStrahlursprung,
                              const R3VektorVec& inStrahlrichtung,
                              const float& inSichtweite, // sollte GeoObjekt selbst ermitteln
                              BoolVec& outSchneidet,
                              E3PunktVec& outSchnittpunkt,
                              R3VektorVec& outNormale,
                              FarbVec& outFarbe) const = 0;
};


