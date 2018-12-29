/****************************** File *********************************

Module: dim3 Common
Author: Brian Barnes
 Usage: win32 Glue Routines

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

#include "dim3editor.h"

#include "glue.h"

extern bool				quit;

extern HINSTANCE		hinst;
extern HWND				wnd;
extern HDC				wnd_gl_dc;
extern HMENU			wnd_menu;
extern HACCEL			wnd_accel;

HCURSOR					cur_arrow,cur_wait,cur_hand,cur_drag,cur_resize,
						cur_add,cur_subtract;
COLORREF				custom_colors[16];
UINT_PTR				os_wnd_timer;
bool					os_dialog_focus_flag,os_dialog_ok;
HWND					os_dialog_wind;
HFONT					os_dialog_font;
os_dialog_ctrl_type		*os_dialog_ctrls;
os_dialog_callback_ptr	os_dialog_callback;

extern void main_wind_timer(void);

/* =======================================================

      Start/Stop Glue
      
======================================================= */

void os_glue_start(void)
{
	int				n;

		// initial com for shell

	CoInitializeEx(NULL,(COINIT_APARTMENTTHREADED|COINIT_DISABLE_OLE1DDE));

		// load cursors

	cur_arrow=LoadCursor(NULL,IDC_ARROW);
	cur_wait=LoadCursor(NULL,IDC_WAIT);
	cur_hand=LoadCursor(NULL,IDC_HAND);
	cur_drag=LoadCursor(NULL,IDC_HAND);
	cur_resize=LoadCursor(NULL,IDC_SIZEALL);
	cur_add=LoadCursor(NULL,IDC_CROSS);
	cur_subtract=LoadCursor(NULL,IDC_NO);

		// custom colors

	for (n=0;n!=16;n++) {
		custom_colors[n]=RGB(0,0,0);
	}
}

void os_glue_end(void)
{
		// destroy cursors

	DestroyCursor(cur_arrow);
	DestroyCursor(cur_wait);
	DestroyCursor(cur_hand);
	DestroyCursor(cur_drag);
	DestroyCursor(cur_resize);
	DestroyCursor(cur_add);
	DestroyCursor(cur_subtract);

		// unload com

	CoUninitialize();
}

/* =======================================================

      Files
      
======================================================= */
		
void os_get_support_file_path(char *path)
{
	strcpy(path,"dim3 App Support\\");
}

void os_create_directory(char *path)
{
	CreateDirectory(path,NULL);
}

/* =======================================================

      Windows
      
======================================================= */

void os_application_quit(void)
{
	quit=TRUE;
}

void os_get_window_box(d3rect *box)
{
	RECT			wbox;

	GetClientRect(wnd,&wbox);

	box->lx=wbox.left;
	box->rx=wbox.right;
	box->ty=wbox.top;
	box->by=wbox.bottom;
}

void os_select_window(void)
{
    ShowWindow(wnd,SW_SHOW);
}

void os_set_title_window(char *title)
{
	SetWindowText(wnd,title);
}

void os_swap_gl_buffer(void)
{
	SwapBuffers(wnd_gl_dc);
}

/* =======================================================

      Timers
      
======================================================= */

void CALLBACK os_timer_proc(HWND hwnd,UINT msg,UINT_PTR id,DWORD tick)
{
	main_wind_timer();
}

void os_start_timer(void)
{
	os_wnd_timer=SetTimer(wnd,1,10,os_timer_proc);
}

void os_end_timer(void)
{
	KillTimer(wnd,1);
}

/* =======================================================

      Cursors
      
======================================================= */

void os_set_arrow_cursor(void)
{
	SetCursor(cur_arrow);
}

void os_set_wait_cursor(void)
{
	SetCursor(cur_wait);
}

void os_set_hand_cursor(void)
{
	SetCursor(cur_hand);
}

void os_set_drag_cursor(void)
{
	SetCursor(cur_drag);
}

void os_set_resize_cursor(void)
{
	SetCursor(cur_resize);
}

void os_set_add_cursor(void)
{
	SetCursor(cur_add);
}

