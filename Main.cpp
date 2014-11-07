// SphereWorld.cpp
// OpenGL SuperBible
// Demonstrates an immersive 3D environment using actors
// and a camera.
// Program by Richard S. Wright Jr.


#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
//#include <GL\glut.h>
#include "lib\gltools.h"
#include "lib\glFrame.h"

int			WinNumber			= NULL;               //�Ψө�m�����N�X
const float DEG2RAD				= 3.14159 / 180.0;    //�����੷��
const float windowWidth			= 800;				  //�����w�]�e��
const float windowHeight		= 600;				  //�����w�]����

int			old_rot_x			= 0;                  //����U�ƹ��ɪ������y��
int			old_rot_y			= 0;
int			rot_x				= 0;                  //�즲�᪺�۹�y�СA�γo�M�w�n����X��
int			rot_y				= 0;
int			record_x			= 0;                  //�����W�@�����઺����
int			record_y			= 0;

float		distance			= 0;                  //�b�����x�}(glTranslatef();)���ϥ�
float		light_position[]	= { 20, 20, 20 };     //��������m

char		mss[50];								  //��r��
double		RA					= 0.0;				  //���g
double		Dec					= 0.0;				  //���n
double		moveSpeed			= 0.5;				  //���F���ʳt��
float		target_RA			= 0.0;				  //GOTO-�ت��aRA
float		target_Dec			= 0.0;				  //GOTO-�ت��aDec
bool		mykey[6]			= { false };		  //�O��������U���A
bool		noLightMode			= false;			  //Wire/Shading Mode

const float HammerR				= 1.8;
const float HammerThick			= 0.7;
const int   Slice				= 64;
GLFrame     sun;
const float sun_RA				= 42.7;
const float sun_Dec				= 156.0;


void DrawGround(void);								  //�e�a�O��u
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
void printText(char*, float, float, float);
void updateRA_Dec();
void drawCube(bool, float, float, float, float, float, float, float);

int main()
{
	printf("\
|---------------Control---------------|\n\n\
  �W�U�� | �վ㻷��\n\
  �ƹ�   | ��Խվ����\n\
  A, D   | �վ㨪�g (RA)\n\
  W, S   | �վ㨪�n (Dec)\n\
  +, -   | �վ㰨�F�t�� (Motor Speed)\n\
  G      | GoTo �۰ʰl�ܤӶ�\n\
  P      | Park �k��ܪ�l��m\n\
  L      | Wire/Shading Mode (�}������)\n\
  Esc    | �����{��\n\n\
|---------------Control---------------|\n\n\
   �F�j�Ѥ���B�F�j��T��Ǩt �\�Ѱa");

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
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

	glutMainLoop();

	return 0;
}

///////////////////////////////////////////////////////////
// Draw a gridded ground
void DrawGround(void)
{
	GLfloat fExtent = 200.0f;
	GLfloat fStep = 5.0f;
	GLfloat y = -11.0f;
	GLint iLine;


	glBegin(GL_LINES);
	for (iLine = -fExtent; iLine <= fExtent; iLine += fStep)
	{
		glVertex3f(iLine, y, fExtent);    // Draw Z lines
		glVertex3f(iLine, y, -fExtent);

		glVertex3f(fExtent, y, iLine);
		glVertex3f(-fExtent, y, iLine);
	}

	glEnd();
}


