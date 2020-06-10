#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "GLTools/GLTools.h"
#include "GLTools/math3d.h"
#include "GLTools/GLFrame.h"
#include "TypeDef.h"
#include "Drawer.h"
#include "glm.h"

int			WinNumber			= 0;                    //用來放置視窗代碼
  
int			accumlateX			= 0;					   //沿X軸旋轉的累積角度
int			old_rot_x			= 0;                       //剛按下滑鼠時的視窗座標x
int			old_rot_y			= 0;				       //剛按下滑鼠時的視窗座標y
int			rot_x				= 0;                       //拖曳後的相對座標，用這決定要旋轉幾度
int			rot_y				= 0;
int			record_x			= 0;                       //紀錄上一次旋轉的角度x
int			record_y			= 0;                       //紀錄上一次旋轉的角度y

float		distance			= -30;                     //在平移矩陣(glTranslatef();)中使用
float		fLightPos[]			= { 20, 20, 20, 1.0f };    //光源的位置
float		fLightPosMirror[]   = { 20, -20, 20, 1.0f };   //倒影光源的位置
float		fViewingLightPos[]  = { 0, 0, 0, 1.0f };    


GLFrame     sun;
GLFrame     flashlight;
char		mss[50];									   //放字串
double		RA					= 0.0;					   //赤經
double		Dec					= -90.0;					   //赤緯
double		moveSpeed			= 0.2;			  		   //馬達移動速度
float		target_RA			= 0.0;					   //GOTO-目的地RA
float		target_Dec			= 0.0;					   //GOTO-目的地Dec
double		angle				= 35;					   //角架張角 MAX=35, min=0
bool		mykey[6]			= { false };			   //記錄按鍵按下狀態
bool		noLightMode			= false;				   //Wire/Shading Mode
bool		FlashLight			= true;				       //Flashlight on/off
bool        polygonoffset		= true;				       //polygonoffset is on/off
bool		antiAlias			= true;					   //是否開啟反鋸齒


void SetLightSource(void);							  //設定光源屬性
void SetMaterial(GLfloat, GLfloat, GLfloat, GLfloat,  // Ka RGBA
				 GLfloat, GLfloat, GLfloat, GLfloat,  // Kd RGBA
			  	 GLfloat, GLfloat, GLfloat, GLfloat,  // Ks RGBA
				 GLfloat, GLfloat, GLfloat, GLfloat,  // Ke RGBA
				 GLfloat);						      // Se
void SetupRC();
void WindowSize(int, int);							  //負責視窗及繪圖內容的比例
void MyKeys(unsigned char, int, int);				  //獲取鍵盤輸入
void MyKeysUp(unsigned char, int, int);				  //獲取鍵盤彈起
void SpecialKeys(int key, int x, int y);			  //獲取特殊鍵輸入
void Mouse(int, int, int, int);						  //獲取滑鼠按下和放開時的訊息
void MotionMouse(int, int);							  //獲取滑鼠按下期間的訊息
void Display(void);									  //畫面刷新
void ShowInfo();									  //顯示訊息於螢幕上(赤經、赤緯等)
void PrintText(char*, float, float, float);           //印字
void UpdateCoordinate();								  //更新赤經赤緯

