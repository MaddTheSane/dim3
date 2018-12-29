/****************************** File *********************************

Module: dim3 Map Utility
Author: Brian Barnes
 Usage: Map File Reading

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
	#include "dim3maputility.h"
#endif

extern file_path_setup_type	file_path_setup;

char					media_type_str[][32]={"none","chooser","title","movie",""},
						camera_mode_str[][32]={"fpp","chase","static","chase_static",""},
						camera_input_str[][32]={"fpp","side_scroll","top_down","fly","thrust"},
						sky_type_str[][32]={"dome_panoramic","dome_hemisphere","cube",""},
						gl_fog_type_str[][32]={"linear","exp","exp2",""},
						mesh_hide_mode_str[][32]={"never","single_player","multiplayer",""},
						mesh_normal_mode_str[][32]={"auto","in","out","in_out","edge","lock",""},
                        light_type_str[][32]={"normal","blink","glow","pulse","flicker","failing",""},
                        light_direction_str[][32]={"all","neg_x","pos_x","neg_y","pos_y","neg_z","pos_z",""},
						spot_type_str[][32]={"Object","Bot","Player","Spawn","Checkpoint",""},
						skill_type_str[][32]={"easy","medium","hard",""},
						spawn_type_str[][32]={"always","single_player","multiplayer",""},
						cinema_action_type_str[][32]={"none","place","move","stop","show","show_fade","hide","fade in","fade out",""},
						cinema_actor_type_str[][32]={"camera","player","object","movement","particle","sound","hud_text","hud_bitmap",""};

/* =======================================================

      Read Map Settings
      
======================================================= */

