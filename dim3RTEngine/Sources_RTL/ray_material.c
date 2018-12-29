//
// dim3RTL
// by Brian Barnes (bbarnes@klinksoftware.com)
// free software offered as is for any use
//

#include "ray_os_defs.h"
#include "ray.h"
#include "ray_interface.h"

#include "png.h"

extern ray_global_type				ray_global;

/* =======================================================

      Find Material Index for ID
      
======================================================= */

int ray_material_get_index(int materialId)
{
	int						n;

	for (n=0;n!=ray_max_material;n++) {
		if (ray_global.material_list.materials[n]!=NULL) {
			if (ray_global.material_list.materials[n]->id==materialId) return(n);
		}
	}

	return(-1);
}

int ray_material_get_free_index(void)
{
	int						n;

	for (n=0;n!=ray_max_material;n++) {
		if (ray_global.material_list.materials[n]==NULL) return(n);
	}

	return(-1);
}

/* =======================================================

      Get Texture RGB
      
======================================================= */

inline int ray_get_material_find_mipmap_level(ray_scene_type *scene,ray_point_type *eye_pnt,ray_poly_type *poly,ray_point_type *trig_pnt,ray_material_type *material)
{
	int					mm_level;
	float				mm_dist,mm_fact;
	ray_point_type		mid;

		// note: there is potential threading
		// problems here, but they will only cause
		// a value to be set twice (to the same value.)
		// this is much faster than locking and doesn't
		// cause rendering errors

		// check cache

	if (poly->mm_level!=-1) return(poly->mm_level);

		// get the mid of the bounds and find the
		// distance to the eye

	mid.x=(poly->bound.min.x+poly->bound.max.x)*0.5f;
	mid.y=(poly->bound.min.y+poly->bound.max.y)*0.5f;
	mid.z=(poly->bound.min.z+poly->bound.max.z)*0.5f;

	mm_dist=ray_distance_between_points(eye_pnt,&mid);
	mm_fact=1.0f-(mm_dist/scene->eye.max_dist);

		// add in an exponential factor to
		// make it a less linear drop off

	mm_fact=1.0f-(mm_fact*powf(mm_fact,ray_mipmap_distance_exponent));

		// get the mipmap level

	mm_level=(int)(((float)(material->mipmap_list.count+1))*mm_fact);
	if (mm_level<0) mm_level=0;
	if (mm_level>=material->mipmap_list.count) mm_level=material->mipmap_list.count-1;

		// put it back into polygon so
		// we only calc it once

	poly->mm_level=mm_level;

	return(mm_level);
}

