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
