/****************************** File *********************************

Module: dim3 Setup
Author: Brian Barnes
 Usage: Property Palette Singleplayer Option

***************************** License ********************************

This code can be freely used as long as these conditions are met:

1. This header, in its entirety, is kept with the code
2. This credit ÒCreated with dim3 TechnologyÓ is given on a single
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

#define kMPOptionPropertySettingsName			0
#define kMPOptionPropertySettingsDescript		1
#define kMPOptionPropertySettingsSSRevealId		2

extern iface_type				iface;
extern list_palette_type		project_palette;

/* =======================================================

      Property Palette Fill Singleplayer Option
      
======================================================= */

void project_palette_fill_singleplayer_option(int singleplayer_option_idx)
{
	iface_sp_option_type		*sp_option;

	sp_option=&iface.singleplayer.option_list.options[singleplayer_option_idx];

	list_palette_set_title(&project_palette,"Singleplayer",NULL,"Option",sp_option->name,NULL,NULL);

		// settings

	list_palette_add_header(&project_palette,0,"Settings");
	list_palette_add_string(&project_palette,kMPOptionPropertySettingsName,"Name",sp_option->name,name_str_len,FALSE);
	list_palette_add_string(&project_palette,kMPOptionPropertySettingsDescript,"Description",sp_option->descript,name_str_len,FALSE);
	list_palette_add_int(&project_palette,kMPOptionPropertySettingsSSRevealId,"Simple Save Reveal ID",&sp_option->ss_reveal_id,FALSE);
}

/* =======================================================

      Property Palette Click Singleplayer Option
      
======================================================= */

void project_palette_click_singleplayer_option(int singleplayer_option_idx,bool double_click)
{
}

