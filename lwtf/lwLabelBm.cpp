#include "stdafx.h"
#include "lwLabelBM.h"
#include "lwRes.h"
#include "lwSprite.h"
#include "lwLog.h"
#include "lwFileSys.h"
#include "lwUtil.h"
#include <map>

namespace lw{

    class FontRes: public Res{
	public:
		static FontRes* create(const char* fntFile);
		
		const char* getFileName(){
			return _fileName.c_str();
		}
        
		struct CommonInfo{
			unsigned short lineHeight;
			unsigned short base;
			unsigned short scaleW;
			unsigned short scaleH;
			unsigned short pages;
			bool packed;
		};
		struct CharInfo{
			unsigned int id;
			unsigned short x;
			unsigned short y;
			unsigned short width;
			unsigned short height;
			short xoffset;
			short yoffset;
			short xadvance;
			unsigned char page;
			unsigned char chnl;
            int kerningIdx;
		};
        struct KerningInfo{
            int first;
            int second;
            int amount;
        };
		const std::map<wchar_t, CharInfo>& getCharInfoMap(){
			return _charInfoMap;
		}
		const CommonInfo& getCommonInfo(){
			return _commonInfo;
		}
		const std::vector<Sprite*> getSprites(){
			return _sprites;
		}
        
	private:
		FontRes(const char* fileName, bool& ok);
		~FontRes();
        
        int SkipWhiteSpace(std::string &str, int start);
        int FindEndOfToken(std::string &str, int start);
        void InterpretCommon(std::string &str, int start);
        void InterpretChar(std::string &str, int start);
        void InterpretPage(std::string &str, int start);
        void InterpretKerning(std::string &str, int start);
        void updateKerning();
        
		std::string _fileName;
		CommonInfo _commonInfo;
		
		std::map<wchar_t, CharInfo> _charInfoMap;
		std::vector<Sprite*> _sprites;
        std::vector<KerningInfo> _kernings;
	};
    
    namespace {
        std::map<std::string, FontRes*> _resMap;
    }
    
    FontRes* FontRes::create(const char* fntFile){
        std::map<std::string, FontRes*>::iterator it = _resMap.find(fntFile);
        if ( it == _resMap.end() ){
            bool ok = false;
            FontRes* p = new FontRes(fntFile, ok);
            if ( p && ok ){
                return p;
            }else if ( p ){
                delete p;
            }
            return NULL;
        }else{
            it->second->retain();
            return it->second;
        }
    }
    
    FontRes::FontRes(const char* fntFile, bool& ok){
        _fileName = fntFile;
        
		FILE* f = fopen(_f(fntFile), "rt");
		if ( f == NULL ){
			lwerror("fopen failed: fileName=" << fntFile);
			return;
		}
        
        std::string line;
        
        while( !feof(f) )
        {
            // Read until line feed (or EOF)
            line = "";
            line.reserve(256);
            while( !feof(f) )
            {
                char ch;
                if( fread(&ch, 1, 1, f) )
                {
                    if( ch != '\n' ) 
                        line += ch; 
                    else
                        break;
                }
            }
            
            // Skip white spaces
            int pos = SkipWhiteSpace(line, 0);
            
            // Read token
            int pos2 = FindEndOfToken(line, pos);
            std::string token = line.substr(pos, pos2-pos);
            
            // Interpret line
            if( token == "info" ){
                //InterpretInfo(line, pos2);
            }else if( token == "common" ){
                InterpretCommon(line, pos2);
            }else if( token == "char" ){
                InterpretChar(line, pos2);
            }else if( token == "kerning" ){
                InterpretKerning(line, pos2);
            }else if( token == "page" )
                InterpretPage(line, pos2);
        }
        updateKerning();
        
        fclose(f);
		ok = true;
    }
    
    FontRes::~FontRes(){
        std::vector<Sprite*>::iterator it = _sprites.begin();
        std::vector<Sprite*>::iterator itend = _sprites.end();
        for ( ; it != itend; ++it ){
            delete *it;
        }
        _resMap.erase(_fileName);
    }
    
