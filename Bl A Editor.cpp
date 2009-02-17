// Blades of Avernum Code

// Welcome to the Blades of Avernum source code.

// I am providing this code because I suspect that some heavy users of the editor
// would like to customize it to their own tastes. Even if we at Spiderweb had
// the resources to place every desired feature in the editor, we would not
// be able to appeal to everyone's individual tastes. Thus the code. We hope
// that people will be able to make different flavors of editor to suit
// different sets of users.

// Before you get started, some things you should know.

// 1. This is just the editor code, not the game. You can't use this to add new calls
// or features to the game itself. Along these lines, you must make sure to preserve
// the data structures.

// 2. This is basically the same editor code, heavily modified, that I wrote 10 years
// ago. It is, in parts, clumsy, kludgy, and, in general,  written to my own personal tastes
// and style. The inevitable E-mails explaning to me how my coding would be better 
// if I did this or that will be read with interest and probably make no difference
// whatsoever. If I wrote this from scratch today, it would look much different.

// 3. Also, if I wrote this today, it would be commented much better than it is.
// I would like to comment this heavily, but I just don't have the time.

// Some files of great importance.

// global.h - This is where all of the data structures are stored.
// dlogool.c - This is the custom dialog engine. There are comments on this in
// that file.

#include "stdafx.h"
/*
#include <windows.h>

#include "stdlib.h"
#include "string.h"
#include "stdio.h"
#include "math.h"
*/

#include "Resource.h"
#include "global.h"
#include "version.h"

// Gloabl varialbes

HINSTANCE store_hInstance;

HWND	mainPtr;
HWND	right_sbar;
HACCEL	accel;

char szWinName[] = "Blades of Avernum dialogs";

RECT	windRect;
Boolean mouse_button_held = FALSE;
short cen_x, cen_y;

short ulx = 0, uly = 0;

short mode_count = 0;

// short old_depth = 16;

// q_3DModStart
Boolean change_made_town = FALSE;
Boolean change_made_outdoors = FALSE;
// q_3DModEnd

Boolean file_is_loaded = FALSE;

// DATA TO EDIT
scenario_data_type	scenario;
town_record_type	town;
big_tr_type			t_d;
outdoor_record_type current_terrain;
scen_item_data_type scen_data;

// outdoor border
//	the first index 0:top, 1:right, 2:bottom, 3:left, the second index x(bottom, top) or y(left, right) coordinate
// short borders[4][50];
// unsigned char border_floor[4][50];
// unsigned char border_height[4][50];

// q_3DModStart
outdoor_record_type border_terrains[3][3];
// q_3DModEnd

short cur_town;
location cur_out;
short dlg_units_x,dlg_units_y;

Boolean small_any_drawn = FALSE;
Boolean kill_next_win_char = FALSE;
 
// MAIN WHAT'S GOING ON VARIABLES
short current_drawing_mode = 0; // 0 - floor 1 - terrain 2 - height
short town_type = 0;  // 0 - big 1 - ave 2 - small
short current_height_mode = 0; // 0 - no autohills, 1 - autohills
Boolean editing_town = FALSE;

// q_3DModStart
short cur_viewing_mode = 10; // 0 - big icons 1 - small icons 10 - big 3D icons 11 - 3D view as in game
// short cur_viewing_mode = 0; // 0 - big icons 1 - small icons 10 - big 3D icons 11 - 3D view as in game
// q_3DModEnd

short overall_mode = 0;
// 0 - 9 - different errain painting modes
// 0 - neutral state, editing terrain/spaces with pencil
// 1 - large paintbrush
// 2 - small paintbrush
// 3 - large spray can
// 4 - small spray can
//
// 10 - 29 - editing rectangle modes:
// 10 - frame rectangle
// 11 - fill rectangle
// 16 - place special enc
// 17 - town boundaries
// 18 - swap wall types
// 19 - add walls
// 20 - set height rectangle (absolute)
// 21 - place text rectangle
// 22 - outdoor only - town entrance
// 24 - change height rectangle (delta)

// regular select modes
// 30 - place north entrance
// 31 - place west entrance 
// 32 - place south entrance 
// 33 - place east entrance

// 40 - select instance
// 41 - delete instance
// 46 - placing creature
// 47 - placing item
// 48 - pasting instance
// 49 - delete special enc
// 50 - set special enc
// 51 - GONE creating path
// 52 - GONE shift path point
// 53 - GONE insert path pt
// 54 - GONE delete path
// 55 - GONE toggle waypoint
// 56 - GONE delete waypoint
// 57 - place nav point
// 58 - delete nav point
// 59 - edit sign
// 60 - wandering monster pts
// 61 - blocked spot
// 62-66 - barrels, atc
// 67 - clean space
// 68 - place different floor stains
// 69 - edit town entrance
// 70 - place, edit terrain script
// 71 - place outdoor start point
// 72 - place town start point

