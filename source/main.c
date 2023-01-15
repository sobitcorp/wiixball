/*===========================================
        WiiXBall v0.01
        by Kirill M	
							
		(c) KMS 2010
        A DXBall Clone
============================================*/
#include <grrlib.h>
#include <stdlib.h>
#include <wiiuse/wpad.h>
#include <ogc/lwp_watchdog.h> //FPS
#include <asndlib.h>
#include <oggplayer.h>
#include <gcmodplay.h>
#include "wpad_leds.h"

//graphics
#include "../gfx/font.h"
#include "../gfx/kfont.h"
#include "../gfx/fontgr.h"
#include "../gfx/kmslogo.h"
#include "../gfx/hcursor.h"
#include "../gfx/intbg1.h"
#include "../gfx/intbg2.h"
#include "../gfx/inttbg.h"
#include "../gfx/intlogo0.h"
#include "../gfx/intlogo1.h"
#include "../gfx/intlogo2.h"
#include "../gfx/intlogo3.h"
#include "../gfx/intlogo4.h"
#include "../gfx/intlogo5.h"
#include "../gfx/intlogo6.h"
#include "../gfx/intlogo7.h"
#include "../gfx/fscore.h"
#include "../gfx/gball.h"
#include "../gfx/gbolt.h"
#include "../gfx/gbullet.h"
#include "../gfx/gexplo.h"
#include "../gfx/gfire.h"
#include "../gfx/ghold.h"
#include "../gfx/glife.h"
#include "../gfx/gpad.h"
#include "../gfx/gpower.h"
#include "../gfx/gtile.h"
#include "../gfx/gwall.h"
#include "../gfx/hstar.h"
#include "../gfx/hwin.h"
#include "../gfx/hnewrec.h"

//sound
#include "../sfx/OGint1.h"
#include "../sfx/OGhigh.h"
#include "../sfx/OGwin.h"
#include "../sfx/MDend.h"
#include "../sfx/sBad.h"
#include "../sfx/sBolt.h"
#include "../sfx/sBoltwait.h"
#include "../sfx/sBonus.h"
#include "../sfx/sBr.h"
#include "../sfx/sBrbonus.h"
#include "../sfx/sExp1.h"
#include "../sfx/sExp2.h"
#include "../sfx/sExp3.h"
#include "../sfx/sFall.h"
#include "../sfx/sGameover.h"
#include "../sfx/sGun.h"
#include "../sfx/sHid.h"
#include "../sfx/sHitwall.h"
#include "../sfx/sHold.h"
#include "../sfx/sLife.h"
#include "../sfx/sLose.h"
#include "../sfx/sPad.h"
#include "../sfx/sPadex.h"
#include "../sfx/sPadfast.h"
#include "../sfx/sPadlow.h"
#include "../sfx/sPadsh.h"
#include "../sfx/sSb.h"
#include "../sfx/sSplit.h"
#include "../sfx/SUsb.h"
#include "../sfx/sPause.h"
#include "../sfx/sPauseoff.h"
#include "../sfx/wBonus.h"
#include "../sfx/wGun.h"
#include "../sfx/wPad.h"

//misc
#include "../msc/bDefault.h"
#include "../msc/hLogo.h"


#define MAXSPARK 1024
#define MAXBOLT 8
#define MAXEXPLO 32
#define MAXBULLET 6
#define MAXBALL 128
#define MAXPRIZE 8
#define MAXSTAR 128  //has to be smaller than MAXSPARK-2
#define MAXBIGSTAR 8 //has to be smaller than MAXPRIZE

#define GPD 16 //game processing detail (small GPD = more clipping errors)
#define NUMTILES 0x30
#define NUMPRIZES 27

#define NORMPADLENGTH 84 //64 is kinda hard... also was 96
#define NORMBALLSPEED 4.5 //5 is hard too
#define NORMBALLSIZE 10
#define BALLSPARKSPEED 9.5
#define PADGROWFACTOR 48

#define PF_EXPLO 8 //explosive tile prize frequency
#define PF_BALL 8  //ball hit tile prize freq
#define PF_BULLET 12

#define NUMBUTTONS 96
#define NUMFONTS 3

#define FONT_SYS 0
#define FONT_8BIT 1
#define FONT_GR 2

#define HSNAMELEN 20
#define HSRECORDS 16

#define OPTSVERSION 3

//GRRLIB_texImg *GFXFont;
//GRRLIB_texImg *GFXKfont;
GRRLIB_texImg *GFXLogo;
GRRLIB_texImg *GFXCursor;
GRRLIB_texImg *GFXIntbg1;
GRRLIB_texImg *GFXIntbg2;
GRRLIB_texImg *GFXInttbg;
GRRLIB_texImg *GFXIntlogo[8];
GRRLIB_texImg *GFXFscore;
GRRLIB_texImg *GFXGball;
GRRLIB_texImg *GFXGbolt;
GRRLIB_texImg *GFXGbullet;
GRRLIB_texImg *GFXGexplo;
GRRLIB_texImg *GFXGfire;
GRRLIB_texImg *GFXGhold;
GRRLIB_texImg *GFXGlife;
GRRLIB_texImg *GFXGpad;
GRRLIB_texImg *GFXGpower;
GRRLIB_texImg *GFXGtile;
GRRLIB_texImg *GFXGwall;
GRRLIB_texImg *GFXHstar;
GRRLIB_texImg *GFXHwin;
GRRLIB_texImg *GFXHnewrec;

GRRLIB_texImg *GBuf[2];

