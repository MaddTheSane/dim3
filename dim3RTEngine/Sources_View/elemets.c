/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: GUI Control Elements

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

extern app_type				app;
extern map_type				map;
extern server_type			server;
extern view_type			view;
extern iface_type			iface;
extern setup_type			setup;
extern file_path_setup_type	file_path_setup;

int							nelement,element_click_down_id,
							element_open_text_field_id,element_open_combo_id;
element_type				elements[max_element];

SDL_mutex					*element_thread_lock;

/* =======================================================

      Element Models
      
======================================================= */

model_draw* element_load_model(char *name,char *animate,float resize,d3pnt *offset,d3ang *rot)
{
	char				err_str[256];
	model_draw			*draw;
	
	draw=(model_draw*)malloc(sizeof(model_draw));
	if (draw==NULL) return(NULL);
	
	strcpy(draw->name,name);
	if (!model_draw_initialize(draw,"Interface","Element",err_str)) {
		console_add_error(err_str);
		free(draw);
		return(NULL);
	}

	draw->resize=resize;
	
	if (offset==NULL) {
		draw->offset.x=draw->offset.y=draw->offset.z=0;
	}
	else {
		memmove(&draw->offset,offset,sizeof(d3pnt));
	}

	if (rot==NULL) {
		draw->rot.x=draw->rot.y=draw->rot.z=0.0f;
	}
	else {
		memmove(&draw->rot,rot,sizeof(d3ang));
	}
	
		// start the animation

	model_start_animation(draw,animate,game_time_get_raw());
	
	return(draw);
}

void element_free_model(element_type *element)
{
	if (element->setup.model.draw==NULL) return;

	model_draw_free(element->setup.model.draw);
	free(element->setup.model.draw);

	element->setup.model.draw=NULL;
}

/* =======================================================

      Elements Clear and Free
      
======================================================= */

void element_reset_controls(void)
{
	nelement=0;

	element_click_down_id=-1;
	element_open_text_field_id=-1;
	element_open_combo_id=-1;
}

void element_release_control_memory(void)
{
	int				n,k;
	element_type	*element;
	
	element=elements;
	
	for (n=0;n<nelement;n++) {
		
		switch (element->type) {
		
			case element_type_button:
				bitmap_close(&element->setup.button.bitmap);
				bitmap_close(&element->setup.button.select_bitmap);
				break;
				
			case element_type_bitmap:
				bitmap_close(&element->setup.button.bitmap);
				break;
				
			case element_type_table:
				if (element->data!=NULL) free(element->data);
				for (k=0;k!=element_table_max_image;k++) {
					if (element->setup.table.images[k].path[0]==0x0) continue;
					bitmap_close(&element->setup.table.images[k].bitmap);
				}
				break;
				
			case element_type_text_box:
				if (element->data!=NULL) free(element->data);
				break;
				
			case element_type_model:
				element_free_model(element);
				break;
		
			case element_type_count:
				bitmap_close(&element->setup.count.bitmap);
				bitmap_close(&element->setup.count.disable_bitmap);
				break;
		}
		
		element++;
	}
}

/* =======================================================

      Initialize/Shutdown Elements
      
======================================================= */

void element_initialize(void)
{
	element_reset_controls();

	element_thread_lock=SDL_CreateMutex();		// used to make elements thread safe for certain UIs
}

void element_shutdown(void)
{
	element_release_control_memory();

	SDL_DestroyMutex(element_thread_lock);
}

/* =======================================================

      Clear Elements
      
======================================================= */

void element_clear(void)
{
	SDL_mutexP(element_thread_lock);

	element_release_control_memory();
	element_reset_controls();

	SDL_mutexV(element_thread_lock);
}

/* =======================================================

      Element Info
      
======================================================= */

int element_get_control_separation_high(void)
{
	return((int)(((float)iface.scale_x)*iface.devices[view.device_type].scale.control_high)+5);
}

int element_get_padding(void)
{
	return((int)(((float)iface.scale_x)*iface.devices[view.device_type].scale.control_padding));
}

int element_get_margin(void)
{
	return((int)(((float)iface.scale_x)*iface.devices[view.device_type].scale.dialog_margin));
}

int element_get_tab_control_high(void)
{
	return((int)(((float)iface.scale_x)*iface.devices[view.device_type].scale.tab_high));
}

int element_get_control_short_wid(void)
{
	return((int)(((float)iface.scale_x)*iface.devices[view.device_type].scale.control_short_wid));
}

int element_get_control_long_wid(void)
{
	return((int)(((float)iface.scale_x)*iface.devices[view.device_type].scale.control_long_wid));
}

int element_get_control_high(void)
{
	return((int)(((float)iface.scale_x)*iface.devices[view.device_type].scale.control_high));
}

void element_get_tab_button_bottom_left(int *x,int *y)
{
	*x=element_get_margin()+element_get_padding();
	*y=iface.scale_y-(element_get_margin()+element_get_padding());
}

void element_get_tab_button_bottom_right(int *x,int *y)
{
	*x=iface.scale_x-(element_get_margin()+element_get_padding());
	*y=iface.scale_y-(element_get_margin()+element_get_padding());
}

void element_get_frame_button_bottom_right(int fx,int fy,int wid,int high,int *x,int *y)
{
	*x=(fx+wid)-element_get_padding();
	*y=(fy+high)-element_get_padding();
}

int element_get_button_short_wid(void)
{
	return((int)(((float)iface.scale_x)*iface.devices[view.device_type].scale.button_short_wid));
}

int element_get_button_long_wid(void)
{
	return((int)(((float)iface.scale_x)*iface.devices[view.device_type].scale.button_long_wid));
}

int element_get_button_high(void)
{
	return((int)(((float)iface.scale_x)*iface.devices[view.device_type].scale.button_high));
}

int element_get_control_scroll_size(void)
{
	return((int)(((float)iface.scale_x)*iface.devices[view.device_type].scale.control_scroll_size));
}

int element_get_frame_title_high(void)
{
	int			high;

	high=gl_text_get_char_height(iface.font.text_size_medium);
	return(high+4);
}

/* =======================================================

      Add Elements
      
======================================================= */

void element_button_text_add(char *name,int id,int x,int y,int wid,int high,int x_pos,int y_pos)
{
	element_type	*element;

	SDL_mutexP(element_thread_lock);
	
	element=&elements[nelement];
	nelement++;
	
	element->id=id;
	element->type=element_type_button;
	
	element->x=x;
	element->y=y;
	
	element->selectable=TRUE;
	element->enabled=TRUE;
	element->hidden=FALSE;

	element->setup.button.mode=element_button_mode_text;
	strcpy(element->setup.button.name,name);
	
	element->setup.button.bitmap.gl_id=-1;
	element->setup.button.select_bitmap.gl_id=-1;
	
	element->wid=wid;
	element->high=high;
	
	switch (x_pos) {
		case element_pos_right:
			element->x-=element->wid;
			break;
		case element_pos_center:
			element->x-=(element->wid>>1);
			break;
	}

	switch (y_pos) {
		case element_pos_bottom:
			element->y-=element->high;
			break;
		case element_pos_center:
			element->y-=(element->high>>1);
			break;
	}

	SDL_mutexV(element_thread_lock);
}

void element_button_bitmap_add(char *path,char *path2,int id,int x,int y,int wid,int high,int x_pos,int y_pos)
{
	element_type	*element;

	SDL_mutexP(element_thread_lock);
	
	element=&elements[nelement];
	nelement++;
	
	element->id=id;
	element->type=element_type_button;
	
	element->x=x;
	element->y=y;
	
	element->selectable=TRUE;
	element->enabled=TRUE;
	element->hidden=FALSE;

	element->setup.button.mode=element_button_mode_bitmap;
	
		// get button graphics
	
	bitmap_open(&element->setup.button.bitmap,path,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE);
	bitmap_open(&element->setup.button.select_bitmap,path2,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE);

		// setup size and position
		
	if ((wid!=-1) && (high!=-1)) {
		element->wid=wid;
		element->high=high;
	}
	else {
		element->wid=element->setup.button.bitmap.wid;
		element->high=element->setup.button.bitmap.high;
	}
	
	switch (x_pos) {
		case element_pos_right:
			element->x-=element->wid;
			break;
		case element_pos_center:
			element->x-=(element->wid>>1);
			break;
	}

	switch (y_pos) {
		case element_pos_bottom:
			element->y-=element->high;
			break;
		case element_pos_center:
			element->y-=(element->high>>1);
			break;
	}

	SDL_mutexV(element_thread_lock);
}

void element_button_box_add(int id,int x,int y,int wid,int high,int x_pos,int y_pos)
{
	element_type	*element;

	SDL_mutexP(element_thread_lock);
	
	element=&elements[nelement];
	nelement++;
	
	element->id=id;
	element->type=element_type_button;
	
	element->x=x;
	element->y=y;
	
	element->selectable=TRUE;
	element->enabled=TRUE;
	element->hidden=FALSE;

	element->setup.button.mode=element_button_mode_box;

	element->setup.button.name[0]=0x0;

	element->setup.button.bitmap.gl_id=-1;
	element->setup.button.select_bitmap.gl_id=-1;
	
	element->wid=wid;
	element->high=high;
	
	switch (x_pos) {
		case element_pos_right:
			element->x-=element->wid;
			break;
		case element_pos_center:
			element->x-=(element->wid>>1);
			break;
	}

	switch (y_pos) {
		case element_pos_bottom:
			element->y-=element->high;
			break;
		case element_pos_center:
			element->y-=(element->high>>1);
			break;
	}

	SDL_mutexV(element_thread_lock);
}

void element_bitmap_add(char *path,int id,int x,int y,int wid,int high,bool framed)
{
	element_type	*element;

	SDL_mutexP(element_thread_lock);
	
	element=&elements[nelement];
	nelement++;
	
	element->id=id;
	element->type=element_type_bitmap;
	
	bitmap_open(&element->setup.button.bitmap,path,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE);
	element->setup.button.select_bitmap.gl_id=-1;
	
	element->x=x;
	element->y=y;
	
	if ((wid==-1) || (high==-1)) {
		element->wid=element->setup.button.bitmap.wid;
		element->high=element->setup.button.bitmap.high;
	}
	else {
		element->wid=wid;
		element->high=high;
	}
	
	element->selectable=FALSE;
	element->enabled=TRUE;
	element->hidden=FALSE;
	element->framed=framed;

	SDL_mutexV(element_thread_lock);
}

void element_text_add(char *str,int id,int x,int y,int size,int just,d3col *col,bool selectable)
{
	int				wid,high;
	char			*c,*c2;
	element_type	*element;

	SDL_mutexP(element_thread_lock);
	
	element=&elements[nelement];
	nelement++;
	
	element->id=id;
	element->type=element_type_text;
	
	element->x=x;
	element->y=y;
	
	element->setup.text.size=size;
	element->setup.text.just=just;
	
	if (col==NULL) {
		memmove(&element->setup.text.col,&iface.color.control.label,sizeof(d3col));
	}
	else {
		memmove(&element->setup.text.col,col,sizeof(d3col));
	}

	element->selectable=selectable;
	element->enabled=TRUE;
	element->hidden=FALSE;
	
	strncpy(element->str,str,max_element_str_len);
	element->str[max_element_str_len-1]=0x0;

		// need to calculate size through returns

	high=gl_text_get_char_height(size);

	element->wid=0;
	element->high=high;

	c=element->str;
	
	while (*c!=0x0) {
		c2=strstr(c,"{r}");
		if (c2!=NULL) *c2=0x0;
		
		wid=gl_text_get_string_width(font_interface_index,size,FALSE,c);
		if (wid>element->wid) element->wid=wid;
		
		if (c2==NULL) break;

		element->high+=high;
		
		*c2='{';
		
		c=c2+3;
	}

	SDL_mutexV(element_thread_lock);
}

void element_text_field_add(char *str,char *value_str,int max_value_str_sz,int id,int x,int y,bool selectable)
{
	element_type	*element;

	SDL_mutexP(element_thread_lock);

	element=&elements[nelement];
	nelement++;
	
	element->id=id;
	element->type=element_type_text_field;
	
	element->x=x;
	element->y=y;

	element->wid=(int)(((float)iface.scale_x)*iface.devices[view.device_type].scale.control_long_wid);
	element->high=element_get_control_high();
	
	element->selectable=selectable;
	element->enabled=TRUE;
	element->hidden=FALSE;
	
	strcpy(element->str,str);
	strncpy(element->value_str,value_str,max_value_str_sz);
	element->value_str[max_value_str_sz-1]=0x0;

	element->max_value_str_sz=max_value_str_sz;

	SDL_mutexV(element_thread_lock);
}

void element_number_add(char *str,int value,int id,int x,int y,int min,int max)
{
	element_type	*element;

	SDL_mutexP(element_thread_lock);

	element=&elements[nelement];
	nelement++;
	
	element->id=id;
	element->type=element_type_number;
	
	element->x=x;
	element->y=y;

	element->wid=(int)(((float)iface.scale_x)*iface.devices[view.device_type].scale.control_short_wid);
	element->high=element_get_control_high();
	
	element->selectable=TRUE;
	element->enabled=TRUE;
	element->hidden=FALSE;
	
	strcpy(element->str,str);
	element->value=value;
	element->setup.number.min=min;
	element->setup.number.max=max;

	SDL_mutexV(element_thread_lock);
}

