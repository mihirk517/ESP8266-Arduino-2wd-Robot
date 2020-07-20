//#include <IRremote.h>
//const int RECV_PIN = 11;
//IRrecv irrecv(RECV_PIN);
//decode_results results;
#include <Ultrasonic.h>
#include <MsTimer2.h>
#include <Servo.h>
#include <SharpIR.h>
#include<Wire.h>
#define slaveAddress 9
#define in1 9   //leftMotor
#define in2 8   //lefttMotor
#define in3 7   //rightMotor
#define in4 4   //rightMotor
#define ENA 6
#define ENB 5
#define dutyCycle 165 // 75% duty cycle
#define SERVO_PIN 10
Servo myservo;
#define trigPin 13
#define echoPin 12
//#define iterations 5
//NewPing sonar(trigPin,echoPin,MAX_DIST);
Ultrasonic ultrasonic(trigPin, echoPin, 40000UL);
SharpIR sensorR( SharpIR::GP2Y0A21YK0F, A1 );
SharpIR sensorL( SharpIR::GP2Y0A21YK0F, A0 );
float Ldist,Rdist,Fdist,FSdist,Fduration,Lduration,Rduration;
int cliffd=0;
enum States {AUTO_FORWARD,AUTO_BOTH_CLIFF,AUTO_RIGHT_CLIFF,AUTO_LEFT_CLIFF,AUTO_REVERSE,AUTO_LEFT,AUTO_RIGHT,BEGIN,CORNER, MANUAL_FORWARD, MANUAL_REVERSE, MANUAL_LEFT, MANUAL_RIGHT, MANUAL_STOP, MANUAL_MODE_OFF, MANUAL_MODE_ON, MANUAL_STANDBY,MANUAL_CLIFF_REV};
States currentState=MANUAL_STANDBY;
volatile bool manualmode = 1;
bool ic2receiver = 1;
volatile int x;
//unsigned long key_value = 0;
int AdutyCycle = dutyCycle;
int BdutyCycle = dutyCycle;
// Constants for Interrupt Pins
const byte MOTOR_A = 3;  // Motor 1 Interrupt Pin - INT 1 - Right Motor
const byte MOTOR_B = 2;  // Motor 2 Interrupt Pin - INT 0 - Left Motor

// Constant for steps in disk
const float stepcount = 20.00;  // 20 Slots in disk

// Constant for wheel diameter
const float wheeldiameter = 66.10; // Wheel diameter in millimeters

// Integers for pulse counters
volatile int counter_A = 0;
volatile int counter_B = 0;

float rotation1;
float rotation2;


////////////////////////////////SETUP_AND_MAIN////////////////////////////////////////


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);   
  //irrecv.enableIRIn();
  //irrecv.blink13(true);
  myservo.attach(SERVO_PIN); 
  myservo.write(105);   
  pinMode(in1,OUTPUT);
  pinMode(in2,OUTPUT);
  pinMode(in3,OUTPUT);
  pinMode(in4,OUTPUT);
  pinMode(ENA,OUTPUT);
  pinMode(ENB,OUTPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  x=9;
  //attachInterrupt(0,CHECK_IR,CHANGE);
  Wire.begin(slaveAddress);
  Wire.onReceive(recieveEvent);
  Serial.println("Slave Ready");
    }

