#include "types.h"
#include "toolbox.h"
//#include <malloc.h> /* deprecated! */
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
//#include <values.h>
//#include <macros.h> /* Atari ST only? */
//#include <osbind.h>/* Atari ST only */

/*
+-------------------------------------------------------------------------------
|
| Generic procedures on triangle primitives
|
+-------------------------------------------------------------------------------
*/

typedef struct TRIANGLE { /* Triangle object structure */
  FLT *A, *B, *C;	/* pointers to vertices */
  VEC normal;		/* unitised normal vector of the triangle */
  VEC NxAB, NxBC, NxCA;	/* precompute for TriInOut */
  FLT D;		/* distance component for the plane equation */
  struct TRIANGLE *next; /* pointer to next triangle in linked list */
  struct SURF *surface;	/* pointer to a surface structure */
  } TRIANGLE;

TRIANGLE *Tri0 = NULL; /* pointer to first triangle in linked list */

/*
+--------------------------------- RayPlane() ----------------------------------
|
| intersection of a ray with a plane, resulting point coordinates in _pnt_,
| distance in _dist_.
| Returns 1 for a hit that's seen (normal facing), -1 for a hit not seen
| (normal not facing), 0 for a ray that is parallel to the plane.
|
| N.B. Local function
|
+-------------------------------------------------------------------------------
*/

static int RayPlane(POINT pnt, FLT *dist, RAY *ray, TRIANGLE *tri) {
  FLT vd;
  int r;
  FLT vp;

  vd = VecDot(tri->normal, ray->Dir);
  if (vd == 0.)
    return (0); /* triangle parallel to ray */
  if (vd > 0.)
    r = -1; /* triangle normal pointing away from viewer */
  else
    r = 1; /* triangle normal pointing to viewer */

  vp = VecDot(tri->normal, ray->Pos);
  *dist = -(vp + tri->D) / vd;

  RayPoint (pnt, *dist, ray);
  return (r);
  }

/*
+--------------------------------- TriInOut() ----------------------------------
|
| returns TRUE if PNT is inside tri, FALSE for failure. Assumes pnt and tri are
| co-plannar.
|
| N.B. Local function
|
+-------------------------------------------------------------------------------
*/

static int TriInOut(POINT pnt, TRIANGLE *tri) {
  VEC AP, BP, CP;

  VecSub(AP,tri->A,pnt);
  VecSub(BP,tri->B,pnt);
  VecSub(CP,tri->C,pnt);

  if ( VecDot(tri -> NxAB,AP) >= 0.
    && VecDot(tri -> NxBC,BP) >= 0.
    && VecDot(tri -> NxCA,CP) >= 0. )
    return (TRUE);
  else
    return (FALSE);
  }

/*
+------------------------------- TriFirstHit() ---------------------------------
|
| Ray intersection with triangles.
|   Run through the linked list of triangles and get a hit while
|   disregarding any hit on the object from which the ray is departing.
| Returns _TRUE_ for success, or _NULL_ for failure. The intersection point
|   structure contains the result.
| If _isect_ initially contains a pointer to an object (isect->object != NULL)
|   thsi object is disregarded in the search.
|
| N.B. Global function
|
+-------------------------------------------------------------------------------
*/

int TriFirstHit(ISECT *isect, RAY *ray) {

  FLT dist, smalldist;
  POINT M, N;
  register TRIANGLE *Tp, *Tnear;
  int face;



  Tp = (TRIANGLE *)Tri0;
  Tnear = NULL;
  smalldist = MAXFLOAT;

  for (;Tp != NULL;) {
    if (Tp != (TRIANGLE *)isect->object) {

      /*
      If not the object from which the ray starts...
      */

      face = RayPlane(M, &dist, ray, Tp);

      if (TriInOut(M, Tp) && dist > 0.) {

        /*
        the ray pierces the triangle...
        The normal direction _face_ and distance _dist_ of the intersection
        determine which triangle is seen first along the ray.
        */

        if (dist < smalldist && face > 0) {
          VecCopy(N, M);
          smalldist = dist;
          Tnear = Tp;
          }
        }
      }
    Tp = Tp -> next;
    }

  /*
  If there was a hit copy the info into the isect structure and signal it on
  return, otherwise signal no hits on return.
  */

  if (Tnear != NULL) {
    isect -> t = smalldist;
    VecCopy (isect -> point, N);
    isect -> object = (PTR)Tnear;
    isect -> surface = Tnear -> surface;
    VecCopy(isect -> normal,Tnear -> normal);
    return (-1);
    }
  else
    return (0);
  }


