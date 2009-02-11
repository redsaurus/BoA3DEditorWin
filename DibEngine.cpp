/*-------------------------------------------------------
   DIBCONV.C -- Converts DIBs from one format to another
                (c) Charles Petzold, 1998
  -------------------------------------------------------*/

#include "stdafx.h"
/*
#include <windows.h>
#include "stdio.h"
*/
#include "Global.h"
#include "DibEngine.h"

#define HDIB_SIGNATURE (* (int *) "Dib ")

/*-----------------------------------------------
   Quickie no-bounds-checked pixel gets and sets
  -----------------------------------------------*/

#define DibPixelPtr1(hdib, x, y)  (((* (PBYTE **) hdib) [y]) + ((x) >> 3))
#define DibPixelPtr4(hdib, x, y)  (((* (PBYTE **) hdib) [y]) + ((x) >> 1))
#define DibPixelPtr8(hdib, x, y)  (((* (PBYTE **) hdib) [y]) +  (x)      )
#define DibPixelPtr16(hdib, x, y)  \
                        ((WORD *) (((* (PBYTE **) hdib) [y]) +  (x) *  2))

#define DibPixelPtr24(hdib, x, y)  \
                   ((RGBTRIPLE *) (((* (PBYTE **) hdib) [y]) +  (x) *  3))

#define DibPixelPtr32(hdib, x, y)  \
                       ((DWORD *) (((* (PBYTE **) hdib) [y]) +  (x) *  4))

#define DibGetPixel1(hdib, x, y)   \
               (0x01 & (* DibPixelPtr1 (hdib, x, y) >> (7 - ((x) & 7))))

#define DibGetPixel4(hdib, x, y)   \
               (0x0F & (* DibPixelPtr4 (hdib, x, y) >> ((x) & 1 ? 0 : 4)))

#define DibGetPixel8(hdib, x, y)     (* DibPixelPtr8  (hdib, x, y))
#define DibGetPixel16(hdib, x, y)    (* DibPixelPtr16 (hdib, x, y))
#define DibGetPixel24(hdib, x, y)    (* DibPixelPtr24 (hdib, x, y))
#define DibGetPixel32(hdib, x, y)    (* DibPixelPtr32(hdib, x, y))

#define DibSetPixel1(hdib, x, y, p)                                        \
          ((* DibPixelPtr1 (hdib, x, y) &= ~( 1  << (7 - ((x) & 7)))),     \
           (* DibPixelPtr1 (hdib, x, y) |=  ((p) << (7 - ((x) & 7)))))

#define DibSetPixel4(hdib, x, y, p)                                        \
          ((* DibPixelPtr4 (hdib, x, y) &= (0x0F << ((x) & 1 ? 4 : 0))),   \
           (* DibPixelPtr4 (hdib, x, y) |= ((p)  << ((x) & 1 ? 0 : 4))))

#define DibSetPixel8(hdib, x, y, p)  (* DibPixelPtr8 (hdib, x, y) = p)
#define DibSetPixel16(hdib, x, y, p) (* DibPixelPtr16 (hdib, x, y) = p)
#define DibSetPixel24(hdib, x, y, p) (* DibPixelPtr24 (hdib, x, y) = p)
#define DibSetPixel32(hdib, x, y, p) (* DibPixelPtr32 (hdib, x, y) = p)


// function prototype

// HDIB DibConvert (HDIB hdibSrc, int iBitCountDst) ;

BOOL DibIsValid (HDIB hdib) ;
int DibRowLength (HDIB hdib) ;
// int DibNumColors (HDIB hdib) ;
// DWORD DibMask (HDIB hdib, int i) ;
// int DibRShift (HDIB hdib, int i) ;
// int DibLShift (HDIB hdib, int i) ;
int DibCompression (HDIB hdib) ;
BOOL DibIsAddressable (HDIB hdib) ;
// DWORD DibInfoHeaderSize (HDIB hdib) ;
// DWORD DibMaskSize (HDIB hdib) ;
// DWORD DibColorSize (HDIB hdib) ;
// DWORD DibInfoSize (HDIB hdib) ;
// DWORD DibBitsSize (HDIB hdib) ;
// DWORD DibTotalSize (HDIB hdib) ;
// BITMAPINFOHEADER * DibInfoHeaderPtr (HDIB hdib) ;
// DWORD * DibMaskPtr (HDIB hdib) ;
// void * DibBitsPtr (HDIB hdib) ;
// BOOL DibGetColor (HDIB hdib, int index, RGBQUAD * prgb) ;
// BOOL DibSetColor (HDIB hdib, int index, RGBQUAD * prgb) ;
// DWORD DibGetPixel (HDIB hdib, int x, int y) ;
// BOOL DibSetPixel (HDIB hdib, int x, int y, DWORD dwPixel) ;
// BOOL DibGetPixelColor (HDIB hdib, int x, int y, RGBQUAD * prgb) ;
// BOOL DibSetPixelColor (HDIB hdib, int x, int y, RGBQUAD * prgb) ;
HDIB DibCreateFromInfo (const BITMAPINFO * pbmi);
// HDIB DibCopy (HDIB hdibSrc, BOOL fRotate) ;
// BITMAPINFO * DibCopyToPackedDib (HDIB hdib, BOOL fUseGlobal) ;
// HDIB DibCopyFromPackedDib (BITMAPINFO * pPackedDib) ;
// BOOL DibFileSave (HDIB hdib, const TCHAR * szFileName) ;
// HBITMAP DibCopyToDdb (HDIB hdib, HWND hwnd, HPALETTE hPalette) ;
// HDIB DibCreateFromDdb (HBITMAP hBitmap) ;

/*-----------------------------------
   DIBPAL.H header file for DIBPAL.C
  -----------------------------------*/

// HPALETTE DibPalDibTable (HDIB hdib) ;
// HPALETTE DibPalAllPurpose (void) ;
// HPALETTE DibPalUniformGrays (int iNum) ;
// HPALETTE DibPalUniformColors (int iNumR, int iNumG, int iNumB) ;
// HPALETTE DibPalVga (void) ;
// HPALETTE DibPalPopularity (HDIB hdib, int iRes) ;
// HPALETTE DibPalMedianCut (HDIB hdib, int iRes) ;



