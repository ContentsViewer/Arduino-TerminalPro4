/*
*アプリ: #Bullet
*  最終更新日: 4.27.2016
*
*  説明:
*    概要:
*      弾幕?ゲーム
*
*    操作方法:
*      UP, DOWN, RIGHT, LEFTボタン: 移動
*      Aボタン: 弾発射
*      Bボタン: スローモード
*
*      UP + DOWN + B + A:無敵
*      RIGHT + LEFT + B + A:隠しステージ
*
*      ステージ間レベルアップA + B で抜ける
*
*  更新履歴:
*    12.4.2016:
*      プログラムの完成
*
*    4.27.2016:
*      TerminalPro2用にスクリプト修正
*/

#include "TerminalPro4CommonSetting.h"

#define TYPEebulletMax 20  //敵弾のバリエーション上限

#define ebulletmax 15  //敵弾数上限
#define pbulletmax 15  //自機弾数上限

#define F0(Y, T, X0) Y = X0;
#define F1(Y, T, X0) Y = X0 - 3 * (T);
#define F2(Y, T, X0) Y = X0 + (T);
#define F3(Y, T, X0) Y = X0 - 4 * (T);
#define F4(Y, T, X0) Y = X0 -( 4 * (T) + 7 * (1.0 + (double)sin(T)));
#define F5(Y, T, X0) Y = X0 -( (T) + (T) * (1.0 + (double)sin(T * 0.25)));
#define F6(Y, T, X0) Y = X0 -(5 * (double)sin((T) * 0.5));
#define F7(Y, T, X0) Y = X0 + 0.3 * (double)(T);
#define F8(Y, T, X0) Y = X0 - 0.3 * (double)(T);
#define F9(Y, T, X0) Y = X0 + 10 * (T);
#define F10(Y, T, X0) Y = X0 - pow(1.2, (T));
#define F11(Y, T, X0) Y = X0 + pow(1.2, (T));
#define F12(Y, T, X0){\
    a = -((double)(py - hby)/(double)(px - hbx));\
    Y = X0 + a * (T);\
    if(Y - X0 > 1) Y = X0 + 1;\
    if(Y - X0 < -1) Y = X0 - 1;}
#define F13(Y, T, X0) Y = X0 - 2 * (T);
#define F14(Y, T, X0) Y = X0 - (5 * (double)sin(0.25 * T));
#define F15(Y, T, X0) Y = X0 + (10 * (double)sin(0.25 * T));

//関数プロタイプ宣言
extern void init_bullet(void);
extern void gamemain_img();
extern void clearScrB(void);
extern void clearScr(void);
extern void traLvStr(void);
extern void game_reset(void);
extern void gamelvup(void);

int px, py, ex, ey;  //自機, 敵機位置
byte csx, csy;  //キャラサイズ
byte hisposX, hisposY;
char esy;  //敵キャラスピード
char ehp;  //敵耐久力 0~120
int ehpmax;
//敵耐久力表示バー
char ehpbarmax;
char ehpbarX;
char ehpbarY;

char ebullet[ebulletmax][7]; //0: x方向関数; 1: y方向関数; 2: t; 3: 発射位置x; 4: 発射位置y; 5:過去データx; 6: 過去データy
char pbullet[pbulletmax][7];
byte n;
char bx, by, hbx, hby;  //弾丸の位置
byte bsx, bsy;  //弾丸サイズ
byte GameFlg;  //0: プレイ中; 1: GAMEOVER; 2: complete

//あたり判定調整変数　弾丸
byte tby;
byte tbx;

//あたり判定調整変数 キャラ
byte tcx;
byte tcy;

char lv;

byte ebsp;  //敵弾スピード

byte ebd;  //敵弾発射間隔
byte ebd_c;

byte pbd;  //自機弾発射間隔
byte pbd_c;  //

byte typebulletMax;
byte typebulletMin;

char lvs[2];
char str[25];

float a;  //弾丸軌道　傾き

byte Secret;

