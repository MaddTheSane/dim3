/****************************** File *********************************

Module: dim3 Auto Generator
Author: Brian Barnes
 Usage: Model Auto Generation Pieces

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

#define ag_model_piece_hand_y_size			200
#define ag_model_piece_hand_y_size_extra	150

#define ag_model_piece_foot_x_size			100
#define ag_model_piece_foot_x_size_extra	50
#define ag_model_piece_foot_z_size			250
#define ag_model_piece_foot_z_size_extra	250

int 					ag_model_piece_hand_high,
						ag_model_piece_foot_x_sz,ag_model_piece_foot_z_front;

/* =======================================================

      Set Vertex and Polys
      
======================================================= */

void ag_model_piece_add_vertex(model_type *model,model_vertex_type *vertex,int center_bone_idx,int major_bone_idx,int minor_bone_idx,float bone_factor,int x_off,int y_off,int z_off)
{
	model_bone_type		*bone;

	bone=&model->bones[center_bone_idx];

	vertex->pnt.x=bone->pnt.x+x_off;
	vertex->pnt.y=bone->pnt.y+y_off;
	vertex->pnt.z=bone->pnt.z+z_off;

		// bone connection

	vertex->major_bone_idx=major_bone_idx;
	vertex->minor_bone_idx=minor_bone_idx;
	vertex->bone_factor=bone_factor;

		// tangent space

	vector_create(&vertex->tangent_space.normal,vertex->pnt.x,vertex->pnt.y,vertex->pnt.z,bone->pnt.x,bone->pnt.y,bone->pnt.z);
}

void ag_model_piece_add_vertex_by_point(model_type *model,model_vertex_type *vertex,d3pnt *pnt,d3pnt *radius,float ang_y,int major_bone_idx,int minor_bone_idx,float bone_factor)
{
	float				fx,fy,fz;
	matrix_type			mat;

		// build the rotations

	matrix_rotate_xyz(&mat,0.0f,ang_y,0.0f);
	
	fx=1.0f;
	fy=0.0f;
	fz=0.0f;
	
	matrix_vertex_multiply(&mat,&fx,&fy,&fz);

	vertex->pnt.x=pnt->x+(int)(fx*((float)radius->x));
	vertex->pnt.y=pnt->y;
	vertex->pnt.z=pnt->z+(int)(fz*((float)radius->z));

		// bone connection

	vertex->major_bone_idx=major_bone_idx;
	vertex->minor_bone_idx=minor_bone_idx;
	vertex->bone_factor=bone_factor;

		// tangent space

	vector_create(&vertex->tangent_space.normal,vertex->pnt.x,vertex->pnt.y,vertex->pnt.z,pnt->x,pnt->y,pnt->z);
}

void ag_model_piece_add_quad(model_type *model,model_poly_type *poly,int v_off,int v1_idx,int v2_idx,int v3_idx,int v4_idx,float gx,float gx_sz,float gy,float gy_sz)
{
	poly->ptsz=4;
	poly->txt_idx=0;

	poly->v[0]=v_off+v1_idx;
	poly->v[1]=v_off+v2_idx;
	poly->v[2]=v_off+v3_idx;
	poly->v[3]=v_off+v4_idx;

	poly->gx[0]=poly->gx[3]=gx;
	poly->gx[1]=poly->gx[2]=gx+gx_sz;
	poly->gy[0]=poly->gy[1]=gy;
	poly->gy[2]=poly->gy[3]=gy+gy_sz;
}

void ag_model_piece_add_trig(model_type *model,model_poly_type *poly,int v_off,int v1_idx,int v2_idx,int v3_idx)
{
	poly->ptsz=3;
	poly->txt_idx=0;

	poly->v[0]=v_off+v1_idx;
	poly->v[1]=v_off+v2_idx;
	poly->v[2]=v_off+v3_idx;

	poly->gx[0]=poly->gx[1]=0.0f;
	poly->gx[2]=0.5f;
	poly->gy[0]=poly->gy[1]=0.0f;
	poly->gy[2]=0.5f;
}

