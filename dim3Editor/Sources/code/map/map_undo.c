/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Map Undo

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

extern int							nselect_item;
extern select_item_type				select_items[select_max_item];

extern map_type						map;

int					map_undo_level=0;
map_undo_type		map_undos[max_map_undo_level];

/* =======================================================

      Initialize Undo
      
======================================================= */

void map_undo_initialize_chunk(map_undo_chunk_type *chunk)
{
	chunk->count=0;
	chunk->data=NULL;
}

void map_undo_initialize_single(map_undo_type *undo)
{
	map_undo_initialize_chunk(&undo->mesh);
	map_undo_initialize_chunk(&undo->liquid);
	map_undo_initialize_chunk(&undo->spot);
	map_undo_initialize_chunk(&undo->scenery);
	map_undo_initialize_chunk(&undo->node);
	map_undo_initialize_chunk(&undo->light);
	map_undo_initialize_chunk(&undo->sound);
	map_undo_initialize_chunk(&undo->particle);
	map_undo_initialize_chunk(&undo->selection);
}

void map_undo_initialize(void)
{
	int				n;
	map_undo_type	*undo;
	
	map_undo_level=0;
	
	undo=map_undos;
	
	for (n=0;n!=max_map_undo_level;n++) {
		map_undo_initialize_single(undo);
		undo++;
	}
}

/* =======================================================

      Clear Undos
      
======================================================= */

void map_undo_clear_chunck(map_undo_chunk_type *chunk)
{
	if (chunk->data!=NULL) free(chunk->data);
	
	chunk->count=0;
	chunk->data=NULL;
}

void map_undo_clear_single(map_undo_type *undo)
{
	int				n;
	map_mesh_type	*mesh;
	
		// clear mesh vertexes and polygons
	
	if (undo->mesh.data!=NULL) {
	
		mesh=(map_mesh_type*)undo->mesh.data;
		
		for (n=0;n!=undo->mesh.count;n++) {
			if (mesh->vertexes!=NULL) free(mesh->vertexes);
			if (mesh->polys!=NULL) free(mesh->polys);
			
			mesh++;
		}
	}
	
	map_undo_clear_chunck(&undo->mesh);
	
	map_undo_clear_chunck(&undo->liquid);
	map_undo_clear_chunck(&undo->spot);
	map_undo_clear_chunck(&undo->scenery);
	map_undo_clear_chunck(&undo->node);
	map_undo_clear_chunck(&undo->light);
	map_undo_clear_chunck(&undo->sound);
	map_undo_clear_chunck(&undo->particle);
	map_undo_clear_chunck(&undo->selection);
}

void map_undo_clear(void)
{
	int				n;
	map_undo_type	*undo;
	
	undo=map_undos;
	
	for (n=0;n!=max_map_undo_level;n++) {
		map_undo_clear_single(undo);
		undo++;
	}
	
	os_menu_enable_item(map_menu_edit,1,FALSE);
}

/* =======================================================

      Copy Undos
      
======================================================= */

void undo_copy_chunk(map_undo_chunk_type *dest_chunk,map_undo_chunk_type *srce_chunk)
{
	dest_chunk->count=srce_chunk->count;
	dest_chunk->data=srce_chunk->data;
}

void undo_copy(map_undo_type *dest_undo,map_undo_type *srce_undo)
{
	undo_copy_chunk(&dest_undo->mesh,&srce_undo->mesh);
	undo_copy_chunk(&dest_undo->liquid,&srce_undo->liquid);
	undo_copy_chunk(&dest_undo->spot,&srce_undo->spot);
	undo_copy_chunk(&dest_undo->scenery,&srce_undo->scenery);
	undo_copy_chunk(&dest_undo->node,&srce_undo->node);
	undo_copy_chunk(&dest_undo->light,&srce_undo->light);
	undo_copy_chunk(&dest_undo->sound,&srce_undo->sound);
	undo_copy_chunk(&dest_undo->particle,&srce_undo->particle);
	undo_copy_chunk(&dest_undo->selection,&srce_undo->selection);
}

/* =======================================================

      Push Undo
      
======================================================= */

bool map_undo_push_internal_chunk(int count,int size,unsigned char *data,map_undo_chunk_type *chunk)
{
	int				sz;
	
	if (count==0) return(TRUE);
	
	sz=count*size;
	
	chunk->data=(unsigned char*)malloc(sz);
	if (chunk->data==NULL) return(FALSE);
		
	memmove(chunk->data,data,sz);
	chunk->count=count;
	
	return(TRUE);
}

