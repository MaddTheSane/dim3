/****************************** File *********************************

Module: dim3 Editor
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

#define kMeshPropertyOn							0
#define kMeshPropertyPassThrough				1
#define kMeshPropertyMovable					2
#define kMeshPropertyShiftable					3
#define kMeshPropertyHilite						4
#define kMeshPropertySimpleCollision			5
#define kMeshPropertyLockUV						6
#define kMeshPropertyLockMove					7
#define kMeshPropertyCascadeSize				8
#define kMeshPropertyNeverObscure				9
#define kMeshPropertyNeverCull					10
#define kMeshPropertyRotIndependent				11
#define kMeshPropertyNoLightMap					12
#define kMeshPropertySkipLightMapTrace			13
#define kMeshPropertyNoHaloObscure				14

#define kMeshPropertyGroup						20

#define kMeshPropertyHideMode					30
#define kMeshPropertyHarm						31
#define kMeshPropertyRot						32

#define kMeshPropertyImportOBJName				40
#define kMeshPropertyImportGroupName			41

#define kMeshPropertyMessageEnter				50
#define kMeshPropertyMessageEnterId				51
#define kMeshPropertyMessageExit				52
#define kMeshPropertyMessageExitId				53
#define kMeshPropertyMessageMapChange			54
#define kMeshPropertyMessageMapChangeName		55
#define kMeshPropertyMessageMapChangeSpotName	56

#define kMeshPropertyBaseOn						57
#define kMeshPropertyBaseTeam					58

#define kMeshPropertyPolyCount					60
#define kMeshPropertyVertexCount				61
#define kMeshPropertyBoundMin					62
#define kMeshPropertyBoundMax					63
#define kMeshPropertyBoundSize					64

#define kMeshPolyPropertyClimbable				70
#define kMeshPolyPropertyNeverCull				71
#define kMeshPolyPropertyObscuring				72

#define kMeshPolyPropertyOff					80
#define kMeshPolyPropertySize					81
#define kMeshPolyPropertyShift					82

#define kMeshPolyPropertyTangent				90
#define kMeshPolyPropertyBinormal				91
#define kMeshPolyPropertyNormal					92

#define kMeshPolyPropertyCamera					93

extern map_type					map;

extern list_palette_type		map_palette;

char							mesh_map_hide_list[][name_str_len]={"Never","Single Player","Multiplayer",""},
								mesh_map_team_list[][name_str_len]={"None","Red","Blue","Common",""};

int								pal_mesh_index,pal_poly_index;
d3pnt							pal_mesh_min,pal_mesh_max,pal_mesh_size,
								pal_mesh_prev_min,pal_mesh_prev_max,pal_mesh_prev_size;
d3vct							pal_mesh_binormal;
d3uv							pal_mesh_uv_offset,pal_mesh_uv_size,
								pal_mesh_prev_uv_offset,pal_mesh_prev_uv_size;

/* =======================================================

      Property Palette Fill Mesh
      
======================================================= */

