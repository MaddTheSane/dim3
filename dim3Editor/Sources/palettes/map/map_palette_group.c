/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Property Palette Group

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

#define kGroupPropertyName					0

#define kGroupPropertyClear					10

extern map_type					map;
extern app_state_type			state;

extern list_palette_type		map_palette;

int								pal_mesh_count,pal_liquid_count;

/* =======================================================

      Property Palette Fill Group
      
======================================================= */

void map_palette_fill_group(int group_idx)
{
	int					n;
	map_mesh_type		*mesh;
	map_liquid_type		*liq;
	group_type			*group;

	group=&map.group.groups[group_idx];

	list_palette_set_title(&map_palette,"Group",group->name,NULL,NULL,NULL,NULL);

		// get some counts

	pal_mesh_count=0;
	pal_liquid_count=0;
	
	mesh=map.mesh.meshes;
	
	for (n=0;n!=map.mesh.nmesh;n++) {
		if (mesh->group_idx==group_idx) pal_mesh_count++;
		mesh++;
	}
	
	liq=map.liquid.liquids;
	
	for (n=0;n!=map.liquid.nliquid;n++) {
		if (liq->group_idx==group_idx) pal_liquid_count++;
		liq++;
	}

		// fill items

	list_palette_add_header(&map_palette,0,"Group Options");
	list_palette_add_string(&map_palette,kGroupPropertyName,"Name",group->name,name_str_len,FALSE);
	
	list_palette_add_header(&map_palette,0,"Group Info");
	list_palette_add_int(&map_palette,-1,"Mesh Count",&pal_mesh_count,TRUE);
	list_palette_add_int(&map_palette,-1,"Liquid Count",&pal_liquid_count,TRUE);
	
	list_palette_add_header(&map_palette,0,"Group Clear");
	list_palette_add_string_selectable_button(&map_palette,kGroupPropertyClear,list_button_set,kGroupPropertyClear,"Clear Group Mesh and Liquids",FALSE,FALSE);
}

/* =======================================================

      Property Palette Click Group
      
======================================================= */

void map_palette_click_group(bool double_click)
{
		// group clear

	if (map_palette.item_pane.click.id==kGroupPropertyClear) {
		group_clear(state.map.cur_group_idx,FALSE);
		return;
	}
}

