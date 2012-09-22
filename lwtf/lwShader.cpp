#include "lwShader.h"
#include "lwRes.h"
#include "lwFileSys.h"
#include "lwLog.h"
#include <map>

namespace lw {
    
    namespace {
        std::map<std::string, ShaderProgramRes*> _shaderProgamResMap;
    }
    
    ShaderProgramRes* ShaderProgramRes::create(const char *vertFile, const char *fragFile){
        bool ok = false;
        
        std::string key = vertFile;
        key.append(fragFile);
        std::map<std::string, ShaderProgramRes*>::iterator it = _shaderProgamResMap.find(key);
        if ( it != _shaderProgamResMap.end() ){
            it->second->retain();
            return it->second;
        }
        ShaderProgramRes* p = new ShaderProgramRes(vertFile, fragFile, key.c_str(), ok);
        if ( p ){
            if ( ok ){
                _shaderProgamResMap[key] = p;
                return p;
            }else{
                delete p;
                return NULL;
            }
        }
        return NULL;
    }
    
    namespace {
        bool compileShader(GLuint &shader, GLenum type, const char *file){
            shader = glCreateShader(type);
            FILE* pf = fopen(_f(file), "rt");
            lwassert(pf);
            fseek(pf, 0, SEEK_END);
            int len = ftell(pf);
            char *buf = new char[len+1];
            fseek(pf, 0, SEEK_SET);
            fread(buf, len, 1, pf);
            buf[len] = 0;
            //lwinfo(buf);
            const GLchar* source = buf;
            glShaderSource(shader, 1, &source, NULL);
            fclose(pf);
            delete [] buf;
            
            glCompileShader(shader);
            
#if defined(DEBUG)
            GLint logLength;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
            if (logLength > 0) {
                GLchar *log = (GLchar *)malloc(logLength);
                glGetShaderInfoLog(shader, logLength, &logLength, log);
                lwerror(log);
                free(log);
            }
#endif
            
            GLint status;
            glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
            if (status == 0) {
                glDeleteShader(shader);
                return false;
            }
            
            return true;
        }
        
        bool linkProgram(GLuint prog){
            GLint status;
            glLinkProgram(prog);
            glGetProgramiv(prog, GL_LINK_STATUS, &status);
            if (status == 0) {
                return false;
            }
            
            return true;
        }
    }
    
    ShaderProgramRes::ShaderProgramRes(const char *vertFile, const char *fragFile, const char *key, bool &ok){
        ok = false;
        _program = 0;
        _key = key;
        
        GLuint vertShader, fragShader;
        if ( !compileShader(vertShader, GL_VERTEX_SHADER, vertFile) ){
            ok = false;
            return;
        }
        if ( !compileShader(fragShader, GL_FRAGMENT_SHADER, fragFile) ){
            ok = false;
            return;
        }
        
        _program = glCreateProgram();
        glAttachShader(_program, vertShader);
        glAttachShader(_program, fragShader);
        
        if (!linkProgram(_program)) {
            lwerror("Failed to link program: " << _program);
            
            if (vertShader) {
                glDeleteShader(vertShader);
                vertShader = 0;
            }
            if (fragShader) {
                glDeleteShader(fragShader);
                fragShader = 0;
            }
            
            ok = false;
            return;
        }
        
        if (vertShader) {
            glDetachShader(_program, vertShader);
            glDeleteShader(vertShader);
        }
        if (fragShader) {
            glDetachShader(_program, fragShader);
            glDeleteShader(fragShader);
        }
        ok = true;
    }
    
    ShaderProgramRes::~ShaderProgramRes(){
        if (_program) {
            glDeleteProgram(_program);
        }
        _shaderProgamResMap.erase(_key);
    }
    
    int ShaderProgramRes::getUniformLocation(const char* name){
        return glGetUniformLocation(_program, name);
    }
    
    int ShaderProgramRes::getAttribLocation(const char* name){
        return glGetAttribLocation(_program, name);
    }
    
    void ShaderProgramRes::use(){
        glUseProgram(_program);
    }
	
} //namespace lw

