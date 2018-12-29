/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script Global Data

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

#include "scripts.h"

extern js_type			js;

/* =======================================================

      Start Globals
      
======================================================= */

void script_global_initialize_list(void)
{
	int				n;

	for (n=0;n!=max_global_list;n++) {
		js.global_list.globals[n]=NULL;
	}
}

void script_global_free_list(void)
{
	int				n;

	for (n=0;n!=max_global_list;n++) {
		if (js.global_list.globals[n]!=NULL) free(js.global_list.globals[n]);
	}
}

int script_global_count_list(void)
{
	int				n,count;

	count=0;

	for (n=0;n!=max_global_list;n++) {
		if (js.global_list.globals[n]!=NULL) count++;
	}

	return(count);
}

/* =======================================================

      Find Globals
      
======================================================= */

int script_find_global(char *name)
{
	int				n;
	global_type		*global;
	
	for (n=0;n!=max_global_list;n++) {
		global=js.global_list.globals[n];
		if (global==NULL) continue;

		if (strcasecmp(global->name,name)==0) return(n);
	}
	
	return(-1);
}

/* =======================================================

      Set and Get Globals
      
======================================================= */

void script_set_global_by_index(JSContextRef cx,int idx,JSValueRef val)
{
	global_type		*global;
	
	global=js.global_list.globals[idx];
	
	if (JSValueIsNumber(cx,val)) {
		global->type=d3_jsval_type_number;
		global->data.d3_number=script_value_to_float(cx,val);
		return;
	}
	
	if (JSValueIsBoolean(cx,val)) {
		global->type=d3_jsval_type_boolean;
		global->data.d3_boolean=script_value_to_bool(cx,val);
		return;
	}
	
	global->type=d3_jsval_type_string;
	script_value_to_string(cx,val,global->data.d3_string,max_d3_jsval_str_len);
}

bool script_set_global(JSContextRef cx,char *name,JSValueRef val)
{
	int				idx;
	
	idx=script_find_global(name);
	if (idx==-1) return(FALSE);
	
	script_set_global_by_index(cx,idx,val);
	return(TRUE);
}

JSValueRef script_get_global(JSContextRef cx,char *name)
{
	int				idx;
	global_type		*global;
	
	idx=script_find_global(name);
	if (idx==-1) return(script_null_to_value(cx));
	
	global=js.global_list.globals[idx];
	
	switch (global->type) {
		case d3_jsval_type_number:
			return(script_float_to_value(cx,global->data.d3_number));
		case d3_jsval_type_boolean:
			return(script_bool_to_value(cx,global->data.d3_boolean));
		case d3_jsval_type_string:
			return(script_string_to_value(cx,global->data.d3_string));
	}
	
	return(script_null_to_value(cx));
}

/* =======================================================

      Add and Delete Globals
      
======================================================= */

bool script_add_global(JSContextRef cx,char *name,JSValueRef val)
{
	int				n,idx;
	global_type		*global;
	
		// does it already exist?
		
	idx=script_find_global(name);
	if (idx!=-1) {
		script_set_global_by_index(cx,idx,val);
		return(TRUE);
	}
	
		// find free global

	idx=-1;

	for (n=0;n!=max_global_list;n++) {
		global=js.global_list.globals[n];
		if (global==NULL) {
			idx=n;
			break;
		}
	}

	if (idx==-1) return(FALSE);

		// create it

	js.global_list.globals[idx]=(global_type*)malloc(sizeof(global_type));
	if (js.global_list.globals[idx]==NULL) return(FALSE);

		// add it
	
	global=js.global_list.globals[idx];

	strcpy(global->name,name);
	
	script_set_global_by_index(cx,idx,val);
	
	return(TRUE);
}

void script_delete_global(char *name)
{
	int				idx;
	
	idx=script_find_global(name);
	if (idx==-1) return;

	if (js.global_list.globals[idx]!=NULL) {
		free(js.global_list.globals[idx]);
		js.global_list.globals[idx]=NULL;
	}
}

