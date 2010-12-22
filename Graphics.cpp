#include "stdafx.h"
#include "global.h"
#include "dibengine.h"

// Constant

// 3D clipping range
const int kTer3DTopClip = 1;
const int kTer3DBottomClip = 3;


// Global variables

HDC main_dc;
HDC main_dc2;
HDC main_dc3;
HDC main_dc4;
HDC main_dc5;
HDIB mixed_gworld = NULL;
HDIB ed_pattern_gworld = NULL;
HBITMAP bw_bitmap = NULL;
HFONT font;
HFONT small_bold_font;
HFONT italic_font;
HFONT underline_font;
HFONT bold_font;
HFONT tiny_font;

RECT terrain_buttons_rect = {0,0,210,584}; /* was 550 */

Boolean showed_graphics_error = FALSE;

// external global variables

extern HWND	mainPtr;
extern HWND right_sbar;
extern RECT right_sbar_rect;

extern scenario_data_type scenario;
extern zone_names_data_type zone_names;
extern town_record_type town;
extern big_tr_type t_d;
extern outdoor_record_type current_terrain;
extern scen_item_data_type scen_data;
extern outdoor_record_type border_terrains[3][3];

extern short cur_town;
extern location cur_out;
extern short current_drawing_mode;
extern short town_type;
extern short current_height_mode;
extern Boolean editing_town;
extern short cur_viewing_mode;
extern short ulx,uly;

extern short cen_x, cen_y;

extern short available_dlog_buttons[NUM_DLOG_B];
extern short max_dim[3];
extern RECT windRect;
extern RECT world_screen;

extern Boolean file_is_loaded;
extern char *button_strs[140];

extern short current_floor_drawn;
extern short current_terrain_drawn;

	// border rects order: top, left, bottom, right //
extern RECT border_rect[4];
extern RECT terrain_rects[264];							

extern RECT terrain_rects_3D[264];
extern short hill_c_heights[12][4];

extern RECT palette_buttons[8][6];
extern RECT small_edit_ter_rects[MAX_TOWN_SIZE][MAX_TOWN_SIZE];
extern RECT medium_edit_ter_rects[32][32];
extern RECT large_edit_ter_rects[9][9];
extern RECT left_text_lines[14];
extern RECT right_text_lines[6];

extern short selected_item_number;

char hintbook_mode0 = 0;
char hintbook_mode1 = 1;
char hintbook_mode2 = 0;
char hintbook_mode3 = 0;
char hintbook_mode4 = 0;
char hintbook_mode5 = 0;
char hintbook_mode6 = 0;
char hintbook_mode7 = 0;
char hintbook_mode8 = 0;
char hintbook_mode9 = 0;

char grid_mode = 0;
char *numerical_display_captions[5] = {"(Null Display)","Display Floor Type",
"Display Terrain Type","Display Height","Display Screen Coordinates"};
char *object_display_captions[7] = {"(Null Display)","Placed Specials",
"Sign Numbers","Town Creatures","Terrain Script Numbers","Placed Fields","Horses and Boats"};
char *object_out_display_captions[7] = {"(Null Display)","Placed Specials",
"Sign Numbers","Preset Encounters","Town Entrances","(Null Display)","(Null Display)"};

extern Boolean small_any_drawn;
extern short numerical_display_mode;
extern short object_display_mode;

// local variables

HBITMAP main_bitmap = NULL;
HBITMAP main_bitmap_save;
HDIB editor_mixed = NULL;
HDIB terrain_buttons_gworld = NULL;
HDIB ter_draw_gworld = NULL;

HDIB dlog_horiz_border_bottom_gworld = NULL;
HDIB dlog_horiz_border_top_gworld = NULL;
HDIB dlog_vert_border_gworld = NULL;
HDIB pattern_gworld = NULL;

HDIB dialog_pattern_gworld = NULL;
HDIB pat_source_gworld = NULL;
HDIB buttons_gworld = NULL;
HDIB ter_resize_buffer_gworld = NULL;

// Other game rectangles
RECT terrain_rect_gr_size = {0,0,512,512};

// RECT blue_button_from = {91,112,107,126}; /**/						
// RECT start_button_from = {70,112,91,119};
RECT base_small_button_from = {0,103,10,113};
RECT base_small_3D_button_from = {0,143,16,154};
RECT palette_button_base = {0,0,25,18};
extern const RECT terrain_viewport_3d = {5,5,501,420};

RECT buttons_from[NUM_BUTTONS] = {{0,0,23,23},{46,0,108,23},{0,132,102,155},{126,23,142,36},
								{0,92,62,115},{0,69,62,92},{126,46,188,69},{126,69,188,92}, // l, r, u, d
								{172,30,184,42},{172,30,184,42},{196,30,208,42},{0,23,62,46},  // small, small, , done
								{0,46,62,69},{208,30,226,46},{204,132,222,148},{172,0,202,30}}; // cancel, up arrow  down arrow

// Palette UL x = 525 y = 382

// graphics library
HDIB graphics_library[MAX_NUM_SHEETS_IN_LIBRARY];
graphic_id_type graphics_library_index[MAX_NUM_SHEETS_IN_LIBRARY];
short num_sheets_in_library = 0;

char current_string[256] = "";
char current_string2[256] = "";
char *attitude_types[4] = {"Friendly","Neutral","Hostile, Type A","Hostile, Type B"};
char *facings[4] = {"North","West","South","East"};

short small_what_drawn[64][64];
short small_what_floor_drawn[64][64];

HDIB tint_area;
RECT tint_rect = {0,0,PICT_BOX_WIDTH_3D,PICT_BOX_HEIGHT_3D};

// function prototypes
// PARTIAL  TABLE  OF  CONTENTS

void load_main_screen();
void delete_graphic(HDIB *to_delete);
void draw_wall_3D_sidebar(short t_to_draw, RECT to_rect);
Boolean place_icon_into_3D_sidebar(graphic_id_type icon, RECT to_rect, short unscaled_offset_x, short unscaled_offset_y);
Boolean place_icon_into_ter_3D_large(graphic_id_type icon,short at_point_center_x,short at_point_center_y,RECT *to_whole_area_rect,short lighting);
Boolean place_creature_icon_into_ter_3D_large(graphic_id_type icon,short at_point_center_x,short at_point_center_y,RECT *to_whole_area_rect,short lighting,short r,short g,short b);
Boolean place_cliff_icon_into_ter_3D_large(short sheet,short at_point_center_x,short at_point_center_y,	short direction,RECT *to_whole_area_rect,short lighting);
Boolean place_item_icon_into_ter_3D_large(graphic_id_type icon,short at_point_center_x,short at_point_center_y,RECT *to_whole_area_rect,short lighting);
Boolean place_outdoor_creature_icon_into_ter_3D_large(graphic_id_type icon,short at_point_center_x,short at_point_center_y,RECT *to_whole_area_rect,short lighting);
Boolean place_corner_wall_icon_into_ter_3D_large(graphic_id_type icon,short at_point_center_x,short at_point_center_y,Boolean left_side_of_template,RECT *to_whole_area_rect,short lighting);
void place_ter_icon_on_tile_3D(short at_point_center_x,short at_point_center_y,short position,short which_icon,RECT *to_whole_area_rect);
void draw_ter_script_3D(short at_point_center_x,short at_point_center_y,RECT *to_whole_area_rect);
void place_ter_icons_3D(location which_outdoor_sector, outdoor_record_type *drawing_terrain, short square_x, short square_y,short t_to_draw, short floor_to_draw, short at_point_center_x,short at_point_center_y,RECT *to_whole_area_rect);
void draw_ter_icon_3D(short terrain_number,short icon_number,short x,short y,graphic_id_type a,short t_to_draw,RECT *to_whole_area_rect,short lighting,short height);
void draw_terrain_3D(short t_to_draw, short x, short y, short sector_x, short sector_y, short at_x, short at_y, Boolean see_in_neighbors[3][3], Boolean is_wall_corner,RECT *to_whole_area_rect,short lighting);
void draw_creature_3D(short creature_num,short at_point_center_x,short at_point_center_y, short square_x, short square_y,RECT *to_whole_area_rect,short lighting);
void draw_item_3D(short item_num,short at_point_center_x,short at_point_center_y, short square_x, short square_y,RECT *to_whole_area_rect,short lighting);
void put_line_segment_in_gworld_3D(HDC line_gworld,outdoor_record_type *drawing_terrain,short at_point_center_3D_x,short at_point_center_3D_y,short square_2D_x, short square_2D_y, short line_on_2D_x_side, short line_on_2D_y_side, Boolean corner_label_x,Boolean corner_label_y, short inset_3D_y, short offset_3D_y,short r,short g, short b,RECT *to_whole_area_rect);
void maybe_draw_part_of_3D_rect(outdoor_record_type *drawing_terrain, short center_of_current_square_x, short center_of_current_square_y, short x, short y,macRECT rect, short inset, short r, short g, short b,RECT *to_whole_area_rect);
void draw_town_objects_3D(short x, short y, short at_point_center_x, short at_point_center_y,RECT *to_whole_area_rect,short lighting);
void draw_ter_3D_large();
void draw_ter_large();
void draw_ter_medium();
void draw_ter_small();
Boolean place_terrain_icon_into_ter_large(graphic_id_type icon,short in_square_x,short in_square_y);
Boolean place_terrain_icon_into_ter_medium(graphic_id_type icon,short in_square_x,short in_square_y);
Boolean place_terrain_icon_into_ter_small(graphic_id_type icon,short in_square_x,short in_square_y);
void place_ter_icon_on_tile(short tile_x,short tile_y,short position,short which_icon);
void draw_creature(HDC ter_hdc,HBITMAP store_bmp,short creature_num,location loc_drawn,short in_square_x,short in_square_y);
void draw_creature_medium(HDC ter_hdc,HBITMAP store_bmp,short creature_num,location loc_drawn,short in_square_x,short in_square_y);
void draw_item(HDC ter_hdc,HBITMAP store_bmp,short item_num,location loc_drawn,short in_square_x,short in_square_y);
void draw_ter_script(short script_num,location loc_drawn,short in_square_x,short in_square_y);
void place_left_text();
void win_draw_string_outline(HDC dest_hdc,RECT dest_rect,char *str,short mode,short line_height);
void win_draw_string(HDC dest_hdc,RECT dest_rect,char *str,short mode);
Boolean load_sheet_into_library(graphic_id_type *new_sheet);
short get_index_of_sheet(graphic_id_type *sheet);
short safe_get_index_of_sheet(graphic_id_type *sheet);
void put_rect_in_gworld(HDC hdc,RECT rect,short r,short g, short b);
void fill_rect_in_gworld(HDC line_gworld,RECT to_rect,short r,short g, short b);
void put_line_in_gworld(HDC line_gworld,short from_x,short from_y,short to_x,short to_y,short r,short g, short b);
void put_rect_on_screen(HDC win,RECT to_rect,short r,short g, short b);
void put_clipped_rect_in_gworld(HDC line_gworld,RECT to_rect,RECT clip_rect,short r,short g, short b);
Boolean is_field_type(short i,short j,short field_type);
void make_field_type(short i,short j,short field_type);
void take_field_type(short i,short j,short field_type);
Boolean is_fire_barrier(short i,short j);
Boolean is_force_barrier(short i,short j);
Boolean is_blocked(short i,short j);
Boolean is_sfx(short i,short j,short type);
void place_dlog_border_on_win(HDIB to_gworld,HWND win,RECT border_to_rect,short horiz_or_vert,short bottom_or_top);
void cant_draw_graphics_error(graphic_id_type a,char *bonus_string,short bonus_num);
void adjust_graphic(HDIB *src_gworld_ptr, RECT *from_rect_ptr, short graphic_adjust);
void apply_lighting_to_graphic(HDIB *src_gworld_ptr, RECT *from_rect_ptr, short lighting);
void add_border_to_graphic(HDIB *src_gworld_ptr, RECT *from_rect_ptr, short border_r, short border_g, short border_b);
//void make_Boat(short i,short j);
// declared elsewhere but still found here:
// void set_blitter_color(UInt16 color);
// void tint_graphic( GWorldPtr dest,short tint_strength);
// void hue_graphic( GWorldPtr dest,short up_or_down,short red_shift,short green_shift,short blue_shift);


// void refresh_graphics_on_screen();


void Set_up_win ()
{
	short i,j;

	font = CreateFont(12,0,0,0,0, 0,0,0, 0,0, 0,0,0,"MS Sans Serif");
	small_bold_font = CreateFont(12,0,0,0,700, 0,0,0, 0,0, 0,0,0,"MS Sans Serif");
	italic_font = CreateFont(12,0,0,0,0, 1,0,0, 0,0, 0,0,0,"MS Sans Serif");
	underline_font = CreateFont(12,0,0,0,0, 0,1,0, 0,0, 0,0,0,"MS Sans Serif");
	bold_font = CreateFont(14,0,0,0,700, 0,0,0, 0,0, 0,0,0,"MS Sans Serif");
	tiny_font = font;

	for (i = 0; i < 8; i++){
		for (j = 0; j < 6; j++){
		    palette_buttons[i][j] = palette_button_base;
		    OffsetRect(&palette_buttons[i][j],i * 25 + PALETTE_BUT_UL_X, j * 17 + PALETTE_BUT_UL_Y);
		}	
    }
	for (i = 0; i < MAX_TOWN_SIZE; i++){
		for (j = 0; j < MAX_TOWN_SIZE; j++){
			SetRECT(small_edit_ter_rects[i][j],  i * SMALL_SPACE_SIZE,  j * SMALL_SPACE_SIZE,
			    (i + 1) * SMALL_SPACE_SIZE,  (j + 1) * SMALL_SPACE_SIZE);
		}
    }

	for (i = 0; i < MAX_TOWN_SIZE; i++){
		for (j = 0; j < MAX_TOWN_SIZE; j++){
			SetRECT(medium_edit_ter_rects[i][j],  i * MEDIUM_SPACE_SIZE,  j * MEDIUM_SPACE_SIZE,
			    (i + 1) * MEDIUM_SPACE_SIZE,  (j + 1) * MEDIUM_SPACE_SIZE);
		}
    }

	for (i = 0; i < 9; i++){
		for (j = 0; j < 9; j++){
			SetRECT(large_edit_ter_rects[i][j],TERRAIN_BORDER_WIDTH + i * BIG_SPACE_SIZE,TERRAIN_BORDER_WIDTH + j * BIG_SPACE_SIZE,
			  TERRAIN_BORDER_WIDTH + (i + 1) * BIG_SPACE_SIZE,TERRAIN_BORDER_WIDTH + (j + 1) * BIG_SPACE_SIZE);
		}
    }
	for (i = 0; i < 264; i++)
		SetRECT(terrain_rects[i],3 + (i % 12) * (TER_BUTTON_SIZE + 1),2 + (i / 12) * (TER_BUTTON_SIZE + 1),
			3 + (i % 12) * (TER_BUTTON_SIZE + 1) + TER_BUTTON_SIZE,2 + (i / 12) * (TER_BUTTON_SIZE + 1) + TER_BUTTON_SIZE);

	for (i = 0; i < 228; i++)
		SetRECT(terrain_rects_3D[i],3 + (i % 12) * (TER_BUTTON_SIZE + 1),2 + (i / 12) * (TER_BUTTON_HEIGHT_3D + 1),
			3 + (i % 12) * (TER_BUTTON_SIZE + 1) + TER_BUTTON_SIZE,2 + (i / 12) * (TER_BUTTON_HEIGHT_3D + 1) + TER_BUTTON_HEIGHT_3D);

	for (i = 0; i < MAX_NUM_SHEETS_IN_LIBRARY; i++) {
		graphics_library[i] = NULL;
	}
	RECT world_screen = {large_edit_ter_rects[0][0].left,large_edit_ter_rects[0][0].top,
		large_edit_ter_rects[8][8].right,large_edit_ter_rects[8][8].bottom}; /**/

	MacInsetRect(&world_screen,-15,-15);
	OffsetRect(&world_screen,TER_RECT_UL_X,TER_RECT_UL_Y);

	for (i = 0; i < 4; i++)
		border_rect[i] = world_screen;
	border_rect[0].bottom = border_rect[0].top + 15;
	border_rect[1].right = border_rect[1].left + 15;
	border_rect[2].top = border_rect[2].bottom - 15;
	border_rect[3].left = border_rect[3].right - 15;
			
	load_main_screen();
}

void load_main_screen()
{
	main_dc = GetDC(mainPtr);

	SelectObject(main_dc,font);
	SetBkMode(main_dc,TRANSPARENT);
	SetStretchBltMode(main_dc,STRETCH_DELETESCANS);
	main_dc2 = CreateCompatibleDC(main_dc);
	SetMapMode(main_dc2,GetMapMode(main_dc));
	SetStretchBltMode(main_dc2,STRETCH_DELETESCANS);
	main_dc3 = CreateCompatibleDC(main_dc);
	SetMapMode(main_dc3,GetMapMode(main_dc));
	SetStretchBltMode(main_dc3,STRETCH_DELETESCANS);
	main_dc4 = CreateCompatibleDC(main_dc);
	SetMapMode(main_dc4,GetMapMode(main_dc));
	SetStretchBltMode(main_dc4,STRETCH_DELETESCANS);
	main_dc5 = CreateCompatibleDC(main_dc);
	SetMapMode(main_dc5,GetMapMode(main_dc));
	SetStretchBltMode(main_dc5,STRETCH_DELETESCANS);

	terrain_buttons_gworld = DibCreate (terrain_buttons_rect.right,terrain_buttons_rect.bottom, 16,0);
	
	ter_draw_gworld = DibCreate (terrain_rect_gr_size.right,terrain_rect_gr_size.bottom, 16,0);

	ter_resize_buffer_gworld = DibCreate (100,100, 16,0);

	tint_area = DibCreate (tint_rect.right,tint_rect.bottom, 16,0);

	buttons_gworld = load_pict(2000);

	bw_bitmap = CreateBitmap(72,72,1,1,NULL);
	pattern_gworld = DibCreate (192,256, 16,0);
	dialog_pattern_gworld = DibCreate (192,256, 16,0);
	pat_source_gworld = load_pict(4900);
	editor_mixed = load_pict(4915);

	mixed_gworld = load_pict(903);
	if (dlog_horiz_border_bottom_gworld == NULL)
		dlog_horiz_border_bottom_gworld = load_pict(842);//(52);
	if (dlog_horiz_border_top_gworld == NULL)
		dlog_horiz_border_top_gworld = load_pict(841);//(51);
	if (dlog_vert_border_gworld == NULL)
		dlog_vert_border_gworld = load_pict(840);//(850);
	if (ed_pattern_gworld == NULL)
		ed_pattern_gworld = load_pict(4901);//(850);
}

// lose other graphics
void lose_graphics() 
{
	if (bw_bitmap != NULL) {
		DeleteObject(bw_bitmap);
		bw_bitmap = NULL;
	}
	if (main_bitmap != NULL) {
		SelectObject( main_dc, main_bitmap_save );
		DeleteObject( main_bitmap );
		main_bitmap = NULL;
	}
	refresh_graphics_library();
	delete_graphic(&ed_pattern_gworld);
	delete_graphic(&dlog_vert_border_gworld);
	delete_graphic(&dlog_horiz_border_top_gworld);
	delete_graphic(&dlog_horiz_border_bottom_gworld);
	delete_graphic(&mixed_gworld);
	delete_graphic(&editor_mixed);
	delete_graphic(&pat_source_gworld);
	delete_graphic(&dialog_pattern_gworld);
	delete_graphic(&pattern_gworld);
	delete_graphic(&buttons_gworld);
	delete_graphic(&tint_area);
	delete_graphic(&ter_resize_buffer_gworld);
	delete_graphic(&ter_draw_gworld);
	delete_graphic(&terrain_buttons_gworld);

	ReleaseDC( mainPtr, main_dc );

	DeleteDC(main_dc2);
	DeleteDC(main_dc3);
	DeleteDC(main_dc4);
	DeleteDC(main_dc5);

	DeleteObject(font);
	DeleteObject(small_bold_font);
	DeleteObject(italic_font);
	DeleteObject(underline_font);
	DeleteObject(bold_font);

}

void redraw_screen()
{
	// fill left with pattern
	RECT to_rect = windRect;
	to_rect.right = RIGHT_BUTTONS_X_SHIFT;
	paint_pattern(NULL,1,to_rect,0);
	
	// fill lower right corner
	to_rect = windRect;
	to_rect.left = terrain_buttons_rect.right + RIGHT_BUTTONS_X_SHIFT;
	to_rect.top = 22 * (TER_BUTTON_SIZE + 1) + 1;
	paint_pattern(NULL,1,to_rect,0);

	small_any_drawn = TRUE;
	draw_main_screen();
}

void draw_main_screen()
{
	place_right_buttons();
	draw_terrain();
}


void set_up_terrain_buttons()
{
	short i,j;
	RECT ter_from,ter_from_base = {0,0,16,16};
	HBITMAP store_bmp;
	HFONT store_font;

 	paint_pattern(terrain_buttons_gworld,0,terrain_buttons_rect,2);
	
	// frame rect around buttons
	SetBkMode(main_dc5,TRANSPARENT);
	store_font = (HFONT)SelectObject(main_dc5,bold_font);
	store_bmp = (HBITMAP) SelectObject(main_dc5,DibBitmapHandle(terrain_buttons_gworld));
	put_rect_in_gworld(main_dc5,terrain_buttons_rect,0,0,0);

	if (file_is_loaded == FALSE) {
		SelectObject(main_dc5,store_font);
		SelectObject(main_dc5,store_bmp);
		return;
	}

 	// first make terrain buttons
	for (i = 0; i < ((cur_viewing_mode >= 10 && current_drawing_mode > 0) ? 228 : 264); i++){
		if ((i < 256) || (current_drawing_mode > 0)) {
			ter_from = ter_from_base;
			
			graphic_id_type a;
			
			Boolean do_this_item = TRUE;
			short store_ter_type = 0;
			
			if(cur_viewing_mode == 10 || cur_viewing_mode == 11) {
				SetRECT(ter_from,0,0,PICT_BOX_WIDTH_3D,PICT_BOX_HEIGHT_3D);
				switch (current_drawing_mode) {
				case 0:
					a = scen_data.scen_floors[i].pic;
					OffsetRect(&ter_from,(1 + PICT_BOX_WIDTH_3D) * (a.which_icon % 10) + 1,(1 + PICT_BOX_HEIGHT_3D) * (a.which_icon / 10) + 1);
					if (a.not_legit())
						do_this_item = FALSE;
					break;
				case 1: case 2:
					short sbar_pos = GetControlValue(right_sbar);
					store_ter_type = sbar_pos * 12 + i;
					if(store_ter_type == 0)
						fill_rect_in_gworld(main_dc5,terrain_rects_3D[i],255,255,255);
					if (store_ter_type < 512) {
						a = scen_data.scen_ter_types[store_ter_type].pic;
						//if a wall (or fence - at least one move block, but not all)
						//this doesn't include open fence gates, but perhaps that's good - they can 
						//stick out of a square unpredictably
						if( (store_ter_type >= 2 && store_ter_type <= 73)
							|| (     !(scen_data.scen_ter_types[store_ter_type].move_block[0] == 1
									&& scen_data.scen_ter_types[store_ter_type].move_block[1] == 1
									&& scen_data.scen_ter_types[store_ter_type].move_block[2] == 1
									&& scen_data.scen_ter_types[store_ter_type].move_block[3] == 1)
								&&    (scen_data.scen_ter_types[store_ter_type].move_block[0] == 1
									|| scen_data.scen_ter_types[store_ter_type].move_block[1] == 1
									|| scen_data.scen_ter_types[store_ter_type].move_block[2] == 1
									|| scen_data.scen_ter_types[store_ter_type].move_block[3] == 1) ) ) {
							SelectObject(main_dc5,store_bmp);
							draw_wall_3D_sidebar(store_ter_type, terrain_rects_3D[i]);
							SelectObject(main_dc5,DibBitmapHandle(terrain_buttons_gworld));
							do_this_item = FALSE;
						}
						OffsetRect(&ter_from,(1 + PICT_BOX_WIDTH_3D) * (a.which_icon % 10) + 1,(1 + PICT_BOX_HEIGHT_3D) * (a.which_icon / 10) + 1);
						if (a.not_legit())
							do_this_item = FALSE;
					}
					else do_this_item = FALSE;
					break;
				}
				//It seems one of the terrain graphics sheets is covered up by (same resource number as) an editor graphic.
				//So I had to copy that sheet into 3D editor graphics and make this horrible hack (in multiple places!)
				if(a.which_sheet == 701)
					a.which_sheet = 4916;
			} 
            else {
				switch (current_drawing_mode) {
				case 0:
					a = scen_data.scen_floors[i].ed_pic;
					OffsetRect(&ter_from,(1 + TER_BUTTON_SIZE) * (a.which_icon % 10) + 1,(1 + TER_BUTTON_SIZE) * (a.which_icon / 10) + 1);
					if (a.not_legit())
						do_this_item = FALSE;
					break;
				case 1: case 2:
					short sbar_pos = GetControlValue(right_sbar);
					store_ter_type = sbar_pos * 12 + i;
					if (store_ter_type < 512) {
						a = scen_data.scen_ter_types[store_ter_type].ed_pic;
						OffsetRect(&ter_from,(1 + TER_BUTTON_SIZE) * (a.which_icon % 10) + 1,(1 + TER_BUTTON_SIZE) * (a.which_icon / 10) + 1);
						if (a.not_legit())
							do_this_item = FALSE;
					}
					else do_this_item = FALSE;
					break;
				}
			}

			if (do_this_item == TRUE) {
				short index = safe_get_index_of_sheet(&a);
				if (index < 0) {
					SelectObject(main_dc5,store_bmp);
					if (current_drawing_mode == 0)
						cant_draw_graphics_error(a,"Error was for floor type",i);
					else
						cant_draw_graphics_error(a,"Error was for terrain type",store_ter_type);
					return;	
				}
                
				if((cur_viewing_mode >= 10) && (current_drawing_mode == 0))
					ter_from.top += (PICT_BOX_HEIGHT_3D - PICT_BOX_WIDTH_3D);
				
				SelectObject(main_dc5,store_bmp);
				rect_draw_some_item(graphics_library[index],
					ter_from,terrain_buttons_gworld,(cur_viewing_mode >= 10 && current_drawing_mode > 0) ? 
					terrain_rects_3D[i] : terrain_rects[i],0,0);
				SelectObject(main_dc5,DibBitmapHandle(terrain_buttons_gworld));
			}
		}
	}

	SelectObject(main_dc5,store_font);
	SelectObject(main_dc5,store_bmp);

	// palette buttons

	const RECT kRealisticButtonRect = {PALETTE_BUT_WIDTH * 7,PALETTE_BUT_HEIGHT * 6,
				PALETTE_BUT_WIDTH * 8,PALETTE_BUT_HEIGHT * 7 + 1};
	const RECT kObliqueButtonRect = {PALETTE_BUT_WIDTH * 5,PALETTE_BUT_HEIGHT * 6,
				PALETTE_BUT_WIDTH * 6,PALETTE_BUT_HEIGHT * 7 + 1};
	const RECT kFacingButtonRect = {PALETTE_BUT_WIDTH * 6,PALETTE_BUT_HEIGHT * 6,
				PALETTE_BUT_WIDTH * 7,PALETTE_BUT_HEIGHT * 7 + 1};

	for (i = 0; i < 8; i++){
		for (j = 0; j < ((editing_town == TRUE) ? 6 : 3); j++) {
			RECT from_rect = {PALETTE_BUT_WIDTH * i,PALETTE_BUT_HEIGHT * j,
				PALETTE_BUT_WIDTH * (i + 1),PALETTE_BUT_HEIGHT * (j + 1) + 1}; /**/
			RECT to_rect = palette_buttons[i][j];
			if(cur_viewing_mode >= 10 && i == 0 && j == 1)		// realistic mode palette icon
				from_rect = kRealisticButtonRect;
			if(editing_town == TRUE && i == 6 && j == 1)
				from_rect = kObliqueButtonRect;
			if(editing_town == TRUE && i == 7 && j == 1)
				from_rect = kFacingButtonRect;

			to_rect.right++;
			from_rect.right++;
			
			rect_draw_some_item(editor_mixed,
				from_rect,terrain_buttons_gworld,to_rect,0,0);
		}
    }
}

void delete_graphic(HDIB *to_delete)
{
	if (*to_delete == NULL)
		return;
		
	DibDelete(*to_delete);
	*to_delete = NULL;
}

void draw_wall_3D_sidebar(short t_to_draw, RECT to_rect)
{			
	graphic_id_type a;
	short temp_t_to_draw;

	HBITMAP store_bmp;
	SetBkMode(main_dc4,TRANSPARENT);
	store_bmp = (HBITMAP) SelectObject(main_dc4,DibBitmapHandle(terrain_buttons_gworld));

	fill_rect_in_gworld(main_dc4,to_rect,255,255,255);

	SelectObject(main_dc4,store_bmp);

	//use cutaway when possible, so user can see better
	//but not in sidebar
	//a = scen_data.scen_ter_types[t_to_draw].cut_away_pic;
	//if (a.not_legit())
	a = scen_data.scen_ter_types[t_to_draw].pic;

	//walls
	if(t_to_draw >= 2 && t_to_draw <= 73) {
		if(editing_town == FALSE) {
			a.which_sheet = ((current_terrain.is_on_surface) ? 616 : 614);
		}
		else if(t_to_draw >= 2 && t_to_draw <= 37) {
			a.which_sheet = town.wall_1_sheet;
		}
		else if(t_to_draw >= 38 && t_to_draw <= 73) {
			a.which_sheet = town.wall_2_sheet;
		}
	}
	//draw L (2-side) walls
	if(t_to_draw == 6 || t_to_draw == 42) {//NW
		a.which_icon = 5;
		if (a.not_legit() == FALSE)
			if (place_icon_into_3D_sidebar(a,to_rect,scen_data.scen_ter_types[t_to_draw].icon_offset_x,
			scen_data.scen_ter_types[t_to_draw].icon_offset_y - 10) == FALSE)
				cant_draw_graphics_error(a,"Error was for terrain type",t_to_draw);
	}
	else if(t_to_draw == 7 || t_to_draw == 43) {//SW
		temp_t_to_draw = t_to_draw;
		temp_t_to_draw = 3;//west
		a.which_icon = 1;
		if (a.not_legit() == FALSE)
			if (place_icon_into_3D_sidebar(a,to_rect,scen_data.scen_ter_types[temp_t_to_draw].icon_offset_x,
			scen_data.scen_ter_types[temp_t_to_draw].icon_offset_y) == FALSE)
				cant_draw_graphics_error(a,"Error was for terrain type",t_to_draw);
		temp_t_to_draw = 4;//south
		a.which_icon = 0;
		if (a.not_legit() == FALSE)
			if (place_icon_into_3D_sidebar(a,to_rect,scen_data.scen_ter_types[temp_t_to_draw].icon_offset_x,
			scen_data.scen_ter_types[temp_t_to_draw].icon_offset_y) == FALSE)
				cant_draw_graphics_error(a,"Error was for terrain type",t_to_draw);
	}
	else if(t_to_draw == 8 || t_to_draw == 44) {//SE
		a.which_icon = 4;
		if (a.not_legit() == FALSE)
			if (place_icon_into_3D_sidebar(a,to_rect,scen_data.scen_ter_types[t_to_draw].icon_offset_x,
			scen_data.scen_ter_types[t_to_draw].icon_offset_y) == FALSE)
				cant_draw_graphics_error(a,"Error was for terrain type",t_to_draw);
	}
	else if(t_to_draw == 9 || t_to_draw == 45) {//NE
		temp_t_to_draw = t_to_draw;
		temp_t_to_draw = 2;//north wall
		a.which_icon = 0;
		if (a.not_legit() == FALSE)
			if (place_icon_into_3D_sidebar(a,to_rect,scen_data.scen_ter_types[temp_t_to_draw].icon_offset_x,
			scen_data.scen_ter_types[temp_t_to_draw].icon_offset_y) == FALSE)
				cant_draw_graphics_error(a,"Error was for terrain type",t_to_draw);
		temp_t_to_draw = 5;//east wall
		a.which_icon = 1;
		if (a.not_legit() == FALSE)
			if (place_icon_into_3D_sidebar(a,to_rect,scen_data.scen_ter_types[temp_t_to_draw].icon_offset_x,
			scen_data.scen_ter_types[temp_t_to_draw].icon_offset_y) == FALSE)
				cant_draw_graphics_error(a,"Error was for terrain type",t_to_draw);
	}
	else {
		if (a.not_legit() == FALSE)
			if (place_icon_into_3D_sidebar(a,to_rect,scen_data.scen_ter_types[t_to_draw].icon_offset_x,
			scen_data.scen_ter_types[t_to_draw].icon_offset_y) == FALSE)
				cant_draw_graphics_error(a,"Error was for terrain type",t_to_draw);
		//draw second icon if it exists
/*		a.which_icon = scen_data.scen_ter_types[t_to_draw].second_icon;
		if (a.not_legit() == FALSE)
			if (place_icon_into_ter_3D_large(a,at_point_center_x + scen_data.scen_ter_types[t_to_draw].second_icon_offset_x,
			at_point_center_y + scen_data.scen_ter_types[t_to_draw].second_icon_offset_y) == FALSE)
				cant_draw_graphics_error(a,"Error was for terrain type",t_to_draw);
*/		
	}
}

Boolean place_icon_into_3D_sidebar(graphic_id_type icon, RECT to_rect, short unscaled_offset_x, short unscaled_offset_y)
{
	RECT from_rect;
	graphic_id_type a = icon;
	
	//It seems one of the terrain graphics sheets is covered up by (same resource number as) an editor graphic.
	//So I had to copy that sheet into 3D editor graphics and make this horrible hack (in multiple places!)
	if(a.which_sheet == 701)
		a.which_sheet = 4916;
	
	OffsetRect(&to_rect,(unscaled_offset_x * TER_BUTTON_SIZE) / PICT_BOX_WIDTH_3D,
						(unscaled_offset_y * TER_BUTTON_SIZE) / PICT_BOX_HEIGHT_3D);
	
	short index = safe_get_index_of_sheet(&a);
	if (index < 0) {
		return FALSE;	
	}
	SetRECT(from_rect,1 + (PICT_BOX_WIDTH_3D + 1) * (a.which_icon % 10),1 + (PICT_BOX_HEIGHT_3D + 1) * (a.which_icon / 10),
	  1 + (PICT_BOX_WIDTH_3D + 1) * (a.which_icon % 10) + PICT_BOX_WIDTH_3D,1 + (PICT_BOX_HEIGHT_3D + 1) * (a.which_icon / 10) + PICT_BOX_HEIGHT_3D);

	HDIB src_gworld = graphics_library[index];
	if ( from_rect.top >= DibHeight( src_gworld ) )		// check the icon exists on the sheet, simplified version
		return TRUE;		// actually FALSE

	adjust_graphic(&src_gworld,&from_rect,a.graphic_adjust);
	
	rect_draw_some_item(src_gworld,from_rect,terrain_buttons_gworld,to_rect,1,0);
	return TRUE;
}

Boolean place_icon_into_ter_3D_large(graphic_id_type icon,short at_point_center_x,short at_point_center_y,RECT *to_whole_area_rect,short lighting)
{
	RECT to_rect;
	RECT from_rect;
	graphic_id_type a;
	
	SetRECT(to_rect,at_point_center_x,at_point_center_y,
		at_point_center_x + PICT_BOX_WIDTH_3D,at_point_center_y + PICT_BOX_HEIGHT_3D);
	OffsetRect(&to_rect, -PICT_BOX_WIDTH_3D / 2, -PICT_BOX_HEIGHT_3D / 2);
	
	if(!rects_touch(&to_rect,to_whole_area_rect))
		return TRUE;
	
	a = icon;
	//It seems one of the terrain graphics sheets is covered up by (same resource number as) an editor graphic.
	//So I had to copy that sheet into 3D editor graphics and make this horrible hack (in multiple places!)
	if(a.which_sheet == 701)
		a.which_sheet = 4916;
	
	short index = safe_get_index_of_sheet(&a);
	if (index < 0) {
		//cant_draw_graphics_error(a);
		return FALSE;	
		}		
	SetRECT(from_rect,1 + (PICT_BOX_WIDTH_3D + 1) * (a.which_icon % 10),1 + (PICT_BOX_HEIGHT_3D + 1) * (a.which_icon / 10),
	  1 + (PICT_BOX_WIDTH_3D + 1) * (a.which_icon % 10) + PICT_BOX_WIDTH_3D,1 + (PICT_BOX_HEIGHT_3D + 1) * (a.which_icon / 10) + PICT_BOX_HEIGHT_3D);
	
	HDIB src_gworld = graphics_library[index];
	adjust_graphic(&src_gworld,&from_rect,a.graphic_adjust);
	apply_lighting_to_graphic(&src_gworld,&from_rect,lighting);
	
	rect_draw_some_item(src_gworld,from_rect,ter_draw_gworld,to_rect,1,0);
	return TRUE;
}

