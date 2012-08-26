#include "stdafx.h"
#include "lwtf/lwColor.h"

namespace lw{

	const Color COLOR_BLACK = Color((unsigned char)0, (unsigned char)0, (unsigned char)0, (unsigned char)255);
	const Color COLOR_WHITE = Color((unsigned char)255, (unsigned char)255, (unsigned char)255, (unsigned char)255);
	const Color COLOR_NONE = Color((unsigned char)0, (unsigned char)0, (unsigned char)0, (unsigned char)0);

    
	bool operator == (const Color& a, const Color& b){
		return ( a.a == b.a && a.r == b.r && a.g == b.g && a.b == b.b );
	}

	bool operator != (const Color& a, const Color& b){
		return !( a.a == b.a && a.r == b.r && a.g == b.g && a.b == b.b );
	}

	Color::Color(int c){
		a = (c&0xff000000)>>24;
		b = (c&0x00ff0000)>>16;
		g = (c&0x0000ff00)>>8;
		r = (c&0x000000ff);
	}
    void Color::set(int c){
        a = (c&0xff000000)>>24;
		b = (c&0x00ff0000)>>16;
		g = (c&0x0000ff00)>>8;
		r = (c&0x000000ff);
    }
    void Color::set(float rr, float gg, float bb, float aa){
        rr *= 255;
        gg *= 255;
        bb *= 255;
        aa *= 255;
        r = (unsigned char)std::max(std::min(rr, 255.f), 0.f);
        g = (unsigned char)std::max(std::min(gg, 255.f), 0.f);
        b = (unsigned char)std::max(std::min(bb, 255.f), 0.f);
        a = (unsigned char)std::max(std::min(aa, 255.f), 0.f);
    }
    void Color::set(int rr, int gg, int bb, int aa){
        r = (unsigned char)rr;
        g = (unsigned char)gg;
        b = (unsigned char)bb;
        a = (unsigned char)aa;
    }

	int Color::getInt(){
		int c = (a << 24) + (b << 16) +  (g << 8) + r;
		return c;
	}

	void rgb2Hsv(float R, float G, float B, float& H, float& S, float&V)
	{
		// r,g,b values are from 0 to 1
		// h = [0,360], s = [0,1], v = [0,1]
		// if s == 0, then h = -1 (undefined)

		float m1, m2, delta,tmp;
		tmp = std::min(R, G);
		m1 = std::min( tmp, B );
		tmp = std::max( R, G);
		m2 = std::max(tmp, B );
		V = m2; // v

		delta = m2 - m1;

		if( m2 != 0 )
			S = delta / m2; // s
		else
		{
			// r = g = b = 0 // s = 0, v is undefined
			S = 0;
			H = 0;
			return;
		}
		if( R == m2 )
			H = ( G - B ) / delta; // between yellow & magenta
		else if( G == m2 )
			H = 2 + ( B - R ) / delta; // between cyan & yellow
		else
			H = 4 + ( R - G ) / delta; // between magenta & cyan

		H *= 60; // degrees
		if( H < 0 )
			H += 360;
	}



	void hsv2Rgb(float H, float S, float V, float &R, float &G, float &B)
	{
		int i;
		float f, p, q, t;

		if( S == 0 ) 
		{
			// achromatic (grey)
			R = G = B = V;
			return;
		}

		H /= 60; // sector 0 to 5
		i = (int)H;
		f = H - i; // factorial part of h
		p = V * ( 1 - S );
		q = V * ( 1 - S * f );
		t = V * ( 1 - S * ( 1 - f ) );

		switch( i ) 
		{
		case 0: 
			R = V;
			G = t;
			B = p;
			break;
		case 1:
			R = q;
			G = V;
			B = p;
			break;
		case 2:
			R = p;
			G = V;
			B = t;
			break;
		case 3:
			R = p;
			G = q;
			B = V;
			break;
		case 4:
			R = t;
			G = p;
			B = V;
			break;
		default: // case 5:
			R = V;
			G = p;
			B = q;
			break;
		}
	}

} //namespace lw