void element_checkbox_add(char *str,int value,int id,int x,int y,bool selectable)
{
	element_type	*element;

	SDL_mutexP(element_thread_lock);

	element=&elements[nelement];
	nelement++;
	
	element->id=id;
	element->type=element_type_checkbox;
	
	element->x=x;
	element->y=y;
	
	element->selectable=selectable;
	element->enabled=TRUE;
	element->hidden=FALSE;

	element->wid=(int)(((float)iface.scale_x)*iface.devices[view.device_type].scale.control_short_wid);
	element->high=element_get_control_high();
	
	strcpy(element->str,str);
	
	element->value=value;

	SDL_mutexV(element_thread_lock);
}

void element_combo_add(char *str,char *combo_data,int value,int id,int x,int y,bool selectable)
{
	element_type	*element;

	SDL_mutexP(element_thread_lock);

	element=&elements[nelement];
	nelement++;
	
	element->id=id;
	element->type=element_type_combo;
	
	element->x=x;
	element->y=y;
	
	element->selectable=selectable;
	element->enabled=TRUE;
	element->hidden=FALSE;

	element->wid=(int)(((float)iface.scale_x)*iface.devices[view.device_type].scale.control_long_wid);
	element->high=element_get_control_high();
	
	strcpy(element->str,str);
	
	element->value=value;
	element->data=combo_data;

	SDL_mutexV(element_thread_lock);
}

void element_slider_add(char *str,float value,float value_min,float value_max,int id,int x,int y,bool selectable)
{
	element_type	*element;

	SDL_mutexP(element_thread_lock);

	element=&elements[nelement];
	nelement++;
	
	element->id=id;
	element->type=element_type_slider;
	
	element->x=x;
	element->y=y;
	
	element->selectable=selectable;
	element->enabled=TRUE;
	element->hidden=FALSE;

	element->wid=(int)(((float)iface.scale_x)*iface.devices[view.device_type].scale.control_long_wid);
	element->high=element_get_control_high();

	strcpy(element->str,str);
	
	element->setup.slider.min=value_min;
	element->setup.slider.max=value_max;
	element->setup.slider.value=(value-value_min)/(value_max-value_min);

	SDL_mutexV(element_thread_lock);
}

int element_table_add_get_data_size(char *row_data)
{
	int			count;
	char		*c;
	
	count=0;
	c=row_data;
	
	while (TRUE) {
		if (*c==0x0) break;
		c+=128;
		count++;
	}

	return((count+1)*128);
}

void element_table_add(element_column_type* cols,char *row_data,int id,int ncolumn,int x,int y,int wid,int high,bool checkbox,int bitmap_mode)
{
	int				n,sz;
	element_type	*element;

	SDL_mutexP(element_thread_lock);

	element=&elements[nelement];
	nelement++;
	
	element->id=id;
	element->type=element_type_table;
	
	element->setup.table.busy=FALSE;
	
	element->x=x;
	element->y=y;
	element->wid=wid;
	element->high=high;
	
	element->selectable=TRUE;
	element->enabled=TRUE;
	element->hidden=FALSE;
	
	element->value=-1;
	element->offset=0;
	
	if (row_data==NULL) {
		element->data=NULL;
	}
	else {
		sz=element_table_add_get_data_size(row_data);
		element->data=malloc(sz);
		memmove(element->data,row_data,sz);
	}

	element->setup.table.checkbox=checkbox;
	element->setup.table.bitmap_mode=bitmap_mode;

	bzero(element->setup.table.checks,element_table_max_check);
	
	for (n=0;n!=element_table_max_image;n++) {
		element->setup.table.images[n].path[0]=0x0;
		element->setup.table.images[n].bitmap.gl_id=-1;
	}
	
	element->setup.table.next_image_idx=0;

		// if checkbox is on, we need to
		// add the checkbox column and remove
		// that space from the first column

	if (!checkbox) {
		element->setup.table.ncolumn=ncolumn;
		memmove(element->setup.table.cols,cols,(sizeof(element_column_type)*ncolumn));
	}
	else {
		element->setup.table.ncolumn=ncolumn+1;
		memmove(&element->setup.table.cols[1],cols,(sizeof(element_column_type)*ncolumn));

		element->setup.table.cols[0].percent_size=element_table_check_column_size;
		element->setup.table.cols[0].name[0]=0x0;

		element->setup.table.cols[1].percent_size-=element_table_check_column_size;
	}
	
	SDL_mutexV(element_thread_lock);
}

void element_tab_add(int id,int x,int y,int wid,int high,int tab_count,char *tabs)
{
	int				n;
	element_type	*element;

	SDL_mutexP(element_thread_lock);

	element=&elements[nelement];
	nelement++;
	
	element->id=id;
	element->type=element_type_tab;
	
	element->x=x;
	element->y=y;
	element->wid=wid;
	element->high=high;
	
	element->selectable=TRUE;
	element->enabled=TRUE;
	element->hidden=FALSE;
	
	element->value=0;
	element->offset=0;

	element->setup.tab.ntab=tab_count;
	
	for (n=0;n!=tab_count;n++) {
		strcpy(element->setup.tab.name[n],(char*)&tabs[n*name_str_len]);
	}

	SDL_mutexV(element_thread_lock);
}

void element_text_box_add(char *data,int id,int x,int y,int wid,int high,bool error_display)
{
	int				sz;
	element_type	*element;

	SDL_mutexP(element_thread_lock);

	element=&elements[nelement];
	nelement++;
	
	element->id=id;
	element->type=element_type_text_box;
	
	element->x=x;
	element->y=y;
	element->wid=wid;
	element->high=high;
	
	element->selectable=TRUE;
	element->enabled=TRUE;
	element->hidden=FALSE;
	
	element->value=-1;
	element->offset=0;
	
	element->setup.text_box.error_display=error_display;
	element->setup.text_box.scroll_up_ok=element->setup.text_box.scroll_down_ok=FALSE;
	
	if (data==NULL) {
		element->data=NULL;
	}
	else {
		sz=strlen(data)+1;
		element->data=malloc(sz);
		memmove(element->data,data,sz);
	}
	
	SDL_mutexV(element_thread_lock);
}

void element_info_field_add(char *str,char *value_str,int id,int x,int y)
{
	element_type	*element;

	SDL_mutexP(element_thread_lock);

	element=&elements[nelement];
	nelement++;
	
	element->id=id;
	element->type=element_type_info_field;
	
	element->x=x;
	element->y=y;

	element->wid=element_get_control_long_wid();
	element->high=element_get_control_high();
	
	element->selectable=FALSE;
	element->enabled=TRUE;
	element->hidden=FALSE;
	
	strcpy(element->str,str);
	strncpy(element->value_str,value_str,max_element_value_str_len);
	element->value_str[max_element_value_str_len-1]=0x0;

	SDL_mutexV(element_thread_lock);
}

void element_model_add(char *name,char *animate,float resize,d3pnt *offset,d3ang *rot,int id,int x,int y)
{
	element_type	*element;

	SDL_mutexP(element_thread_lock);

	element=&elements[nelement];
	nelement++;
	
	element->id=id;
	element->type=element_type_model;
	
	element->x=x;
	element->y=y;
	
	element->selectable=FALSE;
	element->enabled=TRUE;
	element->hidden=FALSE;
	
	element->setup.model.draw=element_load_model(name,animate,resize,offset,rot);

	SDL_mutexV(element_thread_lock);
}

void element_count_add(char *path,char *disable_path,int id,int x,int y,int wid,int high,int bitmap_add,bool horizontal,int wrap_count,int count,int max_count)
{
	element_type	*element;

	SDL_mutexP(element_thread_lock);
	
	element=&elements[nelement];
	nelement++;
	
	element->id=id;
	element->type=element_type_count;
	
	bitmap_open(&element->setup.count.bitmap,path,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE);
	bitmap_open(&element->setup.count.disable_bitmap,disable_path,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE);
	
	element->x=x;
	element->y=y;
	
	if ((wid==-1) || (high==-1)) {
		element->wid=element->setup.count.bitmap.wid;
		element->high=element->setup.count.bitmap.high;
	}
	else {
		element->wid=wid;
		element->high=high;
	}
	
	element->value=count;
	
	element->selectable=FALSE;
	element->enabled=TRUE;
	element->hidden=FALSE;
	element->framed=FALSE;
	
	element->setup.count.bitmap_add=bitmap_add;
	element->setup.count.horizontal=horizontal;
	element->setup.count.wrap_count=wrap_count;
	element->setup.count.max_count=max_count;

	SDL_mutexV(element_thread_lock);
}

void element_frame_add(char *title,int id,int x,int y,int wid,int high,int tab_id,int tab_count,char *tabs,int button_count,element_frame_button_type *buttons)
{
	int				n,blx,brx,ty,by,
					butt_wid,butt_high,padding;
	element_type	*element;

		// the frame itself

	SDL_mutexP(element_thread_lock);

	element=&elements[nelement];
	nelement++;
	
	element->id=id;
	element->type=element_type_frame;
	
	element->x=x;
	element->y=y;
	element->wid=wid;
	element->high=high;
	
	strcpy(element->str,title);
	
	element->selectable=FALSE;
	element->enabled=TRUE;
	element->hidden=FALSE;

	SDL_mutexV(element_thread_lock);

		// tabs

	if (tab_count!=0) {
		ty=y;
		if (title[0]!=0x0) ty+=element_get_frame_title_high();

		element_tab_add(tab_id,(x+1),ty,(wid-2),element_get_tab_control_high(),tab_count,tabs);
	}

		// buttons

	butt_wid=element_get_button_short_wid();
	butt_high=element_get_button_high();

	padding=element_get_padding();

	blx=x+padding;
	by=(y+high)-padding;

	brx=(x+wid)+butt_wid;

	for (n=0;n!=button_count;n++) {
		if (buttons[n].right) brx-=(butt_wid+padding);
	}

	for (n=0;n!=button_count;n++) {
		if (!buttons[n].right) {
			element_button_text_add(buttons[n].text,buttons[n].id,blx,by,butt_wid,butt_high,element_pos_left,element_pos_bottom);
			blx+=(butt_wid+padding);
		}
		else {
			element_button_text_add(buttons[n].text,buttons[n].id,brx,by,butt_wid,butt_high,element_pos_right,element_pos_bottom);
			brx+=(butt_wid+padding);
		}
	}

		// remember if tabs or buttons

	element->setup.frame.has_tabs=(tab_count!=0);
	element->setup.frame.has_buttons=(button_count!=0);
}

/* =======================================================

      Change Elements
      
======================================================= */

element_type* element_find(int id)
{
	int					n;
	element_type		*element;
		
	element=elements;
	
	for (n=0;n<nelement;n++) {
		if (element->id==id) return(element);
		element++;
	}
	
	return(NULL);
}

void element_enable(int id,bool enabled)
{
	element_type		*element;
	
	element=element_find(id);
	if (element!=NULL) element->enabled=enabled;
}

void element_hide(int id,bool hide)
{
	element_type		*element;
	
	element=element_find(id);
	if (element!=NULL) element->hidden=hide;
}

void element_table_busy(int id,bool busy)
{
	element_type		*element;
	
	element=element_find(id);
	if (element!=NULL) element->setup.table.busy=busy;
}

void element_text_change(int id,char *str)
{
	element_type		*element;
	
	element=element_find(id);
	if (element!=NULL) strcpy(element->str,str);
}

/* =======================================================

      Select Element
      
======================================================= */

void element_get_box(element_type *element,int *lft,int *rgt,int *top,int *bot)
{
	switch (element->type) {
	
		case element_type_button:
		case element_type_bitmap:
		case element_type_table:
		case element_type_tab:
		case element_type_text_box:
		case element_type_count:
		case element_type_frame:
			*lft=element->x;
			*rgt=(*lft)+element->wid;
			*top=element->y;
			*bot=(*top)+element->high;
			return;
			
		case element_type_text:
			switch (element->setup.text.just) {
				case tx_center:
					*lft=element->x-(element->wid>>1);
					*rgt=element->x+(element->wid>>1);
					break;
				case tx_right:
					*lft=element->x-element->wid;
					*rgt=element->x;
					break;
				default:
					*lft=element->x;
					*rgt=element->x+element->wid;
					break;
			}
			*top=element->y-element->high;
			*bot=element->y;
			return;

		case element_type_text_field:
		case element_type_number:
		case element_type_combo:
		case element_type_slider:
		case element_type_info_field:
			*lft=element->x+5;
			*rgt=(element->x+10)+element->wid;
			*top=(element->y-element->high)-1;
			*bot=element->y+1;
			return;
			
		case element_type_checkbox:
			*lft=element->x+5;
			*rgt=(element->x+5)+element->wid;
			*top=element->y-element->high;
			*bot=element->y;
			return;

	}
	
	*lft=*rgt=*top=*bot=-1;
}

