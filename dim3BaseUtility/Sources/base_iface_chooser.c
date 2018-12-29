/****************************** File *********************************

Module: dim3 Base Utility
Author: Brian Barnes
 Usage: Chooser XML

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

extern char						just_mode_str[][32];

char							chooser_key_str[][32]=chooser_key_list_def;

/* =======================================================

      Read Chooser XML
      
======================================================= */

iface_chooser_piece_type* read_setting_interface_chooser_common(iface_chooser_type *chooser,int piece_type,int item_tag)
{
	iface_chooser_piece_type	*piece;

		// create new piece
		
	if (chooser->npiece>=max_chooser_piece) return(NULL);

	piece=&chooser->pieces[chooser->npiece];
	chooser->npiece++;

		// common piece settings
		
	piece->type=piece_type;

	piece->id=xml_get_attribute_int(item_tag,"id");
	piece->x=xml_get_attribute_int(item_tag,"x");
	piece->y=xml_get_attribute_int(item_tag,"y");
	piece->wid=xml_get_attribute_int_default(item_tag,"width",-1);
	piece->high=xml_get_attribute_int_default(item_tag,"height",-1);
	piece->clickable=xml_get_attribute_boolean(item_tag,"clickable");
	piece->no_close=xml_get_attribute_boolean(item_tag,"no_close");
	piece->key=xml_get_attribute_list(item_tag,"key",(char*)chooser_key_str);
	xml_get_attribute_text(item_tag,"goto",piece->goto_name,name_str_len);
	
	return(piece);
}

void iface_read_settings_chooser_items(iface_chooser_type *chooser,int item_tag)
{
	iface_chooser_piece_type	*piece;
	
	while (item_tag!=-1) {
		piece=read_setting_interface_chooser_common(chooser,chooser_piece_type_item,item_tag);
		if (piece==NULL) return;
		
		xml_get_attribute_text(item_tag,"file",piece->data.item.file,file_str_len);
		
		item_tag=xml_findnextchild(item_tag);
	}
}

void iface_read_settings_chooser_models(iface_chooser_type *chooser,int item_tag)
{
	iface_chooser_piece_type	*piece;
	
	while (item_tag!=-1) {
		piece=read_setting_interface_chooser_common(chooser,chooser_piece_type_model,item_tag);
		if (piece==NULL) return;
		
		xml_get_attribute_text(item_tag,"model",piece->data.model.model_name,name_str_len);
		xml_get_attribute_text(item_tag,"animate",piece->data.model.animate_name,name_str_len);
		xml_get_attribute_3_coord_float(item_tag,"rot",&piece->data.model.rot.x,&piece->data.model.rot.y,&piece->data.model.rot.z);
		piece->data.model.resize=xml_get_attribute_float_default(item_tag,"resize",1.0f);
		
		item_tag=xml_findnextchild(item_tag);
	}
}

void iface_read_settings_chooser_texts(iface_type *iface,iface_chooser_type *chooser,int item_tag)
{
	iface_chooser_piece_type	*piece;

	while (item_tag!=-1) {
		piece=read_setting_interface_chooser_common(chooser,chooser_piece_type_text,item_tag);
		if (piece==NULL) return;

		xml_get_attribute_text(item_tag,"data",piece->data.text.str,max_chooser_text_data_sz);
		piece->data.text.size=xml_get_attribute_int_default(item_tag,"size",iface->font.text_size_small);
		piece->data.text.just=xml_get_attribute_list(item_tag,"just",(char*)just_mode_str);

		item_tag=xml_findnextchild(item_tag);
	}
}

void iface_read_settings_chooser_buttons(iface_chooser_type *chooser,int item_tag)
{
	iface_chooser_piece_type	*piece;
	
	while (item_tag!=-1) {
		piece=read_setting_interface_chooser_common(chooser,chooser_piece_type_button,item_tag);
		if (piece==NULL) return;

		xml_get_attribute_text(item_tag,"name",piece->data.button.name,max_chooser_button_text_sz);
		
		item_tag=xml_findnextchild(item_tag);
	}
}

