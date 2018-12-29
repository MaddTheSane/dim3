/****************************** File *********************************

Module: dim3 Setup
Author: Brian Barnes
 Usage: Property Palette HUD

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

#define kHUDPropertyChatX						0
#define kHUDPropertyChatY						1
#define kHUDPropertyChatLastAddLifeSeconds		2
#define kHUDPropertyChatNextLifeSeconds			3

#define kHUDPropertyFont						10

#define kHUDPropertyBitmapAdd					1000
#define kHUDPropertyBitmapName					2000
#define kHUDPropertyBitmapDelete				3000

#define kHUDPropertyTextAdd						4000
#define kHUDPropertyTextName					5000
#define kHUDPropertyTextDelete					6000

#define kHUDPropertyBarAdd						7000
#define kHUDPropertyBarName						8000
#define kHUDPropertyBarDelete					9000

extern iface_type				iface;
extern app_state_type			state;
extern list_palette_type		project_palette;

/* =======================================================

      Property Palette Fill HUD
      
======================================================= */

void project_palette_fill_hud(void)
{
	int				n;
	char			name[256];

	list_palette_set_title(&project_palette,"HUD",NULL,NULL,NULL,NULL,NULL);

		// fonts
		
	list_palette_add_header(&project_palette,0,"Font");
	for (n=0;n!=max_iface_font_variant;n++) {
		sprintf(name,"HUD Font %d",n);
		list_palette_add_string(&project_palette,(kHUDPropertyFont+n),name,iface.font.hud_name[n],name_str_len,FALSE);
	}

		// bitmaps

	list_palette_add_header_button(&project_palette,kHUDPropertyBitmapAdd,"Bitmaps",list_button_plus);

	list_palette_sort_mark_start(&project_palette,&project_palette.item_pane);
	
	for (n=0;n!=iface.bitmap_list.nbitmap;n++) {
		list_palette_add_string_selectable_button(&project_palette,(kHUDPropertyBitmapName+n),list_button_minus,(kHUDPropertyBitmapDelete+n),iface.bitmap_list.bitmaps[n].name,(state.proj.cur_hud_bitmap_idx==n),FALSE);
	}

	list_palette_sort(&project_palette,&project_palette.item_pane);

		// texts

	list_palette_add_header_button(&project_palette,kHUDPropertyTextAdd,"Texts",list_button_plus);

	list_palette_sort_mark_start(&project_palette,&project_palette.item_pane);
	
	for (n=0;n!=iface.text_list.ntext;n++) {
		list_palette_add_string_selectable_button(&project_palette,(kHUDPropertyTextName+n),list_button_minus,(kHUDPropertyTextDelete+n),iface.text_list.texts[n].name,(state.proj.cur_hud_text_idx==n),FALSE);
	}

	list_palette_sort(&project_palette,&project_palette.item_pane);

		// bars

	list_palette_add_header_button(&project_palette,kHUDPropertyBarAdd,"Bars",list_button_plus);

	list_palette_sort_mark_start(&project_palette,&project_palette.item_pane);
	
	for (n=0;n!=iface.bar_list.nbar;n++) {
		list_palette_add_string_selectable_button(&project_palette,(kHUDPropertyBarName+n),list_button_minus,(kHUDPropertyBarDelete+n),iface.bar_list.bars[n].name,(state.proj.cur_hud_bar_idx==n),FALSE);
	}

	list_palette_sort(&project_palette,&project_palette.item_pane);
	
		// chat
		
	list_palette_add_header(&project_palette,0,"Chat");
	list_palette_add_int(&project_palette,kHUDPropertyChatX,"X",&iface.chat.x,FALSE);
	list_palette_add_int(&project_palette,kHUDPropertyChatY,"Y",&iface.chat.y,FALSE);
	list_palette_add_int(&project_palette,kHUDPropertyChatLastAddLifeSeconds,"Last Add Life Seconds",&iface.chat.last_add_life_sec,FALSE);
	list_palette_add_int(&project_palette,kHUDPropertyChatNextLifeSeconds,"Next Life Seconds",&iface.chat.next_life_sec,FALSE);
}

/* =======================================================

      Property Palette Click HUD
      
======================================================= */

