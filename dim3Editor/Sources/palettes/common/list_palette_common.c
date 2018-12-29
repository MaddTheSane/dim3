/****************************** File *********************************

Module: dim3 Common
Author: Brian Barnes
 Usage: List Palette Common

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

extern char						property_file_list[file_paths_max_directory_file][file_str_len];

extern file_path_setup_type		file_path_setup;

bitmap_type						list_btn_bitmaps[7];

/* =======================================================

      List Palette Global Setup
      
======================================================= */

void list_palette_initialize(void)
{
	int				n;
	char			sub_path[1024],path[1024];
	char			btn_names[7][32]={"Back","Edit","Plus","Minus","Set","Up","Down"};

		// load buttons

	os_get_support_file_path(sub_path);
	strcat(sub_path,"/Lists");
		
	for (n=0;n!=7;n++) {
		file_paths_app(&file_path_setup,path,sub_path,btn_names[n],"png");
		bitmap_open(&list_btn_bitmaps[n],path,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE);
	}
}

void list_palette_shutdown(void)
{
	int				n;

	for (n=0;n!=7;n++) {
		bitmap_close(&list_btn_bitmaps[n]);
	}
}

/* =======================================================

      List Palette List Setup
      
======================================================= */

void list_palette_list_initialize_single_pane(list_palette_pane_type *pane)
{
	pane->level=0;

	pane->scroll_offset=0;
	pane->total_high=0;
	
	pane->back_push_on=FALSE;
	pane->item_push_on=FALSE;
	
	pane->item_push_idx=-1;
	pane->button_push_idx=list_button_push_type_none;

	pane->click.id=-1;
	pane->click.idx=-1;
	pane->click.item=NULL;

	pane->items=(list_palette_item_type*)malloc(list_max_item_count*sizeof(list_palette_item_type));
}

void list_palette_list_initialize(list_palette_type *list,char *title,bool left,bool never_hide_picker,bool never_open)
{
	strcpy(list->item_pane.titles[0],title);
	list->item_pane.titles[1][0]=list->item_pane.titles[2][0]=0x0;
	
	list->open=TRUE;
	list->hide=FALSE;

	list->flag.left=left;
	list->flag.never_hide_picker=never_hide_picker;
	list->flag.never_open=never_open;

	list->picker.on=FALSE;
	list->picker.catch_last_click=FALSE;

	list_palette_list_initialize_single_pane(&list->item_pane);
	list_palette_list_initialize_single_pane(&list->picker_pane);
}

void list_palette_list_shutdown(list_palette_type *list)
{
	if (list->item_pane.items!=NULL) free(list->item_pane.items);
	if (list->picker_pane.items!=NULL) free(list->picker_pane.items);
}

/* =======================================================

      List Palette Titles
      
======================================================= */

void list_palette_set_title_single(list_palette_type *list,int idx,char *title,char *name)
{
	if (title==NULL) {
		list->item_pane.titles[idx][0]=0x0;
		return;
	}

	if (name!=NULL) {
		sprintf(list->item_pane.titles[idx],"%s:%s",title,name);
		return;
	}
	
	strcpy(list->item_pane.titles[idx],title);
}

void list_palette_set_title(list_palette_type *list,char *title_0,char *name_0,char *title_1,char *name_1,char *title_2,char *name_2)
{
	list_palette_set_title_single(list,0,title_0,name_0);
	list_palette_set_title_single(list,1,title_1,name_1);
	list_palette_set_title_single(list,2,title_2,name_2);
}

int list_palette_pane_get_title_high(list_palette_pane_type *pane)
{
	int				n,high;

	high=0;

	for (n=0;n!=3;n++) {
		if (pane->titles[n][0]==0x0) break;
		high+=list_title_line_high;
	}

	return(high+5);
}

/* =======================================================

      List Palette Levels and Open State
      
======================================================= */

int list_palette_get_level(list_palette_type *list)
{
	return(list->item_pane.level);
}

void list_palette_set_level(list_palette_type *list,int level)
{
		// if we are going from 0
		// to another level, save the
		// scroll, if we are going to
		// level 0, restore the scroll

	if (level!=0) {
		if (list->item_pane.level==0) list->item_pane.sav_scroll_offset=list->item_pane.scroll_offset;
		list->item_pane.scroll_offset=0;
	}
	else {
		if (list->item_pane.level!=0) list->item_pane.scroll_offset=list->item_pane.sav_scroll_offset;
	}

		// level changes always turns off
		// any picker

	list->picker.on=FALSE;
	list->item_pane.level=level;
}

bool list_palette_is_open(list_palette_type *list)
{
	return(list->open);
}

/* =======================================================

      List Palette Box
      
======================================================= */

int list_palette_width(list_palette_type *list)
{
	int				wid;

	if (list->open) {
		wid=list_palette_tree_sz;
	}
	else {
		wid=list_palette_border_sz;
	}

	if (list->picker.on) wid+=list_palette_tree_sz;

	return(wid);
}

void list_palette_total_box(list_palette_type *list,d3rect *box)
{
	d3rect			wbox;
	
	os_get_window_box(&wbox);

	if (list->open) {
		if (!list->flag.left) {
			box->lx=wbox.rx-list_palette_tree_sz;
			if (list->picker.on) box->lx-=list_palette_tree_sz;
			box->rx=wbox.rx;
		}
		else {
			box->lx=0;
			box->rx=list_palette_tree_sz;
			if (list->picker.on) box->rx+=list_palette_tree_sz;
		}
	}
	else {
		if (!list->flag.left) {
			box->lx=wbox.rx-list_palette_border_sz;
			box->rx=wbox.rx;
		}
		else {
			box->lx=0;
			box->rx=list_palette_border_sz;
		}
	}

#ifndef D3_SETUP
	box->ty=wbox.ty+(tool_button_size+1);
#else
	box->ty=wbox.ty;
#endif
	box->by=wbox.by;
}

void list_palette_item_box(list_palette_type *list,d3rect *box)
{
	d3rect			wbox;
	
	os_get_window_box(&wbox);

	if (list->open) {
		if (!list->flag.left) {
			box->lx=wbox.rx-list_palette_tree_sz;
			if (list->picker.on) box->lx-=list_palette_tree_sz;
			box->rx=box->lx+list_palette_tree_sz;
		}
		else {
			box->lx=0;
			box->rx=list_palette_tree_sz;
		}
	}
	else {
		if (!list->flag.left) {
			box->lx=wbox.rx-list_palette_border_sz;
			box->rx=wbox.rx;
		}
		else {
			box->lx=0;
			box->rx=list_palette_border_sz;
		}
	}

#ifndef D3_SETUP
	box->ty=wbox.ty+(tool_button_size+1);
#else
	box->ty=wbox.ty;
#endif
	box->by=wbox.by;
}

void list_palette_picker_box(list_palette_type *list,d3rect *box)
{
	d3rect			wbox;
	
	os_get_window_box(&wbox);

	if (!list->flag.left) {
		box->lx=wbox.rx-list_palette_tree_sz;
		box->rx=wbox.rx;
	}
	else {
		box->lx=list_palette_tree_sz;
		box->rx+=list_palette_tree_sz;
	}

#ifndef D3_SETUP
	box->ty=wbox.ty+(tool_button_size+1);
#else
	box->ty=wbox.ty;
#endif
	box->by=wbox.by;
}

void list_palette_border_box(list_palette_type *list,d3rect *box)
{
	d3rect			wbox;
	
	os_get_window_box(&wbox);

	if (list->open) {
		if (!list->flag.left) {
			box->lx=wbox.rx-list_palette_tree_sz;
			box->rx=box->lx+list_palette_border_sz;
		}
		else {
			box->lx=list_palette_tree_sz-list_palette_border_sz;
			box->rx=box->lx+list_palette_border_sz;
		}
	}
	else {
		if (!list->flag.left) {
			box->lx=wbox.rx-list_palette_border_sz;
			box->rx=wbox.rx;
		}
		else {
			box->lx=0;
			box->rx=list_palette_border_sz;
		}
	}

#ifndef D3_SETUP
	box->ty=wbox.ty+(tool_button_size+1);
#else
	box->ty=wbox.ty;
#endif
	box->by=wbox.by;
}

/* =======================================================

      List Add Items
      
======================================================= */

list_palette_item_type* list_palette_pane_create_item(list_palette_pane_type *pane,int ctrl_type)
{
	list_palette_item_type		*item;

	item=&pane->items[pane->item_count];
	pane->item_count++;

	item->ctrl_type=ctrl_type;
	item->button_type=list_button_none;
	
	item->button_id=-1;
	item->button_up_id=-1;
	item->button_down_id=-1;

	item->disabled=FALSE;
	item->selected=FALSE;
	item->moveable=FALSE;

	item->x=list_palette_border_sz+4;
	if (ctrl_type!=list_item_ctrl_header) item->x+=10;

	item->y=(pane->item_count*list_item_font_high)+list_palette_pane_get_title_high(pane);

	pane->total_high+=list_item_font_high;

	return(item);
}