void iface_read_settings_chooser_single(iface_type *iface,int chooser_tag)
{
	int					tag,texts_head_tag,buttons_head_tag,
						items_head_tag,models_head_tag,item_tag;
	iface_chooser_type	*chooser;

		// find next chooser

	if (iface->chooser_list.nchooser>=max_iface_chooser) return;
	
	chooser=&iface->chooser_list.choosers[iface->chooser_list.nchooser];
	iface->chooser_list.nchooser++;
	
	xml_get_attribute_text(chooser_tag,"name",chooser->name,name_str_len);
	xml_get_attribute_text(chooser_tag,"template",chooser->template_name,name_str_len);

	chooser->npiece=0;
	chooser->frame.on=FALSE;
	
		// frames and keys
		
	tag=xml_findfirstchild("Frame",chooser_tag);
	if (tag!=-1) {
		chooser->frame.on=xml_get_attribute_boolean(tag,"on");
		xml_get_attribute_text(tag,"title",chooser->frame.title,max_chooser_frame_text_sz);
		chooser->frame.x=xml_get_attribute_int(tag,"x");
		chooser->frame.y=xml_get_attribute_int(tag,"y");
		chooser->frame.wid=xml_get_attribute_int(tag,"width");
		chooser->frame.high=xml_get_attribute_int(tag,"height");
	}
	
		// items
	
	items_head_tag=xml_findfirstchild("Items",chooser_tag);
	if (items_head_tag!=-1) {
		item_tag=xml_findfirstchild("Item",items_head_tag);
		iface_read_settings_chooser_items(chooser,item_tag);
	}
	
		// models
	
	models_head_tag=xml_findfirstchild("Models",chooser_tag);
	if (models_head_tag!=-1) {
		item_tag=xml_findfirstchild("Model",models_head_tag);
		iface_read_settings_chooser_models(chooser,item_tag);
	}

		// text

	texts_head_tag=xml_findfirstchild("Texts",chooser_tag);
	if (texts_head_tag!=-1) {
		item_tag=xml_findfirstchild("Text",texts_head_tag);
		iface_read_settings_chooser_texts(iface,chooser,item_tag);
	}
	
		// buttons
	
	buttons_head_tag=xml_findfirstchild("Buttons",chooser_tag);
	if (buttons_head_tag!=-1) {
		item_tag=xml_findfirstchild("Button",buttons_head_tag);
		iface_read_settings_chooser_buttons(chooser,item_tag);
	}
}

void iface_read_settings_chooser(iface_type *iface)
{
	int					n,nchooser,
						choosers_head_tag,chooser_tag;
	char				path[1024],c_name[256];

	iface_xml_substitution("Choosers",c_name);

	file_paths_data(&file_path_setup,path,"Settings",c_name,"xml");
	if (!xml_open_file(path)) return;

		// chooser counts
	
	choosers_head_tag=xml_findrootchild("Choosers");
    if (choosers_head_tag==-1) {
		xml_close_file();
		return;
	}

	nchooser=xml_countchildren(choosers_head_tag);
	if (nchooser==0) {
		xml_close_file();
		return;
	}

		// read the choosers

	chooser_tag=xml_findfirstchild("Chooser",choosers_head_tag);

	for (n=0;n!=nchooser;n++) {
		iface_read_settings_chooser_single(iface,chooser_tag);
		chooser_tag=xml_findnextchild(chooser_tag);
	}

	xml_close_file();
}

/* =======================================================

      Write Chooser XML
      
======================================================= */

