



void Display(void)
{
	#ifdef DEBUG
		IND_Output(I_out/10, 1, 1, 3, 0);
	#else
		#ifdef RUNNING_STR
			if(Mode == MODE_AUTO_SELECT) 
			{
				speed_shift = 0; //^ Без бегущей строки
				// IND_Output_symbol(symbol_U, 14);
		 		IND_Output(U_Out/10, 1, 1, 3, 0);
				//  IND_Output_symbol(symbol_EMPTY, 10);
				// IND_Output_symbol(symbol_A, 9);
				// IND_Output(I_out_disp/10, 1, 6, 3, 0);
				// IND_Output_symbol(symbol_EMPTY, 5);
				// IND_Output_symbol(symbol_t, 4);
				// IND_Output(Temp, 1, 1, 3, 0);
			}
			else if(Mode == MODE_SAFE_KEEPING) // Покажем Напряжение на 1сек, погасим индикатор на 5сек
			{
				speed_shift = 0; //^ Без бегущей строки
				if(Count_Change_display < 1000) {IND_Output(U_Out/10, 1, 1, 3, 0);}
				else if(Count_Change_display < 6000) {IND_Output_symbol(symbol_EMPTY, 3);IND_Output_symbol(symbol_EMPTY, 2);IND_Output_symbol(symbol_EMPTY, 1);}
				else {Count_Change_display = 0;} // Сбрасываем счетчик изменения дисплея
			}
			else //^ Все остальные режимы
			{
				speed_shift = 70; //^ Без бегущей строки
				IND_Output_symbol(symbol_U, 14);
		 		IND_Output(U_Out/10, 1, 11, 3, 0);
				 IND_Output_symbol(symbol_EMPTY, 10);
				IND_Output_symbol(symbol_A, 9);
				IND_Output(I_out_disp/10, 1, 6, 3, 0);
				IND_Output_symbol(symbol_EMPTY, 5);
				IND_Output_symbol(symbol_t, 4);
				IND_Output(Temp, 1, 1, 3, 0);
			}
		#else
		if(Mode == MODE_AUTO_SELECT) 
		{IND_Output(U_Out/10, 1, 1, 3, 1);}
		else if(Mode == MODE_SAFE_KEEPING) 
		{
			if(Count_Change_display < 1000) {IND_Output(U_Out/10, 1, 1, 3, 1);}
			else if(Count_Change_display < 6000) {IND_Output_symbol(symbol_EMPTY, 3);IND_Output_symbol(symbol_EMPTY, 2);IND_Output_symbol(symbol_EMPTY, 1);}
			else {Count_Change_display = 0;} // Сбрасываем счетчик изменения дисплея
		}
		else //^ Все остальные режимы
		{
			if(Count_Change_display < 300) {IND_Output_symbol(symbol_U, 3);IND_Output_symbol(symbol_EMPTY, 2);IND_Output_symbol(symbol_EMPTY, 1);}
			else if(Count_Change_display < 1500) {IND_Output(U_Out_disp/10, 1, 1, 3, 1);}
			else if(Count_Change_display < 1800) {IND_Output_symbol(symbol_A, 3);IND_Output_symbol(symbol_EMPTY, 2);IND_Output_symbol(symbol_EMPTY, 1);}
			else if(Count_Change_display < 3000) {IND_Output(I_out_disp/10, 1, 1, 3, 1);}
			else if(Count_Change_display < 3300) {IND_Output_symbol(symbol_t, 3);IND_Output_symbol(symbol_EMPTY, 2);IND_Output_symbol(symbol_EMPTY, 1);}
			else if(Count_Change_display < 4500) {IND_Output(Temp, 1, 1, 3, 1);}
			else {Count_Change_display = 0;} // Сбрасываем счетчик изменения дисплея
		}
		#endif // RUNNING_STR
	#endif // DEBUG
}

//* *****---******
//* U14.2 A6.2 t25.0
/*
Создать массив равный кол. всех символов,пробелов и цифр = 13шт.
сделать функцию которая записывает первую букву и 3 цифры в массив в нужные ячейки. (например U14.2 в ячейки 0-3)
так записать все буквы и числа в массив.
Создать функцию которая будет выводить на дисплей по три ячейки (0-3), с каждым вызовом функции (через 100мс) 
выводить (1-4) ячейки, (2-5), (3-6), (4-7), (5-8), (6-9), (7-10), (8-11), (9-12), (10-13).
 */