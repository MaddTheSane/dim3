//
// dim3RTL
// by Brian Barnes (bbarnes@klinksoftware.com)
// free software offered as is for any use
//

	// force ray.h/ray_interface.h to compile correctly
	// when they are used together

#define _ray_types

	// internal types

typedef struct		{
                        float						x,y,z;
                    } ray_vector_type;

typedef struct		{
                        float						x,y,z;
                    } ray_point_type;

typedef struct		{
						int							x,y;
					} ray_2d_point_type;
					
typedef struct		{
                        float						x,y;
                    } ray_uv_type;
					
typedef struct		{
						float						r,g,b,a;
					} ray_color_type;

typedef struct		{
						float						a,b,c,d;
					} ray_plane_type;

typedef struct		{
						float						data[4][4];
					} ray_matrix_type;
					
	// external types

typedef ray_vector_type rtlVector;
typedef ray_point_type rtlPoint;
typedef ray_2d_point_type rtl2DPoint;
typedef ray_uv_type rtlUV;
typedef ray_color_type rtlColor;
typedef ray_matrix_type rtlMatrix;

					
