/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Map View Mesh Drawing Routines

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

extern map_type				map;
extern app_state_type		state;
extern app_pref_type		pref;
extern bitmap_type			spot_bitmap,scenery_bitmap,node_bitmap,node_defined_bitmap,
							light_bitmap,sound_bitmap,particle_bitmap;

view_mesh_sort_type			view_mesh_sort;
map_poly_sort_type			view_poly_trans_sort;

/* =======================================================

      Mesh & Poly Culling and Clipping
      
======================================================= */

bool view_cull_poly(editor_view_type *view,map_mesh_type *mesh,map_mesh_poly_type *poly,int mn,int mk)
{
	d3vct		face_vct;
	
	if (!view->cull) return(FALSE);
	if (mesh->flag.never_cull) return(FALSE);
	if (poly->ptsz==0) return(FALSE);
	if (poly->flag.never_cull) return(FALSE);
	
		// zero normals are always culled
		
	if ((poly->tangent_space.normal.x==0.0f) && (poly->tangent_space.normal.y==0.0f) && (poly->tangent_space.normal.z==0.0f)) return(TRUE);
	
		// is normal facing away?
		
	vector_create(&face_vct,poly->box.mid.x,poly->box.mid.y,poly->box.mid.z,view->pnt.x,view->pnt.y,view->pnt.z);
	return(vector_dot_product(&poly->tangent_space.normal,&face_vct)>map.optimize.cull_angle);
}

bool view_clip_poly(editor_view_type *view,map_mesh_poly_type *poly)
{
	int			dist;

	if (!view->clip) return(FALSE);
	
		// if top or bottom, only clip Y
		
	if ((view->ang.x<=270.0f) && (view->ang.x>=45.0f)) {
		dist=abs(view->pnt.y-poly->box.mid.y);
	}
	else {
		dist=distance_2D_get(view->pnt.x,view->pnt.z,poly->box.mid.x,poly->box.mid.z);
	}
	
		// get distance
		
	return(dist<(pref.clip_distance*view_snap_clip_size_factor));
}

bool view_clip_liquid(editor_view_type *view,map_liquid_type *liq)
{
	int			x,z,dist;

	if (!view->clip) return(FALSE);
	
		// if top or bottom, only clip Y
		
	if ((view->ang.x<=270.0f) && (view->ang.x>=45.0f)) {
		dist=abs(view->pnt.y-liq->y);
	}
	else {
		x=(liq->lft+liq->rgt)>>1;
		z=(liq->top+liq->bot)>>1;
		dist=distance_2D_get(view->pnt.x,view->pnt.z,x,z);
	}
	
		// get distance
		
	return(dist<(pref.clip_distance*view_snap_clip_size_factor));
}

bool view_clip_point(editor_view_type *view,d3pnt *pnt)
{
	int			dist;

	if (!view->clip) return(FALSE);
	
		// if top or bottom, only clip Y
		
	if ((view->ang.x<=270.0f) && (view->ang.x>=45.0f)) {
		dist=abs(view->pnt.y-pnt->y);
	}
	else {
		dist=distance_2D_get(view->pnt.x,view->pnt.z,pnt->x,pnt->z);
	}
	
		// get distance
		
	return(dist<(pref.clip_distance*view_snap_clip_size_factor));
}

/* =======================================================

      View Mesh Draw List
      
======================================================= */

void view_draw_create_mesh_sort_list(editor_view_type *view)
{
	int					n,k,sz,idx,
						dist;
	d3pnt				min,max;
	map_mesh_type		*mesh;

		// sort it

	view_mesh_sort.count=0;

	for (n=0;n!=map.mesh.nmesh;n++) {

			// clip meshes to view
			
		map_mesh_calculate_extent(&map,n,&min,&max);
		if (!view_cull_boundbox_in_frustum(&min,&max)) continue;
			
			// get sorting distance

		mesh=&map.mesh.meshes[n];
		dist=map_mesh_calculate_distance(mesh,&view->pnt);

			// add to list, near ones first

		idx=-1;

		for (k=0;k!=view_mesh_sort.count;k++) {
			if (view_mesh_sort.meshes[k].dist>dist) {
				idx=k;
				break;
			}
		}

			// insert at end of list
			
		if (idx==-1) {
			view_mesh_sort.meshes[view_mesh_sort.count].mesh_idx=n;
			view_mesh_sort.meshes[view_mesh_sort.count].dist=dist;
		}
		
			// insert in list
		
		else {
			sz=sizeof(view_mesh_sort_mesh_type)*(view_mesh_sort.count-idx);
			memmove(&view_mesh_sort.meshes[idx+1],&view_mesh_sort.meshes[idx],sz);
			
			view_mesh_sort.meshes[idx].mesh_idx=n;
			view_mesh_sort.meshes[idx].dist=dist;
		}

			// add up list

		view_mesh_sort.count++;
		if (view_mesh_sort.count==view_mesh_sort_max_mesh) break;
	}
}


