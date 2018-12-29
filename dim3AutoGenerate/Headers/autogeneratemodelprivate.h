/****************************** File *********************************

Module: dim3 Auto Generator
Author: Brian Barnes
 Usage: Auto Generator Private Model Defines

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

(c) 2000-2014 Klink! Software www.klinksoftware.com
 
*********************************************************************/

//
// bones
//

extern bool ag_model_bone_is_special(model_type *model,int bone_idx);
extern bool ag_model_bone_is_body(model_type *model,int bone_idx);
extern bool ag_model_bone_is_hip(model_type *model,int bone_idx);
extern bool ag_model_bone_is_connected_to_hips(model_type *model,int bone_idx);
extern bool ag_model_bone_is_shallow_y(model_type *model,int bone_idx);
extern bool ag_model_bone_is_head(model_type *model,int bone_idx);
extern bool ag_model_bone_is_arm(model_type *model,int bone_idx);
extern bool ag_model_bone_is_hand(model_type *model,int bone_idx);
extern bool ag_model_bone_is_leg(model_type *model,int bone_idx);
extern bool ag_model_bone_is_foot(model_type *model,int bone_idx);
extern bool ag_model_bone_is_decoration_ok(model_type *model,int bone_idx);
extern void ag_model_bone_create_skeleton(model_type *model);

//
// pieces
//

extern void ag_model_piece_setup(void);
extern void ag_model_piece_complex_cylinder(model_type *model,model_mesh_type *mesh,int p_major_bone_idx,int p_minor_bone_idx,d3pnt *spt,d3pnt *ept,int stack_count,d3pnt *radius,d3pnt *radius_extra,float gx_offset,float gy_offset,float cap_gx_offset,float cap_gy_offset,bool force_end_size,bool no_end_cap);
extern void ag_model_piece_bone_head(model_type *model,model_mesh_type *mesh,int bone_idx,int limb_radius);
extern void ag_model_piece_bone_body(model_type *model,model_mesh_type *mesh,int body_bone_idx,int hip_bone_idx,int limb_radius);
extern void ag_model_piece_bone_hand(model_type *model,model_mesh_type *mesh,int bone_idx,int limb_radius);
extern void ag_model_piece_bone_foot(model_type *model,model_mesh_type *mesh,int bone_idx,int limb_radius);

//
// decorations
//

void ag_model_add_decorations(model_type *model);
