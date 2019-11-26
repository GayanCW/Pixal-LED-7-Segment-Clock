#include <Wire.h>
#include "RTClib.h"
#include <FastLED.h>

#define CLOCK_DATA_PIN 2
#define CLOCK_NUM_LEDS 88

CRGB CLOCK_leds[CLOCK_NUM_LEDS];
RTC_DS3231 rtc;

byte end_seconds, end_minutes, end_hours, end_days, end_months; int end_years;
byte now_seconds, now_minutes, now_hours, now_days, now_months; int now_years;
byte seconds, minutes, hours, days;
byte digit_leds=2;
int segment_color=1;
long int unix_TIME;
int hue;

byte segment_arr[8];
byte boolVal_of_decNUM[10][7]={
  {1,1,1,1,1,1,0}, //0
  {0,0,0,1,1,0,0}, //1          //    Segment
  {1,0,1,1,0,1,1}, //2          //     __3__
  {0,0,1,1,1,1,1}, //3          //    |     |
  {0,1,0,1,1,0,1}, //4          //    2     4
  {0,1,1,0,1,1,1}, //5          //    |__7__|
  {1,1,1,0,1,1,1}, //6          //    |     |
  {0,0,1,1,1,0,0}, //7          //    1     5
  {1,1,1,1,1,1,1}, //8          //    |__6__|
  {0,1,1,1,1,1,1}  //9
  };


void setup () {
  
  Serial.begin(9600);
  FastLED.addLeds<WS2811, CLOCK_DATA_PIN, RGB>(CLOCK_leds, CLOCK_NUM_LEDS);
  rtc.begin();
}


void loop () {
    
    //blink_seconds(true);
    dots(1);    
    get_dateTime();
    //print_val(0);   
        set_time(1574793000);
    //print_val(1);
    set_segment();
    digit();
        delay(500);
    //blink_seconds(false);
    dots(0);
        delay(500);
        //delay(1000);
    //dots();
 
}

/*-----------------------------------------------------------------------------------------------*/
/*************************************************************************************************/
/*-----------------------------------------------------------------------------------------------*/

void get_dateTime(){

    DateTime now = rtc.now();
    unix_TIME=now.unixtime();

    now_seconds = now.second();
    now_minutes = now.minute();
    now_hours   = now.hour(); 
    now_days    = now.day();
    now_months  = now.month();
    now_years   = now.year();
}
void set_time(long int setUnix_TIME){  
  unix_TIME = setUnix_TIME - unix_TIME ;
  unix_to_DT_convertor();  
}
void unix_to_DT_convertor(){

    end_seconds = unix_TIME%60; unix_TIME=unix_TIME/60;
    end_minutes = unix_TIME%60; unix_TIME=unix_TIME/60;
    end_hours   = unix_TIME%24; unix_TIME=unix_TIME/24;
    
    end_years=0; int dYear=0;   
    if(unix_TIME%4==0) dYear=366; 
    else dYear=365;  
     while (unix_TIME>=dYear) {
      end_years++;
      unix_TIME-=dYear;
        if (end_years % 4 == 0) dYear = 366;      
        else  dYear = 365;      
     }
     
    end_days=unix_TIME;  end_months = 0;
    int dMonth[12] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};  
     while (end_days > dMonth[end_months+1]){
      end_months++;
      end_days-=dMonth[end_months];
      end_months++;
      end_days++;
     }
      
   if(end_years == 0) end_years=0;
   else end_years=1970+end_years;

}
void set_segment(){
 
  days    = end_days;
  hours   = end_hours;
  minutes = end_minutes;
  seconds = end_seconds; 
  
    segment_arr[0] = days/10;
    segment_arr[1] = days%10;
    
    segment_arr[2] = hours/10;
    segment_arr[3] = hours%10;
    segment_arr[4] = minutes/10;
    segment_arr[5] = minutes%10;
    //segment_arr[0] = seconds/10;
    //segment_arr[1] = seconds%10;
       
    for(byte i=0; i<6; i++){
      Serial.print(segment_arr[i]);
      Serial.print(":"); 
    }
      Serial.print("\t");

      
}
void digit(){
      
  byte segment_index=0;  
  while(segment_index<6){
      byte digit_index=0;
      while(digit_index<7){       
          if( boolVal_of_decNUM[segment_arr[segment_index]][digit_index] == true){
              powerON_digit(segment_index,digit_index);  //segment_color
              Serial.print("1 "); 
          }
          if( boolVal_of_decNUM[segment_arr[segment_index]][digit_index] == false){
              powerOFF_digit(segment_index,digit_index);
              Serial.print("0 ");  
          }
      digit_index++;          
      }
  segment_index++;
  }
  Serial.print(" pass ");
  Serial.println();
}
void powerON_digit(byte segment_NUM,byte digit_NUM){
    
  if(segment_NUM<2)  CLOCK_leds[digit_leds*(7*(segment_NUM)+(digit_NUM))] = CRGB::Red;  
  if(segment_NUM>=2)  CLOCK_leds[digit_leds*(7*(segment_NUM)+(digit_NUM))] = CRGB::Green;
  FastLED.show();
}
void powerOFF_digit(byte segment_NUM,byte digit_NUM){
   
  CLOCK_leds[digit_leds*(7*(segment_NUM)+(digit_NUM))] = CRGB::Black; 
  FastLED.show();
}

void dots(boolean state){

  if(state == 1){
    for(byte led_num=CLOCK_NUM_LEDS-4; led_num<CLOCK_NUM_LEDS-2; led_num++){
      CLOCK_leds[led_num] = CRGB::Lime;  //CHSV(255,255,255);
    }
    for(byte led_num=CLOCK_NUM_LEDS-2; led_num<CLOCK_NUM_LEDS; led_num++){
      CLOCK_leds[led_num] = CRGB::Black;  //CHSV(255,255,255);
    }
    FastLED.show();
  }

  if(state == 0){  
    for(byte led_num=CLOCK_NUM_LEDS-4; led_num<CLOCK_NUM_LEDS-2; led_num++){
      CLOCK_leds[led_num] = CRGB::Black;  //CHSV(255,255,255);
    }
    FastLED.show();
  }
}

void print_val(byte part){

    if(part==0){
        Serial.print(now_years);    Serial.print("/");
        Serial.print(now_months);   Serial.print("/");
        Serial.print(now_days);     Serial.print(" || ");
        Serial.print(now_hours);    Serial.print(":");
        Serial.print(now_minutes);  Serial.print(":");
        Serial.print(now_seconds);  Serial.print("\t");      
    }

    if(part==1){
        Serial.print(end_years);     Serial.print("/");   
        Serial.print(end_months);    Serial.print("/");
        Serial.print(end_days);      Serial.print(" || ");
        Serial.print(end_hours);     Serial.print(":");
        Serial.print(end_minutes);   Serial.print(":");
        Serial.print(end_seconds);   Serial.print(":::");
    }
}

/*-----------------------------------------------------------------------------------------------*/
/*************************************************************************************************/
/*-----------------------------------------------------------------------------------------------*/