void os_set_subtract_cursor(void)
{
	SetCursor(cur_subtract);
}

/* =======================================================

      Menus
      
======================================================= */

void os_menu_create(os_menu_item_type *os_menus)
{
	int						len,accel_count;
	char					last_menu_name[32],item_name[64];
	os_menu_item_type		*menu;
	HMENU					sub_menu;
	ACCEL					accels[64];

		// create the menu

	wnd_menu=CreateMenu();

	menu=os_menus;

	last_menu_name[0]=0x0;
	sub_menu=NULL;

	while (TRUE) {
		if (menu->menu_name[0]==0x0) break;

			// switching to a new menu

		if (strcmp(menu->menu_name,last_menu_name)!=0) {
			if (sub_menu!=NULL) AppendMenu(wnd_menu,(MF_STRING|MF_POPUP),(UINT)sub_menu,last_menu_name);
			strcpy(last_menu_name,menu->menu_name);
			sub_menu=CreatePopupMenu();
		}

			// add item

		if (menu->item_name[0]!=0x0) {

				// add in ctrl/alt/shift

			strcpy(item_name,menu->item_name);

			switch (menu->key_type) {

				case os_menu_key_cmd:
					strcat(item_name,"\tCtrl+");
					len=strlen(item_name);
					item_name[len]=menu->key;
					item_name[len+1]=0x0;
					break;

				case os_menu_key_cmd_opt:
					strcat(item_name,"\tCtrl+Alt+");
					len=strlen(item_name);
					item_name[len]=menu->key;
					item_name[len+1]=0x0;
					break;

				case os_menu_key_cmd_shift:
					strcat(item_name,"\tCtrl+Shift+");
					len=strlen(item_name);
					item_name[len]=menu->key;
					item_name[len+1]=0x0;
					break;

			}

			AppendMenu(sub_menu,MF_STRING,menu->id,item_name);
		}
		else {
			AppendMenu(sub_menu,MF_SEPARATOR,0,"");
		}

			// next menu item

		menu++;
	}

	if (sub_menu!=NULL) AppendMenu(wnd_menu,(MF_STRING|MF_POPUP),(UINT)sub_menu,last_menu_name);

		// set the menu

	SetMenu(wnd,wnd_menu);

		// create the accelerators

	accel_count=0;
	menu=os_menus;

	while (TRUE) {
		if (menu->menu_name[0]==0x0) break;

		switch (menu->key_type) {

			case os_menu_key_cmd:
				accels[accel_count].fVirt=FVIRTKEY|FCONTROL;
				accels[accel_count].key=menu->key;
				accels[accel_count].cmd=menu->id;
				accel_count++;
				break;

			case os_menu_key_cmd_opt:
				accels[accel_count].fVirt=FVIRTKEY|FCONTROL|FALT;
				accels[accel_count].key=menu->key;
				accels[accel_count].cmd=menu->id;
				accel_count++;
				break;

			case os_menu_key_cmd_shift:
				accels[accel_count].fVirt=FVIRTKEY|FCONTROL|FSHIFT;
				accels[accel_count].key=menu->key;
				accels[accel_count].cmd=menu->id;
				accel_count++;
				break;

		}

		menu++;
	}

	wnd_accel=CreateAcceleratorTable(accels,accel_count);
}

void os_menu_dispose(void)
{
	DestroyAcceleratorTable(wnd_accel);
	DestroyMenu(wnd_menu);
}

void os_menu_enable_item(int menu_idx,int item_idx,bool enable)
{
	HMENU			menu;

		// enabling entire menu

	if (item_idx==0) {
		if (enable) {
			EnableMenuItem(GetMenu(wnd),menu_idx,MF_BYPOSITION|MF_ENABLED);
		}
		else {
			EnableMenuItem(GetMenu(wnd),menu_idx,MF_BYPOSITION|MF_GRAYED);
		}
		DrawMenuBar(wnd);
		return;
	}

		// enabling single item

	menu=GetSubMenu(GetMenu(wnd),menu_idx);

	if (enable) {
		EnableMenuItem(menu,(item_idx-1),MF_BYPOSITION|MF_ENABLED);
	}
	else {
		EnableMenuItem(menu,(item_idx-1),MF_BYPOSITION|MF_GRAYED);
	}
}