int element_find_for_xy(int x,int y)
{
	int					n,lft,rgt,top,bot;
	element_type		*element;
	
	element=elements;
	
	for (n=0;n!=nelement;n++) {
		if ((element->selectable) && (!element->hidden) && (element->enabled)) {
			element_get_box(element,&lft,&rgt,&top,&bot);
			if ((x>=lft) && (x<=rgt) && (y>=top) && (y<=bot)) return(element->id);
		}
		element++;
	}
	
	return(-1);
}

/* =======================================================

      Button Element
      
======================================================= */

void element_draw_button_text(element_type *element,int sel_id)
{
	int				x,y,lft,rgt,top,bot;
	float			alpha;
	d3col			outline_col;
	
	if (element->enabled) {
		alpha=1.0f;
		memmove(&outline_col,&iface.color.button.outline,sizeof(d3col));
	}
	else {
		alpha=0.3f;
		memmove(&outline_col,&iface.color.button.outline,sizeof(d3col));
	}
	
	element_get_box(element,&lft,&rgt,&top,&bot);
	
		// button background and outline

	view_primitive_2D_color_quad(&iface.color.button.fill,alpha,lft,rgt,top,bot);
	view_primitive_2D_line_quad(&outline_col,alpha,lft,rgt,top,bot);

	if (element->id==sel_id) view_primitive_2D_line_quad(&iface.color.control.mouse_over,alpha,(lft-1),(rgt+1),(top-1),(bot+1));
	
		// button text

	x=(lft+rgt)>>1;
	y=((top+bot)>>1)-(iface.font.text_size_medium/10);

	gl_text_start(font_interface_index,iface.font.text_size_medium,FALSE);
	gl_text_draw(x,y,element->setup.button.name,tx_center,TRUE,&iface.color.button.text,alpha);
	gl_text_end();
}

void element_draw_button_bitmap(element_type *element,int sel_id)
{
	int				lft,rgt,top,bot;
	float			alpha;
	GLuint			gl_id;
	
	if (element->enabled) {
		if ((element->id==sel_id) && (element->setup.button.select_bitmap.gl_id!=-1)) {
			gl_id=element->setup.button.select_bitmap.gl_id;
			alpha=1.0f;
		}
		else {
			gl_id=element->setup.button.bitmap.gl_id;
			alpha=1.0f;
		}
	}
	else {
		gl_id=element->setup.button.bitmap.gl_id;
		alpha=0.5f;
	}
	
	element_get_box(element,&lft,&rgt,&top,&bot);

	view_primitive_2D_texture_quad(gl_id,NULL,alpha,lft,rgt,top,bot,0.0f,1.0f,0.0f,1.0f,TRUE);
}

void element_draw_button_box(element_type *element,int sel_id)
{
	int				y,lft,rgt,top,bot;
	float			alpha;
	d3col			gradient_start,gradient_end,outline_col;

	if (element->enabled) {
		alpha=1.0f;
		memmove(&outline_col,&iface.color.picker.outline,sizeof(d3col));
	}
	else {
		alpha=0.3f;
		memmove(&outline_col,&iface.color.picker.outline,sizeof(d3col));
	}
	
	element_get_box(element,&lft,&rgt,&top,&bot);
	
		// button background and outline

	if (element->id!=sel_id) {
		memmove(&gradient_end,&iface.color.picker.fill,sizeof(d3col));
	}
	else {
		memmove(&gradient_end,&iface.color.picker.hilite,sizeof(d3col));
	}
	
	gradient_start.r=gradient_end.r*element_gradient_factor_background;
	gradient_start.g=gradient_end.g*element_gradient_factor_background;
	gradient_start.b=gradient_end.b*element_gradient_factor_background;

	y=(top+bot)>>1;
	view_primitive_2D_color_poly(lft,top,&gradient_start,rgt,top,&gradient_start,rgt,y,&gradient_end,lft,y,&gradient_end,alpha);
	view_primitive_2D_color_poly(lft,y,&gradient_end,rgt,y,&gradient_end,rgt,bot,&gradient_start,lft,bot,&gradient_start,alpha);

	view_primitive_2D_line_quad(&outline_col,alpha,lft,rgt,top,bot);
	if (element->id==sel_id) view_primitive_2D_line_quad(&iface.color.control.mouse_over,alpha,(lft-1),(rgt+1),(top-1),(bot+1));
}

void element_draw_button(element_type *element,int sel_id)
{
	switch (element->setup.button.mode) {

		case element_button_mode_text:
			element_draw_button_text(element,sel_id);
			break;

		case element_button_mode_bitmap:
			element_draw_button_bitmap(element,sel_id);
			break;

		case element_button_mode_box:
			element_draw_button_box(element,sel_id);
			break;

	}
}

/* =======================================================

      Bitmap and Count Element
      
======================================================= */

void element_draw_bitmap(element_type *element)
{
	int				lft,rgt,top,bot;
	
	element_get_box(element,&lft,&rgt,&top,&bot);

		// the picture
		
	view_primitive_2D_texture_quad(element->setup.button.bitmap.gl_id,NULL,1.0f,lft,rgt,top,bot,0.0f,1.0f,0.0f,1.0f,TRUE);
	
		// the frame
		
	if (element->framed) {
		view_primitive_2D_line_quad(&iface.color.control.outline,1.0f,lft,rgt,top,bot);
	}
}

void element_draw_count(element_type *element)
{
	int				n,lft,rgt,top,bot,
					org_lft,org_rgt,org_top,org_bot,
					row_count;
	GLuint			gl_id;
	
	element_get_box(element,&lft,&rgt,&top,&bot);
	
	org_lft=lft;
	org_rgt=rgt;
	org_top=top;
	org_bot=bot;

		// the count images
		
	row_count=element->setup.count.wrap_count;
		
	for (n=0;n!=element->setup.count.max_count;n++) {
	
		if (n<element->value) {
			gl_id=element->setup.count.bitmap.gl_id;
		}
		else {
			gl_id=element->setup.count.disable_bitmap.gl_id;
		}
		
		view_primitive_2D_texture_quad(gl_id,NULL,1.0f,lft,rgt,top,bot,0.0f,1.0f,0.0f,1.0f,TRUE);
		
		if (element->setup.count.horizontal) {
			if (row_count!=-1) {
				row_count--;
				if (row_count==0) {
					row_count=element->setup.count.wrap_count;
					lft=org_lft;
					rgt=org_rgt;
					top+=element->setup.count.bitmap_add;
					bot+=element->setup.count.bitmap_add;
					continue;
				}
			}
			
			lft+=element->setup.count.bitmap_add;
			rgt+=element->setup.count.bitmap_add;
		}
		else {
			if (row_count!=-1) {
				row_count--;
				if (row_count==0) {
					row_count=element->setup.count.wrap_count;
					top=org_top;
					bot=org_bot;
					lft+=element->setup.count.bitmap_add;
					rgt+=element->setup.count.bitmap_add;
					continue;
				}
			}
			
			top+=element->setup.count.bitmap_add;
			bot+=element->setup.count.bitmap_add;
		}
	}
}

/* =======================================================

      Text Element
      
======================================================= */

void element_draw_text(element_type *element,int sel_id)
{
	int				y,high;
	char			*c,*c2;
	d3col			col;

		// get text color

	if (!element->enabled) {
		memmove(&col,&iface.color.control.disabled,sizeof(d3col));
	}
	else {
		if ((element->id!=-1) && (element->id==sel_id)) {
			memmove(&col,&iface.color.control.mouse_over,sizeof(d3col));
		}
		else {
			memmove(&col,&element->setup.text.col,sizeof(d3col));
		}
	}

		// get height

	y=element->y;
	high=gl_text_get_char_height(element->setup.text.size);

		// draw text

	gl_text_start(font_interface_index,element->setup.text.size,FALSE);

	c=element->str;
	
	while (*c!=0x0) {
		c2=strstr(c,"{r}");
		if (c2!=NULL) *c2=0x0;
		
		gl_text_draw(element->x,y,c,element->setup.text.just,FALSE,&col,1.0f);
		
		if (c2==NULL) break;
		
		*c2='{';
		
		c=c2+3;
		y+=high;
	}

	gl_text_end();
}

/* =======================================================

      Text Field Element
      
======================================================= */

void element_click_text_field(element_type *element)
{
	element_open_text_field_id=element->id;
}

bool element_click_text_field_open(element_type *element,int x,int y)
{
	int				high,lft,rgt,top,bot;
	
	high=gl_text_get_char_height(iface.font.text_size_medium);
		
		// control box
		
	lft=element->x+3;
	rgt=lft+252;
	top=(element->y-high)-1;
	bot=element->y;
	
		// in box?
		
	if ((x<lft) || (x>rgt) || (y<top) || (y>bot)) return(FALSE);

	return(TRUE);
}

void element_draw_text_field(element_type *element,int sel_id)
{
	int				x,y,ky,wid,
					lft,rgt,top,bot;
	float			alpha;
	
	x=element->x;
	y=element->y;
	
		// label

	ky=y-(element->high>>1);
		
	gl_text_start(font_interface_index,iface.font.text_size_medium,FALSE);
	gl_text_draw((x-5),(ky-2),element->str,tx_right,TRUE,&iface.color.control.label,1.0f);
	gl_text_draw(x,(ky-3),":",tx_center,TRUE,&iface.color.control.label,1.0f);
	gl_text_end();
		
		// control box
		
	lft=x+10;
	rgt=lft+element->wid;
	top=ky-(element->high>>1);
	bot=top+element->high;
	
	alpha=(element->enabled?1.0f:0.3f);

		// background

	view_primitive_2D_color_quad(&iface.color.control.fill,1.0f,lft,rgt,top,bot);

		// outline

	view_primitive_2D_line_quad(&iface.color.control.outline,alpha,lft,rgt,top,bot);
	if ((element->id==sel_id) && (element->enabled)) view_primitive_2D_line_quad(&iface.color.control.mouse_over,alpha,(lft-1),(rgt+1),(top-1),(bot+1));

		// control text

	gl_text_start(font_interface_index,iface.font.text_size_medium,FALSE);
		
	if (element->enabled) {
		if (element->id==element_open_text_field_id) {
			gl_text_draw((x+15),(ky-2),element->value_str,tx_left,TRUE,&iface.color.control.mouse_over,1.0f);
		}
		else {
			gl_text_draw((x+15),(ky-2),element->value_str,tx_left,TRUE,&iface.color.control.text,1.0f);
		}
	}
	else {
		gl_text_draw((x+15),(ky-2),element->value_str,tx_left,TRUE,&iface.color.control.disabled,1.0f);
	}
	
	gl_text_end();

		// flashing cursor

	if (element_open_text_field_id!=element->id) return;
	if ((game_time_get_raw()&0x200)==0x0) return;

	wid=gl_text_get_string_width(font_interface_index,iface.font.text_size_medium,FALSE,element->value_str);

	top+=3;
	bot-=3;
	lft+=(wid+8);
	rgt=lft+((bot-top)>>1);

	view_primitive_2D_color_quad(&iface.color.control.mouse_over,1.0f,lft,rgt,top,bot);
}

/* =======================================================

      Number Element
      
======================================================= */

void element_click_number(element_type *element,int x,int y)
{
	int				lft,rgt,mid;

	lft=element->x;
	rgt=lft+element->wid;
	mid=(lft+rgt)>>1;

		// left arrow

	if ((x>=lft) && (x<=mid)) {
		if (element->value>element->setup.number.min) element->value--;
		return;
	}

		// right arrow

	if ((x>=mid) && (x<=rgt)) {
		if (element->value<element->setup.number.max) element->value++;
		return;
	}
}