void list_palette_add_header(list_palette_type *list,int id,char *name)
{
	list_palette_item_type		*item;

	item=list_palette_pane_create_item(&list->item_pane,list_item_ctrl_header);

	item->id=id;
	item->idx=-1;
	item->count=-1;

	item->selected=FALSE;

	strcpy(item->name,name);
}

void list_palette_add_picker_header(list_palette_type *list,int id,char *name)
{
	list_palette_item_type		*item;

	item=list_palette_pane_create_item(&list->picker_pane,list_item_ctrl_header);

	item->id=id;
	item->idx=-1;
	item->count=-1;

	item->selected=FALSE;

	strcpy(item->name,name);
}

void list_palette_add_header_count(list_palette_type *list,int id,char *name,int count)
{
	list_palette_item_type		*item;

	item=list_palette_pane_create_item(&list->item_pane,list_item_ctrl_header);

	item->id=id;
	item->idx=-1;
	item->count=count;

	item->selected=FALSE;

	strcpy(item->name,name);
}

void list_palette_add_header_button(list_palette_type *list,int id,char *name,int button_type)
{
	list_palette_item_type		*item;

	item=list_palette_pane_create_item(&list->item_pane,list_item_ctrl_header);

	item->id=-1;
	item->idx=-1;
	item->button_id=id;
	item->count=-1;
	item->button_type=button_type;

	item->selected=FALSE;

	strcpy(item->name,name);
}

void list_palette_add_item(list_palette_type *list,int id,int idx,char *name,bool selected,bool disabled)
{
	list_palette_item_type		*item;

	item=list_palette_pane_create_item(&list->item_pane,list_item_ctrl_text);

	item->id=id;
	item->idx=idx;

	item->selected=selected;
	item->disabled=disabled;

	strcpy(item->name,name);
}

void list_palette_add_picker_item(list_palette_type *list,int id,int idx,char *name,bool selected,bool disabled)
{
	list_palette_item_type		*item;

	item=list_palette_pane_create_item(&list->picker_pane,list_item_ctrl_text);

	item->id=id;
	item->idx=idx;

	item->selected=selected;
	item->disabled=disabled;

	strcpy(item->name,name);
	
	if (item->name[0]==0x0) strcpy(item->name,"(no name)");
}

void list_palette_add_color(list_palette_type *list,int id,int idx,d3col *col_ptr,bool selected,bool disabled)
{
	list_palette_item_type		*item;

	item=list_palette_pane_create_item(&list->item_pane,list_item_ctrl_color);

	item->id=id;
	item->idx=idx;

	item->selected=selected;
	item->disabled=disabled;

	item->value.col_ptr=col_ptr;
}

void list_palette_add_string_selectable(list_palette_type *list,int id,char *name,char *str_ptr,int str_len,bool selected,bool disabled)
{
	list_palette_item_type		*item;
	
	item=list_palette_pane_create_item(&list->item_pane,list_item_ctrl_string);

	item->id=id;
	item->idx=-1;

	item->selected=selected;
	item->disabled=disabled;

	strcpy(item->name,name);

	item->limit.str_len=str_len;
	item->value.str_ptr=str_ptr;
}

void list_palette_add_string_selectable_button(list_palette_type *list,int id,int button_type,int button_id,char *name,bool selected,bool disabled)
{
	list_palette_item_type		*item;

		// add item

	list_palette_add_string_selectable(list,id,name,NULL,-1,selected,disabled);

		// put in the button

	item=&list->item_pane.items[list->item_pane.item_count-1];
	item->button_type=button_type;
	item->button_id=button_id;
}

void list_palette_add_string_selectable_moveable_button(list_palette_type *list,int id,int button_type,int button_id,int button_up_id,int button_down_id,char *name,bool selected,bool disabled)
{
	list_palette_item_type		*item;

		// add item

	list_palette_add_string_selectable(list,id,name,NULL,-1,selected,disabled);

		// put in the button

	item=&list->item_pane.items[list->item_pane.item_count-1];
	item->button_type=button_type;
	item->button_id=button_id;
	item->button_up_id=button_up_id;
	item->button_down_id=button_down_id;

		// moveable flag

	item->moveable=TRUE;
}

void list_palette_add_string(list_palette_type *list,int id,char *name,char *str_ptr,int str_len,bool disabled)
{
	list_palette_add_string_selectable(list,id,name,str_ptr,str_len,FALSE,disabled);
}

void list_palette_add_parameter(list_palette_type *list,int id,char *name,char *params,int param_idx,bool disabled)
{
	list_palette_item_type		*item;
	
	item=list_palette_pane_create_item(&list->item_pane,list_item_ctrl_param);

	item->id=id;
	item->idx=-1;

	item->selected=FALSE;
	item->disabled=disabled;

	strcpy(item->name,name);

	item->limit.str_len=256;
	item->limit.param_idx=param_idx;
	item->value.str_ptr=params;
}

void list_palette_add_na_blank(list_palette_type *list,int id,char *name)
{
	list_palette_item_type		*item;
	
	item=list_palette_pane_create_item(&list->item_pane,list_item_ctrl_na_blank);

	item->id=id;
	item->idx=-1;

	item->selected=FALSE;
	item->disabled=TRUE;

	strcpy(item->name,name);
}

void list_palette_add_int(list_palette_type *list,int id,char *name,int *int_ptr,bool disabled)
{
	list_palette_item_type		*item;

	item=list_palette_pane_create_item(&list->item_pane,list_item_ctrl_int);

	item->id=id;
	item->idx=-1;

	item->selected=FALSE;
	item->disabled=disabled;

	strcpy(item->name,name);
	item->value.int_ptr=int_ptr;
}

void list_palette_add_float(list_palette_type *list,int id,char *name,float *float_ptr,bool disabled)
{
	list_palette_item_type		*item;

	item=list_palette_pane_create_item(&list->item_pane,list_item_ctrl_float);

	item->id=id;
	item->idx=-1;

	item->selected=FALSE;
	item->disabled=disabled;

	strcpy(item->name,name);
	item->value.float_ptr=float_ptr;
}

void list_palette_add_checkbox(list_palette_type *list,int id,char *name,bool *bool_ptr,bool disabled)
{
	list_palette_item_type		*item;

	item=list_palette_pane_create_item(&list->item_pane,list_item_ctrl_checkbox);

	item->id=id;
	item->idx=-1;

	item->selected=FALSE;
	item->disabled=disabled;

	strcpy(item->name,name);
	item->value.bool_ptr=bool_ptr;
}

void list_palette_add_pick_color(list_palette_type *list,int id,char *name,d3col *col_ptr,bool disabled)
{
	list_palette_item_type		*item;

	item=list_palette_pane_create_item(&list->item_pane,list_item_ctrl_pick_color);

	item->id=id;
	item->idx=-1;

	item->selected=FALSE;
	item->disabled=disabled;

	strcpy(item->name,name);
	item->value.col_ptr=col_ptr;
}

void list_palette_add_point(list_palette_type *list,int id,char *name,d3pnt *pnt_ptr,bool disabled)
{
	list_palette_item_type		*item;

	item=list_palette_pane_create_item(&list->item_pane,list_item_ctrl_point);

	item->id=id;
	item->idx=-1;

	item->selected=FALSE;
	item->disabled=disabled;

	strcpy(item->name,name);
	item->value.pnt_ptr=pnt_ptr;
}

void list_palette_add_angle(list_palette_type *list,int id,char *name,d3ang *ang_ptr,bool disabled)
{
	list_palette_item_type		*item;

	item=list_palette_pane_create_item(&list->item_pane,list_item_ctrl_angle);

	item->id=id;
	item->idx=-1;

	item->selected=FALSE;
	item->disabled=disabled;

	strcpy(item->name,name);
	item->value.ang_ptr=ang_ptr;
}

void list_palette_add_vector(list_palette_type *list,int id,char *name,d3vct *vct_ptr,bool disabled)
{
	list_palette_item_type		*item;

	item=list_palette_pane_create_item(&list->item_pane,list_item_ctrl_vector);

	item->id=id;
	item->idx=-1;

	item->selected=FALSE;
	item->disabled=disabled;

	strcpy(item->name,name);
	item->value.vct_ptr=vct_ptr;
}

void list_palette_add_normal_vector(list_palette_type *list,int id,char *name,d3vct *vct_ptr,bool disabled)
{
	list_palette_item_type		*item;

	item=list_palette_pane_create_item(&list->item_pane,list_item_ctrl_normal_vector);

	item->id=id;
	item->idx=-1;

	item->selected=FALSE;
	item->disabled=disabled;

	strcpy(item->name,name);
	item->value.vct_ptr=vct_ptr;
}

