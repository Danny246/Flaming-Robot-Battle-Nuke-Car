#include <NeoPixelAnimator.h>
#include <NeoPixelBrightnessBus.h>
#include <NeoPixelBus.h>

#include <Servo.h> //servo library
Servo myservo; // create servo object to control servo

#define colorSaturation 128
const uint16_t pixelCount = 24;
const uint8_t pixelPin = 2;
int pixelBrightness = 20; //scale from 0 to 128

NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> strip(pixelCount,pixelPin);

//set pins for driving stuff
int Echo = A4;  
int Trig = A5; 
int in1 = 6;
int in2 = 7;
int in3 = 8;
int in4 = 9;
int ENA = 5;
int ENB = 11;

//speed out of 255
int ABS = 100;
//distance before it reacts
int distInCM = 40;
//optimising turn time
int tTime = round(50000/ABS);

//globals for making the servo sweep
int servoPos = 0;
boolean up = true;

//store the distances in array
int distance[5];
//misc variables
int backCount = 0;
boolean obst[5];


//main
void setup() 
{ 
  getReady();
  strip.Begin();
  strip.ClearTo(RgbColor(120,120,0));
  strip.Show();
} 
void loop() 
{   
  //lights flash randomly

    for (int i = 0; i<pixelCount; i++)
    {
       strip.Begin();
       int rand1 = random(0,3);
       switch(rand1)
       {
        case 0: strip.SetPixelColor(i, RgbColor(0,random(0,pixelBrightness),random(0,pixelBrightness)));
        break;
        case 1: strip.SetPixelColor(i, RgbColor(random(0,pixelBrightness),0,random(0,pixelBrightness)));
        break;
        case 2: strip.SetPixelColor(i, RgbColor(random(0,pixelBrightness),random(0,pixelBrightness),0));
        break;
       }
       strip.Show();
    }
    //servo does sweep
    sweep();
    //check if distances are too short
    checkSurroundings(distance);
    //performs the cars decisionmaking 
    logic();
    
}


//car moves forward
void mForward()
{
 analogWrite(ENA,ABS);
 analogWrite(ENB,ABS-7);
  digitalWrite(in1,HIGH);//digital output
  digitalWrite(in2,LOW);
  digitalWrite(in3,LOW);
  digitalWrite(in4,HIGH);
 Serial.println("go forward!");
}
//car reverses
void mBack()
{
 analogWrite(ENA,ABS);
 analogWrite(ENB,ABS);
  digitalWrite(in1,LOW);
  digitalWrite(in2,HIGH);
  digitalWrite(in3,HIGH);
  digitalWrite(in4,LOW);
 Serial.println("go back!");
}
//car turns left
void tLeft()
{
 analogWrite(ENA,ABS+80);
 analogWrite(ENB,ABS+80);
  digitalWrite(in1,HIGH);
  digitalWrite(in2,LOW);
  digitalWrite(in3,HIGH);
  digitalWrite(in4,LOW); 
 Serial.println("go left!");
}
//car turns right
void tRight()
{
 analogWrite(ENA,ABS+80);
 analogWrite(ENB,ABS+80);
  digitalWrite(in1,LOW);
  digitalWrite(in2,HIGH);
  digitalWrite(in3,LOW);
  digitalWrite(in4,HIGH);
 Serial.println("go right!");
} 
//car arks left
void arkLeft()
{
  analogWrite(ENA,ABS+100);
  analogWrite(ENB,ABS-87);
  digitalWrite(in1,HIGH);
  digitalWrite(in2,LOW);
  digitalWrite(in3,LOW);
  digitalWrite(in4,HIGH);
  Serial.println("ark left!");
}
//car arks right
void arkRight()
{
  analogWrite(ENA,ABS-80);
  analogWrite(ENB,ABS+103);
  digitalWrite(in1,HIGH);
  digitalWrite(in2,LOW);
  digitalWrite(in3,LOW);
  digitalWrite(in4,HIGH);
  Serial.println("ark right!");
}
//stops car
void stopCar()
{
  analogWrite(ENA,ABS+80);
  analogWrite(ENB,ABS+80);
  digitalWrite(in1,LOW);
  digitalWrite(in2,HIGH);
  digitalWrite(in3,HIGH);
  digitalWrite(in4,LOW);
  delay(100);
  analogWrite(ENA,0);
  analogWrite(ENB,0);
  Serial.println("stopCar!");
} 
//tests distance
int distTest()   
{
  digitalWrite(Trig, LOW);   
  delayMicroseconds(2);
  digitalWrite(Trig, HIGH);  
  delayMicroseconds(20);
  digitalWrite(Trig, LOW);   
  float Fdistance = pulseIn(Echo, HIGH);  
  Fdistance= Fdistance/58;       
  return (int)Fdistance;
}  

