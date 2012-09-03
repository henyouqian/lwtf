#include "stdafx.h"
#include "lwUtil.h"
#include <CoreFoundation/cfDate.h>
#include <iostream>

namespace lw{

	void srand(){
        std::srand(CFAbsoluteTimeGetCurrent());
	}

} //namespace lw

struct W2UTF8::Data{
    NSString* _str;
};

W2UTF8::W2UTF8(const wchar_t* w){
    _p = new Data;
	_p->_str = [[NSString alloc]initWithBytes:w length:wcslen(w)*4 encoding:NSUTF32LittleEndianStringEncoding ];
}
W2UTF8::~W2UTF8(){
	[_p->_str release];
    delete _p;
}
int W2UTF8::size(){
	return [_p->_str length];
}
const char* W2UTF8::data(){
	return [_p->_str UTF8String];
}

W2UTF8::operator const char*(){
	return [_p->_str UTF8String];
}

struct UTF82W::Data{
    NSString* _str;
};

UTF82W::UTF82W(const char* c){
    _p = new Data;
    _p->_str = [[NSString alloc]initWithUTF8String: c];
}
UTF82W::~UTF82W(){
    [_p->_str release];
    delete _p;
}
int UTF82W::size(){
	return [_p->_str length];
}
const wchar_t* UTF82W::data(){
	return (const wchar_t*)[_p->_str cStringUsingEncoding:NSUTF32LittleEndianStringEncoding];
}

UTF82W:: operator const wchar_t*(){
	return (const wchar_t*)[_p->_str cStringUsingEncoding:NSUTF32LittleEndianStringEncoding];
}
