/*===========================================
        WiiXBall v1.0.2
        by Kirill M	
							
		(c) KMS 2010-2011
        A DXBall Clone
============================================*/
#include "WiiXBall.h"

void loadData(){
	GBuf[0] = GRRLIB_CreateEmptyTexture(rmode->fbWidth, rmode->xfbHeight);
	GBuf[1] = GRRLIB_CreateEmptyTexture(rmode->fbWidth, rmode->xfbHeight);
	
	GFXCursor = GRRLIB_LoadTexturePNG(hcursor);
	GFXIntbg1 = GRRLIB_LoadTexturePNG(intbg1);
	GFXIntbg2 = GRRLIB_LoadTexturePNG(intbg2);
	GFXInttbg = GRRLIB_LoadTexturePNG(inttbg);
	GFXIntlogo[0] = GRRLIB_LoadTexturePNG(intlogo0);
	GFXIntlogo[1] = GRRLIB_LoadTexturePNG(intlogo1);
	GFXIntlogo[2] = GRRLIB_LoadTexturePNG(intlogo2);
	GFXIntlogo[3] = GRRLIB_LoadTexturePNG(intlogo3);
	GFXIntlogo[4] = GRRLIB_LoadTexturePNG(intlogo4);
	GFXIntlogo[5] = GRRLIB_LoadTexturePNG(intlogo5);
	GFXIntlogo[6] = GRRLIB_LoadTexturePNG(intlogo6);
	GFXIntlogo[7] = GRRLIB_LoadTexturePNG(intlogo7);
	GFXFscore = GRRLIB_LoadTexturePNG(fscore);
	GFXGball = GRRLIB_LoadTexturePNG(gball);
	GFXGbolt = GRRLIB_LoadTexturePNG(gbolt);
	GFXGbullet = GRRLIB_LoadTexturePNG(gbullet);
	GFXGexplo = GRRLIB_LoadTexturePNG(gexplo);
	GFXGfire = GRRLIB_LoadTexturePNG(gfire);
	GFXGhold = GRRLIB_LoadTexturePNG(ghold);
	GFXGlife = GRRLIB_LoadTexturePNG(glife);
	GFXGpad = GRRLIB_LoadTexturePNG(gpad);
	GFXGpower = GRRLIB_LoadTexturePNG(gpower);
	GFXGtile = GRRLIB_LoadTexturePNG(gtile);
	GFXGwall = GRRLIB_LoadTexturePNG(gwall);
	GFXHstar = GRRLIB_LoadTexturePNG(hstar);
	GFXHwin = GRRLIB_LoadTexturePNG(hwin);
	GFXHnewrec = GRRLIB_LoadTexturePNG(hnewrec);
	GRRLIB_InitTileSet(GFXIntbg1, 64, 160, 0);
	GRRLIB_InitTileSet(GFXFscore, 30, 30, '0');
	GRRLIB_InitTileSet(GFXGexplo, 64, 64, 0);
	GRRLIB_InitTileSet(GFXGfire, 64, 64, 0);
	GRRLIB_InitTileSet(GFXGhold, 132, 15, 0);
	GRRLIB_InitTileSet(GFXGpad, 296, 30, 0);
	GRRLIB_InitTileSet(GFXGpower, 32, 32, 0);
	GRRLIB_InitTileSet(GFXGtile, 30, 15, 0);
	GRRLIB_InitTileSet(GFXGwall, 20, 480, 0);
	
	sExp[0] = sExp1;
	sExp[1] = sExp2;
	sExp[2] = sExp3;
	sExp_size[0] = sExp1_size;
	sExp_size[1] = sExp2_size;
	sExp_size[2] = sExp3_size;
	
	initFont(FONT_SYS, sysfont, 8, 16, 32);
	initFont(FONT_8BIT, kfont, 16, 16, 32);
	initFont(FONT_GR, fontgr, 16, 18, 32);
}

