/*
*アプリ: MediaPlayer
*  最終更新日: 4.24.2016
*
*  説明:
*    概要:
*      SDカードにある音楽ファイルを再生します.
*      曲の登録は「MusicList.h」に記入する必要があります.
*    操作方法:
*      UPボタン, DOWNボタン: モード切替
*        モード: Normal: 通常再生; Repeat: 繰り返し再生; Continuous: 連続再生
*      RIGHTボタン, LEFTボタン: 曲選択
*      Aボタン: 再生
*      Bボタン: 停止
*
*  更新履歴:
*    4.24.2016:
*      プログラムの完成
*
*    4.25.2016:
*      スクリプト修正
*/



#include <avr/pgmspace.h>
#include "MusicList.h"

#include "TerminalPro4CommonSetting.h"


//文字列の長さ
#define STR_LENGTH 20

//文字列
char str[STR_LENGTH];

//曲番号
char index;
char indexDisplay;

//mode
char mode;
char modeDisplay;

//合計の曲数
char totalMusic;

typedef enum {
  STOP,
  PLAYING
} STATE;

STATE state;
STATE statePrev;

bool updateDisplay;

void UpdateDisplay();
void ReadProgmem(const uint8_t *strProgmem, char *strReturn, byte length);
void ClearScr(void);

void setup() {
  SetupTerminalPro4();
  
  //変数設定
  totalMusic = sizeof(musicList) / sizeof(musicList[0]);
  index = 0;
  indexDisplay = 0;
  mode = 0;
  modeDisplay = 0;

  state = STOP;
  statePrev = STOP;

  updateDisplay = true;
}



bool InputAnyKey() {
  byte cR, cL;
  ctrlRight.Stick(&cR, A_BUTTON + B_BUTTON + X_BUTTON + Y_BUTTON);
  ctrlLeft.Stick(&cL, UP_BUTTON + DOWN_BUTTON + RIGHT_BUTTON + LEFT_BUTTON);
  if (cR | cL) {
    return true;
  }

  return false;
}


void loop() {
  //連続再生モードのとき
  if (mode == 2)
  {
    if (state == STOP)
    {
      //曲番号を+1
      index++;

      //indexの値を0~合計の曲数に設定
      if (index < 0) index = totalMusic - 1;
      if (index > totalMusic - 1) index = 0;

      //画面用の変数を更新
      indexDisplay = index;
      modeDisplay = mode;

      //音楽ファイル名を取得
      ReadProgmem(musicList[index][1], str, STR_LENGTH);

      mediaCtrl.Load(str, 0);
      mediaCtrl.Play();

      delay(1000);  //音マイコンの状態送信待機

      updateDisplay = true;
    }
  }
  
  byte cR, cL;
  ctrlRight.Stick(&cR, A_BUTTON + B_BUTTON + X_BUTTON + Y_BUTTON);
  ctrlLeft.Stick(&cL, UP_BUTTON + DOWN_BUTTON);
  
  //UPbuttonが押されたとき
  if (cL & UP_BUTTON)
  {
    modeDisplay++;
    updateDisplay = true;
  }

  //DOWNbuttonが押されたとき
  if (cL & DOWN_BUTTON)
  {
    modeDisplay--;
    updateDisplay = true;
  }

  //modeの値を0~2に設定
  if (modeDisplay < 0) modeDisplay = 2;
  if (modeDisplay > 2) modeDisplay = 0;

  //RIGHTbuttonが押されたとき
  if (cL & RIGHT_BUTTON)
  {
    indexDisplay++;
    updateDisplay = true;
  }

  //LEFTbuttonが押されたとき
  if (cL & LEFT_BUTTON)
  {
    indexDisplay--;
    updateDisplay = true;
  }

  //indexの値を0~合計の曲数に設定
  if (indexDisplay < 0) indexDisplay = totalMusic - 1;
  if (indexDisplay > totalMusic - 1) indexDisplay = 0;



  //Abuttonが押されたとき
  if (cR & A_BUTTON)
  {
    
    index = indexDisplay;
    mode = modeDisplay;

    //音楽ファイル名を取得
    ReadProgmem(musicList[index][1], str, STR_LENGTH);
    

    //音楽ファイル読み込み
    switch (mode) {
      default:
      case 0:
        mediaCtrl.Load(str, MediaPlayerController::PLAY_MODE::NORMAL);
        break;

      case 1:
        mediaCtrl.Load(str,  MediaPlayerController::PLAY_MODE::LOOP);
        break;

      case 2:
        mediaCtrl.Load(str,  MediaPlayerController::PLAY_MODE::NORMAL);
        break;
    }
    mediaCtrl.Play();
  }


  //Bbuttonが押されたとき
  if (cR & B_BUTTON)
  {
    //再生モードを標準に戻す; ContinuousModeで連続再生されないようにするため
    mode = 0;
    //音楽停止
    mediaCtrl.Stop();
  }


  //再生状態取得
  if (mediaCtrl.IsPlaying())
  {
    statePrev = state;
    state = PLAYING;
  }
  else
  {
    statePrev = state;
    state = STOP;
  }

  //値が変化したときのみ実行
  if (statePrev != state) {
    updateDisplay = true;
  }


  if (updateDisplay) {
    UpdateDisplay();
  }

  delay(10);
}

