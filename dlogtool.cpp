// dlogtool.c

// OK. What on Earth is this?

// For various reasons (mainly customization and porting ease), we use a custom
// dialog box engine. This is it. 

// Dialog boxes are created using the calls in this file. Basically, what happens,
// is that the editor loads in a dialog box resource. The text entries in that resource
// tell the editor what the dialog box should look like. An empty dialog box is created by
// the editor, and the editor draws the buttons on top of that.

// Events for that dialog box are handled using the call cd_event_filter. If you make a new
// dialog box, it must have a line hooking it in in that procedure.

// How to make a new dialog box.
// 1. Make the DLOG resource are usual.
// 2. Make item 1 a button. Move that button off of the window.
// 3. If you want text entry fields, make those items 2 - whatever. (i.e.
//    3 text entries their numbers are items 2-4).
// 4. All of the other items are text. Each is a number, then an underscore,
//    then another number. (i.e. "0_1", "3_5").
//    The meanings of the numbers are:
//    First number = 0: Button. The second number is which button, the button types
//      are described in arrays button_type and button_strs below
//    First number = 1: Default Button. Like button, but is the default.
//    First number = 2: Radio button. Next number should be 0.
//    First number = 5: Picture. I don't think this works anymore.
//    One exception: If you just want a chunk of text, you can have the text entry contain
//    that text. If that text begins with ~, =, or *, formats text differently.

// Of course, you can just ignore all this mess and write regular dialog boxes.

#include "stdafx.h"
/*
#include <windows.h>
#include "stdio.h"
#include "string.h"
*/
#include "global.h"
#include "DibEngine.h"


#define ND	15
#define	NI	500
#define	NL	100
#define	NUM_DLOG_B		53
#define	DIALOG_PIXEL_ADJUST		6

// mode selector for cd_press_button()
enum EDLGBtnRes {
	eDLGBtnResCompatible,	// change graphics of the button - sound and delay - recover graphics
	eDLGBtnResChange,		// change graphics of the button - sound (no delay)
	eDLGBtnResRecover,		// recover graphics of the button
};


// Gloabl varialbes

Boolean dialog_not_toast = FALSE;

// external gloabal variables

extern HINSTANCE store_hInstance;
extern HWND	mainPtr;
extern HDC main_dc;
extern HFONT font;
extern HFONT bold_font;
extern HACCEL accel;
extern short dlg_units_x;
extern short dlg_units_y;
extern char szWinName[];
extern RECT buttons_from[NUM_BUTTONS];
//extern Boolean block_erase;
extern Boolean mouse_button_held;

/* Adventure globals */
extern HDIB buttons_gworld;

// local variables

short current_key = 0;
short dlg_keys[ND];
short dlg_types[ND];
HWND dlgs[ND];
HWND dlg_parent[ND];
short dlg_highest_item[ND];
Boolean dlg_draw_ready[ND];
short dlog_item_type[NI];

short item_dlg[NI];
short item_number[NI];
// char dlg_item_type[NI];
RECT item_rect[NI];
short item_flag[NI];
short item_active[NI];
char item_key[NI];
short item_label[NI];
short item_label_loc[NI];

char text_long_str[10][256];
char text_short_str[140][40];
char labels[NL][25];
Boolean label_taken[NL];
/**/char custom_dlg_strs[12][256];
short custom_dlg_strs_indent[12];
short store_free_slot,store_dlog_num;
short win_adjust_x = 18,win_adjust_y = 0;//20;

HWND edit_box[80];
HWND store_edit_parent[80];
short store_edit_parent_num[80];
short store_edit_item[80]; // kludgy
WNDPROC old_edit_proc[80];

HDC dlg_force_dc = NULL; // save HDCs when dealing with dlogs

HWND store_parent;
HFONT font_save;

Boolean block_next_dialog_key_event = FALSE;
short available_dlog_buttons[NUM_DLOG_B] = {0,63,64,65,1, 4,5,8,128,9,
								10,11,12,13,14, 15,16,17,29, 51,
								60,61,62, 66,69,70, 71,72,73,74, // 20
								79,80,83,86,87,88, 91,92,93,99,
								100,
								101,102,104, 129,130,131,132,133,134,135,136,137};
short button_type[150] = {1,1,4,5,1,1,0,0,1,1,
						 1,1,1,1,1,1,1,1,8,8,
						 9,9,9,1,1,2,1,6,7,1,
						 1,12,1,1,2,0,0,0,0,0,
						 0,0,0,0,0,0,0,0,0,0,
						 0,1,1,1,2,1,1,1,2,2, // 50
						 1,1,1,1,1,1,2,3,1,1,
						 1,1,1,1,2,2,2,2,2,1,
						 1,1,1,1,2,2,1,1,1,2,
						 0,1,1,1,15,13,12,12,12,1,
						 1,1,1,2,1,2,2,2,2,1, // 100
						 2,2,2,2,2,2,2,2,2,2,
						 2,2,2,2,2,2,2,2,1,1,
						 1,1,1,1,1,1,1,1,13,14,
						 0,0,0,0,0,0,0,0,0,0};
						 
// These are the bits of text that appear on the buttons in the dialog boxes.
// If a button has text 0_51, it shows text 51 ("Take"). If a button has text 1_0, it
// means text 0 ("Done"). The 0_ or the 1_ at the beginning doesn't mean anything to you. 
char *button_strs[150] = {"Done ","Ask"," "," ","Keep", "Cancel","+","-","Buy","Leave",
						"Get","1","2","3","4","5","6","Cast"," "," ",
						" "," "," ","Buy","Sell","Choose","Buy x10"," "," ","Save",
						"Race","Train","Items","Spells","Heal Party","1","2","3","4","5",
						"6","7","8","9","10","11","12","13","14","15",
				/*50*/  "16","Take","Create","Delete","Race/Special","Skill","Name","Graphic","Bash Door","Pick Lock",
				/*60*/	"Leave","Steal","Attack","OK","Yes","No","Step In"," ","Record","Climb",
						"Flee","Onward","Answer","Drink","Approach","Mage Spells","Priest Spells","Advantages","Main Menu","Land",
						"Under","Restore","Restart","Quit","Save First","Just Quit","Rest","Read","Pull","Potions",
				/*90*/		"17","Cancel","Pray","Wait","","","Delete","Graphic","Create","Give",
				/*100*/		"Destroy","Pay","Free","Next Tip","Touch", "Select Icon","Create/Edit","Clear Special","Edit Abilities","Choose",
						"Go Back","Create New","General","One Shots","Affect PCs","If-Thens","Town Specs","Out Specs","Advanced","Weapon Abil",
						"General Abil.","NonSpell Use","Spell Usable","Reagents","Missiles","Abilities","Pick Picture","Animated","Enter","Burn",
						"Insert","Remove","Accept","Refuse","Open","Close","Sit","Stand","","",
						"18","19","20","Invisible!","3DEditor","","","","",""};
short button_left_adj[150] = {2,0,0,0,6,0,0,0,4,5,
							3,0,0,0,0,0,0,4,0,0,
							0,0,0,4,0,5,0,0,0,6,
							6,6,0,0,0,0,0,0,0,0,
							0,0,0,0,6,6,6,6,6,6,
							6,6,0,2,0,0,0,2,3,3, // 50
							6,6,0,7,5,5,0,0,2,6,
							4,2,0,5,0,4,10,8,0,6,
							6,2,1,6,4,3,6,4,6,4,
							6,6,6,6,0,0,0,2,0,4,
							2,6,6,3,6,7,3,0,0,0, // 100
							0,5,3,3,3,3,3,4,4,4,
							6,4,4,2,2,3,6,5,6,6,
							0,4,2,2,6,4,6,3,0,0,
							0,0,0,0,0,0,0,0,0,0
							};
char button_def_key[150] = {
		 					0,0,20,21,'k', 24,0,0,0,0,
							'g','1','2','3','4', '5','6',0,0,0,
							0,0,0,0,0,' ',0,22,23,0,
							0,0,0,0,0,'1','2','3','4','5',
							'6','7','8','9','a', 'b','c','d','e','f',  
							'g',0,0,0,0,0,0,0,0,0,
							0,0,0,0,'y','n',0,'?','r',0,
							0,0,0,0,0,0,0,0,0, 0,
							0,0,0,0,0,0,0,0,0,0,
							'g',0,0,0,0,0,0,0,0,0,
							0,0,0,0,0,0,0,0,0,0,
							0,0,0,0,0,0,0,0,0,0,
							0,0,0,0,0,0,0,0,0,0,
							0,0,0,0,'o',0,0,0,0,0,
							0,0,0,0,0,0,0,0,0,0
							};
							// specials ... 20 - <-  21 - ->  22 up  23 down  24 esc
							// 25-30  ctrl 1-6  31 - return\


// function prototypes

short cd_create_dialog(short dlog_num,HWND parent);
// void process_new_window (HWND hDlg);
// void cd_set_edit_focus(short which_win);
short cd_process_mousetrack( short dlg_num, short item_num, RECT theRect );
// void cd_init_button(short dlog_num,short item_num, short button_num, short status);
// void cd_attach_key(short dlog_num,short item_num,char key);
// void csp(short dlog_num, short item_num, short pict_num);
// void cd_set_pict(short dlog_num, short item_num, short pict_num);
// short cd_get_active(short dlog_num, short item_num);
void cd_set_item_text(short dlog_num, short item_num, char *str);
void cd_set_item_num(short dlog_num, short item_num, short num);
// void cd_set_flag(short dlog_num,short item_num,short flag);
// void cd_text_frame(short dlog_num,short item_num,short frame);
// void cd_take_label(short dlog_num, short item_num);
// void cd_key_label(short dlog_num, short item_num,short loc);
void cd_draw_item(short dlog_num,short item_num);
void cd_initial_draw(short dlog_num);
void cd_draw(short dlog_num);
void cd_frame_item(short dlog_num, short item_num, short width);
void cd_erase_item(short dlog_num, short item_num);
// void cd_erase_rect(short dlog_num,RECT to_fry);
void cd_press_button(short dlog_num, short item_num, EDLGBtnRes mode );
short cd_get_indices(short dlg_num, short item_num, short *dlg_index, short *item_index);
short cd_get_dlg_index(short dlog_num);
short cd_get_item_id(short dlg_num, short item_num);
RECT get_item_rect(HWND hDlg, short item_num);
void frame_dlog_rect(HWND hDlg, RECT rect, short val);
// void draw_dialog_graphic(HWND hDlg, RECT rect, short which_g, Boolean do_frame,short win_or_gworld);
// RECT calc_rect(short i, short j);
// RECT calc_from_rect(short i, short j);
// RECT get_graphic_rect(HDIB graf);
// void clear_dlog_strs();
// void set_next_dlg_str(short which_str,char *str,short indent);
// short cd_complex_create_custom_dialog(HWND parent, short buttons[3],Boolean record_button);
void cd_kill_dc(short which_slot,HDC hdc);
HDC cd_get_dlog_dc(short which_slot);


