/*-------------------------------------
   DIBHELP.H header file for DIBHELP.C
  -------------------------------------*/
#pragma once

     // Functions in DIBHELP.C

HBITMAP DibBitmapHandle (HDIB hdib) ;
int DibWidth (HDIB hdib) ;
int DibHeight (HDIB hdib) ;
int DibBitCount (HDIB hdib) ;
BYTE * DibPixelPtr (HDIB hdib, int x, int y) ;
BOOL DibDelete (HDIB hdib) ;
HDIB DibCreate (int cx, int cy, int cBits, int cColors) ;
HDIB DibFileLoad (const TCHAR * szFileName) ;

