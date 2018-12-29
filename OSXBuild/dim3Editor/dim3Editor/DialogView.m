/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Obj-C Dialog Delegate

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

#import "DialogView.h"

#include "glue.h"
#include "interface.h"

extern os_dialog_callback_ptr			diag_callback;

@implementation DialogView

/* =======================================================

      Clicking Messages
	        
======================================================= */

-(IBAction)diagButtonDown:(id)sender
{
	(*diag_callback)(os_dialog_msg_type_command,(int)[(NSControl*)sender tag]);
}

-(IBAction)diagComboPick:(id)sender
{
	(*diag_callback)(os_dialog_msg_type_command,(int)[(NSControl*)sender tag]);
}

-(IBAction)fileClick:(id)sender
{
	(*diag_callback)(os_dialog_msg_type_tree_change,(int)[(NSControl*)sender tag]);
}

-(IBAction)fileDoubleClick:(id)sender
{
	(*diag_callback)(os_dialog_msg_type_tree_double_click,(int)[(NSControl*)sender tag]);
}

@end