/* =======================================================

      Build Poly Objects
      
======================================================= */

void ag_model_piece_complex_cylinder(model_type *model,model_mesh_type *mesh,int p_major_bone_idx,int p_minor_bone_idx,d3pnt *spt,d3pnt *ept,int stack_count,d3pnt *radius,d3pnt *radius_extra,float gx_offset,float gy_offset,float cap_gx_offset,float cap_gy_offset,bool force_end_size,bool no_end_cap)
{
	int					n,k,v_idx,vc_idx,major_bone_idx,minor_bone_idx,
						vertex_idx,poly_idx,va_idx[4];
	float				bone_factor,ang,
						gx,gy,gx_add,gy_add,uv_x[8],uv_y[8];
	d3pnt				pt,rnd_rad;
	model_vertex_type	*vertex;
	model_poly_type		*poly;

		// add the vertexes and polys
		// need 1 extra for top row of vertexes

	vertex_idx=mesh->nvertex;
	model_mesh_set_vertex_count(model,0,(vertex_idx+(8*(stack_count+1))));

	poly_idx=mesh->npoly;
	model_mesh_set_poly_count(model,0,(poly_idx+((8*stack_count)+(2*6))));

		// build the vertexes
	
	vertex=&mesh->vertexes[vertex_idx];

	for (n=0;n!=(stack_count+1);n++) {

			// get position

		pt.x=spt->x+(((ept->x-spt->x)*n)/stack_count);
		pt.y=spt->y+(((ept->y-spt->y)*n)/stack_count);
		pt.z=spt->z+(((ept->z-spt->z)*n)/stack_count);

			// randomize x-z coordinates

		rnd_rad.x=radius->x;
		rnd_rad.z=radius->z;

		if (!((force_end_size) && ((n==0) || (n==stack_count)))) {
			rnd_rad.x+=ag_random_int(radius_extra->x);
			rnd_rad.z+=ag_random_int(radius_extra->z);
		}

			// get attached bone
		
		major_bone_idx=p_major_bone_idx;
		minor_bone_idx=-1;
		bone_factor=1.0f;

		if (minor_bone_idx!=-1) {
			if (n==stack_count) {
				major_bone_idx=p_minor_bone_idx;
				minor_bone_idx=-1;
				bone_factor=1.0f;
			}
			else {
				major_bone_idx=p_major_bone_idx;
				minor_bone_idx=p_minor_bone_idx;
				bone_factor=1.0f-((float)n)/((float)stack_count);
			}
		}

			// the vertexes

		ag_model_piece_add_vertex_by_point(model,vertex++,&pt,&rnd_rad,0.0f,major_bone_idx,minor_bone_idx,bone_factor);
		ag_model_piece_add_vertex_by_point(model,vertex++,&pt,&rnd_rad,45.0f,major_bone_idx,minor_bone_idx,bone_factor);
		ag_model_piece_add_vertex_by_point(model,vertex++,&pt,&rnd_rad,90.0f,major_bone_idx,minor_bone_idx,bone_factor);
		ag_model_piece_add_vertex_by_point(model,vertex++,&pt,&rnd_rad,135.0f,major_bone_idx,minor_bone_idx,bone_factor);
		ag_model_piece_add_vertex_by_point(model,vertex++,&pt,&rnd_rad,180.0f,major_bone_idx,minor_bone_idx,bone_factor);
		ag_model_piece_add_vertex_by_point(model,vertex++,&pt,&rnd_rad,225.0f,major_bone_idx,minor_bone_idx,bone_factor);
		ag_model_piece_add_vertex_by_point(model,vertex++,&pt,&rnd_rad,270.0f,major_bone_idx,minor_bone_idx,bone_factor);
		ag_model_piece_add_vertex_by_point(model,vertex++,&pt,&rnd_rad,315.0f,major_bone_idx,minor_bone_idx,bone_factor);
	}

		// build the side polys

	poly=&mesh->polys[poly_idx];

	v_idx=0;

	gx_add=0.5f/8.0f;
	gy_add=0.5f/((float)stack_count);

	gy=gy_offset;

	for (n=0;n!=stack_count;n++) {

		va_idx[0]=0;
		va_idx[1]=1;
		va_idx[2]=9;
		va_idx[3]=8;

		gx=gx_offset;

		for (k=0;k!=8;k++) {
			ag_model_piece_add_quad(model,poly++,vertex_idx,(v_idx+va_idx[0]),(v_idx+va_idx[1]),(v_idx+va_idx[2]),(v_idx+va_idx[3]),gx,gx_add,gy,gy_add);

			va_idx[0]++;
			va_idx[1]++;
			if (va_idx[1]==8) va_idx[1]=0;
			va_idx[2]++;
			if (va_idx[2]==16) va_idx[2]=8;
			va_idx[3]++;

			gx+=gx_add;
		}

		v_idx+=8;
		gy+=gy_add;
	}

		// skip if no close

	if (no_end_cap) return;

		// top and bottom UVs

	for (n=0;n!=8;n++) {
		ang=(TRIG_PI/8.0f)*(float)n;
		uv_x[n]=cap_gx_offset+fabsf(0.5f*sinf(ang));
		uv_y[n]=cap_gy_offset+fabsf(0.5f*cosf(ang));
	}

		// build the top and bottom cap polys

	v_idx=vc_idx=vertex_idx;

	for (n=0;n!=6;n++) {
		poly->ptsz=3;
		poly->txt_idx=0;

		poly->v[0]=vc_idx;
		poly->v[1]=v_idx+1;
		poly->v[2]=v_idx+2;

		poly->gx[0]=uv_x[0];
		poly->gy[0]=uv_y[0];
		poly->gx[1]=uv_x[n+1];
		poly->gy[1]=uv_y[n+1];
		poly->gx[2]=uv_x[n+2];
		poly->gy[2]=uv_y[n+2];

		poly++;
		v_idx++;
	}

	v_idx=vc_idx=vertex_idx+(stack_count*8);

	for (n=0;n!=6;n++) {
		poly->ptsz=3;
		poly->txt_idx=0;

		poly->v[0]=vc_idx;
		poly->v[1]=v_idx+1;
		poly->v[2]=v_idx+2;

		poly->gx[0]=uv_x[0];
		poly->gy[0]=uv_y[0];
		poly->gx[1]=uv_x[n+1];
		poly->gy[1]=uv_y[n+1];
		poly->gx[2]=uv_x[n+2];
		poly->gy[2]=uv_y[n+2];

		poly++;
		v_idx++;
	}
}

