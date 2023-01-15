/*===========================================
        WiiXBall v1.0.2
        by Kirill M	
							
		(c) KMS 2010-2011
        A DXBall Clone
============================================*/
#include "WiiXBall.h"


//GENERATION FUNCTIONS--------------------------------------------------------------------------------------------------

int genBall(s8 held, float holdpos, float x, float y, float vel, float dir, int size){
	int i;
	for(i=0; i<MAXBALL-1; i++){
		if(game.ball[i].state < 0)
			break;	
	}
	if(game.ball[i].state >= 0)
		return -1;
	game.ball[i].state = 0;
	game.ball[i].held = held;
	game.ball[i].holdpos = holdpos;
	game.ball[i].x = x;
	game.ball[i].y = y;
	game.ball[i].vel = vel;
	game.ball[i].dir = dir;
	game.ball[i].size = 0;
	game.ball[i].dsize = size;
	game.ball[i].rotDeg = 0;
	game.ball[i].rotVel = 0;
	return i;
}

int genBolt(int tx, int ty){
	int i;
	for(i=0; i<MAXBOLT-1; i++){
		if(game.bolt[i].state < 0)
			break;
	}
	if(game.bolt[i].state >= 0)
		return -1;
	game.bolt[i].state = 0;
	game.bolt[i].tx = tx;
	game.bolt[i].ty = ty;
	genExplo(tx, ty);
	return i;
}

int genBullet(int pad){
	int i;
	for(i=0; i<MAXBULLET-2; i++){
		if(game.pad[pad].bullet[i].state < 0 && game.pad[pad].bullet[i+1].state < 0)
			break;
	}
	if(game.pad[pad].bullet[i].state >= 0 || game.pad[pad].bullet[i+1].state >= 0)
		return -1;
	game.pad[pad].bullet[i].state = 0;
	game.pad[pad].bullet[i+1].state = 0;
	game.pad[pad].bullet[i].x = game.pad[pad].calcx + game.pad[pad].calclen / 16;
	game.pad[pad].bullet[i+1].x = game.pad[pad].calcx + game.pad[pad].calclen - game.pad[pad].calclen / 16;
	game.pad[pad].bullet[i].y = game.pad[pad].y - 10;
	game.pad[pad].bullet[i+1].y = game.pad[pad].y - 10;
	game.pad[pad].bullet[i].vel = 8;
	game.pad[pad].bullet[i+1].vel = 8;
	game.pad[pad].bullet[i].dir = deg2rad * (90 - game.pad[pad].rotDeg*2);
	game.pad[pad].bullet[i+1].dir = deg2rad * (90 - game.pad[pad].rotDeg*2);
	wsndPlay(pad, wGun, wGun_size);
	sndPlay(sGun, sGun_size, .6, game.pad[pad].x / 640.0 * .6 + .2, 2000);
	game.mouse[pad].rumble = 4;
	return i;
}

int genExplo(float x, float y){
	int i;
	for(i=0; i<MAXEXPLO-1; i++){
		if(game.explo[i].state < 0)
			break;
	}
	if(game.explo[i].state >= 0)
		return -1;
	game.explo[i].state = 0;
	game.explo[i].x = x;
	game.explo[i].y = y;
	game.explo[i].size = rand()%80/100.0 + .6;
	i = rand()%3;
	sndPlay(sExp[i], sExp_size[i], rand()%20/100.0 + .8, x/640.0 * .8 + .1, 8000);
	return i;
}

int genPrize(int ball, float x, float y){ //play with probability later
	int i;
	float m, n;
	for(i=0; i<MAXPRIZE-1; i++){
		if(game.prize[i].state < 0)
			break;
	}
	if(game.prize[i].state >= 0)
		return -1;
	game.prize[i].state = rand()%NUMPRIZES;
	game.prize[i].x = x;
	game.prize[i].y = y;
	m = 0;
	n = 0;
	if(ball >=0){
		m = game.ball[ball].vel * cos(game.ball[ball].dir) / 2;
		n = game.ball[ball].vel * sin(game.ball[ball].dir) / 2;
	}
	m += rand()%12 - 6; //was %16
	n += rand()%10 - 3;
	game.prize[i].vx = m;
	game.prize[i].vy = n;
	game.prize[i].rotDeg = rand()%360;
	game.prize[i].rotVel = rand()%320/10.0 - 16;
	sndPlay(sBrbonus, sBrbonus_size, rand()%30/100.0 + .7, x / 640.0 * .6 + .2, 12000);
	genSpark(0, x, y, 16, 16, 8, 8, rand()%16 + 16);
	return i;
}

//sparks: 0-brbonus, 1-hid, 2-padspark, 3-fire
void genSpark(s8 type, float x, float y, int xspread, int yspread, int xvel, int yvel, int num){
	int i, j;
	for(j = 0; j < num; j++){
		for(i=0; i<MAXSPARK-1; i++){
			if(game.spark[i].type < 0)
				break;
		}
		if(game.spark[i].type >= 0)
			return;
		game.spark[i].type = type;
		game.spark[i].x = x + rand()%xspread - xspread/2;
		game.spark[i].y = y + rand()%yspread - yspread/2;
		game.spark[i].vx = (rand()%100/100.0) * xvel - xvel/2;
		game.spark[i].vy = (rand()%100/100.0) * yvel - yvel/2;
		if(type == 0)
			game.spark[i].color = 0xFFFFFFFF;
		else if(type == 1)
			game.spark[i].color = 0xAAAAAAFF;
		else if(type == 2)
			game.spark[i].color = 0xFFFF22FF;
		else if(type == 3)
			game.spark[i].color = 0xFF5555FF;
	}
}
