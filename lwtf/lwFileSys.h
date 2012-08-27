#ifndef __LW_FILE_SYS_H__
#define __LW_FILE_SYS_H__

namespace lw{
    
	const char* getDocDir();
	
} //namespace lw


class _f
{
public:
	_f(const char* filename);
	const char* getPath(){
		return _pathStr.c_str();
	}
	operator const char*(){
		return _pathStr.c_str();
	}
	bool isValid(){
		return _isValid;
	}

private:
	std::string _pathStr;
    bool _isValid;
};

class _fdoc{
public:
	_fdoc(const char* filename);
	const char* getPath(){
		return _pathStr.c_str();
	}
	operator const char*(){
		return _pathStr.c_str();
	}
	bool isValid(){
		return _isValid;
	}
    
private:
	std::string _pathStr;
    bool _isValid;
};


#endif //__LW_FILE_SYS_H__