Boolean place_creature_icon_into_ter_3D_large(graphic_id_type icon,short at_point_center_x,short at_point_center_y,RECT *to_whole_area_rect,short lighting,short r,short g,short b)
{
	RECT to_rect;
	RECT from_rect;
	graphic_id_type a;
	
	SetRECT(to_rect,at_point_center_x,at_point_center_y,
		at_point_center_x + PICT_BOX_WIDTH_3D,at_point_center_y + PICT_BOX_HEIGHT_3D);
	OffsetRect(&to_rect, -PICT_BOX_WIDTH_3D / 2, -PICT_BOX_HEIGHT_3D / 2);
	
	if(!rects_touch(&to_rect,to_whole_area_rect))
		return TRUE;
	
	a = icon;
	//It seems one of the terrain graphics sheets is covered up by (same resource number as) an editor graphic.
	//So I had to copy that sheet into 3D editor graphics and make this horrible hack (in multiple places!)
	if(a.which_sheet == 701)
		a.which_sheet = 4916;
	
	short index = safe_get_index_of_sheet(&a);
	if (index < 0) {
		//cant_draw_graphics_error(a);
		return FALSE;	
		}		
	SetRECT(from_rect,1 + (PICT_BOX_WIDTH_3D + 1) * (a.which_icon % 10),1 + (PICT_BOX_HEIGHT_3D + 1) * (a.which_icon / 10),
	  1 + (PICT_BOX_WIDTH_3D + 1) * (a.which_icon % 10) + PICT_BOX_WIDTH_3D,1 + (PICT_BOX_HEIGHT_3D + 1) * (a.which_icon / 10) + PICT_BOX_HEIGHT_3D);
	
	HDIB src_gworld = graphics_library[index];
	adjust_graphic(&src_gworld,&from_rect,a.graphic_adjust);
	apply_lighting_to_graphic(&src_gworld,&from_rect,lighting);
	if(cur_viewing_mode != 11)
		add_border_to_graphic(&src_gworld,&from_rect,r,g,b);
	
	rect_draw_some_item(src_gworld,from_rect,ter_draw_gworld,to_rect,1,0);
	return TRUE;
}

Boolean place_cliff_icon_into_ter_3D_large(short sheet,short at_point_center_x,short at_point_center_y,	short direction,RECT *to_whole_area_rect,short lighting)//direction:   0: east/west 1: NW/SE 2:north/south
{
	RECT to_rect;
	RECT from_rect;
	graphic_id_type a;
	
	SetRECT(to_rect,at_point_center_x,at_point_center_y,
		at_point_center_x + PICT_BOX_WIDTH_3D,at_point_center_y + PICT_BOX_HEIGHT_3D);
	OffsetRect(&to_rect, -PICT_BOX_WIDTH_3D / 2, -PICT_BOX_HEIGHT_3D / 2);
	
	if(!rects_touch(&to_rect,to_whole_area_rect))
		return TRUE;
	
	a.which_sheet = sheet;
	if(direction == 2) {//north/south
		a.which_icon = 0;
	}
	else {
		a.which_icon = 1;
	}
	short index = safe_get_index_of_sheet(&a);
	if (index < 0) {
		//cant_draw_graphics_error(a);
		return FALSE;	
		}		
	SetRECT(from_rect,1 + (PICT_BOX_WIDTH_3D + 1) * (a.which_icon % 10),1 + (PICT_BOX_HEIGHT_3D + 1) * (a.which_icon / 10),
	  1 + (PICT_BOX_WIDTH_3D + 1) * (a.which_icon % 10) + PICT_BOX_WIDTH_3D,1 + (PICT_BOX_HEIGHT_3D + 1) * (a.which_icon / 10) + PICT_BOX_HEIGHT_3D);
	
	if(direction == 1) {//NW/SE
		from_rect.right = from_rect.right - PICT_BOX_WIDTH_3D + 4;
		to_rect.right = to_rect.right - PICT_BOX_WIDTH_3D + 4;
		OffsetRect(&to_rect,22,0);
	}
	else if(direction == 0) {//east/west
		from_rect.left = from_rect.left + 4;
		to_rect.left = to_rect.left + 4;
	}
	
	HDIB src_gworld = graphics_library[index];
	apply_lighting_to_graphic(&src_gworld,&from_rect,lighting);
	
	rect_draw_some_item(src_gworld,from_rect,ter_draw_gworld,to_rect,1,0);
	return TRUE;
}

Boolean place_item_icon_into_ter_3D_large(graphic_id_type icon,short at_point_center_x,short at_point_center_y,RECT *to_whole_area_rect,short lighting)
{
	RECT to_rect;
	RECT from_rect;
	graphic_id_type a;
	
	SetRECT(to_rect,at_point_center_x,at_point_center_y,
		at_point_center_x + ITEM_BOX_SIZE_3D,at_point_center_y + ITEM_BOX_SIZE_3D);
	OffsetRect(&to_rect, -ITEM_BOX_SIZE_3D / 2, -ITEM_BOX_SIZE_3D / 2);
	
	if(!rects_touch(&to_rect,to_whole_area_rect))
		return TRUE;
	
	a = icon;
	short index = safe_get_index_of_sheet(&a);
	if (index < 0) {
		//cant_draw_graphics_error(a);
		return FALSE;	
		}		
	SetRECT(from_rect,1 + (ITEM_BOX_SIZE_3D + 1) * (a.which_icon % 10),1 + (ITEM_BOX_SIZE_3D + 1) * (a.which_icon / 10),
	  1 + (ITEM_BOX_SIZE_3D + 1) * (a.which_icon % 10) + ITEM_BOX_SIZE_3D,1 + (ITEM_BOX_SIZE_3D + 1) * (a.which_icon / 10) + ITEM_BOX_SIZE_3D);
	
	HDIB src_gworld = graphics_library[index];
	adjust_graphic(&src_gworld,&from_rect,a.graphic_adjust);
	apply_lighting_to_graphic(&src_gworld,&from_rect,lighting);
	
	rect_draw_some_item(src_gworld,from_rect,ter_draw_gworld,to_rect,1,0);
	return TRUE;
}

Boolean place_outdoor_creature_icon_into_ter_3D_large(graphic_id_type icon,short at_point_center_x,short at_point_center_y,RECT *to_whole_area_rect,short lighting)
{
	RECT to_rect;
	RECT from_rect;
	graphic_id_type a;
	
	SetRECT(to_rect,at_point_center_x,at_point_center_y + PICT_BOX_HEIGHT_3D - OUTDOOR_CREATURE_HEIGHT_3D,
		at_point_center_x + OUTDOOR_CREATURE_WIDTH_3D,at_point_center_y + PICT_BOX_HEIGHT_3D);
	OffsetRect(&to_rect, -PICT_BOX_WIDTH_3D / 2, -PICT_BOX_HEIGHT_3D / 2);
	
	if(!rects_touch(&to_rect,to_whole_area_rect))
		return TRUE;
	
	a = icon;
	short index = safe_get_index_of_sheet(&a);
	if (index < 0) {
		//cant_draw_graphics_error(a);
		return FALSE;	
		}		
	SetRECT(from_rect,1 + (PICT_BOX_WIDTH_3D + 1) * (a.which_icon % 4),1 + (PICT_BOX_HEIGHT_3D + 1) * (a.which_icon / 4),
	  1 + (PICT_BOX_WIDTH_3D + 1) * (a.which_icon % 4) + OUTDOOR_CREATURE_WIDTH_3D,1 + (PICT_BOX_HEIGHT_3D + 1) * (a.which_icon / 4) + OUTDOOR_CREATURE_HEIGHT_3D);
	
	HDIB src_gworld = graphics_library[index];
	adjust_graphic(&src_gworld,&from_rect,a.graphic_adjust);
	apply_lighting_to_graphic(&src_gworld,&from_rect,lighting);
	
	rect_draw_some_item(src_gworld,from_rect,ter_draw_gworld,to_rect,1,0);
	return TRUE;
}


Boolean place_corner_wall_icon_into_ter_3D_large(graphic_id_type icon,short at_point_center_x,short at_point_center_y,Boolean left_side_of_template,RECT *to_whole_area_rect,short lighting)
{
	RECT to_rect;
	RECT from_rect;
	graphic_id_type a;
	
	SetRECT(to_rect,at_point_center_x,at_point_center_y,
		at_point_center_x + PICT_BOX_WIDTH_3D,at_point_center_y + PICT_BOX_HEIGHT_3D);
	OffsetRect(&to_rect, -PICT_BOX_WIDTH_3D / 2, -PICT_BOX_HEIGHT_3D / 2);
	
	if(!rects_touch(&to_rect,to_whole_area_rect))
		return TRUE;
	
	a = icon;
	short index = safe_get_index_of_sheet(&a);
	if (index < 0) {
		//cant_draw_graphics_error(a);
		return FALSE;	
		}		
	SetRECT(from_rect,1 + (PICT_BOX_WIDTH_3D + 1) * (a.which_icon % 10),1 + (PICT_BOX_HEIGHT_3D + 1) * (a.which_icon / 10),
	  1 + (PICT_BOX_WIDTH_3D + 1) * (a.which_icon % 10) + PICT_BOX_WIDTH_3D,1 + (PICT_BOX_HEIGHT_3D + 1) * (a.which_icon / 10) + PICT_BOX_HEIGHT_3D);
	
	if(left_side_of_template) {
		from_rect.right -= PICT_BOX_WIDTH_3D / 2;
		to_rect.right -= PICT_BOX_WIDTH_3D / 2;
	}
	else {
		from_rect.left += PICT_BOX_WIDTH_3D / 2;
		to_rect.left += PICT_BOX_WIDTH_3D / 2;
	}
	
	HDIB src_gworld = graphics_library[index];
	apply_lighting_to_graphic(&src_gworld,&from_rect,lighting);
	
	rect_draw_some_item(src_gworld,from_rect,ter_draw_gworld,to_rect,1,0);
	return TRUE;
}

void place_ter_icon_on_tile_3D(short at_point_center_x,short at_point_center_y,short position,short which_icon,RECT *to_whole_area_rect)
{
	//too bad, there's no room left
	if(position > 9)
		return;
	
	RECT tiny_to = {at_point_center_x - 8, at_point_center_y - 4, at_point_center_x -8 + 16, at_point_center_y - 4 + 11};
	OffsetRect(&tiny_to,-8 * (position % 3) + 8 * (position / 3),6 * (position % 3) + 6 * (position / 3));
	
	if(!rects_touch(&tiny_to,to_whole_area_rect))
		return;
	
	RECT tiny_from = base_small_3D_button_from;
	OffsetRect(&tiny_from,16 * (which_icon % 10),11 * (which_icon / 10));
	
	rect_draw_some_item(editor_mixed,tiny_from,ter_draw_gworld,tiny_to,1,0);
}

void draw_ter_script_3D(short at_point_center_x,short at_point_center_y,RECT *to_whole_area_rect)
{
	RECT ter_script_icon_from = {160,143,183,157};
	
	RECT to_rect = {at_point_center_x, at_point_center_y + 7, at_point_center_x + 23, at_point_center_y + 7 + 14};
	
	if(rects_touch(&to_rect,to_whole_area_rect))
		rect_draw_some_item(editor_mixed,ter_script_icon_from,ter_draw_gworld,to_rect,1,0);
}

void place_ter_icons_3D(location which_outdoor_sector, outdoor_record_type *drawing_terrain, short square_x, short square_y,short t_to_draw, short floor_to_draw, short at_point_center_x,short at_point_center_y,RECT *to_whole_area_rect)
{
	location loc_drawn;
	loc_drawn.x = (char)square_x;
	loc_drawn.y = (char)square_y;
	short i;
	short small_icon_position = 0;
	
	
	if (editing_town) {
		// draw ter scripts
		for (i = 0; i < NUM_TER_SCRIPTS; i++) {
			if (town.ter_scripts[i].exists && same_point(town.ter_scripts[i].loc,loc_drawn) ) {
				draw_ter_script_3D(at_point_center_x,at_point_center_y,to_whole_area_rect);
			}
		}
	}
						
	// now place the tiny icons in the lower right corner
	// first, stuff that is done for both town and outdoors
	/*
	//I don't think either of these is necessary for users understanding.  Their only 'purpose' is to look bad.
	// signs
	if (scen_data.scen_ter_types[t_to_draw].special == 39) {
		place_ter_icon_on_tile_3D(at_point_center_x,at_point_center_y,small_icon_position,20);
		small_icon_position++;
	}			
	//containers
	if (scen_data.scen_ter_types[t_to_draw].special == 40) {
		place_ter_icon_on_tile_3D(at_point_center_x,at_point_center_y,small_icon_position,21);
		small_icon_position++;
	}			*/
	
	// icons for secret doors
	if (((t_to_draw >= 18) && (t_to_draw <= 21)) || ((t_to_draw >= 54) && (t_to_draw <= 57))) {
		place_ter_icon_on_tile_3D(at_point_center_x,at_point_center_y,small_icon_position,26,to_whole_area_rect);
		small_icon_position++;
	}
	// icons for floor damage
	if ((scen_data.scen_ter_types[t_to_draw].special == 1) || 
	  (scen_data.scen_floors[floor_to_draw].special == 1)) {
		place_ter_icon_on_tile_3D(at_point_center_x,at_point_center_y,small_icon_position,37,to_whole_area_rect);
		small_icon_position++;
	}
	if ((scen_data.scen_ter_types[t_to_draw].special == 2) || 
	  (scen_data.scen_floors[floor_to_draw].special == 2)) {
		place_ter_icon_on_tile_3D(at_point_center_x,at_point_center_y,small_icon_position,38,to_whole_area_rect);
		small_icon_position++;
	}
	if ((scen_data.scen_ter_types[t_to_draw].special == 3) || 
	  (scen_data.scen_floors[floor_to_draw].special == 3)) {
		place_ter_icon_on_tile_3D(at_point_center_x,at_point_center_y,small_icon_position,39,to_whole_area_rect);
		small_icon_position++;
	}
	
	if ((scen_data.scen_ter_types[t_to_draw].special == 6) || 
	  (scen_data.scen_floors[floor_to_draw].special == 6)) {
		place_ter_icon_on_tile_3D(at_point_center_x,at_point_center_y,small_icon_position,22,to_whole_area_rect);
		small_icon_position++;
	}
	
	// then town only tiny icons
	if (editing_town) {
		for (i = 0; i < 4; i++) {
			if (same_point(loc_drawn,town.start_locs[i])) {
				place_ter_icon_on_tile_3D(at_point_center_x,at_point_center_y,small_icon_position,i,to_whole_area_rect);
				small_icon_position++;
			}	
		}

		for (i = 0; i < NUM_WAYPOINTS; i++) {
			if ((town.waypoints[i].x < 255) && (same_point(loc_drawn,town.waypoints[i]))) {
				place_ter_icon_on_tile_3D(at_point_center_x,at_point_center_y,small_icon_position,10 + i,to_whole_area_rect);
				small_icon_position++;
			}	
		}

		for (i = 0; i < 6; i++)	{	
			if (same_point(loc_drawn,town.respawn_locs[i])) {
				place_ter_icon_on_tile_3D(at_point_center_x,at_point_center_y,small_icon_position,24,to_whole_area_rect);
				small_icon_position++;
			}	
		}

		// start scenario icon
		if ((scenario.start_in_what_town == cur_town) && (same_point(loc_drawn,scenario.what_start_loc_in_town))) {
			place_ter_icon_on_tile_3D(at_point_center_x,at_point_center_y,small_icon_position,23,to_whole_area_rect);
			small_icon_position++;
		}

		// blocked spaces
		if (is_blocked(loc_drawn.x,loc_drawn.y)) {
			place_ter_icon_on_tile_3D(at_point_center_x,at_point_center_y,small_icon_position,22,to_whole_area_rect);
			small_icon_position++;
		}

	}
			
	// Outdoor mode: draw tiny icons
	if (editing_town == FALSE) {
		
		
		// WANDERING MONST ICON
		for (i = 0; i < 4; i++) {
			if (same_point(loc_drawn,drawing_terrain->wandering_locs[i])) {
				place_ter_icon_on_tile_3D(at_point_center_x,at_point_center_y,small_icon_position,24,to_whole_area_rect);
				small_icon_position++;
			}	
		}
		
		// start scenario icon
		if ((same_point(which_outdoor_sector,scenario.what_outdoor_section_start_in)) && 
		  (same_point(loc_drawn,scenario.start_where_in_outdoor_section))) {
			place_ter_icon_on_tile_3D(at_point_center_x,at_point_center_y,small_icon_position,23,to_whole_area_rect);
			small_icon_position++;
		}
		
		// Preset MONST ICON
		//nope, they're actually drawn instead of getting a tiny icon
		//unless the other icons cover up the drawn monsters
		//currently, preset monsters are drawn on top
		/*for (i = 0; i < 8; i++) {
			if ((drawing_terrain->preset[i].start_loc.x > 0) &&
			(same_point(loc_drawn,drawing_terrain->preset[i].start_loc))) {
				if(small_icon_position >= 4 && drawing_terrain->preset[i].hostile_amount[0] < 4) {
					place_ter_icon_on_tile_3D(at_point_center_x,at_point_center_y,small_icon_position,25,to_whole_area_rect);
					small_icon_position++;
				}
				else if(small_icon_position >= 6) {
					place_ter_icon_on_tile_3D(at_point_center_x,at_point_center_y,small_icon_position,25,to_whole_area_rect);
					small_icon_position++;
				}
			}
		}*/
	}
}

void draw_ter_icon_3D(short terrain_number,short icon_number,short x,short y,graphic_id_type a,short t_to_draw,RECT *to_whole_area_rect,short lighting,short height)
{
	a.which_icon = icon_number;
	short i;
	if (a.not_legit() == FALSE) {
		for(i = 0; i < height; i++) {
			if (place_icon_into_ter_3D_large(a,x + scen_data.scen_ter_types[terrain_number].icon_offset_x,
			y + scen_data.scen_ter_types[terrain_number].icon_offset_y - i * 35,to_whole_area_rect,lighting) == FALSE) {
				cant_draw_graphics_error(a,"Error was for terrain type",t_to_draw);
				break;
			}
		}
	}
}

void draw_terrain_3D(short t_to_draw, short x, short y, short sector_x, short sector_y, short at_x, short at_y, Boolean see_in_neighbors[3][3], Boolean is_wall_corner,RECT *to_whole_area_rect,short lighting)
{			
	graphic_id_type a;
	short temp_t_to_draw;
	short height = 1;
	short i;
	Boolean cutaway = FALSE;
	
	short cutHeight = height;

	if(cur_viewing_mode == 10) {
		//use cutaway when possible, so user can see better
		a = scen_data.scen_ter_types[t_to_draw].cut_away_pic;
		cutaway = TRUE;
	}
	else {
		if(see_in_neighbors[1][1] == FALSE) {
			//If the space can't be seen, the terrain's only chance for display is blocking the party's sight.
			//They block the party's sight only if the party could see to the space that they're blocking sight from.
			//If they successfully block the party's sight at all, then they're displayed.
			if(!(
			(scen_data.scen_ter_types[t_to_draw].see_block[0] == 1 && see_in_neighbors[1][0] == TRUE) ||
			(scen_data.scen_ter_types[t_to_draw].see_block[1] == 1 && see_in_neighbors[0][1] == TRUE) ||
			(scen_data.scen_ter_types[t_to_draw].see_block[2] == 1 && see_in_neighbors[1][2] == TRUE) ||
			(scen_data.scen_ter_types[t_to_draw].see_block[3] == 1 && see_in_neighbors[2][1] == TRUE)
			||
			(/*poor, foolish BoA that I have to emulate...*/
				is_wall_corner && scen_data.scen_ter_types[t_to_draw].see_block[0] == 0 && 
				scen_data.scen_ter_types[t_to_draw].see_block[1] == 0 && 
				scen_data.scen_ter_types[t_to_draw].see_block[2] == 0 &&
				scen_data.scen_ter_types[t_to_draw].see_block[3] == 0
			)
			))
				return;
		}
		//use cutaway?
		if((scen_data.scen_ter_types[t_to_draw].blocks_view[0] && see_in_neighbors[1][0]) ||
		(scen_data.scen_ter_types[t_to_draw].blocks_view[1] && see_in_neighbors[0][1]) ||
		(scen_data.scen_ter_types[t_to_draw].blocks_view[2] && see_in_neighbors[1][1]) ||
		(scen_data.scen_ter_types[t_to_draw].blocks_view[3] && see_in_neighbors[1][1]) ||
		(editing_town == FALSE && (
			(scen_data.scen_ter_types[t_to_draw].blocks_view[0] && see_in_neighbors[0][0]) ||
			(scen_data.scen_ter_types[t_to_draw].blocks_view[1] && see_in_neighbors[0][0])
		))
		) {
			a = scen_data.scen_ter_types[t_to_draw].cut_away_pic;
			cutaway = TRUE;
		}
		else
			a = scen_data.scen_ter_types[t_to_draw].pic;
	}
	//if there is no cutaway pic, use the normal one
	if (a.not_legit())
		a = scen_data.scen_ter_types[t_to_draw].pic;
	
	//walls
	if(t_to_draw >= 2 && t_to_draw <= 73) {
		if(cur_viewing_mode == 11) {
			//sometimes walls next to slopes aren't displayed, in order to make it look better
			short other_terrain = 0, special;
			//if it blocks moves to the north
			if(scen_data.scen_ter_types[t_to_draw].move_block[0] == 1) {
				//if the requested other terrain exists
				if(!(
				(editing_town && (y == 0))
				|| 
				(!editing_town && (y == 0 && (sector_y == 0 || sector_y - cur_out.y == -1)))
				)) {
					other_terrain = (editing_town) ? t_d.terrain[x][y - 1] : 
					border_terrains[sector_x - cur_out.x + 1][sector_y - cur_out.y + 1 - ((y == 0) ? 1 : 0)].terrain[x][(y == 0) ? 47 : y - 1];
					
					special = scen_data.scen_ter_types[other_terrain].special;
					if(special == 19 || special == 20 || special == 22 || special == 23 || special == 27 || special == 30)
						return;
				}
			}
			//if it blocks moves to the west
			if(scen_data.scen_ter_types[t_to_draw].move_block[1] == 1) {
				//if the requested other terrain exists
				if(!(
				(editing_town && (x == 0))
				|| 
				(!editing_town && (x == 0 && (sector_x == 0 || sector_x - cur_out.x == -1)))
				)) {
					other_terrain = (editing_town) ? t_d.terrain[x - 1][y] : 
					border_terrains[sector_x - cur_out.x + 1 - ((x == 0) ? 1 : 0)][sector_y - cur_out.y + 1].terrain[(x == 0) ? 47 : x - 1][y];
					
					special = scen_data.scen_ter_types[other_terrain].special;
					if(special == 21 || special == 22 || special == 24 || special == 25 || special == 27 || special == 28)
						return;
				}
			}
		}
		if(editing_town == FALSE) {
			//yes, even if it's a neigboring sector with a different is_on_surface value, it gets displayed like the current sector
			//not that that situation would happen
			a.which_sheet = ((current_terrain.is_on_surface) ? 616 : 614);
			height = ((t_to_draw >= 2 && t_to_draw <= 37) ? 2 : 1);
		}
		else if(t_to_draw >= 2 && t_to_draw <= 37) {
			a.which_sheet = town.wall_1_sheet;
			height = town.wall_1_height;
		}
		else if(t_to_draw >= 38 && t_to_draw <= 73) {
			a.which_sheet = town.wall_2_sheet;
			height = town.wall_2_height;
		}
		cutHeight = height;
		if(cutaway)
			cutHeight = 1;
	}
	//draw L (2-side) walls
	temp_t_to_draw = t_to_draw;
	if(t_to_draw == 6 || t_to_draw == 42) {//NW - that's why there are two cutaway walls! for this
		if(cur_viewing_mode != 11 || (see_in_neighbors[0][1] && see_in_neighbors[1][0])) {
			draw_ter_icon_3D(3/*west*/, 3, at_x, at_y, a, t_to_draw, to_whole_area_rect, lighting, cutHeight);
			draw_ter_icon_3D(2/*north*/, 6, at_x + 4, at_y + 3, a, t_to_draw, to_whole_area_rect, lighting, cutHeight);
		}
		else if(see_in_neighbors[0][1]) {//cutaway the west wall, but not the north one
			if(see_in_neighbors[1][1]) {
				draw_ter_icon_3D(2/*north*/, 0, at_x, at_y, a, t_to_draw, to_whole_area_rect, lighting, height);
				draw_ter_icon_3D(3/*west*/, 7, at_x + 3, at_y - 5, a, t_to_draw, to_whole_area_rect, lighting, cutHeight);
			}
			else {
				draw_ter_icon_3D(3/*west*/, 3, at_x, at_y, a, t_to_draw, to_whole_area_rect, lighting, height);
			}
		}
		else if(see_in_neighbors[1][0]) {//cutaway the north wall, but not the west one
			if(see_in_neighbors[1][1]) {
				draw_ter_icon_3D(3/*west*/, 1, at_x, at_y, a, t_to_draw, to_whole_area_rect, lighting, height);
				draw_ter_icon_3D(2/*north*/, 6, at_x + 4, at_y + 3, a, t_to_draw, to_whole_area_rect, lighting, cutHeight);
			}
			else {
				draw_ter_icon_3D(2/*north*/, 2, at_x, at_y, a, t_to_draw, to_whole_area_rect, lighting, height);
			}
		}
		else {//no cutaway
			draw_ter_icon_3D(t_to_draw/*northwest*/, 5, at_x, at_y - 10, a, t_to_draw, to_whole_area_rect, lighting, height);
		}
	}
	else if(t_to_draw == 7 || t_to_draw == 43) {//SW
		if(cur_viewing_mode != 11 || (see_in_neighbors[0][1] && see_in_neighbors[1][1])) {
			draw_ter_icon_3D(3/*west*/, 3, at_x, at_y, a, t_to_draw, to_whole_area_rect, lighting, cutHeight);
			draw_ter_icon_3D(4/*south*/, 2, at_x, at_y, a, t_to_draw, to_whole_area_rect, lighting, cutHeight);
		}
		else if(see_in_neighbors[0][1]) {//only have to draw the south wall for this - cutaway west wall would be covered up! (so what?)
			if(see_in_neighbors[1][2]) {
				draw_ter_icon_3D(3/*west*/, 3, at_x, at_y, a, t_to_draw, to_whole_area_rect, lighting, cutHeight);
				draw_ter_icon_3D(4/*south*/, 0, at_x, at_y, a, t_to_draw, to_whole_area_rect, lighting, height);
			}
			else {
				draw_ter_icon_3D(3/*west*/, 3, at_x, at_y, a, t_to_draw, to_whole_area_rect, lighting, height);
			}
		}
		else if(see_in_neighbors[1][1]) {//cutaway south wall but not west
			draw_ter_icon_3D(3/*west*/, 1, at_x, at_y, a, t_to_draw, to_whole_area_rect, lighting, height);
			draw_ter_icon_3D(4/*south*/, 6, at_x + 4, at_y + 3, a, t_to_draw, to_whole_area_rect, lighting, cutHeight);
		}
		else {//no cutaway, west side not visible
			draw_ter_icon_3D(4/*south*/, 0, at_x, at_y, a, t_to_draw, to_whole_area_rect, lighting, height);
		}
	}
	else if(t_to_draw == 8 || t_to_draw == 44) {//SE - that's why there are two cutaway walls! for this
		if(cur_viewing_mode != 11 || see_in_neighbors[1][1]) {
			draw_ter_icon_3D(5/*east*/, 7, at_x, at_y, a, t_to_draw, to_whole_area_rect, lighting, cutHeight);
			draw_ter_icon_3D(4/*south*/, 2, at_x, at_y, a, t_to_draw, to_whole_area_rect, lighting, cutHeight);
		}
		else {
			if(see_in_neighbors[1][2] && see_in_neighbors[2][1]) {
				draw_ter_icon_3D(t_to_draw/*southeast*/, 4, at_x, at_y, a, t_to_draw, to_whole_area_rect, lighting, height);
			}
			else if(see_in_neighbors[1][2]) {
				draw_ter_icon_3D(4/*south*/, 0, at_x, at_y, a, t_to_draw, to_whole_area_rect, lighting, height);
			}
			else if(see_in_neighbors[2][1]) {
				draw_ter_icon_3D(5/*east*/, 1, at_x, at_y, a, t_to_draw, to_whole_area_rect, lighting, height);
			}
		}
	}
	else if(t_to_draw == 9 || t_to_draw == 45) {//NE
		if(cur_viewing_mode != 11 || (see_in_neighbors[1][0] && see_in_neighbors[1][1])) {
			draw_ter_icon_3D(2/*north*/, 2, at_x, at_y, a, t_to_draw, to_whole_area_rect, lighting, cutHeight);
			draw_ter_icon_3D(5/*east*/, 3, at_x, at_y, a, t_to_draw, to_whole_area_rect, lighting, cutHeight);
		}
		else if(see_in_neighbors[1][0]) {//only have to draw the east wall for this - cutaway north wall would be covered up! (so what?)
			if(see_in_neighbors[2][1]) {
				draw_ter_icon_3D(2/*north*/, 2, at_x, at_y, a, t_to_draw, to_whole_area_rect, lighting, cutHeight);
				draw_ter_icon_3D(5/*east*/, 1, at_x, at_y, a, t_to_draw, to_whole_area_rect, lighting, height);
			}
			else {
				draw_ter_icon_3D(2/*north*/, 2, at_x, at_y, a, t_to_draw, to_whole_area_rect, lighting, height);
			}
		}
		else if(see_in_neighbors[1][1]) {//cutaway east wall but not north
			draw_ter_icon_3D(2/*north*/, 0, at_x, at_y, a, t_to_draw, to_whole_area_rect, lighting, height);
			draw_ter_icon_3D(5/*east*/, 7, at_x - 4, at_y + 3, a, t_to_draw, to_whole_area_rect, lighting, cutHeight);
		}
		else {//no cutaway, north side not visible
			draw_ter_icon_3D(5/*east*/, 1, at_x, at_y, a, t_to_draw, to_whole_area_rect, lighting, height);
		}
	}
	else {
		if (a.not_legit() == FALSE) {
			for(i = 0; i < cutHeight; i++) {
				if (place_icon_into_ter_3D_large(a,at_x + scen_data.scen_ter_types[t_to_draw].icon_offset_x,
				at_y + scen_data.scen_ter_types[t_to_draw].icon_offset_y - i * 35,to_whole_area_rect, lighting) == FALSE) {
					cant_draw_graphics_error(a,"Error was for terrain type",t_to_draw);
					break;
				}
			}
		}
		//draw second icon if it exists
		a.which_icon = scen_data.scen_ter_types[t_to_draw].second_icon;
		if (a.not_legit() == FALSE)
			if (place_icon_into_ter_3D_large(a,at_x + scen_data.scen_ter_types[t_to_draw].second_icon_offset_x,
			at_y + scen_data.scen_ter_types[t_to_draw].second_icon_offset_y,to_whole_area_rect, lighting) == FALSE)
				cant_draw_graphics_error(a,"Error was for terrain type",t_to_draw);
	}
}

void draw_creature_3D(short creature_num,short at_point_center_x,short at_point_center_y, short square_x, short square_y,RECT *to_whole_area_rect,short lighting)
{
	graphic_id_type a;
	short r = 0, g = 0, b = 0;
	Boolean hidden;
	
	if (town.creatures[creature_num].exists() == FALSE)
		return;
	
	hidden = town.creatures[creature_num].hidden_class != 0;
	
	switch( (int)(town.creatures[creature_num].start_attitude) ) {
		case 0: case 1: case 2:
			b = 31;
			if(hidden)
				g = 31;
			break;
		case 3:
			if(hidden)
				g = 31;
			break;
		case 4:
			r = 31;
			if(hidden)
				g = 31;
			break;
		case 5:
				r = 23;
				g = 12;
			if(hidden) {
				r = 31;
				g = 0;
				b = 16;
				}
			break;
	}
	/*if (town.creatures[creature_num].start_attitude >= 4) {
		r = 23;
	}
	else if (town.creatures[creature_num].start_attitude < 3)
		b = 31;
	
	if (town.creatures[creature_num].hidden_class > 0) 
		g = 31;*/
	
	a = scen_data.scen_creatures[town.creatures[creature_num].number].char_graphic;
	a.which_icon = town.creatures[creature_num].facing;
	//for creatures sitting in chairs
	if(scen_data.scen_creatures[town.creatures[creature_num].number].small_or_large_template && (t_d.terrain[square_x][square_y] >= 225 && t_d.terrain[square_x][square_y] <= 228)) {
		a.which_icon = t_d.terrain[square_x][square_y] - 225 + 30;
	}
	if (a.not_legit() == FALSE)
		if(place_creature_icon_into_ter_3D_large(a, at_point_center_x,at_point_center_y	- scen_data.scen_ter_types[t_d.terrain[square_x][square_y]].height_adj,to_whole_area_rect,lighting,r,g,b) == FALSE)
			cant_draw_graphics_error(a,"Error was for creature type",town.creatures[creature_num].number);
	
	//draw second (top) icon if it exists (tall creatures like giants)
	a.which_sheet = scen_data.scen_creatures[town.creatures[creature_num].number].char_upper_graphic_sheet;
	a.which_icon = town.creatures[creature_num].facing;
	//for creatures sitting in chairs
	if(scen_data.scen_creatures[town.creatures[creature_num].number].small_or_large_template && (t_d.terrain[square_x][square_y] >= 225 && t_d.terrain[square_x][square_y] <= 228)) {
		a.which_icon = t_d.terrain[square_x][square_y] - 225 + 30;
	}
	if (a.not_legit() == FALSE)
		if(place_creature_icon_into_ter_3D_large(a,at_point_center_x,at_point_center_y - PICT_BOX_HEIGHT_3D,to_whole_area_rect,lighting,r,g,b) == FALSE)
			cant_draw_graphics_error(a,"Error was for creature type",town.creatures[creature_num].number);
	
/*
	//if (same_point(town.creatures[creature_num].start_loc,loc_drawn)) {
	//	Rect base_rect = large_edit_ter_rects[in_square_x][in_square_y];
		from_rect = get_template_from_rect(town.creatures[creature_num].facing,0);
		InsetRect(&from_rect,3,6);
		Rect to_rect = from_rect;
		CenterRectInRect(&to_rect,&base_rect);
		
		short obj_index = safe_get_index_of_sheet(&a);
		if (obj_index < 0) {
			cant_draw_graphics_error(a,"Error was for creature type",town.creatures[creature_num].number);
			}		
			else rect_draw_some_item(graphics_library[obj_index],
				from_rect,ter_draw_gworld,to_rect,0,0);

		if (town.creatures[creature_num].start_attitude >= 4)
			r = 255;
			else if (town.creatures[creature_num].start_attitude < 3)
				b = 255;
		if (town.creatures[creature_num].hidden_class > 0) 
			g = 255;
		put_rect_in_gworld(ter_draw_gworld,to_rect,
		  r,g,b);

		// do facing
		Rect facing_to_rect = to_rect;
		InsetRect(&facing_to_rect,1,1);
		switch (town.creatures[creature_num].facing) {
			case 0: InsetRect(&facing_to_rect,3,0); facing_to_rect.bottom = facing_to_rect.top + 2; break;
			case 1: InsetRect(&facing_to_rect,0,3); facing_to_rect.right = facing_to_rect.left + 2; break;
			case 2: InsetRect(&facing_to_rect,3,0); facing_to_rect.top = facing_to_rect.bottom - 2; break;
			case 3: InsetRect(&facing_to_rect,0,3); facing_to_rect.left = facing_to_rect.right - 2; break;
			}
		put_rect_in_gworld(ter_draw_gworld,facing_to_rect,
		  0,0,0);*/
		  
		// Labels for wandering and hidden
	//	if (town.creatures[creature_num].hidden_class > 0) {
	//		sprintf(str,"H");
	//		}
	//	}

}

