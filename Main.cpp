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

int			WinNumber			= NULL;               //用來放置視窗代碼
const float DEG2RAD				= 3.14159 / 180.0;    //角度轉弧度

int			old_rot_x			= 0;                  //剛按下滑鼠時的視窗座標
int			old_rot_y			= 0;
int			rot_x				= 0;                  //拖曳後的相對座標，用這決定要旋轉幾度
int			rot_y				= 0;
int			record_x			= 0;                  //紀錄上一次旋轉的角度
int			record_y			= 0;

float		distance			= 0;                  //在平移矩陣(glTranslatef();)中使用
float		light_position[]	= { -20, 20, 0 };     //光源的位置

double		RA					= 0.0;				  //赤經
double		Dec					= 0.0;				  //赤緯
double		moveSpeed			= 1.0;				  //馬達移動速度
bool		mykey[6]			= { false };		  //記錄按鍵按下狀態

const float HammerR				= 1.6;
const float HammerThick			= 0.6;


void WindowSize(int, int);							  //負責視窗及繪圖內容的比例
void myKeys(unsigned char, int, int);				  //獲取鍵盤輸入
void myKeysUp(unsigned char, int, int);				  //獲取鍵盤彈起
void SpecialKeys(int key, int x, int y);			  //獲取特殊鍵輸入
void Mouse(int, int, int, int);						  //獲取滑鼠按下和放開時的訊息
void MotionMouse(int, int);							  //獲取滑鼠按下期間的訊息
void Display(void);
void SetLightSource(void);							  //設定光源屬性
void SetMaterial(void);								  //設定材質屬性
void SetupRC();

