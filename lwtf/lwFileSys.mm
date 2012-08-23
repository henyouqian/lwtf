#include "lwFileSys.h"

_f::_f(const char* filename){
	NSString* str = [[NSString alloc] initWithUTF8String:filename];
	NSString* path = [[NSBundle mainBundle] pathForResource:str ofType:nil];
    [str release];
	if ( path == nil ){
		_isValid = false;
	}else{
		_pathStr = [path UTF8String];
        _isValid = true;
	}
}

_fdoc::_fdoc(const char* filename){
    NSArray* paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString* documentsDirectory = [paths objectAtIndex:0];
    _pathStr = [documentsDirectory UTF8String];
    _pathStr.append("/");
    _pathStr.append(filename);
    
	FILE* pf = fopen(_pathStr.c_str(), "rb");
	if ( pf == NULL ){
        _isValid = false;
    }else{
        fclose(pf);
        _isValid = true;
    }
}


namespace lw {
	const char* getDocDir(){
		static std::string docDir;
		if ( docDir.empty() ){
			NSArray* paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
			NSString* documentsDirectory = [paths objectAtIndex:0];
			docDir = [documentsDirectory UTF8String];
		}
		return docDir.c_str();
	}
}

