#    This file is part of the FElt finite element analysis package.
#    Copyright (C) 1993-2000 Jason I. Gobat and Darren C. Atkinson
#
#    This program is free software; you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation; either version 2 of the License, or
#    (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program; if not, write to the Free Software
#    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

CFLAGS	= $(CCOPTS) -I$(TOPDIR)/include
LIB	= libgen.a
OBJS	= brick_grid.o coalesce.o grid.o line.o quad_grid.o rules.o \
          trimesh.o triangle.o

CURDIR	= ./lib/Generate
TOPDIR	= ../..

include $(TOPDIR)/etc/Makefile.lib

triangle.o:	triangle.c triangle.h
		$(CC) $(CCOPTS) -DTRILIBRARY -DREDUCED -c triangle.c -o triangle.o
