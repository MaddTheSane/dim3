/****************************** File *********************************

Module: dim3 Setup
Author: Brian Barnes
 Usage: Property Palette Chooser Piece

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

#define kChooserPieceSettingsType				0
#define kChooserPieceSettingsId					1
#define kChooserPieceSettingsClickable			2
#define kChooserPieceSettingsNoClose			3
#define kChooserPieceSettingsKey				4
#define kChooserPieceSettingsGoto				5

#define kChooserPiecePositionX					10
#define kChooserPiecePositionY					11
#define kChooserPiecePositionWid				12
#define kChooserPiecePositionHigh				13

#define kChooserPieceTextStr					20
#define kChooserPieceTextSize					21
#define kChooserPieceTextJust					22
#define kChooserPieceItemFile					23
#define kChooserPieceModelModelName				24
#define kChooserPieceModelAnimateName			25
#define kChooserPieceModelResize				26
#define kChooserPieceModelRot					27
#define kChooserPieceButtonText					28

extern iface_type				iface;
extern list_palette_type		project_palette;

extern char						chooser_type_str[][32],
								chooser_key_str[][32],
								hud_text_just_type_str[][32];

/* =======================================================

      Property Palette Fill Chooser Piece
      
======================================================= */

void project_palette_fill_chooser_piece(int chooser_idx,int chooser_piece_idx)
{
	iface_chooser_type				*chooser;
	iface_chooser_piece_type		*piece;

	chooser=&iface.chooser_list.choosers[chooser_idx];
	piece=&chooser->pieces[chooser_piece_idx];

	list_palette_set_title(&project_palette,"Choosers",NULL,"Chooser",chooser->name,"Item",chooser_type_str[piece->type]);

		// settings

	list_palette_add_header(&project_palette,0,"Settings");
	list_palette_add_picker_list_int(&project_palette,kChooserPieceSettingsType,"Type",(char*)chooser_type_str,-1,name_str_len,0,FALSE,FALSE,&piece->type,FALSE);
	list_palette_add_int(&project_palette,kChooserPieceSettingsId,"Id",&piece->id,FALSE);
	list_palette_add_checkbox(&project_palette,kChooserPieceSettingsClickable,"Clickable",&piece->clickable,FALSE);
	list_palette_add_checkbox(&project_palette,kChooserPieceSettingsNoClose,"No Close",&piece->no_close,FALSE);
	list_palette_add_picker_list_int(&project_palette,kChooserPieceSettingsKey,"Key",(char*)chooser_key_str,-1,name_str_len,0,FALSE,FALSE,&piece->key,FALSE);
	list_palette_add_string(&project_palette,kChooserPieceSettingsGoto,"Goto Chooser",piece->goto_name,name_str_len,FALSE);

		// position

	list_palette_add_header(&project_palette,0,"Position");
	list_palette_add_int(&project_palette,kChooserPiecePositionX,"X",&piece->x,FALSE);
	list_palette_add_int(&project_palette,kChooserPiecePositionY,"Y",&piece->y,FALSE);
	list_palette_add_int(&project_palette,kChooserPiecePositionWid,"Width",&piece->wid,FALSE);
	list_palette_add_int(&project_palette,kChooserPiecePositionHigh,"Height",&piece->high,FALSE);

		// types

	switch (piece->type) {

		case chooser_piece_type_text:
			list_palette_add_header(&project_palette,0,"Text");
			list_palette_add_string(&project_palette,kChooserPieceTextStr,"Text",piece->data.text.str,max_chooser_text_data_sz,FALSE);
			list_palette_add_int(&project_palette,kChooserPieceTextSize,"Size",&piece->data.text.size,FALSE);
			list_palette_add_picker_list_int(&project_palette,kChooserPieceTextJust,"Justification",(char*)hud_text_just_type_str,-1,name_str_len,0,FALSE,FALSE,&piece->data.text.just,FALSE);
			break;

		case chooser_piece_type_item:
			list_palette_add_header(&project_palette,0,"Item");
			list_palette_add_picker_file(&project_palette,kChooserPieceItemFile,list_button_none,0,"Bitmap","Chooser","png","",piece->data.item.file,FALSE);
			break;

		case chooser_piece_type_model:
			list_palette_add_header(&project_palette,0,"Model");
			list_palette_add_picker_file(&project_palette,kChooserPieceModelModelName,list_button_none,0,"Model","Models","","Mesh.xml;Model.xml",piece->data.model.model_name,FALSE);
			list_palette_add_string(&project_palette,kChooserPieceModelAnimateName,"Animation",piece->data.model.animate_name,name_str_len,FALSE);
			list_palette_add_float(&project_palette,kChooserPieceModelResize,"Resize",&piece->data.model.resize,FALSE);
			list_palette_add_angle(&project_palette,kChooserPieceModelRot,"Rotate",&piece->data.model.rot,FALSE);
			break;

		case chooser_piece_type_button:
			list_palette_add_header(&project_palette,0,"Button");
			list_palette_add_string(&project_palette,kChooserPieceButtonText,"Text",piece->data.button.name,max_chooser_text_data_sz,FALSE);
			break;

	}
}

/* =======================================================

      Property Palette Click Chooser Piece
      
======================================================= */

void project_palette_click_chooser_piece(int chooser_idx,int chooser_piece_idx,bool double_click)
{
	iface_chooser_piece_type		*piece;

	if (!double_click) return;

	piece=&iface.chooser_list.choosers[chooser_idx].pieces[chooser_piece_idx];

	switch (project_palette.item_pane.click.id) {

			// settings

		case kChooserPieceSettingsType:
			bzero(&piece->data,sizeof(iface_chooser_piece_data_type));		// need to clear data or it'll crash
			break;

	}
}

