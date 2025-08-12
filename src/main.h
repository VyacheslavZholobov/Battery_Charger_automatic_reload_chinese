
#define T_IND_OUT 200 // ms - indicator buffer refresh time
uint8_t Time_ind_out = T_IND_OUT; // to count down the indicator buffer refresh time
uint8_t Time_ind_upd = 0; // to count down the indicator buffer refresh time
uint16_t count_ms = 0; // for _delay_ms()

int ADC_cur = 0;
int ADC_U_DC = 0;
int U_DC = 0; // display / 10
#define COEF_U_DC 1.8311292 
int ADC_U_OUT = 0;
int U_Out = 0; // 100 is 1.0 to display / 10
int U_Out_disp = 0; // 100 is 1.0 to display / 10
#define COEF_U_OUT 1.8518518519
int ADC_I = 0; 
int I_out = 0; // 100 is 1.0 to display / 10
int I_out_disp = 0; // 100 is 1.0 to display / 10
#define COEF_I 1.0541871921182266009852216748768
#define U_AUTOMOBILE_CHARGING 1420
#define U_AGM_GEL 1380
#define U_MOTO 1600 
#define U_PULSE 1550
#define U_RECHARGING 1220 //  12.2(50%) - 12.3(60%) - 12.4(75%) - 12.5(80%)
int Ustavka_U = 1420; // 100 is 1.0 to display / 10
#define I_MAX 620 // 100 is 1.0 to display / 10
#define I_MIN 50 // 100 is 1.0 to display / 10
int Ustavka_I = I_MAX; // 100 is 1.0 to display / 10

uint16_t PWM_TM2_CH3 = 250; // PWM value for channel 3


enum MODE { //^ Device operating mode
	MODE_AUTO_SELECT,
	MODE_AUTOMOBILE_CHARGING,
	MODE_CHARGE_AGM_GEL,
	MODE_CHARGE_MOTO,
	MODE_CHARGE_PULSE,
	MODE_SAFE_KEEPING,
	MODE_RESET_MODE
} Mode = MODE_AUTO_SELECT;

enum BATTERY_TYPE { //^ Battery type
	BATTERY_Pb,
	BATTERY_AGM_GEL,
	BATTERY_MOTO,
	BATTERY_PULSE,
} Battery_type = BATTERY_Pb;

bool Flag_LED_CAR = FALSE; // 1 LED - charging the acid battery
bool Flag_LED_GEL = FALSE; // 2 LED - AMG/GEL battery charge
bool Flag_LED_MOTO = FALSE; // 3 LED - motorcycle battery charge
bool Flag_LED_PULSE = FALSE; // 4 LED - charge by pulsation
bool Flag_Fan = FALSE;
bool Flag_ON_Out = FALSE;

#define TIME_WAIT_ON 5000 // 5 seconds
#define TIME_END_CHARGE 1800000 // 30 minutes
uint16_t Count_Change_display = 0;
uint16_t Count_Wait_ON = TIME_WAIT_ON;
uint16_t Count_Blink = 0;
uint32_t Count_End_charge = 0;


int ADC_NTC = 0;
int Temp = 0; // Temperature NTC: 250 this 25.0C