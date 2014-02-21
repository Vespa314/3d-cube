#ifndef ___XXX__3D__PLANE____

#define ___XXX__3D__PLANE____

//Ax+By+Cz+D = 0
class _3DPlane
{
public:
	_3DPlane(float a,float b,float c,float d);
	_3DPlane(){};
	float A;
	float B;
	float C;
	float D;
};

#endif