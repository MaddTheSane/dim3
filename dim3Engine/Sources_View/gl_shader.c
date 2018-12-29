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
extern setup_type			setup;
extern file_path_setup_type	file_path_setup;

int							gl_shader_cur_mesh_idx;
shader_type					*gl_shader_current;

extern int					nuser_shader;
extern shader_type			user_shaders[max_iface_user_shader],
							core_shaders[max_shader_light+1][max_core_shader],
							color_shader,gradient_shader,black_shader,
							bitmap_shader;

extern float				gl_proj_matrix[16],gl_model_view_matrix[16],
							gl_normal_matrix[9],
							light_shader_direction[7][3];

extern bitmap_type			lmap_black_bitmap,lmap_white_bitmap;

/* =======================================================

      Initialize Shaders
      
======================================================= */

void gl_shader_initialize(void)
{
	gl_shader_current=NULL;
}

/* =======================================================

      Setup Shader Variables
      
======================================================= */

void gl_shader_set_instance_variables(shader_type *shader)
{
	GLint					var;
	
		// texture pointers
		
	var=glGetUniformLocation(shader->program_obj,"dim3Tex");
	if (var!=-1) glUniform1i(var,0);

	var=glGetUniformLocation(shader->program_obj,"dim3TexBump");
	if (var!=-1) glUniform1i(var,1);
	
	var=glGetUniformLocation(shader->program_obj,"dim3TexSpecular");
	if (var!=-1) glUniform1i(var,2);

	var=glGetUniformLocation(shader->program_obj,"dim3TexGlow");
	if (var!=-1) glUniform1i(var,3);
	
	var=glGetUniformLocation(shader->program_obj,"dim3TexLightMap");
	if (var!=-1) glUniform1i(var,4);
}

void gl_shader_reset_light_values(shader_type *shader)
{
	int				n;

	shader->current_hilite=0;		// 0 = not set, 1 = on, -1 = off

	shader->var_values.nlight=-1;

	for (n=0;n!=max_shader_light;n++) {
		shader->var_values.lights[n].intensity=-1.0f;
		shader->var_values.lights[n].invertIntensity=-1.0f;
		shader->var_values.lights[n].exponent=-1.0f;
		shader->var_values.lights[n].position.x=-1.0f;
		shader->var_values.lights[n].position.y=-1.0f;
		shader->var_values.lights[n].position.z=-1.0f;
		shader->var_values.lights[n].direction=-1;
		shader->var_values.lights[n].color.r=-1.0f;
		shader->var_values.lights[n].color.g=-1.0f;
		shader->var_values.lights[n].color.b=-1.0f;
	}
}

