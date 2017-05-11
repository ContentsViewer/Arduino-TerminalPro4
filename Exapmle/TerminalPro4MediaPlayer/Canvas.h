#ifndef _INCLUDE_CANVAS_H_
#define _INCLUDE_CANVAS_H_

#include "Font.h"
#include "Images.h"

class Canvas
{
  private:
    static int Abs(int a) {
      return a > 0 ? a : -a;
    }
    static Unallocate(unsigned char* array)
    {
      if (array != 0x00)
      {
        delete[] array;
        array = 0x00;
      }
    }

    int sizeX, sizeY;
    int updatedCount = 0, scrBufCount = 0;

  public:
    unsigned char *scrBuf = 0x00;
    unsigned char *updated = 0x00;

    int SizeX() {
      return sizeX;
    }
    int SizeY() {
      return sizeY;
    }
    void SetSize(int sizeX, int sizeY);
    bool color = true;
    int posX = 0, posY = 0;
    void Pos(int x, int y) {
      posX = x;
      posY = y;
    }
    Canvas(int sizeX, int sizeY);
    Canvas();
    ~Canvas();
    void Dot(int x, int y) {
      if (x < 0 || x >= sizeX || y < 0 || y >= sizeY)
      {
        return;
      }

      int bufIndex = (y >> 3) * sizeX + x;

      //前回の情報保存
      unsigned char temp = scrBuf[bufIndex];

      //点を打つ
      if (color)
      {
        //memo:
        //  y & 0x07:
        //    yを8で割ったときのあまり
        scrBuf[bufIndex] |= 0x01 << (y & 0x07);
      }
      else
      {
        scrBuf[bufIndex] &= ~(0x01 << (y & 0x07));
      }

      //前回のデータと異なるときフラグを立てる
      if (scrBuf[bufIndex] != temp)
      {
        //memo:
        //  bufIndex >> 3:
        //    bufIndexを8で割ったときの商
        updated[bufIndex >> 3] |= (0x01 << (bufIndex & 0x07));
      }
    }

    void Line(int x0, int y0, int x1, int y1)
    {
      int steep, t;
      int deltaX, deltaY;
      int x, y;
      int yStep, error;

      // 差分の大きいほうを求める
      steep = (Abs(y1 - y0) > Abs(x1 - x0));
      //x,yの入れ替え
      if (steep)
      {
        t = x0; x0 = y0; y0 = t;
        t = x1; x1 = y1; y1 = t;
      }
      if (x0 > x1)
      {
        t = x0; x0 = x1; x1 = t;
        t = y0; y0 = y1; y1 = t;
      }
      deltaX = x1 - x0;  // 傾き計算
      deltaY = Abs(y1 - y0);
      error = 0;
      y = y0;
      // 傾きでステップの正負を切り替え
      if (y0 < y1) yStep = 1; else yStep = -1;
      // 直線を点で描画
      for (x = x0; x <= x1; x++)
      {
        if (steep)
        {
          Dot(y, x);
        }
        else
        {
          Dot(x, y);
        }

        error += deltaY;

        if ((error << 1) >= deltaX)
        {
          y += yStep;
          error -= deltaX;
        }
      }
    }

    void VerticalLine(int x, int y, int length) {
      int dir = (length > 0) ? 1 : -1;
      length = Abs(length);
      for (int i = 0; i < length; i++) {
        Dot(x, y);
        y += dir;
      }
    }

    void Boxf(int x0, int y0, int x1, int y1) {
      int t;
      if (x1 < x0) //xの入れかえ
      {
        t = x0; x0 = x1; x1 = x0;
      }
      for (; x0 <= x1; x0++)
      {
        //直線を描く
        VerticalLine(x0, y0, y1 - y0 + 1);
      }
    }

    void Boxw(int x0, int y0, int x1, int y1) {
      Line(x0, y0, x1, y0);
      Line(x1, y0, x1, y1);
      Line(x1, y1, x0, y1);
      Line(x0, y1, x0, y0);
    }

    template <typename T, int COL>
    void Celput(const T(&imagName)[COL]) {

      //画像の縦の長さを調べる
      byte row = sizeof(T);

      for (int i = 0; i < COL; i++)
      {
        //縦の長さに応じて処理を分ける
        if (row == 1)
        {
          for (int j = 0; j < 8; j++)
          {
            if (0x01 & (pgm_read_byte_near(&imagName[i]) >> j))
            {
              Dot(posX, posY + j);
            }
          }
        }
        else if (row == 2)
        {
          for (int j = 0; j < 16; j++)
          {

            if (0x01 & (pgm_read_word_near(&imagName[i]) >> j))
            {
              Dot(posX, posY + j);
            }
          }
        }
        else if (row == 4)
        {
          for (int j = 0; j < 32; j++)
          {
            if (0x01 & (pgm_read_dword_near(&imagName[i]) >> j))
            {
              Dot(posX, posY + j);
            }
          }
        }

        //次の列へ移動
        posX++;
      }
    }

    void PutChar(byte b) {
      if (b < 0x20 || b > 0x7f)
      {
        Celput(font[0x2e - 0x20]);
      }
      else
      {
        Celput(font[b - 0x20]);
      }

      //文字間隔を1ドット分あける
      posX++;
    }

    void Mes(char *c)
    {
      while (*c)
      {
        PutChar(*c++);
      }
    }
};

Canvas::Canvas(int sizeX, int sizeY)
{
  SetSize(sizeX, sizeY);
}

Canvas::Canvas()
{
  SetSize(0, 0);
}

Canvas::~Canvas()
{
  Unallocate(scrBuf);
  Unallocate(updated);
}

void Canvas::SetSize(int sizeX, int sizeY)
{
  Unallocate(scrBuf);
  Unallocate(updated);

  this->sizeX = sizeX;
  this->sizeY = sizeY;

  scrBufCount = sizeX * ((sizeY + 7) / 8);
  updatedCount = (scrBufCount + 7) / 8;

  if (scrBufCount > 0)
  {
    scrBuf = new unsigned char[scrBufCount];
  }

  if (updatedCount > 0)
  {
    updated = new unsigned char[updatedCount];
  }

  for (int i = 0; i < scrBufCount; i++)
  {
    scrBuf[i] = 0x00;
  }
  for (int i = 0; i < updatedCount; i++)
  {
    updated[i] = 0x00;
  }
}
#endif

