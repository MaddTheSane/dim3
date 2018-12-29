//
// dim3RTL
// by Brian Barnes (bbarnes@klinksoftware.com)
// free software offered as is for any use
//

#include "ray_os_defs.h"
#include "ray.h"
#include "ray_interface.h"

extern ray_global_type		ray_global;

/* =======================================================

      Vector and Point Functions
      
======================================================= */

float ray_vector_normalize(ray_vector_type *v)
{
	float			f,f_dist;
	
	f_dist=sqrtf((v->x*v->x)+(v->y*v->y)+(v->z*v->z));
	if (f_dist==0.0f) return(0.0f);
	
	f=1.0f/f_dist;
	v->x*=f;
	v->y*=f;
	v->z*=f;

	return(f_dist);		// return original distance of ray
}

void ray_vector_create_from_points(ray_vector_type *v,ray_point_type *p1,ray_point_type *p2)
{
	v->x=p1->x-p2->x;
	v->y=p1->y-p2->y;
	v->z=p1->z-p2->z;
}

void ray_vector_scalar_multiply(ray_vector_type *sm,ray_vector_type *v,float f)
{
	sm->x=v->x*f;
	sm->y=v->y*f;
	sm->z=v->z*f;
}

void ray_vector_add(ray_vector_type *v,ray_vector_type *v1,ray_vector_type *v2)
{
	v->x=v1->x+v2->x;
	v->y=v1->y+v2->y;
	v->z=v1->z+v2->z;
}

void ray_vector_cross_product(ray_vector_type *cp,ray_vector_type *v1,ray_vector_type *v2)
{
	cp->x=(v1->y*v2->z)-(v2->y*v1->z);
	cp->y=(v1->z*v2->x)-(v2->z*v1->x);
	cp->z=(v1->x*v2->y)-(v2->x*v1->y);
}

float ray_vector_dot_product(ray_vector_type *v1,ray_vector_type *v2)
{
	return((v1->x*v2->x)+(v1->y*v2->y)+(v1->z*v2->z));
}

void ray_vector_find_line_point_for_T(ray_point_type *p,ray_vector_type *v,float t,ray_point_type *lp)
{
	lp->x=p->x+(v->x*t);
	lp->y=p->y+(v->y*t);
	lp->z=p->z+(v->z*t);
}

/* =======================================================

      Identity Matrix
      
======================================================= */

void rtlMatrixIdentity(ray_matrix_type *mat)
{
	memset(mat,0x0,sizeof(ray_matrix_type));
	mat->data[0][0]=mat->data[1][1]=mat->data[2][2]=mat->data[3][3]=1;
}

/* =======================================================

      Multiple Matrix
      
======================================================= */

void rtlMatrixMultiply(ray_matrix_type *mat_1,ray_matrix_type *mat_2)
{
	int					i;
	ray_matrix_type		mat;
	
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
	
	memmove(mat_1,&mat,sizeof(ray_matrix_type));
}

void rtlMatrixVectorMultiply(ray_matrix_type *mat,ray_vector_type *v)
{
	float		fx,fy,fz;
	
	fx=v->x;
	fy=v->y;
	fz=v->z;
	
	v->x=(fx*mat->data[0][0])+(fy*mat->data[0][1])+(fz*mat->data[0][2])+mat->data[0][3];
	v->y=(fx*mat->data[1][0])+(fy*mat->data[1][1])+(fz*mat->data[1][2])+mat->data[1][3];
	v->z=(fx*mat->data[2][0])+(fy*mat->data[2][1])+(fz*mat->data[2][2])+mat->data[2][3];
}

/* =======================================================

      Setup Special Matrixes
      
======================================================= */

void rtlMatrixRotateX(ray_matrix_type *mat,float ang)
{
	float		r_cos,r_sin,rad;
	
	rad=ang*ray_ANG_to_RAD;
	r_cos=cosf(rad);
	r_sin=sinf(rad);
	
	rtlMatrixIdentity(mat);

	mat->data[1][1]=r_cos;
	mat->data[1][2]=r_sin;
	mat->data[2][1]=-r_sin;
	mat->data[2][2]=r_cos;
}

void rtlMatrixRotateY(ray_matrix_type *mat,float ang)
{
	float		r_cos,r_sin,rad;
	
	rad=ang*ray_ANG_to_RAD;
	r_cos=cosf(rad);
	r_sin=sinf(rad);
	
	rtlMatrixIdentity(mat);
	
	mat->data[0][0]=r_cos;
	mat->data[0][2]=-r_sin;
	mat->data[2][0]=r_sin;
	mat->data[2][2]=r_cos;
}

