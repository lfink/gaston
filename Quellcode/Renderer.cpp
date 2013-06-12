/**
*
 *  Written in 2003 by Leo Fink (mail@leofink.de).
 *
 **/

#include "Renderer.h"
#include "Parallel.h"
#include "VecMath.h"
#include "VecFarbe.h"

#include <iostream>


Renderer::Renderer(const Projektionsraum& inProjektionsraum,
				   const GeoObjekt& inObjekt,
				   const Farbe& inHintergrundfarbe,
				   const int& inAAQualitaet,
				   const bool& inMitSchlagschatten,
				   const bool& inStereoskopisch,
				   const int& inAnzSpalten,const int& inAnzZeilen,
				   const int& inAnzahlKachelnX,const int& inAnzahlKachelnY,
                   Farbe32Bit* ioBitmap[],
				   const int& inAnzBitmapSpalten,
				   unsigned char* ioAntialiasBitmap)
:	mProjektionsraum(inProjektionsraum),
	mObjekt(inObjekt),
	mHintergrundfarbe(inHintergrundfarbe),
	mAAQualitaet(inAAQualitaet),
	mMitSchlagschatten(inMitSchlagschatten),
	mStereoskopisch(inStereoskopisch),
	mAnzSpalten(inAnzSpalten),mAnzZeilen(inAnzZeilen),
	mAnzBitmapSpalten(inAnzBitmapSpalten),
	mAnzahlKachelnX(inAnzahlKachelnX),mAnzahlKachelnY(inAnzahlKachelnY),mKachelNr(0),
	mAntialiasBitmap(ioAntialiasBitmap),
	mSollWeitermachen(true),
	mAnzBerechneterPixel(0)
{
	for (int i=0;i<3;i++)
		mBitmap[i] = ioBitmap[i];
		
	setzeUnerledigt();
}

float Renderer::anteilErledigt() const
{
	const int dieAnzZuBerechnenderPixel = mAnzZeilen*mAnzSpalten*(mStereoskopisch?2:1)*(mAAQualitaet>1?2:1);
	return float(mAnzBerechneterPixel)/dieAnzZuBerechnenderPixel;
}

Kachel Renderer::kachelMitNr(const int& inNr) const
{
	const int kX = inNr%mAnzahlKachelnX;
	const int kY = inNr/mAnzahlKachelnX;
	
	return Kachel(kX*mAnzSpalten/mAnzahlKachelnX,(kX+1)*mAnzSpalten/mAnzahlKachelnX,
				  kY*mAnzZeilen/mAnzahlKachelnY,(kY+1)*mAnzZeilen/mAnzahlKachelnY);
}

void Renderer::rendere(const int& inKachelNr) const
{
	const Kachel& dieKachel = kachelMitNr(inKachelNr);
	
	for (int m=0;m<(mStereoskopisch?2:1);m++)
	{
		Projektionsraum::Auge dasAuge = mStereoskopisch?((m==0)?Projektionsraum::augeLinks:Projektionsraum::augeRechts):Projektionsraum::augeMono;
		Farbe32Bit* dieBitmap = mBitmap[m];
	
		berechneBild(dasAuge,dieBitmap,dieKachel);
		
		if (mAAQualitaet>1)
		{
			findeKanten(dieBitmap,dieKachel);
			berechneAntialiasing(dasAuge,dieBitmap,dieKachel);
		}
	}
}

/*void Renderer::rendere(const int& inKachelNr) const // DEBUG
{
	berechneBild(Projektionsraum::augeLinks,mBitmap[0],Kachel(50,51,50,51));
}*/


