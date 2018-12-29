/****************************** File *********************************

Module: dim3 Common
Author: Brian Barnes
 Usage: Texture Editing

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

int								texture_edit_scroll_pos,
								texture_edit_frame_click_idx;

extern app_state_type			state;
extern map_type					map;
extern model_type				model;
extern list_palette_type		file_palette;
extern list_palette_type		map_palette,model_palette;

extern file_path_setup_type		file_path_setup;

/* =======================================================

      Switching View
      
======================================================= */

void texture_edit_switch(int texture_idx)
{
		// clicking again leaves

	if (state.mode==app_mode_map) {
		if ((state.map.texture_edit_idx!=-1) && (state.map.texture_edit_idx==texture_idx)) {
			state.map.texture_edit_idx=-1;
		}
		else {
			state.map.texture_edit_idx=texture_idx;
		}
		map_palette_reset();
		list_palette_set_level(&map_palette,0);
	}
	else {
		if ((state.model.texture_edit_idx!=-1) && (state.model.texture_edit_idx==texture_idx)) {
			state.model.texture_edit_idx=-1;
		}
		else {
			state.model.texture_edit_idx=texture_idx;
		}
		list_palette_set_level(&model_palette,0);
	}
	
	texture_edit_frame_click_idx=-1;

	main_wind_draw();
}

/* =======================================================

      View Texture Edit Draw
      
======================================================= */

void texture_edit_get_box(d3rect *box)
{
	os_get_window_box(box);
	
	box->lx+=list_palette_width(&file_palette);
	box->ty+=tool_button_size;

	if (state.mode==app_mode_map) {
		box->rx-=(list_palette_width(&map_palette)+1);
		box->by-=map_texture_palette_pixel_size();
	}
	else {
		box->rx-=(list_palette_width(&model_palette)+1);
		box->by-=model_texture_palette_pixel_size();
	}
}

void texture_edit_draw_bitmap(d3rect *box,char *name,unsigned long gl_id)
{
	float				vertexes[8],uvs[8]={0.0f,0.0f,1.0f,0.0f,1.0f,1.0f,0.0f,1.0f};

		// the draw vertexes

	vertexes[0]=vertexes[6]=(float)box->lx;
	vertexes[2]=vertexes[4]=(float)box->rx;
	vertexes[1]=vertexes[3]=(float)box->ty;
	vertexes[5]=vertexes[7]=(float)box->by;
	
	glVertexPointer(2,GL_FLOAT,0,vertexes);

		// the bitmap

	if (gl_id!=-1) {

		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D,gl_id);

		glColor4f(1.0f,1.0f,1.0f,1.0f);

  		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);

		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glTexCoordPointer(2,GL_FLOAT,0,uvs);

		glDrawArrays(GL_QUADS,0,4);

		glDisableClientState(GL_TEXTURE_COORD_ARRAY);

		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);

		glDisable(GL_TEXTURE_2D);
	}

		// the box

	glColor4f(0.0f,0.0f,0.0f,1.0f);
	glDrawArrays(GL_LINE_LOOP,0,4);

		// the name
		
	text_draw_center(((box->lx+box->rx)>>1),(box->by+15),12.0f,NULL,name);
}

void texture_edit_draw_button(d3rect *box,int title_offset,char *title,float font_size,bool has_trig,int sel_idx)
{
	float			col_rg,vertexes[8],colors[16];
	
		// the button box

	col_rg=0.7f;
	if (texture_edit_frame_click_idx==sel_idx) col_rg=0.5f;

	vertexes[0]=vertexes[6]=(float)box->lx;
	vertexes[2]=vertexes[4]=(float)box->rx;
	vertexes[1]=vertexes[3]=(float)box->ty;
	vertexes[5]=vertexes[7]=(float)box->by;

	colors[0]=colors[1]=colors[4]=colors[5]=col_rg;
	colors[2]=colors[6]=1.0f;
	colors[8]=colors[9]=colors[12]=colors[13]=col_rg-0.2f;
	colors[10]=colors[14]=0.8f;
	colors[3]=colors[7]=colors[11]=colors[15]=1.0f;

	glVertexPointer(2,GL_FLOAT,0,vertexes);

	glEnableClientState(GL_COLOR_ARRAY);
	glColorPointer(4,GL_FLOAT,0,colors);

	glDrawArrays(GL_QUADS,0,4);

	glDisableClientState(GL_COLOR_ARRAY);

		// the outline

	glColor4f(0.0f,0.0f,0.0f,1.0f);
	glDrawArrays(GL_LINE_LOOP,0,4);

		// the trig

	if (has_trig) {
		vertexes[0]=(float)(box->rx-20);
		vertexes[1]=(float)(box->ty+5);
		vertexes[2]=(float)(box->rx-5);
		vertexes[3]=(float)(box->by-5);
		vertexes[4]=(float)(box->rx-35);
		vertexes[5]=(float)(box->by-5);
		
		glVertexPointer(2,GL_FLOAT,0,vertexes);

		glColor4f(0.2f,0.2f,1.0f,1.0f);
		glDrawArrays(GL_TRIANGLES,0,3);
	
		glColor4f(0.0f,0.0f,0.0f,1.0f);
		glDrawArrays(GL_LINE_LOOP,0,3);
	}

		// the button text

	text_draw((box->lx+title_offset),(box->by-5),font_size,NULL,title);
}

