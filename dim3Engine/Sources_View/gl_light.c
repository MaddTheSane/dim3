/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: OpenGL Lights

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
	#include "dim3engine.h"
#endif

#include "interface.h"
#include "objects.h"

extern map_type				map;
extern view_type			view;
extern server_type			server;
extern camera_type			camera;
extern setup_type			setup;

float						light_shader_direction[7][3]={{0.0f,0.0f,0.0f},{1.0f,0.0f,0.0f},{-1.0f,0.0f,0.0f},{0.0f,1.0f,0.0f},{0.0f,-1.0f,0.0f},{0.0f,0.0f,1.0f},{0.0f,0.0f,-1.0f}},

							light_flicker_value[64]={
														1.00,1.00,1.00,1.00,1.00,1.00,1.00,1.00,
														0.92,0.85,0.70,0.85,0.92,1.00,0.92,0.85,
														0.92,1.00,1.00,1.00,1.00,0.92,0.85,0.70,
														0.50,0.70,0.85,0.70,0.85,0.92,1.00,1.00,
														1.00,1.00,1.00,1.00,0.92,0.85,0.70,0.85,
														0.70,0.85,0.92,1.00,1.00,0.92,0.85,0.70,
														0.50,0.70,0.85,0.92,1.00,1.00,0.92,1.00,
														0.92,0.85,0.70,0.85,0.92,1.00,1.00,1.00
													},
													
							light_fail_value[64]=	{
														0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
														0.00,0.15,0.45,0.15,0.00,0.00,0.00,0.15,
														0.00,0.00,0.00,0.00,0.00,0.00,0.15,0.45,
														0.85,0.45,0.15,0.45,0.15,0.00,0.00,0.00,
														0.00,0.00,0.00,0.00,0.00,0.15,0.45,0.15,
														0.45,0.15,0.00,0.00,0.00,0.00,0.15,0.45,
														0.85,0.45,0.15,0.00,0.00,0.00,0.00,0.00,
														0.00,0.15,0.45,0.15,0.00,0.00,0.00,0.00
													};
		
extern bool view_obj_in_draw_list(int obj_idx);
extern bool view_proj_in_draw_list(int proj_idx);

/* =======================================================

      Get Light Intensity For Type
      
======================================================= */

int gl_light_get_intensity(int tick,int light_type,int intensity)
{
	switch (light_type) {
	
		case lt_normal:
			return(intensity);
	
		case lt_blink:
			if ((tick&0x100)==0) return(intensity);
			return(0);
			
		case lt_glow:
			tick=tick&0x7FF;
			if (tick>1024) tick=2048-tick;
			return((int)((float)intensity*(0.75f+(((float)tick)*0.0003f))));
			
		case lt_pulse:
			tick=tick&0x3FF;
			if (tick>512) tick=1024-tick;
			return((int)((float)intensity*(0.5f+(((float)tick)*0.001f))));
			
		case lt_flicker:
			return((int)((float)intensity*light_flicker_value[(tick>>7)&0x3F]));
			break;
			
		case lt_failing:
			return((int)((float)intensity*light_fail_value[(tick>>7)&0x3F]));
			break;
	}
	
	return(intensity);
}

/* =======================================================

      Light Collisions and Directions
      
======================================================= */

