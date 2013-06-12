/**
 *
 *  Written in 2003 by Leo Fink (mail@leofink.de).
 *
 **/

#include <limits>
#include <algorithm>
#include <functional>
#include "GeoObjektGruppe.h"
#include "Farbe.h"

using namespace std;

GeoObjektGruppe::~GeoObjektGruppe()
{
    //for_each(elemente.begin(),elemente.end(),operator delete); // will gcc nicht...
    for (vector<GeoObjekt*>::iterator o=elemente.begin();o!=elemente.end();o++)
    	delete *o;
}

/*void GeoObjektGruppe::drehe(const float& inWinkel, const R3Vektor& inAchse)
{
    for (vector<GeoObjekt*>::iterator o=elemente.begin();o!=elemente.end();o++)
    	(**o).drehe(inWinkel,inAchse);
}

void GeoObjektGruppe::setzeUngedreht()
{
	for_each(elemente.begin(),elemente.end(),mem_fun(&GeoObjekt::setzeUngedreht));
}*/

void GeoObjektGruppe::setzeOrientierung(const Quaternion& inOrientierung)
{
    for (vector<GeoObjekt*>::iterator o=elemente.begin();o!=elemente.end();o++)
    	(**o).setzeOrientierung(inOrientierung);
}

void GeoObjektGruppe::drehe(const Quaternion& inDrehung)
{
    for (vector<GeoObjekt*>::iterator o=elemente.begin();o!=elemente.end();o++)
    	(**o).drehe(inDrehung);
}

void GeoObjektGruppe::fuegeHinzu(GeoObjekt* inObjekt)
{
    elemente.push_back(inObjekt);
}

void GeoObjektGruppe::schnitt(const E3PunktVec& inStrahlursprung,
                              const R3VektorVec& inStrahlrichtung,
                              const float& inSichtweite, // sollte GeoObjekt selbst ermitteln
                              BoolVec& outSchneidet,
                              E3PunktVec& outSchnittpunkt,
                              R3VektorVec& outNormale,
                              FarbVec& outFarbe) const
{
    /*bool derStrahlTrifft = false;
    float dieEntfernung = numeric_limits<float>::max();
    
    for (vector<GeoObjekt*>::const_iterator o=elemente.begin();o!=elemente.end();o++)
    {
        E3Punkt   derSchnittpunkt;
        R3Vektor  dieNormale;
        Farbe  	   dieFarbe(0,0,0);
        
        if ((**o).schnitt(inStrahlursprung,inStrahlrichtung,inSichtweite,derSchnittpunkt,dieNormale,dieFarbe))
        {
            float dieSchnittEntfernung = (derSchnittpunkt-inStrahlursprung).laenge();

            if (dieSchnittEntfernung<dieEntfernung)
            {
            	dieEntfernung = dieSchnittEntfernung;
                outSchnittpunkt = derSchnittpunkt;
                outNormale = dieNormale;
                outFarbe = dieFarbe;
                derStrahlTrifft = true;
            }
        }
    }
    
    return derStrahlTrifft;*/
}