void rtlMatrixRotateZ(ray_matrix_type *mat,float ang)
{
	float		r_cos,r_sin,rad;
	
	rad=ang*ray_ANG_to_RAD;
	r_cos=cosf(rad);
	r_sin=sinf(rad);
	
	rtlMatrixIdentity(mat);
	
	mat->data[0][0]=r_cos;
	mat->data[0][1]=r_sin;
	mat->data[1][0]=-r_sin;
	mat->data[1][1]=r_cos;
}

void rtlMatrixScale(ray_matrix_type *mat,ray_vector_type *v)
{
	mat->data[0][0]=v->x;
	mat->data[0][1]=0.0f;
	mat->data[0][2]=0.0f;
	mat->data[0][3]=0.0f;
	
	mat->data[1][0]=0.0f;
	mat->data[1][1]=v->y;
	mat->data[1][2]=0.0f;
	mat->data[1][3]=0.0f;

	mat->data[2][0]=0.0f;
	mat->data[2][1]=0.0f;
	mat->data[2][2]=v->z;
	mat->data[2][3]=0.0f;

	mat->data[3][0]=0.0f;
	mat->data[3][1]=0.0f;
	mat->data[3][2]=0.0f;
	mat->data[3][3]=1.0f;
}

/* =======================================================

      Transpose Matrixes
      
======================================================= */

