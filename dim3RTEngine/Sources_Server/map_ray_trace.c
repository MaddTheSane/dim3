/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Map Ray Tracing

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

extern map_type				map;
extern server_type			server;

extern int					collide_obj_count,collide_proj_count,
							collide_obj_list[max_obj_list],
							collide_proj_list[max_proj_list];

int							ray_item_count;
ray_trace_check_item_type	*ray_item_list;

/* =======================================================

      Ray Trace Item List
      
======================================================= */

bool ray_trace_initialize(char *err_str)
{
	ray_item_list=(ray_trace_check_item_type*)malloc(sizeof(ray_trace_check_item_type)*ray_trace_max_check_item);
	if (ray_item_list==NULL) {
		strcpy(err_str,"Out of Memory");
		return(FALSE);
	}

	ray_item_count=0;
	
	return(TRUE);
}

void ray_trace_shutdown(void)
{
	free(ray_item_list);
}

/* =======================================================

      Ray Trace Contact Utility
      
======================================================= */

inline void ray_trace_contact_clear(d3pnt *ept,ray_trace_contact_type *contact)
{
	contact->hit=FALSE;

	contact->poly.mesh_idx=-1;
	contact->obj.idx=-1;
	contact->proj.idx=-1;

	contact->hpt.x=ept->x;
	contact->hpt.y=ept->y;
	contact->hpt.z=ept->z;
}

/* =======================================================

      Ray Trace Last Item List
      
======================================================= */

inline ray_trace_check_item_type* ray_trace_get_last_item_list(int *item_count)
{
	*item_count=ray_item_count;
	return(ray_item_list);
}

/* =======================================================

      Ray Trace Triangles
      
======================================================= */

float ray_trace_triangle(d3pnt *spt,d3vct *vct,d3pnt *hpt,d3pnt *tpt_0,d3pnt *tpt_1,d3pnt *tpt_2)
{
	float				det,invDet,t,u,v;
	d3vct				perpVector,lineToTrigPointVector,lineToTrigPerpVector,v1,v2;
	
		// get triangle vectors
		// tpt_0 is inbetween tpt_1 and tpt_2

	v1.x=(float)(tpt_1->x-tpt_0->x);
	v1.y=(float)(tpt_1->y-tpt_0->y);
	v1.z=(float)(tpt_1->z-tpt_0->z);

	v2.x=(float)(tpt_2->x-tpt_0->x);
	v2.y=(float)(tpt_2->y-tpt_0->y);
	v2.z=(float)(tpt_2->z-tpt_0->z);
	
		// calculate the cross product and
		// then the inner product to get the
		// determinate
		
	perpVector.x=(vct->y*v2.z)-(v2.y*vct->z);
	perpVector.y=(vct->z*v2.x)-(v2.z*vct->x);
	perpVector.z=(vct->x*v2.y)-(v2.x*vct->y);

	det=(v1.x*perpVector.x)+(v1.y*perpVector.y)+(v1.z*perpVector.z);
	
		// is line on the same plane as triangle?
		
	if ((det>-0.00001f) && (det<0.00001f)) return(-1.0f);

		// get the inverse determinate

	invDet=1.0f/det;

		// calculate triangle U and test
		// using the vector from spt to tpt_0
		// and the inner product of that result and
		// the perpVector
		
	lineToTrigPointVector.x=(float)(spt->x-tpt_0->x);
	lineToTrigPointVector.y=(float)(spt->y-tpt_0->y);
	lineToTrigPointVector.z=(float)(spt->z-tpt_0->z);

	u=invDet*((lineToTrigPointVector.x*perpVector.x)+(lineToTrigPointVector.y*perpVector.y)+(lineToTrigPointVector.z*perpVector.z));
	if ((u<0.0f) || (u>1.0f)) return(-1.0f);
	
		// calculate triangle V and test
		// using the cross product of lineToTrigPointVector
		// and v1 and the inner product of that result and vct

	lineToTrigPerpVector.x=(lineToTrigPointVector.y*v1.z)-(v1.y*lineToTrigPointVector.z);
	lineToTrigPerpVector.y=(lineToTrigPointVector.z*v1.x)-(v1.z*lineToTrigPointVector.x);
	lineToTrigPerpVector.z=(lineToTrigPointVector.x*v1.y)-(v1.x*lineToTrigPointVector.y);
	
	v=invDet*((vct->x*lineToTrigPerpVector.x)+(vct->y*lineToTrigPerpVector.y)+(vct->z*lineToTrigPerpVector.z));
	if ((v<0.0f) || ((u+v)>1.0f)) return(-1.0f);
	
		// get line T for point(t) =  start_point + (vector*t)
		// use the inner product of v2 and lineToTrigPerpVector
		// -t are on the negative vector behind the point, so ignore

	t=invDet*((v2.x*lineToTrigPerpVector.x)+(v2.y*lineToTrigPerpVector.y)+(v2.z*lineToTrigPerpVector.z));
	if (t<0.0f) return(-1.0f);
	
		// get point on line of intersection
		
	hpt->x=spt->x+(int)(vct->x*t);
	hpt->y=spt->y+(int)(vct->y*t);
	hpt->z=spt->z+(int)(vct->z*t);
	
		// return t
		
	return(t);
}

