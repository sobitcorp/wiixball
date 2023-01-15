/*===========================================
        WiiXBall v1.0.2
        by Kirill M	
							
		(c) KMS 2010-2011
        A DXBall Clone
============================================*/
#include "WiiXBall.h"


void tcp_thread(){
	while(1){
	wsndOn();
	wsndPlay(0, wBonus, wBonus_size);
	sndPlay(sBolt, sBolt_size, 1, .5, 1);
	LWP_ThreadSleep(tcpthreadq);
	}

}

int tcp_init(){
	
	if(LWP_InitQueue(&tcpthreadq) < 0){
	
	}
	
	return LWP_CreateThread(&tcpthread, (void*) tcp_thread, NULL, NULL, 0, 127);
}
