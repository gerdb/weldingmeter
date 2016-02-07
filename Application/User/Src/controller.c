/**
 *  Project     WeldingMeter
 *  @file		controller.c
 *  @author		Gerd Bartelt - www.sebulli.com
 *  @brief		Touch display controller
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
#include "display.h"
#include "stm32746g_discovery.h"
#include "stm32746g_discovery_lcd.h"
#include "stm32746g_discovery_ts.h"
#include "audio.h"
#include "measurement.h"
#include "setup.h"
#include "voice.h"

/* Private defines ---------------------------------------------------------*/

// Touch fields
#define TOUCH_BACK_XMIN		290
#define TOUCH_BACK_XMAX		340
#define TOUCH_BACK_YMIN		211
#define TOUCH_BACK_YMAX		261

#define TOUCH_FORW_XMIN		350
#define TOUCH_FORW_XMAX		400
#define TOUCH_FORW_YMIN		211
#define TOUCH_FORW_YMAX		261

#define TOUCH_NEXT_XMIN		420
#define TOUCH_NEXT_XMAX		470
#define TOUCH_NEXT_YMIN		211
#define TOUCH_NEXT_YMAX		261

#define TOUCH_VOL_M_XMIN	10
#define TOUCH_VOL_M_XMAX	80
#define TOUCH_VOL_M_YMIN	10
#define TOUCH_VOL_M_YMAX	70

#define TOUCH_VOL_P_XMIN	310
#define TOUCH_VOL_P_XMAX	380
#define TOUCH_VOL_P_YMIN	10
#define TOUCH_VOL_P_YMAX	70

#define TOUCH_AC_XMIN		20
#define TOUCH_AC_XMAX		70
#define TOUCH_AC_YMIN		111
#define TOUCH_AC_YMAX		161

#define TOUCH_DC_XMIN		80
#define TOUCH_DC_XMAX		130
#define TOUCH_DC_YMIN		111
#define TOUCH_DC_YMAX		161

#define TOUCH_MEAN_XMIN		180
#define TOUCH_MEAN_XMAX		230
#define TOUCH_MEAN_YMIN		111
#define TOUCH_MEAN_YMAX		251

#define TOUCH_PEAK_XMIN		240
#define TOUCH_PEAK_XMAX		290
#define TOUCH_PEAK_YMIN		111
#define TOUCH_PEAK_YMAX		161

#define TOUCH_LANG_DE_XMIN	20
#define TOUCH_LANG_DE_XMAX	70
#define TOUCH_LANG_DE_YMIN	201
#define TOUCH_LANG_DE_YMAX	251

#define TOUCH_LANG_DA_XMIN	80
#define TOUCH_LANG_DA_XMAX	130
#define TOUCH_LANG_DA_YMIN	201
#define TOUCH_LANG_DA_YMAX	251

/* Private functions ---------------------------------------------------------*/

static void CONTROLLER_TouchButtons(void);
static void CONTROLLER_Speak(void);

/* Private variables ---------------------------------------------------------*/

/* Types  ---------------------------------------------------------*/

typedef enum {
	start,
	fade_in_intro,
	intro,
	fade_out_intro,
	fade_in_main_screen,
	main_screen,
	fade_out_main_screen,
	fade_in_settings,
	settings,
	fade_out_settings

} en_CONTROLLER_screen;

en_CONTROLLER_screen screen = start;	// The current state
int startup_cnt = 0;	// Timer to switch between the screens and to fade in/out

int cursor = 0;	// Cursor position
int autosave_cnt = 0;	// Autosave timer

int speak_cnt = 0;	// Timer for voice output

/**
 * Initialization of the module
 */
void CONTROLLER_Init(void) {
}


/**
 * Controller task with main statemachine
 */
