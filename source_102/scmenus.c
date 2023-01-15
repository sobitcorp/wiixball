/*===========================================
        WiiXBall v1.0.2
        by Kirill M	
							
		(c) KMS 2010-2011
        A DXBall Clone
============================================*/
#include "WiiXBall.h"


//PREGAME DRAWING AND CONTROL FUNCTIONS--------------------------------------------------------------------

void ctGlobal(){
	int i;
	for(i = 0; i < 4; i++){
		if(game.mouse[i].enable){
			if(wd[i]->btns_h & WPAD_BUTTON_MINUS){
				game.mouse[i].dctr++;
				if(game.mouse[i].dctr > 60*3){
					game.mouse[i].enable = 0;
					WPAD_SetLeds(i, 0);
					game.mouse[i].dctr = 0;
				}
			}
			else if(wd[i]->btns_u & WPAD_BUTTON_MINUS)
				game.mouse[i].dctr = 0;
			if(wd[i]->btns_d & WPAD_BUTTON_HOME)
				escape(0);
		}
		else if(wd[i]->btns_d & (WPAD_BUTTON_2 | WPAD_BUTTON_1 | WPAD_BUTTON_B | WPAD_BUTTON_A | WPAD_BUTTON_PLUS | WPAD_BUTTON_LEFT | WPAD_BUTTON_RIGHT | WPAD_BUTTON_DOWN | WPAD_BUTTON_UP)){
				game.mouse[i].enable = 1;
				WPAD_SetLeds(i, WIIMOTE_LED_1 << i);
		}
	}
}

void scLogo(){
	if(sub < 255)
		GRRLIB_DrawImg( 0, 0, GFXLogo, 0, 1, 1, RGBA(255, 255, 255, sub) );
	else if(sub == 255){
		loadData();
		loadSets();
		GRRLIB_DrawImg( 0, 0, GFXLogo, 0, 1, 1, 0xFFFFFFFF);
	}
	else if(sub <= 256 + 128)
		GRRLIB_DrawImg( 0, 0, GFXLogo, 0, 1, 1, 0xFFFFFFFF);
	else
		GRRLIB_DrawImg( 0, 0, GFXLogo, 0, 1, 1, RGBA(255, 255, 255, (512-sub) * 2) );
	sub++;
	if(sub > 512){
		screen = 1;
		sub = 0;
	}
}

void scIntro(){
	int i;
	GRRLIB_SetBlend(GRRLIB_BLEND_ALPHA);
	for(i=0; i<4; i++)
		GRRLIB_DrawTile(i*256 - bpos%256, 0, GFXIntbg1, 0, 4, 4, 0xFFFFFFBF,((bpos/256)+i)%4);
	GRRLIB_DrawImg( 0, 0, GFXIntbg2, 0, 2, 2, 0xFFFFFF60 );
	GRRLIB_SetBlend(GRRLIB_BLEND_MULTI);
	GRRLIB_DrawImg( 80, 60, GFXInttbg, 0, 1, 1, 0xFFFFFFFF);
	GRRLIB_SetBlend(GRRLIB_BLEND_ALPHA);
	GRRLIB_DrawImg( 80, 60, GFXIntlogo[(bpos/16)%8], 0, 1, 1, 0xFFFFFFFF);
	bpos = (bpos + 8)%1024;
	if(sub == 0){
		GRRLIB_FillScreen(0xFF);
		if(stopgame){
			screen = -1;
			escape(0);
			return;
		}
		sub++;
		PlayOgg(mem_open((char*)OGint1, OGint1_size), 0, OGG_ONE_TIME);
		SetVolumeOgg(255);
	}
	else if(sub < 64)
		GRRLIB_FillScreen(256-(sub++)*4);
	else if(sub == 64 + 64){
		GRRLIB_FillScreen(0xFF);
		StopOgg();
		screen = 4;
		sub = -2;
	}
	else if(sub == 64 + 64 + 32){
		GRRLIB_FillScreen(0x80);
		screen = 2; //options
		sub = 0;
	}
	else if(sub > 64+64+32 && sub < 64+64*2){
		GRRLIB_FillScreen(255-(sub%64)*4);
		SetVolumeOgg((sub++%64) * 4);
	}
	else if(sub == 64+64*2)
		sub = 64;
	else if(sub == 64 + 64 * 3){
		GRRLIB_FillScreen(0xFF);
		screen = -1;
		escape(0);
	}
	else if(sub > 64){
		GRRLIB_FillScreen((sub%64) * 4);
		SetVolumeOgg(256-(sub++%64) * 4);
	}
	else{
		for(i=0; i<3; i++)
			drawButton(game.button[i]);
		for(i=3; i>=0; i--)
			if(game.mouse[i].x >= 0 && game.mouse[i].y >= 0)
				GRRLIB_DrawImg(game.mouse[i].x + 67.88*sin(deg2rad * (game.mouse[i].rotDeg-45)), game.mouse[i].y - 67.88*cos(deg2rad * (game.mouse[i].rotDeg-45)), GFXCursor, game.mouse[i].rotDeg, 1, 1, game.pcolor[i]);		
	}
	if(GetTimeOgg() > 70520)
		SetTimeOgg(2022);
}

