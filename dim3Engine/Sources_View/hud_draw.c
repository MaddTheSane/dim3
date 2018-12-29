/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: HUD Routines

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
extern server_type			server;
extern view_type			view;
extern iface_type			iface;
extern setup_type			setup;
extern network_setup_type	net_setup;
extern render_info_type		render_info;

/* =======================================================

      Run HUD Item Fades
      
======================================================= */

bool hud_item_fade_run(iface_item_fade_type *fade,float *alpha)
{
	int			fade_tick;
	float		achg;

		// if fade not on then do nothing
	
	if (!fade->on) return(FALSE);
	
		// fade still on?
		
	fade_tick=game_time_get()-fade->start_tick;
	
	if (fade_tick>=(fade->fade_in_tick+fade->life_tick+fade->fade_out_tick)) {
		fade->on=FALSE;
		return(TRUE);
	}
	
	if (fade_tick>(fade->fade_in_tick+fade->life_tick)) {
		achg=1.0f-((float)(fade_tick-(fade->fade_in_tick+fade->life_tick))/(float)fade->fade_out_tick);
	}
	else {
		if (fade_tick>fade->fade_in_tick) {
			achg=1.0f;
		}
		else {
			achg=(float)fade_tick/(float)fade->fade_in_tick;
		}
	}
	
	*alpha=(*alpha)*achg;

	return(FALSE);
}

/* =======================================================

      Draw HUD Bitmaps
      
======================================================= */

