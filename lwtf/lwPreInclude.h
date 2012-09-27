#ifndef __LW_PREINCLUDE_H__
#define __LW_PREINCLUDE_H__

#ifdef __cplusplus
    #include "cml/cml.h"
    namespace cml{
        typedef matrix44f_c Matrix4;
        typedef matrix33f_c Matrix3;
        typedef matrix22f_c Matrix2;
        typedef vector2f Vector2;
        typedef vector3f Vector3;
        typedef vector4f Vector4;
    }
    #include <list>
    #include <string>
    #include <OpenGLES/ES2/gl.h>

#endif //__cplusplus


#endif //__LW_PREINCLUDE_H__