float ray_trace_triangle_f(d3fpnt *spt,d3vct *vct,d3fpnt *hpt,d3fpnt *tpt_0,d3fpnt *tpt_1,d3fpnt *tpt_2)
{
	float				det,invDet,t,u,v;
	d3vct				perpVector,lineToTrigPointVector,lineToTrigPerpVector,v1,v2;
	
		// get triangle vectors
		// tpt_0 is inbetween tpt_1 and tpt_2

	v1.x=tpt_1->x-tpt_0->x;
	v1.y=tpt_1->y-tpt_0->y;
	v1.z=tpt_1->z-tpt_0->z;

	v2.x=tpt_2->x-tpt_0->x;
	v2.y=tpt_2->y-tpt_0->y;
	v2.z=tpt_2->z-tpt_0->z;
	
		// calculate the cross product and
		// then the inner product to get the
		// determinate
		
	perpVector.x=(vct->y*v2.z)-(v2.y*vct->z);
	perpVector.y=(vct->z*v2.x)-(v2.z*vct->x);
	perpVector.z=(vct->x*v2.y)-(v2.x*vct->y);

	det=(v1.x*perpVector.x)+(v1.y*perpVector.y)+(v1.z*perpVector.z);
	
		// is line on the same plane as triangle?
		
	if ((det>-0.00001f) && (det<0.00001f)) return(-1.0f);

		// get the inverse determinate

	invDet=1.0f/det;

		// calculate triangle U and test
		// using the vector from spt to tpt_0
		// and the inner product of that result and
		// the perpVector
		
	lineToTrigPointVector.x=spt->x-tpt_0->x;
	lineToTrigPointVector.y=spt->y-tpt_0->y;
	lineToTrigPointVector.z=spt->z-tpt_0->z;

	u=invDet*((lineToTrigPointVector.x*perpVector.x)+(lineToTrigPointVector.y*perpVector.y)+(lineToTrigPointVector.z*perpVector.z));
	if ((u<0.0f) || (u>1.0f)) return(-1.0f);
	
		// calculate triangle V and test
		// using the cross product of lineToTrigPointVector
		// and v1 and the inner product of that result and vct

	lineToTrigPerpVector.x=(lineToTrigPointVector.y*v1.z)-(v1.y*lineToTrigPointVector.z);
	lineToTrigPerpVector.y=(lineToTrigPointVector.z*v1.x)-(v1.z*lineToTrigPointVector.x);
	lineToTrigPerpVector.z=(lineToTrigPointVector.x*v1.y)-(v1.x*lineToTrigPointVector.y);
	
	v=invDet*((vct->x*lineToTrigPerpVector.x)+(vct->y*lineToTrigPerpVector.y)+(vct->z*lineToTrigPerpVector.z));
	if ((v<0.0f) || ((u+v)>1.0f)) return(-1.0f);
	
		// get line T for point(t) =  start_point + (vector*t)
		// use the inner product of v2 and lineToTrigPerpVector
		// -t are on the negative vector behind the point, so ignore

	t=invDet*((v2.x*lineToTrigPerpVector.x)+(v2.y*lineToTrigPerpVector.y)+(v2.z*lineToTrigPerpVector.z));
	if (t<0.0f) return(-1.0f);
	
		// get point on line of intersection
		
	hpt->x=spt->x+(vct->x*t);
	hpt->y=spt->y+(vct->y*t);
	hpt->z=spt->z+(vct->z*t);
	
		// return t
		
	return(t);
}

bool ray_trace_triangle_blocking_f(d3fpnt *spt,d3vct *vct,d3fpnt *tpt_0,d3fpnt *tpt_1,d3fpnt *tpt_2)
{
	float				det,invDet,t,u,v;
	d3vct				perpVector,lineToTrigPointVector,lineToTrigPerpVector,v1,v2;
	
		// get triangle vectors
		// tpt_0 is inbetween tpt_1 and tpt_2

	v1.x=tpt_1->x-tpt_0->x;
	v1.y=tpt_1->y-tpt_0->y;
	v1.z=tpt_1->z-tpt_0->z;

	v2.x=tpt_2->x-tpt_0->x;
	v2.y=tpt_2->y-tpt_0->y;
	v2.z=tpt_2->z-tpt_0->z;
	
		// calculate the cross product and
		// then the inner product to get the
		// determinate
		
	perpVector.x=(vct->y*v2.z)-(v2.y*vct->z);
	perpVector.y=(vct->z*v2.x)-(v2.z*vct->x);
	perpVector.z=(vct->x*v2.y)-(v2.x*vct->y);

	det=(v1.x*perpVector.x)+(v1.y*perpVector.y)+(v1.z*perpVector.z);
	
		// is line on the same plane as triangle?
		
	if ((det>-0.00001f) && (det<0.00001f)) return(FALSE);

		// get the inverse determinate

	invDet=1.0f/det;

		// calculate triangle U and test
		// using the vector from spt to tpt_0
		// and the inner product of that result and
		// the perpVector
		
	lineToTrigPointVector.x=spt->x-tpt_0->x;
	lineToTrigPointVector.y=spt->y-tpt_0->y;
	lineToTrigPointVector.z=spt->z-tpt_0->z;

	u=invDet*((lineToTrigPointVector.x*perpVector.x)+(lineToTrigPointVector.y*perpVector.y)+(lineToTrigPointVector.z*perpVector.z));
	if ((u<0.0f) || (u>1.0f)) return(FALSE);
	
		// calculate triangle V and test
		// using the cross product of lineToTrigPointVector
		// and v1 and the inner product of that result and vct

	lineToTrigPerpVector.x=(lineToTrigPointVector.y*v1.z)-(v1.y*lineToTrigPointVector.z);
	lineToTrigPerpVector.y=(lineToTrigPointVector.z*v1.x)-(v1.z*lineToTrigPointVector.x);
	lineToTrigPerpVector.z=(lineToTrigPointVector.x*v1.y)-(v1.x*lineToTrigPointVector.y);
	
	v=invDet*((vct->x*lineToTrigPerpVector.x)+(vct->y*lineToTrigPerpVector.y)+(vct->z*lineToTrigPerpVector.z));
	if ((v<0.0f) || ((u+v)>1.0f)) return(FALSE);
	
		// get line T for point(t) =  start_point + (vector*t)
		// use the inner product of v2 and lineToTrigPerpVector
		// -t are on the negative vector behind the point, so ignore

	t=invDet*((v2.x*lineToTrigPerpVector.x)+(v2.y*lineToTrigPerpVector.y)+(v2.z*lineToTrigPerpVector.z));
	return(t>=0.0f);
}

