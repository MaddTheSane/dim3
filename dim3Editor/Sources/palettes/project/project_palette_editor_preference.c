/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Property Palette Editor Preferences

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

#define kPrefPropertyEngineName				0
#define kPrefPropertyMipMapMode				1
#define kPrefPropertyFreeLook				2
#define kPrefPropertyAutoTexture			3
#define kPrefPropertyBigTexture				4
#define kPrefPropertyShowTangentBinormal	5
#define kPrefPropertyDuplicateOffset		6
#define kPrefPropertySnapSize				7
#define kPrefPropertyClipDistance			8
#define kPrefPropertyFlipHorzMovement		9
#define kPrefPropertyFlipVertMovement		10
#define kPrefPropertyFlipHorzTurn			11
#define kPrefPropertyFlipVertTurn			12
#define kPrefPropertyFlipForwardMovement	13
#define kPrefPropertyBackgroundColor		14
#define kPrefPropertyLineColor				15
#define kPrefPropertyMeshSelColor			16
#define kPrefPropertyPolySelColor			17
#define kPrefPropertyPolyCullColor			18

extern map_type					map;
extern app_pref_type			pref;

extern list_palette_type		project_palette;

/* =======================================================

      Property Palette Fill Editor Preferences
      
======================================================= */

void project_palette_fill_preferences(void)
{
	list_palette_set_title(&project_palette,"Editor Preferences",NULL,NULL,NULL,NULL,NULL);

	list_palette_add_header(&project_palette,0,"Editor Engine Launch");
	list_palette_add_string(&project_palette,kPrefPropertyEngineName,"Engine Name",pref.engine_name,name_str_len,FALSE);

	list_palette_add_header(&project_palette,0,"Editor Settings");
	list_palette_add_checkbox(&project_palette,kPrefPropertyFreeLook,"Free Look",&pref.free_look,FALSE);
	list_palette_add_checkbox(&project_palette,kPrefPropertyAutoTexture,"Auto Texture",&pref.auto_texture,FALSE);
	list_palette_add_checkbox(&project_palette,kPrefPropertyBigTexture,"Large Texture Palette",&pref.big_texture,FALSE);
	list_palette_add_checkbox(&project_palette,kPrefPropertyShowTangentBinormal,"Show Tangent-Binormal",&pref.show_tangent_binormal,FALSE);

	list_palette_add_header(&project_palette,0,"Editor Colors");
	list_palette_add_pick_color(&project_palette,kPrefPropertyBackgroundColor,"Background",&pref.col.background,FALSE);
	list_palette_add_pick_color(&project_palette,kPrefPropertyLineColor,"Mesh Line",&pref.col.mesh_line,FALSE);
	list_palette_add_pick_color(&project_palette,kPrefPropertyMeshSelColor,"Mesh Select",&pref.col.mesh_sel,FALSE);
	list_palette_add_pick_color(&project_palette,kPrefPropertyPolySelColor,"Polygon Select",&pref.col.poly_sel,FALSE);
	list_palette_add_pick_color(&project_palette,kPrefPropertyPolyCullColor,"Culled Polygons",&pref.col.poly_cull,FALSE);

	list_palette_add_header(&project_palette,0,"Editor Map Options");
	list_palette_add_int(&project_palette,kPrefPropertyDuplicateOffset,"Duplicate Offset",&pref.duplicate_offset,FALSE);
	list_palette_add_int(&project_palette,kPrefPropertySnapSize,"Snap Size",&pref.snap_size,FALSE);
	list_palette_add_int(&project_palette,kPrefPropertyClipDistance,"Clip Distance",&pref.clip_distance,FALSE);

	list_palette_add_header(&project_palette,0,"Editor Map Movement");
	list_palette_add_checkbox(&project_palette,kPrefPropertyFlipHorzMovement,"Flip Horizontal Movement",&pref.flip_horz_movement,FALSE);
	list_palette_add_checkbox(&project_palette,kPrefPropertyFlipVertMovement,"Flip Vertical Movement",&pref.flip_vert_movement,FALSE);
	list_palette_add_checkbox(&project_palette,kPrefPropertyFlipHorzTurn,"Flip Horizontal Turn",&pref.flip_horz_turn,FALSE);
	list_palette_add_checkbox(&project_palette,kPrefPropertyFlipVertTurn,"Flip Vertical Turn",&pref.flip_vert_turn,FALSE);
	list_palette_add_checkbox(&project_palette,kPrefPropertyFlipForwardMovement,"Flip Forward Movement",&pref.flip_forward_movement,FALSE);
}

/* =======================================================

      Property Palette Click Editor Preference
      
======================================================= */

void project_palette_click_preferences(bool double_click)
{
		// write prefs

	setup_xml_write();
}