byte c1, c2;
void setup() {
  SetupTerminalPro4();
  
  
  randomSeed(analogRead(5));

  //キャラサイズ
  csx = 13;
  csy = 15;

  //弾サイズ
  bsx = 8;
  bsy = 8;

  //あたり判定調整変数 弾丸
  tby = 3;
  tbx = 3;

  //あたり判定調整変数 キャラ
  tcx = 1;
  tcy = 1;

  esy = 1;    //敵移動速度(y方向)
  ebsp = 1;  //敵弾スピード
  ebd = 10;  //敵弾発射間隔
  ehp = 1;  //敵耐久力
  ehpmax = 1;
  
  //敵耐久力表示バー
  ehpbarmax = 12;
  ehpbarX = 0;
  ehpbarY = 0;

  typebulletMax = 0;
  typebulletMin = 0;

  pbd = 1;  //自機弾発射間隔

  //loop に入る前初期化
  //lv = 0; GameFlg = 1;
  lv = 0;
  GameFlg = 1;
  Secret = 0;  //0b01: 隠しステージ; 0b10: 無敵

}

void loop() {
if (lv >= 9 && GameFlg == 2)  //LV 9以上ですべてのゲームクリア
  {
    clearScrB();

    glcdCtrl.canvas.color = false;
    glcdCtrl.canvas.Pos(22, 28);
    str[0] = 'G';
    str[1] = 'a';
    str[2] = 'm';
    str[3] = 'e';
    str[4] = ' ';
    str[5] = 'C';
    str[6] = 'o';
    str[7] = 'm';
    str[8] = 'p';
    str[9] = 'l';
    str[10] = 'e';
    str[11] = 't';
    str[12] = 'e';
    str[13] = '!';
    str[14] = '\0';
    glcdCtrl.canvas.Mes(str);
    glcdCtrl.Draw();
    delay(2000);

    clearScrB();
    glcdCtrl.canvas.color = false;
    glcdCtrl.canvas.Pos(0, 28);
    str[0] = 'T';
    str[1] = 'h';
    str[2] = 'a';
    str[3] = 'n';
    str[4] = 'k';
    str[5] = ' ';
    str[6] = 'y';
    str[7] = 'o';
    str[8] = 'u';
    str[9] = ' ';
    str[10] = 'f';
    str[11] = 'o';
    str[12] = 'r';
    str[13] = ' ';
    str[14] = 'p';
    str[15] = 'l';
    str[16] = 'a';
    str[17] = 'y';
    str[18] = 'i';
    str[19] = 'n';
    str[20] = 'g';
    str[21] = '\0';
    glcdCtrl.canvas.Mes(str);
    if (lv == 10)
    {
      glcdCtrl.canvas.color = false;
      
      gamemain_img();
      
    }
    glcdCtrl.Draw();
    delay(5000);
    
    if (lv == 10)
    {
      clearScr();
      glcdCtrl.canvas.color = true;
      glcdCtrl.canvas.Pos(50, 10);
      glcdCtrl.canvas.Celput(remilia_img);
      glcdCtrl.Draw();
      
      delay(5000);
      clearScrB();
      
    }

    game_reset();
    Secret |= 0x01;
  }
  //stageセレクト***************************************************************************/
  if (GameFlg == 1)
  {
    clearScr();

    game_reset();
    for (;;)
    {
      ctrlRight.Stick(&c1, A_BUTTON);  //ボタン情報入力
      ctrlLeft.Stick(&c2, 0);
      
      
      if (c2 & UP_BUTTON)
      {
        lv++;
        if (lv > 9)
        {
          if (Secret & 0x01) lv = 10;
          else lv = 9;
        }
        clearScr();
      }
      if (c2 & DOWN_BUTTON)
      {
        lv--;
        if (lv < 0) lv = 0;
        clearScr();
      }
      
      //隠しコマンド 隠しステージ
      if (c1 & B_BUTTON && c2 & RIGHT_BUTTON && c2 & LEFT_BUTTON && c1 & A_BUTTON)
      {
        clearScrB();
        clearScr();
        if(Secret & 0x01) Secret = ~((~Secret)| 0x01);
        else Secret |= 0x01;
        continue;
      }
      
      //隠しコマンド 無敵化
      if (c1 & B_BUTTON && c2 & UP_BUTTON && c2 & DOWN_BUTTON && c1 & A_BUTTON)
      {
        clearScrB();
        clearScr();
        if(Secret & 0x02) Secret = ~((~Secret)| 0x02);
        else Secret |= 0x02;
        continue;
      }
      
      //ステージ決定
      if (c1 & A_BUTTON)
      {
        for (n = 0; n < lv; n++)
        {
          gamelvup();
        }
        if (lv == 10) //Secret stage 設定*******************************************//
        {
          esy = 3;
          ehpmax = 120;
          typebulletMin = 3;
          typebulletMax = TYPEebulletMax;
          ebd = 5;
        }
        break;
      }
      
      //画面表示
      glcdCtrl.canvas.color = true;
      glcdCtrl.canvas.Pos(0, 0);
      str[0] = '#';
      str[1] = 'B';
      str[2] = 'u';
      str[3] = 'l';
      str[4] = 'l';
      str[5] = 'e';
      str[6] = 't';
      str[7] = '\0';
      glcdCtrl.canvas.Mes(str);

      glcdCtrl.canvas.Pos(10, 20);
      str[0] = 'L';
      str[1] = 'e';
      str[2] = 'v';
      str[3] = 'e';
      str[4] = 'l';
      str[5] = ' ';
      str[6] = 'S';
      str[7] = 'e';
      str[8] = 'l';
      str[9] = 'e';
      str[10] = 'c';
      str[11] = 't';
      str[12] = '\0';
      glcdCtrl.canvas.Mes(str);

      glcdCtrl.canvas.color = true;
      glcdCtrl.canvas.Pos(30, 30);
      traLvStr();  //Lv数値を数字に, 文字をScrに描画

      glcdCtrl.canvas.color = true;
      gamemain_img();
      
      if(Secret & 0x02)
      {
        glcdCtrl.canvas.color = true;
        glcdCtrl.canvas.Pos(90,5);
        glcdCtrl.canvas.Celput(remilia_img);
      }
      glcdCtrl.Draw();
    }
  }

  //Game前処理************************************************************
  if (GameFlg == 2)
  {
    lv++;
    gamelvup();  //難易度アップ lvアップしてから呼び出すこと
  }

  //Gameplay 前初期化***********************ここから
  init_bullet();

  px = 0;
  py = 0;

  ex = glcdCtrl.canvas.SizeX() - csx;
  ey = random(0, glcdCtrl.canvas.SizeY() - csy);
  ehp = ehpmax;
  
  //弾丸発射間隔用カウント変数
  pbd_c = 0;
  ebd_c = 0;

  GameFlg = 0;
  //***************************************ここまで

  clearScr();

  //Lv画面表示
  glcdCtrl.canvas.color = true;
  glcdCtrl.canvas.Pos(30, 28);
  traLvStr();  //Lv数値を数字に
  glcdCtrl.Draw();
  delay(1500);
  
  clearScr();
  
  switch(lv % 2)
  {
    case 0:
      //曲の読み込み
      str[0] = 'N';
      str[1] = '_';
      str[2] = 'O';
      str[3] = '_';
      str[4] = 'N';
      str[5] = '_';
      str[6] = '1';
      str[7] = '.';
      str[8] = 'w';
      str[9] = 'a';
      str[10] = 'v';
      str[11] = '\0';
      break;
  
    case 1:
      str[0] = 'B';
      str[1] = '_';
      str[2] = 'A';
      str[3] = '_';
      str[4] = '3';
      str[5] = '.';
      str[6] = 'w';
      str[7] = 'a';
      str[8] = 'v';
      str[9] = '\0';
      break;
  }
  mediaCtrl.Load(str, 1);
  
  mediaCtrl.Play();
  //ここまで***************************************************
  
  /***************************************************************************************************************************************
  *ここから
  *  game main
  *
  *
  *
  *****************************************************************************************************************************************/
  for (;;)
  {
    
    /*boss******************************************/
    glcdCtrl.canvas.color = false;
    glcdCtrl.canvas.Pos(ex, ey);
    glcdCtrl.canvas.Celput(boss);

    ex = glcdCtrl.canvas.SizeX() - csx;
    ey += esy;

    if (ey < 0) esy *= -1;;
    if (ey > glcdCtrl.canvas.SizeY() - csy) esy *= -1;

    glcdCtrl.canvas.color = true;
    glcdCtrl.canvas.Pos(ex, ey);
    glcdCtrl.canvas.Celput(boss);

    glcdCtrl.Draw();
    /***********************************************/
    /*boss耐久表示************************************/
    
    glcdCtrl.canvas.color = false;
    glcdCtrl.canvas.Boxf(ehpbarX, ehpbarY, ehpbarX + ehpbarmax, ehpbarY + 1);
    
    //前のバーを消してから値を更新
    ehpbarX = ex - 1;
    ehpbarY = ey - 5;
    
    //バー描画
    glcdCtrl.canvas.color = true;
    glcdCtrl.canvas.Boxf(ehpbarX, ehpbarY, ehpbarX + ehpbarmax * ((double)ehp / (double)ehpmax), ehpbarY + 1);
    glcdCtrl.Draw();
    /****************************************************/
    /*弾丸*******************************************/
    //弾発射開始
    if (ebd_c == 0)
    {
      for (n = 0; n < ebulletmax; n++)
      {
        if (ebullet[n][0] == 127)
        {
          //弾のバリエーション設定////////////////////////////////////////////////////////////////////////////
          switch (random(typebulletMin, typebulletMax))
          {
            case 0:  //直線
              ebullet[n][0] = 1;
              ebullet[n][1] = 0;
              break;
            case 1:  //直線　斜め上
              ebullet[n][0] = 1;
              ebullet[n][1] = 7;
              break;
            case 2:  //直線　斜め下
              ebullet[n][0] = 1;
              ebullet[n][1] = 8;
              break;
            case 3:  //直線 速い　斜め上
              ebullet[n][0] = 3;
              ebullet[n][1] = 7;
              break;
            case 4:  //直線 速い　斜め下
              ebullet[n][0] = 3;
              ebullet[n][1] = 8;
              break;
            case 5:  //直線　速度上がる
              ebullet[n][0] = 3;
              ebullet[n][1] = 0;
              break;
            case 6:  //直線　速い　y指数で減る
              ebullet[n][0] = 3;
              ebullet[n][1] = 10;
              break;
            case 7:  //直線　速い　y指数で増える
              ebullet[n][0] = 3;
              ebullet[n][1] = 11;
              break;
            case 8:  //弾速速い　向かってくる
              ebullet[n][0] = 3;
              ebullet[n][1] = 12;
              break;
            case 9:  //縦に振動
              ebullet[n][0] = 13;
              ebullet[n][1] = 6;
              break;
            case 10:  //縦に振動　直線速い
              ebullet[n][0] = 1;
              ebullet[n][1] = 6;
              break;
            case 11:  //縦に振動　ゆっくり振幅　速度速い
              ebullet[n][0] = 3;
              ebullet[n][1] = 14;
              break;
            case 12:  //縦に振動　ゆっくり振幅　振幅増大　速度速い
              ebullet[n][0] = 3;
              ebullet[n][1] = 15;
              break;

            case 13:  //弾速速い　向かってくる
              ebullet[n][0] = 3;
              ebullet[n][1] = 12;
              break;
            case 14:  //直線　振幅が大きくなる
              ebullet[n][0] = 5;
              ebullet[n][1] = 0;
              break;
            case 15:  //直線　速い　y指数で減る
              ebullet[n][0] = 3;
              ebullet[n][1] = 10;
              break;
            case 16:  //直線　速い　y指数で増える
              ebullet[n][0] = 3;
              ebullet[n][1] = 11;
              break;
            case 17:  //直線　振幅が大きくなる 向かってくる
              ebullet[n][0] = 5;
              ebullet[n][1] = 12;
              break;
            case 18:  //　向かってくる
              ebullet[n][0] = 3;
              ebullet[n][1] = 12;
              break;
            default:  //直線 変則的　横振動
              ebullet[n][0] = 4;
              ebullet[n][1] = 0;
              break;
          }
          //////////////////////////////////////////////////////////////////////////////////////////////////////
          ebullet[n][2] = 0;
          ebullet[n][3] = ex;
          ebullet[n][4] = ey + csy / 2 - 4;
          ebullet[n][5] = ex;
          ebullet[n][6] = ey + csy / 2 - 4;

          break;
        }
      }
    }
    //弾丸描画
    for (n = 0; n < ebulletmax; n++)
    {
      //x方向速度設定/////////////////////////////////////////////////////////
      hbx = ebullet[n][5];

      if (ebullet[n][0] != 127)
      {
        if (ebullet[n][0] == 0)
        {

          F0(bx, ebullet[n][2], ebullet[n][3]);
        }
        if (ebullet[n][0] == 1)
        {

          F1(bx, ebullet[n][2], ebullet[n][3]);
        }
        if (ebullet[n][0] == 3)
        {

          F3(bx, ebullet[n][2], ebullet[n][3]);
        }
        if (ebullet[n][0] == 4)
        {

          F4(bx, ebullet[n][2], ebullet[n][3]);
        }
        if (ebullet[n][0] == 5)
        {

          F5(bx, ebullet[n][2], ebullet[n][3]);
        }
        if (ebullet[n][0] == 13)
        {

          F13(bx, ebullet[n][2], ebullet[n][3]);
        }

        ebullet[n][5] = bx;  //現在のデータ記憶しておく
        //ｙ方向速度設定/////////////////////////////////////////////////////////
        hby = ebullet[n][6];

        if (ebullet[n][1] == 0)
        {
          F0(by, ebullet[n][2], ebullet[n][4]);
        }

        if (ebullet[n][1] == 1)
        {
          F1(by, ebullet[n][2], ebullet[n][4]);
        }

        if (ebullet[n][1] == 6)
        {
          F6(by, ebullet[n][2], ebullet[n][4]);
        }

        if (ebullet[n][1] == 7)
        {
          F7(by, ebullet[n][2], ebullet[n][4]);
        }

        if (ebullet[n][1] == 8)
        {
          F8(by, ebullet[n][2], ebullet[n][4]);
        }

        if (ebullet[n][1] == 10)
        {
          F10(by, ebullet[n][2], ebullet[n][4]);
        }

        if (ebullet[n][1] == 11)
        {
          F11(by, ebullet[n][2], ebullet[n][4]);
        }

        if (ebullet[n][1] == 12)
        {
          F12(by, ebullet[n][2], ebullet[n][6]); //ここではX0に弾丸のY座標を代入
        }

        if (ebullet[n][1] == 14)
        {
          F14(by, ebullet[n][2], ebullet[n][4]);
        }

        if (ebullet[n][1] == 15)
        {
          F15(by, ebullet[n][2], ebullet[n][4]);
        }

        ebullet[n][6] = by;  //現在のデータを記憶しておく

        //前の画像を消す
        glcdCtrl.canvas.color = false;
        glcdCtrl.canvas.Pos(hbx, hby);
        glcdCtrl.canvas.Celput(ebullet_img);

        //画面外に出たら弾丸を消す
        if (bx + bsx < 0 || bx > glcdCtrl.canvas.SizeX() - 1 || by + bsy <  0 || by > glcdCtrl.canvas.SizeY() - 1)
        {
          ebullet[n][0] = 127;
          continue;
        }

        if (ebullet[n][2] > 100)
        {
          ebullet[n][0] = 127;
          continue;
        }
        ebullet[n][2] += ebsp;

        //弾丸描画
        glcdCtrl.canvas.color = true;
        glcdCtrl.canvas.Pos(bx, by);
        glcdCtrl.canvas.Celput(ebullet_img);
        glcdCtrl.Draw();

        //敵弾あたり判定
        if (!(Secret & 0x02))  //裏コマンド 敵弾あたり判定無効
        {
        if ((bx + tbx > px + tcx &&  bx  + tbx < px + csx - tcx && by + tby > py + tcy && by + tby < py + csy - tcy) |
            (bx + bsx - tbx > px + tcx && bx + bsx - tbx < px + csx - tcx && by + tby > py + tcy && by + tby < py + csy - tcy) |
            (bx + tbx > px + tcx && bx + tbx < px + csx - tcx && by + bsy - tby > py + tcy && by + bsy - tby < py + csy - tcy) |
            (bx + bsx > px + tcx && bx + bsx < px + csx - tcx && by + bsy - tby > py + tcy && by + bsy - tby < py + csy - tcy))
        {
          //弾は残しておく

          GameFlg = 1;
          continue;
        }
        }
      }
    }

    /*player*****************************************/
    glcdCtrl.canvas.color = false;
    glcdCtrl.canvas.Pos(px, py);
    glcdCtrl.canvas.Celput(player);
    

    ctrlRight.Stick(&c1, 0);  //ボタン情報を変数bに代入,
    ctrlLeft.Stick(&c2, 0);

    if (c1 & B_BUTTON) //Bbutton が押されると移動スピードおちる
    {
      if (c2 & UP_BUTTON) py -= 2;  //UPbuttonが押されたとき
      if (c2 & DOWN_BUTTON) py += 2; //DOWNbuttonが押されたとき
      if (c2 & RIGHT_BUTTON) px += 2; //RIGHTbuttonが押されたとき
      if (c2 & LEFT_BUTTON) px -= 2; //LEFTbuttonが押されたとき
    }
    else  //Bbuttonが押されないなら通常
    {
      if (c2 & UP_BUTTON) py -= 4;  //UPbuttonが押されたとき
      if (c2 & DOWN_BUTTON) py += 4; //DOWNbuttonが押されたとき
      if (c2 & RIGHT_BUTTON) px += 4; //RIGHTbuttonが押されたとき
      if (c2 & LEFT_BUTTON) px -= 4; //LEFTbuttonが押されたとき
    }
    if (c1 & A_BUTTON) //Abuttonが押されたとき
    {
      if (pbd_c == 0)
      {
        for (n = 0; n < pbulletmax; n++)
        {
          if (pbullet[n][0] == 127)
          {
            pbullet[n][0] = 9;
            pbullet[n][1] = 0;
            pbullet[n][2] = 0;
            pbullet[n][3] = px + csx;
            pbullet[n][4] = py + csy / 2 - 4;
            pbullet[n][5] = px + csx;
            pbullet[n][6] = py + csy / 2 - 4;

            break;
          }
        }
      }
    }

    //画面外に出ないようにする
    if (px < 0) px = 0;
    if (px > glcdCtrl.canvas.SizeX() - csx) px = glcdCtrl.canvas.SizeX() - csx;
    if (py < 0) py = 0;
    if (py > glcdCtrl.canvas.SizeY() - csy) py = glcdCtrl.canvas.SizeY() - csy;
    glcdCtrl.canvas.color = true;
    glcdCtrl.canvas.Pos(px, py);
    glcdCtrl.canvas.Celput(player);
    glcdCtrl.Draw();
    /*****************************************************/
    /*player 弾丸******************************************/
    //弾丸描画
    for (n = 0; n < pbulletmax; n++)
    {
      if (pbullet[n][0] != 127)
      {
        //x方向速度設定/////////////////////////////////////////////////////////
        hbx = pbullet[n][5];

        if (pbullet[n][0] == 0)
        {
          F0(bx, ebullet[n][2], pbullet[n][3]);
        }

        if (pbullet[n][0] == 2)
        {
          F2(bx, pbullet[n][2], pbullet[n][3]);
        }

        if (pbullet[n][0] == 9)
        {
          F9(bx, pbullet[n][2], pbullet[n][3]);
        }

        pbullet[n][5] = bx;
        //y方向速度設定/////////////////////////////////////////////////////////
        hby = pbullet[n][6];

        if (pbullet[n][1] == 0)
        {
          F0(by, pbullet[n][2], pbullet[n][4]);
        }
        pbullet[n][6] = by;

        //前の画像を消す
        glcdCtrl.canvas.color = false;
        glcdCtrl.canvas.Pos(hbx, hby);
        glcdCtrl.canvas.Celput(pbullet_img);

        //画面外に出たら弾丸を消す
        if (bx + bsx < 0 || bx > glcdCtrl.canvas.SizeX() - 1 || by + bsy <  0 || by > glcdCtrl.canvas.SizeY() - 1)
        {
          pbullet[n][0] = 127;
          continue;
        }

        pbullet[n][2]++;  //弾発射からの経過時間を更新

        glcdCtrl.canvas.color = true;
        glcdCtrl.canvas.Pos(bx, by);
        glcdCtrl.canvas.Celput(pbullet_img);
        glcdCtrl.Draw();


        //自機弾あたり判定
        if ((bx + tbx > ex + tcx &&  bx + tbx < ex + csx - tcx && by + tby > ey + tcy && by + tby < ey + csy - tcy) |
            (bx + bsx - tbx > ex + tcx && bx + bsx - tbx < ex + csx - tcx && by + tby > ey + tcy && by + tby < ey + csy - tcy) |
            (bx + tbx > ex + tcx && bx + tbx < ex + csx - tcx && by + bsy - tby > ey + tcy && by + bsy - tby < ey + csy - tcy) |
            (bx + bsx - tbx > ex + tcx && bx + bsx - tbx < ex + csx - tcx && by + bsy - tby > ey + tcy && by + bsy - tby < ey + csy - tcx))
        {
          ehp--;  //敵のhpをマイナス-1
          if (ehp <= 0) GameFlg = 2;

          glcdCtrl.canvas.color = false;
          glcdCtrl.canvas.Pos(bx, by);
          glcdCtrl.canvas.Celput(pbullet_img);

          pbullet[n][0] = 127;  //敵に弾が当たればその弾を消す
          continue;
        }
      }
    }

    if (GameFlg != 0) break;

    delay(1);
    ebd_c++;
    if (ebd_c > ebd) ebd_c = 0;

    pbd_c++;
    if (pbd_c > pbd) pbd_c = 0;

  }
  //画面消去, result表示


  if (GameFlg == 1)
  {
    
    clearScrB();
    clearScr();

    glcdCtrl.canvas.color = true;
    glcdCtrl.canvas.Pos(0, 0);
    str[0] = 'G';
    str[1] = 'a';
    str[2] = 'm';
    str[3] = 'e';
    str[4] = ' ';
    str[5] = 'O';
    str[6] = 'v';
    str[7] = 'e';
    str[8] = 'r';
    str[9] = '.';
    str[10] = '.';
    str[11] = '.';
    str[12] = '\0';
    glcdCtrl.canvas.Mes(str);
  }
  if (GameFlg == 2)
  {
    
    clearScr();

    glcdCtrl.canvas.color = true;
    glcdCtrl.canvas.Pos(0, 0);
    str[0] = 'G';
    str[1] = 'a';
    str[2] = 'm';
    str[3] = 'e';
    str[4] = ' ';
    str[5] = 'C';
    str[6] = 'l';
    str[7] = 'e';
    str[8] = 'a';
    str[9] = 'r';
    str[10] = '!';
    str[11] = '!';
    str[12] = '\0';
    glcdCtrl.canvas.Mes(str);
    
    glcdCtrl.canvas.Pos(5, 15);
    str[0] = 'N';
    str[1] = 'e';
    str[2] = 'x';
    str[3] = 't';
    str[4] = ' ';
    str[5] = 'L';
    str[6] = 'e';
    str[7] = 'v';
    str[8] = 'e';
    str[9] = 'l';
    str[10] = NULL;
    glcdCtrl.canvas.Mes(str);
  }
  mediaCtrl.Stop();
  
  glcdCtrl.Draw();
  delay(3000);
  
  ctrlRight.Stick(&c1, 0);
  if((c1 & A_BUTTON) && (c1 & B_BUTTON)) GameFlg = 1;  //  強制的にstage Select 画面へ
  
}
void gamelvup(void)  //lv upは手動で
{
  //敵の移動速度倍に
  if (esy < 0) esy *= -1;
  esy *= 2;
  if (esy > 3) esy = 3;

  //敵弾発射間隔短く

  //敵弾バリエーション豊かに
  switch (lv)
  {
    case 0:
      ebd = 20;
      typebulletMin = 0;
      typebulletMax = 1;
      break;
    case 1:
      ebd = 10;
      typebulletMin = 0;
      typebulletMax = 3;
      break;
    case 2:
      ebd = 5;
      typebulletMin = 3;
      typebulletMax = 6;
      break;
    case 3:
      ebd = 5;
      typebulletMin = 5;
      typebulletMax = 8;
      break;
    case 4:
      ebd = 6;
      typebulletMin = 6;
      typebulletMax = 9;
      break;
    case 5:
      ebd = 10;
      typebulletMin = 9;
      typebulletMax = 11;
      break;
    case 6:
      ebd = 5;
      typebulletMin = 11;
      typebulletMax = 13;
      break;
    case 7:
      ebd = 5;
      typebulletMin = 13;
      typebulletMax = 14;
      break;
    case 8:
      ebd = 5;
      typebulletMin = 14;
      typebulletMax = 17;
      break;
    case 9:
      ebd = 5;
      typebulletMin = 17;
      typebulletMax = 19;
      break;

  }

  //敵耐久力アップ
  ehpmax *= 2;  //2倍に
  if (ehpmax > 32) ehpmax = 32;
}
void game_reset(void)
{
  /*
   ebsp = 1;
   */
  esy = 1;
  ebd = 10;

  typebulletMax = 0;
  typebulletMin = 0;

  ehpmax = 1;

  GameFlg = 1;
}

