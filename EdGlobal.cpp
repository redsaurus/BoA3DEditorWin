#include "stdafx.h"
/*
#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
*/
#include <math.h>

#include "global.h"

// Gloabl varialbes


// external gloabal variables

extern HINSTANCE store_hInstance;
extern HWND	mainPtr;
extern HDC main_dc;
extern HFONT font;

// local variables

Boolean give_delays = TRUE;

// function prototype

void Delay(short val /*,long *dummy */);
// void WinBlackDrawString(char *string,short x,short y);
// void DrawString(char *string,short x,short y,HDC hdc);
// void undo_clip();
// void ClipRect(RECT *rect);
// void force_reprint();
// void MacMoveTo(short x, short y);
// void MoveToDrawString(char *string,HDC hdc);
// void SetControlValue(HWND sbar,short pos);
// void mes_nums(short a, short b, short c, short d);
// void ShowControl(HWND scroll_bar);
// void HideControl(HWND scroll_bar);


short get_ran (short times, short min, short  max)
{
	short store;
	short i, to_ret = 0;

	if ((max - min + 1) == 0)
		return 0;
	for (i = 1; i < times + 1; i++) {
		store = (short)(rand() % (max - min + 1));
		to_ret = to_ret + min + store;
		}
	return to_ret;
}

/*
short s_pow(short x,short y)
{
	return (short) pow((double) x, (double) y);
}
*/

short dist(location p1,location p2)
{
	return (short) sqrt((double)((p1.x - p2.x) * (p1.x - p2.x) +
							(p1.y - p2.y) * (p1.y - p2.y)));
}

short smax(short a,short b)
{
	if (a > b)
		return a;
		else return b;
}

short smin(short a,short b)
{
	if (a < b)
		return a;
		else return b;
}

short minmax(short min,short max,short k)
{
	if (k < min)
		return min;
	if (k > max)
		return max;
	return k;
}

short n_abs(short x)
{
	if (x < 0)
		return x * -1;
	return x;
}

Boolean same_point(location l1,location l2)
{
	if ((l1.x == l2.x) && (l1.y == l2.y))
		return TRUE;
		else return FALSE;
}

void Delay(short val /*,long *dummy */)
{
	long then,now,wait_val;

	wait_val = (long) val;
	wait_val = wait_val * 16;
	then = (long)GetCurrentTime();
	now = then;
	while (now - then < wait_val)   {
		now = (long) GetCurrentTime();
		}
}

void pause(short length)
{
//	long dummy;
	
	if (give_delays == 0)
		Delay(length /*, &dummy */);
}

void beep()
{
//	long dummy;

	MessageBeep(MB_OK);
	Delay(50 /* ,&dummy */);

	}

void SysBeep(/* short a */)
{
	MessageBeep(MB_ICONQUESTION);

}

/*
void WinBlackDrawString(char *string,short x,short y)
{
	HDC hdc;

	hdc = GetDC(mainPtr);
	//SelectPalette(hdc,hpal,0);
 	//SetViewportOrg(hdc,ulx,uly);
	SelectObject(hdc,font);
	SetBkMode(hdc,TRANSPARENT);
	DrawString(string,x,y,hdc);
	fry_dc(mainPtr,hdc);
}
*/
/*
void DrawString(char *string,short x,short y,HDC hdc)
{
	RECT text_r = {0,0,450,20};

	OffsetRect(&text_r,x,y);
	DrawText(hdc,string,-1,&text_r,DT_LEFT | DT_SINGLELINE | DT_TOP | DT_NOCLIP);

}
*/
/*
void undo_clip()
{
 //	RECT overall_rect = {0,0,530,435};
	HRGN rgn;
	RECT r;
	
	GetClientRect(mainPtr,&r);
	//GetWindowRect(GetDesktopWindow(),&r);
	rgn = CreateRectRgn(r.left,r.top,r.right,r.bottom);
	SelectClipRgn(main_dc,rgn);
	DeleteObject(rgn);
}
*/
/*
void ClipRect(RECT *rect)
{
	HRGN rgn;
	RECT rect2;

	rect2 = *rect;
	//OffsetRect(&rect2,ulx,uly);

	rgn = CreateRectRgn(rect2.left,rect2.top,rect2.right,rect2.bottom);
	SelectClipRgn(main_dc,rgn);
	DeleteObject(rgn);
}
*/
/*
void StringToNum(char *the_string,long *number_given) {
	long store_short;
	sscanf(the_string,"%ld",&store_short);
	*number_given = (long) store_short;
}
*/

