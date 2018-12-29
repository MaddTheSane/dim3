/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Model Drawing

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

extern map_type				map;
extern server_type			server;
extern iface_type			iface;
extern view_type			view;
extern setup_type			setup;

/* =======================================================

      Texture and Alpha Utilities
      
======================================================= */

static inline int render_model_get_texture_frame(model_type *mdl,model_draw *draw,int txt_idx)
{
	texture_type		*texture;

		// regular texture animation
		
	texture=&mdl->textures[txt_idx];
	if (texture->animate.on) return(texture->animate.current_frame);
	
		// if script based animation is on, replace
		// frame
		
	if (draw->textures[txt_idx].animation_on) draw->textures[txt_idx].frame=bitmap_texture_get_current_frame(texture,draw->textures[txt_idx].animation_reverse,game_time_get());
	return(draw->textures[txt_idx].frame);
}

static inline float render_model_get_mesh_alpha(model_draw *draw,int mesh_idx)
{
	if (!draw->meshes[mesh_idx].fade.on) return(draw->alpha);
	return(draw->meshes[mesh_idx].fade.alpha);
}

/* =======================================================

      Model Normals
      
======================================================= */

void render_model_create_normal_vertexes(model_type *mdl,int mesh_mask,model_draw *draw)
{
	int				n;

	for (n=0;n!=mdl->nmesh;n++) {
		if ((mesh_mask&(0x1<<n))==0) continue;
		model_create_draw_normals(mdl,n,&draw->setup);
		if (draw->flip_x) model_flip_draw_normals(mdl,n,&draw->setup);
	}
}

/* =======================================================

      Setup Model Vertex Object
      
======================================================= */

void render_model_fill_vertex_objects(model_type *mdl,int mesh_idx,model_draw *draw,unsigned char *vertex_ptr)
{
	int					n,k,offset,stride;
	float				*gx,*gy,*pf,
						*va,*va_start,*ta,*ta_start,*na,*na_start;
	unsigned char		*vp;
	model_mesh_type		*mesh;
	model_poly_type		*poly;
	
	mesh=&mdl->meshes[mesh_idx];
	
	vp=vertex_ptr;

	va_start=draw->setup.mesh_arrays[mesh_idx].gl_vertex_array;
	ta_start=draw->setup.mesh_arrays[mesh_idx].gl_tangent_array;
	na_start=draw->setup.mesh_arrays[mesh_idx].gl_normal_array;

	stride=draw->vbo[mesh_idx].vertex_stride;

	poly=mesh->polys;

	for (n=0;n!=mesh->npoly;n++) {
	
		gx=poly->gx;
		gy=poly->gy;

		for (k=0;k!=poly->ptsz;k++) {
			offset=poly->v[k]*3;
			
			va=va_start+offset;
			ta=ta_start+offset;
			na=na_start+offset;

			pf=(float*)vp;

			*pf++=*va++;
			*pf++=*va++;
			*pf++=*va;

			*pf++=*gx++;
			*pf++=*gy++;

			*pf++=*ta++;
			*pf++=*ta++;
			*pf++=*ta;

			*pf++=*na++;
			*pf++=*na++;
			*pf++=*na;

			vp+=stride;
		}

		poly++;
	}
}

bool render_model_initialize_vertex_objects(model_type *mdl,int mesh_idx,model_draw *draw)
{
	unsigned char		*vertex_ptr;
	model_mesh_type		*mesh;
	
 		// construct VBO
		
	mesh=&mdl->meshes[mesh_idx];

	view_bind_model_vertex_object(draw,mesh_idx);
	vertex_ptr=(unsigned char*)view_map_model_vertex_object();
	if (vertex_ptr==NULL) {
		view_unbind_model_vertex_object();
		return(FALSE);
	}

	render_model_fill_vertex_objects(mdl,mesh_idx,draw,vertex_ptr);
	view_unmap_model_vertex_object();

	gl_shader_draw_execute_reset_cached_offsets();

	return(TRUE);
}

void render_model_release_vertex_objects(void)
{
	view_unbind_model_vertex_object();
}