void map_palette_fill_mesh(int mesh_idx,int poly_idx)
{
	char					str[256];
	map_mesh_type			*mesh;
	map_mesh_poly_type		*poly;
	editor_view_type		*view;
	
		// mesh settings

	mesh=&map.mesh.meshes[mesh_idx];

	if (poly_idx==-1) {
		sprintf(str,"%d",mesh_idx);
		list_palette_set_title(&map_palette,"Mesh",str,NULL,NULL,NULL,NULL);
	}
	else {
		sprintf(str,"%d.%d",mesh_idx,poly_idx);
		list_palette_set_title(&map_palette,"Mesh Poly",str,NULL,NULL,NULL,NULL);
	}

	pal_mesh_index=mesh_idx;

		// settings
		
	list_palette_add_header(&map_palette,0,"Mesh Settings");
	list_palette_add_checkbox(&map_palette,kMeshPropertyOn,"On",&mesh->flag.on,FALSE);
	list_palette_add_checkbox(&map_palette,kMeshPropertyPassThrough,"Pass Through",&mesh->flag.pass_through,FALSE);
	list_palette_add_checkbox(&map_palette,kMeshPropertyMovable,"Movable",&mesh->flag.moveable,FALSE);
	list_palette_add_checkbox(&map_palette,kMeshPropertyHilite,"High Lighted",&mesh->flag.hilite,FALSE);
	list_palette_add_checkbox(&map_palette,kMeshPropertySimpleCollision,"Simple Box Collision",&mesh->flag.simple_collision,FALSE);
	list_palette_add_checkbox(&map_palette,kMeshPropertyLockUV,"Lock UVs",&mesh->flag.lock_uv,FALSE);
	list_palette_add_checkbox(&map_palette,kMeshPropertyLockMove,"Lock Position",&mesh->flag.lock_move,FALSE);
	list_palette_add_checkbox(&map_palette,kMeshPropertyCascadeSize,"Cascade Size",&mesh->flag.cascade_size,FALSE);
	list_palette_add_checkbox(&map_palette,kMeshPropertyNeverObscure,"Never Obscure",&mesh->flag.never_obscure,FALSE);
	list_palette_add_checkbox(&map_palette,kMeshPropertyNeverCull,"Never Cull",&mesh->flag.never_cull,FALSE);
	list_palette_add_checkbox(&map_palette,kMeshPropertyRotIndependent,"Rotate Independently",&mesh->flag.rot_independent,FALSE);
	list_palette_add_checkbox(&map_palette,kMeshPropertyNoLightMap,"No Light Map",&mesh->flag.no_light_map,FALSE);
	list_palette_add_checkbox(&map_palette,kMeshPropertySkipLightMapTrace,"Skip Light Map Trace",&mesh->flag.skip_light_map_trace,FALSE);
	list_palette_add_checkbox(&map_palette,kMeshPropertyNoHaloObscure,"No Halo Obscure",&mesh->flag.no_halo_obscure,FALSE);

		// options
		
	list_palette_add_header(&map_palette,0,"Mesh Options");
	list_palette_add_picker_list_int(&map_palette,kMeshPropertyHideMode,"Hide",(char*)mesh_map_hide_list,-1,name_str_len,0,FALSE,FALSE,&mesh->hide_mode,FALSE);
	list_palette_add_int(&map_palette,kMeshPropertyHarm,"Harm",&mesh->harm,FALSE);
	list_palette_add_point(&map_palette,kMeshPropertyRot,"Rotational Center",&mesh->rot_off,FALSE);

	list_palette_add_header(&map_palette,0,"Mesh Group");
	list_palette_add_picker_list_int(&map_palette,kMeshPropertyGroup,"Group",(char*)map.group.groups,map.group.ngroup,sizeof(group_type),(int)offsetof(group_type,name),TRUE,TRUE,&mesh->group_idx,FALSE);

		// messages
		
	list_palette_add_header(&map_palette,0,"Mesh Messages");
	list_palette_add_checkbox(&map_palette,kMeshPropertyMessageEnter,"Entry On",&mesh->msg.entry_on,FALSE);
	list_palette_add_int(&map_palette,kMeshPropertyMessageEnterId,"Entry ID",&mesh->msg.entry_id,FALSE);
	list_palette_add_checkbox(&map_palette,kMeshPropertyMessageExit,"Exit On",&mesh->msg.exit_on,FALSE);
	list_palette_add_int(&map_palette,kMeshPropertyMessageExitId,"Exit ID",&mesh->msg.exit_id,FALSE);
	list_palette_add_checkbox(&map_palette,kMeshPropertyMessageMapChange,"Map Change On",&mesh->msg.map_change_on,FALSE);
	list_palette_add_string(&map_palette,kMeshPropertyMessageMapChangeName,"Map Name",mesh->msg.map_name,name_str_len,FALSE);
	list_palette_add_string(&map_palette,kMeshPropertyMessageMapChangeSpotName,"Map Spot Name",mesh->msg.map_spot_name,name_str_len,FALSE);
	
		// bases
		
	list_palette_add_header(&map_palette,0,"Mesh Base");
	list_palette_add_checkbox(&map_palette,kMeshPropertyBaseOn,"On",&mesh->msg.base_on,FALSE);
	list_palette_add_picker_list_int(&map_palette,kMeshPropertyBaseTeam,"Team",(char*)mesh_map_team_list,-1,name_str_len,0,FALSE,FALSE,&mesh->msg.base_team,FALSE);

		// import
		
	list_palette_add_header(&map_palette,0,"Mesh Import");
	list_palette_add_string(&map_palette,kMeshPropertyImportOBJName,"OBJ Name",mesh->import.obj_name,name_str_len,FALSE);
	list_palette_add_string(&map_palette,kMeshPropertyImportGroupName,"Group Name",mesh->import.group_name,name_str_len,FALSE);

		// mesh sizes

	memmove(&pal_mesh_prev_min,&pal_mesh_min,sizeof(d3pnt));		// need to remember previous as list redraw will reset before we get change
	memmove(&pal_mesh_prev_max,&pal_mesh_max,sizeof(d3pnt));
	memmove(&pal_mesh_prev_size,&pal_mesh_size,sizeof(d3pnt));

	map_mesh_calculate_extent(&map,mesh_idx,&pal_mesh_min,&pal_mesh_max);
	pal_mesh_size.x=pal_mesh_max.x-pal_mesh_min.x;
	pal_mesh_size.y=pal_mesh_max.y-pal_mesh_min.y;
	pal_mesh_size.z=pal_mesh_max.z-pal_mesh_min.z;

	list_palette_add_header(&map_palette,0,"Mesh Data");
	list_palette_add_int(&map_palette,kMeshPropertyPolyCount,"Polygons",&mesh->npoly,TRUE);
	list_palette_add_int(&map_palette,kMeshPropertyVertexCount,"Vertexes",&mesh->nvertex,TRUE);
	list_palette_add_point(&map_palette,kMeshPropertyBoundMin,"Min",&pal_mesh_min,FALSE);
	list_palette_add_point(&map_palette,kMeshPropertyBoundMax,"Max",&pal_mesh_max,FALSE);
	list_palette_add_point(&map_palette,kMeshPropertyBoundSize,"Size",&pal_mesh_size,FALSE);

		// polygon settings

	if (poly_idx!=-1) {

		view=view_get_current_view();
		poly=&mesh->polys[poly_idx];
		
		list_palette_add_header(&map_palette,0,"Poly Settings");
		list_palette_add_checkbox(&map_palette,kMeshPolyPropertyClimbable,"Cimbable",&poly->flag.climbable,FALSE);
		list_palette_add_checkbox(&map_palette,kMeshPolyPropertyNeverCull,"Never Cull",&poly->flag.never_cull,FALSE);
		list_palette_add_checkbox(&map_palette,kMeshPolyPropertyObscuring,"Obscuring",&poly->flag.obscuring,FALSE);

		memmove(&pal_mesh_prev_uv_offset,&pal_mesh_uv_offset,sizeof(d3uv));		// need to remember previous as list redraw will reset before we get change
		memmove(&pal_mesh_prev_uv_size,&pal_mesh_uv_size,sizeof(d3uv));
		map_mesh_get_poly_uv_as_box(&map,mesh_idx,poly_idx,(view->uv_layer==uv_layer_light_map),&pal_mesh_uv_offset.x,&pal_mesh_uv_offset.y,&pal_mesh_uv_size.x,&pal_mesh_uv_size.y);

		list_palette_add_header(&map_palette,0,"Poly UVs");
		list_palette_add_uv(&map_palette,kMeshPolyPropertyOff,"Offset",&pal_mesh_uv_offset,FALSE);
		list_palette_add_uv(&map_palette,kMeshPolyPropertySize,"Size",&pal_mesh_uv_size,FALSE);
		list_palette_add_uv(&map_palette,kMeshPolyPropertyShift,"Shift",&poly->shift,FALSE);
		
		list_palette_add_header(&map_palette,0,"Poly Tangent Space");
		list_palette_add_normal_vector(&map_palette,kMeshPolyPropertyTangent,"Tangent",&poly->tangent_space.tangent,FALSE);
		vector_cross_product(&pal_mesh_binormal,&poly->tangent_space.tangent,&poly->tangent_space.normal);
		list_palette_add_normal_vector(&map_palette,kMeshPolyPropertyBinormal,"Binormal",&pal_mesh_binormal,TRUE);
		list_palette_add_normal_vector(&map_palette,kMeshPolyPropertyNormal,"Normal",&poly->tangent_space.normal,FALSE);

		list_palette_add_header(&map_palette,0,"Poly Camera");
		list_palette_add_picker_list_string(&map_palette,kMeshPolyPropertyCamera,"Node",(char*)map.nodes,map.nnode,sizeof(node_type),(int)offsetof(node_type,name),TRUE,TRUE,poly->camera,FALSE);
		
		pal_poly_index=poly_idx;
	}
}

