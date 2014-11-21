// SphereWorld.cpp
// OpenGL SuperBible
// Demonstrates an immersive 3D environment using actors
// and a camera.
// Program by Richard S. Wright Jr.


#include <math.h>
#include <stdio.h>
#include <stdlib.h>
//#include <string>
//#include <GL\glut.h>
#include "lib\gltools.h"
#include "lib\math3d.h"
#include "lib\glFrame.h"


int			WinNumber			= NULL;                    //�Ψө�m�����N�X
const float DEG2RAD				= 3.14159 / 180.0;         //�����੷��
const float windowWidth			= 800;				       //�����w�]�e��
const float windowHeight		= 600;				       //�����w�]����
  
int			accumlateX			= 0;					   //�uX�b���઺�ֿn����
int			old_rot_x			= 0;                       //����U�ƹ��ɪ������y��x
int			old_rot_y			= 0;				       //����U�ƹ��ɪ������y��y
int			rot_x				= 0;                       //�즲�᪺�۹�y�СA�γo�M�w�n����X��
int			rot_y				= 0;
int			record_x			= 0;                       //�����W�@�����઺����x
int			record_y			= 0;                       //�����W�@�����઺����y

float		distance			= -10;                     //�b�����x�}(glTranslatef();)���ϥ�
float		fLightPos[]			= { 20, 20, 20, 0.0f };    //��������m
float		fLightPosMirror[]   = { 20, -20, 20, 0.0f };   //�˼v��������m

GLFrame     sun;								
char		mss[50];									   //��r��
double		RA					= 0.0;					   //���g
double		Dec					= 0.0;					   //���n
double		moveSpeed			= 0.2;			  		   //���F���ʳt��
float		target_RA			= 0.0;					   //GOTO-�ت��aRA
float		target_Dec			= 0.0;					   //GOTO-�ت��aDec
double		angle				= 35;					   //���[�i�� MAX=35, min=0
bool		mykey[6]			= { false };			   //�O��������U���A
bool		noLightMode			= true;				       //Wire/Shading Mode
bool        polygonoffset		= true;				   //polygonoffset is on/off
bool		antiAlias			= true;					   //�O�_�}�ҤϿ���

const float HammerR				= 1.8;
const float HammerThick			= 0.7;
const int   Slice				= 24;
const float sun_RA				= 42.7;
const float sun_Dec				= 156.0;

void SetLightSource(void);							  //�]�w�����ݩ�
void SetMaterial(void);								  //�]�w�����ݩ�
void SetupRC();
void WindowSize(int, int);							  //�t�d������ø�Ϥ��e�����
void myKeys(unsigned char, int, int);				  //�����L��J
void myKeysUp(unsigned char, int, int);				  //�����L�u�_
void SpecialKeys(int key, int x, int y);			  //����S�����J
void Mouse(int, int, int, int);						  //����ƹ����U�M��}�ɪ��T��
void MotionMouse(int, int);							  //����ƹ����U�������T��
void Display(void);									  //�e����s
void showInfo();									  //��ܰT����ù��W(���g�B���n��)
void printText(char*, float, float, float);           //�L�r
void updateRA_Dec();								  //��s���g���n
void DrawGround(void);								  //�e�a�O��u
void DrawTelescope(bool);					          //�e�滷�� or �滷��v�l
void DrawSun(void);									  //�e�Ӷ�	
void drawCube(bool, float, float, float, float, float, float, float);//�Ȼs�� Wire/Solid Cubes

