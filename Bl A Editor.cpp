// Blades of Avernum Code

// Welcome to the Blades of Avernum source code.

// I am providing this code because I suspect that some heavy users of the editor
// would like to customize it to their own tastes. Even if we at Spiderweb had
// the resources to place every desired feature in the editor, we would not
// be able to appeal to everyone's individual tastes. Thus the code. We hope
// that people will be able to make different flavors of editor to suit
// different sets of users.

#include "stdafx.h"
#include "Resource.h"
#include "global.h"
#define kVersion "   Version:  Chivalrous Chitrach (r106)"

// Global variables

HINSTANCE store_hInstance;

HWND	mainPtr;
HWND	right_sbar;
HACCEL	accel;

HWND	palettePtr;//this is the window which has the tools in it
HWND	palette_tooltips;
HWND	main_tooltips;
HWND	tile_tooltips;
HWND	tilesPtr;//this is the window with the terrain etc. tiles in it

char szTilesName[] = "Tiles";
char szPaletteName[] = "Tools";
char szWinName[] = "Blades of Avernum dialogs";

RECT	windRect;
Boolean mouse_button_held = FALSE;
short cen_x, cen_y;
short ulx = 0, uly = 0;
short mode_count = 0;

extern RECT terrain_rect_gr_size;
extern RECT terrain_viewport_3d;
extern HDIB ter_draw_gworld;
extern void delete_graphic(HDIB *to_delete);
extern HDIB DibCreate (int cx, int cy, int cBits, int cColors);
extern RECT kRect3DEditScrn;
extern void update_screen_locs(void);

extern Boolean play_sounds;
extern void write_should_play_sounds(bool play);
extern bool get_should_play_sounds();

Boolean change_made_town = FALSE;
Boolean change_made_outdoors = FALSE;
Boolean file_is_loaded = FALSE;

// DATA TO EDIT
scenario_data_type	scenario;
town_record_type	town;
big_tr_type			t_d;
outdoor_record_type current_terrain;
scen_item_data_type scen_data;
zone_names_data_type zone_names;

// outdoor border
//	the first index 0:top, 1:right, 2:bottom, 3:left, the second index x(bottom, top) or y(left, right) coordinate
// short borders[4][50];
// unsigned char border_floor[4][50];
// unsigned char border_height[4][50];

outdoor_record_type border_terrains[3][3];

short cur_town;
location cur_out;
short dlg_units_x,dlg_units_y;
Boolean small_any_drawn = FALSE;
Boolean kill_next_win_char = FALSE;
 
// MAIN WHAT'S GOING ON VARIABLES
short current_drawing_mode = 0; // 0 - floor 1 - terrain 2 - height
short town_type = 0;  // 0 - big 1 - ave 2 - small
short current_height_mode = 0; // 0 - no autohills, 1 - autohills
Boolean editing_town = TRUE;
short numerical_display_mode = 0;
short object_display_mode = 0;
short last_large_mode = 0;
short cur_viewing_mode = 10;
// 0 - big icons = 9*9 view
// 1 - small icons = 64*64 zoom-out view
// 2 - medium icons = 32*32 view
// 10 - big 3D icons
// 11 - 3D view as in game
int tileZoomLevel = 1;

short overall_mode = 0;
// 0 - 9 - different terrain painting modes
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
// 73 - eyedropper
// 74 - make oblique mirror
// 75 - make facing mirror(
// 76 - make_boat
// 77 - make_horse

// file selection editing files
short selected_item_number = -1;
// codes for selected items
//  7000 + x - creature x
//  9000 + x - terrain script x
// 11000 + x - items x
// 13000 + x - placed specials x
// 14000 + x - area descriptions x
// 15000 + x - town entrances x
// 16000 + x - sign x
// 17000 + x - wandering location x
// 18000 + x - town: preset field x/ outdoor: preset encounter
// 19000 + x - town: waypoint x

item_type copied_item;
creature_start_type copied_creature;
in_town_on_ter_script_type copied_ter_script;

// external global variables
extern char hintbook_mode8;
extern HDC main_dc;
extern HBITMAP main_bitmap;
extern short max_zone_dim[3];
extern RECT terrain_buttons_rect;
extern RECT terrain_rects[330];
extern Boolean use_custom_name;

extern Boolean setUpCreaturePalette;
extern Boolean setUpItemPalette;
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
void handle_help_menu(int item_hit);
void handle_tiles_menu(int item_hit);
short check_cd_event(HWND hwnd,UINT message,WPARAM wparam,LPARAM lparam);
// void max_window(HWND window);
void check_colors();
void check_game_done();
short last_file_printed = 0;
extern char grid_mode;

// extern void put_placed_terrain_script_in_dlog();
// MW specified argument and return type.
int APIENTRY _tWinMain (HINSTANCE hInstance,
						HINSTANCE /* hPrevInstance */,
						LPSTR /* lpszCmdParam */,
						int nCmdShow)
{
	MSG msg;
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
	return (int)msg.wParam;
}