static inline bool gl_lights_collide_with_box(view_light_spot_type *lspot,d3pnt *min,d3pnt *max)
{
	int				i_add;
	
	i_add=lspot->i_intensity;
		
	if ((lspot->pnt.x+i_add)<min->x) return(FALSE);
	if ((lspot->pnt.x-i_add)>max->x) return(FALSE);
	if ((lspot->pnt.z+i_add)<min->z) return(FALSE);
	if ((lspot->pnt.z-i_add)>max->z) return(FALSE);
	if ((lspot->pnt.y+i_add)<min->y) return(FALSE);
	if ((lspot->pnt.y-i_add)>max->y) return(FALSE);

		// in direction
		
	if (lspot->direction==ld_all) return(TRUE);

	switch (lspot->direction) {

		case ld_neg_x:
			if (min->x>lspot->pnt.x) return(FALSE);
			break;

		case ld_pos_x:
			if (max->x<lspot->pnt.x) return(FALSE);
			break;

		case ld_neg_y:
			if (min->y>lspot->pnt.y) return(FALSE);
			break;

		case ld_pos_y:
			if (max->y<lspot->pnt.y) return(FALSE);
			break;

		case ld_neg_z:
			if (min->z>lspot->pnt.z) return(FALSE);
			break;

		case ld_pos_z:
			if (max->z<lspot->pnt.z) return(FALSE);
			break;

	}
	
	return(TRUE);
}

static inline bool gl_lights_direction_float_ok(float x,float y,float z,view_light_spot_type *lspot)
{
	switch (lspot->direction) {

		case ld_all:
			return(TRUE);

		case ld_neg_x:
			return(x<=lspot->f_x);

		case ld_pos_x:
			return(x>=lspot->f_x);

		case ld_neg_y:
			return(y<=lspot->f_y);

		case ld_pos_y:
			return(y>=lspot->f_y);

		case ld_neg_z:
			return(z<=lspot->f_z);

		case ld_pos_z:
			return(z>=lspot->f_z);

	}

	return(TRUE);
}

static inline bool gl_lights_direction_pnt_ok(d3pnt *pnt,view_light_spot_type *lspot)
{
	switch (lspot->direction) {

		case ld_all:
			return(TRUE);

		case ld_neg_x:
			return(pnt->x<=lspot->pnt.x);

		case ld_pos_x:
			return(pnt->x>=lspot->pnt.x);

		case ld_neg_y:
			return(pnt->y<=lspot->pnt.y);

		case ld_pos_y:
			return(pnt->y>=lspot->pnt.y);

		case ld_neg_z:
			return(pnt->z<=lspot->pnt.z);

		case ld_pos_z:
			return(pnt->z>=lspot->pnt.z);

	}

	return(TRUE);
}

/* =======================================================

      Setup Meshes, Liquids, and Models for
	  Lighting Calculations (get a list of hit lights
	  to make later calculations quicker)
      
======================================================= */

int gl_lights_setup_box(d3pnt *mid,d3pnt *min,d3pnt *max,int max_list_idx,int *list_idx,bool light_map_skip)
{
	int							n,k,far_idx,count;
	float						f,fx,fy,fz,far_dist,
								list_dist[max_map_light_cache_index];
	view_light_spot_type		*lspot;

		// find all lights which contact this mesh

	count=0;

	for (n=0;n!=view.render->light.count;n++) {
		lspot=&view.render->light.spots[n];
		
			// light map skip
			
		if (light_map_skip) {
			if (lspot->light_map) continue;
		}

			// box hit?
			
		if (!gl_lights_collide_with_box(lspot,min,max)) continue;

			// get distance
			// skip square root as we are comparing
			// them against other distances

		fx=(float)(mid->x-lspot->pnt.x);
		fy=(float)(mid->y-lspot->pnt.y);
		fz=(float)(mid->z-lspot->pnt.z);

		f=((fx*fx)+(fy*fy)+(fz*fz));

			// put in list

		if (count<max_list_idx) {
			list_idx[count]=n;
			list_dist[count]=f;
			count++;
		}
		else {
			far_idx=0;
			far_dist=list_dist[0];

			for (k=1;k<count;k++) {
				if (list_dist[k]>far_dist) {
					far_idx=k;
					far_dist=list_dist[k];
				}
			}

			if (f<far_dist) {
				list_idx[far_idx]=n;
				list_dist[far_idx]=f;
			}
		}
	}

	return(count);
}

