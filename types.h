/* data types for raytracer */

#undef TRUE
#define TRUE	-1
#undef FALSE
#define FALSE	0

typedef float FLT;	/* floating point data type */
typedef FLT VEC[3];	/* a 3D vector */
typedef VEC POINT;	/* xyz point data type */
typedef VEC COLOUR;	/* rgb color data type */
typedef FLT MAT[16];	/* a 4x4 matrix (can be refered to by MAT[row][col] */

typedef char *PTR;	/* a pointer to anything you want */

typedef struct RAY {	/* a RAY */
  POINT Pos;		/* position origin */
  POINT Dir;		/* direction unitised */
  } RAY;

typedef struct SURF {	/* a surface type */
  FLT kdiff;		/* diffuse reflection coefficient */
  COLOUR colour;	/* surface colour */
  } SURF;

typedef struct ISECT {	/* an intersection point */
  FLT t;		/* line parameter at intersection (distance) */
  POINT point;		/* intersection point */
  VEC normal;		/* normal of the surface */
  SURF *surface;	/* surface optics */
  PTR object;		/* pointer to obkect intersected */
  } ISECT;

#define RayPoint(point, t, ray) VecAddS(point, t, (ray)->Dir, (ray)->Pos)

/*============================================================================*/