void gl_shader_cache_dynamic_variable_locations(shader_type *shader)
{
	int				n;
	char			var_name[256];

		// cache attribute locations

	shader->var_locs.dim3Vertex=glGetAttribLocation(shader->program_obj,"dim3Vertex");
	shader->var_locs.dim3VertexColor=glGetAttribLocation(shader->program_obj,"dim3VertexColor");
	shader->var_locs.dim3VertexUV=glGetAttribLocation(shader->program_obj,"dim3VertexUV");
	shader->var_locs.dim3VertexLightMapUV=glGetAttribLocation(shader->program_obj,"dim3VertexLightMapUV");
	shader->var_locs.dim3VertexTangent=glGetAttribLocation(shader->program_obj,"dim3VertexTangent");
	shader->var_locs.dim3VertexNormal=glGetAttribLocation(shader->program_obj,"dim3VertexNormal");
	
		// cache uniform locations
		
	shader->var_locs.dim3ProjectionMatrix=glGetUniformLocation(shader->program_obj,"dim3ProjectionMatrix");
	shader->var_locs.dim3ModelViewMatrix=glGetUniformLocation(shader->program_obj,"dim3ModelViewMatrix");
	shader->var_locs.dim3NormalMatrix=glGetUniformLocation(shader->program_obj,"dim3NormalMatrix");
		
	shader->var_locs.dim3FrequencySecond=glGetUniformLocation(shader->program_obj,"dim3FrequencySecond");
	shader->var_locs.dim3CameraPosition=glGetUniformLocation(shader->program_obj,"dim3CameraPosition");
	shader->var_locs.dim3AmbientColor=glGetUniformLocation(shader->program_obj,"dim3AmbientColor");
	shader->var_locs.dim3ShineFactor=glGetUniformLocation(shader->program_obj,"dim3ShineFactor");
	shader->var_locs.dim3GlowFactor=glGetUniformLocation(shader->program_obj,"dim3GlowFactor");
	shader->var_locs.dim3Alpha=glGetUniformLocation(shader->program_obj,"dim3Alpha");
	shader->var_locs.dim3DiffuseVector=glGetUniformLocation(shader->program_obj,"dim3DiffuseVector");

	shader->var_locs.dim3FogEnd=glGetUniformLocation(shader->program_obj,"dim3FogEnd");
	shader->var_locs.dim3FogScale=glGetUniformLocation(shader->program_obj,"dim3FogScale");
	shader->var_locs.dim3FogColor=glGetUniformLocation(shader->program_obj,"dim3FogColor");

	shader->var_locs.dim3SimpleColor=glGetUniformLocation(shader->program_obj,"dim3SimpleColor");
	
	for (n=0;n!=max_shader_light;n++) {
		sprintf(var_name,"dim3Light_%d.position",n);
		shader->var_locs.dim3Lights[n].position=glGetUniformLocation(shader->program_obj,var_name);
		sprintf(var_name,"dim3Light_%d.color",n);
		shader->var_locs.dim3Lights[n].color=glGetUniformLocation(shader->program_obj,var_name);
		sprintf(var_name,"dim3Light_%d.intensity",n);
		shader->var_locs.dim3Lights[n].intensity=glGetUniformLocation(shader->program_obj,var_name);
		sprintf(var_name,"dim3Light_%d.invertIntensity",n);
		shader->var_locs.dim3Lights[n].invertIntensity=glGetUniformLocation(shader->program_obj,var_name);
		sprintf(var_name,"dim3Light_%d.exponent",n);
		shader->var_locs.dim3Lights[n].exponent=glGetUniformLocation(shader->program_obj,var_name);
		sprintf(var_name,"dim3Light_%d.direction",n);
		shader->var_locs.dim3Lights[n].direction=glGetUniformLocation(shader->program_obj,var_name);
	}
	
		// we remember all the last settings
		// so we don't reset state when no necessary
			
	shader->var_values.attrib.vertex_offset=-1;
	shader->var_values.attrib.color_offset=-1;
	shader->var_values.attrib.uv_offset=-1;
	shader->var_values.attrib.lmap_uv_offset=-1;
	shader->var_values.attrib.tangent_offset=-1;
	shader->var_values.attrib.normal_offset=-1;
	
	shader->var_values.simple_color.r=shader->var_values.simple_color.g=shader->var_values.simple_color.b=shader->var_values.simple_color.a=-1.0f;

	shader->var_values.alpha=-1.0f;
	shader->var_values.shine_factor=-1.0f;
	shader->var_values.glow_factor=-1.0f;
	shader->var_values.diffuse_vct.x=shader->var_values.diffuse_vct.y=shader->var_values.diffuse_vct.z=-1.0f;

	gl_shader_reset_light_values(shader);
}

/* =======================================================

      Shader Error Reporter
      
======================================================= */

