/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Project Tool Palette

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

#define project_tool_count		1

extern map_type					map;
extern app_state_type			state;
extern app_pref_type			pref;
extern file_path_setup_type		file_path_setup;

tool_palette_type				project_tool_palette;

tool_palette_tool_type			project_tool_palette_setup[project_tool_count]=
									{
										{"Project/Tool Run Project",FALSE,"Run Project In Engine"},
									};

/* =======================================================

      Project Tool Palette Initialize/Shutdown
      
======================================================= */

void project_tool_palette_initialize(void)
{
	tool_palette_initialize(&project_tool_palette,project_tool_count,project_tool_palette_setup);
}

void project_tool_palette_shutdown(void)
{
	tool_palette_shutdown(&project_tool_palette);
}

/* =======================================================

      Project Tool Palette State
      
======================================================= */

void project_tool_palette_set_state(void)
{
	tool_palette_set_state(&project_tool_palette,0,TRUE,FALSE);
}

/* =======================================================

      Project Tool Palette Clicking
      
======================================================= */

void project_tool_palette_click(int tool_idx)
{
	switch (tool_idx) {
			
		case 0:
			launch_engine();
			break;
						
	}
	
	main_wind_draw();
}

/* =======================================================

      Project Tool Palette Default State
      
======================================================= */

void project_tool_default(void)
{
}
