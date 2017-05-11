//DB0~DB7
//PORTD 2~7 PORTB 0~1


#include "Canvas.h"
#include "Arduino.h"

class GLCDController
{
  public:
    class Param
    {
      public:
        byte pinRS;
        byte pinRW;
        byte pinE;
        byte pinCS1;
        byte pinCS2;
        byte pinRST;

        byte sizeX;
        byte sizeY;

        void CopyFrom(Param &from) {
          this->pinRS = from.pinRS;
          this->pinRW = from.pinRW;
          this->pinE = from.pinE;
          this->pinCS1 = from.pinCS1;
          this->pinCS2 = from.pinCS2;
          this->pinRST = from.pinRST;
          this->sizeX = from.sizeX;
          this->sizeY = from.sizeY;
        }
    };

  private:
    Param param;
    unsigned char *scrBuf;
    unsigned char *updated;

  public:
    Canvas canvas;

    void Begin(Param &param)
    {
      this->param.CopyFrom(param);

      //グラフィックディスプレイを使う前に必ずする
      InitGLCD();  // GLCD の初期化
      ClsGLCD();   // CLS

      //canvas設定
      canvas.SetSize(param.sizeX, param.sizeY);

      scrBuf = canvas.scrBuf;
      updated = canvas.updated;
    }

    //チップの選択
    void ChipSelect(boolean cs)
    {
      if (cs == 0)
      {
        digitalWrite(param.pinCS1, HIGH); // cs = 0 chip 1
        digitalWrite(param.pinCS2, LOW);
      }
      else
      {
        digitalWrite(param.pinCS1, LOW);  // cs = 1 chip 2
        digitalWrite(param.pinCS2, HIGH);
      }
    }

    //ポートのセットと送信
    void WriteBUS(boolean rs, boolean rw, byte dat)
    {
      digitalWrite(param.pinRS, rs);  // RS をセット

      // データをバスにセット
      PORTD = (dat << 2) | (PORTD & 0x03);
      PORTB = (dat >> 6) | (PORTB & 0xfc);

      digitalWrite(param.pinE, HIGH); // イネーブルをラッチ
      digitalWrite(param.pinE, LOW);
    }

    //コマンドの送信
    void WriteCommand(byte dat)
    {
      WriteBUS(0, 0, dat);  // RS=0, RW=0
    }

    //データの送信
    void WriteData(byte dat)
    {
      WriteBUS(1, 0, dat);  // RS=1, RW=0
    }

    //アドレスのセット
    void SetAddress(byte col, byte row)
    {
      WriteBUS(0, 0, 0x40 | (col & 0x3F)); // set address (0-63)
      WriteBUS(0, 0, 0xB8 | (row & 0x07)); // set page (0-7)
    }

    //すでに表示されているデータを読み取る
    byte ReadData(void)
    {
      byte ret = 0x00;  // 戻り値用変数

      //バスピンを入力モードに設定
      DDRD &= ~B11111100;
      DDRB &= ~B00000011;

      digitalWrite(param.pinRS, HIGH);
      digitalWrite(param.pinRW, HIGH);
      digitalWrite(param.pinE, HIGH);
      digitalWrite(param.pinE, LOW);
      digitalWrite(param.pinE, HIGH);

      ret = (PIND >> 2) | (PINB << 6);

      digitalWrite(param.pinE, LOW);
      digitalWrite(param.pinRW, LOW);

      //バスピンを出力モードに設定
      DDRD |= B11111100;
      DDRB |= B00000011;

      return (ret);
    }

    //GLCDの初期化
    void InitGLCD(void)
    {
      // ポート設定
      pinMode(param.pinRS, OUTPUT);
      pinMode(param.pinRW, OUTPUT);
      pinMode(param.pinE, OUTPUT);
      pinMode(param.pinCS1, OUTPUT);
      pinMode(param.pinCS2, OUTPUT);
      pinMode(param.pinRST, OUTPUT);

      //バスピンを出力モードに設定
      DDRD |= B11111100;
      DDRB |= B00000011;

      // 初期状態として LOW に設定
      digitalWrite(param.pinRS, LOW);
      digitalWrite(param.pinRW, LOW);
      digitalWrite(param.pinE, LOW);
      digitalWrite(param.pinCS1, LOW);
      digitalWrite(param.pinCS2, LOW);
      digitalWrite(param.pinRST, HIGH); // Reset 解除

      delay(30);

      // chip 1
      ChipSelect(0);

      // Display Start Line = 0
      WriteCommand(0xC0);

      // Display On
      WriteCommand(0x3F);

      // chip 2
      ChipSelect(1);
      WriteCommand(0xC0);
      WriteCommand(0x3F);
    }

    //画面に表示されているものを消す, アドレスを左上に設定
    void ClsGLCD(void)
    {
      byte col, row, i;

      for (i = 0; i < 2; i++)
      {
        ChipSelect(i);        // チップを選択
        for (row = 0; row < 8; row++)
        {
          SetAddress(0, row);  // アドレスをセット
          for (col = 0; col < 64; col++)
          {
            WriteData(0);     // 0x00 を送信
          }
        }
      }
      SetAddress(0, 0);       // Return Home
    }

    //ディスプレイ,オンオフ
    void SwitchDisplay(byte i)
    {
      if (i == 0)
      {
        ChipSelect(0);
        WriteCommand(0x3E); // Display Off
        ChipSelect(1);
        WriteCommand(0x3E); // Display Off
      }
      else
      {
        ChipSelect(0);
        WriteCommand(0x3F); // Display On
        ChipSelect(1);
        WriteCommand(0x3F); // Display On
      }
    }

    void Draw(void)
    {
      for (byte i = 0; i < 2; i++)
      {
        ChipSelect(i);
        for (int page = 0; page < 8; page++) {
          for (int address = 0; address < 64; address++) {
            int bufIndex = page * 128 + i * 64 + address;
            //内容が更新されているのみ更新
            if (updated[bufIndex >> 3] & (0x01 << (bufIndex & 0x07))) {
              //アドレス指定, 書き込み
              SetAddress(address, page);
              WriteData(scrBuf[bufIndex]);

              //更新済みにする
              updated[bufIndex >> 3] &= ~(0x01 << (bufIndex & 0x07));
            }
          }
        }
      }
    }
};
