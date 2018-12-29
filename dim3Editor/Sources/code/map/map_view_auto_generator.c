/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Map Auto-Generate View Routines

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

extern list_palette_type		file_palette,map_palette;

extern file_path_setup_type		file_path_setup;
extern map_type					map;
extern app_state_type			state;
extern app_pref_type			pref;

/* =======================================================

      Map Auto Generation Setup/Finish
      
======================================================= */

void view_map_auto_generate_setup(void)
{
	select_clear();
	view_vbo_map_free();
}

void view_map_auto_generate_finish(void)
{
		// restore the VBOs,

	view_vbo_map_initialize();

		// center view to start spot

	select_add(item_map_spot,0,-1);
	map_view_goto_select_all();
}

/* =======================================================

      Enter/Exit Map Auto-Generate View
      
======================================================= */

void view_map_auto_generate_start(void)
{
	char			err_str[256];
	bool			ok;

		// run auto generator first time

	view_map_auto_generate_setup();
	ok=auto_generate_map(&map,err_str);
	view_map_auto_generate_finish();
	
	if (!ok) {
		os_dialog_alert("Auto Generator",err_str);
		return;
	}

		// switch to auto-generated mode

	state.map.auto_generate_on=TRUE;
	state.map.auto_generate_button_idx=-1;

	file_palette.hide=TRUE;
	file_palette.open=FALSE;
	map_palette.hide=TRUE;
	map_palette.open=FALSE;

	map_menu_update();

	main_wind_draw();
}

void view_map_auto_generate_end(void)
{
	state.map.auto_generate_on=FALSE;

	file_palette.hide=FALSE;
	map_palette.hide=FALSE;
	map_palette.open=TRUE;

	map_menu_update();

	main_wind_draw();
}

/* =======================================================

      Auto-Generate Textures
      
======================================================= */

void view_map_auto_generate_textures(void)
{
	char			base_path[1024],dir_path[1024];

	os_set_wait_cursor();

		// create directory for new textures
		
	file_paths_data_default(&file_path_setup,base_path,"Bitmaps/Textures",NULL,NULL);

	sprintf(dir_path,"%s/%s",base_path,map.info.name);
	os_create_directory(dir_path);

		// run bitmap generation for a map

	auto_generate_map_create_texture_set(&map,base_path);

	os_set_arrow_cursor();

	main_wind_draw();
}

/* =======================================================

      Map Auto-Generate View Cursors
      
======================================================= */

bool map_view_auto_generate_cursor(d3pnt *pnt)
{
	os_set_arrow_cursor();
	return(TRUE);
}

/* =======================================================

      Map Auto-Generate View Scroll Wheel
      
======================================================= */

void map_view_auto_generate_scroll_wheel(d3pnt *pnt,int delta)
{
}

/* =======================================================

      Map Auto-Generate View Clicking
      
======================================================= */

void map_view_auto_generate_previous_button_box(d3rect *box)
{
	int				high;
	d3rect			wbox;

	os_get_window_box(&wbox);

	high=(wbox.by-wbox.ty)-tool_button_size;

	box->lx=5;
	box->rx=205;
	box->ty=high-55;
	box->by=high-5;
}

void map_view_auto_generate_next_button_box(d3rect *box)
{
	int				high;
	d3rect			wbox;

	os_get_window_box(&wbox);

	high=(wbox.by-wbox.ty)-tool_button_size;

	box->lx=215;
	box->rx=415;
	box->ty=high-55;
	box->by=high-5;
}

void map_view_auto_generate_done_button_box(d3rect *box)
{
	int				wid,high;
	d3rect			wbox;

	os_get_window_box(&wbox);

	wid=wbox.rx-wbox.lx;
	high=(wbox.by-wbox.ty)-tool_button_size;

	box->lx=wid-205;
	box->rx=wid-5;
	box->ty=high-55;
	box->by=high-5;
}

bool map_view_auto_generate_click_button(d3pnt *pnt,d3rect *wbox,d3rect *bbox,int button_idx)
{
	bool			out_box;

	if ((pnt->x<bbox->lx) || (pnt->x>bbox->rx) || (pnt->y<bbox->ty) || (pnt->y>bbox->by)) return(FALSE);

	state.map.auto_generate_button_idx=button_idx;
	main_wind_draw();

	while (!os_track_mouse_location(pnt,wbox)) {

		out_box=((pnt->x<bbox->lx) || (pnt->x>bbox->rx) || (pnt->y<bbox->ty) || (pnt->y>bbox->by));
		if ((out_box) && (state.map.auto_generate_button_idx==-1)) {
			usleep(10000);
			continue;
		}

		if (out_box) {
			state.map.auto_generate_button_idx=-1;
		}
		else {
			state.map.auto_generate_button_idx=button_idx;
		}

		main_wind_draw();
		usleep(1000);
	}

	if (state.map.auto_generate_button_idx!=-1) {
		state.map.auto_generate_button_idx=-1;
		return(TRUE);
	}

	return(FALSE);
}