void decode_map_settings_xml(map_type *map,int map_head)
{
	int				n,main_ambient_light_tag,main_ambient_sound_tag,main_rain_tag,
					main_background_tag,main_sky_tag,main_fog_tag,
					tag;
	char			name[32];
	
        // map info
    
    tag=xml_findfirstchild("Info",map_head);
    if (tag!=-1) {
        xml_get_attribute_text(tag,"name",map->info.title,name_str_len);
        xml_get_attribute_text(tag,"author",map->info.author,name_str_len);
   }
    
    tag=xml_findfirstchild("Settings",map_head);
    if (tag!=-1) {
		map->physics.gravity=xml_get_attribute_float_default(tag,"gravity",1.0f);
        map->physics.gravity_max_power=xml_get_attribute_float_default(tag,"gravity_max_power",32.0f);
		map->physics.gravity_max_speed=xml_get_attribute_float_default(tag,"gravity_max_speed",400.0f);
		map->physics.resistance=xml_get_attribute_float_default(tag,"resistance",1.0f);
		map->physics.slope_min_ang=xml_get_attribute_float_default(tag,"slope_min_ang",25.0f);
		map->physics.slope_max_ang=xml_get_attribute_float_default(tag,"slope_max_ang",45.0f);
		map->physics.slope_max_speed=xml_get_attribute_float_default(tag,"slope_max_speed",250.0f);
		map->physics.slope_min_gravity=xml_get_attribute_float_default(tag,"slope_min_gravity",200.0f);
	}
	
    tag=xml_findfirstchild("Optimize",map_head);
    if (tag!=-1) {
		map->optimize.never_cull=xml_get_attribute_boolean(tag,"never_cull");
		map->optimize.cull_angle=xml_get_attribute_float_default(tag,"cull_angle",0.0f);
		map->optimize.ray_trace_obscure=xml_get_attribute_boolean(tag,"ray_trace_obscure");
		map->optimize.obscure_dist.model=xml_get_attribute_int(tag,"model_obscure_distance");
		map->optimize.obscure_dist.shadow=xml_get_attribute_int(tag,"shadow_obscure_distance");
		map->optimize.obscure_dist.effect=xml_get_attribute_int(tag,"effect_obscure_distance");
		map->optimize.shadow_poly_min_area=xml_get_attribute_int(tag,"shadow_poly_min_area");
		map->optimize.shadow_floor_only=xml_get_attribute_boolean(tag,"shadow_floor_only");
	}

    tag=xml_findfirstchild("Singleplayer",map_head);
    if (tag!=-1) {
 		map->singleplayer.map_picker=xml_get_attribute_boolean(tag,"map_picker");
   }
	
    tag=xml_findfirstchild("Network",map_head);
    if (tag!=-1) {
        xml_get_attribute_text(tag,"game_list",map->multiplayer.game_list,256);
		xml_get_attribute_text(tag,"params",map->settings.params,param_str_len);
	}
	
    tag=xml_findfirstchild("LightMap",map_head);
    if (tag!=-1) {
        map->light_map.quality=xml_get_attribute_int_default(tag,"quality",2);
        map->light_map.size=xml_get_attribute_int_default(tag,"size",2);
		map->light_map.use_normals=xml_get_attribute_boolean(tag,"use_normals");
		map->light_map.skip_glows=xml_get_attribute_boolean(tag,"skip_glows");
        map->light_map.diffuse_boost=xml_get_attribute_float_default(tag,"diffuse_boost",0.0f);
		
		if (map->light_map.size>=256) map->light_map.size=((int)log2(map->light_map.size))-8;
	}
	
    tag=xml_findfirstchild("Editor",map_head);
    if (tag!=-1) {
		map->editor_setup.view_near_dist=xml_get_attribute_int_default(tag,"view_near_dist",400);
		map->editor_setup.view_far_dist=xml_get_attribute_int_default(tag,"view_far_dist",300000);
		map->editor_setup.link_always_start=xml_get_attribute_boolean(tag,"link_always_start");
	}
	
    tag=xml_findfirstchild("Media",map_head);
    if (tag!=-1) {
		map->media.type=xml_get_attribute_list(tag,"type",(char*)media_type_str);
		map->media.event_id=xml_get_attribute_int_default(tag,"event_id",-1);
		xml_get_attribute_text(tag,"name",map->media.name,name_str_len);
		xml_get_attribute_text(tag,"title_sound_name",map->media.title_sound_name,name_str_len);
	}
	
    tag=xml_findfirstchild("Music",map_head);
    if (tag!=-1) {
		map->music.fade_msec=xml_get_attribute_int(tag,"fade_msec");
		xml_get_attribute_text(tag,"name",map->music.name,name_str_len);

		for (n=0;n!=max_music_preload;n++) {
			sprintf(name,"preload_%d",n);
			xml_get_attribute_text(tag,name,map->music.preload_name[n],name_str_len);
		}
	}
	
	main_ambient_light_tag=xml_findfirstchild("Ambient_Light",map_head);
    if (main_ambient_light_tag!=-1) {
		xml_get_attribute_color(main_ambient_light_tag,"rgb",&map->ambient.light_color);
	}
	
	main_ambient_sound_tag=xml_findfirstchild("Ambient_Sound",map_head);
    if (main_ambient_sound_tag!=-1) {
		xml_get_attribute_text(main_ambient_sound_tag,"name",map->ambient.sound_name,name_str_len);
		map->ambient.sound_pitch=xml_get_attribute_float_default(main_ambient_sound_tag,"pitch",1.0f);
	}
	
	main_background_tag=xml_findfirstchild("Background",map_head);
    if (main_background_tag!=-1) {
		map->background.on=xml_get_attribute_boolean(main_background_tag,"on");
		map->background.front.fill=xml_get_attribute_int(main_background_tag,"front_fill");
		xml_get_attribute_2_coord_float(main_background_tag,"front_stamp",&map->background.front.size.x,&map->background.front.size.y);
		xml_get_attribute_2_coord_float(main_background_tag,"front_scroll",&map->background.front.scroll_factor.x,&map->background.front.scroll_factor.y);
		if (!xml_get_attribute_2_coord_float(main_background_tag,"front_clip",&map->background.front.clip.x,&map->background.front.clip.y)) {
			map->background.front.clip.x=0.0f;
			map->background.front.clip.y=1.0f;
		}

		map->background.middle.fill=xml_get_attribute_int(main_background_tag,"middle_fill");
		xml_get_attribute_2_coord_float(main_background_tag,"middle_stamp",&map->background.middle.size.x,&map->background.middle.size.y);
		xml_get_attribute_2_coord_float(main_background_tag,"middle_scroll",&map->background.middle.scroll_factor.x,&map->background.middle.scroll_factor.y);
		if (!xml_get_attribute_2_coord_float(main_background_tag,"middle_clip",&map->background.middle.clip.x,&map->background.middle.clip.y)) {
			map->background.middle.clip.x=0.0f;
			map->background.middle.clip.y=1.0f;
		}

		map->background.back.fill=xml_get_attribute_int(main_background_tag,"back_fill");
		xml_get_attribute_2_coord_float(main_background_tag,"back_stamp",&map->background.back.size.x,&map->background.back.size.y);
		xml_get_attribute_2_coord_float(main_background_tag,"back_scroll",&map->background.back.scroll_factor.x,&map->background.back.scroll_factor.y);
 		if (!xml_get_attribute_2_coord_float(main_background_tag,"back_clip",&map->background.back.clip.x,&map->background.back.clip.y)) {
			map->background.back.clip.x=0.0f;
			map->background.back.clip.y=1.0f;
		}
   }
  
	main_sky_tag=xml_findfirstchild("Sky",map_head);
    if (main_sky_tag!=-1) {
		tag=xml_findfirstchild("Settings",main_sky_tag);
		if (tag!=-1) {
			map->sky.on=xml_get_attribute_boolean(tag,"on");
			map->sky.type=xml_get_attribute_list(tag,"type",(char*)sky_type_str);
			map->sky.radius=xml_get_attribute_int(tag,"radius");
			map->sky.dome_y=xml_get_attribute_int(tag,"dome_y");
			map->sky.dome_mirror=xml_get_attribute_boolean(tag,"dome_mirror");
		}
		tag=xml_findfirstchild("Fill",main_sky_tag);
		if (tag!=-1) {
			map->sky.fill=xml_get_attribute_int_default(tag,"index",-1);
			map->sky.bottom_fill=xml_get_attribute_int_default(tag,"bottom_index",-1);
			map->sky.north_fill=xml_get_attribute_int_default(tag,"north_index",-1);
			map->sky.south_fill=xml_get_attribute_int_default(tag,"south_index",-1);
			map->sky.east_fill=xml_get_attribute_int_default(tag,"east_index",-1);
			map->sky.west_fill=xml_get_attribute_int_default(tag,"west_index",-1);
			map->sky.txt_fact=xml_get_attribute_float_default(tag,"factor",1);
		}
    }

	main_fog_tag=xml_findfirstchild("Fog",map_head);
    if (main_fog_tag!=-1) {
		map->fog.on=xml_get_attribute_boolean(main_fog_tag,"on");
		map->fog.count=xml_get_attribute_int(main_fog_tag,"count");
		map->fog.inner_radius=xml_get_attribute_int(main_fog_tag,"inner_radius");
		map->fog.outer_radius=xml_get_attribute_int(main_fog_tag,"outer_radius");
		map->fog.high=xml_get_attribute_int(main_fog_tag,"high");
		map->fog.drop=xml_get_attribute_int(main_fog_tag,"drop");
		map->fog.texture_idx=xml_get_attribute_int_default(main_fog_tag,"texture_index",-1);
		map->fog.speed=xml_get_attribute_float(main_fog_tag,"speed");
		map->fog.txt_fact.x=xml_get_attribute_float(main_fog_tag,"txt_x_fact");
		map->fog.txt_fact.y=xml_get_attribute_float(main_fog_tag,"txt_y_fact");
		xml_get_attribute_color(main_fog_tag,"rgb",&map->fog.col);
		map->fog.alpha=xml_get_attribute_float(main_fog_tag,"alpha");
		map->fog.use_solid_color=xml_get_attribute_boolean(main_fog_tag,"use_solid_color");
    }
	
	main_rain_tag=xml_findfirstchild("Rain",map_head);
    if (main_rain_tag!=-1) {
		map->rain.on=xml_get_attribute_boolean(main_rain_tag,"on");
		map->rain.density=xml_get_attribute_int(main_rain_tag,"density");
		map->rain.radius=xml_get_attribute_int(main_rain_tag,"radius");
		map->rain.height=xml_get_attribute_int(main_rain_tag,"height");
		map->rain.speed=xml_get_attribute_int(main_rain_tag,"speed");
		map->rain.line_width=xml_get_attribute_int(main_rain_tag,"line_width");
		map->rain.line_length=xml_get_attribute_int(main_rain_tag,"line_length");
		map->rain.slant_add=xml_get_attribute_int(main_rain_tag,"slant_add");
		map->rain.slant_time_msec=xml_get_attribute_int(main_rain_tag,"slant_time_msec");
		map->rain.slant_change_msec=xml_get_attribute_int(main_rain_tag,"slant_change_msec");
		map->rain.alpha=xml_get_attribute_float(main_rain_tag,"alpha");
		xml_get_attribute_color(main_rain_tag,"start_rgb",&map->rain.start_color);
		xml_get_attribute_color(main_rain_tag,"end_rgb",&map->rain.end_color);
	}
}

