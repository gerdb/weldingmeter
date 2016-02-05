/* Includes ------------------------------------------------------------------*/
#include "display.h"
#include "font8x5.h"
#include "jpg_data.h"
#include "integer.h"
#include "tjpgd.h"
#include "stm32746g_discovery.h"
#include "stm32746g_discovery_lcd.h"
#include "stm32746g_discovery_ts.h"
#include "audio.h"
#include "measurement.h"
#include "setup.h"
#include "voice.h"

/* Private defines ---------------------------------------------------------*/

#define TOUCH_BACK_XMIN         290
#define TOUCH_BACK_XMAX         340
#define TOUCH_BACK_YMIN         211
#define TOUCH_BACK_YMAX         261

#define TOUCH_FORW_XMIN         350
#define TOUCH_FORW_XMAX         400
#define TOUCH_FORW_YMIN         211
#define TOUCH_FORW_YMAX         261

#define TOUCH_NEXT_XMIN         420
#define TOUCH_NEXT_XMAX         470
#define TOUCH_NEXT_YMIN         211
#define TOUCH_NEXT_YMAX         261

#define TOUCH_VOL_M_XMIN         10
#define TOUCH_VOL_M_XMAX         80
#define TOUCH_VOL_M_YMIN         10
#define TOUCH_VOL_M_YMAX         70

#define TOUCH_VOL_P_XMIN         310
#define TOUCH_VOL_P_XMAX         380
#define TOUCH_VOL_P_YMIN         10
#define TOUCH_VOL_P_YMAX         70

#define TOUCH_AC_XMIN         20
#define TOUCH_AC_XMAX         70
#define TOUCH_AC_YMIN         111
#define TOUCH_AC_YMAX         161

#define TOUCH_DC_XMIN         80
#define TOUCH_DC_XMAX         130
#define TOUCH_DC_YMIN         111
#define TOUCH_DC_YMAX         161

#define TOUCH_MEAN_XMIN         180
#define TOUCH_MEAN_XMAX         230
#define TOUCH_MEAN_YMIN         111
#define TOUCH_MEAN_YMAX         251

#define TOUCH_PEAK_XMIN         240
#define TOUCH_PEAK_XMAX         290
#define TOUCH_PEAK_YMIN         111
#define TOUCH_PEAK_YMAX         161

#define TOUCH_LANG_DE_XMIN         20
#define TOUCH_LANG_DE_XMAX         70
#define TOUCH_LANG_DE_YMIN         201
#define TOUCH_LANG_DE_YMAX         251

#define TOUCH_LANG_DA_XMIN         80
#define TOUCH_LANG_DA_XMAX         130
#define TOUCH_LANG_DA_YMIN         201
#define TOUCH_LANG_DA_YMAX         251

/* Private functions ---------------------------------------------------------*/

static void DISPLAY_NewValue(int val);
static void DISPLAY_ShowIntro(void);
static void DISPLAY_ShowBackground(void);
static void DISPLAY_ShowSettings(void);
static void DISPLAY_DrawString(int xpos, int ypos, int size, char* s);
static void DISPLAY_DrawChar(int xpos, int ypos, int size, char c);
static void DISPLAY_Show_Value_Amps(int amps);
static void DISPLAY_Show_Value_Frq(int frq);
static void DISPLAY_Show_Value_Duty(int duty);
static void DISPLAY_Show_Value_AmpsMin(int amps);
static void DISPLAY_Show_Value_AmpsMax(int amps);
static void DISPLAY_Show_Scope1(void);
static void DISPLAY_Show_Scope2(void);
static void DISPLAY_TouchButtons(void);
static void DISPLAY_DrawBar(int xpos, int ypos, int max, int val);
static void DISPLAY_Show_Cursor(void);
static void DISPLAY_ShowHistory(int index);

/* Private variables ---------------------------------------------------------*/

/* Types  ---------------------------------------------------------*/
typedef struct {
	int jpg_size;
	int jpg_pos;
	uint8_t* jpg_data;
} IODEV;

typedef enum {
	start,
	fade_in_intro,
	intro,
	fade_out_intro,
	fade_in_background,
	background,
	fade_out_background,
	fade_in_settings,
	settings,
	fade_out_settings

} en_DISPLAY_startup;

extern const uint8_t font8x5[96][5];

