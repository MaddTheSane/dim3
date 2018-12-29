/****************************** File *********************************

Module: dim3 Animator
Author: Brian Barnes
 Usage: Model Add or Delete Pieces

***************************** License ********************************

This code can be freely used as long as these conditions are met:

1. This header, in its entirety, is kept with the code
2. This credit “Created with dim3 Technology” is given on a single
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

extern model_type				model;
extern app_state_type			state;
extern iface_type				iface;

extern list_palette_type		model_palette;

/* =======================================================

      Add or Delete Meshes
      
======================================================= */

void model_piece_add_mesh(void)
{
	int				idx;

	idx=model_mesh_add(&model);
	if (idx==-1) {
		os_dialog_alert("Add Mesh","You've reached the mesh limit for this model.");
		return;
	}

	state.model.cur_item=item_model_mesh;
	state.model.cur_mesh_idx=idx;

	model_palette_scroll_into_view(item_model_mesh,idx);

	dialog_property_string_run(list_string_value_string,(void*)model.meshes[idx].name,name_str_len,0,0);
}

void model_piece_duplicate_mesh(int mesh_idx)
{
	int				idx;

	idx=model_mesh_duplicate(&model,mesh_idx);
	if (idx==-1) {
		os_dialog_alert("Duplicate Mesh","You've reached the mesh limit for this model.");
		return;
	}
			
	state.model.cur_item=item_model_mesh;
	state.model.cur_mesh_idx=idx;

	model_palette_scroll_into_view(item_model_mesh,idx);

	dialog_property_string_run(list_string_value_string,(void*)model.meshes[idx].name,name_str_len,0,0);
}

void model_piece_delete_mesh(int mesh_idx)
{
	if (model.nmesh==1) {
		os_dialog_alert("Delete Mesh","Can not delete -- you need at least one mesh in the model.");
		return;
	}

	model_mesh_delete(&model,mesh_idx);

	state.model.cur_item=item_model;
	state.model.cur_mesh_idx=0;

	model_palette_scroll_into_view(item_model,0);
}

/* =======================================================

      Add or Delete Bones
      
======================================================= */

void model_piece_add_bone(void)
{
	int				x,y,z,idx;

	model_vertex_find_center_sel_vertexes(state.model.cur_mesh_idx,&x,&y,&z);
	
	idx=model_bone_add(&model,x,y,z);
	if (idx==-1) {
		os_dialog_alert("Add Bone","You've reached the bone limit for this model.");
		return;
	}
	
	model_vertex_mask_set_sel_vertex_to_bone(state.model.cur_mesh_idx,idx,-1,1.0f);

	state.model.cur_item=item_model_bone;
	state.model.cur_bone_idx=idx;
	state.model.cur_pose_idx=-1;		// reset to neutral pose so user doesn't get confused by bone movement

	model_palette_scroll_into_view(item_model_bone,idx);
	
	dialog_property_string_run(list_string_value_string,(void*)model.bones[idx].name,name_str_len,0,0);
}

void model_piece_delete_bone(int bone_idx)
{
 	model_bone_delete(&model,bone_idx);

	state.model.cur_item=item_model;

	state.model.cur_bone_idx=-1;
	state.model.cur_pose_bone_move_idx=-1;

	model_palette_scroll_into_view(item_model,0);
}

/* =======================================================

      Add or Delete Poses
      
======================================================= */

void model_piece_add_pose(void)
{
	int				idx;

	if (model.nbone==0) {
		os_dialog_alert("Add Pose","You need to have at least one bone before creating a pose.");
		return;
	}
	
	idx=model_pose_add(&model);
	if (idx==-1) {
		os_dialog_alert("Add Pose","You've reached the pose limit for this model.");
		return;
	}
	
	state.model.cur_item=item_model_pose;
	state.model.cur_pose_idx=idx;
	state.model.cur_pose_bone_move_idx=-1;

	model_palette_scroll_into_view(item_model_pose,idx);
	
	dialog_property_string_run(list_string_value_string,(void*)model.poses[idx].name,name_str_len,0,0);
}

void model_piece_duplicate_pose(int pose_idx)
{
	int				idx;

	idx=model_pose_duplicate(&model,state.model.cur_pose_idx);
	if (idx==-1) {
		os_dialog_alert("Duplicate Pose","You've reached the pose limit for this model.");
		return;
	}
	
	state.model.cur_item=item_model_pose;
	state.model.cur_pose_idx=idx;
	state.model.cur_pose_bone_move_idx=-1;

	model_palette_scroll_into_view(item_model_pose,idx);

	dialog_property_string_run(list_string_value_string,(void*)model.poses[idx].name,name_str_len,0,0);
}

