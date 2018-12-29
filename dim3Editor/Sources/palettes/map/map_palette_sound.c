/****************************** File *********************************

Module: dim3 Editor
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

#define kSoundPropertyOn					0
#define kSoundPropertyName					1
#define kSoundPropertyPitch					2

extern map_type					map;
extern iface_type				iface;

extern list_palette_type		map_palette;

int								pal_sound_index;

/* =======================================================

      Property Palette Fill Sound
      
======================================================= */

void map_palette_fill_sound(int sound_idx)
{
	map_sound_type		*sound;

	sound=&map.sounds[sound_idx];

	list_palette_set_title(&map_palette,"Sound",sound->name,NULL,NULL,NULL,NULL);

	list_palette_add_header(&map_palette,0,"Sound Settings");
	list_palette_add_checkbox(&map_palette,kSoundPropertyOn,"On",&sound->on,FALSE);

	list_palette_add_header(&map_palette,0,"Sound Audio");
	list_palette_add_picker_list_string(&map_palette,kSoundPropertyName,"Sound",(char*)iface.sound_list.sounds,iface.sound_list.nsound,sizeof(iface_sound_type),(int)offsetof(iface_sound_type,name),TRUE,TRUE,sound->name,FALSE);
	list_palette_add_float(&map_palette,kSoundPropertyPitch,"Pitch",&sound->pitch,FALSE);
	
	pal_sound_index=sound_idx;
	
	list_palette_add_header(&map_palette,0,"Sound Info");
	list_palette_add_int(&map_palette,-1,"Index",&pal_sound_index,TRUE);
	list_palette_add_point(&map_palette,-1,"Position",&sound->pnt,TRUE);
}

/* =======================================================

      Property Palette Click Sound
      
======================================================= */

void map_palette_click_sound(int sound_idx,bool double_click)
{
}

