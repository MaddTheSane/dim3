/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script C<-->JSValueRef Conversion

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

extern JSClassRef		global_point_angle_vector_class,global_color_class;


/* =======================================================

      NULL JSValueRef
      
======================================================= */

JSValueRef script_null_to_value(JSContextRef cx)
{
	return(JSValueMakeNull(cx));
}

bool script_is_value_null(JSContextRef cx,JSValueRef val)
{
	return(JSValueIsNull(cx,val));
}

/* =======================================================

      Int-JSValueRef Translations
      
======================================================= */

int script_value_to_int(JSContextRef cx,JSValueRef val)
{
	return((int)JSValueToNumber(cx,val,NULL));
}

JSValueRef script_int_to_value(JSContextRef cx,int i)
{
	return(JSValueMakeNumber(cx,(float)i));
}

/* =======================================================

      Float-JSValueRef Translations
      
======================================================= */

float script_value_to_float(JSContextRef cx,JSValueRef val)
{
	return((float)JSValueToNumber(cx,val,NULL));
}

JSValueRef script_float_to_value(JSContextRef cx,float f)
{
	return(JSValueMakeNumber(cx,(double)f));
}

/* =======================================================

      Bool-JSValueRef Translations
      
======================================================= */

bool script_value_to_bool(JSContextRef cx,JSValueRef val)
{
	return(JSValueToBoolean(cx,val));
}

JSValueRef script_bool_to_value(JSContextRef cx,bool b)
{
	return(JSValueMakeBoolean(cx,b));
}

/* =======================================================

      String-JSValueRef Translations
      
======================================================= */

void script_value_to_string(JSContextRef cx,JSValueRef val,char *str,int len)
{
	JSStringRef		js_str;
	
	if (JSValueIsNull(cx,val)) {
		str[0]=0x0;
		return;
	}

	js_str=JSValueToStringCopy(cx,val,NULL);
	if (js_str==NULL) {
		str[0]=0x0;
		return;
	}

	JSStringGetUTF8CString(js_str,str,len);
	JSStringRelease(js_str);

	str[len-1]=0x0;
}

JSValueRef script_string_to_value(JSContextRef cx,char *str)
{
	JSStringRef		js_str;

	js_str=JSStringCreateWithUTF8CString(str);
	return(JSValueMakeString(cx,js_str));
}

/* =======================================================

      Array-JSValueRef Translations
      
======================================================= */

JSValueRef script_int_array_to_value(JSContextRef cx,int cnt,int *values)
{
	int				n;
	JSValueRef		js_vals[256];

		// 256 is the longest array

	if (cnt>256) cnt=256;

		// create the values

	for (n=0;n!=cnt;n++) {
		js_vals[n]=script_int_to_value(cx,values[n]);
	}

	return((JSValueRef)JSObjectMakeArray(cx,cnt,js_vals,NULL));
}

/* =======================================================

      Angle-JSValue Transformations
      
======================================================= */

void script_value_to_angle(JSContextRef cx,JSValueRef val,d3ang *ang)
{
	JSObjectRef			obj;
	JSValueRef			vp;

	ang->x=ang->y=ang->z=0.0f;

	obj=JSValueToObject(cx,val,NULL);
	if (obj==NULL) return;

	vp=script_get_single_property(cx,obj,"x");
	if (vp!=NULL) ang->x=script_value_to_float(cx,vp);

	vp=script_get_single_property(cx,obj,"y");
	if (vp!=NULL) ang->y=script_value_to_float(cx,vp);

	vp=script_get_single_property(cx,obj,"z");
	if (vp!=NULL) ang->z=script_value_to_float(cx,vp);
}

JSValueRef script_angle_to_value(JSContextRef cx,d3ang *ang)
{
	JSObjectRef		j_obj;

	j_obj=JSObjectMake(cx,NULL,NULL);

	script_set_single_property(cx,j_obj,"x",script_float_to_value(cx,ang->x),(kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete));
	script_set_single_property(cx,j_obj,"y",script_float_to_value(cx,ang->y),(kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete));
	script_set_single_property(cx,j_obj,"z",script_float_to_value(cx,ang->z),(kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete));
	
	return((JSValueRef)j_obj);
}

/* =======================================================

      Vector-JSValue Transformations
      
======================================================= */

void script_value_to_vector(JSContextRef cx,JSValueRef val,d3vct *vct)
{
	JSObjectRef			obj;
	JSValueRef			vp;

	vct->x=vct->y=vct->z=0.0f;

	obj=JSValueToObject(cx,val,NULL);
	if (obj==NULL) return;

	vp=script_get_single_property(cx,obj,"x");
	if (vp!=NULL) vct->x=script_value_to_float(cx,vp);

	vp=script_get_single_property(cx,obj,"y");
	if (vp!=NULL) vct->y=script_value_to_float(cx,vp);

	vp=script_get_single_property(cx,obj,"z");
	if (vp!=NULL) vct->z=script_value_to_float(cx,vp);
}

JSValueRef script_vector_to_value(JSContextRef cx,d3vct *vct)
{
	JSObjectRef		j_obj;

	j_obj=JSObjectMake(cx,NULL,NULL);

	script_set_single_property(cx,j_obj,"x",script_float_to_value(cx,vct->x),(kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete));
	script_set_single_property(cx,j_obj,"y",script_float_to_value(cx,vct->y),(kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete));
	script_set_single_property(cx,j_obj,"z",script_float_to_value(cx,vct->z),(kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete));
	
	return((JSValueRef)j_obj);
}

/* =======================================================

      Point-JSValue Transformations
      
======================================================= */