void element_draw_number(element_type *element,int sel_id)
{
	int				x,y,ky,lft,lft_mid,rgt,rgt_mid,top,bot;
	float			alpha;
	char			txt[256];
	d3col			col,gradient_start,gradient_end;
	
	x=element->x;
	y=element->y;
	
		// label

	ky=y-(element->high>>1);
		
	gl_text_start(font_interface_index,iface.font.text_size_medium,FALSE);
	gl_text_draw((x-5),(ky-2),element->str,tx_right,TRUE,&iface.color.control.label,1.0f);
	gl_text_draw(x,(ky-3),":",tx_center,TRUE,&iface.color.control.label,1.0f);
	gl_text_end();
		
		// control box
		
	lft=x+10;
	rgt=lft+element->wid;
	top=ky-(element->high>>1);
	bot=top+element->high;
	
	lft_mid=lft+(bot-top);
	rgt_mid=rgt-(bot-top);
	
	alpha=(element->enabled?1.0f:0.3f);

		// background

	memmove(&gradient_start,&iface.color.control.fill,sizeof(d3col));
	gradient_end.r=gradient_start.r*element_gradient_factor_background;
	gradient_end.g=gradient_start.g*element_gradient_factor_background;
	gradient_end.b=gradient_start.b*element_gradient_factor_background;

	view_primitive_2D_color_poly(lft,top,&gradient_start,rgt,top,&gradient_start,rgt,ky,&gradient_end,lft,ky,&gradient_end,alpha);
	view_primitive_2D_color_poly(lft,ky,&gradient_end,rgt,ky,&gradient_end,rgt,bot,&gradient_start,lft,bot,&gradient_start,alpha);

		// outline

	view_primitive_2D_line_quad(&iface.color.control.outline,alpha,lft,rgt,top,bot);
	if ((element->id==sel_id) && (element->enabled)) view_primitive_2D_line_quad(&iface.color.control.mouse_over,alpha,(lft-1),(rgt+1),(top-1),(bot+1));

		// control text
		
	x=(lft_mid+rgt_mid)>>1;

	sprintf(txt,"%d",element->value);

	gl_text_start(font_interface_index,iface.font.text_size_medium,FALSE);
		
	if (element->enabled) {
		if (element->id==element_open_text_field_id) {
			gl_text_draw(x,(ky-1),txt,tx_center,TRUE,&iface.color.control.mouse_over,1.0f);
		}
		else {
			gl_text_draw(x,(ky-1),txt,tx_center,TRUE,&iface.color.control.text,1.0f);
		}
	}
	else {
		gl_text_draw(x,(ky-1),txt,tx_center,TRUE,&iface.color.control.disabled,1.0f);
	}
	
	gl_text_end();

		// arrows

	lft+=4;
	lft_mid-=4;
	top+=4;
	bot-=4;

	col.r=col.g=col.b=0.0f;

	if (element->value>element->setup.number.min) {
		view_primitive_2D_color_trig(&iface.color.control.hilite,1.0f,lft,lft_mid,top,bot,3);
		view_primitive_2D_line_trig(&col,1.0f,lft,lft_mid,top,bot,3);
	}

	rgt_mid+=4;
	rgt-=4;

	if (element->value<element->setup.number.max) {
		view_primitive_2D_color_trig(&iface.color.control.hilite,1.0f,rgt_mid,rgt,top,bot,1);
		view_primitive_2D_line_trig(&col,1.0f,rgt_mid,rgt,top,bot,1);
	}
}

/* =======================================================

      Checkbox Element
      
======================================================= */

void element_click_checkbox(element_type *element)
{
	element->value=element->value^0x1;
}

void element_draw_checkbox_control(int x,int y,int wid,bool checked,bool enabled,bool hilite)
{
	int			lft,rgt,top,bot,high,
				chk_lft,chk_rgt;
	float		alpha;
	d3col		col,col2,gradient_start,gradient_end;

		// checkbox

	high=element_get_control_high();

	lft=x;
	rgt=lft+wid;
	top=y-(high>>1);
	bot=top+high;

	alpha=(enabled?1.0f:0.3f);

		// background
		
	memmove(&gradient_start,&iface.color.control.fill,sizeof(d3col));
	gradient_end.r=gradient_start.r*element_gradient_factor_background;
	gradient_end.g=gradient_start.g*element_gradient_factor_background;
	gradient_end.b=gradient_start.b*element_gradient_factor_background;

	view_primitive_2D_color_poly(lft,top,&gradient_start,rgt,top,&gradient_start,rgt,y,&gradient_end,lft,y,&gradient_end,alpha);
	view_primitive_2D_color_poly(lft,y,&gradient_end,rgt,y,&gradient_end,rgt,bot,&gradient_start,lft,bot,&gradient_start,alpha);

		// check position

	if (checked) {
		chk_lft=lft+(wid>>1);
		chk_rgt=rgt;

		col.r=iface.color.control.hilite.r;
		col.g=iface.color.control.hilite.g;
		col.b=iface.color.control.hilite.b;

		col2.r=iface.color.control.hilite.r*element_gradient_factor_foreground;
		col2.g=iface.color.control.hilite.g*element_gradient_factor_foreground;
		col2.b=iface.color.control.hilite.b*element_gradient_factor_foreground;
	}
	else {
		chk_lft=lft;
		chk_rgt=lft+(wid>>1);

		col.r=iface.color.control.fill.r*element_gradient_factor_darken;
		col.g=iface.color.control.fill.g*element_gradient_factor_darken;
		col.b=iface.color.control.fill.b*element_gradient_factor_darken;

		col2.r=(iface.color.control.fill.r*element_gradient_factor_darken)*element_gradient_factor_foreground;
		col2.g=(iface.color.control.fill.g*element_gradient_factor_darken)*element_gradient_factor_foreground;
		col2.b=(iface.color.control.fill.b*element_gradient_factor_darken)*element_gradient_factor_foreground;
	}

	view_primitive_2D_color_poly(chk_lft,top,&col,chk_rgt,top,&col,chk_rgt,y,&col2,chk_lft,y,&col2,alpha);
	view_primitive_2D_color_poly(chk_lft,y,&col2,chk_rgt,y,&col2,chk_rgt,bot,&col,chk_lft,bot,&col,alpha);

		// text

	gl_text_start(font_interface_index,iface.font.text_size_medium,FALSE);
	if (checked) {
		gl_text_draw((rgt-(wid>>2)),(y-1),"on",tx_center,TRUE,&iface.color.control.label,1.0f);
	}
	else {
		gl_text_draw((lft+(wid>>2)),(y-1),"off",tx_center,TRUE,&iface.color.control.label,1.0f);
	}
	gl_text_end();

		// outline

	view_primitive_2D_line_quad(&iface.color.control.outline,alpha,lft,rgt,top,bot);
	view_primitive_2D_line_quad(&iface.color.control.outline,alpha,chk_lft,chk_rgt,top,bot);

	if ((hilite) && (enabled)) view_primitive_2D_line_quad(&iface.color.control.mouse_over,alpha,(lft-1),(rgt+1),(top-1),(bot+1));
}

void element_draw_checkbox(element_type *element,int sel_id)
{
	int				x,y,wid,ky;
	
	x=element->x;
	y=element->y;
	
		// label

	ky=y-(element->high>>1);

	gl_text_start(font_interface_index,iface.font.text_size_medium,FALSE);
	gl_text_draw((x-5),(ky-2),element->str,tx_right,TRUE,&iface.color.control.label,1.0f);
	gl_text_draw(x,(ky-3),":",tx_center,TRUE,&iface.color.control.label,1.0f);
	gl_text_end();
	
		// checkbox

	wid=element_get_control_short_wid();
	element_draw_checkbox_control((x+10),ky,wid,(element->value!=0),element->enabled,(element->id==sel_id));
}

/* =======================================================

      Combo Element
      
======================================================= */

int element_get_combo_list_count(element_type *element)
{
	int			count;
	char		*c;
	
	count=0;
	c=element->data;
	
	while (TRUE) {
		if (*c==0x0) return(count);
		c+=32;
		count++;
	}

	return(0);
}

void element_click_combo(element_type *element)
{
	element_open_combo_id=element->id;
}

void element_box_combo_open(element_type *element,int *lft,int *rgt,int *top,int *bot)
{
	int				y,cnt;

	cnt=element_get_combo_list_count(element);

	y=element->y-(element->high*element->value);
	if (y<element->high) y=element->high;
		
	*lft=element->x+10;
	*rgt=(*lft+element->wid)-element->high;
	*top=y-element->high;
	*bot=*top+(cnt*element->high);
}

bool element_in_combo_open(element_type *element,int x,int y)
{
	int				lft,rgt,top,bot;
	
	element_box_combo_open(element,&lft,&rgt,&top,&bot);
	if ((x<lft) || (x>rgt) || (y<top) || (y>bot)) return(FALSE);
	
	return(TRUE);
}

bool element_click_combo_open(element_type *element,int x,int y)
{
	int				lft,rgt,top,bot;

		// in box?
		
	element_box_combo_open(element,&lft,&rgt,&top,&bot);
	if ((x<lft) || (x>rgt) || (y<top) || (y>bot)) return(FALSE);
	
		// change clicked item and close
		
	element->value=(y-top)/element->high;
	
		// close open combo
		
	element_open_combo_id=-1;
	
	return(TRUE);
}

void element_draw_combo(element_type *element,int sel_id)
{
	int				x,y,ky,lft,rgt,top,bot;
	char			str[256];
	float			alpha;
	d3col			col,gradient_start,gradient_end;
	
	x=element->x;
	y=element->y;
	
		// label
	
	ky=y-(element->high>>1);

	gl_text_start(font_interface_index,iface.font.text_size_medium,FALSE);
	gl_text_draw((x-5),(ky-2),element->str,tx_right,TRUE,&iface.color.control.label,1.0f);
	gl_text_draw(x,(ky-3),":",tx_center,TRUE,&iface.color.control.label,1.0f);
	gl_text_end();
		
		// combo box
		
	lft=x+10;
	rgt=lft+element->wid;
	top=ky-(element->high>>1);
	bot=top+element->high;

	alpha=(element->enabled?1.0f:0.3f);

		// background
		
	memmove(&gradient_start,&iface.color.control.fill,sizeof(d3col));
	gradient_end.r=gradient_start.r*element_gradient_factor_background;
	gradient_end.g=gradient_start.g*element_gradient_factor_background;
	gradient_end.b=gradient_start.b*element_gradient_factor_background;

	view_primitive_2D_color_poly(lft,top,&gradient_start,rgt,top,&gradient_start,rgt,ky,&gradient_end,lft,ky,&gradient_end,alpha);
	view_primitive_2D_color_poly(lft,ky,&gradient_end,rgt,ky,&gradient_end,rgt,bot,&gradient_start,lft,bot,&gradient_start,alpha);

		// outline

	view_primitive_2D_line_quad(&iface.color.control.outline,alpha,lft,rgt,top,bot);
	if ((element->id==sel_id) && (element->enabled)) view_primitive_2D_line_quad(&iface.color.control.mouse_over,alpha,(lft-1),(rgt+1),(top-1),(bot+1));

		// arrow

	lft=(rgt-element->high)+4;
	rgt-=4;
	top+=4;
	bot-=4;

	view_primitive_2D_color_trig(&iface.color.control.hilite,alpha,lft,rgt,top,bot,2);

	col.r=col.g=col.b=0.0f;
	view_primitive_2D_line_trig(&col,alpha,lft,rgt,top,bot,2);
	
	lft-=4;
	top-=2;
	bot+=2;
	view_primitive_2D_line(&iface.color.control.outline,alpha,lft,top,lft,bot);

		// control text

	strcpy(str,(element->data+(element->value*32)));

	gl_text_start(font_interface_index,iface.font.text_size_medium,FALSE);
	gl_text_draw((x+15),(ky-2),str,tx_left,TRUE,&iface.color.control.text,alpha);
	gl_text_end();
}

void element_draw_combo_open(element_type *element)
{
	int				x,y,ky,n,cnt,lft,rgt,top,bot,sel_item_idx;
	char			str[256];
	d3col			gradient_start,gradient_end;
	
		// combo count

	cnt=element_get_combo_list_count(element);

		// get selected item

	input_gui_get_hilite_position(&x,&y);

	sel_item_idx=-1;

	element_box_combo_open(element,&lft,&rgt,&top,&bot);
	if ((x>=lft) && (x<=rgt) && (y>=top) && (y<=bot)) {
		sel_item_idx=(y-top)/element->high;
	}

		// drawing sizes

	x=element->x;
	y=element->y-(element->high*element->value);

	if (y<element->high) y=element->high;
	
	lft=x+10;
	rgt=(lft+element->wid)-element->high;
	top=y-element->high;

		// background
		
	memmove(&gradient_start,&iface.color.control.fill,sizeof(d3col));
	gradient_end.r=gradient_start.r*element_gradient_factor_background;
	gradient_end.g=gradient_start.g*element_gradient_factor_background;
	gradient_end.b=gradient_start.b*element_gradient_factor_background;

	bot=top+(cnt*element->high);

	view_primitive_2D_color_quad(&gradient_start,1.0f,lft,rgt,top,bot);

		// combo items

	for (n=0;n!=cnt;n++) {

		bot=top+element->high;

			// selection

		if (element->value==n) {
			memmove(&gradient_start,&iface.color.control.hilite,sizeof(d3col));
			gradient_end.r=gradient_start.r*element_gradient_factor_foreground;
			gradient_end.g=gradient_start.g*element_gradient_factor_foreground;
			gradient_end.b=gradient_start.b*element_gradient_factor_foreground;

			ky=(top+bot)>>1;
			view_primitive_2D_color_poly(lft,top,&gradient_start,rgt,top,&gradient_start,rgt,ky,&gradient_end,lft,ky,&gradient_end,1.0f);
			view_primitive_2D_color_poly(lft,ky,&gradient_end,rgt,ky,&gradient_end,rgt,bot,&gradient_start,lft,bot,&gradient_start,1.0f);
		}

			// text

		gl_text_start(font_interface_index,iface.font.text_size_medium,FALSE);
		strcpy(str,(element->data+(n*32)));

		if (sel_item_idx==n) {
			gl_text_draw((x+10),(((top+bot)>>1)-2),str,tx_left,TRUE,&iface.color.control.mouse_over,1.0f);
		}
		else {
			gl_text_draw((x+10),(((top+bot)>>1)-2),str,tx_left,TRUE,&iface.color.control.text,1.0f);
		}

		gl_text_end();

		top+=element->high;
	}

		// outline

	top=y-element->high;
	bot=top+(cnt*element->high);

	view_primitive_2D_line_quad(&iface.color.control.outline,1.0f,lft,rgt,top,bot);
	view_primitive_2D_line_quad(&iface.color.control.mouse_over,1.0f,(lft-1),(rgt+1),(top-1),(bot+1));
}

