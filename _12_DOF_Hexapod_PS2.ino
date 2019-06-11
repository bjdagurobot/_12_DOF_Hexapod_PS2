/*********For Minidriver Board**************************
  * attention  - to digital pin 6  (simulate SS pin)
    PB2        - PWM_R
  * command    - to digital pin 11 (MOSI pin)
  * data       - to digital pin 12 (MISO pin)
  * clock      - to digital pin 13 (SCK pin)
  __MISO___MOSI_____________________SS________SCK_______
  \ data command X | ground 3.3V attention | clock X X /
   --50-----51----------------------53--------52------
*******************************************************/

//============================================================== DAGU 12 DOF Hexapod sample code =====================================
//                                                                 Written by: Russell Cameron


//#include <IRremote.h>                                      // IRremote library written by Ken Shirriff
//#include <IRremoteInt.h>                                   // the library can be downloaded from here: 
//                                           http://www.arcfn.com/2009/08/multi-protocol-infrared-remote-library.html


//--------------------------- different remotes will generate different values depending on make and model ---------------------------
//----------------------- use the IRtest mode and serial monitor to display the values received by your remote -----------------------

#define IRtest 1                                           // set to 1 for IR test mode
int IRC;
char i=0;


const int slaveSelectPin = 53;
unsigned char PS2buf[10];

#include <Servo.h>                                         // Standard Arduino servo libarary
#include <SPI.h>

//#define Debug 


int svc[12]={                                              // servo center positions (typically 1500uS)
  1500,1500,1500,1500,                                     // D27 knee1, D28 Hip1, D29 knee2, D46 Hip2
  1500,1500,1500,1500,                                     // D47 knee3, D48 Hip3, D49 knee4, D50 Hip4
  1500,1500,1500,1500                                      // D51 knee5, D24 Hip5, D25 knee6, D26 Hip6
};

Servo sv[12];                                              // Yes! servos can be an array

int angle;                                                 // determines the direction/angle (0°-360°) that the robot will walk in 
int rotate;                                                // rotate mode: -1 = anticlockwise, +1 = clockwise, 0 = no rotation
int Speed;                                                 // walking speed: -15 to +15 
int Stride;                                                // size of step: exceeding 400 may cause the legs to hit each other

int RECV_PIN = 10;                                         // define D10 as input for IR receiver
//IRrecv irrecv(RECV_PIN);
//decode_results results;

void setup()
{
  sv[0].attach(37,800,2200);                               // knee 1 
  delay(40);
  sv[1].attach(36,800,2200);                               // Hip  1
  delay(40);
  sv[2].attach(34,800,2200);                               // knee 2
  delay(40);
  sv[3].attach(33,800,2200);                               // Hip  2
  delay(40);
  sv[4].attach(31,800,2200);                               // knee 3
  delay(40);
  sv[5].attach(30,800,2200);                               // Hip  3
  delay(40);
  sv[6].attach(4,800,2200);                               // knee 4
  delay(40);
  sv[7].attach(5,800,2200);                               // Hip  4
  delay(40);
  sv[8].attach(7,800,2200);                               // knee 5
  delay(40);
  sv[9].attach(8,800,2200);                               // Hip  5
  delay(40);
  sv[10].attach(10,800,2200);                              // knee 6
  delay(40);
  sv[11].attach(11,800,2200);                              // Hip  6
  delay(40);
  
  for(int i=0;i<12;i++)
  {
    sv[i].writeMicroseconds(svc[i]);                       // initialize servos
  }
  delay(3000);                                             // optional - gives you time to put the robot down before it starts
  
  //if(IRtest==1) Serial.begin(57600);                        // IR test mode displays IR receiver values on serial monitor
  //irrecv.enableIRIn();         // Start the receiver
  
  Serial.begin(9600);
  initSPI();
}

