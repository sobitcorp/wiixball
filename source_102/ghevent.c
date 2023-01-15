/*===========================================
        WiiXBall v1.0.2
        by Kirill M	
							
		(c) KMS 2010-2011
        A DXBall Clone
============================================*/
#include "WiiXBall.h"


//POWERUP AND ACTION FUNCTIONS-----------------------------------------------------------------------------------------------

void ballHitPad(int ball, int pad, float baseline, double* vx, double* vy){
	if(game.ball[ball].y + game.ball[ball].size/2 > baseline+10)
		sndPlay(sPadlow, sPadlow_size, rand()%20/100.0 + .8, game.pad[pad].x / 640.0 * .6 + .2, 8000);
	if(game.pow.fall)
		tileFall();
	game.ball[ball].y = baseline - game.ball[ball].size/2;
	if(game.pad[pad].hold){
		game.ball[ball].held = pad;
		game.ball[ball].holdpos = (game.ball[ball].x - game.pad[pad].calcx) / game.pad[pad].calclen;
		if(game.ball[ball].holdpos < .08)
			game.ball[ball].holdpos = .08;
		if(game.ball[ball].holdpos > .92)
			game.ball[ball].holdpos = .92;		
		sndPlay(sHold, sHold_size, rand()%20/100.0 + .5, game.pad[pad].x / 640.0 * .6 + .2, 8000);
	}
	else{
		wsndPlay(pad, wPad, wPad_size);
		if(game.ball[ball].vel < BALLSPARKSPEED)
			sndPlayP(sPad, sPad_size, rand()%30/100.0 + .6, game.pad[pad].x / 640.0 * .6 + .2, game.ball[ball].size * -1024 + 12000 + rand()%2000);
		else{
			sndPlay(sPadfast, sPadfast_size, rand()%30/100.0 + .7, game.pad[pad].x / 640.0 * .6 + .2, 12000);
			genSpark(2, game.ball[ball].x, game.ball[ball].y + game.ball[ball].size/2, 4, 4, *vx + 2, *vy * 1.5 + 2, rand()%16 + 16);
		}
		ballRelease(ball, pad);
	}
	*vy = .25;
	*vx = (game.ball[ball].x - game.pad[pad].calcmid) / game.pad[pad].length;
	game.mouse[pad].rumble = game.ball[ball].size / 5 + game.ball[ball].vel / 2;
}

void ballHitTile(int ball, int tile){ //if ball is -1, it was hit by bullet
	if(game.tile[tile].state >= 0x20 && game.tile[tile].state <= 0x27)
		game.tile[tile].state = 0x28;
	else if(game.tile[tile].state == 0x15)
		game.tile[tile].state = 0x1f;
	else if(game.tile[tile].state == 0x02)
		game.tile[tile].state = 0x1e;
	else if(game.tile[tile].state == 0x03)
		game.tile[tile].state = 0x1c;
	else if(game.tile[tile].state == 0x04)
		game.tile[tile].state = 0x1d;
	else if(game.tile[tile].state == 0x07)
		game.tile[tile].state = 0x19;
	else if(game.tile[tile].state == 0x17)
		game.tile[tile].state = 0x31;
	else if(game.tile[tile].state == 0x18)
		game.tile[tile].state = 0x32;
	else if(game.tile[tile].state == 0x01 || (game.tile[tile].state >= 0x05 && game.tile[tile].state <= 0x06) || (game.tile[tile].state >= 0x09 && game.tile[tile].state <= 0x14) || game.tile[tile].state == 0x16){
		game.tile[tile].state = 0x1a;
		if(ball >= 0 && rand()%PF_BALL == 0)
			genPrize(ball, game.ball[ball].x, game.ball[ball].y);
		else if(rand()%PF_BULLET == 0)
			genPrize(-1, game.tile[tile].x + game.tile[tile].w/2, game.tile[tile].y + game.tile[tile].h/2);
	}
		
}

void ballRelease(int ball, int pad){ //does not check if ball actually held!
	game.ball[ball].ddir -= deg2rad * game.pad[pad].rotDeg * 4; //to be added after any current processing
	game.ball[ball].y -=3;
	game.ball[ball].held = -1;
}

void ballSizeDown(){
	int i;
	for(i = 0; i<MAXBALL; i++){
		if(game.ball[i].state >= 0)
			game.ball[i].dsize = game.ball[i].size * -.5;
	}
}

void ballSizeUp(){
	int i;
	for(i = 0; i<MAXBALL; i++){
		if(game.ball[i].state >= 0)
			game.ball[i].dsize += rand()%5 + 8;
	}
	sndPlay(sPadex, sPadex_size, 1, .5, 1);
}

void ballSpeedDown(){
	int i;
	for(i = 0; i<MAXBALL; i++){
		if(game.ball[i].state >= 0)
			game.ball[i].vel *= rand()%40/100.0 + .3;
	}
} 

