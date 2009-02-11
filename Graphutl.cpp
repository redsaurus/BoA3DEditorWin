#include "stdafx.h"
/*
#include <windows.h>
#include <windowsx.h>
#include <commdlg.h>
//#include <commctrl.h>

#include <wingdi.h>

#include <stdio.h>
#include <stdlib.h>
#include "string.h"
*/

#include "global.h"
#include "DibEngine.h"

const char * k3DEdGrphDir = "3DEditorGraphics";			// 3D Editor Grahpics folder

#define xmin(a,b)	((a) < (b) ?  (a) : (b))

// Gloabl varialbes


// external gloabal variables

extern HWND mainPtr;
extern HDC main_dc;
extern HDC main_dc2;
extern HDC main_dc3;
extern short ulx,uly;
// extern long anim_ticks;
// extern char file_path_name[256];
// extern char store_path[_MAX_PATH + 1];
// extern HDIB terrain_gworld, masked_gworld;
extern HDIB pat_source_gworld;
extern HDIB mixed_gworld;
extern HDIB dialog_pattern_gworld;
extern HDIB pattern_gworld;
extern HDIB ed_pattern_gworld;
// extern Boolean running_on_nt;
// extern RECT ter_gworld_inside_rect;
extern HBITMAP bw_bitmap;
extern char store_editor_path[_MAX_PATH + 1];
extern char appl_path[_MAX_PATH + 1];
extern char scenario_path[_MAX_PATH];
 
// local variables

Boolean graphic_alarm_given = FALSE;
Boolean graphics_messed = FALSE;
short dlog_pat_placed = 0;
short current_pattern = -1;
short store_mask = -1;
Boolean using_a_mask = FALSE;
HDIB current_mask;
RECT current_mask_rect;
short store_store_mask;
RGBTRIPLE set_color;
short darkness_scale = 14;

unsigned short floor_animate[23][16] = {
{1,0,1,2,3,2,1,0,1,2,3,4,5,4,3,2},
{1,0,1,2,3,2,1,0,1,2,3,4,5,4,3,2},
{2,1,0,1,2,3,2,1,0,1,2,3,4,5,4,3},
{3,2,1,0,1,2,3,2,1,0,1,2,3,4,5,4},
{4,3,2,1,0,1,2,3,2,1,0,1,2,3,4,5},
{5,4,3,2,1,0,1,2,3,2,1,0,1,2,3,4},//
{4,5,4,3,2,1,0,1,2,3,2,1,0,1,2,3},
{3,4,5,4,3,2,1,0,1,2,3,2,1,0,1,2},
{2,3,4,5,4,3,2,1,0,1,2,3,2,1,0,1},
{2,3,4,5,4,3,2,1,0,1,2,3,2,1,0,1},
{1,2,3,4,5,4,3,2,1,0,1,2,3,2,1,0},//
{1,2,3,4,5,4,3,2,1,0,1,2,3,2,1,0},
{1,2,3,4,5,4,3,2,1,0,1,2,3,2,1,0},
{1,2,3,4,5,4,3,2,1,0,1,2,3,2,1,0},
{2,3,4,5,4,3,2,1,0,1,2,3,2,1,0,1},
{3,4,5,4,3,2,1,0,1,2,3,2,1,0,1,2},//
{4,5,4,3,2,1,0,1,2,3,2,1,0,1,2,3},
{5,4,3,2,1,0,1,2,3,2,1,0,1,2,3,4},
{4,3,2,1,0,1,2,3,2,1,0,1,2,3,4,5},
{3,2,1,0,1,2,3,2,1,0,1,2,3,4,5,4},
{2,1,0,1,2,3,2,1,0,1,2,3,4,5,4,3},//
{1,0,1,2,3,2,1,0,1,2,3,4,5,4,3,2},
{1,0,1,2,3,2,1,0,1,2,3,4,5,4,3,2}
};
short floor_offsets[32] = 
	{22,21,19,18,16, 15,13,12,10,9, 
	7,6,4,3,1, 0,0,1,3,4,
	6,7,9,10,12, 13,15,16,18,19,
	21,22};
short floor_width[32] = 
	{2,4,8,10,14, 16,20,22,26,28,
	32,34,38,40,44, 46,46,44,40,38,
	34,32,28,26,22, 20,16,14,10,8,
	4,2
	};

// BOOL pal_ok = FALSE;
// Boolean syscolors_stored = FALSE;
//	int elements[5] = {COLOR_ACTIVEBORDER,COLOR_ACTIVECAPTION,
//			COLOR_WINDOWFRAME,COLOR_SCROLLBAR,COLOR_BTNFACE};
//	COLORREF store_element_colors[5];
// HPALETTE syspal = NULL;
// Boolean common_controls_inited = FALSE;
// Boolean print_val = FALSE;
// short store_bitmap_height,store_bitmap_width;


// function prototype

// void test_new_bmap_engine();
void graphics_error(short which_pict);
HDIB load_pict_from( const char * parent, const char * folder, int which_pict );
// void set_graphic_mask(short which_mask);
// void masked_rect_draw_some_item (HDIB src_gworld, RECT src_rect,  HDIB targ_gworld, RECT targ_rect, short masked, short main_win);
// void set_blitter_color(unsigned int new_color);
// void transbitblit( HDIB src,RECT source_rect,HDIB dest,RECT dest_rect,short draw_mode,short adjusts) ;
// void floor_transbitblit( HDIB src,RECT source_rect,HDIB dest,RECT dest_rect,short draw_mode,short adjusts) ;
// void floor_draw_lower_frame( HDIB src,RECT source_rect,HDIB dest,RECT dest_rect,short draw_mode,short adjusts) ;
// Boolean Button();
// void bmp_rect_draw_some_item(HBITMAP src,RECT src_rect,HBITMAP dest,RECT dest_rect, short trans, short main_win) ;
// short get_graphic_mask();
// void dim_rect(HDIB dest,RECT dest_rect,BYTE r_dim,BYTE g_dim,BYTE b_dim) ;
// void hue_rect(HDIB dest,RECT dest_rect) ;



void graphics_error(short which_pict)
{
	char d_s[256];
	
	//sprintf((char *) d_s, "Stuck on %d.",(short) which_pict);
	//add_string_to_buf((char *)d_s);
	//print_buf(); 
	if (graphic_alarm_given == FALSE) {
		sprintf((char *) d_s, "Blades of Avernum was unable to find or load graphic %d. Make sure the editor is in the Data folder of Blades of Avernum (and not in a sub-folder). Also, make sure all graphics are where they should be.", (int)which_pict);
		MessageBox(NULL,
			d_s,
			"Graphic Missing",MB_OK | MB_ICONEXCLAMATION);

		graphic_alarm_given = TRUE;
		graphics_messed = TRUE;
		}
}

HDIB load_pict_from( const char * parent, const char * folder, int which_pict )
{
	const char * slash = "\\";

	char file_n[_MAX_PATH + 1];
	if ( parent ) {
		if ( folder )
			sprintf((char *) file_n,"%s%s%s%sG%d.bmp",parent,slash,folder,slash,which_pict);
		else
			sprintf((char *) file_n,"%s%sG%d.bmp",parent,slash,which_pict);
		return DibFileLoad(file_n);
	}
	return NULL;
}

