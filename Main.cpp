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


int			WinNumber			= NULL;                    //用來放置視窗代碼
const float DEG2RAD				= 3.14159 / 180.0;         //角度轉弧度
const float windowWidth			= 800;				       //視窗預設寬度
const float windowHeight		= 600;				       //視窗預設高度
  
int			accumlateX			= 0;					   //沿X軸旋轉的累積角度
int			old_rot_x			= 0;                       //剛按下滑鼠時的視窗座標x
int			old_rot_y			= 0;				       //剛按下滑鼠時的視窗座標y
int			rot_x				= 0;                       //拖曳後的相對座標，用這決定要旋轉幾度
int			rot_y				= 0;
int			record_x			= 0;                       //紀錄上一次旋轉的角度x
int			record_y			= 0;                       //紀錄上一次旋轉的角度y

float		distance			= -10;                     //在平移矩陣(glTranslatef();)中使用
float		fLightPos[]			= { 20, 20, 20, 0.0f };    //光源的位置
float		fLightPosMirror[]   = { 20, -20, 20, 0.0f };   //倒影光源的位置

GLFrame     sun;								
char		mss[50];									   //放字串
double		RA					= 0.0;					   //赤經
double		Dec					= 0.0;					   //赤緯
double		moveSpeed			= 0.2;			  		   //馬達移動速度
float		target_RA			= 0.0;					   //GOTO-目的地RA
float		target_Dec			= 0.0;					   //GOTO-目的地Dec
double		angle				= 35;					   //角架張角 MAX=35, min=0
bool		mykey[6]			= { false };			   //記錄按鍵按下狀態
bool		noLightMode			= true;				       //Wire/Shading Mode
bool        polygonoffset		= true;				   //polygonoffset is on/off
bool		antiAlias			= true;					   //是否開啟反鋸齒

const float HammerR				= 1.8;
const float HammerThick			= 0.7;
const int   Slice				= 24;
const float sun_RA				= 42.7;
const float sun_Dec				= 156.0;

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
void showInfo();									  //顯示訊息於螢幕上(赤經、赤緯等)
void printText(char*, float, float, float);           //印字
void updateRA_Dec();								  //更新赤經赤緯
void DrawGround(void);								  //畫地板格線
void DrawTelescope(bool);					          //畫望遠鏡 or 望遠鏡影子
void DrawSun(void);									  //畫太陽	
void drawCube(bool, float, float, float, float, float, float, float);//客製化 Wire/Solid Cubes

