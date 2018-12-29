/****************************** File *********************************

Module: dim3 Base Utility
Author: Brian Barnes
 Usage: Matrix Functions

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
	#include "dim3baseutility.h"
#endif
	
/* =======================================================

      Identity Matrix
      
======================================================= */
	
void matrix_identity(matrix_type *mat)
{
	memset(mat,0x0,sizeof(matrix_type));
	mat->data[0][0]=mat->data[1][1]=mat->data[2][2]=mat->data[3][3]=1;
}

/* =======================================================

      Multiple Matrix
      
======================================================= */

void matrix_multiply(matrix_type *mat_1,matrix_type *mat_2)
{
	int				i;
	matrix_type		mat;
	
	for (i=0;i!=4;i++) {
		mat.data[i][0]=
			(mat_1->data[i][0]*mat_2->data[0][0])+
			(mat_1->data[i][1]*mat_2->data[1][0])+
			(mat_1->data[i][2]*mat_2->data[2][0])+
			(mat_1->data[i][3]*mat_2->data[3][0]);

		mat.data[i][1]=
			(mat_1->data[i][0]*mat_2->data[0][1])+
			(mat_1->data[i][1]*mat_2->data[1][1])+
			(mat_1->data[i][2]*mat_2->data[2][1])+
			(mat_1->data[i][3]*mat_2->data[3][1]);

		mat.data[i][2]=
			(mat_1->data[i][0]*mat_2->data[0][2])+
			(mat_1->data[i][1]*mat_2->data[1][2])+
			(mat_1->data[i][2]*mat_2->data[2][2])+
			(mat_1->data[i][3]*mat_2->data[3][2]);

		mat.data[i][3]=
			(mat_1->data[i][0]*mat_2->data[0][3])+
			(mat_1->data[i][1]*mat_2->data[1][3])+
			(mat_1->data[i][2]*mat_2->data[2][3])+
			(mat_1->data[i][3]*mat_2->data[3][3]);
	}
	
	memmove(mat_1,&mat,sizeof(matrix_type));
}

void matrix_vertex_multiply(matrix_type *mat,float *x,float *y,float *z)
{
	float		fx,fy,fz;
	
	fx=*x;
	fy=*y;
	fz=*z;
	
	*x=(fx*mat->data[0][0])+(fy*mat->data[0][1])+(fz*mat->data[0][2])+mat->data[0][3];
	*y=(fx*mat->data[1][0])+(fy*mat->data[1][1])+(fz*mat->data[1][2])+mat->data[1][3];
	*z=(fx*mat->data[2][0])+(fy*mat->data[2][1])+(fz*mat->data[2][2])+mat->data[2][3];
}

void matrix_vertex_multiply_ignore_transform(matrix_type *mat,float *x,float *y,float *z)
{
	float		fx,fy,fz;
	
	fx=*x;
	fy=*y;
	fz=*z;
	
	*x=(fx*mat->data[0][0])+(fy*mat->data[0][1])+(fz*mat->data[0][2]);
	*y=(fx*mat->data[1][0])+(fy*mat->data[1][1])+(fz*mat->data[1][2]);
	*z=(fx*mat->data[2][0])+(fy*mat->data[2][1])+(fz*mat->data[2][2]);
}

void matrix_vertex_2_multiply_ignore_transform(matrix_type *mat,d3vct *vct)
{
	float		fx,fy,fz;
	
	fx=vct->x;
	fy=vct->y;
	fz=vct->z;
	
	vct->x=(fx*mat->data[0][0])+(fy*mat->data[0][1])+(fz*mat->data[0][2]);
	vct->y=(fx*mat->data[1][0])+(fy*mat->data[1][1])+(fz*mat->data[1][2]);
	vct->z=(fx*mat->data[2][0])+(fy*mat->data[2][1])+(fz*mat->data[2][2]);
}

void matrix_angle_multiply(matrix_type *mat,float *x,float *y,float *z)
{
	float		fx,fy,fz;
	
	fx=*x;
	fy=*y;
	fz=*z;
	
	*x=(fx*mat->data[0][0])+(fy*mat->data[0][1])+(fz*mat->data[0][2])+mat->data[0][3];
	*y=(fx*mat->data[1][0])+(fy*mat->data[1][1])+(fz*mat->data[1][2])+mat->data[1][3];
	*z=(fx*mat->data[2][0])+(fy*mat->data[2][1])+(fz*mat->data[2][2])+mat->data[2][3];
}