/*
HDIB DibConvert (HDIB hdibSrc, int iBitCountDst)
{
     HDIB         hdibDst ;
     HPALETTE     hPalette ;
     int          i, x, y, cx, cy, iBitCountSrc, cColors ;
     PALETTEENTRY pe ;
     RGBQUAD      rgb ;
     WORD         wNumEntries ;

     cx = DibWidth (hdibSrc) ;
     cy = DibHeight (hdibSrc) ;
     iBitCountSrc = DibBitCount (hdibSrc) ;

     if (iBitCountSrc == iBitCountDst)
          return NULL ;

          // DIB with color table to DIB with larger color table:

     if ((iBitCountSrc < iBitCountDst) && (iBitCountDst <= 8))
     {
          cColors = DibNumColors (hdibSrc) ;
          hdibDst = DibCreate (cx, cy, iBitCountDst, cColors) ;

          for (i = 0 ; i < cColors ; i++)
          {
               DibGetColor (hdibSrc, i, &rgb) ;
               DibSetColor (hdibDst, i, &rgb) ;
          }

          for (x = 0 ; x < cx ; x++)
          for (y = 0 ; y < cy ; y++)
          {
               DibSetPixel (hdibDst, x, y, DibGetPixel (hdibSrc, x, y)) ;
          }
     }
          // Any DIB to DIB with no color table

     else if (iBitCountDst >= 16)
     {
          hdibDst = DibCreate (cx, cy, iBitCountDst, 0) ;

          for (x = 0 ; x < cx ; x++)
          for (y = 0 ; y < cy ; y++)
          {
               DibGetPixelColor (hdibSrc, x, y, &rgb) ;
               DibSetPixelColor (hdibDst, x, y, &rgb) ;
          }
     }
          // DIB with no color table to 8-bit DIB

     else if (iBitCountSrc >= 16 && iBitCountDst == 8)
     {
          hPalette = DibPalMedianCut (hdibSrc, 6) ;

          GetObject (hPalette, sizeof (WORD), &wNumEntries) ;

          hdibDst = DibCreate (cx, cy, 8, wNumEntries) ;
          
          for (i = 0 ; i < (int) wNumEntries ; i++)
          {
               GetPaletteEntries (hPalette, i, 1, &pe) ;

               rgb.rgbRed   = pe.peRed ;
               rgb.rgbGreen = pe.peGreen ;
               rgb.rgbBlue  = pe.peBlue ;
               rgb.rgbReserved = 0 ;

               DibSetColor (hdibDst, i, &rgb) ;
          }

          for (x = 0 ; x < cx ; x++)
          for (y = 0 ; y < cy ; y++)
          {
               DibGetPixelColor (hdibSrc, x, y, &rgb) ;

               DibSetPixel (hdibDst, x, y,
                    GetNearestPaletteIndex (hPalette, 
                         RGB (rgb.rgbRed, rgb.rgbGreen, rgb.rgbBlue))) ;
          }
          DeleteObject (hPalette) ;
     }
          // Any DIB to monochrome DIB

     else if (iBitCountDst == 1)
     {
          hdibDst = DibCreate (cx, cy, 1, 0) ;
          hPalette = DibPalUniformGrays (2) ;

          for (i = 0 ; i < 2 ; i++)
          {
               GetPaletteEntries (hPalette, i, 1, &pe) ;

               rgb.rgbRed   = pe.peRed ;
               rgb.rgbGreen = pe.peGreen ;
               rgb.rgbBlue  = pe.peBlue ;
               rgb.rgbReserved = 0 ;

               DibSetColor (hdibDst, i, &rgb) ;
          }

          for (x = 0 ; x < cx ; x++)
          for (y = 0 ; y < cy ; y++)
          {
               DibGetPixelColor (hdibSrc, x, y, &rgb) ;

               DibSetPixel (hdibDst, x, y,
                    GetNearestPaletteIndex (hPalette, 
                         RGB (rgb.rgbRed, rgb.rgbGreen, rgb.rgbBlue))) ;
          }
          DeleteObject (hPalette) ;
     }
          // All non-monochrome DIBs to 4-bit DIB

     else if (iBitCountSrc >= 8 && iBitCountDst == 4)
     {
          hdibDst = DibCreate (cx, cy, 4, 0) ;
          hPalette = DibPalVga () ;

          for (i = 0 ; i < 16 ; i++)
          {
               GetPaletteEntries (hPalette, i, 1, &pe) ;

               rgb.rgbRed   = pe.peRed ;
               rgb.rgbGreen = pe.peGreen ;
               rgb.rgbBlue  = pe.peBlue ;
               rgb.rgbReserved = 0 ;

               DibSetColor (hdibDst, i, &rgb) ;
          }

          for (x = 0 ; x < cx ; x++)
          for (y = 0 ; y < cy ; y++)
          {
               DibGetPixelColor (hdibSrc, x, y, &rgb) ;

               DibSetPixel (hdibDst, x, y,
                    GetNearestPaletteIndex (hPalette, 
                         RGB (rgb.rgbRed, rgb.rgbGreen, rgb.rgbBlue))) ;
          }
          DeleteObject (hPalette) ;
     }
          // Should not be necessary

     else
          hdibDst = NULL ;

     return hdibDst ;
}
*/

/*------------------------------------------
   DIBHELP.C -- DIB Section Helper Routines 
                (c) Charles Petzold, 1998
  ------------------------------------------*/




/*---------------------------------------------------------------
   DibIsValid:  Returns TRUE if hdib points to a valid DIBSTRUCT
  ---------------------------------------------------------------*/

BOOL DibIsValid (HDIB hdib)
{
     PDIBSTRUCT pdib = (PDIBSTRUCT) hdib ;

     if (pdib == NULL)
          return FALSE ;

     if (IsBadReadPtr (pdib, sizeof (DIBSTRUCT)))
          return FALSE ;

     if (pdib->iSignature != HDIB_SIGNATURE)
          return FALSE ;

     return TRUE ;
}

/*-----------------------------------------------------------------------
   DibBitmapHandle:  Returns the handle to the DIB section bitmap object
  -----------------------------------------------------------------------*/

HBITMAP DibBitmapHandle (HDIB hdib)
{
     if (!DibIsValid (hdib))
          return NULL ;
     
     return ((PDIBSTRUCT) hdib)->hBitmap ;
}

/*-------------------------------------------
   DibWidth:  Returns the bitmap pixel width
  -------------------------------------------*/

int DibWidth (HDIB hdib)
{
     if (!DibIsValid (hdib))
          return 0 ;
     
     return ((PDIBSTRUCT) hdib)->ds.dsBm.bmWidth ;
}

/*---------------------------------------------
   DibHeight:  Returns the bitmap pixel height
  ---------------------------------------------*/

int DibHeight (HDIB hdib)
{
     if (!DibIsValid (hdib))
          return 0 ; 
     
     return ((PDIBSTRUCT) hdib)->ds.dsBm.bmHeight ;
}

/*----------------------------------------------------
   DibBitCount:  Returns the number of bits per pixel
  ----------------------------------------------------*/

int DibBitCount (HDIB hdib)
{
//     if (!DibIsValid (hdib))
//          return 0 ;
     
     return ((PDIBSTRUCT) hdib)->ds.dsBm.bmBitsPixel ;
}

/*--------------------------------------------------------------
   DibRowLength:  Returns the number of bytes per row of pixels
  --------------------------------------------------------------*/

int DibRowLength (HDIB hdib)
{
//     if (!DibIsValid (hdib))
//          return 0 ;
     
     return 4 * ((DibWidth (hdib) * DibBitCount (hdib) + 31) / 32) ;
}

/*----------------------------------------------------------------
   DibNumColors:  Returns the number of colors in the color table
  ----------------------------------------------------------------*/
/*
int DibNumColors (HDIB hdib)
{
     PDIBSTRUCT pdib = (PDIBSTRUCT) hdib ;
//     if (!DibIsValid (hdib))
//          return 0 ;

     if (pdib->ds.dsBmih.biClrUsed != 0)
     {
          return pdib->ds.dsBmih.biClrUsed ;
     }
     else if (DibBitCount (hdib) <= 8)
     {
          return 1 << DibBitCount (hdib) ;
     }
     return 0 ;
}
*/
/*------------------------------------------
   DibMask:  Returns one of the color masks
  ------------------------------------------*/
/*
DWORD DibMask (HDIB hdib, int i)
{
     PDIBSTRUCT pdib = (PDIBSTRUCT) hdib ;
     if (!DibIsValid (hdib) || i < 0 || i > 2)
          return 0 ;
     
     return pdib->ds.dsBitfields[i] ;
}
*/
/*---------------------------------------------------
   DibRShift:  Returns one of the right-shift values
  ---------------------------------------------------*/
/*
int DibRShift (HDIB hdib, int i)
{
     PDIBSTRUCT pdib = (PDIBSTRUCT) hdib ;
     if (!DibIsValid (hdib) || i < 0 || i > 2)
          return 0 ;
     
     return pdib->iRShift[i] ;
}
*/
/*--------------------------------------------------
   DibLShift:  Returns one of the left-shift values
  --------------------------------------------------*/
/*
int DibLShift (HDIB hdib, int i)
{
     PDIBSTRUCT pdib = (PDIBSTRUCT) hdib ;
     if (!DibIsValid (hdib) || i < 0 || i > 2)
          return 0 ;
     
     return pdib->iLShift[i] ;
}
*/
/*---------------------------------------------------------------
   DibCompression:  Returns the value of the biCompression field
  ---------------------------------------------------------------*/

int DibCompression (HDIB hdib)
{
//     if (!DibIsValid (hdib))
//          return 0 ;

     return ((PDIBSTRUCT) hdib)->ds.dsBmih.biCompression ;
}

/*--------------------------------------------------------------
   DibIsAddressable:  Returns TRUE if the DIB is not compressed
  --------------------------------------------------------------*/

BOOL DibIsAddressable (HDIB hdib)
{
     int iCompression ;

     if (!DibIsValid (hdib))
          return FALSE ;

     iCompression = DibCompression (hdib) ;

     if (iCompression == BI_RGB || iCompression == BI_BITFIELDS)
         return TRUE ;

     return FALSE ;
}

/*---------------------------------------------------------------------------
   These functions return the sizes of various components of the DIB section
     AS THEY WOULD APPEAR in a packed DIB. These functions aid in converting
     the DIB section to a packed DIB and in saving DIB files.
  ---------------------------------------------------------------------------*/
