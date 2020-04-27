//
// for ESP8266 
// HC-SR04,IR LED,C1815,10ohm、667ohm
//
// HC-SR04
//  VCC 5V
//  GND GND
//  Echo GPIO14
//  Trig GPIO5
// IR LED GPIO4
// C1815
//  B 3.3V - 667ohm - GPIO4
//  C 10ohm - 5V
//  E GND
    
#include <IRsend.h>
#define SPIN 12

int Echo = 14;  // HC-SR04 Echoピン
int Trig = 5;  // HC-SR04 Trigピン
float dstCMb = 200;   // 一回前の距離値
int count = 0;
unsigned long timed;

const uint16_t kIrLed = 4;  // IRLED GPIO4
IRsend irsend(kIrLed); 

void setup() {
  // シリアル通信を開始
  Serial.begin(115200);

  // HC-SR04ピン設定
  pinMode(Trig, OUTPUT);
  pinMode(Echo, INPUT);
  
  // 赤外線関数初期化
  irsend.begin();
  timed = millis();
}

void loop() {
unsigned long time_tmp;

  // HC-SR04 距離測定
  digitalWrite(Trig, HIGH);
  delay(20); // msec
  digitalWrite(Trig, LOW);

  // HC-SR04 echo
  int intervalUS = pulseIn(Echo, HIGH) / 2;  

  // 時間を距離に変換
  float dstM = (float)intervalUS * 340.0 / 1000.0 / 1000.0;  
  float dstCMs = dstM * 100.0;  /
  float dstCM;

  // ローパスフィルター
  if (dstCMs <= 300.0) {                   
    dstCM = 0.95 * dstCMb + 0.05 * dstCMs; 
    dstCMb = dstCM;                        
  }

  // 1m30cm以下ならカウントダウン
  if ( (dstCMs < 130) and (dstCMs != 0) ) {
    Serial.print("Distance:");
    Serial.print(dstCMs);                
    Serial.print("  count:");
    Serial.println(count);
    if ( count !=0 ){
        tone(SPIN,300 + count*10,100);
        delay(1000);      
    }
    count = count + 1;
    timed = millis();
  } else {
    Serial.print("Distance:");
    Serial.print(dstCMs);                
    Serial.print("  count:");
    Serial.println(count);
    if ( count > 0) {
        time_tmp = millis() - 1000;
        if ( time_tmp > timed ) {
            count = count - 1;  
            timed = millis();
        }
    }
  }

  // TV消灯
  if (count > 5) {
      irsend.sendNEC(0x02FD48B7,32);  
      delay(60000);
      count = 0;
      dstCMb = 200
  }
}