void loadSets(){
	int i,j;
	char* s;
	FILE* file;
	game.pcolor[0] = 0xFFAAAAFF;
	game.pcolor[1] = 0xAAAAFFFF;
	game.pcolor[2] = 0xAAFFAAFF;
	game.pcolor[3] = 0xFFFFAAFF;
	
	//menu buttons
	makeButton(0, 320, 280, 288, 46, 10, 1, 1.8, FONT_GR, "New Game", 0x10FF10FF, 0xBFBFFFFF, 0x555510FF, 0xBF);
	makeButton(1, 320, 340, 288, 46, 10, 1, 1.8, FONT_GR, "Options", 0x10FF10FF, 0xBFBFFFFF, 0x555510FF, 0xBF);
	makeButton(2, 320, 400, 288, 46, 10, 1, 1.8, FONT_GR, "Exit", 0x10FF10FF, 0xBFBFFFFF, 0x555510FF, 0xBF);
	
	//hiscore control buttons
	makeButton(4, 120+80*5, 172+64*4, 144, 36, 4, 1, 1.2, FONT_GR, "Done", 0x10FF10FF, 0xBFBFFFFF, 0x555510FF, 0x22000AF);
	makeButton(5, 128+192*0, 220, 160, 32, 4, 1, 1, FONT_GR, "UPPER", 0x10FF10FF, 0xBFBFFFFF, 0x555510FF, 0x22000AF);
	makeButton(6, 128+192*1, 220, 160, 32, 4, 1, 1, FONT_GR, "lower", 0x10FF10FF, 0xBFBFFFFF, 0x555510FF, 0x22000AF);
	makeButton(7, 128+192*2, 220, 160, 32, 4, 1, 1, FONT_GR, "Other", 0x10FF10FF, 0xBFBFFFFF, 0x555510FF, 0x22000AF);
	
	//A-Z
	s = strdup("A");
	for(i=8; i<34; i++){
		makeButton(i, 80+80*((i-8)%7), 272+52*((i-8)/7), 64, 36, 4, 1, 1.4, FONT_GR, strdup(s), 0x10FF10FF, 0xBFBFFFFF, 0x555510FF, 0x22000AF);
		s[0]++;
	}
	//a-z
	s = strdup("a");
	for(i=34; i<60; i++){
		makeButton(i, 80+80*((i-34)%7), 272+52*((i-34)/7), 64, 36, 4, 1, 1.4, FONT_GR, strdup(s), 0x10FF10FF, 0xBFBFFFFF, 0x555510FF, 0x22000AF);
		s[0]++;
	}
	//!-', *-9, @, ^, ~
	s = strdup("!");
	for(i=60; i<67; i++){
		makeButton(i, 80+80*((i-60)%7), 272+52*((i-60)/7), 64, 36, 4, 1, 1.4, FONT_GR, strdup(s), 0x10FF10FF, 0xBFBFFFFF, 0x555510FF, 0x22000AF);
		s[0]++;
	}
	s = strdup("*");
	for(i=67; i<83; i++){
		makeButton(i, 80+80*((i-60)%7), 272+52*((i-60)/7), 64, 36, 4, 1, 1.4, FONT_GR, strdup(s), 0x10FF10FF, 0xBFBFFFFF, 0x555510FF, 0x22000AF);
		s[0]++;
	}
	makeButton(83, 80+80*((83-60)%7), 272+52*((83-60)/7), 64, 36, 4, 1, 1.4, FONT_GR, "@", 0x10FF10FF, 0xBFBFFFFF, 0x555510FF, 0x22000AF);
	makeButton(84, 80+80*((84-60)%7), 272+52*((84-60)/7), 64, 36, 4, 1, 1.4, FONT_GR, "^", 0x10FF10FF, 0xBFBFFFFF, 0x555510FF, 0x22000AF);
	makeButton(85, 80+80*((85-60)%7), 272+52*((85-60)/7), 64, 36, 4, 1, 1.4, FONT_GR, "~", 0x10FF10FF, 0xBFBFFFFF, 0x555510FF, 0x22000AF);
	
	//pausemenu buttons
	makeButton(86, 290-90, 380, 180, 46, 8, 1, 1.2, FONT_GR, "Continue", 0x10FF10FF, 0xBFBFFFFF, 0x555510FF, 0x6F);
	makeButton(87, 350+90, 380, 180, 46, 8, 1, 1.2, FONT_GR, "Quit", 0x10FF10FF, 0xBFBFFFFF, 0x555510FF, 0x6F);
	
	//options buttons
	makeButton(88, 536, 428, 128, 64, 10, 1, 2, FONT_GR, "OK", 0x10FF10FF, 0xBFBFFFFF, 0x555510FF, 0x6F);
	makeButton(89, 140, 444, 200, 32, 10, 1, 1.2, FONT_8BIT, "Credits", 0x10FF10FF, 0xBFBFFFFF, 0x555510FF, 0x6F);
	makeButton(90, 280-64, 140, 128, 46, 10, 1, 1.7, FONT_GR, "CRT", 0x558810FF, 0xBFBFFFFF, 0xFF4444FF, 0x6F);
	makeButton(91, 360+64, 140, 128, 46, 10, 1, 1.7, FONT_GR, "LCD", 0x558810FF, 0xBFBFFFFF, 0xFF4444FF, 0x6F);
	makeButton(92, 280-64, 250, 128, 46, 10, 1, 1.7, FONT_GR, "On", 0x558810FF, 0xBFBFFFFF, 0xFF4444FF, 0x6F);
	makeButton(93, 360+64, 250, 128, 46, 10, 1, 1.7, FONT_GR, "Off", 0x558810FF, 0xBFBFFFFF, 0xFF4444FF, 0x6F);
	makeButton(94, 280-64, 360, 128, 46, 10, 1, 1.7, FONT_GR, "On", 0x558810FF, 0xBFBFFFFF, 0xFF4444FF, 0x6F);
	makeButton(95, 360+64, 360, 128, 46, 10, 1, 1.7, FONT_GR, "Off", 0x558810FF, 0xBFBFFFFF, 0xFF4444FF, 0x6F);
	
	for(i = 0; i<8; i++)
		game.button[i].state = 0;

	game.boardset = (char*)bDefault;
	game.boardcount = bDefault_size/(20*20);
	
	file = fopen("wxbhigh.dat", "rb");
	if(file == NULL){
		s = strdup("*_www.KMSZone.com_* ");
		for(i=0; i<HSRECORDS; i++){
			game.high[i].level = HSRECORDS-i;
			for(j=0; j<HSNAMELEN; j++)
				game.high[i].name[j] = s[j];
			game.high[i].score = (HSRECORDS-i)*1600;
		}
		file = fopen("wxbhigh.dat", "wb");
		if(file != NULL){
			fwrite((char*)game.high, 1, sizeof(kRecord)*16, file);
			fclose(file);
			file = fopen("wxbhigh.dat", "rb");
		}
	}
	if(file != NULL){
		fread(game.high, 1, sizeof(kRecord)*16, file);
		fclose(file);
	}
	
	file = fopen("wxbopts.dat", "rb");
	if(file == NULL){
		makeOptsFile();
		file = fopen("wxbopts.dat", "rb");
	}
	if(file != NULL){
		fread(&game.opts.version, 1, sizeof(int), file);
		if(game.opts.version != OPTSVERSION){
			fclose(file);
			makeOptsFile();
			file = fopen("wxbopts.dat", "rb");
		}
		else
			rewind(file);
		if(file != NULL){
			fread(&game.opts, 1, sizeof(kOptions), file);
			fclose(file);
		}
	}
}

