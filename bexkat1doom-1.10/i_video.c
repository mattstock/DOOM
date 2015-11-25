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

#include "doomdef.h"

#define POINTER_WARP_COUNTDOWN	1

unsigned char *vga = (unsigned char *)0xc0000000;
unsigned int *vga_palette = (unsigned int *)0xc0400000;

int		X_width;
int		X_height;

// Blocky mode,
// replace each 320x200 pixel with multiply*multiply pixels.
// According to Dave Taylor, it still is a bonehead thing
// to use ....
static int	multiply=1;


//
//  Translates the key currently in X_event
//
/*
int xlatekey(void)
{

    int rc;

    switch(rc = XKeycodeToKeysym(X_display, X_event.xkey.keycode, 0))
    {
      case XK_Left:	rc = KEY_LEFTARROW;	break;
      case XK_Right:	rc = KEY_RIGHTARROW;	break;
      case XK_Down:	rc = KEY_DOWNARROW;	break;
      case XK_Up:	rc = KEY_UPARROW;	break;
      case XK_Escape:	rc = KEY_ESCAPE;	break;
      case XK_Return:	rc = KEY_ENTER;		break;
      case XK_Tab:	rc = KEY_TAB;		break;
      case XK_F1:	rc = KEY_F1;		break;
      case XK_F2:	rc = KEY_F2;		break;
      case XK_F3:	rc = KEY_F3;		break;
      case XK_F4:	rc = KEY_F4;		break;
      case XK_F5:	rc = KEY_F5;		break;
      case XK_F6:	rc = KEY_F6;		break;
      case XK_F7:	rc = KEY_F7;		break;
      case XK_F8:	rc = KEY_F8;		break;
      case XK_F9:	rc = KEY_F9;		break;
      case XK_F10:	rc = KEY_F10;		break;
      case XK_F11:	rc = KEY_F11;		break;
      case XK_F12:	rc = KEY_F12;		break;
	
      case XK_BackSpace:
      case XK_Delete:	rc = KEY_BACKSPACE;	break;

      case XK_Pause:	rc = KEY_PAUSE;		break;

      case XK_KP_Equal:
      case XK_equal:	rc = KEY_EQUALS;	break;

      case XK_KP_Subtract:
      case XK_minus:	rc = KEY_MINUS;		break;

      case XK_Shift_L:
      case XK_Shift_R:
	rc = KEY_RSHIFT;
	break;
	
      case XK_Control_L:
      case XK_Control_R:
	rc = KEY_RCTRL;
	break;
	
      case XK_Alt_L:
      case XK_Meta_L:
      case XK_Alt_R:
      case XK_Meta_R:
	rc = KEY_RALT;
	break;
	
      default:
	if (rc >= XK_space && rc <= XK_asciitilde)
	    rc = rc - XK_space + ' ';
	if (rc >= 'A' && rc <= 'Z')
	    rc = rc - 'A' + 'a';
	break;
    }

    return rc;

}
*/
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

void I_GetEvent(void)
{

  event_t event;
  // implement simple keyboard
  // poll events for joystick for mouse events
  // and keyboard
  // store in event structure
  // push to D_PostEvent()
}

//
// I_StartTic
//
void I_StartTic (void)
{
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

    static int	lasttic;
    int		tics;
    int		i;
    // UNUSED static unsigned char *bigscreen=0;

    // draws little dots on the bottom of the screen
    if (devparm)
    {
	i = I_GetTime();
	tics = i - lasttic;
	lasttic = i;
	if (tics > 20) tics = 20;

	for (i=0 ; i<tics*2 ; i+=2)
	    screens[0][ (SCREENHEIGHT-1)*SCREENWIDTH + i] = 0xff;
	for ( ; i<20*2 ; i+=2)
	    screens[0][ (SCREENHEIGHT-1)*SCREENWIDTH + i] = 0x0;
    }
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

  // set the X colormap entries
  for (i=0 ; i<256 ; i++)
    {
      vga_palette[i] = (gammatable[usegamma][*palette++] << 16);
      vga_palette[i] |= (gammatable[usegamma][*palette++] << 8);
      vga_palette[i] |= gammatable[usegamma][*palette++];
    }
}

//
// I_SetPalette
//
void I_SetPalette (byte* palette)
{
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

    screens[0] = vga;
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