/* =======================================================

      Ray Trace Polys and Boxes
      
======================================================= */

float ray_trace_mesh_polygon(d3pnt *spt,d3vct *vct,d3pnt *hpt,map_mesh_type *mesh,map_mesh_poly_type *poly)
{
	int			n,trig_count;
	float		hit_t;
	
		// run through all the triangles of the polygon
		// since this is a single polygon, the
		// first hit is always the correct hit
		
	trig_count=poly->ptsz-2;
	
	for (n=0;n<trig_count;n++) {
		hit_t=ray_trace_triangle(spt,vct,hpt,&mesh->vertexes[poly->v[0]],&mesh->vertexes[poly->v[n+1]],&mesh->vertexes[poly->v[n+2]]);
		if (hit_t!=-1.0f) return(hit_t);
	}
	
	return(-1.0f);
}

float ray_trace_quad(d3pnt *spt,d3vct *vct,d3pnt *hpt,int ptsz,int *x,int *y,int *z)
{
	float		hit_t;
	d3pnt		tpt_0,tpt_1,tpt_2,tpt_3;
	
		// first vertex is always 0
		
	tpt_0.x=x[0];
	tpt_0.y=y[0];
	tpt_0.z=z[0];
	
		// first trig of quad
		
	tpt_1.x=x[1];
	tpt_1.y=y[1];
	tpt_1.z=z[1];

	tpt_2.x=x[2];
	tpt_2.y=y[2];
	tpt_2.z=z[2];
		
	hit_t=ray_trace_triangle(spt,vct,hpt,&tpt_0,&tpt_1,&tpt_2);
	if (hit_t!=-1.0f) return(hit_t);
	
		// second trig of quad
	
	tpt_3.x=x[3];
	tpt_3.y=y[3];
	tpt_3.z=z[3];
		
	hit_t=ray_trace_triangle(spt,vct,hpt,&tpt_0,&tpt_2,&tpt_3);
	if (hit_t!=-1.0f) return(hit_t);
	
	return(-1.0f);
}

float ray_trace_rotated_box(d3pnt *spt,d3vct *vct,d3pnt *hpt,int x,int z,int lx,int rx,int tz,int bz,int ty,int by,float rang)
{
	int			n,idx,px[4],py[4],pz[4];
	float		t[6],hit_t;
	d3pnt		pt[6];

		// min z side
		
	px[0]=px[3]=lx;
	px[1]=px[2]=rx;
	py[0]=py[1]=ty;
	py[2]=py[3]=by;
	pz[0]=pz[1]=pz[2]=pz[3]=tz;
	if (rang!=0.0f) rotate_2D_polygon(4,px,pz,x,z,rang);
	t[0]=ray_trace_quad(spt,vct,&pt[0],4,px,py,pz);

		// max z side
		
	px[0]=px[3]=lx;
	px[1]=px[2]=rx;
	py[0]=py[1]=ty;
	py[2]=py[3]=by;
	pz[0]=pz[1]=pz[2]=pz[3]=bz;
	if (rang!=0.0f) rotate_2D_polygon(4,px,pz,x,z,rang);
	t[1]=ray_trace_quad(spt,vct,&pt[1],4,px,py,pz);
	
		// min x side
	
	px[0]=px[1]=px[2]=px[3]=lx;
	py[0]=py[1]=ty;
	py[2]=py[3]=by;
	pz[0]=pz[3]=tz;
	pz[1]=pz[2]=bz;
	if (rang!=0.0f) rotate_2D_polygon(4,px,pz,x,z,rang);
	t[2]=ray_trace_quad(spt,vct,&pt[2],4,px,py,pz);
	
		// max x side
		
	px[0]=px[1]=px[2]=px[3]=rx;
	py[0]=py[1]=ty;
	py[2]=py[3]=by;
	pz[0]=pz[3]=tz;
	pz[1]=pz[2]=bz;
	if (rang!=0.0f) rotate_2D_polygon(4,px,pz,x,z,rang);
	t[3]=ray_trace_quad(spt,vct,&pt[3],4,px,py,pz);
	
		// min y side
		// we need to push in past
		// the other walls or we get unecessary hits
		// when pushed against a wall
		
	px[0]=px[3]=lx+1;
	px[1]=px[2]=rx-1;
	py[0]=py[1]=py[2]=py[3]=ty;
	pz[0]=pz[1]=tz+1;
	pz[2]=pz[3]=bz-1;
	if (rang!=0.0f) rotate_2D_polygon(4,px,pz,x,z,rang);
	t[4]=ray_trace_quad(spt,vct,&pt[4],4,px,py,pz);
	
		// max y side
		// same push in here
		
	px[0]=px[3]=lx+1;
	px[1]=px[2]=rx-1;
	py[0]=py[1]=py[2]=py[3]=by;
	pz[0]=pz[1]=tz+1;
	pz[2]=pz[3]=bz-1;
	if (rang!=0.0f) rotate_2D_polygon(4,px,pz,x,z,rang);
	t[5]=ray_trace_quad(spt,vct,&pt[5],4,px,py,pz);
	
		// find closest hit
		
	idx=-1;
	hit_t=-1.0f;
	
	for (n=0;n!=6;n++) {
		if (t[n]==-1.0f) continue;
		if ((t[n]<hit_t) || (hit_t==-1.0f)) {
			idx=n;
			hit_t=t[n];
		}
	}
	
	if (idx!=-1) {
		hpt->x=pt[idx].x;
		hpt->y=pt[idx].y;
		hpt->z=pt[idx].z;
		return(hit_t);
	}
	
	return(-1.0f);
}