/*
DWORD DibInfoHeaderSize (HDIB hdib)
{
//     if (!DibIsValid (hdib))
//          return 0 ;

     return ((PDIBSTRUCT) hdib)->ds.dsBmih.biSize ;
}
*/
/*
DWORD DibMaskSize (HDIB hdib)
{
     PDIBSTRUCT pdib = (PDIBSTRUCT) hdib ;
//     if (!DibIsValid (hdib))
//          return 0 ;

     if (pdib->ds.dsBmih.biCompression == BI_BITFIELDS)
          return 3 * sizeof (DWORD) ;

     return 0 ;
}
*/
/*
DWORD DibColorSize (HDIB hdib)
{
     return DibNumColors (hdib) * sizeof (RGBQUAD) ;
} 
*/
/*
DWORD DibInfoSize (HDIB hdib)
{
     return DibInfoHeaderSize(hdib) + DibMaskSize(hdib) + DibColorSize(hdib) ;
}
*/
/*
DWORD DibBitsSize (HDIB hdib)
{
     PDIBSTRUCT pdib = (PDIBSTRUCT) hdib ;
//     if (!DibIsValid (hdib))
//          return 0 ;

     if (pdib->ds.dsBmih.biSizeImage != 0)
     {
          return pdib->ds.dsBmih.biSizeImage ;
     }
     return DibHeight (hdib) * DibRowLength (hdib) ;
}
*/
/*
DWORD DibTotalSize (HDIB hdib)
{
     return DibInfoSize (hdib) + DibBitsSize (hdib) ;
}
*/
/*----------------------------------------------------------------------
   These functions return pointers to the various components of the DIB 
     section.
  ----------------------------------------------------------------------*/
/*
BITMAPINFOHEADER * DibInfoHeaderPtr (HDIB hdib)
{
//     if (!DibIsValid (hdib))
//          return NULL ;
     
     return & (((PDIBSTRUCT) hdib)->ds.dsBmih) ;
}
*/
/*
DWORD * DibMaskPtr (HDIB hdib)
{
     PDIBSTRUCT pdib = (PDIBSTRUCT) hdib ;
//     if (!DibIsValid (hdib))
//          return 0 ;

     return pdib->ds.dsBitfields ;
}
*/
/*
void * DibBitsPtr (HDIB hdib)
{
//     if (!DibIsValid (hdib))
//          return NULL ;
     
     return ((PDIBSTRUCT) hdib)->pBits ;
}
*/
/*------------------------------------------------------
   DibSetColor:  Obtains entry from the DIB color table
  ------------------------------------------------------*/
/*
BOOL DibGetColor (HDIB hdib, int index, RGBQUAD * prgb)
{
     PDIBSTRUCT pdib = (PDIBSTRUCT) hdib ;     HDC        hdcMem ;
     int        iReturn ;

//     if (!DibIsValid (hdib))
//          return 0 ;

     hdcMem = CreateCompatibleDC (NULL) ;
     SelectObject (hdcMem, pdib->hBitmap) ;
     iReturn = GetDIBColorTable (hdcMem, index, 1, prgb) ;
     DeleteDC (hdcMem) ;

     return iReturn ? TRUE : FALSE ;
}
*/
/*----------------------------------------------------
   DibGetColor:  Sets an entry in the DIB color table
  ----------------------------------------------------*/
/*
BOOL DibSetColor (HDIB hdib, int index, RGBQUAD * prgb)
{
     PDIBSTRUCT pdib = (PDIBSTRUCT) hdib ;     HDC        hdcMem ;
     int        iReturn ;

//     if (!DibIsValid (hdib))
//          return 0 ;

     hdcMem = CreateCompatibleDC (NULL) ;
     SelectObject (hdcMem, pdib->hBitmap) ;
     iReturn = SetDIBColorTable (hdcMem, index, 1, prgb) ;
     DeleteDC (hdcMem) ;

     return iReturn ? TRUE : FALSE ;
}
*/
/*-----------------------------------------------------------------
   DibPixelPtr:  Returns a pointer to the pixel at position (x, y)
  -----------------------------------------------------------------*/

BYTE * DibPixelPtr (HDIB hdib, int x, int y)
{
     if (!DibIsAddressable (hdib))
          return NULL ;

     if (x < 0 || x >= DibWidth (hdib) || y < 0 || y >= DibHeight (hdib))
          return NULL ;

     return (((PDIBSTRUCT) hdib)->ppRow)[y] + (x * DibBitCount (hdib) >> 3) ;
}

/*-----------------------------------------------
   DibGetPixel:  Obtains a pixel value at (x, y)
  -----------------------------------------------*/
/*
DWORD DibGetPixel (HDIB hdib, int x, int y)
{
     PBYTE pPixel ;

     if (!(pPixel = DibPixelPtr (hdib, x, y)))
          return 0 ;

     switch (DibBitCount (hdib))
     {
     case  1:  return 0x01 & (* pPixel >> (7 - (x & 7))) ;
     case  4:  return 0x0F & (* pPixel >> (x & 1 ? 0 : 4)) ;
     case  8:  return * pPixel ;
     case 16:  return * (WORD *) pPixel ;
     case 24:  return 0x00FFFFFF & * (DWORD *) pPixel ; 
     case 32:  return * (DWORD *) pPixel ;
     }
     return 0 ;
}
*/
/*--------------------------------------------
   DibSetPixel:  Sets a pixel value at (x, y)
  --------------------------------------------*/
/*
BOOL DibSetPixel (HDIB hdib, int x, int y, DWORD dwPixel)
{
     PBYTE pPixel ;

     if (!(pPixel = DibPixelPtr (hdib, x, y)))
          return FALSE ;

     switch (DibBitCount (hdib))
     {
     case  1:  * pPixel &= ~(1     << (7 - (x & 7))) ;
               * pPixel |= dwPixel << (7 - (x & 7)) ;
               break ;

     case  4:  * pPixel &= 0x0F    << (x & 1 ? 4 : 0) ;
               * pPixel |= dwPixel << (x & 1 ? 0 : 4) ;
               break ;

     case  8:  * pPixel = (BYTE) dwPixel ;
               break ;

     case 16:  * (WORD *) pPixel = (WORD) dwPixel ;
               break ;

     case 24:  * (RGBTRIPLE *) pPixel = * (RGBTRIPLE *) &dwPixel ; 
               break ;

     case 32:  * (DWORD *) pPixel = dwPixel ;
               break ;
     default:
          return FALSE ;
     }
     return TRUE ;
}
*/
/*------------------------------------------------------
   DibGetPixelColor:  Obtains the pixel color at (x, y)
  ------------------------------------------------------*/
/*
BOOL DibGetPixelColor (HDIB hdib, int x, int y, RGBQUAD * prgb)
{
     DWORD      dwPixel ;
     int        iBitCount ;
     PDIBSTRUCT pdib = (PDIBSTRUCT) hdib ;
          // Get bit count; also use this as a validity check

     if (0 == (iBitCount = DibBitCount (hdib)))
          return FALSE ;

          // Get the pixel value

     dwPixel = DibGetPixel (hdib, x, y) ;

          // If the bit-count is 8 or less, index the color table

     if (iBitCount <= 8)
          return DibGetColor (hdib, (int) dwPixel, prgb) ;

          // If the bit-count is 24, just use the pixel

     else if (iBitCount == 24)
     {
          * (RGBTRIPLE *) prgb = * (RGBTRIPLE *) & dwPixel ;
          prgb->rgbReserved = 0 ;
     }

          // If the bit-count is 32 and the biCompression field is BI_RGB,
          //   just use the pixel

     else if (iBitCount == 32 && 
               pdib->ds.dsBmih.biCompression == BI_RGB)
     {
          * prgb = * (RGBQUAD *) & dwPixel ;
     }

          // Otherwise, use the mask and shift values
          //   (for best performance, don't use DibMask and DibShift functions)
     else
     {
          prgb->rgbRed   = (BYTE) (((pdib->ds.dsBitfields[0] & dwPixel) 
                                   >> pdib->iRShift[0]) << pdib->iLShift[0]) ;

          prgb->rgbGreen = (BYTE) (((pdib->ds.dsBitfields[1] & dwPixel) 
                                   >> pdib->iRShift[1]) << pdib->iLShift[1]) ;

          prgb->rgbBlue  = (BYTE) (((pdib->ds.dsBitfields[2] & dwPixel) 
                                   >> pdib->iRShift[2]) << pdib->iLShift[2]) ;
     }
     return TRUE ;
}
*/
/*---------------------------------------------------
   DibSetPixelColor:  Sets the pixel color at (x, y) 
  ---------------------------------------------------*/
