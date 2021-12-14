#include "mandelbrot.h"
#include <stdio.h>
#include <stdlib.h>
int input_params(fractal_param_t *p, FILE *input)
{
  int n;
  n = fscanf(input, "%d %d %d %d", &(p->left), &(p->low), &(p->ires),
             &(p->jres));
  if (n == EOF)
    return n;

  if (n != 4)
  {
    perror("fscanf(left,low,ires,jres)");
    exit(-1);
  }
  n = fscanf(input, "%lf %lf %lf %lf", &(p->xmin), &(p->ymin), &(p->xmax),
             &(p->ymax));
  if (n != 4)
  {
    perror("scanf(xmin,ymin,xmax,ymax)");
    exit(-1);
  }
  return 8;
}

void fractal(fractal_param_t *p)
{
  double dx, dy;
  int i, j, k;
  double x, y, u, v, u2, v2;

  dx = (p->xmax - p->xmin) / p->ires;
  dy = (p->ymax - p->ymin) / p->jres;

  // scanning every point in that rectangular area.
  // Each point represents a Complex number (x + yi).
  // Iterate that complex number
  for (j = 0; j < p->jres; j++)
  {
    for (i = 0; i <= p->ires; i++)
    {
      x = i * dx + p->xmin; // c_real
      u = u2 = 0;           // z_real
      y = j * dy + p->ymin; // c_imaginary
      v = v2 = 0;           // z_imaginary

      // Calculate whether c(c_real + c_imaginary) belongs
      // to the Mandelbrot set or not and draw a pixel
      // at coordinates (i, j) accordingly
      // If you reach the Maximum number of iterations
      // and If the distance from the origin is
      // greater than 2 exit the loop
      for (k = 0; (k < MAXITER) && ((u2 + v2) < 4); ++k)
      {
        // Calculate Mandelbrot function
        // z = z*z + c where z is a complex number

        // imag = 2*z_real*z_imaginary + c_imaginary
        v = 2 * u * v + y;
        // real = z_real^2 - z_imaginary^2 + c_real
        u = u2 - v2 + x;
        u2 = u * u;
        v2 = v * v;
      }
    }
  }
}