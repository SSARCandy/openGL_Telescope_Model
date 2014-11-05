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

int			WinNumber			= NULL;               //用來放置視窗代碼
const float DEG2RAD				= 3.14159 / 180.0;    //角度轉弧度
const float windowWidth			= 800;				  //視窗預設寬度
const float windowHeight		= 600;				  //視窗預設高度

int			old_rot_x			= 0;                  //剛按下滑鼠時的視窗座標
int			old_rot_y			= 0;
int			rot_x				= 0;                  //拖曳後的相對座標，用這決定要旋轉幾度
int			rot_y				= 0;
int			record_x			= 0;                  //紀錄上一次旋轉的角度
int			record_y			= 0;

float		distance			= 0;                  //在平移矩陣(glTranslatef();)中使用
float		light_position[]	= { 20, 20, 20 };     //光源的位置

char		mss[50];								  //放字串
double		RA					= 0.0;				  //赤經
double		Dec					= 90.0;				  //赤緯
double		moveSpeed			= 0.5;				  //馬達移動速度
bool		mykey[6]			= { false };		  //記錄按鍵按下狀態

const float HammerR				= 1.8;
const float HammerThick			= 0.7;
const int   Slice				= 64;
GLFrame     sun;


void DrawGround(void);								  //畫地板格線
void SetLightSource(void);							  //設定光源屬性
void SetMaterial(void);								  //設定材質屬性
void SetupRC();
void WindowSize(int, int);							  //負責視窗及繪圖內容的比例
void myKeys(unsigned char, int, int);				  //獲取鍵盤輸入
void myKeysUp(unsigned char, int, int);				  //獲取鍵盤彈起
void SpecialKeys(int key, int x, int y);			  //獲取特殊鍵輸入
void Mouse(int, int, int, int);						  //獲取滑鼠按下和放開時的訊息
void MotionMouse(int, int);							  //獲取滑鼠按下期間的訊息
void Display(void);									  //畫面刷新
void printText(char*, float, float, float);
void updateRA_Dec();

