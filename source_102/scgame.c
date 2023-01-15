/*===========================================
        WiiXBall v1.0.2
        by Kirill M	
							
		(c) KMS 2010-2011
        A DXBall Clone
============================================*/
#include "WiiXBall.h"


//IN-GAME DRAWING AND CONTROL FUNCTIONS -----------------------------------------------------------------------------------

void scStart(){
	int i, j;
	if(sub == -2){
		srand(gettime());
		game.level = 0;
		game.lives = 2;
		game.score = 0;
		game.dscore = 0;
		game.time = 0;
		game.state = -1;
		for(i=0; i<4; i++){
			game.mouse[i].rumble = 0;
			game.pad[i].state = -1;
			game.pad[i].color = game.pcolor[i];
		}
	}
	else if(sub == -1){
		if(++game.level > game.boardcount){
			if(game.boardcount <= 0)
				screen = 1;
			else
				screen = 9;
		}
		else{
			for(i=0; i<20*20; i++){
				game.tile[i].state = game.boardset[(game.level-1)*20*20+i];
				game.tile[i].x = (i%20)*30 + 20;
				game.tile[i].y = (i/20)*15 + 50;
				game.tile[i].w = 30;
				game.tile[i].h = 15;
				game.tile[i].vx = 0;
				game.tile[i].vy = 0;
				game.tile[i].rotDeg = 0;
				game.tile[i].rotVel = 0;
			}
		}
	}
	else if(sub == 0){
		for(i=0; i<MAXSPARK; i++)
			game.spark[i].type = -1;
		for(i=0; i<MAXBOLT; i++)
			game.bolt[i].state = -1;
		for(i=0; i<MAXEXPLO; i++)
			game.explo[i].state = -1;
		for(i=0; i<MAXBALL; i++)
			game.ball[i].state = -1;
		for(i=0; i<MAXPRIZE; i++)
			game.prize[i].state = -1;
		for(i=0; i<4; i++)
			for(j=0; j<MAXBULLET; j++)
				game.pad[i].bullet[j].state = -1;
		game.pow.unarmor = 0;
		game.pow.boltvoice = -1;
		game.pow.boltfactor = 0;
		bpos = 0;  //will be bolt delay
		resetItems();
	}
	else if(sub == 64){
		if(padDetect() <= 0)
			return;
		for(i=0; i<4; i++)
			if(padOk(i))
				break;
		if(game.opts.wsound == 0)
			wsndOn();
		i = genBall(i, rand()%600/1000.0 + .2, 320, 460, NORMBALLSPEED, 0, NORMBALLSIZE);
		game.ball[i].dir = deg2rad*(160 - 140 * game.ball[i].holdpos);

		movePads();
		screen = 5;
		sub = -1;
		if(stopgame){
			screen = 6;
			sub = 0;
		}
	}
	else
		GRRLIB_FillScreen(RGBA(0, 0, 0, 256-(sub)*4));
	sub++;
}