// file selection editing files
short selected_item_number = -1;
// codes for selected items
// 7000 + x - creature x
// 9000 + x - terrain script x
// 11000 + x - items x
item_type copied_item;
creature_start_type copied_creature;
in_town_on_ter_script_type copied_ter_script;

// external gloabal variables

extern HDC main_dc;
extern HBITMAP main_bitmap;
extern short max_dim[3];
extern RECT terrain_buttons_rect;
extern RECT terrain_rects[264];

// local variables

char szAppName[] = "Blades of Avernum Scenario Editor";
char file_path_name[_MAX_PATH];
Boolean All_Done = FALSE;
Boolean window_in_front = TRUE;

RECT right_sbar_rect;

Boolean force_game_end = FALSE;


// Function prototype

void MyRegisterClass( HINSTANCE hInstance );
BOOL InitInstance( HINSTANCE hInstance, int nCmdShow );
LRESULT CALLBACK WndProc (HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam);
Boolean handle_menu (short, HMENU);
// void Handle_Activate();
void handle_menu_choice(short choice);
void handle_file_menu(int item_hit);
void handle_campaign_menu(int item_hit);
void handle_item_menu(int item_hit);
void handle_monst_menu(int item_hit);
void handle_edit_menu(int item_hit);
short check_cd_event(HWND hwnd,UINT message,WPARAM wparam,LPARAM lparam);
// void max_window(HWND window);
void check_colors();
void check_game_done();



//MW specified argument and return type.
int APIENTRY _tWinMain (HINSTANCE hInstance,
						HINSTANCE /* hPrevInstance */,
						LPSTR /* lpszCmdParam */,
						int nCmdShow)
{
	MSG msg;

/*
	// Check another instance of this App is running
	HANDLE hMutex = NULL;
	BOOL bPrevInstance = FALSE;

	hMutex = CreateMutex(
		NULL,						// no security attributes
		FALSE,						// initially not owned
		"MutexToBoA3DEditorApp");	// name of mutex
	if ( GetLastError() == ERROR_ALREADY_EXISTS )
		bPrevInstance = TRUE;
*/

	MyRegisterClass(hInstance);

	if (!InitInstance(hInstance, nCmdShow)) 
		return FALSE;

	accel = LoadAccelerators(hInstance, (LPCTSTR)IDC_BOA3DEDITOR);

	// Main message loop
	while(GetMessage(&msg,NULL,0,0)) {
		if (!TranslateAccelerator(msg.hwnd, accel, &msg)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

/*
	// Release mutex
	if ( hMutex )
		ReleaseMutex( hMutex );
*/
	return (int)msg.wParam;
}


void MyRegisterClass( HINSTANCE hInstance )
{
	WNDCLASSEX wndclass,wndclass2;

	wndclass.cbSize			= sizeof(WNDCLASSEX); 
	wndclass.style			= CS_HREDRAW | CS_VREDRAW | CS_BYTEALIGNWINDOW;
	wndclass.lpfnWndProc	= (WNDPROC)WndProc;
	wndclass.cbClsExtra		= 0;
	wndclass.cbWndExtra		= 0;
	wndclass.hInstance		= hInstance;
	wndclass.hIcon			= LoadIcon(wndclass.hInstance,(LPCTSTR)IDI_BOA3DEDITOR);
	wndclass.hCursor		= NULL;
	wndclass.hbrBackground	= (HBRUSH) GetStockObject(WHITE_BRUSH);
	wndclass.lpszMenuName	= (LPCTSTR)IDC_BOA3DEDITOR;
	wndclass.lpszClassName	= szAppName;
	wndclass.hIconSm		= NULL;

	RegisterClassEx(&wndclass);

	wndclass2.cbSize		= sizeof(WNDCLASSEX); 
	wndclass2.style			= CS_HREDRAW | CS_VREDRAW | CS_BYTEALIGNWINDOW;
	wndclass2.lpfnWndProc	= (WNDPROC)WndProc;
	wndclass2.cbClsExtra	= 0;
	wndclass2.cbWndExtra	= 0;
	wndclass2.hInstance		= hInstance;
	wndclass2.hIcon			= LoadIcon(wndclass2.hInstance,(LPCTSTR)IDI_BOA3DEDITOR);
	wndclass2.hCursor		= NULL;
	wndclass2.hbrBackground	= (HBRUSH) GetStockObject(WHITE_BRUSH);
	wndclass2.lpszMenuName	= NULL;
	wndclass2.lpszClassName	= szWinName;
	wndclass2.hIconSm		= NULL;

	RegisterClassEx(&wndclass2);
}

LRESULT CALLBACK folderErrMsgWndProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM /*lParam*/)
{
	switch (message)
	{
	case WM_INITDIALOG:
		return TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK) 
		{
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
		}
		break;
	}
	return FALSE;
}