// Loads in the pict with the correct number. First ports it to 16 bit mode, but currently 24 bit
HDIB load_pict(short which_pict)
{
	HDIB graphic = NULL,ported_graphic;
	char file_n[_MAX_PATH + 1];
	
	// needs to be able to look in a number of locations
	for (;;) {
		if ( (graphic = load_pict_from( store_editor_path, NULL,				which_pict )) != NULL )	break;
		if ( (graphic = load_pict_from( store_editor_path, "Character Graphics",which_pict )) != NULL )	break;
		if ( (graphic = load_pict_from( store_editor_path, "Editor Graphics",	which_pict )) != NULL )	break;
		if ( (graphic = load_pict_from( store_editor_path, "Game Graphics",		which_pict )) != NULL )	break;
		if ( (graphic = load_pict_from( store_editor_path, "Item Graphics",		which_pict )) != NULL )	break;
		if ( (graphic = load_pict_from( store_editor_path, "Scen Icon Graphics",which_pict )) != NULL )	break;
		if ( (graphic = load_pict_from( store_editor_path, "Terrain Graphics",	which_pict )) != NULL )	break;
		if ( (graphic = load_pict_from( appl_path,			k3DEdGrphDir,		which_pict )) != NULL )	break;

		sprintf((char *) file_n,"%sG%d.bmp",scenario_path,(int)which_pict);
		graphic = DibFileLoad(file_n);
		break;
	}

	if (graphic == NULL) { // error!
		if (graphics_messed == FALSE)
			refresh_graphics_library();
		graphic = DibFileLoad(file_n);

		if (graphic == NULL)  {
			graphics_error(which_pict);
			return NULL;
			}

		}
	
	// if it's 24 bit, we're done
	if (DibBitCount(graphic) == 24)
		return graphic;
	
	//Otherwise, make a 24 bit version of what we just loaded.
	ported_graphic = DibCreate(DibWidth(graphic),DibHeight(graphic),24,0);
	if (ported_graphic == NULL) { // error!
		if (graphics_messed == FALSE)
			refresh_graphics_library();
		ported_graphic = DibCreate(DibWidth(graphic),DibHeight(graphic),24,0);

		if (ported_graphic == NULL)  {
			graphics_error(which_pict);
			DibDelete(graphic);
			return NULL;
			}

		}	
	RECT port_rect = {0,0,DibWidth(graphic),DibHeight(graphic)};

	rect_draw_some_item(graphic,port_rect,ported_graphic,port_rect,0,0); 	
	
	DibDelete(graphic);
	
	return ported_graphic;
}


// These 2 procedures are to set the hues which
// influence the graphics drawn by rect_draw_some_item_hued
// r,g,b are 0 to 255
/*
void set_graphic_mask(short which_mask)
{
	store_mask = which_mask;
}
*/
/*
short get_graphic_mask()
{
	return store_mask;
}
*/
// Mask values
// 0 no mask
// 1 - 6 grays, from light to not so light
// 8 blues
// 9 red
// 10 green
/*
void masked_rect_draw_some_item (HDIB src_gworld, RECT src_rect,  HDIB targ_gworld, 
RECT targ_rect, short masked, short main_win)

{
rect_draw_some_item ( src_gworld,  src_rect,   targ_gworld, 
 targ_rect,  masked,  main_win);

}
*/

// trans:		0	overwrite draw
//				1	transparent
//				2	hued draw, brush: RGB(80,80,255), MERGECOPY (Merges the colors of the source rectangle with the brush currently selected in hdcDest, by using the Boolean AND operator.)
//
// main_win:	0	draw to bitmap of dest_hdib (using main_dc2 and main_dc3 as working device contexts)
//				1	draw to main window
//				2	draw to dialog window