void os_menu_check_item(int menu_idx,int item_idx,bool check)
{
	HMENU			menu;

	menu=GetSubMenu(GetMenu(wnd),menu_idx);

	if (check) {
		CheckMenuItem(menu,(item_idx-1),MF_BYPOSITION|MF_CHECKED);
	}
	else {
		CheckMenuItem(menu,(item_idx-1),MF_BYPOSITION|MF_UNCHECKED);
	}
}

/* =======================================================

      Keyboard
      
======================================================= */

bool os_key_space_down(void)
{
	return((GetAsyncKeyState(VK_SPACE)&0x8000)!=0x0);
}

bool os_key_option_down(void)
{
	return(((GetAsyncKeyState(VK_LWIN)&0x8000)!=0x0)||((GetAsyncKeyState(VK_RWIN)&0x8000)!=0x0));
}

bool os_key_control_down(void)
{
	return((GetAsyncKeyState(VK_CONTROL)&0x8000)!=0x0);
}

bool os_key_command_down(void)
{
	return((GetAsyncKeyState(VK_MENU)&0x8000)!=0x0);
}

bool os_key_shift_down(void)
{
	return(((GetAsyncKeyState(VK_SHIFT)&0x8000)!=0x0)||((GetAsyncKeyState(VK_CAPITAL)&0x8000)!=0x0));
}

/* =======================================================

      Mouse
      
======================================================= */

void os_get_cursor(d3pnt *pnt)
{
	POINT			ui_pt;

	GetCursorPos(&ui_pt);
	ScreenToClient(wnd,&ui_pt);
	
	pnt->x=ui_pt.x;
	pnt->y=ui_pt.y;
}

bool os_button_down(void)
{
	if (!GetSystemMetrics(SM_SWAPBUTTON)) return(GetAsyncKeyState(VK_LBUTTON)!=0);
	return(GetAsyncKeyState(VK_RBUTTON)!=0);
}

bool os_track_mouse_location(d3pnt *pnt,d3rect *offset_box)
{
	POINT			ui_pt;

		// get cursor pos in window

	GetCursorPos(&ui_pt);
	ScreenToClient(wnd,&ui_pt);

		// add in offsets
	
	if (offset_box==NULL) {
		pnt->x=ui_pt.x;
		pnt->y=ui_pt.y;
	}
	else {
		pnt->x=ui_pt.x-offset_box->lx;
		pnt->y=ui_pt.y-offset_box->ty;
	}
	
	return(!os_button_down());
}

/* =======================================================

      Dialog Alerts and Messages
      
======================================================= */

void os_dialog_alert(char *title,char *msg)
{
	MessageBox(NULL,msg,title,MB_ICONINFORMATION|MB_OK);
}

int os_dialog_confirm(char *title,char *msg,bool include_cancel)
{
	int			rtn;
	
	if (include_cancel) {
		rtn=MessageBox(NULL,msg,title,MB_ICONINFORMATION|MB_YESNOCANCEL);
	}
	else {
		rtn=MessageBox(NULL,msg,title,MB_ICONINFORMATION|MB_YESNO);
	}

	if (rtn==IDYES) return(0);
	if (rtn==IDNO) return(2);

	return(1);
}

/* =======================================================

      Open Files
      
======================================================= */

