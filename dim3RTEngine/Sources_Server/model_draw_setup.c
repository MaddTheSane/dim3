/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Model Draw Setup

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
#include "objects.h"

extern map_type				map;
extern view_type			view;
extern server_type			server;
extern iface_type			iface;
extern camera_type			camera;
extern setup_type			setup;
extern network_setup_type	net_setup;

/* =======================================================

      Dynamic Bone Setups
      
======================================================= */

void model_draw_setup_dynamic_bones(model_type *mdl,model_draw *draw,model_draw_setup *setup)
{
	int							n;
	model_draw_alter_bone_type	*alter_bone;
	model_draw_dynamic_bone		*dyn_bone;

		// clear dynamic changes

	alter_bone=setup->alter_bones;

	for (n=0;n!=mdl->nbone;n++) {
		alter_bone->parent_dist_add.x=alter_bone->parent_dist_add.y=alter_bone->parent_dist_add.z=0.0f;
		alter_bone->rot_add.x=alter_bone->rot_add.y=alter_bone->rot_add.z=0.0f;
		alter_bone->resize=1.0f;
		alter_bone++;
	}

		// setup dynamics

	dyn_bone=draw->dynamic_bones;

	for (n=0;n!=max_model_dynamic_bone;n++) {

			// any setting?

		if (dyn_bone->bone_idx==-1) {
			dyn_bone++;
			continue;
		}

		alter_bone=&setup->alter_bones[dyn_bone->bone_idx];
		
		memmove(&alter_bone->parent_dist_add,&dyn_bone->mov,sizeof(d3vct));
		memmove(&alter_bone->rot_add,&dyn_bone->rot,sizeof(d3vct));
		alter_bone->resize=dyn_bone->resize;

		dyn_bone++;
	}
}

/* =======================================================

      Model Draw Setup For Objects
      
======================================================= */

void model_draw_setup_object(obj_type *obj)
{
	int					tick,model_bounce_cnt;
	float				spin_ang;
	model_draw			*draw;
	model_draw_setup	*setup;
	model_type			*mdl;
		
	draw=&obj->draw;
	if (draw->model_idx==-1) return;

	mdl=server.model_list.models[draw->model_idx];

		// clear setup

	setup=&draw->setup;
	model_draw_setup_clear(mdl,&draw->setup);

		// special player mark

	draw->player=(obj->type==object_type_player);
	
		// position
	
	draw->pnt.x=obj->pnt.x+draw->offset.x;
	draw->pnt.z=obj->pnt.z+draw->offset.z;
	draw->pnt.y=obj->pnt.y+draw->offset.y;
	
		// regular object models can be culled
		
	draw->no_culling=FALSE;

		// ui regular lighting

	draw->ui_lighting=FALSE;
	
		// no flips
		
	draw->flip_x=FALSE;
	
		// connection settings

	draw->connect.net_sound=(obj->idx==server.player_obj_idx) || (obj->type==object_type_bot_multiplayer);
	draw->connect.net_uid=obj->remote.net_uid;
	draw->connect.motion_vct.x=obj->motion.vct.x;
	draw->connect.motion_vct.y=obj->motion.vct.y;
	draw->connect.motion_vct.z=obj->motion.vct.z;

		// rigid body offsets

	if (mdl->rigid_body.on) draw->pnt.y+=obj->motion.rigid_body_offset_y;

		// timing for bounces and spins

	tick=game_time_get();
	
		// bounces

	if (draw->bounce) {
		model_bounce_cnt=(tick>>4)&0x3F;
		if (model_bounce_cnt>32) model_bounce_cnt=64-model_bounce_cnt;
		draw->pnt.y-=model_bounce_cnt;
	}

		// center
		
	setup->center.x=draw->center.x;
	setup->center.y=draw->center.y;
	setup->center.z=draw->center.z;
	
		// angles
		
	if (draw->face_forward) {
		setup->ang.x=0.0f;
		setup->ang.y=angle_find(view.render->camera.pnt.x,view.render->camera.pnt.z,draw->pnt.x,draw->pnt.z);
		setup->ang.z=0.0f;
	}
	else {
		setup->ang.x=angle_add(obj->ang.x,draw->rot.x);
		setup->ang.y=angle_add(obj->ang.y,draw->rot.y);
		setup->ang.z=angle_add(obj->ang.z,draw->rot.z);
		
		setup->ang.x=angle_add(setup->ang.x,obj->face.ang.x);
		setup->ang.y=angle_add(setup->ang.y,obj->face.ang.y);
		setup->ang.z=angle_add(setup->ang.z,obj->face.ang.z);
	
			// spins
			
		spin_ang=(float)((tick-draw->spin_tick)>>4);

		if (draw->spin.x!=0) setup->ang.x=angle_add(setup->ang.x,(float)((int)(spin_ang*draw->spin.x)%360));
		if (draw->spin.z!=0) setup->ang.z=angle_add(setup->ang.z,(float)((int)(spin_ang*draw->spin.z)%360));
		if (draw->spin.y!=0) setup->ang.y=angle_add(setup->ang.y,(float)((int)(spin_ang*draw->spin.y)%360));
	}

		// dynamic bones

	model_draw_setup_dynamic_bones(mdl,draw,setup);
}