void project_palette_click_hud(bool double_click)
{
	int					id,idx,sz;
	char				name[name_str_len];

	id=project_palette.item_pane.click.id;

		// select bitmap

	if ((id>=kHUDPropertyBitmapName) && (id<(kHUDPropertyBitmapName+max_iface_bitmap))) {
		state.proj.cur_hud_bitmap_idx=id-kHUDPropertyBitmapName;
		state.proj.cur_hud_text_idx=-1;
		state.proj.cur_hud_bar_idx=-1;
		if (double_click) list_palette_set_level(&project_palette,2);
		return;
	}

		// delete bitmap

	if ((id>=kHUDPropertyBitmapDelete) && (id<(kHUDPropertyBitmapDelete+max_iface_bitmap))) {
		state.proj.cur_hud_bitmap_idx=-1;
		state.proj.cur_hud_text_idx=-1;
		state.proj.cur_hud_bar_idx=-1;

		idx=id-kHUDPropertyBitmapDelete;

		sz=(iface.bitmap_list.nbitmap-idx)-1;
		if (sz>0) memmove(&iface.bitmap_list.bitmaps[idx],&iface.bitmap_list.bitmaps[idx+1],(sz*sizeof(iface_bitmap_type)));

		iface.bitmap_list.nbitmap--;

		return;
	}

		// add bitmap

	if (id==kHUDPropertyBitmapAdd) {
		state.proj.cur_hud_text_idx=-1;
		state.proj.cur_hud_bar_idx=-1;

		if (iface.bitmap_list.nbitmap>=max_iface_bitmap) {
			os_dialog_alert("Add HUD Bitmap","Reached the maximum number of bitmaps");
			return;
		}
		
		name[0]=0x0;
		dialog_property_string_run(list_string_value_string,(void*)name,name_str_len,0,0);
		if (name[0]==0x0) return;

		idx=iface.bitmap_list.nbitmap;
		iface.bitmap_list.nbitmap++;

		iface.bitmap_list.bitmaps[idx].pnt.x=0;
		iface.bitmap_list.bitmaps[idx].pnt.y=0;
		iface.bitmap_list.bitmaps[idx].size.x=-1;
		iface.bitmap_list.bitmaps[idx].size.y=-1;
		iface.bitmap_list.bitmaps[idx].alpha=1.0f;
		iface.bitmap_list.bitmaps[idx].rot=0.0f;
		strcpy(iface.bitmap_list.bitmaps[idx].name,name);
		iface.bitmap_list.bitmaps[idx].filename[0]=0x0;
		iface.bitmap_list.bitmaps[idx].show=TRUE;
		iface.bitmap_list.bitmaps[idx].flash=FALSE;
		iface.bitmap_list.bitmaps[idx].flip_horz=FALSE;
		iface.bitmap_list.bitmaps[idx].flip_vert=FALSE;
		iface.bitmap_list.bitmaps[idx].team_tint=FALSE;
		iface.bitmap_list.bitmaps[idx].checkpoint=FALSE;

		iface.bitmap_list.bitmaps[idx].repeat.on=FALSE;
		iface.bitmap_list.bitmaps[idx].repeat.count=0;
		iface.bitmap_list.bitmaps[idx].repeat.x_add=0;
		iface.bitmap_list.bitmaps[idx].repeat.y_add=0;
		iface.bitmap_list.bitmaps[idx].repeat.col=0;
		iface.bitmap_list.bitmaps[idx].repeat.row=0;
		
		iface.bitmap_list.bitmaps[idx].fade.on=FALSE;
		iface.bitmap_list.bitmaps[idx].fade.fade_in_tick=0;
		iface.bitmap_list.bitmaps[idx].fade.life_tick=0;
		iface.bitmap_list.bitmaps[idx].fade.fade_out_tick=0;

		iface.bitmap_list.bitmaps[idx].animate.image_count=1;
		iface.bitmap_list.bitmaps[idx].animate.msec=0;
		iface.bitmap_list.bitmaps[idx].animate.loop=FALSE;
		iface.bitmap_list.bitmaps[idx].animate.loop_back=FALSE;

		state.proj.cur_hud_bitmap_idx=idx;

		return;
	}

		// select text

	if ((id>=kHUDPropertyTextName) && (id<(kHUDPropertyTextName+max_iface_text))) {
		state.proj.cur_hud_bitmap_idx=-1;
		state.proj.cur_hud_text_idx=id-kHUDPropertyTextName;
		state.proj.cur_hud_bar_idx=-1;
		if (double_click) list_palette_set_level(&project_palette,2);
		return;
	}

		// delete text

	if ((id>=kHUDPropertyTextDelete) && (id<(kHUDPropertyTextDelete+max_iface_text))) {
		state.proj.cur_hud_bitmap_idx=-1;
		state.proj.cur_hud_text_idx=-1;
		state.proj.cur_hud_bar_idx=-1;

		idx=id-kHUDPropertyTextDelete;

		sz=(iface.text_list.ntext-idx)-1;
		if (sz>0) memmove(&iface.text_list.texts[idx],&iface.text_list.texts[idx+1],(sz*sizeof(iface_text_type)));

		iface.text_list.ntext--;

		return;
	}

		// add text

	if (id==kHUDPropertyTextAdd) {
		state.proj.cur_hud_bitmap_idx=-1;
		state.proj.cur_hud_bar_idx=-1;

		if (iface.text_list.ntext>=max_iface_text) {
			os_dialog_alert("Add HUD Text","Reached the maximum number of texts");
			return;
		}
		
		name[0]=0x0;
		dialog_property_string_run(list_string_value_string,(void*)name,name_str_len,0,0);
		if (name[0]==0x0) return;

		idx=iface.text_list.ntext;
		iface.text_list.ntext++;

		iface.text_list.texts[idx].pnt.x=0;
		iface.text_list.texts[idx].pnt.y=0;
		iface.text_list.texts[idx].size=20;
		iface.text_list.texts[idx].just=tx_left;
		iface.text_list.texts[idx].special=text_special_none;
		iface.text_list.texts[idx].alpha=1.0f;
		strcpy(iface.text_list.texts[idx].name,name);
		iface.text_list.texts[idx].data[0]=0x0;
		iface.text_list.texts[idx].show=TRUE;
		iface.text_list.texts[idx].monospaced=FALSE;
		iface.text_list.texts[idx].has_return=FALSE;
		iface.text_list.texts[idx].checkpoint=FALSE;
		iface.text_list.texts[idx].color.r=1.0f;
		iface.text_list.texts[idx].color.g=1.0f;
		iface.text_list.texts[idx].color.b=1.0f;

		iface.text_list.texts[idx].fade.on=FALSE;
		iface.text_list.texts[idx].fade.fade_in_tick=0;
		iface.text_list.texts[idx].fade.life_tick=0;
		iface.text_list.texts[idx].fade.fade_out_tick=0;

		state.proj.cur_hud_text_idx=idx;

		return;
	}

		// select bar

	if ((id>=kHUDPropertyBarName) && (id<(kHUDPropertyBarName+max_iface_bar))) {
		state.proj.cur_hud_bitmap_idx=-1;
		state.proj.cur_hud_text_idx=-1;
		state.proj.cur_hud_bar_idx=id-kHUDPropertyBarName;
		if (double_click) list_palette_set_level(&project_palette,2);
		return;
	}

		// delete bar

	if ((id>=kHUDPropertyBarDelete) && (id<(kHUDPropertyBarDelete+max_iface_bar))) {
		state.proj.cur_hud_bitmap_idx=-1;
		state.proj.cur_hud_text_idx=-1;
		state.proj.cur_hud_bar_idx=-1;

		idx=id-kHUDPropertyBarDelete;

		sz=(iface.bar_list.nbar-idx)-1;
		if (sz>0) memmove(&iface.bar_list.bars[idx],&iface.bar_list.bars[idx+1],(sz*sizeof(iface_bar_type)));

		iface.bar_list.nbar--;

		return;
	}

		// add bar

	if (id==kHUDPropertyBarAdd) {
		state.proj.cur_hud_bitmap_idx=-1;
		state.proj.cur_hud_text_idx=-1;

		if (iface.bar_list.nbar>=max_iface_bar) {
			os_dialog_alert("Add HUD Bar","Reached the maximum number of bars");
			return;
		}
		
		name[0]=0x0;
		dialog_property_string_run(list_string_value_string,(void*)name,name_str_len,0,0);
		if (name[0]==0x0) return;

		idx=iface.bar_list.nbar;
		iface.bar_list.nbar++;
		
		strcpy(iface.bar_list.bars[idx].name,name);
		iface.bar_list.bars[idx].type=bar_type_horizontal;

		iface.bar_list.bars[idx].pnt.x=0;
		iface.bar_list.bars[idx].pnt.y=0;
		iface.bar_list.bars[idx].size.x=20;
		iface.bar_list.bars[idx].size.y=20;
		iface.bar_list.bars[idx].fill_alpha=1.0f;
		iface.bar_list.bars[idx].outline_alpha=1.0f;
		iface.bar_list.bars[idx].value=0.0f;
		iface.bar_list.bars[idx].show=TRUE;
		iface.bar_list.bars[idx].outline=TRUE;

		iface.bar_list.bars[idx].fill_start_color.r=0.5f;
		iface.bar_list.bars[idx].fill_start_color.g=0.5f;
		iface.bar_list.bars[idx].fill_start_color.b=0.5f;

		iface.bar_list.bars[idx].fill_end_color.r=0.8f;
		iface.bar_list.bars[idx].fill_end_color.g=0.8f;
		iface.bar_list.bars[idx].fill_end_color.b=0.8f;

		iface.bar_list.bars[idx].outline_color.r=1.0f;
		iface.bar_list.bars[idx].outline_color.g=1.0f;
		iface.bar_list.bars[idx].outline_color.b=1.0f;

		state.proj.cur_hud_bar_idx=idx;

		return;
	}
}

