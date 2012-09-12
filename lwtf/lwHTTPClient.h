#ifndef __LW_HTTP_CLIENT_H__
#define __LW_HTTP_CLIENT_H__

#define LWHTTPERR_NONE 0
#define LWHTTPERR_CREATE -1
#define LWHTTPERR_TRANS -2

namespace lw{
	
	class HTTPMsg{
	public:
		HTTPMsg();
        virtual ~HTTPMsg();
		void send();
		virtual void onRespond(int error){}
        void* _pDelegate;

	protected:
        void setURL(const char* url);
		std::string _buff;
        bool _isSent;
        
    public:
        std::string& getBuff(){
            return _buff;
        }

	friend class HTTPClient;
	};

    typedef void (*HTTPCallback)(int);
    void setHTTPCallback(HTTPCallback pCallback);
    
} //namespace lw

#endif //__LW_HTTP_CLIENT_H__