int main(int argc, char *argv[])
{
	glutInit( & argc, argv );

	printf("|---------------Control---------------|\n");
	printf("  滑鼠   | 拖拉調整視角                \n");
	printf("  Z, X   | 調整遠近                    \n");
	printf("  A, D   | 調整赤經 (RA)               \n");
	printf("  W, S   | 調整赤緯 (Dec)              \n");
	printf("  [, ]   | 調整腳架張角                \n");
	printf("  +, -   | 調整馬達速度 (Motor Speed)  \n");
	printf("  G      | GoTo 自動追蹤太陽           \n");
	printf("  P      | Park 歸位至初始位置         \n");
	printf("  C      | Crazy 瘋狂亂移動所有可動關節\n");
	printf("  方向鍵 | 點光源平移                  \n");
	printf("  PgUp   | 點光源向上                  \n");
	printf("  PgDown | 點光源向下                  \n");
	printf("|---------------Control---------------|\n\n");
	printf("|---------------Setting---------------|\n");
	printf("  L      | Shading on/off      (光源)  \n");
	printf("  K      | Antialias on/off    (反鋸齒)\n");
	printf("  J      | PolygonOffset on/off(實心)  \n");
	printf("  H      | Flashlight on/off   (手電筒)\n");
	printf("  Esc    | 關閉程式                    \n");
	printf("|---------------Setting---------------|\n\n");
	printf("     政大天文社、政大資訊科學系 許書軒\n");

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_MULTISAMPLE);
	glutInitWindowSize(windowWidth, windowHeight);     //視窗長寬
	glutInitWindowPosition(300, 150);                  //視窗左上角的位置
	WinNumber = glutCreateWindow("Telescope Model -- SSARCandy");   

	glutReshapeFunc(WindowSize);
	glutKeyboardFunc(MyKeys);
	glutKeyboardUpFunc(MyKeysUp);
	glutSpecialFunc(SpecialKeys);
	glutMouseFunc(Mouse);
	glutMotionFunc(MotionMouse);
	glutDisplayFunc(Display);

	SetLightSource();
	SetMaterial(.7, .7, .7, 1.0, 
				.64, .6, .6, 1.0, 
				.52, .5, .52, 1.0,
				.05, .05, .05, 1.0, 
				45.0);
	SetupRC();

	//glEnable(GL_MULTISAMPLE);
	glutMainLoop();

	return 0;
}

///////////////////////////////////////////////////////////


void DrawFlashlight(void){
	///////////////////
	// 畫光源(手電筒)
	if (noLightMode) return;
	glPushMatrix();
		flashlight.ApplyActorTransform();
		glColor4ub(200, 200, 0, 255);
		glutSolidSphere(.50, 15, 15);
		glLightfv(GL_LIGHT1, GL_POSITION, fViewingLightPos);
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

void Display(void) {
	noLightMode ? glClearColor(0.5, 0.5, 0.5, 1.0) : glClearColor(0.8, 0.8, 0.8, 1.0);//根據是否開光影來塗背景顏色

	UpdateCoordinate(); //控制 RA, Dec 的改變

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
			Draw::Telescope(noLightMode, polygonoffset, {RA, Dec, angle, 24, 1.8, 0.7}); //畫望遠鏡
			glDisable(GL_POLYGON_OFFSET_FILL);

			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			polygonoffset = false;
			Draw::Telescope(noLightMode, polygonoffset, {RA, Dec, angle, 24, 1.8, 0.7}); //畫望遠鏡
			Draw::Sun(noLightMode, sun);
			polygonoffset = true;
		}
		else{
			Draw::Sun(noLightMode, sun);
			Draw::Telescope(noLightMode, polygonoffset, {RA, Dec, angle, 24, 1.8, 0.7}); //畫望遠鏡
		}
		Draw::Ground(noLightMode);	//畫地板格線
	}
	else{
		Draw::Sun(noLightMode, sun);
		Draw::Telescope(noLightMode, false, {RA, Dec, angle, 24, 1.8, 0.7}); //畫望遠鏡
		//// Move light under floor to light the "reflected" world
		glLightfv(GL_LIGHT2, GL_POSITION, fLightPosMirror);
		glPushMatrix();
			glFrontFace(GL_CW);  // geometry is mirrored, swap orientation
				glScalef(1.0f, -1.0f, 1.0f);
				glTranslated(0, 22.5, 0);
				Draw::Sun(noLightMode, sun); //畫地板反光的太陽
				Draw::Telescope(noLightMode, false, {RA, Dec, angle, 24, 1.8, 0.7}); //畫地板反光的望遠鏡
			glFrontFace(GL_CCW);
		glPopMatrix();
		Draw::Ground(noLightMode);												
		glLightfv(GL_LIGHT2, GL_POSITION, fLightPos);// Restore correct lighting and draw the world correctly
	}

	glPushMatrix();// save global matrix
		glTranslated(25, -2.5, 0);
		glColor4ub(20, 60, 30, 255);
		glScaled(22,22,22);
		glRotated(-90, 0, 1, 0);

		SetMaterial(.1, .1, .1, 1.0,
					.5, .5, .5, 1.0,
					.5, .5, .5, 1.0,
					.0, .0, .0, 1.0,
					0.0);
		// glmDraw(m, GLM_SMOOTH);//GLM_FLAT  GLM_SMOOTH
		SetMaterial(.7, .7, .7, 1.0,
					.64, .6, .6, 1.0,
					.52, .5, .52, 1.0,
					.05, .05, .05, 1.0,
					45.0);
	glPopMatrix(); // restore global matrix
		DrawFlashlight();

	glPushMatrix();// save global matrix
		ShowInfo();// 在螢幕上寫字
	glPopMatrix(); // restore global matrix

	glutSwapBuffers();
}

