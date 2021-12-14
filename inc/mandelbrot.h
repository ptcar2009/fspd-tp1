#ifndef MANDELBROT_H
#define MANDELBROT_H
#include <stdio.h>
/**
 * @brief Parameter for a call to the callculation of an area of the mandelbrot set.
 * 
 */
typedef struct {
  int left;
  int low; // lower left corner in the screen
  int ires;
  int jres; // resolution in pixels of the area to compute
  double xmin;
  double ymin; // lower left corner in domain (x,y)
  double xmax;
  double ymax; // upper right corner in domain (x,y)

} fractal_param_t;

/**
 * @brief takes the information to a fractal_param_t from input.
 * 
 * @param p Allocated fractal_param_t which will contain the information recovered.
 * @param input File from which the information will be recovered. Can be stdin.
 * @return int EOF on EOF.
 */
int input_params(fractal_param_t *p, FILE* input);

/**
 * @brief Performs the computation needed to calculate the pixels in the area described by p.
 * 
 * @param p Description of the calculations that need to be done. 
 */
void fractal(fractal_param_t *p);

/**
 * @brief Maximum number of iterations before a point is considered converging.
 * 
 */
#define MAXITER 32768

#endif // MANDELBROT_H
