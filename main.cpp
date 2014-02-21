/*
将来可以改进之处:
1.目光不一定要盯着（0,0,0）
2.虚线要怎么画   //已实现
3.正方体以外的透视图的实现
*/
#include <iostream>
#include <stdlib.h>
#include "3D_Object.h"
#include "3D_Plane.h"
#include "3D_Line.h"
#include "Space_Cal.h"
#include <math.h>
using namespace std;
#ifdef _DEBUG
#pragma comment ( lib, "cxcore200d.lib" )
#pragma comment ( lib, "cv200d.lib" )
#pragma comment ( lib, "highgui200d.lib" )
#else
#pragma comment ( lib, "cxcore200.lib" )
#pragma comment ( lib, "cv200.lib" )
#pragma comment ( lib, "highgui200.lib" )
#endif
#include "cv.h"
#include "highgui.h"


#define X0 (g_ViewPoint.x)
#define Y0 (g_ViewPoint.y)
#define Z0 (g_ViewPoint.z)
#define eye_focal_distance 1
#define PI 3.14159265358979f
#define FRAME_HEIGTH 400
#define FRAME_WIDTH 400
#define PIXEL_RATE 200
#define  WINDOW_NAME "3D Model"


//1  正方体   2  长方体   3  椎体    4   面
#define OBJECT_CHOOSE 1

IplImage* frame = NULL;
_3DPoint g_ViewPoint(4,2,0);//视点
_3DObject g_Object;//观测的物体

const float weitiao = 0.2f;
int mouse_begin_x;
int mouse_begin_y;
int mouse_counter;


void InitObject();//初始化观测物体的参数
void on_mouse( int event, int x, int y, int flags, void* param );//鼠标响应函数
void Draw_3D_Object();//画3D图形
void DrawDotLine(CvPoint point1,CvPoint point2);//画虚线
void SetInvisiablePoint();//设置不可见的点
//获取视网膜平面
_3DPlane GetRetinaPlane()
{
	return _3DPlane(X0,Y0,Z0,-X0*X0-Y0*Y0-Z0*Z0-eye_focal_distance*sqrt(X0*X0+Y0*Y0+Z0*Z0));
}

//获取视网膜平面原点空间坐标
_3DPoint GetOriPoint_In_RetinaPlane()
{
	float Dis = GetDistance_From_TowPoint(_3DPoint(0,0,0),g_ViewPoint);
	float k = (Dis+eye_focal_distance)/Dis;
	return _3DPoint(X0*k,Y0*k,Z0*k);
}

//视网膜平面定义的X轴
_3DVector GetRetinaXAxis()
{
	_3DVector X_Axis = Cross(g_ViewPoint,_3DVector(0,0,1));
	//归一化
	float m = GetDistance_From_TowPoint(_3DPoint(0,0,0),X_Axis);
	X_Axis.x /= m;
	X_Axis.y /= m;
	X_Axis.z /= m;
	return X_Axis;
}

//视网膜平面定义的Y轴
_3DVector GetRetinaYAxis(_3DVector X_Axis)
{
	_3DVector Y_Axis = Cross(g_ViewPoint,X_Axis);
	//归一化
	float m = GetDistance_From_TowPoint(_3DPoint(0,0,0),Y_Axis);
	Y_Axis.x /= m;
	Y_Axis.y /= m;
	Y_Axis.z /= m;
	return Y_Axis;
}

//返回值前两个坐标有效，表示二维平面下的坐标
_3DPoint Get2DPoint(_3DPlane RetinaPlane,_3DPoint _OriPoint,_3DPoint RetinaOriPoint,_3DVector XAxis,_3DVector YAxis)
{	
	_3DLine Line = GetLine_From_TwoPoint(g_ViewPoint,_OriPoint);
	_3DPoint Point_In_Retina = GetPoint_From_Line_And_Plane(Line,RetinaPlane);

	_3DVector temp = _3DVector(Point_In_Retina.x-RetinaOriPoint.x,
		Point_In_Retina.y-RetinaOriPoint.y,
		Point_In_Retina.z-RetinaOriPoint.z);

	return _3DPoint(Dot(temp,XAxis),Dot(temp,YAxis),0);
}