void hud_bitmaps_draw(void)
{
	int							n,tick,r,wrap_count,
								px[4],py[4],sx,sy,rx,ry,
								wid,high,repeat_count;
	float						gx,gy,gx2,gy2,g_size,alpha;
	float						*vp;
	d3col						col,team_tint;
	iface_bitmap_type			*bitmap;
	obj_type					*obj;
	bitmap_type					*bitmap_data;
	
	if (iface.bitmap_list.nbitmap==0) return;
	
	obj=server.obj_list.objs[server.player_obj_idx];
	object_get_tint(obj,&team_tint);
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	
		// timing for flashes and fades

	tick=game_time_get();
	
		// draw bitmaps
		
	for (n=0;n!=iface.bitmap_list.nbitmap;n++) {
		bitmap=&iface.bitmap_list.bitmaps[n];
		if (!bitmap->show) continue;

			// flashing

		if (bitmap->flash) {
			if (((tick>>8)&0x1)==0x0) continue;		// quarter-second flash
		}
		
			// fading?
			
		alpha=bitmap->alpha;
		if (hud_item_fade_run(&bitmap->fade,&alpha)) {
			bitmap->show=FALSE;			// a fade has turned off bitmap
			continue;
		}
		
			// tint
			
		if (!bitmap->team_tint) {
			col.r=col.g=col.b=1.0f;
		}
		else {
			col.r=team_tint.r;
			col.g=team_tint.g;
			col.b=team_tint.b;
		}

            // get bitmap and position
			
		bitmap_data=view_images_get_bitmap(bitmap->image_idx);
            
		sx=bitmap->pnt.x;
		sy=bitmap->pnt.y;
		
		wid=bitmap->size.x;
		if (wid==-1) wid=bitmap_data->wid;

		high=bitmap->size.y;
		if (high==-1) high=bitmap_data->high;
		
			// setup images
			
		effect_image_animate_get_uv((tick-bitmap->show_tick),0,&bitmap->animate,&gx,&gy,&g_size);

            // flips
           
		if (!bitmap->flip_horz) {
			gx2=gx+g_size;
		}
		else {
			gx2=gx;
			gx+=g_size;
		}

		if (!bitmap->flip_vert) {
			gy2=gy+g_size;
		}
		else {
			gy2=gy;
			gy+=g_size;
		}
		
			// find the repeat count
		
		if (bitmap->repeat.on) {
			repeat_count=bitmap->repeat.count;
			if (repeat_count<1) continue;
		}
		else {
			repeat_count=1;
		}

			// scale and rotate bitmap

		rx=sx+wid;
		ry=sy+high;

		px[0]=px[3]=sx;
		px[1]=px[2]=rx;

		py[0]=py[1]=sy;
		py[2]=py[3]=ry;

		if (bitmap->rot!=0.0f) rotate_2D_polygon(4,px,py,((sx+rx)>>1),((sy+ry)>>1),bitmap->rot);

		rx=bitmap->repeat.x_add;
		ry=bitmap->repeat.y_add;
		
		wrap_count=0;

			// draw the bitmap

		for (r=0;r!=repeat_count;r++) {

				// quad

			view_bind_utility_vertex_object();
			vp=(float*)view_map_utility_vertex_object();

			*vp++=(float)px[0];
			*vp++=(float)py[0];
			*vp++=gx;
			*vp++=gy;

			*vp++=(float)px[3];
			*vp++=(float)py[3];
			*vp++=gx;
			*vp++=gy2;

			*vp++=(float)px[1];
			*vp++=(float)py[1];
			*vp++=gx2;
			*vp++=gy;

			*vp++=(float)px[2];
			*vp++=(float)py[2];
			*vp++=gx2;
			*vp++=gy2;
			
			view_unmap_utility_vertex_object();
		
				// column wrapping repeats
				
			if (bitmap->repeat.col!=0) {
				px[0]+=rx;
				px[1]+=rx;
				px[2]+=rx;
				px[3]+=rx;

				wrap_count++;
				if (wrap_count==bitmap->repeat.col) {
					wrap_count=0;
					
					px[0]=px[3]=sx;
					px[1]=px[2]=sx+wid;

					py[0]+=ry;
					py[1]+=ry;
					py[2]+=ry;
					py[3]+=ry;
				}
			}
			
			else {
			
					// row wrapping repeats
					
				if (bitmap->repeat.row!=0) {
					py[0]+=ry;
					py[1]+=ry;
					py[2]+=ry;
					py[3]+=ry;

					wrap_count++;
					if (wrap_count==bitmap->repeat.row) {
						wrap_count=0;
						
						py[0]=py[1]=sy;
						py[2]=py[3]=sy+high;
						
						px[0]+=rx;
						px[1]+=rx;
						px[2]+=rx;
						px[3]+=rx;
					}
				}
				
					// regular repeats
					
				else {
					px[0]+=rx;
					px[1]+=rx;
					px[2]+=rx;
					px[3]+=rx;
					py[0]+=ry;
					py[1]+=ry;
					py[2]+=ry;
					py[3]+=ry;
				}
			}

				// draw the quad

			gl_shader_draw_execute_simple_bitmap_set_texture(bitmap_data->gl_id);
			gl_shader_draw_execute_simple_bitmap_start(2,0,(2*sizeof(float)),((2+2)*sizeof(float)),&col,alpha);
			glDrawArrays(GL_TRIANGLE_STRIP,0,4);
			gl_shader_draw_execute_simple_bitmap_end();

			view_unbind_utility_vertex_object();
		}
	}
}

/* =======================================================

      HUD Text Specials
      
======================================================= */

void hud_texts_fps(char *data)
{
	if (view.fps.total==0.0f) {
		strcpy(data,"--.--");
	}
	else {
		if ((!setup.ignore_fps_lock) && (view.fps.total>render_info.monitor_refresh_rate)) {
			sprintf(data,"%d.00",render_info.monitor_refresh_rate);
		}
		else {
			sprintf(data,"%.2f",view.fps.total);
		}
	}
}

void hud_texts_score(char *data)
{
	obj_type		*obj;
	
	obj=server.obj_list.objs[server.player_obj_idx];
	sprintf(data,"%d",obj->score.score);
}

void hud_texts_place(char *data)
{
	obj_type		*obj;
	
	obj=server.obj_list.objs[server.player_obj_idx];
	sprintf(data,"%d",obj->score.place);
}

