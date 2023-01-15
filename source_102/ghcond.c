/*===========================================
        WiiXBall v1.0.2
        by Kirill M	
							
		(c) KMS 2010-2011
        A DXBall Clone
============================================*/
#include "WiiXBall.h"


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
