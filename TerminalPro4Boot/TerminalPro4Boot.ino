/*
  プログラム
   最終更新日:
     4.23.2017

   説明:
     TeriminalPro4用のブートです

   メモ:
     ctrl1: ABXY
     ctrl2: UDRL

   更新履歴:
    4.23.2017:
      プログラムの完成
*/


#include <SPI.h>
#include <SD.h>
#include "Controller.h"
#include "SketchWriter.h"

//各ボタンの設定
#define A_BUTTON 4
#define B_BUTTON 8
#define X_BUTTON 1
#define Y_BUTTON 2

#define UP_BUTTON 1
#define RIGHT_BUTTON 4
#define DOWN_BUTTON 8
#define LEFT_BUTTON 2


#define PIN_RESET_MAIN 3
#define PIN_RESET_MEDIA 4
#define PIN_RESET_OUTPUT 2

#define PIN_CTRL_LEFT 4
#define PIN_CTRL_RIGHT 5

#define PIN_SWITCH_SERIAL 6
#define PIN_SWITCH_SD 7

#define PIN_ACT_LED 5

#define SERIAL_SPEED_MESSENGER 19200

#define SKETCH_ROOT_DIRECTORY "Term4/Sketch/"
#define SERIAL_MONITOR_SKETCH_NAME "SerMon.hex"

enum MICON {
  MAIN,
  OTHER,
  MEDIA
};


enum STATE {
  START,
  MENU,
  SKETCH_WRITE,
  BACK_GROUND,
  SHOW_VERSION,
  HELP
};

STATE state;

Controller ctrlLeft;
Controller ctrlRight;

SketchWriter sketchWriter;

//関数プロトタイプ宣言
void ResetArduinoMain();
void ResetArduinoMedia();
void ResetArduinoOutput();

void ShutdownArduinoMain();
void ShutdownArduinoMedia();
void StartupArduinoMain();
void StartupArduinoMedia();

void RestartArduinoMain();
void RestartArduinoMedia();
void RestartAllArduino();

void SwitchSD(bool sw);
void SwitchSerial(bool sw);

void StateStart() ;
void StateMenu() ;
void StateSketchWrite();
void StateBackGround();
void StateHelp();
void StateShowVersion();

void StateSketchWriteSecondStep(MICON target);

bool SketchWrite(char *fileName, MICON target);

void ErrorBlink();

bool OpenNextFile(File *root, File *entry);
void StartApp();
bool InputAnyKey();
bool Confirm();

void setup() {

  //ResetPin初期化
  sketchWriter.SetResetPin(PIN_RESET_MAIN);
  sketchWriter.SetResetPin(PIN_RESET_MEDIA);
  sketchWriter.SetResetPin(PIN_RESET_OUTPUT);

  //Boot以外のマイコンをshutdown
  ShutdownArduinoMain();
  ShutdownArduinoMedia();

  //シリアルスイッチ初期化
  pinMode(PIN_SWITCH_SERIAL, OUTPUT);
  SwitchSerial(false);

  //SDスイッチ初期化
  pinMode(PIN_SWITCH_SD, OUTPUT);
  SwitchSD(false);

  //ACT_LED初期化
  pinMode(PIN_ACT_LED, OUTPUT);

  //Controller開始
  ctrlLeft.Begin(PIN_CTRL_LEFT, 4, DEFAULT);
  ctrlRight.Begin(PIN_CTRL_RIGHT, 4, DEFAULT);

  //SketchWriter開始
  sketchWriter.Begin(10);

  state = STATE::START;
}

void loop() {
  switch (state) {
    case START:
      StateStart();
      break;

    case MENU:
      StateMenu();
      break;

    case SKETCH_WRITE:
      StateSketchWrite();
      break;

    case BACK_GROUND:
    default:
      StateBackGround();
      break;

    case HELP:
      StateHelp();
      break;

    case SHOW_VERSION:
      StateShowVersion();
      break;
  }

}