void list_palette_add_uv(list_palette_type *list,int id,char *name,d3uv *uv_ptr,bool disabled)
{
	list_palette_item_type		*item;

	item=list_palette_pane_create_item(&list->item_pane,list_item_ctrl_uv);

	item->id=id;
	item->idx=-1;

	item->selected=FALSE;
	item->disabled=disabled;

	strcpy(item->name,name);
	item->value.uv_ptr=uv_ptr;
}

void list_palette_add_picker_list_int(list_palette_type *list,int id,char *name,char *list_ptr,int list_count,int list_item_sz,int list_name_offset,bool include_none,bool sort,int *int_ptr,bool disabled)
{
	list_palette_item_type		*item;

	item=list_palette_pane_create_item(&list->item_pane,list_item_ctrl_picker);

	item->id=id;
	item->idx=-1;

	item->selected=FALSE;
	item->disabled=disabled;

		// if list count is -1, then calculate
		// it from the first blank string

	if (list_count==-1) list_count=property_pick_get_list_count(list_ptr,list_item_sz,list_name_offset);

		// setup the picking list

	item->list.ptr=list_ptr;
	item->list.count=list_count;
	item->list.item_sz=list_item_sz;
	item->list.name_offset=list_name_offset;
	item->list.file.file_list=FALSE;
	item->list.include_none=include_none;
	item->list.sort=sort;

		// setup the value

	strcpy(item->name,name);
	item->list.is_index=TRUE;
	item->value.int_ptr=int_ptr;
}

void list_palette_add_picker_list_string(list_palette_type *list,int id,char *name,char *list_ptr,int list_count,int list_item_sz,int list_name_offset,bool include_none,bool sort,char *str_ptr,bool disabled)
{
	list_palette_item_type		*item;

	item=list_palette_pane_create_item(&list->item_pane,list_item_ctrl_picker);

	item->id=id;
	item->idx=-1;

	item->selected=FALSE;
	item->disabled=disabled;

		// if list count is -1, then calculate
		// it from the first blank string

	if (list_count==-1) list_count=property_pick_get_list_count(list_ptr,list_item_sz,list_name_offset);

		// setup the picking list

	item->list.ptr=list_ptr;
	item->list.count=list_count;
	item->list.item_sz=list_item_sz;
	item->list.name_offset=list_name_offset;
	item->list.file.file_list=FALSE;
	item->list.include_none=include_none;
	item->list.sort=sort;

		// setup the value

	strcpy(item->name,name);
	item->list.is_index=FALSE;
	item->value.str_ptr=str_ptr;
}

void list_palette_add_picker_file(list_palette_type *list,int id,int button_type,int button_id,char *name,char *search_path,char *extension,char *required_file_name,char *str_ptr,bool disabled)
{
	list_palette_item_type		*item;

	item=list_palette_pane_create_item(&list->item_pane,list_item_ctrl_picker);

	item->id=id;
	item->idx=-1;

	item->selected=FALSE;
	item->disabled=disabled;

		// setup the picking list

	item->list.ptr=property_pick_file_get_list();
	item->list.count=-1;
	item->list.item_sz=file_str_len;
	item->list.name_offset=0;
	item->list.include_none=TRUE;
	
	item->list.file.file_list=TRUE;
	strcpy(item->list.file.search_path,search_path);
	strcpy(item->list.file.extension,extension);
	strcpy(item->list.file.required_file_name,required_file_name);

		// setup the value

	strcpy(item->name,name);
	item->list.is_index=FALSE;
	item->value.str_ptr=str_ptr;

		// the button

	item->button_type=button_type;
	item->button_id=button_id;
}

/* =======================================================

      List Delete Items
      
======================================================= */

void list_palette_delete_all_items(list_palette_type *list)
{
	list->item_pane.item_count=0;
	list->item_pane.total_high=0;
}

/* =======================================================

      List Palette Picking Mode
      
======================================================= */

void list_palette_start_picking_mode(list_palette_type *list,char *title,char *list_ptr,int list_count,int list_item_sz,int list_name_offset,bool include_none,bool file_list,bool sort,int *idx_ptr,char *name_ptr)
{
	int						n;
	char					*str_ptr;
	bool					sel;
	list_palette_pane_type	*pane;

		// turn on picker

	list->picker.on=TRUE;
	list->picker.picker_idx_ptr=idx_ptr;
	list->picker.picker_name_ptr=name_ptr;

	list->picker.file_list=file_list;
	
		// flip off any regular pane
		// click items
		
	list->item_pane.click.id=-1;

		// if list count is -1, then calculate
		// it from the first blank string

	if (list_count==-1) list_count=property_pick_get_list_count(list_ptr,list_item_sz,list_name_offset);
		
		// fill in the pane

	pane=&list->picker_pane;

		// the title

	strcpy(pane->titles[0],title);
	pane->titles[1][0]=pane->titles[2][0]=0x0;

	pane->item_count=0;
	pane->total_high=0;

		// none item

	if (include_none) {

		sel=FALSE;
		if (idx_ptr!=NULL) {
			sel=(*idx_ptr==-1);
		}
		if (name_ptr!=NULL) {
			sel=(*name_ptr==0x0);
		}

		list_palette_add_picker_item(list,-1,-1,"None",sel,FALSE);
	}

		// other items

	if (sort) list_palette_sort_mark_start(list,&list->picker_pane);

	for (n=0;n!=list_count;n++) {
		str_ptr=list_ptr+((list_item_sz*n)+list_name_offset);

			// @ marks headers

		if (str_ptr[0]=='@') {
			list_palette_add_picker_header(list,-1,(char*)&str_ptr[1]);
			continue;
		}

			// ~ marks disabled

		if (str_ptr[0]=='~') {
			list_palette_add_picker_item(list,-1,n,(char*)&str_ptr[1],sel,TRUE);
			continue;
		}

			// regular clickable items
			// ignore blanks as they can only select nones

		sel=FALSE;
		if (idx_ptr!=NULL) {
			sel=(*idx_ptr==n);
		}
		if (name_ptr!=NULL) {
			if (*name_ptr!=0x0) sel=(strcasecmp(name_ptr,str_ptr)==0);
		}

		list_palette_add_picker_item(list,-1,n,str_ptr,sel,FALSE);
	}

	if (sort) list_palette_sort(list,&list->picker_pane);
}

void list_palette_start_picking_item_mode(list_palette_type *list,list_palette_item_type *item)
{
	char				title[256];
	char				*list_ptr;

		// if a file list, build the files

	list_ptr=item->list.ptr;

	if (item->list.file.file_list) {
		item->list.count=property_pick_file_fill_list(item->list.file.search_path,item->list.file.extension,item->list.file.required_file_name);
		list_ptr=(char*)property_file_list;
	}

	sprintf(title,"Pick %s",item->name);

	if (item->list.is_index) {
		list_palette_start_picking_mode(list,title,list_ptr,item->list.count,item->list.item_sz,item->list.name_offset,item->list.include_none,item->list.file.file_list,item->list.sort,item->value.int_ptr,NULL);
	}
	else {
		list_palette_start_picking_mode(list,title,list_ptr,item->list.count,item->list.item_sz,item->list.name_offset,item->list.include_none,item->list.file.file_list,item->list.sort,NULL,item->value.str_ptr);
	}
}

void list_palette_click_picker(list_palette_type *list)
{
	int						idx;
	char					dir[256];
	list_palette_pane_type	*pane;

		// turn off list picker
		// and set flag used to ignore double
		// clicks that might come after closing
		// this list

	list->picker.on=FALSE;
	list->picker.catch_last_click=TRUE;

	pane=&list->picker_pane;

		// integer list pickers just get index

	if (list->picker.picker_idx_ptr!=NULL) {
		*list->picker.picker_idx_ptr=pane->click.idx;
		main_wind_draw();
		return;
	}

		// blank string picker

	if (pane->click.idx==-1) {
		*list->picker.picker_name_ptr=0x0;
		main_wind_draw();
		return;
	}

		// if not file picker, just grab the name

	if (!list->picker.file_list) {
		strcpy(list->picker.picker_name_ptr,pane->click.item->name);
		main_wind_draw();
		return;
	}

		// file picker needs to find directory

	dir[0]=0x0;

	idx=pane->click.item_idx-1;

	while (idx>=0) {
		if (pane->items[idx].ctrl_type==list_item_ctrl_header) {
			strcpy(dir,(char*)&pane->items[idx].name[5]);	// get over Root/
			break;
		}
		idx--;
	}

		// now add in the directory + file name

	if (dir[0]==0x0) {
		strcpy(list->picker.picker_name_ptr,pane->click.item->name);
	}
	else {
		sprintf(list->picker.picker_name_ptr,"%s/%s",dir,pane->click.item->name);
	}

	main_wind_draw();
}

/* =======================================================

      List Sorting
      
======================================================= */

void list_palette_sort_mark_start(list_palette_type *list,list_palette_pane_type *pane)
{
	pane->item_sort_start_idx=pane->item_count;
}

