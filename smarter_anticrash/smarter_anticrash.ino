
#include <Servo.h> //servo library
Servo myservo; // create servo object to control servo
int Echo = A4;  
int Trig = A5; 
int in1 = 6;
int in2 = 7;
int in3 = 8;
int in4 = 9;
int ENA = 5;
int ENB = 11;

int ABS = 100;
int distInCM = 20;
int tTime = round(50000/ABS);

int servoPos = 0;
boolean up = true;
int distance[5];

boolean obst[5];


void _mForward()
{
 analogWrite(ENA,ABS);
 analogWrite(ENB,ABS-8);
  digitalWrite(in1,HIGH);//digital output
  digitalWrite(in2,LOW);
  digitalWrite(in3,LOW);
  digitalWrite(in4,HIGH);
 Serial.println("go forward!");
}

void _mBack()
{
 analogWrite(ENA,ABS);
 analogWrite(ENB,ABS);
  digitalWrite(in1,LOW);
  digitalWrite(in2,HIGH);
  digitalWrite(in3,HIGH);
  digitalWrite(in4,LOW);
 Serial.println("go back!");
}

void _mleft()
{
 analogWrite(ENA,ABS+80);
 analogWrite(ENB,ABS+80);
  digitalWrite(in1,HIGH);
  digitalWrite(in2,LOW);
  digitalWrite(in3,HIGH);
  digitalWrite(in4,LOW); 
 Serial.println("go left!");
}

void _mright()
{
 analogWrite(ENA,ABS+80);
 analogWrite(ENB,ABS+80);
  digitalWrite(in1,LOW);
  digitalWrite(in2,HIGH);
  digitalWrite(in3,LOW);
  digitalWrite(in4,HIGH);
 Serial.println("go right!");
} 

void arkLeft()
{
  analogWrite(ENA,ABS+80);
 analogWrite(ENB,ABS-50);
  digitalWrite(in1,HIGH);
  digitalWrite(in2,LOW);
  digitalWrite(in3,LOW);
  digitalWrite(in4,HIGH);
 Serial.println("ark left!");
}

void arkRight()
{
  analogWrite(ENA,ABS-50);
  analogWrite(ENB,ABS+72);
  digitalWrite(in1,HIGH);
  digitalWrite(in2,LOW);
  digitalWrite(in3,LOW);
  digitalWrite(in4,HIGH);
  Serial.println("ark right!");
}

void _mStop()
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
  Serial.println("Stop!");
} 
 /*Ultrasonic distance measurement Sub function*/
int Distance_test()   
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

void setup() 
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
  _mStop();

  for (int i = 0; i<5; i++)
  {
   distance[i] = distInCM +3;
   obst[i] = false;
  }
} 


int backCount = 0;

void loop() 
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
        case 0:distance[0] = Distance_test();
        break;
        case 44: distance[1] = Distance_test();
        break;
        case 90: distance[2] = Distance_test();
        break;
        case 134: distance[3] = Distance_test();
        break;
        case 178: distance[4] = Distance_test();
        break;
      }
      myservo.write(servoPos);//setservo position
      delay(5);
      //Serial.println("servoPos is: "+String(servoPos));
    }
    
    //check if there are obsstacles
    checkSurroundings(distance);

    for (int i = 0; i<5; i++)
    {
    Serial.print(String(obst[i]));
    }
    
    if (obst[2])
    {
      _mStop();
      obst[2] = false;
      if (!obst[0]&&!obst[1])
      {
        backCount=0;
        obst[3],obst[4] = false;
        _mright();
        delay(tTime);
        _mForward();
      }
      else if (!obst[3] && !obst[4])
      {
        backCount=0;
        obst[0],obst[1] = false;
        _mleft();
        delay(tTime);
        _mForward();
      }
      else
      {
        obst[0],obst[1],obst[3],obst[4] = false;
        backCount++;
        if(backCount>=3)
        {
          _mright();
        delay(tTime);
        _mForward();
        backCount = 0;
        }
        else
        {
        _mBack();
        delay(800);
        _mStop();
        }
      }
    }
    else if (obst[1])
    {
      backCount = 0;
      obst[1] = false;
      myservo.write(90);
      int newD = Distance_test();
      if(newD<= distInCM+50)
      {
        _mleft();
        delay(tTime);
        _mForward();
      }
      else
      {
        arkLeft();
        delay(800);
        _mForward();
      }
    }
    else if(obst[3])
    {
      backCount =0;
      obst[3] = false;
      myservo.write(90);
      int newD = Distance_test();
      if(newD<= distInCM+50)
      {
        _mright();
        delay(tTime);
        _mForward();
      }
      else
      {
        arkRight();
        delay(800);
        _mForward();
      }
    }
    else
    {
      backCount=0;
     _mForward();
    }

    for (int i = 0; i<5; i++)
    {
    obst[i] = false;
    }
}