void ray_get_material_pixel(ray_scene_type *scene,ray_point_type *eye_pnt,ray_point_type *trig_pnt,ray_collision_type *collision,ray_material_pixel_type *pixel)
{
	int							x,y,offset,mm_level;
	unsigned long				buf;
	float						inv,fx,fy;
	ray_mesh_type				*mesh;
	ray_poly_type				*poly;
	ray_trig_type				*trig;
	ray_uv_type					*uv0,*uv1,*uv2;
	ray_vector_type				*n0,*n1,*n2;
	ray_material_type			*material;
	ray_material_mipmap_type	*mipmap;

		// get mesh/poly/trig and materials
		
	mesh=scene->mesh_list.meshes[collision->mesh_idx];
	poly=&mesh->poly_block.polys[collision->poly_idx];
	trig=&poly->trig_block.trigs[collision->trig_idx];
	
	material=ray_global.material_list.materials[poly->material_idx];

		// get the mipmap level

	mm_level=ray_get_material_find_mipmap_level(scene,eye_pnt,poly,trig_pnt,material);
	mipmap=&material->mipmap_list.mipmaps[mm_level];
	
		// sanity check for bad materials
		
	if (mipmap->data.color==NULL) {
		pixel->color.on=FALSE;
		pixel->normal.on=FALSE;
		pixel->specular.on=FALSE;
		return;
	}
	
		// calculate the uv

	uv0=&mesh->uv_block.uvs[trig->idxs[0].uv];
	uv1=&mesh->uv_block.uvs[trig->idxs[1].uv];
	uv2=&mesh->uv_block.uvs[trig->idxs[2].uv];
		
	inv=(1-collision->u)-collision->v;
	fx=(inv*uv0->x)+(collision->u*uv1->x)+(collision->v*uv2->x);
	fy=(inv*uv0->y)+(collision->u*uv1->y)+(collision->v*uv2->y);

		// calculate the surface normal
		
	if (mesh->normal_block.normals==NULL) {
		pixel->surface.normal.x=pixel->surface.normal.y=0.0f;
		pixel->surface.normal.z=-1.0f;
	}
	else {
		n0=&mesh->normal_block.normals[trig->idxs[0].normal];
		n1=&mesh->normal_block.normals[trig->idxs[1].normal];
		n2=&mesh->normal_block.normals[trig->idxs[2].normal];
			
		pixel->surface.normal.x=(inv*n0->x)+(collision->u*n1->x)+(collision->v*n2->x);
		pixel->surface.normal.y=(inv*n0->y)+(collision->u*n1->y)+(collision->v*n2->y);
		pixel->surface.normal.z=(inv*n0->z)+(collision->u*n1->z)+(collision->v*n2->z);
		
		ray_vector_normalize(&pixel->surface.normal);
	}

		// calculate the surface tangent
		
	if (mesh->tangent_block.tangents==NULL) {
		pixel->surface.tangent.x=pixel->surface.tangent.z=0.0f;
		pixel->surface.tangent.y=1.0f;
	}
	else {
		n0=&mesh->tangent_block.tangents[trig->idxs[0].tangent];
		n1=&mesh->tangent_block.tangents[trig->idxs[1].tangent];
		n2=&mesh->tangent_block.tangents[trig->idxs[2].tangent];
			
		pixel->surface.tangent.x=(inv*n0->x)+(collision->u*n1->x)+(collision->v*n2->x);
		pixel->surface.tangent.y=(inv*n0->y)+(collision->u*n1->y)+(collision->v*n2->y);
		pixel->surface.tangent.z=(inv*n0->z)+(collision->u*n1->z)+(collision->v*n2->z);
		
		ray_vector_normalize(&pixel->surface.tangent);
	}

		// calculate the binormal

	ray_vector_cross_product(&pixel->surface.binormal,&pixel->surface.normal,&pixel->surface.tangent);
	ray_vector_normalize(&pixel->surface.binormal);

		// change to texture coordinate

	fx-=floorf(fx);
	x=(int)(fx*mipmap->wid_scale);
	if (x>=mipmap->wid) x=mipmap->wid-1;

	fy-=floorf(fy);
	y=(int)(fy*mipmap->high_scale);
	if (y>=mipmap->high) y=mipmap->high-1;
	
	offset=(mipmap->wid*y)+x;

		// get color

	pixel->color.on=TRUE;
	buf=*(((unsigned long*)mipmap->data.color)+offset);
	ray_create_float_color_from_ulong(buf,&pixel->color.rgb);

		// add in the poly color

	pixel->color.rgb.r*=poly->col.r;
	pixel->color.rgb.g*=poly->col.g;
	pixel->color.rgb.b*=poly->col.b;
	pixel->color.rgb.a*=poly->col.a;
	
		// get normal
		
	if (mipmap->data.normal==NULL) {
		pixel->normal.on=FALSE;
	}
	else {
		pixel->normal.on=TRUE;
		buf=*(((unsigned long*)mipmap->data.normal)+offset);
		ray_create_float_color_from_ulong_no_alpha(buf,&pixel->normal.rgb);
	}
	
		// get specular
		
	if (mipmap->data.specular==NULL) {
		pixel->specular.on=FALSE;
	}
	else {
		pixel->specular.on=TRUE;
		pixel->shine_factor=material->shine_factor;
		buf=*(((unsigned long*)mipmap->data.specular)+offset);
		ray_create_float_color_from_ulong_no_alpha(buf,&pixel->specular.rgb);
	}

		// get glow

	if (mipmap->data.glow==NULL) {
		pixel->glow.on=FALSE;
	}
	else {
		pixel->glow.on=TRUE;
		buf=*(((unsigned long*)mipmap->data.glow)+offset);
		ray_create_float_color_from_ulong(buf,&pixel->glow.rgb);

		pixel->glow.rgb.r*=material->glow_factor;
		pixel->glow.rgb.g*=material->glow_factor;
		pixel->glow.rgb.b*=material->glow_factor;
	}
}

