/****************************** File *********************************

Module: dim3 Base Utility
Author: Brian Barnes
 Usage: Interface XML

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
	#include "dim3baseutility.h"
#endif

extern file_path_setup_type		file_path_setup;

char							just_mode_str[][32]=text_just_list_def,
								text_special_str[][32]=text_special_list_def,
								bar_type_str[][32]=bar_type_list_def,
								score_format_str[][32]=score_format_list_def;

#ifdef D3_OS_IPHONE
	extern bool cocoa_is_ipad(void);
#endif

/* =======================================================

      Read Bitmap XML
      
======================================================= */

void iface_read_settings_bitmap(iface_type *iface,int bitmap_tag)
{
	int					tag;
	iface_bitmap_type	*bitmap;
    
		// read next bitmap

	if (iface->bitmap_list.nbitmap>max_iface_bitmap) return;
		
	bitmap=&iface->bitmap_list.bitmaps[iface->bitmap_list.nbitmap];
	iface->bitmap_list.nbitmap++;
	
		// read attributes
		
	xml_get_attribute_text(bitmap_tag,"name",bitmap->name,name_str_len);
	
	tag=xml_findfirstchild("Image",bitmap_tag);
	if (tag==-1) return;
	
	xml_get_attribute_text(tag,"file",bitmap->filename,file_str_len);
	bitmap->animate.image_count=xml_get_attribute_int_default(tag,"count",1);
	bitmap->animate.image_per_row=(int)sqrtf((float)bitmap->animate.image_count);
	bitmap->animate.msec=xml_get_attribute_int(tag,"time");
    bitmap->animate.loop=xml_get_attribute_boolean(tag,"loop");
    bitmap->animate.loop_back=xml_get_attribute_boolean(tag,"loop_back");
	bitmap->rot=xml_get_attribute_float_default(tag,"rot",0.0f);
	
	bitmap->show_tick=0;

	bitmap->pnt.x=bitmap->pnt.y=0;
	
	tag=xml_findfirstchild("Position",bitmap_tag);
	if (tag!=-1) {
		bitmap->pnt.x=xml_get_attribute_int(tag,"x");
		bitmap->pnt.y=xml_get_attribute_int(tag,"y");
	}
	
	bitmap->size.x=bitmap->size.y=-1;
	
	tag=xml_findfirstchild("Size",bitmap_tag);
	if (tag!=-1) {
		bitmap->size.x=xml_get_attribute_int(tag,"width");
		bitmap->size.y=xml_get_attribute_int(tag,"height");
	}
	
	bitmap->alpha=1.0f;
	bitmap->flip_horz=bitmap->flip_vert=FALSE;
	bitmap->team_tint=FALSE;
	bitmap->show=TRUE;
	bitmap->checkpoint=FALSE;
	
	tag=xml_findfirstchild("Settings",bitmap_tag);
	if (tag!=-1) {
		bitmap->alpha=xml_get_attribute_float_default(tag,"alpha",1.0f);
		bitmap->flip_horz=xml_get_attribute_boolean(tag,"flip_horizontal");
		bitmap->flip_vert=xml_get_attribute_boolean(tag,"flip_vertical");
		bitmap->team_tint=xml_get_attribute_boolean(tag,"team_tint");
		bitmap->show=!xml_get_attribute_boolean(tag,"hide");
		bitmap->flash=xml_get_attribute_boolean(tag,"flash");
		bitmap->checkpoint=xml_get_attribute_boolean(tag,"checkpoint");
	}
	
	bitmap->repeat.on=FALSE;
	bitmap->repeat.count=0;
	bitmap->repeat.x_add=bitmap->repeat.y_add=0;
	bitmap->repeat.col=bitmap->repeat.row=0;
	
	tag=xml_findfirstchild("Repeat",bitmap_tag);
	if (tag!=-1) {
		bitmap->repeat.on=xml_get_attribute_boolean(tag,"on");
		bitmap->repeat.count=xml_get_attribute_int(tag,"count");
		bitmap->repeat.x_add=xml_get_attribute_int(tag,"x");
		bitmap->repeat.y_add=xml_get_attribute_int(tag,"y");
		bitmap->repeat.col=xml_get_attribute_int(tag,"col");
		bitmap->repeat.row=xml_get_attribute_int(tag,"row");
	}
	
	bitmap->fade.on=FALSE;
	bitmap->fade.fade_in_tick=bitmap->fade.life_tick=bitmap->fade.fade_out_tick=100;
	
	tag=xml_findfirstchild("Fade",bitmap_tag);
	if (tag!=-1) {
		bitmap->fade.on=xml_get_attribute_boolean(tag,"on");
		bitmap->fade.fade_in_tick=xml_get_attribute_int(tag,"fade_in_msec");
		bitmap->fade.life_tick=xml_get_attribute_int(tag,"life_msec");
		bitmap->fade.fade_out_tick=xml_get_attribute_int(tag,"fade_out_msec");
	}
}

/* =======================================================

      Read Text XML
      
======================================================= */

void iface_read_settings_text(iface_type *iface,int text_tag)
{
	int					tag;
	char				data[max_hud_text_str_sz];
	iface_text_type		*text;
	
		// read next text
	
	if (iface->text_list.ntext>=max_iface_text) return;
	
	text=&iface->text_list.texts[iface->text_list.ntext];
	iface->text_list.ntext++;
	
		// read attributes

	xml_get_attribute_text(text_tag,"name",text->name,name_str_len);
	
	text->pnt.x=text->pnt.y=0;
	
	tag=xml_findfirstchild("Position",text_tag);
	if (tag!=-1) {
		text->pnt.x=xml_get_attribute_int(tag,"x");
		text->pnt.y=xml_get_attribute_int(tag,"y");
	}
	
	text->size=iface->font.text_size_small;
	text->color.r=text->color.g=text->color.b=1;
	text->show=TRUE;
	text->monospaced=FALSE;
	text->checkpoint=FALSE;
	text->special=text_special_none;
	text->just=tx_left;
	text->alpha=1.0f;

	data[0]=0x0;
	
	tag=xml_findfirstchild("Settings",text_tag);
	if (tag!=-1) {
		text->alpha=xml_get_attribute_float_default(tag,"alpha",1.0f);
		xml_get_attribute_text(tag,"data",data,max_hud_text_str_sz);
		text->size=xml_get_attribute_int_default(tag,"size",iface->font.text_size_small);
		xml_get_attribute_color(tag,"color",&text->color);
		text->just=xml_get_attribute_list(tag,"just",(char*)just_mode_str);
		text->show=!xml_get_attribute_boolean(tag,"hide");
		text->monospaced=xml_get_attribute_boolean(tag,"monospaced");
		text->checkpoint=xml_get_attribute_boolean(tag,"checkpoint");
		text->special=xml_get_attribute_list(tag,"special",(char*)text_special_str);
	}

	text->fade.on=FALSE;
	text->fade.fade_in_tick=text->fade.life_tick=text->fade.fade_out_tick=100;
	
	tag=xml_findfirstchild("Fade",text_tag);
	if (tag!=-1) {
		text->fade.on=xml_get_attribute_boolean(tag,"on");
		text->fade.fade_in_tick=xml_get_attribute_int(tag,"fade_in_msec");
		text->fade.life_tick=xml_get_attribute_int(tag,"life_msec");
		text->fade.fade_out_tick=xml_get_attribute_int(tag,"fade_out_msec");
	}

	iface_text_set(text,data);
}

/* =======================================================

      Read Bar XML
      
======================================================= */

