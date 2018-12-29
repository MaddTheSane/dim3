/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: App Definitions Header

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

(c) 2000-2012 Klink! Software www.klinksoftware.com
 
*********************************************************************/

//
// fatal error struct
//

typedef struct		{
						char							title[256],descript[256];
						bool							is_error;
					} fatal_error_type;

//
// editor overrides
//

typedef struct		{
						char							map[256];
						bool							on;
						d3pnt							pt;
						d3ang							ang;
					} app_editor_override_type;

//
// app struct
//
					
typedef struct		{
						int								state;
						bool							dedicated_host,loop_quit;
						app_editor_override_type		editor_override;
					} app_type;