/* =======================================================

      Read Groups
      
======================================================= */

void decode_map_groups_xml(map_type *map,int map_head)
{
	int				n,ngroup,idx,
					main_group_tag,group_tag;

    main_group_tag=xml_findfirstchild("Groups",map_head);
    if (main_group_tag==-1) return;
    
	map->group.ngroup=0;

    ngroup=xml_countchildren(main_group_tag);
	group_tag=xml_findfirstchild("Group",main_group_tag);

    for (n=0;n!=ngroup;n++) {
		idx=map_group_add(map);
		if (idx==-1) break;

 		xml_get_attribute_text(group_tag,"name",map->group.groups[idx].name,name_str_len);
		group_tag=xml_findnextchild(group_tag);
	}
}

/* =======================================================

      Read Movements
      
======================================================= */

void decode_map_movements_xml(map_type *map,int map_head)
{
	int						n,k,nmovement,nmove,movement_idx,move_idx,
							main_movement_tag,movement_tag,main_movement_move_tag,movement_move_tag;
	movement_type			*movement;
	movement_move_type		*move;
    
    main_movement_tag=xml_findfirstchild("Movements",map_head);
    if (main_movement_tag==-1) return;

	map->movement.nmovement=0;
    
    nmovement=xml_countchildren(main_movement_tag);
	movement_tag=xml_findfirstchild("Movement",main_movement_tag);

    for (n=0;n!=nmovement;n++) {
		movement_idx=map_movement_add(map);
		if (movement_idx==-1) break;

		movement=&map->movement.movements[movement_idx];
	
 		xml_get_attribute_text(movement_tag,"name",movement->name,name_str_len);
		movement->group_idx=xml_get_attribute_int(movement_tag,"group");
		movement->reverse_group_idx=xml_get_attribute_int_default(movement_tag,"group_reverse",-1);
 		movement->auto_start=xml_get_attribute_boolean(movement_tag,"auto_start");
 		movement->auto_open=xml_get_attribute_boolean(movement_tag,"auto_open");
 		movement->auto_open_stand=xml_get_attribute_boolean(movement_tag,"auto_open_stand");
 		movement->auto_open_never_close=xml_get_attribute_boolean(movement_tag,"auto_open_never_close");
 		movement->auto_open_distance=xml_get_attribute_int(movement_tag,"auto_open_distance");
		movement->loop=xml_get_attribute_boolean(movement_tag,"loop");
		
		main_movement_move_tag=xml_findfirstchild("Moves",movement_tag);
		if (main_movement_move_tag!=-1) {
			
			movement->nmove=0;

			nmove=xml_countchildren(main_movement_move_tag);
			movement_move_tag=xml_findfirstchild("Move",main_movement_move_tag);
		
			for (k=0;k!=nmove;k++) {
				move_idx=map_movement_move_add(map,movement_idx);
				if (move_idx==-1) break;

				move=&movement->moves[move_idx];

				xml_get_attribute_3_coord_int(movement_move_tag,"c3",&move->mov.x,&move->mov.y,&move->mov.z);
				xml_get_attribute_3_coord_float(movement_move_tag,"r3",&move->rot.x,&move->rot.y,&move->rot.z);
				move->msec=(short)xml_get_attribute_int(movement_move_tag,"msec");
				move->user_id=(short)xml_get_attribute_int(movement_move_tag,"user_id");
				xml_get_attribute_text(movement_move_tag,"sound_name",move->sound_name,name_str_len);
				move->sound_pitch=xml_get_attribute_float(movement_move_tag,"sound_pitch");
				
				movement_move_tag=xml_findnextchild(movement_move_tag);
			}
		}
		
		movement_tag=xml_findnextchild(movement_tag);
	}
}

/* =======================================================

      Read Textures
      
======================================================= */

void decode_map_textures_xml(map_type *map,int map_head)
{
	int						i,main_fill_tag,fill_tag,
							id,cnt;
 
    main_fill_tag=xml_findfirstchild("Fills",map_head);
    if (main_fill_tag==-1) return;
    
    cnt=xml_countchildren(main_fill_tag);
	fill_tag=xml_findfirstchild("Fill",main_fill_tag);

    for (i=0;i!=cnt;i++) {
		id=xml_get_attribute_int(fill_tag,"id");
		bitmap_texture_read_xml(&map->textures[id],fill_tag,TRUE);
		fill_tag=xml_findnextchild(fill_tag);
    }
}

/* =======================================================

      Read Cinemas
      
======================================================= */

void decode_map_cinemas_xml(map_type *map,int map_head)
{
	int						n,k,cinema_idx,action_idx,ncinema,naction,
							cinemas_tag,cinema_tag,actions_tag,action_tag;
	map_cinema_type			*cinema;
	map_cinema_action_type	*action;
    
    cinemas_tag=xml_findfirstchild("Cinemas",map_head);
    if (cinemas_tag==-1) return;
    
    map->cinema.ncinema=0;
	
	ncinema=xml_countchildren(cinemas_tag);
	cinema_tag=xml_findfirstchild("Cinema",cinemas_tag);

    for (n=0;n!=ncinema;n++) {
		cinema_idx=map_cinema_add(map);
		if (cinema_idx==-1) break;

		cinema=&map->cinema.cinemas[cinema_idx];

		xml_get_attribute_text(cinema_tag,"name",cinema->name,name_str_len);
		cinema->len_msec=xml_get_attribute_int(cinema_tag,"length");
		cinema->freeze_input=xml_get_attribute_boolean(cinema_tag,"freeze_input");
		cinema->show_hud=xml_get_attribute_boolean(cinema_tag,"show_hud");
		cinema->no_cancel=xml_get_attribute_boolean(cinema_tag,"no_cancel");
		
		cinema->naction=0;
		
		actions_tag=xml_findfirstchild("Actions",cinema_tag);
		if (actions_tag==-1) continue;

		naction=xml_countchildren(actions_tag);
		action_tag=xml_findfirstchild("Action",actions_tag);
		
		for (k=0;k!=naction;k++) {
			action_idx=map_cinema_add_action(map,cinema_idx);
			if (action_idx==-1) break;

			action=&cinema->actions[action_idx];

			action->action=xml_get_attribute_list(action_tag,"action",(char*)cinema_action_type_str);

			action->actor_type=xml_get_attribute_list(action_tag,"actor_type",(char*)cinema_actor_type_str);
			xml_get_attribute_text(action_tag,"actor_name",action->actor_name,name_str_len);

			action->start_msec=xml_get_attribute_int_default(action_tag,"start",0);
			action->end_msec=xml_get_attribute_int_default(action_tag,"end",0);

			action->move_reverse=xml_get_attribute_boolean(action_tag,"move_reverse");

			xml_get_attribute_text(action_tag,"animation",action->animation_name,name_str_len);
			xml_get_attribute_text(action_tag,"next_animation",action->next_animation_name,name_str_len);
			xml_get_attribute_text(action_tag,"node",action->node_name,name_str_len);
			
			xml_get_attribute_text(action_tag,"text",action->text_str,256);
			
			action_tag=xml_findnextchild(action_tag);
		}
		
		cinema_tag=xml_findnextchild(cinema_tag);
	}
}

