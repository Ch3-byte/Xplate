#ifndef ARDUINO_INKPLATE10
#error "Wrong board selection for this example, please select Inkplate 10 in the boards menu."
#endif

#include "Inkplate.h"            //Include Inkplate library to the sketch
#include "SdFat.h"               //Include library for SD card
#include "driver/rtc_io.h" //ESP32 library used for deep sleep and RTC wake up pins


Inkplate display(INKPLATE_3BIT);

SdFile file;                     // Create SdFile object used for accessing files on SD card
String fileName = "InkplateV_" ;
String InkVfiles = "InkplateV_";
String InkHfiles = "InkplateH_";
String format = ".png";

String files = "InkplateV_";
String ofiles = "nuse";

int timer=0;

int fileCountV = 1;
int fileCountH = 1;
int button =2;

#define uS_TO_S_FACTOR 60000000 // Conversion factor for micro seconds to seconds
#define TIME_TO_SLEEP  30      // How long ESP32 will be in deep sleep (in minutes) 86400 és un dia 7200 dos hores
RTC_DATA_ATTR int slide = 0;
// Store int in rtc data, to remain persistent during deep sleep
RTC_DATA_ATTR int bootCount = 0;

// Set clock
uint8_t hour = 12;
uint8_t minutes = 00;
uint8_t seconds = 00;

// Set date and weekday (NOTE: In weekdays 0 means Sunday, 1 menas Monday, ...)
uint8_t weekday = 4;
uint8_t day = 11;
uint8_t month = 11;
uint8_t year = 21;

// Set alarm time and date (alarm will be generated 10 seconds after board power up)
uint8_t alarmHour = 12;
uint8_t alarmMinutes = 00;
uint8_t alarmSeconds = 20;
uint8_t alarmWeekday = 4;
uint8_t alarmDay = 11;



void countVFiles(){
        sd.chdir("/V");
        sd.vwd()->rewind(); 
        while (1){
            fileName = InkVfiles + fileCountV + format;
            Serial.print("File:"); 
            Serial.println(fileName);
            if (sd.exists(fileName.c_str() )) fileCountV++; 
            else break;              
        }
        fileCountV = fileCountV -1;
        Serial.print("Fitxer Verticals: ");
        Serial.println(fileCountV);
}

void countHFiles(){
        sd.chdir("/H");
        sd.vwd()->rewind(); 
        while (1){
            fileName = InkHfiles + fileCountH + format;
            Serial.print("File:"); 
            Serial.println(fileName);
            if (sd.exists(fileName.c_str() )) fileCountH++; 
            else break;              
        }
        
        fileCountH = fileCountH -1;
        Serial.print("Fitxer Horitzontals: ");
        Serial.println(fileCountH);
 
}

void lowPow(){
    
    rtc_gpio_isolate(GPIO_NUM_12); // Isolate/disable GPIO12 on ESP32 (only to reduce power consumption in sleep)
    esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR); // Activate wake-up timer -- wake up after 20s here
    esp_deep_sleep_start();      
}
    
void readTouch(){
 
  
}

void randomV(){

   countVFiles();
  
   display.setRotation(3);
    int nfiles = fileCountV;
    String files = "/V/InkplateV_";
    String ofiles = "nuse";
    String JPG = ".png";
    
  if (display.sdCardInit())
    {
          int i = random(fileCountV);
          display.clearDisplay(); // Clear frame buffer of display
          ofiles = files + i + JPG ;
          Serial.print (ofiles);
          if (!display.drawImage( ofiles.c_str() , 0 ,0 , true, false))
          {
            // If is something failed (wrong filename or wrong bitmap format), write error message on the screen.
            // REMEMBER! You can only use Windows Bitmap file with color depth of 1, 4, 8 or 24 bits with no
            // compression! You can turn of dithering for somewhat faster image load by changing the last 1 to 0, or
            // removing the 1 argument completely
            Serial.println("Image open error");
            display.display();
          }
          display.display();
          delay(100);     
             
    }
  
}

void resetSeconds(){
  uint8_t hour = 12;
  uint8_t minutes = 00;
  uint8_t seconds = 00;
  display.rtcSetTime(hour, minutes, seconds);
}