int main()
{
	printf("上下鍵調整遠近\n滑鼠調整視角\n\nW, S調整赤緯\nA, D調整赤經\n\nEsc關閉程式\n");
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(800, 600);                     //視窗長寬
	glutInitWindowPosition(600, 80);                  //視窗左上角的位置
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

void Display(void)
{
	glClearColor(1.0, 1.0, 1.0, 1.0);                            //用白色塗背景
	glColor3ub(50, 50, 50);

	if (mykey[0]) Dec += moveSpeed;
	if (mykey[1]) Dec -= moveSpeed;
	if (mykey[2]) RA -= moveSpeed;
	if (mykey[3]) RA += moveSpeed;
	if (mykey[4] && moveSpeed < 10) moveSpeed++;
	if (mykey[5] && moveSpeed > 0) moveSpeed--;


	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);                   //設定面的正、背面都填滿

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(0, 0, 30.0, 0, 0, 0, 0, 1, 0);                    //視線的座標及方向
	glTranslatef(0, 0, distance);                               //沿著z軸平移
	glRotatef((float)rot_y + (float)record_y, 1.0, 0.0, 0.0);   //以x軸當旋轉軸
	glRotatef((float)rot_x + (float)record_x, 0.0, 1.0, 0.0);   //以y軸當旋轉軸

	/**------------------ Draw 赤道儀基座 START----------------**/
	glPushMatrix();    // save global matrix
	glScaled(2.2, 4, 2.2);
	glutSolidCube(1.0);
	glPopMatrix();     // restore global matrix
	/**------------------ Draw 赤道儀基座 END------------------**/

	/**------------------ Draw 赤道儀極軸(赤經軸) START----------------**/
	glPushMatrix();    // save global matrix
	glTranslated(0, 2, 0);
	glRotated(66.5, 0, 0, 1);
	glRotated(90, 1, 0, 0);
	glTranslated(0, 0, -1);
	gluCylinder(gluNewQuadric(), 1.2f, 1.2f, 3.0f, 32, 32);

	glTranslated(0, 0, 3.0);
	gluCylinder(gluNewQuadric(), 1.2f, 0.6f, 1.0f, 32, 32); // 極望蓋子
	glTranslated(0, 0, 1.0);
	gluDisk(gluNewQuadric(), 0, 0.6, 32, 32);               // 極望蓋子

	glPopMatrix();     // restore global matrix
	/**------------------ Draw 赤道儀極軸(赤經軸) END------------------**/

	/**------------------ Draw 赤道儀赤緯軸 START----------------**/
	glPushMatrix();    // save global matrix
	glRotated(180 - 23.5, 0, 0, 1);
	glTranslated(3, -1, 0);

	glTranslated(0, -1, 0);
	glRotated(RA, 1, 0, 0);
	glTranslated(0, 1, 0);

	glPushMatrix();    // save 赤道儀赤緯軸 matrix
	glScaled(2.4, 5, 2.4);
	glutSolidCube(1.0);
	glPopMatrix();     // restore 赤道儀赤緯軸 matrix
	/**------------------ Draw 赤道儀赤緯軸 END------------------**/

	/**------------------ Draw 重錘杆&重錘 START----------------**/
	glPushMatrix();    // save 赤道儀赤緯軸 matrix

	glTranslated(0, 8, 0);
	glRotated(90, 1, 0, 0);
	gluCylinder(gluNewQuadric(), 0.2f, 0.2f, 6.0f, 64, 64); // HammerStick

	glTranslated(0, 0, HammerR);//由重錘杆底步向上移
	gluCylinder(gluNewQuadric(), HammerR, HammerR, HammerThick, 64, 64); // Hammer1
	gluDisk(gluNewQuadric(), 0, HammerR, 64, 64); // Hammer1底面
	glTranslated(0, 0, HammerThick);
	gluDisk(gluNewQuadric(), 0, HammerR, 64, 64); // Hammer1頂面

	glTranslated(0, 0, 0.5);
	gluCylinder(gluNewQuadric(), HammerR, HammerR, HammerThick, 64, 64); // Hammer2
	gluDisk(gluNewQuadric(), 0, HammerR, 64, 64); // Hammer2底面
	glTranslated(0, 0, HammerThick);
	gluDisk(gluNewQuadric(), 0, HammerR, 64, 64); // Hammer2頂面
	glPopMatrix();     // restore 赤道儀赤緯軸 matrix
	/**------------------ Draw 重錘杆&重錘 END------------------**/

	/**------------------ Draw 主鏡 START----------------**/
	glPushMatrix();    // save 赤道儀赤緯軸 matrix

	glRotated(90, 0, 1, 0);
	glTranslated(0, -4.5, -3);

	glTranslated(0, 0, 3);
	glRotated(Dec, 0, 1, 0);
	glTranslated(0, 0, -3);

	gluCylinder(gluNewQuadric(), 2.2f, 2.2f, 7.0f, 128, 128); // Telescope
	gluDisk(gluNewQuadric(), 0, 2.2, 128, 128); // Telescope背面

	glTranslated(0, 0, -0.5);
	gluCylinder(gluNewQuadric(), 1.0f, 1.0f, 0.6f, 64, 64); // 目鏡座
	gluDisk(gluNewQuadric(), 0, 1.0, 64, 64); // 目鏡座背面

	glTranslated(0, 0, -1.0);
	gluCylinder(gluNewQuadric(), 0.5f, 0.5f, 1.0f, 64, 64); // 目鏡座
	gluDisk(gluNewQuadric(), 0, 0.5, 64, 64); // 目鏡座背面

	glPopMatrix();     // restore 赤道儀赤緯軸 matrix
	/**------------------ Draw 主鏡 END------------------**/

	/**------------------ Draw 腳架 START--------------------**/
	glPopMatrix();   // restore global matrix

	glPushMatrix();  // save global matrix

	glTranslated(0, -6 * cos(35.0*DEG2RAD), -2.7);
	glRotated(35, 1, 0, 0);
	glScaled(1, 12, 0.2);
	glutSolidCube(1.0);// 腳架1
	glPopMatrix();   // restore global matrix

	glPushMatrix();  // save global matrix
	glRotated(120, 0, 1, 0);
	glTranslated(0, -6 * cos(35.0*DEG2RAD), -2.7);
	glRotated(35, 1, 0, 0);
	glScaled(1, 12, 0.2);
	glutSolidCube(1.0);// 腳架2
	glPopMatrix();   // restore global matrix

	glPushMatrix();  // save global matrix
	glRotated(240, 0, 1, 0);
	glTranslated(0, -6 * cos(35.0*DEG2RAD), -2.7);
	glRotated(35, 1, 0, 0);
	glScaled(1, 12, 0.2);
	glutSolidCube(1.0);// 腳架3
	glPopMatrix();   // restore global matrix

	glPushMatrix();  // save global matrix

	glBegin(GL_TRIANGLES); // Draw 置物三腳盤
	glVertex3f(0.0, -5.0, -2.7);
	glVertex3f(-2.7*sin(120 * DEG2RAD), -5.0, -2.7*cos(120 * DEG2RAD));
	glVertex3f(-2.7*sin(240 * DEG2RAD), -5.0, -2.7*cos(240 * DEG2RAD));
	glEnd();


	/**------------------ Draw 腳架 END  --------------------**/



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
	if (h == 0) h = 1;                        //阻止h為零，分母可不能為零啊
	glViewport(0, 0, w, h);                 //當視窗長寬改變時，畫面也跟著變
	rate = (float)w / (float)h;                //畫面視野變了，但內容不變形

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, rate, 1.0, 500.0);   //透視投影

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void Mouse(int button, int state, int x, int y)
{
	if (state)
	{
		record_x += x - old_rot_x;
		record_y += y - old_rot_y;

		rot_x = 0;   //沒有歸零會有不理想的結果
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

	glEnable(GL_LIGHTING);                                 //開燈

	// 設定發光體的光源的特性
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);      //環境光(Ambient Light)
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);      //散射光(Diffuse Light)
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);     //反射光(Specular Light)

	glLightfv(GL_LIGHT0, GL_POSITION, light_position);     //光的座標

	glEnable(GL_LIGHT0);
	glEnable(GL_DEPTH_TEST);                               //啟動深度測試
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
	// Light values and coordinates
	glEnable(GL_DEPTH_TEST);	// Hidden surface removal
	glFrontFace(GL_CCW);		// Counter clock-wise polygons face out

	// Enable lighting
	glEnable(GL_LIGHTING);

	// Enable color tracking
	glEnable(GL_COLOR_MATERIAL);

	// Set Material properties to follow glColor values
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

	// Black blue background
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}