bool os_load_file(char *title,char *path,char *ext)
{
	char			filter[256],file_title[256],ext2[8],
					initial_dir[1024],*c;
	OPENFILENAME	ofn;

	GetModuleFileName(NULL,initial_dir,1024);

	if (initial_dir[0]=='\"') {				// parse out the quotes
		strcpy(initial_dir,(char*)&initial_dir[1]);
		c=strchr(initial_dir,'\"');
		if (c!=NULL) *c=0x0;
	}

	c=strrchr(initial_dir,'\\');
	if (c!=0x0) *(c+1)=0x0;

		// setup call

	ext2[0]=toupper(ext[0]);
	ext2[1]=toupper(ext[1]);
	ext2[2]=toupper(ext[2]);
	ext2[3]=0x0;

	sprintf(filter,"%s Files\0*.%s\0\0",ext2,ext);

	file_title[0]=0x0;
	sprintf(path,"*.%s",ext);

		// get file

	ofn.lStructSize=sizeof(OPENFILENAME);
	ofn.hwndOwner=wnd;
	ofn.hInstance=hinst;
	ofn.lpstrFilter=filter;
	ofn.lpstrCustomFilter=NULL;
	ofn.nMaxCustFilter=0;
	ofn.nFilterIndex=1;
	ofn.lpstrFile=path;
	ofn.nMaxFile=1024;
	ofn.lpstrFileTitle=file_title;
	ofn.nMaxFileTitle=256;
	ofn.lpstrInitialDir=initial_dir;
	ofn.lpstrTitle=title;
	ofn.Flags=OFN_HIDEREADONLY|OFN_LONGNAMES|OFN_OVERWRITEPROMPT|OFN_PATHMUSTEXIST;
	ofn.nFileOffset=0;
	ofn.nFileExtension=0;
	ofn.lpstrDefExt=ext;
	ofn.lCustData=(LPARAM)NULL;
	ofn.lpfnHook=NULL;
	ofn.lpTemplateName=NULL;

	if (!GetOpenFileName(&ofn)) return(FALSE);

		// check for proper extension

	c=strrchr(path,'.');
	if (c!=NULL) {
		if (strcasecmp((c+1),ext)!=0) {
			MessageBox(wnd,"Please pick a file of the correct type.","Wrong File Type",MB_OK);
			return(FALSE);
		}
	}

	return(TRUE);
}

/* =======================================================

      Color Picker
      
======================================================= */

void os_pick_color(d3col *col)
{
	CHOOSECOLOR			cc;

	bzero(&cc,sizeof(CHOOSECOLOR));
	cc.lStructSize=sizeof(CHOOSECOLOR);
	cc.hwndOwner=wnd;
	cc.lpCustColors=(LPDWORD)custom_colors;
	cc.rgbResult=RGB((int)(col->r*255.0f),(int)(col->g*255.0f),(int)(col->b*255.0f));
	cc.Flags=CC_ANYCOLOR|CC_FULLOPEN|CC_SOLIDCOLOR|CC_RGBINIT;

	if (ChooseColor(&cc)) {
		col->r=((float)GetRValue(cc.rgbResult))/255.0f;
		col->g=((float)GetGValue(cc.rgbResult))/255.0f;
		col->b=((float)GetBValue(cc.rgbResult))/255.0f;
	}
}

/* =======================================================

      Launch Process
      
======================================================= */

bool os_launch_process(char *path,bool text_editor)
{
	int			n,len,err;
	char		*c,path2[1024];

		// we need to flip any / to \

	len=strlen(path);
	c=path;

	for (n=0;n!=len;n++) {
		if (*c=='/') *c='\\';
		c++;
	}

		// text edit or run link

	if (text_editor) {
		sprintf(path2,"\"%s\"",path);
		err=(int)ShellExecute(wnd,"open","wordpad.exe",path2,NULL,SW_SHOW);
	}
	else {
		err=(int)ShellExecute(wnd,"open",path,NULL,NULL,SW_SHOW);
	}

	return(err>32);
}

/* =======================================================

      Dialogs
      
======================================================= */

LRESULT CALLBACK os_dialog_proc(HWND diag,UINT msg,WPARAM wparam,LPARAM lparam)
{
	LPNMHDR				hdr;

	switch (msg) {

		case WM_INITDIALOG:

				// remember handle so dialogend works

			os_dialog_wind=diag;

				// send init message back
				// we need to check if focus was set

			os_dialog_focus_flag=FALSE;
			(*os_dialog_callback)(os_dialog_msg_type_init,0);

			return(!os_dialog_focus_flag);

		case WM_COMMAND:
			return((*os_dialog_callback)(os_dialog_msg_type_command,(int)LOWORD(wparam)));

		case WM_NOTIFY:
			hdr=(LPNMHDR)lparam;
			if (hdr->code==TVN_SELCHANGED) return((*os_dialog_callback)(os_dialog_msg_type_tree_change,hdr->idFrom));
			if (hdr->code==NM_DBLCLK) return((*os_dialog_callback)(os_dialog_msg_type_tree_double_click,hdr->idFrom));
			break;

	}

	return(FALSE);
}