void iface_read_settings_bar(iface_type *iface,int bar_tag)
{
	int					tag;
	iface_bar_type		*bar;
    
		// read next bar

	if (iface->bar_list.nbar>=max_iface_bar) return;
		
	bar=&iface->bar_list.bars[iface->bar_list.nbar];
	iface->bar_list.nbar++;
	
		// read attributes
		
	xml_get_attribute_text(bar_tag,"name",bar->name,name_str_len);
	bar->type=xml_get_attribute_list(bar_tag,"type",(char*)bar_type_str);
		
	bar->pnt.x=bar->pnt.y=0;
	
	tag=xml_findfirstchild("Position",bar_tag);
	if (tag!=-1) {
		bar->pnt.x=xml_get_attribute_int(tag,"x");
		bar->pnt.y=xml_get_attribute_int(tag,"y");
	}
	
	bar->size.x=bar->size.y=-1;
	
	tag=xml_findfirstchild("Size",bar_tag);
	if (tag!=-1) {
		bar->size.x=xml_get_attribute_int(tag,"width");
		bar->size.y=xml_get_attribute_int(tag,"height");
	}
	
	bar->outline=FALSE;
	bar->outline_alpha=1;
	bar->outline_color.r=bar->outline_color.g=bar->outline_color.b=0;
	
	tag=xml_findfirstchild("Outline",bar_tag);
	if (tag!=-1) {
		bar->outline=xml_get_attribute_boolean(tag,"on");
		bar->outline_alpha=xml_get_attribute_float_default(tag,"alpha",1.0f);
		xml_get_attribute_color(tag,"color",&bar->outline_color);
	}

	bar->background=FALSE;
	bar->background_alpha=1;
	bar->background_color.r=bar->background_color.g=bar->background_color.b=0;
	
	tag=xml_findfirstchild("Background",bar_tag);
	if (tag!=-1) {
		bar->background=xml_get_attribute_boolean(tag,"on");
		bar->background_alpha=xml_get_attribute_float_default(tag,"alpha",1.0f);
		xml_get_attribute_color(tag,"color",&bar->background_color);
	}
	
	bar->fill_alpha=1;
	bar->fill_start_color.r=bar->fill_start_color.g=bar->fill_start_color.b=0;
	bar->fill_end_color.r=bar->fill_end_color.g=bar->fill_end_color.b=0;
	
	tag=xml_findfirstchild("Fill",bar_tag);
	if (tag!=-1) {
		bar->fill_alpha=xml_get_attribute_float_default(tag,"alpha",1.0f);
		xml_get_attribute_color(tag,"start_color",&bar->fill_start_color);
		xml_get_attribute_color(tag,"end_color",&bar->fill_end_color);
	}
	
	bar->show=TRUE;
	
	tag=xml_findfirstchild("Settings",bar_tag);
	if (tag!=-1) {
		bar->show=!xml_get_attribute_boolean(tag,"hide");
	}
	
		// value starts at 1.0
		// and no countdown
		
	bar->value=1.0f;
	
	bar->countdown.start_tick=0;
	bar->countdown.msec=0;
}

/* =======================================================

      Read Virtual Control XML
      
======================================================= */

void iface_read_settings_virtual_control(iface_type *iface,int virtual_head_tag)
{
	int							n,head_tag,tag;
	iface_virtual_stick_type	*stick;
	iface_virtual_button_type	*button;

		// virtual control sticks

	head_tag=xml_findfirstchild("Sticks",virtual_head_tag);
	if (head_tag!=-1) {
	
		tag=xml_findfirstchild("Stick",head_tag);

		stick=iface->virtual_control.sticks;

		for (n=0;n!=max_virtual_stick;n++) {
			if (tag==-1) break;

			stick->on=xml_get_attribute_boolean(tag,"on");
			stick->show=TRUE;
			stick->click_control_idx=xml_get_attribute_int_default(tag,"click_control",-1);
			stick->use_bitmap=xml_get_attribute_boolean(tag,"use_bitmap");
			stick->pnt.x=xml_get_attribute_int(tag,"x");
			stick->pnt.y=xml_get_attribute_int(tag,"y");
			stick->size.x=xml_get_attribute_int(tag,"x_size");
			stick->size.y=xml_get_attribute_int(tag,"y_size");
			stick->flip_x=xml_get_attribute_boolean(tag,"x_flip");
			stick->flip_y=xml_get_attribute_boolean(tag,"y_flip");
			xml_get_attribute_text(tag,"outer_bitmap",stick->outer_bitmap_name,file_str_len);
			xml_get_attribute_text(tag,"inner_bitmap",stick->inner_bitmap_name,file_str_len);
			xml_get_attribute_color(tag,"color",&stick->color);

			stick++;
			tag=xml_findnextchild(tag);
		}
	}
	
		// virtual control buttons
		
	head_tag=xml_findfirstchild("Buttons",virtual_head_tag);
	if (head_tag!=-1) {
	
		tag=xml_findfirstchild("Button",head_tag);

		button=iface->virtual_control.buttons;

		for (n=0;n!=max_virtual_button;n++) {
			if (tag==-1) break;

			button->on=xml_get_attribute_boolean(tag,"on");
			button->show=TRUE;
			button->sticky=xml_get_attribute_boolean(tag,"sticky");
			button->use_bitmap=xml_get_attribute_boolean(tag,"use_bitmap");
			button->control_idx=xml_get_attribute_int_default(tag,"control",0);
			button->pnt.x=xml_get_attribute_int(tag,"x");
			button->pnt.y=xml_get_attribute_int(tag,"y");
			button->size.x=xml_get_attribute_int(tag,"x_size");
			button->size.y=xml_get_attribute_int(tag,"y_size");
			xml_get_attribute_text(tag,"up_bitmap",button->up_bitmap_name,file_str_len);
			xml_get_attribute_text(tag,"down_bitmap",button->down_bitmap_name,file_str_len);
			xml_get_attribute_color(tag,"color",&button->color);

			button++;
			tag=xml_findnextchild(tag);
		}
	}
}

/* =======================================================

      Read Radar XML
      
======================================================= */

void iface_read_settings_radar(iface_type *iface,int radar_tag)
{
	int					tag,icons_head_tag,icon_tag;

		// defaults

	iface->radar.on=TRUE;
	iface->radar.rot=TRUE;
	iface->radar.no_motion_fade=0;
	
	iface->radar.pnt.x=iface->radar.pnt.y=0;
	iface->radar.display_radius=32;
	iface->radar.view_radius=15000;
	iface->radar.nicon=0;
	iface->radar.background_bitmap_name[0]=0x0;

		// read settings

	tag=xml_findfirstchild("Setting",radar_tag);
	if (tag!=-1) {
		iface->radar.on=xml_get_attribute_boolean(tag,"on");		// only use on switch if it exists
		iface->radar.rot=!xml_get_attribute_boolean(tag,"no_rot");
		iface->radar.no_motion_fade=xml_get_attribute_int(tag,"no_motion_fade");
	}

	tag=xml_findfirstchild("Position",radar_tag);
	if (tag!=-1) {
		iface->radar.pnt.x=xml_get_attribute_int(tag,"x");
		iface->radar.pnt.y=xml_get_attribute_int(tag,"y");
		iface->radar.display_radius=xml_get_attribute_int(tag,"radius");
	}

	tag=xml_findfirstchild("View",radar_tag);
	if (tag!=-1) {
		iface->radar.view_radius=xml_get_attribute_int(tag,"radius");
	}

	tag=xml_findfirstchild("Background",radar_tag);
	if (tag!=-1) {
		xml_get_attribute_text(tag,"file",iface->radar.background_bitmap_name,name_str_len);
	}

		// get the icons

	icons_head_tag=xml_findfirstchild("Icons",radar_tag);
	if (icons_head_tag==-1) return;
	
	icon_tag=xml_findfirstchild("Icon",icons_head_tag);
	
	while (icon_tag!=-1) {
		xml_get_attribute_text(icon_tag,"name",iface->radar.icons[iface->radar.nicon].name,name_str_len);
		xml_get_attribute_text(icon_tag,"file",iface->radar.icons[iface->radar.nicon].bitmap_name,name_str_len);
		iface->radar.icons[iface->radar.nicon].size=xml_get_attribute_int(icon_tag,"size");
		iface->radar.icons[iface->radar.nicon].rot=xml_get_attribute_boolean(icon_tag,"rot");
		iface->radar.nicon++;

		icon_tag=xml_findnextchild(icon_tag);
	}
}