/* =======================================================

      Transparent Polygon Sorting
      
======================================================= */

float render_model_transparent_poly_closest_z(model_draw *draw,int mesh_idx,model_poly_type *poly)
{
	int				n;
	float			d,dist;
	float			*pv;
	d3fpnt			pnt;

		// calculate the farest z
		// that is on screen

	dist=2.0f;

	for (n=0;n!=poly->ptsz;n++) {
		pv=draw->setup.mesh_arrays[mesh_idx].gl_vertex_array+(poly->v[n]*3);
		pnt.x=*pv++;
		pnt.y=*pv++;
		pnt.z=*pv;

		if (!gl_project_in_view_z_f(&pnt)) continue;
		
		d=gl_project_get_depth_f(&pnt);
		if (d<dist) dist=d;
	}

	return(dist);
}

bool model_draw_transparent_sort(model_type *mdl,model_draw *draw,int mesh_idx)
{
	int					n,k,v_idx,
						sort_cnt,sort_idx;
	float				dist;
	model_mesh_type		*mesh;
    model_poly_type		*poly;

		// create sort list

	mesh=&mdl->meshes[mesh_idx];

	sort_cnt=0;

	v_idx=0;				// need to track vertex offsets as we won't be going through poly list
	poly=mesh->polys;
	
	for (n=0;n!=mesh->npoly;n++) {

			// skip out if opaque

		if (!draw->meshes[mesh_idx].textures[poly->txt_idx].transparent) {
			v_idx+=poly->ptsz;
			poly++;
			continue;
		}

			// find distance from camera

		dist=render_model_transparent_poly_closest_z(draw,mesh_idx,poly);

			// find position in sort list
			// list is sorted from farthest to nearest

		sort_idx=sort_cnt;

		for (k=0;k!=sort_cnt;k++) {
			if (dist<mesh->trans_sort.polys[k].dist) {
				sort_idx=k;
				break;
			}
		}

			// add to sort list

		if (sort_idx<sort_cnt) {
			memmove(&mesh->trans_sort.polys[sort_idx+1],&mesh->trans_sort.polys[sort_idx],((sort_cnt-sort_idx)*sizeof(model_trans_sort_poly_type)));
		}

		mesh->trans_sort.polys[sort_idx].poly_idx=n;
		mesh->trans_sort.polys[sort_idx].v_idx=v_idx;
		mesh->trans_sort.polys[sort_idx].dist=dist;

		sort_cnt++;

		v_idx+=poly->ptsz;
		poly++;
	}

	mesh->trans_sort.count=sort_cnt;

	return(sort_cnt!=0);
}

/* =======================================================

      Draw Model Polys
      
======================================================= */

void render_model_opaque_mesh(model_type *mdl,int mesh_idx,model_draw *draw,view_glsl_light_list_type *light_list)
{
	int						n,v_idx,frame,txt_idx,stride;
	unsigned char			*cull_ptr;
	model_mesh_type			*mesh;
	model_poly_type			*poly;
 	model_draw_mesh_type	*draw_mesh;
	texture_type			*texture;

	mesh=&mdl->meshes[mesh_idx];
	draw_mesh=&draw->meshes[mesh_idx];
	stride=draw->vbo[mesh_idx].vertex_stride;

		// run through the polys

	v_idx=0;
	cull_ptr=draw->setup.mesh_arrays[mesh_idx].poly_cull_array;

	poly=mesh->polys;

	for (n=0;n!=mesh->npoly;n++) {

			// polygon culling
			
		if (*cull_ptr++==0x1) {
			v_idx+=poly->ptsz;
			poly++;
			continue;
		}

			// is this poly texture opaque

		txt_idx=poly->txt_idx;

		if (!draw_mesh->textures[txt_idx].opaque) {
			v_idx+=poly->ptsz;
			poly++;
			continue;
		}

			// set texture

		frame=draw_mesh->textures[txt_idx].frame;
		texture=&mdl->textures[txt_idx];

			// set hilite and tint on per
			// mesh basis
					
		light_list->hilite=(mesh->no_lighting);

			// run the shader
			
		gl_shader_draw_execute_model_start(texture,txt_idx,frame,1.0f,0,(3*sizeof(float)),(5*sizeof(float)),(8*sizeof(float)),stride,light_list);
		glDrawArrays(GL_TRIANGLE_FAN,v_idx,poly->ptsz);
		gl_shader_draw_execute_model_end(texture,light_list);
		
		v_idx+=poly->ptsz;
		poly++;

		view.count.model_poly++;
	}
}