/* =======================================================

      Rotation Matrixes
      
======================================================= */

void matrix_rotate_x(matrix_type *mat,float ang)
{
	float		rad,r_cos,r_sin;
	
	rad=ang*ANG_to_RAD;
	r_cos=cosf(rad);
	r_sin=sinf(rad);
	
	matrix_identity(mat);

	mat->data[1][1]=r_cos;
	mat->data[1][2]=r_sin;
	mat->data[2][1]=-r_sin;
	mat->data[2][2]=r_cos;
}

void matrix_rotate_y(matrix_type *mat,float ang)
{
	float		rad,r_cos,r_sin;
	
	rad=ang*ANG_to_RAD;
	r_cos=cosf(rad);
	r_sin=sinf(rad);
	
	matrix_identity(mat);
	
	mat->data[0][0]=r_cos;
	mat->data[0][2]=-r_sin;
	mat->data[2][0]=r_sin;
	mat->data[2][2]=r_cos;
}

void matrix_rotate_z(matrix_type *mat,float ang)
{
	float		rad,r_cos,r_sin;
	
	rad=ang*ANG_to_RAD;
	r_cos=cosf(rad);
	r_sin=sinf(rad);
	
	matrix_identity(mat);
	
	mat->data[0][0]=r_cos;
	mat->data[0][1]=r_sin;
	mat->data[1][0]=-r_sin;
	mat->data[1][1]=r_cos;
}

void matrix_rotate_xy(matrix_type *mat,float x_ang,float y_ang)
{
	matrix_type		y_mat;
	
	matrix_rotate_x(mat,x_ang);

	matrix_rotate_y(&y_mat,y_ang);
	matrix_multiply(mat,&y_mat);
}

void matrix_rotate_xyz(matrix_type *mat,float x_ang,float y_ang,float z_ang)
{
	matrix_type		y_mat,z_mat;
	
	matrix_rotate_x(mat,x_ang);

	matrix_rotate_y(&y_mat,y_ang);
	matrix_multiply(mat,&y_mat);
	
	matrix_rotate_z(&z_mat,z_ang);
	matrix_multiply(mat,&z_mat);
}

void matrix_rotate_zyx(matrix_type *mat,float x_ang,float y_ang,float z_ang)
{
	matrix_type		x_mat,y_mat;
	
	matrix_rotate_z(mat,z_ang);
	
	matrix_rotate_y(&y_mat,y_ang);
	matrix_multiply(mat,&y_mat);

	matrix_rotate_x(&x_mat,x_ang);
	matrix_multiply(mat,&x_mat);
}

void matrix_rotate_xzy(matrix_type *mat,float x_ang,float y_ang,float z_ang)
{
	matrix_type		y_mat,z_mat;
	
	matrix_rotate_x(mat,x_ang);
	
	matrix_rotate_z(&z_mat,z_ang);
	matrix_multiply(mat,&z_mat);

	matrix_rotate_y(&y_mat,y_ang);
	matrix_multiply(mat,&y_mat);
}

bool matrix_has_rotation(matrix_type *mat)
{
	if (mat->data[0][0]!=0.0f) return(TRUE);
	if (mat->data[0][1]!=0.0f) return(TRUE);
	if (mat->data[0][2]!=0.0f) return(TRUE);

	if (mat->data[1][0]!=0.0f) return(TRUE);
	if (mat->data[1][1]!=0.0f) return(TRUE);
	if (mat->data[1][2]!=0.0f) return(TRUE);

	if (mat->data[2][0]!=0.0f) return(TRUE);
	if (mat->data[2][1]!=0.0f) return(TRUE);
	if (mat->data[2][2]!=0.0f) return(TRUE);

	if (mat->data[3][0]!=0.0f) return(TRUE);
	if (mat->data[3][1]!=0.0f) return(TRUE);
	if (mat->data[3][2]!=0.0f) return(TRUE);

	return(FALSE);
}

