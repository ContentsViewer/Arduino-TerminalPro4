
#include "TerminalPro4CommonSetting.h"

void MesScr(char *c);
void ClearScr();

void setup() {
  SetupTerminalPro4();
  
  MesScr("ControllerTest");
}

void loop() {
  byte cR, cL;
  
  ctrlRight.Stick(&cR, A_BUTTON + B_BUTTON + X_BUTTON + Y_BUTTON);
  ctrlLeft.Stick(&cL, UP_BUTTON + DOWN_BUTTON + RIGHT_BUTTON + LEFT_BUTTON);
 

  if(cR & A_BUTTON){
    MesScr("A");
  }
  
  if(cR & B_BUTTON){
    MesScr("B");
  }
  
  if(cR & X_BUTTON){
    MesScr("X");
  }
  
  if(cR & Y_BUTTON){
    MesScr("Y");
  }
  
  if(cL & UP_BUTTON){
    MesScr("U");
  }
  
  if(cL & DOWN_BUTTON){
    MesScr("D");
  }
  
  if(cL & RIGHT_BUTTON){
    MesScr("R");
  }
  
  if(cL & LEFT_BUTTON){
    MesScr("L");
  }
  delay(10);
  
}

void ClearScr(){
  glcdCtrl.canvas.color = false;
  glcdCtrl.canvas.Boxf(0, 0, glcdCtrl.canvas.SizeX(), glcdCtrl.canvas.SizeY());
}

void MesScr(char *c){
  ClearScr();
  
  glcdCtrl.canvas.color = true;
  glcdCtrl.canvas.Pos(0, 0);
  glcdCtrl.canvas.Mes(c);
  glcdCtrl.Draw();
}
