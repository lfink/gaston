/**
 *
 *  Written in 2003 by Leo Fink (mail@leofink.de).
 *
 **/

#import "JuliaGeoObjekt.h"
#import "Kugel.h"
#import "GastonViewController.h"
#import "Parallel.h"

#include <iostream>


@implementation GastonViewController

- (id)initWithFrame:(NSRect)frame
{
    self = [super initWithFrame:frame];
    
    if (self)
    {	    
        [self stelleBasisParameterEin];
                
        renderer = nil;
        bitmap[0] = bitmap[1] = bitmap[2] = nil;
		antialiasBitmap = nil;

        benoetigtAktualisierung = false;
        fortschrittsanzeigeTimer = nil;
        abgebrochen = false;
        
        achse = new R3Vektor();
        achseNeu = new R3Vektor();
        winkel = winkelNeu = 0;

		anzahlThreads = 16; //::MPProcessorsScheduled();	// so viele wie Prozessoren
        anzahlKachelnX = 8;
        anzahlKachelnY = 8;
		
		parallelLocks = [[NSMutableArray alloc] init];
		for (int i=0;i<anzahlThreads;i++)
			[parallelLocks addObject:[[NSConditionLock alloc] initWithCondition:false]];
		berechnungLock = [[NSConditionLock alloc] initWithCondition:false];
		
        [[NSApplication sharedApplication] setDelegate:self];
        
        [self setzeStatus:NSLocalizedString(@"STARTING",@"")];
    }
    
    return self;
}

-(NSMutableDictionary*) defaultParameter // nur die Einstellbaren
{
    NSString* derPfad = [[NSBundle mainBundle] pathForResource:@"Default" ofType:@"plist"];
    return [NSMutableDictionary dictionaryWithContentsOfFile:derPfad];
}

-(IBAction)oeffneParameter:(id)sender
{
    int dasErgebnis;
    NSArray* dieDateitypen = [NSArray arrayWithObject:@"gpm"];
    NSOpenPanel *oPanel = [NSOpenPanel openPanel];

    [oPanel setAllowsMultipleSelection:NO];

    dasErgebnis = [oPanel runModalForTypes:dieDateitypen];

    if (dasErgebnis == NSOKButton)
    {
        NSString* derPfad = [[oPanel filenames] objectAtIndex:0];
        
        [self stelleParameterEin:[NSMutableDictionary dictionaryWithContentsOfFile:derPfad]];
        [self aktualisiere:self];
    }
}

- (BOOL)application:(NSApplication *)inApplication openFile:(NSString *)inPfad
{
	[self stelleParameterEin:[NSMutableDictionary dictionaryWithContentsOfFile:inPfad]];
    [self aktualisiere:self];
}

-(IBAction)speichereParameter:(id)sender
{
    NSSavePanel* derDialog;
    int dasErgebnis;
    
    derDialog = [NSSavePanel savePanel];

    //[derDialog setAccessoryView:self];
    [derDialog setRequiredFileType:@"gpm"];

    dasErgebnis = [derDialog runModalForDirectory:NSHomeDirectory() file:@""];
    
    if (dasErgebnis == NSOKButton)
    {
        NSDictionary* d = [self aktuellEingestellteParameter];
        if (![d writeToFile:[derDialog filename] atomically:YES]) // TODO: Fehler abfangen
             NSBeep();
    }
}

