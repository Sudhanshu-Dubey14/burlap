/*
    This file is part of the FElt finite element analysis package.
    Copyright (C) 1993-1997 Jason I. Gobat and Darren C. Atkinson

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

/******************************************************************************
 *
 * File:	util.c
 *
 * Description:	contains utility routines for format conversion
 *
 *****************************************************************************/

# include <math.h>
# include <stdio.h>
# include <string.h>
# include <malloc.h>
# include "fe.h"
# include "problem.h"
# include "objects.h"
# include "Tree.h"
# include "patchwork.h"

# define INITIAL_NODES		50
# define INITIAL_ELEMENTS	50

static int	max_nodes;
static int	max_elements;

int InitializeProblem ( )
{
   ReadFeltFile (NULL);

   problem.nodes = (Node *) malloc (sizeof (Node) * INITIAL_NODES);
   problem.elements = (Element *) malloc (sizeof (Element) * INITIAL_ELEMENTS);
   problem.nodes --;
   problem.elements --;

   max_nodes = INITIAL_NODES;
   max_elements = INITIAL_ELEMENTS;

   return 0;
}

Node AddNode (x, y, z, constraint, force)
   double	x;
   double	y;
   double	z;
   Constraint	constraint;
   Force	force;
{
   Node		node;

   node = CreateNode (++ problem.num_nodes);

   node -> x = x;
   node -> y = y;
   node -> z = z;

   node -> force = force;
   node -> constraint = constraint;

   if (problem.num_nodes > max_nodes) {
      max_nodes += 50;
      problem.nodes = (Node *) realloc (problem.nodes + 1, 
                                        sizeof (Node) * max_nodes);
      problem.nodes --;
   }

   problem.nodes [problem.num_nodes] = node;

   if (force) 
      TreeInsert (problem.force_tree, (Item) force);

   if (constraint)
      TreeInsert (problem.constraint_tree, (Item) constraint);

   return node;
}

Element AddElement (defn, nodes, material, distributed, numdistributed)
   Definition	defn;
   Node		*nodes;
   Material	material; 
   Distributed	*distributed;
   unsigned	numdistributed;
{
   unsigned	i;
   Element	element;

   element = CreateElement (++ problem.num_elements, defn);
   for (i = 1 ; i <= defn -> numnodes ; i++)
      element -> node [i] = nodes [i-1]; 

   element -> material = material;

   for (i = 1 ; i <= numdistributed ; i++) {
      element -> distributed [i] = distributed [i-1];
      TreeInsert (problem.distributed_tree, distributed [i-1]);
   }

   if (problem.num_elements > max_elements) {
      max_elements += 50;
      problem.elements = (Element *) realloc (problem.elements + 1, 
                                        sizeof (Element) * max_elements);
      problem.elements --;
   }

   problem.elements [problem.num_elements] = element;

   if (material) 
      TreeInsert (problem.material_tree, (Item) material);

   return element;
}