void model_piece_delete_pose(int pose_idx)
{
	if (model_check_pose_in_animation(&model,pose_idx)) {
		os_dialog_alert("Delete Pose","Can not delete -- this pose is being used in an animation.");
		return;
	}
			
	model_pose_delete(&model,pose_idx);

	state.model.cur_item=item_model;

	if (model.npose==0) {
		state.model.cur_pose_idx=-1;
	}
	else {
		state.model.cur_pose_idx=0;
	}
	state.model.cur_pose_bone_move_idx=-1;

	model_palette_scroll_into_view(item_model,0);
}

int model_shift_pose_index(int pose_idx,int dir)
{

	int						n,k,
							temp_index,indexes[max_model_pose];
	char					temp_name[64],names[max_model_pose][64];
	bool					shuffle;

		// build a name list to sort
		
	for (n=0;n!=model.npose;n++) {
		strcpy(names[n],model.poses[n].name);
		indexes[n]=n;
	}
	
		// sort list

	while (TRUE) {

		shuffle=FALSE;

		for (n=0;n<(model.npose-1);n++) {
			k=n+1;

			if (strcasecmp(names[n],names[k])>0) {
				shuffle=TRUE;

				strcpy(temp_name,names[n]);
				strcpy(names[n],names[k]);
				strcpy(names[k],temp_name);
				
				temp_index=indexes[n];
				indexes[n]=indexes[k];
				indexes[k]=temp_index;
			}
		}

		if (!shuffle) break;
	}
	
		// pick previous or next

	for (n=0;n!=model.npose;n++) {
		if (pose_idx==indexes[n]) {
			pose_idx=n;
			break;
		}
	}
	
	pose_idx+=dir;
	if (pose_idx==-1) pose_idx=model.npose-1;
	if (pose_idx==model.npose) pose_idx=0;
	
	return(indexes[pose_idx]);
}

/* =======================================================

      Add or Delete Animations
      
======================================================= */

void model_piece_add_animate(void)
{
	int				idx;

	if (model.npose==0) {
		os_dialog_alert("Add Animation","You need to have at least one pose before creating an animation.");
		return;
	}
	
	model_play(model_play_mode_stop);
	
	idx=model_animate_add(&model);
	if (idx==-1) {
		os_dialog_alert("Add Animation","You've reached the animation limit for this model.");
		return;
	}
	
	state.model.cur_item=item_model_animate;
	state.model.cur_animate_idx=idx;
	state.model.cur_animate_pose_move_idx=-1;
	state.model.cur_animate_pose_move_particle_idx=-1;
	state.model.cur_animate_pose_move_ring_idx=-1;

	model_palette_scroll_into_view(item_model_animate,idx);
	
	dialog_property_string_run(list_string_value_string,(void*)model.animates[idx].name,name_str_len,0,0);
}

void model_piece_duplicate_animate(int animate_idx)
{
	int				idx;

	model_play(model_play_mode_stop);

	idx=model_animate_duplicate(&model,state.model.cur_animate_idx);
	if (idx==-1) {
		os_dialog_alert("Duplicate Animation","You've reached the animation limit for this model.");
		return;
	}
	
	state.model.cur_item=item_model_animate;
	state.model.cur_animate_idx=idx;
	state.model.cur_animate_pose_move_idx=-1;
	state.model.cur_animate_pose_move_particle_idx=-1;
	state.model.cur_animate_pose_move_ring_idx=-1;

	model_palette_scroll_into_view(item_model_animate,idx);

	dialog_property_string_run(list_string_value_string,(void*)model.animates[idx].name,name_str_len,0,0);
}

void model_piece_delete_animate(int animate_idx)
{
	model_play(model_play_mode_stop);

	model_animate_delete(&model,animate_idx);

	state.model.cur_item=item_model;

	if (model.nanimate==0) {
		state.model.cur_animate_idx=-1;
	}
	else {
		state.model.cur_animate_idx=0;
	}

	state.model.cur_animate_pose_move_idx=-1;
	state.model.cur_animate_pose_move_particle_idx=-1;
	state.model.cur_animate_pose_move_ring_idx=-1;

	model_palette_scroll_into_view(item_model,0);
}

/* =======================================================

      Add or Delete Animation Pose Move Pieces
      
======================================================= */

