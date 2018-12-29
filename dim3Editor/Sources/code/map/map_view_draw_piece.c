/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Map View Piece Drawing

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

/* =======================================================

      View Sprites Drawing
      
======================================================= */

void view_get_sprite_vertexes(d3pnt *pnt,d3ang *ang,d3pnt *v_pnts)
{
    int				n;
	float			fx[8],fy[8],fz[8];
	matrix_type		mat;
	
	fx[0]=fx[3]=fx[4]=fx[7]=-(float)view_sprite_size;
	fx[1]=fx[2]=fx[5]=fx[6]=(float)view_sprite_size;
	
	fy[0]=fy[1]=fy[2]=fy[3]=-(float)(view_sprite_size*2);
	fy[4]=fy[5]=fy[6]=fy[7]=0.0f;
	
	fz[0]=fz[1]=fz[4]=fz[5]=-(float)view_sprite_size;
	fz[2]=fz[3]=fz[6]=fz[7]=(float)view_sprite_size;
	
	if (ang!=NULL) matrix_rotate_xyz(&mat,ang->x,ang->y,ang->z);
		
	for (n=0;n!=8;n++) {
		if (ang!=NULL) matrix_vertex_multiply(&mat,&fx[n],&fy[n],&fz[n]);
		
		v_pnts[n].x=pnt->x+(int)fx[n];
		v_pnts[n].y=pnt->y+(int)fy[n];
		v_pnts[n].z=pnt->z+(int)fz[n];
	}
}

void view_draw_sprite(d3pnt *pnt,d3ang *ang,unsigned long gl_id)
{
	int				n,idx,face_idx[24]={0,1,2,3,4,5,6,7,0,1,5,4,3,2,6,7,1,2,6,5,0,3,7,4};
	float			vertexes[24*3],uvs[24*2];
	float			*pv,*pt;
	d3pnt			v_pnts[8];
	
	view_get_sprite_vertexes(pnt,ang,v_pnts);
	
		// draw sprite

	glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,gl_id);
   
	glColor4f(1.0f,1.0f,1.0f,1.0f);
	
	pv=vertexes;
	pt=uvs;
	
	for (n=0;n!=24;n+=4) {
	
		idx=face_idx[n];
		*pv++=(float)v_pnts[idx].x;
		*pv++=(float)v_pnts[idx].y;
		*pv++=(float)v_pnts[idx].z;
		*pt++=0.0f;
		*pt++=0.0f;
		
		idx=face_idx[n+1];
		*pv++=(float)v_pnts[idx].x;
		*pv++=(float)v_pnts[idx].y;
		*pv++=(float)v_pnts[idx].z;
		*pt++=1.0f;
		*pt++=0.0f;
	
		idx=face_idx[n+2];
		*pv++=(float)v_pnts[idx].x;
		*pv++=(float)v_pnts[idx].y;
		*pv++=(float)v_pnts[idx].z;
		*pt++=1.0f;
		*pt++=1.0f;
		
		idx=face_idx[n+3];
		*pv++=(float)v_pnts[idx].x;
		*pv++=(float)v_pnts[idx].y;
		*pv++=(float)v_pnts[idx].z;
		*pt++=0.0f;
		*pt++=1.0f;
	}
	
	glVertexPointer(3,GL_FLOAT,0,vertexes);

	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2,GL_FLOAT,0,uvs);

	glDrawArrays(GL_QUADS,0,24);

	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	
	glDisable(GL_TEXTURE_2D);
}

