#import "AppDelegate.h"
#import "ViewController.h"
#import "GLESView.h"

@implementation AppDelegate
{
    @private
    UIWindow *mainWindow;
    ViewController *mainViewController;
    GLESView *glesView;
}

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    // code
    CGRect screenBounds = [[UIScreen mainScreen]bounds];
    
    mainWindow = [[UIWindow alloc]initWithFrame:screenBounds];
    
    mainViewController = [[ViewController alloc]init];
    
    [mainWindow setRootViewController:mainViewController];
    
    glesView = [[GLESView alloc]initWithFrame:screenBounds];
    
    [mainViewController setView:glesView];
    
    [glesView release];
    
    [mainWindow addSubview:[mainViewController view]];
    
    [mainWindow makeKeyAndVisible];
    
    return YES;
}

-(void)applicationWillResignActive:(UIApplication *)application {
    // Sent when the application is about to move from active to inactive state.
    
}

-(void)applicationDidEnterBackground:(UIApplication *)application {
    
}

-(void)applicationWillEnterForeground:(UIApplication *)application {
    
}

-(void)applicationDidBecomeActive:(UIApplication *)application {
    
}

-(void)applicationWillTerminate:(UIApplication *)application {
    
}


- (void)dealloc {
    // code
    [glesView release];
    
    [mainViewController release];
    
    [mainWindow release];
    
    [super dealloc];
}

@end