/*
BOOL DibSetPixelColor (HDIB hdib, int x, int y, RGBQUAD * prgb)
{
     DWORD      dwPixel ;
     int        iBitCount ;
     PDIBSTRUCT pdib = (PDIBSTRUCT) hdib ;
          // Don't do this function for DIBs with color tables

     iBitCount = DibBitCount (hdib) ;

     if (iBitCount <= 8)
          return FALSE ;

          // The rest is just the opposite of DibGetPixelColor

     else if (iBitCount == 24)
     {
          * (RGBTRIPLE *) & dwPixel = * (RGBTRIPLE *) prgb ;
          dwPixel &= 0x00FFFFFF ;
     }

     else if (iBitCount == 32 &&
               pdib->ds.dsBmih.biCompression == BI_RGB)
     {
          * (RGBQUAD *) & dwPixel = * prgb ;
     }

     else
     {
          dwPixel  = (((DWORD) prgb->rgbRed >> pdib->iLShift[0]) 
                         << pdib->iRShift[0]) ; 

          dwPixel |= (((DWORD) prgb->rgbGreen >> pdib->iLShift[1])
                         << pdib->iRShift[1]) ; 

          dwPixel |= (((DWORD) prgb->rgbBlue >> pdib->iLShift[2])
                         << pdib->iRShift[2]) ; 
     }

     DibSetPixel (hdib, x, y, dwPixel) ;
     return TRUE ;
}
*/
/*--------------------------------------------------------------
   Calculating shift values from color masks is required by the 
     DibCreateFromInfo function.
  --------------------------------------------------------------*/

static int MaskToRShift (DWORD dwMask)
{
     int iShift ;

     if (dwMask == 0)
          return 0 ;

     for (iShift = 0 ; !(dwMask & 1) ; iShift++)
          dwMask >>= 1 ;

     return iShift ;
}

static int MaskToLShift (DWORD dwMask)
{
     int iShift ;

     if (dwMask == 0)
          return 0 ;

     while (!(dwMask & 1))
          dwMask >>= 1 ;

     for (iShift = 0 ; dwMask & 1 ; iShift++)
          dwMask >>= 1 ;

     return 8 - iShift ;
}

/*-------------------------------------------------------------------------
   DibCreateFromInfo: All DIB creation functions ultimately call this one.
     This function is responsible for calling CreateDIBSection, allocating
     memory for DIBSTRUCT, and setting up the row pointer.
  -------------------------------------------------------------------------*/

HDIB DibCreateFromInfo (const BITMAPINFO * pbmi)
{
     BYTE      * pBits ;
     DIBSTRUCT * pdib ;
     HBITMAP     hBitmap ;
     int         i, iRowLength, cy, y ;
     HDC	wank = NULL;
     
     hBitmap = CreateDIBSection (wank, pbmi, (unsigned int) ( DIB_RGB_COLORS), (void **) ( &pBits), (void *) (NULL), (unsigned long) (0)) ;
     //hBitmap = CreateDIBSection (NULL, pbmi, DIB_RGB_COLORS, &pBits, NULL, 0) ;

     if (hBitmap == NULL)
          return NULL ;

     if (NULL == (pdib = (PDIBSTRUCT) malloc (sizeof (DIBSTRUCT))))
     {
          DeleteObject (hBitmap) ;
          return NULL ;
     }

     pdib->iSignature = HDIB_SIGNATURE ;
     pdib->hBitmap    = hBitmap ;
     pdib->pBits      = pBits ;

     GetObject (hBitmap, sizeof (DIBSECTION), &pdib->ds) ;

          // Notice that we can now use the DIB information functions 
          //   defined above.

          // If the compression is BI_BITFIELDS, calculate shifts from masks

     if (DibCompression (pdib) == BI_BITFIELDS)
     {
          for (i = 0 ; i < 3 ; i++)
          {
               pdib->iLShift[i] = MaskToLShift (pdib->ds.dsBitfields[i]) ;
               pdib->iRShift[i] = MaskToRShift (pdib->ds.dsBitfields[i]) ;
          }
     }

          // If the compression is BI_RGB, but bit-count is 16 or 32,
          //   set the bitfields and the masks

     else if (DibCompression (pdib) == BI_RGB)
     {
          if (DibBitCount (pdib) == 16)
          {
               pdib->ds.dsBitfields[0] = 0x00007C00 ;
               pdib->ds.dsBitfields[1] = 0x000003E0 ;
               pdib->ds.dsBitfields[2] = 0x0000001F ;

               pdib->iRShift[0] = 10 ;
               pdib->iRShift[1] =  5 ;
               pdib->iRShift[2] =  0 ; 

               pdib->iLShift[0] =  3 ;
               pdib->iLShift[1] =  3 ;
               pdib->iLShift[2] =  3 ;
          }
          else if (DibBitCount (pdib) == 24 || DibBitCount (pdib) == 32)
          {
               pdib->ds.dsBitfields[0] = 0x00FF0000 ;
               pdib->ds.dsBitfields[1] = 0x0000FF00 ;
               pdib->ds.dsBitfields[2] = 0x000000FF ;

               pdib->iRShift[0] = 16 ;
               pdib->iRShift[1] =  8 ;
               pdib->iRShift[2] =  0 ; 

               pdib->iLShift[0] =  0 ;
               pdib->iLShift[1] =  0 ;
               pdib->iLShift[2] =  0 ;
          }
     }
          // Allocate an array of pointers to each row in the DIB

     cy = DibHeight (pdib) ;

     if (NULL == (pdib->ppRow = (PBYTE *) malloc (cy * sizeof (BYTE *))))
     {
          free (pdib) ;
          DeleteObject (hBitmap) ;
          return NULL ;
     }

          // Initialize them.

     iRowLength = DibRowLength (pdib) ; 

     if (pbmi->bmiHeader.biHeight > 0)       // ie, bottom up
     {
          for (y = 0 ; y < cy ; y++)
               pdib->ppRow[y] = pBits + (cy - y - 1) * iRowLength ;
     }
     else                                    // top down
     {
          for (y = 0 ; y < cy ; y++)
               pdib->ppRow[y] = pBits + y * iRowLength ;
     }
     return pdib ;
}

/*--------------------------------------------------
   DibDelete:  Frees all memory for the DIB section
  --------------------------------------------------*/

BOOL DibDelete (HDIB hdib)
{
     DIBSTRUCT * pdib = (DIBSTRUCT *) hdib ;

     if (!DibIsValid (hdib))
          return FALSE ;
	
     free (pdib->ppRow) ;
     DeleteObject (pdib->hBitmap) ;
     free (pdib) ;
     return TRUE ;
}

/*----------------------------------------------------
   DibCreate: Creates an HDIB from explicit arguments
  ----------------------------------------------------*/ 

HDIB DibCreate (int cx, int cy, int cBits, int cColors)
{
     BITMAPINFO * pbmi ;
     DWORD        dwInfoSize ;
     HDIB         hDib ;
     int          cEntries = 1;

     if (cx <= 0 || cy <= 0 || 
         ((cBits !=  1) && (cBits !=  4) && (cBits !=  8) && 
          (cBits != 16) && (cBits != 24) && (cBits != 32)))
     {
          return NULL ;
     }

     if (cColors != 0)
          cEntries = cColors ;
     else if (cBits <= 8)
          cEntries = 1 << cBits ;

     dwInfoSize = sizeof (BITMAPINFOHEADER) + (cEntries - 1) * sizeof (RGBQUAD);

     if (NULL == (pbmi = (BITMAPINFO *) malloc (dwInfoSize)))
     {
          return NULL ;
     }

     ZeroMemory (pbmi, dwInfoSize) ;

     pbmi->bmiHeader.biSize          = sizeof (BITMAPINFOHEADER) ;
     pbmi->bmiHeader.biWidth         = cx ;
     pbmi->bmiHeader.biHeight        = cy ;
     pbmi->bmiHeader.biPlanes        = 1 ;
     pbmi->bmiHeader.biBitCount      = (WORD)cBits ;
     pbmi->bmiHeader.biCompression   = BI_RGB ;
     pbmi->bmiHeader.biSizeImage     = 0 ;
     pbmi->bmiHeader.biXPelsPerMeter = 0 ;
     pbmi->bmiHeader.biYPelsPerMeter = 0 ;
     pbmi->bmiHeader.biClrUsed       = cColors ;
     pbmi->bmiHeader.biClrImportant  = 0 ;

     hDib = DibCreateFromInfo (pbmi) ;
     free (pbmi) ;

     return hDib ;
}

/*--------------------------------------------------
   DibCopyToInfo:  Builds BITMAPINFO structure.
                   Used by DibCopy and DibCopyToDdb
  --------------------------------------------------*/