INT_PTR CALLBACK dummy_dialog_proc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK fresh_edit_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);



void cd_init_dialogs()
{
	short i;

	for (i = 0; i < ND; i++) {
		dlg_keys[i] = -1;
		dlg_types[i] = 0;
		dlgs[i] = NULL;
		dlg_highest_item[i] = 0;
		}
	for (i = 0; i < NI; i++) {
		item_dlg[i] = -1;
		}
	for (i = 0; i < NL; i++) {
		label_taken[i] = FALSE;
		}
	for (i = 0; i < 80; i++) {
		edit_box[i] = NULL;
		store_edit_parent[i] = NULL;
		store_edit_parent_num[i] = -1;
		store_edit_item[i] = -1	;
		}
}

LRESULT CALLBACK fresh_edit_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	short i,cur_box = -1,cur_item_num = -1,item_for_focus = -1,first_edit_box = -1;

	for (i = 0; i < 80; i++)
		if (edit_box[i] == hwnd) {
			cur_box = i;
			cur_item_num = store_edit_item[i];
		}
	if (cur_box >= 0) {
		switch (message) {

		case WM_CHAR:
			  if ((char) (wParam) == 9) {
					return TRUE;
					}
			break;
		case WM_KEYDOWN:
//				if (wParam == VK_RETURN)
//					SendMessage(store_edit_parent[cur_box],WM_COMMAND,9,0);
//				if (wParam == VK_ESCAPE)
//					SendMessage(store_edit_parent[cur_box],WM_COMMAND,8,0);
				if (wParam == VK_RETURN)
					SendMessage(store_edit_parent[cur_box],WM_KEYDOWN,wParam,lParam);
				if (wParam == VK_ESCAPE)
					SendMessage(store_edit_parent[cur_box],WM_KEYDOWN,wParam,lParam);

				if (wParam == VK_TAB) {
					//play_sound(1);
					for (i = 0; i < 80; i++) {
						if ((store_edit_parent[i] == store_edit_parent[cur_box]) &&
							(store_edit_item[i] == cur_item_num + 1))
								item_for_focus = i;
						if ((store_edit_parent[i] == store_edit_parent[cur_box]) &&
							(store_edit_item[i] == 2))
								first_edit_box = i;
						}
					//SendMessage(store_edit_parent[cur_box],WM_KEYDOWN,wParam,lParam);
					if (item_for_focus >= 0)
						SetFocus(edit_box[item_for_focus]);
						else if (first_edit_box >= 0)
							SetFocus(edit_box[first_edit_box]);
					return TRUE;
					}
				break;   
			}

			return CallWindowProc(old_edit_proc[cur_box],hwnd,message,wParam,lParam);
		}
	return 0;
}


short cd_create_dialog_parent_num(short dlog_num,short parent)
{
	short i;

	if ((parent == 0) || (parent == 1))
		return cd_create_dialog(dlog_num,mainPtr);
	i = cd_get_dlg_index(parent);
	if (i < 0)
		return -1;
	return cd_create_dialog(dlog_num,dlgs[i]);

}

/*
short cd_create_custom_dialog(HWND parent,
	char *str_block,short pic_num, short buttons[3], Boolean record_button)
{
	short i;
	char strs[6][256];

	for (i = 0; i < 256; i++) {
		strs[0][i] = str_block[i];
		strs[1][i] = str_block[i + 256];
		strs[2][i] = str_block[i + 512];
		strs[3][i] = str_block[i + 768];
		strs[4][i] = str_block[i + 1024];
		strs[5][i] = str_block[i + 1280];
		}	
	clear_dlog_strs();
	for (i = 0; i < 6; i++)
		set_next_dlg_str(i,(char *) strs[i],0);
	return cd_complex_create_custom_dialog( parent,
		buttons, record_button);
}
*/
/*
void clear_dlog_strs()
{
	short i;
	
	for (i = 0; i < 12; i++)
		set_next_dlg_str(i,"",0);
}
*/
/*
void set_next_dlg_str(short which_str,char *str,short indent)
{
	if (which_str != minmax(0,11,which_str)) 
		return;
	if (which_str >= 10)
		strcpy((char *) custom_dlg_strs[which_str],"");
		else strcpy((char *) custom_dlg_strs[which_str],str);

	for (short k = 0; k < 256; k++) {
		if (custom_dlg_strs[which_str][k] == '|')
			 custom_dlg_strs[which_str][k] = 13;
		if (custom_dlg_strs[which_str][k] == '_')
			 custom_dlg_strs[which_str][k] = '"';
		if (custom_dlg_strs[which_str][k] == 213)
			 custom_dlg_strs[which_str][k] = 39;
		if (custom_dlg_strs[which_str][k] == -43)
			 custom_dlg_strs[which_str][k] = 39;
		}
	custom_dlg_strs_indent[which_str] = indent;
}
*/
// If button >= 1000, using a text button
/*
short cd_complex_create_custom_dialog(HWND parent,
	short buttons[3],Boolean record_button)
{

	short i,j,free_slot = -1,free_item = -1,str_width;
	HWND dlg;
	char str[256];
	short cur_item = 1;
	short but_items[4] = {-1,-1,-1,-1};
	RECT cur_text_rect = {15,15,0,0};
	short win_width = 100, win_height = 100;
	
	store_dlog_num = 900;
	//store_parent = parent;
	for (i = 0; i < ND; i++) {
		if ((dlg_keys[i] >= 0) && (dlg_types[i] == 900))
			return -1;
		}
	for (i = 0; i < ND; i++) {
		if (dlg_keys[i] < 0) {
			free_slot = i;
			i = 500;
			}
		}
	if (free_slot < 0)
		return -2;
		
	// quick check, to make sure there's at least 1 button
	if ((buttons[0] < 0) && (buttons[1] < 0) && (buttons[2] < 0))
		buttons[0] = 1;
	current_key++;
	dlg_keys[free_slot] = current_key;
	dlg_types[free_slot] = 900;
	dlg_highest_item[free_slot] = 1;
	dlg_draw_ready[free_slot] = FALSE;
	dlgs[free_slot] = NULL;

	// first, create dummy dlog
	store_free_slot = free_slot;
	dlgs[free_slot] = CreateWindow (szWinName,
			"Avernum Dialog B",
			WS_BORDER | WS_POPUP,// was visible
//			WS_DLGFRAME | WS_POPUP,// was visible
			0,
			0,
			100,
			100,
			NULL,
			NULL,
			store_hInstance,
			NULL);
	dlg = dlgs[free_slot];
	if (dlgs[free_slot] == NULL) {
		play_sound(3);
		return -3;
		}

	dlg_parent[free_slot] = parent;
	
	//process_new_window (dlgs[free_slot]);
	// instead of this, custom make items
	free_item = -1;
	
	// first, do 1-3 buttons
	for (i = 0; i < 3; i++) 
		if (buttons[i] >= 1000) {// buttons
			for (j = 150; j < NI; j++)
				if (item_dlg[j] < 0) {
					free_item = j;
					j = NI + 1;
					}
			item_dlg[free_item] = store_dlog_num;
			dlog_item_type[free_item] = (i == 0) ? 1 : 0;
			item_number[free_item] = cur_item;
			//item_rect[free_item] = get_item_rect(hDlg,i + 1);

			item_flag[free_item] = buttons[i] + 1000 * i;
			item_active[free_item] = 1;
			item_label[free_item] = 0;
           	item_label_loc[free_item] = -1;
            item_key[free_item] = '1' + i;

            but_items[i] = free_item; // remember this to set item rect later
       		cur_item++;
       		}
    // now, the record button
    if (record_button == TRUE) {
		for (j = 150; j < NI; j++)
			if (item_dlg[j] < 0) {
				free_item = j;
				j = NI + 1;
				}
		item_dlg[free_item] = store_dlog_num;
		dlog_item_type[free_item] = 0;
		item_number[free_item] = cur_item;

		item_flag[free_item] = 1000;
		item_active[free_item] = 1;
		item_label[free_item] = 0;
       	item_label_loc[free_item] = -1;
        item_key[free_item] = 0;

        but_items[3] = free_item; // remember this to set item rect later
   		cur_item++;
    	}
  

	// finally, 0-6 text, first do preprocessing to find out how much room we need
	str_width = 410;
	cur_text_rect.right = cur_text_rect.left + str_width;
	//TextSize(12);
	// finally, 0-6 text, then create the items
	for (i = 0; i < 10; i++) 
		if (strlen((char *) custom_dlg_strs[i]) > 0) {// text
			for (j = 0; j < 10; j++)
				if (item_dlg[j] < 0) {
					free_item = j;
					j = NI + 1; 
					}
			//ASB_big((char *) custom_dlg_strs[i],"","","",i,"");
			item_dlg[free_item] = store_dlog_num;
			dlog_item_type[free_item] = 9;
			item_number[free_item] = cur_item;
			item_rect[free_item] = cur_text_rect;
			SelectObject(main_dc,bold_font);
			
			//item_rect[free_item].bottom = item_rect[free_item].top + 
			//	((string_length((char *) custom_dlg_strs[i],main_dc) + 0) / (str_width - custom_dlg_strs_indent[i]) + 1) * 14 + 16;
			item_rect[free_item].bottom = item_rect[free_item].top;
			if (string_length((char *) custom_dlg_strs[i],main_dc) % (str_width - custom_dlg_strs_indent[i]) == 0)
				item_rect[free_item].bottom += ((string_length((char *) custom_dlg_strs[i],main_dc) + 0) / (str_width - custom_dlg_strs_indent[i]) + 1) * 14 + 16;
				else item_rect[free_item].bottom += ((string_length((char *) custom_dlg_strs[i],main_dc) + 0) / (str_width - custom_dlg_strs_indent[i])) * 14 + 16;

			item_rect[free_item].right += 50;
			item_rect[free_item].left += custom_dlg_strs_indent[i];
			OffsetRect(&item_rect[free_item],DIALOG_PIXEL_ADJUST,DIALOG_PIXEL_ADJUST);
			
			cur_text_rect.top = item_rect[free_item].bottom + 8;
			item_flag[free_item] = 0;
			item_active[free_item] = 1;
			item_label[free_item] = 0;
           	item_label_loc[free_item] = -1;
            item_key[free_item] = 0;
 			sprintf(text_long_str[free_item],"%s",
				(char *) custom_dlg_strs[i]);
      		cur_item++;
       		}
	
	dlg_highest_item[free_slot] = cur_item - 1;
	cur_text_rect.top += 10;

	// finally, do button rects
	for (i = 0; i < 3; i++)
		if (buttons[i] >= 1000) {
			item_rect[but_items[i]] = cur_text_rect;
			
			if (record_button == TRUE)
				item_rect[but_items[i]].left += 50;//38;
			get_str(str,1,buttons[i] % 1000);		
			
			if (string_length((char *) str,main_dc) > rect_width(&item_rect[but_items[i]]))
				item_rect[but_items[i]].bottom = item_rect[but_items[i]].top + 
					16 * (1 + (string_length((char *) str,main_dc) / rect_width(&item_rect[but_items[i]])));
				else item_rect[but_items[i]].bottom = item_rect[but_items[i]].top + 16;
			OffsetRect(&item_rect[but_items[i]],DIALOG_PIXEL_ADJUST,DIALOG_PIXEL_ADJUST);
			cur_text_rect.top = item_rect[but_items[i]].bottom + 8;
				
			win_width = item_rect[but_items[i]].right + 30;
			win_height = item_rect[but_items[i]].bottom + 14;
			}	

	if (record_button == TRUE) {
			//item_rect[but_items[3]].left = 18;
			//item_rect[but_items[3]].bottom = win_height - 10;
			//item_rect[but_items[3]].top = item_rect[but_items[3]].bottom - rect_height(record_button_rect);
			//item_rect[but_items[3]].right = item_rect[but_items[3]].left + rect_width(record_button_rect);
			//###OffsetRect(&item_rect[but_items[3]],DIALOG_PIXEL_ADJUST,0);
			}
	
	MoveWindow(dlgs[free_slot],0,0,win_width + win_adjust_x + 20,win_height + win_adjust_y + 20,FALSE);
	make_cursor_sword();
	center_window(dlgs[free_slot]);
	ShowWindow(dlgs[free_slot],SW_SHOW);
						
	if (dlg_parent[free_slot] != NULL) {
		EnableWindow(dlg_parent[free_slot],FALSE);
		//if (dlg_parent[free_slot] == mainPtr)
		//		if (map_dlg_exists == TRUE)
		//			EnableWindow(map_dlg,FALSE);
		}
	dialog_not_toast = TRUE;

	return 0;
}
*/