void loop(){
  

  ////MANUAL MODE////

    
  
  
  if (manualmode == 1){
 
        switch(currentState){
         case MANUAL_FORWARD:
          Serial.println("MANUAL FORWARD");
           Serial.println("inloop");          
            if(frontScan() <10)
            {Serial.println("Obstacle ahead");
            reverse(); 
            delay(200);
            Stop();
            currentState=MANUAL_STANDBY;
            break;              
            }
          if (cliff()==1)
            {Serial.println("Cliff R &L");
            reverse(); 
            delay(200);
            Stop();
            currentState=MANUAL_STANDBY;
            break;
            }
          else if(cliff()==2)
            {Serial.println("Cliff R");
            reverse(); 
            delay(200);
            Stop();
            currentState=MANUAL_STANDBY;
            break;
            }
          else if(cliff()==3){
            Serial.println("Cliff L");
            reverse(); 
            delay(200);
            Stop();
            currentState=MANUAL_STANDBY;
            break;
            }
          else{
            Serial.println("Move Forward");
            forward();
            }
            break ;          
          
          case MANUAL_LEFT:
          Serial.println("MANUAL LEFT");
          Serial.println("4");
            if(cliff()==0){
             left();
             delay(600);
             Stop();
             currentState=MANUAL_STANDBY;
             }
             else
             {reverse(); 
            delay(200);
            Stop();
            currentState=MANUAL_STANDBY;
             Serial.println("Cliff L");}
             break ;
          
          case MANUAL_RIGHT:
          Serial.println("MANUAL RIGHT");
          Serial.println("6");
            if(cliff()==0){
             right();
             delay(600);
             Stop();
             currentState=MANUAL_STANDBY;
             }
             else
             {reverse(); 
            delay(200);
            Stop();
            currentState=MANUAL_STANDBY;
             Serial.println("Cliff R");}
             break ;

          case MANUAL_STOP:
          Serial.println("MANUAL STOP");
          Stop();
          currentState=MANUAL_STANDBY;
          break;

          case MANUAL_REVERSE:
          Serial.println("MANUAL REVERSE");
          reverse();
          delay(1000);
          currentState=MANUAL_STANDBY;
          break;
          
//          case MANUAL_CLIFF_REV:
//          Serial.print("Reversing due to cliff");
//          reverse();
//          delay(1000);
//          currentState=MANUAL_STANDBY;
//          break;
          
          case MANUAL_STANDBY:
          Serial.print(".");
          Stop();
          break;
        }
  }


  ////AUTOMATIC MODE////

  if(manualmode == 0){
  
  
  switch(currentState){
     
     case AUTO_FORWARD://no obstacle
     Serial.println("No Obsatcle detected");
     Serial.print("Auto Mode");
     while (1)
     {if (x!=6)
       {
              {//reverse(); 
               //delay(200);
               Stop();
               Serial.println(x);
               currentState=BEGIN;                            
              break;}
       }
      if(frontScan() <15)
     {Serial.println("Obstacle ahead");
     reverse(); 
     delay(200);
     Stop();
     currentState=BEGIN;
     break;              
     }
      if (cliff()==1)
     {Serial.println("R&L");
      reverse(); 
     delay(200);
     Stop();
      currentState=AUTO_BOTH_CLIFF;
     break;           
     }
     else if(cliff()==2)
     {Serial.println("Cliff R");
     reverse(); 
     delay(200);
     Stop();     
     currentState=AUTO_RIGHT_CLIFF;
     break;    
     }
     else if(cliff()==3)
      {Serial.println("Cliff L");
      reverse(); 
     delay(200);
     Stop();
     currentState=AUTO_LEFT_CLIFF;     
     break;    
     }
     if (manualmode ==1){
      Serial.println("Manual ON");
      //reverse(); 
     //delay(200);
     Stop();
     currentState=MANUAL_STANDBY;
     break;
      }
      else      
      Serial.println("No cliff Move Forward");
      Serial.println(frontScan());
      Serial.println("Before forward");
      forward();
      Serial.println("After forward");
      break;
     }
     break;
     
     
     case CORNER:// obstacle
     Serial.println("CORNER detected,Go Left");
     reverse();
     delay(500);
     left();
     delay(500);
     currentState=AUTO_FORWARD;
     break;
     
     case AUTO_RIGHT_CLIFF:
     reverse();
     delay(500);
     left();
     delay(500);
     currentState=AUTO_FORWARD;
     break;
     
     case AUTO_LEFT_CLIFF:
     reverse();
     delay(500);
     right();
     delay(500);    
     currentState=AUTO_FORWARD;
     break;

     case AUTO_RIGHT:
     right();
     delay(500);    
     currentState=AUTO_FORWARD;
     break;
     
     case AUTO_LEFT:
     left();
     delay(500);    
     currentState=AUTO_FORWARD;
     break;
     
     case AUTO_BOTH_CLIFF:
     reverse();
     delay(500);
     right();
     delay(500);
     currentState=AUTO_FORWARD;
     break;
     
     case BEGIN :
     Serial.println("scan");
     Stop();
     sweep();
     analyse();
     break;
      
     default :
     Serial.println("Default Case");
     Stop();
     currentState=BEGIN;
     break;
      
  
}
  }
}