/* =======================================================

      Frustum and Ortho Matrix
      
======================================================= */

void matrix_frustum(matrix_type *mat,float x_min,float x_max,float y_min,float y_max,float near_z,float far_z)
{
		// frustum matrix equation
		
	mat->data[0][0]=(2.0f*near_z)/(x_max-x_min);
	mat->data[0][1]=0.0f;
	mat->data[0][2]=(x_max+x_min)/(x_max-x_min);
	mat->data[0][3]=0.0f;
	
	mat->data[1][0]=0.0f;
	mat->data[1][1]=(2.0f*near_z)/(y_max-y_min);
	mat->data[1][2]=(y_max+y_min)/(y_max-y_min);
	mat->data[1][3]=0.0f;
	
	mat->data[2][0]=0.0f;
	mat->data[2][1]=0.0f;
	mat->data[2][2]=-((far_z+near_z)/(far_z-near_z));
	mat->data[2][3]=-((2.0f*far_z*near_z)/(far_z-near_z));
	
	mat->data[3][0]=0.0f;
	mat->data[3][1]=0.0f;
	mat->data[3][2]=-1.0f;
	mat->data[3][3]=0.0f;
}

void matrix_perspective(matrix_type *mat,float fov_y,float aspect,float near_z,float far_z)
{
	float			x_min,x_max,y_min,y_max;
	
		// turn prespective into frustum
	
	y_max=near_z*tanf((fov_y*TRIG_PI)/360.0f);
	y_min=-y_max;
	x_min=y_min*aspect;
	x_max=y_max*aspect;
	
	matrix_frustum(mat,x_min,x_max,y_min,y_max,near_z,far_z);
}

void matrix_ortho(matrix_type *mat,float x_min,float x_max,float y_min,float y_max,float near_z,float far_z)
{
		// ortho matrix equation
		
	mat->data[0][0]=2.0f/(x_max-x_min);
	mat->data[0][1]=0.0f;
	mat->data[0][2]=0.0f;
	mat->data[0][3]=-((x_max+x_min)/(x_max-x_min));
	
	mat->data[1][0]=0.0f;
	mat->data[1][1]=2.0f/(y_max-y_min);
	mat->data[1][2]=0.0f;
	mat->data[1][3]=-((y_max+y_min)/(y_max-y_min));
	
	mat->data[2][0]=0.0f;
	mat->data[2][1]=0.0f;
	mat->data[2][2]=-2.0f/(far_z-near_z);
	mat->data[2][3]=-((far_z+near_z)/(far_z-near_z));
	
	mat->data[3][0]=0.0f;
	mat->data[3][1]=0.0f;
	mat->data[3][2]=0.0f;
	mat->data[3][3]=1.0f;
}

/* =======================================================

      Translate, Scale, Rotate,
	  Transpose, Inverse Matrix
      
======================================================= */

void matrix_translate(matrix_type *mat,float x,float y,float z)
{
	matrix_type			trans_mat;

	trans_mat.data[0][0]=1.0f;
	trans_mat.data[0][1]=0.0f;
	trans_mat.data[0][2]=0.0f;
	trans_mat.data[0][3]=x;
	
	trans_mat.data[1][0]=0.0f;
	trans_mat.data[1][1]=1.0f;
	trans_mat.data[1][2]=0.0f;
	trans_mat.data[1][3]=y;

	trans_mat.data[2][0]=0.0f;
	trans_mat.data[2][1]=0.0f;
	trans_mat.data[2][2]=1.0f;
	trans_mat.data[2][3]=z;

	trans_mat.data[3][0]=0.0f;
	trans_mat.data[3][1]=0.0f;
	trans_mat.data[3][2]=0.0f;
	trans_mat.data[3][3]=1.0f;
	
	matrix_multiply(mat,&trans_mat);
}