void Display(void)
{
	if (!noLightMode) glClearColor(0.8, 0.8, 0.8, 1.0);      //��I��
	else			  glClearColor(0.5, 0.5, 0.5, 1.0);

	updateRA_Dec(); //���� RA, Dec ������

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	if (!noLightMode) glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);   // shading = true
	else			  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);   // shading = false

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(0, 0, 30.0, 0, 0, 0, 0, 1, 0);                    //���u���y�ФΤ�V
	glTranslatef(0, 0, distance);                               //�u��z�b����
	glRotatef((float)rot_y + (float)record_y, 1.0, 0.0, 0.0);   //�Hx�b�����b
	glRotatef((float)rot_x + (float)record_x, 0.0, 1.0, 0.0);   //�Hy�b�����b

	if (!noLightMode) 	glColor4ub(40, 40, 40, 255);
	else				glColor4ub(200, 200, 200, 255);
	DrawGround();												//�e�a�O��u


	///////////////////
	// �e����(�Ӷ�)
	glPushMatrix();
		sun.ApplyActorTransform();
		glDisable(GL_LIGHTING);
		glColor4ub(255, 64, 64, 255);
		glutWireSphere(2.0, 16, 16);
		if(!noLightMode) glEnable(GL_LIGHTING);
	glPopMatrix();

	/**------------------ Draw ���D����y START----------------**/
	glPushMatrix();    // save global matrix
		drawCube(noLightMode, 1.0, 2.3, 3.2, 2.0, 80, 0, 0);
	glPopMatrix();     // restore global matrix

	glPushMatrix();    // save global matrix
		glColor4ub(70, 70, 70, 255);
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
		glColor4ub(90, 10, 10, 255);
		glTranslated(0, 2, 0);
		glRotated(66.5, 0, 0, 1);
		glRotated(90, 1, 0, 0);
		glTranslated(0, 0, -1.5);
		gluCylinder(gluNewQuadric(), 1.2f, 1.2f, 3.5f, Slice, 16); // ���g�b
		glTranslated(0, 0, 3.5);
		gluDisk(gluNewQuadric(), 0, 1.2, Slice, 16);               // �\�l

		glColor4ub(0, 0, 0, 255);                                 ////////////////
		gluCylinder(gluNewQuadric(), 0.3f, 0.3f, 0.5f, Slice, 8); ////////////////
		glTranslated(0, 0, 0.5);								  //���b�滷��
		gluDisk(gluNewQuadric(), 0, 0.3, Slice, 8);               //���b�滷��I��
		glTranslated(0, 0, -4.0);                                 ////////////////

		glPushMatrix();
			glColor4ub(0, 0, 0,255);
			gluCylinder(gluNewQuadric(), 1.21f, 1.21f, 0.5f, Slice, 8); // ���g�B���n�b�汵�B
		glPopMatrix();

		glColor4ub(0, 0, 255, 200);
		glTranslated(0, 0, 3.5);
		gluCylinder(gluNewQuadric(), 1.2f, 0.6f, 1.0f, Slice, 8); // ����\�l
		glTranslated(0, 0, 1.0);
		gluDisk(gluNewQuadric(), 0, 0.6, Slice, 8);               // ����\�l
		glColor4ub(82, 0, 0, 255);
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
			drawCube(noLightMode,  1.0, 2.4, 4, 2.4,80, 0, 0);
		glPopMatrix();     // restore ���D�����n�b matrix

		glPushMatrix();    // save ���D�����n�b matrix
			glColor4ub(0, 0, 0, 255);
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
			glColor4ub(70, 70, 70, 255);
			glTranslated(0, 8, 0);
			glRotated(90, 1, 0, 0);
			gluCylinder(gluNewQuadric(), 0.2f, 0.2f, 6.0f, 8, 32); // HammerStick

			glColor4ub(70, 50, 50, 255);
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
			glRotated(90+Dec, 0, 1, 0); //�ھڨ��n������
			glTranslated(0, 0, -3);     //�վ���त���I
	
			glColor4ub(20, 20, 20, 255);
			//gltDrawUnitAxes();
			gluCylinder(gluNewQuadric(), 2.2f, 2.2f, 7.0f, Slice, 24); // Telescope
			gluDisk(gluNewQuadric(), 0.3, 2.2, Slice, 24);             // Telescope�I��
			
			glPushMatrix();
				glTranslated(0, 2.3, 3.3);
				drawCube(noLightMode, 1.0, 1, 0.7, 5, 80, 60, 60);
				glColor4ub(20, 20, 20, 255);
			glPopMatrix();

			glPushMatrix();
				glTranslated(0, 0, 6);

				glColor4ub(100, 100, 100, 100);               // Glass - Transparency
				gluDisk(gluNewQuadric(), 0, 2.2, Slice, 24);  // Telescope����(�d�ɮ�L��)
				glColor4ub(20, 20, 20, 255);

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
				glColor4ub(100, 100, 100, 150);						      // Glass - Transparency
				gluDisk(gluNewQuadric(), 0, 0.5, Slice, 8);				  // ����I��
				glColor4ub(20, 20, 20, 255);
			glPopMatrix();

			glRotated(45, 0, 0, 1);
			glTranslated(0,-3,0);
			gluCylinder(gluNewQuadric(), 0.5f, 0.5f, 3.0f, Slice, 16); // �M�P��
			gluDisk(gluNewQuadric(), 0, 0.5, Slice, 16);               // �M�P��I��
			glTranslated(0, 0, 3.0);
			gluDisk(gluNewQuadric(), 0, 0.5, Slice, 16);               // �M�P�西��
			glTranslated(0, 0, -3.5);
			gluCylinder(gluNewQuadric(), 0.3f, 0.3f, 1.0f, Slice, 8);  // ����
			gluDisk(gluNewQuadric(), 0, 0.3, Slice, 8);                // ����I��

			glTranslated(0, 0.6, 1.5);
			drawCube(noLightMode, 1.0, 1, 1, 1, 10, 10, 10);

		glPopMatrix();     // restore ���D�����n�b matrix
	/**------------------ Draw �D�� END------------------**/

	/**------------------ Draw �}�[ START--------------------**/
		const double offset = -3.2;
		glPopMatrix();   // restore global matrix
			glPushMatrix();  // save global matrix
				glTranslated(0, -6 * cos(35.0*DEG2RAD), offset);
				glRotated(35, 1, 0, 0);
				glTranslated(0, -2, 0);
				drawCube(noLightMode, 1.0, 1, 11, 0.2, 70, 70, 70);
			glPopMatrix();   // restore global matrix

			glPushMatrix();  // save global matrix
				glRotated(120, 0, 1, 0);
				glTranslated(0, -6 * cos(35.0*DEG2RAD), offset);
				glRotated(35, 1, 0, 0);
				glTranslated(0, -2, 0);
				drawCube(noLightMode, 1.0, 1, 11, 0.2, 70, 70, 70);
			glPopMatrix();   // restore global matrix

			glPushMatrix();  // save global matrix
				glRotated(240, 0, 1, 0);
				glTranslated(0, -6 * cos(35.0*DEG2RAD), offset);
				glRotated(35, 1, 0, 0);
				glTranslated(0, -2, 0);
				drawCube(noLightMode, 1.0, 1, 11, 0.2, 70, 70, 70);
			glPopMatrix();   // restore global matrix
		glPushMatrix();  // save global matrix

		glColor4ub(10.0, 10.0, 10.0, 255);
		glBegin(GL_TRIANGLES); // Draw �m���T�}�L
		glVertex3f(0.0, -5.0, offset);
		glVertex3f(offset*sin(120 * DEG2RAD), -5.0, offset*cos(120 * DEG2RAD));
		glVertex3f(offset*sin(240 * DEG2RAD), -5.0, offset*cos(240 * DEG2RAD));
		glEnd();
		glColor4ub(40, 40, 40, 255);
	/**------------------ Draw �}�[ END  --------------------**/
	glPopMatrix();   // restore global matrix


	glPushMatrix();///�b�ù��W�g�r
		glRotatef(-(float)rot_x - (float)record_x, 0.0, 1.0, 0.0);   //�Hy�b�����b�A���ܮy�Шt�����ܵe��
		glRotatef(-(float)rot_y - (float)record_y, 1.0, 0.0, 0.0);   //�Hx�b�����b�A���ܮy�Шt�����ܵe��
		glTranslated(0, 0, -distance);

		glTranslated(-15.58, 10.9, 0);
		sprintf(mss, "Motor Speed: %2.1f", moveSpeed);
		printText(mss, 0.0, 0.7, 0.0);

		glTranslated(0.0, -1, 0);
		sprintf(mss, "RA  : %3.1f", RA );
		printText(mss, 0.0, 0.7, 0.0);

		glTranslated(0.0, -1, 0);
		sprintf(mss, "Dec: %3.1f", Dec);
		printText(mss, 0.0, 0.7, 0.0);
	glPopMatrix();   // restore global matrix


	glutSwapBuffers();
}