/* =======================================================

      Model Draw Setup For Weapons
      
======================================================= */

void model_draw_setup_weapon(obj_type *obj,weapon_type *weap,bool ignore_y_shifts,bool dual_hand)
{
	int					swap_yadd,weap_mode,
						move_tick,swap_tick,y_shift;
	float				fx,fy,fz;
	matrix_type			mat;
    model_draw			*draw;
	model_draw_setup	*setup;
	model_type			*mdl;
	
		// get proper draw and model
		
	if (!dual_hand) {
		draw=&weap->draw;
	}
	else {
		draw=&weap->draw_dual;
	}

	if (draw->model_idx==-1) return;

	mdl=server.model_list.models[draw->model_idx];

		// clear setup

	setup=&draw->setup;
	model_draw_setup_clear(mdl,&draw->setup);

		// special check for player

	draw->player=(obj->type==object_type_player);
	
        // adjust y for raise/lower modes
	
	weap_mode=obj->held_weapon.mode;
	swap_tick=obj->held_weapon.swap_tick;
	
    swap_yadd=0;
	move_tick=game_time_get();

	if (weap_mode==wm_lower) {
		move_tick-=swap_tick;
		swap_yadd=weap->hand.select_shift-(((weap->hand.raise_tick-move_tick)*weap->hand.select_shift)/weap->hand.raise_tick);
	}
	if (weap_mode==wm_raise) {
		move_tick-=swap_tick;
		swap_yadd=((weap->hand.raise_tick-move_tick)*weap->hand.select_shift)/weap->hand.raise_tick;
	}

	y_shift=obj->duck.y_move+obj->liquid.bob_y_move;
	if (!ignore_y_shifts) y_shift+=(swap_yadd+weapon_get_bounce(obj,weap));

		// get weapon position in
		// front of player

	fx=(float)weap->hand.shift.x;
	fz=(float)weap->hand.shift.z;
		
	fy=(float)(y_shift-weap->hand.shift.y);
	if (obj->bump.on) fy+=(float)obj->bump.smooth_offset;

	if (dual_hand) fx+=weap->dual.hand_offset;

		// rotate it with the camera

	matrix_rotate_x(&mat,obj->view_ang.x);
	matrix_vertex_multiply(&mat,&fx,&fy,&fz);
	
	matrix_rotate_y(&mat,angle_add(obj->ang.y,180.0f));
	matrix_vertex_multiply(&mat,&fx,&fy,&fz);

	draw->pnt.x=(int)fx+obj->pnt.x;
	draw->pnt.y=(int)fy+(obj->pnt.y+obj->size.eye_offset);
	draw->pnt.z=(int)fz+obj->pnt.z;

		// viewing angle
		
	draw->setup.ang.x=weap->hand.ang.x-obj->view_ang.x;
	draw->setup.ang.y=angle_add(obj->ang.y,weap->hand.ang.y);
	draw->setup.ang.z=weap->hand.ang.z;

	if (dual_hand) draw->setup.ang.y=360.0f-draw->setup.ang.y;

		// recoil

	weapon_recoil_add(obj,weap,&setup->ang);

		// flips

	draw->flip_x=dual_hand;

		// centering

	setup->center.x=draw->center.x;
	setup->center.y=draw->center.y;
	setup->center.z=draw->center.z;
	
		// don't cull held weapons
		
	draw->no_culling=TRUE;

		// ui lighting

	draw->ui_lighting=FALSE;

		// connection settings

	draw->connect.net_sound=(obj->idx==server.player_obj_idx);
	draw->connect.motion_vct.x=obj->motion.vct.x;
	draw->connect.motion_vct.y=obj->motion.vct.y;
	draw->connect.motion_vct.z=obj->motion.vct.z;
	
	draw->connect.weap_in_dual=dual_hand;

		// dynamic bones

	model_draw_setup_dynamic_bones(mdl,draw,setup);
}

/* =======================================================

      Model Draw Setup For Projectiles
      
======================================================= */

