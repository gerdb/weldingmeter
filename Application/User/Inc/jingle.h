/**
 *  Project     WeldingMeter
 *  @file		jingle.h
 *  @author		Gerd Bartelt - www.sebulli.com
 *  @brief		header file for jingle.c
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
#ifndef __jingle_H
#define __jingle_H

/* includes  ---------------------------------------------------------*/
#include <stdint.h>

/* global variables -------------------------------------------------*/
extern const uint8_t AUDIO_PING_DATA[];
extern int AUDIO_PING_LENGTH;

extern const uint8_t AUDIO_SPARK_DATA[];
extern int AUDIO_SPARK_LENGTH;

extern const uint8_t AUDIO_BEEP_DATA[];
extern int AUDIO_BEEP_LENGTH;

/* Function prototypes -------------------------------------------------*/

#endif // __jingle_H
