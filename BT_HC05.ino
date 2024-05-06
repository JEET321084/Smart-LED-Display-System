#include <SPI.h>        //SPI.h must be included as DMD is written by SPI (the IDE complains otherwise)
#include <DMD.h>        
#include <TimerOne.h>   
#include <SystemFont5x7.h>
#include <Arial_black_16.h>
#include <SoftwareSerial.h>// import the serial library

SoftwareSerial testserial(2,3); // RX, TX
//Fire up the DMD library as dmd
#define DISPLAYS_ACROSS 2
#define DISPLAYS_DOWN 1
DMD dmd(DISPLAYS_ACROSS, DISPLAYS_DOWN);

#define max_char 200
char message[max_char];    // stores you message
char r_char;               // reads each character
byte index = 0;            // defines the position into your array
int i;            
char greeting[] = "Welcome To The Department of ECE";   //"Welcome to Techno International New Town";

void ScanDMD()
{ 
  dmd.scanDisplayBySPI();
}

void setup(void)
{

   //initialize TimerOne's interrupt/CPU usage used to scan and refresh the display
   Timer1.initialize( 5000 );           //period in microseconds to call ScanDMD. Anything longer than 5000 (5s) and you can see flicker.
   Timer1.attachInterrupt( ScanDMD );   //attach the Timer1 interrupt to ScanDMD which goes to dmd.scanDisplayBySPI()
  
   //clear/init the DMD pixels held in RAM
   dmd.clearScreen( true );   //true is normal (all pixels off), false is negative (all pixels on)
   testserial.begin(9600); //Software serial initialization
   Serial.begin(19200);
strcpy(message,greeting);
}
void loop(void)
{
   //check if serial is avaible an before reading a new message delete's the old message
    
   if(testserial.available())
   {       
        for(i=0; i<199; i++){
            message[i] = '\0';
            Serial.print(message[i]);
        } 
        //resests the index        
        index=0;
    }

    //while is reading the message 
    while(testserial.available() > 0){
       //the message can have up to 200 characters 
       dmd.clearScreen( true );
       if(index < (max_char-1)) 
       {         
           r_char = testserial.read();      // Reads a character
           message[index] = r_char;     // Stores the character in message array
           index++;                     // Increment position      
       } 
   }
    
 //prepares the display to print our message
   dmd.selectFont(Arial_black_16);
   //displays the message
   dmd.drawMarquee(message ,max_char,(32*DISPLAYS_ACROSS)-1 ,0);
   long start=millis();
   long timer=start;
   boolean ret=false;
   while(!ret)
   {       if ((timer+30) < millis()) {
       ret=dmd.stepMarquee(-1,0);
       timer=millis();
     }    } }
