/*===========================================
        WiiXBall v1.0.2
        by Kirill M	
							
		(c) KMS 2010-2011
        A DXBall Clone
============================================*/
#include "WiiXBall.h"


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