short cd_create_dialog(short dlog_num,HWND parent)
{
	short i,free_slot = -1;
	HWND dlg;

	if (parent != NULL) {
		if (IsWindowEnabled(parent) == 0)
      	return -1;
		}
	
	mouse_button_held = FALSE;
			
	store_dlog_num = dlog_num;
	//store_parent = parent;
	for (i = 0; i < ND; i++) {
		if ((dlg_keys[i] >= 0) && (dlg_types[i] == dlog_num))
			return -1;
		}
	for (i = 0; i < ND; i++) {
		if (dlg_keys[i] < 0) {
			free_slot = i;
			i = 500;
			}
		}
	if (free_slot < 0)
		return -2;
	current_key++;
	dlg_keys[free_slot] = current_key;
	dlg_types[free_slot] = dlog_num;
	dlg_highest_item[free_slot] = 1;
	dlg_draw_ready[free_slot] = FALSE;
	dlgs[free_slot] = NULL;

	// first, create dummy dlog
	store_free_slot = free_slot;
	dlg = CreateDialog(store_hInstance,MAKEINTRESOURCE(dlog_num),0, dummy_dialog_proc);

	if (dlgs[free_slot] == NULL) {
		play_sound(3);
		return -3;
		}
	make_cursor_sword();
	center_window(dlgs[free_slot]);

	dlg_parent[free_slot] = parent;


	ShowWindow(dlgs[free_slot],SW_SHOW);
	DestroyWindow(dlg); //Necesary? Dunno.

	if (dlg_parent[free_slot] != NULL) {
		EnableWindow(dlg_parent[free_slot],FALSE);
		//if (dlg_parent[free_slot] == mainPtr)
		//	if (map_dlg_exists == TRUE)
		//			EnableWindow(map_dlg,FALSE);
		}
	dialog_not_toast = TRUE;
	return 0;

}

INT_PTR CALLBACK dummy_dialog_proc(HWND hDlg, UINT message, WPARAM /*wParam*/, LPARAM /*lParam*/) {
	
	short i,j,k,free_slot = -1,free_item = -1,type,flag;
	char item_str[256];
	Boolean str_stored = FALSE,focus_set = FALSE;
	RECT dlg_rect,store_rect;
	short win_height = 0, win_width = 0, num_text_boxes = 0;
	short str_offset = 1;
	RECT pic_rect = {7,7,43,43};

	
	free_slot = store_free_slot;

	
	switch (message) {
		case WM_INITDIALOG:

	// now, make a window, matching dialog
	GetWindowRect(hDlg,&dlg_rect);
	dlgs[store_free_slot] = CreateWindow (szWinName,
			"BoA Editor Dialog",
			// WS_BORDER | WS_POPUP,// was visible
			WS_TILEDWINDOW,
			0,
			0,
			(dlg_rect.right - dlg_rect.left * 8) / dlg_units_x,
			(dlg_rect.bottom - dlg_rect.top * 16) / dlg_units_y,
			NULL,
			NULL,
			store_hInstance,
			NULL);
	// Now, give the window its items
	for (i = 0; i < 200; i++)
		if (GetDlgItem(hDlg,i) != NULL) {
			GetDlgItemText(hDlg,i,item_str,256);
      	str_offset = 1;
			dlg_highest_item[free_slot] = i;
			str_stored = FALSE;
			if (strlen((char *)item_str) == 0) {
				sprintf((char *) item_str, "+");
				type = 3;
				flag = 0;
	            str_stored = TRUE;
			}
			else if (item_str[0] == '+') { // default is framed text
				type = 3;
				flag = 1;
            str_stored = TRUE;
				}
			else if (item_str[0] == '*') {
				type = 3;
				flag = 0;
				str_stored = TRUE;
				}
			else if (item_str[0] == '~') {
				type = 7;
				flag = 0;
				str_stored = TRUE;
				}
			else if (item_str[0] == '!') {
				type = 4;
				flag = 0;
				str_stored = TRUE;
				}
			else if (item_str[0] == '=') {
				type = 9;
				flag = 1;
				str_stored = TRUE;
				}
			else if (((item_str[0] >= 65) && (item_str[0] <= 122)) || (item_str[0] == '"')) {
				type = 9;
				flag = 0;
				str_offset = 0;
				str_stored = TRUE;
				}
			else if ((item_str[0] == '^') || (item_str[0] == '&')) {
				type = (item_str[0] == '^') ? 10 : 11;
				flag = 1;
				//if (string_length((char *) item_str) > 55)
				//	flag = 2;
				str_stored = TRUE;
				}
			else sscanf(item_str,"%hd_%hd",&type,&flag);

			free_item = -1;
			// find free item
			switch (type) {
				case 0: case 1: case 2: case 5: case 6:
					for (j = 150; j < NI; j++)
						if (item_dlg[j] < 0) {
							free_item = j;
							j = NI + 1;
							}
					break;
				default:
					if ((type == 9) ||
					 ((str_stored == TRUE) && (strlen((char *) item_str) > 35))) {
						for (j = 0; j < 10; j++)
							if (item_dlg[j] < 0) {
								free_item = j;
								j = NI + 1;
								}
						}
						else {
							for (j = 10; j < 140; j++)
								if (item_dlg[j] < 0) {
									free_item = j;
									j = NI + 1;
									}
							}
					break;
				}

			if (free_item >= 0) {
					item_dlg[free_item] = store_dlog_num;
					dlog_item_type[free_item] = type;
					item_number[free_item] = i;

					item_rect[free_item] = get_item_rect(hDlg,i);
					OffsetRect(&item_rect[free_item],12,12);
					
					item_rect[free_item].top = item_rect[free_item].top / 2;
					item_rect[free_item].left = item_rect[free_item].left / 2;
					item_rect[free_item].bottom = item_rect[free_item].bottom / 2;
					item_rect[free_item].right = item_rect[free_item].right / 2;

					item_rect[free_item].top = (item_rect[free_item].top * 16) / dlg_units_y;
					item_rect[free_item].left = (item_rect[free_item].left * 8) / dlg_units_x;
					item_rect[free_item].bottom = (item_rect[free_item].bottom * 16) / dlg_units_y;
					item_rect[free_item].right = (item_rect[free_item].right * 8) / dlg_units_x;

					item_flag[free_item] = flag;
					item_active[free_item] = 1;
					item_label[free_item] = 0;
            	    item_label_loc[free_item] = -1;
               		item_key[free_item] = 0;

					switch (type) {
						case 0: case 1:
							if (item_flag[free_item] != 143) {
								store_rect = buttons_from[button_type[flag]];
								item_rect[free_item].right = item_rect[free_item].left + (store_rect.right - store_rect.left);
								item_rect[free_item].bottom = item_rect[free_item].top + (store_rect.bottom - store_rect.top);
								item_key[free_item] = button_def_key[flag];
								if (type == 1)
									item_key[free_item] = 31;
								}
							break;
						case 2:
							item_rect[free_item].right = item_rect[free_item].left + 12;
							item_rect[free_item].bottom = item_rect[free_item].top + 12;
                    		item_key[free_item] = -1;
							break;
						case 5: // std dlog pic position
							if (item_flag[free_item] < 700)
								item_rect[free_item] = pic_rect;
							break;
						case 3: case 4: case 7: case 8: case 9: case 10: case 11:
							sprintf(((free_item < 10) ? text_long_str[free_item] : text_short_str[free_item - 10]),"");
							if (str_stored == TRUE) {
								if (free_item < 10) {
									sprintf(text_long_str[free_item],"%s",
									  (char *) (item_str + str_offset));
									for (k = 0; k < 256; k++) {
										if (text_long_str[free_item][k] == '|')
											 text_long_str[free_item][k] = 13;
										if (text_long_str[free_item][k] == '_')
											 text_long_str[free_item][k] = '"';
										if (text_long_str[free_item][k] == 213)
											 text_long_str[free_item][k] = 39;
										if (text_long_str[free_item][k] == -43)
											 text_long_str[free_item][k] = 39;
										}
									// give text a little extra room
									//if ((store_dlog_num >= 2000) || (store_dlog_num == 986))
									//	item_rect[free_item].right += 20;
									}
								else {
									sprintf(text_short_str[free_item - 10],"%-34s",
									  (char *) (item_str + str_offset));
									for (k = 0; k < 35; k++) {
										if (text_short_str[free_item][k] == '|')
											 text_short_str[free_item][k] = 13;
										if (text_short_str[free_item][k] == '_')
											 text_short_str[free_item][k] = '"';
										if (text_long_str[free_item][k] == 213)
											 text_long_str[free_item][k] = 39;
										if (text_long_str[free_item][k] == -43)
											 text_long_str[free_item][k] = 39;
										}
										}
								}
							item_key[free_item] = -1;
							if (type >= 10) { // never got used
                        break;
								//store_rect = dlg_buttons_gworld[1][0]->portRect;
								//item_rect[free_item].right = item_rect[free_item].left + store_rect.right;
								//item_rect[free_item].bottom = item_rect[free_item].top + store_rect.bottom;
								//if (type == 11)
								//	item_key[free_item] = 31;
								}
							break;
						case 6:
							for (short l = 0; l < 80; l++)
								if (store_edit_parent[l] == NULL) {
									if (item_rect[free_item].bottom - item_rect[free_item].top < 24)
										item_rect[free_item].top -= 2;
									if (item_rect[free_item].bottom - item_rect[free_item].top <= 22)
										edit_box[l] = CreateWindow("edit",NULL,WS_CHILD | WS_BORDER | WS_VISIBLE | ES_AUTOVSCROLL,
											item_rect[free_item].left,item_rect[free_item].top,
											item_rect[free_item].right - item_rect[free_item].left,
											smax(22, (short)(item_rect[free_item].bottom - item_rect[free_item].top)),
											dlgs[free_slot],(HMENU) 150,store_hInstance,NULL);
										else edit_box[l] = CreateWindow("edit",NULL,WS_CHILD | WS_BORDER | WS_VISIBLE | ES_MULTILINE | ES_AUTOVSCROLL,
											item_rect[free_item].left,item_rect[free_item].top,
											item_rect[free_item].right - item_rect[free_item].left,
											smax(22, (short)(item_rect[free_item].bottom - item_rect[free_item].top)),
											dlgs[free_slot],(HMENU) 150,store_hInstance,NULL);
									num_text_boxes++;
									store_edit_parent[l] =  dlgs[free_slot];
 									store_edit_parent_num[l] = store_dlog_num;
									store_edit_item[l] = i;
									old_edit_proc[l] = (WNDPROC)GetWindowLong(edit_box[l], GWL_WNDPROC);
									SetWindowLong(edit_box[l], GWL_WNDPROC, (LONG)fresh_edit_proc);
									if (focus_set == FALSE) {
										SetFocus(edit_box[l]);
										focus_set = TRUE;
										}
									l = 80;
									}
							break;
						}
					if (free_item > 1) {
						win_height = smax(win_height, (short)(item_rect[free_item].bottom + 28 + 
							((dlg_units_y > 16) ? 3 : 0)));
						win_width = smax(win_width, (short)item_rect[free_item].right + 11);
						}
				}

			}
			MoveWindow(dlgs[free_slot],0,0,win_width + win_adjust_x,win_height + 3 + win_adjust_y,FALSE);
			EndDialog(hDlg, 0);
			return TRUE;
		}
		return TRUE;
}

