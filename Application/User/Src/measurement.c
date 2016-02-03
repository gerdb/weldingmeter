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

int zoom[160];
int zoom_cnt = 0;
int sensor = 0;


int sign = 0;
int sign_old = 0;
int pos_edge = 0;
int pos_edge_cnt = 0;
int period_cnt = 0;
int period = 0;

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
		if (zoom_cnt < 160) {
			zoom[zoom_cnt] = zoomfilt;
			zoom_cnt++;
		}
	}


	// edge detection
	if (prefilt > (dc_offset+THRESHOLD))
		sign = 1;
	else if (prefilt < (dc_offset-THRESHOLD))
		sign = -1;
	pos_edge = ((sign==1) && (sign_old == -1));
	sign_old = sign;

	// Suppress a positive edge for 1ms (fmax = 1kHz)
	if (pos_edge_cnt > 0) {
		pos_edge_cnt --;
	} else if (pos_edge) {
		pos_edge_cnt = POS_EDGE_TIMEOUT;
		period = period_cnt;
		period_cnt = 0;
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

static int MEASUREMENT_ScaleAmps(int rawval) {
	return (rawval);
}

int MEASUREMENT_GetSlowFilt(void) {
	return MEASUREMENT_ScaleAmps(slowfilt);
}

void MEASUREMENT_CopyZoomField(uint8_t target[]) {
	int i;
	if (zoom_cnt == 160) {
		for (i=0; i<160; i++) {
			target[i] = (uint8_t) (zoom[i] /32);
		}
		zoom_cnt = 0;
	}

}


