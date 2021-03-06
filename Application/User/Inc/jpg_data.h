/**
 *  Project     WeldingMeter
 *  @file		jpg_data.h
 *  @author		Gerd Bartelt - www.sebulli.com
 *  @brief		header file for jpg_data.c
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
#ifndef __jpg_data_H
#define __jpg_data_H

/* includes  ---------------------------------------------------------*/
#include <stdint.h>

/* global variables -------------------------------------------------*/
extern const uint8_t JPG_INTRO_DATA[];
extern const int JPG_INTRO_LENGTH;

extern const uint8_t JPG_BACKGROUND_DATA[];
extern const int JPG_BACKGROUND_LENGTH;

extern const uint8_t JPG_SETTINGS_DATA[];
extern const int JPG_SETTINGS_LENGTH;

/* Function prototypes -------------------------------------------------*/

#endif // __jpg_data_H