bool gl_shader_report_shader_error(char *err_str,char *vertex_name,char *fragment_name,GLuint shader_obj,char *err_type,char *code,int check_type)
{
	int				line;
	GLint			result,len;
	char			*c,*str,path[1024],log_line[256];
	FILE			*file;
	struct tm		*tm;
	time_t			curtime;
	
		// operation OK?
		
	glGetShaderiv(shader_obj,check_type,&result);
	if (result==1) return(TRUE);
	
		// start or append log file
		
	file_paths_app_data(&file_path_setup,path,"Debug","glsl_error_log","txt");

	file=fopen(path,"w");
	if (file==NULL) {
		strcpy(err_str,"GLSL Error: Could not write log file");
		return(FALSE);
	}
	
		// header
		
	fwrite("***\n",1,4,file);

	sprintf(log_line,"Error Type: %s\n",err_type);
	fwrite(log_line,1,strlen(log_line),file);
	
	if (vertex_name!=NULL) {
		sprintf(log_line,"Vertex: %s\n",vertex_name);
		fwrite(log_line,1,strlen(log_line),file);
	}

	if (fragment_name!=NULL) {
		sprintf(log_line,"Fragment: %s\n",fragment_name);
		fwrite(log_line,1,strlen(log_line),file);
	}

	curtime=time(NULL);
	tm=localtime(&curtime);
	
	sprintf(log_line,"Time: %.4d %.2d %.2d %.2d:%.2d.%.2d\n",(tm->tm_year+1900),(tm->tm_mon+1),tm->tm_mday,tm->tm_hour,tm->tm_min,tm->tm_sec);
	fwrite(log_line,1,strlen(log_line),file);

		// the error
		
	glGetShaderiv(shader_obj,GL_INFO_LOG_LENGTH,&len);
	if (len>0) {
		str=malloc(len);
		if (str!=NULL) {
			glGetShaderInfoLog(shader_obj,len,NULL,str);

			fwrite(str,1,len,file);
			fwrite("\n",1,1,file);

			free(str);
		}
	}
	
		// the code
		
	if (code!=NULL) {
	
		c=code;
		line=2;
		
		fwrite("001:",1,4,file);
		
		while (*c!=0x0) {
			
			if (*c=='\r') {
				c++;
				continue;
			}
			
			fwrite(c,1,1,file);
			
			if (*c=='\n') {
				sprintf(log_line,"%.3d:",line);
				fwrite(log_line,1,4,file);
				line++;
			}

			c++;
		}
	}
	
	fwrite("\n",1,1,file);
		
		// finish log

	fclose(file);

		// fatal error string

	strcpy(err_str,"GLSL: Could not compile shader.\nSee Debug\\glsl_error_log.txt for more information");
	
	return(FALSE);
}

bool gl_shader_report_program_error(char *err_str,char *vertex_name,char *fragment_name,GLuint program_obj,char *err_type,char *code,int check_type)
{
	int				line;
	GLint			result,len;
	char			*c,*str,path[1024],log_line[256];
	FILE			*file;
	struct tm		*tm;
	time_t			curtime;
	
		// operation OK?
		
	glGetProgramiv(program_obj,check_type,&result);
	if (result==1) return(TRUE);
	
		// start or append log file
		
	file_paths_app_data(&file_path_setup,path,"Debug","glsl_error_log","txt");

	file=fopen(path,"w");
	if (file==NULL) {
		strcpy(err_str,"GLSL Error: Could not write log file");
		return(FALSE);
	}
	
		// header
		
	fwrite("***\n",1,4,file);

	sprintf(log_line,"Error Type: %s\n",err_type);
	fwrite(log_line,1,strlen(log_line),file);
	
	if (vertex_name!=NULL) {
		sprintf(log_line,"Vertex: %s\n",vertex_name);
		fwrite(log_line,1,strlen(log_line),file);
	}

	if (fragment_name!=NULL) {
		sprintf(log_line,"Fragment: %s\n",fragment_name);
		fwrite(log_line,1,strlen(log_line),file);
	}

	curtime=time(NULL);
	tm=localtime(&curtime);
	
	sprintf(log_line,"Time: %.4d %.2d %.2d %.2d:%.2d.%.2d\n",(tm->tm_year+1900),(tm->tm_mon+1),tm->tm_mday,tm->tm_hour,tm->tm_min,tm->tm_sec);
	fwrite(log_line,1,strlen(log_line),file);

		// the error
		
	glGetProgramiv(program_obj,GL_INFO_LOG_LENGTH,&len);
	if (len>0) {
		str=malloc(len);
		if (str!=NULL) {
			glGetProgramInfoLog(program_obj,len,NULL,str);

			fwrite(str,1,len,file);
			fwrite("\n",1,1,file);

			free(str);
		}
	}
	
		// the code
		
	if (code!=NULL) {
	
		c=code;
		line=2;
		
		fwrite("001:",1,4,file);
		
		while (*c!=0x0) {
			
			if (*c=='\r') {
				c++;
				continue;
			}
			
			fwrite(c,1,1,file);
			
			if (*c=='\n') {
				sprintf(log_line,"%.3d:",line);
				fwrite(log_line,1,4,file);
				line++;
			}

			c++;
		}
	}
	
	fwrite("\n",1,1,file);
		
		// finish log

	fclose(file);

		// fatal error string

	strcpy(err_str,"GLSL: Could not link shader program.\nSee Debug\\glsl_error_log.txt for more information");
	
	return(FALSE);
}

