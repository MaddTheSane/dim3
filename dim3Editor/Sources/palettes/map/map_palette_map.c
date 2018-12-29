/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Property Palette Map

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

#define kMapPropertyInfoTitle				0
#define kMapPropertyInfoAuthor				1

#define kMapPropertySingleplayerMapPick		2
#define kMapPropertyMultiplayerGameList		3

#define kMapPropertyGravity					10
#define kMapPropertyGravityMaxPower			11
#define kMapPropertyGravityMaxSpeed			12
#define kMapPropertyResistance				13
#define kMapPropertySlopeMinAngle			14
#define kMapPropertySlopeMaxAngle			15
#define kMapPropertySlopeMaxSpeed			16
#define kMapPropertySlopeMinGravity			17

#define kMapPropertyNetworkGameList			20

#define kMapPropertyNormalCull				30
#define kMapPropertyCullAngle				31
#define kMapPropertyRayTraceObscure			32
#define kMapPropertyModelObscureDistance	33
#define kMapPropertyShadowObscureDistance	34
#define kMapPropertyEffectObscureDistance	35
#define kMapPropertyShadowPolyMinArea		36
#define kMapPropertyShadowFloorOnly			37

#define kMapPropertyEditorTextureFactor		40
#define kMapPropertyEditorViewNearZ			41
#define kMapPropertyEditorViewFarZ			42
#define kMapPropertyEditorLinkStartAlways	43

#define kMapPropertyParamsStart				100
#define kMapPropertyParamsEnd				109

extern map_type					map;

extern list_palette_type		map_palette;

/* =======================================================

      Property Palette Fill Map
      
======================================================= */

void map_palette_fill_map(void)
{
	int				n;
	char			name[256];

	list_palette_set_title(&map_palette,"Map Settings",NULL,NULL,NULL,NULL,NULL);

		// info

	list_palette_add_header(&map_palette,0,"Map Info");
	list_palette_add_string(&map_palette,-1,"Name",map.info.name,name_str_len,TRUE);
	list_palette_add_string(&map_palette,kMapPropertyInfoTitle,"Title",map.info.title,name_str_len,FALSE);
	list_palette_add_string(&map_palette,kMapPropertyInfoAuthor,"Author",map.info.author,name_str_len,FALSE);

		// settings

	list_palette_add_header(&map_palette,0,"Map Physics");
	list_palette_add_float(&map_palette,kMapPropertyGravity,"Gravity",&map.physics.gravity,FALSE);
	list_palette_add_float(&map_palette,kMapPropertyGravityMaxPower,"Gravity Max Power",&map.physics.gravity_max_power,FALSE);
	list_palette_add_float(&map_palette,kMapPropertyGravityMaxSpeed,"Gravity Max Speed",&map.physics.gravity_max_speed,FALSE);
	list_palette_add_float(&map_palette,kMapPropertyResistance,"Resistance",&map.physics.resistance,FALSE);
	list_palette_add_float(&map_palette,kMapPropertySlopeMinAngle,"Slope Min Angle",&map.physics.slope_min_ang,FALSE);
	list_palette_add_float(&map_palette,kMapPropertySlopeMaxAngle,"Slope Max Angle",&map.physics.slope_max_ang,FALSE);
	list_palette_add_float(&map_palette,kMapPropertySlopeMaxSpeed,"Slope Max Speed",&map.physics.slope_max_speed,FALSE);
	list_palette_add_float(&map_palette,kMapPropertySlopeMinGravity,"Slope Min Gravity",&map.physics.slope_min_gravity,FALSE);

		// single player

	list_palette_add_header(&map_palette,0,"Singleplayer");
	list_palette_add_checkbox(&map_palette,kMapPropertySingleplayerMapPick,"Include in Map Picker",&map.singleplayer.map_picker,FALSE);

		// multiplayer

	list_palette_add_header(&map_palette,0,"Multiplayer");
	list_palette_add_string(&map_palette,kMapPropertyMultiplayerGameList,"Game List",map.multiplayer.game_list,256,FALSE);

		// optimizations

	list_palette_add_header(&map_palette,0,"Map Optimizations");
	list_palette_add_checkbox(&map_palette,kMapPropertyNormalCull,"Never Cull",&map.optimize.never_cull,FALSE);
	list_palette_add_float(&map_palette,kMapPropertyCullAngle,"Cull Angle",&map.optimize.cull_angle,FALSE);
	list_palette_add_checkbox(&map_palette,kMapPropertyRayTraceObscure,"Ray Trace Obscure",&map.optimize.ray_trace_obscure,FALSE);
	list_palette_add_int(&map_palette,kMapPropertyModelObscureDistance,"Model Obscure Distance",&map.optimize.obscure_dist.model,FALSE);
	list_palette_add_int(&map_palette,kMapPropertyShadowObscureDistance,"Shadow Obscure Distance",&map.optimize.obscure_dist.shadow,FALSE);
	list_palette_add_int(&map_palette,kMapPropertyEffectObscureDistance,"Effect Obscure Distance",&map.optimize.obscure_dist.effect,FALSE);
	list_palette_add_int(&map_palette,kMapPropertyShadowPolyMinArea,"Shadow Poly Single Min Area",&map.optimize.shadow_poly_min_area,FALSE);
	list_palette_add_checkbox(&map_palette,kMapPropertyShadowFloorOnly,"Shadow Floor Only",&map.optimize.shadow_floor_only,FALSE);

		// parameters

	list_palette_add_header(&map_palette,0,"Map Parameters");
	for (n=0;n!=10;n++) {
		sprintf(name,"Param %d",n);
		list_palette_add_parameter(&map_palette,(kMapPropertyParamsStart+n),name,map.settings.params,n,FALSE);
	}

		// editor setup

	list_palette_add_header(&map_palette,0,"Map Editor Setup");
	list_palette_add_int(&map_palette,kMapPropertyEditorViewNearZ,"Near Z",&map.editor_setup.view_near_dist,FALSE);
	list_palette_add_int(&map_palette,kMapPropertyEditorViewFarZ,"Far Z",&map.editor_setup.view_far_dist,FALSE);
	list_palette_add_checkbox(&map_palette,kMapPropertyEditorLinkStartAlways,"Always Start at Player Spot",&map.editor_setup.link_always_start,FALSE);

	list_palette_add_header(&map_palette,0,"Map Counts");
	list_palette_add_int(&map_palette,-1,"Meshes",&map.mesh.nmesh,TRUE);
	list_palette_add_int(&map_palette,-1,"Liquids",&map.liquid.nliquid,TRUE);
	list_palette_add_int(&map_palette,-1,"Spot",&map.nspot,TRUE);
	list_palette_add_int(&map_palette,-1,"Light",&map.nlight,TRUE);
	list_palette_add_int(&map_palette,-1,"Sound",&map.nsound,TRUE);
	list_palette_add_int(&map_palette,-1,"Particle",&map.nparticle,TRUE);
	list_palette_add_int(&map_palette,-1,"Scenery",&map.nscenery,TRUE);
	list_palette_add_int(&map_palette,-1,"Node",&map.nnode,TRUE);
}

/* =======================================================

      Property Palette Click Map
      
======================================================= */

void map_palette_click_map(bool double_click)
{
}