void render_model_transparent_mesh(model_type *mdl,int mesh_idx,model_draw *draw,view_glsl_light_list_type *light_list)
{
	int						n,v_idx,frame,txt_idx,stride;
	bool					cur_additive,is_additive;
	model_mesh_type			*mesh;
	model_poly_type			*poly;
 	model_draw_mesh_type	*draw_mesh;
    texture_type			*texture;
	
	mesh=&mdl->meshes[mesh_idx];
	draw_mesh=&draw->meshes[mesh_idx];
	stride=draw->vbo[mesh_idx].vertex_stride;

		// sort the polygons

	if (!model_draw_transparent_sort(mdl,draw,mesh_idx)) return;
	
		// minimize state changes

	cur_additive=FALSE;

		// run through the polys
		// this sorted list has already eliminated
		// non-transparent polys

		// models are sorted farthest to nearest
		// because they are based on closest point,
		// map segments are the opposite as they are bigger

	for (n=(mesh->trans_sort.count-1);n>=0;n--) {

		poly=&mesh->polys[mesh->trans_sort.polys[n].poly_idx];
	
			// get texture

		txt_idx=poly->txt_idx;
		frame=draw_mesh->textures[txt_idx].frame;
		texture=&mdl->textures[txt_idx];

			// blending changes
			
		is_additive=(mesh->blend_add) || (texture->additive);
		if (is_additive!=cur_additive) {
			cur_additive=is_additive;
			if (cur_additive) {
				glBlendFunc(GL_SRC_ALPHA,GL_ONE);
			}
			else {
				glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
			}
		}

			// set hilite and diffuse on a per
			// mesh basis
		
		light_list->hilite=(mesh->no_lighting);

			// draw poly
			// vertex idx was cached when sorted
			// list was created

		v_idx=mesh->trans_sort.polys[n].v_idx;
			
		gl_shader_draw_execute_model_start(texture,txt_idx,frame,draw_mesh->alpha,0,(3*sizeof(float)),(5*sizeof(float)),(8*sizeof(float)),stride,light_list);
		glDrawArrays(GL_TRIANGLE_FAN,v_idx,poly->ptsz);
		gl_shader_draw_execute_model_end(texture,light_list);
		
		view.count.model_poly++;
	}
	
	if (cur_additive) glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
}

/* =======================================================

      Debug for Testing Normals
      
======================================================= */

void render_model_debug_normals(model_type *mdl,int mesh_idx,model_draw *draw)
{
	int						n,k,idx;
	float					*va,*na;
	d3pnt					pnt1,pnt2;
	d3col					col;
	model_mesh_type			*mesh;
 	model_poly_type			*poly;
	
	glEnable(GL_DEPTH_TEST); 
	
	col.r=1.0f;
	col.g=0.0f;
	col.b=1.0f;

	mesh=&mdl->meshes[mesh_idx];
	poly=mesh->polys;

	for (n=0;n!=mesh->npoly;n++) {
	
		for (k=0;k!=poly->ptsz;k++) {
			idx=poly->v[k]*3;

			va=draw->setup.mesh_arrays[mesh_idx].gl_vertex_array+idx;
			pnt1.x=(int)*va++;
			pnt1.y=(int)*va++;
			pnt1.z=(int)*va;

			na=draw->setup.mesh_arrays[mesh_idx].gl_normal_array+idx;
			pnt2.x=pnt1.x+(int)(100.0f*(*na++));
			pnt2.y=pnt1.y+(int)(100.0f*(*na++));
			pnt2.z=pnt1.z+(int)(100.0f*(*na));

			view_primitive_3D_line(&col,1.0f,pnt1.x,pnt1.y,pnt1.z,pnt2.x,pnt2.y,pnt2.z);
		}

		poly++;
	}
}