/* =======================================================

      Piece Setups
      
======================================================= */

void ag_model_piece_setup(void)
{
		// setup certain random elements early so
		// they are the same across all times they are
		// used

	ag_model_piece_hand_high=ag_model_piece_hand_y_size+ag_random_int(ag_model_piece_hand_y_size_extra);

	ag_model_piece_foot_x_sz=ag_model_piece_foot_x_size+ag_random_int(ag_model_piece_foot_x_size_extra);
	ag_model_piece_foot_z_front=-(ag_model_piece_foot_z_size+ag_random_int(ag_model_piece_foot_z_size_extra));
}

/* =======================================================

      Head
      
======================================================= */

void ag_model_piece_bone_head(model_type *model,model_mesh_type *mesh,int bone_idx,int limb_radius)
{
	int				stack_count;
	d3pnt			spt,ept,radius,radius_extra;
	model_bone_type	*bone;

		// top to bottom of head

	bone=&model->bones[bone_idx];

	spt.x=bone->pnt.x;
	spt.y=bone->pnt.y-(350+ag_random_int(350));
	spt.z=bone->pnt.z;

	ept.x=bone->pnt.x;
	ept.y=bone->pnt.y;
	ept.z=bone->pnt.z;

		// radius

	radius.x=radius.z=150;
	radius_extra.x=radius_extra.z=250;

		// how many stacks?

	stack_count=2+ag_random_int(3);

		// create the cylinder

	ag_model_piece_complex_cylinder(model,mesh,bone_idx,-1,&spt,&ept,stack_count,&radius,&radius_extra,0.5f,0.0f,0.0f,0.0f,FALSE,FALSE);
}

