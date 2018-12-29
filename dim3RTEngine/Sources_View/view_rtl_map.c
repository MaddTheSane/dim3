/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: View dim3RTL Map

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
#include "ray_interface.h"

extern map_type				map;
extern server_type			server;
extern camera_type			camera;
extern view_type			view;
extern iface_type			iface;
extern setup_type			setup;
extern network_setup_type	net_setup;
extern file_path_setup_type	file_path_setup;

extern int					view_rtl_draw_scene_id;

extern int view_dim3rtl_create_material_from_texture(char *sub_path,texture_type *texture,texture_frame_type *frame);

/* =======================================================

      dim3RTL Map Mesh Setup
      
======================================================= */

void view_dim3rtl_map_mesh_start(void)
{
	int					n,k,i,t,uv_count,mesh_id,light_id;
	float				*vp,*vu,*vn;
	short				*vk,*ray_polys;
	d3pnt				*pnt;
	map_mesh_type		*mesh;
	map_mesh_poly_type	*poly;
	texture_type		*texture;
	texture_frame_type	*frame;
	map_light_type		*lit;
	rtlPoint			lit_pnt;
	rtlColor			lit_col;

		// build the materials
		
	for (n=0;n!=max_map_texture;n++) {
		texture=&map.textures[n];
		
		frame=&texture->frames[0];
		if (frame->name[0]==0x0) continue;
		
		frame->bitmap.rl_material_id=view_dim3rtl_create_material_from_texture("Bitmaps/Textures",texture,frame);

		progress_update();
	}
		
		// build the meshes

	for (n=0;n!=map.mesh.nmesh;n++) {
		mesh=&map.mesh.meshes[n];
		if (!mesh->flag.on) continue;

			// this flag determines when a mesh
			// has been moved so we only update
			// at that time

		mesh->draw.moved=FALSE;
			
			// add the mesh

		mesh_id=rtlSceneMeshAdd(view_rtl_draw_scene_id,0);
		if (mesh_id<0) continue;

		mesh->rtl_mesh_id=mesh_id;

			// the vertexes
		
		rtlSceneMeshSetVertex(view_rtl_draw_scene_id,mesh_id,RL_MESH_FORMAT_VERTEX_3_FLOAT,mesh->nvertex,NULL);
		k=rtlSceneMeshMapVertexPointer(view_rtl_draw_scene_id,mesh_id,(void**)&vp);

		pnt=mesh->vertexes;

		for (k=0;k!=mesh->nvertex;k++) {
			*vp++=(float)pnt->x;
			*vp++=(float)pnt->y;
			*vp++=(float)pnt->z;
			pnt++;
		}

		rtlSceneMeshUnMapVertexPointer(view_rtl_draw_scene_id,mesh_id);

			// the UVs

		uv_count=0;
		poly=mesh->polys;
	
		for (i=0;i!=mesh->npoly;i++) {
			uv_count+=poly->ptsz;
			poly++;
		}

		rtlSceneMeshSetUV(view_rtl_draw_scene_id,mesh_id,RL_MESH_FORMAT_UV_2_FLOAT,uv_count,NULL);
		rtlSceneMeshMapUVPointer(view_rtl_draw_scene_id,mesh_id,(void**)&vu);

		poly=mesh->polys;
	
		for (i=0;i!=mesh->npoly;i++) {
			for (t=0;t!=poly->ptsz;t++) {
				*vu++=poly->main_uv.uvs[t].x;
				*vu++=poly->main_uv.uvs[t].y;
			}
			poly++;
		}

		rtlSceneMeshUnMapUVPointer(view_rtl_draw_scene_id,mesh_id);

			// the normals

		rtlSceneMeshSetNormal(view_rtl_draw_scene_id,mesh_id,RL_MESH_FORMAT_NORMAL_3_FLOAT,mesh->npoly,NULL);
		rtlSceneMeshMapNormalPointer(view_rtl_draw_scene_id,mesh_id,(void**)&vn);

		poly=mesh->polys;
	
		for (i=0;i!=mesh->npoly;i++) {
			*vn++=poly->tangent_space.normal.x;
			*vn++=poly->tangent_space.normal.y;
			*vn++=poly->tangent_space.normal.z;
			poly++;
		}

		rtlSceneMeshUnMapNormalPointer(view_rtl_draw_scene_id,mesh_id);

			// the tangents

		rtlSceneMeshSetTangent(view_rtl_draw_scene_id,mesh_id,RL_MESH_FORMAT_TANGENT_3_FLOAT,mesh->npoly,NULL);
		rtlSceneMeshMapTangentPointer(view_rtl_draw_scene_id,mesh_id,(void**)&vn);

		poly=mesh->polys;
	
		for (i=0;i!=mesh->npoly;i++) {
			*vn++=poly->tangent_space.tangent.x;
			*vn++=poly->tangent_space.tangent.y;
			*vn++=poly->tangent_space.tangent.z;
			poly++;
		}

		rtlSceneMeshUnMapTangentPointer(view_rtl_draw_scene_id,mesh_id);

			// polygons

		ray_polys=(short*)malloc((mesh->npoly*(2+(4*8)))*sizeof(short));		// supergumba -- this will work but chews up a lot of memory
		vk=ray_polys;

		uv_count=0;
		poly=mesh->polys;
	
		for (i=0;i!=mesh->npoly;i++) {
			*vk++=poly->ptsz;
			*vk++=(short)map.textures[poly->txt_idx].frames[0].bitmap.rl_material_id;

			for (t=0;t!=poly->ptsz;t++) {
				*vk++=(short)poly->v[t];	// vertex
				*vk++=uv_count;				// uv, each vertex has unique uv count
				*vk++=(short)i;				// normal, one normal for each poly
				*vk++=(short)i;				// tangent, one tangent for each poly
				uv_count++;
			}

			poly++;
		}

		rtlSceneMeshSetPoly(view_rtl_draw_scene_id,mesh_id,RL_MESH_FORMAT_POLY_SHORT_VERTEX_UV_NORMAL_TANGENT,mesh->npoly,ray_polys);
		free(ray_polys);

		progress_update();
	}

		// the ambient

	lit_col.r=map.ambient.light_color.r;
	lit_col.g=map.ambient.light_color.g;
	lit_col.b=map.ambient.light_color.b;
	rtlSceneAmbient(view_rtl_draw_scene_id,&lit_col);
	
		// build the lights
		
	for (n=0;n!=map.nlight;n++) {
		lit=&map.lights[n];
		
		light_id=rtlSceneLightAdd(view_rtl_draw_scene_id);
		rtlSceneLightSetIntensity(view_rtl_draw_scene_id,light_id,(float)lit->setting.intensity,lit->setting.exponent);
	
		lit_col.r=lit->setting.col.r;
		lit_col.g=lit->setting.col.g;
		lit_col.b=lit->setting.col.b;
		rtlSceneLightSetColor(view_rtl_draw_scene_id,light_id,&lit_col);

		lit_pnt.x=(float)lit->pnt.x;
		lit_pnt.y=(float)lit->pnt.y;
		lit_pnt.z=(float)lit->pnt.z;
		rtlSceneLightSetPosition(view_rtl_draw_scene_id,light_id,&lit_pnt);

		lit->rtl_light_id=light_id;

		progress_update();
	}
}