void rect_draw_some_item(HDIB src_hdib,RECT src_rect,HDIB dest_hdib,RECT dest_rect,
	short trans, short main_win) {
	HDC hdcMem,hdcMem2 = NULL,destDC;
	HBITMAP transbmp = NULL,old_bmp;
	COLORREF white = RGB(255,255,255),oldcolor;
//	COLORREF black = RGB(0,0,0);
	HBRUSH hbrush = NULL, old_brush = NULL;
	COLORREF x = RGB(80,80,255);//RGB(150,150,255);
	HBITMAP store = NULL,store2,storea,storeb;
	Boolean dlog_draw = FALSE,mask_prepared = FALSE;
	HBITMAP src,dest;
	
	if ((src_hdib == NULL) || (dest_hdib == NULL))
		return;

	src = DibBitmapHandle(src_hdib);
	if (main_win == 0)
		dest = DibBitmapHandle(dest_hdib);	
	else
		dest = (HBITMAP) dest_hdib;

	if ((src == NULL) || (dest == NULL))
		return;	

	//if ((store_mask >= 8) && (main_win != 2)) {
	//	outline_masked_rect_draw_some_item(src_hdib,src_rect,dest_hdib,dest_rect,trans,main_win);
	//	return;
	//	}
	
	LONG src_width = src_rect.right - src_rect.left;
	LONG src_height = src_rect.bottom - src_rect.top;
	LONG dest_width = dest_rect.right - dest_rect.left;
	LONG dest_height = dest_rect.bottom - dest_rect.top;

	if (main_win == 2) {
		destDC = (HDC) dest;
		main_win = 1;
		dlog_draw = TRUE;
		hdcMem = CreateCompatibleDC(destDC);
		store = (HBITMAP)SelectObject(hdcMem, src);
		SetMapMode(hdcMem,GetMapMode(main_dc));
	} else {
		destDC = main_dc;
		hdcMem = main_dc2;
		store = (HBITMAP)SelectObject(hdcMem,src);
	}

	if (trans != 1) {
		if (main_win == 0) { // Not transparent, into bitmap
			hdcMem2 = main_dc3;
			store2 = (HBITMAP) SelectObject(hdcMem2, dest);

			if (trans == 2) {
				hbrush = CreateSolidBrush(x);
				old_brush = (HBRUSH) SelectObject(hdcMem2,hbrush);
			}
								
			if (store_store_mask > 0) {
				//hbrush = CreateSolidBrush(shade_color);
				//old_brush = SelectObject(hdcMem2, hbrush);
				switch (store_store_mask) {
					case 8: SetTextColor(hdcMem2,RGB(0,0,255)); break;
					case 9: SetTextColor(hdcMem2,RGB(255,0,0)); break;
					case 10: SetTextColor(hdcMem2,RGB(0,255,0)); break;
				}
			}				
			StretchBlt(hdcMem2,dest_rect.left,dest_rect.top,dest_width,dest_height,
				hdcMem,src_rect.left,src_rect.top,src_width,src_height, (trans == 0) ? SRCCOPY : MERGECOPY);
			//BitBlt(hdcMem2,dest_rect.left,dest_rect.top,dest_width,dest_height,
			//	hdcMem,src_rect.left,src_rect.top, (trans == 0) ? SRCCOPY : MERGECOPY);
			
			if (store_store_mask > 0) {
				SetTextColor(hdcMem2,RGB(0,0,0));
			}				

			if (trans == 2) {
				SelectObject(hdcMem2,old_brush);
				if (DeleteObject(hbrush) == 0)
					play_sound(1);
			}
			
			SelectObject(hdcMem2,store2);
		}
		else { // Not transparent, onto screen
			if (trans == 2) { /// hued draw
				hbrush = CreateSolidBrush(x);
				old_brush = (HBRUSH) SelectObject(destDC,hbrush);
			}

			StretchBlt(destDC,dest_rect.left,dest_rect.top,dest_width,dest_height,
					hdcMem,src_rect.left,src_rect.top,src_width,src_height, (trans == 0) ? SRCCOPY : MERGECOPY);
			//BitBlt(destDC,dest_rect.left,dest_rect.top,dest_width,dest_height,
			//		hdcMem,src_rect.left,src_rect.top,(trans == 0) ? SRCCOPY : MERGECOPY);

			if (trans == 2) {
				SelectObject(destDC,old_brush);
				if (DeleteObject(hbrush) == 0)
					play_sound(1);
			}		
		}
	} // end of non-transparent draws

	if (trans == 1) { // begin transparent draws

		if (main_win == 0) { // transparent, to bmp
			storea = (HBITMAP) SelectObject(main_dc3, dest);
			SelectObject(hdcMem,store);
			store = NULL;
			storeb = (HBITMAP) SelectObject(main_dc2, src);

			old_bmp = NULL;
			bool create_transbmp = false;
			if (using_a_mask == FALSE) {
				if (mask_prepared == FALSE) {
					if ((src_width < 72) &&
						(src_height < 72))
							transbmp = bw_bitmap;
					else {
						transbmp = CreateBitmap(src_width,src_height,1,1,NULL);
						create_transbmp = true;
					}
					old_bmp = (HBITMAP) SelectObject(main_dc3, transbmp);

					oldcolor = SetBkColor(main_dc2, white);
					StretchBlt(main_dc3,0,0,dest_width,dest_height,
						main_dc2,src_rect.left,src_rect.top,src_width,src_height,SRCCOPY);
					SetBkColor(main_dc2, oldcolor);
					StretchBlt(main_dc3,0,0,dest_width,dest_height,
						main_dc2,src_rect.left,src_rect.top,src_width,src_height,DSTINVERT);

					SelectObject(main_dc3, dest);
				}
			}		
			
			if (using_a_mask == FALSE) {
				SelectObject(main_dc2, transbmp);
				
				if (mask_prepared == TRUE) 
					StretchBlt(main_dc3,dest_rect.left,dest_rect.top,dest_width,dest_height,
						main_dc2,src_rect.left,src_rect.top,src_width,src_height,SRCPAINT);
				else
					StretchBlt(main_dc3,dest_rect.left,dest_rect.top,dest_width,dest_height,
									main_dc2,0,0,dest_width,dest_height,SRCPAINT);
					
					//BitBlt(main_dc3,dest_rect.left,dest_rect.top,dest_width,dest_height,
					//	main_dc2,src_rect.left,src_rect.top,SRCPAINT);
				
			} else {
				SelectObject(main_dc2, current_mask);
				StretchBlt(main_dc3,dest_rect.left,dest_rect.top,dest_width,dest_height,
					main_dc2,current_mask_rect.left,current_mask_rect.top,
					current_mask_rect.right - current_mask_rect.left,
					current_mask_rect.bottom - current_mask_rect.top,SRCPAINT);
				//BitBlt(main_dc3,dest_rect.left,dest_rect.top,dest_width,dest_height,
				//	main_dc2,current_mask_rect.left,current_mask_rect.top,SRCPAINT);
				
			}

			SelectObject(main_dc2, src);
			
			StretchBlt(main_dc3,dest_rect.left,dest_rect.top,dest_width,dest_height,
				main_dc2,src_rect.left,src_rect.top,src_width,src_height,SRCAND);
			//if (mask_prepared == TRUE)
			//BitBlt(main_dc3,dest_rect.left,dest_rect.top,dest_width,dest_height,
			//	main_dc2,src_rect.left,src_rect.top,SRCAND);
			
			if ( create_transbmp )
				DeleteObject(transbmp);
			SelectObject(main_dc3, storea);
			SelectObject(main_dc2, storeb);
		}
		else { // transparent, to screen, never used
			//if (dlog_draw == FALSE)
			//	SetViewportOrg(destDC,ulx,uly);
			if ((src_width < 72) && (src_height < 72))
				transbmp = bw_bitmap;
			else
				transbmp = CreateBitmap(src_width,src_height,1,1,NULL);
			hdcMem2 = CreateCompatibleDC(destDC);
			old_bmp = (HBITMAP)SelectObject(hdcMem2, transbmp);
			oldcolor = SetBkColor(hdcMem, white);
			StretchBlt(hdcMem2,0,0,dest_width,dest_height,
					hdcMem,src_rect.left,src_rect.top,src_width,src_height,SRCCOPY);
	
			SetBkColor(hdcMem, oldcolor);
			StretchBlt(hdcMem2,0,0,dest_width,dest_height,
					hdcMem,src_rect.left,src_rect.top,src_width,src_height,DSTINVERT);
	
			//StretchBlt(destDC,dest_rect.left,dest_rect.top,dest_width,dest_height,
			//		hdcMem,src_rect.left,src_rect.top,src_width,src_height,SRCINVERT);
			StretchBlt(destDC,dest_rect.left,dest_rect.top,dest_width,dest_height,
					hdcMem2,0,0,src_width,src_height,SRCPAINT);
			StretchBlt(destDC,dest_rect.left,dest_rect.top,dest_width,dest_height,
					hdcMem,src_rect.left,src_rect.top,src_width,src_height,SRCAND);
			//if (dlog_draw == FALSE)
			//	SetViewportOrg(destDC,0,0);
			SelectObject(hdcMem2, old_bmp);
			if (transbmp != bw_bitmap)
				DeleteObject(transbmp);
			DeleteDC(hdcMem2);
		}
	}
	if ( store )
		SelectObject(hdcMem,store);
	if (dlog_draw == TRUE)
		DeleteDC(hdcMem);
}

 void fry_dc(HWND hwnd,HDC dc) {
	//if (ReleaseDC(hwnd,dc) == 0)
	//	PostQuitMessage(0);
	//if (DeleteDC(dc) == 0)
	//	PostQuitMessage(0);
	ReleaseDC(hwnd,dc);
	DeleteDC(dc);
  }

// Setting color functions
// rgb are all in the 0 .. 31 range
/*
void set_blitter_color(unsigned int new_color)
{// not needed for editor
	//set_color.rgbtRed = GETR(new_color) << 3;
	//set_color.rgbtGreen = GETG(new_color) << 3;
	//set_color.rgbtBlue = GETB(new_color) << 3;
}
*/
/*
void hue_rect(HDIB dest,RECT dest_rect) 
{
	RGBTRIPLE *destBits;
//	RGBTRIPLE white = {255,255,255};
	short height = rect_height(&dest_rect);
	short width = rect_width(&dest_rect);
	short v,h;

//	RGBTRIPLE BMask = {0,0,255};
	BYTE dark_level;
	
	if (dest == NULL)
		return;

	dark_level = (BYTE) (store_mask * darkness_scale);
	for (v = 0; v < height; v++) {
		destBits = (RGBTRIPLE *) (DibPixelPtr (dest,dest_rect.left,dest_rect.top + v));
		for (h = 0; h < width; h++) {
			destBits->rgbtBlue = (destBits->rgbtBlue >> 1) + (set_color.rgbtBlue >> 1);
			destBits->rgbtRed = (destBits->rgbtRed >> 1) + (set_color.rgbtRed >> 1);
			destBits->rgbtGreen = (destBits->rgbtGreen >> 1) + (set_color.rgbtGreen >> 1);
				  
			destBits++;
			}
				
		}

}
*/
/*
void dim_rect(HDIB dest,RECT dest_rect,BYTE r_dim,BYTE g_dim,BYTE b_dim) 
{
	RGBTRIPLE *destBits;
//	RGBTRIPLE white = {255,255,255};
	short height = rect_height(&dest_rect);
	short width = rect_width(&dest_rect);
	short v,h;

//	RGBTRIPLE BMask = {0,0,255};
	BYTE dark_level;
	
	if (dest == NULL)
		return;

	dark_level = (BYTE) (store_mask * darkness_scale);
	for (v = 0; v < height; v++) {
		destBits = (RGBTRIPLE *) (DibPixelPtr (dest,dest_rect.left,dest_rect.top + v));
		for (h = 0; h < width; h++) {	
			destBits->rgbtBlue = (destBits->rgbtBlue < b_dim) ? 0 : destBits->rgbtBlue - b_dim;
			destBits->rgbtRed = (destBits->rgbtRed < r_dim) ? 0 : destBits->rgbtRed - r_dim;
			destBits->rgbtGreen = (destBits->rgbtGreen < g_dim) ? 0 : destBits->rgbtGreen - g_dim;
				  
			destBits++;
			}
				
		}

}
*/


