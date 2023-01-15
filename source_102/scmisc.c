/*===========================================
        WiiXBall v1.0.2
        by Kirill M	
							
		(c) KMS 2010-2011
        A DXBall Clone
============================================*/
#include "WiiXBall.h"


//VIDEO EFFECTS & HELPER FUNCTIONS-----------------------------------------------------------------------------------------------

void scFX(){
	int curBuf = 0;
	GRRLIB_Screen2Texture(0,0,GBuf[curBuf],false);
	GRRLIB_FillScreen(0xFF);
	if(game.pow.flip == 1){
		GRRLIB_BMFX_FlipV(GBuf[curBuf], GBuf[curBuf^1]);
		curBuf ^= 1;
	}
	if(game.pow.pixelate > 5){
		GRRLIB_BMFX_Pixelate(GBuf[curBuf], GBuf[curBuf^1], game.pow.pixelate);
		GRRLIB_ClearTex(GBuf[curBuf]);
		curBuf ^= 1;
		//GRRLIB_BMFX_Scatter(GBuf[1], GBuf[0], game.pow.pixelate/4 + rand()%200/100.0); //too slow
	}
	if(game.opts.drawMode)
		GRRLIB_DrawImg(0,0,GBuf[curBuf],0,1,1,0xFFFFFFFF);
	else{
		GRRLIB_DrawImg(7,22,GBuf[curBuf],0,.98,.92,0xFFFFFFFF);
		GRRLIB_Line(0,464,640,464,0xFF); //cover some graphical glitches from screen2textrure function
	}
}

void drawButton(kButton b){
	if(b.state < 0)
		return;
	int sw = b.w * b.scale;
	int sh = b.h * b.scale;
	int sx = b.x - sw / 2.0;
	int sy = b.y - sh / 2.0;
	GRRLIB_Rectangle(b.x - (b.w + b.border*2)*b.scale/2.0, b.y - (b.h + b.border*2)*b.scale/2.0, (b.w + b.border*2)*b.scale, (b.h + b.border*2)*b.scale, b.bgcolor, true);
	GRRLIB_Rectangle(sx, sy, sw, sh, blendColor(b.color[b.state], 0xFFFFFFFF), false);
	GRRLIB_Rectangle(sx+1, sy+1, sw-2, sh-2, blendColor(b.color[b.state], 0xFFFFFFFF), false);
	GRRLIB_Rectangle(sx+2, sy+2, sw-4, sh-4, blendColor(b.color[b.state], 0x888888FF), false);
	GRRLIB_Rectangle(sx+3, sy+3, sw-6, sh-6, blendColor(b.color[b.state], 0x888888FF), false);
	GRRLIB_Rectangle(sx, sy+sh-1, sw, 1, blendColor(b.color[b.state], 0x444444FF), false);
	GRRLIB_Rectangle(sx+sw-1, sy, 1, sh, blendColor(b.color[b.state], 0x444444FF), false);
	GRRLIB_Printf(sx + sw/2.0 - (strlen(b.text)*game.gfont[b.bfont].sx*b.scale*b.fontscale)/2.0, sy + sh/2.0 - (game.gfont[b.bfont].sy*b.scale*b.fontscale)/2.0, game.gfont[b.bfont].img, b.color[b.state], b.scale*b.fontscale, b.text);
}

void showKeys(int from, int to){
	int i;
	for(i=8; i<86; i++)
		game.button[i].state = -1;
	for(i=from; i<to; i++)
		game.button[i].state = 0;
}

void printLine(int* y, int fnt, char* text, int yspace, float zoom, u32 color){
	GRRLIB_Printf(320-game.gfont[fnt].sx*strlen(text)*zoom/2.0, *y, game.gfont[fnt].img, color, zoom, "%s", text);
	*y += game.gfont[fnt].sy*zoom + yspace;
}
