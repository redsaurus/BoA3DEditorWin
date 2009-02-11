#include "stdafx.h"
/*
#include <Windows.h>
#include <stdio.h>
#include "math.h"
#include "stdlib.h"
*/
#include <ctype.h>

#include "global.h"

// Gloabl varialbes

short current_cursor = 0;

// external gloabal variables

extern Boolean dialog_not_toast;
extern short cur_viewing_mode;
extern HINSTANCE store_hInstance;
extern short dialog_answer;

// local variables

short store_which_string_dlog;
short which_page ;
short store_res_list;
short store_first_t ;
short store_last_t ;
short store_cur_t ;
// short num_specs[3] = {256,60,100};
Boolean store_strict_string;

// q_3DModStart
HCURSOR cursors[10] = {NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL};
// q_3DModEnd


// function prototype

void display_strings(char *text1, char *text2, char *title, /* short sound_num, short graphic_num, */ short parent_num);
void put_text_res();
// short choice_dialog(short pic,short num);



	
void fancy_choice_dialog_event_filter (short item_hit)
{
	dialog_not_toast = FALSE;
	dialog_answer = item_hit;
}

short fancy_choice_dialog(short which_dlog,short parent)
// ignore parent in Mac version
{
	short i,store_dialog_answer;
	
	store_dialog_answer = dialog_answer;
	//make_cursor_sword();
	
	cd_create_dialog_parent_num(which_dlog,parent);
	

//#ifndef EXILE_BIG_GUNS
//	while (dialog_not_toast)
//		ModalDialog((ModalFilterProcPtr) cd_event_filter, &item_hit);
//#endif		
	while (dialog_not_toast)
		ModalDialog();	

	cd_kill_dialog(which_dlog,0);


	i = dialog_answer;
	dialog_answer = store_dialog_answer;
	
	return i;
}



//cre = check range error
Boolean cre(short val,short min,short max,char *text1, char *text2,short parent_num) 
{
	if ((val < min) || (val > max)) {
		give_error(text1,text2,parent_num);
		return TRUE;
		}		
	return FALSE;
}

Boolean string_not_clean(char *str,short max_length,short strict_file_naming,char *beginning_of_error,short parent_num)
{
	char error_str[256];
	Boolean error = FALSE;
	
//	short dbug = strlen(str);
	
	if ((short)strlen(str) > max_length - 1)
		error = TRUE;
		else if (strict_file_naming) {
			for (short i = 0; i < (short)strlen(str); i++) 
				if ((isalpha(str[i]) == FALSE) && (isdigit(str[i]) == FALSE) && (str[i] != ' '))
						error = TRUE;
			}
			
	if (error) {
		if (strict_file_naming)
			sprintf(error_str,"%s is either too long (max. length is %d characters) or has an illegal character. There can be only letters and numbers (no spaces or other characters).",
			  beginning_of_error, (int)(max_length - 1));
			else sprintf(error_str,"%s too long (max. length is %d characters.",beginning_of_error,(int)(max_length - 1));
		give_error(error_str,"",parent_num);
		return TRUE;
		}
	return FALSE;
}

void give_error(char *text1, char *text2,short parent_num)
{
	display_strings(text1,text2,"Error!", /* 57, 716,*/ parent_num);
}

void display_strings_event_filter (short item_hit)
{	
	switch (item_hit) {
		case 1:
			dialog_not_toast = FALSE;
			break;

		}
}

void display_strings(char *text1, char *text2,
	char *title, /* short sound_num, short graphic_num, */ short parent_num)
{
	if ((text1 == NULL) && (text2 == NULL))
		return;
	store_which_string_dlog = 970;
	if (strlen(title) > 0)
		store_which_string_dlog += 2;
	if ((text2 != NULL) && (text2[0] != 0))
		store_which_string_dlog++;
	cd_create_dialog_parent_num(store_which_string_dlog,parent_num);
	
	csit(store_which_string_dlog,4,(char *) text1);
	if ((text2 != NULL) && (text2[0] != 0)) {
		csit(store_which_string_dlog,5,(char *) text2);
		}
	if (strlen(title) > 0)
		csit(store_which_string_dlog,6,title);

	while (dialog_not_toast)
		ModalDialog();	
	
	cd_kill_dialog(store_which_string_dlog,0);
}


void choose_text_res_event_filter (short item_hit)
{
	short i;
	
	switch (item_hit) {
		case 2:
			dialog_answer = store_cur_t;
			dialog_not_toast = FALSE;
			break;
		case 6:
			dialog_answer = -1;
			dialog_not_toast = FALSE;
			break;
		case 4:
			if (which_page == 0)
				which_page = (store_last_t - store_first_t) / 40;
				else which_page--;
			put_text_res();
			break;
		case 5:
			if (which_page == (store_last_t - store_first_t) / 40)
				which_page = 0;
				else which_page++;
			put_text_res();		
			break;
		default:
			if ((item_hit >= 8) && (item_hit <= 86)) {
				store_cur_t = 40 * which_page + (item_hit - 8) / 2 + store_first_t;
				for (i = 8; i <= 86; i += 2) 
					cd_set_led(820,i,(i == item_hit) ? 1 : 0);
				}
			break;
		}
}

