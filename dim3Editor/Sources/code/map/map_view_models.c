/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Walk View Model Routines

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

extern map_type					map;

extern file_path_setup_type		file_path_setup;

int								nmodel;
model_type						*models;

/* =======================================================

      Start and Close Models
	        
======================================================= */

void map_view_models_start(void)
{
	nmodel=0;
	models=malloc(sizeof(model_type)*max_model);
}

void map_view_models_close(void)
{
	int					n;
	model_type			*model;
	
	if (models!=NULL) {
	
		model=models;
	
		for (n=0;n!=nmodel;n++) {
			model_close(model);
			model++;
		}
		
		free(models);
	}
	
	nmodel=0;
}

/* =======================================================

      Reload Models Set
	        
======================================================= */

int view_models_find_model(char *name)
{
	int			n;
	model_type	*model;
	
	model=models;
	
	for (n=0;n!=nmodel;n++) {
		if (strcmp(model->name,name)==0) return(n);
		model++;
	}
	
	return(-1);
}

void map_view_models_reset(void)
{
	int					n,idx;
	bool				used[max_model];
	spot_type			*spot;
	map_scenery_type	*scenery;
	
	if (models==NULL) return;
	
		// find models loaded but not in spots/scenery
	
	for (n=0;n!=nmodel;n++) {
		used[n]=FALSE;
	}
	
	spot=map.spots;
	
	for (n=0;n!=map.nspot;n++) {
		if (spot->display_model[0]!=0x0) {
			idx=view_models_find_model(spot->display_model);
			if (idx!=-1) used[idx]=TRUE;
		}
		spot++;
	}
	
	scenery=map.sceneries;
	
	for (n=0;n!=map.nscenery;n++) {
		if (scenery->model_name[0]!=0x0) {
			idx=view_models_find_model(scenery->model_name);
			if (idx!=-1) used[idx]=TRUE;
		}
		scenery++;
	}
	
		// remove models loaded but not in spots/scenery
		
	n=0;
	
	while (n<nmodel) {
		if (used[n]) {
			n++;
			continue;
		}
		
		model_close(&models[n]);
	
		if (n<(nmodel-1)) {
			memmove(&models[n],&models[n+1],(sizeof(model_type)*((nmodel-n)-1)));
			memmove(&used[n],&used[n+1],(sizeof(bool)*((nmodel-n)-1)));
		}
		
		nmodel--;
		if (nmodel==0) break;
	}
	
		// add in models in spots/scenery but not loaded
		
	spot=map.spots;
	
	for (n=0;n!=map.nspot;n++) {
	
		if (spot->display_model[0]!=0x0) {
		
			idx=view_models_find_model(spot->display_model);
			
			if ((idx==-1) && (nmodel<max_model)) {
				if (model_open(&models[nmodel],spot->display_model,TRUE)) {
					nmodel++;
				}
			}
		}
		
		spot++;
	}
	
	scenery=map.sceneries;
	
	for (n=0;n!=map.nscenery;n++) {
	
		if (scenery->model_name[0]!=0x0) {
		
			idx=view_models_find_model(scenery->model_name);
			
			if ((idx==-1) && (nmodel<max_model)) {
				if (model_open(&models[nmodel],scenery->model_name,TRUE)) {
					nmodel++;
				}
			}
		}
		
		scenery++;
	}
}

/* =======================================================

      Model Size
      
======================================================= */

void map_view_model_get_size(char *model_name,d3pnt *size)
{
	int					idx;

	size->x=size->y=size->z=view_sprite_size;

	if (model_name[0]==0x0) return;
	
	idx=view_models_find_model(model_name);
	if (idx==-1) return;
	
	memmove(size,&models[idx].view_box.size,sizeof(d3pnt));
}

/* =======================================================

      Transparent Polygon Sorting
      
======================================================= */

float map_view_model_draw_transparent_poly_closest_z(editor_view_type *view,model_draw_setup *draw_setup,int mesh_idx,model_poly_type *poly)
{
	int				n;
	float			d,dist;
	float			*pv;
	d3fpnt			pnt;

		// calculate the farest z
		// that is on screen

	dist=2.0f;

	for (n=0;n!=poly->ptsz;n++) {
		pv=draw_setup->mesh_arrays[mesh_idx].gl_vertex_array+(poly->v[n]*3);
		pnt.x=*pv++;
		pnt.y=*pv++;
		pnt.z=*pv;
		
		d=map_view_project_get_depth_f(view,&pnt);
		if (d<dist) dist=d;
	}

	return(dist);
}