void MyRegisterClass( HINSTANCE hInstance )
{
	WNDCLASSEX wndclass,wndclass2,wndclass3,wndclass4;

	//Main Window

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

	//Dialogue Windows

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

	//Floor Tiles Window
	wndclass3.cbSize			= sizeof(WNDCLASSEX); 
	wndclass3.style			= CS_HREDRAW | CS_VREDRAW | CS_BYTEALIGNWINDOW;
	wndclass3.lpfnWndProc	= (WNDPROC)WndProc;
	wndclass3.cbClsExtra		= 0;
	wndclass3.cbWndExtra		= 0;
	wndclass3.hInstance		= hInstance;
	wndclass3.hIcon			= NULL;
	wndclass3.hCursor		= NULL;
	wndclass3.hbrBackground	= (HBRUSH) GetStockObject(WHITE_BRUSH);
	wndclass3.lpszMenuName	= NULL;
	wndclass3.lpszClassName	= szTilesName;
	wndclass3.hIconSm		= NULL;

	RegisterClassEx(&wndclass3);

	wndclass4.cbSize			= sizeof(WNDCLASSEX); 
	wndclass4.style			= CS_HREDRAW | CS_VREDRAW | CS_BYTEALIGNWINDOW;
	wndclass4.lpfnWndProc	= (WNDPROC)WndProc;
	wndclass4.cbClsExtra		= 0;
	wndclass4.cbWndExtra		= 0;
	wndclass4.hInstance		= hInstance;
	wndclass4.hIcon			= NULL;
	wndclass4.hCursor		= NULL;
	wndclass4.hbrBackground	= (HBRUSH) GetStockObject(WHITE_BRUSH);
	wndclass4.lpszMenuName	= NULL;
	wndclass4.lpszClassName	= szPaletteName;
	wndclass4.hIconSm		= NULL;

	RegisterClassEx(&wndclass4);
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
	RECT main_rect;

	mainPtr = CreateWindow (
		szAppName,
		"3D Blades of Avernum Variant Scenario Editor " kVersion,
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_SIZEBOX,
		0,
		0,
		546,
		601, // was originally 601, then 690
		NULL,
		NULL,
		hInstance,
		NULL);

	tilesPtr = CreateWindow ( szTilesName, "Tiles", WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_SIZEBOX, 0, 0, 280, 435, mainPtr, NULL, hInstance, NULL);

	palettePtr = CreateWindow ( szPaletteName, "Tools", WS_OVERLAPPED | WS_CAPTION | WS_MINIMIZEBOX, 0, 0, 240, 225, mainPtr, NULL, hInstance, NULL);

	if ( !mainPtr )
		return FALSE;

	store_hInstance = hInstance;

	if ( !file_initialize() )
	{
		DialogBox( hInstance, (LPCTSTR)IDD_DIALOG1, mainPtr, (DLGPROC)folderErrMsgWndProc);
		return FALSE;
	}

	// center_window(mainPtr);
 	GetModuleFileName(hInstance,file_path_name,256);

	short seed = (short) GetCurrentTime();
	srand(seed);

	center_window(mainPtr);
	ShowWindow(mainPtr,nCmdShow);

	GetWindowRect(GetDesktopWindow(),&main_rect);

	MoveWindow(tilesPtr,((main_rect.right - main_rect.left) + 546) / 2,
		((main_rect.bottom - main_rect.top) - 650) / 2,TILES_WINDOW_WIDTH,435,TRUE);
//	center_window(tilesPtr);
	ShowWindow(tilesPtr,nCmdShow);

	MoveWindow(palettePtr,((main_rect.right - main_rect.left) + 546) / 2,
		((main_rect.bottom - main_rect.top) - 650) / 2 + 435,240,225,TRUE);
	ShowWindow(palettePtr,nCmdShow);

	GetClientRect(mainPtr,&windRect);
	SetTimer(mainPtr,1,20,NULL);
	Set_up_win ();

	cen_x = 24; cen_y = 24;

	right_sbar_rect.top = RIGHT_BUTTONS_Y_SHIFT;;
	right_sbar_rect.left = terrain_buttons_rect.right + RIGHT_BUTTONS_X_SHIFT;
	right_sbar_rect.bottom = 22 * (TER_BUTTON_SIZE_OLD + 1) + 1 + RIGHT_BUTTONS_Y_SHIFT;
	right_sbar_rect.right = terrain_buttons_rect.right + RIGHT_BUTTONS_X_SHIFT + 16;
	right_sbar = CreateWindow("scrollbar",NULL,
		WS_CHILD | WS_TABSTOP | SBS_VERT, right_sbar_rect.left + ulx,right_sbar_rect.top + uly,
		right_sbar_rect.right - right_sbar_rect.left,
		right_sbar_rect.bottom - right_sbar_rect.top,
		tilesPtr,(HMENU) 1,(HINSTANCE) store_hInstance,(void *) NULL);
	SetScrollRange(right_sbar,SB_CTL,0,get_right_sbar_max(),TRUE);

	cd_init_dialogs();

	DWORD d = GetDialogBaseUnits();
	dlg_units_x = (short) LOWORD(d);
	dlg_units_y = (short) HIWORD(d);

	init_scripts();
	reset_small_drawn();
	init_screen_locs();

	check_colors();
	update_item_menu();
	play_sounds = get_should_play_sounds();
	shut_down_menus(/* 0 */);

	//add extra menus to tiles menu
	HMENU menu = GetSystemMenu(tilesPtr,false);
	RemoveMenu(menu,6,MF_BYPOSITION);
	RemoveMenu(menu,4,MF_BYPOSITION);
	RemoveMenu(menu,3,MF_BYPOSITION);
	RemoveMenu(menu,0,MF_BYPOSITION);
	AppendMenu(menu,MF_GRAYED|MF_DISABLED|MF_STRING,1701,"Zoom In\tCtrl+");
	AppendMenu(menu,MF_GRAYED|MF_DISABLED|MF_STRING,1702,"Zoom Out\tCtrl-");

	shut_down_tile_menus();

	load_sounds();
	init_warriors_grove();
	set_up_terrain_buttons();
	redraw_screen();
	ShowScrollBar(right_sbar,SB_CTL,TRUE);
//	UpdateWindow(mainPtr);
	UpdateWindow(tilesPtr);
	UpdateWindow(palettePtr);

	return TRUE;
}

extern bool handle_scroll( int map_size, int scrl, bool ctrl_key, bool shft_key );

