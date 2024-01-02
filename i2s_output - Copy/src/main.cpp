#include <Arduino.h>
#include "WAVFileReader.h"
#include "SinWaveGenerator.h"
#include "I2SOutput.h"
#include "SD.h"
#include "FS.h"
#include "SPI.h"

#include <FastLED.h>

#include <BluetoothSerial.h>

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run make menuconfig to and enable it
#endif

int incoming;
BluetoothSerial ESP_BT;

// i2s pins
i2s_pin_config_t i2sPins = {
    .bck_io_num = GPIO_NUM_26,
    .ws_io_num = GPIO_NUM_27,
    .data_out_num = GPIO_NUM_25,
    .data_in_num = -1};

I2SOutput *output;
SampleSource *sampleSource1;
SampleSource *noSound;
SampleSource *sampleSource2;
SampleSource *sampleSource3;
SampleSource *sampleSource4;
SampleSource *sampleSource5;
SampleSource *sampleSource6;


const int digit[4] = {15, 4, 16, 22};
const int pins[7] = {2, 17, 21, 32, 13, 0, 33};

int number = 0;
int d = 0;
int wait = 0;
int sec = 0;
int sec1 = 0;
int sec2 = 0;
int minute = 0;
int min1 = 0;
int min2 = 0;

int red=255;
int green = 69;

int timeee = 0;
int sunset = 10800; //sunset time in seconds
bool sunset_mode = false;
bool countdown = true;
int hour_now = 12;
int min_now = 36;
volatile int minute_plus = 0;

//timer for clock
hw_timer_t *Timer0_Cfg = NULL;

int i = 0;
int c_min;
int c_sec;
int c_min1;
int c_sec1;
int c_min2;
int c_sec2;
int c_time = 20000;
int hour1;
int hour2;
int m_sec = 0;
int first_run = 1;
int second_run = 0;
int third_run = 0;
int forth_run = 0;
int sunset_hour = 0;
int sunset_minute = 0;
//bools for turning on and off the components for the app --- general settings
bool diffuser = true;
bool speaker = true;
bool light = true;
int divider = 0;
int current_light = 6;
int new_sunset = 10*3600;
int incom_1;
int incom_2;

 
void IRAM_ATTR Timer0_ISR()
{
  minute_plus++;
  Serial.println(minute_plus);
}

//number check for display
void numbercheck(int d, int number)
{
  for (int i = 0; i < 7; i++){
        digitalWrite(pins[i], HIGH);
      }
      for (int i = 0; i < 4; i++){
        digitalWrite(digit[i], LOW);
      }
for (int i = 0; i < 7; i++){
        digitalWrite(pins[i], HIGH);
    }
    for (int i = 0; i < 4; i++){
        digitalWrite(digit[i], LOW);
    }
 // {6, 19, 0, 3, 8, 5, 1};
   
    if (number == 1){
        digitalWrite(digit[d], HIGH); // Turn the LED on
        digitalWrite(pins[1], LOW); // Turn the LED on
        digitalWrite(pins[2], LOW); // Turn the LED on
         
    } else if( number == 2){
        digitalWrite(digit[d], HIGH); // Turn the LED on
        digitalWrite(pins[0], LOW); // Turn the LED on
        digitalWrite(pins[1], LOW); // Turn the LED on
        digitalWrite(pins[6], LOW); // Turn the LED on
        digitalWrite(pins[4], LOW); // Turn the LED on
        digitalWrite(pins[3], LOW); // Turn the LED on
         
    }else if( number == 3){
        digitalWrite(digit[d], HIGH); // Turn the LED on
        digitalWrite(pins[0], LOW); // Turn the LED on
        digitalWrite(pins[1], LOW); // Turn the LED on
        digitalWrite(pins[2], LOW); // Turn the LED on
        digitalWrite(pins[3], LOW); // Turn the LED on
        digitalWrite(pins[6], LOW); // Turn the LED on
    }else if( number == 4){
        digitalWrite(digit[d], HIGH); // Turn the LED on
        digitalWrite(pins[1], LOW); // Turn the LED on
        digitalWrite(pins[2], LOW); // Turn the LED on
        digitalWrite(pins[5], LOW); // Turn the LED on
        digitalWrite(pins[6], LOW); // Turn the LED on
    }else if( number == 5){
        digitalWrite(digit[d], HIGH); // Turn the LED on
        digitalWrite(pins[0], LOW); // Turn the LED on
        digitalWrite(pins[3], LOW); // Turn the LED on
        digitalWrite(pins[5], LOW); // Turn the LED on
        digitalWrite(pins[6], LOW); // Turn the LED on
        digitalWrite(pins[2], LOW); // Turn the LED on
    }else if( number == 6){
        digitalWrite(digit[d], HIGH); // Turn the LED on
        digitalWrite(pins[0], LOW); // Turn the LED on
        digitalWrite(pins[2], LOW); // Turn the LED on
        digitalWrite(pins[3], LOW); // Turn the LED on
        digitalWrite(pins[4], LOW); // Turn the LED on
        digitalWrite(pins[5], LOW); // Turn the LED on
        digitalWrite(pins[6], LOW); // Turn the LED on
    }else if( number == 7){
        digitalWrite(digit[d], HIGH); // Turn the LED on
        digitalWrite(pins[0], LOW); // Turn the LED on
        digitalWrite(pins[1], LOW); // Turn the LED on
        digitalWrite(pins[2], LOW); // Turn the LED on
        digitalWrite(pins[5], HIGH);
    }else if( number == 8){
        digitalWrite(digit[d], HIGH); // Turn the LED on
        digitalWrite(pins[0], LOW); // Turn the LED on
        digitalWrite(pins[1], LOW); // Turn the LED on
        digitalWrite(pins[2], LOW); // Turn the LED on
        digitalWrite(pins[3], LOW); // Turn the LED on
        digitalWrite(pins[4], LOW); // Turn the LED on
        digitalWrite(pins[5], LOW); // Turn the LED on
        digitalWrite(pins[6], LOW); // Turn the LED on
    }else if( number == 9){
        digitalWrite(digit[d], HIGH); // Turn the LED on
        digitalWrite(pins[0], LOW); // Turn the LED on
        digitalWrite(pins[1], LOW); // Turn the LED on
        digitalWrite(pins[2], LOW); // Turn the LED on
        digitalWrite(pins[3], LOW); // Turn the LED on
        digitalWrite(pins[5], LOW); // Turn the LED on
        digitalWrite(pins[6], LOW); // Turn the LED on
    }else if( number == 0){
        digitalWrite(digit[d], HIGH); // Turn the LED on
        digitalWrite(pins[0], LOW); // Turn the LED on
        digitalWrite(pins[1], LOW); // Turn the LED on
        digitalWrite(pins[2], LOW); // Turn the LED on
        digitalWrite(pins[3], LOW); // Turn the LED on
        digitalWrite(pins[4], LOW); // Turn the LED on
        digitalWrite(pins[5], LOW); // Turn the LED on
    }
  
}


