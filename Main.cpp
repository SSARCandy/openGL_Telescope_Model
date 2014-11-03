// SphereWorld.cpp
// OpenGL SuperBible
// Demonstrates an immersive 3D environment using actors
// and a camera.
// Program by Richard S. Wright Jr.


#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <GL\glut.h>

int			WinNumber			= NULL;               //�Ψө�m�����N�X
const float DEG2RAD				= 3.14159 / 180.0;    //�����੷��

int			old_rot_x			= 0;                  //����U�ƹ��ɪ������y��
int			old_rot_y			= 0;
int			rot_x				= 0;                  //�즲�᪺�۹�y�СA�γo�M�w�n����X��
int			rot_y				= 0;
int			record_x			= 0;                  //�����W�@�����઺����
int			record_y			= 0;

float		distance			= 0;                  //�b�����x�}(glTranslatef();)���ϥ�
float		light_position[]	= { -20, 20, 0 };     //��������m

double		RA					= 0.0;				  //���g
double		Dec					= 0.0;				  //���n
double		moveSpeed			= 1.0;				  //���F���ʳt��
bool		mykey[6]			= { false };		  //�O��������U���A

const float HammerR				= 1.8;
const float HammerThick			= 0.7;


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

int main()
{
	printf("�W�U��վ㻷��\n�ƹ��վ����\n\nW, S�վ㨪�n\nA, D�վ㨪�g\n\nEsc�����{��\n");
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(800, 600);                     //�������e
	glutInitWindowPosition(600, 80);                  //�������W������m
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
	GLfloat fStep = 2.5f;
	GLfloat y = -10.0f;
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
	glClearColor(0.9, 0.9, 0.9, 1.0);      //��I��
	glColor4ub(50, 50, 50, 1.0);
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	if (mykey[0]) Dec += moveSpeed;
	if (mykey[1]) Dec -= moveSpeed;
	if (mykey[2]) RA  -= moveSpeed;
	if (mykey[3]) RA  += moveSpeed;
	if (mykey[4] && moveSpeed < 10) moveSpeed++;
	if (mykey[5] && moveSpeed > 0) moveSpeed--;


	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);                   //�]�w�������B�I������

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(0, 0, 30.0, 0, 0, 0, 0, 1, 0);                    //���u���y�ФΤ�V
	glTranslatef(0, 0, distance);                               //�u��z�b����
	glRotatef((float)rot_y + (float)record_y, 1.0, 0.0, 0.0);   //�Hx�b������b
	glRotatef((float)rot_x + (float)record_x, 0.0, 1.0, 0.0);   //�Hy�b������b
	DrawGround();												//�e�a�O��u

	/**------------------ Draw ���D����y START----------------**/
	glPushMatrix();    // save global matrix
	glColor4ub(30, 10, 10, 1.0);

	glScaled(2.3, 3.2, 2.0);
	glutSolidCube(1.0);
	glPopMatrix();     // restore global matrix

	/**------------------ Draw ���D����y END------------------**/

	/**------------------ Draw ���D�����b(���g�b) START----------------**/
	glPushMatrix();    // save global matrix
	glTranslated(0, 2, 0);
	glRotated(66.5, 0, 0, 1);
	glRotated(90, 1, 0, 0);
	glTranslated(0, 0, -1);
	gluCylinder(gluNewQuadric(), 1.2f, 1.2f, 3.0f, 32, 32);

	glColor4ub(0, 0, 128, 0.1);
	glTranslated(0, 0, 3.0);
	gluCylinder(gluNewQuadric(), 1.2f, 0.6f, 1.0f, 32, 32); // ����\�l
	glTranslated(0, 0, 1.0);
	gluDisk(gluNewQuadric(), 0, 0.6, 32, 32);               // ����\�l
	glColor4ub(30, 10, 10, 1.0);

	glPopMatrix();     // restore global matrix
	/**------------------ Draw ���D�����b(���g�b) END------------------**/

	/**------------------ Draw ���D�����n�b START----------------**/
	glPushMatrix();    // save global matrix
	glRotated(180 - 23.5, 0, 0, 1);
	glTranslated(3, -1, 0);

	glTranslated(0, -1, 0); //�վ���त���I
	glRotated(RA, 1, 0, 0); //�ھڨ��g����
	glTranslated(0, 1, 0);  //�վ���त���I

	glPushMatrix();    // save ���D�����n�b matrix
	glScaled(2.4, 5, 2.4);
	glutSolidCube(1.0);
	glPopMatrix();     // restore ���D�����n�b matrix
	/**------------------ Draw ���D�����n�b END------------------**/

	/**------------------ Draw �����&���� START----------------**/
	glPushMatrix();    // save ���D�����n�b matrix
	glColor4ub(60, 60, 70, 1.0);

	glTranslated(0, 8, 0);
	glRotated(90, 1, 0, 0);
	gluCylinder(gluNewQuadric(), 0.2f, 0.2f, 6.0f, 64, 64); // HammerStick

	glColor4ub(70, 50, 50, 1.0);

	glTranslated(0, 0, 1);//�ѭ�������B�V�W��
	gluCylinder(gluNewQuadric(), HammerR, HammerR, HammerThick, 64, 64); // Hammer1
	gluDisk(gluNewQuadric(), 0, HammerR, 64, 64); // Hammer1����
	glTranslated(0, 0, HammerThick);
	gluDisk(gluNewQuadric(), 0, HammerR, 64, 64); // Hammer1����

	glTranslated(0, 0, 0.5);
	gluCylinder(gluNewQuadric(), HammerR, HammerR, HammerThick, 64, 64); // Hammer2
	gluDisk(gluNewQuadric(), 0, HammerR, 64, 64); // Hammer2����
	glTranslated(0, 0, HammerThick);
	gluDisk(gluNewQuadric(), 0, HammerR, 64, 64); // Hammer2����
	glPopMatrix();     // restore ���D�����n�b matrix
	/**------------------ Draw �����&���� END------------------**/

	/**------------------ Draw �D�� START----------------**/
	glPushMatrix();    // save ���D�����n�b matrix

	glRotated(90, 0, 1, 0);
	glTranslated(0, -4.5, -3);

	glTranslated(0, 0, 3);   //�վ�襤�����I
	glRotated(Dec, 0, 1, 0); //�ھڨ��n������
	glTranslated(0, 0, -3);  //�վ���त���I
	
	glColor4ub(15, 15, 15, 1.0);
	gluCylinder(gluNewQuadric(), 2.2f, 2.2f, 7.0f, 128, 128); // Telescope
	gluDisk(gluNewQuadric(), 0, 2.2, 128, 128); // Telescope�I��

	glPushMatrix();
	glTranslated(0, 0, 6);
	gluDisk(gluNewQuadric(), 0, 2.2, 32, 32); // Telescope����(�d�ɮ�L��)
	glTranslated(0, 0, 0.5);
	gluCylinder(gluNewQuadric(), 1.0f, 1.0f, 0.5f, 32, 32); // �e�Ϯg��
	glTranslated(0, 0, 0.5);
	gluDisk(gluNewQuadric(), 0, 1.0, 32, 32);
	glPopMatrix();

	glPushMatrix();
	glTranslated(0, 0, -0.5);
	gluCylinder(gluNewQuadric(), 1.0f, 1.0f, 0.6f, 64, 64); // ����y
	gluDisk(gluNewQuadric(), 0, 1.0, 64, 64); // ����y�I��

	glTranslated(0, 0, -1.0);
	gluCylinder(gluNewQuadric(), 0.5f, 0.5f, 1.0f, 64, 64); // ����
	gluDisk(gluNewQuadric(), 0, 0.5, 64, 64); // ����I��
	glPopMatrix();

	glRotated(45, 0, 0, 1);
	glTranslated(0,-3,0);
	gluCylinder(gluNewQuadric(), 0.5f, 0.5f, 3.0f, 32, 32); // �M�P��
	gluDisk(gluNewQuadric(), 0, 0.5, 32, 32); // �M�P��I��
	glTranslated(0, 0, -0.5);
	gluCylinder(gluNewQuadric(), 0.3f, 0.3f, 1.0f, 32, 32); // ����
	gluDisk(gluNewQuadric(), 0, 0.3, 32, 32); // ����I��
	glTranslated(0, 0.6, 1.5);
	glutSolidCube(0.5);

	glColor4ub(50, 50, 50, 1.0);
	glPopMatrix();     // restore ���D�����n�b matrix
	/**------------------ Draw �D�� END------------------**/

	/**------------------ Draw �}�[ START--------------------**/
	double offset = -3.2;
	glPopMatrix();   // restore global matrix

	glPushMatrix();  // save global matrix
	glColor4ub(35, 35, 35, 1.0);

	glTranslated(0, -6 * cos(35.0*DEG2RAD), offset);
	glRotated(35, 1, 0, 0);
	glScaled(1, 12, 0.2);
	glutSolidCube(1.0);// �}�[1
	glPopMatrix();   // restore global matrix

	glPushMatrix();  // save global matrix
	glRotated(120, 0, 1, 0);
	glTranslated(0, -6 * cos(35.0*DEG2RAD), offset);
	glRotated(35, 1, 0, 0);
	glScaled(1, 12, 0.2);
	glutSolidCube(1.0);// �}�[2
	glPopMatrix();   // restore global matrix

	glPushMatrix();  // save global matrix
	glRotated(240, 0, 1, 0);
	glTranslated(0, -6 * cos(35.0*DEG2RAD), offset);
	glRotated(35, 1, 0, 0);
	glScaled(1, 12, 0.2);
	glutSolidCube(1.0);// �}�[3
	glPopMatrix();   // restore global matrix

	glPushMatrix();  // save global matrix

	glBegin(GL_TRIANGLES); // Draw �m���T�}�L
	glVertex3f(0.0, -5.0, offset);
	glVertex3f(offset*sin(120 * DEG2RAD), -5.0, offset*cos(120 * DEG2RAD));
	glVertex3f(offset*sin(240 * DEG2RAD), -5.0, offset*cos(240 * DEG2RAD));
	glEnd();


	/**------------------ Draw �}�[ END  --------------------**/



	glPopMatrix();   // restore global matrix
	//	glPopMatrix();



	glutSwapBuffers();
}

