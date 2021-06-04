#include "types.h"
#include "toolbox.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h> // exit

POINT rotvert[1024];
MAT ViewMatrix;
SURF Surface[64];


int Focal, Width, Height;
FLT Gamma = 1.0/2.2; // television default

/*----------------------------------------------------------------------------*/

void AddVert (point)
POINT point; {
  static int index;

/*  MatVec(rotvert[index++], ViewMatrix, point); */

  VecCopy(rotvert[index++], point);
  }

void AddSurf(kdiff, colour)
FLT kdiff;
COLOUR colour; {
  static int index;

  fprintf(stderr,"kdiff %f\ncolour %lx\n",kdiff,(unsigned long)colour);

  if ( index >= 0 && index < 64 ) {
    Surface[index].kdiff = kdiff;
    fprintf(stderr,"Surface %lx, %lu\n",(unsigned long)Surface,sizeof(Surface));
    VecCopy(Surface[index].colour, colour);
    index++; // was missing in 1991 version!
    }
  else {
    fprintf(stderr,"EXIT: index out of bounds.\n");
    exit(-1);
    }
  }

void rem(fp)
FILE *fp; {
  char c;
  for (;;) {
    c = fgetc (fp);
    if (c == EOF || c == '\n')
      break;
    }
  }

int get_obj (filename)
char *filename; {
  FILE *fp, *fopen();
  int fclose();
  char c;
  int p, q, r, s;
  FLT kdiff;
  int zerovert = 0;
  int nvertex = 0;
  POINT origin;
  VEC rotate;
  MAT matrix;
  POINT point, vertex;
  COLOUR colour;

  /* load the object file */
  if ((fp = fopen (filename,"r")) == NULL) {
    fprintf(stderr,"ERROR: Can't open the object file\n");
    return (-1);
    }

  origin[0] = origin[1] = origin[2] = 0.;
  rotate[0] = rotate[1] = rotate[2] = 0.;
  Gamma = 1.0/2.2;

  SetRotation(matrix, rotate);

  while ((c = fgetc (fp)) != EOF) {
    switch (c) {
      case '\n':
        break;
      case 'g':
        fscanf (fp, "%f", &Gamma);
        rem (fp);
        fprintf(stderr,"Gamma = %f\n",Gamma);
        Gamma = 1.0/Gamma;
        break;
      case 'f':
        /* focal distance, width and height of image, in pixel units */
        fscanf (fp, "%d %d %d", &Focal, &Width, &Height);
        rem (fp);
        fprintf(stderr,"Focal = %d, Width = %d, Height = %d\n",
          Focal, Width, Height);
        break;
      case 'v':
        /* vertex data follows */
        fscanf (fp, "%f %f %f", point, point+1, point+2);
        MatVec(point, matrix, point);
        VecSub(point, point, origin);
        AddVert(point);
        nvertex++;
        rem (fp);
        fprintf(stderr,"Point %f %f %f\n", point[0], point[1], point[2]);
        break;
      case 't':
        /* triangle data follows */
        fscanf (fp, "%d %d %d %d", &p, &q, &r, &s);
        AddTriangle (rotvert, p + zerovert, q + zerovert, r + zerovert,
          Surface+s);
        rem (fp);
        fprintf(stderr,"triangle %d %d %d %d\n",p,q,r,s);
        break;
      case 'o':
        /* change origin coordinates */
        fscanf (fp, "%f %f %f", origin, origin+1, origin+2);
        rem (fp);
        fprintf(stderr,"origin %f %f %f\n", origin[0], origin[1], origin[2]);
        break;
      case 'r':
        /* change rotation of subsequent vertices */
        fscanf (fp, "%f %f %f", rotate, rotate+1, rotate+2);
        rotate[0] *= M_PI/180.;
        rotate[1] *= M_PI/180.;
        rotate[2] *= M_PI/180.;
        SetRotation(matrix, rotate);
        rem(fp);
        fprintf(stderr,"rotate %f %f %f\n", rotate[0], rotate[1], rotate[2]);
        break;
      case 'z':
        /* zero the vertex counter */
        zerovert = nvertex;
        rem(fp);
        fprintf(stderr,"zero vertex counter\n");
        break;
      case 's':
        /* Sun */
        fscanf (fp, "%f %f %f %f %f %f",
          point, point+1, point+2,
          colour, colour+1, colour+2);
        AddLight(1, point, colour);
        rem(fp);
        fprintf(stderr,"Sun light %f %f %f %f %f %f\n",
          point[0], point[1], point[2],
          colour[0], colour[1], colour[2]);
        break;
      case 'l':
        /* Lamp */
        fscanf (fp, "%f %f %f %f %f %f",
          point, point+1, point+2,
          colour, colour+1, colour+2);
        AddLight(0, point, colour);
        rem(fp);
        fprintf(stderr,"Lamp light %f %f %f %f %f %f\n",
          point[0], point[1], point[2],
          colour[0], colour[1], colour[2]);
        break;
      case 'u':
        /* surface */
        fscanf (fp, "%f %f %f %f",
          &kdiff, colour, colour+1, colour+2);
        fprintf(stderr,"surface %f %f %f %f\n",
          kdiff, colour[0], colour[1], colour[2]);
        AddSurf(kdiff,colour);
        rem(fp);
        break;
      default:
        /* unrecognised character, the whole line is a comment */
        rem(fp);
        break;
      }
    }
  fclose(fp);
  return (0);
  }

/*============================================================================*/


void write_pbm_p4(filename,x,y,data)
char *filename;
int x, y;
char *data;{
  int line, length;
  FILE *out;
  if ((out = fopen(filename,"wb")) != NULL) {
    fprintf(out,"P4\n%d %d ",x,y);
    line = x >> 3;
    if (x & 7) line++;
    length = line * y;
    fwrite(data,sizeof(char),length,out);
    fclose(out); }
  else fprintf(stderr,"Can't open %s\n",filename);
}