float ray_trace_mesh_box(d3pnt *spt,d3vct *vct,d3pnt *hpt,map_mesh_type *mesh)
{
	return(ray_trace_rotated_box(spt,vct,hpt,mesh->box.mid.x,mesh->box.mid.z,mesh->box.min.x,mesh->box.max.x,mesh->box.min.z,mesh->box.max.z,mesh->box.min.y,mesh->box.max.y,0.0f));
}

/* =======================================================

      Ray Trace Planes
      
======================================================= */

void ray_trace_plane(d3pnt *spt,d3vct *vct,d3pnt *hpt,d3pnt *ppt_0,d3pnt *ppt_1,d3pnt *ppt_2)
{
	float			t,ka,kb,kc,kd;
	d3fpnt			f0,f1,f2;

		// get plane equation for polygon
		// ppt_0 is inbetween ppt_1 and ppt_2

	f0.x=(float)ppt_0->x;
	f0.y=(float)ppt_0->y;
	f0.z=(float)ppt_0->z;

	f1.x=(float)ppt_1->x;
	f1.y=(float)ppt_1->y;
	f1.z=(float)ppt_1->z;

	f2.x=(float)ppt_2->x;
	f2.y=(float)ppt_2->y;
	f2.z=(float)ppt_2->z;

	ka=(f1.y*(f0.z-f2.z))+(f0.y*(f2.z-f1.z))+(f2.y*(f1.z-f0.z));
	kb=(f1.z*(f0.x-f2.x))+(f0.z*(f2.x-f1.x))+(f2.z*(f1.x-f0.x));
	kc=(f1.x*(f0.y-f2.y))+(f0.x*(f2.y-f1.y))+(f2.x*(f1.y-f0.y));
	kd=((-f1.x)*((f0.y*f2.z)-(f2.y*f0.z)))-(f0.x*((f2.y*f1.z)-(f1.y*f2.z)))-(f2.x*((f1.y*f0.z)-(f0.y*f1.z)));

		// insert the ray line equation and
		// solve for the t

	t=(-((ka*(float)spt->x)+(kb*(float)spt->y)+(kc*(float)spt->z)+kd)/((ka*vct->x)+(kb*vct->y)+(kc*vct->z)));

		// get the hit point
		// this routine always hits the plane
		// no matter which direction the ray is

	hpt->x=spt->x+(int)(vct->x*t);
	hpt->y=spt->y+(int)(vct->y*t);
	hpt->z=spt->z+(int)(vct->z*t);
}

/* =======================================================

      Ray Trace Bounds
      
======================================================= */

inline void ray_trace_get_single_ray_bounds(d3pnt *spt,d3pnt *ept,d3pnt *min,d3pnt *max)
{
	if (spt->x<ept->x) {
		min->x=spt->x;
		max->x=ept->x;
	}
	else {
		min->x=ept->x;
		max->x=spt->x;
	}

	if (spt->y<ept->y) {
		min->y=spt->y;
		max->y=ept->y;
	}
	else {
		min->y=ept->y;
		max->y=spt->y;
	}

	if (spt->z<ept->z) {
		min->z=spt->z;
		max->z=ept->z;
	}
	else {
		min->z=ept->z;
		max->z=spt->z;
	}
}

/* =======================================================

      Ray Trace Mesh Bounds
      
======================================================= */

static inline bool ray_trace_mesh_bound_check(map_mesh_type *mesh,d3pnt *min,d3pnt *max)
{
		// compare x/z first as it's a better chance
		// for an elimination
		
	if (max->x<mesh->box.min.x) return(FALSE);
	if (min->x>mesh->box.max.x) return(FALSE);
	if (max->z<mesh->box.min.z) return(FALSE);
	if (min->z>mesh->box.max.z) return(FALSE);
	if (max->y<mesh->box.min.y) return(FALSE);
	if (min->y>mesh->box.max.y) return(FALSE);
		
		// skip off and pass through meshes
		// we do this second as I suspect there will
		// be a limited number of these, a lot less then
		// will be eliminated with the bounds check

	if (!mesh->flag.on) return(FALSE);
	if (mesh->flag.pass_through) return(FALSE);
	
	return(TRUE);
}

static inline bool ray_trace_mesh_poly_bound_check(map_mesh_poly_type *poly,d3pnt *min,d3pnt *max)
{
		// compare x/z first as it's a better chance
		// for an elimination

	if (max->x<poly->box.min.x) return(FALSE);
	if (min->x>poly->box.max.x) return(FALSE);
	if (max->z<poly->box.min.z) return(FALSE);
	if (min->z>poly->box.max.z) return(FALSE);
	if (max->y<poly->box.min.y) return(FALSE);
	if (min->y>poly->box.max.y) return(FALSE);
	
	return(TRUE);
}

/* =======================================================

      Ray Trace Objects
      
======================================================= */

