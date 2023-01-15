/*===========================================
        WiiXBall v1.0.2
        by Kirill M	
							
		(c) KMS 2010-2011
        A DXBall Clone
============================================*/
#include "WiiXBall.h"


//POST-GAME DRAWING AND CONTROL FUNCTIONS------------------------------------------------------------------------------

void scHigh(){
	int i, j;
	float m, n;
	char* s;
	FILE* file;
	u32 col;
	static float phase = 0;
	if(stopgame && sub<529)
		sub = 529;
		
	if(sub > 0){  //stars
		for(i=0; i<MAXSTAR; i++){
			game.spark[i].x += 3;
			game.spark[i].vx += 1;
			if(game.spark[i].x > 320){
				game.spark[i].x = -320;
				game.spark[i].y = rand()%480-240;
				game.spark[i].vx = rand()%128;
			}
			GRRLIB_Rectangle(320+(game.spark[i].x * game.spark[i].vx/64.0), 240+(game.spark[i].y * game.spark[i].vx/64.0), game.spark[i].vx / 64.0, game.spark[i].vx / 64.0, 0xFFFFFFAA, true);
			if(GetTimeOgg() > 20000){
				for(j=1; j<game.spark[i].type/2; j++)
					GRRLIB_Rectangle(320+((game.spark[i].x - j) * (game.spark[i].vx-j)/64.0), 240+(game.spark[i].y * (game.spark[i].vx-j)/64.0), (game.spark[i].vx-j) / 64.0, (game.spark[i].vx-j) / 64.0, RGBA(255,255,255,128-j*4), true);
				game.spark[i].x += game.spark[i].type/20.0;
				game.spark[i].vx += game.spark[i].type/64.0;
				if(game.spark[i].type<64)
					game.spark[i].type++;
			}
		}
		for(i=MAXSPARK-2; i<MAXSPARK; i++){ //will be used for color fx of name field & hilite
			game.spark[i].x += game.spark[i].vx;
			if(game.spark[i].x < 0)
				game.spark[i].x += 6;
			if(game.spark[i].x > 6)
				game.spark[i].x -= 6;
			game.spark[i].vx += rand()%20/1000.0 - .01;
			if(game.spark[i].vx > .1)
				game.spark[i].vx = .1;
			if(game.spark[i].vx < -.1)
				game.spark[i].vx = -.1;
		}
		if(GetTimeOgg() > 20000)
			for(i=0; i<MAXBIGSTAR; i++){
				game.prize[i].x += 6;
				if(game.prize[i].x > 320 && rand()%512==0){
					game.prize[i].x = -320;
					game.prize[i].y = rand()%480-240;
					game.prize[i].vx = rand()%256;
					game.prize[i].rotDeg = rand()%360;
					game.prize[i].rotVel = rand()%200/10.0 - 10;
				}
				game.prize[i].vx += 1;
				game.prize[i].rotDeg += game.prize[i].rotVel;
				if(game.prize[i].rotDeg < 0)
					game.prize[i].rotDeg += 360;
				if(game.prize[i].rotDeg > 360)
					game.prize[i].rotDeg -= 360;
				game.prize[i].vy += .2;
				if(game.prize[i].vy > 6)
					game.prize[i].vy -= 6;
				m = game.prize[i].vx / 512.0;
				n = m*90.51;
				n = n*n*2;
				n = sqrt(n);
				GRRLIB_DrawImg(320 + (game.prize[i].x * game.prize[i].vx/64.0) + n*sin(deg2rad * (game.prize[i].rotDeg-45)), 240+(game.prize[i].y * game.prize[i].vx/64.0) - n*cos(deg2rad * (game.prize[i].rotDeg-45)), GFXHstar, game.prize[i].rotDeg, m, m, hsv2rgb(game.prize[i].vy, .9, .8, 255));
			}
	}
	if(sub > 336){  //hiscore logo - 48x10
		for(i=0; i<48; i++)
			for(j=0; j<10; j++)
				if(hLogo[j*48+i]){
					m = (((int)(phase + j/1.25 + i/6.0)*1000)%6000)/1000.0;
					GRRLIB_DrawImg(80+i*10+sin(phase+j/2.0)*4, 40+j*8+cos(phase+i/2.0)*4, GFXGball, 0, .2, .2, desaturate(hsv2rgb(m, 1, .5, 255)));
				}
		phase+=.1;
	}
	
	//main
	if(sub == 0){
		for(i=0; i<MAXSPARK; i++)
			game.spark[i].type = -1; //mark all unused, just in case
		for(i=0; i<MAXSTAR; i++){
			game.spark[i].type = 4;  //mark as used, will also be tail length
			game.spark[i].x = rand()%640-320;
			game.spark[i].y = rand()%480-240;
			game.spark[i].vx = rand()%128;
		}
		for(i=0; i<MAXBIGSTAR; i++){
			game.prize[i].x = 640;
			game.prize[i].y = rand()%480-240;
			game.prize[i].vx = rand()%256;
			game.prize[i].rotDeg = rand()%360;
			game.prize[i].rotVel = rand()%200/10.0 - 10;
			game.prize[i].vy = rand()%60/10.0; //hue
		}
		for(i=MAXSPARK-2; i<MAXSPARK; i++){ //will be used for color fx of name field
			game.spark[i].type = 127;
			game.spark[i].x = rand()%60/10.0; //hue
			game.spark[i].vx = rand()%20/100.0 - .1;
		}
		for(i=0; i<HSNAMELEN; i++)
			trec.name[i] = ' ';
		trec.score = -1;
		if(GetTimeOgg() == 0){
			if(game.score >= game.high[HSRECORDS-1].score){
				//PlayOgg(mem_open((char*)OGhigh, OGhigh_size), 0, OGG_ONE_TIME);
				SetVolumeOgg(255);
			}
			else{
				//MODPlay_SetMOD(&modplay,MDend);
				//MODPlay_Start(&modplay);
				//MODPlay_SetVolume(&modplay, 64, 64);
			}
		}
		if(game.score >= game.high[HSRECORDS-1].score)
			sub++;
		else
			sub = 337;
		showKeys(8, 34);
	}
	else if(sub < 128)
		GRRLIB_FillScreen(256-(sub++)*2);
	else if(sub == 128){
		if(GetTimeOgg() > 2500)
			sub++;
	}
	else if(sub < 192){
		m = ((sub++)-128)/64.0;
		m = m*m*m;
		GRRLIB_DrawImg(320-m*288, 240-m*64, GFXHnewrec, 0, m,m, 0xFFFFFFFF);
	}
	else if(sub < 240){
		if(rand()%2){
			i = rand()%576+32;
			genSpark(2, rand()%576+32, rand()%128+176, 16, 16, rand()%20-10, rand()%20-16, rand()%32+16);
			if(rand()%4==0)
				sndPlay(sBrbonus, sBrbonus_size, rand()%30/100.0 + .2, i / 640.0, 12000); 
		}
		GRRLIB_DrawImg(32, 176, GFXHnewrec, 0, 1, 1, 0xFFFFFFFF);
		sub++;
	}
	else if(sub == 240){
		GRRLIB_DrawImg(32, 176, GFXHnewrec, 0, 1, 1, 0xFFFFFFFF);
		if(GetTimeOgg() > 10000)
			sub++;
	}
	else if(sub < 256){
		GRRLIB_DrawImg(32, 176-9.75*(sub-240), GFXHnewrec, 0, 1, 1, 0xFFFFFFFF);
		GRRLIB_Rectangle(32, 268-8*(sub-240), 576, 320, 0xAAAAFF00 + (sub-240)*5.3, true);
		sub++;
	}
	else if(sub == 256){
		GRRLIB_DrawImg(32, 20, GFXHnewrec, 0, 1, 1, 0xFFFFFFFF);
		GRRLIB_Rectangle(32, 140, 576, 320, 0xAAAAFF55, true);
		GRRLIB_Rectangle(48, 148, 543, 36, hsv2rgb(game.spark[MAXSPARK-1].x, .8, .9, 255), false);
		GRRLIB_Rectangle(49, 149, 543, 36, hsv2rgb(game.spark[MAXSPARK-2].x, .8, .9, 255), false);
		if(strabslen(trec.name) > 0)
			GRRLIB_Printf(320-strabslen(trec.name)*12, 154, game.gfont[FONT_GR].img, 0xAAAAFFFF, 1.5, trec.name);
		else
			GRRLIB_Printf(320-22*12, 154, game.gfont[FONT_GR].img, 0x555555FF, 1.5, "Please enter your name");
		for(i=4; i<86; i++)
			if(game.button[i].state > -1)
				drawButton(game.button[i]);
		for(i=3; i>=0; i--)
			if(game.mouse[i].x >= 0 && game.mouse[i].y >= 0)
				GRRLIB_DrawImg(game.mouse[i].x + 67.88*sin(deg2rad * (game.mouse[i].rotDeg-45)), game.mouse[i].y - 67.88*cos(deg2rad * (game.mouse[i].rotDeg-45)), GFXCursor, game.mouse[i].rotDeg, 1, 1, game.pcolor[i]);		 
	}
	else if(sub < 272){
		GRRLIB_DrawImg(32, 20, GFXHnewrec, 0, 1, 1, 0xFFFFFFFF);
		GRRLIB_Rectangle(32, 268-8*(272-sub), 576, 320, 0xAAAAFF00 + (272 - sub)*5.3, true);
		sub++;
	}
	else if(sub < 336){
		GRRLIB_DrawImg(32, 20, GFXHnewrec, 0, 1, 1, 0xFFFFFFFF);
		GRRLIB_FillScreen(((sub++)-272)*4);
	}
	else if(sub == 336){ //save HS here
		GRRLIB_FillScreen(0xFF);
		sub = 464;
		if(strabslen(trec.name) == 0){
			s = strdup("Anonymous");
			for(i=0; i<strlen(s); i++)
				trec.name[i] = s[i];
		}
		i = HSRECORDS-1;
		while(i > 0 && game.score >= game.high[i-1].score){
			game.high[i].level = game.high[i-1].level;
			game.high[i].score = game.high[i-1].score;
			for(j=0; j<HSNAMELEN; j++)
				game.high[i].name[j] = game.high[i-1].name[j];
			i--;
		}
		game.high[i].level = game.level;
		game.high[i].score = game.score;
		for(j=0; j<HSNAMELEN; j++)
			game.high[i].name[j] = trec.name[j];
		trec.score = i;  //this score will be hilit later
		file = fopen("wxbhigh.dat", "wb");
		if(file == NULL)
			return;
		fwrite((char*)game.high, 1, sizeof(kRecord)*16, file);
		fclose(file);
	}
	else if(sub < 464){
		int i = log10(game.score);
		if(i < 0)
			i=0;
		GRRLIB_Printf(320-176, 200, game.gfont[FONT_GR].img, 0xCCCC11FF, 2, "Your score:");
		GRRLIB_Printf(320-30*(i+1), 280, GFXFscore, 0xFFFFFFFF, 2, "%d", game.score);
		if(sub < 400)
			GRRLIB_FillScreen(255-((sub++)-336)*4);
		else if(sub > 400)
			GRRLIB_FillScreen(((sub++)-400)*4);
	}
	else if(sub < 592){
		GRRLIB_Printf(40, 120, game.gfont[FONT_GR].img, 0x666655FF, .7, "Name");
		GRRLIB_Printf(472-11.2*5, 120, game.gfont[FONT_GR].img, 0x666655FF, .7, "Level");
		GRRLIB_Printf(600-11.2*5, 120, game.gfont[FONT_GR].img, 0x666655FF, .7, "Score");
		for(i=0; i<HSRECORDS; i++){
			col = blendColor(0x88AAFFFF, 0xFFFFFF00 + (1 - i/(HSRECORDS * 2.0))*255);
			if(game.high[i].level > game.boardcount)
				col = 0xEEEE11FF;
			if(i == trec.score)
				col = desaturate(hsv2rgb(game.spark[MAXSPARK-1].x, 1, 1, 0xFF));
			GRRLIB_Printf(40, 136+i*20, game.gfont[FONT_GR].img, col, 1, game.high[i].name);
			if(game.high[i].level > game.boardcount)
				GRRLIB_Printf(472 - 16*3, 136+i*20, game.gfont[FONT_GR].img, col, 1, "WON");
			else
				GRRLIB_Printf(472 - 16*(floor(log10(game.high[i].level))+1), 136+i*20, game.gfont[FONT_GR].img, col, 1, "%d",game.high[i].level);
			GRRLIB_Printf(600 - 16*(floor(log10(game.high[i].score))+1), 136+i*20, game.gfont[FONT_GR].img, col, 1, "%d",game.high[i].score);
		}
		if(trec.score >= 0){
			i = rand()%8 - 4;
			j = rand()%8 - 4;
			GRRLIB_Line(32+i, 134+trec.score*20+i, 608+j, 134+trec.score*20+j, hsv2rgb(game.spark[MAXSPARK-1].x, .8, .9, 0x7E));
			GRRLIB_Line(32-j, 156+trec.score*20-j, 608-i, 156+trec.score*20-i, hsv2rgb(game.spark[MAXSPARK-1].x, .8, .9, 0x7E));
			GRRLIB_Line(32+i, 134+trec.score*20+i, 32-j, 156+trec.score*20-j, hsv2rgb(game.spark[MAXSPARK-1].x, .8, .9, 0x7E));
			GRRLIB_Line(608+j, 134+trec.score*20+j, 608-i, 156+trec.score*20-i, hsv2rgb(game.spark[MAXSPARK-1].x, .8, .9, 0x7E));
			GRRLIB_Line(32+j, 134+trec.score*20+j, 608+i, 134+trec.score*20+i, hsv2rgb(game.spark[MAXSPARK-2].x, .8, .9, 0x7E));
			GRRLIB_Line(32-i, 156+trec.score*20-i, 608-j, 156+trec.score*20-j, hsv2rgb(game.spark[MAXSPARK-2].x, .8, .9, 0x7E));
			GRRLIB_Line(32+j, 134+trec.score*20+j, 32-i, 156+trec.score*20-i, hsv2rgb(game.spark[MAXSPARK-2].x, .8, .9, 0x7E));
			GRRLIB_Line(608+i, 134+trec.score*20+i, 608-j, 156+trec.score*20-j, hsv2rgb(game.spark[MAXSPARK-2].x, .8, .9, 0x7E));
		}
	
		if(sub < 528)
			GRRLIB_FillScreen(255-((sub++)-464)*4);
		else if(sub > 528){
			MODPlay_SetVolume(&modplay, 64-(sub-528), 64-(sub-528));
			SetVolumeOgg(255-(sub-528)*4);
			GRRLIB_FillScreen(((sub++)-528)*4);
		}
	}
	else{
		GRRLIB_FillScreen(0xFF);
		StopOgg();
		MODPlay_Stop(&modplay);
		screen = 1;
		if(game.level > game.boardcount)
			screen = 3;	//if u won the game, roll the credits
		sub = 0;
		bpos = 0; //for credits
	}
	for(i=0; i<MAXSPARK; i++)
		if(game.spark[i].type == 2){
			game.spark[i].x += game.spark[i].vx/2.0;
			game.spark[i].y -= game.spark[i].vy/2.0;
			game.spark[i].vy -= .1;
			if(game.spark[i].y > 500)
				game.spark[i].type = -1;
			GRRLIB_Rectangle(game.spark[i].x, game.spark[i].y, 2, 2, game.spark[i].color, true); 
		}
	if(GetTimeOgg() > 130449)
		SetTimeOgg(20305);
}