/* =======================================================

      Compile Shader
      
======================================================= */

void gl_shader_code_clear(shader_type *shader)
{
	shader->vertex_obj=0;
	shader->fragment_obj=0;
	shader->program_obj=0;
}

bool gl_shader_code_compile(shader_type *shader,char *vertex_data,char *fragment_data,char *err_str)
{
	const GLchar			*vertex_ptr,*fragment_ptr;

		// create the shader
			
	shader->program_obj=glCreateProgram();
		
	shader->vertex_obj=glCreateShader(GL_VERTEX_SHADER);
	
	vertex_ptr=vertex_data;
	glShaderSource(shader->vertex_obj,1,&vertex_ptr,NULL);

	glCompileShader(shader->vertex_obj);
	if (!gl_shader_report_shader_error(err_str,shader->vertex_name,NULL,shader->vertex_obj,"Vertex",vertex_data,GL_COMPILE_STATUS)) return(FALSE);

	glAttachShader(shader->program_obj,shader->vertex_obj);
	
	shader->fragment_obj=glCreateShader(GL_FRAGMENT_SHADER);
	
	fragment_ptr=fragment_data;
	glShaderSource(shader->fragment_obj,1,&fragment_ptr,NULL);
	
	glCompileShader(shader->fragment_obj);
	if (!gl_shader_report_shader_error(err_str,NULL,shader->fragment_name,shader->fragment_obj,"Fragment",fragment_data,GL_COMPILE_STATUS)) return(FALSE);
	
	glAttachShader(shader->program_obj,shader->fragment_obj);
	
		// link shaders into program
		
	glLinkProgram(shader->program_obj);
	if (!gl_shader_report_program_error(err_str,shader->vertex_name,shader->fragment_name,shader->program_obj,"Program",NULL,GL_LINK_STATUS)) return(FALSE);

		// set the shader and then
		// setup the uniforms and attributes

	glUseProgram(shader->program_obj);

	gl_shader_set_instance_variables(shader);
	gl_shader_cache_dynamic_variable_locations(shader);

	glUseProgram(0);
	
		// timing only works for full screen shaders
		
	shader->start_tick=0;

	return(TRUE);
}

void gl_shader_code_shutdown(shader_type *shader)
{
	if (shader->vertex_obj!=0) {
		glDetachShader(shader->program_obj,shader->vertex_obj);
		glDeleteShader(shader->vertex_obj);
	}

	if (shader->fragment_obj!=0) {
		glDetachShader(shader->program_obj,shader->fragment_obj);
		glDeleteShader(shader->fragment_obj);
	}

	if (shader->program_obj!=0) glDeleteProgram(shader->program_obj);
}

/* =======================================================

      Attach Shaders to Textures
      
======================================================= */

void gl_shader_attach_map(void)
{
	int					n;
	texture_type		*texture;
	
	texture=map.textures;
	
	for (n=0;n!=max_map_texture;n++) {
		texture->shader_idx=gl_shader_core_index;
		
		if (texture->shader_name[0]!=0x0) {
			texture->shader_idx=gl_user_shader_find(texture->shader_name);
		}
		
		texture++;
	}
}