/*
static BITMAPINFO * DibCopyToInfo (HDIB hdib)
{
     BITMAPINFO * pbmi ;
     int          i, iNumColors ;
     RGBQUAD    * prgb ;

//     if (!DibIsValid (hdib))
//          return NULL ;

          // Allocate the memory

     if (NULL == (pbmi = (BITMAPINFO *) malloc (DibInfoSize (hdib))))
          return NULL ;

          // Copy the information header

     CopyMemory (pbmi, DibInfoHeaderPtr (hdib), 
                                        sizeof (BITMAPINFOHEADER));
          
          // Copy the possible color masks

     prgb = (RGBQUAD *) ((BYTE *) pbmi + sizeof (BITMAPINFOHEADER)) ;

     if (DibMaskSize (hdib))
     {
          CopyMemory (prgb, DibMaskPtr (hdib), 3 * sizeof (DWORD)) ;

          prgb = (RGBQUAD *) ((BYTE *) prgb + 3 * sizeof (DWORD)) ;
     }
          // Copy the color table

     iNumColors = DibNumColors (hdib) ;

     for (i = 0 ; i < iNumColors ; i++)
          DibGetColor (hdib, i, prgb + i) ;

     return pbmi ;
}
*/
/*-------------------------------------------------------------------
   DibCopy:  Creates a new DIB section from an existing DIB section,
     possibly swapping the DIB width and height.
  -------------------------------------------------------------------*/
/*
HDIB DibCopy (HDIB hdibSrc, BOOL fRotate)
{
     BITMAPINFO * pbmi ;
     BYTE       * pBitsSrc, * pBitsDst ;
     HDIB         hdibDst ;

//     if (!DibIsValid (hdibSrc))
//          return NULL ;

     if (NULL == (pbmi = DibCopyToInfo (hdibSrc)))
          return NULL ;

     if (fRotate)
     {
          pbmi->bmiHeader.biWidth = DibHeight (hdibSrc) ;
          pbmi->bmiHeader.biHeight = DibWidth (hdibSrc) ;
     }

     hdibDst = DibCreateFromInfo (pbmi) ;
     free (pbmi) ;

     if (hdibDst == NULL)
          return NULL ;

          // Copy the bits

     if (!fRotate)
     {
          pBitsSrc = (unsigned char *) DibBitsPtr (hdibSrc) ;
          pBitsDst = (unsigned char *) DibBitsPtr (hdibDst) ;

          CopyMemory (pBitsDst, pBitsSrc, DibBitsSize (hdibSrc)) ;
     }
     return hdibDst ;
}
*/
/*----------------------------------------------------------------------
   DibCopyToPackedDib is generally used for saving DIBs and for 
     transferring DIBs to the clipboard. In the second case, the second 
     argument should be set to TRUE so that the memory is allocated 
     with the GMEM_SHARE flag.
  ----------------------------------------------------------------------*/
/*
BITMAPINFO * DibCopyToPackedDib (HDIB hdib, BOOL fUseGlobal)
{
     BITMAPINFO * pPackedDib ;
     BYTE       * pBits ;
     DWORD        dwDibSize ;
     HDC          hdcMem ;
     HGLOBAL      hGlobal ;
     int          iNumColors ;
     PDIBSTRUCT   pdib = (PDIBSTRUCT) hdib ;
     RGBQUAD    * prgb ;

//     if (!DibIsValid (hdib))
//          return NULL ;

          // Allocate memory for packed DIB

     dwDibSize = DibTotalSize (hdib) ;

     if (fUseGlobal)
     {
          hGlobal = GlobalAlloc (GHND | GMEM_SHARE, dwDibSize) ;
          pPackedDib = (BITMAPINFO *) GlobalLock (hGlobal) ;
     }
     else
     {
          pPackedDib = (BITMAPINFO *) malloc (dwDibSize) ;
     }

     if (pPackedDib == NULL) 
          return NULL ;

          // Copy the information header

     CopyMemory (pPackedDib, &pdib->ds.dsBmih, sizeof (BITMAPINFOHEADER)) ;

     prgb = (RGBQUAD *) ((BYTE *) pPackedDib + sizeof (BITMAPINFOHEADER)) ;

          // Copy the possible color masks

     if (pdib->ds.dsBmih.biCompression == BI_BITFIELDS)
     {
          CopyMemory (prgb, pdib->ds.dsBitfields, 3 * sizeof (DWORD)) ;

          prgb = (RGBQUAD *) ((BYTE *) prgb + 3 * sizeof (DWORD)) ;
     }
          // Copy the color table

     if ((iNumColors = DibNumColors (hdib)) != 0)
     {
          hdcMem = CreateCompatibleDC (NULL) ;
          SelectObject (hdcMem, pdib->hBitmap) ;
          GetDIBColorTable (hdcMem, 0, iNumColors, prgb) ;
          DeleteDC (hdcMem) ;
     }

     pBits = (BYTE *) (prgb + iNumColors) ;

          // Copy the bits

     CopyMemory (pBits, pdib->pBits, DibBitsSize (pdib)) ;

          // If last argument is TRUE, unlock global memory block and 
          //   cast it to pointer in preparation for return

     if (fUseGlobal)
     {
          GlobalUnlock (hGlobal) ;
          pPackedDib = (BITMAPINFO *) hGlobal ;     
     }
     return pPackedDib ;
}
*/
/*------------------------------------------------------------------
   DibCopyFromPackedDib is generally used for pasting DIBs from the 
     clipboard.
  ------------------------------------------------------------------*/
/*
HDIB DibCopyFromPackedDib (BITMAPINFO * pPackedDib)
{
     BYTE     * pBits ;     
     DWORD      dwInfoSize, dwMaskSize, dwColorSize ;
     int        iBitCount ;
     PDIBSTRUCT pdib ;

          // Get the size of the information header and do validity check
     
     dwInfoSize = pPackedDib->bmiHeader.biSize ;

     if (dwInfoSize != sizeof (BITMAPCOREHEADER) &&
         dwInfoSize != sizeof (BITMAPINFOHEADER) &&
         dwInfoSize != sizeof (BITMAPV4HEADER)   &&
         dwInfoSize != sizeof (BITMAPV5HEADER))
     {
          return NULL ;
     }
          // Get the possible size of the color masks

     if (dwInfoSize == sizeof (BITMAPINFOHEADER) &&
          pPackedDib->bmiHeader.biCompression == BI_BITFIELDS)
     {
          dwMaskSize = 3 * sizeof (DWORD) ;
     }
     else
     {
          dwMaskSize = 0 ;
     }
          // Get the size of the color table

     if (dwInfoSize == sizeof (BITMAPCOREHEADER))
     {
          iBitCount = ((BITMAPCOREHEADER *) pPackedDib)->bcBitCount ;

          if (iBitCount <= 8)
          {
               dwColorSize = (1 << iBitCount) * sizeof (RGBTRIPLE) ;
          }
          else
               dwColorSize = 0 ;
     }
     else           // all non-OS/2 compatible DIBs
     {
          if (pPackedDib->bmiHeader.biClrUsed > 0)
          {
               dwColorSize = pPackedDib->bmiHeader.biClrUsed * sizeof (RGBQUAD);
          }
          else if (pPackedDib->bmiHeader.biBitCount <= 8)
          {
               dwColorSize = (1 << pPackedDib->bmiHeader.biBitCount) * 
                                                       sizeof (RGBQUAD) ;
          }
          else
          {
               dwColorSize = 0 ;
          }
     }
          // Finally, get the pointer to the bits in the packed-DIB

     pBits = (BYTE *) pPackedDib + dwInfoSize + dwMaskSize + dwColorSize ;

          // Create the HDIB from the packed DIB pointer

     pdib = (PDIBSTRUCT) DibCreateFromInfo (pPackedDib) ;

          // Copy the pixel bits

     CopyMemory (pdib->pBits, pBits, DibBitsSize (pdib)) ;

     return pdib ;
}
*/
/*-----------------------------------------------------
   DibFileLoad:  Creates a DIB section from a DIB file
  -----------------------------------------------------*/
