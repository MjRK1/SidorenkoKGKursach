#include "Render.h"

#include <sstream>
#include <iostream>

#include <windows.h>
#include <GL\GL.h>
#include <GL\GLU.h>

#include "MyOGL.h"

#include "Camera.h"
#include "Light.h"
#include "Primitives.h"

#include "GUItextRectangle.h"

bool textureMode = true;
bool lightMode = true;

//����� ��� ��������� ������
class CustomCamera : public Camera
{
public:
	//��������� ������
	double camDist;
	//���� �������� ������
	double fi1, fi2;


	//������� ������ �� ���������
	CustomCamera()
	{
		camDist = 15;
		fi1 = 1;
		fi2 = 1;
	}


	//������� ������� ������, ������ �� ����� ��������, ���������� �������
	void SetUpCamera()
	{
		//�������� �� ������� ������ ������
		lookPoint.setCoords(0, 0, 0);

		pos.setCoords(camDist * cos(fi2) * cos(fi1),
			camDist * cos(fi2) * sin(fi1),
			camDist * sin(fi2));

		if (cos(fi2) <= 0)
			normal.setCoords(0, 0, -1);
		else
			normal.setCoords(0, 0, 1);

		LookAt();
	}

	void CustomCamera::LookAt()
	{
		//������� ��������� ������
		gluLookAt(pos.X(), pos.Y(), pos.Z(), lookPoint.X(), lookPoint.Y(), lookPoint.Z(), normal.X(), normal.Y(), normal.Z());
	}



}  camera;   //������� ������ ������


//����� ��� ��������� �����
class CustomLight : public Light
{
public:
	CustomLight()
	{
		//��������� ������� �����
		pos = Vector3(1, 1, 3);
	}


	//������ ����� � ����� ��� ���������� �����, ���������� �������
	void  DrawLightGhismo()
	{
		glDisable(GL_LIGHTING);


		glColor3d(0.9, 0.8, 0);
		Sphere s;
		s.pos = pos;
		s.scale = s.scale * 0.08;
		s.Show();

		if (OpenGL::isKeyPressed('G'))
		{
			glColor3d(0, 0, 0);
			//����� �� ��������� ����� �� ����������
			glBegin(GL_LINES);
			glVertex3d(pos.X(), pos.Y(), pos.Z());
			glVertex3d(pos.X(), pos.Y(), 0);
			glEnd();

			//������ ���������
			Circle c;
			c.pos.setCoords(pos.X(), pos.Y(), 0);
			c.scale = c.scale * 1.5;
			c.Show();
		}

	}

	void SetUpLight()
	{
		GLfloat amb[] = { 0.2, 0.2, 0.2, 0 };
		GLfloat dif[] = { 1.0, 1.0, 1.0, 0 };
		GLfloat spec[] = { .7, .7, .7, 0 };
		GLfloat position[] = { pos.X(), pos.Y(), pos.Z(), 1. };

		// ��������� ��������� �����
		glLightfv(GL_LIGHT0, GL_POSITION, position);
		// �������������� ����������� �����
		// ������� ��������� (���������� ����)
		glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
		// ��������� ������������ �����
		glLightfv(GL_LIGHT0, GL_DIFFUSE, dif);
		// ��������� ���������� ������������ �����
		glLightfv(GL_LIGHT0, GL_SPECULAR, spec);

		glEnable(GL_LIGHT0);
	}


} light;  //������� �������� �����




//������ ���������� ����
int mouseX = 0, mouseY = 0;

