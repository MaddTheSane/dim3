/****************************** File *********************************

Module: dim3 Common
Author: Brian Barnes
 Usage: Specific OS Definitions

***************************** License ********************************

This code can be freely used as long as these conditions are met:

1. This header, in its entirety, is kept with the code
2. This credit ñCreated with dim3 Technologyî is given on a single
application screen and in a single piece of the documentation
3. It is not resold, in it's current form or modified, as an
engine-only product

This code is presented as is. The author of dim3 takes no
responsibilities for any version of this code.

Any non-engine product (games, etc) created with this code is free
from any and all payment and/or royalties to the author of dim3,
and can be sold or given away.

(c) 2000-2005 Klink! Software www.klinksoftware.com

*********************************************************************/

//
// includes
//

#define _CRT_SECURE_NO_WARNINGS

#include <winsock2.h>
#include <windows.h>

#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <math.h>

#include <shlobj.h>

#include "glew.h"
#include "wglew.h"

#include "zlib.h"

//
// os flags
//

#define D3_OS_WINDOWS			1
#undef D3_OPENGL_ES
#undef D3_ROTATE_VIEW

//
// os specific keys
//

#define D3_KEY_TAB				0x9
#define D3_KEY_ESC				0x1B
#define D3_KEY_BACKSPACE		0x8
#define D3_KEY_DELETE			0x2E
#define D3_KEY_LEFT				0x25
#define D3_KEY_RIGHT			0x27
#define D3_KEY_UP				0x26
#define D3_KEY_DOWN				0x28

//
// os specific types
//

typedef unsigned char			bool;
typedef unsigned char*			ptr;
typedef SOCKET					d3socket;
typedef int						socklen_t;

#define FOUR_CHAR_CODE(x)		(x)

#define inline					__inline
#define random					rand
#define srandom					srand
#define snprintf				_snprintf
#define strtof(a,b)				((float)strtod(a,b))
#define bzero(a,b)				memset(a,0x0,(b))
#define bcmp(a,b,c)				memcmp(a,b,(c))
#define usleep(x)               Sleep((DWORD)((x)/1000))
#define strcasecmp(a,b)			_stricmp(a,b)
#define strncasecmp(a,b,c)		_strnicmp(a,b,c)
#define stat					_stat
#define log2(a)					(log(a)/log(2))

#define D3_NULL_SOCKET			INVALID_SOCKET

#define WindowRef				HWND

#define D3_APP_EXTENSION		"exe"