void ray_get_material_color(ray_scene_type *scene,ray_point_type *eye_pnt,ray_point_type *trig_pnt,ray_collision_type *collision,ray_color_type *col)
{
	int							x,y,offset,mm_level;
	unsigned long				buf;
	float						inv,fx,fy;
	ray_mesh_type				*mesh;
	ray_poly_type				*poly;
	ray_trig_type				*trig;
	ray_uv_type					*uv0,*uv1,*uv2;
	ray_material_type			*material;
	ray_material_mipmap_type	*mipmap;
	
		// get mesh/poly/trig and materials
		
	mesh=scene->mesh_list.meshes[collision->mesh_idx];
	poly=&mesh->poly_block.polys[collision->poly_idx];
	trig=&poly->trig_block.trigs[collision->trig_idx];
	material=ray_global.material_list.materials[poly->material_idx];

		// get the mipmap level

	mm_level=ray_get_material_find_mipmap_level(scene,eye_pnt,poly,trig_pnt,material);
	mipmap=&material->mipmap_list.mipmaps[mm_level];
	
		// sanity check for bad materials
		
	if (mipmap->data.color==NULL) {
		col->r=col->g=col->b=1.0f;
		col->a=0.0f;
		return;
	}
	
		// calculate the uv
		
	uv0=&mesh->uv_block.uvs[trig->idxs[0].uv];
	uv1=&mesh->uv_block.uvs[trig->idxs[1].uv];
	uv2=&mesh->uv_block.uvs[trig->idxs[2].uv];
		
	inv=(1-collision->u)-collision->v;
	fx=(inv*uv0->x)+(collision->u*uv1->x)+(collision->v*uv2->x);
	fy=(inv*uv0->y)+(collision->u*uv1->y)+(collision->v*uv2->y);

		// change to texture coordinate
		
	fx-=floorf(fx);
	x=(int)(fx*mipmap->wid_scale);

	fy-=floorf(fy);
	y=(int)(fy*mipmap->high_scale);
	
	offset=(mipmap->wid*y)+x;
	
		// get color

	buf=*(((unsigned long*)mipmap->data.color)+offset);
	ray_create_float_color_from_ulong(buf,col);

		// add in the poly color

	col->r*=poly->col.r;
	col->g*=poly->col.g;
	col->b*=poly->col.b;
	col->a*=poly->col.a;
}

float ray_get_material_alpha(ray_scene_type *scene,ray_point_type *eye_pnt,ray_point_type *trig_pnt,ray_collision_type *collision)
{
	int							x,y,offset;
	unsigned long				buf;
	float						inv,fx,fy;
	ray_mesh_type				*mesh;
	ray_poly_type				*poly;
	ray_trig_type				*trig;
	ray_uv_type					*uv0,*uv1,*uv2;
	ray_material_type			*material;
	ray_material_mipmap_type	*mipmap;
	ray_color_type				rgb;
	
		// get mesh/poly/trig and materials
		
	mesh=scene->mesh_list.meshes[collision->mesh_idx];
	poly=&mesh->poly_block.polys[collision->poly_idx];
	trig=&poly->trig_block.trigs[collision->trig_idx];

		// fast check for no alphas

	material=ray_global.material_list.materials[poly->material_idx];
	if (material->no_alpha) return(1.0f);

		// when getting the alpha, we
		// don't care about the mipmap level,
		// always get it from highest level

	mipmap=&material->mipmap_list.mipmaps[0];
	
		// sanity check for bad materials
		
	if (mipmap->data.color==NULL) return(1.0f);
	
		// calculate the uv
		
	uv0=&mesh->uv_block.uvs[trig->idxs[0].uv];
	uv1=&mesh->uv_block.uvs[trig->idxs[1].uv];
	uv2=&mesh->uv_block.uvs[trig->idxs[2].uv];
		
	inv=(1-collision->u)-collision->v;
	fx=(inv*uv0->x)+(collision->u*uv1->x)+(collision->v*uv2->x);
	fy=(inv*uv0->y)+(collision->u*uv1->y)+(collision->v*uv2->y);

		// change to texture coordinate
		
	fx-=floorf(fx);
	x=(int)(fx*mipmap->wid_scale);

	fy-=floorf(fy);
	y=(int)(fy*mipmap->high_scale);
	
	offset=(mipmap->wid*y)+x;
	
		// get alpha

	buf=*(((unsigned long*)mipmap->data.color)+offset);
	ray_create_float_color_from_ulong(buf,&rgb);

	return(rgb.a*poly->col.a);
}