float ray_trace_object(d3pnt *spt,d3pnt *ept,d3vct *vct,d3pnt *hpt,int *hit_box_idx,obj_type *obj)
{
	int					n,nhit_box,wid,
						x,y,z,lx,rx,tz,bz,ty,by;
	float				rang,hit_t;
	d3pnt				pnt,pt;
	model_draw			*draw;
	model_type			*model;
	model_hit_box_type	*hit_box;
	
	x=obj->pnt.x;
	z=obj->pnt.z;
	
	ty=obj->pnt.y-obj->size.y;
	y=by=obj->pnt.y;

		// setup fine check
		
	wid=obj->size.x>>1;
	lx=x-wid;
	rx=x+wid;
	
	wid=obj->size.z>>1;
	tz=z-wid;
	bz=z+wid;
		
	rang=angle_add(obj->ang.y,obj->draw.rot.y);

	*hit_box_idx=-1;

		// regular collisions

	if (!obj->hit_box.on) {
		return(ray_trace_rotated_box(spt,vct,hpt,x,z,lx,rx,tz,bz,ty,by,rang));
	}

		// check model
		
	draw=&obj->draw;
	if ((draw->model_idx==-1) || (!draw->on)) return(-1.0f);
	
	model=server.model_list.models[draw->model_idx];
	
		// check hit boxes

	pnt.x=x+draw->offset.x;
	pnt.y=y+draw->offset.y;
	pnt.z=z+draw->offset.z;
		
	hit_box=model->hit_boxes;
	nhit_box=model->nhit_box;
	
	for (n=0;n!=nhit_box;n++) {
		model_hit_box_get_center(model,n,&pnt,&draw->setup,&pt);

		wid=hit_box->box.size.x>>1;
		lx=pt.x-wid;
		rx=pt.x+wid;
		
		by=pt.y;
		ty=pt.y-hit_box->box.size.y;

		wid=hit_box->box.size.z>>1;
		tz=pt.z-wid;
		bz=pt.z+wid;
		
		hit_t=ray_trace_rotated_box(spt,vct,hpt,x,z,lx,rx,tz,bz,ty,by,rang);
		if (hit_t!=-1.0f) {
			*hit_box_idx=n;
			return(hit_t);
		}

		hit_box++;
	}

	return(-1.0f);
}

void ray_trace_object_set_hitbox(obj_type *obj,int hit_box_idx,ray_trace_contact_type *contact)
{
	if (contact->origin==poly_ray_trace_origin_object) {
		if (hit_box_idx!=-1) {
			obj->hit_box.obj_hit_box_idx=hit_box_idx;
		}
		else {
			obj->hit_box.obj_hit_box_idx=-1;
		}

		return;
	}

	if (contact->origin==poly_ray_trace_origin_projectile) {
		if (hit_box_idx!=-1) {
			obj->hit_box.proj_hit_box_idx=hit_box_idx;
		}
		else {
			obj->hit_box.proj_hit_box_idx=-1;
		}
	}
}

bool ray_trace_object_bound_check(obj_type *obj,d3pnt *min,d3pnt *max,ray_trace_contact_type *contact)
{
	int			x,z,radius;
	
		// rough x/z vector box checks
		// run this compare first as it's
		// a better chance for elimination
		
	x=obj->pnt.x;
	z=obj->pnt.z;

	radius=object_get_radius(obj);
	
	if (max->x<(x-radius)) return(FALSE);
	if (min->x>(x+radius)) return(FALSE);
	if (max->z<(z-radius)) return(FALSE);
	if (min->z>(z+radius)) return(FALSE);
	
		// rough y vector box check
		// run this next
		
	if (max->y<(obj->pnt.y-obj->size.y)) return(FALSE);
	if (min->y>obj->pnt.y) return(FALSE);
	
		// finally check the flags, I
		// suspect these situations will be
		// the least cause of eliminations

	if ((obj->hidden) || (obj->pickup.on) || (!obj->contact.object_on) || (obj->idx==contact->obj.ignore_idx)) return(FALSE);
	
	if ((contact->origin==poly_ray_trace_origin_object) && (!obj->contact.object_on)) return(FALSE);
	if ((contact->origin==poly_ray_trace_origin_projectile) && (!obj->contact.projectile_on)) return(FALSE);
	
	return(TRUE);
}

/* =======================================================

      Ray Trace Projectiles
      
======================================================= */

float ray_trace_projectile(d3pnt *spt,d3pnt *ept,d3vct *vct,d3pnt *hpt,proj_type *proj)
{
	int			wid,x,z,lx,rx,tz,bz,ty,by;
	float		rang;

	x=proj->pnt.x;
	z=proj->pnt.z;
	
	ty=proj->pnt.y-proj->size.y;
	by=proj->pnt.y;
	
		// setup fine check
		
	wid=proj->size.x>>1;
	lx=x-wid;
	rx=x+wid;
	
	wid=proj->size.z>>1;
	tz=z-wid;
	bz=z+wid;
		
	rang=angle_add(proj->ang.y,proj->draw.rot.y);

		// box collisions

	return(ray_trace_rotated_box(spt,vct,hpt,x,z,lx,rx,tz,bz,ty,by,rang));
}

bool ray_trace_projectile_bound_check(proj_type *proj,d3pnt *min,d3pnt *max,ray_trace_contact_type *contact)
{
	int			x,z,radius;

		// rough x/z vector box checks
		// run this compare first as it's
		// a better chance for elimination
		
	x=proj->pnt.x;
	z=proj->pnt.z;

	radius=projectile_get_radius(proj);
	
	if (max->x<(x-radius)) return(FALSE);
	if (min->x>(x+radius)) return(FALSE);
	if (max->z<(z-radius)) return(FALSE);
	if (min->z>(z+radius)) return(FALSE);
	
		// rough y vector box check
		
	if (max->y<(proj->pnt.y-proj->size.y)) return(FALSE);
	if (min->y>proj->pnt.y) return(FALSE);
	
		// check flags last

	return(proj->idx!=contact->proj.ignore_idx);
}

