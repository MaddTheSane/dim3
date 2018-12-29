/****************************** File *********************************

Module: dim3 Setup
Author: Brian Barnes
 Usage: Property Palette Color

***************************** License ********************************

This code can be freely used as long as these conditions are met:

1. This header, in its entirety, is kept with the code
2. This credit “Created with dim3 Technology” is given on a single
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

#define kSettingsColorBackground				0
#define kSettingsColorDefaultTint				1

#define kSettingsColorMenuBackground			10
#define kSettingsColorMenuText					11
#define kSettingsColorMenuMouseOver				12
#define kSettingsColorMenuOutline				13

#define kSettingsColorDialogBackground			20
#define kSettingsColorDialogHeader				21
#define kSettingsColorDialogOutline				22
#define kSettingsColorDialogTitle				23

#define kSettingsColorTabBackground				30
#define kSettingsColorTabDimmed					31
#define kSettingsColorTabOutline				32
#define kSettingsColorTabText					33
#define kSettingsColorTabTextMouseOver			34
#define kSettingsColorTabTextDimmed				35

#define kSettingsColorControlLabel				40
#define kSettingsColorControlText				41
#define kSettingsColorControlFill				42
#define kSettingsColorControlHeader				43
#define kSettingsColorControlOutline			44
#define kSettingsColorControlMouseOver			45
#define kSettingsColorControlHilite				46
#define kSettingsColorControlDisabled			47

#define kSettingsColorScrollBarBackground		50
#define kSettingsColorScrollBarThumb			51

#define kSettingsColorButtonFill				60
#define kSettingsColorButtonText				61
#define kSettingsColorButtonOutline				62

#define kSettingsColorPickerFill				70
#define kSettingsColorPickerHilite				71
#define kSettingsColorPickerText				72
#define kSettingsColorPickerOutline				73

#define kSettingColorSystemMetric				80

extern iface_type				iface;
extern list_palette_type		project_palette;

/* =======================================================

      Property Palette Fill Color
      
======================================================= */

void project_palette_fill_color(void)
{
	list_palette_set_title(&project_palette,"Color",NULL,NULL,NULL,NULL,NULL);

		// main color
		
	list_palette_add_header(&project_palette,0,"Main Colors");
	list_palette_add_pick_color(&project_palette,kSettingsColorBackground,"Background",&iface.color.background,FALSE);
	
		// menu color
		
	list_palette_add_header(&project_palette,0,"Menu Colors");
	list_palette_add_pick_color(&project_palette,kSettingsColorMenuBackground,"Background",&iface.color.menu.background,FALSE);
	list_palette_add_pick_color(&project_palette,kSettingsColorMenuText,"Text",&iface.color.menu.text,FALSE);
	list_palette_add_pick_color(&project_palette,kSettingsColorMenuMouseOver,"Mouse Over",&iface.color.menu.mouse_over,FALSE);
	list_palette_add_pick_color(&project_palette,kSettingsColorMenuOutline,"Outline",&iface.color.menu.outline,FALSE);

		// dialog color
		
	list_palette_add_header(&project_palette,0,"Dialog Colors");
	list_palette_add_pick_color(&project_palette,kSettingsColorDialogBackground,"Dialog Background",&iface.color.dialog.background,FALSE);
	list_palette_add_pick_color(&project_palette,kSettingsColorDialogHeader,"Dialog Header",&iface.color.dialog.header,FALSE);
	list_palette_add_pick_color(&project_palette,kSettingsColorDialogOutline,"Dialog Outline",&iface.color.dialog.outline,FALSE);
	list_palette_add_pick_color(&project_palette,kSettingsColorDialogTitle,"Dialog Title",&iface.color.dialog.title,FALSE);
	
		// tab color
		
	list_palette_add_header(&project_palette,0,"Tab Colors");
	list_palette_add_pick_color(&project_palette,kSettingsColorTabBackground,"Tab Background",&iface.color.tab.background,FALSE);
	list_palette_add_pick_color(&project_palette,kSettingsColorTabDimmed,"Tab Dimmed",&iface.color.tab.dimmed,FALSE);
	list_palette_add_pick_color(&project_palette,kSettingsColorTabOutline,"Tab Outline",&iface.color.tab.outline,FALSE);
	list_palette_add_pick_color(&project_palette,kSettingsColorTabText,"Tab Text",&iface.color.tab.text,FALSE);
	list_palette_add_pick_color(&project_palette,kSettingsColorTabTextMouseOver,"Tab Text Mouse Over",&iface.color.tab.text_mouse_over,FALSE);
	list_palette_add_pick_color(&project_palette,kSettingsColorTabTextDimmed,"Tab Text Dimmed",&iface.color.tab.text_dimmed,FALSE);
	
		// control color
		
	list_palette_add_header(&project_palette,0,"Control Colors");
	list_palette_add_pick_color(&project_palette,kSettingsColorControlLabel,"Control Label",&iface.color.control.label,FALSE);
	list_palette_add_pick_color(&project_palette,kSettingsColorControlText,"Control Text",&iface.color.control.text,FALSE);
	list_palette_add_pick_color(&project_palette,kSettingsColorControlFill,"Control Fill",&iface.color.control.fill,FALSE);
	list_palette_add_pick_color(&project_palette,kSettingsColorControlHeader,"Control Header",&iface.color.control.header,FALSE);
	list_palette_add_pick_color(&project_palette,kSettingsColorControlOutline,"Control Outline",&iface.color.control.outline,FALSE);
	list_palette_add_pick_color(&project_palette,kSettingsColorControlMouseOver,"Control Mouse Over",&iface.color.control.mouse_over,FALSE);
	list_palette_add_pick_color(&project_palette,kSettingsColorControlHilite,"Control Highlight",&iface.color.control.hilite,FALSE);
	list_palette_add_pick_color(&project_palette,kSettingsColorControlDisabled,"Control Disabled",&iface.color.control.disabled,FALSE);
	
		// button color
		
	list_palette_add_header(&project_palette,0,"Button Colors");
	list_palette_add_pick_color(&project_palette,kSettingsColorButtonFill,"Button Fill",&iface.color.button.fill,FALSE);
	list_palette_add_pick_color(&project_palette,kSettingsColorButtonText,"Button Text",&iface.color.button.text,FALSE);
	list_palette_add_pick_color(&project_palette,kSettingsColorButtonOutline,"Button Outline",&iface.color.button.outline,FALSE);
	
		// picker color
		
	list_palette_add_header(&project_palette,0,"Picker Colors");
	list_palette_add_pick_color(&project_palette,kSettingsColorPickerFill,"Picker Fill",&iface.color.picker.fill,FALSE);
	list_palette_add_pick_color(&project_palette,kSettingsColorPickerHilite,"Picker Highlight",&iface.color.picker.hilite,FALSE);
	list_palette_add_pick_color(&project_palette,kSettingsColorPickerText,"Picker Text",&iface.color.picker.text,FALSE);
	list_palette_add_pick_color(&project_palette,kSettingsColorPickerOutline,"Picker Outline",&iface.color.picker.outline,FALSE);

		// system color
		
	list_palette_add_header(&project_palette,0,"System Colors");
	list_palette_add_pick_color(&project_palette,kSettingColorSystemMetric,"Metrics",&iface.color.system.metric,FALSE);
}

/* =======================================================

      Property Palette Click Color
      
======================================================= */

void project_palette_click_color(bool double_click)
{
}

