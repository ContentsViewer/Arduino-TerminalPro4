/*
*プログラム SerialMonitor
*  最終更新日:
*    4.23.2017
*
*  説明:
*    シリアル通信で送られてきた文字列をグラフィック液晶に表示します
*    文字表示モジュールとして使うことができるでしょう
*
*  導入方法:
*    1: 文字表示範囲を設定します
*    2: シリアル通信速度を設定します
*    3: グラフィック液晶のピンを設定します
*    4: モジュールとしたいArduinoにこのプログラムを書き込みます
*    5: Rxピンと文字列を送るマイコンのTxピンを接続します
*    6: 設定したシリアル通信速度で文字列を送るとその文字列がグラフィック液晶に表示されます
*
*  更新履歴:
*    4.23.2017:
*      プログラムの完成
*/

#include "TerminalPro4CommonSetting.h"


//文字表示範囲
#define COLUMN 21
#define ROW 8

//シリアル通信速度
#define SERIAL_SPEED 19200

//表示位置
int printPos = 0;

//メッセージバッファ
char message[ROW][COLUMN];
char messagePrev[ROW][COLUMN];

//カーソル
unsigned long cursorTime = 0;
byte cursorSwitch = 0;
int cursorPosPrev = 0;

void UpdateMessageRender();
void UpdateMessageReceive();
void PutMessage(char c);
void ScrollMessage(int numberOfSteps);

void setup() {
  SetupTerminalPro4();

  //messageを初期化
  for (int i = 0; i < ROW; i++) {
    for (int j = 0; j < COLUMN; j++) {
      message[i][j] = ' ';
      messagePrev[i][j] = ' ';
    }
  }

  //シリアル通信開始
  Serial.begin(SERIAL_SPEED);
}

void loop() {
  UpdateMessageReceive();
  UpdateMessageRender();

}


//シリアルデータ受信
void UpdateMessageReceive() {
  int ch = Serial.read();
  while(ch != -1){
    PutMessage(ch);
    ch = Serial.read();
  }
}

//画面更新
void UpdateMessageRender() {

  //描画位置を左上に
  glcdCtrl.canvas.Pos(0, 0);

  for (int i = 0; i < ROW; i++) {
    for (int j = 0; j < COLUMN; j++) {

      //画面に表示されている文字と異なるときのみ文字更新
      if (message[i][j] != messagePrev[i][j]) {
        glcdCtrl.canvas.color = 0;
        glcdCtrl.canvas.PutChar((byte)messagePrev[i][j]);
        glcdCtrl.canvas.posX -= 6;

        glcdCtrl.canvas.color = 1;
        glcdCtrl.canvas.PutChar((byte)message[i][j]);

        messagePrev[i][j] = message[i][j];
      }
      else {
        glcdCtrl.canvas.posX += 6;
      }
    }

    //次の行へ
    glcdCtrl.canvas.posY += 8;
    glcdCtrl.canvas.posX = 0;
  }

  //カーソル表示
  if (micros() - cursorTime > 500000 || cursorTime > micros()) {
    if (cursorSwitch) {
      //カーソルを表示する
      glcdCtrl.canvas.color = 1;
      glcdCtrl.canvas.Pos((printPos % COLUMN) * 6, (printPos / COLUMN) * 8);
      glcdCtrl.canvas.PutChar('_');
      cursorPosPrev = printPos;
    }
    else {
      //カーソルを消す
      glcdCtrl.canvas.color = 0;
      glcdCtrl.canvas.Pos((cursorPosPrev % COLUMN) * 6, (cursorPosPrev / COLUMN) * 8);
      glcdCtrl.canvas.PutChar('_');

      //カーソル位置にある文字を表示; カーソルを消すとき文字まで消してしまうのを防ぐため
      glcdCtrl.canvas.posX -= 6;
      glcdCtrl.canvas.color = 1;
      glcdCtrl.canvas.PutChar(message[cursorPosPrev / COLUMN][cursorPosPrev % COLUMN]);
    }

    cursorSwitch ^= 1;
    cursorTime = micros();
  }

  //グラフィック液晶に描画
  glcdCtrl.Draw();
}

//文字を仮想画面に描画
void PutMessage(char c) {

  if (c == '\n') {
    printPos = ((printPos + COLUMN) / COLUMN) * COLUMN;
  }
  else if (c == '\r') {
    printPos = (printPos / COLUMN) * COLUMN;
  }
  else {
    int row = printPos / COLUMN;
    int col = printPos % COLUMN ;
    message[row][col] = c;
    printPos++;
  }

  //表示範囲を超えたとき
  if (printPos >= ROW * COLUMN) {
    printPos = COLUMN * (ROW - 1);
    ScrollMessage(1);
  }

}

//メッセージをスクロールします
void ScrollMessage(int numberOfSteps) {
  //スクロール分ずらす
  for (int i = numberOfSteps; i < ROW; i ++) {
    for (int j = 0; j < COLUMN; j++) {
      message[i - numberOfSteps][j] = message[i][j];
    }
  }

  //ずれた分の行にスペースを埋める
  for (int i = ROW - numberOfSteps; i < ROW; i++) {
    for (int j = 0; j < COLUMN; j++) {
      message[i][j] = ' ';
    }
  }
}