-(void) stelleBasisParameterEin // Initialisierung - nicht die einstellbaren
{    
    NSString* derPfad = [[NSBundle mainBundle] pathForResource:@"Parameters" ofType:@"plist"];
    NSDictionary* d = [NSMutableDictionary dictionaryWithContentsOfFile:derPfad];
	
	vector<Lichtquelle> dieLichtquellen; // passiert unten nochmal...?
    NSEnumerator* e = [[d objectForKey:@"lightSources"] objectEnumerator];
    NSDictionary* l = nil;
    while ((l = [e nextObject]) != nil)
    {
        dieLichtquellen.push_back(Lichtquelle(R3Vektor([[[l objectForKey:@"direction"] objectAtIndex:0] floatValue],
                                                        [[[l objectForKey:@"direction"] objectAtIndex:1] floatValue],
                                                        [[[l objectForKey:@"direction"] objectAtIndex:2] floatValue]),
                                              Farbe([[[l objectForKey:@"color"] objectAtIndex:0] floatValue],
                                                    [[[l objectForKey:@"color"] objectAtIndex:1] floatValue],
                                                    [[[l objectForKey:@"color"] objectAtIndex:2] floatValue]),
                                              [[l objectForKey:@"isDiffuse"] boolValue]));
    }
    	
    raum = new Projektionsraum(E3Punkt([[[d objectForKey:@"screenCenter"] objectAtIndex:0] floatValue],
                                        [[[d objectForKey:@"screenCenter"] objectAtIndex:1] floatValue],
                                        [[[d objectForKey:@"screenCenter"] objectAtIndex:2] floatValue]),
                               R3Vektor([[[d objectForKey:@"screenX"] objectAtIndex:0] floatValue],
                                         [[[d objectForKey:@"screenX"] objectAtIndex:1] floatValue],
                                         [[[d objectForKey:@"screenX"] objectAtIndex:2] floatValue]),
                               R3Vektor([[[d objectForKey:@"screenY"] objectAtIndex:0] floatValue],
                                         [[[d objectForKey:@"screenY"] objectAtIndex:1] floatValue],
                                         [[[d objectForKey:@"screenY"] objectAtIndex:2] floatValue]),
                               [[d objectForKey:@"eyeScreenDistance"] floatValue],
                               [[d objectForKey:@"viewingAngle"] floatValue]*acos(0.)/90,
                               [[d objectForKey:@"eyeEyeDistance"] floatValue],
                               2,dieLichtquellen);
    
    NSDictionary* m = [d objectForKey:@"juliaSet"];
    
    objekt = new JuliaGeoObjekt([[m objectForKey:@"limitIterations"] intValue],
                                Quaternion([[m objectForKey:@"1"] floatValue],
                                           [[m objectForKey:@"i"] floatValue],
                                           [[m objectForKey:@"j"] floatValue],
                                           [[m objectForKey:@"k"] floatValue]),
                                           [[m objectForKey:@"theta"] floatValue],
                                           [[d objectForKey:@"precision"] floatValue],
                                Farbe([[[d objectForKey:@"color"] objectAtIndex:0] floatValue],
                                      [[[d objectForKey:@"color"] objectAtIndex:1] floatValue],
                                      [[[d objectForKey:@"color"] objectAtIndex:2] floatValue]));
    
    /*objekt = new Kugel(E3Punkt(0,0,0),1,Farbe([[[d objectForKey:@"color"] objectAtIndex:0] floatValue],
                                      [[[d objectForKey:@"color"] objectAtIndex:1] floatValue],
                                      [[[d objectForKey:@"color"] objectAtIndex:2] floatValue]));*/ 
	
	antialiasFaktor = [[d objectForKey:@"antialiasFactor"] intValue];
}

