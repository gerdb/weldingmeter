/**
 *  Project     WeldingMeter
 *  @file		audio.c
 *  @author		Gerd Bartelt - www.sebulli.com
 *  @brief		Play WAV files from flash
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
#include "audio.h"
#include "jingle.h"
#include "setup.h"
#include "stm32746g_discovery.h"
#include "stm32746g_discovery_audio.h"

/* Private functions ---------------------------------------------------------*/

static void AUDIO_PlayNow(const uint8_t* wav_data, int wav_length);
static AUDIO_FifoDef AUDIO_GetLastFifo(void);

/* Private variables ---------------------------------------------------------*/

// Some flags to control the audio playback
int audio_play = 0;
int audio_pause = 0;
int audio_mute = 0;
int audio_played = 0;
int audio_read = 0;


uint8_t audio_buff[AUDIO_OUT_BUFFER_SIZE]; // Buffer for audio DMA transfer
int audio_file_pos = 0;	// file position of the WAV file (WAV array in flash)

const uint8_t* audio_data_ptr; // pointer to the WAV array
int audio_data_length; // Length of the wav array

AUDIO_FifoDef audio_fifo[AUDIO_STACK_SIZE]; // Fifo to play more than 1 WAV files
int audio_fifo_index = -1; // Fifo index

/**
 * Initialize the Audio module and configure the sound chip
 */
void AUDIO_Init(void) {
	BSP_AUDIO_OUT_Init(OUTPUT_DEVICE_BOTH, 1, AUDIO_FREQUENCY_22K);
	AUDIO_SetVolume(SETUP_GetVolume());
}

/**
 * Play a new WAV file, actually put it into the fifo
 *
 * @param wav_data
 *  pointer to the WAV array
 * @param wav_length
 *  length of the WAV data in bytes
 *
 */
void AUDIO_PlayWav(const uint8_t* wav_data, int wav_length){

	// Do not play it at volume = 0
	if (audio_mute)
		return;

	// Put it into the fifo
	if (audio_fifo_index < (AUDIO_STACK_SIZE-1)) {
		audio_fifo_index++;
		audio_fifo[audio_fifo_index].wav_data = wav_data;
		audio_fifo[audio_fifo_index].wav_length = wav_length;
	}

}

/**
 * Plays a sound by its sound ID
 *
 * @param sound
 * 	ID of the sound
 */
void AUDIO_PlaySound(enSOUND sound) {
	switch (sound) {
	case SOUND_PING:
		AUDIO_PlayWav(AUDIO_PING_DATA, AUDIO_PING_LENGTH);
		break;
	case SOUND_SPARK:
		AUDIO_PlayWav(AUDIO_SPARK_DATA, AUDIO_SPARK_LENGTH);
		break;
	case SOUND_BEEP:
		AUDIO_PlayWav(AUDIO_BEEP_DATA, AUDIO_BEEP_LENGTH);
		break;
	default:
		return;
	}
}

/**
 * Get the first entry from the fifo
 */
static AUDIO_FifoDef AUDIO_GetLastFifo(void) {
	AUDIO_FifoDef fifo_element;
	int i;

	// get the first element (FIFO)
	fifo_element.wav_data = audio_fifo[0].wav_data;
	fifo_element.wav_length = audio_fifo[0].wav_length;

	// Remove the first element
	for (i = 1; i<= audio_fifo_index; i++) {
		audio_fifo[i-1].wav_data = audio_fifo[i].wav_data;
		audio_fifo[i-1].wav_length = audio_fifo[i].wav_length;
	}
	audio_fifo_index --;

	return fifo_element;
}

/**
 * Checks whether there is new data in the fifo. If yes, play it
 */
static void AUDIO_CheckFifo(void) {

	AUDIO_FifoDef fifo_element;

	// Something in the fifo? -> play it
	if ((audio_fifo_index >= 0) && (!audio_play)) {
		fifo_element = AUDIO_GetLastFifo();
		AUDIO_PlayNow(fifo_element.wav_data,fifo_element.wav_length );
	}

}

/**
 * Play the WAV now. (do not put it into a fifo)
 *
 * @param wav_data
 *  pointer to the WAV array
 * @param wav_length
 *  length of the WAV data in bytes
 */
static void AUDIO_PlayNow(const uint8_t* wav_data, int wav_length) {

	// Remember the WAV data
	audio_data_ptr = wav_data;
	audio_data_length = wav_length;

	// Read the WAV
	audio_file_pos = 0;
	audio_read = 0;
	AUDIO_Read((uint16_t*)&audio_buff[0], AUDIO_OUT_BUFFER_SIZE);

	// Start playing
	if (audio_pause) {
		BSP_AUDIO_OUT_Resume();
		audio_pause = 0;
	}
	audio_played = 0;
	audio_play = 1;

	// Play it!
	BSP_AUDIO_OUT_Play((uint16_t*) &audio_buff[0], AUDIO_OUT_BUFFER_SIZE);
}

/**
 * Reads an amount of bytes from the WAV array
 *
 * @param buff
 * 	pointer to destinated buffer
 * @param size
 * 	size of bytes to read
 *
 */