/* =======================================================

      Transparent Polygon Sorting
      
======================================================= */

float view_draw_meshes_transparent_poly_farthest_z(editor_view_type *view,map_mesh_type *mesh,map_mesh_poly_type *poly)
{
	int				n;
	float			d,dist;
	d3pnt			*pt;

		// calculate the farthest z
		// that is on screen

	dist=0.0f;

	for (n=0;n!=poly->ptsz;n++) {

		pt=&mesh->vertexes[poly->v[n]];

		if (!map_view_project_point_in_z(pt)) continue;
		
		d=map_view_project_get_depth(view,pt);
		if (d>dist) dist=d;
	}

	return(dist);
}

int view_draw_meshes_transparent_sort(editor_view_type *view)
{
	int							n,k,i,mesh_idx,
								sort_cnt,sort_idx;
	float						dist;
	map_mesh_type				*mesh;
	map_mesh_poly_type			*poly;
	map_poly_sort_poly_type		*sort_list;
	texture_type				*texture;

	sort_list=view_poly_trans_sort.polys;

		// create sort list

	sort_cnt=0;
	
	for (n=0;n!=view_mesh_sort.count;n++) {

		mesh_idx=view_mesh_sort.meshes[n].mesh_idx;
		mesh=&map.mesh.meshes[mesh_idx];

			// skip any meshes that don't have
			// light maps if on light maps
			
		if ((view->uv_layer==uv_layer_light_map) && (mesh->flag.no_light_map)) continue;
		
			// sort the polygons

		for (k=0;k!=mesh->npoly;k++) {
		
			poly=&mesh->polys[k];

				// check if opaque
				
			texture=&map.textures[poly->txt_idx];
			if (texture->frames[0].bitmap.opaque) continue;
		
				// check for clipping
			
			if (view_clip_poly(view,poly)) continue;
			
				// find distance from camera

			dist=view_draw_meshes_transparent_poly_farthest_z(view,mesh,poly);

				// find position in sort list
				// this list is farthest to nearest

			sort_idx=sort_cnt;

			for (i=0;i!=sort_cnt;i++) {
				if (dist>sort_list[i].dist) {
					sort_idx=i;
					break;
				}
			}

				// add to sort list

			if (sort_idx<sort_cnt) {
				memmove(&sort_list[sort_idx+1],&sort_list[sort_idx],((sort_cnt-sort_idx)*sizeof(map_poly_sort_poly_type)));
			}

			sort_list[sort_idx].mesh_idx=mesh_idx;
			sort_list[sort_idx].poly_idx=k;
			sort_list[sort_idx].dist=dist;

			sort_cnt++;
			if (sort_cnt>=max_sort_poly) break;
		}

		if (sort_cnt>=max_sort_poly) break;
	}

	view_poly_trans_sort.count=sort_cnt;

	return(sort_cnt!=0);
}

/* =======================================================

      Draw Meshes
      
======================================================= */

