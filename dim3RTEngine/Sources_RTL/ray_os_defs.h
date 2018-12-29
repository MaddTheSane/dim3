//
// dim3RTL
// by Brian Barnes (bbarnes@klinksoftware.com)
// free software offered as is for any use
//

#ifdef __APPLE__

	#include <stdio.h>
	#include <stdlib.h>
	#include <sys/stat.h>
	#include <sys/param.h>
	#include <sys/sysctl.h>
	#include <unistd.h>
	#include <pthread.h>

#ifndef TARGET_IPHONE
	#include <OpenGL/OpenGL.h>
	#include <OpenGL/gl.h>
#else
	#include <OpenGLES/es2/gl.h>
	#include <OpenGLES/es2/glext.h>
#endif

	typedef pthread_mutex_t			ray_mutex;
	typedef pthread_t				ray_thread;
	typedef pthread_cond_t			ray_cond;

#endif

#ifdef __linux__

	#include <unistd.h>
	#include <pthread.h>

	#include <GL/glew.h>

	typedef pthread_mutex_t			ray_mutex;
	typedef pthread_t				ray_thread;
	typedef pthread_cond_t			ray_cond;

#endif

#ifdef WIN32

	#define _CRT_SECURE_NO_WARNINGS

	#include <windows.h>

	#include <time.h>
	#include <sys/types.h>
	#include <sys/stat.h>
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include <math.h>
	#include <time.h>
	#include <process.h>

	#include "glew.h"

	#define inline					__inline

	typedef unsigned char			bool;
	typedef CRITICAL_SECTION		ray_mutex;
	typedef HANDLE					ray_thread;
	typedef int						ray_cond;

	#define usleep(x)               Sleep((DWORD)((x)/1000))

#endif