void matrix_scale(matrix_type *mat,float x,float y,float z)
{
	matrix_type			scale_mat;
	
	scale_mat.data[0][0]=x;
	scale_mat.data[0][1]=0.0f;
	scale_mat.data[0][2]=0.0f;
	scale_mat.data[0][3]=0.0f;
	
	scale_mat.data[1][0]=0.0f;
	scale_mat.data[1][1]=y;
	scale_mat.data[1][2]=0.0f;
	scale_mat.data[1][3]=0.0f;

	scale_mat.data[2][0]=0.0f;
	scale_mat.data[2][1]=0.0f;
	scale_mat.data[2][2]=z;
	scale_mat.data[2][3]=0.0f;

	scale_mat.data[3][0]=0.0f;
	scale_mat.data[3][1]=0.0f;
	scale_mat.data[3][2]=0.0f;
	scale_mat.data[3][3]=1.0f;
	
	matrix_multiply(mat,&scale_mat);
}

void matrix_rotate(matrix_type *mat,float ang,float x,float y,float z)
{
	float				rad,cs,rcs,sn;
	d3vct				vct;
	matrix_type			rot_mat;
	
		// make sure x/y/z is normalized

	vct.x=x;
	vct.y=y;
	vct.z=z;
	vector_normalize(&vct);

		// create the rot matrix

	rad=ang*ANG_to_RAD;
	cs=cosf(rad);
	rcs=1.0f-cs;
	sn=sinf(rad);

	rot_mat.data[0][0]=((vct.x*vct.x)*rcs)+cs;
	rot_mat.data[0][1]=((vct.x*vct.y)*rcs)-(vct.z*sn);
	rot_mat.data[0][2]=((vct.x*vct.z)*rcs)+(vct.y*sn);
	rot_mat.data[0][3]=0.0f;
	
	rot_mat.data[1][0]=((vct.y*vct.x)*rcs)+(vct.z*sn);
	rot_mat.data[1][1]=((vct.y*vct.y)*rcs)+cs;
	rot_mat.data[1][2]=((vct.y*vct.z)*rcs)-(vct.x*sn);
	rot_mat.data[1][3]=0.0f;

	rot_mat.data[2][0]=((vct.x*vct.z)*rcs)-(vct.y*sn);
	rot_mat.data[2][1]=((vct.y*vct.z)*rcs)+(vct.x*sn);
	rot_mat.data[2][2]=((vct.z*vct.z)*rcs)+cs;
	rot_mat.data[2][3]=0.0f;

	rot_mat.data[3][0]=0.0f;
	rot_mat.data[3][1]=0.0f;
	rot_mat.data[3][2]=0.0f;
	rot_mat.data[3][3]=1.0f;
	
	matrix_multiply(mat,&rot_mat);
}

void matrix_transpose(matrix_type *mat)
{
	matrix_type			trans_mat;

	memmove(&trans_mat,mat,sizeof(matrix_type));
	
	mat->data[0][0]=trans_mat.data[0][0];
	mat->data[0][1]=trans_mat.data[1][0];
	mat->data[0][2]=trans_mat.data[2][0];
	mat->data[0][3]=trans_mat.data[3][0];
	
	mat->data[1][0]=trans_mat.data[0][1];
	mat->data[1][1]=trans_mat.data[1][1];
	mat->data[1][2]=trans_mat.data[2][1];
	mat->data[1][3]=trans_mat.data[3][1];

	mat->data[2][0]=trans_mat.data[0][2];
	mat->data[2][1]=trans_mat.data[1][2];
	mat->data[2][2]=trans_mat.data[2][2];
	mat->data[2][3]=trans_mat.data[3][2];

	mat->data[3][0]=trans_mat.data[0][3];
	mat->data[3][1]=trans_mat.data[1][3];
	mat->data[3][2]=trans_mat.data[2][3];
	mat->data[3][3]=trans_mat.data[3][3];
}