void hud_texts_spread(char *data)
{
	int				n,spread,find_place;
	bool			hit;
	obj_type		*obj,*chk_obj;
	
	obj=server.obj_list.objs[server.player_obj_idx];
	
		// if in first place, compare with second place
		// else compare with first place
		
	if (obj->score.place==1) {
		find_place=2;
	}
	else {
		find_place=1;
	}
	
	hit=FALSE;

	for (n=0;n!=max_obj_list;n++) {
		chk_obj=server.obj_list.objs[n];
		if (chk_obj==NULL) continue;

		if ((chk_obj->type!=object_type_player) && (chk_obj->type!=object_type_remote_player) && (chk_obj->type!=object_type_bot_multiplayer)) continue;

		if (chk_obj->score.place==find_place) {
			hit=TRUE;
			break;
		}
	}
	
		// create the spread
		
	spread=0;
	if (hit) spread=obj->score.score-chk_obj->score.score;
	
	sprintf(data,"%d",spread);
}

void hud_texts_map_timer(char *data)
{
	int				tick,min,sec,millisec;
	
	tick=game_time_get()-server.time.map_start_tick;
	
	min=tick/(1000*60);
	tick=tick%(1000*60);
	
	sec=tick/1000;
	millisec=(tick%1000)/100;

	sprintf(data,"%d:%.2d.%d",min,sec,millisec);
}

/* =======================================================

      Draw HUD Text
      
======================================================= */

void hud_texts_draw_return(int x,int y,int ysz,char *data,int just,d3col *col,float alpha)
{
	char			*c,*c2;
	
	c=data;
	
	while (*c!=0x0) {
		c2=strstr(c,"{r}");
		if (c2!=NULL) *c2=0x0;
		
		gl_text_draw(x,y,c,just,FALSE,col,alpha);
		
		if (c2==NULL) return;
		
		*c2='{';
		
		c=c2+3;
		y+=ysz;
	}
}

void hud_texts_draw(void)
{
	int				n,high,cur_size;
	float			alpha;
	bool			cur_monospaced;
	iface_text_type	*text;

	cur_size=-1;
	cur_monospaced=FALSE;

	for (n=0;n!=iface.text_list.ntext;n++) {

		text=&iface.text_list.texts[n];
		if (!text->show) continue;

			// time for a new text size?

		if ((text->size!=cur_size) || (text->monospaced!=cur_monospaced)) {
			cur_size=text->size;
			cur_monospaced=text->monospaced;

			gl_text_start(font_hud_index,cur_size,cur_monospaced);
			high=gl_text_get_char_height(cur_size);
		}

			// fading?
	
		alpha=text->alpha;
		if (hud_item_fade_run(&text->fade,&alpha)) {
			text->show=FALSE;			// a fade has turned off bitmap
			continue;
		}

			// any special replacements

		switch (text->special) {
		
			case text_special_fps:
				hud_texts_fps(text->data);
				break;
				
			case text_special_score:
				hud_texts_score(text->data);
				break;
				
			case text_special_place:
				hud_texts_place(text->data);
				break;
				
			case text_special_spread:
				hud_texts_spread(text->data);
				break;
				
			case text_special_map_timer:
				hud_texts_map_timer(text->data);
				break;

		}

			// draw text
			
		if (text->has_return) {
			hud_texts_draw_return(text->pnt.x,text->pnt.y,high,text->data,text->just,&text->color,alpha);
		}
		else {
			gl_text_draw(text->pnt.x,text->pnt.y,text->data,text->just,FALSE,&text->color,alpha);
		}
	}
	
	gl_text_end();		// OK to call text_end without a text_start
}

/* =======================================================

      Draw HUD Bars
      
======================================================= */

float hud_bars_draw_get_value(iface_bar_type *bar)
{
	int			tick;
	
		// no countdown is just value
		
	if (bar->countdown.msec<=0) return(bar->value);
	
		// run countdown
		
	tick=game_time_get()-bar->countdown.start_tick;
	if (tick>bar->countdown.msec) return(0.0f);
	
	return(((float)tick)/((float)bar->countdown.msec));
}

