/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script Events

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

bool						scripts_event_lock;

extern server_type			server;
extern js_type				js;
extern setup_type			setup;
extern iface_type			iface;

/* =======================================================

      Script Events Setup
      
======================================================= */

bool scripts_setup_event_attach(int script_idx,int main_event,char *func_name,char *err_str)
{
	int					idx;
	script_type			*script;
	JSObjectRef			func;

		// get the script

	script=js.script_list.scripts[script_idx];

		// get the func and determine if
		// it exists and is callable
		
	func=script_get_single_function(script->cx,script->global_obj,func_name);
	if (func==NULL) {
		sprintf(err_str,"[%s] '%s' does not exist or is not a callable function",script->name,func_name);
		return(FALSE);
	}

		// legal event?

	if ((main_event<event_main_id_start) || (main_event>event_main_id_end)) {
		strcpy(err_str,"Attempting to setup an unknown event");
		return(FALSE);
	}

		// attach

	idx=main_event-event_main_id_start;

	script->event_attach_list.func[idx]=func;

	return(TRUE);
}

/* =======================================================

      Locking and Unlocking Events
      
======================================================= */

void scripts_lock_events(void)
{
		// force the core to never call events
		// this is mostly used when restoring a saved
		// game (need to load game/map but not affect script state)

	scripts_event_lock=TRUE;
}

void scripts_unlock_events(void)
{
	scripts_event_lock=FALSE;
}

/* =======================================================

      Scripts Recursion Checks
      
======================================================= */

bool scripts_recursion_in(script_type *script,char *err_str)
{
	if (script->recursive.count==js_max_recursive_count) {
		sprintf(err_str,"[%s] Script has passed the maximum number of recursions: %d",script->name,js_max_recursive_count);
		return(FALSE);
	}

	script->recursive.count++;

	return(TRUE);
}

void scripts_recursion_out(script_type *script)
{
	script->recursive.count--;
}

/* =======================================================

      Script Events
      
======================================================= */

bool scripts_post_event_on_attach(int script_idx,int override_proj_idx,int main_event,int sub_event,int id,char *err_str)
{
	int						event_idx,tick,old_proj_idx;
	JSValueRef				rval,exception,argv[5];
	script_type				*script;

		// no error
		
	err_str[0]=0x0;

		// are events locked?  If so, don't call
		// any events, but act as if it was OK

	if (scripts_event_lock) return(TRUE);
	
		// find script
		
	script=js.script_list.scripts[script_idx];

		// is this an attached event?

	event_idx=main_event-event_main_id_start;
	if (script->event_attach_list.func[event_idx]==NULL) return(TRUE);
	
		// can't re-enter an event we are already in
		// this is a silent failure as it usually happens
		// in cascading damage events
		
	if (script->recursive.in_event[event_idx]) return(TRUE);

		// enter recursion

	if (!scripts_recursion_in(script,err_str)) return(FALSE);
	
		// in event
		
	script->recursive.in_event[event_idx]=TRUE;
	
		// attach to proper script
		
	tick=game_time_get();

	script->event_state.main_event=main_event;
	script->event_state.sub_event=sub_event;
	script->event_state.id=id;
	script->event_state.tick=tick;

		// handle any proj_setup/proj swaps

	if (override_proj_idx!=-1) {
		old_proj_idx=script->attach.proj_idx;
		script->attach.proj_idx=override_proj_idx;
	}

		// run the event function

	argv[0]=(JSValueRef)script->obj;
	argv[1]=script_int_to_value(script->cx,sub_event);
	argv[2]=script_int_to_value(script->cx,id);
	argv[3]=script_int_to_value(script->cx,tick);
	
	rval=JSObjectCallAsFunction(script->cx,script->event_attach_list.func[event_idx],NULL,4,argv,&exception);
	if (rval==NULL) script_exception_to_string(script->cx,main_event,exception,err_str,256);

		// switch back any saved proj_idx

	if (override_proj_idx!=-1) script->attach.proj_idx=old_proj_idx;
	
		// leave event
		
	script->recursive.in_event[event_idx]=FALSE;

		// leave recursion

	scripts_recursion_out(script);
	
	return(err_str[0]==0x0);
}

