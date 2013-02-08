#include <stdint.h>
#include <TouchScreen.h> 
#include <TFT.h>
#include <PSTRStrings.h>

// #define DEMO

#define TS_MINX 140
#define TS_MAXX 900

#define TS_MINY 120
#define TS_MAXY 940

#define MAXX 220
#define MAXY 320

#ifdef SEEEDUINO
  #define YP A2   // must be an analog pin, use "An" notation!
  #define XM A1   // must be an analog pin, use "An" notation!
  #define YM 14   // can be a digital pin, this is A0
  #define XP 17   // can be a digital pin, this is A3 
#endif

#define MAXCOMMANDS 20
PSTRStrings commands(MAXCOMMANDS);
bool commandMode = false;
int x = 0;
int y = 0;
int deltax = 100;
int deltay = 100;
int fontsize = 3;
int color;
int radius = 50;
int width = 200;
int height = 60;
int state = 0;
unsigned long touchTimeout = 0;

// For better pressure precision, we need to know the resistance
// between X+ and X- Use any multimeter to read it
// The 2.8" TFT Touch shield has 300 ohms across the X plate
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);  

void setup()
{
  Serial.begin ( 115200);
  Tft.init();  //init TFT library
    
  commands.addString ( PSTR ( "char"));       //  0
  commands.addString ( PSTR ( "fontsize" ));  //  1
  commands.addString ( PSTR ( "deltax"));     //  2
  commands.addString ( PSTR ( "delayy"));     //  3
  commands.addString ( PSTR ( "circle" ));    //  4
  commands.addString ( PSTR ( "line" ));      //  5
  commands.addString ( PSTR ( "rect"));       //  6
  commands.addString ( PSTR ( "CYAN"));       //  7
  commands.addString ( PSTR ( "GREEN"));      //  8
  commands.addString ( PSTR ( "RED"));        //  9
  commands.addString ( PSTR ( "radius"));     // 10
  commands.addString ( PSTR ( "width"));      // 11 
  commands.addString ( PSTR ( "height"));     // 12
  commands.addString ( PSTR ( "BLUE"));       // 13
  commands.addString ( PSTR ( "x" ));         // 14
  commands.addString ( PSTR ( "y" ));         // 15
  commands.addString ( PSTR ( "clear"));      // 16
  commands.addString ( PSTR ( "YELLOW"));     // 17
  // commands.showAll();
  color = GREEN;
  #ifdef DEMO
  state = 1;
  Tft.drawString ( "Hello this is", 10,10,2,CYAN);    
  Tft.drawString ( "a cool TFT   ", 10,34,2,CYAN);    
  Tft.drawString ( "Touchscreen  ", 10,58,2,CYAN);    
  Tft.drawString ( "now available", 10,82,2,CYAN);    
  Tft.drawString ( "on Tindie!   ", 10,106,2,CYAN);    
  Tft.drawString ( "             ", 10,130,2,CYAN);    
  Tft.drawString ( "You can draw ", 10,154,2,CYAN);   
  Tft.drawString ( "graphics and ", 10,178,2,CYAN);   
  Tft.drawString ( "text.        ", 10,202,2,CYAN);   
  Tft.drawRectangle ( 130,220,50,50, CYAN );
  Tft.drawString ( "ok", 140,250,2,GREEN);  
  #endif
}

int readDec (int numDigits)
{
  char ch;
  int total = 0;
  int count = 0;
  bool negative = false;
  while (true)
  {
    if (Serial.available()) // block on user input
    {
      ch = Serial.read();
      
      if (ch == '-') 
        negative = true;
      else 
      {  
        // Exit if number if not a decimal
        if ((ch < '0') || (ch > '9'))
          break;
        count++;  
      }
      
      total *= 10;
      total += ch - '0';
      if (count == numDigits)
         break;
    }  
  }

  if (negative)
    total = 0 - total;
    
  return total;
}

void handleCh (char ch)
{
  int gotCommand = commands.matchString ( ch, false);
  switch (gotCommand)
  {
    case 0:
      commandMode = false;
      break;
    case 1: // fontsize
      fontsize = readDec (1);
      break;  
    case 2: // deltax
      deltax = readDec(3);
      break;
    case 3: // deltay
      deltay = readDec(3);
      break;  
    case 4: // circle
      Tft.drawCircle(x,y,radius,color);
      break;
    case 5: // line
      Tft.drawLine ( x,y,x+deltax,y+deltay,color);
      break;
    case 6: // rect
      Tft.drawRectangle ( x,y,width, height, color );  
    case 7: // CYAN
      color = CYAN;
      break;
    case 8: // GREEN
      color = GREEN;
      break;
    case 9: // RED
      color = RED;  
      break;
    case 10: // radius
      radius = readDec(3);
      break;
    case 11: // width
      width = readDec(3);
      break;
    case 12: // height
      height = readDec(3);
      break;  
    case 13: // BLUE
      color = BLUE;
      break;
    case 14: // x
      x = readDec (3);
      if (x > MAXX)
        x = MAXX;      
      break;
    case 15: // y
      y = readDec (3);
      if (y > MAXY)
         y = MAXY;
      break;  
    case 16: // clear
      Tft.paintScreenBlack();
      break;
    case 17: // YELLOW
      color = YELLOW;
      break;  
    default:
      break;
  }
}