void mouseEvent(OpenGL* ogl, int mX, int mY)
{
	int dx = mouseX - mX;
	int dy = mouseY - mY;
	mouseX = mX;
	mouseY = mY;

	//������ ���� ������ ��� ������� ����� ������ ����
	if (OpenGL::isKeyPressed(VK_RBUTTON))
	{
		camera.fi1 += 0.01 * dx;
		camera.fi2 += -0.01 * dy;
	}


	//������� ���� �� ���������, � ����� ��� ����
	if (OpenGL::isKeyPressed('G') && !OpenGL::isKeyPressed(VK_LBUTTON))
	{
		LPPOINT POINT = new tagPOINT();
		GetCursorPos(POINT);
		ScreenToClient(ogl->getHwnd(), POINT);
		POINT->y = ogl->getHeight() - POINT->y;

		Ray r = camera.getLookRay(POINT->x, POINT->y);

		double z = light.pos.Z();

		double k = 0, x = 0, y = 0;
		if (r.direction.Z() == 0)
			k = 0;
		else
			k = (z - r.origin.Z()) / r.direction.Z();

		x = k * r.direction.X() + r.origin.X();
		y = k * r.direction.Y() + r.origin.Y();

		light.pos = Vector3(x, y, z);
	}

	if (OpenGL::isKeyPressed('G') && OpenGL::isKeyPressed(VK_LBUTTON))
	{
		light.pos = light.pos + Vector3(0, 0, 0.02 * dy);
	}


}

void mouseWheelEvent(OpenGL* ogl, int delta)
{

	if (delta < 0 && camera.camDist <= 1)
		return;
	if (delta > 0 && camera.camDist >= 100)
		return;

	camera.camDist += 0.01 * delta;

}

void keyDownEvent(OpenGL* ogl, int key)
{
	if (key == 'L')
	{
		lightMode = !lightMode;
	}

	if (key == 'T')
	{
		textureMode = !textureMode;
	}

	if (key == 'R')
	{
		camera.fi1 = 1;
		camera.fi2 = 1;
		camera.camDist = 15;

		light.pos = Vector3(1, 1, 3);
	}

	if (key == 'F')
	{
		light.pos = camera.pos;
	}
}

void keyUpEvent(OpenGL* ogl, int key)
{

}



GLuint texId;

//����������� ����� ������ ��������
void initRender(OpenGL* ogl)
{
	//��������� �������

	//4 ����� �� �������� �������
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	//��������� ������ ��������� �������
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	//�������� ��������
	glEnable(GL_TEXTURE_2D);


	//������ ����������� ���������  (R G B)
	RGBTRIPLE* texarray;

	//������ ��������, (������*������*4      4, ���������   ����, �� ������� ������������ �� 4 ����� �� ������� �������� - R G B A)
	char* texCharArray;
	int texW, texH;
	OpenGL::LoadBMP("texture.bmp", &texW, &texH, &texarray);
	OpenGL::RGBtoChar(texarray, texW, texH, &texCharArray);



	//���������� �� ��� ��������
	glGenTextures(1, &texId);
	//������ ��������, ��� ��� ����� ����������� � ���������, ����� ����������� �� ����� ��
	glBindTexture(GL_TEXTURE_2D, texId);

	//��������� �������� � �����������, � ���������� ��� ������  ��� �� �����
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texW, texH, 0, GL_RGBA, GL_UNSIGNED_BYTE, texCharArray);

	//�������� ������
	free(texCharArray);
	free(texarray);

	//������� ����
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);


	//������ � ���� ����������� � "������"
	ogl->mainCamera = &camera;
	ogl->mainLight = &light;

	// ������������ �������� : �� ����� ����� ����� 1
	glEnable(GL_NORMALIZE);

	// ���������� ������������� ��� �����
	glEnable(GL_LINE_SMOOTH);


	//   ������ ��������� ���������
	//  �������� GL_LIGHT_MODEL_TWO_SIDE - 
	//                0 -  ������� � ���������� �������� ���������(�� ���������), 
	//                1 - ������� � ���������� �������������� ������� ��������       
	//                �������������� ������� � ���������� ��������� ����������.    
	//  �������� GL_LIGHT_MODEL_AMBIENT - ������ ������� ���������, 
	//                �� ��������� �� ���������
	// �� ��������� (0.2, 0.2, 0.2, 1.0)

	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 0);

	camera.fi1 = -1.3;
	camera.fi2 = 0.8;
}