void Renderer::berechneBild(const Projektionsraum::Auge& inAuge,Farbe32Bit* ioBitmap,const Kachel& inKachel) const
{	
	Farbe32Bit dieMaske(1,1,1,1);
	
	switch (inAuge)
	{
		case Projektionsraum::augeLinks:
			dieMaske = Farbe32Bit(1,0,0,1);
			break;
		case Projektionsraum::augeRechts:
			dieMaske = Farbe32Bit(0,1,1,1);
			break;
		default:
			break;
	}

	for (int y=inKachel.yMin;y<inKachel.yMax && mSollWeitermachen;y+=PARY)
	{		
		for (int x=inKachel.xMin;x<inKachel.xMax;x+=PARX)
		{				
			R3VektorVec	b;	
			
			for (int sy=0;sy<PARY;sy++)
			{
				for (int sx=0;sx<PARX;sx++)
				{
					const R3Vektor& einBlickvektor = mProjektionsraum.blickvektor((float(x+sx)+.5f)/mAnzSpalten,1.f-(float(y+sy)+.5f)/mAnzZeilen,inAuge);
					const int i = sy*PARX+sx;
					b.x[i] = einBlickvektor.x;
					b.y[i] = einBlickvektor.y;
					b.z[i] = einBlickvektor.z;
				}
			}
			
			FarbVec	f;
			BoolVec h;
			
			berechneFarbe(mProjektionsraum.augenpunkt(inAuge),b,f,h);
			
			for (int sy=0;sy<PARY && y+sy<inKachel.yMax;sy++)
				for (int sx=0;sx<PARX && x+sx<inKachel.xMax;sx++)
					ioBitmap[(y+sy)*mAnzBitmapSpalten+x+sx] = f[sy*PARX+sx].als32BitFarbe()*dieMaske;
		}	
		
		mAnzBerechneterPixel += (inKachel.xMax-inKachel.xMin)*PARY; // nur grob, wenn Kachelbreite nicht glatt durch PARX geht
	}
}

void Renderer::findeKanten(Farbe32Bit* ioBitmap,const Kachel& inKachel) const
{
	// KachelrŠnder sind immer Kanten
	for (int y=inKachel.yMin;y<inKachel.yMax;y++)
	{
		for (int x=inKachel.xMin;x<inKachel.xMax;x++)
		{
			const bool derPixelLiegtAmRand = (x==inKachel.xMin || x==inKachel.xMax-1 || y==inKachel.yMin || y==inKachel.yMax-1);
			mAntialiasBitmap[y*mAnzBitmapSpalten+x] = derPixelLiegtAmRand?255:0;
		}
	}/**/
	
	// Edge detection durch Faltung mit Laplace-Kern
	for (int y=inKachel.yMin+1;y<inKachel.yMax-1 && mSollWeitermachen;y++)
	{
		for (int x=inKachel.xMin+1;x<inKachel.xMax-1;x++)
		{
			// mitte, rechts, oben, links, unten
			int i0 = y*mAnzBitmapSpalten+x;
			int i1 = i0+1;	
			int i2 = i0-mAnzBitmapSpalten;
			int i3 = i0-1;	
			int i4 = i0+mAnzBitmapSpalten;
			
			const Farbe& f0 = ioBitmap[i0].alsFarbe().gammaUnkorrigiert();
			const Farbe& f1 = ioBitmap[i1].alsFarbe().gammaUnkorrigiert();
			const Farbe& f2 = ioBitmap[i2].alsFarbe().gammaUnkorrigiert();
			const Farbe& f3 = ioBitmap[i3].alsFarbe().gammaUnkorrigiert();
			const Farbe& f4 = ioBitmap[i4].alsFarbe().gammaUnkorrigiert();
			
			Farbe dieLaplaceFaltung = f1+f2+f3+f4-f0*4.f;
			dieLaplaceFaltung.alpha = f1.alpha+f2.alpha+f3.alpha+f4.alpha-f0.alpha*4.f;
			
			unsigned char fd = min(dieLaplaceFaltung.tonAbstandZu(Farbe(0,0,0,0)),1.f)*255.f;

			mAntialiasBitmap[i0] = max(mAntialiasBitmap[i0],fd);
			mAntialiasBitmap[i1] = max(mAntialiasBitmap[i1],fd);
			mAntialiasBitmap[i2] = max(mAntialiasBitmap[i2],fd);
			mAntialiasBitmap[i3] = max(mAntialiasBitmap[i3],fd);
			mAntialiasBitmap[i4] = max(mAntialiasBitmap[i4],fd);
		}
	}/**/

}