void draw_item_3D(short item_num,short at_point_center_x,short at_point_center_y, short square_x, short square_y,RECT *to_whole_area_rect,short lighting)
{
	graphic_id_type a;	
		
	if (town.preset_items[item_num].exists() == FALSE)
		return;
	
	//to convert from center of PICT_BOX_WIDTH_3D x PICT_BOX_HEIGHT_3D image to center of drawn item
	at_point_center_x += 1;
	at_point_center_y += 7;
	
	a = scen_data.scen_items[town.preset_items[item_num].which_item].item_floor_graphic;
	if (a.not_legit() == FALSE)
		if(place_item_icon_into_ter_3D_large(a, at_point_center_x + town.preset_items[item_num].item_shift.x,
												at_point_center_y + town.preset_items[item_num].item_shift.y
													- scen_data.scen_ter_types[t_d.terrain[square_x][square_y]].height_adj,to_whole_area_rect, lighting) == FALSE)
			cant_draw_graphics_error(a,"Error was for item type",town.preset_items[item_num].which_item);
	
	/*if (same_point(town.preset_items[item_num].item_loc,loc_drawn)) {
		Rect to_rect = large_edit_ter_rects[in_square_x][in_square_y];
		InsetRect(&to_rect,10,10);
		OffsetRect(&to_rect, town.preset_items[item_num].item_shift.x,town.preset_items[item_num].item_shift.y);
		
		a = scen_data.scen_items[town.preset_items[item_num].which_item].item_floor_graphic;

		icon_to_use = scen_data.scen_items[town.preset_items[item_num].which_item].inven_icon;

		SetRect(&from_rect,1 + 29 * (icon_to_use % 10),
		  1 + 29 * (icon_to_use / 10),
		  1 + 29 * (icon_to_use % 10) + 28,
		  1 + 29 * (icon_to_use / 10) + 28);
		
		short obj_index = safe_get_index_of_sheet(&a);
		if (obj_index < 0) {
			cant_draw_graphics_error(a,"Error was for item type",town.preset_items[item_num].which_item);
			put_rect_in_gworld(ter_draw_gworld,to_rect,0,0,0);
			return;
			}		
		
		rect_draw_some_item(graphics_library[obj_index],
			from_rect,ter_draw_gworld,to_rect,0,0);

		put_rect_in_gworld(ter_draw_gworld,to_rect,0,0,0);
		}*/

}

//XXX if line_on_2D_x_side and line_on_2D_y_side are nonzero, it's a corner (they should be -1, 0, or 1)
//line_on_2D_x_side and line_on_2D_y_side should not both be nonzero. (also they should only be -1, 0, or 1)
void put_line_segment_in_gworld_3D(HDC line_gworld,outdoor_record_type *drawing_terrain,short at_point_center_3D_x,short at_point_center_3D_y,short square_2D_x, short square_2D_y, short line_on_2D_x_side, short line_on_2D_y_side, Boolean corner_label_x,Boolean corner_label_y, short inset_3D_y, short offset_3D_y,short r,short g, short b,RECT *to_whole_area_rect)
{
	Boolean is_corner = (line_on_2D_x_side != 0 && line_on_2D_y_side != 0);//this should be false!
	
	if(is_corner == FALSE) {
		//to convert from center of PICT_BOX_WIDTH_3D x PICT_BOX_HEIGHT_3D image to center of drawn 3D floor
		at_point_center_3D_y += 11;
		
		/*
		Ok, here's my attempt at explaining this.
		Unfortunately, there is a three-way naming conflict using 'x' and 'y'.
		I will in this description give them additional notation.
		
		Definitions:
		2D - the same view as the standard editor, or any Exile game.  Distance usually measured in squares in this function
		3D - the '3D isometric' view as seen in the Avernum game, or as this version of the editor newly displays. Distance 
			usually measured in pixels in this function
		label - just something to distinguish the third party of the naming conflict.
		N,E,S,W - cardinal directions north, east, south and west.
			As in the game, these always refer to the same orientation in data terms, which means they can point
			different screen directions depending on whether the view is 2D or 3D. North is up in 2D, to the top right in 3D.
		
		Purpose of this function:
		Draw part of a rect (such as a special rect) in the 3D view.  Draw a part that goes on a single square.
		('square' meaning a single piece of Avernum-world, with one piece of floor, up to one creature, etc.)
		However, this function must be used twice for the corner of a rect: one for each side of the square.
		Also, if the line goes up a cliff to the north or west, draw it going up the cliff.
		
		See diagram:
		
		------------------------------------------------------------------------------------------------
		*
		 *
		  3
		   D
		    *
			 *
		
		'3D y' axis (aka NW-SE)
		 |				
		-+-'3D x' axis (aka NE-SW)
		 |				
						
		
		   '2D y' axis (aka north-south)
		\ /
		 X
		/ \
		   '2D x' axis (aka east-west)
		
		
		 W  /|\  N
		   / | \
		  /  |  \
		  ---+------ 'label x' corners (on '3D x', which is NE-SW)
		  \  |  /		
		   \ | /	
		 S  \|/  E		
		     |			
			 |			
		   'label y' corners (on '3D y', which is NW-SE)
		
		------------------------------------------------------------------------------------------------
		
		The diagonal lines are meant to represent the borders of a square drawn in 3D.
		It's really more stretched out horizontally but I can't draw that in ASCII art.
		Converted to a 2D view:
		
		Rotate everything 45¡ counterclockwise (approximately - the proportions are distorted):
		
		------------------------------------------------------------------------------------------------
		**2D**
		
		
		'3D y' axis   '3D x' axis (aka NE-SW)
		(aka NW-SE)\ /				
					X 
				   / \			
						
		
		'2D y' axis (aka north-south)
		 |
		-+-'2D x' axis (aka east-west)
		 |
		
		
		         'label x' corners (on '3D x', which is NE-SW)
		    N   /
		  \----/
		  |\  /|
		W | \/ | E
		  | /\ |
		  |/  \|
		  /----\
		    S   \
				 'label y' corners (on '3D y', which is NW-SE)
				 
		------------------------------------------------------------------------------------------------
		
		General idea:
		
		In order to draw a line segment, a first point and a second point are needed.
		We know that this line segment will be along one of the borders of the 'square'.
		(Although it will be slightly to the inside of the square, which process will be described later.)
		Now the problem is, how to have a general form for which is the first point and which is the second,
		so that it can be calculated simply.
		My answer:  observe the 'label x' and 'label y' lines in the diagrams.  The corners of the square
		which lie on the 'label x' line are called 'x corners' and those that lie on the 'label y' line are 'y corners'.
		No matter which of the four positions on the edge of the square, the line will go between one 'x corner' and one 'y corner'.
		
		Now, how to specifically calculate the points?  Summarized here:
		It is a simple matter to merely get the corner points themselves, using SPACE_X_DISPLACEMENT_3D and SPACE_Y_DISPLACEMENT_3D (add or subtract).
		Now it needs to be adjusted inwards (towards the center of the square) based on the inset.
		
		This function is used as part of making rectangles.  The rectangles have corners.  Observe:
		
		------------------------------------------------------------------------------------------------
		*
		 *
		  3
		   D
		    *
			 *
		
		  \
		   X\
		  / \\
		 /   \\
		/     \\
		\     //
		 \   //
		  \ //
		   X/
		  /
		------------------------------------------------------------------------------------------------
		
		This diagram shows the corner of a special rectangle (for example), which is inset to be inside of the square border.
		Now, this call of this function is only going to draw part of that (I still show the square border for reference):
		
		------------------------------------------------------------------------------------------------
		  
		   /\
		  /  \
		 /    \
		/      \ (a)
		\     //
		 \   //
		  \ //
		   X/
		   (b)
		------------------------------------------------------------------------------------------------
		
		Notice, when the line approaches the corner (a), it does not go all the way to the square's edge.
		It does that, however, at (b).  I hope it is obvious why:  it has to connect the whole rectangle,
		but not draw crossed lines at the corners.
		
		The difficulty is implementing that.
		
		The arguments corner_label_x and corner_label_y say which parts of the line are corners.
		(Remember, they could both be corners if this is a 1x1 rectangle, for example.)
		If one of them is yes, it means the point represented by that label_x or label_y is reaching a corner,
		so it should not go all the way to the edge of the square.
		
		Then if the square has a sloping terrain on it, the line should adjust and go up the slope, essentialy
		raising one of the points by ELEVATION_Y_DISPLACEMENT_3D.
		
		After doing all that, it should check whether there's a cliff behind it and if so, draw a line up the cliff.
		To find out which point to start at, the label_x or the label_y one, it checks where it was drawing the line
		using the values of line_on_2D_x_side or line_on_2D_y_side and picks the label_x point or the label_y point accordingly.
		
		BEGINNING OF MAIN CODE:
		*/
		
		
		
		
		
		//Does the line segment touch the 'right' or 'bottom' corner of the square as viewed from 3D; that is, respectively, the northwest and the southwest.
		Boolean right, bottom;
		right = (line_on_2D_x_side == 1 || line_on_2D_y_side == -1);
		bottom = (line_on_2D_x_side == 1 || line_on_2D_y_side == 1);
		
		//Since the 3D lines aren't really at a 45¡ angle, the inset must be converted for use on 3D_x so that the line is continuous
		//(between multiple calls of this function for the same rectangle), and at the correct angle
		//The reason x is derived but y is given is:  y is smaller.  If x were given, rounding error would be more of a problem when getting y.
		short inset_3D_x = (inset_3D_y * SPACE_X_DISPLACEMENT_3D) / SPACE_Y_DISPLACEMENT_3D;
		
		/*
		Find the label_x point
		
		
		zoomed in view (possible; the analysis also works if it was the other possible label_x point, at the '<' corner)
		line coming to corner of rectangle:
		------------------------------------------------------------------------------------------------
		\
		 \
		  \
		   \
		/  /
		  /
		 /
		/
		------------------------------------------------------------------------------------------------
		In this one we simply shift over by inset_3D_x.
		
		
		not coming to corner of rectangle:
		------------------------------------------------------------------------------------------------
		\
		 \
		  X
		 / \
		/  /
		  /
		 /
		/
		------------------------------------------------------------------------------------------------
		Now this is tougher.  X distance from the corner is half of the other circumstance, so shift by inset_3D_x / 2.
		Also to match it, keeping the line at the right angle, shift y by inset_3D_y / 2.
		
		*/
		short label_x_displacement_3D_x = SPACE_X_DISPLACEMENT_3D;
		if(!corner_label_x)
			label_x_displacement_3D_x -= inset_3D_x / 2;
		else
			label_x_displacement_3D_x = label_x_displacement_3D_x - inset_3D_x;
		if(!right)
			label_x_displacement_3D_x = -label_x_displacement_3D_x;
		
		short label_x_displacement_3D_y = inset_3D_y / 2;
		if(bottom)
			label_x_displacement_3D_y = -label_x_displacement_3D_y;
		if(corner_label_x)
			label_x_displacement_3D_y = 0;
		
		
		/*
		Find the label_y point
		
		A similar analysis can be applied as for the label_x point
		*/
		short label_y_displacement_3D_y = SPACE_Y_DISPLACEMENT_3D;
		if(!corner_label_y)
			label_y_displacement_3D_y -= inset_3D_y / 2;
		else
			label_y_displacement_3D_y = label_y_displacement_3D_y - inset_3D_y;
		if(!bottom)
			label_y_displacement_3D_y = -label_y_displacement_3D_y;
		
		short label_y_displacement_3D_x = inset_3D_x / 2;
		if(right)
			label_y_displacement_3D_x = -label_y_displacement_3D_x;
		if(corner_label_y)
			label_y_displacement_3D_x = 0;
		
		//Hill terrains effectively modify the height of the square's corners, raising some of them by one elevation level.
		//The result will be a straight line going from the bottom of the hill to the top.  Not ideal for the curvy hills
		//Avernum uses, but it should work.  Also, not all of Avernum's "hills" are like that.  There are also stairs.
		short height_adjust_label_x = 0, height_adjust_label_y = 0;
		short t_to_draw = (editing_town) ? t_d.terrain[square_2D_x][square_2D_y] : drawing_terrain->terrain[square_2D_x][square_2D_y];
		short hill_ability = scen_data.scen_ter_types[t_to_draw].special;
		if(hill_ability >= 19 && hill_ability <= 30) {
			hill_ability -= 19;
			if(right) {
				height_adjust_label_x = hill_c_heights[hill_ability][3];
			}
			else {
				height_adjust_label_x = hill_c_heights[hill_ability][1];
			}
		
			if(bottom) {
				height_adjust_label_y = hill_c_heights[hill_ability][2];
			}
			else {
				height_adjust_label_y = hill_c_heights[hill_ability][0];
			}
		}
		
		//don't draw it if it's out of the drawing area
		if( !(to_whole_area_rect->right <= at_point_center_3D_x + ((label_x_displacement_3D_x > label_y_displacement_3D_x) ? label_y_displacement_3D_x : label_x_displacement_3D_x)/*left*/) && 
			!(to_whole_area_rect->left >= at_point_center_3D_x + ((label_x_displacement_3D_x > label_y_displacement_3D_x) ? label_x_displacement_3D_x : label_y_displacement_3D_x)/*right*/) && 
			!(to_whole_area_rect->top >= at_point_center_3D_y + offset_3D_y + ((label_x_displacement_3D_y - height_adjust_label_x * ELEVATION_Y_DISPLACEMENT_3D > label_y_displacement_3D_y - height_adjust_label_y * ELEVATION_Y_DISPLACEMENT_3D) ?
											label_x_displacement_3D_y - height_adjust_label_x * ELEVATION_Y_DISPLACEMENT_3D : label_y_displacement_3D_y - height_adjust_label_y * ELEVATION_Y_DISPLACEMENT_3D)/*bottom*/) && 
			!(to_whole_area_rect->bottom <= at_point_center_3D_y + offset_3D_y + ((label_x_displacement_3D_y - height_adjust_label_x * ELEVATION_Y_DISPLACEMENT_3D > label_y_displacement_3D_y - height_adjust_label_y * ELEVATION_Y_DISPLACEMENT_3D) ?
											label_y_displacement_3D_y - height_adjust_label_y * ELEVATION_Y_DISPLACEMENT_3D : label_x_displacement_3D_y - height_adjust_label_x * ELEVATION_Y_DISPLACEMENT_3D)/*top*/))
		{
			//draw the important line
			put_line_in_gworld(line_gworld,
				at_point_center_3D_x + label_x_displacement_3D_x,
				at_point_center_3D_y + label_x_displacement_3D_y - height_adjust_label_x * ELEVATION_Y_DISPLACEMENT_3D + offset_3D_y,
				at_point_center_3D_x + label_y_displacement_3D_x,
				at_point_center_3D_y + label_y_displacement_3D_y - height_adjust_label_y * ELEVATION_Y_DISPLACEMENT_3D + offset_3D_y,
				r, g, b);
		}
		
		
		
		//NOW, draw lines up cliff faces
		short top_or_left_height, //height of square that the line continues towards
		current_height, //height of this square
		height_difference, //number of elevation levels to go up from this square to other square
		start_point_3D_x, start_point_3D_y;
		
		current_height = (editing_town) ? t_d.height[square_2D_x][square_2D_y] : drawing_terrain->height[square_2D_x][square_2D_y];
		
		//north-south line
		if(line_on_2D_x_side != 0) {
			//don't do it if it goes off the map!  This is only needed for the grid, which isn't supposed to go off the real (editable) map anyway
			if(square_2D_y == 0)
				return;
			//don't do it if line stops before reaching the cliff!
			if((line_on_2D_x_side == 1 && corner_label_x) || (line_on_2D_x_side == -1 && corner_label_y))
				return;
			top_or_left_height = (editing_town) ? t_d.height[square_2D_x][square_2D_y - 1] : drawing_terrain->height[square_2D_x][square_2D_y - 1];
			if (line_on_2D_x_side == 1)
				current_height = current_height + height_adjust_label_x;
			else
				current_height = current_height + height_adjust_label_y;
			start_point_3D_x = at_point_center_3D_x + ((line_on_2D_x_side == 1) ? label_x_displacement_3D_x : label_y_displacement_3D_x);
			start_point_3D_y = at_point_center_3D_y + ((line_on_2D_x_side == 1) ? label_x_displacement_3D_y - height_adjust_label_x * ELEVATION_Y_DISPLACEMENT_3D : 
																				  label_y_displacement_3D_y - height_adjust_label_y * ELEVATION_Y_DISPLACEMENT_3D);
		}
		//east-west line
		else {
			//don't do it if it goes off the map!  This is only needed for the grid, which isn't supposed to go off the real (editable) map anyway
			if(square_2D_x == 0)
				return;
			//don't do it if line stops before reaching the cliff!
			if((line_on_2D_y_side == 1 && corner_label_x) || (line_on_2D_y_side == -1 && corner_label_y))
				return;
			top_or_left_height = (editing_town) ? t_d.height[square_2D_x - 1][square_2D_y] : drawing_terrain->height[square_2D_x - 1][square_2D_y];
			if (line_on_2D_y_side == 1)
				current_height = current_height + height_adjust_label_x;
			else
				current_height = current_height + height_adjust_label_y;
			start_point_3D_x = at_point_center_3D_x + ((line_on_2D_y_side == 1) ? label_x_displacement_3D_x : label_y_displacement_3D_x);
			start_point_3D_y = at_point_center_3D_y + ((line_on_2D_y_side == 1) ? label_x_displacement_3D_y - height_adjust_label_x * ELEVATION_Y_DISPLACEMENT_3D : 
																				  label_y_displacement_3D_y - height_adjust_label_y * ELEVATION_Y_DISPLACEMENT_3D);
		}
		
		height_difference = top_or_left_height - current_height;
		
		if(height_difference > 0) {
			//don't draw it if it's out of the drawing area
			//this is easier because it's a vertical line, so I don't have to figure out which side is the left vs. the right side,
			//and I know which is the top vs. the bottom of the line
			if( !(to_whole_area_rect->right <= start_point_3D_x/*left*/) && 
				!(to_whole_area_rect->left >= start_point_3D_x/*right*/) && 
				!(to_whole_area_rect->top >= start_point_3D_y + offset_3D_y/*bottom*/) && 
				!(to_whole_area_rect->bottom <= start_point_3D_y - height_difference * ELEVATION_Y_DISPLACEMENT_3D + offset_3D_y/*top*/))
			{
				put_line_in_gworld(line_gworld, start_point_3D_x, start_point_3D_y + offset_3D_y,
					start_point_3D_x, start_point_3D_y - height_difference * ELEVATION_Y_DISPLACEMENT_3D + offset_3D_y, r, g, b);
			}
		}
	}
}

void maybe_draw_part_of_3D_rect(outdoor_record_type *drawing_terrain, short center_of_current_square_x, short center_of_current_square_y, short x, short y,macRECT rect, short inset, short r, short g, short b,RECT *to_whole_area_rect)
{
	HBITMAP store_bmp;
	SetBkMode(main_dc4,TRANSPARENT);
	store_bmp = (HBITMAP) SelectObject(main_dc4,DibBitmapHandle(ter_draw_gworld));

	if(rect.top == y && x >= rect.left && x <= rect.right)
		put_line_segment_in_gworld_3D(main_dc4,drawing_terrain,center_of_current_square_x,center_of_current_square_y,x,y,
				0,-1,x == rect.right,x == rect.left,inset,0,r,g,b,to_whole_area_rect);
	if(rect.left == x && y >= rect.top && y <= rect.bottom)
		put_line_segment_in_gworld_3D(main_dc4,drawing_terrain,center_of_current_square_x,center_of_current_square_y,x,y,
				-1,0,y == rect.bottom,y == rect.top,inset,0,r,g,b,to_whole_area_rect);
	if(rect.bottom == y && x >= rect.left && x <= rect.right)
		put_line_segment_in_gworld_3D(main_dc4,drawing_terrain,center_of_current_square_x,center_of_current_square_y,x,y,
				0,1,x == rect.left,x == rect.right,inset,0,r,g,b,to_whole_area_rect);
	if(rect.right == x && y >= rect.top && y <= rect.bottom)
		put_line_segment_in_gworld_3D(main_dc4,drawing_terrain,center_of_current_square_x,center_of_current_square_y,x,y,
				1,0,y == rect.top,y == rect.bottom,inset,0,r,g,b,to_whole_area_rect);

	SelectObject(main_dc4,store_bmp);
}

void draw_town_objects_3D(short x, short y, short at_point_center_x, short at_point_center_y,RECT *to_whole_area_rect,short lighting)
{
	int i;
	graphic_id_type a;
	Boolean field_of_type[22] = {};//initialize to 0 (FALSE)
	short ftype;
	location loc_drawn;
	Boolean same_point(location l1,location l2);
	short k;

			if ((editing_town) && (hintbook_mode2 == 0) && (hintbook_mode5 == 0)) {
	for (k = 0; k < NUM_TOWN_PLACED_FIELDS; k++)
		if ((town.preset_fields[k].field_loc.x == x) && (town.preset_fields[k].field_loc.y == y)) {
			ftype = town.preset_fields[k].field_type;
			if ( (0 <= ftype) && (ftype < 22) )
				field_of_type[ftype] = TRUE;
		}

	if (/*is_oblique_mirror(x,y)*/  field_of_type[2]) {
		a.which_sheet = 790;
		a.which_icon = 13;//this probably doesn't look different enough - it's just another animation picture
		place_icon_into_ter_3D_large(a,at_point_center_x,at_point_center_y
			- scen_data.scen_ter_types[t_d.terrain[x][y]].height_adj,to_whole_area_rect,lighting);
	}
		
	if (/*is_force_barrier(x,y)*/  field_of_type[3]) {
		a.which_sheet = 743;
		a.which_icon = 6;//this probably doesn't look different enough - it's just another animation picture
		place_icon_into_ter_3D_large(a,at_point_center_x,at_point_center_y
			- scen_data.scen_ter_types[t_d.terrain[x][y]].height_adj,to_whole_area_rect,lighting);
	}

	//I want my own custom pictures for these?  It would be nice to be able to distinguish them, anyway...
	if (/*is_fire_barrier(x,y)*/  field_of_type[4]) {
		a.which_sheet = 743;
		a.which_icon = 3;
		place_icon_into_ter_3D_large(a,at_point_center_x,at_point_center_y
			- scen_data.scen_ter_types[t_d.terrain[x][y]].height_adj,to_whole_area_rect,lighting);
	}
	if (/*is_web(x,y)*/  field_of_type[5]) {
		a.which_sheet = 707;
		a.which_icon = 8;
		place_icon_into_ter_3D_large(a,at_point_center_x,at_point_center_y
			- scen_data.scen_ter_types[t_d.terrain[x][y]].height_adj,to_whole_area_rect,lighting);
	}
	if (/*is_crate(x,y)*/  field_of_type[6]) {
		a.which_sheet = 707;
		a.which_icon = 6;
		place_icon_into_ter_3D_large(a,at_point_center_x,at_point_center_y
			- scen_data.scen_ter_types[t_d.terrain[x][y]].height_adj,to_whole_area_rect,lighting);
	}
	// draw barrels, etc
	if (/*is_barrel(x,y)*/  field_of_type[7]) {
		a.which_sheet = 707;
		a.which_icon = 5;
		place_icon_into_ter_3D_large(a,at_point_center_x,at_point_center_y
			- scen_data.scen_ter_types[t_d.terrain[x][y]].height_adj,to_whole_area_rect,lighting);
	}
	if (/*is_facing_mirror(x,y)*/  field_of_type[8]) {
		a.which_sheet = 790;
		a.which_icon = 12;//this probably doesn't look different enough - it's just another animation picture
		place_icon_into_ter_3D_large(a,at_point_center_x,at_point_center_y
			- scen_data.scen_ter_types[t_d.terrain[x][y]].height_adj,to_whole_area_rect,lighting);
	}

				for (short i = 0; i < 30; i++)
				if ((x == scenario.scen_boats[i].boat_loc.x) && (y == scenario.scen_boats[i].boat_loc.y) && (cur_town == scenario.scen_boats[i].which_town)) {
					a.which_sheet = 741;
					a.which_icon = 7;
					place_icon_into_ter_3D_large(a,at_point_center_x,at_point_center_y
			- scen_data.scen_ter_types[t_d.terrain[x][y]].height_adj,to_whole_area_rect,lighting);
					}
				for (short i = 0; i < 30; i++)
				if ((x == scenario.scen_horses[i].horse_loc.x) && (y == scenario.scen_horses[i].horse_loc.y) && (cur_town == scenario.scen_horses[i].which_town)) {
					a.which_sheet = 768;
					a.which_icon = 3;
					place_icon_into_ter_3D_large(a,at_point_center_x,at_point_center_y
			- scen_data.scen_ter_types[t_d.terrain[x][y]].height_adj,to_whole_area_rect,lighting);
					}
	}
	
	// draw stains
for (short j = 14; j < 22; j++) {
	if ( field_of_type[j]) {

		if(j == 14) {
			a.which_sheet = 706;
			a.which_icon = 4;
		}
		if(j == 15) {
			a.which_sheet = 706;
			a.which_icon = 4;
		}
		if(j == 16) {
			a.which_sheet = 706;
			a.which_icon = 5;
		}
		if(j == 17) {
			a.which_sheet = 706;
			a.which_icon = 7;
		}
		if(j == 18) {
			a.which_sheet = 706;
			a.which_icon = 8;
		}
		if(j == 19) {
			a.which_sheet = 707;
			a.which_icon = 2;
		}
		if(j == 20) {
			a.which_sheet = 706;
			a.which_icon = 9;
		}
		if(j == 21) {
			a.which_sheet = 707;
			a.which_icon = 0;
		}

		}
			place_icon_into_ter_3D_large(a,at_point_center_x,at_point_center_y
				- scen_data.scen_ter_types[t_d.terrain[x][y]].height_adj,to_whole_area_rect,lighting);
	}
	
	// draw creatures
			if ((hintbook_mode2 == 0) && (hintbook_mode3 == 0)) {
	for (i = 0; i < NUM_TOWN_PLACED_CREATURES; i++)
		if (town.creatures[i].number >= 0/*exists()*/ && town.creatures[i].start_loc.x == x && town.creatures[i].start_loc.y == y)
			draw_creature_3D((short)i,at_point_center_x,at_point_center_y,x,y,to_whole_area_rect,lighting);
	if ((selected_item_number >= 7000) && (selected_item_number < 7000 + NUM_TOWN_PLACED_CREATURES && town.creatures[selected_item_number - 7000].start_loc.x == x && town.creatures[selected_item_number - 7000].start_loc.y == y)) {
		draw_creature_3D(selected_item_number % 1000,at_point_center_x,at_point_center_y,x,y,to_whole_area_rect,lighting);
	}
}
	// draw items
			if ((hintbook_mode2 == 0) && (hintbook_mode4 == 0)) {
	for (i = 0; i < NUM_TOWN_PLACED_ITEMS; i++)
		if (town.preset_items[i].which_item >= 0/*exists()*/ && town.preset_items[i].item_loc.x == x && town.preset_items[i].item_loc.y == y)
			draw_item_3D((short)i,at_point_center_x,at_point_center_y,x,y,to_whole_area_rect,lighting);

	// draw selected instance (why's this needed? what does/did it do in 2D?)
	//oh, I know.  To draw it on top of all the others.  This would be good for 3D as well
	if ((selected_item_number >= 11000) && (selected_item_number < 11000 + NUM_TOWN_PLACED_ITEMS && town.preset_items[selected_item_number - 11000].item_loc.x == x && town.preset_items[selected_item_number - 11000].item_loc.y == y)) {
		draw_item_3D(selected_item_number % 1000,at_point_center_x,at_point_center_y,x,y,to_whole_area_rect,lighting);
	}

}

}

