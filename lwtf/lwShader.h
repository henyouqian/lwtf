#ifndef __LW_SHADER_H__
#define __LW_SHADER_H__

#include "lwtf/lwRes.h"
#include <OpenGLES/ES2/gl.h>

namespace lw {
    
    class ShaderProgramRes : public Res{
    public:
        static ShaderProgramRes* create(const char *vertFile, const char *fragFile, std::vector<std::string>& attribNames);
        
        int getUniformLocation(const char* name);
        int getAttribLocation(const char* name);
        void use();
        
    private:
        ShaderProgramRes(const char *vertFile, const char *fragFile, const char *key, std::vector<std::string>& attribNames, bool &ok);
        ~ShaderProgramRes();
        
        GLuint _program;
        std::string _key;
    };
	
} //namespace lw


#endif //__LW_SHADER_H__