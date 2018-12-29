/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Liquid Reflection Map Routines

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
	#include "dim3editor.h"
#endif

#include "glue.h"
#include "interface.h"

extern map_type					map;

extern file_path_setup_type		file_path_setup;

#define liquid_reflection_texture_start_idx					160
#define max_liquid_reflection_map_textures					32
#define liquid_reflection_mirror_distance					50000

typedef struct		{
						unsigned char						*data;
					} liquid_reflection_map_map_texture_type;

int										liquid_reflection_texture_pixel_size;
liquid_reflection_map_map_texture_type	liquid_reflection_map_map_texture[max_map_texture];
					
/* =======================================================

      Find Ray Trace Hit Pixel
      
======================================================= */

void liquid_reflection_map_bitmap_pixel_start(void)
{
	int				n;
	
	for (n=0;n!=max_map_texture;n++) {
		liquid_reflection_map_map_texture[n].data=NULL;
	}
}

void liquid_reflection_map_bitmap_pixel_free(void)
{
	int				n;
	
	for (n=0;n!=max_map_texture;n++) {
		if (liquid_reflection_map_map_texture[n].data!=NULL) free(liquid_reflection_map_map_texture[n].data);
	}
}

void liquid_reflection_map_bitmap_pixel_get(d3pnt *spt,d3vct *vct,map_mesh_type *mesh,map_mesh_poly_type *poly,float hit_t,d3col *col)
{
	int				n,txt_idx,txt_sz,x,y,
					lft_idx,rgt_idx,top_idx,bot_idx;
	float			fx,fy,fz,f1,f2,min_gx,max_gx,min_gy,max_gy;
	unsigned char	*ptr;
	d3pnt			hpt;
	d3pnt			*pt;
	bitmap_type		*bitmap;
	
	col->r=col->g=col->b=0;

		// get texture
		
	txt_idx=poly->txt_idx;
	if ((txt_idx<0) || (txt_idx>=max_map_texture)) return;
	
	bitmap=&map.textures[txt_idx].frames[0].bitmap;
	if (bitmap->gl_id==-1) return;

		// need to load up this bitmap
		
	if (liquid_reflection_map_map_texture[txt_idx].data==NULL) {
		txt_sz=(bitmap->wid*3)*bitmap->high;
		liquid_reflection_map_map_texture[txt_idx].data=(unsigned char*)malloc(txt_sz);
		bzero(liquid_reflection_map_map_texture[txt_idx].data,txt_sz);
		
		glBindTexture(GL_TEXTURE_2D,bitmap->gl_id);
		glGetTexImage(GL_TEXTURE_2D,0,GL_RGB,GL_UNSIGNED_BYTE,(GLvoid*)liquid_reflection_map_map_texture[txt_idx].data);
	}
	
		// get the hit point
		
	hpt.x=spt->x+(int)(vct->x*hit_t);
	hpt.y=spt->y+(int)(vct->y*hit_t);
	hpt.z=spt->z+(int)(vct->z*hit_t);
	
		// hits on wall like polygons
		
	if (poly->box.wall_like) {
	
			// find points for uv extents
			
		lft_idx=rgt_idx=0;
		top_idx=bot_idx=0;
		
		for (n=1;n<poly->ptsz;n++) {
			pt=&mesh->vertexes[poly->v[n]];
			if ((pt->x==poly->line.lx) && (pt->z==poly->line.lz)) lft_idx=n;
			if ((pt->x==poly->line.rx) && (pt->z==poly->line.rz)) rgt_idx=n;
			if (pt->y==poly->box.min.y) top_idx=n;
			if (pt->y==poly->box.max.y) bot_idx=n;
		}
		
			// find distances that hit point is on
			// wall x/z plane
			
		fx=(float)(poly->line.rx-poly->line.lx);
		fz=(float)(poly->line.rz-poly->line.lz);
		f1=sqrtf((fx*fx)+(fz*fz));
		
		fx=(float)(hpt.x-poly->line.lx);
		fz=(float)(hpt.z-poly->line.lz);
		f2=sqrtf((fx*fx)+(fz*fz));
		
		fx=f2/f1;
		fx=poly->main_uv.uvs[lft_idx].x+(fx*(poly->main_uv.uvs[rgt_idx].x-poly->main_uv.uvs[lft_idx].x));
		
			// find the distances for hit point on y plane
			
		fy=(float)(hpt.y-poly->box.min.y)/(float)(poly->box.max.y-poly->box.min.y);
		fy=poly->main_uv.uvs[top_idx].y+(fy*(poly->main_uv.uvs[bot_idx].y-poly->main_uv.uvs[top_idx].y));
	}
	
		// hits on floor like polygons
		
	else {
	
			// find points for uv extents
			
		min_gx=max_gx=poly->main_uv.uvs[0].x;
		min_gy=max_gy=poly->main_uv.uvs[0].y;
		
		for (n=1;n<poly->ptsz;n++) {
			if (poly->main_uv.uvs[n].x<min_gx) min_gx=poly->main_uv.uvs[n].x;
			if (poly->main_uv.uvs[n].x>max_gx) max_gx=poly->main_uv.uvs[n].x;
			if (poly->main_uv.uvs[n].y<min_gy) min_gy=poly->main_uv.uvs[n].y;
			if (poly->main_uv.uvs[n].y>max_gy) max_gy=poly->main_uv.uvs[n].y;
		}
		
			// find the hit points in the box
			
		fx=(float)(hpt.x-poly->box.min.x)/(float)(poly->box.max.x-poly->box.min.x);
		fx=min_gx+(fx*(max_gx-min_gx));
	
		fy=(float)(hpt.z-poly->box.min.z)/(float)(poly->box.max.z-poly->box.min.z);
		fy=min_gy+(fy*(max_gy-min_gy));
	}

		// get the alpha

	fx-=(float)floor(fx);
	fy-=(float)floor(fy);

	x=(int)(((float)bitmap->wid)*fx);
	y=(int)(((float)bitmap->high)*fy);
	
	ptr=liquid_reflection_map_map_texture[txt_idx].data+((x*3)+(y*(bitmap->wid*3)));
	
	col->r=((float)*ptr++)/255.0f;
	col->g=((float)*ptr++)/255.0f;
	col->b=((float)*ptr)/255.0f;
}

