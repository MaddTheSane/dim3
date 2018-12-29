/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script Engine Main Code

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

#include "interface.h"
#include "scripts.h"

extern bool					scripts_event_lock;

extern js_type				js;
extern setup_type			setup;

/* =======================================================

      JavaScript Initialize/Shutdown
      
======================================================= */

bool scripts_engine_initialize(char *err_str)
{
		// initialize classes

	script_initialize_classes();

		// load up the define file
		
	script_load_user_defines();

		// create a group for contexts
		// so they can share objects

	js.cx_group=JSContextGroupCreate();

		// current events are not locked out

	scripts_event_lock=FALSE;
	
	return(TRUE);
}

void scripts_engine_shutdown(void)
{
	JSContextGroupRelease(js.cx_group);

	script_free_user_defines();
	script_release_classes();
}

/* =======================================================

      Initialize Scripts
      
======================================================= */

void scripts_initialize_list(void)
{
	int			n;
	
	for (n=0;n!=max_script_list;n++) {
		js.script_list.scripts[n]=NULL;
	}
}

void scripts_free_list(void)
{
	int			n;
	
	for (n=0;n!=max_script_list;n++) {
		if (js.script_list.scripts[n]!=NULL) free(js.script_list.scripts[n]);
	}
}

/* =======================================================

      Script Clear Attach Data
      
======================================================= */

void scripts_setup_data(script_type *script,int thing_type,int obj_idx,int weap_idx,int proj_setup_idx)
{
	int				n;

		// clear attached event list

	for (n=0;n!=event_main_id_count;n++) {
		script->event_attach_list.func[n]=NULL;
	}

		// setup recursion checks

	script->recursive.count=0;

	for (n=0;n!=event_main_id_count;n++) {
		script->recursive.in_event[n]=FALSE;
	}

		// attachment

	script->attach.thing_type=thing_type;
	script->attach.obj_idx=obj_idx;
	script->attach.weap_idx=weap_idx;
	script->attach.proj_setup_idx=proj_setup_idx;
	script->attach.proj_idx=-1;

		// event state
		
	script->event_state.main_event=-1;
	script->event_state.sub_event=-1;
	script->event_state.id=0;
	script->event_state.tick=0;
}

/* =======================================================

      Compile and Execute Scripts
      
======================================================= */

bool scripts_execute(script_type *script,char *err_str)
{
	JSStringRef		j_script_data,j_script_name;
	JSValueRef		rval,exception;
	
		// make sure UTF8 conversion didn't go crazy
		// on some hidden characters
	
	j_script_data=JSStringCreateWithUTF8CString(script->data);
	if (JSStringGetLength(j_script_data)!=strlen(script->data)) {
		JSStringRelease(j_script_data);
		sprintf(err_str,"[%s] contains extraneous control characters",script->name);
		return(FALSE);
	}

		// evaulate script
		
	j_script_name=JSStringCreateWithUTF8CString(script->name);
	
	rval=JSEvaluateScript(script->cx,j_script_data,NULL,j_script_name,0,&exception);

	JSStringRelease(j_script_name);
	JSStringRelease(j_script_data);

		// check for errors
		
	if (rval==NULL) {
		script_exception_to_string(script->cx,-1,exception,err_str,256);
		return(FALSE);
	}
	
	return(TRUE);
}

/* =======================================================

      Add A Script
      
======================================================= */
	
int scripts_add_single(int thing_type,char *sub_dir,char *name,int obj_idx,int weap_idx,int proj_setup_idx,char *err_str)
{
	int						n,idx;
	bool					ok;
	script_type				*script;

		// find a unused script
		
	idx=-1;
	
	for (n=0;n!=max_script_list;n++) {
		script=js.script_list.scripts[n];
		if (script==NULL) {
			idx=n;
			break;
		}
	}
	
	if (idx==-1) {
		strcpy(err_str,"JavaScript Engine: Reached the maximum number of scripts");
		return(-1);
	}
	
		// create it
		
	js.script_list.scripts[idx]=(script_type*)malloc(sizeof(script_type));
	if (js.script_list.scripts[idx]==NULL) {
		strcpy(err_str,"JavaScript Engine: Out of memory");
		return(-1);
	}

		// add it

	script=js.script_list.scripts[idx];

	script->idx=idx;
	script->parent_idx=-1;
	script->child_idx=-1;
	
	strcpy(script->name,name);
	strcpy(script->sub_dir,sub_dir);
	script->implement_name[0]=0x0;

		// setup script data

	scripts_setup_data(script,thing_type,obj_idx,weap_idx,proj_setup_idx);
	
		// create the context
		// and remember the global object
		
	script->cx=JSGlobalContextCreateInGroup(js.cx_group,NULL);
	JSGlobalContextRetain(script->cx);
	
	script->global_obj=JSContextGetGlobalObject(script->cx);
	
		// load in script

	if (!script_load_file(script,sub_dir,name,err_str)) {
		free(js.script_list.scripts[idx]);
		js.script_list.scripts[idx]=NULL;
		return(-1);
	}
	
		// create the global object

	if (!script_add_global_object(script,err_str)) {
		script_free_file(script);
		free(js.script_list.scripts[idx]);
		js.script_list.scripts[idx]=NULL;
		return(-1);
	}

		// add in any constants

	script_defines_create_constants(script);
	
		// create the object
		
	script->obj=script_create_main_object(script->cx,script->idx);
	if (script->obj==NULL) {
		strcpy(err_str,"JavaScript Engine: Not enough memory to create an object");
		script_free_file(script);
		free(js.script_list.scripts[idx]);
		js.script_list.scripts[idx]=NULL;
		return(-1);
	}
	
		// root the object
	
	JSValueProtect(script->cx,(JSValueRef)script->obj);
	
		// compile and execute
		
	ok=scripts_execute(script,err_str);
	script_free_file(script);
	
	if (!ok) {
		free(js.script_list.scripts[idx]);
		js.script_list.scripts[idx]=NULL;
		return(-1);
	}
		
	return(idx);
}

