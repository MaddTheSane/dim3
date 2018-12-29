/****************************** File *********************************

Module: dim3 Model Utility
Author: Brian Barnes
        Rocco Bowling for Original Comulative Deform Mode
 Usage: Bone Routines

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

(c) 2000-2010 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3modelutility.h"
#endif

/* =======================================================

      Create Rotations/Moves from Neutral
      
======================================================= */

void model_create_draw_bone_from_neutral(model_type *model,model_draw_setup *draw_setup,model_draw_bone_type *draw_bones)
{
	int							n,nbone;
	model_bone_type				*bone;
	model_draw_bone_type		*draw_bone;
	model_draw_alter_bone_type	*alter_bone;
	
	bone=model->bones;
	draw_bone=draw_bones;
	alter_bone=draw_setup->alter_bones;
	
	nbone=model->nbone;
	
	for (n=0;n!=nbone;n++) {
		draw_bone->parent_idx=bone->parent_idx;
		
		draw_bone->rot.x=angle_add(alter_bone->rot_add.x,bone->natural_rot.x);
		draw_bone->rot.z=angle_add(alter_bone->rot_add.y,bone->natural_rot.y);
		draw_bone->rot.y=angle_add(alter_bone->rot_add.z,bone->natural_rot.z);
		
		draw_bone->parent_dist.x=bone->parent_dist.x+(alter_bone->parent_dist_add.x+bone->natural_offset.x);
		draw_bone->parent_dist.y=bone->parent_dist.y+(alter_bone->parent_dist_add.y+bone->natural_offset.y);
		draw_bone->parent_dist.z=bone->parent_dist.z+(alter_bone->parent_dist_add.z+bone->natural_offset.z);

		bone++;
		draw_bone++;
		alter_bone++;
	}
}

/* =======================================================

      Create Draw Bones
      
======================================================= */

void model_create_draw_bone_from_pose(model_type *model,model_draw_setup *draw_setup,model_draw_bone_type *draw_bones,int pose_1)
{
	int							n,nbone;
	float						ang;
	model_bone_type				*bone;
	model_draw_bone_type		*draw_bone;
	model_bone_move_type		*bone_move;
	model_draw_alter_bone_type	*alter_bone;
	
		// build draw list from model bones
	
	bone=model->bones;
	bone_move=model->poses[pose_1].bone_moves;
	draw_bone=draw_bones;
	alter_bone=draw_setup->alter_bones;
	
	nbone=model->nbone;
	
	for (n=0;n!=nbone;n++) {
		draw_bone->parent_idx=bone->parent_idx;
		
		ang=angle_add((bone_move->rot.x*draw_setup->enhance_factor),alter_bone->rot_add.x);
		draw_bone->rot.x=angle_add(ang,bone->natural_rot.x);

		ang=angle_add((bone_move->rot.y*draw_setup->enhance_factor),alter_bone->rot_add.y);
        draw_bone->rot.y=angle_add(ang,bone->natural_rot.y);

		ang=angle_add((bone_move->rot.z*draw_setup->enhance_factor),alter_bone->rot_add.z);
        draw_bone->rot.z=angle_add(ang,bone->natural_rot.z);

		draw_bone->parent_dist.x=(bone->parent_dist.x*bone_move->mov.x)+(alter_bone->parent_dist_add.x+bone->natural_offset.x);
		draw_bone->parent_dist.y=(bone->parent_dist.y*bone_move->mov.y)+(alter_bone->parent_dist_add.y+bone->natural_offset.y);
		draw_bone->parent_dist.z=(bone->parent_dist.z*bone_move->mov.z)+(alter_bone->parent_dist_add.z+bone->natural_offset.z);
		
		bone++;
		draw_bone++;
		alter_bone++;
		bone_move++;
	}
}