void view_draw_meshes_opaque(editor_view_type *view)
{
	int						n,k,mesh_idx;
	GLuint					old_gl_id;
	bool					culled;
	map_mesh_type			*mesh;
	map_mesh_poly_type		*poly;
	texture_type			*texture;
							
		// setup drawing

	glEnable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
	
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);

	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glColor4f(1.0f,1.0f,1.0f,1.0f);
	
	old_gl_id=-1;

		// draw meshes
		
	for (n=0;n!=view_mesh_sort.count;n++) {

		mesh_idx=view_mesh_sort.meshes[n].mesh_idx;
		mesh=&map.mesh.meshes[mesh_idx];
	
			// skip any meshes that don't have
			// light maps if on light maps
			
		if ((view->uv_layer==uv_layer_light_map) && (mesh->flag.no_light_map)) continue;

			// mesh VBO

		glBindBuffer(GL_ARRAY_BUFFER,mesh->vbo.vertex);
		glVertexPointer(3,GL_FLOAT,((3+2+2)*sizeof(float)),(GLvoid*)0);

		if (view->uv_layer==uv_layer_normal) {
			glTexCoordPointer(2,GL_FLOAT,((3+2+2)*sizeof(float)),(GLvoid*)(3*sizeof(float)));
		}
		else {
			glTexCoordPointer(2,GL_FLOAT,((3+2+2)*sizeof(float)),(GLvoid*)((3+2)*sizeof(float)));
		}

			// draw polys
	
		for (k=0;k!=mesh->npoly;k++) {
		
			poly=&mesh->polys[k];
			
				// check if opaque
				
			texture=&map.textures[poly->txt_idx];
			if (!texture->frames[0].bitmap.opaque) continue;
			
				// need to switch to light map?
				
			if (view->uv_layer==uv_layer_light_map) {
				if (poly->lmap_txt_idx==-1) continue;
				texture=&map.textures[poly->lmap_txt_idx];
			}
			
				// check for clipping
				
			if (view_clip_poly(view,poly)) continue;
			
				// culling
			
			culled=view_cull_poly(view,mesh,poly,mesh_idx,k);
		
				// setup texture
				
			if (!culled) {
				if (texture->frames[0].bitmap.gl_id!=old_gl_id) {
					old_gl_id=texture->frames[0].bitmap.gl_id;
					glBindTexture(GL_TEXTURE_2D,old_gl_id);
				}

				if ((poly->flag.obscuring) && (view->cull)) glColor4f(0.0f,1.0f,0.0f,1.0f);		// obscured polygons are tinted in green
			}
			else {
				glDisable(GL_TEXTURE_2D);
				glColor4f(0.9f,0.9f,0.9f,1.0f);
			}
		
				// draw polygon

			glDrawArrays(GL_POLYGON,poly->draw.vertex_offset,poly->ptsz);

				// if culled, turn back on texture
				
			if (culled) {
				glEnable(GL_TEXTURE_2D);
				glColor4f(1.0f,1.0f,1.0f,1.0f);
			}
			else {
				if ((poly->flag.obscuring) && (view->cull)) glColor4f(1.0f,1.0f,1.0f,1.0f);
			}
		}
	}

		// unbind any VBO

	glBindBuffer(GL_ARRAY_BUFFER,0);

		// clean up state

	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	
	glEnable(GL_BLEND);
	glDisable(GL_TEXTURE_2D);
}

