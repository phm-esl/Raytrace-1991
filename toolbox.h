extern FLT	VecLen(VEC A);
extern FLT	VecDot(VEC A, VEC B);
extern void	VecCross(VEC W, VEC U, VEC V);
extern void	VecCopy(VEC B, VEC A);
extern void	VecAdd(VEC C, VEC A, VEC B);
extern void	VecSub(VEC C, VEC A, VEC B);
extern void	VecComb(VEC C, FLT a, VEC A, FLT b, VEC B);
extern void	VecAddS(VEC C, FLT a, VEC A, VEC B);
extern FLT	VecUnit(VEC B, VEC A);
extern void SetRotation(MAT matrix, VEC rotate); // toolbox.c
extern void MatVec(VEC Vdest, MAT mat, VEC Vsrc); // toolbox.c
extern int AddTriangle(POINT *rotvert, int A, int B, int C, SURF *surf); // triangle.c
extern int AddLight(int issun, VEC source, COLOUR colour); // shade.c
extern int get_obj(char * filename); // load.c
extern FLT Trace(COLOUR colour, RAY *ray); // triangle.c

/*============================================================================*/
