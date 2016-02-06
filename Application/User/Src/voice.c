/**
 *  Project     WeldingMeter
 *  @file		voice.c
 *  @author		Gerd Bartelt - www.sebulli.com
 *  @brief		Voice output in german and danish language
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


/* Includes ------------------------------------------------------------------*/
#include "voice.h"
#include "audio.h"
#include "setup.h"

static void VOICE_SayNumber(int number);
static void VOICE_SayAnd(void);


#include "voice_data_de.h"
#include "voice_data_da.h"



static void VOICE_SayNumber(int number) {
	if (SETUP_GetLanguage() == SETUP_DE) {
		switch (number) {
		case 1: AUDIO_PlayWav(VOICE_1_DE_DATA, sizeof(VOICE_1_DE_DATA)); break;
		case 2: AUDIO_PlayWav(VOICE_2_DE_DATA, sizeof(VOICE_2_DE_DATA)); break;
		case 3: AUDIO_PlayWav(VOICE_3_DE_DATA, sizeof(VOICE_2_DE_DATA)); break;
		case 4: AUDIO_PlayWav(VOICE_4_DE_DATA, sizeof(VOICE_4_DE_DATA)); break;
		case 5: AUDIO_PlayWav(VOICE_5_DE_DATA, sizeof(VOICE_5_DE_DATA)); break;
		case 6: AUDIO_PlayWav(VOICE_6_DE_DATA, sizeof(VOICE_6_DE_DATA)); break;
		case 7: AUDIO_PlayWav(VOICE_7_DE_DATA, sizeof(VOICE_7_DE_DATA)); break;
		case 8: AUDIO_PlayWav(VOICE_8_DE_DATA, sizeof(VOICE_8_DE_DATA)); break;
		case 9: AUDIO_PlayWav(VOICE_9_DE_DATA, sizeof(VOICE_9_DE_DATA)); break;
		case 10: AUDIO_PlayWav(VOICE_10_DE_DATA, sizeof(VOICE_10_DE_DATA)); break;
		case 11: AUDIO_PlayWav(VOICE_11_DE_DATA, sizeof(VOICE_11_DE_DATA)); break;
		case 12: AUDIO_PlayWav(VOICE_12_DE_DATA, sizeof(VOICE_12_DE_DATA)); break;
		case 13: AUDIO_PlayWav(VOICE_13_DE_DATA, sizeof(VOICE_13_DE_DATA)); break;
		case 14: AUDIO_PlayWav(VOICE_14_DE_DATA, sizeof(VOICE_14_DE_DATA)); break;
		case 15: AUDIO_PlayWav(VOICE_15_DE_DATA, sizeof(VOICE_15_DE_DATA)); break;
		case 16: AUDIO_PlayWav(VOICE_16_DE_DATA, sizeof(VOICE_16_DE_DATA)); break;
		case 17: AUDIO_PlayWav(VOICE_17_DE_DATA, sizeof(VOICE_17_DE_DATA)); break;
		case 18: AUDIO_PlayWav(VOICE_18_DE_DATA, sizeof(VOICE_18_DE_DATA)); break;
		case 19: AUDIO_PlayWav(VOICE_19_DE_DATA, sizeof(VOICE_19_DE_DATA)); break;
		case 20: AUDIO_PlayWav(VOICE_20_DE_DATA, sizeof(VOICE_20_DE_DATA)); break;
		case 30: AUDIO_PlayWav(VOICE_30_DE_DATA, sizeof(VOICE_30_DE_DATA)); break;
		case 40: AUDIO_PlayWav(VOICE_40_DE_DATA, sizeof(VOICE_40_DE_DATA)); break;
		case 50: AUDIO_PlayWav(VOICE_50_DE_DATA, sizeof(VOICE_50_DE_DATA)); break;
		case 60: AUDIO_PlayWav(VOICE_60_DE_DATA, sizeof(VOICE_60_DE_DATA)); break;
		case 70: AUDIO_PlayWav(VOICE_70_DE_DATA, sizeof(VOICE_70_DE_DATA)); break;
		case 80: AUDIO_PlayWav(VOICE_80_DE_DATA, sizeof(VOICE_80_DE_DATA)); break;
		case 90: AUDIO_PlayWav(VOICE_90_DE_DATA, sizeof(VOICE_90_DE_DATA)); break;
		case 100: AUDIO_PlayWav(VOICE_100_DE_DATA, sizeof(VOICE_100_DE_DATA)); break;
		}
	} else {
		switch (number) {
		case 1: AUDIO_PlayWav(VOICE_1_DA_DATA, sizeof(VOICE_1_DA_DATA)); break;
		case 2: AUDIO_PlayWav(VOICE_2_DA_DATA, sizeof(VOICE_2_DA_DATA)); break;
		case 3: AUDIO_PlayWav(VOICE_3_DA_DATA, sizeof(VOICE_2_DA_DATA)); break;
		case 4: AUDIO_PlayWav(VOICE_4_DA_DATA, sizeof(VOICE_4_DA_DATA)); break;
		case 5: AUDIO_PlayWav(VOICE_5_DA_DATA, sizeof(VOICE_5_DA_DATA)); break;
		case 6: AUDIO_PlayWav(VOICE_6_DA_DATA, sizeof(VOICE_6_DA_DATA)); break;
		case 7: AUDIO_PlayWav(VOICE_7_DA_DATA, sizeof(VOICE_7_DA_DATA)); break;
		case 8: AUDIO_PlayWav(VOICE_8_DA_DATA, sizeof(VOICE_8_DA_DATA)); break;
		case 9: AUDIO_PlayWav(VOICE_9_DA_DATA, sizeof(VOICE_9_DA_DATA)); break;
		case 10: AUDIO_PlayWav(VOICE_10_DA_DATA, sizeof(VOICE_10_DA_DATA)); break;
		case 11: AUDIO_PlayWav(VOICE_11_DA_DATA, sizeof(VOICE_11_DA_DATA)); break;
		case 12: AUDIO_PlayWav(VOICE_12_DA_DATA, sizeof(VOICE_12_DA_DATA)); break;
		case 13: AUDIO_PlayWav(VOICE_13_DA_DATA, sizeof(VOICE_13_DA_DATA)); break;
		case 14: AUDIO_PlayWav(VOICE_14_DA_DATA, sizeof(VOICE_14_DA_DATA)); break;
		case 15: AUDIO_PlayWav(VOICE_15_DA_DATA, sizeof(VOICE_15_DA_DATA)); break;
		case 16: AUDIO_PlayWav(VOICE_16_DA_DATA, sizeof(VOICE_16_DA_DATA)); break;
		case 17: AUDIO_PlayWav(VOICE_17_DA_DATA, sizeof(VOICE_17_DA_DATA)); break;
		case 18: AUDIO_PlayWav(VOICE_18_DA_DATA, sizeof(VOICE_18_DA_DATA)); break;
		case 19: AUDIO_PlayWav(VOICE_19_DA_DATA, sizeof(VOICE_19_DA_DATA)); break;
		case 20: AUDIO_PlayWav(VOICE_20_DA_DATA, sizeof(VOICE_20_DA_DATA)); break;
		case 30: AUDIO_PlayWav(VOICE_30_DA_DATA, sizeof(VOICE_30_DA_DATA)); break;
		case 40: AUDIO_PlayWav(VOICE_40_DA_DATA, sizeof(VOICE_40_DA_DATA)); break;
		case 50: AUDIO_PlayWav(VOICE_50_DA_DATA, sizeof(VOICE_50_DA_DATA)); break;
		case 60: AUDIO_PlayWav(VOICE_60_DA_DATA, sizeof(VOICE_60_DA_DATA)); break;
		case 70: AUDIO_PlayWav(VOICE_70_DA_DATA, sizeof(VOICE_70_DA_DATA)); break;
		case 80: AUDIO_PlayWav(VOICE_80_DA_DATA, sizeof(VOICE_80_DA_DATA)); break;
		case 90: AUDIO_PlayWav(VOICE_90_DA_DATA, sizeof(VOICE_90_DA_DATA)); break;
		case 100: AUDIO_PlayWav(VOICE_100_DA_DATA, sizeof(VOICE_100_DA_DATA)); break;
		}
	}

}

static void VOICE_SayAnd(void) {
	if (SETUP_GetLanguage() == SETUP_DE) {
		AUDIO_PlayWav(VOICE_AND_DE_DATA, sizeof(VOICE_AND_DE_DATA));
	} else {
		AUDIO_PlayWav(VOICE_AND_DA_DATA, sizeof(VOICE_AND_DA_DATA));

	}
}



void VOICE_Say (int number) {
	int n_ones, n_tens, n_hundreds;

	if (number > 100) {
		n_hundreds = number / 100;
		if (n_hundreds > 1) {
			VOICE_SayNumber(n_hundreds);
		}
		VOICE_SayNumber(100);
		number %= 100;
	}

	// 1..19
	if (number < 20) {
		VOICE_SayNumber(number);
	// 20 .. 99
	} else {
		n_ones = number % 10;
		n_tens = (number / 10) * 10;
		if (n_ones > 0) {
			VOICE_SayNumber(n_ones);
			VOICE_SayAnd();
		}
		VOICE_SayNumber(n_tens);
	}

}