/* =======================================================

      Head
      
======================================================= */

void ag_model_piece_bone_body(model_type *model,model_mesh_type *mesh,int body_bone_idx,int hip_bone_idx,int limb_radius)
{
	int					n,stack_count,
						major_bone_idx,minor_bone_idx;
	bool				chk_bne;
	d3pnt				spt,ept,min,max,radius,radius_extra;
	model_bone_type		*bone;

		// find body size by bones

	min.x=min.y=spt.z=0;
	max.x=max.y=max.z=0;

	major_bone_idx=minor_bone_idx=-1;

		// always start with torso
		// bone

	bone=model->bones;

	for (n=0;n!=model->nbone;n++) {
		if (strcmp(bone->name,"Torso")==0) {
			major_bone_idx=n;
			memmove(&min,&bone->pnt,sizeof(d3pnt));
			memmove(&max,&bone->pnt,sizeof(d3pnt));
		}
		if (strcmp(bone->name,"Hips")==0) {
			minor_bone_idx=n;
		}
		bone++;
	}

		// change by other bone positions

	bone=model->bones;

	for (n=0;n!=model->nbone;n++) {

		chk_bne=FALSE;

		chk_bne|=(strcmp(bone->name,"Left Shoulder")==0);
		chk_bne|=(strcmp(bone->name,"Right Shoulder")==0);
		chk_bne|=(strcmp(bone->name,"Hips")==0);
		chk_bne|=(strcmp(bone->name,"Left Hip")==0);
		chk_bne|=(strcmp(bone->name,"Right Hip")==0);

		if (chk_bne) {
			if (bone->pnt.x<min.x) min.x=bone->pnt.x;
			if (bone->pnt.y<min.y) min.y=bone->pnt.y;
			if (bone->pnt.z<min.z) min.z=bone->pnt.z;

			if (bone->pnt.x>max.x) max.x=bone->pnt.x;
			if (bone->pnt.y>max.y) max.y=bone->pnt.y;
			if (bone->pnt.z>max.z) max.z=bone->pnt.z;
		}

		bone++;
	}

	if ((major_bone_idx==-1) || (minor_bone_idx==-1)) return;

		// get the start/end points

	bone=&model->bones[major_bone_idx];
	memmove(&spt,&bone->pnt,sizeof(d3pnt));

	bone=&model->bones[minor_bone_idx];
	memmove(&ept,&bone->pnt,sizeof(d3pnt));

	ept.y=max.y;		// need to go down to hips

		// get radius

	radius.x=(abs(max.x-min.x)>>1)-100;
	radius.z=limb_radius;

	radius_extra.x=100;
	radius_extra.z=limb_radius;

		// how many stacks?

	stack_count=2+ag_random_int(3);

		// create the cylinder

	ag_model_piece_complex_cylinder(model,mesh,major_bone_idx,minor_bone_idx,&spt,&ept,stack_count,&radius,&radius_extra,0.5f,0.5f,0.5f,0.5f,FALSE,FALSE);
}

/* =======================================================

      Hand
      
======================================================= */