int main()
{
	printf("\
|---------------Control---------------|\n\
  上下鍵 | 調整遠近                    \n\
  滑鼠   | 拖拉調整視角                \n\
  A, D   | 調整赤經 (RA)               \n\
  W, S   | 調整赤緯 (Dec)              \n\
  [, ]   | 調整腳架張角                \n\
  +, -   | 調整馬達速度 (Motor Speed)  \n\
  G      | GoTo 自動追蹤太陽           \n\
  P      | Park 歸位至初始位置         \n\
  C      | Crazy 瘋狂亂移動所有可動關節\n\
|---------------Control---------------|\n\n\
|---------------Setting---------------|\n\
  L      | Shading on/off      (光源)  \n\
  K      | Antialias on/off    (反鋸齒)\n\
  J      | PolygonOffset on/off(實心)  \n\
  Esc    | 關閉程式                    \n\
|---------------Setting---------------|\n\n\
   政大天文社、政大資訊科學系 許書軒");

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_MULTISAMPLE);
	glutInitWindowSize(windowWidth, windowHeight);     //視窗長寬
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

	/**------------------ Draw 赤道儀基座 START----------------**/
	glPushMatrix();    // save global matrix
		drawCube(noLightMode, 1.0, 2.3, 3.2, 2.0, 80, 0, 0);
	glPopMatrix();     // restore global matrix

	glPushMatrix();    // save global matrix
		if(!p) glColor4ub(70, 70, 70, 255);
		glTranslated(0, -1.6, 0);
		glRotated(90, 1, 0, 0);
		gluCylinder(gluNewQuadric(), 1.7f, 1.7f, 0.5f, Slice, 6); /////////
		gluDisk(gluNewQuadric(), 0, 1.7f, Slice, 12);             /////////
		glTranslated(0, 0, 0.5);							      //底座
		gluDisk(gluNewQuadric(), 0, 1.7f, Slice, 12);             /////////
	glPopMatrix();     // restore global matrix
	/**------------------ Draw 赤道儀基座 END------------------**/

	/**------------------ Draw 赤道儀極軸(赤經軸) START----------------**/
	glPushMatrix();    // save global matrix
		if (!p) glColor4ub(90, 10, 10, 255);
		glTranslated(0, 2, 0);
		glRotated(66.5, 0, 0, 1);
		glRotated(90, 1, 0, 0);
		glTranslated(0, 0, -1.5);
		gluCylinder(gluNewQuadric(), 1.2f, 1.2f, 3.5f, Slice, 16); // 赤經軸
		glTranslated(0, 0, 3.5);
		gluDisk(gluNewQuadric(), 0, 1.2, Slice, 16);               // 蓋子

		if (!p) glColor4ub(0, 0, 0, 255);                                 ////////////////
		gluCylinder(gluNewQuadric(), 0.3f, 0.3f, 0.5f, Slice, 8); ////////////////
		glTranslated(0, 0, 0.5);								  //極軸望遠鏡
		gluDisk(gluNewQuadric(), 0, 0.3, Slice, 8);               //極軸望遠鏡背面
		glTranslated(0, 0, -4.0);                                 ////////////////

		glPushMatrix();
			if (!p) glColor4ub(0, 0, 0, 255);
			gluCylinder(gluNewQuadric(), 1.21f, 1.21f, 0.5f, Slice, 8); // 赤經、赤緯軸交接處
		glPopMatrix();

		if (!p) glColor4ub(0, 0, 255, 200);
		glTranslated(0, 0, 3.5);
		gluCylinder(gluNewQuadric(), 1.2f, 0.6f, 1.0f, Slice, 8); // 極望蓋子
		glTranslated(0, 0, 1.0);
		gluDisk(gluNewQuadric(), 0, 0.6, Slice, 8);               // 極望蓋子
		if (!p) glColor4ub(82, 0, 0, 255);
	glPopMatrix();     // restore global matrix
	/**------------------ Draw 赤道儀極軸(赤經軸) END------------------**/

	/**------------------ Draw 赤道儀赤緯軸 START----------------**/
	glPushMatrix();    // save global matrix
		glRotated(180 - 23.5, 0, 0, 1);
		glTranslated(3.5, -1.3, 0);

		glTranslated(0, -0.5, 0); //調整旋轉中心點
		glRotated(RA, 1, 0, 0);   //根據赤經旋轉
		glTranslated(0, 0.5, 0);  //調整旋轉中心點

		glPushMatrix();    // save 赤道儀赤緯軸 matrix
			drawCube(noLightMode, 1.0, 2.4, 4, 2.4, 80, 0, 0);
		glPopMatrix();     // restore 赤道儀赤緯軸 matrix

		glPushMatrix();    // save 赤道儀赤緯軸 matrix
			if (!p) glColor4ub(0, 0, 0, 255);
			glTranslated(0, -1.9, 0);
			glRotated(90, 1, 0, 0);
			gluCylinder(gluNewQuadric(), 1.7f, 1.7f, 0.5f, Slice, 6);  /////////
			gluDisk(gluNewQuadric(), 0, 1.7f, Slice, 12);              /////////
			glTranslated(0, 0, 0.5);								   //載物台
			gluDisk(gluNewQuadric(), 0, 1.7f, Slice, 12);              /////////
		glPopMatrix();     // restore 赤道儀赤緯軸 matrix
	/**------------------ Draw 赤道儀赤緯軸 END------------------**/

	/**------------------ Draw 重錘杆&重錘 START----------------**/
		glPushMatrix();    // save 赤道儀赤緯軸 matrix
			if (!p) glColor4ub(70, 70, 70, 255);
			glTranslated(0, 8, 0);
			glRotated(90, 1, 0, 0);
			gluCylinder(gluNewQuadric(), 0.2f, 0.2f, 6.0f, 8, 32); // HammerStick

			if (!p) glColor4ub(70, 50, 50, 255);
			glTranslated(0, 0, 1);//由重錘杆底步向上移
			gluCylinder(gluNewQuadric(), HammerR, HammerR, HammerThick, Slice, 8); // Hammer1
			gluDisk(gluNewQuadric(), 0, HammerR, Slice, 16); // Hammer1底面
			glTranslated(0, 0, HammerThick);
			gluDisk(gluNewQuadric(), 0, HammerR, Slice, 16); // Hammer1頂面

			glTranslated(0, 0, 0.5);
			gluCylinder(gluNewQuadric(), HammerR, HammerR, HammerThick, Slice, 8); // Hammer2
			gluDisk(gluNewQuadric(), 0, HammerR, Slice, 16); // Hammer2底面
			glTranslated(0, 0, HammerThick);
			gluDisk(gluNewQuadric(), 0, HammerR, Slice, 16); // Hammer2頂面
		glPopMatrix();     // restore 赤道儀赤緯軸 matrix
	/**------------------ Draw 重錘杆&重錘 END------------------**/

	/**------------------ Draw 主鏡 START----------------**/
		glPushMatrix(); // save 赤道儀赤緯軸 matrix
			glRotated(90, 0, 1, 0);
			glTranslated(0, -4.7, -3);
			glTranslated(0, 0, 3);      //調整選中心轉點
			glRotated(90 + Dec, 0, 1, 0); //根據赤緯做旋轉
			glTranslated(0, 0, -3);     //調整選轉中心點

			if (!p) glColor4ub(20, 20, 20, 255);
			//gltDrawUnitAxes();
			gluCylinder(gluNewQuadric(), 2.2f, 2.2f, 7.0f, Slice, 16); // Telescope
			gluDisk(gluNewQuadric(), 0.3, 2.2, Slice, 24);             // Telescope背面

			glPushMatrix();
				glTranslated(0, 2.3, 3.3);
				drawCube(noLightMode, 1.0, 1, 0.7, 5, 80, 60, 60);
				if (!p) glColor4ub(20, 20, 20, 255);
			glPopMatrix();

			glPushMatrix();
				glTranslated(0, 0, 6);
				if (!p) glColor4ub(100, 100, 100, 100);               // Glass - Transparency
				gluDisk(gluNewQuadric(), 0, 2.2, Slice, 16);  // Telescope正面(卡賽格林式)
				if (!p) glColor4ub(20, 20, 20, 255);

				glTranslated(0, 0, 0.5);
				gluCylinder(gluNewQuadric(), 1.0f, 1.0f, 0.5f, Slice, 16); // 前反射面
				glTranslated(0, 0, 0.5);
				gluDisk(gluNewQuadric(), 0, 1.0, Slice, 16);
			glPopMatrix();

			glPushMatrix();
				glTranslated(0, 0, -0.5);
				gluCylinder(gluNewQuadric(), 1.0f, 1.0f, 0.6f, Slice, 8); // 目鏡座
				gluDisk(gluNewQuadric(), 0.5, 1.0, Slice, 8);             // 目鏡座背面

				glTranslated(0, 0, -1.0);
				gluCylinder(gluNewQuadric(), 0.5f, 0.5f, 1.0f, Slice, 8); // 目鏡
				if (!p) glColor4ub(100, 100, 100, 150);						      // Glass - Transparency
				gluDisk(gluNewQuadric(), 0, 0.5, Slice, 8);				  // 目鏡背面
				if (!p) glColor4ub(20, 20, 20, 255);
			glPopMatrix();

			glRotated(45, 0, 0, 1);
			glTranslated(0, -3, 0);
			gluCylinder(gluNewQuadric(), 0.5f, 0.5f, 3.0f, Slice, 8); // 尋星鏡
			gluDisk(gluNewQuadric(), 0, 0.5, Slice, 16);               // 尋星鏡背面
			glTranslated(0, 0, 3.0);
			gluDisk(gluNewQuadric(), 0, 0.5, Slice, 16);               // 尋星鏡正面
			glTranslated(0, 0, -3.5);
			gluCylinder(gluNewQuadric(), 0.3f, 0.3f, 1.0f, Slice, 4);  // 目鏡
			gluDisk(gluNewQuadric(), 0, 0.3, Slice, 8);                // 目鏡背面

			glTranslated(0, 0.6, 1.5);
			drawCube(noLightMode, 1.0, 1, 1, 1, 10, 10, 10);
		glPopMatrix();     // restore 赤道儀赤緯軸 matrix
	/**------------------ Draw 主鏡 END------------------**/

	/**------------------ Draw 腳架 START--------------------**/
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
	glBegin(GL_TRIANGLES); // Draw 置物三腳盤
		if (!p) glColor4ub(255.0, 0.0, 0.0, 255);
		glVertex3f(0.0, -5.0, offset);
		if (!p) glColor4ub(0.0, 255.0, 0.0, 255);
		glVertex3f(offset*sin(120 * DEG2RAD), -5.0, offset*cos(120 * DEG2RAD));
		if (!p) glColor4ub(0.0, 0.0, 255.0, 255);
		glVertex3f(offset*sin(240 * DEG2RAD), -5.0, offset*cos(240 * DEG2RAD));
	glEnd();
	if (!p) glColor4ub(40, 40, 40, 255);

	glPopMatrix(); // restore global matrix
	/**------------------ Draw 腳架 END  --------------------**/

	glPopMatrix();
}