/*
+--------------------------------- TriAnyHit() ---------------------------------
|
| Ray intersection with triangles.
|   Run through the linked list of triangles and get a hit while
|   disregarding any hit on the object from which the ray is departing.
| Returns _TRUE_ for success, or _NULL_ for failure. The intersection point
|   structure contains the result.
| If _isect_ initially contains a pointer to an object (isect->object != NULL)
|   this object is disregarded in the search.
|
| N.B. Global function
|
+-------------------------------------------------------------------------------
*/


int TriAnyHit(ISECT *isect, RAY *ray) {


  FLT dist;
  POINT M;
  register TRIANGLE *Tp;


  Tp = (TRIANGLE *)Tri0;

  for (;Tp != NULL;) {
    if (Tp != (TRIANGLE *)isect->object) {

      /*
      If not the object from which the ray starts...
      */

      if (RayPlane(M, &dist, ray, Tp) != 0) {

        if (TriInOut(M, Tp) && dist > 0.) {

          /*
          the ray pierces the triangle...
          */

          isect -> t = dist;
          VecCopy (isect -> point, M);
          isect -> object = (PTR)Tp;
          isect -> surface = Tp -> surface;
          VecCopy(isect -> normal,Tp -> normal);
          return (-1);
          }
        }
      }
    Tp = Tp -> next;
    }
  return (0);
  }


/*
+----------------------------------- Trace() -----------------------------------
|
| The colour of the ray is returned in _colour_
| Called from the view ray creator, acts as an entry function.
| N.B. Global function
|
+-------------------------------------------------------------------------------
*/

void BackGrnd(COLOUR colour, RAY *ray);
void Shade(COLOUR colour, ISECT *isect, int (*Intersect)());

FLT Trace(COLOUR colour, RAY *ray) {
  ISECT isect = {.0,{.0,.0,.0},{.0,.0,.0},NULL,NULL};
  FLT t;

  isect.object = NULL; /* the origin of the ray is not on an object */

  if (TriFirstHit(&isect, ray) == 0) {
    t = 100.;
    BackGrnd(colour, ray);
    }
  else {
    t = isect.t;
    Shade(colour, &isect, TriAnyHit);
    }


  return(t);
  }

/*
+-------------------------------- AddTriangle() --------------------------------
|
| Add a triangle to the linked list.
| Returns TRUE for success FALSE otherwise.
| rotvert is a pointer ro an array of Points.
| A,B, and C are indices of the points that make the triangle
| N.B. Global function
|
+-------------------------------------------------------------------------------
*/

int AddTriangle(POINT *rotvert, int A, int B, int C, SURF *surf) {

  VEC U, V, W;
  TRIANGLE *tp = NULL;
  VEC AB, BC, CA;

  if ((tp = (TRIANGLE *) calloc (sizeof(TRIANGLE),1)) == NULL)
    return (FALSE);

  tp -> next = NULL;
  tp -> A = rotvert[A];
  tp -> B = rotvert[B];
  tp -> C = rotvert[C];
  VecSub(U, rotvert[B], rotvert[A]);
  VecSub(V, rotvert[C], rotvert[A]);
  VecCross(W, U, V);
  VecUnit(tp -> normal, W);

  VecSub(AB, tp->A, tp->B);
  VecSub(BC, tp->B, tp->C);
  VecSub(CA, tp->C, tp->A);

  VecCross(tp -> NxAB, tp -> normal, AB);
  VecCross(tp -> NxBC, tp -> normal, BC);
  VecCross(tp -> NxCA, tp -> normal, CA);

  tp -> D = -VecDot(rotvert[A], tp -> normal);
  tp -> surface = surf;

  tp -> next = Tri0;
  Tri0 = tp;

  return(TRUE);
  }

/*
+-------------------------------- PrtTriangle() --------------------------------
|
| Print all the triangles in the linked list to screen.
| Returns nothing.
| N.B. Global function
|
+-------------------------------------------------------------------------------
*/

void PrtTriangle() {
  TRIANGLE *tp;

  tp = Tri0;
  for(;tp != NULL ;) {

    fprintf(stderr,"\ntriangle <- %p\n", tp);
    fprintf(stderr,"A = %f, %f, %f\n", tp -> A[0], tp -> A[1], tp -> A[2]);
    fprintf(stderr,"B = %f, %f, %f\n", tp -> B[0], tp -> B[1], tp -> B[2]);
    fprintf(stderr,"C = %f, %f, %f\n", tp -> C[0], tp -> C[1], tp -> C[2]);

    fprintf(stderr,"Normal = %f, %f, %f\n",
      tp -> normal[0], tp -> normal[1], tp -> normal[2]);
    fprintf(stderr,"D = %f\n", tp -> D);
    fprintf(stderr,"surface <= %p\n", tp -> surface);

    tp = tp -> next;
    }
  }

/*============================================================================*/
