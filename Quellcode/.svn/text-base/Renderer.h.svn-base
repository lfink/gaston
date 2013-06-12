/**
 *
 *  Written in 2003 by Leo Fink (mail@leofink.de).
 *
 **/


#include "Projektionsraum.h"
#include "GeoObjekt.h"

using namespace std;

class Kachel
{
public:
	Kachel(const int& inXMin,const int& inXMax,
		   const int& inYMin,const int& inYMax) : xMin(inXMin),xMax(inXMax),yMin(inYMin),yMax(inYMax) {}

	int anzahlPixel() const { return (xMax-xMin)*(yMax-yMin); }

	int xMin,xMax,yMin,yMax;
};

class Renderer
{
public:
	Renderer(const Projektionsraum& inProjektionsraum,
			 const GeoObjekt& inObjekt,
			 const Farbe& inHintergrundfarbe,
			 const int& inAAQualitaet, // N: NxN-Antialiasing
			 const bool& inMitSchlagschatten,
			 const bool& inStereoskopisch,
			 const int& inAnzSpalten,const int& inAnzZeilen,
			 const int& inAnzahlKachelnX,const int& inAnzahlKachelnY,
			 Farbe32Bit* ioBitmap[],
			 const int& inAnzPixelProBitmapZeile,
			 unsigned char* ioAntialiasBitmap = 0);
	
	Kachel kachelMitNr(const int& inNr) const;
	void rendere(const int& inKachelNr) const;
	void berechneBild(const Projektionsraum::Auge& inAuge, Farbe32Bit* ioBitmap, const Kachel& inKachel) const;
	void findeKanten(Farbe32Bit* ioBitmap, const Kachel& inKachel) const;
	void berechneAntialiasing(const Projektionsraum::Auge& inAuge, Farbe32Bit* inBitmap, const Kachel& inKachel) const;
	void rechneAlphaEin() const;
	void komponiereBild() const;
	
	float anteilErledigt() const;
	void setzeUnerledigt() { mAnzBerechneterPixel = 0; mSollWeitermachen = true; }
	void setzeErledigt() { mSollWeitermachen = false; }
	int anzahlBerechneterPixel() const { return mAnzBerechneterPixel; }
	int anzahlKacheln() const { return mAnzahlKachelnX*mAnzahlKachelnY; }
	
	static Farbe farbeFuerStereoskopie(const Farbe& inFarbe); // für Rot-Cyan-Stereoskopie

protected:
			
	void berechneFarbe(const E3Punkt& inStrahlursprung,
					   const R3VektorVec& inStrahlrichtung,
					   FarbVec& outFarbe,
					   BoolVec& outIstHintergrund) const;
	
	const Projektionsraum&	mProjektionsraum;
	const GeoObjekt& 		mObjekt;
	Farbe 					mHintergrundfarbe;
	int						mAAQualitaet; // NxN-Antialiasing
	bool					mMitSchlagschatten;
	bool                    mStereoskopisch;
	int 					mAnzSpalten,mAnzZeilen;
	int						mAnzBitmapSpalten;
	int						mKachelNr;
	int						mAnzahlKachelnX,mAnzahlKachelnY;
	Farbe32Bit*				mBitmap[3];
	unsigned char*			mAntialiasBitmap;
	
	volatile mutable int	mAnzBerechneterPixel;
	bool		  			mSollWeitermachen;
};