void rlMatrixTranpose(ray_matrix_type *mat)
{
	ray_matrix_type			trans_mat;

	memmove(&trans_mat,mat,sizeof(ray_matrix_type));
	
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

/* =======================================================

      Bound Collisions
      
======================================================= */

void ray_to_bound(ray_point_type *p,ray_vector_type *v,ray_bound_type *bnd)
{
	if (v->x<0.0f) {
		bnd->min.x=p->x+v->x;
		bnd->max.x=p->x;
	}
	else {
		bnd->min.x=p->x;
		bnd->max.x=p->x+v->x;
	}

	if (v->y<0.0f) {
		bnd->min.y=p->y+v->y;
		bnd->max.y=p->y;
	}
	else {
		bnd->min.y=p->y;
		bnd->max.y=p->y+v->y;
	}

	if (v->z<0.0f) {
		bnd->min.z=p->z+v->z;
		bnd->max.z=p->z;
	}
	else {
		bnd->min.z=p->z;
		bnd->max.z=p->z+v->z;
	}
}

bool ray_bound_bound_collision(ray_bound_type *bnd_1,ray_bound_type *bnd_2)
{
	if (bnd_1->max.x<bnd_2->min.x) return(FALSE);
	if (bnd_1->max.y<bnd_2->min.y) return(FALSE);
	if (bnd_1->max.z<bnd_2->min.z) return(FALSE);

	if (bnd_1->min.x>bnd_2->max.x) return(FALSE);
	if (bnd_1->min.y>bnd_2->max.y) return(FALSE);
	if (bnd_1->min.z>bnd_2->max.z) return(FALSE);
	
	return(TRUE);
}

bool ray_bound_ray_collision(ray_point_type *p,ray_vector_type *v,ray_bound_type *bnd)
{
	float			f,f1,f2;
	ray_point_type	p1,p2;
	
		// start with simple bounding
		// eliminations

	if (v->x<0.0f) {
		if (p->x<bnd->min.x) return(FALSE);
	}
	else {
		if (p->x>bnd->max.x) return(FALSE);
	}
	
	if (v->y<0.0f) {
		if (p->y<bnd->min.y) return(FALSE);
	}
	else {
		if (p->y>bnd->max.y) return(FALSE);
	}
	
	if (v->z<0.0f) {
		if (p->z<bnd->min.z) return(FALSE);
	}
	else {
		if (p->z>bnd->max.z) return(FALSE);
	}

		// find set of two X points on line,
		// and compare them with Y and Z to
		// see if the vector passes these bounds

	f=1.0f/v->x;
	f1=(bnd->min.x-p->x)*f;
	f2=(bnd->max.x-p->x)*f;

	p1.y=p->y+(v->y*f1);
	p2.y=p->y+(v->y*f2);
	
	if ((p1.y<bnd->min.y) && (p2.y<bnd->min.y)) return(FALSE);
	if ((p1.y>bnd->max.y) && (p2.y>bnd->max.y)) return(FALSE);
	
	p1.z=p->z+(v->z*f1);
	p2.z=p->z+(v->z*f2);
	
	if ((p1.z<bnd->min.z) && (p2.z<bnd->min.z)) return(FALSE);
	if ((p1.z>bnd->max.z) && (p2.z>bnd->max.z)) return(FALSE);

		// repeat for Y
		
	f=1.0f/v->y;
	f1=(bnd->min.y-p->y)*f;
	f2=(bnd->max.y-p->y)*f;
	
	p1.x=p->x+(v->x*f1);
	p2.x=p->x+(v->x*f2);

	if ((p1.x<bnd->min.x) && (p2.x<bnd->min.x)) return(FALSE);
	if ((p1.x>bnd->max.x) && (p2.x>bnd->max.x)) return(FALSE);

	p1.z=p->z+(v->z*f1);
	p2.z=p->z+(v->z*f2);
	
	if ((p1.z<bnd->min.z) && (p2.z<bnd->min.z)) return(FALSE);
	if ((p1.z>bnd->max.z) && (p2.z>bnd->max.z)) return(FALSE);
	
		// repeat for Z
		
	f=1.0f/v->z;
	f1=(bnd->min.z-p->z)*f;
	f2=(bnd->max.z-p->z)*f;
	
	p1.x=p->x+(v->x*f1);
	p2.x=p->x+(v->x*f2);

	if ((p1.x<bnd->min.x) && (p2.x<bnd->min.x)) return(FALSE);
	if ((p1.x>bnd->max.x) && (p2.x>bnd->max.x)) return(FALSE);

	p1.y=p->y+(v->y*f1);
	p2.y=p->y+(v->y*f2);

	if ((p1.y<bnd->min.y) && (p2.y<bnd->min.y)) return(FALSE);
	if ((p1.y>bnd->max.y) && (p2.y>bnd->max.y)) return(FALSE);

	return(TRUE);
}

/* =======================================================

      Ray and Plane Collisions
      
======================================================= */

bool ray_plane_ray_collision(ray_point_type *p,ray_vector_type *nv,float nv_dist,ray_plane_type *plane)
{
	float			f,f2,t;

		// nv is the normal vector for the ray,
		// and nv_dist is the distance of the ray (ray end point = p+(nv*nv_dist) )

		// the plane/ray collision is
		// -((plane_normal dot ray_start_point)+plane_d)/(plane_normal dot vector_normal)

		// calculate the denom first, if
		// it's 0, then the lines are parallel, skip out

	f=(plane->a*nv->x)+(plane->b*nv->y)+(plane->c*nv->z);
	if (f==0.0f) return(FALSE);

		// finish the equation and check
		// where point is on line

	f2=(plane->a*p->x)+(plane->b*p->y)+(plane->c*p->z);
	t=-((f2+plane->d)/f);

	return((t>=0.0f) && (t<nv_dist));
}

/* =======================================================

      Distance Functions
      
======================================================= */

float ray_distance_between_points(ray_point_type *p1,ray_point_type *p2)
{
	double			dx,dy,dz;

	dx=(double)(p1->x-p2->x);
	dy=(double)(p1->y-p2->y);
	dz=(double)(p1->z-p2->z);

	return((float)sqrt((dx*dx)+(dy*dy)+(dz*dz)));
}

/* =======================================================

      Bitmap Reductions
      
======================================================= */

unsigned char* ray_bitmap_reduction(int factor,int wid,int high,unsigned char *data)
{
	int					x,y,x2,y2,
						lx,rx,ty,by,half_factor,
						r_wid,r_high,sz,
						merge_count,pixel_offset;
	float				f;
	unsigned char		*r_data;
	unsigned long		*buf,*r_buf;
	ray_color_type		col,merge_col;

		// memory for new bitmap

	r_wid=wid/factor;
	r_high=high/factor;

	sz=(r_wid*r_high)<<2;
	r_data=(unsigned char*)malloc(sz);
	if (r_data==NULL) return(NULL);

	half_factor=factor>>1;

		// build it

	for (y=0;y!=r_high;y++) {

		for (x=0;x!=r_wid;x++) {

				// merge the pixels into
				// the reduced pixel

			merge_count=0;
			merge_col.r=merge_col.g=merge_col.b=merge_col.a=0.0f;

			lx=(x*factor)-half_factor;
			rx=lx+factor;

			if (lx<0) lx=0;
			if (rx>wid) rx=wid;

			ty=(y*factor)-half_factor;
			by=ty+factor;

			if (ty<0) ty=0;
			if (by>high) by=high;

			for (y2=ty;y2<by;y2++) {

				pixel_offset=(wid*y2)+lx;
				buf=(((unsigned long*)data)+pixel_offset);

				for (x2=lx;x2<rx;x2++) {
					ray_create_float_color_from_ulong(*buf,&col);
					buf++;

					merge_col.r+=col.r;
					merge_col.g+=col.g;
					merge_col.b+=col.b;
					merge_col.a+=col.a;

					merge_count++;
				}
			}

			if (merge_count!=0) {
				f=1.0f/((float)merge_count);
				merge_col.r*=f;
				merge_col.g*=f;
				merge_col.b*=f;
				merge_col.a*=f;
			}

				// write back merged pixel

			pixel_offset=(r_wid*y)+x;
			r_buf=(((unsigned long*)r_data)+pixel_offset);
			*r_buf=ray_create_ulong_color_from_float(&merge_col);
		}
	}

	return(r_data);
}
