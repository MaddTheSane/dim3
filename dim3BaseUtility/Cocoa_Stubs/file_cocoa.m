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
#ifdef D3_OS_IPHONE
#import <UIKit/UIDevice.h>
#endif

void cocoa_file_get_application_support_path(char *path)
{
	NSArray				*paths;
	
		// iOS applications need to write
		// to the application/document directory
		// OS X writes to the application support
		
#ifdef D3_OS_IPHONE
	paths=NSSearchPathForDirectoriesInDomains(NSDocumentDirectory,NSUserDomainMask,TRUE);
#else
	paths=NSSearchPathForDirectoriesInDomains(NSApplicationSupportDirectory,NSUserDomainMask,TRUE);
#endif

	strncpy(path,[[paths objectAtIndex:0] UTF8String],1024);
	path[1023]=0x0;
}

bool cocoa_is_ipad(void)
{
#ifdef D3_OS_IPHONE
	return(UI_USER_INTERFACE_IDIOM()==UIUserInterfaceIdiomPad);
#else
	return(FALSE);
#endif
}