void texture_edit_draw(void)
{
	int					n,ty,by,wid,high,frame_count;
	float				vertexes[8];
	char				str[256];
	d3rect				wbox,box,tbox;
	texture_type		*texture;

		// get texture and frame count
		
	if (state.mode==app_mode_map) {
		texture=&map.textures[state.map.texture_edit_idx];
		frame_count=map_count_texture_frames(&map,state.map.texture_edit_idx);
	}
	else {
		texture=&model.textures[state.model.texture_edit_idx];
		frame_count=model_count_texture_frames(&model,state.model.texture_edit_idx);
	}

		// setup drawing

	os_get_window_box(&wbox);
	texture_edit_get_box(&box);

	wid=box.rx-box.lx;
	high=box.by-box.ty;

	glEnable(GL_SCISSOR_TEST);
	glScissor(box.lx,(wbox.by-box.by),((box.rx-box.lx)+1),(box.by-box.ty));

	glViewport(box.lx,(wbox.by-box.by),((box.rx-box.lx)+1),(box.by-box.ty));
		
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho((GLdouble)0,(GLdouble)wid,(GLdouble)high,(GLdouble)0,-1.0,1.0);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

		// draw setup

	glDisable(GL_DEPTH_TEST);

		// the textures

	for (n=0;n!=max_texture_frame;n++) {
		ty=(texture_edit_item_high*n)-texture_edit_scroll_pos;
		by=ty+texture_edit_item_high;
		
			// the background
			
		if (n>frame_count) {
			glColor4f(0.5f,0.5f,0.5f,1.0f);
		}
		else {
			if (texture_edit_frame_click_idx==n) {
				glColor4f(0.85f,0.85f,0.85f,1.0f);
			}
			else {
				glColor4f(0.95f,0.95f,0.95f,1.0f);
			}
		}
	
		vertexes[0]=vertexes[6]=0.0f;
		vertexes[1]=vertexes[3]=(float)ty;
		vertexes[2]=vertexes[4]=(float)wid;
		vertexes[5]=vertexes[7]=(float)by;

		glVertexPointer(2,GL_FLOAT,0,vertexes);
		glDrawArrays(GL_QUADS,0,4);

			// the separator

		glColor4f(0.0f,0.0f,0.0f,1.0f);

		vertexes[0]=0.0f;
		vertexes[1]=(float)ty;
		vertexes[2]=(float)wid;
		vertexes[3]=(float)ty;

		glVertexPointer(2,GL_FLOAT,0,vertexes);
		glDrawArrays(GL_LINES,0,2);

			// the frame

		sprintf(str,"Frame %d",n);
		text_draw(5,(ty+20),15.0f,NULL,str);

			// the textures

		tbox.lx=25;
		tbox.rx=tbox.lx+100;
		tbox.ty=ty+22;
		tbox.by=tbox.ty+100;
		texture_edit_draw_bitmap(&tbox,"Bitmap",texture->frames[n].bitmap.gl_id);

		tbox.lx+=110;
		tbox.rx+=110;
		texture_edit_draw_bitmap(&tbox,"Bump",texture->frames[n].bumpmap.gl_id);

		tbox.lx+=110;
		tbox.rx+=110;
		texture_edit_draw_bitmap(&tbox,"Specular",texture->frames[n].specularmap.gl_id);

		tbox.lx+=110;
		tbox.rx+=110;
		texture_edit_draw_bitmap(&tbox,"Glow",texture->frames[n].glowmap.gl_id);

			// the info

		str[0]=0x0;
	
		if (texture->frames[n].name[0]!=0x0) {
			sprintf(str,"Name: %s",texture->frames[n].name);
			text_draw(470,(ty+35),15.0f,NULL,str);

			sprintf(str,"Size: %dx%d",texture->frames[n].bitmap.wid,texture->frames[n].bitmap.high);
			text_draw(470,(ty+55),15.0f,NULL,str);

			if (texture->frames[n].bitmap.opaque) {
				strcpy(str,"Opaque");
			}
			else {
				strcpy(str,"Transparent");
			}
			text_draw(470,(ty+75),15.0f,NULL,str);

			sprintf(str,"Wait: %d",texture->animate.wait[n]);
			text_draw(470,(ty+95),15.0f,NULL,str);
		}
		else {
			text_draw(470,(ty+35),15.0f,NULL,"Name: (empty)");
		}
		
			// delete button
		
		if (frame_count>0) {
			tbox.lx=470;
			tbox.rx=600;
			tbox.ty=ty+100;
			tbox.by=tbox.ty+25;

			if ((frame_count==1) && (n==0)) {
				texture_edit_draw_button(&tbox,15,"Delete Texture",15.0f,FALSE,texture_edit_frame_click_delete_idx);
			}
			else {
				if (n==(frame_count-1)) {
					texture_edit_draw_button(&tbox,20,"Delete Frame",15.0f,FALSE,texture_edit_frame_click_delete_idx);
				}
			}
		}
	}

		// the return button

	tbox.lx=wid-120;
	tbox.rx=tbox.lx+120;
	tbox.ty=high-40;
	tbox.by=tbox.ty+40;
	
	texture_edit_draw_button(&tbox,5,"Return",25.0f,TRUE,texture_edit_frame_click_return_idx);
}

