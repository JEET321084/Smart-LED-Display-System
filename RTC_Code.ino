#include <Wire.h>
#include <RTClib.h>
#include <SPI.h>
#include <TimerOne.h>
#include <DMD.h>
#include <SystemFont5x7.h>
#include <angka6x13.h>
#include <SoftwareSerial.h> 


double jam_skr;
RTC_DS1307 rtc;
byte SW_0 = 2;  //slide switch
byte SW1 = 3;   //slide switch
byte SW2 = 4;   //Tact switch
byte SW3 = 5;   //Tact switch
int setupMode;
int Button;
boolean mboh;
int timer1 = 1;
char daysOfTheWeek[7][11] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
int  delay_now;
int delay_past;
boolean showMode=0;
DateTime now;
char datToString[2];
DMD dmd(1,1); // DMD controls the entire display

void ScanDMD()
{
  dmd.scanDisplayBySPI();
}

void setup() 
{
  Serial.begin(9600);
  if (! rtc.begin()) 
  {
    // Serial.println("Couldn't find RTC");
    while (1);
  }
  if (! rtc.isrunning()) 
  {
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
  pinMode(SW_0, INPUT_PULLUP);  // Set Time (for this use a slide switch)
  pinMode(SW1, INPUT_PULLUP);  // Set Date (for this use a slide switch)
  pinMode(SW2, INPUT_PULLUP);  // N.O. push button switch
  pinMode(SW3, INPUT_PULLUP);  // N.O. push button switch
  //initialize TimerOne's interrupt/CPU usage used to scan and refresh the display
  Timer1.initialize( 3000 );           //period in microseconds to call ScanDMD. Anything longer than 5000 (5ms) and you can see flicker.
  Timer1.attachInterrupt( ScanDMD );   //attach the Timer1 interrupt to ScanDMD which goes to dmd.scanDisplayBySPI()
}

void loop() 
{
  dmd.selectFont(System5x7);
  if (millis() <= 500) 
  {   
    setingJam();
  }
  Serial.print(delay_now);
  delay_now=millis();
  if (delay_now-delay_past>30000)
  {
    delay_past=delay_now;
    showMode=true;
  }
  if ((showMode) && (digitalRead(SW3))) scrollDate();
  else ShowTimeBig();
}


void drawText( String dispString )
{
  char newString[128];
  int sLength = dispString.length();
  dispString.toCharArray( newString, sLength + 1 );
  dmd.drawMarquee( newString , sLength , 31 , 8);
  long timer = millis();
  boolean ret = false;
  dmd.clearScreen(true);
  NowTimes();
  while ( !ret ) 
  {
    if ( ( timer + 40 ) < millis() )
    {
      NowTimes();
      ShowTime();
      ret = dmd.stepMarquee(8, 15);
      timer = millis();
    }
  }
  dispString="";
  showMode=false;
  delay_past=millis();
  dmd.clearScreen(true);
}

void NowTimes()
{
  now=rtc.now();
}

void Show_TwoDigit(int posX, int posY, double value) //tampil 2 digit( jika kurang dari 10 maka di awali anka 0)
{
  if (value < 10)
  {
    dtostrf(value, 1, 0, datToString);
    dmd.drawString(posX, posY, "0", 1, GRAPHICS_NORMAL);
    dmd.drawString(posX + 7, posY, datToString, 1, GRAPHICS_NORMAL);
  }
  else
  {
    dtostrf(value, 2, 0, datToString);
    dmd.drawString(posX, posY, datToString, 2, GRAPHICS_NORMAL);
  }
}

void scrollDate()
{
  String  text2, textSmntara ;
  NowTimes();

  String twodigit;
  textSmntara="Welcome to Department of ECE.  Today is "; ///u can add  text here 
  text2 += textSmntara;
  text2 += daysOfTheWeek[now.dayOfTheWeek()];
  text2 += " ";
  NowTimes();

  TwoDigit(now.day(),twodigit);
  text2 +=twodigit;
  text2 += "-";
  NowTimes();

  TwoDigit(now.month(),twodigit);
  text2 +=twodigit;
  text2 += "-";
  NowTimes();

  dtostrf(now.year(),4,0,datToString);
  text2 +=datToString;
  drawText (text2);
}
  
void ShowTime() //show small time
{
  NowTimes();
  int det=now.second()%2; 
  if(det!=0)
  dmd.drawString(16,0,":",1,GRAPHICS_NORMAL);  //blink dot
  else
    dmd.drawString(16,0,":",1,GRAPHICS_NOR);
    //if(det==0){
    Show_TwoDigit (2,0,now.hour()); // hsow hour
    Show_TwoDigit (19,0,now.minute());//show minute
    //} 
 }

 void ShowTimeBig()//show big time
{
  String bigDigit="";
  dmd.selectFont(angka6x13);
  NowTimes();
  int j;
  int det=now.second()%2;
  if(det!=0)
  {
    dmd.drawCircle(16,5,1,GRAPHICS_NORMAL); //blink clock dot
    dmd.drawCircle(16,10,1,GRAPHICS_NORMAL);
  } 
  else
  {
    dmd.drawCircle(16,5,1,GRAPHICS_NOR);
    dmd.drawCircle(16,10,1,GRAPHICS_NOR);
  }
  j=now.hour();
  TwoDigit(j,bigDigit);
  bigDigit.toCharArray(datToString, 3); 
  dmd.drawString(1,0,datToString,2,GRAPHICS_NORMAL); //show big hour
  j=now.minute();
  TwoDigit(j,bigDigit);
  bigDigit.toCharArray(datToString, 3); 
  dmd.drawString(19,0,datToString,2,GRAPHICS_NORMAL);//show big minute
  // dmd.selectFont(System5x7);
}

void TwoDigit(int digit, String &hasilDigit) //tampil 2 digit( jika kurang dari 10 maka di awali anka 0)
{
  hasilDigit = "";
  if (digit < 10)
  {
    hasilDigit += "0";
    hasilDigit += digit;
  }
  else
  {
    hasilDigit += digit;
  }
}
 
double moreLess24(double value)   //make sure a value is between 0 and 24
{
  while (value > 24 || value < 0)
  {
    if (value > 24)
      value -= 24;
    else if (value < 0)
      value += 24;
  }
  return value;
}

void doubleToHrMin(double number, int &hours, int &minutes)//convert the double number to Hours and Minutes
{
  hours = floor(moreLess24(number));
  minutes = floor(moreLess24(number - hours) * 60);
}

void seting()
{
  char modesetup[6][9] = {"S-Hrs", "S-Mnt", "S-Dte", "S-Mth", "S-Yar", "S-Day"};
 NowTimes();
  byte sminutes = now.minute();
  byte shours = now.hour();
  byte tgl = now.day();
  byte bln =now.month();
  byte th = now.year()-2000;
  byte hr = now.dayOfTheWeek();
  dmd.clearScreen( true );
  while (setupMode!=0)
  {
    //|| (Button==2)||(Button==3)){}
    delay(500);
    dmd.drawString(0, 0, modesetup[setupMode-1], 8, GRAPHICS_NORMAL);
    ReadButton();
    if (setupMode>6)setupMode=1;
    switch (setupMode)
    {
      case 1:
      {  
        //set jam 
        if (Button==2)
        {
          shours++;          
          if (shours >= 24) shours = 0;
        }
        if (Button==3)
        {
          if (shours<=0)shours=24;
          shours--;          
        }
        dmd.drawString(0, 0, modesetup[setupMode-1], 8, GRAPHICS_NORMAL);
        dmd.drawString(0, 8, "     ", 5, GRAPHICS_NORMAL);
        Show_TwoDigit (0,8,shours);  
      }        
      break;
      
      case 2:
      {     
        //set menit
        if (Button==2)
        {
          sminutes++;          
          if (sminutes >=60) sminutes = 0;
        }
        if (Button==3)
        {
          sminutes--;          
          if (sminutes <=0) sminutes = 59;
        }
      dmd.drawString(0, 0, modesetup[setupMode-1], 8, GRAPHICS_NORMAL);
      Show_TwoDigit (0,8,sminutes);  
      }     
      break;

      case 3:
      {     
        //tanggal
        if (Button==2)
        {
          tgl++;          
          if (tgl >= 31) tgl = 1;
        }
        if (Button==3)
        {
          tgl--;          
          if (tgl == 0) tgl = 31;
        }
        dmd.drawString(0, 0, modesetup[setupMode-1], 8, GRAPHICS_NORMAL);
        Show_TwoDigit (0,8,tgl);      
      }     
      break;
      case 4:
      {                 
        //set bulan 
        if (Button==2)
        {
          bln++;          
          if (bln >= 12) bln = 1;
        }
        if (Button==3)
        {
          bln--; 
          if (bln==0)bln=12;
        }
        dmd.drawString(0, 0, modesetup[setupMode-1], 8, GRAPHICS_NORMAL);
        Show_TwoDigit (0,8,bln);
      }        
      break;

      case 5:
      {     
        //set tahun
        if (Button==2)
        {
          th++;          
          if (th >99) th = 0;
        }
        if (Button==3)
        {
          th--;          
          if (th < 0) th = 99;
        }
        dmd.drawString(0, 0, modesetup[setupMode-1], 8, GRAPHICS_NORMAL);
        Show_TwoDigit (0,8,th);
      }     
      break;
            
      case 6:
      { 
        //set hari
        if (Button==2)
        {
          hr++;          
          if (hr >6) hr = 0;
        }
        if (Button==3)
        {
          hr--;          
          if (hr < 0) hr = 6;
        }
        dmd.drawString(0, 0, modesetup[setupMode-1], 8, GRAPHICS_NORMAL);
        dmd.drawString(0, 8,daysOfTheWeek[hr], 7, GRAPHICS_NORMAL); 
      }     
      break;      
    } 
    while (Button==0)
    {
      ReadButton();
    } //wait for key pressed
    if (Button==1)setupMode++;
    if (Button==4)
    {  
      Wire.beginTransmission(0x68); // activate DS1307
      Wire.write(0); // where to begin
      Wire.write(0x00);          //seconds
      Wire.write((decToBcd(sminutes)));          //minutes
      Wire.write(decToBcd(shours));    //hours (24hr time)
      Wire.write(decToBcd(hr));  // Day 01-07
      Wire.write(decToBcd(tgl));  // Date 0-31
      Wire.write(decToBcd(bln));  // month 0-12
      Wire.write(decToBcd(th));  // Year 00-99
      Wire.write(0x10); // Control 0x10 produces a 1 HZ square wave on pin 7. 
      Wire.endTransmission();
      dmd.clearScreen( true );
      setupMode=0;
      softReset();
    }
  }
}

void setingJam()
{
  ReadButton();
  while ((Button!=0 )|| (setupMode!=0))
  {
    ReadButton();
    if (Button==1)setupMode=1;
    seting();
  }
} 
 

void ReadButton()
{
 
  if ((!(digitalRead(SW1))) && (!(digitalRead(SW2))) )Button=4;
  else if (!(digitalRead(SW_0))) Button=1;  // minus button
  else  if (!(digitalRead(SW1))) Button=2; //  plus button
  else if (!(digitalRead(SW2))) Button=3; // menu button
  else Button=0; 
}

byte decToBcd(byte val) 
{
  return ((val/10*16) + (val%10));
}

void softReset()
{
  asm volatile ("  jmp 0");      
}

   