BOOL InitInstance( HINSTANCE hInstance, int nCmdShow )
{
	mainPtr = CreateWindow (
		szAppName,
		"3D Blades of Avernum Scenario Editor " kVersion,
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
		0,
		0,
		772,
		601,
		NULL,
		NULL,
		hInstance,
		NULL);

	if ( !mainPtr )
		return FALSE;

	store_hInstance = hInstance;

	if ( !file_initialize() )
	{
		DialogBox( hInstance, (LPCTSTR)IDD_DIALOG1, mainPtr, (DLGPROC)folderErrMsgWndProc);
		return FALSE;
	}

	//center_window(mainPtr);
 	GetModuleFileName(hInstance,file_path_name,256);

	short seed = (short) GetCurrentTime();
	srand(seed);

	center_window(mainPtr);
	ShowWindow(mainPtr,nCmdShow);

	GetClientRect(mainPtr,&windRect);
	SetTimer(mainPtr,1,20,NULL);
	Set_up_win ();

	cen_x = 24; cen_y = 24;

	right_sbar_rect.top = 0;
	right_sbar_rect.left = terrain_buttons_rect.right + RIGHT_BUTTONS_X_SHIFT;
	right_sbar_rect.bottom = 22 * (TER_BUTTON_SIZE + 1) + 1;
	right_sbar_rect.right = terrain_buttons_rect.right + RIGHT_BUTTONS_X_SHIFT + 16;
	right_sbar = CreateWindow("scrollbar",NULL,
		WS_CHILD | WS_TABSTOP | SBS_VERT, right_sbar_rect.left + ulx,right_sbar_rect.top + uly,
		right_sbar_rect.right - right_sbar_rect.left,
		right_sbar_rect.bottom - right_sbar_rect.top,
		mainPtr,(HMENU) 1,(HINSTANCE) store_hInstance,(void *) NULL);

	cd_init_dialogs();

	DWORD d = GetDialogBaseUnits();
	dlg_units_x = (short) LOWORD(d);
	dlg_units_y = (short) HIWORD(d);

	init_scripts();
	reset_small_drawn();
	init_screen_locs();

	check_colors();
	update_item_menu();
	shut_down_menus(/* 0 */);

	load_sounds();
	init_warriors_grove();
	set_up_terrain_buttons();
	redraw_screen();
	ShowScrollBar(right_sbar,SB_CTL,TRUE);
//	UpdateWindow(mainPtr);

	return TRUE;
}