void list_palette_sort(list_palette_type *list,list_palette_pane_type *pane)
{
	int						n,k,x,y;
	bool					shuffle;
	list_palette_item_type	temp_item;

	if (pane->item_sort_start_idx>=(pane->item_count-1)) return;

	while (TRUE) {

		shuffle=FALSE;

		for (n=pane->item_sort_start_idx;n<(pane->item_count-1);n++) {
			k=n+1;

			if (strcasecmp(pane->items[n].name,pane->items[k].name)>0) {
				shuffle=TRUE;

				memmove(&temp_item,&pane->items[n],sizeof(list_palette_item_type));
				memmove(&pane->items[n],&pane->items[k],sizeof(list_palette_item_type));
				memmove(&pane->items[k],&temp_item,sizeof(list_palette_item_type));

				x=pane->items[n].x;
				pane->items[n].x=pane->items[k].x;
				pane->items[k].x=x;

				y=pane->items[n].y;
				pane->items[n].y=pane->items[k].y;
				pane->items[k].y=y;
			}
		}

		if (!shuffle) break;
	}
}

/* =======================================================

      Picker Index/Name to Name
      
======================================================= */

void list_palette_get_current_picker_value(list_palette_item_type *item,char *str)
{
	int				idx;

	if (!item->list.is_index) {
		strcpy(str,item->value.str_ptr);
		return;
	}

	idx=*item->value.int_ptr;
	if (idx==-1) {
		str[0]=0x0;
		return;
	}
	
	strcpy(str,(item->list.ptr+((item->list.item_sz*idx)+item->list.name_offset)));
}

/* =======================================================

      List Palette Scroll Pages
      
======================================================= */

int list_palette_get_scroll_offset_max(list_palette_pane_type *pane,d3rect *box)
{
	int				high;
	
	high=pane->total_high-(box->by-(box->ty+list_palette_pane_get_title_high(pane)));
	if (high<=0) return(0);
	
	return(high);
}

void list_palette_get_scroll_thumb_position(list_palette_pane_type *pane,d3rect *box,int *thumb_ty,int *thumb_by)
{
	int				ty,high;

	ty=box->ty+list_palette_pane_get_title_high(pane);
	high=box->by-ty;
	
	*thumb_ty=ty+((pane->scroll_offset*high)/pane->total_high);
	if (*thumb_ty<ty) *thumb_ty=ty;
	
	*thumb_by=ty+(((pane->scroll_offset+high)*high)/pane->total_high);
	if (*thumb_by>box->by) *thumb_by=box->by;
}

int list_palette_pane_get_scroll_thumb_page_offset(list_palette_pane_type *pane,d3rect *box)
{
	int				ty;

	ty=box->ty+list_palette_pane_get_title_high(pane);
	return(box->by-ty);
}

/* =======================================================

      List Palette Draw Utilities
      
======================================================= */

void list_palette_pane_draw_item_color_box(list_palette_pane_type *pane,d3rect *box,list_palette_item_type *item,d3col *col)
{
	int					x,y;
	float				vertexes[8];

	x=box->rx-(list_item_font_high+(list_palette_scroll_wid+4));
	y=(box->ty+item->y)-pane->scroll_offset;

	vertexes[0]=vertexes[6]=(float)x;
	vertexes[2]=vertexes[4]=(float)(x+list_item_font_high);
	vertexes[1]=vertexes[3]=(float)((y-list_item_font_high)+2);
	vertexes[5]=vertexes[7]=(float)(y-2);

	glVertexPointer(2,GL_FLOAT,0,vertexes);

	glColor4f((col->r*0.5f),(col->g*0.5f),(col->b*0.5f),1.0f);
	glDrawArrays(GL_QUADS,0,4);

	vertexes[0]=vertexes[6]=(float)(x+1);
	vertexes[2]=vertexes[4]=(float)((x+list_item_font_high)-1);
	vertexes[1]=vertexes[3]=(float)((y-list_item_font_high)+3);
	vertexes[5]=vertexes[7]=(float)(y-3);

	glVertexPointer(2,GL_FLOAT,0,vertexes);

	glColor4f(col->r,col->g,col->b,1.0f);
	glDrawArrays(GL_QUADS,0,4);
}

void list_palette_pane_draw_item_check_box(list_palette_pane_type *pane,d3rect *box,list_palette_item_type *item,bool checked)
{
	int					lx,rx,y,ty,by;
	float				vertexes[8];

	lx=box->rx-(list_item_font_high+(list_palette_scroll_wid+2));
	rx=lx+(list_item_font_high-2);

	y=(box->ty+item->y)-pane->scroll_offset;
	ty=(y-list_item_font_high)+2;
	by=y;

	vertexes[0]=vertexes[6]=(float)lx;
	vertexes[2]=vertexes[4]=(float)rx;
	vertexes[1]=vertexes[3]=(float)ty;
	vertexes[5]=vertexes[7]=(float)by;

	glVertexPointer(2,GL_FLOAT,0,vertexes);

	glColor4f(0.0f,0.0f,0.0f,1.0f);
	glDrawArrays(GL_QUADS,0,4);

	vertexes[0]=vertexes[6]=(float)(lx+1);
	vertexes[2]=vertexes[4]=(float)(rx-1);
	vertexes[1]=vertexes[3]=(float)(ty+1);
	vertexes[5]=vertexes[7]=(float)(by-1);

	glVertexPointer(2,GL_FLOAT,0,vertexes);

	glColor4f(0.9f,0.9f,0.9f,1.0f);
	glDrawArrays(GL_QUADS,0,4);

	if (checked) {
		glLineWidth(3.0f);

		vertexes[0]=vertexes[6]=(float)(lx+1);
		vertexes[2]=vertexes[4]=(float)(rx-1);
		vertexes[1]=vertexes[5]=(float)(ty+1);
		vertexes[3]=vertexes[7]=(float)(by-1);

		glVertexPointer(2,GL_FLOAT,0,vertexes);

		glColor4f(0.0f,0.8f,0.0f,1.0f);
		glDrawArrays(GL_LINES,0,4);

		glLineWidth(1.0f);
	}
}

void list_palette_pane_draw_item_string(list_palette_pane_type *pane,d3rect *box,list_palette_item_type *item,char *str)
{
	int					rx,y;
	char				str2[list_value_clip_size+8];
	d3col				col;

	if (str==NULL) return;

		// clip string

	if (strlen(str)>=list_value_clip_size) {
		strncpy(str2,str,list_value_clip_size);
		strcpy((char*)&str2[list_value_clip_size],"...");
	}
	else {
		strcpy(str2,str);
	}

		// draw

	rx=box->rx-(list_palette_scroll_wid+4);
	if (item->button_type!=list_button_none) rx-=(list_item_font_high+2);
	
	y=(box->ty+item->y)-pane->scroll_offset;

	if (!item->disabled) {
		text_draw_right(rx,y,list_item_font_size,NULL,str2);
		return;
	}

	col.r=col.g=0.0f;
	col.b=1.0f;
	text_draw_right(rx,y,list_item_font_size,&col,str2);
}

void list_palette_pane_draw_item_button_bitmap(list_palette_pane_type *pane,list_palette_item_type *item,int bitmap_idx,int lx,int ty,bool pushed)
{
	float			vertexes[8],uvs[8]={0.0f,0.0f,1.0f,0.0f,1.0f,1.0f,0.0f,1.0f};
	
	vertexes[0]=vertexes[6]=(float)lx;
	vertexes[2]=vertexes[4]=(float)(lx+list_item_font_high);
	vertexes[1]=vertexes[3]=(float)ty;
	vertexes[5]=vertexes[7]=(float)(ty+list_item_font_high);

	if (pushed) {
		glColor4f(0.6f,0.6f,0.6f,1.0f);
	}
	else {
		glColor4f(1.0f,1.0f,1.0f,1.0f);
	}

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D,list_btn_bitmaps[bitmap_idx].gl_id);

	glVertexPointer(2,GL_FLOAT,0,vertexes);

	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2,GL_FLOAT,0,uvs);

	glDrawArrays(GL_QUADS,0,4);

	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	
	glDisable(GL_TEXTURE_2D);
}

void list_palette_pane_draw_item_button(list_palette_pane_type *pane,d3rect *box,bool left,int idx)
{
	int						lx,ty;
	bool					pushed;
	list_palette_item_type *item;
	
	item=&pane->items[idx];
	if (item->button_type==list_button_none) return;

	lx=box->rx-(list_palette_scroll_wid+1);
	if (left) lx-=list_palette_border_sz;
	lx-=16;
	ty=((box->ty+item->y)-list_item_font_high)-pane->scroll_offset;

	pushed=((pane->item_push_on) && (pane->item_push_idx==idx) && (pane->button_push_idx==list_button_push_type_button));
	list_palette_pane_draw_item_button_bitmap(pane,item,(item->button_type+1),lx,ty,pushed);
}