void model_create_draw_bone_from_pose_factor(model_type *model,model_draw_setup *draw_setup,model_draw_bone_type *draw_bones,int pose_1,int pose_2,float pose_factor)
{
	int							n,nbone;
	float						accel_pose_factor,
								mov_x_start,mov_z_start,mov_y_start,
								mov_x_end,mov_z_end,mov_y_end,
								rot_x_start,rot_z_start,rot_y_start,
								rot_x_end,rot_z_end,rot_y_end;
	model_bone_type				*bone;
	model_draw_bone_type		*draw_bone;
	model_bone_move_type		*bone_move_start,*bone_move_end;
	model_draw_alter_bone_type	*alter_bone;
	
		// find the bone moves to interept from
		
	bone_move_start=model->poses[pose_1].bone_moves;
	bone_move_end=model->poses[pose_2].bone_moves;
	
		// build draw list from model bones
	
	bone=model->bones;
	draw_bone=draw_bones;
	alter_bone=draw_setup->alter_bones;
	
	nbone=model->nbone;
	
	for (n=0;n!=nbone;n++) {
		draw_bone->parent_idx=bone->parent_idx;
		
			// find the bone moves
		
		rot_x_start=bone_move_start->rot.x;
		rot_y_start=bone_move_start->rot.y;
		rot_z_start=bone_move_start->rot.z;

		mov_x_start=bone_move_start->mov.x;
		mov_y_start=bone_move_start->mov.y;
		mov_z_start=bone_move_start->mov.z;
		
		rot_x_end=bone_move_end->rot.x;
		rot_y_end=bone_move_end->rot.y;
		rot_z_end=bone_move_end->rot.z;
			
		mov_x_end=bone_move_end->mov.x;
		mov_y_end=bone_move_end->mov.y;
		mov_z_end=bone_move_end->mov.z;
		
			// change the factor based on acceleration

		accel_pose_factor=acceleration_calculate(pose_factor,bone_move_start->acceleration);
	
			// factor the bone moves
			
		draw_bone->rot.x=(rot_x_end+((rot_x_start-rot_x_end)*accel_pose_factor))*draw_setup->enhance_factor;
		draw_bone->rot.x=angle_add(draw_bone->rot.x,alter_bone->rot_add.x);
		draw_bone->rot.x=angle_add(draw_bone->rot.x,bone->natural_rot.x);

		draw_bone->rot.y=(rot_y_end+((rot_y_start-rot_y_end)*accel_pose_factor))*draw_setup->enhance_factor;
		draw_bone->rot.y=angle_add(draw_bone->rot.y,alter_bone->rot_add.y);
		draw_bone->rot.y=angle_add(draw_bone->rot.y,bone->natural_rot.y);

		draw_bone->rot.z=(rot_z_end+((rot_z_start-rot_z_end)*accel_pose_factor))*draw_setup->enhance_factor;
		draw_bone->rot.z=angle_add(draw_bone->rot.z,alter_bone->rot_add.z);
		draw_bone->rot.z=angle_add(draw_bone->rot.z,bone->natural_rot.z);
		
		mov_x_start=mov_x_end+((mov_x_start-mov_x_end)*accel_pose_factor);
		draw_bone->parent_dist.x=(bone->parent_dist.x*mov_x_start)+(alter_bone->parent_dist_add.x+bone->natural_offset.x);

		mov_y_start=mov_y_end+((mov_y_start-mov_y_end)*accel_pose_factor);
		draw_bone->parent_dist.y=(bone->parent_dist.y*mov_y_start)+(alter_bone->parent_dist_add.y+bone->natural_offset.y);

		mov_z_start=mov_z_end+((mov_z_start-mov_z_end)*accel_pose_factor);
		draw_bone->parent_dist.z=(bone->parent_dist.z*mov_z_start)+(alter_bone->parent_dist_add.z+bone->natural_offset.z);

		bone++;
		draw_bone++;
		alter_bone++;
		bone_move_start++;
		bone_move_end++;
	}
}

/* =======================================================

      Combine Model Rotations Through Parents (Non-Comulative)
      
======================================================= */

void model_combine_draw_bone_add_rotate(model_draw_bone_type *draw_bones,int parent_idx,float *rot_x,float *rot_y,float *rot_z)
{
	model_draw_bone_type		*draw_bone;

	if (parent_idx==-1) return;
	
	draw_bone=&draw_bones[parent_idx];
	
	*rot_x+=draw_bone->rot.x;
	*rot_y+=draw_bone->rot.y;
	*rot_z+=draw_bone->rot.z;
	
	model_combine_draw_bone_add_rotate(draw_bones,draw_bone->parent_idx,rot_x,rot_y,rot_z);
}