-(void) stelleParameterEin:(NSDictionary*)inParameter // nur die im GUI einstellbaren
{
    NSDictionary* d = inParameter;
    
    [self setzeFarbe:Farbe([[[d objectForKey:@"color"] objectAtIndex:0] floatValue],
                           [[[d objectForKey:@"color"] objectAtIndex:1] floatValue],
                           [[[d objectForKey:@"color"] objectAtIndex:2] floatValue])];
	[self setzeHintergrundfarbe:Farbe([[[d objectForKey:@"backgroundColor"] objectAtIndex:0] floatValue],
                                      [[[d objectForKey:@"backgroundColor"] objectAtIndex:1] floatValue],
                                      [[[d objectForKey:@"backgroundColor"] objectAtIndex:2] floatValue])];
	
    NSDictionary* m = [d objectForKey:@"juliaSet"];
    
	[rSchieber setFloatValue:[[m objectForKey:@"1"] floatValue]];
	[iSchieber setFloatValue:[[m objectForKey:@"i"] floatValue]];
	[jSchieber setFloatValue:[[m objectForKey:@"j"] floatValue]];
	[kSchieber setFloatValue:[[m objectForKey:@"k"] floatValue]];
	[winkelSchieber setFloatValue:[[m objectForKey:@"theta"] floatValue]];
	
	[iterationslimitSchieber setIntValue:[[m objectForKey:@"limitIterations"] intValue]];
	[genauigkeitsSchieber setFloatValue:[[d objectForKey:@"precision"] floatValue]];
	
    [bitmapgroesseFeld setIntValue:[[d objectForKey:@"resolution"] intValue]];
	[antialiasingSchalter setIntValue:[[d objectForKey:@"antialias"] boolValue]];
    [schlagschattenSchalter setIntValue:[[d objectForKey:@"castShadow"] boolValue]];
    [stereoskopischSchalter setIntValue:[[d objectForKey:@"stereoscopic"] boolValue]];

    [entfernungsSchieber setFloatValue:[[d objectForKey:@"viewingAngle"] floatValue]];
    
    NSArray* r = [d objectForKey:@"orientation"];

    objekt->setzeOrientierung(Quaternion([[r objectAtIndex:0] floatValue],[[r objectAtIndex:1] floatValue],[[r objectAtIndex:2] floatValue],[[r objectAtIndex:3] floatValue]));
    
    vector<Lichtquelle> dieLichtquellen;
    NSEnumerator* e = [[d objectForKey:@"lightSources"] objectEnumerator];
    NSDictionary* l = nil;
    while ((l = [e nextObject]) != nil)
    {
        dieLichtquellen.push_back(Lichtquelle(R3Vektor([[[l objectForKey:@"direction"] objectAtIndex:0] floatValue],
                                                        [[[l objectForKey:@"direction"] objectAtIndex:1] floatValue],
                                                        [[[l objectForKey:@"direction"] objectAtIndex:2] floatValue]),
                                              Farbe([[[l objectForKey:@"color"] objectAtIndex:0] floatValue],
                                                    [[[l objectForKey:@"color"] objectAtIndex:1] floatValue],
                                                    [[[l objectForKey:@"color"] objectAtIndex:2] floatValue]),
                                              [[l objectForKey:@"isDiffuse"] boolValue]));
    }
    
    raum->setzeLichtquellen(dieLichtquellen);
}