#ifdef DEMO
void doDemo ()
{
  static int demoState = 0;
  static int lastDemoState = 0;
  
  switch (demoState++)
  {
    case 0:
      Tft.paintScreenBlack();
      Tft.drawString ( "The best part", 10,10,2,CYAN);    
      Tft.drawString ( "is that it   ", 10,34,2,CYAN);    
      Tft.drawString ( "has a Serial ", 10,58,2,CYAN);    
      Tft.drawString ( "interface, so", 10,82,2,CYAN);    
      Tft.drawString ( "you can drive", 10,106,2,CYAN);    
      Tft.drawString ( "it from your ", 10,130,2,CYAN);    
      Tft.drawString ( "PC, an Arduino", 10,154,2,CYAN);   
      Tft.drawString ( "even a rasp- ", 10,178,2,CYAN);   
      Tft.drawString ( "berry pi!    ", 10,202,2,CYAN);   
      Tft.drawRectangle ( 150,240,50,50, CYAN );
      Tft.drawString ( "ok", 160,270,2,GREEN);  
    break;
    case 1:
      Tft.paintScreenBlack();
      Tft.drawString ( "Create your  ", 10,10,2,CYAN);    
      Tft.drawString ( "own menus or ", 10,34,2,CYAN);    
      Tft.drawString ( "display debug", 10,58,2,CYAN);    
      Tft.drawString ( "data.        ", 10,82,2,CYAN);    
      Tft.drawString ( "You could use", 10,106,2,CYAN);    
      Tft.drawString ( "it to drive  ", 10,130,2,CYAN);    
      Tft.drawString ( "LEDs or even  ", 10,154,2,CYAN);   
      Tft.drawString ( "a remote      ",10,178,2,CYAN);   
      Tft.drawString ( "control tank.", 10,202,2,CYAN);   
      Tft.drawRectangle ( 150,240,50,50, CYAN );
      Tft.drawString ( "ok", 160,270,2,GREEN);  
    break;
    default:
      Tft.paintScreenBlack();
      Tft.drawString ( "Here are some", 10,10,2,CYAN);    
      Tft.drawString ( "example      ", 10,34,2,CYAN);    
      Tft.drawString ( "graphics     ", 10,58,2,CYAN);
      Tft.drawCircle(100, 130, 50, RED);
      Tft.drawLine ( 10, 180, 200, 200, YELLOW);
      Tft.drawRectangle ( 150,240,50,50, CYAN );
    break;
  }
}
#endif

void loop()
{
  static int x = 0;
  static int y = 0;
  char oneCh[] = " ";
  char ch;
  
  if (Serial.available())
  {
    if (commandMode)
      handleCh (Serial.read());
    else  
    {
      ch = Serial.read();
      if (ch == '^')
        commandMode = true;
      else if (ch >= ' ')
      {
        oneCh[0] = ch;
        Tft.drawString ( oneCh, x,y,fontsize,color );  
        x += fontsize * 6;
        if (x > 230)
        {
          x = 0;
          y = y + fontsize * 8;
        }
      }  
    }
  }  
  
  if (millis() > touchTimeout)
  {
    // a point object holds x y and z coordinates
    Point p = ts.getPoint();

    // we have some minimum pressure we consider 'valid'
    // pressure of 0 means no pressing!
    if (p.z > ts.pressureThreshhold) 
    {
       p.x = map(p.x, TS_MINX, TS_MAXX, 240, 0);
       p.y = map(p.y, TS_MINY, TS_MAXY, 320, 0);
       Serial.print ( "[" );
       if (p.x < 10)
         Serial.print ( "00" );
       else if (p.x < 100)
         Serial.print ( "0");
       Serial.print ( p.x );
       Serial.print ( "," );
       if (p.y < 10)
         Serial.print ( "00" );
       else if (p.y < 100)
         Serial.print ( "0");
       Serial.print ( p.y );
       Serial.println ( "]");
       /*      
         Serial.print("X = "); Serial.print(p.x);
         Serial.print("\tY = "); Serial.print(p.y);
         Serial.print("\tPressure = "); Serial.println(p.z);
       */
       touchTimeout = millis() + 500;
       
#ifdef DEMO
       if (state == 1)
         doDemo();
#endif
    }  
  }
}