const unsigned char* sExp[3];
int sExp_size[3];

typedef struct s_kRecord{
	u8 level;
	char name[HSNAMELEN+1];
	int score;
} kRecord;

typedef struct s_kFont{
	GRRLIB_texImg *img;
	int sx;
	int sy;
} kFont;

typedef struct s_kButton{
	int x;
	int y;
	int w;
	int h;
	int border;
	float scale;
	float fontscale;
	s8 state;
	int font;
	char* text;
	u32 color[3];
	u32 bgcolor;
} kButton;

typedef struct s_kMouse{
	float x;
	float y;
	float rotDeg;
	int rumble;
	int dctr;
	u8 enable;/*
	float ax;
	float ay;
	float az;
	float axavg;
	float ayavg;
	float azavg;
	long axsum;
	long aysum;
	long azsum;
	int axct;
	int ayct;
	int azct;
	float axcal;
	float aycal;
	float azcal;*/
} kMouse;

typedef struct s_kSpark{
	s8 type;
	float x;
	float y;
	float vx;
	float vy;
	u32 color;
} kSpark;

typedef struct s_kBolt{
	s8 state;
	float tx;
	float ty;
	float rotDeg;
} kBolt;

typedef struct s_kExplo{
	s8 state;
	float x;
	float y;
	float size;
} kExplo;

typedef struct s_kBullet{
	s8 state;
	float x;
	float y;
	float vel;
	float dir;
} kBullet;

typedef struct s_kTile{
	s8 state;
	float x;
	float y;
	int w;
	int h;
	float vx;
	float vy;
	float rotDeg;
	float rotVel;
} kTile;

typedef struct s_kPaddle{
	s8 state;
	float x;
	float y;
	int length;
	int dlength;
	float rotDeg;
	char hold;
	char shoot;
	u32 color;
	kBullet bullet[MAXBULLET];
	float calcx;
	float calclen;
	float calcmid;
	float calcholdlen;
} kPaddle;

typedef struct s_kBall{
	s8 state;
	s8 held; //by which pad
	float holdpos;
	float x;
	float y;
	float vel;
	float dir;
	float ddir; //added to dir each prGame
	int size;
	int dsize;
	float rotDeg;
	float rotVel;
} kBall;

typedef struct s_kPrize{
	s8 state;
	float x;
	float y;
	float vx;
	float vy;
	float rotDeg;
	float rotVel;
} kPrize;

typedef struct s_kPowers{
	char noclip;
	char fire;
	char fall;
	char nowall;
	int unarmor;
	float rand;
	float pixelate;
	char flip;
	float magnet;
	int boltvoice;
	float boltfactor;
} kPowers;

typedef struct s_kOptions{
	int version;
	char drawMode;
	char rumble;
	char wsound;
} kOptions;

typedef struct s_kGame{
	s8 state;
	int level;
	int lives;
	int score;
	int dscore;
	unsigned int time;
	u32 pcolor[4];
	kMouse mouse[4];
	kFont font[NUMFONTS];
	kButton button[NUMBUTTONS];
	kSpark spark[MAXSPARK];
	kBolt bolt[MAXBOLT];
	kExplo explo[MAXEXPLO];
	kTile tile[20*20];
	kPaddle pad[4];
	kBall ball[MAXBALL];
	kPrize prize[MAXPRIZE];
	kPowers pow;
	kRecord high[HSRECORDS];
	kOptions opts;
	int boardcount;
	char* boardset;
} kGame;
	
extern GXRModeObj *rmode;
static MODPlay modplay;
kGame game;
int go, screen, sub, bpos;
WPADData *wd[4];
kRecord trec;
bool stopgame = false;
bool turnoff = false;
static const double deg2rad = 0.0174532925;
static const double rad2deg = 57.295779513;

void loadData();
void loadSets();
void makeOptsFile();
void saveOpts();
void initFont(int, const unsigned char*, int, int, char);
void makeButton(int, int, int, int, int, int, float, float, int, char*, u32, u32, u32, u32);
void drawButton(kButton);

void ctGlobal();
void scLogo();
void scIntro();
void ctIntro();
void scOpts();
void ctOpts();
void scCred();
void ctCred();

void scStart();
void scGame();
void prAnim();
void prGame();
void ctGame();
void scEnd();
void scPause();
void ctPause();

void scHigh();
void ctHigh();
void scWin();
void ctWin();

void scFX();
void showKeys();
void printLine(int*, int, char*, int, float, u32);

int genBall(s8, float, float, float, float, float, int);
int genBolt(int, int);
int genBullet(int);
int genExplo(float, float);
int genPrize(int, float, float);
void genSpark(s8, float, float, int, int, int, int, int);

void ballHitPad(int, int, float, double*, double*);
void ballHitTile(int, int);
void ballRelease(int, int);
void ballSizeDown();
void ballSizeUp();
void ballSpeedDown(); 
void ballSpeedUp();
void ballSplit();
void ballSpread();
void boltFire();
void boltPrize();
void levelWin();
void levelLose();
void tileExplode();
void tileFall();
void tilePlus();
void tileZap();
void padRelease(int);
void powstopgame();
void prizeHitPad(int, int);
void resetItems();

void checkBolt();
void checkLose();
void checkWin();
bool isPadHolding(int);
int getBreakableTilesLeft();
int getNearestTileFromPos(int, int);
int getNearestTile(int, int, u8*);