    int FontRes::SkipWhiteSpace(std::string &str, int start){
        int n = start;
        while( n < str.size() ){
            char ch = str[n];
            if( ch != ' ' && 
               ch != '\t' && 
               ch != '\r' && 
               ch != '\n' )
                break;
            ++n;
        }
        return n;
    }
    
    int FontRes::FindEndOfToken(std::string &str, int start){
        int n = start;
        if( str[n] == '"' ){
            n++;
            while( n < str.size() ){
                char ch = str[n];
                if( ch == '"' ){
                    // Include the last quote char in the token
                    ++n;
                    break;
                }
                ++n;
            }
        }else{
            while( n < str.size() ){
                char ch = str[n];
                if( ch == ' ' ||
                   ch == '\t' ||
                   ch == '\r' ||
                   ch == '\n' ||
                   ch == '=' )
                    break;
                
                ++n;
            }
        }
        return n;
    }
    
    void FontRes::InterpretCommon(std::string &str, int start){
        // Read all attributes
        int pos, pos2 = start;
        while( true ){
            pos = SkipWhiteSpace(str, pos2);
            pos2 = FindEndOfToken(str, pos);
            
            std::string token = str.substr(pos, pos2-pos);
            
            pos = SkipWhiteSpace(str, pos2);
            if( pos == str.size() || str[pos] != '=' ) break;
            
            pos = SkipWhiteSpace(str, pos+1);
            pos2 = FindEndOfToken(str, pos);
            
            std::string value = str.substr(pos, pos2-pos);
            
            if( token == "lineHeight" )
                _commonInfo.lineHeight = (short)strtol(value.c_str(), 0, 10);
            else if( token == "base" )
                _commonInfo.base = (short)strtol(value.c_str(), 0, 10);
            else if( token == "scaleW" )
                _commonInfo.scaleW = (short)strtol(value.c_str(), 0, 10);
            else if( token == "scaleH" )
                _commonInfo.scaleH = (short)strtol(value.c_str(), 0, 10);
            else if( token == "pages" )
                _commonInfo.pages = strtol(value.c_str(), 0, 10);
            else if( token == "packed" )
                _commonInfo.packed = strtol(value.c_str(), 0, 10);
            
            if( pos == str.size() ) break;
        }
    }
    
    void FontRes::InterpretChar(std::string &str, int start){
        CharInfo info;
        info.kerningIdx = -1;
        int pos, pos2 = start;
        while( true ){
            pos = SkipWhiteSpace(str, pos2);
            pos2 = FindEndOfToken(str, pos);
            
            std::string token = str.substr(pos, pos2-pos);
            
            pos = SkipWhiteSpace(str, pos2);
            if( pos == str.size() || str[pos] != '=' ) break;
            
            pos = SkipWhiteSpace(str, pos+1);
            pos2 = FindEndOfToken(str, pos);
            
            std::string value = str.substr(pos, pos2-pos);
            
            if( token == "id" )
                info.id = strtol(value.c_str(), 0, 10);
            else if( token == "x" )
                info.x = strtol(value.c_str(), 0, 10);
            else if( token == "y" )
                info.y = strtol(value.c_str(), 0, 10);
            else if( token == "width" )
                info.width = strtol(value.c_str(), 0, 10);
            else if( token == "height" )
                info.height = strtol(value.c_str(), 0, 10);
            else if( token == "xoffset" )
                info.xoffset = strtol(value.c_str(), 0, 10);
            else if( token == "yoffset" )
                info.yoffset = strtol(value.c_str(), 0, 10);
            else if( token == "xadvance" )
                info.xadvance = strtol(value.c_str(), 0, 10);
            else if( token == "page" )
                info.page = strtol(value.c_str(), 0, 10);
            else if( token == "chnl" )
                info.chnl = strtol(value.c_str(), 0, 10);
            
            if( pos == str.size() ) break;
        }
        _charInfoMap[info.id] = info;
    }
    