void MacInsetRect(RECT *rect,short x, short y) 
{
	InflateRect(rect,-1 * x, -1 * y);
}

// Note ... this expects a str len of at most 256 and
// len_array pointing to a 256 long array of shorts
/*
void MeasureText(short str_len,char *str, short *len_array,HDC hdc)
{
	short text_len[257];
	short i;
	char p_str[257];
	char *store_array;
	short *store2;
	SIZE s;
	
	store_array = (char *) len_array;
	//text_len = len_array;
	for (i = 0; i < 256; i++)
		text_len[i] = 0;
	for (i = 1; i < str_len; i++) {
		strncpy(p_str,str,i);
		p_str[i] = 0;
		GetTextExtentPoint32(hdc,p_str,i,&s);
		text_len[i] = s.cx;
		}
	for (i = 0; i < 256; i++) {
		store2 = (short *) store_array;
		*store2 = text_len[i];
		store_array += 2;
		}
}
*/

// kludgy annoyance
/*
void MacMoveTo(short x, short y)
{//###
	//store_text_x = x;
	//store_text_y = y - 16;
}
*/
/*
void MoveToDrawString(char *string,HDC hdc)
{
	//DrawString(string,store_text_x,store_text_y, hdc);
}
*/

void PaintRect(RECT *rect,HDC hdc,short r, short g, short b)
{
	HBRUSH old_brush,new_brush;
	HPEN hpen,old_pen;
	
	hpen = CreatePen(PS_SOLID,1,RGB(r,g,b));
	old_pen = (HPEN) SelectObject(hdc,hpen);	
	new_brush = CreateSolidBrush(RGB(r,g,b));
	old_brush = (HBRUSH) SelectObject(hdc,new_brush);
	Rectangle(hdc,rect->left,rect->top,rect->right,rect->bottom);
	SelectObject(hdc,old_brush);
	SelectObject(hdc,old_pen);
	DeleteObject(new_brush);
	DeleteObject(hpen);
}

/*
void SetControlValue(HWND sbar,short pos)
{
	SetScrollPos(sbar,SB_CTL,pos,TRUE);
}
*/

short GetControlValue(HWND sbar)
{
	return (short) GetScrollPos(sbar,SB_CTL);
}

/*
void mes_nums(short a, short b, short c, short d)
{
	char s[100];
	
	sprintf(s,"Debug: %d %d %d %d",(int)a,(int)b,(int)c,(int)d);
	MessageBox(NULL,s,"Hey, baby!",MB_OK);
}
*/

void mes_box(char *str)
{
	MessageBox(NULL,str,"Blades of Avernum Error",MB_OK);
}

/*
void ShowControl(HWND scroll_bar)
{
	ShowScrollBar(scroll_bar,SB_CTL,TRUE);
}
*/
/*
void HideControl(HWND scroll_bar)
{
	ShowScrollBar(scroll_bar,SB_CTL,FALSE);
}
*/

void GetIndString(char *str,short i, short j)
{
   UINT resnum = 0,len;
   short k;

   // extra stupid kludge here
   resnum = i * 300 + j;
   len = LoadString(store_hInstance,resnum,str,256);
   if (len == 0) {
       sprintf(str,"");
       return;
       }
   for (k = 0; k < 256; k++)  {
       if (str[k] == '|')
           str[k] = 13;
       if (str[k] == '_')
           str[k] = 34;
       }
}
/*
Boolean point_in_rect(POINT the_point,RECT *world_screen)
{
	return (Boolean)PtInRect(world_screen,the_point);
}
*/
