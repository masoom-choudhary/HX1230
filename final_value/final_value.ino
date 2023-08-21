/*
  HX1230 96x68 LCD connections (header on bottom, from left):
  #1 RST - D6 or any digital
  #2 CE  - D7 or any digital
  #3 N/C
  #4 DIN - D11/MOSI 
  #5 CLK - D13/SCK
  #6 VCC - 3V3
  #7 BL  - 3V3 or any digital
  #8 GND - GND
*/

#define LCD_RST 6
#define LCD_CS  7
#define LCD_BL  8

#include "HX1230_FB.h"
#include <SPI.h>
HX1230_FB lcd(LCD_RST, LCD_CS);

#include "bold13x20digtop_font.h"
#include "small4x6_font.h"

int cx,cy;
int sx,sy;
int xs0,ys0,xe0,ye0;
int xs1,ys1,xe1,ye1;
int i;
char buf[20];

// ------------------------------------------------
#define MAXSIN 255
const uint8_t sinTab[800] PROGMEM = {
0,4,8,13,17,22,26,31,35,39,44,48,53,57,61,65,70,74,78,83,87,91,95,99,103,107,111,115,119,123,
127,131,135,138,142,146,149,153,156,160,163,167,170,173,177,180,183,186,189,192,195,198,200,203,206,208,211,213,216,218,
220,223,225,227,229,231,232,234,236,238,239,241,242,243,245,246,247,248,249,250,251,251,252,253,253,254,254,254,254,254,
255
};

int fastSin(int i)
{
  if(i<0) i+=360;
  if(i>=800) i-=360;
  if(i<90)  return(pgm_read_byte(&sinTab[i])); else
  if(i<180) return(pgm_read_byte(&sinTab[180-i])); else
  if(i<270) return(-pgm_read_byte(&sinTab[i-180])); else
            return(-pgm_read_byte(&sinTab[360-i]));
            
}

int fastCos(int i)
{
  return fastSin(i+90);
}

// ------------------------------------------------

void drawGauge1(int level)
{
  lcd.cls();
  cx=96/2;
  cy=68-26;/// main 26
  int rx0=35, ry0=30; /////rx=35..ry0=30
  int rx1=45, ry1=40; ////rx1=45..ry1=40
  int mina=-100;
  int maxa=200+40;

  int barLength = map(level, 0, 800, 0, maxa - mina);  / value change 

 
  for(int i=mina; i<mina +barLength; i+=20) { ////////////////////////////20
    sx = fastCos(i-180);
    sy = fastSin(i-180);
    xs0 = cx+sx*rx0/MAXSIN;
    ys0 = cy+sy*ry0/MAXSIN;
    xe0 = cx+sx*rx1/MAXSIN;
    ye0 = cy+sy*ry1/MAXSIN;
    sx = fastCos(i-180+15);
    sy = fastSin(i-180+15);
    xs1 = cx+sx*rx0/MAXSIN;
    ys1 = cy+sy*ry0/MAXSIN;
    xe1 = cx+sx*rx1/MAXSIN;
    ye1 = cy+sy*ry1/MAXSIN;

      lcd.fillTriangle(xs0,ys0,xe0,ye0,xe1,ye1, 1);
      lcd.fillTriangle(xs1,ys1,xe1,ye1,xs0,ys0, 1);
  
  }
  lcd.setFont(Bold13x20);
  snprintf(buf,10,"%d",level);
  lcd.printStr(ALIGN_CENTER, SCR_HT/2-2,buf);
  lcd.display();


}

unsigned long ms,tm=15000;

void setup(void) 
{
  Serial.begin(9600);
  pinMode(LCD_BL, OUTPUT);
  digitalWrite(LCD_BL, HIGH);
  lcd.init();
  lcd.cls();
  lcd.setFont(Small4x6PL);
  lcd.printStr(ALIGN_CENTER,SCR_HT/2-3," VACUUM GAUGES");
  lcd.display();
  delay(2000);
}

void loop() 
{
  ms=millis();
  while(millis() -ms < tm) {
    for(i=0; i<=800; i+=10) { drawGauge1(i); delay(200); }
    delay(1000);
   
  }


}


