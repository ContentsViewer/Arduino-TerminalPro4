/*
*画像データ保存用ヘッダー
* 最終更新日: 2015.11.7
*8bit用:  static const prog_uint8_t imgname[] PROGMEM ={};
*
*16bit用:  static const prog_uint16_t imgname[] PROGMEM ={};
*
*32bit用:  static const prog_uint32_t unsigned long imgname[] PROGMEM = {};
*/



//
//playerデータ
//縦16 横13 正味15, 13
//
//
static const uint16_t player[] PROGMEM =
{
  0x1fff,  //0
  0x0ffe,  //1
  0x3c2c,  //2
  0x2ac4,  //3
  0x6d32,  //4
  0x5d62,  //5
  0x5b12,  //6
  0x5b12,  //7
  0x5d63,  //8
  0x6d35,  //9
  0x2acb,  //10
  0x3c3e,  //11
  0x0ffe,  //12
  0x1fff  //13
};

static const uint8_t ebullet_img[] PROGMEM =
{
  0x18,
  0x3c,
  0x7e,
  0x66,
  0x66,
  0xe7,
  0xe7,
  0x18
};

static const uint8_t pbullet_img[] PROGMEM =
{
  0x18,
  0x66,
  0x66,
  0x66,
  0x24,
  0x24,
  0x3c,
  0x18
};


//
//Bossデータ
//縦16 横13 正味15,13
//
static const uint16_t boss[] PROGMEM =
{
  0x33fe,
  0x08fc,
  0x1df0,
  0x0288,
  0x1574,
  0x2afb,
  0x7ef9,
  0x2afb,
  0x1574,
  0x0288,
  0x1df0,
  0x08fc,
  0x33fe
};

/*
static const prog_uint32_t unsigned long lili_img[] PROGMEM =
{
  0x0001e400,  //0
  0x01862600,  //1
  0x3d6cf900,  //2
  0x13164080,  //3
  0x39996040,  //4
  0x7f71803e,  //5
  0x7f4a0024,  //6
  0xe3b42044,  //7
  0xc10c104c,  //8
  0xc169f04c,  //9
  0x8230004a,  //10
  0x826c0092,  //11
  0x820c0092,  //12
  0x82000091,  //13
  0x82036111,  //14
  0x8133e121,  //15
  0x80984321,  //16
  0xc12c0241,  //17
  0xc3e204f9,  //18
  0xf7510cf9,  //19
  0xfe0909f1,  //20
  0x7f07f9e1,  //21
  0x3c39f9f1,  //22
  0x1bc84df1,  //23
  0x1008237d,  //24
  0x20881003,  //25
  0x7f1e1000,  //26
  0x00bb9000,  //27
  0x01c0f000   //28
};
*/
static const uint32_t unsigned long remilia_img[] PROGMEM =
{
  0x00030000,  //0
  0x00028000,  //1
  0x00048000,  //2
  0x00088078,  //3
  0x00108084,  //4
  0x006180fe,  //5
  0x0013007d,  //6
  0x0046e6fd,  //7
  0x002dfffe,  //8
  0x0c7fe7f4,  //9
  0x1e9f9ed2,  //10
  0x19b3ef79,  //11
  0x70d383e9,  //12
  0xb0ef3ae9,  //13
  0xe1af3f69,  //14
  0x731b8379,  //15
  0x31fd8759,  //16
  0x731b8359,  //17
  0xe1af3b79,  //18
  0xf0ef3ee9,  //19
  0x70d383e9,  //20
  0x19b3ef69,  //21
  0x1e9d9ed9,  //22
  0x0c7ceff2,  //23
  0x002dffe4,  //24
  0x0046e7f8,  //25
  0x009300e0,  //26
  0x00618000,  //27
  0x00108000,  //28
  0x00088000,  //29
  0x00048000,  //30
  0x00050000   //31
};