void gl_lights_setup_liquid(map_liquid_type *liq)
{
	d3pnt			mid,min,max;

	mid.x=(liq->lft+liq->rgt)>>1;
	mid.y=liq->y;
	mid.z=(liq->top+liq->bot)>>1;
	
	min.x=liq->lft;
	min.y=liq->y;
	min.z=liq->top;
	
	max.x=liq->rgt;
	max.y=liq->y;
	max.z=liq->bot;

	liq->light_cache.count=gl_lights_setup_box(&mid,&min,&max,max_shader_light,liq->light_cache.indexes,FALSE);
}

void gl_lights_setup_model(model_draw *draw)
{
	d3pnt				mid,min,max;

	model_get_view_min_max(draw,&mid,&min,&max);
	draw->light_cache.count=gl_lights_setup_box(&mid,&min,&max,max_shader_light,draw->light_cache.indexes,FALSE);
}

void gl_lights_setup_cache(void)
{
	int					n;
	obj_type			*obj;
	proj_type			*proj;
	weapon_type			*weap;
	map_mesh_type		*mesh;
	
		// cache all the lights for the meshes, liquids,
		// and models in this view

		// meshes remember all lights hitting them
		// so polygons have a better list.  Everything else
		// just gets max shader lights
		
	for (n=0;n!=view.render->draw_list.count;n++) {

		switch (view.render->draw_list.items[n].type) {
			
			case view_render_type_mesh:
				mesh=&map.mesh.meshes[view.render->draw_list.items[n].idx];
				mesh->light_cache.count=gl_lights_setup_box(&mesh->box.mid,&mesh->box.min,&mesh->box.max,max_map_light_cache_index,mesh->light_cache.indexes,FALSE);
				break;

			case view_render_type_liquid:
				gl_lights_setup_liquid(&map.liquid.liquids[view.render->draw_list.items[n].idx]);
				break;

			case view_render_type_object:
				obj=server.obj_list.objs[view.render->draw_list.items[n].idx];

				obj->draw.light_cache.count=0;
				if ((view.render->draw_list.items[n].flag&view_list_item_flag_model_in_view)!=0x0) gl_lights_setup_model(&obj->draw);
				break;

			case view_render_type_projectile:
				proj=server.proj_list.projs[view.render->draw_list.items[n].idx];

				proj->draw.light_cache.count=0;
				if ((view.render->draw_list.items[n].flag&view_list_item_flag_model_in_view)!=0x0) gl_lights_setup_model(&proj->draw);
				break;
				
		}
	}

		// if in FPP, then hand weapon needs
		// light cache

	if (map.camera.camera_mode==cv_fpp) {

		obj=server.obj_list.objs[server.player_obj_idx];

		if (obj->held_weapon.current_idx!=-1) {
			weap=weapon_find_current(obj);
			if (weap!=NULL) {
				weap->draw.light_cache.count=0;
				gl_lights_setup_model(&weap->draw);
				
				if ((weap->dual.on) && (weap->dual.active)) {
					weap->draw_dual.light_cache.count=0;
					gl_lights_setup_model(&weap->draw_dual);
				}
			}
		}

	}
}

/* =======================================================

      Compile Light List
      
======================================================= */

void gl_lights_compile_add(int tick,d3pnt *pnt,int light_type,bool light_map,int intensity,float exponent,int direction,d3col *col)
{
	view_light_spot_type			*lspot;
	
		// already too many lights?

	if (view.render->light.count==max_light_spot) return;

		// calculate intensity

	intensity=gl_light_get_intensity(tick,light_type,intensity);
	if (intensity<=0) return;
	
		// create light

	lspot=&view.render->light.spots[view.render->light.count];
	
		// create intensity for light type
		
	lspot->i_intensity=intensity;			// need alternate versions of data to speed up later calculations
	lspot->f_intensity=(float)intensity;
	lspot->f_exponent=exponent;
	
	lspot->light_map=light_map;
	lspot->direction=direction;
	
		// create light position and color
		
	memmove(&lspot->pnt,pnt,sizeof(d3pnt));
	memmove(&lspot->col,col,sizeof(d3col));

		// non shader calculation speed ups

	lspot->f_inv_intensity=1.0f/lspot->f_intensity;

	lspot->f_x=(float)lspot->pnt.x;
	lspot->f_y=(float)lspot->pnt.y;
	lspot->f_z=(float)lspot->pnt.z;

		// eye space for shaders

	lspot->pnt_eye_space.x=lspot->f_x;
	lspot->pnt_eye_space.y=lspot->f_y;
	lspot->pnt_eye_space.z=lspot->f_z;
		
	gl_project_to_eye_coordinates(&lspot->pnt_eye_space);

	view.render->light.count++;
}

