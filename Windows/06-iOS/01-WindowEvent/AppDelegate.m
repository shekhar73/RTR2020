#import "AppDelegate.h"
#import "ViewController.h"
#import "MyView.h"

@implementation AppDelegate
{
    @private
    UIWindow *mainWindow;
    ViewController *mainViewController;
    MyView *myView;
}

- (bool)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    // code
    CGRect screenBounds = [[UIScreen mainScreen]bounds];
    mainWindow = [[UIWindow alloc]initWithFrame:screenBounds];
    mainViewController = [[ViewController alloc]init];
    [mainWindow setRootViewController:mainViewController];
    myView = [[MyView alloc]initWithFrame:screenBounds];
    [mainViewController setView:myView];
    [myView release];    
    [mainWindow makeKeyAndVisible];
    return YES;
}

-(void)applicationWillResignActive:(UIApplication *)application {
    
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
    [mainViewController release];
    [myView release];
    [mainWindow release];
    [super dealloc];
}

@end