void printText(char* str, float r, float g, float b){
	glColor3f(r, g, b);     //set font color
	glRasterPos2i(0, 0);    //set font start position
	for (int i = 0; i<strlen(str); i++)		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, str[i]);
}

void updateRA_Dec(){
	if (mykey[0]) Dec += moveSpeed;
	if (mykey[1]) Dec -= moveSpeed;
	if (mykey[2]) RA -= moveSpeed;
	if (mykey[3]) RA += moveSpeed;
	if (mykey[4] && moveSpeed < 9.9) moveSpeed += 0.2;
	if (mykey[5] && moveSpeed > 0.2) moveSpeed -= 0.2;

}

void drawCube(bool noLightMode, float size, float sX, float sY, float sZ, float r, float g, float b){
	glColor4ub(r+10, g+10, b+10, 255);
	if (!noLightMode) {
		glScaled(sX, sY, sZ);
		glutSolidCube(1.0);
		glColor4ub(r, g, b, 255);
		glutWireCube(1.0);
	}
	else
	{
		glPushMatrix();
			glScaled(sX, sY, sZ);
			glutWireCube(1.0);
		glPopMatrix();

		// �e����
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

	if (!RA_Done || !Dec_Done){
		glutTimerFunc(33, GOTO, 0);
		glutPostRedisplay();
	}
}

void myKeys(unsigned char key, int x, int y)
{
	switch (key)
	{
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
		mykey[4] = true;
		break;
	case '-':
		mykey[5] = true;
		break;
	case 'l':
	case 'L':
		if (!noLightMode) noLightMode = true;
		else noLightMode = false;
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
	case '+':
		mykey[4] = false;
		break;
	case '-':
		mykey[5] = false;
		break;
	}

}

// Respond to arrow keys by moving the camera frame of reference
void SpecialKeys(int key, int x, int y)
{
	if (key == GLUT_KEY_UP)
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

	glLightfv(GL_LIGHT1, GL_POSITION, light_position);     //�����y��

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
	glEnable(GL_BLEND);
	
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	// Enable color tracking
	glEnable(GL_COLOR_MATERIAL);

	sun.SetOrigin(light_position[0], light_position[1], light_position[2]);
}