/* =======================================================

      Setup Model and Vertex Lists
      
======================================================= */

void render_model_setup(model_draw *draw,int tick)
{
	int					n,k,frame,txt_idx;
	bool				texture_hits[max_model_texture];
	model_type			*mdl;
	model_mesh_type		*mesh;
	model_poly_type		*poly;
    texture_type		*texture;
	
		// get model
	
	if ((draw->model_idx==-1) || (!draw->on)) return;
	
	mdl=server.model_list.models[draw->model_idx];

		// setup animated textures

	model_setup_animated_textures(mdl,tick);

		// setup the rendering mesh mask

	draw->render_mesh_mask=draw->mesh_mask;

	for (n=0;n!=mdl->nmesh;n++) {
		if (draw->meshes[n].fade.on) {
			draw->render_mesh_mask|=(0x1<<n);
		}
	}

		// hasn't build vertex list yet

	draw->built_vertex_list=FALSE;

		// run through the meshes and
		// setup flags

		// we setup glow, opaque, and transparent
		// flag for textures, meshes, and drawing as
		// a whole to quickly eliminate some paths

		// we also setup current texture frames
		// and current mesh alphas (can be changed
		// by fades.)

	draw->has_opaque=FALSE;
	draw->has_transparent=FALSE;
	draw->has_glow=FALSE;
	
	for (n=0;n!=mdl->nmesh;n++) {
		if ((draw->render_mesh_mask&(0x1<<n))==0) continue;
		
		mesh=&mdl->meshes[n];

			// get alpha
		
		draw->meshes[n].alpha=render_model_get_mesh_alpha(draw,n);

			// setup textures

		texture=mdl->textures;
		
		for (k=0;k!=max_model_texture;k++) {
			texture_hits[k]=FALSE;

			frame=render_model_get_texture_frame(mdl,draw,k);
			draw->meshes[n].textures[k].frame=frame;

			draw->meshes[n].textures[k].glow=(texture->frames[frame].glowmap.gl_id!=-1);
			if ((!texture->frames[frame].bitmap.opaque) || (draw->meshes[n].alpha!=1.0f)) {
				draw->meshes[n].textures[k].opaque=FALSE;
				draw->meshes[n].textures[k].transparent=TRUE;
			}
			else {
				draw->meshes[n].textures[k].opaque=TRUE;
				draw->meshes[n].textures[k].transparent=FALSE;
			}

			texture++;
		}
		
			// check for any transparent textures
		
		draw->meshes[n].has_opaque=FALSE;
		draw->meshes[n].has_glow=FALSE;

		if (draw->meshes[n].alpha!=1.0f) {
			draw->meshes[n].has_transparent=TRUE;
			draw->has_transparent=TRUE;
		}
		else {
			draw->meshes[n].has_transparent=FALSE;
		}

		poly=mesh->polys;

		for (k=0;k!=mesh->npoly;k++) {

				// only check once per texture

			txt_idx=poly->txt_idx;
			if (texture_hits[txt_idx]) {
				poly++;
				continue;
			}

			texture_hits[txt_idx]=TRUE;

			texture=&mdl->textures[txt_idx];
			frame=draw->meshes[n].textures[txt_idx].frame;

				// look for glows, opaque, and transparent
			
			if (texture->frames[frame].glowmap.gl_id!=-1) {
				draw->meshes[n].has_glow=TRUE;
				draw->has_glow=TRUE;
			}
	
			if (!texture->frames[frame].bitmap.opaque) {
				draw->meshes[n].has_transparent=TRUE;
				draw->has_transparent=TRUE;
			}
			else {
				draw->meshes[n].has_opaque=TRUE;
				draw->has_opaque=TRUE;
			}
			
			poly++;
		}
	}
}