void ray_get_material_normal(ray_scene_type *scene,ray_point_type *eye_pnt,ray_point_type *trig_pnt,ray_collision_type *collision,ray_vector_type *normal)
{
	float						inv,fx,fy;
	ray_mesh_type				*mesh;
	ray_poly_type				*poly;
	ray_trig_type				*trig;
	ray_uv_type					*uv0,*uv1,*uv2;
	ray_vector_type				*n0,*n1,*n2;

		// get mesh/poly/trig and materials
		
	mesh=scene->mesh_list.meshes[collision->mesh_idx];
	poly=&mesh->poly_block.polys[collision->poly_idx];
	trig=&poly->trig_block.trigs[collision->trig_idx];
	
		// calculate the uv

	uv0=&mesh->uv_block.uvs[trig->idxs[0].uv];
	uv1=&mesh->uv_block.uvs[trig->idxs[1].uv];
	uv2=&mesh->uv_block.uvs[trig->idxs[2].uv];
		
	inv=(1-collision->u)-collision->v;
	fx=(inv*uv0->x)+(collision->u*uv1->x)+(collision->v*uv2->x);
	fy=(inv*uv0->y)+(collision->u*uv1->y)+(collision->v*uv2->y);

		// calculate the surface normal
		
	if (mesh->normal_block.normals==NULL) {
		normal->x=normal->y=0.0f;
		normal->z=-1.0f;
	}
	else {
		n0=&mesh->normal_block.normals[trig->idxs[0].normal];
		n1=&mesh->normal_block.normals[trig->idxs[1].normal];
		n2=&mesh->normal_block.normals[trig->idxs[2].normal];
			
		normal->x=(inv*n0->x)+(collision->u*n1->x)+(collision->v*n2->x);
		normal->y=(inv*n0->y)+(collision->u*n1->y)+(collision->v*n2->y);
		normal->z=(inv*n0->z)+(collision->u*n1->z)+(collision->v*n2->z);
		
		ray_vector_normalize(normal);
	}
}

/* =======================================================

      Adds a New Material with No Attachments

	  Returns:
	   If >=0, then a material ID
	   RL_ERROR_OUT_OF_MEMORY
	   RL_ERROR_UNKNOWN_ALPHA_TYPE
	   RL_ERROR_TOO_MANY_MATERIALS
      
======================================================= */

int rtlMaterialAdd(int wid,int high,int alphaType,unsigned long flags)
{
	int							n,idx;
	ray_material_type			*material;
	ray_material_mipmap_type	*mipmap;

		// validate alpha type

	if ((alphaType!=RL_MATERIAL_ALPHA_PASS_THROUGH) && (alphaType!=RL_MATERIAL_ALPHA_REFLECT) && (alphaType!=RL_MATERIAL_ALPHA_REFRACT) && (alphaType!=RL_MATERIAL_ALPHA_ADDITIVE)) return(RL_ERROR_UNKNOWN_ALPHA_TYPE);
	
		// find free spot

	idx=ray_material_get_free_index();
	if (idx==-1) return(RL_ERROR_TOO_MANY_MATERIALS);

		// add material

	material=(ray_material_type*)malloc(sizeof(ray_material_type));
	if (material==NULL) return(RL_ERROR_OUT_OF_MEMORY);
		
		// no mipmaps yet

	material->mipmap_list.count=0;

		// original wid/high

	material->wid=wid;
	material->high=high;
	material->alpha_type=alphaType;
	material->no_alpha=TRUE;

		// start with no attachments

	mipmap=material->mipmap_list.mipmaps;

	for (n=0;n!=ray_mipmap_max_level;n++) {
		mipmap->data.color=NULL;
		mipmap->data.normal=NULL;
		mipmap->data.specular=NULL;
		mipmap->data.glow=NULL;
		mipmap++;
	}

		// default factors

	material->shine_factor=1.0f;
	material->refract_factor=1.25f;

		// set id

	material->id=ray_global.material_list.next_id;
	ray_global.material_list.next_id++;

		// add to list

	ray_global.material_list.materials[idx]=material;
	
	return(material->id);
}

/* =======================================================

      Get the count of Materials

	  Returns:
	   Material Count
      
======================================================= */

int rtlMaterialCount(void)
{
	int				n,count;

	count=0;

	for (n=0;n!=ray_max_material;n++) {
		if (ray_global.material_list.materials[n]!=NULL) count++;
	}

	return(count);
}