void draw_ter_3D_large()
{
	short x,y,i;
	short t_to_draw,floor_to_draw,height_to_draw;
//	RECT tiny_to_base = {15,15,22,22};
	RECT to_rect;
	RECT *to_whole_area_rect = &to_rect;
	graphic_id_type a;
	short sheet;
	HFONT store_font;
	short last_large_mode = cur_viewing_mode;
	short current_size = ((editing_town) ? max_dim[town_type] : 48);
	short center_area_x, center_area_y;
	short center_of_current_square_x, center_of_current_square_y;
	short center_height, neighbor_height;
	short rel_x, rel_y;
	Boolean te_full_move_block, te_partial_move_block, te_no_move_block;
	
	location which_outdoor_sector = {0,0};
	
	short hill_ability;
	short height_adjust_top, height_adjust_left, height_adjust_bottom, height_adjust_right, height_adjust_center;

	short nw_corner, ne_corner, sw_corner, se_corner;//0 if nonexistant, 1 if wall type 1, 2 if wall type 2
	Boolean nw_corner_cutaway, ne_corner_cutaway, sw_corner_cutaway, se_corner_cutaway;
	Boolean is_wall_corner;
	
	short wall_height = 0;
	
	//used when cur_viewing_mode == 11
	location view_from, view_to;
	Boolean see_to = TRUE, see_in = TRUE;
	Boolean see_in_neighbors[3][3] = {{TRUE,TRUE,TRUE},{TRUE,TRUE,TRUE},{TRUE,TRUE,TRUE}};
	Boolean terrain_in_front = FALSE;
	short lighting = 8, rad, distance;
	
	view_from.x = (char)cen_x;
	view_from.y = (char)cen_y;
	
	//extern Rect large_edit_ter_rects[9][9];
	
 	paint_pattern(ter_draw_gworld,0,terrain_rect_gr_size,2);

	RECT whole_area_rect = terrain_viewport_3d;
        /*= {large_edit_ter_rects[0][0].left,large_edit_ter_rects[0][0].top,
		large_edit_ter_rects[8][8].right,large_edit_ter_rects[8][8].bottom};*/
	
	//MacInsetRect(&whole_area_rect,-15,-15);
	ZeroRectCorner(&whole_area_rect);

	HRGN clipRgn = CreateRectRgn( whole_area_rect.left, whole_area_rect.top, whole_area_rect.right, whole_area_rect.bottom );
	HBITMAP store_bmp;
	SetBkMode(main_dc5,TRANSPARENT);
	store_font = (HFONT)SelectObject(main_dc5,bold_font);
	store_bmp = (HBITMAP) SelectObject(main_dc5,DibBitmapHandle(ter_draw_gworld));
	SelectClipRgn( main_dc5, clipRgn );

	// clip rectangle for algorithmic clipping
	RECT clipRect = whole_area_rect;
	InflateRect( &clipRect, SPACE_X_DISPLACEMENT_3D / 2 + 1, SPACE_Y_DISPLACEMENT_3D );
	clipRect.top	-= (ELEVATION_Y_DISPLACEMENT_3D * kTer3DTopClip);		// for cliff of outer screen origin 
	clipRect.bottom	+= (ELEVATION_Y_DISPLACEMENT_3D * kTer3DBottomClip);	// for item/cliff of outer screen origin

	put_rect_in_gworld(main_dc5,whole_area_rect,0,0,0);

	to_rect = whole_area_rect;
	MacInsetRect(&to_rect,1,1);
	fill_rect_in_gworld(main_dc5,to_rect,0,0,0);

	SelectObject(main_dc5,store_bmp);

	if (file_is_loaded == FALSE) {
		SelectObject(main_dc5,DibBitmapHandle(ter_draw_gworld));
		to_rect = whole_area_rect;
		MacInsetRect(&to_rect,1,1);
		fill_rect_in_gworld(main_dc5,to_rect,230,230,230);

		SelectObject(main_dc5,store_font);
		SelectObject(main_dc5,store_bmp);

		to_rect = whole_area_rect;
		OffsetRect(&to_rect,TER_RECT_UL_X,TER_RECT_UL_Y);
		rect_draw_some_item(ter_draw_gworld,
			whole_area_rect,ter_draw_gworld,to_rect,0,1);			
		return;
	}

	center_area_x = (short)(whole_area_rect.right / 2);
	center_area_y = (short)(whole_area_rect.bottom / 2);
	
	center_height = (editing_town) ? t_d.height[cen_x][cen_y] : current_terrain.height[cen_x][cen_y];
	
	outdoor_record_type *drawing_terrain = NULL;
	
	short temp_x, temp_y;
	//this crazy code draws bordering sectors when in outdoor mode (or repeating floors when in town mode)
	short sector_offset_x, sector_offset_y;
	
	short temp_floor[2][2];
	short temp_terrain[2][2];
	short temp_height[2][2];
	short temp_see_to[2][2];
	
	set_up_corner_and_sight_map();
	
	for(sector_offset_x = -1;sector_offset_x <= 1/*!editing_town*/;sector_offset_x++){
    	for(sector_offset_y = -1;sector_offset_y <= 1/*!editing_town*/;sector_offset_y++){
		//don't draw nonexistant sectors
		if(!editing_town && (cur_out.x + sector_offset_x < 0 || cur_out.y + sector_offset_y < 0 || 
		cur_out.x + sector_offset_x >= scenario.out_width || cur_out.y + sector_offset_y >= scenario.out_height))
			continue;
	
		//don't waste time drawing totally useless stuff
		if(cen_x > 18 && sector_offset_x == -1)
			continue;
		if(cen_x < (current_size - 18) && sector_offset_x == 1)
			continue;
		if(cen_y > 18 && sector_offset_y == -1)
			continue;
		if(cen_y < (current_size - 18) && sector_offset_y == 1)
			continue;
		
		if(!editing_town) {
			if(sector_offset_x == 0 && sector_offset_y == 0)
				drawing_terrain = &current_terrain;
			else
				drawing_terrain = &border_terrains[sector_offset_x + 1][sector_offset_y + 1];
			
			//region number
			if(current_terrain.extra[0] != drawing_terrain->extra[0])
				continue;
	
			which_outdoor_sector = cur_out;
			OffsetLocation( which_outdoor_sector, sector_offset_x, sector_offset_y );
		}
	
		for(x = 0; x < current_size; x++) {
			for(y = 0; y < current_size; y++) {
				temp_x = temp_y = 0;
				//only draw things in line of sight from center
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
					
					see_in = get_see_in(sector_offset_x,sector_offset_y,x,y);
					see_to = get_see_to(sector_offset_x,sector_offset_y,x,y);
					
					for(temp_x = 0; temp_x < 3; temp_x++) {
						for(temp_y = 0; temp_y < 3; temp_y++) {
							see_in_neighbors[temp_x][temp_y] = get_see_in(sector_offset_x,sector_offset_y,x + temp_x - 1,y + temp_y - 1);
						}
					}
				
					if(see_to) {
						if(editing_town ? town.is_on_surface : current_terrain.is_on_surface)
							lighting = 8;
						else {
							lighting = (editing_town) ? t_d.lighting[view_to.x][view_to.y] : 0;
							if(editing_town) {
								if(town.lighting == 0)
									lighting += 4;
								
								if(town.lighting != 3)
									rad = 8;
								else
									rad = 1;
							}
							else {
								lighting += 5;
								rad = 2;
							}
							distance = dist(view_from,view_to);
							
							if(distance <= rad) {
								lighting += rad - distance + 1;
							}
							
							if(lighting == 0)
								continue;
							
							lighting = min((editing_town ? 6 : 8),lighting);
						}
					}
				}
			
				if(editing_town && (sector_offset_x != 0 || sector_offset_y != 0)) {
					temp_x = x;
					temp_y = y;
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
					floor_to_draw = t_d.floor[temp_x][temp_y];
					t_to_draw = 0;
					height_to_draw = t_d.height[temp_x][temp_y];
					if(town.external_floor_type != -1)
						floor_to_draw = town.external_floor_type;
				}
				else {
					t_to_draw = (editing_town) ? t_d.terrain[x][y] : drawing_terrain->terrain[x][y];		
					floor_to_draw = (editing_town) ? t_d.floor[x][y] : drawing_terrain->floor[x][y];		
					height_to_draw = (editing_town) ? t_d.height[x][y] : drawing_terrain->height[x][y];		
				}
			
				rel_x = (x + sector_offset_x * current_size) - cen_x;
				rel_y = (y + sector_offset_y * current_size) - cen_y;
				
				center_of_current_square_x = (rel_x - rel_y) * SPACE_X_DISPLACEMENT_3D + center_area_x;
				center_of_current_square_y = (rel_x + rel_y) * SPACE_Y_DISPLACEMENT_3D + center_area_y
					- (height_to_draw - center_height) * ELEVATION_Y_DISPLACEMENT_3D;
	
				// clipping
				POINT center_of_current_square = {center_of_current_square_x, center_of_current_square_y};
				if ( ! POINTInRECT( center_of_current_square, clipRect ) )
					continue;

				//draw only bordering floors in outside of town, where the border rect doesn't touch
				if(editing_town && (sector_offset_x != 0 || sector_offset_y != 0) && see_in) {
					
					//don't draw that which can't be seen.  it's easy here.
					//no, place_icon_into_ter_3D_large() already checks for this
					/*if(center_of_current_square_x - SPACE_X_DISPLACEMENT_3D > whole_area_rect.right ||
					center_of_current_square_y - SPACE_Y_DISPLACEMENT_3D > whole_area_rect.bottom ||
					center_of_current_square_x + SPACE_X_DISPLACEMENT_3D < whole_area_rect.left ||
					center_of_current_square_y + SPACE_Y_DISPLACEMENT_3D < whole_area_rect.top)
						continue;*/
					
					//don't draw solid stone
					if(floor_to_draw == 255)
						continue;
					
					//if the town borders touch the edge, the outside terrain isn't displayed
					//remember, at least one of temp_x and temp_y is 0 or (current_size - 1)
					//so at least one of the first parts of these if's will be checked
					if((temp_x == 0) && (temp_x == town.in_town_rect.left  ) && (temp_y >= town.in_town_rect.top ) && (temp_y <= town.in_town_rect.bottom))
						continue;
					if((temp_x == (current_size - 1))
									 && (temp_x == town.in_town_rect.right ) && (temp_y >= town.in_town_rect.top ) && (temp_y <= town.in_town_rect.bottom))
						continue;
					if((temp_y == 0) && (temp_y == town.in_town_rect.top   ) && (temp_x >= town.in_town_rect.left) && (temp_x <= town.in_town_rect.right ))
						continue;
					if((temp_y == (current_size - 1)) 
									 && (temp_y == town.in_town_rect.bottom) && (temp_x >= town.in_town_rect.left) && (temp_x <= town.in_town_rect.right ))
						continue;
					
					a = scen_data.scen_floors[floor_to_draw].pic;
				
					// if graphic is undefined for floor, just draw white
					if (a.not_legit()) {
						//fill_rect_in_gworld(ter_draw_gworld,large_edit_ter_rects[q][r],230,230,230);
					}
					else if (place_icon_into_ter_3D_large(a,center_of_current_square_x,center_of_current_square_y,to_whole_area_rect,lighting) == FALSE)
						cant_draw_graphics_error(a,"Error was for floor type",floor_to_draw);
					
					continue;
				}
			
				//nasty - floor outside town isn't affected by this, so this has to be done after drawing it
				center_of_current_square_y = center_of_current_square_y - scen_data.scen_floors[floor_to_draw].floor_height;
				
				//draw cliffs
				if(!editing_town) {
					//yes, even if it's a neigboring sector with a different is_on_surface value, it gets displayed like the current sector
					//not that that situation would happen
					if(current_terrain.is_on_surface)
						sheet = 657;
					else
						sheet = 658;
				}
				else
					sheet = town.cliff_sheet;
			
				if (sheet >= 0 && see_to) {
					//draw cliffs to SE, then E, then S
					
					temp_floor [0][0] = temp_floor [1][0] = temp_floor [0][1] = temp_floor [1][1] = floor_to_draw;
					temp_height[0][0] = temp_height[1][0] = temp_height[0][1] = temp_height[1][1] = height_to_draw;
					temp_terrain[0][0] = temp_terrain[1][0] = temp_terrain[0][1] = temp_terrain[1][1] = 0;
					
					temp_see_to[0][0] = see_to;
					temp_see_to[1][0] = get_see_to(sector_offset_x,sector_offset_y,x + 1,y);
					temp_see_to[0][1] = get_see_to(sector_offset_x,sector_offset_y,x,y + 1);
					temp_see_to[1][1] = get_see_to(sector_offset_x,sector_offset_y,x + 1,y + 1);
					
					if(x < current_size - 1) {
						temp_floor[1][0] = (editing_town) ? t_d.floor[x + 1][y] : drawing_terrain->floor[x + 1][y];
						temp_terrain[1][0] = (editing_town) ? t_d.terrain[x + 1][y] : drawing_terrain->terrain[x + 1][y];
						temp_height[1][0] = (editing_town) ? t_d.height[x + 1][y] : drawing_terrain->height[x + 1][y];
					}
					else if(!editing_town && sector_offset_x < 1) {
						temp_floor[1][0] = border_terrains[sector_offset_x + 2][sector_offset_y + 1].floor[0][y];
						temp_terrain[1][0] = border_terrains[sector_offset_x + 2][sector_offset_y + 1].terrain[0][y];
						temp_height[1][0] = border_terrains[sector_offset_x + 2][sector_offset_y + 1].height[0][y];
					}
					
					if(y < current_size - 1) {
						temp_floor[0][1] = (editing_town) ? t_d.floor[x][y + 1] : drawing_terrain->floor[x][y + 1];
						temp_terrain[0][1] = (editing_town) ? t_d.terrain[x][y + 1] : drawing_terrain->terrain[x][y + 1];
						temp_height[0][1] = (editing_town) ? t_d.height[x][y + 1] : drawing_terrain->height[x][y + 1];
					}
					else if(!editing_town && sector_offset_y < 1) {
						temp_floor[0][1] = border_terrains[sector_offset_x + 1][sector_offset_y + 2].floor[x][0];
						temp_terrain[0][1] = border_terrains[sector_offset_x + 1][sector_offset_y + 2].terrain[x][0];
						temp_height[0][1] = border_terrains[sector_offset_x + 1][sector_offset_y + 2].height[x][0];
					}
					
					if(x < current_size - 1 && y < current_size - 1) {
						temp_floor[1][1] = (editing_town) ? t_d.floor[x + 1][y + 1] : drawing_terrain->floor[x + 1][y + 1];
						temp_terrain[1][1] = (editing_town) ? t_d.terrain[x + 1][y + 1] : drawing_terrain->terrain[x + 1][y + 1];
						temp_height[1][1] = (editing_town) ? t_d.height[x + 1][y + 1] : drawing_terrain->height[x + 1][y + 1];
					}
					else if(!editing_town && sector_offset_y < 1 && sector_offset_x < 1) {
						temp_floor[1][1] = border_terrains[sector_offset_x + 2][sector_offset_y + 2].floor[0][0];
						temp_terrain[1][1] = border_terrains[sector_offset_x + 2][sector_offset_y + 2].terrain[0][0];
						temp_height[1][1] = border_terrains[sector_offset_x + 2][sector_offset_y + 2].height[0][0];
					}
				
					//don't draw cliffs if lower space is solid stone (true to BoA)
					//can only see a cliff if see_to base and top of them
					//For E and S cliffs (also similar for SE cliffs), don't draw them if nearby sloping terrain slopes up to both border corner-points
					//(the game does it this way, it looks better, and there's no loss of information)
					
					//SE cliffs
					if(!(x == current_size - 1 && cur_out.x + sector_offset_x == scenario.out_width - 1) && 
					!(y == current_size - 1 && cur_out.y + sector_offset_y == scenario.out_height - 1) && 
					!((temp_height[1][0] >= height_to_draw) && (temp_height[0][1] < height_to_draw)) && //??? this is what I got from observing BoA...
					(temp_floor[0][1] != 255) &&
					(temp_see_to[0][1]) &&
					!(temp_height[0][1] == height_to_draw - 1 && (scen_data.scen_ter_types[temp_terrain[1][1]].special == 19 || scen_data.scen_ter_types[temp_terrain[1][1]].special == 25 || scen_data.scen_ter_types[temp_terrain[1][1]].special == 26 || scen_data.scen_ter_types[temp_terrain[1][1]].special == 27 || scen_data.scen_ter_types[temp_terrain[1][1]].special == 28 || scen_data.scen_ter_types[temp_terrain[1][1]].special == 30))) {
						neighbor_height = temp_height[1][1];
						while(neighbor_height < min(height_to_draw,temp_height[0][1])) {
							if (place_cliff_icon_into_ter_3D_large(sheet,center_of_current_square_x, center_of_current_square_y +
							(height_to_draw - neighbor_height) * ELEVATION_Y_DISPLACEMENT_3D, 1, to_whole_area_rect, lighting) == FALSE)
								cant_draw_graphics_error(a,"Error was for cliff type",sheet);
							neighbor_height++;
						}
					}
				
					//E cliffs
					if(!(x == current_size - 1 && cur_out.x + sector_offset_x == scenario.out_width - 1) && 
					(temp_height[1][0] < height_to_draw) && 
					(temp_floor[1][0] != 255) &&
					(temp_see_to[1][0]) &&
					!(temp_height[1][0] == height_to_draw - 1 && (scen_data.scen_ter_types[temp_terrain[1][0]].special == 19 || scen_data.scen_ter_types[temp_terrain[1][0]].special == 27 || scen_data.scen_ter_types[temp_terrain[1][0]].special == 28))) {
						neighbor_height = min(temp_height[1][0],temp_height[1][1]);//as in BoA, depends on lower of east and southeast spaces
						while(neighbor_height < height_to_draw) {
							if (place_cliff_icon_into_ter_3D_large(sheet,center_of_current_square_x, center_of_current_square_y +
							(height_to_draw - neighbor_height) * ELEVATION_Y_DISPLACEMENT_3D, 0, to_whole_area_rect, lighting) == FALSE)
								cant_draw_graphics_error(a,"Error was for cliff type",sheet);
							neighbor_height++;
						}
					}
					
					//S cliffs
					if(!(y == current_size - 1 && cur_out.y + sector_offset_y == scenario.out_height - 1) && 
					(temp_height[0][1] < height_to_draw) && 
					(temp_floor[0][1] != 255) &&
					(temp_see_to[0][1]) &&
					!(temp_height[0][1] == height_to_draw - 1 && (scen_data.scen_ter_types[temp_terrain[0][1]].special == 25 || scen_data.scen_ter_types[temp_terrain[0][1]].special == 27 || scen_data.scen_ter_types[temp_terrain[0][1]].special == 30))) {
						neighbor_height = temp_height[0][1];
						while(neighbor_height < height_to_draw) {
							if (place_cliff_icon_into_ter_3D_large(sheet,center_of_current_square_x, center_of_current_square_y +
							(height_to_draw - neighbor_height) * ELEVATION_Y_DISPLACEMENT_3D, 2, to_whole_area_rect, lighting) == FALSE)
								cant_draw_graphics_error(a,"Error was for cliff type",sheet);
							neighbor_height++;
						}
					}
				}
				
				// draw floor if the terrain doesn't cover it up, but not solid stone
				if(floor_to_draw != 255 && scen_data.scen_ter_types[t_to_draw].suppress_floor == FALSE && see_in) {
					if (hintbook_mode6 == 0)
					a = scen_data.scen_floors[floor_to_draw].pic;
					else a = scen_data.scen_floors[254].pic;
					// if graphic is undefined for floor, just draw white
					if (a.not_legit()) {
						//fill_rect_in_gworld(ter_draw_gworld,large_edit_ter_rects[q][r],230,230,230);
					}
					else if (place_icon_into_ter_3D_large(a,center_of_current_square_x,center_of_current_square_y,to_whole_area_rect,lighting) == FALSE)
						cant_draw_graphics_error(a,"Error was for floor type",floor_to_draw);
				}
				
				te_full_move_block = (scen_data.scen_ter_types[t_to_draw].move_block[0] == 1 && scen_data.scen_ter_types[t_to_draw].move_block[1] == 1 &&
					scen_data.scen_ter_types[t_to_draw].move_block[2] == 1 && scen_data.scen_ter_types[t_to_draw].move_block[3] == 1);
				
				te_partial_move_block = (!te_full_move_block && (scen_data.scen_ter_types[t_to_draw].move_block[0] == 1 || scen_data.scen_ter_types[t_to_draw].move_block[1] == 1 ||
					scen_data.scen_ter_types[t_to_draw].move_block[2] == 1 || scen_data.scen_ter_types[t_to_draw].move_block[3] == 1));
				
				te_no_move_block = (scen_data.scen_ter_types[t_to_draw].move_block[0] == 0 && scen_data.scen_ter_types[t_to_draw].move_block[1] == 0 &&
					scen_data.scen_ter_types[t_to_draw].move_block[2] == 0 && scen_data.scen_ter_types[t_to_draw].move_block[3] == 0);
				
				//deal with putting extra wall piece in corner of other walls
				nw_corner = get_nw_corner(sector_offset_x,sector_offset_y,x,y);
				sw_corner = get_sw_corner(sector_offset_x,sector_offset_y,x,y);
				se_corner = get_se_corner(sector_offset_x,sector_offset_y,x,y);
				ne_corner = get_ne_corner(sector_offset_x,sector_offset_y,x,y);
			
				//only draw this editor stuff when not viewing as in the game
				if(cur_viewing_mode == 10) {
					// draw terrain that goes under drawn rects (no move block is a rough approximation)
			if (hintbook_mode7 == 0) {
					if (t_to_draw > 0 && te_no_move_block) {
						draw_terrain_3D(t_to_draw, x, y, cur_out.x + sector_offset_x, cur_out.y + sector_offset_y, center_of_current_square_x, center_of_current_square_y, see_in_neighbors, 0, to_whole_area_rect, lighting);
					}
					}
					if(sector_offset_x == 0 && sector_offset_y == 0) {
						//grid over everything being edited
						//nasty floor height adjust strikes again!  don't let the grid be adjusted by it, because it corresponds to clicking.
						SelectObject(main_dc5,DibBitmapHandle(ter_draw_gworld));
						put_line_segment_in_gworld_3D(main_dc5,drawing_terrain,center_of_current_square_x,
						center_of_current_square_y + scen_data.scen_floors[floor_to_draw].floor_height,x,y,
						1,0,FALSE,FALSE,0,-1,48,48,48,to_whole_area_rect);
						put_line_segment_in_gworld_3D(main_dc5,drawing_terrain,center_of_current_square_x,
						center_of_current_square_y + scen_data.scen_floors[floor_to_draw].floor_height,x,y,
						-1,0,FALSE,FALSE,0,0,48,48,48,to_whole_area_rect);
						put_line_segment_in_gworld_3D(main_dc5,drawing_terrain,center_of_current_square_x,
						center_of_current_square_y + scen_data.scen_floors[floor_to_draw].floor_height,x,y,
						0,1,FALSE,FALSE,0,0,48,48,48,to_whole_area_rect);
						put_line_segment_in_gworld_3D(main_dc5,drawing_terrain,center_of_current_square_x,
						center_of_current_square_y + scen_data.scen_floors[floor_to_draw].floor_height,x,y,
						0,-1,FALSE,FALSE,0,1,48,48,48,to_whole_area_rect);
						SelectObject(main_dc5,store_bmp);
					}
				
					//draw little wall corners corners if they should be drawn
					if(sw_corner == 1 || sw_corner == 2) {
						a.clear_graphic_id_type();
						a.which_icon = 9;
						if(editing_town == FALSE) {
							//yes, even if it's a neigboring sector with a different is_on_surface value, it gets displayed like the current sector
							//not that that situation would happen
							a.which_sheet = ((current_terrain.is_on_surface) ? 616 : 614);
						}
						else if(sw_corner == 1)
							a.which_sheet = town.wall_1_sheet;
						else if(sw_corner == 2)
							a.which_sheet = town.wall_2_sheet;
						place_corner_wall_icon_into_ter_3D_large(a,center_of_current_square_x,center_of_current_square_y,TRUE,to_whole_area_rect,lighting);
					}
					if(nw_corner == 1 || nw_corner == 2) {
						a.clear_graphic_id_type();
						a.which_icon = 9;
						if(editing_town == FALSE) {
							//yes, even if it's a neigboring sector with a different is_on_surface value, it gets displayed like the current sector
							//not that that situation would happen
							a.which_sheet = ((current_terrain.is_on_surface) ? 616 : 614);
						}
						else if(nw_corner == 1)
							a.which_sheet = town.wall_1_sheet;
						else if(nw_corner == 2)
							a.which_sheet = town.wall_2_sheet;
						place_corner_wall_icon_into_ter_3D_large(a,center_of_current_square_x + 18,center_of_current_square_y - 12,TRUE,to_whole_area_rect,lighting);
					}
					if(ne_corner == 1 || ne_corner == 2) {
						a.clear_graphic_id_type();
						a.which_icon = 9;
						if(editing_town == FALSE) {
							//yes, even if it's a neigboring sector with a different is_on_surface value, it gets displayed like the current sector
							//not that that situation would happen
							a.which_sheet = ((current_terrain.is_on_surface) ? 616 : 614);
						}
						else if(ne_corner == 1)
							a.which_sheet = town.wall_1_sheet;
						else if(ne_corner == 2)
							a.which_sheet = town.wall_2_sheet;
						place_corner_wall_icon_into_ter_3D_large(a,center_of_current_square_x,center_of_current_square_y,FALSE,to_whole_area_rect,lighting);
					}
					if(se_corner == 1 || se_corner == 2) {
						a.clear_graphic_id_type();
						a.which_icon = 9;
						if(editing_town == FALSE) {
							//yes, even if it's a neigboring sector with a different is_on_surface value, it gets displayed like the current sector
							//not that that situation would happen
							a.which_sheet = ((current_terrain.is_on_surface) ? 616 : 614);
						}
						else if(se_corner == 1)
							a.which_sheet = town.wall_1_sheet;
						else if(se_corner == 2)
							a.which_sheet = town.wall_2_sheet;
						place_corner_wall_icon_into_ter_3D_large(a,center_of_current_square_x + 18,center_of_current_square_y + 12,TRUE,to_whole_area_rect,lighting);
					}
					
					//draw walls, etc. after the grid so it doesn't look bad (walls stick up a little, and the grid doesn't go in a bump over them)
			if (hintbook_mode7 == 0) {
					if (t_to_draw > 0 && te_partial_move_block) {
						draw_terrain_3D(t_to_draw, x, y, cur_out.x + sector_offset_x, cur_out.y + sector_offset_y, center_of_current_square_x, center_of_current_square_y, see_in_neighbors, 0, to_whole_area_rect, lighting);
					}
					}
					// draw Various Rectangles
					// Town mode: special encs and other rectangles
					if (editing_town) {
						for (i = 0; i < NUM_TOWN_PLACED_SPECIALS; i++) {
							if ((town.spec_id[i] >= 0) && (town.spec_id[i] < 255)) {
								maybe_draw_part_of_3D_rect(drawing_terrain,center_of_current_square_x,center_of_current_square_y,x,y,
										town.special_rects[i], 1 + 4, 200, 200, 255, to_whole_area_rect);
								maybe_draw_part_of_3D_rect(drawing_terrain,center_of_current_square_x,center_of_current_square_y,x,y,
										town.special_rects[i], 2 + 4, 0, 0, 255, to_whole_area_rect);
							}
						}
						
						maybe_draw_part_of_3D_rect(drawing_terrain,center_of_current_square_x,center_of_current_square_y,x,y,
									town.in_town_rect, 3 + 4, 255, 0, 0, to_whole_area_rect);
						
						// description rects
						for (i = 0; i < 16; i++) {
							if (town.room_rect[i].right > 0) {
								maybe_draw_part_of_3D_rect(drawing_terrain,center_of_current_square_x,center_of_current_square_y,x,y,
										town.room_rect[i], 4 + 4, 0, 255, 0, to_whole_area_rect);
							}
						}
					}
					// Outdoor mode: special encs and other rectangles
					if (editing_town == FALSE) {
						// town entry rects
						for (i = 0; i < 8; i++) {
							if ((current_terrain.exit_rects[i].right > 0) && (drawing_terrain->exit_dests[i] >= 0)) {
								maybe_draw_part_of_3D_rect(drawing_terrain,center_of_current_square_x,center_of_current_square_y,x,y,
										drawing_terrain->exit_rects[i], 3 + 4, 255, 0, 255, to_whole_area_rect);
							}
						}
					
						// special enc rects
						for (i = 0; i < NUM_OUT_PLACED_SPECIALS; i++) {
							if (drawing_terrain->spec_id[i] >= 0) {
								maybe_draw_part_of_3D_rect(drawing_terrain,center_of_current_square_x,center_of_current_square_y,x,y,
										drawing_terrain->special_rects[i], 1 + 4, 200, 200, 255, to_whole_area_rect);
								maybe_draw_part_of_3D_rect(drawing_terrain,center_of_current_square_x,center_of_current_square_y,x,y,
										drawing_terrain->special_rects[i], 2 + 4, 0, 0, 255, to_whole_area_rect);
							}
						}
						
						// description rects
						for (i = 0; i < 8; i++) {
							if (current_terrain.info_rect[i].right > 0) {
								maybe_draw_part_of_3D_rect(drawing_terrain,center_of_current_square_x,center_of_current_square_y,x,y,
										drawing_terrain->info_rect[i], 4 + 4, 0, 255, 0, to_whole_area_rect);
							}
						}
					}
				
					// draw terrain that goes over drawn rects (move block is a rough approximation)
			if (hintbook_mode7 == 0) {
					if (t_to_draw > 0 && te_full_move_block) {
						draw_terrain_3D(t_to_draw, x, y, cur_out.x + sector_offset_x, cur_out.y + sector_offset_y, center_of_current_square_x, center_of_current_square_y, see_in_neighbors, 0, to_whole_area_rect, lighting);
					}
					}
					// Town mode: draw all instances
					if (editing_town) {
						draw_town_objects_3D(x,y,center_of_current_square_x,center_of_current_square_y,to_whole_area_rect,lighting);
					}
					
					place_ter_icons_3D(which_outdoor_sector,drawing_terrain,x,y,t_to_draw,floor_to_draw,center_of_current_square_x,center_of_current_square_y,to_whole_area_rect);
				}
				if(cur_viewing_mode == 11 && see_to) {
					terrain_in_front = (!scen_data.scen_ter_types[t_to_draw].blocks_view[0] && !scen_data.scen_ter_types[t_to_draw].blocks_view[1] &&
					(scen_data.scen_ter_types[t_to_draw].blocks_view[2] || scen_data.scen_ter_types[t_to_draw].blocks_view[3]) );
					
					//determine whether extra corner wall pieces will be drawn cut away (regardless of whether they'll be drawn at all)
					nw_corner_cutaway = see_in_neighbors[0][0];
					sw_corner_cutaway = see_in_neighbors[0][1];
					se_corner_cutaway = see_in_neighbors[1][1];
					ne_corner_cutaway = see_in_neighbors[1][0];
					
					//figure out if any of them will be drawn (to account for BoA's strangeness - if a corner wall is
					//drawn on a space, all the other terrain on that space [except sometimes other walls] is drawn)
					is_wall_corner = (nw_corner == 1 || nw_corner == 2) || (sw_corner == 1 || sw_corner == 2) || 
					(se_corner == 1 || se_corner == 2) || (ne_corner == 1 || ne_corner == 2);
				
					//draw nw, ne, sw corners if they should be drawn
					if(sw_corner == 1 || sw_corner == 2) {
						a.clear_graphic_id_type();
						a.which_icon = sw_corner_cutaway ? 9 : 8;
						if(editing_town == FALSE) {
							//yes, even if it's a neigboring sector with a different is_on_surface value, it gets displayed like the current sector
							//not that that situation would happen
							a.which_sheet = ((current_terrain.is_on_surface) ? 616 : 614);
							wall_height = ((sw_corner == 1) ? 2 : 1);
						}
						else if(sw_corner == 1) {
							a.which_sheet = town.wall_1_sheet;
							wall_height = town.wall_1_height;
						}
						else if(sw_corner == 2) {
							a.which_sheet = town.wall_2_sheet;
							wall_height = town.wall_2_height;
						}
						if(sw_corner_cutaway)
							wall_height = 1;
						for(i = 0; i < wall_height; i++) {
							place_corner_wall_icon_into_ter_3D_large(a,center_of_current_square_x,
							center_of_current_square_y - i * 35,TRUE,to_whole_area_rect,lighting);
						}
					}
					if(nw_corner == 1 || nw_corner == 2) {
						a.clear_graphic_id_type();
						a.which_icon = nw_corner_cutaway ? 9 : 8;
						if(editing_town == FALSE) {
							//yes, even if it's a neigboring sector with a different is_on_surface value, it gets displayed like the current sector
							//not that that situation would happen
							a.which_sheet = ((current_terrain.is_on_surface) ? 616 : 614);
							wall_height = ((nw_corner == 1) ? 2 : 1);
						}
						else if(nw_corner == 1) {
							a.which_sheet = town.wall_1_sheet;
							wall_height = town.wall_1_height;
						}
						else if(nw_corner == 2) {
							a.which_sheet = town.wall_2_sheet;
							wall_height = town.wall_2_height;
						}
						if(nw_corner_cutaway)
							wall_height = 1;
						for(i = 0; i < wall_height; i++) {
							place_corner_wall_icon_into_ter_3D_large(a,center_of_current_square_x + 18,
							(center_of_current_square_y - 12) - i * 35,TRUE,to_whole_area_rect,lighting);
						}
					}
					if(ne_corner == 1 || ne_corner == 2) {
						a.clear_graphic_id_type();
						a.which_icon = ne_corner_cutaway ? 9 : 8;
						if(editing_town == FALSE) {
							//yes, even if it's a neigboring sector with a different is_on_surface value, it gets displayed like the current sector
							//not that that situation would happen
							a.which_sheet = ((current_terrain.is_on_surface) ? 616 : 614);
							wall_height = ((ne_corner == 1) ? 2 : 1);
						}
						else if(ne_corner == 1) {
							a.which_sheet = town.wall_1_sheet;
							wall_height = town.wall_1_height;
						}
						else if(ne_corner == 2) {
							a.which_sheet = town.wall_2_sheet;
							wall_height = town.wall_2_height;
						}
						if(ne_corner_cutaway)
							wall_height = 1;
						for(i = 0; i < wall_height; i++) {
							place_corner_wall_icon_into_ter_3D_large(a,center_of_current_square_x,
							center_of_current_square_y - i * 35,FALSE,to_whole_area_rect,lighting);
						}
					}
			if (hintbook_mode7 == 0) {
					if(!terrain_in_front) {
						if (t_to_draw > 0) {
							draw_terrain_3D(t_to_draw, x, y, cur_out.x + sector_offset_x, cur_out.y + sector_offset_y, center_of_current_square_x, center_of_current_square_y, see_in_neighbors, is_wall_corner, to_whole_area_rect, lighting);
						}
					}
					}
							// Town mode: draw all instances
					if (editing_town && see_in) {
						draw_town_objects_3D(x,y,center_of_current_square_x,center_of_current_square_y,to_whole_area_rect,lighting);
					}
			if (hintbook_mode7 == 0) {
					if(terrain_in_front) {
						if (t_to_draw > 0) {
							draw_terrain_3D(t_to_draw, x, y, cur_out.x + sector_offset_x, cur_out.y + sector_offset_y, center_of_current_square_x, center_of_current_square_y, see_in_neighbors, is_wall_corner, to_whole_area_rect, lighting);
						}
					}
					}
					//draw se corner if it should be drawn
					if(se_corner == 1 || se_corner == 2) {
						a.clear_graphic_id_type();
						a.which_icon = se_corner_cutaway ? 9 : 8;
						if(editing_town == FALSE) {
							//yes, even if it's a neigboring sector with a different is_on_surface value, it gets displayed like the current sector
							//not that that situation would happen
							a.which_sheet = ((current_terrain.is_on_surface) ? 616 : 614);
							wall_height = ((se_corner == 1) ? 2 : 1);
						}
						else if(se_corner == 1) {
							a.which_sheet = town.wall_1_sheet;
							wall_height = town.wall_1_height;
						}
						else if(se_corner == 2) {
							a.which_sheet = town.wall_2_sheet;
							wall_height = town.wall_2_height;
						}
						if(se_corner_cutaway)
							wall_height = 1;
						for(i = 0; i < wall_height; i++) {
							place_corner_wall_icon_into_ter_3D_large(a,center_of_current_square_x + 18,
							(center_of_current_square_y + 12) - i * 35,TRUE,to_whole_area_rect,lighting);
						}
					}
				}
			
				//outdoor mode
				if(!editing_town && see_in) {
					//Hill terrains effectively modify the height of the square's corners, raising some of them by (almost) one elevation level.
					hill_ability = scen_data.scen_ter_types[t_to_draw].special;
					height_adjust_right = height_adjust_left = height_adjust_bottom = height_adjust_top = 
										height_adjust_center = scen_data.scen_ter_types[t_to_draw].height_adj;
					//if it's actually a HILL ability...
					if(hill_ability >= 19 && hill_ability <= 30) {
						hill_ability -= 19;
						
						if(hill_c_heights[hill_ability][3])
							height_adjust_right += 10;
							
						if(hill_c_heights[hill_ability][1])
							height_adjust_left += 10;
						
						if(hill_c_heights[hill_ability][2])
							height_adjust_bottom += 10;
							
						if(hill_c_heights[hill_ability][0])
							height_adjust_top += 10;
					}
				
					// Preset monsters
					for (i = 0; i < 8; i++) {
						if ((drawing_terrain->preset[i].start_loc.x > 0) &&
						(drawing_terrain->preset[i].start_loc.x == x) && (drawing_terrain->preset[i].start_loc.y == y)) {
							a = (scen_data.scen_creatures[drawing_terrain->preset[i].hostile[0]].char_graphic);
							a.which_icon = (scen_data.scen_creatures[drawing_terrain->preset[i].hostile[0]].small_or_large_template) ? (5 * 4) : (3 * 4);
							if (a.not_legit() == FALSE) {
								if(drawing_terrain->preset[i].hostile_amount[0] == 0) {//do nothing
							}
							else if(drawing_terrain->preset[i].hostile_amount[0] < 4) {
								if(place_outdoor_creature_icon_into_ter_3D_large(a,center_of_current_square_x + 15,center_of_current_square_y - 11 - height_adjust_center,to_whole_area_rect,lighting) == FALSE)
									cant_draw_graphics_error(a,"Error was for creature type",drawing_terrain->preset[i].hostile[0]);
							}
							else {//place_outdoor_creature_icon_into_ter_3D_large
								if(place_outdoor_creature_icon_into_ter_3D_large(a,center_of_current_square_x + 15,center_of_current_square_y - 21 - height_adjust_top,to_whole_area_rect,lighting) == FALSE)
									cant_draw_graphics_error(a,"Error was for creature type",drawing_terrain->preset[i].hostile[0]);
								if(place_outdoor_creature_icon_into_ter_3D_large(a,center_of_current_square_x + 2,center_of_current_square_y - 11 - height_adjust_left,to_whole_area_rect,lighting) == FALSE)
									cant_draw_graphics_error(a,"Error was for creature type",drawing_terrain->preset[i].hostile[0]);
								if(place_outdoor_creature_icon_into_ter_3D_large(a,center_of_current_square_x + 28,center_of_current_square_y - 11 - height_adjust_right,to_whole_area_rect,lighting) == FALSE)
									cant_draw_graphics_error(a,"Error was for creature type",drawing_terrain->preset[i].hostile[0]);
								if(place_outdoor_creature_icon_into_ter_3D_large(a,center_of_current_square_x + 15,center_of_current_square_y - 1 - height_adjust_bottom,to_whole_area_rect,lighting) == FALSE)
									cant_draw_graphics_error(a,"Error was for creature type",drawing_terrain->preset[i].hostile[0]);
							}
						}
					}
				}
				
				//draw the imaginary player
				if(cur_viewing_mode == 11 && x == cen_x && y == cen_y) {
					a.which_sheet = 4917;
					a.which_icon = 4;
					a.graphic_adjust = 0;
					if(place_outdoor_creature_icon_into_ter_3D_large(a,center_of_current_square_x + 15,center_of_current_square_y - 11 - height_adjust_center,to_whole_area_rect,lighting) == FALSE)
						cant_draw_graphics_error(a,"Make sure that the editor and the current version of 3D Editor Graphics are in the folder 'Blades of Avernum Files'.",0);
				}
			}
			if(editing_town && see_in) {	
	            //draw the imaginary player
	            if(cur_viewing_mode == 11 && x == cen_x && y == cen_y) {
	                a.which_sheet = 4917;
	                a.which_icon = 0;
	                a.graphic_adjust = 0;
	                if(place_icon_into_ter_3D_large(a,center_of_current_square_x,center_of_current_square_y - scen_data.scen_ter_types[t_to_draw].height_adj,to_whole_area_rect,lighting) == FALSE)
					    cant_draw_graphics_error(a,"Make sure that the editor and the current version of 3D Editor Graphics are in the folder 'Blades of Avernum Files'.",0);
				}
			}	
		}
	}

	}
	}

	/* 
	 * Draw corner 'buttons' for scrolling
	 * space in resource:
	 * x,y
	 * topright:130,223
	 * bottomleft:220,313
	 * centertopright:174,267
	 * centerbottomleft:176,269
	 * size:44,44
	 * SetRect(&corner_to,left(x),top(y),right(x),bottom(y));
	 */
	RECT corner_to, corner_from;
	
	corner_to = whole_area_rect;
	corner_to.right = corner_to.left + 44;
	corner_to.bottom = corner_to.top + 44;
	SetRECT(corner_from,129,222,174,267);
	rect_draw_some_item(mixed_gworld,corner_from,ter_draw_gworld,corner_to,1,0);
	
	corner_to = whole_area_rect;
	corner_to.left = corner_to.right - 44;
	corner_to.bottom = corner_to.top + 44;
	SetRECT(corner_from,175,222,220,267);
	rect_draw_some_item(mixed_gworld,corner_from,ter_draw_gworld,corner_to,1,0);
	
	corner_to = whole_area_rect;
	corner_to.left = corner_to.right - 44;
	corner_to.top = corner_to.bottom - 44;
	SetRECT(corner_from,175,268,220,313);
	rect_draw_some_item(mixed_gworld,corner_from,ter_draw_gworld,corner_to,1,0);
	
	corner_to = whole_area_rect;
	corner_to.right = corner_to.left + 44;
	corner_to.top = corner_to.bottom - 44;
	SetRECT(corner_from,129,268,174,313);
	rect_draw_some_item(mixed_gworld,corner_from,ter_draw_gworld,corner_to,1,0);
	
	//draw border (slightly covering up corner buttons for good look)
	SelectObject(main_dc5,DibBitmapHandle(ter_draw_gworld));
	put_line_in_gworld(main_dc5,	(short)(whole_area_rect.left + 38),(short)whole_area_rect.top,
									(short)(whole_area_rect.right - 42),(short)whole_area_rect.top,			173,173,173);
	put_line_in_gworld(main_dc5,	(short)whole_area_rect.left,(short)(whole_area_rect.top + 38),
									(short)whole_area_rect.left,(short)(whole_area_rect.bottom - 42),		173,173,173);
	put_line_in_gworld(main_dc5,	(short)(whole_area_rect.left + 38),(short)(whole_area_rect.bottom - 1),
									(short)(whole_area_rect.right - 38),(short)(whole_area_rect.bottom - 1),82,82,82);
	put_line_in_gworld(main_dc5,	(short)(whole_area_rect.right - 1),(short)(whole_area_rect.top + 38),
									(short)(whole_area_rect.right - 1),(short)(whole_area_rect.bottom - 38),82,82,82);

	SelectObject(main_dc5,store_font);
	SelectObject(main_dc5,store_bmp);
	SelectClipRgn( main_dc5, NULL );
	DeleteObject( clipRgn );

	// plop ter on screen
	to_rect = whole_area_rect;
	OffsetRect(&to_rect,TER_RECT_UL_X,TER_RECT_UL_Y);
	rect_draw_some_item(ter_draw_gworld,whole_area_rect,ter_draw_gworld,to_rect,0,1);

	small_any_drawn = FALSE;
}