LRESULT CALLBACK WndProc (HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT ps;
	//BYTE huge * lpDibBits;
	POINT press;
	short handled = 0,which_sbar,sbar_pos = 0,old_setting;
	int min = 0, max = 0;
//	HMENU menu;
	POINT p;
	RECT r;

	switch (message) {
	case WM_KEYDOWN:
		if (hwnd != mainPtr) {
			check_cd_event(hwnd,message,wParam,lParam);
			}
			else {
				All_Done = handle_syskeystroke(wParam,lParam,&handled);
				}
		return 0;
		break;

	case WM_CHAR:
		if (hwnd != mainPtr)
			check_cd_event(hwnd,message,wParam,lParam);
			else {
				All_Done = handle_keystroke(wParam,lParam);
				}
		kill_next_win_char = FALSE;
		return 0;
		break;

	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
		if (hwnd != mainPtr)
			check_cd_event(hwnd,message,wParam,lParam);
		else {
			SetFocus(hwnd);
			press.x = LOWORD(lParam);
			press.y = HIWORD(lParam);

			All_Done = handle_action(press, wParam,lParam);
			check_game_done();
		}
		return 0;
		break;

	case WM_LBUTTONUP:
		mouse_button_held = FALSE;
		lockLatestStep();
		break;

/*
	case WM_RBUTTONDOWN:
		mouse_button_held = FALSE;
		cursor_stay();
		if (hwnd != mainPtr)
			check_cd_event(hwnd,message,wParam,lParam);
		else {
			SetFocus(hwnd);
			press = MAKEPOINT(lParam);

			All_Done = handle_action(press, wParam,lParam);
			check_game_done();
		}
		return 0;
		break;
*/

	case WM_TIMER:
		// first, mouse held?
		if ((wParam == 1) && (mouse_button_held == TRUE)) {
			GetCursorPos(&p);
			ScreenToClient(mainPtr,&p);
			All_Done = handle_action(p, 0,0);
			}
		// second, refresh cursor?
		if ((wParam == 1) && (overall_mode < 60) && (GetFocus() == mainPtr)) {
			GetClientRect(mainPtr,&windRect);

			GetCursorPos(&p);
			ScreenToClient(mainPtr,&p);
			GetClientRect(mainPtr,&r);
			if (POINTInRECT(p,r))
				restore_cursor();
			}
		break;

	case WM_PALETTECHANGED:
		 //palette_suspect = TRUE;
		 return 0;

	case WM_ACTIVATEAPP:
		break;

	case WM_ACTIVATE:
		if (hwnd == mainPtr) {
			if ((wParam == WA_ACTIVE) ||(wParam == WA_CLICKACTIVE)) {
				window_in_front = TRUE;
				 }
			if (wParam == WA_INACTIVE) {
				window_in_front = FALSE;
				}
			}
		return 0;


	case WM_MOUSEMOVE:
		if ((mouse_button_held == TRUE) && (hwnd == mainPtr)) {
			press.x = LOWORD(lParam);
			press.y = HIWORD(lParam);
			All_Done = handle_action(press, wParam,lParam);
			}
		//if (hwnd == mainPtr)
      	restore_cursor();
		return 0;

	case WM_ERASEBKGND:
		if (hwnd == mainPtr)
			return (LRESULT)1;		// Say we handled it.
		else
			return (LRESULT)0;

	case WM_PAINT:
		// put the buffer on the screen
		hdc = BeginPaint( hwnd, &ps );
/*
		GetClientRect( hwnd, &r );
		BitBlt( hdc, 0, 0, r.right, r.bottom, main_dc, 0, 0, SRCCOPY );
*/
		EndPaint( hwnd, &ps );

		if (hwnd != mainPtr)
			check_cd_event(hwnd,message,wParam,lParam);
		else
			redraw_screen();
		return 0;

	case WM_VSCROLL:
		which_sbar = (short)GetWindowLong((HWND) lParam, GWL_ID);
		switch (which_sbar) {
			case 1:
				sbar_pos = (short)GetScrollPos(right_sbar,SB_CTL);
				old_setting = sbar_pos;
				GetScrollRange(right_sbar,SB_CTL,&min,&max);
				switch (wParam ) {
					case SB_PAGEDOWN: sbar_pos += 21; break;
					case SB_LINEDOWN: sbar_pos++; break;
					case SB_PAGEUP: sbar_pos -= 21; break;
					case SB_LINEUP: sbar_pos--; break;
					case SB_TOP: sbar_pos = 0; break;
					case SB_BOTTOM: sbar_pos = (short)max; break;
					case SB_THUMBPOSITION:
					case SB_THUMBTRACK:
						sbar_pos = LOWORD(lParam);
						break;
					}
				if (sbar_pos < 0)
					sbar_pos = 0;
				if (sbar_pos > (short)max)
					sbar_pos = (short)max;
				SetScrollPos(right_sbar,SB_CTL,sbar_pos,TRUE);
				if (sbar_pos != old_setting) {
					set_up_terrain_buttons();
					place_right_buttons(/* 0 */);
//					update_main_screen();
				}
				break;
			}
	    SetFocus(mainPtr);
		return 0;

	case WM_COMMAND:
		if (hwnd == mainPtr) {

//			menu = GetMenu(mainPtr);
			handle_menu_choice((short) wParam);
			check_game_done();
			}
			else {
				check_cd_event(hwnd,message,wParam,lParam);
				}
		return 0;


	case WM_DESTROY:
		if (hwnd == mainPtr) {
			force_game_end = TRUE;
			}
		return 0;
	case WM_CLOSE:
		if (hwnd == mainPtr) {
			force_game_end = TRUE;
			}
		return 0;
	case WM_QUIT:
    	if (hwnd == mainPtr)
			force_game_end = TRUE;
		break;


	}
	check_game_done();
return DefWindowProc(hwnd,message,wParam,lParam);
}

/*
void Handle_Activate()
{
	restore_cursor();
}
*/

