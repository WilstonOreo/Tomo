#pragma once

#ifdef __APPLE__
//#include <OpenGL/glew.h>
#define QT_NO_OPENGL_ES_2
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/glew.h>
#define QT_NO_OPENGL_ES_2
#include <GL/gl.h>
#include <GL/glu.h>
#endif

#include <tbd/property.h>

#include "gl/types.hpp"
#include "gl/draw.hpp"
#include "gl/normal.hpp"
#include "gl/vertex.hpp"
#include "gl/texCoord.hpp"
#include "gl/color.hpp"
#include "gl/enable_disable.hpp"
#include "gl/transform.hpp"

#include "gl/Texture.hpp"
#include "gl/Shader.hpp"
#include "gl/DisplayList.hpp"
#include "gl/FrameBufferTexture.hpp"

