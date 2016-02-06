/**
 *  Project     WeldingMeter
 *  @file		display.c
 *  @author		Gerd Bartelt - www.sebulli.com
 *  @brief		LCD display functions
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

/* Private functions ---------------------------------------------------------*/

void DISPLAY_NewValue(int val);
static void DISPLAY_DrawString(int xpos, int ypos, int size, char* s);
static void DISPLAY_DrawChar(int xpos, int ypos, int size, char c);



/* Private variables ---------------------------------------------------------*/

/* Types  ---------------------------------------------------------*/
typedef struct {
	int jpg_size;
	int jpg_pos;
	uint8_t* jpg_data;
} IODEV;


extern const uint8_t font8x5[96][5];

uint8_t scope1[250];
uint8_t scope1_old[250];
uint8_t scope2[160];
uint8_t scope2_old[160];
MEASUREMENT_tHistory history_entry;

uint8_t work[3100];
JDEC jdec; // Decompression object
JRESULT res; // Result code of TJpgDec API




static void DISPLAY_ShowJPG(uint32_t LayerIndex, const int jpg_length,
		const uint8_t jpg_data[]);

void DISPLAY_ShowIntro(void) {
	BSP_LCD_SetTransparency(0, 255);
	BSP_LCD_SetTransparency(1, 0);
	DISPLAY_ShowJPG(1, JPG_INTRO_LENGTH, JPG_INTRO_DATA);
	BSP_LCD_SetTransparency(0, 0);
}

void DISPLAY_ShowBackground(void) {
	DISPLAY_ShowJPG(0, JPG_BACKGROUND_LENGTH, JPG_BACKGROUND_DATA);
	BSP_LCD_SetTransparency(1, 0);
}
void DISPLAY_ShowSettings(void) {
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

void DISPLAY_DrawBar(int xpos, int ypos, int max, int val) {
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


void DISPLAY_Show_Cursor(int cursor, int autosave_cnt) {
	int x,y;


	for (x = -8; x< (250+8); x++) {
		for (y = 0; y< 8; y++) {
			if ((x >= (250-(cursor * AUTOSAVE_CYCLE + autosave_cnt)-y)) && (x <= (250-(cursor * AUTOSAVE_CYCLE+ autosave_cnt)+y)) && (cursor >= 0)) {
				BSP_LCD_DrawPixel(20 + x, 128 + y, DISPLAY_COLOR_LTBLUE);
			} else {
				BSP_LCD_DrawPixel(20 + x, 128 + y, 0x00000000);
			}
		}
	}
}



void DISPLAY_Show_Version(void) {
	DISPLAY_DrawString(295, 42, 1, "V1.0.0");
}

void DISPLAY_Show_Value_AmpsMax(int amps) {
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
void DISPLAY_Show_Scope1(void) {
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
void DISPLAY_Show_Scope2(void) {
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

void DISPLAY_NewValue(int amps) {
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

	DISPLAY_InitScopes();

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

void DISPLAY_InitScopes(void) {
	int x;
	for (x = 0; x < 250; x++) {
		scope1[x] = 0;
		scope1_old[x] = 0;
	}
	for (x = 0; x < 160; x++) {
		scope2[x] = 0;
		scope2_old[x] = 0;
	}
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

void DISPLAY_ShowHistory(int index) {
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