void Renderer::berechneAntialiasing(const Projektionsraum::Auge& inAuge,
									Farbe32Bit* ioBitmap,
									const Kachel& inKachel) const
{
	long dieAnzNeuberechneter = 0;
	const int A = mAAQualitaet;
	Farbe32Bit dieMaske(1,1,1,1);
	const float dieKontrastschwelle = 10; // TODO: auslagern
	
	switch (inAuge)
	{
		case Projektionsraum::augeLinks:
			dieMaske = Farbe32Bit(1,0,0,1);
			break;
		case Projektionsraum::augeRechts:
			dieMaske = Farbe32Bit(0,1,1,1);
			break;
		default:
			break;
	}
	
	R3Vektor	b[256]; // Muss grš§er sein als A*A
	
	for (int y=inKachel.yMin;y<inKachel.yMax && mSollWeitermachen;y++)
	{					
		for (int x=inKachel.xMin;x<inKachel.xMax;x++)
		{
			if (mAntialiasBitmap[y*mAnzBitmapSpalten+x]>=dieKontrastschwelle)
			{						
				
				
				for (int sy=0;sy<A;sy++)
					for (int sx=0;sx<A;sx++)
						b[sy*A+sx] = mProjektionsraum.blickvektor((float(x)-float(A-1)/(2*A)+float(sx)/A+.5f)/mAnzSpalten,
																  1.f-(float(y)-float(A-1)/(2*A)+float(sy)/A+.5f)/mAnzZeilen,inAuge);
				Farbe fs(0);
				R3VektorVec	bv;	
				int n = 0;
				
				for (int k=0;k<(A*A+PARALLEL-1)/PARALLEL*PARALLEL;k++)
				{
					bv.x[k%PARALLEL] = b[min(k,A*A-1)].x;
					bv.y[k%PARALLEL] = b[min(k,A*A-1)].y;
					bv.z[k%PARALLEL] = b[min(k,A*A-1)].z;
					
					if ((k+1)%PARALLEL==0)
					{
						FarbVec	f;
						BoolVec h;
						
						berechneFarbe(mProjektionsraum.augenpunkt(inAuge),bv,f,h);
						
						for (int i=k-PARALLEL+1;i<min(k+1,A*A);i++)
						{
							if (!h[i%PARALLEL])
							{
								fs += f[i%PARALLEL];
								n++;
							}
						}
					}
				}
								
				fs.alpha = float(n)/(A*A);
				
				ioBitmap[y*mAnzBitmapSpalten+x] = (fs*(1.f/n)).als32BitFarbe()*dieMaske;
				
				dieAnzNeuberechneter++;
			}
		}
		
		mAnzBerechneterPixel += (inKachel.xMax-inKachel.xMin); // nur grob, wenn AA-Matrix nicht glatt durch PARX,PARY geht
	}/**/
	
	
	/*for (int y=inKachel.yMin;y<inKachel.yMax;y++)
	{
		for (int x=inKachel.xMin;x<inKachel.xMax;x++)
		{
			ioBitmap[y*mAnzBitmapSpalten+x] = Farbe32Bit(mAntialiasBitmap[y*mAnzBitmapSpalten+x]);
			if (mAntialiasBitmap[y*mAnzBitmapSpalten+x]>dieKontrastschwelle)
			{
				dieAnzNeuberechneter++;
				//ioBitmap[y*mAnzBitmapSpalten+x] = Farbe32Bit(255);
			}
			//else ioBitmap[y*mAnzBitmapSpalten+x] = Farbe32Bit(0);
		}
	}*/
	
	//cout << int(100.*dieAnzNeuberechneter/inKachel.anzahlPixel()) << "%" << endl;
}

void Renderer::rechneAlphaEin() const
{
	// "Pre-multiplication"
	const float c = 1.f/255;
	for (int m=0;m<(mStereoskopisch?2:1);m++)
	{
		for (int i=0;i<mAnzZeilen*mAnzBitmapSpalten;i++)
		{
			Farbe32Bit& p = mBitmap[m][i];
			const float a = float(p.alpha)*c;
			p = Farbe32Bit(a*p.rot,a*p.gruen,a*p.blau,p.alpha);
		}
	}
}

void Renderer::komponiereBild() const
{
	const Farbe32Bit& h = mHintergrundfarbe.als32BitFarbe();
	const float c = 1.f/255;
	
	if (!mStereoskopisch)
	{
		for (int i=0;i<mAnzZeilen*mAnzBitmapSpalten;i++)
		{
			const Farbe32Bit& l = mBitmap[0][i];
			
			float na = float(255-l.alpha)*c;
			
			mBitmap[2][i] = Farbe32Bit(l.rot+na*h.rot,l.gruen+na*h.gruen,l.blau+na*h.blau);
		}
	}
	else
	{
		for (int i=0;i<mAnzZeilen*mAnzBitmapSpalten;i++)
		{
			const Farbe32Bit& l = mBitmap[0][i];
			const Farbe32Bit& r = mBitmap[1][i];
			
			float lna = float(255-l.alpha)*c;
			float rna = float(255-r.alpha)*c;
			
			mBitmap[2][i] = Farbe32Bit(l.rot+lna*h.rot,r.gruen+rna*h.gruen,r.blau+rna*h.blau);
		}
	}
}


