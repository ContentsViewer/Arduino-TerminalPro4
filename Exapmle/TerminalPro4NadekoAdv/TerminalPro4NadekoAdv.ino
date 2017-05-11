#include "TerminalPro4CommonSetup.h"

#include "StageList.h"

char stageCount = 4;

char viewportSizeX = 88;
char viewportSizeY = 64;

char spaceSizeXBetweenScrollLineAndPlayer = 32;
char spaceSizeYBetweenScrollLineAndPlayer = 20;

char imgSwapInterval = 10;

int deadLineY = -16;

void PrintNum(unsigned int num);
void ClearScr();
void ClearScrBlack();

class Character {
  public:
    int posX = 0;
    int posY = 0;
    int posXPrev = 0;
    int posYPrev = 0;

    char imgId = 0;
    char imgIdPrev = 0;

    char dir = 1;
    char dirPrev = 1;

    bool grounded = false;
};

class Enemy : public Character {
  public:
    //0: 非アクティブ
    //1: アクティブ
    //2: 死亡
    char state = 0;
    static char size;
    static  char velocityYMin;
    static char speed;

    static void Celput(char id) {
      switch (id) {
        case 0:
          glcdCtrl.canvas.Celput(snakeA);
          break;
        case 1:
          glcdCtrl.canvas.Celput(snakeAR);
          break;
        default:
          glcdCtrl.canvas.Celput(snakeA);
          break;
      }
    }
};

char Enemy::size = 8;
char Enemy::velocityYMin = -4;
char Enemy::speed = 1;
class Player : public Character {
  public:
    float velocityYMin = -5;
    float jumpVelocity = 5;
    float gravity = 0.5;

    char walkSpeed = 2;
    char runSpeed = 4;
    char size = 16;
    char footBeginX = 7;
    char footEndX = 13;
    char footBeginXR = 2;
    char footEndXR = 8;
    bool groundedPrev = false;
    float velocityY = 0;

    void Celput(char id) {
      switch (id) {
        case 0:
          glcdCtrl.canvas.Celput(nadekoWalkA);
          break;

        case 1:
          glcdCtrl.canvas.Celput(nadekoWalkB);
          break;

        case 2:
          glcdCtrl.canvas.Celput(nadekoWalkAR);
          break;

        case 3:
          glcdCtrl.canvas.Celput(nadekoWalkBR);
          break;

        default:
          glcdCtrl.canvas.Celput(nadekoWalkA);
          break;
      }
    }
};
class Goal : public Character {
  public:
    int size = 16;
    void Celput(char id) {
      switch (id) {
        case 0:
          glcdCtrl.canvas.Celput(koyomiA);
          break;

        case 1:
          glcdCtrl.canvas.Celput(koyomiB);
          break;

        default:
          glcdCtrl.canvas.Celput(koyomiA);
          break;
      }
    }
};

class Stage {
  private:
    int height = 12;
    int length = 0;
    int count = 0;
    uint8_t *elements = 0x00;

  public:
    Player player;
    Goal goal;
    Enemy enemyList[12];
    char enemyCount = 0;

    char blockSize = 8;
    enum OBJECT
    {
      SPACE = 0,
      BLOCK = 1,
      PLAYER = 2,
      ENEMY = 3,
      GOAL = 4
    };

    OBJECT GetObject(int x, int y) {
      if (x < length && x >= 0 && y < height && y >= 0) {
        return (OBJECT)pgm_read_byte_near(&elements[height * x + y]);
      }
      return OBJECT::SPACE;
    }

    OBJECT GetObject(int index) {
      if (count > index) {
        return (OBJECT)pgm_read_byte_near(&elements[index]);
      }
      return OBJECT::SPACE;
    }


    ~Stage() {
    }