int main()
{
	printf("\
|---------------Control---------------|\n\
  �W�U�� | �վ㻷��                    \n\
  �ƹ�   | ��Խվ����                \n\
  A, D   | �վ㨪�g (RA)               \n\
  W, S   | �վ㨪�n (Dec)              \n\
  [, ]   | �վ�}�[�i��                \n\
  +, -   | �վ㰨�F�t�� (Motor Speed)  \n\
  G      | GoTo �۰ʰl�ܤӶ�           \n\
  P      | Park �k��ܪ�l��m         \n\
  C      | Crazy �ƨg�ò��ʩҦ��i�����`\n\
|---------------Control---------------|\n\n\
|---------------Setting---------------|\n\
  L      | Shading on/off      (����)  \n\
  K      | Antialias on/off    (�Ͽ���)\n\
  J      | PolygonOffset on/off(���)  \n\
  Esc    | �����{��                    \n\
|---------------Setting---------------|\n\n\
   �F�j�Ѥ���B�F�j��T��Ǩt �\�Ѱa");

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_MULTISAMPLE);
	glutInitWindowSize(windowWidth, windowHeight);     //�������e
	glutInitWindowPosition(300, 150);                  //�������W������m
	WinNumber = glutCreateWindow("Telescope Model -- SSARCandy");   

	glutReshapeFunc(WindowSize);
	glutKeyboardFunc(myKeys);
	glutKeyboardUpFunc(myKeysUp);
	glutSpecialFunc(SpecialKeys);
	glutMouseFunc(Mouse);
	glutMotionFunc(MotionMouse);
	glutDisplayFunc(Display);

	SetLightSource();
	SetMaterial();
	SetupRC();

	//glEnable(GL_MULTISAMPLE);
	glutMainLoop();

	return 0;
}

///////////////////////////////////////////////////////////
// Draw a gridded ground
void DrawGround(void)
{
	GLfloat fExtent = 200.0f;
	GLfloat fStep = 10.0f;
	GLfloat y = -11.2f;
	GLint xLine, zLine, iLine;

	if (!noLightMode){
		int i = 0;
		for (zLine = -fExtent; zLine <= fExtent; zLine += fStep){
			for (xLine = -fExtent; xLine <= fExtent; xLine += fStep){
				(i++) % 2 ? glColor4ub(0, 0, 0, 180) : glColor4ub(255, 255, 255, 180);
				glBegin(GL_POLYGON);
					glVertex3f(xLine, y, zLine);
					glVertex3f(xLine, y, zLine + fStep);
					glVertex3f(xLine + fStep, y, zLine + fStep);
					glVertex3f(xLine + fStep, y, zLine);
				glEnd();
			}
		}
	}
//	else{
		glColor4ub(200, 200, 200, 255);

		for (iLine = -fExtent; iLine <= fExtent; iLine += fStep){
			glBegin(GL_LINES);
				glVertex3f(iLine, y+0.001, fExtent);    // Draw Z lines
				glVertex3f(iLine, y+0.001, -fExtent);

				glVertex3f(fExtent, y+0.001, iLine);
				glVertex3f(-fExtent, y+0.001, iLine);
			glEnd();
		}
//	}
}