void scGame(){
	int i, j;
	if(game.pow.nowall == 0){
		GRRLIB_DrawTile(0, 0, GFXGwall, 0, 1, 1, 0xFFFFFFFF,0);
		GRRLIB_DrawTile(620, 0, GFXGwall, 0, 1, 1, 0xFFFFFFFF,1);
	}
	for(i=0; i<20*20; i++)
		GRRLIB_DrawTile(game.tile[i].x, game.tile[i].y, GFXGtile, game.tile[i].rotDeg, 1, 1, 0xFFFFFFFF,game.tile[i].state);
	GRRLIB_Printf(30,2,GFXFscore,0xFFFFFFFF,1.2,"%d",game.score);
	for(i=1; i<=game.lives; i++)
		GRRLIB_DrawImg(620-i*33, 2, GFXGlife, 0, 1.5, 1.5, 0xFFFFFFFF);
		
	//explo -- anim is here too now
	GRRLIB_SetBlend(GRRLIB_BLEND_ADD);
	for(i=0; i<MAXEXPLO; i++)
		if(game.explo[i].state >= 0){
			GRRLIB_DrawTile(game.explo[i].x - 32*game.explo[i].size, game.explo[i].y - 32*game.explo[i].size, GFXGexplo, 0, game.explo[i].size, game.explo[i].size, 0xFFFFFFCC,game.explo[i].state++);
			if(game.explo[i].state > 44)
				game.explo[i].state = -1;
		}
	GRRLIB_SetBlend(GRRLIB_BLEND_ALPHA);
	
	//spark
	for(i=0; i<MAXSPARK; i++)
		if(game.spark[i].type >= 0)
			GRRLIB_Rectangle(game.spark[i].x, game.spark[i].y, 2, 2, game.spark[i].color, true);

	//pads & padhold & bullets
	for(i=0; i<4; i++){
		for(j=0; j<MAXBULLET; j++){
			if(game.pad[i].bullet[j].state >= 0)
				GRRLIB_DrawImg(game.pad[i].bullet[j].x-2, game.pad[i].bullet[j].y, GFXGbullet, 90-rad2deg*game.pad[i].bullet[j].dir, 1.5, 1.5, 0xFFFFFFFF);
		}
		if(game.pad[i].state >= 0){
			j = game.pad[i].hold;
			if(isPadHolding(i)){
				GRRLIB_DrawTile(game.pad[i].calcmid + 15*sin(deg2rad*game.pad[i].rotDeg) - (game.pad[i].calcholdlen/2)*cos(deg2rad*game.pad[i].rotDeg), game.pad[i].y - 15*cos(deg2rad*game.pad[i].rotDeg) - 66*sin(deg2rad*game.pad[i].rotDeg), GFXGhold, game.pad[i].rotDeg, game.pad[i].calcholdlen/132.0, 1, 0xFFFFFFFF, rand()%4);
				//noration -GRRLIB_DrawTile(game.pad[i].calcx + game.pad[i].calclen/16.0, game.pad[i].y - 15, GFXGhold, 0, (game.pad[i].calclen - game.pad[i].calclen/16.0)/132.0, 1, 0xFFFFFFFF, rand()%4);
				//veryold - GRRLIB_DrawTile(n + m/8, game.pad[i].y - 15, GFXGhold, game.pad[i].rotDeg, (m - m/4)/132, 1, 0xFFFFFFFF, rand()%4);
				j = 1;
			}
			if(game.pad[i].shoot)
				j = 2;
			GRRLIB_DrawTile(game.pad[i].calcmid + 15*sin(deg2rad*game.pad[i].rotDeg) - (game.pad[i].calclen/2)*cos(deg2rad*game.pad[i].rotDeg), game.pad[i].y - 15*cos(deg2rad*game.pad[i].rotDeg) - 148*sin(deg2rad*game.pad[i].rotDeg), GFXGpad, game.pad[i].rotDeg, game.pad[i].calclen/296.0, 1, game.pad[i].color, game.pad[i].state + 4*j);

			//no rotation- GRRLIB_DrawTile(game.pad[i].calcx, game.pad[i].y - 15, GFXGpad, 0, game.pad[i].calclen/296.0, 1, game.pad[i].color, game.pad[i].state + 4*j);
			//veryold - GRRLIB_DrawTile(n + 79.12*sin(deg2rad * (game.pad[i].rotDeg-45)), game.pad[i].y - 15 - 79.12*cos(deg2rad * (game.pad[i].rotDeg-45)), GFXGpad, game.pad[i].rotDeg, m/146, 1, game.pad[i].color, game.pad[i].state*3 + j);
		}
	}
		
	//prizes
	for(i=0; i<MAXPRIZE; i++)
		if(game.prize[i].state >= 0)
			GRRLIB_DrawTile(game.prize[i].x + 22.63*sin(deg2rad * (game.prize[i].rotDeg-45)), game.prize[i].y - 22.63*cos(deg2rad * (game.prize[i].rotDeg-45)), GFXGpower, game.prize[i].rotDeg, 1, 1, 0xFFFFFFFF, game.prize[i].state);		
	
	//balls - rotation currently unused!
	for(i=0; i<MAXBALL; i++)
		if(game.ball[i].state >= 0){
			if(game.pow.fire)
				GRRLIB_DrawTile(game.ball[i].x - game.ball[i].size/2.0, game.ball[i].y - game.ball[i].size/2.0, GFXGfire, 0, game.ball[i].size/64.0, game.ball[i].size/64.0, 0xFFFFFFFF, game.ball[i].state);
			else
				GRRLIB_DrawImg(game.ball[i].x - game.ball[i].size/2.0, game.ball[i].y - game.ball[i].size/2.0, GFXGball, 0, game.ball[i].size/64.0, game.ball[i].size/64.0, 0xFFFFFFFF);
		}
	
	//bolts
	for(i=0; i<MAXBOLT; i++)
		if(game.bolt[i].state >= 0){
			GRRLIB_FillScreen(RGBA(0,0,0,rand()%128+32));
			//GRRLIB_Printf(40,40+i*50,GFXFont,0xFFFFFFFF,2,"%d %d",i,game.bolt[i].state);
			GRRLIB_DrawImg(game.bolt[i].tx-80, game.bolt[i].ty-450, GFXGbolt, game.bolt[i].rotDeg, 1, 1, 0xFFFFFFFF);
		}
		
		
	//debug- draws the pad's baseline here
	//for(i=game.pad[0].calcx + game.pad[0].calclen; i >= game.pad[0].calcx; i--){
	//	GRRLIB_Rectangle(i, game.pad[0].y - game.pad[0].rotDeg * game.pad[0].calclen * deg2rad/2 + game.pad[0].rotDeg * (i - game.pad[0].calcx) * deg2rad, 1, 2, 0x0000FFFF, true);
	//}
	
	scFX();
}

