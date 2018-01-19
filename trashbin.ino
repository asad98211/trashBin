  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //                                                                         PROJECT TRASH BIN                                                                                    //
  //                                                                       ABU JAFAR MD ASADULLAH                                                                                  //
  //                                                                                                                                                                               //
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <U8glib.h>  
#include <Servo.h>
#include<math.h>
Servo myservo;  
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////// ALL PIN CONFIGURATION//////////////////////////////////////////////////////////////////////////////////////
  const int First_Sonar_Trig=43;
  const int First_Sonar_Echo=42;
  const int Second_Sonar_Trig=41;
  const int Second_Sonar_Echo=40;
  const int Third_Sonar_Trig=45;
  const int Third_Sonar_Echo=44;
  const int LCD_Enable=6;
  const int LCD_RW=5;
  const int LCD_RS=4;
  const int LCD_Reset=7;
  const int Weight_Sensor_Pin=A0;
  const int Servo_Control_Pin=9;
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int servoPosition = 0;
int bottleCounter=0;
int avgSize,avgWeightDiff;
char num_bits [20][8]={"0001101","0011001","0010011","0111101","0100011","0110001","0101111","0111011","0110111","0001011","1110010","1100110","1101100","1000010","1011100","1001110","1010000","1000100","1001000","1110100"};

U8GLIB_ST7920_128X64_1X u8g(LCD_Enable,LCD_RW,LCD_RS ,LCD_Reset); //Enable, RW, RS, RESET

long microsecondsToInches(long microseconds) {
  return microseconds / 74 / 2;
}

long microsecondsToCentimeters(long microseconds) {
  return microseconds / 29 / 2;
}

long sonar(int trig,int echo){
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(5);
  digitalWrite(trig, LOW);
  return  pulseIn(echo, HIGH);
}
void drawBarCode(char *number){
  int i,j,k;
  for(i=0;i<17;i++){
    u8g.drawLine(i, 0, i, 64);
  }
  for(i=0,j=0;i<95;i++){
//    printf("%d",i);
    if(i<3){
       if(i%2==1){
//         printf("a");
         u8g.drawLine(i+17, 0, i+17, 64);
       }
    }
    else if(i>43&&i<50){
      if(i%2==1){
//         printf("c");
        u8g.drawLine(i+17, 0, i+17, 64);
      }
    }
    else if(i>91){
      if(i%2==1){
//         printf("e");
        u8g.drawLine(i+17, 0, i+17, 64);
      }
    }
    else{
      if(i<=43){
        for(k=0;k<7;k++,i++){
          if(num_bits[number[j]-'0'][k]=='0'){
//            printf("b");
              u8g.drawLine(i+17, 0, i+17, 64);
          }
//          else{
//            printf("0");
//          }
        }
        j++;
        i=2+j*7;
      }
      else{
        for(k=0;k<7;k++,i++){
          if(num_bits[number[j]-'0'+10][k]=='0'){
//
//            printf("d");
              u8g.drawLine(i+17, 0, i+17, 64);
          }
//          else{
//            printf("0");
//          }
        }
        j++;
        i=7+j*7;
      }
//      printf("\n");
    }
//    newLine(i);
  }
  for(i=112;i<129;i++){
    u8g.drawLine(i, 0, i, 64);
  }
}


