
#include "TerminalPro4CommonSetting.h"

#include "MusicChartList.h"

char chartCount = 5;

int farTime = 900;
#define JUDGE_LINE 58
char noteWidthMin = 3;
char noteWidthMax = 20;
char noteHeightMin = 3;
char noteHeightMax = 8;
char centerLineY = 32;
int acceptableTime = 100;
char speedStep = 10;
char speedStepInterval = 100;
char speedStepMax = 20;
int farTimeMin = 100;
bool autoPlay = false;
int timingAdjustment = 200;

extern int Power(int a, int power);
extern int GetProportionalY(int x, int xRange, int yRange);
extern void MakeCoordinateConvTable(char *table, int n);
extern void PrintNumber(int num);
extern void ClearScrSplit();
extern void ClearScr();
extern bool InputAnyKey();

class Note {
  public:
    enum TYPE {
      LU,
      LD,
      RU,
      RD,
      LL,
      RR,
      LR,
      RL
    };
    char posX0, posX1;
    char posY0, posY1;
    char posX0Prev, posX1Prev;
    char posY0Prev, posY1Prev;
    bool hit = false;
    TYPE type;
    unsigned long time = 0;

    Note() {

    }

    Note(unsigned long time, TYPE type) {
      this->time = time;
      this->type = type;
    }
    ~Note() {

    }
    Note(const Note &note) {
      posX0 = note.posX0;
      posX1 = note.posX1;
      posY0 = note.posY0;
      posY1 = note.posY1;
      posX0Prev = note.posX0Prev;
      posX1Prev = note.posX1Prev;
      posY0Prev = note.posY0Prev;
      posY1Prev = note.posY1Prev;

      type = note.type;
      time = note.time;
    }

    void UpdatePrev() {
      posX0Prev = posX0;
      posX1Prev = posX1;
      posY0Prev = posY0;
      posY1Prev = posY1;
    }
};

class MusicChart {
  private:
    uint32_t *timeList = 0x00;
    uint8_t *typeList = 0x00;
    int noteIndex = 0;
    int length = 0;

  public:
    int Length() {
      return length;
    }

    template <typename T1, typename T2, int COL1, int COL2>
    bool Set(const T1(&time)[COL1], const T2(&type)[COL2]) {
      if (COL1 != COL2) {
        return false;
      }

      length = COL1;
      noteIndex = 0;

      timeList = time;
      typeList = type;

      return true;
    }

    bool Read(unsigned long *time, Note::TYPE *type) {
      if (noteIndex >= length) {
        return false;
      }
      *time = pgm_read_dword_near(&timeList[noteIndex]);
      *type = pgm_read_byte_near(&typeList[noteIndex]);
      noteIndex++;
      return true;
    }

    bool Peek(unsigned long *time, Note::TYPE *type) {
      if (noteIndex >= length) {
        return false;
      }
      *time = pgm_read_dword_near(&timeList[noteIndex]);
      *type = pgm_read_byte_near(&typeList[noteIndex]);

      return true;
    }
};


class NoteQueue {
  private:
    const static int capacity = 10;
    Note notes[capacity];
    int count = 0;
    int startIndex = -1;

  public:
    int Count() {
      return count;
    }

    Note& operator[] (int index) {
      return notes[(startIndex + index) % capacity];
    }

    bool Push(Note note) {
      if (count + 1 > capacity) {
        return false;
      }

      count++;
      if (count == 1) {
        startIndex = 0;
        notes[startIndex] = note;
      }
      else {
        notes[(startIndex + count - 1) % capacity] = note;
      }

      return true;
    }

    bool Pop(Note *note)
    {
      if (count <= 0)
      {
        return false;
      }

      count--;
      *note = notes[startIndex];

      startIndex = (startIndex + 1) % capacity;
      return true;
    }

};


MusicChart chart;
NoteQueue noteQueue;
char coordinateConvTable[JUDGE_LINE];
unsigned long startTime;

int combo = 0;
long hits = 0;
int maxCombo = 0;
char chartIndex = 0;
String str;

void setup() {
  SetupTerminalPro4();

  //Table作成
  MakeCoordinateConvTable(coordinateConvTable, JUDGE_LINE);
}