/* =======================================================

      Slider Element
      
======================================================= */

void element_click_slider(element_type *element,int x,int y)
{
	x-=element->x;
	if (x<0) x=0;
	if (x>element->wid) x=element->wid;
	
	element->setup.slider.value=((float)x)/((float)element->wid);
}

void element_move_slider(element_type *element,int x,int y)
{
	int				id;
	
	id=element_find_for_xy(x,y);
	if (id==element->id) element_click_slider(element,x,y);
}

void element_scroll_slider(element_type *element,bool up)
{
	if (up) {
		element->setup.slider.value-=0.1f;
		if (element->setup.slider.value<0.0f) element->setup.slider.value=0.0f;
	}
	else {
		element->setup.slider.value+=0.1f;
		if (element->setup.slider.value>1.0f) element->setup.slider.value=1.0f;
	}
}

void element_draw_slider(element_type *element,int sel_id)
{
	int				x,y,ky,lft,rgt,top,bot,mid,lx,rx;
	float			alpha;
	char			str[32];
	d3col			col2,gradient_start,gradient_end;
	
	x=element->x;
	y=element->y;
	
		// label
	
	ky=y-(element->high>>1);

	gl_text_start(font_interface_index,iface.font.text_size_medium,FALSE);
	gl_text_draw((x-5),(ky-2),element->str,tx_right,TRUE,&iface.color.control.label,1.0f);
	gl_text_draw(x,(ky-3),":",tx_center,TRUE,&iface.color.control.label,1.0f);
	gl_text_end();
	
		// slider size
		
	lft=x+10;
	rgt=lft+element->wid;
	top=ky-(element->high>>1);
	bot=top+element->high;
	
	mid=lft+(int)(((float)element->wid)*element->setup.slider.value);

	alpha=(element->enabled?1.0f:0.3f);
	
		// background
		
	memmove(&gradient_start,&iface.color.control.fill,sizeof(d3col));
	gradient_end.r=gradient_start.r*element_gradient_factor_background;
	gradient_end.g=gradient_start.g*element_gradient_factor_background;
	gradient_end.b=gradient_start.b*element_gradient_factor_background;

	ky=(top+bot)>>1;

	view_primitive_2D_color_poly(lft,top,&gradient_start,rgt,top,&gradient_start,rgt,ky,&gradient_end,lft,ky,&gradient_end,1.0f);
	view_primitive_2D_color_poly(lft,ky,&gradient_end,rgt,ky,&gradient_end,rgt,bot,&gradient_start,lft,bot,&gradient_start,1.0f);

		// slider value
		
	if (element->enabled) view_primitive_2D_color_quad(&iface.color.control.hilite,alpha,lft,mid,top,bot);

		// handle

	if (element->enabled) {
		lx=mid-(element->wid/20);
		rx=mid+(element->wid/20);

		if (lx<lft) {
			lx=lft;
			rx=lx+(element->wid/10);
		}
		if (rx>rgt) {
			lx=rgt-(element->wid/10);
			rx=rgt;
		}

		col2.r=iface.color.control.hilite.r*element_gradient_factor_foreground;
		col2.g=iface.color.control.hilite.g*element_gradient_factor_foreground;
		col2.b=iface.color.control.hilite.b*element_gradient_factor_foreground;
	
		view_primitive_2D_color_poly(lx,top,&iface.color.control.hilite,rx,top,&iface.color.control.hilite,rx,ky,&col2,lx,ky,&col2,alpha);
		view_primitive_2D_color_poly(lx,ky,&col2,rx,ky,&col2,rx,bot,&iface.color.control.hilite,lx,bot,&iface.color.control.hilite,alpha);
		
		view_primitive_2D_line_quad(&iface.color.control.outline,alpha,lx,rx,top,bot);
	}

		// outlines

	view_primitive_2D_line_quad(&iface.color.control.outline,alpha,lft,rgt,top,bot);
	if ((element->id==sel_id) && (element->enabled)) view_primitive_2D_line_quad(&iface.color.control.mouse_over,alpha,(lft-1),(rgt+1),(top-1),(bot+1));

		// text

	gl_text_start(font_interface_index,iface.font.text_size_medium,FALSE);
	sprintf(str,"%d%%",(int)(element->setup.slider.value*100.0f));
	gl_text_draw((rgt-5),(ky-1),str,tx_right,TRUE,&iface.color.control.text,1.0f);
	gl_text_end();
}

/* =======================================================

      Scrolling Controls for Tables and Text Elements
      
======================================================= */

int element_click_scroll_controls(element_type *element,int header_high,int x,int y,bool up_ok,bool down_ok)
{
	int				lft,rgt,top,bot,ctrl_sz;
	
		// element size
		// click size is a little bigger than
		// actual draw size

	ctrl_sz=element_get_control_scroll_size();
	ctrl_sz+=8;
		
	element_get_box(element,&lft,&rgt,&top,&bot);
	
	lft=(rgt-ctrl_sz)-8;
	top+=header_high;

		// top scroll bar

	if (up_ok) {
		
		if ((x>=(rgt-ctrl_sz)) && (x<=rgt) && (y>=top) && (y<=(top+ctrl_sz))) return(-1);
	}

		// bottom scroll bar

	if (down_ok) {
		if ((x>=(rgt-ctrl_sz)) && (x<=rgt) && (y>=(bot-ctrl_sz)) && (y<=bot)) return(1);
	}

	return(0);
}

void element_draw_scroll_controls(element_type *element,int header_high,bool up_ok,bool down_ok)
{
	int				lft,rgt,top,bot,ty,by,
					ctrl_sz;
	float			alpha;
	d3col			col,col2,col3;
	
		// element size

	ctrl_sz=element_get_control_scroll_size();
		
	element_get_box(element,&lft,&rgt,&top,&bot);
	
	lft=(rgt-ctrl_sz)-4;
	rgt-=4;
	top=top+(header_high+8);
	bot-=4;

		// colors

	memmove(&col,&iface.color.control.fill,sizeof(d3col));
	col2.r=col.r*element_gradient_factor_background;
	col2.g=col.g*element_gradient_factor_background;
	col2.b=col.b*element_gradient_factor_background;
	
	col3.r=col3.g=col3.b=0.0f;

		// top scroll bar

	alpha=up_ok?1.0f:0.1f;

	ty=top;
	by=top+ctrl_sz;

	view_primitive_2D_color_poly(lft,ty,&col2,rgt,ty,&col2,rgt,by,&col,lft,by,&col,alpha);
	view_primitive_2D_line_quad(&iface.color.control.outline,alpha,lft,rgt,ty,by);

	view_primitive_2D_color_trig(&iface.color.control.hilite,alpha,(lft+5),(rgt-5),(ty+5),(by-5),0);
	view_primitive_2D_line_trig(&col3,alpha,(lft+5),(rgt-5),(ty+5),(by-5),0);

		// bottom scroll bar

	alpha=down_ok?1.0f:0.1f;

	ty=bot-ctrl_sz;
	by=bot;

	view_primitive_2D_color_poly(lft,ty,&col,rgt,ty,&col,rgt,by,&col2,lft,by,&col2,alpha);
	view_primitive_2D_line_quad(&iface.color.control.outline,alpha,lft,rgt,ty,by);

	view_primitive_2D_color_trig(&iface.color.control.hilite,alpha,(lft+5),(rgt-5),(ty+5),(by-5),2);
	view_primitive_2D_line_trig(&col3,alpha,(lft+5),(rgt-5),(ty+5),(by-5),2);
}

/* =======================================================

      Table Element
      
======================================================= */

int element_get_table_row_count(element_type *element)
{
	int			count;
	char		*c;
	
	c=element->data;
	if (c==NULL) return(0);
	
	count=0;
	
	while (TRUE) {
		if (*c==0x0) return(count);
		c+=128;
		count++;
	}

	return(0);
}

int element_get_table_row_high(element_type *element)
{
	if (element->setup.table.bitmap_mode==element_table_bitmap_none) return(gl_text_get_char_height(iface.font.text_size_medium)+4);
	return((int)(((float)iface.scale_x)*element_table_bitmap_size)+4);
}

void element_click_table(element_type *element,int x,int y)
{
	int				high,row_high,row_cnt,cnt,vis_cnt,
					scroll_dir;
	bool			up_ok,down_ok;
	
		// get text sizes
		
	high=gl_text_get_char_height(iface.font.text_size_medium)+2;
	row_high=element_get_table_row_high(element);
	
	row_cnt=element_get_table_row_count(element);
	
		// check for clicking in scroll bar

	vis_cnt=cnt=((element->high-(high+4))/row_high)-1;
	if (((element->high-(high+4))%row_high)!=0) vis_cnt--;

	up_ok=(element->offset!=0);
	down_ok=((element->offset+(vis_cnt+1))<row_cnt);

	scroll_dir=element_click_scroll_controls(element,high,x,y,up_ok,down_ok);
	
	if (scroll_dir!=0) {
		element->offset+=((cnt+1)*scroll_dir);
		return;
	}
	
		// select cliked on element
		
	y-=((element->y+4)+(high+2));
	element->value=element->offset+(y/element_get_table_row_high(element));
	
	if ((element->value<0) || (element->value>=row_cnt)) {
		element->value=-1;
	}

		// flip any check boxes
		// and turn off selection

	if ((element->setup.table.checkbox) && (element->value>=0) && (element->value<element_table_max_check)) {
		element->setup.table.checks[element->value]^=0x1;
		element->value=-1;
	}
}

void element_scroll_table(element_type *element,bool up)
{
	int					high,row_high,scroll_cnt;

	if (up) {
		element->offset--;
		if (element->offset<0) element->offset=0;
		return;
	}

	high=gl_text_get_char_height(iface.font.text_size_medium)+2;
	row_high=element_get_table_row_high(element);
	scroll_cnt=element_get_table_row_count(element)-((element->high-(high+4))/row_high);
	if (scroll_cnt<0) scroll_cnt=0;

	element->offset++;
	if (element->offset>scroll_cnt) element->offset=scroll_cnt;
}

void element_draw_table_row_column_lines(element_type *element,int ty,int by,float col_factor)
{
	int			n,x;
	float		f_wid;
	d3col		col;
	
	x=element->x;
	f_wid=(float)element->wid;
	
	col.r=iface.color.control.outline.r*col_factor;
	col.g=iface.color.control.outline.g*col_factor;
	col.b=iface.color.control.outline.b*col_factor;

	for (n=1;n<element->setup.table.ncolumn;n++) {
		x+=(int)(element->setup.table.cols[n-1].percent_size*f_wid);
		view_primitive_2D_line(&col,1.0f,x,ty,x,by);
	}
}

void element_draw_table_header_fill(element_type *element,int high)
{
	int				lft,rgt,top,bot,y;
	d3col			col2;
	
		// header
		
	element_get_box(element,&lft,&rgt,&top,&bot);
	
	bot=(top+high)+4;
	y=(top+bot)>>1;
	
	col2.r=iface.color.control.header.r*element_gradient_factor_background;
	col2.g=iface.color.control.header.g*element_gradient_factor_background;
	col2.b=iface.color.control.header.b*element_gradient_factor_background;
	
	view_primitive_2D_color_poly(lft,top,&col2,rgt,top,&col2,rgt,y,&iface.color.control.header,lft,y,&iface.color.control.header,1.0f);
	view_primitive_2D_color_poly(lft,y,&iface.color.control.header,rgt,y,&iface.color.control.header,rgt,bot,&col2,lft,bot,&col2,1.0f);
	
	element_draw_table_row_column_lines(element,top,bot,1.0f);
}

void element_draw_table_line_header(element_type *element,int x,int y,int wid,int row_high)
{
	int			n;
	d3col		col;
	
	col.r=col.g=col.b=0.0f;

	y+=((row_high>>1)-3);
	
	for (n=0;n!=element->setup.table.ncolumn;n++) {
		gl_text_start(font_interface_index,iface.font.text_size_medium,FALSE);
		gl_text_draw((x+4),y,element->setup.table.cols[n].name,tx_left,TRUE,&col,1.0f);
		gl_text_end();
		
		x+=(int)(element->setup.table.cols[n].percent_size*(float)wid);
	}
}