void ShowInfo(){
	glDisable(GL_LIGHTING);
	glRotatef(-(float)rot_x - (float)record_x, 0.0, 1.0, 0.0);   //以y軸當旋轉軸，改變座標系對應至畫面
	glRotatef(-(float)rot_y - (float)record_y, 1.0, 0.0, 0.0);   //以x軸當旋轉軸，改變座標系對應至畫面
	glTranslated(0, 0, -distance);

	glTranslated(-15.58, 10.9, 0);
	sprintf(mss, "Motor Speed: %.2f", moveSpeed);
	PrintText(mss, 0.0, 0.7, 0.0);

	glTranslated(0.0, -1, 0);
	// 修正顯示的赤經成實際的赤經格式
	float rRA;
	int RA_h, RA_m, RA_s;
	RA > 0 ? rRA = RA : rRA = 360 - RA;
	RA_h = rRA / 15.0;
	RA_m = (rRA - RA_h * 15) * 4.0;
	RA_s = (rRA - RA_h * 15 - RA_m / 4) * 15;

	sprintf(mss, "RA  : %.2dh %.2dm %.2ds", RA_h, RA_m, RA_s);
	PrintText(mss, 0.0, 0.7, 0.0);

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
	// sprintf(mss, "Dec: %.0f* %.2f' %.2f  %.2f", fViewingLightPos[0], fViewingLightPos[1], fViewingLightPos[2], fViewingLightPos[3]);
	PrintText(mss, 0.0, 0.7, 0.0);


	if(!noLightMode) glEnable(GL_LIGHTING);

}

void PrintText(char* str, float r, float g, float b){
	glColor3f(r, g, b);     //set font color
	glRasterPos2i(0, 0);    //set font start position
	for (unsigned i = 0; i<strlen(str); i++)		
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, str[i]);
}