void hud_bars_draw_horizontal_vertical(iface_bar_type *bar)
{
	int					lx,rx,ty,by,wid,high;
	d3col				fill_end_color;
	
		// find size
		
	wid=bar->size.x;
	high=bar->size.y;
	
	if (bar->type==bar_type_horizontal) {
		wid=(int)(((float)wid)*hud_bars_draw_get_value(bar));
	}
	else {
		high=(int)(((float)high)*hud_bars_draw_get_value(bar));
	}
	
		// find the color
		
	fill_end_color.r=bar->fill_start_color.r+((bar->fill_end_color.r-bar->fill_start_color.r)*bar->value);
	fill_end_color.g=bar->fill_start_color.g+((bar->fill_end_color.g-bar->fill_start_color.g)*bar->value);
	fill_end_color.b=bar->fill_start_color.b+((bar->fill_end_color.b-bar->fill_start_color.b)*bar->value);

		// background

	if (bar->background) {
	
		if (bar->type==bar_type_horizontal) {
			lx=bar->pnt.x+wid;
			rx=bar->pnt.x+bar->size.x;
			ty=bar->pnt.y;
			by=bar->pnt.y+bar->size.y;
		}
		else {
			lx=bar->pnt.x;
			rx=bar->pnt.x+bar->size.x;
			ty=bar->pnt.y+high;
			by=bar->pnt.y+bar->size.y;
		}

		if ((lx!=rx) && (ty!=by)) view_primitive_2D_color_quad(&bar->background_color,bar->background_alpha,lx,rx,ty,by);
	}

		// draw bar

	lx=bar->pnt.x;
	rx=lx+wid;
	by=bar->pnt.y+bar->size.y;
	ty=by-high;
		
	if ((lx!=rx) && (ty!=by)) {
		if (bar->type==bar_type_horizontal) {
			view_primitive_2D_color_poly(lx,by,&bar->fill_start_color,lx,ty,&bar->fill_start_color,rx,ty,&fill_end_color,rx,by,&fill_end_color,bar->fill_alpha);
		}
		else {
			view_primitive_2D_color_poly(lx,ty,&fill_end_color,rx,ty,&fill_end_color,rx,by,&bar->fill_start_color,lx,by,&bar->fill_start_color,bar->fill_alpha);
		}
	}
	
		// draw outline
		
	if (bar->outline) {
	
		lx=bar->pnt.x;
		rx=lx+bar->size.x;
		ty=bar->pnt.y;
		by=ty+bar->size.y;
		
		glLineWidth((float)view.screen.x_sz/(float)iface.scale_x);
		view_primitive_2D_line_quad(&bar->outline_color,bar->outline_alpha,lx,rx,ty,by);
		glLineWidth(1.0f);
	}
}