unsigned long element_draw_table_get_image_gl_id(element_type *element,int row_idx)
{
	int				n,idx;
	char			*c,*c2,subdir[256],filename[256],path[1024];
	struct stat		sb;
	
		// get path
		
	c=element->data+(row_idx*128);
	
	strcpy(subdir,c);
	c2=strchr(subdir,';');
	if (c2==NULL) return(-1);

	*c2=0x0;
	strcpy(filename,(c2+1));

	c2=strchr(filename,';');
	if (c2==NULL) return(-1);

	*c2=0x0;

		// blanks are always missing

	if (filename[0]==0x0) return(-1);

		// get path

	if (element->setup.table.bitmap_mode==element_table_bitmap_data) {
		file_paths_data(&file_path_setup,path,subdir,filename,"png");
	}
	else {
		file_paths_app_data(&file_path_setup,path,subdir,filename,"png");
	}
	
		// is it already loaded?
		
	for (n=0;n!=element_table_max_image;n++) {
		if (element->setup.table.images[n].path[0]==0x0) continue;
		if (strcmp(element->setup.table.images[n].path,path)==0) return(element->setup.table.images[n].bitmap.gl_id);
	}

		// does it exist?

	if (stat(path,&sb)!=0) return(-1);
	
		// need to load, find open bitmap
		
	idx=-1;
	
	for (n=0;n!=element_table_max_image;n++) {
		if (element->setup.table.images[n].path[0]==0x0) {
			idx=n;
			break;
		}
	}
	
		// no spots left, replace a image
		
	if (idx==-1) {
		idx=element->setup.table.next_image_idx;
		element->setup.table.next_image_idx++;
		if (element->setup.table.next_image_idx==element_table_max_image) element->setup.table.next_image_idx=0;
	}
	
		// open
		
	strcpy(element->setup.table.images[idx].path,path);

	bitmap_open(&element->setup.table.images[idx].bitmap,path,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE);
	return(element->setup.table.images[idx].bitmap.gl_id);
}

void element_draw_table_line_data_text(int x,int y,int row_high,d3col *txt_col,char *txt)
{
	int				dy,high;
	char			*c;
	
	high=gl_text_get_char_height(iface.font.text_size_medium);
	
	gl_text_start(font_interface_index,iface.font.text_size_medium,FALSE);
	
		// any returns?
		
	c=strchr(txt,'\n');
	if (c==NULL) {
		dy=y+((row_high>>1)-3);
		gl_text_draw(x,(dy+2),txt,tx_left,TRUE,txt_col,1.0f);
	}
	else {
		*c=0x0;
		
		dy=y+(high+8);
		gl_text_draw(x,dy,txt,tx_left,FALSE,txt_col,1.0f);
		gl_text_draw(x,(dy+(high+2)),(c+1),tx_left,FALSE,txt_col,1.0f);
	}
		
	gl_text_end();
}

void element_draw_table_line_data(element_type *element,int x,int y,int row,int wid,int row_high,d3col *txt_col,char *data)
{
	int				n,dx,dy,tx,col_wid,bitmap_sz;
	unsigned long	gl_id;
	char			*c,*c2,txt[256];
	bool			first_col,checked;
	d3col			col,col2;

	dx=x+4;
	dy=y+((row_high>>1)-3);

		// data column

	c=data;
	first_col=TRUE;
	
	for (n=0;n!=element->setup.table.ncolumn;n++) {

			// checkboxes

		if ((element->setup.table.checkbox) && (n==0)) {

			col_wid=(int)(element->setup.table.cols[n].percent_size*(float)wid);

			checked=FALSE;
			if ((row>=0) && (row<element_table_max_check)) checked=element->setup.table.checks[row];

			element_draw_checkbox_control((dx+4),dy,(col_wid-20),checked,TRUE,FALSE);

			dx+=col_wid;
			continue;
		}
	
			// get current data
			
		strncpy(txt,c,255);
		txt[255]=0x0;
		c2=strchr(txt,'\t');
		if (c2!=NULL) *c2=0x0;
		
			// text draw spot
			
		tx=dx;

			// draw any bitmaps

		if ((element->setup.table.bitmap_mode!=element_table_bitmap_none) && (first_col)) {

			bitmap_sz=(int)(((float)iface.scale_x)*element_table_bitmap_size);

				// draw bitmap

			gl_id=element_draw_table_get_image_gl_id(element,row);

			if (gl_id!=-1) {
				view_primitive_2D_texture_quad(gl_id,NULL,1.0f,dx,(dx+bitmap_sz),(y+2),((y+2)+bitmap_sz),0.0f,1.0f,0.0f,1.0f,TRUE);
				view_primitive_2D_line_quad(&iface.color.control.outline,1.0f,dx,(dx+bitmap_sz),(y+2),((y+2)+bitmap_sz));
			}

				// missing graphic

			else {
				
				col.r=col.g=col.b=0.6f;
				col2.r=col2.g=col2.b=0.4f;

				view_primitive_2D_color_poly(dx,(y+2),&col,(dx+bitmap_sz),(y+2),&col,(dx+bitmap_sz),((y+2)+bitmap_sz),&col2,dx,((y+2)+bitmap_sz),&col2,1.0f);
				view_primitive_2D_line_quad(&iface.color.control.outline,1.0f,dx,(dx+bitmap_sz),(y+2),((y+2)+bitmap_sz));

				col.r=col.g=col.b=1.0f;

				gl_text_start(font_interface_index,iface.font.text_size_large,FALSE);
				gl_text_draw((dx+(bitmap_sz>>1)),(y+(bitmap_sz>>1)),"?",tx_center,TRUE,&col,1.0f);
				gl_text_end();
			}

				// get to correct text

			c2=strchr(txt,';');
			if (c2!=NULL) {
				c2=strchr((c2+1),';');
				if (c2!=NULL) {
					strcpy(txt,(c2+1));
				}
			}

			tx+=(bitmap_sz+4);
		}

		first_col=FALSE;
		
			// draw text
			
		element_draw_table_line_data_text(tx,y,row_high,txt_col,txt);
		
			// get next data
			
		c=strchr(c,'\t');
		if (c==NULL) break;
		c++;
		
		dx+=(int)(element->setup.table.cols[n].percent_size*(float)wid);
	}
}

void element_draw_table_line_background(element_type *element,int idx,int lft,int rgt,int top,int bot)
{
	d3col			col;

	memmove(&col,&iface.color.control.fill,sizeof(d3col));

	if (((idx+element->offset)&0x1)==0) {
		col.r*=element_gradient_factor_table_line_1;
		col.g*=element_gradient_factor_table_line_1;
		col.b*=element_gradient_factor_table_line_1;
	}
	else {
		col.r*=element_gradient_factor_table_line_2;
		col.g*=element_gradient_factor_table_line_2;
		col.b*=element_gradient_factor_table_line_2;
	}
		
	view_primitive_2D_color_poly(lft,top,&col,rgt,top,&col,rgt,bot,&col,lft,bot,&col,1.0f);
}

void element_draw_table_busy(element_type *element)
{
	int				n,idx,lft,rgt,top,bot,high;
	float			start_perc,end_perc;
	d3col			in_hilite,in_background;

	element_get_box(element,&lft,&rgt,&top,&bot);

	high=(gl_text_get_char_height(iface.font.text_size_medium)*2)+10;
	
	lft=((lft+rgt)>>1)-(high/2);
	rgt=lft+high;
	top=((top+bot)>>1)-(high/2);
	bot=top+high;

		// selected arc

	idx=(game_time_get_raw()>>7)&0xF;
	
		// inner colors
	
	in_hilite.r=iface.color.control.hilite.r*element_gradient_factor_foreground;
	in_hilite.g=iface.color.control.hilite.g*element_gradient_factor_foreground;
	in_hilite.b=iface.color.control.hilite.b*element_gradient_factor_foreground;
	
	in_background.r=iface.color.control.fill.r*element_gradient_factor_foreground;
	in_background.g=iface.color.control.fill.g*element_gradient_factor_foreground;
	in_background.b=iface.color.control.fill.b*element_gradient_factor_foreground;

		// spinning busy wheel

	for (n=0;n!=16;n++) {
		start_perc=((float)n)/16.0f;
		end_perc=((float)(n+1))/16.0f;

		if (n==idx) {
			view_primitive_2D_color_arc(&iface.color.control.hilite,&in_hilite,0.7f,lft,rgt,top,bot,start_perc,end_perc);
		}
		else {
			view_primitive_2D_color_arc(&iface.color.control.fill,&in_background,0.7f,lft,rgt,top,bot,start_perc,end_perc);
		}
	}
}

void element_draw_table(element_type *element,int sel_id)
{
	int				n,x,y,ky,wid,high,cnt,vis_cnt,
					lft,rgt,top,bot,row_high,
					lx,rx,ty,by,last_idx;
	char			*c;
	bool			up_ok,down_ok;
	d3col			col,col2;

		// sizes
	
	wid=element->wid;
	high=gl_text_get_char_height(iface.font.text_size_medium)+2;
	row_high=element_get_table_row_high(element);
	
		// get element counts
		
	vis_cnt=cnt=((element->high-(high+4))/row_high);
	if (((element->high-(high+4))%row_high)!=0) vis_cnt--;
	
		// header fill
		
	element_draw_table_header_fill(element,high);
		
		// outline
		
	element_get_box(element,&lft,&rgt,&top,&bot);
	view_primitive_2D_line_quad(&iface.color.control.outline,1.0f,lft,rgt,top,bot);
	
	element_get_box(element,&lft,&rgt,&top,&bot);
	top+=(high+4);

	view_primitive_2D_line(&iface.color.control.outline,1.0f,lft,top,rgt,top);

	if (element->id==sel_id) view_primitive_2D_line_quad(&iface.color.control.mouse_over,1.0f,(lft-1),(rgt+1),(top-(high+5)),(bot+2));

		// text positions
		
	x=element->x;
	y=element->y+3;
	
		// header
		
	element_draw_table_line_header(element,x,y,wid,high);
	
		// items
		
	if (element->data!=NULL) {

			// scissor so we can draw partically
			// obscured rows

		element_get_box(element,&lx,&rx,&ty,&by);
		gl_2D_scissor_start(lx,rx,ty,by);
	
			// draw the data lines

		last_idx=0;
		y=(element->y+4)+(high+1);
		
		c=element->data+(element->offset*128);
		
		for (n=0;n<=cnt;n++) {
			if (*c==0x0) {
				last_idx=n;
				break;
			}
			
				// selection or background

			element_get_box(element,&lft,&rgt,&top,&bot);
			
			lft+=1;
			rgt-=1;
			top=y;
			bot=y+row_high;
				
			if (((n+element->offset)==element->value) && (!element->setup.table.checkbox)) {
				memmove(&col,&iface.color.control.hilite,sizeof(d3col));
				col2.r=col.r*element_gradient_factor_foreground;
				col2.g=col.g*element_gradient_factor_foreground;
				col2.b=col.b*element_gradient_factor_foreground;

				ky=(top+bot)>>1;

				view_primitive_2D_color_poly(lft,top,&col2,rgt,top,&col2,rgt,ky,&col,lft,ky,&col,1.0f);
				view_primitive_2D_color_poly(lft,ky,&col,rgt,ky,&col,rgt,bot,&col2,lft,bot,&col2,1.0f);
			}
			else {
				element_draw_table_line_background(element,n,lft,rgt,top,bot);
			}

			element_draw_table_row_column_lines(element,top,bot,0.5f);

				// table line data
				
			element_draw_table_line_data(element,x,y,(element->offset+n),wid,row_high,&iface.color.control.text,c);
			
			c+=128;
			y+=row_high;
		}

			// finish with blank rows

		while (y<by) {
			element_get_box(element,&lft,&rgt,&top,&bot);
			element_draw_table_line_background(element,last_idx,(lft+1),(rgt-1),y,(y+row_high));
			element_draw_table_row_column_lines(element,y,(y+row_high),0.5f);
			
			last_idx++;
			y+=row_high;
		}

		gl_2D_scissor_end();
	}

		// scroll controls

	up_ok=(element->offset!=0);
	down_ok=((element->offset+(vis_cnt+1))<element_get_table_row_count(element));

	element_draw_scroll_controls(element,high,up_ok,down_ok);
	
		// busy
		
	if (element->setup.table.busy) element_draw_table_busy(element);
}

/* =======================================================

      Tab Element
      
======================================================= */

int element_mouse_over_tab(element_type *element,int x,int y)
{
	int				max_sz,xadd,idx;
	
		// within tab box?
		
	if ((x<element->x) || (x>(element->x+element->wid))) return(-1);
	if ((y<element->y) || (y>(element->y+element->high))) return(-1);
	
		// select value
		
	xadd=element->wid/element->setup.tab.ntab;
	max_sz=(int)(((float)iface.scale_x)*0.2f);
	if (xadd>max_sz) xadd=max_sz;

	idx=(x-element->x)/xadd;
	if ((idx<0) || (idx>=element->setup.tab.ntab)) return(-1);

	return(idx);
}

bool element_click_tab(element_type *element,int x,int y)
{
	int				value;
	
	value=element_mouse_over_tab(element,x,y);
	if (value==-1) return(FALSE);
	
	element->value=value;
	
	return(TRUE);
}

