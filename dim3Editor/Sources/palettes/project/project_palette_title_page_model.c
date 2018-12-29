/****************************** File *********************************

Module: dim3 Setup
Author: Brian Barnes
 Usage: Property Palette Title Page Model

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

#define kModelSettingsName						0
#define kModelSettingsAnimate					1

#define kModelPositionX							2
#define kModelPositionY							3

#define kModelOptionResize						4
#define kModelOptionRot							5

extern iface_type				iface;
extern list_palette_type		project_palette;

/* =======================================================

      Property Palette Fill Title Page Model
      
======================================================= */

void project_palette_fill_title_page_model(int intro_model_idx)
{
	iface_intro_model_type				*model;

	model=&iface.intro.model_list.models[intro_model_idx];

	list_palette_set_title(&project_palette,"Title Page",NULL,"Model",model->model_name,NULL,NULL);

		// settings

	list_palette_add_header(&project_palette,0,"Settings");
	list_palette_add_picker_file(&project_palette,kModelSettingsName,list_button_none,0,"Model","Models","","Mesh.xml;Model.xml",model->model_name,FALSE);
	list_palette_add_string(&project_palette,kModelSettingsAnimate,"Animation",model->animate_name,name_str_len,FALSE);

		// position

	list_palette_add_header(&project_palette,0,"Position");
	list_palette_add_int(&project_palette,kModelPositionX,"X",&model->x,FALSE);
	list_palette_add_int(&project_palette,kModelPositionY,"Y",&model->y,FALSE);

		// options

	list_palette_add_header(&project_palette,0,"Options");
	list_palette_add_float(&project_palette,kModelOptionResize,"Resize",&model->resize,FALSE);
	list_palette_add_angle(&project_palette,kModelOptionRot,"Rotate",&model->rot,FALSE);
}

/* =======================================================

      Property Palette Click Title Page Model
      
======================================================= */

void project_palette_click_title_page_model(int intro_model_idx,bool double_click)
{
}

