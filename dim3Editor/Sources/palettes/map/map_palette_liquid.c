/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Property Palette Liquid

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

#define kLiquidPropertyLockUV					0
#define kLiquidPropertyNeverObscure				1
#define kLiquidPropertyNeverCull				2
#define kLiquidPropertyNoReflectionMap			3
#define kLiquidPropertyNoDraw					4

#define kLiquidPropertyColor					5
#define kLiquidPropertyTintAlpha				6
#define kLiquidPropertySpeedAlter				7
#define kLiquidPropertySoundName				8

#define kLiquidPropertyTideSize					9
#define kLiquidPropertyTideRate					10
#define kLiquidPropertyTideUVShift				11

#define kLiquidPropertyWaveOn					20
#define kLiquidPropertyWaveDirNorthSouth		21
#define kLiquidPropertyWaveLength				22
#define kLiquidPropertyWaveHigh					23
#define kLiquidPropertyWavePeriodMSec			24
#define kLiquidPropertyWaveReset				25

#define kLiquidPropertyHarm						30
#define kLiquidPropertyDrownTick				31
#define kLiquidPropertyDrownHarm				32

#define kLiquidPropertyReflectTextureSize		40
#define kLiquidPropertyReflectXRefract			41
#define kLiquidPropertyReflectZRefract			42
#define kLiquidPropertyReflectNoHitColor		43
#define kLiquidPropertyReflectAlpha				44

#define kLiquidPropertyOverlayOn				50
#define kLiquidPropertyOverlayTexture			51
#define kLiquidPropertyOverlayStampSize			52
#define kLiquidPropertyOverlayShift				53

#define kLiquidPropertyGroup					60

#define kLiquidPropertyOff						70
#define kLiquidPropertySize						71
#define kLiquidPropertyShift					72
#define kLiquidPropertyCamera					73

extern map_type					map;
extern iface_type				iface;

extern list_palette_type		map_palette;
extern char						map_map_light_map_size_list[][name_str_len];

int								pal_liquid_index;
d3pnt							pal_liquid_pnt,pal_liquid_size;

/* =======================================================

      Property Palette Fill Liquid
      
======================================================= */