void ctIntro(){
	int i, j;
	char btSel[3] = {0,0,0};
	updMice();
	if(sub!=64)
		return;
	for(i = 0; i < 3; i++){
		for(j=0; j<4; j++){
			if(game.button[i].state > -1 && game.mouse[j].x > game.button[i].x - game.button[i].w/2.0*game.button[i].scale && game.mouse[j].y > game.button[i].y - game.button[i].h/2.0*game.button[i].scale && game.mouse[j].x < game.button[i].x + game.button[i].w/2.0*game.button[i].scale && game.mouse[j].y < game.button[i].y + game.button[i].h/2.0*game.button[i].scale){
				if(game.button[i].state == 0){
					game.button[i].state = 1;
					game.button[i].scale = 1.2;
					//game.button[i].x = 320 - (game.button[i].img->w * game.button[i].scale) / 2;
					//game.button[i].y = (280 + 60*i) - (game.button[i].img->h * game.button[i].scale) / 2;
					game.mouse[j].rumble = 4;
				}
				btSel[i] = 1;
				if(wd[j]->btns_d & WPAD_BUTTON_A)
					game.mouse[j].rumble = 5;
				if(wd[j]->btns_h & WPAD_BUTTON_A){
					game.button[i].state = 2;
					game.button[i].scale = .95;
				}
				if(wd[j]->btns_u & WPAD_BUTTON_A){
					sub = 65 + i*64;
					game.mouse[j].rumble = 20;
				}
			}
		}
		if(game.button[i].state > 0 && btSel[i] == 0){
			game.button[i].state = 0;
			game.button[i].scale = 1;
			//game.button[i].x = 320 - (game.button[i].img->w * game.button[i].scale) / 2;
			//game.button[i].y = (280 + 60*i) - (game.button[i].img->h * game.button[i].scale) / 2;
			
		}
	}
}

void scOpts(){
	int i;
	if(stopgame)
		sub = 999;
	for(i=0; i<4; i++)
		GRRLIB_DrawTile(i*256 - bpos%256, 0, GFXIntbg1, 0, 4, 4, 0xFFFFFFBF,((bpos/256)+i)%4);
	GRRLIB_DrawImg( 0, 0, GFXIntbg2, 0, 2, 2, 0xFFFFFF60 );
	GRRLIB_FillScreen(0x7A);
	bpos = (bpos + 8)%1024;
	if(sub == 0){
		showKeys(88, 96);
		i = 90;
		if(game.opts.drawMode)
			i = 91;
		game.button[i].state = 2;
		game.button[i].scale = .95;
		i = 92;
		if(game.opts.rumble)
			i = 93;
		game.button[i].state = 2;
		game.button[i].scale = .95;
		i = 94;
		if(game.opts.wsound)
			i = 95;
		game.button[i].state = 2;
		game.button[i].scale = .95;
		sub++;
	}
	else if(sub == 1){
		GRRLIB_Printf(320-56*2.5, 20, game.gfont[FONT_8BIT].img, 0xAACCFFFF, 2.5, "Options");
		GRRLIB_Printf(320-112*1.2, 82, game.gfont[FONT_GR].img, 0x10FF10FF, 1.2, "Rendering mode");
		GRRLIB_Printf(320-112*1.2, 192, game.gfont[FONT_GR].img, 0x10FF10FF, 1.2, "Wiimote rumble");
		GRRLIB_Printf(320-112*1.2, 302, game.gfont[FONT_GR].img, 0x10FF10FF, 1.2, "Wiimote sound");
		for(i=88; i<96; i++)
			drawButton(game.button[i]);
		for(i=3; i>=0; i--)
			if(game.mouse[i].x >= 0 && game.mouse[i].y >= 0)
				GRRLIB_DrawImg(game.mouse[i].x + 67.88*sin(deg2rad * (game.mouse[i].rotDeg-45)), game.mouse[i].y - 67.88*cos(deg2rad * (game.mouse[i].rotDeg-45)), GFXCursor, game.mouse[i].rotDeg, 1, 1, game.pcolor[i]);		
	}
	else if(sub == 2){
		saveOpts();
		screen = 1;
		sub = 128+33;
	}
	else if(sub < 32){
		GRRLIB_FillScreen(sub*8);
		SetVolumeOgg(128-(sub++)*4);
	}
	else if(sub == 32){
		GRRLIB_FillScreen(0xFF);
		StopOgg();
		screen = 3;
		sub = 0;
		bpos = 0;
	}
	else{
		screen = 1;
		sub = 256-32;
	}
	scFX();
	if(GetTimeOgg() > 70520)
		SetTimeOgg(2022);
}

