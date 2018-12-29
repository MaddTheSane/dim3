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
#import "View.h"

#include "glue.h"
#include "interface.h"

d3col							*os_color_pick_col_ptr;

@implementation View

/* =======================================================

      Initialize and Shutdown View
	        
======================================================= */

-(id)initWithFrame:(NSRect)frame
{
	NSOpenGLPixelFormatAttribute pf_attrib[]= 
				{
					NSOpenGLPFAWindow,
					NSOpenGLPFAAccelerated,
					NSOpenGLPFANoRecovery,
					NSOpenGLPFADoubleBuffer,
					NSOpenGLPFAColorSize,24,
					NSOpenGLPFAAlphaSize,8,
					NSOpenGLPFADepthSize,24,
					0
				};
	
		// init the view
		
	self=[super initWithFrame:frame];
    if (self==nil) return(nil);
	
		// openGL context
		
	NSOpenGLPixelFormat *pf=[[NSOpenGLPixelFormat alloc] initWithAttributes:pf_attrib]; 
		
	ctx=[[NSOpenGLContext alloc] initWithFormat:pf shareContext:nil];
	[ctx makeCurrentContext];
	
	[pf release];
	
		// setup window
		
	main_wind_gl_setup();
	main_wind_initialize();
    
    return self;
}

-(BOOL)windowShouldClose:(id)sender
{
	if (main_app_quit()) os_application_quit();
	return(NO);
}

-(void)windowWillClose:(NSNotification *)notification
{
	main_wind_shutdown();
}

/* =======================================================

      Drawing
      
======================================================= */

-(BOOL)isOpaque
{
	return(YES);
}

-(void)lockFocus
{
  [super lockFocus];
  
  if ([ctx view]!=self) [ctx setView:self];
  
  [ctx makeCurrentContext];
}

-(void)drawRect:(NSRect)dirtyRect
{
	[ctx update];
	main_wind_draw();
}

-(void)openGLFlush
{
	[ctx flushBuffer];
}

/* =======================================================

      Color Picker
      
======================================================= */

-(void)changeColor:(id)sender
{
	NSColorPanel		*colPanel;
	
	colPanel=(NSColorPanel*)sender;
	
	os_color_pick_col_ptr->r=[[colPanel color] redComponent];
	os_color_pick_col_ptr->g=[[colPanel color] greenComponent];
	os_color_pick_col_ptr->b=[[colPanel color] blueComponent];
	
	[colPanel close];
	
	main_wind_draw();
}

/* =======================================================

      Events
      
======================================================= */

-(BOOL)acceptsFirstResponder
{
    return(YES);
}

-(BOOL)acceptsMouseMovedEvents
{
	return(YES);
}

-(void)mouseDown:(NSEvent*)event
{
	bool			double_click;
	d3pnt			pnt;
	NSPoint			wnd_pnt;
	
	wnd_pnt=[self convertPoint:[event locationInWindow] fromView:nil];
	
	pnt.x=(int)wnd_pnt.x;
	pnt.y=(int)([self frame].size.height-wnd_pnt.y);
	double_click=([event clickCount]==2);
	
	main_wind_click(&pnt,double_click);
}

-(void)mouseMoved:(NSEvent*)event
{
	d3pnt			pnt;
	NSPoint			wnd_pnt;
	
	wnd_pnt=[self convertPoint:[event locationInWindow] fromView:nil];
	
	pnt.x=(int)wnd_pnt.x;
	pnt.y=(int)([self frame].size.height-wnd_pnt.y);

	main_wind_mouse_move(&pnt);
}

-(void)scrollWheel:(NSEvent*)event
{
	d3pnt			pnt;
	NSPoint			wnd_pnt;
	
	wnd_pnt=[self convertPoint:[event locationInWindow] fromView:nil];
	
	pnt.x=(int)wnd_pnt.x;
	pnt.y=(int)([self frame].size.height-wnd_pnt.y);

	main_wind_scroll_wheel(&pnt,(int)[event deltaY]);
}

-(void)keyDown:(NSEvent*)event
{
	int				key;
	NSString		*chars;
	
		// get the key
		
	chars=[event charactersIgnoringModifiers];
	if ([chars length]!=1) return;
	
		// translate some keys
		
	key=[chars characterAtIndex:0];
	
	switch (key) {
		case NSUpArrowFunctionKey:
			key=D3_KEY_UP;
			break;
		case NSDownArrowFunctionKey:
			key=D3_KEY_DOWN;
			break;
		case NSLeftArrowFunctionKey:
			key=D3_KEY_LEFT;
			break;
		case NSRightArrowFunctionKey:
			key=D3_KEY_RIGHT;
			break;
	}
	
		// send main key through
		
	main_wind_key((char)key);
	
		// always check cursor changes
		
	main_wind_cursor();
}

-(void)keyUp:(NSEvent*)event
{
	main_wind_cursor();
}

-(void)flagsChanged:(NSEvent *)event
{
	main_wind_cursor();
}

-(void)cursorUpdate:(NSEvent*)event
{
	if (main_wind_cursor()) return;
	
	[super cursorUpdate:event];
}

@end