//Boot立ち上がり時
void StateStart() {
  //SerialMonitorプログラム検索
  if (!SD.exists((String(SKETCH_ROOT_DIRECTORY) + SERIAL_MONITOR_SKETCH_NAME).c_str())) {
    ErrorBlink();
    StartApp();
    return;
  }

  //Wait
  bool input = false;
  for (int i = 0; i < 8; i++) {
    digitalWrite(PIN_ACT_LED, HIGH);
    delay(100);
    digitalWrite(PIN_ACT_LED, LOW);
    delay(300);
    if (InputAnyKey()) {
      input = true;
      break;
    }
  }

  if (!input) {
    StartApp();
    return;
  }

  if (!SketchWrite(SERIAL_MONITOR_SKETCH_NAME, MICON::MAIN)) {
    ErrorBlink();
    StartApp();
    return;
  }

  //SerialSpeed
  Serial.begin(SERIAL_SPEED_MESSENGER);
  delay(2000);

  Serial.print(F("WelcomeTo\n"));
  Serial.print(F("TerminalPro4Boot\n"));

  delay(1000);
  state = MENU;
}

//BootMenu画面
void StateMenu() {
  Serial.print(F("\n[MENU]\n"));
  Serial.print(F("SelectItem>>\n"));

  char index = 0;
  char indexPrev = -1;

  //全項目数
  //項目を追加するときはここを増やしていくこと
  char indexAll = 3;

  for (;;) {
    if (indexPrev != index) {
      switch (index) {
        case 0:
          Serial.print(F("SketchWrite\r"));
          indexPrev = index;
          break;

        case 1:
          Serial.print(F("Help       \r"));
          indexPrev = index;
          break;

        case 2:
          Serial.print(F("Version    \r"));
          indexPrev = index;
          break;
      }
    }
    byte cR, cL;
    ctrlRight.Stick(&cR, A_BUTTON + B_BUTTON + X_BUTTON + Y_BUTTON);
    ctrlLeft.Stick(&cL, UP_BUTTON + DOWN_BUTTON + RIGHT_BUTTON + LEFT_BUTTON);

    if (cR) {
      switch (index) {
        case 0:
          state = SKETCH_WRITE;
          break;

        case 1:
          state = HELP;
          break;

        case 2:
          state = SHOW_VERSION;
          break;

        default:
          break;
      }
      break;
    }

    if (cL & RIGHT_BUTTON) {
      index++;
      if (index >= indexAll) {
        index = 0;
      }
    }

    if (cL & LEFT_BUTTON) {
      index--;
      if (index < 0) {
        index = indexAll - 1;
      }
    }

    delay(10);
  }
}

//スケッチ書き込み画面
void StateSketchWrite() {
  Serial.print(F("\n[SketchWrite]\n"));
  Serial.print(F("SelectTarget>>\n"));
  char index = 0;
  char indexPrev = -1;
  char indexAll = 3;

  for (;;) {
    if (indexPrev != index) {
      switch (index) {
        case 0:
          Serial.print(F("Main  \r"));
          indexPrev = index;
          break;

        case 1:
          Serial.print(F("Other \r"));
          indexPrev = index;
          break;

        case 2:
          Serial.print(F("Cancel\r"));
          indexPrev = index;
          break;
      }
    }
    byte cR, cL;
    ctrlRight.Stick(&cR, A_BUTTON + B_BUTTON + X_BUTTON + Y_BUTTON);
    ctrlLeft.Stick(&cL, UP_BUTTON + DOWN_BUTTON + RIGHT_BUTTON + LEFT_BUTTON);

    if (cR) {
      Serial.print(F("\n"));
      switch (index) {
        default:
        case 0:
          StateSketchWriteSecondStep(MICON::MAIN);
          break;

        case 1:
          StateSketchWriteSecondStep(MICON::OTHER);
          break;

        case 2:
          state = MENU;
          break;
      }
      break;
    }

    if (cL & RIGHT_BUTTON) {
      index++;
      if (index >= indexAll) {
        index = 0;
      }
    }

    if (cL & LEFT_BUTTON) {
      index--;
      if (index < 0) {
        index = indexAll - 1;
      }
    }

    delay(10);
  }
}