void DrawTelescope(bool p){
	glPushMatrix();
	float translateUP = 11 * cos(angle*DEG2RAD) - 11 * cos(35 * DEG2RAD);
	glTranslated(0, translateUP, 0);

	/**------------------ Draw ���D����y START----------------**/
	glPushMatrix();    // save global matrix
		drawCube(noLightMode, 1.0, 2.3, 3.2, 2.0, 80, 0, 0);
	glPopMatrix();     // restore global matrix

	glPushMatrix();    // save global matrix
		if(!p) glColor4ub(70, 70, 70, 255);
		glTranslated(0, -1.6, 0);
		glRotated(90, 1, 0, 0);
		gluCylinder(gluNewQuadric(), 1.7f, 1.7f, 0.5f, Slice, 6); /////////
		gluDisk(gluNewQuadric(), 0, 1.7f, Slice, 12);             /////////
		glTranslated(0, 0, 0.5);							      //���y
		gluDisk(gluNewQuadric(), 0, 1.7f, Slice, 12);             /////////
	glPopMatrix();     // restore global matrix
	/**------------------ Draw ���D����y END------------------**/

	/**------------------ Draw ���D�����b(���g�b) START----------------**/
	glPushMatrix();    // save global matrix
		if (!p) glColor4ub(90, 10, 10, 255);
		glTranslated(0, 2, 0);
		glRotated(66.5, 0, 0, 1);
		glRotated(90, 1, 0, 0);
		glTranslated(0, 0, -1.5);
		gluCylinder(gluNewQuadric(), 1.2f, 1.2f, 3.5f, Slice, 16); // ���g�b
		glTranslated(0, 0, 3.5);
		gluDisk(gluNewQuadric(), 0, 1.2, Slice, 16);               // �\�l

		if (!p) glColor4ub(0, 0, 0, 255);                                 ////////////////
		gluCylinder(gluNewQuadric(), 0.3f, 0.3f, 0.5f, Slice, 8); ////////////////
		glTranslated(0, 0, 0.5);								  //���b�滷��
		gluDisk(gluNewQuadric(), 0, 0.3, Slice, 8);               //���b�滷��I��
		glTranslated(0, 0, -4.0);                                 ////////////////

		glPushMatrix();
			if (!p) glColor4ub(0, 0, 0, 255);
			gluCylinder(gluNewQuadric(), 1.21f, 1.21f, 0.5f, Slice, 8); // ���g�B���n�b�汵�B
		glPopMatrix();

		if (!p) glColor4ub(0, 0, 255, 200);
		glTranslated(0, 0, 3.5);
		gluCylinder(gluNewQuadric(), 1.2f, 0.6f, 1.0f, Slice, 8); // ����\�l
		glTranslated(0, 0, 1.0);
		gluDisk(gluNewQuadric(), 0, 0.6, Slice, 8);               // ����\�l
		if (!p) glColor4ub(82, 0, 0, 255);
	glPopMatrix();     // restore global matrix
	/**------------------ Draw ���D�����b(���g�b) END------------------**/

	/**------------------ Draw ���D�����n�b START----------------**/
	glPushMatrix();    // save global matrix
		glRotated(180 - 23.5, 0, 0, 1);
		glTranslated(3.5, -1.3, 0);

		glTranslated(0, -0.5, 0); //�վ���त���I
		glRotated(RA, 1, 0, 0);   //�ھڨ��g����
		glTranslated(0, 0.5, 0);  //�վ���त���I

		glPushMatrix();    // save ���D�����n�b matrix
			drawCube(noLightMode, 1.0, 2.4, 4, 2.4, 80, 0, 0);
		glPopMatrix();     // restore ���D�����n�b matrix

		glPushMatrix();    // save ���D�����n�b matrix
			if (!p) glColor4ub(0, 0, 0, 255);
			glTranslated(0, -1.9, 0);
			glRotated(90, 1, 0, 0);
			gluCylinder(gluNewQuadric(), 1.7f, 1.7f, 0.5f, Slice, 6);  /////////
			gluDisk(gluNewQuadric(), 0, 1.7f, Slice, 12);              /////////
			glTranslated(0, 0, 0.5);								   //�����x
			gluDisk(gluNewQuadric(), 0, 1.7f, Slice, 12);              /////////
		glPopMatrix();     // restore ���D�����n�b matrix
	/**------------------ Draw ���D�����n�b END------------------**/

	/**------------------ Draw �����&���� START----------------**/
		glPushMatrix();    // save ���D�����n�b matrix
			if (!p) glColor4ub(70, 70, 70, 255);
			glTranslated(0, 8, 0);
			glRotated(90, 1, 0, 0);
			gluCylinder(gluNewQuadric(), 0.2f, 0.2f, 6.0f, 8, 32); // HammerStick

			if (!p) glColor4ub(70, 50, 50, 255);
			glTranslated(0, 0, 1);//�ѭ�������B�V�W��
			gluCylinder(gluNewQuadric(), HammerR, HammerR, HammerThick, Slice, 8); // Hammer1
			gluDisk(gluNewQuadric(), 0, HammerR, Slice, 16); // Hammer1����
			glTranslated(0, 0, HammerThick);
			gluDisk(gluNewQuadric(), 0, HammerR, Slice, 16); // Hammer1����

			glTranslated(0, 0, 0.5);
			gluCylinder(gluNewQuadric(), HammerR, HammerR, HammerThick, Slice, 8); // Hammer2
			gluDisk(gluNewQuadric(), 0, HammerR, Slice, 16); // Hammer2����
			glTranslated(0, 0, HammerThick);
			gluDisk(gluNewQuadric(), 0, HammerR, Slice, 16); // Hammer2����
		glPopMatrix();     // restore ���D�����n�b matrix
	/**------------------ Draw �����&���� END------------------**/

	/**------------------ Draw �D�� START----------------**/
		glPushMatrix(); // save ���D�����n�b matrix
			glRotated(90, 0, 1, 0);
			glTranslated(0, -4.7, -3);
			glTranslated(0, 0, 3);      //�վ�襤�����I
			glRotated(90 + Dec, 0, 1, 0); //�ھڨ��n������
			glTranslated(0, 0, -3);     //�վ���त���I

			if (!p) glColor4ub(20, 20, 20, 255);
			//gltDrawUnitAxes();
			gluCylinder(gluNewQuadric(), 2.2f, 2.2f, 7.0f, Slice, 16); // Telescope
			gluDisk(gluNewQuadric(), 0.3, 2.2, Slice, 24);             // Telescope�I��

			glPushMatrix();
				glTranslated(0, 2.3, 3.3);
				drawCube(noLightMode, 1.0, 1, 0.7, 5, 80, 60, 60);
				if (!p) glColor4ub(20, 20, 20, 255);
			glPopMatrix();

			glPushMatrix();
				glTranslated(0, 0, 6);
				if (!p) glColor4ub(100, 100, 100, 100);               // Glass - Transparency
				gluDisk(gluNewQuadric(), 0, 2.2, Slice, 16);  // Telescope����(�d�ɮ�L��)
				if (!p) glColor4ub(20, 20, 20, 255);

				glTranslated(0, 0, 0.5);
				gluCylinder(gluNewQuadric(), 1.0f, 1.0f, 0.5f, Slice, 16); // �e�Ϯg��
				glTranslated(0, 0, 0.5);
				gluDisk(gluNewQuadric(), 0, 1.0, Slice, 16);
			glPopMatrix();

			glPushMatrix();
				glTranslated(0, 0, -0.5);
				gluCylinder(gluNewQuadric(), 1.0f, 1.0f, 0.6f, Slice, 8); // ����y
				gluDisk(gluNewQuadric(), 0.5, 1.0, Slice, 8);             // ����y�I��

				glTranslated(0, 0, -1.0);
				gluCylinder(gluNewQuadric(), 0.5f, 0.5f, 1.0f, Slice, 8); // ����
				if (!p) glColor4ub(100, 100, 100, 150);						      // Glass - Transparency
				gluDisk(gluNewQuadric(), 0, 0.5, Slice, 8);				  // ����I��
				if (!p) glColor4ub(20, 20, 20, 255);
			glPopMatrix();

			glRotated(45, 0, 0, 1);
			glTranslated(0, -3, 0);
			gluCylinder(gluNewQuadric(), 0.5f, 0.5f, 3.0f, Slice, 8); // �M�P��
			gluDisk(gluNewQuadric(), 0, 0.5, Slice, 16);               // �M�P��I��
			glTranslated(0, 0, 3.0);
			gluDisk(gluNewQuadric(), 0, 0.5, Slice, 16);               // �M�P�西��
			glTranslated(0, 0, -3.5);
			gluCylinder(gluNewQuadric(), 0.3f, 0.3f, 1.0f, Slice, 4);  // ����
			gluDisk(gluNewQuadric(), 0, 0.3, Slice, 8);                // ����I��

			glTranslated(0, 0.6, 1.5);
			drawCube(noLightMode, 1.0, 1, 1, 1, 10, 10, 10);
		glPopMatrix();     // restore ���D�����n�b matrix
	/**------------------ Draw �D�� END------------------**/

	/**------------------ Draw �}�[ START--------------------**/
	glPopMatrix();   // restore global matrix
	glPushMatrix();  // save global matrix
		glTranslated(0, -1.6, -1.0);
		glRotated(angle, 1, 0, 0);
		glTranslated(0, -5.9, 0);
		drawCube(noLightMode, 1.0, 1, 11.5, 0.2, 70, 70, 70);
	glPopMatrix();   // restore global matrix

	glPushMatrix();  // save global matrix
		glRotated(120, 0, 1, 0);
		glTranslated(0, -1.6, -1.0);
		glRotated(angle, 1, 0, 0);
		glTranslated(0, -5.9, 0);
		drawCube(noLightMode, 1.0, 1, 11.5, 0.2, 70, 70, 70);
	glPopMatrix();   // restore global matrix

	glPushMatrix();  // save global matrix
		glRotated(240, 0, 1, 0);
		glTranslated(0, -1.6, -1.0);
		glRotated(angle, 1, 0, 0);
		glTranslated(0, -5.9, 0);
		drawCube(noLightMode, 1.0, 1, 11.5, 0.2, 70, 70, 70);
	glPopMatrix();   // restore global matrix
	glPushMatrix();  // save global matrix

	double offset = -11.5 * sin(angle*DEG2RAD) / 2 - (1 - angle / 35);
	if (!p) glColor4ub(10.0, 10.0, 10.0, 255);
	glBegin(GL_TRIANGLES); // Draw �m���T�}�L
		if (!p) glColor4ub(255.0, 0.0, 0.0, 255);
		glVertex3f(0.0, -5.0, offset);
		if (!p) glColor4ub(0.0, 255.0, 0.0, 255);
		glVertex3f(offset*sin(120 * DEG2RAD), -5.0, offset*cos(120 * DEG2RAD));
		if (!p) glColor4ub(0.0, 0.0, 255.0, 255);
		glVertex3f(offset*sin(240 * DEG2RAD), -5.0, offset*cos(240 * DEG2RAD));
	glEnd();
	if (!p) glColor4ub(40, 40, 40, 255);

	glPopMatrix(); // restore global matrix
	/**------------------ Draw �}�[ END  --------------------**/

	glPopMatrix();
}

