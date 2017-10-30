int flag=0;
int r=0;
int g=0;
int b=0;
int fanflag = 0;
#define rp 9
#define gp 10
#define bp 11
int ldr = 0;  
#define ldrled 13 // Analog output pin that the LED is attached to
int bright =0;
int result;
#define relay1 34
#define relay2 35
#define relay3 36
#define relay4 37
#define tempPin A2                          //Temperature
#define fan 12    
#define led 7   
#define gasSensor A3      
int temp;
int tempMin=17;   
int tempMax=27;   
int fanSpeed;
int a=0;
#define panic 52                            //Panic


#define playback 22
#define flame A1
#define Beep 53

#define vcc1 3
#define vcc2 A8
#define vcc3 21
#define vcc8 27
#define vcc4 A10
#define vcc5 40
#define vcc6 42
#define vcc7 44

#define grnd1 4
#define grnd2 A9
#define grnd3 20
#define grnd4 51
#define grnd5 26
#define grnd6 A11
#define grnd7 41
#define grnd8 43
#define grnd9 45
String number ="9968681026"; 
void setup()
{
  Serial.begin(9600);
  Serial1.begin(9600);              
  pinMode(fan,OUTPUT);                //Fan
 pinMode(led,OUTPUT);
  pinMode(tempPin,INPUT);
    pinMode(gasSensor,INPUT);
  pinMode(6,OUTPUT);                  //SmartRoom
  pinMode(5,INPUT);
  pinMode(Beep,OUTPUT) ;            //Flame
  pinMode(flame,INPUT);
  pinMode(panic,INPUT_PULLUP);                //Panic
//  pinMode(alarm, OUTPUT) ;         //Rain
  pinMode(ldr,INPUT);
  pinMode(ldrled,OUTPUT);
//  pinMode(A4,OUTPUT);
  pinMode(rp,OUTPUT);
  pinMode(gp,OUTPUT);
  pinMode(bp,OUTPUT);
  digitalWrite(Beep, LOW);
//pinMode (water, INPUT) ;
  pinMode(playback,OUTPUT);
  pinMode(relay1,OUTPUT);
  pinMode(relay2,OUTPUT);
  pinMode(relay3,OUTPUT);
  pinMode(relay4,OUTPUT);
  pinMode(grnd1,OUTPUT);
  pinMode(grnd2,OUTPUT);
   pinMode(grnd3,OUTPUT);
  pinMode(grnd4,OUTPUT);
    pinMode(grnd5,OUTPUT);
  pinMode(grnd6,OUTPUT);
    pinMode(vcc1,OUTPUT);
  pinMode(vcc2,OUTPUT);
     pinMode(vcc3,OUTPUT);
  pinMode(vcc4,OUTPUT);
  digitalWrite(vcc1,HIGH);
  digitalWrite(vcc2,HIGH);
  digitalWrite(vcc3,HIGH);
  digitalWrite(vcc4,HIGH);
  digitalWrite(vcc5,HIGH);
  digitalWrite(vcc6,HIGH);
  digitalWrite(vcc7,HIGH);
    digitalWrite(vcc8,HIGH);
  digitalWrite(grnd7,LOW);
  digitalWrite(grnd8,LOW);
  digitalWrite(grnd9,LOW);
  digitalWrite(grnd1,LOW);
  digitalWrite(grnd2,LOW);
  digitalWrite(grnd3,LOW);
  digitalWrite(grnd4,LOW);
  digitalWrite(grnd5,LOW);
  digitalWrite(grnd6,LOW);
        
}

void loop()
{
  //rgb_disco(); 
  presenceDetector();  
  fireAlarm();
   if (digitalRead(flame)== HIGH)
   {
    digitalWrite(Beep,HIGH);
    alertPlayback();
   } 

  panicB(); 
  if (fanflag == 1)
  {
    fanControl();
  }
  airCheck();
 
  ldr = analogRead(A0);
  if(Serial1.available())
  {
    result= Serial1.read();
    translateSerial(); 
  }
 
}
 
//FUNCTIONS


void fanControl()
{
  temp = readTemp();     // get the temperature
   if(temp < tempMin)
   {   // if temp is lower than minimum temp
       fanSpeed = 0;      // fan is not spinning
       digitalWrite(fan, LOW);       
   } 
   if((temp >= tempMin) && (temp <= tempMax)) 
   {  // if temperature is higher than minimum temp
       fanSpeed = map(temp, tempMin, tempMax, 32, 255); // the actual speed of fan
       analogWrite(fan, fanSpeed);  // spin the fan at the fanSpeed speed
   } 
   
 if(temp > tempMax) 
   {        // if temp is higher than tempMax
     digitalWrite(fan, HIGH);  // turn on led 
   } 
   else
   {                    // else turn of led
     digitalWrite(fan, LOW); 
   }
   //Serial.println(temp,DEC);   
}
  int readTemp() 
  {  // get the temperature and convert it to celsius
  temp = analogRead(tempPin);
  return temp * 0.48828125;
  }

  void fireAlarm()
 {
   a = analogRead(flame) ;
  // Serial.println (a) ;
   if (a >= 600) // When the analog value is greater than 600 when the buzzer sounds
   {
      digitalWrite(Beep, HIGH);
      digitalWrite(relay1,LOW);
      digitalWrite(relay2,LOW);
      digitalWrite(relay3,LOW);
      digitalWrite(relay4,LOW);
      digitalWrite(fan,LOW);
      alertPlayback();
   } 
   else
   {
      digitalWrite (Beep, LOW);
   }
   
 }
 
  void presenceDetector()
 { 
  digitalWrite(led,digitalRead(5));
 }
 
 void alertPlayback()
   {
     Serial.println("ATD+91"+number+";");
     delay(3000);
     digitalWrite(playback,LOW);
      digitalWrite (Beep, LOW);
     delay(20000);
     digitalWrite(playback,HIGH);
     Serial.println("ATH");
   }
 
 void panicB() 
{
 if(digitalRead(panic)==HIGH)
 {
  digitalWrite(Beep,LOW);
 }
 else
 {
  digitalWrite(Beep,HIGH);
      digitalWrite(relay1,HIGH);
      digitalWrite(relay2,HIGH);
      digitalWrite(relay3,HIGH);
      digitalWrite(relay4,HIGH);
  //delay(60000);
 }
}

