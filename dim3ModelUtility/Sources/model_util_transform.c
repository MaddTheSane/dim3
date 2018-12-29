/****************************** File *********************************

Module: dim3 Model Utility
Author: Brian Barnes
 Usage: Model Transforms

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

(c) 2000-2010 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3modelutility.h"
#endif

/* =======================================================

      Scale Model
      
======================================================= */

void model_rescale_box(model_box_type *mbox,d3fpnt *scale)
{
	mbox->size.x=(int)((float)mbox->size.x*scale->x);
	mbox->size.y=(int)((float)mbox->size.y*scale->y);
	mbox->size.z=(int)((float)mbox->size.z*scale->z);

	mbox->offset.x=(int)((float)mbox->offset.x*scale->x);
	mbox->offset.y=(int)((float)mbox->offset.y*scale->y);
	mbox->offset.z=(int)((float)mbox->offset.z*scale->z);
}

void model_scale(model_type *model,int mesh_idx,d3fpnt *scale)
{
	int					n,nvertex;
	model_vertex_type	*vertex;
	
	if ((scale->x<=0.0f) || (scale->y<=0.0f) || (scale->z<=0.0f)) return;
    
	nvertex=model->meshes[mesh_idx].nvertex;
	vertex=model->meshes[mesh_idx].vertexes;
	
	for (n=0;n!=nvertex;n++) {
		vertex->pnt.x=(int)((float)vertex->pnt.x*scale->x);
		vertex->pnt.y=(int)((float)vertex->pnt.y*scale->y);
		vertex->pnt.z=(int)((float)vertex->pnt.z*scale->z);
		vertex++;
	}
	
	model_calculate_parents(model);
}

void model_scale_all(model_type *model,d3fpnt *scale)
{
	int					n,nmesh,nbone;
	model_bone_type		*bone;
	
	if ((scale->x<=0.0f) || (scale->y<=0.0f) || (scale->z<=0.0f)) return;
    
        // boxes
        
	model_rescale_box(&model->view_box,scale);
	
		// scale meshes
		
	nmesh=model->nmesh;
	
	for (n=0;n!=nmesh;n++) {
		model_scale(model,n,scale);
	}
	
		// scale bones
		
	nbone=model->nbone;
	bone=model->bones;
	
	for (n=0;n!=nbone;n++) {
		bone->pnt.x=(int)((float)bone->pnt.x*scale->x);
		bone->pnt.y=(int)((float)bone->pnt.y*scale->y);
		bone->pnt.z=(int)((float)bone->pnt.z*scale->z);
		bone++;
	}
	
	model_calculate_parents(model);
}

/* =======================================================

      Flips
      
======================================================= */

void model_flip(model_type *model,int mesh_idx,bool flip_x,bool flip_y,bool flip_z)
{
	int						n,nvertex;
	model_vertex_type		*vertex;
	
	nvertex=model->meshes[mesh_idx].nvertex;
	vertex=model->meshes[mesh_idx].vertexes;
	
	for (n=0;n!=nvertex;n++) {
	
		if (flip_x) {
			vertex->pnt.x=-vertex->pnt.x;
			vertex->tangent_space.normal.x=-vertex->tangent_space.normal.x;
		}
		
		if (flip_y) {
			vertex->pnt.y=-vertex->pnt.y;
			vertex->tangent_space.normal.y=-vertex->tangent_space.normal.y;
		}

		if (flip_z) {
			vertex->pnt.z=-vertex->pnt.z;
			vertex->tangent_space.normal.z=-vertex->tangent_space.normal.z;
		}
		
		vertex++;
	}
}