void model_combine_draw_bone_rotations(model_type *model,model_draw_bone_type *draw_bones)
{
	int						n,nbone;
	float					rot_x,rot_y,rot_z;
	model_draw_bone_type	*draw_bone;
	
	draw_bone=draw_bones;

	nbone=model->nbone;
	
	for (n=0;n!=nbone;n++) {
		rot_x=draw_bone->rot.x;
		rot_y=draw_bone->rot.y;
		rot_z=draw_bone->rot.z;

		model_combine_draw_bone_add_rotate(draw_bones,draw_bone->parent_idx,&rot_x,&rot_y,&rot_z);
		matrix_rotate_xyz(&draw_bone->rot_mat,rot_x,rot_y,rot_z);

		draw_bone++;
	}
}

/* =======================================================

      Combine Model Rotations Through Parents (Comulative)
      
======================================================= */

void model_comulative_combine_draw_bone_combine_rotate_by_matrix(model_draw_bone_type *draw_bones,int idx)
{
	model_draw_bone_type		*draw_bone,*parent_draw_bone;
	matrix_type					local_matrix;
	
	draw_bone=&draw_bones[idx];
	
	if (draw_bone->parent_idx!=-1) {

		parent_draw_bone=&draw_bones[draw_bone->parent_idx];
		if (!parent_draw_bone->touch) model_comulative_combine_draw_bone_combine_rotate_by_matrix(draw_bones,draw_bone->parent_idx);
		
		memcpy(&draw_bone->rot_mat,&parent_draw_bone->rot_mat,sizeof(matrix_type));
		
		draw_bone->rot_mat.data[0][3]=0.0f;
		draw_bone->rot_mat.data[1][3]=0.0f;
		draw_bone->rot_mat.data[2][3]=0.0f;
	}
	
	matrix_rotate_z(&local_matrix,draw_bone->rot.z);
	matrix_multiply(&draw_bone->rot_mat,&local_matrix);
	
	matrix_rotate_y(&local_matrix,draw_bone->rot.y);
	matrix_multiply(&draw_bone->rot_mat,&local_matrix);
	
	matrix_rotate_x(&local_matrix,draw_bone->rot.x);
	matrix_multiply(&draw_bone->rot_mat,&local_matrix);
	
	draw_bone->touch=TRUE;
}

void model_comulative_combine_draw_bone_rotations(model_type *model,model_draw_bone_type *draw_bones)
{
	int						n,nbone;
	model_draw_bone_type	*draw_bone;
	
	nbone=model->nbone;
	draw_bone=draw_bones;
	
	for (n=0;n!=nbone;n++) {
		draw_bone->touch=FALSE;
		matrix_identity(&draw_bone->rot_mat);
		draw_bone++;
	}
	
	draw_bone=draw_bones;

	for (n=0;n!=nbone;n++) {
		if (!draw_bone->touch) model_comulative_combine_draw_bone_combine_rotate_by_matrix(draw_bones,n);
		draw_bone++;
	}
}

/* =======================================================

      Move The Draw Bones to their Matrixes
      
======================================================= */

void model_move_single_draw_bone(model_type *model,model_draw_bone_type *draw_bones,int bone_idx)
{
	int						parent_idx;
	float					fx,fz,fy;
	model_bone_type			*bone;
	model_draw_bone_type	*draw_bone,*parent_draw_bone;
	
		// already moved?
		
	draw_bone=&draw_bones[bone_idx];
	if (draw_bone->touch) return;
	
	draw_bone->touch=TRUE;

		// if no parent, just no movement

	parent_idx=draw_bone->parent_idx;

	if (parent_idx==-1) {
		
		bone=&model->bones[bone_idx];
		draw_bone->fpnt.x=(float)bone->pnt.x;
		draw_bone->fpnt.y=(float)bone->pnt.y;
		draw_bone->fpnt.z=(float)bone->pnt.z;

		return;
	}

		// move parents first
	
	parent_draw_bone=&draw_bones[parent_idx];
	model_move_single_draw_bone(model,draw_bones,parent_idx);

		// rotate the bone
	
	fx=draw_bone->parent_dist.x;
	fy=draw_bone->parent_dist.y;
	fz=draw_bone->parent_dist.z;
	
	matrix_vertex_multiply(&parent_draw_bone->rot_mat,&fx,&fy,&fz);
	
	draw_bone->fpnt.x=parent_draw_bone->fpnt.x+fx;
	draw_bone->fpnt.y=parent_draw_bone->fpnt.y+fy;
	draw_bone->fpnt.z=parent_draw_bone->fpnt.z+fz;
}

