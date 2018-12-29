/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Property Palette Node

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

#define kNodePropertyName					0
#define kNodePropertyEventID				1
#define kNodePropertyAlpha					2

#define kNodePropertyFollowCamera			3

extern map_type					map;

extern list_palette_type		map_palette;

int								pal_node_index;

/* =======================================================

      Property Palette Fill Node
      
======================================================= */

void map_palette_fill_node(int node_idx)
{
	node_type			*node;

	node=&map.nodes[node_idx];

	list_palette_set_title(&map_palette,"Node",node->name,NULL,NULL,NULL,NULL);

	list_palette_add_header(&map_palette,0,"Node Identity");
	list_palette_add_string(&map_palette,kNodePropertyName,"Name",node->name,name_str_len,FALSE);
	list_palette_add_int(&map_palette,kNodePropertyEventID,"Event ID",&node->event_id,FALSE);
	list_palette_add_float(&map_palette,kNodePropertyAlpha,"Alpha",&node->alpha,FALSE);

	list_palette_add_header(&map_palette,0,"Node Settings");
	list_palette_add_checkbox(&map_palette,kNodePropertyFollowCamera,"Follow Camera",&node->follow_camera,FALSE);

	pal_node_index=node_idx;

	list_palette_add_header(&map_palette,0,"Node Info");
	list_palette_add_int(&map_palette,-1,"Index",&pal_node_index,TRUE);
	list_palette_add_point(&map_palette,-1,"Position",&node->pnt,TRUE);
	list_palette_add_angle(&map_palette,-1,"Angle",&node->ang,TRUE);
}

/* =======================================================

      Property Palette Click Node
      
======================================================= */

void map_palette_click_node(int node_idx,bool double_click)
{
}