void view_draw_circle(d3pnt *pnt,d3ang *ang,d3col *col,int dist,bool checkpoint)
{
    int				n,kx,ky,kz;
	float			vertexes[36*3];
	float			*pv;

	glLineWidth(4.0f);
	if (col==NULL) {
		glColor4f(0.25f,0.25f,1.0f,1.0f);
	}
	else {
		glColor4f(col->r,col->g,col->b,0.5f);
	}

		// circle

	if (!checkpoint) {
		pv=vertexes;
		
		for (n=0;n!=360;n+=10) {
			ky=dist;
			kz=0;
			rotate_2D_point_center(&ky,&kz,(float)n);
			*pv++=(float)pnt->x;
			*pv++=(float)(pnt->y+ky);
			*pv++=(float)(pnt->z+kz);
		}
		
		glVertexPointer(3,GL_FLOAT,0,vertexes);
		glDrawArrays(GL_LINE_LOOP,0,36);
	}

	pv=vertexes;
	
	for (n=0;n!=360;n+=10) {
		kx=dist;
		kz=0;
		rotate_2D_point_center(&kx,&kz,(float)n);
		*pv++=(float)(pnt->x+kx);
		*pv++=(float)pnt->y;
		*pv++=(float)(pnt->z+kz);
	}
	
	glVertexPointer(3,GL_FLOAT,0,vertexes);
	glDrawArrays(GL_LINE_LOOP,0,36);

	if (!checkpoint) {
		pv=vertexes;
		
		for (n=0;n!=360;n+=10) {
			kx=dist;
			ky=0;
			rotate_2D_point_center(&kx,&ky,(float)n);
			*pv++=(float)(pnt->x+kx);
			*pv++=(float)(pnt->y+ky);
			*pv++=(float)pnt->z;
		}

		glVertexPointer(3,GL_FLOAT,0,vertexes);
		glDrawArrays(GL_LINE_LOOP,0,36);
	}

		// checkpoint view line

	if (checkpoint) {
		pv=vertexes;

		*pv++=(float)pnt->x;
		*pv++=(float)pnt->y;
		*pv++=(float)pnt->z;

		kx=0;
		kz=dist;
		rotate_2D_point_center(&kx,&kz,ang->y);
		*pv++=(float)(pnt->x-kx);
		*pv++=(float)pnt->y;
		*pv++=(float)(pnt->z-kz);

		glVertexPointer(3,GL_FLOAT,0,vertexes);
		glDrawArrays(GL_LINES,0,2);
	}

	glLineWidth(1.0f);
	glColor4f(1.0f,1.0f,1.0f,1.0f);
}

/* =======================================================

      View Movements
      
======================================================= */

void view_draw_movements(editor_view_type *view)
{
	int					n,k,cnt;
	d3pnt				group_pnt,pnt;
	map_mesh_type		*mesh;
	movement_type		*movement;
	movement_move_type	*move;
	
	if (!state.map.show_movements) return;

	glDisable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	
	glLineWidth(2.0f);
	glPointSize(10.0f);
	
	glColor4f(0.0f,0.0f,1.0f,1.0f);

		// draw movements
	
	movement=map.movement.movements;

	for (n=0;n!=map.movement.nmovement;n++) {
	
			// get movement group start
			
		group_pnt.x=group_pnt.y=group_pnt.z=0;
		
		cnt=0;
		mesh=map.mesh.meshes;
			
		for (k=0;k!=map.mesh.nmesh;k++) {
			if (mesh->group_idx==movement->group_idx) {
				group_pnt.x+=mesh->box.mid.x;
				group_pnt.y+=mesh->box.mid.y;
				group_pnt.z+=mesh->box.mid.z;
				cnt++;
			}
			mesh++;
		}
		
		if (cnt==0) continue;
		
		group_pnt.x/=cnt;
		group_pnt.y/=cnt;
		group_pnt.z/=cnt;
		
			// the lines

		move=movement->moves;
		
		memmove(&pnt,&group_pnt,sizeof(d3pnt));
			
		glBegin(GL_LINES);
		
		for (k=0;k!=movement->nmove;k++) {
			glVertex3i(pnt.x,pnt.y,pnt.z);
			pnt.x+=move->mov.x;
			pnt.y+=move->mov.y;
			pnt.z+=move->mov.z;
			glVertex3i(pnt.x,pnt.y,pnt.z);
			move++;
		}
				
		glEnd();
		
			// the points

		move=movement->moves;
		
		memmove(&pnt,&group_pnt,sizeof(d3pnt));
			
		glBegin(GL_POINTS);
		
		glVertex3i(pnt.x,pnt.y,pnt.z);
		
		for (k=0;k!=movement->nmove;k++) {
			pnt.x+=move->mov.x;
			pnt.y+=move->mov.y;
			pnt.z+=move->mov.z;
			glVertex3i(pnt.x,pnt.y,pnt.z);
			move++;
		}
				
		glEnd();
		
		movement++;
	}
	
	glLineWidth(1.0f);
	glColor4f(0.0f,0.0f,0.0f,1.0f);
}

