#include "GLCDController.h"
#include "MediaPlayerController.h"
#include "Controller.h"

#include "Arduino.h"

//---
//共通設定
//---
//各ボタンの設定
#define A_BUTTON 4
#define B_BUTTON 8
#define X_BUTTON 1
#define Y_BUTTON 2

#define UP_BUTTON 1
#define RIGHT_BUTTON 4
#define DOWN_BUTTON 8
#define LEFT_BUTTON 2

#define PIN_CTRL_RIGHT 5
#define PIN_CTRL_LEFT 4

#define PIN_MEDIA_STOP 14
#define PIN_MEDIA_STATE 13

Controller ctrlLeft;
Controller ctrlRight;

GLCDController glcdCtrl;

MediaPlayerController mediaCtrl;

void SetupTerminalPro4(){
  //---共通設定---
  //ピン設定
  GLCDController::Param param;
  param.pinRS = 12;
  param.pinRW = 11;
  param.pinE = 10;
  param.pinCS1 = 17;
  param.pinCS2 = 16;
  param.pinRST = 15;

  param.sizeX = 128;
  param.sizeY = 64;

  //グラフィックディスプレイを使う前に必ずする
  glcdCtrl.Begin(param);

  mediaCtrl.Begin(PIN_MEDIA_STOP, PIN_MEDIA_STATE);

  ctrlLeft.Begin(PIN_CTRL_LEFT, 4, DEFAULT);
  ctrlRight.Begin(PIN_CTRL_RIGHT, 4, DEFAULT);
  
}