void myKeys(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'w':
		mykey[0] = true;
		break;
	case 's':
		mykey[1] = true;
		break;
	case 'a':
		mykey[2] = true;
		break;
	case 'd':
		mykey[3] = true;
		break;
	case '+':
		mykey[4] = true;
		break;
	case '-':
		mykey[5] = true;
		break;

	case 27:
		glDisable(GL_LIGHT0);
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
		mykey[0] = false;
		break;
	case 's':
		mykey[1] = false;
		break;
	case 'a':
		mykey[2] = false;
		break;
	case 'd':
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
	if (h == 0) h = 1;                        //����h���s�A�����i���ର�s��
	glViewport(0, 0, w, h);                 //���������e���ܮɡA�e���]�����
	rate = (float)w / (float)h;                //�e�������ܤF�A�����e���ܧ�

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
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);      //���ҥ�(Ambient Light)
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);      //���g��(Diffuse Light)
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);     //�Ϯg��(Specular Light)

	glLightfv(GL_LIGHT0, GL_POSITION, light_position);     //�����y��

	glEnable(GL_LIGHT0);
	glEnable(GL_DEPTH_TEST);                               //�Ұʲ`�״���
}

void SetMaterial()
{
	float material_ambient[] = { 0.2, 0.2, 0.2, 1.0 };
	float material_diffuse[] = { 0.3, 0.3, 0.3, 1.0 };
	float material_specular[] = { 0.2, 0.2, 0.2, 1.0 };

	glMaterialfv(GL_FRONT, GL_AMBIENT, material_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular);
}

// This function does any needed initialization on the rendering
// context.
void SetupRC()
{

	glEnable(GL_BLEND);




	// Enable color tracking
	glEnable(GL_COLOR_MATERIAL);


}