/* =======================================================

      View Nodes, Scenery, etc Drawing
      
======================================================= */

void view_draw_nodes(editor_view_type *view)
{
	int			n,k;
	float		fx,fy,fz,vertexes[2*3];
	node_type	*node,*lnode;
	matrix_type	mat;

	if (!state.map.show_node) return;
	
		// angles
		
	glLineWidth(3.0f);
	glColor4f(1.0f,0.7f,0.0f,1.0f);
	
	for (n=0;n!=map.nnode;n++) {
		node=&map.nodes[n];
		
		if (view_clip_point(view,&node->pnt)) continue;
	
		fx=0.0f;
		fy=0.0f;
		fz=-(float)1000.0f;

		matrix_rotate_x(&mat,node->ang.x);
		matrix_vertex_multiply(&mat,&fx,&fy,&fz);
		
		matrix_rotate_y(&mat,node->ang.y);
		matrix_vertex_multiply(&mat,&fx,&fy,&fz);

		matrix_rotate_z(&mat,node->ang.z);
		matrix_vertex_multiply(&mat,&fx,&fy,&fz);
			
		vertexes[0]=(float)node->pnt.x;
		vertexes[1]=(float)(node->pnt.y-300);
		vertexes[2]=(float)node->pnt.z;
		vertexes[3]=(float)(node->pnt.x+(int)fx);
		vertexes[4]=(float)((node->pnt.y-300)+(int)fy);
		vertexes[5]=(float)(node->pnt.z+(int)fz);
		
		glVertexPointer(3,GL_FLOAT,0,vertexes);
		glDrawArrays(GL_LINES,0,2);
	}
	
		// connections
		
	glColor4f(0.0f,1.0f,0.0f,1.0f);
	
	for (n=0;n!=map.nnode;n++) {
		node=&map.nodes[n];
		
		if (view_clip_point(view,&node->pnt)) continue;
			
		for (k=0;k!=max_node_link;k++) {
		
			if (node->link[k]!=-1) {
				lnode=&map.nodes[node->link[k]];

				vertexes[0]=(float)node->pnt.x;
				vertexes[1]=(float)(node->pnt.y-300);
				vertexes[2]=(float)node->pnt.z;

				vertexes[3]=(float)lnode->pnt.x;
				vertexes[4]=(float)(lnode->pnt.y-300);
				vertexes[5]=(float)lnode->pnt.z;
		
				glVertexPointer(3,GL_FLOAT,0,vertexes);
				glDrawArrays(GL_LINES,0,2);
			}
		}
	}

	glLineWidth(1.0f);
	glColor4f(1.0f,1.0f,1.0f,1.0f);
		
		// nodes
	
	for (n=0;n!=map.nnode;n++) {
		node=&map.nodes[n];
		
		if (view_clip_point(view,&node->pnt)) continue;
		
		if (node->name[0]==0x0) {
			view_draw_sprite(&node->pnt,NULL,node_bitmap.gl_id);
		}
		else {
			view_draw_sprite(&node->pnt,NULL,node_defined_bitmap.gl_id);
		}
	}
}