void gl_shader_attach_model(model_type *mdl)
{
	int					n;
	texture_type		*texture;
	
	texture=mdl->textures;
	
	for (n=0;n!=max_model_texture;n++) {
		texture->shader_idx=gl_shader_core_index;
		
		if (texture->shader_name[0]!=0x0) {
			texture->shader_idx=gl_user_shader_find(texture->shader_name);
		}
		
		texture++;
	}
}

/* =======================================================

      Scene Shader Variables
      
======================================================= */

void gl_shader_set_scene_variables(shader_type *shader)
{
	d3col			col;
		
		// other dim3 uniforms
	
	if (shader->var_locs.dim3FrequencySecond!=-1) glUniform1f(shader->var_locs.dim3FrequencySecond,game_time_fequency_second_get(shader->start_tick));
	if (shader->var_locs.dim3CameraPosition!=-1) glUniform3f(shader->var_locs.dim3CameraPosition,(float)view.render->camera.pnt.x,(float)view.render->camera.pnt.y,(float)view.render->camera.pnt.z);

	if (shader->var_locs.dim3AmbientColor!=-1) {
		gl_lights_calc_ambient_color(&col);
		glUniform3f(shader->var_locs.dim3AmbientColor,col.r,col.g,col.b);
	}
	
	if (fog_solid_on()) {
		if (shader->var_locs.dim3FogEnd!=-1) glUniform1f(shader->var_locs.dim3FogEnd,(float)map.fog.outer_radius);
		if (shader->var_locs.dim3FogScale!=-1) glUniform1f(shader->var_locs.dim3FogScale,(1.0f/(float)(map.fog.outer_radius-map.fog.inner_radius)));
		if (shader->var_locs.dim3FogColor!=-1) glUniform3f(shader->var_locs.dim3FogColor,map.fog.col.r,map.fog.col.g,map.fog.col.b);
	}
}

/* =======================================================

      Drawing Matrix Shader Variables
      
======================================================= */

void gl_shader_set_draw_matrix_variables(shader_type *shader)
{
		// only change matrix when we've reset any
		// of them, which happens per frame and anytime
		// certain view calls are made
		
	if (!shader->need_matrix_reset) return;
	
		// reset the matrixes
		
	shader->need_matrix_reset=FALSE;
	
	if (shader->var_locs.dim3ProjectionMatrix!=-1) glUniformMatrix4fv(shader->var_locs.dim3ProjectionMatrix,1,GL_FALSE,gl_proj_matrix);
	if (shader->var_locs.dim3ModelViewMatrix!=-1) glUniformMatrix4fv(shader->var_locs.dim3ModelViewMatrix,1,GL_FALSE,gl_model_view_matrix);
	if (shader->var_locs.dim3NormalMatrix!=-1) glUniformMatrix3fv(shader->var_locs.dim3NormalMatrix,1,GL_FALSE,gl_normal_matrix);
}

/* =======================================================

      Drawing Shader Variables
      
======================================================= */

