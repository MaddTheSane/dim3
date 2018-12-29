/****************************** File *********************************

Module: dim3 Base Utility
Author: Brian Barnes
 Usage: Special Obj-C file for OS X/iPad

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

#import <Foundation/Foundation.h>

#ifndef D3_OS_IPHONE
	#import <AppKit/AppKit.h>
#else
	#import <UIKit/UIKit.h>
#endif

bool cocoa_bitmap_text_font_exist(char *name)
{
	bool				ok;
	NSString			*font_name;
#ifndef D3_OS_IPHONE
	NSArray				*fonts;
#endif

	font_name=[[NSString alloc] initWithUTF8String:name];
	
#ifndef D3_OS_IPHONE
	fonts=[[NSFontManager sharedFontManager] availableFontFamilies];
	ok=([fonts containsObject:font_name]==YES);
#else
	ok=([UIFont fontWithName:font_name size:12.0f]!=nil);
#endif
	
	[font_name release];
	
	return(ok);
}



