#ifndef __measurement_H
#define __measurement_H

/* includes  ---------------------------------------------------------*/
#include <stdint.h>

#define SAMPLING_FRQ 100000 // ADC sampling frequency
#define SAMPLING_TIME 10 // ADC sampling time in µs

#define T_PREFILT 8 // Filter time of the prefilter in 10µs steps
#define T_DC_OFFSET 65536 // Filter time of the DC offset filter in 10µs steps
#define T_SLOWFILT 16384 // Filter time of the slow filter for 1st scope scrren in 10µs steps
#define POS_EDGE_TIMEOUT 100 // in 10µs steps. So 100 = 1000µs -> fmax = 1kHz
#define THRESHOLD 20 // Threshold in ADC steps. 20 = +- 2A

#define SENSOR_ENTRIES 3

/* Global variables -------------------------------------------------*/

/* Function prototypes -------------------------------------------------*/
void MEASUREMENT_Init(void);
inline void MEASUREMENT_NewSample(uint16_t value);
int MEASUREMENT_GetFrequency(void);
int MEASUREMENT_GetSlowFilt(void);
void MEASUREMENT_CopyZoomField(uint8_t target[]);

#endif // __measurement_H