void DrawSun(void){
	///////////////////
	// �e����(�Ӷ�)
	glPushMatrix();
		sun.ApplyActorTransform();
		glDisable(GL_LIGHTING);
		glColor4ub(255, 64, 64, 255);
		noLightMode ? glutWireSphere(2.0, 16, 16) : glutSolidSphere(2.0, 16, 16);
		if(!noLightMode) glEnable(GL_LIGHTING);
	glPopMatrix();
}

void CameraView(bool noLightMode){
	if (!noLightMode){
		accumlateX = (float)rot_y + (float)record_y;
		if (-accumlateX < asin(11.2 / (30 - distance)) / DEG2RAD &&
			accumlateX < asin(11.2 / (30 - distance)) / DEG2RAD + 180){
			glRotatef((float)rot_y + (float)record_y, 1.0, 0.0, 0.0);   //�Hx�b�����b
		}
		else{
			rot_y = 0;
			if (-accumlateX > asin(11.2 / (30 - distance)) / DEG2RAD)
				record_y = -asin(11.2 / (30 - distance)) / DEG2RAD;
			else
				record_y = asin(11.2 / (30 - distance)) / DEG2RAD + 180;
			glRotatef(record_y, 1.0, 0.0, 0.0);   //�Hx�b�����b
		}
		glRotatef((float)rot_x + (float)record_x, 0.0, 1.0, 0.0);   //�Hy�b�����b
	}
	else{
		glRotatef((float)rot_y + (float)record_y, 1.0, 0.0, 0.0);   //�Hx�b�����b
		glRotatef((float)rot_x + (float)record_x, 0.0, 1.0, 0.0);   //�Hy�b�����b
	}
}