void ag_model_piece_bone_hand(model_type *model,model_mesh_type *mesh,int bone_idx,int limb_radius)
{
	int					vertex_idx,poly_idx,center_bone_idx,
						y_mid,y_bot;
	model_vertex_type	*vertex;
	model_poly_type		*poly;

		// get wrist bone

	center_bone_idx=model->bones[bone_idx].parent_idx;
	if (center_bone_idx==-1) return;

		// add the vertexes and polys

	vertex_idx=mesh->nvertex;
	model_mesh_set_vertex_count(model,0,(vertex_idx+12));

	poly_idx=mesh->npoly;
	model_mesh_set_poly_count(model,0,(poly_idx+10));

		// y and z coordinates

	y_mid=ag_model_piece_hand_high>>1;
	y_bot=ag_model_piece_hand_high;

		// build the vertexes
	
	vertex=&mesh->vertexes[vertex_idx];

	ag_model_piece_add_vertex(model,vertex++,center_bone_idx,bone_idx,-1,1.0f,limb_radius,0,-limb_radius);
	ag_model_piece_add_vertex(model,vertex++,center_bone_idx,bone_idx,-1,1.0f,limb_radius,0,limb_radius);
	ag_model_piece_add_vertex(model,vertex++,center_bone_idx,bone_idx,-1,1.0f,limb_radius,y_mid,(limb_radius+50));
	ag_model_piece_add_vertex(model,vertex++,center_bone_idx,bone_idx,-1,1.0f,limb_radius,y_bot,limb_radius);
	ag_model_piece_add_vertex(model,vertex++,center_bone_idx,bone_idx,-1,1.0f,limb_radius,y_bot,-limb_radius);
	ag_model_piece_add_vertex(model,vertex++,center_bone_idx,bone_idx,-1,1.0f,limb_radius,y_mid,-(limb_radius+50));

	ag_model_piece_add_vertex(model,vertex++,center_bone_idx,bone_idx,-1,1.0f,-limb_radius,0,-limb_radius);
	ag_model_piece_add_vertex(model,vertex++,center_bone_idx,bone_idx,-1,1.0f,-limb_radius,0,limb_radius);
	ag_model_piece_add_vertex(model,vertex++,center_bone_idx,bone_idx,-1,1.0f,-limb_radius,y_mid,(limb_radius+50));
	ag_model_piece_add_vertex(model,vertex++,center_bone_idx,bone_idx,-1,1.0f,-limb_radius,y_bot,limb_radius);
	ag_model_piece_add_vertex(model,vertex++,center_bone_idx,bone_idx,-1,1.0f,-limb_radius,y_bot,-limb_radius);
	ag_model_piece_add_vertex(model,vertex++,center_bone_idx,bone_idx,-1,1.0f,-limb_radius,y_mid,-(limb_radius+50));

		// build the polys

	poly=&mesh->polys[poly_idx];

	ag_model_piece_add_quad(model,poly++,vertex_idx,0,1,7,6,0.0f,0.5f,0.5f,1.0f);
	ag_model_piece_add_quad(model,poly++,vertex_idx,1,2,8,7,0.0f,0.5f,0.5f,1.0f);
	ag_model_piece_add_quad(model,poly++,vertex_idx,2,3,9,8,0.0f,0.5f,0.5f,1.0f);
	ag_model_piece_add_quad(model,poly++,vertex_idx,3,4,10,9,0.0f,0.5f,0.5f,1.0f);
	ag_model_piece_add_quad(model,poly++,vertex_idx,4,5,11,10,0.0f,0.5f,0.5f,1.0f);
	ag_model_piece_add_quad(model,poly++,vertex_idx,5,0,6,11,0.0f,0.5f,0.5f,1.0f);

	ag_model_piece_add_quad(model,poly++,vertex_idx,0,1,2,5,0.0f,0.5f,0.5f,1.0f);
	ag_model_piece_add_quad(model,poly++,vertex_idx,2,3,4,5,0.0f,0.5f,0.5f,1.0f);
	ag_model_piece_add_quad(model,poly++,vertex_idx,6,7,8,11,0.0f,0.5f,0.5f,1.0f);
	ag_model_piece_add_quad(model,poly++,vertex_idx,8,9,10,11,0.0f,0.5f,0.5f,1.0f);
}

/* =======================================================

      Feet
      
======================================================= */