bool map_undo_push_internal(void)
{
	int				n,nmesh;
	map_undo_type	*undo;
	map_mesh_type	*org_mesh,*mesh;
	
		// clear last undo
	
	map_undo_clear_single(&map_undos[max_map_undo_level-1]);
	
		// push undo stack down
		
	for (n=(max_map_undo_level-2);n>=0;n--) {
		undo_copy(&map_undos[n+1],&map_undos[n]);
	}
	
		// start with clear
		
	undo=&map_undos[0];
	map_undo_initialize_single(undo);
	
		// save all meshes
		
	nmesh=map.mesh.nmesh;
	if (nmesh!=0) {
	
		undo->mesh.data=(unsigned char*)malloc(nmesh*sizeof(map_mesh_type));
		if (undo->mesh.data==NULL) return(FALSE);
				
		org_mesh=map.mesh.meshes;
		mesh=(map_mesh_type*)undo->mesh.data;
		
		for (n=0;n!=nmesh;n++) {
		
			memmove(mesh,org_mesh,sizeof(map_mesh_type));
			
				// need to dup the vertexes and polygons
				
			mesh->vertexes=(d3pnt*)malloc(org_mesh->nvertex*sizeof(d3pnt));
			if (mesh->vertexes==NULL) {
				free(undo->mesh.data);
				undo->mesh.data=NULL;
				return(FALSE);
			}
			
			memmove(mesh->vertexes,org_mesh->vertexes,(org_mesh->nvertex*sizeof(d3pnt)));
			
			mesh->polys=(map_mesh_poly_type*)malloc(org_mesh->npoly*sizeof(map_mesh_poly_type));
			if (mesh->polys==NULL) {
				free(undo->mesh.data);
				undo->mesh.data=NULL;
				return(FALSE);
			}
			
			memmove(mesh->polys,org_mesh->polys,(org_mesh->npoly*sizeof(map_mesh_poly_type)));
						
			org_mesh++;
			mesh++;
		}
		
	}
	
	undo->mesh.count=nmesh;
	
		// liquids
		// we can do it simply on the push as there are
		// no internal mallocs in the liquids structure
		
	if (!map_undo_push_internal_chunk(map.liquid.nliquid,sizeof(map_liquid_type),(unsigned char*)map.liquid.liquids,&undo->liquid)) return(FALSE);
	
		// save other chunks
		
	if (!map_undo_push_internal_chunk(map.nspot,sizeof(spot_type),(unsigned char*)map.spots,&undo->spot)) return(FALSE);
	if (!map_undo_push_internal_chunk(map.nscenery,sizeof(map_scenery_type),(unsigned char*)map.sceneries,&undo->scenery)) return(FALSE);
	if (!map_undo_push_internal_chunk(map.nnode,sizeof(node_type),(unsigned char*)map.nodes,&undo->node)) return(FALSE);
	if (!map_undo_push_internal_chunk(map.nlight,sizeof(map_light_type),(unsigned char*)map.lights,&undo->light)) return(FALSE);
	if (!map_undo_push_internal_chunk(map.nsound,sizeof(map_sound_type),(unsigned char*)map.sounds,&undo->sound)) return(FALSE);
	if (!map_undo_push_internal_chunk(map.nparticle,sizeof(map_particle_type),(unsigned char*)map.particles,&undo->particle)) return(FALSE);
	if (!map_undo_push_internal_chunk(nselect_item,sizeof(select_item_type),(unsigned char*)select_items,&undo->selection)) return(FALSE);
	
		// move up undo level
		
	map_undo_level++;
	if (map_undo_level>max_map_undo_level) map_undo_level=max_map_undo_level;
	
	return(TRUE);
}

void map_undo_push(void)
{
	if (map_undo_push_internal()) {
		os_menu_enable_item(map_menu_edit,1,TRUE);
		return;
	}
	
	os_dialog_alert("Undo","Not enough memory to setup undo");
	map_undo_clear();
	
	os_menu_enable_item(map_menu_edit,1,FALSE);
}

/* =======================================================

      Push Undo
      
======================================================= */

int map_undo_pull_chunk(int size,unsigned char *data,map_undo_chunk_type *chunk)
{
	if (chunk->count!=0) memmove(data,chunk->data,(chunk->count*size));
	return(chunk->count);
}

