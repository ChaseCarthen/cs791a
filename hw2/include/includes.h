#ifndef _X_H
#define _X_H

///CODE of x.h
#include <GL/glew.h>

#include "wx/wx.h"
#include "wx/sizer.h"
#include "wx/glcanvas.h"

// include OpenGL
#ifdef __WXMAC__
#include "OpenGL/glu.h"
#include "OpenGL/gl.h"
#else
//#include <GL/glu.h>
#include <GL/gl.h>

#endif
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <gishandler.h>
#include <math.h>
#include <glm/gtx/norm.hpp>
#include <iomanip>
#include <map>

//gdal
#include <gdal_priv.h>
#include <cpl_conv.h>
#include <ogrsf_frmts.h>

#endif
