/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Map Multiplayer Routines

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

#include "objects.h"

extern map_type				map;
extern server_type			server;
extern network_setup_type	net_setup;

/* =======================================================

      Multiplayer Mesh Setup
      
======================================================= */

void map_multiplayer_show_hide_meshes(void)
{
	int					n;
	map_mesh_type		*mesh;

		// turn off any meshes based
		// on network settings

	mesh=map.mesh.meshes;

	for (n=0;n!=map.mesh.nmesh;n++) {

		switch (mesh->hide_mode) {

			case mesh_hide_mode_single_player:
				mesh->flag.on=(net_setup.mode!=net_mode_none);
				break;

			case mesh_hide_mode_multiplayer:
				mesh->flag.on=(net_setup.mode==net_mode_none);
				break;

		}

		mesh++;
	}
}


