---
Пример использования:
// В NTC_sls.h раскоментировать те дефайны с какими датчиками работаем и по какой схемме подключения
// подключаем библиотеки
#include "../lib/ADC_AVR/ADC_AVR_sls.h"
#include "../lib/NTC_sls/NTC_sls.h"

// Объявим глобальные переменные
i16 ADC_current; // Этой можно пользоваться для всех измерений ацп как временной.
i16 Temp_NTC_ARD;

void setup() {

}

void loop() {
  //____________________________________________________ Temperature NTC
  ADC_current = ADC_read(PIN_NTC_ARD, 16);
  Temp_NTC_ARD = calc_temperature(ADC_current, table_arduino_10k_3950_Ra10,sizeof(table_arduino_10k_3950_Ra10), -400, 1500, 50);
  lcd.print((float)Temp_NTC_ARD / 10, 1); // Приведем к float, разделим на 10 и получим температуру с десятыми градуса.
  Serial.println((float)Temp_NTC_ARD / 10, 1); // Приведем к float, разделим на 10 и получим температуру с десятыми градуса.
}