void view_dim3rtl_map_mesh_stop(void)
{
	int					n;
	map_mesh_type		*mesh;
	texture_type		*texture;
	texture_frame_type	*frame;
	
		// delete meshes

	for (n=0;n!=map.mesh.nmesh;n++) {
		mesh=&map.mesh.meshes[n];
		if (!mesh->flag.on) continue;
		
		rtlSceneMeshDelete(view_rtl_draw_scene_id,mesh->rtl_mesh_id);
		progress_update();
	}

		// delete lights

	for (n=0;n!=map.nlight;n++) {
		rtlSceneLightDelete(view_rtl_draw_scene_id,map.lights[n].rtl_light_id);
		progress_update();
	}

		// delete materials
		// we do this afterwards because we can't
		// delete attached materials
		
	for (n=0;n!=max_map_texture;n++) {
		texture=&map.textures[n];
		
		frame=&texture->frames[0];
		if (frame->name[0]==0x0) continue;
		
		if (frame->bitmap.rl_material_id!=-1) rtlMaterialDelete(frame->bitmap.rl_material_id);
		progress_update();
	}
}

/* =======================================================

      dim3RTL Map Mesh Update
      
======================================================= */

void view_dim3rtl_map_mesh_update(void)
{
	int					n,k;
	float				*vp,*vn;
	d3pnt				*pnt;
	map_mesh_type		*mesh;
	map_mesh_poly_type	*poly;

		// rebuild meshes that have
		// moved since last draw

	for (n=0;n!=map.mesh.nmesh;n++) {
		mesh=&map.mesh.meshes[n];
		if (!mesh->flag.on) continue;

			// has this mesh been moved?

		if (!mesh->flag.moveable) continue;
		if (!mesh->draw.moved) continue;

			// reset vertexes

		rtlSceneMeshMapVertexPointer(view_rtl_draw_scene_id,mesh->rtl_mesh_id,(void**)&vp);

		pnt=mesh->vertexes;

		for (k=0;k!=mesh->nvertex;k++) {
			*vp++=(float)pnt->x;
			*vp++=(float)pnt->y;
			*vp++=(float)pnt->z;
			pnt++;
		}

		rtlSceneMeshUnMapVertexPointer(view_rtl_draw_scene_id,mesh->rtl_mesh_id);

			// the normals

		rtlSceneMeshMapNormalPointer(view_rtl_draw_scene_id,mesh->rtl_mesh_id,(void**)&vn);

		poly=mesh->polys;
	
		for (k=0;k!=mesh->npoly;k++) {
			*vn++=poly->tangent_space.normal.x;
			*vn++=poly->tangent_space.normal.y;
			*vn++=poly->tangent_space.normal.z;
			poly++;
		}

		rtlSceneMeshUnMapNormalPointer(view_rtl_draw_scene_id,mesh->rtl_mesh_id);

			// the tangents

		rtlSceneMeshMapTangentPointer(view_rtl_draw_scene_id,mesh->rtl_mesh_id,(void**)&vn);

		poly=mesh->polys;
	
		for (k=0;k!=mesh->npoly;k++) {
			*vn++=poly->tangent_space.tangent.x;
			*vn++=poly->tangent_space.tangent.y;
			*vn++=poly->tangent_space.tangent.z;
			poly++;
		}

		rtlSceneMeshUnMapTangentPointer(view_rtl_draw_scene_id,mesh->rtl_mesh_id);

			// reset moved flag

		mesh->draw.moved=FALSE;
	}
}
