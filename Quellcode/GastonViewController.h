/* GastonViewController */

#import <Cocoa/Cocoa.h>

#import "GeoObjekt.h"
#import "Renderer.h"


@interface GastonViewController : NSView <NSApplicationDelegate>
{
    NSBitmapImageRep*	bitmap[3]; // Links, Rechts, Komponiert
	unsigned char*		antialiasBitmap;
    
    bool				benoetigtAktualisierung;
    NSPoint				klickPos;
	NSTimer*			fortschrittsanzeigeTimer;
	bool				abgebrochen;
	
    // Model
    GeoObjekt*          objekt;
    Projektionsraum* 	raum;
    Renderer*		 	renderer;

    float       winkel,winkelNeu;
    R3Vektor    *achse,*achseNeu;
    //Quaternion *orientierung;
    
	NSMutableArray* parallelLocks;
	NSConditionLock* berechnungLock;
	long		anzahlThreads;
    long		anzahlKachelnX;
	long		anzahlKachelnY;
    long		naechsteKachelNr;
	long		antialiasFaktor;
	
    NSDate*		startzeit;
    double		renderdauer;
        
    // Kontrollelemente 
    IBOutlet id bitmapgroesseFeld; // "Auflšsung"!
    IBOutlet id farbwaehler;
    IBOutlet id hintergrundfarbwaehler;
    IBOutlet id iterationslimitSchieber;
    IBOutlet id genauigkeitsSchieber;
    IBOutlet id rSchieber;
    IBOutlet id iSchieber;
    IBOutlet id jSchieber;
    IBOutlet id kSchieber;
    IBOutlet id winkelSchieber;
    IBOutlet id antialiasingSchalter;
    IBOutlet id schlagschattenSchalter;
    IBOutlet id stereoskopischSchalter;
    IBOutlet id statusFeld;
    IBOutlet id fortschrittsAnzeiger;
    IBOutlet id drehbarMenue;
    IBOutlet id entfernungsSchieber;
}

-(IBAction) oeffneParameter:(id)sender;
-(NSMutableDictionary*) defaultParameter;
-(IBAction) speichereParameter:(id)sender;

-(void) stelleBasisParameterEin;
-(void) stelleParameterEin:(NSDictionary*)inParameter;
-(NSDictionary*) aktuellEingestellteParameter;

-(void) bereiteBitmapsVor;
//-(void) maleBild:(NSRect)inRect ausBitmap:(NSBitmapImageRep*)inBitmap operation:(NSCompositingOperation)inOperation;
-(IBAction) starteBenchmark:(id)inSender;
-(IBAction) aktualisiere:(id)inSender;
-(void) aktualisiereThreaded;
-(void) starteBerechnung;
-(void) berechneBildParallel:(NSNumber*)inProzessNr;
-(void) schliesseBerechnungAb;
-(IBAction) brichAktionAb:(id)inSender;
-(NSString*) erzeugeInfoString;
-(void) setzeStatus:(NSString*)inString;
-(void) aktualisiereFortschrittsanzeige:(id)inSender;
-(void) setzeFarbe:(Farbe)inFarbe;
-(void) setzeHintergrundfarbe:(Farbe)inFarbe;
-(Farbe) farbe;
-(Farbe) hintergrundfarbe;
-(void) verarbeiteDrehungUmWinkel:(float)inWinkel umAchse:(R3Vektor*)inAchse;

-(IBAction) exportiereBild:(id)sender;
-(IBAction) exportiereBildOhneHintergrund:(id)sender;
-(IBAction) exportiereBildTransparent:(BOOL)inTransparent;
-(IBAction) setzeZurueck:(id)sender;

- (BOOL)application:(NSApplication *)theApplication openFile:(NSString *)filename;

@end


