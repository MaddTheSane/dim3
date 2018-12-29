/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Decals

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
#include "objects.h"

extern map_type			map;
extern server_type		server;
extern iface_type		iface;
extern setup_type		setup;

/* =======================================================

      Find Marks
      
======================================================= */

int mark_find(char *name)
{
	int				n;
	iface_mark_type	*mark;
	
	mark=iface.mark_list.marks;

	for (n=0;n!=iface.mark_list.nmark;n++) {
		if (strcasecmp(mark->name,name)==0)  return(n);
		mark++;
	}
	
	return(-1);
}

/* =======================================================

      Decal List
      
======================================================= */

bool decal_initialize_list(void)
{
	int				n;

		// pre-alloc all decals

	for (n=0;n!=max_decal_list;n++) {

			// memory for decals

		server.decal_list.decals[n]=(decal_type*)malloc(sizeof(decal_type));
		if (server.decal_list.decals[n]==NULL) return(FALSE);

			// not used

		server.decal_list.decals[n]->on=FALSE;
	}

	return(TRUE);
}

void decal_free_list(void)
{
	int				n;

	for (n=0;n!=max_decal_list;n++) {
		if (server.decal_list.decals[n]!=NULL) free(server.decal_list.decals[n]);
	}
}

int decal_count_list(void)
{
	int				n,count;

	count=0;

	for (n=0;n!=max_decal_list;n++) {
		if (server.decal_list.decals[n]!=NULL) {
			if (server.decal_list.decals[n]->on) count++;
		}
	}

	return(count);
}

/* =======================================================

      Check if Segment is OK for Decal
      
======================================================= */

bool decal_segment_ok(map_mesh_poly_type *poly,int mark_idx)
{
	if (map.textures[poly->txt_idx].frames[0].bitmap.opaque) return(!iface.mark_list.marks[mark_idx].no_opaque);
	return(!iface.mark_list.marks[mark_idx].no_transparent);
}

/* =======================================================

      Move Decals Attached To Meshes
      
======================================================= */

void decal_move_with_mesh(int mesh_idx,d3pnt *motion)
{
	int			n,i;
	decal_type	*decal;

	for (n=0;n!=max_decal_list;n++) {
		decal=server.decal_list.decals[n];
		if (!decal->on) continue;

		if (decal->mesh_idx==mesh_idx) {
			for (i=0;i!=4;i++) {
				decal->x[i]+=motion->x;
				decal->y[i]+=motion->y;
				decal->z[i]+=motion->z;
			}
		}
	}
}

void decal_rotate_with_mesh(int mesh_idx,float y)
{
	int				n;
	d3pnt			mpt;
	decal_type		*decal;
	map_mesh_type	*mesh;

		// get center point for mesh

	mesh=&map.mesh.meshes[mesh_idx];

	map_mesh_calculate_center(&map,mesh_idx,&mpt);
	mpt.x+=mesh->rot_off.x;
	mpt.y+=mesh->rot_off.y;
	mpt.z+=mesh->rot_off.z;

		// rotate any decals on it

	for (n=0;n!=max_decal_list;n++) {
		decal=server.decal_list.decals[n];
		if (!decal->on) continue;

		if (decal->mesh_idx==mesh_idx) {
			rotate_2D_polygon(4,decal->x,decal->z,mpt.x,mpt.z,y);
		}
	}
}

/* =======================================================

      Specific Poly Types
      
======================================================= */

void decal_add_wall_like(d3pnt *pnt,decal_type *decal,map_mesh_poly_type *poly,int mark_idx,int sz)
{
	int				idx,lx,rx,lz,rz;
    
		// decal rotation

	idx=0;
	if (!iface.mark_list.marks[mark_idx].no_rotate) idx=random_int(4);

        // setup decal
        
    line_2D_find_inside_infinite(pnt->x,pnt->z,sz,poly->line.lx,poly->line.lz,poly->line.rx,poly->line.rz,&lx,&lz,&rx,&rz);
        
	decal->x[idx]=decal->x[((idx+3)&0x3)]=lx;
	decal->x[((idx+1)&0x3)]=decal->x[((idx+2)&0x3)]=rx;
	decal->z[idx]=decal->z[((idx+3)&0x3)]=lz;
	decal->z[((idx+1)&0x3)]=decal->z[((idx+2)&0x3)]=rz;

	decal->y[idx]=pnt->y-sz;
	decal->y[((idx+1)&0x3)]=pnt->y-sz;
	decal->y[((idx+2)&0x3)]=pnt->y+sz;
	decal->y[((idx+3)&0x3)]=pnt->y+sz;
}