void ctHigh(){
	int i, j;
	char btSel[NUMBUTTONS];
	updMice();
	if(sub == 256){
		for(i = 4; i < 86; i++)
			btSel[i] = 0;
		for(i = 85; i >= 4; i--){ //backwards for auto up2low case chg, would type 2 letters
			for(j=0; j<4; j++){
				if(game.button[i].state > -1 && game.mouse[j].x > game.button[i].x - game.button[i].w/2.0*game.button[i].scale && game.mouse[j].y > game.button[i].y - game.button[i].h/2.0*game.button[i].scale && game.mouse[j].x < game.button[i].x + game.button[i].w/2.0*game.button[i].scale && game.mouse[j].y < game.button[i].y + game.button[i].h/2.0*game.button[i].scale){
					if(game.button[i].state == 0){
						game.button[i].state = 1;
						game.button[i].scale = 1.2;
						game.mouse[j].rumble = 3;
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
						if(i==4)
							sub++;
						else if(i==5)
							showKeys(8, 34);
						else if(i==6)
							showKeys(34, 60);
						else if(i==7)
							showKeys(60, 86);
						else if(i>=8 && i<34){
							if(strabslen(trec.name) == 0){
								showKeys(34, 60);
								game.button[i].scale = 1;
							}
							if(strabslen(trec.name) < HSNAMELEN)
								trec.name[strabslen(trec.name)] = (i-8)+'A';
						}
						else if(i>=34 && i<60){
							if(strabslen(trec.name) < HSNAMELEN)
								trec.name[strabslen(trec.name)] = (i-34)+'a';
						}
						else if(i==64){ //% doesnt get printed..
							if(strabslen(trec.name) < HSNAMELEN)
								trec.name[strabslen(trec.name)] = '_';
						}
						else if(i>=60 && i<67){
							if(strabslen(trec.name) < HSNAMELEN)
								trec.name[strabslen(trec.name)] = (i-60)+'!';		
						}
						else if(i>=67 && i<83){
							if(strabslen(trec.name) < HSNAMELEN)
								trec.name[strabslen(trec.name)] = (i-67)+'*';
						}
						else if(i==83){
							if(strabslen(trec.name) < HSNAMELEN)
								trec.name[strabslen(trec.name)] = '@';
						}
						else if(i==84){
							if(strabslen(trec.name) < HSNAMELEN)
								trec.name[strabslen(trec.name)] = '^';
						}
						else if(i==85){
							if(strabslen(trec.name) < HSNAMELEN)
								trec.name[strabslen(trec.name)] = '~';
						}
						game.mouse[j].rumble = 4;
					}
				}
			}
			if(game.button[i].state > 0 && btSel[i] == 0){
				game.button[i].state = 0;
				game.button[i].scale = 1;
			}
		}
		for(i=0; i<4; i++){
			if((wd[i]->btns_d & WPAD_BUTTON_B) && strabslen(trec.name) > 0)
				trec.name[strabslen(trec.name)-1] = ' ';
		}
	}
	else if(sub == 400 || sub == 528){
		for(i = 0; i < 4; i++)
			if(wd[i]->btns_d & (WPAD_BUTTON_A | WPAD_BUTTON_B))
				sub++;
	}
	
	/*
	for(i = 0; i < 4; i++){
		//if(wd[i]->btns_d & (WPAD_BUTTON_A | WPAD_BUTTON_B))
			//escape(0);
		if(wd[i]->btns_d & (WPAD_BUTTON_1)){
			screen = 9;
			sub = 0;
		}
	}*/ //u cheater
	
}