void model_flip_all(model_type *model,bool flip_x,bool flip_y,bool flip_z)
{
	int						i,n,nmesh,nbone,npose;
	model_bone_type			*bone;
	model_pose_type			*pose;
	model_bone_move_type	*bone_move;
	
		// flip meshes
		
	nmesh=model->nmesh;
	
	for (i=0;i!=nmesh;i++) {
		model_flip(model,i,flip_x,flip_y,flip_z);
	}
	
		// flip bones
		
	nbone=model->nbone;
	bone=model->bones;
	
	for (i=0;i!=nbone;i++) {
		if (flip_x) bone->pnt.x=-bone->pnt.x;
		if (flip_y) bone->pnt.y=-bone->pnt.y;
		if (flip_z) bone->pnt.z=-bone->pnt.z;
		bone++;
	}

	model_calculate_parents(model);
	
		// flip poses
		
	npose=model->npose;
	pose=model->poses;
	
	for (i=0;i!=npose;i++) {
	
		bone_move=pose->bone_moves;
		
		for (n=0;n!=nbone;n++) {
		
			if (flip_x) {
				bone_move->rot.y=-bone_move->rot.y;
				bone_move->rot.z=-bone_move->rot.z;
				bone_move->mov.x=1.0f-(bone_move->mov.x-1.0f);
			}
			if (flip_y) {
				bone_move->rot.x=-bone_move->rot.x;
				bone_move->rot.z=-bone_move->rot.z;
				bone_move->mov.y=1.0f-(bone_move->mov.y-1.0f);
			}
			if (flip_z) {
				bone_move->rot.x=-bone_move->rot.x;
				bone_move->rot.y=-bone_move->rot.y;
				bone_move->mov.z=1.0f-(bone_move->mov.z-1.0f);
			}
			
			bone_move++;
		}
		
		pose++;
	}
}

/* =======================================================

      Swaps and Centers
      
======================================================= */

void model_swap_xz(model_type *model,int mesh_idx)
{
	int					n,k,nvertex;
	float				f;
	model_vertex_type	*vertex;
	
	nvertex=model->meshes[mesh_idx].nvertex;
	vertex=model->meshes[mesh_idx].vertexes;
	
	for (n=0;n!=nvertex;n++) {
	
		k=vertex->pnt.x;
		vertex->pnt.x=vertex->pnt.z;
		vertex->pnt.z=k;
		
		f=vertex->tangent_space.normal.x;
		vertex->tangent_space.normal.x=vertex->tangent_space.normal.z;
		vertex->tangent_space.normal.z=f;

		vertex++;
	}
}

void model_swap_xz_all(model_type *model)
{
	int						i,k,n,nmesh,nbone,npose;
	float					f;
	model_bone_type			*bone;
	model_pose_type			*pose;
	model_bone_move_type	*bone_move;
	
		// swap meshes
		
	nmesh=model->nmesh;
	
	for (i=0;i!=nmesh;i++) {
		model_swap_xz(model,i);
	}
	
		// swap bones
		
	nbone=model->nbone;
	bone=model->bones;
	
	for (i=0;i!=nbone;i++) {
		k=bone->pnt.x;
		bone->pnt.x=bone->pnt.z;
		bone->pnt.z=k;
		bone++;
	}

	model_calculate_parents(model);
	
		// swap poses
		
	npose=model->npose;
	pose=model->poses;
	
	for (i=0;i!=npose;i++) {
	
		bone_move=pose->bone_moves;
		
		for (n=0;n!=nbone;n++) {

			f=bone_move->rot.x;
			bone_move->rot.x=bone_move->rot.z;
			bone_move->rot.z=f;
			
			f=bone_move->mov.x;
			bone_move->mov.x=bone_move->mov.z;
			bone_move->mov.z=f;
			
			bone_move++;
		}
		
		pose++;
	}
}

void model_swap_yz(model_type *model,int mesh_idx)
{
	int					n,k,nvertex;
	float				f;
	model_vertex_type	*vertex;
	
	nvertex=model->meshes[mesh_idx].nvertex;
	vertex=model->meshes[mesh_idx].vertexes;
	
	for (n=0;n!=nvertex;n++) {
	
		k=vertex->pnt.y;
		vertex->pnt.y=vertex->pnt.z;
		vertex->pnt.z=k;
		
		f=vertex->tangent_space.normal.y;
		vertex->tangent_space.normal.y=vertex->tangent_space.normal.z;
		vertex->tangent_space.normal.z=f;

		vertex++;
	}
}

void model_swap_yz_all(model_type *model)
{
	int						i,k,n,nmesh,nbone,npose;
	float					f;
	model_bone_type			*bone;
	model_pose_type			*pose;
	model_bone_move_type	*bone_move;
	
		// swap meshes
		
	nmesh=model->nmesh;
	
	for (i=0;i!=nmesh;i++) {
		model_swap_yz(model,i);
	}
	
		// swap bones
		
	nbone=model->nbone;
	bone=model->bones;
	
	for (i=0;i!=nbone;i++) {
		k=bone->pnt.y;
		bone->pnt.y=bone->pnt.z;
		bone->pnt.z=k;
		bone++;
	}

	model_calculate_parents(model);
	
		// swap poses
		
	npose=model->npose;
	pose=model->poses;
	
	for (i=0;i!=npose;i++) {
	
		bone_move=pose->bone_moves;
		
		for (n=0;n!=nbone;n++) {

			f=bone_move->rot.y;
			bone_move->rot.y=bone_move->rot.z;
			bone_move->rot.z=f;
			
			f=bone_move->mov.y;
			bone_move->mov.y=bone_move->mov.z;
			bone_move->mov.z=f;
			
			bone_move++;
		}
		
		pose++;
	}
}

