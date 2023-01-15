/*===========================================
        WiiXBall v1.0.2
        by Kirill M	
							
		(c) KMS 2010-2011
        A DXBall Clone
============================================*/
#ifndef _WIIXBALL_H_
#define _WIIXBALL_H_

#include <grrlib.h>
#include <stdlib.h>
#include <wiiuse/wpad.h>
#include <ogc/lwp_watchdog.h> //FPS
#include <asndlib.h>
#include <oggplayer.h>
#include <gcmodplay.h>
#include "wpad_leds.h"

//graphics
#include "../gfx/sysfont.h"
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
//#include "../sfx/OGhigh.h"
//#include "../sfx/OGwin.h"
//#include "../sfx/MDend.h"
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
	int bfont;
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
	u8 enable;
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
	float azcal;
	float vx;
	float vy;
	float vz;
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
	kFont gfont[NUMFONTS];
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
bool stopgame;
bool turnoff;
static const double deg2rad = 0.0174532925;
static const double rad2deg = 57.295779513;
lwp_t tcpthread;
lwpq_t tcpthreadq;


//main.c
void escape(int);
void sysreset(void);
void syspower(void);

//inits.c
void loadData();
void loadSets();
void makeOptsFile();
void saveOpts();
void initFont(int, const unsigned char*, int, int, char);
void makeButton(int, int, int, int, int, int, float, float, int, char*, u32, u32, u32, u32);

//scmenus.c
void ctGlobal();
void scLogo();
void scIntro();
void ctIntro();
void scOpts();
void ctOpts();

//scgame.c
void scStart();
void scGame();
void prAnim();
void prGame();
void ctGame();
void scEnd();
void scPause();
void ctPause();

//scend.c
void scHigh();
void ctHigh();
void scWin();
void ctWin();
void scCred();
void ctCred();

//scmisc.c
void scFX();
void drawButton(kButton);
void showKeys();
void printLine(int*, int, char*, int, float, u32);

//ghgen.c
int genBall(s8, float, float, float, float, float, int);
int genBolt(int, int);
int genBullet(int);
int genExplo(float, float);
int genPrize(int, float, float);
void genSpark(s8, float, float, int, int, int, int, int);

//ghevent.c
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

//ghcond.c
void checkBolt();
void checkLose();
void checkWin();
bool isPadHolding(int);
int getBreakableTilesLeft();
int getNearestTileFromPos(int, int);
int getNearestTile(int, int, u8*);

//control.c
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

//sound.c
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

//network.c
int tcp_init();
void tcp_thread();

//misc.c
u32 hsv2rgb(float, float, float, u8);
u32 blendColor(u32, u32);
u32 desaturate(u32);
int strabslen(char*);
u8 CalculateFrameRate();


#endif