void decal_add_floor_like(d3pnt *pnt,decal_type *decal,map_mesh_type *mesh,map_mesh_poly_type *poly,int mark_idx,int sz)
{
	int				n,ptsz,px[8],py[8],pz[8];
	d3pnt			*pt;

	decal->x[0]=decal->x[3]=decal->z[0]=decal->z[1]=-sz;
	decal->x[1]=decal->x[2]=decal->z[2]=decal->z[3]=sz;
	decal->y[0]=decal->y[1]=decal->y[2]=decal->y[3]=0;

	if (!iface.mark_list.marks[mark_idx].no_rotate) rotate_polygon_center(4,decal->x,decal->y,decal->z,0,random_float(359),0);
	
		// if poly not flat, need array of
		// coordinates to calculate y position

	if (!poly->box.flat) {

		ptsz=poly->ptsz;

		for (n=0;n!=ptsz;n++) {
			pt=&mesh->vertexes[poly->v[n]];
			px[n]=pt->x;
			py[n]=pt->y;
			pz[n]=pt->z;
		}

	}

	for (n=0;n!=4;n++) {
		decal->x[n]=decal->x[n]+pnt->x;
		decal->z[n]=decal->z[n]+pnt->z;

		if (poly->box.flat) {
			decal->y[n]=poly->box.mid.y;
		}
		else {
			decal->y[n]=polygon_find_y(ptsz,px,py,pz,decal->x[n],decal->z[n]);
			if (decal->y[n]==-1) decal->y[n]=poly->box.mid.y;
		}
	}
}

/* =======================================================

      Add Decal to Poly
      
======================================================= */

void decal_add(int obj_idx,d3pnt *pnt,poly_pointer_type *poly_ptr,int mark_idx,int sz,float alpha)
{
	int					n,idx;
	decal_type			*decal;
	map_mesh_type		*mesh;
	map_mesh_poly_type	*poly;

		// can decal this poly?

	mesh=&map.mesh.meshes[poly_ptr->mesh_idx];
	poly=&mesh->polys[poly_ptr->poly_idx];
	
	if (!decal_segment_ok(poly,mark_idx)) return;
     
		// find a free decal
		// if no more decals, use the first one
		// in the list as it will be the oldest

	idx=0;

	for (n=0;n!=max_decal_list;n++) {
		decal=server.decal_list.decals[n];
		if (!decal->on) {
			idx=n;
			break;
		}
	}
   
        // get decal spot
        
    decal=server.decal_list.decals[idx];
	decal->on=TRUE;
	
	decal->start_tick=game_time_get();
	
        // wall-like decals

	if (poly->box.wall_like) {
		decal_add_wall_like(pnt,decal,poly,mark_idx,sz);
	}

		// floor-like decals

	else {
		decal_add_floor_like(pnt,decal,mesh,poly,mark_idx,sz);
	}

		// finish decal setup

	decal->mesh_idx=poly_ptr->mesh_idx;
	decal->poly_idx=poly_ptr->poly_idx;
    
    decal->mark_idx=mark_idx;
    decal->alpha=alpha;
}

/* =======================================================

      Dispose Decals
      
======================================================= */

void decal_dispose(void)
{
	int					n,tick;
	decal_type			*decal;
	iface_mark_type		*mark;
	
	tick=game_time_get();
	
		// turn off all decals that have timed-out

	for (n=0;n!=max_decal_list;n++) {
		decal=server.decal_list.decals[n];
		if (!decal->on) continue;

			// is it timed out?

		mark=&iface.mark_list.marks[decal->mark_idx];
		if ((tick-decal->start_tick)<mark->total_msec) continue;

			// turn off

		decal->on=FALSE;
	}
}

