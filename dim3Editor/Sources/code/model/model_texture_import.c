/****************************** File *********************************

Module: dim3 Animator
Author: Brian Barnes
 Usage: Model Texture Importer

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

extern model_type				model;
extern file_path_setup_type		file_path_setup;
extern app_state_type			state;

/* =======================================================

      Texture Counts
      
======================================================= */

int model_texture_count(void)
{
	int				n,count;
	texture_type	*texture;
	
	count=0;
	texture=model.textures;
	
    for (n=0;n!=max_model_texture;n++) {
		if (texture->frames[0].bitmap.gl_id!=-1) count++;
		texture++;
	}
	
	return(count);
}

/* =======================================================

      Texture Copy
      
======================================================= */

void texture_copy(char *path,char *name,char *sub_name)
{
    char			*c,dest_name[256],srce_path[1024],dest_path[1024],
					sub_path[1024];

	strcpy(srce_path,path);
	
	if (sub_name!=NULL) {
		c=strrchr(srce_path,'.');
		if (c==NULL) return;
		
		*c=0x0;
		strcat(srce_path,sub_name);
		strcat(srce_path,".png");
	}
	
	strcpy(dest_name,name);
	
	if (sub_name!=NULL) strcat(dest_name,sub_name);
	
	sprintf(sub_path,"Models/%s/Textures",model.name);
	file_paths_data_default(&file_path_setup,dest_path,sub_path,dest_name,"png");
	bitmap_copy(srce_path,dest_path);
}

/* =======================================================

      Does Texture Already Exist?
      
======================================================= */

bool model_texture_exists(char *material_name)
{
	int				n;
	texture_type	*texture;
	
	texture=model.textures;
	
    for (n=0;n!=max_model_texture;n++) {

		if (texture->frames[0].bitmap.gl_id!=-1) {
			if (strcasecmp(texture->material_name,material_name)==0) return(TRUE);
		}
		
		texture++;
	}
	
	return(FALSE);
}

/* =======================================================

      Texture Pick
      
======================================================= */

int model_texture_pick(char *material_name,char *err_str)
{
	int				n,idx;
    char			*c,path[1024],title[256],file_name[256];
	texture_type	*texture;
	
		// find a free texture or texture
		// that already has material name
		
	idx=-1;
	
	texture=model.textures;
	
    for (n=0;n!=max_model_texture;n++) {
	
			// a free texture
			
		if (texture->frames[0].bitmap.gl_id==-1) {
			if (idx==-1) idx=n;
		}
		
			// a used texture, check to see if
			// material already exists
			
		else {
			if (strcasecmp(texture->material_name,material_name)==0) return(n);
		}
		
		texture++;
	}
	
	if (idx==-1) return(0);
	
		// load texture and remember name
	
	texture=&model.textures[idx];
	strcpy(texture->material_name,material_name);
	
		// pick a bitmap
		
	sprintf(title,"Select a PNG for Material: %s",material_name);
	
	if (!os_load_file(title,path,"png")) {
		strcpy(err_str,"No texture was choosen.");
		return(-1);
	}
	
		// is it good?
		
	if (!bitmap_check(path,err_str)) return(-1);
	
		// get the actual file name
		
	c=strrchr(path,'/');
	if (c==NULL) c=strrchr(path,'\\');
	if (c==NULL) {
		strcpy(file_name,"unknown");
	}
	else {
		strcpy(file_name,(c+1));
		c=strrchr(file_name,'.');
		if (c!=NULL) *c=0x0;
	}
	
		// copy bitmaps to model folder
		// copy selected bitmap and any addition _n, _s, or _g files
		
	strcpy(texture->frames[0].name,file_name);
		
	texture_copy(path,texture->frames[0].name,NULL);
	texture_copy(path,texture->frames[0].name,"_n");
	texture_copy(path,texture->frames[0].name,"_s");
	texture_copy(path,texture->frames[0].name,"_g");
	
		// open the textures
		
	model_refresh_textures(&model);

	return(idx);
}


