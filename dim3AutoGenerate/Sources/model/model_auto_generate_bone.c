/****************************** File *********************************

Module: dim3 Auto Generator
Author: Brian Barnes
 Usage: Model Auto Generation Bones

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

(c) 2000-2014 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3autogenerate.h"
#endif

#define ag_model_cube_head_x_size			180
#define ag_model_cube_head_y_size			200
#define ag_model_cube_head_z_size			150

/* =======================================================

      Bone Stats
      
======================================================= */

bool ag_model_bone_is_special(model_type *model,int bone_idx)
{
	int					n;
	model_bone_type		*bone;

	bone=&model->bones[bone_idx];

		// base bone

	if (strcmp(bone->name,"Base")==0) return(TRUE);

		// connector bones

	if (bone_idx==model->bone_connect.name_bone_idx) return(TRUE);

	for (n=0;n!=max_model_light;n++) {
		if (bone_idx==model->bone_connect.light_bone_idx[n]) return(TRUE);
	}

	for (n=0;n!=max_model_halo;n++) {
		if (bone_idx==model->bone_connect.halo_bone_idx[n]) return(TRUE);
	}

	return(FALSE);
}

bool ag_model_bone_is_body(model_type *model,int bone_idx)
{
	model_bone_type		*bone,*parent_bone;

	bone=&model->bones[bone_idx];
	if (bone->parent_idx==-1) return(FALSE);

	parent_bone=&model->bones[bone->parent_idx];

	if ((strcmp(bone->name,"Torso")==0) && (strcmp(parent_bone->name,"Hips")==0)) return(TRUE);
	return((strcmp(bone->name,"Hips")==0) && (strcmp(parent_bone->name,"Torse")==0));
}

bool ag_model_bone_is_hip(model_type *model,int bone_idx)
{
	model_bone_type		*bone;

	bone=&model->bones[bone_idx];
	return(strcmp(bone->name,"Hips")==0);
}

bool ag_model_bone_is_connected_to_hips(model_type *model,int bone_idx)
{
	model_bone_type		*bone,*parent_bone;

	bone=&model->bones[bone_idx];
	if (bone->parent_idx==-1) return(FALSE);

	parent_bone=&model->bones[bone->parent_idx];
	return(strcmp(parent_bone->name,"Hips")==0);
}

bool ag_model_bone_is_shallow_y(model_type *model,int bone_idx)
{
	model_bone_type		*bone;

	bone=&model->bones[bone_idx];

	if (bone->parent_idx==-1) return(FALSE);
	return(abs(bone->pnt.y-(model->bones[bone->parent_idx].pnt.y))<20);
}

bool ag_model_bone_is_head(model_type *model,int bone_idx)
{
	return(strcmp(model->bones[bone_idx].name,"Head")==0);
}

bool ag_model_bone_is_arm(model_type *model,int bone_idx)
{
	if (strstr(model->bones[bone_idx].name,"Left Wrist")!=NULL) return(TRUE);
	if (strstr(model->bones[bone_idx].name,"Left Elbow")!=NULL) return(TRUE);
	if (strstr(model->bones[bone_idx].name,"Right Wrist")!=NULL) return(TRUE);
	if (strstr(model->bones[bone_idx].name,"Right Elbow")!=NULL) return(TRUE);

	return(FALSE);
}

bool ag_model_bone_is_hand(model_type *model,int bone_idx)
{
	return(strstr(model->bones[bone_idx].name,"Hand")!=NULL);
}

bool ag_model_bone_is_leg(model_type *model,int bone_idx)
{
	if (strstr(model->bones[bone_idx].name,"Left Ankle")!=NULL) return(TRUE);
	if (strstr(model->bones[bone_idx].name,"Left Knee")!=NULL) return(TRUE);
	if (strstr(model->bones[bone_idx].name,"Right Ankle")!=NULL) return(TRUE);
	if (strstr(model->bones[bone_idx].name,"Right Knee")!=NULL) return(TRUE);

	return(FALSE);
}

bool ag_model_bone_is_foot(model_type *model,int bone_idx)
{
	return(strstr(model->bones[bone_idx].name,"Foot")!=NULL);
}

bool ag_model_bone_is_decoration_ok(model_type *model,int bone_idx)
{
	if (strcmp(model->bones[bone_idx].name,"Head")==0) return(FALSE);
	if (strcmp(model->bones[bone_idx].name,"Base")==0) return(FALSE);
	if (strcmp(model->bones[bone_idx].name,"Name")==0) return(FALSE);
	if (strstr(model->bones[bone_idx].name,"Hand")!=NULL) return(FALSE);
	if (strstr(model->bones[bone_idx].name,"Foot")!=NULL) return(FALSE);

	return(TRUE);
}

/* =======================================================

      Add Bones
      
======================================================= */