void ag_model_piece_bone_foot(model_type *model,model_mesh_type *mesh,int bone_idx,int limb_radius)
{
	int					vertex_idx,poly_idx,center_bone_idx,
						z_mid,z_back,y_bot,y_mid,high;
	model_vertex_type	*vertex;
	model_poly_type		*poly;

		// get ankle bone

	center_bone_idx=model->bones[bone_idx].parent_idx;
	if (center_bone_idx==-1) return;

		// add the vertexes and polys

	vertex_idx=mesh->nvertex;
	model_mesh_set_vertex_count(model,0,(vertex_idx+12));

	poly_idx=mesh->npoly;
	model_mesh_set_poly_count(model,0,(poly_idx+10));

		// y and z coordinates

	z_mid=-(limb_radius<<1);
	z_back=limb_radius;

	high=-model->bones[center_bone_idx].pnt.y;

	y_mid=high>>1;
	y_bot=high;

		// build the vertexes
	
	vertex=&mesh->vertexes[vertex_idx];

	ag_model_piece_add_vertex(model,vertex++,center_bone_idx,bone_idx,-1,1.0f,ag_model_piece_foot_x_sz,y_mid,ag_model_piece_foot_z_front);
	ag_model_piece_add_vertex(model,vertex++,center_bone_idx,bone_idx,-1,1.0f,ag_model_piece_foot_x_sz,0,z_mid);
	ag_model_piece_add_vertex(model,vertex++,center_bone_idx,bone_idx,-1,1.0f,ag_model_piece_foot_x_sz,0,z_back);
	ag_model_piece_add_vertex(model,vertex++,center_bone_idx,bone_idx,-1,1.0f,ag_model_piece_foot_x_sz,y_bot,z_back);
	ag_model_piece_add_vertex(model,vertex++,center_bone_idx,bone_idx,-1,1.0f,ag_model_piece_foot_x_sz,y_bot,z_mid);
	ag_model_piece_add_vertex(model,vertex++,center_bone_idx,bone_idx,-1,1.0f,ag_model_piece_foot_x_sz,y_bot,ag_model_piece_foot_z_front);

	ag_model_piece_add_vertex(model,vertex++,center_bone_idx,bone_idx,-1,1.0f,-ag_model_piece_foot_x_sz,y_mid,ag_model_piece_foot_z_front);
	ag_model_piece_add_vertex(model,vertex++,center_bone_idx,bone_idx,-1,1.0f,-ag_model_piece_foot_x_sz,0,z_mid);
	ag_model_piece_add_vertex(model,vertex++,center_bone_idx,bone_idx,-1,1.0f,-ag_model_piece_foot_x_sz,0,z_back);
	ag_model_piece_add_vertex(model,vertex++,center_bone_idx,bone_idx,-1,1.0f,-ag_model_piece_foot_x_sz,y_bot,z_back);
	ag_model_piece_add_vertex(model,vertex++,center_bone_idx,bone_idx,-1,1.0f,-ag_model_piece_foot_x_sz,y_bot,z_mid);
	ag_model_piece_add_vertex(model,vertex++,center_bone_idx,bone_idx,-1,1.0f,-ag_model_piece_foot_x_sz,y_bot,ag_model_piece_foot_z_front);

		// build the polys

	poly=&mesh->polys[poly_idx];

	ag_model_piece_add_quad(model,poly++,vertex_idx,0,1,4,5,0.5f,0.5f,0.5f,0.5f);
	ag_model_piece_add_quad(model,poly++,vertex_idx,1,2,3,4,0.5f,0.5f,0.5f,0.5f);

	ag_model_piece_add_quad(model,poly++,vertex_idx,6,7,10,11,0.5f,0.5f,0.5f,0.5f);
	ag_model_piece_add_quad(model,poly++,vertex_idx,7,8,9,10,0.5f,0.5f,0.5f,0.5f);

	ag_model_piece_add_quad(model,poly++,vertex_idx,6,0,5,11,0.5f,0.5f,0.5f,0.5f);
	ag_model_piece_add_quad(model,poly++,vertex_idx,7,1,0,6,0.5f,0.5f,0.5f,0.5f);
	ag_model_piece_add_quad(model,poly++,vertex_idx,8,2,1,7,0.5f,0.5f,0.5f,0.5f);
	ag_model_piece_add_quad(model,poly++,vertex_idx,8,2,3,9,0.5f,0.5f,0.5f,0.5f);
	ag_model_piece_add_quad(model,poly++,vertex_idx,9,3,4,10,0.5f,0.5f,0.5f,0.5f);
	ag_model_piece_add_quad(model,poly++,vertex_idx,10,4,5,11,0.5f,0.5f,0.5f,0.5f);
}