// This is ONLY for drawing from a gworld into another gworld.
// This does NO clipping. Beware!
// adjusts:
//	0 - no
//	1 - shimmers
//	2 - use outline
/*
void transbitblit( HDIB src,RECT source_rect,HDIB dest,RECT dest_rect,short draw_mode,short adjusts) 
{

	RGBTRIPLE *destBits;
	RGBTRIPLE *srcBits;		
	RGBTRIPLE white = {255,255,255};
	short height = rect_height(&source_rect);
	short width = rect_width(&source_rect);
	short v,h;
	RGBTRIPLE halfmask = {128,128,128};
	RGBTRIPLE qtrmask = {64,64,64};

	RGBTRIPLE RMask = {255,0,0};
	RGBTRIPLE GMask = {0,255,0};
	RGBTRIPLE temp,BMask = {0,0,255};
	short anim_add;
	BYTE dark_level;
	 
	
	//rect_draw_some_item(src,source_rect,dest,dest_rect,1,0);
	//return;
	
	if ((src == NULL) || (dest == NULL))
		return;

	
			// Begin draws. There's a bunch of different things,e ach depending on a diff. way of drawing.
	switch (adjusts) {
		case 0: // normal
			switch (store_mask) {
				case 1: case 2: case 3: case 4: case 5: case 6: case 7:
					dark_level = (BYTE) (store_mask * darkness_scale);
					for (v = 0; v < height; v++) {
						srcBits = (RGBTRIPLE *) (DibPixelPtr (src,source_rect.left,source_rect.top + v));
						destBits = (RGBTRIPLE *) (DibPixelPtr (dest,dest_rect.left,dest_rect.top + v));
							for (h = 0; h < width; h++) {
									
								//	Our transparent color is white
								if ((srcBits->rgbtBlue != 255) ||
									(srcBits->rgbtGreen != 255) ||
									(srcBits->rgbtRed != 255)) {
									  destBits->rgbtBlue = (srcBits->rgbtBlue < dark_level) ? 0 : srcBits->rgbtBlue - dark_level;
									  destBits->rgbtRed = (srcBits->rgbtRed < dark_level) ? 0 : srcBits->rgbtRed - dark_level;
									  destBits->rgbtGreen = (srcBits->rgbtGreen < dark_level) ? 0 : srcBits->rgbtGreen - dark_level;
									  }
								destBits++;
								srcBits++;
									
							}
								
						}
					break;
				default:
					for (v = 0; v < height; v++) {
						srcBits = (RGBTRIPLE *) (DibPixelPtr (src,source_rect.left,source_rect.top + v));
						destBits = (RGBTRIPLE *) (DibPixelPtr (dest,dest_rect.left,dest_rect.top + v));

						//srcBits = ((PDIBSTRUCT) src)->ppRow)[source_rect.top + v] + source_rect.left;
						//destBits = ((PDIBSTRUCT) dest)->ppRow)[dest_rect.top + v] + dest_rect.left;

						for (h = 0; h < width; h++) {
									
								//	Our transparent color is white
								if ((srcBits->rgbtBlue != 255) ||
									(srcBits->rgbtGreen != 255) ||
									(srcBits->rgbtRed != 255))
									*destBits = *srcBits;
									//*destBits = (*srcBits & mask) << 1;
								destBits++;
								srcBits++;
									
							}
								
						}
					break;
				}
			break;
		case 1: // shimmer
			if (anim_ticks % 16 >= 8)
				anim_add = 8 - (anim_ticks % 8);
				else anim_add = anim_ticks % 8;
			anim_add = anim_add * 6;
			for (v = 0; v < height; v++)
				{
					srcBits = (RGBTRIPLE *) (DibPixelPtr (src,source_rect.left,source_rect.top + v));
					destBits = (RGBTRIPLE *) (DibPixelPtr (dest,dest_rect.left,dest_rect.top + v));
					for (h = 0; h < width; h++)
					{
						if ((srcBits->rgbtBlue != 255) ||
						  (srcBits->rgbtGreen != 255) ||
						  (srcBits->rgbtRed != 255)) {
							//temp = ((*srcBits & RMask) >> 10) + anim_add;
							//*destBits = ((temp > 31) ? 31 : temp) << 10;
							//temp = ((*srcBits & GMask) >> 5) + anim_add;
							//*destBits += ((temp > 31) ? 31 : temp) << 5;
							//temp = (*srcBits & BMask) + anim_add;
							//*destBits += ((temp > 31) ? 31 : temp);
							
							destBits->rgbtRed = srcBits->rgbtRed + anim_add;
							if (destBits->rgbtRed  < srcBits->rgbtRed)
								destBits->rgbtRed = 255;
							destBits->rgbtGreen = srcBits->rgbtGreen + anim_add;
							if (destBits->rgbtGreen  < srcBits->rgbtGreen)
								destBits->rgbtGreen = 255;
							destBits->rgbtBlue = srcBits->rgbtBlue + anim_add;
							if (destBits->rgbtBlue  < srcBits->rgbtBlue)
								destBits->rgbtBlue = 255;
							}
						destBits++;
						srcBits++;
							
					}
						

				}
			break;
		case 2: // outline
			for (v = 0; v < height; v++)
				{
					srcBits = (RGBTRIPLE *) (DibPixelPtr (src,source_rect.left,source_rect.top + v));
					destBits = (RGBTRIPLE *) (DibPixelPtr (dest,dest_rect.left,dest_rect.top + v));
					for (h = 0; h < width; h++)
					{
						if ((srcBits->rgbtBlue == 255) &&
						  (srcBits->rgbtGreen == 255) &&
						  (srcBits->rgbtRed == 255) && 
						  (((h > 0) && ((srcBits - 1)->rgbtBlue != 255)) || 
							((h < width - 1) && ((srcBits + 1)->rgbtBlue != 255)) ||
						 	((v > 0) && (((RGBTRIPLE *) (DibPixelPtr (src,source_rect.left + h,source_rect.top + v - 1)))->rgbtBlue != 255)) ||
							((v < height - 1) && (((RGBTRIPLE *) (DibPixelPtr (src,source_rect.left + h,source_rect.top + v + 1)))->rgbtBlue != 255)))) {
								*destBits = set_color;
							
								}
							else if ((srcBits->rgbtBlue != 255) ||
							  (srcBits->rgbtGreen != 255) ||
							  (srcBits->rgbtRed != 255)) 
								*destBits = *srcBits;

						destBits++;
						srcBits++;
							
					}
						

				}
			break;
		
		}
		

}
*/

