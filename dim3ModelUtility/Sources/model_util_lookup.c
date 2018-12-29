/****************************** File *********************************

Module: dim3 Model Utility
Author: Brian Barnes
 Usage: Lookup Routines

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

      Model Lookup Routines
      
======================================================= */

int model_find_mesh(model_type *model,char *mesh_name)
{
	int					n,nmesh;
	model_mesh_type		*mesh;
	
	nmesh=model->nmesh;
	mesh=model->meshes;
	
	for (n=0;n!=nmesh;n++) {
		if (strcasecmp(mesh->name,mesh_name)==0) return(n);
		mesh++;
	}
	
	return(-1);
}

int model_find_bone(model_type *model,char *bone_name)
{
	int					n,nbone;
	model_bone_type		*bone;
	
	nbone=model->nbone;
	bone=model->bones;
	
	for (n=0;n!=nbone;n++) {
		if (strcasecmp(bone->name,bone_name)==0) return(n);
		bone++;
	}
	
	return(-1);
}

int model_find_pose(model_type *model,char *pose_name)
{
	int					n,npose;
	model_pose_type		*pose;
	
	npose=model->npose;
	pose=model->poses;
	
	for (n=0;n!=npose;n++) {
		if (strcasecmp(pose->name,pose_name)==0) return(n);
		pose++;
	}
	
	return(-1);
}

int model_find_animate(model_type *model,char *animate_name)
{
	int					n,nanimate;
	model_animate_type	*animate;
	
	nanimate=model->nanimate;
	animate=model->animates;
	
	for (n=0;n!=nanimate;n++) {
		if (strcasecmp(animate->name,animate_name)==0) return(n);
		animate++;
	}
	
	return(-1);
}