void list_palette_pane_draw_item_move_buttons(list_palette_pane_type *pane,d3rect *box,int idx)
{
	int						lx,ty;
	bool					pushed;
	list_palette_item_type *item;
	
	item=&pane->items[idx];
	if (!item->moveable) return;

	lx=box->lx+12;
	ty=((box->ty+item->y)-list_item_font_high)-pane->scroll_offset;
	
	pushed=((pane->item_push_on) && (pane->item_push_idx==idx) && (pane->button_push_idx==list_button_push_type_up));
	list_palette_pane_draw_item_button_bitmap(pane,item,5,lx,ty,pushed);

	lx+=(list_item_font_high+2);
	
	pushed=((pane->item_push_on) && (pane->item_push_idx==idx) && (pane->button_push_idx==list_button_push_type_down));
	list_palette_pane_draw_item_button_bitmap(pane,item,6,lx,ty,pushed);
}

/* =======================================================

      List Palette Title, Scrollbars and Border
      
======================================================= */

void list_palette_pane_draw_setup(list_palette_pane_type *pane,d3rect *box)
{
	float					vertexes[8];
	d3rect					wbox;

		// viewport setup
		
	os_get_window_box(&wbox);

	glEnable(GL_SCISSOR_TEST);
	glScissor(box->lx,(wbox.by-box->by),(box->rx-box->lx),(box->by-box->ty));

	glViewport(box->lx,(wbox.by-box->by),(box->rx-box->lx),(box->by-box->ty));
		
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho((GLdouble)box->lx,(GLdouble)box->rx,(GLdouble)box->by,(GLdouble)box->ty,-1.0,1.0);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

		// draw setup

	glDisable(GL_DEPTH_TEST);

		// background

	vertexes[0]=vertexes[6]=(float)box->lx;
	vertexes[2]=vertexes[4]=(float)box->rx;
	vertexes[1]=vertexes[3]=(float)box->ty;
	vertexes[5]=vertexes[7]=(float)box->by;

	glVertexPointer(2,GL_FLOAT,0,vertexes);

	glColor4f(1.0f,1.0f,1.0f,1.0f);
	glDrawArrays(GL_QUADS,0,4);
}

void list_palette_pane_draw_title(list_palette_pane_type *pane,d3rect *box)
{
	int					n,x,y,lx,rx,ty,by,high;
	float				vertexes[8],colors[16],uvs[8]={0.0f,0.0f,1.0f,0.0f,1.0f,1.0f,0.0f,1.0f};

		// title background

	high=list_palette_pane_get_title_high(pane);

	ty=box->ty;
	by=ty+high;

	vertexes[0]=vertexes[6]=(float)box->lx;
	vertexes[2]=vertexes[4]=(float)box->rx;
	vertexes[1]=vertexes[3]=(float)ty;
	vertexes[5]=vertexes[7]=(float)by;

	colors[0]=colors[1]=colors[2]=colors[4]=colors[5]=colors[6]=0.9f;
	colors[8]=colors[9]=colors[10]=colors[12]=colors[13]=colors[14]=0.7f;
	colors[3]=colors[7]=colors[11]=colors[15]=1.0f;

	glVertexPointer(2,GL_FLOAT,0,vertexes);

	glEnableClientState(GL_COLOR_ARRAY);
	glColorPointer(4,GL_FLOAT,0,colors);

	glDrawArrays(GL_QUADS,0,4);

	glDisableClientState(GL_COLOR_ARRAY);

		// title line

	vertexes[0]=vertexes[4]=(float)box->lx;
	vertexes[2]=vertexes[6]=(float)box->rx;
	vertexes[1]=vertexes[3]=(float)(ty+1);
	vertexes[5]=vertexes[7]=(float)by;

	glVertexPointer(2,GL_FLOAT,0,vertexes);

	glColor4f(0.0f,0.0f,0.0f,1.0f);
	glDrawArrays(GL_LINES,0,4);

		// text
	
	x=(box->lx+box->rx)>>1;
	y=ty+19;

	for (n=0;n!=3;n++) {
		if (pane->titles[0][0]==0x0) break;
		text_draw_center(x,y,list_title_font_size,NULL,pane->titles[n]);
		y+=list_title_line_high;
	}
	
		// back arrow
		
	if (pane->level==0) return;
	
#ifndef D3_SETUP
	lx=box->lx+12;
#else
	lx=box->lx+4;
#endif
	rx=lx+32;
	ty=box->ty+((high-16)/2);
	by=ty+32;

	if (!pane->back_push_on) {
		glColor4f(1.0f,1.0f,1.0f,1.0f);
	}
	else {
		glColor4f(0.7f,0.7f,0.7f,1.0f);
	}

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D,list_btn_bitmaps[0].gl_id);

	vertexes[0]=vertexes[6]=(float)lx;
	vertexes[2]=vertexes[4]=(float)rx;
	vertexes[1]=vertexes[3]=(float)ty;
	vertexes[5]=vertexes[7]=(float)by;

	glVertexPointer(2,GL_FLOAT,0,vertexes);

	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2,GL_FLOAT,0,uvs);

	glDrawArrays(GL_QUADS,0,4);

	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	
	glDisable(GL_TEXTURE_2D);
}

void list_palette_pane_draw_scrollbar(list_palette_pane_type *pane,d3rect *box,bool left)
{
	int					lx,rx,mx,ty,by,thumb_ty,thumb_by,
						scroll_max;
	float				vertexes[8],colors[16];
	
		// scroll bar background

	glEnableClientState(GL_COLOR_ARRAY);

	lx=box->rx-list_palette_scroll_wid;
	if (left) lx-=list_palette_border_sz;
	rx=lx+list_palette_scroll_wid;
	ty=box->ty+list_palette_pane_get_title_high(pane);
	by=box->by;

	mx=(lx+rx)>>1;

	vertexes[0]=vertexes[6]=(float)lx;
	vertexes[2]=vertexes[4]=(float)mx;
	vertexes[1]=vertexes[3]=(float)ty;
	vertexes[5]=vertexes[7]=(float)by;

	colors[0]=colors[1]=colors[2]=colors[12]=colors[13]=colors[14]=0.6f;
	colors[4]=colors[5]=colors[6]=colors[8]=colors[9]=colors[10]=0.9f;
	colors[3]=colors[7]=colors[11]=colors[15]=1.0f;

	glVertexPointer(2,GL_FLOAT,0,vertexes);
	glColorPointer(4,GL_FLOAT,0,colors);
	glDrawArrays(GL_QUADS,0,4);

	vertexes[0]=vertexes[6]=(float)mx;
	vertexes[2]=vertexes[4]=(float)rx;
	vertexes[1]=vertexes[3]=(float)ty;
	vertexes[5]=vertexes[7]=(float)by;

	colors[0]=colors[1]=colors[2]=colors[12]=colors[13]=colors[14]=0.9f;
	colors[4]=colors[5]=colors[6]=colors[8]=colors[9]=colors[10]=0.6f;
	colors[3]=colors[7]=colors[11]=colors[15]=1.0f;

	glVertexPointer(2,GL_FLOAT,0,vertexes);
	glColorPointer(4,GL_FLOAT,0,colors);
	glDrawArrays(GL_QUADS,0,4);

	glDisableClientState(GL_COLOR_ARRAY);

		// scroll bar thumb

	scroll_max=list_palette_get_scroll_offset_max(pane,box);

	if ((pane->item_count!=0) && (scroll_max!=0)) {
		
		list_palette_get_scroll_thumb_position(pane,box,&thumb_ty,&thumb_by);

		vertexes[0]=vertexes[6]=(float)(lx+1);
		vertexes[2]=vertexes[4]=(float)(rx-1);
		vertexes[1]=vertexes[3]=(float)(thumb_ty+1);
		vertexes[5]=vertexes[7]=(float)(thumb_by-1);

		glVertexPointer(2,GL_FLOAT,0,vertexes);
	
		glColor4f(0.9f,0.9f,0.9f,1.0f);
		glDrawArrays(GL_QUADS,0,4);

		vertexes[0]=vertexes[6]=(float)(lx+1);
		vertexes[2]=vertexes[4]=(float)rx;
		vertexes[1]=vertexes[3]=(float)(thumb_ty+1);
		vertexes[5]=vertexes[7]=(float)thumb_by;

		glVertexPointer(2,GL_FLOAT,0,vertexes);
	
		glColor4f(0.0f,0.0f,0.0f,1.0f);
		glDrawArrays(GL_LINE_LOOP,0,4);
	}

	vertexes[0]=vertexes[2]=(float)lx;
	vertexes[1]=(float)ty;
	vertexes[3]=(float)by;

	glVertexPointer(2,GL_FLOAT,0,vertexes);

	glColor4f(0.0f,0.0f,0.0f,1.0f);
	glDrawArrays(GL_LINES,0,2);
}

