#include "types.h"
#include <math.h>

#include <stdio.h>

/*
 these are routined for vector manipulation
 parameter assignment is consistant with C library conventions
 i.e. when one of the parameters is a pointer to a result it is the FIRST
      in the parameter list
*/



/*------------------------------ vector length -------------------------------*/

FLT VecLen(VEC A) {
  return (sqrt(A[0]*A[0] + A[1]*A[1] + A[2]*A[2]));
  }

/*------------------------------- dot product --------------------------------*/

FLT VecDot(VEC A, VEC B) {
  return (A[0]*B[0] + A[1]*B[1] + A[2]*B[2]);
  }

/*------------------------- cross product W = U x V --------------------------*/

void VecCross(VEC W, VEC U, VEC V) {
  FLT x, y, z;

  x = U[1]*V[2] - V[1]*U[2];
  y = U[2]*V[0] - V[2]*U[0];
  z = U[0]*V[1] - V[0]*U[1];
  W[0] = x;
  W[1] = y;
  W[2] = z;
  }

/*---------------------------- vector copy B = A -----------------------------*/

void VecCopy(VEC B, VEC A) {
  B[0] = A[0];
  B[1] = A[1];
  B[2] = A[2];
  }

/*------------------------ vector addition C = A + B -------------------------*/

void VecAdd(VEC C, VEC A, VEC B) {
  C[0] = A[0] + B[0];
  C[1] = A[1] + B[1];
  C[2] = A[2] + B[2];
  }

/*----------------------- vector subtraction C = A - B -----------------------*/

void VecSub(VEC C, VEC A, VEC B) {
  C[0] = A[0] - B[0];
  C[1] = A[1] - B[1];
  C[2] = A[2] - B[2];
  }

/*---------------------- linear combination C = aA + bB ----------------------*/

void VecComb(VEC C, FLT a, VEC A, FLT b, VEC B) {
  C[0] = a*A[0] + b*B[0];
  C[1] = a*A[1] + b*B[1];
  C[2] = a*A[2] + b*B[2];
  }

/*---------------------- add scalar multiple C = aA + B ----------------------*/

void VecAddS(VEC C, FLT a, VEC A, VEC B) {
  C[0] = a*A[0] + B[0];
  C[1] = a*A[1] + B[1];
  C[2] = a*A[2] + B[2];
  }

/*------------ vector unitise (normalise) B = A / |A|, returns |A| -----------*/

FLT VecUnit(VEC B, VEC A) {
  FLT n;

  n = VecLen(A);
  B[0] = A[0]/n;
  B[1] = A[1]/n;
  B[2] = A[2]/n;

  return (n);
  }

/*----------------------------------------------------------------------------*/

void MatCopy(MAT Dest, MAT Src) {
  register int i;

  for (i = 0; i < 16; i++)
    Dest[i] = Src[i];
  }

/*----------------------------------------------------------------------------*/

void MatMulti(MAT Ans, MAT Mat1, MAT Mat2) {
  register int row, col, r, c;
  MAT temp;

  for (row = 0; row < 16; row += 4) {
    for (col = 0; col < 4; col++) {
      temp[row + col] = 0.;
      for (r = 0, c = 0; c < 4; r += 4, c++)
        temp[row + col] += Mat1[row + c] * Mat2[r + col];
      }
    }
  MatCopy(Ans, temp);
  }

/*----------------------------------------------------------------------------*/

void MatVec(VEC Vdest, MAT Mat, VEC Vsrc) {
  FLT x, y, z;

  x = Mat[0] * Vsrc[0] + Mat[1] * Vsrc[1] + Mat[2] * Vsrc[2] + Mat[3];
  y = Mat[4] * Vsrc[0] + Mat[5] * Vsrc[1] + Mat[6] * Vsrc[2] + Mat[7];
  z = Mat[8] * Vsrc[0] + Mat[9] * Vsrc[1] + Mat[10] * Vsrc[2] + Mat[11];

  Vdest[0] = x;
  Vdest[1] = y;
  Vdest[2] = z;
  }

/*----------------------------------------------------------------------------*/

void SetRotation(MAT matrix, VEC rotate) {
  MAT temp;
  FLT cosine, sinus;
  register int i;

  for (i = 0; i < 16; i++)
    temp[i] = matrix[i] = 0.;
  for (i = 0; i < 16; i += 5)
    matrix[i] = temp[i] = 1.; /* Neutral matrix */

  cosine = cos(rotate[0]);
  sinus = sin(rotate[0]);

  temp[5] = temp[10] = cosine;
  temp[6] = -sinus;
  temp[9] = sinus;

  MatMulti(matrix, matrix, temp);



  for (i = 0; i < 16; i++)
    temp[i] = 0.;
  for (i = 0; i < 16; i += 5)
    temp[i] = 1.; /* Neutral matrix */

  cosine = cos(rotate[1]);
  sinus = sin(rotate[1]);

  temp[0] = temp[10] = cosine;
  temp[2] = sinus;
  temp[8] = -sinus;

  MatMulti(matrix, matrix, temp);



  for (i = 0; i < 16; i++)
    temp[i] = 0.;
  for (i = 0; i < 16; i += 5)
    temp[i] = 1.; /* Neutral matrix */

  cosine = cos(rotate[2]);
  sinus = sin(rotate[2]);

  temp[0] = temp[5] = cosine;
  temp[1] = -sinus;
  temp[4] = sinus;

  MatMulti(matrix, matrix, temp);

 }

/*======================== E N D   O F   L I S T I N G =======================*/
