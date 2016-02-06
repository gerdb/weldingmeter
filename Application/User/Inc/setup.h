/**
 *  Project     WeldingMeter
 *  @file		setup.h
 *  @author		Gerd Bartelt - www.sebulli.com
 *  @brief		header file for setup.c
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
#ifndef __setup_H
#define __setup_H

/* includes  ---------------------------------------------------------*/
#include <stdint.h>

/* defines  ---------------------------------------------------------*/
#define SETUP_DC 0
#define SETUP_AC 1

#define SETUP_MEAN 0
#define SETUP_PEAK 1

#define SETUP_DE 0
#define SETUP_DA 1

/* Function prototypes -------------------------------------------------*/

void SETUP_Init(void);
void SETUP_SetVolume(int vol);
int SETUP_GetVolume(void);
void SETUP_SetACDC(int acdc);
int SETUP_GetACDC(void);
int SETUP_GetMEAN_PEAK(void);
void SETUP_SetMEAN_PEAK(int acdc);
void SETUP_SetLanguage(int language);
int SETUP_GetLanguage(void);
void SETUP_Save(void);

#endif // __setup_H