/* =======================================================

      Delete Material

	  Returns:
	   RL_ERROR_OK
	   RL_ERROR_UNKNOWN_MATERIAL_ID
	   RL_ERROR_MATERIAL_ATTACHED_TO_MESH
	   RL_ERROR_MATERIAL_ATTACHED_TO_OVERLAY
      
======================================================= */

int rtlMaterialDelete(int materialId)
{
	int							n,k,t,idx;
	ray_mesh_type				*mesh;
	ray_poly_type				*poly;
	ray_material_type			*material;
	ray_material_mipmap_type	*mipmap;
	ray_scene_type				*scene;

		// get material

	idx=ray_material_get_index(materialId);
	if (idx==-1) return(RL_ERROR_UNKNOWN_MATERIAL_ID);

	material=ray_global.material_list.materials[idx];

		// check to see if held by a mesh
		// or overlay

	for (n=0;n!=ray_global.scene_list.count;n++) {
		scene=ray_global.scene_list.scenes[n];

		for (k=0;k!=scene->mesh_list.count;k++) {
			mesh=scene->mesh_list.meshes[k];

			poly=mesh->poly_block.polys;			// don't need to parent, we will check the parents as we get them

			for (t=0;t!=mesh->poly_block.count;t++) {
				if (poly->material_idx==idx) return(RL_ERROR_MATERIAL_ATTACHED_TO_MESH);
				poly++;
			}
		}
	}

		// clear buffers

	mipmap=material->mipmap_list.mipmaps;

	for (n=0;n!=material->mipmap_list.count;n++) {
		if (mipmap->data.color!=NULL) free(mipmap->data.color);
		if (mipmap->data.normal!=NULL) free(mipmap->data.normal);
		if (mipmap->data.specular!=NULL) free(mipmap->data.specular);
		if (mipmap->data.glow!=NULL) free(mipmap->data.glow);
		mipmap++;
	}

		// remove material

	free(material);

	ray_global.material_list.materials[idx]=NULL;

	return(RL_ERROR_OK);
}

/* =======================================================

      Deletes all Materials

	  Returns:
	   RL_ERROR_OK
	   RL_ERROR_MATERIAL_ATTACHED_TO_MESH
	   RL_ERROR_MATERIAL_ATTACHED_TO_OVERLAY
      
======================================================= */

int rtlMaterialDeleteAll(void)
{
	int				n;
	int				err;

	for (n=0;n!=ray_max_material;n++) {
		if (ray_global.material_list.materials[n]!=NULL) {
			err=rtlMaterialDelete(ray_global.material_list.materials[n]->id);
			if (err!=RL_ERROR_OK) return(err);
		}
	}
	
	return(RL_ERROR_OK);
}

/* =======================================================

      Attach a Material Target from a Memory Block

	  Returns:
	   RL_ERROR_OK
	   RL_ERROR_UNKNOWN_MATERIAL_ID
	   RL_ERROR_OUT_OF_MEMORY
	   RL_ERROR_UNKNOWN_TARGET
	   RL_ERROR_UNKNOWN_FORMAT
      
======================================================= */

