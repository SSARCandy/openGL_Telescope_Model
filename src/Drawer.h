#pragma once

#include "GLTools/GLTools.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include <array>

#include "TypeDef.h"
#include "GLTools/math3d.h"
#include "GLTools/GLFrame.h"
#include "glm.h"

namespace Draw {

struct CubeInfo {
  float size;
  float sX;
  float sY;
  float sZ;
  std::array<float, 3> rgb;
};

struct TelescopeInfo {
  double RA;
  double Dec;
  double tripod_angle;
  int Slice;
  float HammerR;
  float HammerThick;
};

// 畫地板格線 - Draw a gridded ground
void Ground(bool noLightMode) {
  GLfloat fExtent = 200.0f;
  GLfloat fStep = 10.0f;
  GLfloat y = -11.2f;
  GLint xLine, zLine, iLine;

  if (!noLightMode) {
    int i = 0;
    for (zLine = -fExtent; zLine <= fExtent; zLine += fStep) {
      for (xLine = -fExtent; xLine <= fExtent; xLine += fStep) {
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
  glColor4ub(200, 200, 200, 255);

  for (iLine = -fExtent; iLine <= fExtent; iLine += fStep) {
    glBegin(GL_LINES);
    glVertex3f(iLine, y + 0.001, fExtent);  // Draw Z lines
    glVertex3f(iLine, y + 0.001, -fExtent);

    glVertex3f(fExtent, y + 0.001, iLine);
    glVertex3f(-fExtent, y + 0.001, iLine);
    glEnd();
  }
}

// 畫光源(太陽)
void Sun(bool noLightMode, GLFrame& sun) {
  glPushMatrix();
  sun.ApplyActorTransform();
  glDisable(GL_LIGHTING);
  glColor4ub(255, 64, 64, 255);
  noLightMode ? glutWireSphere(2.0, 16, 16) : glutSolidSphere(2.0, 16, 16);
  if (!noLightMode) glEnable(GL_LIGHTING);
  glPopMatrix();
}

// 客製化 Wire/Solid Cubes
// Draw customized Wire/Solid Cubes
void Cube(bool noLightMode, bool polygonoffset, CubeInfo info) {
  if (!noLightMode) {
    glColor4ub(info.rgb[0] + 10, info.rgb[1] + 10, info.rgb[2] + 10, 255);
    glScaled(info.sX, info.sY, info.sZ);
    glutSolidCube(1.0);
    glColor4ub(info.rgb[0], info.rgb[1], info.rgb[2], 255);
    glutWireCube(1.0);
    return;
  }

  if (!polygonoffset)
    glColor4ub(info.rgb[0] + 10, info.rgb[1] + 10, info.rgb[2] + 10, 255);
  glPushMatrix();
  glScaled(info.sX, info.sY, info.sZ);
  if (polygonoffset) glutSolidCube(0.99);
  glutWireCube(1.0);
  glPopMatrix();

  // 畫網格
  if (polygonoffset) return;
  for (GLfloat i = 0; i < info.sX; i += 0.5) {
    glPushMatrix();
    glScaled(i, info.sY, info.sZ);
    glutWireCube(1.0);
    glPopMatrix();
  }
  for (GLfloat i = 0; i < info.sY; i += 0.5) {
    glPushMatrix();
    glScaled(info.sX, i, info.sZ);
    glutWireCube(1.0);
    glPopMatrix();
  }
  for (GLfloat i = 0; i < info.sZ; i += 0.5) {
    glPushMatrix();
    glScaled(info.sX, info.sY, i);
    glutWireCube(1.0);
    glPopMatrix();
  }
}

// 畫望遠鏡 or 望遠鏡影子
void Telescope(bool noLightMode, bool polygonoffset, TelescopeInfo info) {
  // clang-format off
  const auto [RA, Dec, angle, Slice, HammerR, HammerThick] = info;
  const auto p = polygonoffset;
  glPushMatrix();
  float translateUP = 11 * cos(angle * DEG2RAD) - 11 * cos(35 * DEG2RAD);
  glTranslated(0, translateUP, 0);

  /**------------------ Draw 赤道儀基座 START----------------**/
  glPushMatrix();  // save global matrix
    Draw::Cube(noLightMode, polygonoffset, {1.0, 2.3, 3.2, 2.0, {80, 0, 0}});
  glPopMatrix();  // restore global matrix

  glPushMatrix();  // save global matrix
    if (!p) glColor4ub(70, 70, 70, 255);
    glTranslated(0, -1.6, 0);
    glRotated(90, 1, 0, 0);
    gluCylinder(gluNewQuadric(), 1.7f, 1.7f, 0.5f, Slice, 6);  /////////
    gluDisk(gluNewQuadric(), 0, 1.7f, Slice, 12);              /////////
    glTranslated(0, 0, 0.5);                                   //底座
    gluDisk(gluNewQuadric(), 0, 1.7f, Slice, 12);              /////////
  glPopMatrix();  // restore global matrix
  /**------------------ Draw 赤道儀基座 END------------------**/

  /**------------------ Draw 赤道儀極軸(赤經軸) START----------------**/
  glPushMatrix();  // save global matrix
    if (!p) glColor4ub(90, 10, 10, 255);
    glTranslated(0, 2, 0);
    glRotated(66.5, 0, 0, 1);
    glRotated(90, 1, 0, 0);
    glTranslated(0, 0, -1.5);
    gluCylinder(gluNewQuadric(), 1.2f, 1.2f, 3.5f, Slice, 16);  // 赤經軸
    glTranslated(0, 0, 3.5);
    gluDisk(gluNewQuadric(), 0, 1.2, Slice, 16);  // 蓋子

    if (!p) glColor4ub(0, 0, 0, 255);                          ////////////////
    gluCylinder(gluNewQuadric(), 0.3f, 0.3f, 0.5f, Slice, 8);  ////////////////
    glTranslated(0, 0, 0.5);                                   //極軸望遠鏡
    gluDisk(gluNewQuadric(), 0, 0.3, Slice, 8);  //極軸望遠鏡背面
    glTranslated(0, 0, -4.0);                    ////////////////

    glPushMatrix();
      if (!p) glColor4ub(0, 0, 0, 255);
      gluCylinder(gluNewQuadric(), 1.21f, 1.21f, 0.5f, Slice, 8);  // 赤經、赤緯軸交接處
    glPopMatrix();

    if (!p) glColor4ub(0, 0, 255, 200);
    glTranslated(0, 0, 3.5);
    gluCylinder(gluNewQuadric(), 1.2f, 0.6f, 1.0f, Slice, 8);  // 極望蓋子
    glTranslated(0, 0, 1.0);
    gluDisk(gluNewQuadric(), 0, 0.6, Slice, 8);  // 極望蓋子
    if (!p) glColor4ub(82, 0, 0, 255);
  glPopMatrix();  // restore global matrix
  /**------------------ Draw 赤道儀極軸(赤經軸) END------------------**/

  /**------------------ Draw 赤道儀赤緯軸 START----------------**/
  glPushMatrix();  // save global matrix
    glRotated(180 - 23.5, 0, 0, 1);
    glTranslated(3.5, -1.3, 0);

    glTranslated(0, -0.5, 0);  //調整旋轉中心點
    glRotated(RA, 1, 0, 0);    //根據赤經旋轉
    glTranslated(0, 0.5, 0);   //調整旋轉中心點

    glPushMatrix();  // save 赤道儀赤緯軸 matrix
      Draw::Cube(noLightMode, polygonoffset, {1.0, 2.4, 4, 2.4, {80, 0, 0}});
      glPopMatrix();  // restore 赤道儀赤緯軸 matrix

      glPushMatrix();  // save 赤道儀赤緯軸 matrix
        if (!p) glColor4ub(0, 0, 0, 255);
        glTranslated(0, -1.9, 0);
        glRotated(90, 1, 0, 0);
        gluCylinder(gluNewQuadric(), 1.7f, 1.7f, 0.5f, Slice, 6);  /////////
        gluDisk(gluNewQuadric(), 0, 1.7f, Slice, 12);              /////////
        glTranslated(0, 0, 0.5);                                   //載物台
        gluDisk(gluNewQuadric(), 0, 1.7f, Slice, 12);              /////////
      glPopMatrix();  // restore 赤道儀赤緯軸 matrix
      /**------------------ Draw 赤道儀赤緯軸 END------------------**/

      /**------------------ Draw 重錘杆&重錘 START----------------**/
      glPushMatrix();  // save 赤道儀赤緯軸 matrix
        if (!p) glColor4ub(70, 70, 70, 255);
        glTranslated(0, 8, 0);
        glRotated(90, 1, 0, 0);
        gluCylinder(gluNewQuadric(), 0.2f, 0.2f, 6.0f, 8, 32);  // HammerStick

        if (!p) glColor4ub(70, 50, 50, 255);
        glTranslated(0, 0, 1);  //由重錘杆底步向上移
        gluCylinder(gluNewQuadric(), HammerR, HammerR, HammerThick, Slice,
                    8);                                   // Hammer1
        gluDisk(gluNewQuadric(), 0, HammerR, Slice, 16);  // Hammer1底面
        glTranslated(0, 0, HammerThick);
        gluDisk(gluNewQuadric(), 0, HammerR, Slice, 16);  // Hammer1頂面

        glTranslated(0, 0, 0.5);
        gluCylinder(gluNewQuadric(), HammerR, HammerR, HammerThick, Slice,
                    8);                                   // Hammer2
        gluDisk(gluNewQuadric(), 0, HammerR, Slice, 16);  // Hammer2底面
        glTranslated(0, 0, HammerThick);
        gluDisk(gluNewQuadric(), 0, HammerR, Slice, 16);  // Hammer2頂面
      glPopMatrix();  // restore 赤道儀赤緯軸 matrix
      /**------------------ Draw 重錘杆&重錘 END------------------**/

      /**------------------ Draw 主鏡 START----------------**/
      glPushMatrix();  // save 赤道儀赤緯軸 matrix
        glRotated(90, 0, 1, 0);
        glTranslated(0, -4.7, -3);
        glTranslated(0, 0, 3);         //調整選中心轉點
        glRotated(90 + Dec, 0, 1, 0);  //根據赤緯做旋轉
        glTranslated(0, 0, -3);        //調整選轉中心點

        if (!p) glColor4ub(20, 20, 20, 255);
        // gltDrawUnitAxes();
        gluCylinder(gluNewQuadric(), 2.2f, 2.2f, 7.0f, Slice, 16);  // Telescope
        gluDisk(gluNewQuadric(), 0.3, 2.2, Slice, 24);              // Telescope背面

        glPushMatrix();
          glTranslated(0, 2.3, 3.3);
          Draw::Cube(noLightMode, polygonoffset, {1.0, 1, 0.7, 5, {80, 60, 60}});
          if (!p) glColor4ub(20, 20, 20, 255);
        glPopMatrix();

        glPushMatrix();
          glTranslated(0, 0, 6);
          if (!p) glColor4ub(100, 100, 100, 100);       // Glass - Transparency
          gluDisk(gluNewQuadric(), 0, 2.2, Slice, 16);  // Telescope正面(卡賽格林式)
          if (!p) glColor4ub(20, 20, 20, 255);

          glTranslated(0, 0, 0.5);
          gluCylinder(gluNewQuadric(), 1.0f, 1.0f, 0.5f, Slice, 16);  // 前反射面
          glTranslated(0, 0, 0.5);
          gluDisk(gluNewQuadric(), 0, 1.0, Slice, 16);
        glPopMatrix();

        glPushMatrix();
          glTranslated(0, 0, -0.5);
          gluCylinder(gluNewQuadric(), 1.0f, 1.0f, 0.6f, Slice, 8);  // 目鏡座
          gluDisk(gluNewQuadric(), 0.5, 1.0, Slice, 8);              // 目鏡座背面

          glTranslated(0, 0, -1.0);
          gluCylinder(gluNewQuadric(), 0.5f, 0.5f, 1.0f, Slice, 8);  // 目鏡
          if (!p) glColor4ub(100, 100, 100, 150);      // Glass - Transparency
          gluDisk(gluNewQuadric(), 0, 0.5, Slice, 8);  // 目鏡背面
          if (!p) glColor4ub(20, 20, 20, 255);
        glPopMatrix();

        glRotated(45, 0, 0, 1);
        glTranslated(0, -3, 0);
        gluCylinder(gluNewQuadric(), 0.5f, 0.5f, 3.0f, Slice, 8);  // 尋星鏡
        gluDisk(gluNewQuadric(), 0, 0.5, Slice, 16);               // 尋星鏡背面
        glTranslated(0, 0, 3.0);
        gluDisk(gluNewQuadric(), 0, 0.5, Slice, 16);  // 尋星鏡正面
        glTranslated(0, 0, -3.5);
        gluCylinder(gluNewQuadric(), 0.3f, 0.3f, 1.0f, Slice, 4);  // 目鏡
        gluDisk(gluNewQuadric(), 0, 0.3, Slice, 8);                // 目鏡背面

        glTranslated(0, 0.6, 1.5);
        Draw::Cube(noLightMode, polygonoffset, {1.0, 1, 1, 1, {10, 10, 10}});
      glPopMatrix();  // restore 赤道儀赤緯軸 matrix
    glPopMatrix();   // restore global matrix
    /**------------------ Draw 主鏡 END------------------**/

    /**------------------ Draw 腳架 START--------------------**/
    glPushMatrix();  // save global matrix
      glTranslated(0, -1.6, -1.0);
      glRotated(angle, 1, 0, 0);
      glTranslated(0, -5.9, 0);
      Draw::Cube(noLightMode, polygonoffset, {1.0, 1, 11.5, 0.2, {70, 70, 70}});
    glPopMatrix();  // restore global matrix

    glPushMatrix();  // save global matrix
      glRotated(120, 0, 1, 0);
      glTranslated(0, -1.6, -1.0);
      glRotated(angle, 1, 0, 0);
      glTranslated(0, -5.9, 0);
      Draw::Cube(noLightMode, polygonoffset, {1.0, 1, 11.5, 0.2, {70, 70, 70}});
    glPopMatrix();  // restore global matrix

    glPushMatrix();  // save global matrix
      glRotated(240, 0, 1, 0);
      glTranslated(0, -1.6, -1.0);
      glRotated(angle, 1, 0, 0);
      glTranslated(0, -5.9, 0);
      Draw::Cube(noLightMode, polygonoffset, {1.0, 1, 11.5, 0.2, {70, 70, 70}});
    glPopMatrix();   // restore global matrix
    glPushMatrix();  // save global matrix

      double offset = -11.5 * sin(angle * DEG2RAD) / 2 - (1 - angle / 35);
      if (!p) glColor4ub(10.0, 10.0, 10.0, 255);
      glBegin(GL_TRIANGLES);  // Draw 置物三腳盤
      if (!p) glColor4ub(255.0, 0.0, 0.0, 255);
      glVertex3f(0.0, -5.0, offset);
      if (!p) glColor4ub(0.0, 255.0, 0.0, 255);
      glVertex3f(offset * sin(120 * DEG2RAD), -5.0, offset * cos(120 * DEG2RAD));
      if (!p) glColor4ub(0.0, 0.0, 255.0, 255);
      glVertex3f(offset * sin(240 * DEG2RAD), -5.0, offset * cos(240 * DEG2RAD));
      glEnd();
      if (!p) glColor4ub(40, 40, 40, 255);

    glPopMatrix();  // restore global matrix
    /**------------------ Draw 腳架 END  --------------------**/

  glPopMatrix();
  // clang-format on
}

}  // namespace Draw