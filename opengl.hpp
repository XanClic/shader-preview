#ifndef OPENGL_HPP
#define OPENGL_HPP

#define GL_GLEXT_PROTOTYPES

#ifndef _WIN32
#include <GL/gl.h>
#include <GL/glext.h>
#else
#include <GL/glew.h>
#include <GL/gl.h>
#endif

#endif