void matrix_inverse(matrix_type *mat)
{
	int				n;
	float			det,*m1,*m2;
	matrix_type		mat_inv;

    mat_inv.data[0][0]=
		(mat->data[1][1]*mat->data[2][2]*mat->data[3][3])-
		(mat->data[1][1]*mat->data[3][2]*mat->data[2][3])-
		(mat->data[1][2]*mat->data[2][1]*mat->data[3][3])+
		(mat->data[1][2]*mat->data[3][1]*mat->data[2][3])+
		(mat->data[1][3]*mat->data[2][1]*mat->data[3][2])-
		(mat->data[1][3]*mat->data[3][1]*mat->data[2][2]);
    mat_inv.data[0][1]=
		(-mat->data[0][1]*mat->data[2][2]*mat->data[3][3])+
		(mat->data[0][1]*mat->data[3][2]*mat->data[2][3])+
		(mat->data[0][2]*mat->data[2][1]*mat->data[3][3])-
		(mat->data[0][2]*mat->data[3][1]*mat->data[2][3])-
		(mat->data[0][3]*mat->data[2][1]*mat->data[3][2])+
		(mat->data[0][3]*mat->data[3][1]*mat->data[2][2]);
    mat_inv.data[0][2]=
		(mat->data[0][1]*mat->data[1][2]*mat->data[3][3])-
		(mat->data[0][1]*mat->data[3][2]*mat->data[1][3])-
		(mat->data[0][2]*mat->data[1][1]*mat->data[3][3])+
		(mat->data[0][2]*mat->data[3][1]*mat->data[1][3])+
		(mat->data[0][3]*mat->data[1][1]*mat->data[3][2])-
		(mat->data[0][3]*mat->data[3][1]*mat->data[1][2]);
    mat_inv.data[0][3]=
		(-mat->data[0][1]*mat->data[1][2]*mat->data[2][3])+
		(mat->data[0][1]*mat->data[2][2]*mat->data[1][3])+
		(mat->data[0][2]*mat->data[1][1]*mat->data[2][3])-
		(mat->data[0][2]*mat->data[2][1]*mat->data[1][3])-
		(mat->data[0][3]*mat->data[1][1]*mat->data[2][2])+
		(mat->data[0][3]*mat->data[2][1]*mat->data[1][2]);
    mat_inv.data[1][0]=
		(-mat->data[1][0]*mat->data[2][2]*mat->data[3][3])+
		(mat->data[1][0]*mat->data[3][2]*mat->data[2][3])+
		(mat->data[1][2]*mat->data[2][0]*mat->data[3][3])-
		(mat->data[1][2]*mat->data[3][0]*mat->data[2][3])-
		(mat->data[1][3]*mat->data[2][0]*mat->data[3][2])+
		(mat->data[1][3]*mat->data[3][0]*mat->data[2][2]);
    mat_inv.data[1][1]=
		(mat->data[0][0]*mat->data[2][2]*mat->data[3][3])-
		(mat->data[0][0]*mat->data[3][2]*mat->data[2][3])-
		(mat->data[0][2]*mat->data[2][0]*mat->data[3][3])+
		(mat->data[0][2]*mat->data[3][0]*mat->data[2][3])+
		(mat->data[0][3]*mat->data[2][0]*mat->data[3][2])-
		(mat->data[0][3]*mat->data[3][0]*mat->data[2][2]);
    mat_inv.data[1][2]=
		(-mat->data[0][0]*mat->data[1][2]*mat->data[3][3])+
		(mat->data[0][0]*mat->data[3][2]*mat->data[1][3])+
		(mat->data[0][2]*mat->data[1][0]*mat->data[3][3])-
		(mat->data[0][2]*mat->data[3][0]*mat->data[1][3])-
		(mat->data[0][3]*mat->data[1][0]*mat->data[3][2])+
		(mat->data[0][3]*mat->data[3][0]*mat->data[1][2]);
    mat_inv.data[1][3]=
		(mat->data[0][0]*mat->data[1][2]*mat->data[2][3])-
		(mat->data[0][0]*mat->data[2][2]*mat->data[1][3])-
		(mat->data[0][2]*mat->data[1][0]*mat->data[2][3])+
		(mat->data[0][2]*mat->data[2][0]*mat->data[1][3])+
		(mat->data[0][3]*mat->data[1][0]*mat->data[2][2])-
		(mat->data[0][3]*mat->data[2][0]*mat->data[1][2]);
    mat_inv.data[2][0]=
		(mat->data[1][0]*mat->data[2][1]*mat->data[3][3])-
		(mat->data[1][0]*mat->data[3][1]*mat->data[2][3])-
		(mat->data[1][1]*mat->data[2][0]*mat->data[3][3])+
		(mat->data[1][1]*mat->data[3][0]*mat->data[2][3])+
		(mat->data[1][3]*mat->data[2][0]*mat->data[3][1])-
		(mat->data[1][3]*mat->data[3][0]*mat->data[2][1]);
    mat_inv.data[2][1]=
		(-mat->data[0][0]*mat->data[2][1]*mat->data[3][3])+
		(mat->data[0][0]*mat->data[3][1]*mat->data[2][3])+
		(mat->data[0][1]*mat->data[2][0]*mat->data[3][3])-
		(mat->data[0][1]*mat->data[3][0]*mat->data[2][3])-
		(mat->data[0][3]*mat->data[2][0]*mat->data[3][1])+
		(mat->data[0][3]*mat->data[3][0]*mat->data[2][1]);
    mat_inv.data[2][2]=
		(mat->data[0][0]*mat->data[1][1]*mat->data[3][3])-
		(mat->data[0][0]*mat->data[3][1]*mat->data[1][3])-
		(mat->data[0][1]*mat->data[1][0]*mat->data[3][3])+
		(mat->data[0][1]*mat->data[3][0]*mat->data[1][3])+
		(mat->data[0][3]*mat->data[1][0]*mat->data[3][1])-
		(mat->data[0][3]*mat->data[3][0]*mat->data[1][1]);
    mat_inv.data[2][3]=
		(-mat->data[0][0]*mat->data[1][1]*mat->data[2][3])+
		(mat->data[0][0]*mat->data[2][1]*mat->data[1][3])+
		(mat->data[0][1]*mat->data[1][0]*mat->data[2][3])-
		(mat->data[0][1]*mat->data[2][0]*mat->data[1][3])-
		(mat->data[0][3]*mat->data[1][0]*mat->data[2][1])+
		(mat->data[0][3]*mat->data[2][0]*mat->data[1][1]);
    mat_inv.data[3][0]=
		(-mat->data[1][0]*mat->data[2][1]*mat->data[3][2])+
		(mat->data[1][0]*mat->data[3][1]*mat->data[2][2])+
		(mat->data[1][1]*mat->data[2][0]*mat->data[3][2])-
		(mat->data[1][1]*mat->data[3][0]*mat->data[2][2])-
		(mat->data[1][2]*mat->data[2][0]*mat->data[3][1])+
		(mat->data[1][2]*mat->data[3][0]*mat->data[2][1]);
    mat_inv.data[3][1]=
		(mat->data[0][0]*mat->data[2][1]*mat->data[3][2])-
		(mat->data[0][0]*mat->data[3][1]*mat->data[2][2])-
		(mat->data[0][1]*mat->data[2][0]*mat->data[3][2])+
		(mat->data[0][1]*mat->data[3][0]*mat->data[2][2])+
		(mat->data[0][2]*mat->data[2][0]*mat->data[3][1])-
		(mat->data[0][2]*mat->data[3][0]*mat->data[2][1]);
    mat_inv.data[3][2]=
		(-mat->data[0][0]*mat->data[1][1]*mat->data[3][2])+
		(mat->data[0][0]*mat->data[3][1]*mat->data[1][2])+
		(mat->data[0][1]*mat->data[1][0]*mat->data[3][2])-
		(mat->data[0][1]*mat->data[3][0]*mat->data[1][2])-
		(mat->data[0][2]*mat->data[1][0]*mat->data[3][1])+
		(mat->data[0][2]*mat->data[3][0]*mat->data[1][1]);
    mat_inv.data[3][3]=
		(mat->data[0][0]*mat->data[1][1]*mat->data[2][2])-
		(mat->data[0][0]*mat->data[2][1]*mat->data[1][2])-
		(mat->data[0][1]*mat->data[1][0]*mat->data[2][2])+
		(mat->data[0][1]*mat->data[2][0]*mat->data[1][2])+
		(mat->data[0][2]*mat->data[1][0]*mat->data[2][1])-
		(mat->data[0][2]*mat->data[2][0]*mat->data[1][1]);

    det=(mat->data[0][0]*mat_inv.data[0][0])+(mat->data[1][0]*mat_inv.data[0][1])+(mat->data[2][0]*mat_inv.data[0][2])+(mat->data[3][0]*mat_inv.data[0][3]);
    if (det==0.0f) return;

    det=1.0f/det;

	m1=&mat->data[0][0];
	m2=&mat_inv.data[0][0];

	for (n=0;n!=16;n++) {
        *m1++=(*m2++)*det;
	}
}