void map_palette_fill_liquid(int liq_idx)
{
	char					str[32];
	map_liquid_type			*liq;
	editor_view_type		*view;

		// liquid settings
		
	liq=&map.liquid.liquids[liq_idx];

	sprintf(str,"%d",liq_idx);
	list_palette_set_title(&map_palette,"Liquid",str,NULL,NULL,NULL,NULL);

	list_palette_add_header(&map_palette,0,"Liquid Settings");
	list_palette_add_checkbox(&map_palette,kLiquidPropertyLockUV,"Lock UV",&liq->flag.lock_uv,FALSE);
	list_palette_add_checkbox(&map_palette,kLiquidPropertyNeverObscure,"Never Obscure",&liq->flag.never_obscure,FALSE);
	list_palette_add_checkbox(&map_palette,kLiquidPropertyNeverCull,"Never Cull",&liq->flag.never_cull,FALSE);
	list_palette_add_checkbox(&map_palette,kLiquidPropertyNoDraw,"No Draw (Volume Only)",&liq->flag.no_draw,FALSE);

	list_palette_add_header(&map_palette,0,"Liquid Under");
	list_palette_add_pick_color(&map_palette,kLiquidPropertyColor,"Color",&liq->col,FALSE);
	list_palette_add_float(&map_palette,kLiquidPropertyTintAlpha,"Tint Alpha",&liq->tint_alpha,FALSE);
	list_palette_add_float(&map_palette,kLiquidPropertySpeedAlter,"Speed Alter",&liq->speed_alter,FALSE);
	list_palette_add_picker_list_string(&map_palette,kLiquidPropertySoundName,"Sound",(char*)iface.sound_list.sounds,iface.sound_list.nsound,sizeof(iface_sound_type),(int)offsetof(iface_sound_type,name),TRUE,TRUE,liq->ambient.sound_name,FALSE);

	list_palette_add_header(&map_palette,0,"Liquid Tides");
	list_palette_add_int(&map_palette,kLiquidPropertyTideSize,"Size",&liq->tide.high,FALSE);
	list_palette_add_int(&map_palette,kLiquidPropertyTideRate,"Rate",&liq->tide.rate,FALSE);
	list_palette_add_float(&map_palette,kLiquidPropertyTideUVShift,"UV Shift",&liq->tide.uv_shift,FALSE);

	list_palette_add_header(&map_palette,0,"Liquid Waves");
	list_palette_add_checkbox(&map_palette,kLiquidPropertyWaveOn,"On",&liq->wave.on,FALSE);
	list_palette_add_checkbox(&map_palette,kLiquidPropertyWaveDirNorthSouth,"North-South Dir",&liq->wave.dir_north_south,FALSE);
	list_palette_add_int(&map_palette,kLiquidPropertyWaveLength,"Length",&liq->wave.length,FALSE);
	list_palette_add_int(&map_palette,kLiquidPropertyWaveHigh,"High",&liq->wave.high,FALSE);
	list_palette_add_int(&map_palette,kLiquidPropertyWavePeriodMSec,"Period msec",&liq->wave.period_msec,FALSE);
	list_palette_add_string_selectable_button(&map_palette,kLiquidPropertyWaveReset,list_button_set,kLiquidPropertyWaveReset,"Resize Liquid To Fit Wave Length",FALSE,FALSE);
	
	list_palette_add_header(&map_palette,0,"Liquid Harm");
	list_palette_add_int(&map_palette,kLiquidPropertyHarm,"In Damage",&liq->harm.in_harm,FALSE);
	list_palette_add_int(&map_palette,kLiquidPropertyDrownTick,"Drowning Tick",&liq->harm.drown_tick,FALSE);
	list_palette_add_int(&map_palette,kLiquidPropertyDrownHarm,"Drowning Damage",&liq->harm.drown_harm,FALSE);

	list_palette_add_header(&map_palette,0,"Liquid Reflection");
	list_palette_add_picker_list_int(&map_palette,kLiquidPropertyReflectTextureSize,"Texture Size",(char*)map_map_light_map_size_list,-1,name_str_len,0,FALSE,FALSE,&liq->reflect.texture_size,FALSE);
	list_palette_add_int(&map_palette,kLiquidPropertyReflectXRefract,"X Refraction Factor",&liq->reflect.x_refract_factor,FALSE);
	list_palette_add_int(&map_palette,kLiquidPropertyReflectZRefract,"Z Refraction Factor",&liq->reflect.z_refract_factor,FALSE);
	list_palette_add_pick_color(&map_palette,kLiquidPropertyReflectNoHitColor,"No Hit Color",&liq->reflect.no_hit_col,FALSE);
	list_palette_add_float(&map_palette,kLiquidPropertyReflectAlpha,"Alpha",&liq->reflect.alpha,FALSE);

	list_palette_add_header(&map_palette,0,"Liquid Overlay");
	list_palette_add_checkbox(&map_palette,kLiquidPropertyOverlayOn,"On",&liq->overlay.on,FALSE);
	list_palette_add_picker_list_int(&map_palette,kLiquidPropertyOverlayTexture,"Texture",(char*)map.textures,max_map_texture,sizeof(texture_type),(int)offsetof(texture_type,frames[0].name),TRUE,FALSE,&liq->overlay.txt_idx,FALSE);
	list_palette_add_int(&map_palette,kLiquidPropertyOverlayStampSize,"Stamp Size",&liq->overlay.stamp_size,FALSE);
	list_palette_add_uv(&map_palette,kLiquidPropertyOverlayShift,"Shift",&liq->overlay.shift,FALSE);

	list_palette_add_header(&map_palette,0,"Liquid Group");
	list_palette_add_picker_list_int(&map_palette,kLiquidPropertyGroup,"Group",(char*)map.group.groups,map.group.ngroup,sizeof(group_type),(int)offsetof(group_type,name),TRUE,TRUE,&liq->group_idx,FALSE);

		// polygon like settings
		
	view=view_get_current_view();
	
	list_palette_add_header(&map_palette,0,"Liquid UVs");

	if (view->uv_layer==uv_layer_normal) {
		list_palette_add_uv(&map_palette,kLiquidPropertyOff,"Offset",&liq->main_uv.offset,FALSE);
		list_palette_add_uv(&map_palette,kLiquidPropertySize,"Size",&liq->main_uv.size,FALSE);
	}
	else {
		list_palette_add_uv(&map_palette,kLiquidPropertyOff,"Offset",&liq->lmap_uv.offset,FALSE);
		list_palette_add_uv(&map_palette,kLiquidPropertySize,"Size",&liq->lmap_uv.size,FALSE);
	}
	
	list_palette_add_uv(&map_palette,kLiquidPropertyShift,"Shift",&liq->shift,FALSE);

	list_palette_add_header(&map_palette,0,"Liquid Camera");
	list_palette_add_picker_list_string(&map_palette,kLiquidPropertyCamera,"Attach Node",(char*)map.nodes,map.nnode,sizeof(node_type),(int)offsetof(node_type,name),TRUE,TRUE,liq->camera,FALSE);
	
		// info
	
	pal_liquid_index=liq_idx;
		
	pal_liquid_pnt.x=liq->lft;
	pal_liquid_pnt.y=liq->y;
	pal_liquid_pnt.z=liq->top;
	
	pal_liquid_size.x=liq->rgt-liq->lft;
	pal_liquid_size.y=liq->depth;
	pal_liquid_size.z=liq->bot-liq->top;
		
	list_palette_add_header(&map_palette,0,"Liquid Info");
	list_palette_add_int(&map_palette,-1,"Index",&pal_liquid_index,FALSE);
	list_palette_add_point(&map_palette,-1,"Position",&pal_liquid_pnt,TRUE);
	list_palette_add_point(&map_palette,-1,"Size",&pal_liquid_size,TRUE);
}

/* =======================================================

      Property Palette Click Liquid
      
======================================================= */

void map_palette_click_liquid(int liq_idx,bool double_click)
{
	int						id;
	map_liquid_type			*liq;

	liq=&map.liquid.liquids[liq_idx];

	id=map_palette.item_pane.click.id;

		// wave reset

	if (id==kLiquidPropertyWaveReset) {
		piece_liquid_reset_size(liq);
		return;
	}
}