//check where objects are and store to global booleans
void checkSurroundings(int distance[])
{
   for (int i =0; i<5;i++)
    {
      if (distance[i] <= distInCM)
      {
        obst[i] = true;
      }
    }
}
//to get everything ready
void getReady()
{
  myservo.attach(3);// attach servo on pin 3 to servo object
  Serial.begin(9600);     
  pinMode(Echo, INPUT);    
  pinMode(Trig, OUTPUT);  
  pinMode(in1,OUTPUT);
  pinMode(in2,OUTPUT);
  pinMode(in3,OUTPUT);
  pinMode(in4,OUTPUT);
  pinMode(ENA,OUTPUT);
  pinMode(ENB,OUTPUT);
  stopCar();

  for (int i = 0; i<5; i++)
  {
   distance[i] = distInCM +3;
   obst[i] = false;
  }
}
//makes servo do a sweep and saves the distances
void sweep()
{
  for(int i = 0; i<89; i++)
    {
      if(servoPos >= 178)
      {
        up = false;
      }
      if(servoPos <= 0)
      {
        up = true;
      }
      if (up)
      {
        servoPos+=2;
      }
      else
      {
        servoPos-=2;
      }
      
      switch (servoPos)
      {
        case 0:distance[0] = distTest();
        break;
        case 44: distance[1] = distTest();
        break;
        case 90: distance[2] = distTest();
        break;
        case 134: distance[3] = distTest();
        break;
        case 178: distance[4] = distTest();
        break;
      }
      myservo.write(servoPos);//setservo position
      delay(5);
      //Serial.println("servoPos is: "+String(servoPos));
    }
}
//does the logic for the cars movement
void logic()
{
  if (obst[2])
    {
      stopCar();
      obst[2] = false;
      if (!obst[0]&&!obst[1])
      {
        backCount=0;
        obst[3],obst[4] = false;
        tRight();
        delay(tTime);
        mForward();
      }
      else if (!obst[3] && !obst[4])
      {
        backCount=0;
        obst[0],obst[1] = false;
        tLeft();
        delay(tTime);
        mForward();
      }
      else
      {
        obst[0],obst[1],obst[3],obst[4] = false;
        backCount++;
        if(backCount>=3)
        {
          tRight();
        delay(tTime);
        mForward();
        backCount = 0;
        }
        else
        {
        mBack();
        delay(800);
        stopCar();
        }
      }
    }
    else if (obst[1])
    {
      backCount = 0;
      obst[1] = false;
      myservo.write(90);
      int newD = distTest();
      if(newD<= distInCM+30)
      {
        tLeft();
        delay(tTime);
        mForward();
      }
      else
      {
        arkLeft();
        delay(400);
        mForward();
      }
    }
    else if(obst[3])
    {
      backCount =0;
      obst[3] = false;
      myservo.write(90);
      int newD = distTest();
      if(newD<= distInCM+30)
      {
        tRight();
        delay(tTime);
        mForward();
      }
      else
      {
        arkRight();
        delay(400);
        mForward();
      }
    }
    else
    {
      backCount=0;
      if(random(0,40) == 7)
      {
        mBack();
        delay(800);
        if(random(0,2) == 1)
        {
          tLeft();
        }
        else
        {
          tRight();
        }
        delay(tTime);
        mForward();
      }
      else
      {
        mForward();
      }
      
    }

    for (int i = 0; i<5; i++)
    {
    obst[i] = false;
    }
}