void put_text_res()
{
	short i;
	char str[256];
	
	for (i = 0; i < 40; i++) {
		if (store_first_t + which_page * 40 + i > store_last_t) {
			csit(820,7 + i * 2,"");
			cd_activate_item(820,8 + i * 2,0);
			}
			else {
				get_str(str,store_res_list,store_first_t + 40 * which_page + i);
				csit(820,7 + i * 2,(char *) str);
				cd_activate_item(820,8 + i * 2,1);
				}
		if (which_page * 40 + i == store_cur_t - store_first_t)
			cd_set_led(820,8 + i * 2,1);
			else cd_set_led(820,8 + i * 2,0);
		}

}

// res_list:
//   if >= 0, corresponds to a string list
//   -1 - creature types
//   -2 - item types
//   -3 - buttons
//   -4 - terrain
//   -6 - floor
short choose_text_res(short res_list,short first_t,short last_t,short cur_choice,short parent_num,char *title)
{
	store_res_list = res_list;
	store_first_t = first_t;
	store_last_t = last_t;
	if ((cur_choice >= first_t) && (cur_choice <= last_t))
		store_cur_t = cur_choice;
		else store_cur_t = first_t;
	which_page = (store_cur_t - store_first_t) / 40;

	cd_create_dialog_parent_num(820,parent_num);

	csit(820,3,title);
	if (last_t - first_t < 40) {
		cd_activate_item(820,4,0);
		cd_activate_item(820,5,0);
		}
	put_text_res();
		

	while (dialog_not_toast)
		ModalDialog();	
	
	cd_kill_dialog(820,0);
	
	return dialog_answer;
}


void edit_special_num_event_filter (short item_hit)
{
	short i;
	
	switch (item_hit) {
		case 7:
			dialog_answer = -1;
			dialog_not_toast = FALSE; 
			break;

		case 3:
			i = CDGN(825,2);
			dialog_answer = i;
			dialog_not_toast = FALSE;
			break;
		}
}

// mode - unused
// what_start - starting value
short edit_special_num( /* short mode, */ short what_start)
{			
	cd_create_dialog_parent_num(825,0);
	
	CDSN(825,2,what_start);
		
	while (dialog_not_toast)
		ModalDialog();	
	
	cd_kill_dialog(825,0);
	
	if (dialog_answer < 0)
		return what_start;
	return dialog_answer;
}

void how_many_dlog_event_filter (short item_hit)
{
	short i;
	
	switch (item_hit) {
		case 3:
			i = CDGN(828,2);
			dialog_answer = i;
			dialog_not_toast = FALSE;
			break;
		}
}

// mode - unused
// what_start - starting value
short how_many_dlog(short what_start,short minimum,short maximum,char *what_text)
{			
	cd_create_dialog_parent_num(828,0);
	
	csit(828,4,what_text);
	CDSN(828,2,what_start);
		
	while (dialog_not_toast)
		ModalDialog();	

	cd_kill_dialog(828,0);
	
	dialog_answer = minmax(minimum,maximum,dialog_answer);
	//if (dialog_answer < 0)
	//	return what_start;
	
	return dialog_answer;
}

void get_str_dlog_event_filter (short item_hit)
{
	char str[256];
	
	switch (item_hit) {
		case 3:
			if (store_strict_string) {
				CDGT(829,2,str);
				if (string_not_clean(str,SCRIPT_NAME_LEN,1,"This script name",829)) 
					return;
				}
			dialog_not_toast = FALSE;
			break;
		}
}

// strict_string - If true, only alphanumeric characters
void get_str_dlog(char *start_str,char *header_str,char *response,Boolean strict_string)
{			
	cd_create_dialog_parent_num(829,0);
	
	store_strict_string = strict_string;
	CDST(829,2,start_str);
	csit(829,4,header_str);
		
	while (dialog_not_toast)
		ModalDialog();	
	
	CDGT(829,2,response);
	cd_kill_dialog(829,0);
	

}
void make_cursor_sword() 
{
	set_cursor(0);
}

void set_cursor(short which_c) 
{
	short i;

// q_3DModStart
	if(cur_viewing_mode == 10 || cur_viewing_mode == 11) {
		if(which_c == 5)
			which_c = 8;
		if(which_c == 6)
			which_c = 9;
	}
// q_3DModEnd

	if (cursors[0] == NULL) {
// q_3DModStart
		for (i = 0; i < 10; i++)
			cursors[i] = LoadCursor(store_hInstance,MAKEINTRESOURCE(130 + i));
// q_3DModEnd
	}

	current_cursor = which_c;
	SetCursor(cursors[current_cursor]);
}
void restore_cursor()
{
	set_cursor(current_cursor);
}