void ballSpeedUp(){
	int i;
	for(i = 0; i<MAXBALL; i++){
		if(game.ball[i].state >= 0)
			game.ball[i].vel /= rand()%30/100.0 + .5;
	}
}

void ballSplit(){
	int i, j;
	for(i = 0; i<MAXBALL; i++)
		if(game.ball[i].state >= 0 && game.ball[i].state != 127){
			j = genBall(game.ball[i].held, game.ball[i].holdpos, game.ball[i].x, game.ball[i].y, game.ball[i].vel + rand()%200/100.0 - 1, (int)(game.ball[i].dir*rad2deg + rand()%40 - 20 + 180)%360 * deg2rad, game.ball[i].size + rand()%200/100.0 - 1);
			game.ball[j].state = 127;
		}
	for(i = 0; i<MAXBALL; i++)
		if(game.ball[i].state == 127)
			game.ball[i].state = 0;
	sndPlay(sSplit, sSplit_size, 1, .5, 1);
}

void ballSpread(){
	int i, j;
	sndPlay(sPadfast, sPadfast_size, 1, .5, 1);
	for(i = 0; i<MAXBALL; i++)
		if(game.ball[i].state >= 0){
			game.ball[i].state = -1;
			for(j = 0; j<360; j+=45)
				genBall(-1, 0, game.ball[i].x, game.ball[i].y, game.ball[i].vel + rand()%200/100.0 + .5, (int)(game.ball[i].dir*rad2deg + j)%360 * deg2rad, game.ball[i].size + rand()%200/100.0 - 2);
			if(rand()%3)
				return;
		}
}

void boltFire(){
	int i, j;
	if(game.pow.boltvoice != -1 && sndActive(game.pow.boltvoice))
		sndStop(game.pow.boltvoice);
	for(i=0; i<20*20; i++)
		if(game.tile[i].state == 0x1 || (game.tile[i].state > 0x2 && game.tile[i].state <= 0x18) || (game.tile[i].state >= 0x20 && game.tile[i].state < 0x28))
			break;
	if(i >= 20*20)
		return;
	game.tile[i].state = 0x28;
	for(j=-1; j<=1; j+=2) //x
		if(i%20+j >= 0 && i%20+j < 20)
			game.tile[(i/20)*20+(i%20+j)].state = 0x28;
	for(j=-1; j<=1; j+=2) //y
		if(i/20+j >= 0 && i/20+j < 20)
			game.tile[(i/20+j)*20+(i%20)].state = 0x28;
	genBolt(game.tile[i].x + game.tile[i].w/2, game.tile[i].y + game.tile[i].h/2);
	sndPlay(sBolt, sBolt_size, 1, game.tile[i].x / 640.0 * .6 + .25, 4000);
}

void boltPrize(){
	while(1){
		boltFire();
		if(rand()%2)
			return;
	}
}

void levelWin(){
	screen = 6;
	sub = 64;
}

void levelLose(){
	game.pow.boltvoice = -1;
	sndStopAll();
	sndPlay(sGameover, sGameover_size, 1, .5, 1);
	screen = 6;
	sub = 128;
}

void tileExplode(){
	int i;
	for(i=0; i<20*20; i++)
		if(game.tile[i].state >= 0x20 && game.tile[i].state <= 0x27)
			game.tile[i].state = 0x28;
}

void tileFall(){ //old fashioned fall- switches tiles
	int i;
	bool fell = false;
	for(i=20*20-1; i>=20; i--)
		if(game.tile[i].state == 0x0 && game.tile[i-20].state != 0){
			game.tile[i].state = game.tile[i-20].state;
			game.tile[i-20].state = 0x0;
			fell = true;
		}
	if(fell)
		sndPlay(sFall, sFall_size, .8, .5, 2000);
}

void tilePlus(){
	int i, j;
	for(i=0; i<20*20; i++)
		if(game.tile[i].state >= 0x20 && game.tile[i].state <= 0x27){
			for(j=-1; j<=1; j+=2) //x
				if(i%20+j >= 0 && i%20+j < 20 && game.tile[(i/20)*20+(i%20+j)].state < 0x20)
					game.tile[(i/20)*20+(i%20+j)].state = 0x2f;
			for(j=-1; j<=1; j+=2) //y
				if(i/20+j >= 0 && i/20+j < 20 && game.tile[(i/20+j)*20+(i%20)].state < 0x20)
					game.tile[(i/20+j)*20+(i%20)].state = 0x2f;	
		}
	for(i=0; i<20*20; i++)
		if(game.tile[i].state == 0x2f)
			game.tile[i].state = 0x20;
}

