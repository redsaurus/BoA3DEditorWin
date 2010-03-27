#include "stdafx.h"
/*
#include <Windows.h>
#include <stdio.h>
#include "math.h"
#include "stdlib.h"
*/

#include "Resource.h"
#include "global.h"

#define	MAX_RECURSION_DEPTH	200

// bit definition of scroll direction
enum {
	eSCRL_NoScrl	= 0,
	eSCRL_Top		= 0x08,
	eSCRL_Left		= 0x04,
	eSCRL_Bottom	= 0x02,
	eSCRL_Right		= 0x01,
};

// Edit screen coordinates
/*const RECT kRect3DEditScrn = {
	TERRAIN_BORDER_WIDTH + TER_RECT_UL_X - 15,
	TERRAIN_BORDER_WIDTH + TER_RECT_UL_Y - 15,
	TERRAIN_BORDER_WIDTH + BIG_SPACE_SIZE * 9 + TER_RECT_UL_X + 15,
	TERRAIN_BORDER_WIDTH + BIG_SPACE_SIZE * 9 + TER_RECT_UL_Y + 15
};*/
const RECT kRect3DEditScrn = {
	0 + TER_RECT_UL_Y,
	0 + TER_RECT_UL_X,
	496 + TER_RECT_UL_Y,
	415 + TER_RECT_UL_X
};
extern const RECT terrain_viewport_3d;

// scroll speed limit: duration / cycle
const DWORD kScrollLimitTime = 100;		// msec

// Global variables

	// border rects order: top, left, bottom, right //
RECT border_rect[4];
RECT terrain_rects[264];
// RECT terrain_rect_base = {0,0,16,16};							

RECT terrain_rects_3D[264];
// RECT terrain_rect_base_3D = {0,0,16,16};

RECT palette_buttons[8][6];

// These are the rects of the terrain spots INSIDE the terrain GWORLD, NOT the screen.
RECT small_edit_ter_rects[MAX_TOWN_SIZE][MAX_TOWN_SIZE];
RECT medium_edit_ter_rects[32][32];
RECT large_edit_ter_rects[9][9];

// Bottom text rectangles
RECT left_text_lines[14];
// right_text_lines rects are for INSIDE the buttons GWORLD, NOT the screen.
RECT right_text_lines[6];

location spot_hit;
bool object_sticky_draw;
short current_floor_drawn = 0;
short current_terrain_drawn = 0;

extern char hintbook_mode0;
extern char hintbook_mode1;
extern char hintbook_mode2;
extern char hintbook_mode3;
extern char hintbook_mode4;
extern char hintbook_mode5;
extern char hintbook_mode6;
extern char hintbook_mode7;
extern char hintbook_mode8;
extern char hintbook_mode9;

extern char grid_mode;
// if a terrain type has special property from 19-30, it is a slope. this
// array says what corners for these 12 terrain types are elevated.
// first field is nw corner
// 2nd field is sw corner
// 3rd field is se corner
// 4th field is ne corner
// if 1, is elevated
short hill_c_heights[12][4] = {{1,1,0,0},{0,1,0,0},{0,1,1,0},
								{0,0,1,0},{0,0,1,1},{0,0,0,1},
								{1,0,0,1},{1,0,0,0},{1,1,0,1},
								{1,1,1,0},{0,1,1,1},{1,0,1,1}};

// external global variables

extern scenario_data_type scenario;
extern town_record_type town;
extern big_tr_type t_d;
big_tr_type clipboard;
RECT clipboardSize;
Boolean dataOnClipboard=false;
extern outdoor_record_type current_terrain;
extern scen_item_data_type scen_data;
// extern short borders[4][50];
// extern unsigned char border_floor[4][50];
// extern unsigned char border_height[4][50];

extern outdoor_record_type border_terrains[3][3];

extern short cur_town;
extern location cur_out;
extern short current_drawing_mode;
extern short town_type ;
extern short current_height_mode ;
extern Boolean editing_town;
extern short cur_viewing_mode;
extern short overall_mode;
extern short ulx,uly;
extern Boolean small_any_drawn;

extern Boolean file_is_loaded;
extern Boolean mouse_button_held;
extern short cen_x, cen_y;

extern short mode_count;
extern short current_height_mode;

extern Boolean change_made_town;
extern Boolean change_made_outdoors;

extern HWND	mainPtr;

extern short selected_item_number;
extern item_type copied_item;
extern creature_start_type copied_creature;
extern short max_dim[4];

extern HWND right_sbar;
extern in_town_on_ter_script_type copied_ter_script;

extern Boolean kill_next_win_char;

extern short current_cursor;

// local variables

short current_terrain_type = 0;
location last_spot_hit = kINVAL_LOC;

RECT working_rect;
Boolean erasing_mode;

struct corner_and_sight_on_space {
	unsigned nw_corner : 2;
	unsigned sw_corner : 2;
	unsigned se_corner : 2;
	unsigned ne_corner : 2;
	unsigned see_in : 1;
	unsigned see_to : 1;
} corner_and_sight_map[88][88];
typedef struct corner_and_sight_on_space *temp_space_info_ptr;

short recursive_depth = 0; // used for recursive hill/terrain correcting function
short recursive_hill_up_depth = 0;
short recursive_hill_down_depth = 0;

linkedList undo;
linkedList redo;

short store_control_value = 0;


// function prototype

int check_scroller( POINT the_point );
int check_scroller_2D( POINT the_point );
int check_scroller_3D( POINT the_point );
bool process_scroll_click( int map_size, POINT thePoint  );
bool handle_scroll( int map_size, int scrl, bool ctrl_key, bool shft_key );
Boolean clean_up_from_scrolling( int map_size, int dx, int dy );

void handle_ter_spot_press(location spot_hit,Boolean option_hit,Boolean right_click);

// void play_press_snd();
void set_new_terrain(short selected_terrain);
void set_new_floor(short selected_terrain);
// Boolean is_hill(short i,short j);
Boolean is_rocks(short i,short j);
Boolean is_water(short i,short j);
void shy_change_circle_terrain(location center,short radius,short terrain_type,short probability);
void change_circle_terrain(location center,short radius,short terrain_type,short probability);
void change_rect_terrain(RECT r,short terrain_type,short probability,Boolean hollow);
Boolean town_fix_grass_rocks(location l);
Boolean out_fix_grass_rocks(location l);
Boolean town_fix_rocks_water(location l);
Boolean out_fix_rocks_water(location l);
Boolean town_fix_hills(location l);
Boolean out_fix_hills(location l);
// short get_corner_height(short x, short y,short out_or_town,short which_corner) ;
short adjust_get_ter(short x, short y,short out_or_town);
void adjust_space(location l);
// short coord_to_ter(short x,short y);
void change_height(location l,short lower_or_raise);
void adjust_space_height(location l,short lower_or_raise);
void adjust_space_height_lower(location l);
void adjust_space_height_raise(location l);
short locs_to_dir(location l1,location l2);
void paste_selected_instance(location create_loc);
void check_selected_item_number();
void shift_selected_instance(short dx,short dy);
void create_navpoint(location spot_hit);
void delete_navpoint(location spot_hit);
void create_new_creature(short c_to_create,location create_loc,creature_start_type *c_to_make);
void shift_item_locs(location spot_hit);
void create_town_entry(RECT rect_hit);
void edit_town_entry(location spot_hit);
void set_rect_height(RECT r);
void add_rect_height(RECT r);
void shy_put_terrain(short i,short j,short ter);
void transform_walls(RECT working_rect);
Boolean is_not_darkness_floor(short i,short j); 
void place_bounding_walls(RECT working_rect);
Boolean is_wall(short x, short y);
Boolean is_dumb_terrain(short ter);
short get_height(short x, short y,short out_or_town);

Boolean old_can_see_in(location p1,location p2, /* short check_light,*/ short check_travel);
void old_can_see(location p1,location p2,/* short check_light,*/ short check_travel,Boolean *see_to,Boolean *see_in);

// Boolean can_see_to(location p1,location p2,short check_light,short check_travel);
Boolean can_see_in(location p1,location p2,short check_light,short check_travel);
void can_see(location p1,location p2,short check_light,short check_travel,Boolean *see_to,Boolean *see_in);

Boolean can_see_single(location p1,location p2,short check_light,short check_travel,Boolean *see_to);
Boolean no_block(location l, short direction,short check_light,short check_travel);

Boolean look_block(location l, short direction);
Boolean move_block(location l, short direction);
void set_drawing_mode(short new_mode);
void recursive_clean_terrain(location l);
void recursive_adjust_space_height_raise(location l);
void recursive_adjust_space_height_lower(location l);
Boolean control_key_down();

void set_see_in(short sector_offset_x, short sector_offset_y, short x, short y, Boolean value);
void set_see_to(short sector_offset_x, short sector_offset_y, short x, short y, Boolean value);
void set_nw_corner(short sector_offset_x, short sector_offset_y, short x, short y, short value);
void set_sw_corner(short sector_offset_x, short sector_offset_y, short x, short y, short value);
void set_se_corner(short sector_offset_x, short sector_offset_y, short x, short y, short value);
void set_ne_corner(short sector_offset_x, short sector_offset_y, short x, short y, short value);
void find_out_about_corner_walls(outdoor_record_type *drawing_terrain, short x, short y, short current_size, short *nw_corner, short *ne_corner, short *se_corner, short *sw_corner);
void find_out_about_corner_walls_being_hidden(outdoor_record_type *drawing_terrain, short sector_offset_x, short sector_offset_y,
											  short x, short y, /* short current_size,*/ Boolean see_in_neighbors[3][3],
											  /* Boolean see_to_neighbors[3][3], Boolean see_to, */
											  short *nw_corner, short *ne_corner, short *se_corner, short *sw_corner);
Boolean is_wall_drawn(outdoor_record_type *drawing_terrain, short sector_offset_x, short sector_offset_y, short x, short y);

void init_screen_locs()
{
	int i;
	for (i = 0; i < 14; i++)
		SetRECT(left_text_lines[i],LEFT_TEXT_LINE_WIDTH * (i / 7) + LEFT_TEXT_LINE_ULX,
			LEFT_TEXT_LINE_ULY + (i % 7) * TEXT_LINE_HEIGHT,
			LEFT_TEXT_LINE_WIDTH * (i / 7) + LEFT_TEXT_LINE_ULX + LEFT_TEXT_LINE_WIDTH,
			LEFT_TEXT_LINE_ULY + (i % 7) * TEXT_LINE_HEIGHT + TEXT_LINE_HEIGHT);
// right_text_lines rects are for INSIDE the buttons GWORLD, NOT the screen.
	for (i = 0; i < 6; i++)
		SetRECT(right_text_lines[i], RIGHT_TEXT_LINE_ULX,
			RIGHT_TEXT_LINE_ULY + (i % 6) * TEXT_LINE_HEIGHT,
			 RIGHT_TEXT_LINE_ULX + 200,
			RIGHT_TEXT_LINE_ULY + (i % 6) * TEXT_LINE_HEIGHT + TEXT_LINE_HEIGHT);
}


// "Outdoor: drawing mode failure after moving section" fix
// rewrite mouse click handling for the scroller buttons (bars)
// to an orthodox mouse tracker method
// In this method, the event loop is handled in process_scroll_click()
// and no other command dispatcher is concerned.
// Thus this structure constrains the mouse command only to the scroller
// and minimizes unexpected changes to the other part.

// detect on which scroll area mouse click drops
int check_scroller( POINT the_point )
{
	if ((cur_viewing_mode == 0) || (cur_viewing_mode == 2))
		return check_scroller_2D( the_point );
	if (cur_viewing_mode == 10 || cur_viewing_mode == 11)
		return check_scroller_3D( the_point );
	return eSCRL_NoScrl;
}

// check four rectangle side bars
int check_scroller_2D( POINT the_point )
{
	int scrl = eSCRL_NoScrl;
	if ( POINTInRECT( the_point, border_rect[0] ))	scrl |= eSCRL_Top;
	if ( POINTInRECT( the_point, border_rect[1] ))	scrl |= eSCRL_Left;
	if ( POINTInRECT( the_point, border_rect[2] ))	scrl |= eSCRL_Bottom;
	if ( POINTInRECT( the_point, border_rect[3] ))	scrl |= eSCRL_Right;
	return scrl;
}

// check four triangle corners
int check_scroller_3D( POINT the_point )
{
	int scrl = eSCRL_NoScrl;
	if( POINTInRECT( the_point, kRect3DEditScrn )) {
		if ( abs(the_point.y - kRect3DEditScrn.top) + abs(the_point.x - kRect3DEditScrn.left) < 32 )
			scrl |= eSCRL_Left;
		if ( abs(the_point.y - kRect3DEditScrn.top) + abs(the_point.x - (kRect3DEditScrn.right - 1)) < 32 )
			scrl |= eSCRL_Top;
		if ( abs(the_point.y - (kRect3DEditScrn.bottom - 1)) + abs(the_point.x - (kRect3DEditScrn.right - 1)) < 32 )
			scrl |= eSCRL_Right;
		if ( abs(the_point.y - (kRect3DEditScrn.bottom - 1)) + abs(the_point.x - kRect3DEditScrn.left) < 32 )
			scrl |= eSCRL_Bottom;
	}
	return scrl;
}

// track mouse click until its button is released