/* =======================================================

      Ray Trace Map
      
======================================================= */

void ray_trace_map_items(d3pnt *spt,d3pnt *ept,d3vct *vct,ray_trace_contact_type *base_contact,ray_trace_contact_type *contact)
{
	int							n,hit_box_idx;
	float						t,hit_t;
	d3pnt						pt;
	obj_type					*obj;
	proj_type					*proj;
	map_mesh_type				*mesh;
	map_mesh_poly_type			*poly;
	ray_trace_check_item_type	*item;

		// default is past end of line
		
	hit_t=1.0f;
	
		// check all hit candidates

	item=ray_item_list;

	for (n=0;n!=ray_item_count;n++) {
		
		switch (item->type) {

			case ray_trace_check_item_object:
				obj=server.obj_list.objs[item->index];

				t=ray_trace_object(spt,ept,vct,&pt,&hit_box_idx,obj);
				if (t==-1.0f) break;
				if (t>hit_t) break;
				
				hit_t=t;

				contact->hpt.x=pt.x;
				contact->hpt.y=pt.y;
				contact->hpt.z=pt.z;

				contact->hit=TRUE;

				contact->obj.idx=obj->idx;
				contact->proj.idx=-1;
				contact->poly.mesh_idx=-1;
				
				ray_trace_object_set_hitbox(obj,hit_box_idx,base_contact);
				break;

			case ray_trace_check_item_projectile:
				proj=server.proj_list.projs[item->index];
				
				t=ray_trace_projectile(spt,ept,vct,&pt,proj);
				if (t==-1.0f) break;
				if (t>hit_t) break;
					
				hit_t=t;

				contact->hpt.x=pt.x;
				contact->hpt.y=pt.y;
				contact->hpt.z=pt.z;

				contact->hit=TRUE;
				
				contact->obj.idx=-1;
				contact->proj.idx=proj->idx;
				contact->poly.mesh_idx=-1;
				break;

			case ray_trace_check_item_mesh_poly:

				mesh=&map.mesh.meshes[item->index];
				poly=&mesh->polys[item->index_2];
						
					// rough bounds check
					// list building is a little rough to quickly eliminate
					// most meshes, so we'll still check it here just in case
					// we can eliminate a easy mesh

				if ((spt->x<poly->box.min.x) && (ept->x<poly->box.min.x)) break;
				if ((spt->x>poly->box.max.x) && (ept->x>poly->box.max.x)) break;
				if ((spt->y<poly->box.min.y) && (ept->y<poly->box.min.y)) break;
				if ((spt->y>poly->box.max.y) && (ept->y>poly->box.max.y)) break;
				if ((spt->z<poly->box.min.z) && (ept->z<poly->box.min.z)) break;
				if ((spt->z>poly->box.max.z) && (ept->z>poly->box.max.z)) break;

					// ray trace
					
				t=ray_trace_mesh_polygon(spt,vct,&pt,mesh,poly);
				if (t==-1.0f) break;
				if (t>hit_t) break;
				
				hit_t=t;

				contact->hpt.x=pt.x;
				contact->hpt.y=pt.y;
				contact->hpt.z=pt.z;

				contact->hit=TRUE;

				contact->obj.idx=-1;
				contact->proj.idx=-1;
				contact->poly.mesh_idx=item->index;
				contact->poly.poly_idx=item->index_2;

				break;
				
			case ray_trace_check_item_mesh_box:

				mesh=&map.mesh.meshes[item->index];
				
					// ray trace
					
				t=ray_trace_mesh_box(spt,vct,&pt,mesh);
				if (t==-1.0f) break;
				if (t>hit_t) break;
				
				hit_t=t;

				contact->hpt.x=pt.x;
				contact->hpt.y=pt.y;
				contact->hpt.z=pt.z;

				contact->hit=TRUE;

				contact->obj.idx=-1;
				contact->proj.idx=-1;
				contact->poly.mesh_idx=item->index;
				contact->poly.poly_idx=0;
				break;
				
		}
		
		item++;
	}
}