void render_model_build_vertex_lists(model_draw *draw,bool always_build)
{
	int					n,k,t,idx;
	float				f,*va,*na;
	unsigned char		*cull_ptr;
	d3fpnt				pnt,camera_pnt;
	d3vct				normal;
	model_type			*mdl;
	model_mesh_type		*mesh;
	model_poly_type		*poly;

		// already built list?
		// this happens if we've already draw for
		// the model but need it again for shadows
		// or other rendering

	if ((!always_build) && (draw->built_vertex_list)) return;

	draw->built_vertex_list=TRUE;

		// get model
		
	if ((draw->model_idx==-1) || (!draw->on)) return;

	mdl=server.model_list.models[draw->model_idx];

		// create vertex and uv lists
		
	for (n=0;n!=mdl->nmesh;n++) {
		if ((draw->render_mesh_mask&(0x1<<n))==0) continue;

			// build model vertex list

		model_create_draw_vertexes(mdl,n,&draw->setup);
		if (draw->resize!=1.0f) model_resize_draw_vertex(mdl,n,draw->resize,&draw->setup);
		if (draw->flip_x) model_flip_draw_vertex(mdl,n,&draw->setup);

			// translate vertex to view
			
		model_translate_draw_vertex(mdl,n,draw->pnt.x,draw->pnt.y,draw->pnt.z,&draw->setup);
	}

		// setup the normals

	render_model_create_normal_vertexes(mdl,draw->render_mesh_mask,draw);

		// setup culling

	camera_pnt.x=(float)view.render->camera.pnt.x;
	camera_pnt.y=(float)view.render->camera.pnt.y;
	camera_pnt.z=(float)view.render->camera.pnt.z;

	for (n=0;n!=mdl->nmesh;n++) {
		if ((draw->render_mesh_mask&(0x1<<n))==0) continue;

		mesh=&mdl->meshes[n];

			// no culling
			// initial array is set to 0s so we
			// can just skip out

		if ((mesh->never_cull) || (draw->no_culling)) continue;

			// culled

		poly=mesh->polys;
		cull_ptr=draw->setup.mesh_arrays[n].poly_cull_array;

		for (k=0;k!=mesh->npoly;k++) {

				// get poly normal
				// don't need to normalize or divide
				// out average for normal, but have to
				// for point

			pnt.x=pnt.y=pnt.z=0.0f;
			normal.x=normal.y=normal.z=0.0f;

			for (t=0;t!=poly->ptsz;t++) {
				idx=poly->v[t]*3;

				va=draw->setup.mesh_arrays[n].gl_vertex_array+idx;
				pnt.x+=*va++;
				pnt.y+=*va++;
				pnt.z+=*va;

				na=draw->setup.mesh_arrays[n].gl_normal_array+idx;
				normal.x+=*na++;
				normal.y+=*na++;
				normal.z+=*na;
			}

			f=(float)poly->ptsz;
			pnt.x=(pnt.x/f)-camera_pnt.x;
			pnt.y=(pnt.y/f)-camera_pnt.y;
			pnt.z=(pnt.z/f)-camera_pnt.z;

			*cull_ptr++=(((normal.x*pnt.x)+(normal.y*pnt.y)+(normal.z*pnt.z))>0.0f)?0x1:0x0;

			poly++;
		}
	}
}

/* =======================================================

      Draw Normal Model
      
======================================================= */

void render_model_opaque(model_draw *draw)
{
	int							n;
	model_type					*mdl;
	view_glsl_light_list_type	light_list;
	
		// any opaque?

	if ((draw->model_idx==-1) || (!draw->on)) return;
	if (!draw->has_opaque) return;
	
		// get model

	mdl=server.model_list.models[draw->model_idx];
	
		// setup drawing

	glEnable(GL_DEPTH_TEST); 

	glDisable(GL_BLEND);

		// start glsl lighting

		// if we are drawing this model in the UI,
		// then we have to create a fake set of
		// lights for the rendering
		
	gl_lights_build_model_glsl_light_list(mdl,draw,&light_list);

		// draw opaque materials
		
	for (n=0;n!=mdl->nmesh;n++) {
		if ((draw->render_mesh_mask&(0x1<<n))==0) continue;
		if (!draw->meshes[n].has_opaque) continue;
	
			// create VBO for this mesh

		if (!render_model_initialize_vertex_objects(mdl,n,draw)) return;

			// render opaque mesh

		render_model_opaque_mesh(mdl,n,draw,&light_list);

		render_model_release_vertex_objects();
		
			// debugging for normals
		
	//	render_model_debug_normals(mdl,n,draw);
	}
}