int os_dialog_unit_convert_x(int x)
{
	return((int)(((float)x)*0.68f));
}

int os_dialog_unit_convert_y(int y)
{
	return((int)(((float)y)*0.66f));
}

int os_dialog_count_controls(os_dialog_ctrl_type *ctrls)
{
	int					count;
	os_dialog_ctrl_type	*ctrl;

	count=0;
	ctrl=ctrls;

	while (ctrl->id!=-1) {
		count++;
		ctrl++;
	}

	return(count);
}

bool os_dialog_run(char *title,int wid,int high,os_dialog_ctrl_type *ctrls,void *callback)
{
	int					font_high,style,ex_style,atom,sz,pad,
						x,y,c_wid,c_high;
	unsigned char		*ptr,*diag_template;
	char				atom_name[32];
	WORD				*w_ptr;
	DWORD				*d_ptr;
	short				*s_ptr;
	os_dialog_ctrl_type	*ctrl;

		// setup callback

	os_dialog_callback=callback;

		// build the dialog template
		// in memory

	diag_template=malloc(10240);
	memset(diag_template,0x0,10240);

	ptr=diag_template;

		// dialog

	w_ptr=(WORD*)ptr;
	*w_ptr++=1;
	*w_ptr++=0xFFFF;

	d_ptr=(DWORD*)w_ptr;
	*d_ptr++=0;
	*d_ptr++=0;
	*d_ptr++=DS_SETFONT|DS_MODALFRAME|DS_FIXEDSYS|DS_CENTER|WS_POPUP|WS_CAPTION;

	w_ptr=(WORD*)d_ptr;
	*w_ptr++=os_dialog_count_controls(ctrls);

	s_ptr=(short*)w_ptr;
	*s_ptr++=0;
	*s_ptr++=0;
	*s_ptr++=os_dialog_unit_convert_x(wid);
	*s_ptr++=os_dialog_unit_convert_y(high);

	w_ptr=(WORD*)s_ptr;
	*w_ptr++=0;
	*w_ptr++=0;

	ptr=(unsigned char*)w_ptr;

	sz=MultiByteToWideChar(CP_ACP,0,title,-1,(LPWSTR)ptr,256);
	ptr+=(sz*sizeof(wchar_t));

	w_ptr=(WORD*)ptr;
	*w_ptr++=8;
	*w_ptr++=400;

	ptr=(unsigned char*)w_ptr;
	*ptr++=0x0;
	*ptr++=0x1;

	sz=MultiByteToWideChar(CP_ACP,0,"MS Shell Dlg",-1,(LPWSTR)ptr,256);
	ptr+=(sz*sizeof(wchar_t));

		// remember controls
		// so we can set font later

	os_dialog_ctrls=ctrls;

		// controls

	ctrl=ctrls;

	while (ctrl->id!=-1) {

		atom_name[0]=0x0;
		atom=0x82;
		ex_style=0;

		x=ctrl->x;
		y=ctrl->y;
		c_wid=ctrl->wid;
		c_high=ctrl->high;

		switch (ctrl->type) {

			case os_dialog_ctrl_type_button:
				style=BS_PUSHBUTTON|WS_CHILD|WS_VISIBLE|WS_TABSTOP;
				atom=0x80;
				break;

			case os_dialog_ctrl_type_default_button:
				style=BS_DEFPUSHBUTTON|WS_CHILD|WS_VISIBLE|WS_TABSTOP;
				atom=0x80;
				break;

			case os_dialog_ctrl_type_text_left:
				style=SS_LEFT|WS_CHILD|WS_VISIBLE;
				atom=0x82;
				y+=3;
				break;

			case os_dialog_ctrl_type_text_right:
				style=SS_RIGHT|WS_CHILD|WS_VISIBLE;
				atom=0x82;
				y+=3;
				break;

			case os_dialog_ctrl_type_text_edit:
				style=WS_CHILD|WS_VISIBLE|WS_TABSTOP|ES_LEFT|ES_AUTOHSCROLL;
				ex_style=WS_EX_CLIENTEDGE;
				atom=0x81;
				break;

			case os_dialog_ctrl_type_combo:
				style=WS_CHILD|WS_VISIBLE|WS_VSCROLL|WS_TABSTOP|CBS_DROPDOWNLIST;
				ex_style=WS_EX_CLIENTEDGE;
				atom=0x85;
				break;

			case os_dialog_ctrl_type_checkbox:
				style=WS_CHILD|WS_VISIBLE|WS_TABSTOP|BS_AUTOCHECKBOX;
				ex_style=0;
				atom=0x0;
				strcpy(atom_name,"Button");
				break;

			case os_dialog_ctrl_type_files:
				style=WS_CHILD|WS_VISIBLE|WS_TABSTOP|TVS_HASBUTTONS|TVS_HASLINES;
				ex_style=WS_EX_CLIENTEDGE;
				atom=0x0;
				strcpy(atom_name,"SysTreeView32");
				break;

		}

			// must be on DWORD boundry

		pad=(((int)(ptr-diag_template))%sizeof(DWORD));
		if (pad!=0) ptr+=(sizeof(DWORD)-pad);

		d_ptr=(DWORD*)ptr;
		*d_ptr++=0;
		*d_ptr++=ex_style;
		*d_ptr++=style;

		s_ptr=(short*)d_ptr;
		*s_ptr++=os_dialog_unit_convert_x(x);
		*s_ptr++=os_dialog_unit_convert_y(y);
		*s_ptr++=os_dialog_unit_convert_x(c_wid);
		*s_ptr++=os_dialog_unit_convert_y(c_high);

		d_ptr=(DWORD*)s_ptr;
		*d_ptr++=ctrl->id;

		ptr=(unsigned char*)d_ptr;

		if (atom_name[0]==0x0) {
			w_ptr=(WORD*)ptr;
			*w_ptr++=0xFFFF;
			*w_ptr++=atom;

			ptr=(unsigned char*)w_ptr;
		}
		else {
			sz=MultiByteToWideChar(CP_ACP,0,atom_name,-1,(LPWSTR)ptr,256);
			ptr+=(sz*sizeof(wchar_t));
		}

		if (ctrl->str[0]==0x0) {
			w_ptr=(WORD*)ptr;
			*w_ptr++=0x0;

			ptr=(unsigned char*)w_ptr;
		}
		else {
			sz=MultiByteToWideChar(CP_ACP,0,ctrl->str,-1,(LPWSTR)ptr,256);
			ptr+=(sz*sizeof(wchar_t));
		}

		w_ptr=(WORD*)ptr;
		*w_ptr++=0x0;
		
		ptr=(unsigned char*)w_ptr;

		ctrl++;
	}

		// window font

	font_high=-MulDiv(8,GetDeviceCaps(GetDC(wnd),LOGPIXELSY),72);
	os_dialog_font=CreateFont(font_high,0,0,0,400,0,0,0,0x1,0,0,0,0,"MS Shell Dlg");

		// run dialog

	os_dialog_ok=FALSE;

	DialogBoxIndirect(hinst,(DLGTEMPLATE*)diag_template,wnd,os_dialog_proc);

		// clean up

	DeleteObject(os_dialog_font);

	free(diag_template);

	return(os_dialog_ok);
}