LRESULT CALLBACK WndProc (HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT ps;
	//BYTE huge * lpDibBits;
	POINT press;
	short handled = 0,which_sbar,sbar_pos = 0,old_setting;
	int min = 0, max = 0;
//	HMENU menu;
	int tool_index;
	POINT p;
	RECT r, wheelHitRect;
	int wheel_delta, wheel_keystate;

	char *tool_string_2d[2] = {"Toggle 3D", "Toggle Zoom"};
	char *tool_string_3d[2] = {"Toggle 3D", "Toggle Realistic Mode"};

	switch (message) {

	case WM_NOTIFY:
		//whee dynamic tooltips
		if (hwnd == mainPtr){
			switch (((LPNMHDR)lParam)->code){
				case TTN_GETDISPINFO:
					if (cur_viewing_mode >= 10){
						((LPNMTTDISPINFO)lParam)->lpszText = TEXT(tool_string_3d[((LPNMTTDISPINFO)lParam)->lParam]);
					}
					else {
						((LPNMTTDISPINFO)lParam)->lpszText = TEXT(tool_string_2d[((LPNMTTDISPINFO)lParam)->lParam]);
					}
					break;
			}
		}
		else if (hwnd == tilesPtr){
			switch (((LPNMHDR)lParam)->code){
				case TTN_GETDISPINFO:
					tool_index = ((LPNMTTDISPINFO)lParam)->lParam;
					switch (current_drawing_mode) {
						case 0:
							if (!((scen_data.scen_floors[tool_index].pic.which_sheet < 0)||(scen_data.scen_floors[tool_index].pic.which_icon < 0)||(tool_index>255)))
							{
								((LPNMTTDISPINFO)lParam)->lpszText = TEXT(scen_data.scen_floors[tool_index].floor_name);
							}
						break;
						case 1:
							if (!((scen_data.scen_ter_types[tool_index].pic.which_sheet < 0)||(scen_data.scen_ter_types[tool_index].pic.which_icon < 0)))
							{
								((LPNMTTDISPINFO)lParam)->lpszText = TEXT(scen_data.scen_ter_types[tool_index].ter_name);
							}
						break;
					}
					break;
			}
		}
		break;
	case WM_MOUSEHWHEEL:
		if (hwnd == mainPtr){
			wheelHitRect = terrain_viewport_3d;
			wheel_delta = GET_WHEEL_DELTA_WPARAM(wParam);
			press.x = GET_X_LPARAM(lParam);
			press.y = GET_Y_LPARAM(lParam);
			ScreenToClient(mainPtr, &press);
			wheel_keystate = GET_KEYSTATE_WPARAM(wParam);
			if (!POINTInRECT(press, wheelHitRect))
				break;
			if (wheel_delta > 0)
				handle_scroll((editing_town) ? max_zone_dim[town_type] : 48, eSCRL_Left, (wheel_keystate & MK_CONTROL), FALSE);
			else if (wheel_delta < 0)
				handle_scroll((editing_town) ? max_zone_dim[town_type] : 48, eSCRL_Right, (wheel_keystate & MK_CONTROL), FALSE);
		}
		break;
	case WM_MOUSEWHEEL:
		if (hwnd == mainPtr){
			wheelHitRect = terrain_viewport_3d;
			wheel_delta = GET_WHEEL_DELTA_WPARAM(wParam);
			press.x = GET_X_LPARAM(lParam);
			press.y = GET_Y_LPARAM(lParam);
			ScreenToClient(mainPtr, &press);
			wheel_keystate = GET_KEYSTATE_WPARAM(wParam);
			if (!POINTInRECT(press, wheelHitRect))
				break;
			if (wheel_delta > 0)
				handle_scroll((editing_town) ? max_zone_dim[town_type] : 48, (wheel_keystate & MK_SHIFT) ? eSCRL_Left : eSCRL_Top, (wheel_keystate & MK_CONTROL), FALSE);
			else if (wheel_delta < 0)
				handle_scroll((editing_town) ? max_zone_dim[town_type] : 48, (wheel_keystate & MK_SHIFT) ? eSCRL_Right : eSCRL_Bottom, (wheel_keystate & MK_CONTROL), FALSE);
		}
		else if (hwnd == tilesPtr){
			wheelHitRect = terrain_buttons_rect;
			wheelHitRect.bottom = right_sbar_rect.bottom;
			wheel_delta = GET_WHEEL_DELTA_WPARAM(wParam);
			press.x = GET_X_LPARAM(lParam);
			press.y = GET_Y_LPARAM(lParam);
			ScreenToClient(tilesPtr, &press);
			if (!POINTInRECT(press, wheelHitRect))
				break;
			sbar_pos = (short)GetScrollPos(right_sbar,SB_CTL);
			old_setting = sbar_pos;
			GetScrollRange(right_sbar,SB_CTL,&min,&max);
			if (wheel_delta/120)
				sbar_pos -= wheel_delta/120;
			else if (wheel_delta > 0)
				sbar_pos -= 1;
			else if (wheel_delta < 0)
				sbar_pos += 1;

			if (sbar_pos < 0)
				sbar_pos = 0;
			if (sbar_pos > (short)max)
				sbar_pos = (short)max;
			SetScrollPos(right_sbar,SB_CTL,sbar_pos,TRUE);
			if (sbar_pos != old_setting) {
				set_up_terrain_buttons();
				place_right_buttons(); // (0)
//					update_main_screen();
			}
		}
		break;
	case WM_KEYDOWN:
		if (hwnd != mainPtr && hwnd != tilesPtr && hwnd != palettePtr) {
			check_cd_event(hwnd,message,wParam,lParam);
			}
			else {
				All_Done = handle_syskeystroke(wParam,lParam,&handled);
				}
		return 0;
		break;

	case WM_CHAR:
		if (hwnd != mainPtr && hwnd != tilesPtr && hwnd != palettePtr)
			check_cd_event(hwnd,message,wParam,lParam);
			else {
				All_Done = handle_keystroke(wParam,lParam);
				}
		kill_next_win_char = FALSE;
		return 0;
		break;

	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
		if (hwnd != mainPtr && hwnd != tilesPtr && hwnd != palettePtr)
			check_cd_event(hwnd,message,wParam,lParam);
		else if (hwnd == palettePtr) {
			SetFocus(hwnd);
			press.x = LOWORD(lParam);
			press.y = HIWORD(lParam);

			All_Done = handleToolPaletteClick(press, wParam,lParam);
			check_game_done();
		}
		else {
			SetFocus(hwnd);
			press.x = LOWORD(lParam);
			press.y = HIWORD(lParam);

			All_Done = handle_action(press, wParam,lParam, (hwnd != mainPtr) ? TILES_WINDOW_NUM : MAIN_WINDOW_NUM);
			check_game_done();
		}
		return 0;
		break;

	case WM_LBUTTONUP:
		mouse_button_held = FALSE;
		lockLatestStep();
		break;

	case WM_TIMER:
		// first, mouse held?
		if ((wParam == 1) && (mouse_button_held == TRUE)) {
			GetCursorPos(&p);
			ScreenToClient(mainPtr,&p);
			All_Done = handle_action(p, 0,0,0);
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
			All_Done = handle_action(press, wParam,lParam,0);
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
		EndPaint( hwnd, &ps );

		if (hwnd != mainPtr && hwnd != tilesPtr && hwnd != palettePtr)
			check_cd_event(hwnd,message,wParam,lParam);
		else
			redraw_screen();
		return 0;

	case WM_SIZE://using wm_size rather than wm_sizing makes sure that it works w/ the maximise button
		if (hwnd == mainPtr && main_dc != NULL/*don't want this when opening window for first time before initing:|*/){
			GetClientRect(mainPtr,&windRect);
			terrain_viewport_3d.bottom = windRect.bottom - 120;//40;
			terrain_viewport_3d.right = windRect.right - 40;
			kRect3DEditScrn.bottom = windRect.bottom - 100;//20;
			kRect3DEditScrn.right = windRect.right - 20;
			terrain_rect_gr_size.bottom = ((windRect.bottom - windRect.top) > (windRect.right - windRect.left)) ? (windRect.bottom - windRect.top) : (windRect.right - windRect.left);
			terrain_rect_gr_size.right = terrain_rect_gr_size.bottom;
			delete_graphic(&ter_draw_gworld);
			ter_draw_gworld = DibCreate (terrain_rect_gr_size.right,terrain_rect_gr_size.bottom, 16,0);
			recalculate_2D_view_details();
			recalculate_draw_distances();
			set_up_view_buttons();
			update_screen_locs();
			redraw_screen((cur_viewing_mode == 1) ? true : false);
		}
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
					place_right_buttons(); // (0)
//					update_main_screen();
				}
				break;
			}
	    SetFocus(tilesPtr);
		return 0;

	case WM_SYSCOMMAND:
		if (hwnd == tilesPtr && ((short) wParam == 1701 || (short) wParam == 1702)) {
			handle_menu_choice((short) wParam);
			return 0;
		}
		break;

	case WM_COMMAND:
		if (hwnd == mainPtr || hwnd == tilesPtr || hwnd == palettePtr) {

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
			if (save_check(869) == FALSE)
				return 0;
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
				handle_help_menu(menu_item % 100);					 
				break;
			case 16:	// patch for ctrl-<key> command
				handle_keystroke( choice % 100, 0 );
				break;
			case 17:
				handle_tiles_menu(menu_item % 100);
				break;
			}
		} 
}