int rtlMaterialAttachBufferData(int materialId,int target,int format,unsigned char* data)
{
	int							n,pixel_cnt,idx,sz;
	unsigned char				*rgba_data,*rptr,*dptr;
	ray_material_type			*material;
	ray_material_mipmap_type	*mipmap;

		// get material

	idx=ray_material_get_index(materialId);
	if (idx==-1) return(RL_ERROR_UNKNOWN_MATERIAL_ID);

	material=ray_global.material_list.materials[idx];

		// validate target and format

	if ((target!=RL_MATERIAL_TARGET_COLOR) && (target!=RL_MATERIAL_TARGET_NORMAL) && (target!=RL_MATERIAL_TARGET_SPECULAR) && (target!=RL_MATERIAL_TARGET_GLOW)) return(RL_ERROR_UNKNOWN_TARGET);
	if ((format!=RL_MATERIAL_FORMAT_32_RGBA) && (format!=RL_MATERIAL_FORMAT_24_RGB)) return(RL_ERROR_UNKNOWN_FORMAT);

		// setup the alpha flag
		// we use this to quickly skip out
		// of alpha comparisons.
		// supergumba -- in the future maybe scan
		// RGBA textures in case they are uploaded with
		// black alpha

	if (target==RL_MATERIAL_TARGET_COLOR) {
		material->no_alpha=(format==RL_MATERIAL_FORMAT_24_RGB);
	}

		// memory for rgba data
		
	sz=(material->wid*material->high)<<2;
	
	rgba_data=malloc(sz);
	if (rgba_data==NULL) return(RL_ERROR_OUT_OF_MEMORY);
	
		// all materials are in 32 bit RGBA
		// if material is not that format, than
		// we need to translate
		
	if (format==RL_MATERIAL_FORMAT_32_RGBA) {
		memmove(rgba_data,data,sz);
	}
	else {
	
		rptr=rgba_data;
		dptr=data;
		
		pixel_cnt=material->wid*material->high;
		
		for (n=0;n!=pixel_cnt;n++) {
			*rptr++=*dptr++;
			*rptr++=*dptr++;
			*rptr++=*dptr++;
			*rptr++=0xFF;
		}
	}

		// any buffer attachment clears
		// all mipmap data

	mipmap=&material->mipmap_list.mipmaps[1];

	for (n=1;n!=ray_mipmap_max_level;n++) {
		if (mipmap->data.color!=NULL) free(mipmap->data.color);
		if (mipmap->data.normal!=NULL) free(mipmap->data.normal);
		if (mipmap->data.specular!=NULL) free(mipmap->data.specular);
		if (mipmap->data.glow!=NULL) free(mipmap->data.glow);
		mipmap++;
	}

	material->mipmap_list.count=1;

		// attachments are always mipmap
		// level 0

	mipmap=&material->mipmap_list.mipmaps[0];

		// build the UV texturing data

	mipmap->wid=material->wid;
	mipmap->high=material->high;
	
	mipmap->wid_scale=(float)mipmap->wid;
	mipmap->high_scale=(float)mipmap->high;
		
		// save new material target
	
	switch (target) {
	
		case RL_MATERIAL_TARGET_COLOR:
			if (mipmap->data.color!=NULL) free(mipmap->data.color);
			mipmap->data.color=rgba_data;
			break;
			
		case RL_MATERIAL_TARGET_NORMAL:
			if (mipmap->data.normal!=NULL) free(mipmap->data.normal);
			mipmap->data.normal=rgba_data;
			break;
			
		case RL_MATERIAL_TARGET_SPECULAR:
			if (mipmap->data.specular!=NULL) free(mipmap->data.specular);
			mipmap->data.specular=rgba_data;
			break;
			
		case RL_MATERIAL_TARGET_GLOW:
			if (mipmap->data.glow!=NULL) free(mipmap->data.glow);
			mipmap->data.glow=rgba_data;
			break;
			
	}
	
	return(RL_ERROR_OK);
}

/* =======================================================

      Attach a Material Target from a Solid Color

	  Returns:
	   RL_ERROR_OK
	   RL_ERROR_UNKNOWN_MATERIAL_ID
	   RL_ERROR_OUT_OF_MEMORY
      
======================================================= */

int rtlMaterialAttachBufferColor(int materialId,int target,ray_color_type *col)
{
	int						n,sz,idx,err;
	unsigned long			pixel;
	unsigned long			*pd,*pixel_data;
	ray_material_type		*material;

		// get material

	idx=ray_material_get_index(materialId);
	if (idx==-1) return(RL_ERROR_UNKNOWN_MATERIAL_ID);

	material=ray_global.material_list.materials[idx];

		// create the color block

	sz=material->wid*material->high;

	pixel_data=(unsigned long*)malloc(sz<<2);
	if (pixel_data==NULL) return(RL_ERROR_OUT_OF_MEMORY);

	pd=pixel_data;
	
	pixel=ray_create_ulong_color_from_float(col);

	for (n=0;n!=sz;n++) {
		*pd++=pixel;
	}

		// attach to material

	err=rtlMaterialAttachBufferData(materialId,target,RL_MATERIAL_FORMAT_32_RGBA,(unsigned char*)pixel_data);

	free(pixel_data);

	return(err);
}

/* =======================================================

      Sets the Shine Factor for Speculars

	  Returns:
	   RL_ERROR_OK
	   RL_ERROR_UNKNOWN_MATERIAL_ID
      
======================================================= */

int rtlMaterialSetShineFactor(int materialId,float shineFactor)
{
	int						idx;
	ray_material_type		*material;

		// get material

	idx=ray_material_get_index(materialId);
	if (idx==-1) return(RL_ERROR_UNKNOWN_MATERIAL_ID);

	material=ray_global.material_list.materials[idx];

		// set the shine

	material->shine_factor=shineFactor;
	
	return(RL_ERROR_OK);
}