-(NSDictionary*) aktuellEingestellteParameter
{
    NSMutableDictionary* d = [self defaultParameter];
    
    // Farben (Juliamenge, Hintergrund)
    const Farbe& f = [self farbe];
    NSMutableArray* c = [d objectForKey:@"color"];
    [c replaceObjectAtIndex:0 withObject:[NSNumber numberWithFloat:f.rot]];
    [c replaceObjectAtIndex:1 withObject:[NSNumber numberWithFloat:f.gruen]];
    [c replaceObjectAtIndex:2 withObject:[NSNumber numberWithFloat:f.blau]];
    c = [d objectForKey:@"backgroundColor"];
    const Farbe& h = [self hintergrundfarbe];
    [c replaceObjectAtIndex:0 withObject:[NSNumber numberWithFloat:h.rot]];
    [c replaceObjectAtIndex:1 withObject:[NSNumber numberWithFloat:h.gruen]];
    [c replaceObjectAtIndex:2 withObject:[NSNumber numberWithFloat:h.blau]];
    
    // Juliamenge
    NSMutableDictionary* m = [d objectForKey:@"juliaSet"];
    [m setObject:[NSNumber numberWithFloat:[rSchieber floatValue]] forKey:@"1"];
    [m setObject:[NSNumber numberWithFloat:[iSchieber floatValue]] forKey:@"i"];
    [m setObject:[NSNumber numberWithFloat:[jSchieber floatValue]] forKey:@"j"];
    [m setObject:[NSNumber numberWithFloat:[kSchieber floatValue]] forKey:@"k"];
    [m setObject:[NSNumber numberWithFloat:[winkelSchieber floatValue]] forKey:@"theta"];
    [m setObject:[NSNumber numberWithFloat:[iterationslimitSchieber floatValue]] forKey:@"limitIterations"];
    
    // Geometrie
    [d setObject:[NSNumber numberWithFloat:[entfernungsSchieber floatValue]] forKey:@"viewingAngle"];
    NSMutableArray* r = [d objectForKey:@"orientation"];
    [r replaceObjectAtIndex:0 withObject:[NSNumber numberWithFloat:objekt->orientierung().r]];
    [r replaceObjectAtIndex:1 withObject:[NSNumber numberWithFloat:objekt->orientierung().i]];
    [r replaceObjectAtIndex:2 withObject:[NSNumber numberWithFloat:objekt->orientierung().j]];
    [r replaceObjectAtIndex:3 withObject:[NSNumber numberWithFloat:objekt->orientierung().k]];
    NSMutableArray* l = [d objectForKey:@"lightSources"];
    for (int i=0;i<raum->lichtquellen().size();++i)
    {
        NSMutableDictionary* l = [[d objectForKey:@"lightSources"] objectAtIndex:i];
        r = [l objectForKey:@"direction"];
        [r replaceObjectAtIndex:0 withObject:[NSNumber numberWithFloat:raum->lichtquellen()[i].richtung.x]];
        [r replaceObjectAtIndex:1 withObject:[NSNumber numberWithFloat:raum->lichtquellen()[i].richtung.y]];
        [r replaceObjectAtIndex:2 withObject:[NSNumber numberWithFloat:raum->lichtquellen()[i].richtung.z]];
        c = [l objectForKey:@"color"];
        [c replaceObjectAtIndex:0 withObject:[NSNumber numberWithFloat:raum->lichtquellen()[i].farbe.rot]];
        [c replaceObjectAtIndex:1 withObject:[NSNumber numberWithFloat:raum->lichtquellen()[i].farbe.gruen]];
        [c replaceObjectAtIndex:2 withObject:[NSNumber numberWithFloat:raum->lichtquellen()[i].farbe.blau]];
        [l setObject:[NSNumber numberWithBool:raum->lichtquellen()[i].istDiffus] forKey:@"isDiffuse"];
    }

    // Renderer
    [d setObject:[NSNumber numberWithFloat:[genauigkeitsSchieber floatValue]] forKey:@"precision"];
    [d setObject:[NSNumber numberWithInt:[bitmapgroesseFeld intValue]] forKey:@"resolution"];
    [d setObject:[NSNumber numberWithBool:[antialiasingSchalter intValue]] forKey:@"antialias"];
	[d setObject:[NSNumber numberWithBool:[schlagschattenSchalter intValue]] forKey:@"castShadow"];
    [d setObject:[NSNumber numberWithBool:[stereoskopischSchalter intValue]] forKey:@"stereoscopic"];
    
    return d;
}

-(void) applicationDidFinishLaunching:(NSNotification*)inNotification
{
    [winkelSchieber setMaxValue:acos(0.)*2]; // Interface Builder kann nur zweistellige Genauigkeit
    [[self window] setExcludedFromWindowsMenu:YES];
	//[[self window] setOpaque:NO];
	
	fortschrittsanzeigeTimer = [[NSTimer scheduledTimerWithTimeInterval:.2
    								     target:self
    								     selector:@selector(aktualisiereFortschrittsanzeige:)
    								     userInfo:nil repeats:YES] retain];
	if (renderer == NULL) // Start ohne geladene Datei
	{
		[self stelleParameterEin:[self defaultParameter]];
		[self aktualisiere:self];
    }
}