void handle_file_menu(int item_hit)
{
	switch (item_hit) {
		case 1: // Open Scenario
			if (change_made_town || change_made_outdoors) {
				if (save_check(858) == FALSE)
					break;							
			}
			load_campaign();
			if (file_is_loaded) {
				setUpCreaturePalette = FALSE;
				setUpItemPalette = FALSE;
				update_item_menu();
				purgeUndo();
				purgeRedo();
				redraw_screen();
				shut_down_menus(); // (0)
				shut_down_tile_menus();
			}
			break;
		case 2: // Save Scenario
			save_campaign();
			break;
		case 3: // New Scenario
			if ((file_is_loaded) &&  (change_made_town || change_made_outdoors)) {
				give_error("You need to save the changes made to your scenario before you can create a new scenario.",
					"",0);
				return;
			}
			build_scenario();
			redraw_screen();
			break;
		case 4: // Import Blades of Exile Scenario
			if ((file_is_loaded) && (change_made_town || change_made_outdoors)) {
				give_error("You need to save the changes made to your scenario before you can import a Blades of Exile scenario.",
					"",0);
				return;
			}
			switch (fancy_choice_dialog(876,0)) {
			case 1:
					use_custom_name = 0;
					import_blades_of_exile_scenario();
			break;
			case 2:
			break;
			case 9:
					use_custom_name = 1;
					import_blades_of_exile_scenario();
			break;
			}
		break;
			
		case 6: // Quit
			if (save_check(869) == FALSE)
				break;
			force_game_end = TRUE;
			break;
	}
}

