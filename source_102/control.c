/*===========================================
        WiiXBall v1.0.2
        by Kirill M	
							
		(c) KMS 2010-2011
        A DXBall Clone
============================================*/
#include "WiiXBall.h"


//GAMEPAD FUNCTIONS------------------------------------------------------------------------------------------------------------

void initPads(){
	int i;
	WPAD_Init();
	WPAD_SetIdleTimeout( 60*3 );
	for(i=0; i<4; i++){
		WPAD_SetVRes(WPAD_CHAN_0 + i, rmode->fbWidth, rmode->xfbHeight);
		WPAD_SetDataFormat(WPAD_CHAN_0 + i, WPAD_FMT_BTNS_ACC_IR);
		game.mouse[i].enable = 1;
		game.mouse[i].dctr = 0;
		game.mouse[i].rumble = 0;
		game.mouse[i].x = -1;
		game.mouse[i].y = -1;
		game.mouse[i].vx = 0;
		game.mouse[i].vy = 0;
		game.mouse[i].vz = 0;
		
		/*
		game.mouse[i].axct=0;
		game.mouse[i].ayct=0;
		game.mouse[i].azct=0;
		game.mouse[i].axsum=0;
		game.mouse[i].aysum=0;
		game.mouse[i].azsum=0;*/
	}
}

void updPads(){
	WPAD_ScanPads();  // Scan the wiimotes
	//WPAD_ReadPending(WPAD_CHAN_ALL, countevs);
	/*wpKd[0] = WPAD_ButtonsDown(WPAD_CHAN_0);
	wpKd[1] = WPAD_ButtonsDown(WPAD_CHAN_1);
	wpKd[2] = WPAD_ButtonsDown(WPAD_CHAN_2);
	wpKd[3] = WPAD_ButtonsDown(WPAD_CHAN_3);
	wpKh[0] = WPAD_ButtonsHeld(WPAD_CHAN_0);
	wpKh[1] = WPAD_ButtonsHeld(WPAD_CHAN_1);
	wpKh[2] = WPAD_ButtonsHeld(WPAD_CHAN_2);
	wpKh[3] = WPAD_ButtonsHeld(WPAD_CHAN_3);
	wpKu[0] = WPAD_ButtonsUp(WPAD_CHAN_0);
	wpKu[1] = WPAD_ButtonsUp(WPAD_CHAN_1);
	wpKu[2] = WPAD_ButtonsUp(WPAD_CHAN_2);
	wpKu[3] = WPAD_ButtonsUp(WPAD_CHAN_3);
	WPAD_IR(WPAD_CHAN_0, &(wpIr[0]));
	WPAD_IR(WPAD_CHAN_1, &(wpIr[1]));
	WPAD_IR(WPAD_CHAN_2, &(wpIr[2]));
	WPAD_IR(WPAD_CHAN_3, &(wpIr[3]));*/
	wd[0] = WPAD_Data(WPAD_CHAN_0);
	wd[1] = WPAD_Data(WPAD_CHAN_1);
	wd[2] = WPAD_Data(WPAD_CHAN_2);
	wd[3] = WPAD_Data(WPAD_CHAN_3);
}

bool padOk(int chan){
	if(wd[chan]->data_present >= 11 && game.mouse[chan].enable) //11-ir&accel&buttons
		return true;
	return false;
}

int padDetect(){
	int i, found = 0;
	for(i=0; i<4; i++)
		if(padOk(i))
			found++;
	if(found == 0){
		game.state = 0;
		GRRLIB_FillScreen(0xE0);
		GRRLIB_Printf(40,160,game.gfont[FONT_SYS].img,0xCCCCFFFF,2,"Please connect at least one WiiMote");
		GRRLIB_Printf(150,200,game.gfont[FONT_SYS].img,0xCCCCFFFF,2,"to your Wii console!");
		GRRLIB_Printf(320-240,300,game.gfont[FONT_SYS].img,0xDDDDFFBB,1.5,"You can drop out of the game at any time");
		GRRLIB_Printf(320-252,328,game.gfont[FONT_SYS].img,0xDDDDFFBB,1.5,"by holding the minus button for 3 seconds.");
		GRRLIB_Printf(320-264,380,game.gfont[FONT_SYS].img,0xDDDDFFBB,1.5,"To rejoin, press any button on your WiiMote.");
		//GRRLIB_Printf(420,420,game.gfont[FONT_SYS].img,0xDDDDFFFF,1,"Wiimote data code: %d",wd[0]->data_present);
	}
	else
		game.state = 1;
	return found;
}