/* =======================================================

      Cursors and Scrolling
      
======================================================= */

bool texture_edit_cursor(void)
{
	if (os_key_space_down()) {
		os_set_hand_cursor();
        return(TRUE);
    }

	return(FALSE);
}

void texture_edit_scroll(int y_move)
{
	int			high;
	d3rect		box;

	texture_edit_scroll_pos-=y_move;

	if (texture_edit_scroll_pos<0) texture_edit_scroll_pos=0;
	
	texture_edit_get_box(&box);
	high=(texture_edit_item_high*max_texture_frame)-(box.by-box.ty);
	if (texture_edit_scroll_pos>high) texture_edit_scroll_pos=high;

	main_wind_draw();
}

void texture_edit_scroll_wheel(int delta)
{
	texture_edit_scroll(delta*texture_edit_scroll_wheel_move);
}

/* =======================================================

      Load New Textures
      
======================================================= */

bool texture_edit_click_bitmap_open(char *bitmap_name)
{
    char				err_str[256],sub_path[1024],path[1024];
	
		// get bitmap

	if (state.mode==app_mode_map) {
		strcpy(sub_path,"Bitmaps/Textures");
	}
	else {
		sprintf(sub_path,"Models/%s/Textures",model.name);
	}

	if (!dialog_file_open_run("Open a Bitmap",sub_path,"png",NULL,bitmap_name)) return(FALSE);
	
		// check bitmap

	file_paths_data(&file_path_setup,path,sub_path,bitmap_name,"png");
	if (!bitmap_check(path,err_str)) {
		os_dialog_alert("Texture Error",err_str);
		return(FALSE);
	}

	return(TRUE);
}

/* =======================================================

      View Texture Scrolling Click
      
======================================================= */

void texture_edit_click_scroll(void)
{
	int				org_y;
	d3pnt			pt;
	d3rect			box;

	org_y=-1;
	texture_edit_get_box(&box);

	main_wind_draw();

	while (!os_track_mouse_location(&pt,&box)) {

		if (org_y==-1) org_y=pt.y;

		if (pt.y!=org_y) {
			texture_edit_scroll(pt.y-org_y);
			main_wind_draw();
			org_y=pt.y;
		}
		
		usleep(10000);
	}
}

/* =======================================================

      View Texture Edit Click
      
======================================================= */