void iface_write_settings_chooser_piece_type(iface_chooser_type *chooser,int piece_type,char *major_tag,char *minor_tag)
{
	int							n;
	iface_chooser_piece_type	*chooser_piece;

	xml_add_tagstart(major_tag);
	xml_add_tagend(FALSE);

	for (n=0;n!=chooser->npiece;n++) {
		chooser_piece=&chooser->pieces[n];
		if (chooser_piece->type!=piece_type) continue;

		xml_add_tagstart(minor_tag);

			// common settings

		xml_add_attribute_int("id",chooser_piece->id);
		xml_add_attribute_int("x",chooser_piece->x);
		xml_add_attribute_int("y",chooser_piece->y);
		if (chooser_piece->wid!=-1) xml_add_attribute_int("width",chooser_piece->wid);
		if (chooser_piece->high!=-1) xml_add_attribute_int("height",chooser_piece->high);
		xml_add_attribute_boolean("clickable",chooser_piece->clickable);
		xml_add_attribute_boolean("no_close",chooser_piece->no_close);
		xml_add_attribute_list("key",(char*)chooser_key_str,chooser_piece->key);
		xml_add_attribute_text("goto",chooser_piece->goto_name);

			// specific settings

		switch (chooser_piece->type) {
			case chooser_piece_type_item:
				xml_add_attribute_text("file",chooser_piece->data.item.file);
				break;
			case chooser_piece_type_model:
				xml_add_attribute_text("model",chooser_piece->data.model.model_name);
				xml_add_attribute_text("animate",chooser_piece->data.model.animate_name);
				xml_add_attribute_3_coord_float("rot",chooser_piece->data.model.rot.x,chooser_piece->data.model.rot.y,chooser_piece->data.model.rot.z);
				xml_add_attribute_float("resize",chooser_piece->data.model.resize);
				break;
			case chooser_piece_type_text:
				xml_add_attribute_text("data",chooser_piece->data.text.str);
				xml_add_attribute_int("size",chooser_piece->data.text.size);
				xml_add_attribute_list("just",(char*)just_mode_str,chooser_piece->data.text.just);
				break;
			case chooser_piece_type_button:
				xml_add_attribute_text("name",chooser_piece->data.button.name);
				break;
		}
		
		xml_add_tagend(TRUE);
	}

	xml_add_tagclose(major_tag);
}

bool iface_write_settings_chooser(iface_type *iface,char *err_str)
{
	int							n;
	char						path[1024];
	bool						ok;
	iface_chooser_type			*chooser;
	
		// start new file
		
	xml_new_file();

		// choosers

	xml_add_tagstart("Choosers");
	xml_add_tagend(FALSE);

	chooser=iface->chooser_list.choosers;
	
	for (n=0;n!=iface->chooser_list.nchooser;n++) {

		xml_add_tagstart("Chooser");
		xml_add_attribute_text("name",chooser->name);
		xml_add_attribute_text("template",chooser->template_name);
		xml_add_tagend(FALSE);

			// frame

		xml_add_tagstart("Frame");
		xml_add_attribute_boolean("on",chooser->frame.on);
		xml_add_attribute_text("title",chooser->frame.title);
		xml_add_attribute_int("x",chooser->frame.x);
		xml_add_attribute_int("y",chooser->frame.y);
		xml_add_attribute_int("width",chooser->frame.wid);
		xml_add_attribute_int("height",chooser->frame.high);
		xml_add_tagend(TRUE);

			// chooser pieces

		iface_write_settings_chooser_piece_type(chooser,chooser_piece_type_item,"Items","Item");
		iface_write_settings_chooser_piece_type(chooser,chooser_piece_type_model,"Models","Model");
		iface_write_settings_chooser_piece_type(chooser,chooser_piece_type_text,"Texts","Text");
		iface_write_settings_chooser_piece_type(chooser,chooser_piece_type_button,"Buttons","Button");

		xml_add_tagclose("Chooser");
		
		chooser++;
	}

	xml_add_tagclose("Choosers");

        // save the map
		
	file_paths_data(&file_path_setup,path,"Settings","Choosers","xml");
		
	ok=xml_save_file(path,err_str);
    xml_close_file();
	
	return(ok);
}
