//============================================================================
//----------------------------------------------------------------------------
//									EdUtils.c
//----------------------------------------------------------------------------
//============================================================================

// General utility functions.

#include "stdafx.h"
/*
#include <stdio.h>
#include "string.h"
*/

#include "global.h"

// Global variables


// external global variables

extern town_record_type town;
extern scen_item_data_type scen_data;

// local variables


// function prototype

// void debug_stop(short which_error);
// void RedAlert_c (char *theStr);
// void RedAlert (char *theStr);
// void RedAlert_big_color (char *str1,char *str2,char *str3,char *str4,short num,char *str5,short color);
// void RedAlert_big (char *str1,char *str2,char *str3,char *str4,short num,char *str5);
void display_error_dialog(const char* theStr,Boolean shut_down);
// short rect_dist(RECT *r1,RECT *r2);
// RECT rect_union(RECT *r1,RECT *r2);
Boolean r1_in_r2(macRECT r1,macRECT r2);
// RECT rect_centered_around_point(RECT r, location l);
// Boolean loc_in_rect(location loc,RECT r);
// Boolean loc_touch_rect(location loc,macRECT r);
// short a_v(short x);


// for cross-platform compatibility

void SetRECT( RECT& rect, int left, int top, int right, int bottom )
{
	rect.left = left;	rect.top = top;	rect.right = right;	rect.bottom = bottom;
}

void OffsetLocation( location& loc, int xofst, int yofst )
{
	loc.x = loc.x + (char)xofst;	loc.y = loc.y + (char)yofst;
}

// Beaware, in xxxInRECT, right and bottom is not included in the area
// The RECT is on the mesh, but the POINT is within the mesh.
// The original concept is from Mac QuickDraw
bool LocationInRECT( location& loc, RECT& rect )
{
	return ( (rect.left <= (int)loc.x) && ((int)loc.x < rect.right)
			&& (rect.top <= (int)loc.y) && ((int)loc.y < rect.bottom) );
}

bool POINTInRECT( POINT p, RECT r )
{
	return ( (r.left <= (int)p.x) && ((int)p.x < r.right)
			&& (r.top <= (int)p.y) && ((int)p.y < r.bottom) );
}



// DBUG UTILITIES

// Used to stop game when an error happens
/*
void debug_stop(short which_error)
{
//	short i = which_error;
}
*/
/*
void RedAlert_c (char *theStr)
{
	char new_str[256];
	
	strcpy((char *) new_str,theStr);
	c2p(new_str);
	display_error_dialog(new_str,TRUE);
}
*/

void ASB (const char* theStr)
{
	char new_str[256];
	
	strncpy((char *) new_str,theStr,255);
	new_str[255]=0; //strncpy doesn't guarantee null termination
	display_error_dialog(new_str,FALSE);
}

void ASB_big (const char* str1,const char* str2,const char* str3,const char* str4,short num,const char* str5)
{
	char str[256];
	//!!!: this may overflow
	if (num >= 0)
		sprintf((char *) str,"%s%s%s%s%d%s",str1,str2,str3,str4,(int)num,str5);
	else
        sprintf((char *) str,"%s%s%s%s%s",str1,str2,str3,str4,str5);
	display_error_dialog(str,FALSE);
}

/*
void ASB_big_color (char *str1,char *str2,char *str3,char *str4,short num,char *str5,short dummy)
{
	char str[256];
	
	if (num >= 0)
		sprintf((char *) str,"%s%s%s%s%d%s",str1,str2,str3,str4,(int)num,str5);
		else sprintf((char *) str,"%s%s%s%s%s",str1,str2,str3,str4,str5);
	c2p(str);
	display_error_dialog(str,FALSE);
}
*/
/*
void RedAlert (char *theStr)
{
	display_error_dialog(theStr,TRUE);
}
*/
/*
void RedAlert_big_color (char *str1,char *str2,char *str3,char *str4,short num,char *str5,short color)
{
	char str[256];
	
	if (num >= 0)
		sprintf((char *) str,"%s%s%s%s%d%s",str1,str2,str3,str4,(int)num,str5);
		else sprintf((char *) str,"%s%s%s%s%s",str1,str2,str3,str4,str5);
	c2p(str);
	display_error_dialog(str,TRUE);
}
*/
/*
void RedAlert_big (char *str1,char *str2,char *str3,char *str4,short num,char *str5)
{
	char str[256];
	
	if (num >= 0)
		sprintf((char *) str,"%s%s%s%s%d%s",str1,str2,str3,str4,(int)num,str5);
		else sprintf((char *) str,"%s%s%s%s%s",str1,str2,str3,str4,str5);
	c2p(str);
	display_error_dialog(str,TRUE);
}
*/

void display_error_dialog(const char* theStr,Boolean shut_down)
{
	mes_box(theStr);			// Bring up alert.
	if (shut_down == TRUE)
		PostQuitMessage(0);
}

void ZeroRectCorner (RECT* theRect)
{
	theRect->right -= theRect->left;	// Move right edge by amount of left.
	theRect->bottom -= theRect->top;	// Move bottom edge by amount of top.
	theRect->left = 0;					// Can now set left to zero.
	theRect->top = 0;					// Can set top edge to zero as well.
}