void model_move_draw_bones(model_type *model,model_draw_bone_type *draw_bones)
{
	int						n,nbone;
	model_draw_bone_type	*draw_bone;
	
		// no bones moved

	nbone=model->nbone;
	draw_bone=draw_bones;
	
	for (n=0;n!=nbone;n++) {
		draw_bone->touch=FALSE;
		draw_bone++;
	}

		// move bones

	for (n=0;n!=nbone;n++) {
		model_move_single_draw_bone(model,draw_bones,n);
	}
}

/* =======================================================

      Constrained Drawing Bones
      
======================================================= */

void model_fix_constrained_bones(model_type *model,int pose_idx,model_draw_bone_type *draw_bones)
{
	int						n,nbone,bone_idx;
	model_pose_type			*pose;
	model_draw_bone_type	*draw_bone,*constrained_draw_bone;

		// get pose
		
	pose=&model->poses[pose_idx];
	
		// fix any constrained bones
		
	nbone=model->nbone;
	draw_bone=draw_bones;
	
	for (n=0;n!=nbone;n++) {
	
		bone_idx=pose->bone_moves[n].constraint.bone_idx;
		if (bone_idx!=-1) {
		
			constrained_draw_bone=&draw_bones[bone_idx];
			draw_bone->fpnt.x=constrained_draw_bone->fpnt.x+pose->bone_moves[n].constraint.offset.x;
			draw_bone->fpnt.y=constrained_draw_bone->fpnt.y+pose->bone_moves[n].constraint.offset.y;
			draw_bone->fpnt.z=constrained_draw_bone->fpnt.z+pose->bone_moves[n].constraint.offset.z;
		}
		
		draw_bone++;
	}
}

/* =======================================================

      Create Drawing Bones
      
======================================================= */

void model_create_draw_bones_single(model_type *model,model_draw_setup *draw_setup,int blend_idx,model_draw_bone_type *bones)
{
	int			pose_1,pose_2;
	float		pose_factor;
	
		// get poses

	pose_1=draw_setup->poses[blend_idx].idx_1;
	pose_2=draw_setup->poses[blend_idx].idx_2;
	pose_factor=draw_setup->poses[blend_idx].factor;

		// no poses get simple neutral bones

	if (pose_1==-1) {
		model_create_draw_bone_from_neutral(model,draw_setup,bones);
	}

		// get single pose or tween pose bones

	else {
		if (pose_2==-1) {
			model_create_draw_bone_from_pose(model,draw_setup,bones,pose_1);
		}
		else {
			pose_factor=acceleration_calculate(pose_factor,draw_setup->poses[blend_idx].acceleration);
			model_create_draw_bone_from_pose_factor(model,draw_setup,bones,pose_1,pose_2,pose_factor);
		}
	}

		// combine rotations and create matrixes
		// based on deform mode

	if (!model->comulative_rotation) {
		model_combine_draw_bone_rotations(model,bones);
		model_move_draw_bones(model,bones);
	}
	else {
		model_comulative_combine_draw_bone_rotations(model,bones);
		model_move_draw_bones(model,bones);
	}
	
		// fix any constrained bones
		
	if (pose_1!=-1) model_fix_constrained_bones(model,pose_1,bones);
	if (pose_2!=-1) model_fix_constrained_bones(model,pose_2,bones);
}

