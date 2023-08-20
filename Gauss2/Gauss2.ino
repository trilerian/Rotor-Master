#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define SELPIN 10 //Selection Pin 
#define DATAOUT 11//MOSI 
#define DATAIN  12//MISO 
#define SPICLOCK  13//Clock 


// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);


float VCC = 4.9710; // Arduino Voltage
float QOV1 = 2.4793;  // Quiescent Output Sensor1
float QOV2 = 2.4809;  // Quiescent Output Sensor2
float QOV3 = 2.4819;  // Quiescent Output Sensor3
float M1 = -1.4352; // Multiplier Sen1
float M2 = -1.2668; // Multiplier Sen2
float M3 = -1.4447; // Multiplier Sen3
int Offset1 = 2041; // Set raw reading to 0
int Offset2 = 2042; // Set raw reading to 0
int Offset3 = 2044; // Set raw reading to 0
float Sensitivity = 0.0013;
int DEL=0;
float divider = (4095 / VCC); // Compensate for Voltage difference of 5v
int readvalue1; //Raw read from ADC1
int readvalue2; //Raw read from ADC2
int readvalue3; //Raw read from ADC3
int Zero1; // Zero the reading for calculations, allows for +/-
int Zero2; // Zero the reading for calculations, allows for +/-
int Zero3; // Zero the reading for calculations, allows for +/-
int Gauss1; //Calculated Gauss
int Gauss2; //Calculated Gauss
int Gauss3; //Calculated Gauss
int HighGauss1=0; //Variable for Max + Value
int HighGauss2=0; //Variable for Max + Value
int HighGauss3=0; //Variable for Max + Value
int LowGauss1=0; //Variable for Max - Value
int LowGauss2=0; //Variable for Max - Value
int LowGauss3=0; //Variable for Max - Value
float Voltage1; // Voltage of Zero
float Voltage2; // Voltage of Zero
float Voltage3; // Voltage of Zero
float G1; // Gauss before Multiplier
float G2; // Gauss before Multiplier
float G3; // Gauss before Multiplier
float OffsetV1;
float OffsetV2;
float OffsetV3;



void setup() {
  Serial.begin(9600);
   if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    }
  delay(1000);
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println("WMH Racing");
  display.setCursor(0,16);
  display.println("Where the");
  display.setCursor(0,32);
  display.println("Magic");
  display.setCursor(0,48);
  display.println("HAPPENS!!");
  display.display(); 
  pinMode(SELPIN, OUTPUT); 
  pinMode(DATAOUT, OUTPUT); 
  pinMode(DATAIN, INPUT); 
  pinMode(SPICLOCK, OUTPUT); 
  digitalWrite(SELPIN,HIGH); 
  digitalWrite(DATAOUT,LOW); 
  digitalWrite(SPICLOCK,LOW); 
  delay(2000);

}


int read_adc(int channel){
  int adcvalue = 0;
  byte commandbits = B11000000; //command bits - start, mode, chn (3), dont care (3)

  //allow channel selection
  commandbits|=((channel-1)<<3);

  digitalWrite(SELPIN,LOW); //Select adc
  // setup bits to be written
  for (int i=7; i>=3; i--){
    digitalWrite(DATAOUT,commandbits&1<<i);
    //cycle clock
    digitalWrite(SPICLOCK,HIGH);
    digitalWrite(SPICLOCK,LOW);    
  }

  digitalWrite(SPICLOCK,HIGH);    //ignores 2 null bits
  digitalWrite(SPICLOCK,LOW);
  digitalWrite(SPICLOCK,HIGH);  
  digitalWrite(SPICLOCK,LOW);

  //read bits from adc
  for (int i=11; i>=0; i--){
    adcvalue+=digitalRead(DATAIN)<<i;
    //cycle clock
    digitalWrite(SPICLOCK,HIGH);
    digitalWrite(SPICLOCK,LOW);
  }
  digitalWrite(SELPIN, HIGH); //turn off device
  return adcvalue;
}