void gl_lights_compile_model_add(int tick,model_draw *draw)
{
	int					n;
	d3pnt				pnt;
	model_type			*mdl;
	model_draw_light	*light;

		// any model?
		
	if ((draw->model_idx==-1) || (!draw->on)) return;
	
	mdl=server.model_list.models[draw->model_idx];
	
		// add lights
		
	light=draw->lights;
	
	for (n=0;n!=max_model_light;n++) {

		if (light->on) {
			memmove(&pnt,&draw->pnt,sizeof(d3pnt));
			
			model_get_light_position(mdl,&draw->setup,n,&pnt);
			gl_lights_compile_add(tick,&pnt,light->type,FALSE,light->intensity,light->exponent,light->direction,&light->col);
		}

		light++;
	}
}

void gl_lights_compile_effect_add(int tick,effect_type *effect)
{
	int					intensity,count,mid_tick,fade_tick;
	flash_effect_data	*flash;
	
	if (effect->effecttype!=ef_flash) return;

		// calculate flash
		
	flash=&effect->data.flash;

	intensity=flash->intensity;
	mid_tick=flash->mid_tick;
	
	count=tick-effect->start_tick;

	if (count<mid_tick) {										// start of flash
		if (mid_tick>0) intensity=(intensity*count)/mid_tick;
	}
	else {
		if (count>mid_tick) {									// fade of flash
			fade_tick=(effect->life_tick-flash->mid_tick);
			count=fade_tick-(count-mid_tick);
	
			if (fade_tick>0) intensity=(intensity*count)/fade_tick;
		}
	}
	
	gl_lights_compile_add(tick,&effect->pnt,lt_normal,FALSE,intensity,flash->exponent,ld_all,&flash->col);
}

void gl_lights_compile(int tick)
{
	int					n;
	map_light_type		*lit;
	map_particle_type	*particle;
	obj_type			*obj;
	weapon_type			*weap;
	proj_type			*proj;
	effect_type			*effect;
	
	view.render->light.count=0;

		// map lights
		
	lit=map.lights;
		
	for (n=0;n!=map.nlight;n++) {
		if (lit->setting.on) gl_lights_compile_add(tick,&lit->pnt,lit->setting.type,lit->setting.light_map,lit->setting.intensity,lit->setting.exponent,lit->setting.direction,&lit->setting.col);
		lit++;
	}
	
		// map particles
		
	particle=map.particles;
		
	for (n=0;n!=map.nparticle;n++) {
		if (particle->light_setting.on) gl_lights_compile_add(tick,&particle->pnt,particle->light_setting.type,particle->light_setting.light_map,particle->light_setting.intensity,particle->light_setting.exponent,particle->light_setting.direction,&particle->light_setting.col);
		particle++;
	}	

		// lights from objects and their weapons
	
	for (n=0;n!=max_obj_list;n++) {
		obj=server.obj_list.objs[n];
		if (obj==NULL) continue;

		gl_lights_compile_model_add(tick,&obj->draw);
		if (obj->held_weapon.current_idx!=-1) {
			weap=weapon_find_current(obj);
			if (weap!=NULL) {
				gl_lights_compile_model_add(tick,&weap->draw);
				if ((weap->dual.on) && (weap->dual.active)) gl_lights_compile_model_add(tick,&weap->draw_dual);
			}
		}
	}
	
		// lights from projectiles

	for (n=0;n!=max_proj_list;n++) {
		proj=server.proj_list.projs[n];
		if (!proj->on) continue;
		
		gl_lights_compile_model_add(tick,&proj->draw);
	}
	
		// lights from effects
		
	for (n=0;n!=max_effect_list;n++) {
		effect=server.effect_list.effects[n];
		if (!effect->on) continue;

		gl_lights_compile_effect_add(tick,effect);		
	}
	
		// setup per render item caches

	gl_lights_setup_cache();
}