void model_create_draw_bones(model_type *model,model_draw_setup *draw_setup)
{
	int						n,i,nbone,cnt,
							idx,avg_mat_cnt[max_model_bone];
	float					f_cnt;
	d3vct					fpnt[max_model_bone];
	matrix_type				avg_mats[max_model_bone][max_model_blend_animation];
	model_bone_move_type	*bone_moves;
	model_draw_bone_type	draw_bones[max_model_blend_animation][max_model_bone];

		// count number of animations

	cnt=0;

	for (n=0;n!=max_model_blend_animation;n++) {
		if (draw_setup->poses[n].idx_1!=-1) cnt++;
	}

		// if only one animation, just pass through
		// to setup bone set

	if (cnt<=1) {
		model_create_draw_bones_single(model,draw_setup,0,draw_setup->bones);
		return;
	}

		// run all animations and gather bone sets

	for (n=0;n!=max_model_blend_animation;n++) {
		if (draw_setup->poses[n].idx_1!=-1) {
			model_create_draw_bones_single(model,draw_setup,n,draw_bones[n]);
		}
	}

		// use settings from first blend as default

	nbone=model->nbone;

	memmove(&draw_setup->bones,draw_bones[0],(sizeof(model_draw_bone_type)*nbone));

		// clear out position and matrix count
		// to get ready to average out the positions

	for (n=0;n!=nbone;n++) {
		fpnt[n].x=0.0f;
		fpnt[n].y=0.0f;
		fpnt[n].z=0.0f;
		avg_mat_cnt[n]=0;
	}

		// add up all the positions and matrixes
		// note that we skip matrixes that have no
		// rotational data in them (which is why we
		// need a separate count for each bone so
		// we can average them correctly)
		
	for (n=0;n!=max_model_blend_animation;n++) {

		if (draw_setup->poses[n].idx_1!=-1) {
			
			bone_moves=model->poses[draw_setup->poses[n].idx_1].bone_moves;

				// average position and matrixes

			for (i=0;i!=nbone;i++) {

				if ((matrix_has_rotation(&draw_bones[n][i].rot_mat)) && (!bone_moves[i].skip_blended)) {

					idx=avg_mat_cnt[i];
					avg_mat_cnt[i]++;

					fpnt[i].x+=draw_bones[n][i].fpnt.x;
					fpnt[i].y+=draw_bones[n][i].fpnt.y;
					fpnt[i].z+=draw_bones[n][i].fpnt.z;

					memmove(&avg_mats[i][idx],&draw_bones[n][i].rot_mat,sizeof(matrix_type));

				}
			}
		}
	}

		// now finish out the blended average

	for (n=0;n!=nbone;n++) {
	
			// if no moves for this bone in the blended animation, then pick default bone
			// position from the model and the rotation from the first animation
		
		if (avg_mat_cnt[n]==0) {

			draw_setup->bones[n].fpnt.x=draw_bones[0][n].fpnt.x;
			draw_setup->bones[n].fpnt.y=draw_bones[0][n].fpnt.y;
			draw_setup->bones[n].fpnt.z=draw_bones[0][n].fpnt.z;

			memmove(&draw_setup->bones[n].rot_mat,&draw_bones[0][n].rot_mat,sizeof(matrix_type));

			continue;
		}
		
			// only single bone move
			
		if (avg_mat_cnt[n]==1) {
			draw_setup->bones[n].fpnt.x=fpnt[n].x;
			draw_setup->bones[n].fpnt.y=fpnt[n].y;
			draw_setup->bones[n].fpnt.z=fpnt[n].z;

			memmove(&draw_setup->bones[n].rot_mat,&avg_mats[n][0],sizeof(matrix_type));
			
			continue;
		}
		
			// else calculate the average of all the bone moves
			
		f_cnt=(float)avg_mat_cnt[n];

		draw_setup->bones[n].fpnt.x=fpnt[n].x/f_cnt;
		draw_setup->bones[n].fpnt.y=fpnt[n].y/f_cnt;
		draw_setup->bones[n].fpnt.z=fpnt[n].z/f_cnt;

		matrix_average(&draw_setup->bones[n].rot_mat,avg_mat_cnt[n],(matrix_type*)&avg_mats[n]);
	}
}