void draw_ter_large()
{
	short q,r,i;
	location where_draw;
	short t_to_draw,floor_to_draw,height_to_draw,terrsc_to_draw;
	RECT to_rect;
	graphic_id_type a;
	char str[256];
	HBITMAP store_bmp;
	HFONT store_font;
	short last_large_mode = 0;

 	paint_pattern(ter_draw_gworld,0,terrain_rect_gr_size,1);

	RECT whole_area_rect = {large_edit_ter_rects[0][0].left,large_edit_ter_rects[0][0].top,
		large_edit_ter_rects[8][8].right,large_edit_ter_rects[8][8].bottom}; /**/
	
	MacInsetRect(&whole_area_rect,-15,-15);
	ZeroRectCorner(&whole_area_rect);

	SetBkMode(main_dc5,TRANSPARENT);
	store_font = (HFONT)SelectObject(main_dc5,bold_font);
	store_bmp = (HBITMAP) SelectObject(main_dc5,DibBitmapHandle(ter_draw_gworld));
	
	put_rect_in_gworld(main_dc5,whole_area_rect,0,0,0);
	MacInsetRect(&whole_area_rect,15,15);
	put_rect_in_gworld(main_dc5,whole_area_rect,0,0,0);
	MacInsetRect(&whole_area_rect,-15,-15);
	
	if (file_is_loaded == FALSE) {
		to_rect = whole_area_rect;
		MacInsetRect(&to_rect,16,16);
		fill_rect_in_gworld(main_dc5,to_rect,230,230,230);

		SelectObject(main_dc5,store_font);
		SelectObject(main_dc5,store_bmp);

		to_rect = whole_area_rect;
		OffsetRect(&to_rect,TER_RECT_UL_X,TER_RECT_UL_Y);
		rect_draw_some_item(ter_draw_gworld,
			whole_area_rect,ter_draw_gworld,to_rect,0,1);			

		return;
	}
		
	for (q = 0; q < 9; q++) 
		for (r = 0; r < 9; r++) {
			where_draw.x = (t_coord)q; where_draw.y = (t_coord)r;
			if((editing_town == FALSE) && ((cen_x + q - 4 < 0) || (cen_x + q - 4 >= 48) || (cen_y + r - 4 < 0) || (cen_y + r - 4 >= 48)) ) {
				short sector_offset_x = ((cen_x + q - 4 < 0) ? -1 : ((cen_x + q - 4 >= 48) ? 1 : 0));
				short sector_offset_y = ((cen_y + r - 4 < 0) ? -1 : ((cen_y + r - 4 >= 48) ? 1 : 0));

				//leave the wood background there
				if(cur_out.x + sector_offset_x < 0 || cur_out.y + sector_offset_y < 0 || 
				cur_out.x + sector_offset_x >= scenario.out_width || cur_out.y + sector_offset_y >= scenario.out_height)
					continue;
				t_to_draw = border_terrains[sector_offset_x + 1][sector_offset_y + 1].terrain[cen_x + q - 4 - sector_offset_x * 48][cen_y + r - 4 - sector_offset_y * 48];
				floor_to_draw = border_terrains[sector_offset_x + 1][sector_offset_y + 1].floor[cen_x + q - 4 - sector_offset_x * 48][cen_y + r - 4 - sector_offset_y * 48];
				height_to_draw = border_terrains[sector_offset_x + 1][sector_offset_y + 1].height[cen_x + q - 4 - sector_offset_x * 48][cen_y + r - 4 - sector_offset_y * 48];
			}
			else if((editing_town == TRUE) && ((cen_x + q - 4 < 0) || (cen_x + q - 4 >= max_dim[town_type]) || (cen_y + r - 4 < 0) || (cen_y + r - 4 >= max_dim[town_type])) ) {

				short temp_x = cen_x + q - 4;
				short temp_y = cen_y + r - 4;
				if(cen_x + q - 4 < 0) {
					temp_x = 0;
				}
				if(cen_x + q - 4 >= max_dim[town_type]) {
					temp_x = max_dim[town_type] - 1;
				}
				if(cen_y + r - 4 < 0) {
					temp_y = 0;
				}
				if(cen_y + r - 4 >= max_dim[town_type]) {
					temp_y = max_dim[town_type] - 1;
				}

				if((temp_x == 0) && (temp_x == town.in_town_rect.left  ) && (temp_y >= town.in_town_rect.top ) && (temp_y <= town.in_town_rect.bottom))
					continue;
				if((temp_x == (max_dim[town_type] - 1))
								 && (temp_x == town.in_town_rect.right ) && (temp_y >= town.in_town_rect.top ) && (temp_y <= town.in_town_rect.bottom))
					continue;
				if((temp_y == 0) && (temp_y == town.in_town_rect.top   ) && (temp_x >= town.in_town_rect.left) && (temp_x <= town.in_town_rect.right ))
					continue;
				if((temp_y == (max_dim[town_type] - 1)) 
								 && (temp_y == town.in_town_rect.bottom) && (temp_x >= town.in_town_rect.left) && (temp_x <= town.in_town_rect.right ))
					continue;
				
				floor_to_draw = t_d.floor[temp_x][temp_y];
				t_to_draw = 0;
				height_to_draw = t_d.height[temp_x][temp_y];
				if(town.external_floor_type != -1) {
					floor_to_draw = town.external_floor_type;
				}
			}

			else {
				t_to_draw = (editing_town) ? t_d.terrain[cen_x + q - 4][cen_y + r - 4] : current_terrain.terrain[cen_x + q - 4][cen_y + r - 4];		
				floor_to_draw = (editing_town) ? t_d.floor[cen_x + q - 4][cen_y + r - 4] : current_terrain.floor[cen_x + q - 4][cen_y + r - 4];		
				height_to_draw = (editing_town) ? t_d.height[cen_x + q - 4][cen_y + r - 4] : current_terrain.height[cen_x + q - 4][cen_y + r - 4];
				}

			location loc_drawn = {(t_coord)(cen_x + q - 4), (t_coord)(cen_y + r - 4)};
			if (same_point(loc_drawn,town.ter_scripts[i].loc)) {
				terrsc_to_draw = i;
				}
			else
				terrsc_to_draw = 999;

			// draw floor
			if (hintbook_mode6 == 0)
			a = scen_data.scen_floors[floor_to_draw].ed_pic;
					else
			a = scen_data.scen_floors[254].ed_pic;

			// if graphic is underfined for floor, just draw white
			if (a.not_legit()) {
				fill_rect_in_gworld(main_dc5,large_edit_ter_rects[q][r],230,0,0);
				}
				else {
					SelectObject(main_dc5,store_bmp);
					if (place_terrain_icon_into_ter_large(a,q,r) == FALSE)
						cant_draw_graphics_error(a,"Error was for floor type",floor_to_draw);
					SelectObject(main_dc5,DibBitmapHandle(ter_draw_gworld));
					}

			// draw terrain
			if (hintbook_mode7 == 0) {
			if (t_to_draw > 0) {
				a = scen_data.scen_ter_types[t_to_draw].ed_pic;
				if (a.not_legit())
					 fill_rect_in_gworld(main_dc5,large_edit_ter_rects[q][r],0,255,0);
				else {
					SelectObject(main_dc5,store_bmp);
					if (place_terrain_icon_into_ter_large(a,q,r) == FALSE)
						cant_draw_graphics_error(a,"Error was for terrain type",t_to_draw);
					SelectObject(main_dc5,DibBitmapHandle(ter_draw_gworld));
				}
			}
		}
			else
			t_to_draw == 0;

			// Town mode: draw all instances
			if (editing_town) {
				SelectObject(main_dc5,store_bmp);
				// draw barrels, etc
			if ((hintbook_mode2 == 0) && (hintbook_mode5 == 0)) {
				if (is_barrel(loc_drawn.x,loc_drawn.y)) {
					a.which_sheet = 680;
					a.which_icon = 75;
					place_terrain_icon_into_ter_large(a,q,r);
					}
				if (is_crate(loc_drawn.x,loc_drawn.y)) {
					a.which_sheet = 680;
					a.which_icon = 76;
					place_terrain_icon_into_ter_large(a,q,r);
					}
				if (is_web(loc_drawn.x,loc_drawn.y)) {
					a.which_sheet = 680;
					a.which_icon = 78;
					place_terrain_icon_into_ter_large(a,q,r);
					}
				if (is_fire_barrier(loc_drawn.x,loc_drawn.y)) {
					a.which_sheet = 686;
					a.which_icon = 40;
					place_terrain_icon_into_ter_large(a,q,r);
					}
				if (is_force_barrier(loc_drawn.x,loc_drawn.y)) {
					a.which_sheet = 686;
					a.which_icon = 41;
					place_terrain_icon_into_ter_large(a,q,r);
					}
				if (is_oblique_mirror(loc_drawn.x,loc_drawn.y)) {
					a.which_sheet = 689;
					a.which_icon = 13;
					place_terrain_icon_into_ter_large(a,q,r);
					}
				if (is_facing_mirror(loc_drawn.x,loc_drawn.y)) {
					a.which_sheet = 689;
					a.which_icon = 12;
					place_terrain_icon_into_ter_large(a,q,r);
					}
				for (short i = 0; i < 30; i++)
				if ((same_point(loc_drawn,scenario.scen_boats[i].boat_loc)) && (cur_town == scenario.scen_boats[i].which_town)){
					a.which_sheet = 683;
					a.which_icon = 17;
					place_terrain_icon_into_ter_large(a,q,r);
					}
				for (short i = 0; i < 30; i++)
				if ((same_point(loc_drawn,scenario.scen_horses[i].horse_loc)) && (cur_town == scenario.scen_horses[i].which_town)){
					a.which_sheet = 686;
					a.which_icon = 83;
					place_terrain_icon_into_ter_large(a,q,r);
					}

					}

				// draw stains
				for (short j = 0; j < 8; j++)
					if (is_sfx(loc_drawn.x,loc_drawn.y,j)) {
						a.which_sheet = 680;
						a.which_icon = 63 + j;
						place_terrain_icon_into_ter_large(a,q,r);
						}

				// draw ter scripts
				for (i = 0; i < NUM_TER_SCRIPTS; i++)
					if (town.ter_scripts[i].exists) {
						draw_ter_script(i,loc_drawn,q,r);
						}
						
			if ((hintbook_mode2 == 0) && (hintbook_mode3 == 0)) {
				// draw creatures
				for (i = 0; i < NUM_TOWN_PLACED_CREATURES; i++)
					if (town.creatures[i].exists()) {
						draw_creature(main_dc5,store_bmp,i,loc_drawn,q,r);
						}
				if ((selected_item_number >= 7000) && (selected_item_number < 7000 + NUM_TOWN_PLACED_CREATURES)) {
					draw_creature(main_dc5,store_bmp,selected_item_number % 1000,loc_drawn,q,r);
					}
					}
					
			if ((hintbook_mode2 == 0) && (hintbook_mode4 == 0)) {
				// draw items
				for (i = 0; i < NUM_TOWN_PLACED_ITEMS; i++)
					if (town.preset_items[i].exists()) {
						draw_item(main_dc5,store_bmp,i,loc_drawn,q,r);
						}	
				// draw selected instance
				if ((selected_item_number >= 11000) && (selected_item_number < 11000 + NUM_TOWN_PLACED_ITEMS)) {
					draw_item(main_dc5,store_bmp,selected_item_number % 1000,loc_drawn,q,r);
					}						
					}
					
				SelectObject(main_dc5,DibBitmapHandle(ter_draw_gworld));
			}

			// draw height
			if (current_drawing_mode == 2) {
				sprintf(str,"%d",height_to_draw);
				to_rect = large_edit_ter_rects[q][r];
				OffsetRect(&to_rect,3,14);
				if (strlen(str) == 1)
				to_rect.left = to_rect.right - 14;
				if (strlen(str) == 2)
				to_rect.left = to_rect.right - 23;
				win_draw_string_outline(main_dc5,to_rect,str,2,10);
				}

			else {
			short script_num;
			location loc_drawn;
			short in_square_x;
			short in_square_y;

			// sign numbers, placed special numbers, (town) terrain script numbers,
			if (numerical_display_mode == 0)
				 sprintf(str,"");
			if (numerical_display_mode == 1)
				sprintf(str,"%d",floor_to_draw);
			if (numerical_display_mode == 2)
				sprintf(str,"%d",t_to_draw);
			if (numerical_display_mode == 3)
				sprintf(str,"%d",height_to_draw);
			if (numerical_display_mode == 4)
				sprintf(str,"%d,%d",q,r);

 				to_rect = large_edit_ter_rects[q][r];
				OffsetRect(&to_rect,3,13);
				if (strlen(str) == 1)
				to_rect.left = to_rect.right - 34;
				if (strlen(str) == 2)
				to_rect.left = to_rect.right - 41;
				if (strlen(str) == 3)
				to_rect.left = to_rect.right - 48;
				win_draw_string_outline(main_dc5,to_rect,str,2,10);
				}

			if(((editing_town == FALSE) && ((cen_x + q - 4 <= -1) || (cen_x + q - 4 >= 48) || (cen_y + r - 4 <= -1) || (cen_y + r - 4 >= 48))) ||
			(editing_town == TRUE) && ((cen_x + q - 4 <= -1) || (cen_x + q - 4 >= max_dim[town_type]) || (cen_y + r - 4 <= -1) || (cen_y + r - 4 >= max_dim[town_type])) ) {
				continue;
			}

			put_rect_in_gworld(main_dc5,large_edit_ter_rects[q][r],0,0,0);

			short small_icon_position = 0;

			SelectObject(main_dc5,store_bmp);
				
			// now place the tiny icons in the lower right corner
			// first, stuff that is done for both town and outdoors
			// signs
			if (scen_data.scen_ter_types[t_to_draw].special == 39) {
				place_ter_icon_on_tile(q,r,small_icon_position,20);
				small_icon_position++;
				}			
			if (scen_data.scen_ter_types[t_to_draw].special == 40) {
				place_ter_icon_on_tile(q,r,small_icon_position,21);
				small_icon_position++;
				}			
			
			// icons for secret doors
			if (((t_to_draw >= 18) && (t_to_draw <= 21)) || ((t_to_draw >= 54) && (t_to_draw <= 57))) {
				place_ter_icon_on_tile(q,r,small_icon_position,26);
				small_icon_position++;
				}
			// icons for floor damage
			if ((scen_data.scen_ter_types[t_to_draw].special == 1) || 
			  (scen_data.scen_floors[floor_to_draw].special == 1)) {
				place_ter_icon_on_tile(q,r,small_icon_position,37);
				small_icon_position++;
			  	}
			if ((scen_data.scen_ter_types[t_to_draw].special == 2) || 
			  (scen_data.scen_floors[floor_to_draw].special == 2)) {
				place_ter_icon_on_tile(q,r,small_icon_position,38);
				small_icon_position++;
			  	}
			if ((scen_data.scen_ter_types[t_to_draw].special == 3) || 
			  (scen_data.scen_floors[floor_to_draw].special == 3)) {
				place_ter_icon_on_tile(q,r,small_icon_position,39);
				small_icon_position++;
			  	}

			if ((scen_data.scen_ter_types[t_to_draw].special == 6) || 
			  (scen_data.scen_floors[floor_to_draw].special == 6)) {
				place_ter_icon_on_tile(q,r,small_icon_position,22);
				small_icon_position++;
			  	}
			  	
			// then town only tiny icons
			if (editing_town) {
				for (i = 0; i < 4; i++){	
					if (same_point(loc_drawn,town.start_locs[i])) {
						place_ter_icon_on_tile(q,r,small_icon_position,i);
						small_icon_position++;
					}
        }

				for (i = 0; i < NUM_WAYPOINTS; i++){	
					if (same_point(loc_drawn,town.waypoints[i])) {
						place_ter_icon_on_tile(q,r,small_icon_position,10 + i);
						small_icon_position++;
					}
        }
				for (i = 0; i < 6; i++)	{
					if (same_point(loc_drawn,town.respawn_locs[i])) {
						place_ter_icon_on_tile(q,r,small_icon_position,24);
						small_icon_position++;
					}
        }
				// start scenario icon
				if ((scenario.start_in_what_town == cur_town) && (same_point(loc_drawn,scenario.what_start_loc_in_town))) {
					place_ter_icon_on_tile(q,r,small_icon_position,23);
					small_icon_position++;
				}

				// blocked spaces
				if (is_blocked(loc_drawn.x,loc_drawn.y)) {
					place_ter_icon_on_tile(q,r,small_icon_position,22);
					small_icon_position++;
					}
        }
			SelectObject(main_dc5,DibBitmapHandle(ter_draw_gworld));
			
			// Outdoor mode: draw tiny icons
			if (editing_town == FALSE){
				SelectObject(main_dc5,store_bmp);
				
				// WANDERING MONST ICON
				for (i = 0; i < 4; i++){	
					if (same_point(loc_drawn,current_terrain.wandering_locs[i])) {
						place_ter_icon_on_tile(q,r,small_icon_position,24);
						small_icon_position++;
					}
        }
				// Preset MONST ICON
				for (i = 0; i < 8; i++){	
					if ((current_terrain.preset[i].start_loc.x > 0) &&
					  (same_point(loc_drawn,current_terrain.preset[i].start_loc))) {
						place_ter_icon_on_tile(q,r,small_icon_position,25);
						small_icon_position++;
					}
				}
				// start scenario icon
				if ((same_point(cur_out,scenario.what_outdoor_section_start_in)) && 
				  (same_point(loc_drawn,scenario.start_where_in_outdoor_section))) {
					place_ter_icon_on_tile(q,r,small_icon_position,23);
					small_icon_position++;
				}
				SelectObject(main_dc5,DibBitmapHandle(ter_draw_gworld));
			}
		}

	// draw Various Rectangles
	RECT rectangle_draw_rect;
	RECT clip_rect = whole_area_rect;
	MacInsetRect(&clip_rect,15,15);

	// Town mode: special encs and other rectangles
	if (editing_town){
		if (hintbook_mode8 == 0) {
			for (i = 0; i < NUM_TOWN_PLACED_SPECIALS; i++) {
			if (town.spec_id[i] != kNO_TOWN_SPECIALS) {
				rectangle_draw_rect.left = 15 + BIG_SPACE_SIZE * (town.special_rects[i].left - cen_x + 4);
				rectangle_draw_rect.right = 15 + BIG_SPACE_SIZE * (town.special_rects[i].right - cen_x + 5) - 1;
				rectangle_draw_rect.top = 15 + BIG_SPACE_SIZE * (town.special_rects[i].top - cen_y + 4);
				rectangle_draw_rect.bottom = 15 + BIG_SPACE_SIZE * (town.special_rects[i].bottom - cen_y + 5) - 1;
				MacInsetRect(&rectangle_draw_rect,1,1);
				put_clipped_rect_in_gworld(main_dc5,rectangle_draw_rect,clip_rect,200,200,255);
				MacInsetRect(&rectangle_draw_rect,1,1);
				put_clipped_rect_in_gworld(main_dc5,rectangle_draw_rect,clip_rect,0,0,255);
				MacInsetRect(&rectangle_draw_rect,1,1);
				put_clipped_rect_in_gworld(main_dc5,rectangle_draw_rect,clip_rect,200,200,255);
				}
			}
		}

		if (hintbook_mode8 == 1) {
			for (i = 0; i < 15; i++){
			if (town.sign_locs[i].x >= 0) {
				rectangle_draw_rect.left = 15 + BIG_SPACE_SIZE * (town.sign_locs[i].x - cen_x + 4);
				rectangle_draw_rect.right = 15 + BIG_SPACE_SIZE * (town.sign_locs[i].x - cen_x + 5) + 1;
				rectangle_draw_rect.top = 15 + BIG_SPACE_SIZE * (town.sign_locs[i].y - cen_y + 4);
				rectangle_draw_rect.bottom = 15 + BIG_SPACE_SIZE * (town.sign_locs[i].y - cen_y + 5) + 1;
				MacInsetRect(&rectangle_draw_rect,1,1);
				put_clipped_rect_in_gworld(main_dc5,rectangle_draw_rect,clip_rect,255,0,0);
				MacInsetRect(&rectangle_draw_rect,1,1);
				put_clipped_rect_in_gworld(main_dc5,rectangle_draw_rect,clip_rect,255,0,0);
				MacInsetRect(&rectangle_draw_rect,1,1);
				put_clipped_rect_in_gworld(main_dc5,rectangle_draw_rect,clip_rect,0,255,0);
				MacInsetRect(&rectangle_draw_rect,1,1);
				put_clipped_rect_in_gworld(main_dc5,rectangle_draw_rect,clip_rect,0,255,0);
				MacInsetRect(&rectangle_draw_rect,1,1);
				put_clipped_rect_in_gworld(main_dc5,rectangle_draw_rect,clip_rect,0,0,255);
				MacInsetRect(&rectangle_draw_rect,1,1);
				put_clipped_rect_in_gworld(main_dc5,rectangle_draw_rect,clip_rect,0,0,255);
				}
			}
		}

		if (hintbook_mode8 == 2) {
			 for (i = 0; i < 60; i++){
			 if (town.preset_fields[i].field_loc.x >= 0) {
				rectangle_draw_rect.left = 15 + BIG_SPACE_SIZE * (town.preset_fields[i].field_loc.x - cen_x + 4);
				rectangle_draw_rect.right = 15 + BIG_SPACE_SIZE * (town.preset_fields[i].field_loc.x - cen_x + 5) + 1;
				rectangle_draw_rect.top = 15 + BIG_SPACE_SIZE * (town.preset_fields[i].field_loc.y - cen_y + 4);
				rectangle_draw_rect.bottom = 15 + BIG_SPACE_SIZE * (town.preset_fields[i].field_loc.y - cen_y + 5) + 1;
				MacInsetRect(&rectangle_draw_rect,1,1);
				put_clipped_rect_in_gworld(main_dc5,rectangle_draw_rect,clip_rect,0,0,255);
				MacInsetRect(&rectangle_draw_rect,1,1);
				put_clipped_rect_in_gworld(main_dc5,rectangle_draw_rect,clip_rect,0,0,255);
				MacInsetRect(&rectangle_draw_rect,1,1);
				put_clipped_rect_in_gworld(main_dc5,rectangle_draw_rect,clip_rect,0,255,0);
				MacInsetRect(&rectangle_draw_rect,1,1);
				put_clipped_rect_in_gworld(main_dc5,rectangle_draw_rect,clip_rect,0,255,0);
				MacInsetRect(&rectangle_draw_rect,1,1);
				put_clipped_rect_in_gworld(main_dc5,rectangle_draw_rect,clip_rect,255,0,0);
				MacInsetRect(&rectangle_draw_rect,1,1);
				put_clipped_rect_in_gworld(main_dc5,rectangle_draw_rect,clip_rect,255,0,0);
				}
			}
		}

	if (object_display_mode == 1) {
		for (i = 0; i < 60; i++){
			if (town.special_rects[i].left >= 0) {
				rectangle_draw_rect.left = 15 + BIG_SPACE_SIZE * (town.special_rects[i].left - cen_x + 4);
				rectangle_draw_rect.right = 15 + BIG_SPACE_SIZE * (town.special_rects[i].left - cen_x + 5) + 1;
				rectangle_draw_rect.top = 15 + BIG_SPACE_SIZE * (town.special_rects[i].top - cen_y + 4);
				rectangle_draw_rect.bottom = 15 + BIG_SPACE_SIZE * (town.special_rects[i].top - cen_y + 5) + 1;
				sprintf(str,"%d",town.spec_id[i]);
 				to_rect = rectangle_draw_rect;
				OffsetRect(&to_rect,3,13);
				if (strlen(str) == 1)
				to_rect.left = to_rect.right - 34;
				if (strlen(str) == 2)
				to_rect.left = to_rect.right - 41;
				if (strlen(str) == 3)
				to_rect.left = to_rect.right - 48;
				win_draw_string_outline(main_dc5,to_rect,str,2,10);
					}
        }
    	}
	if (object_display_mode == 2) {
		for (i = 0; i < 15; i++){
			if (town.sign_locs[i].x >= 0) {
				rectangle_draw_rect.left = 15 + BIG_SPACE_SIZE * (town.sign_locs[i].x - cen_x + 4);
				rectangle_draw_rect.right = 15 + BIG_SPACE_SIZE * (town.sign_locs[i].x - cen_x + 5) + 1;
				rectangle_draw_rect.top = 15 + BIG_SPACE_SIZE * (town.sign_locs[i].y - cen_y + 4);
				rectangle_draw_rect.bottom = 15 + BIG_SPACE_SIZE * (town.sign_locs[i].y - cen_y + 5) + 1;
				sprintf(str,"%d",i);
 				to_rect = rectangle_draw_rect;
				OffsetRect(&to_rect,3,13);
				if (strlen(str) == 1)
				to_rect.left = to_rect.right - 34;
				if (strlen(str) == 2)
				to_rect.left = to_rect.right - 41;
				win_draw_string_outline(main_dc5,to_rect,str,2,10);
					}
        }
    	}

	if (object_display_mode == 3) { // town creatures
		for (i = 0; i < 80; i++){
			if (town.creatures[i].start_loc.x >= 0) {
				rectangle_draw_rect.left = 15 + BIG_SPACE_SIZE * (town.creatures[i].start_loc.x - cen_x + 4);
				rectangle_draw_rect.right = 15 + BIG_SPACE_SIZE * (town.creatures[i].start_loc.x - cen_x + 5) + 1;
				rectangle_draw_rect.top = 15 + BIG_SPACE_SIZE * (town.creatures[i].start_loc.y - cen_y + 4);
				rectangle_draw_rect.bottom = 15 + BIG_SPACE_SIZE * (town.creatures[i].start_loc.y - cen_y + 5) + 1;
				sprintf(str,"%d",town.creatures[i].number);
 				to_rect = rectangle_draw_rect;
				OffsetRect(&to_rect,3,13);
				if (strlen(str) == 1)
				to_rect.left = to_rect.right - 34;
				if (strlen(str) == 2)
				to_rect.left = to_rect.right - 41;
				if (strlen(str) == 3)
				to_rect.left = to_rect.right - 48;
				win_draw_string_outline(main_dc5,to_rect,str,2,10);
			    }
        }
    	}

	if (object_display_mode == 4) {
		for (i = 0; i < 100; i++) {
			if (town.ter_scripts[i].exists > 0) {
				rectangle_draw_rect.left = 15 + BIG_SPACE_SIZE * (town.ter_scripts[i].loc.x - cen_x + 4);
				rectangle_draw_rect.right = 15 + BIG_SPACE_SIZE * (town.ter_scripts[i].loc.x - cen_x + 5) + 1;
				rectangle_draw_rect.top = 15 + BIG_SPACE_SIZE * (town.ter_scripts[i].loc.y - cen_y + 4);
				rectangle_draw_rect.bottom = 15 + BIG_SPACE_SIZE * (town.ter_scripts[i].loc.y - cen_y + 5) + 1;
				sprintf(str,"%d",i);
 				to_rect = rectangle_draw_rect;
				OffsetRect(&to_rect,3,13);
				if (strlen(str) == 1)
				to_rect.left = to_rect.right - 34;
				if (strlen(str) == 2)
				to_rect.left = to_rect.right - 41;
				win_draw_string_outline(main_dc5,to_rect,str,2,10);
					}
				}
			}

	if (object_display_mode == 5) { // preset fields
		for (i = 0; i < 60; i++){
			if (town.preset_fields[i].field_loc.x >= 0) {
				rectangle_draw_rect.left = 15 + BIG_SPACE_SIZE * (town.preset_fields[i].field_loc.x - cen_x + 4);
				rectangle_draw_rect.right = 15 + BIG_SPACE_SIZE * (town.preset_fields[i].field_loc.x - cen_x + 5) + 1;
				rectangle_draw_rect.top = 15 + BIG_SPACE_SIZE * (town.preset_fields[i].field_loc.y - cen_y + 4);
				rectangle_draw_rect.bottom = 15 + BIG_SPACE_SIZE * (town.preset_fields[i].field_loc.y - cen_y + 5) + 1;
				sprintf(str,"%d",i);
 				to_rect = rectangle_draw_rect;
				OffsetRect(&to_rect,3,13);
				if (strlen(str) == 1)
				to_rect.left = to_rect.right - 34;
				if (strlen(str) == 2)
				to_rect.left = to_rect.right - 41;
				if (strlen(str) == 3)
				to_rect.left = to_rect.right - 48;
				win_draw_string_outline(main_dc5,to_rect,str,2,10);
			    }
        }
    	}

	if (object_display_mode == 6) { // scenario horse or scenario boat

	for (i = 0; i < 30; i++) {
	if (scenario.scen_horses[i].which_town == cur_town) {
				rectangle_draw_rect.left = 15 + BIG_SPACE_SIZE * (scenario.scen_horses[i].horse_loc.x - cen_x + 4);
				rectangle_draw_rect.right = 15 + BIG_SPACE_SIZE * (scenario.scen_horses[i].horse_loc.x - cen_x + 5) + 1;
				rectangle_draw_rect.top = 15 + BIG_SPACE_SIZE * (scenario.scen_horses[i].horse_loc.y - cen_y + 4);
				rectangle_draw_rect.bottom = 15 + BIG_SPACE_SIZE * (scenario.scen_horses[i].horse_loc.y - cen_y + 5) + 1;
				sprintf(str,"%d",i);
 				to_rect = rectangle_draw_rect;
				OffsetRect(&to_rect,3,13);
				if (strlen(str) == 1)
				to_rect.left = to_rect.right - 34;
				if (strlen(str) == 2)
				to_rect.left = to_rect.right - 41;
				win_draw_string_outline(main_dc5,to_rect,str,2,10);
			    }
       	}
	for (i = 0; i < 30; i++) {
	if (scenario.scen_boats[i].which_town == cur_town) {
				rectangle_draw_rect.left = 15 + BIG_SPACE_SIZE * (scenario.scen_boats[i].boat_loc.x - cen_x + 4);
				rectangle_draw_rect.right = 15 + BIG_SPACE_SIZE * (scenario.scen_boats[i].boat_loc.x - cen_x + 5) + 1;
				rectangle_draw_rect.top = 15 + BIG_SPACE_SIZE * (scenario.scen_boats[i].boat_loc.y - cen_y + 4);
				rectangle_draw_rect.bottom = 15 + BIG_SPACE_SIZE * (scenario.scen_boats[i].boat_loc.y - cen_y + 5) + 1;
				sprintf(str,"%d",i);
 				to_rect = rectangle_draw_rect;
				OffsetRect(&to_rect,3,13);
				if (strlen(str) == 1)
				to_rect.left = to_rect.right - 34;
				if (strlen(str) == 2)
				to_rect.left = to_rect.right - 41;
				win_draw_string_outline(main_dc5,to_rect,str,2,10);
			    }
       	}
    	}

		// description rects
		for (i = 0; i < 16; i++){
			if (town.room_rect[i].right > 0) {
				rectangle_draw_rect.left = 15 + BIG_SPACE_SIZE * (town.room_rect[i].left - cen_x + 4);
				rectangle_draw_rect.right = 15 + BIG_SPACE_SIZE * (town.room_rect[i].right - cen_x + 5) - 1;
				rectangle_draw_rect.top = 15 + BIG_SPACE_SIZE * (town.room_rect[i].top - cen_y + 4);
				rectangle_draw_rect.bottom = 15 + BIG_SPACE_SIZE * (town.room_rect[i].bottom - cen_y + 5) - 1;
				MacInsetRect(&rectangle_draw_rect,4,4);
				put_clipped_rect_in_gworld(main_dc5,rectangle_draw_rect,clip_rect,0,255,0);
				}
      }

			 rectangle_draw_rect.left = 15 + BIG_SPACE_SIZE * (town.in_town_rect.left - cen_x + 4);
			 rectangle_draw_rect.right = 15 + BIG_SPACE_SIZE * (town.in_town_rect.right - cen_x + 5) - 1;
			 rectangle_draw_rect.top = 15 + BIG_SPACE_SIZE * (town.in_town_rect.top - cen_y + 4);
			 rectangle_draw_rect.bottom = 15 + BIG_SPACE_SIZE * (town.in_town_rect.bottom - cen_y + 5) - 1;
			 MacInsetRect(&rectangle_draw_rect,5,5);
			 put_clipped_rect_in_gworld(main_dc5,rectangle_draw_rect,clip_rect,255,0,0);
		}

		if (editing_town == FALSE) {

		if (hintbook_mode8 == 0) {
			 for (i = 0; i < NUM_OUT_PLACED_SPECIALS; i++) {
			 if (current_terrain.spec_id[i] != kNO_OUT_SPECIALS) {
				rectangle_draw_rect.left = 15 + BIG_SPACE_SIZE * (current_terrain.special_rects[i].left - cen_x + 4);
				rectangle_draw_rect.right = 15 + BIG_SPACE_SIZE * (current_terrain.special_rects[i].right - cen_x + 5) - 1;
				rectangle_draw_rect.top = 15 + BIG_SPACE_SIZE * (current_terrain.special_rects[i].top - cen_y + 4);
				rectangle_draw_rect.bottom = 15 + BIG_SPACE_SIZE * (current_terrain.special_rects[i].bottom - cen_y + 5) - 1;
				MacInsetRect(&rectangle_draw_rect,1,1);
				put_clipped_rect_in_gworld(main_dc5,rectangle_draw_rect,clip_rect,200,200,255);
				MacInsetRect(&rectangle_draw_rect,1,1);
				put_clipped_rect_in_gworld(main_dc5,rectangle_draw_rect,clip_rect,0,0,255);
				MacInsetRect(&rectangle_draw_rect,1,1);
				put_clipped_rect_in_gworld(main_dc5,rectangle_draw_rect,clip_rect,200,200,255);
				    }
					}
				}

		if (hintbook_mode8 == 1) {
			 for (i = 0; i < 8; i++) {
			 if (current_terrain.sign_locs[i].x >= 0) {
				rectangle_draw_rect.left = 15 + BIG_SPACE_SIZE * (current_terrain.sign_locs[i].x - cen_x + 4);
				rectangle_draw_rect.right = 15 + BIG_SPACE_SIZE * (current_terrain.sign_locs[i].x - cen_x + 5) + 1;
				rectangle_draw_rect.top = 15 + BIG_SPACE_SIZE * (current_terrain.sign_locs[i].y - cen_y + 4);
				rectangle_draw_rect.bottom = 15 + BIG_SPACE_SIZE * (current_terrain.sign_locs[i].y - cen_y + 5) + 1;
				MacInsetRect(&rectangle_draw_rect,1,1);
				put_clipped_rect_in_gworld(main_dc5,rectangle_draw_rect,clip_rect,255,0,0);
				MacInsetRect(&rectangle_draw_rect,1,1);
				put_clipped_rect_in_gworld(main_dc5,rectangle_draw_rect,clip_rect,255,0,0);
				MacInsetRect(&rectangle_draw_rect,1,1);
				put_clipped_rect_in_gworld(main_dc5,rectangle_draw_rect,clip_rect,0,255,0);
				MacInsetRect(&rectangle_draw_rect,1,1);
				put_clipped_rect_in_gworld(main_dc5,rectangle_draw_rect,clip_rect,0,255,0);
				MacInsetRect(&rectangle_draw_rect,1,1);
				put_clipped_rect_in_gworld(main_dc5,rectangle_draw_rect,clip_rect,0,0,255);
				MacInsetRect(&rectangle_draw_rect,1,1);
				put_clipped_rect_in_gworld(main_dc5,rectangle_draw_rect,clip_rect,0,0,255);
				}
			}
		}

	// Outdoor mode: special encs and other rectangles
		// description rects
		for (i = 0; i < 8; i++){
			if (current_terrain.info_rect[i].right > 0) {
				rectangle_draw_rect.left = 15 + BIG_SPACE_SIZE * (current_terrain.info_rect[i].left - cen_x + 4);
				rectangle_draw_rect.right = 15 + BIG_SPACE_SIZE * (current_terrain.info_rect[i].right - cen_x + 5) - 1;
				rectangle_draw_rect.top = 15 + BIG_SPACE_SIZE * (current_terrain.info_rect[i].top - cen_y + 4);
				rectangle_draw_rect.bottom = 15 + BIG_SPACE_SIZE * (current_terrain.info_rect[i].bottom - cen_y + 5) - 1;
				MacInsetRect(&rectangle_draw_rect,4,4);
				put_clipped_rect_in_gworld(main_dc5,rectangle_draw_rect,clip_rect,0,255,0);
				   }
        }
        		// town entry rects
		for (i = 0; i < 8; i++){
			if ((current_terrain.exit_rects[i].right > 0) && (current_terrain.exit_dests[i] >= 0)) {
				rectangle_draw_rect.left = 15 + BIG_SPACE_SIZE * (current_terrain.exit_rects[i].left - cen_x + 4);
				rectangle_draw_rect.right = 15 + BIG_SPACE_SIZE * (current_terrain.exit_rects[i].right - cen_x + 5) - 1;
				rectangle_draw_rect.top = 15 + BIG_SPACE_SIZE * (current_terrain.exit_rects[i].top - cen_y + 4);
				rectangle_draw_rect.bottom = 15 + BIG_SPACE_SIZE * (current_terrain.exit_rects[i].bottom - cen_y + 5) - 1;
				MacInsetRect(&rectangle_draw_rect,5,5);
				put_clipped_rect_in_gworld(main_dc5,rectangle_draw_rect,clip_rect,255,0,255);
			     }
        }

	if (object_display_mode == 1) {
		for (i = 0; i < 30; i++){
			if (current_terrain.special_rects[i].left >= 0) {
				rectangle_draw_rect.left = 15 + BIG_SPACE_SIZE * (current_terrain.special_rects[i].left - cen_x + 4);
				rectangle_draw_rect.right = 15 + BIG_SPACE_SIZE * (current_terrain.special_rects[i].left - cen_x + 5) + 1;
				rectangle_draw_rect.top = 15 + BIG_SPACE_SIZE * (current_terrain.special_rects[i].top - cen_y + 4);
				rectangle_draw_rect.bottom = 15 + BIG_SPACE_SIZE * (current_terrain.special_rects[i].top - cen_y + 5) + 1;
				sprintf(str,"%d",current_terrain.spec_id[i]);
 				to_rect = rectangle_draw_rect;
				OffsetRect(&to_rect,3,13);
				if (strlen(str) == 1)
				to_rect.left = to_rect.right - 34;
				if (strlen(str) == 2)
				to_rect.left = to_rect.right - 41;
				if (strlen(str) == 3)
				to_rect.left = to_rect.right - 48;
				win_draw_string_outline(main_dc5,to_rect,str,2,10);
					}
        }
    	}

	if (object_display_mode == 2) {
		for (i = 0; i < 8; i++){
			if (current_terrain.sign_locs[i].x >= 0) {
				rectangle_draw_rect.left = 15 + BIG_SPACE_SIZE * (current_terrain.sign_locs[i].x - cen_x + 4);
				rectangle_draw_rect.right = 15 + BIG_SPACE_SIZE * (current_terrain.sign_locs[i].x - cen_x + 5) + 1;
				rectangle_draw_rect.top = 15 + BIG_SPACE_SIZE * (current_terrain.sign_locs[i].y - cen_y + 4);
				rectangle_draw_rect.bottom = 15 + BIG_SPACE_SIZE * (current_terrain.sign_locs[i].y - cen_y + 5) + 1;
				sprintf(str,"%d",i);
 				to_rect = rectangle_draw_rect;
				OffsetRect(&to_rect,3,13);
				if (strlen(str) == 1)
				to_rect.left = to_rect.right - 34;
				if (strlen(str) == 2)
				to_rect.left = to_rect.right - 41;
				win_draw_string_outline(main_dc5,to_rect,str,2,10);
					}
        }
    	}
	if (object_display_mode == 3) { // current_terrain.preset
		for (i = 0; i < 8; i++){
			if (current_terrain.preset[i].start_loc.x >= 0) {
				rectangle_draw_rect.left = 15 + BIG_SPACE_SIZE * (current_terrain.preset[i].start_loc.x - cen_x + 4);
				rectangle_draw_rect.right = 15 + BIG_SPACE_SIZE * (current_terrain.preset[i].start_loc.x - cen_x + 5) + 1;
				rectangle_draw_rect.top = 15 + BIG_SPACE_SIZE * (current_terrain.preset[i].start_loc.y - cen_y + 4);
				rectangle_draw_rect.bottom = 15 + BIG_SPACE_SIZE * (current_terrain.preset[i].start_loc.y - cen_y + 5) + 1;
				sprintf(str,"%d",i);
 				to_rect = rectangle_draw_rect;
				OffsetRect(&to_rect,3,13);
				if (strlen(str) == 1)
				to_rect.left = to_rect.right - 34;
				if (strlen(str) == 2)
				to_rect.left = to_rect.right - 41;
				win_draw_string_outline(main_dc5,to_rect,str,2,10);
			    }
        }
    	}

	if (object_display_mode == 4) {
		for (i = 0; i < 8; i++) {
			if (current_terrain.exit_rects[i].left  >= 0) {
				rectangle_draw_rect.left = 15 + BIG_SPACE_SIZE * (current_terrain.exit_rects[i].left - cen_x + 4);
				rectangle_draw_rect.right = 15 + BIG_SPACE_SIZE * (current_terrain.exit_rects[i].left - cen_x + 5) + 1;
				rectangle_draw_rect.top = 15 + BIG_SPACE_SIZE * (current_terrain.exit_rects[i].top - cen_y + 4);
				rectangle_draw_rect.bottom = 15 + BIG_SPACE_SIZE * (current_terrain.exit_rects[i].top - cen_y + 5) + 1;
				sprintf(str,"%d",current_terrain.exit_dests[i]);
 				to_rect = rectangle_draw_rect;
				OffsetRect(&to_rect,3,13);
				if (strlen(str) == 1)
				to_rect.left = to_rect.right - 34;
				if (strlen(str) == 2)
				to_rect.left = to_rect.right - 41;
				if (strlen(str) == 3)
				to_rect.left = to_rect.right - 48;
				win_draw_string_outline(main_dc5,to_rect,str,2,10);
					}
				}
			}

	if (object_display_mode == 5) {
		 }
		 
	if (object_display_mode == 6) {
		 }

	}

	SelectObject(main_dc5,store_font);
	SelectObject(main_dc5,store_bmp);

	// plop ter on screen
	to_rect = whole_area_rect;
	OffsetRect(&to_rect,TER_RECT_UL_X,TER_RECT_UL_Y);
	rect_draw_some_item(ter_draw_gworld,
		whole_area_rect,ter_draw_gworld,to_rect,0,1);

	small_any_drawn = FALSE;
}