/* =======================================================

      Read Menu XML
      
======================================================= */

void iface_read_settings_menu(iface_type *iface,int menu_tag)
{
	int						items_head_tag,item_tag;
	iface_menu_type			*menu;
	iface_menu_item_type	*item;

		// read next menu

	menu=&iface->menu_list.menus[iface->menu_list.nmenu];
	iface->menu_list.nmenu++;

	xml_get_attribute_text(menu_tag,"name",menu->name,name_str_len);
	menu->nitem=0;

		// read menu

	items_head_tag=xml_findfirstchild("Items",menu_tag);
	if (items_head_tag==-1) return;
	
	item_tag=xml_findfirstchild("Item",items_head_tag);
	
	while (item_tag!=-1) {

		if (menu->nitem>=max_menu_item) return;
	
		item=&menu->items[menu->nitem];
		menu->nitem++;

		item->item_id=xml_get_attribute_int(item_tag,"id");
		xml_get_attribute_text(item_tag,"data",item->data,max_menu_item_data_sz);
		xml_get_attribute_text(item_tag,"sub_menu",item->sub_menu,name_str_len);
		item->multiplayer_disable=xml_get_attribute_boolean(item_tag,"multiplayer_disable");
		item->quit=xml_get_attribute_boolean(item_tag,"quit");
		
		item_tag=xml_findnextchild(item_tag);
	}
}

/* =======================================================

      Read Intro XML
      
======================================================= */

void iface_read_settings_intro_button(int tag,iface_intro_button_type *btn)
{
	if (tag==-1) return;
	
	btn->x=xml_get_attribute_int(tag,"x");
	btn->y=xml_get_attribute_int(tag,"y");
	btn->mobile_x=xml_get_attribute_int_default(tag,"mobile_x",-1);
	btn->mobile_y=xml_get_attribute_int_default(tag,"mobile_y",-1);
	btn->wid=xml_get_attribute_int_default(tag,"width",-1);
	btn->high=xml_get_attribute_int_default(tag,"height",-1);
	btn->on=xml_get_attribute_boolean(tag,"on");
	btn->mobile_hide=xml_get_attribute_boolean(tag,"mobile_hide");
}

void iface_read_settings_intro_model(iface_type *iface,int tag)
{
	iface_intro_model_type		*intro_model;

	if (iface->intro.model_list.nmodel==max_hud_intro_model) return;

	intro_model=&iface->intro.model_list.models[iface->intro.model_list.nmodel];
	iface->intro.model_list.nmodel++;

	xml_get_attribute_text(tag,"model",intro_model->model_name,name_str_len);
	xml_get_attribute_text(tag,"animate",intro_model->animate_name,name_str_len);
	intro_model->x=xml_get_attribute_int(tag,"x");
	intro_model->y=xml_get_attribute_int(tag,"y");
	xml_get_attribute_3_coord_float(tag,"rot",&intro_model->rot.x,&intro_model->rot.y,&intro_model->rot.z);
	intro_model->resize=xml_get_attribute_float_default(tag,"resize",1.0f);
}

/* =======================================================

      Read Device XML
      
======================================================= */

void iface_read_settings_device(int tag,iface_device_type *device)
{
	int				scale_tag,render_tag;

	scale_tag=xml_findfirstchild("Scale",tag);
	if (scale_tag!=-1) {
		device->scale.control_padding=xml_get_attribute_float(scale_tag,"control_padding");
		device->scale.control_high=xml_get_attribute_float(scale_tag,"control_height");
		device->scale.control_short_wid=xml_get_attribute_float(scale_tag,"control_short_width");
		device->scale.control_long_wid=xml_get_attribute_float(scale_tag,"control_long_width");
		device->scale.control_scroll_size=xml_get_attribute_float(scale_tag,"control_scroll_size");
		device->scale.button_high=xml_get_attribute_float(scale_tag,"button_height");
		device->scale.button_short_wid=xml_get_attribute_float(scale_tag,"button_short_width");
		device->scale.button_long_wid=xml_get_attribute_float(scale_tag,"button_long_width");
		device->scale.dialog_margin=xml_get_attribute_float_default(scale_tag,"dialog_margin",0.02f);
		device->scale.tab_high=xml_get_attribute_float(scale_tag,"tab_height");
	}
	
	render_tag=xml_findfirstchild("Render",tag);
	if (render_tag!=-1) {
		device->render.fov_adjust=xml_get_attribute_float(render_tag,"fov_adjust");
	}
}

/* =======================================================

      Read Interface XML
	  For Project Name, so we can put it in the
	  File Path so project name can override the application's
	  name (therefore we can have multiple data files
	  per same application)
      
======================================================= */

void iface_read_settings_project_name(file_path_setup_type *path_setup)
{
	int						interface_head_tag,proj_tag;
	char					path[1024];
	
		// default project name is blank
		// (will use application name)
		
	path_setup->proj_name[0]=0x0;

		// read in interface from setting files
		
	file_paths_data(path_setup,path,"Settings","Interface","xml");
	if (!xml_open_file(path)) return;
	
		// decode the file
      
    interface_head_tag=xml_findrootchild("Interface");
    if (interface_head_tag==-1) {
		xml_close_file();
		return;
	}

		// project setup
		
	proj_tag=xml_findfirstchild("Project",interface_head_tag);
	if (proj_tag!=-1) {
		xml_get_attribute_text(proj_tag,"name",path_setup->proj_name,256);
	}
	

	xml_close_file();
}

/* =======================================================

      Read Interface XML
      
======================================================= */

