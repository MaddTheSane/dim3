/****************************** File *********************************

Module: dim3 Setup
Author: Brian Barnes
 Usage: Property Palette Title Page Button

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

#define kButtonOptionOn							0
	
#define kButtonPositionX						10
#define kButtonPositionY						11
#define kButtonPositionWid						12
#define kButtonPositionHigh						13

#define kButtonMobileSkip						20
#define kButtonMobilePositionX					21
#define kButtonMobilePositionY					22

extern iface_type				iface;
extern list_palette_type		project_palette;

/* =======================================================

      Property Palette Title Page Button Utilities
      
======================================================= */

iface_intro_button_type* get_title_page_button_from_item_idx(int item_idx,char *name)
{
	switch (item_idx) {

		case item_intro_button_game_new:
			if (name!=NULL) strcpy(name,"Game New");
			return(&iface.intro.button_game_new);

		case item_intro_button_game_load:
			if (name!=NULL) strcpy(name,"Game Load");
			return(&iface.intro.button_game_load);

		case item_intro_button_game_setup:
			if (name!=NULL) strcpy(name,"Game Setup");
			return(&iface.intro.button_game_setup);

		case item_intro_button_multiplayer_host:
			if (name!=NULL) strcpy(name,"Multiplayer Host");
			return(&iface.intro.button_multiplayer_host);

		case item_intro_button_multiplayer_join:
			if (name!=NULL) strcpy(name,"Multiplayer Join");
			return(&iface.intro.button_multiplayer_join);

		case item_intro_button_credit:
			if (name!=NULL) strcpy(name,"Credits");
			return(&iface.intro.button_credit);

		case item_intro_button_quit:
			if (name!=NULL) strcpy(name,"Quit");
			return(&iface.intro.button_quit);
	}

	return(NULL);
}

/* =======================================================

      Property Palette Fill Title Page Button
      
======================================================= */

void project_palette_fill_title_page_button(int intro_button_idx)
{
	char							name[64];
	iface_intro_button_type			*btn;

	btn=get_title_page_button_from_item_idx(intro_button_idx,name);

	list_palette_set_title(&project_palette,"Title Page Button",name,NULL,NULL,NULL,NULL);

		// settings
	
	list_palette_add_header(&project_palette,0,"Settings");
	list_palette_add_checkbox(&project_palette,kButtonOptionOn,"On",&btn->on,FALSE);
	
		// position

	list_palette_add_header(&project_palette,0,"Position");
	list_palette_add_int(&project_palette,kButtonPositionX,"X",&btn->x,FALSE);
	list_palette_add_int(&project_palette,kButtonPositionY,"Y",&btn->y,FALSE);
	list_palette_add_int(&project_palette,kButtonPositionWid,"Width",&btn->wid,FALSE);
	list_palette_add_int(&project_palette,kButtonPositionHigh,"Height",&btn->high,FALSE);

		// mobile override

	list_palette_add_header(&project_palette,0,"Mobile Override");
	list_palette_add_checkbox(&project_palette,kButtonMobileSkip,"Hide if Mobile",&btn->mobile_hide,FALSE);
	list_palette_add_int(&project_palette,kButtonMobilePositionX,"X",&btn->mobile_x,FALSE);
	list_palette_add_int(&project_palette,kButtonMobilePositionY,"Y",&btn->mobile_y,FALSE);
}

/* =======================================================

      Property Palette Click Title Page Buttons
      
======================================================= */

void project_palette_click_title_page_button(int intro_button_idx,bool double_click)
{
}