void draw_ter_medium()
{
	short q,r,i;
	location where_draw;
	short t_to_draw,floor_to_draw,height_to_draw;
	RECT to_rect;
	graphic_id_type a;
	HBITMAP store_bmp;
	HFONT store_font;

	RECT whole_area_rect = {medium_edit_ter_rects[0][0].left,medium_edit_ter_rects[0][0].top,
		medium_edit_ter_rects[31][31].right,medium_edit_ter_rects[31][31].bottom}; 

	RECT rectangle_draw_rect;
	RECT clip_rect = whole_area_rect;
	MacInsetRect(&clip_rect,15,15);

	ZeroRectCorner(&whole_area_rect);
	SetBkMode(main_dc5,TRANSPARENT);
	store_font = (HFONT)SelectObject(main_dc5,bold_font);
	store_bmp = (HBITMAP) SelectObject(main_dc5,DibBitmapHandle(ter_draw_gworld));
	
	for (q = 0; q < 32; q++)
		for (r = 0; r < 32; r++) {
			where_draw.x = (t_coord)q; where_draw.y = (t_coord)r;

			if((editing_town == FALSE) && ((cen_x + q - 16 < 0) || (cen_x + q - 16 >= 48) || (cen_y + r - 16 < 0) || (cen_y + r - 16 >= 48)) ) {
				short sector_offset_x = ((cen_x + q - 16 < 0) ? -1 : ((cen_x + q - 16 >= 48) ? 1 : 0));
				short sector_offset_y = ((cen_y + r - 16 < 0) ? -1 : ((cen_y + r - 16 >= 48) ? 1 : 0));
				//leave the wood background there
				if(cur_out.x + sector_offset_x < 0 || cur_out.y + sector_offset_y < 0 ||
				cur_out.x + sector_offset_x >= scenario.out_width || cur_out.y + sector_offset_y >= scenario.out_height)
					continue;
				t_to_draw = border_terrains[sector_offset_x + 1][sector_offset_y + 1].terrain[cen_x + q - 16 - sector_offset_x * 48][cen_y + r - 16 - sector_offset_y * 48];
				floor_to_draw = border_terrains[sector_offset_x + 1][sector_offset_y + 1].floor[cen_x + q - 16 - sector_offset_x * 48][cen_y + r - 16 - sector_offset_y * 48];
				height_to_draw = border_terrains[sector_offset_x + 1][sector_offset_y + 1].height[cen_x + q - 16 - sector_offset_x * 48][cen_y + r - 16 - sector_offset_y * 48];
			}

			else if((editing_town == TRUE) && ((cen_x + q - 16 < 0) || (cen_x + q - 16 >= max_dim[town_type]) || (cen_y + r - 16 < 0) || (cen_y + r - 16 >= max_dim[town_type])) ) {

				short temp_x = cen_x + q - 16;
				short temp_y = cen_y + r - 16;

				if(cen_x + q - 16 < 0) {
					temp_x = 0;
				}
				if(cen_x + q - 16 >= max_dim[town_type]) {
					temp_x = max_dim[town_type] - 1;
				}
				if(cen_y + r - 16 < 0) {
					temp_y = 0;
				}
				if(cen_y + r - 16 >= max_dim[town_type]) {
					temp_y = max_dim[town_type] - 1;
				}

				if((temp_x == 0) && (temp_x == town.in_town_rect.left  ) && (temp_y >= town.in_town_rect.top ) && (temp_y <= town.in_town_rect.bottom))
					continue;
				if((temp_x == (max_dim[town_type] - 1))
								 && (temp_x == town.in_town_rect.right ) && (temp_y >= town.in_town_rect.top ) && (temp_y <= town.in_town_rect.bottom))
					continue;
				if((temp_y == 0) && (temp_y == town.in_town_rect.top   ) && (temp_x >= town.in_town_rect.left) && (temp_x <= town.in_town_rect.right ))
					continue;
				if((temp_y == (max_dim[town_type] - 1))
								 && (temp_y == town.in_town_rect.bottom) && (temp_x >= town.in_town_rect.left) && (temp_x <= town.in_town_rect.right ))
					continue;

				floor_to_draw = t_d.floor[temp_x][temp_y];
				t_to_draw = 0;
				height_to_draw = t_d.height[temp_x][temp_y];
				if(town.external_floor_type != -1) {
					floor_to_draw = town.external_floor_type;
				}
			}

			else {
				t_to_draw = (editing_town) ? t_d.terrain[cen_x + q - 16][cen_y + r - 16] : current_terrain.terrain[cen_x + q - 16][cen_y + r - 16];
				floor_to_draw = (editing_town) ? t_d.floor[cen_x + q - 16][cen_y + r - 16] : current_terrain.floor[cen_x + q - 16][cen_y + r - 16];
				height_to_draw = (editing_town) ? t_d.height[cen_x + q - 16][cen_y + r - 16] : current_terrain.height[cen_x + q - 16][cen_y + r - 16];
				}

				// draw floor
			if (hintbook_mode6 == 0)
			a = scen_data.scen_floors[floor_to_draw].ed_pic;
					else
			a = scen_data.scen_floors[254].ed_pic;

				if (a.not_legit()) {
					fill_rect_in_gworld(main_dc5,medium_edit_ter_rects[q][r],230,0,0);
				}
				
				else {
					SelectObject(main_dc5,store_bmp);
					if (place_terrain_icon_into_ter_medium(a,q,r) == FALSE)
						cant_draw_graphics_error(a,"Error was for floor type",floor_to_draw);
					SelectObject(main_dc5,DibBitmapHandle(ter_draw_gworld));
				}

			// draw terrain
		if (hintbook_mode7 == 0) {
			if (t_to_draw > 0) {
				a = scen_data.scen_ter_types[t_to_draw].ed_pic;
				if (a.not_legit())
					 fill_rect_in_gworld(main_dc5,medium_edit_ter_rects[q][r],0,255,0);
				else {
					SelectObject(main_dc5,store_bmp);
					if (place_terrain_icon_into_ter_medium(a,q,r) == FALSE)
						cant_draw_graphics_error(a,"Error was for terrain type",t_to_draw);
					SelectObject(main_dc5,DibBitmapHandle(ter_draw_gworld));
				}
			}
		}

			else
			t_to_draw == 0;

				small_what_drawn[q][r] = t_to_draw;
				small_what_floor_drawn[q][r] = floor_to_draw;
				
			if (editing_town) {
			location loc_drawn = {(t_coord)(cen_x + q - 16), (t_coord)(cen_y + r - 16)};
				SelectObject(main_dc5,store_bmp);
				// draw barrels, etc
			if ((hintbook_mode2 == 0) && (hintbook_mode5 == 0)) {

				if (is_barrel(loc_drawn.x,loc_drawn.y)) {
					a.which_sheet = 680;
					a.which_icon = 75;
					place_terrain_icon_into_ter_medium(a,q,r);
					}
				if (is_crate(loc_drawn.x,loc_drawn.y)) {
					a.which_sheet = 680;
					a.which_icon = 76;
					place_terrain_icon_into_ter_medium(a,q,r);
					}
				if (is_web(loc_drawn.x,loc_drawn.y)) {
					a.which_sheet = 680;
					a.which_icon = 78;
					place_terrain_icon_into_ter_medium(a,q,r);
					}
				if (is_fire_barrier(loc_drawn.x,loc_drawn.y)) {
					a.which_sheet = 686;
					a.which_icon = 40;
					place_terrain_icon_into_ter_medium(a,q,r);
					}
				if (is_force_barrier(loc_drawn.x,loc_drawn.y)) {
					a.which_sheet = 686;
					a.which_icon = 41;
					place_terrain_icon_into_ter_medium(a,q,r);
					}
				if (is_oblique_mirror(loc_drawn.x,loc_drawn.y)) {
					a.which_sheet = 689;
					a.which_icon = 13;
					place_terrain_icon_into_ter_medium(a,q,r);
					}
				if (is_facing_mirror(loc_drawn.x,loc_drawn.y)) {
					a.which_sheet = 689;
					a.which_icon = 12;
					place_terrain_icon_into_ter_medium(a,q,r);
					}
				for (short i = 0; i < 30; i++)
				if ((same_point(loc_drawn,scenario.scen_boats[i].boat_loc)) && (cur_town == scenario.scen_boats[i].which_town)){
					a.which_sheet = 683;
					a.which_icon = 17;
					place_terrain_icon_into_ter_medium(a,q,r);
					}
				for (short i = 0; i < 30; i++)
				if ((same_point(loc_drawn,scenario.scen_horses[i].horse_loc)) && (cur_town == scenario.scen_horses[i].which_town)){
					a.which_sheet = 686;
					a.which_icon = 83;
					place_terrain_icon_into_ter_medium(a,q,r);
					}

					}
					}
					
			if ((editing_town) && (hintbook_mode2 == 0) && (hintbook_mode3 == 0)) {
         SelectObject(main_dc5,store_bmp);
				// draw creatures
				for (i = 0; i < NUM_TOWN_PLACED_CREATURES; i++)
					if ((town.creatures[i].exists()) && (town.creatures[i].start_loc.x == cen_x + q - 16) && (town.creatures[i].start_loc.y == cen_y + r - 16)) {
						a.which_sheet = 4918;
						if (town.creatures[i].hidden_class == 0)
							 a.which_icon = town.creatures[i].start_attitude - 2;
						else
							 a.which_icon = town.creatures[i].start_attitude + 2;
							 
						place_terrain_icon_into_ter_medium(a,q,r);
						}
						}
				SelectObject(main_dc5,DibBitmapHandle(ter_draw_gworld));
			}


    small_any_drawn = TRUE;

	if (editing_town) {

	if (hintbook_mode8 == 0) {
		for (i = 0; i < NUM_TOWN_PLACED_SPECIALS; i++) {
			if (town.spec_id[i] != kNO_TOWN_SPECIALS) {
				rectangle_draw_rect.left = MEDIUM_SPACE_SIZE * (town.special_rects[i].left - cen_x + 16);
				rectangle_draw_rect.right = MEDIUM_SPACE_SIZE * (town.special_rects[i].right - cen_x + 17) - 1;
				rectangle_draw_rect.top = MEDIUM_SPACE_SIZE * (town.special_rects[i].top - cen_y + 16);
				rectangle_draw_rect.bottom = MEDIUM_SPACE_SIZE * (town.special_rects[i].bottom - cen_y + 17) - 1;
				MacInsetRect(&rectangle_draw_rect,1,1);
				put_clipped_rect_in_gworld(main_dc5,rectangle_draw_rect,clip_rect,200,200,255);
				MacInsetRect(&rectangle_draw_rect,1,1);
				put_clipped_rect_in_gworld(main_dc5,rectangle_draw_rect,clip_rect,0,0,255);
				MacInsetRect(&rectangle_draw_rect,1,1);
				put_clipped_rect_in_gworld(main_dc5,rectangle_draw_rect,clip_rect,200,200,255);
				}
			}
		}

	if (hintbook_mode8 == 1) {
		for (i = 0; i < 15; i++) {
			if (town.sign_locs[i].x >= 0) {
				rectangle_draw_rect.left = MEDIUM_SPACE_SIZE * (town.sign_locs[i].x - cen_x + 16);
				rectangle_draw_rect.right = MEDIUM_SPACE_SIZE * (town.sign_locs[i].x - cen_x + 17) - 1;
				rectangle_draw_rect.top = MEDIUM_SPACE_SIZE * (town.sign_locs[i].y - cen_y + 16);
				rectangle_draw_rect.bottom = MEDIUM_SPACE_SIZE * (town.sign_locs[i].y - cen_y + 17) - 1;
				MacInsetRect(&rectangle_draw_rect,1,1);
				put_clipped_rect_in_gworld(main_dc5,rectangle_draw_rect,clip_rect,255,0,0);
				MacInsetRect(&rectangle_draw_rect,1,1);
				put_clipped_rect_in_gworld(main_dc5,rectangle_draw_rect,clip_rect,255,0,0);
				MacInsetRect(&rectangle_draw_rect,1,1);
				put_clipped_rect_in_gworld(main_dc5,rectangle_draw_rect,clip_rect,0,255,0);
				MacInsetRect(&rectangle_draw_rect,1,1);
				put_clipped_rect_in_gworld(main_dc5,rectangle_draw_rect,clip_rect,0,255,0);
				MacInsetRect(&rectangle_draw_rect,1,1);
				put_clipped_rect_in_gworld(main_dc5,rectangle_draw_rect,clip_rect,0,0,255);
				MacInsetRect(&rectangle_draw_rect,1,1);
				put_clipped_rect_in_gworld(main_dc5,rectangle_draw_rect,clip_rect,0,0,255);
				}
			}
		}

	if (hintbook_mode8 == 2) {
		for (i = 0; i < 60; i++) {
			if (town.preset_fields[i].field_loc.x >= 0) {
				rectangle_draw_rect.left = MEDIUM_SPACE_SIZE * (town.preset_fields[i].field_loc.x - cen_x + 16);
				rectangle_draw_rect.right = MEDIUM_SPACE_SIZE * (town.preset_fields[i].field_loc.x - cen_x + 17) - 1;
				rectangle_draw_rect.top = MEDIUM_SPACE_SIZE * (town.preset_fields[i].field_loc.y - cen_y + 16);
				rectangle_draw_rect.bottom = MEDIUM_SPACE_SIZE * (town.preset_fields[i].field_loc.y - cen_y + 17) - 1;
				MacInsetRect(&rectangle_draw_rect,1,1);
				put_clipped_rect_in_gworld(main_dc5,rectangle_draw_rect,clip_rect,0,0,255);
				MacInsetRect(&rectangle_draw_rect,1,1);
				put_clipped_rect_in_gworld(main_dc5,rectangle_draw_rect,clip_rect,0,0,255);
				MacInsetRect(&rectangle_draw_rect,1,1);
				put_clipped_rect_in_gworld(main_dc5,rectangle_draw_rect,clip_rect,0,255,0);
				MacInsetRect(&rectangle_draw_rect,1,1);
				put_clipped_rect_in_gworld(main_dc5,rectangle_draw_rect,clip_rect,0,255,0);
				MacInsetRect(&rectangle_draw_rect,1,1);
				put_clipped_rect_in_gworld(main_dc5,rectangle_draw_rect,clip_rect,255,0,0);
				MacInsetRect(&rectangle_draw_rect,1,1);
				put_clipped_rect_in_gworld(main_dc5,rectangle_draw_rect,clip_rect,255,0,0);
				}
			}
		}

		// description rects
		for (i = 0; i < 16; i++){
			if (town.room_rect[i].right > 0) {
				rectangle_draw_rect.left = MEDIUM_SPACE_SIZE * (town.room_rect[i].left - cen_x + 16);
				rectangle_draw_rect.right = MEDIUM_SPACE_SIZE * (town.room_rect[i].right - cen_x + 17) - 1;
				rectangle_draw_rect.top = MEDIUM_SPACE_SIZE * (town.room_rect[i].top - cen_y + 16);
				rectangle_draw_rect.bottom = MEDIUM_SPACE_SIZE * (town.room_rect[i].bottom - cen_y + 17) - 1;
				MacInsetRect(&rectangle_draw_rect,4,4);
				put_clipped_rect_in_gworld(main_dc5,rectangle_draw_rect,clip_rect,0,255,0);
				}
      }
        		// zone border rect
		rectangle_draw_rect.left = MEDIUM_SPACE_SIZE * (town.in_town_rect.left - cen_x + 16);
		rectangle_draw_rect.right = MEDIUM_SPACE_SIZE * (town.in_town_rect.right - cen_x + 17) - 1;
		rectangle_draw_rect.top = MEDIUM_SPACE_SIZE * (town.in_town_rect.top - cen_y + 16);
		rectangle_draw_rect.bottom = MEDIUM_SPACE_SIZE * (town.in_town_rect.bottom - cen_y + 17) - 1;
		MacInsetRect(&rectangle_draw_rect,4,4);
		put_clipped_rect_in_gworld(main_dc5,rectangle_draw_rect,clip_rect,255,0,0);

	}
	
	if (editing_town == FALSE) {

		if (hintbook_mode8 == 0) {
			 for (i = 0; i < NUM_OUT_PLACED_SPECIALS; i++) {
			 if (current_terrain.spec_id[i] != kNO_OUT_SPECIALS) {
				rectangle_draw_rect.left = MEDIUM_SPACE_SIZE * (current_terrain.special_rects[i].left - cen_x + 16);
				rectangle_draw_rect.right = MEDIUM_SPACE_SIZE * (current_terrain.special_rects[i].right - cen_x + 17) - 1;
				rectangle_draw_rect.top = MEDIUM_SPACE_SIZE * (current_terrain.special_rects[i].top - cen_y + 16);
				rectangle_draw_rect.bottom = MEDIUM_SPACE_SIZE * (current_terrain.special_rects[i].bottom - cen_y + 17) - 1;
				MacInsetRect(&rectangle_draw_rect,1,1);
				put_clipped_rect_in_gworld(main_dc5,rectangle_draw_rect,clip_rect,200,200,255);
				MacInsetRect(&rectangle_draw_rect,1,1);
				put_clipped_rect_in_gworld(main_dc5,rectangle_draw_rect,clip_rect,0,0,255);
				MacInsetRect(&rectangle_draw_rect,1,1);
				put_clipped_rect_in_gworld(main_dc5,rectangle_draw_rect,clip_rect,200,200,255);
				    }
					}
				}

		if (hintbook_mode8 == 1) {
				for (i = 0; i < 8; i++) {
				if (current_terrain.sign_locs[i].x >= 0) {
				rectangle_draw_rect.left = MEDIUM_SPACE_SIZE * (current_terrain.sign_locs[i].x - cen_x + 16);
				rectangle_draw_rect.right = MEDIUM_SPACE_SIZE * (current_terrain.sign_locs[i].x - cen_x + 17) - 1;
				rectangle_draw_rect.top = MEDIUM_SPACE_SIZE * (current_terrain.sign_locs[i].y - cen_y + 16);
				rectangle_draw_rect.bottom = MEDIUM_SPACE_SIZE * (current_terrain.sign_locs[i].y - cen_y + 17) - 1;
				MacInsetRect(&rectangle_draw_rect,1,1);
				put_clipped_rect_in_gworld(main_dc5,rectangle_draw_rect,clip_rect,255,0,0);
				MacInsetRect(&rectangle_draw_rect,1,1);
				put_clipped_rect_in_gworld(main_dc5,rectangle_draw_rect,clip_rect,255,0,0);
				MacInsetRect(&rectangle_draw_rect,1,1);
				put_clipped_rect_in_gworld(main_dc5,rectangle_draw_rect,clip_rect,0,255,0);
				MacInsetRect(&rectangle_draw_rect,1,1);
				put_clipped_rect_in_gworld(main_dc5,rectangle_draw_rect,clip_rect,0,255,0);
				MacInsetRect(&rectangle_draw_rect,1,1);
				put_clipped_rect_in_gworld(main_dc5,rectangle_draw_rect,clip_rect,0,0,255);
				MacInsetRect(&rectangle_draw_rect,1,1);
				put_clipped_rect_in_gworld(main_dc5,rectangle_draw_rect,clip_rect,0,0,255);
				}
			}
		}

		for (i = 0; i < 8; i++){
			if ((current_terrain.exit_rects[i].right > 0) && (current_terrain.exit_dests[i] >= 0)) {
				rectangle_draw_rect.left = MEDIUM_SPACE_SIZE * (current_terrain.exit_rects[i].left - cen_x + 16);
				rectangle_draw_rect.right = MEDIUM_SPACE_SIZE * (current_terrain.exit_rects[i].right - cen_x + 17) - 1;
				rectangle_draw_rect.top = MEDIUM_SPACE_SIZE * (current_terrain.exit_rects[i].top - cen_y + 16);
				rectangle_draw_rect.bottom = MEDIUM_SPACE_SIZE * (current_terrain.exit_rects[i].bottom - cen_y + 17) - 1;
				MacInsetRect(&rectangle_draw_rect,4,4);
				put_clipped_rect_in_gworld(main_dc5,rectangle_draw_rect,clip_rect,255,0,255);
				}
      }

		// description rects
		for (i = 0; i < 8; i++){
			if (current_terrain.info_rect[i].right > 0) {
				rectangle_draw_rect.left = MEDIUM_SPACE_SIZE * (current_terrain.info_rect[i].left - cen_x + 16);
				rectangle_draw_rect.right = MEDIUM_SPACE_SIZE * (current_terrain.info_rect[i].right - cen_x + 17) - 1;
				rectangle_draw_rect.top = MEDIUM_SPACE_SIZE * (current_terrain.info_rect[i].top - cen_y + 16);
				rectangle_draw_rect.bottom = MEDIUM_SPACE_SIZE * (current_terrain.info_rect[i].bottom - cen_y + 17) - 1;
				MacInsetRect(&rectangle_draw_rect,4,4);
				put_clipped_rect_in_gworld(main_dc5,rectangle_draw_rect,clip_rect,0,255,0);
				}
      }
	}

	// draw grid of lines
	if (grid_mode == 1) {
		for (i = 1; i < 32; i++){
			if (i % 4 == 0)
				put_line_in_gworld(main_dc5,0,MEDIUM_SPACE_SIZE * i,MEDIUM_SPACE_SIZE * MAX_TOWN_SIZE - 1,MEDIUM_SPACE_SIZE * i,3 * 8,3 * 8,6 * 8);
		}
		for (i = 1; i < 32; i++){
			if (i % 4 == 0)
				put_line_in_gworld(main_dc5,MEDIUM_SPACE_SIZE * i,0,MEDIUM_SPACE_SIZE * i,MEDIUM_SPACE_SIZE * MAX_TOWN_SIZE - 1,3 * 8,3 * 8,6 * 8);
		}
	}

	if (grid_mode == 2) {
		for (i = 1; i < 32; i++){
				put_line_in_gworld(main_dc5,0,MEDIUM_SPACE_SIZE * i,MEDIUM_SPACE_SIZE * MAX_TOWN_SIZE - 1,MEDIUM_SPACE_SIZE * i,3 * 8,3 * 8,6 * 8);
		}
		for (i = 1; i < 32; i++){
				put_line_in_gworld(main_dc5,MEDIUM_SPACE_SIZE * i,0,MEDIUM_SPACE_SIZE * i,MEDIUM_SPACE_SIZE * MAX_TOWN_SIZE - 1,3 * 8,3 * 8,6 * 8);
		}
	}

	SelectObject(main_dc5,store_font);
	SelectObject(main_dc5,store_bmp);

	// plop ter on screen
	to_rect = whole_area_rect;
	OffsetRect(&to_rect,TER_RECT_UL_X,TER_RECT_UL_Y);
	rect_draw_some_item(ter_draw_gworld,
		whole_area_rect,ter_draw_gworld,to_rect,0,1);
}

void draw_ter_small()
{
	short q,r,i;
	location where_draw;
	short t_to_draw,floor_to_draw;
	RECT to_rect;
	graphic_id_type a;
	HBITMAP store_bmp;
	HFONT store_font;

	// first, clear area not drawn on
	if ((editing_town == FALSE) || (town_type > 0)) {
		RECT fill_area_rect = {small_edit_ter_rects[0][0].left,small_edit_ter_rects[0][0].top,
			small_edit_ter_rects[63][63].right,small_edit_ter_rects[63][63].bottom}; /**/
		fill_area_rect.left = small_edit_ter_rects[((editing_town) ? max_dim[town_type] : 48)][0].left;
 		paint_pattern(ter_draw_gworld,0,fill_area_rect,2);
		fill_area_rect.left = 0;
		fill_area_rect.top = small_edit_ter_rects[0][((editing_town) ? max_dim[town_type] : 48)].top;
 		paint_pattern(ter_draw_gworld,0,fill_area_rect,2);
	}

	RECT whole_area_rect;
	SetRECT(whole_area_rect,small_edit_ter_rects[0][0].left,small_edit_ter_rects[0][0].top,
		small_edit_ter_rects[MAX_TOWN_SIZE - 1][MAX_TOWN_SIZE - 1].right,small_edit_ter_rects[MAX_TOWN_SIZE - 1][MAX_TOWN_SIZE - 1].bottom);

	ZeroRectCorner(&whole_area_rect);

	SetBkMode(main_dc5,TRANSPARENT);
	store_font = (HFONT)SelectObject(main_dc5,bold_font);
	store_bmp = (HBITMAP) SelectObject(main_dc5,DibBitmapHandle(ter_draw_gworld));

	for (q = 0; q < ((editing_town) ? max_dim[town_type] : 48); q++)
		for (r = 0; r < ((editing_town) ? max_dim[town_type] : 48); r++) {
			where_draw.x = (t_coord)q; where_draw.y = (t_coord)r;

			t_to_draw = (editing_town) ? t_d.terrain[q][r] : current_terrain.terrain[q][r];
			floor_to_draw = (editing_town) ? t_d.floor[q][r] : current_terrain.floor[q][r];

			if ((small_any_drawn == FALSE) || (small_what_drawn[q][r] != t_to_draw) ||
				(small_what_floor_drawn[q][r] != floor_to_draw)) {

				// draw floor
			if (hintbook_mode6 == 0)
			a = scen_data.scen_floors[floor_to_draw].ed_pic;
					else
			a = scen_data.scen_floors[254].ed_pic;

				if (a.not_legit()) {
					fill_rect_in_gworld(main_dc5,small_edit_ter_rects[q][r],230,0,0);
				}
				else {
					SelectObject(main_dc5,store_bmp);
					if (place_terrain_icon_into_ter_small(a,q,r) == FALSE)
						cant_draw_graphics_error(a,"Error was for floor type",floor_to_draw);
					SelectObject(main_dc5,DibBitmapHandle(ter_draw_gworld));
				}
			// draw terrain
			if (hintbook_mode7 == 0) {
			if (t_to_draw > 0) {
				a = scen_data.scen_ter_types[t_to_draw].ed_pic;
				if (a.not_legit())
					 fill_rect_in_gworld(main_dc5,small_edit_ter_rects[q][r],0,255,0);
				else {
					SelectObject(main_dc5,store_bmp);
					if (place_terrain_icon_into_ter_small(a,q,r) == FALSE)
						cant_draw_graphics_error(a,"Error was for terrain type",t_to_draw);
					SelectObject(main_dc5,DibBitmapHandle(ter_draw_gworld));
				}
			}
		}

			else
			t_to_draw == 0;

				small_what_drawn[q][r] = t_to_draw;
				small_what_floor_drawn[q][r] = floor_to_draw;
			}
	RECT rectangle_draw_rect;
	RECT clip_rect = whole_area_rect;
	MacInsetRect(&clip_rect,8,8);

	if (editing_town == FALSE) {

	if (hintbook_mode9 == 1) {

			 for (i = 0; i < NUM_OUT_PLACED_SPECIALS; i++) {
			 if (current_terrain.spec_id[i] != kNO_OUT_SPECIALS) {
				rectangle_draw_rect.left = SMALL_SPACE_SIZE * (current_terrain.special_rects[i].left);
				rectangle_draw_rect.right = SMALL_SPACE_SIZE * (current_terrain.special_rects[i].right + 1) - 1;
				rectangle_draw_rect.top = SMALL_SPACE_SIZE * (current_terrain.special_rects[i].top);
				rectangle_draw_rect.bottom = SMALL_SPACE_SIZE * (current_terrain.special_rects[i].bottom + 1) - 1;
				MacInsetRect(&rectangle_draw_rect,1,1);
				put_clipped_rect_in_gworld(main_dc5,rectangle_draw_rect,clip_rect,200,200,255);
				MacInsetRect(&rectangle_draw_rect,1,1);
				put_clipped_rect_in_gworld(main_dc5,rectangle_draw_rect,clip_rect,0,0,255);
				MacInsetRect(&rectangle_draw_rect,1,1);
				put_clipped_rect_in_gworld(main_dc5,rectangle_draw_rect,clip_rect,200,200,255);
				    }
					}
				}
	if (hintbook_mode9 == 2) {
		// description rects
			 for (i = 0; i < 8; i++){
			 if (current_terrain.info_rect[i].right > 0) {
				rectangle_draw_rect.left = SMALL_SPACE_SIZE * (current_terrain.info_rect[i].left);
				rectangle_draw_rect.right = SMALL_SPACE_SIZE * (current_terrain.info_rect[i].right + 1) - 1;
				rectangle_draw_rect.top = SMALL_SPACE_SIZE * (current_terrain.info_rect[i].top);
				rectangle_draw_rect.bottom = SMALL_SPACE_SIZE * (current_terrain.info_rect[i].bottom + 1) - 1;
				MacInsetRect(&rectangle_draw_rect,2,2);
				put_clipped_rect_in_gworld(main_dc5,rectangle_draw_rect,clip_rect,0,255,0);
				}
      }
		for (i = 0; i < 8; i++){
			if ((current_terrain.exit_rects[i].right > 0) && (current_terrain.exit_dests[i] >= 0)) {
				rectangle_draw_rect.left = SMALL_SPACE_SIZE * (current_terrain.exit_rects[i].left);
				rectangle_draw_rect.right = SMALL_SPACE_SIZE * (current_terrain.exit_rects[i].right + 1) - 1;
				rectangle_draw_rect.top = SMALL_SPACE_SIZE * (current_terrain.exit_rects[i].top);
				rectangle_draw_rect.bottom = SMALL_SPACE_SIZE * (current_terrain.exit_rects[i].bottom + 1) - 1;
				MacInsetRect(&rectangle_draw_rect,2,2);
				put_clipped_rect_in_gworld(main_dc5,rectangle_draw_rect,clip_rect,255,0,255);
				}
      }
		}
      else {
				 continue;
			 }
	}

			// draw creatures
			if (editing_town) {

			if ((hintbook_mode2 == 0) && (hintbook_mode3 == 0)) {
				for (i = 0; i < NUM_TOWN_PLACED_CREATURES; i++){
					if ((town.creatures[i].exists()) && (town.creatures[i].start_loc.x == q) && (town.creatures[i].start_loc.y == r)) {
					  	to_rect = small_edit_ter_rects[q][r];
					  	MacInsetRect(&to_rect,1,1);
						if (town.creatures[i].start_attitude < 3)
      				put_rect_in_gworld(main_dc5,to_rect,0,0,255);
						else if (town.creatures[i].start_attitude == 3)
							put_rect_in_gworld(main_dc5,to_rect,0,255,0);
						else if (town.creatures[i].start_attitude == 4)
							put_rect_in_gworld(main_dc5,to_rect,255,0,0);
						else
							put_rect_in_gworld(main_dc5,to_rect,190,100,0);

					}
				}
			}

	if (hintbook_mode9 == 1) {
		// description rects
		for (i = 0; i < NUM_TOWN_PLACED_SPECIALS; i++) {
			if (town.spec_id[i] != kNO_TOWN_SPECIALS) {
				rectangle_draw_rect.left = SMALL_SPACE_SIZE * (town.special_rects[i].left);
				rectangle_draw_rect.right = SMALL_SPACE_SIZE * (town.special_rects[i].right + 1) - 1;
				rectangle_draw_rect.top = SMALL_SPACE_SIZE * (town.special_rects[i].top);
				rectangle_draw_rect.bottom = SMALL_SPACE_SIZE * (town.special_rects[i].bottom + 1) - 1;
				MacInsetRect(&rectangle_draw_rect,1,1);
				put_clipped_rect_in_gworld(main_dc5,rectangle_draw_rect,clip_rect,200,200,255);
				MacInsetRect(&rectangle_draw_rect,1,1);
				put_clipped_rect_in_gworld(main_dc5,rectangle_draw_rect,clip_rect,0,0,255);
				MacInsetRect(&rectangle_draw_rect,1,1);
				put_clipped_rect_in_gworld(main_dc5,rectangle_draw_rect,clip_rect,200,200,255);
				}
			}
		}
		
	if (hintbook_mode9 == 2) {
			 for (i = 0; i < 16; i++) {
			 if (town.room_rect[i].right > 0) {
				rectangle_draw_rect.left = SMALL_SPACE_SIZE * (town.room_rect[i].left);
				rectangle_draw_rect.right = SMALL_SPACE_SIZE * (town.room_rect[i].right + 1) - 1;
				rectangle_draw_rect.top = SMALL_SPACE_SIZE * (town.room_rect[i].top);
				rectangle_draw_rect.bottom = SMALL_SPACE_SIZE * (town.room_rect[i].bottom + 1) - 1;
				MacInsetRect(&rectangle_draw_rect,2,2);
				put_clipped_rect_in_gworld(main_dc5,rectangle_draw_rect,clip_rect,0,255,0);
				}
      }
				rectangle_draw_rect.left = SMALL_SPACE_SIZE * (town.in_town_rect.left);
				rectangle_draw_rect.right = SMALL_SPACE_SIZE * (town.in_town_rect.right + 1) - 1;
				rectangle_draw_rect.top = SMALL_SPACE_SIZE * (town.in_town_rect.top);
				rectangle_draw_rect.bottom = SMALL_SPACE_SIZE * (town.in_town_rect.bottom + 1) - 1;
				MacInsetRect(&rectangle_draw_rect,2,2);
				put_clipped_rect_in_gworld(main_dc5,rectangle_draw_rect,clip_rect,255,0,0);
      }
  else {
						 continue;
			 }
    }
    small_any_drawn = TRUE;

put_rect_in_gworld( main_dc5,terrain_rect_gr_size,0,0,0);
										}
	// draw grid of lines
	if (grid_mode == 1) {
		for (i = 1; i < 64; i++){
			if (i % 8 == 0)
				put_line_in_gworld(main_dc5,0,SMALL_SPACE_SIZE * i,SMALL_SPACE_SIZE * MAX_TOWN_SIZE - 1,SMALL_SPACE_SIZE * i,3 * 8,3 * 8,6 * 8);
		}
		for (i = 1; i < 64; i++){
			if (i % 8 == 0)
				put_line_in_gworld(main_dc5,SMALL_SPACE_SIZE * i,0,SMALL_SPACE_SIZE * i,SMALL_SPACE_SIZE * MAX_TOWN_SIZE - 1,3 * 8,3 * 8,6 * 8);
		}
	}
	if (grid_mode == 2) {
		for (i = 1; i < 64; i++){
				put_line_in_gworld(main_dc5,0,SMALL_SPACE_SIZE * i,SMALL_SPACE_SIZE * MAX_TOWN_SIZE - 1,SMALL_SPACE_SIZE * i,3 * 8,3 * 8,6 * 8);
		}
		for (i = 1; i < 64; i++){
				put_line_in_gworld(main_dc5,SMALL_SPACE_SIZE * i,0,SMALL_SPACE_SIZE * i,SMALL_SPACE_SIZE * MAX_TOWN_SIZE - 1,3 * 8,3 * 8,6 * 8);
		}
	}

	SelectObject(main_dc5,store_font);
	SelectObject(main_dc5,store_bmp);

	// plop ter on screen
	to_rect = whole_area_rect;
	OffsetRect(&to_rect,TER_RECT_UL_X,TER_RECT_UL_Y);
	rect_draw_some_item(ter_draw_gworld,
		whole_area_rect,ter_draw_gworld,to_rect,0,1);
}