void list_palette_draw_border(d3rect *box,bool left)
{
	int					lx,rx;
	float				vertexes[8],colors[16];
	
	if (!left) {
		lx=box->lx;
		rx=box->lx+list_palette_border_sz;
	}
	else {
		lx=box->rx-list_palette_border_sz;
		rx=box->rx;
	}

	vertexes[0]=vertexes[6]=(float)lx;
	vertexes[2]=vertexes[4]=(float)rx;
	vertexes[1]=vertexes[3]=(float)box->ty;
	vertexes[5]=vertexes[7]=(float)box->by;

	colors[0]=colors[1]=colors[12]=colors[13]=0.0f;
	colors[2]=colors[14]=1.0f;
	colors[4]=colors[5]=colors[8]=colors[9]=0.0f;
	colors[6]=colors[10]=0.6f;
	colors[3]=colors[7]=colors[11]=colors[15]=1.0f;

	glVertexPointer(2,GL_FLOAT,0,vertexes);

	glEnableClientState(GL_COLOR_ARRAY);
	glColorPointer(4,GL_FLOAT,0,colors);
	
	glDrawArrays(GL_QUADS,0,4);

	glDisableClientState(GL_COLOR_ARRAY);

	vertexes[0]=vertexes[2]=(float)lx;
	vertexes[4]=vertexes[6]=(float)(rx-1);
	vertexes[1]=vertexes[5]=(float)box->ty;
	vertexes[3]=vertexes[7]=(float)box->by;

	glVertexPointer(2,GL_FLOAT,0,vertexes);
	
	glColor4f(0.0f,0.0f,0.0f,1.0f);
	glDrawArrays(GL_LINES,0,4);
}

void list_palette_pane_disable(d3rect *box)
{
	float				vertexes[8];
	
	vertexes[0]=vertexes[6]=(float)box->lx;
	vertexes[2]=vertexes[4]=(float)box->rx;
	vertexes[1]=vertexes[3]=(float)box->ty;
	vertexes[5]=vertexes[7]=(float)box->by;

	glColor4f(0.4f,0.4f,0.4f,0.5f);

	glVertexPointer(2,GL_FLOAT,0,vertexes);
	glDrawArrays(GL_QUADS,0,4);
	
	glColor4f(0.0f,0.0f,0.0f,1.0f);
}

/* =======================================================

      List Palette Draw
      
======================================================= */

void list_palette_pane_draw_item(list_palette_pane_type *pane,d3rect *box,bool left,int idx)
{
	int							x,y,lx,rx;
	float						vertexes[8];
	bool						selected;
	char						str[256];
	d3col						col;
	list_palette_item_type		*item;

	item=&pane->items[idx];

		// early exits
		
	y=(box->ty+item->y)-pane->scroll_offset;

	if ((y<(box->ty+list_palette_pane_get_title_high(pane))) || ((y-list_item_font_high)>box->by)) return;

		// draw header

	lx=box->lx;
	rx=box->rx-list_palette_scroll_wid;
	if (left) {
		lx-=list_palette_border_sz;
		rx-=list_palette_border_sz;
	}

	if (item->ctrl_type==list_item_ctrl_header) {
		vertexes[0]=vertexes[6]=(float)lx;
		vertexes[2]=vertexes[4]=(float)rx;
		vertexes[1]=vertexes[3]=(float)(y-list_item_font_high);
		vertexes[5]=vertexes[7]=(float)y;

		glVertexPointer(2,GL_FLOAT,0,vertexes);
	
		glColor4f(0.9f,0.9f,0.9f,1.0f);
		glDrawArrays(GL_QUADS,0,4);
	}
		
		// draw selected item
	
	else {

		if (!pane->item_push_on) {
			selected=item->selected;
		}
		else {
			if ((pane->button_push_idx!=list_button_push_type_none)) {
				selected=FALSE;
			}
			else {
				selected=(pane->item_push_idx==idx);
			}
		}

		if (selected) {
			vertexes[0]=vertexes[6]=(float)lx;
			vertexes[2]=vertexes[4]=(float)rx;
			vertexes[1]=vertexes[3]=(float)((y-list_item_font_high)+1);
			vertexes[5]=vertexes[7]=(float)y;

			glVertexPointer(2,GL_FLOAT,0,vertexes);
		
			glColor4f(1.0f,1.0f,0.0f,1.0f);
			glDrawArrays(GL_QUADS,0,4);
		}
	}

		// disable/enable colors

	if (!item->disabled) {
		col.r=col.g=col.b=0.0f;
	}
	else {
		col.r=col.g=0.0f;
		col.b=1.0f;
	}

		// draw item
		
	x=lx+item->x;
	y=(box->ty+item->y)-pane->scroll_offset;

	if (item->moveable) {
		list_palette_pane_draw_item_move_buttons(pane,box,idx);
		x+=22;
	}

	switch (item->ctrl_type) {

			// text or header

		case list_item_ctrl_header:
			text_draw(x,y,list_item_font_size,NULL,item->name);
			list_palette_pane_draw_item_button(pane,box,left,idx);
			break;

		case list_item_ctrl_text:
			text_draw(x,y,list_item_font_size,&col,item->name);
			list_palette_pane_draw_item_button(pane,box,left,idx);
			break;

			// color

		case list_item_ctrl_color:
			text_draw(x,y,list_item_font_size,NULL,"Light");
			list_palette_pane_draw_item_color_box(pane,box,item,item->value.col_ptr);
			break;

			// string

		case list_item_ctrl_string:
			text_draw(x,y,list_item_font_size,&col,item->name);
			list_palette_pane_draw_item_string(pane,box,item,item->value.str_ptr);
			list_palette_pane_draw_item_button(pane,box,left,idx);
			break;

			// param

		case list_item_ctrl_param:
			text_draw(x,y,list_item_font_size,&col,item->name);
			property_get_parameter(item->limit.param_idx,item->value.str_ptr,str);
			list_palette_pane_draw_item_string(pane,box,item,str);
			list_palette_pane_draw_item_button(pane,box,left,idx);
			break;

			// na blank

		case list_item_ctrl_na_blank:
			text_draw(x,y,list_item_font_size,&col,item->name);
			list_palette_pane_draw_item_string(pane,box,item,"n/a");
			break;

			// int

		case list_item_ctrl_int:
			sprintf(str,"%d",*item->value.int_ptr);
			text_draw(x,y,list_item_font_size,&col,item->name);
			list_palette_pane_draw_item_string(pane,box,item,str);
			list_palette_pane_draw_item_button(pane,box,left,idx);
			break;

			// float

		case list_item_ctrl_float:
			sprintf(str,"%.2f",*item->value.float_ptr);
			text_draw(x,y,list_item_font_size,&col,item->name);
			list_palette_pane_draw_item_string(pane,box,item,str);
			list_palette_pane_draw_item_button(pane,box,left,idx);
			break;

			// checkbox

		case list_item_ctrl_checkbox:
			text_draw(x,y,list_item_font_size,NULL,item->name);
			list_palette_pane_draw_item_check_box(pane,box,item,*item->value.bool_ptr);
			break;

			// point

		case list_item_ctrl_point:
			sprintf(str,"%d,%d,%d",item->value.pnt_ptr->x,item->value.pnt_ptr->y,item->value.pnt_ptr->z);
			text_draw(x,y,list_item_font_size,&col,item->name);
			list_palette_pane_draw_item_string(pane,box,item,str);
			list_palette_pane_draw_item_button(pane,box,left,idx);
			break;

			// angle

		case list_item_ctrl_angle:
			sprintf(str,"%.2f,%.2f,%.2f",item->value.ang_ptr->x,item->value.ang_ptr->y,item->value.ang_ptr->z);
			text_draw(x,y,list_item_font_size,&col,item->name);
			list_palette_pane_draw_item_string(pane,box,item,str);
			list_palette_pane_draw_item_button(pane,box,left,idx);
			break;

			// vector

		case list_item_ctrl_vector:
		case list_item_ctrl_normal_vector:
			sprintf(str,"%.2f,%.2f,%.2f",item->value.vct_ptr->x,item->value.vct_ptr->y,item->value.vct_ptr->z);
			text_draw(x,y,list_item_font_size,&col,item->name);
			list_palette_pane_draw_item_string(pane,box,item,str);
			list_palette_pane_draw_item_button(pane,box,left,idx);
			break;

			// uv

		case list_item_ctrl_uv:
			sprintf(str,"%.2f,%.2f",item->value.uv_ptr->x,item->value.uv_ptr->y);
			text_draw(x,y,list_item_font_size,&col,item->name);
			list_palette_pane_draw_item_string(pane,box,item,str);
			list_palette_pane_draw_item_button(pane,box,left,idx);
			break;

			// a list

		case list_item_ctrl_picker:
			list_palette_get_current_picker_value(item,str);
			text_draw(x,y,list_item_font_size,&col,item->name);
			list_palette_pane_draw_item_string(pane,box,item,str);
			list_palette_pane_draw_item_button(pane,box,left,idx);
			break;

			// pick color

		case list_item_ctrl_pick_color:
			text_draw(x,y,list_item_font_size,NULL,item->name);
			list_palette_pane_draw_item_color_box(pane,box,item,item->value.col_ptr);
			break;

	}
	
		// header counts
		
	if (item->ctrl_type==list_item_ctrl_header) {
		if (item->count!=-1) {
			sprintf(str,"%d",item->count);
			text_draw_right((box->rx-(list_palette_scroll_wid+4)),y,list_item_font_size,NULL,str);
		}
	}
}