/* =======================================================

      Read Cameras
      
======================================================= */

void decode_map_camera_xml(map_type *map,int map_head)
{
	int				camera_tag,tag;
	
    camera_tag=xml_findfirstchild("Camera",map_head);
    if (camera_tag==-1) return;
	
	map->camera.camera_mode=xml_get_attribute_list(camera_tag,"mode",(char*)camera_mode_str);
	map->camera.input_mode=xml_get_attribute_list(camera_tag,"input",(char*)camera_input_str);
	xml_get_attribute_3_coord_float(camera_tag,"ang",&map->camera.ang_offset.x,&map->camera.ang_offset.y,&map->camera.ang_offset.z);
	xml_get_attribute_3_coord_int(camera_tag,"pnt",&map->camera.pnt_offset.x,&map->camera.pnt_offset.y,&map->camera.pnt_offset.z);
	
    tag=xml_findfirstchild("Plane",camera_tag);
    if (tag!=-1) {
		map->camera.plane.fov=xml_get_attribute_float(tag,"fov");
		map->camera.plane.aspect_ratio=xml_get_attribute_float(tag,"aspectRatio");
		map->camera.plane.near_z=xml_get_attribute_int(tag,"near");
		map->camera.plane.far_z=xml_get_attribute_int(tag,"far");
		map->camera.plane.near_z_offset=xml_get_attribute_int(tag,"nearOffset");
	}
	
    tag=xml_findfirstchild("Chase",camera_tag);
    if (tag!=-1) {
		map->camera.chase.distance=xml_get_attribute_int(tag,"distance");
		map->camera.chase.track_speed=xml_get_attribute_float(tag,"track_speed");
		if (!xml_get_attribute_3_coord_float(tag,"track_ang",&map->camera.chase.track_ang.x,&map->camera.chase.track_ang.y,&map->camera.chase.track_ang.z)) {
			memmove(&map->camera.chase.track_ang,&map->camera.ang_offset,sizeof(d3ang));
			map->camera.ang_offset.x=map->camera.ang_offset.y=map->camera.ang_offset.z=0.0f;
		}
		xml_get_attribute_3_coord_float(tag,"slop",&map->camera.chase.slop.x,&map->camera.chase.slop.y,&map->camera.chase.slop.z);
	}
	
    tag=xml_findfirstchild("Static",camera_tag);
    if (tag!=-1) {
		map->camera.c_static.follow=xml_get_attribute_boolean(tag,"follow");
		xml_get_attribute_text(tag,"attach_node",map->camera.c_static.attach_node,name_str_len);
	}
}

/* =======================================================

      Read Editor Views
      
======================================================= */

void read_map_editor_views_xml(map_type *map,int map_head)
{
	int						n,main_views_tag,view_tag;
	editor_view_type		*view;

	map->editor_views.count=0;

    main_views_tag=xml_findfirstchild("Editor_Views",map_head);
    if (main_views_tag==-1) return;

    map->editor_views.count=xml_countchildren(main_views_tag);
	view_tag=xml_findfirstchild("Editor_View",main_views_tag);

    view=map->editor_views.views;
    
    for (n=0;n!=map->editor_views.count;n++) {
    
		xml_get_attribute_2_coord_float(view_tag,"box_top_left",&view->box.top,&view->box.lft);
		xml_get_attribute_2_coord_float(view_tag,"box_bottom_right",&view->box.bot,&view->box.rgt);
		xml_get_attribute_3_coord_int(view_tag,"pnt",&view->pnt.x,&view->pnt.y,&view->pnt.z);
		xml_get_attribute_3_coord_float(view_tag,"ang",&view->ang.x,&view->ang.y,&view->ang.z);
		view->uv_layer=xml_get_attribute_int(view_tag,"uv_layer");
  		view->ortho=xml_get_attribute_boolean(view_tag,"ortho");
    	view->cull=xml_get_attribute_boolean(view_tag,"cull");
     	view->clip=xml_get_attribute_boolean(view_tag,"clip");
     	view->no_rot=xml_get_attribute_boolean(view_tag,"no_rot");
    
		view_tag=xml_findnextchild(view_tag);
        view++;
    }
}

/* =======================================================

      Check Map Network Game Types
      
======================================================= */
    
bool map_host_load_info(char *map_name,char *info_name,bool *singleplayer_map_picker,char *game_list)
{
	int				map_head,tag;
	char			path[1024];

	file_paths_data(&file_path_setup,path,"Maps",map_name,"xml");

		// open XML file

	if (!xml_open_file(path)) return(FALSE);

    map_head=xml_findrootchild("Map");
    if (map_head==-1) {
		xml_close_file();
		return(FALSE);
    }
	
		// get info name
		
	info_name[0]=0x0;
	
    tag=xml_findfirstchild("Info",map_head);
    if (tag!=-1) {
        xml_get_attribute_text(tag,"name",info_name,name_str_len);
    }
	
	if (info_name[0]==0x0) strcpy(info_name,map_name);

		// get singleplayer map picker

	*singleplayer_map_picker=FALSE;

    tag=xml_findfirstchild("Singleplayer",map_head);
    if (tag!=-1) {
        *singleplayer_map_picker=xml_get_attribute_boolean(tag,"map_picker");
	}

		// get game list
		
	game_list[0]=0x0;

    tag=xml_findfirstchild("Network",map_head);
    if (tag!=-1) {
        xml_get_attribute_text(tag,"game_list",game_list,256);
	}

	xml_close_file();

	return(TRUE);
}