void view_draw_meshes_transparent(editor_view_type *view)
{
	int						n,mesh_idx,poly_idx;
	GLuint					old_gl_id;
	bool					culled;
	map_mesh_type			*mesh;
	map_mesh_poly_type		*poly;
	texture_type			*texture;

		// sort transparent polys

	if (!view_draw_meshes_transparent_sort(view)) return;
							
		// setup drawing

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);

	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	
	glColor4f(1.0f,1.0f,1.0f,1.0f);
	
	old_gl_id=-1;

		// draw meshes
		// poly sort list is farthest to
		// nearest

		// this list already removes non-opaque
		// and hidden polys
	
	for (n=0;n!=view_poly_trans_sort.count;n++) {
//	for (n=(view_poly_trans_sort.count-1);n>=0;n--) {

		mesh_idx=view_poly_trans_sort.polys[n].mesh_idx;
		poly_idx=view_poly_trans_sort.polys[n].poly_idx;

		mesh=&map.mesh.meshes[mesh_idx];
		poly=&mesh->polys[poly_idx];

			// get texture

		texture=&map.textures[poly->txt_idx];
			
		if (view->uv_layer==uv_layer_light_map) {
			if (poly->lmap_txt_idx==-1) continue;
			texture=&map.textures[poly->lmap_txt_idx];
		}

			// mesh VBO

		glBindBuffer(GL_ARRAY_BUFFER,mesh->vbo.vertex);
		glVertexPointer(3,GL_FLOAT,((3+2+2)*sizeof(float)),(GLvoid*)0);

		if (view->uv_layer==uv_layer_normal) {
			glTexCoordPointer(2,GL_FLOAT,((3+2+2)*sizeof(float)),(GLvoid*)(3*sizeof(float)));
		}
		else {
			glTexCoordPointer(2,GL_FLOAT,((3+2+2)*sizeof(float)),(GLvoid*)((3+2)*sizeof(float)));
		}
		
			// culling
		
		culled=view_cull_poly(view,mesh,poly,mesh_idx,poly_idx);
	
			// setup texture
			
		if (!culled) {
			if (texture->frames[0].bitmap.gl_id!=old_gl_id) {
				old_gl_id=texture->frames[0].bitmap.gl_id;
				glBindTexture(GL_TEXTURE_2D,old_gl_id);
			}

			if ((poly->flag.obscuring) && (view->cull)) glColor4f(0.0f,1.0f,0.0f,1.0f);		// obscured polygons are tinted in green
		}
		else {
			glDisable(GL_TEXTURE_2D);
			glColor4f(0.9f,0.9f,0.9f,1.0f);
		}
	
			// draw polygon

		glDrawArrays(GL_POLYGON,poly->draw.vertex_offset,poly->ptsz);

			// if culled, turn back on texture
			
		if (culled) {
			glEnable(GL_TEXTURE_2D);
			glColor4f(1.0f,1.0f,1.0f,1.0f);
		}
		else {
			if ((poly->flag.obscuring) && (view->cull)) glColor4f(1.0f,1.0f,1.0f,1.0f);
		}
	}

		// unbind any VBO

	glBindBuffer(GL_ARRAY_BUFFER,0);

		// clean up state

	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDepthMask(GL_TRUE);

	glDisable(GL_TEXTURE_2D);
}

/* =======================================================

      Draw Liquids
      
======================================================= */