- (void)bereiteBitmapsVor
{
    int dieNeueAufloesung = [bitmapgroesseFeld intValue];
	    
    if (bitmap[0]==nil || // noch gar keine da
        (dieNeueAufloesung>0 && dieNeueAufloesung != [bitmap[0] pixelsWide])) // Größe verändert
    {
        for (int i=0;i<3;i++)
		{
			[bitmap[i] release];

			bitmap[i] = [[NSBitmapImageRep alloc] initWithBitmapDataPlanes:nil
											pixelsWide:dieNeueAufloesung
											pixelsHigh:dieNeueAufloesung
											bitsPerSample:8
											samplesPerPixel:4
											hasAlpha:YES
											isPlanar:NO
											colorSpaceName:NSDeviceRGBColorSpace
											bytesPerRow:0
											bitsPerPixel:0];
		}
		
		delete[] antialiasBitmap;	
										
		antialiasBitmap = new unsigned char[[bitmap[0] bytesPerRow]/4*[bitmap[0] bytesPerRow]/4];
		
		[[self window] setTitle:[NSString stringWithFormat:@"%dx%d pixels",dieNeueAufloesung,dieNeueAufloesung]];
    }
}

-(BOOL) isOpaque
{
	return NO; // Transparent: NO
}

-(void) dealloc
{      
    [parallelLocks release];
	[berechnungLock release];
    
    delete achse;
    delete achseNeu;
    delete renderer;
    delete raum;
    delete objekt;
    
	for (int i=0;i<3;i++)
		[bitmap[i] release];
    
    [super dealloc];
}

-(BOOL) acceptsFirstMouse:(NSEvent*)inEvent
{
	return YES;
}

-(void) drawRect:(NSRect)inRect
{        
    [[NSGraphicsContext currentContext] setImageInterpolation:NSImageInterpolationHigh];
    //[[NSGraphicsContext currentContext] setImageInterpolation:NSImageInterpolationNone];
    
    float dieGroesse = max(inRect.size.width,inRect.size.height); // zur Erhaltung des Seitenverhältisses
    inRect.origin.x = (inRect.size.width-dieGroesse)/2;
    inRect.origin.y = (inRect.size.height-dieGroesse)/2;
    inRect.size = NSMakeSize(dieGroesse,dieGroesse);
        
    if (bitmap[2]!=nil)
		[bitmap[2] drawInRect:inRect];
}

-(NSString*) erzeugeInfoString
{
	const float derSchrittdurchsatz = 1e-6*JuliaGeoObjekt::anzSchritte/renderdauer; // MSteps/s
	const float derPixeldurchsatz = 1e-3*renderer->anzahlBerechneterPixel()/renderdauer; // kPixels/s
	//const int64_t dieAnzGleitkommaOps = JuliaGeoObjekt::anzSchritte*([iterationslimitSchieber intValue]*52+69); // gezählt AltiVec
	const int64_t dieAnzGleitkommaOps = JuliaGeoObjekt::anzSchritte*([iterationslimitSchieber intValue]*46+38); // gemessen SSE
	const float derGleitkommaDurchsatz = 1e-9*dieAnzGleitkommaOps/renderdauer; // GFLOPS
	
	/*NSLog(@"t=%.2f s, Res=%dx%d, Steps=%lld, FloatOps=%lld, MSteps/s=%.3f, kPixels/s=%.2f, GFLOPS=%.2f\n",
		renderdauer,
		[bitmap[0] pixelsWide],[bitmap[0] pixelsHigh],
		JuliaGeoObjekt::anzSchritte,
		dieAnzGleitkommaOps,
		derSchrittdurchsatz,
		derPixeldurchsatz,
		derGleitkommaDurchsatz);*/
	
	return [NSString localizedStringWithFormat:@"%.2f s (%.2f GFLP; %.2f MP/s)",renderdauer,derGleitkommaDurchsatz,derPixeldurchsatz*.001];
}