/* =======================================================

      Read Meshes and Liquids
      
======================================================= */

bool read_single_mesh_v3(map_type *map,int mesh_idx,int mesh_tag)
{
	int					n,nvertex,npoly,
						msg_tag,main_vertex_tag,vertex_tag,main_poly_tag,poly_tag,tag;
	d3pnt				*pt;
	map_mesh_type		*mesh;
	map_mesh_poly_type	*poly;

	mesh=&map->mesh.meshes[mesh_idx];

		// mesh settings

	mesh->group_idx=xml_get_attribute_int_default(mesh_tag,"group",-1);

	mesh->flag.on=!xml_get_attribute_boolean(mesh_tag,"off");
	mesh->flag.pass_through=xml_get_attribute_boolean(mesh_tag,"pass");
	mesh->flag.moveable=xml_get_attribute_boolean(mesh_tag,"moveable");
	mesh->flag.hilite=xml_get_attribute_boolean(mesh_tag,"hilite");
	mesh->flag.simple_collision=xml_get_attribute_boolean(mesh_tag,"simple_collision");
	mesh->flag.lock_uv=xml_get_attribute_boolean(mesh_tag,"lock_uv");
	mesh->flag.lock_move=xml_get_attribute_boolean(mesh_tag,"lock_move");
	mesh->flag.cascade_size=xml_get_attribute_boolean(mesh_tag,"cascade_size");
	mesh->flag.never_obscure=xml_get_attribute_boolean(mesh_tag,"never_obscure");
	mesh->flag.never_cull=xml_get_attribute_boolean(mesh_tag,"never_cull");
	mesh->flag.rot_independent=xml_get_attribute_boolean(mesh_tag,"rot_independent");
	mesh->flag.no_light_map=xml_get_attribute_boolean(mesh_tag,"no_light_map");
	mesh->flag.skip_light_map_trace=xml_get_attribute_boolean(mesh_tag,"skip_light_map_trace");
	mesh->flag.no_halo_obscure=xml_get_attribute_boolean(mesh_tag,"no_halo_obscure");
	
	mesh->hide_mode=xml_get_attribute_list(mesh_tag,"hide",(char*)mesh_hide_mode_str);
	
	mesh->harm=xml_get_attribute_int_default(mesh_tag,"harm",0);

	xml_get_attribute_3_coord_int(mesh_tag,"rot_off",&mesh->rot_off.x,&mesh->rot_off.y,&mesh->rot_off.z);

		// messages

    mesh->msg.entry_on=mesh->msg.exit_on=mesh->msg.base_on=mesh->msg.map_change_on=FALSE;
    mesh->msg.entry_id=mesh->msg.exit_id=0;
	mesh->msg.map_name[0]=0x0;
	strcpy(mesh->msg.map_spot_name,"Start");
    
    msg_tag=xml_findfirstchild("Messages",mesh_tag);
    if (msg_tag!=-1) {
        tag=xml_findfirstchild("Entry",msg_tag);
        if (tag!=-1) {
            mesh->msg.entry_on=xml_get_attribute_boolean(tag,"on");
            mesh->msg.entry_id=xml_get_attribute_int(tag,"id");
        }
        tag=xml_findfirstchild("Exit",msg_tag);
        if (tag!=-1) {
            mesh->msg.exit_on=xml_get_attribute_boolean(tag,"on");
            mesh->msg.exit_id=xml_get_attribute_int(tag,"id");
        }
        tag=xml_findfirstchild("Base",msg_tag);
        if (tag!=-1) {
            mesh->msg.base_on=xml_get_attribute_boolean(tag,"on");
            mesh->msg.base_team=xml_get_attribute_int(tag,"team");
        }
        tag=xml_findfirstchild("Map",msg_tag);
        if (tag!=-1) {
            mesh->msg.map_change_on=xml_get_attribute_boolean(tag,"on");
			xml_get_attribute_text(tag,"name",mesh->msg.map_name,name_str_len);
			xml_get_attribute_text(tag,"spot_name",mesh->msg.map_spot_name,name_str_len);
        }
	}
	
		// import
		
	tag=xml_findfirstchild("Import",mesh_tag);
	if (tag!=-1) {
		xml_get_attribute_text(tag,"obj",mesh->import.obj_name,name_str_len);
		xml_get_attribute_text(tag,"group",mesh->import.group_name,name_str_len);
	}
	else {
		mesh->import.obj_name[0]=0x0;
		mesh->import.group_name[0]=0x0;
	}

		// vertexes

    main_vertex_tag=xml_findfirstchild("Vertexes",mesh_tag);
    if (main_vertex_tag!=-1) {
    
        nvertex=xml_countchildren(main_vertex_tag);
		vertex_tag=xml_findfirstchild("v",main_vertex_tag);

		if (!map_mesh_set_vertex_count(map,mesh_idx,nvertex)) return(FALSE);

		pt=mesh->vertexes;

		for (n=0;n!=nvertex;n++) {
			xml_get_attribute_3_coord_int(vertex_tag,"c3",&pt->x,&pt->y,&pt->z);
			pt++;
			vertex_tag=xml_findnextchild(vertex_tag);
		}
	}

		// polys

    main_poly_tag=xml_findfirstchild("Polys",mesh_tag);
    if (main_poly_tag!=-1) {
    
        npoly=xml_countchildren(main_poly_tag);
		poly_tag=xml_findfirstchild("p",main_poly_tag);

		if (!map_mesh_set_poly_count(map,mesh_idx,npoly)) return(FALSE);

		poly=mesh->polys;

		for (n=0;n!=npoly;n++) {
			poly->ptsz=xml_get_attribute_int_array(poly_tag,"v",poly->v,8);
			
			poly->txt_idx=xml_get_attribute_int(poly_tag,"txt");
			poly->lmap_txt_idx=xml_get_attribute_int_default(poly_tag,"lmp",-1);

			xml_get_attribute_3_coord_float(poly_tag,"t3",&poly->tangent_space.tangent.x,&poly->tangent_space.tangent.y,&poly->tangent_space.tangent.z);
			xml_get_attribute_3_coord_float(poly_tag,"n3",&poly->tangent_space.normal.x,&poly->tangent_space.normal.y,&poly->tangent_space.normal.z);

			xml_get_attribute_uv_x_array(poly_tag,"x",poly->main_uv.uvs,8);
			xml_get_attribute_uv_y_array(poly_tag,"y",poly->main_uv.uvs,8);
			xml_get_attribute_2_coord_float(poly_tag,"shift",&poly->shift.x,&poly->shift.y);

			if (poly->lmap_txt_idx!=-1) {
				xml_get_attribute_uv_x_array(poly_tag,"x_1",poly->lmap_uv.uvs,8);
				xml_get_attribute_uv_y_array(poly_tag,"y_1",poly->lmap_uv.uvs,8);
			}

			poly->flag.climbable=xml_get_attribute_boolean(poly_tag,"climbable");
			poly->flag.never_cull=xml_get_attribute_boolean(poly_tag,"never_cull");
			poly->flag.obscuring=xml_get_attribute_boolean(poly_tag,"obscuring");

			xml_get_attribute_text(poly_tag,"camera",poly->camera,name_str_len);

			poly++;
			poly_tag=xml_findnextchild(poly_tag);
		}
	}

	return(TRUE);
}