void tileZap(){
	int i;
	sndPlay(sSb, sSb_size, 1, .5, 1);
	for(i=0; i<20*20; i++){
		if(game.tile[i].state == 0x02 || game.tile[i].state == 0x15 || (game.tile[i].state >= 0x1e && game.tile[i].state <= 0x1f))
			game.tile[i].state = 0x13;
		else if((game.tile[i].state >= 0x03 && game.tile[i].state <= 0x04) || game.tile[i].state == 0x1c)
			game.tile[i].state = 0x05;
		else if(game.tile[i].state == 0x07)
			game.tile[i].state = 0x06;
	}
	sndPlay(sSb, sSb_size, 1, .5, 1);
}

void padRelease(int pad){
	int j;
	for(j = 0; j<MAXBALL; j++)
		if(game.ball[j].state >= 0 && game.ball[j].held == pad)
			ballRelease(j, pad);
}

void prizeHitPad(int prize, int pad){
	bool defaultSnd = true;
	wsndPlay(pad, wBonus, wBonus_size);
	if((game.prize[prize].state > 13 && game.prize[prize].state <= 17)||(game.prize[prize].state >= 24 && game.prize[prize].state <= 25)){
		defaultSnd = false;
		sndPlay(sBad, sBad_size, 1, .5, 1);
	}
	if((game.prize[prize].state >= 0 && game.prize[prize].state <= 2) || game.prize[prize].state == 8 || game.prize[prize].state == 16)
		padRelease(pad);
	if(game.prize[prize].state == 0){
		game.pad[pad].dlength = PADGROWFACTOR;
		sndPlay(sPadex, sPadex_size, 1, .5, 1);
	}
	else if(game.prize[prize].state == 1){
		game.pad[pad].dlength = -PADGROWFACTOR;
		sndPlay(sPadsh, sPadsh_size, 1, .5, 1);
	}
	else if(game.prize[prize].state == 2) ballSplit();
	else if(game.prize[prize].state == 3) game.pow.noclip = 1;
	else if(game.prize[prize].state == 4) tileExplode();
	else if(game.prize[prize].state == 5) game.pow.fire = 1;
	else if(game.prize[prize].state == 6) game.pad[pad].shoot = 1;
	else if(game.prize[prize].state == 7) game.pad[pad].hold = 1;
	else if(game.prize[prize].state == 8){
		game.lives++;
		game.pad[pad].hold = 0;
		game.pad[pad].shoot = 0;
		defaultSnd = false;
		sndPlay(sLife, sLife_size, 1, .5, 1);
	}
	else if(game.prize[prize].state == 9) levelWin();
	else if(game.prize[prize].state == 10) tileZap();
	else if(game.prize[prize].state == 11) ballSpeedDown();
	else if(game.prize[prize].state == 12) tilePlus();
	else if(game.prize[prize].state == 13) levelLose();
	else if(game.prize[prize].state == 14) ballSizeDown();
	else if(game.prize[prize].state == 15) ballSpeedUp();
	else if(game.prize[prize].state == 16) game.pad[pad].dlength -= game.pad[pad].length;
	else if(game.prize[prize].state == 17) game.pow.fall = 1;
	else if(game.prize[prize].state == 18) ballSizeUp();
	else if(game.prize[prize].state == 19){
		game.pow.nowall ^= 1;
		defaultSnd = false;
		sndPlay(sHold, sHold_size, 1, .5, 1);
	}
	else if(game.prize[prize].state == 20) resetItems();
	else if(game.prize[prize].state == 21) game.pow.rand+=.5;
	else if(game.prize[prize].state == 22) boltPrize();
	else if(game.prize[prize].state == 23) ballSpread();
	else if(game.prize[prize].state == 24) game.pow.pixelate = 16 + rand()%16;
	else if(game.prize[prize].state == 25) game.pow.flip ^= 1;
	else if(game.prize[prize].state == 26) game.pow.magnet += .001; //short-range technique used .5
	if(game.prize[prize].state <= 2 || game.prize[prize].state == 10 || game.prize[prize].state == 13 || game.prize[prize].state == 18 || game.prize[prize].state == 22 || game.prize[prize].state == 23)
		defaultSnd = false;
	if(defaultSnd)
		sndPlay(sBonus, sBonus_size, 1, .5, 1);
	game.dscore += rand()%25 + 75;
	game.prize[prize].state = -1;
}

void resetItems(){
	int i;
	for(i=0; i<MAXBALL; i++)
		if(game.ball[i].state >= 0){
			game.ball[i].vel = NORMBALLSPEED;
			game.ball[i].dsize = NORMBALLSIZE - game.ball[i].size;
		
		}
	for(i=0; i<4; i++)
		if(game.pad[i].state >= 0){
			game.pad[i].shoot = 0;
			game.pad[i].hold = 0;
			game.pad[i].dlength = NORMPADLENGTH - game.pad[i].length;
			padRelease(i);
		}
	game.pow.noclip = 0;
	game.pow.fire = 0;
	game.pow.fall = 0;
	game.pow.nowall = 0;
	game.pow.rand = 0;
	game.pow.pixelate = 0;
	game.pow.flip = 0;
	game.pow.magnet = 0;
}
