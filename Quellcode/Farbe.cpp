/**
 *
 *  Written in 2003 by Leo Fink (mail@leofink.de).
 *
 **/

#include "Farbe.h"
#include "Mathematik.h"

const Farbe Farbe::SCHWARZ(0.,0.,0.,1.);
const Farbe Farbe::WEISS(1.,1.,1.,1.);

Farbe Farbe::gammaUnkorrigiert() const
{
	return Farbe(sqrt(rot),sqrt(gruen),sqrt(blau),alpha);
	//return *this;
}

float Farbe::tonAbstandZu(const Farbe& inFarbe) const // alpha?
{
	// euklidisch
	/*Farbe d(rot-inFarbe.rot,gruen-inFarbe.gruen,blau-inFarbe.blau);
	return sqrt(d.rot*d.rot+d.gruen*d.gruen+d.blau*d.blau);
	*/
	
	// euklidisch, normiert
	/*Farbe d(rot-inFarbe.rot,gruen-inFarbe.gruen,blau-inFarbe.blau);
	float h = (sqrt(rot*rot+gruen*gruen+blau*blau)+sqrt(inFarbe.rot*inFarbe.rot+inFarbe.gruen*inFarbe.gruen+inFarbe.blau*inFarbe.blau))*.5;
	return sqrt(d.rot*d.rot+d.gruen*d.gruen+d.blau*d.blau)/h*255.f;
	*/

	const Farbe a(abs(rot),abs(gruen),abs(blau));
	const Farbe b(abs(inFarbe.rot),abs(inFarbe.gruen),abs(inFarbe.blau));/**/

	// Inverse Gamma-Korrektur
	//float gamma = 1.f/1.8f;
	//Farbe a(pow(rot,gamma),pow(gruen,gamma),pow(blau,gamma));
	//Farbe b(pow(inFarbe.rot,gamma),pow(inFarbe.gruen,gamma),pow(inFarbe.blau,gamma));
	
	/*Farbe a(sqrt(abs(rot)),sqrt(abs(gruen)),sqrt(abs(blau)));
	Farbe b(sqrt(abs(inFarbe.rot)),sqrt(abs(inFarbe.gruen)),sqrt(abs(inFarbe.blau)));*/
	
	// Nach Thiadmer Riemersma, Compuphase
	float rm = (a.rot+b.rot)*.5f;
	const Farbe& d = a-b;
	return sqrt((2.f+rm)*d.rot*d.rot+4.f*d.gruen*d.gruen+(3.f-rm)*d.blau*d.blau)*.33333f+abs(alpha-inFarbe.alpha);
}

float Farbe::helligkeit() const
{
    //return wurzel(rot*rot+gruen*gruen+blau*blau); // euklidisch
    //return max(max(rot,gruen),blau); // V (HSV-Modell)
    return (rot+gruen+blau)*.33333f; // L (HSL-Modell)
}

float Farbe::saettigung() const
{
    float h = helligkeit();
    float m = min(min(rot,gruen),blau);
    return (h-m)/h; // S (HSV-Modell)
}

Farbe Farbe::alsXYZFarbe() const
{
	return Farbe(0.412453*rot+0.357580*gruen+0.180423*blau,
				 0.212671*rot+0.715160*gruen+0.072169*blau,
				 0.019334*rot+0.119193*gruen+0.950227*blau);
}

/*Farbe Farbe::alsCIEFarbe() const
{
	return ;
}*/


float Farbe32Bit::tonAbstandZu(const Farbe32Bit& inFarbe) const
{
	return alsFarbe().tonAbstandZu(inFarbe.alsFarbe())*255;
}

Farbe Farbe32Bit::alsFarbe() const
{
	float n = 1.f/255;
	return Farbe(n*rot,n*gruen,n*blau,n*alpha);
}


