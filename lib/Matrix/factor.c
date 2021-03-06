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
 * File: 	factor.c
 *	
 * Description:	
 *		
 ************************************************************************/

# include <stdio.h>
# include <math.h>
# include <stdlib.h>
# include "matrix.h"
# include "error.h"

# define SIGN(x) ((x) < 0 ? -1 : 1)

static void HouseHolderRow (a, v, j, m, n, w)
   Matrix	a;
   double	*v;
   unsigned	m, n, j;
   double	*w;
{
   unsigned	i, k;
   double	beta;
   double	dot;

   dot = 0;
   for (i = j ; i <= m ; i++)
      dot += v[i]*v[i];

   beta = -2.0/dot; 

   for (i = j ; i <= n ; i++) {
      w [i] = 0;
      for (k = j ; k <= m ; k++) 
         w [i] += v[k]*mdata(a, k, i);

      w [i] *= beta;
   }

   for (i = j ; i <= m ; i++) 
      for (k = j ; k <= n ; k++)
         sdata(a,i,k) += v[i]*w[k];
 
   return;
}

static void HouseHolder (v, a, j, m)
   double	*v;
   Matrix	a;
   unsigned	j, m; 
{
   unsigned	i;
   double	mu, beta;

   mu = 0;
   for (i = j ; i <= m ; i++) {
      v[i] = mdata(a, i, j);
      mu += v[i]*v[i];
   }

   mu = sqrt(mu);

   if (mu != 0) {
      beta = mdata(a, j, j) + SIGN(mdata(a, j, j))*mu;
      for (i = j+1 ; i <= m ; i++)
         v[i] /= beta;
   }

   v[j] = 1; 

   return;
}

int QRFactorMatrix (q, r, a)	/* factor a such that qTa = r		*/
   Matrix	q;		/* destination matrix for q		*/
   Matrix	r;		/* destination matrix for r		*/
   Matrix	a;		/* source matrix			*/
{
   unsigned	i, j;
   unsigned	m , n;
   double	*v, *w;
   int		status;

   if (IsCompact(q) || IsCompact(r))
      return M_COMPACT;

   if (Mrows(a) < Mcols(a))
      return M_NOTSQUARE;

   if (Mrows(a) != Mrows(r) || Mrows(r) != Mrows(q))
      return M_SIZEMISMATCH;

   if (Mcols(a) != Mcols(r) || Mcols(r) != Mcols(q))
      return M_SIZEMISMATCH;

   if (a == q)		/* can overwrite a with r but not with q */
      return M_NOOVERWRITE;

   if (a != r) {
      status = CopyMatrix (r, a);
      if (status)
         return status;
   }

   status = IdentityMatrix (q);
   if (status)
      return status;

   m = Mrows(r);
   n = Mcols(r);

   v = (double *) malloc (sizeof(double) * m);
   if (v == NULL)
	Fatal ("unable to allocate temporary vector");
   v --;
   w = (double *) malloc (sizeof(double) * m);
   if (w == NULL)
	Fatal ("unable to allocate temporary vector");
   w --;

   for (j = 1 ; j <= n ; j++) {
      HouseHolder (v, r, j, m);
      HouseHolderRow (r, v, j, m, n, w);       
      
      if (j < m) {
         for (i = j+1 ; i <= m ; i++)
            sdata(r,i,j) = v[i];
      }
   }

   for (j = n ; j >= 1 ; j--) {
      v [j] = 1;
      for (i = j+1 ; i <= m ; i++)
         v [i] = mdata(r, i, j);

      HouseHolderRow (q, v, j, m, m, w);
   }

   v++; free(v);
   w++; free(w);

   return 0;
}

int CholeskyFactorMatrix (b, a)	/* factor a such that bbT=a	*/
   Matrix	b;			/* destination matrix		*/
   Matrix	a;			/* source matrix		*/
{
   unsigned	i, j, k;
   unsigned	n;
   double	t;
   int		status;

   if (IsCompact(b))
      return M_COMPACT;

   if (Mrows(a) != Mcols(a))
      return M_NOTSQUARE;

   if (Mrows(a) != Mrows(b) || Mcols(a) != Mcols(b))
      return M_SIZEMISMATCH;

   if (!IsSymmetricMatrix (a))
      return M_NOTSYMMETRIC;

   if (a != b) {
      status = CopyMatrix (b, a);
      if (status)
         return status;
   }

   n = Mrows(b);

   for (k = 1 ; k <= n ; k++) {
      t = mdata(b, k, k);
      if (t <= 0)
         return M_NOTPOSITIVEDEFINITE;
 
      t = sqrt (t);
      sdata(b,k,k) = t;

      for (j = k+1 ; j <= n ; j++)
         sdata(b,k,j) /= t;
/*
         sdata(b,j,k) /= t;
*/

      for (j = k+1 ; j <= n ; j++) {
         t = sdata(b,k,j);
/*
         t = sdata(b,j,k); 
*/
         for (i = j ; i <= n ; i++) 
            sdata(b,i,j) -= t*mdata(b, k, i);
/*
            sdata(b,i,j) -= t*mdata(b, i, k);
*/
      }
   }

   for (i = 1 ; i <= n ; i++)
      for (j = i+1 ; j <= n ; j++)
         sdata(b,j,i) = 0.0;
/*
         sdata(b,i,j) = 0.0;
*/

   return 0;
}