void view_draw_liquids(editor_view_type *view,bool opaque)
{
	int					n,nliquid,y,y2,lx,rx,tz,bz;
	float				vertexes[8*3],uvs[8*2];
	float				*pv,*pt;
	GLuint				old_gl_id;
	texture_type		*texture;
	map_liquid_type		*liquid;
	map_liquid_uv_type	*uv;
	
	if (!state.map.show_liquid) return;
	
		// no depth buffer for transparent segments
		
	if (opaque) {
		glDisable(GL_BLEND);
	}
	else {
		glEnable(GL_BLEND);
	}
	
	glEnable(GL_DEPTH_TEST);
	if (opaque) {
		glDepthMask(GL_TRUE);
	}
	else {
		glDepthMask(GL_FALSE);
	}
	
	old_gl_id=-1;

		// run through the liquids
		
	nliquid=map.liquid.nliquid;
	
	for (n=0;n!=nliquid;n++) {
		liquid=&map.liquid.liquids[n];
		
				// clipping
				
		if (view_clip_liquid(view,liquid)) continue;

			// no light map?
				
		if ((view->uv_layer==uv_layer_light_map) && (liquid->lmap_txt_idx==-1)) continue;
			
			// get texture.  If in second UV, we use light map
			// texture for display if it exists
			
		if (view->uv_layer==uv_layer_normal) {
			texture=&map.textures[liquid->txt_idx];
			uv=&liquid->main_uv;
		}
		else {
			texture=&map.textures[liquid->lmap_txt_idx];
			uv=&liquid->lmap_uv;
		}
	
		if (opaque!=texture->frames[0].bitmap.opaque) continue;
		
		if (texture->frames[0].bitmap.gl_id!=old_gl_id) {
			old_gl_id=texture->frames[0].bitmap.gl_id;
			glBindTexture(GL_TEXTURE_2D,old_gl_id);
		}
		
			// dimensions
			
		y=liquid->y;
		y2=liquid->y+liquid->depth;
		
		lx=liquid->lft;
		rx=liquid->rgt;
		tz=liquid->top;
		bz=liquid->bot;

		glColor4f(1.0f,1.0f,1.0f,1.0f);
		
		pv=vertexes;
		pt=uvs;
		
			// bottom
			
		*pv++=(float)lx;
		*pv++=(float)y2;
		*pv++=(float)tz;
		*pt++=uv->offset.x;
		*pt++=uv->offset.y;

		*pv++=(float)rx;
		*pv++=(float)y2;
		*pv++=(float)tz;
		*pt++=(uv->offset.x+uv->size.x);
		*pt++=uv->offset.y;

		*pv++=(float)rx;
		*pv++=(float)y2;
		*pv++=(float)bz;
		*pt++=(uv->offset.x+uv->size.x);
		*pt++=(uv->offset.y+uv->size.y);

		*pv++=(float)lx;
		*pv++=(float)y2;
		*pv++=(float)bz;
		*pt++=uv->offset.x;
		*pt++=(uv->offset.y+uv->size.y);

			// top
			
		*pv++=(float)lx;
		*pv++=(float)y;
		*pv++=(float)tz;
		*pt++=uv->offset.x;
		*pt++=uv->offset.y;

		*pv++=(float)rx;
		*pv++=(float)y;
		*pv++=(float)tz;
		*pt++=(uv->offset.x+uv->size.x);
		*pt++=uv->offset.y;

		*pv++=(float)rx;
		*pv++=(float)y;
		*pv++=(float)bz;
		*pt++=(uv->offset.x+uv->size.x);
		*pt++=(uv->offset.y+uv->size.y);

		*pv++=(float)lx;
		*pv++=(float)y;
		*pv++=(float)bz;
		*pt++=uv->offset.x;
		*pt++=(uv->offset.y+uv->size.y);

		glEnable(GL_TEXTURE_2D);
		glColor4f(1.0f,1.0f,1.0f,1.0f);

		glVertexPointer(3,GL_FLOAT,0,vertexes);

		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glTexCoordPointer(2,GL_FLOAT,0,uvs);

		glDrawArrays(GL_QUADS,0,8);

		glDisableClientState(GL_TEXTURE_COORD_ARRAY);

		glDisable(GL_TEXTURE_2D);
		
			// depth lines
	
		glColor4f(pref.col.mesh_line.r,pref.col.mesh_line.g,pref.col.mesh_line.b,1.0f);

		pv=vertexes;
		
		*pv++=(float)liquid->lft;
		*pv++=(float)y;
		*pv++=(float)liquid->top;

		*pv++=(float)liquid->lft;
		*pv++=(float)y2;
		*pv++=(float)liquid->top;

		*pv++=(float)liquid->rgt;
		*pv++=(float)y;
		*pv++=(float)liquid->top;

		*pv++=(float)liquid->rgt;
		*pv++=(float)y2;
		*pv++=(float)liquid->top;

		*pv++=(float)liquid->lft;
		*pv++=(float)y;
		*pv++=(float)liquid->bot;

		*pv++=(float)liquid->lft;
		*pv++=(float)y2;
		*pv++=(float)liquid->bot;

		*pv++=(float)liquid->rgt;
		*pv++=(float)y;
		*pv++=(float)liquid->bot;

		*pv++=(float)liquid->rgt;
		*pv++=(float)y2;
		*pv++=(float)liquid->bot;

		glVertexPointer(3,GL_FLOAT,0,vertexes);
		glDrawArrays(GL_LINES,0,8);
	}
	
	if (opaque) glEnable(GL_BLEND);
	if (!opaque) glDepthMask(GL_TRUE);
}

/* =======================================================

      Draw Mesh Lines and Normals
      
======================================================= */

void view_draw_meshes_line(editor_view_type *view,bool opaque)
{
	int					n,k,mesh_idx;
	map_mesh_type		*mesh;
	map_mesh_poly_type	*poly;
	texture_type		*texture;

	glDisable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);

	glDepthMask(GL_FALSE);
	
	glColor4f(pref.col.mesh_line.r,pref.col.mesh_line.g,pref.col.mesh_line.b,1.0f);

		// draw portal mesh lines

	for (n=0;n!=view_mesh_sort.count;n++) {

		mesh_idx=view_mesh_sort.meshes[n].mesh_idx;
		mesh=&map.mesh.meshes[mesh_idx];

			// mesh VBO

		glBindBuffer(GL_ARRAY_BUFFER,mesh->vbo.vertex);
		glVertexPointer(3,GL_FLOAT,((3+2+2)*sizeof(float)),(GLvoid*)0);

			// draw polys
	
		for (k=0;k!=mesh->npoly;k++) {

			poly=&mesh->polys[k];
			
				// opaque or transparent flag

			texture=&map.textures[poly->txt_idx];
			if (opaque!=texture->frames[0].bitmap.opaque) continue;
			
				// check for clipping
				
			if (view_clip_poly(view,poly)) continue;

				// draw mesh line

			glDrawArrays(GL_LINE_LOOP,poly->draw.vertex_offset,poly->ptsz);
		}
	}

	glDepthMask(GL_TRUE);

		// unbind any VBO

	glBindBuffer(GL_ARRAY_BUFFER,0);
}

