#ifndef ___XXX__3D__SPACE__VECTOR__CAL____
#define ___XXX__3D__SPACE__VECTOR__CAL____
#include "3D_Line.h"
#include "3D_Plane.h"
#include "3D_Point.h"
#include <math.h>

_3DLine GetLine_From_TwoPoint(_3DPoint point1,_3DPoint point2);
_3DPoint GetPoint_From_Line_And_Plane(_3DLine line,_3DPlane plane);
float GetDistance_From_TowPoint(_3DPoint point1,_3DPoint point2);
_3DVector Cross(_3DVector vector1,_3DVector vector2);
float Dot(_3DVector vector1,_3DVector vector2);

//http://blog.sina.com.cn/s/blog_61feffe10100n65y.html
int PointIsInTriangle(_3DPoint p,_3DPoint A,_3DPoint B,_3DPoint C);
#endif