void read_single_liquid_v3(map_type *map,int liquid_idx,int liquid_tag)
{
	int					tag;
	map_liquid_type		*liq;

	liq=&map->liquid.liquids[liquid_idx];

		// liquid settings

	liq->group_idx=xml_get_attribute_int_default(liquid_tag,"group",-1);
	liq->flag.never_obscure=xml_get_attribute_boolean(liquid_tag,"never_obscure");
	liq->flag.never_cull=xml_get_attribute_boolean(liquid_tag,"never_cull");
	liq->flag.no_draw=xml_get_attribute_boolean(liquid_tag,"no_draw");
	liq->flag.lock_uv=xml_get_attribute_boolean(liquid_tag,"lock_uv");

		// polygon

    tag=xml_findfirstchild("Poly",liquid_tag);
    if (tag!=-1) {
		liq->txt_idx=xml_get_attribute_int(tag,"txt");
		liq->lmap_txt_idx=xml_get_attribute_int_default(tag,"lmp",-1);
		xml_get_attribute_3_coord_int(tag,"v1",&liq->lft,&liq->y,&liq->top);
		xml_get_attribute_3_coord_int(tag,"v2",&liq->rgt,&liq->y,&liq->bot);
		liq->depth=xml_get_attribute_int(tag,"depth");

		xml_get_attribute_2_coord_float(tag,"uv_off",&liq->main_uv.offset.x,&liq->main_uv.offset.y);
		xml_get_attribute_2_coord_float(tag,"uv_size",&liq->main_uv.size.x,&liq->main_uv.size.y);
		if (liq->lmap_txt_idx!=-1) {
			xml_get_attribute_2_coord_float(tag,"uv_1_off",&liq->lmap_uv.offset.x,&liq->lmap_uv.offset.y);
			xml_get_attribute_2_coord_float(tag,"uv_1_size",&liq->lmap_uv.size.x,&liq->lmap_uv.size.y);
		}

		xml_get_attribute_color(tag,"rgb",&liq->col);
		liq->tint_alpha=xml_get_attribute_float(tag,"tint_alpha");
		xml_get_attribute_2_coord_float(tag,"shift",&liq->shift.x,&liq->shift.y);
		
		xml_get_attribute_text(tag,"camera",liq->camera,name_str_len);
		xml_get_attribute_text(tag,"ambient_sound_name",liq->ambient.sound_name,name_str_len);
	}

		// physics

    tag=xml_findfirstchild("Physic",liquid_tag);
    if (tag!=-1) {
		liq->speed_alter=xml_get_attribute_float(tag,"speed_alter");
	}

		// harm

    tag=xml_findfirstchild("Harm",liquid_tag);
    if (tag!=-1) {
		liq->harm.in_harm=xml_get_attribute_int(tag,"harm");
		liq->harm.drown_harm=xml_get_attribute_int(tag,"drown_harm");
		liq->harm.drown_tick=xml_get_attribute_int(tag,"drown_tick");
	}

		// tides

    tag=xml_findfirstchild("Tide",liquid_tag);
    if (tag!=-1) {
		liq->tide.rate=xml_get_attribute_int(tag,"rate");
		liq->tide.high=xml_get_attribute_int(tag,"high");
		liq->tide.uv_shift=xml_get_attribute_float(tag,"uv_shift");
	}

		// waves

    tag=xml_findfirstchild("Wave",liquid_tag);
    if (tag!=-1) {
		liq->wave.on=xml_get_attribute_boolean(tag,"on");
		liq->wave.dir_north_south=xml_get_attribute_boolean(tag,"dir_north_south");
		liq->wave.length=xml_get_attribute_int(tag,"length");
		liq->wave.high=xml_get_attribute_int(tag,"high");
		liq->wave.period_msec=xml_get_attribute_int(tag,"period_msec");
	}
	else {
		liq->wave.on=FALSE;
		liq->wave.dir_north_south=FALSE;
		liq->wave.length=2000;
		liq->wave.high=200;
		liq->wave.period_msec=3000;
	}

		// reflection

    tag=xml_findfirstchild("Reflect",liquid_tag);
    if (tag!=-1) {
		liq->reflect.texture_size=xml_get_attribute_int(tag,"texture_size");
		liq->reflect.x_refract_factor=xml_get_attribute_int(tag,"x_refract_factor");
		liq->reflect.z_refract_factor=xml_get_attribute_int(tag,"z_refract_factor");
		xml_get_attribute_color(tag,"no_hit_col",&liq->reflect.no_hit_col);
		liq->reflect.alpha=xml_get_attribute_float(tag,"alpha");
		if (liq->reflect.texture_size>=256) liq->reflect.texture_size=((int)log2(liq->reflect.texture_size))-8;
	}
	else {
		liq->reflect.texture_size=1;
		liq->reflect.x_refract_factor=20000;
		liq->reflect.z_refract_factor=20000;
		liq->reflect.no_hit_col.r=liq->reflect.no_hit_col.g=liq->reflect.no_hit_col.b=0.5f;
		liq->reflect.alpha=0.9f;
	}

		// overlay

    tag=xml_findfirstchild("Overlay",liquid_tag);
    if (tag!=-1) {
		liq->overlay.on=xml_get_attribute_boolean(tag,"on");
		liq->overlay.txt_idx=xml_get_attribute_int(tag,"txt_idx");
		liq->overlay.stamp_size=xml_get_attribute_int(tag,"stamp_size");
		xml_get_attribute_2_coord_float(tag,"shift",&liq->overlay.shift.x,&liq->overlay.shift.y);
	}
	else {
		liq->overlay.on=FALSE;
		liq->overlay.txt_idx=-1;
		liq->overlay.stamp_size=10000;
		liq->overlay.shift.x=liq->overlay.shift.y=0.0f;
	}
}

