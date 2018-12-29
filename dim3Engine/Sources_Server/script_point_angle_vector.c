/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: Point/Angle/Vector Constructor

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
	#include "dim3engine.h"
#endif

#include "interface.h"
#include "objects.h"
#include "scripts.h"

JSObjectRef js_global_point_angle_vector_constructor_func(JSContextRef cx,JSObjectRef func,size_t argc,const JSValueRef argv[],JSValueRef* exception);

extern js_type			js;

JSClassRef			global_point_angle_vector_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_global_point_angle_vector_object(void)
{
	global_point_angle_vector_class=script_create_class("global_point_angle_vector",NULL,NULL);
}

void script_free_global_point_angle_vector_object(void)
{
	script_free_class(global_point_angle_vector_class);
}

JSObjectRef script_add_global_point_angle_vector_object(JSContextRef cx,JSObjectRef parent_obj,int script_idx)
{
	JSObjectRef			j_obj;
	
	j_obj=JSObjectMakeConstructor(cx,global_point_angle_vector_class,js_global_point_angle_vector_constructor_func);
	script_set_single_property(cx,parent_obj,"Point",JSValueToObject(cx,j_obj,NULL),(kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete));

	j_obj=JSObjectMakeConstructor(cx,global_point_angle_vector_class,js_global_point_angle_vector_constructor_func);
	script_set_single_property(cx,parent_obj,"Angle",JSValueToObject(cx,j_obj,NULL),(kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete));

	j_obj=JSObjectMakeConstructor(cx,global_point_angle_vector_class,js_global_point_angle_vector_constructor_func);
	script_set_single_property(cx,parent_obj,"Vector",JSValueToObject(cx,j_obj,NULL),(kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete));

	return(j_obj);
}

/* =======================================================

      Constructor
      
======================================================= */

JSObjectRef js_global_point_angle_vector_constructor_func(JSContextRef cx,JSObjectRef c_j_obj,size_t argc,const JSValueRef argv[],JSValueRef* exception)
{
	JSValueRef		vp;
	JSObjectRef		j_obj;
	
		// creating new point/angle/vector from object
		
	if (argc==1) {
	
		if (!JSValueIsObject(cx,argv[0])) {
			script_create_exception(cx,"This constructor requires an object when using one parameter.");
			return(JSValueToObject(cx,script_null_to_value(cx),NULL));
		}
		
		j_obj=JSObjectMake(cx,global_point_angle_vector_class,NULL);
		
		vp=script_get_single_property(cx,((JSObjectRef)argv[0]),"x");
		if (vp!=NULL) script_set_single_property(cx,j_obj,"x",vp,kJSPropertyAttributeDontDelete);
		
		vp=script_get_single_property(cx,((JSObjectRef)argv[0]),"y");
		if (vp!=NULL) script_set_single_property(cx,j_obj,"y",vp,kJSPropertyAttributeDontDelete);
		
		vp=script_get_single_property(cx,((JSObjectRef)argv[0]),"z");
		if (vp!=NULL) script_set_single_property(cx,j_obj,"z",vp,kJSPropertyAttributeDontDelete);
	
		return(j_obj);
	}
	
		// creating from parameters

	if (!script_check_param_count(cx,NULL,argc,3,exception)) return(NULL);

	j_obj=JSObjectMake(cx,global_point_angle_vector_class,NULL);
	
	script_set_single_property(cx,j_obj,"x",argv[0],kJSPropertyAttributeDontDelete);
	script_set_single_property(cx,j_obj,"y",argv[1],kJSPropertyAttributeDontDelete);
	script_set_single_property(cx,j_obj,"z",argv[2],kJSPropertyAttributeDontDelete);
	
	return(j_obj);
}

