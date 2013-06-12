/**
 *
 *  Written in 2003 by Leo Fink (leofink@mac.com).
 *
 **/


/**

GastonBench: Plattformunabhängiger Benchmark

Kompilieren gcc/xlC/icc:
PowerPC: g++ -O3 -o GastonBenchGCC Quellcode/ *.cpp
x86: g++ -arch x86_64 -O3 -o GastonBenchGCC Quellcode/ *.cpp

xlC -w -O3 -o GastonBenchXLC Quellcode/ *.cpp
icc -O1 -mp -vec-report0 -o GastonBenchICC Quellcode/ *.cpp

**/



#include <iostream>
#include <fstream>
//#include <sys/time.h>
#include <time.h> 


#include "Renderer.h"
#include "JuliaGeoObjekt.h"
#include "Kugel.h"

using namespace std;


void saveImage(Farbe32Bit* inBitmap,const int& inAufloesung);

int main(int argc, const char* argv[])
{
	int dieAufloesung = 1000;
    int dieAnzahlDurchlaeufe = 1;
	    
    if (argc>1)
    	dieAufloesung = atol(argv[1]);
    
    if (argc>2)
    	dieAnzahlDurchlaeufe = atol(argv[2]);
    
	dieAufloesung = (dieAufloesung+7)/8*8;
	
    vector<Lichtquelle> dieLichtquellen;   
    dieLichtquellen.push_back(Lichtquelle(R3Vektor(.5,-2,-2),Farbe::WEISS*1.25,false)); // plist?
    dieLichtquellen.push_back(Lichtquelle(R3Vektor(0,1,-1),Farbe::WEISS*.25,true));
    
    Projektionsraum derRaum(E3Punkt(0,0,.2),R3Vektor(1,0,0),R3Vektor(0,1,0),5,33.*acos(0.)/90,.5,2,dieLichtquellen);
	
	JuliaGeoObjekt dasObjekt(10,Quaternion(-.35,.7,.15,.3),0,3,Farbe(1,.689,.357));  // Farbe nicht brillenoptimiert
	dasObjekt.setzeOrientierung(Quaternion(0,.9689124217,0,-.2474039593));
	
	//JuliaGeoObjekt dasObjekt(5,Quaternion(0),0,2,Farbe(1,.689,.357));
	//dasObjekt.setzeOrientierung(Quaternion(1,0,0,0));
	
	//Kugel dasObjekt(E3Punkt(0,0,0),1,Farbe::WEISS);

	Farbe32Bit* dieBitmaps[3];
	
	for (int i=0;i<3;i++)
		dieBitmaps[i] = new Farbe32Bit[dieAufloesung*dieAufloesung];

	unsigned char* dieAABitmap = new unsigned char[dieAufloesung*dieAufloesung];
			
	Renderer derRenderer(derRaum,dasObjekt,
						 Farbe(.157,.196,.259),
						 4, // AA
						 true, // Schatten
						 false, // Stereo
						 dieAufloesung,dieAufloesung,
						 1,1, // Anz. Teilbilder
						 dieBitmaps,
						 dieAufloesung,
						 dieAABitmap);
       
    clock_t t = ::clock(); // user-time, nicht real
    
    for (int i=0;i<dieAnzahlDurchlaeufe;i++)
    {
		derRenderer.rendere(0);
		derRenderer.rechneAlphaEin();
		derRenderer.komponiereBild();
    }
	
	double dt = double(::clock()-t);
	
	/*long diePruefsumme = 0;
	for (int i=0;i<dieAufloesung*dieAufloesung;i++)
	{
		diePruefsumme += dieBitmap[i].rot;
	}
	
	cout << "Korrekt: " << (diePruefsumme/(dieAufloesung*dieAufloesung)==61) << endl;*/
	
    cout << "time: " << dt/CLOCKS_PER_SEC << " s";
    cout << ", steps: " << JuliaGeoObjekt::anzSchritte;
	cout << ", Msteps/s: " << 1e-6*JuliaGeoObjekt::anzSchritte*CLOCKS_PER_SEC/dt;
	cout << ", kpixels/s: " << 1e-3*dieAufloesung*dieAufloesung*CLOCKS_PER_SEC/dt;
	cout << ", GFLOPS: " << 1e-9*JuliaGeoObjekt::anzSchritte*(dasObjekt.iterationslimit()*46+38)*CLOCKS_PER_SEC/dt << endl;
	
	saveImage(dieBitmaps[2],dieAufloesung);
	
	delete[] dieAABitmap;
	for (int i=0;i<3;i++)
		delete[] dieBitmaps[i];
    
    return 0;
}

void saveImage(Farbe32Bit* inBitmap,const int& inAufloesung)
{
	// RGBA -> ARGB Interlaced
	for (int i=0;i<inAufloesung*inAufloesung;i++)
	{
		unsigned char h = inBitmap[i].alpha;
		inBitmap[i].alpha = inBitmap[i].blau;
		inBitmap[i].blau = inBitmap[i].gruen;
		inBitmap[i].gruen = inBitmap[i].rot;
		inBitmap[i].rot = h;
	}
	
	// TODO: BMP-Format?
	ofstream derStream("GastonBenchBild.raw",ofstream::binary);
	derStream.write((const char*)inBitmap,inAufloesung*inAufloesung*sizeof(Farbe32Bit));
	derStream.close();
}

