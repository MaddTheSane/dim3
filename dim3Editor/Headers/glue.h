/****************************** File *********************************

Module: dim3 Common
Author: Brian Barnes
 Usage: Editor/Animator Cross Platform Glue Routines

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

//
// menu definitions
//

#define os_menu_key_none						0
#define os_menu_key_cmd							1
#define os_menu_key_cmd_opt						2
#define os_menu_key_cmd_shift					3

typedef struct {
					char		menu_name[32],item_name[32];
					int			id,key_type;
					char		key;
				} os_menu_item_type;

//
// dialog definitions
//

#define os_dialog_msg_type_init					0
#define os_dialog_msg_type_command				1
#define os_dialog_msg_type_tree_change			2
#define os_dialog_msg_type_tree_double_click	3

#define os_dialog_ctrl_type_button				0
#define os_dialog_ctrl_type_default_button		1
#define os_dialog_ctrl_type_text_left			2
#define os_dialog_ctrl_type_text_right			3
#define os_dialog_ctrl_type_text_edit			4
#define os_dialog_ctrl_type_combo				5
#define os_dialog_ctrl_type_checkbox			6
#define os_dialog_ctrl_type_files				7

typedef struct {
					int			type,id;
					char		str[256];
					int			x,y,wid,high;
				} os_dialog_ctrl_type;

typedef bool (*os_dialog_callback_ptr)(int msg_type,int id);

//
// functions
//

extern void os_glue_start(void);
extern void os_glue_end(void);

extern void os_get_support_file_path(char *path);
extern void os_create_directory(char *path);

extern void os_application_quit(void);
extern void os_get_window_box(d3rect *box);
extern void os_select_window(void);
extern void os_set_title_window(char *title);
extern void os_swap_gl_buffer(void);

extern void os_start_timer(void);
extern void os_end_timer(void);

extern void os_set_arrow_cursor(void);
extern void os_set_wait_cursor(void);
extern void os_set_hand_cursor(void);
extern void os_set_drag_cursor(void);
extern void os_set_resize_cursor(void);
extern void os_set_add_cursor(void);
extern void os_set_subtract_cursor(void);

extern void os_menu_create(os_menu_item_type *os_menus);
extern void os_menu_dispose(void);
extern void os_menu_enable_item(int menu_idx,int item_idx,bool enable);
extern void os_menu_check_item(int menu_idx,int item_idx,bool check);

extern bool os_key_space_down(void);
extern bool os_key_option_down(void);
extern bool os_key_control_down(void);
extern bool os_key_command_down(void);
extern bool os_key_shift_down(void);

extern void os_get_cursor(d3pnt *pnt);
extern bool os_button_down(void);
extern bool os_track_mouse_location(d3pnt *pnt,d3rect *offset_box);

extern void os_dialog_alert(char *title,char *msg);
extern int os_dialog_confirm(char *title,char *msg,bool include_cancel);

extern bool os_load_file(char *title,char *path,char *ext);

extern void os_pick_color(d3col *col);

extern bool os_launch_process(char *path,bool text_editor);

extern bool os_dialog_run(char *title,int wid,int high,os_dialog_ctrl_type *ctrls,void *callback);
extern void os_dialog_close(bool ok);

extern void os_dialog_set_text(int id,char *value);
extern void os_dialog_get_text(int id,char *value,int value_len);
extern void os_dialog_set_int(int id,int i);
extern int os_dialog_get_int(int id);
extern void os_dialog_set_float(int id,float f);
extern float os_dialog_get_float(int id);
extern void os_dialog_set_bool(int id,bool value);
extern bool os_dialog_get_bool(int id);

extern void os_dialog_combo_clear(int id);
extern void os_dialog_combo_add(int id,char *str);
extern void os_dialog_combo_set_value(int id,int value);
extern int os_dialog_combo_get_value(int id);

extern void os_dialog_tree_add(int id,file_path_directory_type *fpd);
extern int os_dialog_tree_get_value(int id);

extern void os_dialog_set_focus(int id,bool select_all);
extern void os_dialog_enable(int id,bool enable);

