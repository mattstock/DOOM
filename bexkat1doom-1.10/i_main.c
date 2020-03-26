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
//	Main program, simply calls D_DoomMain high level loop.
//
//-----------------------------------------------------------------------------

static const char
rcsid[] = "$Id: i_main.c,v 1.4 1997/02/03 22:45:10 b1 Exp $";

#include "vectors.h"
#include "ff.h"
#include "timers.h"

extern void disk_timerproc(void);
extern void init_vectors(void);

INTERRUPT_HANDLER(timer3)
static void timer3(void) {
  disk_timerproc();
  timers[1] = 0x8; // clear the interrupt
  timers[7] += 500000; // reset timer3 interval
}

#include "doomdef.h"

#include "d_main.h"

void DebugPrint(char *str) {
  static unsigned int stamp;

  printf("[%8u] %s", timers[12] - stamp, str);
  stamp = timers[12];
}

int
main
( int		argc,
  char**	argv ) 
{
  cli();
  init_vectors();
  timers[7] = timers[12] + 500000; // 100Hz
  set_interrupt_handler(intr_timer3, timer3);
  timers[0] |= 0x88; // enable timer and interrupt
  sti();
  
  D_DoomMain (); 
  
  return 0;
} 