/* =======================================================

      Calculate Ambient Only Lighting
      
======================================================= */

void gl_lights_calc_ambient_color(d3col *col)
{
	col->r=map.ambient.light_color.r+setup.gamma;
	col->g=map.ambient.light_color.g+setup.gamma;
	col->b=map.ambient.light_color.b+setup.gamma;
}

/* =======================================================

      Calculate Diffuse Vector
      
======================================================= */

void gl_lights_calc_diffuse_vector(d3pnt *pnt,int count,int *indexes,d3vct *vct)
{
	int						n,light_count;
	float					f_light_count;
	d3vct					light_vct;
	view_light_spot_type	*lspot;
	
		// average lights
		
	light_count=0;
	vct->x=vct->y=vct->z=0.0f;
	
		// if there is a ambient, then
		// consider it a light from above
		
	if ((map.ambient.light_color.r!=0.0f) || (map.ambient.light_color.g!=0.0f) || (map.ambient.light_color.b!=0.0f)) {
		light_count++;
		vct->y+=-1.0f;
	}
	
	for (n=0;n!=count;n++) {
	
		lspot=&view.render->light.spots[indexes[n]];

		light_count++;
		
		light_vct.x=(float)(lspot->pnt.x-pnt->x);
		light_vct.y=(float)(lspot->pnt.y-pnt->y);
		light_vct.z=(float)(lspot->pnt.z-pnt->z);
		
		vector_normalize(&light_vct);
		
		vct->x+=light_vct.x;
		vct->y+=light_vct.y;
		vct->z+=light_vct.z;
	}
	
		// if no lights, count as light from above
		
	if (light_count==0) {
		vct->x=0.0f;
		vct->y=-1.0f;
		vct->z=0.0f;
		return;
	}
	
		// average
		
	f_light_count=(float)light_count;
		
	vct->x/=f_light_count;
	vct->y/=f_light_count;
	vct->z/=f_light_count;
	
		// and normalize
		
	vector_normalize(vct);
}

/* =======================================================

      Calculate Lighting at a Point
      
======================================================= */

void gl_lights_calc_color(float x,float y,float z,float *cf)
{
	int						n;
	float					f,fx,fy,fz,mult,r,g,b;
	view_light_spot_type	*lspot;
	
		// combine all light spots attenuated for distance
		
	r=g=b=0.0f;

	lspot=view.render->light.spots;
	
	for (n=0;n!=view.render->light.count;n++) {

		fx=lspot->f_x-x;
		fy=lspot->f_y-y;
		fz=lspot->f_z-z;
		
		f=sqrtf((fx*fx)+(fz*fz)+(fy*fy));

		if (f<=lspot->f_intensity) {
			if (gl_lights_direction_float_ok(x,y,z,lspot)) {

				mult=(lspot->f_intensity-f)*lspot->f_inv_intensity;
				
				mult+=powf(mult,lspot->f_exponent);

				r+=(lspot->col.r*mult);
				g+=(lspot->col.g*mult);
				b+=(lspot->col.b*mult);
			}
		}

		lspot++;
	}

		// set light value

	*cf++=(map.ambient.light_color.r+setup.gamma)+r;
	*cf++=(map.ambient.light_color.g+setup.gamma)+g;
	*cf=(map.ambient.light_color.b+setup.gamma)+b;
}