    int Length() {
      return length;
    }
    int Height() {
      return height;
    }
    template <typename T, int COL>
    bool Set(const T(&stage)[COL]) {

      if (COL % height != 0) {
        return false;
      }

      count = COL;
      length = COL / height;

      elements = &stage[0];

      enemyCount = 0;

      for (int i = 0; i < COL; i++) {
        if ((OBJECT)pgm_read_byte_near(&elements[i]) == OBJECT::PLAYER) {
          player.posX =  (i / height) * blockSize;
          player.posY = (i % height) * blockSize;
          player.posXPrev = player.posX;
          player.posYPrev = player.posY;
          player.dir = 1;
          player.dirPrev = player.dir;
          player.imgId = 0;
          player.imgIdPrev = 0;
          player.velocityY = 0.0;
        }
        else if ((OBJECT)pgm_read_byte_near(&elements[i]) == OBJECT::GOAL) {
          goal.posX =  (i / height) * blockSize;
          goal.posY = (i % height) * blockSize;
          goal.posXPrev = goal.posX;
          goal.posYPrev = goal.posY;
        }
        else if ((OBJECT)pgm_read_byte_near(&elements[i]) == OBJECT::ENEMY) {
          if (enemyCount < (sizeof(enemyList) / sizeof(enemyList[0]))) {
            enemyList[enemyCount].posX = (i / height) * blockSize;
            enemyList[enemyCount].posY = (i % height) * blockSize;
            enemyList[enemyCount].posXPrev = enemyList[enemyCount].posX;
            enemyList[enemyCount].posYPrev = enemyList[enemyCount].posY;
            enemyList[enemyCount].dir = -1;
            enemyList[enemyCount].dirPrev = enemyList[enemyCount].dir;
            enemyList[enemyCount].state = 0;
            enemyCount++;
          }
        }
      }
    }
};

//グローバル変数
Stage stage;

int stageIndex = 0;

String str;


void setup() {
  SetUpTerminalPro3Main();
}

