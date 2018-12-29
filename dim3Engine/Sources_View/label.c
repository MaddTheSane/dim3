/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Object Labels

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
extern camera_type			camera;
extern server_type			server;
extern iface_type			iface;
extern js_type				js;
extern setup_type			setup;
extern network_setup_type	net_setup;

extern int					remote_slow_image_idx,remote_talk_image_idx;

/* =======================================================

      Label Setup
      
======================================================= */

void label_draw_setup_single(obj_type *obj,int bone_idx,obj_label_draw *label_draw)
{
	bool					hit;
	d3pnt					ept;
	ray_trace_contact_type	contact;

		// get bone position

	label_draw->pnt.x=obj->pnt.x;
	label_draw->pnt.y=obj->pnt.y;
	label_draw->pnt.z=obj->pnt.z;

	if (!model_get_generic_bone_position(&obj->draw.setup,bone_idx,&label_draw->pnt)) return;

		// skip if behind z

	if (!gl_project_in_view_z(&label_draw->pnt)) return;

		// ignore if obscured by ray trace

	contact.obj.on=TRUE;
	contact.proj.on=FALSE;
	contact.origin=poly_ray_trace_origin_object;
	
	ept.x=view.render->camera.pnt.x;
	ept.y=view.render->camera.pnt.y;
	ept.z=view.render->camera.pnt.z;

	contact.obj.ignore_idx=obj->idx;
	
	hit=ray_trace_map_by_point(&label_draw->pnt,&ept,&contact);
	
	if (map.camera.camera_mode==cv_fpp) {
		if (!hit) return;
		if (contact.obj.idx!=server.player_obj_idx) return;
	}
	else {
		if (hit) return;
	}

		// project point in 2D

	gl_project_point(&label_draw->pnt);
	label_draw->pnt.y=view.screen.y_sz-label_draw->pnt.y;	

	label_draw->on=TRUE;
}

void label_draw_setup(void)
{
	int						n;
	obj_type				*obj;
	model_type				*mdl;

		// clear all labels
	
	for (n=0;n!=view.render->draw_list.count;n++) {
		if (view.render->draw_list.items[n].type!=view_render_type_object) continue;

		obj=server.obj_list.objs[view.render->draw_list.items[n].idx];
		obj->label.text.draw.on=FALSE;
		obj->label.bitmap.draw.on=FALSE;
		obj->label.bar.draw.on=FALSE;
		obj->label.remote_name.draw.on=FALSE;
	}

		// remove labels behind z or off-screen
		// ignore console as it won't matter for projection
		
	for (n=0;n!=view.render->draw_list.count;n++) {
		if (view.render->draw_list.items[n].type!=view_render_type_object) continue;

			// skip objects hidden or with no models

		obj=server.obj_list.objs[view.render->draw_list.items[n].idx];
		if (obj->hidden) continue;
		if ((obj->draw.model_idx==-1) || (!obj->draw.on)) continue;
		
		mdl=server.model_list.models[obj->draw.model_idx];
	
			// setup labels

		if (obj->label.text.str[0]!=0x0) label_draw_setup_single(obj,mdl->label.text.bone_idx,&obj->label.text.draw);
		if (obj->label.bitmap.idx!=-1) label_draw_setup_single(obj,mdl->label.bitmap.bone_idx,&obj->label.bitmap.draw);
		if (obj->label.bar.value>=0.0f) label_draw_setup_single(obj,mdl->label.bar.bone_idx,&obj->label.bar.draw);
		if ((net_setup.mode!=net_mode_none) && (setup.network.show_names) && (obj->type==object_type_remote_player)) label_draw_setup_single(obj,mdl->bone_connect.name_bone_idx,&obj->label.remote_name.draw);
	}
}

/* =======================================================

      Remote Icons
      
======================================================= */