int InvertMatrix (b, a, p)	/* compute b = inv(a) where a is LU factored */
   Matrix	b;		/* destination matrix			     */
   Matrix	a;		/* factored source matrix  		     */
   Matrix	p;		/* pivot vector				     */
{
   unsigned	i, j;
   unsigned	n;
   Matrix	work;

   if (IsCompact(b))
      return M_COMPACT;

   if (Mrows(a) != Mcols(a))
      return M_NOTSQUARE;

   if (Mrows(a) != Mrows(p) || Mcols(p) != 1)
      return M_SIZEMISMATCH; 

   if (Mrows(a) != Mrows(b) || Mcols(a) != Mcols(b))
      return M_SIZEMISMATCH;

   if (a == b) 
      return M_NOOVERWRITE;

   n = Mrows(a);
   work = CreateColumnVector (n);

   for (i = 1 ; i <= n ; i++) {
      ZeroMatrix (work);
      sdata(work, i, 1) = 1.0;
      LUBackSolveMatrix(work, a, work, p);
      for (j = 1 ; j <= n ; j++)
         sdata(b,j,i) = mdata(work,j,1);
   }
   
   DestroyMatrix (work);
   return 0;
}

int DeterminantMatrix (result, a, p)	/* result = |a|			*/
   double	*result;		/* pointer to result location	*/
   Matrix	a;			/* factorized source matrix	*/
   Matrix	p;			/* pivot vector			*/
{
   unsigned	i;
   double	x, y;
   unsigned	n;

   if (Mrows(a) != Mcols(a))
      return M_NOTSQUARE;

   if (Mrows(a) != Mrows(p) || Mcols(p) != 1)
      return M_SIZEMISMATCH;

   n = Mrows(a);

   x = 1.0;
   y = 0.0;
   for (i = 1 ; i <= n ; i++) {
      if ((int) mdata(p, i, 1) != i) 
         x = -x;

      x *= mdata(a, i, i);
      if (x != 0) {
         while (fabs (x) < 1.0) {
            x *= 10.0;
            y -= 1.0;
         }

         while (fabs (x) >= 10.0) {
            x /= 10.0;
            y += 1.0;
         }
      }
      else
         break;
   }

   *result = x*pow(10.0, y);
         
   return 0;
}

int LUFactorMatrix (b, a, p, info) /* factor a into LU and store in b 	*/
   Matrix	b;		    /* destination matrix		*/
   Matrix 	a;	  	    /* source matrix			*/
   Matrix 	p; 		    /* permutation vector		*/
   int		*info;		    /* singularity code			*/
{
   double	t;
   unsigned	i, j, k;
   unsigned	n;
   double	max;
   int		status;
   unsigned	mu;

   if (IsCompact(b))
      return M_COMPACT;

   if (Mrows(a) != Mcols(a))
      return M_NOTSQUARE;

   if (Mrows(a) != Mrows(p))
       return M_SIZEMISMATCH;

   if (Mrows(a) != Mrows(b) || Mcols(a) != Mcols(b))
      return M_SIZEMISMATCH;

   if (a != b) {
      status = CopyMatrix (b, a);
      if (status)
         return status;
   }

   *info = 0;

   n = Mrows(b);

   for (j = 1 ; j <= n ; j++)
      sdata(p, j, 1) = j;

   for (j = 1 ; j <= n ; j++) {

      for (k = 1 ; k <= j-1 ; k++) { 
         mu = mdata(p, k, 1);
         if (mu != k) {
            t = mdata(b, k, j);
            sdata(b,k,j) = mdata(b,mu,j);
            sdata(b,mu,j) = t;
         }
      }

      for (k = 1 ; k <= j-1 ; k++) 
         for (i = k+1 ; i <= j-1 ; i++) 
            sdata(b,i,j) = mdata(b,i,j) - mdata(b,i,k)*mdata(b,k,j);

      for (k = 1 ; k <= j-1 ; k++)
         for (i = j ; i <= n ; i++)
            sdata(b,i,j) = mdata(b,i,j) - mdata(b,i,k)*mdata(b,k,j);

      mu = j;
      max = fabs (mdata(b,j,j));
      for (k = j ; k <= n ; k++) {
         if ((t = fabs (mdata(b,k,j))) > max) {
            max = t;
            mu = k;
         }
      } 

      sdata(p,j,1) = mu;

      if (j != mu) {
         for (k = 1 ; k <= j ; k++) {
            t = mdata(b, j, k);
            sdata(b,j,k) = mdata(b,mu,k);
            sdata(b,mu,k) = t;
         }
      }

      if (mdata(b,j,j) != 0) {
         t = mdata(b, j, j);
         for (i = j+1 ; i <= n ; i++)
            sdata(b,i,j) = mdata(b, i, j) / t;
      }
      else
         *info = j;
   }
                  
   return 0; 
}