bool process_scroll_click( int map_size, POINT thePoint  )
{
	int scrl;
	bool alt_key;
	bool ctrl_key;
	bool shft_key;
	POINT currPt;

	if ( check_scroller( thePoint ) == eSCRL_NoScrl )
		return false;

	// Track mouse while it is down
	DWORD nextTick;
	DWORD prevTick = GetTickCount();
	bool still_down = true;
	while ( still_down ) {

		// handle movement
		if ( GetCursorPos( &currPt ) ) {
			ScreenToClient( mainPtr, &currPt );
	ctrl_key = (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0;	// check MSB for current key state
	alt_key = (GetAsyncKeyState( VK_MENU ) & 0x8000) != 0;
	shft_key = (GetAsyncKeyState(0xC0) & 0x8000) != 0;

			if ( (scrl = check_scroller( currPt )) != eSCRL_NoScrl )
				if ( handle_scroll( map_size, scrl, ctrl_key, shft_key ) )
					return true;
		}

		MSG msg;
		if ( PeekMessage(&msg, NULL, WM_MOUSEFIRST , WM_MOUSELAST, PM_REMOVE) != 0 ) {

			switch (msg.message) {

			// handle accept messages
			case WM_LBUTTONUP:
				still_down = false;
				break;

			// ignore these messages
			case WM_MOUSEMOVE:
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
		// limit too fast scroll
		nextTick = prevTick + kScrollLimitTime;
//		if ( nextTick < prevTick )		// take care of the wrap around case - 49 days
//			while ( nextTick < GetTickCount() );
		while ( nextTick > GetTickCount() );
		prevTick = GetTickCount();
	}

	return true;
}

// execute scroll
bool handle_scroll( int map_size, int scrl, bool ctrl_key, bool shft_key )
{
	int dx = 0;		// displacement
	int dy = 0;


	if (hintbook_mode0 == 1) {
	if (ctrl_key) {
		if ( scrl & eSCRL_Top )		dy = -cen_y;		// go to the limb
		if ( scrl & eSCRL_Left )	dx = -cen_x;
		if ( scrl & eSCRL_Bottom )	dy = map_size - 1 - cen_y;
		if ( scrl & eSCRL_Right )	dx = map_size - 1 - cen_x;
	}
	else if (shft_key) {
		if ( scrl & eSCRL_Top )		dy = -8;
		if ( scrl & eSCRL_Left )	dx = -8;
		if ( scrl & eSCRL_Bottom )	dy = 8;
		if ( scrl & eSCRL_Right )	dx = 8;
	}
	else {
		if ( scrl & eSCRL_Top )		dy = -1;
		if ( scrl & eSCRL_Left )	dx = -1;
		if ( scrl & eSCRL_Bottom )	dy = 1;
		if ( scrl & eSCRL_Right )	dx = 1;
	}
	if(clean_up_from_scrolling( map_size, dx, dy ))
		return true;

	if ( scrl != eSCRL_NoScrl )
		draw_main_screen();

	return false;
	
	}

	else {
	if (ctrl_key) {
      if (cur_viewing_mode == 2) {
		if ( scrl & eSCRL_Top )		dy = 16 - cen_y;		// go to the limb
		if ( scrl & eSCRL_Left )	dx = 16 - cen_x;
		if ( scrl & eSCRL_Bottom )	dy = map_size - 16 - cen_y;
		if ( scrl & eSCRL_Right )	dx = map_size - 16 - cen_x;
		}
      else {
		if ( scrl & eSCRL_Top )		dy = -cen_y;		// go to the limb
		if ( scrl & eSCRL_Left )	dx = -cen_x;
		if ( scrl & eSCRL_Bottom )	dy = map_size - 1 - cen_y;
		if ( scrl & eSCRL_Right )	dx = map_size - 1 - cen_x;
			 }
	}
	else if (shft_key) {
      if ((cur_viewing_mode == 2) && (cen_x <= 23)) {
					if ( scrl & eSCRL_Left )	dx = 0;
					}
	else {
					if ( scrl & eSCRL_Left )	dx = -8;
		}

      if ((cur_viewing_mode == 2) && (cen_y <= 23)) {
					if ( scrl & eSCRL_Top )		dy = 0;
					}
	else {
					if ( scrl & eSCRL_Top )		dy = -8;
		}

      if ((cur_viewing_mode == 2) && (cen_x >= map_size - 23)) {
					if ( scrl & eSCRL_Right )	dx = 0;
					}
	else {
					if ( scrl & eSCRL_Right )	dx = 8;
		}

      if ((cur_viewing_mode == 2) && (cen_y >= map_size - 23)) {
					if ( scrl & eSCRL_Bottom )	dy = 0;
					}
	else {
					if ( scrl & eSCRL_Bottom )	dy = 8;
		}
					}

	else {
      if ((cur_viewing_mode == 2) && (cen_x <= 16)) {
					if ( scrl & eSCRL_Left )	dx = 0;
					}
	else {
					if ( scrl & eSCRL_Left )	dx = -1;
		}

      if ((cur_viewing_mode == 2) && (cen_y <= 16)) {
					if ( scrl & eSCRL_Top )		dy = 0;
					}
	else {
					if ( scrl & eSCRL_Top )		dy = -1;
		}

      if ((cur_viewing_mode == 2) && (cen_x >= map_size - 16)) {
					if ( scrl & eSCRL_Right )	dx = 0;
					}
	else {
					if ( scrl & eSCRL_Right )	dx = 1;
		}

      if ((cur_viewing_mode == 2) && (cen_y >= map_size - 16)) {
					if ( scrl & eSCRL_Bottom )	dy = 0;
					}
	else {
					if ( scrl & eSCRL_Bottom )	dy = 1;
		}
					}

	if(clean_up_from_scrolling( map_size, dx, dy ))
		return true;

	if ( scrl != eSCRL_NoScrl )
		draw_main_screen();

	return false;
	}
}

// "Outdoor: drawing mode failure after moving section" fix
// returns TRUE when "save change" dialog is displayed
// if dialog is displayed, mouse tracking should be stopped

Boolean clean_up_from_scrolling( int map_size, int dx, int dy )
{
	int sector_x_overflow = 0;
	int sector_y_overflow = 0;
	char sector_offset_x = 0;
	char sector_offset_y = 0;

	short cen_x_save = cen_x;
	short cen_y_save = cen_y;
	
	cen_x = cen_x + (short)dx;
	cen_y = cen_y + (short)dy;

	sector_x_overflow = cen_x % map_size;
	if(cen_x < 0) {
		sector_offset_x = -1;
		sector_x_overflow += map_size;
	}
	else if(cen_x >= map_size) {
		sector_offset_x = 1;
	}

	sector_y_overflow = cen_y % map_size;
	if(cen_y < 0) {
		sector_offset_y = -1;
		sector_y_overflow += map_size;
	}
	else if(cen_y >= map_size) {
		sector_offset_y = 1;
	}

	// limit center position
	cen_x = minmax( 0, (short)map_size - 1, cen_x );
	cen_y = minmax( 0, (short)map_size - 1, cen_y );

	if( !editing_town && ((sector_offset_x != 0) || (sector_offset_y != 0)) ) {
		// change outdoor sector
		location sector_move_to = {cur_out.x + sector_offset_x,cur_out.y + sector_offset_y};

		if((sector_move_to.x < 0) || (sector_move_to.y < 0)
			|| (sector_move_to.x >= scenario.out_width)
			|| (sector_move_to.y >= scenario.out_height))
			return FALSE;

		bool save_dlg_displayed = false;
		if (change_made_outdoors == TRUE) {
			save_dlg_displayed = true;
			if ( save_check(990) == FALSE ) {	// canceled
				cen_x = cen_x_save;				// recover center position
				cen_y = cen_y_save;
				return TRUE;					// stop mouse tracking
			}
		}

		clear_selected_copied_objects();
		load_outdoor_and_borders( sector_move_to );
		set_up_terrain_buttons();
		cen_x = (short)sector_x_overflow;
		cen_y = (short)sector_y_overflow;
//		reset_drawing_mode();
		check_selected_item_number();		// maybe need?
		if (current_drawing_mode == 0)
			set_new_floor(current_floor_drawn);
		else
			set_new_terrain(current_terrain_drawn);
		small_any_drawn = TRUE;

// redraw by WM_PAINT on main WndProc, because dialog area is invalidated
//		if ( change_made_outdoors == TRUE )		// if dialog is displayed
//			redraw_screen();					//   redraw full screen
//		else									// else
//			draw_main_screen();					//   only edit screen and terrain palette

		change_made_outdoors = FALSE;
		return save_dlg_displayed;
	}
	return FALSE;
}

// "Outdoor: drawing mode failure after moving section" fix
// mouse click to the scroller is handled on separate routines.

Boolean handle_action(POINT the_point, WPARAM wparam, LPARAM lparam )
{
	short i,j;
	Boolean are_done = FALSE;
	char str_response[256] = "";
	void put_placed_terrain_script_in_dlog();
	Boolean need_redraw = FALSE, option_hit = FALSE, right_click = FALSE;

	location spot_hit;
	POINT cur_point,cur_point2;
	short old_mode,choice;

	if (file_is_loaded == FALSE) 
		return are_done; 
	
	Boolean ctrl_key = FALSE;
	Boolean shift_key = FALSE;

	if (MK_CONTROL & wparam)
		option_hit = ctrl_key = TRUE;

	if (MK_SHIFT & wparam)
		shift_key = TRUE;

	if (lparam == -2)
		option_hit = TRUE;
	if (lparam != -1)
		the_point.x -= ulx;		the_point.y -= uly;

	right_click = (GetKeyState( VK_RBUTTON ) & 0x8000) != 0;

	int map_size = (editing_town) ? max_dim[town_type] : 48;
	cur_point = the_point;

	// scroller on edit screen
	if ( process_scroll_click( map_size, the_point ) ){
		mouse_button_held = FALSE;
		return are_done;
	}

	// clicking in terrain spots, big icon mode
	if (cur_viewing_mode == 0) {
		cur_point2 = cur_point;
		cur_point2.x -= TER_RECT_UL_X; cur_point2.y -= TER_RECT_UL_Y;
		for (i = 0; i < 9; i++)
			for (j = 0; j < 9; j++)
				if (POINTInRECT(cur_point2,large_edit_ter_rects[i][j])) {
					spot_hit.x = (t_coord)(cen_x + i - 4);
					spot_hit.y = (t_coord)(cen_y + j - 4);					

					if ((mouse_button_held == TRUE) && (spot_hit.x == last_spot_hit.x) &&
						(spot_hit.y == last_spot_hit.y))
						return are_done;
						else last_spot_hit = spot_hit;
					if (mouse_button_held == FALSE)
						last_spot_hit = spot_hit;
					
					old_mode = overall_mode;
					if(editing_town)
						change_made_town = TRUE;
					else
						change_made_outdoors = TRUE;

					handle_ter_spot_press(spot_hit,option_hit,right_click);
					
					need_redraw = TRUE;
				}
	}

	// clicking in terrain spots, small icon mode
	if (cur_viewing_mode == 1) {
		cur_point2 = cur_point;
		cur_point2.x -= TER_RECT_UL_X; cur_point2.y -= TER_RECT_UL_Y;
		for (i = 0; i < MAX_TOWN_SIZE; i++)
			for (j = 0; j < MAX_TOWN_SIZE; j++)
				if (POINTInRECT(cur_point2,small_edit_ter_rects[i][j])) {
					spot_hit.x = (t_coord)i;
					spot_hit.y = (t_coord)j;					

					if ((mouse_button_held == TRUE) && (spot_hit.x == last_spot_hit.x) &&
						(spot_hit.y == last_spot_hit.y))
						return are_done;
						else last_spot_hit = spot_hit;
					if (mouse_button_held == FALSE)
						last_spot_hit = spot_hit;
					
					old_mode = overall_mode;
					if(editing_town)
						change_made_town = TRUE;
					else
						change_made_outdoors = TRUE;
							
					handle_ter_spot_press(spot_hit,option_hit,right_click);			
					need_redraw = TRUE;
				}
	}

	// clicking in terrain spots, medium icon mode
	if (cur_viewing_mode == 2) {
		cur_point2 = cur_point;
		cur_point2.x -= TER_RECT_UL_X; cur_point2.y -= TER_RECT_UL_Y;
		for (i = 0; i < 32; i++)
			for (j = 0; j < 32; j++)
				if (POINTInRECT(cur_point2,medium_edit_ter_rects[i][j])) {
					spot_hit.x = (t_coord)(cen_x + i - 16);
					spot_hit.y = (t_coord)(cen_y + j - 16);

					if ((mouse_button_held == TRUE) && (spot_hit.x == last_spot_hit.x) &&
						(spot_hit.y == last_spot_hit.y))
						return are_done;
						else last_spot_hit = spot_hit;
					if (mouse_button_held == FALSE)
						last_spot_hit = spot_hit;

					old_mode = overall_mode;
					if(editing_town)
						change_made_town = TRUE;
					else
						change_made_outdoors = TRUE;

					handle_ter_spot_press(spot_hit,option_hit,right_click);
					need_redraw = TRUE;
				}
	}


	// clicking in terrain spots, big 3D icon mode
	else if (cur_viewing_mode == 10 || cur_viewing_mode == 11) {
		if( POINTInRECT(cur_point, kRect3DEditScrn)) {

			//detect click on terrain spot (possibilities: that <-, hit a cliff, or hit outside the map.  last two do nothing)
			cur_point2 = cur_point;
			cur_point2.x -= TER_RECT_UL_X; cur_point2.y -= TER_RECT_UL_Y;
			short done = FALSE;
			short x = 0, y = 0;
			long mid_tenth_x, mid_tenth_y, result, x_off_from_center, y_off_from_center;
			short height_to_draw;
			graphic_id_type a;
			
			short current_size = ((editing_town) ? max_dim[town_type] : 48);
			short center_area_x, center_area_y;
			short center_of_current_square_x, center_of_current_square_y;
			short center_height;
			short rel_x, rel_y;
			
			RECT whole_area_rect = terrain_viewport_3d;
				/*= {large_edit_ter_rects[0][0].left,large_edit_ter_rects[0][0].top,
				large_edit_ter_rects[8][8].right,large_edit_ter_rects[8][8].bottom};*/
			
			//MacInsetRect(&whole_area_rect,-15,-15);
			ZeroRectCorner(&whole_area_rect);
			
			
			center_area_x = (short)(whole_area_rect.right / 2);
			center_area_y = (short)(whole_area_rect.bottom / 2);
			
			center_height = (editing_town) ? t_d.height[cen_x][cen_y] : current_terrain.height[cen_x][cen_y];

			for(x = current_size - 1; x >= 0; x--) {
				for(y = current_size - 1; y >= 0; y--) {
					height_to_draw = (editing_town) ? t_d.height[x][y] : current_terrain.height[x][y];
					
					rel_x = x - cen_x;
					rel_y = y - cen_y;
					
					center_of_current_square_x = (rel_x - rel_y) * SPACE_X_DISPLACEMENT_3D + center_area_x;
					center_of_current_square_y = (rel_x + rel_y) * SPACE_Y_DISPLACEMENT_3D + center_area_y
						- (height_to_draw - center_height) * ELEVATION_Y_DISPLACEMENT_3D;
					//convert the center
					center_of_current_square_y += 11 + 2;//I thought 11 was the right amount...
					
					
					//ugly click detection
					mid_tenth_x = center_of_current_square_x * 10 + 5;
					mid_tenth_y = center_of_current_square_y * 10 + 5;
					
					result = abs(mid_tenth_x - cur_point2.x * 10L) * SPACE_Y_DISPLACEMENT_3D +
								abs(mid_tenth_y - cur_point2.y * 10L) * SPACE_X_DISPLACEMENT_3D;
					
					//this is distance for 2D - in case anyone wants to implement BetterEditor's "wall drawing" in 3D
					//divide this number by a lot if you want
					//( / about 200 if you want to get back to anything near pixel size, I think)
					//A warning, however: I haven't tested this code. If you try something and it goes wrong, consider this.
					x_off_from_center = (mid_tenth_x - cur_point2.x * 10L) * SPACE_Y_DISPLACEMENT_3D +
								(mid_tenth_y - cur_point2.y * 10L) * SPACE_X_DISPLACEMENT_3D;
					y_off_from_center = -(mid_tenth_x - cur_point2.x * 10L) * SPACE_Y_DISPLACEMENT_3D +
								(mid_tenth_y - cur_point2.y * 10L) * SPACE_X_DISPLACEMENT_3D;
					
					if(result >= 0 && result <= SPACE_X_DISPLACEMENT_3D * SPACE_Y_DISPLACEMENT_3D * 10) {
						done = TRUE;
					/*Str255 draw_str;
					Rect the_rect = {200,200,400,400};
					sprintf((char *) draw_str,"%d,%d,%d,%d,%d,%d,%d",(long)cur_point2.h,(long)cur_point2.v,
					(long)center_of_current_square_x,(long)center_of_current_square_y,(long)mid_tenth_x,
					(long)mid_tenth_y,(long)result); 
					char_win_draw_string(GetWindowPort(mainPtr),the_rect,(char *) draw_str,2,12);*/
						break;
					}
					
					//cliff checking (anything within the square's width and below its center)
					if(cur_point2.y > center_of_current_square_y &&
					cur_point2.x > center_of_current_square_x - SPACE_X_DISPLACEMENT_3D + 1 &&
					cur_point2.x < center_of_current_square_x + SPACE_X_DISPLACEMENT_3D) {
						done = 2;
					/*Str255 draw_str;
					Rect the_rect = {200,200,400,400};
					sprintf((char *) draw_str,"%d,%d,%d,%d,%d,%d,%d",(long)cur_point2.h,(long)cur_point2.v,
					(long)center_of_current_square_x,(long)center_of_current_square_y,(long)mid_tenth_x,
					(long)mid_tenth_y,(long)result); 
					char_win_draw_string(GetWindowPort(mainPtr),the_rect,(char *) draw_str,2,12);*/
						beep();//this might actually be a good idea permanently
						break;
					}
					/*if(q + 1 == current_num_diagonals && r + 1 == current_diagonal_length){
					Str255 draw_str;
					Rect the_rect = {200,200,400,400};
					sprintf((char *) draw_str,"%d,%d,%d,%d,%d,%d,%d",(long)cur_point2.h,(long)cur_point2.v,
					(long)center_of_current_square_x,(long)center_of_current_square_y,(long)mid_tenth_x,
					(long)mid_tenth_y,(long)result); 
					char_win_draw_string(GetWindowPort(mainPtr),the_rect,(char *) draw_str,2,12);
					}*/
				}
				if(done != FALSE)
					break;
			}
			if(done == TRUE) {
				spot_hit.x = (char)x;
				spot_hit.y = (char)y;
				
				if ((mouse_button_held == TRUE) && (spot_hit.x == last_spot_hit.x) && (spot_hit.y == last_spot_hit.y))
					return are_done;
				else
					last_spot_hit = spot_hit;
					
				old_mode = overall_mode;
				if(editing_town)
					change_made_town = TRUE;
				else
					change_made_outdoors = TRUE;
					//erasing_mode = TRUE;
					//mouse_button_held = TRUE;		
					
				handle_ter_spot_press(spot_hit,option_hit,right_click);			
				
				need_redraw = TRUE;
			}
			else if(done == FALSE) {
				beep();
			}
		}
	}
// q_3DModEnd

	// TERRAIN BUTTONS TO RIGHT
	if (mouse_button_held == FALSE) {
		cur_point = the_point;
		cur_point.x -= RIGHT_BUTTONS_X_SHIFT;
		
		if (current_drawing_mode == 0) { // floor mode
			for (i = 0; i < 256; i++)

// q_3DModStart
//				if (point_in_rect(cur_point,&terrain_rects[i])) {
				if (POINTInRECT(cur_point,((cur_viewing_mode >= 10 && current_drawing_mode > 0) ? terrain_rects_3D[i] : terrain_rects[i]))) {
					reset_drawing_mode();
					set_new_floor(i);
					}
// q_3DModEnd

			}
			else { // terrain/height mode
				short sbar_pos = GetControlValue(right_sbar);

				for (i = 0; i < 264; i++) 
					if (sbar_pos * 12 + i < 512) {
// q_3DModStart
//						if (point_in_rect(cur_point,&terrain_rects[i])) {
						if (POINTInRECT(cur_point,((cur_viewing_mode >= 10 && current_drawing_mode > 0) ? terrain_rects_3D[i] : terrain_rects[i]))) {
							if (current_drawing_mode != 1)
								set_drawing_mode(1);
							reset_drawing_mode();
							set_new_terrain(sbar_pos * 12 + i);
							}
						}
// q_3DModEnd
					}		

				
				//place_right_buttons(0);
		}

		// PRESSING MAIN BUTTONS
		for (i = 0; i < 8; i++)
			for (j = 0; j < ((editing_town) ? 6 : 3); j++) {
			cur_point2 = the_point;
				cur_point2.x -= RIGHT_BUTTONS_X_SHIFT;
				if ((mouse_button_held == FALSE) && (POINTInRECT(cur_point2,palette_buttons[i][j]))) {
					play_sound(34);
					switch (i + 100 * j) {
						case 0: // regular drawing mode
							//pre-emptive support for creature and item placement modes
							if(current_drawing_mode==3){
								overall_mode = 46;
								object_sticky_draw = shift_key;
							}
							else if(current_drawing_mode==4){
								overall_mode = 47;
								object_sticky_draw = shift_key;
							}
							else
								reset_drawing_mode();
							break;					
						case 1: // paintbrush 1
							set_string("Paintbrush (large)","");
							overall_mode = 1;
							set_cursor(2);
							break;					
						case 2: // paintbrush 2
							set_string("Paintbrush (small)","");
							overall_mode = 2;
							set_cursor(2);
							break;					
						case 3: // spray can 1
							set_string("Spraycan (large)","");
							overall_mode = 3;
							set_cursor(3);
							break;					
						case 4: // spray can 2
							set_string("Spraycan (small)","");
							overall_mode = 4;
							set_cursor(3);
							break;					
						case 5: // set/add height rectangle
							set_string("Set Height","Select rectangle to set");
							mode_count = 2;
							set_cursor(5);
							overall_mode = (shift_key?24:20); //if shift is on, add height
							need_redraw = TRUE;
							break;					
						case 6: case 7: // empty, full rectangle
							overall_mode = (i == 6) ? 11 : 10;
							mode_count = 2;
							set_cursor(5);
							if (i == 6)
								set_string("Fill rectangle (hollow)","Select upper left corner");
									else set_string("Fill rectangle (solid)","Select upper left corner");
							break;					
				
						case 100: // switch view size
// q_3DModStart
							if ((cur_viewing_mode == 1) || (cur_viewing_mode == 2))
								cur_viewing_mode = 0;
							else if(cur_viewing_mode == 0)
								cur_viewing_mode = 1;
							else if(cur_viewing_mode == 10) {
								cur_viewing_mode = 11;
								set_up_lights();
							}
							else if(cur_viewing_mode == 11)
								cur_viewing_mode = 10;
							
							set_up_terrain_buttons();
// q_3DModEnd

							reset_small_drawn();
							redraw_screen();
							break;
						case 101: // cycle drawing mode
							set_drawing_mode((current_drawing_mode + 1) % 3);
						
							need_redraw = TRUE;
							break;		
						case 102: // switch height mode
							current_height_mode = 1 - current_height_mode;
							need_redraw = TRUE;
							break;		
						case 103: // edit sign
							set_string("Edit sign","Select sign to edit");
							set_cursor(7);
							overall_mode = 59;
							break;		
						case 104: // create text rectangle
							overall_mode = 21;
							mode_count = 2;
							set_cursor(5);
							set_string("Create room rectangle","Select upper left corner");
							break;		
						case 105: // place wandering monster pts
							overall_mode = 60;
							mode_count = (editing_town == TRUE) ? 6 : 4;
							set_cursor(7);
							set_string("Place 1st spawn point","");
							break;		
						case 106: // make town entrance
							if (editing_town == TRUE) {
								beep();
								set_string("This only used outdoors","");
								break;
								}
							set_string("Create town entrance","Select upper left corner");
							mode_count = 2;
							set_cursor(5);
							overall_mode = 22;
							break;		
						case 107: // edit town entrance
							if (editing_town == TRUE) {
								beep();
								set_string("This only used outdoors","");
								break;
								}
							set_string("Edit town entrance","");
							overall_mode = 69;
							set_cursor(7);
							break;
					
						case 200: // create walls
							set_string("Place bounding walls","");
							mode_count = 2;
							set_cursor(5);
							overall_mode = 19;
							break;
						case 201: // Swap walls
							set_string("Swap walls 1 <--> 2","Select rectangle to set");
							mode_count = 2;
							set_cursor(5);
							overall_mode = 18;
							break;		
						case 202:  // find-replace terrain
							swap_terrain();
							need_redraw = TRUE;
							mouse_button_held = FALSE;
							break;		
						case 203: // eyedropper
							set_cursor(1);		
							set_string("Click on terrain/floor to select it.","   ");				
							overall_mode = 73;
						break;		
// q_3DModStart
						case 204: //switch 3D on/off
							if(cur_viewing_mode >= 10) {
								cur_viewing_mode = 0;
								if(current_cursor == 8)
									set_cursor(5);
								if(current_cursor == 9)
									set_cursor(6);
								// do proper modifications of scroll bar
								if (current_drawing_mode != 0) {
									//SetControlMinimum(right_sbar,0);
//									SetControlMaximum(right_sbar,22);
									SetScrollRange(right_sbar,SB_CTL,0,22,TRUE);
									//SetControlValue(right_sbar,store_control_value);
								}
							}
							else {
								cur_viewing_mode = 10;
								if(current_cursor == 5)
									set_cursor(8);
								if(current_cursor == 6)
									set_cursor(9);
								// do proper modifications of scroll bar
								if (current_drawing_mode != 0) {
									//SetControlMinimum(right_sbar,0);
//									SetControlMaximum(right_sbar,25);
									SetScrollRange(right_sbar,SB_CTL,0,25,TRUE);
									//SetControlValue(right_sbar,store_control_value);
								}
							}
							set_up_terrain_buttons();
							reset_small_drawn();
							redraw_screen();
							break;		
// q_3DModEnd

						case 205: // place spec enc
							set_string("Create Special Encouter","Select rectangle for encounter");
							mode_count = 2;
							set_cursor(5);
							overall_mode = 16;
							break;		
						case 206: // Erase special
							set_string("Erase Special Encouter","");
							overall_mode = 49;
							set_cursor(4);
							break;		
						case 207: // Set special
							set_string("Edit Special Encouter","");
							overall_mode = 50;
							set_cursor(7);
							break;		

						case 300: //Select/edit placed item
							set_string("Select/edit placed object","Select object to edit");
							set_cursor(7);
							overall_mode = 40;
							break;		
						case 301: // Delete an item
							set_string("Delete an object","Select object");
							set_cursor(7);
							overall_mode = 41;
							break;		
						case 302: // Place Navpoint
							set_string("Place a Waypoint","");
							set_cursor(7);
							overall_mode = 57;
							break;		
						case 303: // Delete Waypoint
							set_string("Delete Waypoint","");
							set_cursor(7);
							overall_mode = 58;
							break;		
						case 304: 
							set_string("Place north entrance","Select entrance location");
							set_cursor(7);
							overall_mode = 30;
							break;
						case 305: 
							set_string("Place west entrance","Select entrance location");
							set_cursor(7);
							overall_mode = 31;
							break;
						case 306: 
							set_string("Place south entrance","Select entrance location");
							set_cursor(7);
							overall_mode = 32;
							break;
						case 307: 
							set_string("Place east entrance","Select entrance location");
							set_cursor(7);
							overall_mode = 33;
							break;

						case 400: // Make Spot Blocked
							set_string("Make Spot Blocked","Select location");
							overall_mode = 61;
							object_sticky_draw = shift_key;
							set_cursor(10);
							break;		
						case 401: 
							set_string("Place web","Select location");
							overall_mode = 62;
							object_sticky_draw = shift_key;
							set_cursor(11);
							break;		
						case 402: 
							set_string("Place crate","Select location");
							overall_mode = 63;
							object_sticky_draw = shift_key;
							set_cursor(12);
							break;		
						case 403: 
							set_string("Place barrel","Select location");
							overall_mode = 64;
							object_sticky_draw = shift_key;
							set_cursor(13);
							break;		
						case 404: 
							set_string("Place fire barrier","Select location");
							overall_mode = 65;
							object_sticky_draw = shift_key;
							set_cursor(14);
							break;
						case 405: 
							set_string("Place force barrier","Select location");
							overall_mode = 66;
							object_sticky_draw = shift_key;
							set_cursor(14);
							break;		
						case 406: 
							set_string("Clear space","Select space to clear");
							overall_mode = 67;
							object_sticky_draw = shift_key;
							set_cursor(4);
							break;
						case 407: // place terrain script
							if (editing_town == FALSE) {
								set_string("Place Terrain Script","Only in towns.");
								break;
								}
							set_string("Place Terrain Script","");
							overall_mode = 70;
							set_cursor(7);
							break;

						case 500: 
							set_string("Place small blood stain","Select stain location");
							overall_mode = 68; mode_count = 0;
							object_sticky_draw = shift_key;
							set_cursor(0);
							break;
						case 501: 
							set_string("Place ave. blood stain","Select stain location");
							overall_mode = 68; mode_count = 1;
							object_sticky_draw = shift_key;
							set_cursor(0);
							break;
						case 502: 
							set_string("Place large blood stain","Select stain location");
							overall_mode = 68; mode_count = 2;
							object_sticky_draw = shift_key;
							set_cursor(0);
							break;
						case 503: 
							set_string("Place small slime pool","Select slime location");
							overall_mode = 68; mode_count = 3;
							object_sticky_draw = shift_key;
							set_cursor(0);
							break;
						case 504: 
							set_string("Place large slime pool","Select slime location");
							overall_mode = 68; mode_count = 4;
							object_sticky_draw = shift_key;
							set_cursor(0);
							break;
						case 505: 
							set_string("Place dried blood","Select dried blood location");
							overall_mode = 68; mode_count = 5;
							object_sticky_draw = shift_key;
							set_cursor(0);
							break;
						case 506: 
							set_string("Place bones","Select bones location");
							overall_mode = 68; mode_count = 6;
							object_sticky_draw = shift_key;
							set_cursor(0);
							break;
						case 507: 
							set_string("Place rocks","Select rocks location");
							overall_mode = 68; mode_count = 7;
							object_sticky_draw = shift_key;					
							set_cursor(0);
							break;
		
					
					}
				
				place_right_buttons(/* 0 */);
				
				}
		} // end right buttons

	// TEXT LINES TO LOWER LEFT
	// Clicking on the text lines to the lower left enables you to change a wide range of instance properties

// q_3DModStart
	if ((selected_item_number >= 0) && ((cur_viewing_mode == 0) || (cur_viewing_mode == 10) || (cur_viewing_mode == 11))) {
// q_3DModEnd
		for (i = 0; i < 14; i++)
			if ((mouse_button_held == FALSE) && (POINTInRECT(the_point,left_text_lines[i]))) {
				// if editing an item
				if ((selected_item_number >= 11000) && (selected_item_number < 11000 + NUM_TOWN_PLACED_ITEMS)) {
					switch (i) {
						case 0:
							choice = choose_text_res(-2,0,NUM_SCEN_ITEMS - 1,town.preset_items[selected_item_number % 1000].which_item,
							  0,"What sort of item?");
							if (choice >= 0)
								town.preset_items[selected_item_number % 1000].which_item = choice;			
							break;
// case 1: Edit this item
						case 2:
							town.preset_items[selected_item_number % 1000].properties =
								town.preset_items[selected_item_number % 1000].properties ^ 1;
							break;
						case 3:
							town.preset_items[selected_item_number % 1000].properties = 
								town.preset_items[selected_item_number % 1000].properties ^ 2;
							break;
						case 4:
							town.preset_items[selected_item_number % 1000].properties = 
								town.preset_items[selected_item_number % 1000].properties ^ 4;
							break;
						case 5:
							town.preset_items[selected_item_number % 1000].properties =
								town.preset_items[selected_item_number % 1000].properties ^ 8;
							break;
						case 6:
							town.preset_items[selected_item_number % 1000].properties =
								town.preset_items[selected_item_number % 1000].properties ^ 16;
							break;
						case 8:
								town.preset_items[selected_item_number % 1000].item_shift.x =
									(t_coord)how_many_dlog(town.preset_items[selected_item_number % 1000].item_shift.x,-5,5,"Horizontal Pixel Offset  (-5..5)");
							break;
						case 9:
								town.preset_items[selected_item_number % 1000].item_shift.y =
									(t_coord)how_many_dlog(town.preset_items[selected_item_number % 1000].item_shift.y,-5,5,"Vertical Pixel Offset  (-5..5)");
							break;
							
						case 10:
							if (town.preset_items[selected_item_number % 1000].charges > 0) {
								town.preset_items[selected_item_number % 1000].charges =
									(unsigned char)how_many_dlog(town.preset_items[selected_item_number % 1000].charges,0,250,"How many charges?  (0-255)");
								if (town.preset_items[selected_item_number % 1000].charges <= 0)
									town.preset_items[selected_item_number % 1000].charges = 1;
								}
							break;

						}
					}

				if ((selected_item_number >= 9000) && (selected_item_number < 9000 + NUM_TER_SCRIPTS)) {
					switch (i) {
						case 1:
//						put_placed_terrain_script_in_dlog(selected_item_number % 1000);
						break;
						case 2: case 3: case 4: case 5: case 6:
							town.ter_scripts[selected_item_number % 1000].memory_cells[i - 2] =
								how_many_dlog(town.ter_scripts[selected_item_number % 1000].memory_cells[i - 2],-30000,30000,"Put what in this memory cell?");
							break;

						case 8:
							get_str_dlog(town.ter_scripts[selected_item_number % 1000].script_name,"What terrain script? (Name must be 13 characters or less).",str_response,TRUE);
							str_response[SCRIPT_NAME_LEN - 1] = 0;
							strcpy(town.ter_scripts[selected_item_number % 1000].script_name,str_response);
							break;
						case 9: case 10: case 11: case 12: case 13:
							town.ter_scripts[selected_item_number % 1000].memory_cells[i - 4] =
								how_many_dlog(town.ter_scripts[selected_item_number % 1000].memory_cells[i - 4],-30000,30000,"Put what in this memory cell?");
							break;
						default:
							break;
						}
					}
	
				if ((selected_item_number >= 7000) && (selected_item_number < 7000 + NUM_TOWN_PLACED_CREATURES)) {
				switch (i) {
					case 0:
						choice = choose_text_res(-1,0,255,town.creatures[selected_item_number % 1000].number,
						  0,"What sort of Creature?");
						if (choice >= 0)
							town.creatures[selected_item_number % 1000].number = choice;			
						break;
					case 1:
						edit_placed_monst(selected_item_number % 1000);
						break;
					case 2:
						town.creatures[selected_item_number % 1000].start_attitude =
						  (town.creatures[selected_item_number % 1000].start_attitude + 1) % 6;
						if (town.creatures[selected_item_number % 1000].start_attitude < 2)
							town.creatures[selected_item_number % 1000].start_attitude = 2;
						break;

					case 3:
						get_str_dlog(town.creatures[selected_item_number % 1000].char_script,"What creature script? (Name must be 13 characters or less).",str_response,TRUE);
						str_response[SCRIPT_NAME_LEN - 1] = 0;
						strcpy(town.creatures[selected_item_number % 1000].char_script,str_response);
						break;
					case 4:
						town.creatures[selected_item_number % 1000].personality =
							how_many_dlog(town.creatures[selected_item_number % 1000].personality,0,3999,"What personality?  (0-3999)");
						break;

					case 5:
						town.creatures[selected_item_number % 1000].character_id = 
							how_many_dlog(town.creatures[selected_item_number % 1000].character_id,0,19999,"What character id?  (0-19999)");
						break;
					case 6:
						town.creatures[selected_item_number % 1000].hidden_class =
							how_many_dlog(town.creatures[selected_item_number % 1000].hidden_class,0,19,"What hidden class?  (0-19)");
						break;

					case 8:
						town.creatures[selected_item_number % 1000].attached_event =
							how_many_dlog(town.creatures[selected_item_number % 1000].attached_event,0,9,"What event?  (0-9)");
						break;

					case 9:
						town.creatures[selected_item_number % 1000].facing = 
							(town.creatures[selected_item_number % 1000].facing + 1) % 4;						
						break;
					case 10: case 11: case 12: case 13:
							town.creatures[selected_item_number % 1000].memory_cells[i - 10] =
							how_many_dlog(town.creatures[selected_item_number % 1000].memory_cells[i - 10],-30000,30000,"Put what in this memory cell?");
						break;

					}
				}
			need_redraw = TRUE;	
			}
		}
	// Cleanup and error checking
	check_selected_item_number();

	if (need_redraw == TRUE) {
		draw_main_screen();
		//draw_terrain();
		}
		
	return are_done;
}

void set_drawing_mode(short new_mode)
{
	current_drawing_mode = new_mode;
							
	// do proper modifications of scroll bar
	if (current_drawing_mode == 0) {
		SetScrollRange(right_sbar,SB_CTL,0,0,TRUE);
		set_up_terrain_buttons();
		set_new_floor(current_floor_drawn);
	}
	else {
// q_3DModStart
//		SetScrollRange(right_sbar,SB_CTL,0,22,TRUE);
//		SetControlMinimum(right_sbar,0);
//		SetControlMaximum(right_sbar,(cur_viewing_mode >= 10) ? 25 : 22 );
		SetScrollRange(right_sbar,SB_CTL,0,(cur_viewing_mode >= 10) ? 25 : 22,TRUE);
// q_3DModEnd

		SetScrollPos(right_sbar,SB_CTL,store_control_value,TRUE);
		set_up_terrain_buttons();
		set_new_terrain(current_terrain_drawn);
	}
//	update_main_screen();
}
	
	
// when the player clicks on location spot_hit in a way that the click should be processed,
// this is the function that handles what happens.
// spot_hit is the zone location 
// game_world_point_selected is the exact point in the game world

// q_3DModStart
void handle_ter_spot_press(location spot_hit,Boolean option_hit,Boolean right_click)
// q_3DModEnd
{
// q_3DModStart
//	short current_terrain_type = 0;
// q_3DModEnd

	short i;
	short x;
	short item_to_try;

	if ((editing_town == TRUE) && 
	  ((spot_hit.x < 0) || (spot_hit.x > max_dim[town_type] - 1) || (spot_hit.y < 0) || (spot_hit.y > max_dim[town_type] - 1)))
		return ;
	if ((editing_town == FALSE) && 
	  ((spot_hit.x < 0) || (spot_hit.x > 47) || (spot_hit.y < 0) || (spot_hit.y > 47)))
		return ;

// q_3DModStart
	if(right_click) {
		cen_x = spot_hit.x;
		cen_y = spot_hit.y;
	if ((cur_viewing_mode == 1) || (cur_viewing_mode == 2)) {
			cur_viewing_mode = 0;
			set_up_terrain_buttons();
			reset_small_drawn();
			redraw_screen();
		}
		else {
			draw_main_screen();
		}
		return;
	}
// q_3DModEnd

	switch (current_drawing_mode) {
		case 0: current_terrain_type = current_floor_drawn;
			break;
		case 1: current_terrain_type = current_terrain_drawn;
			break;
		case 2: current_terrain_type = 0;
			break;
		}
	
	short ter_in_spot = (editing_town) ? t_d.terrain[spot_hit.x][spot_hit.y] : current_terrain.terrain[spot_hit.x][spot_hit.y];
	short floor_in_spot = (editing_town) ? t_d.floor[spot_hit.x][spot_hit.y] : current_terrain.floor[spot_hit.x][spot_hit.y];
			
	switch (overall_mode) {
		case 0: // just change terrain or select item
			switch (current_drawing_mode) {
			case 0: // floor
				  
				 if (((mouse_button_held == FALSE) && 
					(floors_match(current_floor_drawn,floor_in_spot))) ||
					((mouse_button_held == TRUE) && (erasing_mode == TRUE))) {
				 	if (editing_town)
				 		set_terrain(spot_hit,(town.is_on_surface == 0) ? 0 : 37);
					else
						set_terrain(spot_hit,(current_terrain.is_on_surface == 0) ? 0 : 37);
					set_cursor(0);
					erasing_mode = TRUE;
					mouse_button_held = TRUE;
				} else {
					mouse_button_held = TRUE;
					set_cursor(0);
				 	set_terrain(spot_hit,current_floor_drawn);
					erasing_mode = FALSE;
				}
				break;
			case 1: // terrain
				 if (((mouse_button_held == FALSE) && (terrains_match(current_terrain_drawn,ter_in_spot))) ||
				((mouse_button_held == TRUE) && (erasing_mode == TRUE))) {
				 	set_terrain(spot_hit,0);

					set_cursor(0);
					erasing_mode = TRUE;
					mouse_button_held = TRUE;
				} else {
					mouse_button_held = TRUE;
					set_cursor(0);
				 	set_terrain(spot_hit,current_terrain_drawn);
					erasing_mode = FALSE;
				}
				break;
			case 2: // set height
				mouse_button_held = FALSE;
				erasing_mode = FALSE;
				set_cursor(0);
				change_height(spot_hit,(option_hit != 0) ? 0 : 1);
				break;
			}
			break;


		case 1: // 1 - large paintbrush
				if (current_drawing_mode == 2)
					return;
				mouse_button_held = TRUE;
				change_circle_terrain(spot_hit,4,current_terrain_type,20);
				break;
		case 2:// 2 - small paintbrush
				if (current_drawing_mode == 2)
					return;
				mouse_button_held = TRUE;
				change_circle_terrain(spot_hit,1,current_terrain_type,20);
				break;
		case 3:// 3 - large spray can
				if (current_drawing_mode == 2)
					return;
				mouse_button_held = TRUE;
				shy_change_circle_terrain(spot_hit,4,current_terrain_type,1);
				break;
		case 4:// 4 - small spray can
				if (current_drawing_mode == 2)
					return;
				mouse_button_held = TRUE;
				shy_change_circle_terrain(spot_hit,2,current_terrain_type,1);
				break;		


		case 30: // 30 - place north entrance
		case 31: // 31 - place west entrance 
		case 32: // 32 - place south entrance 
		case 33: // 33 - place east entrance
				town.start_locs[overall_mode - 30].x = spot_hit.x; 
				town.start_locs[overall_mode - 30].y = spot_hit.y; 
				reset_drawing_mode(); 
				break;		

		case 40: // 40 - select instance
				// 7000 + x - creature x
				// 11000 + x - items x
				for (i = 0; i < 12000; i++) {
					item_to_try = (selected_item_number + i + 1) % 12000;

					// select creature
					if ((item_to_try >= 7000) && (item_to_try < 7000 + NUM_TOWN_PLACED_CREATURES) && (town.creatures[item_to_try % 1000].exists())) {
						if (same_point(spot_hit,town.creatures[item_to_try % 1000].start_loc)) {
							selected_item_number = item_to_try;
							play_sound(37);
							return;
							}
						}

					// select terrain script
					if ((item_to_try >= 9000) && (item_to_try < 9000 + NUM_TER_SCRIPTS) && (town.ter_scripts[item_to_try % 1000].exists)) {
						if (same_point(spot_hit,town.ter_scripts[item_to_try % 1000].loc)) {
							selected_item_number = item_to_try;
							play_sound(37);
							return;
							}
						}

					// select item
					if ((item_to_try >= 11000) && (item_to_try < 11000 + NUM_TOWN_PLACED_ITEMS) && (town.preset_items[item_to_try % 1000].exists())) {
						if (same_point(spot_hit,town.preset_items[item_to_try % 1000].item_loc)) {
							selected_item_number = item_to_try;
							play_sound(37);
							return;
							}
						}					
					}
				break;		
		case 41: // 41 - delete instance and return to mode 40: select instance.
				for (i = 0; i < 12000; i++) {

					item_to_try = (selected_item_number + i) % 12000;
					// select creature
					if ((item_to_try >= 7000) && (item_to_try < 7000 + NUM_TOWN_PLACED_CREATURES) && (town.creatures[item_to_try % 1000].exists())) {
						if (same_point(spot_hit,town.creatures[item_to_try % 1000].start_loc)) {
							town.creatures[item_to_try % 1000].number = -1;
							}
						}

					// select ter script
					if ((item_to_try >= 7000) && (item_to_try < 7000 + NUM_TER_SCRIPTS) && (town.ter_scripts[item_to_try % 1000].exists)) {
						if (same_point(spot_hit,town.ter_scripts[item_to_try % 1000].loc)) {
							town.ter_scripts[item_to_try % 1000].exists = FALSE;
							}
						}

					// select item
					if ((item_to_try >= 11000) && (item_to_try < 11000 + NUM_TOWN_PLACED_ITEMS) && (town.preset_items[item_to_try % 1000].exists())) {
						if (same_point(spot_hit,town.preset_items[item_to_try % 1000].item_loc)) {
							town.preset_items[item_to_try % 1000].which_item = -1;
							}
						}					
					}
				overall_mode = 40;
				set_cursor(7);	 
				break;		
	
		case 46: // 46 - placing creature
				create_new_creature(mode_count,spot_hit,NULL);
				if(current_drawing_mode==3){
					if(!object_sticky_draw){
						set_string("Select/edit placed object","Select object to edit");
						set_cursor(7);
						overall_mode = 40;
					}
				}
				else
					reset_drawing_mode(); 
				break;		
		case 47: // 47 - placing item
				create_new_item(mode_count,spot_hit,FALSE,NULL);
				if(current_drawing_mode==4){
					if(!object_sticky_draw){
						set_string("Select/edit placed object","Select object to edit");
						set_cursor(7);
						overall_mode = 40;
					}
				}
				else
					reset_drawing_mode(); 
				break;		
		case 48: // 48 - pasting instance
				paste_selected_instance(spot_hit);
				set_string("Select/edit placed object","Select object to edit");
				set_cursor(7);
				overall_mode = 40;
				//reset_drawing_mode(); 
				break;		
		case 49: // 49 - delete special enc
				if (editing_town) {
					for (i = 0; i < NUM_TOWN_PLACED_SPECIALS; i++)
						if ((town.spec_id[i] != kNO_TOWN_SPECIALS) && (loc_touches_rect(spot_hit,town.special_rects[i]))) {
							town.spec_id[i] = kNO_TOWN_SPECIALS;
							SetMacRect(&town.special_rects[i],-1,-1,-1,-1);
							break;
							}
					}
					else {
						for (i = 0; i < 30; i++)
							if ((current_terrain.spec_id[i] != kNO_OUT_SPECIALS) && (loc_touches_rect(spot_hit,current_terrain.special_rects[i]))) {
								current_terrain.spec_id[i] = kNO_OUT_SPECIALS;
								SetMacRect(&current_terrain.special_rects[i],-1,-1,-1,-1);
								break;
								}
						}
				reset_drawing_mode(); 
				break;		
		case 50: // 50 - set special enc
				if (editing_town) {
					for (i = 0; i < NUM_TOWN_PLACED_SPECIALS; i++)
						if ((town.spec_id[i] != kNO_TOWN_SPECIALS) && (loc_touches_rect(spot_hit,town.special_rects[i]))) {
							town.spec_id[i]  = (unsigned char)edit_special_num(/* 0,*/ town.spec_id[i]);
							break;
							}
					}
					else {
						for (i = 0; i < 30; i++)
							if ((current_terrain.spec_id[i] != kNO_OUT_SPECIALS) && (loc_touches_rect(spot_hit,current_terrain.special_rects[i]))) {
								current_terrain.spec_id[i]  = edit_special_num(/* 0,*/ current_terrain.spec_id[i]);
								break;
								}
						}
				reset_drawing_mode(); 
				break;		
				
		case 51: // 51 - delete instance and remain in delete mode
				set_cursor(7);
				for (i = 0; i < 12000; i++) {

					item_to_try = (selected_item_number + i) % 12000;
					// select creature
					if ((item_to_try >= 7000) && (item_to_try < 7000 + NUM_TOWN_PLACED_CREATURES) && (town.creatures[item_to_try % 1000].exists())) {
						if (same_point(spot_hit,town.creatures[item_to_try % 1000].start_loc)) {
							town.creatures[item_to_try % 1000].number = -1;
							}
						}

					// select ter script
					if ((item_to_try >= 7000) && (item_to_try < 7000 + NUM_TER_SCRIPTS) && (town.ter_scripts[item_to_try % 1000].exists)) {
						if (same_point(spot_hit,town.ter_scripts[item_to_try % 1000].loc)) {
							town.ter_scripts[item_to_try % 1000].exists = FALSE;
							}
						}

					// select item
					if ((item_to_try >= 11000) && (item_to_try < 11000 + NUM_TOWN_PLACED_ITEMS) && (town.preset_items[item_to_try % 1000].exists())) {
						if (same_point(spot_hit,town.preset_items[item_to_try % 1000].item_loc)) {
							town.preset_items[item_to_try % 1000].which_item = -1;
							}
						}
					}
				break;

		case 57: // 57 - place nav point
				create_navpoint(spot_hit); 
				reset_drawing_mode(); 
				break;
		case 58: // 58 - delete nav point
				delete_navpoint(spot_hit);
				reset_drawing_mode(); 
				break;		
		case 59: // 59 - edit sign
				if (editing_town == TRUE) {
					for (x = 0; x < 15; x++)
						if ((town.sign_locs[x].x == spot_hit.x) && (town.sign_locs[x].y == spot_hit.y)) {
							edit_sign(x);
							x = 30;
							}

					// no sign. make one?
					if ((x < 30) && (scen_data.scen_ter_types[t_d.terrain[spot_hit.x][spot_hit.y]].special == 39)) {
						for (x = 0; x < 15; x++)
							if (town.sign_locs[x].x < 0) {
								town.sign_locs[x].x = spot_hit.x;
								town.sign_locs[x].y = spot_hit.y;
								edit_sign(x);
								x = 30;
								}
						}
	
					if (x == 15) {
						give_error("Either this space is not a sign, or you have already placed too many signs on this level.",
							"",0);
						}	
					}
				if (editing_town == FALSE) {
					for (x = 0; x < 8; x++)
						if ((current_terrain.sign_locs[x].x == spot_hit.x) && (current_terrain.sign_locs[x].y == spot_hit.y)) {
							edit_sign(x);
							x = 30;
							}

					// no sign. make one?
					if ((x < 30) && 
					  (scen_data.scen_ter_types[current_terrain.terrain[spot_hit.x][spot_hit.y]].special == 39)) {
						for (x = 0; x < 8; x++)
							if (current_terrain.sign_locs[x].x < 0) {
								current_terrain.sign_locs[x].x = spot_hit.x;
								current_terrain.sign_locs[x].y = spot_hit.y;
								edit_sign(x);
								x = 30;
								}
						}
					if (x == 8) {
						give_error("Either this space is not a sign, or you have already placed too many signs on this level.",
							"",0);
						}	
					}
				reset_drawing_mode(); 
				break;		
		case 60: // 60 - wandering monster pts
				if (mouse_button_held == TRUE)
					break;
				if (editing_town == FALSE) {
					current_terrain.wandering_locs[mode_count - 1].x = spot_hit.x;
					current_terrain.wandering_locs[mode_count - 1].y = spot_hit.y;
					}
				if (editing_town == TRUE) {
					town.respawn_locs[mode_count - 1].x = spot_hit.x;
					town.respawn_locs[mode_count - 1].y = spot_hit.y;
					}
				mode_count--;
				if (editing_town) {
					switch (mode_count) {	
						case 5: set_string("Place 2nd spawn point","");break;
						case 4: set_string("Place 3rd spawn point","");break;
						case 3: set_string("Place 4th spawn point","");break;
						case 2: set_string("Place 5th spawn point","");break;
						case 1: set_string("Place 6th spawn point","");break;
						case 0:
							reset_drawing_mode(); 
							break;
						}
 					}
 					else {
	 					switch (mode_count) {	
							case 3: set_string("Place 2nd spawn point","");break;
							case 2: set_string("Place 3rd spawn point","");break;
							case 1: set_string("Place 4th spawn point","");break;
							case 0:
								reset_drawing_mode(); 
								break;
							}
						}
				break;		
		case 61: // 61 - blocked spot
				make_blocked(spot_hit.x,spot_hit.y);
				if(!object_sticky_draw)
					reset_drawing_mode(); 
				break;		
		case 62: // 62-66 - barrels, atc
				make_web(spot_hit.x,spot_hit.y); 
				if(!object_sticky_draw)
					reset_drawing_mode(); 
				break;		
		case 63: // 62-66 - barrels, atc
				make_crate(spot_hit.x,spot_hit.y);
				if(!object_sticky_draw)
					reset_drawing_mode(); 
				break;		
		case 64: // 62-66 - barrels, atc
				make_barrel(spot_hit.x,spot_hit.y);
				if(!object_sticky_draw)
					reset_drawing_mode(); 
				break;		
		case 65: // 62-66 - barrels, atc
				make_fire_barrier(spot_hit.x,spot_hit.y);
				if(!object_sticky_draw)
					reset_drawing_mode(); 
				break;		
		case 66: // 62-66 - barrels, atc
				make_force_barrier(spot_hit.x,spot_hit.y);
				if(!object_sticky_draw)
					reset_drawing_mode(); 
				break;		
		case 67: // 67 - clean space
				take_blocked(spot_hit.x,spot_hit.y); 
				take_force_barrier(spot_hit.x,spot_hit.y); 
				take_fire_barrier(spot_hit.x,spot_hit.y); 
				take_barrel(spot_hit.x,spot_hit.y); 
				take_crate(spot_hit.x,spot_hit.y); 
				take_web(spot_hit.x,spot_hit.y); 
				for (i = 0; i < 8; i++)
					take_sfx(spot_hit.x,spot_hit.y,i);
				if(!object_sticky_draw)
					reset_drawing_mode(); 
				break;		
		case 68: // 68 - place different floor stains
				make_sfx(spot_hit.x,spot_hit.y,mode_count);
				if(!object_sticky_draw)
					reset_drawing_mode(); 
				break;		

		case 69: //edit town entrance
				edit_town_entry(spot_hit);
				if(!object_sticky_draw)
					reset_drawing_mode(); 
				break;		
			
		case 70:
				create_new_ter_script("specobj",spot_hit,NULL);
				if(!object_sticky_draw)
					reset_drawing_mode(); 				
				break;
				
		case 71: //set outdoor start pt
				scenario.what_outdoor_section_start_in = cur_out;
				scenario.start_where_in_outdoor_section = spot_hit;
				if(!object_sticky_draw)
					reset_drawing_mode(); 
				break;		

		case 72: //set town start pt
				scenario.start_in_what_town = cur_town;
				scenario.what_start_loc_in_town = spot_hit;
				if(!object_sticky_draw)
					reset_drawing_mode(); 
				break;		
		case 73:
				set_cursor(1);		
				if (current_drawing_mode == 0) {
				set_string("Click on a square to select the","type of floor it contains.");
				current_floor_drawn = floor_in_spot;
				}
				if (current_drawing_mode >= 1) {
				current_terrain_drawn = ter_in_spot;
				set_string("Click on a square to select the","type of terrain it contains.");
				}
				set_cursor(0);
				overall_mode = 0;
				break;

		}
		
	// rectangle commands
	if ((overall_mode >= 10) && (overall_mode < 30)) {
		if (mode_count == 2) {
			working_rect.left = spot_hit.x;
		 	working_rect.top = spot_hit.y;
			mode_count = 1;
			set_cursor(6);
			set_string("Now select lower right corner","");
			return;
			}		
		working_rect.right = spot_hit.x;
	 	working_rect.bottom = spot_hit.y;	
	
	 	if ((working_rect.right < working_rect.left) || (working_rect.bottom < working_rect.top)) {
			overall_mode = 0;
			set_cursor(0);	 	
	 		return;
	 		}

		switch (overall_mode) {
			case 10: // 10 - frame rectangle
				change_rect_terrain(working_rect,current_terrain_type,20,0);
				break;
			case 11: // 11 - fill rectangle
				change_rect_terrain(working_rect,current_terrain_type,20,1);				
				break;
			case 16: // 16 - place special enc
				if (editing_town == TRUE) {
					for (i = 0; i < NUM_TOWN_PLACED_SPECIALS; i++) {
						if (town.spec_id[i] == kNO_TOWN_SPECIALS) {
							town.spec_id[i] = (unsigned char)edit_special_num(i+10);
							town.special_rects[i].top = (short)working_rect.top;
							town.special_rects[i].left = (short)working_rect.left;
							town.special_rects[i].bottom =(short)working_rect.bottom;
							town.special_rects[i].right = (short)working_rect.right;
							i = 500;
							}
						}
						
						if (i < 500) {
							give_error("Each town can have at most 60 locations with special encounters. You'll need to erase some special encounters before you can place more.","",0);
							return;			
							}
					}

				if (editing_town == FALSE) {
					for (i = 0; i < 30; i++)
						if (current_terrain.spec_id[i] == kNO_OUT_SPECIALS) {
							current_terrain.spec_id[i] = edit_special_num(/* 0,*/ i+10);
							current_terrain.special_rects[i].top = (short)working_rect.top;
							current_terrain.special_rects[i].left = (short)working_rect.left;
							current_terrain.special_rects[i].bottom = (short)working_rect.bottom;
							current_terrain.special_rects[i].right = (short)working_rect.right;
							i = 500;
							}

						if (i < 500) {
							give_error("Each section can have at most 30 locations with special encounters. You'll need to erase some special encounters before you can place more.","",0);
							return;			
							}

						}
				break;
			case 17: // 17 - town boundaries
				town.in_town_rect.top = (short)working_rect.top;
				town.in_town_rect.left = (short)working_rect.left;
				town.in_town_rect.bottom = (short)working_rect.bottom;
				town.in_town_rect.right = (short)working_rect.right;
				break;
			case 18: // 18 - swap wall types
				transform_walls(working_rect);
				break;
			case 19: // 19 - add walls
				place_bounding_walls(working_rect);
				break;
			case 20: // 20 - set height rectangle
				set_rect_height(working_rect);
				break;
			case 24: // 24 - add height rectangle
				add_rect_height(working_rect);
				break;
			case 21: // 21 - place text rectangle
				x = 0;
				if (editing_town == TRUE) {
					for (x = 0; x < 16; x++)
						if (town.room_rect[x].right <= 0) {
							town.room_rect[x].top = (short)working_rect.top;
							town.room_rect[x].left = (short)working_rect.left;
							town.room_rect[x].bottom = (short)working_rect.bottom;
							town.room_rect[x].right = (short)working_rect.right;
							sprintf(town.info_rect_text[x],"");
							if (edit_area_rect_str(x /*,1 */) == FALSE)
								town.room_rect[x].right = 0;
							x = 500;
						}
				}
				if (editing_town == FALSE) {
					for (x = 0; x < 8; x++)
						if (current_terrain.info_rect[x].right <= 0) {
							current_terrain.info_rect[x].top = (short)working_rect.top;
							current_terrain.info_rect[x].left = (short)working_rect.left;
							current_terrain.info_rect[x].bottom = (short)working_rect.bottom;
							current_terrain.info_rect[x].right = (short)working_rect.right;
							sprintf(current_terrain.info_rect_text[x],"");
							if (edit_area_rect_str(x /*,0 */) == FALSE)
								current_terrain.info_rect[x].right = 0;
							x = 500;
						}
				}
				if (x < 500)
					give_error("You have placed the maximum number of area rectangles (16 in town, 8 outdoors).","",0);
				break;
			case 22: // 22 - outdoor only - town entrance
				create_town_entry(working_rect);
				break;


			}
		reset_drawing_mode(); 	
		}
		


}

void reset_drawing_mode()
{
	if (current_drawing_mode == 0) {
		set_new_floor(current_floor_drawn);
		}
		else {
			set_new_terrain(current_terrain_drawn);
			}
	overall_mode = 0;
	set_cursor(0);
}

/*
void play_press_snd()
{
	play_sound(37);
}
*/


void swap_terrain()
{
	short a,b,c,i,j;
	short ter;
	
	change_ter(&a,&b,&c);
	if (a < 0) return;
	
	for (i = 0; i < ((editing_town == TRUE) ? max_dim[town_type] : 48); i++){
		for (j = 0; j < ((editing_town == TRUE) ? max_dim[town_type] : 48); j++) {
			if (current_drawing_mode > 0) {
				ter = (editing_town == TRUE) ? t_d.terrain[i][j] : current_terrain.terrain[i][j];
				if ((ter == a) && (get_ran(1,1,100) <= c)) {
					if (editing_town == TRUE)
						t_d.terrain[i][j] = b;
					else
						current_terrain.terrain[i][j] = b;
					appendChangeToLatestStep(new drawingChange(i,j,b,a,2));
				}
			}
			else{
				ter = (editing_town == TRUE) ? t_d.floor[i][j] : current_terrain.floor[i][j];
				if ((ter == a) && (get_ran(1,1,100) <= c)) {
					if (editing_town == TRUE)
						t_d.floor[i][j] = (unsigned char)b;
					else
						current_terrain.floor[i][j] = (unsigned char)b;
					appendChangeToLatestStep(new drawingChange(i,j,b,a,1));
				}
			}
		}
	}
}

void set_new_terrain(short selected_terrain)
{
	if (scen_data.scen_ter_types[selected_terrain].ed_pic.not_legit() == TRUE)
		return;
	current_terrain_drawn = selected_terrain;

	char str1[256],str2[256];
	sprintf(str1,"Drawing terrain number %d:", (int)current_terrain_drawn);
	sprintf(str2,"  %s",scen_data.scen_ter_types[current_terrain_drawn].ter_name);
	set_string(str1,str2);

	place_right_buttons(/* 0 */);
}

// check if floor is valid, and, if so, make that floor current floor drawn
void set_new_floor(short selected_terrain)
{
	if (scen_data.scen_floors[selected_terrain].ed_pic.not_legit() == TRUE)
		return;
	current_floor_drawn = selected_terrain;

	char str1[256],str2[256];
	sprintf(str1,"Drawing floor number %d:", (int)current_floor_drawn);
	sprintf(str2,"  %s",scen_data.scen_floors[current_floor_drawn].floor_name);
	set_string(str1,str2);

	place_right_buttons(/* 0 */);
}

Boolean control_key_down()
{
	// is control key down?
	Boolean control_down = FALSE;
	BYTE cBuf[256];
	GetKeyboardState(cBuf);
	if (cBuf[VK_CONTROL] & 0x80)
		control_down = TRUE;
	return control_down;
	
}


// "Outdoor: drawing mode failure after moving section" fix
// As the scroll handler of edit screen is revised,
// key scroll process is also needed to rewritten.

Boolean handle_syskeystroke(WPARAM wParam,LPARAM /* lParam */,short *handled)
{
	short nonsensical_variabl = 0;
	char *str;
	char get_text[280];
	char get_text2[280];
	Boolean need_redraw = TRUE;
// q_3DModStart
	const int kNumKeyAssign = 10;	// number of key assignment

	// virtual key code of ten key
	const WPARAM num_key[ kNumKeyAssign ] =
		{	VK_NUMPAD0,VK_NUMPAD1, VK_NUMPAD2, VK_NUMPAD3, VK_NUMPAD4,	// num pad key 0-9
			VK_NUMPAD5, VK_NUMPAD6, VK_NUMPAD7, VK_NUMPAD8, VK_NUMPAD9
		};

	// num pad key scroller assignment
	const int scrl_key_2D[ kNumKeyAssign ] = {
				eSCRL_NoScrl,			// 0 num pad
		eSCRL_Left	|	eSCRL_Bottom,	// 1
						eSCRL_Bottom,	// 2
		eSCRL_Right |	eSCRL_Bottom,	// 3
		eSCRL_Left,						// 4
				eSCRL_NoScrl,			// 5
		eSCRL_Right,					// 6
		eSCRL_Left	|	eSCRL_Top,		// 7
						eSCRL_Top,		// 8
		eSCRL_Right |	eSCRL_Top,		// 9
	};

	const int scrl_key_3D[ kNumKeyAssign ] = {
				eSCRL_NoScrl,			// 0 num pad
						eSCRL_Bottom,	// 1
		eSCRL_Right |	eSCRL_Bottom,	// 2
		eSCRL_Right,					// 3
		eSCRL_Left |	eSCRL_Bottom,	// 4
				eSCRL_NoScrl,			// 5
		eSCRL_Right	|	eSCRL_Top,		// 6
		eSCRL_Left,						// 7
		eSCRL_Left |	eSCRL_Top,		// 8
						eSCRL_Top		// 9
	};

	const POINT kCenterPoint = {
	TER_RECT_UL_Y + 15 + 4 * 48,
	TER_RECT_UL_X + 15 + 4 * 48
	};

	// num pad key handling

	*handled = 0;
	bool ctrl_key = (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0;	// check MSB for current key state
	bool alt_key = (GetAsyncKeyState( VK_MENU ) & 0x8000) != 0;
	bool shft_key = (GetAsyncKeyState(0xC0) & 0x8000) != 0;

	for ( int i = 0; i < kNumKeyAssign; i++) {
		if ( wParam == num_key[i] ) {
			kill_next_win_char = TRUE;
			if ( i == 5 ) {		// "5" key
				if ((cur_viewing_mode == 0) || (cur_viewing_mode == 2)) {
					handle_action( kCenterPoint, ((ctrl_key) ? MK_CONTROL : 0),-1 );
					mouse_button_held = FALSE;
					return FALSE;
				}
				break;
			}
			
			else {						// other ten keys
				int map_size = (editing_town) ? max_dim[town_type] : 48;
				int scrl = eSCRL_NoScrl;
				if ((cur_viewing_mode == 0) || (cur_viewing_mode == 2))
					scrl = scrl_key_2D[ i ];
				if (cur_viewing_mode == 10 || cur_viewing_mode == 11)
					scrl = scrl_key_3D[ i ];
				if ( scrl != eSCRL_NoScrl ) {
					handle_scroll( map_size, scrl, ctrl_key, shft_key );
					return FALSE;
				}
				break;
			}
		}
	}
	

	if (((wParam == VK_LEFT) || (wParam == VK_DOWN) || (wParam == VK_UP) ||
	(wParam == VK_RIGHT)) && (cur_viewing_mode == 0 || cur_viewing_mode == 10
	|| cur_viewing_mode == 11)) {
		if ((wParam == VK_UP) && (selected_item_number >= 0))
			shift_selected_instance(0, -1);
		if ((wParam == VK_RIGHT) && (selected_item_number >= 0))
			shift_selected_instance(1,0);
		if ((wParam == VK_LEFT) && (selected_item_number >= 0))
			shift_selected_instance(-1,0);
		if ((wParam == VK_DOWN) && (selected_item_number >= 0))
			shift_selected_instance(0, 1);
		draw_terrain();
		return FALSE;
	}

	if ((wParam == VK_BACK) && (editing_town == TRUE) && (cur_viewing_mode == 0 || cur_viewing_mode == 10 || cur_viewing_mode == 11)) {
	if (selected_item_number > -1) {
	delete_selected_instance();
	set_string("Selected Instance Deleted","");
	}
	}

	if ((wParam == VK_DELETE) && (editing_town == TRUE) && (cur_viewing_mode == 0 || cur_viewing_mode == 10 || cur_viewing_mode == 11)) {
		set_string("Delete an object","Select object");
		set_cursor(7);
		overall_mode = 41;		
	}
	if ((wParam == VK_HOME) && (editing_town == TRUE) && (cur_viewing_mode == 0 || cur_viewing_mode == 10 || cur_viewing_mode == 11)) {
		set_string("Select/edit placed object","Select object to edit");
		set_cursor(7);
		overall_mode = 40;
	}

	if ((wParam == VK_END) && (editing_town == TRUE) && (cur_viewing_mode == 0 || cur_viewing_mode == 10 || cur_viewing_mode == 11)) {
		set_string("Delete one object after another","Select next object to delete");
		set_cursor(7);
		overall_mode = 51;
	}
	
	if (wParam == VK_SUBTRACT) { // Zoom out
							if(cur_viewing_mode == 0)
								cur_viewing_mode = 2;
							else if(cur_viewing_mode == 2)
								cur_viewing_mode = 1;
							else if(cur_viewing_mode == 1)
								cur_viewing_mode = 0;

							set_up_terrain_buttons();
							reset_small_drawn();
							redraw_screen();
			}

	if (wParam == VK_ADD) { // Zoom in
							if(cur_viewing_mode == 1)
								cur_viewing_mode = 2;
							else if(cur_viewing_mode == 2)
								cur_viewing_mode = 0;
							else if(cur_viewing_mode == 0)
								cur_viewing_mode = 1;

							set_up_terrain_buttons();
							reset_small_drawn();
							redraw_screen();
			}
			
	if (wParam == 0x30) {
					 if (hintbook_mode0 > 1)
					    hintbook_mode0 = 0;
		 	 hintbook_mode0 = 1 - hintbook_mode0;
				small_any_drawn = FALSE;
				draw_terrain();
				}

	if (wParam == 0x31) {
	     hintbook_mode1 = 1;
			 hintbook_mode2 = 0;
		 	 hintbook_mode3 = 0;
		 	 hintbook_mode4 = 0;
		 	 hintbook_mode5 = 0;
		 	 hintbook_mode6 = 0;
		 	 hintbook_mode7 = 0;
		 		small_any_drawn = FALSE;
				draw_terrain();
				}

	if (wParam == 0x32) {
	     hintbook_mode1 = 0;
			 hintbook_mode2 = 1;
		 	 hintbook_mode3 = 1;
		 	 hintbook_mode4 = 1;
		 	 hintbook_mode5 = 1;
		 	 hintbook_mode6 = 0;
		 	 hintbook_mode7 = 0;
				small_any_drawn = FALSE;
				draw_terrain();
				}
				
	if ((wParam == 0x33) && (editing_town == TRUE)) {
			 if (hintbook_mode3 == 0) {
	     hintbook_mode1 = 0;
			 hintbook_mode2 = 0;
		 	 hintbook_mode3 = 1;
			 }
			 else if (hintbook_mode3 == 1) {
		 	 hintbook_mode3 = 0;
			 }
				small_any_drawn = FALSE;
				draw_terrain();
				}
				
	if ((wParam == 0x34) && (editing_town == TRUE)) {
			 if (hintbook_mode4 == 0) {
	     hintbook_mode1 = 0;
			 hintbook_mode2 = 0;
		 	 hintbook_mode4 = 1;
			 }
			 else if (hintbook_mode4 == 1) {
		 	 hintbook_mode4 = 0;
			 }
				small_any_drawn = FALSE;
				draw_terrain();
				}
	if ((wParam == 0x35) && (editing_town == TRUE)) {
			 if (hintbook_mode5 == 0) {
	     hintbook_mode1 = 0;
			 hintbook_mode2 = 0;
		 	 hintbook_mode5 = 1;
			 }
			 else if (hintbook_mode5 == 1) {
		 	 hintbook_mode5 = 0;
			 }
				small_any_drawn = FALSE;
				draw_terrain();
				}
	if (wParam == 0x36) {
			 if (hintbook_mode6 == 0) {
	     hintbook_mode1 = 0;
			 hintbook_mode2 = 0;
		 	 hintbook_mode6 = 1;
			 }
			 else if (hintbook_mode6 == 1) {
		 	 hintbook_mode6 = 0;
			 }
				small_any_drawn = FALSE;
				draw_terrain();
				}
	if (wParam == 0x37) {
			 if (hintbook_mode7 == 0) {
	     hintbook_mode1 = 0;
			 hintbook_mode2 = 0;
		 	 hintbook_mode7 = 1;
			 }
			 else if (hintbook_mode7 == 1) {
		 	 hintbook_mode7 = 0;
			 }
				small_any_drawn = FALSE;
				draw_terrain();
				}
	if (wParam == 0x38) {
		 	 hintbook_mode8 = 1 - hintbook_mode8;
				small_any_drawn = FALSE;
				draw_terrain();
				}

if (wParam == 0x39) {
		 	 hintbook_mode9 = 1 - hintbook_mode9;
				small_any_drawn = FALSE;
				draw_terrain();
				}

	if (wParam == VK_F3) {

								if (current_drawing_mode == 0) {
								   set_drawing_mode(2);
									 redraw_screen();
									 }
								else {
								set_drawing_mode(current_drawing_mode - 1);
								redraw_screen();
							}
	}

	if (wParam == VK_F4) {
							set_drawing_mode((current_drawing_mode + 1) % 3);
								redraw_screen();
	}

  if (wParam == VK_F5) {
		if (current_drawing_mode == 0) {
		sprintf(get_text,"Drawing Floor number %d",current_floor_drawn);
	 	sprintf(get_text2,"%s",scen_data.scen_floors[current_floor_drawn].floor_name);
		set_string(get_text,get_text2);
		if (current_floor_drawn > 11)
		current_floor_drawn = current_floor_drawn - 12;
		else
		current_floor_drawn = current_floor_drawn + 244;
		}
		if (current_drawing_mode > 0) {
		sprintf(get_text,"Drawing Terrain number %d",current_terrain_drawn);
	 	sprintf(get_text2,"%s",scen_data.scen_ter_types[current_terrain_drawn].ter_name);
		set_string(get_text,get_text2);
		if (current_terrain_drawn > 11)
		current_terrain_drawn = current_terrain_drawn - 12;
		else
		current_terrain_drawn = current_terrain_drawn + 500;
		}
	}

		if (wParam == VK_F6) {
		if (current_drawing_mode == 0) {
		sprintf(get_text,"Drawing Floor number %d",current_floor_drawn);
	 	sprintf(get_text2,"%s",scen_data.scen_floors[current_floor_drawn].floor_name);
		set_string(get_text,get_text2);
		if (current_floor_drawn < 255)
		current_floor_drawn = current_floor_drawn + 1;
		else
		current_floor_drawn = 0;
		}
		if (current_drawing_mode > 0) {
		sprintf(get_text,"Drawing Terrain number %d",current_terrain_drawn);
	 	sprintf(get_text2,"%s",scen_data.scen_ter_types[current_terrain_drawn].ter_name);
		set_string(get_text,get_text2);
		if (current_terrain_drawn < 511)
		current_terrain_drawn = current_terrain_drawn + 1;
		else
		current_terrain_drawn = 0;
		}
	}
		if (wParam == VK_F7) {
		if (current_drawing_mode == 0) {
		sprintf(get_text,"Drawing Floor number %d",current_floor_drawn);
	 	sprintf(get_text2,"%s",scen_data.scen_floors[current_floor_drawn].floor_name);
		set_string(get_text,get_text2);
		if (current_floor_drawn > 0)
		current_floor_drawn = current_floor_drawn - 1;
		else
		current_floor_drawn = 255;
		}
		if (current_drawing_mode > 0) {
		sprintf(get_text,"Drawing Terrain number %d",current_terrain_drawn);
	 	sprintf(get_text2,"%s",scen_data.scen_ter_types[current_terrain_drawn].ter_name);
		set_string(get_text,get_text2);
		if (current_terrain_drawn > 0)
		current_terrain_drawn = current_terrain_drawn - 1;
		else
		current_terrain_drawn = 511;
		}
	}
		if (wParam == VK_F8) {
		if (current_drawing_mode == 0) {
		sprintf(get_text,"Drawing Floor number %d",current_floor_drawn);
	 	sprintf(get_text2,"%s",scen_data.scen_floors[current_floor_drawn].floor_name);
		set_string(get_text,get_text2);
		if (current_floor_drawn < 244)
		current_floor_drawn = current_floor_drawn + 12;
		else
		current_floor_drawn = current_floor_drawn - 244;
		}
		if (current_drawing_mode > 0) {
		sprintf(get_text,"Drawing Terrain number %d",current_terrain_drawn);
	 	sprintf(get_text2,"%s",scen_data.scen_ter_types[current_terrain_drawn].ter_name);
		set_string(get_text,get_text2);
		if (current_terrain_drawn < 500)
		current_terrain_drawn = current_terrain_drawn + 12;
		else
		current_terrain_drawn = current_terrain_drawn - 500;
		}
	}

	return FALSE;
// q_3DModEnd
}

Boolean handle_keystroke(WPARAM wParam, LPARAM /* lParam */)
{
	POINT pass_point;
	short i,j,store_ter;
	char chr;
	chr = (char) wParam;
	store_ter = current_terrain_type;
	Boolean need_redraw = FALSE, option_hit = FALSE, right_click = FALSE;
	extern short selected_item_number;
	bool ctrl_key = (GetKeyState( VK_CONTROL ) & 0x8000) != 0;	// check MSB for current key state

	switch(chr)
		{
	case 'A': // Pencil
	pass_point.x = RIGHT_BUTTONS_X_SHIFT + 6 + palette_buttons[0][0].left;
	pass_point.y = 6 + palette_buttons[0][0].top;
	    handle_action(pass_point,wParam,-1);
	break;
	case 'B': // Paintbrush (Large)
	pass_point.x = RIGHT_BUTTONS_X_SHIFT + 6 + palette_buttons[1][0].left;
	pass_point.y = 6 + palette_buttons[1][0].top;
	    handle_action(pass_point,wParam,-1);
	break;
	case 'C': // Paintbrush (Small)
	pass_point.x = RIGHT_BUTTONS_X_SHIFT + 6 + palette_buttons[2][0].left;
	pass_point.y = 6 + palette_buttons[2][0].top;
	    handle_action(pass_point,wParam,-1);
	break;
	case 'D': // Spraycan (Large)
	pass_point.x = RIGHT_BUTTONS_X_SHIFT + 6 + palette_buttons[3][0].left;
	pass_point.y = 6 + palette_buttons[3][0].top;
	    handle_action(pass_point,wParam,-1);
	break;
	case 'E': // Spraycan (Small)
	pass_point.x = RIGHT_BUTTONS_X_SHIFT + 6 + palette_buttons[4][0].left;
	pass_point.y = 6 + palette_buttons[4][0].top;
	    handle_action(pass_point,wParam,-1);
	break;
	case 'F': // Change Height
	pass_point.x = RIGHT_BUTTONS_X_SHIFT + 6 + palette_buttons[5][0].left;
	pass_point.y = 6 + palette_buttons[5][0].top;
	    handle_action(pass_point,wParam,-1);
	break;
	case 'G': // Paint Rectangle (Hollow)
	pass_point.x = RIGHT_BUTTONS_X_SHIFT + 6 + palette_buttons[6][0].left;
	pass_point.y = 6 + palette_buttons[6][0].top;
	    handle_action(pass_point,wParam,-1);
	break;
	case 'H': // Paint Rectangle (Full)
	pass_point.x = RIGHT_BUTTONS_X_SHIFT + 6 + palette_buttons[7][0].left;
	pass_point.y = 6 + palette_buttons[7][0].top;
	    handle_action(pass_point,wParam,-1);
	break;
	case 'I': case '[': // Zoom Out/Zoom In
	pass_point.x = RIGHT_BUTTONS_X_SHIFT + 6 + palette_buttons[0][1].left;
	pass_point.y = 6 + palette_buttons[0][1].top;
	    handle_action(pass_point,wParam,-1);
	break;
	case 'J': // Change Drawing Mode
	pass_point.x = RIGHT_BUTTONS_X_SHIFT + 6 + palette_buttons[1][1].left;
	pass_point.y = 6 + palette_buttons[1][1].top;
	    handle_action(pass_point,wParam,-1);
	break;
	case 'K': // Automatic Hills
	pass_point.x = RIGHT_BUTTONS_X_SHIFT + 6 + palette_buttons[2][1].left;
	pass_point.y = 6 + palette_buttons[2][1].top;
	    handle_action(pass_point,wParam,-1);
	break;
	case 'L': // Edit Sign
	pass_point.x = RIGHT_BUTTONS_X_SHIFT + 6 + palette_buttons[3][1].left;
	pass_point.y = 6 + palette_buttons[3][1].top;
	    handle_action(pass_point,wParam,-1);
	break;
	case 'M': // Create Room Rectangle
	pass_point.x = RIGHT_BUTTONS_X_SHIFT + 6 + palette_buttons[4][1].left;
	pass_point.y = 6 + palette_buttons[4][1].top;
	    handle_action(pass_point,wParam,-1);
	break;
	case 'N': // Place Spawn Point
	pass_point.x = RIGHT_BUTTONS_X_SHIFT + 6 + palette_buttons[5][1].left;
	pass_point.y = 6 + palette_buttons[5][1].top;
	    handle_action(pass_point,wParam,-1);
	break;
	case 'O': // Create Town Entrance
	pass_point.x = RIGHT_BUTTONS_X_SHIFT + 6 + palette_buttons[6][1].left;
	pass_point.y = 6 + palette_buttons[6][1].top;
	    handle_action(pass_point,wParam,-1);
	break;
	case 'P': // Edit Town Entrance
	pass_point.x = RIGHT_BUTTONS_X_SHIFT + 6 + palette_buttons[7][1].left;
	pass_point.y = 6 + palette_buttons[7][1].top;
	    handle_action(pass_point,wParam,-1);
	break;

	case 'Q': // Place Walls
	pass_point.x = RIGHT_BUTTONS_X_SHIFT + 6 + palette_buttons[0][2].left;
	pass_point.y = 6 + palette_buttons[0][2].top;
	    handle_action(pass_point,wParam,-1);
	break;
	case 'R': // Switch Wall Types
	pass_point.x = RIGHT_BUTTONS_X_SHIFT + 6 + palette_buttons[1][2].left;
	pass_point.y = 6 + palette_buttons[1][2].top;
	    handle_action(pass_point,wParam,-1);
	break;
	case 'S': // Change Terrain Randomly
	pass_point.x = RIGHT_BUTTONS_X_SHIFT + 6 + palette_buttons[2][2].left;
	pass_point.y = 6 + palette_buttons[2][2].top;
	    handle_action(pass_point,wParam,-1);
	break;
	case 'T': // Eyedropper
	pass_point.x = RIGHT_BUTTONS_X_SHIFT + 6 + palette_buttons[3][2].left;
	pass_point.y = 6 + palette_buttons[3][2].top;
	    handle_action(pass_point,wParam,-1);
	break;
	case 'U': case ']': // 2D/3D
	pass_point.x = RIGHT_BUTTONS_X_SHIFT + 6 + palette_buttons[4][2].left;
	pass_point.y = 6 + palette_buttons[4][2].top;
	    handle_action(pass_point,wParam,-1);
	break;
	case 'V': // Create Special Encounter
	pass_point.x = RIGHT_BUTTONS_X_SHIFT + 6 + palette_buttons[5][2].left;
	pass_point.y = 6 + palette_buttons[5][2].top;
	    handle_action(pass_point,wParam,-1);
	break;
	case 'W': // Delete Special Encounter
	pass_point.x = RIGHT_BUTTONS_X_SHIFT + 6 + palette_buttons[6][2].left;
	pass_point.y = 6 + palette_buttons[6][2].top;
	    handle_action(pass_point,wParam,-1);
	break;
	case 'X': // Edit Special Encounter
	pass_point.x = RIGHT_BUTTONS_X_SHIFT + 6 + palette_buttons[7][2].left;
	pass_point.y = 6 + palette_buttons[7][2].top;
	    handle_action(pass_point,wParam,-1);
	break;
	case 'Y': // Select/Edit Object
	pass_point.x = RIGHT_BUTTONS_X_SHIFT + 6 + palette_buttons[0][3].left;
	pass_point.y = 6 + palette_buttons[0][3].top;
	    handle_action(pass_point,wParam,-1);
	break;
	case 'Z': // Place Terrain Script
	pass_point.x = RIGHT_BUTTONS_X_SHIFT + 6 + palette_buttons[7][4].left;
	pass_point.y = 6 + palette_buttons[7][4].top;
	    handle_action(pass_point,wParam,-1);
	break;

	case '\\': // toggle gridline mode
				if (grid_mode == 2)
				 grid_mode = 0;
				 else  grid_mode =  grid_mode + 1;
				small_any_drawn = FALSE;
				draw_terrain();
	break;
/*
	case '-': // Delete selected instance
	delete_selected_instance();
	set_string("Selected Instance Deleted","");
	break;
*/
	case '=': // Clear selected instance
	set_string("Drawing mode","  ");
	selected_item_number = -1;
	set_cursor(0);
	overall_mode = 0;
	break;

	case ';': // Clear Space
	pass_point.x = RIGHT_BUTTONS_X_SHIFT + 6 + palette_buttons[6][4].left;
	pass_point.y = 6 + palette_buttons[6][4].top;
	handle_action(pass_point,wParam,-1);
	break;

	case ',': // Cut selected instance
			set_string("Instance copied and deleted","  ");
				 cut_selected_instance();
			if(editing_town)
				change_made_town = TRUE;
			else
				change_made_outdoors = TRUE;
	break;
	case '.': // Copy selected instance
				set_string("Instance copied","  ");
			copy_selected_instance();
			if(editing_town)
				change_made_town = TRUE;
			else
				change_made_outdoors = TRUE;
	break;
	case '/': // Paste selected instance
			if ((copied_creature.exists() == FALSE) && (copied_item.exists() == FALSE) && (copied_ter_script.exists == FALSE)) {
				beep();
				break;
				}
			set_string("Paste copied instance","Select location to place");
			set_cursor(7);
			overall_mode = 48;
	break;

	case ' ':
	pass_point.x = RIGHT_BUTTONS_X_SHIFT + 6 + palette_buttons[1][1].left;
	pass_point.y = 6 + palette_buttons[1][1].top;
	handle_action(pass_point,wParam,-1);
	break;

// q_3DModStart
	case VK_TAB:	// '\t':
//				if (option_hit != 0) {
				if ( ctrl_key ) {					// toggle Realistic mode
					pass_point.x = RIGHT_BUTTONS_X_SHIFT + 6 + palette_buttons[0][1].left;
					pass_point.y = 6 + palette_buttons[0][1].top;
				}
				else {								// swap 2D/3D mode
					pass_point.x = RIGHT_BUTTONS_X_SHIFT + 6 + palette_buttons[4][2].left;
					pass_point.y = 6 + palette_buttons[4][2].top;
				}

				handle_action(pass_point,wParam,-1);
	break;
// q_3DModEnd

					
			default:
				Boolean ctrl = control_key_down();
				
				// Ctrl + D = outdoor/town details
				if ((chr == 4) && (ctrl)) {
					if (editing_town)
						handle_town_menu(2);
						else handle_outdoor_menu(2);
					break;
				}

				// Ctrl + L = load new zone
				if ((chr == 12) && (ctrl)) {
					if (editing_town)
						handle_town_menu(1);
						else handle_outdoor_menu(1);
					break;
				}
		
				if ((chr >= 97) && (chr <= 122)) {
					if (current_drawing_mode == 0) { // use shortcut keys, if editing floor
						for (i = 0; i < 256; i++) {
							j = current_floor_drawn + i + 1;
							j = j % 256;
							if ((scen_data.scen_floors[j].shortcut_key < 26) && (scen_data.scen_floors[j].shortcut_key == chr - 97)) {
								set_new_floor(j);
								return FALSE;	
								}
							}

						}
					if (current_drawing_mode > 0) { // use shortcut keys, if editing terrain
						for (i = 0; i < 512; i++) {
							j = current_terrain_type + i + 1;
							j = j % 512;
							if ((scen_data.scen_ter_types[j].shortcut_key < 26) && (scen_data.scen_ter_types[j].shortcut_key == chr - 97)) {
								set_new_terrain(j);
								return FALSE;	
								}
							}

						}
					}

				break;

			}
	
	draw_terrain();
	mouse_button_held = FALSE;
	
	return FALSE;	
}

/*
Boolean is_hill(short i,short j)
{

	Boolean answer = FALSE;
	short ter;
	
	ter = (editing_town == TRUE) ? t_d.terrain[i][j] : current_terrain.terrain[i][j];
	if ((editing_town == TRUE) && ((i < 0) || (i > max_dim[town_type] - 1) || (j < 0) || (j > max_dim[town_type] - 1)))
		return TRUE;
	if ((editing_town == FALSE) && ((i < 0) || (i > 47) || (j < 0) || (j > 47)))
		return TRUE;
	if ((ter >= 74) && (ter <= 121)) 
		answer = TRUE;	
	if (ter == 255)
		answer = TRUE;	
	return answer;		

}
*/

// Returns true if rocky cave floor or desert
Boolean is_rocks(short i,short j)
{

	Boolean answer = FALSE;
	short ter;
	
	ter = (editing_town == TRUE) ? t_d.floor[i][j] : current_terrain.floor[i][j];
	if ((editing_town == TRUE) && ((i < 0) || (i > max_dim[town_type] - 1) || (j < 0) || (j > max_dim[town_type] - 1)))
		return TRUE;
	if ((editing_town == FALSE) && ((i < 0) || (i > 47) || (j < 0) || (j > 47)))
		return TRUE;

	/*if ((ter >= 4) &&
		(ter <= 7)) 
		answer = TRUE;	
	if ((ter >= 11) &&
		(ter <= 22)) 
		answer = TRUE;	
	if ((ter >= 41) &&
		(ter <= 56)) 
		answer = TRUE;	*/

	if (ter == 255)
		answer = TRUE;	
	
	if (scen_data.scen_floors[ter].is_rough)
		answer = TRUE;	
	
	return answer;		
}

// Returns true if space is any sort of water
Boolean is_water(short i,short j)
{
	Boolean answer = FALSE;
	short ter;
	
	ter = (editing_town == TRUE) ? t_d.floor[i][j] : current_terrain.floor[i][j];
	if ((editing_town == TRUE) && ((i < 0) || (i > max_dim[town_type] - 1) || (j < 0) || (j > max_dim[town_type] - 1)))
		return TRUE;
	if ((editing_town == FALSE) && ((i < 0) || (i > 47) || (j < 0) || (j > 47)))
		return TRUE;
	
	if (ter == 255)
		answer = TRUE;	
	if (scen_data.scen_floors[ter].is_water)
		answer = TRUE;	
	
	return answer;		
}

// prob is 0 - 20, 0 no, 20 always
void shy_change_circle_terrain(location center,short radius,short terrain_type,short probability)
{
	location l;
	short i,j;
	short ter;
		
	for (i = 0; i < ((editing_town == TRUE) ? max_dim[town_type] : 48); i++)
		for (j = 0; j < ((editing_town == TRUE) ? max_dim[town_type] : 48); j++) {
			l.x = (t_coord)i;
			l.y = (t_coord)j;
			if (current_drawing_mode == 0) {
				ter = (editing_town == TRUE) ? t_d.floor[i][j] : current_terrain.floor[i][j];
				if ((dist(center,l) <= radius) && (get_ran(1,1,20) <= probability) 
					&& (ter == 0))
					set_terrain(l,terrain_type);
				}
				else {
					ter = (editing_town == TRUE) ? t_d.terrain[i][j] : current_terrain.terrain[i][j];
					if ((dist(center,l) <= radius) && (get_ran(1,1,20) <= probability) 
						&& (ter == 0))
						set_terrain(l,terrain_type);
				
					}
			}
}

//changes terrain within the circle of given radius and center with probability that any given space is changed
void change_circle_terrain(location center,short radius,short terrain_type,short probability)
// prob is 0 - 20, 0 no, 20 always
{
	location l;
	short i,j;

	for (i = 0; i < ((editing_town == TRUE) ? max_dim[town_type] : 48); i++)
		for (j = 0; j < ((editing_town == TRUE) ? max_dim[town_type] : 48); j++) {
			l.x = (t_coord)i;
			l.y = (t_coord)j;
			if ((dist(center,l) <= radius) && (get_ran(1,1,20) <= probability))
				set_terrain(l,terrain_type);
			}
}

//changes height within the circle of given radius and center with probability that any given space is changed
void change_circle_height(location center,short radius,short lower_or_raise,short probability)
// prob is 0 - 20, 0 no, 20 always
{
	location l;
	short i,j;
	for (i = 0; i < ((editing_town == TRUE) ? max_dim[town_type] : 48); i++){
		for (j = 0; j < ((editing_town == TRUE) ? max_dim[town_type] : 48); j++) {
			l.x = i;
			l.y = j;
			if ((dist(center,l) <= radius) && (get_ran(1,1,20) <= probability))
				change_height(l,lower_or_raise);
		}
	}
}

//This is a scan-line flood-fill algorithm to replace all connected spaces of old_floor with new_floor
//squares that touch corners are not considered connected
int flood_fill_floor(short new_floor, short old_floor, int x, int y)
{
	int minx=x, maxx=x;
	if(floors_match(new_floor,old_floor))
		return(-1);
	short i,max_size=((editing_town == TRUE) ? max_dim[town_type] : 48);
	//start at the seed point and move right to find one end of the line
	for (i = x; i < max_size; i++){
		if(editing_town){
			if(!floors_match(t_d.floor[i][y],old_floor)){
				maxx=i-1;
				break;
			}
		}
		else if(!floors_match(current_terrain.floor[i][y],old_floor)){
			maxx=i-1;
			break;
		}
	}
	//if we went all the way to the edge of the zone without finding the end, the end must be the edge
	if((i==max_size) && maxx==x)
		maxx=max_size-1;
	//start at the seed point again and move left to find the other end of the line
	for (i = x; i>=0; i--){
		if(editing_town){
			if(!floors_match(t_d.floor[i][y],old_floor)){
				minx=i+1;
				break;
			}
		}
		else if(!floors_match(current_terrain.floor[i][y],old_floor)){
			minx=i+1;
			break;
		}
	}
	//set the edge of the zone to be the end if we couldn't find the end
	if(i==-1 && minx==x)
		minx=0;
	//fill the line
	location l;
	l.y=y;
	for(i=minx; i<=maxx;i++){
		l.x=i;
		set_terrain(l,new_floor);
	}
	//fill connected spaces in the row above
	if(y>0){
		for(i=minx; i<=maxx;i++){
			if(editing_town && floors_match(t_d.floor[i][y-1],old_floor)){
				if(flood_fill_floor(new_floor,old_floor,i,y-1)>=maxx)
					break;//if the recursive call filled farther out than the
				//end of the current line, we know we don't need to check any farther
			}
			else if(floors_match(current_terrain.floor[i][y-1],old_floor)){
				if(flood_fill_floor(new_floor,old_floor,i,y-1)>=maxx)
					break;
			}
		}
	}
	//fill connected spaces in the row below
	if(y<(max_size-1)){
		for(i=minx; i<=maxx;i++){
			if(editing_town && floors_match(t_d.floor[i][y+1],old_floor)){
				if(flood_fill_floor(new_floor,old_floor,i,y+1)>=maxx)
					break;
			}
			else if(floors_match(current_terrain.floor[i][y+1],old_floor)){
				if(flood_fill_floor(new_floor,old_floor,i,y+1)>=maxx)
					break;
			}
		}
	}
	//return the farthest exent of this scan line; this value is used to
	//optimize the recursion, and can be ignored by any other functions
	//that call this one
	return(maxx);
}

int flood_fill_terrain(short new_terrain, short old_terrain, int x, int y)
{
	int minx=x, maxx=x;
	if(new_terrain==old_terrain)
		return(-1);
	short i,max_size=((editing_town == TRUE) ? max_dim[town_type] : 48);
	for (i = x; i < max_size; i++){
		if(editing_town){
			if(t_d.terrain[i][y]!=old_terrain){
				maxx=i-1;
				break;
			}
		}
		else if(current_terrain.terrain[i][y]!=old_terrain){
			maxx=i-1;
			break;
		}
	}
	if((i==max_size) && maxx==x)
		maxx=max_size-1;
	for (i = x; i>=0; i--){
		if(editing_town){
			if(t_d.terrain[i][y]!=old_terrain){
				minx=i+1;
				break;
			}
		}
		else if(current_terrain.terrain[i][y]!=old_terrain){
			minx=i+1;
			break;
		}
	}
	if(i==-1 && minx==x)
		minx=0;
	for(i=minx; i<=maxx;i++){
		if(editing_town)
			t_d.terrain[i][y]=new_terrain;
		else
			current_terrain.terrain[i][y]=new_terrain;
		appendChangeToLatestStep(new drawingChange(i,y,new_terrain,old_terrain,2));
	}
	if(y>0){
		for(i=minx; i<=maxx;i++){
			if(editing_town && t_d.terrain[i][y-1]==old_terrain){
				if(flood_fill_terrain(new_terrain,old_terrain,i,y-1)>=maxx)
					break;
			}
			else if(current_terrain.terrain[i][y-1]==old_terrain){
				if(flood_fill_terrain(new_terrain,old_terrain,i,y-1)>=maxx)
					break;
			}
		}
	}
	if(y<(max_size-1)){
		for(i=minx; i<=maxx;i++){
			if(editing_town && t_d.terrain[i][y+1]==old_terrain){
				if(flood_fill_terrain(new_terrain,old_terrain,i,y+1)>=maxx)
					break;
			}
			else if(current_terrain.terrain[i][y+1]==old_terrain){
				if(flood_fill_terrain(new_terrain,old_terrain,i,y+1)>=maxx)
					break;
			}
		}
	}
	return(maxx);
}

//Changs the terrain in rectangle r to terrain_type with probability that a each space is 
//changed. If hollow is true, only the edges of the rectangle are changed
void change_rect_terrain(RECT r,short terrain_type,short probability,Boolean hollow)
// prob is 0 - 20, 0 no, 20 always
{
	location l;
	short i,j;
	
	for (i = 0; i < ((editing_town == TRUE) ? max_dim[town_type] : 48); i++){
		for (j = 0; j < ((editing_town == TRUE) ? max_dim[town_type] : 48); j++) {
			l.x = i;
			l.y = j;
			if ((i >= r.left) && (i <= r.right) && (j >= r.top) && (j <= r.bottom)
				&& ((hollow == FALSE) || (i == r.left) || (i == r.right) || (j == r.top) || (j == r.bottom))
				&& ((hollow == TRUE) || (get_ran(1,1,20) <= probability)))
				set_terrain(l,terrain_type);
		}
	}
}

//this function just copies the indicated rectangular area of the current zone onto the clipboard
void copy_rect_terrain(RECT r)
{
	short i,j;
	for (i = 0; i < ((editing_town == TRUE) ? max_dim[town_type] : 48); i++){
		for (j = 0; j < ((editing_town == TRUE) ? max_dim[town_type] : 48); j++) {
			if ((i >= r.left) && (i <= r.right) && (j >= r.top) && (j <= r.bottom)){
				if(editing_town){
					clipboard.terrain[i-r.left][j-r.top]=t_d.terrain[i][j];
					clipboard.floor[i-r.left][j-r.top]=t_d.floor[i][j];
					clipboard.height[i-r.left][j-r.top]=t_d.height[i][j];
				}
				else{
					clipboard.terrain[i-r.left][j-r.top]=current_terrain.terrain[i][j];
					clipboard.floor[i-r.left][j-r.top]=current_terrain.floor[i][j];
					clipboard.height[i-r.left][j-r.top]=current_terrain.height[i][j];
				}
			}
		}
	}
	dataOnClipboard=true;
	clipboardSize.top=0;
	clipboardSize.left=0;
	clipboardSize.bottom=r.bottom-r.top;
	clipboardSize.right=r.right-r.left;
}

void paste_terrain(location l,Boolean option_hit,Boolean alt_hit,Boolean ctrl_hit)
{
	/*command - option_hit
	option - alt_hit
	control - ctrl_hit*/
	if(!dataOnClipboard)
		return;
	short i,j,x,y;
	Boolean pasteF = (!option_hit && !alt_hit && !ctrl_hit) || alt_hit;
	Boolean pasteT = (!option_hit && !alt_hit && !ctrl_hit) || ctrl_hit;
	Boolean pasteH = (!option_hit && !alt_hit && !ctrl_hit) || option_hit;
	for (i = l.x; (i < ((editing_town == TRUE) ? max_dim[town_type] : 48)) && (i<=(l.x+clipboardSize.right)); i++){
		for (j = l.y; (j < ((editing_town == TRUE) ? max_dim[town_type] : 48)) && (j<=(l.y+clipboardSize.bottom)); j++) {
			x = i-l.x;
			y = j-l.y;
			if(editing_town){
				if(pasteT){
					appendChangeToLatestStep(new drawingChange(i,j,clipboard.terrain[x][y],t_d.terrain[i][j],2));
					t_d.terrain[i][j]=clipboard.terrain[i-l.x][j-l.y];
				}
				if(pasteF){
					appendChangeToLatestStep(new drawingChange(i,j,clipboard.floor[x][y],t_d.floor[i][j],1));
					t_d.floor[i][j]=clipboard.floor[i-l.x][j-l.y];
				}
				if(pasteH){
					appendChangeToLatestStep(new drawingChange(i,j,clipboard.height[x][y],t_d.height[i][j],3));
					t_d.height[i][j]=clipboard.height[i-l.x][j-l.y];
				}
			}
			else{
				if(pasteF){
					appendChangeToLatestStep(new drawingChange(i,j,clipboard.terrain[x][y],current_terrain.terrain[i][j],2));
					current_terrain.terrain[i][j]=clipboard.terrain[x][y];
				}
				if(pasteT){
					appendChangeToLatestStep(new drawingChange(i,j,clipboard.floor[x][y],current_terrain.floor[i][j],1));
					current_terrain.floor[i][j]=clipboard.floor[x][y];
				}
				if(pasteH){
					appendChangeToLatestStep(new drawingChange(i,j,clipboard.height[i-l.x][j-l.y],current_terrain.height[i][j],3));
					current_terrain.height[i][j]=clipboard.height[x][y];
				}
			}
		}
	}
}

void set_terrain(location l,short terrain_type)
{
	short i,j,which_sign = -1;
	terrain_type_type ter_info,ter_info2;
	location l2;
	
	i = l.x; j = l.y;
	if ((editing_town == TRUE) && ((i < 0) || (i > max_dim[town_type] - 1) || (j < 0) || (j > max_dim[town_type] - 1)))
		return ;
	if ((editing_town == FALSE) && ((i < 0) || (i > 47) || (j < 0) || (j > 47)))
		return ;
	short old_terrain;
	
	if (current_drawing_mode == 0) {
		if (editing_town){
			old_terrain = t_d.floor[i][j];
			t_d.floor[i][j] = (unsigned char)terrain_type;
		}
		else{
			old_terrain = current_terrain.floor[i][j];
			current_terrain.floor[i][j] = (unsigned char)terrain_type;
		}
	}
	else {
		if (editing_town){
			old_terrain = t_d.terrain[i][j];
			t_d.terrain[i][j] = terrain_type;
		}
		else{
			old_terrain = current_terrain.terrain[i][j];
			current_terrain.terrain[i][j] = terrain_type;
		}
	}
	
	l2 = l;
	appendChangeToLatestStep(new drawingChange(i,j,terrain_type,old_terrain,current_drawing_mode==0?1:2));
	
	adjust_space(l);
	l.x--;
	adjust_space(l);
	l.y--;
	adjust_space(l);
	l.x++;
	adjust_space(l);
	l.x++;
	adjust_space(l);
	l.y++;
	adjust_space(l);
	l.y++;
	adjust_space(l);
	l.x--;
	adjust_space(l);
	l.x--;
	adjust_space(l);
	l.x++;
	l.y--;
	
	// now handle placing signs
	if(current_drawing_mode>0){//if we just placed a floor, it can't be a sign
	Boolean sign_error_received = FALSE;
	ter_info = scen_data.scen_ter_types[terrain_type];
	if (ter_info.special == 39) { // town mode, is a sign
		if (editing_town == TRUE) { /// it's a sign
			for (i = 0; i < 15; i++){ // find is this sign has already been assigned
				if (which_sign < 0) {
					if ((town.sign_locs[i].x == l.x) && (town.sign_locs[i].y == l.y))
						which_sign = i;
				}
			}
			if (which_sign < 0) { // if not assigned, pick a new sign
				for (i = 0; i < 15; i++){
					if ((town.sign_locs[i].x > 64) || (town.sign_locs[i].x < 0) )
						which_sign = i;
				}
			}
				
			if (which_sign >= 0) {
				town.sign_locs[which_sign] = l;
				edit_sign(which_sign);
				sign_error_received = FALSE;
			}
			else {
				if (sign_error_received == FALSE) {
					give_error("Towns can have at most 15 signs. Outdoor sections can have at most 8. When the party looks at this sign, they will get no message.","",0);
					sign_error_received = TRUE;
				}
			}
			mouse_button_held = FALSE;
		}
		if (editing_town == FALSE) { // outdoor mode, it's a sign
			for (i = 0; i < 8; i++) // find is this sign has already been assigned
				if (which_sign < 0) {
					if ((current_terrain.sign_locs[i].x == l.x) && (current_terrain.sign_locs[i].y == l.y))
						which_sign = i;
					}
			if (which_sign < 0) { // if not assigned, pick a new sign
				for (i = 0; i < 8; i++) 
					if ((current_terrain.sign_locs[i].x > 64) || (current_terrain.sign_locs[i].x < 0) )
						which_sign = i;
				}
				
			if (which_sign >= 0) {
				current_terrain.sign_locs[which_sign] = l;
				edit_sign(which_sign);
				sign_error_received = FALSE;
				}
				else {
					if (sign_error_received == FALSE) {
						give_error("Towns can have at most 15 signs. Outdoor sections can have at most 8. When the party looks at this sign, they will get no message.","",0);
						sign_error_received = TRUE;
						}
					}
			mouse_button_held = FALSE;
			}
		}
		else { // not a sign, so delete any signs placed here
			if (editing_town == TRUE) {
				for (i = 0; i < 15; i++){ // find is this spot has a sign atrtached to it, and erase it
					if ((town.sign_locs[i].x == l.x) && (town.sign_locs[i].y == l.y)) {
						town.sign_locs[i].x = kINVAL_LOC_XY;	town.sign_locs[i].y = kINVAL_LOC_XY;
						town.sign_text[i][0] = 0;
					}
				}
			}
			if (editing_town == FALSE) {
				for (i = 0; i < 8; i++){// find is this spot has a sign attached to it, and erase it
					if ((current_terrain.sign_locs[i].x == l.x) && (current_terrain.sign_locs[i].y == l.y)) {
						current_terrain.sign_locs[i].x = kINVAL_LOC_XY;	current_terrain.sign_locs[i].y = kINVAL_LOC_XY;
						current_terrain.sign_text[i][0] = 0;
					}
				}
			}
		}
	}
	
	// now do placing scripts if placing terrain in a town
	if ((editing_town) && (current_drawing_mode > 0)) {
		for (short i = 0; i < NUM_TER_SCRIPTS; i++){ 
			if ((town.ter_scripts[i].exists) && (same_point(l,town.ter_scripts[i].loc))) {
				town.ter_scripts[i].exists = FALSE;
				check_selected_item_number();
			}
		}
		if (strlen(scen_data.scen_ter_types[t_d.terrain[l.x][l.y]].default_script) > 0) {
			create_new_ter_script(scen_data.scen_ter_types[t_d.terrain[l.x][l.y]].default_script,l,NULL);
			mouse_button_held = FALSE;
		}
	}
				
	if(cur_viewing_mode == 11 && editing_town)
		set_up_lights();
	
}

//corrects boundary spaces between grass and rock floors
Boolean town_fix_grass_rocks(location l)
{
	short new_ter_to_place;
	short store_ter;
	short i,j;
	
	i = l.x; j = l.y;	
	if ((i < 0) || (i > max_dim[town_type] - 1) || (j < 0) || (j > max_dim[town_type] - 1))
		return FALSE;
	
	store_ter = t_d.floor[i][j];
	
	if ((store_ter == 4) || (store_ter == 41) ||
		((store_ter >= 11) && (store_ter <= 22)) || ((store_ter >= 45) && (store_ter <= 56))) {
	  	Boolean translated_to_cave = FALSE;
	  	if (store_ter > 22) {
	  		//if (store_ter == 41)
	  		//	store_ter = 11;
	  		//if (store_ter >= 45)
	  		//	store_ter -= 34;
	  		translated_to_cave = TRUE;	
		}
		
		if (is_rocks(i-1,j) == FALSE) {
			if (is_rocks(i,j-1) == FALSE)
				new_ter_to_place = 18;
			else {
				if (is_rocks(i,j+1) == FALSE)
					new_ter_to_place = 12;
				else 
					new_ter_to_place = 11;
			}		
		}
		else {// rocks(i-1,j) == TRUE
			if (is_rocks(i+1,j) == FALSE) {
				if (is_rocks(i, j-1) == FALSE)
					new_ter_to_place = 16;
				else if (is_rocks(i,j+1) == FALSE)
					new_ter_to_place = 14;
				else 
					new_ter_to_place = 15;
			}
			else { //rocks(i+1,j) == TRUE
				if (!is_rocks(i, j-1))
					new_ter_to_place = 17;
				else if (!is_rocks(i,j+1))
					new_ter_to_place = 13;
				else if (!is_rocks(i-1,j-1))
					new_ter_to_place = 20;
				else if(!is_rocks(i-1,j+1))
					new_ter_to_place = 19;
				else if (!is_rocks(i+1,j-1))
					new_ter_to_place = 21;
				else if (!is_rocks(i+1,j+1))
					new_ter_to_place = 22;
				else 
					new_ter_to_place = 4;//(get_ran(1,0,2) == 1) ? 10 : 8; 
			}
		}
		
		if (translated_to_cave) {
	  		if (new_ter_to_place == 4)
	  			new_ter_to_place = 41;
			else 
				new_ter_to_place += 34;
		}
		
		t_d.floor[i][j] = new_ter_to_place; 				
	}
	
	if(store_ter!=t_d.floor[i][j]){
		appendChangeToLatestStep(new drawingChange(i,j,t_d.floor[i][j],store_ter,1));
	}
	return(store_ter != t_d.floor[i][j]);
}

//corrects boundary spaces between grass and rock floors
Boolean out_fix_grass_rocks(location l)
{
	short new_ter_to_place;
	short store_ter;
	short i,j;
	
	i = l.x; j = l.y;
	if ((i < 0) || (i > 47) || (j < 0) || (j > 47))
		return FALSE;
	
	store_ter = current_terrain.floor[i][j];
	
	if ((store_ter == 4) || (store_ter == 41) ||
		((store_ter >= 11) && (store_ter <= 22)) || ((store_ter >= 45) && (store_ter <= 56))) {
	  	Boolean translated_to_cave = FALSE;
	  	if (store_ter > 22) {
	  		//if (store_ter == 41)
	  		//	store_ter = 11;
	  		//if (store_ter >= 45)
	  		//	store_ter -= 34;
	  		translated_to_cave = TRUE;	
		}
		
		if (is_rocks(i-1,j) == FALSE) {
			if (is_rocks(i,j-1) == FALSE) {
				new_ter_to_place = 18;
			}
			else {
				if (is_rocks(i,j+1) == FALSE)
					new_ter_to_place = 12;
				else 
					new_ter_to_place = 11;
			}		
		}
		else {// rocks(i-1,j) == TRUE
			if (is_rocks(i+1,j) == FALSE) {
				if (is_rocks(i, j-1) == FALSE)
					new_ter_to_place = 16;
				else if (is_rocks(i,j+1) == FALSE)
					new_ter_to_place = 14;
				else 
					new_ter_to_place = 15;
			}
			else { //rocks(i+1,j) == TRUE
				if (is_rocks(i, j-1) == FALSE)
					new_ter_to_place = 17;
				else if (is_rocks(i,j+1) == FALSE)
					new_ter_to_place = 13;
				else if (is_rocks(i-1,j-1) == FALSE)
					new_ter_to_place = 20;
				else if(is_rocks(i-1,j+1) == FALSE)
					new_ter_to_place = 19;
				else if (is_rocks(i+1,j-1) == FALSE)
					new_ter_to_place = 21;
				else if (is_rocks(i+1,j+1) == FALSE)
					new_ter_to_place = 22;
				else 
					new_ter_to_place = 4;//(get_ran(1,0,2) == 1) ? 10 : 8; 
			}
			
		}
		
		if (translated_to_cave) {
	  		if (new_ter_to_place == 4)
	  			new_ter_to_place = 41;
			else 
				new_ter_to_place += 34;
		}
		
		current_terrain.floor[i][j] = new_ter_to_place; 				
	}
	if(store_ter!=current_terrain.floor[i][j])
		appendChangeToLatestStep(new drawingChange(i,j,current_terrain.floor[i][j],store_ter,1));
	return(store_ter != current_terrain.floor[i][j]);
}

//corrects boundary spaces between water and rock floors
Boolean town_fix_rocks_water(location l)
{

	short new_ter_to_place;
	short store_ter;
	short i,j;
	
	i = l.x; j = l.y;
	if ((i < 0) || (i > max_dim[town_type] - 1) || (j < 0) || (j > max_dim[town_type] - 1))
		return FALSE;
	
	store_ter = t_d.floor[i][j];
	
	if ((store_ter == 23) || (store_ter == 57) ||
		((store_ter >= 25) && (store_ter <= 36)) || ((store_ter >= 59) && (store_ter <= 70))) {
	  	Boolean translated_to_cave = FALSE;
	  	if (store_ter > 36) {
	  		//if (store_ter == 57)
	  		//	store_ter = 23;
	  		//if (store_ter >= 59)
	  		//	store_ter -= 34;
	  		translated_to_cave = TRUE;	
		}
		
		if (is_water(i-1,j) == FALSE) {
			if (is_water(i,j-1) == FALSE) {
				new_ter_to_place = 36;
			}
			else {
				if (is_water(i,j+1) == FALSE)
					new_ter_to_place = 35;
				else
					new_ter_to_place = 29;
			}
		}					
		else {// rocks(i-1,j) == TRUE
			if (is_water(i+1,j) == FALSE) {
				if (is_water(i, j-1) == FALSE)
					new_ter_to_place = 33;
				else if (is_water(i,j+1) == FALSE)
					new_ter_to_place = 34;
				else 
					new_ter_to_place = 25;
			}
			
			else { //rocks(i+1,j) == TRUE
				if (is_water(i, j-1) == FALSE)
					new_ter_to_place = 27;
				else if (is_water(i,j+1) == FALSE)
					new_ter_to_place = 31;
				else if (is_water(i-1,j-1) == FALSE)
					new_ter_to_place = 28;
				else if(is_water(i-1,j+1) == FALSE)
					new_ter_to_place = 30;
				else if (is_water(i+1,j-1) == FALSE)
					new_ter_to_place = 26;
				else if (is_water(i+1,j+1) == FALSE)
					new_ter_to_place = 32;
				else 
					new_ter_to_place = 23;
			}
			
		}
		
		if (translated_to_cave) {
	  		if (new_ter_to_place == 23)
	  			new_ter_to_place = 57;
			else 
				new_ter_to_place += 34;
		}
		t_d.floor[i][j] = new_ter_to_place; 
	}
	if(store_ter!=t_d.floor[i][j])
		appendChangeToLatestStep(new drawingChange(i,j,t_d.floor[i][j],store_ter,1));
	return(store_ter != t_d.floor[i][j]);
}

//corrects boundary spaces between water and rock floors
Boolean out_fix_rocks_water(location l)
{
	short new_ter_to_place;
	short store_ter;
	short i,j;
	
	i = l.x; j = l.y;
	if ((i < 0) || (i > 47) || (j < 0) || (j > 47))
		return FALSE;
	
	store_ter = current_terrain.floor[i][j];
	
	if ((store_ter == 23) || (store_ter == 57) ||
		((store_ter >= 25) && (store_ter <= 36)) || ((store_ter >= 59) && (store_ter <= 70))) {
	  	Boolean translated_to_cave = FALSE;
	  	if (store_ter > 36) {
	  		//if (store_ter == 57)
	  		//	store_ter = 23;
	  		//if (store_ter >= 59)
	  		//	store_ter -= 34;
	  		translated_to_cave = TRUE;	
		}
		
		if (is_water(i-1,j) == FALSE) {
			if (is_water(i,j-1) == FALSE) {
				new_ter_to_place = 36;
			} else {
				if (is_water(i,j+1) == FALSE)
					new_ter_to_place = 35;
				else new_ter_to_place = 29;
			}
		} else {// rocks(i-1,j) == TRUE
			if (is_water(i+1,j) == FALSE) {
				if (is_water(i, j-1) == FALSE)
					new_ter_to_place = 33;
				else if (is_water(i,j+1) == FALSE)
					new_ter_to_place = 34;
				else new_ter_to_place = 25;
			} else { //rocks(i+1,j) == TRUE
				if (is_water(i, j-1) == FALSE)
					new_ter_to_place = 27;
				else if (is_water(i,j+1) == FALSE)
					new_ter_to_place = 31;
				else if (is_water(i-1,j-1) == FALSE)
					new_ter_to_place = 28;
				else if(is_water(i-1,j+1) == FALSE)
					new_ter_to_place = 30;
				else if (is_water(i+1,j-1) == FALSE)
					new_ter_to_place = 26;
				else if (is_water(i+1,j+1) == FALSE)
					new_ter_to_place = 32;
				else new_ter_to_place = 23;
			}
		}
		
		if (translated_to_cave) {
	  		if (new_ter_to_place == 23)
	  			new_ter_to_place = 57;
			else new_ter_to_place += 34;
		}
		current_terrain.floor[i][j] = new_ter_to_place; 
	}
	if(store_ter!=current_terrain.floor[i][j])
		appendChangeToLatestStep(new drawingChange(i,j,current_terrain.floor[i][j],store_ter,1));
	return(store_ter != current_terrain.floor[i][j]);
}

//auto-hill generation
Boolean town_fix_hills(location l)
{
	short store_ter;
	short cur_height;
	short i;
	short corner_heights[4] = {0,0,0,0}; // 0 - nw, 1 - sw se ne
	int lx = l.x;
	int ly = l.y;
	
	if ((l.x < 0) || (l.x > max_dim[town_type] - 1) || (l.y < 0) || (l.y > max_dim[town_type] - 1))
		return FALSE;
	if (current_height_mode != 1)
		return FALSE;
	store_ter = t_d.terrain[lx][ly];
	cur_height = t_d.height[lx][ly];
	
	if (t_d.floor[lx][ly] == 255)
		return FALSE;
	
	if (get_height(l.x - 1,l.y,1) > cur_height) {
		corner_heights[0] = 1; corner_heights[1] = 1;
	}
	
	if (get_height(l.x - 1,l.y + 1,1) > cur_height) {
		corner_heights[1] = 1;
	}
	
	if (get_height(l.x,l.y + 1,1) > cur_height) {
		corner_heights[2] = 1; corner_heights[1] = 1;
	}
	
	if (get_height(l.x + 1,l.y + 1,1) > cur_height) {
		corner_heights[2] = 1;
	}
	
	if (get_height(l.x + 1,l.y,1) > cur_height) {
		corner_heights[2] = 1; corner_heights[3] = 1;
	}
	
	if (get_height(l.x + 1,l.y - 1,1) > cur_height) {
		corner_heights[3] = 1;
	}
	
	if (get_height(l.x,l.y - 1,1) > cur_height) {
		corner_heights[0] = 1; corner_heights[3] = 1;
	}
	
	if (get_height(l.x - 1,l.y - 1,1) > cur_height) {
		corner_heights[0] = 1;
	}
	
	if(corner_heights[0] || corner_heights[1] || corner_heights[2] || corner_heights[3]){
		//find out what type of hill this requires:
		for (i = 0; i < 12; i++){
			if ((hill_c_heights[i][0] == corner_heights[0]) && 
				(hill_c_heights[i][1] == corner_heights[1]) &&
				(hill_c_heights[i][2] == corner_heights[2]) &&
				(hill_c_heights[i][3] == corner_heights[3]))
				break;
		}
		//if the required hill type doesn't match the type presently there, replace it
		if(i+19!=scen_data.scen_ter_types[t_d.terrain[lx][ly]].special){
			short hill_adjust = 0;
			if (town.is_on_surface == FALSE)
				hill_adjust = 32;
			else if (scen_data.scen_floors[t_d.floor[lx][ly]].is_rough)
				hill_adjust = 16;
			t_d.terrain[lx][ly] = i + 74 + hill_adjust;
		}
	} //handle the case where there is a slope where none is now needed:
	else if ((t_d.terrain[lx][ly] >= 74) && (t_d.terrain[lx][ly] <= 121))
		t_d.terrain[lx][ly] = 0;
	if(store_ter != t_d.terrain[lx][ly])
		appendChangeToLatestStep(new drawingChange(lx,ly,t_d.terrain[lx][ly],store_ter,2));
	return(store_ter != t_d.terrain[lx][ly]);
}

//outdoor auto-hill generation
Boolean out_fix_hills(location l)
{
	short store_ter;
	short cur_height;
	short i;
	short corner_heights[4] = {0,0,0,0}; // 0 - nw, 1 - sw se ne
	int lx = l.x;
	int ly = l.y;
	
	if ((l.x < 0) || (l.x > 47) || (l.y < 0) || (l.y > 47))
		return FALSE;
	if (current_height_mode != 1)
		return FALSE;
	
	store_ter = current_terrain.terrain[lx][ly];
	cur_height = current_terrain.height[lx][ly];
	
	if (current_terrain.floor[lx][ly] == 255)
		return FALSE;
	
	if (get_height(l.x - 1,l.y,1) > cur_height) {
		corner_heights[0] = 1; corner_heights[1] = 1;
	}
	
	if (get_height(l.x - 1,l.y + 1,1) > cur_height) {
		corner_heights[1] = 1;
	}
	
	if (get_height(l.x,l.y + 1,1) > cur_height) {
		corner_heights[2] = 1; corner_heights[1] = 1;
	}
	
	if (get_height(l.x + 1,l.y + 1,1) > cur_height) {
		corner_heights[2] = 1;
	}
	
	if (get_height(l.x + 1,l.y,1) > cur_height) {
		corner_heights[2] = 1; corner_heights[3] = 1;
	}
	
	if (get_height(l.x + 1,l.y - 1,1) > cur_height) {
		corner_heights[3] = 1;
	}
	
	if (get_height(l.x,l.y - 1,1) > cur_height) {
		corner_heights[0] = 1; corner_heights[3] = 1;
	}
	
	if (get_height(l.x - 1,l.y - 1,1) > cur_height) {
		corner_heights[0] = 1;
	}
	
	if(corner_heights[0] || corner_heights[1] || corner_heights[2] || corner_heights[3]){
		//find out what type of hill this requires:
		for (i = 0; i < 12; i++){
			if ((hill_c_heights[i][0] == corner_heights[0]) && 
				(hill_c_heights[i][1] == corner_heights[1]) &&
				(hill_c_heights[i][2] == corner_heights[2]) &&
				(hill_c_heights[i][3] == corner_heights[3]))
				break;
		}
		//if the required hill type doesn't match the type presently there, replace it
		if(i+19!=scen_data.scen_ter_types[current_terrain.terrain[lx][ly]].special){
			short hill_adjust = 0;
			if (current_terrain.is_on_surface == FALSE)
				hill_adjust = 32;
			else if (scen_data.scen_floors[current_terrain.floor[lx][ly]].is_rough)
				hill_adjust = 16;
			current_terrain.terrain[lx][ly] = i + 74 + hill_adjust;
		}
	} //handle the case where there is a slope where none is now needed:
	else if ((current_terrain.terrain[lx][ly] >= 74) && (current_terrain.terrain[lx][ly] <= 121))
		current_terrain.terrain[lx][ly] = 0;
	//record any change made to the undo list
	if(store_ter != current_terrain.terrain[lx][ly])
		appendChangeToLatestStep(new drawingChange(lx,ly,current_terrain.terrain[lx][ly],store_ter,2));
	return(store_ter != current_terrain.terrain[lx][ly]);
}

// given a spot in the world and a corner, returns 1 if that corner is elevated
// 0 - level with floor, 1 - above floor
// which corner 0 - nw, 1 - sw, 2 - se, 3 - ne
short get_corner_height(short x, short y,short out_or_town,short which_corner) 
{
	short new_ter;
	
	new_ter = adjust_get_ter(x,y,out_or_town);
	if ((scen_data.scen_ter_types[new_ter].special < 19) || (scen_data.scen_ter_types[new_ter].special > 30))
		return 0;
	return hill_c_heights[scen_data.scen_ter_types[new_ter].special - 19][which_corner];
}

//returns the height at location (x,y) in the outdoor section or town
short get_height(short x, short y,short out_or_town)
{
	if (x < 0) x = 0;
	if (y < 0) y = 0;
	
	if (editing_town == FALSE) {
		if (x > 47) x = 47;
		if (y > 47) y = 47;
		
		return current_terrain.height[x][y];
	}
	if (x > max_dim[town_type] - 1) x = max_dim[town_type] - 1;
	if (y > max_dim[town_type] - 1) y = max_dim[town_type] - 1;
	
	return t_d.height[x][y];
}

// does boundary check and returns 0 if off world
short adjust_get_ter(short x, short y,short out_or_town)
{
	if (x < 0) return 0;
	if (y < 0) return 0;
	
	if (x > MAX_TOWN_SIZE - 1)  return 0;
	if (y > MAX_TOWN_SIZE - 1)  return 0;
	
	return (editing_town) ? t_d.terrain[x][y] : current_terrain.terrain[x][y];
}

//starting function for recursive cleaning
void adjust_space(location l)
{
	recursive_depth = 0;
	recursive_clean_terrain(l);
}

//recursively does auto-hill corections and floor boundary corrections
void recursive_clean_terrain(location l)
{
	Boolean needed_change = FALSE;
	location l2;
	short i,j;
	
	if (recursive_depth >= MAX_RECURSION_DEPTH)
		return;
	
	i = l.x; j = l.y;
	if ((editing_town == TRUE) && ((i < 0) || (i > max_dim[town_type] - 1) || (j < 0) || (j > max_dim[town_type] - 1)))
		return ;
	if ((editing_town == FALSE) && ((i < 0) || (i > 47) || (j < 0) || (j > 47)))
		return ;
	if (editing_town == TRUE) {
		if (town_fix_hills(l) == TRUE)
			needed_change = TRUE;
		if (town_fix_grass_rocks(l) == TRUE)
			needed_change = TRUE;
		if (town_fix_rocks_water(l) == TRUE)
			needed_change = TRUE;
	}
	if (editing_town == FALSE) {
		if (out_fix_hills(l) == TRUE)
			needed_change = TRUE;
		if (out_fix_grass_rocks(l) == TRUE)
			needed_change = TRUE;
		if (out_fix_rocks_water(l) == TRUE)
			needed_change = TRUE;
	}
	if (needed_change == TRUE) {
		recursive_depth++;
		l2 = l;
		l2.x--;
		recursive_clean_terrain(l2);
		l2.x += 2;
		recursive_clean_terrain(l2);
		l2.x--;
		l2.y--;
		recursive_clean_terrain(l2);
		l2.y += 2;
		recursive_clean_terrain(l2); 
		recursive_depth--;
	}
}


/*
short coord_to_ter(short x,short y)
{
	short what_terrain;
	if (x < 0) return 0;
	if (y < 0) return 0;
	if (x > (editing_town) ? max_dim[town_type] - 1 : 47)  return 0;
	if (y > (editing_town) ? max_dim[town_type] - 1 : 47)  return 0;

	if (editing_town == TRUE)
		what_terrain = t_d.terrain[x][y];
		else what_terrain = current_terrain.terrain[x][y];

	return what_terrain;
}
*/

//lowers or raises the space at location l
void change_height(location l,short lower_or_raise)
{
	Boolean needed_change = TRUE;
	short i,j;
	
	i = l.x; j = l.y;
	if ((editing_town == TRUE) && ((i < 0) || (i > max_dim[town_type] - 1) || (j < 0) || (j > max_dim[town_type] - 1)))
		return ;
	if ((editing_town == FALSE) && ((i < 0) || (i > 47) || (j < 0) || (j > 47)))
		return ;
	
	if (editing_town) {
		appendChangeToLatestStep(new drawingChange(i,j,minmax(1,100,(lower_or_raise == 0) ? (t_d.height[i][j] - 1) : (t_d.height[i][j] + 1)),t_d.height[i][j],3));
		t_d.height[i][j] = (unsigned char)minmax(1,100,(lower_or_raise == 0) ? (t_d.height[i][j] - 1) : (t_d.height[i][j] + 1));
	}
	else{
		appendChangeToLatestStep(new drawingChange(i,j,minmax(1,100,(lower_or_raise == 0) ? (current_terrain.height[i][j] - 1) : (current_terrain.height[i][j] + 1)),current_terrain.height[i][j],3));
		current_terrain.height[i][j] = (unsigned char)minmax(1,100,(lower_or_raise == 0) ? (current_terrain.height[i][j] - 1) : (current_terrain.height[i][j] + 1));
	}
	
	if (needed_change == TRUE) {
		adjust_space_height(l,lower_or_raise);
		l.x--;
		adjust_space_height(l,lower_or_raise);
		l.y--;
		adjust_space_height(l,lower_or_raise);
		l.x++;
		adjust_space_height(l,lower_or_raise);
		l.x++;
		adjust_space_height(l,lower_or_raise);
		l.y++;
		adjust_space_height(l,lower_or_raise);
		l.y++;
		adjust_space_height(l,lower_or_raise);
		l.x--;
		adjust_space_height(l,lower_or_raise);
		l.x--;
		adjust_space_height(l,lower_or_raise);
		l.x++;
		l.y--;	
		adjust_space(l);
		l.x--;
		adjust_space(l);
		l.y--;
		l.x++;
		adjust_space(l);
		l.y++;
		l.x++;
		adjust_space(l);
		l.y++;
		l.x--;
		adjust_space(l);
	}
}

void adjust_space_height(location l,short lower_or_raise)
{
	if (current_height_mode != 1)
		return;
	if (lower_or_raise == 0)
		adjust_space_height_lower(l);
	else
		adjust_space_height_raise(l);
}

void adjust_space_height_lower(location l)
{
	recursive_hill_down_depth = 0;
	recursive_adjust_space_height_lower(l);
	adjust_space(l);
}

void recursive_adjust_space_height_lower(location l)
{
	short i,j,height,old_height;
	
	if (recursive_hill_down_depth >= MAX_RECURSION_DEPTH)
		return;
	
	i = l.x; j = l.y;
	if ((editing_town == TRUE) && ((i < 0) || (i > max_dim[town_type] - 1) || (j < 0) || (j > max_dim[town_type] - 1)))
		return ;
	if ((editing_town == FALSE) && ((i < 0) || (i > 47) || (j < 0) || (j > 47)))
		return ;
	
	old_height = height = get_height(i,j,editing_town);
	
	if (height - 1 > get_height(i + 1,j,editing_town))
		height = get_height(i + 1,j,editing_town) + 1;
	if (height - 1 > get_height(i - 1,j,editing_town))
		height = get_height(i - 1,j,editing_town) + 1;
	if (height - 1 > get_height(i,j + 1,editing_town))
		height = get_height(i,j + 1,editing_town) + 1;
	if (height - 1 > get_height(i,j - 1,editing_town))
		height = get_height(i,j - 1,editing_town) + 1;
	if (height - 1 > get_height(i + 1,j + 1,editing_town))
		height = get_height(i + 1,j + 1,editing_town) + 1;
	if (height - 1 > get_height(i - 1,j + 1,editing_town))
		height = get_height(i - 1,j + 1,editing_town) + 1;
	if (height - 1 > get_height(i + 1,j - 1,editing_town))
		height = get_height(i + 1,j - 1,editing_town) + 1;
	if (height - 1 > get_height(i - 1,j - 1,editing_town))
		height = get_height(i - 1,j - 1,editing_town) + 1;
	
	if (old_height != height) {
		if (editing_town == TRUE)
			t_d.height[i][j] = (unsigned char)height;
		else
			current_terrain.height[i][j] = (unsigned char)height;
		appendChangeToLatestStep(new drawingChange(i,j,height,old_height,3));
		//if (editing_town == TRUE)
		//	else out_fix_hills(l);
		recursive_hill_down_depth++;
		l.x--;
		recursive_adjust_space_height_lower(l);
		l.y--;
		l.x++;
		recursive_adjust_space_height_lower(l);
		l.x++;
		l.y++;
		recursive_adjust_space_height_lower(l);
		l.y++;
		l.x--;
		recursive_adjust_space_height_lower(l);
		
		recursive_hill_down_depth--;
	}
	adjust_space(l);	
}

void adjust_space_height_raise(location l)
{
	recursive_hill_up_depth = 0;
	recursive_adjust_space_height_raise(l);
	adjust_space(l);
}

void recursive_adjust_space_height_raise(location l)
{
	short i,j,height,old_height;
	
	if (recursive_hill_up_depth >= MAX_RECURSION_DEPTH)
		return;
	
	i = l.x; j = l.y;
	if ((editing_town == TRUE) && ((i < 0) || (i > max_dim[town_type] - 1) || (j < 0) || (j > max_dim[town_type] - 1)))
		return ;
	if ((editing_town == FALSE) && ((i < 0) || (i > 47) || (j < 0) || (j > 47)))
		return ;
	
	old_height = height = get_height(i,j,editing_town);
	
	if (height + 1 < get_height(i + 1,j,editing_town))
		height = get_height(i + 1,j,editing_town) - 1;
	if (height + 1 < get_height(i - 1,j,editing_town))
		height = get_height(i - 1,j,editing_town) - 1;
	if (height + 1 < get_height(i,j + 1,editing_town))
		height = get_height(i,j + 1,editing_town) - 1;
	if (height + 1 < get_height(i,j - 1,editing_town))
		height = get_height(i,j - 1,editing_town) - 1;
	if (height + 1 < get_height(i + 1,j + 1,editing_town))
		height = get_height(i + 1,j + 1,editing_town) - 1;
	if (height + 1 < get_height(i - 1,j + 1,editing_town))
		height = get_height(i - 1,j + 1,editing_town) - 1;
	if (height + 1 < get_height(i + 1,j - 1,editing_town))
		height = get_height(i + 1,j - 1,editing_town) - 1;
	if (height + 1 < get_height(i - 1,j - 1,editing_town))
		height = get_height(i - 1,j - 1,editing_town) - 1;
	
	if (old_height != height) {
		if (editing_town == TRUE)
			t_d.height[i][j] = (unsigned char)height;
		else
			current_terrain.height[i][j] = (unsigned char)height;
		appendChangeToLatestStep(new drawingChange(i,j,height,old_height,3));
		//if (editing_town == TRUE)
		//	else out_fix_hills(l);
		recursive_hill_up_depth++;
		l.x--;
		recursive_adjust_space_height_raise(l);
		l.y--;
		l.x++;
		recursive_adjust_space_height_raise(l);
		l.x++;
		l.y++;
		recursive_adjust_space_height_raise(l);
		l.y++;
		l.x--;
		recursive_adjust_space_height_raise(l);
		
		recursive_hill_up_depth--;
	}
	adjust_space(l);
}

//disables menu items which aren't applicable at the moment
void shut_down_menus()
{
	HMENU menu;

	menu = GetMenu(mainPtr);
	//	EnableMenuItem(menu,2,MF_ENABLED | MF_BYCOMMAND);
		//	EnableMenuItem(menu,town_items[i],MF_GRAYED | MF_BYCOMMAND);

	if (file_is_loaded == FALSE) {
		EnableMenuItem(menu,2,MF_GRAYED | MF_BYCOMMAND);
		//EnableMenuItem(menu,3,MF_GRAYED | MF_BYCOMMAND);

		for (short i = 101; i < 109; i++)
			EnableMenuItem(menu,i,MF_GRAYED | MF_BYCOMMAND);
		for (short i = 201; i < 221; i++)
			EnableMenuItem(menu,i,MF_GRAYED | MF_BYCOMMAND);
		for (short i = 301; i < 320; i++)
			EnableMenuItem(menu,i,MF_GRAYED | MF_BYCOMMAND);
		for (short i = 401; i < 413; i++)
			EnableMenuItem(menu,i,MF_GRAYED | MF_BYCOMMAND);
		for (short i = 600; i < 1100; i++)
			EnableMenuItem(menu,i,MF_GRAYED | MF_BYCOMMAND);
		for (short i = 1100; i < 1356; i++)
			EnableMenuItem(menu,i,MF_GRAYED | MF_BYCOMMAND);
		for (short i = 1503; i < 1520; i++)
			EnableMenuItem(menu,i,MF_GRAYED | MF_BYCOMMAND);

		return;
	}
	else {
		EnableMenuItem(menu,2,MF_ENABLED | MF_BYCOMMAND);
		EnableMenuItem(menu,3,MF_ENABLED | MF_BYCOMMAND);
		for (short i = 201; i < 221; i++)
			EnableMenuItem(menu,i,MF_ENABLED | MF_BYCOMMAND);

	}

	if (editing_town == TRUE) {
		for (short i = 101; i < 109; i++)
			EnableMenuItem(menu,i,MF_ENABLED | MF_BYCOMMAND);
		for (short i = 301; i < 320; i++)
			EnableMenuItem(menu,i,MF_ENABLED | MF_BYCOMMAND);
		for (short i = 401; i < 413; i++)
			EnableMenuItem(menu,i,MF_GRAYED | MF_BYCOMMAND);
		for (short i = 600; i < 1100; i++)
			EnableMenuItem(menu,i,MF_ENABLED | MF_BYCOMMAND);
		for (short i = 1100; i < 1356; i++)
			EnableMenuItem(menu,i,MF_ENABLED | MF_BYCOMMAND);
		for (short i = 1503; i < 1520; i++)
			EnableMenuItem(menu,i,MF_ENABLED | MF_BYCOMMAND);
	}
	else {
		EnableMenuItem(menu,101,MF_ENABLED | MF_BYCOMMAND);
		EnableMenuItem(menu,102,MF_ENABLED | MF_BYCOMMAND);
		for (short i = 103; i < 109; i++)
			EnableMenuItem(menu,i,MF_GRAYED | MF_BYCOMMAND);
		for (short i = 301; i < 320; i++)
			EnableMenuItem(menu,i,MF_GRAYED | MF_BYCOMMAND);
		for (short i = 401; i < 413; i++)
			EnableMenuItem(menu,i,MF_ENABLED | MF_BYCOMMAND);
		for (short i = 600; i < 1100; i++)
			EnableMenuItem(menu,i,MF_GRAYED | MF_BYCOMMAND);
		for (short i = 1100; i < 1356; i++)
			EnableMenuItem(menu,i,MF_GRAYED | MF_BYCOMMAND);
		for (short i = 1503; i < 1520; i++)
			EnableMenuItem(menu,i,MF_ENABLED | MF_BYCOMMAND);
	}
}

//Called when the data in memory is going to be overwritten or dumped. 
//Returns TRUE if the user hasn't made any changes to the data, so it can be safely thrown away.
//If the user has made changes, dialog which_dlog is displayed to ask the user whether to save 
//the changes, and does so if so instructed. Returns whether the user chose to save the changes or not.
Boolean save_check(short which_dlog)
{
	short choice;
	
	if (change_made_town == FALSE && change_made_outdoors == FALSE)
		return TRUE;
	choice = fancy_choice_dialog(which_dlog,0);
	if (choice == 3)
		return TRUE;
	if (choice == 2) {
		return FALSE;
	}
	save_campaign();
	return TRUE;
}

//refreshs the menu items in the item menus to reflect the current item definitions
void update_item_menu()
{
	short i,j;
	HMENU menu[14],big_menu;
	char item_name[256];

	big_menu = GetMenu(mainPtr);

	for (i = 0; i < 14; i++)
		menu[i] = GetSubMenu(big_menu,5 + i);

	for (j = 0; j < 10; j++) {
		for (i = 0; i < 50; i++) {
			DeleteMenu(menu[j],0,MF_BYPOSITION);
			}
		if (overall_mode < 61) {
			for (i = 0; i < 50; i++) {
					if (strlen(scen_data.scen_items[i + j * 50].full_name) == 0)
						sprintf(scen_data.scen_items[i + j * 50].full_name,"Unused");
					sprintf((char *) item_name, "%d - %s",(int)(i + j * 50), scen_data.scen_items[i + j * 50].full_name);
					if ((i % 25 == 0) && (i > 0))
						AppendMenu(menu[j],MF_MENUBREAK | MF_BYCOMMAND | MF_ENABLED | MF_STRING, 600 + (50 * j) + i, item_name);
						else AppendMenu(menu[j],MF_BYCOMMAND | MF_ENABLED | MF_STRING, 600 + (50 * j) + i, item_name);
					}
			}
		}

	for (j = 0; j < 4; j++) {
		for (i = 0; i < 64; i++) {
			DeleteMenu(menu[j + 10],0,MF_BYPOSITION);
			}
		if (overall_mode < 61) {
			for (i = 0; i < 64; i++) {
					if (same_string(scen_data.scen_creatures[i + j * 64].name,"Unused") == FALSE)
						sprintf((char *) item_name, "%d - %s, L%d",
					  (int)(i + j * 64), scen_data.scen_creatures[i + j * 64].name,
					  (int)scen_data.scen_creatures[i + j * 64].level);
						else sprintf((char *) item_name, "%d - %s",
					  (int)(i + j * 64), scen_data.scen_creatures[i + j * 64].name);

					if ((i % 32 == 0) && (i > 0))
						AppendMenu(menu[j + 10],MF_MENUBREAK | MF_BYCOMMAND | MF_ENABLED | MF_STRING, 1100 + (64 * j) + i, item_name);
						else AppendMenu(menu[j + 10],MF_BYCOMMAND | MF_ENABLED | MF_STRING, 1100 + (64 * j) + i, item_name);
					}
				}
			}
			

}

// given 2 locations, finds the direction (0 - 7)
// from l1 to l2
short locs_to_dir(location l1,location l2)
{
	if (l1.x < l2.x) {
		if (l2.y < l1.y)
			return 7;
		if (l2.y == l1.y)
			return 6;
		return 5;
	}
	if (l1.x > l2.x) {
		if (l2.y < l1.y)
			return 1;
		if (l2.y == l1.y)
			return 2;
		return 3;
	}
	if (l2.y < l1.y)
		return 0;
	return 4;
}

//destroys whatever creature, terrain script, or item the user has selected
void delete_selected_instance()
{					
	if (selected_item_number < 0) {
		beep();
		return;
	}		
	// select creature
	if ((selected_item_number >= 7000) && (selected_item_number < 7000 + NUM_TOWN_PLACED_CREATURES)) {
		town.creatures[selected_item_number % 1000].number = -1;
	}
	
	// select ter script
	if ((selected_item_number >= 9000) && (selected_item_number < 9000 + NUM_TER_SCRIPTS)) {
		town.ter_scripts[selected_item_number % 1000].exists = FALSE;
	}
	
	// select item
	if ((selected_item_number >= 11000) && (selected_item_number < 11000 + NUM_TOWN_PLACED_ITEMS)) {
		town.preset_items[selected_item_number % 1000].which_item = -1;
	}				
	check_selected_item_number();
	set_all_items_containment();
}

//copies the selected creature, terrain script, or item for later pasting
//NOTE: this actually only makes a reference to the copied item
void copy_selected_instance()
{		
	if (selected_item_number < 0) {
		beep();
		return;
	}		
	copied_creature.number = -1;			
	copied_item.which_item = -1;
	copied_ter_script.exists = FALSE;	
	
	// select creature
	if ((selected_item_number >= 7000) && (selected_item_number < 7000 + NUM_TOWN_PLACED_CREATURES)) {
		copied_creature = town.creatures[selected_item_number % 1000];
	}
	
	// select ter script
	if ((selected_item_number >= 9000) && (selected_item_number < 9000 + NUM_TER_SCRIPTS)) {
		copied_ter_script = town.ter_scripts[selected_item_number % 1000];
	}
	
	// select item
	if ((selected_item_number >= 11000) && (selected_item_number < 11000 + NUM_TOWN_PLACED_ITEMS)) {
		copied_item = town.preset_items[selected_item_number % 1000];
	}					
	set_all_items_containment();
}

//copies and then deletes the selected creature, terrain script or item.
void cut_selected_instance()
{			
	if (selected_item_number < 0) {
		beep();
		return;
	}		
	copy_selected_instance();	
	delete_selected_instance();		
	set_all_items_containment();
}

void paste_selected_instance(location create_loc)
{	
	// select creature
	if (copied_creature.number >= 0) {
		create_new_creature(copied_creature.number,create_loc,&copied_creature);
	}
	
	// select ter script
	if (copied_ter_script.exists) {
		create_new_ter_script("dummyname",create_loc,&copied_ter_script);
	}
	
	// select item
	if (copied_item.which_item >= 0) {
		create_new_item(copied_item.which_item,create_loc,FALSE,&copied_item);
	}					
	set_all_items_containment();
}

// looks to make sure that the selected item is still legal. If not, undoes it.
void check_selected_item_number()
{
	// select creature
	if ((selected_item_number >= 7000) && (selected_item_number < 7000 + NUM_TOWN_PLACED_CREATURES) && (town.creatures[selected_item_number % 1000].exists() == FALSE)) {
		selected_item_number = -1;
	}
	
	// select ter script
	if ((selected_item_number >= 9000) && (selected_item_number < 9000 + NUM_TER_SCRIPTS) && (town.ter_scripts[selected_item_number % 1000].exists == FALSE)) {
		selected_item_number = -1;
	}
	
	// select item
	if ((selected_item_number >= 11000) && (selected_item_number < 11000 + NUM_TOWN_PLACED_ITEMS) && (town.preset_items[selected_item_number % 1000].exists() == FALSE)) {
		selected_item_number = -1;
	}	
	set_all_items_containment();
}

void clear_selected_copied_objects()
{
	selected_item_number = -1;
	copied_creature.number = -1;
	copied_item.which_item = -1;
	copied_ter_script.exists = FALSE;
}

location selected_instance_location(){
	location loc={-1,-1};
	if ((selected_item_number >= 7000) && (selected_item_number < 7000 + NUM_TOWN_PLACED_CREATURES) && (town.creatures[selected_item_number % 1000].exists()))
		loc = town.creatures[selected_item_number % 1000].start_loc;
	else if ((selected_item_number >= 9000) && (selected_item_number < 9000 + NUM_TER_SCRIPTS) && (town.ter_scripts[selected_item_number % 1000].exists))
		loc = town.ter_scripts[selected_item_number % 1000].loc;
	else if ((selected_item_number >= 11000) && (selected_item_number < 11000 + NUM_TOWN_PLACED_ITEMS) && (town.preset_items[selected_item_number % 1000].exists()))
		loc = town.preset_items[selected_item_number % 1000].item_loc;
	return(loc);
}

void shift_selected_instance(short dx,short dy)
{
	// select creature
	if ((selected_item_number >= 7000) && (selected_item_number < 7000 + NUM_TOWN_PLACED_CREATURES) && (town.creatures[selected_item_number % 1000].exists())) {
		location current_loc = town.creatures[selected_item_number % 1000].start_loc;
		
		current_loc.x += dx;
		current_loc.y += dy;
		
		if (loc_in_active_area(current_loc) == FALSE)
			return;
		
		town.creatures[selected_item_number % 1000].start_loc = current_loc;
	}
	
	// select ter script
	if ((selected_item_number >= 9000) && (selected_item_number < 9000 + NUM_TER_SCRIPTS) && (town.ter_scripts[selected_item_number % 1000].exists)) {
		location current_loc = town.ter_scripts[selected_item_number % 1000].loc;
		
		current_loc.x += dx;
		current_loc.y += dy;
		
		if (loc_in_active_area(current_loc) == FALSE)
			return;
		
		town.ter_scripts[selected_item_number % 1000].loc = current_loc;
	}
	
	// select item
	if ((selected_item_number >= 11000) && (selected_item_number < 11000 + NUM_TOWN_PLACED_ITEMS) && (town.preset_items[selected_item_number % 1000].exists())) {
		location current_loc = town.preset_items[selected_item_number % 1000].item_loc;
		
		current_loc.x += dx;
		current_loc.y += dy;
		
		if (loc_in_active_area(current_loc) == FALSE)
			return;
		
		town.preset_items[selected_item_number % 1000].item_loc = current_loc;
		shift_item_locs(current_loc);
	}			
	
	set_all_items_containment();		
}

void rotate_selected_instance(int dir)
{
	//only work on creatures
	if ((selected_item_number >= 7000) && (selected_item_number < 7000 + NUM_TOWN_PLACED_CREATURES) && (town.creatures[selected_item_number % 1000].exists())) {
		short facing = town.creatures[selected_item_number % 1000].facing + dir;
		if(facing<0)
			facing+=4;
		else if(facing>3)
			facing-=4;
		town.creatures[selected_item_number % 1000].facing = facing;
	}
}

void create_navpoint(location spot_hit)
{
	short i;
	
	for (i = 0; i < NUM_WAYPOINTS; i++) {
		if (town.waypoints[i].x < 0) {
			town.waypoints[i] = spot_hit;
			return;
		}
	}
}

void delete_navpoint(location spot_hit)
{
	short i;
	
	for (i = 0; i < NUM_WAYPOINTS; i++) {
		if (same_point(town.waypoints[i],spot_hit)) {
			town.waypoints[i].x = kINVAL_LOC_XY;	town.waypoints[i].y = kINVAL_LOC_XY;
			return;
		}
	}
}

void frill_terrain()
{
	short i,j;
	short terrain_type;
	unsigned char floor_type;
	
	for (i = 0; i < ((editing_town == TRUE) ? max_dim[town_type] : 48); i++){
		for (j = 0; j < ((editing_town == TRUE) ? max_dim[town_type] : 48); j++) {
			terrain_type = (editing_town == TRUE) ? t_d.terrain[i][j] : current_terrain.terrain[i][j];
			floor_type = (editing_town == TRUE) ? t_d.floor[i][j] : current_terrain.floor[i][j];
			
			// cave type floors
			if ((floor_type == 0) && (get_ran(1,1,20) < 4))
				floor_type = 3;
			if ((floor_type == 0) && (get_ran(1,1,20) < 3))
				floor_type = 1;
			if ((floor_type == 0) && (get_ran(1,1,20) < 2))
				floor_type = 2;
			if ((floor_type == 4) && (get_ran(1,1,20) < 4))
				floor_type = 7;
			if ((floor_type == 4) && (get_ran(1,1,20) < 3))
				floor_type = 5;
			if ((floor_type == 4) && (get_ran(1,1,20) < 2))
				floor_type = 6;
			if ((floor_type == 8) && (get_ran(1,1,20) < 6))
				floor_type = 10;
			if ((floor_type == 72) && (get_ran(1,1,100) < 5))
				floor_type = 73;
			if ((floor_type == 72) && (get_ran(1,1,100) < 5))
				floor_type = 74;
			
			// terrain type floors
			if ((floor_type == 37) && (get_ran(1,1,20) < 4))
				floor_type = 40;
			if ((floor_type == 37) && (get_ran(1,1,20) < 3))
				floor_type = 38;
			if ((floor_type == 37) && (get_ran(1,1,20) < 2))
				floor_type = 39;
			if ((floor_type == 41) && (get_ran(1,1,20) < 4))
				floor_type = 44;
			if ((floor_type == 41) && (get_ran(1,1,40) == 1))
				floor_type = 42;
			if ((floor_type == 41) && (get_ran(1,1,20) < 2))
				floor_type = 43;
			
			if (editing_town == TRUE)
				t_d.terrain[i][j] = terrain_type;
			else current_terrain.terrain[i][j] = terrain_type;
			if (editing_town == TRUE)
				t_d.floor[i][j] = floor_type;
			else current_terrain.floor[i][j] = floor_type;
		}
	}
	draw_terrain();
}

void unfrill_terrain()
{
	short i,j;
	short terrain_type;
	unsigned char floor_type;
	
	for (i = 0; i < ((editing_town == TRUE) ? max_dim[town_type] : 48); i++){
		for (j = 0; j < ((editing_town == TRUE) ? max_dim[town_type] : 48); j++) {
			terrain_type = (editing_town == TRUE) ? t_d.terrain[i][j] : current_terrain.terrain[i][j];
			floor_type = (editing_town == TRUE) ? t_d.floor[i][j] : current_terrain.floor[i][j];
			
			if (floor_type == 3)
				floor_type = 0;
			if (floor_type == 2)
				floor_type = 0;
			if (floor_type == 1)
				floor_type = 0;
			if (floor_type == 5)
				floor_type = 4;
			if (floor_type == 6)
				floor_type = 4;
			if (floor_type == 10)
				floor_type = 8;
			if (floor_type == 73)
				floor_type = 72;
			if (floor_type == 74)
				floor_type = 72;
			
			if (floor_type == 38)
				floor_type = 37;
			if (floor_type == 39)
				floor_type = 37;
			if (floor_type == 40)
				floor_type = 37;
			if (floor_type == 42)
				floor_type = 41;
			if (floor_type == 43)
				floor_type = 41;
			if (floor_type == 44)
				floor_type = 41;
			
			if (editing_town == TRUE)
				t_d.terrain[i][j] = terrain_type;
			else current_terrain.terrain[i][j] = terrain_type;
			if (editing_town == TRUE)
				t_d.floor[i][j] = floor_type;
			else current_terrain.floor[i][j] = floor_type;
		}
	}
	draw_terrain();
}

void create_new_creature(short c_to_create,location create_loc,creature_start_type *c_to_make)
{
	short i;
	
	//if (!strcmp(scen_data.scen_creatures[c_to_create].name,"Unused"))
	//	return;
	if (c_to_create < 0)	
		return;
	if (loc_in_active_area(create_loc) == FALSE) {
		give_error("You can't place a creature here. This space is outside of the active town area.","",0);
		return;
	}
	
	for (i = 0; i < NUM_TOWN_PLACED_CREATURES; i++){
		if ((town.creatures[i].exists()) && (same_point(town.creatures[i].start_loc,create_loc)))  {
		  	beep();
		  	return;
		}
	}
	
	for (i = 0; i < NUM_TOWN_PLACED_CREATURES; i++){
		if (town.creatures[i].exists() == FALSE) {
			town.creatures[i].clear_creature_start_type();
			selected_item_number = 7000 + i;
			if (c_to_make != NULL) {
				town.creatures[i] = *c_to_make;
				town.creatures[i].start_loc = create_loc;
				town.creatures[i].character_id = cur_town * 100 + i;
				return;
			}
			
			town.creatures[i].number = c_to_create;
			town.creatures[i].start_loc = create_loc;
			
			town.creatures[i].start_attitude = 
				scen_data.scen_creatures[c_to_create].default_attitude;
			if (town.creatures[i].start_attitude < 2)
				town.creatures[i].start_attitude = 2;
			town.creatures[i].character_id = cur_town * 100 + i;
			
			return;
		}	
	}
}

// if i_to_make is not null, copies all of the info from this structure
// returns TRUE is tried to place in an impossible spot, or item was placed
// returns false is item not palced because too many
Boolean create_new_item(short item_to_create,location create_loc,Boolean property,item_type *i_to_make)
{
	short i;
	
	//if (!strcmp(scen_data.scen_items[item_to_create].name,"Unused"))
	//	return TRUE;
	if (item_to_create < 0)	
		return TRUE;
	if (loc_in_active_area(create_loc) == FALSE) {
		give_error("You can't place an item here. This space is outside of the active town area.","",0);
		return TRUE;
	}
	
	for (i = 0; i < NUM_TOWN_PLACED_ITEMS; i++){
		if (town.preset_items[i].exists() == FALSE) {
			selected_item_number = 11000 + i;
			if (i_to_make != NULL) {
				town.preset_items[i] = *i_to_make;
				town.preset_items[i].item_loc = create_loc;
				shift_item_locs(create_loc);
				return TRUE;
			}
			town.preset_items[i].which_item = item_to_create;
			town.preset_items[i].item_loc = create_loc;
			town.preset_items[i].charges = scen_data.scen_items[item_to_create].charges;
			
			town.preset_items[i].properties = 0;
			if (property)
				town.preset_items[i].properties += 2;
			
			//set_all_items_containment();
			town.preset_items[i].properties |= (( is_crate(town.preset_items[i].item_loc.x,town.preset_items[i].item_loc.y)) || 
												(is_barrel(town.preset_items[i].item_loc.x,town.preset_items[i].item_loc.y)) || 
												(scen_data.scen_ter_types[t_d.terrain[(int)town.preset_items[i].item_loc.x][(int)town.preset_items[i].item_loc.y]].special==40))<<2;
			shift_item_locs(create_loc);
			
			return TRUE;
		}
	}
	return FALSE;
}

Boolean create_new_ter_script(char *ter_script_name,location create_loc,in_town_on_ter_script_type *script_to_make)
{
	short i;
	
	if (strlen(ter_script_name) <= 0)	
		return TRUE;
	if (strlen(ter_script_name) >= SCRIPT_NAME_LEN)	
		return TRUE;
	if (loc_in_active_area(create_loc) == FALSE) {
		give_error("You can't place a terrain script here. This space is outside of the active town area.","",0);
		return TRUE;
	}
	
	for (i = 0; i < NUM_TER_SCRIPTS; i++){
		if ((town.ter_scripts[i].exists) && (same_point(town.ter_scripts[i].loc,create_loc)))  {
			give_error("You can't make or paste a terrain script on a spot which already has a terrain script.","If you want to edit this terrain script, use the Select/Edit Placed Object button",0);
		  	return TRUE;
		}	
	}
	for (i = 0; i < NUM_TER_SCRIPTS; i++){
		if (town.ter_scripts[i].exists == FALSE) {
			town.ter_scripts[i].clear_in_town_on_ter_script_type();
			
			selected_item_number = 9000 + i;
			if (script_to_make != NULL) {
				town.ter_scripts[i] = *script_to_make;
				town.ter_scripts[i].loc = create_loc;
				return TRUE;
			}
			town.ter_scripts[i].exists = TRUE;
			strcpy(town.ter_scripts[i].script_name,ter_script_name);
			town.ter_scripts[i].loc = create_loc;
			
			return TRUE;
		}	
	}
	give_error("You can only have 100 active terrain scripts in a zone. No terrain script has been created.","",0);
	return FALSE;
}

void shift_item_locs(location spot_hit)
{
	short i;
	short num_items_there = 0,cur_item_pos = 0;
	location offsets[4][4] = {{{0,0},{0,0},{0,0},{0,0}},
					{{-4,-2},{4,2},{0,0},{0,0}},
					{{-4,-2},{4,-2},{-4,2},{0,0}},
					{{-4,-2},{4,-2},{-4,2},{4,2}}};

	for (i = 0; i < NUM_TOWN_PLACED_ITEMS; i++)
		if ((town.preset_items[i].exists()) && (same_point(town.preset_items[i].item_loc,spot_hit))) 
			num_items_there++;
	for (i = 0; i < NUM_TOWN_PLACED_ITEMS; i++)
		if ((town.preset_items[i].exists()) && (same_point(town.preset_items[i].item_loc,spot_hit))) {
			town.preset_items[i].item_shift = offsets[smin(num_items_there - 1,3)][cur_item_pos % 4];
			cur_item_pos++;
			}
	

}

void place_items_in_town()
{
	location l;
	short i,j,k,x;
	Boolean place_failed = FALSE;
	
	for (i=town.in_town_rect.left; i < town.in_town_rect.right;i++){
		for (j = town.in_town_rect.top; j < town.in_town_rect.bottom;j++) {
			l.x = i; l.y = j;
			
			for (k = 0; k < 10; k++){
				if (t_d.terrain[i][j] == scenario.storage_shortcuts[k].ter_type) {
					for (x = 0; x < 10; x++){
						if (create_new_item(scenario.storage_shortcuts[k].item_num[x],l,(Boolean)scenario.storage_shortcuts[k].property,NULL) == FALSE)
							place_failed = TRUE;
					}
				}
			}
		}
	}
	if (place_failed == TRUE)
		give_error("Not all of the random items could be placed. The preset item per town limit of 144 was reached.","",0);
	draw_terrain();
}

void create_town_entry(RECT rect_hit)
{
	short x,y;

	for (x = 0; x < 8; x++)
		if ((current_terrain.exit_dests[x] < 0) || (current_terrain.exit_rects[x].right <= 0)) {
			y = get_a_number(856,0,0,scenario.num_towns - 1);
			if (cre(y,0,scenario.num_towns - 1,"The town number you gave was out of range. It must be from 0 to the number of towns in the scenario - 1.","",0) == TRUE) return;
			if (y >= 0) {
				current_terrain.exit_dests[x] = y;
				current_terrain.exit_rects[x].top = (short)rect_hit.top;
				current_terrain.exit_rects[x].left = (short)rect_hit.left;
				current_terrain.exit_rects[x].bottom = (short)rect_hit.bottom;
				current_terrain.exit_rects[x].right = (short)rect_hit.right;
				}
			return;
			}
		give_error("You can't set more than 8 town entrances in any outdoor section.","",0);

}

void edit_town_entry(location spot_hit)
{
	short x,y;
	
	for (x = 0; x < 8; x++){
		if ((current_terrain.exit_dests[x] >= 0) && (loc_touches_rect(spot_hit,current_terrain.exit_rects[x]))) {
			y = get_a_number(856,current_terrain.exit_dests[x],0,scenario.num_towns - 1);
			if (cre(y,-1,scenario.num_towns - 1,"The town number you gave was out of range. It must be from 0 to the number of towns in the scenario - 1. You can also enter -1 to delete this town.","",0) == TRUE) return;
			if (y < 0) {
				current_terrain.exit_dests[x] = -1;
				current_terrain.exit_rects[x].right = 0;							
			}
			else current_terrain.exit_dests[x] = y;
			return;
		}
	}
}

void set_rect_height(RECT r)
{
	short i,j,h;
	
	h = get_a_number(872,0,0,99);
	if ((h < 1) || (h > 100))
		return;
	for (i = (short)r.left; i < (short)r.right + 1; i++){
		for (j = (short)r.top; j < (short)r.bottom + 1; j++) {
			if (editing_town == TRUE){
				appendChangeToLatestStep(new drawingChange(i,j,h,t_d.height[i][j],3));
				t_d.height[i][j] = (unsigned char)h;
			}
			else{
				appendChangeToLatestStep(new drawingChange(i,j,h,current_terrain.height[i][j],3));
				current_terrain.height[i][j] = (unsigned char)h;
			}
		}
	}
}

void add_rect_height(RECT r)
{
	short i,j,h, newh;
	
	h = get_a_number(880,0,-99,99);
	if ((h < -100) || (h > 100))
		return;
	for (i = r.left; i < r.right + 1; i++){
		for (j = r.top; j < r.bottom + 1; j++) {
			if (editing_town){
				newh = t_d.height[i][j] + h;
				if(newh<0)
					newh=0;
				else if(newh>99)
					newh=99;
				appendChangeToLatestStep(new drawingChange(i,j,newh,t_d.height[i][j],3));
				t_d.height[i][j] = newh;
			}
			else if(!editing_town){
				newh = current_terrain.height[i][j] + h;
				if(newh<0)
					newh=0;
				else if(newh>99)
					newh=99;
				appendChangeToLatestStep(new drawingChange(i,j,newh,current_terrain.height[i][j],3));
				current_terrain.height[i][j] = newh;
			}
		}
	}
}

// put a bit of terrain down without doing redrawing or anything odd and without drawing anything
void shy_put_terrain(short i,short j,short ter)
{
	if (editing_town == TRUE) {
		if (t_d.terrain[i][j] == 0)
			t_d.terrain[i][j] = ter;
		return;
	}
	if (current_terrain.terrain[i][j] == 0)
		current_terrain.terrain[i][j] = ter;
}

// swaps walls of type 1 to type 2 and vise versa
void transform_walls(RECT working_rect)
{
	short i,j;
	short ter,new_ter;

	for (i = (short)working_rect.left; i <= (short)working_rect.right; i++){
		for (j = (short)working_rect.top; j <= (short)working_rect.bottom; j++) {
		
			new_ter = ter = (editing_town == TRUE) ? t_d.terrain[i][j] : current_terrain.terrain[i][j];
			
			if ((ter >= 2) && (ter <= 37))
				new_ter = ter + 36;
			else if ((ter >= 38) && (ter <= 73))
				new_ter = ter - 36;
			
			if (editing_town == TRUE)
				t_d.terrain[i][j] = new_ter;
			else 
				current_terrain.terrain[i][j] = new_ter;
		}
	}
}

Boolean is_not_darkness_floor(short i,short j) 
{
	unsigned char floor = (editing_town == TRUE) ? t_d.floor[i][j] : current_terrain.floor[i][j];
	return(floor!=255);
}

// places walls of wall type 1 surrounding blackness floor (floor 255)
void place_bounding_walls(RECT working_rect)
{
	short i,j;
	Boolean ft[4]; // means: wallable floor there
	short ter,new_ter;
	unsigned char floor;
	
	if (working_rect.left == 0)
		working_rect.left = 1;
	if (working_rect.top == 0)
		working_rect.top = 1;
	if (working_rect.right == ((editing_town == TRUE) ? max_dim[town_type] : 48) - 1)
		working_rect.right = ((editing_town == TRUE) ? max_dim[town_type] : 48) - 2;
	if (working_rect.bottom == ((editing_town == TRUE) ? max_dim[town_type] : 48) - 1)
		working_rect.bottom = ((editing_town == TRUE) ? max_dim[town_type] : 48) - 2;
	
	for (i = (short)working_rect.left; i <= (short)working_rect.right; i++)
		for (j = (short)working_rect.top;j <= (short)working_rect.bottom; j++) {
			ter = (editing_town == TRUE) ? t_d.terrain[i][j] : current_terrain.terrain[i][j];
			floor = (editing_town == TRUE) ? t_d.floor[i][j] : current_terrain.floor[i][j];
			if ((floor == 255) && (ter == 0)) {
				ft[0] = ft[1] = ft[2] = ft[3] = FALSE;
				
				if (is_not_darkness_floor(i,j-1))
					ft[0] = TRUE;
				if (is_not_darkness_floor(i - 1,j))
					ft[1] = TRUE;
				if (is_not_darkness_floor(i,j+1))
					ft[2] = TRUE;
				if (is_not_darkness_floor(i + 1,j))
					ft[3] = TRUE;
				
				new_ter = (editing_town == TRUE) ? t_d.terrain[i][j] : current_terrain.terrain[i][j];
				if ((ft[0] == TRUE) && (ft[1] == TRUE)) {
					new_ter = 6;
					if (ft[2] == TRUE) 
						shy_put_terrain(i,j + 1,2);
					if (ft[3] == TRUE) 
						shy_put_terrain(i + 1,j,3);
				}
				else if ((ft[2] == TRUE) && (ft[1] == TRUE)) {
					new_ter = 7;
					if (ft[3] == TRUE) 
						shy_put_terrain(i + 1,j,3);					
				}
				else if ((ft[2] == TRUE) && (ft[3] == TRUE))
					new_ter = 8;
				else if ((ft[0] == TRUE) && (ft[3] == TRUE))
					new_ter = 9;
				else if (ft[0] == TRUE) {
					new_ter = 2;
					if (ft[2] == TRUE) 
						shy_put_terrain(i,j + 1,2);					
				}
				else if (ft[1] == TRUE) {
					new_ter = 3;
					if (ft[3] == TRUE) 
						shy_put_terrain(i + 1,j,3);					
				}
				else if (ft[2] == TRUE) {
					new_ter = 4;
				}
				else if (ft[3] == TRUE) {
					new_ter = 5;
				}
				if(new_ter!=ter)
					appendChangeToLatestStep(new drawingChange(i,j,new_ter,ter,2));
				if (editing_town)
					t_d.terrain[i][j] = new_ter;
				else
					current_terrain.terrain[i][j] = new_ter;
			}
		}
}

Boolean is_wall(short x, short y)
{
	if (editing_town == TRUE) {
		if ((t_d.terrain[x][y] >= 2) && (t_d.terrain[x][y] <= 73))
			return TRUE;
	}
	if (editing_town == FALSE) {
		if ((current_terrain.terrain[x][y] >= 2) && (current_terrain.terrain[x][y] <= 73))
			return TRUE;
	}
	return FALSE;
}

void clean_walls()
{
	short i,j;
	
	if (editing_town == TRUE) {
		for (i = 1; i < max_dim[town_type] - 1; i++){
			for (j = 1; j < max_dim[town_type] - 1; j++) {
				if ((t_d.terrain[i][j] == 6) && (is_dumb_terrain(t_d.terrain[i][j - 1])) && (is_wall(i + 1,j) == FALSE)) {
					t_d.terrain[i][j] = 3; t_d.terrain[i][j - 1] = 4;
				}
				if ((t_d.terrain[i][j] == 6) && (is_dumb_terrain(t_d.terrain[i - 1][j])) && (is_wall(i,j + 1) == FALSE)) {
					t_d.terrain[i][j] = 2; t_d.terrain[i - 1][j] = 5;
				}
				if ((t_d.terrain[i][j] == 7) && (is_dumb_terrain(t_d.terrain[i][j + 1])) && (is_wall(i + 1,j) == FALSE)) {
					t_d.terrain[i][j] = 3; t_d.terrain[i][j + 1] = 2;
				}
				if ((t_d.terrain[i][j] == 7) && (is_dumb_terrain(t_d.terrain[i - 1][j])) && (is_wall(i,j - 1) == FALSE)) {
					t_d.terrain[i][j] = 4; t_d.terrain[i - 1][j] = 5;
				}
				if ((t_d.terrain[i][j] == 8) && (is_dumb_terrain(t_d.terrain[i][j + 1])) && (is_wall(i - 1,j ) == FALSE)) {
					t_d.terrain[i][j] = 5; t_d.terrain[i][j + 1] = 2;
				}
				if ((t_d.terrain[i][j] == 8) && (is_dumb_terrain(t_d.terrain[i + 1][j])) && (is_wall(i,j - 1) == FALSE)) {
					t_d.terrain[i][j] = 4; t_d.terrain[i + 1][j] = 3;
				}
				if ((t_d.terrain[i][j] == 9) && (is_dumb_terrain(t_d.terrain[i][j - 1])) && (is_wall(i - 1,j) == FALSE)) {
					t_d.terrain[i][j] = 5; t_d.terrain[i][j - 1] = 4;
				}
				if ((t_d.terrain[i][j] == 9) && (is_dumb_terrain(t_d.terrain[i + 1][j])) && (is_wall(i,j + 1) == FALSE)) {
					t_d.terrain[i][j] = 2; t_d.terrain[i + 1][j] = 3;
				}				
				if ((t_d.terrain[i][j] == 42) && (is_dumb_terrain(t_d.terrain[i][j - 1])) && (is_wall(i + 1,j) == FALSE)) {
					t_d.terrain[i][j] = 3 + 36; t_d.terrain[i][j - 1] = 4 + 36;
				}
				if ((t_d.terrain[i][j] == 42) && (is_dumb_terrain(t_d.terrain[i - 1][j])) && (is_wall(i,j + 1) == FALSE)) {
					t_d.terrain[i][j] = 2 + 36; t_d.terrain[i - 1][j] = 5 + 36;
				}
				if ((t_d.terrain[i][j] == 43) && (is_dumb_terrain(t_d.terrain[i][j + 1])) && (is_wall(i + 1,j) == FALSE)) {
					t_d.terrain[i][j] = 3 + 36; t_d.terrain[i][j + 1] = 2 + 36;
				}
				if ((t_d.terrain[i][j] == 43) && (is_dumb_terrain(t_d.terrain[i - 1][j])) && (is_wall(i,j - 1) == FALSE)) {
					t_d.terrain[i][j] = 4 + 36; t_d.terrain[i - 1][j] = 5 + 36;
				}
				if ((t_d.terrain[i][j] == 44) && (is_dumb_terrain(t_d.terrain[i][j + 1])) && (is_wall(i - 1,j) == FALSE)) {
					t_d.terrain[i][j] = 41; t_d.terrain[i][j + 1] = 38;
				}
				if ((t_d.terrain[i][j] == 44) && (is_dumb_terrain(t_d.terrain[i + 1][j])) && (is_wall(i,j - 1) == FALSE)) {
					t_d.terrain[i][j] = 40; t_d.terrain[i + 1][j] = 39;
				}
				if ((t_d.terrain[i][j] == 45) && (is_dumb_terrain(t_d.terrain[i][j - 1])) && (is_wall(i - 1,j) == FALSE)) {
					t_d.terrain[i][j] = 5 + 36; t_d.terrain[i][j - 1] = 4 + 36;
				}
				if ((t_d.terrain[i][j] == 45) && (is_dumb_terrain(t_d.terrain[i + 1][j])) && (is_wall(i,j + 1) == FALSE)) {
					t_d.terrain[i][j] = 2 + 36; t_d.terrain[i + 1][j] = 3 + 36;
				}		
			}
		}
	}
	if (editing_town == FALSE) {
		for (i = 1; i < 47; i++){
			for (j = 1; j < 47; j++) {
				if ((current_terrain.terrain[i][j] == 6) && (is_dumb_terrain(current_terrain.terrain[i][j - 1])) && (is_wall(i + 1,j) == FALSE)) {
					current_terrain.terrain[i][j] = 3; current_terrain.terrain[i][j - 1] = 4;
				}
				if ((current_terrain.terrain[i][j] == 6) && (is_dumb_terrain(current_terrain.terrain[i - 1][j])) && (is_wall(i,j + 1) == FALSE)) {
					current_terrain.terrain[i][j] = 2; current_terrain.terrain[i - 1][j] = 5;
				}
				if ((current_terrain.terrain[i][j] == 7) && (is_dumb_terrain(current_terrain.terrain[i][j + 1])) && (is_wall(i + 1,j) == FALSE)) {
					current_terrain.terrain[i][j] = 3; current_terrain.terrain[i][j + 1] = 2;
				}
				if ((current_terrain.terrain[i][j] == 7) && (is_dumb_terrain(current_terrain.terrain[i - 1][j])) && (is_wall(i,j - 1) == FALSE)) {
					current_terrain.terrain[i][j] = 4; current_terrain.terrain[i - 1][j] = 5;
				}
				if ((current_terrain.terrain[i][j] == 8) && (is_dumb_terrain(current_terrain.terrain[i][j + 1])) && (is_wall(i - 1,j) == FALSE)) {
					current_terrain.terrain[i][j] = 5; current_terrain.terrain[i][j + 1] = 2;
				}
				if ((current_terrain.terrain[i][j] == 8) && (is_dumb_terrain(current_terrain.terrain[i + 1][j])) && (is_wall(i,j - 1) == FALSE)) {
					current_terrain.terrain[i][j] = 4; current_terrain.terrain[i + 1][j] = 3;
				}
				if ((current_terrain.terrain[i][j] == 9) && (is_dumb_terrain(current_terrain.terrain[i][j - 1])) && (is_wall(i - 1,j) == FALSE)) {
					current_terrain.terrain[i][j] = 5; current_terrain.terrain[i][j - 1] = 4;
				}
				if ((current_terrain.terrain[i][j] == 9) && (is_dumb_terrain(current_terrain.terrain[i + 1][j])) && (is_wall(i,j + 1) == FALSE)) {
					current_terrain.terrain[i][j] = 2; current_terrain.terrain[i + 1][j] = 3;
				}
				if ((current_terrain.terrain[i][j] == 42) && (is_dumb_terrain(current_terrain.terrain[i][j - 1])) && (is_wall(i + 1,j) == FALSE)) {
					current_terrain.terrain[i][j] = 3 + 36; current_terrain.terrain[i][j - 1] = 4 + 36;
				}
				if ((current_terrain.terrain[i][j] == 42) && (is_dumb_terrain(current_terrain.terrain[i - 1][j])) && (is_wall(i,j + 1) == FALSE)) {
					current_terrain.terrain[i][j] = 2 + 36; current_terrain.terrain[i - 1][j] = 5 + 36;
				}
				if ((current_terrain.terrain[i][j] == 43) && (is_dumb_terrain(current_terrain.terrain[i][j + 1])) && (is_wall(i + 1,j) == FALSE)) {
					current_terrain.terrain[i][j] = 3 + 36; current_terrain.terrain[i][j + 1] = 2 + 36;
				}
				if ((current_terrain.terrain[i][j] == 43) && (is_dumb_terrain(current_terrain.terrain[i - 1][j])) && (is_wall(i,j - 1) == FALSE)) {
					current_terrain.terrain[i][j] = 4 + 36; current_terrain.terrain[i - 1][j] = 5 + 36;
				}
				if ((current_terrain.terrain[i][j] == 44) && (is_dumb_terrain(current_terrain.terrain[i][j + 1])) && (is_wall(i - 1,j) == FALSE)) {
					current_terrain.terrain[i][j] = 41; current_terrain.terrain[i][j + 1] = 38;
				}
				if ((current_terrain.terrain[i][j] == 44) && (is_dumb_terrain(current_terrain.terrain[i + 1][j])) && (is_wall(i,j - 1) == FALSE)) {
					current_terrain.terrain[i][j] = 40; current_terrain.terrain[i + 1][j] = 39;
				}
				if ((current_terrain.terrain[i][j] == 45) && (is_dumb_terrain(current_terrain.terrain[i][j - 1])) && (is_wall(i - 1,j) == FALSE)) {
					current_terrain.terrain[i][j] = 5 + 36; current_terrain.terrain[i][j - 1] = 4 + 36;
				}
				if ((current_terrain.terrain[i][j] == 45) && (is_dumb_terrain(current_terrain.terrain[i + 1][j])) && (is_wall(i,j + 1) == FALSE)) {
					current_terrain.terrain[i][j] = 2 + 36; current_terrain.terrain[i + 1][j] = 3 + 36;
				}
			}
		}
	}
}

// returns true if it's a ter which can be easily rubbed out
Boolean is_dumb_terrain(short ter)
{
	return(!ter);
}

// Goes to all items and sets whether they are currently contained in an object or not
void set_all_items_containment()
{
	for (short i = 0; i < NUM_TOWN_PLACED_ITEMS; i++)
		if (town.preset_items[i].which_item >= 0) {
			short ter = t_d.terrain[ (UCHAR) town.preset_items[i].item_loc.x][ (UCHAR) town.preset_items[i].item_loc.y];
//			town.preset_items[i].properties = town.preset_items[i].properties & 251;
			
			if (( is_crate(town.preset_items[i].item_loc.x,town.preset_items[i].item_loc.y)) 
			  || (is_barrel(town.preset_items[i].item_loc.x,town.preset_items[i].item_loc.y))
			  || (scen_data.scen_ter_types[ter].special == 40));
//				town.preset_items[i].properties = town.preset_items[i].properties | 4;
            else 
                town.preset_items[i].properties = town.preset_items[i].properties & 251;
		} 
}

void set_up_lights()
{
	short i,j,rad;
	location where,l;
	short where_lit[64][64];
	Boolean store_edit;
	terrain_type_type ter;
	
	store_edit =  editing_town;
	editing_town = TRUE;
	
	// Find bonfires, braziers, etc.
	for (i = 0; i < 64; i++)
		for (j = 0; j < 64; j++)
			where_lit[i][j] = 0;
			
	for (i = 0; i < max_dim[town_type]; i++)
		for (j = 0; j < max_dim[town_type]; j++) {
			l.x = (t_coord)i; l.y = (t_coord)j;
			ter = get_ter(t_d.terrain[i][j]);
			rad = smax(ter.light_radius,
					scen_data.scen_floors[t_d.floor[i][j]].light_radius);
			if (rad > 0) {
				for (where.x = (t_coord)smax(0,i - rad); where.x < (t_coord)smin(max_dim[town_type],i + rad + 1); where.x++)
					for (where.y = (t_coord)smax(0,j - rad); where.y < (t_coord)smin(max_dim[town_type],j + rad + 1); where.y++)
// q_3DModStart
						if (/*(where_lit[where.x][where.y] == 0) && */(dist(where,l) <= rad) && (old_can_see_in(l,where, /* 0,*/ 0) == TRUE)) {
// q_3DModEnd

							where_lit[where.x][where.y] = where_lit[where.x][where.y] + rad - dist(where,l) + 1;
							where_lit[where.x][where.y] = smin(8,where_lit[where.x][where.y]);
							}
				}
			}
	for (i = 0; i < 64; i++)	
		for (j = 0; j < 64; j++) {
			t_d.lighting[i][j] = (where_lit[i][j] >= 0) ? (unsigned char)where_lit[i][j]: 0;
			}
	editing_town = store_edit;
}

// q_3DModStart
Boolean old_can_see_in(location p1,location p2, /* short check_light,*/ short check_travel)
// q_3DModEnd
{
	Boolean a,b;
	
// q_3DModStart
	old_can_see(p1,p2, /*check_light,*/ check_travel,&a,&b);
// q_3DModEnd
	return b;
}
// returns TRUE if you can see p2 from p1
// check_light - returns FALSE if p2 is in darkness
// check travel - returns FALSE if path is obstructed by, say, water or a wall w. window

// q_3DModStart
void old_can_see(location p1,location p2,/* short check_light,*/ short check_travel,Boolean *see_to,Boolean *see_in)
// q_3DModEnd
{
	short t;
	short dx,dy;
	short x_dir = 2,y_dir = 0,x_back_dir = 6,y_back_dir = 4,num_shifts = 0;
	// dirs 0 - n 1 - nw 2 - w and so on to 7 - ne
	location shift_dir = {0,0},empty_loc = {0,0};
	location cur_loc,next_loc;
	Boolean store_to = FALSE,store_in = FALSE;
	
	*see_to = TRUE;
	*see_in = TRUE;
	if (same_point(p1,p2))
		return;
	dx = abs(p2.x - p1.x);
	dy = abs(p2.y - p1.y);
	shift_dir.x = (p2.x > p1.x) ? 1 : -1;
	shift_dir.y = (p2.y > p1.y) ? 1 : -1;
	if (p2.x > p1.x) {
		x_dir = 6; x_back_dir = 2;
	}
	if (p2.y > p1.y) {
		y_dir = 4; y_back_dir = 0;
	}
	cur_loc = p1;
	
	if (dx >= dy) {
		y_dir = locs_to_dir(empty_loc,shift_dir);
		y_back_dir = (y_dir + 4) % 8;
		//if ((p1.x - p2.x == 1) && (p1.y - p2.y == 1))
		//	print_nums(99,y_dir,y_back_dir);
		for (t = 0; t < dx; t++) {
			if (((t + 1) * dy) / dx > num_shifts) {
				num_shifts++;// = (t * dy) / dx;
				next_loc = cur_loc;
				//next_loc.x += shift_dir.x;
				next_loc.y += shift_dir.y;
				if (look_block(cur_loc,y_dir) == TRUE) 
					*see_to = FALSE;
				if (look_block(next_loc,y_back_dir) == TRUE)
					*see_in = FALSE;
				//if ((p1.x - p2.x == 1) && (p1.y - p2.y == 1)) {
				//	print_nums(999,look_block(cur_loc,y_dir),look_block(next_loc,y_back_dir));
				//	print_nums(next_loc.x,next_loc.y,y_back_dir);	
				//	}
				if ((check_travel == TRUE) &&
					(move_block(cur_loc,y_dir) == TRUE))
					*see_to = FALSE;
				if ((check_travel == TRUE) &&
					(move_block(next_loc,y_back_dir) == TRUE))
					*see_in = FALSE;
				cur_loc = next_loc;
			}
			//else {
			next_loc = cur_loc;
			next_loc.x += shift_dir.x;
			if (look_block(cur_loc,x_dir) == TRUE) 
				*see_to = FALSE;
			if (look_block(next_loc,x_back_dir) == TRUE)
				*see_in = FALSE;
			if ((check_travel == TRUE) &&
				(move_block(cur_loc,x_dir) == TRUE))
				*see_to = FALSE;
			if ((check_travel == TRUE) &&
				(move_block(next_loc,x_back_dir) == TRUE))
				*see_in = FALSE;
			cur_loc = next_loc;
			//	}
			if (same_point(cur_loc,p2)) {
				if (*see_to == FALSE)
					*see_in = FALSE;
				t = 10000;
				return;
				}
				else if ((*see_in == FALSE) || (*see_to == FALSE)) {
					*see_in = *see_to = FALSE;
					t = 10000;
					return;
					}
			}
		store_to = *see_to;
		store_in = *see_in;
	}
	num_shifts = 0;
	if (dy > dx) {
		x_dir = locs_to_dir(empty_loc,shift_dir);
		x_back_dir = (x_dir + 4) % 8;
		for (t = 0; t < dy; t++) {
			if (((t + 1) * dx) / dy > num_shifts) {
				num_shifts++;// = (t * dx) / dy;
				next_loc = cur_loc;
				next_loc.x += shift_dir.x;
				//next_loc.y += shift_dir.y;
				if (look_block(cur_loc,x_dir) == TRUE) 
					*see_to = FALSE;
				if (look_block(next_loc,x_back_dir) == TRUE)
					*see_in = FALSE;
				if ((check_travel == TRUE) &&
					(move_block(cur_loc,x_dir) == TRUE))
					*see_to = FALSE;
				if ((check_travel == TRUE) &&
					(move_block(next_loc,x_back_dir) == TRUE))
					*see_in = FALSE;
				cur_loc = next_loc;
			}
			//else {
			next_loc = cur_loc;
			next_loc.y += shift_dir.y;
			if (look_block(cur_loc,y_dir) == TRUE) 
				*see_to = FALSE;
			if (look_block(next_loc,y_back_dir) == TRUE)
				*see_in = FALSE;
			if ((check_travel == TRUE) &&
				(move_block(cur_loc,y_dir) == TRUE))
				*see_to = FALSE;
			if ((check_travel == TRUE) &&
				(move_block(next_loc,y_back_dir) == TRUE))
				*see_in = FALSE;
			cur_loc = next_loc;
			//	}
			
			if (same_point(cur_loc,p2)) {
				if (*see_to == FALSE)
					*see_in = FALSE;
				t = 10000;
				}
				else if ((*see_in == FALSE) || (*see_to == FALSE)) {
					*see_in = *see_to = FALSE;
					t = 10000;
					}
			}
		}
}

// q_3DModStart
/*
Boolean can_see_to(location p1,location p2,short check_light,short check_travel)
{
	Boolean a,b;
	
	can_see(p1,p2,check_light,check_travel,&a,&b);
	return a;
}
*/

Boolean can_see_in(location p1,location p2,short check_light,short check_travel)
{
	Boolean a,b;
	
	can_see(p1,p2,check_light,check_travel,&a,&b);
	return b;
}

// returns TRUE if you can see p2 from p1
// check_light - returns FALSE if p2 is in darkness
// check travel - returns FALSE if path is obstructed by, say, water or a wall w. window
void can_see(location p1,location p2,short check_light,short check_travel,Boolean *see_to,Boolean *see_in)
{
	short t;
	short dx,dy;
	short x_dir = 2,y_dir = 0,x_back_dir = 6,y_back_dir = 4,num_shifts = 0;
	// dirs 0 - n 1 - nw 2 - w and so on to 7 - ne
	location shift_dir = {0,0},empty_loc = {0,0};
	location cur_loc,next_loc;
	
	*see_to = TRUE;
	*see_in = TRUE;
	if (same_point(p1,p2))
		return;
	dx = abs(p2.x - p1.x);
	dy = abs(p2.y - p1.y);
	shift_dir.x = (p2.x > p1.x) ? 1 : -1;
	shift_dir.y = (p2.y > p1.y) ? 1 : -1;
	if (p2.x > p1.x) {
		x_dir = 6; x_back_dir = 2;
	}
	if (p2.y > p1.y) {
		y_dir = 4; y_back_dir = 0;
	}
	cur_loc = p1;
	
	if (dx >= dy) {
		y_dir = locs_to_dir(empty_loc,shift_dir);
		y_back_dir = (y_dir + 4) % 8;
		//if ((p1.x - p2.x == 1) && (p1.y - p2.y == 1))
		//	print_nums(99,y_dir,y_back_dir);
		for (t = 0; t < dx; t++) {
			next_loc = cur_loc;
			if (((t + 1) * dy) / dx > num_shifts) {
				num_shifts++;// = (t * dy) / dx;
							 //next_loc.x += shift_dir.x;
				next_loc.y += shift_dir.y;
			}
			//else {
			next_loc.x += shift_dir.x;
			//	}
			
			if(can_see_single(cur_loc,next_loc,check_light,check_travel,see_to) == FALSE) {
				*see_in = FALSE;
				*see_to &= (t + 1 >= dx);
				return;
			}
			cur_loc = next_loc;
		}
		return;
	}
	num_shifts = 0;
	if (dy > dx) {
		x_dir = locs_to_dir(empty_loc,shift_dir);
		x_back_dir = (x_dir + 4) % 8;
		for (t = 0; t < dy; t++) {
			next_loc = cur_loc;
			if (((t + 1) * dx) / dy > num_shifts) {
				num_shifts++;// = (t * dx) / dy;
				next_loc.x += shift_dir.x;
				//next_loc.y += shift_dir.y;
			}
			//else {
			next_loc.y += shift_dir.y;
			//	}
			if(can_see_single(cur_loc,next_loc,check_light,check_travel,see_to) == FALSE) {
				*see_in = FALSE;
				*see_to &= (t + 1 >= dy);
				return;
			}
			cur_loc = next_loc;
		}
		return;
	}
}

Boolean can_see_single(location p1,location p2,short check_light,short check_travel,Boolean *see_to)
{
	
	location shift_dir = {0,0},shift_dir_x = {0,0},shift_dir_y = {0,0},empty_loc = {0,0};
	location test_loc_x_y_forwards, test_loc_y_x_forwards;
	short x_dir, y_dir, x_back_dir, y_back_dir;
	if(p1.x != p2.x) {
		shift_dir_x.x = shift_dir.x = (p2.x > p1.x) ? 1 : -1;
	}
	if(p1.y != p2.y) {
		shift_dir_y.y = shift_dir.y = (p2.y > p1.y) ? 1 : -1;
	}
	
	x_dir = locs_to_dir(empty_loc,shift_dir_x);
	y_dir = locs_to_dir(empty_loc,shift_dir_y);
	x_back_dir = (x_dir + 4) % 8;
	y_back_dir = (y_dir + 4) % 8;
	
	if(p1.x != p2.x && p1.y != p2.y) {
		Boolean x_no_block00 = FALSE;
		Boolean x_no_block01 = FALSE;
		Boolean x_no_block10 = FALSE;
		Boolean x_no_block11 = FALSE;
		Boolean y_no_block00 = FALSE;
		Boolean y_no_block01 = FALSE;
		Boolean y_no_block10 = FALSE;
		Boolean y_no_block11 = FALSE;
		
		test_loc_x_y_forwards = test_loc_y_x_forwards = p1;
		test_loc_x_y_forwards.x += shift_dir.x;
		test_loc_y_x_forwards.y += shift_dir.y;
		
		// [          0,0:p1         ] [1,0:test_loc_x_y_forwards]
		// [0,1:test_loc_y_x_forwards] [          1,1:p2         ]
		//
		// of course, this is just an example.  it can be in any orientation
		
		Boolean corner2 = FALSE;
		
		if(shift_dir.x == 1 && shift_dir.y == 1) {
			if(get_se_corner(0,0,p1.x,p1.y)) {
				*see_to = FALSE;
				return FALSE;
			}
			else if(get_nw_corner(0,0,p2.x,p2.y)) {
				corner2 = TRUE;
			}
		}
		else if(shift_dir.x == -1 && shift_dir.y == 1) {
			if(get_sw_corner(0,0,p1.x,p1.y)) {
				*see_to = FALSE;
				return FALSE;
			}
			else if(get_ne_corner(0,0,p2.x,p2.y)) {
				corner2 = TRUE;
			}
		}
		else if(shift_dir.x == -1 && shift_dir.y == -1) {
			if(get_nw_corner(0,0,p1.x,p1.y)) {
				*see_to = FALSE;
				return FALSE;
			}
			else if(get_se_corner(0,0,p2.x,p2.y)) {
				corner2 = TRUE;
			}
		}
		else if(shift_dir.x == 1 && shift_dir.y == -1) {
			if(get_ne_corner(0,0,p1.x,p1.y)) {
				*see_to = FALSE;
				return FALSE;
			}
			else if(get_sw_corner(0,0,p2.x,p2.y)) {
				corner2 = TRUE;
			}
		}
		if(corner2 == FALSE) {
			x_no_block00 = no_block(p1,x_dir,check_light,check_travel);
			x_no_block10 = no_block(test_loc_x_y_forwards,x_back_dir,check_light,check_travel);
			y_no_block10 = no_block(test_loc_x_y_forwards,y_dir,check_light,check_travel);
			y_no_block11 = no_block(p2,y_back_dir,check_light,check_travel);
			
			if (x_no_block00 && x_no_block10 && y_no_block10 && y_no_block11) 
				return TRUE;
			
			y_no_block00 = no_block(p1,y_dir,check_light,check_travel);
			y_no_block01 = no_block(test_loc_y_x_forwards,y_back_dir,check_light,check_travel);
			x_no_block01 = no_block(test_loc_y_x_forwards,x_dir,check_light,check_travel);
			x_no_block11 = no_block(p2,x_back_dir,check_light,check_travel);
			
			if (y_no_block00 && y_no_block01 && x_no_block01 && x_no_block11) 
				return TRUE;
			
			//deal with BoA's strangeness
			if ((x_no_block00 && y_no_block00 && x_no_block11 && y_no_block11)
				&& (x_no_block10 || y_no_block01) && (x_no_block01 || y_no_block10))
				return TRUE;
			
			//deal with more of BoA's strangeness - sort of symmetric to the previous 'if'
			if ((x_no_block10 && y_no_block10 && x_no_block01 && y_no_block01) && (x_no_block11 || y_no_block11)
				&& (x_no_block00 || y_no_block00))
				return TRUE;
		}
		
		//ok, at this point, we know we can't see in
		
		//!_ around goal has already been checked for
		
		//I don't understand what's wrong with this, but without it, everything works,
		//and with it, sometimes little corner walls aren't displayed in strange (but not unusual) circumstances
		// --> hey, you forget to initialize x(y)_no_block
		/*
		//L on start
		if(!x_no_block00 && !y_no_block00)
			*see_to = FALSE;
		
		//|_ on start and neighbor
		if((!x_no_block00 && !y_no_block01) || (!y_no_block00 && !x_no_block10))
			*see_to = FALSE;*/
		
		//__ on start and neighbor
		if((!x_no_block00 && !x_no_block01) || (!y_no_block00 && !y_no_block10))
			*see_to = FALSE;
		
		/*
		*see_to = FALSE;
		
		if (
			x_no_block00 && x_no_block10 &&
			y_no_block10
		) *see_to = TRUE;
		
		if (
			y_no_block00 && y_no_block01 &&
			x_no_block01
		) *see_to = TRUE;
		
		//deal with BoA's strangeness
		if (
			(
				x_no_block00 && y_no_block00
			)
			&&
			(
				x_no_block10 || y_no_block01
			)
			&&
			(
				x_no_block01 || y_no_block10
			)
		) *see_to = TRUE;
		
		//deal with more of BoA's strangeness - sort of symmetric to the previous 'if'
		if (
			(
				x_no_block10 && y_no_block10 &&
				x_no_block01 && y_no_block01
			)
			&&
			(
				x_no_block00 || y_no_block00
			)
		) *see_to = TRUE;*/
		
		return FALSE;
	}
	else if(p1.x != p2.x) {
		if(no_block(p1,x_dir,check_light,check_travel) && no_block(p2,x_back_dir,check_light,check_travel)) {
			return TRUE;
		}
		else {
			*see_to = no_block(p1,x_dir,check_light,check_travel);
			return FALSE;
		}
	}
	else if(p1.y != p2.y) {
		if(no_block(p1,y_dir,check_light,check_travel) && no_block(p2,y_back_dir,check_light,check_travel)) {
			return TRUE;
		}
		else {
			*see_to = no_block(p1,y_dir,check_light,check_travel);
			return FALSE;
		}
	}
	else {
		return TRUE;
	}
}

Boolean no_block(location l, short direction,short check_light,short check_travel)
{
	if(check_light)
		if(look_block(l,direction))
			return FALSE;
	if(check_travel)
		if(move_block(l,direction))
			return FALSE;
	return TRUE;
}

// direction 0 - n, 1 - nw, 2 - w, 3 - sw, ..., 7 - ne
/// hey, this should never get a diagonal, so ignore diag. values
Boolean look_block(location l, short direction)
{
	//short ter;
	//terrain_type_type t;
	
	//ter = (editing_town == FALSE) ? border_terrains[(l.x + 48) / 48][(l.y + 48) / 48].terrain[l.x - (((l.x + 48) / 48) * 48 - 48)][l.y - (((l.y + 48) / 48) * 48 - 48)] : t_d.terrain[l.x][l.y];
	//t = get_ter(ter);
	
	switch (direction) {
		case 0: return scen_data.scen_ter_types[(editing_town == FALSE) ? ( ((unsigned char)l.x > 47) ? border_terrains[(l.x + 48) / 48][(l.y + 48) / 48].terrain[l.x - (((l.x + 48) / 48) * 48 - 48)][l.y - (((l.y + 48) / 48) * 48 - 48)] : current_terrain.terrain[l.x][l.y]) : t_d.terrain[l.x][l.y]].see_block[0] ; break;
		case 1: return ((look_block(l,0)) || (look_block(l,2))); break;
		case 2: return scen_data.scen_ter_types[(editing_town == FALSE) ? ( ((unsigned char)l.x > 47) ? border_terrains[(l.x + 48) / 48][(l.y + 48) / 48].terrain[l.x - (((l.x + 48) / 48) * 48 - 48)][l.y - (((l.y + 48) / 48) * 48 - 48)] : current_terrain.terrain[l.x][l.y]) : t_d.terrain[l.x][l.y]].see_block[1] ; break;
		case 3: return ((look_block(l,2)) || (look_block(l,4))); break;
		case 4: return scen_data.scen_ter_types[(editing_town == FALSE) ? ( ((unsigned char)l.x > 47) ? border_terrains[(l.x + 48) / 48][(l.y + 48) / 48].terrain[l.x - (((l.x + 48) / 48) * 48 - 48)][l.y - (((l.y + 48) / 48) * 48 - 48)] : current_terrain.terrain[l.x][l.y]) : t_d.terrain[l.x][l.y]].see_block[2] ; break;
		case 5: return ((look_block(l,4)) || (look_block(l,6))); break;
		case 6: return scen_data.scen_ter_types[(editing_town == FALSE) ? ( ((unsigned char)l.x > 47) ? border_terrains[(l.x + 48) / 48][(l.y + 48) / 48].terrain[l.x - (((l.x + 48) / 48) * 48 - 48)][l.y - (((l.y + 48) / 48) * 48 - 48)] : current_terrain.terrain[l.x][l.y]) : t_d.terrain[l.x][l.y]].see_block[3] ; break;
		case 7: return ((look_block(l,6)) || (look_block(l,0))); break;
	}
	return TRUE;
}
// direction 0 - n, 1 - nw, 2 - w, 3 - sw, ..., 7 - ne
/// hey, this should never get a diagonal, so ignore diag. values
Boolean move_block(location l, short direction)
{
	short ter;
	terrain_type_type t;
	
	ter = (editing_town == FALSE) ? current_terrain.terrain[l.x][l.y] : t_d.terrain[l.x][l.y];
	t = get_ter(ter);
	
	switch (direction) {
		case 0: return t.move_block[0]; break;
		case 1: return ((move_block(l,0)) || (move_block(l,2))); break;
		case 2: return t.move_block[1]; break;
		case 3: return ((move_block(l,2)) || (move_block(l,4))); break;
		case 4: return t.move_block[2]; break;
		case 5: return ((move_block(l,4)) || (move_block(l,6))); break;
		case 6: return t.move_block[3]; break;
		case 7: return ((move_block(l,6)) || (move_block(l,0))); break;
	}
	return TRUE;
}

temp_space_info_ptr corner_and_sight_map_at_loc(short sector_offset_x, short sector_offset_y, short x, short y);
//this must be fast
temp_space_info_ptr corner_and_sight_map_at_loc(short sector_offset_x, short sector_offset_y, short x, short y)
{
	short map_size;
	short sight_reach;

//	if(editing_town) {
//		x += ((sector_offset_x == 1) ? max_dim[town_type] : ((sector_offset_x) ? -max_dim[town_type] : 0)) + 12;
//		y += ((sector_offset_y == 1) ? max_dim[town_type] : ((sector_offset_y) ? -max_dim[town_type] : 0)) + 12;
//	}
//	else {
//		x += ((sector_offset_x == 1) ? 48 : ((sector_offset_x) ? -48 : 0)) + 20;
//		y += ((sector_offset_y == 1) ? 48 : ((sector_offset_y) ? -48 : 0)) + 20;
//	}

	if(editing_town) {
		map_size = max_dim[town_type];	sight_reach = 12;
	} else {
		map_size = 48;					sight_reach = 20;
	}

	x += ((sector_offset_x == 1) ? map_size : ((sector_offset_x) ? -map_size : 0)) + sight_reach;
	y += ((sector_offset_y == 1) ? map_size : ((sector_offset_y) ? -map_size : 0)) + sight_reach;

	if(x < 0 || x > 87 || y < 0 || y > 87)
		return NULL;
	return &corner_and_sight_map[x][y];
}

Boolean get_see_in(short sector_offset_x, short sector_offset_y, short x, short y)
{
	temp_space_info_ptr p;
	
	if(cur_viewing_mode != 11)
		return TRUE;
	
	p = corner_and_sight_map_at_loc(sector_offset_x,sector_offset_y,x,y);
	
	if(p == NULL)
		return FALSE;
	return p->see_in;
}

void set_see_in(short sector_offset_x, short sector_offset_y, short x, short y, Boolean value)
{
	temp_space_info_ptr p;
	
	if(cur_viewing_mode != 11)
		return;
	
	p = corner_and_sight_map_at_loc(sector_offset_x,sector_offset_y,x,y);
	
	if(p == NULL)
		return;
	p->see_in = value;
}

Boolean get_see_to(short sector_offset_x, short sector_offset_y, short x, short y)
{
	temp_space_info_ptr p;
	
	if(cur_viewing_mode != 11)
		return TRUE;
	
	p = corner_and_sight_map_at_loc(sector_offset_x,sector_offset_y,x,y);
	
	if(p == NULL)
		return FALSE;
	return p->see_to;
}

void set_see_to(short sector_offset_x, short sector_offset_y, short x, short y, Boolean value)
{
	temp_space_info_ptr p;
	
	if(cur_viewing_mode != 11)
		return;
	
	p = corner_and_sight_map_at_loc(sector_offset_x,sector_offset_y,x,y);
	
	if(p == NULL)
		return;
	p->see_to = value;
}

short get_nw_corner(short sector_offset_x, short sector_offset_y, short x, short y)
{
	temp_space_info_ptr p = corner_and_sight_map_at_loc(sector_offset_x,sector_offset_y,x,y);
	if(p == NULL)
		return 0;
	
	return p->nw_corner;
}

void set_nw_corner(short sector_offset_x, short sector_offset_y, short x, short y, short value)
{
	temp_space_info_ptr p = corner_and_sight_map_at_loc(sector_offset_x,sector_offset_y,x,y);
	if(p == NULL)
		return;
	p->nw_corner = value;
}

short get_sw_corner(short sector_offset_x, short sector_offset_y, short x, short y)
{
	temp_space_info_ptr p = corner_and_sight_map_at_loc(sector_offset_x,sector_offset_y,x,y);
	if(p == NULL)
		return 0;
	return p->sw_corner;
}

void set_sw_corner(short sector_offset_x, short sector_offset_y, short x, short y, short value)
{
	temp_space_info_ptr p = corner_and_sight_map_at_loc(sector_offset_x,sector_offset_y,x,y);
	if(p == NULL)
		return;
	p->sw_corner = value;
}

short get_se_corner(short sector_offset_x, short sector_offset_y, short x, short y)
{
	temp_space_info_ptr p = corner_and_sight_map_at_loc(sector_offset_x,sector_offset_y,x,y);
	if(p == NULL)
		return 0;
	return p->se_corner;
}

void set_se_corner(short sector_offset_x, short sector_offset_y, short x, short y, short value)
{
	temp_space_info_ptr p = corner_and_sight_map_at_loc(sector_offset_x,sector_offset_y,x,y);
	if(p == NULL)
		return;
	p->se_corner = value;
}

short get_ne_corner(short sector_offset_x, short sector_offset_y, short x, short y)
{
	temp_space_info_ptr p = corner_and_sight_map_at_loc(sector_offset_x,sector_offset_y,x,y);
	if(p == NULL)
		return 0;
	return p->ne_corner;
}

void set_ne_corner(short sector_offset_x, short sector_offset_y, short x, short y, short value)
{
	temp_space_info_ptr p = corner_and_sight_map_at_loc(sector_offset_x,sector_offset_y,x,y);
	if(p == NULL)
		return;
	p->ne_corner = value;
}

void set_up_corner_and_sight_map()
{
	short nw_corner, ne_corner, se_corner, sw_corner;
	Boolean see_to, see_in;
	
	short sector_offset_x, sector_offset_y, x, y, temp_x, temp_y;
	location view_to, view_from;
	
	outdoor_record_type *drawing_terrain = NULL;
	
	view_from.x = (char)cen_x;
	view_from.y = (char)cen_y;
	
	Boolean see_in_neighbors[3][3] = {{TRUE,TRUE,TRUE},{TRUE,TRUE,TRUE},{TRUE,TRUE,TRUE}};
	Boolean see_to_neighbors[3][3] = {{TRUE,TRUE,TRUE},{TRUE,TRUE,TRUE},{TRUE,TRUE,TRUE}};
	
	
	short current_size = ((editing_town) ? max_dim[town_type] : 48);
	
	temp_space_info_ptr temp_ptr;
	
	
	for(sector_offset_x = -1;sector_offset_x <= 1/*!editing_town*/;sector_offset_x++){
	for(sector_offset_y = -1;sector_offset_y <= 1/*!editing_town*/;sector_offset_y++){
	
	if(!editing_town) {
		//don't draw nonexistant sectors
		if(cur_out.x + sector_offset_x < 0 || cur_out.y + sector_offset_y < 0 || 
		cur_out.x + sector_offset_x >= scenario.out_width || cur_out.y + sector_offset_y >= scenario.out_height)
			continue;
		
		if(sector_offset_x == 0 && sector_offset_y == 0)
			drawing_terrain = &current_terrain;
		else
			drawing_terrain = &border_terrains[sector_offset_x + 1][sector_offset_y + 1];
		
		//region number
		if(current_terrain.extra[0] != drawing_terrain->extra[0])
			continue;
	}
	
	for(x = 0; x < current_size; x++) {
		for(y = 0; y < current_size; y++) {
			temp_ptr = corner_and_sight_map_at_loc(sector_offset_x,sector_offset_y,x,y);
			
			if(temp_ptr == NULL)
				continue;
			
			if(cur_viewing_mode == 11) {
				
				temp_x = x;
				temp_y = y;
				if(editing_town) {
					if(sector_offset_x == -1) {
						temp_x = 0;
					}
					if(sector_offset_x == 1) {
						temp_x = current_size - 1;
					}
					if(sector_offset_y == -1) {
						temp_y = 0;
					}
					if(sector_offset_y == 1) {
						temp_y = current_size - 1;
					}
				}
				else {
					temp_x += sector_offset_x * current_size;
					temp_y += sector_offset_y * current_size;
				}
				view_to.x = (char)temp_x;
				view_to.y = (char)temp_y;
				
				//in the game, things a certain distance away aren't drawn.  Also, 
				//that helps here by reducing the number of line-of-sight calculations needed
				if(editing_town && (n_abs(view_to.x - cen_x) > 10 || n_abs(view_to.y - cen_y) > 10))
					continue;
				if(!editing_town && (n_abs(view_to.x - cen_x) + n_abs(view_to.y - cen_y) > 14))
					continue;
			}
			
			find_out_about_corner_walls(drawing_terrain,x,y,current_size,&nw_corner,&ne_corner,&se_corner,&sw_corner);
			
			temp_ptr->nw_corner = nw_corner;
			temp_ptr->sw_corner = sw_corner;
			temp_ptr->se_corner = se_corner;
			temp_ptr->ne_corner = ne_corner;
			//set_nw_corner(sector_offset_x,sector_offset_y,x,y,nw_corner);
			//set_sw_corner(sector_offset_x,sector_offset_y,x,y,sw_corner);
			//set_se_corner(sector_offset_x,sector_offset_y,x,y,se_corner);
			//set_ne_corner(sector_offset_x,sector_offset_y,x,y,ne_corner);
		}
	}
	
	}
	}
	
	
	
	
	
	
	if(cur_viewing_mode == 11) {
	
	for(sector_offset_x = -1;sector_offset_x <= 1/*!editing_town*/;sector_offset_x++){
	for(sector_offset_y = -1;sector_offset_y <= 1/*!editing_town*/;sector_offset_y++){
	
	if(!editing_town) {
		//don't draw nonexistant sectors
		if(cur_out.x + sector_offset_x < 0 || cur_out.y + sector_offset_y < 0 || 
		cur_out.x + sector_offset_x >= scenario.out_width || cur_out.y + sector_offset_y >= scenario.out_height)
			continue;
		
		if(sector_offset_x == 0 && sector_offset_y == 0)
			drawing_terrain = &current_terrain;
		else
			drawing_terrain = &border_terrains[sector_offset_x + 1][sector_offset_y + 1];
		
		//region number
		if(current_terrain.extra[0] != drawing_terrain->extra[0])
			continue;
	}
	
	for(x = 0; x < current_size; x++) {
		for(y = 0; y < current_size; y++) {
			
			
				
				temp_x = x;
				temp_y = y;
				if(editing_town) {
					if(sector_offset_x == -1) {
						temp_x = 0;
					}
					if(sector_offset_x == 1) {
						temp_x = current_size - 1;
					}
					if(sector_offset_y == -1) {
						temp_y = 0;
					}
					if(sector_offset_y == 1) {
						temp_y = current_size - 1;
					}
				}
				else {
					temp_x += sector_offset_x * current_size;
					temp_y += sector_offset_y * current_size;
				}
				view_to.x = (char)temp_x;
				view_to.y = (char)temp_y;
				
				//in the game, things a certain distance away aren't drawn.  Also, 
				//that helps here by reducing the number of line-of-sight calculations needed
				if(editing_town && (n_abs(view_to.x - cen_x) > 10 || n_abs(view_to.y - cen_y) > 10))
					continue;
				if(!editing_town && (n_abs(view_to.x - cen_x) + n_abs(view_to.y - cen_y) > 14))
					continue;
				
				can_see(view_from, view_to, TRUE, FALSE,&see_to,&see_in);
				
				set_see_in(sector_offset_x,sector_offset_y,x,y,see_in);
				set_see_to(sector_offset_x,sector_offset_y,x,y,see_to);
			
			
		}
	}
	
	}
	}
	
	}
	
	
	
	
	
	
	
	for(sector_offset_x = -1;sector_offset_x <= 1;sector_offset_x++){
	for(sector_offset_y = -1;sector_offset_y <= 1;sector_offset_y++){
	
	if(!editing_town) {
		//don't draw nonexistant sectors
		if(cur_out.x + sector_offset_x < 0 || cur_out.y + sector_offset_y < 0 || 
		cur_out.x + sector_offset_x >= scenario.out_width || cur_out.y + sector_offset_y >= scenario.out_height)
			continue;
		
		if(sector_offset_x == 0 && sector_offset_y == 0)
			drawing_terrain = &current_terrain;
		else
			drawing_terrain = &border_terrains[sector_offset_x + 1][sector_offset_y + 1];
		
		//region number
		if(current_terrain.extra[0] != drawing_terrain->extra[0])
			continue;
	}
	
	for(x = 0; x < current_size; x++) {
		for(y = 0; y < current_size; y++) {
			
			temp_ptr = corner_and_sight_map_at_loc(sector_offset_x,sector_offset_y,x,y);
			
			if(temp_ptr == NULL)
				continue;
			
			if(cur_viewing_mode == 11) {
				
				temp_x = x;
				temp_y = y;
				if(editing_town) {
					if(sector_offset_x == -1) {
						temp_x = 0;
					}
					if(sector_offset_x == 1) {
						temp_x = current_size - 1;
					}
					if(sector_offset_y == -1) {
						temp_y = 0;
					}
					if(sector_offset_y == 1) {
						temp_y = current_size - 1;
					}
				}
				else {
					temp_x += sector_offset_x * current_size;
					temp_y += sector_offset_y * current_size;
				}
				view_to.x = (char)temp_x;
				view_to.y = (char)temp_y;
				
				//in the game, things a certain distance away aren't drawn.  Also, 
				//that helps here by reducing the number of line-of-sight calculations needed
				if(editing_town && (n_abs(view_to.x - cen_x) > 10 || n_abs(view_to.y - cen_y) > 10))
					continue;
				if(!editing_town && (n_abs(view_to.x - cen_x) + n_abs(view_to.y - cen_y) > 14))
					continue;
			
				for(view_to.x = 0; view_to.x < 3; view_to.x++) {
					for(view_to.y = 0; view_to.y < 3; view_to.y++) {
						see_in_neighbors[view_to.x][view_to.y] = get_see_in(sector_offset_x,sector_offset_y,x + view_to.x - 1,y + view_to.y - 1);
						see_to_neighbors[view_to.x][view_to.y] = get_see_to(sector_offset_x,sector_offset_y,x + view_to.x - 1,y + view_to.y - 1);
					}
				}
			}
			
			
			//nw_corner = get_nw_corner(sector_offset_x,sector_offset_y,x,y);
			//sw_corner = get_sw_corner(sector_offset_x,sector_offset_y,x,y);
			//se_corner = get_se_corner(sector_offset_x,sector_offset_y,x,y);
			//ne_corner = get_ne_corner(sector_offset_x,sector_offset_y,x,y);
			nw_corner = temp_ptr->nw_corner;
			sw_corner = temp_ptr->sw_corner;
			se_corner = temp_ptr->se_corner;
			ne_corner = temp_ptr->ne_corner;
			
			find_out_about_corner_walls_being_hidden(drawing_terrain,sector_offset_x,sector_offset_y,x,y, /* current_size,*/ see_in_neighbors,
				/* see_to_neighbors,get_see_to(sector_offset_x,sector_offset_y,x,y),*/&nw_corner,&ne_corner,&se_corner,&sw_corner);
			
			temp_ptr->nw_corner = nw_corner;
			temp_ptr->sw_corner = sw_corner;
			temp_ptr->se_corner = se_corner;
			temp_ptr->ne_corner = ne_corner;
			//set_nw_corner(sector_offset_x,sector_offset_y,x,y,nw_corner);
			//set_sw_corner(sector_offset_x,sector_offset_y,x,y,sw_corner);
			//set_se_corner(sector_offset_x,sector_offset_y,x,y,se_corner);
			//set_ne_corner(sector_offset_x,sector_offset_y,x,y,ne_corner);
		}
	}
	
	}
	}
	
	
	
}


void find_out_about_corner_walls(outdoor_record_type *drawing_terrain, short x, short y, short current_size, short *nw_corner, short *ne_corner, short *se_corner, short *sw_corner)
{	
	short n_terrain = 0, w_terrain = 0, s_terrain = 0, e_terrain = 0;
	short n_terrain_position = 0, w_terrain_position = 0, s_terrain_position = 0, e_terrain_position = 0;//0 - non-relevantwall 1 - towards counterclockwise  2 - clockwise
	short /*n_terrain_wall_type = 0,*/ w_terrain_wall_type = 0, /*s_terrain_wall_type = 0,*/ e_terrain_wall_type = 0;//0 if non-relevantwall, 1 if wall type 1, 2 if wall type 2
	
	//initialize the variables to 0
	*nw_corner = *ne_corner = *sw_corner = *se_corner = 0;
	
	//get the terrains
	if(x > 0)
		w_terrain = (editing_town) ? t_d.terrain[x - 1][y] : drawing_terrain->terrain[x - 1][y];
	if(y > 0)
		n_terrain = (editing_town) ? t_d.terrain[x][y - 1] : drawing_terrain->terrain[x][y - 1];
	if(x + 1 < current_size)
		e_terrain = (editing_town) ? t_d.terrain[x + 1][y] : drawing_terrain->terrain[x + 1][y];
	if(y + 1 < current_size)
		s_terrain = (editing_town) ? t_d.terrain[x][y + 1] : drawing_terrain->terrain[x][y + 1];
		
	//only walls are relevant
	if( !(w_terrain >= 2 && w_terrain <= 73))
		w_terrain = 0;
	if( !(n_terrain >= 2 && n_terrain <= 73))
		n_terrain = 0;
	if( !(e_terrain >= 2 && e_terrain <= 73))
		e_terrain = 0;
	if( !(s_terrain >= 2 && s_terrain <= 73))
		s_terrain = 0;
	
	//find out what kind of relevance the wall has, if any
	if(n_terrain != 0)
		n_terrain_position = (n_terrain % 4 == 3 || n_terrain == 6 || n_terrain == 42) ? 1 : ((n_terrain % 4 == 1 || n_terrain == 8 || n_terrain == 44) ? 2 : 0);
	if(w_terrain != 0)
		w_terrain_position = (w_terrain % 4 == 0 || w_terrain == 7 || w_terrain == 43) ? 1 : ((w_terrain % 4 == 2 || w_terrain == 9 || w_terrain == 45) ? 2 : 0);
	if(s_terrain != 0)
		s_terrain_position = (s_terrain % 4 == 1 || s_terrain == 8 || s_terrain == 44) ? 1 : ((s_terrain % 4 == 3 || s_terrain == 6 || s_terrain == 42) ? 2 : 0);
	if(e_terrain != 0)
		e_terrain_position = (e_terrain % 4 == 2 || e_terrain == 9 || e_terrain == 45) ? 1 : ((e_terrain % 4 == 0 || e_terrain == 7 || e_terrain == 43) ? 2 : 0);
	
	
	//preliminary to finding out the wall types of the possible corner walls.
	//two of them have no effect, so don't bother with them.
	//if(n_terrain_position != 0)
	//  n_terrain_wall_type = (n_terrain < 38) ? 1 : 2;
	if(w_terrain_position != 0)
		w_terrain_wall_type = (w_terrain < 38) ? 1 : 2;
	//if(s_terrain_position != 0)
	//  s_terrain_wall_type = (s_terrain < 38) ? 1 : 2;
	if(e_terrain_position != 0)
		e_terrain_wall_type = (e_terrain < 38) ? 1 : 2;
	
	//actually find out which ones exist (and with which wall type).
	//check if the walls are in the right place that there should be a corner wall
	if(n_terrain_position == 1 && w_terrain_position == 2)
		*nw_corner = w_terrain_wall_type;
	if(w_terrain_position == 1 && s_terrain_position == 2)
		*sw_corner = w_terrain_wall_type;
	if(s_terrain_position == 1 && e_terrain_position == 2)
		*se_corner = e_terrain_wall_type;
	if(e_terrain_position == 1 && n_terrain_position == 2)
		*ne_corner = e_terrain_wall_type;
}

void find_out_about_corner_walls_being_hidden(outdoor_record_type *drawing_terrain, short sector_offset_x, short sector_offset_y,
											  short x, short y, /*short current_size,*/ Boolean see_in_neighbors[3][3],
											  /* Boolean see_to_neighbors[3][3], Boolean see_to, */
											  short *nw_corner, short *ne_corner, short *se_corner, short *sw_corner)
{
	Boolean nw_disrupted = FALSE, sw_disrupted = FALSE, se_disrupted = FALSE, ne_disrupted = FALSE;
	Boolean see_a_nw_wall = FALSE, see_a_sw_wall = FALSE, see_a_se_wall = FALSE, see_a_ne_wall = FALSE;
	short center_terrain = 0;
	
	//get the terrain
	center_terrain = (editing_town) ? t_d.terrain[x][y] : drawing_terrain->terrain[x][y];
	
	//only walls are relevant
	if( !(center_terrain >= 2 && center_terrain <= 73))
		center_terrain = 0;
	
	//if there's a wall in the same place a corner wall would be, there shouldn't be a corner wall.
	//this only applies when the interfering wall is visible, though.
	if(*nw_corner && center_terrain != 0)
		nw_disrupted = (
						(center_terrain % 4 == 3 || center_terrain == 6 || center_terrain == 42)
						&&
						(see_in_neighbors[0][1] || see_in_neighbors[1][1])
					   )
					   ||
					   (
						(center_terrain % 4 == 2 || center_terrain == 9 || center_terrain == 45)
						&&
						(see_in_neighbors[1][0] || see_in_neighbors[1][1])
					   );
	if(*ne_corner && center_terrain != 0)
		ne_disrupted = (
						(center_terrain % 4 == 2 || center_terrain == 9 || center_terrain == 45)
						&&
						(see_in_neighbors[1][0] || see_in_neighbors[1][1])
					   )
					   ||
					   (
						(center_terrain % 4 == 1 || center_terrain == 8 || center_terrain == 44)
						&&
						(see_in_neighbors[2][1] || see_in_neighbors[1][1])
					   );
	if(*se_corner && center_terrain != 0)
		se_disrupted = (
						(center_terrain % 4 == 1 || center_terrain == 8 || center_terrain == 44)
						&&
						(see_in_neighbors[2][1] || see_in_neighbors[1][1])
					   )
					   ||
					   (
						(center_terrain % 4 == 0 || center_terrain == 7 || center_terrain == 43)
						&&
						(see_in_neighbors[1][2] || see_in_neighbors[1][1])
					   );
	if(*sw_corner && center_terrain != 0)
		sw_disrupted = (
						(center_terrain % 4 == 0 || center_terrain == 7 || center_terrain == 43)
						&&
						(see_in_neighbors[1][2] || see_in_neighbors[1][1])
					   )
					   ||
					   (
						(center_terrain % 4 == 3 || center_terrain == 6 || center_terrain == 42)
						&&
						(see_in_neighbors[0][1] || see_in_neighbors[1][1])
					   );
	
	if(*nw_corner)
		see_a_nw_wall = ((cen_x < x && cen_y < y) ? (
						is_wall_drawn(drawing_terrain,sector_offset_x,sector_offset_y,x - 1,y) &&
						is_wall_drawn(drawing_terrain,sector_offset_x,sector_offset_y,x,y - 1)
						) : (
						is_wall_drawn(drawing_terrain,sector_offset_x,sector_offset_y,x - 1,y) ||
						is_wall_drawn(drawing_terrain,sector_offset_x,sector_offset_y,x,y - 1)
						));
	if(*sw_corner)
		see_a_sw_wall = ((cen_x < x && cen_y > y) ? (
						is_wall_drawn(drawing_terrain,sector_offset_x,sector_offset_y,x - 1,y) &&
						is_wall_drawn(drawing_terrain,sector_offset_x,sector_offset_y,x,y + 1)
						) : (
						is_wall_drawn(drawing_terrain,sector_offset_x,sector_offset_y,x - 1,y) ||
						is_wall_drawn(drawing_terrain,sector_offset_x,sector_offset_y,x,y + 1)
						));
	if(*se_corner)
		see_a_se_wall = ((cen_x > x && cen_y > y) ? (
						is_wall_drawn(drawing_terrain,sector_offset_x,sector_offset_y,x + 1,y) &&
						is_wall_drawn(drawing_terrain,sector_offset_x,sector_offset_y,x,y + 1)
						) : (
						is_wall_drawn(drawing_terrain,sector_offset_x,sector_offset_y,x + 1,y) ||
						is_wall_drawn(drawing_terrain,sector_offset_x,sector_offset_y,x,y + 1)
						));
	if(*ne_corner)
		see_a_ne_wall = ((cen_x > x && cen_y < y) ? (
						is_wall_drawn(drawing_terrain,sector_offset_x,sector_offset_y,x + 1,y) &&
						is_wall_drawn(drawing_terrain,sector_offset_x,sector_offset_y,x,y - 1)
						) : (
						is_wall_drawn(drawing_terrain,sector_offset_x,sector_offset_y,x + 1,y) ||
						is_wall_drawn(drawing_terrain,sector_offset_x,sector_offset_y,x,y - 1)
						));
	
	
	//actually find out which ones should be drawn.
	//check that there isn't already a wall that would be displayed where the corner wall would be,
	//and that you can actually see the corner
	if(*nw_corner && (nw_disrupted || !see_a_nw_wall))
		*nw_corner = 3;
	if(*sw_corner && (sw_disrupted || !see_a_sw_wall))
		*sw_corner = 3;
	if(*se_corner && (se_disrupted || !see_a_se_wall))
		*se_corner = 3;
	if(*ne_corner && (ne_disrupted || !see_a_ne_wall))
		*ne_corner = 3;
	/*if(*nw_corner && (nw_disrupted || !((see_in_neighbors[0][0] && see_to && cen_x < x && cen_y < y) || see_in_neighbors[1][1])))
		*nw_corner = 3;
	if(*sw_corner && (sw_disrupted || !((see_in_neighbors[0][2] && see_to && cen_x < x && cen_y > y) || see_in_neighbors[1][1])))
		*sw_corner = 3;
	if(*se_corner && (se_disrupted || !((see_in_neighbors[2][2] && see_to && cen_x > x && cen_y > y) || see_in_neighbors[1][1])))
		*se_corner = 3;
	if(*ne_corner && (ne_disrupted || !((see_in_neighbors[2][0] && see_to && cen_x > x && cen_y < y) || see_in_neighbors[1][1])))
		*ne_corner = 3;*/
}

Boolean is_wall_drawn(outdoor_record_type *drawing_terrain, short sector_offset_x, short sector_offset_y, short x, short y)
{
	//get the terrain
	short wall_terrain = (editing_town) ? t_d.terrain[x][y] : drawing_terrain->terrain[x][y];
	
	//only walls are relevant
	if( !(wall_terrain >= 2 && wall_terrain <= 73))
		return FALSE;
	
	if(get_see_in(sector_offset_x,sector_offset_y,x,y))
		return TRUE;
	if(!get_see_to(sector_offset_x,sector_offset_y,x,y))
		return FALSE;
	
	return (
		(scen_data.scen_ter_types[wall_terrain].see_block[0] == 1 && get_see_in(sector_offset_x,sector_offset_y,x,y - 1) == TRUE) ||
		(scen_data.scen_ter_types[wall_terrain].see_block[1] == 1 && get_see_in(sector_offset_x,sector_offset_y,x - 1,y) == TRUE) ||
		(scen_data.scen_ter_types[wall_terrain].see_block[2] == 1 && get_see_in(sector_offset_x,sector_offset_y,x,y + 1) == TRUE) ||
		(scen_data.scen_ter_types[wall_terrain].see_block[3] == 1 && get_see_in(sector_offset_x,sector_offset_y,x + 1,y) == TRUE)
	);
}

//undo system functions

//This should not be called directly except by appendChangeToLatestStep
//Calls from the editing functions to register undo steps should go to 
//appendChangeToLatestStep and lockLatestStep
void pushNewUndoStep()
{
	undo.push(new drawingUndoStep());
	purgeRedo();
}

//should only be called by redo
//adds an entire undo step to the undo stack. During editing undo steps 
//should be accumulated using appendChangeToLatestStep.
void pushUndoStep(drawingUndoStep* s)
{
	undo.push(s);
}

//retrieves the most recent undo step and removes it from the undo stack. 
//This should only be called by undo()
drawingUndoStep* popUndoStep()
{
	if(undo.size()==0)
		return(NULL);
	return((drawingUndoStep*)(undo.pop()));
}

//Adds a change to the most recent undo step. If there is no step in the stack, or the most recent 
//step is locked, a new step which contains the indicated change is created, and pushed onto the stack. 
//Operations should call appendChangeToLatestStep repeatedly to accumulate changes and then call lockLatestStep 
//when the set of changes is complete. This forces a new step to be begun when the next changes are registered. 
//When the user selects the undo command the most recent undo step should include all of the logically related 
//changes; that is all of the changes that have been registered since the last call to lockLatestStep
void appendChangeToLatestStep(drawingChange* c)
{
	if(undo.size()==0 || ((drawingUndoStep*)(undo.itemAtIndex(0)))->locked)
		pushNewUndoStep();
	((drawingUndoStep*)(undo.itemAtIndex(0)))->appendChange(c);
}

//locks the most recent undo step, so that any new changes, even if they are of a matching type 
//will be forced to go to a new step which can be undone seperately
void lockLatestStep()
{
	if(undo.size()==0)
		return;
	((drawingUndoStep*)(undo.itemAtIndex(0)))->locked=TRUE;
}

//should be called every time a new zone is loaded
//Otherwise the user could still undo changes made in other zones, but they would be undone 
//in the _current_ zone, which would make a mess and serve no purpose. Therefore existing undo steps
//must be invalidated and removed by calling this function when the zone is switched.
void purgeUndo()
{
	while(undo.size()>0){
		drawingUndoStep* temp = (drawingUndoStep*)undo.pop();
		delete temp;
	}
}

//This is the actual undo operation that should be called when the user wants to undo editing changes
void performUndo()
{
	if(undo.size()==0){//there are no steps to undo; complain
		beep();
		return;
	}
	drawingUndoStep* s = popUndoStep();
	//actually perform the operations to reverse whatever was done
	for(int i=0; i<s->numChanges(); i++){
		drawingChange* c = s->getChange(i);
		if(editing_town){
			if(c->type==1)
				t_d.floor[c->x][c->y]=c->oldval;
			else if(c->type==2)
				t_d.terrain[c->x][c->y]=c->oldval;
			else if(c->type==3)
				t_d.height[c->x][c->y]=c->oldval;
		}
		else{
			if(c->type==1)
				current_terrain.floor[c->x][c->y]=c->oldval;
			else if(c->type==2)
				current_terrain.terrain[c->x][c->y]=c->oldval;
			else if(c->type==3)
				current_terrain.height[c->x][c->y]=c->oldval;
		}
	}
	//add this step to the redo list so that it can be un-undone
	s->invert();
	pushRedoStep(s);
	redraw_screen();
}

//adds a redo step to the redo stack. Should only be called by undo()
void pushRedoStep(drawingUndoStep* s)
{
	redo.push(s);
}

drawingUndoStep* popRedoStep()
{
	if(redo.size()==0)
		return(NULL);
	return((drawingUndoStep*)(redo.pop()));
}

//should be called every time a new zone is loaded _and_ every time the user executes an editing operation
//redo steps need to be invalidated for the same reason as undo steps when the zone is switched, 
//but should also be invalidated whenever the user makes a new editing change. For example, suppose
//the user opens a file, places a tree, and then undoes the drawing operation. At this point there 
//will be no undo steps, and one redo step to redraw the tree. Suppose the user then places a wall on the 
//same spot. Then, if the redo steps are not purged, there will be one undo step, to remove the wall again, 
//and the redo step to re-place the tree will still remain. If the user then selects Redo, the wall will be 
//replaced with the tree, which makes no sense. Instead, as soon as the wall is added, the redo steps should
//be purged to eliminate nonsensical redo operations. However, if the user undoes one or more operations 
//without doing anything else, the redo steps must not be purged, in order to allow the user to move freely
//back and forth through the recorded states. 
void purgeRedo()
{
	while(redo.size()>0){
		drawingUndoStep* temp = (drawingUndoStep*)redo.pop();
		delete temp;
	}
}

//This is the actual redo operation that should be called when the user wants to redo editing changes that were undone
void performRedo()
{
	if(redo.size()==0){//there are no steps to redo; complain
		beep();
		return;
	}
	drawingUndoStep* s = popRedoStep();
	//actually perform the operations to reverse whatever was undone
	for(int i=0; i<s->numChanges(); i++){
		drawingChange* c = s->getChange(i);
		if(editing_town){
			if(c->type==1)
				t_d.floor[c->x][c->y]=c->oldval;
			else if(c->type==2)
				t_d.terrain[c->x][c->y]=c->oldval;
			else if(c->type==3)
				t_d.height[c->x][c->y]=c->oldval;
		}
		else{
			if(c->type==1)
				current_terrain.floor[c->x][c->y]=c->oldval;
			else if(c->type==2)
				current_terrain.terrain[c->x][c->y]=c->oldval;
			else if(c->type==3)
				current_terrain.height[c->x][c->y]=c->oldval;
		}
	}
	//add this step to the redo list so that it can be undone again
	s->invert();
	s->locked = TRUE;//don't let this step accumulate any more changes than it has
	pushUndoStep(s);
}