void CONTROLLER_Task(void) {

	static int taskcnt_256ms = 0;
	int taskcnt_256ms_mod_16;
	int taskcnt_256ms_mod_32;

	// Timer to switch between the screens and to fade in/out
	if (startup_cnt < 99999) {
		startup_cnt++;
	}

	switch (screen) {
	// Start state
	case start:
		if (startup_cnt > 1000) {
			startup_cnt = 0;
			screen = fade_in_intro;
			DISPLAY_ShowIntro();
			AUDIO_PlaySound(SOUND_PING);
		}
		break;

	// Fade in the intro screen
	case fade_in_intro:
		BSP_LCD_SetTransparency(1, startup_cnt);

		if (startup_cnt >= 255) {
			startup_cnt = 0;
			screen = intro;
			BSP_LCD_SelectLayer(0);
			BSP_LCD_SetTransparency(0, 255);
			BSP_LCD_Clear(LCD_COLOR_WHITE);
			BSP_LCD_SelectLayer(1);
		}
		break;

	// Intro screen with welding effect
	case intro:
		if (startup_cnt == 1500) {
			AUDIO_PlaySound(SOUND_SPARK);
		}

		if (((startup_cnt > 1500) && (startup_cnt < 1700))
				|| ((startup_cnt > 2000) && (startup_cnt < 3400))) {
			BSP_LCD_SetTransparency(1, ((startup_cnt - 1000) * 8 + 55000) / 2);
		} else {
			BSP_LCD_SetTransparency(1, 255);
		}

		if (startup_cnt > 4000) {
			BSP_LCD_SelectLayer(0);
			BSP_LCD_SetTransparency(0, 0);
			BSP_LCD_Clear(LCD_COLOR_BLACK);

			startup_cnt = 0;
			screen = fade_out_intro;
		}
		break;

	// Fade out the intro screen
	case fade_out_intro:
		BSP_LCD_SetTransparency(1, 255 - startup_cnt);

		if (startup_cnt >= 255) {
			startup_cnt = 0;
			screen = fade_in_main_screen;
			DISPLAY_ShowBackground();
		}
		break;

	// Fade in the main screen
	case fade_in_main_screen:
		BSP_LCD_SetTransparency(0, startup_cnt);

		if (startup_cnt >= 255) {
			startup_cnt = 0;
			screen = main_screen;
			DISPLAY_InitScopes();
			BSP_LCD_SelectLayer(1);
			BSP_LCD_Clear(0x00000000);
			BSP_LCD_SetTransparency(1, 255);
			DISPLAY_Show_Value_Amps(0);
			DISPLAY_Show_Value_Frq(0);
			DISPLAY_Show_Value_Duty(0);
			DISPLAY_Show_Value_AmpsMax(0);
			DISPLAY_Show_Value_AmpsMin(0);
			DISPLAY_Show_Value_AmpsMin(0);
			DISPLAY_Show_Scope1();
			DISPLAY_Show_Scope2();
			DISPLAY_Show_Version();
		}
		break;

	// Display the main screen. The tasks are shared over the 1ms time slots
	case main_screen:

		taskcnt_256ms_mod_16 = taskcnt_256ms % 16;
		taskcnt_256ms_mod_32 = taskcnt_256ms % 32;

		if (MEASUREMENT_GetRun() == RUN) {
			cursor = -1;
			if (taskcnt_256ms == 0) {

				// Get new value and handle the autosave mechanism
				DISPLAY_NewValue(MEASUREMENT_GetSlowFilt());
				if (autosave_cnt < AUTOSAVE_CYCLE) {
					autosave_cnt++;
				} else {
					MEASUREMENT_SaveHistory();
					autosave_cnt = 0;
				}

			} else if (taskcnt_256ms == 1) {
				DISPLAY_Show_Scope1();
			} else if (taskcnt_256ms == 2) {
				DISPLAY_Show_Value_Amps(MEASUREMENT_GetSlowFilt());
			} else if (taskcnt_256ms == 3) {
				DISPLAY_Show_Value_Frq(MEASUREMENT_GetFrequency());
			} else if (taskcnt_256ms == 4) {
				DISPLAY_Show_Value_AmpsMin(MEASUREMENT_GetMin());
			} else if (taskcnt_256ms == 5) {
				DISPLAY_Show_Value_AmpsMax(MEASUREMENT_GetMax());
			} else if (taskcnt_256ms == 6) {
				DISPLAY_Show_Value_Duty(MEASUREMENT_GetRatio());
			} else if (taskcnt_256ms == 7) {
				DISPLAY_Show_Cursor(cursor, autosave_cnt);
			} else if (taskcnt_256ms == 8) {
				CONTROLLER_Speak();
			} else if (taskcnt_256ms_mod_32 == 20) {
				MEASUREMENT_CopyZoomField(scope2);
			} else if (taskcnt_256ms_mod_32 == 21) {
				DISPLAY_Show_Scope2();
			}
		} else {

			// Show the cursor and reset it to position 4
			if (cursor == -1) {
				cursor = 4;
				DISPLAY_ShowHistory(cursor);
			}
			DISPLAY_Show_Cursor(cursor, autosave_cnt);
		}

		// Read touch buttons
		if (taskcnt_256ms_mod_16 == 15) {
			CONTROLLER_TouchButtons();
		}

		taskcnt_256ms++;
		if (taskcnt_256ms >= 256) {
			taskcnt_256ms = 0;
		}
		break;

	// Fade ou the main screen
	case fade_out_main_screen:
		BSP_LCD_SetTransparency(0, 255 - startup_cnt);
		BSP_LCD_SetTransparency(1, 255 - startup_cnt);

		if (startup_cnt >= 255) {
			startup_cnt = 0;
			screen = fade_in_settings;
			DISPLAY_ShowSettings();
		}
		break;

	// Fade in the settings screen
	case fade_in_settings:
		BSP_LCD_SetTransparency(0, startup_cnt);

		if (startup_cnt >= 255) {
			startup_cnt = 0;
			screen = settings;
			BSP_LCD_SelectLayer(1);
			BSP_LCD_Clear(0x00000000);
			BSP_LCD_SetTransparency(1, 255);
		}
		break;

	// The settings screen
	case settings:
		CONTROLLER_TouchButtons();
		DISPLAY_DrawBar(100, 34, 10, SETUP_GetVolume());
		if (SETUP_GetACDC() == SETUP_AC) {
			BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
			BSP_LCD_FillCircle(106, 99, 4);
			BSP_LCD_SetTextColor(DISPLAY_COLOR_LTBLUE);
			BSP_LCD_FillCircle(46, 99, 4);
		} else {
			BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
			BSP_LCD_FillCircle(46, 99, 4);
			BSP_LCD_SetTextColor(DISPLAY_COLOR_LTBLUE);
			BSP_LCD_FillCircle(106, 99, 4);
		}
		if (SETUP_GetMEAN_PEAK() == SETUP_MEAN) {
			BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
			BSP_LCD_FillCircle(266, 99, 4);
			BSP_LCD_SetTextColor(DISPLAY_COLOR_LTBLUE);
			BSP_LCD_FillCircle(206, 99, 4);
		} else {
			BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
			BSP_LCD_FillCircle(206, 99, 4);
			BSP_LCD_SetTextColor(DISPLAY_COLOR_LTBLUE);
			BSP_LCD_FillCircle(266, 99, 4);
		}
		if (SETUP_GetLanguage() == SETUP_DE) {
			BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
			BSP_LCD_FillCircle(106, 189, 4);
			BSP_LCD_SetTextColor(DISPLAY_COLOR_LTBLUE);
			BSP_LCD_FillCircle(46, 189, 4);
		} else {
			BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
			BSP_LCD_FillCircle(46, 189, 4);
			BSP_LCD_SetTextColor(DISPLAY_COLOR_LTBLUE);
			BSP_LCD_FillCircle(106, 189, 4);
		}
		break;

	// Fade out the settings screen
	case fade_out_settings:
		BSP_LCD_SetTransparency(0, 255 - startup_cnt);
		BSP_LCD_SetTransparency(1, 255 - startup_cnt);

		if (startup_cnt >= 255) {
			startup_cnt = 0;
			screen = fade_in_main_screen;
			DISPLAY_ShowBackground();
		}
		break;
	}

}