-(void) setzeStatus:(NSString*)inString
{
	[inString retain];
	if (inString != nil && ![inString isEqualToString:[statusFeld stringValue]])
	{
		[statusFeld setStringValue:inString];
		//[statusFeld display]; // Deadlock?
	}
	[inString release];
}

-(void) aktualisiereFortschrittsanzeige:(id)inSender
{
    if (startzeit != nil && -[startzeit timeIntervalSinceNow]>[fortschrittsanzeigeTimer timeInterval])
	{
		if (!abgebrochen)
			[self setzeStatus:NSLocalizedString(@"COMPUTING",@"")];
		[fortschrittsAnzeiger setDoubleValue:renderer->anteilErledigt()];
		//[self display]; // TEST
	}
}

-(Farbe) farbe
{ 
	NSColor* g = [[farbwaehler color] colorUsingColorSpaceName:NSDeviceRGBColorSpace];
    return Farbe([g redComponent],[g greenComponent],[g blueComponent]);
}

-(Farbe) hintergrundfarbe
{	
	NSColor* g = [[hintergrundfarbwaehler color] colorUsingColorSpaceName:NSDeviceRGBColorSpace];
    return Farbe([g redComponent],[g greenComponent],[g blueComponent],0.0); // Transparent: 0.0
}

- (void) setzeFarbe:(Farbe)inFarbe
{
	NSColor* dieFarbe = [NSColor colorWithDeviceRed:inFarbe.rot green:inFarbe.gruen blue:inFarbe.blau alpha:inFarbe.alpha];
	[farbwaehler setColor:dieFarbe];
}

-(void) setzeHintergrundfarbe:(Farbe)inFarbe
{
	NSColor* dieFarbe = [NSColor colorWithDeviceRed:inFarbe.rot green:inFarbe.gruen blue:inFarbe.blau alpha:inFarbe.alpha];
	[hintergrundfarbwaehler setColor:dieFarbe];
}

-(IBAction) brichAktionAb:(id)inSender
{
	abgebrochen = true;
    benoetigtAktualisierung = false;
	renderer->setzeErledigt();
	[self setzeStatus:NSLocalizedString(@"CANCELLED",@"")];
}

- (IBAction)starteBenchmark:(id)inSender
{
    NSString* derPfad = [[NSBundle mainBundle] pathForResource:@"Benchmark" ofType:@"plist"];
    [self stelleParameterEin:[NSMutableDictionary dictionaryWithContentsOfFile:derPfad]];
    [self aktualisiere:self];
}


-(IBAction) aktualisiere:(id)inSender
{	
	if ([berechnungLock tryLockWhenCondition:false]) // Erstes Event gewinnt ...
	{
		Quaternion dieJuliaKonstante([rSchieber floatValue],[iSchieber floatValue],[jSchieber floatValue],[kSchieber floatValue]);
		
		JuliaGeoObjekt* dasObjekt = (JuliaGeoObjekt*)objekt;
		dasObjekt->setzeIterationslimit([iterationslimitSchieber intValue]);
		dasObjekt->setzeKonstante(dieJuliaKonstante);
		dasObjekt->setzeWinkel([winkelSchieber floatValue]);
		dasObjekt->setzeGenauigkeit([genauigkeitsSchieber floatValue]);
		dasObjekt->setzeFarbe([stereoskopischSchalter intValue]?Renderer::farbeFuerStereoskopie([self farbe]):[self farbe]);
		
		raum->setzeSichtwinkel([entfernungsSchieber floatValue]*acos(0.)/90);
		
		[self bereiteBitmapsVor];
		
		Farbe32Bit* dieBitmaps[3];
	
		for (int i=0;i<3;i++)
			dieBitmaps[i] = reinterpret_cast<Farbe32Bit*>([bitmap[i] bitmapData]);
		
		delete renderer;   
		renderer = new Renderer(*raum,*objekt,[stereoskopischSchalter intValue]?Renderer::farbeFuerStereoskopie([self hintergrundfarbe]):[self hintergrundfarbe],
								[antialiasingSchalter intValue]?antialiasFaktor:1,
								[schlagschattenSchalter intValue],
								[stereoskopischSchalter intValue],
								[bitmap[0] pixelsWide],[bitmap[0] pixelsHigh],
								anzahlKachelnX,anzahlKachelnY,
								dieBitmaps,
								[bitmap[0] bytesPerRow]/4,
								antialiasBitmap);
		
		// Berechnung in Thread abzweigen
		[NSThread detachNewThreadSelector:@selector(aktualisiereThreaded) toTarget:self withObject:nil];
		
		[berechnungLock unlockWithCondition:true];
	}
	else
		benoetigtAktualisierung = true; // ... und folgende melden nur
}

