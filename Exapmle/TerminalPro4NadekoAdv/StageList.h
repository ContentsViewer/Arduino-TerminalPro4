/*
*画像データ保存用ヘッダー
* 最終更新日: 2015.11.7
*8bit用:  static const uint8_t imgname[] PROGMEM ={};
*
*16bit用:  static const uint16_t imgname[] PROGMEM ={};
*
*32bit用:  static const uint32_t unsigned long imgname[] PROGMEM = {};
*/
static const uint8_t stageA[] PROGMEM =
{
  1,1,1,1,1,1,1,1,1,1,1,1,
  1,0,0,0,1,0,0,0,0,0,0,1,
  1,2,0,0,1,0,0,0,0,0,0,1,
  1,0,0,0,0,0,0,0,0,0,0,1,
  1,1,0,0,0,0,0,0,0,0,0,1,
  1,1,1,0,0,0,0,0,0,0,0,1,
  1,0,0,1,1,1,1,3,0,0,0,1,
  1,0,0,0,0,0,1,0,0,0,0,1,
  1,0,0,0,0,0,1,0,0,0,0,1,
  1,0,0,0,0,0,0,0,0,0,0,1,
  1,4,0,0,0,0,0,0,1,0,0,0,
  1,0,0,0,0,0,0,0,1,0,0,0,
  1,1,1,1,1,1,1,1,1,0,0,0
};
static const uint8_t stageB[] PROGMEM =
{
  1,1,1,1,1,1,1,1,1,1,1,1,
  1,0,0,0,0,0,0,0,0,0,0,0,
  1,2,0,0,0,0,0,0,0,0,0,0,
  1,0,0,0,0,1,0,0,0,0,0,0,
  1,1,0,0,0,1,0,0,0,0,0,0,
  1,1,1,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,1,0,0,0,
  0,0,0,0,0,0,0,1,0,0,0,0,
  1,1,1,0,0,0,0,1,0,0,0,0,
  1,1,0,0,0,0,0,1,3,0,0,0,
  1,0,0,0,0,0,1,0,0,0,0,0,
  1,0,0,0,0,0,1,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,
  1,0,0,0,0,0,0,0,0,0,0,0,
  1,1,1,0,0,0,0,0,0,0,0,0,
  1,3,1,1,0,0,0,0,0,0,0,0,
  1,0,0,1,1,1,0,0,0,0,0,0,
  1,0,0,0,1,1,0,0,0,0,0,0,
  1,0,0,0,0,1,1,1,4,0,0,0,
  1,0,0,0,0,1,1,1,3,0,0,0,
  1,1,1,1,1,1,1,1,1,1,1,1
};
static const uint8_t stageC[] PROGMEM =
{
  1,1,1,1,1,1,1,1,1,1,1,1,
  1,2,0,0,0,1,1,0,0,1,0,0,
  1,0,0,0,0,1,1,0,0,0,0,0,
  1,1,1,0,0,0,0,0,0,0,0,0,
  0,0,1,1,1,0,0,0,0,0,0,0,
  0,0,0,0,1,1,1,1,1,0,0,1,
  1,0,0,0,0,0,0,0,0,0,0,1,
  0,0,0,0,0,0,0,0,0,0,0,1,
  0,0,0,0,0,0,0,0,0,0,1,3,
  0,0,0,0,0,0,0,1,1,1,1,0,
  0,0,0,0,0,0,1,0,0,0,1,0,
  1,1,1,1,0,0,0,0,1,0,1,0,
  1,3,1,1,0,0,1,1,1,0,1,0,
  1,0,1,1,0,0,1,1,1,0,1,0,
  1,4,0,1,0,0,1,1,1,0,1,0,
  1,0,0,1,0,0,1,1,1,3,1,0,
  1,0,0,1,0,0,1,1,1,3,1,0,
  1,0,0,1,0,0,0,0,0,0,1,0,
  1,0,0,1,0,0,1,1,1,0,0,0,
  1,0,0,1,0,0,1,1,1,1,1,1,
  1,0,0,0,0,0,0,0,0,0,0,0,
  1,1,1,1,1,1,1,1,1,1,1,1
};

static const uint8_t stageD[] PROGMEM ={
  1,1,1,1,1,1,1,1,1,1,1,1,
  1,2,0,0,0,0,0,0,0,0,0,0,
  1,0,0,0,0,0,0,0,0,0,0,0,
  1,0,0,0,0,0,1,3,0,0,0,0,
  1,0,0,0,0,0,1,3,0,0,0,0,
  1,3,0,0,0,0,1,3,0,0,0,0,
  1,3,0,0,0,0,1,3,0,0,0,0,
  1,3,0,0,0,0,0,0,0,0,0,0,
  1,3,0,0,0,0,0,0,0,0,0,0,
  1,3,0,0,0,0,0,0,0,0,0,0,
  1,3,0,0,0,0,0,0,0,0,0,0,
  1,3,0,0,4,0,0,0,0,0,0,0,
  1,3,0,0,0,0,0,0,0,0,0,0,
  1,1,1,1,1,1,1,1,1,1,1,1
};