    void FontRes::InterpretPage(std::string &str, int start){
        int id = 0;
        std::string file;
        
        // Read all attributes
        int pos, pos2 = start;
        while( true ){
            pos = SkipWhiteSpace(str, pos2);
            pos2 = FindEndOfToken(str, pos);
            
            std::string token = str.substr(pos, pos2-pos);
            
            pos = SkipWhiteSpace(str, pos2);
            if( pos == str.size() || str[pos] != '=' ) break;
            
            pos = SkipWhiteSpace(str, pos+1);
            pos2 = FindEndOfToken(str, pos);
            
            std::string value = str.substr(pos, pos2-pos);
            
            if( token == "id" )
                id = strtol(value.c_str(), 0, 10);
            else if( token == "file" )
                file = value.substr(1, value.length()-2);
            
            if( pos == str.size() ) break;
        }
        lw::Sprite *pSpt = lw::Sprite::create(file.c_str());
        lwassert(pSpt);
        _sprites.push_back(pSpt);
    }
    
    void FontRes::InterpretKerning(std::string &str, int start){
        // Read the attributes
        KerningInfo info;
        
        int pos, pos2 = start;
        while( true ){
            pos = SkipWhiteSpace(str, pos2);
            pos2 = FindEndOfToken(str, pos);
            
            std::string token = str.substr(pos, pos2-pos);
            
            pos = SkipWhiteSpace(str, pos2);
            if( pos == str.size() || str[pos] != '=' ) break;
            
            pos = SkipWhiteSpace(str, pos+1);
            pos2 = FindEndOfToken(str, pos);
            
            std::string value = str.substr(pos, pos2-pos);
            
            if( token == "first" )
                info.first = strtol(value.c_str(), 0, 10);
            else if( token == "second" )
               info.second = strtol(value.c_str(), 0, 10);
            else if( token == "amount" )
                info.amount = strtol(value.c_str(), 0, 10);
            
            if( pos == str.size() ) break;
        }
        _kernings.push_back(info);
    }
    
    void FontRes::updateKerning(){
        for ( int i = 0; i < _kernings.size(); ++i ){
            std::map<wchar_t, CharInfo>::iterator it = _charInfoMap.find(_kernings[i].first);
            if ( it != _charInfoMap.end() ){
                it->second.kerningIdx = i;
            }
        }
    }
    
    //=====================================================
    LabelBM* LabelBM::create(const char *fntFile){
        bool ok = false;
        LabelBM *p = new LabelBM(fntFile, ok);
        if ( p && ok ){
            return p;
        }else if ( p ){
            delete p;
        }
        return NULL;
    }
    
    LabelBM::LabelBM(const char *fntFile, bool &ok)
    :_posX(0), _posY(0), _scaleX(1.f), _scaleY(1.f), _rotate(0), _color(255, 255, 255, 255), _align(ALIGN_TOP_LEFT){
        _pRes = FontRes::create(fntFile);
        if ( !_pRes ){
            return;
        }
        ok = true;
    }
    
    LabelBM::~LabelBM(){
        if ( _pRes ){
            _pRes->release();
        }
    }
    
    void LabelBM::draw(){
        if ( _needUpdate ){
			update();
		}
		const wchar_t* text = _text.c_str();
		size_t len = _text.size();
		const wchar_t* p = text;
		float currX = _posX;
		if ( !_linesOffset.empty() ){
			currX += _linesOffset[0];
		}
		float currY = _y0;
        
		int currLine = 1;
		const FontRes::CommonInfo& comInfo = _pRes->getCommonInfo();
		const std::map<wchar_t, FontRes::CharInfo>& charInfoMap = _pRes->getCharInfoMap();
		const std::vector<Sprite*> sprites = _pRes->getSprites();
		while ( p < text+len ){
			if ( *p == '\n' ){
				currY += comInfo.lineHeight*_scaleY;
				currX = _posX;
				if ( currLine < (int)_linesOffset.size() ){
					currX += _linesOffset[currLine];
				}
				++currLine;
			}else{
				std::map<wchar_t, FontRes::CharInfo>::const_iterator it = charInfoMap.find(*p);
				if ( it == charInfoMap.end() ){
                    int n = *p;
                    lwinfo(n);
					it = charInfoMap.find(' ');
					if ( it == charInfoMap.end() ){
						currX += 3;
						++p;
						continue;
					}
				}
				const FontRes::CharInfo& charInfo = it->second;
				lwassert(charInfo.page < sprites.size());
                Sprite* pSprite = sprites[charInfo.page];
				pSprite->setUV(charInfo.x, charInfo.y, charInfo.width, charInfo.height);
				if ( _rotate == 0 ){
					pSprite->setPos((float)currX+charInfo.xoffset, (float)currY+charInfo.yoffset*_scaleY);
					pSprite->setScale(_scaleX, _scaleY);
					pSprite->setColor(_color);
					pSprite->setRotate(0);
					pSprite->draw();
				}else{
					cml::Vector2 v2;
					v2[0] = (float)currX+charInfo.xoffset-_posX;
					v2[1] = (float)currY+charInfo.yoffset-_posY;
					v2 = cml::rotate_vector_2D(v2, _rotate);
					pSprite->setPos(_posX+v2[0], _posY+v2[1]);
					pSprite->setScale(_scaleX, _scaleY);
					pSprite->setRotate(_rotate);
					pSprite->setColor(_color);
					pSprite->draw();
				}
				currX += charInfo.xadvance*_scaleX;
			}
			++p;
		}
    }
    