void Display(void)
{
	noLightMode ? glClearColor(0.5, 0.5, 0.5, 1.0) : glClearColor(0.8, 0.8, 0.8, 1.0);//�ھڬO�_�}���v�Ӷ�I���C��

	updateRA_Dec(); //���� RA, Dec ������

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	noLightMode ? glPolygonMode(GL_FRONT_AND_BACK, GL_LINE) : glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // Shading = true/false

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(0, 0, 30.0, 0, 0, 0, 0, 1, 0);                    //���u���y�ФΤ�V
	glTranslatef(0, 0, distance);                               //�u��z�b����

	CameraView(noLightMode);

	if (noLightMode){
		glDisable(GL_LIGHTING);
		if (polygonoffset){
			glEnable(GL_POLYGON_OFFSET_FILL);
			glPolygonOffset(.5, .5);
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glColor3f(0.35f, 0.35f, 0.35f);
			DrawTelescope(polygonoffset);//�e�滷��
			glDisable(GL_POLYGON_OFFSET_FILL);

			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			polygonoffset = false;
			DrawTelescope(polygonoffset);//�e�滷��
			DrawSun();
			polygonoffset = true;
		}
		else{
			DrawSun();
			DrawTelescope(polygonoffset);//�e�滷��
		}
		DrawGround();	//�e�a�O��u
	}
	else{
		DrawSun();
		DrawTelescope(false);//�e�滷��
		//// Move light under floor to light the "reflected" world
		glLightfv(GL_LIGHT1, GL_POSITION, fLightPosMirror);
		glPushMatrix();
			glFrontFace(GL_CW);  // geometry is mirrored, swap orientation
				glScalef(1.0f, -1.0f, 1.0f);
				glTranslated(0, 22.5, 0);
				DrawSun();		 //�e�a�O�ϥ����Ӷ�
				DrawTelescope(false); //�e�a�O�ϥ����滷��
			glFrontFace(GL_CCW);
		glPopMatrix();
		DrawGround();												
		glLightfv(GL_LIGHT1, GL_POSITION, fLightPos);// Restore correct lighting and draw the world correctly
	}

	glPushMatrix();// save global matrix
		showInfo();// �b�ù��W�g�r
	glPopMatrix(); // restore global matrix

	glutSwapBuffers();
}