bool map_view_model_draw_transparent_sort(editor_view_type *view,model_type *mdl,model_draw_setup *draw_setup,int mesh_idx)
{
	int					n,k,sort_cnt,sort_idx;
	float				dist;
	model_mesh_type		*mesh;
    model_poly_type		*poly;

		// create sort list

	mesh=&mdl->meshes[mesh_idx];

	sort_cnt=0;
	
	for (n=0;n!=mesh->npoly;n++) {

		poly=&mesh->polys[n];

			// skip out if opaque

		if (mdl->textures[poly->txt_idx].frames[0].bitmap.opaque) continue;

			// find distance from camera

		dist=map_view_model_draw_transparent_poly_closest_z(view,draw_setup,mesh_idx,poly);

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
		mesh->trans_sort.polys[sort_idx].dist=dist;

		sort_cnt++;
	}

	mesh->trans_sort.count=sort_cnt;

	return(sort_cnt!=0);
}

/* =======================================================

      Draw Model
      
======================================================= */

void map_view_model_draw_polys_opaque(model_type *model,model_draw_setup *draw_setup,int *texture_frame,int frame_count)
{
	int					n,k,frame,cur_txt_idx;
	float				vertexes[8*3],uvs[8*2];
	float				*pa,*pv,*pt;
	model_mesh_type		*mesh;
    model_poly_type		*poly;

		// setup drawing
	
	glDisable(GL_BLEND);

	cur_txt_idx=-1;
	
		// model meshes

	mesh=&model->meshes[0];

	for (n=0;n!=mesh->npoly;n++) {
		poly=&mesh->polys[n];

			// opaque or transparent

		if (!model->textures[poly->txt_idx].frames[0].bitmap.opaque) continue;

			// new texture

		if (poly->txt_idx!=cur_txt_idx) {
			cur_txt_idx=poly->txt_idx;

			frame=0;
			if (n<frame_count) frame=texture_frame[n];

			glBindTexture(GL_TEXTURE_2D,model->textures[cur_txt_idx].frames[frame].bitmap.gl_id);
		}

			// draw triangle

		pv=vertexes;
		pt=uvs;

		for (k=0;k!=poly->ptsz;k++) {
			pa=draw_setup->mesh_arrays[0].gl_vertex_array+(poly->v[k]*3);
			*pv++=*pa++;
			*pv++=*pa++;
			*pv++=*pa;
			*pt++=poly->gx[k];
			*pt++=poly->gy[k];
		}

		glVertexPointer(3,GL_FLOAT,0,vertexes);
		glTexCoordPointer(2,GL_FLOAT,0,uvs);

		glDrawArrays(GL_POLYGON,0,poly->ptsz);
	}
}

void map_view_model_draw_polys_transparent(editor_view_type *view,model_type *model,model_draw_setup *draw_setup,int *texture_frame,int frame_count)
{
	int					n,k,frame,cur_txt_idx;
	float				vertexes[8*3],uvs[8*2];
	float				*pa,*pv,*pt;
	model_mesh_type		*mesh;
    model_poly_type		*poly;

		// sort polys

	if (!map_view_model_draw_transparent_sort(view,model,draw_setup,0)) return;

		// setup drawing

	glEnable(GL_BLEND);
	glDepthMask(GL_FALSE);

	cur_txt_idx=-1;
	
		// model meshes
		// sort list has already eliminated any
		// opaque polys

		// sorted list is farthest to nearest,
		// models are sorted this way because it works
		// better for small polygons

	mesh=&model->meshes[0];

	for (n=(mesh->trans_sort.count-1);n>=0;n--) {
		poly=&mesh->polys[mesh->trans_sort.polys[n].poly_idx];

			// new texture

		if (poly->txt_idx!=cur_txt_idx) {
			cur_txt_idx=poly->txt_idx;

			frame=0;
			if (n<frame_count) frame=texture_frame[n];

			glBindTexture(GL_TEXTURE_2D,model->textures[cur_txt_idx].frames[frame].bitmap.gl_id);
		}

			// draw triangle

		pv=vertexes;
		pt=uvs;

		for (k=0;k!=poly->ptsz;k++) {
			pa=draw_setup->mesh_arrays[0].gl_vertex_array+(poly->v[k]*3);
			*pv++=*pa++;
			*pv++=*pa++;
			*pv++=*pa;
			*pt++=poly->gx[k];
			*pt++=poly->gy[k];
		}

		glVertexPointer(3,GL_FLOAT,0,vertexes);
		glTexCoordPointer(2,GL_FLOAT,0,uvs);

		glDrawArrays(GL_POLYGON,0,poly->ptsz);
	}

		// restore state

	glDepthMask(GL_TRUE);
}