int scripts_add(int thing_type,char *sub_dir,char *name,int obj_idx,int weap_idx,int proj_setup_idx,char *err_str)
{
	int					script_idx,parent_script_idx;
	script_type			*script,*parent_script;

		// add the script
		
	script_idx=scripts_add_single(thing_type,sub_dir,name,obj_idx,weap_idx,proj_setup_idx,err_str);
	if (script_idx==-1) return(-1);
	
		// setup any implemented parent scripts
		
	script=js.script_list.scripts[script_idx];
		
	if (script->implement_name[0]!=0x0) {
	
			// start the parent script
	
		parent_script_idx=scripts_add(thing_type,script->sub_dir,script->implement_name,obj_idx,weap_idx,proj_setup_idx,err_str);
		if (parent_script_idx==-1) return(-1);

			// set the parent and the child

		script->parent_idx=parent_script_idx;

		parent_script=js.script_list.scripts[parent_script_idx];
		parent_script->child_idx=script_idx;
	}

	return(script_idx);
}

bool scripts_set_implement(int script_idx,char *name,char *err_str)
{
	script_type			*script;
	
		// already have a parent?

	script=js.script_list.scripts[script_idx];
	if (script->parent_idx!=-1) {
		strcpy(err_str,"This script already has an implemented script");
		return(FALSE);
	}

		// parent scripts get constructed when child scripts
		// finished executing
		
	strcpy(script->implement_name,name);

	return(TRUE);
}

/* =======================================================

      Dispose Scripts
      
======================================================= */

void scripts_dispose_single(int idx)
{
	script_type		*script;
	
		// dispose all script timers

	timers_script_dispose(idx);

		// unroot the object and clean up
		// the context

	script=js.script_list.scripts[idx];
	
	JSValueUnprotect(script->cx,(JSValueRef)script->obj);
	JSGlobalContextRelease(script->cx);
	
		// force clean-up of removed scripts
	
	JSGarbageCollect(NULL);
	
		// script is now free
		
	free(js.script_list.scripts[idx]);
	js.script_list.scripts[idx]=NULL;
}

void scripts_dispose(int idx)
{
	script_type		*script;

		// no attached script

	if (idx==-1) return;

		// dispose any parent
		
	script=js.script_list.scripts[idx];
	if (script->parent_idx!=-1) scripts_dispose_single(script->parent_idx);

		// dispose the script itself

	scripts_dispose_single(idx);
}

/* =======================================================

      JS Glue Code
      
======================================================= */

// supergumba -- js -- as a patch?

bool JSValueIsArray(JSContextRef ctx,JSValueRef value)
{
	char				*ch,str[64];
	JSStringRef			jsStr;
	JSObjectRef			arrayObj;
	JSValueRef			val;
	
		// only check objects

	if (!JSValueIsObject(ctx,value)) return(FALSE);
	
		// get constructor

	arrayObj=JSValueToObject(ctx,value,NULL);
	if (arrayObj==NULL) return(FALSE);

	jsStr=JSStringCreateWithUTF8CString("constructor");
	val=JSObjectGetProperty(ctx,arrayObj,jsStr,NULL);
	JSStringRelease(jsStr);

	if (val==NULL) return(FALSE);

		// get constructor as a function

	jsStr=JSValueToStringCopy(ctx,val,NULL);
	if (jsStr==NULL) return(FALSE);

	JSStringGetUTF8CString(jsStr,str,64);
	str[63]=0x0;

	JSStringRelease(jsStr);

		// special check to make sure we don't have Array
		// anywhere else in a function body or there just
		// instead a function body

	ch=strchr(str,'{');
	if (ch==NULL) return(FALSE);

	*ch=0x0;

		// look for array in string

	return(strstr(str,"Array")!=NULL);
}