bool posValid(int chan){
	if(getBoundX(chan) >= 0 && getBoundY(chan) >= 0)
		return true;
	return false;
}

int pointDetect(){
	int i, points = 0;
	for(i=0; i<4; i++)
		if(posValid(i))
			points++;
	if(points == 0){
		GRRLIB_Rectangle(80, 150, 480, 128, 0x55558880, true);
		GRRLIB_Printf(320-8*25, 150+24, game.gfont[FONT_SYS].img, 0xFFFFAADD, 2, "Please point your WiiMote");
		GRRLIB_Printf(320-8*14, 150+24+40, game.gfont[FONT_SYS].img, 0xFFFFAADD, 2, "at the screen.");
	}
	return points;
}

void updMice(){
	int i;
	for(i = 0; i < 4; i++){
		if(game.mouse[i].rumble > 0){
			game.mouse[i].rumble--;
			if(game.opts.rumble)
				WPAD_Rumble(WPAD_CHAN_0 + i, 0);
			else
				WPAD_Rumble(WPAD_CHAN_0 + i, game.mouse[i].rumble);
		}
		if(padOk(i) && getBoundX(i) >= 0){
			if(game.mouse[i].x < 0)
				game.mouse[i].x = getBoundX(i);
			game.mouse[i].x = (game.mouse[i].x + getBoundX(i))/2;
		}
		else
			game.mouse[i].x = -1;
		if(padOk(i) && getBoundY(i) >= 0){
			if(game.mouse[i].y < 0)
				game.mouse[i].y = getBoundY(i);
			game.mouse[i].y = (game.mouse[i].y + getBoundY(i))/2;
		}
		else
			game.mouse[i].y = -1;
		if(padOk(i)){
			game.mouse[i].rotDeg = wd[i]->orient.roll;
			//game.mouse[i].ax = wd[i]->gforce.x;
			//game.mouse[i].ay = wd[i]->gforce.y;
			//game.mouse[i].az = wd[i]->gforce.z;
			
			/*
			game.mouse[i].axavg = (game.mouse[i].axavg + wd[i]->gforce.x)/2.0;
			game.mouse[i].ayavg = (game.mouse[i].ayavg + wd[i]->gforce.y)/2.0;
			game.mouse[i].azavg = (game.mouse[i].azavg + wd[i]->gforce.z)/2.0;
			game.mouse[i].vx += game.mouse[i].axavg;
			game.mouse[i].vy += game.mouse[i].ayavg;
			game.mouse[i].vz += game.mouse[i].azavg;
			game.mouse[i].vx *= .8;
			game.mouse[i].vy *= .8;
			game.mouse[i].vz *= .8;
			
			if(fabs(game.mouse[i].vy) < .2)
				game.mouse[i].vy = 0;
			
			game.mouse[i].x += game.mouse[i].vy * 8;
			*/
			
			/*
			game.mouse[i].ax = game.mouse[i].axcal - wd[i]->accel.x;
			game.mouse[i].ay = game.mouse[i].aycal - wd[i]->accel.y;
			game.mouse[i].az = game.mouse[i].azcal - wd[i]->accel.z;
			if(game.mouse[i].axct < 1024 || abs(game.mouse[i].ax) < 32){
				game.mouse[i].axsum += wd[i]->accel.x;
				game.mouse[i].axct++;
			}
			if(game.mouse[i].ayct < 1024 || abs(game.mouse[i].ay) < 32){
				game.mouse[i].aysum += wd[i]->accel.y;
				game.mouse[i].ayct++;
			}
			if(game.mouse[i].azct < 1024 || abs(game.mouse[i].az) < 32){
				game.mouse[i].azsum += wd[i]->accel.z;
				game.mouse[i].azct++;
			}
			game.mouse[i].axcal = game.mouse[i].axsum/game.mouse[i].axct;
			game.mouse[i].aycal = game.mouse[i].aysum/game.mouse[i].ayct;
			game.mouse[i].azcal = game.mouse[i].azsum/game.mouse[i].azct;*/

		}
		
		/*
		if(wd[i]->ir.smooth_valid){
			n = wd[i]->ir.x;  //used sx & sy before, sometimes was unresponsive...
			if(n < 0)
				n = 0;
			if(n > 640)
				n = 640;
			if(game.mouse[i].x >= 0)
				game.mouse[i].x = (n + game.mouse[i].x)/2;
			else
				game.mouse[i].x = wd[i]->ir.x;
			n = wd[i]->ir.y;
			if(n < 0)
				n = 0;
			if(n > 480)
				n = 480;
			if(game.mouse[i].y >= 0)
				game.mouse[i].y = (wd[i]->ir.y + game.mouse[i].y)/2;
			else
				game.mouse[i].y = wd[i]->ir.y;
			game.mouse[i].rotDeg = wd[i]->orient.roll;
		}
		else{
			game.mouse[i].x = -1;
			game.mouse[i].y = -1;
		}*/// old method -the getBound* functions take care of all this
	}
}

