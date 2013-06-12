/**
 *
 *  Written in 2003 by Leo Fink (mail@leofink.de).
 *
 **/

#pragma once

#include "Farbe.h"
#include "R3Vektor.h"

class Lichtquelle
{
    public:
        Lichtquelle(const R3Vektor& inRichtung,
                    const Farbe&     inFarbe,
                    const bool&		 inIstDiffus = false)
                     : richtung(inRichtung.normiert()),farbe(inFarbe),istDiffus(inIstDiffus) {}
        
        R3Vektor richtung;
        Farbe    farbe;
        bool	  istDiffus;
};


