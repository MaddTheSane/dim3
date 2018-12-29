/****************************** File *********************************

Module: dim3 Setup
Author: Brian Barnes
 Usage: Property Palette Shaders

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

#define kShaderPropertyAdd						0

#define kShaderProperyName						1000
#define kShaderProperyDelete					2000

extern iface_type				iface;
extern app_state_type			state;
extern list_palette_type		project_palette;

/* =======================================================

      Property Palette Fill Shaders
      
======================================================= */

void project_palette_fill_shaders(void)
{
	int						n;

	list_palette_set_title(&project_palette,"Shaders",NULL,NULL,NULL,NULL,NULL);

	list_palette_add_header_button(&project_palette,kShaderPropertyAdd,"Shaders",list_button_plus);

	list_palette_sort_mark_start(&project_palette,&project_palette.item_pane);
	
	for (n=0;n!=iface.shader_list.nshader;n++) {
		list_palette_add_string_selectable_button(&project_palette,(kShaderProperyName+n),list_button_minus,(kShaderProperyDelete+n),iface.shader_list.shaders[n].name,(state.proj.cur_shader_idx==n),FALSE);
	}

	list_palette_sort(&project_palette,&project_palette.item_pane);
}

/* =======================================================

      Property Palette Click Shaders
      
======================================================= */

void project_palette_click_shaders(bool double_click)
{
	int				id,idx,sz;

	id=project_palette.item_pane.click.id;

		// shader edit
		
	if ((id>=kShaderProperyName) && (id<(kShaderProperyName+max_iface_user_shader))) {
		state.proj.cur_shader_idx=id-kShaderProperyName;
		if (double_click) list_palette_set_level(&project_palette,2);
		return;
	}
	
		// shader delete
		
	if ((id>=kShaderProperyDelete) && (id<(kShaderProperyDelete+max_iface_user_shader))) {
		state.proj.cur_shader_idx=-1;

		idx=id-kShaderProperyDelete;

		sz=(iface.shader_list.nshader-idx)-1;
		if (sz>0) memmove(&iface.shader_list.shaders[idx],&iface.shader_list.shaders[idx+1],(sz*sizeof(iface_shader_type)));

		iface.shader_list.nshader--;

		return;
	}
	
		// shader add

	if (id==kShaderPropertyAdd) {
		state.proj.cur_shader_idx=-1;

		if (iface.shader_list.nshader>=max_iface_user_shader) {
			os_dialog_alert("Add User Shader","Reached the maximum number of user shaders");
			return;
		}

		idx=iface.shader_list.nshader;
		iface.shader_list.nshader++;
		
		iface.shader_list.shaders[idx].name[0]=0x0;
		dialog_property_string_run(list_string_value_string,(void*)iface.shader_list.shaders[idx].name,name_str_len,0,0);
	
		iface.shader_list.shaders[idx].vert_name[0]=0x0;
		iface.shader_list.shaders[idx].frag_name[0]=0x0;

		state.proj.cur_shader_idx=idx;

		return;
	}
}