/* =======================================================

      Liquid Reflection Map Ray Trace
      
======================================================= */

float liquid_reflection_map_ray_trace_mesh_polygon(d3pnt *spt,d3vct *vct,map_mesh_type *mesh,map_mesh_poly_type *poly,d3col *col)
{
	int			n,trig_count;
	int			px[3],py[3],pz[3];
	float		hit_t;
	d3pnt		*m_pt;
	
		// first vertex is always 0

	m_pt=&mesh->vertexes[poly->v[0]];
		
	px[0]=m_pt->x;
	py[0]=m_pt->y;
	pz[0]=m_pt->z;
	
		// run through all the triangles of the polygon
		
	trig_count=poly->ptsz-2;
	
	for (n=0;n<trig_count;n++) {
		m_pt=&mesh->vertexes[poly->v[n+1]];
		px[1]=m_pt->x;
		py[1]=m_pt->y;
		pz[1]=m_pt->z;

		m_pt=&mesh->vertexes[poly->v[n+2]];
		px[2]=m_pt->x;
		py[2]=m_pt->y;
		pz[2]=m_pt->z;
		
			// check for hit
			
		hit_t=light_map_ray_trace_triangle(spt,vct,px,py,pz);
		
			// ignore 0.0 hits to stop edge to edge polygon
			// collisions
		
		if ((hit_t>0.0f) && (hit_t<=1.0f)) {
			liquid_reflection_map_bitmap_pixel_get(spt,vct,mesh,poly,hit_t,col);
			return(hit_t);
		}
	}
	
	return(-1.0f);
}