void gl_lights_calc_color_light_cache_byte(int count,int *indexes,bool skip_light_map,float x,float y,float z,unsigned char *cp)
{
	int						n,k;
	float					f,fx,fy,fz,mult,r,g,b;
	view_light_spot_type	*lspot;

		// combine all light spots attenuated for distance
		
	r=g=b=0.0f;
	
	for (n=0;n!=count;n++) {
	
		lspot=&view.render->light.spots[indexes[n]];
		if ((skip_light_map) && (lspot->light_map)) continue;

		fx=lspot->f_x-x;
		fy=lspot->f_y-y;
		fz=lspot->f_z-z;
		
		f=sqrtf((fx*fx)+(fy*fy)+(fz*fz));

		if (f<=lspot->f_intensity) {

			if (gl_lights_direction_float_ok(x,y,z,lspot)) {

				mult=(lspot->f_intensity-f)*lspot->f_inv_intensity;
				
				mult+=powf(mult,lspot->f_exponent);

				r+=(lspot->col.r*mult);
				g+=(lspot->col.g*mult);
				b+=(lspot->col.b*mult);
			}
		}
	}

		// set light value
		
	k=(int)(((map.ambient.light_color.r+setup.gamma)+r)*255.0f);
	if (k>255) k=255;
	*cp++=(unsigned char)k;
	
	k=(int)(((map.ambient.light_color.g+setup.gamma)+g)*255.0f);
	if (k>255) k=255;
	*cp++=(unsigned char)k;
	
	k=(int)(((map.ambient.light_color.b+setup.gamma)+b)*255.0f);
	if (k>255) k=255;
	*cp=(unsigned char)k;
}

void gl_lights_calc_color_light_cache_float(int count,int *indexes,bool skip_light_map,float x,float y,float z,float *cp)
{
	int						n;
	float					f,fx,fy,fz,mult,r,g,b;
	view_light_spot_type	*lspot;

		// combine all light spots attenuated for distance
		
	r=g=b=0.0f;
	
	for (n=0;n!=count;n++) {
	
		lspot=&view.render->light.spots[indexes[n]];
		if ((skip_light_map) && (lspot->light_map)) continue;

		fx=lspot->f_x-x;
		fy=lspot->f_y-y;
		fz=lspot->f_z-z;
		
		f=sqrtf((fx*fx)+(fy*fy)+(fz*fz));

		if (f<=lspot->f_intensity) {

			if (gl_lights_direction_float_ok(x,y,z,lspot)) {

				mult=(lspot->f_intensity-f)*lspot->f_inv_intensity;
				
				mult+=powf(mult,lspot->f_exponent);

				r+=(lspot->col.r*mult);
				g+=(lspot->col.g*mult);
				b+=(lspot->col.b*mult);
			}
		}
	}

		// set light value

	f=(map.ambient.light_color.r+setup.gamma)+r;
	if (f>1.0f) f=1.0f;
	*cp++=f;
	
	f=(map.ambient.light_color.g+setup.gamma)+g;
	if (f>1.0f) f=1.0f;
	*cp++=f;
	
	f=(map.ambient.light_color.b+setup.gamma)+b;
	if (f>1.0f) f=1.0f;
	*cp=f;
}

/* =======================================================

      Find Averaged Shadow Light
      
======================================================= */

