/****************************** File *********************************

Module: dim3 Setup
Author: Brian Barnes
 Usage: Property Palette Title Page

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

#define kIntroPropertyClickSound						0
#define kIntroPropertyMusic								1

#define kIntroPropertyButtonGameNew						10
#define kIntroPropertyButtonGameLoad					11
#define kIntroPropertyButtonGameSetup					12
#define kIntroPropertyButtonMultiplayerHost				13
#define kIntroPropertyButtonMultiplayerJoin				14
#define kIntroPropertyButtonCredit						15
#define kIntroPropertyButtonQuit						16

#define kIntroPropertySimpleSaveProgressOn				30
#define kIntroPropertySimpleSaveProgressMaxPoint		31
#define kIntroPropertySimpleSaveProgressMaxBitmap		32
#define kIntroPropertySimpleSaveProgressBitmap			33
#define kIntroPropertySimpleSaveProgressBitmapDisable	34

#define kIntroPropertyFont								70

#define kIntroPropertyScoreOn							80
#define kIntroPropertyScoreX							81
#define kIntroPropertyScoreY							82
#define kIntroPropertyScoreWid							83
#define kIntroPropertyScoreHigh							84
#define kIntroPropertyScoreTextSize						85
#define kIntroPropertyScoreFormat						86
#define kIntroPropertyScoreColor						87

#define kIntroPropertyModelAdd							100
#define kIntroPropertyModelName							1000
#define kIntroPropertyModelDelete						2000

extern iface_type				iface;
extern app_state_type			state;
extern list_palette_type		project_palette;

extern char						score_format_str[][32];

/* =======================================================

      Property Palette Fill Title Page
      
======================================================= */

void project_palette_fill_title_page(void)
{
	int				n;
	char			name[256];

	list_palette_set_title(&project_palette,"Title Page",NULL,NULL,NULL,NULL,NULL);

		// fonts
		
	list_palette_add_header(&project_palette,0,"Font");
	for (n=0;n!=max_iface_font_variant;n++) {
		sprintf(name,"Interface Font %d",n);
		list_palette_add_string(&project_palette,(kIntroPropertyFont+n),name,iface.font.interface_name[n],name_str_len,FALSE);
	}

		// sound
		
	list_palette_add_header(&project_palette,0,"Sound");
	list_palette_add_picker_list_string(&project_palette,kIntroPropertyClickSound,"Click",(char*)iface.sound_list.sounds,iface.sound_list.nsound,sizeof(iface_sound_type),(int)offsetof(iface_sound_type,name),TRUE,TRUE,iface.click_sound,FALSE);
	list_palette_add_picker_file(&project_palette,kIntroPropertyMusic,list_button_none,0,"Music","Music","mp3","",iface.intro.music,FALSE);

		// buttons

	list_palette_add_header(&project_palette,0,"Regular Buttons");
	list_palette_add_string_selectable(&project_palette,kIntroPropertyButtonGameNew,"Game New",NULL,-1,(state.proj.cur_intro_button_idx==item_intro_button_game_new),FALSE);
	list_palette_add_string_selectable(&project_palette,kIntroPropertyButtonGameLoad,"Game Load",NULL,-1,(state.proj.cur_intro_button_idx==item_intro_button_game_load),FALSE);
	list_palette_add_string_selectable(&project_palette,kIntroPropertyButtonGameSetup,"Game Setup",NULL,-1,(state.proj.cur_intro_button_idx==item_intro_button_game_setup),FALSE);
	list_palette_add_string_selectable(&project_palette,kIntroPropertyButtonMultiplayerHost,"Multiplayer Host",NULL,-1,(state.proj.cur_intro_button_idx==item_intro_button_multiplayer_host),FALSE);
	list_palette_add_string_selectable(&project_palette,kIntroPropertyButtonMultiplayerJoin,"Multiplayer Join",NULL,-1,(state.proj.cur_intro_button_idx==item_intro_button_multiplayer_join),FALSE);
	list_palette_add_string_selectable(&project_palette,kIntroPropertyButtonCredit,"Credit",NULL,-1,(state.proj.cur_intro_button_idx==item_intro_button_credit),FALSE);
	list_palette_add_string_selectable(&project_palette,kIntroPropertyButtonQuit,"Quit",NULL,-1,(state.proj.cur_intro_button_idx==item_intro_button_quit),FALSE);
	
		// simple save settings
		
	list_palette_add_header(&project_palette,0,"Simple Save Progress");
	list_palette_add_checkbox(&project_palette,kIntroPropertySimpleSaveProgressOn,"On",&iface.intro.simple_save_progress.on,FALSE);
	list_palette_add_int(&project_palette,kIntroPropertySimpleSaveProgressMaxPoint,"Max Points",&iface.intro.simple_save_progress.max_point,FALSE);
	list_palette_add_int(&project_palette,kIntroPropertySimpleSaveProgressMaxBitmap,"Max Bitmaps",&iface.intro.simple_save_progress.max_bitmap,FALSE);
	list_palette_add_picker_file(&project_palette,kIntroPropertySimpleSaveProgressBitmap,list_button_none,0,"Enabled Bitmap","Bitmaps/Interface","png","",iface.intro.simple_save_progress.bitmap_name,FALSE);
	list_palette_add_picker_file(&project_palette,kIntroPropertySimpleSaveProgressBitmapDisable,list_button_none,0,"Disabled Bitmap","Bitmaps/Interface","png","",iface.intro.simple_save_progress.bitmap_disable_name,FALSE);

		// score
		
	list_palette_add_header(&project_palette,0,"Scores");
	list_palette_add_checkbox(&project_palette,kIntroPropertyScoreOn,"On",&iface.intro.score.on,FALSE);
	list_palette_add_int(&project_palette,kIntroPropertyScoreX,"X",&iface.intro.score.x,FALSE);
	list_palette_add_int(&project_palette,kIntroPropertyScoreY,"Y",&iface.intro.score.y,FALSE);
	list_palette_add_int(&project_palette,kIntroPropertyScoreWid,"Width",&iface.intro.score.wid,FALSE);
	list_palette_add_int(&project_palette,kIntroPropertyScoreHigh,"Height",&iface.intro.score.high,FALSE);
	list_palette_add_int(&project_palette,kIntroPropertyScoreTextSize,"Text Size",&iface.intro.score.text_size,FALSE);
	list_palette_add_picker_list_int(&project_palette,kIntroPropertyScoreFormat,"Format",(char*)score_format_str,-1,name_str_len,0,FALSE,FALSE,&iface.intro.score.format,FALSE);
	list_palette_add_pick_color(&project_palette,kIntroPropertyScoreColor,"Color",&iface.intro.score.col,FALSE);

		// models

	list_palette_add_header_button(&project_palette,kIntroPropertyModelAdd,"Models",list_button_plus);

	list_palette_sort_mark_start(&project_palette,&project_palette.item_pane);
	
	for (n=0;n!=iface.intro.model_list.nmodel;n++) {
		list_palette_add_string_selectable_button(&project_palette,(kIntroPropertyModelName+n),list_button_minus,(kIntroPropertyModelDelete+n),iface.intro.model_list.models[n].model_name,(state.proj.cur_intro_model_idx==n),FALSE);
	}

	list_palette_sort(&project_palette,&project_palette.item_pane);
}