void map_undo_pull(void)
{
	int				n,idx,nmesh,old_nmesh,
					nliquid,old_nliquid;
	map_undo_type	*undo;
	map_mesh_type	*old_meshes,*org_mesh,*mesh;
	map_liquid_type	*old_liquids,*org_liq,*liq;
	
	if (map_undo_level==0) return;
	
	undo=&map_undos[0];

		// free VBOs

	view_vbo_map_free();
	
		// backup old meshes
		// and clear mesh list
		
	old_nmesh=map.mesh.nmesh;
	old_meshes=map.mesh.meshes;
	
	free(map.mesh.meshes);
	
	map.mesh.nmesh=0;
	map.mesh.meshes=NULL;
	
		// restore all meshes
		
	nmesh=undo->mesh.count;
		
	if (nmesh!=0) {
	
		org_mesh=(map_mesh_type*)undo->mesh.data;
		
		for (n=0;n!=nmesh;n++) {
		
				// new mesh
				// if it fails, put everything back
				// and return
				
			idx=map_mesh_add(&map);
			if (idx==-1) {
				free(map.mesh.meshes);
				
				map.mesh.nmesh=old_nmesh;
				map.mesh.meshes=old_meshes;
				return;
			}
			
			mesh=&map.mesh.meshes[idx];
			
				// just copy over from backup mesh
				// and then null out the pointers
				// so close doesn't erase them
				
			if (mesh->vertexes!=NULL) free(mesh->vertexes);
			if (mesh->polys!=NULL) free(mesh->polys);
				
			memmove(mesh,org_mesh,sizeof(map_mesh_type));

			org_mesh->vertexes=NULL;
			org_mesh->polys=NULL;
						
			org_mesh++;
		}
	}
		
	map.mesh.nmesh=nmesh;
	
		// backup old liquids
		// and clear liquid list
		
	old_nliquid=map.liquid.nliquid;
	old_liquids=map.liquid.liquids;
	
	free(map.liquid.liquids);
	
	map.liquid.nliquid=0;
	map.liquid.liquids=NULL;
	
		// restore all meshes
		
	nliquid=undo->liquid.count;
		
	if (nliquid!=0) {
	
		org_liq=(map_liquid_type*)undo->liquid.data;
		
		for (n=0;n!=nliquid;n++) {
		
				// new liquid
				// if it fails, put everything back
				// and return
				
			idx=map_liquid_add(&map);
			if (idx==-1) {
				free(map.liquid.liquids);
				
				map.liquid.nliquid=old_nliquid;
				map.liquid.liquids=old_liquids;
				return;
			}
			
			liq=&map.liquid.liquids[idx];
			memmove(liq,org_liq,sizeof(map_liquid_type));
						
			org_liq++;
		}
	}
		
	map.liquid.nliquid=nliquid;
	
		// restore other chunks
	
	map.nspot=map_undo_pull_chunk(sizeof(spot_type),(unsigned char*)map.spots,&undo->spot);
	map.nscenery=map_undo_pull_chunk(sizeof(map_scenery_type),(unsigned char*)map.sceneries,&undo->scenery);
	map.nnode=map_undo_pull_chunk(sizeof(node_type),(unsigned char*)map.nodes,&undo->node);
	map.nlight=map_undo_pull_chunk(sizeof(map_light_type),(unsigned char*)map.lights,&undo->light);
	map.nsound=map_undo_pull_chunk(sizeof(map_sound_type),(unsigned char*)map.sounds,&undo->sound);
	map.nparticle=map_undo_pull_chunk(sizeof(map_particle_type),(unsigned char*)map.particles,&undo->particle);
	nselect_item=map_undo_pull_chunk(sizeof(select_item_type),(unsigned char*)select_items,&undo->selection);
	
		// clear level
		// and move stack up
		
	map_undo_clear_single(&map_undos[0]);
	
	for (n=1;n!=max_map_undo_level;n++) {
		undo_copy(&map_undos[n-1],&map_undos[n]);
	}
	
	if (max_map_undo_level>1) map_undo_initialize_single(&map_undos[max_map_undo_level-1]);
	
		// move down undo level
		
	map_undo_level--;
	if (map_undo_level==0) os_menu_enable_item(map_menu_edit,1,FALSE);

		// rebuild VBOs

	view_vbo_map_initialize();
	
		// redraw windows
		
	map_palette_reset();

	main_wind_draw();
}