void gl_shader_set_light_variables(shader_type *shader,int core_shader_group,bool is_core,view_glsl_light_list_type *light_list)
{
	int								n,k,count,max_light;
	float							f_intensity;
	d3col							*col,col_lmap={0.0f,0.0f,0.0f};
	view_light_spot_type			*lspot;
	shader_cached_var_light_loc		*loc_light;
	shader_current_var_light_value	*cur_light;
	
		// anything UI lite always replaces
		
	if (light_list->ui_light.on) {
	
		loc_light=&shader->var_locs.dim3Lights[0];
	
		f_intensity=(float)light_list->ui_light.intensity;
	
		glUniform3f(loc_light->position,(GLfloat)light_list->ui_light.pnt.x,(GLfloat)light_list->ui_light.pnt.y,(GLfloat)light_list->ui_light.pnt.z);
		glUniform3f(loc_light->color,light_list->ui_light.col.r,light_list->ui_light.col.g,light_list->ui_light.col.b);
		glUniform1f(loc_light->intensity,f_intensity);
		glUniform1f(loc_light->invertIntensity,(1.0f/f_intensity));
		glUniform1f(loc_light->exponent,light_list->ui_light.exponent);
		glUniform3fv(loc_light->direction,3,light_shader_direction[ld_all]);

		loc_light++;

			// set all other lights off

		for (n=1;n<max_shader_light;n++) {
			glUniform1f(loc_light->intensity,0.0f);
			glUniform1f(loc_light->invertIntensity,0.0f);
			loc_light++;
		}
	
			// make sure lights are always marked
			// as changed

		gl_shader_reset_light_values(shader);

		return;
	}
	
		// if in core and no lights,
		// skip all this as core shaders ignore lights

	if ((is_core) && (light_list->nlight==0)) return;
	
		// core shaders ignore lights outside their
		// range, so we don't need to look through max
		// lights for core shaders
		
	if (is_core) {
		max_light=light_list->nlight;
	}
	else {
		max_light=max_shader_light;
	}
	
		// have lights changed?
		
	if (light_list->nlight==shader->var_values.nlight) {
		
		count=0;
	
		for (n=0;n!=light_list->nlight;n++) {
			for (k=0;k!=shader->var_values.nlight;k++) {
				if (light_list->light_idx[n]==shader->var_values.light_idx[k]) {
					count++;
					break;
				}
			}
		}
		
		if (count==light_list->nlight) return;
	}
	
		// light changed, remember it for next time
		
	for (n=0;n!=light_list->nlight;n++) {
		shader->var_values.light_idx[n]=light_list->light_idx[n];
	}

	shader->var_values.nlight=light_list->nlight;
	
		// set the lights uniforms
		
	for (n=0;n!=max_light;n++) {

		loc_light=&shader->var_locs.dim3Lights[n];
		cur_light=&shader->var_values.lights[n];
	
			// lights that are off
			// have intensity of 0
			
		if (n>=light_list->nlight) {

			if (cur_light->intensity!=0.0f) {
				cur_light->intensity=0.0f;
				glUniform1f(loc_light->intensity,0.0f);
			}
			
			if (cur_light->invertIntensity!=0.0f) {
				cur_light->invertIntensity=0.0f;
				glUniform1f(loc_light->invertIntensity,0.0f);
			}

			continue;
		}
		
			// set regular light
			
		lspot=&view.render->light.spots[light_list->light_idx[n]];
		
		if (loc_light->position!=-1) {
			if ((cur_light->position.x!=lspot->pnt_eye_space.x) || (cur_light->position.y!=lspot->pnt_eye_space.y) || (cur_light->position.z!=lspot->pnt_eye_space.z)) {
				cur_light->position.x=lspot->pnt_eye_space.x;
				cur_light->position.y=lspot->pnt_eye_space.y;
				cur_light->position.z=lspot->pnt_eye_space.z;
				glUniform3f(loc_light->position,lspot->pnt_eye_space.x,lspot->pnt_eye_space.y,lspot->pnt_eye_space.z);
			}
		}

		if (loc_light->color!=-1) {
		
				// for non-models, anything that is light map
				// only gets an ambient of 0
			
			col=&lspot->col;
			
			if (core_shader_group!=core_shader_group_model) {
				if (lspot->light_map) col=&col_lmap;
			}
			
			if ((cur_light->color.r!=col->r) || (cur_light->color.g!=col->g) || (cur_light->color.b!=col->b)) {
				cur_light->color.r=col->r;
				cur_light->color.g=col->g;
				cur_light->color.b=col->b;
				glUniform3f(loc_light->color,col->r,col->g,col->b);
			}
		}

		if (loc_light->intensity!=-1) {
			if (cur_light->intensity!=lspot->f_intensity) {
				cur_light->intensity=lspot->f_intensity;
				glUniform1f(loc_light->intensity,lspot->f_intensity);
			}
		}
		
		if (loc_light->invertIntensity!=-1) {
			if (cur_light->invertIntensity!=lspot->f_inv_intensity) {
				cur_light->invertIntensity=lspot->f_inv_intensity;
				glUniform1f(loc_light->invertIntensity,lspot->f_inv_intensity);
			}
		}

		if (loc_light->exponent!=-1) {
			if (cur_light->exponent!=lspot->f_exponent) {
				cur_light->exponent=lspot->f_exponent;
				glUniform1f(loc_light->exponent,lspot->f_exponent);
			}
		}

		if (loc_light->direction!=-1) {
			if (cur_light->direction!=lspot->direction) {
				cur_light->direction=lspot->direction;
				glUniform3fv(loc_light->direction,3,light_shader_direction[lspot->direction]);
			}
		}
	}
}