void handle_menu_choice(short choice)
{
	int menu_item;
	menu_item = choice;

	if (choice != 0) {
		set_cursor(0);
		switch (choice / 100) {
			case 0:
				handle_file_menu(menu_item % 100);
				break;
			case 1:
				handle_edit_menu(menu_item % 100);
				break;
			case 2: 
				handle_campaign_menu(menu_item % 100);
				break;
			case 3:
				handle_town_menu(menu_item % 100);
				break;
			case 4:
				handle_outdoor_menu(menu_item % 100);
				break;

			case 6: case 7: case 8: case 9: case 10:
				handle_item_menu(menu_item - 600);
				break;
			case 11: case 12: case 13: 
				handle_monst_menu(menu_item - 1100);
				break;
			case 15:
				fancy_choice_dialog(1062,0);
				break;
			case 16:				// patch for ctrl-<key> command
				handle_keystroke( choice % 100, 0 );
				break;
			}
		} 
}

void handle_file_menu(int item_hit)
{
	switch (item_hit) {
		case 1: // open
			if (change_made_town || change_made_outdoors) {
				if (save_check(858) == FALSE)
					break;							
			}
			load_campaign();
			if (file_is_loaded) {
				update_item_menu();
				purgeUndo();
				purgeRedo();
				redraw_screen();
				shut_down_menus(/* 0 */);
			}
			break;
		case 2: // save
			//modify_lists();
			save_campaign();
			break;
		case 3: // new scen
			if ((file_is_loaded) &&  (change_made_town || change_made_outdoors)) {
				give_error("You need to save the changes made to your scenario before you can create a new scenario.",
					"",0);
				return;
			}
			build_scenario();
			redraw_screen();
			break;
		case 4: // import blades
			if ((file_is_loaded) && (change_made_town || change_made_outdoors)) {
				give_error("You need to save the changes made to your scenario before you can import a Blades of Exile scenario.",
					"",0);
				return;
			}
			if (fancy_choice_dialog(876,0) == 2)
				break;
			import_blades_of_exile_scenario();
			break;
		case 6: // quit
			if (save_check(869) == FALSE)
				break;
			force_game_end = TRUE;
			break;
	}
}

void handle_campaign_menu(int item_hit)
{
	switch (item_hit) {
		case 1: // edit town
			small_any_drawn = FALSE;
			cen_x = max_dim[town_type] / 2; cen_y = max_dim[town_type] / 2;
			current_drawing_mode = current_height_mode = 0;
			editing_town = TRUE;
			set_up_terrain_buttons();
			shut_down_menus(/* 4 */);
			//set_string("Drawing mode","");
			DrawMenuBar(mainPtr);
			reset_drawing_mode();
			purgeUndo();
			purgeRedo();
			redraw_screen();
			break;
		case 2: // outdoor section
			small_any_drawn = FALSE;
			cen_x = 24; cen_y = 24;
			current_drawing_mode = current_height_mode = 0;
			editing_town = FALSE;
			set_up_terrain_buttons();
			//set_string("Drawing mode","");
			shut_down_menus(/* 2 */);
			clear_selected_copied_objects();
			DrawMenuBar(mainPtr);
			reset_drawing_mode();
			purgeUndo();
			purgeRedo();
			redraw_screen();
			break;
		case 3: // new town
			if (change_made_town || change_made_outdoors) {
				give_error("You need to save the changes made to your scenario before you can add a new town.",
					"",0);
				return;
			}
			if (scenario.num_towns >= 199) {
				give_error("You have reached the limit of 200 zones you can have in one campaign.",
					"",0);
				return;
			}
			new_town();
			reset_drawing_mode();
			cen_x = max_dim[town_type] / 2; cen_y = max_dim[town_type] / 2;
			purgeUndo();
			purgeRedo();
			redraw_screen();
			change_made_town = TRUE;
			break;
		case 5: // basic scen data
			edit_scen_details();
			break;
		case 6: // label icon
			edit_scen_intro_pic();
			break;
		case 7: // intro text 1
			edit_scen_intro(0);
			break;
		case 8: // intro text 2
			edit_scen_intro(1);
			break;
		case 9: // intro text 3
			edit_scen_intro(2);
			break;
		case 12: // reload scen script
			if (fancy_choice_dialog(871,0) == 2)
				break;
			char file_name[256];
			get_name_of_current_scenario(file_name);
			if (load_individual_scenario_data(file_name /*,TRUE */) == FALSE) {
				file_is_loaded = FALSE;
				return;
				}			
			update_item_menu();
			set_up_terrain_buttons();
			place_right_buttons(/* 0 */);
			break;
		case 13: // Clean Up Walls
			if (fancy_choice_dialog(873,0) == 2)
				break;
			clean_walls();
			redraw_screen();
			change_made_town = TRUE;
			break;
		case 14: //   Import Town
			clear_selected_copied_objects();
			if(import_boa_town()){
				change_made_town = TRUE;
				purgeUndo();
				purgeRedo();
				redraw_screen();
			}
			break;
		case 15: //   Import Outdoor Section
			clear_selected_copied_objects();
			if(import_boa_outdoors()){
				change_made_outdoors = TRUE;
				purgeUndo();
				purgeRedo();
				redraw_screen();
			}
			break;
		case 16: //   Set Variable Town Entry
			edit_add_town();
			change_made_town = TRUE;
			break;
		case 17: //   Edit Item Placement Shortcuts
			edit_item_placement();
			change_made_town = TRUE;
			break;
		case 18: //   Delete Last Town
			if (change_made_town || change_made_outdoors) {
				give_error("You need to save the changes made to your scenario before you can delete a town.",
					"",0);
				return;
			}
			if (scenario.num_towns == 1) {
				give_error("You can't delete the last town in a scenario. All scenarios must have at least 1 town.",
					"",0);
				return;
			}
			if (scenario.num_towns - 1 == cur_town) {
				give_error("You can't delete the last town in a scenario while you're working on it. Load a different town, and try this again.",
					"",0);
				return;
			}
			if (fancy_choice_dialog(865,0) == 1) {
				delete_last_town();
				clear_selected_copied_objects();
				purgeUndo();
				purgeRedo();
				redraw_screen();
				change_made_town = TRUE;
			}
			break;
		case 19: //   Write Scenario Data to Text File
			if (fancy_choice_dialog(866,0) == 1) {
				start_data_dump();
			}
			break;
		case 20: //   Change Outdoor Size
			if (change_made_town || change_made_outdoors) {
				give_error("You need to save the changes made to your scenario before you can change the outdoor size.",
					"",0);
				return;
			}
			if(change_outdoor_size()) {
				reset_drawing_mode();
				purgeUndo();
				purgeRedo();
				redraw_screen();
			}
			break;
		}
}