void ray_trace_map_single(d3pnt *spt,d3pnt *ept,d3vct *vct,ray_trace_contact_type *contact)
{
	int							n,k,hit_box_idx;
	float						t,hit_t;
	d3pnt						pt,min,max;
	obj_type					*obj;
	proj_type					*proj;
	map_mesh_type				*mesh;
	map_mesh_poly_type			*poly;

		// get bounds for vector
		// and use those as a collision check
		// this is rough, but going through all
		// the meshes is actually longer than
		// dealing with a few missed hits

	ray_trace_get_single_ray_bounds(spt,ept,&min,&max);

		// default is past end of line
		
	hit_t=1.0f;

		// check objects

	if (contact->obj.on) {
	
		for (n=0;n!=collide_obj_count;n++) {
			obj=server.obj_list.objs[collide_obj_list[n]];
			if (obj==NULL) continue;

			if (!ray_trace_object_bound_check(obj,&min,&max,contact)) continue;
			
			t=ray_trace_object(spt,ept,vct,&pt,&hit_box_idx,obj);
			if (t==-1.0f) continue;
			if (t>hit_t) continue;
			
			hit_t=t;

			contact->hpt.x=pt.x;
			contact->hpt.y=pt.y;
			contact->hpt.z=pt.z;

			contact->hit=TRUE;

			contact->obj.idx=obj->idx;
			contact->proj.idx=-1;
			contact->poly.mesh_idx=-1;
			
			ray_trace_object_set_hitbox(obj,hit_box_idx,contact);
		}
	}
	
		// check projectiles
		
	if (contact->proj.on) {
	
		for (n=0;n!=collide_proj_count;n++) {
			proj=server.proj_list.projs[collide_proj_list[n]];
			if (!proj->on) continue;

			if (!ray_trace_projectile_bound_check(proj,&min,&max,contact)) continue;
			
			t=ray_trace_projectile(spt,ept,vct,&pt,proj);
			if (t==-1.0f) continue;
			if (t>hit_t) continue;
				
			hit_t=t;

			contact->hpt.x=pt.x;
			contact->hpt.y=pt.y;
			contact->hpt.z=pt.z;

			contact->hit=TRUE;

			contact->obj.idx=-1;
			contact->proj.idx=proj->idx;
			contact->poly.mesh_idx=-1;
		}
	}

		// check all meshes

	for (n=0;n!=map.mesh.nmesh;n++) {

		mesh=&map.mesh.meshes[n];
		if (!ray_trace_mesh_bound_check(mesh,&min,&max)) continue;

			// halo checks

		if (contact->origin==poly_ray_trace_origin_halo) {
			if (mesh->flag.no_halo_obscure) continue;
		}
		
			// simple collisions
			
		if (mesh->flag.simple_collision) {
			t=ray_trace_mesh_box(spt,vct,&pt,mesh);
			if (t==-1.0f) continue;
			if (t>hit_t) continue;
		
			hit_t=t;

			contact->hpt.x=pt.x;
			contact->hpt.y=pt.y;
			contact->hpt.z=pt.z;

			contact->hit=TRUE;
			
			contact->obj.idx=-1;
			contact->proj.idx=-1;
			contact->poly.mesh_idx=n;
			contact->poly.poly_idx=0;
		
			continue;
		}
		
			// complex collisions
		
		for (k=0;k!=mesh->npoly;k++) {

			poly=&mesh->polys[k];
			if (!ray_trace_mesh_poly_bound_check(poly,&min,&max)) continue;

			t=ray_trace_mesh_polygon(spt,vct,&pt,mesh,poly);
			if (t==-1.0f) continue;
			if (t>hit_t) continue;
			
			hit_t=t;

			contact->hpt.x=pt.x;
			contact->hpt.y=pt.y;
			contact->hpt.z=pt.z;

			contact->hit=TRUE;
			
			contact->obj.idx=-1;
			contact->proj.idx=-1;
			contact->poly.mesh_idx=n;
			contact->poly.poly_idx=k;
		}
				
	}
}

/* =======================================================

      Setup Item Hist Lists
      
======================================================= */

void ray_trace_map_item_list_setup(int cnt,d3pnt *bounds_min,d3pnt *bounds_max,d3pnt *spts,d3pnt *epts,ray_trace_contact_type *contact)
{
	int							n,k;
	obj_type					*obj;
	proj_type					*proj;
	map_mesh_type				*mesh;
	map_mesh_poly_type			*poly;
	ray_trace_check_item_type	*item;
	
	ray_item_count=0;
	item=ray_item_list;

		// check objects

	if (contact->obj.on) {
	
		for (n=0;n!=collide_obj_count;n++) {
			obj=server.obj_list.objs[collide_obj_list[n]];
			if (obj==NULL) continue;
			
			if (ray_trace_object_bound_check(obj,bounds_min,bounds_max,contact)) {
				item->type=ray_trace_check_item_object;
				item->index=obj->idx;

				item++;
				ray_item_count++;
			
				if (ray_item_count==ray_trace_max_check_item) return;
			}
		}
	}
	
		// check projectiles
		
	if (contact->proj.on) {
	
		for (n=0;n!=collide_proj_count;n++) {
			proj=server.proj_list.projs[collide_proj_list[n]];
			if (!proj->on) continue;

			if (ray_trace_projectile_bound_check(proj,bounds_min,bounds_max,contact)) {
				item->type=ray_trace_check_item_projectile;
				item->index=proj->idx;

				item++;
				ray_item_count++;
				
				if (ray_item_count==ray_trace_max_check_item) return;
			}
		}
	}

		// check all meshes

	for (n=0;n!=map.mesh.nmesh;n++) {

		mesh=&map.mesh.meshes[n];
		if (!ray_trace_mesh_bound_check(mesh,bounds_min,bounds_max)) continue;

			// halo checks

		if (contact->origin==poly_ray_trace_origin_halo) {
			if (mesh->flag.no_halo_obscure) continue;
		}
		
			// simple collisions
			
		if (mesh->flag.simple_collision) {
			item->type=ray_trace_check_item_mesh_box;
			item->index=n;

			item++;
			ray_item_count++;

			if (ray_item_count==ray_trace_max_check_item) return;
			
			continue;
		}

			// complex collisions
			
		for (k=0;k!=mesh->npoly;k++) {

			poly=&mesh->polys[k];
			if (!ray_trace_mesh_poly_bound_check(poly,bounds_min,bounds_max)) continue;

				// add to item list

			item->type=ray_trace_check_item_mesh_poly;
			item->index=n;
			item->index_2=k;

			item++;
			ray_item_count++;

			if (ray_item_count==ray_trace_max_check_item) return;
		}
				
	}
}

/* =======================================================

      Ray Utilities
      
======================================================= */