void DrawSun(void){
	///////////////////
	// 畫光源(太陽)
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
			glRotatef((float)rot_y + (float)record_y, 1.0, 0.0, 0.0);   //以x軸當旋轉軸
		}
		else{
			rot_y = 0;
			if (-accumlateX > asin(11.2 / (30 - distance)) / DEG2RAD)
				record_y = -asin(11.2 / (30 - distance)) / DEG2RAD;
			else
				record_y = asin(11.2 / (30 - distance)) / DEG2RAD + 180;
			glRotatef(record_y, 1.0, 0.0, 0.0);   //以x軸當旋轉軸
		}
		glRotatef((float)rot_x + (float)record_x, 0.0, 1.0, 0.0);   //以y軸當旋轉軸
	}
	else{
		glRotatef((float)rot_y + (float)record_y, 1.0, 0.0, 0.0);   //以x軸當旋轉軸
		glRotatef((float)rot_x + (float)record_x, 0.0, 1.0, 0.0);   //以y軸當旋轉軸
	}
}

void Display(void)
{
	noLightMode ? glClearColor(0.5, 0.5, 0.5, 1.0) : glClearColor(0.8, 0.8, 0.8, 1.0);//根據是否開光影來塗背景顏色

	updateRA_Dec(); //控制 RA, Dec 的改變

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	noLightMode ? glPolygonMode(GL_FRONT_AND_BACK, GL_LINE) : glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // Shading = true/false

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(0, 0, 30.0, 0, 0, 0, 0, 1, 0);                    //視線的座標及方向
	glTranslatef(0, 0, distance);                               //沿著z軸平移

	CameraView(noLightMode);

	if (noLightMode){
		glDisable(GL_LIGHTING);
		if (polygonoffset){
			glEnable(GL_POLYGON_OFFSET_FILL);
			glPolygonOffset(.5, .5);
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glColor3f(0.35f, 0.35f, 0.35f);
			DrawTelescope(polygonoffset);//畫望遠鏡
			glDisable(GL_POLYGON_OFFSET_FILL);

			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			polygonoffset = false;
			DrawTelescope(polygonoffset);//畫望遠鏡
			DrawSun();
			polygonoffset = true;
		}
		else{
			DrawSun();
			DrawTelescope(polygonoffset);//畫望遠鏡
		}
		DrawGround();	//畫地板格線
	}
	else{
		DrawSun();
		DrawTelescope(false);//畫望遠鏡
		//// Move light under floor to light the "reflected" world
		glLightfv(GL_LIGHT1, GL_POSITION, fLightPosMirror);
		glPushMatrix();
			glFrontFace(GL_CW);  // geometry is mirrored, swap orientation
				glScalef(1.0f, -1.0f, 1.0f);
				glTranslated(0, 22.5, 0);
				DrawSun();		 //畫地板反光的太陽
				DrawTelescope(false); //畫地板反光的望遠鏡
			glFrontFace(GL_CCW);
		glPopMatrix();
		DrawGround();												
		glLightfv(GL_LIGHT1, GL_POSITION, fLightPos);// Restore correct lighting and draw the world correctly
	}

	glPushMatrix();// save global matrix
		showInfo();// 在螢幕上寫字
	glPopMatrix(); // restore global matrix

	glutSwapBuffers();
}