int gl_light_get_averaged_shadow_light(d3pnt *pnt,d3pnt *size,d3pnt *light_pnt)
{
	int						n,hit_idx,count;
	float					f,fx,fy,fz,
							f_tot_intensity,f_weight_intensity;
	float					f_intensity[max_light_spot];
	view_light_spot_type	*lspot;

		// no lights in scene

	if (view.render->light.count==0) return(-1);
	
		// get the intensity for all the lights
		// hit in the cache
		
	count=0;
	hit_idx=0;

	f_tot_intensity=0.0f;
		
	for (n=0;n!=view.render->light.count;n++) {
		lspot=&view.render->light.spots[n];

		f_intensity[n]=0.0f;

			// get distance
			
		fx=(float)(lspot->pnt.x-pnt->x);
		fy=(float)(lspot->pnt.y-pnt->y);
		fz=(float)(lspot->pnt.z-pnt->z);

		f=sqrtf((fx*fx)+(fy*fy)+(fz*fz));

		if (f>lspot->f_intensity) continue;
		if (!gl_lights_direction_pnt_ok(pnt,lspot)) continue;

		f_intensity[n]=(lspot->f_intensity-f)*lspot->f_inv_intensity;
		f_intensity[n]+=powf(f_intensity[n],lspot->f_exponent);
		
			// added up the total intensity
			// so we can weight them later
					
		f_tot_intensity+=f_intensity[n];

		hit_idx=n;
		count++;
	}

		// no hits

	if (count==0) return(-1);

		// special check for single light
		
	if (count==1) {
		lspot=&view.render->light.spots[hit_idx];
		light_pnt->x=lspot->pnt.x;
		light_pnt->y=lspot->pnt.y;
		light_pnt->z=lspot->pnt.z;
		return(lspot->i_intensity);
	}
	
		// now we need to weighed average
		// them together.  We give more
		// weight to the ones that are
		// more intense

		// we also need to rerun the instensities
		// because the other routines expect
		// intensity at light, not at model
		
	light_pnt->x=0;
	light_pnt->y=0;
	light_pnt->z=0;

	f_weight_intensity=0.0f;
	
	for (n=0;n!=view.render->light.count;n++) {
		if (f_intensity[n]==0.0f) continue;

		lspot=&view.render->light.spots[n];
		
		f=f_intensity[n]/f_tot_intensity;
		
		light_pnt->x+=(int)(((float)lspot->pnt.x)*f);
		light_pnt->y+=(int)(((float)lspot->pnt.y)*f);
		light_pnt->z+=(int)(((float)lspot->pnt.z)*f);

		f_weight_intensity+=(lspot->f_intensity*f);
	}
	
	return((int)f_weight_intensity);
}

/* =======================================================

      GLSL Light Lists
      
======================================================= */

void gl_lights_build_mesh_glsl_light_list(map_mesh_type *mesh,view_glsl_light_list_type *light_list)
{
	int					n;

		// misc settings, mostly for models
		
	light_list->ui_light.on=FALSE;

	light_list->diffuse.vct.x=light_list->diffuse.vct.y=light_list->diffuse.vct.z=0.0f;
	light_list->diffuse.boost=0.0f;

		// highlighting

	light_list->hilite=setup.debug_on;
	
		// this setup is for small meshes
		// that are small enough so we don't
		// have to recalc lights for each
		// polygon

	light_list->nlight=mesh->light_cache.count;
	if (light_list->nlight>max_shader_light) light_list->nlight=max_shader_light;
		
	for (n=0;n!=light_list->nlight;n++) {
		light_list->light_idx[n]=mesh->light_cache.indexes[n];
	}
}