int generateBarcode(int bt_size, int bin_no, int counter, char num_str[13])
{
    sprintf(num_str, "%05d%01d%06d", bin_no, bt_size, counter);
    
    return 0;
}
int getBottleSize(){
  if(avgSize>8&&avgSize<11){
    return 3; //large size bottle
  }
  else if(avgSize>=6&&avgSize<8){
    return 2; //medium size bottle
  }
  else if(avgSize>=4&&avgSize<6){
    return 1; //small size bottle
  }
  return 0;
}
void getCounter(){
  bottleCounter++;
}
void draw(int choice){
  char Barcode[13];
  switch(choice){
  case 1:                             //case 1 does all those steps;
       u8g.sleepOff();//wake up monitor;
       //perky rabbit logo
       u8g.setFont(u8g_font_unifont);
       //u8g.setFont(u8g_font_osb21);
       u8g.drawStr( 0, 32, "Scan The Barcode.");       //code scanning request;
       break;
  case 2:                             //case 2 doest all the steps below it;
      generateBarcode(getBottleSize(),1090,bottleCounter,Barcode);
      drawBarCode(Barcode);
      break;
  case 3:                             //case 3 only greets and sleeps monitor;
       u8g.setFont(u8g_font_unifont);
       //u8g.setFont(u8g_font_osb21);
       u8g.drawStr( 32, 32, "Thank You");      //greetings;
      break;
  default:
      u8g.sleepOn();//sleep;
      break;
  }
}
void displaySomething(int choice){
  u8g.firstPage();  
  do {
    draw(choice);
  } while( u8g.nextPage() );
  
}
void dump_right(){
  for(servoPosition=90;servoPosition>=0;servoPosition-=1){
    myservo.write(servoPosition);
    delay(50);
  }
  delay(100);
  for(servoPosition=0;servoPosition<=90;servoPosition+=1){
      myservo.write(servoPosition);
      delay(50);
  
  }
}
void dump_left(){
  for(servoPosition=90;servoPosition<=180;servoPosition+=1){
    myservo.write(servoPosition);
    delay(50);
  }
  delay(100);
  for(servoPosition=180;servoPosition>=90;servoPosition-=1){
      myservo.write(servoPosition);
      delay(50);
  
  }
}
void binStatus(){
      int tempDistance=microsecondsToCentimeters(sonar(Third_Sonar_Trig,Third_Sonar_Echo));
       Serial.println("Bin Status:");
       Serial.println(tempDistance);
       
       if(tempDistance>45){
          Serial.println("Bin Empty.");  
       }
       else if(tempDistance>15){
          Serial.println("Bin Half Full.");
       }
       else{
          Serial.println("Bin Almost Full.");
       }
       
}
void runSystem(){
    static int prevSonar1=0,prevSonar2=0,prevWeightVolt=0,init=0,detectAndVerifyObject=0;
    int tempDistance1=0,tempDistance2=0,weightVolt=0;
    while(init<=4){
      myservo.write(90);
      if(init==4){
        prevSonar1=tempDistance1/4;
        prevSonar2=tempDistance2/4;
        prevWeightVolt=weightVolt/4;
        init++;
        Serial.println("Initial Values:");
        Serial.println(prevSonar1);
        Serial.println(prevSonar2);
        Serial.println(prevWeightVolt);
        Serial.println();
        continue;
      }
      tempDistance1+=microsecondsToCentimeters(sonar(First_Sonar_Trig,First_Sonar_Echo));
      delay(100);
      tempDistance2+=microsecondsToCentimeters(sonar(Second_Sonar_Trig,Second_Sonar_Echo));
      delay(100);
      weightVolt+=analogRead(Weight_Sensor_Pin);
      init++;
    }
      tempDistance1=microsecondsToCentimeters(sonar(First_Sonar_Trig,First_Sonar_Echo));
      delay(100);
      tempDistance2=microsecondsToCentimeters(sonar(Second_Sonar_Trig,Second_Sonar_Echo));
      delay(100);
      weightVolt=analogRead(Weight_Sensor_Pin);
      avgSize=(abs(prevSonar1-tempDistance1)<abs(prevSonar2-tempDistance2))? abs(prevSonar1-tempDistance1):abs(prevSonar2-tempDistance2);
      /*Serial.println(abs(prevSonar1-tempDistance1));
      Serial.println(abs(prevSonar2-tempDistance2));
      Serial.println(abs(weightVolt-prevWeightVolt));
      Serial.println();
      */
//      binStatus();
      if(avgSize>2){
        detectAndVerifyObject++;
        avgWeightDiff+=abs(weightVolt-prevWeightVolt);
        delay(500);
      }
      else{
        detectAndVerifyObject=0;
        avgWeightDiff=0;
      }
      if(detectAndVerifyObject==4){
        avgWeightDiff=avgWeightDiff/4;
        detectAndVerifyObject=0; 
        Serial.println("Average Weight Diff:");
        Serial.println(avgWeightDiff);
        if(avgWeightDiff>40){
          dump_right();
          displaySomething(3);
          delay(1000);
          displaySomething(0);
        }
        else{
          getCounter();
          dump_left();
          displaySomething(1);
          delay(1000);
          displaySomething(2);
          delay(10000);
          displaySomething(3);
          delay(1000);
          displaySomething(0);
         }
         avgWeightDiff=0;
      }
}
void setup() {
  u8g.setRot180(); // flip screen, if required
  pinMode(First_Sonar_Trig,OUTPUT);
  pinMode(Second_Sonar_Trig,OUTPUT);
  pinMode(First_Sonar_Echo,INPUT);
  pinMode(Second_Sonar_Echo,INPUT);
  myservo.attach(Servo_Control_Pin);
  Serial.begin(9600);
}

void loop() {
  runSystem();
}