void model_piece_add_animation_pose_move_particle(int animate_idx,int animate_pose_move_idx)
{
	int						idx;
	model_animate_type		*animate;
	model_pose_move_type	*pose_move;
	model_particle_type		*particle;

	animate=&model.animates[animate_idx];
	pose_move=&animate->pose_moves[animate_pose_move_idx];

	if (pose_move->particle.count>=max_model_animate_particle) {
		os_dialog_alert("Add Particle Pose Move","You've reached the particle limit for this animation pose move.");
		return;
	}

	idx=pose_move->particle.count;
	particle=&pose_move->particle.particles[idx];
	pose_move->particle.count++;

	particle->name[0]=0x0;
	particle->bone_idx=-1;
	particle->motion_factor=1.0f;
	particle->motion=particle->rotate=particle->stick=FALSE;
	particle->slop.x=particle->slop.y=particle->slop.z=0;

	list_palette_start_picking_mode(&model_palette,"Pick a Particle",(char*)iface.particle_list.particles,iface.particle_list.nparticle,sizeof(iface_particle_type),(int)offsetof(iface_particle_type,name),FALSE,FALSE,TRUE,NULL,particle->name);
}

void model_piece_delete_animation_pose_move_particle(int animate_idx,int animate_pose_move_idx,int idx)
{
	int						sz;
	model_animate_type		*animate;
	model_pose_move_type	*pose_move;

	animate=&model.animates[animate_idx];
	pose_move=&animate->pose_moves[animate_pose_move_idx];

	sz=sizeof(model_particle_type)*(pose_move->particle.count-(idx+1));
	if (sz>0) memmove(&pose_move->particle.particles[idx],&pose_move->particle.particles[idx+1],sz);

	pose_move->particle.count--;
}

void model_piece_add_animation_pose_move_ring(int animate_idx,int animate_pose_move_idx)
{
	int						idx;
	model_animate_type		*animate;
	model_pose_move_type	*pose_move;
	model_ring_type			*ring;

	animate=&model.animates[animate_idx];
	pose_move=&animate->pose_moves[animate_pose_move_idx];

	if (pose_move->ring.count>=max_model_animate_ring) {
		os_dialog_alert("Add Ring Pose Move","You've reached the ring limit for this animation pose move.");
		return;
	}

	idx=pose_move->ring.count;
	ring=&pose_move->ring.rings[idx];
	pose_move->ring.count++;

	ring->name[0]=0x0;
	ring->bone_idx=-1;
	ring->angle=FALSE;
	ring->slop.x=ring->slop.y=ring->slop.z=0;

	list_palette_start_picking_mode(&model_palette,"Pick a Ring",(char*)iface.ring_list.rings,iface.ring_list.nring,sizeof(iface_ring_type),(int)offsetof(iface_ring_type,name),FALSE,FALSE,TRUE,NULL,ring->name);
}

void model_piece_delete_animation_pose_move_ring(int animate_idx,int animate_pose_move_idx,int idx)
{
	int						sz;
	model_animate_type		*animate;
	model_pose_move_type	*pose_move;

	animate=&model.animates[animate_idx];
	pose_move=&animate->pose_moves[animate_pose_move_idx];

	sz=sizeof(model_ring_type)*(pose_move->ring.count-(idx+1));
	if (sz>0) memmove(&pose_move->ring.rings[idx],&pose_move->ring.rings[idx+1],sz);

	pose_move->ring.count--;
}

/* =======================================================

      Shift Pose Moves
      
======================================================= */

int model_shift_animation_pose_move(int animate_idx,int pose_move_idx,int dir)
{
	int						idx;
	model_animate_type		*animate;
	model_pose_move_type	temp_pose_move;
	
	animate=&model.animates[animate_idx];

		// can't move if at top or bottom
		
	if ((dir==-1) && (pose_move_idx==0)) return(pose_move_idx);
	if ((dir==1) && (pose_move_idx==(animate->npose_move-1))) return(pose_move_idx);
	
		// move pose
		
	idx=pose_move_idx+dir;
	memmove(&temp_pose_move,&animate->pose_moves[pose_move_idx],sizeof(model_pose_move_type));
	memmove(&animate->pose_moves[pose_move_idx],&animate->pose_moves[idx],sizeof(model_pose_move_type));
	memmove(&animate->pose_moves[idx],&temp_pose_move,sizeof(model_pose_move_type));
	
	return(idx);
}

/* =======================================================

      Add or Delete Hit Boxes
      
======================================================= */

void model_piece_add_hit_box(void)
{
	int				idx;

	idx=model_hit_box_add(&model);
	if (idx==-1) {
		os_dialog_alert("Add Hit Box","You've reached the hit box limit for this model.");
		return;
	}

	state.model.hit_box=TRUE;
	state.model.cur_item=item_model_hit_box;
	state.model.cur_hit_box_idx=idx;

	model_palette_scroll_into_view(item_model_hit_box,idx);

	dialog_property_string_run(list_string_value_string,(void*)model.hit_boxes[idx].name,name_str_len,0,0);
}

void model_piece_delete_hit_box(int hit_box_idx)
{
	model_hit_box_delete(&model,hit_box_idx);

	state.model.cur_item=item_model;
	state.model.cur_hit_box_idx=-1;

	model_palette_scroll_into_view(item_model,0);
}
