// Emacs style mode select   -*- C++ -*- 
//-----------------------------------------------------------------------------
//
// $Id:$
//
// Copyright (C) 1993-1996 by id Software, Inc.
//
// This source is available for distribution and/or modification
// only under the terms of the DOOM Source Code License as
// published by id Software. All rights reserved.
//
// The source is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// FITNESS FOR A PARTICULAR PURPOSE. See the DOOM Source Code License
// for more details.
//
// $Log:$
//
// DESCRIPTION:
//	DOOM graphics stuff for X11, UNIX.
//
//-----------------------------------------------------------------------------

static const char
rcsid[] = "$Id: i_x.c,v 1.6 1997/02/03 22:45:10 b1 Exp $";

#include <stdlib.h>
#include <unistd.h>

#include <stdarg.h>
#include <sys/time.h>
#include <sys/types.h>

#include <errno.h>
#include <signal.h>

#include "doomstat.h"
#include "i_system.h"
#include "v_video.h"
#include "m_argv.h"
#include "d_main.h"
#include "spi.h"
#include "vga.h"
#include "keyboard.h"
#include "misc.h"
#include "doomdef.h"

int		X_width;
int		X_height;

// Blocky mode,
// replace each 320x200 pixel with multiply*multiply pixels.
// According to Dave Taylor, it still is a bonehead thing
// to use ....
static int	multiply=2;

void I_ShutdownGraphics(void)
{
}

//
// I_StartFrame
//
void I_StartFrame (void)
{
}

boolean		mousemoved = false;
boolean		shmFinished;

int scancode(boolean extended, unsigned char keyval)
{
  int rc;

  printf("scancode(%02x, %02x)\n", extended, keyval);
  
  if (extended)
    switch (keyval)
      {
      case 0x75: rc = KEY_UPARROW;    break;
      case 0x6b: rc = KEY_LEFTARROW;  break;
      case 0x72: rc = KEY_DOWNARROW;  break;
      case 0x74: rc = KEY_RIGHTARROW; break;
      case 0x71: rc = KEY_BACKSPACE;  break;
      case 0x5a: rc = KEY_EQUALS;     break;
      case 0x14: rc = KEY_RCTRL;      break;	
      case 0x11: rc = KEY_RALT;       break;
      }
  else
    switch (keyval)
      {
      case 0x76: rc = KEY_ESCAPE;     break;
      case 0x5a: rc = KEY_ENTER;      break;
      case 0x0d: rc = KEY_TAB;        break;
      case 0x05: rc = KEY_F1;         break;
      case 0x06: rc = KEY_F2;         break;
      case 0x04: rc = KEY_F3;         break;
      case 0x0c: rc = KEY_F4;         break;
      case 0x03: rc = KEY_F5;         break;
      case 0x0b: rc = KEY_F6;         break;
      case 0x83: rc = KEY_F7;         break;
      case 0x0a: rc = KEY_F8;         break;
      case 0x01: rc = KEY_F9;         break;
      case 0x09: rc = KEY_F10;        break;
      case 0x78: rc = KEY_F11;        break;
      case 0x07: rc = KEY_F12;        break;
      case 0x66: rc = KEY_BACKSPACE;  break;
      case 0x55: rc = KEY_EQUALS;     break;
      case 0x7b: rc = KEY_MINUS;      break;
      case 0x4e: rc = KEY_MINUS;      break;
      case 0x12: rc = KEY_RSHIFT;     break;
      case 0x59: rc = KEY_RSHIFT;     break;
      case 0x14: rc = KEY_RCTRL;      break;
      case 0x11: rc = KEY_RALT;       break;
      case 0x1c: rc = 'a';            break;
      case 0x32: rc = 'b';            break;
      case 0x21: rc = 'c';            break;
      case 0x23: rc = 'd';            break;
      case 0x24: rc = 'e';            break;
      case 0x2b: rc = 'f';            break;
      case 0x34: rc = 'g';            break;
      case 0x33: rc = 'h';            break;
      case 0x43: rc = 'i';            break;
      case 0x3b: rc = 'j';            break;
      case 0x42: rc = 'k';            break;
      case 0x4b: rc = 'l';            break;
      case 0x3a: rc = 'm';            break;
      case 0x31: rc = 'n';            break;
      case 0x44: rc = 'o';            break;
      case 0x4d: rc = 'p';            break;
      case 0x15: rc = 'q';            break;
      case 0x2d: rc = 'r';            break;
      case 0x1b: rc = 's';            break;
      case 0x2c: rc = 't';            break;
      case 0x3c: rc = 'u';            break;
      case 0x2a: rc = 'v';            break;
      case 0x1d: rc = 'w';            break;
      case 0x22: rc = 'x';            break;
      case 0x35: rc = 'y';            break;
      case 0x1a: rc = 'z';            break;
      case 0x45: rc = '0';            break;
      case 0x16: rc = '1';            break;
      case 0x1e: rc = '2';            break;
      case 0x26: rc = '3';            break;
      case 0x25: rc = '4';            break;
      case 0x2e: rc = '5';            break;
      case 0x36: rc = '6';            break;
      case 0x3d: rc = '7';            break;
      case 0x3e: rc = '8';            break;
      case 0x46: rc = '9';            break;
      case 0x29: rc = ' ';            break;
      }

  return rc;
}


void I_GetEvent(void)
{
  event_t event;
  unsigned int rawkey;
  
  rawkey = keyboard_getevent();
  event.type = (rawkey & 0x200 ? ev_keydown : ev_keyup);
  event.data1 = scancode(rawkey & 0x100, rawkey & 0xff);
  D_PostEvent(&event);
}

//
// I_StartTic
//
void I_StartTic (void)
{
  while (keyboard_count())
    I_GetEvent();
}


