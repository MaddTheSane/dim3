/****************************** File *********************************

Module: dim3 Setup
Author: Brian Barnes
 Usage: Property Palette Logo

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

#define kIntroPropertyTitleName						0
#define kIntroPropertyTitleSound					1
#define kIntroPropertyTitleLifeMsec					2

extern iface_type				iface;
extern list_palette_type		project_palette;

/* =======================================================

      Property Palette Fill Logo
      
======================================================= */

void project_palette_fill_logo(void)
{
	list_palette_set_title(&project_palette,"Logo",NULL,NULL,NULL,NULL,NULL);
	
		// logo

	list_palette_add_header(&project_palette,0,"Logo");
	list_palette_add_picker_file(&project_palette,kIntroPropertyTitleName,list_button_none,0,"Bitmap","Titles","png","",iface.logo.name,FALSE);
	list_palette_add_picker_list_string(&project_palette,kIntroPropertyTitleSound,"Sound",(char*)iface.sound_list.sounds,iface.sound_list.nsound,sizeof(iface_sound_type),(int)offsetof(iface_sound_type,name),TRUE,TRUE,iface.logo.sound,FALSE);
	list_palette_add_int(&project_palette,kIntroPropertyTitleLifeMsec,"Life Millsec",&iface.logo.life_msec,FALSE);
}

/* =======================================================

      Property Palette Click Logo
      
======================================================= */

void project_palette_click_logo(bool double_click)
{
}