void gl_shader_set_diffuse_variables(shader_type *shader,view_glsl_light_list_type *light_list)
{
		// diffuse vector

	if (shader->var_locs.dim3DiffuseVector!=-1) {
		if ((shader->var_values.diffuse_vct.x!=light_list->diffuse.vct.x) || (shader->var_values.diffuse_vct.y!=light_list->diffuse.vct.y) || (shader->var_values.diffuse_vct.z!=light_list->diffuse.vct.z)) {
			memmove(&shader->var_values.diffuse_vct,&light_list->diffuse.vct,sizeof(d3vct));
			glUniform3f(shader->var_locs.dim3DiffuseVector,light_list->diffuse.vct.x,light_list->diffuse.vct.y,light_list->diffuse.vct.z);
		}
	}
}

void gl_shader_set_poly_variables(shader_type *shader,float alpha)
{
		// alpha
		
	if (shader->var_locs.dim3Alpha!=-1) {
		if (shader->var_values.alpha!=alpha) {
			shader->var_values.alpha=alpha;
			glUniform1f(shader->var_locs.dim3Alpha,alpha);
		}
	}
}

void gl_shader_ambient_hilite_override(shader_type *shader,view_glsl_light_list_type *light_list,bool hilite)
{
	d3col			col;
	
	if (shader->var_locs.dim3AmbientColor==-1) return;
	
	if ((hilite) || (light_list->ui_light.on)) {
		glUniform3f(shader->var_locs.dim3AmbientColor,1.0f,1.0f,1.0f);
		return;
	}
	
	gl_lights_calc_ambient_color(&col);
	glUniform3f(shader->var_locs.dim3AmbientColor,col.r,col.g,col.b);
}

void gl_shader_hilite_override(shader_type *shader,view_glsl_light_list_type *light_list)
{
		// never set?
		
	if (shader->current_hilite==0) {
		if (light_list->hilite) {
			shader->current_hilite=1;
			gl_shader_ambient_hilite_override(shader,light_list,TRUE);
			return;
		}
		shader->current_hilite=-1;
		gl_shader_ambient_hilite_override(shader,light_list,FALSE);
		return;
	}
			
		// going into highlight

	if (light_list->hilite) {
		if (shader->current_hilite!=1) {
			shader->current_hilite=1;
			gl_shader_ambient_hilite_override(shader,light_list,TRUE);
		}
		return;
	}

		// not in highlight, check if
		// we are in and then fix override

	if (shader->current_hilite!=-1) {
		shader->current_hilite=-1;
		gl_shader_ambient_hilite_override(shader,light_list,FALSE);
	}
}

/* =======================================================

      Set Shader Textures
      
======================================================= */