void loop() {
readvalue1 = read_adc(1); 
readvalue2 = read_adc(2);
readvalue3 = read_adc(3); 
OffsetV1 = (Offset1 - (QOV1 * divider)) / divider; 
OffsetV2 = (Offset2 - (QOV2 * divider)) / divider;
OffsetV3 = (Offset3 - (QOV3 * divider)) / divider;
Voltage1 = (readvalue1) / divider;
Voltage2 = (readvalue2) / divider;
Voltage3 = (readvalue3) / divider;
Zero1 = readvalue1 - Offset1;
Zero2 = readvalue2 - Offset2;
Zero3 = readvalue3 - Offset3;
G1 = ((Voltage1 - OffsetV1)-QOV1) / Sensitivity;
G2 = ((Voltage2 - OffsetV2)-QOV2) / Sensitivity;
G3 = ((Voltage3 - OffsetV3)-QOV3) / Sensitivity;
Gauss1 = G1 * M1;
Gauss2 = G2 * M2;
Gauss3 = G3 * M3;
if (Gauss1 > HighGauss1) HighGauss1 = Gauss1;
if (Gauss2 > HighGauss2) HighGauss2 = Gauss2;
if (Gauss3 > HighGauss3) HighGauss3 = Gauss3;
if (Gauss1 < LowGauss1) LowGauss1 = Gauss1;
if (Gauss2 < LowGauss2) LowGauss2 = Gauss2;
if (Gauss3 < LowGauss3) LowGauss3 = Gauss3;


  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println("WMH Racing");
  display.setCursor(0,16);
  if ((Gauss1 <= 10) && (Gauss1 >= -10)) {
    display.println("0");
  } 
    else
    {
    display.println(Gauss1);
    }

  display.setCursor(64,16);
  if (Gauss1 >= 0) 
   {
      display.print(HighGauss1);
   }
     else
     {
       display.print(LowGauss1);
     }

  display.setCursor(0,32);
  if ((Gauss2 <= 10) && (Gauss2 >= -10)) {
  display.println("0");
  } 
    else
    {
    display.println(Gauss2);
    }

  display.setCursor(64,32);
  if (Gauss2 >= 0) 
    {
     display.print(HighGauss2);
   }
    else
    {
       display.print(LowGauss2);
    }

  display.setCursor(0,48);
  if ((Gauss3 <= 10) && (Gauss3 >= -10)) {
    display.println("0");
  } 
    else
    {
    display.println(Gauss3);
    }

  display.setCursor(64,48);
  if (Gauss3 >= 0) 
    {
      display.print(HighGauss3);
    }
      else
      {
        display.print(LowGauss3);
      }

  display.display();
  
 
 
 Serial.print(readvalue1,DEC); 
 Serial.print(" RV1 ");
 Serial.print((Voltage1),3);
 Serial.print(" V1 ");
 Serial.print(Zero1);
 Serial.print(" ZERO1 ");
 Serial.print(G1);
 Serial.print(" 1st ");
 Serial.print(Gauss1);
 Serial.print(" Gauss1");
 Serial.println();
 Serial.print(readvalue2,DEC); 
 Serial.print(" RV2 ");
 Serial.print((Voltage2),3);
 Serial.print(" V2 ");
 Serial.print(Zero2);
 Serial.print(" ZERO2 ");
 Serial.print(G2);
 Serial.print(" 2nd ");
 Serial.print(Gauss2);
 Serial.print(" Gauss2");
 Serial.println(" "); 
 Serial.print(readvalue3,DEC); 
 Serial.print(" RV3 ");
 Serial.print((Voltage3),3);
 Serial.print(" V3 ");
 Serial.print(Zero3);
 Serial.print(" ZERO3 ");
 Serial.print(G3);
 Serial.print(" 3rd ");
 Serial.print(Gauss3);
 Serial.print(" Gauss3");
 Serial.println(" "); 
 

delay(DEL);

 
}