void prAnim(){
	int i, j, k;
	for(i=0; i<20*20; i++){
		if(game.tile[i].state == 0x08)
			game.tile[i].state = 0x20;
		else if(game.tile[i].state >= 0x20 && game.tile[i].state <= 0x27)
			game.tile[i].state = (game.tile[i].state - 0x20 + 1) % 8 + 0x20;
		else if(game.tile[i].state == 0x28){
			game.tile[i].state++;
			for(j=-1; j<=1; j++)
				for(k=-1; k<=1; k++)
					if(i%20+k >= 0 && i%20+k < 20 && i/20+j >= 0 && i/20+j < 20){
						if(game.tile[(i/20+j)*20+(i%20+k)].state >= 0x20 && game.tile[(i/20+j)*20+(i%20+k)].state <= 0x27)
							game.tile[(i/20+j)*20+(i%20+k)].state = 0x28;
						else if(game.tile[(i/20+j)*20+(i%20+k)].state > 0x0 && game.tile[(i/20+j)*20+(i%20+k)].state < 0x20)
							game.tile[(i/20+j)*20+(i%20+k)].state = 0x29;
					}
			if(rand()%PF_EXPLO == 0)
				genPrize(-1, game.tile[i].x + game.tile[i].w/2, game.tile[i].y + game.tile[i].h/2);
			j = rand()%3;
			sndPlay(sExp[j], sExp_size[j], rand()%30/100.0 + .7, game.tile[i].x / 640.0 * .6 + .2, 8000);
		}
		else if(game.tile[i].state >= 0x29 && game.tile[i].state <= 0x2E){
			game.tile[i].state++;
			if(game.tile[i].state == 0x2F){
				game.tile[i].state = 0x0;
				game.dscore += rand()%5 + 15;
				checkWin();
				//there's acommented out s:br_exp[n]
			}
		}
		else if(game.tile[i].state == 0x19){
			game.tile[i].state = 0x06;
			game.dscore += rand()%5 + 9;
			if(game.pow.noclip)
				game.tile[i].state = 0x1b;
			sndPlay(sHid, sHid_size, rand()%20/100.0 + .75, game.tile[i].x / 640.0 * .6 + .2, 8000);
			genSpark(1, game.tile[i].x + game.tile[i].w/2, game.tile[i].y + game.tile[i].h/2, game.tile[i].w, game.tile[i].h, 5, 5, rand()%10 + 6);
		}
		else if(game.tile[i].state == 0x1a){
			sndPlay(sBr, sBr_size, rand()%20/100.0 + .75, game.tile[i].x / 640.0 * .6 + .2, 8000);
			game.tile[i].state++;
		}
		else if(game.tile[i].state == 0x1b){
			game.tile[i].state = 0x0;
			game.dscore += rand()%5 + 10;
			checkWin();
		}
		else if(game.tile[i].state == 0x1c){
			game.tile[i].state = 0x04;
			game.dscore += rand()%5 + 7;
			if(game.pow.noclip)
				game.tile[i].state = 0x1b;
			sndPlay(sSb, sSb_size, rand()%20/100.0 + .75, game.tile[i].x / 640.0 * .6 + .2, 8000);
		}
		else if(game.tile[i].state == 0x1d){
			game.tile[i].state = 0x05;
			game.dscore += rand()%5 + 7;
			if(game.pow.noclip)
				game.tile[i].state = 0x1b;
			sndPlay(sSb, sSb_size, rand()%20/100.0 + .75, game.tile[i].x / 640.0 * .6 + .2, 8000);
		}
		else if(game.tile[i].state == 0x1e){
			game.tile[i].state = 0x02;
			if(game.pow.noclip)
				game.tile[i].state = 0x1b;
			sndPlay(sUsb, sUsb_size, rand()%20/100.0 + .75, game.tile[i].x / 640.0 * .6 + .2, 8000);
		}
		else if(game.tile[i].state == 0x1f){
			game.tile[i].state = 0x02;
			game.dscore += rand()%5 + 6;
			if(game.pow.noclip)
				game.tile[i].state = 0x1b;
			sndPlay(sSb, sSb_size, rand()%20/100.0 + .75, game.tile[i].x / 640.0 * .6 + .2, 8000);
			checkWin();
		}
		else if(game.tile[i].state == 0x31){
			game.tile[i].state = 0x17;
			game.dscore += rand()%10 + 16;
			sndPlay(sSb, sSb_size, rand()%20/100.0 + .75, game.tile[i].x / 640.0 * .6 + .2, 8000);
		}
		else if(game.tile[i].state == 0x32){
			game.tile[i].state = 0x18;
			game.dscore += rand()%10 + 16;
			sndPlay(sSb, sSb_size, rand()%20/100.0 + .75, game.tile[i].x / 640.0 * .6 + .2, 8000);
		}
	}
	//score
	j = game.dscore / 2;
	game.score += j;
	game.dscore -= j;
	
	//spark
	for(i=0; i<MAXSPARK; i++)
		if(game.spark[i].type >= 0){
			u8 r, g, b, a;
			r = game.spark[i].color;
			g = game.spark[i].color >> 8;
			b = game.spark[i].color >> 16;
			a = game.spark[i].color >> 24;
			if (r) r-=5;
			if (g) g-=5;
			if (b) b-=5;
			if (a) a-=5;
			game.spark[i].color = RGBA(r,g,b,a);
		}
	
	//bolts -- very primitive, perhaps add rotation later?
	for(i=0; i<MAXBOLT; i++)
		if(game.bolt[i].state >= 0){
			game.bolt[i].state++;
			if(game.bolt[i].state > 4)
				game.bolt[i].state = -1;
		}
	
	if(game.state != 1 || screen != 5) //don't change sizes if game not active
		return;
	
	//pads
	for(i=0; i<4; i++)
		if(game.pad[i].state >= 0){
			game.pad[i].state++;
			game.pad[i].state %= 4;
			j = game.pad[i].dlength / 2;
			game.pad[i].length += j;
			game.pad[i].dlength -= j;
			if(game.pad[i].length < 32)
				game.pad[i].length = 32;
			if(game.pad[i].length > 386)
				game.pad[i].length = 386;
		}
		
	//balls
	for(i=0; i<MAXBALL; i++)
		if(game.ball[i].state >= 0){
			game.ball[i].state++;
			game.ball[i].state %= 8;
			j = game.ball[i].dsize / 2;
			game.ball[i].size += j;
			game.ball[i].dsize -= j;
			if(game.ball[i].size < 5)
				game.ball[i].size = 5;
			if(game.ball[i].size > 48)
				game.ball[i].size = 48;
		}
		
	if(game.pow.unarmor > 64 && game.pow.unarmor != -1){
		tileZap();
		game.pow.unarmor = -1;
	}
	if(game.pow.pixelate > 0)
		game.pow.pixelate-=.2;
		
	checkBolt();
}