uint8_t scope1[250];
uint8_t scope1_old[250];
uint8_t scope2[160];
uint8_t scope2_old[160];

uint8_t work[3100];
JDEC jdec; // Decompression object
JRESULT res; // Result code of TJpgDec API

en_DISPLAY_startup screen = start;
int startup_cnt = 0;

volatile int xxxx = 0;
int cursor = 0;
int autosave_cnt = 0;
MEASUREMENT_tHistory history_entry;


static void DISPLAY_ShowJPG(uint32_t LayerIndex, const int jpg_length,
		const uint8_t jpg_data[]);

static void DISPLAY_ShowIntro(void) {
	BSP_LCD_SetTransparency(0, 255);
	BSP_LCD_SetTransparency(1, 0);
	DISPLAY_ShowJPG(1, JPG_INTRO_LENGTH, JPG_INTRO_DATA);
	BSP_LCD_SetTransparency(0, 0);
}

static void DISPLAY_ShowBackground(void) {
	DISPLAY_ShowJPG(0, JPG_BACKGROUND_LENGTH, JPG_BACKGROUND_DATA);
	BSP_LCD_SetTransparency(1, 0);
}
static void DISPLAY_ShowSettings(void) {
	DISPLAY_ShowJPG(0, JPG_SETTINGS_LENGTH, JPG_SETTINGS_DATA);
	BSP_LCD_SetTransparency(1, 0);
}

static void DISPLAY_DrawString(int xpos, int ypos, int size, char* s) {
	while (*s) {
		DISPLAY_DrawChar(xpos, ypos, size, *s);
		xpos += 6 * size;
		s++;
	}
}

static void DISPLAY_DrawChar(int xpos, int ypos, int size, char c) {
	int x, y;
	int xx, yy;
	int xxx, yyy;
	int pixelsize;
	uint8_t col;

	if (size <= 2) {
		pixelsize = size;
	} else {
		pixelsize = size - 1;
	}

	for (x = 0; x < 5; x++) {
		col = font8x5[(int) (c - ' ')][x];
		xx = xpos + x * size;
		for (y = 0; y < 7; y++) {
			yy = ypos + y * size;
			for (xxx = 0; xxx < pixelsize; xxx++) {
				for (yyy = 0; yyy < pixelsize; yyy++) {
					if (col & 0x01) {
						BSP_LCD_DrawPixel(xx + xxx, yy + yyy, DISPLAY_COLOR_LTBLUE);
					} else {
						BSP_LCD_DrawPixel(xx + xxx, yy + yyy, 0x00000000);
					}
				}
			}
			col >>= 1;
		}

	}
}

static void DISPLAY_DrawBar(int xpos, int ypos, int max, int val) {
	int x;
	int xx;
	int xxx, yyy;

	for (x = 0; x < max; x++) {
		xx = xpos + x * 20;
		for (xxx = 0; xxx < 16; xxx++) {
			for (yyy = 0; yyy < 24; yyy++) {
				if (val > x) {
					BSP_LCD_DrawPixel(xx + xxx, ypos + yyy, DISPLAY_COLOR_LTBLUE);
				} else {
					BSP_LCD_DrawPixel(xx + xxx, ypos + yyy, 0x00000000);
				}
			}
		}
	}
}