void liquid_reflection_map_ray_trace_map(d3pnt *spt,d3pnt *ept,d3col *col)
{
	int							n,k;
	float						hit_t,cur_hit_t;
	d3pnt						min,max;
	d3vct						vct;
	d3col						hit_col;
	map_mesh_type				*mesh;
	map_mesh_poly_type			*poly;
	
		// ray trace vector
		
	vct.x=(float)(ept->x-spt->x);
	vct.y=(float)(ept->y-spt->y);
	vct.z=(float)(ept->z-spt->z);
	
		// ray trace bounds
		
	if (spt->x<ept->x) {
		min.x=spt->x;
		max.x=ept->x;
	}
	else {
		min.x=ept->x;
		max.x=spt->x;
	}
	if (spt->y<ept->y) {
		min.y=spt->y;
		max.y=ept->y;
	}
	else {
		min.y=ept->y;
		max.y=spt->y;
	}
	if (spt->z<ept->z) {
		min.z=spt->z;
		max.z=ept->z;
	}
	else {
		min.z=ept->z;
		max.z=spt->z;
	}
	
		// look for poly collisions
	
	cur_hit_t=-1.0f;
	
	for (n=0;n!=map.mesh.nmesh;n++) {
		mesh=&map.mesh.meshes[n];
	
		if ((mesh->box.min.x>max.x) || (mesh->box.max.x<min.x)) continue;
		if ((mesh->box.min.y>max.y) || (mesh->box.max.y<min.y)) continue;
		if ((mesh->box.min.z>max.z) || (mesh->box.max.z<min.z)) continue;
			
		for (k=0;k!=mesh->npoly;k++) {
			poly=&mesh->polys[k];
		
			if ((poly->box.min.x>max.x) || (poly->box.max.x<min.x)) continue;
			if ((poly->box.min.y>max.y) || (poly->box.max.y<min.y)) continue;
			if ((poly->box.min.z>max.z) || (poly->box.max.z<min.z)) continue;
			
			hit_t=liquid_reflection_map_ray_trace_mesh_polygon(spt,&vct,mesh,poly,&hit_col);
			if (hit_t!=-1.0f) {
				if ((cur_hit_t==-1.0f) || (hit_t<cur_hit_t)) {
					cur_hit_t=hit_t;
					memmove(col,&hit_col,sizeof(d3col));
				}
			}
		}
	}
}

/* =======================================================

      Liquid Reflection Map Render
      
======================================================= */

bool liquid_reflection_map_run_for_liquid(int txt_idx,int liq_idx,char *base_path,char *map_name,char *err_str)
{
	int							x,z,x_add,z_add,sz,pixel_size;
	unsigned char				*pixel,*pixel_data,uc_alpha;
	char						path[1024],bitmap_name[256];
	d3pnt						spt,ept,center;
	d3col						col;
	map_liquid_type				*liq;
	
		// get the liquid to render
		
	liq=&map.liquid.liquids[liq_idx];

	pixel_size=(int)pow(2,(liq->reflect.texture_size+8));

		// get the pixel data for texture

	sz=(pixel_size*pixel_size)*4;
	pixel_data=(unsigned char*)malloc(sz);
	bzero(pixel_data,sz);
	
	uc_alpha=(unsigned char)((int)(liq->reflect.alpha*255.0f));

		// get scan ratio

	x_add=((liq->rgt-1)-liq->lft)/pixel_size;
	z_add=((liq->bot-1)-liq->top)/pixel_size;

	center.x=(liq->lft+liq->rgt)>>1;
	center.z=(liq->top+liq->bot)>>1;

		// scan through the quad

	pixel=pixel_data;

	spt.y=liq->y;
	spt.z=liq->top;
	
	for (z=0;z!=pixel_size;z++) {

		spt.x=liq->lft;

		for (x=0;x!=pixel_size;x++) {

				// create the ray.  The further from
				// the center, the more it slants out

			ept.x=spt.x+(((spt.x-center.x)*liq->reflect.x_refract_factor)/(center.x-liq->lft));
			ept.y=spt.y-liquid_reflection_mirror_distance;
			ept.z=spt.z+(((spt.z-center.z)*liq->reflect.z_refract_factor)/(center.z-liq->top));

				// run the ray trace

			memmove(&col,&liq->reflect.no_hit_col,sizeof(d3col));
			liquid_reflection_map_ray_trace_map(&spt,&ept,&col);

				// set color
				
			*pixel++=(unsigned char)((int)(col.r*255.0f));
			*pixel++=(unsigned char)((int)(col.g*255.0f));
			*pixel++=(unsigned char)((int)(col.b*255.0f));
			*pixel++=uc_alpha;

			spt.x+=x_add;
		}

		spt.z+=z_add;
	}

		// save the texture

	sprintf(map.textures[txt_idx].material_name,"Light Map %.3d",liq_idx);

	map.textures[txt_idx].shader_name[0]=0x0;
			
	map.textures[txt_idx].additive=FALSE;
	map.textures[txt_idx].compress=FALSE;
		
		// save bitmap
		
	sprintf(bitmap_name,"ReflectionMaps/%s/rm%.3d",map_name,liq_idx);
	sprintf(path,"%s/%s.png",base_path,bitmap_name);
	bitmap_write_png_data(pixel_data,pixel_size,pixel_size,TRUE,path);

	liq->txt_idx=txt_idx;
	liq->flag.lock_uv=TRUE;
	liq->main_uv.offset.x=liq->main_uv.offset.y=0.0f;
	liq->main_uv.size.x=liq->main_uv.size.y=1.0f;
		
		// put in texture list
			
	map_replace_texture(&map,txt_idx,bitmap_name);
	
	return(TRUE);
}