//三维向二维映射
void Get_2D_Model()
{
	_3DPlane RetinaPlane = GetRetinaPlane();//视网膜平面方程
	_3DPoint RetinaOriPoint = GetOriPoint_In_RetinaPlane();
	_3DVector RetinaXAxis = GetRetinaXAxis();
	_3DVector RetinaYAxis = GetRetinaYAxis(RetinaXAxis);

	for(int i = 0;i < g_Object.PointNum;i++)
	{
		g_Object.Point_2D[i] = Get2DPoint(RetinaPlane,g_Object.Point[i],RetinaOriPoint,RetinaXAxis,RetinaYAxis);
	}
}


int main(int argc,char* argv[])
{
	InitObject();
	frame = cvCreateImage(cvSize(FRAME_WIDTH,FRAME_HEIGTH),IPL_DEPTH_8U,3);
	cvNamedWindow(WINDOW_NAME);
	cvSetMouseCallback(WINDOW_NAME, on_mouse, 0 );
	Draw_3D_Object();
	while(1)
	{
		cvShowImage(WINDOW_NAME,frame);
		cvWaitKey(2);
	}
	return 0;
}

void DrawDotLine(CvPoint point1,CvPoint point2)
{
	const int DotNum = 5;
	for(int i = 0;i < DotNum;i++)
	{
		cvLine(frame,
			cvPoint(point1.x+2*i*(point2.x-point1.x)/(2*DotNum-1),
				    point1.y+2*i*(point2.y-point1.y)/(2*DotNum-1)),
			cvPoint(point1.x+(2*i+1)*(point2.x-point1.x)/(2*DotNum-1),
					point1.y+(2*i+1)*(point2.y-point1.y)/(2*DotNum-1)),
			cvScalar(0,0,255),1
					);
	}
}

void SetInvisiablePoint()
{
	int UnvisiablePointNum;
	
	if((int(abs(g_ViewPoint.x)<=1) + int(abs(g_ViewPoint.y)<=1) + int(abs(g_ViewPoint.z)<=1)) == 2 )
		UnvisiablePointNum = 4;
	else if((int(abs(g_ViewPoint.x)<=1) + int(abs(g_ViewPoint.y)<=1) + int(abs(g_ViewPoint.z)<=1)) == 1 )
		UnvisiablePointNum = 2;
	else
		UnvisiablePointNum = 1;

	for(int temp = 0;temp < UnvisiablePointNum;temp++)
	{
		float Max_Dis = -1;
		int max_id = -1;
		for(int i = 0;i < g_Object.PointNum;i++)
		{
			if(g_Object.PointUnvisiable[i] != 1 && Max_Dis < GetDistance_From_TowPoint(g_ViewPoint,g_Object.Point[i]))
			{
				Max_Dis = GetDistance_From_TowPoint(g_ViewPoint,g_Object.Point[i]);
				max_id = i;
			}
		}
		g_Object.PointUnvisiable[max_id] = 1;
	}
}

void Draw_3D_Object()
{
	Get_2D_Model();
	for(int i = 0;i < g_Object.PointNum;i++)
	{
		g_Object.PointUnvisiable[i] = 0;
	}

#if OBJECT_CHOOSE == 1
	SetInvisiablePoint();
#endif
	cvZero(frame);
	for(int i = 0;i < g_Object.LineNum;i++)
	{
		if (g_Object.PointUnvisiable[g_Object.PointPair[i][0]] == 1 || g_Object.PointUnvisiable[g_Object.PointPair[i][1]] == 1)
		{
			DrawDotLine(
						cvPoint(int((g_Object.Point_2D[g_Object.PointPair[i][0]]).x*PIXEL_RATE*(-1)+FRAME_HEIGTH/2),
				                int((g_Object.Point_2D[g_Object.PointPair[i][0]]).y*PIXEL_RATE*(-1)+FRAME_WIDTH/2)),
					    cvPoint(int((g_Object.Point_2D[g_Object.PointPair[i][1]]).x*PIXEL_RATE*(-1)+FRAME_HEIGTH/2),
								int((g_Object.Point_2D[g_Object.PointPair[i][1]]).y*PIXEL_RATE*(-1)+FRAME_WIDTH/2))
						);
		}
		else
		{
			cvLine(frame,
					cvPoint(int((g_Object.Point_2D[g_Object.PointPair[i][0]]).x*PIXEL_RATE*(-1)+FRAME_HEIGTH/2),
							int((g_Object.Point_2D[g_Object.PointPair[i][0]]).y*PIXEL_RATE*(-1)+FRAME_WIDTH/2)),
					cvPoint(int((g_Object.Point_2D[g_Object.PointPair[i][1]]).x*PIXEL_RATE*(-1)+FRAME_HEIGTH/2),
							int((g_Object.Point_2D[g_Object.PointPair[i][1]]).y*PIXEL_RATE*(-1)+FRAME_WIDTH/2)),
					cvScalar(0,0,255),2
				  );
		}
	}
	//cout<<g_ViewPoint.x<<" "<<g_ViewPoint.y<<" "<<g_ViewPoint.z<<"\n";
}