void draw_terrain()
{
	if (cur_viewing_mode == 0) 
		draw_ter_large();
		
	if (cur_viewing_mode == 1) 
			draw_ter_small();
			
	if (cur_viewing_mode == 2)
		draw_ter_medium();

	if (cur_viewing_mode == 10 || cur_viewing_mode == 11) 
		draw_ter_3D_large();

	place_left_text();
	
	place_right_buttons();
}

Boolean place_terrain_icon_into_ter_large(graphic_id_type icon,short in_square_x,short in_square_y)
{
	RECT to_rect = large_edit_ter_rects[in_square_x][in_square_y];
	RECT from_rect;
	graphic_id_type a = icon;

	short index = safe_get_index_of_sheet(&a);
	if (index < 0) {
		return FALSE;
	}

	SetRECT(from_rect,1 + (TER_BUTTON_SIZE + 1) * (a.which_icon % 10),1 + (TER_BUTTON_SIZE + 1) * (a.which_icon / 10),1 + (TER_BUTTON_SIZE + 1) * (a.which_icon % 10) + TER_BUTTON_SIZE,1 + (TER_BUTTON_SIZE + 1) * (a.which_icon / 10) + TER_BUTTON_SIZE);
	RECT buffer_to_rect = to_rect;
	ZeroRectCorner(&buffer_to_rect);
	rect_draw_some_item(graphics_library[index],from_rect,ter_resize_buffer_gworld,buffer_to_rect,0,0);
	rect_draw_some_item(ter_resize_buffer_gworld,buffer_to_rect,ter_draw_gworld,to_rect,1,0);

	return TRUE;
}

Boolean place_terrain_icon_into_ter_medium(graphic_id_type icon,short in_square_x,short in_square_y)
{
	RECT to_rect = medium_edit_ter_rects[in_square_x][in_square_y];
	RECT from_rect;
	graphic_id_type a = icon;

	short index = safe_get_index_of_sheet(&a);
	if (index < 0) {
		return FALSE;
	}

	SetRECT(from_rect,1 + (TER_BUTTON_SIZE + 1) * (a.which_icon % 10),1 + (TER_BUTTON_SIZE + 1) * (a.which_icon / 10),1 + (TER_BUTTON_SIZE + 1) * (a.which_icon % 10) + TER_BUTTON_SIZE,1 + (TER_BUTTON_SIZE + 1) * (a.which_icon / 10) + TER_BUTTON_SIZE);
	RECT buffer_to_rect = to_rect;
	ZeroRectCorner(&buffer_to_rect);
	rect_draw_some_item(graphics_library[index],from_rect,ter_resize_buffer_gworld,buffer_to_rect,0,0);
	rect_draw_some_item(ter_resize_buffer_gworld,buffer_to_rect,ter_draw_gworld,to_rect,1,0);

	return TRUE;
}

Boolean place_terrain_icon_into_ter_small(graphic_id_type icon,short in_square_x,short in_square_y)
{
	RECT to_rect = small_edit_ter_rects[in_square_x][in_square_y];
	RECT from_rect;

	graphic_id_type a = icon;

	short index = safe_get_index_of_sheet(&a);
	if (index < 0)
		return FALSE;

	SetRECT(from_rect,1 + (TER_BUTTON_SIZE + 1) * (a.which_icon % 10),1 + (TER_BUTTON_SIZE + 1) * (a.which_icon / 10),1 + (TER_BUTTON_SIZE + 1) * (a.which_icon % 10) + TER_BUTTON_SIZE,1 + (TER_BUTTON_SIZE + 1) * (a.which_icon / 10) + TER_BUTTON_SIZE);

	RECT buffer_to_rect = to_rect;
	ZeroRectCorner(&buffer_to_rect);
	rect_draw_some_item(graphics_library[index],from_rect,ter_resize_buffer_gworld,buffer_to_rect,0,0);
	rect_draw_some_item(ter_resize_buffer_gworld,buffer_to_rect,ter_draw_gworld,to_rect,1,0);

	return TRUE;
}

void place_ter_icon_on_tile(short tile_x,short tile_y,short position,short which_icon)
{
	RECT tiny_to = large_edit_ter_rects[tile_x][tile_y];
	tiny_to.right = tiny_to.left + 10;
	tiny_to.bottom = tiny_to.top + 10;
	OffsetRect(&tiny_to,10 * (position / 4) + 1,10 * (position % 4) + 1);
	
	RECT tiny_from = base_small_button_from;
	OffsetRect(&tiny_from,10 * (which_icon % 10),10 * (which_icon / 10));
	rect_draw_some_item(editor_mixed,tiny_from,ter_draw_gworld,tiny_to,0,0);
}

// obj_num is num of object to drawn (in zone's list of objects)
// loc_game_rect is rect on screen of object being drawn
void draw_creature(HDC ter_hdc,HBITMAP store_bmp,short creature_num,location loc_drawn,short in_square_x,short in_square_y)
{
	graphic_id_type a;
	RECT from_rect;
	short r = 0, g = 0, b = 0;
	char str[256];
	
	if (town.creatures[creature_num].exists() == FALSE)
		return;
		
	if (same_point(town.creatures[creature_num].start_loc,loc_drawn)){
		RECT base_rect = large_edit_ter_rects[in_square_x][in_square_y];
		a = scen_data.scen_creatures[town.creatures[creature_num].number].char_graphic;

		from_rect = get_template_from_rect(0,0);
		MacInsetRect(&from_rect,3,6);
		RECT to_rect = from_rect;
		CenterRectInRect(&to_rect,&base_rect);
		
		short obj_index = safe_get_index_of_sheet(&a);
		if (obj_index < 0) {
			cant_draw_graphics_error(a,"Error was for creature type",town.creatures[creature_num].number);
		}		
		else 
            rect_draw_some_item(graphics_library[obj_index], from_rect,ter_draw_gworld,to_rect,0,0);

		if (town.creatures[creature_num].start_attitude < 3) {
			b = 255;
				if (town.creatures[creature_num].hidden_class > 0)
				g = 255;
		}
		else if (town.creatures[creature_num].start_attitude == 3) {
			r = 0, g = 0, b = 0;
				if (town.creatures[creature_num].hidden_class > 0)
				g = 255;
			}
		else if (town.creatures[creature_num].start_attitude == 4) {
			r = 255;
				if (town.creatures[creature_num].hidden_class > 0)
				g = 255;
			}
		else {
			r = 190, g = 100, b = 0;
			if (town.creatures[creature_num].hidden_class > 0)
			r = 255, g = 0, b = 128;
			}
			
		SelectObject(ter_hdc,DibBitmapHandle(ter_draw_gworld));
		put_rect_in_gworld(ter_hdc,to_rect,r,g,b);

		// do facing
		RECT facing_to_rect = to_rect;
		MacInsetRect(&facing_to_rect,1,1);
		switch (town.creatures[creature_num].facing) {
			case 0: MacInsetRect(&facing_to_rect,3,0); facing_to_rect.bottom = facing_to_rect.top + 2; break;
			case 1: MacInsetRect(&facing_to_rect,0,3); facing_to_rect.right = facing_to_rect.left + 2; break;
			case 2: MacInsetRect(&facing_to_rect,3,0); facing_to_rect.top = facing_to_rect.bottom - 2; break;
			case 3: MacInsetRect(&facing_to_rect,0,3); facing_to_rect.left = facing_to_rect.right - 2; break;
		}
		put_rect_in_gworld(ter_hdc,facing_to_rect,0,0,0);
		SelectObject(ter_hdc,store_bmp);
		  
		// Labels for wandering and hidden
		if (town.creatures[creature_num].hidden_class > 0) {
			sprintf(str,"H");
		}
	}
}


void draw_creature_medium(HDC ter_hdc,HBITMAP store_bmp,short creature_num,location loc_drawn,short in_square_x,short in_square_y)
{
	graphic_id_type a;
	RECT from_rect;
	short r = 0, g = 0, b = 0;
	char str[256];

	if (town.creatures[creature_num].exists() == FALSE)
		return;

	if (same_point(town.creatures[creature_num].start_loc,loc_drawn)){
		RECT base_rect = medium_edit_ter_rects[in_square_x][in_square_y];
		a = scen_data.scen_creatures[town.creatures[creature_num].number].char_graphic;

		from_rect = get_template_from_medium_rect(0,3);
		MacInsetRect(&from_rect,0,0);
		RECT to_rect = from_rect;
		CenterRectInRect(&to_rect,&base_rect);

		short obj_index = safe_get_index_of_sheet(&a);
		if (obj_index < 0) {
			cant_draw_graphics_error(a,"Error was for creature type",town.creatures[creature_num].number);
		}
		else
            rect_draw_some_item(graphics_library[obj_index], from_rect,ter_draw_gworld,to_rect,0,0);

		if (town.creatures[creature_num].start_attitude < 3) {
			b = 255;
				if (town.creatures[creature_num].hidden_class > 0)
				g = 255;
		}
		else if (town.creatures[creature_num].start_attitude == 3) {
			r = 0, g = 0, b = 0;
				if (town.creatures[creature_num].hidden_class > 0)
				g = 255;
			}
		else if (town.creatures[creature_num].start_attitude == 4) {
			r = 255;
				if (town.creatures[creature_num].hidden_class > 0)
				g = 255;
			}
		else {
			r = 190, g = 100, b = 0;
			if (town.creatures[creature_num].hidden_class > 0)
			r = 255, g = 0, b = 128;
			}

		SelectObject(ter_hdc,DibBitmapHandle(ter_draw_gworld));
		put_rect_in_gworld(ter_hdc,to_rect,r,g,b);

		// do facing
		RECT facing_to_rect = to_rect;
		MacInsetRect(&facing_to_rect,1,1);
		switch (town.creatures[creature_num].facing) {
			case 0:
					 MacInsetRect(&facing_to_rect,3,0);
					 facing_to_rect.bottom = facing_to_rect.top + 2;
			break;
			case 1:
					 MacInsetRect(&facing_to_rect,0,3);
					 facing_to_rect.right = facing_to_rect.left + 2;
			break;
			case 2:
					 MacInsetRect(&facing_to_rect,3,0);
					 facing_to_rect.top = facing_to_rect.bottom - 2;
			break;
			case 3:
					 MacInsetRect(&facing_to_rect,0,3);
					 facing_to_rect.left = facing_to_rect.right - 2;
			break;
		}
		put_rect_in_gworld(ter_hdc,facing_to_rect,0,0,0);
		SelectObject(ter_hdc,store_bmp);

		// Labels for wandering and hidden
		if (town.creatures[creature_num].hidden_class > 0) {
			sprintf(str,"H");
		}
	}
}


void draw_item(HDC ter_hdc,HBITMAP store_bmp,short item_num,location loc_drawn,short in_square_x,short in_square_y)
{
	graphic_id_type a;
	short icon_to_use;
	RECT from_rect;
		
	if (town.preset_items[item_num].exists() == FALSE)
		return;
		
	if (same_point(town.preset_items[item_num].item_loc,loc_drawn)) {
		RECT to_rect = large_edit_ter_rects[in_square_x][in_square_y];
		MacInsetRect(&to_rect,10,10);
		OffsetRect(&to_rect, town.preset_items[item_num].item_shift.x,town.preset_items[item_num].item_shift.y);
		
		a = scen_data.scen_items[town.preset_items[item_num].which_item].item_floor_graphic;

		icon_to_use = scen_data.scen_items[town.preset_items[item_num].which_item].inven_icon;

		SetRECT(from_rect,1 + 29 * (icon_to_use % 10),
		  1 + 29 * (icon_to_use / 10),
		  1 + 29 * (icon_to_use % 10) + 28,
		  1 + 29 * (icon_to_use / 10) + 28);
		
		short obj_index = safe_get_index_of_sheet(&a);
		if (obj_index < 0) {
			cant_draw_graphics_error(a,"Error was for item type",town.preset_items[item_num].which_item);
			SelectObject(ter_hdc,DibBitmapHandle(ter_draw_gworld));
			put_rect_in_gworld(ter_hdc,to_rect,0,0,0);
			SelectObject(ter_hdc,store_bmp);
			return;
		}		
		
		rect_draw_some_item(graphics_library[obj_index],
			from_rect,ter_draw_gworld,to_rect,0,0);

		SelectObject(ter_hdc,DibBitmapHandle(ter_draw_gworld));
		put_rect_in_gworld(ter_hdc,to_rect,0,0,0);
		SelectObject(ter_hdc,store_bmp);
	}
}

void draw_ter_script(short script_num,location loc_drawn,short in_square_x,short in_square_y)
{
	char str[256];
	RECT to_rect = large_edit_ter_rects[in_square_x][in_square_y];
	RECT ter_script_icon_from = {100,103,120,123}; 
	if (town.ter_scripts[script_num].exists == FALSE)
		return;
		
	if (same_point(town.ter_scripts[script_num].loc,loc_drawn)) {
		to_rect = large_edit_ter_rects[in_square_x][in_square_y];
		to_rect.left = to_rect.right - 20;
		to_rect.bottom = to_rect.top + 20;
		rect_draw_some_item(editor_mixed,ter_script_icon_from,ter_draw_gworld,to_rect,0,0);
		}
}


void reset_small_drawn()
{
	for (short q = 0; q < 64; q++){
		for (short r = 0; r < 64; r++){
			small_what_drawn[q][r] = small_what_floor_drawn[q][r] = -1;
		}
    }
}


// Takes care of everything on the left side of the screen that isn't terrain. 
// Cleans up screen with background and puts 8 text strings.
void place_left_text()
{
	char draw_str[256];
	
	if ((cur_viewing_mode == 1) || (cur_viewing_mode == 2))
		return;

	for (short i = 0; i < 14; i++)
 		paint_pattern(NULL,1,left_text_lines[i],0);

	HFONT store_font = (HFONT)SelectObject(main_dc,bold_font);
	
	if (file_is_loaded == FALSE) {
		sprintf((char *) draw_str,"No Scenario Loaded"); 
		char_win_draw_string(main_dc,left_text_lines[0],(char *) draw_str,2,10);		
		SelectObject(main_dc,store_font);
		return;
	}
		
	if (editing_town){
		// Erase and draw bottom text strs
		if ((selected_item_number >= 7000) && (selected_item_number < 7000 + NUM_TOWN_PLACED_CREATURES)) {
			sprintf((char *) draw_str,"Creature %d: %s",(int)(selected_item_number % 1000 + 6),
				scen_data.scen_creatures[town.creatures[selected_item_number % 1000].number].name); 
			char_win_draw_string(main_dc,left_text_lines[0],(char *) draw_str,2,10);		

			sprintf((char *) draw_str,"  Edit This Creature  (Type %d, L%d)",
				(int)town.creatures[selected_item_number % 1000].number,
				(int)scen_data.scen_creatures[town.creatures[selected_item_number % 1000].number].level); 
			char_win_draw_string(main_dc,left_text_lines[1],(char *) draw_str,2,10);	

			sprintf((char *) draw_str,"  Attitude: %s",
			  attitude_types[town.creatures[selected_item_number % 1000].start_attitude - 2]); 
			char_win_draw_string(main_dc,left_text_lines[2],(char *) draw_str,2,10);
			
			if (strlen(town.creatures[selected_item_number % 1000].char_script) <= 0)
				sprintf((char *) draw_str,"  Script: Default");
			else sprintf((char *) draw_str,"  Script: %s",
			  	town.creatures[selected_item_number % 1000].char_script);
			char_win_draw_string(main_dc,left_text_lines[3],(char *) draw_str,2,10);

			sprintf((char *) draw_str,"  Personality: %d",
			   (int)town.creatures[selected_item_number % 1000].personality);
			char_win_draw_string(main_dc,left_text_lines[4],(char *) draw_str,2,10);

			sprintf((char *) draw_str,"  Character ID: %d",
			   (int)town.creatures[selected_item_number % 1000].character_id); 
			char_win_draw_string(main_dc,left_text_lines[5],(char *) draw_str,2,10);

			sprintf((char *) draw_str,"  Hidden Class: %d",
			  (int)town.creatures[selected_item_number % 1000].hidden_class);
			char_win_draw_string(main_dc,left_text_lines[6],(char *) draw_str,2,10);

			sprintf((char *) draw_str,"  Creature Location: x = %d, y = %d",
				   (int)town.creatures[selected_item_number % 1000].start_loc.x,
				   (int)town.creatures[selected_item_number % 1000].start_loc.y);
			char_win_draw_string(main_dc,left_text_lines[7],(char *) draw_str,2,10);
	
			sprintf((char *) draw_str,"  Attached Event: %d",
			  (int)town.creatures[selected_item_number % 1000].attached_event);
			char_win_draw_string(main_dc,left_text_lines[8],(char *) draw_str,2,10);

			sprintf((char *) draw_str,"  Facing: %s",
			  facings[town.creatures[selected_item_number % 1000].facing]); 
			char_win_draw_string(main_dc,left_text_lines[9],(char *) draw_str,2,10);		

			for (short i = 0; i < 4; i++) {
				sprintf((char *) draw_str,"  Memory Cell %d: %d",
				   (int)i, (int)town.creatures[selected_item_number % 1000].memory_cells[i]);
				char_win_draw_string(main_dc,left_text_lines[i + 10],(char *) draw_str,2,10);
			}
		}
			
			
		if ((selected_item_number >= 9000) && (selected_item_number < 9000 + NUM_TER_SCRIPTS)) {
			sprintf((char *) draw_str,"Terrain Script %d:", (int)(selected_item_number % 1000)); 
			char_win_draw_string(main_dc,left_text_lines[0],(char *) draw_str,2,10);		

			sprintf((char *) draw_str,"  Edit This Script");
			char_win_draw_string(main_dc,left_text_lines[1],(char *) draw_str,2,10);

			for (short i = 0; i < 5; i++) {
				sprintf((char *) draw_str,"  Memory Cell %d: %d",
				   (int)i, (int)town.ter_scripts[selected_item_number % 1000].memory_cells[i]);
				char_win_draw_string(main_dc,left_text_lines[i + 2],(char *) draw_str,2,10);
			}

			sprintf((char *) draw_str,"  Script Location: x = %d, y = %d",
				   (int)town.ter_scripts[selected_item_number % 1000].loc.x,
				   (int)town.ter_scripts[selected_item_number % 1000].loc.y);
			char_win_draw_string(main_dc,left_text_lines[7],(char *) draw_str,2,10);

			sprintf((char *) draw_str,"  Script: %s",
			  	 town.ter_scripts[selected_item_number % 1000].script_name); 
			char_win_draw_string(main_dc,left_text_lines[8],(char *) draw_str,2,10);

			for (short i = 5; i < 10; i++) {
				sprintf((char *) draw_str,"  Memory Cell %d: %d",
				   (int)i, (int)town.ter_scripts[selected_item_number % 1000].memory_cells[i]); 
				char_win_draw_string(main_dc,left_text_lines[i + 4],(char *) draw_str,2,10);
			}
		}

		if ((selected_item_number >= 11000) && (selected_item_number < 11000 + NUM_TOWN_PLACED_ITEMS)) {

			sprintf((char *) draw_str,"Item %d: %s", (int)(selected_item_number % 1000),
			scen_data.scen_items[town.preset_items[selected_item_number % 1000].which_item].full_name);
			char_win_draw_string(main_dc,left_text_lines[0],(char *) draw_str,2,10);

			sprintf((char *) draw_str,"  Edit This Placed Item  (Type %d)",
				(int)town.preset_items[selected_item_number % 1000].which_item);
			char_win_draw_string(main_dc,left_text_lines[1],(char *) draw_str,2,10);

			if (town.preset_items[selected_item_number % 1000].properties & 1)
						sprintf((char *) draw_str,"  Identified");
			else sprintf((char *) draw_str,"  Not Identified");
			char_win_draw_string(main_dc,left_text_lines[2],(char *) draw_str,2,10);

			if (town.preset_items[selected_item_number % 1000].properties & 2)
						sprintf((char *) draw_str,"  Property");
			else sprintf((char *) draw_str,"  Not Property");
			char_win_draw_string(main_dc,left_text_lines[3],(char *) draw_str,2,10);		

			if (town.preset_items[selected_item_number % 1000].properties & 4)
						sprintf((char *) draw_str,"  Contained");
			else sprintf((char *) draw_str,"  Not Contained");
			char_win_draw_string(main_dc,left_text_lines[4],(char *) draw_str,2,10);		

			if (town.preset_items[selected_item_number % 1000].properties & 8)
						sprintf((char *) draw_str,"  Cursed");
			else sprintf((char *) draw_str,"  Not Cursed");
			char_win_draw_string(main_dc,left_text_lines[5],(char *) draw_str,2,10);

			if (town.preset_items[selected_item_number % 1000].properties & 16)
						sprintf((char *) draw_str,"  Once per Day");
			else sprintf((char *) draw_str,"  Not Once per Day");
			char_win_draw_string(main_dc,left_text_lines[6],(char *) draw_str,2,10);

			sprintf((char *) draw_str,"  Placed Item Location: x = %d, y = %d",
			   (int)town.preset_items[selected_item_number % 1000].item_loc.x,
			   (int)town.preset_items[selected_item_number % 1000].item_loc.y);
			char_win_draw_string(main_dc,left_text_lines[7],(char *) draw_str,2,10);

						sprintf((char *) draw_str,"  Drawing Shift X: %d",
			  (int)town.preset_items[selected_item_number % 1000].item_shift.x);
			char_win_draw_string(main_dc,left_text_lines[8],(char *) draw_str,2,10);
			sprintf((char *) draw_str,"  Drawing Shift Y: %d",
			  (int)town.preset_items[selected_item_number % 1000].item_shift.y);
			char_win_draw_string(main_dc,left_text_lines[9],(char *) draw_str,2,10);

			if (scen_data.scen_items[town.preset_items[selected_item_number % 1000].which_item].charges > 0)
				sprintf((char *) draw_str,"  Charges/Amount: %d",(int)town.preset_items[selected_item_number % 1000].charges);
			else sprintf((char *) draw_str,"  Needs no Charges");
				char_win_draw_string(main_dc,left_text_lines[10],(char *) draw_str,2,10);

			for (short i = 11; i < 14; i++) {
				sprintf((char *) draw_str,"  ");
				char_win_draw_string(main_dc,left_text_lines[i],(char *) draw_str,2,10);
			}
		}

		if (selected_item_number < 0) {
			sprintf((char *) draw_str,"Editing Town/Dungeon %d", (int)cur_town); 
			char_win_draw_string(main_dc,left_text_lines[0],(char *) draw_str,2,10);		
			sprintf((char *) draw_str,"  %s",town.town_name); 
			char_win_draw_string(main_dc,left_text_lines[1],(char *) draw_str,2,10);		
			sprintf((char *) draw_str,"Unrestricted Movement:    %d ",hintbook_mode0);
			char_win_draw_string(main_dc,left_text_lines[2],(char *) draw_str,2,10);
			sprintf((char *) draw_str,"Numerical Display Mode:  %d = %s",numerical_display_mode,numerical_display_captions[numerical_display_mode]);
			char_win_draw_string(main_dc,left_text_lines[3],(char *) draw_str,2,10);
			sprintf((char *) draw_str,"Object Display Mode:  %d = %s",object_display_mode,object_display_captions[object_display_mode]);
			char_win_draw_string(main_dc,left_text_lines[4],(char *) draw_str,2,10);
			}
		}
		else {
       short out_num = cur_out.y * scenario.out_width + cur_out.x;
			sprintf((char *) draw_str,"Editing Outdoors");
			char_win_draw_string(main_dc,left_text_lines[0],(char *) draw_str,2,10);
			sprintf((char *) draw_str,"  %s",current_terrain.name);
			char_win_draw_string(main_dc,left_text_lines[1],(char *) draw_str,2,10);
			sprintf((char *) draw_str,"  Section %d,  X = %d, Y = %d",out_num, (int)cur_out.x, (int)cur_out.y);
			char_win_draw_string(main_dc,left_text_lines[2],(char *) draw_str,2,10);
			sprintf((char *) draw_str,"Numerical Display Mode:  %d = %s",numerical_display_mode,numerical_display_captions[numerical_display_mode]);
			char_win_draw_string(main_dc,left_text_lines[3],(char *) draw_str,2,10);
			sprintf((char *) draw_str,"Object Display Mode:  %d = %s",object_display_mode,object_out_display_captions[object_display_mode]);
			char_win_draw_string(main_dc,left_text_lines[4],(char *) draw_str,2,10);
		}
	SelectObject(main_dc,store_font);
}

// Draws the whole or part of the button and location area to right
// mode: 0 - draw whole thing
//		 1 - just location
void place_right_buttons( /* short mode */ )
{
	RECT to_rect;
	char draw_str[256];

	HBITMAP store_bmp;
	HFONT store_font;

	for (short i = 0; i < 6; i++){
 		paint_pattern(terrain_buttons_gworld,0,right_text_lines[i],2);
	}
	
	if (file_is_loaded == FALSE){
		to_rect = terrain_buttons_rect;
		OffsetRect(&to_rect,RIGHT_BUTTONS_X_SHIFT,0);
		rect_draw_some_item(terrain_buttons_gworld,terrain_buttons_rect,
			terrain_buttons_gworld,to_rect,0,1); 
		return;
	}

	SetBkMode(main_dc4,TRANSPARENT);
	store_font = (HFONT) SelectObject(main_dc4,bold_font);
	store_bmp = (HBITMAP) SelectObject(main_dc4,DibBitmapHandle(terrain_buttons_gworld));
	
	switch (current_drawing_mode) {
		case 0: sprintf((char *) draw_str,"Drawing mode: FLOORS"); break;
		case 1: sprintf((char *) draw_str,"Drawing mode: TERRAIN"); break;
		case 2: sprintf((char *) draw_str,"Drawing mode: HEIGHT"); break;
		}
	char_win_draw_string(main_dc4,right_text_lines[0],(char *) draw_str,2,12);
	char_win_draw_string(main_dc4,right_text_lines[1],(char *) current_string,2,12);
	char_win_draw_string(main_dc4,right_text_lines[2],(char *) current_string2,2,12);

	sprintf((char *) draw_str,"Center: x = %d, y = %d ", (int)cen_x, (int)cen_y);
	char_win_draw_string(main_dc4,right_text_lines[3],(char *) draw_str,2,12);

	if (current_drawing_mode == 2) {
		if (current_height_mode == 0)
			char_win_draw_string(main_dc4,right_text_lines[4],"Automatic Hills: OFF",2,12);
			else char_win_draw_string(main_dc4,right_text_lines[4],"Automatic Hills: ON",2,12);
	}
	else {
 		if (editing_town) {
			sprintf((char *) draw_str,"Editing Town/Dungeon %d", (int)cur_town);
			char_win_draw_string(main_dc4,right_text_lines[4],(char *) draw_str,2,12);
		}
		else {
       short out_num = cur_out.y * scenario.out_width + cur_out.x;
			sprintf((char *) draw_str,"  Section %d,  X = %d, Y = %d",out_num, (int)cur_out.x, (int)cur_out.y);
			char_win_draw_string(main_dc4,right_text_lines[4],(char *) draw_str,2,12);
		}
		
 		if (editing_town) {
			sprintf((char *) draw_str,"  %s",town.town_name);
			char_win_draw_string(main_dc4,right_text_lines[5],(char *) draw_str,2,12);
			}
		else {
			sprintf((char *) draw_str,"  %s",current_terrain.name);
			char_win_draw_string(main_dc4,right_text_lines[5],(char *) draw_str,2,12);
        }
    }

	// palette buttons
	for (short i = 0; i < 8; i++){
		for (short j = 0; j < ((editing_town == TRUE) ? 6 : 3); j++) {
			//RECT from_rect = {PALETTE_BUT_HEIGHT * j,PALETTE_BUT_WIDTH * 1,
			//	PALETTE_BUT_HEIGHT * (j + 1),PALETTE_BUT_WIDTH * (i + 1)};
			RECT to_rect = palette_buttons[i][j];
			RECT from_rect = to_rect;
			ZeroRectCorner(&from_rect);

			if(cur_viewing_mode >= 10 && i == 0 && j == 1)		// realistic mode palette icon - not working
				OffsetRect(&from_rect,PALETTE_BUT_WIDTH * 7,PALETTE_BUT_HEIGHT * 6);
			else
				OffsetRect(&from_rect,PALETTE_BUT_WIDTH * i,PALETTE_BUT_HEIGHT * j);
// "realistic mode palette icon - not working", no wonder it is not working, this code does nothing
// the palette buttons are handled up the page in the function void set_up_terrain_buttons()
			to_rect.right++;
			from_rect.right++;
			
			rect_draw_some_item(editor_mixed,
				from_rect,terrain_buttons_gworld,to_rect,0,0);
		}
    }
		
	// place buttons on screen
	to_rect = terrain_buttons_rect;
	OffsetRect(&to_rect,RIGHT_BUTTONS_X_SHIFT,0);
	SelectObject(main_dc4,store_bmp);
	rect_draw_some_item(terrain_buttons_gworld,terrain_buttons_rect,
		terrain_buttons_gworld,to_rect,0,1);
	SelectObject(main_dc4,DibBitmapHandle(terrain_buttons_gworld));

	// draw frames around selected ter
	short selected_ter = -1;
	switch (current_drawing_mode) {
		case 0: selected_ter = current_floor_drawn; to_rect = terrain_rects[selected_ter]; break;
		case 1: 
		case 2: selected_ter = current_terrain_drawn; 
			short draw_position = selected_ter - 12 * GetControlValue(right_sbar);

			if (draw_position != minmax(0,(cur_viewing_mode >= 10) ? 227 : 263,draw_position))
				selected_ter = -1;
			else
                to_rect = ((cur_viewing_mode >= 10) ? terrain_rects_3D[draw_position] : terrain_rects[draw_position]);
			break;
	}	
	if (selected_ter >= 0) {
		OffsetRect(&to_rect,RIGHT_BUTTONS_X_SHIFT,0);
		MacInsetRect(&to_rect,-1,-1);
		put_rect_on_screen(main_dc,to_rect,0,0,0);
	}
	SelectObject(main_dc4,store_font);
	SelectObject(main_dc4,store_bmp);
}

void set_string(char *string,char *string2)
{
	strcpy((char *)current_string,string);
	strcpy((char *)current_string2,string2);
	place_right_buttons();
}

/*
Boolean container_there(location l)
{
	terrain_type_type terrain;
	
	if (editing_town == FALSE)
		return FALSE;
	terrain = scen_data.scen_ter_types[t_d.terrain[l.x][l.y]];
	if (terrain.special == 40)
		return TRUE;
	if (is_barrel(l.x,l.y) == TRUE)
		return TRUE;
	if (is_crate(l.x,l.y) == TRUE)
		return TRUE;

	return 0;		
}
*/

void win_draw_string_outline(HDC dest_hdc,RECT dest_rect,char *str,short mode,short line_height)
{
	SetTextColor(dest_hdc,RGB(255,255,255));
	OffsetRect(&dest_rect,1,0);
	char_win_draw_string(dest_hdc,dest_rect,str,mode,line_height);
	OffsetRect(&dest_rect,-1,1);
	char_win_draw_string(dest_hdc,dest_rect,str,mode,line_height);
	OffsetRect(&dest_rect,-1,-1);
	char_win_draw_string(dest_hdc,dest_rect,str,mode,line_height);
	OffsetRect(&dest_rect,1,-1);
	char_win_draw_string(dest_hdc,dest_rect,str,mode,line_height);
	OffsetRect(&dest_rect,0,1);
	
	SetTextColor(dest_hdc,RGB(0,0,0));
	char_win_draw_string(dest_hdc,dest_rect,str,mode,line_height);
}