void model_center_xz(model_type *model,int mesh_idx)
{
	int					i,nvertex,xoff,zoff,
                        minx,maxx,minz,maxz,miny,maxy;
	model_vertex_type	*vertex;
	
		// mesh
		
	model_get_vertex_extent(model,mesh_idx,&minx,&maxx,&minz,&maxz,&miny,&maxy);
    xoff=-(minx+maxx)/2;
    zoff=-(minz+maxz)/2;
    
	nvertex=model->meshes[mesh_idx].nvertex;
	vertex=model->meshes[mesh_idx].vertexes;
	
	for (i=0;i!=nvertex;i++) {
		vertex->pnt.x+=xoff;
		vertex->pnt.z+=zoff;
		vertex++;
	}
}

void model_center_xz_all(model_type *model)
{
	int					i,n,nmesh,nvertex,nbone,xoff,zoff,
						minx,maxx,minz,maxz,miny,maxy;
	model_vertex_type	*vertex;
	model_bone_type		*bone;
	
	model_get_vertex_extent_all(model,&minx,&maxx,&minz,&maxz,&miny,&maxy);
    xoff=-(minx+maxx)/2;
    zoff=-(minz+maxz)/2;
	
		// center meshes
		
	nmesh=model->nmesh;
	
	for (i=0;i!=nmesh;i++) {
		nvertex=model->meshes[i].nvertex;
		vertex=model->meshes[i].vertexes;
	
		for (n=0;n!=nvertex;n++) {
			vertex->pnt.x+=xoff;
			vertex->pnt.z+=zoff;
			vertex++;
		}
	}
	
		// center bones
		
	nbone=model->nbone;
	bone=model->bones;
	
	for (i=0;i!=nbone;i++) {
		bone->pnt.x+=xoff;
		bone->pnt.z+=zoff;
		bone++;
	}

	model_calculate_parents(model);
}

/* =======================================================

      Floor Y
      
======================================================= */

void model_floor(model_type *model,int mesh_idx)
{
	int					i,nvertex,yoff,
                        minx,maxx,minz,maxz,miny,maxy;
	model_vertex_type	*vertex;

	model_get_vertex_extent(model,mesh_idx,&minx,&maxx,&minz,&maxz,&miny,&maxy);
    yoff=-maxy;
    
	nvertex=model->meshes[mesh_idx].nvertex;
	vertex=model->meshes[mesh_idx].vertexes;
	
	for (i=0;i!=nvertex;i++) {
		vertex->pnt.y+=yoff;
		vertex++;
	}
}

void model_floor_all(model_type *model)
{
	int					i,n,nmesh,nvertex,nbone,yoff,
                        minx,maxx,minz,maxz,miny,maxy;
	model_vertex_type	*vertex;
	model_bone_type		*bone;

	model_get_vertex_extent_all(model,&minx,&maxx,&minz,&maxz,&miny,&maxy);
    yoff=-maxy;
    
		// floor meshes
		
	nmesh=model->nmesh;
	
	for (i=0;i!=nmesh;i++) {
		nvertex=model->meshes[i].nvertex;
		vertex=model->meshes[i].vertexes;
		
		for (n=0;n!=nvertex;n++) {
			vertex->pnt.y+=yoff;
			vertex++;
		}
	}
	
		// floor bones
		
	nbone=model->nbone;
	bone=model->bones;
	
	for (i=0;i!=nbone;i++) {
		bone->pnt.y+=yoff;
		bone++;
	}

	model_calculate_parents(model);
}

/* =======================================================

      Flip UVs
      
======================================================= */

void model_flip_uv(model_type *model,int mesh_idx,bool flip_u,bool flip_v)
{
	int						i,k,npoly;
	model_poly_type			*poly;

	npoly=model->meshes[mesh_idx].npoly;
	poly=model->meshes[mesh_idx].polys;
	
	for (i=0;i!=npoly;i++) {
		for (k=0;k!=poly->ptsz;k++) {
			if (flip_u) poly->gx[k]=1.0f-poly->gx[k];
			if (flip_v) poly->gy[k]=1.0f-poly->gy[k];
		}
		poly++;
	}
}