void on_mouse( int event, int x, int y, int flags, void* param )
{
	switch( event )
	{
	case CV_EVENT_LBUTTONDOWN:
		mouse_begin_x = x;
		mouse_begin_y = y;
		mouse_counter = 0;
		break;
	case CV_EVENT_LBUTTONUP:
		break;
	case CV_EVENT_MOUSEMOVE:
		if (flags == CV_EVENT_FLAG_LBUTTON)
		{
			if (mouse_counter != 5)
			{
				mouse_counter++;
				return;
			}
			mouse_counter = 0;
			float Dis_moved = float(x-mouse_begin_x)/PIXEL_RATE*5;
			float Disxy = GetDistance_From_TowPoint(_3DPoint(g_ViewPoint.x,g_ViewPoint.y,0),_3DPoint(0,0,0));
			float angel = Dis_moved/Disxy;
			angel = atan2(g_ViewPoint.y,g_ViewPoint.x) + angel;
			if (angel > PI)
				angel -= 2*PI;
			if(angel <= -PI)
				angel += 2*PI;

			g_ViewPoint.x = Disxy*cos(angel);
			g_ViewPoint.y = Disxy*sin(angel);

			Dis_moved = float(y-mouse_begin_y)/PIXEL_RATE*5;
			float Disz = GetDistance_From_TowPoint(g_ViewPoint,_3DPoint(0,0,0));
			angel = Dis_moved/Disz;
			angel = atan2(g_ViewPoint.z,Disxy) + angel;
			if (angel > PI)
				angel -= 2*PI;
			if(angel <= -PI)
				angel += 2*PI;
			g_ViewPoint.z = Disz*sin(angel);
			g_ViewPoint.x *= (Disz*cos(angel)/Disxy);
			g_ViewPoint.y *= (Disz*cos(angel)/Disxy);

			mouse_begin_x = x;
			mouse_begin_y = y;
			Draw_3D_Object();
		}
		break;
	}
}

#if OBJECT_CHOOSE == 1
void InitObject()
{
	g_Object.PointNum = 8;
	g_Object.Point[0] = _3DPoint(-1,-1,1);
	g_Object.Point[1] = _3DPoint(-1,1,1);
	g_Object.Point[2] = _3DPoint(1,1,1);
	g_Object.Point[3] = _3DPoint(1,-1,1);
	g_Object.Point[4] = _3DPoint(-1,-1,-1);
	g_Object.Point[5] = _3DPoint(-1,1,-1);
	g_Object.Point[6] = _3DPoint(1,1,-1);
	g_Object.Point[7] = _3DPoint(1,-1,-1);

	g_Object.LineNum = 12;
	g_Object.PointPair[0][0] = 0;g_Object.PointPair[0][1] = 1;
	g_Object.PointPair[1][0] = 1;g_Object.PointPair[1][1] = 2;
	g_Object.PointPair[2][0] = 2;g_Object.PointPair[2][1] = 3;
	g_Object.PointPair[3][0] = 3;g_Object.PointPair[3][1] = 0;

	g_Object.PointPair[4][0] = 4;g_Object.PointPair[4][1] = 5;
	g_Object.PointPair[5][0] = 5;g_Object.PointPair[5][1] = 6;
	g_Object.PointPair[6][0] = 6;g_Object.PointPair[6][1] = 7;
	g_Object.PointPair[7][0] = 7;g_Object.PointPair[7][1] = 4;

	g_Object.PointPair[8][0] = 0;g_Object.PointPair[8][1] = 4;
	g_Object.PointPair[9][0] = 1;g_Object.PointPair[9][1] = 5;
	g_Object.PointPair[10][0] = 2;g_Object.PointPair[10][1] = 6;
	g_Object.PointPair[11][0] = 3;g_Object.PointPair[11][1] = 7;

}
#endif