bool map_view_model_draw(editor_view_type *view,d3pnt *pnt,d3ang *ang,char *name,float resize,int *texture_frame,int frame_count)
{
	int								idx;
	model_type						*model;
	model_draw_setup				draw_setup;

	if (name[0]==0x0) return(FALSE);
	
	idx=view_models_find_model(name);
	if (idx==-1) return(FALSE);
	
	model=&models[idx];

		// build model vertex list
		
	model_draw_setup_initialize(model,&draw_setup,FALSE);
	model_draw_setup_clear(model,&draw_setup);
	
	draw_setup.poses[0].idx_1=draw_setup.poses[0].idx_2=-1;
    draw_setup.poses[0].factor=0;
	draw_setup.poses[0].acceleration=0;
	
	memmove(&draw_setup.ang,ang,sizeof(d3ang));
	draw_setup.ang.y=ang->y;
	
	model_create_draw_bones(model,&draw_setup);
	model_create_draw_vertexes(model,0,&draw_setup);
	if (resize!=1.0f) model_resize_draw_vertex(model,0,resize,&draw_setup);

	model_translate_draw_vertex(model,0,pnt->x,pnt->y,pnt->z,&draw_setup);

		// draw triangles
	
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);

	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glColor4f(1.0f,1.0f,1.0f,1.0f);
    
	map_view_model_draw_polys_opaque(model,&draw_setup,texture_frame,frame_count);
	map_view_model_draw_polys_transparent(view,model,&draw_setup,texture_frame,frame_count);

	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	
	glDisable(GL_TEXTURE_2D);
	
	model_draw_setup_shutdown(model,&draw_setup);
	
	return(TRUE);
}

/* =======================================================

      Get Model Click Size
      
======================================================= */

void map_view_model_cube_vertexes(char *name,d3pnt *pnt,d3ang *ang,float resize,d3pnt *v_pnts)
{
	int						n,idx,cx,cy,cz,wid_x,wid_z,high,ty,by,
							px[8],py[8],pz[8];
	float					fx,fy,fz;
	matrix_type				mat;
	model_type				*model;
	
		// default size
		
    wid_x=wid_z=view_sprite_size;
	high=view_sprite_size*2;
	
	cx=cy=cz=0;

		// if there is a model, then get size

	if (name!=NULL) {
		if (name[0]!=0x0) {
	
			idx=view_models_find_model(name);
			if (idx!=-1) {
	
				model=&models[idx];
		
				wid_x=model->view_box.size.x>>1;
				wid_z=model->view_box.size.z>>1;
				high=model->view_box.size.y;
				
				cx=model->center.x;
				cy=model->center.y;
				cz=model->center.z;
			}
		}
	}
	
		// resize
		
	ty=-high;
	by=0;
	
	if (resize!=1.0f) {
		wid_x=(int)(((float)wid_x)*resize);
		wid_z=(int)(((float)wid_z)*resize);
		
		ty=((int)(((float)(ty+(high>>1)))*resize))-(high>>1);
		by=((int)(((float)(by+(high>>1)))*resize))-(high>>1);
	}

		// get polygons
	
	px[0]=px[3]=px[4]=px[7]=-wid_x;
	px[1]=px[2]=px[5]=px[6]=wid_x;
	
	py[0]=py[1]=py[2]=py[3]=ty;
	py[4]=py[5]=py[6]=py[7]=by;
		
	pz[0]=pz[1]=pz[4]=pz[5]=-wid_z;
	pz[2]=pz[3]=pz[6]=pz[7]=wid_z;
	
		// any rotations
	
	if (ang!=NULL) {
		matrix_rotate_xyz(&mat,ang->x,ang->y,ang->z);
		
		for (n=0;n!=8;n++) {
			fx=(float)(px[n]-cx);
			fy=(float)(py[n]-cy);
			fz=(float)(pz[n]-cz);
			
			matrix_vertex_multiply(&mat,&fx,&fy,&fz);
			
			px[n]=((int)fx)+cx;
			py[n]=((int)fy)+cy;
			pz[n]=((int)fz)+cz;
		}
	}
	
	for (n=0;n!=8;n++) {
		v_pnts[n].x=px[n]+pnt->x;
		v_pnts[n].y=py[n]+pnt->y;
		v_pnts[n].z=pz[n]+pnt->z;
	}
}

