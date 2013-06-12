/**
 *
 *  Written in 2003 by Leo Fink (mail@leofink.de).
 *
 **/

 

#include <vector>
#include "GeoObjekt.h"

using namespace std;

class GeoObjektGruppe : public GeoObjekt
{
    public:
        virtual ~GeoObjektGruppe();
        
        void fuegeHinzu(GeoObjekt* inObjekt);

        //virtual void drehe(const float& inWinkel, const R3Vektor& inAchse);
        //virtual void setzeUngedreht();
        
        virtual void setzeOrientierung(const Quaternion& inOrientierung);
        virtual void drehe(const Quaternion& inDrehung);
        virtual Quaternion orientierung() const { return Quaternion(1); }

        virtual void schnitt(const E3PunktVec& inStrahlursprung,
                              const R3VektorVec& inStrahlrichtung,
                              const float& inSichtweite, // sollte GeoObjekt selbst ermitteln
                              BoolVec& outSchneidet,
                              E3PunktVec& outSchnittpunkt,
                              R3VektorVec& outNormale,
                              FarbVec& outFarbe) const;
                             		         
    protected:
        vector<GeoObjekt*> elemente;
};