void handle_town_menu(int item_hit)
{
	short i,x;	

	switch (item_hit) {
		case 1: // laod new town
			if (change_made_town == TRUE) {
				if (save_check(859) == FALSE)
					break;							
			}
			x =  get_a_number(855,cur_town,0,scenario.num_towns - 1);
			if (x >= 0) {
				load_town( x );
				clear_selected_copied_objects();
				set_up_terrain_buttons();
				change_made_town = FALSE;
				cen_x = max_dim[town_type] / 2; cen_y = max_dim[town_type] / 2;
				reset_drawing_mode();
				reset_small_drawn();
				purgeUndo();
				purgeRedo();
				redraw_screen();
			}
			break;
// q_3DModStart
		case 2: edit_town_details(); change_made_town = TRUE; break; // Town Details

		case 3: edit_town_wand(); change_made_town = TRUE; break; //Town Wandering Monsters
// q_3DModEnd

		case 4: // Set Town Boundaries							
			overall_mode = 17;
			mode_count = 2;
			set_cursor(5);
			set_string("Set town boundary","Select upper left corner");
			break;
// q_3DModStart
		case 5: frill_terrain(); change_made_town = TRUE; break;
		case 6: unfrill_terrain(); change_made_town = TRUE; break;
// q_3DModEnd
		case 7:  //Edit Area Descriptions
			 edit_town_strs();
			 break;
			
		case 9: //Set Starting Location
			if (fancy_choice_dialog(867,0) == 2)
					break;
			set_string("Set Town Start Point","Where will party start?");
			overall_mode = 72;
			set_cursor(7);
			break;

			
		case 11: if (fancy_choice_dialog(863,0) == 2)
					break;
				place_items_in_town();
				change_made_town = TRUE; 
				redraw_screen();
				 break; // add random
		case 12: for (i = 0; i < 144; i++)
					town.preset_items[i].properties = town.preset_items[i].properties & 253;
				fancy_choice_dialog(861,0);
				draw_terrain();
				change_made_town = TRUE; 
				break; // set not prop
		case 13: for (i = 0; i < 144; i++)
					town.preset_items[i].properties = town.preset_items[i].properties | 2;
				fancy_choice_dialog(877,0);
				draw_terrain();
				change_made_town = TRUE; 
				break; // set  prop
		case 14:
			if (fancy_choice_dialog(862,0) == 2)
				break;
			for (i = 0; i < 144; i++)
				town.preset_items[i].which_item = -1;
			draw_terrain();
			change_made_town = TRUE; 
			redraw_screen();
			break; // clear all items

		case 16: 
			if (fancy_choice_dialog(878,0) == 2)
				break;
			for (i = 0; i < 80; i++)
				town.creatures[i].number = -1;
			draw_terrain();
			change_made_town = TRUE; 
			redraw_screen();
			break;
		case 17: // clear special encs
			if (fancy_choice_dialog(877,0) == 2)
				break;
			for (x = 0; x < NUM_TOWN_PLACED_SPECIALS; x++) {
				town.spec_id[x] = kNO_TOWN_SPECIALS;
				SetMacRect(&town.special_rects[x],-1,-1,-1,-1);
				}
			change_made_town = TRUE; 
			redraw_screen();
			break;	


		}
}
void handle_outdoor_menu(int item_hit)
{
	short x;
				
	switch (item_hit) {
		case 1: // load new
			if (change_made_outdoors == TRUE) {
				if (save_check(859) == FALSE)
					break;
			}
			x = pick_out(cur_out,scenario.out_width,scenario.out_height);
			if (x >= 0) {
				location spot_hit = {(t_coord)(x / 100),(t_coord)(x % 100)};
				clear_selected_copied_objects();
			//	load_outdoors(spot_hit,0);
				load_outdoor_and_borders(spot_hit);
				set_up_terrain_buttons();
				cen_x = 24; cen_y = 24;
				reset_drawing_mode();
				purgeUndo();
				purgeRedo();
				redraw_screen();
				change_made_outdoors = FALSE;
			}
			break;
		case 2:							//Outdoor Details
				outdoor_details();
				set_up_terrain_buttons();
				draw_main_screen();
				change_made_outdoors = TRUE;
				break;

		case 3: edit_out_wand(0); change_made_outdoors = TRUE; break;
		case 4: edit_out_wand(1);  change_made_outdoors = TRUE; break;
		case 5: edit_out_wand(2);
				draw_terrain();
				change_made_outdoors = TRUE;
				break;

		case 6: frill_terrain(); change_made_outdoors = TRUE; break;
		case 7: unfrill_terrain(); change_made_outdoors = TRUE; break;
		case 8: edit_out_strs(); change_made_outdoors = TRUE; break; //Edit Area Descriptions
		case 10: 					//Set Starting Location
			if (fancy_choice_dialog(864,0) == 2)
				return;
			set_string("Set Outdoor Start Point","Where will party start?");
			overall_mode = 71;
			set_cursor(7);
			break;
		}
}

