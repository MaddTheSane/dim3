/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Obj-C App Delegate

***************************** License ********************************

This code can be freely used as long as these conditions are met:

1. This header, in its entirety, is kept with the code
2. This credit “Created with dim3 Technology” is given on a single
application screen and in a single piece of the documentation
3. It is not resold, in it's current form or modified, as an
engine-only product

This code is presented as is. The author of dim3 takes no
responsibilities for any version of this code.

Any non-engine product (games, etc) created with this code is free
from any and all payment and/or royalties to the author of dim3,
and can be sold or given away.

(c) 2000-2011 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#import "AppDelegate.h"
#include "interface.h"

@implementation AppDelegate

/* =======================================================

      Init and Shutdown Delegate
	        
======================================================= */

-(void)applicationDidFinishLaunching:(NSNotification*)aNotification
{
	int				wid,high;
	NSUInteger		styleMask;
	NSRect			deskTopRect,frame,windRect;
	
		// build the main menu
		
	[NSBundle loadNibNamed:@"MainMenu" owner:NSApp];
	[[NSApp mainMenu] setAutoenablesItems:NO];
	
	main_wind_menu_create();
	
		// get desktop window

	deskTopRect=[[NSScreen mainScreen] visibleFrame];
	wid=deskTopRect.size.width;
	high=deskTopRect.size.height;
	
		// create the main window
	
	frame=NSMakeRect(0,high,wid,high);
	styleMask=NSTitledWindowMask|NSClosableWindowMask|NSMiniaturizableWindowMask|NSResizableWindowMask;
	windRect=[NSWindow contentRectForFrameRect:frame styleMask:styleMask];
	
	window=[[NSWindow alloc] initWithContentRect:windRect styleMask:styleMask backing:NSBackingStoreBuffered defer:false];
	[window setTitle:@"dim3 Editor"];
	
		// create the view
	
	view=[[View alloc] initWithFrame:windRect];
	[window setContentView:view];
	[window setDelegate:view];
	[window setAcceptsMouseMovedEvents:YES];
	
		// display the window

	[window makeKeyAndOrderFront:NSApp];
}

-(NSApplicationTerminateReply)applicationShouldTerminate:(NSApplication*)sender
{
	if (!main_app_quit()) return(NSTerminateCancel);
	
	return(NSTerminateNow);
}

-(void)applicationWillTerminate:(NSNotification*)aNotification
{
	[view removeFromSuperviewWithoutNeedingDisplay];
	[view release];

	[window close];
	
	main_app_shutdown();
}

/* =======================================================

      Menu Commands
	        
======================================================= */

-(void)menuCommand:(id)sender
{
	NSMenuItem		*item;
	
	item=(NSMenuItem*)sender;
	main_wind_menu_event_run([item tag]);
}

/* =======================================================

      Timers
	        
======================================================= */

-(void)timerFire:(NSTimer*)timer
{
	main_wind_timer();
}

@end