int main()
{
	printf("上下鍵調整遠近\n\
滑鼠調整視角\n\n\
+, -調整馬達速度\n\
W, S調整赤緯\n\
A, D調整赤經\n\n\
Esc關閉程式\n");
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(windowWidth, windowHeight);                     //視窗長寬
	glutInitWindowPosition(300, 150);                  //視窗左上角的位置
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
	glClearColor(0.8, 0.8, 0.8, 1.0);      //塗背景
	glColor4ub(40, 40, 40, 1.0);
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	updateRA_Dec();


	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);                   //設定面的正、背面都填滿

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(0, 0, 30.0, 0, 0, 0, 0, 1, 0);                    //視線的座標及方向
	glTranslatef(0, 0, distance);                               //沿著z軸平移
	glRotatef((float)rot_y + (float)record_y, 1.0, 0.0, 0.0);   //以x軸當旋轉軸
	glRotatef((float)rot_x + (float)record_x, 0.0, 1.0, 0.0);   //以y軸當旋轉軸
	DrawGround();												//畫地板格線


	///////////////////
	// 畫光源(太陽)
	glPushMatrix();
		sun.ApplyActorTransform();
		glDisable(GL_LIGHTING);
		glColor4ub(255, 64, 64, 1.0);
		glutWireSphere(2.0, 16, 16);
		glEnable(GL_LIGHTING);
	glPopMatrix();

	/**------------------ Draw 赤道儀基座 START----------------**/
	glPushMatrix();    // save global matrix
		glColor4ub(82, 0, 0, 1.0);
		glScaled(2.3, 3.2, 2.0);
		glutSolidCube(1.0);
	glPopMatrix();     // restore global matrix

	glPushMatrix();    // save global matrix
		glColor4ub(70, 70, 70, 1.0);
		glTranslated(0, -1.6, 0);
		glRotated(90, 1, 0, 0);
		gluCylinder(gluNewQuadric(), 1.7f, 1.7f, 0.5f, Slice, Slice); /////////
		gluDisk(gluNewQuadric(), 0, 1.7f, Slice, Slice);              /////////
		glTranslated(0, 0, 0.5);									  //底座
		gluDisk(gluNewQuadric(), 0, 1.7f, Slice, Slice);              /////////
	glPopMatrix();     // restore global matrix
	/**------------------ Draw 赤道儀基座 END------------------**/

	/**------------------ Draw 赤道儀極軸(赤經軸) START----------------**/
	glPushMatrix();    // save global matrix
		glColor4ub(80, 0, 0, 1.0);
		glTranslated(0, 2, 0);
		glRotated(66.5, 0, 0, 1);
		glRotated(90, 1, 0, 0);
		glTranslated(0, 0, -1.5);
		gluCylinder(gluNewQuadric(), 1.2f, 1.2f, 3.5f, Slice, Slice); // 赤經軸

		glPushMatrix();
		//	glTranslated(0, 0, -2.5);
			glColor4ub(0, 0, 0,0);
			gluCylinder(gluNewQuadric(), 1.21f, 1.21f, 0.5f, Slice, Slice); // 赤經軸
		glPopMatrix();

		glColor4ub(0, 0, 128, 0.1);
		glTranslated(0, 0, 3.5);
		gluCylinder(gluNewQuadric(), 1.2f, 0.6f, 1.0f, Slice, Slice); // 極望蓋子
		glTranslated(0, 0, 1.0);
		gluDisk(gluNewQuadric(), 0, 0.6, Slice, Slice);               // 極望蓋子
		glColor4ub(82, 0, 0, 1.0);
	glPopMatrix();     // restore global matrix
	/**------------------ Draw 赤道儀極軸(赤經軸) END------------------**/

	/**------------------ Draw 赤道儀赤緯軸 START----------------**/
	glPushMatrix();    // save global matrix
		glRotated(180 - 23.5, 0, 0, 1);
		glTranslated(3.5, -1.3, 0);

		glTranslated(0, -0.5, 0); //調整旋轉中心點
		glRotated(RA, 1, 0, 0); //根據赤經旋轉
		glTranslated(0, 0.5, 0);  //調整旋轉中心點

		glPushMatrix();    // save 赤道儀赤緯軸 matrix
			glScaled(2.4, 4, 2.4);
			glutSolidCube(1.0);
		glPopMatrix();     // restore 赤道儀赤緯軸 matrix

		glPushMatrix();    // save 赤道儀赤緯軸 matrix
			glColor4ub(0, 0, 0, 0);
			glTranslated(0, -1.9, 0);
			glRotated(90, 1, 0, 0);
			gluCylinder(gluNewQuadric(), 1.7f, 1.7f, 0.5f, Slice, Slice); /////////
			gluDisk(gluNewQuadric(), 0, 1.7f, Slice, Slice);              /////////
			glTranslated(0, 0, 0.5);									  //載物台
			gluDisk(gluNewQuadric(), 0, 1.7f, Slice, Slice);              /////////
		glPopMatrix();     // restore 赤道儀赤緯軸 matrix

	/**------------------ Draw 赤道儀赤緯軸 END------------------**/

	/**------------------ Draw 重錘杆&重錘 START----------------**/
		glPushMatrix();    // save 赤道儀赤緯軸 matrix
			glColor4ub(70, 70, 70, 1.0);

			glTranslated(0, 8, 0);
			glRotated(90, 1, 0, 0);
			gluCylinder(gluNewQuadric(), 0.2f, 0.2f, 6.0f, Slice, Slice); // HammerStick

			glColor4ub(70, 50, 50, 1.0);

			glTranslated(0, 0, 1);//由重錘杆底步向上移
			gluCylinder(gluNewQuadric(), HammerR, HammerR, HammerThick, Slice, Slice); // Hammer1
			gluDisk(gluNewQuadric(), 0, HammerR, Slice, Slice); // Hammer1底面
			glTranslated(0, 0, HammerThick);
			gluDisk(gluNewQuadric(), 0, HammerR, Slice, Slice); // Hammer1頂面

			glTranslated(0, 0, 0.5);
			gluCylinder(gluNewQuadric(), HammerR, HammerR, HammerThick, Slice, Slice); // Hammer2
			gluDisk(gluNewQuadric(), 0, HammerR, Slice, Slice); // Hammer2底面
			glTranslated(0, 0, HammerThick);
			gluDisk(gluNewQuadric(), 0, HammerR, Slice, Slice); // Hammer2頂面
		glPopMatrix();     // restore 赤道儀赤緯軸 matrix
	/**------------------ Draw 重錘杆&重錘 END------------------**/

	/**------------------ Draw 主鏡 START----------------**/
		glPushMatrix();    // save 赤道儀赤緯軸 matrix
			glRotated(90, 0, 1, 0);
			glTranslated(0, -4.7, -3);
			glTranslated(0, 0, 3);   //調整選中心轉點
			glRotated(Dec, 0, 1, 0); //根據赤緯做旋轉
			glTranslated(0, 0, -3);  //調整選轉中心點
	
			glColor4ub(20, 20, 20, 1.0);
			gluCylinder(gluNewQuadric(), 2.2f, 2.2f, 7.0f, Slice, Slice); // Telescope