void loop() {
  ClearScrSplit();
  char chartIndexPrev = -1;
  char speedStepPrev = -1;
  bool repaint = false;
  for (;;) {
    unsigned char cL, cR;
    ctrlLeft.Stick(&cL, UP_BUTTON + RIGHT_BUTTON + DOWN_BUTTON + LEFT_BUTTON);
    ctrlRight.Stick(&cR, A_BUTTON + B_BUTTON + X_BUTTON + Y_BUTTON);

    //曲選択
    if (cL & RIGHT_BUTTON) {
      chartIndex++;
    }
    if (cL & LEFT_BUTTON) {
      chartIndex--;
    }
    if (chartIndex >= chartCount) {
      chartIndex = chartCount - 1;
    }
    if (chartIndex < 0) {
      chartIndex = 0;
    }
    if (chartIndex != chartIndexPrev) {
      repaint = true;
      chartIndexPrev = chartIndex;
      switch (chartIndex) {
        default:
        case 0:
          mediaCtrl.Load("senbon.wav", MediaPlayerController::PLAY_MODE::LOOP);
          mediaCtrl.Play();
          break;

        case 1:
          mediaCtrl.Load((str = F("Kemono.wav")).c_str(), MediaPlayerController::PLAY_MODE::LOOP);
          mediaCtrl.Play();
          break;

        case 2:
          mediaCtrl.Load((str = F("Inocen.wav")).c_str(), MediaPlayerController::PLAY_MODE::LOOP);
          mediaCtrl.Play();
          break;

        case 3:
          mediaCtrl.Load((str = F("Droout.wav")).c_str(), MediaPlayerController::PLAY_MODE::LOOP);
          mediaCtrl.Play();
          break;

        case 4:
          mediaCtrl.Load((str = F("Miiro.wav")).c_str(), MediaPlayerController::PLAY_MODE::LOOP);
          mediaCtrl.Play();
          break;
      }
    }

    //Speed選択
    if (cL & UP_BUTTON) {
      speedStep++;
    }
    if (cL & DOWN_BUTTON) {
      speedStep--;
    }
    if (speedStep > speedStepMax) {
      speedStep = speedStepMax;
    }
    if (speedStep < 1) {
      speedStep = 1;
    }
    if (speedStep != speedStepPrev) {
      repaint = true;
      speedStepPrev = speedStep;
    }

    if (cR & X_BUTTON) {
      autoPlay ^= true;
      repaint = true;
    }

    if (repaint) {
      ClearScr(); glcdCtrl.canvas.color = true;
      glcdCtrl.canvas.Line(0, 55, 127, 55);
      glcdCtrl.canvas.Line(96, 63, 127, 32);
      glcdCtrl.canvas.Line(100, 63, 127, 36);
      glcdCtrl.canvas.Pos(0, 0);
      glcdCtrl.canvas.Mes((str = F("#Notes")).c_str());
      glcdCtrl.canvas.Pos(10, 20);
      switch (chartIndex) {
        default:
        case 0:
          glcdCtrl.canvas.Mes((str = F("SenbonZakura")).c_str());
          break;
        case 1:
          glcdCtrl.canvas.Mes((str = F("YoukosoJapariParkE!")).c_str());
          break;
        case 2:
          glcdCtrl.canvas.Mes((str = F("InocentFlower")).c_str());
          break;
        case 3:
          glcdCtrl.canvas.Mes((str = F("GabrielDropout")).c_str());
          break;
        case 4:
          glcdCtrl.canvas.Mes((str = F("Miiro")).c_str());
          break;
      }
      glcdCtrl.canvas.Pos(60, 45);
      glcdCtrl.canvas.Mes((str = F("Speed:")).c_str());
      PrintNumber(speedStep);

      if (autoPlay) {
        glcdCtrl.canvas.Pos(104, 0);
        glcdCtrl.canvas.Mes((str = F("Auto")).c_str());
      }
      glcdCtrl.Draw();
      repaint = false;
    }

    //決定
    if (cR & A_BUTTON) {
      break;
    }

    delay(10);
  }
  //---GameMain------------------------------------------------
  ClearScrSplit();

  //各種変数初期化
  combo = 0;
  hits = 0;
  maxCombo = 0;
  farTime = farTimeMin + (20 - speedStep) * speedStepInterval;

  //読み込み
  switch (chartIndex) {
    default:
    case 0:

      chart.Set(senbonTime, senbonType);
      mediaCtrl.Load((str = F("senbon.wav")).c_str(), MediaPlayerController::PLAY_MODE::NORMAL);
      mediaCtrl.Play();
      break;

    case 1:

      chart.Set(kemonoTime, kemonoType);
      mediaCtrl.Load((str = F("Kemono.wav")).c_str(), MediaPlayerController::PLAY_MODE::NORMAL);
      mediaCtrl.Play();
      break;

    case 2:

      chart.Set(inocenTime, inocenType);
      mediaCtrl.Load((str = F("Inocen.wav")).c_str(), MediaPlayerController::PLAY_MODE::NORMAL);
      mediaCtrl.Play();
      break;
    case 3:

      chart.Set(drooutTime, drooutType);
      mediaCtrl.Load((str = F("Droout.wav")).c_str(), MediaPlayerController::PLAY_MODE::NORMAL);
      mediaCtrl.Play();
      break;

    case 4:

      chart.Set(miiroTime, miiroType);
      mediaCtrl.Load((str = F("Miiro.wav")).c_str(), MediaPlayerController::PLAY_MODE::NORMAL);
      mediaCtrl.Play();
      break;
  }

  //曲再生開始時間
  startTime = millis();

  while (mediaCtrl.IsPlaying()) {
    //再生時間取得
    unsigned long time = millis() - startTime + timingAdjustment;

    //キー入力取得
    unsigned char cR, cL;
    ctrlRight.Stick(&cR, A_BUTTON + B_BUTTON + X_BUTTON + Y_BUTTON);
    ctrlLeft.Stick(&cL, UP_BUTTON * RIGHT_BUTTON + DOWN_BUTTON + LEFT_BUTTON);

    //Note追加
    Note::TYPE nextNoteType;
    unsigned long nextNoteTime;

    for (;;) {
      if (chart.Peek(&nextNoteTime, &nextNoteType)
          && (nextNoteTime < time + farTime)
          && noteQueue.Push(Note(nextNoteTime, nextNoteType))
          && chart.Read(&nextNoteTime, &nextNoteType)) {
      }
      else {
        break;
      }
    }

    //Note移動
    for (int i = 0; i < noteQueue.Count(); i++) {
      Note *note = &noteQueue[i];
      int tableIndex = (note->time - time) * (JUDGE_LINE - 1) / farTime;
      if (tableIndex >= 0 && tableIndex < JUDGE_LINE) {
        int movingDistance = coordinateConvTable[tableIndex];
        int height = GetProportionalY(movingDistance, JUDGE_LINE, noteHeightMax - noteHeightMin) + noteHeightMin;
        int width = GetProportionalY(movingDistance, JUDGE_LINE, noteWidthMax - noteWidthMin) + noteWidthMin;
        switch (note->type) {
          case Note::TYPE::LU:
            note->posX0 = 63 - movingDistance;
            note->posX1 = note->posX0 + height;

            note->posY0 = centerLineY - width / 2 - width;
            note->posY1 = note->posY0 + width;
            break;
          case Note::TYPE::LR:
            note->posX0 = 63 - movingDistance;
            note->posX1 = note->posX0 + height;

            note->posY0 = centerLineY - width / 2;
            note->posY1 = note->posY0 + width;
            break;
          case Note::TYPE::LL:
            note->posX0 = 63 - movingDistance;
            note->posX1 = note->posX0 + height;

            note->posY0 = centerLineY - width / 2;
            note->posY1 = note->posY0 + width;
            break;
          case Note::TYPE::LD:
            note->posX0 = 63 - movingDistance;
            note->posX1 = note->posX0 + height;

            note->posY0 = centerLineY + width / 2;
            note->posY1 = note->posY0 + width;
            break;

          case Note::TYPE::RU:
            note->posX1 = 64 + movingDistance;
            note->posX0 = note->posX1 - height;

            note->posY0 = centerLineY - width / 2 - width;
            note->posY1 = note->posY0 + width;
            break;
          case Note::TYPE::RR:
            note->posX1 = 64 + movingDistance;
            note->posX0 = note->posX1 - height;

            note->posY0 = centerLineY - width / 2;
            note->posY1 = note->posY0 + width;
            break;
          case Note::TYPE::RL:
            note->posX1 = 64 + movingDistance;
            note->posX0 = note->posX1 - height;

            note->posY0 = centerLineY - width / 2;
            note->posY1 = note->posY0 + width;
            break;
          case Note::TYPE::RD:
            note->posX1 = 64 + movingDistance;
            note->posX0 = note->posX1 - height;

            note->posY0 = centerLineY + width / 2;
            note->posY1 = note->posY0 + width;
            break;

        }
      }
    }

    //---描画------------------------------------------------------------------------

    //Note描画
    for (int i = 0; i < noteQueue.Count(); i++) {

      Note *note = &noteQueue[i];

      glcdCtrl.canvas.color = false;
      glcdCtrl.canvas.Boxf(note->posX0Prev, note->posY0Prev, note->posX1Prev, note->posY1Prev);

      glcdCtrl.canvas.color = true;
      glcdCtrl.canvas.Boxf(note->posX0, note->posY0, note->posX1, note->posY1);
    }

    //Note削除, 非表示
    for (;;) {
      if ((noteQueue.Count() > 0) && (noteQueue[0].time < time - acceptableTime)) {
        Note note;
        noteQueue.Pop(&note);
        glcdCtrl.canvas.color = false;
        glcdCtrl.canvas.Boxf(note.posX0, note.posY0, note.posX1, note.posY1);

        if (!note.hit) {
          combo = 0;
        }
      }
      else {
        break;
      }
    }

    //NoteLaneLine
    glcdCtrl.canvas.color = true;
    glcdCtrl.canvas.Line(0, centerLineY, 127, centerLineY);

    glcdCtrl.canvas.Line(63 - JUDGE_LINE, centerLineY - noteWidthMax / 2, 63, centerLineY - noteWidthMin / 2);
    glcdCtrl.canvas.Line(63 - JUDGE_LINE, centerLineY + noteWidthMax / 2, 63, centerLineY + noteWidthMin / 2);
    glcdCtrl.canvas.Line(63 - JUDGE_LINE, centerLineY - noteWidthMax / 2 - noteWidthMax, 63, centerLineY - noteWidthMin / 2 - noteWidthMin);
    glcdCtrl.canvas.Line(63 - JUDGE_LINE, centerLineY + noteWidthMax / 2 + noteWidthMax, 63, centerLineY + noteWidthMin / 2 + noteWidthMin);

    glcdCtrl.canvas.Line(64 + JUDGE_LINE, centerLineY - noteWidthMax / 2, 64, centerLineY - noteWidthMin / 2);
    glcdCtrl.canvas.Line(64 + JUDGE_LINE, centerLineY + noteWidthMax / 2, 64, centerLineY + noteWidthMin / 2);
    glcdCtrl.canvas.Line(64 + JUDGE_LINE, centerLineY - noteWidthMax / 2 - noteWidthMax, 64, centerLineY - noteWidthMin / 2 - noteWidthMin);
    glcdCtrl.canvas.Line(64 + JUDGE_LINE, centerLineY + noteWidthMax / 2 + noteWidthMax, 64, centerLineY + noteWidthMin / 2 + noteWidthMin);

    //判定ライン
    glcdCtrl.canvas.color = true;
    glcdCtrl.canvas.Line(63 - JUDGE_LINE, 0, 63 - JUDGE_LINE, 63);
    glcdCtrl.canvas.Line(64 + JUDGE_LINE, 0, 64 + JUDGE_LINE, 63);

    glcdCtrl.canvas.color = false;
    glcdCtrl.canvas.Line(63 - JUDGE_LINE + 1, 0, 63 - JUDGE_LINE + 1, 63);
    glcdCtrl.canvas.Line(63 - JUDGE_LINE - 1, 0, 63 - JUDGE_LINE - 1, 63);
    glcdCtrl.canvas.Line(64 + JUDGE_LINE + 1, 0, 64 + JUDGE_LINE + 1, 63);
    glcdCtrl.canvas.Line(64 + JUDGE_LINE - 1, 0, 64 + JUDGE_LINE - 1, 63);

    //判定, 非表示
    bool hitOnFrame = false;
    for (int i = 0; i < noteQueue.Count(); i++) {
      Note *note = &noteQueue[i];

      if ((note->time >= (time - acceptableTime)) && (note->time <= (time + acceptableTime))) {
        if (!note->hit) {
          bool hit = false;
          switch (note->type) {
            case Note::TYPE::LL:
            case Note::TYPE::LR:
              if ((cL & LEFT_BUTTON) || (cL & RIGHT_BUTTON)) {
                hit = true;
              }
              break;
            case Note::TYPE::LU:
              if (cL & UP_BUTTON) {
                hit = true;
              }
              break;
            case Note::TYPE::LD:
              if (cL & DOWN_BUTTON) {
                hit = true;
              }
              break;
            case Note::TYPE::RL:
            case Note::TYPE::RR:
              if ((cR & Y_BUTTON) || (cR & A_BUTTON)) {
                hit = true;
              }
              break;
            case Note::TYPE::RU:
              if (cR & X_BUTTON) {
                hit = true;
              }
              break;
            case Note::TYPE::RD:
              if (cR & B_BUTTON) {
                hit = true;
              }
              break;
          }
          if (autoPlay) {
            hit = true;
          }
          if (hit) {
            note->hit = true;
            glcdCtrl.canvas.color = false;
            glcdCtrl.canvas.Boxf(note->posX0, note->posY0, note->posX1, note->posY1);

            combo++;
            if (combo > maxCombo) {
              maxCombo = combo;
            }
            hits++;

            hitOnFrame = true;
          }
        }
      }
      else {
        break;
      }
    }
    //コンボ表示
    glcdCtrl.canvas.color = true;
    glcdCtrl.canvas.Pos(50, 0);
    glcdCtrl.canvas.Mes("Combo");
    if (hitOnFrame) {
      glcdCtrl.canvas.color = true;
      glcdCtrl.canvas.Boxf(52, 8, 76, 16);
      glcdCtrl.canvas.color = false;
      glcdCtrl.canvas.Pos(52, 8);
      PrintNumber(combo);
    }
    else {
      glcdCtrl.canvas.color = false;
      glcdCtrl.canvas.Boxf(52, 8, 76, 16);
      glcdCtrl.canvas.color = true;
      glcdCtrl.canvas.Pos(52, 8);
      PrintNumber(combo);
    }

    //hit表示

    glcdCtrl.canvas.color = false;
    glcdCtrl.canvas.Boxf(52, 55, 76, 63);

    glcdCtrl.canvas.color = true;
    glcdCtrl.canvas.Pos(52, 55);
    PrintNumber(hits * 100 / chart.Length());
    glcdCtrl.canvas.Pos(70, 55);
    glcdCtrl.canvas.Mes("%");

    glcdCtrl.Draw();

    //Prev更新
    for (int i = 0; i < noteQueue.Count(); i++) {
      noteQueue[i].UpdatePrev();
    }

  }


  //---Result------
  ClearScrSplit();
  glcdCtrl.canvas.color = true;
  glcdCtrl.canvas.Pos(0, 0);
  glcdCtrl.canvas.Mes((str = "Result:").c_str());
  glcdCtrl.canvas.Pos(10, 8);
  glcdCtrl.canvas.Mes("Hits: ");
  PrintNumber(hits);
  glcdCtrl.canvas.Mes((str = F(" / ")).c_str());
  PrintNumber(chart.Length());
  glcdCtrl.canvas.Pos(10, 16);
  glcdCtrl.canvas.Mes((str = F("MaxCombo: ")).c_str());
  PrintNumber(maxCombo);
  if (maxCombo == chart.Length()) {
    glcdCtrl.canvas.Pos(10, 24);
    glcdCtrl.canvas.Mes((str = F("FULL COMBO!!")).c_str());
  }

  glcdCtrl.Draw();
  while (!InputAnyKey());
}