void char_win_draw_string(HDC dest_window,RECT dest_rect,char *str,short mode, short /* line_height*/)
{
	char store_s[256];
	strcpy((char *) store_s,str);
	win_draw_string( dest_window, dest_rect,store_s, mode /*, line_height */);
}

// mode: 0 - align up and left, 1 - center on one line
// str is a c string, 256 characters
// uses current font
void win_draw_string(HDC dest_hdc,RECT dest_rect,char *str,short mode /*,short line_height */)
{
	short i;

// this will need formatting for '|' line breaks
	for (i = 0; i < 256; i++)  {
		if (str[i] == '|')
			str[i] = 13;
		if (str[i] == '_')
      	str[i] = 34;
	}
	// if dest is main window, add ulx, uly
	if (dest_hdc == main_dc)
		OffsetRect(&dest_rect,ulx,uly);
	switch (mode) {
		case 0:
         dest_rect.bottom += 6;
			DrawText(dest_hdc,str,(int)strlen((char *)str),&dest_rect,DT_LEFT | DT_WORDBREAK); break;
		case 1:
			dest_rect.bottom += 6; dest_rect.top -= 6;
			DrawText(dest_hdc,str,(int)strlen((char *)str),&dest_rect,
			DT_CENTER | DT_VCENTER | DT_NOCLIP | DT_SINGLELINE); break;
		case 2: case 3:
			dest_rect.bottom += 6; dest_rect.top -= 6;
			DrawText(dest_hdc,str,(int)strlen((char *)str),&dest_rect,
			DT_LEFT | DT_VCENTER | DT_NOCLIP | DT_SINGLELINE); break;
	}
	// not yet done adjusts for 1, 2, 3
}

//returns a string from one of a number of sets, listed below. 
//this is used to build up lists of things that have customized 
//names in the scenario, like cretaures or towns
// i,j is a STR# resource.
// If i < 0, get instead other strings
//   -1 - creature types
//   -2 - item types
//   -3 - buttons
//   -4 - terrain
//   -6 - floor
//   -7 - town name
//   -8 - outdoor section name
void get_str(char *str,short i, short j)
{
	int y;
	switch(i){
		case -1:
			sprintf((char *) str,"%s (L%d)",scen_data.scen_creatures[j].name,(int)scen_data.scen_creatures[j].level);
			break;
		case -2:
			strcpy((char *) str,scen_data.scen_items[j].full_name);
			break;
		case -3:
			strcpy((char *) str,button_strs[available_dlog_buttons[j]]);
			break;
		case -4:
			strcpy((char *) str,scen_data.scen_ter_types[j].ter_name);
			break;
		case -6:
			strcpy((char *) str,scen_data.scen_floors[j].floor_name);
			break;
		case -7:
			sprintf((char *) str,&zone_names.town_names[j][0]);
			break;
		case -8:
			y = j/zone_names.out_width;
			sprintf((char *) str,"(%i,%i) %s",j-(y*zone_names.out_width),y,(char*)&zone_names.section_names[j][0]);
			break;
		case -9:
			strcpy((char *) str,town.ter_scripts[j].script_name);
			break;
		case -10:
			strcpy((char *) str,town.creatures[j].char_script);
			break;

		default:
			GetIndString(str, i, j);
	}
}

/*
short string_length(char *str,HDC hdc)
{
	short text_len[257];
	short total_width = 0,i,len;
	char p_str[256];
	
	for (i = 0; i < 257; i++)
		text_len[i]= 0;
	
	strcpy((char *) p_str,str);
	MeasureText(256,p_str,text_len,hdc);
	len = strlen((char *)str);

	//print_nums(text_len[1],text_len[2],text_len[3]);
   //print_nums(text_len[10],text_len[20],text_len[30]);
	for (i = 0; i < 257; i++)
		if ((text_len[i] > total_width) && (i <= len))
			total_width = text_len[i];
	return total_width;
}
*/
/*
Boolean spot_in_rect(location l,RECT r)
{
	if ((l.x < r.left) || (l.x > r.right) || (l.y < r.top) || (l.y > r.bottom))
		return FALSE;
	return TRUE;
}
*/

Boolean refresh_graphics_library()
{
	for (short i = 0; i < num_sheets_in_library; i++) {
		delete_graphic(&graphics_library[i]);
		}
	num_sheets_in_library = 0;

	return TRUE;
}

// Adds a new sheet into library. Returns FALSE if at end of procedure sheet
// is not safely there.
// Not e that the which_icon field of new_sheet is ignored.
Boolean load_sheet_into_library(graphic_id_type *new_sheet)
{
	// error checking
	if (new_sheet->not_legit())
		return FALSE;
	
	// first, make sure new sheet isn't already in there
	if (get_index_of_sheet(new_sheet) >= 0)
		return TRUE;
	
	if (num_sheets_in_library >= MAX_NUM_SHEETS_IN_LIBRARY)
		return FALSE;
		
	graphics_library[num_sheets_in_library] = NULL;
	graphics_library[num_sheets_in_library] = load_pict(new_sheet->which_sheet);	
	if (graphics_library[num_sheets_in_library] == NULL)
		return FALSE;
		
	graphics_library_index[num_sheets_in_library] = *new_sheet;

	// modify graphics ccording to flag
	
	num_sheets_in_library++;
	return TRUE;
}

// Returns the number of the sheet witht he right characteristics.
// Returns -1 if not there.
short get_index_of_sheet(graphic_id_type *sheet)
{
	for (short i = 0; i < num_sheets_in_library; i++)
		if ((graphics_library[i] != NULL) && ((graphics_library_index[i].which_sheet) == (sheet->which_sheet)) &&
		((graphics_library_index[i].graphic_adjust) == (sheet->graphic_adjust)))
			return i;
	return -1;
}

// like get_index_of_sheet, but, if it's not there, loads it and then gets index. returns -1 if load fails.
// Returning -1 from here VERY bad karma.
short safe_get_index_of_sheet(graphic_id_type *sheet)
{
	if (sheet->not_legit())
		return -1;
		
	short index = get_index_of_sheet(sheet); 
	if (index < 0) {
		if (load_sheet_into_library(sheet) == FALSE) {
			refresh_graphics_library();
			if (load_sheet_into_library(sheet) == FALSE)
				return -1;
			}
		index = get_index_of_sheet(sheet);
		if (index < 0)
			return -1;
	}
	return index;
}

void put_rect_in_gworld(HDC hdc,RECT rect,short r,short g, short b)
{
	HPEN hpen,old_pen;

	if (hdc == NULL) {
		beep();
		return;
		}
	rect.right--;
	rect.bottom--;
	hpen = CreatePen(PS_SOLID,1,RGB(r,g,b));
	old_pen = (HPEN) SelectObject(hdc,hpen);
	MoveToEx(hdc,rect.left,rect.top,NULL);
	LineTo(hdc,rect.right,rect.top);
	LineTo(hdc,rect.right,rect.bottom);
	LineTo(hdc,rect.left,rect.bottom);
	LineTo(hdc,rect.left,rect.top);
	SelectObject(hdc,old_pen);
	DeleteObject(hpen);
}

void fill_rect_in_gworld(HDC line_gworld,RECT to_rect,short r,short g, short b)
{
	PaintRect(&to_rect,line_gworld,r,g,b);
}

void put_line_in_gworld(HDC line_gworld,short from_x,short from_y,short to_x,short to_y,short r,short g, short b)
{
	HPEN hpen,old_pen;

	if (line_gworld == NULL) {
		beep();
		return;
		}
	hpen = CreatePen(PS_SOLID,1,RGB(r,g,b));
	old_pen = (HPEN) SelectObject(line_gworld,hpen);
	MoveToEx(line_gworld,from_x,from_y,NULL);
	LineTo(line_gworld,to_x,to_y);
	SelectObject(line_gworld,old_pen);
	DeleteObject(hpen);
}

void put_rect_on_screen(HDC win,RECT to_rect,short r,short g, short b)
{
	put_rect_in_gworld(win,to_rect,r,g,b);
}

/*
void put_line_on_screen(HDC win,short from_x,short from_y,short to_x,short to_y,short r,short g, short b)
{
	put_line_in_gworld(win,from_x,from_y,to_x,to_y,r,g,b);
}
*/
/*
void put_clipped_rect_on_screen(HDC win,RECT to_rect,RECT clip_rect,short r,short g, short b)
{
	if (rects_touch(&to_rect,&clip_rect) == FALSE)
		return;

	if ((to_rect.top >= clip_rect.top) && (to_rect.top < clip_rect.bottom))
		put_line_on_screen(win,smax(to_rect.left,clip_rect.left),to_rect.top,
		  smin(to_rect.right,clip_rect.right),to_rect.top,r,g,b);
	if ((to_rect.bottom >= clip_rect.top) && (to_rect.bottom < clip_rect.bottom))
		put_line_on_screen(win,smax(to_rect.left,clip_rect.left),to_rect.bottom,
		  smin(to_rect.right,clip_rect.right),to_rect.bottom,r,g,b);
	
	if ((to_rect.left >= clip_rect.left) && (to_rect.left < clip_rect.right))
		put_line_on_screen(win,to_rect.left,smax(to_rect.top,clip_rect.top),
		  to_rect.left,smin(to_rect.bottom,clip_rect.bottom),r,g,b);
	if ((to_rect.right >= clip_rect.left) && (to_rect.right < clip_rect.right))
		put_line_on_screen(win,to_rect.right,smax(to_rect.top,clip_rect.top),
		  to_rect.right,smin(to_rect.bottom,clip_rect.bottom),r,g,b);

}
*/
void put_clipped_rect_in_gworld(HDC line_gworld,RECT to_rect,RECT clip_rect,short r,short g, short b)
{
	if (rects_touch(&to_rect,&clip_rect) == FALSE)
		return;
	if ((to_rect.top >= clip_rect.top) && (to_rect.top < clip_rect.bottom))
		put_line_in_gworld(line_gworld,smax((short)to_rect.left,(short)clip_rect.left),(short)to_rect.top,
		  smin((short)to_rect.right,(short)clip_rect.right),(short)to_rect.top,r,g,b);
	if ((to_rect.bottom >= clip_rect.top) && (to_rect.bottom < clip_rect.bottom))
		put_line_in_gworld(line_gworld,smax((short)to_rect.left,(short)clip_rect.left),(short)to_rect.bottom,
		  smin((short)to_rect.right,(short)clip_rect.right),(short)to_rect.bottom,r,g,b);

	if ((to_rect.left >= clip_rect.left) && (to_rect.left < clip_rect.right))
		put_line_in_gworld(line_gworld,(short)to_rect.left,smax((short)to_rect.top,(short)clip_rect.top),
		  (short)to_rect.left,smin((short)to_rect.bottom,(short)clip_rect.bottom),r,g,b);
	if ((to_rect.right >= clip_rect.left) && (to_rect.right < clip_rect.right))
		put_line_in_gworld(line_gworld,(short)to_rect.right,smax((short)to_rect.top,(short)clip_rect.top),
		  (short)to_rect.right,smin((short)to_rect.bottom,(short)clip_rect.bottom),r,g,b);
}


Boolean is_field_type(short i,short j,short field_type)
{
	short k;
	
	for (k = 0; k < NUM_TOWN_PLACED_FIELDS; k++)
		if ((town.preset_fields[k].field_type == field_type) &&
			(town.preset_fields[k].field_loc.x == i) &&
			(town.preset_fields[k].field_loc.y == j))
				return TRUE;
	return FALSE;
}

void make_field_type(short i,short j,short field_type)
{
	short k;
	
	if (is_field_type(i,j,field_type) == TRUE)
		return;
	for (k = 0; k < NUM_TOWN_PLACED_FIELDS; k++)
		if (town.preset_fields[k].field_type == -1) {
			town.preset_fields[k].field_loc.x = (t_coord)i;
			town.preset_fields[k].field_loc.y = (t_coord)j;
			town.preset_fields[k].field_type = field_type;
			return;
		}
	//sprintf(str,"Field error  %d %d %d",i,j,field_type);
	//give_error(str,"",0);
	//give_error("Each town can have at most 60 fields and special effects (webs, barrels, blood stains, etc.). To place more, use the eraser first.","",0);
}

void take_field_type(short i,short j,short field_type)
{
	short k;
	
	for (k = 0; k < NUM_TOWN_PLACED_FIELDS; k++)
		if ((town.preset_fields[k].field_type == field_type) &&
			(town.preset_fields[k].field_loc.x == i) &&
			(town.preset_fields[k].field_loc.y == j)) {
				town.preset_fields[k].field_type = -1;
				return;
		}
}

Boolean is_oblique_mirror(short i,short j)
{
	return is_field_type(i,j,2);
}
void make_oblique_mirror(short i,short j)
{
	make_field_type(i,j,2);
}
void take_oblique_mirror(short i,short j)
{
	take_field_type(i,j,2);
}
Boolean is_facing_mirror(short i,short j)
{
	return is_field_type(i,j,8);
}
void make_facing_mirror(short i,short j)
{
	make_field_type(i,j,8);
}
void take_facing_mirror(short i,short j)
{
	take_field_type(i,j,8);
}

Boolean is_web(short i,short j)
{
	return is_field_type(i,j,5);
}
void make_web(short i,short j)
{
	make_field_type(i,j,5);
}
void take_web(short i,short j)
{
	take_field_type(i,j,5);
}

Boolean is_crate(short i,short j)
{
	return is_field_type(i,j,6);
}
void make_crate(short i,short j)
{
	make_field_type(i,j,6);
}
void take_crate(short i,short j)
{
	take_field_type(i,j,6);
}

Boolean is_barrel(short i,short j)
{
	return is_field_type(i,j,7);
}
void make_barrel(short i,short j)
{
	make_field_type(i,j,7);
}
void take_barrel(short i,short j)
{
	take_field_type(i,j,7);
}

Boolean is_fire_barrier(short i,short j)
{
	return is_field_type(i,j,4);
}
void make_fire_barrier(short i,short j)
{
	make_field_type(i,j,4);
}
void take_fire_barrier(short i,short j)
{
	take_field_type(i,j,4);
}

Boolean is_force_barrier(short i,short j)
{
	return is_field_type(i,j,3);
}
void make_force_barrier(short i,short j)
{
	make_field_type(i,j,3);
}
void take_force_barrier(short i,short j)
{
	take_field_type(i,j,3);
}

Boolean is_blocked(short i,short j)
{
	return is_field_type(i,j,1);
}
void make_blocked(short i,short j)
{
	make_field_type(i,j,1);
}
void take_blocked(short i,short j)
{
	take_field_type(i,j,1);
}
Boolean is_sfx(short i,short j,short type)
{
	return is_field_type(i,j,type + 14);
}
void make_sfx(short i,short j,short type)
{
	make_field_type(i,j,type + 14);
}
void take_sfx(short i,short j,short type)
{
	take_field_type(i,j,type + 14);
}

void make_Horse(short i,short j)
{
	short k;
	for (k = 0; k < 30; k++) {
	if ((scenario.scen_horses[k].horse_loc.x == i) && (scenario.scen_horses[k].horse_loc.y == j) && (scenario.scen_horses[k].which_town == cur_town)) {
		set_string("A Horse already exists here","Choose another square.");
		return;
	}
	if (scenario.scen_horses[k].which_town == -1) {
			scenario.scen_horses[k].horse_loc.x = (t_coord)i;
			scenario.scen_horses[k].horse_loc.y = (t_coord)j;
			scenario.scen_horses[k].which_town = cur_town;
			scenario.scen_horses[k].exists = TRUE;
			set_string("Default: party can use this horse","Hit '=' to resume drawing.");
			return;
			}
	}
}

void make_Boat(short i,short j)
{
	short k;
	for (k = 0; k < 30; k++) {
	if ((scenario.scen_boats[k].boat_loc.x == i) && (scenario.scen_boats[k].boat_loc.y == j) && (scenario.scen_boats[k].which_town == cur_town)) {
		set_string("A Boat already exists here","Choose another square.");
		return;
	}
	if (scenario.scen_boats[k].which_town == -1) {
			scenario.scen_boats[k].boat_loc.x = (t_coord)i;
			scenario.scen_boats[k].boat_loc.y = (t_coord)j;
			scenario.scen_boats[k].which_town = cur_town;
			scenario.scen_boats[k].exists = TRUE;
			set_string("Default: party can use this boat","Hit '=' to resume drawing.");
			return;
			}
	}
}

void take_Horse(short i,short j)
{
	short k;
	for (k = 0; k < 30; k++) {
	if ((scenario.scen_horses[k].horse_loc.x == i) && (scenario.scen_horses[k].horse_loc.y == j) && (scenario.scen_horses[k].which_town == cur_town)) {
		scenario.scen_horses[k].horse_loc.x = 0;
		scenario.scen_horses[k].horse_loc.y = 0;
		scenario.scen_horses[k].which_town = -1;
		scenario.scen_horses[k].exists = FALSE;
		scenario.scen_horses[k].property = FALSE;
			set_string("Horse deleted.","");
	}
	}
}

void take_Boat(short i,short j)
{
	short k;
	for (k = 0; k < 30; k++) {
	if ((scenario.scen_boats[k].boat_loc.x == i) && (scenario.scen_boats[k].boat_loc.y == j) && (scenario.scen_boats[k].which_town == cur_town)) {
		scenario.scen_boats[k].boat_loc.x = 0;
		scenario.scen_boats[k].boat_loc.y = 0;
		scenario.scen_boats[k].which_town = -1;
		scenario.scen_boats[k].exists = FALSE;
		scenario.scen_boats[k].property = FALSE;
			set_string("Boat deleted.","");
	}
	}
}

void place_dlog_borders_around_rect(HDIB to_gworld,HWND win,RECT border_to_rect)
{
	RECT to_rect;
	
	place_dlog_border_on_win(to_gworld,win,border_to_rect,0,1);
	place_dlog_border_on_win(to_gworld,win,border_to_rect,1,1);
	to_rect = border_to_rect;
	to_rect.top = to_rect.bottom - 14;
	place_dlog_border_on_win(to_gworld,win,to_rect,0,0);
	to_rect = border_to_rect;
	to_rect.left = to_rect.right - 14;
	place_dlog_border_on_win(to_gworld,win,to_rect,1,0);
}
	
// if to_gworld is NULL, put on window
void place_dlog_border_on_win(HDIB to_gworld,HWND win,RECT border_to_rect,short horiz_or_vert,short bottom_or_top)
{
	RECT from_rect,to_rect,horiz_from = {0,0,607,14};
	short num_sections,section_width,i;
	HDIB from_gworld;

	if (horiz_or_vert == 0) {
		border_to_rect.bottom = border_to_rect.top + 14;
		from_rect = horiz_from;
		to_rect = border_to_rect;
		to_rect.right -= 14;
		from_rect.right = rect_width(&to_rect);
		from_gworld = (bottom_or_top == 0) ? dlog_horiz_border_bottom_gworld : dlog_horiz_border_top_gworld;
		if (to_gworld != NULL) {
			if ((win == mainPtr) || (win == NULL))
				rect_draw_some_item(from_gworld,from_rect,to_gworld,to_rect,0,0);
			else
                rect_draw_some_item(from_gworld,from_rect,to_gworld,to_rect,0,2);
		}
		else
            rect_draw_some_item(from_gworld,from_rect,from_gworld,to_rect,0,(win == mainPtr) ? 1 : 2);
		to_rect = border_to_rect;
		to_rect.left = to_rect.right - 14;
		from_rect = horiz_from;
		from_rect.left = from_rect.right - 14;
		if (to_gworld != NULL) {
			if ((win == mainPtr) || (win == NULL))
				rect_draw_some_item(from_gworld,from_rect,to_gworld,to_rect,0,0);
			else
                rect_draw_some_item(from_gworld,from_rect,to_gworld,to_rect,0,2);
		}
		else
            rect_draw_some_item(from_gworld,from_rect,from_gworld,to_rect,0,(win == mainPtr) ? 1 : 2);
	}
	if (horiz_or_vert == 1) {
		section_width = 370;
		border_to_rect.right = border_to_rect.left + 14;
		MacInsetRect(&border_to_rect,0,14);
		num_sections = (rect_height(&border_to_rect) / section_width) + 1;
		for (i = 0; i < num_sections; i++) {
			to_rect = border_to_rect;
			to_rect.top += section_width * i;
			to_rect.bottom = to_rect.top + section_width;
			if (to_rect.bottom > border_to_rect.bottom)
				to_rect.bottom = border_to_rect.bottom;
			from_rect = to_rect;
			OffsetRect(&from_rect,-1 * from_rect.left, -1 * to_rect.top);
			if (to_gworld != NULL) {				
				if ((win == mainPtr) || (win == NULL))
					rect_draw_some_item(dlog_vert_border_gworld,from_rect,to_gworld,to_rect,0,0);
				else
                    rect_draw_some_item(dlog_vert_border_gworld,from_rect,to_gworld,to_rect,0,2);
			}
			else
                rect_draw_some_item(dlog_vert_border_gworld,from_rect,dlog_vert_border_gworld,to_rect,0,(win == mainPtr) ? 1 : 2);
		}
	}
}

void cant_draw_graphics_error(graphic_id_type a,char *bonus_string,short bonus_num)
{
	char error[256];
	char error2[256];
	
	if (showed_graphics_error)
		return;
	showed_graphics_error = TRUE;
	sprintf(error,"The editor just failed to load this graphic: sheet %d, icon %d. The editor might have run out of memory but, more likely, the graphic it was trying to find wasn't there.",
		(int)a.which_sheet, (int)a.which_icon);
	if (a.which_sheet < 0)
		sprintf(error,"The editor just failed to load this graphic: sheet %d, icon %d. This means that one of your items or creature types has been left set to its default graphic type.",
			(int)a.which_sheet, (int)a.which_icon);
	
	if (bonus_num >= 0)
		sprintf(error2,"%s %d. You won't see this error again until you load a new outdoor section or town.",bonus_string, (int)bonus_num);
		else sprintf(error2,"You won't see this error again until you load a new outdoor section or town.");
	give_error(error,error2,0);
}

// DIB does not always have a monolithic (continuous) bitmap area.
// Sometimes each row is assigned to scattered area on memory.
// thus, every time to access to the another row, we should get a pointer via DibPixelPtr()

typedef unsigned long UInt32;
typedef unsigned short UInt16;

#define GETR(c) (((c) >> 10) & 31)
#define GETG(c) (((c) >> 5) & 31)
#define GETB(c) (((c)) & 31)
#define RGB16BIT(r,g,b) (((r) << 10) + ((g) << 5) + ((b)))

//takes graphic from arguments and if any tinting happens, changes them
//+1 - Swap red  and blue. 
//+2 - Swap blue and green. 
//+4 - Swap green  and red. 
//+8 - Tint graphic to a neutral shade. 
//+16 - Darken the graphic. 
//+32 - Lighten the graphic. 
//+64 - Invert all of the pixels. 
//+128 - Tint the  graphic red 
//+256 - Tint the  graphic  green 
//+512 - Tint the  graphic  blue 
void adjust_graphic(HDIB *src_gworld_ptr, RECT *from_rect_ptr, short graphic_adjust)
// short light_level, Boolean has_border, short border_r, short border_g, short border_b
{
	//don't waste time on the common untinted graphic
	if(!graphic_adjust)
		return;
	
	if (tint_area == NULL)
		return;
	
	RECT from_rect = *from_rect_ptr;
	ZeroRectCorner(from_rect_ptr);
	rect_draw_some_item(*src_gworld_ptr,from_rect,tint_area,*from_rect_ptr,0,0);
	*src_gworld_ptr = tint_area;

	RECT rect_to_fill = *from_rect_ptr;
	UInt16	*destBits;
	short height,width;
//	short dest_shift_width;
	short v,h;
	UInt16 r1,g1,b1,store;
	UInt16 white = 31 + (31 << 5) + (31 << 10);

//	destBits = (UInt16 *)DibPixelPtr(tint_area, rect_to_fill.left, rect_to_fill.top);

	height = rect_height(&rect_to_fill);
	width = rect_width(&rect_to_fill);

//	dest_shift_width = (short)DibWidth(tint_area) - width;
	for (v = 0; v < height; v++){
		destBits = (UInt16 *)DibPixelPtr(tint_area, rect_to_fill.left, rect_to_fill.top + v);
		for (h = 0; h < width; h++){
			if (*destBits != white) {
				r1 = GETR(*destBits);
				g1 = GETG(*destBits);
				b1 = GETB(*destBits);
				
//+1 - Swap red  and blue. 
				if(graphic_adjust & 1) {
					store = r1;
					r1 = b1;
					b1 = store;
				}
//+2 - Swap blue and green. 
				if(graphic_adjust & 2) {
					store = b1;
					b1 = g1;
					g1 = store;
				}
//+4 - Swap green  and red. 
				if(graphic_adjust & 4) {
					store = g1;
					g1 = r1;
					r1 = store;
				}
//+8 - Tint graphic to a neutral shade. 
				if(graphic_adjust & 8) {
					r1 = (3 * r1 + 14) >> 2;
					g1 = (3 * g1 + 14) >> 2;
					b1 = (3 * b1 + 14) >> 2;
				}
//+16 - Darken the graphic. 
				if(graphic_adjust & 16) {
					r1 = (5 * r1) >> 3;
					g1 = (5 * g1) >> 3;
					b1 = (5 * b1) >> 3;
				}
//+32 - Lighten the graphic. 
				if(graphic_adjust & 32) {
					r1 = (3 * r1 + 25) >> 2;
					g1 = (3 * g1 + 25) >> 2;
					b1 = (3 * b1 + 25) >> 2;
				}
//+64 - Invert all of the pixels. 
				if(graphic_adjust & 64) {
					r1 = 31 - r1;
					g1 = 31 - g1;
					b1 = 31 - b1;
				}
//+128 - Tint the  graphic red 
				if(graphic_adjust & 128) {
					r1 = r1 + 7;
					r1 = minmax(1,31,r1);
					g1 = minmax(1,31,g1);
					b1 = minmax(1,31,b1);
				}
//+256 - Tint the  graphic  green 
				if(graphic_adjust & 256) {
					g1 = g1 + 7;
					r1 = minmax(1,31,r1);
					g1 = minmax(1,31,g1);
					b1 = minmax(1,31,b1);
				}
//+512 - Tint the  graphic  blue 
				if(graphic_adjust & 512) {
					b1 = b1 + 7;
					r1 = minmax(1,31,r1);
					g1 = minmax(1,31,g1);
					b1 = minmax(1,31,b1);
				}
				
				*destBits = RGB16BIT(r1,g1,b1);
			}
			destBits++;
		}
//		destBits += dest_shift_width;//(width * 2);
	}
}

//takes graphic from arguments and if any tinting happens, changes them
//lighting:  0 to 8.  8 is fully lit, lesser values are -2 to r,g,b values each
void apply_lighting_to_graphic(HDIB *src_gworld_ptr, RECT *from_rect_ptr, short lighting)
{
	if(lighting == 8)
		return;
	
	if (tint_area == NULL)
		return;
	
	//only mess with switching gworld if it hasn't already been done
	if(tint_area != *src_gworld_ptr) {
		RECT from_rect = *from_rect_ptr;
		ZeroRectCorner(from_rect_ptr);
		rect_draw_some_item(*src_gworld_ptr,from_rect,tint_area,*from_rect_ptr,0,0);
		*src_gworld_ptr = tint_area;
	}

	RECT rect_to_fill = *from_rect_ptr;
	UInt16	*destBits;
	short height,width;
//	short dest_shift_width;
	short v,h;
	UInt16 r1,g1,b1;
	UInt16 white = 31 + (31 << 5) + (31 << 10);

//	destBits = (UInt16 *)DibPixelPtr(tint_area, rect_to_fill.left, rect_to_fill.top);

	height = rect_height(&rect_to_fill);
	width = rect_width(&rect_to_fill);

//	dest_shift_width = (short)DibWidth(tint_area) - width;

	for (v = 0; v < height; v++){
		destBits = (UInt16 *)DibPixelPtr(tint_area, rect_to_fill.left, rect_to_fill.top + v);
		for (h = 0; h < width; h++){
			if (*destBits != white){
				r1 = GETR(*destBits);
				g1 = GETG(*destBits);
				b1 = GETB(*destBits);
				
				r1 = r1 + (lighting - 8) * 2;
				g1 = g1 + (lighting - 8) * 2;
				b1 = b1 + (lighting - 8) * 2;
				r1 = minmax(0,31,r1);
				g1 = minmax(0,31,g1);
				b1 = minmax(0,31,b1);
				
				*destBits = RGB16BIT(r1,g1,b1);
			}
			destBits++;
		}
//		destBits += dest_shift_width;//(width * 2);
	}
}

// give outline to the picture
void add_border_to_graphic(HDIB *src_gworld_ptr, RECT *from_rect_ptr, short border_r, short border_g, short border_b)
{
	if (tint_area == NULL)
		return;
	
	//only mess with switching gworld if it hasn't already been done
	if(tint_area != *src_gworld_ptr) {
		RECT from_rect = *from_rect_ptr;
		ZeroRectCorner(from_rect_ptr);
		rect_draw_some_item(*src_gworld_ptr,from_rect,tint_area,*from_rect_ptr,0,0);
		*src_gworld_ptr = tint_area;
	}

	RECT rect_to_fill = *from_rect_ptr;
	UInt16 *destBits;
	UInt16 *destBitsU;
	UInt16 *destBitsL;
//	UINT16 *destBitsStore;
	short height,width;
//	short dest_shift_width;
	short v,h;
//	short dest_width;
	UInt16 border_color;
	UInt16 white = 31 + (31 << 5) + (31 << 10);

//	destBits = destBitsStore = (UInt16 *)DibPixelPtr(tint_area, rect_to_fill.left, rect_to_fill.top);

	height = rect_height(&rect_to_fill);
	width = rect_width(&rect_to_fill);

//	dest_shift_width = (short)DibWidth(tint_area) - width;

	border_color = RGB16BIT(border_r,border_g,border_b);
//	dest_width = (short)DibWidth(tint_area);
	for (v = 0; v < height; v++){
		destBitsL = (UInt16 *)DibPixelPtr(tint_area, rect_to_fill.left, rect_to_fill.top + v - 1);
		destBits  = (UInt16 *)DibPixelPtr(tint_area, rect_to_fill.left, rect_to_fill.top + v);
		destBitsU = (UInt16 *)DibPixelPtr(tint_area, rect_to_fill.left, rect_to_fill.top + v + 1);
		for (h = 0; h < width; h++){
/*
			if((*destBits == white) && (
			(v + 1 < height && (*(destBits + dest_width) & 0x7FFF) != white) ||
			(h + 1 < width && (*(destBits + 1) & 0x7FFF) != white) ||
			(v > 0 && (*(destBits - dest_width) & 0x7FFF) != white) ||
			(h > 0 && (*(destBits - 1) & 0x7FFF) != white)
			))
*/
			if((*destBits == white) && (
			(v + 1 < height && (*destBitsU & 0x7FFF) != white) ||
			(h + 1 < width && (*(destBits + 1) & 0x7FFF) != white) ||
			(v > 0 && (*destBitsL & 0x7FFF) != white) ||
			(h > 0 && (*(destBits - 1) & 0x7FFF) != white)
			))
				*destBits |= 0x8000;
			else
				*destBits &= 0x7FFF;
			destBits++;
			destBitsL++;
			destBitsU++;
		}
//		destBits += dest_shift_width;//(width * 2);
	}
//	destBits = destBitsStore;
	for (v = 0; v < height; v++){
		destBits = (UInt16 *)DibPixelPtr(tint_area, rect_to_fill.left, rect_to_fill.top + v);
		for (h = 0; h < width; h++){
			if(*destBits & 0x8000) {
				*destBits = border_color;
			}
			destBits++;
		}
//		destBits += dest_shift_width;//(width * 2);
	}
}

// Setting color functions
// rgb are all in the 0 .. 31 range
#if 0
void set_blitter_color(UInt16 color)
{
	set_color = color;
}

// Given a graphic, hues the graphic in the shade of the blitter color.
// tint_strength: 0 - weak, 1 - strong
void tint_graphic( GWorldPtr dest,short tint_strength)
{
	if (dest == NULL)
		return;

	RECT rect_to_fill = get_graphic_rect(dest);
	UInt32	destRowBytes;
	UInt16	*destBits;
	short height,width;
	//PixMap *destmap = *(dest->portPixMap);
	short dest_shift_width,v,h;
	UInt16 r1,g1,b1,store,r2,g2,b2;
	UInt16 white = 31 + (31 << 5) + (31 << 10);

	PixMapHandle destmap;

	destmap = GetPortPixMap(dest);

	//	Find the base address and rowbytes of the buffers
	destRowBytes = GetPixRowBytes(destmap);
	destBits = (UInt16 *) GetPixBaseAddr(destmap); //destmap->baseAddr;

	//	Jump down to the target pixel in the destination
	destBits += rect_to_fill.top * (destRowBytes >> 1);
	destBits += rect_to_fill.left;

	height = rect_height(rect_to_fill);
	width = rect_width(rect_to_fill);

	dest_shift_width = (destRowBytes >> 1) - width;

	r2 = GETR(set_color);
	g2 = GETG(set_color);
	b2 = GETB(set_color);

	for (v = 0; v < height; v++){
		for (h = 0; h < width; h++){
			if (*destBits != white) {
				r1 = GETR(*destBits);
				g1 = GETG(*destBits);
				b1 = GETB(*destBits);

				if (tint_strength == 1) {
					r1 = (5 * r1 + 3 * r2) >> 3;
					g1 = (5 * g1 + 3 * g2) >> 3;
					b1 = (5 * b1 + 3 * b2) >> 3;
				}
				else {
					r1 = (3 * r1 + r2) >> 2;
					g1 = (3 * g1 + g2) >> 2;
					b1 = (3 * b1 + b2) >> 2;
				}
				//r1 = (r1 + r2) / 2;
				//g1 = (g1 + g2) / 2;
				//b1 = (b1 + b2) / 2;
				*destBits = RGB16BIT(r1,g1,b1);
			}
			destBits++;
		}
		destBits += dest_shift_width;//(width * 2);
	}
}

// Given a graphic, shifts its colors by a given amount up or down.
// all shifts are 0 to 31
void hue_graphic( GWorldPtr dest,short up_or_down,short red_shift,short green_shift,short blue_shift)
{
	if (dest == NULL)
		return;

	RECT rect_to_fill = get_graphic_rect(dest);
	UInt32	destRowBytes;
	UInt16	*destBits;
	short height,width;
	short dest_shift_width,v,h;
	UInt16 white = 31 + (31 << 5) + (31 << 10);
	UInt16 r1,g1,b1;

	PixMapHandle destmap;

	destmap = GetPortPixMap(dest);

	//	Find the base address and rowbytes of the buffers
	destRowBytes = GetPixRowBytes(destmap);
	destBits = (UInt16 *) GetPixBaseAddr(destmap); //destmap->baseAddr;

	//	Jump down to the target pixel in the destination
	destBits += rect_to_fill.top * (destRowBytes >> 1);
	destBits += rect_to_fill.left;

	height = rect_height(rect_to_fill);
	width = rect_width(rect_to_fill);

	dest_shift_width = (destRowBytes >> 1) - width;

	for (v = 0; v < height; v++){
		for (h = 0; h < width; h++){
			if (*destBits != white) {
				r1 = GETR(*destBits);
				g1 = GETG(*destBits);
				b1 = GETB(*destBits);

				if (up_or_down == 1) {
					r1 = r1 + red_shift;
					g1 = g1 + green_shift;
					b1 = b1 + blue_shift;
				}
				else {
					r1 = r1 - red_shift;
					g1 = g1 - green_shift;
					b1 = b1 - blue_shift;
				}
				r1 = minmax(1,31,r1);
				g1 = minmax(1,31,g1);
				b1 = minmax(1,31,b1);

				*destBits = RGB16BIT(r1,g1,b1);
			}
			destBits++;
		}
		destBits += dest_shift_width;//(width * 2);
	}
}

#endif

