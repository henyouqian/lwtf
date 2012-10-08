#include "stdafx.h"
#include "lwEffects.h"
#include "lwFileSys.h"
#include "tinyxml2.h"
#include <map>

namespace lw {
    namespace{
        std::map<std::string, EffectsRes*> _resMap;
        
        bool compileShader(GLuint &shader, GLenum type, const GLchar *source){
            shader = glCreateShader(type);
            glShaderSource(shader, 1, &source, NULL);
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
    
    EffectsRes* EffectsRes::create(const char *file){
        lwassert(file);
        std::map<std::string, EffectsRes*>::iterator it = _resMap.find(file);
        if ( it != _resMap.end() ){
            it->second->retain();
            return it->second;
        }
        bool ok = false;
        EffectsRes *p = new EffectsRes(file, ok);
        if ( p && !ok ){
            lwerror("new EffectsRes failed");
            delete p;
            return NULL;
        }
        _resMap[file] = p;
        return p;
    }
    
    EffectsRes::EffectsRes(const char *file, bool &ok){
        lwassert(file);
        _file = file;
        tinyxml2::XMLDocument doc;
        doc.LoadFile(_f(file));
        const tinyxml2::XMLElement *pElemLwfx = doc.RootElement();
        const tinyxml2::XMLElement *pElemVS = pElemLwfx->FirstChildElement("vs");
        lwassert(pElemVS);
        const char *vsSource = pElemVS->GetText();
        
        const tinyxml2::XMLElement *pElemFS = pElemLwfx->FirstChildElement("fs");
        lwassert(pElemFS);
        const char *fsSource = pElemFS->GetText();
        
        GLuint vertShader, fragShader;
        if ( !compileShader(vertShader, GL_VERTEX_SHADER, vsSource) ){
            ok = false;
            return;
        }
        if ( !compileShader(fragShader, GL_FRAGMENT_SHADER, fsSource) ){
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
        
        const const tinyxml2::XMLElement *pElemAttr = pElemLwfx->FirstChildElement("attribute");
        while (pElemAttr) {
            LocSmt locSmt;
            const char *name = pElemAttr->Attribute("name");
            const char *semantic = pElemAttr->Attribute("semantic");
            locSmt.location = glGetAttribLocation(_program, name);
            if ( locSmt.location == -1 ){
                lwerror("glGetAttribLocation failed");
            }else{
                if ( strcmp(semantic, "POSITION") == 0 ){
                    locSmt.semantic = POSITION;
                }else if ( strcmp(semantic, "NORMAL") == 0 ){
                    locSmt.semantic = NORMAL;
                }else if ( strcmp(semantic, "UV0") == 0 ){
                    locSmt.semantic = UV0;
                }else if ( strcmp(semantic, "UV1") == 0 ){
                    locSmt.semantic = UV1;
                }else if ( strcmp(semantic, "UV2") == 0 ){
                    locSmt.semantic = UV2;
                }else if ( strcmp(semantic, "UV3") == 0 ){
                    locSmt.semantic = UV3;
                }else{
                    locSmt.semantic = UNKNOWN;
                }
                if ( locSmt.semantic != UNKNOWN ){
                    _locSmts.push_back(locSmt);
                }
            }
            pElemAttr = pElemAttr->NextSiblingElement("attribute");
        }
        
        const const tinyxml2::XMLElement *pElemUniform = pElemLwfx->FirstChildElement("uniform");
        while (pElemUniform) {
            LocSmt locSmt;
            const char *name = pElemUniform->Attribute("name");
            const char *semantic = pElemUniform->Attribute("semantic");
            locSmt.location = glGetUniformLocation(_program, name);
            if ( locSmt.location == -1 ){
                lwerror("glGetUniformLocation failed");
            }else{
                if ( strcmp(semantic, "WORLDVIEW") == 0 ){
                    locSmt.semantic = WORLDVIEW;
                }else if ( strcmp(semantic, "WORLDVIEWPROJ") == 0 ){
                    locSmt.semantic = WORLDVIEWPROJ;
                }else if ( strcmp(semantic, "WORLDVIEWIT") == 0 ){
                    locSmt.semantic = WORLDVIEWIT;
                }else{
                    locSmt.semantic = UNKNOWN;
                }
                if ( locSmt.semantic != UNKNOWN ){
                    _locSmts.push_back(locSmt);
                }
            }
            pElemUniform = pElemUniform->NextSiblingElement("uniform");
        }
        
        ok = true;
    }
    
    EffectsRes::~EffectsRes(){
        if (_program) {
            glDeleteProgram(_program);
        }
        _resMap.erase(_file);
    }
    
    int EffectsRes::getLocationFromSemantic(Semantic semantic){
        std::vector<LocSmt>::iterator it = _locSmts.begin();
        std::vector<LocSmt>::iterator itend = _locSmts.end();
        for ( ; it != itend; ++it ){
            if ( it->semantic == semantic ){
                return it->location;
            }
        }
        lwerror("sementic not found: " << semantic);
        return -1;
    }
    
    int EffectsRes::getUniformLocation(const char* name){
        return glGetUniformLocation(_program, name);
    }
    
    const std::vector<EffectsRes::LocSmt>& EffectsRes::getLocSmts(){
        return _locSmts;
    }
    
    void EffectsRes::use(){
        glUseProgram(_program);
    }
}
