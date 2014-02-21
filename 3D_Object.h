#ifndef ___XXX__3D__OBJECT____

#define ___XXX__3D__OBJECT____
#include "3D_Point.h"

class ObjectPlane
{
public:
	int PointNum;
	_3DPoint Point[10];
};

class _3DObject
{
public:
	int PointNum;
	_3DPoint Point[30];
	_3DPoint Point_2D[30];//二维平面下的点的坐标，即结果
	int PointUnvisiable[30];
	int LineNum;
	int PointPair[30][2];

	int PlaneNum;
	ObjectPlane Plane[30];
};

#endif