// This is ONLY for drawing from a gworld into another gworld.
// This does NO clipping. Beware!
// This is ONLY for drawing floor sections, and, when drawing the whole floor section, it does some truly 
// excellent optimization. Also,  can do some animating and shimmering and stuff.
// adjusts:
//	0 - no
//	1 - animate water
/*
void floor_transbitblit( HDIB src,RECT source_rect,HDIB dest,RECT dest_rect,short draw_mode,short adjusts) 
{
	RGBTRIPLE *destBits;
	RGBTRIPLE *srcBits;		
	short height = rect_height(&source_rect);
	short width = rect_width(&source_rect);
	short v,h;
	RGBTRIPLE halfmask = {128,128,128};
	RGBTRIPLE qtrmask = {64,64,64};

	RGBTRIPLE RMask = {255,0,0};
	RGBTRIPLE GMask = {0,255,0};
	RGBTRIPLE temp,BMask = {0,0,255};
	RGBTRIPLE mask = {128,128,128};	
	short floor_animate_x,floor_animate_y;
	RECT anim_offsets = {0,0,0,0},base_source_rect;
	BYTE dark_level;

 	if ((src == NULL) || (dest == NULL))
		return;

	//rect_draw_some_item(src,source_rect,dest,dest_rect,1,0);
	//return;
	

	
	switch (store_mask) {
		case 1: case 2: case 3: case 4: case 5: case 6: case 7:
			dark_level = (BYTE) (store_mask * darkness_scale);
			if (adjusts == 0) {
				if ((height != 32) || (width != 46)) {
					for (v = 0; v < height; v++) {
						srcBits = (RGBTRIPLE *) (DibPixelPtr (src,source_rect.left,source_rect.top + v));
						destBits = (RGBTRIPLE *) (DibPixelPtr (dest,dest_rect.left,dest_rect.top + v));

						for (h = 0; h < width; h++) {
							//if ((v == 0) && (h == 0)) {
							//	white = *srcBits;
							//	}
								
							//	Our transparent color is white
							if ((srcBits->rgbtBlue != 255) ||
								(srcBits->rgbtGreen != 255) ||
								(srcBits->rgbtRed != 255)) {
								  destBits->rgbtBlue = (srcBits->rgbtBlue < dark_level) ? 0 : srcBits->rgbtBlue - dark_level;
								  destBits->rgbtRed = (srcBits->rgbtRed < dark_level) ? 0 : srcBits->rgbtRed - dark_level;
								  destBits->rgbtGreen = (srcBits->rgbtGreen < dark_level) ? 0 : srcBits->rgbtGreen - dark_level;
								  }

							destBits++;
							srcBits++;
							}
							
						}
					}
					else { // major optimization
						for (v = 0; v < height; v++)
							{
								srcBits = (RGBTRIPLE *) (DibPixelPtr (src,source_rect.left,source_rect.top + v));
								destBits = (RGBTRIPLE *) (DibPixelPtr (dest,dest_rect.left,dest_rect.top + v));
								srcBits += floor_offsets[v];
								destBits += floor_offsets[v];
								
								for (h = 0; h < floor_width[v]; h++)
								{		
									  destBits->rgbtBlue = (srcBits->rgbtBlue < dark_level) ? 0 : srcBits->rgbtBlue - dark_level;
									  destBits->rgbtRed = (srcBits->rgbtRed < dark_level) ? 0 : srcBits->rgbtRed - dark_level;
									  destBits->rgbtGreen = (srcBits->rgbtGreen < dark_level) ? 0 : srcBits->rgbtGreen - dark_level;
										//*destBits = (*srcBits & mask) << 1;
									destBits++;
									srcBits++;
										
								}
									
							}
						}
				} else {
					floor_animate_x = 0;
					floor_animate_y = 15 - (anim_ticks % 16);
					
					if ((height != 32) || (width != 46)) {
						base_source_rect = source_rect;
						OffsetRect(&base_source_rect,-1,-1);
						anim_offsets.left = base_source_rect.left % 47;
						anim_offsets.right = 46 - anim_offsets.left - rect_width(&source_rect);
						floor_animate_y = (floor_animate_y + (base_source_rect.top - 23)) % 16;
						
						for (v = 0; v < height; v++)
							{
								floor_animate_x = (floor_animate_x + anim_offsets.left) % 23;
								floor_animate_y = (floor_animate_y + 1) % 16;
								srcBits = (RGBTRIPLE *) (DibPixelPtr (src,source_rect.left,source_rect.top + v));
								destBits = (RGBTRIPLE *) (DibPixelPtr (dest,dest_rect.left,dest_rect.top + v));
								for (h = 0; h < width; h++)
								{
									//if ((v == 0) && (h == 0)) {
									//	white = *srcBits;
									//	}
										
									//	Our transparent color is white
									if ((srcBits->rgbtBlue != 255) ||
										(srcBits->rgbtGreen != 255) ||
										(srcBits->rgbtRed != 255)) {
										if ((srcBits->rgbtGreen < 110) && (srcBits->rgbtBlue >= 110)) {
											destBits->rgbtRed = srcBits->rgbtRed;
											destBits->rgbtGreen = srcBits->rgbtGreen;
											
											destBits->rgbtBlue = srcBits->rgbtBlue + 
											  (floor_animate[floor_animate_x][floor_animate_y] << 3);
											if (destBits->rgbtBlue <  srcBits->rgbtBlue)
												destBits->rgbtBlue = 255;
											}
											else *destBits = *srcBits;
										  destBits->rgbtBlue = (destBits->rgbtBlue < dark_level) ? 0 : destBits->rgbtBlue - dark_level;
										  destBits->rgbtRed = (destBits->rgbtRed < dark_level) ? 0 : destBits->rgbtRed - dark_level;
										  destBits->rgbtGreen = (destBits->rgbtGreen < dark_level) ? 0 : destBits->rgbtGreen - dark_level;
										
										}
									floor_animate_x = (floor_animate_x + 1) % 23;
									destBits++;
									srcBits++;
										
								}
									
								floor_animate_x = (floor_animate_x + anim_offsets.right) % 23;

							}
						}
						else { // major optimization
							for (v = 0; v < height; v++)
								{
									srcBits = (RGBTRIPLE *) (DibPixelPtr (src,source_rect.left,source_rect.top + v));
									destBits = (RGBTRIPLE *) (DibPixelPtr (dest,dest_rect.left,dest_rect.top + v));
									srcBits += floor_offsets[v];
									destBits += floor_offsets[v];
									floor_animate_x = (floor_animate_x + floor_offsets[v]) % 23;
									floor_animate_y = (floor_animate_y + 1) % 16;
									
									for (h = 0; h < floor_width[v]; h++)
									{	
										if ((srcBits->rgbtGreen < 110) && (srcBits->rgbtBlue >= 110)) {
											destBits->rgbtRed = srcBits->rgbtRed;
											destBits->rgbtGreen = srcBits->rgbtGreen;
											
											destBits->rgbtBlue = srcBits->rgbtBlue + 
											  (floor_animate[floor_animate_x][floor_animate_y] << 3);
											if (destBits->rgbtBlue <  srcBits->rgbtBlue)
												destBits->rgbtBlue = 255;
											}
											else *destBits = *srcBits;

										  destBits->rgbtBlue = (destBits->rgbtBlue < dark_level) ? 0 : destBits->rgbtBlue - dark_level;
										  destBits->rgbtRed = (destBits->rgbtRed < dark_level) ? 0 : destBits->rgbtRed - dark_level;
										  destBits->rgbtGreen = (destBits->rgbtGreen < dark_level) ? 0 : destBits->rgbtGreen - dark_level;

										floor_animate_x = (floor_animate_x + 1) % 23;
										destBits++;
										srcBits++;
											
									}
										
									floor_animate_x = (floor_animate_x + floor_offsets[v]) % 23;
								}
							}

				}
			break;
	
		default: 

			if (adjusts == 0) {
				if ((height != 32) || (width != 46)) {
					for (v = 0; v < height; v++)
						{
							srcBits = (RGBTRIPLE *) (DibPixelPtr (src,source_rect.left,source_rect.top + v));
							destBits = (RGBTRIPLE *) (DibPixelPtr (dest,dest_rect.left,dest_rect.top + v));
							for (h = 0; h < width; h++)
							{
								//if ((v == 0) && (h == 0)) {
								//	white = *srcBits;
								//	}
									
								//	Our transparent color is white
								if ((srcBits->rgbtBlue != 255) ||
									(srcBits->rgbtGreen != 255) ||
									(srcBits->rgbtRed != 255))
									*destBits = *srcBits;
								destBits++;
								srcBits++;
									
							}
								
						}
					}
					else { // major optimization
						for (v = 0; v < height; v++)
							{
								srcBits = (RGBTRIPLE *) (DibPixelPtr (src,source_rect.left,source_rect.top + v));
								destBits = (RGBTRIPLE *) (DibPixelPtr (dest,dest_rect.left,dest_rect.top + v));
								srcBits += floor_offsets[v];
								destBits += floor_offsets[v];
								
								for (h = 0; h < floor_width[v]; h++)
								{		
									*destBits = *srcBits;
										//*destBits = (*srcBits & mask) << 1;
									destBits++;
									srcBits++;
										
								}
									
							}
						}
				} else {

					floor_animate_x = 0;
					floor_animate_y = 15 - (anim_ticks % 16);
					
					if ((height != 32) || (width != 46)) {
						base_source_rect = source_rect;
						OffsetRect(&base_source_rect,-1,-1);
						anim_offsets.left = base_source_rect.left % 47;
						anim_offsets.right = 46 - anim_offsets.left - rect_width(&source_rect);
						floor_animate_y = (floor_animate_y + (base_source_rect.top - 23)) % 16;
						
						for (v = 0; v < height; v++)
							{
								srcBits = (RGBTRIPLE *) (DibPixelPtr (src,source_rect.left,source_rect.top + v));
								destBits = (RGBTRIPLE *) (DibPixelPtr (dest,dest_rect.left,dest_rect.top + v));
								floor_animate_x = (floor_animate_x + anim_offsets.left) % 23;
								floor_animate_y = (floor_animate_y + 1) % 16;
								for (h = 0; h < width; h++)
								{

									//	Our transparent color is white
									if ((srcBits->rgbtBlue != 255) ||
										(srcBits->rgbtGreen != 255) ||
										(srcBits->rgbtRed != 255)) {
										if ((srcBits->rgbtGreen < 110) && (srcBits->rgbtBlue >= 110)) {
											destBits->rgbtRed = srcBits->rgbtRed;
											destBits->rgbtGreen = srcBits->rgbtGreen;
											
											destBits->rgbtBlue = srcBits->rgbtBlue + 
											  (floor_animate[floor_animate_x][floor_animate_y] << 3);
											if (destBits->rgbtBlue <  srcBits->rgbtBlue)
												destBits->rgbtBlue = 255;
											}
											else *destBits = *srcBits;
										}
									floor_animate_x = (floor_animate_x + 1) % 23;
									destBits++;
									srcBits++;
										
								}
									
								floor_animate_x = (floor_animate_x + anim_offsets.right) % 23;

							}
						}
						else { // major optimization
							for (v = 0; v < height; v++)
								{
									srcBits = (RGBTRIPLE *) (DibPixelPtr (src,source_rect.left,source_rect.top + v));
									destBits = (RGBTRIPLE *) (DibPixelPtr (dest,dest_rect.left,dest_rect.top + v));
									srcBits += floor_offsets[v];
									destBits += floor_offsets[v];
									floor_animate_x = (floor_animate_x + floor_offsets[v]) % 23;
									floor_animate_y = (floor_animate_y + 1) % 16;
									
									for (h = 0; h < floor_width[v]; h++)
									{	
										if ((srcBits->rgbtGreen < 110) && (srcBits->rgbtBlue >= 110)) {
											destBits->rgbtRed = srcBits->rgbtRed;
											destBits->rgbtGreen = srcBits->rgbtGreen;
											
											destBits->rgbtBlue = srcBits->rgbtBlue + 
											  (floor_animate[floor_animate_x][floor_animate_y] << 3);
											if (destBits->rgbtBlue <  srcBits->rgbtBlue)
												destBits->rgbtBlue = 255;
											}
											else *destBits = *srcBits;
										floor_animate_x = (floor_animate_x + 1) % 23;
										destBits++;
										srcBits++;
											
									}
										
									floor_animate_x = (floor_animate_x + floor_offsets[v]) % 23;
								}
							}

					}
			break;
		}

}
*/
/*
void floor_draw_lower_frame( HDIB src,RECT source_rect,HDIB dest,RECT dest_rect,short draw_mode,short adjusts) 
{
	RGBTRIPLE *destBits;
	RGBTRIPLE *srcBits;		
	short height = rect_height(&source_rect);
	short width = rect_width(&source_rect);
	short v,h;
//	RECT anim_offsets = {0,0,0,0};


 	if ((src == NULL) || (dest == NULL))
		return;
	
		
	if ((height != 32) || (width != 46)) {
	
		for (v = 0; v < height; v++) {
				srcBits = (RGBTRIPLE *) (DibPixelPtr (src,source_rect.left,source_rect.top + v));
				destBits = (RGBTRIPLE *) (DibPixelPtr (dest,dest_rect.left,dest_rect.top + v));
				for (h = 0; h < width; h++) {
					//	Our transparent color is white
					if (((srcBits->rgbtBlue != 255) ||
						(srcBits->rgbtGreen != 255) ||
						(srcBits->rgbtRed != 255)) && 
					  (((RGBTRIPLE *) (DibPixelPtr (src,source_rect.left + h,source_rect.top + v + 1)))->rgbtBlue == 255) &&
					  (((RGBTRIPLE *) (DibPixelPtr (src,source_rect.left + h,source_rect.top + v + 1)))->rgbtRed == 255)) {
						if (destBits->rgbtBlue >= 30) {
							destBits->rgbtBlue = destBits->rgbtBlue >> 1;
							destBits->rgbtGreen = destBits->rgbtGreen >> 1;
							destBits->rgbtRed = destBits->rgbtRed >> 1;							
							}
						}
					destBits++;
					srcBits++;
						
					}
//*((UInt16 *) (DibPixelPtr (src,source_rect.left + h,source_rect.top + v + 1))) != white					
			}
		
		}
		else { // major optimization
			for (v = 0; v < height; v++)
				if (v >= height / 2) {
					srcBits = (RGBTRIPLE *) (DibPixelPtr (src,source_rect.left,source_rect.top + v));
					destBits = (RGBTRIPLE *) (DibPixelPtr (dest,dest_rect.left,dest_rect.top + v));
					destBits += floor_offsets[v];
					
					if (destBits->rgbtBlue >= 30) {
						destBits->rgbtBlue = destBits->rgbtBlue >> 1;
						destBits->rgbtGreen = destBits->rgbtGreen >> 1;
						destBits->rgbtRed = destBits->rgbtRed >> 1;							
						}					
					destBits += floor_width[v] - 1;
					if (destBits->rgbtBlue >= 30) {
						destBits->rgbtBlue = destBits->rgbtBlue >> 1;
						destBits->rgbtGreen = destBits->rgbtGreen >> 1;
						destBits->rgbtRed = destBits->rgbtRed >> 1;							
						}	
					}
			}
						
	


}
*/

