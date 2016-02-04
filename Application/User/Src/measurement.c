/* Includes ------------------------------------------------------------------*/
#include "measurement.h"

/* Private functions ---------------------------------------------------------*/
static int MEASUREMENT_ScaleAmps(int rawval);

/* Private variables ---------------------------------------------------------*/
uint32_t prefilt = 0;
uint32_t prefilt_L = 0;
uint32_t dc_offset = 0;
uint32_t dc_offset_L = 0;
uint32_t slowfilt = 0;
uint32_t slowfilt_L = 0;
uint32_t zoomfilt = 0;
uint32_t zoomfilt_L = 0;
int zoomfilt_shift = 0;
int zoom_n = 1;
int zoom_n_cnt = 0;

int zoom_min = 4096;
int zoom_max = 0;
int zoom_min_hold = SENSOR_OFFSET;
int zoom_max_hold = SENSOR_OFFSET;
int zoom[160];
int trigger_pos = 0;
int zoom_cnt = 0;
enTRIGGER trigger = TRG_PREPARE;
int triggercnt = 0;
int sensor = 0;
int trigger_prepare_cnt = 0;

int sign = 0;
int sign_old = 0;
int pos_edge = 0;
int neg_edge = 0;
int pos_edge_cnt = 0;
int period_cnt = 0;
int period = 0;
int pulse = 0;
int sensor_offset = 2068;

void MEASUREMENT_Init(void) {
}

inline void MEASUREMENT_NewSample(uint16_t value) {

	// Filter the stream with t= 80µs for edge detection (frequency calculation)
	prefilt_L += value - prefilt;
	prefilt = prefilt_L / T_PREFILT;

	// Filter the stream with t= 655ms for DC offset
	dc_offset_L += prefilt - dc_offset;
	dc_offset = dc_offset_L / T_DC_OFFSET;

	// Filter the stream with t= 163ms for 1st scope screen
	slowfilt_L += prefilt - slowfilt;
	slowfilt = slowfilt_L / T_SLOWFILT;

	// Filter the zoomed samples
	zoomfilt_L += prefilt - zoomfilt;
	zoomfilt = zoomfilt_L >> zoomfilt_shift;

	// use every nth sample for zoom
	zoom_n_cnt++;
	if (zoom_n_cnt >= zoom_n) {
		zoom_n_cnt = 0;
		if (trigger != TRG_COMPLETE) {

			if (trigger_prepare_cnt <= 32) {
				trigger_prepare_cnt ++;
			} else if (trigger == TRG_PREPARE) {
				trigger = TRG_READY;
				zoom_min = 4096;
				zoom_max = 0;
			}

			zoom_cnt++;
			if (zoom_cnt >= 160) {
				zoom_cnt = 0;
			}
			zoom[zoom_cnt] = zoomfilt;
			if (zoomfilt < zoom_min)
				zoom_min = zoomfilt;
			if (zoomfilt > zoom_max)
				zoom_max = zoomfilt;

			if (trigger == TRG_TRIGGERED) {
				if (triggercnt > 0) {
					triggercnt--;
				} else {
					trigger = TRG_COMPLETE;
				}
			}
		}
	}


	// edge detection
	if (prefilt > (dc_offset+THRESHOLD))
		sign = 1;
	else if (prefilt < (dc_offset-THRESHOLD))
		sign = -1;
	pos_edge = ((sign==1) && (sign_old == -1));
	neg_edge = ((sign==-1) && (sign_old == 1));
	sign_old = sign;

	// Suppress a positive edge for 1ms (fmax = 1kHz)
	if (pos_edge_cnt < (POS_EDGE_TIMEOUT-(POS_EDGE_TIMEOUT/10))) {
		if (neg_edge) {
			pulse = period_cnt;
		}
	}

	// Suppress a positive edge for 1ms (fmax = 1kHz)
	if (pos_edge_cnt > 0) {
		pos_edge_cnt --;
	} else if (pos_edge) {
		pos_edge_cnt = POS_EDGE_TIMEOUT;
		period = period_cnt;
		period_cnt = 0;

		// Trigger (edge detection)
		if (trigger == TRG_READY) {
			// 128 samples after the trigger + 32 samples pre-trigger = 160
			triggercnt = 128;
			trigger = TRG_TRIGGERED;
			//Keep the trigger position
			trigger_pos = zoom_cnt;
		}
	}

	// Count the period ( measure the frequency)
	if (period_cnt < 1000000)
		period_cnt ++;

}

int MEASUREMENT_GetFrequency(void) {
	if (period == 0)
		return 0;
	return SAMPLING_FRQ / period;
}
int MEASUREMENT_GetRatio(void) {
	if (period == 0)
		return 100;
	return ((pulse * 100) / period);
}

/*
 * Scales the ADC values to amps
 *
 * The resistor divider 500R and 1k, or 3:2, so 5V -> 3.333V
 * Vref = 3.3V
 * ADCmax = 4096 = 4,95V
 *
 * Sensor is LEM  HTFS 200-P (Farnell 9135715)
 * SensorOffset = 2.5V = 2068 LSB
 * Vout_Sensor = Vref ± (1.25V・I / 200A )
 * Vout_Sensor = Vref ± 6.25mR * I
 * 1A = 5.1717... LSB
 * I(A) = ADC * 0,193359375 = ADC * 99 / 512
 *
 * @param rawval
 * 		ADC raw value
 * @return the valus in amps
 */
static int MEASUREMENT_ScaleAmps(int rawval) {
	return (((rawval-sensor_offset)*99)/512);
}

int MEASUREMENT_GetSlowFilt(void) {
	return MEASUREMENT_ScaleAmps(slowfilt);
}

int MEASUREMENT_GetMin(void) {
	return MEASUREMENT_ScaleAmps(zoom_min_hold);
}
int MEASUREMENT_GetMax(void) {
	return MEASUREMENT_ScaleAmps(zoom_max_hold);
}


void MEASUREMENT_CopyZoomField(uint8_t target[]) {
	int i,ii,iii;
	int scale, offset;
	int val;
	offset = (zoom_min + zoom_max) / 2;
	scale = (zoom_max - zoom_min);

	zoom_min_hold = zoom_min;
	zoom_max_hold = zoom_max;

	// 10A min
	if (scale <= 51)
		scale = 51;

	// Scale it to 80px
	scale /= 80;

	// calculate 1/x and scale it with 65536
	scale = 65536 / scale;

	if (trigger == TRG_COMPLETE) {
		ii = trigger_pos;
		iii = 32;
		for (i=0; i<160; i++) {
			val = (zoom[ii] - offset) * scale / 65536;

			val += 50;
			// Limit to 0..100
			if (val < 0)
				val = 0;
			if (val > 100)
				val = 100;

			// Copy
			target[iii] = (uint8_t)(val);


			ii++;
			if (ii >= 160) {
				ii = 0;
			}
			iii++;
			if (iii >= 160) {
				iii = 0;
			}
		}
		zoom_cnt = 0;
		trigger_prepare_cnt = 0;
		trigger = TRG_PREPARE;

		zoom_n = period / 50;
	}

}