void iface_read_settings_interface(iface_type *iface)
{
	int							n,interface_head_tag,scale_tag,device_head_tag,device_tag,
								bitmap_head_tag,bitmap_tag,text_head_tag,text_tag,bar_head_tag,bar_tag,
								virtual_head_tag,radar_head_tag,menu_head_tag,menu_tag,title_tag,
								simple_save_tag,score_tag,preload_tag,preload_models_tag,preload_model_tag,
								intro_head_tag,intro_model_head_tag,intro_model_tag,
								color_tag,font_tag,progress_tag,chat_tag,fade_tag,button_tag,sound_tag,
								proj_tag,setup_tag;
	char						path[1024],i_name[256],name[256];

	iface_default_settings(iface);
	
		// read in interface from setting files

	iface_xml_substitution("Interface",i_name);
		
	file_paths_data(&file_path_setup,path,"Settings",i_name,"xml");
	if (!xml_open_file(path)) return;
	
		// decode the file
      
    interface_head_tag=xml_findrootchild("Interface");
    if (interface_head_tag==-1) {
		xml_close_file();
		return;
	}

		// scale

	scale_tag=xml_findfirstchild("Scale",interface_head_tag);
	if (scale_tag!=-1) {
		iface->scale_x=xml_get_attribute_int_default(scale_tag,"x",640);
		iface->scale_y=xml_get_attribute_int_default(scale_tag,"y",480);
	}

		// devices

	device_head_tag=xml_findfirstchild("Devices",interface_head_tag);
	if (device_head_tag!=-1) {

		device_tag=xml_findfirstchild("Device",device_head_tag);

		for (n=0;n!=max_device;n++) {
			if (device_tag==-1) break;

			iface_read_settings_device(device_tag,&iface->devices[n]);
			device_tag=xml_findnextchild(device_tag);
		}
	}

		// some initial text sizes

	iface->font.text_size_mini=(int)(((float)iface->scale_x)*iface_text_mini_wid_factor);
	iface->font.text_size_small=(int)(((float)iface->scale_x)*iface_text_small_wid_factor);
	iface->font.text_size_medium=(int)(((float)iface->scale_x)*iface_text_medium_wid_factor);
	iface->font.text_size_large=(int)(((float)iface->scale_x)*iface_text_large_wid_factor);
	
		// bitmaps
	
    bitmap_head_tag=xml_findfirstchild("Bitmaps",interface_head_tag);
    if (bitmap_head_tag!=-1) {
	
		bitmap_tag=xml_findfirstchild("Bitmap",bitmap_head_tag);
		
		while (bitmap_tag!=-1) {
			iface_read_settings_bitmap(iface,bitmap_tag);
			bitmap_tag=xml_findnextchild(bitmap_tag);
		}
	}
	
		// text
	
	text_head_tag=xml_findfirstchild("Texts",interface_head_tag);
    if (text_head_tag!=-1) {
	
		text_tag=xml_findfirstchild("Text",text_head_tag);
		
		while (text_tag!=-1) {
			iface_read_settings_text(iface,text_tag);
			text_tag=xml_findnextchild(text_tag);
		}
	}
	
		// bars
	
    bar_head_tag=xml_findfirstchild("Bars",interface_head_tag);
    if (bar_head_tag!=-1) {
	
		bar_tag=xml_findfirstchild("Bar",bar_head_tag);
		
		while (bar_tag!=-1) {
			iface_read_settings_bar(iface,bar_tag);
			bar_tag=xml_findnextchild(bar_tag);
		}
	}
	
		// virtual controls

	virtual_head_tag=xml_findfirstchild("Virtual_Control",interface_head_tag);
	if (virtual_head_tag!=-1) iface_read_settings_virtual_control(iface,virtual_head_tag);

		// radar

	radar_head_tag=xml_findfirstchild("Radar",interface_head_tag);
	if (radar_head_tag!=-1) iface_read_settings_radar(iface,radar_head_tag);
	
		// menus
		
	menu_head_tag=xml_findfirstchild("Menus",interface_head_tag);
    if (menu_head_tag!=-1) {
	
		menu_tag=xml_findfirstchild("Menu",menu_head_tag);
		
		while (menu_tag!=-1) {
			iface_read_settings_menu(iface,menu_tag);
			menu_tag=xml_findnextchild(menu_tag);
		}
	}
	
		// colors
		
	color_tag=xml_findfirstchild("Color",interface_head_tag);
	if (color_tag!=-1) {
		xml_get_attribute_color(color_tag,"background",&iface->color.background);
		xml_get_attribute_color(color_tag,"menu_text",&iface->color.menu.text);
		xml_get_attribute_color(color_tag,"menu_mouse_over",&iface->color.menu.mouse_over);
		xml_get_attribute_color(color_tag,"menu_background",&iface->color.menu.background);
		xml_get_attribute_color(color_tag,"menu_outline",&iface->color.menu.outline);
		xml_get_attribute_color(color_tag,"dialog_background",&iface->color.dialog.background);
		xml_get_attribute_color(color_tag,"dialog_header",&iface->color.dialog.header);
		xml_get_attribute_color(color_tag,"dialog_outline",&iface->color.dialog.outline);
		xml_get_attribute_color(color_tag,"dialog_title",&iface->color.dialog.title);
		xml_get_attribute_color(color_tag,"tab_background",&iface->color.tab.background);
		xml_get_attribute_color(color_tag,"tab_dimmed",&iface->color.tab.dimmed);
		xml_get_attribute_color(color_tag,"tab_outline",&iface->color.tab.outline);
		xml_get_attribute_color(color_tag,"tab_text",&iface->color.tab.text);
		xml_get_attribute_color(color_tag,"tab_text_mouse_over",&iface->color.tab.text_mouse_over);
		xml_get_attribute_color(color_tag,"tab_text_dimmed",&iface->color.tab.text_dimmed);
		xml_get_attribute_color(color_tag,"control_label",&iface->color.control.label);
		xml_get_attribute_color(color_tag,"control_text",&iface->color.control.text);
		xml_get_attribute_color(color_tag,"control_fill",&iface->color.control.fill);
		xml_get_attribute_color(color_tag,"control_header",&iface->color.control.header);
		xml_get_attribute_color(color_tag,"control_outline",&iface->color.control.outline);
		xml_get_attribute_color(color_tag,"control_mouse_over",&iface->color.control.mouse_over);
		xml_get_attribute_color(color_tag,"control_hilite",&iface->color.control.hilite);
		xml_get_attribute_color(color_tag,"control_disabled",&iface->color.control.disabled);
		xml_get_attribute_color(color_tag,"button_fill",&iface->color.button.fill);
		xml_get_attribute_color(color_tag,"button_text",&iface->color.button.text);
		xml_get_attribute_color(color_tag,"button_outline",&iface->color.button.outline);
		xml_get_attribute_color(color_tag,"picker_fill",&iface->color.picker.fill);
		xml_get_attribute_color(color_tag,"picker_hilite",&iface->color.picker.hilite);
		xml_get_attribute_color(color_tag,"picker_text",&iface->color.picker.text);
		xml_get_attribute_color(color_tag,"picker_outline",&iface->color.picker.outline);
		xml_get_attribute_color(color_tag,"system_metric",&iface->color.system.metric);
	}

		// fonts

	font_tag=xml_findfirstchild("Font",interface_head_tag);
	if (font_tag!=-1) {
		for (n=0;n!=max_iface_font_variant;n++) {
			if (n==0) {
				strcpy(name,"name");
			}
			else {
				sprintf(name,"name_%d",n);
			}
			xml_get_attribute_text(font_tag,name,iface->font.interface_name[n],name_str_len);

			if (n==0) {
				strcpy(name,"hud_name");
			}
			else {
				sprintf(name,"hud_name_%d",n);
			}
			xml_get_attribute_text(font_tag,name,iface->font.hud_name[n],name_str_len);
		}
	}
	
		// progress

	progress_tag=xml_findfirstchild("Progress",interface_head_tag);
	if (progress_tag!=-1) {
		iface->progress.x=xml_get_attribute_int(progress_tag,"x");
		iface->progress.y=xml_get_attribute_int(progress_tag,"y");
		iface->progress.wid=xml_get_attribute_int(progress_tag,"wid");
		iface->progress.high=xml_get_attribute_int(progress_tag,"high");
		xml_get_attribute_text(progress_tag,"file",iface->progress.bitmap_name,file_str_len);
		iface->progress.animate.image_count=xml_get_attribute_int(progress_tag,"count");
		iface->progress.animate.image_per_row=(int)sqrtf((float)iface->progress.animate.image_count);
		iface->progress.animate.msec=xml_get_attribute_int(progress_tag,"time");
		iface->progress.animate.loop=xml_get_attribute_boolean(progress_tag,"loop");
		iface->progress.animate.loop_back=xml_get_attribute_boolean(progress_tag,"loop_back");
	}
	
		// chat

	chat_tag=xml_findfirstchild("Chat",interface_head_tag);
	if (chat_tag!=-1) {
		iface->chat.x=xml_get_attribute_int(chat_tag,"x");
		iface->chat.y=xml_get_attribute_int(chat_tag,"y");
		iface->chat.last_add_life_sec=xml_get_attribute_int(chat_tag,"last_add_life_sec");
		iface->chat.next_life_sec=xml_get_attribute_int(chat_tag,"next_life_sec");
	}
	
		// fade

	fade_tag=xml_findfirstchild("Fade",interface_head_tag);
	if (fade_tag!=-1) {
		iface->fade.title_msec=xml_get_attribute_int(fade_tag,"title_msec");
		iface->fade.map_msec=xml_get_attribute_int(fade_tag,"map_msec");
	}

		// intro

	intro_head_tag=xml_findfirstchild("Intro",interface_head_tag);
	if (intro_head_tag!=-1) {
	
			// music
			
		xml_get_attribute_text(intro_head_tag,"music",iface->intro.music,name_str_len);
		
			// titles
			
		title_tag=xml_findfirstchild("Title",intro_head_tag);
		if (title_tag!=-1) {
			xml_get_attribute_text(title_tag,"name",iface->logo.name,name_str_len);
			xml_get_attribute_text(title_tag,"sound",iface->logo.sound,name_str_len);
			iface->logo.life_msec=xml_get_attribute_int(title_tag,"life_msec");
		}

			// models

		intro_model_head_tag=xml_findfirstchild("Models",intro_head_tag);
		if (intro_model_head_tag!=-1) {
			intro_model_tag=xml_findfirstchild("Model",intro_model_head_tag);
		
			while (intro_model_tag!=-1) {
				iface_read_settings_intro_model(iface,intro_model_tag);
				intro_model_tag=xml_findnextchild(intro_model_tag);
			}
		}

			// simple save settings

		simple_save_tag=xml_findfirstchild("Simple_Save",intro_head_tag);
		if (simple_save_tag!=-1) {
			iface->intro.simple_save_progress.on=xml_get_attribute_boolean(simple_save_tag,"progress_on");
			iface->intro.simple_save_progress.max_point=xml_get_attribute_int(simple_save_tag,"progress_max_point");
			iface->intro.simple_save_progress.max_bitmap=xml_get_attribute_int(simple_save_tag,"progress_max_bitmap");
			xml_get_attribute_text(simple_save_tag,"progress_bitmap_name",iface->intro.simple_save_progress.bitmap_name,name_str_len);
			xml_get_attribute_text(simple_save_tag,"progress_bitmap_disable_name",iface->intro.simple_save_progress.bitmap_disable_name,name_str_len);
		}

			// score settings

		score_tag=xml_findfirstchild("Score",intro_head_tag);
		if (score_tag!=-1) {
			iface->intro.score.on=xml_get_attribute_boolean(score_tag,"on");
			iface->intro.score.x=xml_get_attribute_int(score_tag,"x");
			iface->intro.score.y=xml_get_attribute_int(score_tag,"y");
			iface->intro.score.wid=xml_get_attribute_int(score_tag,"width");
			iface->intro.score.high=xml_get_attribute_int(score_tag,"height");
			iface->intro.score.text_size=xml_get_attribute_int(score_tag,"text_size");
			iface->intro.score.format=xml_get_attribute_list(score_tag,"format",(char*)score_format_str);
			xml_get_attribute_color(score_tag,"color",&iface->intro.score.col);
		}

			// buttons
			
		button_tag=xml_findfirstchild("Buttons",intro_head_tag);
		if (button_tag!=-1) {
			iface_read_settings_intro_button(xml_findfirstchild("Game_New",button_tag),&iface->intro.button_game_new);
			iface_read_settings_intro_button(xml_findfirstchild("Game_Load",button_tag),&iface->intro.button_game_load);
			iface_read_settings_intro_button(xml_findfirstchild("Game_Setup",button_tag),&iface->intro.button_game_setup);
			iface_read_settings_intro_button(xml_findfirstchild("Multiplayer_Host",button_tag),&iface->intro.button_multiplayer_host);
			iface_read_settings_intro_button(xml_findfirstchild("Multiplayer_Join",button_tag),&iface->intro.button_multiplayer_join);
			iface_read_settings_intro_button(xml_findfirstchild("Credit",button_tag),&iface->intro.button_credit);
			iface_read_settings_intro_button(xml_findfirstchild("Quit",button_tag),&iface->intro.button_quit);
		}
	}
	
		// setup

	setup_tag=xml_findfirstchild("Setup",interface_head_tag);
	if (setup_tag!=-1) {
		iface->setup.game_video=xml_get_attribute_boolean(setup_tag,"game_video");
		iface->setup.game_audio=xml_get_attribute_boolean(setup_tag,"game_audio");
		iface->setup.game_control=xml_get_attribute_boolean(setup_tag,"game_control");
		iface->setup.game_action=xml_get_attribute_boolean(setup_tag,"game_action");
		iface->setup.game_player=xml_get_attribute_boolean(setup_tag,"game_player");
		iface->setup.game_debug=xml_get_attribute_boolean(setup_tag,"game_debug");
		iface->setup.no_resolution_switch=xml_get_attribute_boolean(setup_tag,"no_resolution_switch");
		iface->setup.allow_auto_aim=xml_get_attribute_boolean(setup_tag,"allow_auto_aim");
		iface->setup.allow_run=xml_get_attribute_boolean(setup_tag,"allow_run");
	}
	
		// sound
		
	sound_tag=xml_findfirstchild("Sound",interface_head_tag);
	if (sound_tag!=-1) {
		xml_get_attribute_text(sound_tag,"click",iface->click_sound,name_str_len);
	}
	
		// preload models
		
	preload_tag=xml_findfirstchild("Preload",interface_head_tag);
	if (preload_tag!=-1) {
	
		preload_models_tag=xml_findfirstchild("Models",preload_tag);
		if (preload_models_tag!=-1) {
		
			preload_model_tag=xml_findfirstchild("Model",preload_models_tag);
			
			n=0;
			
			while (preload_model_tag!=-1) {
				xml_get_attribute_text(preload_model_tag,"name",iface->preload_model.names[n++],name_str_len);
				preload_model_tag=xml_findnextchild(preload_model_tag);
			}
		}
	}

		// project setup
		
	proj_tag=xml_findfirstchild("Project",interface_head_tag);
	if (proj_tag!=-1) {
		xml_get_attribute_text(proj_tag,"name",iface->project.name,name_str_len);
		iface->project.simple_save=xml_get_attribute_boolean(proj_tag,"simple_save");
		iface->project.checkpoints=xml_get_attribute_boolean(proj_tag,"checkpoints");
		iface->project.load_requires_click=xml_get_attribute_boolean(proj_tag,"load_requires_click");
	}

	xml_close_file();
}