void view_draw_meshes_normals(editor_view_type *view)
{
	int					n,k,dist;
	float				p_sz,vertexes[2*3];
	d3vct				binormal;
	map_mesh_type		*mesh;
	map_mesh_poly_type	*poly;

	glDisable(GL_TEXTURE_2D);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glLineWidth(2.0f);
	
	mesh=map.mesh.meshes;
	
	for (n=0;n!=map.mesh.nmesh;n++) {
		
			// draw polys
	
		for (k=0;k!=mesh->npoly;k++) {
		
			poly=&mesh->polys[k];
			
			if (pref.show_tangent_binormal) {
			
					// draw the tangent

				vertexes[0]=(float)poly->box.mid.x;
				vertexes[1]=(float)poly->box.mid.y;
				vertexes[2]=(float)poly->box.mid.z;
				vertexes[3]=vertexes[0]+(poly->tangent_space.tangent.x*normal_vector_scale);
				vertexes[4]=vertexes[1]+(poly->tangent_space.tangent.y*normal_vector_scale);
				vertexes[5]=vertexes[2]+(poly->tangent_space.tangent.z*normal_vector_scale);

				glVertexPointer(3,GL_FLOAT,0,vertexes);

				glColor4f(1.0f,0.0f,0.0f,1.0f);
				glDrawArrays(GL_LINES,0,2);
				
					// draw the binormal

				vector_cross_product(&binormal,&poly->tangent_space.tangent,&poly->tangent_space.normal);

				vertexes[0]=(float)poly->box.mid.x;
				vertexes[1]=(float)poly->box.mid.y;
				vertexes[2]=(float)poly->box.mid.z;
				vertexes[3]=vertexes[0]+(binormal.x*normal_vector_scale);
				vertexes[4]=vertexes[1]+(binormal.y*normal_vector_scale);
				vertexes[5]=vertexes[2]+(binormal.z*normal_vector_scale);

				glVertexPointer(3,GL_FLOAT,0,vertexes);

				glColor4f(0.0f,0.0f,1.0f,1.0f);
				glDrawArrays(GL_LINES,0,2);
			}

				// draw normal

			vertexes[0]=(float)poly->box.mid.x;
			vertexes[1]=(float)poly->box.mid.y;
			vertexes[2]=(float)poly->box.mid.z;
			vertexes[3]=vertexes[0]+(poly->tangent_space.normal.x*normal_vector_scale);
			vertexes[4]=vertexes[1]+(poly->tangent_space.normal.y*normal_vector_scale);
			vertexes[5]=vertexes[2]+(poly->tangent_space.normal.z*normal_vector_scale);

			glVertexPointer(3,GL_FLOAT,0,vertexes);

			glColor4f(1.0f,0.0f,1.0f,1.0f);
			glDrawArrays(GL_LINES,0,2);

				// draw the center

			dist=distance_get(view->pnt.x,view->pnt.y,view->pnt.z,poly->box.mid.x,poly->box.mid.y,poly->box.mid.z);

			p_sz=10.0f-(((float)dist)*0.0001f);
			if (p_sz>2.0f) {
			
				glPointSize(p_sz);
				
				vertexes[0]=(float)poly->box.mid.x;
				vertexes[1]=(float)poly->box.mid.y;
				vertexes[2]=(float)poly->box.mid.z;

				glVertexPointer(3,GL_FLOAT,0,vertexes);

				glColor4f(0.6f,0.0f,0.6f,1.0f);
				glDrawArrays(GL_POINTS,0,1);
			}
		}
	
		mesh++;
	}
	
	glLineWidth(1.0f);
	glPointSize(1.0f);
}