void gl_lights_build_poly_glsl_light_list(map_mesh_type *mesh,map_mesh_poly_type *poly,view_glsl_light_list_type *light_list)
{
	int						n,k,count,far_idx;
	float					f,fx,fy,fz,far_dist,
							list_dist[max_shader_light];
	view_light_spot_type	*lspot;

		// misc settings, mostly for models
		
	light_list->ui_light.on=FALSE;

	light_list->diffuse.vct.x=light_list->diffuse.vct.y=light_list->diffuse.vct.z=0.0f;
	light_list->diffuse.boost=0.0f;

		// highlighting

	light_list->hilite=setup.debug_on;
	
		// meshes already have a reduced light list
		// if the mesh already has <= max_shader_light
		// lights, just use them

	if (mesh->light_cache.count<=max_shader_light) {
		light_list->nlight=mesh->light_cache.count;
		
		for (n=0;n!=light_list->nlight;n++) {
			light_list->light_idx[n]=mesh->light_cache.indexes[n];
		}
		
		return;
	}

		// otherwise we need to calc the
		// closest from the list
		
	count=0;
		
	for (n=0;n!=mesh->light_cache.count;n++) {
		lspot=&view.render->light.spots[mesh->light_cache.indexes[n]];

			// box hit?
			
		if (!gl_lights_collide_with_box(lspot,&poly->box.min,&poly->box.max)) continue;

			// get distance
			// skip square root as we are comparing
			// them against other distances

		fx=(float)(poly->box.mid.x-lspot->pnt.x);
		fy=(float)(poly->box.mid.y-lspot->pnt.y);
		fz=(float)(poly->box.mid.z-lspot->pnt.z);

		f=((fx*fx)+(fy*fy)+(fz*fz));

			// put in list

		if (count<max_shader_light) {
			light_list->light_idx[count]=mesh->light_cache.indexes[n];
			list_dist[count]=f;
			count++;
		}
		else {
			far_idx=0;
			far_dist=list_dist[0];

			for (k=1;k<count;k++) {
				if (list_dist[k]>far_dist) {
					far_idx=k;
					far_dist=list_dist[k];
				}
			}

			if (f<far_dist) {
				light_list->light_idx[far_idx]=mesh->light_cache.indexes[n];
				list_dist[far_idx]=f;
			}
		}
	}
		
	light_list->nlight=count;
}

void gl_lights_build_liquid_glsl_light_list(map_liquid_type *liq,view_glsl_light_list_type *light_list)
{
	int				n;

		// misc settings, mostly for models
		
	light_list->ui_light.on=FALSE;

	light_list->diffuse.vct.x=light_list->diffuse.vct.y=light_list->diffuse.vct.z=0.0f;
	light_list->diffuse.boost=0.0f;

		// highlighting

	light_list->hilite=setup.debug_on;

		// lights

	light_list->nlight=liq->light_cache.count;
	if (light_list->nlight>max_shader_light) light_list->nlight=max_shader_light;
		
	for (n=0;n!=light_list->nlight;n++) {
		light_list->light_idx[n]=liq->light_cache.indexes[n];
	}
}

void gl_lights_build_model_glsl_light_list(model_type *mdl,model_draw *draw,view_glsl_light_list_type *light_list)
{
	int				n;
	
		// special check for UI lighted models
		
	if (draw->ui_lighting) {
		light_list->hilite=FALSE;
		
		light_list->nlight=1;
		
		light_list->diffuse.vct.x=(float)mdl->ui.light_offset.x;
		light_list->diffuse.vct.y=(float)mdl->ui.light_offset.y;
		light_list->diffuse.vct.z=(float)mdl->ui.light_offset.z;
		vector_normalize(&light_list->diffuse.vct);
		
		light_list->diffuse.boost=0.0f;
		
		light_list->ui_light.on=TRUE;
		light_list->ui_light.intensity=mdl->ui.light_intensity;
		light_list->ui_light.exponent=mdl->ui.light_exponent;
		light_list->ui_light.pnt.x=draw->pnt.x+mdl->ui.light_offset.x;
		light_list->ui_light.pnt.y=draw->pnt.y+mdl->ui.light_offset.y;
		light_list->ui_light.pnt.z=draw->pnt.z+mdl->ui.light_offset.z;
		memmove(&light_list->ui_light.col,&mdl->ui.light_col,sizeof(d3col));
		
		return;
	}
	
		// regular lighting
		
	light_list->ui_light.on=FALSE;
	
		// misc settings, mostly for models

	light_list->hilite=FALSE;
	gl_lights_calc_diffuse_vector(&draw->pnt,draw->light_cache.count,draw->light_cache.indexes,&light_list->diffuse.vct);
	light_list->diffuse.boost=0.0f;

		// lights

	light_list->nlight=draw->light_cache.count;
	if (light_list->nlight>max_shader_light) light_list->nlight=max_shader_light;
		
	for (n=0;n!=light_list->nlight;n++) {
		light_list->light_idx[n]=draw->light_cache.indexes[n];
	}
}

