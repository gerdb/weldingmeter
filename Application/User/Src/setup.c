/**
 *  Project     WeldingMeter
 *  @file		setup.c
 *  @author		Gerd Bartelt - www.sebulli.com
 *  @brief		Save and recall the settings
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
#include "eeprom.h"
#include "setup.h"
#include "stm32746g_discovery.h"

/* Private defines ---------------------------------------------------------*/
#define ADDR_VOLUME 	0x0000
#define ADDR_ACDC   	0x0001
#define ADDR_MEAN_PEAK	0x0002
#define ADDR_LANGUAGE	0x0003

/* Private functions ---------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
/* Virtual address defined by the user: 0xFFFF value is prohibited */
uint16_t VirtAddVarTab[NB_OF_VAR] = {ADDR_VOLUME, ADDR_ACDC, ADDR_MEAN_PEAK, ADDR_LANGUAGE};
uint16_t VarDataTab[NB_OF_VAR] = {0, 0, 0, 0};
uint16_t VarValue, VarDataTmp = 0;

volatile int eeprom_err = 0;

/**
 * Inistialize the module and reads the settings from the EEProm
 */
void SETUP_Init(void) {
	/* Unlock the Flash Program Erase controller */
	HAL_FLASH_Unlock();
	/* EEPROM Init */
	if (EE_Init() != EE_OK) {
		eeprom_err = 1;
	}

	EE_ReadVariable(VirtAddVarTab[ADDR_VOLUME],		&VarDataTab[ADDR_VOLUME]);
	EE_ReadVariable(VirtAddVarTab[ADDR_ACDC],		&VarDataTab[ADDR_ACDC]);
	EE_ReadVariable(VirtAddVarTab[ADDR_MEAN_PEAK],	&VarDataTab[ADDR_MEAN_PEAK]);
	EE_ReadVariable(VirtAddVarTab[ADDR_LANGUAGE],	&VarDataTab[ADDR_LANGUAGE]);
}

/**
 * Sets the volume
 *
 * @param vol
 * 	Volume value
 */
void SETUP_SetVolume(int vol) {
	VarDataTab[ADDR_VOLUME] = vol;
}

/**
 * Gets the volume
 *
 * @return the volum value
 */
int SETUP_GetVolume(void) {
	if (VarDataTab[ADDR_VOLUME] <= 10)
		return VarDataTab[ADDR_VOLUME];
	else
		return 5;
}

/**
 * Sets the AC/DC setting
 *
 * @param acdc
 * 	AC/DC setting value
 */
void SETUP_SetACDC(int acdc) {
	VarDataTab[ADDR_ACDC] = acdc;
}

/**
 * Gets the AC/DC setting
 *
 * @return the AC/DC setting
 */
int SETUP_GetACDC(void) {
	if (VarDataTab[ADDR_ACDC] <= SETUP_AC)
		return VarDataTab[ADDR_ACDC];
	else
		return SETUP_DC;
}


/**
 * Sets the mean/peak setting
 *
 * @param meanpeak
 * 	mean/peak setting
 */
void SETUP_SetMEAN_PEAK(int meanpeak) {
	VarDataTab[ADDR_MEAN_PEAK] = meanpeak;
}

/**
 * Gets the mean/peak setting
 *
 * @return the mean/peak setting
 */
int SETUP_GetMEAN_PEAK(void) {
	if (VarDataTab[ADDR_MEAN_PEAK] <= SETUP_PEAK)
		return VarDataTab[ADDR_MEAN_PEAK];
	else
		return SETUP_MEAN;
}

/**
 * Sets the language
 *
 * @param language
 * 	language setting
 */
void SETUP_SetLanguage(int language) {
	VarDataTab[ADDR_LANGUAGE] = language;
}

/**
 * Gets the language
 *
 * @return the language
 */
int SETUP_GetLanguage(void) {
	if (VarDataTab[ADDR_LANGUAGE] <= SETUP_DA)
		return VarDataTab[ADDR_LANGUAGE];
	else
		return SETUP_DE;
}

/**
 * Store the settings into the EEProm
 */
void SETUP_Save(void) {
	EE_WriteVariable(VirtAddVarTab[ADDR_VOLUME],	VarDataTab[ADDR_VOLUME]);
	EE_WriteVariable(VirtAddVarTab[ADDR_ACDC],    	VarDataTab[ADDR_ACDC]);
	EE_WriteVariable(VirtAddVarTab[ADDR_MEAN_PEAK],	VarDataTab[ADDR_MEAN_PEAK]);
	EE_WriteVariable(VirtAddVarTab[ADDR_LANGUAGE],	VarDataTab[ADDR_LANGUAGE]);
}