void label_draw_render_remote_icon(obj_type *obj,model_type *mdl,int image_idx)
{
	int				sz,lft,rgt,top,bot;
	d3col			col;
	bitmap_type		*bitmap;

	sz=mdl->label.bitmap.size;

	lft=obj->label.remote_name.draw.pnt.x-(sz>>1);
	rgt=lft+sz;
	top=obj->label.remote_name.draw.pnt.y-(sz>>1);
	bot=top+sz;

	col.r=col.g=col.b=1.0f;

	bitmap=view_images_get_bitmap(image_idx);
	view_primitive_2D_texture_quad(bitmap->gl_id,&col,1.0f,lft,rgt,top,bot,0.0f,1.0f,0.0f,1.0f,TRUE);
}

/* =======================================================

      Label Rendering
      
======================================================= */

void label_draw_render(void)
{
	int					n,sz,wid,high,
						x,y,lft,rgt,top,bot;
	d3col				col;
	obj_type			*obj;
	model_type			*mdl;

	glDisable(GL_DEPTH_TEST);
	
	for (n=0;n!=view.render->draw_list.count;n++) {
		if (view.render->draw_list.items[n].type!=view_render_type_object) continue;

		obj=server.obj_list.objs[view.render->draw_list.items[n].idx];

		if (obj->draw.model_idx==-1) continue;
		mdl=server.model_list.models[obj->draw.model_idx];

			// text

		if (obj->label.text.draw.on) {
			x=obj->label.text.draw.pnt.x;
			y=obj->label.text.draw.pnt.y;

			gl_text_start(font_hud_index,mdl->label.text.size,FALSE);
			gl_text_draw(x,y,obj->label.text.str,tx_center,FALSE,&mdl->label.text.col,1.0f);
			gl_text_end();
		}

			// bitmap

		if (obj->label.bitmap.draw.on) {

				// box size

			sz=mdl->label.bitmap.size;

			lft=obj->label.bitmap.draw.pnt.x-(sz>>1);
			rgt=lft+sz;
			top=obj->label.bitmap.draw.pnt.y-(sz>>1);
			bot=top+sz;

				// draw image

			col.r=col.g=col.b=1.0f;
			view_primitive_2D_texture_quad(view_images_get_gl_id(iface.label_list.labels[obj->label.bitmap.idx].image_idx),&col,1.0f,lft,rgt,top,bot,0.0f,1.0f,0.0f,1.0f,TRUE);
		}

			// bar

		if (obj->label.bar.draw.on) {
			
				// box size

			wid=mdl->label.bar.wid;
			high=mdl->label.bar.high;

			lft=obj->label.bar.draw.pnt.x-(wid>>1);
			rgt=lft+wid;
			top=obj->label.bar.draw.pnt.y-(high>>1);
			bot=top+high;

				// health position

			x=lft+(int)((float)(rgt-lft)*obj->label.bar.value);
			if (x>rgt) x=rgt;
			if (x<lft) x=lft;

				// draw

			if (mdl->label.bar.background_on) view_primitive_2D_color_quad(&mdl->label.bar.background_col,1.0f,lft,rgt,top,bot);
			if (mdl->label.bar.border_on) view_primitive_2D_line_quad(&mdl->label.bar.border_col,1.0f,lft,rgt,top,bot);

			view_primitive_2D_color_quad(&mdl->label.bar.bar_col,1.0f,lft,x,top,bot);
		}

			// remote names

		if (obj->label.remote_name.draw.on) {
			x=obj->label.remote_name.draw.pnt.x;
			y=obj->label.remote_name.draw.pnt.y;

				// the name

			object_get_tint(obj,&col);

			gl_text_start(font_hud_index,mdl->label.text.size,FALSE);
			gl_text_draw(x,y,obj->name,tx_center,FALSE,&col,1.0f);
			gl_text_end();

				// slow and talking status
		
			if (remote_timed_out(obj)) {
				label_draw_render_remote_icon(obj,mdl,remote_slow_image_idx);
			}
			else {
				if ((obj->remote.talking)) {
					label_draw_render_remote_icon(obj,mdl,remote_talk_image_idx);
				}
			}
		}

	}
}