void showInfo(){
	glDisable(GL_LIGHTING);
	glRotatef(-(float)rot_x - (float)record_x, 0.0, 1.0, 0.0);   //�Hy�b�����b�A���ܮy�Шt�����ܵe��
	glRotatef(-(float)rot_y - (float)record_y, 1.0, 0.0, 0.0);   //�Hx�b�����b�A���ܮy�Шt�����ܵe��
	glTranslated(0, 0, -distance);

	glTranslated(-15.58, 10.9, 0);
	sprintf(mss, "Motor Speed: %.2f", moveSpeed);
	printText(mss, 0.0, 0.7, 0.0);

	glTranslated(0.0, -1, 0);
	// �ץ���ܪ����g����ڪ����g�榡
	float rRA;
	int RA_h, RA_m, RA_s;
	RA > 0 ? rRA = RA : rRA = 360 - RA;
	RA_h = rRA / 15.0;
	RA_m = (rRA - RA_h * 15) * 4.0;
	RA_s = (rRA - RA_h * 15 - RA_m / 4) * 15;

	sprintf(mss, "RA  : %.2dh %.2dm %.2ds", RA_h, RA_m, RA_s);
	printText(mss, 0.0, 0.7, 0.0);

	glTranslated(0.0, -1, 0);
	// �ץ���ܪ����n����ڪ����n
	float realDec = Dec;
	realDec > 0 ? realDec = 360 - realDec : realDec = -realDec;
	if (realDec > 90 && realDec <= 270) realDec = 180 - realDec;
	if (realDec > 270 && realDec <= 360) realDec = realDec - 360;
	int Dec_d, Dec_m, Dec_s;
	Dec_d = (int)(realDec);
	Dec_m = (int)((realDec - Dec_d) * 60);
	Dec_s = (int)((realDec - Dec_d) * 3600) - Dec_m * 60;

	sprintf(mss, "Dec: %.0f* %.2d' %.2d''", realDec, abs(Dec_m), abs(Dec_s));
	printText(mss, 0.0, 0.7, 0.0);

	if(!noLightMode) glEnable(GL_LIGHTING);
}

void printText(char* str, float r, float g, float b){
	glColor3f(r, g, b);     //set font color
	glRasterPos2i(0, 0);    //set font start position
	for (unsigned i = 0; i<strlen(str); i++)		
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, str[i]);
}

void updateRA_Dec(){
	if (mykey[0]) Dec += moveSpeed;
	if (mykey[1]) Dec -= moveSpeed;
	if (mykey[2]) RA -= moveSpeed;
	if (mykey[3]) RA += moveSpeed;
	
	if (RA < -360) RA += 360;
	if (RA > 360) RA -= 360;

	if (Dec < -360) Dec += 360;
	if (Dec > 360) Dec -= 360;
}

