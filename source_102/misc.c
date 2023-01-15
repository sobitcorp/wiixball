/*===========================================
        WiiXBall v1.0.2
        by Kirill M	
							
		(c) KMS 2010-2011
        A DXBall Clone
============================================*/
#include "WiiXBall.h"


//MISC FUNCTIONS-------------------------------------------------------------------------------------------

u32 hsv2rgb(float h, float s, float v, u8 a){
	// H is given on [0, 6] or -1. S and V are given on [0, 1].
	float m, n, f;
	int i;
	if (h == -1)
		return RGBA(v*256, v*256, v*256, a);
	i = floor(h);
	f = h - i;
	if ( !(i&1) )
		f = 1 - f; // if i is even
	m = v * (1 - s);
	n = v * (1 - s * f);
	switch (i) {
		case 6:
		case 0: return RGBA(v*256, n*256, m*256, a);
		case 1: return RGBA(n*256, v*256, m*256, a);
		case 2: return RGBA(m*256, v*256, n*256, a);
		case 3: return RGBA(m*256, n*256, v*256, a);
		case 4: return RGBA(n*256, m*256, v*256, a);
		case 5: return RGBA(v*256, m*256, n*256, a);
	}
	return 0;
}

u32 blendColor(u32 col, u32 mask){
	u8 r, g, b, a;
	r = R(col) * (R(mask)/255.0);
	g = G(col) * (G(mask)/255.0);
	b = B(col) * (B(mask)/255.0);
	a = A(col) * (A(mask)/255.0);
	return RGBA(r,g,b,a);
}

u32 desaturate(u32 col){
	u8 r, g, b, a;
	//u8 avg = (R(col)+G(col)+B(col))/3;
	u8 avg = 255;
	r = (R(col) + avg) / 2;
	g = (G(col) + avg) / 2;
	b = (B(col) + avg) / 2;
	a = A(col);
	return RGBA(r,g,b,a);
}

int strabslen(char* s){
	int i=0;
	while(s[i] != '\0' && s[i] != ' ')
		i++;
	return i;
}

/**
 * This function calculates the number of frames we render each second.
 * It must be called right after GRRLIB_Render.
 * @return The number of frames per second.
 */
u8 CalculateFrameRate() {
    static u8 frameCount = 0;
    static u32 lastTime;
    static u8 FPS = 0;
    u32 currentTime = ticks_to_millisecs(gettime());

    frameCount++;
    if(currentTime - lastTime > 1000) {
        lastTime = currentTime;
        FPS = frameCount;
        frameCount = 0;
    }
    return FPS;
} 