int FormLUPMatrices (L, U, P, a, p)	/* separate matrices from factors    */
   Matrix	L;			/* return matrix for L		     */
   Matrix	U;			/* return matrix for U		     */
   Matrix	P;			/* return matrix for P		     */
   Matrix	a;			/* factored form of matrix	     */
   Matrix	p;			/* pivot vector			     */
{
   unsigned	i, j;
   unsigned	n;
   unsigned	k_pvt;
   unsigned	temp;
   unsigned	*pivot;

   if (IsCompact(L) || IsCompact(U) || IsCompact(P))
      return M_COMPACT;

   if (!IsSquare(a))
      return M_NOTSQUARE;

   if (Mrows(a) != Mrows(p) || Mcols(p) != 1)
      return M_SIZEMISMATCH;

   if (Mrows(a) != Mrows(L) || Mrows(a) != Mrows(U) || Mrows(a) != Mrows(P))
      return M_SIZEMISMATCH;

   if (Mcols(a) != Mcols(L) || Mcols(a) != Mcols(U) || Mcols(a) != Mcols(P))
      return M_SIZEMISMATCH;

   n = Mrows(a);

   pivot = (unsigned *) malloc (sizeof (unsigned) * n); pivot --;

   for (i = 1 ; i <= n ; i++) 
      pivot [i] = i;

   for (i = 1 ; i < n ; i++) {
      k_pvt = (unsigned) mdata(p,i,1);

      if (k_pvt != i) {
         temp = pivot [k_pvt];
         pivot [k_pvt] = pivot [i];
         pivot [i] = temp;
      }
   }

   ZeroMatrix (P);
   ZeroMatrix (L);
   ZeroMatrix (U);

   for (i = 1 ; i <= n ; i++) {
      sdata(P,i,pivot[i]) = 1.0;

      sdata(L,i,i) = 1.0;
      for (j = 1 ; j < i ; j++)
         sdata(L,i,j) = mdata(a,i,j);

      for (j = i ; j <= n ; j++)
         sdata(U,i,j) = mdata(a,i,j);
   }

   pivot ++; free (pivot);

   return 0;
}

int LUBackSolveMatrix (c, a, b, p)	/* solve Ax=b and store result in c  */	
   Matrix	c;			/* destination vector		     */
   Matrix	a;			/* factorized source matrix          */	
   Matrix	b;			/* right hand side vector 	     */
   Matrix	p;			/* pivot vector			     */
{
   unsigned	i, k;
   unsigned	n;
   int		mu;
   double	t;
   int		status;

   if (Mrows(a) != Mcols(a))
      return M_NOTSQUARE;

   if (Mcols(b) != 1 || Mcols(p) != 1 || Mcols(c) != 1)
      return M_SIZEMISMATCH;

   if (Mrows(a) != Mrows(p) || Mrows(a) != Mrows(b) || Mrows(b) != Mrows(c))
      return M_SIZEMISMATCH; 

   if (b != c) {
      status = CopyMatrix (c, b);
      if (status)
         return status;
   }

   n = Mrows(a);

	/*
	 * pivot the RHS vector to form x = Pb
	 */

   for (i = 1 ; i <= n ; i++) {
      mu = (int) mdata(p, i, 1);
      if (mu != i) {
         t = mdata(c, i, 1);
         sdata(c,i,1) = mdata(c, mu, 1);
         sdata(c,mu,1) = t;
      }
   }

	/*
	 * Solve Ly = x and store y in c
	 */

   for (k = 1 ; k <= n ; k++) {
      if (mdata(c, k, 1) != 0) {
         for (i = k+1 ; i <= n ; i++) 
            sdata(c,i,1) = mdata(c, i, 1) - mdata(c, k, 1)*mdata(a, i, k);
      }
   }

	/*
	 * Solve Uc = y
	 */

   for (k = n ; k >= 1 ; k--) {
      if (mdata(c, k, 1) != 0) 
         sdata(c,k,1) = mdata(c, k, 1)/mdata(a, k, k);

      for (i = 1 ; i <= k-1 ; i++)
        sdata(c,i,1) = mdata(c, i, 1) - mdata(c, k, 1)*mdata(a, i, k);
   }
   
   return 0;
}