    void LabelBM::update(){
        _needUpdate = false;
        
		_width = 0.f;
		_height = 0.f;
		_linesOffset.clear();
        
        const std::map<wchar_t, FontRes::CharInfo>& charInfoMap = _pRes->getCharInfoMap();
		const FontRes::CommonInfo& comInfo = _pRes->getCommonInfo();
		size_t len = _text.length();
		const wchar_t* p = _text.c_str();
        
		float x = 0;
		int alignH = _align % 3;
		while ( 1 ){
			if ( *p == '\n' || p == _text.c_str()+len ){
				x = x*_scaleX;
				switch (alignH)
				{
                    case 0:
                        _linesOffset.push_back(0);
                        break;
                    case 1:
                        _linesOffset.push_back((-x)*.5f);
                        break;
                    case 2:
                        _linesOffset.push_back(-x);
                        break;
				}
				_width = std::max(_width, x);
				_height += comInfo.lineHeight;
				x = 0;
				if ( p == _text.c_str()+len ){
					break;
				}
			}else{
				std::map<wchar_t, FontRes::CharInfo>::const_iterator it = charInfoMap.find(*p);
				if ( it == charInfoMap.end() ){
					it = charInfoMap.find(' ');
					if ( it == charInfoMap.end() ){
						x += 10.f;
						++p;
						continue;
					}
				}
				const FontRes::CharInfo& charInfo = it->second;
				x += charInfo.xadvance;
			}
			++p;
		}
		int alignV = _align / 3;
		switch (alignV)
		{
            case 0:
                _y0 = _posY;
                break;
            case 1:
                _y0 = _posY-.5f*_height*_scaleY;
                break;
            case 2:
                _y0 = _posY-_height*_scaleY;
                break;
		}
    }
    
    void LabelBM::setPos(float x, float y){
        _needUpdate = true;
        _posX = x;
        _posY = y;
    }
    
    void LabelBM::setAlign(LabelAlign align){
        if ( _align != align ){
            _needUpdate = true;
            _align = align;
        }
    }
    
    void LabelBM::setText(const char *text){
        if ( text == NULL ){
            text = "";
        }
        UTF82W w(text);
        if ( _text.compare(w) != 0 ){
            _needUpdate = true;
            _text = w;
        }
    }
    
    void LabelBM::setRotate(float rotate){
        if ( _rotate != rotate ){
            _needUpdate = true;
            _rotate = rotate;
        }
    }
    
    void LabelBM::setScale(float scaleX, float scaleY){
        if ( _scaleX != scaleX || _scaleY != scaleY ){
            _needUpdate = true;
            _scaleX = scaleX;
            _scaleY = scaleY;
        }
    }
    
    void LabelBM::setColor(const lw::Color& color){
        _color = color;
    }
    
    const char* LabelBM::getText(){
        return W2UTF8(_text.c_str());
    }
    
    void LabelBM::getPos(float &x, float &y){
        x = _posX;
        y = _posY;
    }
    
    void LabelBM::getSize(float &w, float &h){
        w = _width;
        h = _height;
    }
    
	

} //namespace lw