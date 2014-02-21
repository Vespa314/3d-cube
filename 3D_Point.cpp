#include "3D_Point.h"


_3DPoint::_3DPoint( float xx,float yy,float zz )
{
	x = xx;
	y = yy;
	z = zz;
}

_3DPoint::_3DPoint( _3DPoint A,_3DPoint B )
{
	x = B.x-A.x;
	y = B.y-A.y;
	z = B.z-A.z;
}