void DISPLAY_Task(void) {

	static int taskcnt_256ms = 0;
	int taskcnt_256ms_mod_16;
	int taskcnt_256ms_mod_32;

	if (startup_cnt < 99999) {
		startup_cnt++;
	}

	switch (screen) {
	case start:
		if (startup_cnt > 1000) {
			startup_cnt = 0;
			screen = fade_in_intro;
			DISPLAY_ShowIntro();
			AUDIO_PlaySound(SOUND_PING);
		}
		break;
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
	case fade_out_intro:
		BSP_LCD_SetTransparency(1, 255 - startup_cnt);

		if (startup_cnt >= 255) {
			startup_cnt = 0;
			screen = fade_in_background;
			DISPLAY_ShowBackground();
		}
		break;
	case fade_in_background:
		BSP_LCD_SetTransparency(0, startup_cnt);

		if (startup_cnt >= 255) {
			startup_cnt = 0;
			screen = background;
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
		}
		break;
	case background:

		taskcnt_256ms_mod_16 = taskcnt_256ms % 16;
		taskcnt_256ms_mod_32 = taskcnt_256ms % 32;

		if (MEASUREMENT_GetRun() == RUN) {
			if (taskcnt_256ms == 0) {
				DISPLAY_NewValue(MEASUREMENT_GetSlowFilt());
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
			} else if (taskcnt_256ms_mod_32 == 20) {
				MEASUREMENT_CopyZoomField(scope2);
			} else if (taskcnt_256ms_mod_32 == 21) {
				DISPLAY_Show_Scope2();
			}
		} else {
			DISPLAY_Show_Cursor();
		}


		if (taskcnt_256ms_mod_16 == 15) {
			DISPLAY_TouchButtons();
		}

		taskcnt_256ms++;
		if (taskcnt_256ms >= 256) {
			taskcnt_256ms = 0;
			if (MEASUREMENT_GetRun() == RUN) {
				if (autosave_cnt < 25) {
					autosave_cnt++;
				} else {
					MEASUREMENT_SaveHistory();
					autosave_cnt = 0;
				}
			}

		}
		break;
	case fade_out_background:
		BSP_LCD_SetTransparency(0, 255 - startup_cnt);
		BSP_LCD_SetTransparency(1, 255 - startup_cnt);

		if (startup_cnt >= 255) {
			startup_cnt = 0;
			screen = fade_in_settings;
			DISPLAY_ShowSettings();
		}
		break;
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
	case settings:
		DISPLAY_TouchButtons();
		DISPLAY_DrawBar(100, 34, 10, SETUP_GetVolume());
		if (SETUP_GetACDC() == SETUP_AC) {
			BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
			BSP_LCD_FillCircle(106,99,4);
			BSP_LCD_SetTextColor(DISPLAY_COLOR_LTBLUE);
			BSP_LCD_FillCircle(46,99,4);
		} else {
			BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
			BSP_LCD_FillCircle(46,99,4);
			BSP_LCD_SetTextColor(DISPLAY_COLOR_LTBLUE);
			BSP_LCD_FillCircle(106,99,4);
		}
		if (SETUP_GetMEAN_PEAK() == SETUP_MEAN) {
			BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
			BSP_LCD_FillCircle(266,99,4);
			BSP_LCD_SetTextColor(DISPLAY_COLOR_LTBLUE);
			BSP_LCD_FillCircle(206,99,4);
		} else {
			BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
			BSP_LCD_FillCircle(206,99,4);
			BSP_LCD_SetTextColor(DISPLAY_COLOR_LTBLUE);
			BSP_LCD_FillCircle(266,99,4);
		}
		if (SETUP_GetLanguage() == SETUP_DE) {
			BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
			BSP_LCD_FillCircle(106,189,4);
			BSP_LCD_SetTextColor(DISPLAY_COLOR_LTBLUE);
			BSP_LCD_FillCircle(46,189,4);
		} else {
			BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
			BSP_LCD_FillCircle(46,189,4);
			BSP_LCD_SetTextColor(DISPLAY_COLOR_LTBLUE);
			BSP_LCD_FillCircle(106,189,4);
		}
		break;
	case fade_out_settings:
		BSP_LCD_SetTransparency(0, 255 - startup_cnt);
		BSP_LCD_SetTransparency(1, 255 - startup_cnt);

		if (startup_cnt >= 255) {
			startup_cnt = 0;
			screen = fade_in_background;
			DISPLAY_ShowBackground();
		}
		break;
	}

}

static void DISPLAY_Show_Cursor(void) {
	int i,x,y;
	int color;
	if (MEASUREMENT_GetRun() == RUN) {
		cursor = -1;
	} else {
		if (cursor == -1)
			cursor = 0;
	}

	for (i=0; i < HISTORY_ENTRIES; i++) {
		if (i == cursor) {
			color = DISPLAY_COLOR_LTBLUE;
		} else {
			color = 0x00000000;
		}
		for (y = 0; y< 8; y++) {
			for (x = -y; x< y; x++) {
				BSP_LCD_DrawPixel(270 + x - i * 25, 128 + y, color);
			}
		}

	}


}


static void DISPLAY_Show_Value_AmpsMax(int amps) {
	char txt[10];
	sprintf(txt, "%4d", amps);
	DISPLAY_DrawString(180, 152, 3, txt);
	DISPLAY_DrawString(255, 152, 3, "A");
}