/* =======================================================

      Read Map Light Settings
      
======================================================= */

void read_single_light_setting(int light_tag,map_light_setting_type *lit_set)
{
	lit_set->type=xml_get_attribute_list(light_tag,"type",(char*)light_type_str);
	lit_set->light_map=xml_get_attribute_boolean(light_tag,"light_map");
	lit_set->direction=xml_get_attribute_list(light_tag,"direction",(char*)light_direction_str);
	lit_set->intensity=xml_get_attribute_int(light_tag,"intensity");
	lit_set->exponent=xml_get_attribute_float_default(light_tag,"exponent",1.0f);
	xml_get_attribute_color(light_tag,"rgb",&lit_set->col);
	xml_get_attribute_text(light_tag,"halo",lit_set->halo_name,name_str_len);
	lit_set->on=!xml_get_attribute_boolean(light_tag,"off");
}

/* =======================================================

      Read Map XML
      
======================================================= */

bool decode_map_xml(map_type *map,int map_head)
{
	int						k,j,idx,nmesh,mesh_idx,nliquid,liquid_idx,
							main_mesh_tag,mesh_tag,main_liquid_tag,liquid_tag,
							main_light_tag,light_tag,main_sound_tag,sound_tag,
							main_particle_tag,particle_tag,main_node_tag,node_tag,
							main_obj_tag,obj_tag,tag,cnt;
    map_light_type			*light;
    map_sound_type			*sound;
	map_particle_type		*particle;
    node_type				*node;
    spot_type				*spot;
	map_scenery_type		*scenery;

        // meshes

	map->mesh.nmesh=0;
	map->mesh.meshes=NULL;

	main_mesh_tag=xml_findfirstchild("Meshes",map_head);
	if (main_mesh_tag!=-1) {

		nmesh=xml_countchildren(main_mesh_tag);
		if (nmesh!=0) {
			
			mesh_tag=xml_findfirstchild("Mesh",main_mesh_tag);

			for (k=0;k!=nmesh;k++) {

					// create new mesh

				mesh_idx=map_mesh_add(map);
				if (mesh_idx==-1) return(FALSE);

					// read mesh

				if (!read_single_mesh_v3(map,mesh_idx,mesh_tag)) return(FALSE);
				
				mesh_tag=xml_findnextchild(mesh_tag);
			}
		}
	}

		// liquids

	map->liquid.nliquid=0;
	map->liquid.liquids=NULL;

	main_liquid_tag=xml_findfirstchild("Liquids",map_head);
	if (main_liquid_tag!=-1) {

		nliquid=xml_countchildren(main_liquid_tag);
		if (nliquid!=0) {
			
			liquid_tag=xml_findfirstchild("Liquid",main_liquid_tag);

			for (k=0;k!=nliquid;k++) {

					// create new liquid

				liquid_idx=map_liquid_add(map);
				if (liquid_idx==-1) return(FALSE);

					// read liquid

				read_single_liquid_v3(map,liquid_idx,liquid_tag);
				
				liquid_tag=xml_findnextchild(liquid_tag);
			}
		}
	}

		// scenery

	main_obj_tag=xml_findfirstchild("Sceneries",map_head);
	if (main_obj_tag!=-1) {
	
		cnt=xml_countchildren(main_obj_tag);
		obj_tag=xml_findfirstchild("Scenery",main_obj_tag);
		
		for (k=0;k!=cnt;k++) {
			if (map->nscenery>=max_map_scenery) break;
			
			scenery=&map->sceneries[map->nscenery];
			map->nscenery++;
			
			xml_get_attribute_3_coord_int(obj_tag,"c3",&scenery->pnt.x,&scenery->pnt.y,&scenery->pnt.z);
			
			xml_get_attribute_text(obj_tag,"model_name",scenery->model_name,name_str_len);
			xml_get_attribute_text(obj_tag,"animation_name",scenery->animation_name,name_str_len);
			xml_get_attribute_3_coord_float(obj_tag,"angle",&scenery->ang.x,&scenery->ang.y,&scenery->ang.z);
			scenery->contact_object_on=xml_get_attribute_boolean(obj_tag,"contact");
			scenery->contact_projectile_on=xml_get_attribute_boolean(obj_tag,"contact_projectile");
			scenery->contact_hit_box=xml_get_attribute_boolean(obj_tag,"contact_hit_box");
			scenery->face_forward=xml_get_attribute_boolean(obj_tag,"face_forward");
			scenery->shadow=xml_get_attribute_boolean(obj_tag,"shadow");
			
			xml_get_attribute_int_array(obj_tag,"frames",scenery->texture_frame,max_map_scenery_model_texture_frame);
			
			scenery->resize=xml_get_attribute_float_default(obj_tag,"resize",1.0f);
			
			obj_tag=xml_findnextchild(obj_tag);
		}
	}

		// lights
	
	main_light_tag=xml_findfirstchild("Lights",map_head);
	if (main_light_tag!=-1) {
	
		cnt=xml_countchildren(main_light_tag);
		light_tag=xml_findfirstchild("Light",main_light_tag);
		
		for (k=0;k!=cnt;k++) {
			if (map->nlight>=max_map_light) break;
			
			light=&map->lights[map->nlight];
			map->nlight++;

			xml_get_attribute_text(light_tag,"name",light->name,name_str_len);
			xml_get_attribute_3_coord_int(light_tag,"c3",&light->pnt.x,&light->pnt.y,&light->pnt.z);
			
			read_single_light_setting(light_tag,&light->setting);
		
			light_tag=xml_findnextchild(light_tag);
		}
	}
	
		// sounds
		
	main_sound_tag=xml_findfirstchild("Sounds",map_head);
	if (main_sound_tag!=-1) {
	
		cnt=xml_countchildren(main_sound_tag);
		sound_tag=xml_findfirstchild("Sound",main_sound_tag);
		
		for (k=0;k!=cnt;k++) {
			if (map->nsound>=max_map_sound) break;
			
			sound=&map->sounds[map->nsound];
			map->nsound++;
			
			xml_get_attribute_text(sound_tag,"name",sound->name,file_str_len);
			xml_get_attribute_3_coord_int(sound_tag,"c3",&sound->pnt.x,&sound->pnt.y,&sound->pnt.z);
			sound->pitch=xml_get_attribute_float(sound_tag,"pitch");
			sound->on=!xml_get_attribute_boolean(sound_tag,"off");
			
			sound_tag=xml_findnextchild(sound_tag);
		}
	}
	
		// particles
		
	main_particle_tag=xml_findfirstchild("Particles",map_head);
	if (main_particle_tag!=-1) {
	
		cnt=xml_countchildren(main_particle_tag);
		particle_tag=xml_findfirstchild("Particle",main_particle_tag);
		
		for (k=0;k!=cnt;k++) {
			if (map->nparticle>=max_map_particle) break;
			
			particle=&map->particles[map->nparticle];
			map->nparticle++;
			
			xml_get_attribute_text(particle_tag,"name",particle->name,file_str_len);
			xml_get_attribute_3_coord_int(particle_tag,"c3",&particle->pnt.x,&particle->pnt.y,&particle->pnt.z);
			particle->spawn_tick=xml_get_attribute_int(particle_tag,"spawn_tick");
			particle->slop_tick=xml_get_attribute_int(particle_tag,"slop_tick");
			particle->single_spawn=xml_get_attribute_boolean(particle_tag,"single_spawn");
			particle->on=!xml_get_attribute_boolean(particle_tag,"off");
			
			particle->light_setting.on=FALSE;
			
			light_tag=xml_findfirstchild("Light",particle_tag);
			if (light_tag!=-1) read_single_light_setting(light_tag,&particle->light_setting);
			
			particle_tag=xml_findnextchild(particle_tag);
		}
	}

		// nodes

	main_node_tag=xml_findfirstchild("Nodes",map_head);
	if (main_node_tag!=-1) {
	
		cnt=xml_countchildren(main_node_tag);
		node_tag=xml_findfirstchild("Node",main_node_tag);
		
		for (k=0;k!=cnt;k++) {
		
			idx=xml_get_attribute_int(node_tag,"id");
			node=&map->nodes[idx];
			if (idx>=map->nnode) map->nnode=idx+1;

			node->idx=idx;
			
			for (j=0;j!=max_node_link;j++) {
				node->link[j]=-1;
			}
			for (j=0;j!=max_node;j++) {
				node->path_hint[j]=-1;
			}
			
			xml_get_attribute_3_coord_int(node_tag,"c3",&node->pnt.x,&node->pnt.y,&node->pnt.z);

			xml_get_attribute_3_coord_float(node_tag,"angle",&node->ang.x,&node->ang.y,&node->ang.z);
			node->follow_camera=xml_get_attribute_boolean(node_tag,"follow_camera");
			node->alpha=xml_get_attribute_float_default(node_tag,"alpha",1.0f);

			xml_get_attribute_text(node_tag,"name",node->name,name_str_len);
			node->event_id=xml_get_attribute_int_default(node_tag,"event_id",0);
			
			tag=xml_findfirstchild("Link",node_tag);
			xml_get_attribute_short_array(tag,"node",node->link,max_node_link);
			
			tag=xml_findfirstchild("Hint",node_tag);
			xml_get_attribute_short_array(tag,"node",node->path_hint,max_node);

			node_tag=xml_findnextchild(node_tag);
		}
	}

		// spots

	main_obj_tag=xml_findfirstchild("Spots",map_head);
	if (main_obj_tag!=-1) {
	
		cnt=xml_countchildren(main_obj_tag);
		obj_tag=xml_findfirstchild("Spot",main_obj_tag);
		
		for (k=0;k!=cnt;k++) {
		
			idx=xml_get_attribute_int(obj_tag,"id");
			spot=&map->spots[idx];
			if (idx>=map->nspot) map->nspot=idx+1;
			
			xml_get_attribute_3_coord_int(obj_tag,"c3",&spot->pnt.x,&spot->pnt.y,&spot->pnt.z);
			
			xml_get_attribute_text(obj_tag,"name",spot->name,name_str_len);
			spot->type=xml_get_attribute_list(obj_tag,"type",(char*)spot_type_str);
			xml_get_attribute_text(obj_tag,"script",spot->script,name_str_len);
			xml_get_attribute_text(obj_tag,"display_model",spot->display_model,name_str_len);
			xml_get_attribute_text(obj_tag,"params",spot->params,param_str_len);
			spot->ang.y=xml_get_attribute_float(obj_tag,"angle");
			
			spot->skill=xml_get_attribute_list(obj_tag,"skill",(char*)skill_type_str);
			spot->spawn=xml_get_attribute_list(obj_tag,"spawn",(char*)spawn_type_str);
			spot->checkpoint.radius=xml_get_attribute_int(obj_tag,"checkpoint_radius");
			
			obj_tag=xml_findnextchild(obj_tag);
		}
	}

	return(TRUE);
}

/* =======================================================

      Read Map XML
      
======================================================= */

bool read_map_xml(map_type *map)
{
	int			version,map_head,tag;
	bool		map_ok;

	if (!xml_open_file(map->info.load_path)) return(FALSE);
    
    map_head=xml_findrootchild("Map");
    if (map_head==-1) {
		xml_close_file();
		return(FALSE);
    }
	
		// get the version
		// only allow loading of the current version
	
	tag=xml_findfirstchild("Creator",map_head);
	version=xml_get_attribute_int_default(tag,"version",1);
	if (version!=map_current_version) return(FALSE);

		// decode general non-versioned part

	decode_map_settings_xml(map,map_head);
	decode_map_groups_xml(map,map_head);
	decode_map_textures_xml(map,map_head);
	decode_map_movements_xml(map,map_head);
	decode_map_cinemas_xml(map,map_head);
	decode_map_camera_xml(map,map_head);

		// editor setup

	read_map_editor_views_xml(map,map_head);
	
		// decode

	map_ok=decode_map_xml(map,map_head);
	
	xml_close_file(); 
	  
    return(map_ok);
}


