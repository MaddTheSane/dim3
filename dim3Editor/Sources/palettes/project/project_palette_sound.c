/****************************** File *********************************

Module: dim3 Setup
Author: Brian Barnes
 Usage: Property Palette Sound

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

#define kSoundSettingsName						0
#define kSoundSettingsFileName					1
#define kSoundSettingsMinDist					2
#define kSoundSettingsMaxDist					3

extern iface_type				iface;
extern list_palette_type		project_palette;

/* =======================================================

      Property Palette Fill Sound
      
======================================================= */

void project_palette_fill_sound(int sound_idx)
{
	iface_sound_type			*sound;

	sound=&iface.sound_list.sounds[sound_idx];

	list_palette_set_title(&project_palette,"Sounds",NULL,"Sound",sound->name,NULL,NULL);

		// settings

	list_palette_add_header(&project_palette,0,"Settings");
	list_palette_add_string(&project_palette,kSoundSettingsName,"Name",sound->name,name_str_len,FALSE);
	list_palette_add_picker_file(&project_palette,kSoundSettingsFileName,list_button_none,0,"Wave","Sounds","wav","",sound->file_name,FALSE);
	list_palette_add_int(&project_palette,kSoundSettingsMinDist,"Min Distance",&sound->min_dist,FALSE);
	list_palette_add_int(&project_palette,kSoundSettingsMaxDist,"Max Distance",&sound->max_dist,FALSE);
}

/* =======================================================

      Property Palette Click Sound
      
======================================================= */

void project_palette_click_sound(int sound_idx,bool double_click)
{
}