int ag_model_add_bone_single(model_type *model,char *name,int parent_idx,int x,int y,int z)
{
	int				bone_idx;
	model_bone_type	*bone,*ptr;

	/*
	// supergumba -- normally adding a bone deletes
	// any pose moves, so for now we need to do this by hand,
	// eventually add back the real way

	bone_idx=model_bone_add(model,x,y,z);
	*/

	ptr=(model_bone_type*)malloc(sizeof(model_bone_type)*(model->nbone+1));
	if (model->bones!=NULL) {
		memmove(ptr,model->bones,(sizeof(model_bone_type)*model->nbone));
		free(model->bones);
	}
	model->bones=ptr;

	bone_idx=model->nbone;
	model->nbone++;

	bone=&model->bones[bone_idx];
	bzero(bone,sizeof(model_bone_type));
	
	bone->parent_idx=-1;
	bone->pnt.x=x;
	bone->pnt.y=y;
	bone->pnt.z=z;

	bone->natural_offset.x=bone->natural_offset.y=bone->natural_offset.z=0;
	bone->natural_rot.x=bone->natural_rot.y=bone->natural_rot.z=0.0f;
// end of stuff to delete later


	bone=&model->bones[bone_idx];
	strcpy(bone->name,name);
	bone->parent_idx=parent_idx;

	return(bone_idx);
}

void ag_model_get_random_rotate(d3ang *rot_ang,int rot_x,int rot_y,int rot_z)
{
	rot_ang->x=rot_ang->y=rot_ang->z=0.0f;

	if (rot_x!=0) rot_ang->x=(float)ag_random_int(rot_x);
	if (rot_y!=0) rot_ang->y=(float)ag_random_int(rot_y);
	if (rot_z!=0) rot_ang->z=(float)ag_random_int(rot_z);
}

void ag_model_rotate_bone(model_type *model,int bone_idx,d3ang *rot_ang,d3pnt *offset_pnt)
{
	model_bone_type	*bone,*parent_bone;

	bone=&model->bones[bone_idx];
	parent_bone=&model->bones[bone->parent_idx];

	if (offset_pnt!=NULL) memmove(offset_pnt,&bone->pnt,sizeof(d3pnt));
	rotate_point(&bone->pnt.x,&bone->pnt.y,&bone->pnt.z,parent_bone->pnt.x,parent_bone->pnt.y,parent_bone->pnt.z,rot_ang->x,rot_ang->y,rot_ang->z);

	if (offset_pnt!=NULL) {
		offset_pnt->x-=bone->pnt.x;
		offset_pnt->y-=bone->pnt.y;
		offset_pnt->z-=bone->pnt.z;
	}
}

void ag_model_move_bone(model_type *model,int bone_idx,d3pnt *offset_pnt)
{
	model_bone_type	*bone;

	bone=&model->bones[bone_idx];
	bone->pnt.x-=offset_pnt->x;
	bone->pnt.y-=offset_pnt->y;
	bone->pnt.z-=offset_pnt->z;
}

/* =======================================================

     Create Skeleton
      
======================================================= */