void iface_refresh_settings_interface_hud_only(iface_type *iface)
{
	int							interface_head_tag,
								bitmap_head_tag,bitmap_tag,text_head_tag,text_tag,bar_head_tag,bar_tag;
	char						path[1024],i_name[256];

		// read in interface from setting files
	
	iface_xml_substitution("Interface",i_name);

	file_paths_data(&file_path_setup,path,"Settings",i_name,"xml");
	if (!xml_open_file(path)) return;
	
		// decode the file
      
    interface_head_tag=xml_findrootchild("Interface");
    if (interface_head_tag==-1) {
		xml_close_file();
		return;
	}

		// reload bitmaps, text, and bars
		
	iface->bitmap_list.nbitmap=0;
	iface->text_list.ntext=0;
	iface->bar_list.nbar=0;
	
		// bitmaps
	
    bitmap_head_tag=xml_findfirstchild("Bitmaps",interface_head_tag);
    if (bitmap_head_tag!=-1) {
	
		bitmap_tag=xml_findfirstchild("Bitmap",bitmap_head_tag);
		
		while (bitmap_tag!=-1) {
			iface_read_settings_bitmap(iface,bitmap_tag);
			bitmap_tag=xml_findnextchild(bitmap_tag);
		}
	}
	
		// text
	
	text_head_tag=xml_findfirstchild("Texts",interface_head_tag);
    if (text_head_tag!=-1) {
	
		text_tag=xml_findfirstchild("Text",text_head_tag);
		
		while (text_tag!=-1) {
			iface_read_settings_text(iface,text_tag);
			text_tag=xml_findnextchild(text_tag);
		}
	}
	
		// bars
	
    bar_head_tag=xml_findfirstchild("Bars",interface_head_tag);
    if (bar_head_tag!=-1) {
	
		bar_tag=xml_findfirstchild("Bar",bar_head_tag);
		
		while (bar_tag!=-1) {
			iface_read_settings_bar(iface,bar_tag);
			bar_tag=xml_findnextchild(bar_tag);
		}
	}
	xml_close_file();
}

/* =======================================================

      Write Interface Write Utilities
      
======================================================= */

void iface_write_settings_interface_intro_button(char *name,iface_intro_button_type *btn)
{
	xml_add_tagstart(name);
	
	xml_add_attribute_int("x",btn->x);
	xml_add_attribute_int("y",btn->y);
	if (btn->mobile_x!=-1) xml_add_attribute_int("mobile_x",btn->mobile_x);
	if (btn->mobile_y!=-1) xml_add_attribute_int("mobile_y",btn->mobile_y);
	if (btn->wid!=-1) xml_add_attribute_int("width",btn->wid);
	if (btn->high!=-1) xml_add_attribute_int("height",btn->high);
	xml_add_attribute_boolean("on",btn->on);
	xml_add_attribute_boolean("mobile_hide",btn->mobile_hide);

	xml_add_tagend(TRUE);
}

