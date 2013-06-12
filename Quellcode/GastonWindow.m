#import "GastonWindow.h"

@implementation GastonWindow

- (id)initWithContentRect:(NSRect)contentRect styleMask:(NSUInteger)aStyle backing:(NSBackingStoreType)bufferingType defer:(BOOL)flag
{
    GastonWindow* result = [[GastonWindow alloc] initWithContentRect:contentRect styleMask:NSBorderlessWindowMask backing:bufferingType defer:flag];
    [result setBackgroundColor:[NSColor clearColor]];
    [result setHasShadow:NO];
    [result setOpaque:NO];
        
    return result;
}

@end