void element_draw_tab(element_type *element,int sel_id)
{
	int				n,x,y,xadd,x_slant,x_overlap,y_push,
					max_sz,mouse_idx,tab_idx,
					lx,rx,ty,by,margin;
	int				tab_draw_list[max_element_tab];
	d3col			*col_ptr;
	
		// tab sizes
	
	xadd=element->wid/element->setup.tab.ntab;
	max_sz=(int)(((float)iface.scale_x)*0.2f);
	if (xadd>max_sz) xadd=max_sz;

	margin=element_get_margin();

		// get mouse over element
	
	input_gui_get_hilite_position(&x,&y);
	mouse_idx=element_mouse_over_tab(element,x,y);

		// pick drawing order (last to first, selected always on top)
		
	tab_idx=0;
	
	for (n=(element->setup.tab.ntab-1);n>=0;n--) {
		if (n!=element->value) tab_draw_list[tab_idx++]=n;
	}
	
	tab_draw_list[tab_idx]=element->value;
	
		// tabs
		
	x_slant=(int)(((float)xadd)*0.025f);
	y_push=margin>>1;
	
	ty=element->y;
	by=ty+element->high;
	
	for (n=0;n!=element->setup.tab.ntab;n++) {
	
		tab_idx=tab_draw_list[n];

		lx=element->x+(xadd*tab_idx);
		rx=lx+xadd;
		
		if (tab_idx!=(element->setup.tab.ntab-1)) {
			x_overlap=x_slant/2;
		}
		else {
			x_overlap=0;
		}
		
		y=ty;

		if (element->value!=tab_idx) {
			col_ptr=&iface.color.tab.dimmed;
		}
		else {
			y-=y_push;
			col_ptr=&iface.color.tab.background;
		}
		
		view_primitive_2D_color_poly((lx+x_slant),y,col_ptr,((rx-x_slant)+x_overlap),y,col_ptr,(rx+x_overlap),by,col_ptr,lx,by,col_ptr,1.0f);

		view_primitive_2D_line(&iface.color.tab.outline,1.0f,lx,by,(lx+x_slant),y);
		view_primitive_2D_line(&iface.color.tab.outline,1.0f,(lx+x_slant),y,((rx-x_slant)+x_overlap),y);
		view_primitive_2D_line(&iface.color.tab.outline,1.0f,((rx-x_slant)+x_overlap),y,(rx+x_overlap),by);

		col_ptr=&iface.color.tab.text_dimmed;
		
		if (element->value==tab_idx) {
			col_ptr=&iface.color.tab.text;
		}
		
		if (mouse_idx==tab_idx) {
			col_ptr=&iface.color.tab.text_mouse_over;
		}

		gl_text_start(font_interface_index,iface.font.text_size_medium,FALSE);
		gl_text_draw(((lx+rx)>>1),((y+by)>>1),element->setup.tab.name[tab_idx],tx_center,TRUE,col_ptr,1.0f);
		gl_text_end();
	}

		// final underline

	lx=element->x+(xadd*element->value);
	rx=lx+xadd;
	if (element->value!=0) view_primitive_2D_line(&iface.color.tab.outline,1.0f,element->x,by,lx,by);
	view_primitive_2D_line(&iface.color.tab.outline,1.0f,rx,by,(element->x+element->wid),by);
}

/* =======================================================

      Text Box Element
      
======================================================= */

void element_click_text_box(element_type *element,int x,int y)
{
	int				high,page_count,scroll_dir;

		// no click if error display

	if (element->setup.text_box.error_display) return;

		// run the scroll click

	scroll_dir=element_click_scroll_controls(element,0,x,y,element->setup.text_box.scroll_up_ok,element->setup.text_box.scroll_down_ok);
	if (scroll_dir==0) return;

	high=gl_text_get_char_height(iface.font.text_size_medium);
	page_count=(element->high-5)/high;

	element->offset+=(scroll_dir*page_count);
}

void element_draw_text_box(element_type *element)
{
	int				lft,rgt,top,bot,x,y,wid,high,
					wrap_wid,idx,last_space_idx,
					line_count,line_offset;
	char			*c,str[256];
	bool			line_break;
	d3col			col;
	
	element_get_box(element,&lft,&rgt,&top,&bot);
	
	high=gl_text_get_char_height(iface.font.text_size_medium);
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	glDisable(GL_BLEND);
	
		// background

	view_primitive_2D_color_quad(&iface.color.control.fill,1.0f,lft,rgt,top,bot);
	
		// scrolling flags
		
	element->setup.text_box.scroll_up_ok=(element->offset!=0);
	element->setup.text_box.scroll_down_ok=FALSE;
	
		// any text?
		
	if (element->data==NULL) return;

		// the color

	if (!element->setup.text_box.error_display) {
		memmove(&col,&iface.color.control.text,sizeof(d3col));
	}
	else {
		col.r=1.0f;
		col.g=col.b=0.1f;
	}
	
		// wrap width
		
	wrap_wid=element->wid-(element_get_control_scroll_size()+8);

		// draw text
		
	x=lft+5;
	y=element->y+(high+5);

	gl_text_start(font_interface_index,iface.font.text_size_medium,FALSE);

	idx=0;
	last_space_idx=-1;
	line_count=0;
	
	line_offset=element->offset;
	
	c=element->data;
	
	while (TRUE) {
	
			// have we reached the end?
			
		if (*c==0x0) {
			if ((y<bot) && (line_offset==0)) {
				str[idx]=0x0;
				gl_text_draw(lft,y,str,tx_left,FALSE,&col,1.0f);
			}
			line_count++;
			break;
		}
		
			// check to see if there's a line break
	
		line_break=FALSE;
	
		if (*c=='\n') {
			line_break=TRUE;
		}
		else {
			if (*c<0x20) {		// skip all control characters
				c++;
				continue;
			}
		}
		
			// time for a wrap?
			
		str[idx]=0x0;
		
		wid=gl_text_get_string_width(font_interface_index,iface.font.text_size_medium,FALSE,str);
		
		if (wid>=wrap_wid) {
			line_break=TRUE;
			
			if (last_space_idx!=-1) {
				str[last_space_idx]=0x0;		// backup past the wrapping text
				c-=(idx-last_space_idx);
				idx=last_space_idx;
			}
		}
		
			// special break if too many characters
			
		if (idx==255) line_break=TRUE;
		
			// draw or add up line
		
		if (line_break) {
		
			if ((y<bot) && (line_offset==0)) gl_text_draw(x,y,str,tx_left,FALSE,&col,1.0f);
		
			idx=0;
			last_space_idx=-1;
			
			line_count++;
			
			if (line_offset>0) {
				line_offset--;
			}
			else {
				y+=high;
				if (y>=bot) {
					element->setup.text_box.scroll_down_ok=TRUE;
				}
			}
		}
		else {
			str[idx]=*c;
			
			if (*c==0x20) last_space_idx=idx;
			
			idx++;
		}
		
		c++;
	}

	gl_text_end();
	
	element->setup.text_box.line_count=line_count;
	
		// scrollbar

	if (!element->setup.text_box.error_display) element_draw_scroll_controls(element,0,element->setup.text_box.scroll_up_ok,element->setup.text_box.scroll_down_ok);
	
		// outline

	view_primitive_2D_line_quad(&iface.color.control.outline,1.0f,lft,rgt,top,bot);
}

/* =======================================================

      Info Field Elements
      
======================================================= */

void element_draw_info_field(element_type *element)
{
	int				x,y,ky;
	
	x=element->x;
	y=element->y;
	
	ky=y-(element->high>>1);
		
	gl_text_start(font_interface_index,iface.font.text_size_medium,FALSE);
	gl_text_draw((x-5),(ky-3),element->str,tx_right,TRUE,&iface.color.control.label,1.0f);
	gl_text_draw(x,(ky-2),":",tx_center,TRUE,&iface.color.control.label,1.0f);
	gl_text_draw((x+5),(ky-3),element->value_str,tx_left,TRUE,&iface.color.control.label,1.0f);
	gl_text_end();
}

/* =======================================================

      Model Elements
      
======================================================= */

void element_draw_model(element_type *element)
{
	// supergumba -- will need ray trace version
}

/* =======================================================

      Frame Elements
      
======================================================= */

void element_draw_frame(element_type *element)
{
	int				y,lft,rgt,top,mid,bot;
	bool			is_header;
	d3col			col,col2;
	
		// header?
		
	is_header=element->str[0]!=0x0;
	
		// setup draw

	lft=element->x;
	rgt=lft+element->wid;
	top=element->y;
	bot=top+element->high;
	mid=top+element_get_frame_title_high();
	
		// inside frame

	view_primitive_2D_color_quad(&iface.color.dialog.background,1.0f,lft,rgt,top,bot);
	
		// header
		
	if (is_header) {
		y=(top+mid)>>1;

		if (element->enabled) {
			memmove(&col,&iface.color.dialog.header,sizeof(d3col));
		}
		else {
			memmove(&col,&iface.color.dialog.background,sizeof(d3col));
		}
		
		col2.r=col.r*element_gradient_factor_foreground;
		col2.g=col.g*element_gradient_factor_foreground;
		col2.b=col.b*element_gradient_factor_foreground;

		view_primitive_2D_color_poly(lft,top,&col2,rgt,top,&col2,rgt,y,&col,lft,y,&col,1.0f);
		view_primitive_2D_color_poly(lft,y,&col,rgt,y,&col,rgt,mid,&col2,lft,mid,&col2,1.0f);

		gl_text_start(font_interface_index,iface.font.text_size_medium,FALSE);
		gl_text_draw(((lft+rgt)/2),(y-2),element->str,tx_center,TRUE,&iface.color.dialog.title,1.0f);
		gl_text_end();
	}
	
		// outline

	view_primitive_2D_line_quad(&iface.color.dialog.outline,1.0f,lft,rgt,top,bot);
	if (is_header) view_primitive_2D_line_quad(&iface.color.dialog.outline,1.0f,lft,rgt,top,mid);
}

/* =======================================================

      Draw Elements
      
======================================================= */

int element_draw_item_under_cursor(void)
{
	int			id,x,y;

	id=-1;

		// find hilited item

	if (element_open_text_field_id!=-1) {
		id=element_open_text_field_id;
	}
	else {
		if (element_open_combo_id!=-1) {
			id=element_open_combo_id;
		}
		else {
			input_gui_get_hilite_position(&x,&y);
			id=element_find_for_xy(x,y);
		}
	}

		// if clicking and holding, then we
		// can only hilite clicked item

	if (element_click_down_id!=-1) {
		if (id!=element_click_down_id) id=-1;
	}

	return(id);
}

void element_draw_lock(bool cursor_hilite)
{
	int					n,id;
	element_type		*element;
	
		// get element under cursor
		// only works if app active

	id=-1;
	if ((cursor_hilite) && (app.state==as_active)) id=element_draw_item_under_cursor();

		// setup draw

	glDisable(GL_BLEND);

	glLineWidth(1.0f);
	
		// draw elements
				
	element=elements;
	
	for (n=0;n<nelement;n++) {

		if (element->hidden) {
			element++;
			continue;
		}
		
			// draw element
	
		switch (element->type) {
		
			case element_type_button:
				element_draw_button(element,id);
				break;
			case element_type_bitmap:
				element_draw_bitmap(element);
				break;
			case element_type_text:
				element_draw_text(element,id);
				break;
			case element_type_text_field:
				element_draw_text_field(element,id);
				break;
			case element_type_number:
				element_draw_number(element,id);
				break;
			case element_type_checkbox:
				element_draw_checkbox(element,id);
				break;
			case element_type_combo:
				element_draw_combo(element,id);
				break;
			case element_type_slider:
				element_draw_slider(element,id);
				break;
			case element_type_table:
				element_draw_table(element,id);
				break;
			case element_type_tab:
				element_draw_tab(element,id);
				break;
			case element_type_text_box:
				element_draw_text_box(element);
				break;
			case element_type_info_field:
				element_draw_info_field(element);
				break;
			case element_type_model:
				element_draw_model(element);
				break;
			case element_type_count:
				element_draw_count(element);
				break;
			case element_type_frame:
				element_draw_frame(element);
				break;
				
		}
		
		element++;
	}
	
		// draw open combo
		
	if (element_open_combo_id!=-1) {
		element_draw_combo_open(element_find(element_open_combo_id));
	}
}

void element_draw(bool cursor_hilite)
{
	SDL_mutexP(element_thread_lock);
	element_draw_lock(cursor_hilite);
	SDL_mutexV(element_thread_lock);
}

/* =======================================================

      Click Elements
      
======================================================= */

void element_click_down_lock(int x,int y)
{
	element_type		*element;

		// already in a down click, if so,
		// check if we are in a slider

	if (element_click_down_id!=-1) {
		element=element_find(element_click_down_id);
		if (element->type==element_type_slider) {
			element_move_slider(element,x,y);
		}
		return;
	}

		// in open combo?

	if (element_open_combo_id!=-1) {
	
		element=element_find(element_open_combo_id);
		if (element_in_combo_open(element,x,y)) {
			element_click_down_id=element->id;
			return;
		}

		element_open_combo_id=-1;
	}
	
		// get click

	element_click_down_id=element_find_for_xy(x,y);

		// if open text, outside down clicks close

	if (element_open_text_field_id!=-1) {
		if (element_click_down_id!=element_open_text_field_id) element_open_text_field_id=-1;
	}
}

