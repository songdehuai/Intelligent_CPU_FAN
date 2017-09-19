#include <IRremote.h>
int pot_pin = 3; //光敏传感器模拟3针脚
int val ; //实时光亮
int daypwm = 200 ;
int nightpwm = 80 ;
int a =30;     //此处需是环境基础亮度变量，请查看自己的亮度数值，
                //填写到此处数值要略大于所测得的数据但小于灯光下的数据
//用a1 a2 a3 a4 a5存储前5个状，防止突变状态频繁跳转                
int a1 = 0;
int a2 = 0;
int a3 = 0;
int a4 = 0;
int a5 = 0;

int RECV_PIN = 11; //定义11针脚接收信息
int FAN_PIN = 9;    //定义5数字针脚为FAN输出
IRrecv irrecv(RECV_PIN); 
decode_results results;
int fulllspeed = 0;





//更改PWM频率降低pwm对电机产生听域内噪音
void setPwmFrequency(int pin, int divisor) {
  byte mode;
  if(pin == 5 || pin == 6 || pin == 9 || pin == 10) {
    switch(divisor) {
      case 1: mode = 0x01; break;
      case 8: mode = 0x02; break;
      case 64: mode = 0x03; break;
      case 256: mode = 0x04; break;
      case 1024: mode = 0x05; break;
      default: return;
    }
    if(pin == 5 || pin == 6) {
      TCCR0B = TCCR0B & 0b11111000 | mode;
    } else {
      TCCR1B = TCCR1B & 0b11111000 | mode;
    }
  } else if(pin == 3 || pin == 11) {
    switch(divisor) {
      case 1: mode = 0x01; break;
      case 8: mode = 0x02; break;
      case 32: mode = 0x03; break;
      case 64: mode = 0x04; break;
      case 128: mode = 0x05; break;
      case 256: mode = 0x06; break;
      case 1024: mode = 0x07; break;
      default: return;
    }
    TCCR2B = TCCR2B & 0b11111000 | mode;
  }
}


void setup ()
{
  Serial.begin(9600);
  irrecv.enableIRIn();
  pinMode(FAN_PIN,OUTPUT);
}


void loop()
{
  int val = analogRead(3);            //读取模拟口A0数值
  delay(200);
  setPwmFrequency(FAN_PIN, 1);



//按钮识别模块
  if (irrecv.decode(&results)) {
    //前风扇控制
    //按“+”号键
    if (results.value == 0xEE886D7F && daypwm < 255) {
      daypwm = daypwm + 5;
      delay(200);
    }
    else if (results.value == 0xFFE21D && daypwm < 255) {
      daypwm = daypwm + 5;
      delay(200);
    }
    //按下“-”号键    
    else if (results.value == 0xE318261B && daypwm > 0) {
      daypwm = daypwm - 5;
      delay(200);
    }
    else if (results.value == 0xFFA25D && daypwm > 0) {
      daypwm = daypwm - 5;
      delay(200);
    }
    //夜间11111111加号
    else if (results.value == 0xA3C8EDDB && nightpwm < 255) {
      nightpwm = nightpwm + 5;
      delay(200);
    }
    else if (results.value == 0xFFA857 && nightpwm < 255) {
      nightpwm = nightpwm + 5;
      delay(200);
    }
    //按下“-”号键    
    else if (results.value == 0xFFE01F && nightpwm > 0) {
      nightpwm = nightpwm - 5;
      delay(200);
    }
    else if (results.value == 0xF076C13B && nightpwm > 0) {
      nightpwm = nightpwm - 5;
      delay(200);
    } //按下全速号键    
    else if (results.value == 0xE5CFBD7F) {
      fulllspeed = 1;
      delay(200);
    }
    else if (results.value == 0xFF906F) {
      fulllspeed = 1;
      delay(200);
    }
  }
  
//夜间识别模块
  a5 = a4;
  a4 = a3;
  a3 = a2;
  a2 = a1;
  if (val < a ){                   //转速调节模块
    a1 = 1;
  }
  else{
    a1 = 0;
  }
  delay(100);



  if ( a1*a2*a3*a4*a5 != 1 )                   //转速调节模块
  {
    if (fulllspeed == 1){
      analogWrite(FAN_PIN,255);
    }
    else{
      analogWrite(FAN_PIN,daypwm);
        Serial.println("day");
    }
  }
  else
  {
    analogWrite(FAN_PIN,nightpwm);
    fulllspeed = 0;
    Serial.println("night");
  }
  Serial.println(results.value,HEX);
  Serial.println(daypwm);
  Serial.println(nightpwm);
  Serial.println(val);
  Serial.println(a1*a2*a3*a4*a5);

  irrecv.resume(); //接收下一个值
  delay(100);

}