void render_model_transparent(model_draw *draw)
{
	int							n;
	model_type					*mdl;
	view_glsl_light_list_type	light_list;

		// any transparent?

	if ((draw->model_idx==-1) || (!draw->on)) return;
	if (!draw->has_transparent) return;
	
		// get model

	mdl=server.model_list.models[draw->model_idx];

		// setup drawing

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
		
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);
	
		// start lighting
		
	gl_lights_build_model_glsl_light_list(mdl,draw,&light_list);

		// draw transparent materials
		
	for (n=0;n!=mdl->nmesh;n++) {
		if ((draw->render_mesh_mask&(0x1<<n))==0) continue;
		if (!draw->meshes[n].has_transparent) continue;

			// setup the VBO

		if (!render_model_initialize_vertex_objects(mdl,n,draw)) return;	

			// draw transparent mesh

		render_model_transparent_mesh(mdl,n,draw,&light_list);
			
			// release the vbo

		render_model_release_vertex_objects();
	}
	
	glDepthMask(GL_TRUE);
}

/* =======================================================

      Draw Target Model
      
======================================================= */

void render_model_target(model_draw *draw,d3col *col)
{
	int				ty,by,lx,rx,lz,rz,wid,xadd,zadd;
	float			rang;
	float			*vp;
	model_type		*mdl;
	
		// get model
		
	if ((draw->model_idx==-1) || (!draw->on)) return;

		// check distance obscure
		
	if (map.optimize.obscure_dist.model!=0) {
		if (distance_get(draw->pnt.x,draw->pnt.y,draw->pnt.z,view.render->camera.pnt.x,view.render->camera.pnt.y,view.render->camera.pnt.z)>map.optimize.obscure_dist.model) return;
	}

	mdl=server.model_list.models[draw->model_idx];

		// get draw coordinates

	ty=draw->pnt.y-mdl->view_box.size.y;
	by=draw->pnt.y;

	rang=angle_find(view.render->camera.pnt.x,view.render->camera.pnt.z,draw->pnt.x,draw->pnt.z);

	wid=(mdl->view_box.size.x+mdl->view_box.size.z)>>2;

	lx=-wid;
	rx=wid;
	lz=rz=0;

	rotate_2D_point_center(&lx,&lz,rang);
	rotate_2D_point_center(&rx,&rz,rang);

	lx+=draw->pnt.x;
	rx+=draw->pnt.x;
	lz+=draw->pnt.z;
	rz+=draw->pnt.z;

		// move towards camera

	angle_get_movement(rang,wid,&xadd,&zadd);

	lx-=xadd;
	rx-=xadd;
	lz-=zadd;
	rz-=zadd;

		// get the vertexes

	view_bind_utility_vertex_object();
	vp=(float*)view_map_utility_vertex_object();

	*vp++=(float)lx;
	*vp++=(float)ty;
	*vp++=(float)lz;

	*vp++=(float)rx;
	*vp++=(float)ty;
	*vp++=(float)rz;

	*vp++=(float)rx;
	*vp++=(float)by;
	*vp++=(float)rz;

	*vp++=(float)lx;
	*vp++=(float)by;
	*vp++=(float)lz;

	view_unmap_utility_vertex_object();

		// draw target box

	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
		
	gl_shader_draw_execute_simple_color_start(3,0,col,0.1f);
	glDrawArrays(GL_LINE_LOOP,0,4);
	gl_shader_draw_execute_simple_color_end();

	view_unbind_utility_vertex_object();
}