void gl_shader_set_texture(shader_type *shader,int core_shader_group,texture_type *texture,int txt_idx,int lmap_txt_idx,int frame)
{
	GLuint			gl_id;

		// light map

	if ((core_shader_group==core_shader_group_map) || (core_shader_group==core_shader_group_liquid)) {
		if (setup.debug_on) {
			gl_id=lmap_white_bitmap.gl_id;
		}
		else {
			if (lmap_txt_idx==-1) {
				gl_id=lmap_black_bitmap.gl_id;
			}
			else {
				gl_id=map.textures[lmap_txt_idx].frames[0].bitmap.gl_id;
			}
		}
		if (gl_id!=-1) gl_texture_bind(4,gl_id);
	}

		// glow map

	gl_id=texture->frames[frame].glowmap.gl_id;

	if (gl_id!=-1) {
		gl_texture_bind(3,gl_id);
		
		if (shader->var_values.glow_factor!=texture->glow.current_color) {
			shader->var_values.glow_factor=texture->glow.current_color;
			if (shader->var_locs.dim3GlowFactor!=-1) glUniform1f(shader->var_locs.dim3GlowFactor,texture->glow.current_color);
		}
	}
	
		// spec map

	gl_id=texture->frames[frame].specularmap.gl_id;

	if (gl_id!=-1) {
		gl_texture_bind(2,gl_id);
		
		if (shader->var_values.shine_factor!=texture->shine_factor) {
			shader->var_values.shine_factor=texture->shine_factor;
			if (shader->var_locs.dim3ShineFactor!=-1) glUniform1f(shader->var_locs.dim3ShineFactor,texture->shine_factor);
		}
	}

		// bump map

	gl_id=texture->frames[frame].bumpmap.gl_id;
	if (gl_id!=-1) gl_texture_bind(1,gl_id);
	
		// color map

	gl_id=texture->frames[frame].bitmap.gl_id;
	if (gl_id!=-1) gl_texture_bind(0,gl_id);
}

void gl_shader_texture_override(GLuint gl_id,float alpha)
{
		// normally used to override for back rendering

	gl_texture_bind(0,gl_id);
	
		// nodes can override alpha
		// multiply in alpha so fade effects still work
		
	if (alpha==1.0f) return;
	
	if (gl_shader_current->var_locs.dim3Alpha!=-1) {
		alpha*=gl_shader_current->var_values.alpha;
		if (gl_shader_current->var_values.alpha!=alpha) {
			gl_shader_current->var_values.alpha=alpha;
			glUniform1f(gl_shader_current->var_locs.dim3Alpha,alpha);
		}
	}
}

/* =======================================================

      Shader Per Frame Setup
      
======================================================= */

static inline void gl_shader_frame_start_per_shader(shader_type *shader)
{
		// this flag marks when we need to reset
		// the matrixes (projection, model view, etc)
		// it's set per frame and then can get reset
		// if any of the specific view calls are hit
		
	shader->need_matrix_reset=TRUE;
	
		// use this flag to mark frame only variables
		// as needing a load.  In this way we optimize
		// out the amount of variable setting we need to do
	
	shader->need_frame_reset=TRUE;
		
		// dim3 tries to remember if the same lights
		// we set by looking into the light list.  The
		// light list is generated per frame, so it needs
		// to be reset here
		
		// need to set current hilite to 0, which means
		// it hasn't been set yet

	shader->var_values.nlight=-1;
	shader->current_hilite=0;
}

void gl_shader_frame_start(void)
{
	int					n,k;
	shader_type			*shader;

		// regular shaders
		
	for (k=0;k!=(max_shader_light+1);k++) {
		for (n=0;n!=max_core_shader;n++) {
			gl_shader_frame_start_per_shader(&core_shaders[k][n]);
		}
	}
	
		// simple shaders
		
	gl_shader_frame_start_per_shader(&color_shader);
	gl_shader_frame_start_per_shader(&gradient_shader);
	gl_shader_frame_start_per_shader(&black_shader);
	gl_shader_frame_start_per_shader(&bitmap_shader);

		// user shaders
		
	shader=user_shaders;

	for (n=0;n!=nuser_shader;n++) {
		gl_shader_frame_start_per_shader(shader);
		shader++;
	}
}

void gl_shader_force_matrix_resets(void)
{
	int					n,k;
	shader_type			*shader;

		// regular shaders
		
	for (k=0;k!=(max_shader_light+1);k++) {
		for (n=0;n!=max_core_shader;n++) {
			core_shaders[k][n].need_matrix_reset=TRUE;
		}
	}
	
		// simple shaders
		
	color_shader.need_matrix_reset=TRUE;
	gradient_shader.need_matrix_reset=TRUE;
	black_shader.need_matrix_reset=TRUE;
	bitmap_shader.need_matrix_reset=TRUE;

		// user shaders
		
	shader=user_shaders;

	for (n=0;n!=nuser_shader;n++) {
		shader->need_matrix_reset=TRUE;
		shader++;
	}
}


