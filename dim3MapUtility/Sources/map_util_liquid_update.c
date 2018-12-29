/****************************** File *********************************

Module: dim3 Map Utility
Author: Brian Barnes
 Usage: Map Liquid Updates

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

(c) 2000-2010 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3maputility.h"
#endif

/* =======================================================

      Move Liquid
      
======================================================= */

void map_liquid_move(map_type *map,int liquid_idx,d3pnt *mov_pt)
{
	map_liquid_type			*liq;
	
	liq=&map->liquid.liquids[liquid_idx];
	
	liq->lft+=mov_pt->x;
	liq->rgt+=mov_pt->x;
	
	liq->top+=mov_pt->z;
	liq->bot+=mov_pt->z;
	
	liq->y+=mov_pt->y;
}

void map_liquid_move_copy(map_type *map,int liquid_idx,d3pnt *mov_pt)
{
	map_liquid_type			*liq;

	map_liquid_create_copy_data(map,liquid_idx);
	
	liq=&map->liquid.liquids[liquid_idx];
	
	liq->lft=liq->copy.lft+mov_pt->x;
	liq->rgt=liq->copy.rgt+mov_pt->x;
	
	liq->top=liq->copy.top+mov_pt->z;
	liq->bot=liq->copy.bot+mov_pt->z;
	
	liq->y=liq->copy.y+mov_pt->y;
}

/* =======================================================

      Reset Liquid UV
      
======================================================= */

void map_liquid_reset_uv(map_type *map,int liquid_idx)
{
	float					ltxtx,rtxtx,ltxtz,rtxtz;
	d3uv					uv_offset,uv_size;
	map_liquid_type			*liq;

	liq=&map->liquid.liquids[liquid_idx];
	
		// get scale

	map_get_texture_uv_get_scale(map,liq->txt_idx,&uv_offset,&uv_size);
	
	ltxtx=((float)liq->lft)*uv_size.x;
	rtxtx=((float)liq->rgt)*uv_size.x;

	ltxtz=((float)liq->top)*uv_size.y;
	rtxtz=((float)liq->bot)*uv_size.y;

	liq->main_uv.offset.x=map_get_texture_round_coord(map_get_texture_reduce_coord(ltxtx))+uv_offset.x;
	liq->main_uv.size.x=map_get_texture_round_coord(rtxtx-ltxtx);
	
	liq->main_uv.offset.y=map_get_texture_round_coord(map_get_texture_reduce_coord(ltxtz))+uv_offset.y;
	liq->main_uv.size.y=map_get_texture_round_coord(rtxtz-ltxtz);
}