////////////////////////////////FUNCTIONS////////////////////////////////////////


void reverse()
{
 counter_A = 0;  //  reset counter A to zero
  counter_B = 0;  //  reset counter B to zero
  motors(2,2);
  analogWrite(ENA, AdutyCycle);
  analogWrite(ENB, BdutyCycle);
  Motion();
  Serial.println("go reverse!");
}

 void forward() 
 {
  counter_A = 0;  //  reset counter A to zero
  counter_B = 0;  //  reset counter B to zero
  motors(1,1);    
  analogWrite(ENA, AdutyCycle);
  analogWrite(ENB, BdutyCycle);
  Motion();
 Serial.println("go forward!");
}

void left()
{
 counter_A = 0;  //  reset counter A to zero
  counter_B = 0;  //  reset counter B to zero
  motors(1,2);
  analogWrite(ENA, AdutyCycle);
  analogWrite(ENB, BdutyCycle);
  Motion();
  Serial.println("go left!");
}
void right()
{
 counter_A = 0;  //  reset counter A to zero
  counter_B = 0;  //  reset counter B to zero
  motors(2,1);
  analogWrite(ENA, AdutyCycle);
  analogWrite(ENB, BdutyCycle);
  Motion();
  Serial.println("go right!");
} 
void Stop()
{
  digitalWrite(ENA,LOW);
  digitalWrite(ENB,LOW);
  counter_A = 0;  //  reset counter A to zero
  counter_B = 0;  //  reset counter B to zero
  Serial.print(".");
}

void motors(int motor1,int motor2) {
  //Status for Left Motor   
  //Forward
  if(motor1 == 1)   {
    digitalWrite(in1,LOW);    
    digitalWrite(in2,HIGH);     
    }   
  //Reverse
  else if(motor1 == 2)   {
    digitalWrite(in1,HIGH);
    digitalWrite(in2,LOW);    
    }
  
  //Status for Right Motor
  //Forward
  if(motor2 == 1)  {
    digitalWrite(in3,HIGH);
    digitalWrite(in4,LOW);
    }
  //Reverse
  else if(motor2 == 2) {
    digitalWrite(in3,LOW);
    digitalWrite(in4,HIGH);    
    }
  }

void Motion()  {  
  float Nrotation = 0;
  if (AdutyCycle >= 255 || BdutyCycle >= 255) {
    AdutyCycle = dutyCycle;
    BdutyCycle = dutyCycle;
  }
  else if (rotation1 > rotation2)  {
   // error = rotation1 - rotation2;
    BdutyCycle++;
    AdutyCycle--;
    
  }
  else if (rotation2 > rotation1) {
    //error = rotation2 - rotation1;
    AdutyCycle++;
    BdutyCycle--;
  }
}


// Function to convert from centimeters to steps
//int CMtoSteps(float cm) {
//  int result;  // Final calculation result
//  float circumference = (wheeldiameter * 3.14) / 10; // Calculate wheel circumference in cm
//  float cm_step = circumference / stepcount;  // Centimeters per Step
//  
//  float f_result = cm / cm_step;  // Calculate result as a float
//  result = (int) f_result; // Convert to an integer (note this is NOT rounded)
//  
//  return result;  // End and return result
//}







