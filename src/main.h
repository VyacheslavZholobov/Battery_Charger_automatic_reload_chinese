
#define T_IND_OUT 200 // 200ms - время обновления буфера индикатора
uint8_t Time_ind_out = T_IND_OUT; // Переменная для отсчета времени обновления буфера индикатора
uint8_t Time_ind_upd = 0; // Переменная для отсчета времени обновления буфера индикатора
uint16_t count_ms = 0; // for _delay_ms()

int ADC_cur = 0; // Переменная для хранения текущего значения АЦП
int ADC_U_DC = 0; // ADC блока питания
int U_DC = 0; // Напряжение блока питания: 1200 это 12.0V на дисплей выводить / 10
#define COEF_U_DC 1.8311292 // При 18V ADC = 983 coef = 0.18311292  При 12V ADC = 655 coef = 0.1832061069
int ADC_U_OUT = 0; // ADC на аккумуляторе
int U_Out = 0; // Напряжение на аккумуляторе: 1200 это 12.0V на дисплей выводить / 10
int U_Out_disp = 0; // Напряжение на аккумуляторе: 1200 это 12.0V на дисплей выводить / 10
#define COEF_U_OUT 1.8518518519 // При 18V ADC = 975 coef = 0.1846153846154 При 12V ADC = 648 coef = 0.18518518519
int ADC_I = 0; // ADC заряда аккумулятора
int I_out = 0; // Ток заряда аккумулятора: 620 это 6.2A на дисплей выводить / 10
int I_out_disp = 0; // Ток заряда аккумулятора: 620 это 6.2A на дисплей выводить / 10
#define COEF_I 1.0541871921182266009852216748768 // При 4.28A ADC = 406 coef = 1.0541871921182266009852216748768 При 2A ADC = 182 1.0989010989010989010989010989011 coef = 

#define U_AUTOMOBILE_CHARGING 1420
#define U_AGM_GEL 1380
#define U_MOTO 1600 // Этот режим буду использовать для "ПРОЖАРКИ" убитых аккумов: 16V
#define U_PULSE 1550 // Режим заряда пульсацией
#define U_RECHARGING 1220 // Режим хранения. Переход в заряд: - 12.2(50%) - 12.3(60%) - 12.4(75%) - 12.5(80%)
int Ustavka_U = 1420; // Уставка напряжения: 1420 это 14.20V на дисплей выводить / 10
#define I_MAX 620 // Максимальный ток: 620 это 6.2A на дисплей выводить / 10
#define I_MIN 50 // Минимальный ток: 50 это 0.5A на дисплей выводить / 10
int Ustavka_I = I_MAX; // Уставка тока: MAX 620 это 6.2A на дисплей выводить / 10

uint16_t PWM_TM2_CH3 = 250; // PWM value for channel 3


enum MODE { //^ Режим работы устройства
	MODE_AUTO_SELECT, // Автоопределение типа зарядки. AUTOMOBILE_CHARGING или CHARGE_PULSE
	MODE_AUTOMOBILE_CHARGING, // Режим заряда как на автомобиле, до 14.2V
	MODE_CHARGE_AGM_GEL, // Режим заряда AGM/GEL аккумулятора AGM/GEL battery 11.5-13.8V
	MODE_CHARGE_MOTO, // Этот режим буду использовать для "ПРОЖАРКИ" убитых аккумов: 16V
	MODE_CHARGE_PULSE, // Режим заряда пульсацией Pulse charge mode
	MODE_SAFE_KEEPING, // Режим хранения. Переход в заряд: - 12.2(50%) - 12.3(60%) - 12.4(75%) - 12.5(80%) 
	MODE_RESET_MODE // Наклацали больше максимального режима, нужно сбросить на 0 режим
} Mode = MODE_AUTO_SELECT; // Начальный режим работы устройства

enum BATTERY_TYPE { //^ Тип аккумулятора
	BATTERY_Pb, // Свинцовый аккумулятор Lead-acid battery 
	BATTERY_AGM_GEL, // AGM/GEL аккумулятор AGM/GEL battery 11.5-13.8V
	BATTERY_MOTO, // Мото аккумулятор Motorcycle battery 16V
	BATTERY_PULSE, // Пульсация Pulse charge mode
} Battery_type = BATTERY_Pb; // Начальный тип аккумулятора

bool Flag_LED_CAR = FALSE; // 1 светодиод - заряд кислотного аккумулятора
bool Flag_LED_GEL = FALSE; // 2 светодиод - заряд AMG/GEL аккумулятора
bool Flag_LED_MOTO = FALSE; // 3 светодиод - заряд мото аккумулятора
bool Flag_LED_PULSE = FALSE; // 4 светодиод - заряд пульсацией
bool Flag_Fan = FALSE; // Флаг включения вентилятора
bool Flag_ON_Out = FALSE; // Флаг включения выхода

#define TIME_WAIT_ON 5000 // Время ожидания включения (5 секунд)
#define TIME_END_CHARGE 1800000 // Время окончания заряда (30 минут)
uint16_t Count_Change_display = 0; // Счетчик изменения дисплея
uint16_t Count_Wait_ON = TIME_WAIT_ON; // Счетчик ожидания включения
uint16_t Count_Blink = 0; // Счетчик мигания
uint32_t Count_End_charge = 0; // Счетчик окончания заряда


int ADC_NTC = 0;
int Temp = 0; // Температура NTC: 250 это 25.0C