// Handy function for returning the absolute width of a rectangle.
short rect_width (RECT* theRect)
{
	return (short)(theRect->right - theRect->left);
}

// Handy function for returning the absolute height of a rectangle.
short rect_height (RECT* theRect)
{
	return (short)(theRect->bottom - theRect->top);
}

// Other rectangle functions
Boolean rects_touch(RECT* r1,RECT* r2)
{
	if (r1->right <= r2->left)
		return (FALSE);
		
	if (r1->left >= r2->right)
		return (FALSE);
		
	if (r1->top >= r2->bottom)
		return (FALSE);
		
	if (r1->bottom <= r2->top)
		return (FALSE);
		
	return TRUE;

}

/*
short rect_dist(RECT *r1,RECT *r2)
{
	if (rects_touch(r1,r2))
		return 0;

	short cur_dist = 0;
		
	if ((r2->left >= r1->right) && (r2->left - r1->right > cur_dist))
		cur_dist = r2->left - r1->right;
		
	if ((r1->left >= r2->right) && (r1->left - r2->right > cur_dist))
		cur_dist = r1->left - r2->right;
		
	if ((r1->top >= r2->bottom) && (r1->top - r2->bottom > cur_dist))
		cur_dist = r1->top - r2->bottom;
		
	if ((r2->top >= r1->bottom) && (r2->top - r1->bottom > cur_dist))
		cur_dist = r2->top - r1->bottom;
		
	return cur_dist;

}
*/
/*
Boolean rect_empty(RECT *r)
{
	if ((r->top >= r->bottom) || (r->right <= r->left))
		return TRUE;
	return FALSE;
}
*/
/*
RECT rect_union(RECT *r1,RECT *r2)
{
	RECT r;
	
	UnionRect(r1,r2,&r);
	return r;
}
*/
/*
RECT rect_sect(RECT *r1,RECT *r2)
{
	RECT r;
	
	SectRect(r1,r2,&r);
	return r;
}
*/

Boolean r1_in_r2(macRECT r1,macRECT r2)
{
	if ((r1.left >= r2.left) && (r1.top >= r2.top) && 
		(r1.bottom <= r2.bottom) && (r1.right <= r2.right))
			return TRUE;
	return FALSE;
}

// returns the given rect, but centered around point p
/*
RECT rect_centered_around_point(RECT r, location l)
{
	short w = rect_width(&r);
	short h = rect_height(&r);
	SetRect(&r,l.x - (w / 2),l.y - (h / 2),l.x - (w / 2) + w,l.y - (h / 2) + h);
	return r;
}
*/

void CenterRectInRect (RECT* rectA, RECT* rectB)
{
	short	widthA, tallA;
	
	widthA = rect_width(rectA);				// Get width of 1st rect.
	tallA = rect_height(rectA);				// Get height of 1st rect.
											// Do the math (center horizontally).
	rectA->left = rectB->left + (rect_width(rectB) - widthA) / 2;
	rectA->right = rectA->left + widthA;
											// Do the math (center vertically).
	rectA->top = rectB->top + (rect_height(rectB) - tallA) / 2;
	rectA->bottom = rectA->top + tallA;
}

Boolean same_string(const char* str1,const char* str2)
{
	return(!strcmp(str1,str2));
}



// Returns TRUE is this location is in the active region of the zone
Boolean loc_in_active_area(location loc)
{
	return loc_touches_rect(loc,town.in_town_rect);
}

/*
Boolean loc_in_rect(location loc,RECT r)
{
	if ((loc.x >= r.left) && (loc.x < r.right) && (loc.y >= r.top) && (loc.y < r.bottom))
		return TRUE;
	return FALSE;
}
*/
// Returns TRUE is this location touches (is not strictly inside of) rectangle
Boolean loc_touches_rect(location loc,macRECT r)
{
	if ((loc.x >= r.left) && (loc.x <= r.right) && (loc.y >= r.top) && (loc.y <= r.bottom))
		return TRUE;
	return FALSE;
}

/*
Boolean loc_touch_rect(location loc,macRECT r)
{
	if ((loc.x >= r.left) && (loc.x <= r.right) && (loc.y >= r.top) && (loc.y <= r.bottom))
		return TRUE;
	return FALSE;
}
*/
/*
short a_v(short x)
{
	if (x < 0)
		return (-1 * x);
		else return x;
}
*/

terrain_type_type get_ter(short which_ter)
{
	terrain_type_type ter;
		ter = scen_data.scen_ter_types[which_ter];
	return ter;
}

void SetMacRect(macRECT* rect,short top, short left, short bottom, short right)
{
	rect->top = top;
	rect->left = left;
	rect->bottom = bottom;
	rect->right = right;
}

Boolean loc_touches_rect_border(location loc,macRECT r)
{
	if ((loc.y == r.top) && (loc.x >= r.left) && (loc.x <= r.right))
		return TRUE;
	if ((loc.x == r.left) && (loc.y >= r.top) && (loc.y <= r.bottom))
		return TRUE;
	if ((loc.y == r.bottom) && (loc.x >= r.left) && (loc.x <= r.right))
		return TRUE;
	if ((loc.x == r.right) && (loc.y >= r.top) && (loc.y <= r.bottom))
		return TRUE;
	return FALSE;
}