/**
 * Determine, whether the current shout be said.
 *
 */
static void CONTROLLER_Speak(void) {

	int val = 0;
	static int said_val = 0;

	// Which value? Peak or mean?
	if (SETUP_GetMEAN_PEAK() == SETUP_MEAN) {
		val = MEASUREMENT_GetSlowFilt();
	} else {
		val = MEASUREMENT_GetMax();
	}

	// Say it at least every 5 seconds, but not more than every second
	speak_cnt++;
	if (speak_cnt >= 5) {

		// Wait, if there was no, or a slight change
		if (((val * 10) > (said_val * 11)) || // > 10%
				((val * 10) < (said_val * 9)) || // < 10%
				(speak_cnt > 20)) { // always after 5sec
			speak_cnt = 0;

			VOICE_Say(val);
			said_val = val;
		}
	}

}

/**
 * @brief  Test touch screen state and modify audio state machine according to that
 * @param  None
 * @retval None
 */
static void CONTROLLER_TouchButtons(void) {

	static TS_StateTypeDef TS_State = { 0 };

	if (TS_State.touchDetected == 1) /* If previous touch has not been released, we don't proceed any touch command */
	{
		BSP_TS_GetState(&TS_State);
	} else {
		BSP_TS_GetState(&TS_State);
		if (TS_State.touchDetected == 1) {
			if (screen == main_screen) {
				if ((TS_State.touchX[0] >= TOUCH_NEXT_XMIN)
						&& (TS_State.touchX[0] <= TOUCH_NEXT_XMAX)
						&& (TS_State.touchY[0] >= TOUCH_NEXT_YMIN)
						&& (TS_State.touchY[0] <= TOUCH_NEXT_YMAX)) {
					screen = fade_out_main_screen;
					startup_cnt = 0;
					AUDIO_PlaySound(SOUND_BEEP);
				}
				if ((TS_State.touchX[0] >= TOUCH_BACK_XMIN)
						&& (TS_State.touchX[0] <= TOUCH_BACK_XMAX)
						&& (TS_State.touchY[0] >= TOUCH_BACK_YMIN)
						&& (TS_State.touchY[0] <= TOUCH_BACK_YMAX)) {
					if (MEASUREMENT_GetRun() == STOP) {
						if (cursor < (HISTORY_ENTRIES - 1))
							cursor++;
						AUDIO_PlaySound(SOUND_BEEP);
						DISPLAY_ShowHistory(cursor);
					}
				}
				if ((TS_State.touchX[0] >= TOUCH_FORW_XMIN)
						&& (TS_State.touchX[0] <= TOUCH_FORW_XMAX)
						&& (TS_State.touchY[0] >= TOUCH_FORW_YMIN)
						&& (TS_State.touchY[0] <= TOUCH_FORW_YMAX)) {
					if (MEASUREMENT_GetRun() == STOP) {
						if (cursor > 0)
							cursor--;
						AUDIO_PlaySound(SOUND_BEEP);
						DISPLAY_ShowHistory(cursor);
					}
				}
			} else if (screen == settings) {
				if ((TS_State.touchX[0] >= TOUCH_NEXT_XMIN)
						&& (TS_State.touchX[0] <= TOUCH_NEXT_XMAX)
						&& (TS_State.touchY[0] >= TOUCH_NEXT_YMIN)
						&& (TS_State.touchY[0] <= TOUCH_NEXT_YMAX)) {
					SETUP_Save();
					screen = fade_out_settings;
					startup_cnt = 0;
					AUDIO_PlaySound(SOUND_BEEP);
				}
				if ((TS_State.touchX[0] >= TOUCH_VOL_M_XMIN)
						&& (TS_State.touchX[0] <= TOUCH_VOL_M_XMAX)
						&& (TS_State.touchY[0] >= TOUCH_VOL_M_YMIN)
						&& (TS_State.touchY[0] <= TOUCH_VOL_M_YMAX)) {
					if (SETUP_GetVolume() > 0) {
						SETUP_SetVolume(SETUP_GetVolume() - 1);
						AUDIO_SetVolume(SETUP_GetVolume());
						AUDIO_PlaySound(SOUND_BEEP);
					}
				}
				if ((TS_State.touchX[0] >= TOUCH_VOL_P_XMIN)
						&& (TS_State.touchX[0] <= TOUCH_VOL_P_XMAX)
						&& (TS_State.touchY[0] >= TOUCH_VOL_P_YMIN)
						&& (TS_State.touchY[0] <= TOUCH_VOL_P_YMAX)) {
					if (SETUP_GetVolume() < 10) {
						SETUP_SetVolume(SETUP_GetVolume() + 1);
						AUDIO_SetVolume(SETUP_GetVolume());
						AUDIO_PlaySound(SOUND_BEEP);
					}
				}
				if ((TS_State.touchX[0] >= TOUCH_DC_XMIN)
						&& (TS_State.touchX[0] <= TOUCH_DC_XMAX)
						&& (TS_State.touchY[0] >= TOUCH_DC_YMIN)
						&& (TS_State.touchY[0] <= TOUCH_DC_YMAX)) {
					SETUP_SetACDC(SETUP_DC);
					AUDIO_PlaySound(SOUND_BEEP);
				}
				if ((TS_State.touchX[0] >= TOUCH_AC_XMIN)
						&& (TS_State.touchX[0] <= TOUCH_AC_XMAX)
						&& (TS_State.touchY[0] >= TOUCH_AC_YMIN)
						&& (TS_State.touchY[0] <= TOUCH_AC_YMAX)) {
					SETUP_SetACDC(SETUP_AC);
					AUDIO_PlaySound(SOUND_BEEP);
				}
				if ((TS_State.touchX[0] >= TOUCH_MEAN_XMIN)
						&& (TS_State.touchX[0] <= TOUCH_MEAN_XMAX)
						&& (TS_State.touchY[0] >= TOUCH_MEAN_YMIN)
						&& (TS_State.touchY[0] <= TOUCH_MEAN_YMAX)) {
					SETUP_SetMEAN_PEAK(SETUP_MEAN);
					AUDIO_PlaySound(SOUND_BEEP);
				}
				if ((TS_State.touchX[0] >= TOUCH_PEAK_XMIN)
						&& (TS_State.touchX[0] <= TOUCH_PEAK_XMAX)
						&& (TS_State.touchY[0] >= TOUCH_PEAK_YMIN)
						&& (TS_State.touchY[0] <= TOUCH_PEAK_YMAX)) {
					SETUP_SetMEAN_PEAK(SETUP_PEAK);
					AUDIO_PlaySound(SOUND_BEEP);
				}
				if ((TS_State.touchX[0] >= TOUCH_LANG_DE_XMIN)
						&& (TS_State.touchX[0] <= TOUCH_LANG_DE_XMAX)
						&& (TS_State.touchY[0] >= TOUCH_LANG_DE_YMIN)
						&& (TS_State.touchY[0] <= TOUCH_LANG_DE_YMAX)) {
					SETUP_SetLanguage(SETUP_DE);
					AUDIO_PlaySound(SOUND_BEEP);
				}
				if ((TS_State.touchX[0] >= TOUCH_LANG_DA_XMIN)
						&& (TS_State.touchX[0] <= TOUCH_LANG_DA_XMAX)
						&& (TS_State.touchY[0] >= TOUCH_LANG_DA_YMIN)
						&& (TS_State.touchY[0] <= TOUCH_LANG_DA_YMAX)) {
					SETUP_SetLanguage(SETUP_DA);
					AUDIO_PlaySound(SOUND_BEEP);
				}

			}
		}
	}
}
