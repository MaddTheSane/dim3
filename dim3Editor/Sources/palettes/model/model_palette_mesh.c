/****************************** File *********************************

Module: dim3 Animator
Author: Brian Barnes
 Usage: Property Palette Mesh

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

#define kMeshPropertyName					0
#define kMeshPropertyDiffuse				1
#define kMeshPropertyNoLighting				2
#define kMeshPropertyAdditive				3
#define kMeshPropertyCull					4
#define kMeshPropertyLocked					5
#define kMeshPropertyRTNonLightBlocking		6
#define kMeshPropertyMovement				7

extern model_type				model;
extern file_path_setup_type		file_path_setup;

extern list_palette_type		model_palette;

int								sel_vertex_idx,sel_poly_idx;

/* =======================================================

      Property Palette Fill Mesh
      
======================================================= */

void model_palette_fill_mesh(int mesh_idx)
{
	model_mesh_type		*mesh;
	model_vertex_type	*vertex;
	model_poly_type		*poly;
	
	mesh=&model.meshes[mesh_idx];

	list_palette_set_title(&model_palette,"Mesh",mesh->name,NULL,NULL,NULL,NULL);

	list_palette_add_header(&model_palette,0,"Mesh Options");
	list_palette_add_string(&model_palette,kMeshPropertyName,"Name",mesh->name,name_str_len,FALSE);

	list_palette_add_header(&model_palette,0,"Mesh Settings");
	list_palette_add_checkbox(&model_palette,kMeshPropertyDiffuse,"Diffuse Lighting",&mesh->diffuse,FALSE);
	list_palette_add_checkbox(&model_palette,kMeshPropertyNoLighting,"Highlighted",&mesh->no_lighting,FALSE);
	list_palette_add_checkbox(&model_palette,kMeshPropertyAdditive,"Alpha is Additive",&mesh->blend_add,FALSE);
	list_palette_add_checkbox(&model_palette,kMeshPropertyCull,"Never Cull",&mesh->never_cull,FALSE);
	list_palette_add_checkbox(&model_palette,kMeshPropertyLocked,"Locked",&mesh->locked,FALSE);
	list_palette_add_checkbox(&model_palette,kMeshPropertyLocked,"RT Non-Light Blocking",&mesh->rt_non_light_blocking,FALSE);

	list_palette_add_header(&model_palette,0,"Replace OBJ");
	list_palette_add_point(&model_palette,kMeshPropertyMovement,"Movement",&mesh->import_move,FALSE);

	list_palette_add_header(&model_palette,0,"Mesh Info");
	list_palette_add_int(&model_palette,-1,"Vertexes",&mesh->nvertex,TRUE);
	list_palette_add_int(&model_palette,-1,"Polygons",&mesh->npoly,TRUE);
	
	sel_vertex_idx=model_vertex_mask_get_first_sel(mesh_idx);
	if (sel_vertex_idx!=-1) {
		vertex=&mesh->vertexes[sel_vertex_idx];
		list_palette_add_header(&model_palette,0,"Selected Vertex Info");
		list_palette_add_int(&model_palette,-1,"Index",&sel_vertex_idx,TRUE);
		list_palette_add_point(&model_palette,-1,"Position",&vertex->pnt,TRUE);
		list_palette_add_vector(&model_palette,-1,"Normal",&vertex->tangent_space.normal,TRUE);
		list_palette_add_vector(&model_palette,-1,"Tangent",&vertex->tangent_space.tangent,TRUE);
	}
	
	sel_poly_idx=model_poly_mask_get_first_sel(mesh_idx);
	if (sel_poly_idx!=-1) {
		poly=&mesh->polys[sel_poly_idx];
		list_palette_add_header(&model_palette,0,"Selected Poly Info");
		list_palette_add_int(&model_palette,-1,"Index",&sel_poly_idx,TRUE);
		list_palette_add_int(&model_palette,-1,"Points",&poly->ptsz,TRUE);
	}

}

/* =======================================================

      Property Palette Click Mesh
      
======================================================= */

void model_palette_click_mesh(int mesh_idx,bool double_click)
{
	int						n;
	d3pnt					move_pnt;
	model_vertex_type		*vtx;
	model_bone_type			*bone;
	model_mesh_type			*mesh;

	mesh=&model.meshes[mesh_idx];
	
	if (!double_click) return;

		// regular clicks

	switch (model_palette.item_pane.click.id) {

		case kMeshPropertyMovement:
		
				// get change
				
			move_pnt.x=mesh->import_move.x-mesh->org_import_move.x;
			move_pnt.y=mesh->import_move.y-mesh->org_import_move.y;
			move_pnt.z=mesh->import_move.z-mesh->org_import_move.z;
			memmove(&mesh->org_import_move,&mesh->import_move,sizeof(d3pnt));
			
				// now set the vertexes
				
			vtx=mesh->vertexes;

			for (n=0;n!=mesh->nvertex;n++) {
				vtx->pnt.x+=move_pnt.x;
				vtx->pnt.y+=move_pnt.y;
				vtx->pnt.z+=move_pnt.z;

				vtx++;
			}

				// if this is mesh 0, then move the
				// bones

			if (mesh_idx==0) {

				bone=model.bones;

				for (n=0;n!=model.nbone;n++) {
					bone->pnt.x+=move_pnt.x;
					bone->pnt.y+=move_pnt.y;
					bone->pnt.z+=move_pnt.z;

					bone++;
				}
			}

			main_wind_draw();
				
			break;
			
	}
}

