#import <Foundation/Foundation.h>   //This is like stdio.h
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

-(void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
    NSRect win_rect = NSMakeRect(0.0,0.0,800.0,600.0);
    window = [[NSWindow alloc]initWithContentRect:win_rect
                                    styleMask:NSWindowStyleMaskTitled |
                                                  NSWindowStyleMaskClosable |
                                                  NSWindowStyleMaskMiniaturizable |
                                                  NSWindowStyleMaskResizable
                                          backing:NSBackingStoreBuffered
                                            defer:NO];
    [window setTitle:@"SSK : My First macOS window"];
    [window center];
    
    view = [[MyView alloc]initWithFrame:win_rect];
  
    [window setContentView:view];
    [window setDelegate:self];
    [window makeKeyAndOrderFront:self];
  
}

-(void)applicationWillTerminate:(NSNotification *)aNotification
{
    //code
}

-(void)windowWillClose:(NSNotification *)aNotification
{
    //code
    [NSApp terminate:self];
}

-(void)dealloc
{
    //code
    [view release];
    [window release];
    [super dealloc];

}
@end

@implementation MyView
{
    @private
    NSString * centralText;
}

-(id)initWithFrame:(NSRect) Frame
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

-(void)drawRect:(NSRect)dirtyRect
{
    NSColor *backgroundColor = [NSColor blackColor];
    [backgroundColor set];
    NSRectFill(dirtyRect);

    NSDictionary * dictonaryForTextAttribute = [NSDictionary dictionaryWithObjectsAndKeys:[NSFont fontWithName:@"Helvetica" size:32],NSFontAttributeName,[NSColor greenColor],NSForegroundColorAttributeName,nil];
    
    NSSize textSize = [centralText sizeWithAttributes:dictonaryForTextAttribute];
    
    NSPoint point;
    point.x = (dirtyRect.size.width/2) - (textSize.width/2);
    point.y = (dirtyRect.size.height/2) - (textSize.height/2) +1 2;
    [centralText drawAtPoint:point withAttributes:dictonaryForTextAttribute];
}

-(BOOL)acceptsFirstResponder
{
    //code
    [[self window]makeFirstResponder:self];
    return(YES);
}

-(void)keyDown:(NSEvent*)theEvent
{
    //code
    int key = [[theEvent characters]characterAtIndex:0];
    
    switch(key)
    {
        case 27:
            [self release];
            [NSApp terminate:self];
            break;
            
        case 'f':
        case 'F':
            [[self window]toggleFullScreen:self];
            break;
    }
}

-(void)mouseDown:(NSEvent*)theEvent
{
    //code
    centralText = @"Left Mouse Button is clicked";
    [self setNeedsDisplay:YES];
}

-(void)rightMouseDown:(NSEvent*)theEvent
{
    //code
    centralText = @"Right Mouse Button is clicked";
    [self setNeedsDisplay:YES];
    
}


-(void) otherButtonDown:(NSEvent *) theEvent
{
    // code
    centralText = @"Hello World Other button down";
    [self setNeedsDisply:YES];  // Invalidate Rect
}

-(void)dealloc
{
    [super dealloc];
}
@end
