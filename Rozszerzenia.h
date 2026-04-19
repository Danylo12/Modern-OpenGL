#include <gl/GL.h>
#include "glext.h"
#include "wglext.h"


PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = NULL;


bool InicjujRozszerzenia()
{
	wglCreateContextAttribsARB =
		reinterpret_cast<PFNWGLCREATECONTEXTATTRIBSARBPROC>(
		wglGetProcAddress("wglCreateContextAttribsARB"));
	if (wglCreateContextAttribsARB == NULL) return false;
	return true;
}