void initPads();
void updPads();
bool padOk(int);
int padDetect();
bool posValid(int);
int pointDetect();
void updMice();
void movePads();
float getBoundX(int);
float getBoundY(int);

int sndPlay(const unsigned char*,int, float, float, int);
int sndPlayP(const unsigned char*,int, float, float, int);
int sndPlay16(const unsigned char*,int, float, float, int);
void sndVol(int, float);
void sndStop(int);
void sndStopAll();
bool sndActive(int);
void wsndOn();
void wsndOff();
void wsndPlay(int, const unsigned char*, int);

u32 hsv2rgb(float, float, float, u8);
u32 blendColor(u32, u32);
u32 desaturate(u32);
int strabslen(char*);
static u8 CalculateFrameRate();
void escape(int);
void sysreset(void);
void syspower(void);

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
	game.font[FONT_SYS].img = 0;

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
			GRRLIB_Screen2Texture(GBuf[0]);
			GRRLIB_Screen2Texture(GBuf[1]);
		}
		
		
		
		//static int hi = 0;
		//if(GetTimeOgg() > 70200)
		//	hi = GetTimeOgg();
		/*
		int q;
		
		if(game.font[FONT_SYS].img != 0){
			GRRLIB_Printf(32,32,game.font[FONT_SYS].img,0xFFFFFFA0,1,"WiiXBall! Screen: %d Subscreen: %d FPS: %d umm:%d bpos:%d", screen, sub, FPS, GetTimeOgg(), bpos);//GetTimeOgg()
        
			//ir dump
			updMice();
			GRRLIB_Printf(32,52,game.font[FONT_SYS].img,0xFFFFFF80,1,"state:%d distance:%5f z:%5f angle:%5f wmangle:%5f", wd[0]->ir.state, wd[0]->ir.distance, wd[0]->ir.z, wd[0]->ir.angle, wd[0]->orient.roll);
			GRRLIB_Printf(32,52+17  ,game.font[FONT_SYS].img,0xFFFFFF80,1,"raw_valid:%d ax:%5f ay:%5f", wd[0]->ir.raw_valid, wd[0]->ir.ax, wd[0]->ir.ay);
			GRRLIB_Printf(32,52+17*2,game.font[FONT_SYS].img,0xFFFFFF80,1,"smt_valid:%d sx:%5f sy:%5f", wd[0]->ir.smooth_valid, wd[0]->ir.sx, wd[0]->ir.sy);
			GRRLIB_Printf(32,52+17*3,game.font[FONT_SYS].img,0xFFFFFF80,1,"    valid:%d  x:%5f  y:%5f", wd[0]->ir.valid, wd[0]->ir.x, wd[0]->ir.y);
			GRRLIB_Printf(32,52+17*4,game.font[FONT_SYS].img,0xFFFFFF80,1,"    valid:%d  x:%5f  y:%5f", wd[0]->ir.valid, getBoundX(0), getBoundY(0));
			GRRLIB_Printf(32,52+17*5,game.font[FONT_SYS].img,0xFFFFFF80,1,"vres:%d,%d offset:%d,%d", wd[0]->ir.vres[0], wd[0]->ir.vres[1], wd[0]->ir.offset[0], wd[0]->ir.offset[1]);
		
			//accel dump
			//GRRLIB_Printf(32,52,game.font[FONT_SYS].img,0xFFFFFFd0,1,"ax:%d ay:%d az:%d", wd[0]->accel.x, wd[0]->accel.y, wd[0]->accel.z);
			GRRLIB_Printf(32,52,game.font[FONT_SYS].img,0xFFFFFFd0,1,"ax:%5f ay:%5f az:%5f", game.mouse[0].ax, game.mouse[0].ay, game.mouse[0].az);
			GRRLIB_Printf(32,52+17,game.font[FONT_SYS].img,0xFFFFFFd0,1,"axavg:%5f ayavg:%5f azavg:%5f", game.mouse[0].axavg, game.mouse[0].ayavg, game.mouse[0].azavg);
			GRRLIB_Printf(32,52+17*2,game.font[FONT_SYS].img,0xFFFFFFd0,1,"axcal:%5f aycal:%5f azcal:%5f", game.mouse[0].axcal, game.mouse[0].aycal, game.mouse[0].azcal);
			//GRRLIB_Printf(32,52+17,game.font[FONT_SYS].img,0xFFFFFFd0,1,"gx:%5f gy:%5f gz:%5f", wd[0]->gforce.x, wd[0]->gforce.y, wd[0]->gforce.z);
			//GRRLIB_Printf(32,52+40,game.font[FONT_SYS].img,0xFFFFFFd0,1,"roll:%5f pitch:%5f yaw:%5f", wd[0]->orient.roll, wd[0]->orient.pitch, wd[0]->orient.yaw);
			
			if(wd[0]->btns_h & WPAD_BUTTON_A)
				q = 320;
			if(abs(game.mouse[0].ax) > 16)
				q += game.mouse[0].ax/100;
			GRRLIB_Rectangle(q, 240, 16, 16, 0xFFFFFFFF, true);
		}
		else
			q = 0;
		*/
		FPS = CalculateFrameRate();
		GRRLIB_Render();
    }
    GRRLIB_Exit(); // Be a good boy, clear the memory allocated by GRRLIB
	if(turnoff)
		SYS_ResetSystem(SYS_POWEROFF,0,0);
    exit(0);  // Use exit() to exit a program, do not use 'return' from main()
}


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
	
	initFont(FONT_SYS, font, 8, 16, 32);
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
	game.font[num].img = GRRLIB_LoadTexturePNG(img);
	GRRLIB_InitTileSet(game.font[num].img, sx, sy, startchar); 
	game.font[num].sx = sx;
	game.font[num].sy = sy;
}

