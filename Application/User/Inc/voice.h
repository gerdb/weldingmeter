/**
 *  Project     WeldingMeter
 *  @file		voice.h
 *  @author		Gerd Bartelt - www.sebulli.com
 *  @brief		Header file for voice.c
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
#ifndef __voice_H
#define __voice_H

/* includes  ---------------------------------------------------------*/
#include <stdint.h>


/* global variables -------------------------------------------------*/

/* Function prototypes -------------------------------------------------*/
void VOICE_Say (int number);

#endif // __voice_H