bool scripts_post_event(int script_idx,int override_proj_idx,int main_event,int sub_event,int id,char *err_str)
{
	int						event_idx,parent_cnt;
	script_type				*script,*call_script;
	
		// ignore if no script

	if (script_idx==-1) return(TRUE);

	script=js.script_list.scripts[script_idx];

	event_idx=main_event-event_main_id_start;

		// call through script chain

	parent_cnt=0;

	while (TRUE) {

		call_script=js.script_list.scripts[script_idx];
		if (call_script->event_attach_list.func[event_idx]!=NULL) return(scripts_post_event_on_attach(script_idx,override_proj_idx,main_event,sub_event,id,err_str));

		script_idx=call_script->parent_idx;
		if (script_idx==-1) return(TRUE);

		parent_cnt++;
		if (parent_cnt==js_max_parent_count) {
			sprintf(err_str,"[%s] Script has passed the maximum number of implementations (parents): %d",script->name,js_max_parent_count);
			return(FALSE);
		}
	}
}

void scripts_post_event_console(int script_idx,int override_proj_idx,int main_event,int sub_event,int id)
{
	char			err_str[256];
	
	if (!scripts_post_event(script_idx,override_proj_idx,main_event,sub_event,id,err_str)) {
		console_add_error(err_str);
	}
}

/* =======================================================

      Script Parent Calls
      
======================================================= */

bool scripts_post_event_call_parent(int script_idx,char *err_str)
{
	int				override_proj_idx;
	script_type		*script;
	
		// get the parent script

	script=js.script_list.scripts[script_idx];
	if (script->parent_idx==-1) {
		strcpy(err_str,"Can not call parent; this script does not implement a parent script");
		return(FALSE);
	}
		// get the override

	override_proj_idx=-1;
	if (script->attach.thing_type==thing_type_projectile) override_proj_idx=script->attach.proj_idx;

		// call on parent script

	return(scripts_post_event_on_attach(script->parent_idx,override_proj_idx,script->event_state.main_event,script->event_state.sub_event,script->event_state.id,err_str));
}

JSValueRef scripts_get_parent_variable(int script_idx,char *prop_name,char *err_str)
{
	script_type		*script,*parent_script;
	JSValueRef		rval;
	
		// get the parent script

	script=js.script_list.scripts[script_idx];
	if (script->parent_idx==-1) {
		strcpy(err_str,"Can not get parent variable; this script does not implement a parent script");
		return(FALSE);
	}
	
	parent_script=js.script_list.scripts[script->parent_idx];

		// get variable

	rval=script_get_single_property_with_has_check(parent_script->cx,parent_script->global_obj,prop_name);
	if (rval==NULL) {
		sprintf(err_str,"Parent has no variable named: %s",prop_name);
		return(NULL);
	}

	return(rval);
}

JSValueRef scripts_call_parent_function(int script_idx,char *func_name,int arg_count,JSValueRef *args,char *err_str)
{
	int				override_proj_idx;
	script_type		*script,*parent_script;
	
		// get the parent script

	script=js.script_list.scripts[script_idx];
	if (script->parent_idx==-1) {
		strcpy(err_str,"Can not call parent function; this script does not implement a parent script");
		return(FALSE);
	}
	
		// save the event state
		
	parent_script=js.script_list.scripts[script->parent_idx];
	memmove(&parent_script->event_state,&script->event_state,sizeof(script_event_state_type));

		// get the override

	override_proj_idx=-1;
	if (script->attach.thing_type==thing_type_projectile) override_proj_idx=script->attach.proj_idx;

		// call the function

	return(scripts_direct_call(script_idx,script->parent_idx,override_proj_idx,func_name,arg_count,args,err_str));
}

/* =======================================================

      Script Child Calls
      
======================================================= */

JSValueRef scripts_get_child_variable(int script_idx,char *prop_name,char *err_str)
{
	script_type		*script,*child_script;
	JSValueRef		rval;
	
		// get the child script

	script=js.script_list.scripts[script_idx];
	if (script->child_idx==-1) {
		strcpy(err_str,"Can not get child variable; this script is not implemented from a parent script");
		return(FALSE);
	}
	
	child_script=js.script_list.scripts[script->child_idx];

		// get variable

	rval=script_get_single_property_with_has_check(child_script->cx,child_script->global_obj,prop_name);
	if (rval==NULL) {
		sprintf(err_str,"Child has no variable named: %s",prop_name);
		return(NULL);
	}

	return(rval);
}

JSValueRef scripts_call_child_function(int script_idx,char *func_name,int arg_count,JSValueRef *args,char *err_str)
{
	int				override_proj_idx;
	script_type		*script,*child_script;
	
		// get the child script

	script=js.script_list.scripts[script_idx];
	if (script->child_idx==-1) {
		strcpy(err_str,"Can not get call child function; this script is not implemented from a parent script");
		return(FALSE);
	}
	
		// save the event state
		
	child_script=js.script_list.scripts[script->child_idx];
	memmove(&child_script->event_state,&script->event_state,sizeof(script_event_state_type));

		// get the override

	override_proj_idx=-1;
	if (script->attach.thing_type==thing_type_projectile) override_proj_idx=script->attach.proj_idx;

		// call the function

	return(scripts_direct_call(script_idx,script->child_idx,override_proj_idx,func_name,arg_count,args,err_str));
}