/*
void cd_set_edit_focus(short which_win)
{  
	short i;
	
	for (i = 0; i < 80; i++)
		if (store_edit_parent_num[i] == which_win) {
			if (edit_box[i] != NULL)
				SetFocus(edit_box[i]);
			return;
			}
}
*/

short cd_kill_dialog(short dlog_num,short parent_message)
{
	short i,which_dlg = -1;

	
	for (i = 0; i < ND; i++)
		if ((dlg_keys[i] >= 0) && (dlg_types[i] == dlog_num))
			which_dlg = i;
	if (which_dlg < 0)
		return -1;
	

	for (i = 0; i < NI; i++)
		if (item_dlg[i] == dlog_num) {
			if (dlog_item_type[i] == 6)  {
				for (short j = 0; j < 80; j++)
					if ((store_edit_parent_num[j] == dlog_num) && (store_edit_item[j] == item_number[i])) {
						DestroyWindow(edit_box[j]);
                		store_edit_parent[j] = NULL;
						edit_box[j] = NULL;
						}
				}
			if (item_label[i] > 0)
				label_taken[item_label_loc[i]] = FALSE;
			item_dlg[i] = -1;
			}
	

	if (dlg_parent[which_dlg] != NULL) {
		EnableWindow(dlg_parent[which_dlg],TRUE);
		//if (dlg_parent[which_dlg] == mainPtr)
		//	if (map_dlg_exists == TRUE)
		//		EnableWindow(map_dlg,TRUE);
	
		SetFocus(dlg_parent[which_dlg]);
		SetWindowPos(dlg_parent[which_dlg],HWND_TOP,0,0,100,100,
			SWP_NOMOVE | SWP_NOSIZE | SWP_NOREDRAW);
		//cd_set_edit_focus(dlg_parent[which_dlg]);
		}

	if (parent_message > 0)
		SendMessage(dlg_parent[which_dlg],WM_COMMAND,parent_message,0);

	DestroyWindow(dlgs[which_dlg]);
	dlg_keys[which_dlg] = -1;
	
	dialog_not_toast = TRUE;
	//?block_erase = TRUE;


	return 0;
} 


short cd_process_click(HWND window,POINT the_point, WPARAM wparam, /*LPARAM lparam,*/ short *item)
{
	short i,which_dlg,dlg_num,item_id;
	short dlog_key;

	if ((which_dlg = cd_find_dlog(window,&dlg_num,&dlog_key)) < 0)
		return -1;


	for (i = 0; i < dlg_highest_item[which_dlg] + 1; i++)
		if ((item_id = cd_get_item_id(dlg_num,i)) >= 0) {
			if ((POINTInRECT(the_point,item_rect[item_id])) && (item_active[item_id] > 0)
				&& ((dlog_item_type[item_id] < 3) || (dlog_item_type[item_id] == 8)
				|| (dlog_item_type[item_id] == 10)|| (dlog_item_type[item_id] == 11))) {
  					*item = i;
					if (MK_CONTROL & wparam)
						*item += 100;
					if (dlog_item_type[item_id] != 8)
						dlg_num = cd_process_mousetrack( dlg_num, i, item_rect[item_id] );
					return dlg_num;
					}
			}
	return -1;
}

short cd_process_mousetrack( short dlg_num, short item_num, RECT theRect )
{
	short result = -1;
	bool prevInside = false;
	bool currInside = true;
	
	cd_press_button(dlg_num, item_num, eDLGBtnResChange);	// give click response

	// Track mouse while it is down
	bool still_down = true;
	while ( still_down ) {

		POINT currPt;
		MSG msg;

		GetMessage(&msg, NULL, WM_MOUSEFIRST , WM_MOUSELAST);

		currPt.x = (int)(short)LOWORD(msg.lParam);
		currPt.y = (int)(short)HIWORD(msg.lParam);

		switch (msg.message)
		{
		// handle movement messages
		case WM_MOUSEMOVE:
			prevInside = currInside;
			currInside = POINTInRECT(currPt, theRect);
			if ( !prevInside && currInside )
				cd_press_button(dlg_num, item_num, eDLGBtnResChange);
			if ( prevInside && !currInside )
				cd_press_button(dlg_num, item_num, eDLGBtnResRecover);
			break;

		// handle accept messages
		case WM_LBUTTONUP:
			if ( POINTInRECT(currPt, theRect) )
			result = dlg_num;
			still_down = false;
			break;

		// ignore these messages
		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
		case WM_RBUTTONUP:
		case WM_MBUTTONDOWN:
		case WM_MBUTTONUP:
			break;

		// just dispatch rest of the messages
		default:
			DispatchMessage(&msg);
			break;
		}
	}

	cd_press_button(dlg_num, item_num, eDLGBtnResRecover);
	
	return result;
}

short cd_process_syskeystroke(HWND window,WPARAM wparam,/* LPARAM lparam,*/ short *item)
{
	short i,which_dlg,dlg_num,dlg_key,item_id;
	char char_hit;


	if ((which_dlg = cd_find_dlog(window,&dlg_num,&dlg_key)) < 0)
		return -1;
							// specials ... 20 - <-  21 - ->  22 up  23 down  24 esc
							// 25-30  ctrl 1-6

	switch (wparam) {
		case VK_ESCAPE:
			char_hit = 24;
			break;
		case VK_LEFT:
			char_hit = 20;
			break;
		case VK_UP:
			char_hit = 22;
			break;
		case VK_RIGHT:
			char_hit = 21;
			break;
		case VK_DOWN:
			char_hit = 23;
			break;
		case VK_RETURN:
			char_hit = 31;
			break;
		case VK_NUMPAD1: case VK_NUMPAD2: case VK_NUMPAD3: case VK_NUMPAD4: case VK_NUMPAD5: 
			
			block_next_dialog_key_event = TRUE;
			return -1;
			break;
		default:
			return -1;
		}

	for (i = 0; i < dlg_highest_item[which_dlg] + 1; i++)
		if ((item_id = cd_get_item_id(dlg_num,i)) >= 0) {
			if ((item_key[item_id] == char_hit) && (item_active[item_id] > 0)
				&&  ((dlog_item_type[item_id] < 3) || (dlog_item_type[item_id] == 8))) {
					*item = i;
					if (dlog_item_type[item_id] != 8)
						cd_press_button(dlg_num, i, eDLGBtnResCompatible);
					return dlg_num;
			}
			}
	// kludgy. If you get an escape and is isn't processed, make it an enter
	if (wparam == VK_ESCAPE) {
   	char_hit = 31;
		for (i = 0; i < dlg_highest_item[which_dlg] + 1; i++)
			if ((item_id = cd_get_item_id(dlg_num,i)) >= 0) {
				if ((item_key[item_id] == char_hit) && (item_active[item_id] > 0)
					&&  ((dlog_item_type[item_id] < 3) || (dlog_item_type[item_id] == 8))) {
						*item = i;
						if (dlog_item_type[item_id] != 8)
							cd_press_button( dlg_num, i, eDLGBtnResCompatible);
						return dlg_num;
				}
			}
		}

	return -1;
}
short cd_process_keystroke(HWND window,WPARAM wparam,/* LPARAM lparam,*/ short *item)

{
	short i,which_dlg,dlg_num,dlg_key,item_id;
	char char_hit;

	if ((which_dlg = cd_find_dlog(window,&dlg_num,&dlg_key)) < 0)
		return -1;
							// specials ... 20 - <-  21 - ->  22 up  23 down  24 esc
							// 25-30  ctrl 1-6

	char_hit = (char) wparam;

	if (block_next_dialog_key_event == TRUE) {
		block_next_dialog_key_event = FALSE;
		return -1;
	}
		
	for (i = 0; i < dlg_highest_item[which_dlg] + 1; i++)
		if ((item_id = cd_get_item_id(dlg_num,i)) >= 0) {
			if (((item_key[item_id] == char_hit) && (item_active[item_id] > 0)
				&&  ((dlog_item_type[item_id] < 3) || (dlog_item_type[item_id] == 8))) ||
				((item_key[item_id] == '1') && (char_hit == 13))) {
					*item = i;
					if (dlog_item_type[item_id] != 8)
						cd_press_button( dlg_num,i, eDLGBtnResCompatible );
					return dlg_num;
			}
		}
	return -1;
}


