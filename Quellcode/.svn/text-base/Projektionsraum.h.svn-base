/**
 *
 *  Written in 2003 by Leo Fink (mail@leofink.de).
 *
 **/

#pragma once

#include <vector>
#include "Lichtquelle.h"
#include "E3Punkt.h"
//#include "Quaternion.h"

using namespace std;


class Projektionsraum
{
    public:
        enum Auge { augeLinks=0,augeRechts=1,augeMono=2 };
		
        Projektionsraum(const E3Punkt& inBildschirmmitte,
                        const R3Vektor& inBildschirmX,
                        const R3Vektor& inBildschirmY,
                        const float& inAugenentfernung,
                        const float& inSichtwinkel,
                        const float& inAugenabstand,
                        const float& inGebietsradius, // sollte GeoObjekt selbst ermitteln
                        const vector<Lichtquelle>& inLichtquellen);
        
        R3Vektor blickvektor(const float& inAnteilX,
                              const float& inAnteilY,
                              const Auge& inSeite=augeMono) const;
                              
        float sichtweite() const;
        E3Punkt augenpunkt(const Auge& inSeite=augeMono) const;
        const vector<Lichtquelle>& lichtquellen() const { return mLichtquellen; }
        
        void setzeSichtwinkel(const float& inWinkel);
        void setzeLichtquellen(const vector<Lichtquelle>& inLichtquellen);
        
       	void dreheLichteinfall(const int& inLichtquellenIndex, const float& inWinkel, const R3Vektor& inAchse);
        //void dreheLichteinfall(const int& inLichtquellenIndex, const Quaternion& inDrehung);
        
    protected:
        E3Punkt  mAugenpunkt[2];
        float mGebietsradius;
        E3Punkt mBildschirmmitte;
        R3Vektor mBildschirmX;
        R3Vektor mBildschirmY;
        float mAugenentfernung;
        float mAugenabstand;
        vector<Lichtquelle> mLichtquellen;
        float mBlickaufweitung;
};

inline E3Punkt Projektionsraum::augenpunkt(const Projektionsraum::Auge& inSeite) const
{
    switch (inSeite)
    {
        case Projektionsraum::augeLinks:
            return mAugenpunkt[0];
            break;
        case Projektionsraum::augeRechts:
            return mAugenpunkt[1];
            break;
        default:
            return mAugenpunkt[0]+(mAugenpunkt[1]-mAugenpunkt[0])*.5f;
    }
}