void makeButton(int num, int x, int y, int w, int h, int border, float scale, float fontscale, int font, char* text, u32 color, u32 oncolor, u32 downcolor, u32 bgcolor){
	game.button[num].x = x;
	game.button[num].y = y;
	game.button[num].w = w;
	game.button[num].h = h;
	game.button[num].border = border;
	game.button[num].scale = scale;
	game.button[num].fontscale = fontscale;
	game.button[num].font = font;
	game.button[num].text = text;
	game.button[num].color[0] = color;
	game.button[num].color[1] = oncolor;
	game.button[num].color[2] = downcolor;
	game.button[num].bgcolor = bgcolor;
	game.button[num].state = -1;
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
	GRRLIB_Printf(sx + sw/2.0 - (strlen(b.text)*game.font[b.font].sx*b.scale*b.fontscale)/2.0, sy + sh/2.0 - (game.font[b.font].sy*b.scale*b.fontscale)/2.0, game.font[b.font].img, b.color[b.state], b.scale*b.fontscale, b.text);
}

/*
void makeButton(int num, int w, int h, kFont font, char* text){
	int tlen;
	tlen = strlen(text);
	GRRLIB_FillScreen(0xFF);
	GRRLIB_Rectangle(1,1,w-1,h-1,0x000033FF, true);
	GRRLIB_Rectangle(1,1,w-1,h-1,0xFFFFFFFF, false);
	GRRLIB_Rectangle(2,2,w-3,h-3,0xBBBBBBFF, false);
	GRRLIB_Line(1,h,w,h,0x666666FF);
	GRRLIB_Line(w,1,w,h,0x666666FF);
	GRRLIB_Printf(w/2-(font.sx*tlen)/2, h/2-font.sy/2, font.img,0xFFFFFFFF,1,text);
	game.button[num].img = GRRLIB_CreateEmptyTexture(w, h);
	GX_SetTexCopySrc(0, 0, w, h);
    GX_SetTexCopyDst(w, h, GX_TF_RGBA8, GX_FALSE);
    GX_CopyTex(game.button[num].img->data, GX_FALSE);
    GX_PixModeSync();
    GRRLIB_FlushTex(game.button[num].img);
	game.button[num].img->h = h;
	game.button[num].img->w = w;
	
}*/

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
		GRRLIB_DrawImg( 0, 0, GFXLogo, 0, 1, 1, GRRLIB_GetColor(255, 255, 255, sub) );
	else if(sub == 255){
		loadData();
		loadSets();
		GRRLIB_DrawImg( 0, 0, GFXLogo, 0, 1, 1, 0xFFFFFFFF);
	}
	else if(sub <= 256 + 128)
		GRRLIB_DrawImg( 0, 0, GFXLogo, 0, 1, 1, 0xFFFFFFFF);
	else
		GRRLIB_DrawImg( 0, 0, GFXLogo, 0, 1, 1, GRRLIB_GetColor(255, 255, 255, (512-sub) * 2) );
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
				//GRRLIB_Rectangle(game.button[i].x - 8, game.button[i].y - 8, game.button[i].img->w * game.button[i].scale + 16, game.button[i].img->h * game.button[i].scale + 16, 0xBF, true);
				//GRRLIB_DrawImg(game.button[i].x, game.button[i].y, game.button[i].img, 0, game.button[i].scale,game.button[i].scale,game.button[i].color[game.button[i].state-1]);
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
		GRRLIB_Printf(320-56*2.5, 20, game.font[FONT_8BIT].img, 0xAACCFFFF, 2.5, "Options");
		GRRLIB_Printf(320-112*1.2, 82, game.font[FONT_GR].img, 0x10FF10FF, 1.2, "Rendering mode");
		GRRLIB_Printf(320-112*1.2, 192, game.font[FONT_GR].img, 0x10FF10FF, 1.2, "Wiimote rumble");
		GRRLIB_Printf(320-112*1.2, 302, game.font[FONT_GR].img, 0x10FF10FF, 1.2, "Wiimote sound");
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
	/*old-
	if(GetTimeOgg() > 70218)
		SetTimeOgg(1713);*/
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
				GRRLIB_Rectangle(320+((game.spark[i].x + j) * (game.spark[i].vx-j)/64.0), 240+(game.spark[i].y * (game.spark[i].vx-j)/64.0), (game.spark[i].vx-j) / 64.0, (game.spark[i].vx-j) / 64.0, GRRLIB_GetColor(255,255,255,128-j*4), true);
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
		MODPlay_SetMOD(&modplay,MDend);
		MODPlay_Start(&modplay);
		MODPlay_SetVolume(&modplay, 64, 64);
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
		
			//genBall(0, 0, 64, 64, 1, deg2rad * 160, 30);
			//genBall(0, .25, 64, 64, 1, deg2rad * 130, 9);
			//genBall(0, .75, 64, 64, 1, deg2rad * 60, 0);
			//genBall(0, 1, 64, 64, 1, deg2rad * 10, 20);
			

		movePads();
		screen = 5;
		sub = -1;
		if(stopgame){
			screen = 6;
			sub = 0;
		}
	}
	else
		GRRLIB_FillScreen(GRRLIB_GetColor(0, 0, 0, 256-(sub)*4));
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
			GRRLIB_FillScreen(GRRLIB_GetColor(0,0,0,rand()%128+32));
			//GRRLIB_Printf(40,40+i*50,GFXFont,0xFFFFFFFF,2,"%d %d",i,game.bolt[i].state);
			GRRLIB_DrawImg(game.bolt[i].tx-80, game.bolt[i].ty-450, GFXGbolt, game.bolt[i].rotDeg, 1, 1, 0xFFFFFFFF);
		}
		
		
	//debug- draws the pad's baseline here. 
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
			game.tile[i].state = game.tile[i].state++;
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
	
	/*/explo -- done by scGame now
	for(i=0; i<MAXEXPLO; i++)
		if(game.explo[i].state >= 0){
			game.explo[i].state++;
			if(game.explo[i].state >= 20)
				game.explo[i].state = -1;
		}*/
	
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
			game.spark[i].color = GRRLIB_GetColor(r,g,b,a);
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
					
				/*
				//homing fail
				// old technique - long distance - not working correctly
				if(game.pow.magnet > 0 && game.ball[i].y < 400){
					j = getNearestTileFromPos(game.ball[i].x, game.ball[i].y);
					if(j >= 0 && game.tile[j].x >= 0 && game.tile[j].y >= 0){
						GRRLIB_Rectangle(game.tile[j].x + game.tile[j].w/2, game.tile[j].y + game.tile[j].h/2, 10, 10, 0xFFFFFFFF, true);
						m = (game.tile[j].x - game.ball[i].x + game.tile[j].w/2);
						n = -(game.tile[j].y - game.ball[i].y + game.tile[j].h/2);
						o = atan(n/m);
						if(m<0)
							o += deg2rad*180;
						
						m = cos(o);
						n = -sin(o);
						for(k=1; k<32; k++)
							GRRLIB_Rectangle(game.ball[i].x + m*k*2, game.ball[i].y + n*k*2, 4, 4, 0xFFFFFFFF, true);
						o = o-game.ball[i].dir;
						game.ball[i].dir += o*game.pow.magnet/GPD;
					}
				}
				if(game.pow.magnet > 0){ //new short-range technique - SUCKS
					j = (game.ball[i].x-20)/30;
					k = (game.ball[i].y-50)/15;
					if(k > 0 && game.tile[(k-1)*20+j].state > 0x0)
						n+= game.pow.magnet/GPD;
					else if(j > 0 && game.tile[k*20+j-1].state > 0x0)
						m-= game.pow.magnet/GPD;
					else if(j < 19 && game.tile[k*20+j+1].state > 0x0)
						m+= game.pow.magnet/GPD;
					else if(k < 19 && game.tile[(k+1)*20+j].state >0x0)
						n-= game.pow.magnet/GPD;
				}*/		
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
	
	
	//if(rand()%(64*GPD) == 0)
	//	genPrize(-1, rand()%640, rand()%480);
	//if(game.prize[0].state > 0)
	//	game.prize[0].state = 9;
	//genExplo(rand()%640, rand()%480);
	//game.pad[0].shoot = 1;
	//game.pad[0].hold = 1;
	//game.pow.nowall = 1;
	//genBall(0,rand()%90/90.0,rand()%640, rand()%480, 1,1,rand()%90);
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
		GRRLIB_FillScreen(GRRLIB_GetColor(0, 0, 0, sub*4));
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
		GRRLIB_FillScreen(GRRLIB_GetColor(0, 0, 0, (sub%64)*4));
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
		GRRLIB_Rectangle(0,0,640,480,GRRLIB_GetColor((sub%64)*4,(sub%64)*4,(sub%64)*4,255),true);
		GRRLIB_SetBlend(GRRLIB_BLEND_ALPHA);
	}
	else if(sub == 127 + 32){
		GRRLIB_BMFX_Grayscale(GBuf[0], GBuf[1]);
		GRRLIB_FillScreen(0xDDDDDDFF);
	}
	else if(sub < 127 + 64){
		GRRLIB_DrawImg(0,0,GBuf[1],0,1,1,0xFFFFFFFF);
		GRRLIB_SetBlend(GRRLIB_BLEND_INV);
		GRRLIB_Rectangle(0,0,640,480,GRRLIB_GetColor((64 - sub%64)*4,(64 - sub%64)*4,(64 - sub%64)*4,255),true);
		GRRLIB_SetBlend(GRRLIB_BLEND_ALPHA);	
	}
	else if(sub < 255)
		GRRLIB_DrawImg(0,0,GBuf[1],0,1,1,0xFFFFFFFF);
	else if(sub < 255 + 64){
		GRRLIB_DrawImg(0,0,GBuf[1],0,1,1,0xFFFFFFFF);
		GRRLIB_FillScreen(GRRLIB_GetColor(0, 0, 0, ((sub+1)%64)*4));
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
		GRRLIB_Printf(320-40*2.5, 80, game.font[FONT_8BIT].img, 0xAACCFFFF, 2.5, "Pause");
		GRRLIB_Printf(160, 160, game.font[FONT_GR].img, 0xFFAA77FF, 1.5, "Board #");
		GRRLIB_Printf(160, 160+48, game.font[FONT_GR].img, 0xFFAA77FF, 1.5, "Lives");
		GRRLIB_Printf(160, 160+48*2, game.font[FONT_GR].img, 0xFFAA77FF, 1.5, "Score");
		GRRLIB_Printf(480-24*(floor(log10(game.level))+1), 160, game.font[FONT_GR].img, 0xAAFF11FF, 1.5, "%d", game.level);
		i = log10(game.lives);
		if(i<0) i=0;
		GRRLIB_Printf(480-24*(i+1), 160+48, game.font[FONT_GR].img, 0xAAFF11FF, 1.5, "%d", game.lives);
		i = log10(game.score);
		if(i<0) i=0;
		GRRLIB_Printf(480-24*(i+1), 160+48*2, game.font[FONT_GR].img, 0xAAFF11FF, 1.5, "%d", game.score);
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
					GRRLIB_Rectangle(320+((game.spark[i].x - j) * (game.spark[i].vx-j)/64.0), 240+(game.spark[i].y * (game.spark[i].vx-j)/64.0), (game.spark[i].vx-j) / 64.0, (game.spark[i].vx-j) / 64.0, GRRLIB_GetColor(255,255,255,128-j*4), true);
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
				PlayOgg(mem_open((char*)OGhigh, OGhigh_size), 0, OGG_ONE_TIME);
				SetVolumeOgg(255);
			}
			else{
				MODPlay_SetMOD(&modplay,MDend);
				MODPlay_Start(&modplay);
				MODPlay_SetVolume(&modplay, 64, 64);
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
			GRRLIB_Printf(320-strabslen(trec.name)*12, 154, game.font[FONT_GR].img, 0xAAAAFFFF, 1.5, trec.name);
		else
			GRRLIB_Printf(320-22*12, 154, game.font[FONT_GR].img, 0x555555FF, 1.5, "Please enter your name");
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
		GRRLIB_Printf(320-176, 200, game.font[FONT_GR].img, 0xCCCC11FF, 2, "Your score:");
		GRRLIB_Printf(320-30*(i+1), 280, GFXFscore, 0xFFFFFFFF, 2, "%d", game.score);
		if(sub < 400)
			GRRLIB_FillScreen(255-((sub++)-336)*4);
		else if(sub > 400)
			GRRLIB_FillScreen(((sub++)-400)*4);
	}
	else if(sub < 592){
		GRRLIB_Printf(40, 120, game.font[FONT_GR].img, 0x666655FF, .7, "Name");
		GRRLIB_Printf(472-11.2*5, 120, game.font[FONT_GR].img, 0x666655FF, .7, "Level");
		GRRLIB_Printf(600-11.2*5, 120, game.font[FONT_GR].img, 0x666655FF, .7, "Score");
		for(i=0; i<HSRECORDS; i++){
			col = blendColor(0x88AAFFFF, 0xFFFFFF00 + (1 - i/(HSRECORDS * 2.0))*255);
			if(game.high[i].level > game.boardcount)
				col = 0xEEEE11FF;
			if(i == trec.score)
				col = desaturate(hsv2rgb(game.spark[MAXSPARK-1].x, 1, 1, 0xFF));
			GRRLIB_Printf(40, 136+i*20, game.font[FONT_GR].img, col, 1, game.high[i].name);
			if(game.high[i].level > game.boardcount)
				GRRLIB_Printf(472 - 16*3, 136+i*20, game.font[FONT_GR].img, col, 1, "WON");
			else
				GRRLIB_Printf(472 - 16*(floor(log10(game.high[i].level))+1), 136+i*20, game.font[FONT_GR].img, col, 1, "%d",game.high[i].level);
			GRRLIB_Printf(600 - 16*(floor(log10(game.high[i].score))+1), 136+i*20, game.font[FONT_GR].img, col, 1, "%d",game.high[i].score);
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
		PlayOgg(mem_open((char*)OGwin, OGwin_size), 0, OGG_ONE_TIME);
		SetVolumeOgg(255);
		GRRLIB_FillScreen(0xFF);
	}
	else if(sub < 350){
		GRRLIB_FillScreen(GRRLIB_GetColor(0,0,(350-sub)/16.0 + 50, 255));
		for(i=0; i<MAXSTAR; i++){
			game.spark[i].y += game.spark[i].vx / 32.0;
			if(game.spark[i].y > 480){
				game.spark[i].y = 0;
				game.spark[i].x = rand()%640;
			}
			GRRLIB_Rectangle(game.spark[i].x, game.spark[i].y, game.spark[i].vx / 64.0, game.spark[i].vx / 64.0, GRRLIB_GetColor(255,255,255,game.spark[i].vx/4.0 + 128), true); //was /128.0 ?	
		}
		if(sub < 32)
			GRRLIB_FillScreen((32-sub)*8);
	}
	else if(sub < 400){
		GRRLIB_FillScreen(GRRLIB_GetColor(0,0,400-sub, 255));
		for(i=0; i<MAXSTAR; i++){
			game.spark[i].y += (game.spark[i].vx / 32.0) * ((sub-330)/20.0);
			if(game.spark[i].y > 480){
				game.spark[i].y = 0;
				game.spark[i].x = rand()%640;
			}
			for(j=1; j<sub-360; j++)
				GRRLIB_Rectangle(game.spark[i].x, game.spark[i].y - (game.spark[i].vx / 64.0)*j, game.spark[i].vx / 64.0, game.spark[i].vx / 64.0, GRRLIB_GetColor(255,255,255,game.spark[i].vx/128.0 + 72 - j*2), true);		
			GRRLIB_Rectangle(game.spark[i].x, game.spark[i].y, game.spark[i].vx / 64.0, game.spark[i].vx / 64.0, GRRLIB_GetColor(255,255,255,game.spark[i].vx/128.0 + 128), true);		
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
		GRRLIB_Screen2Texture(GBuf[(sub+1)%2]);
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
			GRRLIB_FillScreen(GRRLIB_GetColor(255,255,255,(sub-484)*16));
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
			GRRLIB_FillScreen(GRRLIB_GetColor(255,255,255,(516-sub)*16));
		else if(sub == 888){
			PlayOgg(mem_open((char*)OGhigh, OGhigh_size), 0, OGG_ONE_TIME);
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



//VIDEO EFFECTS & HELPER FUNCTIONS-----------------------------------------------------------------------------------------------

void scFX(){
	int curBuf = 0;
	GRRLIB_Screen2Texture(GBuf[curBuf]);
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

void showKeys(int from, int to){
	int i;
	for(i=8; i<86; i++)
		game.button[i].state = -1;
	for(i=from; i<to; i++)
		game.button[i].state = 0;
}

void printLine(int* y, int font, char* text, int yspace, float zoom, u32 color){
	GRRLIB_Printf(320-game.font[font].sx*strlen(text)*zoom/2.0, *y, game.font[font].img, color, zoom, "%s", text);
	*y += game.font[font].sy*zoom + yspace;
}

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


//CHECKING AND HELPER FUNCTIONS-----------------------------------------------------------------------------------

void checkBolt(){
	float i = getBreakableTilesLeft();
	if(i <= 5){
		bpos++;
		if(bpos < 386)
			return;
		i = 6-i;
		i = (i*i)/8192.0;
		if(game.pow.boltfactor < 1.1)
			game.pow.boltfactor += i;
		if(game.pow.boltvoice == -1 || !sndActive(game.pow.boltvoice))
			game.pow.boltvoice = sndPlay(sBoltwait, sBoltwait_size, game.pow.boltfactor/3, .5, 4000);
		else
			sndVol(game.pow.boltvoice, game.pow.boltfactor);
		if(game.pow.boltfactor > 1 && rand()%64 == 0)
			boltFire();
	}
}

void checkLose(){
	int i;
	for(i=0; i<MAXBALL; i++)
		if(game.ball[i].state >= 0)
			return;
	levelLose();
}

void checkWin(){
	if(getBreakableTilesLeft() == 0)
		levelWin();
}

bool isPadHolding(int n){
	int i;
	for(i=0; i<MAXBALL; i++)
		if(game.ball[i].state >= 0 && game.ball[i].held == n)
			return true;
	return false;
}

int getBreakableTilesLeft(){
	int i, ct=0;
	for(i=0; i<20*20; i++)
		if(game.tile[i].state > 0x0 && game.tile[i].state < 0x2f && game.tile[i].state != 0x2)
			if(ct < 1 || !(game.tile[i].state == 0x17 || game.tile[i].state == 0x18))
				ct++;
	return ct;
}

int getNearestTileFromPos(int x, int y){
	int i;
	u8 tilemap[20*20];
	for(i = 0; i<20*20; i++)
		tilemap[i] = 0;
	return getNearestTile((x-20)/30, (y-50)/15, tilemap);
}

int getNearestTile(int i, int j, u8* tilemap){ //not the best way to do this
	if(i < 0)
		return getNearestTile(0, j, tilemap);
	else if(i >= 20)
		return getNearestTile(19, j, tilemap);
	else if(j < 0)
		return getNearestTile(i, 0, tilemap);
	else if(j >= 20)
		return getNearestTile(i, 19, tilemap);
	else{
		tilemap[j*20+i] = 1;
		if((game.tile[j*20+i].state > 0x0 && game.tile[j*20+i].state <= 0x16 && game.tile[j*20+i].state != 0x2)||(game.tile[j*20+i].state >= 0x20 && game.tile[j*20+i].state < 0x28))
			return j*20+i;
		else if(j > 0 && tilemap[(j-1)*20+i] == 0)
			return getNearestTile(i, j-1, tilemap);
		else if(i > 0 && tilemap[j*20+i-1] == 0)
			return getNearestTile(i-1, j, tilemap);
		else if(j < 19 && tilemap[(j+1)*20+i] == 0)
			return getNearestTile(i, j+1, tilemap);
		else if(i < 19 && tilemap[j*20+i+1] == 0)
			return getNearestTile(i+1, j, tilemap);
	}
	return -1;
}



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
		game.mouse[i].y = -1;/*
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
		GRRLIB_Printf(40,160,game.font[FONT_SYS].img,0xCCCCFFFF,2,"Please connect at least one WiiMote");
		GRRLIB_Printf(150,200,game.font[FONT_SYS].img,0xCCCCFFFF,2,"to your Wii console!");
		GRRLIB_Printf(320-240,300,game.font[FONT_SYS].img,0xDDDDFFBB,1.5,"You can drop out of the game at any time");
		GRRLIB_Printf(320-252,328,game.font[FONT_SYS].img,0xDDDDFFBB,1.5,"by holding the minus button for 3 seconds.");
		GRRLIB_Printf(320-264,380,game.font[FONT_SYS].img,0xDDDDFFBB,1.5,"To rejoin, press any button on your WiiMote.");
		//GRRLIB_Printf(420,420,game.font[FONT_SYS].img,0xDDDDFFFF,1,"Wiimote data code: %d",wd[0]->data_present);
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
		GRRLIB_Printf(320-8*25, 150+24, game.font[FONT_SYS].img, 0xFFFFAADD, 2, "Please point your WiiMote");
		GRRLIB_Printf(320-8*14, 150+24+40, game.font[FONT_SYS].img, 0xFFFFAADD, 2, "at the screen.");
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
			game.mouse[i].rotDeg = wd[i]->orient.roll;/*
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
			game.mouse[i].azcal = game.mouse[i].azsum/game.mouse[i].azct;
			//game.mouse[i].axavg = (game.mouse[i].axavg + wd[i]->accel.x)/2.0;
			//game.mouse[i].ayavg = (game.mouse[i].ayavg + wd[i]->accel.y)/2.0;
			//game.mouse[i].azavg = (game.mouse[i].azavg + wd[i]->accel.z)/2.0;*/
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
	if(wd[chan]->ir.valid)
		return wd[chan]->ir.x;
	else if(wd[chan]->ir.smooth_valid){
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
	if(wd[chan]->ir.valid)
		return wd[chan]->ir.y;
	else if(wd[chan]->ir.smooth_valid){
		res = (wd[chan]->ir.sy - oyt) * (wd[chan]->ir.vres[1] / (oyb-oyt));
		if(res < 0)
			res = 0;
		if(res > wd[chan]->ir.vres[1])
			res = wd[chan]->ir.vres[1];
		return res;
	}
	return -1;
}


//SOUND FUNCTIONS-------------------------------------------------------------------------------------------

int sndPlay(const unsigned char* snd, int len, float vol, float pan, int pitchvar){
	int voice = ASND_GetFirstUnusedVoice();
	if(voice < 0)
		return -1;
	ASND_SetVoice(voice, VOICE_MONO_8BIT, 44100 + (rand()%pitchvar - pitchvar/2), 0, (u8 *)snd, len, vol * (1-pan) * 256, vol * pan * 256, NULL);
	//  _SetVoice(s32 voice, s32 format, s32 pitch,s32 delay, void *snd, s32 size_snd, s32 volume_l, s32 volume_r, void (*callback) (s32 voice)); 
	return voice;
}

int sndPlayP(const unsigned char* snd, int len, float vol, float pan, int pitchdiff){
	int voice = ASND_GetFirstUnusedVoice();
	if(voice < 0)
		return -1;
	ASND_SetVoice(voice, VOICE_MONO_8BIT, 44100 + pitchdiff, 0, (u8 *)snd, len, vol * (1-pan) * 256, vol * pan * 256, NULL);
	return voice;
}

int sndPlay16(const unsigned char* snd, int len, float vol, float pan, int pitchdiff){
	int voice = ASND_GetFirstUnusedVoice();
	if(voice < 0)
		return -1;
	ASND_SetVoice(voice, VOICE_MONO_16BIT, 44100 + pitchdiff, 0, (u8 *)snd, len, vol * (1-pan) * 256, vol * pan * 256, NULL);
	return voice;
}

void sndVol(int voice, float vol){
	ASND_ChangeVolumeVoice(voice, vol * 128, vol * 128);
}

void sndStop(int voice){
	ASND_StopVoice(voice);
}

void sndStopAll(){ //max might not be 16
	int i;
	for(i=0; i<16; i++)
		ASND_StopVoice(i);
}

bool sndActive(int voice){
	int i;
	i = ASND_StatusVoice(voice);
	if(i == SND_WORKING)
		return true;
	return false;
} 
\
//wiimote sound - still don't know which format its supposed to use-sounds like crap right now
void wsndOn(){
	int i;
	for(i=0; i<4; i++)
		WPAD_ControlSpeaker(i, 1);
}

void wsndOff(){
	int i;
	for(i=0; i<4; i++)
		WPAD_ControlSpeaker(i, 0);
}
void wsndPlay(int chan, const unsigned char* data, int len){
	WPAD_SendStreamData(chan, (void*)data, len);
}

//MISC FUNCTIONS-------------------------------------------------------------------------------------------

u32 hsv2rgb(float h, float s, float v, u8 a){
	// H is given on [0, 6] or -1. S and V are given on [0, 1].
	float m, n, f;
	int i;
	if (h == -1)
		return GRRLIB_GetColor(v*256, v*256, v*256, a);
	i = floor(h);
	f = h - i;
	if ( !(i&1) )
		f = 1 - f; // if i is even
	m = v * (1 - s);
	n = v * (1 - s * f);
	switch (i) {
		case 6:
		case 0: return GRRLIB_GetColor(v*256, n*256, m*256, a);
		case 1: return GRRLIB_GetColor(n*256, v*256, m*256, a);
		case 2: return GRRLIB_GetColor(m*256, v*256, n*256, a);
		case 3: return GRRLIB_GetColor(m*256, n*256, v*256, a);
		case 4: return GRRLIB_GetColor(n*256, m*256, v*256, a);
		case 5: return GRRLIB_GetColor(v*256, m*256, n*256, a);
	}
	return 0;
}

u32 blendColor(u32 col, u32 mask){
	u8 r, g, b, a;
	r = R(col) * (R(mask)/255.0);
	g = G(col) * (G(mask)/255.0);
	b = B(col) * (B(mask)/255.0);
	a = A(col) * (A(mask)/255.0);
	return GRRLIB_GetColor(r,g,b,a);
}

u32 desaturate(u32 col){
	u8 r, g, b, a;
	//u8 avg = (R(col)+G(col)+B(col))/3;
	u8 avg = 255;
	r = (R(col) + avg) / 2;
	g = (G(col) + avg) / 2;
	b = (B(col) + avg) / 2;
	a = A(col);
	return GRRLIB_GetColor(r,g,b,a);
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
static u8 CalculateFrameRate() {
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