int element_click_up_lock(int x,int y)
{
	int					id;
	element_type		*element;
	
		// no click down ID
		
	if (element_click_down_id==-1) return(-1);
	
		// in open combo?
		
	if (element_open_combo_id!=-1) {

		element=element_find(element_open_combo_id);
		if (element_click_combo_open(element,x,y)) {
			element_click_down_id=-1;
			return(element->id);
		}

		element_open_combo_id=-1;
	}

		// find element

	id=element_find_for_xy(x,y);
	if (id!=element_click_down_id) {
		element_click_down_id=-1;
		return(-1);
	}
	
	element=element_find(id);

		// ignore disabled elements

	if (!element->enabled) {
		element_click_down_id=-1;
		return(-1);
	}
	
		// click it
		
	switch (element->type) {
		
		case element_type_text_field:
			element_click_text_field(element);
			break;
		case element_type_number:
			element_click_number(element,x,y);
			break;
		case element_type_checkbox:
			element_click_checkbox(element);
			break;
		case element_type_combo:
			element_click_combo(element);
			break;
		case element_type_slider:
			element_click_slider(element,x,y);
			break;
		case element_type_table:
			element_click_table(element,x,y);
			break;
		case element_type_tab:
			if (!element_click_tab(element,x,y)) {
				element_click_down_id=-1;
				return(-1);
			}
			break;
		case element_type_text_box:
			element_click_text_box(element,x,y);
			break;
			
	}
	
		// turn off down click
		
	element_click_down_id=-1;
	
	return(id);
}

void element_click_down(int x,int y)
{
	SDL_mutexP(element_thread_lock);
	element_click_down_lock(x,y);
	SDL_mutexV(element_thread_lock);
}

int element_click_up(int x,int y)
{
	int			id;

	SDL_mutexP(element_thread_lock);
	id=element_click_up_lock(x,y);
	SDL_mutexV(element_thread_lock);

	return(id);
}

/* =======================================================

      Scroll Elements
      
======================================================= */

void element_scroll_wheel_lock(int x,int y,bool up)
{
	int					id;
	element_type		*element;

	id=element_find_for_xy(x,y);
	if (id==-1) return;

	element=element_find(id);
	
	switch (element->type) {
		case element_type_table:
			element_scroll_table(element,up);
			break;

		case element_type_slider:
			element_scroll_slider(element,up);
			break;
	
	}
}

void element_scroll_wheel(int x,int y,bool up)
{
	SDL_mutexP(element_thread_lock);
	element_scroll_wheel_lock(x,y,up);
	SDL_mutexV(element_thread_lock);
}

/* =======================================================

      Key Elements
      
======================================================= */

int element_key_lock(char ch)
{
	int					len;
	element_type		*element;

		// text field open?

	if (element_open_text_field_id==-1) return(-1);

	element=element_find(element_open_text_field_id);

		// returns or tabs close box

	if ((ch==0xD) || (ch==0x9)) {
		element_open_text_field_id=-1;
		return(element->id);
	}

		// get text field

	len=strlen(element->value_str);

		// backspace?

	if (ch==0x8) {
		if (len==0) return(-1);
		element->value_str[len-1]=0x0;
		return(element->id);
	}

		// regular text

	if (len>=(element->max_value_str_sz-1)) return(-1);

	element->value_str[len]=ch;
	element->value_str[len+1]=0x0;

	return(element->id);
}

int element_key(char ch)
{
	int			rtn;

	SDL_mutexP(element_thread_lock);
	rtn=element_key_lock(ch);
	SDL_mutexV(element_thread_lock);

	return(rtn);
}

/* =======================================================

      Find Hilite
      
======================================================= */

int element_get_selected(void)
{
	int			x,y;

	if (element_open_text_field_id!=-1) return(element_open_text_field_id);
	if (element_open_combo_id!=-1) return(element_open_combo_id);

	input_gui_get_hilite_position(&x,&y);
				
	return(element_find_for_xy(x,y));
}

/* =======================================================

      Values
      
======================================================= */

int element_get_value(int id)
{
	int				rtn;
	element_type	*element;

	SDL_mutexP(element_thread_lock);

	element=element_find(id);
	if (element==NULL) {
		rtn=-1;
	}
	else {
		rtn=element->value;
	}

	SDL_mutexV(element_thread_lock);

	return(rtn);
}

float element_get_slider_value(int id)
{
	float			rtn;
	element_type	*element;

	SDL_mutexP(element_thread_lock);

	element=element_find(id);
	if (element==NULL) {
		rtn=0.0f;
	}
	else {
		rtn=element->setup.slider.min+((element->setup.slider.max-element->setup.slider.min)*element->setup.slider.value);
	}

	SDL_mutexV(element_thread_lock);

	return(rtn);
}

void element_get_value_string(int id,char *str)
{
	element_type	*element;

	SDL_mutexP(element_thread_lock);

	str[0]=0x0;

	element=element_find(id);
	if (element!=NULL) strcpy(str,element->value_str);

	SDL_mutexV(element_thread_lock);
}

void element_set_value(int id,int value)
{
	element_type	*element;
	
	SDL_mutexP(element_thread_lock);

	element=element_find(id);
	if (element!=NULL) element->value=value;

	SDL_mutexV(element_thread_lock);
}

void element_set_slider_value(int id,float value)
{
	element_type	*element;
	
	SDL_mutexP(element_thread_lock);

	element=element_find(id);
	if (element!=NULL) element->setup.slider.value=(value-element->setup.slider.min)/(element->setup.slider.max-element->setup.slider.min);

	SDL_mutexV(element_thread_lock);
}

void element_set_value_string(int id,char *str)
{
	element_type	*element;

	SDL_mutexP(element_thread_lock);

	element=element_find(id);
	if (element!=NULL) strcpy(element->value_str,str);

	SDL_mutexV(element_thread_lock);
}

/* =======================================================

      Table Data
      
======================================================= */

void element_sort_table_data(element_type *element)
{
	int				n,k,idx,row_count,sz,m_sz;
	char			*c,*c_cmp,*c2,*c2_cmp,*data;

	row_count=element_get_table_row_count(element);
	if (row_count==0) return;

	sz=(row_count+1)*128;

	data=malloc(sz);
	bzero(data,sz);

	for (n=0;n!=row_count;n++) {
		c=element->data+(n*128);

		c_cmp=strrchr(c,';');
		if (c_cmp==NULL) c_cmp=c;

		idx=-1;

		for (k=0;k!=n;k++) {

			c2=data+(k*128);

			c2_cmp=strrchr(c2,';');
			if (c2_cmp==NULL) c2_cmp=c;

			if (strcasecmp(c_cmp,c2_cmp)<=0) {
				idx=k;
				break;
			}
		}

		if (idx==-1) {
			c2=data+(n*128);
			memmove(c2,c,128);
		}
		else {
			c2=data+(idx*128);
			m_sz=(row_count-idx)*128;
			memmove((c2+128),c2,m_sz);
			memmove(c2,c,128);
		}
	}

	free(element->data);
	element->data=data;
}

void element_set_table_data(int id,bool sort,char *row_data)
{
	int				idx,sz;
	element_type	*element;
	
	idx=element_get_value(id);

	SDL_mutexP(element_thread_lock);

	element=element_find(id);
	if (element!=NULL) {
			
		if (element->data!=NULL) free(element->data);

		if (row_data==NULL) {
			element->data=NULL;
		}
		else {
			sz=element_table_add_get_data_size(row_data);
			element->data=malloc(sz);
			memmove(element->data,row_data,sz);
			if (sort) element_sort_table_data(element);
		}
	}
	
	SDL_mutexV(element_thread_lock);
	
	element_set_value(id,idx);
}

void element_set_bitmap(int id,char *path)
{
	element_type	*element;

	SDL_mutexP(element_thread_lock);

	element=element_find(id);
	if (element!=NULL) {
		bitmap_close(&element->setup.button.bitmap);
		bitmap_open(&element->setup.button.bitmap,path,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE);
	}

	SDL_mutexV(element_thread_lock);
}

void element_enable_table_checkboxes(int id,bool enable)
{
	element_type	*element;

	SDL_mutexP(element_thread_lock);

	element=element_find(id);
	if (element!=NULL) element->setup.table.checkbox=enable;

	SDL_mutexV(element_thread_lock);
}

void element_set_table_checkbox(int id,int idx,bool on)
{
	element_type	*element;
	
	SDL_mutexP(element_thread_lock);

	element=element_find(id);
	if (element!=NULL) element->setup.table.checks[idx]=(on?0x1:0x0);
	
	SDL_mutexV(element_thread_lock);
}

bool element_get_table_checkbox(int id,int idx)
{
	bool			on;
	element_type	*element;
	
	on=FALSE;

	SDL_mutexP(element_thread_lock);

	element=element_find(id);
	if (element!=NULL) on=(element->setup.table.checks[idx]!=0x0);
	
	SDL_mutexV(element_thread_lock);

	return(on);
}

bool element_has_table_check(int id)
{
	int				n;
	bool			hit;
	element_type	*element;

	SDL_mutexP(element_thread_lock);

	hit=FALSE;
	element=element_find(id);

	if (element!=NULL) {
		for (n=0;n!=element_table_max_check;n++) {
			if (element->setup.table.checks[n]!=0x0) {
				hit=TRUE;
				break;
			}
		}
	}
	
	SDL_mutexV(element_thread_lock);

	return(hit);
}

void element_get_table_row(int id,int idx,char *row_str)
{
	element_type	*element;
	
	SDL_mutexP(element_thread_lock);

	element=element_find(id);
	if (element!=NULL) {
		strcpy(row_str,(element->data+(idx*128)));
	}
	
	SDL_mutexV(element_thread_lock);
}

/* =======================================================

      Models
      
======================================================= */

bool element_replace_model(int id,char *name,char *animate,float resize,d3pnt *offset,d3ang *rot)
{
	bool			hit;
	element_type	*element;

	SDL_mutexP(element_thread_lock);

	hit=FALSE;
	element=element_find(id);

	if (element!=NULL) {
		element_free_model(element);
		element->setup.model.draw=element_load_model(name,animate,resize,offset,rot);
	}
	
	SDL_mutexV(element_thread_lock);

	return(hit);
}

/* =======================================================

      Position/Scrolling
      
======================================================= */

int element_get_scroll_position(int id)
{
	int				rtn;
	element_type	*element;

	SDL_mutexP(element_thread_lock);

	element=element_find(id);
	if (element==NULL) {
		rtn=0;
	}
	else {
		rtn=element->offset;
	}

	SDL_mutexV(element_thread_lock);

	return(rtn);
}

void element_set_scroll_position(int id,int pos)
{
	element_type	*element;

	SDL_mutexP(element_thread_lock);

	element=element_find(id);
	if (element!=NULL) element->offset=pos;

	SDL_mutexV(element_thread_lock);
}

void element_make_selection_visible(int id)
{
	int				high,row_high,cnt;
	element_type	*element;

	SDL_mutexP(element_thread_lock);

	element=element_find(id);
	if (element!=NULL) {
	
		if (element->value!=-1) {
	
			high=gl_text_get_char_height(iface.font.text_size_medium)+2;
			row_high=element_get_table_row_high(element);
			cnt=((element->high-(high+4))/row_high)-1;
			
			element->offset=0;
			while (element->value>=(element->offset+(cnt+1))) {
				element->offset+=(cnt+1);
			}
		}
	}
	
	SDL_mutexV(element_thread_lock);
}

int element_get_x_position(int id)
{
	int				x;
	element_type	*element;

	x=0;

	SDL_mutexP(element_thread_lock);

	element=element_find(id);
	if (element!=NULL) x=element->x;

	SDL_mutexV(element_thread_lock);

	return(x);
}

int element_get_y_position(int id)
{
	int				y;
	element_type	*element;

	y=0;

	SDL_mutexP(element_thread_lock);

	element=element_find(id);
	if (element!=NULL) y=element->y;

	SDL_mutexV(element_thread_lock);

	return(y);
}

void element_get_frame_inner_space(int id,int *x,int *y,int *wid,int *high)
{
	int					fy,fhigh,padding,
						title_high,tab_high;
	element_type		*element;
	
	element=element_find(id);
	if (element==NULL) {
		*x=*y=*wid=*high=0;
		return;
	}

	padding=element_get_padding();

	*x=element->x+padding;
	*wid=element->wid-(padding*2);

	fy=element->y+padding;
	fhigh=element->high-(padding*2);

	if (element->str[0]!=0x0) {
		title_high=element_get_frame_title_high();
		fy+=title_high;
		fhigh-=title_high;
	}

	if (element->setup.frame.has_tabs) {
		tab_high=element_get_tab_control_high();
		fy+=tab_high;
		fhigh-=tab_high;
	}

	if (element->setup.frame.has_buttons) fhigh-=(element_get_button_high()+padding);

	*y=fy;
	*high=fhigh;
}