void SectRect(RECT *a, RECT *b, RECT *c) 
{
	IntersectRect(c,a,b);
}

/*
Boolean Button()
{
	MSG msg;

	if (PeekMessage(&msg,mainPtr,WM_MOUSEFIRST,WM_MOUSELAST,PM_REMOVE) > 0)
		if ((msg.message == WM_LBUTTONDOWN) || (msg.message == WM_CHAR)
		|| (msg.message == WM_KEYDOWN))
			return TRUE;
	if (PeekMessage(&msg,mainPtr,WM_KEYFIRST,WM_KEYLAST,PM_REMOVE) > 0)
		if ((msg.message == WM_LBUTTONDOWN) || (msg.message == WM_CHAR)
		|| (msg.message == WM_KEYDOWN))
			return TRUE;
	return FALSE;
}
*/

// which_mode is 0 ... dest is a bitmap
// is 1 ... ignore dest ... paint on mainPtr
// is 2 ... dest is a dialog, use the dialog pattern
// both pattern gworlds are 192 x 256
// If which_pattern is 1, using basic wood pattern. Are not using fill, but using basic draws.
void paint_pattern(HDIB dest,short which_mode,RECT dest_rect,short which_pattern)
{
//	RECT pattern_source = {0,0,64,64};
//	RECT pat_dest_orig = {0,0,64,64};
//	RECT source_rect = {0,221,128,349};
	short i,j;

	RECT draw_from_orig = {0,0,128,128},draw_from,draw_to;
	short store_ulx,store_uly;

/*
	if (which_pattern == 1) {
		num_horiz = dest_rect.right / 128 + 1;
		num_vert = dest_rect.bottom / 128 + 1;
		
		for (i = 0; i < num_horiz; i++)
			for (j = 0; j < num_vert; j++) {
				//from_rect = source_rect;
				
				SetRect(&to_rect,i * 128, j * 128,(i + 1) * 128,(j + 1) * 128);
				to_rect2 = rect_sect(&to_rect,&dest_rect);
				if (rect_empty(&to_rect2) == FALSE) {
					from_rect = to_rect2;
					OffsetRect(&from_rect,-i * 128,-j * 128);

					switch (which_mode) {
						case 0: rect_draw_some_item(ed_pattern_gworld,from_rect,dest,to_rect2,0,0); break;
						case 1: rect_draw_some_item(ed_pattern_gworld,from_rect,mixed_gworld,to_rect2,0,1); break;
						case 2: rect_draw_some_item(ed_pattern_gworld,from_rect,dest,to_rect2,0,2); break;	
						}
	
					}
				}

		return;
		}
*/

/*
	if ((which_mode == 2) || (which_pattern == 1)) {
		source_pat = dialog_pattern_gworld;
		if (dlog_pat_placed == 0) {
			dlog_pat_placed = 1;
			OffsetRect(&pattern_source, 64,0);
			for (i = 0; i < 3; i++)
				for (j = 0; j < 4; j++) {
					pat_dest = pat_dest_orig;
					OffsetRect(&pat_dest,64 * i, 64 * j);
					rect_draw_some_item(pat_source_gworld,pattern_source,
						dialog_pattern_gworld,pat_dest,0,0);
					}
			}
		}
		else {
			source_pat = pattern_gworld;
			if (current_pattern != which_pattern) {
            	current_pattern = which_pattern;
				OffsetRect(&pattern_source, 64 * (which_pattern % 5),
					64 * (which_pattern / 5));
				for (i = 0; i < 3; i++)
					for (j = 0; j < 4; j++) {
						pat_dest = pat_dest_orig;
						OffsetRect(&pat_dest,64 * i, 64 * j);
						rect_draw_some_item(pat_source_gworld,pattern_source,
							pattern_gworld,pat_dest,0,0);
						}
				}
			}
*/

	// now patterns are loaded, so have fun
	// first nullify ul shifting
	store_ulx = ulx;
	store_uly = uly;
	ulx = uly = 0;
	for (i = 0; i < (dest_rect.right / 128) + 1; i++)
		for (j = 0; j < (dest_rect.bottom / 128) + 1; j++) {
			draw_to = draw_from_orig;
			OffsetRect(&draw_to,128 * i, 128 * j);
			IntersectRect(&draw_to,&draw_to,&dest_rect);
			if (draw_to.right != 0) {
				draw_from = draw_to;
				OffsetRect(&draw_from, -128 * i + 128 * which_pattern, -128 * j);
				switch (which_mode) {
					case 0:
						rect_draw_some_item(ed_pattern_gworld,draw_from,
							dest,draw_to,0,0); break;
					case 1:
						rect_draw_some_item(ed_pattern_gworld,draw_from,
							ed_pattern_gworld,draw_to,0,1); break;
					case 2:
						rect_draw_some_item(ed_pattern_gworld,draw_from,
							dest,draw_to,0,2); break;
					}
				}
			}
	ulx = store_ulx;
	uly = store_uly;
}

