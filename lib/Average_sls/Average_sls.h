//! изменено 23.04.25
// #include <Arduino.h>

/* Возвращает усредненное значение между текущим и усредненным ранее числом,
методом "скользящее среднее".
curr - текущее значение.
aver - усредненное ранее.
step - шаг усреднения, битовый сдвиг >> (чем больше число, тем меньше шаг. Не более 6)
>> Работает лучше всего когда входящие значения *10. 
>> Например:
>> num_curr = ADC_read(0, 10); // 10 раз считываем 0 пин ацп 
>> num_aver = Average_int(num_curr, num_aver, 4); - Усредняем на 2^4 (16)
>> Serial.println(num_aver / 10, DEC); // Выводим усредненное значение ацп */
int Average_int(int curr, int aver, uint8_t step)
{	// running_verage - скользящее среднее
	int aver_plus;
	aver_plus = (curr - aver) >> step;
	aver += aver_plus;

	if (aver_plus == 0) // компенсация ошибки вычислений беззнаковых чисел
	{
		if(curr > aver ) { aver += 1; }
		if(curr < aver) { aver -= 1; }
	}
	return aver;
	/* Занимает 38 bytes Flash */
}

/* Возвращает усредненное значение между текущим и усредненным ранее числом,
методом "скользящее среднее".
curr - текущее значение.
aver - усредненное ранее.
step - шаг усреднения, битовый сдвиг >> (чем больше число, тем меньше шаг. Не более 6)
>> Работает лучше всего когда входящие значения *10. 
>> Например:
>> num_curr = ADC_read(0, 10); // 10 раз считываем 0 пин ацп 
>> num_aver = Average_int32(num_curr, num_aver, 4); - Усредняем на 2^4 (16)
>> Serial.println(num_aver / 10, DEC); // Выводим усредненное значение ацп */
// int Average_int32(int32_t curr, int32_t aver, uint8_t step)
// {
// 	int32_t aver_plus;
// 	aver_plus = (curr - aver) >> step;
// 	aver += aver_plus;

// 	if (aver_plus == 0) // компенсация ошибки вычислений беззнаковых чисел
// 	{
// 		if(curr > aver ) { aver += 1; }
// 		if(curr < aver) { aver -= 1; }
// 	}
// 	return aver;
// }

/* Возвращает усредненное значение между текущим и усредненным ранее числом,
методом "скользящее среднее".
curr - текущее значение.
aver - усредненное ранее.
step - шаг усреднения, (чем больше число, тем меньше шаг. Не более 64)
>> Например:
>> num_aver = Average_float(num_curr, num_aver, 16); - Усредняем на 16 */
// float Average_float(float curr, float aver, uint8_t step)
// {	
// 	aver += (curr - aver) / step;
// 	return aver;
	
// }