//			gltDrawUnitAxes();
			gluDisk(gluNewQuadric(), 0, 2.2, Slice, Slice); // Telescope背面
			
			glPushMatrix();
				glColor4ub(80, 60, 60, 1.0);
				glTranslated(0, 2.3, 3.3);
				glScaled(1, 0.7, 5);
				glutSolidCube(1.0);
				glColor4ub(20, 20, 20, 1.0);
			glPopMatrix();

			glPushMatrix();
				glTranslated(0, 0, 6);
				gluDisk(gluNewQuadric(), 0, 2.2, Slice, Slice); // Telescope正面(卡賽格林式)
				glTranslated(0, 0, 0.5);
				gluCylinder(gluNewQuadric(), 1.0f, 1.0f, 0.5f, Slice, Slice); // 前反射面
				glTranslated(0, 0, 0.5);
				gluDisk(gluNewQuadric(), 0, 1.0, Slice, Slice);
			glPopMatrix();

			glPushMatrix();
				glTranslated(0, 0, -0.5);
				gluCylinder(gluNewQuadric(), 1.0f, 1.0f, 0.6f, Slice, Slice); // 目鏡座
				gluDisk(gluNewQuadric(), 0, 1.0, Slice, Slice); // 目鏡座背面

				glTranslated(0, 0, -1.0);
				gluCylinder(gluNewQuadric(), 0.5f, 0.5f, 1.0f, Slice, Slice); // 目鏡
				gluDisk(gluNewQuadric(), 0, 0.5, Slice, Slice); // 目鏡背面
			glPopMatrix();

			glRotated(45, 0, 0, 1);
			glTranslated(0,-3,0);
			gluCylinder(gluNewQuadric(), 0.5f, 0.5f, 3.0f, Slice, Slice); // 尋星鏡
			gluDisk(gluNewQuadric(), 0, 0.5, Slice, Slice); // 尋星鏡背面
			glTranslated(0, 0, -0.5);
			gluCylinder(gluNewQuadric(), 0.3f, 0.3f, 1.0f, Slice, Slice); // 目鏡
			gluDisk(gluNewQuadric(), 0, 0.3, Slice, Slice); // 目鏡背面
	
			//glPopMatrix();
			//glPushMatrix();
			//glTranslated(0, 0, -2);
			//glScaled(2.4, 1, 2.4);
			//glutSolidCube(1.0);
			//glPopMatrix();
			glTranslated(0, 0.6, 1.5);
			glutSolidCube(0.5);

		glPopMatrix();     // restore 赤道儀赤緯軸 matrix

	/**------------------ Draw 主鏡 END------------------**/

	/**------------------ Draw 腳架 START--------------------**/
		const double offset = -3.2;
		glPopMatrix();   // restore global matrix
			glPushMatrix();  // save global matrix
				glColor4ub(70, 70, 70, 1.0);
				glTranslated(0, -6 * cos(35.0*DEG2RAD), offset);
				glRotated(35, 1, 0, 0);
				glScaled(1, 12, 0.2);
				glutSolidCube(1.0);// 腳架1
			glPopMatrix();   // restore global matrix

			glPushMatrix();  // save global matrix
				glRotated(120, 0, 1, 0);
				glTranslated(0, -6 * cos(35.0*DEG2RAD), offset);
				glRotated(35, 1, 0, 0);
				glScaled(1, 12, 0.2);
				glutSolidCube(1.0);// 腳架2
			glPopMatrix();   // restore global matrix

			glPushMatrix();  // save global matrix
				glRotated(240, 0, 1, 0);
				glTranslated(0, -6 * cos(35.0*DEG2RAD), offset);
				glRotated(35, 1, 0, 0);
				glScaled(1, 12, 0.2);
				glutSolidCube(1.0);// 腳架3
			glPopMatrix();   // restore global matrix
		glPushMatrix();  // save global matrix

		glColor4ub(10.0, 10.0, 10.0, 1.0);
		//glDisable(GL_LIGHTING);
		glBegin(GL_TRIANGLES); // Draw 置物三腳盤
		glVertex3f(0.0, -5.0, offset);
		glVertex3f(offset*sin(120 * DEG2RAD), -5.0, offset*cos(120 * DEG2RAD));
		glVertex3f(offset*sin(240 * DEG2RAD), -5.0, offset*cos(240 * DEG2RAD));
		glEnd();
		glColor4ub(40, 40, 40, 1.0);
		//glEnable(GL_LIGHTING);

	/**------------------ Draw 腳架 END  --------------------**/

	glPopMatrix();   // restore global matrix

	glPushMatrix();///在螢幕上寫字
		glRotatef(-(float)rot_x - (float)record_x, 0.0, 1.0, 0.0);   //以y軸當旋轉軸
		glRotatef(-(float)rot_y - (float)record_y, 1.0, 0.0, 0.0);   //以x軸當旋轉軸
		glTranslated(0, 0, -distance);

	//	gltDrawUnitAxes();
		glTranslated(-15.58, 10.9, 0);
		sprintf(mss, "RA  : %3.1f", RA );
		printText(mss, 0.0, 0.7, 0.0);

		glTranslated(0.0, -1, 0);
		sprintf(mss, "Dec: %3.1f", Dec);
		printText(mss, 0.0, 0.7, 0.0);
	glPopMatrix();   // restore global matrix


	glutSwapBuffers();
}