void traLvStr(void)
{
  lvs[0] = 0x30 + lv;
  lvs[1] = '\0';
  glcdCtrl.canvas.Mes("Lv.");
  glcdCtrl.canvas.Mes (lvs);
}

void clearScr(void)
{
  glcdCtrl.canvas.color = false;
  glcdCtrl.canvas.Boxf(0, 0, glcdCtrl.canvas.SizeX(), glcdCtrl.canvas.SizeY());
  glcdCtrl.Draw();
}

void clearScrB(void)
{
  glcdCtrl.canvas.color = true;
  glcdCtrl.canvas.Boxf(0, 0, glcdCtrl.canvas.SizeX(), glcdCtrl.canvas.SizeY());
  glcdCtrl.Draw();
}

void gamemain_img()
{
  glcdCtrl.canvas.Pos(20, 47);
  glcdCtrl.canvas.Celput(player);

  glcdCtrl.canvas.Pos(glcdCtrl.canvas.posX + 10, glcdCtrl.canvas.posY);
  glcdCtrl.canvas.Celput(pbullet_img);
      
  glcdCtrl.canvas.Pos(glcdCtrl.canvas.posX + 3, glcdCtrl.canvas.posY);
  glcdCtrl.canvas.Celput(ebullet_img);
      
  glcdCtrl.canvas.Pos(glcdCtrl.canvas.posX + 10, glcdCtrl.canvas.posY);
  glcdCtrl.canvas.Celput(boss);
}


void init_bullet(void)
{
  for (n = 0; n < ebulletmax; n++)
  {
    ebullet[n][0] = 127;
    ebullet[n][1] = 0;
    ebullet[n][2] = 0;
    ebullet[n][3] = 0;
    ebullet[n][4] = 0;
    ebullet[n][5] = 0;
    ebullet[n][6] = 0;
  }
  for (n = 0; n < pbulletmax; n++)
  {
    pbullet[n][0] = 127;
    pbullet[n][1] = 0;
    pbullet[n][2] = 0;
    pbullet[n][3] = 0;
    pbullet[n][4] = 0;
    pbullet[n][5] = 0;
    pbullet[n][6] = 0;
  }

}