//
// I_UpdateNoBlit
//
void I_UpdateNoBlit (void)
{
    // what is this?
}

//
// I_FinishUpdate
//
void I_FinishUpdate (void)
{
  memcpy (vga_fb, screens[0], SCREENWIDTH*SCREENHEIGHT);
}


//
// I_ReadScreen
//
void I_ReadScreen (byte* scr)
{
    memcpy (scr, screens[0], SCREENWIDTH*SCREENHEIGHT);
}

//
// Palette stuff.
//
void UploadNewPalette(byte *palette)
{
  int i;
  unsigned val;

  for (i=0 ; i<256 ; i++)
    {
      val = (*palette++) << 16;
      val |= (*palette++) << 8;
      val |= (*palette++);
      vga_palette(0, i, val);
    }
}

//
// I_SetPalette
//
void I_SetPalette (byte* palette)
{
    printf("Changing the palette\n");
    UploadNewPalette(palette);
}


void I_InitGraphics(void)
{

    int			n;
    int			pnum;
    int			x=0;
    int			y=0;
    
    // warning: char format, different type arg
    char		xsign=' ';
    char		ysign=' ';
    
    static int		firsttime=1;

    if (!firsttime)
	return;
    firsttime = 0;

    vga_set_mode(VGA_MODE_DOUBLE);
    
    X_width = SCREENWIDTH * multiply;
    X_height = SCREENHEIGHT * multiply;

    // check for command-line geometry
    if ( (pnum=M_CheckParm("-geom")) ) // suggest parentheses around assignment
    {
	// warning: char format, different type arg 3,5
	n = sscanf(myargv[pnum+1], "%c%d%c%d", &xsign, &x, &ysign, &y);
	
	if (n==2)
	    x = y = 0;
	else if (n==6)
	{
	    if (xsign == '-')
		x = -x;
	    if (ysign == '-')
		y = -y;
	}
	else
	    I_Error("bad -geom parameter");
    }

    vga_set_mode(VGA_MODE_DOUBLE);
    
    screens[0] = (unsigned char *)malloc(SCREENWIDTH*SCREENHEIGHT);
}


unsigned	exptable[256];

void InitExpand (void)
{
    int		i;
	
    for (i=0 ; i<256 ; i++)
	exptable[i] = i | (i<<8) | (i<<16) | (i<<24);
}

double		exptable2[256*256];

void InitExpand2 (void)
{
    int		i;
    int		j;
    // UNUSED unsigned	iexp, jexp;
    double*	exp;
    union
    {
	double 		d;
	unsigned	u[2];
    } pixel;
	
    printf ("building exptable2...\n");
    exp = exptable2;
    for (i=0 ; i<256 ; i++)
    {
	pixel.u[0] = i | (i<<8) | (i<<16) | (i<<24);
	for (j=0 ; j<256 ; j++)
	{
	    pixel.u[1] = j | (j<<8) | (j<<16) | (j<<24);
	    *exp++ = pixel.d;
	}
    }
    printf ("done.\n");
}

int	inited;

void
Expand4
( unsigned*	lineptr,
  double*	xline )
{
    double	dpixel;
    unsigned	x;
    unsigned 	y;
    unsigned	fourpixels;
    unsigned	step;
    double*	exp;
	
    exp = exptable2;
    if (!inited)
    {
	inited = 1;
	InitExpand2 ();
    }
		
		
    step = 3*SCREENWIDTH/2;
	
    y = SCREENHEIGHT-1;
    do
    {
	x = SCREENWIDTH;

	do
	{
	    fourpixels = lineptr[0];
			
	    dpixel = *(double *)( (int)exp + ( (fourpixels&0xffff0000)>>13) );
	    xline[0] = dpixel;
	    xline[160] = dpixel;
	    xline[320] = dpixel;
	    xline[480] = dpixel;
			
	    dpixel = *(double *)( (int)exp + ( (fourpixels&0xffff)<<3 ) );
	    xline[1] = dpixel;
	    xline[161] = dpixel;
	    xline[321] = dpixel;
	    xline[481] = dpixel;

	    fourpixels = lineptr[1];
			
	    dpixel = *(double *)( (int)exp + ( (fourpixels&0xffff0000)>>13) );
	    xline[2] = dpixel;
	    xline[162] = dpixel;
	    xline[322] = dpixel;
	    xline[482] = dpixel;
			
	    dpixel = *(double *)( (int)exp + ( (fourpixels&0xffff)<<3 ) );
	    xline[3] = dpixel;
	    xline[163] = dpixel;
	    xline[323] = dpixel;
	    xline[483] = dpixel;

	    fourpixels = lineptr[2];
			
	    dpixel = *(double *)( (int)exp + ( (fourpixels&0xffff0000)>>13) );
	    xline[4] = dpixel;
	    xline[164] = dpixel;
	    xline[324] = dpixel;
	    xline[484] = dpixel;
			
	    dpixel = *(double *)( (int)exp + ( (fourpixels&0xffff)<<3 ) );
	    xline[5] = dpixel;
	    xline[165] = dpixel;
	    xline[325] = dpixel;
	    xline[485] = dpixel;

	    fourpixels = lineptr[3];
			
	    dpixel = *(double *)( (int)exp + ( (fourpixels&0xffff0000)>>13) );
	    xline[6] = dpixel;
	    xline[166] = dpixel;
	    xline[326] = dpixel;
	    xline[486] = dpixel;
			
	    dpixel = *(double *)( (int)exp + ( (fourpixels&0xffff)<<3 ) );
	    xline[7] = dpixel;
	    xline[167] = dpixel;
	    xline[327] = dpixel;
	    xline[487] = dpixel;

	    lineptr+=4;
	    xline+=8;
	} while (x-=16);
	xline += step;
    } while (y--);
}