void iface_write_settings_interface_device(char *name,iface_device_type *device)
{
	xml_add_tagstart("Device");
	xml_add_attribute_text("name",name);
	xml_add_tagend(FALSE);

		// scale

	xml_add_tagstart("Scale");
	xml_add_attribute_float("control_padding",device->scale.control_padding);
	xml_add_attribute_float("control_height",device->scale.control_high);
	xml_add_attribute_float("control_short_width",device->scale.control_short_wid);
	xml_add_attribute_float("control_long_width",device->scale.control_long_wid);
	xml_add_attribute_float("control_scroll_size",device->scale.control_scroll_size);
	xml_add_attribute_float("button_height",device->scale.button_high);
	xml_add_attribute_float("button_short_width",device->scale.button_short_wid);
	xml_add_attribute_float("button_long_width",device->scale.button_long_wid);
	xml_add_attribute_float("dialog_margin",device->scale.dialog_margin);
	xml_add_attribute_float("tab_height",device->scale.tab_high);
	xml_add_tagend(TRUE);
	
	xml_add_tagstart("Render");
	xml_add_attribute_float("fov_adjust",device->render.fov_adjust);
	xml_add_tagend(TRUE);

	xml_add_tagclose("Device");
}

/* =======================================================

      Write Interface XML
      
======================================================= */