HDIB DibFileLoad (const TCHAR * szFileName)
{
     BITMAPFILEHEADER bmfh ;
     BITMAPINFO     * pbmi ;
     BOOL             bSuccess ;
     DWORD            dwInfoSize, dwBitsSize, dwBytesRead ;
     HANDLE           hFile ;
     HDIB             hDib ;

          // Open the file: read access, prohibit write access

     hFile = CreateFile (szFileName, GENERIC_READ, FILE_SHARE_READ, NULL, 
                         OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL) ;

     if (hFile == INVALID_HANDLE_VALUE)
          return NULL ;

          // Read in the BITMAPFILEHEADER

     bSuccess = ReadFile (hFile, &bmfh, sizeof (BITMAPFILEHEADER), 
                          &dwBytesRead, NULL) ;

     if (!bSuccess || (dwBytesRead != sizeof (BITMAPFILEHEADER))         
                   || (bmfh.bfType != * (WORD *) "BM"))
     {
          CloseHandle (hFile) ;
          return NULL ;
     }
          // Allocate memory for the information structure & read it in

     dwInfoSize = bmfh.bfOffBits - sizeof (BITMAPFILEHEADER) ;

     if (NULL == (pbmi = (BITMAPINFO *) malloc (dwInfoSize)))
     {
          CloseHandle (hFile) ;
          return NULL ;
     }

     bSuccess = ReadFile (hFile, pbmi, dwInfoSize, &dwBytesRead, NULL) ;

     if (!bSuccess || (dwBytesRead != dwInfoSize))
     {
          CloseHandle (hFile) ;
          free (pbmi) ;
          return NULL ;
     }
          // Create the DIB

     hDib = DibCreateFromInfo (pbmi) ;
     free (pbmi) ;

     if (hDib == NULL)
     {
          CloseHandle (hFile) ;
          return NULL ;
     }
          // Read in the bits

     dwBitsSize = bmfh.bfSize - bmfh.bfOffBits ;

     bSuccess = ReadFile (hFile, ((PDIBSTRUCT) hDib)->pBits, 
                          dwBitsSize, &dwBytesRead, NULL) ;
     CloseHandle (hFile) ;

     if (!bSuccess || (dwBytesRead != dwBitsSize))
     {
          DibDelete (hDib) ;
          return NULL ;
     }
     return hDib ;
}

/*---------------------------------------------
   DibFileSave:  Saves a DIB section to a file
  ---------------------------------------------*/
/*
BOOL DibFileSave (HDIB hdib, const TCHAR * szFileName)
{
     BITMAPFILEHEADER bmfh ;
     BITMAPINFO     * pbmi ;
     BOOL             bSuccess ;
     DWORD            dwTotalSize, dwBytesWritten ;
     HANDLE           hFile ;

     hFile = CreateFile (szFileName, GENERIC_WRITE, 0, NULL,
                         CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL) ;

     if (hFile == INVALID_HANDLE_VALUE)
          return FALSE ;

     dwTotalSize  = DibTotalSize (hdib) ;

     bmfh.bfType      = * (WORD *) "BM" ;
     bmfh.bfSize      = sizeof (BITMAPFILEHEADER) + dwTotalSize ;
     bmfh.bfReserved1 = 0 ;
     bmfh.bfReserved2 = 0 ;
     bmfh.bfOffBits   = bmfh.bfSize - DibBitsSize (hdib) ;

          // Write the BITMAPFILEHEADER

     bSuccess = WriteFile (hFile, &bmfh, sizeof (BITMAPFILEHEADER), 
                           &dwBytesWritten, NULL) ;

     if (!bSuccess || (dwBytesWritten != sizeof (BITMAPFILEHEADER)))
     {
          CloseHandle (hFile) ;
          DeleteFile (szFileName) ;
          return FALSE ;
     }
          // Get entire DIB in packed DIB format

     if (NULL == (pbmi = DibCopyToPackedDib (hdib, FALSE)))
     {
          CloseHandle (hFile) ;
          DeleteFile (szFileName) ;
          return FALSE ;
     }
          // Write out the packed-DIB

     bSuccess = WriteFile (hFile, pbmi, dwTotalSize, &dwBytesWritten, NULL) ;
     CloseHandle (hFile) ;
     free (pbmi) ;

     if (!bSuccess || (dwBytesWritten != dwTotalSize))
     {
          DeleteFile (szFileName) ;
          return FALSE ;
     }
     return TRUE ;
}
*/
/*---------------------------------------------------
   DibCopyToDdb:  For more efficient screen displays
  ---------------------------------------------------*/
/*
HBITMAP DibCopyToDdb (HDIB hdib, HWND hwnd, HPALETTE hPalette)
{
     BITMAPINFO * pbmi ;
     HBITMAP      hBitmap ;
     HDC          hdc ;

//     if (!DibIsValid (hdib))
//          return NULL ;

     if (NULL == (pbmi = DibCopyToInfo (hdib)))
          return NULL ;

     hdc = GetDC (hwnd) ;

     if (hPalette)
     {
          SelectPalette (hdc, hPalette, FALSE) ;
          RealizePalette (hdc) ;
     }
     
     hBitmap = CreateDIBitmap (hdc, DibInfoHeaderPtr (hdib), CBM_INIT,
                               DibBitsPtr (hdib), pbmi, DIB_RGB_COLORS) ;

     ReleaseDC (hwnd, hdc) ;
     free (pbmi) ;

     return hBitmap ;
}
*/
/*----------------------------------------
   DIBPAL.C -- Palette-Creation Functions
               (c) Charles Petzold, 1998
  ----------------------------------------*/


/*------------------------------------------------------------
   DibPalDibTable: Creates a palette from the DIB color table
  ------------------------------------------------------------*/
/*
HPALETTE DibPalDibTable (HDIB hdib)
{
     HPALETTE     hPalette ;
     int          i, iNum ;
     LOGPALETTE * plp ;
     RGBQUAD      rgb ;

     if (0 == (iNum = DibNumColors (hdib)))
          return NULL ;

     plp = (LOGPALETTE *) malloc (sizeof (LOGPALETTE) + (iNum - 1) * sizeof (PALETTEENTRY)) ;

     plp->palVersion    = 0x0300 ;
     plp->palNumEntries = iNum ;

     for (i = 0 ; i < iNum ; i++)
     {
          DibGetColor (hdib, i, &rgb) ;

          plp->palPalEntry[i].peRed   = rgb.rgbRed ;
          plp->palPalEntry[i].peGreen = rgb.rgbGreen ;
          plp->palPalEntry[i].peBlue  = rgb.rgbBlue ;
          plp->palPalEntry[i].peFlags = 0 ;
     }
     hPalette = CreatePalette (plp) ;
     free (plp) ;
     return hPalette ;
}
*/
/*------------------------------------------------------------------------
   DibPalAllPurpose: Creates a palette suitable for a wide variety
          of images; the palette has 247 entries, but 15 of them are 
          duplicates or match the standard 20 colors.
  ------------------------------------------------------------------------*/
/*
HPALETTE DibPalAllPurpose (void)
{
     HPALETTE     hPalette ;
     int          i, incr, R, G, B ;
     LOGPALETTE * plp ;

     plp = (LOGPALETTE *) malloc (sizeof (LOGPALETTE) + 246 * sizeof (PALETTEENTRY)) ;

     plp->palVersion    = 0x0300 ;
     plp->palNumEntries = 247 ;

          // The following loop calculates 31 grays shades, but 3 of them
          //        will match the standard 20 colors

     for (i = 0, G = 0, incr = 8 ; G <= 0xFF ; i++, G += incr)
     {
          plp->palPalEntry[i].peRed   = (BYTE) G ;
          plp->palPalEntry[i].peGreen = (BYTE) G ;
          plp->palPalEntry[i].peBlue  = (BYTE) G ;
          plp->palPalEntry[i].peFlags = 0 ;

          incr = (incr == 9 ? 8 : 9) ;
     }

          // The following loop is responsible for 216 entries, but 8 of 
          //        them will match the standard 20 colors, and another
          //        4 of them will match the gray shades above.

     for (R = 0 ; R <= 0xFF ; R += 0x33)
     for (G = 0 ; G <= 0xFF ; G += 0x33)
     for (B = 0 ; B <= 0xFF ; B += 0x33)
     {
          plp->palPalEntry[i].peRed   = (BYTE) R ;
          plp->palPalEntry[i].peGreen = (BYTE) G ;
          plp->palPalEntry[i].peBlue  = (BYTE) B ;
          plp->palPalEntry[i].peFlags = 0 ;

          i++ ;
     }
     hPalette = CreatePalette (plp) ;

     free (plp) ;
     return hPalette ;
}
*/
/*------------------------------------------------------------------------
   DibPalUniformGrays:  Creates a palette of iNum grays, uniformly spaced
  ------------------------------------------------------------------------*/