//Bootバックグラウンド
void StateBackGround() {

}

//Help画面
void StateHelp() {
  Serial.print(F("\n[Help]\n"));
  Serial.print(F("SketchWrite:\n"));
  Serial.print(F("Please select sketch(program)\n"));
  Serial.print(F("After sketch is uploaded, the aplication will start.\n"));

  //一時停止; ユーザ応答待ち
  Serial.print(F("Continue>>\n"));
  while (!InputAnyKey());
  Serial.print(F(" When you see 'Confirm>>' on the display,\n"));
  Serial.print(F("please check your decision again.\n"));

  //一時停止; ユーザー応答待ち
  Serial.print(F("Continue>>\n"));
  while (!InputAnyKey());
  state = MENU;

}
//Version表示画面
void StateShowVersion() {
  Serial.print(F("\n[Version]\n"));
  Serial.print(F("TerminalPro4Boot\n"));
  Serial.print(F("Version: 1.0\n"));

  //一時停止; ユーザ応答待ち
  Serial.print(F("Continue>>\n"));
  while (!InputAnyKey());

  Serial.print(F("LastUpdate: 4.23.2017\n"));
  Serial.print(F("CreatedBy\n"));
  Serial.print(F("KenO\n"));

  //一時停止; ユーザー応答待ち
  Serial.print(F("Continue>>\n"));
  while (!InputAnyKey());
  state = MENU;
}
void StateSketchWriteSecondStep(MICON target) {
  File sketchRoot = SD.open(SKETCH_ROOT_DIRECTORY);

  if (!sketchRoot) {
    Serial.print(F("FolderNotFound\n"));
    state = MENU;
    return;
  }

  File entry;
  if (!OpenNextFile(&sketchRoot, &entry)) {
    Serial.print(F("NoFile\n"));
    state = MENU;
    return;
  }
  else {
    Serial.print(F("SelectSkecth>>\n"));
    Serial.print(entry.name());
    Serial.print(F("     \r"));
  }

  for (;;) {
    byte cR, cL;
    ctrlRight.Stick(&cR, A_BUTTON + B_BUTTON + X_BUTTON + Y_BUTTON);
    ctrlLeft.Stick(&cL, UP_BUTTON + DOWN_BUTTON + RIGHT_BUTTON + LEFT_BUTTON);

    if (cR) {
      if (Confirm()) {
        Serial.print(F("Uploading...\n"));
        delay(100);
        switch (target) {
          case MAIN:
            SketchWrite(entry.name(), MAIN);
            break;

          case OTHER:
            SketchWrite(entry.name(), OTHER);
            break;

          default:
            break;
        }
        StartApp();
        return;
      }
      
      state = MENU;
      return;
    }

    if (cL & RIGHT_BUTTON) {
      if (!OpenNextFile(&sketchRoot, &entry)) {
        Serial.print(F("NoMoreFile\n"));
        state = MENU;
        return;
      }

      Serial.print(entry.name());
      Serial.print(F("     \r"));
    }
  }

  return;
}

//指定されたマイコンに指定されたスケッチを書き込みます
bool SketchWrite(char *fileName, MICON target) {
  //各ターゲットごとのファイル処理
  String filePath = String(SKETCH_ROOT_DIRECTORY) + fileName;

  //スケッチロード
  if (!sketchWriter.SketchLoad(filePath.c_str())) {
    return false;
  }

  Serial.end();
  Serial.begin(115200);

  //スケッチ書き込み
  bool done = false;
  switch (target) {
    case MAIN:
      SwitchSerial(false);
      ResetArduinoMain();
      sketchWriter.GetSync();
      sketchWriter.SketchWrite();
      sketchWriter.SketchClose();
      done = true;
      break;

    case OTHER:
      SwitchSerial(true);
      ResetArduinoMain();
      sketchWriter.GetSync();
      sketchWriter.SketchWrite();
      sketchWriter.SketchClose();
      done = true;
      break;

    default:
      break;
  }
  return done;
}

