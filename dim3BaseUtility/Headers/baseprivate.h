/****************************** File *********************************

Module: dim3 Math Utility
Author: Brian Barnes
 Usage: Private Header

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

/* =======================================================

      XML
      
======================================================= */

//
// Tag Defines
//

#define xml_root_tag				-1
#define xml_tag_chunk_count			(1024*20)
#define xml_buffer_chunk_sz			(1024*100)
#define max_tag_depth				256

//
// Tag Structure
//

typedef struct	{
					int				parent,nchild,first_child,last_child;
					unsigned long	tag_offset,attrib_offset,start_offset,end_offset;
					char			name[64];
				} xmltagtype;

/* =======================================================

      Bitmaps
      
======================================================= */

extern unsigned char* png_utility_read(char *path,int *p_wid,int *p_high,bool *alpha_channel);
extern bool png_utility_write(unsigned char *data,int wid,int high,bool alpha_channel,char *path);
extern bool png_utility_check(char *path,char *err_str);
extern bool png_utility_copy(char *srce_path,char *dest_path);
extern bool bitmap_texture_open(bitmap_type *bitmap,unsigned char *data,bool mipmap,bool compress,bool pixelated,bool npot);
extern void bitmap_texture_close(bitmap_type *bitmap);

/* =======================================================

      Auto-Generated Bitmaps
      
======================================================= */

typedef struct	{
					int						pixel_sz;
					int						clip_lx,clip_rx,clip_ty,clip_by;
					bool					no_bump_spec;
					unsigned char			*png_data,*bump_data,*spec_data,*glow_data;
					d3col					back_col;
					texture_frame_type		*frame;
				} bitmap_ag_type;

extern int bitmap_ag_random_int(int max);
extern bool bitmap_ag_random_bool(void);
extern void bitmap_ag_random_color(d3col *col,int min_r,int min_g,int min_b,int max_r,int max_g,int max_b);
extern void bitmap_ag_random_color_lock(d3col *col,int min,int max);
extern void bitmap_ag_random_color_change(d3col *base_col,d3col *col,float fct);

extern bool bitmap_ag_texture_create(bitmap_ag_type *ag_bitmap,bool has_glow);
extern bool bitmap_ag_texture_finish(bitmap_ag_type *ag_bitmap,char *base_path);
extern void bitmap_ag_texture_make_spec(bitmap_ag_type *ag_bitmap,float spec_fct);

extern void bitmap_ag_set_clip(bitmap_ag_type *ag_bitmap,int x,int y,int wid,int high);
extern void bitmap_ag_clear_clip(bitmap_ag_type *ag_bitmap);

extern void bitmap_ag_texture_read_pixel(bitmap_ag_type *ag_bitmap,unsigned char *data,int x,int y,d3col *col);
extern void bitmap_ag_texture_write_pixel(bitmap_ag_type *ag_bitmap,unsigned char *data,int x,int y,d3col *col);
extern void bitmap_ag_texture_read_normal(bitmap_ag_type *ag_bitmap,unsigned char *data,int x,int y,d3vct *normal);
extern void bitmap_ag_texture_write_normal(bitmap_ag_type *ag_bitmap,unsigned char *data,int x,int y,d3vct *normal);

extern void bitmap_ag_texture_add_noise(bitmap_ag_type *ag_bitmap,int x,int y,int wid,int high,float base_darken,float percentage);
extern void bitmap_ag_texture_add_particle(bitmap_ag_type *ag_bitmap,int x,int y,int wid,int high,float fct,bool flip_normal,int density);
extern void bitmap_ag_texture_add_ridge_horizontal(bitmap_ag_type *ag_bitmap,int x,int y,int wid,int high,float min,float max);

extern void bitmap_ag_texture_gradient_overlay_vertical(bitmap_ag_type *ag_bitmap,int px,int py,int wid,int high,float min,float max);
extern void bitmap_ag_texture_random_color_stripe_vertical(bitmap_ag_type *ag_bitmap,int px,int py,int wid,int high,d3col *base_col,float fct);
extern void bitmap_ag_texture_random_color_stripe_horizontal(bitmap_ag_type *ag_bitmap,int px,int py,int wid,int high,d3col *base_col,float fct);
extern void bitmap_ag_texture_random_color_stripe_slant(bitmap_ag_type *ag_bitmap,int px,int py,int wid,int high,d3col *base_col,float fct);

extern void bitmap_ag_texture_draw_rectangle(bitmap_ag_type *ag_bitmap,int px,int py,int wid,int high,int lip_sz,bool flip_normal,d3col *col,d3col *edge_col);
extern void bitmap_ag_texture_draw_oval(bitmap_ag_type *ag_bitmap,int px,int py,int wid,int high,int border_sz,bool flip_normal,d3col *border_col,d3col *col);
extern void bitmap_ag_texture_draw_line_horizontal(bitmap_ag_type *ag_bitmap,int px,int py,int wid,int lip_sz,bool flip_normal,d3col *col);

/* =======================================================

      Interface
      
======================================================= */

extern void iface_xml_substitution(char *name,char *sub_name);

extern void iface_read_settings_interface(iface_type *iface);
extern void iface_refresh_settings_interface_hud_only(iface_type *iface);
extern void iface_read_settings_chooser(iface_type *iface);
extern void iface_read_settings_singleplayer(iface_type *iface);
extern void iface_read_settings_multiplayer(iface_type *iface);
extern void iface_read_settings_particle(iface_type *iface);
extern void iface_read_settings_ring(iface_type *iface);
extern void iface_read_settings_halo(iface_type *iface);
extern void iface_read_settings_mark(iface_type *iface);
extern void iface_read_settings_label(iface_type *iface);
extern void iface_read_settings_crosshair(iface_type *iface);
extern void iface_read_settings_sound(iface_type *iface);
extern void iface_read_settings_action(iface_type *iface);
extern void iface_read_settings_shader(iface_type *iface);

extern bool iface_write_settings_interface(iface_type *iface,char *err_str);
extern bool iface_write_settings_chooser(iface_type *iface,char *err_str);
extern bool iface_write_settings_singleplayer(iface_type *iface,char *err_str);
extern bool iface_write_settings_multiplayer(iface_type *iface,char *err_str);
extern bool iface_write_settings_particle(iface_type *iface,char *err_str);
extern bool iface_write_settings_ring(iface_type *iface,char *err_str);
extern bool iface_write_settings_halo(iface_type *iface,char *err_str);
extern bool iface_write_settings_mark(iface_type *iface,char *err_str);
extern bool iface_write_settings_label(iface_type *iface,char *err_str);
extern bool iface_write_settings_crosshair(iface_type *iface,char *err_str);
extern bool iface_write_settings_sound(iface_type *iface,char *err_str);
extern bool iface_write_settings_action(iface_type *iface,char *err_str);
extern bool iface_write_settings_shader(iface_type *iface,char *err_str);