void handle_edit_menu(int item_hit)
{   
	switch (item_hit) {
		case 1: // undo
			performUndo();
			break;
		case 2: // redo
			performRedo();
			break;	
		case 4: // cut
			cut_selected_instance();
			if(editing_town)
				change_made_town = TRUE;
			else
				change_made_outdoors = TRUE;
			break;
		case 5: // copy
			copy_selected_instance();
			if(editing_town)
				change_made_town = TRUE;
			else
				change_made_outdoors = TRUE;
			break;
		case 6: // paste

			if ((copied_creature.exists() == FALSE) && (copied_item.exists() == FALSE) && (copied_ter_script.exists == FALSE)) {
				beep();
				break;
				}
			set_string("Paste copied instance","Select location to place");
			set_cursor(7);
			overall_mode = 48;			
			break;
		case 7: // clear
			delete_selected_instance();
			if(editing_town)
				change_made_town = TRUE;
			else
				change_made_outdoors = TRUE;
			break;	
		}
	draw_main_screen();		
}


void handle_item_menu(int item_hit)
{

	//if (same_string("Unused",scen_data.scen_items[item_hit].full_name)) {
	//	give_error("You can't place an item named 'Unused'.","",0);
	//	return;
	//	}
	overall_mode = 47;
	mode_count = (short)item_hit;
	set_string("Place New Item","Select location to place");

}

void handle_monst_menu(int item_hit)
{
	if (same_string("Unused",scen_data.scen_creatures[item_hit].name)) {
		give_error("You can't place an creature named 'Unused'.","",0);
		return;
		}
	overall_mode = 46;
	mode_count = (short)item_hit;
	set_string("Place New Creature","Select location to place");
}

