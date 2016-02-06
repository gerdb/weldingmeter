/**
 *  Project     WeldingMeter
 *  @file		display.h
 *  @author		Gerd Bartelt - www.sebulli.com
 *  @brief		header file for display.c
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
#ifndef __display_H
#define __display_H

/* includes  ---------------------------------------------------------*/
#include <stdint.h>

#define DISPLAY_COLOR_LTBLUE 0xFF019cff

extern uint8_t scope2[];

/* Function prototypes -------------------------------------------------*/
void DISPLAY_Init(void);
void DISPLAY_ShowIntro(void);
void DISPLAY_ShowBackground(void);
void DISPLAY_ShowSettings(void);
void DISPLAY_Show_Value_Amps(int amps);
void DISPLAY_Show_Value_Frq(int frq);
void DISPLAY_Show_Value_Duty(int duty);
void DISPLAY_Show_Value_AmpsMin(int amps);
void DISPLAY_Show_Value_AmpsMax(int amps);
void DISPLAY_Show_Scope1(void);
void DISPLAY_Show_Scope2(void);
void DISPLAY_DrawBar(int xpos, int ypos, int max, int val);
void DISPLAY_Show_Cursor(int cursor, int autosave_cnt);
void DISPLAY_ShowHistory(int index);
void DISPLAY_NewValue(int val);
void DISPLAY_InitScopes(void);
void DISPLAY_Show_Version(void);

#endif // __display_H