-(void) aktualisiereThreaded
{
	NSAutoreleasePool* derPool = [[NSAutoreleasePool alloc] init]; 
	
	[self starteBerechnung];
		
	//[parallelLocks makeObjectsPerformSelector:@selector(lockWhenCondition:) withObject:true]; 
	//[parallelLocks makeObjectsPerformSelector:@selector(unlockWithCondition:) withObject:false]; 
	for (id aLock in parallelLocks) // warte Berechnung ab ("join")
	{
		[aLock lockWhenCondition:true];
		[aLock unlockWithCondition:false];
	}
	
	[self schliesseBerechnungAb];	
			
	[berechnungLock lock];
	[berechnungLock unlockWithCondition:false];
			
	if (benoetigtAktualisierung)
		[self aktualisiere:self];
	
	[derPool release];
}

-(void) starteBerechnung
{     
    benoetigtAktualisierung = false;
    abgebrochen = false;
    renderer->setzeUnerledigt();
 
    [self verarbeiteDrehungUmWinkel:-winkel umAchse:achse]; // zurückdrehen
	
	*achse = *achseNeu;
    winkel = winkelNeu;
	
    [self verarbeiteDrehungUmWinkel:winkel umAchse:achse];

	JuliaGeoObjekt::anzSchritte = 0; // debug
	
	naechsteKachelNr = 0;
    
    startzeit = [[NSDate alloc] init];
    
	// Threads abzweigen
    for (int i=0;i<anzahlThreads;i++)
    	[NSThread detachNewThreadSelector:@selector(berechneBildParallel:) toTarget:self withObject:[NSNumber numberWithInt:i]];
}

- (void)berechneBildParallel:(NSNumber*)inProzessNr
{
    NSAutoreleasePool* derPool = [[NSAutoreleasePool alloc] init]; 
 	
    [NSThread setThreadPriority:1.0];

	int dieKachelNr = 0;
	
	while (true)
	{
		@synchronized(self)
		{
			if (naechsteKachelNr<renderer->anzahlKacheln())
				dieKachelNr = naechsteKachelNr++;
			else
				break;
		}
		
		renderer->rendere(dieKachelNr);
    }
	
	[[parallelLocks objectAtIndex:[inProzessNr intValue]] lock];
	[[parallelLocks objectAtIndex:[inProzessNr intValue]] unlockWithCondition:true];
    
	[derPool release];
}

- (void)schliesseBerechnungAb
{
	renderer->rechneAlphaEin();
	renderer->komponiereBild();
	
	renderdauer = -[startzeit timeIntervalSinceNow];
	[startzeit release];
	startzeit = nil;
	
	[self display];
	//[self setNeedsDisplay:YES]; // langsamer
	
	if (!abgebrochen)
		[self setzeStatus:[self erzeugeInfoString]];

	[fortschrittsAnzeiger setDoubleValue:0];	
}