// Draw customized Wire/Solid Cubes
void drawCube(bool noLightMode, float size, float sX, float sY, float sZ, float r, float g, float b){
	if (!noLightMode) {
		glColor4ub(r + 10, g + 10, b + 10, 255);
		glScaled(sX, sY, sZ);
		glutSolidCube(1.0);
		glColor4ub(r, g, b, 255);
		glutWireCube(1.0);
	}
	else
	{
		if (!polygonoffset) glColor4ub(r + 10, g + 10, b + 10, 255);
		glPushMatrix();
		glScaled(sX, sY, sZ);
			if (polygonoffset)  glutSolidCube(0.99);
			glutWireCube(1.0);
		glPopMatrix();

		// �e����
		if (!polygonoffset){
			for (GLfloat i = 0; i < sX; i += 0.5){
				glPushMatrix();
				glScaled(i, sY, sZ);
				glutWireCube(1.0);
				glPopMatrix();
			}
			for (GLfloat i = 0; i < sY; i += 0.5){
				glPushMatrix();
				glScaled(sX, i, sZ);
				glutWireCube(1.0);
				glPopMatrix();
			}
			for (GLfloat i = 0; i < sZ; i += 0.5){
				glPushMatrix();
				glScaled(sX, sY, i);
				glutWireCube(1.0);
				glPopMatrix();
			}
		}
	}
}

// GOTO specify target
void GOTO(int i){
	bool RA_Done = false;
	bool Dec_Done = false;

	if (abs(RA - target_RA) >= moveSpeed) {
		if (RA > target_RA) RA -= moveSpeed;
		else                RA += moveSpeed;
	}
	else
		RA_Done = true;

	if (abs(Dec - target_Dec) >= moveSpeed){
		if (Dec > target_Dec) Dec -= moveSpeed;
		else                  Dec += moveSpeed;
	}
	else
		Dec_Done = true;

	updateRA_Dec();

	if (!RA_Done || !Dec_Done){
		glutTimerFunc(33, GOTO, 0);
		glutPostRedisplay();
	}
}

void goCrazy(int i){
	RA += 3;
	Dec += 3;

	((i / 35) % 2) == 0 ? angle-- : angle++;
	
	if (i < 700){
		glutTimerFunc(10, goCrazy, ++i);
		glutPostRedisplay();
	}
}

void myKeys(unsigned char key, int x, int y)
{
	switch (key)
	{
		case 'k':
		case 'K':
			antiAlias = !antiAlias;
			if (antiAlias){
				glEnable(GL_POINT_SMOOTH);
				glEnable(GL_LINE_SMOOTH);
				glEnable(GL_POLYGON_SMOOTH);
			}
			else{
				glDisable(GL_POINT_SMOOTH);
				glDisable(GL_LINE_SMOOTH);
				glDisable(GL_POLYGON_SMOOTH);
			}
			break;
		case 'j':
		case 'J':
			polygonoffset = !polygonoffset;
			break;
		case 'w':
		case 'W':
			mykey[0] = true;
			break;
		case 's':
		case 'S':
			mykey[1] = true;
			break;
		case 'a':
		case 'A':
			mykey[2] = true;
			break;
		case 'd':
		case 'D':
			mykey[3] = true;
			break;
		case '+':
			if (moveSpeed < 9.95) moveSpeed += 0.05;
			break;
		case '-':
			if (moveSpeed > 0.06) moveSpeed -= 0.05;
			break;
		case ']':
			if (angle < 35) angle++;
			break;
		case '[':
			if (angle > 0) angle--;
			break;
		case 'c':
		case 'C':
			goCrazy(35-angle);
			break;
		case 'l':
		case 'L':
			noLightMode = !noLightMode;
			noLightMode ? glDisable(GL_FOG) : glEnable(GL_FOG);
			break;
		case 'g':
		case 'G':
			target_Dec = sun_Dec;
			target_RA = sun_RA;
			GOTO(0);
			break;
		case 'p':
		case 'P':
			target_Dec = 0.0;
			target_RA = 0.0;
			GOTO(0);
			break;
		case 27: // Esc
			glDisable(GL_LIGHT1);
			glDisable(GL_LIGHTING);
			glDisable(GL_DEPTH_TEST);
			glutDestroyWindow(WinNumber);
			exit(0);
			break;
	}
	glutPostRedisplay();
}