void DISPLAY_Show_Value_AmpsMin(int amps) {
	char txt[10];
	sprintf(txt,"%4d", amps);
	DISPLAY_DrawString(180,227,3, txt);
	DISPLAY_DrawString(255,227,3, "A");
}

void DISPLAY_Show_Value_Amps(int amps) {
	char txt[10];
	sprintf(txt,"%4d", amps);
	DISPLAY_DrawString(320,66,4, txt);
	DISPLAY_DrawString(420,66,4, "A");
}

void DISPLAY_Show_Value_Frq(int frq) {
	char txt[10];
	sprintf(txt,"%3d", frq);
	DISPLAY_DrawString(320,116,4, txt);
	DISPLAY_DrawString(400,116,4, "Hz");
}

void DISPLAY_Show_Value_Duty(int duty) {
	char txt[10];
	sprintf(txt,"%3d", duty);
	DISPLAY_DrawString(320,166,4, txt);
	DISPLAY_DrawString(400,166,4, "%");
}

/**
 * 250 x 101 Pixel
 */
static void DISPLAY_Show_Scope1(void) {
	int x;
	for (x = 0; x < 250; x++) {
		if (scope1[x] != scope1_old[x]) {
			if ((scope1_old[x] >= 0) && (scope1_old[x] <= 100))
				BSP_LCD_DrawPixel(20 + x, 121 - scope1_old[x], 0x00000000);
			if ((scope1[x] >= 0) && (scope1[x] <= 100))
				BSP_LCD_DrawPixel(20 + x, 121 - scope1[x], 0xFF019cff);
			scope1_old[x] = scope1[x];
		}
	}

}

/**
 * 250 x 101 Pixel
 */
static void DISPLAY_Show_Scope2(void) {
	int x;
	for (x = 0; x < 160; x++) {
		//scope2[x] = 0;//ADCConvertedValue[x] / 64;
		if (scope2[x] != scope2_old[x]) {
			if ((scope2_old[x] >= 0) && (scope2_old[x] <= 100))
				BSP_LCD_DrawPixel(20 + x, 248 - scope2_old[x], 0x00000000);
			if ((scope2[x] >= 0) && (scope2[x] <= 100))
				BSP_LCD_DrawPixel(20 + x, 248 - scope2[x], 0xFF019cff);
			scope2_old[x] = scope2[x];
		}
	}
}

static void DISPLAY_NewValue(int amps) {
	static int t = 0;
	int val;
	int x;
	t++;

	// Shift left
	for (x = 0; x < 249; x++) {
		scope1[x] = scope1[x + 1];
	}

	if (SETUP_GetACDC() == SETUP_AC) {
		// Scale -200..200A to 0..100px
		val = amps / 4 + 50;
	} else {
		// Scale 0..200A to 0..100px
		val = amps / 2;
	}

	// Limit it
	if (val < 0)
		val = 0;
	if (val > 100)
		val = 100;
	scope1[249] = val;
}
/**
 * @brief  LCD configuration
 * @param  None
 * @retval None
 */
void DISPLAY_Init(void) {

	int x;
	for (x = 0; x < 250; x++) {
		scope1[x] = 0;
		scope1_old[x] = 0;
	}
	for (x = 0; x < 160; x++) {
		scope2[x] = 0;
		scope2_old[x] = 0;
	}

	/* LCD Initialization */
	BSP_LCD_Init();

	/* LCD Initialization */
	BSP_LCD_LayerDefaultInit(0, LCD_FB_START_ADDRESS);
	BSP_LCD_LayerDefaultInit(1,
	LCD_FB_START_ADDRESS + (BSP_LCD_GetXSize() * BSP_LCD_GetYSize() * 4));

	/* Enable the LCD */
	BSP_LCD_DisplayOn();

	/* Select the LCD Background Layer  */
	BSP_LCD_SelectLayer(0);

	/* Clear the Background Layer */
	BSP_LCD_Clear(LCD_COLOR_BLACK);

	/* Select the LCD Foreground Layer  */
	BSP_LCD_SelectLayer(1);

	/* Clear the Foreground Layer */
	BSP_LCD_Clear(LCD_COLOR_BLACK);

	/* Configure the transparency for foreground and background :
	 Increase the transparency */
	BSP_LCD_SetTransparency(0, 0);
	BSP_LCD_SetTransparency(1, 0);
}