/*
void cd_attach_key(short dlog_num,short item_num,char key)
{
	short dlg_index,item_index;

	if (cd_get_indices(dlog_num,item_num,&dlg_index,&item_index) < 0)
		return;

	if ((dlog_item_type[item_index] > 2) && (dlog_item_type[item_index] != 8)) {
		beep();
		return;
		}
	item_key[item_index] = key;
}
*/
/*
void csp(short dlog_num, short item_num, short pict_num)
{
	cd_set_pict( dlog_num,  item_num,  pict_num);
}
*/
/*
void cd_set_pict(short dlog_num, short item_num, short pict_num)
{
	short dlg_index,item_index;
	if (cd_get_indices(dlog_num,item_num,&dlg_index,&item_index) < 0)
		return;
	if (dlog_item_type[item_index] != 5) {
		beep();
		return;
		}
	item_flag[item_index] = pict_num;
	if (pict_num == -1)
		cd_erase_item(dlog_num,item_num);
		else cd_draw_item(dlog_num,item_num);
}
*/

void cd_activate_item(short dlog_num, short item_num, short status)
{
	short dlg_index,item_index;
	if (cd_get_indices(dlog_num,item_num,&dlg_index,&item_index) < 0)
		return;

	item_active[item_index] = status;
	cd_draw_item(dlog_num,item_num);
}

/*
short cd_get_active(short dlog_num, short item_num)
{
	short dlg_index,item_index;
	if (cd_get_indices(dlog_num,item_num,&dlg_index,&item_index) < 0)
		return -1;

	return item_active[item_index];
}
*/

void cd_get_item_text(short dlog_num, short item_num, char *str)
{
	short dlg_index,item_index;
	if (cd_get_indices(dlog_num,item_num,&dlg_index,&item_index) < 0)
		return ;
	if (dlog_item_type[item_index] == 6) {
		sprintf(str,"");
		for (short i = 0; i < 80; i++)
			if ((store_edit_parent_num[i] == dlog_num) && (store_edit_item[i] == item_num)
				&& (edit_box[i] != NULL))
				GetWindowText(edit_box[i],str,255);
		return;
		}
	if (item_index >= 150) {
		beep();
		return;
		}
	if (item_index < 10)
		sprintf(str,"%s",text_long_str[item_index]);
		else sprintf(str,"%s",text_short_str[item_index - 10]);
}

void csit(short dlog_num, short item_num, char *str)
{
	cd_set_item_text( dlog_num,  item_num, str);
}

/*
void cd_get_text_edit_str(short dlog_num, char *str)
{
		if (edit_box != NULL)
			GetWindowText(edit_box,str,255);
			else str[0] = 0;
}

	
// NOTE!!! Expects a c string
void cd_set_text_edit_str(short dlog_num,short item_num, char *str)
{
		if (edit_box != NULL)
			SetWindowText(edit_box,str);
}
*/

void cd_retrieve_text_edit_str(short dlog_num, short item_num, char *str)
{
	short i;
	
	for (i = 0; i < 80; i++)
		if ((store_edit_parent_num[i] == dlog_num) && (store_edit_item[i] == item_num)
			&& (edit_box[i] != NULL)) {
				GetWindowText(edit_box[i],str,255);
				return;
				}
}
	
short cd_retrieve_text_edit_num(short dlog_num, short item_num)
{
	short i;
	short num_given;
	char str[256];
	
	for (i = 0; i < 80; i++)
		if ((store_edit_parent_num[i] == dlog_num) && (store_edit_item[i] == item_num)
			&& (edit_box[i] != NULL)) {
				GetWindowText(edit_box[i],str,255);
				sscanf(str,"%hd",&num_given);
				return (short) num_given;
				}
	return -1;
				

}	


// NOTE!!! Expects a c string
void cd_set_text_edit_str(short dlog_num, short item_num, char *str)
{
	short i;

	for (i = 0; i < 80; i++)
			if ((store_edit_parent_num[i] == dlog_num) && (store_edit_item[i] == item_num)
				&& (edit_box[i] != NULL))
					SetWindowText(edit_box[i],str);

}
// NOTE!!! Expects a c string
void cd_set_text_edit_num(short dlog_num, short item_num, short num)
{
	short i;
	char store_ptr[256];
	
		sprintf(store_ptr,"%d", (int)num);
		for (i = 0; i < 80; i++)
			if ((store_edit_parent_num[i] == dlog_num) && (store_edit_item[i] == item_num)
				&& (edit_box[i] != NULL))
					SetWindowText(edit_box[i],store_ptr);
}

void cd_set_item_text(short dlog_num, short item_num, char *str)
{
	short k,dlg_index,item_index;
	if (cd_get_indices(dlog_num,item_num,&dlg_index,&item_index) < 0)
		return ;
	if (dlog_item_type[item_index] == 6) {
		for (short i = 0; i < 80; i++)
			if ((store_edit_parent_num[i] == dlog_num) && (store_edit_item[i] == item_num)
				&& (edit_box[i] != NULL))
					SetWindowText(edit_box[i],str);
		return;
		}
	if (item_index >= 150) {
		beep();
		return;
		}
	if (item_index < 10) {
		sprintf(text_long_str[item_index],"%s",str);
		for (k = 0; k < 256; k++) {
			if (text_long_str[item_index][k] == '|')
				text_long_str[item_index][k] = 13;
			if (text_long_str[item_index][k] == '_')
				 text_long_str[item_index][k] = '"';
			if (text_long_str[item_index][k] == 213)
				 text_long_str[item_index][k] = 39;
			if (text_long_str[item_index][k] == -43)
				 text_long_str[item_index][k] = 39;
			}

		}
		else sprintf(text_short_str[item_index - 10],"%-34s",str);
	cd_draw_item( dlog_num,item_num);
}

void cdsin(short dlog_num, short item_num, short num) 
{
	cd_set_item_num( dlog_num,  item_num,  num);
}


void cd_set_item_num(short dlog_num, short item_num, short num)
{
	short dlg_index,item_index;
	if (cd_get_indices(dlog_num,item_num,&dlg_index,&item_index) < 0)
		return ;
	if (item_index >= 150) {
		beep();
		return;
		}
	if (item_index < 10)
		sprintf(text_long_str[item_index],"%d", (int)num);
		else sprintf(text_short_str[item_index - 10],"%d", (int)num);
	cd_draw_item( dlog_num,item_num);
}

void cd_set_led(short dlog_num,short item_num,short state)
{
	short dlg_index,item_index;

	if (cd_get_indices(dlog_num,item_num,&dlg_index,&item_index) < 0)
		return;

	if (dlog_item_type[item_index] != 2) {
		beep();
		return;
		}
	item_flag[item_index] = state;
	cd_draw_item(dlog_num,item_num);
}

/*
void cd_set_flag(short dlog_num,short item_num,short flag)
{
	short dlg_index,item_index;

	if (cd_get_indices(dlog_num,item_num,&dlg_index,&item_index) < 0)
		return;

	item_flag[item_index] = flag;
	cd_draw_item(dlog_num,item_num);
}
*/

// Undocumented feature. This returns the items flag if it's not an LED
short cd_get_led(short dlog_num,short item_num)
{
	short dlg_index,item_index;

	if (cd_get_indices(dlog_num,item_num,&dlg_index,&item_index) < 0)
		return 0;

	//if (dlog_item_type[item_index] != 2) {
	//	beep();
	//	return 0;
	//	}
	return item_flag[item_index];
}

void cd_set_led_range(short dlog_num,short first_led,short last_led,short which_to_set)
{
	short i;
	
	for (i = first_led; i <= last_led; i++) {
		if (i - first_led == which_to_set)
			cd_set_led(dlog_num,i,1);
			else cd_set_led(dlog_num,i,0);
		}
}

// Use when someone presses something in the range, to light up the right button
// Also has an error check ... if which_to_set is outside range, do nothing
void cd_hit_led_range(short dlog_num,short first_led,short last_led,short which_to_set)
{
	short i;
	
	if ((which_to_set < first_led) || (which_to_set > last_led))
		return;
	for (i = first_led; i <= last_led; i++) {
		if (i == which_to_set)
			cd_set_led(dlog_num,i,1);
			else cd_set_led(dlog_num,i,0);
		}
}
short cd_get_led_range(short dlog_num,short first_led,short last_led)
{
	short i;
	
	for (i = first_led; i <= last_led; i++) {
		if (cd_get_led(dlog_num,i) == 1)
			return i - first_led;
		}
	return 0;
}

void cd_flip_led(short dlog_num,short item_num,short item_hit)
{
	if (item_hit != item_num)
		return;
	if (cd_get_led(dlog_num,item_num) > 0)
		cd_set_led(dlog_num,item_num,0);
		else cd_set_led(dlog_num,item_num,1);
}

/*
void cd_text_frame(short dlog_num,short item_num,short frame)
{
	short dlg_index,item_index;

	if (cd_get_indices(dlog_num,item_num,&dlg_index,&item_index) < 0)
		return;

	if (item_index >= 150) {
		beep();
		return;
		}
	item_flag[item_index] = frame;
	cd_draw_item(dlog_num,item_num);
}
*/

void cd_add_label(short dlog_num, short item_num, char *label, short label_flag)
{
	short dlg_index,item_index,label_loc = -1;
	short i;

	if (cd_get_indices(dlog_num,item_num,&dlg_index,&item_index) < 0)
		return;

	if (item_label_loc[item_index] < 0) {
		item_label[item_index] = label_flag;
		for (i = 0; i < 100; i++)
			if (label_taken[i] == FALSE) {
				label_loc = i;
				label_taken[i] = TRUE;
				i = 100;
				}
		if (label_loc < 0) {
			beep();
			return;
			}
		item_label_loc[item_index] = label_loc;
		}
      else cd_erase_item(dlog_num,item_num + 100);
	label_loc = item_label_loc[item_index];
	sprintf((char *) labels[label_loc],"%-24s",label);
	if (item_active[item_index] > 0)
		cd_draw_item(dlog_num,item_num);
}

/*
void cd_take_label(short dlog_num, short item_num)
{
	short dlg_index,item_index;

	if (cd_get_indices(dlog_num,item_num,&dlg_index,&item_index) < 0)
		return;
	item_label[item_index] = 0;
	label_taken[item_label_loc[item_index]] = FALSE;
}
*/

/*
void cd_key_label(short dlog_num, short item_num,short loc)
{
	short dlg_index,item_index;
	char str[10];
	if (cd_get_indices(dlog_num,item_num,&dlg_index,&item_index) < 0)
		return;
	sprintf((char *) str,"  ");
	str[0] = item_key[item_index];
	cd_add_label(dlog_num,item_num, str, 7 + loc * 100);
}
*/