void showInfo(){
	glDisable(GL_LIGHTING);
	glRotatef(-(float)rot_x - (float)record_x, 0.0, 1.0, 0.0);   //以y軸當旋轉軸，改變座標系對應至畫面
	glRotatef(-(float)rot_y - (float)record_y, 1.0, 0.0, 0.0);   //以x軸當旋轉軸，改變座標系對應至畫面
	glTranslated(0, 0, -distance);

	glTranslated(-15.58, 10.9, 0);
	sprintf(mss, "Motor Speed: %.2f", moveSpeed);
	printText(mss, 0.0, 0.7, 0.0);

	glTranslated(0.0, -1, 0);
	// 修正顯示的赤經成實際的赤經格式
	float rRA;
	int RA_h, RA_m, RA_s;
	RA > 0 ? rRA = RA : rRA = 360 - RA;
	RA_h = rRA / 15.0;
	RA_m = (rRA - RA_h * 15) * 4.0;
	RA_s = (rRA - RA_h * 15 - RA_m / 4) * 15;

	sprintf(mss, "RA  : %.2dh %.2dm %.2ds", RA_h, RA_m, RA_s);
	printText(mss, 0.0, 0.7, 0.0);

	glTranslated(0.0, -1, 0);
	// 修正顯示的赤緯成實際的赤緯
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

		// 畫網格
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
	if (h == 0) h = 1;                      //阻止h為零，分母可不能為零啊
	glViewport(0, 0, w, h);                 //當視窗長寬改變時，畫面也跟著變
	rate = (float)w / (float)h;             //畫面視野變了，但內容不變形

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
	glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambient);       //環境光(Ambient Light)
	glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse);       //散射光(Diffuse Light)
	glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular);     //反射光(Specular Light)

	glLightfv(GL_LIGHT1, GL_POSITION, fLightPos);     //光的座標

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