void scWin(){
	int i,j;
	float m, n;
	if(sub == 0){
		for(i=0; i<MAXSTAR; i++){
			game.spark[i].x = rand()%640;
			game.spark[i].y = rand()%480;
			game.spark[i].vx = rand()%256; //z-value
		}
		for(i=1; i<MAXBIGSTAR; i++){ //kPrizes will be used for bigstars
			game.prize[i].x = rand()%128 + 64;
			if(game.prize[i].x > 128)
				game.prize[i].x += 384;
			game.prize[i].y = rand()%128 + 64;
			if(game.prize[i].y > 128)
				game.prize[i].y += 224;
			game.prize[i].vx = rand()%1024 - 4096 - 512; //z-value
			game.prize[i].rotDeg = rand()%360;
			game.prize[i].rotVel = rand()%100/10.0 - 5;
		}
		game.prize[0].x = 640/2;
		game.prize[0].y = 480/2;
		game.prize[0].vx = -4096;
		game.prize[0].rotDeg = 0;
		game.prize[0].rotVel = 0;
		GRRLIB_ClearTex(GBuf[0]);
		GRRLIB_ClearTex(GBuf[1]);
		//PlayOgg(mem_open((char*)OGwin, OGwin_size), 0, OGG_ONE_TIME);
		SetVolumeOgg(255);
		GRRLIB_FillScreen(0xFF);
	}
	else if(sub < 350){
		GRRLIB_FillScreen(RGBA(0,0,(350-sub)/16.0 + 50, 255));
		for(i=0; i<MAXSTAR; i++){
			game.spark[i].y += game.spark[i].vx / 32.0;
			if(game.spark[i].y > 480){
				game.spark[i].y = 0;
				game.spark[i].x = rand()%640;
			}
			GRRLIB_Rectangle(game.spark[i].x, game.spark[i].y, game.spark[i].vx / 64.0, game.spark[i].vx / 64.0, RGBA(255,255,255,game.spark[i].vx/4.0 + 128), true); //was /128.0 ?	
		}
		if(sub < 32)
			GRRLIB_FillScreen((32-sub)*8);
	}
	else if(sub < 400){
		GRRLIB_FillScreen(RGBA(0,0,400-sub, 255));
		for(i=0; i<MAXSTAR; i++){
			game.spark[i].y += (game.spark[i].vx / 32.0) * ((sub-330)/20.0);
			if(game.spark[i].y > 480){
				game.spark[i].y = 0;
				game.spark[i].x = rand()%640;
			}
			for(j=1; j<sub-360; j++)
				GRRLIB_Rectangle(game.spark[i].x, game.spark[i].y - (game.spark[i].vx / 64.0)*j, game.spark[i].vx / 64.0, game.spark[i].vx / 64.0, RGBA(255,255,255,game.spark[i].vx/128.0 + 72 - j*2), true);		
			GRRLIB_Rectangle(game.spark[i].x, game.spark[i].y, game.spark[i].vx / 64.0, game.spark[i].vx / 64.0, RGBA(255,255,255,game.spark[i].vx/128.0 + 128), true);		
		}
	}
	else if(sub < 450){
		for(i=0; i<MAXBIGSTAR; i++){
			game.prize[i].vx += 4096/50.0;
			game.prize[i].rotDeg += game.prize[i].rotVel;
			if(game.prize[i].rotDeg < 0)
				game.prize[i].rotDeg += 360;
			if(game.prize[i].rotDeg > 360)
				game.prize[i].rotDeg -= 360;
			game.prize[i].rotVel *= .96;
		}
		GRRLIB_DrawImg(0,0,GBuf[sub%2], 0, 1, 1, 0xFFFFFFFF);
		for(i=1; i<MAXBIGSTAR; i++){
			m = game.prize[i].vx / 4096.0 + 1;
			m = (m*m*m) * 1.5;
			n = m*90.51;
			n = n*n*2;
			n = sqrt(n);
			GRRLIB_DrawImg(game.prize[i].x + n*sin(deg2rad * (game.prize[i].rotDeg-45)), game.prize[i].y - n*cos(deg2rad * (game.prize[i].rotDeg-45)), GFXHstar, game.prize[i].rotDeg, m, m, hsv2rgb(rand()%60/10.0, .9, .8, 255));
		}
		m = game.prize[0].vx / 4096.0 + 1;
		m = m*m*m;
		GRRLIB_DrawImg(game.prize[0].x - m*240, game.prize[0].y - m*160, GFXHwin, 0, m, m, 0xFFFFFFFF);
		GRRLIB_Screen2Texture(0,0,GBuf[(sub+1)%2],false);
	}
	else if(sub < 500){
		GRRLIB_DrawImg(0,0,GBuf[0], 0, 1, 1, 0xFFFFFFFF);
		for(i=1; i<MAXBIGSTAR; i++){
			m = game.prize[i].vx / 4096.0 + 1;
			m = (m*m*m) * 1.5;
			n = m*90.51;
			n = n*n*2;
			n = sqrt(n);
			GRRLIB_DrawImg(game.prize[i].x + n*sin(deg2rad * (game.prize[i].rotDeg-45)), game.prize[i].y - n*cos(deg2rad * (game.prize[i].rotDeg-45)), GFXHstar, game.prize[i].rotDeg, m, m, hsv2rgb(rand()%60/10.0, .9, .8, 255));
		}
		m = game.prize[0].vx / 4096.0 + 1;
		m = m*m*m;
		GRRLIB_DrawImg(game.prize[0].x - m*240, game.prize[0].y - m*160, GFXHwin, 0, m, m, 0xFFFFFFFF);
		if(sub > 484)
			GRRLIB_FillScreen(RGBA(255,255,255,(sub-484)*16));
	}
	else{
		if(sub == 500)
			for(i=1; i<MAXBIGSTAR; i++){
				game.prize[i].vy = rand()%60/10.0; //now functions as star's hue
				game.prize[i].rotVel = rand()%20/100.0 - .1; //change rate of star's hue
			}
		GRRLIB_DrawImg(0,0,GBuf[0], 0, 1, 1, 0xFFFFFFFF);
		for(i=1; i<MAXBIGSTAR; i++){
			game.prize[i].vy += game.prize[i].rotVel;
			if(game.prize[i].vy < 0)
				game.prize[i].vy += 6;
			if(game.prize[i].vy > 6)
				game.prize[i].vy -= 6;
			game.prize[i].rotVel += rand()%20/1000.0 - .01;
			if(game.prize[i].rotVel > .1)
				game.prize[i].rotVel = .1;
			if(game.prize[i].rotVel < -.1)
				game.prize[i].rotVel = -.1;
			m = game.prize[i].vx / 4096.0 + 1;
			m = (m*m*m) * 1.5;
			n = m*90.51;
			n = n*n*2;
			n = sqrt(n);
			GRRLIB_DrawImg(game.prize[i].x + n*sin(deg2rad * (game.prize[i].rotDeg-45)), game.prize[i].y - n*cos(deg2rad * (game.prize[i].rotDeg-45)), GFXHstar, game.prize[i].rotDeg, m, m, hsv2rgb(game.prize[i].vy, .9, .8, 255));
		}
		m = game.prize[0].vx / 4096.0 + 1;
		m = m*m*m;
		GRRLIB_DrawImg(game.prize[0].x - m*240, game.prize[0].y - m*160, GFXHwin, 0, m, m, 0xFFFFFFFF);
		if(sub < 516)
			GRRLIB_FillScreen(RGBA(255,255,255,(516-sub)*16));
		else if(sub == 888){
			//PlayOgg(mem_open((char*)OGhigh, OGhigh_size), 0, OGG_ONE_TIME);
			SetVolumeOgg(255);
		}
		else if(sub == 999)
			sub--;
		else if(sub >= 1000 && sub < 1256)
			GRRLIB_FillScreen(sub-1000);
		else if(sub >= 1256){
			GRRLIB_FillScreen(0xFF);
			screen = 8;
			sub = -1;
			if(stopgame)
				screen = 1;
		}
	}
	if(GetTimeOgg() > 130449)
		SetTimeOgg(20305);
	sub++;
}