void hud_bars_draw_pie(iface_bar_type *bar)
{
	int				n,value_idx;
	unsigned char	*cp;
	unsigned char	sf_uc_r,sf_uc_g,sf_uc_b,f_uc_alpha,
					ef_uc_r,ef_uc_g,ef_uc_b,
					b_uc_r,b_uc_g,b_uc_b,b_uc_alpha;
	float			fx,fy,r_add,rad,radius;
	float			*vp;
	
		// get center and radius
		
	radius=(float)((bar->size.x+bar->size.y)>>2);
	fx=((float)bar->pnt.x)+radius;
	fy=((float)bar->pnt.y)+radius;
	
		// colors
		
	sf_uc_r=(unsigned char)(bar->fill_start_color.r*255.0f);
	sf_uc_g=(unsigned char)(bar->fill_start_color.g*255.0f);
	sf_uc_b=(unsigned char)(bar->fill_start_color.b*255.0f);
	f_uc_alpha=(unsigned char)(bar->fill_alpha*255.0f);
	
	ef_uc_r=(unsigned char)(bar->fill_end_color.r*255.0f);
	ef_uc_g=(unsigned char)(bar->fill_end_color.g*255.0f);
	ef_uc_b=(unsigned char)(bar->fill_end_color.b*255.0f);
	
	b_uc_r=(unsigned char)(bar->background_color.r*255.0f);
	b_uc_g=(unsigned char)(bar->background_color.g*255.0f);
	b_uc_b=(unsigned char)(bar->background_color.b*255.0f);
	b_uc_alpha=(unsigned char)(bar->background_alpha*255.0f);
	
		// value position
		
	value_idx=(int)(((float)72)*hud_bars_draw_get_value(bar));
	
		// draw the pie
		
	r_add=(TRIG_PI*2.0f)/72.0f;

	view_bind_utility_vertex_object();

	vp=(float*)view_map_utility_vertex_object();
	cp=(unsigned char*)(vp+(6*72));
	
	for (n=0;n!=72;n++) {

			// the vertexes
			
		rad=r_add*((float)n);		
		*vp++=fx+(radius*sinf(rad));
		*vp++=fy-(radius*cosf(rad));
	
		if (n!=71) {
			rad+=r_add;
		}
		else {
			rad=0;
		}
		
		*vp++=fx+(radius*sinf(rad));
		*vp++=fy-(radius*cosf(rad));
		
		*vp++=fx;
		*vp++=fy;
		
			// the colors
			
		if (n<value_idx) {
			*cp++=sf_uc_r;
			*cp++=sf_uc_g;
			*cp++=sf_uc_b;
			*cp++=f_uc_alpha;
			*cp++=sf_uc_r;
			*cp++=sf_uc_g;
			*cp++=sf_uc_b;
			*cp++=f_uc_alpha;
			*cp++=ef_uc_r;
			*cp++=ef_uc_g;
			*cp++=ef_uc_b;
			*cp++=f_uc_alpha;
		}
		else {
			*cp++=b_uc_r;
			*cp++=b_uc_g;
			*cp++=b_uc_b;
			*cp++=b_uc_alpha;
			*cp++=b_uc_r;
			*cp++=b_uc_g;
			*cp++=b_uc_b;
			*cp++=b_uc_alpha;
			*cp++=b_uc_r;
			*cp++=b_uc_g;
			*cp++=b_uc_b;
			*cp++=b_uc_alpha;
		}
	}

	view_unmap_utility_vertex_object();

		// draw

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	glDisable(GL_DEPTH_TEST);

	gl_shader_draw_execute_simple_gradient_start(2,0,((6*72)*sizeof(float)));
	glDrawArrays(GL_TRIANGLES,0,(72*3));
	gl_shader_draw_execute_simple_gradient_end();

	glDisable(GL_BLEND);

	view_unbind_utility_vertex_object();
}

void hud_bars_draw(void)
{
	int					n;
	iface_bar_type		*bar;
	
		// draw bars
		
	for (n=0;n!=iface.bar_list.nbar;n++) {

			// is bar hidden?
			
		bar=&iface.bar_list.bars[n];
		if (!bar->show) continue;
		
			// draw bar

		switch (bar->type) {
		
			case bar_type_horizontal:
			case bar_type_vertical:
				hud_bars_draw_horizontal_vertical(bar);
				break;
				
			case bar_type_pie:
				hud_bars_draw_pie(bar);
				break;
		
		}
	}
}

/* =======================================================

      HUD Checkpoint Show/Hide
      
======================================================= */

void hud_checkpoint_show(bool show)
{
	int					n;
	iface_bitmap_type	*bitmap;
	iface_text_type		*text;

	bitmap=iface.bitmap_list.bitmaps;

	for (n=0;n!=iface.bitmap_list.nbitmap;n++) {
		if (bitmap->checkpoint) bitmap->show=show;
		bitmap++;
	}

	text=iface.text_list.texts;

	for (n=0;n!=iface.text_list.ntext;n++) {
		if (text->checkpoint) text->show=show;
		text++;
	}
}

/* =======================================================

      HUD Sounds
      
======================================================= */

void hud_click(void)
{
	int			buffer_idx;

	if (iface.click_sound[0]==0x0) return;
	
	buffer_idx=al_find_buffer(iface.click_sound);
	if (buffer_idx!=-1) al_play_source(buffer_idx,NULL,1,FALSE,FALSE,TRUE,FALSE);
}

/* =======================================================

      Draw HUD
      
======================================================= */

void hud_draw(void)
{
	if (setup.no_hud) return;

		// set up view
		
	gl_2D_view_interface();
	
	glDisable(GL_DEPTH_TEST);
	
		// draw the bitmaps, bars, and text
		
	hud_bars_draw();
	hud_bitmaps_draw();
	hud_texts_draw();
}