void prGame(){
	//note that this happens GPD times per frame. All time-sensitive operations must use GPD factor!
	int i, j, k;
	double m, n;
	
	game.time ++;
	
	//balls
	for(i=0; i<MAXBALL; i++){
		if(game.ball[i].state >= 0){
			game.ball[i].dir+=game.ball[i].ddir;
			game.ball[i].ddir = 0;
			if(game.ball[i].held >= 0){
				j = game.ball[i].held;
				if(game.pad[j].state >= 0){
					game.ball[i].x = game.pad[j].calcx + game.pad[j].calclen*game.ball[i].holdpos;
					game.ball[i].y = game.pad[j].y - game.pad[j].rotDeg * game.pad[j].calclen/2 * deg2rad + game.pad[j].rotDeg * (game.ball[i].x - game.pad[j].calcx) * deg2rad - game.ball[i].size/2 + 2;
				}
				else
					game.ball[i].held = -1;
			}
			else{
				//movement
				m = game.ball[i].vel * cos(game.ball[i].dir);
				n = game.ball[i].vel * sin(game.ball[i].dir);
				game.ball[i].x += m/GPD;
				game.ball[i].y -= n/GPD;
				m += ((rand()%1000 / 100.0 - 5) * game.pow.rand)/GPD;
				n += ((rand()%1000 / 100.0 - 5) * game.pow.rand)/GPD;
				game.ball[i].vel += (.001)/GPD;
				
				//brick homing
				// old technique - long distance - sorta works
				if(game.pow.magnet > 0 && game.ball[i].y < 360){
					j = getNearestTileFromPos(game.ball[i].x, game.ball[i].y);
					if(j >= 0 && game.tile[j].x >= 0 && game.tile[j].y >= 0){
						//GRRLIB_Rectangle(game.tile[j].x + game.tile[j].w/2, game.tile[j].y + game.tile[j].h/2, 10, 10, 0xFFFFFFFF, true);
						m += ((game.tile[j].x - game.ball[i].x + game.tile[j].w/2)*game.pow.magnet)/GPD;
						n -= ((game.tile[j].y - game.ball[i].y + game.tile[j].h/2)*game.pow.magnet)/GPD;
					}
				}
				
				//bounds
				if(game.ball[i].x - game.ball[i].size/2 < 20 && game.pow.nowall == 0){
					game.ball[i].x = 20 + game.ball[i].size/2;
					m *= -1;
					sndPlay(sHitwall, sHitwall_size, .8, .1, 8000);
				}
				if(game.ball[i].x + game.ball[i].size/2 > 620 && game.pow.nowall == 0){
					game.ball[i].x = 620 - game.ball[i].size/2;
					m *= -1;
					sndPlay(sHitwall, sHitwall_size, .8, .9, 8000);
				}
				if(game.ball[i].x < 0)
					game.ball[i].x = 640;
				if(game.ball[i].x > 640)
					game.ball[i].x = 0;
				if(game.ball[i].y - game.ball[i].size/2 < 0){
					game.ball[i].y = game.ball[i].size/2;
					n *= -1;
					sndPlay(sHitwall, sHitwall_size, .8, game.ball[i].x / 640.0 * .8 + .1, 8000);
				}
				//paddle hittin'
				for(j=0; j<4; j++){
					if(game.pad[j].state >= 0){
						k = game.pad[j].y - game.pad[j].rotDeg * game.pad[j].calclen/2 * deg2rad + game.pad[j].rotDeg * (game.ball[i].x - game.pad[j].calcx) * deg2rad; //baseline
						//region 1 (left)
						if(game.ball[i].x + game.ball[i].size/2 >= game.pad[j].calcx + game.pad[j].calclen * .02 && game.ball[i].x < game.pad[j].calcx + game.pad[j].calclen * .07){
							if(game.ball[i].y + game.ball[i].size/2 > k+1 && game.ball[i].y < k+5) //catch
								ballHitPad(i, j, k, &m, &n);
							else if(game.ball[i].y >= k+5 && game.ball[i].y - game.ball[i].size/2 < k+14){ //bump left
								game.ball[i].x = game.pad[j].calcx - game.ball[i].size/2;
								if(m > 0)
									m *= -1;
								m-=1; //maybe replace by pad's velocity later?
								sndPlay(sPadlow, sPadlow_size, .4, game.pad[j].x / 640.0 * .6, 8000);
							}
						}
						//region 2 (center)
						else if(game.ball[i].x >= game.pad[j].calcx + game.pad[j].calclen * .07 && game.ball[i].x <= game.pad[j].calcx + game.pad[j].calclen * .93){
							if(game.ball[i].y + game.ball[i].size/2 > k+1 && game.ball[i].y < k+10) //catch
								ballHitPad(i, j, k, &m, &n);
							else if(game.ball[i].y >= k+10 && game.ball[i].y - game.ball[i].size/2 < k+14){ //bump down
								game.ball[i].y = k+15 + game.ball[i].size/2;
								if(n > 0)
									n *= -1;
								sndPlay(sPadlow, sPadlow_size, .4, game.pad[j].x / 640.0 * .6 + .2, 8000);
							}
						}
						//region 3 (right)
						else if(game.ball[i].x >= game.pad[j].calcx + game.pad[j].calclen * .93 && game.ball[i].x - game.ball[i].size/2 <= game.pad[j].calcx + game.pad[j].calclen * .98){
							if(game.ball[i].y + game.ball[i].size/2 > k+1 && game.ball[i].y < k+5) //catch
								ballHitPad(i, j, k, &m, &n);
							else if(game.ball[i].y >= k+5 && game.ball[i].y - game.ball[i].size/2 < k+14){ //bump right
								game.ball[i].x = game.pad[j].calcx + game.pad[j].calclen + game.ball[i].size/2;
								if(m < 0)
									m *= -1;
								m+=1;
								sndPlay(sPadlow, sPadlow_size, .4, game.pad[j].x / 640.0 * .6 + .4, 8000);
							}
						}
					}
				}		
				
				//brick breakin'
				for(j=0; j<20*20; j++){
					if(((game.tile[j].state > 0x0 && game.tile[j].state <= 0x19) || (game.tile[j].state >= 0x1c && game.tile[j].state < 0x2f)) && game.ball[i].x + game.ball[i].size/2 >= game.tile[j].x && game.ball[i].x - game.ball[i].size/2 <= game.tile[j].x + game.tile[j].w && game.ball[i].y + game.ball[i].size/2 >= game.tile[j].y && game.ball[i].y - game.ball[i].size/2 <= game.tile[j].y + game.tile[j].h){
						ballHitTile(i, j);
						if(game.pow.fire){
							game.tile[j].state = 0x28;
							genExplo(game.ball[i].x, game.ball[i].y);
						}
						if(game.tile[j].state != 0x1e && game.pow.unarmor != -1)
							game.pow.unarmor = 0;
						if(game.pow.noclip)
							break;
						if(game.tile[j].state == 0x1e && game.pow.unarmor != -1)
							game.pow.unarmor++;
						if(game.ball[i].x + game.ball[i].size/2 - m/GPD < game.tile[j].x){ //left hit
							game.ball[i].x = game.tile[j].x - game.ball[i].size/2;
							m *= -1;
						}
						if(game.ball[i].x - game.ball[i].size/2 - m/GPD > game.tile[j].x + game.tile[j].w){ //right hit
							game.ball[i].x = game.tile[j].x + game.tile[j].w + game.ball[i].size/2;
							m *= -1;
						}
						if(game.ball[i].y + game.ball[i].size/2 + n/GPD < game.tile[j].y){ //top hit
							game.ball[i].y = game.tile[j].y - game.ball[i].size/2;
							n *= -1;
						}
						if(game.ball[i].y - game.ball[i].size/2 + n/GPD > game.tile[j].y + game.tile[j].h){ //bottom hit
							game.ball[i].y = game.tile[j].y + game.tile[j].h + game.ball[i].size/2;
							n *= -1;
						}
						break;
					}
				}
				//losing
				if(game.ball[i].y > 480 + game.ball[i].size){
					game.ball[i].state = -1;
					sndPlay(sLose, sLose_size, .9, game.ball[i].x / 640.0 * .6 + .2, 4000);
					checkLose();
				}
				//fireball sparks
				if(game.pow.fire && rand()%(8*GPD) == 0)
					genSpark(3, game.ball[i].x, game.ball[i].y, game.ball[i].size, game.ball[i].size, m, n/2, 1);
				//update direction
				if(game.pow.magnet == 0){
					if(n < .5 && n >= 0)
						n = .5;
					if(n > -.5 && n < 0)
						n = -.5;
				}
				game.ball[i].dir = atan(n/m);
				if(m < 0)
					game.ball[i].dir += deg2rad*180;
						
			}
		}
	}
	
	//powerups
	for(i=0; i<MAXPRIZE; i++){
		if(game.prize[i].state >= 0){
			//movement
			game.prize[i].x += (game.prize[i].vx)/GPD;
			game.prize[i].y -= (game.prize[i].vy)/GPD;
			game.prize[i].vy -= (.1)/GPD;
			game.prize[i].rotDeg += (game.prize[i].rotVel)/GPD;
			game.prize[i].rotVel *= 1.0-(.01/GPD);
			if(game.prize[i].rotDeg > 360)
				game.prize[i].rotDeg -= 360;
			//bounds
			if(game.prize[i].x - 16 < 20 && game.pow.nowall == 0){
				game.prize[i].x = 20 + 16;
				game.prize[i].vx *= -1;
				game.prize[i].rotVel *= -1;
				sndPlay(sHitwall, sHitwall_size, .8, .1, 8000);
			}
			if(game.prize[i].x + 16 > 620 && game.pow.nowall == 0){
				game.prize[i].x = 620 - 16;
				game.prize[i].vx *= -1;
				game.prize[i].rotVel *= -1;
				sndPlay(sHitwall, sHitwall_size, .8, .9, 8000);
			}
			if(game.prize[i].x < 0)
				game.prize[i].x = 640;
			if(game.prize[i].x > 640)
				game.prize[i].x = 0;
			if(game.prize[i].y > 500)
				game.prize[i].state = -1; //its gone
				
			//paddle hittin'
			for(j=0; j<4; j++){
				if(game.pad[j].state >= 0){
					k = game.pad[j].y - game.pad[j].rotDeg * game.pad[j].calclen/2 * deg2rad + game.pad[j].rotDeg * (game.prize[i].x - game.pad[j].calcx) * deg2rad; //baseline
					//region 1 (left)
					if(game.prize[i].x + 12 >= game.pad[j].calcx + game.pad[j].calclen * .02 && game.prize[i].x < game.pad[j].calcx + game.pad[j].calclen * .07){
						if(game.prize[i].y + 12 >= k+8 && game.prize[i].y < k+5) //catch
							prizeHitPad(i, j);
						else if(game.prize[i].y >= k+5 && game.prize[i].y - 12 < k+14){ //bump left
							game.prize[i].x = game.pad[j].calcx - 12;
							if(game.prize[i].vx > 0)
								game.prize[i].vx *= -1;
							game.prize[i].vx-=1;
							sndPlay(sPadlow, sPadlow_size, .2, game.pad[j].x / 640.0 * .6, 8000);
						}
					}
					//region 2 (center)
					else if(game.prize[i].x >= game.pad[j].calcx + game.pad[j].calclen * .07 && game.prize[i].x <= game.pad[j].calcx + game.pad[j].calclen * .93){
						if(game.prize[i].y + 12 >= k+8 && game.prize[i].y < k+10) //catch
							prizeHitPad(i, j);
						else if(game.prize[i].y >= k+10 && game.prize[i].y - 12 < k+14){ //bump down
							game.prize[i].y = k+15 + 12;
							game.prize[i].vy -=1;
							sndPlay(sPadlow, sPadlow_size, .2, game.pad[j].x / 640.0 * .6 + .2, 8000);
						}
					}
					//region 3 (right)
					else if(game.prize[i].x >= game.pad[j].calcx + game.pad[j].calclen * .93 && game.prize[i].x - 12 <= game.pad[j].calcx + game.pad[j].calclen * .98){
						if(game.prize[i].y + 12 >= k+8 && game.prize[i].y < k+5) //catch
							prizeHitPad(i, j);
						else if(game.prize[i].y >= k+5 && game.prize[i].y - 12 < k+14){ //bump right
							game.prize[i].x = game.pad[j].calcx + game.pad[j].calclen + 12;
							if(game.prize[i].vx < 0)
								game.prize[i].vx *= -1;
							game.prize[i].vx+=1;
							sndPlay(sPadlow, sPadlow_size, .2, game.pad[j].x / 640.0 * .6 + .4, 8000);
						}
					}
				}
			}
		}
	}
	
	//bullets
	for(i=0; i<4; i++){
		for(j=0; j<MAXBULLET; j++){
			if(game.pad[i].bullet[j].state >= 0){
				//movement
				game.pad[i].bullet[j].x += (game.pad[i].bullet[j].vel * cos(game.pad[i].bullet[j].dir))/GPD;
				game.pad[i].bullet[j].y -= (game.pad[i].bullet[j].vel * sin(game.pad[i].bullet[j].dir))/GPD;
				//bounds
				if(game.pad[i].bullet[j].y < -16)
					game.pad[i].bullet[j].state = -1; //gone
				if(game.pad[i].bullet[j].x < 20 && game.pow.nowall == 0){
					game.pad[i].bullet[j].state = -1;
					genExplo(game.pad[i].bullet[j].x, game.pad[i].bullet[j].y);
				}
				if(game.pad[i].bullet[j].x > 620 && game.pow.nowall == 0){
					game.pad[i].bullet[j].state = -1;
					genExplo(game.pad[i].bullet[j].x, game.pad[i].bullet[j].y);
				}
				if(game.pad[i].bullet[j].x < 0)
					game.pad[i].bullet[j].x = 640;
				if(game.pad[i].bullet[j].x > 640)
					game.pad[i].bullet[j].x = 0;
				
				//brick breakin'
				for(k=0; k<20*20; k++){
					if(((game.tile[k].state > 0x0 && game.tile[k].state <= 0x19) || (game.tile[k].state >= 0x1c && game.tile[k].state < 0x2f)) && game.pad[i].bullet[j].x >= game.tile[k].x && game.pad[i].bullet[j].x <= game.tile[k].x + game.tile[k].w && game.pad[i].bullet[j].y >= game.tile[k].y && game.pad[i].bullet[j].y <= game.tile[k].y + game.tile[k].h){
						ballHitTile(-1, k);
						if(game.pow.noclip)
							break;
						game.pad[i].bullet[j].state = -1;
						break;
					}
				}
			}
		}
	}
	
	//sparks
	for(i=0; i<MAXSPARK; i++){
		if(game.spark[i].type >= 0){
			game.spark[i].x += (game.spark[i].vx)/GPD;
			game.spark[i].y -= (game.spark[i].vy)/GPD;
			game.spark[i].vy -= (.2)/GPD;
			if(game.spark[i].y > 500)
				game.spark[i].type = -1; //gone
		}
	}
	
}

