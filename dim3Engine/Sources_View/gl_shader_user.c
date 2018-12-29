/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Shader Routines

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
	#include "dim3engine.h"
#endif

#include "interface.h"

extern map_type				map;
extern view_type			view;
extern iface_type			iface;
extern setup_type			setup;
extern file_path_setup_type	file_path_setup;

int							nuser_shader;
shader_type					user_shaders[max_iface_user_shader];

/* =======================================================

      Load User Shader Files
      
======================================================= */

char* gl_shader_open_file(char *path)
{
	int				sz;
	char			*data;
	FILE			*file;
	struct stat		sb;
	
		// get file size
		
	if (stat(path,&sb)!=0) return(NULL);
	sz=sb.st_size;
	
		// open file
		
	file=fopen(path,"rb");
	if (file==NULL) return(NULL);
    
	data=(char*)malloc(sz+1);
    if (data==NULL) {
        fclose(file);
        return(NULL);
    }
	
	fread(data,1,sz,file);
	fclose(file);
	
	data[sz]=0x0;
    
	return(data);
}

/* =======================================================

      User Shader Init/Shutdown
      
======================================================= */

bool gl_user_shader_initialize(char *err_str)
{
	int						n;
	char					path[1024];
	char					*vertex_data,*fragment_data;
	bool					ok;
	iface_shader_type		*iface_shader;
	shader_type				*shader;

		// start the shaders from the setup list
	
	nuser_shader=iface.shader_list.nshader;
	
	iface_shader=iface.shader_list.shaders;
	shader=user_shaders;

	for (n=0;n!=nuser_shader;n++) {
		strcpy(shader->name,iface_shader->name);
		strcpy(shader->vertex_name,iface_shader->vert_name);
		strcpy(shader->fragment_name,iface_shader->frag_name);
		
		iface_shader++;
		shader++;
	}

		// clear the shaders

	shader=user_shaders;
	
	for (n=0;n!=nuser_shader;n++) {
		gl_shader_code_clear(shader);
		shader++;
	}
	
		// load the shaders

	shader=user_shaders;
	
	for (n=0;n!=nuser_shader;n++) {

			// load the shaders

		file_paths_data(&file_path_setup,path,"Shaders",shader->vertex_name,"vert");
		vertex_data=gl_shader_open_file(path);
		if (vertex_data==NULL) {
			sprintf(err_str,"GLSL Error: Can not open shader file '%s'",shader->vertex_name);
			gl_user_shader_shutdown();
			return(FALSE);
		}
			
		file_paths_data(&file_path_setup,path,"Shaders",shader->fragment_name,"frag");
		fragment_data=gl_shader_open_file(path);
		if (fragment_data==NULL) {
			sprintf(err_str,"GLSL Error: Can not open shader file '%s'",shader->fragment_name);
			gl_user_shader_shutdown();
			return(FALSE);
		}

			// compile the shaders

		ok=gl_shader_code_compile(shader,vertex_data,fragment_data,err_str);

		free(vertex_data);
		free(fragment_data);
		
		if (!ok) {
			gl_user_shader_shutdown();
			return(FALSE);
		}

		shader++;
	}

	return(TRUE);
}

void gl_user_shader_shutdown(void)
{
	int					n;
	shader_type			*shader;

		// shutdown shaders

	shader=user_shaders;
	
	for (n=0;n!=nuser_shader;n++) {
		gl_shader_code_shutdown(shader);
		shader++;
	}
}

/* =======================================================

      User Shader Utilities
      
======================================================= */

int gl_user_shader_find(char *name)
{
	int					n;
	shader_type			*shader;

	shader=user_shaders;
	
	for (n=0;n!=nuser_shader;n++) {
		if (strcasecmp(name,shader->name)==0) return(n);
		shader++;
	}
	
	return(gl_shader_core_index);
}