void airCheck()
{
  if (analogRead(gasSensor)>110)
  {
      digitalWrite(Beep, HIGH);
      digitalWrite(relay1,LOW);
      digitalWrite(relay2,LOW);
      digitalWrite(relay3,LOW);
      digitalWrite(relay4,LOW);
      digitalWrite(fan,LOW);
    
  }
}
void translateSerial()
{
  if( result=='1') //automatic mode
  {
    if(ldr>230)
    {digitalWrite(ldrled, LOW);
    }
    else
      digitalWrite(ldrled, HIGH);
  }
    

  else if( result=='2') //manual bright
  {
    analogWrite(ldrled, 255);
    
  }
    
 else if( result=='3') //manual on
  {
   analogWrite(ldrled, 180);
  }

  else if( result=='4')
  {
   analogWrite(ldrled, 90); //manual dim
  }

  else if( result=='5')
  {
   digitalWrite(ldrled, LOW); //manual off
  }

  else if(result == '6')
  {
    bright+=50;
    if(bright>256)
    {bright=255;
    }
    analogWrite(ldrled, bright);
  }
   else if(result == '7')
  {
     bright-=50;
    if(bright<0)
    {bright=0;
    }
    analogWrite(ldrled, bright);
  }  

  //...............................................
  if( result=='8')
  {
    r+=50;
    if(r>256)
    {r=255;
    }
    analogWrite(rp,r);
analogWrite(gp,g);
analogWrite(bp,b);
  }

  else if( result=='9')
  {
    r-=50;
    if(r<0)
    {r=0;
    }
    analogWrite(rp,r);
analogWrite(gp,g);
analogWrite(bp,b);
  }

else if( result=='a')
  {
    g+=50;
    if(g>256)
    {g=255;
    }
    analogWrite(rp,r);
analogWrite(gp,g);
analogWrite(bp,b);
  }

  else if( result=='b')
  {
    g-=50;
    if(g<0)
    {g=0;
    }
    analogWrite(rp,r);
analogWrite(gp,g);
analogWrite(bp,b);
  }

   if( result=='c')
  {
    b+=50;
    if(b>256)
    {b=255;
    }
    analogWrite(rp,r);
analogWrite(gp,g);
analogWrite(bp,b);
  }

  else if( result=='d')
  {
    b-=50;
    if(b<0)
    {b=0;
    }
    analogWrite(rp,r);
analogWrite(gp,g);
analogWrite(bp,b);
  }
   else if(result=='g')
   {
    digitalWrite(gp,HIGH);
    digitalWrite(bp,HIGH);
    digitalWrite(rp,HIGH);
  }

   else if(result=='h'){
     digitalWrite(gp,LOW);
    digitalWrite(bp,LOW);
    digitalWrite(rp,LOW);
  }
  
  else if( result=='e')
  {
    flag=1;
    rgb_disco();
  }
  else if( result== 'i')
  {
    digitalWrite(relay1,HIGH);
  }
  else if (result == 'j')
  {
     digitalWrite(relay1,LOW);
  }
    else if( result== 'k')
  {
    digitalWrite(relay2,HIGH);
  }
  else if (result == 'l')
  {
     digitalWrite(relay2,LOW);
  }
    else if( result== 'm')
  {
    digitalWrite(relay3,HIGH);
  }
  else if (result == 'n')
  {
     digitalWrite(relay3,LOW);
  }
    else if( result== 'o')
  {
    digitalWrite(relay4,HIGH);
    fanflag = 1;
  }
  else if (result == 'p')
  {
     digitalWrite(relay4,LOW);
     digitalWrite(fan,LOW);
     fanflag = 0;
  }
    
}

void rgb_disco()
{

  while(flag==1)
  {
    digitalWrite(bp,LOW);
    digitalWrite(rp,HIGH);
    delay(60);
    digitalWrite(rp,LOW);
    digitalWrite(gp,HIGH);
    delay(60);
    digitalWrite(gp,LOW);
    digitalWrite(bp,HIGH);
    delay(60);
    if(Serial1.available())
  {
    result= Serial1.read();
  if(result=='f')
  {
    flag=0;
        digitalWrite(gp,HIGH);
    digitalWrite(bp,HIGH);
    digitalWrite(rp,HIGH);
  }
  }
  }
  }