//指定したフォルダ内での次のファイルを取得します
//ファイルが存在しないときはfalseを返します
bool OpenNextFile(File *root, File *entry) {
  (*entry) = root -> openNextFile();
  if (!(*entry)) {
    return false;
  }
  if (entry -> isDirectory()) {
    return false;
  }
  return true;
}


//Userからの認証を待機します
bool Confirm() {
  Serial.print(F("\nConfirm>>\n"));
  char index = 0;
  char indexPrev = -1;
  char indexAll = 2;

  for (;;) {
    if (indexPrev != index) {
      switch (index) {
        case 0:
          Serial.print(F("No \r"));
          indexPrev = index;
          break;

        case 1:
          Serial.print(F("Yes\r"));
          indexPrev = index;
          break;
      }
    }
    byte cR, cL;
    ctrlRight.Stick(&cR, A_BUTTON + B_BUTTON + X_BUTTON + Y_BUTTON);
    ctrlLeft.Stick(&cL, UP_BUTTON + DOWN_BUTTON + RIGHT_BUTTON + LEFT_BUTTON);

    if (cR) {
      Serial.print(F("\n"));
      switch (index) {
        default:
        case 0:
          return false;
          break;

        case 1:
          return true;
          break;
      }
    }

    if (cL & RIGHT_BUTTON) {
      index++;
      if (index >= indexAll) {
        index = 0;
      }
    }

    if (cL & LEFT_BUTTON) {
      index--;
      if (index < 0) {
        index = indexAll - 1;
      }
    }
    
    delay(10);
  }
}

//何かキーが押されたか検出します
bool InputAnyKey() {
  byte c1, c2;
  ctrlRight.Stick(&c1, A_BUTTON + B_BUTTON + X_BUTTON + Y_BUTTON);
  ctrlLeft.Stick(&c2, UP_BUTTON + DOWN_BUTTON + RIGHT_BUTTON + LEFT_BUTTON);
  if (c1 | c2) {
    return true;
  }

  return false;
}

//Mainマイコンのプログラムを実行します
void StartApp() {
  //まずIO関係
  SwitchSD(true);
  SwitchSerial(true);

  delay(100);

  //そしてマイコン
  RestartAllArduino();

  state = BACK_GROUND;
  delay(3000);
}

void ResetArduinoMain() {
  sketchWriter.ResetArduino(PIN_RESET_MAIN);
}

void ResetArduinoMedia() {
  sketchWriter.ResetArduino(PIN_RESET_MEDIA);
}

void ResetArduinoOutput() {
  sketchWriter.ResetArduino(PIN_RESET_OUTPUT);
}

void SwitchSD(bool sw) {
  digitalWrite(PIN_SWITCH_SD, sw);
}


void SwitchSerial(bool sw) {
  digitalWrite(PIN_SWITCH_SERIAL, sw);
}


void ShutdownArduinoMain() {
  digitalWrite(PIN_RESET_MAIN, LOW);
}

void ShutdownArduinoMedia() {
  digitalWrite(PIN_RESET_MEDIA, LOW);
}

void StartupArduinoMain() {
  digitalWrite(PIN_RESET_MAIN, HIGH);
}

void StartupArduinoMedia() {
  digitalWrite(PIN_RESET_MEDIA, HIGH);
}

void RestartArduinoMain() {
  ShutdownArduinoMain();
  delay(10);
  StartupArduinoMain();
}

void RestartArduinoMedia() {
  ShutdownArduinoMedia();
  delay(10);
  StartupArduinoMedia();
}

void RestartAllArduino() {
  RestartArduinoMain();
  RestartArduinoMedia();
}

void ErrorBlink() {
  digitalWrite(PIN_ACT_LED, HIGH);
  delay(2000);
  digitalWrite(PIN_ACT_LED, LOW);
}