void Renderer::berechneFarbe(const E3Punkt& inStrahlursprung,
							 const R3VektorVec& inStrahlrichtung,
							 FarbVec& outFarbe,
							 BoolVec& outIstHintergrund) const
{
    const vector<Lichtquelle>& dieLichtquellen = mProjektionsraum.lichtquellen();
	
    FarbVec		dieFarbe;
    BoolVec		derStrahlSchneidet;
    E3PunktVec  derSchnittpunkt;
    R3VektorVec dieNormale;
	
	mObjekt.schnitt(E3PunktVec(inStrahlursprung),
					inStrahlrichtung,
					mProjektionsraum.sichtweite(),
					derStrahlSchneidet,
					derSchnittpunkt,
					dieNormale,
					dieFarbe);
    
	if (!teilweiseWahr(derStrahlSchneidet)) // brauch ich das? bringt das was?
	{
		outFarbe = FarbVec(mHintergrundfarbe);
		outIstHintergrund = true;
	}
	
	FarbVec dieLichteinfallFarbe(Farbe(0,0,0));
	FarbVec dieGlanzFarbe(Farbe(0,0,0));
	
	for (vector<Lichtquelle>::const_iterator l=dieLichtquellen.begin();l!=dieLichtquellen.end();l++)
    {
		R3VektorVec dieLichtrichtung(-l->richtung);
        BoolVec  derSchnittLiegtImSchatten(false);
		
        if (vollstaendigWahr(dieLichtrichtung*dieNormale<0.f)) // alle abgewandt
            continue;
		
		if(mMitSchlagschatten && !l->istDiffus)
        {			
			R3VektorVec n2; // dummy
            mObjekt.schnitt(derSchnittpunkt-.01f*l->richtung, // Konstante auslagern (eigentlich JuliaGeoObjekt.mMinimalschritt)
							dieLichtrichtung,2,
							derSchnittLiegtImSchatten,
							derSchnittpunkt,n2,
							dieFarbe);
        }
		
        const BoolVec& dieStelleWirdAngestrahlt = derStrahlSchneidet && (BoolVec(!mMitSchlagschatten) || !derSchnittLiegtImSchatten);
	    
        if (!teilweiseWahr(dieStelleWirdAngestrahlt))
            continue;
		
		const FloatVec& dieIntensitaet = maximum(dieLichtrichtung*dieNormale,0.f);
        dieLichteinfallFarbe = auswahl(dieStelleWirdAngestrahlt,dieLichteinfallFarbe+l->farbe*dieIntensitaet,dieLichteinfallFarbe);
        
        const R3VektorVec& derReflektierte = dieNormale*((inStrahlrichtung*dieNormale)*2.f)-inStrahlrichtung;
		
	    const FloatVec& w = abs(derReflektierte+dieLichtrichtung)*4.f; 
        const FloatVec& g = .75f/(pow(w,16)+1.f);
		
        dieGlanzFarbe = auswahl(BoolVec(!l->istDiffus) && dieStelleWirdAngestrahlt,dieGlanzFarbe+(l->farbe*g),dieGlanzFarbe);
	}
	
    outFarbe = auswahl(derStrahlSchneidet,dieFarbe*dieLichteinfallFarbe+dieGlanzFarbe,mHintergrundfarbe);
	outIstHintergrund = !derStrahlSchneidet;
}

Farbe Renderer::farbeFuerStereoskopie(const Farbe& inFarbe)
{   // Reines Rot und reines Cyan vermeiden     
    Farbe dieFarbe(inFarbe);
    const float MIN=.01,MAX=.99;
    dieFarbe.rot = min(max(dieFarbe.rot,MIN),MAX);
    dieFarbe.gruen = min(max(dieFarbe.gruen,MIN),MAX);
    dieFarbe.blau = min(max(dieFarbe.blau,MIN),MAX);
    
    float s = .9;
    float hgb = (dieFarbe.gruen+dieFarbe.blau)/2;
    float h = dieFarbe.helligkeit();
    
    dieFarbe.rot = (1.-s)*dieFarbe.rot+s*h;
    dieFarbe.gruen *= (1.-s)+s*h/hgb;
    dieFarbe.blau *= (1.-s)+s*h/hgb;
    
    return dieFarbe;
}