/*
HPALETTE DibPalUniformGrays (int iNum)
{
     HPALETTE     hPalette ;
     int          i ;
     LOGPALETTE * plp ;

     plp = (LOGPALETTE *) malloc (sizeof (LOGPALETTE) + (iNum - 1) * sizeof (PALETTEENTRY)) ;

     plp->palVersion    = 0x0300 ;
     plp->palNumEntries = iNum ;

     for (i = 0 ; i < iNum ; i++)
     {
          plp->palPalEntry[i].peRed   =
          plp->palPalEntry[i].peGreen = 
          plp->palPalEntry[i].peBlue  = (BYTE) (i * 255 / (iNum - 1)) ;
          plp->palPalEntry[i].peFlags = 0 ;
     }
     hPalette = CreatePalette (plp) ;
     free (plp) ;
     return hPalette ;
}
*/
/*------------------------------------------------------------------------
   DibPalUniformColors: Creates a palette of iNumR x iNumG x iNumB colors
  ------------------------------------------------------------------------*/
/*
HPALETTE DibPalUniformColors (int iNumR, int iNumG, int iNumB)
{
     HPALETTE     hPalette ;
     int          i, iNum, R, G, B ;
     LOGPALETTE * plp ;

     iNum = iNumR * iNumG * iNumB ;

     plp = (LOGPALETTE *) malloc (sizeof (LOGPALETTE) + (iNum - 1) * sizeof (PALETTEENTRY)) ;

     plp->palVersion    = 0x0300 ;
     plp->palNumEntries = iNumR * iNumG * iNumB ;

     i = 0 ;
     for (R = 0 ; R < iNumR ; R++)
     for (G = 0 ; G < iNumG ; G++)
     for (B = 0 ; B < iNumB ; B++)
     {
          plp->palPalEntry[i].peRed   = (BYTE) (R * 255 / (iNumR - 1)) ;
          plp->palPalEntry[i].peGreen = (BYTE) (G * 255 / (iNumG - 1)) ;
          plp->palPalEntry[i].peBlue  = (BYTE) (B * 255 / (iNumB - 1)) ;
          plp->palPalEntry[i].peFlags = 0 ;

          i++ ;
     }
     hPalette = CreatePalette (plp) ;
     free (plp) ;
     return hPalette ;
}
*/
/*---------------------------------------------------------------
   DibPalVga:  Creates a palette based on standard 16 VGA colors
  ---------------------------------------------------------------*/
/*
HPALETTE DibPalVga (void)
{
     static RGBQUAD rgb [16] = { 0x00, 0x00, 0x00, 0x00,
                                 0x00, 0x00, 0x80, 0x00,
                                 0x00, 0x80, 0x00, 0x00,
                                 0x00, 0x80, 0x80, 0x00,
                                 0x80, 0x00, 0x00, 0x00,
                                 0x80, 0x00, 0x80, 0x00,
                                 0x80, 0x80, 0x00, 0x00,
                                 0x80, 0x80, 0x80, 0x00,
                                 0xC0, 0xC0, 0xC0, 0x00,
                                 0x00, 0x00, 0xFF, 0x00,
                                 0x00, 0xFF, 0x00, 0x00,
                                 0x00, 0xFF, 0xFF, 0x00,
                                 0xFF, 0x00, 0x00, 0x00,
                                 0xFF, 0x00, 0xFF, 0x00,
                                 0xFF, 0xFF, 0x00, 0x00,
                                 0xFF, 0xFF, 0xFF, 0x00 } ;
     HPALETTE       hPalette ;
     int            i ;
     LOGPALETTE   * plp ;

     plp = (LOGPALETTE *) malloc (sizeof (LOGPALETTE) + 15 * sizeof (PALETTEENTRY)) ;

     plp->palVersion    = 0x0300 ;
     plp->palNumEntries = 16 ;

     for (i = 0 ; i < 16 ; i++)
     {
          plp->palPalEntry[i].peRed   = rgb[i].rgbRed ;
          plp->palPalEntry[i].peGreen = rgb[i].rgbGreen ;
          plp->palPalEntry[i].peBlue  = rgb[i].rgbBlue ;
          plp->palPalEntry[i].peFlags = 0 ;
     }
     hPalette = CreatePalette (plp) ;
     free (plp) ;
     return hPalette ;
}
*/
/*---------------------------------------------
   Macro used in palette optimization routines
  ---------------------------------------------*/

#define PACK_RGB(R,G,B,iRes) ((int) (R) | ((int) (G) <<  (iRes)) |       \
                                          ((int) (B) << ((iRes) + (iRes))))

/*--------------------------------------------------------------------
   AccumColorCounts: Fills up piCount (indexed by a packed RGB color)
     with counts of pixels of that color.
  --------------------------------------------------------------------*/
/*
static void AccumColorCounts (HDIB hdib, int * piCount, int iRes)
{
     int     x, y, cx, cy ;
     RGBQUAD rgb ;

     cx = DibWidth (hdib) ;
     cy = DibHeight (hdib) ;

     for (y = 0 ; y < cy ; y++)
     for (x = 0 ; x < cx ; x++)
     {
          DibGetPixelColor (hdib, x, y, &rgb) ;

          rgb.rgbRed   >>= (8 - iRes) ;
          rgb.rgbGreen >>= (8 - iRes) ;
          rgb.rgbBlue  >>= (8 - iRes) ;

          ++piCount [PACK_RGB (rgb.rgbRed, rgb.rgbGreen, rgb.rgbBlue, iRes)] ;
     }
}
*/
/*--------------------------------------------------------------
   DibPalPopularity:  Popularity algorithm for optimized colors
  --------------------------------------------------------------*/
/*
HPALETTE DibPalPopularity (HDIB hdib, int iRes)
{
     HPALETTE     hPalette ;
     int          i, iArraySize, iEntry, iCount, iIndex, iMask, R, G, B ;
     int        * piCount ;
     LOGPALETTE * plp ;

          // Validity checks
    
     if (DibBitCount (hdib) < 16)
          return NULL ;

     if (iRes < 3 || iRes > 8)
          return NULL ;

          // Allocate array for counting pixel colors

     iArraySize = 1 << (3 * iRes) ;
     iMask = (1 << iRes) - 1 ;

     if (NULL == (piCount = (int *) calloc (iArraySize, sizeof (int))))
          return NULL ;

          // Get the color counts

     AccumColorCounts (hdib, piCount, iRes) ;

          // Set up a palette

     plp = (LOGPALETTE *) malloc (sizeof (LOGPALETTE) + 235 * sizeof (PALETTEENTRY)) ;

     plp->palVersion = 0x0300 ;

     for (iEntry = 0 ; iEntry < 236 ; iEntry++)
     {
          for (i = 0, iCount = 0 ; i < iArraySize ; i++)
               if (piCount[i] > iCount)
               {
                    iCount = piCount[i] ;
                    iIndex = i ;
               }

          if (iCount == 0)
               break ;

          R = (iMask &  iIndex                  ) << (8 - iRes) ;
          G = (iMask & (iIndex >>         iRes )) << (8 - iRes) ;
          B = (iMask & (iIndex >> (iRes + iRes))) << (8 - iRes) ;

          plp->palPalEntry[iEntry].peRed   = (BYTE) R ; 
          plp->palPalEntry[iEntry].peGreen = (BYTE) G ; 
          plp->palPalEntry[iEntry].peBlue  = (BYTE) B ; 
          plp->palPalEntry[iEntry].peFlags = 0 ;

          piCount [iIndex] = 0 ;
     }
          // On exit from the loop iEntry will be the number of stored entries

     plp->palNumEntries = iEntry ;

          // Create the palette, clean up, and return the palette handle

     hPalette = CreatePalette (plp) ;

     free (piCount) ;
     free (plp) ;

     return hPalette ;
}
*/
/*-------------------------------------------------------
   Structures used for implementing median cut algorithm
  -------------------------------------------------------*/

typedef struct           // defines dimension of a box
{
     int Rmin, Rmax, Gmin, Gmax, Bmin, Bmax ;
}
MINMAX ;

typedef struct           // for Compare routine for qsort
{
     int     iBoxCount ;
     RGBQUAD rgbBoxAv ;
}
BOXES ;

/*----------------------------
   FindAverageColor: In a box
  ----------------------------*/

