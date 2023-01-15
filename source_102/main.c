/*===========================================
        WiiXBall v1.0.2
        by Kirill M
								
		(c) KMS 2010-2011
        A DXBall Clone
============================================*/
#include "WiiXBall.h"

int main() {
	int i;
	
    // Initialise the Graphics & Video subsystem
    GRRLIB_Init();
	ASND_Init(NULL);
	MODPlay_Init(&modplay);
    initPads();
	SYS_SetResetCallback(sysreset);
	SYS_SetPowerCallback(syspower);
	
	// Load Textures
	GFXLogo = GRRLIB_LoadTextureJPG(kmslogo);
	game.gfont[FONT_SYS].img = 0;
	
	
	tcp_init();
	
	

	stopgame = false;
	turnoff = false;
	u8 FPS = 0;
	screen = 0;
	sub = 0;
	bpos = 0;
	go = 1;
    while(go) {

		updPads();
		
		if(screen == 0)
			scLogo();
		else if(screen == 1){
			scIntro();
			ctIntro();
		}
		else if(screen == 2){
			scOpts();
			ctOpts();
		}
		else if(screen == 3){
			scCred();
			ctCred();
		}
		else if(screen == 4){
			if(sub > 0)
				scGame();
			scStart();  //is the sub incrementor
			if(sub%4 == 0)
				prAnim();
		}
		else if(screen == 5){
			sub++; //cause noone else wants to do it
			if(game.state == 1)
				for(i=0; i<GPD; i++)
					prGame();
			scGame();
			ctGame();
			if(sub%4 == 0)
				prAnim();
		}
		else if(screen == 6){
			if(sub%4 == 0)
				prAnim(); //not visible but cleans up nicely
			scEnd();    //is the sub incrementor
		}
		else if(screen == 7){
			scPause();
			ctPause();
		}
		else if(screen == 8){
			scHigh();
			ctHigh();
		}
		else if(screen == 9){
			scWin();
			ctWin();
		}
		ctGlobal();
		
		if((screen == 6 && (sub==0 || sub==64 || sub == 128)) || (screen == 7 && sub==0)){
			GRRLIB_Screen2Texture(0,0,GBuf[0],false);
			GRRLIB_Screen2Texture(0,0,GBuf[1],false);
		}
		
		
		
		int q;
		
		if(game.gfont[FONT_SYS].img != 0){
			GRRLIB_Printf(32,32,game.gfont[FONT_SYS].img,0xFFFFFFA0,1,"WiiXBall! Screen: %d Subscreen: %d FPS: %d umm:%d bpos:%d", screen, sub, FPS, GetTimeOgg(), bpos);//GetTimeOgg()
        /*
			//ir dump
			updMice();
			GRRLIB_Printf(32,52,game.gfont[FONT_SYS].img,0xFFFFFF80,1,"state:%d distance:%5f z:%5f angle:%5f wmangle:%5f", wd[0]->ir.state, wd[0]->ir.distance, wd[0]->ir.z, wd[0]->ir.angle, wd[0]->orient.roll);
			GRRLIB_Printf(32,52+17  ,game.gfont[FONT_SYS].img,0xFFFFFF80,1,"raw_valid:%d ax:%5f ay:%5f", wd[0]->ir.raw_valid, wd[0]->ir.ax, wd[0]->ir.ay);
			GRRLIB_Printf(32,52+17*2,game.gfont[FONT_SYS].img,0xFFFFFF80,1,"smt_valid:%d sx:%5f sy:%5f", wd[0]->ir.smooth_valid, wd[0]->ir.sx, wd[0]->ir.sy);
			GRRLIB_Printf(32,52+17*3,game.gfont[FONT_SYS].img,0xFFFFFF80,1,"    valid:%d  x:%5f  y:%5f", wd[0]->ir.valid, wd[0]->ir.x, wd[0]->ir.y);
			GRRLIB_Printf(32,52+17*4,game.gfont[FONT_SYS].img,0xFFFFFF80,1,"    valid:%d  x:%5f  y:%5f", wd[0]->ir.valid, getBoundX(0), getBoundY(0));
			GRRLIB_Printf(32,52+17*5,game.gfont[FONT_SYS].img,0xFFFFFF80,1,"vres:%d,%d offset:%d,%d", wd[0]->ir.vres[0], wd[0]->ir.vres[1], wd[0]->ir.offset[0], wd[0]->ir.offset[1]);
		*/
			//accel dump
			//GRRLIB_Printf(32,52,game.gfont[FONT_SYS].img,0xFFFFFFd0,1,"ax:%d ay:%d az:%d", wd[0]->accel.x, wd[0]->accel.y, wd[0]->accel.z);
			//GRRLIB_Printf(32,52,game.gfont[FONT_SYS].img,0xFFFFFFd0,1,"ax:%5f ay:%5f az:%5f", game.mouse[0].ax, game.mouse[0].ay, game.mouse[0].az);
			//GRRLIB_Printf(32,52+17,game.gfont[FONT_SYS].img,0xFFFFFFd0,1,"axavg:%5f ayavg:%5f azavg:%5f", game.mouse[0].axavg, game.mouse[0].ayavg, game.mouse[0].azavg);
			//GRRLIB_Printf(32,52+27*2,game.gfont[FONT_SYS].img,0xFFFFFFd0,1,"vx:%5f vy:%5f vz:%5f", game.mouse[0].vx, game.mouse[0].vy, game.mouse[0].vz);
			//GRRLIB_Printf(32,52+17*2,game.gfont[FONT_SYS].img,0xFFFFFFd0,1,"axcal:%5f aycal:%5f azcal:%5f", game.mouse[0].axcal, game.mouse[0].aycal, game.mouse[0].azcal);
			//GRRLIB_Printf(32,52+17,game.gfont[FONT_SYS].img,0xFFFFFFd0,1,"gx:%5f gy:%5f gz:%5f", wd[0]->gforce.x, wd[0]->gforce.y, wd[0]->gforce.z);
			//GRRLIB_Printf(32,52+40,game.gfont[FONT_SYS].img,0xFFFFFFd0,1,"roll:%5f pitch:%5f yaw:%5f", wd[0]->orient.roll, wd[0]->orient.pitch, wd[0]->orient.yaw);
		/*
			if(wd[0]->btns_h & WPAD_BUTTON_A)
				q = 320;
			if(abs(game.mouse[0].ax) > 16)
				q += game.mouse[0].ax/100;
			GRRLIB_Rectangle(q, 240, 16, 16, 0xFFFFFFFF, true);*/
		}
		else
			q = 0;
			
		
		if(screen == 1 && bpos < 1)
			LWP_ThreadSignal(tcpthreadq);
		
		FPS = CalculateFrameRate();
		GRRLIB_Render();
    }
    GRRLIB_Exit(); // Be a good boy, clear the memory allocated by GRRLIB
	if(turnoff)
		SYS_ResetSystem(SYS_POWEROFF,0,0);
    exit(0);  // Use exit() to exit a program, do not use 'return' from main()
}


//0-home, 1-reset, 2-power
void escape(int button){
	if(button == 1 || button == 2)
		stopgame = true;
	if(button == 2)
		turnoff = true;
	if(screen == 0){
		if(sub < 250)
			sub = 250;
		else if(sub > 255)
			sub = 512;
	}
	else if(screen == 1)
		sub = 64 * 3 + 1;
	else if(screen == 2){
		if(sub == 1)
			sub = 999;
	}
	else if(screen == 3)
		sub = 129;
	else if(screen == 4 || screen == 6){
		if(screen == 4 && sub == 64){
			screen = 6;
			sub = 0;
		}
	}
	else if(screen == 5){
		screen = 7;
		sub = 0;
		if(stopgame)
			screen = 6;
	}
	else if(screen == 7){
		if(sub == 32)
			sub++;
	}
	else if(screen == 8){
		if(sub == 256 || sub == 400 || sub == 528)
			sub++;
	}
	else if(screen == 9){
		if(stopgame)
			sub = 1000;
		else if(sub == 999)
			sub++;
	}
	else
		go = 0;
}

void sysreset(void){
	escape(1);
	SYS_SetResetCallback(sysreset);
}

void syspower(void){
	escape(2);
	SYS_SetPowerCallback(syspower);
}