void script_value_to_point(JSContextRef cx,JSValueRef val,d3pnt *pnt)
{
	JSObjectRef			obj;
	JSValueRef			vp;

	pnt->x=pnt->y=pnt->z=0;

	obj=JSValueToObject(cx,val,NULL);
	if (obj==NULL) return;
	
	vp=script_get_single_property(cx,obj,"x");
	if (vp!=NULL) pnt->x=script_value_to_int(cx,vp);

	vp=script_get_single_property(cx,obj,"y");
	if (vp!=NULL) pnt->y=script_value_to_int(cx,vp);

	vp=script_get_single_property(cx,obj,"z");
	if (vp!=NULL) pnt->z=script_value_to_int(cx,vp);
}

void script_value_to_2D_point(JSContextRef cx,JSValueRef val,d3pnt *pnt)
{
	JSObjectRef			obj;
	JSValueRef			vp;

	pnt->x=pnt->y=pnt->z=0;

	obj=JSValueToObject(cx,val,NULL);
	if (obj==NULL) return;

	vp=script_get_single_property(cx,obj,"x");
	if (vp!=NULL) pnt->x=script_value_to_int(cx,vp);

	vp=script_get_single_property(cx,obj,"y");
	if (vp!=NULL) pnt->y=script_value_to_int(cx,vp);
}

void script_value_to_2D_float_point(JSContextRef cx,JSValueRef val,d3fpnt *fpnt)
{
	JSObjectRef			obj;
	JSValueRef			vp;

	fpnt->x=fpnt->y=fpnt->z=0.0f;

	obj=JSValueToObject(cx,val,NULL);
	if (obj==NULL) return;

	vp=script_get_single_property(cx,obj,"x");
	if (vp!=NULL) fpnt->x=script_value_to_float(cx,vp);

	vp=script_get_single_property(cx,obj,"y");
	if (vp!=NULL) fpnt->y=script_value_to_float(cx,vp);
}

JSValueRef script_point_to_value(JSContextRef cx,d3pnt *pnt)
{
	JSObjectRef		j_obj;

	j_obj=JSObjectMake(cx,global_point_angle_vector_class,NULL);

	script_set_single_property(cx,j_obj,"x",script_int_to_value(cx,pnt->x),(kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete));
	script_set_single_property(cx,j_obj,"y",script_int_to_value(cx,pnt->y),(kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete));
	script_set_single_property(cx,j_obj,"z",script_int_to_value(cx,pnt->z),(kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete));
	
	return((JSValueRef)j_obj);
}

/* =======================================================

      Color-JSValue Transformations
      
======================================================= */

void script_value_to_color(JSContextRef cx,JSValueRef val,d3col *col)
{
	JSObjectRef			obj;
	JSValueRef			vp;

		// check for both red/r, green/g, and blue/b

	col->r=col->g=col->b=0.0f;

	obj=JSValueToObject(cx,val,NULL);
	if (obj==NULL) return;

	vp=script_get_single_property(cx,obj,"red");
	if (vp==NULL) vp=script_get_single_property(cx,obj,"r");
	if (vp!=NULL) col->r=script_value_to_float(cx,vp);

	col->g=0.0f;
	vp=script_get_single_property(cx,obj,"green");
	if (vp==NULL) vp=script_get_single_property(cx,obj,"g");
	if (vp!=NULL) col->g=script_value_to_float(cx,vp);

	col->b=0.0f;
	vp=script_get_single_property(cx,obj,"blue");
	if (vp==NULL) vp=script_get_single_property(cx,obj,"b");
	if (vp!=NULL) col->b=script_value_to_float(cx,vp);
}

JSValueRef script_color_to_value(JSContextRef cx,d3col *col)
{
	JSObjectRef		j_obj;

	j_obj=JSObjectMake(cx,global_color_class,NULL);

	script_set_single_property(cx,j_obj,"red",script_float_to_value(cx,col->r),(kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete));
	script_set_single_property(cx,j_obj,"green",script_float_to_value(cx,col->g),(kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete));
	script_set_single_property(cx,j_obj,"blue",script_float_to_value(cx,col->b),(kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete));
	
	return((JSValueRef)j_obj);
}

/* =======================================================

      Exceptions
      
======================================================= */

JSValueRef script_create_exception(JSContextRef cx,char *str)
{
	JSValueRef			argv[1];
	
	argv[0]=script_string_to_value(cx,str);
	return((JSValueRef)JSObjectMakeError(cx,1,argv,NULL));
}

void script_exception_to_string(JSContextRef cx,int main_event,JSValueRef ex_val,char *str,int len)
{
	char				txt[256];
	JSObjectRef			ex_obj;
	JSValueRef			vp;

	ex_obj=JSValueToObject(cx,ex_val,NULL);

		// source and line numbers

	strcpy(str,"[");

	vp=script_get_single_property(cx,ex_obj,"sourceURL");
	script_value_to_string(cx,vp,txt,256);
	string_safe_strcat(str,txt,len);
		
	string_safe_strcat(str,":",len);

	vp=script_get_single_property(cx,ex_obj,"line");
	script_value_to_string(cx,vp,txt,256);
	string_safe_strcat(str,txt,len);
	
	string_safe_strcat(str,"]",len);
	
		// get message

	vp=script_get_single_property(cx,ex_obj,"message");
	if (vp==NULL) {
		string_safe_strcat(str,"Unknown Error",len);
	}
	else {
		script_value_to_string(cx,vp,txt,256);
		string_safe_strcat(str,txt,len);
	}
}