void ctGame(){
	int i;
	if(padDetect() <= 0)
		return;
	updMice();
	movePads();
	pointDetect();
	for(i = 0; i < 4; i++){
		if(wd[i]->btns_d & (WPAD_BUTTON_A | WPAD_BUTTON_B)){
			if(game.pad[i].shoot)
				genBullet(i);
			padRelease(i);
		}
		
		//debug goes here
		/*
		if(wd[i]->btns_h & (WPAD_BUTTON_A))
			//game.pad[i].length = game.pad[i].length % 385 + 1;
		if(wd[i]->btns_h & (WPAD_BUTTON_1)){
			genPrize(-1, 320, 0);
			game.prize[0].state = 4;
		}*/
		
	}
}

void scEnd(){
	if(game.pow.boltvoice != -1 && sndActive(game.pow.boltvoice))
		sndStop(game.pow.boltvoice);
	updMice();  //gets rid of leftover rumble
	if(sub < 63){
		GRRLIB_DrawImg(0,0,GBuf[0],0,1,1,0xFFFFFFFF);
		GRRLIB_FillScreen(RGBA(0, 0, 0, sub*4));
	}
	else if(sub == 63){
		wsndOff();
		stopgame = turnoff; //don't exit the entire program, unless user wants to shutdown completely
		GRRLIB_FillScreen(0xFF);
		screen = 1;
		sub = -1;
	}
	else if(sub < 127){
		GRRLIB_FillScreen(0xFF);
		GRRLIB_BMFX_Scatter(GBuf[sub%2], GBuf[(sub+1)%2], 1);
		GRRLIB_DrawImg(0,0,GBuf[(sub+1)%2],0,1,1,0xFFFFFFFF);
		GRRLIB_FillScreen(RGBA(0, 0, 0, (sub%64)*4));
	}
	else if(sub == 127){
		wsndOff();
		game.pow.flip = 0;
		game.pow.pixelate = 0;
		GRRLIB_FillScreen(0xFF);
		screen = 4;
		sub = -2;
	}
	else if(sub < 127 + 32){
		GRRLIB_DrawImg(0,0,GBuf[0],0,1,1,0xFFFFFFFF);
		GRRLIB_SetBlend(GRRLIB_BLEND_INV);
		GRRLIB_Rectangle(0,0,640,480,RGBA((sub%64)*4,(sub%64)*4,(sub%64)*4,255),true);
		GRRLIB_SetBlend(GRRLIB_BLEND_ALPHA);
	}
	else if(sub == 127 + 32){
		GRRLIB_BMFX_Grayscale(GBuf[0], GBuf[1]);
		GRRLIB_FillScreen(0xDDDDDDFF);
	}
	else if(sub < 127 + 64){
		GRRLIB_DrawImg(0,0,GBuf[1],0,1,1,0xFFFFFFFF);
		GRRLIB_SetBlend(GRRLIB_BLEND_INV);
		GRRLIB_Rectangle(0,0,640,480,RGBA((64 - sub%64)*4,(64 - sub%64)*4,(64 - sub%64)*4,255),true);
		GRRLIB_SetBlend(GRRLIB_BLEND_ALPHA);	
	}
	else if(sub < 255)
		GRRLIB_DrawImg(0,0,GBuf[1],0,1,1,0xFFFFFFFF);
	else if(sub < 255 + 64){
		GRRLIB_DrawImg(0,0,GBuf[1],0,1,1,0xFFFFFFFF);
		GRRLIB_FillScreen(RGBA(0, 0, 0, ((sub+1)%64)*4));
	}
	else if(sub == 255 + 64){
		wsndOff();
		game.pow.flip = 0;
		game.pow.pixelate = 0;
		game.lives--;
		if(game.lives < 0)
			screen = 8;
		else
			screen = 4;
		sub = -1;
	}	
	sub++;
}