void view_draw_spots_scenery(editor_view_type *view)
{
	int					n;
	spot_type			*spot;
	map_scenery_type	*scenery;
	
    if (!state.map.show_object) return;
    
	for (n=0;n!=map.nspot;n++) {
		spot=&map.spots[n];
		
		if (view_clip_point(view,&spot->pnt)) continue;
	
		if (!map_view_model_draw(view,&spot->pnt,&spot->ang,spot->display_model,1.0f,NULL,0)) {
			view_draw_sprite(&spot->pnt,&spot->ang,spot_bitmap.gl_id);
		}

		if (spot->type==spot_type_checkpoint) {
			if (select_check(item_map_spot,n,-1)) view_draw_circle(&spot->pnt,&spot->ang,NULL,spot->checkpoint.radius,TRUE);
		}
	}		
    
	for (n=0;n!=map.nscenery;n++) {
		scenery=&map.sceneries[n];
		
		if (view_clip_point(view,&scenery->pnt)) continue;
	
		if (!map_view_model_draw(view,&scenery->pnt,&scenery->ang,scenery->model_name,scenery->resize,scenery->texture_frame,max_map_scenery_model_texture_frame)) {
			view_draw_sprite(&scenery->pnt,&scenery->ang,scenery_bitmap.gl_id);
		}
	}		
}

void view_draw_lights_sounds_particles(editor_view_type *view)
{
	int				n;
	
	if (!state.map.show_lightsoundparticle) return;
	
	for (n=0;n!=map.nlight;n++) {
		if (view_clip_point(view,&map.lights[n].pnt)) continue;
		view_draw_sprite(&map.lights[n].pnt,NULL,light_bitmap.gl_id);
		if (select_check(item_map_light,n,-1)) view_draw_circle(&map.lights[n].pnt,NULL,&map.lights[n].setting.col,map.lights[n].setting.intensity,FALSE);
	}
	
	for (n=0;n!=map.nsound;n++) {
		if (view_clip_point(view,&map.sounds[n].pnt)) continue;
		view_draw_sprite(&map.sounds[n].pnt,NULL,sound_bitmap.gl_id);
	}
	
	for (n=0;n!=map.nparticle;n++) {
		if (view_clip_point(view,&map.particles[n].pnt)) continue;
		view_draw_sprite(&map.particles[n].pnt,NULL,particle_bitmap.gl_id);
		if ((select_check(item_map_particle,n,-1)) && (map.particles[n].light_setting.on)) view_draw_circle(&map.particles[n].pnt,NULL,&map.particles[n].light_setting.col,map.particles[n].light_setting.intensity,FALSE);
	}
}

/* =======================================================

      View Draw Mainline
      
======================================================= */

void map_view_draw_view(editor_view_type *view)
{
       // 3D view
        
	view_set_viewport(view,TRUE,TRUE);
	view_set_3D_projection(view,map.editor_setup.view_near_dist,map.editor_setup.view_far_dist,view_near_offset);
	
	view_cull_setup_frustum_clipping_planes();

		// setup item drawing

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

		// create the sort list

	view_draw_create_mesh_sort_list(view);

        // draw opaque meshes and liquids
 
	view_draw_meshes_opaque(view);

		// draw opaque mesh lines
		// push view forward to better z-buffer lines
		
	view_set_3D_projection(view,(map.editor_setup.view_near_dist+10),(map.editor_setup.view_far_dist-10),view_near_offset);
	view_draw_meshes_line(view,TRUE);

		// draw nodes, spots, etc
		// and opaque liquids

	view_set_3D_projection(view,map.editor_setup.view_near_dist,map.editor_setup.view_far_dist,view_near_offset);

	view_draw_nodes(view);
	view_draw_spots_scenery(view);
	view_draw_lights_sounds_particles(view);
	view_draw_liquids(view,TRUE);
	
	view_draw_movements(view);

        // draw transparent meshes and liquids

	view_draw_meshes_transparent(view);
	view_draw_liquids(view,FALSE);
 	
        // draw transparent mesh lines
		// push view forward to better z-buffer lines
        
	view_set_3D_projection(view,(map.editor_setup.view_near_dist+10),(map.editor_setup.view_far_dist-10),view_near_offset);
	view_draw_meshes_line(view,FALSE);
	
        // draw normals
		// push view forward to better z-buffer lines
      
	if (state.map.show_normals) {
		view_set_3D_projection(view,map.editor_setup.view_near_dist,map.editor_setup.view_far_dist,view_near_offset);
		view_draw_meshes_normals(view);
	}
		
		// draw selection
		
	map_view_draw_select(view);
}

