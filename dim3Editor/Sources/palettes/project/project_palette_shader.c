/****************************** File *********************************

Module: dim3 Setup
Author: Brian Barnes
 Usage: Property Palette Shader

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

#define kShaderSettingsName						0
#define kShaderCodeVertName						1
#define kShaderCodeFragName						2

extern iface_type				iface;
extern list_palette_type		project_palette;

/* =======================================================

      Property Palette Fill Shader
      
======================================================= */

void project_palette_fill_shader(int shader_idx)
{
	iface_shader_type			*shader;

	shader=&iface.shader_list.shaders[shader_idx];

	list_palette_set_title(&project_palette,"Shaders",NULL,"Shader",shader->name,NULL,NULL);

		// settings

	list_palette_add_header(&project_palette,0,"Settings");
	list_palette_add_string(&project_palette,kShaderSettingsName,"Name",shader->name,name_str_len,FALSE);

		// code

	list_palette_add_header(&project_palette,0,"Code");
	list_palette_add_picker_file(&project_palette,kShaderCodeVertName,list_button_none,0,"Vertex","Shaders","vert","",shader->vert_name,FALSE);
	list_palette_add_picker_file(&project_palette,kShaderCodeFragName,list_button_none,0,"Fragment","Shaders","frag","",shader->frag_name,FALSE);
}

/* =======================================================

      Property Palette Click Shader
      
======================================================= */

void project_palette_click_shader(int shader_idx,bool double_click)
{
}

