# include <stdio.h>
# include <math.h>
# include <string.h>
# include "complex.h"

# ifdef NEED_STRDUP
extern char *strdup ( );
# endif

double re(x)
   complex	x;
{
   return x.r;
}

double im(x)
   complex	x;
{
   return x.i;
}

complex negate(x)
   complex	x;
{
   complex	y;

   y.r = -x.r;
   y.i = -x.i;
 
   return y;
}

complex recip(x)
   complex	x; 
{
   complex	one;

   one.r = 1.0;
   one.i = 0.0;

   return cdiv(one, x); 
}
  
double modulus(x)
   complex	x;
{
   return sqrt(x.r*x.r + x.i*x.i);
}
 
complex mult (x, y)
   complex	x;
   complex	y;
{
   complex	a;

   a.r = x.r*y.r - x.i*y.i;
   a.i = x.i*y.r + x.r*y.i;

   return a;
}

complex add (x, y)
   complex	x;
   complex	y;
{
   complex	a;

   a.r = x.r + y.r;
   a.i = x.i + y.i;

   return a;
}

complex sub (x, y)
   complex	x;
   complex	y;
{
   complex	a;

   a.r = x.r - y.r;
   a.i = x.i - y.i;

   return a;
}

complex cdiv (x, y)
   complex	x;
   complex	y;
{
   complex	a;
   double	factor;

   a = mult (x, conj (y)); 
   factor = y.r*y.r + y.i*y.i; 
    
   a.r = a.r / factor;
   a.i = a.i / factor;

   return a;
}

complex csqrt (x)
   complex	x;
{
   complex	y;
   double	theta, radius;

   radius = sqrt(x.r*x.r + x.i*x.i);
   theta = atan2(x.i, x.r);

   y.r = sqrt(radius)*cos(theta/2);
   y.i = sqrt(radius)*sin(theta/2);

   return y;
}
 
complex cexp (x)
   double	x;
{
   complex	y;

   y.r = cos(x);
   y.i = sin(x);

   return y;
}

complex ccos (x)
   complex	x;
{
   complex	y;

   y = scale(cexp(x.r), exp(-x.i)*0.5, 0.0);
   y = add(y, scale(cexp(-x.r), exp(x.i)*0.5, 0.0));

   return y;
}

complex csin (x)
   complex	x;
{
   complex	y;
   complex	i2;

   i2.r = 0.0;
   i2.i = 2;

   y = scale(cexp(x.r), exp(-x.i), 0.0);
   y = sub(y, scale(cexp(-x.r), exp(x.i), 0.0));
   y = cdiv(y, i2);

   return y;
}

complex conj (x)
   complex	x;
{
   complex	a;
  
   a.r = x.r;
   a.i = -x.i;

   return a;
}

unsigned is_zero(x)
   complex	x;
{
   return (x.r == 0 && x.i == 0);
}

complex zero()
{
   complex	x;

   x.r = 0.0;
   x.i = 0.0;

   return x;
}

complex scale(x, factor, offset)
   complex	x;
   double	factor;
   double	offset;
{
   complex	y;

   y.r = x.r * factor + offset;
   y.i = x.i * factor + offset;

   return y;
}

char *cprint (x)
   complex	x;
{
   char		buffer [32];

   sprintf (buffer, "%7.5f %s %7.5fi  ", x.r, (x.i < 0 ? "-" : "+"), fabs (x.i));

   return strdup(buffer);
}