/* =======================================================

      Property Palette Click Mesh
      
======================================================= */

void map_palette_click_mesh(int mesh_idx,int poly_idx,bool double_click)
{
	map_mesh_type			*mesh;
	map_mesh_poly_type		*poly;
	editor_view_type		*view;

	if (!double_click) return;

		// mesh bound changes

	switch (map_palette.item_pane.click.id) {

		case kMeshPropertyBoundMin:
		case kMeshPropertyBoundMax:
			map_undo_push();
			map_mesh_resize(&map,mesh_idx,&pal_mesh_prev_min,&pal_mesh_prev_max);
			view_vbo_mesh_rebuild(mesh_idx);
			break;

		case kMeshPropertyBoundSize:
			map_undo_push();
			pal_mesh_prev_max.x=pal_mesh_prev_min.x+pal_mesh_prev_size.x;
			pal_mesh_prev_max.y=pal_mesh_prev_min.y+pal_mesh_prev_size.y;
			pal_mesh_prev_max.z=pal_mesh_prev_min.z+pal_mesh_prev_size.z;
			map_mesh_resize(&map,mesh_idx,&pal_mesh_prev_min,&pal_mesh_prev_max);
			view_vbo_mesh_rebuild(mesh_idx);
			break;

	}

		// polygon settings

	if (poly_idx!=-1) {

		view=view_get_current_view();
		mesh=&map.mesh.meshes[mesh_idx];
		poly=&mesh->polys[poly_idx];

		switch (map_palette.item_pane.click.id) {

			case kMeshPolyPropertyOff:
			case kMeshPolyPropertySize:
				map_mesh_set_poly_uv_as_box(&map,mesh_idx,poly_idx,(view->uv_layer==uv_layer_light_map),pal_mesh_prev_uv_offset.x,pal_mesh_prev_uv_offset.y,pal_mesh_prev_uv_size.x,pal_mesh_prev_uv_size.y);
				view_vbo_mesh_rebuild(mesh_idx);
				break;

		}
	}
}