int CroutFactorMatrix (A)	/* crout factorize A and store in A	*/
   Matrix	A;		/* source and destination matrix	*/
{
   unsigned     j,jj,jjlast,jcolht,
          	istart,ij,ii,i,
          	icolht,iilast,
          	length,jtemp,jlngth;
   double 	temp, dot;
   unsigned	n, k;

   if (IsFull(A))
      return M_NOTCOMPACT;
 
   if (Mrows(A) != Mcols(A))
      return M_NOTSQUARE;
  
   if (IsSubsection (A))
      return M_SUBSECTION;

   n = Mrows(A);

   jj = 0;
   for (j = 1; j <= n; j++) {

      jjlast = jj;
      jj = A -> diag [j];
      jcolht = jj - jjlast;

      if (jcolht > 2) {
         
         istart = j - jcolht + 2;
         ij = jjlast + 2;
         ii = A -> diag [istart-1];

         for (i = istart; i <= j - 1 ; i++) {

            iilast = ii;
            ii = A -> diag [i];
            icolht = ii - iilast;
            jlngth = i - istart + 1;
            if (icolht - 1  < jlngth) 
               length = icolht - 1;
            else
               length = jlngth;
            
            if (length > 0) {
               dot = 0;
               for (k = 0 ; k < length ; k++)
                  dot += A -> data [ii-length+k][1] * A -> data[ij-length+k][1];

               A -> data [ij][1] -= dot;
            }

            ij++;
         }
      }

      if (jcolht >= 2) {

         jtemp = j - jj;
         for (ij = jjlast+1 ; ij <= jj-1 ; ij++) {

            ii = A -> diag [jtemp + ij];
           
            if (A -> data [ii][1] != 0.0) {
               temp = A -> data [ij][1];
               A -> data [ij][1] = temp / A -> data [ii][1];
               A -> data [jj][1] -= temp*A -> data [ij][1];
            }
         }
      }
   }

   return 0;
}

int CroutBackSolveMatrix (A, b)	/* solve Ax=b and store x in b	*/
   Matrix   	 A;			/* Crout factored LHS matrix	*/
   Matrix   	 b;			/* RHS (and dest) vector	*/
{
   unsigned	 jj,j,jjlast,
		 jcolht,jjnext,
          	 istart,jtemp,i;
   double 	 Ajj;
   unsigned	 n, k;
   double	 dot;

   if (IsFull(A))
      return M_NOTCOMPACT;

   if (Mrows(A) != Mcols(A))
      return M_NOTSQUARE;
    
   if (Mrows(A) != Mrows(b))
      return M_SIZEMISMATCH;

   if (IsSubsection (A) || IsSubsection (b))
      return M_SUBSECTION;

   n = Mrows(A);

   jj = 0;
   for (j = 1 ; j <= n ; j++) {

      jjlast = jj;
      jj = A -> diag [j];
      jcolht = jj - jjlast;

      if (jcolht > 1) {
         dot = 0;
         for (k = 0 ; k < jcolht-1 ; k++)
            dot += A -> data [jjlast+1+k][1] * b -> data [j-jcolht+1+k][1];

         b -> data [j][1] -= dot;
      }
   }

   for (j = 1 ; j <= n ; j++) {
      Ajj = A -> data [A -> diag[j]][1];
      if (Ajj != 0.0)
         b -> data [j][1] /= Ajj;
   }

   if (n == 1)
      return 0;

   jjnext = A -> diag [n];

   for (j = n ; j >= 2 ; j--) {

      jj = jjnext;
      jjnext = A -> diag [j-1];
      jcolht = jj - jjnext;
      if (jcolht > 1) {

          istart = j - jcolht + 1;
          jtemp = jjnext - istart + 1;

          for (i = istart ; i <= j-1 ; i++) 
             b -> data [i][1] -= A -> data [jtemp + i][1] * b -> data[j][1];
      }
   }

   return 0;
}