bool map_view_auto_generate_click(d3pnt *pnt,bool double_click)
{
	char			err_str[256];
	bool			ok;
	d3rect			wbox,bbox;

		// move within the view

	os_get_window_box(&wbox);
	wbox.ty+=tool_button_size;

	pnt->x-=wbox.lx;
	pnt->y-=wbox.ty;

		// check for button clicks

	map_view_auto_generate_previous_button_box(&bbox);
	if (map_view_auto_generate_click_button(pnt,&wbox,&bbox,map_auto_generate_button_previous)) {
		view_map_auto_generate_setup();
		ok=auto_generate_map_previous(&map,err_str);
		view_map_auto_generate_finish();
		if (!ok) os_dialog_alert("Auto Generator",err_str);
		main_wind_draw();
		return(TRUE);
	}

	map_view_auto_generate_next_button_box(&bbox);
	if (map_view_auto_generate_click_button(pnt,&wbox,&bbox,map_auto_generate_button_next)) {
		view_map_auto_generate_setup();
		ok=auto_generate_map_next(&map,err_str);
		view_map_auto_generate_finish();
		if (!ok) os_dialog_alert("Auto Generator",err_str);
		main_wind_draw();
		return(TRUE);
	}

	map_view_auto_generate_done_button_box(&bbox);
	if (map_view_auto_generate_click_button(pnt,&wbox,&bbox,map_auto_generate_button_done)) {
		view_map_auto_generate_end();
		return(TRUE);
	}

	return(FALSE);
}

/* =======================================================

      Map Auto-Generate View Draw
      
======================================================= */

void map_view_auto_generate_draw_button(d3rect *box,char *title,bool pushed)
{
	float			col_rg,vertexes[8],colors[16];
	
		// the button box

	col_rg=0.7f;
	if (pushed) col_rg=0.5f;

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

		// the button text

	text_draw_center(((box->lx+box->rx)>>1),(box->by-8),30.0f,NULL,title);
}

void map_view_auto_generate_draw(void)
{
	int					n,k,t,sz,wid,high;
	float				xf,zf,x_off;
	float				*vp,vertexes[8*2];
	char				str[256];
	d3pnt				*pnt,min,max;
	d3rect				wbox,box,bbox;
	map_mesh_type		*mesh;
	map_mesh_poly_type	*poly;

		// setup 2D projection

	os_get_window_box(&wbox);

	box.lx=wbox.lx;
	box.rx=wbox.rx;
	box.ty=wbox.ty+tool_button_size;
	box.by=wbox.by;

	wid=box.rx-box.lx;
	high=box.by-box.ty;

	glEnable(GL_SCISSOR_TEST);
	glScissor(box.lx,(wbox.by-box.by),wid,high);

	glViewport(box.lx,(wbox.by-box.by),wid,high);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho((GLdouble)0,(GLdouble)wid,(GLdouble)high,(GLdouble)0,-1.0,1.0);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

		// erase

	vertexes[0]=vertexes[6]=(float)0;
	vertexes[2]=vertexes[4]=(float)wid-100;
	vertexes[1]=vertexes[3]=(float)0;
	vertexes[5]=vertexes[7]=(float)high-100;
	
	glVertexPointer(2,GL_FLOAT,0,vertexes);
	glColor4f(pref.col.background.r,pref.col.background.g,pref.col.background.b,1.0f);
	glDrawArrays(GL_QUADS,0,4);

		// get bounds

	map_view_calculate_bounds(&min,&max);

		// the draw factor uses the
		// height so there's no stretching
		
	sz=max.x-min.x;
	if ((max.z-min.z)>sz) sz=max.z-min.z;

	xf=((float)high)/((float)sz);
	zf=((float)high)/((float)sz);

	x_off=((float)(wid-high))*0.5f;

		// redraw map into bounds

	glDisable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);

	glDisable(GL_DEPTH_TEST);
	
	glLineWidth(1.0f);
	glColor4f(pref.col.mesh_line.r,pref.col.mesh_line.g,pref.col.mesh_line.b,1.0f);

	mesh=map.mesh.meshes;

	for (n=0;n!=map.mesh.nmesh;n++) {

			// draw polys

		poly=mesh->polys;
	
		for (k=0;k!=mesh->npoly;k++) {			

			vp=vertexes;

			for (t=0;t!=poly->ptsz;t++) {
				pnt=&mesh->vertexes[poly->v[t]];
				*vp++=(((float)(pnt->x-min.x))*xf)+x_off;
				*vp++=((float)(pnt->z-min.z))*zf;
			}
	
			glVertexPointer(2,GL_FLOAT,0,vertexes);
			glDrawArrays(GL_POLYGON,0,poly->ptsz);

			poly++;
		}

		mesh++;
	}

		// info text

	sprintf(str,"Seed: %d",ag_random_bool());
	text_draw(5,25,20.0f,NULL,str);

	sprintf(str,"Mesh Count: %d",map.mesh.nmesh);
	text_draw(5,45,20.0f,NULL,str);

	sprintf(str,"Poly Count: %d",map_mesh_count_total_poly(&map));
	text_draw(5,65,20.0f,NULL,str);

		// buttons

	map_view_auto_generate_previous_button_box(&bbox);
	map_view_auto_generate_draw_button(&bbox,"Previous Map",(state.map.auto_generate_button_idx==map_auto_generate_button_previous));

	map_view_auto_generate_next_button_box(&bbox);
	map_view_auto_generate_draw_button(&bbox,"Next Map",(state.map.auto_generate_button_idx==map_auto_generate_button_next));

	map_view_auto_generate_done_button_box(&bbox);
	map_view_auto_generate_draw_button(&bbox,"Done",(state.map.auto_generate_button_idx==map_auto_generate_button_done));

	glDepthMask(GL_TRUE);
}



