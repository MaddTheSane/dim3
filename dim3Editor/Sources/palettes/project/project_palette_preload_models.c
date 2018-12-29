/****************************** File *********************************

Module: dim3 Setup
Author: Brian Barnes
 Usage: Property Palette Preload Models

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

#ifdef D3_PCH
	#include "dim3editor.h"
#endif

#include "glue.h"
#include "interface.h"

#define kPreloadModelsPropertyModelName			100

extern iface_type				iface;
extern list_palette_type		project_palette;

/* =======================================================

      Property Palette Fill Preload Models
      
======================================================= */

void project_palette_fill_preload_models(void)
{
	int				n;
	char			str[256];

	list_palette_set_title(&project_palette,"Preload Models",NULL,NULL,NULL,NULL,NULL);

		// models

	list_palette_add_header(&project_palette,0,"Models");

	for (n=0;n!=max_preload_model;n++) {
		sprintf(str,"Model %d",(n+1));
		list_palette_add_string(&project_palette,(kPreloadModelsPropertyModelName+n),str,iface.preload_model.names[n],name_str_len,FALSE);
	}
}

/* =======================================================

      Property Palette Click Preload Models
      
======================================================= */

void project_palette_click_preload_models(bool double_click)
{
}

