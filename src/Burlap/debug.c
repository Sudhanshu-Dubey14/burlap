/*
    This file is part of the FElt finite element analysis package.
    Copyright (C) 1993-2000 Jason I. Gobat and Darren C. Atkinson

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

/************************************************************************
 * File:	debug.c							*
 *									*
 * Description:	This file contains the function defintions for		*
 *		debugging.						*
 ************************************************************************/

# include <stdio.h>
# include "debug.h"
# include VAR_ARGS_INCLUDE


int debug;

/************************************************************************
 * Function:	eprintf							*
 *									*
 * Description:	Prints a message specified as a format string and	*
 *		arguments to the standard error stream.			*
 ************************************************************************/

# ifdef UseFunctionPrototypes
void eprintf (char *format, ...)
# else
void eprintf (format, va_alist)
    char *format;
    va_dcl
# endif
{
    va_list ap;


    VA_START (ap, format);
    vfprintf (stderr, format, ap);
    va_end (ap);
}
