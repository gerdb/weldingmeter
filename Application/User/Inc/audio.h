#ifndef __audio_H
#define __audio_H

/* includes  ---------------------------------------------------------*/
#include <stdint.h>

#define AUDIO_OUT_BUFFER_SIZE	8192
#define AUDIO_STACK_SIZE		10
#define MAXVOLUME				5

typedef struct {
	const uint8_t* wav_data;
	int wav_length;
}AUDIO_FifoDef;



typedef enum {
  SOUND_PING,
  SOUND_SPARK,
  SOUND_BEEP,
}enSOUND;

/* Function prototypes -------------------------------------------------*/
void AUDIO_Init(void);
void AUDIO_Task(void);
void AUDIO_Read(uint16_t* buff, int size);
void AUDIO_PlayWav(const uint8_t* wav_data, int wav_length);
void AUDIO_PlaySound(enSOUND sound);
void AUDIO_SetVolume(int vol);

#endif // __audio_H