void list_palette_pane_draw(list_palette_pane_type *pane,d3rect *box,bool draw_border,bool left)
{
	int					n;

		// check if scrolling is bad,
		// usually when a page was switched
		
	if (pane->scroll_offset>list_palette_get_scroll_offset_max(pane,box)) pane->scroll_offset=0;

		// items

	for (n=0;n!=pane->item_count;n++) {
		list_palette_pane_draw_item(pane,box,left,n);
	}
	
		// list pieces

	list_palette_pane_draw_scrollbar(pane,box,left);
	list_palette_pane_draw_title(pane,box);
	if (draw_border) list_palette_draw_border(box,left);
}

void list_palette_draw(list_palette_type *list)
{
	d3rect				box;

		// hidden?

	if (list->hide) return;

		// item pane

	list_palette_item_box(list,&box);
	list_palette_pane_draw_setup(&list->item_pane,&box);
	list_palette_pane_draw(&list->item_pane,&box,(!list->flag.never_open),list->flag.left);
	
	if (list->picker.on) list_palette_pane_disable(&box);

		// picker pane
		
	if (list->picker.on) {
		list_palette_picker_box(list,&box);
		list_palette_pane_draw_setup(&list->picker_pane,&box);
		list_palette_pane_draw(&list->picker_pane,&box,FALSE,FALSE);
	}
	else {
		if (list->flag.never_hide_picker) {
			list_palette_picker_box(list,&box);
			list_palette_pane_draw_setup(&list->picker_pane,&box);
			list_palette_pane_disable(&box);
		}
	}
}

/* =======================================================

      List Palette Scrolling
      
======================================================= */

void list_palette_pane_scroll_up(list_palette_pane_type *pane)
{
	pane->scroll_offset-=list_wheel_scroll_size;
	if (pane->scroll_offset<0) pane->scroll_offset=0;
	
	main_wind_draw();
}

void list_palette_pane_scroll_down(list_palette_pane_type *pane,d3rect *box)
{
	int				scroll_max;
	
	pane->scroll_offset+=list_wheel_scroll_size;
	scroll_max=list_palette_get_scroll_offset_max(pane,box);
	
	if (pane->scroll_offset>scroll_max) pane->scroll_offset=scroll_max;
		
	main_wind_draw();
}

void list_palette_scroll_wheel(list_palette_type *list,d3pnt *pnt,int move)
{
	d3rect				box;

		// point over item list?

	list_palette_item_box(list,&box);
	if ((pnt->x>=box.lx) && (pnt->x<=box.rx)) {
		if (move>0) list_palette_pane_scroll_up(&list->item_pane);
		if (move<0) list_palette_pane_scroll_down(&list->item_pane,&box);
		return;
	}

		// point over picker list?

	if (!list->picker.on) return;

	list_palette_picker_box(list,&box);
	if ((pnt->x>=box.lx) && (pnt->x<=box.rx)) {
		if (move>0) list_palette_pane_scroll_up(&list->picker_pane);
		if (move<0) list_palette_pane_scroll_down(&list->picker_pane,&box);
		return;
	}
}

void list_palette_pane_scroll_item_into_view(list_palette_pane_type *pane,d3rect *box,int item_id,int item_idx)
{
	int							n,y,scroll_max;
	bool						hit;
	list_palette_item_type		*item;

		// find item

	y=0;
	hit=FALSE;

	item=pane->items;

	for (n=0;n!=pane->item_count;n++) {

		y+=list_item_font_high;

		if ((item->id==item_id) && (item->idx==item_idx)) {
			hit=TRUE;
			break;
		}

		item++;
	}
	
	if (!hit) return;

		// bring it into view
		// somewhere near the top if possible

	pane->scroll_offset=y;

	if (pane->scroll_offset<0) pane->scroll_offset=0;
	scroll_max=list_palette_get_scroll_offset_max(pane,box);
	if (pane->scroll_offset>scroll_max) pane->scroll_offset=scroll_max;
}

void list_palette_scroll_item_into_view(list_palette_type *list,int item_id,int item_idx)
{
	d3rect			box;

	list_palette_item_box(list,&box);
	list_palette_pane_scroll_item_into_view(&list->item_pane,&box,item_id,item_idx);
}

/* =======================================================

      Item Palette Click
      
======================================================= */

bool list_palette_pane_click_item(list_palette_type *list,list_palette_pane_type *pane,d3rect *box,int item_idx)
{
	int						lx,rx,y;
	bool					out_box;
	d3pnt					pt;
	list_palette_item_type	*item;

		// no clicked item

	pane->click.item=NULL;

		// get clicked item

	item=&pane->items[item_idx];
	
	if (item->disabled) return(FALSE);

		// do the hold and click
		
	pane->item_push_on=TRUE;
	pane->item_push_idx=item_idx;
	
	lx=(box->rx-list_palette_scroll_wid)-list_item_font_high;
	if (list->flag.left) lx-=list_palette_border_sz;
	rx=box->rx-list_palette_scroll_wid;
	if (list->flag.left) rx-=list_palette_border_sz;

	y=(box->ty+item->y)-pane->scroll_offset;
	
	main_wind_draw();

	while (!os_track_mouse_location(&pt,NULL)) {
	
			// find if in or out of box
			
		out_box=FALSE;
		out_box=out_box||(pt.y<(y-list_item_font_high));
		out_box=out_box||(pt.y>=y);
		
		switch (pane->button_push_idx) {
		
			case list_button_push_type_none:
				out_box=out_box||(pt.x<box->lx);
				out_box=out_box||(pt.x>box->rx);
				break;
				
			case list_button_push_type_button:
				lx=(box->rx-list_palette_scroll_wid)-list_item_font_high;
				if (list->flag.left) lx-=list_palette_border_sz;
				out_box=out_box||(pt.x<lx);
				
				rx=box->rx-list_palette_scroll_wid;
				if (list->flag.left) rx-=list_palette_border_sz;
				out_box=out_box||(pt.x>rx);
				break;
				
			case list_button_push_type_up:
				lx=box->lx+12;
				out_box=out_box||(pt.x<lx);
				out_box=out_box||(pt.x>(lx+list_item_font_high));
				break;
				
			case list_button_push_type_down:
				lx=(box->lx+12)+list_item_font_high;
				out_box=out_box||(pt.x<lx);
				out_box=out_box||(pt.x>(lx+list_item_font_high));
				break;

		}
		
			// change selection
			
		if (out_box) {
			if (pane->item_push_idx!=-1) {
				pane->item_push_idx=-1;
				main_wind_draw();
			}
		}
		else {
			if (pane->item_push_idx!=item_idx) {
				pane->item_push_idx=item_idx;
				main_wind_draw();
			}
		}
		
		usleep(10000);
	}

	pane->item_push_on=FALSE;

	if (pane->item_push_idx!=item_idx) {
		main_wind_draw();
		return(FALSE);
	}

		// pass back clicked item
		
	switch (pane->button_push_idx) {
		case list_button_push_type_none:
			pane->click.id=item->id;
			break;
		case list_button_push_type_button:
			pane->click.id=item->button_id;
			break;
		case list_button_push_type_up:
			pane->click.id=item->button_up_id;
			break;
		case list_button_push_type_down:
			pane->click.id=item->button_down_id;
			break;
	}

	pane->click.idx=item->idx;

	pane->click.item_idx=item_idx;
	pane->click.item=item;

	return(TRUE);
}

bool list_palette_pane_click_back(list_palette_pane_type *pane,d3rect *box)
{
	bool					on,out_box,by;
	d3pnt					pt;

		// no click if level==0

	if (pane->level==0) return(FALSE);

		// do the hold and click
		
	by=box->ty+list_palette_pane_get_title_high(pane);

	pane->back_push_on=TRUE;
	
	main_wind_draw();

	while (!os_track_mouse_location(&pt,NULL)) {
	
		out_box=FALSE;
		out_box=out_box||(pt.x<box->lx);
		out_box=out_box||(pt.x>box->rx);
		out_box=out_box||(pt.y<box->ty);
		out_box=out_box||(pt.y>by);
		
		pane->back_push_on=!out_box;
		main_wind_draw();
		
		usleep(10000);
	}

	on=pane->back_push_on;
	pane->back_push_on=FALSE;

	return(on);
}