/* =======================================================

      Setup Meshes for Ray Tracing
      
======================================================= */

void liquid_reflection_map_setup(void)
{
	int					n,k;
	map_mesh_type		*mesh;
	map_mesh_poly_type	*poly;

	mesh=map.mesh.meshes;

	for (n=0;n!=map.mesh.nmesh;n++) {
			
		map_prepare_mesh_box(mesh);

		poly=mesh->polys;
	
		for (k=0;k!=mesh->npoly;k++) {
			map_prepare_mesh_poly(&map,mesh,poly);
			poly++;
		}

		mesh++;
	}
}

/* =======================================================

      Run Liquid Reflection Maps
      
======================================================= */

bool liquid_reflection_maps_create_process(int nliq,char *err_str)
{
	int				n,txt_idx;
	char			base_path[1024],dir_path[1024],
					map_name[256],str[256];
	
		// base path
		
	file_paths_data_default(&file_path_setup,base_path,"Bitmaps/Textures",NULL,NULL);
	
		// create folders if they don't exist
		
	sprintf(dir_path,"%s/ReflectionMaps",base_path);
	os_create_directory(dir_path);
	
	light_map_texture_map_folder_name(map_name);
	
	sprintf(dir_path,"%s/ReflectionMaps/%s",base_path,map_name);
	os_create_directory(dir_path);
		
		// clear the textures and
		// start mesh-poly and/or liquid setup
		
	progress_next_title("Liquid Reflection Map: Setup");
		
	map_delete_texture(&map,liquid_reflection_texture_start_idx,(liquid_reflection_texture_start_idx+max_liquid_reflection_map_textures));
	
	liquid_reflection_map_setup();
	liquid_reflection_map_bitmap_pixel_start();
	
		// create a ray traced light map
		// for all the polys

	txt_idx=liquid_reflection_texture_start_idx;
		
	for (n=0;n!=map.liquid.nliquid;n++) {

		sprintf(str,"Liquid Reflection Map: Rendering Liquid %d/%d",(n+1),nliq);
		progress_next_title(str);
			
		if (!liquid_reflection_map_run_for_liquid(txt_idx,n,base_path,map_name,err_str)) {
			liquid_reflection_map_bitmap_pixel_free();
			return(FALSE);
		}
		
		txt_idx++;
	}
	
		// free textures and polys
		
	progress_next_title("Liquid Reflection Map: Finishing");

	liquid_reflection_map_bitmap_pixel_free();

	main_wind_draw();
	
	return(TRUE);
}

bool liquid_reflection_maps_create(void)
{
	bool			ok;
	char			err_str[256];

	if (os_dialog_confirm("Build Liquid Reflection Maps","Building liquids reflection maps will replace the 6th sets of textures and save the map.\nAre you sure you want to build the liquid reflection maps?",FALSE)!=0) return(FALSE);
	
		// see if there are any liquids
		
	if (map.liquid.nliquid==0) {
		os_dialog_alert("Can not build liquid reflection maps","There are no liquids set to build reflection maps in this map.");
		return(FALSE);
	}

	if (map.liquid.nliquid>max_liquid_reflection_map_textures) {
		os_dialog_alert("Can not build liquid reflection maps","There are too many liquids set to build reflection maps in this map.");
		return(FALSE);
	}
	
		// generate the reflection maps
		
	progress_start("Generating Liquid Reflection Maps...",(2+map.liquid.nliquid));
	ok=liquid_reflection_maps_create_process(map.liquid.nliquid,err_str);
	progress_end();

	if (!ok) {
		os_dialog_alert("Can not build liquid reflection maps",err_str);
		return(FALSE);
	}

	file_save_map();
	
	return(TRUE);
}