void sweep(){
  myservo.write(105);
  delay(600);
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH); 
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW); 
  Fduration = pulseIn(echoPin, HIGH); 
  Fdist = Fduration / 58;
  Serial.print("Fdist:");
  Serial.println(Fdist);
  delay(600);   
  myservo.write(75);
  delay(600);
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH); 
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW); 
  Rduration = pulseIn(echoPin, HIGH); 
  Rdist = Rduration / 58;
  Serial.print("Rdist:");
  Serial.println(Rdist);
  delay(600) ;
  myservo.write(135); 
  delay(600);
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH); 
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW); 
  Lduration = pulseIn(echoPin, HIGH); 
  Ldist = Lduration / 58;
  Serial.print("Ldist:");
  Serial.println(Ldist);
  delay(600);
  myservo.write(105);
}


float frontScan(){
  myservo.write(105);
  //delay(200);
  FSdist = ultrasonic.read();
  Serial.println("Front Scan Running");
  return FSdist;
    }

 int cliff() {
  int det = 0;
  int stepright = sensorR.getDistance(); // worked out from datasheet graph
  int stepleft = sensorL.getDistance(); // worked out from datasheet graph
  //delay(150); // slow down serial port 
  if(stepright >= 20 && stepleft >= 20)
  {det=1;}
  else if (stepright >= 20){
    Serial.println("cliff detected R");
     det = 2;
  }
  else if (stepleft >= 20){
    Serial.println("cliff detected L"); // print the distance
    det = 3;
  }  
  else {
    det =0;
   Serial.println("No cliff detected");
  }
   return det;
}
int analyse(){
   if (manualmode==1){
  currentState =MANUAL_STANDBY;
   }
  if(Fdist<=10 && Ldist <=10 && Rdist <=10 && manualmode !=1){
  currentState = CORNER;
  }
  else if (Ldist != Rdist && Fdist>=10 && manualmode !=1){
  currentState =AUTO_FORWARD;
  }
  else if ( Fdist <=10 && Ldist > Rdist && manualmode !=1){
  currentState =AUTO_LEFT;
  }
  else if ( Fdist <=10 && Ldist < Rdist && manualmode !=1){
  currentState =AUTO_RIGHT;
  }
 }




///////////////////////////////////////////INTERRUPTS////////////////////////////////////////

// Interrupt Service Routines
// Motor A pulse count ISR
void ISR_countA()  
{
  counter_A++;  // increment Motor A counter value
} 
// Motor B pulse count ISR
void ISR_countB()  
{
  counter_B++;  // increment Motor B counter value
}

// Timertwo ISR
void ISR_timerone()
{
  //Timer1.detachInterrupt();  // Stop the timer
  MsTimer2::stop();
  Serial.print("Motor Speed 1: "); 
  rotation1 = (counter_A / stepcount) * 60.00;  // calculate RPM for Motor 1
  Serial.print(rotation1);  
  Serial.print(" RPM - "); 
  counter_A = 0;  //  reset counter to zero
  Serial.print("Motor Speed 2: "); 
  rotation2 = (counter_B / stepcount) * 60.00;  // calculate RPM for Motor 2
  Serial.print(rotation2);  
  Serial.println(" RPM"); 
  counter_B = 0;  //  reset counter to zero
  //Timer1.attachInterrupt( ISR_timerone );  // Enable the timer
  MsTimer2::start();
}


void recieveEvent(){
  Serial.println("INTERRUPT_I2C");
  if(Wire.available()>0)
  {
    x = Wire.read();
  }
  Serial.print("Recieved Message:");
  Serial.println(x);
  if (x==0){Serial.println("FWD"); currentState=MANUAL_FORWARD;}
  if (x==1){Serial.println("LEFT"); currentState=MANUAL_LEFT;}
  if (x==2){Serial.println("STOP"); currentState=MANUAL_STOP;}
  if (x==3){Serial.println("RIGHT"); currentState=MANUAL_RIGHT;}
  if (x==4){Serial.println("REV"); currentState=MANUAL_REVERSE;}
  if (x==5){Serial.println("ON"); manualmode=1;}
  if (x==6){Serial.println("OFF"); manualmode=0;}
 }