void randomH(){
    
    countHFiles();

    display.setRotation(2);
    int nfiles = fileCountH;
    String files = "/H/InkplateH_";
    String ofiles = "nuse";
    String JPG = ".png";
    
  if (display.sdCardInit())
    {
          int i = random(fileCountH);
          display.clearDisplay(); // Clear frame buffer of display
          ofiles = files + i + JPG ;
          Serial.print (ofiles);
          if (!display.drawImage( ofiles.c_str() , 0 ,0 , true, false))
          {
            // If is something failed (wrong filename or wrong bitmap format), write error message on the screen.
            // REMEMBER! You can only use Windows Bitmap file with color depth of 1, 4, 8 or 24 bits with no
            // compression! You can turn of dithering for somewhat faster image load by changing the last 1 to 0, or
            // removing the 1 argument completely
            Serial.println("Image open error");
            display.display();
          }
          display.display();
          delay(100);     
             
    }
  
}

/*
 * printejar POWER de bateria, i crono
 * fer botó de sleep
 * Coses a fer: Contar el num d'arxius
 * Carpeta landscape i vertical // 
 * Randomejar display
 * Deep Sleep (inclos mcp low pins...) i restart at 1 minute
 * Internet, Llegir arxius
 * Bluetooth, enviar arxius desde bluetooth
 * Rellotge Realtime check
 * Rename process canvas crop rename // ANDROID
 * Google images retrieve random...
 * 
 * 
 * dels tres butons: 
 *    1) Restart el random i aplicar a les 6am
 *    2) AP/Bluetooth Mode a SD Card
 *    3) 
 * 
 */



void setup()
{
    Serial.begin(115200);
    display.begin();        // Init Inkplate library (you should call this function ONLY ONCE)
    //display.clearDisplay(); // Clear frame buffer of display
    //display.display();      // Put clear image on display
    Serial.println("Setupejant Rellotge");
    display.rtcSetTime(hour, minutes, seconds);    // Send time to RTC
    display.rtcSetDate(weekday, day, month, year); // Send date to RTC
    display.rtcSetAlarm(alarmSeconds, alarmMinutes, alarmHour, alarmDay, alarmWeekday); // Set alarm
    button = 0;
    Serial.print("Collons de boot:");
    Serial.println (bootCount) ;
    ++bootCount;
    
    // Enable wakup from deep sleep on gpio 36
    esp_sleep_enable_ext1_wakeup((1ULL << 36), ESP_EXT1_WAKEUP_ALL_LOW);

    

    // Init SD card. Display if SD card is init propery or not.
    if (display.sdCardInit())
    {
        Serial.println("SD Card ok! Reading data...");
        delay(500);
        //countFiles();
        //while(button = 0){
        //  readTouch();  
        //}
    
    }
    else
    { // If card init was not successful, display error on screen and stop the program (using infinite loop)
        Serial.println("SD Card error!");
        while (true)
        ;
    }
    Serial.print("Esperant que toquis un botó");
}

void loop()
{
    display.rtcGetRtcData();
    seconds = display.rtcGetSecond();  // Store senconds in a variable
    minutes = display.rtcGetMinute();  // Store minutes in a variable
    hour = display.rtcGetHour();       // Store hours in a variable
    day = display.rtcGetDay();         // Store day of month in a variable
    weekday = display.rtcGetWeekday(); // Store day of week in a variable
    month = display.rtcGetMonth();     // Store month in a variable
    year = display.rtcGetYear();       // Store year in a variable


    //Serial.print(timer);
    //Serial.println(timer);
    if (display.readTouchpad(PAD1))
    { // Check if first pad has been touched. If it is, decrement the number and refresh the screen.
        Serial.println("Pad1");
        button=1;
        randomV();
        resetSeconds();

    
    }

    if (display.readTouchpad(PAD2))
    { // If you touched second touchpad, set number to zero and refresh screen by calling our displayNumber() function
        Serial.println("Pad2: Going to sleep");
        lowPow();
    }

    if (display.readTouchpad(PAD3))
    { // If you touched third touchpad, incerement the number and refresh the screen.
        Serial.println("Pad3");
        button=3;
        randomH();
        resetSeconds();

        //displayNumber();
    }

    if (display.rtcCheckAlarmFlag())  // Check if alarm has occurred
    {
      display.rtcClearAlarmFlag();    // It's recommended to clear alarm flag after alarm has occurred
      display.setCursor(400, 400);    // Set new position for cursor
      display.print("ALARM!");
      Serial.println("a dormir");
      lowPow();
    }


}