bool iface_write_settings_interface(iface_type *iface,char *err_str)
{
	int							n,k;
	char						path[1024],name[256];
	bool						ok;
	iface_bitmap_type			*bitmap;
	iface_text_type				*text;
	iface_bar_type				*bar;
	iface_virtual_stick_type	*stick;
	iface_virtual_button_type	*button;
	iface_radar_icon_type		*radar_icon;
	iface_menu_type				*menu;
	iface_menu_item_type		*menu_item;
	iface_intro_model_type		*model;
	
		// start new file
		
	xml_new_file();
    
    xml_add_tagstart("Interface");
    xml_add_tagend(FALSE);

		// scale
		
	xml_add_tagstart("Scale");
	xml_add_attribute_int("x",iface->scale_x);
	xml_add_attribute_int("y",iface->scale_y);
	xml_add_tagend(TRUE);

		// devices

	xml_add_tagstart("Devices");
	xml_add_tagend(FALSE);
	iface_write_settings_interface_device("pc",&iface->devices[device_type_pc]);
	iface_write_settings_interface_device("phone",&iface->devices[device_type_phone]);
	iface_write_settings_interface_device("pad",&iface->devices[device_type_pad]);
	xml_add_tagclose("Devices");
	
		// bitmaps
		
	xml_add_tagstart("Bitmaps");
	xml_add_tagend(FALSE);
	
	for (n=0;n!=iface->bitmap_list.nbitmap;n++) {
		bitmap=&iface->bitmap_list.bitmaps[n];
		
		xml_add_tagstart("Bitmap");
		xml_add_attribute_text("name",bitmap->name);
		xml_add_tagend(FALSE);
		
		xml_add_tagstart("Image");
		xml_add_attribute_text("file",bitmap->filename);
		xml_add_attribute_int("count",bitmap->animate.image_count);
		xml_add_attribute_int("time",bitmap->animate.msec);
		xml_add_attribute_boolean("loop",bitmap->animate.loop);
		xml_add_attribute_boolean("loop_back",bitmap->animate.loop_back);
		xml_add_attribute_float("rot",bitmap->rot);
		xml_add_tagend(TRUE);
	
		xml_add_tagstart("Position");
		xml_add_attribute_int("x",bitmap->pnt.x);
		xml_add_attribute_int("y",bitmap->pnt.y);
		xml_add_tagend(TRUE);
		
		xml_add_tagstart("Size");
		xml_add_attribute_int("width",bitmap->size.x);
		xml_add_attribute_int("height",bitmap->size.y);
		xml_add_tagend(TRUE);

		xml_add_tagstart("Settings");
		xml_add_attribute_float("alpha",bitmap->alpha);
		xml_add_attribute_boolean("flip_horizontal",bitmap->flip_horz);
		xml_add_attribute_boolean("flip_vertical",bitmap->flip_vert);
		xml_add_attribute_boolean("team_tint",bitmap->team_tint);
		xml_add_attribute_boolean("hide",!bitmap->show);
		xml_add_attribute_boolean("flash",bitmap->flash);
		xml_add_attribute_boolean("checkpoint",bitmap->checkpoint);
		xml_add_tagend(TRUE);
		
		xml_add_tagstart("Repeat");
		xml_add_attribute_boolean("on",bitmap->repeat.on);
		xml_add_attribute_int("count",bitmap->repeat.count);
		xml_add_attribute_int("x",bitmap->repeat.x_add);
		xml_add_attribute_int("y",bitmap->repeat.y_add);
		xml_add_attribute_int("col",bitmap->repeat.col);
		xml_add_attribute_int("row",bitmap->repeat.row);
		xml_add_tagend(TRUE);
	
		xml_add_tagstart("Fade");
		xml_add_attribute_boolean("on",bitmap->fade.on);
		xml_add_attribute_int("fade_in_msec",bitmap->fade.fade_in_tick);
		xml_add_attribute_int("life_msec",bitmap->fade.life_tick);
		xml_add_attribute_int("fade_out_msec",bitmap->fade.fade_out_tick);
		xml_add_tagend(TRUE);

		xml_add_tagclose("Bitmap");
	}
	
	xml_add_tagclose("Bitmaps");
	
		// text
		
	xml_add_tagstart("Texts");
	xml_add_tagend(FALSE);
	
	for (n=0;n!=iface->text_list.ntext;n++) {
		text=&iface->text_list.texts[n];
		
		xml_add_tagstart("Text");
		xml_add_attribute_text("name",text->name);
		xml_add_tagend(FALSE);
	
		xml_add_tagstart("Position");
		xml_add_attribute_int("x",text->pnt.x);
		xml_add_attribute_int("y",text->pnt.y);
		xml_add_tagend(TRUE);

		xml_add_tagstart("Settings");
		xml_add_attribute_float("alpha",text->alpha);
		xml_add_attribute_text("data",text->data);
		xml_add_attribute_int("size",text->size);
		xml_add_attribute_color("color",&text->color);
		xml_add_attribute_list("just",(char*)just_mode_str,text->just);
		xml_add_attribute_boolean("hide",!text->show);
		xml_add_attribute_boolean("monospaced",text->monospaced);
		xml_add_attribute_boolean("checkpoint",text->checkpoint);
		xml_add_attribute_list("special",(char*)text_special_str,text->special);
		xml_add_tagend(TRUE);

		xml_add_tagstart("Fade");
		xml_add_attribute_boolean("on",text->fade.on);
		xml_add_attribute_int("fade_in_msec",text->fade.fade_in_tick);
		xml_add_attribute_int("life_msec",text->fade.life_tick);
		xml_add_attribute_int("fade_out_msec",text->fade.fade_out_tick);
		xml_add_tagend(TRUE);

		xml_add_tagclose("Text");
	}
	
	xml_add_tagclose("Texts");

		// bars
	
	xml_add_tagstart("Bars");
	xml_add_tagend(FALSE);
	
	for (n=0;n!=iface->bar_list.nbar;n++) {
		bar=&iface->bar_list.bars[n];
		
		xml_add_tagstart("Bar");
		xml_add_attribute_text("name",bar->name);
		xml_add_attribute_list("type",(char*)bar_type_str,bar->type);
		xml_add_tagend(FALSE);
		
		xml_add_tagstart("Position");
		xml_add_attribute_int("x",bar->pnt.x);
		xml_add_attribute_int("y",bar->pnt.y);
		xml_add_tagend(TRUE);
		
		xml_add_tagstart("Size");
		xml_add_attribute_int("width",bar->size.x);
		xml_add_attribute_int("height",bar->size.y);
		xml_add_tagend(TRUE);
		
		xml_add_tagstart("Outline");
		xml_add_attribute_boolean("on",bar->outline);
		xml_add_attribute_float("alpha",bar->outline_alpha);
		xml_add_attribute_color("color",&bar->outline_color);
		xml_add_tagend(TRUE);

		xml_add_tagstart("Background");
		xml_add_attribute_boolean("on",bar->background);
		xml_add_attribute_float("alpha",bar->background_alpha);
		xml_add_attribute_color("color",&bar->background_color);
		xml_add_tagend(TRUE);
		
		xml_add_tagstart("Fill");
		xml_add_attribute_float("alpha",bar->fill_alpha);
		xml_add_attribute_color("start_color",&bar->fill_start_color);
		xml_add_attribute_color("end_color",&bar->fill_end_color);
		xml_add_tagend(TRUE);
		
		xml_add_tagstart("Settings");
		xml_add_attribute_boolean("hide",!bar->show);
		xml_add_tagend(TRUE);
		
		xml_add_tagclose("Bar");
	}
	
	xml_add_tagclose("Bars");
	
		// virtual controls
		
	xml_add_tagstart("Virtual_Control");
	xml_add_tagend(FALSE);
		
		// virtual control sticks

	xml_add_tagstart("Sticks");
	xml_add_tagend(FALSE);
	
	stick=iface->virtual_control.sticks;

	for (n=0;n!=max_virtual_stick;n++) {
		xml_add_tagstart("Stick");
		xml_add_attribute_boolean("on",stick->on);
		xml_add_attribute_int("click_control",stick->click_control_idx);
		xml_add_attribute_boolean("use_bitmap",stick->use_bitmap);
		xml_add_attribute_int("x",stick->pnt.x);
		xml_add_attribute_int("y",stick->pnt.y);
		xml_add_attribute_int("x_size",stick->size.x);
		xml_add_attribute_int("y_size",stick->size.y);
		xml_add_attribute_boolean("x_flip",stick->flip_x);
		xml_add_attribute_boolean("y_flip",stick->flip_y);
		xml_add_attribute_text("outer_bitmap",stick->outer_bitmap_name);
		xml_add_attribute_text("inner_bitmap",stick->inner_bitmap_name);
		xml_add_attribute_color("color",&stick->color);
		xml_add_tagend(TRUE);

		stick++;
	}

	xml_add_tagclose("Sticks");
	
		// virtual control buttons
		
	xml_add_tagstart("Buttons");
	xml_add_tagend(FALSE);
	
	button=iface->virtual_control.buttons;

	for (n=0;n!=max_virtual_button;n++) {
		xml_add_tagstart("Button");
		xml_add_attribute_boolean("on",button->on);
		xml_add_attribute_boolean("sticky",button->sticky);
		xml_add_attribute_boolean("use_bitmap",button->use_bitmap);
		xml_add_attribute_int("control",button->control_idx);
		xml_add_attribute_int("x",button->pnt.x);
		xml_add_attribute_int("y",button->pnt.y);
		xml_add_attribute_int("x_size",button->size.x);
		xml_add_attribute_int("y_size",button->size.y);
		xml_add_attribute_text("up_bitmap",button->up_bitmap_name);
		xml_add_attribute_text("down_bitmap",button->down_bitmap_name);
		xml_add_attribute_color("color",&button->color);
		xml_add_tagend(TRUE);

		button++;
	}
	
	xml_add_tagclose("Buttons");

	xml_add_tagclose("Virtual_Control");

		// radar

	xml_add_tagstart("Radar");
	xml_add_tagend(FALSE);

	xml_add_tagstart("Setting");
	xml_add_attribute_boolean("on",iface->radar.on);
	xml_add_attribute_boolean("no_rot",!iface->radar.rot);
	xml_add_attribute_int("no_motion_fade",iface->radar.no_motion_fade);
	xml_add_tagend(TRUE);

	xml_add_tagstart("Position");
	xml_add_attribute_int("x",iface->radar.pnt.x);
	xml_add_attribute_int("y",iface->radar.pnt.y);
	xml_add_attribute_int("radius",iface->radar.display_radius);
	xml_add_tagend(TRUE);

	xml_add_tagstart("View");
	xml_add_attribute_int("radius",iface->radar.view_radius);
	xml_add_tagend(TRUE);

	xml_add_tagstart("Background");
	xml_add_attribute_text("file",iface->radar.background_bitmap_name);
	xml_add_tagend(TRUE);

		// radar icons

	xml_add_tagstart("Icons");
	xml_add_tagend(FALSE);

	radar_icon=iface->radar.icons;
	
	for (n=0;n!=iface->radar.nicon;n++) {
		xml_add_tagstart("Icon");
		xml_add_attribute_text("name",radar_icon->name);
		xml_add_attribute_text("file",radar_icon->bitmap_name);
		xml_add_attribute_int("size",radar_icon->size);
		xml_add_attribute_boolean("rot",radar_icon->rot);
		xml_add_tagend(TRUE);
		
		radar_icon++;
	}

	xml_add_tagclose("Icons");

	xml_add_tagclose("Radar");

		// menus

	xml_add_tagstart("Menus");
	xml_add_tagend(FALSE);

	menu=iface->menu_list.menus;
	
	for (n=0;n!=iface->menu_list.nmenu;n++) {

		xml_add_tagstart("Menu");
		xml_add_attribute_text("name",menu->name);
		xml_add_tagend(FALSE);

			// menu items

		xml_add_tagstart("Items");
		xml_add_tagend(FALSE);

		menu_item=menu->items;

		for (k=0;k!=menu->nitem;k++) {
			xml_add_tagstart("Item");
			xml_add_attribute_int("id",menu_item->item_id);
			xml_add_attribute_text("data",menu_item->data);
			xml_add_attribute_text("sub_menu",menu_item->sub_menu);
			xml_add_attribute_boolean("multiplayer_disable",menu_item->multiplayer_disable);
			xml_add_attribute_boolean("quit",menu_item->quit);
			xml_add_tagend(TRUE);

			menu_item++;
		}

		xml_add_tagclose("Items");
		xml_add_tagclose("Menu");
		
		menu++;
	}

	xml_add_tagclose("Menus");
	
		// colors
		
	xml_add_tagstart("Color");
	xml_add_attribute_color("background",&iface->color.background);
	xml_add_attribute_color("menu_text",&iface->color.menu.text);
	xml_add_attribute_color("menu_mouse_over",&iface->color.menu.mouse_over);
	xml_add_attribute_color("menu_background",&iface->color.menu.background);
	xml_add_attribute_color("menu_outline",&iface->color.menu.outline);
	xml_add_attribute_color("dialog_background",&iface->color.dialog.background);
	xml_add_attribute_color("dialog_header",&iface->color.dialog.header);
	xml_add_attribute_color("dialog_outline",&iface->color.dialog.outline);
	xml_add_attribute_color("dialog_title",&iface->color.dialog.title);
	xml_add_attribute_color("tab_background",&iface->color.tab.background);
	xml_add_attribute_color("tab_dimmed",&iface->color.tab.dimmed);
	xml_add_attribute_color("tab_outline",&iface->color.tab.outline);
	xml_add_attribute_color("tab_text",&iface->color.tab.text);
	xml_add_attribute_color("tab_text_mouse_over",&iface->color.tab.text_mouse_over);
	xml_add_attribute_color("tab_text_dimmed",&iface->color.tab.text_dimmed);
	xml_add_attribute_color("control_label",&iface->color.control.label);
	xml_add_attribute_color("control_text",&iface->color.control.text);
	xml_add_attribute_color("control_fill",&iface->color.control.fill);
	xml_add_attribute_color("control_header",&iface->color.control.header);
	xml_add_attribute_color("control_outline",&iface->color.control.outline);
	xml_add_attribute_color("control_mouse_over",&iface->color.control.mouse_over);
	xml_add_attribute_color("control_hilite",&iface->color.control.hilite);
	xml_add_attribute_color("control_disabled",&iface->color.control.disabled);
	xml_add_attribute_color("button_fill",&iface->color.button.fill);
	xml_add_attribute_color("button_text",&iface->color.button.text);
	xml_add_attribute_color("button_outline",&iface->color.button.outline);
	xml_add_attribute_color("picker_fill",&iface->color.picker.fill);
	xml_add_attribute_color("picker_hilite",&iface->color.picker.hilite);
	xml_add_attribute_color("picker_text",&iface->color.picker.text);
	xml_add_attribute_color("picker_outline",&iface->color.picker.outline);
	xml_add_attribute_color("system_metric",&iface->color.system.metric);
	xml_add_tagend(TRUE);

		// fonts

	xml_add_tagstart("Font");
	
	for (n=0;n!=max_iface_font_variant;n++) {
		if (n==0) {
			strcpy(name,"name");
		}
		else {
			sprintf(name,"name_%d",n);
		}
		xml_add_attribute_text(name,iface->font.interface_name[n]);
		if (n==0) {
			strcpy(name,"hud_name");
		}
		else {
			sprintf(name,"hud_name_%d",n);
		}
		xml_add_attribute_text(name,iface->font.hud_name[n]);
	}

	xml_add_tagend(TRUE);
	
		// progress

	xml_add_tagstart("Progress");
	xml_add_attribute_int("x",iface->progress.x);
	xml_add_attribute_int("y",iface->progress.y);
	xml_add_attribute_int("wid",iface->progress.wid);
	xml_add_attribute_int("high",iface->progress.high);
	xml_add_attribute_text("file",iface->progress.bitmap_name);
	xml_add_attribute_int("count",iface->progress.animate.image_count);
	xml_add_attribute_int("time",iface->progress.animate.msec);
	xml_add_attribute_boolean("loop",iface->progress.animate.loop);
	xml_add_attribute_boolean("loop_back",iface->progress.animate.loop_back);
	xml_add_tagend(TRUE);

		// chat

	xml_add_tagstart("Chat");
	xml_add_attribute_int("x",iface->chat.x);
	xml_add_attribute_int("y",iface->chat.y);
	xml_add_attribute_int("last_add_life_sec",iface->chat.last_add_life_sec);
	xml_add_attribute_int("next_life_sec",iface->chat.next_life_sec);
	xml_add_tagend(TRUE);
	
		// fade

	xml_add_tagstart("Fade");
	xml_add_attribute_int("title_msec",iface->fade.title_msec);
	xml_add_attribute_int("map_msec",iface->fade.map_msec);
	xml_add_tagend(TRUE);

		// intro

	xml_add_tagstart("Intro");
	xml_add_attribute_text("music",iface->intro.music);
	xml_add_tagend(FALSE);
	
		// titles

	xml_add_tagstart("Title");
	xml_add_attribute_text("name",iface->logo.name);
	xml_add_attribute_text("sound",iface->logo.sound);
	xml_add_attribute_int("life_msec",iface->logo.life_msec);
	xml_add_tagend(TRUE);

		// intro models

	xml_add_tagstart("Models");
	xml_add_tagend(FALSE);

	model=iface->intro.model_list.models;
	
	for (n=0;n!=iface->intro.model_list.nmodel;n++) {
		xml_add_tagstart("Model");
		xml_add_attribute_text("model",model->model_name);
		xml_add_attribute_text("animate",model->animate_name);
		xml_add_attribute_int("x",model->x);
		xml_add_attribute_int("y",model->y);
		xml_add_attribute_3_coord_float("rot",model->rot.x,model->rot.y,model->rot.z);
		xml_add_attribute_float("resize",model->resize);
		xml_add_tagend(TRUE);

		model++;
	}

	xml_add_tagclose("Models");

		// simple save

	xml_add_tagstart("Simple_Save");
	xml_add_attribute_boolean("progress_on",iface->intro.simple_save_progress.on);
	xml_add_attribute_int("progress_max_point",iface->intro.simple_save_progress.max_point);
	xml_add_attribute_int("progress_max_bitmap",iface->intro.simple_save_progress.max_bitmap);
	xml_add_attribute_text("progress_bitmap_name",iface->intro.simple_save_progress.bitmap_name);
	xml_add_attribute_text("progress_bitmap_disable_name",iface->intro.simple_save_progress.bitmap_disable_name);
	xml_add_tagend(TRUE);

		// score

	xml_add_tagstart("Score");
	xml_add_attribute_boolean("on",iface->intro.score.on);
	xml_add_attribute_int("x",iface->intro.score.x);
	xml_add_attribute_int("y",iface->intro.score.y);
	xml_add_attribute_int("width",iface->intro.score.wid);
	xml_add_attribute_int("height",iface->intro.score.high);
	xml_add_attribute_int("text_size",iface->intro.score.text_size);
	xml_add_attribute_list("format",(char*)score_format_str,iface->intro.score.format);
	xml_add_attribute_color("color",&iface->intro.score.col);
	xml_add_tagend(TRUE);

		// intro buttons

	xml_add_tagstart("Buttons");
	xml_add_tagend(FALSE);

	iface_write_settings_interface_intro_button("Game_New",&iface->intro.button_game_new);
	iface_write_settings_interface_intro_button("Game_Load",&iface->intro.button_game_load);
	iface_write_settings_interface_intro_button("Game_Setup",&iface->intro.button_game_setup);
	iface_write_settings_interface_intro_button("Multiplayer_Host",&iface->intro.button_multiplayer_host);
	iface_write_settings_interface_intro_button("Multiplayer_Join",&iface->intro.button_multiplayer_join);
	iface_write_settings_interface_intro_button("Credit",&iface->intro.button_credit);
	iface_write_settings_interface_intro_button("Quit",&iface->intro.button_quit);

	xml_add_tagclose("Buttons");

	xml_add_tagclose("Intro");

		// setup

	xml_add_tagstart("Setup");
	xml_add_attribute_boolean("game_video",iface->setup.game_video);
	xml_add_attribute_boolean("game_audio",iface->setup.game_audio);
	xml_add_attribute_boolean("game_control",iface->setup.game_control);
	xml_add_attribute_boolean("game_action",iface->setup.game_action);
	xml_add_attribute_boolean("game_player",iface->setup.game_player);
	xml_add_attribute_boolean("game_debug",iface->setup.game_debug);
	xml_add_attribute_boolean("no_resolution_switch",iface->setup.no_resolution_switch);
	xml_add_attribute_boolean("allow_auto_aim",iface->setup.allow_auto_aim);
	xml_add_attribute_boolean("allow_run",iface->setup.allow_run);
	xml_add_tagend(TRUE);

		// sound
		
	xml_add_tagstart("Sound");
	xml_add_attribute_text("click",iface->click_sound);
	xml_add_tagend(TRUE);
	
		// preload models
		
	xml_add_tagstart("Preload");
	xml_add_tagend(FALSE);
	
	xml_add_tagstart("Models");
	xml_add_tagend(FALSE);
	
	for (n=0;n!=max_preload_model;n++) {
		xml_add_tagstart("Model");
		xml_add_attribute_text("name",iface->preload_model.names[n]);
		xml_add_tagend(TRUE);
	}
	
	xml_add_tagclose("Models");
	
	xml_add_tagclose("Preload");

		// project setup
		
	xml_add_tagstart("Project");
	xml_add_attribute_text("name",iface->project.name);
	xml_add_attribute_boolean("simple_save",iface->project.simple_save);
	xml_add_attribute_boolean("checkpoints",iface->project.checkpoints);
	xml_add_attribute_boolean("load_requires_click",iface->project.load_requires_click);
	xml_add_tagend(TRUE);

		// close interface

    xml_add_tagclose("Interface");

        // save the map
		
	file_paths_data(&file_path_setup,path,"Settings","Interface","xml");
		
	ok=xml_save_file(path,err_str);
    xml_close_file();
	
	return(ok);
}