void list_palette_pane_click_scroll_bar(list_palette_pane_type *pane,d3rect *box)
{
	int						y,scroll_view_pixel_sz,
							scroll_offset,old_scroll_offset,
							scroll_max,
							thumb_ty,thumb_by;
	d3pnt					pt,org_pt;
	
		// scrolling sizes
		
	scroll_max=list_palette_get_scroll_offset_max(pane,box);
	if (scroll_max==0) return;
	
	list_palette_get_scroll_thumb_position(pane,box,&thumb_ty,&thumb_by);
	
		// clicking point
		
	os_get_cursor(&org_pt);

		// are we clicking above the thumb?
		
	if (org_pt.y<thumb_ty) {
		pane->scroll_offset-=list_palette_pane_get_scroll_thumb_page_offset(pane,box);
		if (pane->scroll_offset<0) pane->scroll_offset=0;
		main_wind_draw();
		return;
	}
	
	if (org_pt.y>thumb_by) {
		pane->scroll_offset+=list_palette_pane_get_scroll_thumb_page_offset(pane,box);
		if (pane->scroll_offset>scroll_max) pane->scroll_offset=scroll_max;
		main_wind_draw();
		return;
	}
	
		// otherwise we are dragging the thumb

	scroll_view_pixel_sz=(box->by-box->ty)-list_palette_pane_get_title_high(pane);
		
	old_scroll_offset=pane->scroll_offset;
	
	while (!os_track_mouse_location(&pt,NULL)) {

			// any movements?

		if (pt.y==org_pt.y) continue;
		
			// mark as first drag and move
			
		y=((pt.y-org_pt.y)*pane->total_high)/scroll_view_pixel_sz;

		scroll_offset=old_scroll_offset+y;
		if (scroll_offset<0) scroll_offset=0;
		if (scroll_offset>scroll_max) scroll_offset=scroll_max;
		
		if (scroll_offset!=pane->scroll_offset) {
			pane->scroll_offset=scroll_offset;
			main_wind_draw();
		}
		
		usleep(10000);
	}
}

bool list_palette_pane_click(list_palette_type *list,list_palette_pane_type *pane,d3rect *box,d3pnt *pnt)
{
	int						x,item_idx;
	d3pnt					pt;
	list_palette_item_type	*item;

	pt.x=pnt->x-box->lx;
	pt.y=pnt->y-box->ty;
	
		// click in title
		
	if (pt.y<list_palette_pane_get_title_high(pane)) {
		if (list_palette_pane_click_back(pane,box)) {
			if (list->picker.on) {
				list->picker.on=FALSE;
			}
			else {
				list_palette_set_level(list,(pane->level-1));
			}
			main_wind_draw();
		}
		return(FALSE);
	}
	
		// click in scroll bar
		
	x=(box->rx-box->lx)-list_palette_scroll_wid;
	if (list->flag.left) x-=list_palette_border_sz;

	if (pt.x>=x) {
		list_palette_pane_click_scroll_bar(pane,box);
		return(FALSE);
	}

		// click in item

	item_idx=((pt.y-list_palette_pane_get_title_high(pane))+pane->scroll_offset)/list_item_font_high;
	if ((item_idx<0) || (item_idx>=pane->item_count)) return(FALSE);

		// is there a button

	pane->button_push_idx=list_button_push_type_none;

	if (pane->items[item_idx].button_type!=list_button_none) {
		x=((box->rx-box->lx)-list_palette_scroll_wid)-list_item_font_high;
		if (list->flag.left) x-=list_palette_border_sz;

		if (pt.x>=x) pane->button_push_idx=list_button_push_type_button;
	}
	
	if (pane->items[item_idx].moveable) {
		if (pt.x<=(12+list_item_font_high)) {
			pane->button_push_idx=list_button_push_type_up;
		}
		else {
			if (pt.x<(12+(list_item_font_high*2))) {
				pane->button_push_idx=list_button_push_type_down;
			}
		}
	}

		// get clicked item

	item=&pane->items[item_idx];

		// if a header or an id of -1, only click if there's
		// a button

	if (item->ctrl_type==list_item_ctrl_header) {
		if (pane->button_push_idx!=list_button_push_type_button) return(FALSE);
	}

		// run the click

	return(list_palette_pane_click_item(list,pane,box,item_idx));
}

bool list_palette_click(list_palette_type *list,d3pnt *pnt,bool double_click)
{
	char					str[256];
	d3rect					box;
	d3fpnt					uv_ptr;
	list_palette_item_type	*item;

		// hidden?

	if (list->hide) return(FALSE);

		// click in close border

	list_palette_border_box(list,&box);

	if ((pnt->x>=box.lx) && (pnt->x<=box.rx)) {
		list->open=!list->open;
		main_wind_draw();
		return(FALSE);
	}

		// clicking in picker list

	if (list->picker.on) {
		list_palette_picker_box(list,&box);

		if ((pnt->x>=box.lx) && (pnt->x<=box.rx)) {
			if (!list_palette_pane_click(list,&list->picker_pane,&box,pnt)) return(FALSE);
			list_palette_click_picker(list);
			return(TRUE);
		}

		return(FALSE);
	}

		// special check to catch and
		// ignore double clicks that closed
		// the picker list

	if (double_click) {
		if (list->picker.catch_last_click) {
			list->picker.catch_last_click=FALSE;
			return(FALSE);
		}
	}

	list->picker.catch_last_click=FALSE;

		// clicking in item list

	list_palette_item_box(list,&box);
	if ((pnt->x<box.lx) || (pnt->x>box.rx)) return(FALSE);

	if (!list_palette_pane_click(list,&list->item_pane,&box,pnt)) return(FALSE);

		// button clicks just return

	if (list->item_pane.button_push_idx!=list_button_push_type_none) return(TRUE);

		// any items clicked?

	item=list->item_pane.click.item;
	if (item==NULL) return(FALSE);

		// change the data

	switch (item->ctrl_type) {

		case list_item_ctrl_string:
			if (!double_click) break;
			if (item->value.str_ptr==NULL) return(TRUE);
			dialog_property_string_run(list_string_value_string,(void*)item->value.str_ptr,item->limit.str_len,0,0);
			main_wind_draw();
			return(TRUE);

		case list_item_ctrl_param:
			if (!double_click) break;
			property_get_parameter(item->limit.param_idx,item->value.str_ptr,str);
			dialog_property_string_run(list_string_value_string,(void*)str,item->limit.str_len,0,0);
			property_set_parameter(item->limit.param_idx,item->value.str_ptr,str);
			main_wind_draw();
			return(TRUE);
		
		case list_item_ctrl_int:
			if (!double_click) break;
			dialog_property_string_run(list_string_value_int,(void*)item->value.int_ptr,0,0,0);
			main_wind_draw();
			return(TRUE);

		case list_item_ctrl_float:
			if (!double_click) break;
			dialog_property_string_run(list_string_value_float,(void*)item->value.float_ptr,0,0,0);
			main_wind_draw();
			return(TRUE);

		case list_item_ctrl_checkbox:
			*item->value.bool_ptr=!(*item->value.bool_ptr);
			main_wind_draw();
			return(TRUE);

		case list_item_ctrl_point:
			if (!double_click) break;
			dialog_property_chord_run(list_chord_value_point,(void*)item->value.pnt_ptr);
			main_wind_draw();
			return(TRUE);
		
		case list_item_ctrl_angle:
			if (!double_click) break;
			dialog_property_chord_run(list_chord_value_angle,(void*)item->value.ang_ptr);
			main_wind_draw();
			return(TRUE);

		case list_item_ctrl_vector:
			if (!double_click) break;
			dialog_property_chord_run(list_chord_value_vector,(void*)item->value.vct_ptr);
			main_wind_draw();
			return(TRUE);

		case list_item_ctrl_normal_vector:
			if (!double_click) break;
			dialog_property_chord_run(list_chord_value_vector,(void*)item->value.vct_ptr);
			vector_normalize(item->value.vct_ptr);
			main_wind_draw();
			return(TRUE);

		case list_item_ctrl_uv:
			if (!double_click) break;
			uv_ptr.x=item->value.uv_ptr->x;
			uv_ptr.y=item->value.uv_ptr->y;
			dialog_property_chord_run(list_chord_value_uv,&uv_ptr);
			item->value.uv_ptr->x=uv_ptr.x;
			item->value.uv_ptr->y=uv_ptr.y;
			main_wind_draw();
			return(TRUE);

		case list_item_ctrl_picker:
			if (!double_click) break;
			list_palette_start_picking_item_mode(list,item);
			return(TRUE);

		case list_item_ctrl_pick_color:
			os_pick_color(item->value.col_ptr);
			main_wind_draw();
			return(TRUE);

	}

	return(TRUE);
}