bool texture_edit_click(d3pnt *pnt,bool double_click)
{
	int				wid,high,frame_idx,frame_count;
    char			bitmap_name[file_str_len];
	d3pnt			pt;
	d3rect			box,tbox;
	texture_type	*texture;

		// check for scrolling

	if (os_key_space_down()) {
		texture_edit_click_scroll();
		return(TRUE);
	}
	
		// regular clicks

	if (state.mode==app_mode_map) {
		texture=&map.textures[state.map.texture_edit_idx];
		frame_count=map_count_texture_frames(&map,state.map.texture_edit_idx);
	}
	else {
		texture=&model.textures[state.model.texture_edit_idx];
		frame_count=model_count_texture_frames(&model,state.model.texture_edit_idx);
	}

	texture_edit_get_box(&box);

	wid=box.rx-box.lx;
	high=box.by-box.ty;

		// move within view
		
	pnt->x-=box.lx;
	pnt->y-=box.ty;

		// clicking in return?
		
	frame_idx=-1;

	tbox.lx=wid-120;
	tbox.rx=tbox.lx+120;
	tbox.ty=high-40;
	tbox.by=tbox.ty+40;

	if ((pnt->x>=tbox.lx) && (pnt->x<=tbox.rx) && (pnt->y>=tbox.ty) && (pnt->y<=tbox.by)) {
		frame_idx=texture_edit_frame_click_return_idx;
	}

		// clicking in delete
		
	if ((frame_idx==-1) && (frame_count>0)) {
		tbox.lx=470;
		tbox.rx=600;
		tbox.ty=((texture_edit_item_high*(frame_count-1))+100)-texture_edit_scroll_pos;
		tbox.by=tbox.ty+25;

		if ((pnt->x>=tbox.lx) && (pnt->x<=tbox.rx) && (pnt->y>=tbox.ty) && (pnt->y<=tbox.by)) {
			frame_idx=texture_edit_frame_click_delete_idx;
		}
	}
	
		// clicking in item?
		
	if (frame_idx==-1) {
		frame_idx=(pnt->y+texture_edit_scroll_pos)/texture_edit_item_high;
		
		tbox.lx=0;
		tbox.rx=wid;
		tbox.ty=(frame_idx*texture_edit_item_high)-texture_edit_scroll_pos;
		tbox.by=tbox.ty+texture_edit_item_high;
		
		if ((frame_idx<0) || (frame_idx>=max_texture_frame)) return(FALSE);
	}
	
	texture_edit_frame_click_idx=frame_idx;
		
		// run the click
		
	main_wind_draw();

	while (!os_track_mouse_location(&pt,&box)) {
		if ((pt.x<tbox.lx) || (pt.x>=tbox.rx) || (pt.y<tbox.ty) || (pt.y>=tbox.by)) {
			texture_edit_frame_click_idx=-1;
		}
		else {
			texture_edit_frame_click_idx=frame_idx;
		}

		main_wind_draw();
		
		usleep(10000);
	}

	if (texture_edit_frame_click_idx==-1) return(FALSE);

	texture_edit_frame_click_idx=-1;
	main_wind_draw();
		
		// return was clicked
		
	if (frame_idx==texture_edit_frame_click_return_idx) {
		texture_edit_switch(-1);
		return(TRUE);
	}
	
		// delete
		
	if (frame_idx==texture_edit_frame_click_delete_idx) {

		os_set_wait_cursor();
		
		if (state.mode==app_mode_map) {
			map_delete_texture_frame(&map,state.map.texture_edit_idx);
		}
		else {
			model_delete_texture_frame(&model,state.model.texture_edit_idx);
		}

		os_set_arrow_cursor();
		
		main_wind_draw();
		return(TRUE);
	}
	
		// can we edit this item?
		
	if (frame_idx>frame_count) {
		os_dialog_alert("Texture Frames","This texture frame can not be edited.\nYou can only add adjancent texture frames.");
		return(FALSE);
	}
	
		// item was clicked
		
	if (texture_edit_click_bitmap_open(bitmap_name)) {
		strcpy(texture->frames[frame_idx].name,bitmap_name);

		os_set_wait_cursor();
		
		if (state.mode==app_mode_map) {
			map_refresh_textures(&map);
		}
		else {
			model_refresh_textures(&model);
		}

		os_set_arrow_cursor();
	}

	main_wind_draw();

	return(TRUE);
}