// Things for setting up LED
#define NUM_LEDS 30
#define DATA_PIN 14
#define CLOCK_PIN 12
CRGB leds[NUM_LEDS];

const int txPin = 1; // TX pin (GPIO1)

void setup()
{
  SD.begin();
  Serial.begin(115200);

 
    ESP_BT.begin("SleepyTime"); // Name of your Bluetooth interface -> will show up on your phone
    Serial.println("The device started, now you can pair it with bluetooth!");


   if (!SD.begin(5))
  {
    Serial.println("Card Mount Failed");
    return;
  }else{
    Serial.println("Card Set up");
  }

  Serial.println("Starting up");

  noSound = new WAVFileReader("/sound/sample6.wav");
  sampleSource1 = new WAVFileReader("/sound/Rainy1.wav");
  sampleSource2 = new WAVFileReader("/sound/Rainy2.wav");
  sampleSource3 = new WAVFileReader("/sound/Rainy3.wav");
  if (!sampleSource1){
    Serial.println("failed to create sample source");
  }else{
    Serial.println("Created sample source");
  }
  if (!sampleSource2){
    Serial.println("failed to create sample source");
  }else{
    Serial.println("Created sample source");
  }

  Serial.println("Starting I2S Output");
  output = new I2SOutput();

  //settong up the timer
  
    Timer0_Cfg = timerBegin(0, 80, true);
    timerAttachInterrupt(Timer0_Cfg, &Timer0_ISR, true);
    timerAlarmWrite(Timer0_Cfg, 60000000, true);
    timerAlarmEnable(Timer0_Cfg);

 
  // prersets of the display
  for (int i = 0; i < 7; i++){
        pinMode(pins[i], OUTPUT);
    }
    for (int i = 0; i < 4; i++){
        pinMode(digit[i], OUTPUT);
    }
  
  //output->start(I2S_NUM_1, i2sPins, sampleSource2);

  //Setting up LED
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);  // GRB ordering is assumed

 pinMode(txPin, OUTPUT);

  digitalWrite(txPin, HIGH);

   for(int i = 0; i < 30; i++){
    leds[i] = CRGB(0, 0, 0);
    FastLED.show();
    }

  output->start(I2S_NUM_1, i2sPins, noSound); 

}



