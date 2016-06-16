#include <SoftwareSerial.h>
#include "Adafruit_Soundboard.h"
#include <avr/sleep.h>

#define SHOW_SERIAL_MONITOR 1

// Choose any two pins that can be used with SoftwareSerial to RX & TX
#define SFX_TX 5
#define SFX_RX 6
// Connect to the RST pin on the Sound Board
#define SFX_RST 4

#define LED_PIN1 9
#define LED_PIN2 10

#define LIGHT_TIME 10

SoftwareSerial ss = SoftwareSerial(SFX_TX, SFX_RX);
Adafruit_Soundboard sfx = Adafruit_Soundboard(&ss, NULL, SFX_RST);

int mode = 0;
uint8_t track = 0;

int button1_function = 1;
bool playing = false;

void button1_ISR()
{
    noInterrupts();
    delayMicroseconds(10000);
    interrupts();
#ifdef SHOW_SERIAL_MONITOR
    Serial.println("BUTTON 1 ISR");
#endif
    if(button1_function == 1) 
    {
        if(mode == 0) // All stop
        {
            track = 0;
#ifdef SHOW_SERIAL_MONITOR
            Serial.println("Mode 1");
#endif
            digitalWrite(LED_PIN1,HIGH);
            mode = 1; //Play music + Light
        }
        else if(mode == 1)
        {
            if(playing) sfx.stop();
            playing = false;
#ifdef SHOW_SERIAL_MONITOR
            Serial.println("Mode 2");        
#endif
            mode = 2; //Light Only
        }
        else if(mode == 2)
        {
            digitalWrite(LED_PIN1,LOW);
#ifdef SHOW_SERIAL_MONITOR
            Serial.println("Mode 0");
#endif
            mode = 0; //All stop
        }
    }
    else
    {
        button1_function = 1;
        if(playing) sfx.stop();
        playing = false;
        track = 0;
        digitalWrite(LED_PIN1,HIGH);
        digitalWrite(LED_PIN2,LOW);
#ifdef SHOW_SERIAL_MONITOR
        Serial.println("Another Button Press Mode 1");
#endif
        mode = 0; //Play music + Light
    }
}

void button2_ISR()
{
    noInterrupts();
    delayMicroseconds(10000);
    interrupts();
#ifdef SHOW_SERIAL_MONITOR
    Serial.println("BUTTON 2 ISR");
#endif
    if(button1_function == 0)
    {
        if(mode == 0) // All stop
        {
            track = 1;
#ifdef SHOW_SERIAL_MONITOR
            Serial.println("Mode 1");
#endif
            digitalWrite(LED_PIN2,HIGH);
            mode = 1; //Play music + Light
        }
        else if(mode == 1)
        {
            if(playing) sfx.stop();
            playing = false;
#ifdef SHOW_SERIAL_MONITOR
            Serial.println("Mode 2");
#endif
            mode = 2; //Light Only
        }
        else if(mode == 2)
        {
            digitalWrite(LED_PIN2,LOW);
#ifdef SHOW_SERIAL_MONITOR
            Serial.println("Mode 0");
#endif
            mode = 0; //All stop
        }
    }
    else
    {
        button1_function = 0;
        if(playing) sfx.stop();
        playing = false;
        track = 1;
        digitalWrite(LED_PIN2,HIGH);
        digitalWrite(LED_PIN1,LOW);
#ifdef SHOW_SERIAL_MONITOR
        Serial.println("Another Button Press Mode 1");
#endif
        mode = 0; //Play music + Light
    }
}

void sleepNow()         // here we put the arduino to sleep
{ 
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);   // sleep mode is set here
    sleep_enable();          // enables the sleep bit in the mcucr register
    // so sleep is possible. just a safety pin 
    sleep_mode();            // here the device is actually put to sleep!!
    // THE PROGRAM CONTINUES FROM HERE AFTER WAKING UP
    sleep_disable();         // first thing after waking from sleep:
}

void setup() {
    pinMode(2, INPUT);
    pinMode(3, INPUT);
    pinMode(LED_PIN1, OUTPUT);
    pinMode(LED_PIN2, OUTPUT);
    digitalWrite(LED_PIN1,LOW);
    digitalWrite(LED_PIN2,LOW);
    digitalWrite(2, HIGH);
    digitalWrite(3, HIGH);
#ifdef SHOW_SERIAL_MONITOR
    Serial.begin(9600);
    Serial.println("Adafruit Sound Board!");
#endif
    // softwareserial at 9600 baud
    ss.begin(9600);
    // can also do Serial1.begin(9600)

    sfx.reset();
    delay(1000);
//        Serial.println("Not found");
//        while (1);
//}
//    else 
//        Serial.println("SFX board found");
    attachInterrupt(0, button1_ISR, FALLING);
    attachInterrupt(1, button2_ISR, FALLING);
}


void loop() {
start:
    if(mode == 0)
    {
#ifdef SHOW_SERIAL_MONITOR
        Serial.println("Now Sleep");
#endif
        sleepNow();
    }
    if(mode == 1)
    {
        sfx.playTrack(track);
        playing = true;
        for(unsigned int i=0; i<LIGHT_TIME*60; i++)
        {
            if(mode == 0)
                goto start;
            delay(1000);
        }
        digitalWrite(LED_PIN1,LOW);
        digitalWrite(LED_PIN2,LOW);
        mode = 0;
#ifdef SHOW_SERIAL_MONITOR
        Serial.println("From Loop Mode 0");
#endif
    }
}