void cd_draw_item(short dlog_num,short item_num)
{
	short dlg_index,item_index,store_label;
	COLORREF c[4] = {RGB(0,0,0),RGB(255,0,0),RGB(239,205,49),RGB(59,12,12)};
	RECT from_rect,to_rect;
	HDC win_dc;
	HFONT old_font;
	char str[256],button_str[256];

	if (cd_get_indices(dlog_num,item_num,&dlg_index,&item_index) < 0)
		return;
	if (dlg_draw_ready[dlg_index] == FALSE)
		return;
//return;//	could get crash
	win_dc = cd_get_dlog_dc(dlg_index);
	old_font = (HFONT) SelectObject(win_dc,bold_font);
	dlg_force_dc = win_dc;
	
	if (item_active[item_index] == 0) {
		cd_erase_item(dlog_num,item_num);
		cd_erase_item(dlog_num,item_num + 100);
		}
		else {
			switch (dlog_item_type[item_index]) {
				case 0: case 1: case 10: case 11:
					if (item_flag[item_index] == 143)
						break;
					if (item_flag[item_index] == 1000) { // is it the record button?
						//rect_draw_some_item(mixed_gworld,record_button_from_rect,(HDIB) win_dc,item_rect[item_index],1,2);
						
						}
					else if (item_flag[item_index] > 1000) { // is it a text button?
						SetTextColor(win_dc,c[3]);
						get_str(str,1,item_flag[item_index] % 1000);
						sprintf((char *) button_str,"%d - %s",(int)(item_flag[item_index] / 1000),str);
						char_win_draw_string(win_dc,item_rect[item_index],
							 (char *) button_str,0,14);
						}
						else {
							from_rect = buttons_from[button_type[item_flag[item_index]]];
							rect_draw_some_item(buttons_gworld,from_rect,
								(HDIB) win_dc,item_rect[item_index],0,2);
							SelectObject(win_dc,bold_font);
							SetTextColor(win_dc,c[2]);
							//if (dlog_item_type[item_index] < 2)
							//	OffsetRect(&item_rect[item_index],-1 * button_left_adj[item_flag[item_index]],0);
							if (dlog_item_type[item_index] < 2) {
									char_win_draw_string(win_dc,item_rect[item_index],
									 (char *) (button_strs[item_flag[item_index]]),1,8);
								}
								else {
										char_win_draw_string(win_dc,item_rect[item_index],
										 (char *) ((item_index < 10) ? text_long_str[item_index] : 
										  text_short_str[item_index - 10]),1,8);
									}
							//if (dlog_item_type[item_index] < 2)
							//	OffsetRect(&item_rect[item_index],button_left_adj[item_flag[item_index]],0);
							SetTextColor(win_dc,c[0]);
							}

					break;

				case 2:
					from_rect = buttons_from[9];
					OffsetRect(&from_rect,24 * (1 - item_flag[item_index]),0);
					switch (item_flag[item_index]) {
						case 0:  
							rect_draw_some_item( buttons_gworld,from_rect,(HDIB) win_dc,item_rect[item_index],0,2); break;
						case 1: rect_draw_some_item( buttons_gworld,from_rect,(HDIB) win_dc,item_rect[item_index],0,2); break;
						case 2: rect_draw_some_item(buttons_gworld,from_rect,(HDIB) win_dc,item_rect[item_index],0,2); break;
						}
					break;

				case 3: case 4: case 7: case 8: case 9: 
					cd_erase_item(dlog_num,item_num);
					SetTextColor(win_dc,PALETTEINDEX(c[0]));
						SelectObject(win_dc,bold_font);
					//if ((dlog_item_type[item_index] == 3) || (dlog_item_type[item_index] == 9))
					//	SelectObject(win_dc,font_10);
					if (dlog_item_type[item_index] == 4)
						SelectObject(win_dc,font);
					if (dlog_item_type[item_index] == 7)
						SelectObject(win_dc,bold_font);
					if (item_flag[item_index] % 10 == 1) {
						cd_frame_item(dlog_num,item_num,2);
						}
					if (dlog_item_type[item_index] >= 10) {
						SetTextColor(win_dc,PALETTEINDEX(c[1]));
						}
					if (item_rect[item_index].bottom - item_rect[item_index].top < 20) {
						item_rect[item_index].left += 3;
						DrawText(win_dc,(char *) ((item_index < 10) ? text_long_str[item_index] :
							text_short_str[item_index - 10]), -1, &item_rect[item_index],
							DT_LEFT | DT_SINGLELINE | DT_VCENTER | DT_NOCLIP);
						item_rect[item_index].left -= 3;
						}
						else {
//char dbug[256];
//strcpy(dbug,(char *) ((item_index < 10) ? text_long_str[item_index] :
//								text_short_str[item_index - 10]));
								
							MacInsetRect(&item_rect[item_index],4,4);
							DrawText(win_dc,(char *) ((item_index < 10) ? text_long_str[item_index] :
								text_short_str[item_index - 10]), -1, &item_rect[item_index],
								DT_LEFT | DT_WORDBREAK | DT_NOCLIP);
							MacInsetRect(&item_rect[item_index],-4,-4);
							}
					SetTextColor(win_dc,PALETTEINDEX(c[0]));
					break;

				case 5:
					if (item_flag[item_index] == -1)
						cd_erase_item(dlog_num,item_num);
//						else draw_dialog_graphic(dlgs[dlg_index], item_rect[item_index], item_flag[item_index],
//						  (item_flag[item_index] >= 3000) ? FALSE : TRUE,0);
					break;
				}
			}

			if (item_label[item_index] != 0) {
				store_label = item_label[item_index];
				if (store_label >= 1000) {
					store_label -= 1000;
					SelectObject(win_dc,bold_font);
					}
					else {
						SelectObject(win_dc,font);
						}
				to_rect = item_rect[item_index];
				switch (store_label / 100) {
					case 0:
						to_rect.right = to_rect.left;
						to_rect.left -= 2 * (store_label % 100);
						break;
					case 1:
						to_rect.bottom = to_rect.top;
						to_rect.top -= 2 * (store_label % 100);
						break;
					case 2:
						to_rect.left = to_rect.right;
						to_rect.right += 2 * (store_label % 100);
						break;
					case 3:
						to_rect.top = to_rect.bottom;
						to_rect.bottom += 2 * (store_label % 100);
						break;
					}

				if (to_rect.bottom - to_rect.top < 14) {
					to_rect.bottom += (14 - (to_rect.bottom - to_rect.top)) / 2 + 1;
					to_rect.top -= (14 - (to_rect.bottom - to_rect.top)) / 2 + 1;
					}
					else OffsetRect(&to_rect, 0,(to_rect.bottom - to_rect.top) / 6);
				//cd_erase_rect(dlog_num,to_rect);
				if (item_active[item_index] != 0) {
					SetTextColor(win_dc,PALETTEINDEX(c[3]));

					DrawText(win_dc, (char *) labels[item_label_loc[item_index]],
						-1, &to_rect, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
					SetTextColor(win_dc,PALETTEINDEX(c[0]));

					}
				}

	
	SelectObject(win_dc,old_font);
	cd_kill_dc(dlg_index,win_dc);
	dlg_force_dc = NULL;
	}

void cd_initial_draw(short dlog_num)
{
	short i,which_dlg = -1;
	
	for (i = 0; i < ND; i++)
		if ((dlg_keys[i] >= 0) && (dlg_types[i] == dlog_num))
			which_dlg = i;
	if (which_dlg < 0) {
		return;
		}
	dlg_draw_ready[which_dlg] = TRUE;

	cd_erase_item(dlog_num, 0);

		
	cd_draw(dlog_num);
}

void cd_draw(short dlog_num)
{
	short i,which_dlg = -1;

	for (i = 0; i < ND; i++)
		if ((dlg_keys[i] >= 0) && (dlg_types[i] == dlog_num))
			which_dlg = i;
	if (which_dlg < 0)
		return;
	for (i = 0; i < dlg_highest_item[which_dlg] + 1; i++) {
		cd_draw_item(dlog_num,i);
		}
}

void cd_redraw(HWND window)
{
	short which_dlg,dlg_num,dlg_key;

	if ((which_dlg = cd_find_dlog(window,&dlg_num,&dlg_key)) < 0)
		return;
	dlg_draw_ready[which_dlg] = TRUE;
	cd_initial_draw(dlg_num);
	for (short i = 0; i < 80; i++)
		if ((store_edit_parent[i] != NULL) &&
			(store_edit_parent_num[i] == dlg_num)) {

				RedrawWindow(edit_box[i],NULL,NULL,RDW_INVALIDATE | RDW_UPDATENOW);
				}

}

void cd_frame_item(short dlog_num, short item_num, short width)
{
	short dlg_index,item_index;

	if (cd_get_indices(dlog_num,item_num,&dlg_index,&item_index) < 0)
		return;
	frame_dlog_rect(dlgs[dlg_index], item_rect[item_index], width);
}

void cd_erase_item(short dlog_num, short item_num)
// if item_num is 0, nail whole window
// item_num + 100  just erase label
{
	short i,dlg_index = -1,item_index,store_label;
	RECT to_fry;
	HDC win_dc;
	Boolean just_label = FALSE;
//	RECT pict_rect = {6,6,43,43};

	if (item_num >= 100) {
		item_num -= 100;
		just_label = TRUE;
		}

	if (item_num == 0) {
		for (i = 0; i < ND; i++)
				if ((dlg_keys[i] >= 0) && (dlg_types[i] == dlog_num))
					dlg_index = i;
		GetWindowRect(dlgs[dlg_index],&to_fry);
		OffsetRect(&to_fry,-1 * to_fry.left,-1 * to_fry.top);
		}
	else {
		if (cd_get_indices(dlog_num,item_num,&dlg_index,&item_index) < 0)
			return;
		to_fry = item_rect[item_index];
		if (just_label == TRUE) {
			if (item_label[item_index] != 0) {
				store_label = item_label[item_index];
				if (store_label >= 1000)
					store_label -= 1000;

				switch (store_label / 100) {
					case 0:
						to_fry.right = to_fry.left;
						to_fry.left -= 2 * (store_label % 100);
						break;
					case 1:
						to_fry.bottom = to_fry.top;
						to_fry.top -= 2 * (store_label % 100);
						break;
					case 2:
						to_fry.left = to_fry.right;
						to_fry.right += 2 * (store_label % 100);
						break;
					case 3:
						to_fry.top = to_fry.bottom;
						to_fry.bottom += 2 * (store_label % 100);
						break;
					}
				if ((i = 12 - (short)(to_fry.bottom - to_fry.top)) > 0) {
					// adjust rect ... but doesn't work for bold letters
					to_fry.bottom += i / 2;
					to_fry.bottom++; // extra pixel to get dangly letters
					to_fry.top -= i / 2;
					}
				}
			}
		InflateRect(&to_fry,1,1);

		}
	if (dlg_draw_ready[dlg_index] == FALSE)
		return;
	win_dc = cd_get_dlog_dc(dlg_index);
/*	old_brush = SelectObject(win_dc,bg[4]);
	old_pen = SelectObject(win_dc,GetStockObject(NULL_PEN));
	Rectangle(win_dc,to_fry.left, to_fry.top,to_fry.right,to_fry.bottom);
	SelectObject(win_dc,old_brush);
	SelectObject(win_dc,old_pen);  */
	paint_pattern((HDIB) win_dc,2,to_fry,0);
		
	// Now 2nd part of frame processing
	if (item_num == 0) {
		GetClientRect(dlgs[dlg_index],&to_fry);
		place_dlog_borders_around_rect((HDIB) win_dc,dlgs[dlg_index],to_fry);
		}

	cd_kill_dc(dlg_index,win_dc);  

}

/*
void cd_erase_rect(short dlog_num,RECT to_fry)
{
	short dlg_index;
	HDC win_dc;

	if ((dlg_index = cd_get_dlg_index(dlog_num)) < 0)
		return;
	if (dlg_draw_ready[dlg_index] == FALSE)
		return;

	win_dc = cd_get_dlog_dc(dlg_index);

	paint_pattern((HDIB) win_dc,2,to_fry,0);
	cd_kill_dc(dlg_index,win_dc);

}
*/

void cd_press_button(short dlog_num, short item_num, EDLGBtnRes mode )
{
//	RGBColor c[2] = {{26265,18944,6272},{61184,52530,12544}};//{{0,0,4096},{0,0,8192}}; 206,148,49
	short dlg_index,item_index;
	RECT from_rect = {0,0,0,0};
//	RECT x = {0,0,55,55};
	COLORREF c[4] = {RGB(0,0,0),RGB(255,0,0),RGB(239,205,49),RGB(59,12,12)};
	HDC win_dc;
	char str[256],button_str[256];
	HFONT old_font;

	if (cd_get_indices(dlog_num,item_num,&dlg_index,&item_index) < 0)
		return;
	// no press action for redio buttons
	if ((dlog_item_type[item_index] == 2) || (item_flag[item_index] == 143)) {
		if ((mode == eDLGBtnResCompatible) || (mode == eDLGBtnResChange))
			play_sound(34);
		return;
	}
			
	win_dc = cd_get_dlog_dc(dlg_index);


	// change graphics of the button
	if ( (mode == eDLGBtnResCompatible) || (mode == eDLGBtnResChange) ){
		if (item_flag[item_index] == 1000) { // funky record button?
			//from_rect = record_button_from_rect;
			//OffsetRect(&from_rect,0,from_rect.bottom - from_rect.top);
			//rect_draw_some_item(mixed_gworld,from_rect,(HDIB) win_dc,item_rect[item_index],1,2);
		}
		else if (item_flag[item_index] > 1000) { // funky text button?
				SetTextColor(win_dc,c[3]);
				get_str(str,1,item_flag[item_index] % 1000);
				sprintf((char *) button_str,"%d - %s", (int)(item_flag[item_index] / 1000), str);
				char_win_draw_string(win_dc,item_rect[item_index],
					(char *) button_str,0,14);
		}
		else {
			from_rect = buttons_from[button_type[item_flag[item_index]]];
			OffsetRect(&from_rect,from_rect.right - from_rect.left,0);
			rect_draw_some_item(buttons_gworld,from_rect,
				(HDIB) win_dc,item_rect[item_index],0,2);

			old_font = (HFONT)SelectObject(win_dc,bold_font);
			SetTextColor(win_dc,c[1]);
			SelectObject(win_dc,old_font);

			if (dlog_item_type[item_index] < 2) {
				//OffsetRect(&item_rect[item_index],-1 * button_left_adj[item_flag[item_index]],0);
				char_win_draw_string(win_dc,item_rect[item_index],
					(char *) (button_strs[item_flag[item_index]]),1,8);
				//OffsetRect(&item_rect[item_index],button_left_adj[item_flag[item_index]],0);
			}
			else {
				char_win_draw_string(win_dc,item_rect[item_index],
					(char *) ((item_index < 10) ? text_long_str[item_index] : 
					text_short_str[item_index - 10]),1,8);
			}
			old_font = (HFONT)SelectObject(win_dc,bold_font);
			SetTextColor(win_dc,c[0]);
			SelectObject(win_dc,old_font);
		}
	}

	// sound
	if ( (mode == eDLGBtnResCompatible) || (mode == eDLGBtnResChange) )
		play_sound((item_flag[item_index] >= 1000) ? 1 : 37);

	// delay
	if ( mode == eDLGBtnResCompatible )
		pause(6);

	// recover graphics of the button
	if ( (mode == eDLGBtnResCompatible) || (mode == eDLGBtnResRecover) ) {
		if (item_flag[item_index] == 1000) { // funky record button?
			//rect_draw_some_item(mixed_gworld,record_button_from_rect,(HDIB) win_dc,item_rect[item_index],1,2);
		}
		else if (item_flag[item_index] > 1000) { // funky text button?
			SetTextColor(win_dc,RGB(0,0,0));
			get_str(str,1,item_flag[item_index] % 1000);
			sprintf((char *) button_str,"%d - %s", (int)(item_flag[item_index] / 1000), str);
			char_win_draw_string(win_dc,item_rect[item_index],
					(char *) button_str,0,14);
		}
		else {	
			OffsetRect(&from_rect,-1 * from_rect.right + from_rect.left,0);
			rect_draw_some_item(buttons_gworld,from_rect,
				(HDIB) win_dc,item_rect[item_index],0,2);

			old_font = (HFONT)SelectObject(win_dc,bold_font);
			SetTextColor(win_dc,c[2]);
			SelectObject(win_dc,old_font);
			if (dlog_item_type[item_index] < 2) {
				//OffsetRect(&item_rect[item_index],-1 * button_left_adj[item_flag[item_index]],0);
				char_win_draw_string(win_dc,item_rect[item_index],
					(char *) (button_strs[item_flag[item_index]]),1,8);
				//OffsetRect(&item_rect[item_index],button_left_adj[item_flag[item_index]],0);
			}
			else {
				char_win_draw_string(win_dc,item_rect[item_index],
					(char *) ((item_index < 10) ? text_long_str[item_index] : 
					text_short_str[item_index - 10]),1,8);
			}
			old_font = (HFONT)SelectObject(win_dc,bold_font);
			SetTextColor(win_dc,c[0]);
			SelectObject(win_dc,old_font);
		}
	}

	if ( mode == eDLGBtnResCompatible )
		pause(8);

	cd_kill_dc(dlg_index,win_dc);
}

// LOW LEVEL

short cd_get_indices(short dlg_num, short item_num, short *dlg_index, short *item_index)
{
	if ((*dlg_index = cd_get_dlg_index(dlg_num)) < 0) {
		return -1;
		}
	if ((*item_index = cd_get_item_id(dlg_num,item_num)) < 0) {
		return -1;
		}
	return 0;
		}

short cd_get_dlg_index(short dlog_num)
{
	short i;

	for (i = 0; i < ND; i++)
		if ((dlg_keys[i] >= 0) && (dlg_types[i] == dlog_num))
			return i;
	return -1;
}

short cd_find_dlog(HWND window, short *dlg_num, short *dlg_key)
{
	short i;
	for (i = 0; i < ND; i++)
		if ((dlg_keys[i] >= 0) && (dlgs[i] == window)) {
			*dlg_num = dlg_types[i];
			*dlg_key = dlg_keys[i];
			return i;
			}
	return -1;
}

short cd_get_item_id(short dlg_num, short item_num)
{
	short i;

	for (i = 0; i < NI; i++)
		if ((item_dlg[i] == dlg_num) && (item_number[i] == item_num))
			return i;
	return -1;
}

// External graphics tools (huh huh huh ... tool ... huh huh huh)

HDC cd_get_dlog_dc(short which_slot)

{
	HDC win_dc;

	win_dc = GetDC(dlgs[which_slot]);
	font_save = (HFONT)SelectObject(win_dc,bold_font);
	//SelectPalette(win_dc,hpal,0);
	SetBkMode(win_dc,TRANSPARENT);
	return win_dc;
	}

void cd_kill_dc(short which_slot,HDC hdc)
{
	SelectObject(hdc,font_save);
	fry_dc(dlgs[which_slot],hdc);
}

void center_window(HWND window)
{
	RECT main_rect,wind_rect;
	short width,height;

	GetWindowRect(GetDesktopWindow(),&main_rect);
	GetWindowRect(window,&wind_rect);
	width = (short)(wind_rect.right - wind_rect.left);
	height = (short)(wind_rect.bottom - wind_rect.top);
	MoveWindow(window,((main_rect.right - main_rect.left) - width) / 2,
		((main_rect.bottom - main_rect.top) - height) / 2,width,height,TRUE);

}

RECT get_item_rect(HWND hDlg, short item_num)
{
	HWND item;
	RECT big_rect,small_rect;

	item = GetDlgItem(hDlg, item_num);
	GetWindowRect(hDlg,&big_rect);
	GetWindowRect(item,&small_rect);
	OffsetRect(&small_rect, -1 * big_rect.left - 7, -1 * big_rect.top - 7);
	small_rect.right += 2;
	small_rect.bottom += 2;
	return small_rect;
}

void frame_dlog_rect(HWND hDlg, RECT rect, short val)
{
	HDC hdc;
	HPEN dpen,old_pen;
	COLORREF y = RGB(64,15,15);//x = RGB(64,15,15), y = RGB(119,119,119);
	Boolean keep_dc = FALSE;

	InflateRect(&rect,val,val);

	if (hDlg == mainPtr) {
		keep_dc = TRUE;
		hdc = main_dc;
		}
		else if (dlg_force_dc != NULL) {
		hdc = dlg_force_dc;
		keep_dc = TRUE;
		}
		else hdc = GetDC(hDlg);
	if (hdc == NULL) {
		beep();
		return;
		}
	//SelectPalette(hdc,hpal,0);
	//c = GetNearestPaletteIndex(hpal,y);
	dpen = CreatePen(PS_SOLID,1,y);
	old_pen = (HPEN) SelectObject(hdc,dpen);
	MoveToEx(hdc,rect.left,rect.top,NULL);
	LineTo(hdc,rect.right,rect.top);
	LineTo(hdc,rect.right,rect.bottom);
	LineTo(hdc,rect.left,rect.bottom);
	LineTo(hdc,rect.left,rect.top);
	SelectObject(hdc,old_pen);
	if (keep_dc == FALSE)
		fry_dc(hDlg,hdc);
	DeleteObject(dpen);

}

/*
void draw_dialog_graphic(HWND hDlg, RECT rect, short which_g, Boolean do_frame,short win_or_gworld)
// win_or_gworld: 0 - window  1 - gworld
// 0 - 599   number of terrain graphic
// 600 + x - monster graphic num
//   600 is 1st monster, 601 is 2nd , and so on
// 700 + x  dlog graphic
// 800 + x  pc graphic
// 900 + x  B&W graphic
// 950 null item
// 1000 + x  Talking face
// 1100 - item info help  
// 1200 - pc screen help  
// 1300 - combat ap
// 1400-1402 - button help
// 1403 - 1405 - death help
// 1500 - stat symbols help
// 1600 + x - scen graphics
// 1700 + x - anim graphic
// 1800 + x  item graphic
// 2100 + x - pc facial

{
	short picnum;
	
	RECT to_rect,pc_info_from = {0,101,98,130};//
	RECT item_info_from = {160,0,298,84};//
	RECT button_help_from = {0,0,548,154};//
	RECT combat_ap_from = {0,0,275,100};//
	RECT stat_symbols_from = {0,0,386,94};//
	RECT from_rect = {0,0,36,36},from2 = {0,0,36,36},from3 = {0,0,55,46};
	RECT item_from = {1,1,29,29};
	RECT portrait_from = {0,0,49,46};
	RECT death_from = {0,0,350,350};
	RECT pig_g_from = {0,0,127,127};
	RECT big_map_from = {0,0,240,240};
	HDC hdc;
	HBRUSH old_brush;
	HDIB from_gworld;	
	short draw_dest = 2;
	short m_start_pic = 0,square_size = 32;

	//this isn't ever used for this program, no pictures in dialog boxes
	return;
	
	if (win_or_gworld == 1)
		draw_dest = 0;
		
	if (which_g < 0)
		return;

	if (which_g >= 3000)
		do_frame = FALSE;
	which_g = which_g % 3000;
	
	if (win_or_gworld == 0) {
		if (dlg_force_dc != NULL)
			hdc = dlg_force_dc;
			else hdc = GetDC(hDlg);
		//SelectPalette(hdc,hpal,0);
		}
	
	if (which_g == 950) { // Empty. Maybe clear space.
		if (win_or_gworld == 0) {
			paint_pattern((HDIB) hdc,2,rect,1);

			}
		if (dlg_force_dc == NULL)
			fry_dc(hDlg, hdc);
		return;
		}

	switch (which_g / 100) {
		case 0: case 1: case 2: case 3: case 4: case 5: // terrain
			do_frame = FALSE;
			if (ter_gworld[which_g / 10] == NULL)
				ter_gworld[which_g / 10] = load_pict(700 + which_g / 10);
			from_gworld = ter_gworld[which_g / 10];
			which_g = which_g % 10;
			OffsetRect(&from_rect,6 + which_g * 47,18);

			rect_draw_some_item(from_gworld,from_rect,(HDIB) ((win_or_gworld == 1) ? (HDIB) (hDlg): (HDIB) (hdc))
			  ,rect,0,draw_dest);
			break;

		case 6: // monster
			do_frame = FALSE;
			which_g = which_g - 600;
			if (monst_graphics[which_g] == NULL)
				monst_graphics[which_g] = load_pict(1500 + which_g);
			from_gworld = monst_graphics[which_g];
			//OffsetRect(&from_rect,6 + 94,6);
			OffsetRect(&from_rect,1 + 94,1);
			rect.right = rect.left + 40;
			rect.bottom = rect.top + 49;
			from_rect.right = from_rect.left + 46;
			from_rect.bottom = from_rect.top + 55;
			MacInsetRect(&from_rect,3,3);
			rect_draw_some_item(from_gworld,from_rect,(HDIB) ((win_or_gworld == 1) ? (HDIB) (hDlg): (HDIB) (hdc))
			  ,rect,0,draw_dest);
			rect.left--; rect.top--;
			frame_dlog_rect(hDlg,rect,0);
			break;
		case 8: // pc
			//do_frame = FALSE;
			which_g = which_g - 800;
			from_gworld = load_pict(1650 + which_g);
			if (from_gworld == NULL)
				break;
			from_rect = from3;
			OffsetRect(&from_rect,1 + 282,1);
			rect.right = rect.left + (from_rect.right - from_rect.left);
			rect.bottom = rect.top + (from_rect.bottom - from_rect.top);
			rect_draw_some_item(from_gworld,from_rect,(HDIB) ((win_or_gworld == 1) ? (HDIB) (hDlg): (HDIB) (hdc))
			  ,rect,0,draw_dest);
			DibDelete(from_gworld);
			break;

		case 7: // dialog graphics
			do_frame = FALSE;
			which_g -= 700;
			from_gworld = load_pict(1703);
			from_rect = pig_g_from;
			OffsetRect(&from_rect,127 * (which_g % 3),127 * (which_g / 3));
			rect.right = rect.left + (from_rect.right - from_rect.left);
			rect.bottom = rect.top + (from_rect.bottom - from_rect.top);
			rect_draw_some_item(from_gworld,from_rect,(HDIB) ((win_or_gworld == 1) ? (HDIB)hDlg: (HDIB) (hdc))
			  ,rect,0,draw_dest);
			DibDelete(from_gworld);
			break;

		case 9: // big dialog map
			//do_frame = FALSE;
			which_g -= 900;
			from_gworld = load_pict(1750 + which_g);
			from_rect = big_map_from;
			rect.right = rect.left + (from_rect.right - from_rect.left);
			rect.bottom = rect.top + (from_rect.bottom - from_rect.top);
			rect_draw_some_item(from_gworld,from_rect,(HDIB) ((win_or_gworld == 1) ? (HDIB)hDlg: (HDIB) (hdc))
			  ,rect,0,draw_dest);
			DibDelete(from_gworld);
			break;
		
		case 18: case 19: case 20:
			do_frame = FALSE;
			which_g = which_g - 1800;
			if (item_graphics[which_g / 10] == NULL)
				item_graphics[which_g / 10] = load_pict(1000 + which_g / 10);
			from_gworld = item_graphics[which_g / 10];
			from_rect = item_from;
			OffsetRect(&from_rect,29 * (which_g % 10),0);
			to_rect = rect;
			to_rect.right = to_rect.left + 28;
			to_rect.bottom = to_rect.top + 28;
			MacInsetRect(&to_rect,-1,-1);
			MacInsetRect(&from_rect,-1,-1);
			rect_draw_some_item(from_gworld,from_rect,(HDIB) ((win_or_gworld == 1) ? (HDIB) (hDlg): (HDIB) (hdc))
			  ,to_rect,0,draw_dest);

			break;
		case 14: // button help
			which_g -= 1400;
			//if (which_g >= 10) {
			//	from_gworld = load_pict(900 + which_g);
			//	from_rect = large_scen_from;
			//	rect_draw_some_item(from_gworld,from_rect,(HDIB) ((win_or_gworld == 1) ? (HDIB)hDlg: from_gworld)
			//	  ,rect,0,draw_dest);
			//	DibDelete(from_gworld);
			//	break;
			//	}
			if (which_g <= 2) { // total size 584 x 462
				from_gworld = load_pict(1401);
				from_rect = button_help_from;
				rect.right = rect.left + from_rect.right;
				rect.bottom = rect.top + from_rect.bottom;
				OffsetRect(&from_rect,0,(from_rect.bottom - from_rect.top) * which_g);
				rect_draw_some_item(from_gworld,from_rect,(HDIB) ((win_or_gworld == 1) ? (HDIB)hDlg: (HDIB) (hdc))
				  ,rect,0,draw_dest);
				DibDelete(from_gworld);
				break;
				}
			if (which_g <= 5) {
				from_gworld = load_pict(5300 + which_g - 3);
				from_rect = death_from;
				rect.right = rect.left + from_rect.right;
				rect.bottom = rect.top + from_rect.bottom;
				rect_draw_some_item(from_gworld,from_rect,(HDIB) ((win_or_gworld == 1) ? (HDIB)hDlg: (HDIB) (hdc))
				  ,rect,0,draw_dest);
				DibDelete(from_gworld);
				//frame_dlog_rect(hDlg,rect,0);
				do_frame = FALSE;
				}
			break;
		case 13: // combat ap help  
			from_gworld = load_pict(1402);
			from_rect = combat_ap_from;
			rect.right = rect.left + from_rect.right;
			rect.bottom = rect.top + from_rect.bottom;
			rect_draw_some_item(from_gworld,from_rect,(HDIB) ((win_or_gworld == 1) ? (HDIB) (hDlg): (HDIB) (hdc))
			  ,rect,0,draw_dest);
			DibDelete(from_gworld);
			break;
		case 15: // stat symbols help  
			from_gworld = load_pict(1400);
			from_rect = stat_symbols_from;
			rect.right = rect.left + from_rect.right;
			rect.bottom = rect.top + from_rect.bottom;
			rect_draw_some_item(from_gworld,from_rect,(HDIB) ((win_or_gworld == 1) ? (HDIB) (hDlg): (HDIB) (hdc))
			  ,rect,0,draw_dest);
			DibDelete(from_gworld);
			break;
		case 21: // pc face
			do_frame = FALSE;
			which_g -= 2100;
			from_gworld = portraits_gworld;
			from2 = portrait_from;
			OffsetRect(&from2,46 * (which_g % 5),49 * (which_g / 5));
			rect_draw_some_item(from_gworld,from2,(HDIB) ((win_or_gworld == 1) ? (HDIB)hDlg: (HDIB) (hdc))
			  ,rect,0,draw_dest);
			break;
		case 11: // inven buttons
			do_frame = FALSE;
			which_g -= 1100;
			from_gworld = mixed_gworld;
			from_rect = item_info_from;
			rect.right = rect.left + (from_rect.right - from_rect.left);
			rect.bottom = rect.top + (from_rect.bottom - from_rect.top);			
			rect_draw_some_item(from_gworld,from_rect,(HDIB) ((win_or_gworld == 1) ? (HDIB)hDlg: (HDIB) (hdc))
			  ,rect,0,draw_dest);
			break;
		case 12: // pc area buttons
			do_frame = FALSE;
			which_g -= 1200;
			from_gworld = mixed_gworld;
			from_rect = pc_info_from;
			rect.right = rect.left + (from_rect.right - from_rect.left);
			rect.bottom = rect.top + (from_rect.bottom - from_rect.top);			
			rect_draw_some_item(from_gworld,from_rect,(HDIB) ((win_or_gworld == 1) ? (HDIB)hDlg: (HDIB) (hdc))
			  ,rect,0,draw_dest);
			break;

		}

	if ((win_or_gworld == 0) && (dlg_force_dc == NULL))
		fry_dc(hDlg, hdc);
	if (do_frame == TRUE) {
		rect.bottom--; rect.right--;
		frame_dlog_rect(hDlg,rect,3);
		}
}
*/

RECT get_template_from_rect(short x,short y)
{
	RECT base_rect = {1,1,47,56};//
	
	OffsetRect(&base_rect,47 * x,56 * y);
	return base_rect;
}

RECT get_template_from_medium_rect(short x,short y)
{
	RECT base_rect = {1,1,12,17};//

	OffsetRect(&base_rect,47 * x,56 * y);
	return base_rect;
}

/*
void showcursor(Boolean a)
{
	int i;
	
	i = ShowCursor(a);

	if (a == FALSE)
		while (i >= 0)
			i = ShowCursor(FALSE);
	if (a == TRUE)
		while (i < 0)
			i = ShowCursor(TRUE);
}
*/

void ModalDialog()
{
	MSG msg;

		while ((dialog_not_toast == TRUE) && (GetMessage(&msg,NULL,0,0))) {
			if (!TranslateAccelerator(mainPtr, accel, &msg)) {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
				}
			}

}
