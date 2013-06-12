/**
 *
 *  Written in 2003 by Leo Fink (mail@leofink.de).
 *
 **/

#include <iostream>
#include "Projektionsraum.h"


Projektionsraum::Projektionsraum(const E3Punkt& inBildschirmmitte,
                                 const R3Vektor& inBildschirmX,
                                 const R3Vektor& inBildschirmY,
                                 const float& inAugenentfernung,
                                 const float& inSichtwinkel,
                                 const float& inAugenabstand,
                                 const float& inGebietsradius,
                                 const vector<Lichtquelle>& inLichtquellen)
: mGebietsradius(inGebietsradius),
  mBildschirmmitte(inBildschirmmitte),
  mBildschirmX(inBildschirmX.normiert()),
  mBildschirmY(inBildschirmY.normiert()),
  mAugenentfernung(inAugenentfernung),
  mAugenabstand(inAugenabstand),
  mLichtquellen(inLichtquellen)
{ 	
	setzeSichtwinkel(inSichtwinkel);
}

float Projektionsraum::sichtweite() const
{
    return abs(augenpunkt()-E3Punkt(0,0,0))+mGebietsradius;
}

R3Vektor Projektionsraum::blickvektor(const float& inAnteilX,
                                       const float& inAnteilY,
                                       const Projektionsraum::Auge& inSeite) const
{
    return (mBildschirmmitte+mBildschirmX*(inAnteilX-.5)+mBildschirmY*(inAnteilY-.5)-augenpunkt(inSeite)).normiert();
    //return (mBildschirmmitte+mBildschirmX*(inAnteilX)+mBildschirmY*(inAnteilY-1)-augenpunkt(inSeite)).normiert();
	//return (mBildschirmmitte+mBildschirmX*(-.5)+mBildschirmY*(-.5)-augenpunkt(inSeite)).normiert();	
}

void Projektionsraum::setzeSichtwinkel(const float& inWinkel)
{
    mBildschirmX = mBildschirmX.normiert()*(2.f*mAugenentfernung*tan(inWinkel/2));
    mBildschirmY = mBildschirmY.normiert()*(2.f*mAugenentfernung*tan(inWinkel/2));
  
    mAugenpunkt[0] = mBildschirmmitte+kp(mBildschirmX,mBildschirmY).normiert()*mAugenentfernung-mBildschirmX.normiert()*(mAugenabstand/2);
    mAugenpunkt[1] = mAugenpunkt[0]+mBildschirmX.normiert()*mAugenabstand;
}

void Projektionsraum::setzeLichtquellen(const vector<Lichtquelle>& inLichtquellen)
{
    mLichtquellen = inLichtquellen;
}

void Projektionsraum::dreheLichteinfall(const int& inLichtquellenIndex, const float& inWinkel, const R3Vektor& inAchse)
//void Projektionsraum::dreheLichteinfall(const int& inLichtquellenIndex, const Quaternion& inDrehung)
{
	if (inLichtquellenIndex>=0 && inLichtquellenIndex<int(mLichtquellen.size()))
		mLichtquellen[inLichtquellenIndex].richtung.drehe(inWinkel,inAchse);
        //mLichtquellen[inLichtquellenIndex].richtung.drehe(inDrehung);
}


