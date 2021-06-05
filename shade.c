#include "types.h"
#include "toolbox.h"
#include <malloc.h>

/*
+-------------------------------------------------------------------------------
|
| Generic procedures for scene lighting.
|
+-------------------------------------------------------------------------------
*/

typedef struct LIGHT {	/* a light source */
  int IsSun;		/* flag; TRUE => source is a direction vector */
  VEC source;		/* flag == TRUE => direction vector, FALSE => point */
  COLOUR colour;	/* colour of light source */
  struct LIGHT *next;	/* pointer to next light source in list */
  } LIGHT;

LIGHT *Light0; /* pointer to first light in the linked list */
COLOUR Ambient = {0., 0., 0.};
COLOUR SkyColour = {0., 0., 1.};
COLOUR GrndColour = {1., 0., 0.};

/*
+-------------------------------------------------------------------------------
|
| A device to create a horizon, if the y component if ray is negative the ray
| hits the ground, if it is positive it shoots into the sky.
|
+-------------------------------------------------------------------------------
*/

void BackGrnd(colour, ray)
COLOUR colour;
RAY ray; {
  // Clearly not implemented since 1991 (-:
  colour[0] = 1.;
  colour[1] = 0.;
  colour[2] = 0.;
  }

/*
+----------------------------------- Shade() -----------------------------------
|
| Find the colour of the point isect, return it in coulour. Uses Intersect to
| find if the point is in a shadow.
| IsSun indicates a light source at infinite distance that bathes the scene with
| invariable intensity.
| N.B. Global function
|
+-------------------------------------------------------------------------------
*/

void Shade(colour, isect, Intersect)
COLOUR colour;
ISECT *isect;
int (*Intersect)(); {
  RAY ray;
  LIGHT *light;
  FLT distance, r;
  VEC M, normal;
  SURF *surface;
  PTR object;
  POINT point;

  /*
  The surface optics have to be stored for use in the reflection loop
  */

  VecCopy(normal, isect->normal);
  surface = isect->surface;
  object = isect->object;
  VecCopy(point, isect->point);

  colour[0] = colour[1] = colour[2] = 0.;
  light = Light0;

  for (;light != NULL;) { /* for all the lights in the scene... */

    /*
    Create a ray that points to the light source
    */

    VecCopy (ray.Pos, point);
    if (light -> IsSun) {
      VecCopy (ray.Dir, light -> source);
      }
    else {
      VecSub (M, light -> source, point);
      distance = VecUnit(ray.Dir, M);
      }

    if ((*Intersect)(isect, &ray) == 0) {
      if ((r = VecDot(ray.Dir, normal)) < 0.) {
        r = 0.;
        }
      if (light -> IsSun == 0) {
        r /= distance * distance;
        }

      VecAddS(colour, r, light -> colour, colour);
      }
    light = light -> next;
    isect->object = object;
    }

/*  VecAddS(colour, surface->kdiff, colour, Ambient);*/
/*  VecAdd(colour, colour, Ambient);*/
  }

/*
+--------------------------------- AddLight() ----------------------------------
|
| Add a light source to the linked list.
| The light vector is oriented toward the light source.
| If isasun is TRUE source is a light direction vector, light intensity constant
| If isasun is FALSE source is a point, light intensity dependent on distance
| from source
| N.B. Global function
|
+-------------------------------------------------------------------------------
*/

int AddLight(isasun, source, colour)
int isasun;
VEC source;
COLOUR colour; {

  LIGHT *light = NULL;
  static LIGHT *lastlight = NULL;

  if ((light = (LIGHT *) calloc (sizeof(LIGHT),1)) == NULL)
    return (FALSE);
  if (lastlight == NULL)
    Light0 = light;
  else
    lastlight -> next = light;

  light -> next = NULL;
  light -> IsSun = isasun;
  if (isasun)
    VecUnit(light -> source, source);
  else
    VecCopy(light -> source, source);
  VecCopy(light -> colour, colour);
  lastlight = light;
  return (TRUE);
  }

/*============================================================================*/