void ag_model_bone_create_skeleton(model_type *model)
{
	int			base_bone_idx,hip_bone_idx,
				torso_bone_idx,head_bone_idx,
				left_shoulder_bone_idx,right_shoulder_bone_idx,
				left_elbow_bone_idx,right_elbow_bone_idx,
				left_wrist_bone_idx,right_wrist_bone_idx,
				left_hip_bone_idx,right_hip_bone_idx,
				left_knee_bone_idx,right_knee_bone_idx,
				left_ankle_bone_idx,right_ankle_bone_idx,
				left_foot_bone_idx,right_foot_bone_idx;
	int			hip_high,torso_high,torso_radius,arm_swing,
				head_high,hand_high,elbow_high,ankle_high;
	d3pnt		torso_offset_pnt,left_offset_pnt,right_offset_pnt;
	d3ang		rot_ang;

		// random sizes

	hip_high=1000+ag_random_int(1500);
	torso_high=hip_high+(500+ag_random_int(500));
	torso_radius=300+ag_random_int(350);
	arm_swing=40+ag_random_int(40);
	head_high=torso_high+(50+ag_random_int(100));
	hand_high=800+ag_random_int(1000);
	elbow_high=((torso_high-hand_high)>>1)+hand_high;
	ankle_high=100+ag_random_int(200);

		// fix some extreme sizes

	if (abs(elbow_high-torso_high)<300) {
		elbow_high-=300;
		hand_high-=300;
	}
	if (abs(hand_high-elbow_high)<150) {
		hand_high-=250;
	}

	if (hand_high<0) hand_high=0;

		// supergumba -- for now need to create in same
		// order as animations that exist

	hip_bone_idx=ag_model_add_bone_single(model,"Hips",-1,0,-hip_high,0);		// supergumba -- have to reset this later
	torso_bone_idx=ag_model_add_bone_single(model,"Torso",hip_bone_idx,0,-torso_high,0);

	left_shoulder_bone_idx=ag_model_add_bone_single(model,"Left Shoulder",torso_bone_idx,torso_radius,-torso_high,60);
	right_shoulder_bone_idx=ag_model_add_bone_single(model,"Right Shoulder",torso_bone_idx,-torso_radius,-torso_high,60);

	left_elbow_bone_idx=ag_model_add_bone_single(model,"Left Elbow",left_shoulder_bone_idx,(torso_radius+arm_swing),-elbow_high,60);
	right_elbow_bone_idx=ag_model_add_bone_single(model,"Right Elbow",right_shoulder_bone_idx,-(torso_radius+arm_swing),-elbow_high,60);

	left_wrist_bone_idx=ag_model_add_bone_single(model,"Left Wrist",left_elbow_bone_idx,((torso_radius+arm_swing)+30),-(hand_high+150),0);
	right_wrist_bone_idx=ag_model_add_bone_single(model,"Right Wrist",right_elbow_bone_idx,-((torso_radius+arm_swing)+30),-(hand_high+150),0);

	ag_model_add_bone_single(model,"Left Hand",left_wrist_bone_idx,((torso_radius+arm_swing)+30),-hand_high,0);
	ag_model_add_bone_single(model,"Right Hand",right_wrist_bone_idx,-((torso_radius+arm_swing)+30),-hand_high,0);

	right_hip_bone_idx=ag_model_add_bone_single(model,"Right Hip",hip_bone_idx,-150,-(hip_high-200),0);
	left_hip_bone_idx=ag_model_add_bone_single(model,"Left Hip",hip_bone_idx,150,-(hip_high-200),0);

	left_knee_bone_idx=ag_model_add_bone_single(model,"Left Knee",left_hip_bone_idx,120,-(hip_high>>1),45);
	right_knee_bone_idx=ag_model_add_bone_single(model,"Right Knee",right_hip_bone_idx,-120,-(hip_high>>1),45);

	left_ankle_bone_idx=ag_model_add_bone_single(model,"Left Ankle",left_knee_bone_idx,120,-ankle_high,90);
	right_ankle_bone_idx=ag_model_add_bone_single(model,"Right Ankle",right_knee_bone_idx,-120,-ankle_high,90);

	left_foot_bone_idx=ag_model_add_bone_single(model,"Left Foot",left_ankle_bone_idx,120,-60,0);
	right_foot_bone_idx=ag_model_add_bone_single(model,"Right Foot",right_ankle_bone_idx,-120,-60,0);

	head_bone_idx=ag_model_add_bone_single(model,"Head",torso_bone_idx,0,-head_high,0);
	base_bone_idx=ag_model_add_bone_single(model,"Base",-1,0,0,0);
	model->bone_connect.name_bone_idx=ag_model_add_bone_single(model,"Name",head_bone_idx,0,-(head_high+1000),0);

	model->bones[0].parent_idx=base_bone_idx;		// supergumba -- re-arrange these later when animations are created

		// some random rotations

	ag_model_get_random_rotate(&rot_ang,20,0,0);
	rot_ang.x=-rot_ang.x;
	ag_model_rotate_bone(model,torso_bone_idx,&rot_ang,&torso_offset_pnt);
	ag_model_move_bone(model,head_bone_idx,&torso_offset_pnt);
	ag_model_rotate_bone(model,head_bone_idx,&rot_ang,NULL);

	ag_model_get_random_rotate(&rot_ang,5,0,10);
	ag_model_move_bone(model,right_shoulder_bone_idx,&torso_offset_pnt);
	ag_model_rotate_bone(model,right_shoulder_bone_idx,&rot_ang,NULL);
	rot_ang.z=-rot_ang.z;
	ag_model_move_bone(model,left_shoulder_bone_idx,&torso_offset_pnt);
	ag_model_rotate_bone(model,left_shoulder_bone_idx,&rot_ang,NULL);

	ag_model_get_random_rotate(&rot_ang,0,0,20);
	ag_model_rotate_bone(model,left_knee_bone_idx,&rot_ang,&left_offset_pnt);
	ag_model_move_bone(model,left_ankle_bone_idx,&left_offset_pnt);
	ag_model_move_bone(model,left_foot_bone_idx,&left_offset_pnt);

	rot_ang.z=-rot_ang.z;
	ag_model_rotate_bone(model,right_knee_bone_idx,&rot_ang,&right_offset_pnt);
	ag_model_move_bone(model,right_ankle_bone_idx,&right_offset_pnt);
	ag_model_move_bone(model,right_foot_bone_idx,&right_offset_pnt);
}