/* =======================================================

      Sets the Glow Factor for Glows

	  Returns:
	   RL_ERROR_OK
	   RL_ERROR_UNKNOWN_MATERIAL_ID
      
======================================================= */

int rtlMaterialSetGlowFactor(int materialId,float glowFactor)
{
	int						idx;
	ray_material_type		*material;

		// get material

	idx=ray_material_get_index(materialId);
	if (idx==-1) return(RL_ERROR_UNKNOWN_MATERIAL_ID);

	material=ray_global.material_list.materials[idx];

		// set the glow

	material->glow_factor=glowFactor;
	
	return(RL_ERROR_OK);
}

/* =======================================================

      Sets the Refraction Factor for Alphas

	  Returns:
	   RL_ERROR_OK
	   RL_ERROR_UNKNOWN_MATERIAL_ID
      
======================================================= */

int rtlMaterialSetRefractionFactor(int materialId,float refractionFactor)
{
	int						idx;
	ray_material_type		*material;

		// get material

	idx=ray_material_get_index(materialId);
	if (idx==-1) return(RL_ERROR_UNKNOWN_MATERIAL_ID);

	material=ray_global.material_list.materials[idx];

		// set the refraction

	material->refract_factor=refractionFactor;
	
	return(RL_ERROR_OK);
}

/* =======================================================

      Builds Mipmaps For Material

	  Returns:
	   RL_ERROR_OK
	   RL_ERROR_UNKNOWN_MATERIAL_ID
	   RL_ERROR_OUT_OF_MEMORY
      
======================================================= */

int rtlMaterialBuildMipMaps(int materialId)
{
	int							idx,factor;
	ray_material_type			*material;
	ray_material_mipmap_type	*mipmap;

		// get material

	idx=ray_material_get_index(materialId);
	if (idx==-1) return(RL_ERROR_UNKNOWN_MATERIAL_ID);

	material=ray_global.material_list.materials[idx];

		// build the mipmaps

	factor=2;

	while (TRUE) {
			
			// have we reached the smallest
			// mipmap yet?

		if ((material->wid/factor)<ray_mipmap_min_dimension) break;
		if ((material->high/factor)<ray_mipmap_min_dimension) break;
		
			// new mipmap

		mipmap=&material->mipmap_list.mipmaps[material->mipmap_list.count];

		mipmap->wid=material->wid/factor;
		mipmap->high=material->high/factor;
		
		mipmap->wid_scale=(float)mipmap->wid;
		mipmap->high_scale=(float)mipmap->high;

			// color data

		mipmap->data.color=ray_bitmap_reduction(factor,material->wid,material->high,material->mipmap_list.mipmaps[0].data.color);
		if (mipmap->data.color==NULL) return(RL_ERROR_OUT_OF_MEMORY);

		if (material->mipmap_list.mipmaps[0].data.normal!=NULL) {
			mipmap->data.normal=ray_bitmap_reduction(factor,material->wid,material->high,material->mipmap_list.mipmaps[0].data.normal);
			if (mipmap->data.normal==NULL) {
				free(mipmap->data.color);
				return(RL_ERROR_OUT_OF_MEMORY);
			}
		}

		if (material->mipmap_list.mipmaps[0].data.specular!=NULL) {
			mipmap->data.specular=ray_bitmap_reduction(factor,material->wid,material->high,material->mipmap_list.mipmaps[0].data.specular);
			if (mipmap->data.specular==NULL) {
				free(mipmap->data.color);
				if (mipmap->data.normal!=NULL) free(mipmap->data.normal);
				return(RL_ERROR_OUT_OF_MEMORY);
			}
		}

		if (material->mipmap_list.mipmaps[0].data.glow!=NULL) {
			mipmap->data.glow=ray_bitmap_reduction(factor,material->wid,material->high,material->mipmap_list.mipmaps[0].data.glow);
			if (mipmap->data.glow==NULL) {
				free(mipmap->data.color);
				if (mipmap->data.normal!=NULL) free(mipmap->data.normal);
				if (mipmap->data.specular!=NULL) free(mipmap->data.specular);
				return(RL_ERROR_OUT_OF_MEMORY);
			}
		}

			// completed a mipmap, move up the levels

		factor*=2;
		material->mipmap_list.count++;
		if (material->mipmap_list.count>=ray_mipmap_max_level) break;
	}

	return(RL_ERROR_OK);
}