void ctOpts(){
	int i, j;
	char btSel[NUMBUTTONS];
	updMice();
	if(sub != 1)
		return;
	for(i = 88; i < 94; i++)
		btSel[i] = 0;
	for(i = 88; i < 90; i++){
		for(j=0; j<4; j++){
			if(game.button[i].state > -1 && game.mouse[j].x > game.button[i].x - game.button[i].w/2.0*game.button[i].scale && game.mouse[j].y > game.button[i].y - game.button[i].h/2.0*game.button[i].scale && game.mouse[j].x < game.button[i].x + game.button[i].w/2.0*game.button[i].scale && game.mouse[j].y < game.button[i].y + game.button[i].h/2.0*game.button[i].scale){
				if(game.button[i].state == 0){
					game.button[i].state = 1;
					game.button[i].scale = 1.2;
					game.mouse[j].rumble = 4;
				}
				btSel[i] = 1;
				if(wd[j]->btns_d & WPAD_BUTTON_A)
					game.mouse[j].rumble = 4;
				if(wd[j]->btns_h & WPAD_BUTTON_A){
					game.button[i].state = 2;
					game.button[i].scale = .95;
				}
				if(wd[j]->btns_u & WPAD_BUTTON_A){
					game.button[i].state = 1;
					game.button[i].scale = 1.2;
					if(i==88)
						sub++;
					else if(i==89)
						sub = 3;
					game.mouse[j].rumble = 12;
				}
			}
		}
		if(game.button[i].state > 0 && btSel[i] == 0){
			game.button[i].state = 0;
			game.button[i].scale = 1;
		}
	}
	for(i = 90; i < 96; i++){
		for(j=0; j<4; j++){
			if(game.button[i].state > -1 && game.button[i].state != 2 && game.mouse[j].x > game.button[i].x - game.button[i].w/2.0*game.button[i].scale && game.mouse[j].y > game.button[i].y - game.button[i].h/2.0*game.button[i].scale && game.mouse[j].x < game.button[i].x + game.button[i].w/2.0*game.button[i].scale && game.mouse[j].y < game.button[i].y + game.button[i].h/2.0*game.button[i].scale){
				if(game.button[i].state == 0){
					game.button[i].state = 1;
					game.button[i].scale = 1.2;
					game.mouse[j].rumble = 4;
				}
				btSel[i] = 1;
				if(wd[j]->btns_d & WPAD_BUTTON_A){
					game.button[i].state = 2;
					game.button[i].scale = .95;
					game.mouse[j].rumble = 6;
					if(i>=90 && i<92){
						game.opts.drawMode = i-90;
						game.button[((i-90)^1)+90].state = 1;
					}
					if(i>=92 && i<94){
						game.opts.rumble = i-92;
						game.button[((i-92)^1)+92].state = 1;
					}
					if(i>=94 && i<96){
						game.opts.wsound = i-94;
						game.button[((i-94)^1)+94].state = 1;
					}
				}
			}
		}
		if(game.button[i].state == 1 && btSel[i] == 0){
			game.button[i].state = 0;
			game.button[i].scale = 1;
		}
	}
}