void os_dialog_close(bool ok)
{
	os_dialog_ok=ok;
	EndDialog(os_dialog_wind,0);
}

void os_dialog_set_text(int id,char *value)
{
	SendDlgItemMessage(os_dialog_wind,id,WM_SETTEXT,0,(LPARAM)value);
}

void os_dialog_get_text(int id,char *value,int value_len)
{
	char			str[256];

	SendDlgItemMessage(os_dialog_wind,id,WM_GETTEXT,256,(LPARAM)str);

	strncpy(value,str,value_len);
	value[value_len-1]=0x0;
}

void os_dialog_set_int(int id,int i)
{
	char			str[256];

	sprintf(str,"%d",i);
	os_dialog_set_text(id,str);
}

int os_dialog_get_int(int id)
{
	char			str[256];

	os_dialog_get_text(id,str,256);
	return(atoi(str));
}

extern void os_dialog_set_float(int id,float f)
{
	char			str[256];

	sprintf(str,"%.2f",f);
	os_dialog_set_text(id,str);
}

float os_dialog_get_float(int id)
{
	char			str[256];

	os_dialog_get_text(id,str,256);
	return((float)atof(str));
}

void os_dialog_set_bool(int id,boolean value)
{
	SendDlgItemMessage(os_dialog_wind,id,BM_SETCHECK,(value?BST_CHECKED:BST_UNCHECKED),0);
}