void movePads(){
	int i;
	float m, n;
	for(i = 0; i < 4; i++){
		if(padOk(i)){
			if(game.pad[i].state < 0){
				game.pad[i].state = 0;
				game.pad[i].length = 0;
				game.pad[i].dlength = NORMPADLENGTH;
				game.pad[i].hold = 0;
				game.pad[i].shoot = 0;
				game.pad[i].x = 160*i + 80;
				game.pad[i].y = 450;
				game.pad[i].rotDeg = 0;
			}
			else{
				if(game.mouse[i].x >= 0 && game.mouse[i].y >= 0){
					game.pad[i].x = game.mouse[i].x;
					n = game.mouse[i].y;
					if(game.pow.flip)
						n = 480-n;
					n-=140;
					if(n < 0)
						n = 0;
					if(n > 200)
						n = 200;
					n = n/5 + 420;
					game.pad[i].y = n;
				}
				n = game.mouse[i].rotDeg;
				if(n < -90)
					n = -90;
				if(n > 90)
					n = 90;
				if(game.pow.flip)
					n*= -1;
				n /= 10;
				game.pad[i].rotDeg = (game.pad[i].rotDeg + n)/2;
				n = game.pad[i].x;
				m = game.pad[i].length;
				n = n*((640-m)/640.0);
				if(game.pow.nowall == 0){
					n = n*(600/640.0) + 20;
					m = m*(600/640.0);
				}
				game.pad[i].calcx = n;
				game.pad[i].calclen = m;
				game.pad[i].calcmid = n + m/2;
				game.pad[i].calcholdlen = m - m/16; //might not be correct!
			}
		}
		else
			game.pad[i].state = -1;
	}
}

//not sure if the constants used below are valid for every wii console...
float getBoundX(int chan){
	float res;
	float oxl = 231;
	float oxr = 791;
	//if(wd[chan]->ir.valid)
	//	return wd[chan]->ir.x;
	//else 
	if(wd[chan]->ir.smooth_valid){
		res = (wd[chan]->ir.sx - oxl) * (wd[chan]->ir.vres[0] / (oxr-oxl));
		if(res < 0)
			res = 0;
		if(res > wd[chan]->ir.vres[0])
			res = wd[chan]->ir.vres[0];
		return res;
	}
	return -1;
}

float getBoundY(int chan){
	float res;
	float oyt = 64;
	float oyb = 484;
	//if(wd[chan]->ir.valid)
	//	return wd[chan]->ir.y;
	//else 
	if(wd[chan]->ir.smooth_valid){
		res = (wd[chan]->ir.sy - oyt) * (wd[chan]->ir.vres[1] / (oyb-oyt));
		if(res < 0)
			res = 0;
		if(res > wd[chan]->ir.vres[1])
			res = wd[chan]->ir.vres[1];
		return res;
	}
	return -1;
}
