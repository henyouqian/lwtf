#ifndef __LW_SHADER_H__
#define __LW_SHADER_H__

#include "lwtf/lwRes.h"

namespace lw {
    
    class ShaderProgramRes : public Res{
    public:
        static ShaderProgramRes* create(const char *vertFile, const char *fragFile);
        
        int getUniformLocation(const char* name);
        int getAttribLocation(const char* name);
        void use();
        
    private:
        ShaderProgramRes(const char *vertFile, const char *fragFile, const char *key, bool &ok);
        ~ShaderProgramRes();
        
        GLuint _program;
        std::string _key;
    };
	
} //namespace lw


#endif //__LW_SHADER_H__