void loop()
{
  if(countdown){

    if (ESP_BT.available()) {
    incoming = ESP_BT.read(); //Read what we receive 
    //clock_time = ESP_BT.read(); //Read what we receive 
    Serial.println(incoming);
    //Serial.println(clock_time);

//checking signals from bluetooth
    if(incoming <= 10){
      if (incoming == 5){
        speaker = true;
      }
      if (incoming == 6){
        speaker = false;
      }
      if (incoming == 7){
        light = true;
      }
      if (incoming == 8){
        light = false;
      }
      if (incoming == 9){
        diffuser = false;
      }
      if (incoming == 10){
        diffuser = true;
      }
    }
    incom_1 = incoming/10;
      incom_2 = incoming%10;

      if(incom_1 == 7){
        current_light = 9 - incom_2;
      }
      if(incom_1 == 2){
        if(incom_2 == 1){
          new_sunset = new_sunset + 3600;
        }
        if(incom_2 == 2){
          new_sunset = new_sunset - 3600;
          if(new_sunset < 18000){
            new_sunset = 18000;
          }
          if(new_sunset > 54000){
            new_sunset = 54000;
          }
        }
        sunset = new_sunset;
      }
  }

    c_min = c_time/60000;
    c_sec = c_time/1000;

    c_sec1 = c_sec%10;
    c_sec2 = c_sec/10;
    c_min1 = c_min%10;
    c_min2 = c_min/10;

    numbercheck(0, hour2);
    delay(1);

    numbercheck(1, hour1);
    delay(1);

    numbercheck(2, min2);
    delay(1);

    numbercheck(3, min1);
    delay(1); 

    c_time = c_time - 4;

    if( c_time < 0){
      countdown = false;
      sunset_mode = true;

      for (int i = 0; i < 7; i++){
        digitalWrite(pins[i], HIGH);
      }
      for (int i = 0; i < 4; i++){
        digitalWrite(digit[i], LOW);
      }
      int red=255;
      int green = 69;
      divider = 0;
    }
    sunset = new_sunset;
  }else if(sunset_mode || (sunset_hour == hour_now && sunset_minute == min_now)){
     for(int i = 0; i < 30; i++){
          leds[i] = CRGB(red, green, 0);
          FastLED.show();
     }

if (wait%5000 == 0){
    timeee = timeee + 5;
    //Serial.println(timeee);
    if(timeee == 5){
      red = 255 - 4*current_light;
      green = 69 -current_light;
    }
    if (timeee == sunset/2){
      second_run++;
    }
    if (timeee == sunset/10*3){
      third_run++;
    }

    if(timeee == (sunset/30*divider)){
          red = red - 8;
          green = green - 2; 

          if(divider%2 == 0){
            red = red - 1;
          }
          if(divider%3 == 0){
            green = green - 1;
          }
          if(green<0){
            green = 0;
          }
          if(red<0){
            red = 0;
          }
          for(int i = 0; i < 30; i++){
          leds[i] = CRGB(red, green, 0);
         
          FastLED.show();
        }
          divider++;
          if(divider == 31){
            divider = 30;
          }
      }
    if(timeee == sunset){
        red = 0;
        green = 0;
        sunset_mode = false;
        countdown = true;
        for(int i = 0; i < 30; i++){
          leds[i] = CRGB(red, green, 0);
          FastLED.show();
        }
         digitalWrite(txPin, LOW);
        output->start(I2S_NUM_1, i2sPins, noSound);
        delay(4000); // sound quality decent with 4000
    }

    if(diffuser){
      if(timeee%1000 == 0){
        digitalWrite(txPin, LOW);
      }else if(timeee%500 == 0){
        digitalWrite(txPin, HIGH);
      }
    }
    

    if (sec == 60){
      minute++;
      sec = 0;
      if ( minute == 60){
        minute = 0;
      }
     for(int i = 0; i < 30; i++){
    leds[i] = CRGB(red, green, 0);
    FastLED.show();

     
    }}} 
    if(!light){
      red = 0;
      green = 0;
    }

    wait = wait + 5000;
    if(speaker){
      if(first_run == 1){
        output->start(I2S_NUM_1, i2sPins, sampleSource3);
        delay(4600); // sound quality decent with 4000
        first_run = 0;
      } else
      if(second_run == 1){
        output->start(I2S_NUM_1, i2sPins, sampleSource2);
        delay(4600); // sound quality decent with 4000
        second_run = 0;
      }else
      if(third_run == 1){
        output->start(I2S_NUM_1, i2sPins, sampleSource1);
        delay(4600); // sound quality decent with 4000
        third_run = 0;
      }
    }

    }else{

     
      min1 = min_now%10;
      min2 = min_now/10;
      hour1 = hour_now%10;
      hour2 = hour_now/10;

      numbercheck(0, hour2);
      delay(2);

      numbercheck(1, hour1);
      delay(2);

      numbercheck(2, min2);
      delay(2);

      numbercheck(3, min1);
      delay(1); 
}
 if(minute_plus){
        min_now++;
        minute_plus = 0;
        if(min_now == 60){
          hour_now++;
          min_now = 0;
        }
        if(hour_now == 25){
          hour_now = 0;
        }
      }
        min1 = min_now%10;
      min2 = min_now/10;
      hour1 = hour_now%10;
      hour2 = hour_now/10;

}