void loop()
{ 
  
  Get_PS2Dat(PS2buf);
  #ifdef Debug
  for(i=0;i<9;i++)
  {
    Serial.print(PS2buf[i]);
  }
  Serial.println();
  delay(1000);
  #endif
  delay(10);
  
  
  //if (irrecv.decode(&results))                             // check for IR command
  //{                                                        // change IRC comparison values to suit your TV, DVD, Stereo remote
    //int IRC=results.value;
    //if(IRtest==1) Serial.println(IRC);                     // display value from IR receiver on serial monitor in test mode
    
    //  if(!(PS2buf[4] & (~0x7f) )) //按键左
    //  if(!(PS2buf[4] & (~0xdf) )) //按键右
    //  if(!(PS2buf[4] & (~0xef) )) //按键上
    //  if(!(PS2buf[4] & (~0xbf) )) //按键下
    
    //  if(!(PS2buf[3] & (~0xef) )) //方向键上
    //  if(!(PS2buf[3] & (~0xbf) )) //方向键下
    //  if(!(PS2buf[3] & (~0x7f) )) //方向键左
    //  if(!(PS2buf[3] & (~0xdf) )) //方向键右
   
    //  if(!(PS2buf[3] & (~0xfe) )) //SELECT键
    //  if(!(PS2buf[3] & (~0xf7) )) //START键
    
    //  if(!(PS2buf[4] & (~0xfb) )) //L1键
    //  if(!(PS2buf[4] & (~0xfe) )) //L2键
    //  if(!(PS2buf[4] & (~0xf7) )) //R1键
    //  if(!(PS2buf[4] & (~0xfd  )) //R2键
    
    
    if(!(PS2buf[3] & (~0xef) ))                              // FORWARD
    {
      Speed=10;
      rotate=0;
      angle=0;
    }
    else if(!(PS2buf[3] & (~0xbf) ))                             // REVERSE    
    {
      Speed=-10;
      rotate=0;
      angle=0;
    }
    else if(!(PS2buf[3] & (~0xdf) ))                                          // ROTATE CLOCKWISE  
    {
      Speed=10;
      rotate=1;
      angle=0;
    }
    else if(!(PS2buf[3] & (~0x7f) ))                                          // ROTATE COUNTER CLOCKWISE  
    {
      Speed=10;
      rotate=-1;
      angle=0;
    }
    else if(!(PS2buf[4] & (~0x7f) ))                                            // 45 DEGREES    
    {
      Speed=10;
      rotate=0;
      angle=45;
    }
    else if(!(PS2buf[4] & (~0xdf) ))                                          // 90 DEGREES    
    {
      Speed=10;
      rotate=0;
      angle=90;
    }
    else if(!(PS2buf[4] & (~0xef) ))                                          // 135 DEGREES    
    {
      Speed=10;
      rotate=0;
      angle=135;
    }
    else if(!(PS2buf[4] & (~0xbf) ))                                           // 225 DEGREES    
    {
      Speed=10;
      rotate=0;
      angle=225;
    }
    else if(!(PS2buf[4] & (~0xfb) ))                                          // 270 DEGREES    
    {
      Speed=10;
      rotate=0;
      angle=270;
    }
    else if(!(PS2buf[4] & (~0xfe                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   ) ))                                          // 315 DEGREES    
    {
      Speed=10;
      rotate=0;
      angle=315;
    }
    else          // STOP
    {
      Speed=0;
      angle=0;
      rotate=0;
    }
    //irrecv.resume();                                       // receive the next value
  //}
  
  //if(IRtest==1) return;                                    // robot does not walk in IRtest mode
  
  if (angle<0) angle+=360;                                 // keep travel angle within 0°-360°
  if (angle>359) angle-=360;                               // keep travel angle within 0°-360°
  Walk();                                                  // move legs to generate walking gait
  delay(15);
}


