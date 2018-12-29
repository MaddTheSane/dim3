/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Property Palette Map Camera

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

#define kMapPropertyCameraMode						0
#define kMapPropertyInputMode						1

#define kMapPropertyCameraAngle						10
#define kMapPropertyCameraPoint						11

#define kMapPropertyCameraFOV						20
#define kMapPropertyCameraAspectRatio				21
#define kMapPropertyCameraNearZ						22
#define kMapPropertyCameraFarZ						23
#define kMapPropertyCameraNearZOffset				24

#define kMapPropertyCameraChaseDistance				30
#define kMapPropertyCameraChaseTrackSpeed			31
#define kMapPropertyCameraChaseTrackAngle			32
#define kMapPropertyCameraChaseSlop					33

#define kMapPropertyCameraStaticFollow				40
#define kMapPropertyCameraStaticAttachNode			41

extern map_type					map;

extern list_palette_type		map_palette;

char							map_map_camera_mode_list[][name_str_len]={"FPP","Chase","Static","Chase Static",""},
								map_map_camera_input_list[][name_str_len]={"FPP","Side Scroll","Top Down","Fly","Thrust",""};

/* =======================================================

      Property Palette Fill Map Camera
      
======================================================= */

void map_palette_fill_camera(void)
{
	list_palette_set_title(&map_palette,"Map Camera",NULL,NULL,NULL,NULL,NULL);

		// camera settings

	list_palette_add_header(&map_palette,0,"Map Camera Settings");
	list_palette_add_picker_list_int(&map_palette,kMapPropertyCameraMode,"Camera Mode",(char*)map_map_camera_mode_list,-1,name_str_len,0,FALSE,FALSE,&map.camera.camera_mode,FALSE);
	list_palette_add_picker_list_int(&map_palette,kMapPropertyInputMode,"Input Mode",(char*)map_map_camera_input_list,-1,name_str_len,0,FALSE,FALSE,&map.camera.input_mode,FALSE);

		// camera settings

	list_palette_add_header(&map_palette,0,"Map Camera Offsets");
	list_palette_add_angle(&map_palette,kMapPropertyCameraAngle,"Angle Offset",&map.camera.ang_offset,FALSE);
	list_palette_add_point(&map_palette,kMapPropertyCameraPoint,"Point Offset",&map.camera.pnt_offset,FALSE);

		// camera plane

	list_palette_add_header(&map_palette,0,"Map Camera Plane");
	list_palette_add_float(&map_palette,kMapPropertyCameraFOV,"FOV",&map.camera.plane.fov,FALSE);
	list_palette_add_float(&map_palette,kMapPropertyCameraAspectRatio,"Aspect Ratio",&map.camera.plane.aspect_ratio,FALSE);
	list_palette_add_int(&map_palette,kMapPropertyCameraNearZ,"Near Z",&map.camera.plane.near_z,FALSE);
	list_palette_add_int(&map_palette,kMapPropertyCameraFarZ,"Far Z",&map.camera.plane.far_z,FALSE);
	list_palette_add_int(&map_palette,kMapPropertyCameraNearZOffset,"Near Z Offset",&map.camera.plane.near_z_offset,FALSE);

		// camera chase

	list_palette_add_header(&map_palette,0,"Map Camera Chase");
	list_palette_add_int(&map_palette,kMapPropertyCameraChaseDistance,"Distance",&map.camera.chase.distance,FALSE);
	list_palette_add_float(&map_palette,kMapPropertyCameraChaseTrackSpeed,"Track Speed",&map.camera.chase.track_speed,FALSE);
	list_palette_add_angle(&map_palette,kMapPropertyCameraChaseTrackAngle,"Track Angle",&map.camera.chase.track_ang,FALSE);
	list_palette_add_angle(&map_palette,kMapPropertyCameraChaseSlop,"Slop",&map.camera.chase.slop,FALSE);

		// camera static

	list_palette_add_header(&map_palette,0,"Map Camera Static");
	list_palette_add_checkbox(&map_palette,kMapPropertyCameraStaticFollow,"Follow Player",&map.camera.c_static.follow,FALSE);
	list_palette_add_picker_list_string(&map_palette,kMapPropertyCameraStaticAttachNode,"Attach Node",(char*)map.nodes,map.nnode,sizeof(node_type),(int)offsetof(node_type,name),TRUE,TRUE,map.camera.c_static.attach_node,FALSE);
}

/* =======================================================

      Property Palette Click Map Camera
      
======================================================= */

void map_palette_click_camera(bool double_click)
{
}

