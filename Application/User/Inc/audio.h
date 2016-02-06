/**
 *  Project     WeldingMeter
 *  @file		audio.h
 *  @author		Gerd Bartelt - www.sebulli.com
 *  @brief		header file for audio.c
 *
 *  @copyright	GPL3
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
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
