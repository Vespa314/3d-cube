#ifndef ___XXX__3D__POINT____

#define ___XXX__3D__POINT____
#define  _3DVector _3DPoint 
class _3DPoint
{
public:
	_3DPoint(float xx,float yy,float zz);
	_3DPoint(){};
	_3DPoint(_3DPoint A,_3DPoint B);//vector(B-A)
	float x;
	float y;
	float z;
};


#endif