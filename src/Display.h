void Display(void)
{
	#ifdef DEBUG
		IND_Output(I_out/10, 1, 1, 3, 0);
	#else
		#ifdef RUNNING_STR
			if(Mode == MODE_AUTO_SELECT) 
			{
				speed_shift = 0;
				// IND_Output_symbol(symbol_U, 14);
		 		IND_Output(U_Out/10, 1, 1, 3, 0);
				//  IND_Output_symbol(symbol_EMPTY, 10);
				// IND_Output_symbol(symbol_A, 9);
				// IND_Output(I_out_disp/10, 1, 6, 3, 0);
				// IND_Output_symbol(symbol_EMPTY, 5);
				// IND_Output_symbol(symbol_t, 4);
				// IND_Output(Temp, 1, 1, 3, 0);
			}
			else if(Mode == MODE_SAFE_KEEPING) // Show Voltage for 1sec, turn off the indicator for 5sec
			{
				speed_shift = 0;
				if(Count_Change_display < 1000) {IND_Output(U_Out/10, 1, 1, 3, 0);}
				else if(Count_Change_display < 6000) {IND_Output_symbol(symbol_EMPTY, 3);IND_Output_symbol(symbol_EMPTY, 2);IND_Output_symbol(symbol_EMPTY, 1);}
				else {Count_Change_display = 0;}
			}
			else
			{
				speed_shift = 70;
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
			else {Count_Change_display = 0;}
		}
		else
		{
			if(Count_Change_display < 300) {IND_Output_symbol(symbol_U, 3);IND_Output_symbol(symbol_EMPTY, 2);IND_Output_symbol(symbol_EMPTY, 1);}
			else if(Count_Change_display < 1500) {IND_Output(U_Out_disp/10, 1, 1, 3, 1);}
			else if(Count_Change_display < 1800) {IND_Output_symbol(symbol_A, 3);IND_Output_symbol(symbol_EMPTY, 2);IND_Output_symbol(symbol_EMPTY, 1);}
			else if(Count_Change_display < 3000) {IND_Output(I_out_disp/10, 1, 1, 3, 1);}
			else if(Count_Change_display < 3300) {IND_Output_symbol(symbol_t, 3);IND_Output_symbol(symbol_EMPTY, 2);IND_Output_symbol(symbol_EMPTY, 1);}
			else if(Count_Change_display < 4500) {IND_Output(Temp, 1, 1, 3, 1);}
			else {Count_Change_display = 0;}
		}
		#endif // RUNNING_STR
	#endif // DEBUG
}

//* *****---******
//* U14.2 A6.2 t25.0
/*
Create an array of all characters, spaces and digits = 13 pcs.
Make a function that writes the first letter and 3 digits in the array in the required cells. (e.g. U14.2 in cells 0-3)
So write all letters and numbers in the array.
Create a function that will display three cells (0-3), with each call to the function (after 100ms)
output (1-4) cells, (2-5), (3-6), (4-7), (5-8), (6-9), (7-10), (8-11), (9-12), (10-13).
 */