#if OBJECT_CHOOSE == 2
void InitObject()
{
	g_Object.PointNum = 8;
	g_Object.Point[0] = _3DPoint(-1,-2,1);
	g_Object.Point[1] = _3DPoint(-1,2,1);
	g_Object.Point[2] = _3DPoint(1,2,1);
	g_Object.Point[3] = _3DPoint(1,-2,1);
	g_Object.Point[4] = _3DPoint(-1,-2,-1);
	g_Object.Point[5] = _3DPoint(-1,2,-1);
	g_Object.Point[6] = _3DPoint(1,2,-1);
	g_Object.Point[7] = _3DPoint(1,-2,-1);

	g_Object.LineNum = 12;
	g_Object.PointPair[0][0] = 0;g_Object.PointPair[0][1] = 1;
	g_Object.PointPair[1][0] = 1;g_Object.PointPair[1][1] = 2;
	g_Object.PointPair[2][0] = 2;g_Object.PointPair[2][1] = 3;
	g_Object.PointPair[3][0] = 3;g_Object.PointPair[3][1] = 0;

	g_Object.PointPair[4][0] = 4;g_Object.PointPair[4][1] = 5;
	g_Object.PointPair[5][0] = 5;g_Object.PointPair[5][1] = 6;
	g_Object.PointPair[6][0] = 6;g_Object.PointPair[6][1] = 7;
	g_Object.PointPair[7][0] = 7;g_Object.PointPair[7][1] = 4;

	g_Object.PointPair[8][0] = 0;g_Object.PointPair[8][1] = 4;
	g_Object.PointPair[9][0] = 1;g_Object.PointPair[9][1] = 5;
	g_Object.PointPair[10][0] = 2;g_Object.PointPair[10][1] = 6;
	g_Object.PointPair[11][0] = 3;g_Object.PointPair[11][1] = 7;

}
#endif


#if OBJECT_CHOOSE == 3
void InitObject()
{
	g_Object.PointNum = 4;
	g_Object.Point[0] = _3DPoint(0,0,1);
	g_Object.Point[1] = _3DPoint(0,1,-1);
	g_Object.Point[2] = _3DPoint(-1,-1,-1);
	g_Object.Point[3] = _3DPoint(1,-1,-1);

	g_Object.LineNum = 6;
	g_Object.PointPair[0][0] = 0;g_Object.PointPair[0][1] = 1;
	g_Object.PointPair[1][0] = 0;g_Object.PointPair[1][1] = 2;
	g_Object.PointPair[2][0] = 0;g_Object.PointPair[2][1] = 3;
	g_Object.PointPair[3][0] = 1;g_Object.PointPair[3][1] = 2;
	g_Object.PointPair[4][0] = 1;g_Object.PointPair[4][1] = 3;
	g_Object.PointPair[5][0] = 2;g_Object.PointPair[5][1] = 3;

}
#endif

#if OBJECT_CHOOSE == 4
void InitObject()
{
	g_Object.PointNum = 4;
	g_Object.Point[0] = _3DPoint(0,1,1);
	g_Object.Point[1] = _3DPoint(0,1,-1);
	g_Object.Point[2] = _3DPoint(0,-1,-1);
	g_Object.Point[3] = _3DPoint(0,-1,1);

	g_Object.LineNum = 4;
	g_Object.PointPair[0][0] = 0;g_Object.PointPair[0][1] = 1;
	g_Object.PointPair[1][0] = 1;g_Object.PointPair[1][1] = 2;
	g_Object.PointPair[2][0] = 2;g_Object.PointPair[2][1] = 3;
	g_Object.PointPair[3][0] = 3;g_Object.PointPair[3][1] = 0;
}
#endif