void AUDIO_Read(uint16_t* buff, int size) {
	int read = 0;
	uint16_t waveval;
	AUDIO_FifoDef fifo_element;


	while ((audio_file_pos < audio_data_length) && (read < size)) {

		// Get the 8 bit PCM unsigned value and scale it to 16bit signed
		waveval = audio_data_ptr[audio_file_pos];
		waveval -=128;
		waveval *= 256;

		// Put it to the left and right channel
		*buff = waveval;
		buff++;
		*buff = 0;
		buff++;
		*buff = waveval;
		buff++;
		*buff = 0;
		buff++;
		read+=8;
		audio_file_pos++;

		// End of wav file?
		if (audio_file_pos >= audio_data_length) {
			// Still something in the fifo, so concat it!
			if (audio_fifo_index >= 0) {
				audio_file_pos = 0;
				fifo_element= AUDIO_GetLastFifo();
				audio_data_ptr = fifo_element.wav_data;
				audio_data_length = fifo_element.wav_length;
			}
		}
	}

	// Fill the rest with silence
	while (read < size) {
		*buff = 0;
		buff++;
		read+=2;
		audio_read = 1;
	}

}


/**
 * Cyclic called audio task
 */
void AUDIO_Task(void) {
	AUDIO_CheckFifo();
}

/**
 * Sets the volume from 0 to 10
 * Actually we do not use the full dynamic of the discovery board
 * (just 0..50 instead of 0..100), because it would be to loud.
 *
 */
void AUDIO_SetVolume(int vol) {
	if (vol != 0) {
		BSP_AUDIO_OUT_SetVolume( vol * MAXVOLUME);
		audio_mute = 0;
	} else {
		BSP_AUDIO_OUT_SetMute(AUDIO_MUTE_ON);
		audio_mute = 1;
	}
}

/**
 * @brief  Calculates the remaining file size and new position of the pointer.
 * @param  None
 * @retval None
 */
void BSP_AUDIO_OUT_TransferComplete_CallBack(void) {
	if (audio_play) {
		if (audio_read) {
			audio_played = 1;
			audio_pause = 1;
			audio_play = 0;
			BSP_AUDIO_OUT_Pause();
		} else {
			AUDIO_Read((uint16_t*)&audio_buff[AUDIO_OUT_BUFFER_SIZE / 2], AUDIO_OUT_BUFFER_SIZE / 2);
		}
	}
}


/**
 * @brief  Manages the DMA Half Transfer complete interrupt.
 * @param  None
 * @retval None
 */
void BSP_AUDIO_OUT_HalfTransfer_CallBack(void) {
	int i;
	if (audio_play) {
		if (audio_read) {
			// fill the last half with silence
			for (i= AUDIO_OUT_BUFFER_SIZE / 2; i < AUDIO_OUT_BUFFER_SIZE; i++) {
				//audio_buff[i] = 0;
			}
		} else {
			AUDIO_Read((uint16_t*)&audio_buff[0], AUDIO_OUT_BUFFER_SIZE / 2);
		}
	}
}

/**
  * @brief  Clock Config.
  * @param  hsai: might be required to set audio peripheral predivider if any.
  * @param  AudioFreq: Audio frequency used to play the audio stream.
  * @note   This API is called by BSP_AUDIO_OUT_Init() and BSP_AUDIO_OUT_SetFrequency()
  *         Being __weak it can be overwritten by the application
  * @retval None
  */
void BSP_AUDIO_OUT_ClockConfig(SAI_HandleTypeDef *hsai, uint32_t AudioFreq, void *Params)
{
  RCC_PeriphCLKInitTypeDef RCC_ExCLKInitStruct;

  HAL_RCCEx_GetPeriphCLKConfig(&RCC_ExCLKInitStruct);

  /* Set the PLL configuration according to the audio frequency */
  if((AudioFreq == AUDIO_FREQUENCY_11K) || (AudioFreq == AUDIO_FREQUENCY_22K) || (AudioFreq == AUDIO_FREQUENCY_44K))
  {
    /* Configure PLLSAI prescalers */
    /* PLLI2S_VCO: VCO_429M
    SAI_CLK(first level) = PLLI2S_VCO/PLLSAIQ = 429/2 = 214.5 Mhz
    SAI_CLK_x = SAI_CLK(first level)/PLLI2SDivQ = 214.5/19 = 11.289 Mhz */
    RCC_ExCLKInitStruct.PeriphClockSelection = RCC_PERIPHCLK_SAI2;
    RCC_ExCLKInitStruct.Sai2ClockSelection = RCC_SAI2CLKSOURCE_PLLI2S;
    RCC_ExCLKInitStruct.PLLI2S.PLLI2SP = 8;
    RCC_ExCLKInitStruct.PLLI2S.PLLI2SN = 429;
    RCC_ExCLKInitStruct.PLLI2S.PLLI2SQ = 2;
    RCC_ExCLKInitStruct.PLLI2SDivQ = 19;
    HAL_RCCEx_PeriphCLKConfig(&RCC_ExCLKInitStruct);
  }
  else /* AUDIO_FREQUENCY_8K, AUDIO_FREQUENCY_16K, AUDIO_FREQUENCY_48K), AUDIO_FREQUENCY_96K */
  {
    /* SAI clock config
    PLLI2S_VCO: VCO_344M
    SAI_CLK(first level) = PLLI2S_VCO/PLLSAIQ = 344/7 = 49.142 Mhz
    SAI_CLK_x = SAI_CLK(first level)/PLLI2SDivQ = 49.142/1 = 49.142 Mhz */
    RCC_ExCLKInitStruct.PeriphClockSelection = RCC_PERIPHCLK_SAI2;
    RCC_ExCLKInitStruct.Sai2ClockSelection = RCC_SAI2CLKSOURCE_PLLI2S;
//    RCC_ExCLKInitStruct.PLLI2S.PLLI2SP = 8;
    RCC_ExCLKInitStruct.PLLI2S.PLLI2SN = 344;
    RCC_ExCLKInitStruct.PLLI2S.PLLI2SQ = 7;
    RCC_ExCLKInitStruct.PLLI2SDivQ = 1;
    HAL_RCCEx_PeriphCLKConfig(&RCC_ExCLKInitStruct);
  }
}