/* =======================================================

      LookAt matrix
      
======================================================= */

void matrix_lookat(matrix_type *mat,float eye_x,float eye_y,float eye_z,float center_x,float center_y,float center_z,float up_x,float up_y,float up_z)
{
	d3vct			x,y,z;
	matrix_type		look_mat;

		// create the z vector

	z.x=eye_x-center_x;
	z.y=eye_y-center_y;
	z.z=eye_z-center_z;
	vector_normalize(&z);

		// create the y vector

	y.x=up_x;
	y.y=up_y;
	y.z=up_z;
		
		// create x vector, y cross z

	vector_cross_product(&x,&y,&z);

		// recreate the y from z cross x

	vector_cross_product(&y,&z,&x);

		// normalize x and y

	vector_normalize(&x);
	vector_normalize(&y);

		// create and multiply in
		// the rotational matrix
	
	look_mat.data[0][0]=x.x;
	look_mat.data[0][1]=x.y;
	look_mat.data[0][2]=x.z;
	look_mat.data[0][3]=0.0f;

	look_mat.data[1][0]=y.x;
	look_mat.data[1][1]=y.y;
	look_mat.data[1][2]=y.z;
	look_mat.data[1][3]=0.0f;

	look_mat.data[2][0]=z.x;
	look_mat.data[2][1]=z.y;
	look_mat.data[2][2]=z.z;
	look_mat.data[2][3]=0.0f;

	look_mat.data[3][0]=0.0f;
	look_mat.data[3][1]=0.0f;
	look_mat.data[3][2]=0.0f;
	look_mat.data[3][3]=1.0f;

	matrix_multiply(mat,&look_mat);

		// translate eye to origin

    matrix_translate(mat,-eye_x,-eye_y,-eye_z);
}

