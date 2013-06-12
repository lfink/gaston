/**
 *
 *  Written in 2003 by Leo Fink (mail@leofink.de).
 *
 **/

#include "Kugel.h"
#include "VecMath.h"

void Kugel::schnitt(const E3PunktVec& inStrahlursprung,
                      const R3VektorVec& inStrahlrichtung,
                      const float& inSichtweite, // sollte GeoObjekt selbst ermitteln
                      BoolVec& outSchneidet,
                      E3PunktVec& outSchnittpunkt,
                      R3VektorVec& outNormale,
                      FarbVec& outFarbe) const
{
    const R3VektorVec& a = inStrahlursprung-mMitte;
    const R3VektorVec& b = inStrahlrichtung;
    
    //cout << "(" << inStrahlrichtung.x[0] << "," << inStrahlrichtung.y[0] << "," << inStrahlrichtung.z[0] << ")" << endl;
    //cout << derStrahlSchneidet[0] << endl;
    
    //cout << "a=(" << a.x[0] << "," << a.y[0] << "," << a.z[0] << ")" << endl;
    //cout << "b=(" << b.x[0] << "," << b.y[0] << "," << b.z[0] << ")" << endl;
    
    const FloatVec& ba = b*a;            
    
    //cout << "ba=" << ba[0] << endl;
    
    const FloatVec& dieDiskriminante = ba*ba - norm(a) + FloatVec(mRadius*mRadius);
    
    const FloatVec& dieEntfernung = -ba-sqrt(dieDiskriminante);
    
    outSchneidet = dieDiskriminante>0.f && dieEntfernung>0.f;
    
    outSchnittpunkt = inStrahlursprung+b*dieEntfernung;
    outNormale = (outSchnittpunkt-mMitte)*(1.f/mRadius);
    outFarbe = mFarbe;
}