void UpdateCoordinate(){
	if (mykey[0]) Dec += moveSpeed;
	if (mykey[1]) Dec -= moveSpeed;
	if (mykey[2]) RA -= moveSpeed;
	if (mykey[3]) RA += moveSpeed;
	
	if (RA < -360) RA += 360;
	if (RA > 360) RA -= 360;

	if (Dec < -360) Dec += 360;
	if (Dec > 360) Dec -= 360;
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

	UpdateCoordinate();

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

void MyKeys(unsigned char key, int x, int y)
{
	switch (key)
	{
		case 'z':
		case 'Z':
			if (distance < 10)
				distance += 1;
			break;
		case 'x':
		case 'X':
			distance -= 1;
			break;
		case 'h':
		case 'H':
			FlashLight = !FlashLight;
			FlashLight ? glEnable(GL_LIGHT1) : glDisable(GL_LIGHT1);
			break;
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
			glDisable(GL_LIGHT2);
			glDisable(GL_LIGHTING);
			glDisable(GL_DEPTH_TEST);
			glutDestroyWindow(WinNumber);
			exit(0);
			break;
	}
	glutPostRedisplay();
}

void MyKeysUp(unsigned char key, int x, int y){
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
	if (key == GLUT_KEY_UP){
		fViewingLightPos[0] += 0.2;
		flashlight.MoveForward(0.2);
	}
	if (key == GLUT_KEY_DOWN){
		fViewingLightPos[0] -= 0.2;
		flashlight.MoveForward(-0.2);
	}
	if (key == GLUT_KEY_LEFT){
		fViewingLightPos[1] += 0.2;
		flashlight.MoveRight(0.2);
	}
	if (key == GLUT_KEY_RIGHT){
		fViewingLightPos[1] -= 0.2;
		flashlight.MoveRight(-0.2);
	}
	if (key == GLUT_KEY_PAGE_UP){
		fViewingLightPos[2] += 0.2;
		flashlight.MoveUp(0.2);
	}
	if (key == GLUT_KEY_PAGE_DOWN){
		fViewingLightPos[2] -= 0.2;
		flashlight.MoveUp(-0.2);
	}

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
	float light_Ka1[] = { 0, 0, 0, 1.0 };
	float light_Kd1[] = { 1.0, 1.0, .50, 1.0 };
	float light_Ks1[] = { 1.0, 1.0, .50, 1.0 };

	float light_Ka2[] = { .50, .50, .50, 1.0 };
	float light_Kd2[] = { .50, .50, .50, 1.0 };
	float light_Ks2[] = { .5, .5, .5, 1.0 };

	glEnable(GL_LIGHTING);                            //開燈
	// 設定發光體的光源的特性
	glLightfv(GL_LIGHT1, GL_AMBIENT, light_Ka1);      //環境光(Ambient Light)
	glLightfv(GL_LIGHT1, GL_DIFFUSE, light_Kd1);      //散射光(Diffuse Light)
	glLightfv(GL_LIGHT1, GL_SPECULAR, light_Ks1);     //反射光(Specular Light)
	glLightfv(GL_LIGHT1, GL_POSITION, fViewingLightPos);     //光的座標
	glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, 0.02);

	glLightfv(GL_LIGHT2, GL_AMBIENT, light_Ka2);      //環境光(Ambient Light)
	glLightfv(GL_LIGHT2, GL_DIFFUSE, light_Kd2);      //散射光(Diffuse Light)
	glLightfv(GL_LIGHT2, GL_SPECULAR, light_Ks2);     //反射光(Specular Light)
	glLightfv(GL_LIGHT2, GL_POSITION, fLightPos);     //光的座標

	glEnable(GL_LIGHT1);
	glEnable(GL_LIGHT2);
	glEnable(GL_DEPTH_TEST);                               //啟動深度測試
	glEnable(GL_NORMALIZE);
	glEnable(GL_RESCALE_NORMAL);

}

void SetMaterial(GLfloat Ka_r, GLfloat Ka_g, GLfloat Ka_b, GLfloat Ka_a,
		 		 GLfloat Kd_r, GLfloat Kd_g, GLfloat Kd_b, GLfloat Kd_a, 
				 GLfloat Ks_r, GLfloat Ks_g, GLfloat Ks_b, GLfloat Ks_a,
				 GLfloat Ke_r, GLfloat Ke_g, GLfloat Ke_b, GLfloat Ke_a,
				 GLfloat Se)
{
	GLfloat material_Ka[] = { Ka_r, Ka_g, Ka_b, Ka_a };
	GLfloat material_Kd[] = { Kd_r, Kd_g, Kd_b, Kd_a };
	GLfloat material_Ks[] = { Ks_r, Ks_g, Ks_b, Ks_a };
	GLfloat material_Ke[] = { Ke_r, Ke_g, Ke_b, Ke_a };
	GLfloat material_Se[] = { Se };
	glMaterialfv(GL_FRONT, GL_AMBIENT, material_Ka);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, material_Kd);
	glMaterialfv(GL_FRONT, GL_SPECULAR, material_Ks);
	glMaterialfv(GL_FRONT, GL_EMISSION, material_Ke);
	glMaterialfv(GL_FRONT, GL_SHININESS, material_Se);

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
	flashlight.SetOrigin(0, 10, 0);
}