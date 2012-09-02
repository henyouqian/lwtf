#include "stdafx.h"
#include "lwUtil.h"
#include <CoreFoundation/cfDate.h>
#include <iostream>

namespace lw{

	void srand(){
        std::srand(CFAbsoluteTimeGetCurrent());
	}

} //namespace lw


W2UTF8::W2UTF8(const wchar_t* w){
	_str = [[NSString alloc]initWithBytes:w length:wcslen(w)*4 encoding:NSUTF32LittleEndianStringEncoding ];
}
W2UTF8::~W2UTF8(){
	[_str release];
}
int W2UTF8::size(){
	return [_str length];
}
const char* W2UTF8::data(){
	return [_str UTF8String];
}

W2UTF8::operator const char*(){
	return [_str UTF8String];
}

UTF82W::UTF82W(const char* c){
    _str = [[NSString alloc]initWithUTF8String: c];
}
UTF82W::~UTF82W(){
	[_str release];
}
int UTF82W::size(){
	return [_str length];
}
const wchar_t* UTF82W::data(){
	return (const wchar_t*)[_str cStringUsingEncoding:NSUTF32LittleEndianStringEncoding];
}

UTF82W:: operator const wchar_t*(){
	return (const wchar_t*)[_str cStringUsingEncoding:NSUTF32LittleEndianStringEncoding];
}