void printText(char* str, float r, float g, float b){
	glColor3f(r, g, b);  //set font color
	glRasterPos2i(0, 0);    //set font start position
	for (int i = 0; i<strlen(str); i++)		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, str[i]);
}

void updateRA_Dec(){
	if (mykey[0]) Dec += moveSpeed;
	if (mykey[1]) Dec -= moveSpeed;
	if (mykey[2]) RA -= moveSpeed;
	if (mykey[3]) RA += moveSpeed;
	if (mykey[4] && moveSpeed < 10) moveSpeed += 0.2;
	if (mykey[5] && moveSpeed > 0) moveSpeed -= 0.2;

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
	glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambient);      //環境光(Ambient Light)
	glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse);      //散射光(Diffuse Light)
	glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular);     //反射光(Specular Light)

	glLightfv(GL_LIGHT1, GL_POSITION, light_position);     //光的座標

	glEnable(GL_LIGHT1);
	glEnable(GL_DEPTH_TEST);                               //啟動深度測試
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

// This function does any needed initialization on the rendering
// context.
void SetupRC()
{
	glEnable(GL_BLEND);
	// Enable color tracking
	glEnable(GL_COLOR_MATERIAL);

	sun.SetOrigin(light_position[0], light_position[1], light_position[2]);
}