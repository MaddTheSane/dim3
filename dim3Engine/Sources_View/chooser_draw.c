/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: GUI Chooser

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
#include "scripts.h"

extern server_type			server;
extern iface_type			iface;
extern js_type				js;
extern setup_type			setup;
extern file_path_setup_type	file_path_setup;

int							chooser_idx;
bool						chooser_key_held_down;
char						chooser_sub_txt[max_chooser_sub_txt][max_chooser_text_data_sz];

/* =======================================================

      Text Substitutions
      
======================================================= */

void chooser_text_substitute(char *init_str,char *sub_str,int max_len)
{
	int				idx,len,sz;
	char			ch,*c,*c2;
	
	c=init_str;
	c2=sub_str;
	
	while (*c!=0x0) {
	
			// are we over text string max size?
			
		if ((int)(c2-sub_str)>=(max_len-1)) break;
	
			// are we hitting a {0..9} substitution string?
			
		if (*c=='{') {
		
			ch=*(c+1);
			idx=(int)(ch-'0');
			
			if ((idx>=0) && (idx<max_chooser_sub_txt) && (*(c+2)=='}')) {
			
				if (chooser_sub_txt[idx][0]!=0x0) {
					sz=(max_len-1)-(int)(c-init_str);
					len=strlen(chooser_sub_txt[idx]);
					if (len>sz) break;
					
					*c2=0x0;
					strcpy(c2,chooser_sub_txt[idx]);
					c2+=len;
				}
				
				c+=3;
				continue;
			}
		}
		
		*c2++=*c++;
	}
	
	*c2=0x0;
}

/* =======================================================

      Chooser Operations
      
======================================================= */

void chooser_create_elements(void)
{
	int							n,idx,template_idx;
	char						path[1024],path2[1024],fname[256],
								str[max_chooser_text_data_sz],title[max_chooser_frame_text_sz];
	iface_chooser_type			*chooser,*template_chooser;
	iface_chooser_piece_type	*piece,*template_piece;
	iface_chooser_frame_type	*frame;
	
	chooser=&iface.chooser_list.choosers[chooser_idx];
	template_chooser=NULL;

		// check for template

	template_idx=iface_chooser_find_idx(&iface,chooser->template_name);
	if (template_idx!=-1) template_chooser=&iface.chooser_list.choosers[template_idx];
	
		// setup frame
		
	frame=NULL;

	if (template_chooser==NULL) {
		if (chooser->frame.on) frame=&chooser->frame;
	}
	else {
		if (template_chooser->frame.on) frame=&template_chooser->frame;
	}

	if (frame!=NULL) {
		chooser_text_substitute(frame->title,title,max_chooser_frame_text_sz);
		element_frame_add(title,-1,frame->x,frame->y,frame->wid,frame->high,-1,0,NULL,0,NULL);
	}

		// mark regular pieces as not being
		// used to override a template

	piece=chooser->pieces;

	for (n=0;n!=chooser->npiece;n++) {
		piece->used_in_override=FALSE;
		piece++;
	}

		// template pieces

	if (template_chooser!=NULL) {

		template_piece=template_chooser->pieces;

		for (n=0;n!=template_chooser->npiece;n++) {

				// get the override

			piece=NULL;

			idx=iface_chooser_find_piece_idx(chooser,template_piece->id);
			if (idx!=-1) {
				piece=&chooser->pieces[idx];
				piece->used_in_override=TRUE;
			}

			if (piece==NULL) piece=template_piece;

				// add template pieces

			switch (template_piece->type) {

				case chooser_piece_type_text:
					chooser_text_substitute(piece->data.text.str,str,max_chooser_text_data_sz);
					element_text_add(str,template_piece->id,template_piece->x,template_piece->y,template_piece->data.text.size,template_piece->data.text.just,NULL,template_piece->clickable);
					break;

				case chooser_piece_type_item:
					file_paths_data(&file_path_setup,path,"Chooser",piece->data.item.file,"png");
					if (template_piece->clickable) {
						sprintf(fname,"%s_selected",template_piece->data.item.file);
						file_paths_data(&file_path_setup,path2,"Chooser",fname,"png");
						element_button_bitmap_add(path,path2,template_piece->id,template_piece->x,template_piece->y,template_piece->wid,template_piece->high,element_pos_left,element_pos_top);
					}
					else {
						element_bitmap_add(path,0,template_piece->x,template_piece->y,template_piece->wid,template_piece->high,FALSE);
					}
					break;
					
				case chooser_piece_type_model:
					element_model_add(piece->data.model.model_name,piece->data.model.animate_name,template_piece->data.model.resize,NULL,&template_piece->data.model.rot,template_piece->id,template_piece->x,template_piece->y);
					break;

				case chooser_piece_type_button:
					element_button_text_add(piece->data.button.name,template_piece->id,template_piece->x,template_piece->y,template_piece->wid,template_piece->high,element_pos_left,element_pos_top);
					break;

			}

			template_piece++;
		}
	}

		// normal pieces

	for (n=0;n!=chooser->npiece;n++) {

		piece=&chooser->pieces[n];
		if (piece->used_in_override) continue;

		switch (piece->type) {

			case chooser_piece_type_text:
				chooser_text_substitute(piece->data.text.str,str,max_chooser_text_data_sz);
				element_text_add(str,piece->id,piece->x,piece->y,piece->data.text.size,piece->data.text.just,NULL,piece->clickable);
				break;

			case chooser_piece_type_item:
				file_paths_data(&file_path_setup,path,"Chooser",piece->data.item.file,"png");
				
				if (piece->clickable) {
					sprintf(fname,"%s_selected",piece->data.item.file);
					file_paths_data(&file_path_setup,path2,"Chooser",fname,"png");
					element_button_bitmap_add(path,path2,piece->id,piece->x,piece->y,piece->wid,piece->high,element_pos_left,element_pos_top);
				}
				else {
					element_bitmap_add(path,0,piece->x,piece->y,piece->wid,piece->high,FALSE);
				}
				break;
				
			case chooser_piece_type_model:
				element_model_add(piece->data.model.model_name,piece->data.model.animate_name,piece->data.model.resize,NULL,&piece->data.model.rot,piece->id,piece->x,piece->y);
				break;

			case chooser_piece_type_button:
				element_button_text_add(piece->data.button.name,piece->id,piece->x,piece->y,piece->wid,piece->high,element_pos_left,element_pos_top);
				break;

		}
	}

		// no key downs

	chooser_key_held_down=FALSE;
}

