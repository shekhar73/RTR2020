#import <Foundation/Foundation.h>
#import <Cocoa/Cocoa.h> // anyonimous to windows.h

// NS - NeXT Step
// Forward declaration
@interface AppDelegate:NSObject <NSApplicationDelegate, NSWindowDelegate>
		
@end

int main(int argc, char *argv[])
{
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
    NSApp = [NSApplication sharedApplication];
    [NSApp setDelegate:[[AppDelegate alloc] init]];
    [NSApp run];    // This is called as Run Loop or Message Loop or Event loop
    [pool release];
    return 0;
}

//ApplicationDidFinishLaunching - WMCreate in windows both are anyonimous
// when application gets terminated NSApps chi ajun ek method terminate hote i.e. the application will termiate i.e. WM_Destroy()

// Forward declaration
@interface MyView : NSView

@end

@implementation AppDelegate
{
    @private
    NSWindow *window;
    MyView *view;
}
@end

@implementation MyView
{
    @private
        NSString *centralText;
}
@end

- (id) initWithFrame:(NSRect) Frame
{
    // code
    self = [super initWithFrame:Frame];
    if(self)
    {
        centralText = @"Hello World!";
    }
    return self;
}

// dirtyRect - InvalidateRect

- (void) drawRect:(NSRect) dirtyRect
{
    NSColor *backgroundColor = [NSColor blackColor];
    [backgroundColor set];
    NSRectFill(dirtyRect);

    NSDictionary *dictionaryForTextAttributes = [NSDictionary dictionaryWithObjectsAndKeys:[NSFont fontWithName:@helvetica size:32], NSFontAttributeName, 
                                                                                            [NSColor greenColor], NSForgroundColorAttributeName, nill];

    NSSize textSize = [centralText sizeWithAttributes : dictionaryForTextAttributes];

    [NSPoint point];

    point.x = (dirtyRect.size.width / 2) - (textSize.width / 2);
    point.y = (dirtyRect.size.height / 2) - (textSize.height / 2) + 12;

    [centralText drawatpoint:withAttributes:dictionaryForTextAttributes];
}

-(BOOL) acceptFirstResponder
{
    // code
    [[self window] makeFirstResponder];
    return YES;
}

-(void) keyDown:(NSEvent *) theEvent
{
    // code
    int key = [[theEvent characters] characterAtIndex:0];

    switch(key)
    {
        case 27:[self release];
        [NSApp terminate : self];
        break;

        case 'F':
        case 'f':
        [[self window] toggleFullScreen:self];
    }
    
}

-(void) mouseDown:(NSEvent *) theEvent
{
    // code
    centralText = @"Left Mouse Button Is Clicked";
    [self setNeedsDisply:YES];  // Invalidate Rect
}

-(void) rightMouseDown:(NSEvent *) theEvent
{
    // code
    centralText = @"Right Mouse Button Is Clicked";
    [self setNeedsDisply:YES];  // Invalidate Rect
}

-(void) otherButtonDown:(NSEvent *) theEvent
{
    // code
    centralText = @"Hello World Other button down";
    [self setNeedsDisply:YES];  // Invalidate Rect
}

- (void) ApplicationDidFinishLaunching:(NSNotification *) aNotification
{
    // code
    // NSRect is internally CGREct
    NSRect win_rect = NSMakeRect(0.0, 0.0, 800.0, 600.0);   // As this is a C library. First 2 variables are NSPoints i.e. X & Y 3rd and 4th varaibles are width and height i.e. NSSize structure(width and height).
    // NSPoint is internally CGPoint and NSSIze is internally CGSize
    window = [
                [NSWindow alloc] initWithContentRect:win_rect] styleMask:NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | NSWindowStyleMaskMiniaturizable | NSWindowStyleMaskResizable
                Packing:NSPackingStoredBuffer
                defer:NO
            ];

    // styleMask = WS_OVERLAPPEDWINDOWNS
    // WindowStyleMaskTitled - WM_CAPTION
    // NSWindowStyleMaskMiniaturizable - WS_MINIMIZE Box
    // NSWindowStyleMaskClosable - WM_CLOSE
    // NSWindowStyleMaskResizable - WM_RESIZE

    [window setTitle:@"SSK : MacOS Window"];

    view = [[MyView alloc] initWithFrame:win_rect];
    [window setContentView:view];
    [window setDelegate:self];
    [window makeKeyAndOrderFront:self];
}


- (void) ApplicationWillTerminate:(NSNotification *) aNotification
{
    // code
}

- (void) WindowWillClose:(NSNotification *) aNotification
{
    // code
    [NSApp terminate : self];
    
}

- (void) dealloc
{
    // code
    [view release];
    [window release];
    [super dealloc];
}
