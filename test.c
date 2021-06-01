#include "types.h"
#include "toolbox.h"
#include <stdio.h>
#include <stdlib.h> // calloc malloc(3)
#include <unistd.h> // write(2)

/*
+-------------------------------------------------------------------------------
|
| Test the Trace() routine.
|
+-------------------------------------------------------------------------------
*/

void *Physbase();
void pixel();
void write_pbm_p4();
void PrtTriangle();

int main() {
  extern int Focal, Width, Height; // value loaded from input data file

  int x, y;		/* Screen coordinates */
  RAY ray;		/* Primary ray */
  COLOUR colour;
  unsigned char monochrome;
  FLT t;		/* result of Trace() */
  VEC direction;	/* Direction of the primary ray */

  FILE *fp;

  int X0;
  int Y0;
  int line;

  get_obj("box.obj");

  X0 = Width >> 1;
  Y0 = Height >> 1;

  fprintf(stderr,"loaded object data\n");
  fprintf(stderr,"Focal = %d Width = %d Height = %d\n",Focal, Width, Height);

  fprintf(stdout,
    "P7\n"
    "WIDTH %d\n"
    "HEIGHT %d\n"
    "DEPTH 1\n"
    "MAXVAL 64\n"
    "TUPLTYPE GRAYSCALE\n"
    "ENDHDR\n",
    Width,Height);

  for (y = Height ; --y >= 0 ;) {
    for (x = 0 ; x < Width; x++) {

      direction[0] = (FLT)(x - X0);
      direction[1] = (FLT)(y - Y0);
      direction[2] = (FLT)Focal;

      VecUnit(ray.Dir, direction);

      t = Trace(colour, &ray);

      monochrome = (unsigned char)(colour[0]+colour[1]+colour[2]);

      fwrite(&monochrome,sizeof(unsigned char),1,stdout);
      }
    }
  PrtTriangle();

  }

/*======================== E N D   O F   L I S T I N G =======================*/
