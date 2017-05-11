/*
*プログラム
*  最終更新日:
*    5.2.2016
*
*  説明:
*    GLCDControllerTestプログラムです
*    TerminalPro2専用プログラムです
*
*  更新履歴:
*    5.2.2016:
*      プログラム完成
*/
#include "TerminalPro4CommonSetting.h"

unsigned long updateTimePrev = 0;
int fpsPrev = 0;

int Power(int a, int power);
void Draw();
void PrintNumber(int num);

void setup() {
  SetupTerminalPro4();
}

void loop() {
  for(int i = 0; i < 32; i++){
    glcdCtrl.canvas.color = true;
    glcdCtrl.canvas.Line(63 - i * 2, 0, 0, i * 2);
    Draw();
    
    glcdCtrl.canvas.color = true;
    glcdCtrl.canvas.Line(0, i * 2, i * 2, 63);
    Draw();
    
    glcdCtrl.canvas.color = true;
    glcdCtrl.canvas.Line(i * 2, 63, 63, 63 - i * 2);
    Draw();
    
    glcdCtrl.canvas.color = true;
    glcdCtrl.canvas.Line(63, 63 - i * 2, 63 - i * 2, 0);
    Draw();
  }
  
  
  for(int i = 0; i < 32; i++){
    glcdCtrl.canvas.color = false;
    glcdCtrl.canvas.Line(63 - i * 2, 0, 0, i * 2);
    Draw();
    
    glcdCtrl.canvas.color = false;
    glcdCtrl.canvas.Line(0, i * 2, i * 2, 63);
    Draw();
    
    glcdCtrl.canvas.color = false;
    glcdCtrl.canvas.Line(i * 2, 63, 63, 63 - i * 2);
    Draw();
    
    glcdCtrl.canvas.color = false;
    glcdCtrl.canvas.Line(63, 63 - i * 2, 63 - i * 2, 0);
    Draw();
  }
}

void Draw() {
  glcdCtrl.canvas.color = true;
  glcdCtrl.canvas.Pos(64, 0);
  glcdCtrl.canvas.Mes("GLCD");
  glcdCtrl.canvas.Pos(70, 8);
  glcdCtrl.canvas.Mes("Controller");
  glcdCtrl.canvas.Pos(70, 16);
  glcdCtrl.canvas.Mes("Test");
  unsigned long updateTime = millis();
  
  glcdCtrl.canvas.color = true;
  glcdCtrl.canvas.Pos(75, 30);
  glcdCtrl.canvas.Mes("FPS:");
  int posX = glcdCtrl.canvas.posX - 15;
  int posY = glcdCtrl.canvas.posY + 8;
  
  glcdCtrl.canvas.color = false;
  glcdCtrl.canvas.Pos(posX, posY);
  PrintNumber(fpsPrev);
  
  glcdCtrl.canvas.color = true;
  glcdCtrl.canvas.Pos(posX, posY);
  int fps = int(1000 / (updateTime - updateTimePrev));
  PrintNumber(fps);
  glcdCtrl.Draw();

  updateTimePrev = updateTime;
  fpsPrev = fps;
}

void PrintNumber(int num) {
  int numberOfDigits = 1;
  int powered = 1;
  for (numberOfDigits = 0; num / powered != 0; numberOfDigits++, powered *= 10) {
  }

  powered = Power(10, numberOfDigits - 1);
  for (int i = 0; i < numberOfDigits; i++, powered /= 10) {
    int digit = num / powered;
    num %= powered;

    glcdCtrl.canvas.PutChar(0x30 + digit);

  }
}

int Power(int a, int power){
  int powered = 1;
  for(int i = 0; i < power; i++){
    powered *= a;
  }
  return powered;
}