void handle_edit_menu(int item_hit)
{

		 	Boolean need_redraw;
			short x;
			location spot_hit;
			HMENU menu;

	switch (item_hit) {
		case 1: // Undo
			performUndo();
			break;
		case 2: // Redo
			performRedo();
			break;
		case 4: // Cut
			set_string("Instance copied and deleted","  ");
			cut_selected_instance();
			if(editing_town)
				change_made_town = TRUE;
			else
				change_made_outdoors = TRUE;
			break;
		case 5: // Copy
			set_string("Instance copied","  ");
			copy_selected_instance();
			if(editing_town)
				change_made_town = TRUE;
			else
				change_made_outdoors = TRUE;
			break;
		case 6: // Paste
			if ((copied_creature.exists() == FALSE) && (copied_item.exists() == FALSE) && (copied_ter_script.exists == FALSE)) {
				beep();
				break;
				}
			set_string("Paste copied instance","Select location to place");
			set_tool(48);
			break;
		case 7: // delete selected instance: "Delete"
			delete_selected_instance();
			set_string("Selected Instance Deleted","");
			if(editing_town)
				change_made_town = TRUE;
			else
				change_made_outdoors = TRUE;
			break;
		case 8: // clear selected instance: "Clear Screen"
				selected_item_number = -1;
				set_string("Item De-selected","");
		break;
		case 9: // clear selected instance: "Clear Screen"
				set_tool(51);
				set_string("Continual Delete Mode","");
		break;
		case 10: // Decrease View Mode
							if ((cur_viewing_mode == 0) || (cur_viewing_mode > 9)) {
								last_large_mode = cur_viewing_mode;
								set_view_mode(2);
								}
							else if(cur_viewing_mode == 2)
								set_view_mode(1);
							else if(cur_viewing_mode == 1)
								set_view_mode(last_large_mode);
							set_up_terrain_buttons();
							reset_small_drawn();
							redraw_screen();
		break;
		case 11: // Increase View Mode
							if(cur_viewing_mode == 1)
								set_view_mode(2);
							else if(cur_viewing_mode == 2)
								set_view_mode(last_large_mode);
							else {
								last_large_mode = cur_viewing_mode;
								set_view_mode(1);
								}
							set_up_terrain_buttons();
							reset_small_drawn();
							redraw_screen();
		break;
		case 12: // "Decrease Drawing Number Mode"
				 if (numerical_display_mode == 0)
				    numerical_display_mode = 4;
					else numerical_display_mode = ((numerical_display_mode - 1) % 5);
					object_display_mode = 0;
					need_redraw = TRUE;
			break;
		case 13: // "Increase Drawing Number Mode"
					numerical_display_mode = ((numerical_display_mode + 1) % 5);
					object_display_mode = 0;
					need_redraw = TRUE;
			break;
		case 14: //  "Decrease Object Number Mode"
				 if (object_display_mode == 0)
				    object_display_mode = 4;
					else object_display_mode = ((object_display_mode - 1) % 5);
					numerical_display_mode = 0;
					need_redraw = TRUE;
			break;
		case 15: //  "Increase Object Number Mode"
					object_display_mode = ((object_display_mode + 1) % 5);
					numerical_display_mode = 0;
					need_redraw = TRUE;
			break;

		case 17: // Load Previous Outdoor Zone/Town
			if (editing_town) {
			if (change_made_town == TRUE) {
				if (save_check(859) == FALSE)
					break;
			}
			 if (cur_town == 0)
				load_town(scenario.num_towns - 1);
			 else load_town(cur_town - 1);
				clear_selected_copied_objects();
				set_up_terrain_buttons();
				change_made_town = FALSE;
				cen_x = max_zone_dim[town_type] / 2; cen_y = max_zone_dim[town_type] / 2;
				reset_drawing_mode();
				reset_small_drawn();
				purgeUndo();
				purgeRedo();
				redraw_screen();
				}

			else {
			if (change_made_outdoors == TRUE) {
				if (save_check(859) == FALSE)
					break;
			}
				if (cur_out.x > 0) {
				 spot_hit.x = cur_out.x - 1;
				 spot_hit.y = cur_out.y;
				 }
				if ((cur_out.x == 0) && (cur_out.y > 0)) {
				 spot_hit.x = scenario.out_width - 1;
				 spot_hit.y = cur_out.y - 1;
				 }
				if ((cur_out.x == 0) && (cur_out.y == 0)) {
				 spot_hit.x = scenario.out_width - 1;
				 spot_hit.y = scenario.out_height - 1;
				 }
				clear_selected_copied_objects();
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

		case 18: // Load Next Outdoor Zone/Town
							if (editing_town) {
			if (change_made_town == TRUE) {
				if (save_check(859) == FALSE)
					break;
			}
			if (cur_town + 1 == scenario.num_towns)
				load_town(0);
			else load_town(cur_town + 1);

				clear_selected_copied_objects();
				set_up_terrain_buttons();
				change_made_town = FALSE;
				cen_x = max_zone_dim[town_type] / 2; cen_y = max_zone_dim[town_type] / 2;
				reset_drawing_mode();
				reset_small_drawn();
				purgeUndo();
				purgeRedo();
				redraw_screen();
				}

				else {
			if (change_made_outdoors == TRUE) {
				if (save_check(859) == FALSE)
					break;
			}
			if ((cur_out.x < scenario.out_width - 1) && (cur_out.y <= scenario.out_height - 1)) {
				 spot_hit.x = cur_out.x + 1;
				 spot_hit.y = cur_out.y;
				 }
			if ((cur_out.x == scenario.out_width - 1) && (cur_out.y < scenario.out_height - 1)) {
				 spot_hit.x = 0;
				 spot_hit.y = cur_out.y + 1;
				 }
			if ((cur_out.x == scenario.out_width - 1) && (cur_out.y == scenario.out_height - 1)) {
				 spot_hit.x = 0;
				 spot_hit.y = 0;
				 }
				clear_selected_copied_objects();
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

	case 20: // Delete Selected Object Only
	if ((editing_town == TRUE) && (cur_viewing_mode == 0 || cur_viewing_mode == 10 || cur_viewing_mode == 11)) {
		if (selected_item_number > -1) {
	delete_selected_instance();
	set_string("Selected Instance Deleted","");
	set_cursor(7);
	}
	}
	break;


	case 21: // toggle gridline mode
				if (grid_mode == 2)
				 grid_mode = 0;
				 else  grid_mode =  grid_mode + 1;
				small_any_drawn = FALSE;
				draw_terrain();
	break;

		case 22: // Load Outdoor Zone Above
			if (editing_town)
				break;

			if (change_made_outdoors == TRUE) {
				if (save_check(859) == FALSE)
					break;
			}
				if (cur_out.y > 0) {
				 spot_hit.x = cur_out.x;
				 spot_hit.y = cur_out.y - 1;
				 }
				else {
				 spot_hit.x = cur_out.x;
				 spot_hit.y = scenario.out_height - 1;
				 }
				clear_selected_copied_objects();
				load_outdoor_and_borders(spot_hit);
				set_up_terrain_buttons();
				cen_x = 24; cen_y = 24;
				reset_drawing_mode();
				purgeUndo();
				purgeRedo();
				redraw_screen();
				change_made_outdoors = FALSE;
			break;

		case 23: // Load Outdoor Zone Below
			if (editing_town)
				break;

			if (change_made_outdoors == TRUE) {
				if (save_check(859) == FALSE)
					break;
			}
			if (cur_out.y < scenario.out_height - 1) {
				 spot_hit.x = cur_out.x;
				 spot_hit.y = cur_out.y + 1;
				 }
			else {
				 spot_hit.x = cur_out.x;
				 spot_hit.y = 0;
				 }
				clear_selected_copied_objects();
				load_outdoor_and_borders(spot_hit);
				set_up_terrain_buttons();
				cen_x = 24; cen_y = 24;
				reset_drawing_mode();
				purgeUndo();
				purgeRedo();
				redraw_screen();
				change_made_outdoors = FALSE;
			break;

		case 24: // Decrease Group Display Mode
		 if (hintbook_mode8 == 0) 
	     hintbook_mode8 = 11;
		 else hintbook_mode8 = hintbook_mode8 - 1;
			 small_any_drawn = FALSE;
			 draw_terrain();
			break;
			
		case 25: // Increase Group Display Mode
		 if (hintbook_mode8 == 11) 
	     hintbook_mode8 = 0;
		 else hintbook_mode8 = hintbook_mode8 + 1;
			 small_any_drawn = FALSE;
			 draw_terrain();
			break;

		case 26: //Play Sounds
			play_sounds = !play_sounds;
			menu = GetMenu(mainPtr);
			ModifyMenu(menu,126,(play_sounds) ? (MF_ENABLED | MF_CHECKED | MF_BYCOMMAND | MF_STRING) :
			(MF_ENABLED | MF_UNCHECKED | MF_BYCOMMAND | MF_STRING),126,"Play Sounds");
			write_should_play_sounds(play_sounds);
			play_sound(0);
			break;
		}
	draw_main_screen();
}


void handle_campaign_menu(int item_hit)
{
		 short i;
		 
	switch (item_hit) {
		case 1: // Edit Town
			if(editing_town)
				return;
			small_any_drawn = FALSE;
			cen_x = max_zone_dim[town_type] / 2; cen_y = max_zone_dim[town_type] / 2;
			current_drawing_mode = current_height_mode = 0;
			editing_town = TRUE;
			CreateToolTipForRect(palettePtr);
			set_up_terrain_buttons();
			reset_mode_number();
			shut_down_menus(); // (4)
			DrawMenuBar(mainPtr);
			reset_drawing_mode();
			purgeUndo();
			purgeRedo();
			redraw_screen();
			break;
		case 2: // Edit Outdoor Section
			if(!editing_town)
				return;
			small_any_drawn = FALSE;
			cen_x = 24; cen_y = 24;
			current_drawing_mode = current_height_mode = 0;
			editing_town = FALSE;
			CreateToolTipForRect(palettePtr);
			set_up_terrain_buttons();
			reset_mode_number();
			shut_down_menus(); // (2)
			clear_selected_copied_objects();
			DrawMenuBar(mainPtr);
			reset_drawing_mode();
			purgeUndo();
			purgeRedo();
			redraw_screen();
			break;
		case 3: // Create New Town
			if (change_made_town || change_made_outdoors) {
				give_error("You need to save the changes made to your scenario before you can add a new town.",
					"",0);
				return;
			}
			if (scenario.num_towns > 198) {
				give_error("You have reached the limit of 200 zones you can have in one campaign.",
					"",0);
				return;
			}
			new_town();
			reset_drawing_mode();
			cen_x = max_zone_dim[town_type] / 2; cen_y = max_zone_dim[town_type] / 2;
			purgeUndo();
			purgeRedo();
			redraw_screen();
			change_made_town = TRUE;
			break;
		case 5: // Basic Scenario Details
			edit_scen_details();
			break;
		case 6: // Set Label Icon
			edit_scen_intro_pic();
			break;
		case 7: // Intro Text 1
			edit_scen_intro(0);
			break;
		case 8: // Intro Text 2
			edit_scen_intro(1);
			break;
		case 9: // Intro Text 3
			edit_scen_intro(2);
			break;
		case 12: // Reload Scenario Data Script
			if (fancy_choice_dialog(871,0) == 2)
				break;
			char file_name[256];
			get_name_of_current_scenario(file_name);
			if (load_individual_scenario_data(file_name /*,TRUE */) == FALSE) {
				file_is_loaded = FALSE;
				return;
				}	
			setUpCreaturePalette = FALSE;
			setUpItemPalette = FALSE;
			update_item_menu();
			set_up_terrain_buttons();
			place_right_buttons(); /* (0) */
			break;
		case 13: // Clean Up Walls
			if (fancy_choice_dialog(873,0) == 2)
				break;
			clean_walls();
			redraw_screen();
			change_made_town = TRUE;
			break;
		case 14: // Import Town
			clear_selected_copied_objects();
			if(import_boa_town()){
				change_made_town = TRUE;
				purgeUndo();
				purgeRedo();
				redraw_screen();
			}
			break;
		case 15: // Import Outdoor Section
			clear_selected_copied_objects();
			if(import_boa_outdoors()){
				change_made_outdoors = TRUE;
				purgeUndo();
				purgeRedo();
				redraw_screen();
			}
			break;
		case 16: // Set Variable Town Entry
			edit_add_town();
			change_made_town = TRUE;
			break;
		case 17: // Edit Item Placement Shortcuts
			edit_item_placement();
			change_made_town = TRUE;
			break;
/*		case 18: // Delete Last Town
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
			break;*/
		case 18: //Delete Current Town
			if (change_made_town || change_made_outdoors) {
				give_error("You need to save the changes made to your scenario before you can delete a town.","",0);
				return;
			}
			if (scenario.num_towns == 1) {
				give_error("You can't delete the only town in a scenario. All scenarios must have at least 1 town.","",0);
				return;
			}
			if (fancy_choice_dialog(865,0) == 1) {
				delete_town();
				load_all_town_names(NULL);
				load_town(cur_town);
				clear_selected_copied_objects();
				set_up_terrain_buttons();
				change_made_town = FALSE;
				cen_x = max_zone_dim[town_type] / 2; cen_y = max_zone_dim[town_type] / 2;
				reset_drawing_mode();
				reset_small_drawn();
				purgeUndo();
				purgeRedo();
				redraw_screen();
			}
			break;
		case 20: // Change Outdoor Size
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
		case 21: // bring up the Edit Horses dialog screen
			edit_horses();
			break;
		case 22: // bring up the Edit Boats dialog screen
			edit_boats();
			break;
		case 23: // Clear All Horses
			if (fancy_choice_dialog(890,0) == 2)
				break;
				for (i = 0; i < 30; i++)
				scenario.scen_horses[i].clear_horse_record_type();
			break;
		case 24: // Clear All Boats
			if (fancy_choice_dialog(891,0) == 2)
				break;
				for (i = 0; i < 30; i++)
				scenario.scen_boats[i].clear_boat_record_type();
			break;
		}
}

void handle_town_menu(int item_hit)
{
	short i,x;	

	switch (item_hit) {
		case 1: // Load Different Town
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
				cen_x = max_zone_dim[town_type] / 2; cen_y = max_zone_dim[town_type] / 2;
				reset_drawing_mode();
				reset_small_drawn();
				purgeUndo();
				purgeRedo();
				redraw_screen();
			}
			break;
// q_3DModStart
		case 2: // Town Details
				 edit_town_details();
				 change_made_town = TRUE;
			break; 

		case 3: // Town Wandering Monsters
				 edit_town_wand();
				 change_made_town = TRUE;
			break; 
// q_3DModEnd
		case 4: // Set Town Boundaries							
			set_tool(17);
			set_string("Set town boundary","Select upper left corner");
			break;
// q_3DModStart
		case 5: // Frill Up Terrain
				 frill_terrain();
				 change_made_town = TRUE;
			break;
		case 6: // Remove Terrain Frills
				 unfrill_terrain();
				 change_made_town = TRUE;
			break;
// q_3DModEnd
		case 7:  //Edit Area Descriptions
			 edit_town_strs();
			break;
			
		case 9: //Set Starting Location
			if (fancy_choice_dialog(867,0) == 2)
					break;
			set_string("Set Town Start Point","Where will party start?");
			set_tool(72);
			break;
		case 11: // Add Random Items 
				if (fancy_choice_dialog(863,0) == 2)
					break;
				place_items_in_town();
				change_made_town = TRUE; 
				redraw_screen();
				 break; 
		case 12: // Set All Items Not Property
				for (i = 0; i < 144; i++)
					town.preset_items[i].properties = town.preset_items[i].properties & 253;
				fancy_choice_dialog(861,0);
				draw_terrain();
				change_made_town = TRUE; 
				break; 
		case 13: // Set All Items Property
				for (i = 0; i < 144; i++)
					town.preset_items[i].properties = town.preset_items[i].properties | 2;
				fancy_choice_dialog(879,0);
				draw_terrain();
				change_made_town = TRUE; 
				break; 
		case 15: // Clear All Items
			if (fancy_choice_dialog(862,0) == 2)
				break;
			for (i = 0; i < 144; i++)
				town.preset_items[i].clear_item_type();
			draw_terrain();
			change_made_town = TRUE; 
			redraw_screen();
			break; 

		case 16: // Clear All Monsters
			if (fancy_choice_dialog(878,0) == 2)
				break;
			for (i = 0; i < 80; i++)
				town.creatures[i].clear_creature_start_type();
			draw_terrain();
			change_made_town = TRUE; 
			redraw_screen();
			break;
		case 17: // Clear All Special Encounters
			if (fancy_choice_dialog(877,0) == 2)
				break;
			for (x = 0; x < NUM_TOWN_PLACED_SPECIALS; x++) {
				town.spec_id[x] = kNO_TOWN_SPECIALS;
				SetMacRect(&town.special_rects[x],-1,-1,-1,-1);
				}
			change_made_town = TRUE; 
			redraw_screen();
			break;	
		case 18: // Clear All Preset Fields, 0:8
			if (fancy_choice_dialog(883,0) == 2)
				break;
			for (short i = 0; i < 60; i++) {
			if ((town.preset_fields[i].field_type >= 0) && (town.preset_fields[i].field_type < 9))
			town.preset_fields[i].clear_preset_field_type();
			}
			change_made_town = TRUE;
			redraw_screen();
			break; 
		case 19:// Clear All Placed Stains, 14:21
			if (fancy_choice_dialog(884,0) == 2)
				break;
			for (short i = 0; i < 60; i++) {
			if ((town.preset_fields[i].field_type > 13) && (town.preset_fields[i].field_type < 22))
			town.preset_fields[i].clear_preset_field_type();
			}
			change_made_town = TRUE;
			redraw_screen();
			break;
		case 20: // Replace All for Creatures
				monst_replaceall();
				draw_terrain();
				change_made_town = TRUE;
			break;
		}
}

void handle_outdoor_menu(int item_hit)
{
	short x;
				
	switch (item_hit) {
		case 1: // Load Different Outdoor Section
			if (change_made_outdoors == TRUE) {
				if (save_check(859) == FALSE)
					break;
			}
			x = pick_out(cur_out,scenario.out_width,scenario.out_height);
			if (x >= 0) {
				location spot_hit = {(t_coord)(x / 100),(t_coord)(x % 100)};
				clear_selected_copied_objects();
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
		case 2:	//Outdoor Details
				outdoor_details();
				set_up_terrain_buttons();
				draw_main_screen();
				change_made_outdoors = TRUE;
				break;
				
		case 3: // Edit Outdoor Wandering Monsters
				 edit_out_wand(0);
				 change_made_outdoors = TRUE;
				 break;
		case 4: // Edit Outdoor Special Encounters
				 edit_out_wand(1);
				 change_made_outdoors = TRUE;
				 break;
		case 5: // Edit Outdoor Preset Encounters
				 edit_out_wand(2);
				 draw_terrain();
				 change_made_outdoors = TRUE;
				 break;

		case 6: // Frill Up Terrain
				 frill_terrain();
				 change_made_outdoors = TRUE;
		break;
		case 7: // Remove Terrain Frills
				 unfrill_terrain();
				 change_made_outdoors = TRUE;
				 break;
		case 8: //Edit Area Descriptions
				 edit_out_strs();
				 change_made_outdoors = TRUE;
				 break; 
		case 10: //Set Starting Location
			if (fancy_choice_dialog(864,0) == 2)
				return;
			set_string("Set Outdoor Start Point","Where will party start?");
			set_tool(71);
			break;

		case 12: // Clear All Placed Specials
			if (fancy_choice_dialog(885,0) == 2)
				break;
			for (x = 0; x < NUM_OUT_PLACED_SPECIALS; x++) {
				current_terrain.spec_id[x] = kNO_OUT_SPECIALS;
				SetMacRect(&current_terrain.special_rects[x],-1,-1,-1,-1);
				}
				change_made_outdoors = TRUE;
			redraw_screen();
			break;

		}
}


void handle_item_menu(int item_hit)
{

	//if (same_string("Unused",scen_data.scen_items[item_hit].full_name)) {
	//	give_error("You can't place an item named 'Unused'.","",0);
	//	return;
	//	}
	set_tool(47);
	mode_count = (short)item_hit;
	set_string("Place New Item","Select location to place");

}

void handle_monst_menu(int item_hit)
{
	if (same_string("Unused",scen_data.scen_creatures[item_hit].name)) {
		give_error("You can't place an creature named 'Unused'.","",0);
		return;
		}
	set_tool(46);
	mode_count = (short)item_hit;
	set_string("Place New Creature","Select location to place");
}


void handle_help_menu(int item_hit)
{   

	switch (item_hit) {
		case 1: // 2D Editor Credits: "About Blades Scenario Editor"
				fancy_choice_dialog(1062,0);
				break;
		case 2: // 3D Editor Credits
				fancy_choice_dialog(1063,0);
			break;	
		case 4: //   Write Scenario Data to Text File
			if (fancy_choice_dialog(866,0) == 1) {
         last_file_printed = 0;
				 start_data_dump();
				 }
			break;
		case 5: //   Do full write up: "Full Write up: Outdoor/Town Names"
			if (fancy_choice_dialog(881,0) == 1) {
         last_file_printed = 5;
				 start_full_data_dump();
				 }
			break;
		case 6: //   Filtered print out of town data: "Town: Write Data To File"
			if (fancy_choice_dialog(882,0) == 1) {
         last_file_printed = 6;
				 start_town_data_dump();
				 }
			break;
		case 7: // Town: Full Write Data To File
			if (fancy_choice_dialog(886,0) == 1) {
			   last_file_printed = 13;
				 start_full_town_data_dump();
				 }
			break;
			
		case 8: // Repeat Printing of Last File
         if (last_file_printed == 0) {
				 set_string("Repeat Printing of Last File:","Scenario Data.txt");
				 start_data_dump();
				 }
         if (last_file_printed == 5) {
				 set_string("Repeat Printing of Last File:","Scenario Write Up.txt");
				 start_full_data_dump();
				 }
         if (last_file_printed == 6) {
				 set_string("Repeat Printing of Last File:","Concise Town Report.txt");
				 start_town_data_dump();
				 }
         if (last_file_printed == 13) {
				 set_string("Repeat Printing of Last File:","Full Town Report.txt");
				 start_full_town_data_dump();
				 }
         if (last_file_printed == 15) {
				 set_string("Repeat Printing of Last File:","Outdoor Zones Write Up.txt");
				 start_full_data_dump();
				 }
         if (last_file_printed == 16) {
				 set_string("Repeat Printing of Last File:","Current Outdoor Zone Write Up.txt");
				 start_currentout_data_dump();
				 }
         if (last_file_printed == 17) {
				 set_string("Repeat Printing of Last File:","Scenario Text and Data.txt");
				 scenariotext_data_dump();
				 }

				break;

		case 9: // Lists all special rectangles, town locations for each zone: "All Outdoor Zones: Full Write up"
			if (fancy_choice_dialog(887,0) == 1) {
         last_file_printed = 15;
				 start_fullout_data_dump();
				 }
			break;

		case 10: // Current Outdoor Zone: Full Write up
			if (fancy_choice_dialog(888,0) == 1) {
         last_file_printed = 16;
				 start_currentout_data_dump();
				 }
			break;
			
		case 11: // Write Scenario Text To Text File
			if (fancy_choice_dialog(889,0) == 1) {
         last_file_printed = 17;
				 scenariotext_data_dump();
				 }
			break;

		case 12: // 3D Editor - Getting Started
				fancy_choice_dialog(986,0);
			break;	

		case 13: // Shortcut keys 1 screen
				fancy_choice_dialog(987,0);
			break;

		case 14: // Shortcut keys 2 screen
				fancy_choice_dialog(988,0);
			break;
			
		case 15: // Shortcut keys 3 screen
				fancy_choice_dialog(989,0);
			break;

		case 19: // Display a specified dialog
				short j = how_many_dlog(0,800,1063,"Which dialog?  (800 - 1063)");
				fancy_choice_dialog(j,0);
			break;
			
		}
	draw_main_screen();		
}

void handle_tiles_menu(int item_hit)
{
	char string[64];
	switch(item_hit){
		case 1:
			tileZoomLevel++;
			shut_down_tile_menus();
			sprintf(string, "zoomIn: zoom level %d", tileZoomLevel);
			MessageBox(mainPtr,string,"zoom",MB_OK);
			break;
		case 2:
			tileZoomLevel--;
			shut_down_tile_menus();
			sprintf(string, "zoomOut: zoom level %d", tileZoomLevel);
			MessageBox(mainPtr,string,"zoom",MB_OK);
			break;
	}
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
	short which_m;
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
		case 800: edit_make_scen_1_event_filter(item_hit); break;
		case 801: edit_make_scen_2_event_filter(item_hit); break;
		case 803: edit_scen_details_event_filter(item_hit); break;
		case 804: edit_scen_intro_event_filter(item_hit); break;
		case 805: edit_scen_intro_pic_event_filter(item_hit); break;
		case 808: edit_horses_event_filter (item_hit); break;
		case 809: edit_boats_event_filter (item_hit); break;
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
		case 838: edit_placed_script_event_filter(item_hit); break;
		case 839: edit_town_strs_event_filter(item_hit); break;
		case 840: edit_area_rect_event_filter(item_hit); break;
		case 841: pick_import_town_event_filter(item_hit); break;
		case 842: edit_placed_item_event_filter(item_hit); break;
		case 843: monst_replaceall_event_filter (item_hit); break;
		case 850: edit_out_strs_event_filter(item_hit); break;
		case 851: outdoor_details_event_filter(item_hit); break;
		case 852: edit_out_wand_event_filter(item_hit); break;
		case 854: pick_out_event_filter(item_hit); break;
		case 855: get_a_number_event_filter(item_hit); break;
		case 856: get_a_number_event_filter(item_hit); break;
		case 857: change_ter_event_filter(item_hit); break;
		case 872: get_a_number_event_filter(item_hit); break;
		case 970: case 971: case 972: case 973: display_strings_event_filter(item_hit); break;
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
