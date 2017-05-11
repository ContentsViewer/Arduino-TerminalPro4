/*
*プログラム
* 最終更新日: 4.28.2016
*
*  説明
*    コントローラーを制御します. このコントローラーは筆者が作ったコントローラー専用です.詳しくは別ファイル参照.
*
*  特徴
*    利点: 最大8個(推奨6個)のボタンの入力を一本のピンで確かめることができます. ボタン同時押しにも対応しています.
*          ボタンが押されている間もそのボタン入力を検出すること(トリガー形式)ができます.
*          また、ボタンが一回押されてそのあとも押されているときはそのボタン入力を検出しないこと（非トリガー形式)ができます.
*          オブジェクト指向でこのプログラムを作ったことにより,コントローラーを簡単に追加できます.
*
*  使用例
*    ボタン検出
*

#include "Controller.h"

//各ボタンの設定
#define UP_BUTTON 8
#define RIGHT_BUTTON 4
#define DOWN_BUTTON 2
#define LEFT_BUTTON 1

Controller ctrl;

void setup() {
Serial.begin(19200);

ctrl.Begin(0, 4, DEFAULT);
}

void loop() {

byte a;

ctrl.Stick(&a, RIGHT_BUTTON + LEFT_BUTTON + UP_BUTTON + DOWN_BUTTON);
if (a & RIGHT_BUTTON) {
Serial.println("RIGHT_BUTTON");
}

if (a & LEFT_BUTTON) {
Serial.println("LEFT_BUTTON");
}

if (a & UP_BUTTON) {
Serial.println("UP_BUTTON");
}

if (a & DOWN_BUTTON) {
Serial.println("DOWN_BUTTON");
}
}

*  更新履歴:
*    12.4.2015:
*      プログラムの完成
*
*    2.3.2016:
*      説明欄更新
*
*    2.12.2016:
*      ボタン8個では使い物にならないことが分かった
*      調整用変数を用意した; この変数を使わないほうがいい,この変数を使うよりボタンの数を減らしたほうがいい
*
*   4.22.2016:
*      スクリプト修正
*
*    4.28.2016:
*      調整用変数廃止,スクリプト修正
*
*  詳しい情報:
*    詳しい情報をほしい方は下記のサイトを参考にしてください.
*      http://kanrec.web.fc2.com/contents/informatics/contents/Arduino/contents/myCNTL/contents/usage_myCNTL/index_usage_myCNTL.html
*        注: リンクが切れていたときはスミマセン.
*
*/

#include "Arduino.h"

class Controller
{
private:
    //スイッチ情報入力ピン
    byte pinInput;

    //スイッチ合計個数
    byte numberOfSwitch;

    //最大デジタル値
    int maxDigitalVal = 1;

    //ひとつ前のボタン情報
    byte keyDataPrev = 0x00;


public:
    //
    //関数 boolean Begin(byte pin, byte numSW, byte mode)
    //  説明:
    //    コントローラーの初期化をします
    //
    //  引数:
    //    byte pin: コントローラからの信号を入力するピン
    //    byte numSW: コントローラに設置されたボタン数
    //    byte mode: analogReferenceで使用します
    //
    //  補足:
    //    コントローラー使用前に一回呼び出す必要があります
    //
    boolean Begin(byte pinInput, byte numberOfSwitch, byte mode)
    {
      
        if (numberOfSwitch > 8 || numberOfSwitch < 0)
        {
            return false;
        }

        byte n;

        this->pinInput = pinInput;
        this->numberOfSwitch = numberOfSwitch;

        switch (mode)
        {
        case EXTERNAL:
            analogReference(EXTERNAL);
            break;

        case INTERNAL:
            analogReference(INTERNAL);
            break;

        default:
        case DEFAULT:
            analogReference(DEFAULT);
            break;
        }

        for (n = 0; n < numberOfSwitch; n++)
        {
            maxDigitalVal *= 2;
        }
        maxDigitalVal--;
        
        return true;
    }


    //
    //関数 byte Read(void)
    //  説明:
    //    コントローラーからボタン情報を取得します
    //
    byte Read(void)
    {
        byte returnData = 0;
        int readData = analogRead(pinInput);

        int deltaX = 1024;
        int deltaY = maxDigitalVal + 1;
        int error = 0;

        for (int x = 0; x <= readData; x++)
        {
            error += deltaY;
            if ((error << 1) >= deltaX)
            {
                returnData++;
                error -= deltaX;
            }
        }
        return returnData;
    }

    //
    //関数 void Stick(byte *keyData, byte nonTrigger)
    //  説明:
    //    コントローラーからボタン情報を取得します
    //
    //  引数:
    //    byte *a: ボタン情報を返します
    //    byte nontri: 非トリガーに設定するボタン番号
    //
    void Stick(byte *keyData, byte nonTrigger)
    {
        byte currentData, tempData;

        currentData = Read();
        tempData = currentData;

        tempData &= nonTrigger;
        tempData &= keyDataPrev;
        tempData = ~(tempData);
        tempData &= currentData;

        *keyData = tempData;
        keyDataPrev = currentData;
    }

};