void MakeCoordinateConvTable(char *table, int n) {
  for (int i = 0; i < n; i++) {
    //table[i] = (0.5 - 0.5 * cos(M_PI + M_PI * i / n)) * n;
    //table[i] = sqrt(1.0 - ((double)i / n) * ((double)i / n)) * n;
    //table[i] = (-sqrt(1.0 - (((double)i / n) - 1.0) * (((double)i / n) - 1.0)) + 1.0) * n;
    table[i] = ((((double)i / n) - 1.0) * (((double)i / n) - 1.0)) * n;
  }
}

int GetProportionalY(int x, int xRange, int yRange) {

  int deltaX = xRange;
  int deltaY = yRange;
  int error = 0;
  int y = 0;
  for (int i = 0; i <= x; i++)
  {
    error += deltaY;
    if ((error << 1) >= deltaX)
    {
      y++;
      error -= deltaX;
    }
  }
  return y;
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

int Power(int a, int power) {
  int powered = 1;
  for (int i = 0; i < power; i++) {
    powered *= a;
  }
  return powered;
}


void ClearScrSplit() {
  glcdCtrl.canvas.color = false;
  for (int i = 0; i < 64; i++) {
    glcdCtrl.canvas.Line(63 - i, 0, 63 - i, 63);
    glcdCtrl.canvas.Line(64 + i, 0, 64 + i, 63);
    glcdCtrl.Draw();
    delay(5);
  }
}

void ClearScr() {
  glcdCtrl.canvas.color = false;
  glcdCtrl.canvas.Boxf(0, 0, 127, 63);
}

bool InputAnyKey() {
  byte c1, c2;
  ctrlRight.Stick(&c1, A_BUTTON + B_BUTTON + X_BUTTON + Y_BUTTON);
  ctrlLeft.Stick(&c2, UP_BUTTON + DOWN_BUTTON + RIGHT_BUTTON + LEFT_BUTTON);
  if (c1 | c2) {
    return true;
  }

  return false;
}