void scPause(){
	int i;
	if(stopgame && sub < 32*2+1)
		sub = 32*2+1;
	GRRLIB_DrawImg(0,0,GBuf[0],0,1,1,0xFFFFFFFF);
	if(sub == 0){
		sndPlay16(sPause, sPause_size, 1, .5, 0);
		showKeys(86, 88);
		sub++;
	}
	else if(sub < 32){
		GRRLIB_FillScreen(sub*5);
		GRRLIB_Rectangle(80, 60-8*(32-sub), 480, 320, 0x55558800 + sub*4, true);
		sub++;
	}
	else if(sub == 32){
		GRRLIB_FillScreen(0xA0);
		GRRLIB_Rectangle(80, 60, 480, 360, 0x55558880, true);
		GRRLIB_Printf(320-40*2.5, 80, game.gfont[FONT_8BIT].img, 0xAACCFFFF, 2.5, "Pause");
		GRRLIB_Printf(160, 160, game.gfont[FONT_GR].img, 0xFFAA77FF, 1.5, "Board #");
		GRRLIB_Printf(160, 160+48, game.gfont[FONT_GR].img, 0xFFAA77FF, 1.5, "Lives");
		GRRLIB_Printf(160, 160+48*2, game.gfont[FONT_GR].img, 0xFFAA77FF, 1.5, "Score");
		GRRLIB_Printf(480-24*(floor(log10(game.level))+1), 160, game.gfont[FONT_GR].img, 0xAAFF11FF, 1.5, "%d", game.level);
		i = log10(game.lives);
		if(i<0) i=0;
		GRRLIB_Printf(480-24*(i+1), 160+48, game.gfont[FONT_GR].img, 0xAAFF11FF, 1.5, "%d", game.lives);
		i = log10(game.score);
		if(i<0) i=0;
		GRRLIB_Printf(480-24*(i+1), 160+48*2, game.gfont[FONT_GR].img, 0xAAFF11FF, 1.5, "%d", game.score);
		for(i=86; i<88; i++)
			drawButton(game.button[i]);
		for(i=3; i>=0; i--)
			if(game.mouse[i].x >= 0 && game.mouse[i].y >= 0)
				GRRLIB_DrawImg(game.mouse[i].x + 67.88*sin(deg2rad * (game.mouse[i].rotDeg-45)), game.mouse[i].y - 67.88*cos(deg2rad * (game.mouse[i].rotDeg-45)), GFXCursor, game.mouse[i].rotDeg, 1, 1, game.pcolor[i]);		
	}
	else if(sub == 32*2){
		screen = 5;
		sub = 0;
	}
	else if(sub == 32*3){
		screen = 6;
		sub = 0;
	}
	else{
		if(sub%32==1)
			sndPlay16(sPauseoff, sPauseoff_size, 1, .5, 0);
		GRRLIB_FillScreen(0xA0-(sub%32)*5);
		GRRLIB_Rectangle(80, 60+8*(sub%32), 480, 320, 0x55558800 + (32-sub%32)*4, true);
		sub++;
	}
}

void ctPause(){
	int i, j;
	char btSel[NUMBUTTONS];
	updMice();
	if(sub != 32)
		return;
	for(i = 86; i < 88; i++)
		btSel[i] = 0;
	for(i = 86; i < 88; i++){
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
					game.button[i].scale = 1;
					if(i==86)
						sub++;
					else if(i==87)
						sub+=33;
					game.mouse[j].rumble = 12;
				}
			}
		}
		if(game.button[i].state > 0 && btSel[i] == 0){
			game.button[i].state = 0;
			game.button[i].scale = 1;
		}
	}
}
