#include "lwHTTPClient.h"
#include "lwLog.h"

@interface HTTPDelegate : NSObject<NSURLConnectionDelegate> {
    lw::HTTPMsg *pMsg;
    bool dead;
}
- (id)initWithMsg:(lw::HTTPMsg*)p;
- (void)die;
@end

namespace lw{
    HTTPCallback _pHTTPCallback = NULL;
    
    void setHTTPCallback(HTTPCallback pCallback){
        _pHTTPCallback = pCallback;
    }
    
	HTTPMsg::HTTPMsg()
	:_isSent(false){
		
    }

	HTTPMsg::~HTTPMsg(){
        [(HTTPDelegate*)_pDelegate die];
		[(HTTPDelegate*)_pDelegate release];
	}

    void HTTPMsg::setURL(const char *url){
        _buff = url;
    }

	void HTTPMsg::send(){
        if ( _isSent ){
            return;
        }
        _isSent = true;
        NSString* urlString=[[NSString alloc] initWithUTF8String:_buff.c_str()];
        NSURL* url=[[NSURL alloc] initWithString:[urlString stringByAddingPercentEscapesUsingEncoding:NSUTF8StringEncoding]];
        NSURLRequest *theRequest=[NSURLRequest requestWithURL:url
                                                  cachePolicy:NSURLRequestUseProtocolCachePolicy
                                              timeoutInterval:60.0];
        
        [urlString release];
        [url release];
        HTTPDelegate* pCallback = [[HTTPDelegate alloc] initWithMsg:this];
        NSURLConnection *theConnection=[[NSURLConnection alloc] initWithRequest:theRequest delegate:pCallback];
        if (!theConnection) {
            onRespond(LWHTTPERR_CREATE);
            delete this;
        }
	}
	
} //namespace lw

@implementation HTTPDelegate
- (id)initWithMsg:(lw::HTTPMsg*)p{
    if ( self =[super init] ){
        pMsg=p;
        pMsg->_pDelegate=self;
        dead = false;
    }
    return self;
}

- (void)dealloc{
    [super dealloc];
}

-(void)die{
    dead = true;
}

- (void)connection:(NSURLConnection *)connection didReceiveResponse:(NSURLResponse *)response{
    if ( dead )
        return;
    pMsg->getBuff().clear();
}

- (void)connection:(NSURLConnection *)connection didReceiveData:(NSData *)data{
    if ( dead )
        return;
    pMsg->getBuff().append((char*)([data bytes]), [data length]);
}

- (void)connectionDidFinishLoading:(NSURLConnection *)connection{
    if ( dead ){
        [connection release];
        return;
    }
    if ( lw::_pHTTPCallback ){
        lw::_pHTTPCallback(LWHTTPERR_NONE);
    }
    pMsg->onRespond(LWHTTPERR_NONE);
    delete pMsg;
    [connection release];
}

- (void)connection:(NSURLConnection *)connection didFailWithError:(NSError *)error{
    lwerror("http error:" << [[error localizedDescription]UTF8String] << " from: " << pMsg->getBuff().c_str());
    if ( lw::_pHTTPCallback ){
        lw::_pHTTPCallback(LWHTTPERR_CREATE);
    }
    if ( !dead ){
        pMsg->onRespond(LWHTTPERR_CREATE);
        delete pMsg;
    }
    [connection release];
}

- (BOOL)connection:(NSURLConnection *)connection canAuthenticateAgainstProtectionSpace:(NSURLProtectionSpace *)protectionSpace {
    return [protectionSpace.authenticationMethod isEqualToString:NSURLAuthenticationMethodServerTrust];
}

- (void)connection:(NSURLConnection *)connection didReceiveAuthenticationChallenge:(NSURLAuthenticationChallenge *)challenge {
    if ([challenge.protectionSpace.authenticationMethod isEqualToString:NSURLAuthenticationMethodServerTrust])
        [challenge.sender useCredential:[NSURLCredential credentialForTrust:challenge.protectionSpace.serverTrust] forAuthenticationChallenge:challenge];
            
    
    [challenge.sender continueWithoutCredentialForAuthenticationChallenge:challenge];
}

@end