/* =======================================================

      Property Palette Click Title Page
      
======================================================= */

void project_palette_click_title_page(bool double_click)
{
	int					id,idx,sz;

	id=project_palette.item_pane.click.id;

		// select button

	if ((id>=kIntroPropertyButtonGameNew) && (id<=kIntroPropertyButtonQuit)) {
		state.proj.cur_intro_button_idx=(id-kIntroPropertyButtonGameNew)+item_intro_button_game_new;
		state.proj.cur_intro_model_idx=-1;
		if (double_click) list_palette_set_level(&project_palette,2);
		return;
	}

		// select model

	if ((id>=kIntroPropertyModelName) && (id<(kIntroPropertyModelName+max_hud_intro_model))) {
		state.proj.cur_intro_button_idx=-1;
		state.proj.cur_intro_model_idx=id-kIntroPropertyModelName;
		if (double_click) list_palette_set_level(&project_palette,2);
		return;
	}

		// delete model

	if ((id>=kIntroPropertyModelDelete) && (id<(kIntroPropertyModelDelete+max_hud_intro_model))) {
		state.proj.cur_intro_button_idx=-1;
		state.proj.cur_intro_model_idx=-1;

		idx=id-kIntroPropertyModelDelete;

		sz=(iface.intro.model_list.nmodel-idx)-1;
		if (sz>0) memmove(&iface.intro.model_list.models[idx],&iface.intro.model_list.models[idx+1],(sz*sizeof(iface_intro_model_type)));

		iface.intro.model_list.nmodel--;

		return;
	}

		// add model

	if (id==kIntroPropertyModelAdd) {
		state.proj.cur_intro_button_idx=-1;
		state.proj.cur_intro_model_idx=-1;

		if (iface.intro.model_list.nmodel>=max_hud_intro_model) {
			os_dialog_alert("Add Intro Model","Reached the maximum number of models");
			return;
		}

		idx=iface.intro.model_list.nmodel;
		iface.intro.model_list.nmodel++;

		iface.intro.model_list.models[idx].model_name[0]=0x0;
		iface.intro.model_list.models[idx].x=0;
		iface.intro.model_list.models[idx].y=0;
		iface.intro.model_list.models[idx].resize=1.0f;
		iface.intro.model_list.models[idx].rot.x=0.0f;
		iface.intro.model_list.models[idx].rot.y=0.0f;
		iface.intro.model_list.models[idx].rot.z=0.0f;
		iface.intro.model_list.models[idx].animate_name[0]=0x0;

		state.proj.cur_intro_model_idx=idx;
		
		list_palette_set_level(&project_palette,2);
		property_pick_file("Pick a Model","Models",NULL,"Mesh.xml;Model.xml",iface.intro.model_list.models[idx].model_name);

		return;
	}
}