boolean os_dialog_get_bool(int id)
{
	return(SendDlgItemMessage(os_dialog_wind,id,BM_GETCHECK,0,0)==BST_CHECKED);
}

void os_dialog_combo_clear(int id)
{
	SendDlgItemMessage(os_dialog_wind,id,CB_RESETCONTENT,0,0);
}

void os_dialog_combo_add(int id,char *str)
{
	SendDlgItemMessage(os_dialog_wind,id,CB_ADDSTRING,0,(LPARAM)str);
}

void os_dialog_combo_set_value(int id,int value)
{
	SendDlgItemMessage(os_dialog_wind,id,CB_SETCURSEL,value,0);
}

int os_dialog_combo_get_value(int id)
{
	return(SendDlgItemMessage(os_dialog_wind,id,CB_GETCURSEL,0,0));
}

HTREEITEM os_dialog_tree_add_recursive(int id,file_path_directory_type *fpd,HTREEITEM parent_item,int parent_idx)
{
	int					n;
	TV_INSERTSTRUCT		tv_item;
	HTREEITEM			item,first_item;

	first_item=NULL;

	for (n=0;n!=fpd->nfile;n++) {
		if (fpd->files[n].parent_idx==parent_idx) {
			tv_item.hParent=parent_item;
			tv_item.hInsertAfter=TVI_LAST;
			tv_item.item.mask=TVIF_TEXT|TVIF_PARAM;
			tv_item.item.pszText=fpd->files[n].file_name;
			tv_item.item.lParam=(fpd->files[n].is_dir?0xFFFF0000:0x0)|(n&0xFFFF);
			item=(HTREEITEM)SendMessage(GetDlgItem(os_dialog_wind,id),TVM_INSERTITEM,0,(LPARAM)&tv_item);

			if (first_item==NULL) first_item=item;
			os_dialog_tree_add_recursive(id,fpd,item,n);
		}
	}

	return(first_item);
}

void os_dialog_tree_add(int id,file_path_directory_type *fpd)
{
	HTREEITEM			first_item;

	first_item=os_dialog_tree_add_recursive(id,fpd,NULL,-1);
	if (first_item!=NULL) SendMessage(GetDlgItem(os_dialog_wind,id),TVM_EXPAND,TVE_EXPAND,(LPARAM)first_item);
}

int os_dialog_tree_get_value(int id)
{
	HTREEITEM			item;
	TV_ITEM				info;

	item=(HTREEITEM)SendMessage(GetDlgItem(os_dialog_wind,id),TVM_GETNEXTITEM,TVGN_CARET,(LPARAM)0);
	if (item==NULL) return(-1);

	info.mask=TVIF_PARAM;
	info.hItem=item;

	SendMessage(GetDlgItem(os_dialog_wind,id),TVM_GETITEM,0,(LPARAM)&info);

	return(info.lParam);
}

void os_dialog_set_focus(int id,bool select_all)
{
	os_dialog_focus_flag=TRUE;

	SetFocus(GetDlgItem(os_dialog_wind,id));
	if (select_all) SendMessage(GetDlgItem(os_dialog_wind,id),EM_SETSEL,0,-1);
}

void os_dialog_enable(int id,bool enable)
{
	EnableWindow(GetDlgItem(os_dialog_wind,id),enable);
}