void chooser_open(void)
{
	gui_initialize(NULL,NULL);
	chooser_create_elements();
}

void chooser_close(void)
{
	gui_shutdown();
}

bool chooser_setup(char *name,char *sub_txt,char *err_str)
{
	int				n;

		// find chooser

	chooser_idx=iface_chooser_find_idx(&iface,name);
	if (chooser_idx==-1) {
		sprintf(err_str,"Chooser does not exist: %s",name);
		return(FALSE);
	}

		// setup text substitutions

	if (sub_txt!=NULL) {
		for (n=0;n!=max_chooser_sub_txt;n++) {
			strcpy(chooser_sub_txt[n],(char*)&sub_txt[max_chooser_text_data_sz*n]);
		}
	}
	
		// run chooser
		
	server.next_state=gs_chooser;
	
	return(TRUE);
}

/* =======================================================

      Chooser Keys
      
======================================================= */

bool chooser_is_key_down(iface_chooser_piece_type *piece)
{
	switch (piece->key) {
		case chooser_key_return:
			return(input_get_keyboard_return());
		case chooser_key_escape:
			return(input_get_keyboard_escape());
		case chooser_key_space:
			return(input_get_keyboard_key(SDL_SCANCODE_SPACE));
		case chooser_key_0:
			return(input_get_keyboard_key(SDL_SCANCODE_0));
	}

	if ((piece->key>=chooser_key_1) && (piece->key<=chooser_key_9)) return(input_get_keyboard_key(SDL_SCANCODE_1+(piece->key-chooser_key_1)));
	if ((piece->key>=chooser_key_A) && (piece->key<=chooser_key_Z)) return(input_get_keyboard_key(SDL_SCANCODE_A+(piece->key-chooser_key_A)));

	return(FALSE);
}

/* =======================================================

      Chooser Input
      
======================================================= */

void chooser_input(void)
{
	int							n,id,idx,next_idx,template_idx;
	bool						leave_chooser;
	iface_chooser_type			*chooser;
	iface_chooser_piece_type	*piece;
	
	id=-1;

	chooser=&iface.chooser_list.choosers[chooser_idx];

		// check for any key presses

	for (n=0;n!=chooser->npiece;n++) {
		piece=&chooser->pieces[n];
		if (piece->key==chooser_key_none) continue;

		if (chooser_is_key_down(piece)) id=piece->id;
	}

	if (chooser_key_held_down) {
		if (id==-1) chooser_key_held_down=FALSE;
		id=-1;
	}

		// if no key check clicking
		
	if (id==-1) {
		id=gui_click();
		if (id==-1) return;
	}
	
		// run click
		
	hud_click();
	
		// check for any goto clicks

	piece=NULL;

	template_idx=iface_chooser_find_idx(&iface,chooser->template_name);
	if (template_idx!=-1) {
		idx=iface_chooser_find_piece_idx(&iface.chooser_list.choosers[template_idx],id);
		piece=&iface.chooser_list.choosers[template_idx].pieces[idx];
	}

	if (piece==NULL) {
		idx=iface_chooser_find_piece_idx(chooser,id);
		piece=&chooser->pieces[idx];
	}
	
	if (piece!=NULL) {
	
		next_idx=iface_chooser_find_idx(&iface,piece->goto_name);
		
		if (next_idx!=-1) {
			element_clear();
			chooser_idx=next_idx;
			chooser_create_elements();
			return;
		}
	}

		// check if this item leaves
		// the chooser

	leave_chooser=TRUE;

	if (piece!=NULL) {
		leave_chooser=!piece->no_close;
	}
	
		// set the state here as event
		// might reset it to something else
		
	if (leave_chooser) server.next_state=gs_running;
	
	scripts_post_event_console(js.game_script_idx,-1,sd_event_chooser,0,id);
}

/* =======================================================

      Run Chooser
      
======================================================= */

void chooser_run(void)
{
	gui_draw(1.0f,TRUE);
	chooser_input();
}