UINT in_func(JDEC* jd, BYTE* buff, UINT nbyte) {
	int i;
	int read = 0;

	IODEV *dev = (IODEV*) jd->device; // Device identifier for the session (5th argument of jd_prepare function)
	if (buff) {
		for (i = 0; i < nbyte; i++) {
			if (dev->jpg_pos < dev->jpg_size) {
				*buff = dev->jpg_data[dev->jpg_pos];
				buff++;
				dev->jpg_pos++;
				read++;
			}
		}
		return read;
	} else {
		dev->jpg_pos += nbyte;
		if (dev->jpg_pos < dev->jpg_size) {
			return nbyte;
		} else {
			dev->jpg_pos = dev->jpg_size;
			return 0;
		}
	}
}

UINT out_func(JDEC* jd, void* bitmap, JRECT* rect) {
	BYTE *src;
	int i, x, y, bws;
	uint32_t argb;

	src = (BYTE*) bitmap;
	bws = 3 * (rect->right - rect->left + 1);
	for (y = rect->top; y <= rect->bottom; y++) {
		x = rect->left;
		for (i = 0; i < bws; i += 3) {
			argb = 0xFF000000;
			argb |= src[i + 0] << 16;
			argb |= src[i + 1] << 8;
			argb |= src[i + 2] << 0;
			x++;
			BSP_LCD_DrawPixel(x, y, argb);
		}
		src += bws; /* Next line */
	}

	return 1;
}

static void DISPLAY_ShowJPG(uint32_t LayerIndex, const int jpg_length,
		const uint8_t jpg_data[]) {
	IODEV devid;
	BSP_LCD_SelectLayer(LayerIndex);

	devid.jpg_data = (uint8_t*) jpg_data;
	devid.jpg_size = jpg_length;
	devid.jpg_pos = 0;
	res = jd_prepare(&jdec, in_func, work, 3100, &devid);
	if (res == JDR_OK) {
		jd_decomp(&jdec, out_func, 0);
	}
}

static void DISPLAY_ShowHistory(int index) {
	int i;

	history_entry = MEASUREMENT_GetHistory(index);

	DISPLAY_Show_Value_Amps(history_entry.amps);
	DISPLAY_Show_Value_Frq(history_entry.frequency);
	DISPLAY_Show_Value_AmpsMin(history_entry.zoom_min);
	DISPLAY_Show_Value_AmpsMax(history_entry.zoom_max);
	DISPLAY_Show_Value_Duty(history_entry.ratio);

	for (i=0; i<160; i++)
		scope2[i]= history_entry.zoom[i];

	DISPLAY_Show_Scope2();

}

/**
 * @brief  Test touch screen state and modify audio state machine according to that
 * @param  None
 * @retval None
 */
static void DISPLAY_TouchButtons(void) {

	static TS_StateTypeDef TS_State = { 0 };

	if (TS_State.touchDetected == 1) /* If previous touch has not been released, we don't proceed any touch command */
	{
		BSP_TS_GetState(&TS_State);
	} else {
		BSP_TS_GetState(&TS_State);
		if (TS_State.touchDetected == 1) {
			if (screen == background) {
				if ((TS_State.touchX[0] >= TOUCH_NEXT_XMIN)
						&& (TS_State.touchX[0] <= TOUCH_NEXT_XMAX)
						&& (TS_State.touchY[0] >= TOUCH_NEXT_YMIN)
						&& (TS_State.touchY[0] <= TOUCH_NEXT_YMAX)) {
					screen = fade_out_background;
					startup_cnt = 0;
					AUDIO_PlaySound(SOUND_BEEP);
				}
				if ((TS_State.touchX[0] >= TOUCH_BACK_XMIN)
						&& (TS_State.touchX[0] <= TOUCH_BACK_XMAX)
						&& (TS_State.touchY[0] >= TOUCH_BACK_YMIN)
						&& (TS_State.touchY[0] <= TOUCH_BACK_YMAX)) {
					if (MEASUREMENT_GetRun() == STOP) {
						if (cursor < (HISTORY_ENTRIES-1))
							cursor ++;
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
							cursor --;
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
					xxxx = 0;
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
