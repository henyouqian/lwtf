#ifndef __LW_UTIL_H__
#define __LW_UTIL_H__

namespace lw{
	
	void srand();

} //namespace lw

class W2UTF8
{
public:
	W2UTF8(const wchar_t* w);
	~W2UTF8();
	int size();
	const char* data();
	operator const char*();

private:
    struct Data;
    Data *_p;
};

class UTF82W
{
public:
	UTF82W(const char* c);
	~UTF82W();
	int size();
	const wchar_t* data();
	operator const wchar_t*();

private:
    struct Data;
    Data *_p;
};



#endif //__LW_UTIL_H__