void check_colors()
{
	int a,b;

	a = GetDeviceCaps(main_dc,BITSPIXEL);
	b = GetDeviceCaps(main_dc,PLANES);
	if (a * b < 16) {
		MessageBox(mainPtr,"The Blades of Avernum editor requires at least 16 bit color. The current graphics device is set for less."	,
	  "Not 16 Bit color!",MB_OK | MB_ICONEXCLAMATION);
	  }
}

/*
void max_window(HWND window)
{
	RECT main_rect,wind_rect;
	short width,height;

	GetWindowRect(GetDesktopWindow(),&main_rect);
	GetWindowRect(window,&wind_rect);
	width = main_rect.right - main_rect.left;
	height = main_rect.bottom - main_rect.top;
	MoveWindow(window,0,0,width,height,TRUE);

}
*/

short check_cd_event(HWND hwnd,UINT message,WPARAM wparam,LPARAM lparam)
{	
	POINT press;
	short wind_hit = -1,item_hit = -1;
	short low;

	switch (message) {
		case WM_COMMAND:
			// pare out command messages sent from text box
			low = LOWORD(wparam);
			if (LOWORD(wparam) == 150)
         		break;
			cd_find_dlog(hwnd,&wind_hit,&item_hit); // item_hit is dummy
			item_hit = (short) wparam;
			break;
		case WM_KEYDOWN:
			if ((wparam >= 150) && (wparam <= 250)) {
					return -1;
				}
			wind_hit = cd_process_syskeystroke(hwnd, wparam,/* lparam,*/ &item_hit);
			break;

		case WM_CHAR:
			wind_hit = cd_process_keystroke(hwnd, wparam,/* lparam,*/ &item_hit);
			break;

		case WM_LBUTTONDOWN:
			press.x = LOWORD(lparam);
			press.y = HIWORD(lparam);
			wind_hit = cd_process_click(hwnd,press, wparam,/* lparam,*/ &item_hit);
			break;
		case WM_RBUTTONDOWN:
			press.x = LOWORD(lparam);
			press.y = HIWORD(lparam);
			wparam = wparam | MK_CONTROL;
			wind_hit = cd_process_click(hwnd,press, wparam,/* lparam,*/ &item_hit);
			break;
		case WM_MOUSEMOVE:
			restore_cursor();
			break;
		case WM_PAINT:
			cd_redraw(hwnd);
			break;
		}
	if (wind_hit < 0)
		return 0;
	switch (wind_hit) {
		case -1: break;
		case 970: case 971: case 972: case 973: display_strings_event_filter(item_hit); break;
		case 857: change_ter_event_filter(item_hit); break;
		case 800: edit_make_scen_1_event_filter(item_hit); break;
		case 801: edit_make_scen_2_event_filter(item_hit); break;
		case 803: edit_scen_details_event_filter(item_hit); break;
		case 804: edit_scen_intro_event_filter(item_hit); break;
		case 805: edit_scen_intro_pic_event_filter(item_hit); break;
		case 810: edit_add_town_event_filter(item_hit); break;
		case 812: edit_item_placement_event_filter(item_hit); break;
		case 820: choose_text_res_event_filter(item_hit); break;

		case 825: edit_special_num_event_filter(item_hit); break;
		case 828: how_many_dlog_event_filter(item_hit); break;
		case 829: get_str_dlog_event_filter(item_hit); break;
		case 830: new_town_event_filter(item_hit); break;
		case 832: edit_town_details_event_filter(item_hit); break;
		case 835: edit_town_wand_event_filter(item_hit); break;
		case 837: edit_placed_monst_event_filter(item_hit); break;
		case 839: edit_town_strs_event_filter(item_hit); break;
		case 840: edit_area_rect_event_filter(item_hit); break;
		case 841: pick_import_town_event_filter(item_hit); break;
		case 850: edit_out_strs_event_filter(item_hit); break;
		case 851: outdoor_details_event_filter(item_hit); break;
		case 852: edit_out_wand_event_filter(item_hit); break;
		case 854: pick_out_event_filter(item_hit); break;
		case 855: case 856: case 872: get_a_number_event_filter(item_hit); break;

// q_3DModStart
		case 991: change_outdoor_size_event_filter(item_hit); break;
// q_3DModEnd

		default: fancy_choice_dialog_event_filter (item_hit); break;

		}
	return 0;
}

Boolean game_killed_once = FALSE;
void check_game_done()
{
	if (game_killed_once)
		return;
	//through_sending();
	if (force_game_end == TRUE) {
		lose_graphics();
		game_killed_once = TRUE;
   		PostQuitMessage(0);
		}
}
