// * Author: slaventiis (Vyacheslav Zholobov)
//! modified 23.04.25
// #include <Arduino.h>

/* Returns the average value between the current number and the previously averaged number,
using the “moving average” method.
curr - current value.
aver - previously averaged.
step - averaging step, bit shift >> (the larger the number, the smaller the step. No more than 6).
>> Works best when incoming values *10.
*/
int Average_int(int curr, int aver, uint8_t step)
{	// running_average
	int aver_plus;
	aver_plus = (curr - aver) >> step;
	aver += aver_plus;

	if (aver_plus == 0) // Compensation of unsigned numbers calculation error
	{
		if(curr > aver ) { aver += 1; }
		if(curr < aver) { aver -= 1; }
	}
	return aver;
}

/* Returns the average value between the current number and the previously averaged number,
using the “moving average” method.
curr - current value.
aver - previously averaged.
step - averaging step, bit shift >> (the larger the number, the smaller the step. No more than 6).
>> Works best when incoming values are *10.*/
// int Average_int32(int32_t curr, int32_t aver, uint8_t step)
// {
// 	int32_t aver_plus;
// 	aver_plus = (curr - aver) >> step;
// 	aver += aver_plus;

// 	if (aver_plus == 0) // Compensation of unsigned numbers calculation error
// 	{
// 		if(curr > aver ) { aver += 1; }
// 		if(curr < aver) { aver -= 1; }
// 	}
// 	return aver;
// }

/* Returns the average value between the current number and the previously averaged number,
using the “moving average” method.
curr - current value.
aver - previously averaged.
step - averaging step, (the higher the number, the smaller the step. max 64)
>> For example:
>> num_aver = Average_float(num_curr, num_aver, 16); - Averaging by 16 */
// float Average_float(float curr, float aver, uint8_t step)
// {	
// 	aver += (curr - aver) / step;
// 	return aver;
	
// }