void UpdateDisplay() {
  //画面の消去
  ClearScr();

  //色を黒に指定
  glcdCtrl.canvas.color = true;

  //文字列設定
  str[0] = 'M';
  str[1] = 'e';
  str[2] = 'd';
  str[3] = 'i';
  str[4] = 'a';
  str[5] = 'P';
  str[6] = 'l';
  str[7] = 'a';
  str[8] = 'y';
  str[9] = 'e';
  str[10] = 'r';
  str[11] = '\0';

  //Scrに文字を書く
  glcdCtrl.canvas.Pos(0, 0);
  glcdCtrl.canvas.Mes(str);

  glcdCtrl.canvas.Pos(0, glcdCtrl.canvas.posY + 20);
  ReadProgmem(musicList[indexDisplay][0], str, STR_LENGTH);
  glcdCtrl.canvas.Mes(str);

  //再生中のとき
  switch (state) {
    case PLAYING:
      str[0] = 'P';
      str[1] = 'l';
      str[2] = 'a';
      str[3] = 'y';
      str[4] = 'i';
      str[5] = 'n';
      str[6] = 'g';
      str[7] = '\0';
      break;

    case STOP:

      str[0] = 'S';
      str[1] = 't';
      str[2] = 'o';
      str[3] = 'p';
      str[4] = '\0';
      break;
  }
  glcdCtrl.canvas.Pos(70, glcdCtrl.canvas.posY + 10);
  glcdCtrl.canvas.Mes(str);

  //mode表示
  switch (modeDisplay)
  {
    case 0:
      str[0] = 'N';
      str[1] = 'o';
      str[2] = 'r';
      str[3] = 'm';
      str[4] = 'a';
      str[5] = 'l';
      str[6] = '\0';
      break;

    case 1:
      str[0] = 'R';
      str[1] = 'e';
      str[2] = 'p';
      str[3] = 'e';
      str[4] = 'a';
      str[5] = 't';
      str[6] = '\0';
      break;

    case 2:
      str[0] = 'C';
      str[1] = 'o';
      str[2] = 'n';
      str[3] = 't';
      str[4] = 'i';
      str[5] = 'n';
      str[6] = 'u';
      str[7] = 'o';
      str[8] = 'u';
      str[9] = 's';
      str[10] = '\0';
      break;
  }
  glcdCtrl.canvas.Pos(60, glcdCtrl.canvas.posY + 25);
  glcdCtrl.canvas.Mes(str);

  //GLCD画面に描画
  glcdCtrl.Draw();

  updateDisplay = false;
}

//PROGMEMから文字列取得する
void ReadProgmem(const uint8_t *strProgmem, char *strReturn, byte length)
{
  
  byte i = 0;

  for (i = 0; i < length - 1; i++)
  {
    strReturn[i] = pgm_read_byte_near(&strProgmem[i]);
  }
  
  strReturn[i] = '\0';
  
}

//画面を消去する
void ClearScr(void)
{
  glcdCtrl.canvas.color = false;
  glcdCtrl.canvas.Boxf(0, 0, glcdCtrl.canvas.SizeX(), glcdCtrl.canvas.SizeY());
  glcdCtrl.Draw();
}

