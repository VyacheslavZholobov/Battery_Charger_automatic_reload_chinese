// * Created: 28.11.2021 0:45:50
// #include <Arduino.h>
// #include <avr/pgmspace.h>
// #include <avr/delay.h>

#include "stm8s.h"

// Disclose which sensors we work with and what connection scheme we use
#define ARDUINO_10K_3950_RA10
// #define ARDUINO_10K_3950_RA5
// #define EPCOS_B57164K0103J001
// #define NTC_3950_100k_GLASS_DROP_RA10 // Glass drop for 3D printer extruder

#if defined(ARDUINO_10K_3950_RA10)
/* Table of ADC total value (*16) as a function of temperature. From the higher value to the lower one
   The following parameters are used to build the table:
     R1(T1): 10kOhm(25°C) temp-40 to +150, step 5
     Table of R/T characteristics: NTC 10k 3950 Arduino, values taken from: 103_3950_lookuptable arduino.pdf
     Wiring diagram: Ra(+) Rt(gnd) midpoint to controller
     Ra: 10kOhm
*/
const uint16_t table_arduino_10k_3950_Ra10[] = {
	15808,15648,15408,15088,14704,14256,13728,13136,12464,11696,
	10864,9984,9088,8192,7312,6480,5696,4976,4336,3760,
	3248,2800,2416,2096,1808,1568,1360,1184,1040,912,
	800,688,608,528,464,416,368,320,288	// 39 items (including 0)
};
#endif // ARDUINO_10K_3950_RA10

/* 
 * The function calculates the temperature value in tenths of degrees Celsius
 * depending on the total(*16) ADC value. The obtained result
 * convert to float, divide by 10 and get a number with tenths.
 * ---
 * adcsum - sum of 16 ADC values,
 * *table - array of ADC values (16x) of thermistor at different temperatures,
 * size - size of the table array in bytes.
 * tmp_under - lower temperature
 * tmp_over - upper temperature
 * table_step - temperature step in the array
 * ---
 * In case of sensor failure: will return -7777.
 * In case of sensor failure: will return 7777
 */
int16_t calc_temperature (uint16_t  adcsum, const uint16_t *table, uint8_t size, int16_t tmp_under, int16_t tmp_over, int8_t table_step) {
	uint8_t l = 0;
	#define NTC_OVER	7777
	#define NTC_UNDER	-7777
	uint8_t r = (size / sizeof(&table[0])) - 1;
	uint16_t thigh = table[r];
	
	if (adcsum < thigh)	return NTC_OVER;
	else if(adcsum == thigh) return tmp_over;

	uint16_t  tlow = table[0];

	if (adcsum > tlow)	return NTC_UNDER;
	else if (adcsum >= tlow) return tmp_under;

	while ((r - l) > 1) {
		uint8_t  m = (l + r) >> 1;
		uint16_t  mid = table[m];
		if (adcsum > mid)	r = m; 
		else l = m;
	}		
	uint16_t  vl = table[l];	
	if (adcsum >= vl) return l * table_step + tmp_under;

	uint16_t  vr = table[r];	
	uint16_t  vd = vl - vr;								
	int32_t temp = tmp_under + r * table_step;	
	
	if (vd) {
		temp -= ((table_step * (int32_t)(adcsum - vr) + (vd >> 1)) / vd);
	}
	return temp;
}