// THis is here for the rare trimes when we need to specifically draw from BMPs
// No frills
/*
void bmp_rect_draw_some_item(HBITMAP src,RECT src_rect,HBITMAP dest,RECT dest_rect,
	short trans, short main_win) {
	HDC hdcMem,hdcMem2 = NULL,destDC;
	HBITMAP transbmp = NULL,old_bmp;
	COLORREF white = RGB(255,255,255), oldcolor;
//	COLORREF black = RGB(0,0,0);
	HBRUSH hbrush,old_brush;
	COLORREF x = RGB(80,80,255);//RGB(150,150,255);
	HBITMAP store,store2,storea,storeb;
	Boolean dlog_draw = FALSE,mask_prepared = FALSE;
		
	if (main_win == 2) {
		destDC = (HDC) dest;
		main_win = 1;
		dlog_draw = TRUE;
		hdcMem = CreateCompatibleDC(destDC);
		SelectObject(hdcMem, src);
		SetMapMode(hdcMem,GetMapMode(main_dc));
		//SelectPalette(hdcMem,hpal,0);
		}
		else {
			destDC = main_dc;
			hdcMem = main_dc2;
			store = (HBITMAP) SelectObject(hdcMem,src);
			}

	switch (store_store_mask) {
		case 0: break;
		case 1: case 2: case 3: case 4: case 5: case 6: 
			shade_color = RGB(200,200,200); operation = MERGECOPY; break;
		case 8: shade_color = RGB(150,200,255); operation = MERGECOPY; break;
		case 9: shade_color = RGB(255,0,255); operation = MERGECOPY; break;
		default: shade_color = RGB(0,255,0); operation = MERGECOPY; break;


	if (trans != 1) {
		if (main_win == 0) { // Not transparent, into bitmap
			hdcMem2 = main_dc3;
			store2 = (HBITMAP) SelectObject(hdcMem2, dest);

			if (trans == 2) {
				hbrush = CreateSolidBrush(x);
				old_brush = (HBRUSH) SelectObject(hdcMem2,hbrush);
				}
								
			if (store_store_mask > 0) {
				//hbrush = CreateSolidBrush(shade_color);
				//old_brush = SelectObject(hdcMem2, hbrush);
				switch (store_store_mask) {
					case 8: SetTextColor(hdcMem2,RGB(0,0,255)); break;
					case 9: SetTextColor(hdcMem2,RGB(255,0,0)); break;
					case 10: SetTextColor(hdcMem2,RGB(0,255,0)); break;
					}
				}				
			//StretchBlt(hdcMem2,dest_rect.left,dest_rect.top,dest_rect.right - dest_rect.left,
			//	dest_rect.bottom - dest_rect.top,
			//	hdcMem,src_rect.left,src_rect.top,src_rect.right - src_rect.left,
			//	src_rect.bottom - src_rect.top, (trans == 0) ? SRCCOPY : MERGECOPY);
			BitBlt(hdcMem2,dest_rect.left,dest_rect.top,dest_rect.right - dest_rect.left,
				dest_rect.bottom - dest_rect.top,
				hdcMem,src_rect.left,src_rect.top, (trans == 0) ? SRCCOPY : MERGECOPY);
			
			if (store_store_mask > 0) {
				SetTextColor(hdcMem2,RGB(0,0,0));
				}				

			if (trans == 2) {
				SelectObject(hdcMem2,old_brush);
				if (DeleteObject(hbrush) == 0)
					play_sound(1);
				}
			
			SelectObject(hdcMem2,store2);
			}

			else { // Not transparent, onto screen
				if (trans == 2) { /// hued draw
					hbrush = CreateSolidBrush(x);
					old_brush = (HBRUSH) SelectObject(destDC,hbrush);
					}

		
//				StretchBlt(destDC,dest_rect.left,dest_rect.top,dest_rect.right - dest_rect.left,
//					dest_rect.bottom - dest_rect.top,
///					hdcMem,src_rect.left,src_rect.top,src_rect.right - src_rect.left,
	//				src_rect.bottom - src_rect.top,(trans == 0) ? SRCCOPY : MERGECOPY);
				BitBlt(destDC,dest_rect.left,dest_rect.top,dest_rect.right - dest_rect.left,
					dest_rect.bottom - dest_rect.top,
					hdcMem,src_rect.left,src_rect.top,(trans == 0) ? SRCCOPY : MERGECOPY);

				if (trans == 2) {
					SelectObject(destDC,old_brush);
					if (DeleteObject(hbrush) == 0)
						play_sound(1);
					}

		
		
				}
		} // end of non-transparent draws
	if (trans == 1) { // begin transparent draws
		if (main_win == 0) { // transparent, to bmp
			storea = (HBITMAP) SelectObject(main_dc3, dest);
			SelectObject(hdcMem,store);
			storeb = (HBITMAP) SelectObject(main_dc2, src);
	
			if (using_a_mask == FALSE) {
				if (mask_prepared == FALSE) {
					if ((src_rect.right - src_rect.left < 72) &&
						(src_rect.bottom - src_rect.top < 72))
							transbmp = bw_bitmap;
							else transbmp = CreateBitmap(src_rect.right - src_rect.left,
								src_rect.bottom - src_rect.top,1,1,NULL);
					old_bmp = (HBITMAP) SelectObject(main_dc3, transbmp);
					
					//mask_rect = src_rect;
					//OffsetRect(&mask_rect,-1 * mask_rect.left,-1 * mask_rect.top);
					//create_bitmap_mask(main_dc2,src_rect,main_dc3,mask_rect);
					
					//if (src_rect.bottom - src_rect.top > 35) {
					//	print_rect(1,src_rect);
						//print_rect(2,mask_rect);
					//	}
					oldcolor = SetBkColor(main_dc2, white);
					BitBlt(main_dc3,0,0,dest_rect.right - dest_rect.left,
						dest_rect.bottom - dest_rect.top,
						main_dc2,src_rect.left,src_rect.top,SRCCOPY);
					SetBkColor(main_dc2, oldcolor);
					BitBlt(main_dc3,0,0,dest_rect.right - dest_rect.left,
						dest_rect.bottom - dest_rect.top,
						main_dc2,src_rect.left,src_rect.top,DSTINVERT);
					
					
					
					SelectObject(main_dc3, dest);
					}

				}
				else {

					}
			
			if (using_a_mask == FALSE) {
				SelectObject(main_dc2, transbmp);
				
				if (mask_prepared == TRUE) 
					BitBlt(main_dc3,dest_rect.left,dest_rect.top,dest_rect.right - dest_rect.left,
						dest_rect.bottom - dest_rect.top,
						main_dc2,src_rect.left,src_rect.top,SRCPAINT);
					else BitBlt(main_dc3,dest_rect.left,dest_rect.top,dest_rect.right - dest_rect.left,
						dest_rect.bottom - dest_rect.top,
						main_dc2,0,0,SRCPAINT);
					
					//BitBlt(main_dc3,dest_rect.left,dest_rect.top,dest_rect.right - dest_rect.left,
					//	dest_rect.bottom - dest_rect.top,
					//	main_dc2,src_rect.left,src_rect.top,SRCPAINT);
				
				}
				else {
					SelectObject(main_dc2, current_mask);
					//StretchBlt(main_dc3,dest_rect.left,dest_rect.top,dest_rect.right - dest_rect.left,
					//	dest_rect.bottom - dest_rect.top,
					//	main_dc2,current_mask_rect.left,current_mask_rect.top,
					//	current_mask_rect.right - current_mask_rect.left,
					//	current_mask_rect.bottom - current_mask_rect.top,SRCPAINT);
					BitBlt(main_dc3,dest_rect.left,dest_rect.top,dest_rect.right - dest_rect.left,
						dest_rect.bottom - dest_rect.top,
						main_dc2,current_mask_rect.left,current_mask_rect.top,SRCPAINT);
				
					}


			SelectObject(main_dc2, src);
			
			//StretchBlt(main_dc3,dest_rect.left,dest_rect.top,dest_rect.right - dest_rect.left,
			//	dest_rect.bottom - dest_rect.top,
			//	main_dc2,src_rect.left,src_rect.top,src_rect.right - src_rect.left,
			//	src_rect.bottom - src_rect.top,SRCAND);
			//if (mask_prepared == TRUE)
			BitBlt(main_dc3,dest_rect.left,dest_rect.top,dest_rect.right - dest_rect.left,
				dest_rect.bottom - dest_rect.top,
				main_dc2,src_rect.left,src_rect.top,SRCAND);
			
			if ((transbmp != bw_bitmap) && (transbmp != NULL) && (mask_prepared == FALSE))
				DeleteObject(transbmp);
			SelectObject(main_dc3, storea);
			SelectObject(main_dc2, storeb);
			}
			else { // transparent, to screen, never used
				//if (dlog_draw == FALSE)
				//	SetViewportOrg(destDC,ulx,uly);
				if ((src_rect.right - src_rect.left < 72) &&
					(src_rect.bottom - src_rect.top < 72))
						transbmp = bw_bitmap;
						else transbmp = CreateBitmap(src_rect.right - src_rect.left,
							src_rect.bottom - src_rect.top,1,1,NULL);
				hdcMem2 = CreateCompatibleDC(destDC);
				SelectObject(hdcMem2, transbmp);
				oldcolor = SetBkColor(hdcMem, white);
				StretchBlt(hdcMem2,0,0,dest_rect.right - dest_rect.left,
					dest_rect.bottom - dest_rect.top,
					hdcMem,src_rect.left,src_rect.top,src_rect.right - src_rect.left,
					src_rect.bottom - src_rect.top,SRCCOPY);
	
				SetBkColor(hdcMem, oldcolor);
				StretchBlt(hdcMem2,0,0,dest_rect.right - dest_rect.left,
					dest_rect.bottom - dest_rect.top,
					hdcMem,src_rect.left,src_rect.top,src_rect.right - src_rect.left,
					src_rect.bottom - src_rect.top,DSTINVERT);
	
				//StretchBlt(destDC,dest_rect.left,dest_rect.top,dest_rect.right - dest_rect.left,
				//	dest_rect.bottom - dest_rect.top,
				//	hdcMem,src_rect.left,src_rect.top,src_rect.right - src_rect.left,
				//	src_rect.bottom - src_rect.top,SRCINVERT);
				StretchBlt(destDC,dest_rect.left,dest_rect.top,dest_rect.right - dest_rect.left,
					dest_rect.bottom - dest_rect.top,
					hdcMem2,0,0,src_rect.right - src_rect.left,
					src_rect.bottom - src_rect.top,SRCPAINT);
				StretchBlt(destDC,dest_rect.left,dest_rect.top,dest_rect.right - dest_rect.left,
					dest_rect.bottom - dest_rect.top,
					hdcMem,src_rect.left,src_rect.top,src_rect.right - src_rect.left,
					src_rect.bottom - src_rect.top,SRCAND);
				//if (dlog_draw == FALSE)
				//	SetViewportOrg(destDC,0,0);
				DeleteDC(hdcMem2);
				if (transbmp != bw_bitmap)
					DeleteObject(transbmp);
	
	
				}
		}
	if (dlog_draw == TRUE)
		DeleteDC(hdcMem);
		else SelectObject(hdcMem,store);
}
*/