static int FindAverageColor (int * piCount, MINMAX mm, 
                             int iRes, RGBQUAD * prgb)
{
     int R, G, B, iR, iG, iB, iTotal, iCount ;

          // Initialize some variables

     iTotal = iR = iG = iB = 0 ;
          
          // Loop through all colors in the box
          
     for (R = mm.Rmin ; R <= mm.Rmax ; R++)
     for (G = mm.Gmin ; G <= mm.Gmax ; G++)
     for (B = mm.Bmin ; B <= mm.Bmax ; B++)
     {
               // Get the number of pixels of that color

          iCount = piCount [PACK_RGB (R, G, B, iRes)] ;

               // Weight the pixel count by the color value

          iR += iCount * R ;
          iG += iCount * G ;
          iB += iCount * B ;

          iTotal += iCount ;
     }
          // Find the average color

     prgb->rgbRed   = (BYTE) ((iR / iTotal) << (8 - iRes)) ;
     prgb->rgbGreen = (BYTE) ((iG / iTotal) << (8 - iRes)) ;
     prgb->rgbBlue  = (BYTE) ((iB / iTotal) << (8 - iRes)) ;

          // Return the total number of pixels in the box
     
     return iTotal ;
}

/*------------------------------
   CutBox:  Divide a box in two
  ------------------------------*/

static void CutBox (int * piCount, int iBoxCount, MINMAX mm,
                    int iRes, int iLevel, BOXES * pboxes, int * piEntry)
{
     int    iCount, R, G, B ;
     MINMAX mmNew ;
     
          // If the box is empty, return 

     if (iBoxCount == 0)
          return ;

          // If the nesting level is 8, or the box is one pixel, we're ready
          //   to find the average color in the box and save it along with
          //   the number of pixels of that color

     if (iLevel == 8 || (mm.Rmin == mm.Rmax && 
                         mm.Gmin == mm.Gmax && 
                         mm.Bmin == mm.Bmax))
     {
          pboxes[*piEntry].iBoxCount = 
               FindAverageColor (piCount, mm, iRes, &pboxes[*piEntry].rgbBoxAv) ;

          (*piEntry) ++ ;
     }
          // Otherwise, if blue is the largest side, split it

     else if ((mm.Bmax - mm.Bmin > mm.Rmax - mm.Rmin) && 
              (mm.Bmax - mm.Bmin > mm.Gmax - mm.Gmin))
     {
               // Initialize a counter and loop through the blue side

          iCount = 0 ;

          for (B = mm.Bmin ; B < mm.Bmax ; B++)
          {
                    // Accumulate all the pixels for each successive blue value

               for (R = mm.Rmin ; R <= mm.Rmax ; R++)
               for (G = mm.Gmin ; G <= mm.Gmax ; G++)
                    iCount += piCount [PACK_RGB (R, G, B, iRes)] ;

                    // If it's more than half the box count, we're there

               if (iCount >= iBoxCount / 2)
                    break ;

                    // If the next blue value will be the max, we're there

               if (B == mm.Bmax - 1)
                    break ;
          }
               // Cut the two split boxes.
               //   The second argument to CutBox is the new box count.
               //   The third argument is the new min and max values.

          mmNew = mm ;
          mmNew.Bmin = mm.Bmin ;
          mmNew.Bmax = B ;

          CutBox (piCount, iCount, mmNew, iRes, iLevel + 1, 
                  pboxes, piEntry) ;

          mmNew.Bmin = B + 1 ;
          mmNew.Bmax = mm.Bmax ;

          CutBox (piCount, iBoxCount - iCount, mmNew, iRes, iLevel + 1,
                  pboxes, piEntry) ;
     }
          // Otherwise, if red is the largest side, split it (just like blue)

     else if (mm.Rmax - mm.Rmin > mm.Gmax - mm.Gmin)
     {
          iCount = 0 ;

          for (R = mm.Rmin ; R < mm.Rmax ; R++)
          {
               for (B = mm.Bmin ; B <= mm.Bmax ; B++)
               for (G = mm.Gmin ; G <= mm.Gmax ; G++)
                    iCount += piCount [PACK_RGB (R, G, B, iRes)] ;

               if (iCount >= iBoxCount / 2)
                    break ;

               if (R == mm.Rmax - 1)
                    break ;
          }
          mmNew = mm ;
          mmNew.Rmin = mm.Rmin ;
          mmNew.Rmax = R ;

          CutBox (piCount, iCount, mmNew, iRes, iLevel + 1, 
                  pboxes, piEntry) ;

          mmNew.Rmin = R + 1 ;
          mmNew.Rmax = mm.Rmax ;

          CutBox (piCount, iBoxCount - iCount, mmNew, iRes, iLevel + 1,
                  pboxes, piEntry) ;
     }
          // Otherwise, split along the green size
     else 
     {
          iCount = 0 ;

          for (G = mm.Gmin ; G < mm.Gmax ; G++)
          {
               for (B = mm.Bmin ; B <= mm.Bmax ; B++)
               for (R = mm.Rmin ; R <= mm.Rmax ; R++)
                    iCount += piCount [PACK_RGB (R, G, B, iRes)] ;

               if (iCount >= iBoxCount / 2)
                    break ;

               if (G == mm.Gmax - 1)
                    break ;
          }
          mmNew = mm ;
          mmNew.Gmin = mm.Gmin ;
          mmNew.Gmax = G ;

          CutBox (piCount, iCount, mmNew, iRes, iLevel + 1, 
                  pboxes, piEntry) ;

          mmNew.Gmin = G + 1 ;
          mmNew.Gmax = mm.Gmax ;

          CutBox (piCount, iBoxCount - iCount, mmNew, iRes, iLevel + 1,
                  pboxes, piEntry) ;
     }
}

/*---------------------------
   Compare routine for qsort
  ---------------------------*/
/*
static int Compare (const BOXES * pbox1, const BOXES * pbox2)
{
     return pbox1->iBoxCount - pbox2->iBoxCount ;
}
*/
/*-----------------------------------------------------------------
   DibPalMedianCut:  Creates palette based on median cut algorithm
  -----------------------------------------------------------------*/
/*
HPALETTE DibPalMedianCut (HDIB hdib, int iRes)
{
     HPALETTE     hPalette = NULL;
     BOXES        boxes [256] ;
     int          i, iArraySize, iCount, R, G, B, iTotCount, iDim, iEntry = 0 ;
     int        * piCount ;
     LOGPALETTE * plp ;
     MINMAX       mm ;

          // Validity checks
     if (DibBitCount (hdib) < 16)
          return NULL ;

     if (iRes < 3 || iRes > 8)
          return NULL ;

          // Accumulate counts of pixel colors

     iArraySize = 1 << (3 * iRes) ;

     if (NULL == (piCount = (int *) calloc (iArraySize, sizeof (int))))
          return NULL ;

     AccumColorCounts (hdib, piCount, iRes) ;

          // Find the dimensions of the total box

     iDim = 1 << iRes ;

     mm.Rmin = mm.Gmin = mm.Bmin = iDim - 1 ;
     mm.Rmax = mm.Gmax = mm.Bmax = 0 ;

     iTotCount = 0 ;

     for (R = 0 ; R < iDim ; R++)
     for (G = 0 ; G < iDim ; G++)
     for (B = 0 ; B < iDim ; B++)
          if ((iCount = piCount [PACK_RGB (R, G, B, iRes)]) > 0)
          {
               iTotCount += iCount ;

               if (R < mm.Rmin) mm.Rmin = R ;
               if (G < mm.Gmin) mm.Gmin = G ;
               if (B < mm.Bmin) mm.Bmin = B ;

               if (R > mm.Rmax) mm.Rmax = R ;
               if (G > mm.Gmax) mm.Gmax = G ;
               if (B > mm.Bmax) mm.Bmax = B ;
          }

          // Cut the first box (iterative function).
          //   On return, the boxes structure will have up to 256 RGB values, 
          //        one for each of the boxes, and the number of pixels in
          //        each box.
          //   The iEntry value will indicate the number of non-empty boxes.

     CutBox (piCount, iTotCount, mm, iRes, 0, boxes, &iEntry) ;
     free (piCount) ;

          // Sort the RGB table by the number of pixels for each color

     qsort (boxes, iEntry, sizeof (BOXES), Compare) ;

     plp = malloc (sizeof (LOGPALETTE) + (iEntry - 1) * sizeof (PALETTEENTRY)) ;

     if (plp == NULL)
          return NULL ;

     plp->palVersion    = 0x0300 ;
     plp->palNumEntries = iEntry ;

     for (i = 0 ; i < iEntry ; i++)
     {
          plp->palPalEntry[i].peRed   = boxes[i].rgbBoxAv.rgbRed ;
          plp->palPalEntry[i].peGreen = boxes[i].rgbBoxAv.rgbGreen ;
          plp->palPalEntry[i].peBlue  = boxes[i].rgbBoxAv.rgbBlue ;
          plp->palPalEntry[i].peFlags = 0 ;
     }

     hPalette = CreatePalette (plp) ;

     free (plp) ;
     return hPalette ;
}
*/