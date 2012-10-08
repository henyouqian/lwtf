#ifndef __LW_EFFECTS_H__
#define __LW_EFFECTS_H__

#include "lwtf/lwRes.h"

namespace lw {
    
    class EffectsRes : public Res{
    public:
        enum Semantic{
            UNKNOWN,
            POSITION,
            NORMAL,
            UV0,
            UV1,
            UV2,
            UV3,
            WORLDVIEW,
            WORLDVIEWPROJ,
            WORLDVIEWIT,
        };
        
        struct LocSmt{
            int location;
            Semantic semantic;
        };
        
        static EffectsRes* create(const char *file);
        int getLocationFromSemantic(EffectsRes::Semantic semantic);
        int getUniformLocation(const char* name);
        void use();
        
        const std::vector<EffectsRes::LocSmt>& getLocSmts();
        
    private:
        EffectsRes(const char *file, bool &ok);
        ~EffectsRes();
        
        std::string _file;
        GLuint _program;
        std::vector<LocSmt> _locSmts;
    };
    
    
}//namespace lw

#endif //__LW_EFFECTS_H__