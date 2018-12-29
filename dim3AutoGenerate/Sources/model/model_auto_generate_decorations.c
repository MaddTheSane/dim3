/****************************** File *********************************

Module: dim3 Auto Generator
Author: Brian Barnes
 Usage: Model Auto Generation Decorations

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

(c) 2000-2014 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3autogenerate.h"
#endif

#define ag_model_decoration_bone_spike_count		5
#define ag_model_decoration_bone_spike_extra		10

/* =======================================================

      Spike Decorations
      
======================================================= */

void ag_model_decoration_spike(model_type *model,model_mesh_type *mesh,int bone_idx)
{
	int					n,nvertex,npoly;
	int					x[5],y[5],z[5];
	model_bone_type		*bone;
	model_vertex_type	*vertex;
	model_poly_type		*poly;

		// spike attached to bone

	bone=&model->bones[bone_idx];

		// spike standing straight up

	x[0]=x[3]=-50;
	x[1]=x[2]=50;
	x[4]=0;
	y[0]=y[1]=y[2]=y[3]=0;
	y[4]=-(200+ag_random_int(250));
	z[0]=z[1]=-50;
	z[2]=z[3]=50;
	z[4]=0;

	rotate_polygon_center(5,x,y,z,(float)ag_random_int(360),(float)ag_random_int(360),(float)ag_random_int(360));

		// add vertexes

	nvertex=mesh->nvertex;
	model_mesh_set_vertex_count(model,0,(nvertex+5));

	vertex=&mesh->vertexes[nvertex];

	for (n=0;n!=5;n++) {

		vertex->pnt.x=bone->pnt.x+x[n];
		vertex->pnt.y=bone->pnt.y+y[n];
		vertex->pnt.z=bone->pnt.z+z[n];

			// bone connection

		vertex->major_bone_idx=bone_idx;
		vertex->minor_bone_idx=-1;
		vertex->bone_factor=1.0f;

			// tangent space

		vector_create(&vertex->tangent_space.normal,vertex->pnt.x,vertex->pnt.y,vertex->pnt.z,bone->pnt.x,bone->pnt.y,bone->pnt.z);

		vertex++;
	}

		// add polys

	npoly=mesh->npoly;
	model_mesh_set_poly_count(model,0,(npoly+4));

	poly=&mesh->polys[npoly];

	poly->ptsz=3;
	poly->txt_idx=0;

	poly->v[0]=nvertex;
	poly->v[1]=nvertex+1;
	poly->v[2]=nvertex+4;

	poly->gx[0]=0.0f;
	poly->gx[1]=poly->gx[2]=0.5f;
	poly->gy[0]=poly->gy[1]=0.5f;
	poly->gy[2]=1.0f;

	poly++;

	poly->ptsz=3;
	poly->txt_idx=0;

	poly->v[0]=nvertex+1;
	poly->v[1]=nvertex+2;
	poly->v[2]=nvertex+4;

	poly->gx[0]=0.0f;
	poly->gx[1]=poly->gx[2]=0.5f;
	poly->gy[0]=poly->gy[1]=0.5f;
	poly->gy[2]=1.0f;

	poly++;

	poly->ptsz=3;
	poly->txt_idx=0;

	poly->v[0]=nvertex+2;
	poly->v[1]=nvertex+3;
	poly->v[2]=nvertex+4;

	poly->gx[0]=0.0f;
	poly->gx[1]=poly->gx[2]=0.5f;
	poly->gy[0]=poly->gy[1]=0.5f;
	poly->gy[2]=1.0f;

	poly++;

	poly->ptsz=3;
	poly->txt_idx=0;

	poly->v[0]=nvertex+3;
	poly->v[1]=nvertex;
	poly->v[2]=nvertex+4;

	poly->gx[0]=0.0f;
	poly->gx[1]=poly->gx[2]=0.5f;
	poly->gy[0]=poly->gy[1]=0.5f;
	poly->gy[2]=1.0f;
}

void ag_model_decoration_bone_spike(model_type *model,model_mesh_type *mesh,int bone_idx)
{
	int				n,nspike;

	nspike=ag_model_decoration_bone_spike_count+ag_random_int(ag_model_decoration_bone_spike_extra);

	for (n=0;n!=nspike;n++) {
		ag_model_decoration_spike(model,mesh,bone_idx);
	}
}

/* =======================================================

      Model Decorations
      
======================================================= */

void ag_model_add_decorations(model_type *model)
{
	int					n;
	model_mesh_type		*mesh;

	mesh=&model->meshes[0];

	for (n=0;n!=model->nbone;n++) {

			// can bone have decoration

		if (!ag_model_bone_is_decoration_ok(model,n)) continue;

			// add decoration

		switch (ag_random_int(2)) {

			case 1:
				ag_model_decoration_bone_spike(model,mesh,n);
				break;

		}
	}
}
