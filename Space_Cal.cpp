
#include "Space_Cal.h"


_3DLine GetLine_From_TwoPoint( _3DPoint point1,_3DPoint point2 )
{
	_3DLine line;
	line.x0 = point1.x;
	line.y0 = point1.y;
	line.z0 = point1.z;

	line.m = point1.x - point2.x;
	line.p = point1.y - point2.y;
	line.q = point1.z - point2.z;

	return line;
}

_3DPoint GetPoint_From_Line_And_Plane( _3DLine line,_3DPlane plane )
{
	float k = ( 
				 -1*plane.D-
		         line.x0*plane.A- 
				 line.y0*plane.B- 
				 line.z0*plane.C
				) 
				/ 
				(
				plane.A*line.m + plane.B*line.p + plane.C*line.q
				);
	_3DPoint point;

	point.x = k*line.m+line.x0;
	point.y = k*line.p+line.y0;
	point.z = k*line.q+line.z0;
	
	return point;
}

float GetDistance_From_TowPoint( _3DPoint point1,_3DPoint point2 )
{
	return sqrt((point1.x-point2.x)*(point1.x-point2.x) + 
				(point1.y-point2.y)*(point1.y-point2.y) + 
				(point1.z-point2.z)*(point1.z-point2.z));
}

_3DVector Cross( _3DVector vector1,_3DVector vector2 )
{
	return _3DVector(
		(vector1.y*vector2.z-vector1.z*vector2.y),
		(vector1.x*vector2.z-vector1.z*vector2.x)*(-1),
		(vector1.x*vector2.y-vector1.y*vector2.x)
		);
}

float Dot( _3DVector vector1,_3DVector vector2 )
{
	return vector1.x*vector2.x+vector1.y*vector2.y+vector1.z*vector2.z;
}

//最好先判断是否共面
int PointIsInTriangle( _3DPoint p,_3DPoint A,_3DPoint B,_3DPoint C )
{
	_3DVector pa(A,p);
	_3DVector pb(B,p);
	_3DVector pc(C,p);

	_3DVector temp1 = Cross(pa,pb);
	_3DVector temp2 = Cross(pb,pc);
	_3DVector temp3 = Cross(pc,pa);

	if (Dot(temp1,temp2)<0 || Dot(temp2,temp3)<0 || Dot(temp3,temp1)<0)
		return 0;
	else
		return 1;
}