/* =======================================================

      OpenGL Matrix Uniform
      
======================================================= */

void matrix_to_opengl_uniform_4x4(matrix_type *mat,float *mp)
{
		// uniforms in column order
		// our matrixes are row order
		
	*mp++=mat->data[0][0];
	*mp++=mat->data[1][0];
	*mp++=mat->data[2][0];
	*mp++=mat->data[3][0];

	*mp++=mat->data[0][1];
	*mp++=mat->data[1][1];
	*mp++=mat->data[2][1];
	*mp++=mat->data[3][1];

	*mp++=mat->data[0][2];
	*mp++=mat->data[1][2];
	*mp++=mat->data[2][2];
	*mp++=mat->data[3][2];

	*mp++=mat->data[0][3];
	*mp++=mat->data[1][3];
	*mp++=mat->data[2][3];
	*mp=mat->data[3][3];
}

void matrix_to_opengl_uniform_3x3(matrix_type *mat,float *mp)
{
		// uniforms in column order
		// our matrixes are row order
		
	*mp++=mat->data[0][0];
	*mp++=mat->data[1][0];
	*mp++=mat->data[2][0];

	*mp++=mat->data[0][1];
	*mp++=mat->data[1][1];
	*mp++=mat->data[2][1];

	*mp++=mat->data[0][2];
	*mp++=mat->data[1][2];
	*mp=mat->data[2][2];
}

/* =======================================================

      Average Matrixes
      
======================================================= */

void matrix_average(matrix_type *mat,int nmatrix,matrix_type *mats)
{
	int				n,i;
	float			*f,*f2,f_count;

		// all zeros

	memset(mat,0x0,sizeof(matrix_type));

		// add up all the matrixes

	for (n=0;n!=nmatrix;n++) {
		f=(float*)mat->data;
		f2=(float*)mats[n].data;

		for (i=0;i!=16;i++) {
			*f=(*f)+(*f2++);
			f++;
		}
	}

		// average the matrixes

	f_count=(float)nmatrix;
	f=(float*)mat->data;

	for (i=0;i!=16;i++) {
		*f=(*f)/f_count;
		f++;
	}
}