Vector3 getNormal(double* start, double* end1, double* end2)
{
	Vector3 a = Vector3(start[0] - end1[0], start[1] - end1[1], start[2] - end1[2]);
	Vector3 b = Vector3(start[0] - end2[0], start[1] - end2[1], start[2] - end2[2]);
	Vector3 normal = Vector3(a.Y() * b.Z() - b.Y() * a.Z(), -a.X() * b.Z() + b.X() * a.Z(), a.X() * b.Y() - a.Y() * b.X());
	return normal;
}


void Render(OpenGL* ogl)
{



	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);

	glEnable(GL_DEPTH_TEST);
	if (textureMode)
		glEnable(GL_TEXTURE_2D);

	if (lightMode)
		glEnable(GL_LIGHTING);


	//��������������
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	//��������� ���������
	GLfloat amb[] = { 0.2, 0.2, 0.1, 1. };
	GLfloat dif[] = { 0.4, 0.65, 0.5, 1. };
	GLfloat spec[] = { 0.9, 0.8, 0.3, 1. };
	GLfloat sh = 0.1f * 256;


	//�������
	glMaterialfv(GL_FRONT, GL_AMBIENT, amb);
	//��������
	glMaterialfv(GL_FRONT, GL_DIFFUSE, dif);
	//����������
	glMaterialfv(GL_FRONT, GL_SPECULAR, spec); \
		//������ �����
		glMaterialf(GL_FRONT, GL_SHININESS, sh);

	//���� ���� �������, ��� ����������� (����������� ���������)
	glShadeModel(GL_SMOOTH);
	//===================================
	//������� ���  

	glBegin(GL_LINE_LOOP);
	double a[] = { 2,13,0 };
	double b[] = { 9, 18,0 };
	double c[] = { 10, 11,0 };
	double d[] = { 18, 14,0 };
	double e[] = { 14, 10,0 };
	double f[] = { 16, 4,0 };
	double h[] = { 9,4,0 };
	double g[] = { 9, 10,0 };
	Vector3 normal = getNormal(d, c, a);
	glNormal3d(normal.X(), normal.Y(), normal.Z());
	glVertex3dv(a);
	glVertex3dv(b);
	glVertex3dv(c);
	glVertex3dv(d);
	glVertex3dv(e);
	glVertex3dv(f);
	glVertex3dv(h);
	glVertex3dv(g);
	glEnd();
	glBegin(GL_LINE_LOOP);
	double a1[] = { 2,13,4 };
	double b1[] = { 9, 18,4 };
	double c1[] = { 10, 11,4 };
	double d1[] = { 18, 14,4 };
	double e1[] = { 14, 10,4 };
	double f1[] = { 16, 4,4 };
	double h1[] = { 9,4,4 };
	double g1[] = { 9, 10,4 };

	normal = getNormal(d1, c1, a1);
	glNormal3d(normal.X(), normal.Y(), normal.Z());
	glVertex3dv(a1);
	glVertex3dv(b1);
	glVertex3dv(c1);
	glVertex3dv(d1);
	glVertex3dv(e1);
	glVertex3dv(f1);
	glVertex3dv(h1);
	glVertex3dv(g1);
	glEnd();
	glBegin(GL_QUADS);
	glColor3d(0.0, 0.6, 0.5);
	double a2[] = { 2,13,0 };
	double b2[] = { 9,18,0 };
	double c2[] = { 9,18,4 };
	double d2[] = { 2,13,4 };
	normal = getNormal(d2, c2, a2);
	glNormal3d(normal.X(), normal.Y(), normal.Z());
	glVertex3dv(a2);
	glVertex3dv(b2);
	glVertex3dv(c2);
	glVertex3dv(d2);
	glColor3d(0.1, 0.6, 0.5);
	double a3[] = { 9,18,0 };
	double b3[] = { 10,11,0 };
	double c3[] = { 10,11,4 };
	double d3[] = { 9,18,4 };
	normal = getNormal(d3, c3, a3);
	glNormal3d(normal.X(), normal.Y(), normal.Z());
	glVertex3dv(a3);
	glVertex3dv(b3);
	glVertex3dv(c3);
	glVertex3dv(d3);
	glColor3d(0.1, 0.6, 0.5);
	double a4[] = { 10,11,0 };
	double b4[] = { 18,14,0 };
	double c4[] = { 18,14,4 };
	double d4[] = { 10,11,4 };
	normal = getNormal(d4, c4, a4);
	glNormal3d(normal.X(), normal.Y(), normal.Z());
	glVertex3dv(a4);
	glVertex3dv(b4);
	glVertex3dv(c4);
	glVertex3dv(d4);
	double a5[] = { 18,14,0 };
	double b5[] = { 14,10,0 };
	double c5[] = { 14,10,4 };
	double d5[] = { 18,14,4 };
	normal = getNormal(d5, c5, a5);
	glNormal3d(normal.X(), normal.Y(), normal.Z());
	glVertex3dv(a5);
	glVertex3dv(b5);
	glVertex3dv(c5);
	glVertex3dv(d5);
	double a6[] = { 14,10,0 };
	double b6[] = { 16,4,0 };
	double c6[] = { 16,4,4 };
	double d6[] = { 14,10,4 };
	normal = getNormal(d6, c6, a6);
	glNormal3d(normal.X(), normal.Y(), normal.Z());
	glVertex3dv(a6);
	glVertex3dv(b6);
	glVertex3dv(c6);
	glVertex3dv(d6);
	double a7[] = { 14,10,0 };
	double b7[] = { 16,4,0 };
	double c7[] = { 16,4,4 };
	double d7[] = { 14,10,4 };
	normal = getNormal(d7, c7, a7);
	glNormal3d(normal.X(), normal.Y(), normal.Z());
	glVertex3dv(a7);
	glVertex3dv(b7);
	glVertex3dv(c7);
	glVertex3dv(d7);
	double a8[] = { 16,4,0 };
	double b8[] = { 9,4,0 };
	double c8[] = { 9,4,4 };
	double d8[] = { 16,4,4 };
	normal = getNormal(d8, c8, a8);
	glNormal3d(normal.X(), normal.Y(), normal.Z());
	glVertex3dv(a8);
	glVertex3dv(b8);
	glVertex3dv(c8);
	glVertex3dv(d8);
	double a9[] = { 9,4,0 };
	double b9[] = { 9,10,0 };
	double c9[] = { 9,10,4 };
	double d9[] = { 9,4,4 };
	normal = getNormal(d9, c9, a9);
	glNormal3d(normal.X(), normal.Y(), normal.Z());
	glVertex3dv(a9);
	glVertex3dv(b9);
	glVertex3dv(c9);
	glVertex3dv(d9);
	double a10[] = { 9,10,0 };
	double b10[] = { 2,13,0 };
	double c10[] = { 2,13,4 };
	double d10[] = { 9,10,4 };
	normal = getNormal(d10, c10, a10);
	glNormal3d(normal.X(), normal.Y(), normal.Z());
	glVertex3dv(a10);
	glVertex3dv(b10);
	glVertex3dv(c10);
	glVertex3dv(d10);

	glEnd();
	glColor3d(0, 0.6, 0);
	glBegin(GL_QUADS);
	glVertex3d(2, 13, 0);
	glVertex3d(9, 18, 0);
	glVertex3d(10, 11, 0);
	glVertex3d(9, 10, 0);
	glColor3d(0, 0, 0.5);
	glVertex3d(18, 14, 0);
	glVertex3d(10, 11, 0);
	glColor3d(0, 0, 0);
	glVertex3d(9, 10, 0);
	glVertex3d(14, 10, 0);
	glVertex3d(9, 4, 0);
	glVertex3d(16, 4, 0);
	glEnd();
	glBegin(GL_QUADS);
	glVertex3d(9, 10, 0);
	glVertex3d(9, 4, 0);
	glVertex3d(16, 4, 0);
	glVertex3d(14, 10, 0);
	glEnd();
	glColor3d(0, 0.6, 0);
	glBegin(GL_QUADS);
	glVertex3d(2, 13, 0);
	glVertex3d(9, 18, 0);
	glVertex3d(10, 11, 0);
	glVertex3d(9, 10, 0);
	glColor3d(0, 0, 0.5);
	glVertex3d(18, 14, 0);
	glVertex3d(10, 11, 0);
	glColor3d(0, 0, 0);
	glVertex3d(9, 10, 0);
	glVertex3d(14, 10, 0);
	glVertex3d(9, 4, 0);
	glVertex3d(16, 4, 0);
	glEnd();
	glBegin(GL_QUADS);
	glVertex3d(9, 10, 0);
	glVertex3d(9, 4, 0);
	glVertex3d(16, 4, 0);
	glVertex3d(14, 10, 0);
	glEnd();

	glColor3d(0, 0.6, 0);
	glBegin(GL_QUADS);
	glVertex3d(2, 13, 4);
	glVertex3d(9, 18, 4);
	glVertex3d(10, 11, 4);
	glVertex3d(9, 10, 4);
	glColor3d(0, 0, 0.5);
	glVertex3d(18, 14, 4);
	glVertex3d(10, 11, 4);
	glColor3d(0, 0, 0);
	glVertex3d(9, 10, 4);
	glVertex3d(14, 10, 4);
	glVertex3d(9, 4, 4);
	glVertex3d(16, 4, 4);
	glEnd();
	glBegin(GL_QUADS);
	glVertex3d(9, 10, 4);
	glVertex3d(9, 4, 4);
	glVertex3d(16, 4, 4);
	glVertex3d(14, 10, 4);
	glEnd();


	//��������� ������ ������


	glMatrixMode(GL_PROJECTION);	//������ �������� ������� ��������. 
									//(���� ��������� ��������, ����� �� ������������.)
	glPushMatrix();   //��������� ������� ������� ������������� (������� ��������� ������������� ��������) � ���� 				    
	glLoadIdentity();	  //��������� ��������� �������
	glOrtho(0, ogl->getWidth(), 0, ogl->getHeight(), 0, 1);	 //������� ����� ������������� ��������

	glMatrixMode(GL_MODELVIEW);		//������������� �� �����-��� �������
	glPushMatrix();			  //��������� ������� ������� � ���� (��������� ������, ����������)
	glLoadIdentity();		  //���������� �� � ������

	glDisable(GL_LIGHTING);



	GuiTextRectangle rec;		   //������� ����� ��������� ��� ������� ������ � �������� ������.
	rec.setSize(300, 150);
	rec.setPosition(10, ogl->getHeight() - 150 - 10);


	std::stringstream ss;
	ss << "T - ���/���� �������" << std::endl;
	ss << "L - ���/���� ���������" << std::endl;
	ss << "F - ���� �� ������" << std::endl;
	ss << "G - ������� ���� �� �����������" << std::endl;
	ss << "G+��� ������� ���� �� ���������" << std::endl;
	ss << "�����. �����: (" << light.pos.X() << ", " << light.pos.Y() << ", " << light.pos.Z() << ")" << std::endl;
	ss << "�����. ������: (" << camera.pos.X() << ", " << camera.pos.Y() << ", " << camera.pos.Z() << ")" << std::endl;
	ss << "��������� ������: R=" << camera.camDist << ", fi1=" << camera.fi1 << ", fi2=" << camera.fi2 << std::endl;

	rec.setText(ss.str().c_str());
	rec.Draw();

	glMatrixMode(GL_PROJECTION);	  //��������������� ������� �������� � �����-��� �������� �� �����.
	glPopMatrix();


	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

}