-(void) verarbeiteDrehungUmWinkel:(float)inWinkel umAchse:(R3Vektor*)inAchse
{
	if (norm(*inAchse)>0)
    {
        R3Vektor dieAchse = inAchse->normiert();
        
        if ([drehbarMenue indexOfSelectedItem]==0)
        {
            float derSinus = sin(inWinkel*.5);
            Quaternion dieDrehung(cos(inWinkel*.5),dieAchse.x*derSinus,dieAchse.y*derSinus,dieAchse.z*derSinus);
            objekt->drehe(dieDrehung);
        }
        else
            raum->dreheLichteinfall([drehbarMenue indexOfSelectedItem]-1,inWinkel,dieAchse);
    }
}

- (void)resetCursorRects
{
    [self addCursorRect:[self bounds] cursor:[NSCursor openHandCursor]];
}

-(void) mouseDown:(NSEvent*)inEvent
{
    [NSCursor hide];
    klickPos = [inEvent locationInWindow];
        
    [self verarbeiteDrehungUmWinkel:winkel umAchse:achse];
    
    [super mouseDown:inEvent];
}

-(void) mouseUp:(NSEvent*)inEvent
{
    [NSCursor unhide];
    [super mouseUp:inEvent];
}

-(void) mouseDragged:(NSEvent*)inEvent
{
	NSPoint	pn,p = [inEvent locationInWindow];

    pn.x = p.x/([self bounds].size.width/2);
    pn.y = p.y/([self bounds].size.height/2);
    
    R3Vektor k(pn.x-klickPos.x/([self bounds].size.width/2),pn.y-klickPos.y/([self bounds].size.height/2),0);	// „Kraft“
    R3Vektor r(pn.x-1,pn.y-1,0); // „Angriffspunkt“
    r.z = sqrt(1.f-min(r.x*r.x+r.z*r.z,.999f));
    *achseNeu = r.kreuzprodukt(k); // „Drehmoment“
    winkelNeu = -abs(k)*2;
        
	[self aktualisiere:self];
}

-(IBAction) exportiereBild:(id)sender
{
	return [self exportiereBildTransparent:NO];
}

-(IBAction) exportiereBildOhneHintergrund:(id)sender
{
	return [self exportiereBildTransparent:YES];
}

-(IBAction) exportiereBildTransparent:(BOOL)inTransparent
{
    NSSavePanel* derSichernDialog = [NSSavePanel savePanel];
	[derSichernDialog setRequiredFileType:@"png"]; 
    
	if ([derSichernDialog runModalForDirectory:nil file:@""] == NSOKButton)
	{
        [self setzeStatus:NSLocalizedString(@"SAVING IMAGE",@"")];
        [fortschrittsAnzeiger setIndeterminate:YES];
        [fortschrittsAnzeiger setUsesThreadedAnimation:YES];
        [fortschrittsAnzeiger startAnimation:self];
		
		NSString* derDateiname = [derSichernDialog filename];
		
		if ([stereoskopischSchalter intValue] && inTransparent)
		{
			for (int i=0;i<2;i++)
			{
				NSString* derDateiname2 = [NSString stringWithFormat:@"%@_%d.%@", [derDateiname stringByDeletingPathExtension], i, [derDateiname pathExtension]];
				NSData* dieBilddaten = [bitmap[i] representationUsingType:NSPNGFileType properties:nil];
				if (![dieBilddaten writeToFile:derDateiname2 atomically:YES])
					NSBeep();
			}
		}
		else
		{
			NSBitmapImageRep* dieBitmap = inTransparent?bitmap[0]:bitmap[2];
			NSData* dieBilddaten = [dieBitmap representationUsingType:NSPNGFileType properties:nil];
			if (![dieBilddaten writeToFile:derDateiname atomically:YES])
				NSBeep();
		}
		
        [fortschrittsAnzeiger stopAnimation:self];
        [fortschrittsAnzeiger setUsesThreadedAnimation:NO];
        [fortschrittsAnzeiger setIndeterminate:NO];
        [self setzeStatus:NSLocalizedString(@"IMAGE SAVED",@"")];
    }
}

-(IBAction) setzeZurueck:(id)sender
{
	[self stelleParameterEin:[self defaultParameter]];
	[self aktualisiere:self];
}

@end