void ctWin(){
	int i;
	if(sub != 999)
		return;
	for(i = 0; i < 4; i++)
		if(wd[i]->btns_d & (WPAD_BUTTON_A | WPAD_BUTTON_B))
			sub++;
}

void scCred(){
	int i, j;
	if(stopgame && sub<129)
		sub = 129;
	if(sub > 0){
		for(i=0; i<MAXSTAR; i++){
			game.spark[i].x -= 3;
			game.spark[i].vx += 1;
			if(game.spark[i].x < -320){
				game.spark[i].x = 320;
				game.spark[i].y = rand()%480-240;
				game.spark[i].vx = rand()%128;
			}
			GRRLIB_Rectangle(320+(game.spark[i].x * game.spark[i].vx/64.0), 240+(game.spark[i].y * game.spark[i].vx/64.0), game.spark[i].vx / 64.0, game.spark[i].vx / 64.0, 0xFFFFFFAA, true);
			for(j=1; j<16; j++)
				GRRLIB_Rectangle(320+((game.spark[i].x + j) * (game.spark[i].vx-j)/64.0), 240+(game.spark[i].y * (game.spark[i].vx-j)/64.0), (game.spark[i].vx-j) / 64.0, (game.spark[i].vx-j) / 64.0, RGBA(255,255,255,128-j*4), true);
		}
	}
	
	j = 300;
	if(bpos > 512)
		j -= (bpos-512);
	GRRLIB_SetBlend(GRRLIB_BLEND_MULTI);
	GRRLIB_DrawImg(80, j-200, GFXInttbg, 0, 1, 1, 0xFFFFFFFF);
	GRRLIB_SetBlend(GRRLIB_BLEND_ALPHA);
	GRRLIB_DrawImg(80, j-200, GFXIntlogo[(bpos/2)%8], 0, 1, 1, 0xFFFFFFFF);
	printLine(&j, FONT_GR, "* WiiX-Ball *", 		  8, 1.5, 0xFFFFBBFF);
	printLine(&j, FONT_GR, "v 1.0", 				128, 1  , 0x888844FF);
	printLine(&j, FONT_GR, "Programmed by", 		 32, 2  , 0x88AAFFFF);
	printLine(&j, FONT_GR, "Kirill M.", 			128, 1.5, 0x44FF44FF);
	printLine(&j, FONT_GR, "Based on DX-Ball", 		 16, 2  , 0x88AAFFFF);
	printLine(&j, FONT_GR, "by Michael P. Welch", 	128, 1.5, 0x88AAFFFF);
	printLine(&j, FONT_GR, "Graphics and effects", 	 32, 1.8, 0x88AAFFFF);
	printLine(&j, FONT_GR, "Kirill M.", 			 24, 1.5, 0x44FF44FF);
	printLine(&j, FONT_GR, "Michael P. Welch", 		 24, 1.5, 0x44FF44FF);
	printLine(&j, FONT_GR, "Seumas McNally", 		128, 1.5, 0x44FF44FF);
	printLine(&j, FONT_GR, "Music", 				 32, 2  , 0x88AAFFFF);
	printLine(&j, FONT_GR, "Space Debris", 			 16, 1.5, 0x44FF44FF);
	printLine(&j, FONT_GR, "by captain/image", 		 32, 1.5, 0x44FF44FF);
	printLine(&j, FONT_GR, "Team Haze Chiptune", 	 16, 1.5, 0x44FF44FF);
	printLine(&j, FONT_GR, "by dubmood/Razor1911", 	 32, 1.5, 0x44FF44FF);
	printLine(&j, FONT_GR, "Candy Coast", 			 16, 1.5, 0x44FF44FF);
	printLine(&j, FONT_GR, "by Wiklund & Nyhlin", 	128, 1.5, 0x44FF44FF);
	printLine(&j, FONT_GR, "Greetings go out to", 	 32, 1.8, 0x88AAFFFF);
	printLine(&j, FONT_GR, "Michael P. Welch", 		 24, 1.5, 0x44FF44FF);
	printLine(&j, FONT_GR, "Ed & Al Mackey", 		 24, 1.5, 0x44FF44FF);
	printLine(&j, FONT_GR, "Crayon", 				 24, 1.5, 0x44FF44FF);
	printLine(&j, FONT_GR, "Nonameno", 				128, 1.5, 0x44FF44FF);
	printLine(&j, FONT_GR, "Special thanks to", 	 32, 1.8, 0x88AAFFFF);
	printLine(&j, FONT_GR, "Mike Mammarella", 		128, 1.5, 0x44FF44FF);
	printLine(&j, FONT_GR, "Created with", 			 32, 2  , 0x88AAFFFF);
	printLine(&j, FONT_GR, "DevKitPro", 			 24, 1.5, 0x44FF44FF);
	printLine(&j, FONT_GR, "GRRLIB 4.1.0", 			256, 1.5, 0x44FF44FF);
	printLine(&j, FONT_GR, "Please visit", 			 16, 1.5, 0x88AAFFFF);
	printLine(&j, FONT_GR, "www.KMSZone.com", 		 64, 2  , 0xFFFF88FF);
	printLine(&j, FONT_GR, "Send your comments to", 16, 1  , 0x88AAFFFF);
	printLine(&j, FONT_GR, "kms2061 @ gmail . com", 80, 1.2, 0x44FF44FF);
	printLine(&j, FONT_8BIT, "Enjoy the game ;)", 	 64, 1.8, 0xFFFFBBFF);
	printLine(&j, FONT_GR, "                (C) KMS 2010", 0, 1.2, 0x666633FF);
	
	if(sub == 0){
		for(i=0; i<MAXSTAR; i++){
			game.spark[i].x = rand()%640-320;
			game.spark[i].y = rand()%480-240;
			game.spark[i].vx = rand()%128;
		}
		GRRLIB_FillScreen(0xFF);
		//MODPlay_SetMOD(&modplay,MDend);
		//MODPlay_Start(&modplay);
		//MODPlay_SetVolume(&modplay, 64, 64);
		sub++;
	}
	else if(sub < 128)
		GRRLIB_FillScreen(256-(sub++)*2);
	else if(sub == 256){
		GRRLIB_FillScreen(0xFF);
		MODPlay_Stop(&modplay);
		screen = 1;
		sub = 0;
	}
	else if(sub > 128){
		GRRLIB_FillScreen(((sub++)-128)*2);
		MODPlay_SetVolume(&modplay, 64-(sub-128)/2, 64-(sub-128)/2);
	}
	if(bpos < 3150)
		bpos++;
}

void ctCred(){
	int i;
	if(sub != 128)
		return;
	for(i = 0; i < 4; i++)
		if(wd[i]->btns_d & (WPAD_BUTTON_A | WPAD_BUTTON_B))
			sub++;
}