void myKeysUp(unsigned char key, int x, int y){
	switch (key)
	{
	case 'w':
	case 'W':
		mykey[0] = false;
		break;
	case 's':
	case 'S':
		mykey[1] = false;
		break;
	case 'a':
	case 'A':
		mykey[2] = false;
		break;
	case 'd':
	case 'D':
		mykey[3] = false;
		break;
	}
}

// Respond to arrow keys by moving the camera frame of reference
void SpecialKeys(int key, int x, int y)
{
	if (key == GLUT_KEY_UP && distance < 10)
		distance += 1;

	if (key == GLUT_KEY_DOWN)
		distance -= 1;

	// Refresh the Window
	glutPostRedisplay();
}

void WindowSize(int w, int h)
{
	float rate;
	if (h == 0) h = 1;                      //����h���s�A�����i���ର�s��
	glViewport(0, 0, w, h);                 //��������e���ܮɡA�e���]�����
	rate = (float)w / (float)h;             //�e�������ܤF�A�����e���ܧ�

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, rate, 1.0, 500.0);   //�z����v

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void Mouse(int button, int state, int x, int y)
{
	if (state)
	{
		record_x += x - old_rot_x;
		record_y += y - old_rot_y;

		rot_x = 0;   //�S���k�s�|�����z�Q�����G
		rot_y = 0;
	}
	else
	{
		old_rot_x = x;
		old_rot_y = y;
	}
}

void MotionMouse(int x, int y)
{
	rot_x = x - old_rot_x;
	rot_y = y - old_rot_y;
	glutPostRedisplay();
}

void SetLightSource()
{
	float light_ambient[] = { 1.0, 1.0, 1.0, 1.0 };
	float light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
	float light_specular[] = { 1.0, 1.0, 1.0, 1.0 };

	glEnable(GL_LIGHTING);                                 //�}�O

	// �]�w�o���骺�������S��
	glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambient);       //���ҥ�(Ambient Light)
	glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse);       //���g��(Diffuse Light)
	glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular);     //�Ϯg��(Specular Light)

	glLightfv(GL_LIGHT1, GL_POSITION, fLightPos);     //�����y��

	glEnable(GL_LIGHT1);
	glEnable(GL_DEPTH_TEST);                               //�Ұʲ`�״���
}

void SetMaterial()
{
	float material_ambient[] = { 0.5, 0.5, 0.5, 1.0 };
	float material_diffuse[] = { 0.4, 0.4, 0.4, 1.0 };
	float material_specular[] = { 0.3, 0.3, 0.3, 1.0 };

	glMaterialfv(GL_FRONT, GL_AMBIENT, material_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular);
}

// This function does any needed initialization on the rendering context.
void SetupRC()
{
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glEnable(GL_COLOR_MATERIAL);	// Enable color tracking

	//glEnable(GL_MULTISAMPLE);

	glShadeModel(GL_SMOOTH);
	glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
	glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	/* Antialias */
	glEnable(GL_POINT_SMOOTH);
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_POLYGON_SMOOTH);


	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

	// Setup Fog parameters
	glEnable(GL_FOG);	// Turn Fog on
	float fLowLight[] = { 0.8, 0.8, 0.8, 1.0 };	// Set fog color to match background
	glFogfv(GL_FOG_COLOR, fLowLight);
	glFogf(GL_FOG_START, 100.0f);  // How far away does the fog start
	glFogf(GL_FOG_END, 250.0f);	   // How far away does the fog stop  
	glFogi(GL_FOG_MODE, GL_LINEAR);// Which fog equation do I use?     

	sun.SetOrigin(fLightPos[0], fLightPos[1], fLightPos[2]);
}