void ray_push(d3pnt *pt,d3ang *ang,int dist)
{
	d3vct			vct;
	matrix_type		mat;

	vct.x=0.0f;
	vct.y=0.0f;
	vct.z=(float)-dist;

	if (ang->x!=0.0f) {
		matrix_rotate_x(&mat,ang->x);
		matrix_vertex_multiply(&mat,&vct.x,&vct.y,&vct.z);
	}
	
	if (ang->z!=0.0f) {
		matrix_rotate_z(&mat,ang->z);
		matrix_vertex_multiply(&mat,&vct.x,&vct.y,&vct.z);
	}
	
	if (ang->y!=0.0f) {
		matrix_rotate_y(&mat,ang->y);
		matrix_vertex_multiply(&mat,&vct.x,&vct.y,&vct.z);
	}
	
	pt->x+=(int)vct.x;
	pt->y+=(int)vct.y;
	pt->z+=(int)vct.z;
}

void ray_push_to_end(d3pnt *pt,d3pnt *ept,int dist)
{
	int			x,y,z,td;

	x=ept->x-pt->x;
	y=ept->y-pt->y;
	z=ept->z-pt->z;

	td=abs(x)+abs(y)+abs(z);
	if (td==0) return;

	pt->x+=(x*dist)/td;
	pt->y+=(y*dist)/td;
	pt->z+=(z*dist)/td;
}

/* =======================================================

      Ray Trace Main Lines
      
======================================================= */

bool ray_trace_map_by_angle(d3pnt *spt,d3ang *ang,int dist,ray_trace_contact_type *contact)
{
	d3pnt				ept;
	d3vct				vct;
	matrix_type			mat;
		
		// get vector
		
	vct.x=0.0f;
	vct.y=0.0f;
	vct.z=(float)-dist;

	matrix_rotate_zyx(&mat,ang->x,ang->y,ang->z);
	matrix_vertex_multiply(&mat,&vct.x,&vct.y,&vct.z);
	
		// get end point
	
	ept.x=spt->x+(int)vct.x;
	ept.y=spt->y+(int)vct.y;
	ept.z=spt->z+(int)vct.z;
		
		// ray trace map

	ray_trace_contact_clear(&ept,contact);
	ray_trace_map_single(spt,&ept,&vct,contact);
	
	return(contact->hit);
}

bool ray_trace_map_by_point(d3pnt *spt,d3pnt *ept,ray_trace_contact_type *contact)
{
	d3vct			vct;
		
		// create vector from points

	vct.x=(float)(ept->x-spt->x);
	vct.y=(float)(ept->y-spt->y);
	vct.z=(float)(ept->z-spt->z);
	
		// check against map

	ray_trace_contact_clear(ept,contact);
	ray_trace_map_single(spt,ept,&vct,contact);
	
	return(contact->hit);
}

void ray_trace_map_by_point_array(int cnt,d3pnt *bounds_min,d3pnt *bounds_max,d3pnt *spts,d3pnt *epts,ray_trace_contact_type *base_contact,ray_trace_contact_type *contacts)
{
	int						n;
	d3pnt					*spt,*ept;
	d3vct					vct;
	ray_trace_contact_type	*contact;
	
		// setup list of items crossed by this
		// collection of rays
		
	ray_trace_map_item_list_setup(cnt,bounds_min,bounds_max,spts,epts,base_contact);

		// run the ray trace

	spt=spts;
	ept=epts;
	contact=contacts;
	
	for (n=0;n!=cnt;n++) {
		ray_trace_contact_clear(ept,contact);

		vct.x=(float)(ept->x-spt->x);
		vct.y=(float)(ept->y-spt->y);
		vct.z=(float)(ept->z-spt->z);
		
		ray_trace_map_items(spt,ept,&vct,base_contact,contact);
		
		spt++;
		ept++;
		contact++;
	}
}

/* =======================================================

      Ray Trace Shadow Poly
      
======================================================= */

void ray_trace_shadow_to_mesh_poly(int cnt,d3vct *vct,d3fpnt *spt,d3fpnt *hpt,bool *hits,int mesh_idx,int poly_idx)
{
	int						n,k,trig_count;
	bool					*hit;
	d3pnt					*pt;
	d3fpnt					*pp,*sp,*hp,
							plane_vp[8];
	map_mesh_type			*mesh;
	map_mesh_poly_type		*poly;
	
		// get polygon
		
	mesh=&map.mesh.meshes[mesh_idx];
	poly=&mesh->polys[poly_idx];
	
		// setup the plane
		// by enlarging the poly

	pp=plane_vp;
	
	for (n=0;n!=poly->ptsz;n++) {
		pt=&mesh->vertexes[poly->v[n]];
		pp->x=(float)(((pt->x-poly->box.mid.x)*100)+poly->box.mid.x);
		pp->y=(float)(((pt->y-poly->box.mid.y)*100)+poly->box.mid.y);
		pp->z=(float)(((pt->z-poly->box.mid.z)*100)+poly->box.mid.z);
		pp++;
	}
	
		// run the ray array
		
	sp=spt;
	hp=hpt;
	hit=hits;

	for (n=0;n!=cnt;n++) {
		hp->x=sp->x;
		hp->y=sp->y;
		hp->z=sp->z;

			// run through all the triangles of the polygon
			// any hit counts, we are never
			// looking for close hits

		*hit=FALSE;
			
		trig_count=poly->ptsz-2;
		
		for (k=0;k<trig_count;k++) {
			if (ray_trace_triangle_f(sp,vct,hp,&plane_vp[0],&plane_vp[k+1],&plane_vp[k+2])!=-1.0f) {
				*hit=TRUE;
				break;
			}
		}

		sp++;
		hp++;
		hit++;
	}
}