void loop() {
  //---ステージ選択画面--------------------------------------------------------------------------------

  ClearScr();

  glcdCtrl.canvas.color = true;
  glcdCtrl.canvas.Pos(0, 0);
  glcdCtrl.canvas.Celput(nadekoUpperBody);
  glcdCtrl.canvas.Pos(0, 32);
  glcdCtrl.canvas.Celput(nadekoLowerBody);
  glcdCtrl.canvas.Pos(35, 0);
  glcdCtrl.canvas.Mes((str = F("NadekoDaYO!")).c_str());
  glcdCtrl.Draw();

  mediaCtrl.Load((str = F("NaDayo.wav")).c_str(), MediaPlayerController::PLAY_MODE::WAIT);
  mediaCtrl.Play();
  mediaCtrl.Load((str = F("sengok_1.wav")).c_str(), MediaPlayerController::PLAY_MODE::LOOP);
  mediaCtrl.Play();

  int stageIndexPrev = -1;
  for (;;) {
    unsigned char cL, cR;
    ctrlLeft.Stick(&cL, 0);
    ctrlRight.Stick(&cR, A_BUTTON + B_BUTTON + X_BUTTON + Y_BUTTON);

    //ステージ選択
    if (cL & RIGHT_BUTTON) {
      stageIndex++;
    }
    if (cL & LEFT_BUTTON) {
      stageIndex--;
    }
    if (stageIndex >= stageCount) {
      stageIndex = stageCount - 1;
    }
    if (stageIndex < 0) {
      stageIndex = 0;
    }
    if (stageIndex != stageIndexPrev) {
      ClearScr();

      glcdCtrl.canvas.color = true;
      glcdCtrl.canvas.Pos(0, 0);
      glcdCtrl.canvas.Celput(nadekoUpperBody);
      glcdCtrl.canvas.Pos(0, 32);
      glcdCtrl.canvas.Celput(nadekoLowerBody);
      glcdCtrl.canvas.Pos(35, 0);
      glcdCtrl.canvas.Mes((str = F("NadekoDaYO!")).c_str());
      glcdCtrl.canvas.Pos(35, 20);
      glcdCtrl.canvas.Mes((str = F("Stage")).c_str());
      PrintNum(stageIndex);
      glcdCtrl.Draw();

      stageIndexPrev = stageIndex;
    }

    //決定
    if (cR) {
      break;
    }

    delay(10);
  }

  //---GameMain----------------------------------------------------------------------

  for (;;) {
    if (stageIndex % 2 == 0) {
      mediaCtrl.Load((str = F("sengok_1.wav")).c_str(), MediaPlayerController::PLAY_MODE::LOOP);
    }
    else {
      mediaCtrl.Load((str = F("Mousou.wav")).c_str(), MediaPlayerController::PLAY_MODE::LOOP);
    }
    mediaCtrl.Play();
    ClearScr();

    //ステージ読み込み(ステージ追加時変更必要)
    switch (stageIndex) {
      case 0:
        stage.Set(stageA);
        break;
      case 1:
        stage.Set(stageB);
        break;
      case 2:
        stage.Set(stageC);
        break;
      case 3:
        stage.Set(stageD);
        break;
      default:
        stage.Set(stageA);
        break;
    }

    Player *player = &(stage.player);
    Goal *goal = &(stage.goal);
    Enemy *enemyList = &(stage.enemyList[0]);
    char blockSize = stage.blockSize;

    int scrollX = 0;
    int scrollXPrev = scrollX;
    int scrollY = 0;
    int scrollYPrev = scrollY;

    bool isGameOver = false;
    char imgSwapCnt = 0;

    byte cR, cL;

    for (;;) {
      ctrlRight.Stick(&cR, A_BUTTON);
      ctrlLeft.Stick(&cL, 0);

      //---位置更新--------------------------------------------------------------------------------------------
      //memo: 移動した後はその位置を保証せよ. 移動→判定
      //---Player---------------------------
      //左右移動
      if (cL & RIGHT_BUTTON) {
        player->dir = 1;
        if (player->dir == player->dirPrev) {
          if (cR & B_BUTTON) {
            player->posX += player->runSpeed;
          }
          else {
            player->posX += player->walkSpeed;
          }
        }
      }

      if (cL & LEFT_BUTTON) {
        player->dir = -1;
        if (player->dir == player->dirPrev) {
          if (cR & B_BUTTON) {
            player->posX -= player->runSpeed;
          }
          else {
            player->posX -= player->walkSpeed;
          }
        }
      }

      //足元範囲
      char playerFootBeginX, playerFootEndX;
      if (player->dir > 0) {
        playerFootBeginX = player->footBeginX;
        playerFootEndX = player->footEndX;
      }
      else {
        playerFootBeginX = player->footBeginXR;
        playerFootEndX = player->footEndXR;
      }

      //境界判定
      {
        int sideCheckerBPosY = player->posY;
        int sideCheckerCPosY = player->posY + 7;
        int sideCheckerTPosY = player->posY + 15;
        //右から左
        if (player->posX > player->posXPrev) {
          int sideCheckerRPosX = player->posX + playerFootEndX;
          if ((stage.GetObject(sideCheckerRPosX / blockSize, sideCheckerBPosY / blockSize) == Stage::OBJECT::BLOCK)
              | (stage.GetObject(sideCheckerRPosX / blockSize, sideCheckerCPosY / blockSize) == Stage::OBJECT::BLOCK)
              | (stage.GetObject(sideCheckerRPosX / blockSize, sideCheckerTPosY / blockSize) == Stage::OBJECT::BLOCK)) {
            player->posX = (sideCheckerRPosX / blockSize) * blockSize - (playerFootEndX + 1);
          }
        }
        //左から右
        else if (player->posX < player->posXPrev) {
          int sideCheckerLPosX = player->posX + playerFootBeginX;
          if ((stage.GetObject(sideCheckerLPosX / blockSize, sideCheckerBPosY / blockSize) == Stage::OBJECT::BLOCK)
              | (stage.GetObject(sideCheckerLPosX / blockSize, sideCheckerCPosY / blockSize) == Stage::OBJECT::BLOCK)
              | (stage.GetObject(sideCheckerLPosX / blockSize, sideCheckerTPosY / blockSize) == Stage::OBJECT::BLOCK)) {
            player->posX = ((sideCheckerLPosX / blockSize) + 1) * blockSize - (playerFootBeginX);
          }
        }
      }

      //上下移動
      if (player->grounded && (cR & A_BUTTON)) {
        player->velocityY = player->jumpVelocity;
        player->grounded = false;
      }

      if (player->grounded) {
        player->velocityY = 0;
      }
      else {
        player->posY += player->velocityY;
        player->velocityY -= player->gravity;
        if (player->velocityY < player->velocityYMin) {
          player->velocityY = player->velocityYMin;
        }
      }

      //境界判定
      //Player上
      {
        int topCheckerPosY = player->posY + player->size;
        int topCheckerLPosX = player->posX + playerFootBeginX;
        int topCheckerRPosX = player->posX + playerFootEndX;

        if ((stage.GetObject(topCheckerLPosX / blockSize, topCheckerPosY / blockSize) == Stage::OBJECT::BLOCK)
            | (stage.GetObject(topCheckerRPosX / blockSize, topCheckerPosY / blockSize) == Stage::OBJECT::BLOCK)) {
          player->posY = (topCheckerPosY / blockSize) * blockSize - player->size;
          player->velocityY = -1.0;
        }
      }

      //接地判定
      {
        int gndCheckerPosY;
        int gndCheckerLPosX, gndCheckerRPosX;
        gndCheckerPosY = player->posY - 1;
        gndCheckerLPosX = player->posX + playerFootBeginX;
        gndCheckerRPosX = player->posX + playerFootEndX;
        if ((stage.GetObject(gndCheckerLPosX / blockSize, gndCheckerPosY / blockSize) == Stage::OBJECT::BLOCK)
            | (stage.GetObject(gndCheckerRPosX / blockSize, gndCheckerPosY / blockSize) == Stage::OBJECT::BLOCK)) {
          player->grounded = true;
          player->posY = ((gndCheckerPosY / blockSize) + 1) * blockSize;

        } else {
          player->grounded = false;
        }
        for (int i = 0; i < stage.enemyCount; i++) {
          if (enemyList[i].state == 1) {
            if (player->velocityY < 0.0
                && ((gndCheckerLPosX >= enemyList[i].posX && gndCheckerLPosX < (enemyList[i].posX + Enemy::size) && gndCheckerPosY >= enemyList[i].posY && gndCheckerPosY < (enemyList[i].posY + Enemy::size))
                    || (gndCheckerRPosX >= enemyList[i].posX && gndCheckerRPosX < (enemyList[i].posX + Enemy::size) && gndCheckerPosY >= enemyList[i].posY && gndCheckerPosY < (enemyList[i].posY + Enemy::size)))) {
              enemyList[i].state = 2;
              if (cR & A_BUTTON) {
                player->velocityY = player->jumpVelocity + 1.0;
              }
              else {
                player->velocityY = player->jumpVelocity / 2.0;
              }
            }
          }
        }
      }

      //---Enemy-------------------------------------------
      for (int i = 0; i < stage.enemyCount; i++) {
        Enemy *enemy = &(enemyList[i]);
        if (enemy->state == 1) {
          //上下移動
          if (!enemy->grounded) {
            enemy->posY += Enemy::velocityYMin;
          }
          //境界判定
          int gndCheckerPosY = enemy->posY - 1;
          int gndCheckerLPosX, gndCheckerRPosX;
          gndCheckerLPosX = enemy->posX;
          gndCheckerRPosX = enemy->posX + 7;

          if ((stage.GetObject(gndCheckerLPosX / blockSize, gndCheckerPosY / blockSize) == Stage::OBJECT::BLOCK)
              | (stage.GetObject(gndCheckerRPosX / blockSize, gndCheckerPosY / blockSize) == Stage::OBJECT::BLOCK)) {
            enemy->grounded = true;
            enemy->posY = ((gndCheckerPosY / blockSize) + 1) * blockSize;
          } else {
            enemy->grounded = false;
          }

          //左右移動
          if (enemy->grounded) {
            enemy->posX += enemy->dir * Enemy::speed;
          }

          //境界判定
          int sideCheckerBPosY = enemy->posY;
          //左から右
          if (enemy->posX > enemy->posXPrev) {
            int sideCheckerRPosX = enemy->posX + Enemy::size - 1;
            if (stage.GetObject(sideCheckerRPosX / blockSize, sideCheckerBPosY / blockSize) == Stage::OBJECT::BLOCK) {
              enemy->dir = -1;
            }
          }
          //右から左
          else if (enemy->posX < enemy->posXPrev) {
            int sideCheckerLPosX = enemy->posX;
            if (stage.GetObject(sideCheckerLPosX / blockSize, sideCheckerBPosY / blockSize) == Stage::OBJECT::BLOCK) {
              enemy->dir = 1;
            }
          }
        }
      }

      //---キャラ画像---------------------------------------------
      //画像切り替え判定
      bool swapImg = false;
      if (imgSwapCnt++ > imgSwapInterval) {
        imgSwapCnt = 0;
        swapImg = true;
      }

      //Player
      if (player->dir != player->dirPrev) {
        if (player->dir > 0) {
          player->imgId = 0;
        }
        else {
          player->imgId = 2;
        }
      }
      if ((player->grounded != player->groundedPrev) && player->grounded) {
        if (player->dir > 0) {
          player->imgId = 1;
        }
        else {
          player->imgId = 3;
        }
      }
      if (player->grounded) {
        if (swapImg) {
          if (player->dir > 0) {
            player->imgId = (++player->imgId) % 2;
          }
          else {
            player->imgId = 2 + (++player->imgId) % 2;
          }
        }
      }
      else {
        if (player->dir > 0) {
          player->imgId = 0;
        }
        else {
          player->imgId = 2;
        }
      }

      //Enemy
      for (int i = 0; i < stage.enemyCount; i++) {
        if (enemyList[i].state == 1) {

          if (enemyList[i].dir > 0) {
            enemyList[i].imgId = 0;
          }
          else {
            enemyList[i].imgId = 1;
          }
        }
      }


      //Goal
      if (swapImg) {
        goal->imgId = (++goal->imgId) % 2;
      }

      //---レンダリング----------------------------------------------------------------

      //スクロール
      scrollX = player->posX - spaceSizeXBetweenScrollLineAndPlayer;
      if (scrollX < 0) {
        scrollX = 0;
      }
      if (scrollX + viewportSizeX > stage.Length() * blockSize) {
        scrollX = stage.Length() * blockSize - viewportSizeX;
      }

      scrollY = player->posY - spaceSizeYBetweenScrollLineAndPlayer;
      if (scrollY < 0) {
        scrollY = 0;
      }
      if (scrollY + viewportSizeY > stage.Height() * blockSize) {
        scrollY = stage.Height() * blockSize - viewportSizeY;
      }

      //Stage
      int offsetX = scrollX / blockSize;
      int offsetY = scrollY / blockSize;
      int gapX = scrollX % blockSize;
      int gapY = scrollY % blockSize;
      int sizeX = viewportSizeX / blockSize + 1;
      int sizeY = viewportSizeY / blockSize + 1;
      for (int x = 0; x < sizeX; x++) {
        for (int y = 0; y < sizeY; y++) {
          Stage::OBJECT ob = stage.GetObject(offsetX + x, offsetY + y);
          switch (ob) {
            default:
            case Stage::OBJECT::SPACE:
              glcdCtrl.canvas.color = false;
              glcdCtrl.canvas.Boxf(x * blockSize - gapX, glcdCtrl.canvas.SizeY() - 1 - y * blockSize - (blockSize - 1) + gapY,
                                   x * blockSize + (blockSize - 1) - gapX,  glcdCtrl.canvas.SizeY() - 1 - y * blockSize + gapY);
              break;

            case Stage::OBJECT::BLOCK:
              glcdCtrl.canvas.color = true;
              glcdCtrl.canvas.Boxf(x * blockSize - gapX, glcdCtrl.canvas.SizeY() - 1 - y * blockSize - (blockSize - 1) + gapY,
                                   x * blockSize + (blockSize - 1) - gapX,  glcdCtrl.canvas.SizeY() - 1 - y * blockSize + gapY);
              break;
          }
        }
      }

      //Goal
      if ((goal->posX < scrollXPrev + viewportSizeX)
          && (goal->posX + goal->size > scrollXPrev)
          && (goal->posY < scrollYPrev + viewportSizeY)
          && (goal->posY + goal->size > scrollYPrev)) {

        glcdCtrl.canvas.color = false;
        glcdCtrl.canvas.Pos(goal->posXPrev - scrollXPrev, glcdCtrl.canvas.SizeY() - 1 - goal->posYPrev - (goal->size - 1) + scrollYPrev);
        goal->Celput(goal->imgIdPrev);
      }
      if ((goal->posX < scrollX + viewportSizeX)
          && (goal->posX + goal->size > scrollX)
          && (goal->posY < scrollY + viewportSizeY)
          && (goal->posY + goal->size > scrollY)) {
        glcdCtrl.canvas.color = true;
        glcdCtrl.canvas.Pos(goal->posX - scrollX, glcdCtrl.canvas.SizeY() - 1 - goal->posY - (goal->size - 1) + scrollY);
        goal->Celput(goal->imgId);
      }

      //Enemy
      for (int i = 0; i < stage.enemyCount; i++) {
        if ((enemyList[i].posXPrev < scrollXPrev + viewportSizeX)
            && (enemyList[i].posXPrev + Enemy::size > scrollXPrev)
            && (enemyList[i].posYPrev < scrollYPrev + viewportSizeY)
            && (enemyList[i].posYPrev + Enemy::size > scrollYPrev)) {

          glcdCtrl.canvas.color = false;
          glcdCtrl.canvas.Pos(enemyList[i].posXPrev - scrollXPrev, glcdCtrl.canvas.SizeY() - 1 - enemyList[i].posYPrev - (Enemy::size - 1) + scrollYPrev);
          enemyList[i].Celput(enemyList[i].imgIdPrev);
        }
        if ((enemyList[i].posX < scrollX + viewportSizeX)
            && (enemyList[i].posX + Enemy::size > scrollX)
            && (enemyList[i].posY < scrollY + viewportSizeY)
            && (enemyList[i].posY + Enemy::size > scrollY)) {
          if (enemyList[i].state != 2) {
            glcdCtrl.canvas.color = true;
            glcdCtrl.canvas.Pos(enemyList[i].posX - scrollX, glcdCtrl.canvas.SizeY() - 1 - enemyList[i].posY - (Enemy::size - 1) + scrollY);
            enemyList[i].Celput(enemyList[i].imgId);
          }
          //レンダリングされるときアクティブ
          if (enemyList[i].state == 0) {
            enemyList[i].state = 1;
          }
        }
      }

      //Player
      glcdCtrl.canvas.color = false;
      glcdCtrl.canvas.Pos(player->posXPrev - scrollXPrev, glcdCtrl.canvas.SizeY() - 1 - player->posYPrev - (player->size - 1) + scrollYPrev);
      player->Celput(player->imgIdPrev);

      glcdCtrl.canvas.color = true;
      glcdCtrl.canvas.Pos(player->posX - scrollX, glcdCtrl.canvas.SizeY() - 1 - player->posY - (player->size - 1) + scrollY);
      player->Celput(player->imgId);

      //描画
      glcdCtrl.Draw();

      //---あたり判定-----------------------------------------------------------------
      bool breakGameLoop = false;
      if (player->posY < deadLineY) {
        isGameOver = true;
        breakGameLoop = true;
      }

      if (((player->posX + playerFootBeginX) >= goal->posX && (player->posX + playerFootBeginX) < (goal->posX + goal->size) && player->posY >= goal->posY && player->posY < (goal->posY + goal->size))
          || ((player->posX + playerFootEndX) >= goal->posX && (player->posX + playerFootEndX) < (goal->posX + goal->size) && player->posY >= goal->posY && player->posY < (goal->posY + goal->size))
          || ((player->posX + playerFootEndX) >= goal->posX && (player->posX + playerFootEndX) < (goal->posX + goal->size) && (player->posY + player->size) > goal->posY && (player->posY + player->size) <= (goal->posY + goal->size))
          || ((player->posX + playerFootBeginX) >= goal->posX && (player->posX + playerFootBeginX) < (goal->posX + goal->size) && (player->posY + player->size) > goal->posY && (player->posY + player->size) <= (goal->posY + goal->size))) {
        isGameOver = false;
        breakGameLoop = true;
      }

      for (int i = 0; i < stage.enemyCount; i++) {
        if (enemyList[i].state == 1) {
          if (((player->posX + playerFootBeginX) >= enemyList[i].posX && (player->posX + playerFootBeginX) < (enemyList[i].posX + Enemy::size) && player->posY >= enemyList[i].posY && player->posY < (enemyList[i].posY + Enemy::size))
              || ((player->posX + playerFootEndX) >= enemyList[i].posX && (player->posX + playerFootEndX) < (enemyList[i].posX + Enemy::size) && player->posY >= enemyList[i].posY && player->posY < (enemyList[i].posY + Enemy::size))
              || ((player->posX + playerFootEndX) >= enemyList[i].posX && (player->posX + playerFootEndX) < (enemyList[i].posX + Enemy::size) && (player->posY + player->size) > enemyList[i].posY && (player->posY + player->size) <= (enemyList[i].posY + Enemy::size))
              || ((player->posX + playerFootBeginX) >= enemyList[i].posX && (player->posX + playerFootBeginX) < (enemyList[i].posX + Enemy::size) && (player->posY + player->size) > enemyList[i].posY && (player->posY + player->size) <= (enemyList[i].posY + Enemy::size))) {
            isGameOver = true;
            breakGameLoop = true;
          }

          if (enemyList[i].posY < deadLineY) {
            enemyList[i].state = 2;
          }
        }
      }
      if (breakGameLoop) {
        break;
      }


      //---Prev更新------------------------------------------------------------------
      player->posXPrev = player->posX;
      player->posYPrev = player->posY;
      player->imgIdPrev = player->imgId;
      player->dirPrev = player->dir;
      player->groundedPrev = player->grounded;

      goal->posXPrev = goal->posX;
      goal->posYPrev = goal->posY;
      goal->imgIdPrev = goal->imgId;

      scrollXPrev = scrollX;
      scrollYPrev = scrollY;

      for (int i = 0; i < stage.enemyCount; i++) {
        if (enemyList[i].state == 1) {
          enemyList[i].posXPrev = enemyList[i].posX;
          enemyList[i].posYPrev = enemyList[i].posY;
          enemyList[i].imgIdPrev = enemyList[i].imgId;
          enemyList[i].dirPrev = enemyList[i].dir;
        }
      }
    }

    if (isGameOver) {

      ClearScrBlack();
      glcdCtrl.canvas.Pos(0, 0);
      glcdCtrl.canvas.color = false;

      glcdCtrl.canvas.Mes((str = F("GameOver...")).c_str());
      glcdCtrl.Draw();

      if (stageIndex % 2 == 0) {
        mediaCtrl.Load((str = F("Tasuke.wav")).c_str(), MediaPlayerController::PLAY_MODE::WAIT);
      }
      else {
        mediaCtrl.Load((str = F("Yowa.wav")).c_str(), MediaPlayerController::PLAY_MODE::WAIT);
      }
      mediaCtrl.Play();
      break;
    }
    else {
      ClearScr();
      stageIndex++;
      if (stageIndex < stageCount) {

        glcdCtrl.canvas.Pos(0, 0);
        glcdCtrl.canvas.color = true;

        glcdCtrl.canvas.Mes((str = F("StageClear!")).c_str());
        glcdCtrl.canvas.Pos(0, glcdCtrl.canvas.posY + 10);

        glcdCtrl.canvas.Mes((str = F("NextLevel...")).c_str());
        glcdCtrl.Draw();

        mediaCtrl.Load((str = F("OniB.wav")).c_str(), MediaPlayerController::PLAY_MODE::WAIT);
        mediaCtrl.Play();
      }
      else {
        glcdCtrl.canvas.Pos(0, 0);
        glcdCtrl.canvas.color = true;
        glcdCtrl.canvas.Mes((str = F("GameClear!")).c_str());
        glcdCtrl.Draw();
        stageIndex = stageCount - 1;


        mediaCtrl.Load((str = F("OniA.wav")).c_str(), MediaPlayerController::PLAY_MODE::WAIT);
        mediaCtrl.Play();
        break;
      }
    }
  }
}



void PrintNum(unsigned int num) {
  do {
    glcdCtrl.canvas.PutChar('0' + num % 10);
    num /= 10;
  } while (num != 0);
}

void ClearScr() {
  glcdCtrl.canvas.color = false;
  glcdCtrl.canvas.Boxf(0, 0, glcdCtrl.canvas.SizeX(), glcdCtrl.canvas.SizeY());
  glcdCtrl.Draw();
}
void ClearScrBlack() {
  glcdCtrl.canvas.color = true;
  glcdCtrl.canvas.Boxf(0, 0, glcdCtrl.canvas.SizeX(), glcdCtrl.canvas.SizeY());
  glcdCtrl.Draw();
}