void model_draw_setup_projectile(proj_type *proj)
{
	int					tick,model_bounce_cnt;
	float				spin_ang;
	model_draw			*draw;
	model_draw_setup	*setup;
	model_type			*mdl;
		
	draw=&proj->draw;
	if (draw->model_idx==-1) return;

	mdl=server.model_list.models[draw->model_idx];

		// clear setup

	setup=&draw->setup;
	model_draw_setup_clear(mdl,&draw->setup);

		// player check flag

	draw->player=FALSE;
	
		// position
	
	draw->pnt.x=proj->pnt.x+draw->offset.x;
	draw->pnt.z=proj->pnt.z+draw->offset.z;
	draw->pnt.y=proj->pnt.y+draw->offset.y;
	
		// regular projectile models can be culled
		
	draw->no_culling=FALSE;

		// ui lighting

	draw->ui_lighting=FALSE;
	
		// no flips
		
	draw->flip_x=FALSE;
	
		// connection settings
		
	draw->connect.motion_vct.x=proj->motion.vct.x;
	draw->connect.motion_vct.y=proj->motion.vct.y;
	draw->connect.motion_vct.z=proj->motion.vct.z;

		// timing for bounces and spins

	tick=game_time_get();
	
		// bounces

	if (draw->bounce) {
		model_bounce_cnt=(tick>>4)&0x3F;
		if (model_bounce_cnt>32) model_bounce_cnt=64-model_bounce_cnt;
		draw->pnt.y-=model_bounce_cnt;
	}

		// center
		
	setup->center.x=draw->center.x;
	setup->center.y=draw->center.y;
	setup->center.z=draw->center.z;
	
		// angles
		
	if (draw->face_forward) {
		setup->ang.x=angle_find(view.render->camera.pnt.y,view.render->camera.pnt.z,draw->pnt.y,draw->pnt.z);
		setup->ang.y=angle_find(view.render->camera.pnt.x,view.render->camera.pnt.z,draw->pnt.x,draw->pnt.z);
		setup->ang.z=0;
	}
	else {
		setup->ang.x=angle_add(proj->ang.x,draw->rot.x);
		setup->ang.y=angle_add(proj->ang.y,draw->rot.y);
		setup->ang.z=angle_add(proj->ang.z,draw->rot.z);
	
			// spins
			
		spin_ang=(float)(tick>>4);

		if (draw->spin.x!=0) setup->ang.x=angle_add(setup->ang.x,(float)((int)(spin_ang*draw->spin.x)%360));
		if (draw->spin.z!=0) setup->ang.z=angle_add(setup->ang.z,(float)((int)(spin_ang*draw->spin.z)%360));
		if (draw->spin.y!=0) setup->ang.y=angle_add(setup->ang.y,(float)((int)(spin_ang*draw->spin.y)%360));
	}

		// dynamic bones

	model_draw_setup_dynamic_bones(mdl,draw,setup);
}

/* =======================================================

      Model Draw Setup For Interface Models
      
======================================================= */

void model_draw_setup_interface_models(model_type *mdl,model_draw *draw,int x,int y)
{
	int				n;

	model_draw_setup_clear(mdl,&draw->setup);

	draw->on=TRUE;
	
		// don't cull interface
		// normals as there's no
		// real camera
		
	draw->no_culling=TRUE;

		// turn on ui lighting

	draw->ui_lighting=TRUE;
	draw->flip_x=TRUE;
	
	draw->light_cache.count=0;

	draw->alpha=1.0f;

		// need to change point for
		// resizes and projection

	x-=(iface.scale_x>>1);
	x=(int)(((float)x)*5.8f);

	y-=(iface.scale_y>>1);
	y=(int)(((float)y)*6.0f);
	y+=(int)((1.0f-draw->resize)*((float)(mdl->view_box.size.y>>1)));

	draw->pnt.x=x+draw->offset.x;
	draw->pnt.y=y+draw->offset.y;
	draw->pnt.z=draw->offset.z;

		// facing angle

	memmove(&draw->setup.ang,&draw->rot,sizeof(d3ang));

		// only draw first mesh

	draw->mesh_mask=0x1;

		// fix any animation or frames

	for (n=0;n!=mdl->nmesh;n++) {
		draw->meshes[n].fade.on=FALSE;
	}

	for (n=0;n!=max_model_texture;n++) {
		draw->textures[n].animation_on=FALSE;
		draw->textures[n].animation_reverse=FALSE;
		draw->textures[n].frame=0;
	}
}

/* =======================================================

      Utility to get Model Min/Max in View
      
======================================================= */

void model_get_view_min_max(model_draw *draw,d3pnt *pnt,d3pnt *min,d3pnt *max)
{
	int					cx,cy,cz,sz;
	
		// get model bounds

	sz=draw->size.x>>1;
	min->x=draw->pnt.x-sz;
	max->x=draw->pnt.x+sz;

	sz=draw->size.z>>1;
	min->z=draw->pnt.z-sz;
	max->z=draw->pnt.z+sz;

	min->y=draw->pnt.y-draw->size.y;
	max->y=draw->pnt.y;

		// any rotations

	cx=draw->pnt.x+draw->center.x;
	cy=draw->pnt.y+draw->center.y;
	cz=draw->pnt.z+draw->center.z;

	rotate_point(&min->x,&min->y,&min->z,cx,cy,cz,draw->rot.x,draw->rot.y,draw->rot.z);
	rotate_point(&max->x,&max->y,&max->z,cx,cy,cz,draw->rot.x,draw->rot.y,draw->rot.z);
}