/* =======================================================

      Generic Script Call Function
      
======================================================= */

JSValueRef scripts_generic_function_call(int script_idx,char *call_type_str,int arg_count,JSValueRef *argv,char *func_name,char *err_str)
{
	int				org_script_idx,parent_cnt;
	JSObjectRef		func_obj;
	JSValueRef		rval,exception;
	script_type		*call_script;

	org_script_idx=script_idx;
	parent_cnt=0;

	while (TRUE) {

		call_script=js.script_list.scripts[script_idx];
		
			// does function exist, if so, call it

		func_obj=script_get_single_function(call_script->cx,call_script->global_obj,func_name);
		if (func_obj!=NULL) {
			rval=JSObjectCallAsFunction(call_script->cx,func_obj,NULL,arg_count,argv,&exception);
			if (rval==NULL) script_exception_to_string(call_script->cx,call_script->event_state.main_event,exception,err_str,256);
			return(rval);
		}

			// move onto parent

		script_idx=call_script->parent_idx;
		if (script_idx==-1) {
			sprintf(err_str,"%s failed, unknown or non-callable function: %s",call_type_str,func_name);
			return(NULL);
		}

		parent_cnt++;
		if (parent_cnt==js_max_parent_count) {
			sprintf(err_str,"[%s] Script has passed the maximum number of implementations (parents): %d",js.script_list.scripts[org_script_idx]->name,js_max_parent_count);
			return(NULL);
		}
	}

	return(NULL);
}

/* =======================================================

      Script Chains
      
======================================================= */

bool scripts_chain(int script_idx,int override_proj_idx,char *func_name,char *err_str)
{
	int				old_proj_idx;
	JSValueRef		rval,argv[2];
	script_type		*script;
	
		// no error
		
	err_str[0]=0x0;
	
		// find script
		
	script=js.script_list.scripts[script_idx];

		// enter recursion

	if (!scripts_recursion_in(script,err_str)) return(FALSE);

		// handle any proj_setup/proj swaps

	if (override_proj_idx!=-1) {
		old_proj_idx=script->attach.proj_idx;
		script->attach.proj_idx=override_proj_idx;
	}

		// run the event function
		
	argv[0]=(JSValueRef)script->obj;
	argv[1]=script_int_to_value(script->cx,game_time_get());

	rval=scripts_generic_function_call(script_idx,"Chaining",2,argv,func_name,err_str);

		// switch back any saved proj_idx

	if (override_proj_idx!=-1) script->attach.proj_idx=old_proj_idx;

		// leave recursion

	scripts_recursion_out(script);
	
	return(err_str[0]==0x0);
}

void scripts_chain_console(int script_idx,int override_proj_idx,char *func_name)
{
	char			err_str[256];
	
	if (!scripts_chain(script_idx,override_proj_idx,func_name,err_str)) console_add_error(err_str);
}

/* =======================================================

      Script Direct Call
      
======================================================= */

JSValueRef scripts_direct_call(int from_script_idx,int to_script_idx,int override_proj_idx,char *func_name,int arg_count,JSValueRef *args,char *err_str)
{
	int				n,old_proj_idx;
	JSValueRef		rval,argv[32];
	script_type		*to_script;
	
		// find script
		
	to_script=js.script_list.scripts[to_script_idx];

		// enter recursion

	if (!scripts_recursion_in(to_script,err_str)) return(NULL);

		// handle any proj_setup/proj swaps

	if (override_proj_idx!=-1) {
		old_proj_idx=to_script->attach.proj_idx;
		to_script->attach.proj_idx=override_proj_idx;
	}
	
		// inherit the event
		
	memmove(&to_script->event_state,&js.script_list.scripts[from_script_idx]->event_state,sizeof(script_event_state_type));

		// call the function
		
	argv[0]=(JSValueRef)to_script->obj;

	if (arg_count>31) arg_count=31;

	for (n=0;n!=arg_count;n++) { 
		argv[n+1]=args[n];
	}

	rval=scripts_generic_function_call(to_script_idx,"Direct call",(arg_count+1),argv,func_name,err_str);

		// switch back any saved proj_idx

	if (override_proj_idx!=-1) to_script->attach.proj_idx=old_proj_idx;

		// leave recursion

	scripts_recursion_out(to_script);
	
	return(rval);
}