void makeOptsFile(){
	game.opts.version = OPTSVERSION;
	game.opts.drawMode = 0;
	game.opts.rumble = 0;
	game.opts.wsound = 1;
	saveOpts();
}

void saveOpts(){
	FILE* file;
	file = fopen("wxbopts.dat", "wb");
	if(file == NULL)
		return;
	fwrite(&game.opts, 1, sizeof(kOptions), file);
	fclose(file);
}

void initFont(int num, const unsigned char* img, int sx, int sy, char startchar){
	game.gfont[num].img = GRRLIB_LoadTexturePNG(img);
	GRRLIB_InitTileSet(game.gfont[num].img, sx, sy, startchar); 
	game.gfont[num].sx = sx;
	game.gfont[num].sy = sy;
}

void makeButton(int num, int x, int y, int w, int h, int border, float scale, float fontscale, int fnt, char* text, u32 color, u32 oncolor, u32 downcolor, u32 bgcolor){
	game.button[num].x = x;
	game.button[num].y = y;
	game.button[num].w = w;
	game.button[num].h = h;
	game.button[num].border = border;
	game.button[num].scale = scale;
	game.button[num].fontscale = fontscale;
	game.button[num].bfont = fnt;
	game.button[num].text = text;
	game.button[num].color[0] = color;
	game.button[num].color[1] = oncolor;
	game.button[num].color[2] = downcolor;
	game.button[num].bgcolor = bgcolor;
	game.button[num].state = -1;
}
