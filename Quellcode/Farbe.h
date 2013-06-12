/**
 *
 *  Written in 2003 by Leo Fink (mail@leofink.de).
 *
 **/

#pragma once

#include <cmath>
#include <algorithm>
#include <iostream>

#include "Mathematik.h"

using namespace std;

class Farbe;

class Farbe32Bit {
    public:
		Farbe32Bit() : rot(0),gruen(0),blau(0),alpha(255) {}
        Farbe32Bit(const unsigned char& inRot,
                   const unsigned char& inGruen,
                   const unsigned char& inBlau,
                   const unsigned char& inAlpha=255) : rot(inRot),gruen(inGruen),blau(inBlau),alpha(inAlpha) {}
        
        Farbe32Bit(const unsigned char& inWeiss,
                   const unsigned char& inAlpha=255) : rot(inWeiss),gruen(inWeiss),blau(inWeiss),alpha(inAlpha) {}
        
		Farbe32Bit operator*(const Farbe32Bit& inF) const { return Farbe32Bit(rot*inF.rot,gruen*inF.gruen,blau*inF.blau,alpha); }
		
  		float tonAbstandZu(const Farbe32Bit& inFarbe) const;
		
		Farbe alsFarbe() const;
		
        unsigned char rot;
        unsigned char gruen;
        unsigned char blau;
        unsigned char alpha;
};

inline bool operator==(const Farbe32Bit& inF1,const Farbe32Bit& inF2)
{
	return inF1.rot==inF2.rot && inF1.gruen==inF2.gruen && inF1.blau==inF2.blau && inF1.alpha==inF2.alpha;
}

inline bool operator!=(const Farbe32Bit& inF1,const Farbe32Bit& inF2)
{
	return inF1.rot!=inF2.rot || inF1.gruen!=inF2.gruen || inF1.blau!=inF2.blau || inF1.alpha!=inF2.alpha;
}

class Farbe16Bit
{
    public:
        Farbe16Bit(const unsigned char& inRot,
                   const unsigned char& inGruen,
                   const unsigned char& inBlau) : komposition(inRot<<10 | inGruen<<5 | inBlau) {}
                
        unsigned short komposition;
};

class Farbe
{
    public:
        Farbe() : rot(0.f),gruen(0.f),blau(0.f),alpha(1.f) {}
        Farbe(const float& inRot,
              const float& inGruen,
              const float& inBlau,
              const float& inAlpha=1.f) : rot(inRot),gruen(inGruen),blau(inBlau),alpha(inAlpha) {}
        
        Farbe(const float& inWeiss,
              const float& inAlpha=1.f) : rot(inWeiss),gruen(inWeiss),blau(inWeiss),alpha(inAlpha) {}
        
        static const Farbe SCHWARZ;
        static const Farbe WEISS;
        
        Farbe operator*(const float& inS) const
        {
            return Farbe(rot*inS,
                         gruen*inS,
                         blau*inS,
                         alpha);
        }
        
        Farbe operator+(const Farbe& inF) const
        {
            return Farbe(rot+inF.rot,
                         gruen+inF.gruen,
                         blau+inF.blau,
                         alpha);
        }
		
		Farbe operator-(const Farbe& inF) const
        {
            return Farbe(rot-inF.rot,
                         gruen-inF.gruen,
                         blau-inF.blau,
                         alpha);
        }
  		
		Farbe gammaUnkorrigiert() const;
  		float tonAbstandZu(const Farbe& inFarbe) const; // euklidisch
        float helligkeit() const;
        float saettigung() const;
        
        void operator+=(const Farbe& inF) { rot+=inF.rot; gruen+=inF.gruen; blau+=inF.blau; }
        void operator*=(const float& inS) { rot*=inS; gruen*=inS; blau*=inS; }
        
        Farbe operator*(const Farbe& inF) const { return Farbe(rot*inF.rot,gruen*inF.gruen,blau*inF.blau,alpha); }
    
		Farbe alsXYZFarbe() const;
		
		Farbe alsCIEFarbe() const;
	
        Farbe32Bit als32BitFarbe() const
        { 
            return Farbe32Bit(limitiere(rot*255.f),
                              limitiere(gruen*255.f),
                              limitiere(blau*255.f),
                              (unsigned char)min(alpha*255.f,255.f));
        }
	
        /*Farbe16Bit als16BitFarbe() const { return Farbe16Bit((unsigned char)(rot*31),
                                                             (unsigned char)(gruen*31),
                                                             (unsigned char)(blau*31)); }*/
                                                             
        static unsigned char limitiere(const float& inX)
        {            
            const float K = 16.f;
						
			return (unsigned char)(inX/pow_approx(pow_approx(inX/255.f,K)+1,1.f/K));
        }
    
        float rot,gruen,blau,alpha;
};


inline std::ostream& operator<<(std::ostream& inOutStream, const Farbe& inF)
{
    inOutStream << "(" << inF.rot << "," << inF.gruen << "," << inF.blau << "," << inF.alpha << ")";
    return inOutStream;
}

inline std::ostream& operator<<(std::ostream& inOutStream, const Farbe32Bit& inF)
{
    inOutStream << "(" << int(inF.rot) << "," << int(inF.gruen) << "," << int(inF.blau) << "," << int(inF.alpha) << ")";
    return inOutStream;
}


inline bool operator==(const Farbe& inF1,const Farbe& inF2)
{
	//return inF1.rot==inF2.rot && inF1.gruen==inF2.gruen && inF1.blau==inF2.blau && inF1.alpha==inF2.alpha;
	const float EPS = .00390625; // 1/256
	return fabs(inF1.rot-inF2.rot)<EPS && fabs(inF1.gruen-inF2.gruen)<EPS && fabs(inF1.blau-inF2.blau)<EPS && fabs(inF1.alpha-inF2.alpha)<EPS;
}

inline bool operator!=(const Farbe& inF1,const Farbe& inF2)
{
	return !(inF1==inF2);
}