void Walk()                                                // all legs move in a circular motion
{
  if(Speed==0)                                             // return all legs to default position when stopped
  {
    Stride-=25;                                            // as Stride aproaches 0, all servos return to center position
    if(Stride<0) Stride=0;                                 // do not allow negative values, this would reverse movements
  }
  else                                                     // this only affects the robot if it was stopped
  {
    Stride+=25;                                            // slowly increase Stride value so that servos start up smoothly
    if(Stride>450) Stride=450;                             // maximum value reached, prevents legs from colliding.
  }
  
  float A;                                                 // temporary value for angle calculations
  double Xa,Knee,Hip;                                      // results of trigometric functions
  static int Step;                                         // position of legs in circular motion from 0° to 360°                               
  
  for(int i=0;i<6;i+=2)                                    // calculate positions for odd numbered legs 1,3,5
  {
    A=float(60*i+angle);                                   // angle of leg on the body + angle of travel
    if(A>359) A-=360;                                      // keep value within 0°-360°
   
    A=A*PI/180;                                            // convert degrees to radians for SIN function
    
    Xa=Stride*rotate;                                      // Xa value for rotation
    if(rotate==0)                                          // hip movement affected by walking angle
    {
      Xa=sin(A)*-Stride;                                   // Xa hip position multiplier for walking at an angle
    }
        
    A=float(Step);                                         // angle of leg
    A=A*PI/180;                                            // convert degrees to radians for SIN function
    Knee=sin(A)*Stride;
    Hip=cos(A)*Xa;
    
    sv[i*2].writeMicroseconds(svc[i*2]+int(Knee));         // update knee  servos 1,3,5
    sv[i*2+1].writeMicroseconds(svc[i*2+1]+int(Hip));      // update hip servos 1,3,5
  }
  
  for(int i=1;i<6;i+=2)                                    // calculate positions for even numbered legs 2,4,6
  {
    A=float(60*i+angle);                                   // angle of leg on the body + angle of travel
    if(A>359) A-=360;                                      // keep value within 0°-360°
   
    A=A*PI/180;                                            // convert degrees to radians for SIN function
    Xa=Stride*rotate;                                      // Xa value for rotation
    if(rotate==0)                                          // hip movement affected by walking angle
    {
      Xa=sin(A)*-Stride;                                   // Xa hip position multiplier for walking at an angle
    }
        
    A=float(Step+180);                                     // angle of leg
    if(A>359) A-=360;                                      // keep value within 0°-360°
    A=A*PI/180;                                            // convert degrees to radians for SIN function
    Knee=sin(A)*Stride;
    Hip=cos(A)*Xa;
    
    sv[i*2].writeMicroseconds(svc[i*2]+int(Knee));         // update knee  servos 2,4,6
    sv[i*2+1].writeMicroseconds(svc[i*2+1]+int(Hip));      // update hip servos 2,4,6
  }
  
  Step+=Speed;                                             // cycle through circular motion of gait
  if (Step>359) Step-=360;                                 // keep value within 0°-360°
  if (Step<0) Step+=360;                                   // keep value within 0°-360°
}


void initSPI()
{
  pinMode(slaveSelectPin,OUTPUT);
  digitalWrite(slaveSelectPin,HIGH);

  SPI.begin();
  SPI.setBitOrder(LSBFIRST);               //LSBFIRST or MSBFIRST先发
  SPI.setDataMode(SPI_MODE3);              //CPOL=1 CPHA=1
  SPI.setClockDivider(SPI_CLOCK_DIV64);    //
  delay(20);
}

//-----SPI，一个字节的读写
unsigned char PS2_RWByte(unsigned char dat)
{
  SPI.transfer(dat);    //command SPDR=dat;
  //while(!(SPSR&0x80));  //读SPSR，判断是否发送/接受完成！
  return(SPDR);         //SPI.transfer(0x00);  //value
}

/*
至少需要20ms的间隔时间访问一次
buf至少需要9个空间
返回0,失败
1：成功，普通模式
2：成功，摇杆扩展模式
*/
unsigned char Get_PS2Dat(unsigned char *buf)
{
  unsigned char i;
  digitalWrite(slaveSelectPin,LOW);
  delayMicroseconds(15);
  buf[0]=PS2_RWByte(0x01);delayMicroseconds(15); //延时再读取
  buf[1]=PS2_RWByte(0x42);delayMicroseconds(15);
  buf[2]=PS2_RWByte(0x00);delayMicroseconds(15);
  buf[3]=PS2_RWByte(0x00);delayMicroseconds(15);
  buf[4]=PS2_RWByte(0x00);delayMicroseconds(15);
  buf[5]=PS2_RWByte(0x00);delayMicroseconds(15);
  buf[6]=PS2_RWByte(0x00);delayMicroseconds(15);
  buf[7]=PS2_RWByte(0x00);delayMicroseconds(15);
  buf[8]=PS2_RWByte(0x00);delayMicroseconds(15);
  digitalWrite(slaveSelectPin,HIGH);
  if((buf[0]==0xff)&&(buf[1]==0x41)&&(buf[2]==0x5a))  //成功，普通模式
  return 1;
  if((buf[0]==0xff)&&(buf[1]==0x73)&&(buf[2]==0x5a))  //成功，摇杆扩展模式
  return 2;
  return 0;
}
